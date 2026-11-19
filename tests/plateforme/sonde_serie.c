/* sonde_serie.c — sonde du port série (terminal série, rôle A, voie a′)
   Niveau : sur cible seulement. Golden Gate n'implémente ni DInfo ni le port.
   Sous System 6.0.4, *:SYSTEM:DRIVERS:MODEM et PRINTER sont des pilotes de
   port du Print Manager (type $BB, auxtype $0002 ; Toolbox Reference, chap. 15),
   non des pilotes de périphérique de GS/OS (auxtype $01xx ; GS/OS Device Driver
   Reference, chap. 8, fig. 8-2). La voie éprouvée ici est le pilote que GS/OS
   engendre au démarrage pour le microprogramme Pascal 1.1 du port modem
   (Device Driver Reference, chap. 7 et annexe A) : attente seule, aucun appel
   propre au pilote. La sonde établit :
   1. la liste des périphériques par DInfo, et le pilote du slot 2 (caractères) ;
   2. le mot d'état (GetDeviceStatus, chap. 1, fig. 1-2) et l'attente, avec
      l'essai de SetWaitStatus sans attente (attendu : $22) ;
   3. la durée d'un GetDeviceStatus (1000 appels) et d'une ligne de 80
      caractères écrite par DWrite (tampon de sortie ou écriture synchrone) ;
   4. en dialogue avec le terminal : chaque octet reçu, en hexadécimal à
      l'écran du IIGS et en écho sur le terminal. Mode S (défaut) : l'octet
      n'est lu que si le bit 5 du mot d'état (« receive buffer not empty »)
      est à 1 ; « ! » suspend la lecture 5 s puis vide la file (frappe
      d'avance, tampon du microprogramme) ; ESC du terminal ou une touche du
      IIGS termine. Mode L : lecture bloquante, le bit 5 relevé avant chaque
      lecture ; seul ESC du terminal termine.
   Usage : SONDES [S|L] [numéro de périphérique].
   Réglages attendus (tableau de bord, port modem) : XON/XOFF non, tampon oui,
   LF après CR non. Côté Mac : screen -fn /dev/cu.… 9600,cs8,-ixon,-ixoff
   (-fn : sans contrôle de flux de screen, qui garderait Ctrl-S et Ctrl-Q).
   Essais : 05-IMPLEMENTATION/audits/Essais_IIGS_2026-11-19.md. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <gsos.h>
#include <orca.h>

#define CAR_ESC   0x1B
#define BIT_RECU  0x0020                    /* mot d'état : receive buffer not empty */
#define CLAVIER   ((volatile unsigned char *) 0xE0C000L)
#define STROBE    ((volatile unsigned char *) 0xE0C010L)

static Word dev = 0;
static Word ref = 0;                        /* OpenGS, si DWrite rend $23 */
static ResultBuf32 nom_dev;
static unsigned long interrogations = 0;
static unsigned long avec_bit = 0;
static unsigned long recus = 0;

static double ms(clock_t t0, clock_t t1, unsigned int n)
{
    return (double) (t1 - t0) * 1000.0 / (double) CLOCKS_PER_SEC / (double) n;
}

static Word statut(Word code, void *liste, LongWord n, LongWord *transfert)
{
    DAccessRecGS d;
    d.pCount = 5;
    d.devNum = dev;
    d.code = code;
    d.list = (Pointer) liste;
    d.requestCount = n;
    d.transferCount = 0;
    DStatusGS(&d);
    if (transfert != NULL) *transfert = d.transferCount;
    return toolerror();
}

static Word controle(Word code, void *liste, LongWord n)
{
    DAccessRecGS d;
    d.pCount = 5;
    d.devNum = dev;
    d.code = code;
    d.list = (Pointer) liste;
    d.requestCount = n;
    d.transferCount = 0;
    DControlGS(&d);
    return toolerror();
}

static Word mot_etat(Word *mot)
{
    *mot = 0;
    return statut(0x0000, mot, 2, NULL);    /* GetDeviceStatus, mot seul */
}

static Word ecrire(const char *t, LongWord n, LongWord *transfert)
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
    if (transfert != NULL) *transfert = d.transferCount;
    return toolerror();
}

static void envoyer(const char *s)
{
    ecrire(s, (LongWord) strlen(s), NULL);
}

/* Lecture bloquante d'un octet ; -1 en cas d'erreur. */
static int lire1(Word *erreur)
{
    DIORecGS d;
    unsigned char c = 0;
    d.pCount = 6;
    d.devNum = dev;
    d.buffer = (Pointer) &c;
    d.requestCount = 1;
    d.startingBlock = 0;
    d.blockSize = 0;
    d.transferCount = 0;
    DReadGS(&d);
    *erreur = toolerror();
    if (*erreur != 0 || d.transferCount != 1) return -1;
    return c;
}

