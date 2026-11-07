/* plateforme_horloge.c — horloge réelle de la plateforme
   IMP: IMP-0223
   Niveau : sur cible (IIGS ou Golden Gate). Vérifie que horloge_lire rend
   un instant plausible ; l'instant lu est affiché pour contrôle visuel. */

#include "../commun/verif.h"
#include "../../src/interfaces/interface_horloge.h"

int main(void)
{
    InstantHorloge h;

    VERIF_DEBUT("plateforme/horloge");
    horloge_lire(&h);
    printf("  instant lu : %02u/%02u/%u %02u:%02u:%02u\n",
           (unsigned int) h.jour, (unsigned int) h.mois, h.annee,
           (unsigned int) h.heure, (unsigned int) h.minute, (unsigned int) h.seconde);
    VERIFIER(h.annee >= 1980 && h.annee <= 2105);
    VERIFIER(h.mois >= 1 && h.mois <= 12);
    VERIFIER(h.jour >= 1 && h.jour <= 31);
    VERIFIER(h.heure <= 23 && h.minute <= 59 && h.seconde <= 59);
    {                                             /* horloge du système (IMP-0279) */
        static const InstantHorloge nbor = { 0, 0, 0, 28, 26, 42 };
        horloge_fixer(&nbor);
        horloge_lire(&h);
        VERIFIER(h.annee == 0 && h.mois == 0 && h.jour == 0);
        VERIFIER(h.heure == 28 && h.minute >= 26 && h.seconde <= 59);
    }
    VERIF_FIN();
}
