/* composants_bibliotheque.c — fichiers-programmes (RCI-S) et EXECUTER
   EXI: EXI-0009, EXI-0022, EXI-0025  VM: VM-2209, VM-3401 à VM-3403
   IMP: IMP-0245, IMP-0246, IMP-0248 ; points B1 à B7 (étape 7b)
   Spécification des tests : chapitre Bibliothécaire et fichiers §3.
   Niveau : composants, sur fichiers réels (bibliothèque tests:BIBLIO). */

#include "compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/interprete.h"
#include "../../src/noyau/contexte_exec.h"
#include "../../src/noyau/decompilateur.h"
#include "../../src/noyau/bibliotheque.h"
#include "../../src/interfaces/interface_fichiers.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

void bouchon_console(const char *const *lignes);

static PileEvaluation pile;
static DiagnosticExecution diag;
static PositionExecution pos;
static const char *const aucune[] = { NULL };
static char textes[20][DECOMPILATEUR_LONGUEUR_MAX + 1];
static unsigned int nb_textes;

static int charger(const char *const *lignes)
{
    unsigned int i;
    NatureErreur n;
    contexte_vider();
    programme_vider();
    pile_ramener(&pile, 0);
    for (i = 0; lignes[i] != NULL; i++) {
        if (!compiler(lignes[i])) {
            printf("  [%s] rejet : C %u\n", lignes[i], erreurs_numero(d.nature));
            return 0;
        }
        if (!programme_inserer(&r.resultat.ligne_programme, &n)) return 0;
    }
    return 1;
}

/* Décompilation de tout le programme courant dans textes[]. */
static void photographier(void)
{
    EntreeObjetCompile e;
    unsigned int lg;
    for (nb_textes = 0; programme_rang(nb_textes, &e) && nb_textes < 20; nb_textes++)
        decompilateur_decompiler(&e, textes[nb_textes], &lg);
}

static int identique(void)
{
    EntreeObjetCompile e;
    char t[DECOMPILATEUR_LONGUEUR_MAX + 1];
    unsigned int lg;
    unsigned int i;
    if (programme_nombre() != nb_textes) return 0;
    for (i = 0; i < nb_textes; i++) {
        programme_rang(i, &e);
        if (decompilateur_decompiler(&e, t, &lg) != DECOMP_OK || strcmp(t, textes[i]) != 0) {
            printf("  [%s] au lieu de [%s]\n", t, textes[i]);
            return 0;
        }
    }
    return 1;
}

/* Exécution comme le moniteur : FIN_EXECUTER relance (point B5). */
static FinExecution executer(unsigned char depart)
{
    FinExecution f;
    contexte_initialiser(depart, 0, &pos);
    interprete_reinitialiser();
    for (;;) {
        f = interprete_executer(&pile, &pos, &diag);
        if (f != FIN_EXECUTER) return f;
        pile_ramener(&pile, 0);
        interprete_reinitialiser();
    }
}

static float valeur(const char *nom)
{
    ReferenceResolue ref;
    unsigned int id;
    float v = -999.0f;
    if (!symboles_chercher(nom, (unsigned char) strlen(nom), SYM_IDENTIFICATEUR, &id)) return -998.0f;
    if (contexte_resoudre(id, &ref) != ERR_AUCUNE) return -997.0f;
    contexte_lire_nombre(ref.cellule, USAGE_CALCUL, &v);
    return v;
}

/* Image d'un fichier-programme, altérée d'un octet puis réécrite avec sa somme de
   contrôle recalculée (audit du code, AC-17). */
static unsigned char original[512];
static unsigned char image[512];
static unsigned int taille_image;

static void lire_image(const char *nom)
{
    FichierPlateforme f;
    plateforme_fichier_ouvrir(ESPACE_PROGRAMMES, nom, FICHIER_LECTURE, &f);
    taille_image = (unsigned int) plateforme_fichier_taille(f);
    if (taille_image > sizeof original) taille_image = sizeof original;
    plateforme_fichier_lire(f, 0, original, taille_image);
    plateforme_fichier_fermer(f);
}

