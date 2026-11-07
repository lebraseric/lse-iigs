/* codage.h — DECODER et ENCODER : programme courant et fichier-données
   EXI: EXI-0057
   VM: VM-3404, VM-2203
   IV: IV-IF-0004 (décompiler), IV-IF-0005, IV-IF-0006
   IMP: IMP-0259 (format des enregistrements), IMP-0260 (DECODER), IMP-0261
        (ENCODER)
   Tests: tests/composants/composants_codage.c, tests/composants/composants_moniteur.c
   Spécification : chapitre Bibliothécaire et fichiers §7 quinquies. */

#ifndef LSE_CODAGE_H
#define LSE_CODAGE_H

#include "../../src/noyau/erreurs.h"

#define CODAGE_ENREGISTREMENT 252u   /* caractères par enregistrement (IMP-0259) */
#define CODAGE_FIN_LIGNE      0x13   /* X-OFF */
#define CODAGE_FIN_FICHIER    0x00   /* NUL */

/* DECODER : range les lignes n1 à n2 (n2 nul : jusqu'à la fin) du programme
   courant dans le fichier temporaire nom, ou sur l'unité si unite est vrai
   (« $ », IMP-0278), remplacé s'il existe. Erreurs : EX 52 (espace
   temporaire), F 27 ; unité : F 08, F 22, F 29. */
NatureErreur codage_decoder(const char *nom, unsigned char n1, unsigned char n2, int unite);

/* ENCODER : lecture ligne à ligne d'un fichier décodé, temporaire d'abord,
   puis permanent, ou sur l'unité seule si unite est vrai. Erreurs
   d'ouverture : F 05, F 06 (protection L d'un autre compte), F 27 ; unité
   absente : F 29. */
NatureErreur codage_ouvrir(const char *nom, int unite);
/* Ligne suivante (sans son séparateur) dans texte, au plus max caractères ;
   1 si une ligne est rendue, 0 à la fin du fichier ou sur erreur (*e :
   F 101 pour un enregistrement qui n'est pas une chaîne). */
int          codage_ligne(char *texte, unsigned int max, unsigned int *n, NatureErreur *e);
void         codage_fermer(void);

#endif
