/* compilateur.c — analyse et génération en une seule passe (descente
   récursive), une ligne à la fois
   EXI: EXI-0001, EXI-0004, EXI-0011, EXI-0018, EXI-0019, EXI-0020,
        EXI-0021, EXI-0022, EXI-0028, EXI-0051, EXI-0061
   VM: VM-3201 à VM-3203, VM-3205, VM-0702
   IV: IV-IF-0001, ALG-0901, ALG-0902
   IMP: IMP-0164 (passe unique), IMP-0165 (tampon de 255 octets), IMP-0167
        (symboles), IMP-0169, IMP-0170, IMP-0171 (deux formes de ligne),
        IMP-0172 (mode machine de bureau), IMP-0180 (profils), IMP-0181
        (chaînes partagées), IMP-0182 (lexique), IMP-0188 (types), IMP-0189
        (TEM()), IMP-0190 (.n.), IMP-0191 (natures, position), IMP-0195,
        IMP-0196, IMP-0280 (constantes de 23 bits du T1600)
   Tests: tests/composants/composants_compilateur.c
   Spécification : chapitre Compilateur §4 à §9 bis.

   Étapes 2a à 2d : point d'entrée, lexique, expressions (SI en position
   d'expression compris), conditions, affectation, commentaire, SI,
   DEBUT...FIN, ALLER EN, FAIRE, TERMINER, PAUSE, RETOUR, RETOUR EN,
   RESULTAT, AFFICHER et ?, LIRE, TABLEAU, CHAINE, LIBERER, GARER, CHARGER,
   SUPPRIMER, EXECUTER, déclaration de procédure et clause LOCAL (§13),
   appel de procédure en instruction et en expression, UN et ZER (T1600).

   Jeu de caractères : la flèche d'affectation est le caractère 95 (_),
   l'exponentiation le caractère 94 (^), comme dans la restitution (fiche de
   codage RCI-M). */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "compilateur.h"
#include "symboles.h"
#include "../interfaces/types_communs.h"
#include "../interfaces/profil.h"

segment "compilateur";

#define CAR_AFFECTATION '_'
#define CAR_PUISSANCE   '^'

typedef unsigned char TypeStatique;   /* IMP-0188 */
#define TYPE_INDETERMINE 0
#define TYPE_NUMERIQUE   1
#define TYPE_CHAINE      2

/* Propriétés d'une sous-expression analysée. */
typedef struct {
    TypeStatique type;
    int          constante;   /* la sous-séquence est une seule constante numérique */
    double       valeur;      /* sa valeur, si constante */
} InfoExpression;

typedef struct {
    const char            *texte;
    unsigned int           longueur;
    unsigned int           pos;
    TamponRCIM            *t;
    int                    mode_bureau;
    ProfilImplantation     profil;
    DiagnosticCompilation *diag;
} Analyse;

/* --- erreurs ----------------------------------------------------------- */

/* Renseigne le diagnostic (la première erreur rencontrée prévaut) ; renvoie 0. */
static int erreur(Analyse *a, NatureErreur nature, unsigned int position)
{
    if (a->diag->nature == ERR_AUCUNE) {
        a->diag->nature = nature;
        a->diag->position = (unsigned char) position;
    }
    return 0;
}

/* Toute écriture passe par ce contrôle (IMP-0165). */
#define ECRIRE(a, appel) \
    do { if (!(appel)) return erreur((a), ERR_C_LIGNE_TROP_COMPLEXE, (a)->pos); } while (0)

/* --- lexique (§5 bis, IMP-0182) ---------------------------------------- */

/* Macros, sans appel de fonction par caractère (audit du code, AP-05) : leurs
   arguments sont évalués plusieurs fois et ne doivent porter aucun effet de
   bord. */
#define car(a)          ((a)->pos < (a)->longueur ? (a)->texte[(a)->pos] : '\0')
#define car_a(a, p)     ((p) < (a)->longueur ? (a)->texte[(p)] : '\0')
#define est_lettre(c)   ((c) >= 'A' && (c) <= 'Z')
#define est_chiffre(c)  ((c) >= '0' && (c) <= '9')
static int est_minuscule(char c) { return c >= 'a' && c <= 'z'; }

static void sauter_espaces(Analyse *a)
{
    while (car(a) == ' ')
        a->pos++;
}

/* Longueur de la suite de lettres et de chiffres commençant à p par une lettre. */
static unsigned int longueur_mot(const Analyse *a, unsigned int p)
{
    unsigned int q = p;
    if (!est_lettre(car_a(a, q)))
        return 0;
    while (est_lettre(car_a(a, q)) || est_chiffre(car_a(a, q)))
        q++;
    return q - p;
}

/* Premier caractère comparé avant strlen (AP-05). */
static int mot_est(const Analyse *a, unsigned int p, unsigned int lg, const char *mot)
{
    if (lg == 0 || p >= a->longueur || a->texte[p] != mot[0]) return 0;
    return strlen(mot) == lg && memcmp(a->texte + p, mot, lg) == 0;
}

/* Mots réservés (INRDP75 Annexe VI ; §8 bis) */
static const char *const reserves_communs[] = {
    "AFFICHER", "ALLER", "ALORS", "CHAINE", "CHARGER", "DEBUT", "ET",
    "EXECUTER", "FAIRE", "FIN", "GARER", "JUSQUA", "LIBERER", "LIRE", "LOCAL",
    "OU", "PAS", "PAUSE", "POUR", "PROCEDURE", "RESULTAT", "RETOUR", "SI",
    "SINON", "TABLEAU", "TANT", "TERMINER", NULL
};
static const char *const reserves_t1600[] = { "DAT", "TEM", NULL };
static const char *const reserves_mitra15[] = { "SUPPRIMER", "REPRENDRE", NULL };

/* Parcours par indice : sous #pragma debug 32, une incrémentation de
   pointeur écrite p++ provoque un arrêt (Construction §5, constaté le
   2026-10-23). */
static int dans_liste(const Analyse *a, unsigned int p, unsigned int lg, const char *const *liste)
{
    unsigned int i;
    char c0;
    if (lg == 0 || p >= a->longueur) return 0;
    c0 = a->texte[p];
    for (i = 0; liste[i] != NULL; i++)
        if (liste[i][0] == c0 && mot_est(a, p, lg, liste[i]))
            return 1;
    return 0;
}

#define RESERVE_NON     0
#define RESERVE_COMMUN  1
#define RESERVE_PROFIL  2

static int mot_reserve(const Analyse *a, unsigned int p, unsigned int lg)
{
    if (dans_liste(a, p, lg, reserves_communs))
        return RESERVE_COMMUN;
    if (PROFIL_EST_T1600(a->profil) ? dans_liste(a, p, lg, reserves_t1600)
                                     : dans_liste(a, p, lg, reserves_mitra15))
        return RESERVE_PROFIL;
    return RESERVE_NON;
}

/* Un caractère qui peut commencer un opérande (IMP-0191). */
static int debut_operande(const Analyse *a, char c)
{
    return est_lettre(c) || est_chiffre(c) || c == '\'' || c == '(' || c == '&'
        || c == '@' || (PROFIL_EST_T1600(a->profil) && c == '$')
        || (a->profil == PROFIL_T1600_F4 && c == '.');
}

static int est_operateur(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == CAR_PUISSANCE || c == '!';
}

/* Caractères du langage hors chaînes et commentaires. */
static int caractere_du_langage(char c)
{
    return est_lettre(c) || est_chiffre(c) || c == ' ' || c == '\0'
        || strchr("()[],;*/+-^_!@$#<>=.&?'", c) != NULL;
}

/* --- symboles ---------------------------------------------------------- */

static int symbole(Analyse *a, unsigned int p, unsigned int lg, NatureSymbole nature,
                   unsigned int *id)
{
    int r = symboles_chercher_ou_creer(a->texte + p, (unsigned char) lg, nature,
                                       a->mode_bureau ? ORIGINE_MACHINE_BUREAU : ORIGINE_PROGRAMME,
                                       id);
    if (r == SYMB_ERR_MEMOIRE)  return erreur(a, ERR_C_PLUS_DE_PLACE, 0);
    if (r == SYMB_ERR_CAPACITE) return erreur(a, ERR_C_CAPACITE_TABLES, p);
    return r == SYMB_OK;
}

/* --- constantes -------------------------------------------------------- */

/* Écrit une constante numérique dans le plus petit sous-format (fiche §2).
   Le sous-format est choisi sur la valeur convertie en float, celle qui est
   rangée : une valeur trop petite devient 0 (IMP-0191). Sous le profil
   T1600, la valeur est d'abord arrondie à 23 bits (IMP-0280). */
static int ecrire_nombre(Analyse *a, double v, unsigned int p)
{
    float f;
    double vf;
    long double z;

    if (PROFIL_EST_T1600(a->profil)) {
        z = v;
        rcim_arrondir_23(&z);
        v = (double) z;
    }
    if (v > FLT_MAX || v < -FLT_MAX)
        return erreur(a, ERR_C_CONSTANTE_HORS_PLAGE, p);
    f = (float) v;
    vf = (double) f;
    if (vf >= -32768.0 && vf <= 32767.0 && vf == (double) (long) vf)
        ECRIRE(a, rcim_ecrire_entier(a->t, (long) vf));
    else
        ECRIRE(a, rcim_ecrire_reel(a->t, f));
    return 1;
}

/* Constante numérique : chiffres, point, exposant E signé (LSE15 §3-1.1). */
static int constante_numerique(Analyse *a, InfoExpression *info)
{
    unsigned int debut = a->pos;
    char tampon[COMPILATEUR_LONGUEUR_MAX + 1];
    unsigned int lg;
    unsigned int i;
    int entier;
    long n;
    double v;

    while (est_chiffre(car(a))) a->pos++;
    entier = a->pos > debut && a->pos - debut <= 5;
    if (car(a) == '.') {
        entier = 0;
        a->pos++;
        while (est_chiffre(car(a))) a->pos++;
    }
    if (car(a) == 'E') {
        entier = 0;
        a->pos++;
        if (car(a) == '+' || car(a) == '-') a->pos++;
        if (!est_chiffre(car(a)))
            return erreur(a, ERR_C_CONSTANTE_MAL_FORMEE, debut);
        while (est_chiffre(car(a))) a->pos++;
    }
    /* Cinq chiffres au plus, sans point ni exposant, jusqu'à 32 767 : le
       sous-format entier qu'ecrire_nombre choisirait, sans strtod ni calcul
       flottant (audit du code, AP-11). */
    if (entier) {
        n = 0;
        for (i = debut; i < a->pos; i++)
            n = n * 10 + (a->texte[i] - '0');
        if (n <= 32767) {
            info->type = TYPE_NUMERIQUE;
            info->constante = 1;
            info->valeur = (double) n;
            ECRIRE(a, rcim_ecrire_entier(a->t, n));
            return 1;
        }
    }
    lg = a->pos - debut;
    memcpy(tampon, a->texte + debut, lg);
    tampon[lg] = '\0';
    v = strtod(tampon, NULL);
    if (v > FLT_MAX)
        return erreur(a, ERR_C_CONSTANTE_HORS_PLAGE, debut);

    info->type = TYPE_NUMERIQUE;
    info->constante = 1;
    info->valeur = v;
    return ecrire_nombre(a, v, debut);
}

