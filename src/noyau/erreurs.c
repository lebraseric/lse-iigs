/* erreurs.c — catalogue des natures d'erreur et mise en forme des messages
   EXI: EXI-0019, EXI-0051  VM: VM-2210  IV: Tome IV §3.3, §17 (ALG-1701)
   IMP: IMP-0185, IMP-0186, IMP-0187, IMP-0195
   Tests: tests/composants/composants_erreurs.c
   Spécification : chapitre Gestionnaire des erreurs §4, §7, §8, §9, §9 bis, §9 ter.
   Segment : principal (IMP-0135). Aucun affichage : la console s'en charge.
   FICHIER GÉNÉRÉ par scripts/generer_erreurs.py à partir du §7 : ne pas le
   modifier à la main. */

#pragma noroot

#include <string.h>
#include "erreurs.h"
#include "../interfaces/profil.h"

typedef struct {
    char          classe;
    unsigned int  numero;
    const char   *libelle;   /* « _ » note la flèche d'affectation (ASCII 95) */
} EntreeCatalogue;

/* Indice = nature - 1, dans l'ordre du catalogue (§7). */
static const EntreeCatalogue catalogue[ERR_NB_NATURES] = {
    { 'C',  63, "LIGNE DE PLUS DE 80 CARACTERES" },   /* ERR_C_LIGNE_TROP_LONGUE */
    { 'C',   7, "NUMERO DE LIGNE NON SUIVI D'UN ESPACE" },   /* ERR_C_NUMERO_SANS_ESPACE */
    { 'C',  52, "NUMERO DE LIGNE NUL OU SUPERIEUR A 32000" },   /* ERR_C_NUMERO_HORS_BORNES */
    { 'C', 100, "NUMERO DE LIGNE AU-DELA DE LA BORNE DE CETTE MACHINE" },   /* ERR_C_NUMERO_AU_DELA_PROFIL */
    { 'C',   1, "LETTRE MINUSCULE HORS D'UNE CHAINE" },   /* ERR_C_MINUSCULE */
    { 'C',   1, "CARACTERE SPECIAL INTERDIT" },   /* ERR_C_CARACTERE_INTERDIT */
    { 'C',  53, "MOT DU LANGAGE MAL TERMINE" },   /* ERR_C_MOT_MAL_TERMINE */
    { 'C',   3, "IDENTIFICATEUR DE PLUS DE 5 CARACTERES" },   /* ERR_C_IDENTIFICATEUR_TROP_LONG */
    { 'C',   5, "NOM DE PROCEDURE DE PLUS DE 5 CARACTERES" },   /* ERR_C_NOM_PROCEDURE_TROP_LONG */
    { 'C', 101, "MOT RESERVE EMPLOYE COMME IDENTIFICATEUR" },   /* ERR_C_MOT_RESERVE */
    { 'C', 102, "MOT RESERVE DE CETTE MACHINE EMPLOYE COMME IDENTIFICATEUR" },   /* ERR_C_MOT_RESERVE_PROFIL */
    { 'C',   8, "APOSTROPHE DE FIN DE CHAINE MANQUANTE" },   /* ERR_C_CHAINE_NON_TERMINEE */
    { 'C',   8, "POINT DE FIN D'EQUIVALENT NUMERIQUE MANQUANT" },   /* ERR_C_EQUIVALENT_NON_TERMINE */
    { 'C', 109, "EQUIVALENT NUMERIQUE HORS DES BORNES DE EQC" },   /* ERR_C_EQUIVALENT_HORS_BORNES */
    { 'C',   9, "CONSTANTE NUMERIQUE INCORRECTE" },   /* ERR_C_CONSTANTE_MAL_FORMEE */
    { 'C',   9, "CONSTANTE NUMERIQUE TROP GRANDE" },   /* ERR_C_CONSTANTE_HORS_PLAGE */
    { 'C',   6, "NOM DE PROCEDURE INCORRECT APRES &" },   /* ERR_C_NOM_PROCEDURE_INCORRECT */
    { 'C',  53, "INSTRUCTION INCORRECTE" },   /* ERR_C_INSTRUCTION_INCORRECTE */
    { 'C',  45, "PLUS DE ')' QUE DE '('" },   /* ERR_C_PARENTHESE_EN_EXCES */
    { 'C',  19, "OPERATEUR MANQUANT ENTRE DEUX OPERANDES" },   /* ERR_C_OPERATEUR_MANQUANT */
    { 'C',  19, "OPERANDE MANQUANT" },   /* ERR_C_OPERANDE_MANQUANT */
    { 'C',  19, "DEUX OPERATEURS ADJACENTS" },   /* ERR_C_OPERATEURS_ADJACENTS */
    { 'C',  20, "PARENTHESE FERMANTE MANQUANTE" },   /* ERR_C_PARENTHESE_MANQUANTE */
    { 'C',  16, "CROCHET FERMANT MANQUANT" },   /* ERR_C_CROCHET_MANQUANT */
    { 'C',  16, "PLUS DE DEUX INDICES" },   /* ERR_C_TROP_D_INDICES */
    { 'C',  26, "NOMBRE D'ARGUMENTS INCORRECT POUR CETTE FONCTION" },   /* ERR_C_ARGUMENTS_FONCTION */
    { 'C',  26, "DERNIER ARGUMENT DE SCH, GRL OU CNB : IDENTIFICATEUR ATTENDU" },   /* ERR_C_ARGUMENT_FACULTATIF */
    { 'C',  21, "SINON MANQUANT DANS UNE EXPRESSION CONDITIONNELLE" },   /* ERR_C_SINON_MANQUANT */
    { 'C',  17, "OPERATEUR DE RELATION INCORRECT OU ABSENT" },   /* ERR_C_RELATION_MANQUANTE */
    { 'C',  93, "@ NON SUIVI D'UN ENTIER" },   /* ERR_C_AROBASE_SANS_ENTIER */
    { 'C',  27, "CHAINE AU LIEU D'UN NOMBRE" },   /* ERR_C_CHAINE_AU_LIEU_NOMBRE */
    { 'C',  28, "NOMBRE AU LIEU D'UNE CHAINE" },   /* ERR_C_NOMBRE_AU_LIEU_CHAINE */
    { 'C',  15, "INDICE NON NUMERIQUE" },   /* ERR_C_INDICE_NON_NUMERIQUE */
    { 'C',  11, "COMPARAISON D'UN NOMBRE ET D'UNE CHAINE" },   /* ERR_C_COMPARAISON_TYPES */
    { 'C',  38, "INSTRUCTION INCONNUE OU _ MANQUANT" },   /* ERR_C_AFFECTATION_MANQUANTE */
    { 'C',  38, "LA CIBLE D'UNE AFFECTATION DOIT ETRE UNE VARIABLE" },   /* ERR_C_CIBLE_AFFECTATION */
    { 'C',  37, "ALLER NON SUIVI DE EN" },   /* ERR_C_EN_MANQUANT */
    { 'C',  22, "ALORS MANQUANT" },   /* ERR_C_ALORS_MANQUANT */
    { 'C',  36, "';' OU FIN MANQUANT DANS DEBUT...FIN" },   /* ERR_C_FIN_MANQUANT */
    { 'C',  55, "DEBUT D'INSTRUCTION INCORRECT" },   /* ERR_C_DEBUT_INSTRUCTION */
    { 'C',  32, "POUR MANQUANT DANS FAIRE" },   /* ERR_C_POUR_MANQUANT */
    { 'C',  32, "LA VARIABLE DE BOUCLE DOIT ETRE SIMPLE" },   /* ERR_C_VARIABLE_BOUCLE */
    { 'C',  33, "_ MANQUANT APRES LA VARIABLE DE BOUCLE" },   /* ERR_C_AFFECTATION_BOUCLE */
    { 'C',  34, "JUSQUA OU TANT QUE MANQUANT" },   /* ERR_C_FIN_DE_BOUCLE */
    { 'C',  34, "TANT NON SUIVI DE QUE" },   /* ERR_C_TANT_SANS_QUE */
    { 'C',  39, "NOM DE TABLEAU INCORRECT OU ABSENT" },   /* ERR_C_NOM_TABLEAU */
    { 'C',  14, "DIMENSIONS DE TABLEAU INCORRECTES" },   /* ERR_C_DECLARATION_TABLEAU */
    { 'C',  14, "DIMENSION DE TABLEAU NON NUMERIQUE" },   /* ERR_C_DIMENSION_NON_NUMERIQUE */
    { 'C',  35, "NOM DE VARIABLE INCORRECT DANS CHAINE OU LIBERER" },   /* ERR_C_NOM_DECLARATION */
    { 'C', 104, "LA CIBLE DE LIRE DOIT ETRE UNE VARIABLE" },   /* ERR_C_CIBLE_LIRE */
    { 'C',  44, "NOM DE VARIABLE INCORRECT OU ABSENT DANS GARER OU CHARGER" },   /* ERR_C_NOM_GARER_CHARGER */
    { 'C',  29, "ERREUR DE PARAMETRE DANS GARER, CHARGER OU SUPPRIMER" },   /* ERR_C_PARAMETRE_FICHIER */
    { 'C', 105, "EXECUTER : DEUX ARGUMENTS AU PLUS" },   /* ERR_C_ARGUMENTS_EXECUTER */
    { 'C', 103, "CONSTRUCTION ABSENTE DE CETTE MACHINE" },   /* ERR_C_HORS_PROFIL */
    { 'C', 106, "? RESERVE AU MODE MACHINE DE BUREAU" },   /* ERR_C_POINT_INTERROGATION */
    { 'C',  40, "']' MANQUANT DANS UN FORMAT" },   /* ERR_C_FORMAT_CROCHET */
    { 'C',  42, "SPECIFICATION DE FORMAT INCORRECTE" },   /* ERR_C_FORMAT_SPECIFICATION */
    { 'C',  42, "E OU D DE PLUS DE DEUX CHIFFRES DANS UN FORMAT" },   /* ERR_C_FORMAT_CHIFFRES */
    { 'C',  42, "'*' INTERDIT DEVANT UNE DESCRIPTION" },   /* ERR_C_FORMAT_ETOILE */
    { 'C',  41, "FACTEUR DE REPETITION NUL" },   /* ERR_C_FACTEUR_NUL */
    { 'C',  41, "FACTEUR DE REPETITION AU-DELA DE LA BORNE DE CETTE MACHINE" },   /* ERR_C_FACTEUR_AU_DELA_PROFIL */
    { 'C',  41, "FACTEUR DE REPETITION SUPERIEUR A 99" },   /* ERR_C_FACTEUR_SUPERIEUR_99 */
    { 'C',  54, "PLUS DE DESCRIPTIONS QUE DE VALEURS" },   /* ERR_C_FORMAT_DESCRIPTIONS */
    { 'C',  49, "NOM DE PROCEDURE INCORRECT OU ABSENT" },   /* ERR_C_DECLARATION_NOM */
    { 'C',  49, "NOM DE PROCEDURE NON SUIVI DE ()" },   /* ERR_C_DECLARATION_PARENTHESES */
    { 'C',  46, "LISTE DE PARAMETRES INCORRECTE" },   /* ERR_C_PARAMETRES_DECLARATION */
    { 'C',  51, "PARAMETRE FORMEL INDICE" },   /* ERR_C_PARAMETRE_INDICE */
    { 'C',  47, "DEUX PARAMETRES IDENTIQUES" },   /* ERR_C_PARAMETRE_DOUBLE */
    { 'C',  48, "DEUX PARAMETRES PROCEDURE IDENTIQUES" },   /* ERR_C_PARAMETRE_PROCEDURE_DOUBLE */
    { 'C',  48, "PARAMETRE IDENTIQUE AU NOM DE LA PROCEDURE" },   /* ERR_C_PARAMETRE_HOMONYME */
    { 'C',  35, "NOM DE PROCEDURE DANS LA LISTE LOCAL" },   /* ERR_C_LOCAL_PROCEDURE */
    { 'C',  47, "DEUX NOMS IDENTIQUES DANS LA LISTE LOCAL" },   /* ERR_C_LOCAL_DOUBLE */
    { 'C',  46, "TEXTE INATTENDU APRES LA DECLARATION DE PROCEDURE" },   /* ERR_C_DECLARATION_SUITE */
    { 'C',  46, "APPEL DE PROCEDURE MAL FORME" },   /* ERR_C_APPEL_MAL_FORME */
    { 'C',  58, "PROCEDURE INTERDITE EN MODE MACHINE DE BUREAU" },   /* ERR_C_BUREAU_PROCEDURE */
    { 'C',  58, "ALLER EN, FAIRE, RETOUR EN INTERDITS EN MODE MACHINE DE BUREAU" },   /* ERR_C_BUREAU_NUMERO_LIGNE */
    { 'C',  58, "RETOUR, RESULTAT INTERDITS EN MODE MACHINE DE BUREAU" },   /* ERR_C_BUREAU_RETOUR */
    { 'C',  58, "APPEL DE PROCEDURE INTERDIT EN MODE MACHINE DE BUREAU" },   /* ERR_C_BUREAU_APPEL_INSTRUCTION */
    { 'C',  58, "APPEL DE FONCTION INTERDIT EN MODE MACHINE DE BUREAU" },   /* ERR_C_BUREAU_APPEL_FONCTION */
    { 'C',  58, "DECLARATION INTERDITE EN MODE MACHINE DE BUREAU" },   /* ERR_C_BUREAU_DECLARATION */
    { 'C',  58, "EXECUTER INTERDIT EN MODE MACHINE DE BUREAU" },   /* ERR_C_BUREAU_EXECUTER */
    { 'C',  57, "PLUS DE PLACE DANS LA ZONE UTILISATEUR" },   /* ERR_C_PLUS_DE_PLACE */
    { 'C', 107, "LIGNE TROP COMPLEXE" },   /* ERR_C_LIGNE_TROP_COMPLEXE */
    { 'C', 108, "TROP DE NOMS OU DE CONSTANTES" },   /* ERR_C_CAPACITE_TABLES */
    { 'E',   5, "VARIABLE NON DEFINIE DANS UN CALCUL" },   /* ERR_E_NON_DEFINI_CALCUL */
    { 'E',  62, "VARIABLE NON DEFINIE AFFECTEE OU AFFICHEE" },   /* ERR_E_NON_DEFINI_AFFECTATION */
    { 'E',  10, "VARIABLE NON DEFINIE DANS UNE COMPARAISON" },   /* ERR_E_NON_DEFINI_COMPARAISON */
    { 'E',  62, "VARIABLE-CHAINE SANS VALEUR" },   /* ERR_E_CHAINE_SANS_VALEUR */
    { 'E',   6, "CHAINE RANGEE DANS UNE VARIABLE NON CHAINE" },   /* ERR_E_CHAINE_NON_DECLAREE */
    { 'E',   6, "NOMBRE RANGE DANS UNE CHAINE OU UN TABLEAU" },   /* ERR_E_NOMBRE_DANS_CHAINE */
    { 'E',   5, "CHAINE OU TABLEAU DANS UN CALCUL" },   /* ERR_E_CALCUL_NON_NUMERIQUE */
    { 'E',  10, "COMPARAISON D'UN NOMBRE ET D'UNE CHAINE" },   /* ERR_E_COMPARAISON_TYPES */
    { 'E',   8, "OPERANDE NON CHAINE DANS UNE EXPRESSION CHAINE" },   /* ERR_E_OPERANDE_NON_CHAINE */
    { 'E',  29, "DECLARATION TABLEAU D'UNE VARIABLE OU CHAINE" },   /* ERR_E_DECLARATION_TABLEAU */
    { 'E',   4, "DECLARATION CHAINE D'UNE VARIABLE OU TABLEAU" },   /* ERR_E_DECLARATION_CHAINE */
    { 'E',  29, "TABLEAU PARAMETRE DECLARE DE NOUVEAU" },   /* ERR_E_TABLEAU_PARAMETRE */
    { 'E', 100, "LIBERER D'UN IDENTIFICATEUR NON LOCAL" },   /* ERR_E_LIBERER_NON_LOCAL */
    { 'E',  47, "INDICES SUR UN IDENTIFICATEUR NON TABLEAU" },   /* ERR_E_INDICES_SANS_TABLEAU */
    { 'E',  49, "INDICE SUPERIEUR A LA BORNE DECLAREE" },   /* ERR_E_INDICE_TROP_GRAND */
    { 'E',  27, "INDICE NUL OU NEGATIF" },   /* ERR_E_INDICE_NUL */
    { 'E',  46, "NOMBRE D'INDICES NON CONFORME A LA DECLARATION" },   /* ERR_E_NOMBRE_INDICES */
    { 'E',  27, "DIMENSION DE TABLEAU NULLE OU NEGATIVE" },   /* ERR_E_DIMENSION_NULLE */
    { 'E',  45, "TABLEAU DE PLUS DE 65535 ELEMENTS" },   /* ERR_E_TABLEAU_TROP_GRAND */
    { 'E',  32, "ELEMENT DE TABLEAU SANS VALEUR" },   /* ERR_E_ELEMENT_SANS_VALEUR */
    { 'E',  31, "DIVISION PAR ZERO" },   /* ERR_E_DIVISION_PAR_ZERO */
    { 'E',  25, "ZERO ELEVE A UNE PUISSANCE NEGATIVE" },   /* ERR_E_ZERO_PUISSANCE_NEGATIVE */
    { 'E',  26, "NOMBRE NEGATIF A UNE PUISSANCE NON ENTIERE" },   /* ERR_E_PUISSANCE_NON_ENTIERE */
    { 'E',  41, "RAC D'UN NOMBRE NEGATIF" },   /* ERR_E_RAC_NEGATIF */
    { 'E',  42, "LGN D'UN NOMBRE NEGATIF OU NUL" },   /* ERR_E_LGN_NEGATIF */
    { 'E',  44, "DEPASSEMENT DE CAPACITE ARITHMETIQUE" },   /* ERR_E_DEPASSEMENT */
    { 'E',   9, "ARGUMENT DE ALE NEGATIF OU SUPERIEUR A 1" },   /* ERR_E_ARGUMENT_ALE */
    { 'E',  30, "ARGUMENT DE EQC HORS BORNES" },   /* ERR_E_EQC_HORS_BORNES */
    { 'E',  26, "POSITION SUPERIEURE A LA LONGUEUR + 1" },   /* ERR_E_POSITION_AU_DELA */
    { 'E',  11, "POSITION DE DEPART ERRONEE" },   /* ERR_E_POSITION_ERRONEE */
    { 'E',  17, "LONGUEUR DEMANDEE NEGATIVE" },   /* ERR_E_LONGUEUR_NEGATIVE */
    { 'E',  18, "DEUXIEME ARGUMENT NON NUMERIQUE" },   /* ERR_E_ARGUMENT_2_NON_NUMERIQUE */
    { 'E',  17, "TROISIEME ARGUMENT NI CHAINE NI NOMBRE" },   /* ERR_E_ARGUMENT_3_ERRONE */
    { 'E',  21, "POSITION ERRONEE DANS EQN" },   /* ERR_E_POSITION_EQN */
    { 'E', 101, "CHAINE TROP LONGUE" },   /* ERR_E_CHAINE_TROP_LONGUE */
    { 'E',  75, "LIGNE DE BRANCHEMENT INEXISTANTE" },   /* ERR_E_LIGNE_INEXISTANTE */
    { 'E',  37, "NUMERO DE LIGNE DE BRANCHEMENT NUL" },   /* ERR_E_LIGNE_NULLE */
    { 'E',  38, "NUMERO DE LIGNE NON CONVERTIBLE EN ENTIER" },   /* ERR_E_LIGNE_NON_ENTIERE */
    { 'E',  58, "FIN DU PROGRAMME SANS TERMINER" },   /* ERR_E_FIN_DE_PROGRAMME */
    { 'E',  37, "FIN DE BOUCLE NULLE OU NEGATIVE" },   /* ERR_E_FIN_BOUCLE_NULLE */
    { 'E', 102, "FIN DE BOUCLE AVANT LA LIGNE FAIRE" },   /* ERR_E_FIN_BOUCLE_AVANT */
    { 'E',  43, "VARIABLE DE BOUCLE NON NUMERIQUE" },   /* ERR_E_VARIABLE_BOUCLE */
    { 'E',  12, "EXECUTION D'UNE LIGNE DE DECLARATION" },   /* ERR_E_LIGNE_DECLARATION */
    { 'E', 103, "PROCEDURE NON DECLAREE" },   /* ERR_E_PROCEDURE_NON_DECLAREE */
    { 'E',  13, "NOMBRE DE PARAMETRES EFFECTIFS INCORRECT" },   /* ERR_E_NOMBRE_PARAMETRES */
    { 'E',  72, "EXPRESSION PASSEE A UN PARAMETRE REFERENCE" },   /* ERR_E_PARAMETRE_REFERENCE */
    { 'E',  14, "PARAMETRE LOCAL SANS VALEUR" },   /* ERR_E_PARAMETRE_SANS_VALEUR */
    { 'E',  20, "RETOUR OU RESULTAT HORS PROCEDURE" },   /* ERR_E_RETOUR_HORS_PROCEDURE */
    { 'E',  15, "RETOUR SANS RESULTAT DANS UNE FONCTION" },   /* ERR_E_RETOUR_DANS_FONCTION */
    { 'E',  22, "RESULTAT DANS UNE PROCEDURE SOUS-PROGRAMME" },   /* ERR_E_RESULTAT_SOUS_PROGRAMME */
    { 'E',  59, "RESULTAT QUI N'EST NI UN NOMBRE NI UNE CHAINE" },   /* ERR_E_RESULTAT_TABLEAU */
    { 'E', 104, "FACTEUR DE REPETITION NEGATIF" },   /* ERR_E_FACTEUR_NEGATIF */
    { 'E',   6, "VALEUR CONTRAIRE A LA DESCRIPTION DU FORMAT" },   /* ERR_E_VALEUR_FORMAT */
    { 'E',  50, "NUMERO D'ENREGISTREMENT INCORRECT" },   /* ERR_E_NUMERO_ENREGISTREMENT */
    { 'E',  52, "PLUS DE PLACE DANS LE FICHIER" },   /* ERR_E_FICHIER_PLEIN */
    { 'E',  53, "NOM DE FICHIER ERRONE" },   /* ERR_E_NOM_FICHIER */
    { 'E',  55, "FICHIER INEXISTANT" },   /* ERR_E_FICHIER_INEXISTANT */
    { 'E',  54, "FICHIER PROTEGE PAR UNE AUTRE IDENTIFICATION" },   /* ERR_E_FICHIER_PROTEGE */
    { 'E',  56, "ENREGISTREMENT INEXISTANT" },   /* ERR_E_ENREGISTREMENT_INEXISTANT */
    { 'E',  47, "CIBLE DE CHARGER D'UN AUTRE TYPE" },   /* ERR_E_TYPE_CHARGER */
    { 'E',  56, "COMPTE RENDU NON NUMERIQUE" },   /* ERR_E_COMPTE_RENDU */
    { 'E', 105, "MODULE INTROUVABLE POUR EXECUTER" },   /* ERR_E_MODULE_INTROUVABLE */
    { 'E',   3, "MEMOIRE SATUREE" },   /* ERR_E_MEMOIRE_SATUREE */
    { 'E',  96, "PROBLEME INTERNE A LSE" },   /* ERR_E_INTERNE */
    { 'D',   0, "COMMANDE ERRONEE" },   /* ERR_D_COMMANDE_ERRONEE */
    { 'D',   0, "PARAMETRE DE COMMANDE ERRONE" },   /* ERR_D_PARAMETRE */
    { 'D',   0, "LIGNE DE DEPART INEXISTANTE" },   /* ERR_D_LIGNE_DEPART_ABSENTE */
    { 'D',   0, "CONTINUER IMPOSSIBLE" },   /* ERR_D_CONTINUER_IMPOSSIBLE */
    { 'D',   0, "REPRENDRE IMPOSSIBLE" },   /* ERR_D_REPRENDRE_IMPOSSIBLE */
    { 'D',   0, "CONSOLE INEXISTANTE" },   /* ERR_D_CONSOLE_INEXISTANTE */
    { 'D',   0, "IDENTIFICATION ILLEGALE" },   /* ERR_D_IDENTIFICATION_ILLEGALE */
    { 'F',   2, "SUPPORT PLEIN" },   /* ERR_F_SUPPORT_PLEIN */
    { 'F',   3, "PROGRAMME DEJA EXISTANT SUR LE SUPPORT" },   /* ERR_F_PROGRAMME_EXISTANT */
    { 'F',   4, "FICHIER DE DONNEES PERMANENT DEJA EXISTANT" },   /* ERR_F_DONNEES_EXISTANT */
    { 'F',   5, "FICHIER INEXISTANT" },   /* ERR_F_FICHIER_INEXISTANT */
    { 'F',   6, "FICHIER PROGRAMME PROTEGE PAR UNE IDENTIFICATION" },   /* ERR_F_PROGRAMME_PROTEGE */
    { 'F',   7, "DROIT DE CREATION DE PROGRAMME ABSENT" },   /* ERR_F_DROIT_PROGRAMME */
    { 'F',   8, "DROIT DE CREATION DE DONNEES PERMANENTES ABSENT" },   /* ERR_F_DROIT_DONNEES */
    { 'F',  10, "IDENTIFICATION INEXISTANTE" },   /* ERR_F_IDENTIFICATION_INEXISTANTE */
    { 'F',  11, "DROIT DE CREATION DE FICHIER SECRET ABSENT" },   /* ERR_F_DROIT_SECRET */
    { 'F',  13, "EXTENSION TROP IMPORTANTE DE L'ESPACE TEMPORAIRE" },   /* ERR_F_EXTENSION_TEMPORAIRE */
    { 'F',  17, "RACINE * INTERDITE POUR CETTE COMMANDE" },   /* ERR_F_RACINE_INTERDITE */
    { 'F',  19, "CATALOGUE SATURE" },   /* ERR_F_CATALOGUE_SATURE */
    { 'F',  22, "REMPLACEMENT SOUS UNE MAUVAISE IDENTIFICATION" },   /* ERR_F_MAUVAISE_IDENTIFICATION */
    { 'F',  23, "MEMOIRE TROP PETITE POUR LE PROGRAMME" },   /* ERR_F_MEMOIRE_PROGRAMME */
    { 'F',  25, "AUTRE NOM QUE CELUI DU DERNIER APPEL" },   /* ERR_F_AUTRE_NOM */
    { 'F',  27, "OPERATION D'ENTREE-SORTIE IMPOSSIBLE" },   /* ERR_F_ENTREE_SORTIE */
    { 'F',  29, "NUMERO D'UNITE INCORRECT" },   /* ERR_F_UNITE */
    { 'F', 100, "FICHIER RANGE SOUS UN AUTRE PROFIL" },   /* ERR_F_AUTRE_PROFIL */
    { 'F', 101, "FICHIER ENDOMMAGE" },   /* ERR_F_FICHIER_ENDOMMAGE */
    { 'F', 102, "BIBLIOTHEQUE INACCESSIBLE" }   /* ERR_F_BIBLIOTHEQUE */
};

