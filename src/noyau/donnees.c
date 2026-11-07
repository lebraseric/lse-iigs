/* donnees.c — gestionnaire des variables-chaîne, des tableaux et de
   l'écriture des nombres
   EXI: EXI-0039  VM: VM-1102, VM-1201, VM-1202
   IV: Tome IV §4.8
   IMP: IMP-0208, IMP-0209 (chaînes), IMP-0210 (tableaux), IMP-0211
        (écriture U, F, E), IMP-0212 (fonctions sur les chaînes), IMP-0244
        (nombres sans SANE), IMP-0213, IMP-0243 (en-têtes, éléments et textes
        sources sans verrouillage), IMP-0280 (réels de 23 bits du T1600)
   Tests: tests/composants/composants_donnees.c
   Spécification : chapitre Donnees §2 à §6. Segment "donnees". */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "donnees.h"
#include "decompilateur.h"
#include "rcim.h"
#include "../interfaces/types_communs.h"
#include "../interfaces/profil.h"

segment "donnees";

#define TETE_TABLEAU 6u          /* genre, dimensions, d1, d2 (IMP-0210) */
#define ELEMENTS_MAX 65535ul
#define CHAINE_LONGUEUR_MAX 65535ul   /* 2 octets de longueur (IMP-0209) */

/* --- outils ---------------------------------------------------------------- */

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

static unsigned int lire_u16(const unsigned char *p)
{
    return (unsigned int) p[0] | ((unsigned int) p[1] << 8);
}

static void poser_u16(unsigned char *p, unsigned int v)
{
    p[0] = (unsigned char) (v & 0xFF);
    p[1] = (unsigned char) (v >> 8);
}

static int est_lettre(unsigned char c)
{
    return c >= 'A' && c <= 'Z';
}

static int est_chiffre(unsigned char c)
{
    return c >= '0' && c <= '9';
}

/* Texte d'une chaîne de la pile, résolu ; objets_relacher(c->objet) ensuite. */
static unsigned char *resoudre_texte(const ChaineSurPile *c)
{
    unsigned char *o = objets_resoudre(c->objet);
    return o == NULL ? NULL : o + c->decalage;
}

/* --- chaînes (IMP-0209) ------------------------------------------------------ */

/* Objet chaîne neuf de n caractères ; *texte : adresse résolue du texte. */
static NatureErreur nouvel_objet_chaine(unsigned long n, HandleObjet *h, unsigned char **texte)
{
    unsigned char *o;
    if (n > CHAINE_LONGUEUR_MAX) return ERR_E_CHAINE_TROP_LONGUE;
    *h = objets_creer(n + DONNEES_DECALAGE_TEXTE);
    if (*h == GESTOBJ_HANDLE_NUL) return ERR_E_MEMOIRE_SATUREE;
    o = objets_resoudre(*h);
    o[0] = DONNEES_GENRE_CHAINE;
    poser_u16(o + 1, (unsigned int) n);
    *texte = o + DONNEES_DECALAGE_TEXTE;
    return ERR_AUCUNE;
}

static void en_transitoire(HandleObjet h, unsigned int n, ChaineSurPile *res)
{
    res->objet = h;
    res->decalage = DONNEES_DECALAGE_TEXTE;
    res->longueur = n;
    res->transitoire = 1;
}

NatureErreur donnees_chaine_creer(const unsigned char *texte, unsigned int n, ChaineSurPile *res)
{
    HandleObjet h;
    unsigned char *t;
    NatureErreur e = nouvel_objet_chaine(n, &h, &t);
    if (e != ERR_AUCUNE) return e;
    if (n > 0) memcpy(t, texte, n);
    objets_relacher(h);
    en_transitoire(h, n, res);
    return ERR_AUCUNE;
}

/* Sous-chaîne [debut, debut + n) d'une chaîne de la pile, en chaîne transitoire. */
static NatureErreur extraire(const ChaineSurPile *a, unsigned int debut, unsigned int n,
                             ChaineSurPile *res)
{
    HandleObjet h;
    unsigned char *t;
    NatureErreur e = nouvel_objet_chaine(n, &h, &t);
    if (e != ERR_AUCUNE) return e;
    if (n > 0) objets_copier(a->objet, (unsigned long) a->decalage + debut, t, n);   /* IMP-0243 */
    objets_relacher(h);
    en_transitoire(h, n, res);
    return ERR_AUCUNE;
}

NatureErreur donnees_chaine_vers_objet(const ChaineSurPile *c, HandleObjet *objet)
{
    ChaineSurPile r;
    NatureErreur e;
    if (c->transitoire) { *objet = c->objet; return ERR_AUCUNE; }   /* reprise (§4.4) */
    if ((e = extraire(c, 0, c->longueur, &r)) != ERR_AUCUNE) return e;
    *objet = r.objet;
    return ERR_AUCUNE;
}

