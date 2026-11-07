/* analyseur.h — analyseur de commandes : classement d'une entrée validée
   EXI: EXI-0009, EXI-0028, EXI-0029, EXI-0052
   VM: VM-2202
   IMP: IMP-0229 (classement), IMP-0230 (table des commandes), IMP-0236
   Tests: tests/composants/composants_analyseur.c
   Spécification : chapitre Moniteur et console §3, §5. */

#ifndef LSE_ANALYSEUR_H
#define LSE_ANALYSEUR_H

typedef unsigned char CategorieEntree;
#define ENTREE_VIDE       0
#define ENTREE_PROGRAMME  1   /* commence par un chiffre */
#define ENTREE_COMMANDE   2   /* deux lettres désignant une commande du profil */
#define ENTREE_BUREAU     3   /* ligne de machine de bureau */
#define ENTREE_INCONNUE   4   /* deux lettres ne désignant aucune commande */

typedef unsigned char Commande;
#define CMD_AUCUNE          0
#define CMD_BONJOUR         1
#define CMD_AU_REVOIR       2
#define CMD_LISTER          3
#define CMD_PERFORER        4
#define CMD_NUMERO          5
#define CMD_EFFACER         6
#define CMD_EXECUTER        7
#define CMD_CONTINUER       8
#define CMD_POURSUIVRE      9
#define CMD_REPRENDRE      10
#define CMD_PAS_A_PAS      11
#define CMD_NORMAL         12
#define CMD_PRENDRE        13
#define CMD_ABREGER        14
#define CMD_IN_EXTENSO     15
#define CMD_RUBAN          16
#define CMD_SILENCE        17
#define CMD_APPELER        18   /* commandes de fichiers : étape « fichiers » (IMP-0236) */
#define CMD_RANGER         19
#define CMD_IDENTIFICATION 20
#define CMD_UTILISATION    21
#define CMD_TABLE          22
#define CMD_MODIFIER       23
#define CMD_CATALOGUER     24
#define CMD_SUPPRIMER      25
#define CMD_DECODER        26
#define CMD_ENCODER        27
#define CMD_DUPLIQUER      28
#define CMD_CREATION_ID    29   /* initialisateur, état dormant (IMP-0257) */
#define CMD_SUPPRESSION_ID 30
#define CMD_LISTE_ID       31

/* Paramètres d'une commande. */
#define PARAM_AUCUN     0
#define PARAM_LIGNES    1   /* n1{,n2} */
#define PARAM_LISTE     2   /* EFFACER : n1{,n2}..., n1 A n2, * */
#define PARAM_NOMBRE    3   /* n */
#define PARAM_TEXTE     4   /* commandes de fichiers : texte libre */

/* Classe une entrée validée, déjà corrigée. dormant : état dormant de la
   console (seules BONJOUR et les commandes de l'initialisateur y sont des
   commandes ; celles-ci n'existent qu'en état dormant). Pour ENTREE_COMMANDE, *cmd
   reçoit la commande ; sinon CMD_AUCUNE. Aucun état n'est modifié. */
CategorieEntree analyseur_classer(const char *texte, unsigned int n, int dormant, Commande *cmd);
/* Nom complet de la commande dans le profil de la session (« LISTER A PARTIR
   DE ») ; la complétion est ce qui suit les deux premières lettres. */
const char     *analyseur_nom(Commande c);
unsigned char   analyseur_parametres(Commande c);

#endif
