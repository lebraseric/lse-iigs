/* fichiers.c — fichiers-données : enregistrements en secteurs, catalogage
   EXI: EXI-0053 à EXI-0057
   VM: VM-3404, VM-3405, VM-3407
   IV: IV-IF-0005, IV-IF-0006
   IMP: IMP-0247 (secteurs de 256 octets, répertoire chaîné, étendues),
        IMP-0253 (GARER, CHARGER, SUPPRIMER, CATALOGUER, espace temporaire),
        IMP-0255 (droits et protections), IMP-0256 (espace temporaire d'une
        identification)
   Tests: tests/composants/composants_fichiers.c
   Spécification : chapitre Bibliothécaire et fichiers §4, §7 ter.
   Segment "fichiers" (IMP-0249). */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include "fichiers.h"
#include "bibliotheque.h"
#include "../interfaces/interface_fichiers.h"
#include "../interfaces/profil.h"

segment "fichiers";

#define SECTEUR        256u
#define ENTETE         16u
#define NB_ENTREES     20u
#define TAILLE_ENTREE  12u
#define ETAT_LIBRE     0xFE    /* étendue libre */
#define ETAT_INUTILISE 0xFF    /* entrée sans enregistrement */

typedef struct {
    unsigned int  numero;
    unsigned char etat;
    unsigned int  premier;
    unsigned int  nb;
    unsigned long longueur;
} Entree;

typedef struct {
    int           trouve;
    unsigned int  secteur;     /* secteur de répertoire */
    unsigned int  rang;        /* rang de l'entrée dans ce secteur */
    Entree        e;
} Place;

static unsigned long standard = FICHIERS_QUOTA_DEFAUT;
static unsigned long quota = FICHIERS_QUOTA_DEFAUT;

void fichiers_fixer_quota(unsigned long secteurs)
{
    standard = secteurs;
    quota = secteurs;
}

void fichiers_fixer_espace(unsigned long secteurs)
{
    quota = secteurs;
}

unsigned long fichiers_espace_standard(void)
{
    return standard;
}

unsigned long fichiers_espace(void)
{
    return quota;
}

/* --- octets ----------------------------------------------------------------- */

static unsigned int lu16(const unsigned char *t)
{
    return (unsigned int) t[0] | ((unsigned int) t[1] << 8);
}

static void pu16(unsigned char *t, unsigned int v)
{
    t[0] = (unsigned char) (v & 0xFF);
    t[1] = (unsigned char) (v >> 8);
}

static void lire_entree(const unsigned char *s, unsigned int k, Entree *e)
{
    const unsigned char *p = s + ENTETE + k * TAILLE_ENTREE;
    e->numero = lu16(p);
    e->etat = p[2];
    e->premier = lu16(p + 4);
    e->nb = lu16(p + 6);
    e->longueur = (unsigned long) lu16(p + 8) | ((unsigned long) lu16(p + 10) << 16);
}

static void poser_entree(unsigned char *s, unsigned int k, const Entree *e)
{
    unsigned char *p = s + ENTETE + k * TAILLE_ENTREE;
    pu16(p, e->numero);
    p[2] = e->etat;
    p[3] = 0;
    pu16(p + 4, e->premier);
    pu16(p + 6, e->nb);
    pu16(p + 8, (unsigned int) (e->longueur & 0xFFFFul));
    pu16(p + 10, (unsigned int) (e->longueur >> 16));
}

static void secteur_vierge(unsigned char *s, int premier)
{
    unsigned int k;
    Entree e;
    memset(s, 0, SECTEUR);
    if (premier) {
        memcpy(s, "LSED", 4);
        pu16(s + 6, 1);
    }
    s[4] = 1;                                   /* version */
    e.numero = 0;
    e.etat = ETAT_INUTILISE;
    e.premier = 0;
    e.nb = 0;
    e.longueur = 0;
    for (k = 0; k < NB_ENTREES; k++) poser_entree(s, k, &e);
}

/* --- accès ------------------------------------------------------------------ */