/* Texte d'une constante-chaîne, opérande ou libellé de format : morceaux entre
   apostrophes et, sous F4, équivalents numériques .n. accolés (IMP-0190).
   texte reçoit au plus COMPILATEUR_LONGUEUR_MAX caractères. */
static int lire_chaine(Analyse *a, unsigned char *texte, unsigned int *longueur)
{
    unsigned int lg = 0;
    unsigned int debut;
    unsigned long n;

    for (;;) {
        debut = a->pos;
        if (car(a) == '\'') {
            a->pos++;
            for (;;) {
                if (a->pos >= a->longueur)
                    return erreur(a, ERR_C_CHAINE_NON_TERMINEE, debut);
                if (car(a) == '\'') {
                    if (car_a(a, a->pos + 1) == '\'') {   /* apostrophe doublée */
                        texte[lg++] = '\'';
                        a->pos += 2;
                        continue;
                    }
                    a->pos++;
                    break;
                }
                texte[lg++] = (unsigned char) car(a);
                a->pos++;
            }
        } else if (car(a) == '.' && a->profil == PROFIL_T1600_F4) {
            a->pos++;
            if (!est_chiffre(car(a)))
                return erreur(a, ERR_C_EQUIVALENT_NON_TERMINE, debut);
            for (;;) {
                unsigned int p_entier = a->pos;
                n = 0;
                while (est_chiffre(car(a))) {
                    if (n < 1000) n = n * 10 + (unsigned long) (car(a) - '0');
                    a->pos++;
                }
                if (n > 127)   /* bornes d'EQC sous T1600 (INRDP75 l. 1416) */
                    return erreur(a, ERR_C_EQUIVALENT_HORS_BORNES, p_entier);
                texte[lg++] = (unsigned char) n;
                if (car(a) == '.') { a->pos++; break; }
                if (car(a) != ' ')
                    return erreur(a, ERR_C_EQUIVALENT_NON_TERMINE, debut);
                sauter_espaces(a);
                if (!est_chiffre(car(a)))
                    return erreur(a, ERR_C_EQUIVALENT_NON_TERMINE, debut);
            }
        } else
            break;
    }
    *longueur = lg;
    return 1;
}

/* Constante-chaîne en opérande d'expression. */
static int constante_chaine(Analyse *a, InfoExpression *info)
{
    unsigned char texte[COMPILATEUR_LONGUEUR_MAX];
    unsigned int lg;
    unsigned int index;
    int r;

    if (!lire_chaine(a, texte, &lg)) return 0;
    info->type = TYPE_CHAINE;
    info->constante = 0;
    if (!a->mode_bureau && lg > CONSTANTES_SEUIL_PARTAGE) {     /* IMP-0181 */
        r = constantes_chercher_ou_creer(texte, (unsigned char) lg, &index);
        if (r == SYMB_ERR_MEMOIRE)  return erreur(a, ERR_C_PLUS_DE_PLACE, 0);
        if (r == SYMB_ERR_CAPACITE) return erreur(a, ERR_C_CAPACITE_TABLES, a->pos);
        ECRIRE(a, rcim_ecrire_chaine_index(a->t, index));
    } else
        ECRIRE(a, rcim_ecrire_chaine_en_ligne(a->t, texte, lg));
    return 1;
}

/* --- expressions (§5, IMP-0164, IMP-0188) ------------------------------- */

static int expression(Analyse *a, InfoExpression *info);
static int ou_logique(Analyse *a);
static int instruction(Analyse *a);

/* Mot du langage attendu à la position courante (IMP-0182) : 1 s'il y est,
   suivi d'un espace, d'un point-virgule ou de la fin de la ligne, et il est
   alors consommé ; 0 s'il n'y est pas ; -1 s'il y est suivi d'un autre
   caractère (diagnostic C 53). */
static int prendre_mot(Analyse *a, const char *mot)
{
    unsigned int p;
    unsigned int lg;
    char t;

    sauter_espaces(a);
    p = a->pos;
    if (car_a(a, p) != mot[0])
        return 0;
    lg = longueur_mot(a, p);
    if (lg == 0 || !mot_est(a, p, lg, mot))
        return 0;
    t = car_a(a, p + lg);
    if (t != ' ' && t != ';' && t != '\0') {
        erreur(a, ERR_C_MOT_MAL_TERMINE, p);
        return -1;
    }
    a->pos = p + lg;
    return 1;
}

/* Vrai si le mot se trouve à la position courante, sans le consommer. */
static int voir_mot(Analyse *a, const char *mot)
{
    unsigned int p;
    sauter_espaces(a);
    p = a->pos;
    if (car_a(a, p) != mot[0])
        return 0;
    return mot_est(a, p, longueur_mot(a, p), mot);
}

/* Contrôle qu'un opérande de type déterminé convient (IMP-0188). */
static int exiger_nombre(Analyse *a, const InfoExpression *e, unsigned int p)
{
    return e->type == TYPE_CHAINE ? erreur(a, ERR_C_CHAINE_AU_LIEU_NOMBRE, p) : 1;
}

static int exiger_chaine(Analyse *a, const InfoExpression *e, unsigned int p)
{
    return e->type == TYPE_NUMERIQUE ? erreur(a, ERR_C_NOMBRE_AU_LIEU_CHAINE, p) : 1;
}

/* Erreur à la place d'un opérande attendu. */
static int operande_absent(Analyse *a)
{
    char c = car(a);
    if (est_minuscule(c))          return erreur(a, ERR_C_MINUSCULE, a->pos);
    if (!caractere_du_langage(c))  return erreur(a, ERR_C_CARACTERE_INTERDIT, a->pos);
    if (est_operateur(c))          return erreur(a, ERR_C_OPERATEURS_ADJACENTS, a->pos);
    return erreur(a, ERR_C_OPERANDE_MANQUANT, a->pos);
}

/* Indices d'une variable indicée : '[' déjà vu ; 1 ou 2 expressions. */
static int indices(Analyse *a, unsigned char *nb)
{
    InfoExpression e;
    unsigned int p;

    *nb = 0;
    a->pos++;                                   /* '[' */
    for (;;) {
        sauter_espaces(a);
        p = a->pos;
        if (!expression(a, &e)) return 0;
        if (e.type == TYPE_CHAINE) return erreur(a, ERR_C_INDICE_NON_NUMERIQUE, p);
        (*nb)++;
        sauter_espaces(a);
        if (car(a) == ']') { a->pos++; return 1; }
        if (car(a) == ',') {
            if (*nb == 2) return erreur(a, ERR_C_TROP_D_INDICES, a->pos);
            a->pos++;
            continue;
        }
        return erreur(a, ERR_C_CROCHET_MANQUANT, a->pos);
    }
}

/* Référence à une variable, simple ou indicée ; p, lg : nom déjà reconnu. */
static int reference_variable(Analyse *a, unsigned int p, unsigned int lg, InfoExpression *info)
{
    unsigned int id;
    unsigned char nb;
    int reserve = mot_reserve(a, p, lg);

    if (reserve == RESERVE_COMMUN) return erreur(a, ERR_C_MOT_RESERVE, p);
    if (reserve == RESERVE_PROFIL) return erreur(a, ERR_C_MOT_RESERVE_PROFIL, p);
    if (lg > SYMBOLES_NOM_MAX)     return erreur(a, ERR_C_IDENTIFICATEUR_TROP_LONG, p);
    if (!symbole(a, p, lg, SYM_IDENTIFICATEUR, &id)) return 0;
    a->pos = p + lg;
    sauter_espaces(a);
    if (car(a) == '[') {
        if (!indices(a, &nb)) return 0;
        ECRIRE(a, rcim_ecrire_reference_indexee(a->t, id, nb));
        info->type = TYPE_NUMERIQUE;             /* tableaux numériques (LSE15 l. 202) */
    } else {
        ECRIRE(a, rcim_ecrire_reference(a->t, id));
        info->type = TYPE_INDETERMINE;
    }
    info->constante = 0;
    return 1;
}

/* Fonctions usuelles (fiche RCI-M §4 ; signatures de la colonne « Pile »).
   Signature : v nombre, c chaîne, x quelconque, r identificateur. */
#define FN_COMMUN  0
#define FN_T1600   1
typedef struct {
    const char   *nom;
    TagRCIM       op;
    unsigned char min;
    unsigned char max;
    const char   *signature;
    TypeStatique  resultat;
    unsigned char avec_n;     /* charge utile u8 n (IMP-0122) */
    unsigned char profil;
} Fonction;

