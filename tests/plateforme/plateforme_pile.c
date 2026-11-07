/* plateforme_pile.c — pile du 65816 : profondeur atteinte par les sessions les
   plus exigeantes, comparée à la pile du programme LSE sur l'Apple IIGS
   EXI: EXI-0001  IMP: IMP-0272 (pile de 16 Kio), IMP-0192
   Spécification des tests : Construction §5 bis ; audit du code du 2026-10-29
   (AC-01, AC-11).
   Méthode : une récursion de sonde jusqu'au contrôle de débordement (#pragma
   debug bit 0) donne l'adresse la plus basse admise ; SystemError, remplacée,
   reprend la main par longjmp. Avant chaque session, la zone libre est peinte
   d'un motif ; la session est jouée par le bouchon de console ; l'octet peint
   le plus bas qui a changé donne la profondeur atteinte sous main.
   Borne : 15 840 octets disponibles sous main pour LSE sur l'Apple IIGS avec
   16 Kio (mesure du 2026-10-29), moins 1 Kio réservé à SANE et à GS/OS, que
   Golden Gate ne compte pas. La profondeur ne dépend pas de la machine ; la
   pile disponible, si (Golden Gate laisse 270 octets de plus) : la borne porte
   donc sur la profondeur. Niveau : plateforme, sous Golden Gate. */

#pragma stacksize 16384

#include <string.h>
#include <setjmp.h>
#include "../commun/verif.h"
#include "../../src/noyau/moniteur.h"
#include "../../src/noyau/programme.h"
#include "../../src/interfaces/types_communs.h"
#include "../../src/interfaces/interface_fichiers.h"
#include "../../src/interfaces/profil.h"   /* contrôle de pile, indispensable à la sonde */

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

segment "suite";            /* le segment blanc dépasserait 64 Kio en profil dev */

void bouchon_console(const char *const *lignes);

extern pascal void SystemPrintError(int);
extern pascal void SystemErrorLocation(void);

#define X "\x13"            /* X-OFF */
#define ID1 "ID" X, "1" X, ""
#define MOTIF 0xA5u
#define BORNE 14816ul       /* 15 840 - 1 024 */

static jmp_buf reprise;
static int intercepter = 0;
static int erreur_vue = 0;
static unsigned long plus_bas = 0xFFFFFFFFul;
static unsigned long bas_peint;
static unsigned long haut_peint;
static unsigned long ancre;
static unsigned long profondeur_max = 0;
static char ligne[100];

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

/* debut, k parenthèses ouvrantes, milieu, k fermantes, X-OFF. */
static const char *imbriquer(const char *debut, unsigned int k, const char *milieu)
{
    unsigned int i;
    unsigned int j;
    strcpy(ligne, debut);
    i = (unsigned int) strlen(ligne);
    for (j = 0; j < k; j++) ligne[i++] = '(';
    strcpy(ligne + i, milieu);
    i = (unsigned int) strlen(ligne);
    for (j = 0; j < k; j++) ligne[i++] = ')';
    strcpy(ligne + i, X);
    return ligne;
}

/* Profondeur atteinte sous main par la session ; 0 sur un débordement. */
static unsigned long jouer(const char *const *reponses)
{
    unsigned long bas;
    peindre();
    intercepter = 1;
    if (setjmp(reprise) != 0) {
        intercepter = 0;
        return 0;
    }
    bouchon_console(reponses);
    moniteur_session();
    intercepter = 0;
    bas = mesurer();
    return ancre - bas;
}

static int controler(const char *titre, const char *const *reponses)
{
    unsigned long p = jouer(reponses);
    printf("  %-38s : %5lu octets sous main\n", titre, p);
    if (p > profondeur_max) profondeur_max = p;
    return p != 0 && p <= BORNE;
}