/* En-têtes lus par objets_copier, sans verrouillage (IMP-0243). */
NatureErreur donnees_objet_vers_chaine(HandleObjet objet, ChaineSurPile *c)
{
    unsigned char o[DONNEES_DECALAGE_TEXTE];
    if (!objets_copier(objet, 0, o, DONNEES_DECALAGE_TEXTE) || o[0] != DONNEES_GENRE_CHAINE)
        return ERR_E_INTERNE;
    c->objet = objet;
    c->decalage = DONNEES_DECALAGE_TEXTE;
    c->longueur = lire_u16(o + 1);
    c->transitoire = 0;
    return ERR_AUCUNE;
}

static unsigned long taille_objet(const unsigned char *o)
{
    unsigned long n;
    if (o[0] == DONNEES_GENRE_CHAINE) return DONNEES_DECALAGE_TEXTE + (unsigned long) lire_u16(o + 1);
    n = (unsigned long) lire_u16(o + 2) * (unsigned long) lire_u16(o + 4);
    return TETE_TABLEAU + (n + 7) / 8 + 4 * n;
}

NatureErreur donnees_copier(HandleObjet objet, HandleObjet *copie)
{
    unsigned char *o;
    unsigned char *c;
    unsigned long n;

    o = objets_resoudre(objet);
    if (o == NULL) return ERR_E_INTERNE;
    n = taille_objet(o);
    objets_relacher(objet);
    *copie = objets_creer(n);                  /* aucune résolution ouverte à travers la création */
    if (*copie == GESTOBJ_HANDLE_NUL) return ERR_E_MEMOIRE_SATUREE;
    o = objets_resoudre(objet);
    c = objets_resoudre(*copie);
    memcpy(c, o, (size_t) n);
    objets_relacher(*copie);
    objets_relacher(objet);
    return ERR_AUCUNE;
}

unsigned int donnees_texte(const ChaineSurPile *c, unsigned int debut, unsigned char *dest,
                           unsigned int max)
{
    unsigned int n;
    if (debut >= c->longueur) return 0;
    n = c->longueur - debut;
    if (n > max) n = max;
    if (!objets_copier(c->objet, (unsigned long) c->decalage + debut, dest, n)) return 0;   /* IMP-0243 */
    return n;
}

NatureErreur donnees_concatener(const ChaineSurPile *a, const ChaineSurPile *b, ChaineSurPile *res)
{
    HandleObjet h;
    unsigned char *t;
    unsigned long n = (unsigned long) a->longueur + b->longueur;
    NatureErreur e = nouvel_objet_chaine(n, &h, &t);
    if (e != ERR_AUCUNE) return e;
    /* textes sources copiés sans verrouillage (IMP-0243) ; t reste résolu */
    if (a->longueur > 0) objets_copier(a->objet, a->decalage, t, a->longueur);
    if (b->longueur > 0) objets_copier(b->objet, b->decalage, t + a->longueur, b->longueur);
    objets_relacher(h);
    en_transitoire(h, (unsigned int) n, res);
    return ERR_AUCUNE;
}

int donnees_comparer(const ChaineSurPile *a, const ChaineSurPile *b)
{
    unsigned char *sa = a->longueur > 0 ? resoudre_texte(a) : NULL;
    unsigned char *sb = b->longueur > 0 ? resoudre_texte(b) : NULL;
    unsigned int i;
    int r = 0;

    for (i = 0; i < a->longueur && i < b->longueur && r == 0; i++)
        if (sa[i] != sb[i]) r = sa[i] < sb[i] ? -1 : 1;
    if (r == 0 && a->longueur != b->longueur) r = a->longueur < b->longueur ? -1 : 1;
    if (sa != NULL) objets_relacher(a->objet);
    if (sb != NULL) objets_relacher(b->objet);
    return r;
}

/* --- fonctions de LSE15 §4-4.2 (IMP-0212) ----------------------------------- */

/* Position de départ β, 1 ≤ β ≤ longueur + 1 ; rang 0 en sortie. */
static NatureErreur depart(const ChaineSurPile *a, float beta, NatureErreur au_dela, unsigned int *rang)
{
    long b = arrondir(beta);
    if (b < 1) return ERR_E_POSITION_ERRONEE;
    if (b > (long) a->longueur + 1) return au_dela;
    *rang = (unsigned int) (b - 1);
    return ERR_AUCUNE;
}

static int dans(const unsigned char *liste, unsigned int n, unsigned char c)
{
    unsigned int i;
    for (i = 0; i < n; i++) if (liste[i] == c) return 1;
    return 0;
}

