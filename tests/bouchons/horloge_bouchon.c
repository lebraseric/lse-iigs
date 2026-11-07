/* horloge_bouchon.c — horloge de test : instant fixé par la suite
   IMP: IMP-0192 (bouchons dans tests/bouchons/), IMP-0223
   Jamais lié dans LSE. Instant par défaut : celui de l'exemple de LSE15
   §4-4.2, 3 novembre 1972 à 13 h 3 mn 52 s. */

#pragma noroot

#include "../../src/interfaces/interface_horloge.h"

segment "suite";

static InstantHorloge courant = { 1972, 11, 3, 13, 3, 52 };

void bouchon_horloge(unsigned int annee, unsigned int mois, unsigned int jour,
                     unsigned int heure, unsigned int minute, unsigned int seconde)
{
    courant.annee   = annee;
    courant.mois    = (unsigned char) mois;
    courant.jour    = (unsigned char) jour;
    courant.heure   = heure;
    courant.minute  = (unsigned char) minute;
    courant.seconde = (unsigned char) seconde;
}

void horloge_lire(InstantHorloge *instant)
{
    *instant = courant;
}

void horloge_fixer(const InstantHorloge *instant)
{
    courant = *instant;
}
