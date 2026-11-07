/* contexte_exec.c — environnement des identificateurs, cadres d'activation,
   contextes de boucle FAIRE
   EXI: EXI-0022 à EXI-0024, EXI-0029 à EXI-0039, EXI-0042 à EXI-0044
   VM: VM-0801 à VM-0803, VM-0901 à VM-0903, VM-1102, VM-1103, VM-1302,
       VM-1303, VM-3006, VM-3007
   IV: IV-FMT-0010, ALG-1101, ALG-1201 à ALG-1203
   IMP: IMP-0146 (cadre), IMP-0148 (alias), IMP-0149 (sauvegardes), IMP-0244 (pas et test de FAIRE sans SANE),
        IMP-0150 (cadres malloc), IMP-0151, IMP-0152 (boucles), IMP-0153
        (départ, arrêt), IMP-0154, IMP-0179 (environnement), IMP-0208,
        IMP-0243 (ligne d'entrée copiée sans verrouillage)
   Tests: tests/composants/composants_contexte.c
   Spécification : chapitre État d'exécution §3 bis, §5, §6, §7.1.
   Segment "contexte" (choix E3 du 2026-10-23, IMP-0197). */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include "contexte_exec.h"
#include "programme.h"
#include "symboles.h"
#include "donnees.h"
#include "../interfaces/profil.h"

segment "contexte";

/* --- structures (IMP-0146, IMP-0151, D1) ---------------------------------- */

typedef struct {
    unsigned short symbole;
    unsigned short ancienne_cellule;     /* case du nom avant l'appel (D1) */
} SauvegardeLocale;

typedef struct {
    unsigned short   symbole_formel;
    ReferenceResolue cible;               /* case capturée à l'appel (IMP-0148, D1) */
} AliasReference;

#define BOUCLE_INITIALE   0
#define BOUCLE_ITERATION  1

typedef struct {
    unsigned short cellule_V;            /* case de la variable contrôlée, ou CASE_AUCUNE */
    float          pas;
    unsigned char  phase;
    unsigned char  ligne_faire;
    unsigned int   decalage_reprise;
    unsigned char  ligne_fin;
} ContexteBoucle;

typedef struct CadreActivation {
    struct CadreActivation *appelant;     /* NULL : cadre racine */
    unsigned char           numero_ligne_retour;
    unsigned int            offset_retour;
    unsigned char           position_appel;
    unsigned char           numero_ligne_procedure;
    unsigned int            hauteur_base;          /* pile à l'entrée du cadre (point B2) */
    SauvegardeLocale       *sauvegardes;
    unsigned char           nb_sauvegardes;
    AliasReference         *alias;
    unsigned char           nb_alias;
    ContexteBoucle         *pile_boucles;
    unsigned char           nb_boucles_actives;
    unsigned char           capacite_boucles;
    DescriptionAffichage    affichage;             /* AFFICHER suspendu par un appel (IMP-0221) */
} CadreActivation;

#define CASE_LIBRE   0xFF                 /* état d'une case de la liste libre */
#define CASE_AUCUNE  0xFFFFu
/* IMP-0179 : en modèle small, un tableau obtenu par malloc ne dépasse pas 64 Kio
   (manuel ORCA/C, memorymodel) : 13 107 cases de 5 octets. */
#define CASES_MAX    ((unsigned int) (65535u / sizeof(EntreeEnvironnement)))

static unsigned short *noms = NULL;       /* nom -> case (D1) */
static unsigned int taille_noms = 0;
static EntreeEnvironnement *cases = NULL; /* réserve des cases */
static unsigned int nb_cases = 0;
static unsigned int premiere_libre = CASE_AUCUNE;
static CadreActivation *courant = NULL;
static unsigned char ligne_arret = 0;
static unsigned char ligne_entree[RCIM_LONGUEUR_MAX_SEQUENCE];   /* copie de la ligne d'entrée (IMP-0243) */

/* Préparation d'un appel (audit du code, AP-09) : valeurs, reprises et cases
   neuves dans des tableaux statiques plutôt que trois malloc par appel ;
   l'élément 0xCD d'une ligne de 255 octets (5 + 2n + 2m) borne paramètres et
   locaux à 125. contexte_appeler n'est jamais réentré. */
#define ARITE_MAX ((RCIM_LONGUEUR_MAX_SEQUENCE - 5u) / 2u)
static EntreeEnvironnement appel_vals[ARITE_MAX];
static int                 appel_repris[ARITE_MAX];
static unsigned int        appel_neuves[ARITE_MAX];

/* --- cases (D1) -------------------------------------------------------------- */

static void liberer_contenu(EntreeEnvironnement *e)
{
    if ((e->etat == ID_CHAINE || e->etat == ID_TABLEAU) && e->contenu.objet != GESTOBJ_HANDLE_NUL)
        objets_liberer(e->contenu.objet);
}

static void vider_case(unsigned int c)
{
    liberer_contenu(&cases[c]);
    cases[c].etat = ID_NON_DEFINI;
    cases[c].contenu.objet = GESTOBJ_HANDLE_NUL;
}