/* Fin de la sous-chaîne de SCH, rang du premier caractère non inclus. */
static NatureErreur fin_sch(const ChaineSurPile *a, unsigned int r, const ChaineSurPile *arret,
                            float longueur, unsigned int *fin)
{
    long n;
    unsigned char *s;
    unsigned char *l;
    unsigned int i;

    if (arret == NULL) {
        n = arrondir(longueur);
        if (n < 0) return ERR_E_LONGUEUR_NEGATIVE;
        *fin = (unsigned long) r + (unsigned long) n > a->longueur ? a->longueur : r + (unsigned int) n;
        return ERR_AUCUNE;
    }
    i = r;
    if (a->longueur > 0 && arret->longueur > 0) {
        s = resoudre_texte(a);
        l = resoudre_texte(arret);
        while (i < a->longueur && !dans(l, arret->longueur, s[i])) i++;
        objets_relacher(arret->objet);
        objets_relacher(a->objet);
    } else
        i = a->longueur;
    *fin = i;
    return ERR_AUCUNE;
}

NatureErreur donnees_sch(const ChaineSurPile *a, float beta, const ChaineSurPile *arret,
                         float longueur, ChaineSurPile *res, float *delta)
{
    unsigned int r;
    unsigned int fin;
    NatureErreur e;
    if ((e = depart(a, beta, ERR_E_POSITION_AU_DELA, &r)) != ERR_AUCUNE) return e;
    if ((e = fin_sch(a, r, arret, longueur, &fin)) != ERR_AUCUNE) return e;
    if (delta != NULL) *delta = (float) (fin + 1);
    return extraire(a, r, fin - r, res);
}

/* Lettres de GRL : début et fin (rangs). */
static void bornes_grl(const ChaineSurPile *a, unsigned int r, unsigned int *deb, unsigned int *fin)
{
    unsigned char *s;
    unsigned int i = r;
    if (a->longueur == 0) { *deb = *fin = 0; return; }
    s = resoudre_texte(a);
    while (i < a->longueur && !est_lettre(s[i])) i++;
    *deb = i;
    while (i < a->longueur && est_lettre(s[i])) i++;
    *fin = i;
    objets_relacher(a->objet);
}

NatureErreur donnees_grl(const ChaineSurPile *a, float beta, ChaineSurPile *res, float *delta)
{
    unsigned int r;
    unsigned int deb;
    unsigned int fin;
    NatureErreur e;
    if ((e = depart(a, beta, ERR_E_POSITION_ERRONEE, &r)) != ERR_AUCUNE) return e;
    bornes_grl(a, r, &deb, &fin);
    if (deb > fin) deb = fin;
    if (delta != NULL) *delta = (float) (fin + 1);
    return extraire(a, deb, fin - deb, res);
}

NatureErreur donnees_ptr(const ChaineSurPile *a, float beta, int avec_gamma,
                         const ChaineSurPile *arret, float longueur, float *res)
{
    unsigned int r;
    unsigned int deb;
    unsigned int fin;
    NatureErreur e;
    if ((e = depart(a, beta, ERR_E_POSITION_AU_DELA, &r)) != ERR_AUCUNE) return e;
    if (avec_gamma) {
        if ((e = fin_sch(a, r, arret, longueur, &fin)) != ERR_AUCUNE) return e;
    } else
        bornes_grl(a, r, &deb, &fin);
    *res = (float) (fin + 1);
    return ERR_AUCUNE;
}

NatureErreur donnees_skp(const ChaineSurPile *a, float beta, const ChaineSurPile *liste, float *res)
{
    unsigned int r;
    unsigned int i;
    unsigned char *s;
    unsigned char *l = NULL;
    NatureErreur e;
    if ((e = depart(a, beta, ERR_E_POSITION_ERRONEE, &r)) != ERR_AUCUNE) return e;
    i = r;
    if (a->longueur > 0) {
        s = resoudre_texte(a);
        if (liste != NULL && liste->longueur > 0) l = resoudre_texte(liste);
        if (liste == NULL)
            while (i < a->longueur && !est_lettre(s[i])) i++;
        else
            while (i < a->longueur && l != NULL && dans(l, liste->longueur, s[i])) i++;
        if (l != NULL) objets_relacher(liste->objet);
        objets_relacher(a->objet);
    }
    *res = (float) (i + 1);
    return ERR_AUCUNE;
}

