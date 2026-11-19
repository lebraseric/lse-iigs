/* console_iigs.c — console de l'Apple IIGS : écran (pilote .CONSOLE de GS/OS)
   ou terminal relié au port modem (pilote engendré par GS/OS)
   EXI: EXI-0052  VM: services d'entrée-sortie (Tome III)
   IMP: IMP-0136 (adaptation isolée dans src/plateforme/), IMP-0220,
        IMP-0238 (X-OFF, RETURN, ESC), IMP-0239 (interruption), IMP-0240,
        IMP-0242 (registre du clavier avant le pilote), IMP-0273 (SystemError),
        IMP-0281 (flèches), IMP-0283 (port série, -SERIE)
   Tests: tests/plateforme/sonde_console.c, tests/plateforme/sonde_serie.c ;
          essai sur cible (IIGS ou émulateur)
   Références : Apple IIGS GS/OS Reference, chap. 9 (Console Driver) ;
   GS/OS Device Driver Reference, chap. 1 (mot d'état), chap. 7 et annexe A
   (pilote engendré) ; Firmware Reference, chap. 5 (port série).
   Les pilotes sont des pilotes de périphérique de GS/OS, non des outils de la
   Toolbox. Golden Gate ne fournit ni l'un ni l'autre : la console passe alors
   par stdio, sans interruption par ESC. Segment principal (Construction §1). */

#pragma noroot

#include <stdio.h>
#include <string.h>
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

#define BIT_RECU      0x0020  /* mot d'état : « receive buffer not empty » */
#define SERIE_PERIODE 16      /* une interrogation du port sur seize (IMP-0283) */

/* Genre d'une voie. */
#define VOIE_STDIO 0          /* repli sous Golden Gate (IMP-0240) */
#define VOIE_ECRAN 1          /* pilote .CONSOLE (IMP-0240) */
#define VOIE_SERIE 2          /* pilote engendré du port modem (IMP-0283) */

/* État propre à une console (IMP-0283, structure gardant possible le rôle C :
   plusieurs consoles dans une seule session de LSE). Un seul exemplaire. */
typedef struct {
    int  genre;
    Word dev;                          /* numéro de périphérique ; 0 sous stdio */
    Word ref;                          /* OpenGS du port série ; 0 sinon */
    Word mode_initial;                 /* .CONSOLE : état rendu par console_fermer */
    Word attente_initiale;
    Word attente_courante;
    int  echo;                         /* IMP-0254 : IDENTIFICATION, SILENCE */
    int  epuisee;                      /* fin de l'entrée (IMP-0240, IMP-0283) */
    int  rubout;                       /* RUB-OUT vaut X-OFF (IMP-0238) */
    unsigned int compte;               /* arrivées depuis la dernière interrogation */
    unsigned int nb_avance;
    char avance[64];                   /* frappe d'avance (IMP-0239) */
} Voie;

static Voie voie = { VOIE_STDIO, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };

int console_pilote(void)
{
    return voie.genre != VOIE_STDIO;
}

int console_fin_entree(void)
{
    return voie.epuisee;
}

void console_echo(int vrai)
{
    voie.echo = vrai != 0;
}

void console_rubout(int vrai)
{
    voie.rubout = vrai != 0;
}

static Word appel(int controle, Word code, Word *valeur)
{
    DAccessRecGS d;
    d.pCount = 5;
    d.devNum = voie.dev;
    d.code = code;
    d.list = (Pointer) valeur;
    d.requestCount = 2;
    d.transferCount = 0;
    if (controle) DControlGS(&d);
    else DStatusGS(&d);
    return toolerror();
}

static void ecrire_brut(const char *t, unsigned int n)
{
    DIORecGS d;
    if (n == 0) return;
    if (voie.genre == VOIE_STDIO) {
        fwrite(t, 1, n, stdout);
        fflush(stdout);
        return;
    }
    d.pCount = 6;
    d.devNum = voie.dev;
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
   2026-11-05). Le pilote .CONSOLE seul : le port série et le repli sur stdio
   écrivent le texte tel quel (IMP-0283). */
#define TAMPON_ECRAN  96

static void ecrire_ecran(const char *t, unsigned int n)
{
    static char tampon[TAMPON_ECRAN];
    unsigned int i, k;
    char c;

    if (voie.genre != VOIE_ECRAN) { ecrire_brut(t, n); return; }
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
    if (toolerror() != 0) return;             /* Golden Gate : repli sur stdio */
    voie.genre = VOIE_ECRAN;
    voie.dev = dn.devNum;
    appel(0, 0x8005, &voie.mode_initial);     /* GetReadMode */
    appel(0, 0x0002, &voie.attente_initiale); /* GetWaitStatus */
    appel(1, 0x8003, &brut);                  /* SetReadMode : brut */
    voie.attente_courante = voie.attente_initiale;
    ecrire_brut(mouvement, 2);
}