static const Fonction fonctions[] = {
    { "RAC", RCIM_OP_RAC, 1, 1, "v",    TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "ABS", RCIM_OP_ABS, 1, 1, "v",    TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "EXP", RCIM_OP_EXP, 1, 1, "v",    TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "LGN", RCIM_OP_LGN, 1, 1, "v",    TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "SIN", RCIM_OP_SIN, 1, 1, "v",    TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "COS", RCIM_OP_COS, 1, 1, "v",    TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "ATG", RCIM_OP_ATG, 1, 1, "v",    TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "ENT", RCIM_OP_ENT, 1, 1, "v",    TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "ALE", RCIM_OP_ALE, 1, 1, "v",    TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "TEM", RCIM_OP_TEM, 0, 0, "",     TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "LGR", RCIM_OP_LGR, 1, 1, "c",    TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "POS", RCIM_OP_POS, 3, 3, "cvc",  TYPE_NUMERIQUE, 0, FN_COMMUN },
    { "SKP", RCIM_OP_SKP, 2, 3, "cvc",  TYPE_NUMERIQUE, 1, FN_COMMUN },
    { "PTR", RCIM_OP_PTR, 2, 3, "cvx",  TYPE_NUMERIQUE, 1, FN_COMMUN },
    { "CNB", RCIM_OP_CNB, 2, 3, "cvr",  TYPE_NUMERIQUE, 1, FN_COMMUN },
    { "EQN", RCIM_OP_EQN, 1, 2, "cv",   TYPE_NUMERIQUE, 1, FN_COMMUN },
    { "SCH", RCIM_OP_SCH, 3, 4, "cvxr", TYPE_CHAINE,    1, FN_COMMUN },
    { "GRL", RCIM_OP_GRL, 2, 3, "cvr",  TYPE_CHAINE,    1, FN_COMMUN },
    { "CCA", RCIM_OP_CCA, 1, 1, "v",    TYPE_CHAINE,    0, FN_COMMUN },
    { "EQC", RCIM_OP_EQC, 1, 1, "v",    TYPE_CHAINE,    0, FN_COMMUN },
    { "DAT", RCIM_OP_DAT, 0, 0, "",     TYPE_CHAINE,    0, FN_COMMUN },
    { "SH",  RCIM_OP_SH,  1, 1, "v",    TYPE_NUMERIQUE, 0, FN_T1600 },
    { "CH",  RCIM_OP_CH,  1, 1, "v",    TYPE_NUMERIQUE, 0, FN_T1600 },
    { "TH",  RCIM_OP_TH,  1, 1, "v",    TYPE_NUMERIQUE, 0, FN_T1600 },
    { "SGN", RCIM_OP_SGN, 1, 1, "v",    TYPE_NUMERIQUE, 0, FN_T1600 },
    { NULL,  0,           0, 0, NULL,   0,              0, 0 }
};

static const Fonction *chercher_fonction(const Analyse *a, unsigned int p, unsigned int lg)
{
    unsigned int i;
    if (lg < 2 || lg > 3) return NULL;     /* noms des fonctions : 2 ou 3 lettres (AP-05) */
    for (i = 0; fonctions[i].nom != NULL; i++)
        if (mot_est(a, p, lg, fonctions[i].nom))
            return &fonctions[i];
    return NULL;
}

/* Appel d'une fonction usuelle ; a->pos est sur '(' (ou après le nom pour
   TEM et DAT sans parenthèses sous T1600). */
static int appel_fonction(Analyse *a, const Fonction *f, unsigned int p_nom, InfoExpression *info)
{
    InfoExpression e;
    unsigned char k = 0;
    unsigned int p;
    unsigned int lg;
    unsigned int id;
    char s;

    if (car(a) == '(') {
        a->pos++;
        sauter_espaces(a);
        if (car(a) == ')') {
            a->pos++;
        } else {
            for (;;) {
                if (k >= f->max) return erreur(a, ERR_C_ARGUMENTS_FONCTION, a->pos);
                s = f->signature[k];
                sauter_espaces(a);
                p = a->pos;
                if (s == 'r') {                       /* identificateur seul */
                    lg = longueur_mot(a, p);
                    if (lg == 0 || mot_reserve(a, p, lg) != RESERVE_NON)
                        return erreur(a, ERR_C_ARGUMENT_FACULTATIF, p);
                    if (lg > SYMBOLES_NOM_MAX) return erreur(a, ERR_C_IDENTIFICATEUR_TROP_LONG, p);
                    a->pos = p + lg;
                    sauter_espaces(a);
                    if (car(a) != ',' && car(a) != ')')
                        return erreur(a, ERR_C_ARGUMENT_FACULTATIF, p);
                    if (!symbole(a, p, lg, SYM_IDENTIFICATEUR, &id)) return 0;
                    ECRIRE(a, rcim_ecrire_reference(a->t, id));
                } else {
                    if (!expression(a, &e)) return 0;
                    if (s == 'v' && !exiger_nombre(a, &e, p)) return 0;
                    if (s == 'c' && !exiger_chaine(a, &e, p)) return 0;
                }
                k++;
                sauter_espaces(a);
                if (car(a) == ',') { a->pos++; continue; }
                if (car(a) == ')') { a->pos++; break; }
                return erreur(a, ERR_C_ARGUMENTS_FONCTION, a->pos);
            }
        }
        if (k < f->min) return erreur(a, ERR_C_ARGUMENTS_FONCTION, p_nom);
    }
    if (f->avec_n)
        ECRIRE(a, rcim_ecrire_tag_u8(a->t, f->op, k));
    else
        ECRIRE(a, rcim_ecrire_tag(a->t, f->op));
    info->type = f->resultat;
    info->constante = 0;
    return 1;
}

/* Appel de procédure, en position d'expression ou d'instruction (§13.5,
   IMP-0121, IMP-0177) : 40 idP [a1]...[ak] A8 position k */
static int appel_procedure(Analyse *a, InfoExpression *info, unsigned char position)
{
    unsigned int p = a->pos;     /* '&' */
    unsigned int lg;
    unsigned int id;
    unsigned int pa;
    unsigned char k = 0;
    InfoExpression e;

    if (a->mode_bureau)                                  /* IMP-0172, grammaire [K] */
        return erreur(a, position == RCIM_APPEL_EXPRESSION ? ERR_C_BUREAU_APPEL_FONCTION
                                                           : ERR_C_BUREAU_APPEL_INSTRUCTION, p);
    lg = longueur_mot(a, p + 1);
    if (lg == 0) return erreur(a, ERR_C_NOM_PROCEDURE_INCORRECT, p);
    if (lg > SYMBOLES_NOM_MAX) return erreur(a, ERR_C_NOM_PROCEDURE_TROP_LONG, p);
    if (!symbole(a, p + 1, lg, SYM_PROCEDURE, &id)) return 0;
    a->pos = p + 1 + lg;
    sauter_espaces(a);
    if (car(a) != '(') return erreur(a, ERR_C_APPEL_MAL_FORME, a->pos);
    a->pos++;
    ECRIRE(a, rcim_ecrire_reference(a->t, id));
    sauter_espaces(a);
    if (car(a) == ')') {
        a->pos++;
    } else {
        for (;;) {
            sauter_espaces(a);
            pa = a->pos;
            /* un nom de procédure seul est un argument (grammaire effectiveParam) */
            if (car(a) == '&') {
                unsigned int lgp = longueur_mot(a, pa + 1);
                unsigned int q = pa + 1 + lgp;
                while (car_a(a, q) == ' ') q++;
                if (lgp > 0 && (car_a(a, q) == ',' || car_a(a, q) == ')')) {
                    unsigned int idp;
                    if (lgp > SYMBOLES_NOM_MAX) return erreur(a, ERR_C_NOM_PROCEDURE_TROP_LONG, pa);
                    if (!symbole(a, pa + 1, lgp, SYM_PROCEDURE, &idp)) return 0;
                    ECRIRE(a, rcim_ecrire_reference(a->t, idp));
                    a->pos = q;
                    goto argument_suivant;
                }
            }
            if (!expression(a, &e)) return 0;
argument_suivant:
            if (k == 255) return erreur(a, ERR_C_APPEL_MAL_FORME, pa);
            k++;
            sauter_espaces(a);
            if (car(a) == ',') { a->pos++; continue; }
            if (car(a) == ')') { a->pos++; break; }
            return erreur(a, ERR_C_APPEL_MAL_FORME, a->pos);
        }
    }
    ECRIRE(a, rcim_ecrire_appel(a->t, position, k));
    info->type = TYPE_INDETERMINE;
    info->constante = 0;
    return 1;
}

/* atome : ( expr ) | fonction | appel de procédure | variable | constante |
   @ | $ (grammaire atom ; §8 bis). */
static int atome(Analyse *a, InfoExpression *info)
{
    unsigned int p;
    unsigned int lg;
    unsigned int q;
    unsigned long n;
    const Fonction *f;
    char c;

    sauter_espaces(a);
    p = a->pos;
    c = car(a);

    if (c == '(') {
        a->pos++;
        if (!expression(a, info)) return 0;
        sauter_espaces(a);
        if (car(a) != ')') return erreur(a, ERR_C_PARENTHESE_MANQUANTE, a->pos);
        a->pos++;
        return 1;
    }
    if (est_chiffre(c))
        return constante_numerique(a, info);
    if (c == '\'' || (c == '.' && a->profil == PROFIL_T1600_F4))
        return constante_chaine(a, info);
    if (c == '.')                                         /* .n. hors de F4 */
        return erreur(a, ERR_C_HORS_PROFIL, p);
    if (c == '&')
        return appel_procedure(a, info, RCIM_APPEL_EXPRESSION);
    if (c == '@') {
        a->pos++;
        info->type = TYPE_NUMERIQUE;
        info->constante = 0;
        if (a->profil == PROFIL_MITRA15_V8) {             /* constante numéro de ligne */
            if (!est_chiffre(car(a))) return erreur(a, ERR_C_AROBASE_SANS_ENTIER, p);
            n = 0;
            while (est_chiffre(car(a))) {
                if (n < 1000) n = n * 10 + (unsigned long) (car(a) - '0');
                a->pos++;
            }
            if (n < 1 || n > 250) return erreur(a, ERR_C_NUMERO_HORS_BORNES, p + 1);
            ECRIRE(a, rcim_ecrire_tag_u8(a->t, RCIM_VAL_LIGNE, (unsigned char) n));
            return 1;
        }
        if (est_chiffre(car(a)))                         /* sens MITRA 15 sous T1600 */
            return erreur(a, ERR_C_HORS_PROFIL, p);
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_REF_LIGNE_COURANTE));
        return 1;
    }
    if (c == '$') {
        if (!PROFIL_EST_T1600(a->profil)) return erreur(a, ERR_C_HORS_PROFIL, p);
        a->pos++;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_REF_LIGNE_RETOUR));
        info->type = TYPE_NUMERIQUE;
        info->constante = 0;
        return 1;
    }
    if (est_lettre(c)) {
        lg = longueur_mot(a, p);
        f = chercher_fonction(a, p, lg);
        if (f != NULL) {
            q = p + lg;
            while (car_a(a, q) == ' ') q++;
            /* TEM et DAT sous T1600 : sans parenthèses ; admises en outre sous F4 (IMP-0189) */
            if (PROFIL_EST_T1600(a->profil) && (f->op == RCIM_OP_TEM || f->op == RCIM_OP_DAT)) {
                if (car_a(a, q) == '(') {
                    if (a->profil != PROFIL_T1600_F4) return erreur(a, ERR_C_HORS_PROFIL, p);
                    a->pos = q;
                } else
                    a->pos = p + lg;
                return appel_fonction(a, f, p, info);
            }
            if (car_a(a, q) == '(') {
                if (f->profil == FN_T1600 && !PROFIL_EST_T1600(a->profil))
                    return erreur(a, ERR_C_HORS_PROFIL, p);
                a->pos = q;
                return appel_fonction(a, f, p, info);
            }
        }
        return reference_variable(a, p, lg, info);
    }
    return operande_absent(a);
}

