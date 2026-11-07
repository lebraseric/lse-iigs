/* analyseur.c — analyseur de commandes : classement d'une entrée validée
   EXI: EXI-0009, EXI-0028, EXI-0029, EXI-0052
   VM: VM-2202
   IMP: IMP-0229 (classement), IMP-0230 (table des commandes, complétion du
        manuel du profil), IMP-0236 (commandes différées), IMP-0257
        (commandes de l'initialisateur)
   Tests: tests/composants/composants_analyseur.c
   Spécification : chapitre Moniteur et console §3, §5. Segment principal
   (IMP-0227). */

#pragma noroot

#include "analyseur.h"
#include "../interfaces/types_communs.h"
#include "../interfaces/profil.h"

#define PROFILS_DEUX   0
#define PROFIL_MITRA   1   /* MITRA 15 seulement */
#define PROFIL_F4      2   /* LSE F4 seulement */
#define DORMANT        3   /* initialisateur : deux profils, état dormant seulement */

typedef struct {
    Commande       commande;
    const char    *nom;           /* nom complet, profil T1600 ou commun */
    const char    *nom_mitra;     /* nom sous MITRA 15 s'il diffère, sinon 0 */
    unsigned char  parametres;
    unsigned char  profils;
} EntreeCommande;

/* INRDP75 §4, récapitulatif « Commandes » ; LSE15 §9-2 (IMP-0230) ;
   TABLE et UTILISATION : LAFOND83 p. 17 et 110 (IMP-0257). */
static const EntreeCommande table[] = {
    { CMD_BONJOUR,        "BONJOUR",                0,               PARAM_AUCUN,  PROFILS_DEUX },
    { CMD_AU_REVOIR,      "AU REVOIR",              0,               PARAM_AUCUN,  PROFILS_DEUX },
    { CMD_LISTER,         "LISTER A PARTIR DE ",    0,               PARAM_LIGNES, PROFILS_DEUX },
    { CMD_PERFORER,       "PERFORER A PARTIR DE ",  0,               PARAM_LIGNES, PROFILS_DEUX },
    { CMD_NUMERO,         "NUMERO A PARTIR DE ",    0,               PARAM_LIGNES, PROFILS_DEUX },
    { CMD_EFFACER,        "EFFACER LIGNES ",        0,               PARAM_LISTE,  PROFILS_DEUX },
    { CMD_EXECUTER,       "EXECUTER A PARTIR DE ",  0,               PARAM_LIGNES, PROFILS_DEUX },
    { CMD_CONTINUER,      "CONTINUER",              0,               PARAM_AUCUN,  PROFILS_DEUX },
    { CMD_POURSUIVRE,     "POURSUIVRE JUSQU'EN ",   0,               PARAM_NOMBRE, PROFILS_DEUX },
    { CMD_REPRENDRE,      "REPRENDRE A PARTIR DE ", "REPRENDRE EN ", PARAM_LIGNES, PROFILS_DEUX },
    { CMD_PAS_A_PAS,      "PAS A PAS",              0,               PARAM_AUCUN,  PROFILS_DEUX },
    { CMD_NORMAL,         "NORMAL",                 0,               PARAM_AUCUN,  PROFILS_DEUX },
    { CMD_PRENDRE,        "PRENDRE ETAT CONSOLE ",  0,               PARAM_NOMBRE, PROFILS_DEUX },
    { CMD_ABREGER,        "ABREGER",                0,               PARAM_AUCUN,  PROFILS_DEUX },
    { CMD_IN_EXTENSO,     "IN EXTENSO",             0,               PARAM_AUCUN,  PROFILS_DEUX },
    { CMD_RUBAN,          "RUBAN",                  0,               PARAM_AUCUN,  PROFIL_MITRA },
    { CMD_SILENCE,        "SILENCE",                0,               PARAM_AUCUN,  PROFIL_MITRA },
    { CMD_APPELER,        "APPELER ",               0,               PARAM_TEXTE,  PROFILS_DEUX },
    { CMD_RANGER,         "RANGER ",                0,               PARAM_TEXTE,  PROFILS_DEUX },
    { CMD_IDENTIFICATION, "IDENTIFICATION ",        0,               PARAM_TEXTE,  PROFILS_DEUX },
    { CMD_UTILISATION,    "UTILISATION DISQUE A PARTIR DE ", 0,      PARAM_TEXTE,  PROFILS_DEUX },
    { CMD_TABLE,          "TABLE DES FICHIERS A PARTIR DE ", 0,      PARAM_TEXTE,  PROFILS_DEUX },
    { CMD_MODIFIER,       "MODIFIER ",              0,               PARAM_TEXTE,  PROFILS_DEUX },
    { CMD_CATALOGUER,     "CATALOGUER ",            0,               PARAM_TEXTE,  PROFILS_DEUX },
    { CMD_SUPPRIMER,      "SUPPRIMER ",             0,               PARAM_TEXTE,  PROFILS_DEUX },
    { CMD_DECODER,        "DECODER ",               0,               PARAM_TEXTE,  PROFILS_DEUX },
    { CMD_ENCODER,        "ENCODER ",               0,               PARAM_TEXTE,  PROFILS_DEUX },
    { CMD_DUPLIQUER,      "DUPLIQUER ",             0,               PARAM_TEXTE,  PROFIL_F4 },
    /* MEMENTO82 §3.6.2 à §3.6.4 */
    { CMD_CREATION_ID,    "CREATION ID : ",         0,               PARAM_TEXTE,  DORMANT },
    { CMD_SUPPRESSION_ID, "SUPPRESSION ID : ",      0,               PARAM_TEXTE,  DORMANT },
    { CMD_LISTE_ID,       "LISTE ID :",             0,               PARAM_AUCUN,  DORMANT }
};

