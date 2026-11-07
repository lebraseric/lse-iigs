/* decompilateur.c — restitution textuelle d'une ligne compilée, en une
   passe sur la séquence RCI-M
   EXI: EXI-0003, EXI-0010, EXI-0015, EXI-0017, EXI-0022
   VM: VM-2208, VM-1801, VM-0406
   IV: IV-IF-0004, ALG-1001, ANN-K-0014, ANN-K-0031
   IMP: IMP-0183 (forme restituée, blocs DEBUT...FIN amendés le 2026-10-23),
        IMP-0190 (équivalents numériques), IMP-0198 (interface), IMP-0199
        (algorithme), IMP-0200 (diagnostics), IMP-0201 (réels), IMP-0202
        (segment), IMP-0243 (texte d'une constante partagée copié sans
        verrouillage)
   Tests: tests/composants/composants_decompilateur.c,
          tests/historiques/historiques_decompilation.c
   Spécification : chapitre Décompilateur §3 à §8.

   Le texte se construit dans le tableau de l'appelant. Une pile d'entrées
   décrit les morceaux en attente, dont les textes se suivent à la fin du
   tableau (§4.2) ; chaque élément combine les entrées du sommet (§4.3).
   Jeu de caractères : la flèche d'affectation est restituée « _ » (95),
   l'exponentiation « ^ » (94). */

#pragma noroot

#include <string.h>
#include "decompilateur.h"
#include "symboles.h"
#include "../interfaces/types_communs.h"
#include "../interfaces/profil.h"

segment "decompil";

#define PILE_MAX 128   /* §4.2 : suffit à toute séquence valide de 255 octets */

/* Texte d'une constante partagée, copié (IMP-0243) : la résoudre puis la
   relâcher déverrouillerait aussi la ligne en cours de restitution quand les
   deux objets partagent un bloc, avant les calculs flottants qui suivent. */
static unsigned char texte_constante[255];

/* Genres d'entrées (§4.2) */
#define ENT_VALEUR 0
#define ENT_SUITE  1
#define ENT_MARQUE 2

/* Sortes de valeurs */
#define VAL_EXPR    0
#define VAL_REF     1   /* référence simple à un identificateur (0x40) */
#define VAL_INDEXEE 2   /* référence indexée (0x41) */
#define VAL_PROC    3   /* nom de procédure (0x40, nature procédure) */
#define VAL_COND    4   /* relation ou condition logique */

/* Rangs (§4.4) */
#define RANG_SI     1
#define RANG_CONCAT 2
#define RANG_ADD    3
#define RANG_MUL    4
#define RANG_PUIS   5
#define RANG_ATOME  6
#define RANG_OU     11
#define RANG_ET     12
#define RANG_REL    13

/* Genre de la dernière instruction d'une suite */
#define INS_AUTRE    0
#define INS_TABLEAU  1
#define INS_CHAINE   2
#define INS_LIBERER  3
#define INS_AFF      4   /* AFFICHER sans format, ouverte */
#define INS_AFF_FMT  5   /* AFFICHER à format, ouverte */
#define INS_LIRE     6   /* ouverte */
#define INS_FAIRE    7   /* FAIRE ... POUR V_v1, avant PAS */
#define INS_FAIRE_P  8   /* après PAS (ou pas implicite), avant JUSQUA ou TANT QUE */
#define INS_SI       9

/* Phases et formes d'une marque SI (§5) */
#define PH_ALORS 0
#define PH_SINON 1
#define SI_INSTRUCTION 0
#define SI_EXPRESSION  1

typedef struct {
    unsigned int  debut;    /* position du texte de l'entrée */
    unsigned char genre;
    unsigned char sorte;    /* valeur : VAL_* ; suite : INS_* de la dernière ; marque : forme */
    unsigned char rang;     /* valeur : rang ; marque : phase */
    unsigned char nb;       /* suite : nombre d'instructions */
    unsigned char ouvert;   /* suite : se termine par un SI ouvert (§5.3) */
    unsigned char liste;    /* suite : éléments de la liste d'AFFICHER ou de LIRE ouverte */
    unsigned char specs;    /* suite : spécifications du format d'AFFICHER ouverte */
    unsigned char cible;    /* marque : position attendue dans la séquence */
    unsigned int  crochet;  /* AFFICHER à format : position du ']' */
} Entree;

typedef struct {
    char                 *texte;
    unsigned int          lg;
    Entree                pile[PILE_MAX];
    unsigned int          nb;          /* entrées */
    ResultatDecompilation erreur;
    ProfilImplantation    profil;
} Decompilation;

/* --- texte ---------------------------------------------------------------- */

static int echec(Decompilation *d, ResultatDecompilation r)
{
    if (d->erreur == DECOMP_OK)
        d->erreur = r;
    return 0;
}

/* Insère n caractères à la position pos, dans le texte de l'entrée i (en
   tête, au milieu ou à la fin) : les entrées situées au-dessus de i sont
   décalées, ainsi que tout crochet de format situé à pos ou après. */
static int inserer(Decompilation *d, unsigned int i, unsigned int pos, const char *s, unsigned int n)
{
    unsigned int j;

    if (n == 0) return 1;
    if (d->lg + n > DECOMPILATEUR_LONGUEUR_MAX) return echec(d, DECOMP_ERR_TROP_LONGUE);
    memmove(d->texte + pos + n, d->texte + pos, d->lg - pos);
    memcpy(d->texte + pos, s, n);
    d->lg += n;
    for (j = 0; j < d->nb; j++) {
        if (j > i) d->pile[j].debut += n;
        if (d->pile[j].genre == ENT_SUITE && d->pile[j].sorte == INS_AFF_FMT
            && d->pile[j].crochet >= pos)
            d->pile[j].crochet += n;
    }
    return 1;
}

/* Mot ou signe en tête du texte de l'entrée i. */
static int prefixer(Decompilation *d, unsigned int i, const char *s)
{
    return inserer(d, i, d->pile[i].debut, s, (unsigned int) strlen(s));
}

static int ajouter(Decompilation *d, const char *s, unsigned int n)
{
    return inserer(d, d->nb, d->lg, s, n);
}

static int ajouter_ch(Decompilation *d, const char *s)
{
    return ajouter(d, s, (unsigned int) strlen(s));
}