static const char *nom_octet(int c)
{
    switch (c & 0x7F) {
    case 0x07: return "BEL";
    case 0x08: return "BS";
    case 0x0A: return "LF";
    case 0x0D: return "CR (RETURN)";
    case 0x11: return "X-ON (Ctrl-Q)";
    case 0x13: return "X-OFF (Ctrl-S)";
    case 0x1B: return "ESC";
    case 0x7F: return "RUB-OUT (DELETE)";
    default:   return "";
    }
}

/* Octet reçu : relevé à l'écran du IIGS, écho sur le terminal. */
static void traiter(int c, int bit_avant)
{
    char t[16];
    recus++;
    printf("  $%02X%s %c %s%s\n", c, (c & 0x80) ? " (bit 7)" : "",
           (c & 0x7F) >= 0x20 && (c & 0x7F) < 0x7F ? c & 0x7F : ' ',
           nom_octet(c), bit_avant < 0 ? "" : bit_avant ? "  [bit 5 : 1]" : "  [bit 5 : 0]");
    if ((c & 0x7F) == 0x0D) envoyer("\r\n");
    else if ((c & 0x7F) >= 0x20 && (c & 0x7F) < 0x7F) {
        t[0] = (char) (c & 0x7F);
        ecrire(t, 1, NULL);
    } else {
        sprintf(t, "<$%02X>", c);
        envoyer(t);
    }
}

/* « ! » : lecture suspendue 5 s, puis la file est vidée par le bit 5. */
static void pause_et_vidange(void)
{
    clock_t t0;
    Word mot;
    Word e;
    int c;
    unsigned int n = 0;
    char t[48];

    envoyer("\r\nPAUSE 5 S : TAPEZ OU COLLEZ\r\n");
    printf("  pause de 5 s\n");
    t0 = clock();
    while (clock() - t0 < 5 * CLOCKS_PER_SEC) ;
    printf("  vidange :");
    for (;;) {
        if ((e = mot_etat(&mot)) != 0 || (mot & BIT_RECU) == 0) break;
        if ((c = lire1(&e)) < 0) break;
        if (n % 16 == 0) printf("\n   ");
        printf(" %02X", c);
        n++;
    }
    printf("\n  %u octets lus apres la pause (erreur $%04X)\n", n, e);
    recus += n;
    sprintf(t, "\r\nRECUS APRES LA PAUSE : %u\r\n", n);
    envoyer(t);
}

/* 1. Périphériques ; renvoie le premier pilote de caractères du slot 2. */
static Word lister(void)
{
    DInfoRecGS di;
    Word d;
    Word e;
    Word choisi = 0;

    printf("SONDE PERIPHERIQUES (DInfo)\n");
    for (d = 1; d <= 40; d++) {
        memset(&di, 0, sizeof di);
        di.pCount = 8;
        di.devNum = d;
        nom_dev.bufSize = sizeof nom_dev;
        nom_dev.bufString.length = 0;
        di.devName = &nom_dev;
        DInfoGS(&di);
        if ((e = toolerror()) != 0) {
            printf("  %u : erreur $%04X, fin de la liste\n", d, e);
            break;
        }
        printf("  %2u %-16.*s car $%04X empl $%04X unite %u type $%04X%s%s\n", d,
               (int) nom_dev.bufString.length, nom_dev.bufString.text,
               di.characteristics, di.slotNum, di.unitNum, di.deviceID,
               (di.characteristics & 0x0080) ? "" : " caracteres",
               (di.characteristics & 0x4000) ? " engendre" : "");
        if (choisi == 0 && (di.slotNum & 0x0007) == 2 && (di.characteristics & 0x0080) == 0)
            choisi = d;
    }
    return choisi;
}

static void nommer(Word d)
{
    DInfoRecGS di;
    memset(&di, 0, sizeof di);
    di.pCount = 2;
    di.devNum = d;
    nom_dev.bufSize = sizeof nom_dev;
    nom_dev.bufString.length = 0;
    di.devName = &nom_dev;
    DInfoGS(&di);
}

/* DWrite rend $23 (pilote de caractères non ouvert) : ouverture par le FST
   des caractères, sous le nom du périphérique. */
static void ouvrir(void)
{
    static GSString255 chemin;
    OpenRecGS o;
    chemin.length = nom_dev.bufString.length;
    memcpy(chemin.text, nom_dev.bufString.text, chemin.length);
    o.pCount = 2;
    o.pathname = &chemin;
    o.refNum = 0;
    OpenGS(&o);
    printf("  OpenGS %.*s : erreur $%04X, ref %u\n", (int) chemin.length, chemin.text,
           toolerror(), o.refNum);
    if (toolerror() == 0) ref = o.refNum;
}

