/* erreurs.h — natures d'erreur, catalogue et mise en forme des messages
   EXI: EXI-0019, EXI-0051  VM: VM-2210  IV: Tome IV §3.3, §17 (ALG-1701)
   IMP: IMP-0185 (numérotation), IMP-0186 (forme des messages), IMP-0187
        (granularité), IMP-0195 (structure du diagnostic)
   Tests: tests/composants/composants_erreurs.c
   Spécification : chapitre Gestionnaire des erreurs §7 (catalogue de
   compilation), §8, §9 (catalogue d'exécution), §9 bis (dialogue), §9 ter (fichier).
   FICHIER GÉNÉRÉ par scripts/generer_erreurs.py à partir du §7 : ne pas le
   modifier à la main. */

#ifndef LSE_ERREURS_H
#define LSE_ERREURS_H

typedef unsigned int NatureErreur;

#define ERR_AUCUNE 0

/* --- compilation (classe C, domaine COMPILATION), §7 ; exécution (classe E,
   domaine EXECUTION), §9 ; dialogue (classe D, domaine DIALOGUE), §9 bis ;
   fichier (classe F, domaine SERVICE), §9 ter --- */
#define ERR_C_LIGNE_TROP_LONGUE            1   /* C 63 */
#define ERR_C_NUMERO_SANS_ESPACE           2   /* C 07 */
#define ERR_C_NUMERO_HORS_BORNES           3   /* C 52 */
#define ERR_C_NUMERO_AU_DELA_PROFIL        4   /* C 100 */
#define ERR_C_MINUSCULE                    5   /* C 01 */
#define ERR_C_CARACTERE_INTERDIT           6   /* C 01 */
#define ERR_C_MOT_MAL_TERMINE              7   /* C 53 */
#define ERR_C_IDENTIFICATEUR_TROP_LONG     8   /* C 03 */
#define ERR_C_NOM_PROCEDURE_TROP_LONG      9   /* C 05 */
#define ERR_C_MOT_RESERVE                 10   /* C 101 */
#define ERR_C_MOT_RESERVE_PROFIL          11   /* C 102 */
#define ERR_C_CHAINE_NON_TERMINEE         12   /* C 08 */
#define ERR_C_EQUIVALENT_NON_TERMINE      13   /* C 08 */
#define ERR_C_EQUIVALENT_HORS_BORNES      14   /* C 109 */
#define ERR_C_CONSTANTE_MAL_FORMEE        15   /* C 09 */
#define ERR_C_CONSTANTE_HORS_PLAGE        16   /* C 09 */
#define ERR_C_NOM_PROCEDURE_INCORRECT     17   /* C 06 */
#define ERR_C_INSTRUCTION_INCORRECTE      18   /* C 53 */
#define ERR_C_PARENTHESE_EN_EXCES         19   /* C 45 */
#define ERR_C_OPERATEUR_MANQUANT          20   /* C 19 */
#define ERR_C_OPERANDE_MANQUANT           21   /* C 19 */
#define ERR_C_OPERATEURS_ADJACENTS        22   /* C 19 */
#define ERR_C_PARENTHESE_MANQUANTE        23   /* C 20 */
#define ERR_C_CROCHET_MANQUANT            24   /* C 16 */
#define ERR_C_TROP_D_INDICES              25   /* C 16 */
#define ERR_C_ARGUMENTS_FONCTION          26   /* C 26 */
#define ERR_C_ARGUMENT_FACULTATIF         27   /* C 26 */
#define ERR_C_SINON_MANQUANT              28   /* C 21 */
#define ERR_C_RELATION_MANQUANTE          29   /* C 17 */
#define ERR_C_AROBASE_SANS_ENTIER         30   /* C 93 */
#define ERR_C_CHAINE_AU_LIEU_NOMBRE       31   /* C 27 */
#define ERR_C_NOMBRE_AU_LIEU_CHAINE       32   /* C 28 */
#define ERR_C_INDICE_NON_NUMERIQUE        33   /* C 15 */
#define ERR_C_COMPARAISON_TYPES           34   /* C 11 */
#define ERR_C_AFFECTATION_MANQUANTE       35   /* C 38 */
#define ERR_C_CIBLE_AFFECTATION           36   /* C 38 */
#define ERR_C_EN_MANQUANT                 37   /* C 37 */
#define ERR_C_ALORS_MANQUANT              38   /* C 22 */
#define ERR_C_FIN_MANQUANT                39   /* C 36 */
#define ERR_C_DEBUT_INSTRUCTION           40   /* C 55 */
#define ERR_C_POUR_MANQUANT               41   /* C 32 */
#define ERR_C_VARIABLE_BOUCLE             42   /* C 32 */
#define ERR_C_AFFECTATION_BOUCLE          43   /* C 33 */
#define ERR_C_FIN_DE_BOUCLE               44   /* C 34 */
#define ERR_C_TANT_SANS_QUE               45   /* C 34 */
#define ERR_C_NOM_TABLEAU                 46   /* C 39 */
#define ERR_C_DECLARATION_TABLEAU         47   /* C 14 */
#define ERR_C_DIMENSION_NON_NUMERIQUE     48   /* C 14 */
#define ERR_C_NOM_DECLARATION             49   /* C 35 */
#define ERR_C_CIBLE_LIRE                  50   /* C 104 */
#define ERR_C_NOM_GARER_CHARGER           51   /* C 44 */
#define ERR_C_PARAMETRE_FICHIER           52   /* C 29 */
#define ERR_C_ARGUMENTS_EXECUTER          53   /* C 105 */
#define ERR_C_HORS_PROFIL                 54   /* C 103 */
#define ERR_C_POINT_INTERROGATION         55   /* C 106 */
#define ERR_C_FORMAT_CROCHET              56   /* C 40 */
#define ERR_C_FORMAT_SPECIFICATION        57   /* C 42 */
#define ERR_C_FORMAT_CHIFFRES             58   /* C 42 */
#define ERR_C_FORMAT_ETOILE               59   /* C 42 */
#define ERR_C_FACTEUR_NUL                 60   /* C 41 */
#define ERR_C_FACTEUR_AU_DELA_PROFIL      61   /* C 41 */
#define ERR_C_FACTEUR_SUPERIEUR_99        62   /* C 41 */
#define ERR_C_FORMAT_DESCRIPTIONS         63   /* C 54 */
#define ERR_C_DECLARATION_NOM             64   /* C 49 */
#define ERR_C_DECLARATION_PARENTHESES     65   /* C 49 */
#define ERR_C_PARAMETRES_DECLARATION      66   /* C 46 */
#define ERR_C_PARAMETRE_INDICE            67   /* C 51 */
#define ERR_C_PARAMETRE_DOUBLE            68   /* C 47 */
#define ERR_C_PARAMETRE_PROCEDURE_DOUBLE  69   /* C 48 */
#define ERR_C_PARAMETRE_HOMONYME          70   /* C 48 */
#define ERR_C_LOCAL_PROCEDURE             71   /* C 35 */
#define ERR_C_LOCAL_DOUBLE                72   /* C 47 */
#define ERR_C_DECLARATION_SUITE           73   /* C 46 */
#define ERR_C_APPEL_MAL_FORME             74   /* C 46 */
#define ERR_C_BUREAU_PROCEDURE            75   /* C 58 */
#define ERR_C_BUREAU_NUMERO_LIGNE         76   /* C 58 */
#define ERR_C_BUREAU_RETOUR               77   /* C 58 */
#define ERR_C_BUREAU_APPEL_INSTRUCTION    78   /* C 58 */
#define ERR_C_BUREAU_APPEL_FONCTION       79   /* C 58 */
#define ERR_C_BUREAU_DECLARATION          80   /* C 58 */
#define ERR_C_BUREAU_EXECUTER             81   /* C 58 */
#define ERR_C_PLUS_DE_PLACE               82   /* C 57 */
#define ERR_C_LIGNE_TROP_COMPLEXE         83   /* C 107 */
#define ERR_C_CAPACITE_TABLES             84   /* C 108 */
#define ERR_E_NON_DEFINI_CALCUL           85   /* EX 05 */
#define ERR_E_NON_DEFINI_AFFECTATION      86   /* EX 62 */
#define ERR_E_NON_DEFINI_COMPARAISON      87   /* EX 10 */
#define ERR_E_CHAINE_SANS_VALEUR          88   /* EX 62 */
#define ERR_E_CHAINE_NON_DECLAREE         89   /* EX 06 */
#define ERR_E_NOMBRE_DANS_CHAINE          90   /* EX 06 */
#define ERR_E_CALCUL_NON_NUMERIQUE        91   /* EX 05 */
#define ERR_E_COMPARAISON_TYPES           92   /* EX 10 */
#define ERR_E_OPERANDE_NON_CHAINE         93   /* EX 08 */
#define ERR_E_DECLARATION_TABLEAU         94   /* EX 29 */
#define ERR_E_DECLARATION_CHAINE          95   /* EX 04 */
#define ERR_E_TABLEAU_PARAMETRE           96   /* EX 29 */
#define ERR_E_LIBERER_NON_LOCAL           97   /* EX 100 */
#define ERR_E_INDICES_SANS_TABLEAU        98   /* EX 47 */
#define ERR_E_INDICE_TROP_GRAND           99   /* EX 49 */
#define ERR_E_INDICE_NUL                 100   /* EX 27 */
#define ERR_E_NOMBRE_INDICES             101   /* EX 46 */
#define ERR_E_DIMENSION_NULLE            102   /* EX 27 */
#define ERR_E_TABLEAU_TROP_GRAND         103   /* EX 45 */
#define ERR_E_ELEMENT_SANS_VALEUR        104   /* EX 32 */
#define ERR_E_DIVISION_PAR_ZERO          105   /* EX 31 */
#define ERR_E_ZERO_PUISSANCE_NEGATIVE    106   /* EX 25 */
#define ERR_E_PUISSANCE_NON_ENTIERE      107   /* EX 26 */
#define ERR_E_RAC_NEGATIF                108   /* EX 41 */
#define ERR_E_LGN_NEGATIF                109   /* EX 42 */
#define ERR_E_DEPASSEMENT                110   /* EX 44 */
#define ERR_E_ARGUMENT_ALE               111   /* EX 09 */
#define ERR_E_EQC_HORS_BORNES            112   /* EX 30 */
#define ERR_E_POSITION_AU_DELA           113   /* EX 26 */
#define ERR_E_POSITION_ERRONEE           114   /* EX 11 */
#define ERR_E_LONGUEUR_NEGATIVE          115   /* EX 17 */
#define ERR_E_ARGUMENT_2_NON_NUMERIQUE   116   /* EX 18 */
#define ERR_E_ARGUMENT_3_ERRONE          117   /* EX 17 */
#define ERR_E_POSITION_EQN               118   /* EX 21 */
#define ERR_E_CHAINE_TROP_LONGUE         119   /* EX 101 */
#define ERR_E_LIGNE_INEXISTANTE          120   /* EX 75 */
#define ERR_E_LIGNE_NULLE                121   /* EX 37 */
#define ERR_E_LIGNE_NON_ENTIERE          122   /* EX 38 */
#define ERR_E_FIN_DE_PROGRAMME           123   /* EX 58 */
#define ERR_E_FIN_BOUCLE_NULLE           124   /* EX 37 */
#define ERR_E_FIN_BOUCLE_AVANT           125   /* EX 102 */
#define ERR_E_VARIABLE_BOUCLE            126   /* EX 43 */
#define ERR_E_LIGNE_DECLARATION          127   /* EX 12 */
#define ERR_E_PROCEDURE_NON_DECLAREE     128   /* EX 103 */
#define ERR_E_NOMBRE_PARAMETRES          129   /* EX 13 */
#define ERR_E_PARAMETRE_REFERENCE        130   /* EX 72 */
#define ERR_E_PARAMETRE_SANS_VALEUR      131   /* EX 14 */
#define ERR_E_RETOUR_HORS_PROCEDURE      132   /* EX 20 */
#define ERR_E_RETOUR_DANS_FONCTION       133   /* EX 15 */
#define ERR_E_RESULTAT_SOUS_PROGRAMME    134   /* EX 22 */
#define ERR_E_RESULTAT_TABLEAU           135   /* EX 59 */
#define ERR_E_FACTEUR_NEGATIF            136   /* EX 104 */
#define ERR_E_VALEUR_FORMAT              137   /* EX 06 */
#define ERR_E_NUMERO_ENREGISTREMENT      138   /* EX 50 */
#define ERR_E_FICHIER_PLEIN              139   /* EX 52 */
#define ERR_E_NOM_FICHIER                140   /* EX 53 */
#define ERR_E_FICHIER_INEXISTANT         141   /* EX 55 */
#define ERR_E_FICHIER_PROTEGE            142   /* EX 54 */
#define ERR_E_ENREGISTREMENT_INEXISTANT  143   /* EX 56 */
#define ERR_E_TYPE_CHARGER               144   /* EX 47 */
#define ERR_E_COMPTE_RENDU               145   /* EX 56 */
#define ERR_E_MODULE_INTROUVABLE         146   /* EX 105 */
#define ERR_E_MEMOIRE_SATUREE            147   /* EX 03 */
#define ERR_E_INTERNE                    148   /* EX 96 */
#define ERR_D_COMMANDE_ERRONEE           149   /* ???? */
#define ERR_D_PARAMETRE                  150   /* ???? */
#define ERR_D_LIGNE_DEPART_ABSENTE       151   /* ???? */
#define ERR_D_CONTINUER_IMPOSSIBLE       152   /* ???? */
#define ERR_D_REPRENDRE_IMPOSSIBLE       153   /* ???? */
#define ERR_D_CONSOLE_INEXISTANTE        154   /* ???? */
#define ERR_D_IDENTIFICATION_ILLEGALE    155   /* ???? */
#define ERR_F_SUPPORT_PLEIN              156   /* F 02 */
#define ERR_F_PROGRAMME_EXISTANT         157   /* F 03 */
#define ERR_F_DONNEES_EXISTANT           158   /* F 04 */
#define ERR_F_FICHIER_INEXISTANT         159   /* F 05 */
#define ERR_F_PROGRAMME_PROTEGE          160   /* F 06 */
#define ERR_F_DROIT_PROGRAMME            161   /* F 07 */
#define ERR_F_DROIT_DONNEES              162   /* F 08 */
#define ERR_F_IDENTIFICATION_INEXISTANTE 163   /* F 10 */
#define ERR_F_DROIT_SECRET               164   /* F 11 */
#define ERR_F_EXTENSION_TEMPORAIRE       165   /* F 13 */
#define ERR_F_RACINE_INTERDITE           166   /* F 17 */
#define ERR_F_CATALOGUE_SATURE           167   /* F 19 */
#define ERR_F_MAUVAISE_IDENTIFICATION    168   /* F 22 */
#define ERR_F_MEMOIRE_PROGRAMME          169   /* F 23 */
#define ERR_F_AUTRE_NOM                  170   /* F 25 */
#define ERR_F_ENTREE_SORTIE              171   /* F 27 */
#define ERR_F_UNITE                      172   /* F 29 */
#define ERR_F_AUTRE_PROFIL               173   /* F 100 */
#define ERR_F_FICHIER_ENDOMMAGE          174   /* F 101 */
#define ERR_F_BIBLIOTHEQUE               175   /* F 102 */