/* Écrit un entier en décimal dans s ; renvoie la longueur. */
static unsigned int ecrire_entier(long v, char *s)
{
    char chiffres[12];
    unsigned int n = 0;
    unsigned int lg = 0;
    unsigned long u;

    if (v < 0) { s[lg++] = '-'; u = (unsigned long) (-v); } else u = (unsigned long) v;
    do { chiffres[n++] = (char) ('0' + (int) (u % 10)); u /= 10; } while (u != 0);
    while (n > 0) s[lg++] = chiffres[--n];
    s[lg] = '\0';
    return lg;
}

/* --- pile ----------------------------------------------------------------- */

static Entree *sommet(Decompilation *d, unsigned int k)   /* k = 0 : sommet */
{
    return &d->pile[d->nb - 1 - k];
}

/* Fin du texte de l'entrée i. */
static unsigned int fin_entree(const Decompilation *d, unsigned int i)
{
    return i + 1 < d->nb ? d->pile[i + 1].debut : d->lg;
}

static int empiler(Decompilation *d, unsigned char genre, unsigned char sorte, unsigned char rang)
{
    Entree *e;
    if (d->nb == PILE_MAX) return echec(d, DECOMP_ERR_SEQUENCE);
    e = &d->pile[d->nb++];
    memset(e, 0, sizeof *e);
    e->debut = d->lg;
    e->genre = genre;
    e->sorte = sorte;
    e->rang = rang;
    return 1;
}

/* Nouvelle valeur de texte s. */
static int empiler_valeur(Decompilation *d, const char *s, unsigned int n,
                          unsigned char sorte, unsigned char rang)
{
    return empiler(d, ENT_VALEUR, sorte, rang) && ajouter(d, s, n);
}

/* Les k entrées du sommet sont des valeurs (non des conditions si expr). */
static int valeurs(Decompilation *d, unsigned int k, int expressions)
{
    unsigned int i;
    if (d->nb < k) return echec(d, DECOMP_ERR_SEQUENCE);
    for (i = 0; i < k; i++) {
        Entree *e = sommet(d, i);
        if (e->genre != ENT_VALEUR) return echec(d, DECOMP_ERR_SEQUENCE);
        if (expressions && e->sorte == VAL_COND) return echec(d, DECOMP_ERR_SEQUENCE);
    }
    return 1;
}

/* Met le texte de l'entrée i entre parenthèses. */
static int parentheser(Decompilation *d, unsigned int i)
{
    unsigned int fin = fin_entree(d, i);
    return inserer(d, i, fin, ")", 1) && prefixer(d, i, "(");
}

/* Les entrées au-dessus de i sont fondues dans l'entrée i. */
static void fondre(Decompilation *d, unsigned int i)
{
    d->nb = i + 1;
}

/* --- instructions ------------------------------------------------------- */

/* L'entrée i (son texte va jusqu'au bout du tableau) devient une instruction
   de genre g ; elle se fond dans la suite du dessous, avec « ; ». */
static int instruction(Decompilation *d, unsigned int i, unsigned char g, unsigned char si_ouvert)
{
    Entree *e = &d->pile[i];
    Entree *s;

    fondre(d, i);
    if (i > 0 && d->pile[i - 1].genre == ENT_SUITE) {
        s = &d->pile[i - 1];
        if (!prefixer(d, i, ";")) return 0;
        d->nb = i;
        if (s->nb < 255) s->nb++;
        s->sorte = g;
        s->ouvert = si_ouvert;
        s->liste = 0;
        s->specs = 0;
        if (g == INS_AFF_FMT) s->crochet = d->lg - 1;
        return 1;
    }
    e->genre = ENT_SUITE;
    e->sorte = g;
    e->nb = 1;
    e->ouvert = si_ouvert;
    e->liste = 0;
    e->specs = 0;
    if (g == INS_AFF_FMT) e->crochet = d->lg - 1;
    return 1;
}

/* Instruction formée des k valeurs du sommet, précédées du mot et séparées
   par des virgules (GARER, CHARGER, SUPPRIMER, EXECUTER). */
static int instruction_liste(Decompilation *d, const char *mot, unsigned int k)
{
    unsigned int i0;
    unsigned int j;

    if (k == 0 || !valeurs(d, k, 1)) return echec(d, DECOMP_ERR_SEQUENCE);
    i0 = d->nb - k;
    for (j = k - 1; j > 0; j--)
        if (!prefixer(d, i0 + j, ",")) return 0;
    if (!prefixer(d, i0, mot)) return 0;
    return instruction(d, i0, INS_AUTRE, 0);
}

/* Nom d'un symbole, avec & s'il est de nature procédure. */
static int texte_nom(Decompilation *d, unsigned int id, char *s, unsigned int *n, int *proc)
{
    EntreeSymbole e;
    unsigned int lg = 0;
    unsigned int i;

    if (!symboles_lire(id, &e) || e.nom_longueur == 0 || e.nom_longueur > SYMBOLES_NOM_MAX)
        return echec(d, DECOMP_ERR_REFERENCE);
    *proc = e.nature == SYM_PROCEDURE;
    if (*proc) s[lg++] = '&';
    for (i = 0; i < e.nom_longueur; i++) s[lg++] = e.nom[i];
    *n = lg;
    return 1;
}

/* Une déclaration TABLEAU, CHAINE ou LIBERER ; texte : nom ou NOM[d...],
   déjà au sommet (entrée i). Fusion avec la précédente (§4.5). */
static int declaration(Decompilation *d, unsigned int i, unsigned char g, const char *mot)
{
    Entree *s;

    if (i > 0 && d->pile[i - 1].genre == ENT_SUITE && d->pile[i - 1].sorte == g) {
        s = &d->pile[i - 1];
        if (!prefixer(d, i, ",")) return 0;
        d->nb = i;
        s->ouvert = 0;
        return 1;
    }
    return prefixer(d, i, mot) && instruction(d, i, g, 0);
}

/* Suite d'instructions ouverte de genre g au sommet, sous une valeur. */
static Entree *suite_ouverte(Decompilation *d, unsigned char g1, unsigned char g2)
{
    Entree *s;
    if (d->nb < 2) return NULL;
    s = sommet(d, 1);
    if (s->genre != ENT_SUITE || (s->sorte != g1 && s->sorte != g2)) return NULL;
    if (sommet(d, 0)->genre != ENT_VALEUR) return NULL;
    return s;
}

/* --- constantes ----------------------------------------------------------- */

/* Un caractère s'écrit entre apostrophes, sauf sous F4 s'il n'est pas imprimable. */
static int en_clair(const Decompilation *d, unsigned char c)
{
    return d->profil != PROFIL_T1600_F4 || (c >= 32 && c != 127);
}

