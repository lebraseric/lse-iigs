/* fichiers_iigs.c — bibliothèque et fichiers sur l'Apple IIGS : stdio
   d'ORCA/C pour les fichiers, GS/OS pour les répertoires
   EXI: EXI-0009, EXI-0053 à EXI-0057
   VM: VM-3401, VM-3404, VM-3405
   IMP: IMP-0136 (adaptation isolée dans src/plateforme/), IMP-0245
        (bibliothèque, -BIB, @:LSE), IMP-0249 (interface), IMP-0257 (secteurs
        libres), IMP-0258 (unité amovible), IMP-0262 (ruban)
   Tests: tests/plateforme/sonde_fichiers.c, tests/plateforme/plateforme_fichiers.c
   Chemins GS/OS : préfixe de bibliothèque, sous-répertoire, nom, séparés par
   « : ». Segment "fichiers" (Construction §1). */

#pragma noroot

#include <stdio.h>
#include <string.h>
#include <gsos.h>
#include <orca.h>
#include "fichiers_iigs.h"
#include "../interfaces/profil.h"

segment "fichiers";

#define LONGUEUR_CHEMIN 200
/* Au-delà du préfixe ou de l'unité : « : », TEMPORAIRES, « : », un nom de
   15 caractères au plus (plateforme_fichier_lister) et le zéro final. */
#define MARGE_CHEMIN 29

static char prefixe[LONGUEUR_CHEMIN] = "@:LSE";
static char unite[LONGUEUR_CHEMIN] = "";
static char ruban[LONGUEUR_CHEMIN] = "";     /* vide : RUBAN dans la bibliothèque */
static const char *const sous_repertoire[8] = { "", "PROGRAMMES", "DONNEES", "TEMPORAIRES",
                                                "PROGRAMMES", "DONNEES", "", "" };
static GSString255 chemin_gs;
static int disponible = 1;                    /* IMP-0250 : bibliothèque ouverte au lancement */

/* Chemin de l'espace e (nom NULL) ou d'un fichier de cet espace. */
static void composer(char *sortie, EspaceFichier e, const char *nom)
{
    if (e == ESPACE_RUBAN) {                          /* IMP-0262 */
        if (ruban[0] != '\0') {
            strcpy(sortie, ruban);
        } else {
            strcpy(sortie, prefixe);
            strcat(sortie, ":RUBAN");
        }
        return;
    }
    strcpy(sortie, e >= ESPACE_UNITE_PROGRAMMES ? unite : prefixe);
    if (sous_repertoire[e][0] != '\0') {
        strcat(sortie, ":");
        strcat(sortie, sous_repertoire[e]);
    }
    if (nom != NULL) {
        strcat(sortie, ":");
        strcat(sortie, nom);
    }
}

static GSString255 *gs(const char *c)
{
    chemin_gs.length = (Word) strlen(c);
    memcpy(chemin_gs.text, c, chemin_gs.length);
    return &chemin_gs;
}

static void creer_repertoire(const char *c)
{
    CreateRecGS cr;
    cr.pCount = 5;
    cr.pathname = gs(c);
    cr.access = 0xC3;
    cr.fileType = 0x0F;
    cr.auxType = 0;
    cr.storageType = 0x0D;
    CreateGS(&cr);                    /* déjà existant ($47) : sans effet */
}

int plateforme_bibliotheque_disponible(void)
{
    return disponible;
}

int plateforme_bibliotheque_ouvrir(const char *chemin)
{
    char c[LONGUEUR_CHEMIN];
    OpenRecGS op;
    RefNumRecGS rf;
    EspaceFichier e;

    if (chemin != NULL) {
        if (strlen(chemin) + MARGE_CHEMIN > LONGUEUR_CHEMIN) { disponible = 0; return PLAT_ERREUR; }
        strcpy(prefixe, chemin);
    }
    creer_repertoire(prefixe);
    for (e = ESPACE_PROGRAMMES; e <= ESPACE_TEMPORAIRES; e++) {
        composer(c, e, NULL);
        creer_repertoire(c);
    }
    op.pCount = 2;
    op.pathname = gs(prefixe);
    OpenGS(&op);
    if (toolerror() != 0) { disponible = 0; return PLAT_ERREUR; }
    rf.pCount = 1;
    rf.refNum = op.refNum;
    CloseGS(&rf);
    disponible = 1;
    return PLAT_OK;
}

/* Cause d'un échec d'ouverture ou de suppression, demandée à GS/OS : la stdio
   d'ORCA/C pose ENOENT pour un fichier absent comme pour un fichier verrouillé,
   et ne renseigne pas toolerror() (IMP-0249 ; audit du code, AC-29).
   $46 : fichier absent ; $44 : répertoire absent. */
static int cause(const char *c)
{
    FileInfoRecGS fi;
    fi.pCount = 2;
    fi.pathname = gs(c);
    GetFileInfoGS(&fi);
    return toolerror() == 0x46 || toolerror() == 0x44 ? PLAT_INEXISTANT : PLAT_ERREUR;
}

