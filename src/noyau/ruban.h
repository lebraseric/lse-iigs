/* ruban.h — perforateur et lecteur de ruban, réalisés par un fichier texte
   EXI: EXI-0028 (dialogue)
   VM: VM-2201
   IMP: IMP-0262 (ruban), IMP-0263 (lecture), IMP-0264 (PERFORER), IMP-0274
        (perforateur piloté par programme), IMP-0275 (ordres à la télétype)
   Tests: tests/composants/composants_moniteur.c,
          tests/composants/composants_entrees_sorties.c
   Spécification : chapitre Bibliothécaire et fichiers §7 sexies ; INRDP75
   §4-1, §4-2, annexe II §3 ; LSE15 §9-2. */

#ifndef LSE_RUBAN_H
#define LSE_RUBAN_H

#include "../../src/noyau/erreurs.h"

#define RUBAN_LIGNE  1   /* une ligne lue */
#define RUBAN_FIN    0   /* fin du ruban : le lecteur s'arrête, le ruban revient au début */
#define RUBAN_ESC    2   /* ESC perforé sur le ruban : le lecteur s'arrête */

/* Lecteur en marche ou arrêté. */
void         ruban_lecteur(int marche);
int          ruban_en_marche(void);
/* Lit jusqu'au prochain CR (*fin = SAISIE_RETURN) ou X-OFF (SAISIE_XOFF) ;
   LF et NUL sont ignorés. */
int          ruban_lire_ligne(char *texte, unsigned int max, unsigned int *n, unsigned char *fin);

/* PERFORER : chaque ligne est ajoutée à la fin du ruban, suivie de CR.
   Erreur : F 27. */
NatureErreur ruban_perforer_ouvrir(void);
NatureErreur ruban_perforer(const char *texte, unsigned int n);
void         ruban_perforer_fermer(void);

/* Sortie d'un programme (IMP-0274, IMP-0275). EQC(18) met le perforateur en
   marche, EQC(20) l'arrête ; en marche, le reste du texte est ajouté au ruban,
   tel quel. EQC(17) à EQC(21) ne vont pas à la console, le reste y va.
   Renvoie le nombre de caractères écrits à la console. */
unsigned int ruban_sortie(const char *texte, unsigned int n);
/* CR ou LF d'une commande d'AFFICHER : perforé si le perforateur est en marche. */
void         ruban_sortie_commande(char c);
/* 1 si un ajout au ruban a échoué depuis la dernière consultation ; le
   perforateur est alors arrêté. */
int          ruban_sortie_en_echec(void);
/* Arrête le perforateur, à chaque retour au moniteur. */
void         ruban_perforateur_arreter(void);

#endif