static NatureErreur lire_secteur(FichierPlateforme f, unsigned int n, unsigned char *s)
{
    return plateforme_fichier_lire(f, (unsigned long) n * SECTEUR, s, SECTEUR) == PLAT_OK
           ? ERR_AUCUNE : ERR_F_FICHIER_ENDOMMAGE;
}

static NatureErreur ecrire_secteur(FichierPlateforme f, unsigned int n, const unsigned char *s)
{
    int r = plateforme_fichier_ecrire(f, (unsigned long) n * SECTEUR, s, SECTEUR);
    if (r == PLAT_OK) return ERR_AUCUNE;
    return r == PLAT_PLEIN ? ERR_E_FICHIER_PLEIN : ERR_F_ENTREE_SORTIE;
}

/* nb secteurs consécutifs, en une écriture (16 au plus : 4 096 octets). */
static NatureErreur ecrire_secteurs(FichierPlateforme f, unsigned int n, const unsigned char *s,
                                    unsigned int nb)
{
    int r = plateforme_fichier_ecrire(f, (unsigned long) n * SECTEUR, s, nb * SECTEUR);
    if (r == PLAT_OK) return ERR_AUCUNE;
    return r == PLAT_PLEIN ? ERR_E_FICHIER_PLEIN : ERR_F_ENTREE_SORTIE;
}

static EspaceFichier espace(int permanent)
{
    if (permanent == FICHIERS_UNITE) return ESPACE_UNITE_DONNEES;
    return permanent ? ESPACE_DONNEES : ESPACE_TEMPORAIRES;
}

/* Bibliothèque ouverte (IMP-0250), et unité présente pour un fichier de l'unité. */
static NatureErreur disponible(int permanent)
{
    if (!plateforme_bibliotheque_disponible()) return ERR_F_BIBLIOTHEQUE;
    if (permanent == FICHIERS_UNITE && !plateforme_unite_disponible()) return ERR_F_UNITE;
    return ERR_AUCUNE;
}

/* Ouvre un fichier-données existant et vérifie son secteur 0. */
static NatureErreur ouvrir(EspaceFichier esp, const char *nom, ModeFichier m, FichierPlateforme *f)
{
    unsigned char s[SECTEUR];
    int r = plateforme_fichier_ouvrir(esp, nom, m, f);
    if (r != PLAT_OK) return r == PLAT_INEXISTANT ? ERR_E_FICHIER_INEXISTANT : ERR_F_ENTREE_SORTIE;
    if (lire_secteur(*f, 0, s) != ERR_AUCUNE || memcmp(s, "LSED", 4) != 0) {
        plateforme_fichier_fermer(*f);
        return ERR_F_FICHIER_ENDOMMAGE;
    }
    return ERR_AUCUNE;
}

/* Remet le nombre de secteurs du fichier dans le secteur 0. */
static NatureErreur fixer_taille(FichierPlateforme f, unsigned int nb)
{
    unsigned char s[SECTEUR];
    NatureErreur e;
    if ((e = lire_secteur(f, 0, s)) != ERR_AUCUNE) return e;
    pu16(s + 6, nb);
    return ecrire_secteur(f, 0, s);
}

/* Réécrit l'entrée de p et, si q n'est pas nul, celle de q, du même secteur de
   répertoire : une seule écriture (IMP-0247). */
static NatureErreur ecrire_place(FichierPlateforme f, const Place *p, const Entree *e,
                                 const Place *q, const Entree *g)
{
    unsigned char s[SECTEUR];
    NatureErreur n;
    if ((n = lire_secteur(f, p->secteur, s)) != ERR_AUCUNE) return n;
    poser_entree(s, p->rang, e);
    if (q != NULL) poser_entree(s, q->rang, g);
    return ecrire_secteur(f, p->secteur, s);
}

/* Première entrée inutilisée du secteur de répertoire de p ; elle décrira la fin
   d'une étendue libre découpée (IMP-0247). */
