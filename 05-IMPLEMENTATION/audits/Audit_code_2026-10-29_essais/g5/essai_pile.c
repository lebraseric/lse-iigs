/* essai_pile.c — audit du groupe 5 : profondeur de la pile du 65816
   atteinte par des sessions réalistes du moniteur.
   Usage : iix tests/essai_pile n   (n : numéro de scénario)
   Méthode : 1) une récursion de sonde jusqu'au contrôle de débordement
   (#pragma debug bit 0) donne l'adresse la plus basse admise ; SystemError
   est remplacée pour reprendre la main par longjmp ; 2) la zone libre est
   peinte d'un motif ; 3) une session du moniteur est jouée par le bouchon
   de console ; 4) l'octet peint le plus bas qui a changé donne la
   profondeur atteinte. Jamais lié dans LSE. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "../../src/noyau/moniteur.h"
#include "../../src/noyau/programme.h"
#include "../../src/interfaces/types_communs.h"
#include "../../src/interfaces/interface_fichiers.h"
#include "../../src/interfaces/profil.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

segment "suite";

extern char bouchon_affichage[];
extern unsigned int bouchon_lg;
void bouchon_console(const char *const *lignes);

extern pascal void SystemPrintError(int);
extern pascal void SystemErrorLocation(void);

#define MOTIF 0xA5u
#define X "\x13"
#define ID1 "ID" X, "1" X, ""

static jmp_buf reprise;
static int intercepter = 0;
static int erreur_vue = 0;
static unsigned long plus_bas = 0xFFFFFFFFul;
static unsigned long bas_peint;
static unsigned long haut_peint;
static unsigned long ancre;

pascal void SystemError(int e)
{
    erreur_vue = e;
    if (intercepter) longjmp(reprise, 1);
    SystemPrintError(e);
    SystemErrorLocation();
}

static void sonde(unsigned int n)
{
    volatile unsigned char t[32];
    t[0] = (unsigned char) n;
    if ((unsigned long) &t[0] < plus_bas) plus_bas = (unsigned long) &t[0];
    sonde(n + 1);
}

static void peindre(void)
{
    volatile unsigned char repere;
    unsigned long a;
    unsigned char *p;
    haut_peint = (unsigned long) &repere - 64ul;
    for (a = bas_peint; a < haut_peint; a = a + 1) {
        p = (unsigned char *) a;
        *p = MOTIF;
    }
}

static unsigned long mesurer(void)
{
    unsigned long a;
    for (a = bas_peint; a < haut_peint; a = a + 1)
        if (*(unsigned char *) a != MOTIF) return a;
    return haut_peint;
}

/* --- scénarios ------------------------------------------------------------ */
static char parens[100];
static const char *const s_base[] = { "BO" X, "?1+1" X, "AU" X, NULL };
static const char *s_par[] = { "BO" X, parens, "AU" X, NULL };
static const char *const s_si[] = { "BO" X,
    "?SI 1=1 ALORS SI 1=1 ALORS SI 1=1 ALORS (((((1))))) SINON 2 SINON 3 SINON 4" X,
    "AU" X, NULL };
static const char *const s_abs[] = { "BO" X,
    "?ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(1)))))))))))))))" X,
    "AU" X, NULL };
static const char *const s_lister[] = { "BO" X,
    "10 A_((((((((((((((((((((((1.5+2.25E7)))))))))))))))))))))" X,
    "20 AFFICHER[3F10.3,/,E15.6,U]1.5,2.25,3.125E10,-7.5E-3,12345.678" X,
    "30 B_SI A>0 ALORS SCH('ABCDEF',2,3)!'XYZ' SINON 'Q'" X,
    "40 TERMINER" X, "LI" X, "1" X, "EX" X, "1" X, "AU" X, NULL };
static const char *const s_garer[] = { "BO" X, ID1,
    "1 A_3;GARER A,1,'TT';GARER A,2,'TT';GARER A,1,'T2';GARER A,1,'T3'" X, "2 CHARGER B,1,'TT';AFFICHER B" X,
    "3 TERMINER" X, "EX" X, "1" X, "CA" X, "TT,PP" X, "AU" X, NULL };
static const char *const s_codage[] = { "BO" X,
    "1 CHAINE C;C_'5 X_((((((((((((((((1.5))))))))))))))))'!EQC(0);GARER C,1,'EE'" X,
    "2 TERMINER" X, "EX" X, "1" X, "EF" X, "*" X, "EN" X, "EE" X, "LI" X, "1" X,
    "DE" X, "XX" X, "AU" X, NULL };