static NatureErreur allouer_case(unsigned int *c)
{
    EntreeEnvironnement *t;
    unsigned int n;
    unsigned int i;

    if (premiere_libre == CASE_AUCUNE) {
        if (nb_cases >= CASES_MAX) return ERR_E_MEMOIRE_SATUREE;   /* EX 03 */
        n = nb_cases == 0 ? 32 : 2 * nb_cases;
        if (n > CASES_MAX) n = CASES_MAX;
        t = (EntreeEnvironnement *) realloc(cases, n * sizeof(EntreeEnvironnement));
        if (t == NULL) return ERR_E_MEMOIRE_SATUREE;
        cases = t;
        for (i = n; i > nb_cases; i--) {                      /* chaînage des nouvelles cases */
            cases[i - 1].etat = CASE_LIBRE;
            cases[i - 1].contenu.objet = (HandleObjet) premiere_libre;
            premiere_libre = i - 1;
        }
        nb_cases = n;
    }
    *c = premiere_libre;
    premiere_libre = (unsigned int) cases[*c].contenu.objet;
    cases[*c].etat = ID_NON_DEFINI;
    cases[*c].contenu.objet = GESTOBJ_HANDLE_NUL;
    return ERR_AUCUNE;
}

static void rendre_case(unsigned int c)
{
    liberer_contenu(&cases[c]);
    cases[c].etat = CASE_LIBRE;
    cases[c].contenu.objet = (HandleObjet) premiere_libre;
    premiere_libre = c;
}

/* --- environnement (IMP-0179) --------------------------------------------- */

/* Agrandissement paresseux jusqu'au nombre de symboles (choix E4) ; chaque
   nom nouveau reçoit sa case. */
static NatureErreur assurer(unsigned int id)
{
    unsigned short *t;
    unsigned int n;
    unsigned int c;
    NatureErreur e;

    if (id < taille_noms) return ERR_AUCUNE;
    n = symboles_nombre();
    if (n <= id) n = id + 1;
    t = (unsigned short *) realloc(noms, n * sizeof(unsigned short));
    if (t == NULL) return ERR_E_MEMOIRE_SATUREE;
    noms = t;
    while (taille_noms < n) {
        if ((e = allouer_case(&c)) != ERR_AUCUNE) return e;
        noms[taille_noms++] = (unsigned short) c;
    }
    return ERR_AUCUNE;
}

static int case_valide(unsigned int c)
{
    return c < nb_cases && cases[c].etat != CASE_LIBRE;
}

static NatureErreur non_defini(UsageLecture usage)
{
    if (usage == USAGE_COMPARAISON) return ERR_E_NON_DEFINI_COMPARAISON;
    if (usage == USAGE_AFFECTATION) return ERR_E_NON_DEFINI_AFFECTATION;
    return ERR_E_NON_DEFINI_CALCUL;
}

static int est_alias(unsigned int id, unsigned int *rang)
{
    unsigned int i;
    if (courant == NULL) return 0;
    for (i = 0; i < courant->nb_alias; i++)
        if (courant->alias[i].symbole_formel == id) { *rang = i; return 1; }
    return 0;
}

NatureErreur contexte_resoudre(unsigned int id, ReferenceResolue *r)
{
    unsigned int i;
    NatureErreur n;
    EntreeSymbole s;

    if (est_alias(id, &i)) { *r = courant->alias[i].cible; return ERR_AUCUNE; }
    r->decalage = 0;
    if (symboles_lire(id, &s) && s.nature == SYM_PROCEDURE) {     /* nom de procédure */
        r->cellule = (unsigned short) id;
        r->indexee = REF_PROCEDURE;
        return ERR_AUCUNE;
    }
    if ((n = assurer(id)) != ERR_AUCUNE) return n;
    r->cellule = noms[id];
    r->indexee = REF_SIMPLE;
    return ERR_AUCUNE;
}

NatureErreur contexte_entree(unsigned int c, EntreeEnvironnement *copie)
{
    if (!case_valide(c)) return ERR_E_INTERNE;
    *copie = cases[c];
    return ERR_AUCUNE;
}

NatureErreur contexte_lire_nombre(unsigned int c, UsageLecture usage, float *v)
{
    if (!case_valide(c)) return ERR_E_INTERNE;
    switch (cases[c].etat) {
    case ID_VARIABLE:              *v = cases[c].contenu.valeur; return ERR_AUCUNE;
    case ID_NON_DEFINI:            return non_defini(usage);
    case ID_PARAMETRE_SANS_VALEUR: return ERR_E_PARAMETRE_SANS_VALEUR;
    case ID_CHAINE:
        if (usage == USAGE_COMPARAISON) return ERR_E_COMPARAISON_TYPES;
        if (usage == USAGE_AFFECTATION) return ERR_E_CHAINE_NON_DECLAREE;
        return ERR_E_CALCUL_NON_NUMERIQUE;
    default:                       return ERR_E_CALCUL_NON_NUMERIQUE;   /* tableau (RX7) */
    }
}

NatureErreur contexte_lire_chaine(unsigned int c, UsageLecture usage, ChaineSurPile *ch)
{
    if (!case_valide(c)) return ERR_E_INTERNE;
    switch (cases[c].etat) {
    case ID_CHAINE:
        if (cases[c].contenu.objet == GESTOBJ_HANDLE_NUL) return ERR_E_CHAINE_SANS_VALEUR;
        return donnees_objet_vers_chaine(cases[c].contenu.objet, ch);
    case ID_NON_DEFINI:            return non_defini(usage);
    case ID_PARAMETRE_SANS_VALEUR: return ERR_E_PARAMETRE_SANS_VALEUR;
    default:                       return ERR_E_OPERANDE_NON_CHAINE;
    }
}