static NatureErreur inutilisee_voisine(FichierPlateforme f, const Place *p, Place *u)
{
    unsigned char s[SECTEUR];
    unsigned int k;
    NatureErreur n;

    u->trouve = 0;
    if ((n = lire_secteur(f, p->secteur, s)) != ERR_AUCUNE) return n;
    for (k = 0; k < NB_ENTREES; k++) {
        lire_entree(s, k, &u->e);
        if (u->e.etat == ETAT_INUTILISE) {
            u->trouve = 1; u->secteur = p->secteur; u->rang = k;
            return ERR_AUCUNE;
        }
    }
    return ERR_AUCUNE;
}

/* Parcours du répertoire : entrée du numéro, première entrée inutilisée,
   première étendue libre d'au moins besoin secteurs, dernier secteur de
   répertoire ; *taille reçoit le nombre de secteurs du fichier. */
static NatureErreur parcourir(FichierPlateforme f, unsigned int numero, unsigned int besoin,
                              Place *num, Place *vide, Place *libre, unsigned int *dernier,
                              unsigned int *taille)
{
    unsigned char s[SECTEUR];
    unsigned int sect = 0;
    unsigned int k;
    unsigned int tours = 0;
    Entree e;
    NatureErreur n;

    num->trouve = 0;
    vide->trouve = 0;
    libre->trouve = 0;
    for (;;) {
        if ((n = lire_secteur(f, sect, s)) != ERR_AUCUNE) return n;
        if (sect == 0) *taille = lu16(s + 6);
        for (k = 0; k < NB_ENTREES; k++) {
            lire_entree(s, k, &e);
            if (e.etat <= ENREG_CHAINE && e.numero == numero && !num->trouve) {
                num->trouve = 1; num->secteur = sect; num->rang = k; num->e = e;
            } else if (e.etat == ETAT_INUTILISE && !vide->trouve) {
                vide->trouve = 1; vide->secteur = sect; vide->rang = k; vide->e = e;
            } else if (e.etat == ETAT_LIBRE && e.nb >= besoin && !libre->trouve) {
                libre->trouve = 1; libre->secteur = sect; libre->rang = k; libre->e = e;
            }
        }
        *dernier = sect;
        sect = lu16(s + 8);
        if (sect == 0) return ERR_AUCUNE;
        tours++;
        if (tours > 4096u || sect >= *taille) return ERR_F_FICHIER_ENDOMMAGE;
    }
}

/* Ajoute un secteur de répertoire en fin de fichier, chaîné au dernier ;
   *p désigne sa première entrée. */
static NatureErreur nouveau_repertoire(FichierPlateforme f, unsigned int dernier, unsigned int *taille,
                                       Place *p)
{
    unsigned char s[SECTEUR];
    NatureErreur n;
    unsigned int neuf = *taille;

    secteur_vierge(s, 0);
    if ((n = ecrire_secteur(f, neuf, s)) != ERR_AUCUNE) return n;
    *taille = neuf + 1;
    if ((n = fixer_taille(f, *taille)) != ERR_AUCUNE) return n;
    if ((n = lire_secteur(f, dernier, s)) != ERR_AUCUNE) return n;
    pu16(s + 8, neuf);
    if ((n = ecrire_secteur(f, dernier, s)) != ERR_AUCUNE) return n;
    p->trouve = 1;
    p->secteur = neuf;
    p->rang = 0;
    return ERR_AUCUNE;
}

/* Secteurs occupés par l'espace temporaire (point C1). */
unsigned long fichiers_secteurs_temporaires(void)
{
    char nom[16];
    unsigned int rang = 0;
    unsigned long total = 0;
    FichierPlateforme f;

    while (plateforme_fichier_lister(ESPACE_TEMPORAIRES, rang, nom)) {
        if (plateforme_fichier_ouvrir(ESPACE_TEMPORAIRES, nom, FICHIER_LECTURE, &f) == PLAT_OK) {
            total = total + (plateforme_fichier_taille(f) + SECTEUR - 1) / SECTEUR;
            plateforme_fichier_fermer(f);
        }
        rang++;
    }
    return total;
}

/* --- GARER (IMP-0247) ---------------------------------------------------------- */