/* Constante-chaîne, opérande ou libellé de format, écrite dans s à partir de
   *lg (IMP-0183, IMP-0190) : apostrophe doublée ; sous F4, caractères non
   imprimables en .n., regroupés ; ailleurs, où .n. est hors profil, tels
   quels (Décompilateur §4.3). */
static int ecrire_chaine(Decompilation *d, const unsigned char *t, unsigned int n,
                         char *s, unsigned int *lg)
{
    unsigned int i = 0;
    unsigned int j;
    unsigned int k = *lg;

    if (n == 0) {
        s[k++] = '\'';
        s[k++] = '\'';
    }
    while (i < n) {
        if (en_clair(d, t[i])) {
            s[k++] = '\'';
            while (i < n && en_clair(d, t[i])) {
                if (k + 6 > DECOMPILATEUR_LONGUEUR_MAX) return echec(d, DECOMP_ERR_TROP_LONGUE);
                if (t[i] == '\'') s[k++] = '\'';
                s[k++] = (char) t[i];
                i++;
            }
            s[k++] = '\'';
        } else {
            s[k++] = '.';
            j = 0;
            while (i < n && !en_clair(d, t[i])) {
                if (k + 6 > DECOMPILATEUR_LONGUEUR_MAX) return echec(d, DECOMP_ERR_TROP_LONGUE);
                if (j > 0) s[k++] = ' ';
                k += ecrire_entier((long) t[i], s + k);
                i++;
                j++;
            }
            s[k++] = '.';
        }
    }
    *lg = k;
    return 1;
}


/* --- réels (IMP-0201) ------------------------------------------------------ */

/* 10^0 à 10^22, exactes en double, au lieu d'être recalculées à chaque mise à
   l'échelle (audit du code, AP-03) : mêmes valeurs, même arithmétique. */
static const double puissances[23] = {
    1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11,
    1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22
};
static const long puissances_entieres[7] = { 1L, 10L, 100L, 1000L, 10000L, 100000L, 1000000L };

/* x × 10^n, en une opération correctement arrondie pour |n| <= 22. */
static double echelle(double x, int n)
{
    while (n > 22)  { x = x * puissances[22]; n -= 22; }
    while (n < -22) { x = x / puissances[22]; n += 22; }
    return n < 0 ? x / puissances[-n] : x * puissances[n];
}

static long arrondi(double y)
{
    return (long) (y + 0.5);
}

unsigned int decompilateur_ecrire_reel_chiffres(float valeur, unsigned int chiffres, char *sortie)
{
    float f = valeur;            /* copie locale : paramètre au format extended */
    double x = (double) f;
    char ch[8];
    long m;
    long bas;
    long haut;
    int e = 0;
    int nb;
    int i;
    int c = (int) chiffres;
    unsigned int lg = 0;

    if (c < 1) c = 1;
    if (c > 7) c = 7;
    bas = puissances_entieres[c - 1];
    haut = bas * 10L;
    if (x < 0.0) { sortie[lg++] = '-'; x = -x; }
    if (x == 0.0) { sortie[lg++] = '0'; sortie[lg] = '\0'; return lg; }

    if (x >= 1.0) { while (echelle(x, -(e + 1)) >= 1.0) e++; }
    else          { while (echelle(x, -e) < 1.0) e--; }
    m = arrondi(echelle(x, c - 1 - e));
    if (m >= haut) { e++; m = arrondi(echelle(x, c - 1 - e)); }
    if (m < bas)   { e--; m = arrondi(echelle(x, c - 1 - e)); }

    for (i = c - 1; i >= 0; i--) { ch[i] = (char) ('0' + (int) (m % 10)); m /= 10; }
    nb = c;
    while (nb > 1 && ch[nb - 1] == '0') nb--;

    if (e >= -3 && e <= 5) {                       /* forme décimale */
        if (e >= 0) {
            for (i = 0; i <= e; i++) sortie[lg++] = i < nb ? ch[i] : '0';
            if (nb > e + 1) {
                sortie[lg++] = '.';
                for (i = e + 1; i < nb; i++) sortie[lg++] = ch[i];
            }
        } else {
            sortie[lg++] = '0';
            sortie[lg++] = '.';
            for (i = 0; i < -e - 1; i++) sortie[lg++] = '0';
            for (i = 0; i < nb; i++) sortie[lg++] = ch[i];
        }
    } else {                                       /* mantisse E exposant */
        sortie[lg++] = ch[0];
        if (nb > 1) {
            sortie[lg++] = '.';
            for (i = 1; i < nb; i++) sortie[lg++] = ch[i];
        }
        sortie[lg++] = 'E';
        sortie[lg++] = e < 0 ? '-' : '+';
        if (e < 0) e = -e;
        sortie[lg++] = (char) ('0' + e / 10);
        sortie[lg++] = (char) ('0' + e % 10);
    }
    sortie[lg] = '\0';
    return lg;
}

unsigned int decompilateur_ecrire_reel(float valeur, char *sortie)
{
    return decompilateur_ecrire_reel_chiffres(valeur, 6, sortie);    /* IMP-0201 */
}

/* --- opérations ----------------------------------------------------------- */

/* Opérateur binaire de rang r sur les deux valeurs du sommet (§4.4). */
static int binaire(Decompilation *d, const char *op, unsigned char r, unsigned char sorte)
{
    unsigned int ia;
    unsigned int ib;
    int logique = r >= RANG_OU;

    if (!valeurs(d, 2, 0)) return 0;
    ia = d->nb - 2;
    ib = d->nb - 1;
    /* opérandes : conditions pour ET et OU, expressions sinon */
    if (logique && r != RANG_REL) {
        if (d->pile[ia].sorte != VAL_COND || d->pile[ib].sorte != VAL_COND)
            return echec(d, DECOMP_ERR_SEQUENCE);
    } else if (d->pile[ia].sorte == VAL_COND || d->pile[ib].sorte == VAL_COND)
        return echec(d, DECOMP_ERR_SEQUENCE);
    if (r != RANG_REL) {
        if (d->pile[ia].rang < r && !parentheser(d, ia)) return 0;
        if (d->pile[ib].rang <= r && !parentheser(d, ib)) return 0;
    }
    if (!prefixer(d, ib, op)) return 0;
    fondre(d, ia);
    d->pile[ia].sorte = sorte;
    d->pile[ia].rang = r;
    return 1;
}