NatureErreur donnees_pos(const ChaineSurPile *a, float beta, const ChaineSurPile *g, float *res)
{
    unsigned int r;
    unsigned int i;
    unsigned char *s;
    unsigned char *t;
    NatureErreur e;
    if ((e = depart(a, beta, ERR_E_POSITION_ERRONEE, &r)) != ERR_AUCUNE) return e;
    if (g->longueur == 0) { *res = (float) (r + 1); return ERR_AUCUNE; }   /* LSE15 */
    *res = 0.0f;
    if (g->longueur > a->longueur) return ERR_AUCUNE;
    s = resoudre_texte(a);
    t = resoudre_texte(g);
    for (i = r; i + g->longueur <= a->longueur; i++)
        if (memcmp(s + i, t, g->longueur) == 0) { *res = (float) (i + 1); break; }
    objets_relacher(g->objet);
    objets_relacher(a->objet);
    return ERR_AUCUNE;
}

NatureErreur donnees_cnb(const ChaineSurPile *a, float beta, float *res, float *gamma)
{
    unsigned int r;
    unsigned int i;
    unsigned int j;
    unsigned int debut;
    unsigned char *s = NULL;
    char tampon[80];
    unsigned int n;
    double v;
    NatureErreur e;

    if ((e = depart(a, beta, ERR_E_POSITION_AU_DELA, &r)) != ERR_AUCUNE) return e;
    *res = 0.0f;
    i = r;
    if (a->longueur > 0) s = resoudre_texte(a);
#define C(k) ((k) < a->longueur ? s[(k)] : 0)
    while (C(i) == ' ') i++;
    debut = i;
    if (C(i) == '+' || C(i) == '-') i++;
    if (!est_chiffre(C(i))) {                          /* LSE15 : arrêt, valeur nulle */
        i = debut;
    } else {
        while (est_chiffre(C(i))) i++;
        if (C(i) == '.') { i++; while (est_chiffre(C(i))) i++; }   /* premier point seulement */
        if (C(i) == 'E') {                                          /* exposant bien constitué */
            j = i + 1;
            if (C(j) == '+' || C(j) == '-') j++;
            if (est_chiffre(C(j))) { while (est_chiffre(C(j))) j++; i = j; }
        }
        n = i - debut;
        if (n >= sizeof tampon) n = sizeof tampon - 1;
        memcpy(tampon, s + debut, n);
        tampon[n] = '\0';
        v = strtod(tampon, NULL);
        donnees_ranger_reel_borne(v, res);            /* LSE15 : plus grand nombre représentable */
    }
#undef C
    if (s != NULL) objets_relacher(a->objet);
    if (gamma != NULL) *gamma = (float) (i + 1);
    return ERR_AUCUNE;
}

NatureErreur donnees_eqn(const ChaineSurPile *a, float beta, float *res)
{
    long b = arrondir(beta);
    unsigned char *s;
    if (b < 1 || b > (long) a->longueur + 1) return ERR_E_POSITION_EQN;
    if (b == (long) a->longueur + 1) { *res = -1.0f; return ERR_AUCUNE; }    /* LSE15 */
    s = resoudre_texte(a);
    *res = (float) s[b - 1];
    objets_relacher(a->objet);
    return ERR_AUCUNE;
}

NatureErreur donnees_eqc(float v, ChaineSurPile *res)
{
    long n = arrondir(v);
    unsigned char c;
    long max = PROFIL_EST_T1600(configuration_session.profil) ? 127 : 255;   /* plages EQC */
    if (n < 0 || n > max) return ERR_E_EQC_HORS_BORNES;
    c = (unsigned char) n;
    return donnees_chaine_creer(&c, 1, res);
}

NatureErreur donnees_cca(float v, ChaineSurPile *res)
{
    char s[16];
    unsigned int n = decompilateur_ecrire_reel_chiffres(v, 7, s);   /* U sans espace final (IMP-0211) */
    return donnees_chaine_creer((const unsigned char *) s, n, res);
}

/* --- écriture des nombres (IMP-0211) --------------------------------------- */

static double puissance_dix(int k)
{
    double p = 1.0;
    while (k > 0) { p = p * 10.0; k--; }
    return p;
}

static double plancher(double x)
{
    return floor(x);
}

/* Chiffres décimaux de l'entier positif x (double) dans s ; renvoie leur nombre. */
static unsigned int chiffres_entier(double x, char *s)
{
    char t[320];
    unsigned int n = 0;
    unsigned int i;
    double q;
    int c;
    do {
        q = plancher(x / 10.0);
        c = (int) (x - q * 10.0 + 0.5);              /* borné : inexact au-delà de 2^53 (§4) */
        t[n++] = (char) ('0' + (c < 0 ? 0 : c > 9 ? 9 : c));
        x = q;
    } while (x >= 1.0 && n < sizeof t);
    for (i = 0; i < n; i++) s[i] = t[n - 1 - i];
    return n;
}

