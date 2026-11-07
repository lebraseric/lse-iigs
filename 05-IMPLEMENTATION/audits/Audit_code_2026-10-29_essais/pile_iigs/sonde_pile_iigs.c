/* sonde_pile_iigs.c — pile du 65816 disponible sous main, à mesurer sur
   l'Apple IIGS (audit du code du 2026-10-29, AC-01). Jamais lié dans LSE.
   Variantes : sans symbole, pile du lanceur ; PILE4K, #pragma stacksize 4096 ;
   PILE16, #pragma stacksize 16384.
   Méthode : une fonction à 32 octets de locales s'appelle elle-même jusqu'au
   contrôle de débordement (#pragma debug bit 0, profil cible) ; SystemError,
   remplacée, reprend la main par longjmp. On affiche l'adresse d'une locale
   de main, l'adresse la plus basse admise et leur écart, comparable aux
   profondeurs mesurées pour LSE sous Golden Gate (1 977 + 238 k octets pour
   ? suivi de k parenthèses). */
#ifdef PILE4K
#pragma stacksize 4096
#endif
#ifdef PILE16
#pragma stacksize 16384
#endif

#include <stdio.h>
#include <setjmp.h>
#include "../../src/interfaces/profil.h"

extern pascal void SystemPrintError(int);
extern pascal void SystemErrorLocation(void);

static jmp_buf reprise;
static int intercepter = 0;
static int erreur_vue = 0;
static unsigned long plus_bas = 0xFFFFFFFFul;
static unsigned int niveaux = 0;

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
    niveaux = n;
    if ((unsigned long) &t[0] < plus_bas) plus_bas = (unsigned long) &t[0];
    sonde(n + 1);
}

int main(void)
{
    volatile unsigned char repere;
    unsigned long ancre = (unsigned long) &repere;

#if defined(PILE16)
    printf("SONDE DE PILE : STACKSIZE 16384\n");
#elif defined(PILE4K)
    printf("SONDE DE PILE : STACKSIZE 4096\n");
#else
    printf("SONDE DE PILE : PILE DU LANCEUR\n");
#endif
    intercepter = 1;
    if (setjmp(reprise) == 0) sonde(0);
    intercepter = 0;
    printf("LOCALE DE MAIN     : $%06lX\n", ancre);
    printf("PLUS BAS ADMIS     : $%06lX\n", plus_bas);
    printf("OCTETS SOUS MAIN   : %lu\n", ancre - plus_bas);
    printf("NIVEAUX DE SONDE   : %u\n", niveaux);
    printf("ERREUR INTERCEPTEE : %d\n", erreur_vue);
    return 0;
}