NatureErreur contexte_affecter_nombre(unsigned int c, float v)
{
    float copie = v;                         /* paramètre float : format extended */
    return contexte_affecter_nombre_exact(c, &copie);
}

NatureErreur contexte_affecter_nombre_exact(unsigned int c, const float *v)
{
    if (!case_valide(c)) return ERR_E_INTERNE;
    if (cases[c].etat == ID_CHAINE || cases[c].etat == ID_TABLEAU) return ERR_E_NOMBRE_DANS_CHAINE;
    cases[c].etat = ID_VARIABLE;
    cases[c].contenu.valeur = *v;
    return ERR_AUCUNE;
}

NatureErreur contexte_affecter_chaine(unsigned int c, HandleObjet objet)
{
    if (!case_valide(c)) return ERR_E_INTERNE;
    if (cases[c].etat != ID_CHAINE) return ERR_E_CHAINE_NON_DECLAREE;    /* LSE15 §4-3 */
    liberer_contenu(&cases[c]);
    cases[c].contenu.objet = objet;
    return ERR_AUCUNE;
}

NatureErreur contexte_declarer_tableau(unsigned int id, HandleObjet tableau)
{
    unsigned int i;
    unsigned int c;
    NatureErreur n;

    if (est_alias(id, &i)) return ERR_E_TABLEAU_PARAMETRE;              /* LSE15 l. 2104 */
    if ((n = assurer(id)) != ERR_AUCUNE) return n;
    c = noms[id];
    if (cases[c].etat == ID_VARIABLE || cases[c].etat == ID_CHAINE) return ERR_E_DECLARATION_TABLEAU;
    liberer_contenu(&cases[c]);                                          /* libération implicite */
    cases[c].etat = ID_TABLEAU;
    cases[c].contenu.objet = tableau;
    return ERR_AUCUNE;
}

NatureErreur contexte_declarer_chaine(unsigned int id)
{
    ReferenceResolue r;
    NatureErreur n;
    unsigned int c;

    if ((n = contexte_resoudre(id, &r)) != ERR_AUCUNE) return n;
    if (r.indexee) return ERR_E_DECLARATION_CHAINE;
    c = r.cellule;
    if (cases[c].etat == ID_VARIABLE || cases[c].etat == ID_TABLEAU) return ERR_E_DECLARATION_CHAINE;
    liberer_contenu(&cases[c]);                  /* la valeur n'est pas conservée (IMP-0179) */
    cases[c].etat = ID_CHAINE;
    cases[c].contenu.objet = GESTOBJ_HANDLE_NUL;
    return ERR_AUCUNE;
}

/* --- CHARGER : déclaration implicite (Bibliothécaire et fichiers IMP-0253) --- */

static int libre_ou(unsigned int c, unsigned char etat)
{
    return cases[c].etat == ID_NON_DEFINI || cases[c].etat == ID_PARAMETRE_SANS_VALEUR
           || cases[c].etat == etat;
}

NatureErreur contexte_charger_nombre(unsigned int c, const float *v)
{
    if (!case_valide(c)) return ERR_E_INTERNE;
    if (!libre_ou(c, ID_VARIABLE)) return ERR_E_TYPE_CHARGER;
    cases[c].etat = ID_VARIABLE;
    cases[c].contenu.valeur = *v;
    return ERR_AUCUNE;
}

NatureErreur contexte_charger_chaine(unsigned int c, HandleObjet objet)
{
    if (!case_valide(c)) return ERR_E_INTERNE;
    if (!libre_ou(c, ID_CHAINE)) return ERR_E_TYPE_CHARGER;
    if (cases[c].etat == ID_CHAINE) liberer_contenu(&cases[c]);
    cases[c].etat = ID_CHAINE;
    cases[c].contenu.objet = objet;
    return ERR_AUCUNE;
}

NatureErreur contexte_charger_tableau(unsigned int c, HandleObjet tableau)
{
    if (!case_valide(c)) return ERR_E_INTERNE;
    if (!libre_ou(c, ID_TABLEAU)) return ERR_E_TYPE_CHARGER;
    if (cases[c].etat == ID_TABLEAU) liberer_contenu(&cases[c]);
    cases[c].etat = ID_TABLEAU;
    cases[c].contenu.objet = tableau;
    return ERR_AUCUNE;
}

NatureErreur contexte_liberer(unsigned int id)
{
    unsigned int i;
    int local = 0;
    NatureErreur n;

    if (courant != NULL && courant->appelant != NULL) {         /* LSE15 §3-1.2, Remarque 2 */
        for (i = 0; i < courant->nb_sauvegardes; i++)
            if (courant->sauvegardes[i].symbole == id) local = 1;
        if (!local) return ERR_E_LIBERER_NON_LOCAL;
    }
    if ((n = assurer(id)) != ERR_AUCUNE) return n;
    vider_case(noms[id]);
    return ERR_AUCUNE;
}

/* --- cadres (IMP-0146, IMP-0150) ----------------------------------------- */

static CadreActivation *creer_cadre(unsigned int nb_sauvegardes, unsigned int nb_alias)
{
    CadreActivation *c = (CadreActivation *) calloc(1, sizeof(CadreActivation));
    if (c == NULL) return NULL;
    c->sauvegardes = (SauvegardeLocale *) malloc((nb_sauvegardes > 0 ? nb_sauvegardes : 1)
                                                 * sizeof(SauvegardeLocale));
    c->alias = (AliasReference *) malloc((nb_alias > 0 ? nb_alias : 1) * sizeof(AliasReference));
    if (c->sauvegardes == NULL || c->alias == NULL) {
        free(c->sauvegardes);
        free(c->alias);
        free(c);
        return NULL;
    }
    return c;
}