#define ERR_NB_NATURES 175

/* Classe d'une nature : 'C' (compilation), 'E' (exécution), 'D' (dialogue),
   'F' (fichier) ; 0 si la nature est inconnue. */
char          erreurs_classe(NatureErreur n);
/* Numéro affiché (IMP-0185) ; 0 si la nature est inconnue. */
unsigned int  erreurs_numero(NatureErreur n);
/* Libellé de la liste des erreurs ; NULL si la nature est inconnue. */
const char   *erreurs_libelle(NatureErreur n);
/* Message de IMP-0186 pour une nature de compilation : « ERREUR  C nn »,
   suivi de « : libellé » si avec_libelle est vrai. Écrit au plus taille - 1
   caractères et termine toujours la chaîne. Renvoie 0 si la nature est
   inconnue ou n'est pas de classe C (sortie vide). */
int           erreurs_formater(NatureErreur n, int avec_libelle,
                               char *sortie, unsigned int taille);
/* Message de IMP-0186 pour une nature d'exécution : « ERREUR (EX.) nn EN
   LIGNE lll », ou « EN LIGNE BUR » si ligne vaut 0, suivi de « : libellé »
   si avec_libelle est vrai. Mêmes règles de longueur ; 0 si la nature n'est
   pas de classe E. */
int           erreurs_formater_execution(NatureErreur n, unsigned char ligne, int avec_libelle,
                                         char *sortie, unsigned int taille);

/* Message d'une nature du dialogue (Moniteur IMP-0235) : « ???? », suivi de
   « : libellé » si avec_libelle est vrai ; 0 si la nature n'est pas de classe D. */
int           erreurs_formater_dialogue(NatureErreur n, int avec_libelle,
                                        char *sortie, unsigned int taille);

/* Message d'une nature de fichier (Bibliothécaire et fichiers IMP-0250) :
   « ERREUR F nn », suivi de « EN LIGNE lll » (« BUR » si ligne vaut 0) si
   en_execution est vrai, puis de « : libellé » si avec_libelle est vrai ;
   0 si la nature n'est pas de classe F. */
int           erreurs_formater_fichier(NatureErreur n, int en_execution, unsigned char ligne,
                                       int avec_libelle, char *sortie, unsigned int taille);

/* Diagnostic d'exécution (IMP-0195, point 2 bis) : ligne 0 = machine de bureau. */
typedef struct {
    NatureErreur  nature;
    unsigned char ligne;
} DiagnosticExecution;

#endif