int main(int argc, char *argv[])
{
    char mode = 'S';
    Word mot;
    Word attente = 0;
    Word sans_attente = 0x8000;
    Word e;
    LongWord n;
    clock_t t0;
    clock_t t1;
    unsigned int i;
    int c;
    int bit;
    static const char banniere[] = "\r\nSONDE LSE-IIGS : PORT SERIE\r\n";
    static const char ligne[] =                    /* 78 caractères, CR, LF */
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ012345\r\n";

    for (i = 1; i < (unsigned int) argc; i++) {
        if (argv[i][0] == 'L' || argv[i][0] == 'l') mode = 'L';
        else if (argv[i][0] == 'S' || argv[i][0] == 's') mode = 'S';
        else dev = (Word) atoi(argv[i]);
    }

    if (dev == 0) dev = lister();
    else (void) lister();
    if (dev == 0) {
        printf("AUCUN PILOTE DE CARACTERES SUR LE SLOT 2 (tableau de bord : slot 2 = Modem Port ?)\n");
        return 1;
    }
    nommer(dev);
    printf("SONDE PORT SERIE : peripherique %u %.*s, mode %c\n", dev,
           (int) nom_dev.bufString.length, nom_dev.bufString.text, mode);

    /* 2. État et attente. */
    e = mot_etat(&mot);
    printf("  GetDeviceStatus : erreur $%04X, mot $%04X (ouvert %u, en ligne %u, recu %u, emission vide %u)\n",
           e, mot, mot & 1, (mot >> 4) & 1, (mot >> 5) & 1, (mot >> 6) & 1);
    e = statut(0x0002, &attente, 2, &n);
    printf("  GetWaitStatus : erreur $%04X, attente $%04X\n", e, attente);
    e = controle(0x0004, &sans_attente, 2);
    printf("  SetWaitStatus sans attente : erreur $%04X (attendu $0022)\n", e);
    if (e == 0) {
        e = controle(0x0004, &attente, 2);
        printf("  SetWaitStatus retour a l'attente : erreur $%04X\n", e);
    }

    /* 3. Écriture et durées. */
    e = ecrire(banniere, (LongWord) (sizeof banniere - 1), &n);
    printf("  DWrite banniere : erreur $%04X, transfert %lu\n", e, n);
    if (e == 0x0023) {
        ouvrir();
        e = ecrire(banniere, (LongWord) (sizeof banniere - 1), &n);
        printf("  DWrite banniere apres OpenGS : erreur $%04X, transfert %lu\n", e, n);
    }
    if (e != 0) goto fin;

    t0 = clock();
    for (i = 0; i < 1000; i++) (void) mot_etat(&mot);
    t1 = clock();
    printf("  1000 GetDeviceStatus : %.3f ms chacun\n", ms(t0, t1, 1000));
    t0 = clock();
    for (i = 0; i < 5; i++) ecrire(ligne, (LongWord) (sizeof ligne - 1), NULL);
    t1 = clock();
    printf("  5 lignes de 80 car. : %.1f ms par ligne (9600 bauds sans tampon : 83 ms)\n",
           ms(t0, t1, 5));

    /* 4. Dialogue. */
    if (mode == 'S') {
        envoyer("TAPEZ ; ! = PAUSE 5 S ; ESC = FIN\r\n");
        printf("MODE S : tapez sur le terminal ; ESC ou une touche du IIGS termine\n");
        for (;;) {
            if (*CLAVIER & 0x80) {
                *STROBE = 0;
                printf("  touche du IIGS : fin\n");
                break;
            }
            interrogations++;
            if ((e = mot_etat(&mot)) != 0) {
                printf("  GetDeviceStatus : erreur $%04X, fin\n", e);
                break;
            }
            if ((mot & BIT_RECU) == 0) continue;
            avec_bit++;
            if ((c = lire1(&e)) < 0) {
                printf("  DRead : erreur $%04X, fin\n", e);
                break;
            }
            traiter(c, -1);
            if ((c & 0x7F) == CAR_ESC) break;
            if ((c & 0x7F) == '!') pause_et_vidange();
        }
        printf("  interrogations %lu, dont bit 5 a 1 : %lu\n", interrogations, avec_bit);
    } else {
        envoyer("LECTURE BLOQUANTE ; ESC = FIN\r\n");
        printf("MODE L : tapez sur le terminal ; seul ESC du terminal termine\n");
        for (;;) {
            (void) mot_etat(&mot);
            bit = (mot & BIT_RECU) != 0;
            if ((c = lire1(&e)) < 0) {
                printf("  DRead : erreur $%04X, fin\n", e);
                break;
            }
            traiter(c, bit);
            if ((c & 0x7F) == CAR_ESC) break;
        }
    }
    printf("  octets recus : %lu\n", recus);
    envoyer("\r\nFIN SONDE\r\n");

fin:
    if (ref != 0) {
        RefNumRecGS r;
        r.pCount = 1;
        r.refNum = ref;
        CloseGS(&r);
    }
    printf("FIN SONDE\n");
    return 0;
}