static const EntreeCatalogue *entree(NatureErreur n)
{
    if (n < 1 || n > ERR_NB_NATURES)
        return NULL;
    return &catalogue[n - 1];
}

char erreurs_classe(NatureErreur n)
{
    const EntreeCatalogue *e = entree(n);
    return e != NULL ? e->classe : 0;
}

unsigned int erreurs_numero(NatureErreur n)
{
    const EntreeCatalogue *e = entree(n);
    return e != NULL ? e->numero : 0;
}

const char *erreurs_libelle(NatureErreur n)
{
    const EntreeCatalogue *e = entree(n);
    return e != NULL ? e->libelle : NULL;
}

/* Ajoute texte à sortie sans dépasser taille - 1 caractères. */
static void ajouter(char *sortie, unsigned int taille, unsigned int *lg, const char *texte)
{
    unsigned int i = 0;
    while (texte[i] != '\0' && *lg + 1 < taille)
        sortie[(*lg)++] = texte[i++];
    sortie[*lg] = '\0';
}

/* Écrit v en décimal, sur au moins min chiffres complétés de zéros. */
static void ajouter_nombre(char *sortie, unsigned int taille, unsigned int *lg,
                           unsigned int v, unsigned int min)
{
    char chiffres[6];
    unsigned int i = sizeof chiffres - 1;

    chiffres[i] = '\0';
    do {
        chiffres[--i] = (char) ('0' + v % 10);
        v /= 10;
    } while (v != 0);
    while (sizeof chiffres - 1 - i < min)
        chiffres[--i] = '0';
    ajouter(sortie, taille, lg, &chiffres[i]);
}

