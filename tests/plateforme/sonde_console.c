/* sonde_console.c — sonde des appels du pilote .CONSOLE de GS/OS (étape 6c, P6)
   Niveau : sur cible. Non interactive : chaque appel est fait une fois et son
   code d'erreur affiché ; l'état du pilote est restauré à la fin. Mesure en
   outre la durée d'une lecture sans attente du pilote, d'une lecture du
   registre du clavier ($C000) et d'un sinus, sur 1000 répétitions, sans
   toucher au clavier. Affiche d'abord la mémoire selon le Memory Manager
   (IMP-0134), puis alloue par plateforme_bloc_allouer au plus 1 Mio en
   blocs de 4 Kio, sans épuiser la mémoire (un épuisement complet a planté
   le système le 2026-10-29), relève leurs banques et le mot d'attributs que
   le Memory Manager leur a enregistré (IMP-0271 : attrNoSpec posé), et les
   rend. Liée avec src/plateforme/memoire_iigs.c.
   Référence : Apple IIGS GS/OS Reference, chap. 9 (03-APPLE-IIGS/systeme/) ;
   Apple IIGS Toolbox Reference, vol. 1, chap. 12, vol. 3, chap. 36
   (03-APPLE-IIGS/manuels/). */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <gsos.h>
#include <memory.h>
#include <orca.h>
#include "../../src/interfaces/interface_memoire.h"

#define NB_BLOCS_MAX 256                 /* 1 Mio de blocs de 4 Kio */
#define RESERVE      0x100000ul          /* FreeMem laissé au système */

static Word dev;
static HandlePlateforme blocs[NB_BLOCS_MAX];

static int banque_speciale(unsigned int b)
{
    return b == 0x00 || b == 0x01 || b == 0xE0 || b == 0xE1;
}

/* Mot d'attributs du bloc, à l'octet 4 de l'enregistrement du pointeur
   maître (Toolbox Reference, vol. 1, p. 12-30, note de HLock). */
static Word attributs(HandlePlateforme h)
{
    return *(Word *) ((unsigned char *) h + 4);
}

/* IMP-0271 : au plus 1 Mio de blocs de 4 Kio, sans épuiser la mémoire. */
static void allocation(void)
{
    unsigned int n = 0;
    unsigned int speciaux = 0;
    unsigned int sans_nospec = 0;
    unsigned int b;
    unsigned int bmin = 0xFF;
    unsigned int bmax = 0;
    unsigned int i;
    Word a;

    printf("SONDE ALLOCATION (plateforme_bloc_allouer, 4096 octets)\n");
    while (n < NB_BLOCS_MAX && FreeMem() > RESERVE
           && (blocs[n] = plateforme_bloc_allouer(4096ul)) != NULL) {
        b = (unsigned int) (((unsigned long) plateforme_bloc_adresse(blocs[n]) >> 16) & 0xFF);
        if (banque_speciale(b)) speciaux++;
        if (b < bmin) bmin = b;
        if (b > bmax) bmax = b;
        if ((attributs(blocs[n]) & attrNoSpec) == 0) sans_nospec++;
        n++;
    }
    printf("  blocs alloues  %u (%lu octets)\n", n, (unsigned long) n * 4096ul);
    if (n == 0) return;
    a = attributs(blocs[0]);
    printf("  attributs du premier bloc $%04X (NoSpec %s, NoCross %s, purge %u, verrou %s)\n",
           a, (a & attrNoSpec) ? "oui" : "non", (a & attrNoCross) ? "oui" : "non",
           (a & attrPurge) >> 8, (a & attrLocked) ? "oui" : "non");
    printf("  blocs sans NoSpec : %u\n", sans_nospec);
    printf("  banques        $%02X a $%02X\n", bmin, bmax);
    printf("  en memoire speciale ($00, $01, $E0, $E1) : %u\n", speciaux);
    printf("  FreeMem apres allocation %lu\n", (unsigned long) FreeMem());
    for (i = 0; i < n; i++)
        plateforme_bloc_liberer(blocs[i]);
    printf("  FreeMem apres restitution %lu\n", (unsigned long) FreeMem());
}

/* IMP-0134 : mesure sur cible de la mémoire disponible, au lancement. */
static void memoire(void)
{
    printf("SONDE MEMORY MANAGER\n");
    printf("  userid      $%04X\n", (unsigned int) userid());
    printf("  TotalMem    %10lu octets\n", (unsigned long) TotalMem());
    printf("  FreeMem     %10lu octets\n", (unsigned long) FreeMem());
    printf("  RealFreeMem %10lu octets\n", (unsigned long) RealFreeMem());
    printf("  MaxBlock    %10lu octets\n", (unsigned long) MaxBlock());
}

static void statut(const char *nom, Word code, void *liste, LongWord n)
{
    DAccessRecGS d;
    d.pCount = 5;
    d.devNum = dev;
    d.code = code;
    d.list = (Pointer) liste;
    d.requestCount = n;
    d.transferCount = 0;
    DStatusGS(&d);
    printf("  DStatus %-16s $%04X : erreur $%04X, transfert %lu\n", nom, code, toolerror(), d.transferCount);
}

