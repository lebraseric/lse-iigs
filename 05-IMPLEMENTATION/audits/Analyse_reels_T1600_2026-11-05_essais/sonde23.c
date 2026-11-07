/* sonde23.c — coût de l'arrondi à 23 bits d'un réel étendu (essai, hors dépôt) */
#pragma optimize 7
#pragma debug 1
#include <stdio.h>
#include <stdlib.h>

typedef union { long double x; unsigned int w[5]; } Etendu;

/* 23 bits significatifs, demie en s'éloignant de zéro : on ajoute la demie
   unité du dernier rang gardé (bit 40 de la mantisse), puis on tronque. */
static void arrondir23(long double *z)
{
    Etendu *e = (Etendu *) z;
    unsigned int m;

    if ((e->w[4] & 0x7FFF) == 0x7FFF) return;            /* infini, NaN */
    m = e->w[2] + 0x0100u;
    if (m < e->w[2]) {                                    /* retenue */
        e->w[3]++;
        if (e->w[3] == 0) { e->w[3] = 0x8000u; e->w[4]++; }
    }
    e->w[2] = m & 0xFE00u;
    e->w[1] = 0;
    e->w[0] = 0;
}

static volatile long double cible;
static volatile long double source;

int main(int argc, char *argv[])
{
    unsigned int i;
    long double z;
    int mode = argc > 1 ? atoi(argv[1]) : 0;
    unsigned int n = argc > 2 ? (unsigned int) atoi(argv[2]) : 2000u;

    source = 3.6457513110645906L;
    for (i = 0; i < n; i++) {
        z = source;
        if (mode == 1) arrondir23(&z);
        cible = z;
    }
    if (mode == 2) {                                      /* contrôle */
        z = source; arrondir23(&z);
        printf("%.10Lf\n", z);
        z = 8.2L; arrondir23(&z); printf("%.10Lf\n", z);
        z = (long double) 0xFFFFFF; arrondir23(&z); printf("%.1Lf\n", z);
        z = (long double) 0x7FFFFF; arrondir23(&z); printf("%.1Lf\n", z);
    }
    return 0;
}