static NatureErreur garer_ouvert(FichierPlateforme f, int controle_quota, unsigned int numero,
                                 unsigned char type, const unsigned char *contenu,
                                 unsigned long longueur)
{
    unsigned char s[SECTEUR];
    unsigned int besoin = (unsigned int) ((longueur + SECTEUR - 1) / SECTEUR);
    unsigned int taille;
    unsigned int dernier;
    unsigned int i;
    unsigned int pleins;
    unsigned int bloc;
    unsigned long reste;
    unsigned long croissance = 0;
    Place num;
    Place vide;
    Place libre;
    Place reprise;                 /* étendue libre reprise */
    Place pos_rec;
    Place pos_ancien;
    Place pos_fin;                 /* entrée qui décrira la fin d'une étendue découpée */
    Entree rec;
    Entree ancien;
    Entree fin;
    Entree inutilisee;
    NatureErreur n;

    if ((n = parcourir(f, numero, besoin, &num, &vide, &libre, &dernier, &taille)) != ERR_AUCUNE)
        return n;
    rec.numero = numero;
    rec.etat = type;
    rec.longueur = longueur;
    reprise.trouve = 0;
    pos_fin.trouve = 0;
    if (besoin == 0) {
        rec.premier = 0;
        rec.nb = 0;
    } else if (libre.trouve) {
        reprise = libre;
        rec.premier = libre.e.premier;
        rec.nb = libre.e.nb;
        if (libre.e.nb > besoin) {     /* découpe, si le secteur a une entrée inutilisée */
            if ((n = inutilisee_voisine(f, &libre, &pos_fin)) != ERR_AUCUNE) return n;
            if (pos_fin.trouve) {
                rec.nb = besoin;
                fin.numero = 0;
                fin.etat = ETAT_LIBRE;
                fin.premier = libre.e.premier + besoin;
                fin.nb = libre.e.nb - besoin;
                fin.longueur = 0;
            }
        }
    } else {
        rec.premier = taille;
        rec.nb = besoin;
        croissance = besoin;
    }
    /* place de l'entrée de l'enregistrement, puis de l'ancienne étendue */
    pos_rec.trouve = 0;
    pos_ancien.trouve = 0;
    if (num.trouve) pos_rec = num;
    else if (reprise.trouve) pos_rec = reprise;
    else if (vide.trouve) { pos_rec = vide; vide.trouve = 0; }
    if (num.trouve && num.e.nb > 0) {
        if (reprise.trouve) pos_ancien = reprise;
        else if (vide.trouve) { pos_ancien = vide; vide.trouve = 0; }
    }
    if (!pos_rec.trouve) croissance++;
    if (num.trouve && num.e.nb > 0 && !pos_ancien.trouve && pos_rec.trouve) croissance++;
    if (controle_quota && croissance > 0                        /* IMP-0253 : seule une écriture qui agrandit */
        && fichiers_secteurs_temporaires() + croissance > quota) return ERR_E_FICHIER_PLEIN;
    if ((unsigned long) taille + croissance > 0xFFFFul) return ERR_E_FICHIER_PLEIN;   /* numéros u16 */

    /* 1. contenu, dans son étendue : les secteurs pleins en écritures de 16
       secteurs au plus, directement depuis le contenu (audit du code, AP-17) ;
       le dernier secteur partiel et ceux d'une étendue reprise entière qui
       dépassent le contenu, complétés de zéros (AC-36) */
    pleins = (unsigned int) (longueur / SECTEUR);
    if (pleins > rec.nb) pleins = rec.nb;
    for (i = 0; i < pleins; i = i + bloc) {
        bloc = pleins - i > 16u ? 16u : pleins - i;
        if ((n = ecrire_secteurs(f, rec.premier + i, contenu + (unsigned long) i * SECTEUR, bloc))
            != ERR_AUCUNE) return n;
    }
    for (i = pleins; i < rec.nb; i++) {
        memset(s, 0, SECTEUR);
        if ((unsigned long) i * SECTEUR < longueur) {
            reste = longueur - (unsigned long) i * SECTEUR;
            memcpy(s, contenu + (unsigned long) i * SECTEUR, reste > SECTEUR ? SECTEUR : (unsigned int) reste);
        }
        if ((n = ecrire_secteur(f, rec.premier + i, s)) != ERR_AUCUNE) return n;
    }
    if (rec.premier == taille && rec.nb > 0) {
        taille = taille + rec.nb;
        if ((n = fixer_taille(f, taille)) != ERR_AUCUNE) return n;
    }
    /* 2. entrées. L'étendue reprise cesse d'abord d'être décrite comme libre, et la
       fin d'une étendue découpée est décrite par la même écriture ; puis vient
       l'entrée de l'enregistrement, qui ne fait qu'une avec la première quand
       l'enregistrement est nouveau. L'ancien contenu cesse d'être valide ici ;
       jusqu'à l'étape 3, un incident ne peut que perdre des secteurs, sans jamais
       décrire une étendue à la fois vivante et libre (IMP-0247, VM-3405). */
    if (reprise.trouve && num.trouve) {
        inutilisee.numero = 0;
        inutilisee.etat = ETAT_INUTILISE;
        inutilisee.premier = 0;
        inutilisee.nb = 0;
        inutilisee.longueur = 0;
        if ((n = ecrire_place(f, &reprise, &inutilisee, pos_fin.trouve ? &pos_fin : NULL, &fin)) != ERR_AUCUNE)
            return n;
    }
    if (!pos_rec.trouve && (n = nouveau_repertoire(f, dernier, &taille, &pos_rec)) != ERR_AUCUNE) return n;
    if ((n = ecrire_place(f, &pos_rec, &rec, pos_fin.trouve && !num.trouve ? &pos_fin : NULL, &fin)) != ERR_AUCUNE)
        return n;
    /* 3. l'ancienne étendue devient libre */
    if (num.trouve && num.e.nb > 0) {
        ancien = num.e;
        ancien.numero = 0;
        ancien.etat = ETAT_LIBRE;
        ancien.longueur = 0;
        if (!pos_ancien.trouve) {
            if ((n = parcourir(f, 0xFFFFu, 0xFFFFu, &num, &vide, &libre, &dernier, &taille)) != ERR_AUCUNE)
                return n;
            if (vide.trouve) pos_ancien = vide;
            else if ((n = nouveau_repertoire(f, dernier, &taille, &pos_ancien)) != ERR_AUCUNE) return n;
        }
        if ((n = ecrire_place(f, &pos_ancien, &ancien, NULL, NULL)) != ERR_AUCUNE) return n;
    }
    return ERR_AUCUNE;
}

