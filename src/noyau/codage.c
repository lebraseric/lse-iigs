/* codage.c — DECODER et ENCODER : programme courant et fichier-données
   EXI: EXI-0057
   VM: VM-3404, VM-2203
   IV: IV-IF-0004 (décompiler), IV-IF-0005, IV-IF-0006
   IMP: IMP-0259 (format des enregistrements), IMP-0260 (DECODER), IMP-0261
        (ENCODER), IMP-0278 (« $ »)
   Tests: tests/composants/composants_codage.c, tests/composants/composants_moniteur.c
   Spécification : chapitre Bibliothécaire et fichiers §7 quinquies ; INRDP75,
   5e partie, §4 ; LSE15 §9-3. Segment "codage". */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include "codage.h"
#include "fichiers.h"
#include "programme.h"
#include "decompilateur.h"
#include "../interfaces/types_communs.h"
#include "../interfaces/profil.h"

segment "codage";

/* --- DECODER (IMP-0259, IMP-0260) -------------------------------------------- */

static unsigned char tampon[CODAGE_ENREGISTREMENT];
static unsigned int  rempli;
static unsigned int  numero;
static const char   *fichier;
static int           lieu;                /* paramètre permanent de fichiers_garer */

static NatureErreur emettre(unsigned int longueur)
{
    numero++;
    rempli = 0;
    return fichiers_garer(fichier, lieu, numero, ENREG_CHAINE, tampon, longueur);
}

/* MITRA 15 : 252 caractères exactement, lignes coupées ; le dernier
   enregistrement garde la fin du précédent (INRDP75, PG66 bis). */
static NatureErreur ajouter_mitra(const char *t, unsigned int n)
{
    unsigned int i;
    NatureErreur e;

    for (i = 0; i < n; i++) {
        tampon[rempli] = (unsigned char) t[i];
        rempli++;
        if (rempli == CODAGE_ENREGISTREMENT && (e = emettre(CODAGE_ENREGISTREMENT)) != ERR_AUCUNE)
            return e;
    }
    return ERR_AUCUNE;
}

/* T1600 : autant de lignes complètes que possible (INRDP75, PG66) ; une
   ligne plus longue qu'un enregistrement est coupée. */
static NatureErreur ajouter_t1600(const char *t, unsigned int n)
{
    unsigned int i;
    NatureErreur e;

    if (rempli > 0 && rempli + n > CODAGE_ENREGISTREMENT && (e = emettre(rempli)) != ERR_AUCUNE)
        return e;
    for (i = 0; i < n; i++) {
        tampon[rempli] = (unsigned char) t[i];
        rempli++;
        if (rempli == CODAGE_ENREGISTREMENT && i + 1 < n && (e = emettre(rempli)) != ERR_AUCUNE)
            return e;
    }
    return ERR_AUCUNE;
}

NatureErreur codage_decoder(const char *nom, unsigned char n1, unsigned char n2, int unite)
{
    EntreeObjetCompile ligne;
    EntreeObjetCompile suivante;
    char texte[DECOMPILATEUR_LONGUEUR_MAX + 2];
    unsigned int lg;
    unsigned int i;
    int encore;
    int t1600 = PROFIL_EST_T1600(configuration_session.profil);
    NatureErreur e = ERR_AUCUNE;

    lieu = unite ? FICHIERS_UNITE : 0;
    fichiers_supprimer(nom, lieu, 0);                /* remplacé s'il existe */
    memset(tampon, ' ', sizeof tampon);
    rempli = 0;
    numero = 0;
    fichier = nom;
    encore = programme_premiere(&ligne);
    while (encore && ligne.numero_ligne < n1) encore = programme_suivante(ligne.numero_ligne, &ligne);
    if (encore && n2 != 0 && ligne.numero_ligne > n2) encore = 0;
    if (!encore) {                                   /* aucune ligne : le seul NUL */
        texte[0] = CODAGE_FIN_FICHIER;
        e = t1600 ? ajouter_t1600(texte, 1) : ajouter_mitra(texte, 1);
    }
    for (i = 0; encore && e == ERR_AUCUNE; i++) {
        lg = 0;
        if (decompilateur_decompiler(&ligne, texte, &lg) != DECOMP_OK) lg = 0;
        encore = programme_suivante(ligne.numero_ligne, &suivante)
                 && (n2 == 0 || suivante.numero_ligne <= n2);
        if (t1600) {
            /* la dernière ligne finit par NUL au lieu de X-OFF (PG66 : 82) */
            texte[lg] = (char) (encore ? CODAGE_FIN_LIGNE : CODAGE_FIN_FICHIER);
            e = ajouter_t1600(texte, lg + 1);
        } else {
            texte[lg] = (char) CODAGE_FIN_LIGNE;
            texte[lg + 1] = (char) CODAGE_FIN_FICHIER;
            e = ajouter_mitra(texte, encore ? lg + 1 : lg + 2);
        }
        ligne = suivante;
    }
    if (e == ERR_AUCUNE && rempli > 0)
        e = emettre(t1600 ? rempli : CODAGE_ENREGISTREMENT);
    if (e != ERR_AUCUNE) fichiers_supprimer(nom, lieu, 0);
    return e;
}