int plateforme_fichier_ouvrir(EspaceFichier e, const char *nom, ModeFichier m, FichierPlateforme *f)
{
    char c[LONGUEUR_CHEMIN];
    FILE *x;

    if (e >= ESPACE_UNITE_PROGRAMMES && e <= ESPACE_UNITE_RACINE && m == FICHIER_CREATION) {   /* IMP-0258 */
        creer_repertoire(unite);
        composer(c, e, NULL);
        creer_repertoire(c);
    }
    composer(c, e, nom);
    x = fopen(c, m == FICHIER_LECTURE ? "rb" : m == FICHIER_MISE_A_JOUR ? "r+b" : "w+b");
    *f = (FichierPlateforme) x;
    if (x != NULL) return PLAT_OK;
    return m == FICHIER_CREATION ? PLAT_ERREUR : cause(c);
}

int plateforme_fichier_lire(FichierPlateforme f, unsigned long position, void *dest, unsigned int n)
{
    if (fseek((FILE *) f, (long) position, SEEK_SET) != 0) return PLAT_ERREUR;
    if (n > 0 && fread(dest, 1, n, (FILE *) f) != n) return PLAT_ERREUR;
    return PLAT_OK;
}

/* Tout échec d'écriture vaut support plein : fwrite et fflush d'ORCA/C posent
   EIO pour le code $48 de GS/OS comme pour les autres (IMP-0249 ; AC-29). */
int plateforme_fichier_ecrire(FichierPlateforme f, unsigned long position, const void *src,
                              unsigned int n)
{
    if (fseek((FILE *) f, (long) position, SEEK_SET) != 0) return PLAT_ERREUR;
    if (n > 0 && fwrite(src, 1, n, (FILE *) f) != n) return PLAT_PLEIN;
    if (fflush((FILE *) f) != 0) return PLAT_PLEIN;
    return PLAT_OK;
}

unsigned long plateforme_fichier_taille(FichierPlateforme f)
{
    if (fseek((FILE *) f, 0L, SEEK_END) != 0) return 0;
    return (unsigned long) ftell((FILE *) f);
}

void plateforme_fichier_fermer(FichierPlateforme f)
{
    if (f != NULL) fclose((FILE *) f);
}

int plateforme_fichier_existe(EspaceFichier e, const char *nom)
{
    FichierPlateforme f;
    if (plateforme_fichier_ouvrir(e, nom, FICHIER_LECTURE, &f) != PLAT_OK) return 0;
    plateforme_fichier_fermer(f);
    return 1;
}

int plateforme_fichier_supprimer(EspaceFichier e, const char *nom)
{
    char c[LONGUEUR_CHEMIN];
    composer(c, e, nom);
    return remove(c) == 0 ? PLAT_OK : cause(c);
}

int plateforme_fichier_renommer(EspaceFichier e1, const char *nom1, EspaceFichier e2,
                                const char *nom2)
{
    char c1[LONGUEUR_CHEMIN];
    char c2[LONGUEUR_CHEMIN];
    composer(c1, e1, nom1);
    composer(c2, e2, nom2);
    return rename(c1, c2) == 0 ? PLAT_OK : PLAT_ERREUR;
}

int plateforme_fichier_lister(EspaceFichier e, unsigned int rang, char *nom)
{
    char c[LONGUEUR_CHEMIN];
    OpenRecGS op;
    RefNumRecGS rf;
    DirEntryRecGS de;
    ResultBuf255 resultat;
    unsigned int n;
    int trouve;

    composer(c, e, NULL);
    op.pCount = 3;
    op.pathname = gs(c);
    op.requestAccess = 1;
    OpenGS(&op);
    if (toolerror() != 0) return 0;
    resultat.bufSize = sizeof resultat;
    de.pCount = 6;
    de.refNum = op.refNum;
    de.flags = 0;
    de.base = 0;                        /* rang absolu, à partir de 1 */
    de.displacement = rang + 1;
    de.name = &resultat;
    GetDirEntryGS(&de);
    trouve = toolerror() == 0;
    if (trouve) {
        n = resultat.bufString.length;
        if (n > 15) n = 15;
        memcpy(nom, resultat.bufString.text, n);
        nom[n] = '\0';
    }
    rf.pCount = 1;
    rf.refNum = op.refNum;
    CloseGS(&rf);
    return trouve;
}

int plateforme_espace_parcourir(EspaceFichier e, void (*f)(const char *nom, void *contexte),
                                void *contexte)
{
    char c[LONGUEUR_CHEMIN];
    char nom[16];
    OpenRecGS op;
    RefNumRecGS rf;
    DirEntryRecGS de;
    ResultBuf255 resultat;
    unsigned int n;
    unsigned int rang;

    composer(c, e, NULL);
    op.pCount = 3;
    op.pathname = gs(c);
    op.requestAccess = 1;
    OpenGS(&op);
    if (toolerror() != 0) return 0;
    resultat.bufSize = sizeof resultat;
    for (rang = 1; ; rang++) {
        de.pCount = 6;
        de.refNum = op.refNum;
        de.flags = 0;
        de.base = 0;                        /* rang absolu, à partir de 1 */
        de.displacement = rang;
        de.name = &resultat;
        GetDirEntryGS(&de);
        if (toolerror() != 0) break;        /* fin du répertoire */
        n = resultat.bufString.length;
        if (n > 15) n = 15;
        memcpy(nom, resultat.bufString.text, n);
        nom[n] = '\0';
        f(nom, contexte);
    }
    rf.pCount = 1;
    rf.refNum = op.refNum;
    CloseGS(&rf);
    return 1;
}

