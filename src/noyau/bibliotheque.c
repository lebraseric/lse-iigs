/* bibliotheque.c — bibliothécaire : fichiers-programmes (RCI-S) et catalogue
   EXI: EXI-0009, EXI-0022, EXI-0025, EXI-0049
   VM: VM-2209, VM-3401 à VM-3404
   IV: IV-IF-0007
   IMP: IMP-0245 (bibliothèque), IMP-0246 (RCI-S), IMP-0248 (catalogue),
        IMP-0249 (interface, segment "bibliothe"), IMP-0254 (identifications),
        IMP-0255 (droits et protections), IMP-0258 (DUPLIQUER), IMP-0278 (« $ »)
   Tests: tests/composants/composants_bibliotheque.c
   Spécification : chapitre Bibliothécaire et fichiers §2, §3, §5. */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include "bibliotheque.h"
#include "programme.h"
#include "symboles.h"
#include "contexte_exec.h"
#include "rcim.h"
#include "objets.h"
#include "../interfaces/types_communs.h"
#include "../interfaces/interface_horloge.h"
#include "../interfaces/profil.h"

segment "bibliothe";

#define RCIS_VERSION    1
#define RCIS_TRANSIT    "TRANSIT"   /* nom provisoire de MODIFIER : 7 lettres, hors des noms L.S.E. */
#define RCIS_ANCIEN     "ANCIEN"    /* original mis de côté par MODIFIER : 6 lettres, idem */
#define CATALOGUE_NOM   "CATALOGUE"
#define CATALOGUE_TAILLE 16
#define IDENT_NOM       "IDENTIFICATIONS"
#define IDENT_TAILLE    6           /* occupée, numéro (2), attributs, secteurs (2) */

static NatureErreur erreur_plateforme(int r)
{
    if (r == PLAT_INEXISTANT) return ERR_F_FICHIER_INEXISTANT;
    if (r == PLAT_PLEIN) return ERR_F_SUPPORT_PLEIN;
    return ERR_F_ENTREE_SORTIE;
}

/* Noms uniques (point B2, révisé le 2026-11-06) : au sein de chaque sorte sous
   les profils T1600 (INRDP75 : SUPPRIMER ω,P|D, GARER '#…') ; programmes et
   fichiers-données confondus sous le profil MITRA 15 (LSE15 §9-3, SUPPRIMER ω
   sans sorte). La sorte demandée est contrôlée la première. */
NatureErreur bibliotheque_nom_pris(int unite, int programme, const char *nom)
{
    EspaceFichier p = unite ? ESPACE_UNITE_PROGRAMMES : ESPACE_PROGRAMMES;
    EspaceFichier d = unite ? ESPACE_UNITE_DONNEES : ESPACE_DONNEES;

    if (plateforme_fichier_existe(programme ? p : d, nom))
        return programme ? ERR_F_PROGRAMME_EXISTANT : ERR_F_DONNEES_EXISTANT;
    if (configuration_session.profil == PROFIL_MITRA15_V8 && plateforme_fichier_existe(programme ? d : p, nom))
        return programme ? ERR_F_DONNEES_EXISTANT : ERR_F_PROGRAMME_EXISTANT;
    return ERR_AUCUNE;
}

int bibliotheque_nom_valide(const char *nom, unsigned int n)
{
    unsigned int i;
    char c;

    if (n < 1 || n > 5) return 0;
    if (nom[0] < 'A' || nom[0] > 'Z') return 0;
    for (i = 1; i < n; i++) {
        c = nom[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) return 0;
    }
    return 1;
}

/* --- identifications (IMP-0254) ------------------------------------------------ */

static Identification courante = { 0, 0, 0 };
static int            secret = 0;      /* programme courant « exécution seule » */

static void lire_ident(const unsigned char *e, Identification *id)
{
    id->numero = (unsigned int) e[1] | ((unsigned int) e[2] << 8);
    id->attributs = e[3];
    id->secteurs = (unsigned int) e[4] | ((unsigned int) e[5] << 8);
}

/* Lit l'emplacement k du fichier IDENTIFICATIONS ; 1 s'il est occupé. */
static int emplacement(FichierPlateforme f, unsigned int k, Identification *id)
{
    unsigned char e[IDENT_TAILLE];
    if ((unsigned long) (k + 1) * IDENT_TAILLE > plateforme_fichier_taille(f)) return 0;
    if (plateforme_fichier_lire(f, (unsigned long) k * IDENT_TAILLE, e, IDENT_TAILLE) != PLAT_OK) return 0;
    if (e[0] == 0) return 0;
    lire_ident(e, id);
    return 1;
}

static void identification_un(Identification *id)
{
    id->numero = 1;
    id->attributs = ATTR_P | ATTR_D | ATTR_S | ATTR_ETENDU;
    id->secteurs = 0;
}

int bibliotheque_identification_rang(unsigned int rang, Identification *id)
{
    FichierPlateforme f;
    unsigned int k;
    unsigned int vu = 0;
    int trouve = 0;

    if (rang == 0) {
        identification_un(id);
        return 1;
    }
    if (plateforme_fichier_ouvrir(ESPACE_RACINE, IDENT_NOM, FICHIER_LECTURE, &f) != PLAT_OK) return 0;
    for (k = 0; k < IDENTIFICATIONS_MAX && !trouve; k++)
        if (emplacement(f, k, id)) {
            vu++;
            if (vu == rang) trouve = 1;
        }
    plateforme_fichier_fermer(f);
    return trouve;
}