int erreurs_formater(NatureErreur n, int avec_libelle, char *sortie, unsigned int taille)
{
    const EntreeCatalogue *e = entree(n);
    unsigned int lg = 0;

    if (taille == 0)
        return 0;
    sortie[0] = '\0';
    if (e == NULL || e->classe != 'C')
        return 0;
    ajouter(sortie, taille, &lg, "ERREUR  C ");
    ajouter_nombre(sortie, taille, &lg, e->numero, 2);   /* IMP-0186 */
    if (avec_libelle) {
        ajouter(sortie, taille, &lg, " : ");
        ajouter(sortie, taille, &lg, e->libelle);
    }
    return 1;
}

int erreurs_formater_execution(NatureErreur n, unsigned char ligne, int avec_libelle,
                               char *sortie, unsigned int taille)
{
    const EntreeCatalogue *e = entree(n);
    unsigned int lg = 0;

    if (taille == 0)
        return 0;
    sortie[0] = '\0';
    if (e == NULL || e->classe != 'E')
        return 0;
    ajouter(sortie, taille, &lg, "ERREUR (EX.) ");
    ajouter_nombre(sortie, taille, &lg, e->numero, 2);
    ajouter(sortie, taille, &lg, " EN LIGNE ");
    if (ligne == 0)
        ajouter(sortie, taille, &lg, "BUR");
    else
        ajouter_nombre(sortie, taille, &lg, ligne, 3);
    if (avec_libelle) {
        ajouter(sortie, taille, &lg, " : ");
        ajouter(sortie, taille, &lg, e->libelle);
    }
    return 1;
}