static void controle(const char *nom, Word code, void *liste, LongWord n)
{
    DAccessRecGS d;
    d.pCount = 5;
    d.devNum = dev;
    d.code = code;
    d.list = (Pointer) liste;
    d.requestCount = n;
    d.transferCount = 0;
    DControlGS(&d);
    printf("  DControl %-15s $%04X : erreur $%04X\n", nom, code, toolerror());
}

static LongWord ecrire(const char *t, LongWord n)
{
    DIORecGS d;
    d.pCount = 6;
    d.devNum = dev;
    d.buffer = (Pointer) t;
    d.requestCount = n;
    d.startingBlock = 0;
    d.blockSize = 0;
    d.transferCount = 0;
    DWriteGS(&d);
    return d.transferCount;
}

int main(void)
{
    static GSString32 nom = { 8, ".CONSOLE" };
    DevNumRecGS dn;
    DIORecGS lu;
    Word mode = 0;
    Word attente = 0;
    Word brut = 0x8000;
    Word sans_attente = 0x8000;
    char c[4];
    LongWord n;

    memoire();
    allocation();
    printf("SONDE .CONSOLE\n");
    dn.pCount = 2;
    dn.devName = &nom;
    dn.devNum = 0;
    GetDevNumberGS(&dn);
    printf("  GetDevNumber .CONSOLE : erreur $%04X, numero %u\n", toolerror(), dn.devNum);
    dev = dn.devNum;
    if (dev == 0) return 1;

    statut("GetReadMode", 0x8005, &mode, 2);
    printf("    mode de lecture initial $%04X\n", mode);
    statut("GetWaitStatus", 0x0002, &attente, 2);
    printf("    attente initiale $%04X\n", attente);

    controle("SetReadMode", 0x8003, &brut, 2);
    controle("SetWaitStatus", 0x0004, &sans_attente, 2);

    lu.pCount = 6;
    lu.devNum = dev;
    lu.buffer = (Pointer) c;
    lu.requestCount = 1;
    lu.startingBlock = 0;
    lu.blockSize = 0;
    lu.transferCount = 99;
    DReadGS(&lu);
    printf("  DRead 1 car. sans attente : erreur $%04X, transfert %lu\n", toolerror(), lu.transferCount);

    {                                       /* durées, sans toucher au clavier */
        clock_t t0;
        clock_t t1;
        unsigned int i;
        volatile unsigned char *clavier = (volatile unsigned char *) 0xE0C000L;
        volatile unsigned char k = 0;
        volatile double x = 0.0;
        double cps = (double) CLOCKS_PER_SEC;

        t0 = clock();
        for (i = 0; i < 1000; i++) {
            lu.transferCount = 0;
            DReadGS(&lu);
        }
        t1 = clock();
        printf("  1000 DRead sans attente : %.3f ms chacune\n", (double) (t1 - t0) * 1000.0 / cps / 1000.0);
        t0 = clock();
        for (i = 0; i < 1000; i++) k = *clavier;
        t1 = clock();
        printf("  1000 lectures de $C000 : %.4f ms chacune (dernier octet $%02X)\n",
               (double) (t1 - t0) * 1000.0 / cps / 1000.0, (unsigned int) k);
        t0 = clock();
        for (i = 0; i < 1000; i++) x = sin((double) i);
        t1 = clock();
        printf("  1000 sinus : %.3f ms chacun\n", (double) (t1 - t0) * 1000.0 / cps / 1000.0);
        {
            volatile float f = 1.0f;
            volatile float g = 2.0f;
            volatile double y = 0.0;
            volatile long l = 0;
            volatile int c = 0;
            t0 = clock();
            for (i = 0; i < 1000; i++) f = f + g;
            t1 = clock();
            printf("  1000 additions float : %.3f ms chacune\n", (double) (t1 - t0) * 1000.0 / cps / 1000.0);
            t0 = clock();
            for (i = 0; i < 1000; i++) y = (double) f;
            t1 = clock();
            printf("  1000 conversions float -> double : %.3f ms chacune\n", (double) (t1 - t0) * 1000.0 / cps / 1000.0);
            t0 = clock();
            for (i = 0; i < 1000; i++) c = y > 3.4e38;
            t1 = clock();
            printf("  1000 comparaisons double : %.3f ms chacune\n", (double) (t1 - t0) * 1000.0 / cps / 1000.0);
            t0 = clock();
            for (i = 0; i < 1000; i++) l = l + (long) i;
            t1 = clock();
            printf("  1000 additions long : %.4f ms chacune\n", (double) (t1 - t0) * 1000.0 / cps / 1000.0);
        }
        t0 = clock();
        for (i = 0; i < 1000; i++) k = (unsigned char) i;
        t1 = clock();
        printf("  1000 tours de boucle vide : %.4f ms chacun\n", (double) (t1 - t0) * 1000.0 / cps / 1000.0);
    }

    n = ecrire("\x15\x0D", 2);              /* consLF faux : CR sans LF */
    printf("  DWrite mot de mouvement : erreur $%04X, transfert %lu\n", toolerror(), n);
    n = ecrire("ABC\rX\n\x07", 7);
    printf("\n  DWrite ABC CR X LF BEL : erreur $%04X, transfert %lu\n", toolerror(), n);
    n = ecrire("\x15\x1F", 2);              /* valeurs par défaut */

    controle("SetWaitStatus", 0x0004, &attente, 2);
    controle("SetReadMode", 0x8003, &mode, 2);
    printf("FIN SONDE\n");
    return 0;
}