/* puissance : atome (^ atome)*, associativité à gauche (LSE15 §3-3.4). */
static int puissance(Analyse *a, InfoExpression *info)
{
    InfoExpression d;
    unsigned int p;

    sauter_espaces(a);
    p = a->pos;
    if (!atome(a, info)) return 0;
    for (;;) {
        sauter_espaces(a);
        if (car(a) != CAR_PUISSANCE) return 1;
        if (!exiger_nombre(a, info, p)) return 0;
        a->pos++;
        sauter_espaces(a);
        p = a->pos;
        if (!atome(a, &d)) return 0;
        if (!exiger_nombre(a, &d, p)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_OP_PUIS));
        info->type = TYPE_NUMERIQUE;
        info->constante = 0;
    }
}

/* terme : puissance ((* | /) puissance)* */
static int terme(Analyse *a, InfoExpression *info)
{
    InfoExpression d;
    unsigned int p;
    char op;

    sauter_espaces(a);
    p = a->pos;
    if (!puissance(a, info)) return 0;
    for (;;) {
        sauter_espaces(a);
        op = car(a);
        if (op != '*' && op != '/') return 1;
        if (!exiger_nombre(a, info, p)) return 0;
        a->pos++;
        sauter_espaces(a);
        p = a->pos;
        if (!puissance(a, &d)) return 0;
        if (!exiger_nombre(a, &d, p)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, op == '*' ? RCIM_OP_MUL : RCIM_OP_DIV));
        info->type = TYPE_NUMERIQUE;
        info->constante = 0;
    }
}

/* terme signé : (+ | -)? terme ; repli d'un changement de signe sur une
   constante seule (§5, « Repli sur une constante »). */
static int terme_signe(Analyse *a, InfoExpression *info)
{
    char signe;
    unsigned int p;
    unsigned int debut = a->t->curseur;

    sauter_espaces(a);
    signe = car(a);
    if (signe != '+' && signe != '-')
        return terme(a, info);
    a->pos++;
    sauter_espaces(a);
    p = a->pos;
    if (!terme(a, info)) return 0;
    if (!exiger_nombre(a, info, p)) return 0;
    if (signe == '-') {
        if (info->constante) {
            a->t->curseur = debut;                 /* constante négative */
            info->valeur = -info->valeur;
            if (!ecrire_nombre(a, info->valeur, p)) return 0;
        } else
            ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_OP_NEG));
    }
    info->type = TYPE_NUMERIQUE;
    return 1;
}

/* somme : terme_signe ((+ | -) terme)* */
static int somme(Analyse *a, InfoExpression *info)
{
    InfoExpression d;
    unsigned int p;
    char op;

    sauter_espaces(a);
    p = a->pos;
    if (!terme_signe(a, info)) return 0;
    for (;;) {
        sauter_espaces(a);
        op = car(a);
        if (op != '+' && op != '-') return 1;
        if (!exiger_nombre(a, info, p)) return 0;
        a->pos++;
        sauter_espaces(a);
        p = a->pos;
        if (!terme(a, &d)) return 0;
        if (!exiger_nombre(a, &d, p)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, op == '+' ? RCIM_OP_ADD : RCIM_OP_SUB));
        info->type = TYPE_NUMERIQUE;
        info->constante = 0;
    }
}

/* concaténation : somme (! somme)* (LSE15 §4-4.1). */
static int concatenation(Analyse *a, InfoExpression *info)
{
    InfoExpression d;
    unsigned int p;

    sauter_espaces(a);
    p = a->pos;
    if (!somme(a, info)) return 0;
    for (;;) {
        sauter_espaces(a);
        if (car(a) != '!') return 1;
        if (!exiger_chaine(a, info, p)) return 0;
        a->pos++;
        sauter_espaces(a);
        p = a->pos;
        if (!somme(a, &d)) return 0;
        if (!exiger_chaine(a, &d, p)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_OP_CONCAT));
        info->type = TYPE_CHAINE;
        info->constante = 0;
    }
}

/* expression : concaténation | SI condition ALORS expression SINON expression
   (RCI-M §4.8.5, fiche §7) : [c] C0 @S [e1] C1 @F @S:[e2] @F: */
static int expression(Analyse *a, InfoExpression *info)
{
    InfoExpression e2;
    unsigned int si;
    unsigned int saut;
    int r;

    r = prendre_mot(a, "SI");
    if (r < 0) return 0;
    if (r == 0) return concatenation(a, info);
    if (!ou_logique(a)) return 0;
    r = prendre_mot(a, "ALORS");
    if (r < 0) return 0;
    if (r == 0) return erreur(a, ERR_C_ALORS_MANQUANT, a->pos);
    ECRIRE(a, rcim_ecrire_branchement(a->t, RCIM_CTL_SI_FAUX, &si));
    if (!expression(a, info)) return 0;
    r = prendre_mot(a, "SINON");
    if (r < 0) return 0;
    if (r == 0) return erreur(a, ERR_C_SINON_MANQUANT, a->pos);
    ECRIRE(a, rcim_ecrire_branchement(a->t, RCIM_CTL_SAUT, &saut));
    rcim_corriger(a->t, si);
    if (!expression(a, &e2)) return 0;
    rcim_corriger(a->t, saut);
    if (info->type != e2.type)                      /* IMP-0188 */
        info->type = TYPE_INDETERMINE;
    info->constante = 0;
    return 1;
}

/* --- conditions (grammaire booleanExpr) ----------------------------------- */

/* condition : expression relation expression (LSE15 l. 1048 : IMP-0188). */
static int condition(Analyse *a)
{
    InfoExpression g;
    InfoExpression d;
    unsigned int pd;
    TagRCIM op;
    char c;

    if (!expression(a, &g)) return 0;
    sauter_espaces(a);
    c = car(a);
    if (c == '=')      { op = RCIM_OP_EGAL; a->pos++; }
    else if (c == '#') { op = RCIM_OP_DIFF; a->pos++; }
    else if (c == '<') {
        a->pos++;
        if (car(a) == '=') { op = RCIM_OP_INF_EGAL; a->pos++; } else op = RCIM_OP_INF;
    } else if (c == '>') {
        a->pos++;
        if (car(a) == '=') { op = RCIM_OP_SUP_EGAL; a->pos++; } else op = RCIM_OP_SUP;
    } else
        return erreur(a, ERR_C_RELATION_MANQUANTE, a->pos);
    sauter_espaces(a);
    pd = a->pos;
    if (!expression(a, &d)) return 0;
    if (g.type != TYPE_INDETERMINE && d.type != TYPE_INDETERMINE && g.type != d.type)
        return erreur(a, ERR_C_COMPARAISON_TYPES, pd);
    ECRIRE(a, rcim_ecrire_tag(a->t, op));
    return 1;
}

static int est_relation(char c)
{
    return c == '=' || c == '#' || c == '<' || c == '>';
}

/* facteur logique : ( condition logique ) | condition. Une parenthèse
   ouvrante peut aussi commencer une expression arithmétique ((A+B)>C) :
   on essaie d'abord la condition entre parenthèses, et l'on revient en
   arrière si elle n'en est pas une. */
static int facteur_logique(Analyse *a)
{
    unsigned int pos;
    unsigned int curseur;
    DiagnosticCompilation diag;

    sauter_espaces(a);
    if (car(a) == '(') {
        pos = a->pos;
        curseur = a->t->curseur;
        diag = *a->diag;
        a->pos++;
        if (ou_logique(a)) {
            sauter_espaces(a);
            if (car(a) == ')') {
                a->pos++;
                sauter_espaces(a);
                if (!est_relation(car(a)) && !est_operateur(car(a)))
                    return 1;
            }
        }
        a->pos = pos;                               /* retour en arrière */
        a->t->curseur = curseur;
        *a->diag = diag;
    }
    return condition(a);
}

static int et_logique(Analyse *a)
{
    int r;

    if (!facteur_logique(a)) return 0;
    for (;;) {
        r = prendre_mot(a, "ET");
        if (r < 0) return 0;
        if (r == 0) return 1;
        if (!facteur_logique(a)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_OP_ET));
    }
}

/* condition logique : ET avant OU (INRDP75 ; LSE15 §5). */
static int ou_logique(Analyse *a)
{
    int r;

    if (!et_logique(a)) return 0;
    for (;;) {
        r = prendre_mot(a, "OU");
        if (r < 0) return 0;
        if (r == 0) return 1;
        if (!et_logique(a)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_OP_OU));
    }
}

/* --- instructions -------------------------------------------------------- */

/* Second segment du compilateur (IMP-0197) : le code dépasserait sinon les
   64 Kio d'un segment. Lexique, expressions et conditions restent dans
   "compilateur" ; les instructions et le point d'entrée sont dans
   "compil_ins". */
segment "compil_ins";

/* Affectation : variable (indicée ou non) _ expression (V1, V13). */
static int affectation(Analyse *a, unsigned int p, unsigned int lg)
{
    InfoExpression cible;
    InfoExpression e;

    if (!reference_variable(a, p, lg, &cible)) return 0;
    sauter_espaces(a);
    if (car(a) != CAR_AFFECTATION)
        return erreur(a, ERR_C_AFFECTATION_MANQUANTE, a->pos);
    a->pos++;
    if (!expression(a, &e)) return 0;
    ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_OP_AFFECTER));
    return 1;
}

/* Commentaire : '*' jusqu'à la fin de la ligne (RCI-M §4.9, IMP-0113). */
static int commentaire(Analyse *a)
{
    unsigned int debut = a->pos + 1;
    ECRIRE(a, rcim_ecrire_commentaire(a->t, (const unsigned char *) a->texte + debut,
                                      a->longueur - debut));
    a->pos = a->longueur;
    return 1;
}

