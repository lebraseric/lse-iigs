/* profil.h — sélection du profil de compilation
   EXI: —  VM: —  IV: —
   IMP: IMP-0143 (deux profils de compilation, dev et cible)
   Tests: Construction §11

   LSE_PROFIL_DEV est défini par build/Makefile (cc=-dLSE_PROFIL_DEV=1)
   lorsque PROFIL=dev. Tout exécutable destiné à la cible ou à une
   livraison est construit avec PROFIL=cible. */

#ifndef LSE_PROFIL_H
#define LSE_PROFIL_H

#ifdef LSE_PROFIL_DEV
#pragma debug 43   /* bits 0+1+3+5 : + débogage source-level (Golden Gate -g) */
#else
#pragma debug 1    /* bit 0 : contrôle du débordement de pile */
#pragma optimize 7 /* bits 0+1+2 : code intermédiaire, judas 65816, registres */
#endif

#endif
