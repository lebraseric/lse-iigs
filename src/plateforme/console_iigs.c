/* console_iigs.c — console de l'Apple IIGS, par le pilote .CONSOLE de GS/OS
   EXI: EXI-0052  VM: services d'entrée-sortie (Tome III)
   IMP: IMP-0136 (adaptation isolée dans src/plateforme/), IMP-0220,
        IMP-0238 (X-OFF, RETURN, ESC), IMP-0239 (interruption), IMP-0240,
        IMP-0242 (registre du clavier avant le pilote), IMP-0273 (SystemError),
        IMP-0281 (flèches)
   Tests: tests/plateforme/sonde_console.c ; essai sur cible (IIGS ou émulateur)
   Référence : Apple IIGS GS/OS Reference, chap. 9 (Console Driver).
   Le pilote est un pilote de périphérique de GS/OS, non un outil de la
   Toolbox. Golden Gate ne le fournit pas : la console passe alors par stdio,
   sans interruption par ESC. Segment principal (Construction §1). */

#pragma noroot

#include <stdio.h>
#include <gsos.h>
#include <orca.h>
#include "console_iigs.h"
#include "../interfaces/interface_console.h"
#include "../interfaces/profil.h"

#define CAR_CR    0x0D
#define CAR_LF    0x0A
#define CAR_BEL   0x07
#define CAR_XOFF  0x13
#define CAR_XON   0x11        /* lecteur de ruban (IMP-0263) */
#define CAR_ESC   0x1B
#define CAR_RUBOUT 0x7F       /* DELETE du clavier de l'Apple IIGS */
#define MOUVEMENT 0x15        /* Set Cursor Movement Word */

#define LECTURE_BRUTE 0x8000  /* SetReadMode $8003 */
#define SANS_ATTENTE  0x8000  /* SetWaitStatus $0004 */

static Word dev = 0;                   /* 0 : pas de pilote, repli sur stdio */
static Word mode_initial = 0;
static Word attente_initiale = 0;
static Word attente_courante = 0;

static int echo = 1;                   /* IMP-0254 : IDENTIFICATION, SILENCE */
static int epuisee = 0;                /* fin de l'entrée sous le repli stdio (IMP-0240) */
static int rubout = 0;                 /* RUB-OUT vaut X-OFF (IMP-0238) */

int console_pilote(void)
{
    return dev != 0;
}

int console_fin_entree(void)
{
    return epuisee;
}

void console_echo(int vrai)
{
    echo = vrai != 0;
}

void console_rubout(int vrai)
{
    rubout = vrai != 0;
}

/* Touches frappées d'avance, vues par console_interruption (IMP-0239). */
static char avance[64];
static unsigned int nb_avance = 0;

static void appel(int controle, Word code, Word *valeur)
{
    DAccessRecGS d;
    d.pCount = 5;
    d.devNum = dev;
    d.code = code;
    d.list = (Pointer) valeur;
    d.requestCount = 2;
    d.transferCount = 0;
    if (controle) DControlGS(&d);
    else DStatusGS(&d);
}

static void ecrire_brut(const char *t, unsigned int n)
{
    DIORecGS d;
    if (n == 0) return;
    if (dev == 0) {
        fwrite(t, 1, n, stdout);
        fflush(stdout);
        return;
    }
    d.pCount = 6;
    d.devNum = dev;
    d.buffer = (Pointer) t;
    d.requestCount = n;
    d.startingBlock = 0;
    d.blockSize = 0;
    d.transferCount = 0;
    DWriteGS(&d);
}

/* Flèches (IMP-0281) : à l'écran, `_` ($5F) s'affiche `←` et `^` ($5E) `↑`,
   comme sur les terminaux historiques. Caractères MouseText $48 (flèche
   gauche) et $4B (flèche haute), écrits $C8 et $CB : en mode normal, le
   pilote affiche $C0 à $DF en MouseText, que MouseText soit activé ou non
   (GS/OS Reference, chap. 9, tableau 9-3 ; constaté sur l'Apple IIGS le
   2026-11-05). Le pilote seul : le repli sur stdio écrit le texte tel quel. */
