/* audit_g3_pile.c — sessions sous une pile de 4 Kio, comme le lanceur du
   shell ORCA (manuel ORCA/C, #pragma stacksize). Copie de travail seulement.
   Usage : audit_g3_pile n (scénario n). */
#pragma stacksize 4096

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/noyau/moniteur.h"
#include "../../src/noyau/programme.h"
#include "../../src/interfaces/types_communs.h"
#include "../../src/interfaces/interface_fichiers.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

segment "suite";

extern char bouchon_affichage[];
void bouchon_console(const char *const *lignes);

#define X "\x13"

static const char *const s0[] = { "BO" X, "10 X_1" X, "AU" X, NULL };
static const char *const s1[] = { "BO" X, "10 X_((((1))))" X, "AU" X, NULL };
static const char *const s2[] = { "BO" X, "10 X_((((((1))))))" X, "AU" X, NULL };
static const char *const s3[] = { "BO" X, "10 X_((((((((1))))))))" X, "AU" X, NULL };
static const char *const s4[] = { "BO" X, "10 X_(((((((((1)))))))))" X, "AU" X, NULL };
static const char *const s5[] = { "BO" X, "10 X_((((((((((1))))))))))" X, "AU" X, NULL };
static const char *const s6[] = { "BO" X, "10 X_(((((((((((1)))))))))))" X, "AU" X, NULL };
static const char *const s7[] = { "BO" X, "10 X_((((((((((((1))))))))))))" X, "AU" X, NULL };
static const char *const s8[] = { "BO" X, "?1" X, "AU" X, NULL };
static const char *const s9[] = { "BO" X, "?((1))" X, "AU" X, NULL };
static const char *const s10[] = { "BO" X, "?((((1))))" X, "AU" X, NULL };
static const char *const s11[] = { "BO" X, "?(((((1)))))" X, "AU" X, NULL };
static const char *const s12[] = { "BO" X, "?((((((1))))))" X, "AU" X, NULL };
static const char *const s13[] = { "BO" X, "?(((((((1)))))))" X, "AU" X, NULL };
static const char *const s14[] = { "BO" X, "?((((((((1))))))))" X, "AU" X, NULL };
static const char *const s15[] = { "BO" X, "20 AFFICHER['ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789',F12.3,2X,E12.4]1.5,2.75" X, "30 SI A=1 ALORS SI B=2 ALORS X_1.25 SINON X_2" X, "LI" X, "1" X, "AU" X, NULL };
static const char *const s16[] = { "BO" X, "10 A_&F(2)" X, "20 TERMINER" X, "30 PROCEDURE &F(B) LOCAL B" X, "40 AFFICHER[F12.3,2X,E12.4,2X,U]B*1.5,B/3,B" X, "50 RESULTAT B" X, "EX" X, "10" X, "AU" X, NULL };
static const char *const s17[] = { "BO" X, "1 CHAINE C;C_'10 X_((((((((1))))))))'!EQC(0);GARER C,1,'EE'" X, "2 TERMINER" X, "EX" X, "1" X, "EF" X, "*" X, "EN" X, "EE" X, "AU" X, NULL };
static const char *const s18[] = { "BO" X, "10 SI (A+(B*(C-D)))>0 ALORS X_((A+B)*(C+D)) SINON X_1" X, "AU" X, NULL };
static const char *const s19[] = { "BO" X, "?(A+(B*(C-1)))" X, "AU" X, NULL };
static const char *const *const scenarios[] = { s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19 };
static const char *const noms[] = { "ligne 0 parentheses", "ligne 4 parentheses", "ligne 6 parentheses", "ligne 8 parentheses", "ligne 9 parentheses", "ligne 10 parentheses", "ligne 11 parentheses", "ligne 12 parentheses", "bureau 0 parentheses", "bureau 2 parentheses", "bureau 4 parentheses", "bureau 5 parentheses", "bureau 6 parentheses", "bureau 7 parentheses", "bureau 8 parentheses", "LISTER", "EXECUTER AFFICHER F", "ENCODER 8 parentheses", "ligne 4 parentheses et SI", "bureau AFFICHER 3 niveaux" };
#define NB 20

static unsigned int sp_lire(void)
{
    unsigned int v;
    asm {
        tsc
        sta v
    }
    return v;
}

int main(int argc, char *argv[])
{
    int n = argc > 1 ? atoi(argv[1]) : 0;
    char *p;
    if (n < 0 || n >= NB) { printf("NB=%d\n", NB); return 0; }
    printf("SCENARIO %d %s (SP main %04X)\n", n, noms[n], sp_lire());
    fflush(stdout);
    programme_initialiser();
    plateforme_bibliotheque_ouvrir("tests:BIBG3");
    bouchon_console(scenarios[n]);
    moniteur_session();
    for (p = bouchon_affichage; *p; p++) if (*p == '\r') *p = '~';
    printf("%s\nFIN %d\n", bouchon_affichage, n);
    return 0;
}