/* --- ENCODER (IMP-0261) -------------------------------------------------------- */

static char           lu_nom[8];
static unsigned char *lu = NULL;          /* enregistrement courant */
static unsigned long  lu_longueur;
static unsigned long  lu_position;
static unsigned int   lu_numero;
static int            lu_fini;
static int            lu_lieu;

static void liberer(void)
{
    if (lu != NULL) free(lu);
    lu = NULL;
}

NatureErreur codage_ouvrir(const char *nom, int unite)
{
    unsigned char type;
    NatureErreur e;

    liberer();
    strcpy(lu_nom, nom);
    lu_numero = 1;
    lu_position = 0;
    lu_fini = 0;
    lu_lieu = unite ? FICHIERS_UNITE : 0;
    e = fichiers_charger(nom, lu_lieu, 1, &type, &lu, &lu_longueur);
    if (e == ERR_E_FICHIER_INEXISTANT) return ERR_F_FICHIER_INEXISTANT;
    if (e == ERR_E_FICHIER_PROTEGE) return ERR_F_PROGRAMME_PROTEGE;
    if (e == ERR_E_ENREGISTREMENT_INEXISTANT) {      /* fichier vide */
        lu = NULL;
        lu_fini = 1;
        return ERR_AUCUNE;
    }
    if (e != ERR_AUCUNE) return e;
    if (type != ENREG_CHAINE) {
        liberer();
        return ERR_F_FICHIER_ENDOMMAGE;
    }
    return ERR_AUCUNE;
}

/* Caractère suivant du fichier ; -1 à la fin (NUL ou enregistrement absent),
   -2 sur erreur. */
static int caractere(NatureErreur *e)
{
    unsigned char type;
    NatureErreur r;
    int c;

    while (!lu_fini && (lu == NULL || lu_position >= lu_longueur)) {
        liberer();
        lu_numero++;
        lu_position = 0;
        r = fichiers_charger(lu_nom, lu_lieu, lu_numero, &type, &lu, &lu_longueur);
        if (r != ERR_AUCUNE) {
            lu = NULL;
            lu_fini = 1;
            if (r != ERR_E_ENREGISTREMENT_INEXISTANT) {
                *e = r;
                return -2;
            }
        } else if (type != ENREG_CHAINE) {
            liberer();
            lu_fini = 1;
            *e = ERR_F_FICHIER_ENDOMMAGE;
            return -2;
        }
    }
    if (lu_fini) return -1;
    c = lu[lu_position];
    lu_position++;
    if (c == CODAGE_FIN_FICHIER) {
        lu_fini = 1;
        return -1;
    }
    return c;
}

int codage_ligne(char *texte, unsigned int max, unsigned int *n, NatureErreur *e)
{
    int c;
    int vu = 0;

    *e = ERR_AUCUNE;
    *n = 0;
    for (;;) {
        c = caractere(e);
        if (c == -2) return 0;
        if (c == -1) return vu;
        vu = 1;
        if (c == CODAGE_FIN_LIGNE || c == 13) return 1;   /* X-OFF ; CR (IMP-0261) */
        if (c == 10) continue;                             /* LF ignoré */
        if (*n < max) {
            texte[*n] = (char) c;
            *n = *n + 1;
        }
    }
}

void codage_fermer(void)
{
    liberer();
}