/* SI condition ALORS instruction {SINON instruction} (RCI-M §4.8.4) :
   [c] C0 @S [i1] C1 @F @S:[i2] @F:  ou  [c] C0 @F [i1] @F: */
static int instruction_si(Analyse *a)
{
    unsigned int si;
    unsigned int saut;
    int r;

    if (!ou_logique(a)) return 0;
    r = prendre_mot(a, "ALORS");
    if (r < 0) return 0;
    if (r == 0) return erreur(a, ERR_C_ALORS_MANQUANT, a->pos);
    ECRIRE(a, rcim_ecrire_branchement(a->t, RCIM_CTL_SI_FAUX, &si));
    if (!voir_mot(a, "SINON") && !instruction(a)) return 0;
    r = prendre_mot(a, "SINON");
    if (r < 0) return 0;
    if (r == 0) {
        rcim_corriger(a->t, si);
        return 1;
    }
    ECRIRE(a, rcim_ecrire_branchement(a->t, RCIM_CTL_SAUT, &saut));
    rcim_corriger(a->t, si);
    if (!instruction(a)) return 0;
    rcim_corriger(a->t, saut);
    return 1;
}

/* DEBUT instruction {; instruction} FIN : aucun élément propre. */
static int instruction_debut(Analyse *a)
{
    int r;

    for (;;) {
        r = prendre_mot(a, "FIN");
        if (r < 0) return 0;
        if (r) return 1;
        if (!instruction(a)) return 0;
        sauter_espaces(a);
        if (car(a) == ';') { a->pos++; continue; }
        r = prendre_mot(a, "FIN");
        if (r < 0) return 0;
        if (r) return 1;
        return erreur(a, ERR_C_FIN_MANQUANT, a->pos);
    }
}

/* Cible d'ALLER EN ou de RETOUR EN (IMP-0111) : constante entière dans les
   numéros du profil, sinon cible calculée. */
static int cible_ligne(Analyse *a, TagRCIM constante, TagRCIM calculee)
{
    InfoExpression e;
    unsigned int p;
    unsigned int curseur;
    double borne;

    sauter_espaces(a);
    p = a->pos;
    curseur = a->t->curseur;
    if (!expression(a, &e)) return 0;
    if (!exiger_nombre(a, &e, p)) return 0;
    borne = PROFIL_EST_T1600(a->profil) ? 255.0 : 250.0;
    if (e.constante && e.valeur >= 1.0 && e.valeur <= borne
        && e.valeur == (double) (long) e.valeur) {
        a->t->curseur = curseur;
        ECRIRE(a, rcim_ecrire_tag_u8(a->t, constante, (unsigned char) (long) e.valeur));
    } else
        ECRIRE(a, rcim_ecrire_tag(a->t, calculee));
    return 1;
}

/* Expression numérique (IMP-0188). */
static int expression_numerique(Analyse *a)
{
    InfoExpression e;
    unsigned int p;

    sauter_espaces(a);
    p = a->pos;
    if (!expression(a, &e)) return 0;
    return exiger_nombre(a, &e, p);
}

/* Clause TANT QUE c de FAIRE, le mot TANT étant lu : [c] D0 */
static int clause_tant_que(Analyse *a)
{
    int r = prendre_mot(a, "QUE");
    if (r < 0) return 0;
    if (r == 0) return erreur(a, ERR_C_TANT_SANS_QUE, a->pos);
    if (!ou_logique(a)) return 0;
    ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_CTL_FAIRE_TANT_QUE));
    return 1;
}

/* FAIRE n1 POUR V←v1 {PAS p} JUSQUA v2 | TANT QUE c (§6, IMP-0112) :
   [n1] [v1] C9 idV {[p] CA | CE} {[v2] CF | [c] D0}
   FAIRE n1 TANT QUE c, sous F4 (IMP-0277) : [n1] D2 [c] D0 */
static int instruction_faire(Analyse *a)
{
    unsigned int p;
    unsigned int lg;
    unsigned int id;
    int reserve;
    int r;

    if (!expression_numerique(a)) return 0;                 /* n1 */
    r = prendre_mot(a, "POUR");
    if (r < 0) return 0;
    if (r == 0) {
        if (a->profil == PROFIL_T1600_F4) {
            r = prendre_mot(a, "TANT");
            if (r < 0) return 0;
            if (r) {
                ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_CTL_FAIRE_OUVRIR_SEUL));
                return clause_tant_que(a);
            }
        }
        return erreur(a, ERR_C_POUR_MANQUANT, a->pos);
    }
    sauter_espaces(a);
    p = a->pos;
    lg = longueur_mot(a, p);
    if (lg == 0) return erreur(a, ERR_C_VARIABLE_BOUCLE, p);
    reserve = mot_reserve(a, p, lg);
    if (reserve == RESERVE_COMMUN) return erreur(a, ERR_C_MOT_RESERVE, p);
    if (reserve == RESERVE_PROFIL) return erreur(a, ERR_C_MOT_RESERVE_PROFIL, p);
    if (lg > SYMBOLES_NOM_MAX)     return erreur(a, ERR_C_IDENTIFICATEUR_TROP_LONG, p);
    a->pos = p + lg;
    sauter_espaces(a);
    if (car(a) == '[') return erreur(a, ERR_C_VARIABLE_BOUCLE, p);   /* variable simple */
    if (!symbole(a, p, lg, SYM_IDENTIFICATEUR, &id)) return 0;
    if (car(a) != CAR_AFFECTATION) return erreur(a, ERR_C_AFFECTATION_BOUCLE, a->pos);
    a->pos++;
    if (!expression_numerique(a)) return 0;                 /* v1 */
    ECRIRE(a, rcim_ecrire_tag_id(a->t, RCIM_CTL_FAIRE_OUVRIR, id));
    r = prendre_mot(a, "PAS");
    if (r < 0) return 0;
    if (r) {
        if (!expression_numerique(a)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_CTL_FAIRE_PAS));
    } else
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_CTL_FAIRE_PAS_IMPLICITE));
    r = prendre_mot(a, "JUSQUA");
    if (r < 0) return 0;
    if (r) {
        if (!expression_numerique(a)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_CTL_FAIRE_JUSQUA));
        return 1;
    }
    r = prendre_mot(a, "TANT");
    if (r < 0) return 0;
    if (r) return clause_tant_que(a);
    return erreur(a, ERR_C_FIN_DE_BOUCLE, a->pos);
}

/* --- AFFICHER et ? (§5, « Génération d'AFFICHER » ; IMP-0115, IMP-0116,
   IMP-0178) ---------------------------------------------------------------- */

#define FMT_MAX 40   /* une ligne de 79 caractères n'en porte pas davantage */

#define SPEC_LIBELLE      0
#define SPEC_MISE_EN_PAGE 1
#define SPEC_DESCRIPTION  2

typedef struct {
    unsigned char genre;
    TagRCIM       tag;         /* mise en page : B1 à B4 */
    unsigned char variable;    /* facteur * */
    unsigned char facteur;     /* facteur fixe (1 par défaut) ou répétition */
    unsigned char type;        /* description : RCIM_DESC_F, _E, _U */
    unsigned char e;
    unsigned char d;
    unsigned int  debut;       /* libellé : position de la constante-chaîne */
} SpecFormat;

/* Lit un entier décimal (borné pour éviter tout débordement). */
static unsigned int lire_entier(Analyse *a, unsigned int *nb_chiffres)
{
    unsigned int n = 0;
    *nb_chiffres = 0;
    while (est_chiffre(car(a))) {
        if (n < 1000) n = n * 10 + (unsigned int) (car(a) - '0');
        (*nb_chiffres)++;
        a->pos++;
    }
    return n;
}

static int fin_de_mot(const Analyse *a, unsigned int p)
{
    return !est_lettre(car_a(a, p)) && !est_chiffre(car_a(a, p));
}

/* Format entre crochets ; a->pos est sur '['. texte : tampon de lecture des
   libellés, relus à la génération. */
static int format(Analyse *a, SpecFormat *specs, unsigned int *nb, unsigned char *texte)
{
    SpecFormat *sp;
    unsigned int p;
    unsigned int p_facteur;
    unsigned int n;
    unsigned int chiffres;
    unsigned int borne;
    unsigned int lg;
    char c;

    borne = PROFIL_EST_T1600(a->profil) ? 62 : 99;
    *nb = 0;
    a->pos++;
    for (;;) {
        sauter_espaces(a);
        if (*nb == FMT_MAX) return erreur(a, ERR_C_LIGNE_TROP_COMPLEXE, a->pos);
        sp = &specs[*nb];
        memset(sp, 0, sizeof *sp);
        sp->facteur = 1;
        p = a->pos;
        p_facteur = p;
        if (car(a) == '*') {
            sp->variable = 1;
            a->pos++;
        } else if (est_chiffre(car(a))) {
            n = lire_entier(a, &chiffres);
            if (n == 0)     return erreur(a, ERR_C_FACTEUR_NUL, p_facteur);
            if (n > 99)     return erreur(a, ERR_C_FACTEUR_SUPERIEUR_99, p_facteur);
            if (n > borne)  return erreur(a, ERR_C_FACTEUR_AU_DELA_PROFIL, p_facteur);
            sp->facteur = (unsigned char) n;
        }
        p = a->pos;
        c = car(a);
        if (c == '\'' || (c == '.' && a->profil == PROFIL_T1600_F4)) {   /* libellé */
            sp->genre = SPEC_LIBELLE;
            sp->debut = p;
            if (!lire_chaine(a, texte, &lg)) return 0;
        } else if (c == '.') {                                 /* .n. hors de F4 */
            return erreur(a, ERR_C_HORS_PROFIL, p);
        } else if (c == '/') {                                 /* mise en page */
            sp->genre = SPEC_MISE_EN_PAGE;
            sp->tag = RCIM_OP_AFF_LIGNE;
            a->pos++;
        } else if ((c == 'X' || c == 'C' || c == 'L') && fin_de_mot(a, a->pos + 1)) {
            sp->genre = SPEC_MISE_EN_PAGE;
            sp->tag = c == 'X' ? RCIM_OP_AFF_ESPACE : c == 'C' ? RCIM_OP_AFF_RETOUR : RCIM_OP_AFF_SAUT;
            a->pos++;
        } else if (c == 'U' && fin_de_mot(a, a->pos + 1)) {     /* description */
            sp->genre = SPEC_DESCRIPTION;
            sp->type = RCIM_DESC_U;
            a->pos++;
        } else if ((c == 'F' || c == 'E') && est_chiffre(car_a(a, a->pos + 1))) {
            sp->genre = SPEC_DESCRIPTION;
            sp->type = c == 'F' ? RCIM_DESC_F : RCIM_DESC_E;
            a->pos++;
            n = lire_entier(a, &chiffres);
            if (chiffres > 2) return erreur(a, ERR_C_FORMAT_CHIFFRES, p);
            sp->e = (unsigned char) n;
            if (car(a) != '.') return erreur(a, ERR_C_FORMAT_SPECIFICATION, p);
            a->pos++;
            n = lire_entier(a, &chiffres);
            if (chiffres == 0) return erreur(a, ERR_C_FORMAT_SPECIFICATION, p);
            if (chiffres > 2) return erreur(a, ERR_C_FORMAT_CHIFFRES, p);
            sp->d = (unsigned char) n;
        } else
            return erreur(a, ERR_C_FORMAT_SPECIFICATION, p);
        if (sp->genre == SPEC_DESCRIPTION && sp->variable)
            return erreur(a, ERR_C_FORMAT_ETOILE, p_facteur);
        (*nb)++;
        sauter_espaces(a);
        if (car(a) == ',') { a->pos++; continue; }
        if (car(a) == ']') { a->pos++; return 1; }
        return erreur(a, ERR_C_FORMAT_CROCHET, a->pos);
    }
}

