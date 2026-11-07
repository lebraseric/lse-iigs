/* memoire_iigs.c — Memory Manager toolbox pour le gestionnaire d'objets
   EXI: EXI-0050  VM: VM-1601, VM-1602  IV: ANN-K
   IMP: IMP-0132 (allocation par le Memory Manager), IMP-0134 (mémoire
        disponible), IMP-0136 (seul répertoire autorisé à inclure MEMORY.H),
        IMP-0271 (hors de la mémoire spéciale)
   Tests: tests/composants/composants_objets.c
   Segment : "objets" (Construction §1).
   Référence : Apple IIGS Toolbox Reference, vol. 1, chap. 12. MMStartUp et
   MMShutDown sont faits par la bibliothèque d'exécution d'ORCA/C ; userid()
   rend l'identifiant maître avec auxID = 1 (Gestionnaire d'objets §11). */

#pragma noroot

#include <memory.h>
#include <orca.h>
#include "memoire_iigs.h"
#include "../interfaces/profil.h"

segment "objets";

/* Un bloc de 64 Kio au plus est alloué sans franchir de frontière de banque
   (attrNoCross) : un objet reste ainsi dans une seule banque du 65816. Au-delà
   (objets hors-norme de très grande taille), ce n'est pas possible. */
#define TAILLE_MAX_SANS_FRANCHISSEMENT 0x10000ul

/* attrNoSpec (IMP-0271) : jamais dans les banques $00, $01, $E0, $E1. */
HandlePlateforme plateforme_bloc_allouer(unsigned long taille_octets)
{
    Handle h;
    Word attributs;

    attributs = attrNoPurge | attrNoSpec;
    if (taille_octets <= TAILLE_MAX_SANS_FRANCHISSEMENT)
        attributs |= attrNoCross;
    h = NewHandle(taille_octets, (Word) userid(), attributs, NULL);
    if (toolerror() != 0)
        return NULL;
    return (HandlePlateforme) h;
}

void plateforme_bloc_liberer(HandlePlateforme h)
{
    DisposeHandle((Handle) h);
}

unsigned char *plateforme_bloc_verrouiller(HandlePlateforme h)
{
    HLock((Handle) h);
    return *(unsigned char **) h;
}

void plateforme_bloc_deverrouiller(HandlePlateforme h)
{
    HUnlock((Handle) h);
}

/* Pointeur maître, sans HLock (IMP-0243). */
unsigned char *plateforme_bloc_adresse(HandlePlateforme h)
{
    return *(unsigned char **) h;
}

/* IMP-0134 : mémoire libre, plus la mémoire récupérable par purge
   (Toolbox Reference, vol. 3, p. 36-10). */
unsigned long plateforme_memoire_disponible(void)
{
    return RealFreeMem();
}
