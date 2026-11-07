/* fichiers.h — fichiers-données : enregistrements en secteurs, catalogage
   EXI: EXI-0053 à EXI-0057
   VM: VM-3404, VM-3405, VM-3407
   IV: IV-IF-0005, IV-IF-0006
   IMP: IMP-0247 (secteurs, répertoire, étendues), IMP-0253 (GARER, CHARGER,
        SUPPRIMER, CATALOGUER, espace temporaire), IMP-0255 (droits),
        IMP-0256 (espace d'une identification)
   Tests: tests/composants/composants_fichiers.c
   Spécification : chapitre Bibliothécaire et fichiers §4, §7 ter. */

#ifndef LSE_FICHIERS_H
#define LSE_FICHIERS_H

#include "../../src/noyau/erreurs.h"

/* Type d'un enregistrement : les valeurs de δ de LSE15 §6-3.4. */
#define ENREG_NOMBRE     0
#define ENREG_TABLEAU_1  1
#define ENREG_TABLEAU_2  2
#define ENREG_CHAINE     3

#define FICHIERS_NUMERO_MAX     32767u
#define FICHIERS_QUOTA_DEFAUT   200ul   /* secteurs (point C1) */

/* Espace temporaire standard, en secteurs (option -TEMP) ; il devient aussi
   l'espace courant. */
void         fichiers_fixer_quota(unsigned long secteurs);
/* Espace temporaire courant, fixé par IDENTIFICATION (IMP-0256). */
void          fichiers_fixer_espace(unsigned long secteurs);
unsigned long fichiers_espace(void);
unsigned long fichiers_espace_standard(void);
/* Secteurs occupés par les fichiers temporaires. */
unsigned long fichiers_secteurs_temporaires(void);

/* Paramètre permanent des fonctions ci-dessous : 0, temporaire ; 1, permanent
   de la bibliothèque ; FICHIERS_UNITE, fichier de l'unité amovible, permanent
   lui aussi, avec le catalogue de l'unité (« $ », IMP-0278 ; unité absente :
   F 29). */
#define FICHIERS_UNITE 2

/* GARER : range le contenu dans l'enregistrement numero du fichier nom,
   permanent ou temporaire ; crée le fichier au besoin. Erreurs : EX 52
   (plus de place), F 27 ; permanent : F 08, F 22, et F 03 à la création sous
   le profil MITRA 15 (bibliotheque_nom_pris). */
NatureErreur fichiers_garer(const char *nom, int permanent, unsigned int numero,
                            unsigned char type, const unsigned char *contenu,
                            unsigned long longueur);

/* CHARGER : cherche d'abord le fichier temporaire, puis le permanent (seul
   le permanent si permanent est vrai) ; *contenu est alloué par malloc et
   rendu à l'appelant. Erreurs : EX 54 (permanent protégé), EX 55 (fichier
   inconnu), EX 56 (enregistrement inexistant), F 27, F 101. */
NatureErreur fichiers_charger(const char *nom, int permanent, unsigned int numero,
                              unsigned char *type, unsigned char **contenu,
                              unsigned long *longueur);

/* SUPPRIMER : l'enregistrement numero, ou le fichier entier si numero vaut
   0. Erreurs : EX 55, EX 56 ; permanent : F 08, F 22. */
NatureErreur fichiers_supprimer(const char *nom, int permanent, unsigned int numero);

/* CATALOGUER : le fichier temporaire devient permanent sous le nom donné,
   sans ses étendues libres, dans la bibliothèque ou sur l'unité si unite est
   vrai. Erreurs : F 08, F 05, F 04, F 03 (MITRA 15, bibliotheque_nom_pris),
   F 02, F 27 ; unité absente : F 29. */
NatureErreur fichiers_cataloguer(const char *temporaire, const char *permanent,
                                 unsigned char protection, int unite);

/* Nombre de secteurs du fichier, 0 s'il n'existe pas (TABLE DES FICHIERS). */
unsigned long fichiers_secteurs(int permanent, const char *nom);

#endif