/* Chiffres de x sur exactement k positions, zéros de tête compris. */
static void chiffres_fixes(double x, unsigned int k, char *s)
{
    unsigned int i;
    double q;
    int c;
    for (i = k; i > 0; i--) {
        q = plancher(x / 10.0);
        c = (int) (x - q * 10.0 + 0.5);              /* borné : inexact au-delà de 2^53 (§4) */
        s[i - 1] = (char) ('0' + (c < 0 ? 0 : c > 9 ? 9 : c));
        x = q;
    }
}

unsigned int donnees_ecrire_nombre(float v, unsigned char type, unsigned char e, unsigned char d,
                                   char *sortie)
{
    float f = v;                               /* copie : paramètre au format extended */
    double x = (double) f;
    int negatif = x < 0.0;
    char corps[320];
    unsigned int lg = 0;
    unsigned int c;
    unsigned int k;
    double p;
    double r;
    double ent;
    int expo = 0;

    if (negatif) x = -x;
    if (type == RCIM_DESC_U) {
        lg = decompilateur_ecrire_reel_chiffres(f, 7, sortie);   /* IMP-0211 : sept chiffres */
        sortie[lg++] = ' ';
        sortie[lg] = '\0';
        return lg;
    }
    p = puissance_dix(d);
    if (type == RCIM_DESC_E && x != 0.0) {           /* mantisse de 1 à 10 exclu */
        while (x >= 10.0) { x = x / 10.0; expo++; }
        while (x < 1.0) { x = x * 10.0; expo--; }
    }
    r = plancher(x * p + 0.5);                        /* arrondi, moitié loin de zéro */
    if (type == RCIM_DESC_E && r >= 10.0 * p) { r = plancher(r / 10.0 + 0.5); expo++; }
    ent = plancher(r / p);
    k = 0;
    if (negatif) corps[k++] = '-';
    k += chiffres_entier(ent, corps + k);
    c = k;                                            /* signe et chiffres avant le point */
    if (d > 0) {
        corps[k++] = '.';
        chiffres_fixes(r - ent * p, d, corps + k);
        k += d;
    }
    if (type == RCIM_DESC_E) {
        corps[k++] = 'E';
        corps[k++] = expo < 0 ? '-' : '+';
        if (expo < 0) expo = -expo;
        corps[k++] = (char) ('0' + expo / 10);
        corps[k++] = (char) ('0' + expo % 10);
    }
    while (c < e) { sortie[lg++] = ' '; c++; }
    memcpy(sortie + lg, corps, k);
    lg += k;
    sortie[lg] = '\0';
    return lg;
}

/* --- tableaux (IMP-0210) ---------------------------------------------------- */

static unsigned long nb_elements(const unsigned char *o)
{
    return (unsigned long) lire_u16(o + 2) * (unsigned long) lire_u16(o + 4);
}

NatureErreur donnees_creer_tableau(unsigned char dims, float d1, float d2, HandleObjet *tableau)
{
    long b1 = arrondir(d1);
    long b2 = dims == 2 ? arrondir(d2) : 1;
    unsigned long n;
    unsigned long taille;
    unsigned char *o;

    if (b1 < 1 || b2 < 1) return ERR_E_DIMENSION_NULLE;
    if (b1 > 65535L || b2 > 65535L) return ERR_E_TABLEAU_TROP_GRAND;
    n = (unsigned long) b1 * (unsigned long) b2;
    if (n > ELEMENTS_MAX) return ERR_E_TABLEAU_TROP_GRAND;
    taille = TETE_TABLEAU + (n + 7) / 8 + 4 * n;
    *tableau = objets_creer(taille);
    if (*tableau == GESTOBJ_HANDLE_NUL) return ERR_E_MEMOIRE_SATUREE;
    o = objets_resoudre(*tableau);
    o[0] = DONNEES_GENRE_TABLEAU;
    o[1] = dims;
    poser_u16(o + 2, (unsigned int) b1);
    poser_u16(o + 4, (unsigned int) b2);
    memset(o + TETE_TABLEAU, 0, (size_t) ((n + 7) / 8));   /* aucun élément défini */
    objets_relacher(*tableau);
    return ERR_AUCUNE;
}

NatureErreur donnees_tableau_bornes(HandleObjet tableau, unsigned char *dims,
                                    unsigned int *d1, unsigned int *d2)
{
    unsigned char o[TETE_TABLEAU];
    if (!objets_copier(tableau, 0, o, TETE_TABLEAU) || o[0] != DONNEES_GENRE_TABLEAU)
        return ERR_E_INTERNE;
    *dims = o[1];
    *d1 = lire_u16(o + 2);
    *d2 = lire_u16(o + 4);
    return ERR_AUCUNE;
}