int bibliotheque_identification_chercher(unsigned int numero, Identification *id)
{
    unsigned int r;

    if (numero == 0) {
        id->numero = 0;
        id->attributs = 0;
        id->secteurs = 0;
        return 1;
    }
    for (r = 0; bibliotheque_identification_rang(r, id); r++)
        if (id->numero == numero) return 1;
    return 0;
}

/* Rang de l'emplacement de numero, ou du premier libre si numero est nul ;
   -1 si aucun. */
static int chercher_emplacement(FichierPlateforme f, unsigned int numero)
{
    Identification id;
    unsigned int k;
    for (k = 0; k < IDENTIFICATIONS_MAX; k++) {
        if (emplacement(f, k, &id)) {
            if (numero != 0 && id.numero == numero) return (int) k;
        } else if (numero == 0) {
            return (int) k;
        }
    }
    return -1;
}

NatureErreur bibliotheque_identification_creer(const Identification *id)
{
    FichierPlateforme f;
    unsigned char e[IDENT_TAILLE];
    int k;
    int r;

    r = plateforme_fichier_ouvrir(ESPACE_RACINE, IDENT_NOM, FICHIER_MISE_A_JOUR, &f);
    if (r == PLAT_INEXISTANT) r = plateforme_fichier_ouvrir(ESPACE_RACINE, IDENT_NOM, FICHIER_CREATION, &f);
    if (r != PLAT_OK) return erreur_plateforme(r);
    k = chercher_emplacement(f, id->numero);
    if (k < 0) k = chercher_emplacement(f, 0);
    if (k < 0) {
        plateforme_fichier_fermer(f);
        return ERR_F_CATALOGUE_SATURE;
    }
    e[0] = 1;
    e[1] = (unsigned char) (id->numero & 0xFF);
    e[2] = (unsigned char) (id->numero >> 8);
    e[3] = id->attributs;
    e[4] = (unsigned char) (id->secteurs & 0xFF);
    e[5] = (unsigned char) (id->secteurs >> 8);
    r = plateforme_fichier_ecrire(f, (unsigned long) k * IDENT_TAILLE, e, IDENT_TAILLE);
    plateforme_fichier_fermer(f);
    return r == PLAT_OK ? ERR_AUCUNE : erreur_plateforme(r);
}

NatureErreur bibliotheque_identification_supprimer(unsigned int numero)
{
    FichierPlateforme f;
    unsigned char e[IDENT_TAILLE];
    int k;
    int r;

    if (numero == 0) return ERR_F_IDENTIFICATION_INEXISTANTE;
    if ((r = plateforme_fichier_ouvrir(ESPACE_RACINE, IDENT_NOM, FICHIER_MISE_A_JOUR, &f)) != PLAT_OK)
        return r == PLAT_INEXISTANT ? ERR_F_IDENTIFICATION_INEXISTANTE : erreur_plateforme(r);
    k = chercher_emplacement(f, numero);
    if (k >= 0) {
        memset(e, 0, sizeof e);
        plateforme_fichier_ecrire(f, (unsigned long) k * IDENT_TAILLE, e, IDENT_TAILLE);
    }
    plateforme_fichier_fermer(f);
    return k >= 0 ? ERR_AUCUNE : ERR_F_IDENTIFICATION_INEXISTANTE;
}

void bibliotheque_identifier(const Identification *id)
{
    courante = *id;
}

const Identification *bibliotheque_identification(void)
{
    return &courante;
}

unsigned char bibliotheque_compte(void)
{
    return (unsigned char) (courante.numero % 100);
}

int bibliotheque_programme_secret(void)
{
    return secret;
}

void bibliotheque_programme_libre(void)
{
    secret = 0;
}

/* --- catalogue (IMP-0248) --------------------------------------------------- */

/* Rang de l'entrée (catégorie, nom), -1 si absente ; *libre (si libre n'est
   pas NULL) reçoit le rang de la première entrée libre qui la précède, ou de
   la première du catalogue quand elle est absente, -1 s'il n'y en a pas ;
   *taille reçoit le nombre d'entrées. Lecture par blocs de 16 entrées, en
   une seule passe (audit du code, AP-14). */
static long catalogue_chercher(FichierPlateforme f, char categorie, const char *nom, long *libre,
                               unsigned long *taille)
{
    unsigned char t[16 * CATALOGUE_TAILLE];
    const unsigned char *e;
    unsigned long n = plateforme_fichier_taille(f) / CATALOGUE_TAILLE;
    unsigned long i;
    unsigned int k;
    unsigned int bloc;
    unsigned int lg = (unsigned int) strlen(nom);

    *taille = n;
    if (libre != NULL) *libre = -1;
    for (i = 0; i < n; i = i + bloc) {
        bloc = n - i > 16ul ? 16u : (unsigned int) (n - i);
        if (plateforme_fichier_lire(f, i * CATALOGUE_TAILLE, t, bloc * CATALOGUE_TAILLE) != PLAT_OK)
            return -1;
        for (k = 0; k < bloc; k++) {
            e = t + k * CATALOGUE_TAILLE;
            if (libre != NULL && *libre < 0 && e[0] == 0) *libre = (long) (i + k);
            if (e[0] == (unsigned char) categorie && e[1] == lg && memcmp(e + 2, nom, lg) == 0)
                return (long) (i + k);
        }
    }
    return -1;
}

/* Racine du catalogue d'un lieu : la bibliothèque, ou l'unité (IMP-0278). */
static EspaceFichier racine_de(int unite)
{
    return unite ? ESPACE_UNITE_RACINE : ESPACE_RACINE;
}

/* Créé seulement s'il est absent : un catalogue présent mais refusé n'est pas
   vidé (audit du code, AC-29). */
