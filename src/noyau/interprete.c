/* interprete.c — interpréteur : cycle d'exécution, opérations, contrôle
   EXI: EXI-0011, EXI-0022 à EXI-0030, EXI-0037
   VM: VM-2205, VM-2206, VM-3101, VM-3102
   IV: IV-IF-0003, ALG-1102
   IMP: IMP-0214 (interface), IMP-0215 (copie de la séquence), IMP-0216
        (arithmétique ; chemin exact IMP-0244 ; format étendu), IMP-0217 (cibles calculées), IMP-0218 (UN, ZER),
        IMP-0219 (éléments des étapes suivantes), IMP-0223 (horloge),
        IMP-0224 (TEM, DAT), IMP-0225 (ALE), IMP-0280 (réels de 23 bits du
        T1600) ; Bibliothécaire et fichiers
        (EXECUTER, point B5 ; GARER, CHARGER, SUPPRIMER, IMP-0253 ; perforateur
        et ordres à la télétype, IMP-0274, IMP-0275)
   Tests: tests/composants/composants_interprete.c
   Spécification : chapitre Interpréteur ; État d'exécution §7.2 ; fiche de
   codage RCI-M §2 à §7. Segments "interprete" et "interp_es" (IMP-0226). */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "interprete.h"
#include "programme.h"
#include "symboles.h"
#include "donnees.h"
#include "bibliotheque.h"
#include "fichiers.h"
#include "ruban.h"
#include "rcim.h"
#include "../interfaces/interface_console.h"
#include "../interfaces/interface_horloge.h"
#include "../interfaces/types_communs.h"
#include "../interfaces/profil.h"

segment "interprete";

typedef struct {
    PileEvaluation      *p;
    PositionExecution   *pos;
    unsigned char        seq[RCIM_LONGUEUR_MAX_SEQUENCE];   /* IMP-0215 */
    unsigned int         lg;
    unsigned char        ligne;      /* ligne chargée ; 0 : machine de bureau */
    HandleObjet          bloc;       /* bloc de la ligne, pour les chaînes en ligne */
    int                  bureau;
    int                  transition; /* la position a été portée par un branchement */
    FinExecution         fin;
    int                  fini;
} Cycle;

/* AFFICHER (IMP-0221) : description en cours et valeurs qu'elle couvre encore.
   État de l'interpréteur, qui survit aux suspensions ; rangé dans le cadre de
   l'appelant pendant un appel de procédure (IMP-0146). */
static DescriptionAffichage aff;

#define G_NOMBRE   0
#define G_CHAINE   1
#define G_INCONNU  2   /* variable non définie ou paramètre sans valeur */

/* --- pile -------------------------------------------------------------------- */

/* Résultat d'une puissance, d'une fonction, de TEM ou d'ALE : EX 44 au-delà
   du plus grand réel (IMP-0216) ; arrondi à 23 bits sous le profil T1600
   (IMP-0280). */
static NatureErreur pousser_nombre(Cycle *c, double v)
{
    float f;
    NatureErreur n;

    if ((n = donnees_ranger_reel(v, &f)) != ERR_AUCUNE) return n;
    return pile_empiler_nombre_exact(c->p, &f);
}

/* Entier exact empilé sans conversion SANE (IMP-0244). */
static NatureErreur pousser_entier(Cycle *c, long v)
{
    float f;
    donnees_nombre_entier(v, &f);
    return pile_empiler_nombre_exact(c->p, &f);
}

/* Constantes immédiates (RCI-M, -28 à 28), converties une fois par
   donnees_nombre_entier, donc aux mêmes bits (audit du code, AP-10). */
static float immediats[RCIM_VAL_IMMEDIAT_MAX - RCIM_VAL_IMMEDIAT_MIN + 1];
static int immediats_prets = 0;

static NatureErreur pousser_immediat(Cycle *c, int v)
{
    int k;
    if (!immediats_prets) {
        for (k = RCIM_VAL_IMMEDIAT_MIN; k <= RCIM_VAL_IMMEDIAT_MAX; k++)
            donnees_nombre_entier((long) k, &immediats[k - RCIM_VAL_IMMEDIAT_MIN]);
        immediats_prets = 1;
    }
    return pile_empiler_nombre_exact(c->p, &immediats[v - RCIM_VAL_IMMEDIAT_MIN]);
}

static NatureErreur pousser_booleen(Cycle *c, int b)
{
    ElementPile e;
    e.type = PILE_BOOLEEN;
    e.contenu.booleen = (unsigned char) (b != 0);
    return pile_empiler(c->p, &e);
}

static NatureErreur pousser_chaine(Cycle *c, const ChaineSurPile *s)
{
    ElementPile e;
    NatureErreur n;
    e.type = PILE_CHAINE;
    e.contenu.chaine = *s;
    n = pile_empiler(c->p, &e);
    if (n != ERR_AUCUNE && s->transitoire) objets_liberer(s->objet);
    return n;
}

static NatureErreur nombre(Cycle *c, unsigned int prof, UsageLecture u, float *v)
{
    return pile_valeur_numerique(c->p, prof, u, v);
}

static int genre(Cycle *c, unsigned int prof)
{
    ElementPile e;
    EntreeEnvironnement t;

    if (!pile_lire(c->p, prof, &e)) return G_NOMBRE;
    if (e.type == PILE_CHAINE) return G_CHAINE;
    if (e.type != PILE_REFERENCE || e.contenu.reference.indexee != REF_SIMPLE) return G_NOMBRE;
    if (contexte_entree(e.contenu.reference.cellule, &t) != ERR_AUCUNE) return G_NOMBRE;
    if (t.etat == ID_CHAINE) return G_CHAINE;
    if (t.etat == ID_NON_DEFINI || t.etat == ID_PARAMETRE_SANS_VALEUR) return G_INCONNU;
    return G_NOMBRE;
}

static NatureErreur chaine(Cycle *c, unsigned int prof, UsageLecture u, ChaineSurPile *s)
{
    ElementPile e;
    if (!pile_lire(c->p, prof, &e)) return ERR_E_INTERNE;
    if (e.type == PILE_CHAINE) { *s = e.contenu.chaine; return ERR_AUCUNE; }
    if (e.type == PILE_REFERENCE && e.contenu.reference.indexee == REF_SIMPLE)
        return contexte_lire_chaine(e.contenu.reference.cellule, u, s);
    return ERR_E_OPERANDE_NON_CHAINE;
}

/* Deuxième argument numérique des fonctions SCH, GRL, PTR, CNB (EX 18). */
static NatureErreur argument_2(Cycle *c, unsigned int prof, float *v)
{
    if (genre(c, prof) == G_CHAINE) return ERR_E_ARGUMENT_2_NON_NUMERIQUE;
    return nombre(c, prof, USAGE_CALCUL, v);
}

/* Écrit v dans la référence à la profondeur donnée (paramètres δ, γ). */
/* Le nombre passe par adresse : aucune conversion au format étendu
   (IMP-0244 ; audit du code, AP-07). */
static NatureErreur ecrire_reference(Cycle *c, unsigned int prof, const float *v)
{
    ElementPile e;
    EntreeEnvironnement t;
    NatureErreur n;

    if (!pile_lire(c->p, prof, &e) || e.type != PILE_REFERENCE) return ERR_E_INTERNE;
    if (e.contenu.reference.indexee == REF_ELEMENT) {
        if ((n = contexte_entree(e.contenu.reference.cellule, &t)) != ERR_AUCUNE) return n;
        if (t.etat != ID_TABLEAU) return ERR_E_INDICES_SANS_TABLEAU;
        return donnees_ecrire_element_exact(t.contenu.objet, e.contenu.reference.decalage, v);
    }
    if (e.contenu.reference.indexee != REF_SIMPLE) return ERR_E_INTERNE;
    return contexte_affecter_nombre_exact(e.contenu.reference.cellule, v);
}

/* --- arithmétique (IMP-0216) --------------------------------------------- */

static int entier(double x)
{
    return floor(x) == x;
}