/* Fin de la liste d'affichage : fin de ligne, ';', SINON ou FIN. */
static int fin_de_liste(Analyse *a)
{
    sauter_espaces(a);
    return car(a) == '\0' || car(a) == ';' || voir_mot(a, "SINON") || voir_mot(a, "FIN");
}

/* Génère l'expression qui commence à la position p du texte. */
static int generer_expression_a(Analyse *a, unsigned int p, InfoExpression *e)
{
    a->pos = p;
    return expression(a, e);
}

/* Corps d'AFFICHER (ou de ?), après le mot ; p_mot : position du mot. */
static int corps_afficher(Analyse *a, unsigned int p_mot)
{
    SpecFormat specs[FMT_MAX];
    unsigned int debuts[FMT_MAX];
    unsigned int nb_specs = 0;
    unsigned int nb_expr = 0;
    unsigned int besoin = 0;
    unsigned int i;
    unsigned int k;
    unsigned int r;
    unsigned int fin;
    unsigned int lg;
    unsigned char texte[COMPILATEUR_LONGUEUR_MAX];
    unsigned char brouillon[RCIM_LONGUEUR_MAX_SEQUENCE];
    TamponRCIM tb;
    TamponRCIM *vrai;
    InfoExpression e;
    int avec_format;

    avec_format = car(a) == '[';
    if (avec_format && !format(a, specs, &nb_specs, texte)) return 0;

    /* liste d'affichage : bornes relevées, sans rien écrire (brouillon) */
    if (!fin_de_liste(a)) {
        vrai = a->t;
        for (;;) {
            sauter_espaces(a);
            if (nb_expr == FMT_MAX) { a->t = vrai; return erreur(a, ERR_C_LIGNE_TROP_COMPLEXE, a->pos); }
            debuts[nb_expr++] = a->pos;
            rcim_tampon_initialiser(&tb, brouillon, RCIM_LONGUEUR_MAX_SEQUENCE);
            a->t = &tb;
            if (!expression(a, &e)) { a->t = vrai; return 0; }
            a->t = vrai;
            sauter_espaces(a);
            if (car(a) != ',') break;
            a->pos++;
        }
    }
    fin = a->pos;

    /* IMP-0178 : descriptions (répétitions comprises) et facteurs variables
       ne dépassent pas le nombre d'expressions */
    for (i = 0; i < nb_specs; i++) {
        if (specs[i].variable) besoin++;
        if (specs[i].genre == SPEC_DESCRIPTION) besoin += specs[i].facteur;
    }
    if (besoin > nb_expr)
        return erreur(a, ERR_C_FORMAT_DESCRIPTIONS, p_mot);

    /* génération : format et liste parcourus ensemble */
    ECRIRE(a, rcim_ecrire_tag(a->t, avec_format ? RCIM_OP_AFF_TETE_FORMAT
                                                : RCIM_OP_AFF_TETE_SANS_FORMAT));
    k = 0;
    for (i = 0; i < nb_specs; i++) {
        SpecFormat *sp = &specs[i];
        unsigned char facteur = sp->facteur;
        if (sp->genre == SPEC_DESCRIPTION) {
            ECRIRE(a, rcim_ecrire_description(a->t, sp->type, sp->facteur, sp->e, sp->d));
            for (r = 0; r < sp->facteur; r++) {
                if (!generer_expression_a(a, debuts[k++], &e)) return 0;
                ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_OP_AFF_VALEUR));
            }
            continue;
        }
        if (sp->variable) {
            unsigned int pe = debuts[k];
            if (!generer_expression_a(a, debuts[k++], &e)) return 0;
            if (!exiger_nombre(a, &e, pe)) return 0;      /* IMP-0188, LSE15 l. 1678 */
            facteur = RCIM_FACTEUR_VARIABLE;
        }
        if (sp->genre == SPEC_LIBELLE) {
            a->pos = sp->debut;
            if (!lire_chaine(a, texte, &lg)) return 0;
            ECRIRE(a, rcim_ecrire_libelle(a->t, facteur, texte, lg));
        } else
            ECRIRE(a, rcim_ecrire_mise_en_page(a->t, sp->tag, facteur));
    }
    while (k < nb_expr) {
        if (!generer_expression_a(a, debuts[k++], &e)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_OP_AFF_VALEUR));
    }
    a->pos = fin;
    return 1;
}

/* --- LIRE, déclarations, fichiers (§5 ; fiche §4) ------------------------- */

/* Nom d'identificateur à la position courante ; renvoie sa longueur, ou 0
   (sans diagnostic) si aucun identificateur admissible n'y figure. */
static unsigned int nom_identificateur(Analyse *a, unsigned int *p)
{
    unsigned int lg;
    sauter_espaces(a);
    *p = a->pos;
    lg = longueur_mot(a, *p);
    if (lg == 0 || mot_reserve(a, *p, lg) != RESERVE_NON || lg > SYMBOLES_NOM_MAX)
        return 0;
    return lg;
}

/* LIRE cible {, cible} : D1 { [indices] ref BB } (IMP-0183) */
static int instruction_lire(Analyse *a)
{
    unsigned int p;
    unsigned int lg;
    InfoExpression e;

    ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_CTL_LIRE_TETE));
    for (;;) {
        lg = nom_identificateur(a, &p);
        if (lg == 0) return erreur(a, ERR_C_CIBLE_LIRE, p);
        if (!reference_variable(a, p, lg, &e)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_OP_LIRE_VALEUR));
        sauter_espaces(a);
        if (est_operateur(car(a))) return erreur(a, ERR_C_CIBLE_LIRE, p);
        if (car(a) != ',') return 1;
        a->pos++;
    }
}

/* TABLEAU nom[d1{,d2}] {, ...} : [d1] {[d2]} A9 id dim, par tableau */
static int instruction_tableau(Analyse *a)
{
    unsigned int p;
    unsigned int pd;
    unsigned int lg;
    unsigned int id;
    unsigned char dim;
    InfoExpression e;

    for (;;) {
        lg = nom_identificateur(a, &p);
        if (lg == 0) return erreur(a, ERR_C_NOM_TABLEAU, p);
        if (!symbole(a, p, lg, SYM_IDENTIFICATEUR, &id)) return 0;
        a->pos = p + lg;
        sauter_espaces(a);
        if (car(a) != '[') return erreur(a, ERR_C_DECLARATION_TABLEAU, a->pos);
        a->pos++;
        dim = 0;
        for (;;) {
            sauter_espaces(a);
            pd = a->pos;
            if (!expression(a, &e)) return 0;
            if (e.type == TYPE_CHAINE) return erreur(a, ERR_C_DIMENSION_NON_NUMERIQUE, pd);
            dim++;
            sauter_espaces(a);
            if (car(a) == ']') { a->pos++; break; }
            if (car(a) == ',' && dim < 2) { a->pos++; continue; }
            return erreur(a, ERR_C_DECLARATION_TABLEAU, a->pos);
        }
        ECRIRE(a, rcim_ecrire_tableau(a->t, id, dim));
        sauter_espaces(a);
        if (car(a) != ',') return 1;
        a->pos++;
    }
}

/* CHAINE nom {, nom} ou LIBERER nom {, nom} : AA id ou AB id, par nom */
static int instruction_noms(Analyse *a, TagRCIM tag)
{
    unsigned int p;
    unsigned int lg;
    unsigned int id;

    for (;;) {
        lg = nom_identificateur(a, &p);
        if (lg == 0) return erreur(a, ERR_C_NOM_DECLARATION, p);
        if (!symbole(a, p, lg, SYM_IDENTIFICATEUR, &id)) return 0;
        a->pos = p + lg;
        ECRIRE(a, rcim_ecrire_tag_id(a->t, tag, id));
        sauter_espaces(a);
        if (car(a) != ',') return 1;
        a->pos++;
    }
}

/* GARER nom, e, e : 40 id [e] [e] AC
   CHARGER nom, e, e {, nom} : 40 id [e] [e] {40 id} AD n */
static int instruction_garer_charger(Analyse *a, int charger)
{
    unsigned int p;
    unsigned int lg;
    unsigned int id;
    unsigned char n = 3;
    InfoExpression e;
    int i;

    lg = nom_identificateur(a, &p);
    if (lg == 0) return erreur(a, ERR_C_NOM_GARER_CHARGER, p);
    if (!symbole(a, p, lg, SYM_IDENTIFICATEUR, &id)) return 0;
    a->pos = p + lg;
    ECRIRE(a, rcim_ecrire_reference(a->t, id));
    for (i = 0; i < 2; i++) {
        sauter_espaces(a);
        if (car(a) != ',') return erreur(a, ERR_C_PARAMETRE_FICHIER, a->pos);
        a->pos++;
        if (!expression(a, &e)) return 0;
    }
    if (charger) {
        sauter_espaces(a);
        if (car(a) == ',') {
            a->pos++;
            lg = nom_identificateur(a, &p);
            if (lg == 0) return erreur(a, ERR_C_NOM_GARER_CHARGER, p);
            if (!symbole(a, p, lg, SYM_IDENTIFICATEUR, &id)) return 0;
            a->pos = p + lg;
            ECRIRE(a, rcim_ecrire_reference(a->t, id));
            n = 4;
        }
        ECRIRE(a, rcim_ecrire_tag_u8(a->t, RCIM_OP_CHARGER, n));
    } else
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_OP_GARER));
    return 1;
}