static int ouvrir_catalogue(EspaceFichier racine, FichierPlateforme *f)
{
    int r = plateforme_fichier_ouvrir(racine, CATALOGUE_NOM, FICHIER_MISE_A_JOUR, f);
    if (r != PLAT_INEXISTANT) return r;
    return plateforme_fichier_ouvrir(racine, CATALOGUE_NOM, FICHIER_CREATION, f);
}

/* Inscrit l'entrée dans le catalogue de la bibliothèque ou de l'unité ;
   date nulle : la date du jour. */
static NatureErreur inscrire(EspaceFichier racine, char categorie, const char *nom,
                             unsigned char compte, unsigned char protection,
                             const unsigned char *date)
{
    FichierPlateforme f;
    unsigned char e[CATALOGUE_TAILLE];
    InstantHorloge h;
    unsigned long taille;
    long rang;
    long libre;
    int r;

    if ((r = ouvrir_catalogue(racine, &f)) != PLAT_OK) return erreur_plateforme(r);
    memset(e, 0, sizeof e);
    e[0] = (unsigned char) categorie;
    e[1] = (unsigned char) strlen(nom);
    memcpy(e + 2, nom, e[1]);
    e[7] = compte;
    if (date != 0) {
        e[8] = date[0];
        e[9] = date[1];
        e[10] = date[2];
    } else {
        horloge_lire(&h);
        e[8] = h.jour;
        e[9] = h.mois;
        e[10] = (unsigned char) (h.annee % 100);
    }
    e[11] = protection;
    rang = catalogue_chercher(f, categorie, nom, &libre, &taille);
    if (rang < 0) rang = libre;
    if (rang < 0) rang = (long) taille;
    r = plateforme_fichier_ecrire(f, (unsigned long) rang * CATALOGUE_TAILLE, e, CATALOGUE_TAILLE);
    plateforme_fichier_fermer(f);
    return r == PLAT_OK ? ERR_AUCUNE : erreur_plateforme(r);
}

NatureErreur bibliotheque_cataloguer(int unite, char categorie, const char *nom, unsigned char compte,
                                     unsigned char protection)
{
    return inscrire(racine_de(unite), categorie, nom, compte, protection, 0);
}

void bibliotheque_decataloguer(int unite, char categorie, const char *nom)
{
    FichierPlateforme f;
    unsigned char e[CATALOGUE_TAILLE];
    unsigned long taille;
    long rang;

    if (plateforme_fichier_ouvrir(racine_de(unite), CATALOGUE_NOM, FICHIER_MISE_A_JOUR, &f) != PLAT_OK)
        return;
    rang = catalogue_chercher(f, categorie, nom, NULL, &taille);
    if (rang >= 0) {
        memset(e, 0, sizeof e);                   /* entrée libre */
        plateforme_fichier_ecrire(f, (unsigned long) rang * CATALOGUE_TAILLE, e, CATALOGUE_TAILLE);
    }
    plateforme_fichier_fermer(f);
}

static void entree_lue(const unsigned char *e, char *categorie, char *nom, unsigned char *compte,
                       unsigned char *protection, unsigned char date[3])
{
    if (categorie != 0) *categorie = (char) e[0];
    if (nom != 0) {
        memcpy(nom, e + 2, e[1] <= 5 ? e[1] : 5);
        nom[e[1] <= 5 ? e[1] : 5] = '\0';
    }
    if (compte != 0) *compte = e[7];
    if (protection != 0) *protection = e[11];
    if (date != 0) {
        date[0] = e[8];
        date[1] = e[9];
        date[2] = e[10];
    }
}

static int lire_dans(EspaceFichier racine, char categorie, const char *nom, unsigned char *compte,
                     unsigned char *protection, unsigned char date[3])
{
    FichierPlateforme f;
    unsigned char e[CATALOGUE_TAILLE];
    unsigned long taille;
    long rang;

    if (plateforme_fichier_ouvrir(racine, CATALOGUE_NOM, FICHIER_LECTURE, &f) != PLAT_OK) return 0;
    rang = catalogue_chercher(f, categorie, nom, NULL, &taille);
    if (rang >= 0
        && plateforme_fichier_lire(f, (unsigned long) rang * CATALOGUE_TAILLE, e, CATALOGUE_TAILLE) != PLAT_OK)
        rang = -1;
    plateforme_fichier_fermer(f);
    if (rang < 0) return 0;
    entree_lue(e, 0, 0, compte, protection, date);
    return 1;
}

int bibliotheque_catalogue_lire(char categorie, const char *nom, unsigned char *compte,
                                unsigned char *protection, unsigned char date[3])
{
    return lire_dans(ESPACE_RACINE, categorie, nom, compte, protection, date);
}

