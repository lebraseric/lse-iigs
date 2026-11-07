/* composants_contexte.c — pile d'évaluation, environnement des
   identificateurs, cadres d'activation, boucles FAIRE
   EXI: EXI-0023, EXI-0029, EXI-0032 à EXI-0039, EXI-0042 à EXI-0044
   VM: VM-0801 à VM-0803, VM-0901 à VM-0903, VM-1001, VM-1002, VM-1302, VM-1303
   IMP: IMP-0144 à IMP-0155, IMP-0179, IMP-0208
   Spécification des tests : chapitre État d'exécution §3 bis à §7.1, §13 ;
   Gestionnaire des erreurs §9 (RX1 à RX5).
   Niveau : composants. Programme courant réel ; module donnees réel (étape 5a). */

#include "compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/contexte_exec.h"
#include "../../src/noyau/donnees.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

/* Objet chaîne portant le texte donné, créé par le module donnees. */
static HandleObjet objet_chaine(const char *texte, unsigned int n)
{
    ChaineSurPile c;
    HandleObjet h = GESTOBJ_HANDLE_NUL;
    if (donnees_chaine_creer((const unsigned char *) texte, n, &c) == ERR_AUCUNE)
        donnees_chaine_vers_objet(&c, &h);
    return h;
}

static PileEvaluation pile;

static void entrer(const char *texte)
{
    NatureErreur n;
    if (!compiler(texte) || !programme_inserer(&r.resultat.ligne_programme, &n))
        printf("  [%s] non entree\n", texte);
}

static unsigned int id(const char *nom)
{
    unsigned int i = 0xFFFF;
    if (!symboles_chercher(nom, (unsigned char) strlen(nom), SYM_IDENTIFICATEUR, &i))
        symboles_chercher_ou_creer(nom, (unsigned char) strlen(nom), SYM_IDENTIFICATEUR,
                                   ORIGINE_PROGRAMME, &i);
    return i;
}

static unsigned int idp(const char *nom)
{
    unsigned int i = 0xFFFF;
    symboles_chercher_ou_creer(nom, (unsigned char) strlen(nom), SYM_PROCEDURE, ORIGINE_PROGRAMME, &i);
    return i;
}

/* Case désignée par un nom dans le cadre courant (D1). */
static unsigned int cel(const char *nom)
{
    ReferenceResolue r;
    contexte_resoudre(id(nom), &r);
    return r.cellule;
}

static float valeur(const char *nom)
{
    float v = -999.0f;
    contexte_lire_nombre(cel(nom), USAGE_CALCUL, &v);
    return v;
}

static EtatIdentificateur etat(const char *nom)
{
    EntreeEnvironnement e;
    contexte_entree(cel(nom), &e);
    return e.etat;
}

static void empiler_ref(unsigned int i)
{
    ReferenceResolue r;
    contexte_resoudre(i, &r);
    pile_empiler_reference(&pile, &r);
}

static void empiler_booleen(int b)
{
    ElementPile e;
    e.type = PILE_BOOLEEN;
    e.contenu.booleen = (unsigned char) b;
    pile_empiler(&pile, &e);
}

/* Décalage qui suit l'élément d'ouverture C9 de la ligne n. */
static unsigned int reprise(unsigned char n)
{
    EntreeObjetCompile e;
    const unsigned char *o;
    unsigned int q = 0;
    programme_chercher(n, &e);
    o = objets_resoudre(e.bloc);
    while (q < e.longueur_octets && o[q] != RCIM_CTL_FAIRE_OUVRIR)
        q += rcim_longueur_element(o, e.longueur_octets, q);
    q += rcim_longueur_element(o, e.longueur_octets, q);
    objets_relacher(e.bloc);
    return q;
}