/* SUPPRIMER e {, e} (x [x] AE n) ; EXECUTER e {, e} (x [v] AF n) */
static int instruction_un_ou_deux(Analyse *a, TagRCIM tag, NatureErreur trop)
{
    InfoExpression e;
    unsigned int p;
    unsigned char n = 1;

    if (!expression(a, &e)) return 0;
    sauter_espaces(a);
    if (car(a) == ',') {
        a->pos++;
        sauter_espaces(a);
        p = a->pos;
        if (!expression(a, &e)) return 0;
        if (tag == RCIM_OP_EXECUTER && !exiger_nombre(a, &e, p)) return 0;
        n = 2;
        sauter_espaces(a);
        if (car(a) == ',') return erreur(a, trop, a->pos);
    }
    ECRIRE(a, rcim_ecrire_tag_u8(a->t, tag, n));
    return 1;
}

/* Déclaration interdite en mode machine de bureau sous MITRA 15 (§8 bis). */
static int declaration_interdite(const Analyse *a)
{
    return a->mode_bureau && a->profil == PROFIL_MITRA15_V8;
}

/* --- déclaration de procédure (§13 ; IMP-0174, IMP-0175) ------------------ */

#define DECL_MAX 32   /* §13.2 : une ligne de 80 caractères n'en porte pas davantage */

/* PROCEDURE &NOM(p1,...,pn) {LOCAL l1,...,lm}, en tête d'une ligne de
   programme ; a->pos est après le mot PROCEDURE.
   CD idP n p1...pn m l1...lm */
static int declaration_procedure(Analyse *a)
{
    unsigned int parametres[DECL_MAX];
    unsigned int locaux[DECL_MAX];
    unsigned char est_proc[DECL_MAX];
    unsigned char n = 0;
    unsigned char m = 0;
    unsigned int id_proc;
    unsigned int id;
    unsigned int p;
    unsigned int lg;
    unsigned char i;
    int avec_et;
    int r;

    /* nom */
    sauter_espaces(a);
    p = a->pos;
    if (car(a) != '&') return erreur(a, ERR_C_DECLARATION_NOM, p);
    lg = longueur_mot(a, p + 1);
    if (lg == 0) return erreur(a, ERR_C_NOM_PROCEDURE_INCORRECT, p);
    if (lg > SYMBOLES_NOM_MAX) return erreur(a, ERR_C_NOM_PROCEDURE_TROP_LONG, p);
    if (!symbole(a, p + 1, lg, SYM_PROCEDURE, &id_proc)) return 0;
    a->pos = p + 1 + lg;
    sauter_espaces(a);
    if (car(a) != '(') return erreur(a, ERR_C_DECLARATION_PARENTHESES, a->pos);
    a->pos++;

    /* paramètres formels (LSE15 §7-3) */
    sauter_espaces(a);
    if (car(a) == ')') {
        a->pos++;
    } else {
        for (;;) {
            sauter_espaces(a);
            p = a->pos;
            avec_et = car(a) == '&';
            lg = longueur_mot(a, avec_et ? p + 1 : p);
            if (lg == 0) return erreur(a, ERR_C_PARAMETRES_DECLARATION, p);
            if (avec_et) {
                if (lg > SYMBOLES_NOM_MAX) return erreur(a, ERR_C_NOM_PROCEDURE_TROP_LONG, p);
                if (!symbole(a, p + 1, lg, SYM_PROCEDURE, &id)) return 0;
                if (id == id_proc) return erreur(a, ERR_C_PARAMETRE_HOMONYME, p);
                a->pos = p + 1 + lg;
            } else {
                r = mot_reserve(a, p, lg);
                if (r == RESERVE_COMMUN) return erreur(a, ERR_C_MOT_RESERVE, p);
                if (r == RESERVE_PROFIL) return erreur(a, ERR_C_MOT_RESERVE_PROFIL, p);
                if (lg > SYMBOLES_NOM_MAX) return erreur(a, ERR_C_IDENTIFICATEUR_TROP_LONG, p);
                if (!symbole(a, p, lg, SYM_IDENTIFICATEUR, &id)) return 0;
                a->pos = p + lg;
            }
            sauter_espaces(a);
            if (car(a) == '[') return erreur(a, ERR_C_PARAMETRE_INDICE, p);
            for (i = 0; i < n; i++)
                if (parametres[i] == id && est_proc[i] == (unsigned char) avec_et)
                    return erreur(a, avec_et ? ERR_C_PARAMETRE_PROCEDURE_DOUBLE
                                             : ERR_C_PARAMETRE_DOUBLE, p);
            if (n == DECL_MAX) return erreur(a, ERR_C_LIGNE_TROP_COMPLEXE, p);
            parametres[n] = id;
            est_proc[n] = (unsigned char) avec_et;
            n++;
            if (car(a) == ',') { a->pos++; continue; }
            if (car(a) == ')') { a->pos++; break; }
            return erreur(a, ERR_C_PARAMETRES_DECLARATION, a->pos);
        }
    }

    /* clause LOCAL (LSE15 §7-7, §7-9) */
    r = prendre_mot(a, "LOCAL");
    if (r < 0) return 0;
    if (r) {
        for (;;) {
            sauter_espaces(a);
            p = a->pos;
            if (car(a) == '&') return erreur(a, ERR_C_LOCAL_PROCEDURE, p);
            lg = longueur_mot(a, p);
            if (lg == 0) return erreur(a, ERR_C_NOM_DECLARATION, p);
            r = mot_reserve(a, p, lg);
            if (r == RESERVE_COMMUN) return erreur(a, ERR_C_MOT_RESERVE, p);
            if (r == RESERVE_PROFIL) return erreur(a, ERR_C_MOT_RESERVE_PROFIL, p);
            if (lg > SYMBOLES_NOM_MAX) return erreur(a, ERR_C_IDENTIFICATEUR_TROP_LONG, p);
            if (!symbole(a, p, lg, SYM_IDENTIFICATEUR, &id)) return 0;
            a->pos = p + lg;
            for (i = 0; i < m; i++)
                if (locaux[i] == id) return erreur(a, ERR_C_LOCAL_DOUBLE, p);
            if (m == DECL_MAX) return erreur(a, ERR_C_LIGNE_TROP_COMPLEXE, p);
            locaux[m++] = id;
            sauter_espaces(a);
            if (car(a) != ',') break;
            a->pos++;
        }
    }

    sauter_espaces(a);
    if (car(a) != ';' && a->pos < a->longueur)
        return erreur(a, ERR_C_DECLARATION_SUITE, a->pos);
    ECRIRE(a, rcim_ecrire_declaration_procedure(a->t, id_proc, n, parametres, m, locaux));
    return 1;
}

/* Instruction introduite par un mot du langage ; p : position du mot. */
static int instruction_mot_cle(Analyse *a, unsigned int p)
{
    InfoExpression e;
    int r;

    if ((r = prendre_mot(a, "SI")) != 0)
        return r > 0 && instruction_si(a);
    if ((r = prendre_mot(a, "DEBUT")) != 0)
        return r > 0 && instruction_debut(a);
    if ((r = prendre_mot(a, "ALLER")) != 0) {
        if (r < 0) return 0;
        if (a->mode_bureau) return erreur(a, ERR_C_BUREAU_NUMERO_LIGNE, p);
        r = prendre_mot(a, "EN");
        if (r < 0) return 0;
        if (r == 0) return erreur(a, ERR_C_EN_MANQUANT, a->pos);
        return cible_ligne(a, RCIM_CTL_ALLER_EN, RCIM_CTL_ALLER_EN_CALCULE);
    }
    if ((r = prendre_mot(a, "FAIRE")) != 0) {
        if (r < 0) return 0;
        if (a->mode_bureau) return erreur(a, ERR_C_BUREAU_NUMERO_LIGNE, p);
        return instruction_faire(a);
    }
    if ((r = prendre_mot(a, "RETOUR")) != 0) {
        if (r < 0) return 0;
        if (voir_mot(a, "EN")) {
            if (a->mode_bureau) return erreur(a, ERR_C_BUREAU_NUMERO_LIGNE, p);
            if (prendre_mot(a, "EN") < 0) return 0;
            return cible_ligne(a, RCIM_CTL_RETOUR_EN, RCIM_CTL_RETOUR_EN_CALCULE);
        }
        if (a->mode_bureau) return erreur(a, ERR_C_BUREAU_RETOUR, p);
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_CTL_RETOUR));
        return 1;
    }
    if ((r = prendre_mot(a, "RESULTAT")) != 0) {
        if (r < 0) return 0;
        if (a->mode_bureau) return erreur(a, ERR_C_BUREAU_RETOUR, p);
        if (!expression(a, &e)) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_CTL_RESULTAT));
        return 1;
    }
    if ((r = prendre_mot(a, "TERMINER")) != 0) {
        if (r < 0) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_CTL_TERMINER));
        return 1;
    }
    if ((r = prendre_mot(a, "PAUSE")) != 0) {
        if (r < 0) return 0;
        ECRIRE(a, rcim_ecrire_tag(a->t, RCIM_CTL_PAUSE));
        return 1;
    }
    if (voir_mot(a, "AFFICHER")) {                       /* terminé par '[' ou espace */
        unsigned int q = p + 8;
        if (car_a(a, q) == '[') { a->pos = q; return corps_afficher(a, p); }
        if ((r = prendre_mot(a, "AFFICHER")) < 0) return 0;
        sauter_espaces(a);
        return corps_afficher(a, p);
    }
    if ((r = prendre_mot(a, "LIRE")) != 0)
        return r > 0 && instruction_lire(a);
    if ((r = prendre_mot(a, "TABLEAU")) != 0) {
        if (r < 0) return 0;
        if (declaration_interdite(a)) return erreur(a, ERR_C_BUREAU_DECLARATION, p);
        return instruction_tableau(a);
    }
    if ((r = prendre_mot(a, "CHAINE")) != 0) {
        if (r < 0) return 0;
        if (declaration_interdite(a)) return erreur(a, ERR_C_BUREAU_DECLARATION, p);
        return instruction_noms(a, RCIM_OP_CHAINE);
    }
    if ((r = prendre_mot(a, "LIBERER")) != 0) {
        if (r < 0) return 0;
        if (declaration_interdite(a)) return erreur(a, ERR_C_BUREAU_DECLARATION, p);
        return instruction_noms(a, RCIM_OP_LIBERER);
    }
    if ((r = prendre_mot(a, "GARER")) != 0)
        return r > 0 && instruction_garer_charger(a, 0);
    if ((r = prendre_mot(a, "CHARGER")) != 0)
        return r > 0 && instruction_garer_charger(a, 1);
    if ((r = prendre_mot(a, "SUPPRIMER")) != 0) {        /* mot réservé du seul MITRA 15 */
        if (r < 0) return 0;
        return instruction_un_ou_deux(a, RCIM_OP_SUPPRIMER, ERR_C_PARAMETRE_FICHIER);
    }
    if ((r = prendre_mot(a, "EXECUTER")) != 0) {
        if (r < 0) return 0;
        if (a->mode_bureau) return erreur(a, ERR_C_BUREAU_EXECUTER, p);   /* point B6, LSE 8086 */
        return instruction_un_ou_deux(a, RCIM_OP_EXECUTER, ERR_C_ARGUMENTS_EXECUTER);
    }
    if ((r = prendre_mot(a, "PROCEDURE")) != 0) {        /* hors tête de ligne */
        if (r < 0) return 0;
        if (a->mode_bureau) return erreur(a, ERR_C_BUREAU_PROCEDURE, p);
        return erreur(a, ERR_C_DEBUT_INSTRUCTION, p);
    }
    return erreur(a, ERR_C_INSTRUCTION_INCORRECTE, p);
}