int main(void)
{
    volatile unsigned char repere;
    static const char *r_ligne[] = { "BO" X, NULL, "AU" X, NULL };
    static const char *r_programme[] = { "BO" X, NULL, "20 TERMINER" X, "LI" X, "10" X,
                                          "EX" X, "10" X, "AU" X, NULL };
    static const char *const r_abs[] = { "BO" X,
        "?ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(ABS(1)))))))))))))))" X,
        "AU" X, NULL };
    static const char *const r_si[] = { "BO" X,
        "?SI 1=1 ALORS SI 1=1 ALORS SI 1=1 ALORS (((((1))))) SINON 2 SINON 3 SINON 4" X,
        "AU" X, NULL };
    static const char *const r_formats[] = { "BO" X,
        "10 A_((((((((((((((((((((((1.5+2.25E7)))))))))))))))))))))" X,
        "20 AFFICHER[3F10.3,/,E15.6,U]1.5,2.25,3.125E10,-7.5E-3,12345.678" X,
        "30 B_SI A>0 ALORS SCH('ABCDEF',2,3)!'XYZ' SINON 'Q'" X,
        "40 TERMINER" X, "LI" X, "1" X, "EX" X, "1" X, "AU" X, NULL };
    static const char *const r_reels[] = { "BO" X,
        "10 A_1.5E10+2.25*B-3.125E-7/C" X, "20 AFFICHER[2F8.3,E12.4]A,B,C" X,
        "30 B_SCH('ABCDEFGHIJ',1,2)!CCA(1.5E10)" X, "LI" X, "1" X, "AU" X, NULL };
    static const char *const r_garer[] = { "BO" X, ID1,
        "1 A_3;GARER A,1,'TT';GARER A,2,'TT';GARER A,1,'T2';GARER A,1,'T3'" X,
        "2 CHARGER B,1,'TT';AFFICHER B" X, "3 TERMINER" X, "EX" X, "1" X,
        "CA" X, "TT,PP" X, "AU" X, NULL };
    static const char *const r_codage[] = { "BO" X, ID1,
        "1 CHAINE C;C_'5 X_((((((((((((((((1.5))))))))))))))))'!EQC(0);GARER C,1,'EE'" X,
        "2 TERMINER" X, "EX" X, "1" X, "EF" X, "*" X, "EN" X, "EE" X, "LI" X, "1" X,
        "DE" X, "XX" X, "AU" X, NULL };
    static const char *const r_recursion[] = { "BO" X, "10 R_&FACT(25);AFFICHER R" X,
        "20 TERMINER" X, "25 PROCEDURE &FACT(N) LOCAL N" X,
        "27 RESULTAT SI N=0 ALORS 1 SINON N*&FACT(N-1)" X, "EX" X, "10" X, "AU" X, NULL };

    VERIF_DEBUT("plateforme/pile");
    ancre = (unsigned long) &repere;
    intercepter = 1;
    if (setjmp(reprise) == 0) sonde(0);
    intercepter = 0;
    bas_peint = plus_bas;
    VERIFIER_EGAL(15, erreur_vue);                         /* « Stack overflow » rattrapé */
    printf("  pile disponible sous main : %lu octets ; borne de profondeur : %lu\n",
           ancre - plus_bas, BORNE);

    programme_initialiser();
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBPILE"));
    plateforme_espace_vider(ESPACE_TEMPORAIRES);
    plateforme_espace_vider(ESPACE_DONNEES);

    r_ligne[1] = imbriquer("?", 38, "1");                  /* pire cas : 78 caractères */
    VERIFIER(controler("? et 38 parentheses", (const char *const *) r_ligne));
    r_ligne[1] = imbriquer("?", 37, "1+");                 /* erreur de compilation au fond */
    VERIFIER(controler("? et 37 parentheses, erreur", (const char *const *) r_ligne));
    VERIFIER(controler("15 ABS imbriques", r_abs));
    VERIFIER(controler("SI imbriques en expression", r_si));
    r_programme[1] = imbriquer("10 A_", 36, "1");
    VERIFIER(controler("10 A_ et 36 parentheses, LI, EX", (const char *const *) r_programme));
    r_programme[1] = imbriquer("10 AFFICHER ", 33, "1");
    VERIFIER(controler("10 AFFICHER et 33 parentheses, LI, EX", (const char *const *) r_programme));
    VERIFIER(controler("formats, LISTER, EXECUTER", r_formats));
    VERIFIER(controler("LISTER de reels", r_reels));
    VERIFIER(controler("GARER, CHARGER, CATALOGUER", r_garer));
    VERIFIER(controler("ENCODER, DECODER", r_codage));
    VERIFIER(controler("fonction recursive (25 niveaux)", r_recursion));
    printf("  profondeur maximale : %lu octets ; marge sur la borne : %lu\n",
           profondeur_max, profondeur_max <= BORNE ? BORNE - profondeur_max : 0ul);

    plateforme_espace_vider(ESPACE_TEMPORAIRES);
    plateforme_espace_vider(ESPACE_DONNEES);
    VERIF_FIN();
}
