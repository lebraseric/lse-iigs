/* essai_debordement.c — audit du groupe 5 : comportement par défaut du
   contrôle de débordement de pile (#pragma debug bit 0) sous Golden Gate.
   Jamais lié dans LSE. */
#include <stdio.h>
#include "../../src/interfaces/profil.h"

static unsigned long plus_bas = 0xFFFFFFFFul;
static unsigned int profondeur = 0;

static void sonde(unsigned int n)
{
    volatile unsigned char t[32];
    t[0] = (unsigned char) n;
    profondeur = n;
    if ((unsigned long) &t[0] < plus_bas) plus_bas = (unsigned long) &t[0];
    if ((n & 15) == 0) { printf("niveau %u adresse %06lX\n", n, plus_bas); fflush(stdout); }
    sonde(n + 1);
}

int main(void)
{
    volatile unsigned char repere;
    printf("ancre main %06lX\n", (unsigned long) &repere);
    sonde(0);
    printf("jamais atteint\n");
    return 0;
}