NatureErreur fichiers_garer(const char *nom, int permanent, unsigned int numero,
                            unsigned char type, const unsigned char *contenu,
                            unsigned long longueur)
{
    unsigned char s[SECTEUR];
    FichierPlateforme f;
    EspaceFichier esp = espace(permanent);
    NatureErreur n;
    int cree = 0;
    int unite = permanent == FICHIERS_UNITE;
    unsigned char protection = 0;

    if ((n = disponible(permanent)) != ERR_AUCUNE) return n;
    if (permanent && (n = bibliotheque_droit_donnees(nom, unite, &protection)) != ERR_AUCUNE) return n;
    n = ouvrir(esp, nom, FICHIER_MISE_A_JOUR, &f);
    if (n == ERR_E_FICHIER_INEXISTANT) {
        if (!permanent && fichiers_secteurs_temporaires() + 1 > quota) return ERR_E_FICHIER_PLEIN;
        if (permanent && (n = bibliotheque_nom_pris(unite, 0, nom)) != ERR_AUCUNE) return n;   /* B2 */
        if (plateforme_fichier_ouvrir(esp, nom, FICHIER_CREATION, &f) != PLAT_OK) return ERR_F_ENTREE_SORTIE;
        secteur_vierge(s, 1);
        if ((n = ecrire_secteur(f, 0, s)) != ERR_AUCUNE) {
            plateforme_fichier_fermer(f);
            plateforme_fichier_supprimer(esp, nom);
            return n;
        }
        cree = 1;
    } else if (n != ERR_AUCUNE) {
        return n;
    }
    n = garer_ouvert(f, !permanent, numero, type, contenu, longueur);
    plateforme_fichier_fermer(f);
    if (cree) {
        if (n != ERR_AUCUNE) plateforme_fichier_supprimer(esp, nom);
        else if (permanent) {
            n = bibliotheque_cataloguer(unite, CATALOGUE_DONNEES, nom, bibliotheque_compte(), protection);
            if (n != ERR_AUCUNE) plateforme_fichier_supprimer(esp, nom);   /* jamais sans propriétaire */
        }
    }
    return n;
}