static void alterer(const char *nom, unsigned int position, unsigned char octet)
{
    FichierPlateforme f;
    unsigned int i;
    unsigned long somme = 0;

    memcpy(image, original, taille_image);
    image[position] = octet;
    for (i = 0; i + 2 < taille_image; i++) somme = somme + image[i];
    image[taille_image - 2] = (unsigned char) (somme & 0xFF);
    image[taille_image - 1] = (unsigned char) ((somme >> 8) & 0xFF);
    plateforme_fichier_ouvrir(ESPACE_PROGRAMMES, nom, FICHIER_MISE_A_JOUR, &f);
    plateforme_fichier_ecrire(f, 0, image, taille_image);
    plateforme_fichier_fermer(f);
}

int main(void)
{
    static const char *const dbl[] = {
        "10 CHAINE A,B", "20 A_'CHAINE PARTAGEE NUMERO UN 1'",
        "30 B_'CHAINE PARTAGEE NUMERO UN 2'", "40 TERMINER", NULL };
    static const char *const prog[] = {
        "10 CHAINE C;C_'UNE CHAINE ASSEZ LONGUE POUR ETRE PARTAGEE'",
        "20 R_&CARRE(7)+LGR(C)",
        "30 TERMINER",
        "40 PROCEDURE &CARRE(N) LOCAL N",
        "50 RESULTAT N*N", NULL };
    static const char *const module[] = { "1 X_5", "2 Y_X+1", "3 TERMINER", NULL };
    static const char *const appelant[] = { "10 Z_1", "20 EXECUTER 'MOD'", "30 TERMINER", NULL };
    static const char *const appelant2[] = { "10 EXECUTER 'MOD',2", "20 TERMINER", NULL };
    static const char *const appelant3[] = { "10 EXECUTER 'ZZ'", "20 TERMINER", NULL };
    static const char *const appelant4[] = { "10 EXECUTER 'MOD',7", "20 TERMINER", NULL };
    static const char *const vide[] = { "10 FAIRE 20 POUR I_1 JUSQUA 3", "20 ", "30 TERMINER", NULL };
    static const char *const attente[] = { "10 I_0;FAIRE 10 TANT QUE I<2;I_I+1", "20 TERMINER", NULL };
    FichierPlateforme f;
    unsigned char octet;
    static char ligne[90];
    unsigned int k;
    unsigned int j;
    unsigned int lgc;
    NatureErreur n;

    VERIF_DEBUT("composants/bibliotheque");
    programme_initialiser();
    pile_initialiser(&pile);
    bouchon_console(aucune);
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBLIO"));
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_espace_vider(ESPACE_DONNEES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
    {                                             /* identification 1 : P, D, S, X (IMP-0254) */
        Identification id;
        bibliotheque_identification_chercher(1, &id);
        bibliotheque_identifier(&id);
    }

    /* noms (INRDP75, 5e partie, §1-4 b) */
    VERIFIER(bibliotheque_nom_valide("A", 1) && bibliotheque_nom_valide("PROG1", 5));
    VERIFIER(!bibliotheque_nom_valide("PROGRA", 6) && !bibliotheque_nom_valide("1A", 2)
             && !bibliotheque_nom_valide("", 0));

    /* RANGER puis APPELER : programme identique, exécutable (IMP-0246) */
    VERIFIER(charger(prog));
    photographier();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("PROG", 0, 0, 0));
    VERIFIER_EGAL(ERR_F_PROGRAMME_EXISTANT, bibliotheque_ranger("PROG", 0, 0, 0));
    contexte_vider();
    programme_vider();
    VERIFIER_EGAL(0, programme_nombre());
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_appeler("PROG", 0, 0));
    VERIFIER(identique());
    VERIFIER(executer(0) == FIN_TERMINER && valeur("R") == 91.0f);   /* 49 + 42 */

    /* échecs : le programme courant reste intact (VM-3403) */
    VERIFIER_EGAL(ERR_F_FICHIER_INEXISTANT, bibliotheque_appeler("ABSEN", 0, 0));
    VERIFIER(identique());
    VERIFIER_EGAL(ERR_E_LIGNE_INEXISTANTE, bibliotheque_appeler("PROG", 99, 0));
    VERIFIER(identique());
    configuration_session.profil = PROFIL_T1600_V4;
    VERIFIER_EGAL(ERR_F_AUTRE_PROFIL, bibliotheque_appeler("PROG", 0, 0));
    configuration_session.profil = PROFIL_T1600_F4;                    /* IMP-0276 */
    VERIFIER_EGAL(ERR_F_AUTRE_PROFIL, bibliotheque_appeler("PROG", 0, 0));
    configuration_session.profil = PROFIL_MITRA15_V8;
    VERIFIER(identique());
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_ouvrir(ESPACE_PROGRAMMES, "PROG", FICHIER_MISE_A_JOUR, &f));
    plateforme_fichier_lire(f, 40, &octet, 1);
    octet = (unsigned char) (octet ^ 0x55);
    plateforme_fichier_ecrire(f, 40, &octet, 1);
    plateforme_fichier_fermer(f);
    VERIFIER_EGAL(ERR_F_FICHIER_ENDOMMAGE, bibliotheque_appeler("PROG", 0, 0));
    VERIFIER(identique());

    /* MODIFIER (point B3) et noms uniques (point B2) */
    VERIFIER_EGAL(ERR_F_FICHIER_INEXISTANT, bibliotheque_ranger("NOUV", 0, 1, 0));
    VERIFIER(charger(module));
    photographier();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("PROG", 0, 1, 0));    /* remplace l'endommagé */
    VERIFIER(!plateforme_fichier_existe(ESPACE_PROGRAMMES, "TRANSIT"));
    VERIFIER(!plateforme_fichier_existe(ESPACE_PROGRAMMES, "ANCIEN"));    /* audit du code, AC-21 */
    programme_vider();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_appeler("PROG", 0, 0));
    VERIFIER(identique());
    plateforme_fichier_ouvrir(ESPACE_DONNEES, "DATA", FICHIER_CREATION, &f);
    plateforme_fichier_fermer(f);
    VERIFIER_EGAL(ERR_F_DONNEES_EXISTANT, bibliotheque_ranger("DATA", 0, 0, 0));   /* MITRA 15 */
    /* sous les profils T1600, noms uniques au sein de chaque sorte (point B2 révisé
       le 2026-11-06 ; constaté sur LSE 8086) : le programme cohabite avec le fichier */
    configuration_session.profil = PROFIL_T1600_F4;
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("DATA", 0, 0, 0));
    VERIFIER(plateforme_fichier_existe(ESPACE_PROGRAMMES, "DATA")
             && plateforme_fichier_existe(ESPACE_DONNEES, "DATA"));
    VERIFIER_EGAL(ERR_F_PROGRAMME_EXISTANT, bibliotheque_ranger("DATA", 0, 0, 0));
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_supprimer(ESPACE_PROGRAMMES, "DATA"));
    configuration_session.profil = PROFIL_MITRA15_V8;

    /* SUPPRIMER (point B4) */
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_supprimer(ESPACE_PROGRAMMES, "PROG"));
    VERIFIER_EGAL(ERR_F_FICHIER_INEXISTANT, bibliotheque_supprimer(ESPACE_PROGRAMMES, "PROG"));
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_supprimer(ESPACE_DONNEES, "DATA"));
    /* programme présent mais refusé : F 27, non « fichier inexistant » (IMP-0249 ; audit
       du code, AC-29) ; un répertoire non vide tient lieu de fichier verrouillé */
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBLIO:PROGRAMMES:REP"));
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBLIO"));
    VERIFIER_EGAL(ERR_F_ENTREE_SORTIE, bibliotheque_supprimer(ESPACE_PROGRAMMES, "REP"));
    if (plateforme_bibliotheque_ouvrir("tests:BIBLIO:PROGRAMMES:REP") == PLAT_OK) {
        VERIFIER_EGAL(PLAT_OK, plateforme_espace_vider(ESPACE_RACINE));
    }
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBLIO"));
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_supprimer(ESPACE_PROGRAMMES, "REP"));

    /* EXECUTER α{,β} (LSE15 §5-7.3, point B5) */
    VERIFIER(charger(module));
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("MOD", 0, 0, 0));
    VERIFIER(charger(appelant));
    VERIFIER(executer(0) == FIN_TERMINER);
    VERIFIER(valeur("X") == 5.0f && valeur("Y") == 6.0f);
    VERIFIER(valeur("Z") == -998.0f);                         /* données du module appelant perdues */
    VERIFIER(charger(appelant2));
    VERIFIER(executer(0) == FIN_ERREUR && diag.nature == ERR_E_NON_DEFINI_CALCUL);   /* X non défini */
    VERIFIER(charger(appelant3));
    VERIFIER(executer(0) == FIN_ERREUR && diag.nature == ERR_E_MODULE_INTROUVABLE && diag.ligne == 10);
    VERIFIER(charger(appelant4));
    photographier();
    VERIFIER(executer(0) == FIN_ERREUR && diag.nature == ERR_E_LIGNE_INEXISTANTE && diag.ligne == 10);
    VERIFIER(identique());                                   /* appelant intact */

    /* EXECUTER interdit en machine de bureau (point B6, LSE 8086) */
    VERIFIER(!compiler("EXECUTER 'MOD'") && d.nature == ERR_C_BUREAU_EXECUTER);

    /* ligne vide (IMP-0194, INRDP75) rangée puis rappelée (audit du code, AC-08) */
    VERIFIER(charger(vide));
    photographier();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("VIDE", 0, 0, 0));
    contexte_vider();
    programme_vider();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_appeler("VIDE", 0, 0));
    VERIFIER(identique());
    VERIFIER(executer(0) == FIN_TERMINER);
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_supprimer(ESPACE_PROGRAMMES, "VIDE"));

    /* FAIRE sans variable rangé puis rappelé, refusé sous un autre profil (IMP-0277) */
    configuration_session.profil = PROFIL_T1600_F4;
    VERIFIER(charger(attente));
    photographier();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("ATTEN", 0, 0, 0));
    contexte_vider();
    programme_vider();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_appeler("ATTEN", 0, 0));
    VERIFIER(identique());
    VERIFIER(executer(0) == FIN_TERMINER && valeur("I") == 2.0f);
    configuration_session.profil = PROFIL_T1600_V4;
    VERIFIER_EGAL(ERR_F_AUTRE_PROFIL, bibliotheque_appeler("ATTEN", 0, 0));
    configuration_session.profil = PROFIL_MITRA15_V8;
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_supprimer(ESPACE_PROGRAMMES, "ATTEN"));

    /* unité amovible, « $ » (IMP-0278) : mêmes vérifications qu'à la bibliothèque */
    configuration_session.profil = PROFIL_T1600_F4;
    VERIFIER_EGAL(ERR_F_UNITE, bibliotheque_ranger("ATTEN", 0, 0, 1));
    VERIFIER_EGAL(ERR_F_UNITE, bibliotheque_appeler("ATTEN", 0, 1));
    VERIFIER_EGAL(ERR_F_UNITE, bibliotheque_supprimer(ESPACE_UNITE_PROGRAMMES, "ATTEN"));
    plateforme_unite_designer("tests:BIBLUN");
    plateforme_fichier_supprimer(ESPACE_UNITE_PROGRAMMES, "ATTEN");
    plateforme_fichier_supprimer(ESPACE_UNITE_RACINE, "CATALOGUE");
    VERIFIER(charger(attente));
    photographier();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("ATTEN", 0, 0, 1));
    VERIFIER(!plateforme_fichier_existe(ESPACE_PROGRAMMES, "ATTEN"));
    VERIFIER_EGAL(ERR_F_PROGRAMME_EXISTANT, bibliotheque_ranger("ATTEN", 0, 0, 1));
    contexte_vider();
    programme_vider();
    VERIFIER_EGAL(ERR_F_FICHIER_INEXISTANT, bibliotheque_appeler("ATTEN", 0, 0));
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_appeler("ATTEN", 0, 1));
    VERIFIER(identique());
    configuration_session.profil = PROFIL_T1600_V4;
    VERIFIER_EGAL(ERR_F_AUTRE_PROFIL, bibliotheque_appeler("ATTEN", 0, 1));
    configuration_session.profil = PROFIL_MITRA15_V8;
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_supprimer(ESPACE_UNITE_PROGRAMMES, "ATTEN"));
    VERIFIER_EGAL(ERR_F_FICHIER_INEXISTANT, bibliotheque_supprimer(ESPACE_UNITE_PROGRAMMES, "ATTEN"));
    plateforme_unite_designer(NULL);

    /* vérification complète avant de vider le programme (IMP-0246 ; audit du code,
       AC-17) : image de DBL, symboles A et B en 10 et 18, constantes de 27 caractères
       en 28 et 56, lignes à partir de 86 ; chaque fichier altéré est refusé par
       F 101, et le programme courant reste intact (VM-3403) */
    VERIFIER(charger(dbl) && symboles_nombre() == 2 && constantes_nombre() == 2);
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("DBL", 0, 0, 0));
    lire_image("DBL");
    j = 88 + original[87];                       /* ligne 20 : A_ puis la constante 0 */
    VERIFIER(original[11] == 'A' && original[19] == 'B' && original[28] == 27 && original[56] == 27
             && original[86] == 10 && original[j] == 20 && original[j + 2] == RCIM_REF_SIMPLE
             && original[j + 5] == RCIM_VAL_CHAINE && original[j + 6] == RCIM_CHAINE_INDEX);
    VERIFIER(charger(module));
    photographier();
    alterer("DBL", j + 3, 9);                    /* identifiant 9, pour 2 symboles */
    VERIFIER_EGAL(ERR_F_FICHIER_ENDOMMAGE, bibliotheque_appeler("DBL", 0, 0));
    VERIFIER(identique());
    alterer("DBL", j + 7, 5);                    /* index de constante 5, pour 2 constantes */
    VERIFIER_EGAL(ERR_F_FICHIER_ENDOMMAGE, bibliotheque_appeler("DBL", 0, 0));
    VERIFIER(identique());
    alterer("DBL", 19, 'A');                     /* deux symboles A */
    VERIFIER_EGAL(ERR_F_FICHIER_ENDOMMAGE, bibliotheque_appeler("DBL", 0, 0));
    VERIFIER(identique());
    alterer("DBL", 83, '1');                     /* deux constantes égales */
    VERIFIER_EGAL(ERR_F_FICHIER_ENDOMMAGE, bibliotheque_appeler("DBL", 0, 0));
    VERIFIER(identique());
    alterer("DBL", 11, 'a');                     /* minuscule dans un nom */
    VERIFIER_EGAL(ERR_F_FICHIER_ENDOMMAGE, bibliotheque_appeler("DBL", 0, 0));
    VERIFIER(identique());
    alterer("DBL", 12, 'X');                     /* nom non complété de zéros */
    VERIFIER_EGAL(ERR_F_FICHIER_ENDOMMAGE, bibliotheque_appeler("DBL", 0, 0));
    VERIFIER(identique());
    alterer("DBL", 16, 2);                       /* nature inconnue */
    VERIFIER_EGAL(ERR_F_FICHIER_ENDOMMAGE, bibliotheque_appeler("DBL", 0, 0));
    VERIFIER(identique());
    alterer("DBL", 19, 'C');                     /* fichier intact, au nom de B près */
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_appeler("DBL", 0, 0));
    VERIFIER(executer(0) == FIN_TERMINER && symboles_nombre() == 2 && constantes_nombre() == 2);
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_supprimer(ESPACE_PROGRAMMES, "DBL"));

    /* image RCI-S de plus de 64 Kio : 1 200 constantes partagées accumulées sur une
       même ligne (IMP-0107), RANGER puis APPELER (audit du code, AC-16) */
    contexte_vider();
    programme_vider();
    for (k = 0; k < 1200; k++) {
        lgc = 40 + k % 33;
        sprintf(ligne, "10 C_'%04u", k);
        for (j = 4; j < lgc; j++) ligne[6 + j] = (char) ('A' + (k + j) % 26);
        ligne[6 + lgc] = '\'';
        ligne[7 + lgc] = '\0';
        if (!compiler(ligne) || !programme_inserer(&r.resultat.ligne_programme, &n)) break;
    }
    VERIFIER(k == 1200 && constantes_nombre() == 1200);
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("GROS", 0, 0, 0));
    contexte_vider();
    programme_vider();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_appeler("GROS", 0, 0));
    VERIFIER(programme_nombre() == 1 && constantes_nombre() == 1200);
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_supprimer(ESPACE_PROGRAMMES, "GROS"));

    contexte_vider();
    programme_vider();
    pile_liberer(&pile);
    VERIF_FIN();
}