static void liberer_cadre(CadreActivation *c)
{
    free(c->sauvegardes);
    free(c->alias);
    free(c->pile_boucles);
    free(c);
}

/* Restauration des noms d'un cadre, dans l'ordre inverse : la case locale
   est rendue, le nom retrouve sa case d'avant l'appel (IMP-0149, D1). */
static void restaurer(CadreActivation *c)
{
    int i;
    unsigned short s;
    for (i = (int) c->nb_sauvegardes - 1; i >= 0; i--) {
        s = c->sauvegardes[i].symbole;
        rendre_case(noms[s]);
        noms[s] = c->sauvegardes[i].ancienne_cellule;
    }
}

/* Cadres abandonnés sans retour (nouvelle exécution, effacement). */
static void abandonner_cadres(void)
{
    CadreActivation *c;

    while (courant != NULL) {
        c = courant;
        courant = c->appelant;
        restaurer(c);
        liberer_cadre(c);
    }
}

unsigned char contexte_ligne_retour(void)
{
    return courant != NULL && courant->appelant != NULL ? courant->numero_ligne_retour : 0;
}

unsigned int contexte_profondeur(void)
{
    unsigned int n = 0;
    CadreActivation *c = courant;
    while (c != NULL && c->appelant != NULL) { n++; c = c->appelant; }
    return n;
}

/* --- initialisation (§7.1, IMP-0153) ------------------------------------- */

NatureErreur contexte_initialiser(unsigned char depart, unsigned char arret, PositionExecution *pos)
{
    EntreeObjetCompile e;
    unsigned int i;
    int trouvee;

    trouvee = depart != 0 ? programme_chercher(depart, &e) : programme_premiere(&e);
    if (!trouvee) return ERR_E_LIGNE_INEXISTANTE;          /* message : chapitre du moniteur (X-64) */
    abandonner_cadres();
    for (i = 0; i < taille_noms; i++)                       /* LSE15 §9-2 */
        vider_case(noms[i]);
    courant = creer_cadre(0, 0);                            /* cadre racine */
    if (courant == NULL) return ERR_E_MEMOIRE_SATUREE;
    ligne_arret = arret;
    pos->ligne = e.numero_ligne;
    pos->decalage = 0;
    return ERR_AUCUNE;
}

unsigned char contexte_ligne_arret(void)
{
    return ligne_arret;
}

void contexte_fixer_arret(unsigned char arret)
{
    ligne_arret = arret;
}

void contexte_garder_affichage(const DescriptionAffichage *a)
{
    if (courant != NULL) courant->affichage = *a;
}

void contexte_rendre_affichage(DescriptionAffichage *a)
{
    if (courant != NULL) *a = courant->affichage;
    else a->reste = 0;
}

/* REPRENDRE (Moniteur IMP-0232) : niveau principal, identificateurs et valeurs
   conservés ; les cadres des procédures sont abandonnés, leurs LOCAL restaurés,
   et les boucles du niveau principal oubliées. */
NatureErreur contexte_reprendre(unsigned char depart, unsigned char arret, PositionExecution *pos)
{
    EntreeObjetCompile e;
    CadreActivation *c;

    if (!programme_chercher(depart, &e)) return ERR_E_LIGNE_INEXISTANTE;
    while (courant != NULL && courant->appelant != NULL) {
        c = courant;
        courant = c->appelant;
        restaurer(c);
        liberer_cadre(c);
    }
    if (courant == NULL) {
        courant = creer_cadre(0, 0);
        if (courant == NULL) return ERR_E_MEMOIRE_SATUREE;
    }
    courant->nb_boucles_actives = 0;
    ligne_arret = arret;
    pos->ligne = e.numero_ligne;
    pos->decalage = 0;
    return ERR_AUCUNE;
}

void contexte_vider(void)
{
    unsigned int i;
    abandonner_cadres();
    for (i = 0; i < nb_cases; i++)
        if (cases[i].etat != CASE_LIBRE) liberer_contenu(&cases[i]);
    free(cases);
    free(noms);
    cases = NULL;
    noms = NULL;
    nb_cases = 0;
    taille_noms = 0;
    premiere_libre = CASE_AUCUNE;
    ligne_arret = 0;
}

/* --- boucles (§6, IMP-0151, IMP-0152) ------------------------------------- */

unsigned int contexte_nb_boucles(void)
{
    return courant != NULL ? courant->nb_boucles_actives : 0;
}

static ContexteBoucle *boucle_sommet(void)
{
    if (courant == NULL || courant->nb_boucles_actives == 0) return NULL;
    return &courant->pile_boucles[courant->nb_boucles_actives - 1];
}

static void desimbriquer(unsigned char n)
{
    ContexteBoucle *b;
    while ((b = boucle_sommet()) != NULL && (n < b->ligne_faire || n > b->ligne_fin))
        courant->nb_boucles_actives--;
}