static NatureErreur binaire(Cycle *c, TagRCIM t)
{
    float fa;
    float fb;
    float fr;
    double a;
    double b;
    double r = 0.0;
    NatureErreur n;

    if ((n = nombre(c, 1, USAGE_CALCUL, &fa)) != ERR_AUCUNE) return n;
    if ((n = nombre(c, 0, USAGE_CALCUL, &fb)) != ERR_AUCUNE) return n;
    if (t == RCIM_OP_ADD || t == RCIM_OP_SUB || t == RCIM_OP_MUL) {        /* IMP-0244 */
        if (donnees_operation_entiere(t == RCIM_OP_ADD ? '+' : t == RCIM_OP_SUB ? '-' : '*',
                                      &fa, &fb, &fr)) {
            pile_consommer(c->p, 2);
            return pile_empiler_nombre_exact(c->p, &fr);
        }
    }
    if (t != RCIM_OP_PUIS) {                   /* format étendu (AP-04) */
        n = donnees_operation_etendue(t == RCIM_OP_ADD ? '+' : t == RCIM_OP_SUB ? '-'
                                      : t == RCIM_OP_MUL ? '*' : '/', &fa, &fb, &fr);
        if (n != ERR_AUCUNE) return n;
        pile_consommer(c->p, 2);
        return pile_empiler_nombre_exact(c->p, &fr);
    }
    a = fa;
    b = fb;
    if (a == 0.0 && b == 0.0) r = 1.0;         /* constaté sur LSE 8086 */
    else if (a == 0.0 && b < 0.0) return ERR_E_ZERO_PUISSANCE_NEGATIVE;
    else if (a < 0.0 && !entier(b)) return ERR_E_PUISSANCE_NON_ENTIERE;
    else r = pow(a, b);
    pile_consommer(c->p, 2);
    return pousser_nombre(c, r);
}

static NatureErreur fonction_1(Cycle *c, TagRCIM t)
{
    float f;
    double x;
    double r;
    NatureErreur n;

    if ((n = nombre(c, 0, USAGE_CALCUL, &f)) != ERR_AUCUNE) return n;
    if (t == RCIM_OP_NEG) {                                   /* IMP-0244 : bit de signe */
        donnees_opposer(&f, &f);
        pile_consommer(c->p, 1);
        return pile_empiler_nombre_exact(c->p, &f);
    }
    x = f;
    switch (t) {
    case RCIM_OP_NEG: r = -x; break;
    case RCIM_OP_RAC:
        if (x < 0.0) return ERR_E_RAC_NEGATIF;
        r = sqrt(x);
        break;
    case RCIM_OP_ABS: r = fabs(x); break;
    case RCIM_OP_EXP: r = exp(x); break;
    case RCIM_OP_LGN:
        if (x <= 0.0) return ERR_E_LGN_NEGATIF;
        r = log(x);
        break;
    case RCIM_OP_SIN: r = sin(x); break;
    case RCIM_OP_COS: r = cos(x); break;
    case RCIM_OP_ATG: r = atan(x); break;
    case RCIM_OP_ENT: r = floor(x); break;
    case RCIM_OP_SH:  r = sinh(x); break;
    case RCIM_OP_CH:  r = cosh(x); break;
    case RCIM_OP_TH:  r = tanh(x); break;
    default:          r = x > 0.0 ? 1.0 : x < 0.0 ? -1.0 : 0.0; break;   /* SGN */
    }
    pile_consommer(c->p, 1);
    return pousser_nombre(c, r);
}

/* --- relations et logique ---------------------------------------------- */

static NatureErreur relation(Cycle *c, TagRCIM t)
{
    int ga = genre(c, 1);
    int gb = genre(c, 0);
    int cmp;
    float fa;
    float fb;
    ChaineSurPile sa;
    ChaineSurPile sb;
    NatureErreur n;

    if (ga == G_CHAINE || gb == G_CHAINE) {
        if (ga == G_NOMBRE || gb == G_NOMBRE) return ERR_E_COMPARAISON_TYPES;
        if ((n = chaine(c, 1, USAGE_COMPARAISON, &sa)) != ERR_AUCUNE) return n;
        if ((n = chaine(c, 0, USAGE_COMPARAISON, &sb)) != ERR_AUCUNE) return n;
        cmp = donnees_comparer(&sa, &sb);
    } else {
        if ((n = nombre(c, 1, USAGE_COMPARAISON, &fa)) != ERR_AUCUNE) return n;
        if ((n = nombre(c, 0, USAGE_COMPARAISON, &fb)) != ERR_AUCUNE) return n;
        cmp = donnees_comparer_nombres(&fa, &fb);              /* IMP-0244 */
    }
    pile_consommer(c->p, 2);
    switch (t) {
    case RCIM_OP_EGAL:     return pousser_booleen(c, cmp == 0);
    case RCIM_OP_INF:      return pousser_booleen(c, cmp < 0);
    case RCIM_OP_INF_EGAL: return pousser_booleen(c, cmp <= 0);
    case RCIM_OP_DIFF:     return pousser_booleen(c, cmp != 0);
    case RCIM_OP_SUP:      return pousser_booleen(c, cmp > 0);
    default:               return pousser_booleen(c, cmp >= 0);
    }
}

static NatureErreur logique(Cycle *c, TagRCIM t)
{
    ElementPile a;
    ElementPile b;
    if (!pile_lire(c->p, 1, &a) || !pile_lire(c->p, 0, &b)
        || a.type != PILE_BOOLEEN || b.type != PILE_BOOLEEN)
        return ERR_E_INTERNE;
    pile_consommer(c->p, 2);
    if (t == RCIM_OP_ET) return pousser_booleen(c, a.contenu.booleen && b.contenu.booleen);
    return pousser_booleen(c, a.contenu.booleen || b.contenu.booleen);
}

static NatureErreur booleen(Cycle *c, int *b)
{
    ElementPile e;
    if (!pile_lire(c->p, 0, &e) || e.type != PILE_BOOLEEN) return ERR_E_INTERNE;
    *b = e.contenu.booleen;
    pile_consommer(c->p, 1);
    return ERR_AUCUNE;
}

/* --- affectation (§3.3) ------------------------------------------------- */

static NatureErreur affecter(Cycle *c)
{
    ElementPile cible;
    EntreeEnvironnement t;
    ChaineSurPile s;
    HandleObjet h;
    float v;
    NatureErreur n;
    int g = genre(c, 0);

    if (!pile_lire(c->p, 1, &cible) || cible.type != PILE_REFERENCE) return ERR_E_INTERNE;
    if (cible.contenu.reference.indexee == REF_SIMPLE && g == G_CHAINE) {
        if ((n = contexte_entree(cible.contenu.reference.cellule, &t)) != ERR_AUCUNE) return n;
        if (t.etat != ID_CHAINE) return ERR_E_CHAINE_NON_DECLAREE;          /* LSE15 §4-3 */
        if ((n = chaine(c, 0, USAGE_AFFECTATION, &s)) != ERR_AUCUNE) return n;
        if ((n = donnees_chaine_vers_objet(&s, &h)) != ERR_AUCUNE) return n;
        if ((n = contexte_affecter_chaine(cible.contenu.reference.cellule, h)) != ERR_AUCUNE) {
            if (!s.transitoire) objets_liberer(h);
            return n;
        }
        if (s.transitoire)       /* l'objet appartient désormais à la variable */
            c->p->elements[c->p->sommet - 1].contenu.chaine.transitoire = 0;
        pile_consommer(c->p, 2);
        return ERR_AUCUNE;
    }
    if (g == G_CHAINE) return ERR_E_CHAINE_NON_DECLAREE;
    if ((n = nombre(c, 0, USAGE_AFFECTATION, &v)) != ERR_AUCUNE) return n;
    if ((n = ecrire_reference(c, 1, &v)) != ERR_AUCUNE) return n;
    pile_consommer(c->p, 2);
    return ERR_AUCUNE;
}

/* --- fonctions sur les chaînes (Données IMP-0212) ----------------------------- */

