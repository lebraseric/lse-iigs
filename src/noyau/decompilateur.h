/* decompilateur.h — restitution textuelle d'une ligne compilée
   EXI: EXI-0003, EXI-0010, EXI-0015, EXI-0017, EXI-0022
   VM: VM-2208, VM-1801, VM-0406
   IV: IV-IF-0004, ALG-1001, ANN-K-0014, ANN-K-0031
   IMP: IMP-0183 (forme restituée), IMP-0198 à IMP-0203
   Tests: tests/composants/composants_decompilateur.c,
          tests/historiques/historiques_decompilation.c
   Spécification : chapitre Décompilateur §3. */

#ifndef LSE_DECOMPILATEUR_H
#define LSE_DECOMPILATEUR_H

#include "../../src/noyau/rcim.h"

#define DECOMPILATEUR_LONGUEUR_MAX 255u   /* caractères, hors zéro final (IMP-0198) */

typedef unsigned char ResultatDecompilation;   /* IMP-0200 */
#define DECOMP_OK              0
#define DECOMP_ERR_TROP_LONGUE 1   /* texte canonique de plus de 255 caractères */
#define DECOMP_ERR_SEQUENCE    2   /* séquence impossible à décoder */
#define DECOMP_ERR_REFERENCE   3   /* identifiant de symbole ou index de constante inconnu */

/* IV-IF-0004 / VM-2208 (IMP-0198). texte : tableau d'au moins
   DECOMPILATEUR_LONGUEUR_MAX + 1 caractères. Succès : DECOMP_OK, texte reçoit
   « numéro espace instructions », ou « numéro*texte » pour une ligne de
   commentaire, terminé par un zéro, *longueur sa longueur.
   Échec : texte vide, *longueur nul. Ni la ligne ni les tables ne sont
   modifiées. Le profil de la session fixe la graphie de TEM et DAT. */
ResultatDecompilation decompilateur_decompiler(const EntreeObjetCompile *ligne,
                                               char *texte, unsigned int *longueur);

/* Écriture d'un réel selon IMP-0183 (IMP-0201) : six chiffres significatifs
   au plus. sortie : au moins 13 caractères ; renvoie la longueur écrite,
   zéro final non compris. */
unsigned int decompilateur_ecrire_reel(float valeur, char *sortie);
/* Même écriture avec 1 à 7 chiffres significatifs : 7 pour U et CCA
   (Données IMP-0211). */
unsigned int decompilateur_ecrire_reel_chiffres(float valeur, unsigned int chiffres, char *sortie);

#endif