/* FIN_DE_LIGNE(L) */
static NatureErreur fin_de_ligne(unsigned char l, PositionExecution *pos)
{
    ContexteBoucle *b = boucle_sommet();
    EntreeObjetCompile e;

    if (b != NULL && b->ligne_fin == l) {
        b->phase = BOUCLE_ITERATION;
        pos->ligne = b->ligne_faire;
        pos->decalage = b->decalage_reprise;
        return ERR_AUCUNE;
    }
    if (!programme_suivante(l, &e)) return ERR_E_FIN_DE_PROGRAMME;   /* IMP-0154 */
    pos->ligne = e.numero_ligne;
    pos->decalage = 0;
    desimbriquer(e.numero_ligne);
    return ERR_AUCUNE;
}

NatureErreur contexte_fin_de_ligne(PositionExecution *pos)
{
    if (pos->ligne == 0) return ERR_E_INTERNE;
    return fin_de_ligne(pos->ligne, pos);
}

NatureErreur contexte_brancher(unsigned char n, PositionExecution *pos)
{
    EntreeObjetCompile e;
    if (n == 0 || !programme_chercher(n, &e)) return ERR_E_LIGNE_INEXISTANTE;
    pos->ligne = n;
    pos->decalage = 0;
    desimbriquer(n);
    return ERR_AUCUNE;
}

/* Arrondi à l'entier le plus voisin, la moitié vers le haut (IMP-0179). */
static long arrondir(float x)
{
    double y = (double) x + 0.5;
    long l;
    if (y > 2.0e9) return 2000000000L;
    if (y < -2.0e9) return -2000000000L;
    l = (long) y;
    if ((double) l > y) l--;
    return l;
}

/* Ligne de fin effective d'une boucle, n1 étant au rang donné de la pile. */
static NatureErreur boucle_ligne_fin(PileEvaluation *p, unsigned int rang, const PositionExecution *pos,
                                     unsigned char *ligne_fin)
{
    float n1;
    long n;
    EntreeObjetCompile fin;
    NatureErreur e;

    if ((e = pile_valeur_numerique(p, rang, USAGE_CALCUL, &n1)) != ERR_AUCUNE) return e;
    n = arrondir(n1);                                        /* LSE15 §5-5.1 */
    if (n < 1) return ERR_E_FIN_BOUCLE_NULLE;
    if (n < pos->ligne) return ERR_E_FIN_BOUCLE_AVANT;
    if (n > 255) n = 255;
    if (!programme_au_plus((unsigned char) n, &fin)) return ERR_E_INTERNE;
    *ligne_fin = fin.numero_ligne;
    return ERR_AUCUNE;
}

/* Contexte de la boucle ouverte en pos : celui du sommet si c'est la même
   ouverture (branchement vers la ligne FAIRE), sinon un nouveau. */
static NatureErreur boucle_contexte(unsigned int cellule, unsigned char ligne_fin,
                                    const PositionExecution *pos)
{
    ContexteBoucle *b;
    ContexteBoucle *t;

    b = boucle_sommet();
    if (b == NULL || b->ligne_faire != pos->ligne || b->decalage_reprise != pos->decalage) {
        if (courant->nb_boucles_actives == courant->capacite_boucles) {
            unsigned int c = courant->capacite_boucles == 0 ? 4 : 2u * courant->capacite_boucles;
            if (c > 255) return ERR_E_MEMOIRE_SATUREE;
            t = (ContexteBoucle *) realloc(courant->pile_boucles, c * sizeof(ContexteBoucle));
            if (t == NULL) return ERR_E_MEMOIRE_SATUREE;
            courant->pile_boucles = t;
            courant->capacite_boucles = (unsigned char) c;
        }
        courant->nb_boucles_actives++;
        b = boucle_sommet();
    }
    b->cellule_V = (unsigned short) cellule;
    b->pas = 1.0f;
    b->phase = BOUCLE_INITIALE;
    b->ligne_faire = pos->ligne;
    b->decalage_reprise = pos->decalage;
    b->ligne_fin = ligne_fin;
    return ERR_AUCUNE;
}

NatureErreur contexte_ouvrir_boucle(PileEvaluation *p, unsigned int id_v, const PositionExecution *pos)
{
    float v1;
    unsigned char ligne_fin;
    ReferenceResolue r;
    NatureErreur e;

    if (courant == NULL) return ERR_E_INTERNE;
    if ((e = pile_valeur_numerique(p, 0, USAGE_CALCUL, &v1)) != ERR_AUCUNE) return e;
    if ((e = boucle_ligne_fin(p, 1, pos, &ligne_fin)) != ERR_AUCUNE) return e;
    if ((e = contexte_resoudre(id_v, &r)) != ERR_AUCUNE) return e;
    if (r.indexee) return ERR_E_VARIABLE_BOUCLE;
    if (cases[r.cellule].etat == ID_CHAINE || cases[r.cellule].etat == ID_TABLEAU)
        return ERR_E_VARIABLE_BOUCLE;
    if ((e = boucle_contexte(r.cellule, ligne_fin, pos)) != ERR_AUCUNE) return e;
    pile_consommer(p, 2);
    cases[r.cellule].etat = ID_VARIABLE;                     /* V ← v1 */
    cases[r.cellule].contenu.valeur = v1;
    return ERR_AUCUNE;
}

