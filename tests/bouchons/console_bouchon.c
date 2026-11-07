/* console_bouchon.c — console de test : enregistre l'affichage, fournit les
   réponses
   IMP: IMP-0192 (bouchons dans tests/bouchons/), IMP-0220
   Jamais lié dans LSE. L'affichage est noté en clair, le retour chariot
   « \r », l'interligne « \n », le bip « \a ». Une réponse terminée par le
   caractère 0x13 est validée par X-OFF (sans écho de fin), par 0x1B par ESC,
   par 0x11 par X-ON (lecteur de ruban) ; sinon par RETURN (écho « \r\n »). */

#pragma noroot

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/interfaces/interface_console.h"

segment "suite";

char         bouchon_affichage[2048];
unsigned int bouchon_lg = 0;
static const char *const *reponses = 0;
static unsigned int prochaine = 0;
static unsigned int rang_interruption = 0;   /* 0 : jamais */
static int echo = 1;
static unsigned int nb_consultations = 0;

static void noter(const char *t, unsigned int n)
{
    unsigned int i;
    for (i = 0; i < n && bouchon_lg < sizeof bouchon_affichage - 1; i++)
        bouchon_affichage[bouchon_lg++] = t[i];
    bouchon_affichage[bouchon_lg] = '\0';
}

/* Vide l'affichage et fixe les réponses à fournir (liste terminée par 0). */
void bouchon_console(const char *const *lignes)
{
    bouchon_lg = 0;
    bouchon_affichage[0] = '\0';
    reponses = lignes;
    prochaine = 0;
    rang_interruption = 0;
    nb_consultations = 0;
    echo = 1;
}

void console_echo(int vrai)
{
    echo = vrai != 0;
}

/* ESC est vue à la rang-ième consultation de console_interruption (1 : la
   première) ; 0 : jamais. */
void bouchon_interruption(unsigned int rang)
{
    rang_interruption = rang;
    nb_consultations = 0;
}

int console_interruption(void)
{
    nb_consultations++;
    return rang_interruption != 0 && nb_consultations == rang_interruption;
}

int console_fin_entree(void) { return 0; }

void console_ecrire(const char *texte, unsigned int n) { noter(texte, n); }
void console_retour_chariot(void) { noter("\r", 1); }
void console_interligne(void) { noter("\n", 1); }
void console_bip(void) { noter("\a", 1); }

unsigned int console_lire_ligne(char *tampon, unsigned int max, unsigned char *fin)
{
    unsigned int n;
    char d;
    if (reponses == 0 || reponses[prochaine] == 0) {
        printf("  bouchon console : plus de reponse a fournir\n");   /* évite une attente sans fin */
        exit(99);
    }
    n = (unsigned int) strlen(reponses[prochaine]);
    d = n > 0 ? reponses[prochaine][n - 1] : 0;
    *fin = SAISIE_RETURN;
    if (d == 0x13) { *fin = SAISIE_XOFF; n--; }
    else if (d == 0x1B) { *fin = SAISIE_ESC; n--; }
    else if (d == 0x11) { *fin = SAISIE_XON; n--; }
    if (n > max) n = max;
    memcpy(tampon, reponses[prochaine], n);
    prochaine++;
    if (echo) noter(tampon, n);          /* écho de la saisie */
    if (*fin == SAISIE_RETURN) noter("\r\n", 2);
    return n;
}
