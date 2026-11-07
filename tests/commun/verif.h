/* verif.h — macros de vérification des suites de tests
   IMP: IMP-0192 (suites sous Golden Gate ; nombre d'échecs = code de sortie)

   Emploi, dans le seul fichier d'une suite (celui qui contient main) :

       #include "../commun/verif.h"
       int main(void)
       {
           VERIF_DEBUT("nom de la suite");
           VERIFIER(expression);
           VERIFIER_EGAL(attendu, obtenu);
           VERIF_FIN();
       }

   stdio.h n'est employé que dans tests/, jamais dans src/noyau/.
   Les valeurs comparées par VERIFIER_EGAL sont converties en long. */

#ifndef LSE_VERIF_H
#define LSE_VERIF_H

#include <stdio.h>

static unsigned int verif_nb_controles = 0;
static unsigned int verif_nb_echecs = 0;
static const char *verif_nom_suite = "";

#define VERIF_DEBUT(nom)                                                   \
    do {                                                                   \
        verif_nom_suite = (nom);                                           \
        printf("SUITE %s\n", verif_nom_suite);                             \
    } while (0)

#define VERIFIER(expr)                                                     \
    do {                                                                   \
        verif_nb_controles++;                                              \
        if (!(expr)) {                                                     \
            verif_nb_echecs++;                                             \
            printf("  ECHEC %s:%d : %s\n", __FILE__, __LINE__, #expr);     \
        }                                                                  \
    } while (0)

#define VERIFIER_EGAL(attendu, obtenu)                                     \
    do {                                                                   \
        long verif_a = (long) (attendu);                                   \
        long verif_o = (long) (obtenu);                                    \
        verif_nb_controles++;                                              \
        if (verif_a != verif_o) {                                          \
            verif_nb_echecs++;                                             \
            printf("  ECHEC %s:%d : %s = %ld, attendu %ld\n",              \
                   __FILE__, __LINE__, #obtenu, verif_o, verif_a);         \
        }                                                                  \
    } while (0)

#define VERIF_FIN()                                                        \
    do {                                                                   \
        printf("SUITE %s : %u controle(s), %u echec(s)\n",                 \
               verif_nom_suite, verif_nb_controles, verif_nb_echecs);      \
        return (int) (verif_nb_echecs > 255 ? 255 : verif_nb_echecs);      \
    } while (0)

#endif