/* Port modem (IMP-0283) : le premier périphérique de caractères du slot 2,
   trouvé par DInfo (son nom, .DEV20 sur l'Apple IIGS, .DEV33 sous GSSquared,
   n'est pas stable), ouvert par OpenGS sous son nom : sans ouverture, DRead
   et DWrite rendent $0023. Le pilote engendré n'admet que l'attente. Sans
   pilote .CONSOLE (Golden Gate, qui n'a pas DInfo), rien n'est essayé. */
int console_ouvrir_serie(void)
{
    static GSString32 console = { 8, ".CONSOLE" };
    static ResultBuf32 nom;
    static GSString255 chemin;
    DevNumRecGS dn;
    DInfoRecGS di;
    OpenRecGS o;
    Word d;

    dn.pCount = 2;
    dn.devName = &console;
    dn.devNum = 0;
    GetDevNumberGS(&dn);
    if (toolerror() != 0) return 0;
    for (d = 1; d != 0; d++) {
        memset(&di, 0, sizeof di);
        di.pCount = 8;
        di.devNum = d;
        nom.bufSize = sizeof nom;
        nom.bufString.length = 0;
        di.devName = &nom;
        DInfoGS(&di);
        if (toolerror() != 0) return 0;       /* fin de la liste, ou pas de DInfo */
        if ((di.characteristics & 0x0080) == 0 && (di.slotNum & 0x0007) == 2) break;
    }
    if (d == 0) return 0;
    chemin.length = nom.bufString.length;
    memcpy(chemin.text, nom.bufString.text, chemin.length);
    o.pCount = 2;
    o.refNum = 0;
    o.pathname = &chemin;
    OpenGS(&o);
    if (toolerror() != 0) return 0;
    voie.genre = VOIE_SERIE;
    voie.dev = d;
    voie.ref = o.refNum;
    voie.compte = 0;
    return 1;
}

/* Fin de l'entrée : rendue comme ESC, ce qui défait la lecture en cours ; le
   moniteur clôt ensuite la session (IMP-0240, IMP-0283). */
static int fin_entree(void)
{
    voie.epuisee = 1;
    return CAR_ESC;
}

/* Un caractère lu au clavier ou au port, sans passer par la frappe d'avance ;
   -1 si attendre est faux et qu'aucun caractère n'attend. Avec stdio, la
   lecture attend toujours. */
static int lire_clavier(int attendre)
{
    DIORecGS d;
    char c;
    Word voulu;
    Word mot;
    int x;

    if (voie.genre == VOIE_STDIO) {
        if (!attendre) return -1;
        x = getchar();
        if (x == EOF) return fin_entree();
        return x == '\n' ? CAR_CR : x;
    }
    if (voie.genre == VOIE_SERIE) {
        if (voie.epuisee) return attendre ? CAR_ESC : -1;
        if (!attendre) {                      /* bit 5 du mot d'état (IMP-0283) */
            mot = 0;
            if (appel(0, 0x0000, &mot) != 0) return -1;
            if ((mot & BIT_RECU) == 0) return -1;
        }
    } else {
        voulu = attendre ? 0 : SANS_ATTENTE;
        if (voulu != voie.attente_courante) {
            appel(1, 0x0004, &voulu);         /* SetWaitStatus */
            voie.attente_courante = voulu;
        }
    }
    d.pCount = 6;
    d.devNum = voie.dev;
    d.buffer = (Pointer) &c;
    d.requestCount = 1;
    d.startingBlock = 0;
    d.blockSize = 0;
    d.transferCount = 0;
    DReadGS(&d);
    if (voie.genre == VOIE_SERIE && toolerror() != 0) {   /* liaison perdue */
        if (!attendre) return -1;
        return fin_entree();
    }
    if (d.transferCount == 0) return -1;
    return (unsigned char) c & 0x7F;
}

/* Un caractère : d'abord la frappe d'avance, puis le clavier. */
static int lire_car(int attendre)
{
    unsigned int i;
    int x;

    if (voie.nb_avance > 0) {
        x = (unsigned char) voie.avance[0];
        for (i = 1; i < voie.nb_avance; i++) voie.avance[i - 1] = voie.avance[i];
        voie.nb_avance--;
        return x;
    }
    return lire_clavier(attendre);
}

