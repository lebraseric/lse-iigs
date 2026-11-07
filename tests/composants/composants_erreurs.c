/* composants_erreurs.c — catalogues des natures de compilation et
   d'exécution, messages
   EXI: EXI-0019, EXI-0051  VM: VM-2210  IV: Tome IV §3.3, §17
   IMP: IMP-0185, IMP-0186, IMP-0187, IMP-0195
   Spécification des tests : chapitre Gestionnaire des erreurs §3, §4, §5, §7, §8, §9.
   Niveau : composants. Liée avec src/noyau/erreurs.c. */

#include <string.h>
#include "../commun/verif.h"
#include "../../src/interfaces/profil.h"
#include "../../src/noyau/erreurs.h"

int main(void)
{
    char message[100];
    char court[12];
    NatureErreur n;
    NatureErreur m;
    int classes_ok = 1;
    int libelles_ok = 1;
    int numeros_ok = 1;
    int nouveaux_distincts = 1;
    int longueur_ok = 1;

    VERIF_DEBUT("composants/erreurs");

    VERIFIER_EGAL(175, ERR_NB_NATURES);
    VERIFIER_EGAL(84, ERR_C_CAPACITE_TABLES);       /* compilation : 1 à 84 */
    VERIFIER_EGAL(85, ERR_E_NON_DEFINI_CALCUL);     /* exécution : 85 à 148 */
    VERIFIER_EGAL(149, ERR_D_COMMANDE_ERRONEE);     /* dialogue : 149 à 155 */
    VERIFIER_EGAL(156, ERR_F_SUPPORT_PLEIN);        /* fichier : 156 à 175 */

    /* Cohérence des catalogues (§7, §9) */
    for (n = 1; n <= ERR_NB_NATURES; n++) {
        char classe = erreurs_classe(n);
        if (classe != (n <= 84 ? 'C' : n <= 148 ? 'E' : n <= 155 ? 'D' : 'F')) classes_ok = 0;
        if (erreurs_libelle(n) == NULL || erreurs_libelle(n)[0] == '\0') libelles_ok = 0;
        if (classe == 'D' ? erreurs_numero(n) != 0
                          : erreurs_numero(n) < 1
                            || erreurs_numero(n) > (classe == 'C' ? 109u : classe == 'F' ? 102u : 105u))
            numeros_ok = 0;
        /* un numéro à partir de 100 n'est partagé par aucune autre nature de la classe (IMP-0187) */
        if (erreurs_numero(n) >= 100)
            for (m = 1; m <= ERR_NB_NATURES; m++)
                if (m != n && erreurs_classe(m) == classe && erreurs_numero(m) == erreurs_numero(n))
                    nouveaux_distincts = 0;
        /* tout message avec libellé tient sur 80 colonnes (IMP-0195) */
        if (classe == 'C') erreurs_formater(n, 1, message, sizeof message);
        else if (classe == 'D') erreurs_formater_dialogue(n, 1, message, sizeof message);
        else if (classe == 'F') erreurs_formater_fichier(n, 1, 123, 1, message, sizeof message);
        else erreurs_formater_execution(n, 123, 1, message, sizeof message);
        if (strlen(message) > 80) longueur_ok = 0;
    }
    VERIFIER(classes_ok);
    VERIFIER(libelles_ok);
    VERIFIER(numeros_ok);
    VERIFIER(nouveaux_distincts);
    VERIFIER(longueur_ok);

    /* Dialogue (§9 bis, Moniteur IMP-0235) */
    VERIFIER(erreurs_formater_dialogue(ERR_D_CONTINUER_IMPOSSIBLE, 0, message, sizeof message)
             && strcmp(message, "????") == 0);
    VERIFIER(erreurs_formater_dialogue(ERR_D_COMMANDE_ERRONEE, 1, message, sizeof message)
             && strcmp(message, "???? : COMMANDE ERRONEE") == 0);
    VERIFIER(!erreurs_formater_dialogue(ERR_E_INTERNE, 0, message, sizeof message));

    /* Fichier (§9 ter, IMP-0250) */
    VERIFIER(erreurs_formater_fichier(ERR_F_PROGRAMME_EXISTANT, 0, 0, 0, message, sizeof message)
             && strcmp(message, "ERREUR F 03") == 0);                    /* Victor Duruy */
    VERIFIER(erreurs_formater_fichier(ERR_F_DROIT_DONNEES, 1, 12, 0, message, sizeof message)
             && strcmp(message, "ERREUR F 08 EN LIGNE 012") == 0);
    VERIFIER(erreurs_formater_fichier(ERR_F_AUTRE_PROFIL, 1, 0, 1, message, sizeof message)
             && strcmp(message, "ERREUR F 100 EN LIGNE BUR : FICHIER RANGE SOUS UN AUTRE PROFIL") == 0);
    VERIFIER(!erreurs_formater_fichier(ERR_E_INTERNE, 0, 0, 0, message, sizeof message));

    /* Nature inconnue */
    VERIFIER_EGAL(0, erreurs_classe(ERR_AUCUNE));
    VERIFIER_EGAL(0, erreurs_numero(ERR_NB_NATURES + 1));
    VERIFIER(erreurs_libelle(ERR_AUCUNE) == NULL);
    VERIFIER(!erreurs_formater(ERR_AUCUNE, 0, message, sizeof message));
    VERIFIER_EGAL(0, message[0]);

    /* Numéros attestés (§2.2) */
    VERIFIER_EGAL(3, erreurs_numero(ERR_C_IDENTIFICATEUR_TROP_LONG));
    VERIFIER_EGAL(8, erreurs_numero(ERR_C_CHAINE_NON_TERMINEE));
    VERIFIER_EGAL(36, erreurs_numero(ERR_C_FIN_MANQUANT));
    VERIFIER_EGAL(38, erreurs_numero(ERR_C_AFFECTATION_MANQUANTE));
    VERIFIER_EGAL(63, erreurs_numero(ERR_C_LIGNE_TROP_LONGUE));

    /* Granularité : natures distinctes, même numéro (IMP-0187) */
    VERIFIER(ERR_C_BUREAU_NUMERO_LIGNE != ERR_C_BUREAU_DECLARATION);
    VERIFIER_EGAL(58, erreurs_numero(ERR_C_BUREAU_NUMERO_LIGNE));
    VERIFIER_EGAL(58, erreurs_numero(ERR_C_BUREAU_DECLARATION));
    VERIFIER(strcmp(erreurs_libelle(ERR_C_BUREAU_NUMERO_LIGNE),
                    erreurs_libelle(ERR_C_BUREAU_DECLARATION)) != 0);

    /* Forme des messages (IMP-0186) */
    erreurs_formater(ERR_C_AFFECTATION_MANQUANTE, 0, message, sizeof message);
    VERIFIER(strcmp(message, "ERREUR  C 38") == 0);
    erreurs_formater(ERR_C_IDENTIFICATEUR_TROP_LONG, 0, message, sizeof message);
    VERIFIER(strcmp(message, "ERREUR  C 03") == 0);
    erreurs_formater(ERR_C_MOT_RESERVE, 0, message, sizeof message);
    VERIFIER(strcmp(message, "ERREUR  C 101") == 0);
    erreurs_formater(ERR_C_AFFECTATION_MANQUANTE, 1, message, sizeof message);
    VERIFIER(strcmp(message, "ERREUR  C 38 : INSTRUCTION INCONNUE OU _ MANQUANT") == 0);

    /* Messages d'exécution (IMP-0186) et attestations (§2.2, §9) */
    VERIFIER_EGAL(30, erreurs_numero(ERR_E_EQC_HORS_BORNES));
    VERIFIER_EGAL(3, erreurs_numero(ERR_E_MEMOIRE_SATUREE));
    VERIFIER_EGAL(10, erreurs_numero(ERR_E_NON_DEFINI_COMPARAISON));
    erreurs_formater_execution(ERR_E_EQC_HORS_BORNES, 0, 0, message, sizeof message);
    VERIFIER(strcmp(message, "ERREUR (EX.) 30 EN LIGNE BUR") == 0);
    erreurs_formater_execution(ERR_E_NON_DEFINI_COMPARAISON, 11, 0, message, sizeof message);
    VERIFIER(strcmp(message, "ERREUR (EX.) 10 EN LIGNE 011") == 0);
    erreurs_formater_execution(ERR_E_MEMOIRE_SATUREE, 5, 0, message, sizeof message);
    VERIFIER(strcmp(message, "ERREUR (EX.) 03 EN LIGNE 005") == 0);
    erreurs_formater_execution(ERR_E_MODULE_INTROUVABLE, 250, 1, message, sizeof message);
    VERIFIER(strcmp(message, "ERREUR (EX.) 105 EN LIGNE 250 : MODULE INTROUVABLE POUR EXECUTER") == 0);
    /* une nature n'a qu'une forme de message */
    VERIFIER(!erreurs_formater(ERR_E_DIVISION_PAR_ZERO, 0, message, sizeof message));
    VERIFIER(!erreurs_formater_execution(ERR_C_MOT_RESERVE, 1, 0, message, sizeof message));
    VERIFIER_EGAL(0, message[0]);
    /* granularité : trois natures de variable non définie (RX1) */
    VERIFIER_EGAL(5, erreurs_numero(ERR_E_NON_DEFINI_CALCUL));
    VERIFIER_EGAL(62, erreurs_numero(ERR_E_NON_DEFINI_AFFECTATION));

    /* Troncature sûre */
    VERIFIER(erreurs_formater(ERR_C_AFFECTATION_MANQUANTE, 1, court, sizeof court));
    VERIFIER_EGAL(sizeof court - 1, strlen(court));
    VERIFIER(strncmp(court, "ERREUR  C 3", 11) == 0);

    VERIF_FIN();
}