int donnees_image_tableau_valide(const unsigned char *t, unsigned long lg)
{
    unsigned long d1;
    unsigned long d2;
    unsigned long n;

    if (lg < TETE_TABLEAU || t[0] != DONNEES_GENRE_TABLEAU || (t[1] != 1 && t[1] != 2)) return 0;
    d1 = lire_u16(t + 2);
    d2 = lire_u16(t + 4);
    if (d1 < 1 || d2 < 1 || (t[1] == 1 && d2 != 1)) return 0;
    n = d1 * d2;
    return n <= ELEMENTS_MAX && lg == TETE_TABLEAU + (n + 7) / 8 + 4 * n;
}

static int entier_exact(const float *a, long *v, int *negatif);

/* Un indice entier exact est pris tel quel, arrondir(n) valant n ; les autres
   sont arrondis (IMP-0179 ; IMP-0244, audit du code, AP-02). */
static long indice(const float *x)
{
    long v;
    int negatif;
    if (entier_exact(x, &v, &negatif)) return v;
    return arrondir(*x);
}

NatureErreur donnees_rang(HandleObjet tableau, unsigned char nb_indices, float i1, float i2,
                          unsigned int *rang)
{
    float a = i1;
    float b = i2;
    return donnees_rang_exact(tableau, nb_indices, &a, &b, rang);
}

NatureErreur donnees_rang_exact(HandleObjet tableau, unsigned char nb_indices, const float *i1,
                                const float *i2, unsigned int *rang)
{
    unsigned char dims;
    unsigned int d1;
    unsigned int d2;
    long a;
    long b = 1;
    NatureErreur e;

    if ((e = donnees_tableau_bornes(tableau, &dims, &d1, &d2)) != ERR_AUCUNE) return e;
    if (nb_indices != dims) return ERR_E_NOMBRE_INDICES;
    a = indice(i1);
    if (dims == 2) b = indice(i2);
    if (a < 1 || b < 1) return ERR_E_INDICE_NUL;
    if (a > (long) d1 || b > (long) d2) return ERR_E_INDICE_TROP_GRAND;
    *rang = (unsigned int) ((unsigned long) (a - 1) * d2 + (unsigned long) (b - 1));   /* ligne par ligne */
    return ERR_AUCUNE;
}

/* Élément lu et écrit par objets_copier et objets_ecrire, sans verrouillage
   (IMP-0243) : en-tête, octet des marques, puis les quatre octets du nombre. */
NatureErreur donnees_lire_element(HandleObjet tableau, unsigned int rang, float *v)
{
    unsigned char o[TETE_TABLEAU];
    unsigned char marques;
    unsigned long n;

    if (!objets_copier(tableau, 0, o, TETE_TABLEAU)) return ERR_E_INTERNE;
    n = nb_elements(o);
    if (rang >= n || !objets_copier(tableau, TETE_TABLEAU + rang / 8, &marques, 1)) return ERR_E_INTERNE;
    if (!(marques & (1u << (rang % 8)))) return ERR_E_ELEMENT_SANS_VALEUR;
    if (!objets_copier(tableau, TETE_TABLEAU + (n + 7) / 8 + 4ul * rang, (unsigned char *) v, 4))
        return ERR_E_INTERNE;
    return ERR_AUCUNE;
}

NatureErreur donnees_ecrire_element(HandleObjet tableau, unsigned int rang, float v)
{
    float f = v;
    return donnees_ecrire_element_exact(tableau, rang, &f);
}

NatureErreur donnees_ecrire_element_exact(HandleObjet tableau, unsigned int rang, const float *v)
{
    unsigned char o[TETE_TABLEAU];
    unsigned char marques;
    unsigned long n;

    if (!objets_copier(tableau, 0, o, TETE_TABLEAU)) return ERR_E_INTERNE;
    n = nb_elements(o);
    if (rang >= n || !objets_copier(tableau, TETE_TABLEAU + rang / 8, &marques, 1)) return ERR_E_INTERNE;
    marques |= (unsigned char) (1u << (rang % 8));
    if (!objets_ecrire(tableau, TETE_TABLEAU + rang / 8, &marques, 1)
        || !objets_ecrire(tableau, TETE_TABLEAU + (n + 7) / 8 + 4ul * rang, (const unsigned char *) v, 4))
        return ERR_E_INTERNE;
    return ERR_AUCUNE;
}

NatureErreur donnees_remplir(HandleObjet tableau, float v)
{
    float f = v;
    unsigned char *o = objets_resoudre(tableau);
    unsigned long n;
    unsigned long i;

    if (o == NULL) return ERR_E_INTERNE;
    if (o[0] != DONNEES_GENRE_TABLEAU) { objets_relacher(tableau); return ERR_E_INTERNE; }
    n = nb_elements(o);
    memset(o + TETE_TABLEAU, 0xFF, (size_t) ((n + 7) / 8));
    for (i = 0; i < n; i++)
        memcpy(o + TETE_TABLEAU + (n + 7) / 8 + 4ul * i, &f, 4);
    objets_relacher(tableau);
    return ERR_AUCUNE;
}