void plateforme_unite_designer(const char *chemin)
{
    if (chemin == NULL || strlen(chemin) + MARGE_CHEMIN > LONGUEUR_CHEMIN) unite[0] = '\0';
    else strcpy(unite, chemin);
}

void plateforme_ruban_designer(const char *chemin)
{
    if (chemin == NULL || strlen(chemin) >= LONGUEUR_CHEMIN) ruban[0] = '\0';
    else strcpy(ruban, chemin);
}

int plateforme_fichier_creer_texte(EspaceFichier e, const char *nom)
{
    char c[LONGUEUR_CHEMIN];
    CreateRecGS cr;

    composer(c, e, nom);
    cr.pCount = 5;
    cr.pathname = gs(c);
    cr.access = 0xC3;
    cr.fileType = 0x04;                  /* TXT : lisible par l'éditeur d'ORCA */
    cr.auxType = 0;
    cr.storageType = 0x01;
    CreateGS(&cr);
    return toolerror() == 0 || toolerror() == 0x47 ? PLAT_OK : PLAT_ERREUR;   /* $47 : existe */
}

int plateforme_unite_disponible(void)
{
    return unite[0] != '\0';
}

static int volume_permis = 0;

/* Golden Gate n'implémente pas Volume ($2008) et s'arrête à son appel : le
   programme principal ne le permet que sous GS/OS (IMP-0257). */
void plateforme_volume_permettre(int vrai)
{
    volume_permis = vrai != 0;
}

/* Noms de volume GS/OS égaux, sans égard à la casse (ExpandPath rend le nom
   en capitales, Volume tel qu'inscrit sur le disque). */
static int meme_volume(const GSString255 *a, const ResultBuf255 *b)
{
    unsigned int i;
    char x;
    char y;

    if (a->length != b->bufString.length) return 0;
    for (i = 0; i < a->length; i++) {
        x = a->text[i];
        y = b->bufString.text[i];
        if (x >= 'a' && x <= 'z') x = (char) (x - 32);
        if (y >= 'a' && y <= 'z') y = (char) (y - 32);
        if (x != y) return 0;
    }
    return 1;
}

/* Secteurs libres du volume par l'appel Volume, qui demande un nom de
   périphérique (un nom de volume donne l'erreur $0040, constaté sur l'Apple
   IIGS par « SONDEF V » le 2026-10-25). GetDevNumber rend le numéro du
   périphérique qui porte le volume « :VOL », sans accès aux autres lecteurs
   (« SONDEF G » sous GSSquared, 2026-11-06 : « VOL » sans « : » donne $0040) ;
   DInfo en donne le nom. Un parcours des périphériques appelait Volume sur
   chacun et faisait tourner les lecteurs 5,25" (IMP-0257). */
unsigned long plateforme_secteurs_libres(void)
{
    ExpandPathRecGS ex;
    DevNumRecGS dn;
    DInfoRecGS di;
    VolumeRecGS vo;
    static ResultBuf255 complet;
    static ResultBuf255 nom_volume;
    static GSString255 volume;
    static ResultBuf32 peripherique;
    unsigned int i;

    if (!volume_permis) return 0;
    ex.pCount = 2;
    ex.inputPath = gs(prefixe);
    complet.bufSize = sizeof complet;
    ex.outputPath = &complet;
    ExpandPathGS(&ex);
    if (toolerror() != 0) return 0;
    for (i = 1; i < complet.bufString.length && complet.bufString.text[i] != ':'; i++) ;
    volume.length = (Word) i;                 /* « :VOL » */
    memcpy(volume.text, complet.bufString.text, i);
    dn.pCount = 2;
    dn.devName = (GSString32Ptr) &volume;
    GetDevNumberGS(&dn);
    if (toolerror() != 0) return 0;
    di.pCount = 2;
    di.devNum = dn.devNum;
    peripherique.bufSize = sizeof peripherique;
    di.devName = &peripherique;
    DInfoGS(&di);
    if (toolerror() != 0) return 0;
    vo.pCount = 6;
    vo.devName = &peripherique.bufString;
    nom_volume.bufSize = sizeof nom_volume;
    vo.volName = &nom_volume;
    VolumeGS(&vo);
    if (toolerror() != 0 || !meme_volume(&volume, &nom_volume)) return 0;   /* disque changé */
    return vo.freeBlocks * (unsigned long) vo.blockSize / 256ul;
}

int plateforme_espace_vider(EspaceFichier e)
{
    char nom[16];
    while (plateforme_fichier_lister(e, 0, nom))
        if (plateforme_fichier_supprimer(e, nom) != PLAT_OK) return PLAT_ERREUR;
    return PLAT_OK;
}