/* Fonction usuelle à k arguments : NOM(a1,...,ak). */
static int fonction(Decompilation *d, const char *nom, unsigned int k)
{
    unsigned int i0;
    unsigned int j;

    if (k == 0) {
        if (!empiler_valeur(d, nom, (unsigned int) strlen(nom), VAL_EXPR, RANG_ATOME)) return 0;
        return ajouter(d, "()", 2);
    }
    if (!valeurs(d, k, 1)) return 0;
    i0 = d->nb - k;
    for (j = k - 1; j > 0; j--)
        if (!prefixer(d, i0 + j, ",")) return 0;
    if (!prefixer(d, i0, "(") || !prefixer(d, i0, nom) || !ajouter(d, ")", 1))
        return 0;
    fondre(d, i0);
    d->pile[i0].sorte = VAL_EXPR;
    d->pile[i0].rang = RANG_ATOME;
    return 1;
}

static const char *const noms_fonctions[] = {   /* 0x86 à 0x8E */
    "RAC", "ABS", "EXP", "LGN", "SIN", "COS", "ATG", "ENT", "ALE"
};
static const char *const noms_fonctions_n[] = { /* 0x9F à 0xA4 */
    "SKP", "PTR", "CNB", "EQN", "SCH", "GRL"
};
static const char *const noms_t1600[] = { "SH", "CH", "TH", "SGN" };   /* 0xBC à 0xBF */

/* --- SI (§5) ---------------------------------------------------------- */

/* Indice de la marque la plus haute ; -1 s'il n'y en a pas. */
static int marque_haute(const Decompilation *d)
{
    int i;
    for (i = (int) d->nb - 1; i >= 0; i--)
        if (d->pile[i].genre == ENT_MARQUE) return i;
    return -1;
}

/* Branche d'instruction au-dessus de la marque m : 0 ou une suite.
   Restituée entre DEBUT et FIN si bloc est vrai. *nb, *ouvert : sa forme. */
static int branche(Decompilation *d, unsigned int m, int force, unsigned char *ouvert)
{
    unsigned int nb = 0;
    unsigned char ouv = 0;
    Entree *s;

    if (d->nb > m + 2) return echec(d, DECOMP_ERR_SEQUENCE);
    if (d->nb == m + 2) {
        s = &d->pile[m + 1];
        if (s->genre != ENT_SUITE || s->sorte == INS_FAIRE || s->sorte == INS_FAIRE_P)
            return echec(d, DECOMP_ERR_SEQUENCE);
        nb = s->nb;
        ouv = s->ouvert;
    }
    if (nb == 0) {
        if (!ajouter_ch(d, "DEBUT FIN")) return 0;
    } else if (nb > 1 || (force && ouv)) {
        if (!prefixer(d, m + 1, "DEBUT ") || !ajouter_ch(d, " FIN")) return 0;
        ouv = 0;
    }
    fondre(d, m);
    *ouvert = nb == 1 ? ouv : 0;
    return 1;
}

/* Ferme les marques dont la cible est la position p. */
static int fermer_marques(Decompilation *d, unsigned int p)
{
    int m;
    Entree *e;
    unsigned char ouvert;

    for (;;) {
        m = marque_haute(d);
        if (m < 0) return 1;
        e = &d->pile[m];
        if (e->cible > p) return 1;
        if (e->cible < p) return echec(d, DECOMP_ERR_SEQUENCE);
        if (e->rang == PH_SINON && e->sorte == SI_EXPRESSION) {
            if (d->nb != (unsigned int) m + 2 || !valeurs(d, 1, 1)) return echec(d, DECOMP_ERR_SEQUENCE);
            fondre(d, (unsigned int) m);
            e->genre = ENT_VALEUR;
            e->sorte = VAL_EXPR;
            e->rang = RANG_SI;
            continue;
        }
        if (!branche(d, (unsigned int) m, 0, &ouvert)) return 0;
        /* sans SINON : SI ouvert ; avec SINON : celui de la branche SINON */
        if (!instruction(d, (unsigned int) m, INS_SI, e->rang == PH_ALORS ? 1 : ouvert)) return 0;
    }
}

/* C0 à la position p, cible c. */
static int si_faux(Decompilation *d, unsigned int p, unsigned char c, unsigned int longueur)
{
    Entree *e;
    int m = marque_haute(d);

    if (!valeurs(d, 1, 0) || sommet(d, 0)->sorte != VAL_COND) return echec(d, DECOMP_ERR_SEQUENCE);
    if (c < p + 2 || c > longueur || (m >= 0 && c > d->pile[m].cible))
        return echec(d, DECOMP_ERR_SEQUENCE);
    e = sommet(d, 0);
    if (!prefixer(d, d->nb - 1, "SI ") || !ajouter_ch(d, " ALORS ")) return 0;
    e->genre = ENT_MARQUE;
    e->sorte = SI_INSTRUCTION;
    e->rang = PH_ALORS;
    e->cible = c;
    return 1;
}

/* C1 à la position q, cible c. */
static int saut(Decompilation *d, unsigned int q, unsigned char c)
{
    int m = marque_haute(d);
    Entree *e;
    unsigned char ouvert;
    int i;

    if (m < 0) return echec(d, DECOMP_ERR_SEQUENCE);
    e = &d->pile[m];
    if (e->rang != PH_ALORS || e->cible != q + 2 || c < q + 2) return echec(d, DECOMP_ERR_SEQUENCE);
    for (i = m - 1; i >= 0; i--)
        if (d->pile[i].genre == ENT_MARQUE) {
            if (c > d->pile[i].cible) return echec(d, DECOMP_ERR_SEQUENCE);
            break;
        }
    if (d->nb == (unsigned int) m + 2 && d->pile[m + 1].genre == ENT_VALEUR) {
        if (d->pile[m + 1].sorte == VAL_COND) return echec(d, DECOMP_ERR_SEQUENCE);
        fondre(d, (unsigned int) m);
        e->sorte = SI_EXPRESSION;
    } else if (!branche(d, (unsigned int) m, 1, &ouvert))
        return 0;
    if (!ajouter_ch(d, " SINON ")) return 0;
    e->rang = PH_SINON;
    e->cible = c;
    return 1;
}

/* --- AFFICHER (§4.3) ------------------------------------------------------ */