int bibliotheque_catalogue_suivant(int unite, char categorie, const char *apres, int strict,
                                   int compte, char *nom, unsigned char *compte_lu,
                                   unsigned char *protection, unsigned char date[3])
{
    FichierPlateforme f;
    unsigned char t[16 * CATALOGUE_TAILLE];
    unsigned char meilleur[CATALOGUE_TAILLE];
    char candidat[6];
    unsigned long n;
    unsigned long i;
    unsigned int k;
    unsigned int bloc;
    unsigned int lg;
    int trouve = 0;
    int c;

    if (plateforme_fichier_ouvrir(racine_de(unite), CATALOGUE_NOM, FICHIER_LECTURE, &f) != PLAT_OK) return 0;
    n = plateforme_fichier_taille(f) / CATALOGUE_TAILLE;
    for (i = 0; i < n; i = i + bloc) {               /* lecture par 16 entrées */
        bloc = n - i > 16ul ? 16u : (unsigned int) (n - i);
        if (plateforme_fichier_lire(f, i * CATALOGUE_TAILLE, t, bloc * CATALOGUE_TAILLE) != PLAT_OK) break;
        for (k = 0; k < bloc; k++) {
            if (t[k * CATALOGUE_TAILLE] != (unsigned char) categorie) continue;
            if (compte >= 0 && t[k * CATALOGUE_TAILLE + 7] != (unsigned char) compte) continue;
            lg = t[k * CATALOGUE_TAILLE + 1] <= 5 ? t[k * CATALOGUE_TAILLE + 1] : 5;
            memcpy(candidat, t + k * CATALOGUE_TAILLE + 2, lg);
            candidat[lg] = '\0';
            c = strcmp(candidat, apres);
            if (c < 0 || (strict && c == 0)) continue;
            if (trouve && strcmp(candidat, nom) >= 0) continue;
            strcpy(nom, candidat);
            memcpy(meilleur, t + k * CATALOGUE_TAILLE, CATALOGUE_TAILLE);
            trouve = 1;
        }
    }
    plateforme_fichier_fermer(f);
    if (trouve) entree_lue(meilleur, 0, 0, compte_lu, protection, date);
    return trouve;
}

int bibliotheque_catalogue_rang(unsigned int rang, int *libre, char *categorie, char *nom,
                                unsigned char *compte, unsigned char *protection,
                                unsigned char date[3])
{
    FichierPlateforme f;
    unsigned char e[CATALOGUE_TAILLE];
    int r;

    if (plateforme_fichier_ouvrir(ESPACE_RACINE, CATALOGUE_NOM, FICHIER_LECTURE, &f) != PLAT_OK) return 0;
    r = (unsigned long) (rang + 1) * CATALOGUE_TAILLE <= plateforme_fichier_taille(f)
        && plateforme_fichier_lire(f, (unsigned long) rang * CATALOGUE_TAILLE, e, CATALOGUE_TAILLE) == PLAT_OK;
    plateforme_fichier_fermer(f);
    if (!r) return 0;
    *libre = e[0] == 0;
    if (!*libre) entree_lue(e, categorie, nom, compte, protection, date);
    return 1;
}

/* --- droits (IMP-0255) ------------------------------------------------------------ */

/* Création, modification ou suppression d'un fichier permanent : l'attribut,
   puis la propriété d'un fichier existant (INRDP75, 5e partie, §2-2). */
static NatureErreur droit(int unite, char categorie, const char *nom, unsigned char attribut,
                          NatureErreur refus)
{
    unsigned char compte;

    if ((courante.attributs & attribut) == 0) return refus;
    if (lire_dans(racine_de(unite), categorie, nom, &compte, 0, 0) && compte != bibliotheque_compte())
        return ERR_F_MAUVAISE_IDENTIFICATION;
    return ERR_AUCUNE;
}

/* Protection d'un fichier créé : L sous une identification « ! » (MEMENTO82). */
static unsigned char protection_creee(unsigned char demandee)
{
    return (courante.attributs & ATTR_PROTEGE) != 0 ? PROTECTION_L : demandee;
}

NatureErreur bibliotheque_droit_donnees(const char *nom, int unite, unsigned char *protection)
{
    *protection = protection_creee(0);
    return droit(unite, CATALOGUE_DONNEES, nom, ATTR_D, ERR_F_DROIT_DONNEES);
}

int bibliotheque_lecture_interdite(char categorie, const char *nom, int unite)
{
    unsigned char compte;
    unsigned char protection;

    return lire_dans(racine_de(unite), categorie, nom, &compte, &protection, 0)
           && protection == PROTECTION_L && compte != bibliotheque_compte();
}

NatureErreur bibliotheque_supprimer(EspaceFichier e, const char *nom)
{
    int unite = e == ESPACE_UNITE_PROGRAMMES || e == ESPACE_UNITE_DONNEES;
    int programme = e == ESPACE_PROGRAMMES || e == ESPACE_UNITE_PROGRAMMES;
    int donnees = e == ESPACE_DONNEES || e == ESPACE_UNITE_DONNEES;
    NatureErreur n = ERR_AUCUNE;
    int r;

    if (unite && !plateforme_unite_disponible()) return ERR_F_UNITE;
    if (!plateforme_fichier_existe(e, nom)) return ERR_F_FICHIER_INEXISTANT;
    if (programme) n = droit(unite, CATALOGUE_PROGRAMME, nom, ATTR_P, ERR_F_DROIT_PROGRAMME);
    if (donnees) n = droit(unite, CATALOGUE_DONNEES, nom, ATTR_D, ERR_F_DROIT_DONNEES);
    if (n != ERR_AUCUNE) return n;
    if ((r = plateforme_fichier_supprimer(e, nom)) != PLAT_OK) return erreur_plateforme(r);
    if (programme) bibliotheque_decataloguer(unite, CATALOGUE_PROGRAMME, nom);
    if (donnees) bibliotheque_decataloguer(unite, CATALOGUE_DONNEES, nom);
    return ERR_AUCUNE;
}

/* --- DUPLIQUER (IMP-0258) --------------------------------------------------------- */