/* Mots qui introduisent une instruction (grammaire statement). */
static const char *const mots_instruction[] = {
    "AFFICHER", "ALLER", "CHAINE", "CHARGER", "DEBUT", "EXECUTER", "FAIRE",
    "GARER", "LIBERER", "LIRE", "PAUSE", "PROCEDURE", "RESULTAT", "RETOUR",
    "SI", "SUPPRIMER", "TABLEAU", "TERMINER", NULL
};

/* Une instruction ; renvoie 1 si elle a été reconnue sans erreur. */
static int instruction(Analyse *a)
{
    unsigned int p;
    unsigned int lg;
    char c;

    sauter_espaces(a);
    p = a->pos;
    c = car(a);
    if (c == '\0' || c == ';')                   /* instruction vide */
        return 1;
    if (c == '*')
        return commentaire(a);
    if (c == '&') {                                      /* appel en instruction */
        InfoExpression e;
        return appel_procedure(a, &e, RCIM_APPEL_INSTRUCTION);
    }
    if (c == '?') {                                      /* IMP-0173, IMP-0191 */
        if (!a->mode_bureau) return erreur(a, ERR_C_POINT_INTERROGATION, p);
        a->pos++;
        sauter_espaces(a);
        return corps_afficher(a, p);
    }
    if (est_lettre(c)) {
        lg = longueur_mot(a, p);
        if (PROFIL_EST_T1600(a->profil) && mot_est(a, p, lg, "SUPPRIMER"))
            return erreur(a, ERR_C_HORS_PROFIL, p);            /* §8 bis */
        if (mot_est(a, p, lg, "UN") || mot_est(a, p, lg, "ZER")) {
            /* UN id, ZER id (T1600, INRDP75 Annexe III) ; sinon, identificateur */
            unsigned int q = p + lg;
            while (car_a(a, q) == ' ') q++;
            if (car_a(a, p + lg) == ' ' && est_lettre(car_a(a, q))) {
                unsigned int lgi;
                unsigned int pi;
                InfoExpression e;
                TagRCIM op = lg == 2 ? RCIM_OP_UN : RCIM_OP_ZER;
                if (!PROFIL_EST_T1600(a->profil)) return erreur(a, ERR_C_HORS_PROFIL, p);
                a->pos = q;
                lgi = nom_identificateur(a, &pi);
                if (lgi == 0) return erreur(a, ERR_C_INSTRUCTION_INCORRECTE, pi);
                a->pos = pi + lgi;
                sauter_espaces(a);
                if (car(a) == '[') return erreur(a, ERR_C_INSTRUCTION_INCORRECTE, a->pos);
                if (!reference_variable(a, pi, lgi, &e)) return 0;
                ECRIRE(a, rcim_ecrire_tag(a->t, op));
                return 1;
            }
        }
        if (mot_reserve(a, p, lg) == RESERVE_NON)
            return affectation(a, p, lg);
        if (dans_liste(a, p, lg, mots_instruction))
            return instruction_mot_cle(a, p);
        /* autre mot réservé : employé comme identificateur, ou début incorrect */
        {
            unsigned int q = p + lg;
            while (car_a(a, q) == ' ') q++;
            if (car_a(a, q) == CAR_AFFECTATION || car_a(a, q) == '[')
                return affectation(a, p, lg);       /* C 101 ou C 102 */
        }
        return erreur(a, ERR_C_DEBUT_INSTRUCTION, p);
    }
    if (c == '(' || est_chiffre(c))
        return erreur(a, ERR_C_CIBLE_AFFECTATION, p);
    if (est_minuscule(c))         return erreur(a, ERR_C_MINUSCULE, p);
    if (!caractere_du_langage(c)) return erreur(a, ERR_C_CARACTERE_INTERDIT, p);
    return erreur(a, ERR_C_DEBUT_INSTRUCTION, p);
}

/* Texte qui subsiste après une instruction complète (§9 étape 6, IMP-0191). */
static int texte_inattendu(Analyse *a)
{
    char c = car(a);
    if (c == ')')            return erreur(a, ERR_C_PARENTHESE_EN_EXCES, a->pos);
    if (debut_operande(a, c)) return erreur(a, ERR_C_OPERATEUR_MANQUANT, a->pos);
    if (est_minuscule(c))         return erreur(a, ERR_C_MINUSCULE, a->pos);
    if (!caractere_du_langage(c)) return erreur(a, ERR_C_CARACTERE_INTERDIT, a->pos);
    return erreur(a, ERR_C_INSTRUCTION_INCORRECTE, a->pos);
}

/* Liste d'instructions séparées par ';' jusqu'à la fin de la ligne. */
static int instructions(Analyse *a)
{
    for (;;) {
        if (!instruction(a)) return 0;
        sauter_espaces(a);
        if (a->pos >= a->longueur) return 1;
        if (car(a) != ';') return texte_inattendu(a);
        a->pos++;
    }
}

/* --- point d'entrée (§4, §9) ------------------------------------------- */

int compilateur_compiler(const char *texte, unsigned int longueur,
                         ResultatCompilation *resultat,
                         DiagnosticCompilation *diagnostic)
{
    unsigned char octets[RCIM_LONGUEUR_MAX_SEQUENCE];
    TamponRCIM t;
    Analyse a;
    unsigned long numero = 0;
    unsigned int borne;
    HandleObjet bloc;
    unsigned char *contenu;

    diagnostic->nature = ERR_AUCUNE;
    diagnostic->position = 0;

    /* 1. longueur (IMP-0191 : C 63) */
    if (longueur > COMPILATEUR_LONGUEUR_MAX) {
        diagnostic->nature = ERR_C_LIGNE_TROP_LONGUE;
        return 0;
    }

    rcim_tampon_initialiser(&t, octets, RCIM_LONGUEUR_MAX_SEQUENCE);
    a.texte = texte;
    a.longueur = longueur;
    a.pos = 0;
    a.t = &t;
    a.profil = configuration_session.profil;
    a.diag = diagnostic;

    /* 2-3. forme de la ligne (IMP-0171) */
    sauter_espaces(&a);
    a.mode_bureau = !est_chiffre(car(&a));
    if (!a.mode_bureau) {
        unsigned int p = a.pos;
        while (est_chiffre(car(&a))) {
            if (numero <= 99999ul) numero = numero * 10 + (unsigned long) (car(&a) - '0');
            a.pos++;
        }
        if (numero == 0 || numero > 32000ul)
            return erreur(&a, ERR_C_NUMERO_HORS_BORNES, p);
        borne = PROFIL_EST_T1600(a.profil) ? 255 : 250;
        if (numero > borne)
            return erreur(&a, ERR_C_NUMERO_AU_DELA_PROFIL, p);
        if (car(&a) != ' ' && car(&a) != '*')                /* IMP-0182 */
            return erreur(&a, ERR_C_NUMERO_SANS_ESPACE, a.pos);
        if (car(&a) == '*') {
            if (!commentaire(&a)) return 0;
        } else {
            int r = prendre_mot(&a, "PROCEDURE");            /* §13.4, IMP-0175 */
            if (r < 0) return 0;
            if (r) {
                if (!declaration_procedure(&a)) return 0;
                if (car(&a) == ';') {
                    a.pos++;
                    if (!instructions(&a)) return 0;
                }
            } else if (!instructions(&a))
                return 0;
        }
    } else if (!instructions(&a))
        return 0;

    /* 7-8. objet compilé (§9) */
    bloc = objets_creer(t.curseur);
    if (bloc == GESTOBJ_HANDLE_NUL)
        return erreur(&a, ERR_C_PLUS_DE_PLACE, 0);
    contenu = objets_resoudre(bloc);
    memcpy(contenu, octets, t.curseur);
    objets_relacher(bloc);

    /* 9. résultat */
    if (a.mode_bureau) {
        resultat->type = LSE_LIGNE_BUREAU;
        resultat->resultat.bloc_bureau = bloc;
        resultat->longueur_bureau = t.curseur;
    } else {
        resultat->type = LSE_LIGNE_PROGRAMME;
        resultat->resultat.ligne_programme.numero_ligne = (unsigned char) numero;
        resultat->resultat.ligne_programme.longueur_octets = t.curseur;
        resultat->resultat.ligne_programme.bloc = bloc;
        resultat->longueur_bureau = 0;
    }
    return 1;
}