static NatureErreur fonction_chaine(Cycle *c, TagRCIM t, unsigned int k)
{
    ChaineSurPile a;
    ChaineSurPile g;
    ChaineSurPile r;
    float beta = 1.0f;
    float x = 0.0f;
    float res = 0.0f;
    float delta = 0.0f;
    int resultat_chaine = 0;
    NatureErreur n;

    switch (t) {
    case RCIM_OP_LGR:
        if ((n = chaine(c, 0, USAGE_CALCUL, &a)) != ERR_AUCUNE) return n;
        res = (float) a.longueur;
        break;
    case RCIM_OP_CCA:
    case RCIM_OP_EQC:
        if ((n = nombre(c, 0, USAGE_CALCUL, &x)) != ERR_AUCUNE) return n;
        n = t == RCIM_OP_CCA ? donnees_cca(x, &r) : donnees_eqc(x, &r);
        if (n != ERR_AUCUNE) return n;
        resultat_chaine = 1;
        break;
    case RCIM_OP_POS:
        if ((n = chaine(c, 2, USAGE_CALCUL, &a)) != ERR_AUCUNE) return n;
        if ((n = nombre(c, 1, USAGE_CALCUL, &beta)) != ERR_AUCUNE) return n;
        if ((n = chaine(c, 0, USAGE_CALCUL, &g)) != ERR_AUCUNE) return n;
        if ((n = donnees_pos(&a, beta, &g, &res)) != ERR_AUCUNE) return n;
        break;
    case RCIM_OP_EQN:
        if ((n = chaine(c, k - 1, USAGE_CALCUL, &a)) != ERR_AUCUNE) return n;
        if (k == 2 && (n = nombre(c, 0, USAGE_CALCUL, &beta)) != ERR_AUCUNE) return n;
        if ((n = donnees_eqn(&a, beta, &res)) != ERR_AUCUNE) return n;
        break;
    case RCIM_OP_SKP:
        if ((n = chaine(c, k - 1, USAGE_CALCUL, &a)) != ERR_AUCUNE) return n;
        if ((n = nombre(c, k - 2, USAGE_CALCUL, &beta)) != ERR_AUCUNE) return n;
        if (k == 3 && (n = chaine(c, 0, USAGE_CALCUL, &g)) != ERR_AUCUNE) return n;
        if ((n = donnees_skp(&a, beta, k == 3 ? &g : NULL, &res)) != ERR_AUCUNE) return n;
        break;
    case RCIM_OP_CNB:
        if ((n = chaine(c, k - 1, USAGE_CALCUL, &a)) != ERR_AUCUNE) return n;
        if ((n = argument_2(c, k - 2, &beta)) != ERR_AUCUNE) return n;
        if ((n = donnees_cnb(&a, beta, &res, &delta)) != ERR_AUCUNE) return n;
        if (k == 3 && (n = ecrire_reference(c, 0, &delta)) != ERR_AUCUNE) return n;
        break;
    case RCIM_OP_GRL:
        if ((n = chaine(c, k - 1, USAGE_CALCUL, &a)) != ERR_AUCUNE) return n;
        if ((n = argument_2(c, k - 2, &beta)) != ERR_AUCUNE) return n;
        if ((n = donnees_grl(&a, beta, &r, &delta)) != ERR_AUCUNE) return n;
        if (k == 3 && (n = ecrire_reference(c, 0, &delta)) != ERR_AUCUNE) { objets_liberer(r.objet); return n; }
        resultat_chaine = 1;
        break;
    default: {                                     /* SCH (3 ou 4), PTR (2 ou 3) */
        int avec_gamma = t == RCIM_OP_SCH || k == 3;
        unsigned int pa = t == RCIM_OP_SCH ? k - 1 : k - 1;
        int arret = 0;
        if ((n = chaine(c, pa, USAGE_CALCUL, &a)) != ERR_AUCUNE) return n;
        if ((n = argument_2(c, pa - 1, &beta)) != ERR_AUCUNE) return n;
        if (avec_gamma) {
            unsigned int pg = pa - 2;
            int gg = genre(c, pg);
            if (gg == G_CHAINE) {
                if ((n = chaine(c, pg, USAGE_CALCUL, &g)) != ERR_AUCUNE) return n;
                arret = 1;
            } else if ((n = nombre(c, pg, USAGE_CALCUL, &x)) != ERR_AUCUNE)
                return n == ERR_E_CALCUL_NON_NUMERIQUE ? ERR_E_ARGUMENT_3_ERRONE : n;
        }
        if (t == RCIM_OP_PTR) {
            if ((n = donnees_ptr(&a, beta, avec_gamma, arret ? &g : NULL, x, &res)) != ERR_AUCUNE) return n;
        } else {
            if ((n = donnees_sch(&a, beta, arret ? &g : NULL, x, &r, &delta)) != ERR_AUCUNE) return n;
            if (k == 4 && (n = ecrire_reference(c, 0, &delta)) != ERR_AUCUNE) { objets_liberer(r.objet); return n; }
            resultat_chaine = 1;
        }
        break;
    }
    }
    pile_consommer(c->p, k);
    if (resultat_chaine) return pousser_chaine(c, &r);
    return pousser_nombre(c, res);
}

/* Second segment de l'interpréteur (IMP-0197, IMP-0226) : le code dépasserait
   sinon les 64 Kio d'un segment. Pile, arithmétique, relations, affectation
   et fonctions sur les chaînes restent dans "interprete" ; entrée-sortie,
   horloge, distribution des éléments et cycle sont dans "interp_es". */
segment "interp_es";

/* --- AFFICHER (IMP-0221) -------------------------------------------------- */

/* Vrai si la sortie est en début de ligne : après un retour chariot ou une
   saisie terminée par RETURN ; un interligne seul ne change pas la colonne
   (console de type télétype, LSE15 §6-2.2). */
static int en_debut_de_ligne = 1;

void interprete_debut_de_ligne(int vrai)
{
    en_debut_de_ligne = vrai != 0;
}

int interprete_en_debut_de_ligne(void)
{
    return en_debut_de_ligne;
}

/* Toute la sortie d'un programme passe par ces trois fonctions : les ordres à
   la télétype et le perforateur sont traités par ruban_sortie (IMP-0274,
   IMP-0275). */
static void sortie_ecrire(const char *t, unsigned int n)
{
    if (n == 0) return;
    if (ruban_sortie(t, n) > 0) en_debut_de_ligne = 0;
}

static void sortie_retour_chariot(void)
{
    console_retour_chariot();
    ruban_sortie_commande(0x0D);
    en_debut_de_ligne = 1;
}

static void sortie_interligne(void)
{
    console_interligne();
    ruban_sortie_commande(0x0A);
}

static void nouvelle_ligne(void)
{
    sortie_retour_chariot();
    sortie_interligne();
}

/* Facteur de répétition d'une spécification B0 à B4 (LSE15 §6-2.6). */
static NatureErreur facteur(Cycle *c, unsigned char f, long *n)
{
    float v;
    double r;
    NatureErreur e;
    if (!(f & RCIM_FACTEUR_VARIABLE)) { *n = f; return ERR_AUCUNE; }
    if ((e = nombre(c, 0, USAGE_CALCUL, &v)) != ERR_AUCUNE) return e;
    r = floor((double) v + 0.5);
    if (r < 0.0) return ERR_E_FACTEUR_NEGATIF;
    *n = r > 32767.0 ? 32767L : (long) r;
    pile_consommer(c->p, 1);
    return ERR_AUCUNE;
}

static void ecrire_chaine(const ChaineSurPile *s)
{
    char t[256];
    unsigned int debut = 0;
    unsigned int n;
    while ((n = donnees_texte(s, debut, (unsigned char *) t, sizeof t)) > 0) {
        sortie_ecrire(t, n);
        debut += n;
    }
}

static void ecrire_nombre(float v, unsigned char type, unsigned char e, unsigned char d)
{
    char t[300];
    unsigned int n = donnees_ecrire_nombre(v, type, e, d, t);
    sortie_ecrire(t, n);
}

/* Spécification de la valeur suivante : description en cours, sinon U. */
static void specification(Cycle *c, unsigned char *type, unsigned char *e, unsigned char *d)
{
    if (aff.reste > 0) {
        *type = aff.type; *e = aff.e; *d = aff.d;
        aff.reste--;
    } else {
        *type = RCIM_DESC_U; *e = 0; *d = 0;
    }
}