static NatureErreur copier(EspaceFichier source, EspaceFichier cible, const char *nom)
{
    FichierPlateforme fs;
    FichierPlateforme fc;
    unsigned char *t;
    unsigned long n;
    unsigned long p = 0;
    unsigned int bloc;
    int r;

    if ((r = plateforme_fichier_ouvrir(source, nom, FICHIER_LECTURE, &fs)) != PLAT_OK)
        return erreur_plateforme(r);
    if ((t = (unsigned char *) malloc(4096)) == NULL) {
        plateforme_fichier_fermer(fs);
        return ERR_F_MEMOIRE_PROGRAMME;
    }
    if ((r = plateforme_fichier_ouvrir(cible, nom, FICHIER_CREATION, &fc)) == PLAT_OK) {
        n = plateforme_fichier_taille(fs);
        while (p < n && r == PLAT_OK) {
            bloc = n - p > 4096ul ? 4096u : (unsigned int) (n - p);
            r = plateforme_fichier_lire(fs, p, t, bloc);
            if (r == PLAT_OK) r = plateforme_fichier_ecrire(fc, p, t, bloc);
            p = p + bloc;
        }
        plateforme_fichier_fermer(fc);
        if (r != PLAT_OK) plateforme_fichier_supprimer(cible, nom);
    }
    free(t);
    plateforme_fichier_fermer(fs);
    return r == PLAT_OK ? ERR_AUCUNE : erreur_plateforme(r);
}

NatureErreur bibliotheque_dupliquer(int depuis_unite, int programme, const char *nom)
{
    char categorie = programme ? CATALOGUE_PROGRAMME : CATALOGUE_DONNEES;
    EspaceFichier unite = programme ? ESPACE_UNITE_PROGRAMMES : ESPACE_UNITE_DONNEES;
    EspaceFichier bib = programme ? ESPACE_PROGRAMMES : ESPACE_DONNEES;
    unsigned char compte = 0;
    unsigned char protection = 0;
    unsigned char date[3];
    int catalogue;
    NatureErreur e;

    if (!plateforme_unite_disponible()) return ERR_F_UNITE;
    if (depuis_unite) {                              /* unité -> bibliothèque */
        e = programme ? droit(0, CATALOGUE_PROGRAMME, nom, ATTR_P, ERR_F_DROIT_PROGRAMME)
                      : droit(0, CATALOGUE_DONNEES, nom, ATTR_D, ERR_F_DROIT_DONNEES);
        if (e != ERR_AUCUNE) return e;
        if (!plateforme_fichier_existe(unite, nom)) return ERR_F_FICHIER_INEXISTANT;
        if ((e = bibliotheque_nom_pris(0, programme, nom)) != ERR_AUCUNE) return e;
        lire_dans(ESPACE_UNITE_RACINE, categorie, nom, &compte, &protection, date);
        if ((e = copier(unite, bib, nom)) != ERR_AUCUNE) return e;
        /* le créateur est le compte courant ; la protection est gardée (D4) */
        return inscrire(ESPACE_RACINE, categorie, nom, bibliotheque_compte(),
                        protection_creee(protection), 0);
    }
    /* bibliothèque -> unité : rien n'est créé dans la bibliothèque (D3) */
    if (!plateforme_fichier_existe(bib, nom)) return ERR_F_FICHIER_INEXISTANT;
    catalogue = bibliotheque_catalogue_lire(categorie, nom, &compte, &protection, date);
    if (catalogue && compte != bibliotheque_compte()
        && (protection == PROTECTION_L
            || (protection == PROTECTION_E && (courante.attributs & ATTR_PRIVILEGIE) == 0)))
        return ERR_F_PROGRAMME_PROTEGE;
    if ((e = bibliotheque_nom_pris(1, programme, nom)) != ERR_AUCUNE) return e;
    if ((e = copier(bib, unite, nom)) != ERR_AUCUNE) return e;
    if (!catalogue) return ERR_AUCUNE;
    return inscrire(ESPACE_UNITE_RACINE, categorie, nom, compte, protection, date);
}

/* --- écriture RCI-S (IMP-0246) ---------------------------------------------- */

static void poser_u16(unsigned char *t, unsigned long *p, unsigned int v)
{
    t[*p] = (unsigned char) (v & 0xFF);
    t[*p + 1] = (unsigned char) (v >> 8);
    *p = *p + 2;
}

static unsigned int lire_u16(const unsigned char *t, unsigned long p)
{
    return (unsigned int) t[p] | ((unsigned int) t[p + 1] << 8);
}

/* Construit l'image RCI-S du programme courant ; NULL si la mémoire manque. */
static unsigned char *image_rcis(unsigned long *longueur)
{
    unsigned int ns = symboles_nombre();
    unsigned int nc = constantes_nombre();
    unsigned int nl = programme_nombre();
    unsigned long taille = 8 + 2 + 8ul * ns + 2 + 2 + 2;
    unsigned long p = 0;
    unsigned long somme = 0;
    unsigned long q;
    unsigned int i;
    EntreeSymbole s;
    EntreeConstante k;
    EntreeObjetCompile e;
    unsigned char *t;

    for (i = 0; i < nc; i++)
        if (constantes_lire(i, &k)) taille = taille + 1 + k.longueur;
    for (i = 0; i < nl; i++)
        if (programme_rang(i, &e)) taille = taille + 2 + e.longueur_octets;
    t = (unsigned char *) malloc((size_t) taille);
    if (t == NULL) return NULL;
    memcpy(t, "LSEP", 4);
    t[4] = RCIS_VERSION;
    t[5] = configuration_session.profil;
    t[6] = 0;
    t[7] = 0;
    p = 8;
    poser_u16(t, &p, ns);
    for (i = 0; i < ns; i++) {
        memset(t + p, 0, 8);
        if (symboles_lire(i, &s)) {
            t[p] = s.nom_longueur;
            memcpy(t + p + 1, s.nom, s.nom_longueur);
            t[p + 6] = s.nature;
            t[p + 7] = s.origine;
        }
        p = p + 8;
    }
    poser_u16(t, &p, nc);
    for (i = 0; i < nc; i++) {
        constantes_lire(i, &k);
        t[p] = k.longueur;
        objets_copier(k.texte, 0, t + p + 1, k.longueur);
        p = p + 1 + k.longueur;
    }
    poser_u16(t, &p, nl);
    for (i = 0; i < nl; i++) {
        programme_rang(i, &e);
        t[p] = e.numero_ligne;
        t[p + 1] = (unsigned char) e.longueur_octets;
        objets_copier(e.bloc, 0, t + p + 2, e.longueur_octets);
        p = p + 2 + e.longueur_octets;
    }
    for (q = 0; q < p; q++) somme = somme + t[q];          /* image de plus de 64 Kio possible */
    poser_u16(t, &p, (unsigned int) (somme & 0xFFFFul));
    *longueur = p;
    return t;
}