NatureErreur contexte_ouvrir_boucle_seule(PileEvaluation *p, const PositionExecution *pos)
{
    unsigned char ligne_fin;
    NatureErreur e;

    if (courant == NULL) return ERR_E_INTERNE;
    if ((e = boucle_ligne_fin(p, 0, pos, &ligne_fin)) != ERR_AUCUNE) return e;
    if ((e = boucle_contexte(CASE_AUCUNE, ligne_fin, pos)) != ERR_AUCUNE) return e;
    pile_consommer(p, 1);
    return ERR_AUCUNE;
}

NatureErreur contexte_pas(PileEvaluation *p, int explicite)
{
    ContexteBoucle *b = boucle_sommet();
    float pas = 1.0f;
    float v;
    float somme;
    NatureErreur e;

    if (b == NULL || b->cellule_V == CASE_AUCUNE) return ERR_E_INTERNE;
    if (explicite) {
        if ((e = pile_valeur_numerique(p, 0, USAGE_CALCUL, &pas)) != ERR_AUCUNE) return e;
        pile_consommer(p, 1);
    }
    b->pas = pas;
    if (b->phase == BOUCLE_ITERATION) {                       /* V ← V + p (LSE15 §5-5.3) */
        if ((e = contexte_lire_nombre(b->cellule_V, USAGE_CALCUL, &v)) != ERR_AUCUNE) return e;
        if (!donnees_operation_entiere('+', &v, &pas, &somme)       /* IMP-0244 */
            && (e = donnees_operation_etendue('+', &v, &pas, &somme)) != ERR_AUCUNE)
            return e;                         /* EX 44 au-delà de FLT_MAX (IMP-0216 ; AC-35) */
        return contexte_affecter_nombre_exact(b->cellule_V, &somme);      /* AP-07 */
    }
    return ERR_AUCUNE;
}

NatureErreur contexte_tester(PileEvaluation *p, int tant_que, PositionExecution *pos, int *saut)
{
    ContexteBoucle *b = boucle_sommet();
    ElementPile c;
    float v2;
    float v;
    int sortie;
    unsigned char l;
    NatureErreur e;

    *saut = 0;
    if (b == NULL) return ERR_E_INTERNE;
    if (tant_que) {
        if (!pile_lire(p, 0, &c) || c.type != PILE_BOOLEEN) return ERR_E_INTERNE;
        sortie = !c.contenu.booleen;
    } else {
        if (b->cellule_V == CASE_AUCUNE) return ERR_E_INTERNE;
        if ((e = pile_valeur_numerique(p, 0, USAGE_COMPARAISON, &v2)) != ERR_AUCUNE) return e;
        if ((e = contexte_lire_nombre(b->cellule_V, USAGE_COMPARAISON, &v)) != ERR_AUCUNE) return e;
        {                                                        /* IMP-0244 : sans SANE */
            float zero = 0.0f;
            int s = donnees_comparer_nombres(&b->pas, &zero);
            int d = donnees_comparer_nombres(&v, &v2);
            sortie = (s > 0 && d > 0) || (s < 0 && d < 0);
        }
    }
    pile_consommer(p, 1);
    if (!sortie) return ERR_AUCUNE;
    l = b->ligne_fin;                                         /* SORTIE_ORDINAIRE */
    courant->nb_boucles_actives--;
    *saut = 1;
    return fin_de_ligne(l, pos);
}

/* --- appel et retour (§5.5) ------------------------------------------------ */

/* Valeur, propre à l'entrée d'environnement, d'un argument passé à un
   paramètre LOCAL ; *repris : l'objet transitoire de l'élément est repris. */
static NatureErreur valeur_locale(const ElementPile *a, EntreeEnvironnement *v, int *repris)
{
    EntreeEnvironnement cible;
    NatureErreur n;

    *repris = 0;
    v->contenu.objet = GESTOBJ_HANDLE_NUL;
    switch (a->type) {
    case PILE_NUMERIQUE:
        v->etat = ID_VARIABLE;
        v->contenu.valeur = a->contenu.numerique;
        return ERR_AUCUNE;
    case PILE_CHAINE:
        v->etat = ID_CHAINE;
        *repris = a->contenu.chaine.transitoire;
        return donnees_chaine_vers_objet(&a->contenu.chaine, &v->contenu.objet);
    case PILE_REFERENCE:
        if (a->contenu.reference.indexee == REF_ELEMENT) {         /* élément de tableau (§5.5) */
            if ((n = contexte_entree(a->contenu.reference.cellule, &cible)) != ERR_AUCUNE) return n;
            if (cible.etat != ID_TABLEAU) return ERR_E_INDICES_SANS_TABLEAU;
            n = donnees_lire_element(cible.contenu.objet, a->contenu.reference.decalage, &v->contenu.valeur);
            if (n == ERR_E_ELEMENT_SANS_VALEUR) {                  /* RX3 : erreur à l'utilisation */
                v->etat = ID_PARAMETRE_SANS_VALEUR;
                v->contenu.objet = GESTOBJ_HANDLE_NUL;
                return ERR_AUCUNE;
            }
            v->etat = ID_VARIABLE;
            return n;
        }
        if (a->contenu.reference.indexee != REF_SIMPLE) return ERR_E_INTERNE;
        if ((n = contexte_entree(a->contenu.reference.cellule, &cible)) != ERR_AUCUNE) return n;
        switch (cible.etat) {
        case ID_VARIABLE:
            *v = cible;
            return ERR_AUCUNE;
        case ID_CHAINE:
        case ID_TABLEAU:
            v->etat = cible.etat;
            if (cible.contenu.objet == GESTOBJ_HANDLE_NUL) {
                v->etat = ID_PARAMETRE_SANS_VALEUR;
                return ERR_AUCUNE;
            }
            return donnees_copier(cible.contenu.objet, &v->contenu.objet);
        default:                                   /* RX3 : erreur à l'utilisation (C1) */
            v->etat = ID_PARAMETRE_SANS_VALEUR;
            return ERR_AUCUNE;
        }
    default:
        return ERR_E_INTERNE;
    }
}

