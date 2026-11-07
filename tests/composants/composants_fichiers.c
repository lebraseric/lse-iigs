/* composants_fichiers.c — fichiers-données : GARER, CHARGER, SUPPRIMER,
   CATALOGUER, espace temporaire
   EXI: EXI-0053 à EXI-0057  VM: VM-3405, VM-3407
   IMP: IMP-0247, IMP-0253 ; points C1 à C7 (étape 7c)
   Spécification des tests : chapitre Bibliothécaire et fichiers §4, §7 ter ;
   exemples de LSE15 §6-3.3 et §6-3.4.
   Niveau : composants, sur fichiers réels (bibliothèque tests:BIBFICH). */

#include "compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/interprete.h"
#include "../../src/noyau/contexte_exec.h"
#include "../../src/noyau/donnees.h"
#include "../../src/noyau/fichiers.h"
#include "../../src/noyau/bibliotheque.h"
#include "../../src/interfaces/interface_fichiers.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

void bouchon_console(const char *const *lignes);

static PileEvaluation pile;
static DiagnosticExecution diag;
static PositionExecution pos;
static const char *const aucune[] = { NULL };

/* Compile et exécute ; rend la fin d'exécution (99 : rejet). */
static FinExecution executer(const char *const *lignes)
{
    unsigned int i;
    NatureErreur n;
    contexte_vider();
    programme_vider();
    pile_ramener(&pile, 0);
    for (i = 0; lignes[i] != NULL; i++) {
        if (!compiler(lignes[i])) {
            printf("  [%s] rejet : C %u\n", lignes[i], erreurs_numero(d.nature));
            return 99;
        }
        if (!programme_inserer(&r.resultat.ligne_programme, &n)) return 99;
    }
    contexte_initialiser(0, 0, &pos);
    interprete_reinitialiser();
    return interprete_executer(&pile, &pos, &diag);
}

static float valeur(const char *nom)
{
    ReferenceResolue ref;
    unsigned int id;
    float v = -999.0f;
    if (!symboles_chercher(nom, (unsigned char) strlen(nom), SYM_IDENTIFICATEUR, &id)) return -998.0f;
    if (contexte_resoudre(id, &ref) != ERR_AUCUNE) return -997.0f;
    if (contexte_lire_nombre(ref.cellule, USAGE_CALCUL, &v) != ERR_AUCUNE) return -996.0f;
    return v;
}

static int chaine_vaut(const char *nom, const char *attendu)
{
    ReferenceResolue ref;
    unsigned int id;
    ChaineSurPile s;
    unsigned char t[700];
    unsigned int n;
    if (!symboles_chercher(nom, (unsigned char) strlen(nom), SYM_IDENTIFICATEUR, &id)) return 0;
    contexte_resoudre(id, &ref);
    if (contexte_lire_chaine(ref.cellule, USAGE_AFFECTATION, &s) != ERR_AUCUNE) return 0;
    n = donnees_texte(&s, 0, t, sizeof t);
    return n == strlen(attendu) && memcmp(t, attendu, n) == 0;
}

static int termine(const char *const *lignes)
{
    FinExecution f = executer(lignes);
    if (f == FIN_TERMINER) return 1;
    printf("  fin %u, EX %u ligne %u\n", f, erreurs_numero(diag.nature), diag.ligne);
    return 0;
}

static int erreur(const char *const *lignes, NatureErreur attendue)
{
    if (executer(lignes) == FIN_ERREUR && diag.nature == attendue) return 1;
    printf("  nature %u au lieu de %u\n", diag.nature, attendue);
    return 0;
}

#define TERMINE(...) do { static const char *const p_[] = { __VA_ARGS__, NULL }; VERIFIER(termine(p_)); } while (0)
#define ERREUR(n, ...) do { static const char *const p_[] = { __VA_ARGS__, NULL }; VERIFIER(erreur(p_, (n))); } while (0)