static NatureErreur ecrire_fichier(EspaceFichier esp, const char *nom, const unsigned char *t,
                                   unsigned long n)
{
    FichierPlateforme f;
    unsigned long p = 0;
    unsigned int bloc;
    int r;

    if ((r = plateforme_fichier_ouvrir(esp, nom, FICHIER_CREATION, &f)) != PLAT_OK)
        return erreur_plateforme(r);
    while (p < n) {
        bloc = n - p > 4096ul ? 4096u : (unsigned int) (n - p);
        if ((r = plateforme_fichier_ecrire(f, p, t + p, bloc)) != PLAT_OK) break;
        p = p + bloc;
    }
    plateforme_fichier_fermer(f);
    if (r != PLAT_OK) {
        plateforme_fichier_supprimer(esp, nom);
        return erreur_plateforme(r);
    }
    return ERR_AUCUNE;
}

NatureErreur bibliotheque_ranger(const char *nom, unsigned char protection, int remplacer, int unite)
{
    EspaceFichier esp = unite ? ESPACE_UNITE_PROGRAMMES : ESPACE_PROGRAMMES;
    unsigned char *t;
    unsigned long n;
    NatureErreur e;

    if (unite && !plateforme_unite_disponible()) return ERR_F_UNITE;
    if (secret) return ERR_F_PROGRAMME_PROTEGE;
    if ((e = droit(unite, CATALOGUE_PROGRAMME, nom, ATTR_P, ERR_F_DROIT_PROGRAMME)) != ERR_AUCUNE) return e;
    if (protection == PROTECTION_E && (courante.attributs & ATTR_S) == 0) return ERR_F_DROIT_SECRET;
    if (remplacer) {
        if (!plateforme_fichier_existe(esp, nom)) return ERR_F_FICHIER_INEXISTANT;
    } else if ((e = bibliotheque_nom_pris(unite, 1, nom)) != ERR_AUCUNE) {
        return e;                                                                            /* B2 */
    }
    if ((t = image_rcis(&n)) == NULL) return ERR_F_MEMOIRE_PROGRAMME;
    if (remplacer) {                        /* MODIFIER : nouveau fichier d'abord (B3) */
        e = ecrire_fichier(esp, RCIS_TRANSIT, t, n);
        free(t);
        if (e != ERR_AUCUNE) return e;
        /* l'original n'est détruit qu'une fois la nouvelle version en place (AC-21) */
        plateforme_fichier_supprimer(esp, RCIS_ANCIEN);
        if (plateforme_fichier_renommer(esp, nom, esp, RCIS_ANCIEN) != PLAT_OK) {
            plateforme_fichier_supprimer(esp, RCIS_TRANSIT);
            return ERR_F_ENTREE_SORTIE;
        }
        if (plateforme_fichier_renommer(esp, RCIS_TRANSIT, esp, nom) != PLAT_OK) {
            plateforme_fichier_renommer(esp, RCIS_ANCIEN, esp, nom);
            plateforme_fichier_supprimer(esp, RCIS_TRANSIT);
            return ERR_F_ENTREE_SORTIE;
        }
        plateforme_fichier_supprimer(esp, RCIS_ANCIEN);
        return ERR_AUCUNE;
    }
    e = ecrire_fichier(esp, nom, t, n);
    free(t);
    if (e != ERR_AUCUNE) return e;
    e = bibliotheque_cataloguer(unite, CATALOGUE_PROGRAMME, nom, bibliotheque_compte(),
                                protection_creee(protection));
    if (e != ERR_AUCUNE) plateforme_fichier_supprimer(esp, nom);   /* jamais sans propriétaire */
    return e;
}

/* --- lecture et restauration RCI-S (IMP-0246) -------------------------------- */

/* Entrée de symbole d'un fichier : nom de 1 à 5 caractères, une lettre puis des
   lettres ou des chiffres (Compilateur IMP-0191), complété de zéros ; nature et
   origine connues. */
static int symbole_valide(const unsigned char *e)
{
    unsigned int lg = e[0];
    unsigned int i;
    unsigned char c;

    if (lg < 1 || lg > SYMBOLES_NOM_MAX) return 0;
    for (i = 0; i < SYMBOLES_NOM_MAX; i++) {
        c = e[1 + i];
        if (i >= lg) {
            if (c != 0) return 0;
        } else if (!(c >= 'A' && c <= 'Z') && (i == 0 || !(c >= '0' && c <= '9'))) {
            return 0;
        }
    }
    return e[6] <= SYM_PROCEDURE && e[7] <= ORIGINE_MACHINE_BUREAU;
}