static NatureErreur afficher_valeur(Cycle *c)
{
    ElementPile x;
    EntreeEnvironnement t;
    ChaineSurPile s;
    unsigned char type;
    unsigned char e;
    unsigned char d;
    unsigned char dims;
    unsigned int d1;
    unsigned int d2;
    unsigned int i;
    unsigned int j;
    float v;
    NatureErreur n;

    if (!pile_lire(c->p, 0, &x)) return ERR_E_INTERNE;
    if (x.type == PILE_REFERENCE && x.contenu.reference.indexee == REF_SIMPLE
        && contexte_entree(x.contenu.reference.cellule, &t) == ERR_AUCUNE && t.etat == ID_TABLEAU) {
        specification(c, &type, &e, &d);                       /* tableau entier (LSE15 §6-2.5) */
        if ((n = donnees_tableau_bornes(t.contenu.objet, &dims, &d1, &d2)) != ERR_AUCUNE) return n;
        if (!en_debut_de_ligne) nouvelle_ligne();              /* constaté sur LSE 8086 */
        for (i = 0; i < d1; i++) {
            for (j = 0; j < d2; j++) {
                if ((n = donnees_lire_element(t.contenu.objet, i * d2 + j, &v)) != ERR_AUCUNE) return n;
                ecrire_nombre(v, type, e, d);
            }
            nouvelle_ligne();
        }
    } else if (genre(c, 0) == G_CHAINE) {
        specification(c, &type, &e, &d);
        if (type != RCIM_DESC_U) return ERR_E_VALEUR_FORMAT;   /* point G5 */
        if ((n = chaine(c, 0, USAGE_AFFECTATION, &s)) != ERR_AUCUNE) return n;
        ecrire_chaine(&s);
    } else {
        if ((n = nombre(c, 0, USAGE_AFFECTATION, &v)) != ERR_AUCUNE) return n;
        specification(c, &type, &e, &d);
        ecrire_nombre(v, type, e, d);
    }
    pile_consommer(c->p, 1);
    return ERR_AUCUNE;
}

static NatureErreur afficher_element(Cycle *c, const unsigned char *o)
{
    long k;
    long i;
    NatureErreur n;

    switch (o[0]) {
    case RCIM_OP_AFF_TETE_SANS_FORMAT:
        aff.reste = 0;
        nouvelle_ligne();                                      /* LSE15 §6-2.4 */
        return ERR_AUCUNE;
    case RCIM_OP_AFF_TETE_FORMAT:
        aff.reste = 0;
        return ERR_AUCUNE;
    case RCIM_OP_AFF_DESCRIPTION: {
        unsigned char b = o[1];
        unsigned int q = 2;
        aff.type = (unsigned char) (b >> 6);
        if ((b & 0x3F) == 0x3F) { aff.reste = o[2]; q = 3; }
        else aff.reste = (unsigned char) ((b & 0x3F) + 1);
        aff.e = aff.type == RCIM_DESC_U ? 0 : o[q];
        aff.d = aff.type == RCIM_DESC_U ? 0 : o[q + 1];
        return ERR_AUCUNE;
    }
    case RCIM_OP_AFF_VALEUR:
        return afficher_valeur(c);
    default:                                                   /* B0 à B4 */
        if ((n = facteur(c, o[1], &k)) != ERR_AUCUNE) return n;
        for (i = 0; i < k; i++) {
            switch (o[0]) {
            case RCIM_OP_AFF_LIBELLE: sortie_ecrire((const char *) o + 3, o[2]); break;
            case RCIM_OP_AFF_LIGNE:   nouvelle_ligne(); break;
            case RCIM_OP_AFF_ESPACE:  sortie_ecrire(" ", 1); break;
            case RCIM_OP_AFF_RETOUR:  sortie_retour_chariot(); break;
            default:                  sortie_interligne(); break;    /* L */
            }
        }
        return ERR_AUCUNE;
    }
}

static NatureErreur afficher(Cycle *c, const unsigned char *o)
{
    NatureErreur n = afficher_element(c, o);
    if (n == ERR_AUCUNE && ruban_sortie_en_echec()) return ERR_E_FICHIER_PLEIN;   /* IMP-0274 */
    return n;
}

/* --- LIRE (IMP-0222 ; Moniteur IMP-0237, IMP-0238) ------------------------------ */

/* État de la lecture en cours, conservé entre deux appels : la ligne est
   fournie par le moniteur après une fin d'exécution FIN_LECTURE. */
static char          saisie[256];
static unsigned int  saisie_lg = 0;
static unsigned int  saisie_pos = 0;
static unsigned char saisie_fin = SAISIE_RETURN;
static int           saisie_disponible = 0;
static unsigned int  rang_tableau = 0;      /* éléments déjà lus d'un tableau entier */

void interprete_fournir_saisie(const char *ligne, unsigned int n, unsigned char touche_fin)
{
    if (n > sizeof saisie) n = sizeof saisie;
    memcpy(saisie, ligne, n);
    saisie_lg = n;
    saisie_pos = 0;
    saisie_fin = touche_fin;
    saisie_disponible = 1;
    en_debut_de_ligne = touche_fin == SAISIE_RETURN;          /* écho de RETURN seul */
}

static void lecture_oublier(void)
{
    saisie_lg = 0;
    saisie_pos = 0;
    saisie_disponible = 0;
    rang_tableau = 0;
}

/* Ligne de machine de bureau (IMP-0222, IMP-0233) : elle part d'un état de
   lecture vierge ; celui du programme est mis de côté, puis rétabli. Les
   lignes de bureau ne s'imbriquent pas : une seule zone suffit. */
static char          garde_saisie[256];
static unsigned int  garde_lg;
static unsigned int  garde_pos;
static unsigned char garde_fin;
static int           garde_disponible;
static unsigned int  garde_rang;

void interprete_lecture_mettre_de_cote(void)
{
    memcpy(garde_saisie, saisie, saisie_lg);
    garde_lg = saisie_lg;
    garde_pos = saisie_pos;
    garde_fin = saisie_fin;
    garde_disponible = saisie_disponible;
    garde_rang = rang_tableau;
    lecture_oublier();
}

void interprete_lecture_retablir(void)
{
    memcpy(saisie, garde_saisie, garde_lg);
    saisie_lg = garde_lg;
    saisie_pos = garde_pos;
    saisie_fin = garde_fin;
    saisie_disponible = garde_disponible;
    rang_tableau = garde_rang;
}

/* L'élément en p0 attend une ligne : fin FIN_LECTURE, reprise sur ce même élément. */
static NatureErreur attendre_ligne(Cycle *c, unsigned int p0)
{
    saisie_disponible = 0;
    c->fin = FIN_LECTURE;
    c->fini = 1;
    c->pos->decalage = p0;
    return ERR_AUCUNE;
}

static int chiffre(char x)
{
    return x >= '0' && x <= '9';
}

/* 1 : un nombre lu dans *v ; 0 : une ligne est nécessaire. Réponse mal
   formée : bip, « ? », la ligne est abandonnée (point F5, LSE 8086). */
static int saisir_nombre(float *v)
{
    unsigned int i;
    unsigned int debut;
    char tampon[80];
    unsigned int n;
    double x;
    int ok;

    if (!saisie_disponible) return 0;
    while (saisie_pos < saisie_lg && saisie[saisie_pos] == ' ') saisie_pos++;
    if (saisie_pos >= saisie_lg) return 0;
    debut = saisie_pos;
    i = debut;
    if (saisie[i] == '+' || saisie[i] == '-') i++;
    ok = i < saisie_lg && chiffre(saisie[i]);
    while (i < saisie_lg && chiffre(saisie[i])) i++;
    if (ok && i < saisie_lg && saisie[i] == '.') {
        i++;
        while (i < saisie_lg && chiffre(saisie[i])) i++;
    }
    if (ok && i < saisie_lg && saisie[i] == 'E') {
        i++;
        if (i < saisie_lg && (saisie[i] == '+' || saisie[i] == '-')) i++;
        ok = i < saisie_lg && chiffre(saisie[i]);
        while (i < saisie_lg && chiffre(saisie[i])) i++;
    }
    if (ok && i < saisie_lg && saisie[i] != ' ') ok = 0;
    if (!ok) {
        console_bip();
        sortie_ecrire(" ? ", 3);
        return 0;
    }
    n = i - debut;
    if (n >= sizeof tampon) n = sizeof tampon - 1;
    memcpy(tampon, saisie + debut, n);
    tampon[n] = '\0';
    x = strtod(tampon, NULL);
    donnees_ranger_reel_borne(x, v);       /* plus grand réel au-delà ; IMP-0280 */
    saisie_pos = i < saisie_lg ? i + 1 : i;                   /* l'espace qui termine */
    return 1;
}

/* Variable-chaîne : le reste de la ligne ; terminée par RETURN, la ligne
   apporte en outre CR LF, qui termine aussi la valeur (INRDP75 PG3, point L7). */