/* --- CHARGER, SUPPRIMER ------------------------------------------------------------ */

static NatureErreur charger_ouvert(FichierPlateforme f, unsigned int numero, unsigned char *type,
                                   unsigned char **contenu, unsigned long *longueur)
{
    Place num;
    Place vide;
    Place libre;
    unsigned int dernier;
    unsigned int taille;
    unsigned long p = 0;
    unsigned int bloc;
    unsigned char *t;
    NatureErreur n;

    if ((n = parcourir(f, numero, 0xFFFFu, &num, &vide, &libre, &dernier, &taille)) != ERR_AUCUNE) return n;
    if (!num.trouve) return ERR_E_ENREGISTREMENT_INEXISTANT;
    t = (unsigned char *) malloc((size_t) (num.e.longueur > 0 ? num.e.longueur : 1));
    if (t == NULL) return ERR_E_MEMOIRE_SATUREE;
    while (p < num.e.longueur) {
        bloc = num.e.longueur - p > 4096ul ? 4096u : (unsigned int) (num.e.longueur - p);
        if (plateforme_fichier_lire(f, (unsigned long) num.e.premier * SECTEUR + p, t + p, bloc) != PLAT_OK) {
            free(t);
            return ERR_F_FICHIER_ENDOMMAGE;
        }
        p = p + bloc;
    }
    *type = num.e.etat;
    *contenu = t;
    *longueur = num.e.longueur;
    return ERR_AUCUNE;
}

NatureErreur fichiers_charger(const char *nom, int permanent, unsigned int numero,
                              unsigned char *type, unsigned char **contenu,
                              unsigned long *longueur)
{
    FichierPlateforme f;
    int unite = permanent == FICHIERS_UNITE;
    NatureErreur n;

    if ((n = disponible(permanent)) != ERR_AUCUNE) return n;
    n = ERR_E_FICHIER_INEXISTANT;
    if (!permanent) n = ouvrir(ESPACE_TEMPORAIRES, nom, FICHIER_LECTURE, &f);   /* priorité */
    if (n == ERR_E_FICHIER_INEXISTANT) {
        if (bibliotheque_lecture_interdite(CATALOGUE_DONNEES, nom, unite)) return ERR_E_FICHIER_PROTEGE;
        n = ouvrir(unite ? ESPACE_UNITE_DONNEES : ESPACE_DONNEES, nom, FICHIER_LECTURE, &f);
    }
    if (n != ERR_AUCUNE) return n;
    n = charger_ouvert(f, numero, type, contenu, longueur);
    plateforme_fichier_fermer(f);
    return n;
}

NatureErreur fichiers_supprimer(const char *nom, int permanent, unsigned int numero)
{
    FichierPlateforme f;
    EspaceFichier esp = espace(permanent);
    Place num;
    Place vide;
    Place libre;
    unsigned int dernier;
    unsigned int taille;
    Entree e;
    NatureErreur n;
    int r;
    int unite = permanent == FICHIERS_UNITE;
    unsigned char protection;

    if ((n = disponible(permanent)) != ERR_AUCUNE) return n;
    if (permanent && plateforme_fichier_existe(esp, nom)
        && (n = bibliotheque_droit_donnees(nom, unite, &protection)) != ERR_AUCUNE)
        return n;
    if (numero == 0) {
        if ((r = plateforme_fichier_supprimer(esp, nom)) != PLAT_OK)
            return r == PLAT_INEXISTANT ? ERR_E_FICHIER_INEXISTANT : ERR_F_ENTREE_SORTIE;
        if (permanent) bibliotheque_decataloguer(unite, CATALOGUE_DONNEES, nom);
        return ERR_AUCUNE;
    }
    if ((n = ouvrir(esp, nom, FICHIER_MISE_A_JOUR, &f)) != ERR_AUCUNE) return n;
    n = parcourir(f, numero, 0xFFFFu, &num, &vide, &libre, &dernier, &taille);
    if (n == ERR_AUCUNE && !num.trouve) n = ERR_E_ENREGISTREMENT_INEXISTANT;
    if (n == ERR_AUCUNE) {
        e = num.e;
        e.numero = 0;
        e.etat = e.nb > 0 ? ETAT_LIBRE : ETAT_INUTILISE;
        e.longueur = 0;
        n = ecrire_place(f, &num, &e, NULL, NULL);
    }
    plateforme_fichier_fermer(f);
    return n;
}