/* Doublons d'une table du fichier (audit du code, AC-17) : table ouverte
   temporaire, dont chaque position porte le rang + 1 d'une entrée déjà vue
   (0 : vide). Au plus 32 768 positions de 2 octets, soit 64 Kio, la limite
   d'un tableau alloué en modèle small ; une table du fichier compte au plus
   21 845 entrées (IMP-0104). */
#define VUES_MAX 32768ul

typedef struct {
    const unsigned char *t;      /* image du fichier */
    unsigned long  debut;        /* première entrée de la table dans l'image */
    int            symboles;     /* entrées de 8 octets ; sinon constantes */
    unsigned long *reperes;      /* constantes : position des entrées de rang multiple de 8 */
    unsigned int  *cases;
    unsigned int   masque;       /* nombre de positions - 1 */
} Vues;

/* Position, dans l'image, de l'entrée de rang k. */
static unsigned long position_entree(const Vues *v, unsigned int k)
{
    unsigned long p;
    unsigned int i;

    if (v->symboles) return v->debut + 8ul * k;
    p = v->reperes[k >> 3];
    for (i = 0; i < (k & 7u); i++) p = p + 1 + v->t[p];
    return p;
}

/* 1 si l'entrée de rang k, placée en p, égale une entrée déjà vue ; sinon elle
   est notée. Clé : longueur et texte d'une constante ; longueur, nom et nature
   d'un symbole (l'origine n'en fait pas partie, IMP-0167). */
static int deja_vue(Vues *v, unsigned int k, unsigned long p)
{
    unsigned int lg = v->symboles ? 7u : 1u + v->t[p];
    unsigned int h = 0;
    unsigned int i;
    unsigned int j;
    unsigned long q;

    for (i = 0; i < lg; i++) h = (h << 5) + h + v->t[p + i];
    j = h & v->masque;
    while (v->cases[j] != 0) {
        q = position_entree(v, v->cases[j] - 1);
        if (v->t[q] == v->t[p] && memcmp(v->t + q, v->t + p, lg) == 0) return 1;
        j = (j + 1) & v->masque;
    }
    v->cases[j] = k + 1;
    return 0;
}

/* F 101 si deux entrées de la table sont égales ; F 23 si la table temporaire
   ne peut être allouée (le programme courant est encore intact). */
static NatureErreur doublons(const unsigned char *t, unsigned long debut, unsigned int n, int symboles)
{
    Vues v;
    unsigned long m = 16;
    unsigned long p = debut;
    unsigned int k;
    NatureErreur e = ERR_AUCUNE;

    if (n < 2) return ERR_AUCUNE;
    while (m < 2ul * n && m < VUES_MAX) m = m * 2;
    v.t = t;
    v.debut = debut;
    v.symboles = symboles;
    v.masque = (unsigned int) (m - 1);
    v.cases = (unsigned int *) calloc((size_t) m, sizeof(unsigned int));
    v.reperes = NULL;
    if (!symboles) v.reperes = (unsigned long *) malloc((size_t) ((n >> 3) + 1) * sizeof(unsigned long));
    if (v.cases == NULL || (!symboles && v.reperes == NULL)) e = ERR_F_MEMOIRE_PROGRAMME;
    for (k = 0; k < n && e == ERR_AUCUNE; k++) {
        if (!symboles && (k & 7u) == 0) v.reperes[k >> 3] = p;
        if (deja_vue(&v, k, p)) e = ERR_F_FICHIER_ENDOMMAGE;
        p = symboles ? p + 8 : p + 1 + t[p];
    }
    free(v.cases);
    free(v.reperes);
    return e;
}

/* Vérification complète de l'image, sans rien modifier (VM-3403) ; 1 si la
   ligne requise (non nulle) existe. */
static NatureErreur verifier(const unsigned char *t, unsigned long n, unsigned char ligne_requise)
{
    unsigned long p;
    unsigned long somme = 0;
    unsigned long i;
    unsigned long debut_constantes;
    unsigned int ns;
    unsigned int nc;
    unsigned int nl;
    unsigned int k;
    unsigned int lg;
    unsigned int borne = PROFIL_EST_T1600(configuration_session.profil) ? 255u : 250u;
    int precedent = 0;
    int trouvee = ligne_requise == 0;
    NatureErreur e;

    if (n < 16 || memcmp(t, "LSEP", 4) != 0 || t[4] != RCIS_VERSION) return ERR_F_FICHIER_ENDOMMAGE;
    for (i = 0; i < n - 2; i++) somme = somme + t[i];
    if ((unsigned int) (somme & 0xFFFFul) != lire_u16(t, n - 2)) return ERR_F_FICHIER_ENDOMMAGE;
    if (t[5] != configuration_session.profil) return ERR_F_AUTRE_PROFIL;
    p = 8;
    ns = lire_u16(t, p);
    if (ns > SYMBOLES_CAPACITE_MAX) return ERR_F_FICHIER_ENDOMMAGE;          /* IMP-0104 */
    p = p + 2 + 8ul * ns;
    if (p + 2 > n - 2) return ERR_F_FICHIER_ENDOMMAGE;
    for (k = 0; k < ns; k++)
        if (!symbole_valide(t + 10 + 8ul * k)) return ERR_F_FICHIER_ENDOMMAGE;
    nc = lire_u16(t, p);
    if (nc > CONSTANTES_CAPACITE_MAX) return ERR_F_FICHIER_ENDOMMAGE;
    p = p + 2;
    debut_constantes = p;
    for (k = 0; k < nc; k++) {
        if (p >= n - 2) return ERR_F_FICHIER_ENDOMMAGE;
        lg = t[p];
        if (lg <= CONSTANTES_SEUIL_PARTAGE || lg > RCIM_CHAINE_LONGUEUR_MAX) return ERR_F_FICHIER_ENDOMMAGE;
        p = p + 1 + lg;
    }
    if (p + 2 > n - 2) return ERR_F_FICHIER_ENDOMMAGE;
    nl = lire_u16(t, p);
    p = p + 2;
    for (k = 0; k < nl; k++) {
        if (p + 2 > n - 2) return ERR_F_FICHIER_ENDOMMAGE;
        if ((int) t[p] <= precedent || t[p] > borne) return ERR_F_FICHIER_ENDOMMAGE;
        precedent = t[p];
        if (t[p] == ligne_requise) trouvee = 1;
        lg = t[p + 1];
        if (p + 2 + lg > n - 2) return ERR_F_FICHIER_ENDOMMAGE;   /* lg = 0 : ligne vide (IMP-0194) */
        if (!rcim_sequence_conforme(t + p + 2, lg, ns, nc)) return ERR_F_FICHIER_ENDOMMAGE;
        p = p + 2 + lg;
    }
    if (p != n - 2) return ERR_F_FICHIER_ENDOMMAGE;
    if ((e = doublons(t, 10, ns, 1)) != ERR_AUCUNE) return e;
    if ((e = doublons(t, debut_constantes, nc, 0)) != ERR_AUCUNE) return e;
    return trouvee ? ERR_AUCUNE : ERR_E_LIGNE_INEXISTANTE;
}

