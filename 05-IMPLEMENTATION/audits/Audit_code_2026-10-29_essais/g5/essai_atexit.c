/* essai_atexit.c — audit g5 : un gestionnaire atexit est-il appelé après
   l'arrêt par le contrôle de débordement de pile ? Jamais lié dans LSE. */
#include <stdio.h>
#include <stdlib.h>
#include "../../src/interfaces/profil.h"

static void fin(void) { printf("gestionnaire atexit appele\n"); fflush(stdout); }

static void sonde(unsigned int n)
{
    volatile unsigned char t[32];
    t[0] = (unsigned char) n;
    sonde(n + 1);
}

int main(void)
{
    atexit(fin);
    sonde(0);
    return 0;
}