static NatureErreur lire_chaine(Cycle *c, unsigned int p0, unsigned int cellule)
{
    unsigned char texte[sizeof saisie + 2];
    unsigned int n;
    ChaineSurPile s;
    HandleObjet h;
    NatureErreur e;

    if (!saisie_disponible || (saisie_pos >= saisie_lg && saisie_pos != 0))
        return attendre_ligne(c, p0);
    n = saisie_lg - saisie_pos;
    memcpy(texte, saisie + saisie_pos, n);
    if (saisie_fin == SAISIE_RETURN) {                         /* point L5 : CR LF */
        texte[n++] = '\r';
        texte[n++] = '\n';
    }
    saisie_pos = saisie_lg;
    saisie_disponible = 0;                       /* une ligne vide ne sert qu'une fois */
    if ((e = donnees_chaine_creer(texte, n, &s)) != ERR_AUCUNE) return e;
    donnees_chaine_vers_objet(&s, &h);
    if ((e = contexte_affecter_chaine(cellule, h)) != ERR_AUCUNE) {
        objets_liberer(h);
        return e;
    }
    pile_consommer(c->p, 1);
    return ERR_AUCUNE;
}

static NatureErreur lire(Cycle *c, unsigned int p0)
{
    ElementPile x;
    EntreeEnvironnement t;
    unsigned char dims;
    unsigned int d1;
    unsigned int d2;
    float v;
    NatureErreur n;

    if (!pile_lire(c->p, 0, &x) || x.type != PILE_REFERENCE) return ERR_E_INTERNE;
    if (x.contenu.reference.indexee == REF_SIMPLE) {
        if ((n = contexte_entree(x.contenu.reference.cellule, &t)) != ERR_AUCUNE) return n;
        if (t.etat == ID_CHAINE)
            return lire_chaine(c, p0, x.contenu.reference.cellule);
        if (t.etat == ID_TABLEAU) {                             /* ligne par ligne */
            if ((n = donnees_tableau_bornes(t.contenu.objet, &dims, &d1, &d2)) != ERR_AUCUNE) return n;
            while (rang_tableau < d1 * d2) {
                if (!saisir_nombre(&v)) return attendre_ligne(c, p0);
                if ((n = donnees_ecrire_element(t.contenu.objet, rang_tableau, v)) != ERR_AUCUNE) return n;
                rang_tableau++;
            }
            rang_tableau = 0;
            pile_consommer(c->p, 1);
            return ERR_AUCUNE;
        }
    }
    if (!saisir_nombre(&v)) return attendre_ligne(c, p0);
    if ((n = ecrire_reference(c, 0, &v)) != ERR_AUCUNE) return n;
    pile_consommer(c->p, 1);
    return ERR_AUCUNE;
}

/* --- horloge et valeurs pseudo-aléatoires (IMP-0224, IMP-0225) ---------------- */

static NatureErreur fonction_tem(Cycle *c)
{
    InstantHorloge h;
    horloge_lire(&h);
    return pousser_nombre(c, (double) h.heure * 3600.0 + (double) h.minute * 60.0 + (double) h.seconde);
}

static void deux_chiffres(unsigned char *t, unsigned int v)
{
    t[0] = (unsigned char) ('0' + v / 10 % 10);
    t[1] = (unsigned char) ('0' + v % 10);
}

/* « JJ/MM/AA HH:MM:SS » (INRDP75 §6-11, LSE15 §4-4.2). */
static NatureErreur fonction_dat(Cycle *c)
{
    InstantHorloge h;
    unsigned char t[17];
    ChaineSurPile s;
    NatureErreur n;

    horloge_lire(&h);
    deux_chiffres(t, h.jour);
    t[2] = '/';
    deux_chiffres(t + 3, h.mois);
    t[5] = '/';
    deux_chiffres(t + 6, h.annee % 100);
    t[8] = ' ';
    deux_chiffres(t + 9, h.heure);
    t[11] = ':';
    deux_chiffres(t + 12, h.minute);
    t[14] = ':';
    deux_chiffres(t + 15, h.seconde);
    if ((n = donnees_chaine_creer(t, 17, &s)) != ERR_AUCUNE) return n;
    return pousser_chaine(c, &s);
}

/* Générateur congruentiel modulo 2^24, de période complète : a = 1 mod 4,
   c impair. Le produit est calculé modulo 2^32, multiple de 2^24. */
#define ALE_MODULE   0x1000000ul
#define ALE_MASQUE   0xFFFFFFul
#define ALE_A        1664525ul
#define ALE_C        (1013904223ul & ALE_MASQUE)

static unsigned long ale_etat = 0;      /* état de ALE(0), propre à la session */
static int           ale_amorce = 0;

static unsigned long ale_suivant(unsigned long x)
{
    x = (ALE_A * x + ALE_C) & ALE_MASQUE;
    if (x == 0) x = (ALE_A * x + ALE_C) & ALE_MASQUE;   /* résultat dans ]0,1[ */
    return x;
}

static NatureErreur fonction_ale(Cycle *c)
{
    float f;
    double e;
    unsigned long x;
    NatureErreur n;

    if ((n = nombre(c, 0, USAGE_CALCUL, &f)) != ERR_AUCUNE) return n;
    e = f;
    if (e < 0.0 || e > 1.0) return ERR_E_ARGUMENT_ALE;
    if (e == 0.0) {
        if (!ale_amorce) {                                     /* amorce par l'horloge */
            InstantHorloge h;
            horloge_lire(&h);
            ale_etat = ((unsigned long) h.jour * 86400ul + (unsigned long) h.heure * 3600ul
                        + (unsigned long) h.minute * 60ul + h.seconde
                        + (unsigned long) h.mois * 2678400ul) & ALE_MASQUE;
            ale_amorce = 1;
        }
        ale_etat = ale_suivant(ale_etat);
        x = ale_etat;
    } else {
        x = ale_suivant((unsigned long) (e * (double) ALE_MODULE) & ALE_MASQUE);
    }
    /* Sous le profil T1600, 23 bits (IMP-0280) : le dernier bit est tronqué,
       et non arrondi, pour que le résultat reste dans ]0,1[. */
    if (PROFIL_EST_T1600(configuration_session.profil) && x >= 0x800000ul) x &= ~1ul;
    pile_consommer(c->p, 1);
    return pousser_nombre(c, (double) x / (double) ALE_MODULE);
}

/* --- éléments ------------------------------------------------------------- */

/* Cible calculée d'un branchement (IMP-0217). */
static NatureErreur cible_calculee(Cycle *c, unsigned char *ligne)
{
    float f;
    double r;
    NatureErreur n;
    if ((n = nombre(c, 0, USAGE_CALCUL, &f)) != ERR_AUCUNE) return n;
    r = floor((double) f + 0.5);
    if (r < 1.0) return ERR_E_LIGNE_NULLE;
    if (r > 32767.0) return ERR_E_LIGNE_NON_ENTIERE;
    if (r > 255.0) return ERR_E_LIGNE_INEXISTANTE;
    pile_consommer(c->p, 1);
    *ligne = (unsigned char) r;
    return ERR_AUCUNE;
}

/* --- GARER, CHARGER, SUPPRIMER (Bibliothécaire et fichiers IMP-0253) ------------- */

/* γ : « # » facultatif (fichier permanent), puis un nom de fichier L.S.E. ;
   EX 53 sinon (point C7). */
static NatureErreur nom_de_fichier(Cycle *c, unsigned int prof, char *nom, int *permanent)
{
    ChaineSurPile s;
    unsigned char t[8];
    unsigned int n;
    unsigned int d = 0;
    NatureErreur e;

    if ((e = chaine(c, prof, USAGE_CALCUL, &s)) != ERR_AUCUNE) return e;
    n = donnees_texte(&s, 0, t, 7);
    *permanent = n > 0 && t[0] == '#';
    if (*permanent) d = 1;
    if (!bibliotheque_nom_valide((const char *) t + d, n - d)) return ERR_E_NOM_FICHIER;
    memcpy(nom, t + d, n - d);
    nom[n - d] = '\0';
    return ERR_AUCUNE;
}

/* β : entier de 1 à 32 767, EX 50 sinon (point C2). */
static NatureErreur numero_enregistrement(Cycle *c, unsigned int prof, unsigned int *v)
{
    float f;
    double r;
    NatureErreur e;

    if ((e = nombre(c, prof, USAGE_CALCUL, &f)) != ERR_AUCUNE) return e;
    r = f;
    if (r < 1.0 || r > (double) FICHIERS_NUMERO_MAX || floor(r) != r) return ERR_E_NUMERO_ENREGISTREMENT;
    *v = (unsigned int) r;
    return ERR_AUCUNE;
}