/* Curseur : pavé fixe, espace en vidéo inverse, la lecture brute n'en
   affichant pas (GS/OS Reference, chap. 9 : $0F inverse, $0E normal,
   $08 retour arrière). Il est écrasé par l'écho du caractère suivant.
   À l'écran seulement : un terminal série a son propre curseur. */
static void curseur_afficher(void)
{
    static const char pave[4] = { 0x0F, ' ', 0x0E, 0x08 };
    if (voie.genre == VOIE_ECRAN) ecrire_brut(pave, 4);
}

static void curseur_effacer(void)
{
    static const char blanc[2] = { ' ', 0x08 };
    if (voie.genre == VOIE_ECRAN) ecrire_brut(blanc, 2);
}

void console_ecrire(const char *texte, unsigned int n)
{
    ecrire_ecran(texte, n);
}

void console_retour_chariot(void)
{
    static const char cr[1] = { CAR_CR };
    if (voie.genre == VOIE_STDIO) return;   /* stdio : CR y passe à la ligne ; l'interligne suit */
    ecrire_brut(cr, 1);
}

/* L'interligne seul passe au début de la ligne suivante (IMP-0220). */
void console_interligne(void)
{
    static const char crlf[2] = { CAR_CR, CAR_LF };
    if (voie.genre == VOIE_STDIO) { ecrire_brut("\n", 1); return; }
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
        if (c == CAR_RUBOUT && voie.rubout) c = CAR_XOFF;
        if (c == CAR_CR || c == CAR_XOFF || c == CAR_ESC) {
            curseur_effacer();
            *fin = c == CAR_CR ? SAISIE_RETURN : c == CAR_XOFF ? SAISIE_XOFF : SAISIE_ESC;
            if (c == CAR_CR && voie.genre != VOIE_STDIO) console_interligne();   /* écho de RETURN */
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
        if (voie.genre != VOIE_STDIO && voie.echo) {     /* écho ; stdio : le terminal */
            e = (char) c;
            ecrire_ecran(&e, 1);
            curseur_afficher();
        }
    }
}

/* Registre du clavier de l'Apple IIGS : bit 7 à 1 tant qu'une touche frappée
   n'a pas été lue par le pilote (IMP-0242). Lecture seule ; $C010 n'est
   jamais touché à l'écran ; à la fin d'une session série, il est écrit pour
   jeter la touche ignorée (IMP-0283). Le port série est interrogé une fois sur SERIE_PERIODE
   (IMP-0283) : un GetDeviceStatus coûte 0,6 ms sur l'Apple IIGS. Le clavier
   du IIGS n'est pas lu pendant une session série. */
#define CLAVIER ((volatile unsigned char *) 0xE0C000L)
#define STROBE  ((volatile unsigned char *) 0xE0C010L)   /* fin de session série seulement */

int console_interruption(void)
{
    int c;
    if (voie.genre == VOIE_ECRAN && (*CLAVIER & 0x80) == 0) return 0;   /* aucune touche */
    if (voie.genre == VOIE_SERIE) {
        if (++voie.compte < SERIE_PERIODE) return 0;
        voie.compte = 0;
    }
    while ((c = lire_clavier(0)) >= 0) {
        if (c == CAR_ESC) {
            voie.nb_avance = 0;               /* la frappe d'avance est abandonnée */
            return 1;
        }
        if (voie.nb_avance < sizeof voie.avance) voie.avance[voie.nb_avance++] = (char) c;
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
    RefNumRecGS r;

    if (voie.genre == VOIE_SERIE) {                          /* IMP-0283 */
        r.pCount = 1;
        r.refNum = voie.ref;
        CloseGS(&r);
        *STROBE = 0;               /* touche frappée au clavier du IIGS pendant la
                                      session, jetée : le shell la prendrait pour
                                      une pause de sa sortie (essai 5.11) */
        voie.genre = VOIE_STDIO;
        voie.dev = 0;
        voie.ref = 0;
        return;
    }
    if (voie.genre != VOIE_ECRAN) return;
    e.pCount = 6;
    e.devNum = voie.dev;
    e.buffer = (Pointer) mouvement;
    e.requestCount = 2;
    e.startingBlock = 0;
    e.blockSize = 0;
    e.transferCount = 0;
    DWriteGS(&e);
    d.pCount = 5;
    d.devNum = voie.dev;
    d.requestCount = 2;
    d.transferCount = 0;
    d.code = 0x0004;                                         /* SetWaitStatus */
    d.list = (Pointer) &voie.attente_initiale;
    DControlGS(&d);
    d.code = 0x8003;                                         /* SetReadMode */
    d.list = (Pointer) &voie.mode_initial;
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
