/* ruban.c — perforateur et lecteur de ruban, réalisés par un fichier texte
   EXI: EXI-0028 (dialogue)
   VM: VM-2201
   IMP: IMP-0262 (ruban), IMP-0263 (lecture), IMP-0264 (PERFORER), IMP-0274
        (perforateur piloté par programme), IMP-0275 (ordres à la télétype),
        IMP-0282 (console à 7 bits)
   Tests: tests/composants/composants_moniteur.c,
          tests/composants/composants_entrees_sorties.c
   Spécification : chapitre Bibliothécaire et fichiers §7 sexies. Segment
   "codage". */

#pragma noroot

#include <string.h>
#include "ruban.h"
#include "../interfaces/interface_fichiers.h"
#include "../interfaces/interface_console.h"

segment "codage";

#define CAR_NUL  0x00
#define CAR_LF   0x0A
#define CAR_CR   0x0D
#define CAR_XON  0x11                    /* EQC(17) : mise en route du lecteur */
#define CAR_DC2  0x12                    /* EQC(18) : mise en route du perforateur */
#define CAR_XOFF 0x13
#define CAR_DC4  0x14                    /* EQC(20) : arrêt du perforateur */
#define CAR_NAK  0x15                    /* EQC(21) : arrêt du lecteur */
#define CAR_ESC  0x1B
#define BLOC     256u

static int           en_marche = 0;
static unsigned long position = 0;       /* prochain caractère à lire */
static unsigned char bloc[BLOC];
static unsigned long bloc_debut = 0;
static unsigned int  bloc_lg = 0;        /* 0 : bloc vide */
static FichierPlateforme perfo = NULL;

void ruban_lecteur(int marche)
{
    en_marche = marche != 0;
    bloc_lg = 0;                         /* le ruban a pu être perforé entre-temps */
}

int ruban_en_marche(void)
{
    return en_marche;
}

/* Caractère à la position courante ; -1 à la fin du ruban. */
static int caractere(void)
{
    FichierPlateforme f;
    unsigned long taille;
    int c;

    if (bloc_lg == 0 || position < bloc_debut || position >= bloc_debut + bloc_lg) {
        bloc_lg = 0;
        if (plateforme_fichier_ouvrir(ESPACE_RUBAN, "RUBAN", FICHIER_LECTURE, &f) != PLAT_OK) return -1;
        taille = plateforme_fichier_taille(f);
        if (position < taille) {
            bloc_debut = position;
            bloc_lg = taille - position > BLOC ? BLOC : (unsigned int) (taille - position);
            if (plateforme_fichier_lire(f, bloc_debut, bloc, bloc_lg) != PLAT_OK) bloc_lg = 0;
        }
        plateforme_fichier_fermer(f);
        if (bloc_lg == 0) return -1;
    }
    c = bloc[position - bloc_debut];
    position++;
    return c;
}

int ruban_lire_ligne(char *texte, unsigned int max, unsigned int *n, unsigned char *fin)
{
    int c;
    int vu = 0;

    *n = 0;
    for (;;) {
        c = caractere();
        if (c < 0) {                     /* fin du ruban (IMP-0263) */
            en_marche = 0;
            position = 0;
            bloc_lg = 0;
            if (!vu) return RUBAN_FIN;
            *fin = SAISIE_RETURN;
            return RUBAN_LIGNE;
        }
        if (c == CAR_NUL || c == CAR_LF) continue;
        vu = 1;
        if (c == CAR_ESC) {
            en_marche = 0;
            return RUBAN_ESC;
        }
        if (c == CAR_CR || c == CAR_XOFF) {
            *fin = c == CAR_CR ? SAISIE_RETURN : SAISIE_XOFF;
            return RUBAN_LIGNE;
        }
        if (*n < max) {
            texte[*n] = (char) c;
            *n = *n + 1;
        }
    }
}

NatureErreur ruban_perforer_ouvrir(void)
{
    if (plateforme_fichier_creer_texte(ESPACE_RUBAN, "RUBAN") != PLAT_OK) return ERR_F_ENTREE_SORTIE;
    if (plateforme_fichier_ouvrir(ESPACE_RUBAN, "RUBAN", FICHIER_MISE_A_JOUR, &perfo) != PLAT_OK) {
        perfo = NULL;
        return ERR_F_ENTREE_SORTIE;
    }
    return ERR_AUCUNE;
}

NatureErreur ruban_perforer(const char *texte, unsigned int n)
{
    static const char cr[1] = { CAR_CR };
    unsigned long fin;

    if (perfo == NULL) return ERR_F_ENTREE_SORTIE;
    fin = plateforme_fichier_taille(perfo);
    if (plateforme_fichier_ecrire(perfo, fin, texte, n) != PLAT_OK
        || plateforme_fichier_ecrire(perfo, fin + n, cr, 1) != PLAT_OK)
        return ERR_F_SUPPORT_PLEIN;
    return ERR_AUCUNE;
}

void ruban_perforer_fermer(void)
{
    if (perfo != NULL) plateforme_fichier_fermer(perfo);
    perfo = NULL;
    bloc_lg = 0;
}

/* --- sortie d'un programme (IMP-0274, IMP-0275, IMP-0282) ----------------------- */

static int perforateur = 0;              /* mis en route par EQC(18) */
static int echec = 0;

/* Ajoute à la fin du ruban ; aucun fichier ne reste ouvert entre deux ajouts. */
static void ajouter(const char *t, unsigned int n)
{
    int ok;

    if (ruban_perforer_ouvrir() != ERR_AUCUNE) ok = 0;
    else {
        ok = plateforme_fichier_ecrire(perfo, plateforme_fichier_taille(perfo), t, n) == PLAT_OK;
        ruban_perforer_fermer();
    }
    if (!ok) {
        perforateur = 0;
        echec = 1;
    }
}

/* La console est un terminal à 7 bits (IMP-0282) : un octet de $80 à $FF y va
   réduit à ses 7 bits, sauf s'il devient un ordre à la télétype (IMP-0275) ;
   le ruban reçoit l'octet entier. */
unsigned int ruban_sortie(const char *texte, unsigned int n)
{
    unsigned int i;
    unsigned int debut_console = 0;
    unsigned int debut_ruban = 0;
    unsigned int vus = 0;
    int c;
    char sept;

    for (i = 0; i <= n; i++) {
        c = i < n ? (unsigned char) texte[i] : -1;
        if (i == n || (c >= CAR_XON && c <= CAR_NAK) || c >= 0x80) {
            if (i > debut_console) {
                console_ecrire(texte + debut_console, i - debut_console);
                vus += i - debut_console;
            }
            debut_console = i + 1;
            if (c >= 0x80 && ((c & 0x7F) < CAR_XON || (c & 0x7F) > CAR_NAK)) {
                sept = (char) (c & 0x7F);
                console_ecrire(&sept, 1);
                vus++;
            }
        }
        if (i == n || c == CAR_DC2 || c == CAR_DC4) {
            if (perforateur && i > debut_ruban) ajouter(texte + debut_ruban, i - debut_ruban);
            debut_ruban = i + 1;
            if (i < n) perforateur = c == CAR_DC2;
        }
    }
    return vus;
}

void ruban_sortie_commande(char c)
{
    if (perforateur) ajouter(&c, 1);
}

int ruban_sortie_en_echec(void)
{
    int e = echec;
    echec = 0;
    return e;
}

void ruban_perforateur_arreter(void)
{
    perforateur = 0;
}