/* Reconstruction après vérification, les tables étant ajoutées sans recherche
   (audit du code, AP-12) ; en cas d'échec, programme vide. */
static NatureErreur reconstruire(const unsigned char *t)
{
    unsigned long p = 8;
    unsigned int ns = lire_u16(t, p);
    unsigned int nc;
    unsigned int nl;
    unsigned int k;
    unsigned int id;
    unsigned char *o;
    EntreeObjetCompile e;
    NatureErreur n;

    contexte_vider();
    programme_vider();
    p = p + 2;
    for (k = 0; k < ns; k++) {
        if (symboles_ajouter((const char *) t + p + 1, t[p], t[p + 6], t[p + 7], &id)
            != SYMB_OK || id != k)
            goto echec;
        p = p + 8;
    }
    nc = lire_u16(t, p);
    p = p + 2;
    for (k = 0; k < nc; k++) {
        if (constantes_ajouter(t + p + 1, t[p], &id) != SYMB_OK || id != k) goto echec;
        p = p + 1 + t[p];
    }
    nl = lire_u16(t, p);
    p = p + 2;
    for (k = 0; k < nl; k++) {
        e.numero_ligne = t[p];
        e.longueur_octets = t[p + 1];
        e.bloc = objets_creer(e.longueur_octets);
        if (e.bloc == GESTOBJ_HANDLE_NUL) goto echec;
        o = objets_resoudre(e.bloc);
        memcpy(o, t + p + 2, e.longueur_octets);
        objets_relacher(e.bloc);
        if (!programme_inserer(&e, &n)) {
            objets_liberer(e.bloc);
            goto echec;
        }
        p = p + 2 + e.longueur_octets;
    }
    return ERR_AUCUNE;
echec:
    programme_vider();
    return ERR_F_MEMOIRE_PROGRAMME;
}

NatureErreur bibliotheque_appeler(const char *nom, unsigned char ligne_requise, int unite)
{
    FichierPlateforme f;
    unsigned long n;
    unsigned long p = 0;
    unsigned int bloc;
    unsigned char *t;
    NatureErreur e;
    int r;
    unsigned char compte = 0;
    unsigned char protection = 0;
    if (!plateforme_bibliotheque_disponible()) return ERR_F_BIBLIOTHEQUE;   /* IMP-0250 */
    if (unite && !plateforme_unite_disponible()) return ERR_F_UNITE;

    if ((r = plateforme_fichier_ouvrir(unite ? ESPACE_UNITE_PROGRAMMES : ESPACE_PROGRAMMES, nom,
                                       FICHIER_LECTURE, &f)) != PLAT_OK)
        return erreur_plateforme(r);
    lire_dans(racine_de(unite), CATALOGUE_PROGRAMME, nom, &compte, &protection, 0);
    if (protection == PROTECTION_L && compte != bibliotheque_compte()) {   /* IMP-0255 */
        plateforme_fichier_fermer(f);
        return ERR_F_PROGRAMME_PROTEGE;
    }
    n = plateforme_fichier_taille(f);
    t = (unsigned char *) malloc((size_t) (n > 0 ? n : 1));
    if (t == NULL) {
        plateforme_fichier_fermer(f);
        return ERR_F_MEMOIRE_PROGRAMME;
    }
    r = PLAT_OK;
    while (p < n && r == PLAT_OK) {
        bloc = n - p > 4096ul ? 4096u : (unsigned int) (n - p);
        r = plateforme_fichier_lire(f, p, t + p, bloc);
        p = p + bloc;
    }
    plateforme_fichier_fermer(f);
    if (r != PLAT_OK) e = erreur_plateforme(r);
    else if ((e = verifier(t, n, ligne_requise)) == ERR_AUCUNE) e = reconstruire(t);
    free(t);
    if (e == ERR_AUCUNE)
        secret = protection == PROTECTION_E && compte != bibliotheque_compte()
                 && (courante.attributs & ATTR_PRIVILEGIE) == 0;
    else if (e == ERR_F_MEMOIRE_PROGRAMME)
        secret = 0;                                  /* programme courant vidé */
    return e;
}