/* GARER α,β,γ : pile ref α, β, γ (LSE15 §6-3.3, point C3). */
static NatureErreur garer(Cycle *c)
{
    ElementPile x;
    EntreeEnvironnement t;
    ChaineSurPile s;
    char nom[8];
    int permanent;
    unsigned int num;
    unsigned char type;
    unsigned char *contenu;
    unsigned long lg;
    float v;
    NatureErreur e;

    if ((e = nom_de_fichier(c, 0, nom, &permanent)) != ERR_AUCUNE) return e;
    if ((e = numero_enregistrement(c, 1, &num)) != ERR_AUCUNE) return e;
    if (!pile_lire(c->p, 2, &x) || x.type != PILE_REFERENCE) return ERR_E_INTERNE;
    t.etat = ID_VARIABLE;
    if (x.contenu.reference.indexee == REF_SIMPLE
        && (e = contexte_entree(x.contenu.reference.cellule, &t)) != ERR_AUCUNE) return e;
    if (t.etat == ID_TABLEAU) {                               /* image de l'objet (IMP-0247) */
        lg = objets_longueur(t.contenu.objet);
        if ((contenu = (unsigned char *) malloc((size_t) lg)) == NULL) return ERR_E_MEMOIRE_SATUREE;
        memcpy(contenu, objets_resoudre(t.contenu.objet), (size_t) lg);    /* plus de 64 Kio possible */
        objets_relacher(t.contenu.objet);
        type = contenu[1] == 2 ? ENREG_TABLEAU_2 : ENREG_TABLEAU_1;
    } else if (t.etat == ID_CHAINE) {
        if ((e = chaine(c, 2, USAGE_AFFECTATION, &s)) != ERR_AUCUNE) return e;
        lg = s.longueur;
        if ((contenu = (unsigned char *) malloc((size_t) (lg > 0 ? lg : 1))) == NULL) return ERR_E_MEMOIRE_SATUREE;
        donnees_texte(&s, 0, contenu, (unsigned int) lg);
        type = ENREG_CHAINE;
    } else {
        if ((e = nombre(c, 2, USAGE_AFFECTATION, &v)) != ERR_AUCUNE) return e;
        lg = 4;
        if ((contenu = (unsigned char *) malloc(4)) == NULL) return ERR_E_MEMOIRE_SATUREE;
        memcpy(contenu, &v, 4);
        type = ENREG_NOMBRE;
    }
    e = fichiers_garer(nom, permanent, num, type, contenu, lg);
    free(contenu);
    if (e == ERR_AUCUNE) pile_consommer(c->p, 3);
    return e;
}

/* CHARGER α,β,γ{,δ} : pile ref α, β, γ{, ref δ} (LSE15 §6-3.4, point C4). */
static NatureErreur charger(Cycle *c, unsigned char k)
{
    unsigned int base = k == 4 ? 1 : 0;
    ElementPile x;
    ChaineSurPile s;
    HandleObjet h;
    char nom[8];
    int permanent;
    unsigned int num;
    unsigned char type = 0;
    unsigned char *contenu = NULL;
    unsigned long lg = 0;
    float v;
    NatureErreur e;
    NatureErreur r;

    if ((e = nom_de_fichier(c, base, nom, &permanent)) != ERR_AUCUNE) return e;
    if ((e = numero_enregistrement(c, base + 1, &num)) != ERR_AUCUNE) return e;
    if (!pile_lire(c->p, base + 2, &x) || x.type != PILE_REFERENCE
        || x.contenu.reference.indexee != REF_SIMPLE)
        return ERR_E_INTERNE;
    r = fichiers_charger(nom, permanent, num, &type, &contenu, &lg);
    if (k == 4) {                                             /* compte rendu δ */
        if (r == ERR_E_FICHIER_INEXISTANT) v = -2.0f;
        else if (r == ERR_E_ENREGISTREMENT_INEXISTANT) v = -1.0f;
        else if (r == ERR_AUCUNE) donnees_nombre_entier((long) type, &v);
        else return r;
        if ((e = ecrire_reference(c, 0, &v)) != ERR_AUCUNE) { free(contenu); return e; }
        if (r != ERR_AUCUNE) {
            pile_consommer(c->p, k);
            return ERR_AUCUNE;
        }
    } else if (r != ERR_AUCUNE) {
        return r;
    }
    if (type == ENREG_NOMBRE && lg == 4) {
        memcpy(&v, contenu, 4);
        e = contexte_charger_nombre(x.contenu.reference.cellule, &v);
    } else if (type == ENREG_CHAINE) {
        if ((e = donnees_chaine_creer(contenu, (unsigned int) lg, &s)) == ERR_AUCUNE) {
            donnees_chaine_vers_objet(&s, &h);
            if ((e = contexte_charger_chaine(x.contenu.reference.cellule, h)) != ERR_AUCUNE)
                objets_liberer(h);
        }
    } else if ((type == ENREG_TABLEAU_1 || type == ENREG_TABLEAU_2) && donnees_image_tableau_valide(contenu, lg)
               && contenu[1] == (type == ENREG_TABLEAU_2 ? 2 : 1)) {     /* en-tête contrôlé */
        h = objets_creer(lg);
        if (h == GESTOBJ_HANDLE_NUL) e = ERR_E_MEMOIRE_SATUREE;
        else {
            memcpy(objets_resoudre(h), contenu, (size_t) lg);
            objets_relacher(h);
            if ((e = contexte_charger_tableau(x.contenu.reference.cellule, h)) != ERR_AUCUNE)
                objets_liberer(h);
        }
    } else {
        e = ERR_F_FICHIER_ENDOMMAGE;
    }
    free(contenu);
    if (e == ERR_AUCUNE) pile_consommer(c->p, k);
    return e;
}

/* SUPPRIMER γ{,β} : l'enregistrement β, ou le fichier (INRDP75 §2-3-3, point C5). */
static NatureErreur supprimer_donnees(Cycle *c, unsigned char k)
{
    char nom[8];
    int permanent;
    unsigned int num = 0;
    NatureErreur e;

    if ((e = nom_de_fichier(c, k - 1, nom, &permanent)) != ERR_AUCUNE) return e;
    if (k == 2 && (e = numero_enregistrement(c, 0, &num)) != ERR_AUCUNE) return e;
    if ((e = fichiers_supprimer(nom, permanent, num)) == ERR_AUCUNE) pile_consommer(c->p, k);
    return e;
}

/* EXECUTER α{,β} (LSE15 §5-7 ; Bibliothécaire et fichiers, point B5) : le
   module remplace le programme courant, toutes les données sont perdues, et
   l'exécution reprend au niveau principal en ligne β, 1 par défaut. */
static NatureErreur executer_module(Cycle *c, unsigned char k)
{
    ChaineSurPile s;
    unsigned char nom[8];
    unsigned char ligne = 1;
    unsigned int n;
    NatureErreur e;

    if (k == 2 && (e = cible_calculee(c, &ligne)) != ERR_AUCUNE) return e;
    if ((e = chaine(c, 0, USAGE_CALCUL, &s)) != ERR_AUCUNE) return e;
    n = donnees_texte(&s, 0, nom, 6);
    if (!bibliotheque_nom_valide((const char *) nom, n)) return ERR_E_NOM_FICHIER;
    nom[n] = '\0';
    e = bibliotheque_appeler((const char *) nom, ligne, 0);
    if (e == ERR_F_FICHIER_INEXISTANT) return ERR_E_MODULE_INTROUVABLE;    /* EX 105 */
    if (e != ERR_AUCUNE) return e;
    pile_ramener(c->p, 0);
    if ((e = contexte_initialiser(ligne, 0, c->pos)) != ERR_AUCUNE) return e;
    c->fin = FIN_EXECUTER;
    c->fini = 1;
    return ERR_AUCUNE;
}

static NatureErreur retourner(Cycle *c, int a_resultat, unsigned char cible)
{
    int abandon;
    NatureErreur n = contexte_retourner(c->p, a_resultat, cible, c->pos, &abandon);
    if (n == ERR_AUCUNE) {
        c->transition = 1;
        contexte_rendre_affichage(&aff);                     /* AFFICHER de l'appelant */
    }
    return n;
}

