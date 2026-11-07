/* fichiers_iigs.h — réalisation Apple IIGS de interface_fichiers.h
   IMP: IMP-0136, IMP-0249, IMP-0257
   Les fonctions sont déclarées par l'interface ; cet en-tête ajoute la
   permission de l'appel Volume, propre à la plateforme. */

#ifndef LSE_FICHIERS_IIGS_H
#define LSE_FICHIERS_IIGS_H

#include "../../src/interfaces/interface_fichiers.h"

/* Permet plateforme_secteurs_libres d'interroger GS/OS (appel Volume) ;
   sans permission, elle rend 0 (inconnu). */
void plateforme_volume_permettre(int vrai);

#endif