/* La valeur du sommet devient l'élément suivant de la liste. */
static int element_liste(Decompilation *d)
{
    Entree *s = suite_ouverte(d, INS_AFF, INS_AFF_FMT);
    Entree *v;

    if (s == NULL || sommet(d, 0)->sorte == VAL_COND) return echec(d, DECOMP_ERR_SEQUENCE);
    v = sommet(d, 0);
    if (s->liste > 0) { if (!prefixer(d, d->nb - 1, ",")) return 0; }
    else if (s->sorte == INS_AFF) { if (!prefixer(d, d->nb - 1, " ")) return 0; }
    if (s->liste < 255) s->liste++;
    d->nb--;
    return 1;
}

/* Spécification de format : texte inséré avant ']'. */
static int specification(Decompilation *d, const char *s, unsigned int n)
{
    Entree *a;
    if (d->nb == 0) return echec(d, DECOMP_ERR_SEQUENCE);
    a = sommet(d, 0);
    if (a->genre != ENT_SUITE || a->sorte != INS_AFF_FMT) return echec(d, DECOMP_ERR_SEQUENCE);
    if (a->specs > 0 && !inserer(d, d->nb - 1, a->crochet, ",", 1)) return 0;
    if (!inserer(d, d->nb - 1, a->crochet, s, n)) return 0;
    if (a->specs < 255) a->specs++;
    return 1;
}

/* Facteur d'une spécification B0 à B4 : préfixe dans s ; une valeur variable
   devient d'abord l'élément suivant de la liste. */
static int facteur(Decompilation *d, unsigned char f, char *s, unsigned int *n)
{
    *n = 0;
    if (f & RCIM_FACTEUR_VARIABLE) {
        if ((f & 0x7F) != 0 || !element_liste(d)) return echec(d, DECOMP_ERR_SEQUENCE);
        s[(*n)++] = '*';
    } else if (f == 0)
        return echec(d, DECOMP_ERR_SEQUENCE);
    else if (f > 1)
        *n = ecrire_entier((long) f, s);
    return 1;
}

/* --- élément ------------------------------------------------------------- */