#define NB_COMMANDES (sizeof table / sizeof table[0])

static int du_profil(const EntreeCommande *e)
{
    if (e->profils == PROFIL_MITRA) return configuration_session.profil == PROFIL_MITRA15_V8;
    if (e->profils == PROFIL_F4) return configuration_session.profil == PROFIL_T1600_F4;
    return 1;
}

static const EntreeCommande *chercher(Commande c)
{
    unsigned int i;
    for (i = 0; i < NB_COMMANDES; i++)
        if (table[i].commande == c) return &table[i];
    return 0;
}

static int lettre(char x)
{
    return x >= 'A' && x <= 'Z';
}

CategorieEntree analyseur_classer(const char *texte, unsigned int n, int dormant, Commande *cmd)
{
    unsigned int i;
    const char *nom;

    *cmd = CMD_AUCUNE;
    if (n == 0) return ENTREE_VIDE;
    if (texte[0] >= '0' && texte[0] <= '9') return ENTREE_PROGRAMME;
    if (n != 2 || !lettre(texte[0]) || !lettre(texte[1])) return ENTREE_BUREAU;
    for (i = 0; i < NB_COMMANDES; i++) {
        if (!du_profil(&table[i])) continue;
        if (dormant ? table[i].commande != CMD_BONJOUR && table[i].profils != DORMANT
                    : table[i].profils == DORMANT)
            continue;
        nom = analyseur_nom(table[i].commande);
        if (nom[0] == texte[0] && nom[1] == texte[1]) {
            *cmd = table[i].commande;
            return ENTREE_COMMANDE;
        }
    }
    return ENTREE_INCONNUE;
}

const char *analyseur_nom(Commande c)
{
    const EntreeCommande *e = chercher(c);
    if (e == 0) return "";
    if (e->nom_mitra != 0 && configuration_session.profil == PROFIL_MITRA15_V8) return e->nom_mitra;
    return e->nom;
}

unsigned char analyseur_parametres(Commande c)
{
    const EntreeCommande *e = chercher(c);
    return e != 0 ? e->parametres : PARAM_AUCUN;
}