static NatureErreur element(Cycle *c, unsigned int p0)
{
    const unsigned char *o = c->seq + p0;
    TagRCIM t = o[0];
    ReferenceResolue r;
    ChaineSurPile s;
    EntreeEnvironnement e;
    EntreeConstante ce;
    HandleObjet h;
    float i1;
    float i2 = 0.0f;
    unsigned int rang;
    unsigned int k;
    unsigned char ligne;
    int b;
    int saut;
    NatureErreur n;

    /* valeurs immédiates */
    if (t >= RCIM_VAL_IMMEDIAT_BASE + RCIM_VAL_IMMEDIAT_MIN && t <= 0x3F)
        return pousser_immediat(c, (int) t - RCIM_VAL_IMMEDIAT_BASE);
    switch (t) {
    case RCIM_VAL_ENTIER_COURT: return pousser_entier(c, (long) (signed char) o[1]);
    case RCIM_VAL_ENTIER:       return pousser_entier(c, (long) rcim_lire_s16(o + 1));
    case RCIM_VAL_REEL:         return pile_empiler_nombre(c->p, rcim_lire_reel(o + 1));
    case RCIM_VAL_LIGNE:        return pousser_entier(c, (long) o[1]);
    case RCIM_VAL_FAUX:         return pousser_booleen(c, 0);
    case RCIM_VAL_VRAI:         return pousser_booleen(c, 1);
    case RCIM_VAL_CHAINE:
        s.transitoire = 0;
        if (o[1] & RCIM_CHAINE_INDEX) {                         /* constante partagée */
            if (!constantes_lire(rcim_lire_u16(o + 2), &ce)) return ERR_E_INTERNE;
            s.objet = ce.texte;
            s.decalage = 0;
            s.longueur = ce.longueur;
        } else {                                                /* texte dans le bloc */
            s.objet = c->bloc;
            s.decalage = (unsigned char) (p0 + 2);
            s.longueur = o[1] & 0x7Fu;
        }
        return pousser_chaine(c, &s);

    /* références */
    case RCIM_REF_SIMPLE:
        if ((n = contexte_resoudre(rcim_lire_u16(o + 1), &r)) != ERR_AUCUNE) return n;
        return pile_empiler_reference(c->p, &r);
    case RCIM_REF_INDEXEE:
        k = o[3];
        if ((n = nombre(c, k - 1, USAGE_CALCUL, &i1)) != ERR_AUCUNE) return n;
        if (k == 2 && (n = nombre(c, 0, USAGE_CALCUL, &i2)) != ERR_AUCUNE) return n;
        if ((n = contexte_resoudre(rcim_lire_u16(o + 1), &r)) != ERR_AUCUNE) return n;
        if (r.indexee != REF_SIMPLE) return ERR_E_INDICES_SANS_TABLEAU;
        if ((n = contexte_entree(r.cellule, &e)) != ERR_AUCUNE) return n;
        if (e.etat != ID_TABLEAU) return ERR_E_INDICES_SANS_TABLEAU;          /* LSE15 §3-1.2 */
        if ((n = donnees_rang_exact(e.contenu.objet, (unsigned char) k, &i1, &i2, &rang)) != ERR_AUCUNE) return n;
        pile_consommer(c->p, k);
        r.indexee = REF_ELEMENT;
        r.decalage = rang;
        return pile_empiler_reference(c->p, &r);
    case RCIM_REF_LIGNE_COURANTE: return pousser_entier(c, (long) c->ligne);
    case RCIM_REF_LIGNE_RETOUR:   return pousser_entier(c, (long) contexte_ligne_retour());

    /* opérations */
    case RCIM_OP_ADD: case RCIM_OP_SUB: case RCIM_OP_MUL: case RCIM_OP_DIV: case RCIM_OP_PUIS:
        return binaire(c, t);
    case RCIM_OP_NEG: case RCIM_OP_RAC: case RCIM_OP_ABS: case RCIM_OP_EXP: case RCIM_OP_LGN:
    case RCIM_OP_SIN: case RCIM_OP_COS: case RCIM_OP_ATG: case RCIM_OP_ENT:
    case RCIM_OP_SH: case RCIM_OP_CH: case RCIM_OP_TH: case RCIM_OP_SGN:
        return fonction_1(c, t);
    case RCIM_OP_EGAL: case RCIM_OP_INF: case RCIM_OP_INF_EGAL:
    case RCIM_OP_DIFF: case RCIM_OP_SUP: case RCIM_OP_SUP_EGAL:
        return relation(c, t);
    case RCIM_OP_ET: case RCIM_OP_OU:
        return logique(c, t);
    case RCIM_OP_AFFECTER:
        return affecter(c);
    case RCIM_OP_CONCAT: {
        ChaineSurPile a2;
        if ((n = chaine(c, 1, USAGE_CALCUL, &s)) != ERR_AUCUNE) return n;
        if ((n = chaine(c, 0, USAGE_CALCUL, &a2)) != ERR_AUCUNE) return n;
        if ((n = donnees_concatener(&s, &a2, &s)) != ERR_AUCUNE) return n;
        pile_consommer(c->p, 2);
        return pousser_chaine(c, &s);
    }
    case RCIM_OP_LGR: case RCIM_OP_CCA: case RCIM_OP_EQC:
        return fonction_chaine(c, t, 1);
    case RCIM_OP_POS:
        return fonction_chaine(c, t, 3);
    case RCIM_OP_SKP: case RCIM_OP_PTR: case RCIM_OP_CNB: case RCIM_OP_EQN:
    case RCIM_OP_SCH: case RCIM_OP_GRL:
        return fonction_chaine(c, t, o[1]);
    case RCIM_OP_UN:
    case RCIM_OP_ZER: {                                          /* IMP-0218 */
        ElementPile x;
        if (!pile_lire(c->p, 0, &x) || x.type != PILE_REFERENCE || x.contenu.reference.indexee != REF_SIMPLE)
            return ERR_E_INDICES_SANS_TABLEAU;
        if ((n = contexte_entree(x.contenu.reference.cellule, &e)) != ERR_AUCUNE) return n;
        if (e.etat != ID_TABLEAU) return ERR_E_INDICES_SANS_TABLEAU;
        if ((n = donnees_remplir(e.contenu.objet, t == RCIM_OP_UN ? 1.0f : 0.0f)) != ERR_AUCUNE) return n;
        pile_consommer(c->p, 1);
        return ERR_AUCUNE;
    }
    case RCIM_OP_APPEL:
        contexte_garder_affichage(&aff);                     /* cadre de l'appelant (IMP-0221) */
        n = contexte_appeler(c->p, o[2], o[1], c->pos);
        if (n == ERR_AUCUNE) {
            c->transition = 1;
            aff.reste = 0;                                   /* la procédure part sans description */
        }
        return n;
    case RCIM_OP_TABLEAU:
        k = o[3];
        if ((n = nombre(c, k - 1, USAGE_CALCUL, &i1)) != ERR_AUCUNE) return n;
        if (k == 2 && (n = nombre(c, 0, USAGE_CALCUL, &i2)) != ERR_AUCUNE) return n;
        if ((n = donnees_creer_tableau((unsigned char) k, i1, i2, &h)) != ERR_AUCUNE) return n;
        if ((n = contexte_declarer_tableau(rcim_lire_u16(o + 1), h)) != ERR_AUCUNE) {
            objets_liberer(h);
            return n;
        }
        pile_consommer(c->p, k);
        return ERR_AUCUNE;
    case RCIM_OP_CHAINE:  return contexte_declarer_chaine(rcim_lire_u16(o + 1));
    case RCIM_OP_LIBERER: return contexte_liberer(rcim_lire_u16(o + 1));

    /* contrôle */
    case RCIM_CTL_SI_FAUX:
        if ((n = booleen(c, &b)) != ERR_AUCUNE) return n;
        if (!b) c->pos->decalage = o[1];
        return ERR_AUCUNE;
    case RCIM_CTL_SAUT:
        c->pos->decalage = o[1];
        return ERR_AUCUNE;
    case RCIM_CTL_ALLER_EN:
        n = contexte_brancher(o[1], c->pos);
        if (n == ERR_AUCUNE) c->transition = 1;
        return n;
    case RCIM_CTL_ALLER_EN_CALCULE:
        if ((n = cible_calculee(c, &ligne)) != ERR_AUCUNE) return n;
        n = contexte_brancher(ligne, c->pos);
        if (n == ERR_AUCUNE) c->transition = 1;
        return n;
    case RCIM_CTL_RETOUR:     return retourner(c, 0, 0);
    case RCIM_CTL_RETOUR_EN:  return retourner(c, 0, o[1]);
    case RCIM_CTL_RETOUR_EN_CALCULE:
        if ((n = cible_calculee(c, &ligne)) != ERR_AUCUNE) return n;
        return retourner(c, 0, ligne);
    case RCIM_CTL_RESULTAT:   return retourner(c, 1, 0);
    case RCIM_CTL_TERMINER:
        c->fin = FIN_TERMINER;
        c->fini = 1;
        return ERR_AUCUNE;
    case RCIM_CTL_PAUSE:
        c->fin = FIN_PAUSE;
        c->fini = 1;
        return ERR_AUCUNE;
    case RCIM_CTL_FAIRE_OUVRIR:
        return contexte_ouvrir_boucle(c->p, rcim_lire_u16(o + 1), c->pos);
    case RCIM_CTL_FAIRE_OUVRIR_SEUL:
        return contexte_ouvrir_boucle_seule(c->p, c->pos);
    case RCIM_CTL_FAIRE_PAS:           return contexte_pas(c->p, 1);
    case RCIM_CTL_FAIRE_PAS_IMPLICITE: return contexte_pas(c->p, 0);
    case RCIM_CTL_FAIRE_JUSQUA:
    case RCIM_CTL_FAIRE_TANT_QUE:
        n = contexte_tester(c->p, t == RCIM_CTL_FAIRE_TANT_QUE, c->pos, &saut);
        if (n == ERR_AUCUNE && saut) c->transition = 1;
        return n;
    case RCIM_CTL_COMMENTAIRE:
        return ERR_AUCUNE;
    case RCIM_CTL_LIRE_TETE:                                   /* un bip par instruction */
        console_bip();
        rang_tableau = 0;                 /* le reste de la ligne reste à lire (point L6) */
        return ERR_AUCUNE;
    case RCIM_OP_LIRE_VALEUR:
        return lire(c, p0);
    case RCIM_OP_AFF_LIBELLE: case RCIM_OP_AFF_LIGNE: case RCIM_OP_AFF_ESPACE:
    case RCIM_OP_AFF_RETOUR: case RCIM_OP_AFF_SAUT: case RCIM_OP_AFF_DESCRIPTION:
    case RCIM_OP_AFF_VALEUR: case RCIM_OP_AFF_TETE_FORMAT: case RCIM_OP_AFF_TETE_SANS_FORMAT:
        return afficher(c, o);
    case RCIM_CTL_PROCEDURE:
        return ERR_E_LIGNE_DECLARATION;                         /* IMP-0155 */

    case RCIM_OP_EXECUTER: return executer_module(c, o[1]);
    case RCIM_OP_GARER: return garer(c);
    case RCIM_OP_CHARGER: return charger(c, o[1]);
    case RCIM_OP_SUPPRIMER: return supprimer_donnees(c, o[1]);
    case RCIM_OP_ALE: return fonction_ale(c);
    case RCIM_OP_TEM: return fonction_tem(c);
    case RCIM_OP_DAT: return fonction_dat(c);

    default:   /* fichiers, NON : IMP-0219 */
        return ERR_E_INTERNE;
    }
}

