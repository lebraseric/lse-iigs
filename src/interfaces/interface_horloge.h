/* interface_horloge.h — interface entre le noyau et l'horloge
   EXI: EXI-0011, EXI-0027 (fonctions TEM, DAT, ALE évaluées par l'interpréteur)
   VM: services d'entrée-sortie (Tome III)
   IMP: IMP-0136 (séparation noyau / plateforme), IMP-0223
   Tests: tests/bouchons/horloge_bouchon.c (réalisation de test),
          tests/plateforme/plateforme_horloge.c
   Spécification : chapitre Interpréteur §4 ter (IMP-0223). */

#ifndef LSE_INTERFACE_HORLOGE_H
#define LSE_INTERFACE_HORLOGE_H

typedef struct {
    unsigned int  annee;     /* année complète, par exemple 1972 */
    unsigned char mois;      /* 1 à 12 */
    unsigned char jour;      /* 1 à 31 */
    unsigned int  heure;     /* 0 à 23 ; davantage après horloge_fixer */
    unsigned char minute;    /* 0 à 59 */
    unsigned char seconde;   /* 0 à 59 */
} InstantHorloge;

/* Date et heure courantes. */
void horloge_lire(InstantHorloge *instant);

/* Horloge du système (IMP-0279) : à partir de cet appel, horloge_lire rend la
   date donnée, telle quelle, et l'heure donnée augmentée du temps écoulé, sans
   repasser à zéro à 24 h. */
void horloge_fixer(const InstantHorloge *instant);

#endif