int main(void)
{
    float f371 = 3.71f;
    unsigned long avant;

    VERIF_DEBUT("composants/fichiers");
    programme_initialiser();
    pile_initialiser(&pile);
    bouchon_console(aucune);
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBFICH"));
    plateforme_espace_vider(ESPACE_TEMPORAIRES);
    plateforme_espace_vider(ESPACE_DONNEES);
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
    {                                             /* identification 1 : P, D, S, X (IMP-0254) */
        Identification id;
        bibliotheque_identification_chercher(1, &id);
        bibliotheque_identifier(&id);
    }

    /* LSE15 §6-3.3 et §6-3.4, exemples 1 et 2 */
    TERMINE("1 TABLEAU A[4];A[1]_-2;A[3]_3.71", "2 GARER A,2,'ES'",
            "3 CHAINE X33;X33_'MATINAL';GARER X33,3,'ES'", "4 TERMINER");
    TERMINE("1 CHARGER ZZZ,2,'ES';Z1_ZZZ[1];Z3_ZZZ[3]", "2 CHARGER ARTUS,3,'ES'", "3 TERMINER");
    VERIFIER(valeur("Z1") == -2.0f && valeur("Z3") == f371);
    VERIFIER(chaine_vaut("ARTUS", "MATINAL"));
    ERREUR(ERR_E_ELEMENT_SANS_VALEUR, "1 CHARGER ZZZ,2,'ES';Z2_ZZZ[2]+0", "2 TERMINER");   /* non défini conservé */

    /* compte rendu δ (LSE15 §6-3.4) */
    TERMINE("1 CHARGER Z,9,'ES',D1", "2 CHARGER Z,1,'XX',D2", "3 CHARGER Y,3,'ES',D3",
            "4 CHARGER W,2,'ES',D4", "5 TERMINER");
    VERIFIER(valeur("D1") == -1.0f && valeur("D2") == -2.0f && valeur("D3") == 3.0f
             && valeur("D4") == 1.0f && chaine_vaut("Y", "MATINAL"));
    ERREUR(ERR_E_ENREGISTREMENT_INEXISTANT, "1 CHARGER Z,9,'ES'", "2 TERMINER");
    ERREUR(ERR_E_FICHIER_INEXISTANT, "1 CHARGER Z,1,'XX'", "2 TERMINER");
    ERREUR(ERR_E_TYPE_CHARGER, "1 CHAINE C;CHARGER C,2,'ES'", "2 TERMINER");

    /* variable simple, remplacement par un contenu plus long (plusieurs secteurs) */
    TERMINE("1 V_42;GARER V,5,'ES';CHARGER V2,5,'ES'", "2 TERMINER");
    VERIFIER(valeur("V2") == 42.0f);
    TERMINE("1 CHAINE L;L_'ABCDEFGHIJ';FAIRE 2 POUR I_1 JUSQUA 6", "2 L_L!L",
            "3 GARER L,3,'ES';CHARGER M,3,'ES';N_LGR(M)", "4 TERMINER");
    VERIFIER(valeur("N") == 640.0f);

    /* tableau dont l'image dépasse 64 Kio (82 506 octets) : GARER puis CHARGER (audit du code, AC-05) */
    TERMINE("1 TABLEAU T[20000];T[1]_5;T[16000]_9;T[20000]_7", "2 GARER T,1,'#GT';CHARGER U,1,'#GT'",
            "3 A_U[1];B_U[16000];C_U[20000]", "4 TERMINER");
    VERIFIER(valeur("A") == 5.0f && valeur("B") == 9.0f && valeur("C") == 7.0f);

    /* remplacement d'un enregistrement par une étendue libre reprise : l'ancienne
       étendue devient libre, le fichier ne grossit pas (IMP-0247 ; audit du code, AC-18) */
    TERMINE("1 CHAINE L;L_'ABCDEFGHIJ';FAIRE 2 POUR I_1 JUSQUA 6", "2 L_L!L",
            "3 GARER L,1,'RE';A_5;GARER A,2,'RE';SUPPRIMER 'RE',1", "4 TERMINER");
    avant = fichiers_secteurs(0, "RE");
    TERMINE("1 CHAINE L;L_'ABCDEFGHIJ';FAIRE 2 POUR I_1 JUSQUA 6", "2 L_L!L",
            "3 GARER L,2,'RE';CHARGER M,2,'RE';N_LGR(M)", "4 TERMINER");
    VERIFIER(valeur("N") == 640.0f && fichiers_secteurs(0, "RE") == avant);

    /* étendue libre de 3 secteurs découpée : l'enregistrement 2 (300 caractères) en prend
       2, puis les enregistrements 3 et 4 prennent le reste et l'ancienne étendue ; le
       fichier ne grossit pas (IMP-0247 ; audit du code, AC-19) */
    TERMINE("1 CHAINE L;L_'ABCDEFGHIJ';FAIRE 2 POUR I_1 JUSQUA 6", "2 L_L!L",
            "3 GARER L,1,'DC';A_5;GARER A,2,'DC';SUPPRIMER 'DC',1", "4 TERMINER");
    avant = fichiers_secteurs(0, "DC");
    TERMINE("1 CHAINE L;L_'ABCDEFGHIJ';FAIRE 2 POUR I_1 JUSQUA 6", "2 L_L!L",
            "3 L_SCH(L,1,300);GARER L,2,'DC';A_3;GARER A,3,'DC';A_4;GARER A,4,'DC'", "4 TERMINER");
    VERIFIER(fichiers_secteurs(0, "DC") == avant);
    TERMINE("1 CHARGER M,2,'DC';N_LGR(M);CHARGER P,3,'DC';CHARGER Q,4,'DC'", "2 TERMINER");
    VERIFIER(valeur("N") == 300.0f && valeur("P") == 3.0f && valeur("Q") == 4.0f);

    /* enregistrement de longueur nulle remplacé dans une étendue libre reprise : l'étendue
       cesse d'être décrite comme libre, et les GARER suivants, nouveaux enregistrements
       découpant le reste, ne l'écrasent pas (AC-19) */
    TERMINE("1 CHAINE L,V;L_'ABCDEFGHIJ';FAIRE 2 POUR I_1 JUSQUA 6", "2 L_L!L",
            "3 GARER L,1,'ZL';V_'';GARER V,2,'ZL';SUPPRIMER 'ZL',1", "4 TERMINER");
    avant = fichiers_secteurs(0, "ZL");
    TERMINE("1 CHAINE V;V_'XYZ';GARER V,2,'ZL';V_'ABC';GARER V,3,'ZL';A_4;GARER A,4,'ZL'",
            "2 CHARGER M,2,'ZL';CHARGER N,3,'ZL';CHARGER P,4,'ZL'", "3 TERMINER");
    VERIFIER(chaine_vaut("M", "XYZ") && chaine_vaut("N", "ABC") && valeur("P") == 4.0f);
    VERIFIER(fichiers_secteurs(0, "ZL") == avant);

    /* β et γ incorrects (points C2, C7) */
    ERREUR(ERR_E_NUMERO_ENREGISTREMENT, "1 A_1;GARER A,0,'ES'", "2 TERMINER");
    ERREUR(ERR_E_NUMERO_ENREGISTREMENT, "1 A_1;GARER A,1.5,'ES'", "2 TERMINER");
    ERREUR(ERR_E_NOM_FICHIER, "1 A_1;GARER A,1,'E S'", "2 TERMINER");
    ERREUR(ERR_E_NOM_FICHIER, "1 A_1;GARER A,1,'TROPLONG'", "2 TERMINER");

    /* SUPPRIMER (point C5 ; LSE 8086 : fichier inexistant) */
    TERMINE("1 SUPPRIMER 'ES',2", "2 CHARGER Z,2,'ES',D", "3 TERMINER");
    VERIFIER(valeur("D") == -1.0f);
    avant = fichiers_secteurs(0, "ES");
    TERMINE("1 A_7;GARER A,8,'ES'", "2 TERMINER");          /* étendue libre réutilisée */
    VERIFIER(fichiers_secteurs(0, "ES") == avant);
    ERREUR(ERR_E_ENREGISTREMENT_INEXISTANT, "1 SUPPRIMER 'ES',2", "2 TERMINER");
    TERMINE("1 SUPPRIMER 'ES'", "2 CHARGER Z,3,'ES',D", "3 TERMINER");
    VERIFIER(valeur("D") == -2.0f);
    ERREUR(ERR_E_FICHIER_INEXISTANT, "1 SUPPRIMER 'ZZZ',1", "2 TERMINER");

    /* fichiers permanents et priorité au temporaire (point C3 ; INRDP75 §1-4 b) */
    TERMINE("1 A_2;GARER A,1,'#P2';A_1;GARER A,1,'P2'", "2 CHARGER X,1,'P2';CHARGER Y,1,'#P2'",
            "3 TERMINER");
    VERIFIER(valeur("X") == 1.0f && valeur("Y") == 2.0f);
    VERIFIER(plateforme_fichier_existe(ESPACE_DONNEES, "P2"));

    /* plus de 20 enregistrements : répertoire chaîné (IMP-0247) */
    TERMINE("1 FAIRE 2 POUR I_1 JUSQUA 45", "2 GARER I,I,'MANY'", "3 CHARGER P,1,'MANY';CHARGER Q,45,'MANY'",
            "4 TERMINER");
    VERIFIER(valeur("P") == 1.0f && valeur("Q") == 45.0f);

    /* espace temporaire (point C1) */
    fichiers_fixer_quota(fichiers_secteurs(0, "MANY") + fichiers_secteurs(0, "P2") + 1);
    ERREUR(ERR_E_FICHIER_PLEIN, "1 CHAINE L;L_'ABCDEFGHIJ';FAIRE 2 POUR I_1 JUSQUA 6", "2 L_L!L",
           "3 GARER L,1,'GROS'", "4 TERMINER");
    /* une écriture qui n'agrandit pas le fichier échappe au quota (IMP-0253 ; audit du code, AP-15) */
    TERMINE("1 SUPPRIMER 'MANY',30", "2 TERMINER");
    fichiers_fixer_quota(1);
    TERMINE("1 A_46;GARER A,46,'MANY'", "2 TERMINER");
    fichiers_fixer_quota(FICHIERS_QUOTA_DEFAUT);

    /* CATALOGUER (point C6) */
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_cataloguer("MANY", "MANYP", 0, 0));
    VERIFIER(!plateforme_fichier_existe(ESPACE_TEMPORAIRES, "MANY"));
    TERMINE("1 CHARGER P,7,'MANYP';CHARGER Q,45,'MANYP'", "2 TERMINER");
    VERIFIER(valeur("P") == 7.0f && valeur("Q") == 45.0f);
    VERIFIER_EGAL(ERR_F_DONNEES_EXISTANT, fichiers_cataloguer("P2", "MANYP", 0, 0));
    VERIFIER_EGAL(ERR_F_FICHIER_INEXISTANT, fichiers_cataloguer("ABSEN", "NEUF", 0, 0));
    TERMINE("1 A_1", "2 TERMINER");
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("PROG", 0, 0, 0));
    VERIFIER_EGAL(ERR_F_PROGRAMME_EXISTANT, fichiers_cataloguer("P2", "PROG", 0, 0));   /* point B2 */
    /* MITRA 15 : GARER '#…' refuse aussi le nom d'un programme, sans créer le fichier
       (point B2 révisé le 2026-11-06) */
    ERREUR(ERR_F_PROGRAMME_EXISTANT, "1 A_1;GARER A,1,'#PROG'", "2 TERMINER");
    VERIFIER(!plateforme_fichier_existe(ESPACE_DONNEES, "PROG"));
    /* profils T1600 : noms uniques au sein de chaque sorte (constaté sur LSE 8086) */
    configuration_session.profil = PROFIL_T1600_F4;
    TERMINE("1 A_5;GARER A,1,'#PROG';CHARGER B,1,'#PROG'", "2 TERMINER");
    VERIFIER(valeur("B") == 5.0f && plateforme_fichier_existe(ESPACE_PROGRAMMES, "PROG"));
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_supprimer("PROG", 1, 0));
    TERMINE("1 A_3;GARER A,1,'T3'", "2 TERMINER");
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_cataloguer("T3", "PROG", 0, 0));
    VERIFIER(plateforme_fichier_existe(ESPACE_DONNEES, "PROG")
             && plateforme_fichier_existe(ESPACE_PROGRAMMES, "PROG"));
    VERIFIER_EGAL(ERR_F_DONNEES_EXISTANT, fichiers_cataloguer("P2", "PROG", 0, 0));
    configuration_session.profil = PROFIL_MITRA15_V8;

    /* fichier de l'unité, « $ » (IMP-0278) : permanent, avec le catalogue de l'unité */
    {
        static const unsigned char octets[4] = { 1, 2, 3, 4 };
        unsigned char type;
        unsigned char *lu;
        unsigned long lg;

        VERIFIER_EGAL(ERR_F_UNITE, fichiers_garer("UN", FICHIERS_UNITE, 1, ENREG_CHAINE, octets, 4));
        VERIFIER_EGAL(ERR_F_UNITE, fichiers_charger("UN", FICHIERS_UNITE, 1, &type, &lu, &lg));
        VERIFIER_EGAL(ERR_F_UNITE, fichiers_supprimer("UN", FICHIERS_UNITE, 0));
        VERIFIER_EGAL(ERR_F_UNITE, fichiers_cataloguer("P2", "UN", 0, 1));
        plateforme_unite_designer("tests:BIBFUN");
        fichiers_supprimer("UN", FICHIERS_UNITE, 0);              /* essai précédent */
        VERIFIER_EGAL(ERR_AUCUNE, fichiers_garer("UN", FICHIERS_UNITE, 1, ENREG_CHAINE, octets, 4));
        VERIFIER(plateforme_fichier_existe(ESPACE_UNITE_DONNEES, "UN"));
        VERIFIER(!plateforme_fichier_existe(ESPACE_DONNEES, "UN"));
        VERIFIER_EGAL(ERR_AUCUNE, fichiers_charger("UN", FICHIERS_UNITE, 1, &type, &lu, &lg));
        VERIFIER(lg == 4 && lu[2] == 3);
        free(lu);
        VERIFIER_EGAL(ERR_E_FICHIER_INEXISTANT, fichiers_charger("UN", 1, 1, &type, &lu, &lg));
        VERIFIER_EGAL(ERR_AUCUNE, fichiers_supprimer("UN", FICHIERS_UNITE, 0));
        VERIFIER_EGAL(ERR_E_FICHIER_INEXISTANT, fichiers_supprimer("UN", FICHIERS_UNITE, 0));
        plateforme_unite_designer(NULL);
    }

    /* fichier présent mais refusé : F 27, non « fichier inexistant » (IMP-0249 ; audit
       du code, AC-29) ; un répertoire non vide tient lieu de fichier verrouillé */
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBFICH:TEMPORAIRES:REP"));
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBFICH"));
    ERREUR(ERR_F_ENTREE_SORTIE, "1 SUPPRIMER 'REP'", "2 TERMINER");
    ERREUR(ERR_F_ENTREE_SORTIE, "1 SUPPRIMER 'REP',1", "2 TERMINER");
    ERREUR(ERR_F_ENTREE_SORTIE, "1 A_1;GARER A,1,'REP'", "2 TERMINER");
    if (plateforme_bibliotheque_ouvrir("tests:BIBFICH:TEMPORAIRES:REP") == PLAT_OK) {
        VERIFIER_EGAL(PLAT_OK, plateforme_espace_vider(ESPACE_RACINE));
    }
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBFICH"));
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_supprimer(ESPACE_TEMPORAIRES, "REP"));

    contexte_vider();
    programme_vider();
    pile_liberer(&pile);
    VERIF_FIN();
}