/* --- nombres : réalisations exactes, sans SANE (IMP-0244) ------------------ */

typedef union {
    float         f;
    unsigned long u;
} BitsNombre;

#define BIT_SIGNE    0x80000000ul
#define LIMITE_EXACT 0x1000000l          /* 2^24 */
#define LIMITE_23    0x800000l           /* 2^23 : entiers exacts du T1600 (IMP-0280) */
#define REELS_23     PROFIL_EST_T1600(configuration_session.profil)

/* Plus grand réel : FLT_MAX ; sous le profil T1600, (2^23 - 1) x 2^105. */
#define PLUS_GRAND   0x7F7FFFFFul
#define PLUS_GRAND_23 0x7F7FFFFEul

/* Valeur arrondie à 23 bits rangée en float, sans nouvel arrondi. Le plus grand
   réel de 23 bits étant (2^23 - 1) x 2^105, un dépassement donne 2^128, que la
   conversion rend infini : il se reconnaît aux bits, sans comparaison SANE
   (deux comparaisons en format étendu coûtaient 0,43 ms sur l'Apple IIGS). */
static NatureErreur ranger_23(const long double *z, float *r)
{
    BitsNombre x;

    x.f = (float) *z;
    if ((x.u & 0x7FFFFFFFul) >= 0x7F800000ul) return ERR_E_DEPASSEMENT;
    *r = x.f;
    return ERR_AUCUNE;
}

NatureErreur donnees_ranger_reel(double v, float *r)
{
    long double z;

    if (!REELS_23) {
        if (v > FLT_MAX || v < -FLT_MAX) return ERR_E_DEPASSEMENT;       /* IMP-0216 */
        *r = (float) v;
        return ERR_AUCUNE;
    }
    z = v;
    rcim_arrondir_23(&z);                                                 /* IMP-0280 */
    return ranger_23(&z, r);
}

void donnees_ranger_reel_borne(double v, float *r)
{
    BitsNombre x;

    if (donnees_ranger_reel(v, r) == ERR_AUCUNE) return;
    x.u = REELS_23 ? PLUS_GRAND_23 : PLUS_GRAND;
    if (v < 0.0) x.u |= BIT_SIGNE;
    *r = x.f;
}

/* Puissances de deux : un décalage de long d'une quantité variable est une
   boucle sous ORCA/C (0,033 ms mesurée sur l'Apple IIGS le 2026-10-25). */
static const unsigned long puissance[25] = {
    0x1ul, 0x2ul, 0x4ul, 0x8ul, 0x10ul, 0x20ul, 0x40ul, 0x80ul,
    0x100ul, 0x200ul, 0x400ul, 0x800ul, 0x1000ul, 0x2000ul, 0x4000ul, 0x8000ul,
    0x10000ul, 0x20000ul, 0x40000ul, 0x80000ul, 0x100000ul, 0x200000ul, 0x400000ul,
    0x800000ul, 0x1000000ul
};

void donnees_nombre_entier(long v, float *r)
{
    BitsNombre x;
    unsigned long a;
    unsigned long signe = 0;
    int n;
    long limite = REELS_23 ? LIMITE_23 : LIMITE_EXACT;

    if (v >= limite || v <= -limite) {
        (void) donnees_ranger_reel((double) v, r);   /* hors du domaine exact : SANE */
        return;
    }
    if (v < 0) {
        signe = BIT_SIGNE;
        a = (unsigned long) -v;
    } else {
        a = (unsigned long) v;
    }
    if (a == 0) {
        x.u = 0;                             /* +0, comme (float) 0 */
        *r = x.f;
        return;
    }
    n = 0;                                   /* bit de poids fort, par dichotomie */
    if (a >= puissance[16]) n = 16;
    if (a >= puissance[n + 8]) n += 8;
    if (a >= puissance[n + 4]) n += 4;
    if (a >= puissance[n + 2]) n += 2;
    if (a >= puissance[n + 1]) n += 1;
    if (n < 23) a = a << (23 - n);
    x.u = signe | ((unsigned long) (127 + n) << 23) | (a & 0x7FFFFFul);
    *r = x.f;
}

/* Clé d'ordre : l'ordre des valeurs finies suit le signe et la valeur absolue
   de la représentation ; -0 et +0 donnent la même clé. */
static long cle(const float *a)
{
    BitsNombre x;
    x.f = *a;
    if (x.u & BIT_SIGNE) return -(long) (x.u & 0x7FFFFFFFul);
    return (long) x.u;
}

