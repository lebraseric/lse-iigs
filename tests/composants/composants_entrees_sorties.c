/* composants_entrees_sorties.c — AFFICHER, ?, LIRE
   EXI: EXI-0045, EXI-0046  VM: VM-2205, VM-2206
   IMP: IMP-0211, IMP-0220 à IMP-0222, IMP-0275, IMP-0280, IMP-0282
   Spécification des tests : chapitre Interpréteur §4 bis ; exemples de
   LSE15 §6-1, §6-2 et d'INRDP75 (PG4, PG5) ; réels du T1600 (Interpréteur
   §3.1 bis : témoins d'INRDP75 et sortie de NBOR, Victor Duruy).
   Niveau : composants. Console : tests/bouchons/console_bouchon.c, qui note
   le retour chariot « \r », l'interligne « \n », le bip « \a » et l'écho de
   chaque saisie suivi de « \r\n ». */

#include "compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/interprete.h"
#include "../../src/noyau/donnees.h"
#include "../../src/interfaces/interface_console.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

extern char bouchon_affichage[];
void bouchon_console(const char *const *lignes);

static PileEvaluation pile;
static DiagnosticExecution diag;
static PositionExecution pos;
static const char *const aucune[] = { NULL };

/* Exécution jusqu'à une fin autre que FIN_LECTURE ; chaque ligne demandée
   est lue sur la console et fournie, comme le fait le moniteur (IMP-0237). */
static FinExecution executer_avec_saisies(void)
{
    FinExecution f;
    char ligne[256];
    unsigned int n;
    unsigned char fin;

    while ((f = interprete_executer(&pile, &pos, &diag)) == FIN_LECTURE) {
        n = console_lire_ligne(ligne, sizeof ligne, &fin);
        interprete_fournir_saisie(ligne, n, fin);
    }
    return f;
}

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

/* Exécute le programme avec les réponses données ; compare l'affichage. */
static int sortie(const char *const *lignes, const char *const *reponses, FinExecution fin,
                  const char *attendu)
{
    FinExecution f;
    if (!programme(lignes)) return 0;
    bouchon_console(reponses);
    interprete_debut_de_ligne(1);              /* comme après la commande EXECUTER */
    contexte_initialiser(0, 0, &pos);
    interprete_reinitialiser();
    f = executer_avec_saisies();
    if (f != fin) {
        printf("  fin %u (EX %u ligne %u), attendu %u\n", f, erreurs_numero(diag.nature), diag.ligne, fin);
        return 0;
    }
    if (attendu != NULL && strcmp(bouchon_affichage, attendu) != 0) {
        printf("  affichage [%s]\n    attendu [%s]\n", bouchon_affichage, attendu);
        return 0;
    }
    return 1;
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
    return n == strlen(attendu) && memcmp(t, attendu, n) == 0;
}

#define SORTIE(attendu, ...) \
    do { static const char *const p_[] = { __VA_ARGS__, NULL }; \
         VERIFIER(sortie(p_, aucune, FIN_TERMINER, (attendu))); } while (0)
#define DIALOGUE(reponses, attendu, ...) \
    do { static const char *const p_[] = { __VA_ARGS__, NULL }; \
         VERIFIER(sortie(p_, (reponses), FIN_TERMINER, (attendu))); } while (0)