int main(void)
{
    PositionExecution pos;
    PositionExecution d;
    ElementPile el;
    EntreeEnvironnement en;
    ReferenceResolue ref;
    ChaineSurPile ch;
    HandleObjet h;
    HandleObjet h2;
    float v;
    int saut;
    int abandon;
    unsigned int i;
    unsigned int tours;

    VERIF_DEBUT("composants/contexte");
    programme_initialiser();
    pile_initialiser(&pile);

    /* --- pile d'évaluation (IMP-0144, IMP-0145) ------------------------------ */
    for (i = 0; i < 40; i++) pile_empiler_nombre(&pile, (float) i);
    VERIFIER_EGAL(40, pile.sommet);
    VERIFIER(pile_lire(&pile, 0, &el) && el.contenu.numerique == 39.0f);
    VERIFIER(pile_lire(&pile, 39, &el) && el.contenu.numerique == 0.0f);
    VERIFIER(!pile_lire(&pile, 40, &el));
    pile_ramener(&pile, 0);
    VERIFIER_EGAL(ERR_E_INTERNE, pile_retirer(&pile, &el));
    h = objet_chaine("ABC", 3);
    el.type = PILE_CHAINE;
    el.contenu.chaine.objet = h;
    el.contenu.chaine.decalage = DONNEES_DECALAGE_TEXTE;
    el.contenu.chaine.longueur = 3;
    el.contenu.chaine.transitoire = 1;
    pile_empiler(&pile, &el);
    pile_consommer(&pile, 1);
    VERIFIER(!objets_existe(h));                    /* transitoire libérée au retrait */

    /* --- environnement et usages (IMP-0179, RX1, choix E4) ------------------- */
    symboles_vider();
    entrer("10 X_1");
    entrer("20 Y_2");
    entrer("30 Z_3");
    entrer("40 TERMINER");
    VERIFIER_EGAL(ERR_AUCUNE, contexte_initialiser(0, 0, &pos));
    VERIFIER(pos.ligne == 10 && pos.decalage == 0);
    VERIFIER_EGAL(ERR_E_NON_DEFINI_CALCUL, contexte_lire_nombre(cel("X"), USAGE_CALCUL, &v));
    VERIFIER_EGAL(ERR_E_NON_DEFINI_AFFECTATION, contexte_lire_nombre(cel("X"), USAGE_AFFECTATION, &v));
    VERIFIER_EGAL(ERR_E_NON_DEFINI_COMPARAISON, contexte_lire_nombre(cel("X"), USAGE_COMPARAISON, &v));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_affecter_nombre(cel("X"), 2.5f));
    VERIFIER(valeur("X") == 2.5f);
    /* symbole créé après l'environnement : agrandissement paresseux */
    VERIFIER_EGAL(ID_NON_DEFINI, etat("NOUV"));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_affecter_nombre(cel("NOUV"), 1.0f));
    /* pile : déréférencement et usage */
    empiler_ref(id("Y"));
    VERIFIER_EGAL(ERR_E_NON_DEFINI_COMPARAISON, pile_valeur_numerique(&pile, 0, USAGE_COMPARAISON, &v));
    empiler_ref(id("X"));
    VERIFIER(pile_valeur_numerique(&pile, 0, USAGE_CALCUL, &v) == ERR_AUCUNE && v == 2.5f);
    pile_ramener(&pile, 0);

    /* chaînes */
    h = objet_chaine("OUI", 3);
    VERIFIER_EGAL(ERR_E_CHAINE_NON_DECLAREE, contexte_affecter_chaine(cel("C"), h));   /* LSE15 §4-3 */
    VERIFIER_EGAL(ERR_AUCUNE, contexte_declarer_chaine(id("C")));
    VERIFIER_EGAL(ERR_E_CHAINE_SANS_VALEUR, contexte_lire_chaine(cel("C"), USAGE_AFFECTATION, &ch));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_affecter_chaine(cel("C"), h));
    VERIFIER(contexte_lire_chaine(cel("C"), USAGE_AFFECTATION, &ch) == ERR_AUCUNE && ch.longueur == 3);
    VERIFIER_EGAL(ERR_E_CALCUL_NON_NUMERIQUE, contexte_lire_nombre(cel("C"), USAGE_CALCUL, &v));
    VERIFIER_EGAL(ERR_E_COMPARAISON_TYPES, contexte_lire_nombre(cel("C"), USAGE_COMPARAISON, &v));
    VERIFIER_EGAL(ERR_E_NOMBRE_DANS_CHAINE, contexte_affecter_nombre(cel("C"), 1.0f));
    VERIFIER_EGAL(ERR_E_OPERANDE_NON_CHAINE, contexte_lire_chaine(cel("X"), USAGE_CALCUL, &ch));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_declarer_chaine(id("C")));    /* valeur non conservée */
    VERIFIER(!objets_existe(h));
    VERIFIER_EGAL(ERR_E_DECLARATION_CHAINE, contexte_declarer_chaine(id("X")));
    /* tableaux */
    h = objet_chaine("T1", 2);
    VERIFIER_EGAL(ERR_E_DECLARATION_TABLEAU, contexte_declarer_tableau(id("X"), h));  /* LSE15 §3-1.2 */
    VERIFIER_EGAL(ERR_E_DECLARATION_TABLEAU, contexte_declarer_tableau(id("C"), h));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_declarer_tableau(id("T"), h));
    h2 = objet_chaine("T2", 2);
    VERIFIER_EGAL(ERR_AUCUNE, contexte_declarer_tableau(id("T"), h2));  /* libération implicite */
    VERIFIER(!objets_existe(h));
    VERIFIER_EGAL(ERR_E_DECLARATION_CHAINE, contexte_declarer_chaine(id("T")));
    VERIFIER_EGAL(ERR_E_CALCUL_NON_NUMERIQUE, contexte_lire_nombre(cel("T"), USAGE_CALCUL, &v));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_liberer(id("T")));
    VERIFIER(!objets_existe(h2));
    VERIFIER_EGAL(ID_NON_DEFINI, etat("T"));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_liberer(id("T")));             /* sans effet */
    VERIFIER_EGAL(ERR_AUCUNE, contexte_liberer(id("X")));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_declarer_chaine(id("X")));     /* après libération */

    /* --- initialisation (§7.1, IMP-0153) ------------------------------------- */
    contexte_affecter_nombre(cel("Y"), 5.0f);
    VERIFIER_EGAL(ERR_AUCUNE, contexte_initialiser(20, 40, &pos));
    VERIFIER(pos.ligne == 20);
    VERIFIER_EGAL(40, contexte_ligne_arret());
    VERIFIER_EGAL(ID_NON_DEFINI, etat("Y"));                          /* LSE15 §9-2 */
    VERIFIER_EGAL(ID_NON_DEFINI, etat("X"));
    VERIFIER_EGAL(ERR_E_LIGNE_INEXISTANTE, contexte_initialiser(15, 0, &pos));

    /* --- branchements et fin de programme (IMP-0154, RX5) --------------------- */
    contexte_initialiser(0, 0, &pos);
    VERIFIER_EGAL(ERR_AUCUNE, contexte_brancher(30, &pos));
    VERIFIER(pos.ligne == 30);
    VERIFIER_EGAL(ERR_E_LIGNE_INEXISTANTE, contexte_brancher(99, &pos));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_fin_de_ligne(&pos));
    VERIFIER(pos.ligne == 40);
    VERIFIER_EGAL(ERR_E_FIN_DE_PROGRAMME, contexte_fin_de_ligne(&pos));

    /* --- boucle FAIRE (IMP-0151, IMP-0152) ------------------------------------ */
    programme_vider();
    contexte_vider();
    pile_initialiser(&pile);
    entrer("10 FAIRE 30 POUR I_1 JUSQUA 3");
    entrer("20 X_1");
    entrer("30 Y_2");
    entrer("40 TERMINER");
    contexte_initialiser(0, 0, &pos);
    d.ligne = 10;
    d.decalage = reprise(10);
    pile_empiler_nombre(&pile, 30.0f);
    pile_empiler_nombre(&pile, 1.0f);
    VERIFIER_EGAL(ERR_AUCUNE, contexte_ouvrir_boucle(&pile, id("I"), &d));
    VERIFIER_EGAL(0, pile.sommet);
    VERIFIER_EGAL(1, contexte_nb_boucles());
    VERIFIER(valeur("I") == 1.0f);
    tours = 0;
    pos = d;
    for (;;) {                                    /* PAS implicite, TESTER, corps 10 à 30 */
        contexte_pas(&pile, 0);
        pile_empiler_nombre(&pile, 3.0f);
        contexte_tester(&pile, 0, &pos, &saut);
        if (saut) break;
        tours++;
        pos.ligne = 10;
        contexte_fin_de_ligne(&pos);              /* 10 -> 20 */
        contexte_fin_de_ligne(&pos);              /* 20 -> 30 */
        contexte_fin_de_ligne(&pos);              /* 30 : reprise */
        if (pos.ligne != 10 || pos.decalage != d.decalage || tours > 10) break;
    }
    VERIFIER_EGAL(3, tours);
    VERIFIER(valeur("I") == 4.0f);                /* LSE15 §5-5.3 */
    VERIFIER_EGAL(40, pos.ligne);                 /* première ligne après la boucle */
    VERIFIER_EGAL(0, contexte_nb_boucles());
    /* réouverture à la même position : réinitialisation en place */
    pile_empiler_nombre(&pile, 30.0f); pile_empiler_nombre(&pile, 1.0f);
    contexte_ouvrir_boucle(&pile, id("I"), &d);
    pile_empiler_nombre(&pile, 30.0f); pile_empiler_nombre(&pile, 7.0f);
    contexte_ouvrir_boucle(&pile, id("I"), &d);
    VERIFIER_EGAL(1, contexte_nb_boucles());
    VERIFIER(valeur("I") == 7.0f);
    /* désimbrication par l'un et l'autre bord */
    contexte_brancher(40, &pos);
    VERIFIER_EGAL(0, contexte_nb_boucles());
    pile_empiler_nombre(&pile, 30.0f); pile_empiler_nombre(&pile, 1.0f);
    contexte_ouvrir_boucle(&pile, id("I"), &d);
    contexte_brancher(20, &pos);
    VERIFIER_EGAL(1, contexte_nb_boucles());
    /* TANT QUE */
    contexte_pas(&pile, 0);
    empiler_booleen(0);
    pos = d;
    VERIFIER_EGAL(ERR_AUCUNE, contexte_tester(&pile, 1, &pos, &saut));
    VERIFIER(saut && pos.ligne == 40);
    /* fin effective, fins invalides (LSE15 §5-5.1, RX4) */
    pile_empiler_nombre(&pile, 35.0f); pile_empiler_nombre(&pile, 1.0f);
    VERIFIER_EGAL(ERR_AUCUNE, contexte_ouvrir_boucle(&pile, id("I"), &d));
    pos = d;
    contexte_pas(&pile, 0);
    pile_empiler_nombre(&pile, 0.0f);
    contexte_tester(&pile, 0, &pos, &saut);       /* sortie dès le premier test */
    VERIFIER(saut && pos.ligne == 40);            /* ligne de fin effective 30 */
    contexte_affecter_nombre(cel("I"), 99.0f);
    pile_empiler_nombre(&pile, 5.0f); pile_empiler_nombre(&pile, 1.0f);
    VERIFIER_EGAL(ERR_E_FIN_BOUCLE_AVANT, contexte_ouvrir_boucle(&pile, id("I"), &d));
    pile_ramener(&pile, 0);
    pile_empiler_nombre(&pile, 0.2f); pile_empiler_nombre(&pile, 1.0f);
    VERIFIER_EGAL(ERR_E_FIN_BOUCLE_NULLE, contexte_ouvrir_boucle(&pile, id("I"), &d));
    pile_ramener(&pile, 0);
    VERIFIER(valeur("I") == 99.0f);               /* aucune affectation avant l'erreur */
    contexte_declarer_chaine(id("K"));
    pile_empiler_nombre(&pile, 30.0f); pile_empiler_nombre(&pile, 1.0f);
    VERIFIER_EGAL(ERR_E_VARIABLE_BOUCLE, contexte_ouvrir_boucle(&pile, id("K"), &d));
    pile_ramener(&pile, 0);
    /* boucles de même ligne de fin (LSE15 §5-5.5) */
    programme_vider();
    contexte_vider();
    entrer("100 FAIRE 104 POUR I_1 JUSQUA 2");
    entrer("102 FAIRE 104 POUR J_1 JUSQUA 2");
    entrer("104 A_0");
    entrer("106 TERMINER");
    contexte_initialiser(0, 0, &pos);
    {
        PositionExecution di;
        PositionExecution dj;
        unsigned int corps = 0;
        di.ligne = 100; di.decalage = reprise(100);
        dj.ligne = 102; dj.decalage = reprise(102);
        pile_empiler_nombre(&pile, 104.0f); pile_empiler_nombre(&pile, 1.0f);
        contexte_ouvrir_boucle(&pile, id("I"), &di);
        pos = di;
        for (tours = 0; tours < 20; tours++) {
            if (pos.ligne == 100) {                   /* pas et test de I */
                contexte_pas(&pile, 0);
                pile_empiler_nombre(&pile, 2.0f);
                contexte_tester(&pile, 0, &pos, &saut);
                if (saut) break;
                pos.ligne = 100; contexte_fin_de_ligne(&pos);
            } else if (pos.ligne == 102) {
                if (pos.decalage == 0) {
                    pile_empiler_nombre(&pile, 104.0f); pile_empiler_nombre(&pile, 1.0f);
                    contexte_ouvrir_boucle(&pile, id("J"), &dj);
                }
                contexte_pas(&pile, 0);
                pile_empiler_nombre(&pile, 2.0f);
                pos = dj;
                contexte_tester(&pile, 0, &pos, &saut);
                if (!saut) { pos.ligne = 102; contexte_fin_de_ligne(&pos); }
            } else if (pos.ligne == 104) {
                corps++;
                contexte_fin_de_ligne(&pos);
            } else break;
        }
        VERIFIER_EGAL(4, corps);                       /* N² passages dans le corps */
        VERIFIER_EGAL(106, pos.ligne);
        VERIFIER_EGAL(0, contexte_nb_boucles());
    }

    /* --- procédures (§5.5) ---------------------------------------------------- */
    programme_vider();
    contexte_vider();
    entrer("10 X_1");
    entrer("20 PROCEDURE &P(A,B) LOCAL A,M");
    entrer("30 RETOUR");
    entrer("50 PROCEDURE &F(N) LOCAL N");
    entrer("60 PROCEDURE &S(T) LOCAL T");
    entrer("70 PROCEDURE &R(U)");
    contexte_initialiser(0, 0, &pos);
    contexte_affecter_nombre(cel("A"), 3.0f);
    contexte_affecter_nombre(cel("G"), 7.0f);
    contexte_affecter_nombre(cel("M"), 9.0f);
    /* &P(5,G) en instruction */
    empiler_ref(idp("P")); pile_empiler_nombre(&pile, 5.0f); empiler_ref(id("G"));
    pos.ligne = 10; pos.decalage = 6;
    VERIFIER_EGAL(ERR_AUCUNE, contexte_appeler(&pile, 2, RCIM_APPEL_INSTRUCTION, &pos));
    VERIFIER_EGAL(20, pos.ligne);
    VERIFIER(pos.decalage > 0);
    VERIFIER_EGAL(0, pile.sommet);
    VERIFIER_EGAL(1, contexte_profondeur());
    VERIFIER(valeur("A") == 5.0f);                 /* paramètre LOCAL */
    VERIFIER_EGAL(ID_NON_DEFINI, etat("M"));       /* LOCAL non paramètre */
    contexte_resoudre(id("B"), &ref);
    VERIFIER_EGAL(cel("G"), ref.cellule);          /* paramètre référence (IMP-0148, D1) */
    contexte_affecter_nombre(ref.cellule, 8.0f);
    VERIFIER_EGAL(ERR_AUCUNE, contexte_liberer(id("M")));
    VERIFIER_EGAL(ERR_E_LIBERER_NON_LOCAL, contexte_liberer(id("G")));   /* LSE15 §3-1.2 */
    VERIFIER_EGAL(ERR_E_TABLEAU_PARAMETRE, contexte_declarer_tableau(id("B"), GESTOBJ_HANDLE_NUL));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_retourner(&pile, 0, 0, &pos, &abandon));
    VERIFIER(pos.ligne == 10 && pos.decalage == 6 && !abandon);
    VERIFIER(valeur("A") == 3.0f);                 /* restauration (IMP-0149) */
    VERIFIER(valeur("M") == 9.0f);
    VERIFIER(valeur("G") == 8.0f);                 /* modification visible de l'appelant */
    VERIFIER_EGAL(0, contexte_profondeur());
    VERIFIER_EGAL(ERR_E_RETOUR_HORS_PROCEDURE, contexte_retourner(&pile, 0, 0, &pos, &abandon));
    /* erreurs à l'appel, sans modification */
    empiler_ref(idp("P")); pile_empiler_nombre(&pile, 5.0f);
    VERIFIER_EGAL(ERR_E_NOMBRE_PARAMETRES, contexte_appeler(&pile, 1, 0, &pos));
    VERIFIER_EGAL(2, pile.sommet);
    pile_ramener(&pile, 0);
    empiler_ref(idp("Q"));
    VERIFIER_EGAL(ERR_E_PROCEDURE_NON_DECLAREE, contexte_appeler(&pile, 0, 0, &pos));
    pile_ramener(&pile, 0);
    empiler_ref(idp("P")); pile_empiler_nombre(&pile, 5.0f); pile_empiler_nombre(&pile, 6.0f);
    VERIFIER_EGAL(ERR_E_PARAMETRE_REFERENCE, contexte_appeler(&pile, 2, 0, &pos));   /* 72 */
    VERIFIER(valeur("A") == 3.0f);
    VERIFIER_EGAL(0, contexte_profondeur());
    pile_ramener(&pile, 0);
    /* paramètre LOCAL sans valeur : erreur à l'utilisation (RX3, C1) */
    empiler_ref(idp("P")); empiler_ref(id("Z")); empiler_ref(id("G"));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_appeler(&pile, 2, 0, &pos));
    VERIFIER_EGAL(ID_PARAMETRE_SANS_VALEUR, etat("A"));
    VERIFIER_EGAL(ERR_E_PARAMETRE_SANS_VALEUR, contexte_lire_nombre(cel("A"), USAGE_CALCUL, &v));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_affecter_nombre(cel("A"), 1.0f));
    contexte_retourner(&pile, 0, 0, &pos, &abandon);
    VERIFIER(valeur("A") == 3.0f);
    /* RESULTAT en expression, résultat lu avant la restauration */
    contexte_affecter_nombre(cel("N"), 100.0f);
    empiler_ref(idp("F")); pile_empiler_nombre(&pile, 4.0f);
    VERIFIER_EGAL(ERR_AUCUNE, contexte_appeler(&pile, 1, RCIM_APPEL_EXPRESSION, &pos));
    VERIFIER(valeur("N") == 4.0f);
    empiler_ref(id("N"));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_retourner(&pile, 1, 0, &pos, &abandon));
    VERIFIER(pile_lire(&pile, 0, &el) && el.type == PILE_NUMERIQUE && el.contenu.numerique == 4.0f);
    VERIFIER(valeur("N") == 100.0f);
    pile_ramener(&pile, 0);
    /* récursivité : deux niveaux de N (LSE15 §7-6) */
    empiler_ref(idp("F")); pile_empiler_nombre(&pile, 7.0f);
    contexte_appeler(&pile, 1, RCIM_APPEL_EXPRESSION, &pos);
    empiler_ref(idp("F")); pile_empiler_nombre(&pile, 6.0f);
    contexte_appeler(&pile, 1, RCIM_APPEL_EXPRESSION, &pos);
    VERIFIER_EGAL(2, contexte_profondeur());
    VERIFIER(valeur("N") == 6.0f);
    pile_empiler_nombre(&pile, 720.0f);
    contexte_retourner(&pile, 1, 0, &pos, &abandon);
    VERIFIER(valeur("N") == 7.0f);
    pile_ramener(&pile, 0);
    pile_empiler_nombre(&pile, 5040.0f);
    contexte_retourner(&pile, 1, 0, &pos, &abandon);
    VERIFIER(valeur("N") == 100.0f);
    pile_ramener(&pile, 0);
    /* sorties contraires à la position d'appel (RX2) */
    empiler_ref(idp("F")); pile_empiler_nombre(&pile, 1.0f);
    contexte_appeler(&pile, 1, RCIM_APPEL_INSTRUCTION, &pos);
    pile_empiler_nombre(&pile, 1.0f);
    VERIFIER_EGAL(ERR_E_RESULTAT_SOUS_PROGRAMME, contexte_retourner(&pile, 1, 0, &pos, &abandon));
    pile_ramener(&pile, 0);
    VERIFIER_EGAL(ERR_AUCUNE, contexte_retourner(&pile, 0, 0, &pos, &abandon));
    empiler_ref(idp("F")); pile_empiler_nombre(&pile, 1.0f);
    contexte_appeler(&pile, 1, RCIM_APPEL_EXPRESSION, &pos);
    VERIFIER_EGAL(ERR_E_RETOUR_DANS_FONCTION, contexte_retourner(&pile, 0, 0, &pos, &abandon));
    VERIFIER_EGAL(ERR_E_LIGNE_INEXISTANTE, contexte_retourner(&pile, 0, 99, &pos, &abandon));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_retourner(&pile, 0, 10, &pos, &abandon));   /* RETOUR EN */
    VERIFIER(abandon && pos.ligne == 10 && pos.decalage == 0);                    /* §7.3 */
    VERIFIER_EGAL(0, contexte_profondeur());
    /* chaîne passée à un paramètre LOCAL : copie, libérée au retour */
    contexte_declarer_chaine(id("C"));
    contexte_affecter_chaine(cel("C"), objet_chaine("ABC", 3));
    empiler_ref(idp("S")); empiler_ref(id("C"));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_appeler(&pile, 1, 0, &pos));
    contexte_entree(cel("T"), &en);
    VERIFIER_EGAL(ID_CHAINE, en.etat);
    h = en.contenu.objet;
    contexte_entree(cel("C"), &en);
    VERIFIER(h != en.contenu.objet);
    contexte_retourner(&pile, 0, 0, &pos, &abandon);
    VERIFIER(!objets_existe(h) || h == en.contenu.objet);
    VERIFIER_EGAL(ID_NON_DEFINI, etat("T"));
    /* paramètre référence nom de procédure (LSE15 §7-8.2, IMP-0148) */
    empiler_ref(idp("R")); empiler_ref(idp("F"));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_appeler(&pile, 1, 0, &pos));
    contexte_resoudre(id("U"), &ref);
    VERIFIER_EGAL(REF_PROCEDURE, ref.indexee);
    VERIFIER_EGAL(idp("F"), ref.cellule);
    contexte_retourner(&pile, 0, 0, &pos, &abandon);

    /* --- référence vers un nom masqué par un LOCAL (D1, constat sur LSE 8086) ---
       10 A_1;&P(A);AFFICHER A  /  PROCEDURE &P(B) LOCAL A;A_2;B_3;RETOUR : 3 */
    entrer("80 PROCEDURE &PA(B) LOCAL A");
    contexte_affecter_nombre(cel("A"), 1.0f);
    empiler_ref(idp("PA")); empiler_ref(id("A"));
    VERIFIER_EGAL(ERR_AUCUNE, contexte_appeler(&pile, 1, 0, &pos));
    VERIFIER_EGAL(ID_NON_DEFINI, etat("A"));                     /* case locale neuve */
    contexte_affecter_nombre(cel("A"), 2.0f);                    /* A_2 : case locale */
    contexte_resoudre(id("B"), &ref);
    contexte_affecter_nombre(ref.cellule, 3.0f);                 /* B_3 : case globale de A */
    VERIFIER(valeur("A") == 2.0f);
    contexte_retourner(&pile, 0, 0, &pos, &abandon);
    VERIFIER(valeur("A") == 3.0f);                               /* LSE 8086 : 3 */

    /* --- boucle dans une procédure, détruite au retour (IMP-0152) ------------- */
    empiler_ref(idp("F")); pile_empiler_nombre(&pile, 1.0f);
    contexte_appeler(&pile, 1, RCIM_APPEL_INSTRUCTION, &pos);
    d.ligne = 50; d.decalage = 9;
    pile_empiler_nombre(&pile, 60.0f); pile_empiler_nombre(&pile, 1.0f);
    contexte_ouvrir_boucle(&pile, id("I"), &d);
    VERIFIER_EGAL(1, contexte_nb_boucles());
    contexte_retourner(&pile, 0, 0, &pos, &abandon);
    VERIFIER_EGAL(0, contexte_nb_boucles());

    /* pile d'évaluation plafonnée à 64 Kio : 9 362 éléments, puis EX 03, sans altération
       des éléments en place (IMP-0145 ; audit du code, AC-06) */
    {
        PileEvaluation q;
        ElementPile e;
        unsigned int i;
        unsigned int faux = 0;
        NatureErreur n = ERR_AUCUNE;
        float v;
        pile_initialiser(&q);
        for (i = 0; i < 12000u && n == ERR_AUCUNE; i++) {
            donnees_nombre_entier((long) i, &v);
            n = pile_empiler_nombre_exact(&q, &v);
        }
        VERIFIER(n == ERR_E_MEMOIRE_SATUREE && q.sommet == 9362u);
        for (i = 0; i < q.sommet; i++) {
            donnees_nombre_entier((long) i, &v);
            if (!pile_lire(&q, q.sommet - 1 - i, &e) || e.type != PILE_NUMERIQUE
                || donnees_comparer_nombres(&e.contenu.numerique, &v) != 0) faux++;
        }
        VERIFIER_EGAL(0, faux);
        pile_liberer(&q);
    }

    contexte_vider();
    pile_liberer(&pile);
    programme_vider();
    VERIF_FIN();
}