static int element(Decompilation *d, const unsigned char *o, unsigned int p, unsigned int longueur)
{
    TagRCIM t = o[p];
    char s[DECOMPILATEUR_LONGUEUR_MAX + 1];
    unsigned int n;
    unsigned int k;
    unsigned int j;
    unsigned int i0;
    int proc;
    Entree *e;

    /* valeurs immédiates */
    if (t >= RCIM_VAL_IMMEDIAT_BASE + RCIM_VAL_IMMEDIAT_MIN && t <= 0x3F) {
        long v = (long) t - RCIM_VAL_IMMEDIAT_BASE;
        return empiler_valeur(d, s, ecrire_entier(v, s), VAL_EXPR, v < 0 ? RANG_ADD : RANG_ATOME);
    }
    switch (t) {
    case RCIM_VAL_ENTIER_COURT: {
        long v = (long) (signed char) o[p + 1];
        return empiler_valeur(d, s, ecrire_entier(v, s), VAL_EXPR, v < 0 ? RANG_ADD : RANG_ATOME);
    }
    case RCIM_VAL_ENTIER: {
        long v = (long) rcim_lire_s16(o + p + 1);
        return empiler_valeur(d, s, ecrire_entier(v, s), VAL_EXPR, v < 0 ? RANG_ADD : RANG_ATOME);
    }
    case RCIM_VAL_REEL: {
        float f = rcim_lire_reel(o + p + 1);
        n = decompilateur_ecrire_reel(f, s);
        return empiler_valeur(d, s, n, VAL_EXPR, s[0] == '-' ? RANG_ADD : RANG_ATOME);
    }
    case RCIM_VAL_CHAINE:
        n = 0;
        if (o[p + 1] & RCIM_CHAINE_INDEX) {
            EntreeConstante c;
            if ((o[p + 1] & 0x7F) != 0) return echec(d, DECOMP_ERR_SEQUENCE);
            if (!constantes_lire(rcim_lire_u16(o + p + 2), &c)
                || !objets_copier(c.texte, 0, texte_constante, c.longueur))
                return echec(d, DECOMP_ERR_REFERENCE);
            if (!ecrire_chaine(d, texte_constante, c.longueur, s, &n)) return 0;
        } else if (!ecrire_chaine(d, o + p + 2, o[p + 1] & 0x7Fu, s, &n))
            return 0;
        return empiler_valeur(d, s, n, VAL_EXPR, RANG_ATOME);
    case RCIM_VAL_LIGNE:
        s[0] = '@';
        return empiler_valeur(d, s, 1 + ecrire_entier((long) o[p + 1], s + 1), VAL_EXPR, RANG_ATOME);

    /* références */
    case RCIM_REF_SIMPLE:
        if (!texte_nom(d, rcim_lire_u16(o + p + 1), s, &n, &proc)) return 0;
        return empiler_valeur(d, s, n, proc ? VAL_PROC : VAL_REF, RANG_ATOME);
    case RCIM_REF_INDEXEE:
        k = o[p + 3];
        if (k < 1 || k > 2 || !valeurs(d, k, 1)) return echec(d, DECOMP_ERR_SEQUENCE);
        if (!texte_nom(d, rcim_lire_u16(o + p + 1), s, &n, &proc)) return 0;
        if (proc) return echec(d, DECOMP_ERR_SEQUENCE);
        i0 = d->nb - k;
        if (k == 2 && !prefixer(d, i0 + 1, ",")) return 0;
        s[n++] = '[';
        s[n] = '\0';
        if (!prefixer(d, i0, s) || !ajouter(d, "]", 1)) return 0;
        fondre(d, i0);
        d->pile[i0].sorte = VAL_INDEXEE;
        d->pile[i0].rang = RANG_ATOME;
        return 1;
    case RCIM_REF_LIGNE_COURANTE:
        return empiler_valeur(d, "@", 1, VAL_EXPR, RANG_ATOME);
    case RCIM_REF_LIGNE_RETOUR:
        return empiler_valeur(d, "$", 1, VAL_EXPR, RANG_ATOME);

    /* opérations arithmétiques, relations, logique, concaténation */
    case RCIM_OP_ADD:      return binaire(d, "+", RANG_ADD, VAL_EXPR);
    case RCIM_OP_SUB:      return binaire(d, "-", RANG_ADD, VAL_EXPR);
    case RCIM_OP_MUL:      return binaire(d, "*", RANG_MUL, VAL_EXPR);
    case RCIM_OP_DIV:      return binaire(d, "/", RANG_MUL, VAL_EXPR);
    case RCIM_OP_PUIS:     return binaire(d, "^", RANG_PUIS, VAL_EXPR);
    case RCIM_OP_CONCAT:   return binaire(d, "!", RANG_CONCAT, VAL_EXPR);
    case RCIM_OP_EGAL:     return binaire(d, "=", RANG_REL, VAL_COND);
    case RCIM_OP_INF:      return binaire(d, "<", RANG_REL, VAL_COND);
    case RCIM_OP_INF_EGAL: return binaire(d, "<=", RANG_REL, VAL_COND);
    case RCIM_OP_DIFF:     return binaire(d, "#", RANG_REL, VAL_COND);
    case RCIM_OP_SUP:      return binaire(d, ">", RANG_REL, VAL_COND);
    case RCIM_OP_SUP_EGAL: return binaire(d, ">=", RANG_REL, VAL_COND);
    case RCIM_OP_ET:       return binaire(d, " ET ", RANG_ET, VAL_COND);
    case RCIM_OP_OU:       return binaire(d, " OU ", RANG_OU, VAL_COND);
    case RCIM_OP_NEG:
        if (!valeurs(d, 1, 1)) return 0;
        e = sommet(d, 0);
        if (e->rang < RANG_MUL && !parentheser(d, d->nb - 1)) return 0;
        if (!prefixer(d, d->nb - 1, "-")) return 0;
        e->sorte = VAL_EXPR;
        e->rang = RANG_ADD;
        return 1;

    /* fonctions */
    case RCIM_OP_RAC: case RCIM_OP_ABS: case RCIM_OP_EXP: case RCIM_OP_LGN:
    case RCIM_OP_SIN: case RCIM_OP_COS: case RCIM_OP_ATG: case RCIM_OP_ENT:
    case RCIM_OP_ALE:
        return fonction(d, noms_fonctions[t - RCIM_OP_RAC], 1);
    case RCIM_OP_SH: case RCIM_OP_CH: case RCIM_OP_TH: case RCIM_OP_SGN:
        return fonction(d, noms_t1600[t - RCIM_OP_SH], 1);
    case RCIM_OP_LGR: return fonction(d, "LGR", 1);
    case RCIM_OP_POS: return fonction(d, "POS", 3);
    case RCIM_OP_CCA: return fonction(d, "CCA", 1);
    case RCIM_OP_EQC: return fonction(d, "EQC", 1);
    case RCIM_OP_SKP: case RCIM_OP_PTR: case RCIM_OP_CNB: case RCIM_OP_EQN:
    case RCIM_OP_SCH: case RCIM_OP_GRL:
        if (o[p + 1] == 0) return echec(d, DECOMP_ERR_SEQUENCE);
        return fonction(d, noms_fonctions_n[t - RCIM_OP_SKP], o[p + 1]);
    case RCIM_OP_TEM:
    case RCIM_OP_DAT: {
        const char *nom = t == RCIM_OP_TEM ? "TEM" : "DAT";
        if (d->profil == PROFIL_T1600_V4)          /* IMP-0180, IMP-0189 */
            return empiler_valeur(d, nom, 3, VAL_EXPR, RANG_ATOME);
        return fonction(d, nom, 0);
    }

    /* appel de procédure : 40 idP [a1]...[ak] A8 position k */
    case RCIM_OP_APPEL:
        k = o[p + 2];
        if (o[p + 1] > RCIM_APPEL_EXPRESSION || !valeurs(d, k + 1, 0)) return echec(d, DECOMP_ERR_SEQUENCE);
        i0 = d->nb - k - 1;
        if (d->pile[i0].sorte != VAL_PROC) return echec(d, DECOMP_ERR_SEQUENCE);
        for (j = 1; j <= k; j++)
            if (d->pile[i0 + j].sorte == VAL_COND) return echec(d, DECOMP_ERR_SEQUENCE);
        for (j = k; j > 1; j--)
            if (!prefixer(d, i0 + j, ",")) return 0;
        if (!(k > 0 ? prefixer(d, i0 + 1, "(") : ajouter(d, "(", 1)) || !ajouter(d, ")", 1))
            return 0;
        fondre(d, i0);
        if (o[p + 1] == RCIM_APPEL_INSTRUCTION) return instruction(d, i0, INS_AUTRE, 0);
        d->pile[i0].sorte = VAL_EXPR;
        d->pile[i0].rang = RANG_ATOME;
        return 1;

    /* instructions */
    case RCIM_OP_AFFECTER:
        if (!valeurs(d, 2, 1)) return 0;
        i0 = d->nb - 2;
        if (d->pile[i0].sorte != VAL_REF && d->pile[i0].sorte != VAL_INDEXEE)
            return echec(d, DECOMP_ERR_SEQUENCE);
        if (!prefixer(d, i0 + 1, "_")) return 0;
        return instruction(d, i0, INS_AUTRE, 0);
    case RCIM_OP_UN:
    case RCIM_OP_ZER:
        if (!valeurs(d, 1, 1) || sommet(d, 0)->sorte != VAL_REF) return echec(d, DECOMP_ERR_SEQUENCE);
        if (!prefixer(d, d->nb - 1, t == RCIM_OP_UN ? "UN " : "ZER ")) return 0;
        return instruction(d, d->nb - 1, INS_AUTRE, 0);
    case RCIM_OP_TABLEAU:
        k = o[p + 3];
        if (k < 1 || k > 2 || !valeurs(d, k, 1)) return echec(d, DECOMP_ERR_SEQUENCE);
        if (!texte_nom(d, rcim_lire_u16(o + p + 1), s, &n, &proc)) return 0;
        if (proc) return echec(d, DECOMP_ERR_SEQUENCE);
        i0 = d->nb - k;
        if (k == 2 && !prefixer(d, i0 + 1, ",")) return 0;
        s[n++] = '[';
        s[n] = '\0';
        if (!prefixer(d, i0, s) || !ajouter(d, "]", 1)) return 0;
        fondre(d, i0);
        return declaration(d, i0, INS_TABLEAU, "TABLEAU ");
    case RCIM_OP_CHAINE:
    case RCIM_OP_LIBERER:
        if (!texte_nom(d, rcim_lire_u16(o + p + 1), s, &n, &proc)) return 0;
        if (proc || !empiler_valeur(d, s, n, VAL_REF, RANG_ATOME)) return proc ? echec(d, DECOMP_ERR_SEQUENCE) : 0;
        return t == RCIM_OP_CHAINE ? declaration(d, d->nb - 1, INS_CHAINE, "CHAINE ")
                                   : declaration(d, d->nb - 1, INS_LIBERER, "LIBERER ");
    case RCIM_OP_GARER:
        if (!valeurs(d, 3, 1) || sommet(d, 2)->sorte != VAL_REF) return echec(d, DECOMP_ERR_SEQUENCE);
        return instruction_liste(d, "GARER ", 3);
    case RCIM_OP_CHARGER:
        k = o[p + 1];
        if ((k != 3 && k != 4) || !valeurs(d, k, 1) || sommet(d, k - 1)->sorte != VAL_REF
            || (k == 4 && sommet(d, 0)->sorte != VAL_REF))
            return echec(d, DECOMP_ERR_SEQUENCE);
        return instruction_liste(d, "CHARGER ", k);
    case RCIM_OP_SUPPRIMER:
        k = o[p + 1];
        if (k != 1 && k != 2) return echec(d, DECOMP_ERR_SEQUENCE);
        return instruction_liste(d, "SUPPRIMER ", k);
    case RCIM_OP_EXECUTER:
        k = o[p + 1];
        if (k != 1 && k != 2) return echec(d, DECOMP_ERR_SEQUENCE);
        return instruction_liste(d, "EXECUTER ", k);

    /* AFFICHER */
    case RCIM_OP_AFF_TETE_SANS_FORMAT:
        if (!empiler(d, ENT_VALEUR, VAL_EXPR, 0) || !ajouter_ch(d, "AFFICHER")) return 0;
        return instruction(d, d->nb - 1, INS_AFF, 0);
    case RCIM_OP_AFF_TETE_FORMAT:
        if (!empiler(d, ENT_VALEUR, VAL_EXPR, 0) || !ajouter_ch(d, "AFFICHER[]")) return 0;
        return instruction(d, d->nb - 1, INS_AFF_FMT, 0);
    case RCIM_OP_AFF_VALEUR:
        return element_liste(d);
    case RCIM_OP_AFF_LIBELLE:
        if (!facteur(d, o[p + 1], s, &n)) return 0;
        if (!ecrire_chaine(d, o + p + 3, o[p + 2], s, &n)) return 0;
        return specification(d, s, n);
    case RCIM_OP_AFF_LIGNE:
    case RCIM_OP_AFF_ESPACE:
    case RCIM_OP_AFF_RETOUR:
    case RCIM_OP_AFF_SAUT:
        if (!facteur(d, o[p + 1], s, &n)) return 0;
        s[n++] = "/XCL"[t - RCIM_OP_AFF_LIGNE];
        return specification(d, s, n);
    case RCIM_OP_AFF_DESCRIPTION: {
        unsigned char b = o[p + 1];
        unsigned char type = (unsigned char) (b >> 6);
        unsigned int r;
        unsigned int q = p + 2;
        if ((b & 0x3F) == 0x3F) { r = o[q]; q++; } else r = (b & 0x3Fu) + 1;
        if (type > RCIM_DESC_U) return echec(d, DECOMP_ERR_SEQUENCE);
        n = r > 1 ? ecrire_entier((long) r, s) : 0;
        if (type == RCIM_DESC_U) s[n++] = 'U';
        else {
            s[n++] = type == RCIM_DESC_F ? 'F' : 'E';
            n += ecrire_entier((long) o[q], s + n);
            s[n++] = '.';
            n += ecrire_entier((long) o[q + 1], s + n);
        }
        return specification(d, s, n);
    }

    /* LIRE */
    case RCIM_CTL_LIRE_TETE:
        if (!empiler(d, ENT_VALEUR, VAL_EXPR, 0) || !ajouter_ch(d, "LIRE")) return 0;
        return instruction(d, d->nb - 1, INS_LIRE, 0);
    case RCIM_OP_LIRE_VALEUR: {
        Entree *l = suite_ouverte(d, INS_LIRE, INS_LIRE);
        e = sommet(d, 0);
        if (l == NULL || (e->sorte != VAL_REF && e->sorte != VAL_INDEXEE))
            return echec(d, DECOMP_ERR_SEQUENCE);
        if (!prefixer(d, d->nb - 1, l->liste > 0 ? "," : " ")) return 0;
        if (l->liste < 255) l->liste++;
        d->nb--;
        return 1;
    }

    /* contrôle */
    case RCIM_CTL_SI_FAUX:
        return si_faux(d, p, o[p + 1], longueur);
    case RCIM_CTL_SAUT:
        return saut(d, p, o[p + 1]);
    case RCIM_CTL_ALLER_EN:
    case RCIM_CTL_RETOUR_EN:
        n = (unsigned int) strlen(strcpy(s, t == RCIM_CTL_ALLER_EN ? "ALLER EN " : "RETOUR EN "));
        n += ecrire_entier((long) o[p + 1], s + n);
        if (!empiler_valeur(d, s, n, VAL_EXPR, 0)) return 0;
        return instruction(d, d->nb - 1, INS_AUTRE, 0);
    case RCIM_CTL_ALLER_EN_CALCULE:
    case RCIM_CTL_RETOUR_EN_CALCULE:
    case RCIM_CTL_RESULTAT:
        if (!valeurs(d, 1, 1)) return 0;
        if (!prefixer(d, d->nb - 1, t == RCIM_CTL_ALLER_EN_CALCULE ? "ALLER EN "
                                   : t == RCIM_CTL_RESULTAT ? "RESULTAT " : "RETOUR EN "))
            return 0;
        return instruction(d, d->nb - 1, INS_AUTRE, 0);
    case RCIM_CTL_RETOUR:
    case RCIM_CTL_TERMINER:
    case RCIM_CTL_PAUSE: {
        const char *mot = t == RCIM_CTL_RETOUR ? "RETOUR" : t == RCIM_CTL_TERMINER ? "TERMINER" : "PAUSE";
        if (!empiler_valeur(d, mot, (unsigned int) strlen(mot), VAL_EXPR, 0)) return 0;
        return instruction(d, d->nb - 1, INS_AUTRE, 0);
    }
    case RCIM_CTL_COMMENTAIRE:
        if (!empiler(d, ENT_VALEUR, VAL_EXPR, 0) || !ajouter(d, "*", 1)
            || !ajouter(d, (const char *) o + p + 2, o[p + 1]))
            return 0;
        return instruction(d, d->nb - 1, INS_AUTRE, 0);

    /* FAIRE : [n1] [v1] C9 idV {[p] CA | CE} {[v2] CF | [c] D0} ; [n1] D2 [c] D0 */
    case RCIM_CTL_FAIRE_OUVRIR:
        if (!valeurs(d, 2, 1)) return 0;
        if (!texte_nom(d, rcim_lire_u16(o + p + 1), s, &n, &proc)) return 0;
        if (proc) return echec(d, DECOMP_ERR_SEQUENCE);
        i0 = d->nb - 2;
        s[n++] = '_';
        s[n] = '\0';
        if (!prefixer(d, i0 + 1, s) || !prefixer(d, i0 + 1, " POUR ") || !prefixer(d, i0, "FAIRE "))
            return 0;
        return instruction(d, i0, INS_FAIRE, 0);
    case RCIM_CTL_FAIRE_OUVRIR_SEUL:
        if (!valeurs(d, 1, 1) || !prefixer(d, d->nb - 1, "FAIRE ")) return 0;
        return instruction(d, d->nb - 1, INS_FAIRE_P, 0);
    case RCIM_CTL_FAIRE_PAS:
        if (suite_ouverte(d, INS_FAIRE, INS_FAIRE) == NULL || !valeurs(d, 1, 1))
            return echec(d, DECOMP_ERR_SEQUENCE);
        if (!prefixer(d, d->nb - 1, " PAS ")) return 0;
        d->nb--;
        sommet(d, 0)->sorte = INS_FAIRE_P;
        return 1;
    case RCIM_CTL_FAIRE_PAS_IMPLICITE:
        if (d->nb == 0 || sommet(d, 0)->genre != ENT_SUITE || sommet(d, 0)->sorte != INS_FAIRE)
            return echec(d, DECOMP_ERR_SEQUENCE);
        sommet(d, 0)->sorte = INS_FAIRE_P;
        return 1;
    case RCIM_CTL_FAIRE_JUSQUA:
    case RCIM_CTL_FAIRE_TANT_QUE:
        if (suite_ouverte(d, INS_FAIRE_P, INS_FAIRE_P) == NULL) return echec(d, DECOMP_ERR_SEQUENCE);
        if ((sommet(d, 0)->sorte == VAL_COND) != (t == RCIM_CTL_FAIRE_TANT_QUE))
            return echec(d, DECOMP_ERR_SEQUENCE);
        if (!prefixer(d, d->nb - 1, t == RCIM_CTL_FAIRE_JUSQUA ? " JUSQUA " : " TANT QUE "))
            return 0;
        d->nb--;
        sommet(d, 0)->sorte = INS_AUTRE;
        return 1;

    /* déclaration de procédure, en tête de ligne seulement */
    case RCIM_CTL_PROCEDURE: {
        unsigned int np;
        unsigned int nl;
        unsigned int q;
        if (p != 0 || d->nb != 0) return echec(d, DECOMP_ERR_SEQUENCE);
        if (!texte_nom(d, rcim_lire_u16(o + p + 1), s, &n, &proc)) return 0;
        if (!proc) return echec(d, DECOMP_ERR_SEQUENCE);
        if (!empiler(d, ENT_VALEUR, VAL_EXPR, 0) || !ajouter_ch(d, "PROCEDURE ") || !ajouter(d, s, n)
            || !ajouter(d, "(", 1))
            return 0;
        np = o[p + 3];
        q = p + 4;
        for (j = 0; j < np; j++) {
            if ((j > 0 && !ajouter(d, ",", 1)) || !texte_nom(d, rcim_lire_u16(o + q), s, &n, &proc)
                || !ajouter(d, s, n))
                return 0;
            q += 2;
        }
        if (!ajouter(d, ")", 1)) return 0;
        nl = o[q];
        q++;
        for (j = 0; j < nl; j++) {
            if (!ajouter_ch(d, j == 0 ? " LOCAL " : ",") || !texte_nom(d, rcim_lire_u16(o + q), s, &n, &proc)
                || !ajouter(d, s, n))
                return 0;
            if (proc) return echec(d, DECOMP_ERR_SEQUENCE);
            q += 2;
        }
        return instruction(d, d->nb - 1, INS_AUTRE, 0);
    }

    default:   /* 0x04, 0x05, 0x98, 0xB9, 0xBA, réservés : jamais produits */
        return echec(d, DECOMP_ERR_SEQUENCE);
    }
}