int main(void)
{
    VERIF_DEBUT("composants/entrees-sorties");
    programme_initialiser();
    pile_initialiser(&pile);

    /* --- AFFICHER sans format (LSE15 §6-2.4, IMP-0221) --------------------------- */
    SORTIE("\r\n0.6666667 ", "10 AFFICHER 2/3", "20 TERMINER");                 /* U, 7 chiffres */
    SORTIE("\r\n1692 -34.227 1.04006E+07 ", "10 AFFICHER 1692,-34.227,10400600", "20 TERMINER");
    SORTIE("\r\nAB", "10 AFFICHER 'A','B'", "20 TERMINER");                  /* chaîne sans espace */

    /* --- AFFICHER avec format (LSE15 §6-2.1 à §6-2.6) ------------------------------ */
    SORTIE("\r\n34  -5.262500E+01", "10 I_34;TABLEAU A[40];A[I]_-52.625",
           "20 AFFICHER[/,F0.0,E4.6] I,A[I]", "30 TERMINER");
    SORTIE("\r\nLA SOMME DES 10PREMIERS NOMBRES EST:55 ", "10 N_10;S_55",
           "20 AFFICHER[/,'LA SOMME DES',F3.0,'PREMIERS NOMBRES EST:'] N,S", "30 TERMINER");
    SORTIE(" 1 2 3  4.000000E+005 15 FIN", "10 A_1;B_2;C_3;D_4;E_5",
           "20 AFFICHER[3F2.0,E3.6] A,B,C,D,E,A+B+C+D+E,'FIN'", "30 TERMINER");
    SORTIE("AUJOURD'HUI*****TRACE*****", "10 AFFICHER['AUJOURD''HUI'];AFFICHER[5'*','TRACE',5'*']",
           "20 TERMINER");
    SORTIE("            5.700", "10 NBSP_12;RESUL_5.7", "20 AFFICHER[*X,F0.3] NBSP,RESUL",
           "30 TERMINER");
    SORTIE("5.700", "10 NBSP_0;RESUL_5.7", "20 AFFICHER[*X,F0.3] NBSP,RESUL", "30 TERMINER");
    SORTIE("\r\n                         RESULTATS\r\n\r\n\r\n",
           "10 AFFICHER[/,25X,'RESULTATS',3/]", "20 TERMINER");
    SORTIE("A\rB\nC", "10 AFFICHER['A',C,'B',L,'C']", "20 TERMINER");          /* IMP-0220 */
    {
        static const char *const p_[] = { "10 N_-1;AFFICHER[*'X'] N", "20 TERMINER", NULL };
        VERIFIER(sortie(p_, aucune, FIN_ERREUR, NULL) && diag.nature == ERR_E_FACTEUR_NEGATIF);
    }
    {
        static const char *const p_[] = { "10 AFFICHER[F2.0] 'A'", "20 TERMINER", NULL };
        VERIFIER(sortie(p_, aucune, FIN_ERREUR, NULL) && diag.nature == ERR_E_VALEUR_FORMAT);
    }

    /* --- tableau entier (LSE15 §6-2.5) ------------------------------------------- */
    SORTIE("A\r\n 1 2\r\n 3 4\r\n", "10 TABLEAU T[2,2];T[1,1]_1;T[1,2]_2;T[2,1]_3;T[2,2]_4",
           "20 AFFICHER['A'];AFFICHER[F2.0] T", "30 TERMINER");
    /* passage à la ligne avant le tableau seulement hors du début de ligne (LSE 8086) */
    SORTIE("\r\n1 \r\n2 \r\nX", "10 TABLEAU T[2];T[1]_1;T[2]_2;AFFICHER T;AFFICHER['X']",
           "20 TERMINER");
    SORTIE("\r\nA\r\n1 \r\n2 \r\nX", "10 TABLEAU T[2];T[1]_1;T[2]_2;AFFICHER[/,'A',/] T;AFFICHER['X']",
           "20 TERMINER");

    /* --- INRDP75, PG4 et PG5 --------------------------------------------------------- */
    {
        static const char *const rep_[] = { "2354", "-967", NULL };
        DIALOGUE(rep_, "\r\nA=\a2354\r\n\r\nB=\a-967\r\n\r\nA+B=1387 \r\nTOTO=2.2012E+09 A/B=-2.434333 ",
                 "10 AFFICHER 'A=';LIRE A", "12 AFFICHER 'B=';LIRE B", "13 AFFICHER 'A+B=',A+B",
                 "15 AFFICHER 'TOTO=',A*B^2,'A/B=',A/B", "17 TERMINER");
    }
    {
        static const char *const rep_[] = { "CORALIE\x13", NULL };       /* X-OFF (IMP-0238) */
        DIALOGUE(rep_, "\r\nTAPEZ VOTRE PRENOM : \aCORALIE\r\nBON COURAGE, CORALIE, POUR L'ETUDE DU L S E !",
                 "1 CHAINE PR", "3 AFFICHER 'TAPEZ VOTRE PRENOM : ';LIRE PR",
                 "5 AFFICHER 'BON COURAGE, ',PR,', POUR L''ETUDE DU L S E !'", "7 TERMINER");
    }

    /* --- LIRE (IMP-0222) ---------------------------------------------------------- */
    {
        static const char *const rep_[] = { "1 2 3", NULL };
        DIALOGUE(rep_, NULL, "10 LIRE A,B,C", "20 TERMINER");
        VERIFIER(valeur("A") == 1.0f && valeur("B") == 2.0f && valeur("C") == 3.0f);
    }
    {
        static const char *const rep_[] = { "1", "  2   -3.5E2", NULL };
        DIALOGUE(rep_, "\a1\r\n  2   -3.5E2\r\n", "10 LIRE A,B,C", "20 TERMINER");
        VERIFIER(valeur("A") == 1.0f && valeur("B") == 2.0f && valeur("C") == -350.0f);
    }
    {
        static const char *const rep_[] = { "XYZ", "7", NULL };              /* point F5 */
        DIALOGUE(rep_, "\aXYZ\r\n\a ? 7\r\n", "10 LIRE A", "20 TERMINER");
        VERIFIER(valeur("A") == 7.0f);
    }
    {
        static const char *const rep_[] = { "1 2\x13", NULL };   /* reste conservé (L6, LSE 8086) */
        DIALOGUE(rep_, "\a1 2\a\r\n1 2 ", "10 LIRE A;LIRE B", "20 AFFICHER A,B", "30 TERMINER");
        VERIFIER(valeur("A") == 1.0f && valeur("B") == 2.0f);
    }
    {                                     /* PG3, premier passage (INRDP75) */
        static const char *const rep_[] = { "4 PLUS\x13", "5 EGALE\x13", "EH OUI!\x13", NULL };
        DIALOGUE(rep_, NULL, "1 CHAINE A,B,E", "2 LIRE C,A,D", "3 LIRE B,E", "5 TERMINER");
        VERIFIER(valeur("C") == 4.0f && valeur("D") == 5.0f);
        VERIFIER(chaine_vaut("A", "PLUS") && chaine_vaut("B", "EGALE") && chaine_vaut("E", "EH OUI!"));
    }
    {
        static const char *const rep_[] = { "5 BONJOUR LE MONDE\x13", NULL };
        DIALOGUE(rep_, NULL, "10 CHAINE C;LIRE A,C", "20 TERMINER");
        VERIFIER(valeur("A") == 5.0f && chaine_vaut("C", "BONJOUR LE MONDE"));
    }
    {                                     /* RETURN rangé, fin de la valeur (PG3, L3, L7) */
        static const char *const rep_[] = { "AB", NULL };
        DIALOGUE(rep_, "\aAB\r\n", "10 CHAINE C;LIRE C", "20 TERMINER");
        VERIFIER(chaine_vaut("C", "AB\r\n"));
    }
    {                                     /* PG3, second passage (INRDP75) */
        static const char *const rep_[] = { "4 PLUS\x13", "5 EGALE", "EH OUI!\x13", NULL };
        DIALOGUE(rep_, NULL, "1 CHAINE A,B,E", "2 LIRE C,A,D", "3 LIRE B,E", "5 TERMINER");
        VERIFIER(chaine_vaut("B", "EGALE\r\n") && chaine_vaut("E", "EH OUI!"));
    }
    {                                     /* réponse vide : chaîne vide, et chaque LIRE
                                             redemande (LSE 8086, 2026-10-31 ; CHARG) */
        static const char *const rep_[] = { "AAA\x13", "\x13", "\x13", "BBB\x13", "\x13", "CC\x13", NULL };
        DIALOGUE(rep_, NULL, "1 CHAINE A,B,C,D,E,F", "2 LIRE A;LIRE B;LIRE C", "3 LIRE D,E;LIRE F", "5 TERMINER");
        VERIFIER(chaine_vaut("A", "AAA") && chaine_vaut("B", "") && chaine_vaut("C", ""));
        VERIFIER(chaine_vaut("D", "BBB") && chaine_vaut("E", "") && chaine_vaut("F", "CC"));
    }
    {                                     /* ordres à la télétype : rien à la console (IMP-0275) */
        static const char *const rep_[] = { NULL };
        DIALOGUE(rep_, "AB CD", "10 AFFICHER[U]'AB'!EQC(17)!EQC(19)!EQC(21)!' CD'", "20 TERMINER");
        DIALOGUE(rep_, "7 \r\n", "10 TABLEAU T[1];T[1]_7", "20 AFFICHER[U]EQC(19);AFFICHER[U]T", "30 TERMINER");
    }
    {                                     /* console à 7 bits, MITRA 15 (IMP-0282) */
        static const char *const rep_[] = { NULL };
        DIALOGUE(rep_, "AH_^\x7F\rB", "10 AFFICHER[U]'A'!EQC(200)!EQC(223)!EQC(222)!EQC(255)!EQC(141)!'B'", "20 TERMINER");
        DIALOGUE(rep_, "AB", "10 AFFICHER[U]'A'!EQC(145)!EQC(147)!EQC(149)!'B'", "20 TERMINER");
        DIALOGUE(rep_, "7 \r\n", "10 TABLEAU T[1];T[1]_7", "20 AFFICHER[U]EQC(147);AFFICHER[U]T", "30 TERMINER");
    }
    {
        static const char *const rep_[] = { "1 2", "3 4", NULL };      /* tableau sur deux lignes */
        DIALOGUE(rep_, NULL, "10 TABLEAU T[2,2];LIRE T;X_T[2,2]", "20 TERMINER");
        VERIFIER(valeur("X") == 4.0f);
    }
    {
        static const char *const rep_[] = { "1 2 3 4", NULL };
        DIALOGUE(rep_, NULL, "10 TABLEAU T[2,2];LIRE T;X_T[2,1]", "20 TERMINER");
        VERIFIER(valeur("X") == 3.0f);                                     /* ligne par ligne */
    }

    /* --- réels du T1600 : 23 bits, demie en s'éloignant de zéro (IMP-0280) ----------- */
    configuration_session.profil = PROFIL_T1600_F4;
    /* témoins d'INRDP75 (T1600 version 4), reproduits */
    SORTIE("\r\n8.200001 ", "10 AFFICHER 82/10", "20 TERMINER");               /* PG16 */
    SORTIE("\r\n8.461538 ", "10 AFFICHER 110/13", "20 TERMINER");              /* PG27 */
    {                                                                         /* PG7 */
        static const char *const rep_[] = { "86.4 23.009", NULL };
        DIALOGUE(rep_, "\a86.4 23.009\r\n\r\n993.9886 ", "10 LIRE B,H", "20 AFFICHER B*H/2",
                 "30 TERMINER");
    }
    SORTIE("\r\n7.152624 ", "10 X_-0.97;P_0;P_P*X+3;P_P*X+4;P_P*X-2;P_P*X+0.7",    /* Horner */
           "20 P_P*X+9;P_P*X+1;P_P*X+3;AFFICHER P", "30 TERMINER");
    SORTIE("\r\n73.10001 ", "10 V_6.2;V_2*V+2.5;V_2*V+5;V_2*V+3.5;AFFICHER V", "20 TERMINER");   /* PG58 */
    SORTIE("\r\n32.47562 -66.7083 0.96 0.96 \r\n4.608899 -9.701532E-05 -66.28627 ",
           "10 A_5;B_-22.3;C_1.82;D_1.25;XA_-3;YB_-2.5;R1_8.3;PSI_1.047;ALPHA_78.5E-2",
           "20 AFFICHER A-B+(C*D)^2,3*B-(C*D)^2/XA^3,XA/(YB*D),XA/YB/D",
           "30 AFFICHER (B^2-4*A*C)/(4*A^2),PSI*R1*(XA+2)/(A+B)^4,ALPHA^2+3*B-C^(-10)",
           "40 TERMINER");
    /* témoins d'INRDP75 non reproduits : 10.16666, 30.14374, 2.201199E+09, -2.434332 */
    SORTIE("\r\n10.16667 30.14375 2.2012E+09 -2.434333 ",
           "10 A_5;B_-22.3;C_1.82;D_1.25;E_2354;F_-967",
           "20 AFFICHER 122/12,A-B+C*D^2,E*F^2,E/F", "30 TERMINER");
    {                                     /* NBOR (5ème, F4), réponse 7 : 14 OPERATIONS */
        static const char *const rep_[] = { "7", NULL };
        DIALOGUE(rep_, "\a7\r\n"
                 "\r\n3.645751 \r\n2.909385 \r\n2.705692 \r\n2.644899 \r\n2.626315 "
                 "\r\n2.620591 \r\n2.618824 \r\n2.618278 \r\n2.61811 \r\n2.618058 "
                 "\r\n2.618042 \r\n2.618036 \r\n2.618035 \r\n2.618034 "
                 "\r\n1.618034 \r\n14  OPERATIONS.",
                 "7 NBRC_0", "50 LIRE NBRDE", "60 &NBROR()", "70 AFFICHER NBR2",
                 "80 AFFICHER NBRC,' OPERATIONS.'", "90 TERMINER", "100 PROCEDURE &NBROR()",
                 "110 NBRDE_RAC(NBRDE)+1", "115 NBRC_NBRC+1", "116 AFFICHER NBRDE",
                 "120 SI NBRDE#RAC(NBRDE)+1 ALORS ALLER EN 110", "130 NBR2_NBRDE-1", "140 RETOUR");
    }
    /* constantes et LIRE : le décimal le plus proche n'est pas toujours rendu
       (INRDP75 §3-3 : « 0,4 [...] sous la forme 0,3999999 ») */
    SORTIE("\r\n0.09999999 8.299999 0.4 2.51 ", "10 AFFICHER 0.1,8.3,0.4,2.51", "20 TERMINER");
    {
        static const char *const rep_[] = { "8.3", NULL };
        DIALOGUE(rep_, NULL, "10 LIRE A;B_8.3", "20 TERMINER");
        VERIFIER(valeur("A") == valeur("B") && valeur("A") * 524288.0f == 4351590.0f);
    }
    /* entiers exacts jusqu'à 2^23 seulement */
    SORTIE(NULL, "10 A_8388607+1;B_8388608+1;C_4194305*2;D_8388609;E_-8388609",
           "20 F_8388607*1;G_8388609-1;H_-8388607-1", "30 TERMINER");
    VERIFIER(valeur("A") == 8388608.0f && valeur("B") == 8388610.0f && valeur("C") == 8388610.0f);
    VERIFIER(valeur("D") == 8388610.0f && valeur("E") == -8388610.0f && valeur("F") == 8388607.0f);
    VERIFIER(valeur("G") == 8388610.0f && valeur("H") == -8388608.0f);
    /* ALE reste dans ]0,1[ : suivant(16103200) = 2^24 - 1, tronqué à 2^24 - 2 */
    SORTIE(NULL, "10 X_ALE(0.9598255157470703);Y_0.5;N_0", "20 FAIRE 30 POUR I_1 JUSQUA 300",
           "30 Y_ALE(Y);SI Y>0 ET Y<1 ALORS N_N+1", "40 TERMINER");
    VERIFIER(valeur("X") * 16777216.0f == 16777214.0f && valeur("N") == 300.0f);
    configuration_session.profil = PROFIL_T1600_V4;
    SORTIE("\r\n8.200001 ", "10 AFFICHER 82/10", "20 TERMINER");
    SORTIE(NULL, "10 B_8388608+1;D_8388609", "20 TERMINER");
    VERIFIER(valeur("B") == 8388610.0f && valeur("D") == 8388610.0f);
    /* MITRA 15 : 24 bits, arrondi au pair (IMP-0216) */
    configuration_session.profil = PROFIL_MITRA15_V8;
    SORTIE("\r\n8.2 0.1 8.3 ", "10 AFFICHER 82/10,0.1,8.3", "20 TERMINER");
    SORTIE(NULL, "10 B_8388608+1;D_8388609;X_ALE(0.9598255157470703)", "20 TERMINER");
    VERIFIER(valeur("B") == 8388609.0f && valeur("D") == 8388609.0f);
    VERIFIER(valeur("X") * 16777216.0f == 16777215.0f);

    /* --- ? en machine de bureau ----------------------------------------------------- */
    {
        static const char *const p_[] = { "10 TERMINER", NULL };
        VERIFIER(programme(p_) && contexte_initialiser(0, 0, &pos) == ERR_AUCUNE);
        bouchon_console(aucune);
        VERIFIER(compiler("?2+2,'OUI'") && r.type == LSE_LIGNE_BUREAU);
        pos.ligne = 0;
        pos.decalage = 0;
        VERIFIER(interprete_bureau(&pile, r.resultat.bloc_bureau, r.longueur_bureau, &pos, &diag) == FIN_BUREAU);
        VERIFIER(strcmp(bouchon_affichage, "\r\n4 OUI") == 0);
        objets_liberer(r.resultat.bloc_bureau);
    }

    contexte_vider();
    programme_vider();
    pile_liberer(&pile);
    VERIF_FIN();
}