NatureErreur contexte_appeler(PileEvaluation *p, unsigned char k, unsigned char position_appel,
                              PositionExecution *pos)
{
    ElementPile ref;
    ElementPile a;
    EntreeSymbole sp;
    const TableParametresFormels *t;
    EntreeObjetCompile ligne;
    unsigned int entree;
    CadreActivation *c;
    EntreeEnvironnement *vals;
    int *repris;
    unsigned int *neuves;
    unsigned int i;
    unsigned int j;
    unsigned int nl;
    unsigned int nc;
    unsigned int nn;
    NatureErreur n = ERR_AUCUNE;

    if (courant == NULL) return ERR_E_INTERNE;
    /* 1. procédure et arité, sans rien modifier */
    if (!pile_lire(p, k, &ref) || ref.type != PILE_REFERENCE
        || ref.contenu.reference.indexee != REF_PROCEDURE
        || !symboles_lire(ref.contenu.reference.cellule, &sp) || sp.nature != SYM_PROCEDURE)
        return ERR_E_INTERNE;
    if (sp.procedure.numero_ligne_entree == 0) return ERR_E_PROCEDURE_NON_DECLAREE;
    if (k != sp.procedure.arite) return ERR_E_NOMBRE_PARAMETRES;
    /* 2. table des paramètres formels ; entrée après l'élément 0xCD */
    t = programme_parametres(sp.procedure.numero_ligne_entree);
    if (t == NULL || !programme_chercher(sp.procedure.numero_ligne_entree, &ligne)
        || ligne.longueur_octets > RCIM_LONGUEUR_MAX_SEQUENCE
        || !objets_copier(ligne.bloc, 0, ligne_entree, ligne.longueur_octets))
        return ERR_E_INTERNE;
    entree = rcim_longueur_element(ligne_entree, ligne.longueur_octets, 0);

    /* 3. valeurs, alias et cases neuves, préparés avant toute modification */
    nl = 0;
    for (i = 0; i < k; i++) if (t->parametres[i].mode == PARAM_LOCAL) nl++;
    nc = nl + t->nb_locaux_non_parametres;
    if (k > ARITE_MAX || nc > ARITE_MAX) return ERR_E_INTERNE;
    c = creer_cadre(nc, k - nl);
    vals = appel_vals;
    repris = appel_repris;
    neuves = appel_neuves;
    if (c == NULL) {
        return ERR_E_MEMOIRE_SATUREE;
    }
    for (i = 0; i < k && n == ERR_AUCUNE; i++) {
        pile_lire(p, k - 1 - i, &a);
        vals[i].etat = ID_NON_DEFINI;
        vals[i].contenu.objet = GESTOBJ_HANDLE_NUL;
        repris[i] = 0;
        if (t->parametres[i].mode == PARAM_REFERENCE) {
            if (a.type != PILE_REFERENCE) n = ERR_E_PARAMETRE_REFERENCE;      /* LSE15 §7-8.2 */
            else {
                c->alias[c->nb_alias].symbole_formel = t->parametres[i].symbole;
                c->alias[c->nb_alias].cible = a.contenu.reference;    /* case capturée (D1) */
                c->nb_alias++;
            }
        } else
            n = valeur_locale(&a, &vals[i], &repris[i]);
        if (n == ERR_AUCUNE) n = assurer(t->parametres[i].symbole);
    }
    for (j = 0; j < t->nb_locaux_non_parametres && n == ERR_AUCUNE; j++)
        n = assurer(t->locaux_non_parametres[j]);
    nn = 0;
    while (nn < nc && n == ERR_AUCUNE) {
        n = allouer_case(&neuves[nn]);
        if (n == ERR_AUCUNE) nn++;
    }
    if (n != ERR_AUCUNE) {                                   /* rien n'est modifié */
        for (j = 0; j < i; j++)
            if (!repris[j] && (vals[j].etat == ID_CHAINE || vals[j].etat == ID_TABLEAU))
                liberer_contenu(&vals[j]);
        for (j = 0; j < nn; j++) rendre_case(neuves[j]);
        liberer_cadre(c);
        return n;
    }

    /* 4-5. sauvegardes et liaisons : chaque LOCAL reçoit une case neuve (IMP-0149, D1) */
    nn = 0;
    for (i = 0; i < k; i++) {
        unsigned short s = t->parametres[i].symbole;
        if (t->parametres[i].mode != PARAM_LOCAL) continue;
        c->sauvegardes[c->nb_sauvegardes].symbole = s;
        c->sauvegardes[c->nb_sauvegardes].ancienne_cellule = noms[s];
        c->nb_sauvegardes++;
        noms[s] = (unsigned short) neuves[nn];
        cases[neuves[nn]] = vals[i];
        nn++;
        if (repris[i])          /* l'objet transitoire appartient désormais à la case */
            p->elements[p->sommet - k + i].contenu.chaine.transitoire = 0;
    }
    for (j = 0; j < t->nb_locaux_non_parametres; j++) {
        unsigned short s = t->locaux_non_parametres[j];
        c->sauvegardes[c->nb_sauvegardes].symbole = s;
        c->sauvegardes[c->nb_sauvegardes].ancienne_cellule = noms[s];
        c->nb_sauvegardes++;
        noms[s] = (unsigned short) neuves[nn];              /* case neuve : non définie */
        nn++;
    }
    pile_consommer(p, (unsigned int) k + 1);                  /* arguments et référence */

    /* 6-7. cadre courant, position d'entrée */
    c->appelant = courant;
    c->numero_ligne_retour = pos->ligne;
    c->offset_retour = pos->decalage;
    c->position_appel = position_appel;
    c->numero_ligne_procedure = sp.procedure.numero_ligne_entree;
    c->hauteur_base = p->sommet;
    courant = c;
    pos->ligne = sp.procedure.numero_ligne_entree;
    pos->decalage = entree;
    return ERR_AUCUNE;
}