/* --- CATALOGUER (VM-3407) -------------------------------------------------------- */

NatureErreur fichiers_cataloguer(const char *temporaire, const char *permanent,
                                 unsigned char protection, int unite)
{
    EspaceFichier donnees = unite ? ESPACE_UNITE_DONNEES : ESPACE_DONNEES;
    unsigned char s[SECTEUR];
    FichierPlateforme ft;
    FichierPlateforme fp;
    unsigned int sect = 0;
    unsigned int k;
    unsigned char type;
    unsigned char *contenu;
    unsigned long longueur;
    Entree e;
    NatureErreur n;
    unsigned char creee;

    if (unite && !plateforme_unite_disponible()) return ERR_F_UNITE;
    if ((n = bibliotheque_droit_donnees(permanent, unite, &creee)) != ERR_AUCUNE) return n;
    if (creee != 0) protection = creee;
    if ((n = bibliotheque_nom_pris(unite, 0, permanent)) != ERR_AUCUNE) return n;   /* point B2 */
    if ((n = ouvrir(ESPACE_TEMPORAIRES, temporaire, FICHIER_LECTURE, &ft)) != ERR_AUCUNE)
        return n == ERR_E_FICHIER_INEXISTANT ? ERR_F_FICHIER_INEXISTANT : n;
    if (plateforme_fichier_ouvrir(donnees, permanent, FICHIER_CREATION, &fp) != PLAT_OK) {
        plateforme_fichier_fermer(ft);
        return ERR_F_ENTREE_SORTIE;
    }
    secteur_vierge(s, 1);
    n = ecrire_secteur(fp, 0, s);
    while (n == ERR_AUCUNE) {                   /* recopie sans les étendues libres */
        if ((n = lire_secteur(ft, sect, s)) != ERR_AUCUNE) break;
        for (k = 0; k < NB_ENTREES && n == ERR_AUCUNE; k++) {
            lire_entree(s, k, &e);
            if (e.etat > ENREG_CHAINE) continue;
            if ((n = charger_ouvert(ft, e.numero, &type, &contenu, &longueur)) != ERR_AUCUNE) break;
            n = garer_ouvert(fp, 0, e.numero, type, contenu, longueur);
            free(contenu);
        }
        sect = lu16(s + 8);
        if (sect == 0) break;
    }
    plateforme_fichier_fermer(ft);
    plateforme_fichier_fermer(fp);
    if (n != ERR_AUCUNE) {
        plateforme_fichier_supprimer(donnees, permanent);
        return n == ERR_E_FICHIER_PLEIN ? ERR_F_SUPPORT_PLEIN : n;
    }
    n = bibliotheque_cataloguer(unite, CATALOGUE_DONNEES, permanent, bibliotheque_compte(), protection);
    if (n != ERR_AUCUNE) {                      /* le temporaire reste intact (AC-21) */
        plateforme_fichier_supprimer(donnees, permanent);
        return n;
    }
    plateforme_fichier_supprimer(ESPACE_TEMPORAIRES, temporaire);
    return ERR_AUCUNE;
}

unsigned long fichiers_secteurs(int permanent, const char *nom)
{
    FichierPlateforme f;
    unsigned long n;
    if (plateforme_fichier_ouvrir(espace(permanent), nom, FICHIER_LECTURE, &f) != PLAT_OK) return 0;
    n = (plateforme_fichier_taille(f) + SECTEUR - 1) / SECTEUR;
    plateforme_fichier_fermer(f);
    return n;
}
