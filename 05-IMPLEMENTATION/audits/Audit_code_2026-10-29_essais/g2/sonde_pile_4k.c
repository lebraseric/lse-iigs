/* sonde_pile_4k.c — essai d'audit G2 (2026-10-29), hors dépôt : pile par
   défaut (aucun #pragma stacksize, comme src/plateforme/main.c) ; compile
   des lignes de plus en plus imbriquées jusqu'au débordement éventuel. */

#ifdef PILE_4096
#pragma stacksize 4096
#endif
#include "compilateur_outils.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

static char texte[100];

static void parentheses(const char *tete, unsigned int n, const char *coeur)
{
    unsigned int i;
    strcpy(texte, tete);
    for (i = 0; i < n; i++) strcat(texte, "(");
    strcat(texte, coeur);
    for (i = 0; i < n; i++) strcat(texte, ")");
}

int main(void)
{
    unsigned int s;
    unsigned int n;
    int ok;

    asm {
        tsc
        sta s
    }
    printf("SP a l'entree de main : $%04X\n", s);
    for (n = 1; n <= 38; n++) {
        parentheses(SONDE_TETE, n, "1");
        printf("n = %2u : ", n);
        fflush(stdout);
        symboles_vider();
        ok = compiler(texte);
        if (ok) recuperer();
        printf("%s\n", ok ? "accepte" : "rejete");
        fflush(stdout);
    }
    printf("fin sans debordement\n");
    return 0;
}