/* Valeur du RESULTAT, indépendante des entrées qui vont être restaurées. */
static NatureErreur resultat_independant(ElementPile *r)
{
    EntreeEnvironnement e;
    ChaineSurPile ch;
    HandleObjet h;
    NatureErreur n;
    float f;

    if (r->type == PILE_REFERENCE && r->contenu.reference.indexee == REF_ELEMENT) {   /* élément (§5.5) */
        if ((n = contexte_entree(r->contenu.reference.cellule, &e)) != ERR_AUCUNE) return n;
        if (e.etat != ID_TABLEAU) return ERR_E_INDICES_SANS_TABLEAU;
        if ((n = donnees_lire_element(e.contenu.objet, r->contenu.reference.decalage, &f)) != ERR_AUCUNE)
            return n;                                              /* EX 32 : élément sans valeur */
        r->type = PILE_NUMERIQUE;
        r->contenu.numerique = f;
        return ERR_AUCUNE;
    }
    if (r->type == PILE_REFERENCE) {
        if (r->contenu.reference.indexee != REF_SIMPLE) return ERR_E_INTERNE;
        if ((n = contexte_entree(r->contenu.reference.cellule, &e)) != ERR_AUCUNE) return n;
        switch (e.etat) {
        case ID_VARIABLE:
            r->type = PILE_NUMERIQUE;
            r->contenu.numerique = e.contenu.valeur;
            return ERR_AUCUNE;
        case ID_TABLEAU:               return ERR_E_RESULTAT_TABLEAU;
        case ID_PARAMETRE_SANS_VALEUR: return ERR_E_PARAMETRE_SANS_VALEUR;
        case ID_NON_DEFINI:            return ERR_E_NON_DEFINI_AFFECTATION;
        default:
            if ((n = contexte_lire_chaine(r->contenu.reference.cellule, USAGE_AFFECTATION, &ch)) != ERR_AUCUNE)
                return n;
            r->type = PILE_CHAINE;
            r->contenu.chaine = ch;
            break;
        }
    }
    if (r->type == PILE_CHAINE && !r->contenu.chaine.transitoire) {   /* copie propre */
        if ((n = donnees_chaine_vers_objet(&r->contenu.chaine, &h)) != ERR_AUCUNE) return n;
        if ((n = donnees_objet_vers_chaine(h, &r->contenu.chaine)) != ERR_AUCUNE) {
            objets_liberer(h);
            return n;
        }
        r->contenu.chaine.transitoire = 1;
    }
    return ERR_AUCUNE;
}

NatureErreur contexte_retourner(PileEvaluation *p, int a_resultat, unsigned char cible,
                                PositionExecution *pos, int *abandon)
{
    CadreActivation *c = courant;
    ElementPile r;
    EntreeObjetCompile e;
    NatureErreur n;

    *abandon = 0;
    if (c == NULL || c->appelant == NULL) return ERR_E_RETOUR_HORS_PROCEDURE;
    if (a_resultat && c->position_appel == RCIM_APPEL_INSTRUCTION)
        return ERR_E_RESULTAT_SOUS_PROGRAMME;                          /* RX2 */
    if (!a_resultat && cible == 0 && c->position_appel == RCIM_APPEL_EXPRESSION)
        return ERR_E_RETOUR_DANS_FONCTION;                             /* RX2 */
    if (cible != 0 && !programme_chercher(cible, &e)) return ERR_E_LIGNE_INEXISTANTE;
    if (a_resultat) {
        if (!pile_lire(p, 0, &r)) return ERR_E_INTERNE;
        if ((n = resultat_independant(&r)) != ERR_AUCUNE) return n;
        p->sommet--;            /* sans libération : r possède désormais l'éventuel objet */
    }
    restaurer(c);                                     /* IMP-0149, D1 */
    if (cible != 0) {
        pos->ligne = cible;
        pos->decalage = 0;
    } else {
        pos->ligne = c->numero_ligne_retour;
        pos->decalage = c->offset_retour;
    }
    courant = c->appelant;
    *abandon = cible != 0 && c->position_appel == RCIM_APPEL_EXPRESSION;
    if (*abandon) pile_ramener(p, courant->hauteur_base);   /* §7.3, point B2 */
    liberer_cadre(c);
    if (cible != 0) desimbriquer(cible);
    if (a_resultat) return pile_empiler(p, &r);
    return ERR_AUCUNE;
}