/* --- cycle (État d'exécution §7.2) ------------------------------------------- */

static int charger_ligne(Cycle *c)
{
    EntreeObjetCompile e;
    if (!programme_chercher(c->pos->ligne, &e)) return 0;
    if (!objets_copier(e.bloc, 0, c->seq, e.longueur_octets)) return 0;   /* IMP-0215, IMP-0243 */
    c->lg = e.longueur_octets;
    c->bloc = e.bloc;
    c->ligne = c->pos->ligne;
    return 1;
}

static FinExecution erreur(Cycle *c, DiagnosticExecution *d, NatureErreur n, unsigned char ligne)
{
    d->nature = n;
    d->ligne = c->bureau ? 0 : ligne;
    return FIN_ERREUR;
}

/* Points d'arrêt à l'arrivée sur une ligne : ligne d'arrêt, ESC, pas à pas
   (Moniteur IMP-0232, IMP-0239). Le retour sur la ligne FAIRE au début d'une
   itération compte comme une arrivée sur cette ligne. Une ligne signalée ne
   l'est pas de nouveau à la reprise qui la fait exécuter. */
#define FIN_AUCUNE 255
static int           pas_a_pas = 0;
static unsigned char ligne_signalee = 0;

void interprete_pas_a_pas(int vrai)
{
    pas_a_pas = vrai != 0;
}

void interprete_reinitialiser(void)
{
    lecture_oublier();
    ligne_signalee = 0;
    aff.reste = 0;
}

static FinExecution point_d_arret(const PositionExecution *pos, int arrivee_ligne)
{
    unsigned char arret;
    FinExecution f;

    if (ligne_signalee != 0 && ligne_signalee == pos->ligne) {   /* reprise de la ligne signalée */
        ligne_signalee = 0;
        return FIN_AUCUNE;
    }
    if (!arrivee_ligne) return FIN_AUCUNE;
    ligne_signalee = 0;
    arret = contexte_ligne_arret();
    if (arret != 0 && pos->ligne == arret) f = FIN_ARRET;
    else if (console_interruption()) f = FIN_INTERRUPTION;
    else if (pas_a_pas) f = FIN_PAS;
    else return FIN_AUCUNE;
    ligne_signalee = pos->ligne;
    return f;
}

/* Arrivée par progression, itération ou branchement : ligne nouvelle à charger.
   Rend FIN_AUCUNE pour poursuivre, une fin d'exécution sinon. */
static FinExecution arrivee(Cycle *c, DiagnosticExecution *d, unsigned char ligne, int iteration)
{
    if (c->pos->ligne != c->ligne && !charger_ligne(c)) return erreur(c, d, ERR_E_INTERNE, ligne);
    return point_d_arret(c->pos, c->pos->decalage == 0 || iteration);
}

static FinExecution cycle(Cycle *c, DiagnosticExecution *d)
{
    unsigned int p0;
    unsigned int lg;
    unsigned char ligne;
    NatureErreur n;
    FinExecution f;

    d->nature = ERR_AUCUNE;
    d->ligne = 0;
    for (;;) {
        if (c->pos->decalage >= c->lg) {                     /* séquence épuisée */
            if (c->bureau) return FIN_BUREAU;
            ligne = c->ligne;
            if ((n = contexte_fin_de_ligne(c->pos)) != ERR_AUCUNE) return erreur(c, d, n, ligne);
            /* décalage non nul : retour sur la ligne FAIRE, nouvelle itération */
            if ((f = arrivee(c, d, ligne, c->pos->decalage != 0)) != FIN_AUCUNE) return f;
            continue;
        }
        p0 = c->pos->decalage;
        ligne = c->ligne;
        lg = rcim_longueur_element(c->seq, c->lg, p0);
        if (lg == 0) return erreur(c, d, ERR_E_INTERNE, ligne);
        c->pos->decalage = p0 + lg;
        c->transition = 0;
        n = element(c, p0);
        if (n != ERR_AUCUNE) {
            if (!c->transition) { c->pos->ligne = ligne; c->pos->decalage = p0; }
            return erreur(c, d, n, ligne);
        }
        if (c->fini) return c->fin;
        if (c->transition && !c->bureau) {
            if ((f = arrivee(c, d, ligne, 0)) != FIN_AUCUNE) return f;
        }
    }
}

FinExecution interprete_executer(PileEvaluation *p, PositionExecution *pos,
                                 DiagnosticExecution *diagnostic)
{
    Cycle c;
    FinExecution f;
    memset(&c, 0, sizeof c);
    c.p = p;
    c.pos = pos;
    if (!charger_ligne(&c)) {
        diagnostic->nature = ERR_E_LIGNE_INEXISTANTE;
        diagnostic->ligne = pos->ligne;
        return FIN_ERREUR;
    }
    diagnostic->nature = ERR_AUCUNE;
    diagnostic->ligne = 0;
    if ((f = point_d_arret(pos, pos->decalage == 0)) != FIN_AUCUNE) return f;
    return cycle(&c, diagnostic);
}

FinExecution interprete_bureau(PileEvaluation *p, HandleObjet bloc, unsigned int longueur,
                               PositionExecution *pos, DiagnosticExecution *diagnostic)
{
    Cycle c;
    unsigned char *o;
    DescriptionAffichage garde = aff;            /* AFFICHER du programme suspendu (IMP-0221) */
    FinExecution f;

    memset(&c, 0, sizeof c);
    pos->ligne = 0;
    c.p = p;
    c.pos = pos;
    c.bureau = 1;
    c.bloc = bloc;
    c.lg = longueur;
    o = objets_resoudre(bloc);
    if (o == NULL) return erreur(&c, diagnostic, ERR_E_INTERNE, 0);
    memcpy(c.seq, o, longueur);
    objets_relacher(bloc);
    f = cycle(&c, diagnostic);
    aff = garde;
    return f;
}