#define TAMPON_ECRAN  96

static void ecrire_ecran(const char *t, unsigned int n)
{
    static char tampon[TAMPON_ECRAN];
    unsigned int i, k;
    char c;

    if (dev == 0) { ecrire_brut(t, n); return; }
    for (i = 0; i < n; i++)
        if (t[i] == '_' || t[i] == '^') break;
    ecrire_brut(t, i);
    k = 0;
    for (; i < n; i++) {
        if (k == TAMPON_ECRAN) { ecrire_brut(tampon, k); k = 0; }
        c = t[i];
        tampon[k] = c == '_' ? (char) 0xC8 : c == '^' ? (char) 0xCB : c;
        k++;
    }
    ecrire_brut(tampon, k);
}

void console_ouvrir(void)
{
    static GSString32 nom = { 8, ".CONSOLE" };
    static const char mouvement[2] = { MOUVEMENT, 0x0D };   /* défilement, retour,
                                                               avance ; CR sans LF */
    DevNumRecGS dn;
    Word brut = LECTURE_BRUTE;

    dn.pCount = 2;
    dn.devName = &nom;
    dn.devNum = 0;
    GetDevNumberGS(&dn);
    dev = toolerror() == 0 ? dn.devNum : 0;
    if (dev == 0) return;
    appel(0, 0x8005, &mode_initial);          /* GetReadMode */
    appel(0, 0x0002, &attente_initiale);      /* GetWaitStatus */
    appel(1, 0x8003, &brut);                  /* SetReadMode : brut */
    attente_courante = attente_initiale;
    ecrire_brut(mouvement, 2);
}

/* Un caractère lu au clavier, sans passer par la frappe d'avance ; -1 si
   attendre est faux et qu'aucune touche n'attend. Avec stdio, la lecture
   attend toujours. */
static int lire_clavier(int attendre)
{
    DIORecGS d;
    char c;
    Word voulu;
    int x;

    if (dev == 0) {
        if (!attendre) return -1;
        x = getchar();
        if (x == EOF) {                        /* ESC défait la lecture en cours ; */
            epuisee = 1;                       /* le moniteur clôt ensuite la session */
            return CAR_ESC;
        }
        return x == '\n' ? CAR_CR : x;
    }
    voulu = attendre ? 0 : SANS_ATTENTE;
    if (voulu != attente_courante) {
        appel(1, 0x0004, &voulu);             /* SetWaitStatus */
        attente_courante = voulu;
    }
    d.pCount = 6;
    d.devNum = dev;
    d.buffer = (Pointer) &c;
    d.requestCount = 1;
    d.startingBlock = 0;
    d.blockSize = 0;
    d.transferCount = 0;
    DReadGS(&d);
    if (d.transferCount == 0) return -1;
    return (unsigned char) c & 0x7F;
}

/* Un caractère : d'abord la frappe d'avance, puis le clavier. */
static int lire_car(int attendre)
{
    unsigned int i;
    int x;

    if (nb_avance > 0) {
        x = (unsigned char) avance[0];
        for (i = 1; i < nb_avance; i++) avance[i - 1] = avance[i];
        nb_avance--;
        return x;
    }
    return lire_clavier(attendre);
}

/* Curseur : pavé fixe, espace en vidéo inverse, la lecture brute n'en
   affichant pas (GS/OS Reference, chap. 9 : $0F inverse, $0E normal,
   $08 retour arrière). Il est écrasé par l'écho du caractère suivant. */
static void curseur_afficher(void)
{
    static const char pave[4] = { 0x0F, ' ', 0x0E, 0x08 };
    if (dev != 0) ecrire_brut(pave, 4);
}

static void curseur_effacer(void)
{
    static const char blanc[2] = { ' ', 0x08 };
    if (dev != 0) ecrire_brut(blanc, 2);
}

void console_ecrire(const char *texte, unsigned int n)
{
    ecrire_ecran(texte, n);
}

void console_retour_chariot(void)
{
    static const char cr[1] = { CAR_CR };
    if (dev == 0) return;          /* stdio : CR y passe à la ligne ; l'interligne suit */
    ecrire_brut(cr, 1);
}

