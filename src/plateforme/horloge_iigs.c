/* horloge_iigs.c — horloge de l'Apple IIGS, par la bibliothèque d'ORCA/C
   IMP: IMP-0136 (adaptation isolée dans src/plateforme/), IMP-0223, IMP-0279
   Tests: tests/plateforme/plateforme_horloge.c
   time() et localtime() d'ORCA/C lisent l'horloge du IIGS (ReadTimeHex,
   cc.notes.txt n° 231) ; aucun appel direct à la Toolbox. Segment "interp_es", celui de
   l'appelant (Construction §1). */

#pragma noroot

#include <time.h>
#include "../interfaces/interface_horloge.h"
#include "../interfaces/profil.h"

segment "interp_es";

static int            fixee = 0;
static InstantHorloge depart;
static time_t         origine;

void horloge_fixer(const InstantHorloge *instant)
{
    depart = *instant;
    origine = time(NULL);
    fixee = 1;
}

void horloge_lire(InstantHorloge *instant)
{
    time_t t;
    struct tm *d;
    unsigned long s;

    t = time(NULL);
    if (fixee) {
        s = (unsigned long) (t - origine) + (unsigned long) depart.seconde
            + 60ul * depart.minute + 3600ul * depart.heure;
        *instant = depart;
        instant->heure   = (unsigned int) (s / 3600ul);
        instant->minute  = (unsigned char) (s / 60ul % 60ul);
        instant->seconde = (unsigned char) (s % 60ul);
        return;
    }
    d = localtime(&t);
    instant->annee   = (unsigned int) d->tm_year + 1900u;
    instant->mois    = (unsigned char) (d->tm_mon + 1);
    instant->jour    = (unsigned char) d->tm_mday;
    instant->heure   = (unsigned int) d->tm_hour;
    instant->minute  = (unsigned char) d->tm_min;
    instant->seconde = (unsigned char) d->tm_sec;
}