int erreurs_formater_fichier(NatureErreur n, int en_execution, unsigned char ligne,
                             int avec_libelle, char *sortie, unsigned int taille)
{
    const EntreeCatalogue *e = entree(n);
    unsigned int lg = 0;

    if (taille == 0)
        return 0;
    sortie[0] = '\0';
    if (e == NULL || e->classe != 'F')
        return 0;
    ajouter(sortie, taille, &lg, "ERREUR F ");                /* Victor Duruy */
    ajouter_nombre(sortie, taille, &lg, e->numero, 2);
    if (en_execution) {
        ajouter(sortie, taille, &lg, " EN LIGNE ");
        if (ligne == 0)
            ajouter(sortie, taille, &lg, "BUR");
        else
            ajouter_nombre(sortie, taille, &lg, ligne, 3);
    }
    if (avec_libelle) {
        ajouter(sortie, taille, &lg, " : ");
        ajouter(sortie, taille, &lg, e->libelle);
    }
    return 1;
}

int erreurs_formater_dialogue(NatureErreur n, int avec_libelle, char *sortie, unsigned int taille)
{
    const EntreeCatalogue *e = entree(n);
    unsigned int lg = 0;

    if (taille == 0)
        return 0;
    sortie[0] = '\0';
    if (e == NULL || e->classe != 'D')
        return 0;
    ajouter(sortie, taille, &lg, "????");
    if (avec_libelle) {
        ajouter(sortie, taille, &lg, " : ");
        ajouter(sortie, taille, &lg, e->libelle);
    }
    return 1;
}