/* L'interligne seul passe au début de la ligne suivante (IMP-0220). */
void console_interligne(void)
{
    static const char crlf[2] = { CAR_CR, CAR_LF };
    if (dev == 0) { ecrire_brut("\n", 1); return; }
    ecrire_brut(crlf, 2);
}

void console_bip(void)
{
    static const char bel[1] = { CAR_BEL };
    ecrire_brut(bel, 1);
}

unsigned int console_lire_ligne(char *tampon, unsigned int max, unsigned char *fin)
{
    unsigned int n = 0;
    int c;
    char e;

    curseur_afficher();
    for (;;) {
        c = lire_car(1);
        if (c == CAR_RUBOUT && rubout) c = CAR_XOFF;
        if (c == CAR_CR || c == CAR_XOFF || c == CAR_ESC) {
            curseur_effacer();
            *fin = c == CAR_CR ? SAISIE_RETURN : c == CAR_XOFF ? SAISIE_XOFF : SAISIE_ESC;
            if (c == CAR_CR && dev != 0) console_interligne();   /* écho de RETURN */
            return n;
        }
        if (c == CAR_XON && n == 0) {                    /* lecteur de ruban (IMP-0263) */
            curseur_effacer();
            *fin = SAISIE_XON;
            return 0;
        }
        if (c < 0x20 || c > 0x7E) continue;              /* aucune touche d'édition */
        if (n >= max) { console_bip(); continue; }
        tampon[n] = (char) c;
        n++;
        if (dev != 0 && echo) {                          /* écho ; stdio : le terminal */
            e = (char) c;
            ecrire_ecran(&e, 1);
            curseur_afficher();
        }
    }
}

/* Registre du clavier de l'Apple IIGS : bit 7 à 1 tant qu'une touche frappée
   n'a pas été lue par le pilote (IMP-0242). Lecture seule ; $C010 n'est
   jamais touché. */
#define CLAVIER ((volatile unsigned char *) 0xE0C000L)

int console_interruption(void)
{
    int c;
    if (dev != 0 && (*CLAVIER & 0x80) == 0) return 0;    /* aucune touche : pas d'appel */
    while ((c = lire_clavier(0)) >= 0) {
        if (c == CAR_ESC) {
            nb_avance = 0;                    /* la frappe d'avance est abandonnée */
            return 1;
        }
        if (nb_avance < sizeof avance) avance[nb_avance++] = (char) c;
    }
    return 0;
}

/* --- fin de session et erreurs d'exécution d'ORCA/C (IMP-0273) -------------------
   Sans contrôle de pile : console_fermer sert aussi à SystemError, qui peut être
   appelée sur un débordement de la pile ; aucune des deux n'appelle donc une
   fonction compilée avec ce contrôle. */
#pragma debug 0

void console_fermer(void)
{
    static const char mouvement[2] = { MOUVEMENT, 0x1F };   /* valeurs par défaut */
    DIORecGS e;
    DAccessRecGS d;

    if (dev == 0) return;
    e.pCount = 6;
    e.devNum = dev;
    e.buffer = (Pointer) mouvement;
    e.requestCount = 2;
    e.startingBlock = 0;
    e.blockSize = 0;
    e.transferCount = 0;
    DWriteGS(&e);
    d.pCount = 5;
    d.devNum = dev;
    d.requestCount = 2;
    d.transferCount = 0;
    d.code = 0x0004;                                         /* SetWaitStatus */
    d.list = (Pointer) &attente_initiale;
    DControlGS(&d);
    d.code = 0x8003;                                         /* SetReadMode */
    d.list = (Pointer) &mode_initial;
    DControlGS(&d);
}

extern pascal void SystemPrintError(int);
extern pascal void SystemErrorLocation(void);

/* Remplace la routine de la bibliothèque d'ORCA/C (manuel ORCA/C 2.2, chap. 20) :
   la console est rendue avant le message et l'arrêt habituels. */
pascal void SystemError(int e)
{
    console_fermer();
    SystemPrintError(e);
    SystemErrorLocation();
}
