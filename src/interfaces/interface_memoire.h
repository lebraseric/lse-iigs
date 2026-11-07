/* interface_memoire.h — contrat entre noyau/objets.c et plateforme/memoire_iigs.c
   EXI: EXI-0050  VM: VM-1601, VM-1602  IV: ANN-K (gestionnaire mémoire)
   IMP: IMP-0132, IMP-0134, IMP-0136, IMP-0161, IMP-0243, IMP-0271
   Tests: tests/composants/composants_objets.c
   Spécification : Gestionnaire d'objets §11.
   Le verrouillage n'est pas compté (HLock, HUnlock) : un seul déverrouillage
   libère le bloc, quel que soit le nombre de verrouillages (§11). */

#ifndef LSE_INTERFACE_MEMOIRE_H
#define LSE_INTERFACE_MEMOIRE_H

/* Opaque pour noyau/objets.c, jamais interprété au-delà de sa transmission. */
typedef void *HandlePlateforme;

/* Bloc déplaçable, non purgeable, déverrouillé, hors de la mémoire spéciale
   (IMP-0271) ; NULL si la mémoire manque. */
HandlePlateforme plateforme_bloc_allouer(unsigned long taille_octets);
/* Rend le bloc, verrouillé ou non. */
void             plateforme_bloc_liberer(HandlePlateforme h);
/* Verrouille le bloc et renvoie son adresse, valable jusqu'au déverrouillage. */
unsigned char   *plateforme_bloc_verrouiller(HandlePlateforme h);
void             plateforme_bloc_deverrouiller(HandlePlateforme h);
/* Adresse courante du bloc, sans le verrouiller (IMP-0243) : valable tant
   qu'aucun appel système ni toolbox n'a lieu. */
unsigned char   *plateforme_bloc_adresse(HandlePlateforme h);
/* Mémoire libre selon le Memory Manager, y compris la mémoire récupérable par
   purge (RealFreeMem, IMP-0134). Indicative : seul l'échec d'une allocation
   fait foi. Non disponible sous Golden Gate (RealFreeMem non implémenté,
   IMP-0192) : ne pas appeler en test. */
unsigned long    plateforme_memoire_disponible(void);

#endif