/* --- point d'entrée (IMP-0198) ---------------------------------------- */

ResultatDecompilation decompilateur_decompiler(const EntreeObjetCompile *ligne,
                                               char *texte, unsigned int *longueur)
{
    Decompilation d;
    const unsigned char *o;
    unsigned int lg = ligne->longueur_octets;
    unsigned int p = 0;
    unsigned int n;
    char numero[8];

    d.texte = texte;
    d.lg = 0;
    d.nb = 0;
    d.erreur = DECOMP_OK;
    d.profil = configuration_session.profil;
    texte[0] = '\0';
    *longueur = 0;

    o = objets_resoudre(ligne->bloc);
    if (o == NULL) return DECOMP_ERR_REFERENCE;
    if (lg > RCIM_LONGUEUR_MAX_SEQUENCE || !rcim_sequence_valide(o, lg, NULL))
        echec(&d, DECOMP_ERR_SEQUENCE);
    n = ecrire_entier((long) ligne->numero_ligne, numero);
    if (lg == 0 || o[0] != RCIM_CTL_COMMENTAIRE)   /* commentaire en tête : collé (IMP-0183) */
        numero[n++] = ' ';
    if (d.erreur == DECOMP_OK && ajouter(&d, numero, n)) {
        while (p < lg) {
            if (!fermer_marques(&d, p) || !element(&d, o, p, lg)) break;
            p += rcim_longueur_element(o, lg, p);
        }
    }
    objets_relacher(ligne->bloc);
    if (d.erreur == DECOMP_OK && fermer_marques(&d, lg)) {
        if (d.nb > 1 || (d.nb == 1 && (d.pile[0].genre != ENT_SUITE || d.pile[0].sorte == INS_FAIRE
                                        || d.pile[0].sorte == INS_FAIRE_P)))
            echec(&d, DECOMP_ERR_SEQUENCE);
    }
    if (d.erreur != DECOMP_OK) {
        texte[0] = '\0';
        return d.erreur;
    }
    texte[d.lg] = '\0';
    *longueur = d.lg;
    return DECOMP_OK;
}
