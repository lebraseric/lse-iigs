/* composants_interprete.c — interpréteur : programmes compilés puis exécutés
   EXI: EXI-0011, EXI-0022 à EXI-0030, EXI-0034 à EXI-0044
   VM: VM-2205, VM-2206, VM-3101, VM-3102
   IMP: IMP-0214 à IMP-0219 ; État d'exécution IMP-0146 à IMP-0155
   Spécification des tests : chapitre Interpréteur §7 ; exemples de LSE15 §3,
   §4, §5-5, §7.
   Niveau : composants. Les résultats sont lus dans l'environnement. */

#include "compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/interprete.h"
#include "../../src/noyau/donnees.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

static PileEvaluation pile;
static DiagnosticExecution diag;
static PositionExecution pos;

/* Efface le programme et l'état, puis entre les lignes données. */
static int programme(const char *const *lignes)
{
    unsigned int i;
    NatureErreur n;
    programme_vider();
    contexte_vider();
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

/* EXECUTER {A PARTIR DE depart{,arret}} puis cycle. */
static FinExecution lancer(unsigned char depart, unsigned char arret)
{
    NatureErreur n = contexte_initialiser(depart, arret, &pos);
    if (n != ERR_AUCUNE) { diag.nature = n; return FIN_ERREUR; }
    interprete_reinitialiser();
    return interprete_executer(&pile, &pos, &diag);
}

/* Ligne de machine de bureau compilée dans r, exécutée depuis son début. */
static FinExecution bureau(void)
{
    PositionExecution b;
    b.ligne = 0;
    b.decalage = 0;
    return interprete_bureau(&pile, r.resultat.bloc_bureau, r.longueur_bureau, &b, &diag);
}

static float valeur(const char *nom)
{
    ReferenceResolue ref;
    unsigned int id;
    float v = -999.0f;
    if (!symboles_chercher(nom, (unsigned char) strlen(nom), SYM_IDENTIFICATEUR, &id)) return -998.0f;
    contexte_resoudre(id, &ref);
    contexte_lire_nombre(ref.cellule, USAGE_CALCUL, &v);
    return v;
}

static int chaine_vaut(const char *nom, const char *attendu)
{
    ReferenceResolue ref;
    unsigned int id;
    ChaineSurPile s;
    unsigned char t[300];
    unsigned int n;
    if (!symboles_chercher(nom, (unsigned char) strlen(nom), SYM_IDENTIFICATEUR, &id)) return 0;
    contexte_resoudre(id, &ref);
    if (contexte_lire_chaine(ref.cellule, USAGE_AFFECTATION, &s) != ERR_AUCUNE) return 0;
    n = donnees_texte(&s, 0, t, sizeof t);
    t[n] = 0;
    if (n != strlen(attendu) || memcmp(t, attendu, n) != 0) {
        printf("  %s = [%s], attendu [%s]\n", nom, t, attendu);
        return 0;
    }
    return 1;
}

/* Le programme s'arrête sur l'erreur attendue, à la ligne attendue. */
static int erreur_en(const char *const *lignes, NatureErreur nature, unsigned char ligne)
{
    FinExecution f;
    if (!programme(lignes)) return 0;
    f = lancer(0, 0);
    if (f != FIN_ERREUR || diag.nature != nature || diag.ligne != ligne) {
        printf("  fin %u, EX %u en ligne %u ; attendu EX %u en ligne %u\n", f,
               erreurs_numero(diag.nature), diag.ligne, erreurs_numero(nature), ligne);
        return 0;
    }
    return 1;
}

#define TERMINE(...) \
    do { static const char *const p_[] = { __VA_ARGS__, NULL }; \
         VERIFIER(programme(p_) && lancer(0, 0) == FIN_TERMINER); } while (0)
#define ERREUR(nature, ligne, ...) \
    do { static const char *const p_[] = { __VA_ARGS__, NULL }; \
         VERIFIER(erreur_en(p_, (nature), (ligne))); } while (0)

int main(void)
{
    VERIF_DEBUT("composants/interprete");
    programme_initialiser();
    pile_initialiser(&pile);

    /* --- arithmétique et priorités (LSE15 §3-3, IMP-0216) --------------------- */
    TERMINE("10 A_2+3*4;B_(2+3)*4;C_2^3^2;D_-2^2;E_7/2;F_ENT(-3.2);G_RAC(16)+ABS(-3)",
            "20 H_0^0;K_(-2)^3;L_SIN(0)+COS(0);M_EXP(0)+LGN(1);TERMINER");
    VERIFIER(valeur("A") == 14.0f);
    VERIFIER(valeur("B") == 20.0f);
    VERIFIER(valeur("C") == 64.0f);
    VERIFIER(valeur("D") == -4.0f);
    VERIFIER(valeur("E") == 3.5f);
    VERIFIER(valeur("F") == -4.0f);
    VERIFIER(valeur("G") == 7.0f);
    VERIFIER(valeur("H") == 1.0f);                      /* 0^0, LSE 8086 */
    VERIFIER(valeur("K") == -8.0f);
    VERIFIER(valeur("L") == 1.0f);
    VERIFIER(valeur("M") == 1.0f);
    ERREUR(ERR_E_DIVISION_PAR_ZERO, 10, "10 A_1/0", "20 TERMINER");
    ERREUR(ERR_E_RAC_NEGATIF, 10, "10 A_RAC(-1)", "20 TERMINER");
    ERREUR(ERR_E_LGN_NEGATIF, 10, "10 A_LGN(0)", "20 TERMINER");
    ERREUR(ERR_E_DEPASSEMENT, 10, "10 A_1E30*1E30", "20 TERMINER");
    ERREUR(ERR_E_ZERO_PUISSANCE_NEGATIVE, 10, "10 A_0^(-1)", "20 TERMINER");
    ERREUR(ERR_E_PUISSANCE_NON_ENTIERE, 10, "10 A_(-2)^0.5", "20 TERMINER");
    /* pas de FAIRE au-delà du plus grand float : EX 44 (IMP-0216 ; audit du code, AC-35) */
    ERREUR(ERR_E_DEPASSEMENT, 10, "10 FAIRE 20 POUR V_3E38 PAS 1E38 JUSQUA 3.4E38", "20 X_1",
           "30 TERMINER");
    VERIFIER(pos.ligne == 10 && pos.decalage > 0);      /* position sur l'élément en cause */
    /* variable non définie, selon l'usage (RX1) */
    ERREUR(ERR_E_NON_DEFINI_CALCUL, 10, "10 A_B+1", "20 TERMINER");
    ERREUR(ERR_E_NON_DEFINI_AFFECTATION, 10, "10 A_B", "20 TERMINER");
    ERREUR(ERR_E_NON_DEFINI_COMPARAISON, 10, "10 SI B=1 ALORS A_1", "20 TERMINER");

    /* --- SI, conditions -------------------------------------------------- */
    TERMINE("10 A_5;SI A>3 ALORS B_1 SINON B_2;C_SI A<3 ALORS 10 SINON 20",
            "20 SI A>1 ET A<9 OU A=0 ALORS D_1;SI (A=1 OU A=5) ET A#2 ALORS E_1",
            "30 TERMINER");
    VERIFIER(valeur("B") == 1.0f);
    VERIFIER(valeur("C") == 20.0f);
    VERIFIER(valeur("D") == 1.0f);
    VERIFIER(valeur("E") == 1.0f);

    /* --- chaînes (LSE15 §4) ------------------------------------------------- */
    TERMINE("10 CHAINE C,D,E;C_'AB';D_C!'CD';L_LGR(D);P_POS(D,1,'C')",
            "20 SI D='ABCD' ALORS R_1 SINON R_2;E_SCH('ABCD',2,2,J);Q_CNB('23 OU',1,K)",
            "30 SI 'ABC'<'C' ALORS S_1;T_EQN('A');E_E!CCA(3.125)!EQC(65);TERMINER");
    VERIFIER(chaine_vaut("D", "ABCD"));
    VERIFIER(valeur("L") == 4.0f);
    VERIFIER(valeur("P") == 3.0f);
    VERIFIER(valeur("R") == 1.0f);
    VERIFIER(chaine_vaut("E", "BC3.125A"));
    VERIFIER(valeur("J") == 4.0f);
    VERIFIER(valeur("Q") == 23.0f && valeur("K") == 3.0f);
    VERIFIER(valeur("S") == 1.0f);
    VERIFIER(valeur("T") == 65.0f);
    ERREUR(ERR_E_CHAINE_NON_DECLAREE, 10, "10 C_'X'", "20 TERMINER");           /* LSE15 §4-3 */
    ERREUR(ERR_E_COMPARAISON_TYPES, 10, "10 X_1;CHAINE C;C_'A';SI X=C ALORS A_1", "20 TERMINER");
    ERREUR(ERR_E_CHAINE_SANS_VALEUR, 10, "10 CHAINE C;CHAINE D;D_C", "20 TERMINER");

    /* --- tableaux (Données IMP-0210) -------------------------------------- */
    TERMINE("10 TABLEAU T[3,2];T[2,1]_5;X_T[2,1]+1;T[1.4,1.6]_7;Y_T[1,2]", "20 TERMINER");
    VERIFIER(valeur("X") == 6.0f);
    VERIFIER(valeur("Y") == 7.0f);
    ERREUR(ERR_E_ELEMENT_SANS_VALEUR, 20, "10 TABLEAU T[3]", "20 X_T[1]", "30 TERMINER");
    ERREUR(ERR_E_INDICE_TROP_GRAND, 10, "10 TABLEAU T[3];T[4]_1", "20 TERMINER");
    ERREUR(ERR_E_INDICES_SANS_TABLEAU, 10, "10 A_1;A[1]_2", "20 TERMINER");
    ERREUR(ERR_E_DECLARATION_TABLEAU, 10, "10 B_1;TABLEAU B[3]", "20 TERMINER");
    configuration_session.profil = PROFIL_T1600_V4;
    TERMINE("10 TABLEAU T[3];UN T;X_T[2]+T[3];ZER T;Y_T[1]", "20 TERMINER");     /* IMP-0218 */
    VERIFIER(valeur("X") == 2.0f && valeur("Y") == 0.0f);
    ERREUR(ERR_E_INDICES_SANS_TABLEAU, 10, "10 A_1;UN A", "20 TERMINER");
    configuration_session.profil = PROFIL_MITRA15_V8;

    /* --- FAIRE (LSE15 §5-5) ---------------------------------------------------- */
    TERMINE("10 S_0;FAIRE 20 POUR I_1 JUSQUA 10", "20 S_S+I", "30 TERMINER");
    VERIFIER(valeur("S") == 55.0f && valeur("I") == 11.0f);
    TERMINE("99 N_0", "100 FAIRE 104 POUR I_1 JUSQUA 3", "102 FAIRE 104 POUR J_1 JUSQUA 3",
            "104 N_N+1", "106 TERMINER");
    VERIFIER(valeur("N") == 9.0f);                                  /* LSE15 §5-5.5 */
    TERMINE("100 N_0;FAIRE 100 POUR I_1 JUSQUA 3;FAIRE 100 POUR J_1 JUSQUA 3;N_N+1",
            "101 TERMINER");
    VERIFIER(valeur("N") == 9.0f);
    TERMINE("5 S_0", "10 K_99;FAIRE 12 POUR K_1 TANT QUE K<=5", "12 S_S+K", "14 TERMINER");
    VERIFIER(valeur("S") == 15.0f && valeur("K") == 6.0f);          /* LSE15 §5-5.3 */
    TERMINE("1 FAIRE 3 POUR I_1 JUSQUA 5", "3 SI I=3 ALORS ALLER EN 20", "10 TERMINER",
            "20 TERMINER");
    VERIFIER(valeur("I") == 3.0f);                                  /* LSE15 §5-5.4 */
    TERMINE("10 S_0;FAIRE 20 POUR X_10 PAS -2 JUSQUA 1", "20 S_S+X", "30 TERMINER");
    VERIFIER(valeur("S") == 30.0f && valeur("X") == 0.0f);
    /* test à l'entrée de la boucle (LSE15 §5-5.3 ; MEMENTO82, d'après Éric) */
    TERMINE("10 X_0;FAIRE 20 POUR I_5 JUSQUA 1", "20 X_1", "30 TERMINER");
    VERIFIER(valeur("X") == 0.0f && valeur("I") == 5.0f);
    TERMINE("10 X_0;FAIRE 20 POUR I_5 TANT QUE I<1", "20 X_1", "30 TERMINER");
    VERIFIER(valeur("X") == 0.0f && valeur("I") == 5.0f);
    /* --- FAIRE sans variable (LSE F4, IMP-0277 ; SINUS 105) --------------------- */
    configuration_session.profil = PROFIL_T1600_F4;
    TERMINE("10 I_0", "20 FAIRE 30 TANT QUE I<3", "30 I_I+1", "40 TERMINER");
    VERIFIER(valeur("I") == 3.0f);
    TERMINE("10 I_7", "20 FAIRE 30 TANT QUE I<3", "30 I_0", "40 TERMINER");       /* fausse d'emblée */
    VERIFIER(valeur("I") == 7.0f);
    TERMINE("10 I_0;FAIRE 10 TANT QUE I<5;I_I+1", "20 TERMINER");                 /* corps sur la ligne */
    VERIFIER(valeur("I") == 5.0f);
    TERMINE("10 N_0;&P();X_N", "20 TERMINER", "100 PROCEDURE &P()",               /* forme de SINUS */
            "105 FAIRE 105 TANT QUE &F()<4", "107 RETOUR",
            "110 PROCEDURE &F()", "112 N_N+1", "114 RESULTAT N");
    VERIFIER(valeur("X") == 4.0f);
    TERMINE("5 I_0", "10 FAIRE 15 TANT QUE I<2", "12 I_I+1", "20 TERMINER");      /* fin la plus proche */
    VERIFIER(valeur("I") == 2.0f);
    TERMINE("10 S_0", "20 FAIRE 40 POUR I_1 JUSQUA 3", "25 K_0",                  /* même ligne de fin */
            "30 FAIRE 40 TANT QUE K<2", "40 K_K+1;S_S+1", "50 TERMINER");
    VERIFIER(valeur("S") == 6.0f && valeur("I") == 4.0f);
    TERMINE("10 I_0", "20 FAIRE 30 TANT QUE I<3", "30 I_I+1;ALLER EN 20", "40 TERMINER");  /* réinitialisation */
    VERIFIER(valeur("I") == 3.0f);
    TERMINE("10 I_0", "20 FAIRE 30 TANT QUE 1=1", "25 I_I+1;SI I=3 ALORS ALLER EN 40",     /* sortie extraordinaire */
            "30 ", "40 TERMINER");
    VERIFIER(valeur("I") == 3.0f);
    ERREUR(ERR_E_FIN_BOUCLE_AVANT, 20, "10 X_1", "20 FAIRE 10 TANT QUE 1=1", "30 TERMINER");
    ERREUR(ERR_E_FIN_BOUCLE_NULLE, 10, "10 FAIRE 0 TANT QUE 1=1", "20 TERMINER");
    configuration_session.profil = PROFIL_MITRA15_V8;

    /* --- branchements, arrêts (IMP-0153, IMP-0154, IMP-0217) ------------------- */
    TERMINE("10 ALLER EN 30", "20 A_1", "30 B_2;TERMINER");
    VERIFIER(valeur("A") == -999.0f && valeur("B") == 2.0f);
    ERREUR(ERR_E_LIGNE_INEXISTANTE, 10, "10 ALLER EN 99", "20 TERMINER");
    ERREUR(ERR_E_LIGNE_NULLE, 10, "10 N_0;ALLER EN N", "20 TERMINER");
    ERREUR(ERR_E_LIGNE_NON_ENTIERE, 10, "10 N_1E6;ALLER EN N", "20 TERMINER");
    ERREUR(ERR_E_LIGNE_INEXISTANTE, 10, "10 N_300;ALLER EN N", "20 TERMINER");
    ERREUR(ERR_E_FIN_DE_PROGRAMME, 20, "10 A_1", "20 B_2");
    ERREUR(ERR_E_LIGNE_DECLARATION, 20, "10 A_1", "20 PROCEDURE &P();RETOUR");    /* IMP-0155 */
    {
        static const char *const p_[] = { "10 A_1", "20 A_2", "30 TERMINER", NULL };
        VERIFIER(programme(p_) && lancer(0, 20) == FIN_ARRET && pos.ligne == 20);
        VERIFIER(valeur("A") == 1.0f);                              /* ligne 20 non exécutée */
        VERIFIER(lancer(10, 10) == FIN_ARRET);
    }
    {
        static const char *const p_[] = { "10 A_1;PAUSE;A_2", "20 TERMINER", NULL };
        VERIFIER(programme(p_) && lancer(0, 0) == FIN_PAUSE && valeur("A") == 1.0f);
        VERIFIER(interprete_executer(&pile, &pos, &diag) == FIN_TERMINER && valeur("A") == 2.0f);
    }

    /* --- procédures (LSE15 §7) -------------------------------------------------- */
    TERMINE("10 R_&FACT(7)", "20 TERMINER", "25 PROCEDURE &FACT(N) LOCAL N",
            "27 RESULTAT SI N=0 ALORS 1 SINON N*&FACT(N-1)");
    VERIFIER(valeur("R") == 5040.0f);                               /* LSE15 §7-6 */
    TERMINE("10 X_1;&INC(X);&INC(X)", "20 TERMINER", "30 PROCEDURE &INC(V);V_V+1;RETOUR");
    VERIFIER(valeur("X") == 3.0f);                                  /* paramètre référence */
    TERMINE("10 A_1;&P(A)", "20 TERMINER", "30 PROCEDURE &P(B) LOCAL A;A_2;B_3;RETOUR");
    VERIFIER(valeur("A") == 3.0f);                                  /* point D1, LSE 8086 */
    TERMINE("10 Y_5;Y_&RAC(-4,30)", "20 TERMINER", "30 Z_1;TERMINER",
            "40 PROCEDURE &RAC(ARG,ERRE) LOCAL ARG,ERRE", "41 SI ARG<0 ALORS RETOUR EN ERRE",
            "42 RESULTAT RAC(ARG)");
    VERIFIER(valeur("Y") == 5.0f && valeur("Z") == 1.0f);           /* LSE15 §7-5 */
    VERIFIER_EGAL(0, pile.sommet);                                  /* expression abandonnée */
    TERMINE("10 Y_&RAC(16,30)+1", "20 TERMINER", "40 PROCEDURE &RAC(ARG,ERRE) LOCAL ARG,ERRE",
            "41 SI ARG<0 ALORS RETOUR EN ERRE", "42 RESULTAT RAC(ARG)");
    VERIFIER(valeur("Y") == 5.0f);
    ERREUR(ERR_E_RETOUR_DANS_FONCTION, 30, "10 Y_&F(1)", "20 TERMINER", "30 PROCEDURE &F(A) LOCAL A;RETOUR");
    ERREUR(ERR_E_RESULTAT_SOUS_PROGRAMME, 30, "10 &G(1)", "20 TERMINER", "30 PROCEDURE &G(A) LOCAL A;RESULTAT A");
    ERREUR(ERR_E_NOMBRE_PARAMETRES, 10, "10 Y_&F(1,2)", "20 TERMINER", "30 PROCEDURE &F(A) LOCAL A;RESULTAT A");
    ERREUR(ERR_E_PARAMETRE_REFERENCE, 10, "10 Y_&F(1)", "20 TERMINER", "30 PROCEDURE &F(A);RESULTAT A");
    ERREUR(ERR_E_PROCEDURE_NON_DECLAREE, 10, "10 &Q(1)", "20 TERMINER");
    ERREUR(ERR_E_RETOUR_HORS_PROCEDURE, 10, "10 RETOUR", "20 TERMINER");
    /* élément de tableau rendu par RESULTAT ou passé à un paramètre LOCAL (§5.5 ; audit du code, AC-04) */
    TERMINE("10 TABLEAU T[3];T[2]_7;X_&F(2);Y_&G(T[2]);Z_T[2]", "20 TERMINER",
            "30 PROCEDURE &F(N) LOCAL N;RESULTAT T[N]", "40 PROCEDURE &G(A) LOCAL A;A_A+1;RESULTAT A");
    VERIFIER(valeur("X") == 7.0f && valeur("Y") == 8.0f && valeur("Z") == 7.0f);
    ERREUR(ERR_E_ELEMENT_SANS_VALEUR, 30, "10 TABLEAU T[3];X_&F(1)", "20 TERMINER",
           "30 PROCEDURE &F(N) LOCAL N;RESULTAT T[N]");
    ERREUR(ERR_E_PARAMETRE_SANS_VALEUR, 40, "10 TABLEAU T[3];Y_&G(T[1])", "20 TERMINER",
           "40 PROCEDURE &G(A) LOCAL A;RESULTAT A+1");                  /* RX3 */
    /* récursion profonde : pile d'évaluation et cases plafonnées, EX 03 au-delà
       (IMP-0145, IMP-0179 ; LSE 8086 : « Mémoire pleine » ; audit du code, AC-06, AC-07) */
    TERMINE("10 X_&F(9000)", "20 TERMINER", "30 PROCEDURE &F(N) LOCAL N",
            "40 SI N=0 ALORS RESULTAT 0", "50 RESULTAT 1+&F(N-1)");
    VERIFIER(valeur("X") == 9000.0f);
    {
        static const char *const p_[] = { "10 X_&F(15000)", "20 TERMINER", "30 PROCEDURE &F(N) LOCAL N",
                                          "40 SI N=0 ALORS RESULTAT 0", "50 RESULTAT 1+&F(N-1)", NULL };
        VERIFIER(programme(p_) && lancer(0, 0) == FIN_ERREUR && diag.nature == ERR_E_MEMOIRE_SATUREE);
    }
    TERMINE("10 K_0;&P(12000)", "20 TERMINER", "30 PROCEDURE &P(N) LOCAL N",
            "40 K_K+1;SI N>0 ALORS &P(N-1)", "50 RETOUR");
    VERIFIER(valeur("K") == 12001.0f);
    {
        static const char *const p_[] = { "10 K_0;&P(14000)", "20 TERMINER", "30 PROCEDURE &P(N) LOCAL N",
                                          "40 K_K+1;SI N>0 ALORS &P(N-1)", "50 RETOUR", NULL };
        VERIFIER(programme(p_) && lancer(0, 0) == FIN_ERREUR && diag.nature == ERR_E_MEMOIRE_SATUREE);
    }

    /* --- machine de bureau ------------------------------------------------------ */
    {
        static const char *const p_[] = { "10 A_1", "20 TERMINER", NULL };
        VERIFIER(programme(p_) && lancer(0, 0) == FIN_TERMINER);
        VERIFIER(compiler("B_A+2") && r.type == LSE_LIGNE_BUREAU);
        VERIFIER(bureau() == FIN_BUREAU);
        objets_liberer(r.resultat.bloc_bureau);
        VERIFIER(valeur("B") == 3.0f);
        VERIFIER(compiler("B_1/0"));
        VERIFIER(bureau() == FIN_ERREUR);
        VERIFIER(diag.nature == ERR_E_DIVISION_PAR_ZERO && diag.ligne == 0);   /* EN LIGNE BUR */
        objets_liberer(r.resultat.bloc_bureau);
    }

    /* --- instruction de fichier sans bibliothèque ouverte (étape 7c) ----------------- */
    ERREUR(ERR_E_FICHIER_INEXISTANT, 10, "10 SUPPRIMER 'F'", "20 TERMINER");

    contexte_vider();
    programme_vider();
    pile_liberer(&pile);
    VERIF_FIN();
}