int donnees_comparer_nombres(const float *a, const float *b)
{
    long ka = cle(a);
    long kb = cle(b);
    return ka < kb ? -1 : ka > kb ? 1 : 0;
}

/* Entier exact de valeur absolue inférieure à 2^24 : 1, *v et *negatif
   (signe, utile pour -0) renseignés ; 0 sinon. */
static int entier_exact(const float *a, long *v, int *negatif)
{
    BitsNombre x;
    unsigned long m;
    int e;
    int fraction;

    x.f = *a;
    *negatif = (x.u & BIT_SIGNE) != 0;
    if ((x.u & 0x7FFFFFFFul) == 0) {
        *v = 0;
        return 1;
    }
    e = (int) ((x.u >> 23) & 0xFF) - 127;
    if (e < 0 || e > 23) return 0;
    m = (x.u & 0x7FFFFFul) | 0x800000ul;
    fraction = 23 - e;
    if (fraction > 0) {
        if ((m & (puissance[fraction] - 1)) != 0) return 0;
        m = m >> fraction;
    }
    *v = (long) m;
    if (*negatif) *v = -*v;
    return 1;
}

int donnees_operation_entiere(char op, const float *a, const float *b, float *r)
{
    long va;
    long vb;
    long res;
    int na;
    int nb;
    BitsNombre zero;
    long limite = REELS_23 ? LIMITE_23 : LIMITE_EXACT;    /* IMP-0280 */

    if (!entier_exact(a, &va, &na) || !entier_exact(b, &vb, &nb)) return 0;
    if (op == '*') {
        if (va != 0 && vb != 0) {
            if ((va < 0 ? -va : va) >= limite / (vb < 0 ? -vb : vb) + 1) return 0;
        }
        res = va * vb;
        if (res == 0) {                      /* signe d'un produit nul (IEEE) */
            zero.u = na != nb ? BIT_SIGNE : 0;
            *r = zero.f;
            return 1;
        }
    } else {
        if (op == '-') {
            vb = -vb;
            nb = !nb;
        }
        res = va + vb;
        if (res == 0) {                      /* somme nulle : -0 seulement pour -0 + -0 */
            zero.u = (va == 0 && vb == 0 && na && nb) ? BIT_SIGNE : 0;
            *r = zero.f;
            return 1;
        }
    }
    if (res >= limite || res <= -limite) return 0;
    donnees_nombre_entier(res, r);
    return 1;
}

void donnees_opposer(const float *a, float *r)
{
    BitsNombre x;
    x.f = *a;
    x.u ^= BIT_SIGNE;
    *r = x.f;
}

/* Calcul en format étendu (long double, SANE), arrondi directement en float :
   même float que le calcul en double d'IMP-0216, le double arrondi étant sans
   effet pour + - * / dès que le format intermédiaire a au moins 2 x 24 + 2
   bits (53 pour double, 64 pour le format étendu). Le dépassement (EX 44)
   reste décidé sur le résultat en double, qui peut excéder FLT_MAX quand le
   float obtenu vaut encore FLT_MAX (FLT_MAX + 2^80) : la question ne se pose
   que si le float obtenu vaut +-FLT_MAX ou l'infini, et l'opération est
   alors refaite par le calcul en double d'IMP-0216 (audit du code, AP-04).
   Sous le profil T1600, le résultat étendu est arrondi à 23 bits (IMP-0280),
   puis comparé au plus grand float. */
NatureErreur donnees_operation_etendue(char op, const float *a, const float *b, float *r)
{
    long double x = *a;
    long double y = *b;
    long double z;
    double u;
    double v;
    double w;
    BitsNombre res;
    BitsNombre diviseur;

    if (op == '/') {
        diviseur.f = *b;
        if ((diviseur.u & 0x7FFFFFFFul) == 0) return ERR_E_DIVISION_PAR_ZERO;
    }
    switch (op) {
    case '+': z = x + y; break;
    case '-': z = x - y; break;
    case '*': z = x * y; break;
    default:  z = x / y; break;
    }
    if (REELS_23) {                                     /* IMP-0280 */
        rcim_arrondir_23(&z);
        return ranger_23(&z, r);
    }
    res.f = (float) z;
    if ((res.u & 0x7FFFFFFFul) >= 0x7F7FFFFFul) {      /* +-FLT_MAX ou infini */
        u = *a;
        v = *b;
        switch (op) {
        case '+': w = u + v; break;
        case '-': w = u - v; break;
        case '*': w = u * v; break;
        default:  w = u / v; break;
        }
        if (w > FLT_MAX || w < -FLT_MAX) return ERR_E_DEPASSEMENT;
        res.f = (float) w;
    }
    *r = res.f;
    return ERR_AUCUNE;
}
