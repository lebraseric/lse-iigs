/* composants_analyseur.c — classement des entrées et table des commandes
   EXI: EXI-0052  VM: VM-2202
   IMP: IMP-0229, IMP-0230, IMP-0236
   Spécification des tests : chapitre Moniteur et console §3, §5.
   Niveau : composants. */

#include <string.h>
#include "../commun/verif.h"
#include "../../src/noyau/analyseur.h"
#include "../../src/interfaces/types_communs.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

static CategorieEntree classer(const char *t, int dormant, Commande *c)
{
    return analyseur_classer(t, (unsigned int) strlen(t), dormant, c);
}

int main(void)
{
    Commande c;

    VERIF_DEBUT("composants/analyseur");

    /* classement (IMP-0229) */
    VERIFIER(classer("", 0, &c) == ENTREE_VIDE && c == CMD_AUCUNE);
    VERIFIER(classer("10 A_1", 0, &c) == ENTREE_PROGRAMME);
    VERIFIER(classer("1*COMMENTAIRE", 0, &c) == ENTREE_PROGRAMME);
    VERIFIER(classer("10A", 0, &c) == ENTREE_PROGRAMME);           /* diagnostic du compilateur */
    VERIFIER(classer("?2+2", 0, &c) == ENTREE_BUREAU);
    VERIFIER(classer("A_1", 0, &c) == ENTREE_BUREAU);
    VERIFIER(classer("LIS", 0, &c) == ENTREE_BUREAU);               /* LAFOND83 : C 38 */
    VERIFIER(classer("LISTER A PARTIR DE 1", 0, &c) == ENTREE_BUREAU);   /* LSE 8086 */
    VERIFIER(classer("li", 0, &c) == ENTREE_BUREAU);                /* minuscules : compilateur */
    VERIFIER(classer("LI", 0, &c) == ENTREE_COMMANDE && c == CMD_LISTER);
    VERIFIER(classer("EX", 0, &c) == ENTREE_COMMANDE && c == CMD_EXECUTER);
    VERIFIER(classer("AU", 0, &c) == ENTREE_COMMANDE && c == CMD_AU_REVOIR);
    VERIFIER(classer("BO", 0, &c) == ENTREE_COMMANDE && c == CMD_BONJOUR);
    VERIFIER(classer("XY", 0, &c) == ENTREE_INCONNUE && c == CMD_AUCUNE);
    VERIFIER(classer("A1", 0, &c) == ENTREE_BUREAU);

    /* état dormant : BONJOUR et l'initialisateur (points K6, M6 ; IMP-0257) */
    VERIFIER(classer("BO", 1, &c) == ENTREE_COMMANDE && c == CMD_BONJOUR);
    VERIFIER(classer("LI", 1, &c) == ENTREE_COMMANDE && c == CMD_LISTE_ID);
    VERIFIER(classer("CR", 1, &c) == ENTREE_COMMANDE && c == CMD_CREATION_ID);
    VERIFIER(classer("SU", 1, &c) == ENTREE_COMMANDE && c == CMD_SUPPRESSION_ID);
    VERIFIER(classer("SU", 0, &c) == ENTREE_COMMANDE && c == CMD_SUPPRIMER);
    VERIFIER(classer("CR", 0, &c) == ENTREE_INCONNUE);
    VERIFIER(strcmp(analyseur_nom(CMD_CREATION_ID), "CREATION ID : ") == 0);
    VERIFIER(classer("AP", 1, &c) == ENTREE_INCONNUE);

    /* complétion selon le profil (IMP-0230) */
    VERIFIER(strcmp(analyseur_nom(CMD_LISTER), "LISTER A PARTIR DE ") == 0);
    VERIFIER(strcmp(analyseur_nom(CMD_REPRENDRE), "REPRENDRE EN ") == 0);       /* LSE15 */
    VERIFIER(classer("RU", 0, &c) == ENTREE_COMMANDE && c == CMD_RUBAN);
    VERIFIER(classer("SI", 0, &c) == ENTREE_COMMANDE && c == CMD_SILENCE);
    VERIFIER(classer("DU", 0, &c) == ENTREE_INCONNUE);
    VERIFIER(analyseur_parametres(CMD_EXECUTER) == PARAM_LIGNES);
    VERIFIER(analyseur_parametres(CMD_CONTINUER) == PARAM_AUCUN);
    VERIFIER(analyseur_parametres(CMD_EFFACER) == PARAM_LISTE);
    configuration_session.profil = PROFIL_T1600_V4;
    VERIFIER(strcmp(analyseur_nom(CMD_REPRENDRE), "REPRENDRE A PARTIR DE ") == 0);   /* INRDP75 */
    VERIFIER(classer("RU", 0, &c) == ENTREE_INCONNUE);
    VERIFIER(classer("SI", 0, &c) == ENTREE_INCONNUE);
    VERIFIER(classer("DU", 0, &c) == ENTREE_INCONNUE);                         /* absent d'INRDP75 */
    configuration_session.profil = PROFIL_T1600_F4;                           /* IMP-0276 */
    VERIFIER(strcmp(analyseur_nom(CMD_REPRENDRE), "REPRENDRE A PARTIR DE ") == 0);
    VERIFIER(classer("RU", 0, &c) == ENTREE_INCONNUE);
    VERIFIER(classer("DU", 0, &c) == ENTREE_COMMANDE && c == CMD_DUPLIQUER);   /* Victor Duruy */

    VERIF_FIN();
}