static const char *const s_proc[] = { "BO" X, "10 R_&FACT(25);AFFICHER R" X, "20 TERMINER" X,
    "25 PROCEDURE &FACT(N) LOCAL N" X, "27 RESULTAT SI N=0 ALORS 1 SINON N*&FACT(N-1)" X,
    "EX" X, "10" X, "AU" X, NULL };
static const char *s_err[] = { "BO" X, parens, "AU" X, NULL };
static const char *s_prog[] = { "BO" X, parens, "AU" X, NULL };
static const char *const s_dec[] = { "BO" X,
    "10 A_1.5E10+2.25*B-3.125E-7/C" X, "20 AFFICHER[2F8.3,E12.4]A,B,C" X,
    "30 B_SCH('ABCDEFGHIJ',1,2)!CCA(1.5E10)" X, "LI" X, "1" X, "AU" X, NULL };

int main(int argc, char *argv[])
{
    volatile unsigned char repere_main;
    int n = argc > 1 ? atoi(argv[1]) : 0;
    int k = argc > 2 ? atoi(argv[2]) : 10;
    int i;
    unsigned long hwm;
    const char *const *rep;
    const char *titre;

    ancre = (unsigned long) &repere_main;
    printf("argc %d n %d k %d\n", argc, n, k);
    intercepter = 1;
    if (setjmp(reprise) == 0) sonde(0);
    intercepter = 0;
    printf("ancre main %06lX ; plus basse adresse admise %06lX ; pile utilisable sous main : %lu octets (erreur %d)\n",
           ancre, plus_bas, ancre - plus_bas, erreur_vue);
    erreur_vue = 0;
    bas_peint = plus_bas;

    programme_initialiser();
    if (plateforme_bibliotheque_ouvrir("tests:BIBPILE") != PLAT_OK) printf("bibliotheque ?\n");

    switch (n) {
    case 1:  /* k parenthèses imbriquées, machine de bureau */
        parens[0] = '?';
        for (i = 0; i < k; i++) parens[1 + i] = '(';
        parens[1 + k] = '1';
        for (i = 0; i < k; i++) parens[2 + k + i] = ')';
        strcpy(parens + 2 + 2 * k, X);
        rep = s_par; titre = "parentheses imbriquees"; break;
    case 2: rep = s_si; titre = "SI imbriques (3)"; break;
    case 3: rep = s_abs; titre = "ABS imbriquees (15)"; break;
    case 4: rep = s_lister; titre = "programme, LISTER, EXECUTER"; break;
    case 5: rep = s_garer; titre = "GARER, CHARGER, CATALOGUER"; break;
    case 6: rep = s_codage; titre = "ENCODER, DECODER"; break;
    case 7: rep = s_proc; titre = "fonction recursive L.S.E. (25)"; break;
    case 8:  /* erreur de compilation au fond de k parenthèses */
        parens[0] = '?';
        for (i = 0; i < k; i++) parens[1 + i] = '(';
        parens[1 + k] = '1'; parens[2 + k] = '+';
        for (i = 0; i < k; i++) parens[3 + k + i] = ')';
        strcpy(parens + 3 + 2 * k, X);
        rep = s_err; titre = "erreur au fond des parentheses"; break;
    case 9:  /* ligne de programme : k parenthèses dans une affectation */
    case 10: /* ligne de programme : k parenthèses dans AFFICHER */
        strcpy(parens, n == 9 ? "10 A_" : "10 AFFICHER ");
        i = (int) strlen(parens);
        { int j; for (j = 0; j < k; j++) parens[i++] = '(';
          parens[i++] = '1';
          for (j = 0; j < k; j++) parens[i++] = ')'; }
        strcpy(parens + i, X);
        rep = s_prog; titre = n == 9 ? "ligne A_(...)" : "ligne AFFICHER (...)"; break;
    case 11: rep = s_dec; titre = "LISTER (decompilation, reels)"; break;
    default: rep = s_base; titre = "session minimale"; break;
    }
    peindre();
    intercepter = 1;
    if (setjmp(reprise) == 0) {
        bouchon_console((const char *const *) rep);
        moniteur_session();
        intercepter = 0;
        hwm = mesurer();
        printf("scenario %d (%s, k=%d) : %lu octets sous main ; marge restante %lu octets\n",
               n, titre, k, ancre - hwm, hwm - plus_bas);
    } else {
        intercepter = 0;
        printf("scenario %d (%s, k=%d) : DEBORDEMENT (SystemError %d)\n", n, titre, k, erreur_vue);
    }
    i = (int) strlen(bouchon_affichage);
    printf("affichage (fin) : [%s]\n", i > 300 ? bouchon_affichage + i - 300 : bouchon_affichage);
    return 0;
}
