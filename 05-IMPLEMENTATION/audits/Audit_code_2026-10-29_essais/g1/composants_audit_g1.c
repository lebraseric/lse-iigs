/* composants_audit_g1.c — essais de l'audit du groupe 1 (interpréteur, pile,
   contexte d'exécution, données). Copie de travail de l'audit, hors dépôt.
   Chaque essai imprime l'état obtenu ; VERIFIER porte le comportement
   attendu d'après la spécification. */

#include "compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/interprete.h"
#include "../../src/noyau/donnees.h"
#include "../../src/noyau/fichiers.h"
#include "../../src/noyau/bibliotheque.h"
#include "../../src/noyau/rcim.h"
#include "../../src/interfaces/interface_fichiers.h"
#include "../../src/interfaces/interface_console.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

void bouchon_console(const char *const *lignes);
void bouchon_interruption(unsigned int rang);
extern char bouchon_affichage[];

static PileEvaluation pile;
static DiagnosticExecution diag;
static PositionExecution pos;
static const char *const aucune[] = { NULL };

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

static FinExecution lancer(void)
{
    NatureErreur n = contexte_initialiser(0, 0, &pos);
    if (n != ERR_AUCUNE) { diag.nature = n; return FIN_ERREUR; }
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

static float element_de(const char *nom, unsigned int rang)
{
    ReferenceResolue ref;
    EntreeEnvironnement e;
    unsigned int id;
    float v = -999.0f;
    if (!symboles_chercher(nom, (unsigned char) strlen(nom), SYM_IDENTIFICATEUR, &id)) return -998.0f;
    if (contexte_resoudre(id, &ref) != ERR_AUCUNE) return -997.0f;
    if (contexte_entree(ref.cellule, &e) != ERR_AUCUNE || e.etat != ID_TABLEAU) return -995.0f;
    if (donnees_lire_element(e.contenu.objet, rang, &v) != ERR_AUCUNE) return -996.0f;
    return v;
}

static FinExecution bureau_bloc(PositionExecution *b)
{
    return interprete_bureau(&pile, r.resultat.bloc_bureau, r.longueur_bureau, b, &diag);
}

int main(void)
{
    FinExecution f;

    VERIF_DEBUT("composants/audit_g1");
    programme_initialiser();
    pile_initialiser(&pile);
    bouchon_console(aucune);

    /* --- A. ESC dans une boucle FAIRE tenue sur une seule ligne ------------- */
    {
        static const char *const p1[] = { "10 FAIRE 10 POUR I_1 JUSQUA 300;X_I", "20 TERMINER", NULL };
        static const char *const p2[] = { "10 FAIRE 20 POUR I_1 JUSQUA 300", "20 X_I", "30 TERMINER", NULL };
        VERIFIER(programme(p1));
        bouchon_interruption(2);             /* ESC frappée avant la 2e consultation */
        f = lancer();
        printf("  [A1 boucle d'une ligne] fin %u, ligne %u, X = %ld\n", f, pos.ligne, (long) valeur("X"));
        VERIFIER(f == FIN_INTERRUPTION && valeur("X") < 300.0f);
        bouchon_console(aucune);
        VERIFIER(programme(p2));
        bouchon_interruption(2);
        f = lancer();
        printf("  [A2 boucle de deux lignes] fin %u, ligne %u, X = %ld\n", f, pos.ligne, (long) valeur("X"));
        bouchon_console(aucune);
    }

    /* --- B. ligne de machine de bureau en erreur, puis CONTINUER ------------- */
    {
        static const char *const p[] = { "10 X_&F(2)", "20 TERMINER", "30 PROCEDURE &F(A) LOCAL A",
                                         "40 PAUSE", "50 RESULTAT A*10", NULL };
        PositionExecution b;
        unsigned int h0;
        VERIFIER(programme(p));
        f = lancer();
        VERIFIER(f == FIN_PAUSE);
        h0 = pile.sommet;
        VERIFIER(compiler("?Z"));
        b.ligne = 0;
        b.decalage = 0;
        f = bureau_bloc(&b);
        objets_liberer(r.resultat.bloc_bureau);
        printf("  [B bureau ?Z] fin %u, EX %u ; pile %u -> %u\n", f, erreurs_numero(diag.nature), h0, pile.sommet);
        f = interprete_executer(&pile, &pos, &diag);          /* CONTINUER (moniteur.c:358) */
        printf("  [B CONTINUER] fin %u, EX %u, X = %ld, Z = %ld, pile %u\n", f,
               erreurs_numero(diag.nature), (long) valeur("X"), (long) valeur("Z"), pile.sommet);
        VERIFIER(f == FIN_TERMINER && valeur("X") == 20.0f);
    }

    /* --- C. élément de tableau en RESULTAT et en argument LOCAL --------------- */
    {
        static const char *const p1[] = { "10 TABLEAU T[3];T[2]_7;X_&F(2)", "20 TERMINER",
                                          "30 PROCEDURE &F(N) LOCAL N;RESULTAT T[N]", NULL };
        static const char *const p2[] = { "10 TABLEAU T[3];T[2]_7;Y_&G(T[2])", "20 TERMINER",
                                          "30 PROCEDURE &G(A) LOCAL A;RESULTAT A+1", NULL };
        VERIFIER(programme(p1));
        f = lancer();
        printf("  [C1 RESULTAT T[N]] fin %u, EX %u ligne %u, X = %ld\n", f, erreurs_numero(diag.nature),
               diag.ligne, (long) valeur("X"));
        VERIFIER(f == FIN_TERMINER && valeur("X") == 7.0f);
        VERIFIER(programme(p2));
        f = lancer();
        printf("  [C2 &G(T[2]) LOCAL] fin %u, EX %u ligne %u, Y = %ld\n", f, erreurs_numero(diag.nature),
               diag.ligne, (long) valeur("Y"));
        VERIFIER(f == FIN_TERMINER && valeur("Y") == 8.0f);
    }

    /* --- G. état de lecture partagé entre programme et machine de bureau ------- */
    {
        static const char *const p[] = { "10 TABLEAU T[3];LIRE T", "20 TERMINER", NULL };
        PositionExecution b;
        VERIFIER(programme(p));
        f = lancer();
        VERIFIER(f == FIN_LECTURE);
        interprete_fournir_saisie("1", 1, SAISIE_RETURN);
        f = interprete_executer(&pile, &pos, &diag);          /* T[1] lu, attend la suite */
        VERIFIER(f == FIN_LECTURE);
        /* ESC pendant l'attente (moniteur : PRET EN LIGNE 10), puis ligne LIRE Y */
        VERIFIER(compiler("LIRE Y"));
        b.ligne = 0;
        b.decalage = 0;
        f = bureau_bloc(&b);
        if (f == FIN_LECTURE) {
            interprete_fournir_saisie("9", 1, SAISIE_RETURN);
            f = bureau_bloc(&b);
        }
        objets_liberer(r.resultat.bloc_bureau);
        f = interprete_executer(&pile, &pos, &diag);          /* CONTINUER */
        if (f == FIN_LECTURE) {
            interprete_fournir_saisie("2 3", 3, SAISIE_RETURN);
            f = interprete_executer(&pile, &pos, &diag);
        }
        printf("  [G LIRE T repris] fin %u, T = %ld %ld %ld, Y = %ld\n", f, (long) element_de("T", 0),
               (long) element_de("T", 1), (long) element_de("T", 2), (long) valeur("Y"));
        VERIFIER(f == FIN_TERMINER && element_de("T", 0) == 1.0f && element_de("T", 1) == 2.0f
                 && element_de("T", 2) == 3.0f);
    }

    /* --- K. description d'AFFICHER et appel de fonction dans la liste --------- */
    {
        static const char *const p1[] = { "10 AFFICHER[2F6.2] &F(1),2", "20 TERMINER",
                                          "30 PROCEDURE &F(A) LOCAL A;AFFICHER['*'];RESULTAT A", NULL };
        static const char *const p2[] = { "10 AFFICHER[2F6.2] &F(1),2", "20 TERMINER",
                                          "30 PROCEDURE &F(A) LOCAL A", "40 RESULTAT A", NULL };
        unsigned int tours = 0;
        bouchon_console(aucune);
        VERIFIER(programme(p1));
        f = lancer();
        printf("  [K1 AFFICHER imbrique] fin %u, affichage [%s]\n", f, bouchon_affichage);
        VERIFIER(strcmp(bouchon_affichage, "*  1.00  2.00") == 0);
        bouchon_console(aucune);
        VERIFIER(programme(p2));
        interprete_pas_a_pas(1);
        f = lancer();
        while (f == FIN_PAS && tours < 20) {
            tours++;
            f = interprete_executer(&pile, &pos, &diag);
        }
        interprete_pas_a_pas(0);
        printf("  [K2 pas a pas] fin %u apres %u pas, affichage [%s]\n", f, tours, bouchon_affichage);
        VERIFIER(strcmp(bouchon_affichage, "  1.00  2.00") == 0);
        bouchon_console(aucune);
    }

    /* --- H. F45.0 d'un grand nombre ------------------------------------------- */
    {
        char t[300];
        unsigned int n;
        unsigned int i;
        unsigned int intrus = 0;
        float v = 3.0e38f;
        n = donnees_ecrire_nombre(v, RCIM_DESC_F, 45, 0, t);
        for (i = 0; i < n; i++) if (t[i] != ' ' && (t[i] < '0' || t[i] > '9')) intrus++;
        printf("  [H F45.0 de 3E38] [%s] %u caractere(s) hors chiffres\n", t, intrus);
        v = 1.0e20f;
        n = donnees_ecrire_nombre(v, RCIM_DESC_F, 30, 2, t);
        printf("  [H F30.2 de 1E20] [%s]\n", t);
    }

    /* --- D. tableau de 65 535 éléments (objet hors-norme > 64 Kio) ---------- */
    {
        HandleObjet h;
        unsigned int i;
        unsigned int faux = 0;
        unsigned int premier = 0;
        float v;
        float w;
        unsigned char dims = 0;
        unsigned int d1 = 0;
        unsigned int d2 = 0;
        NatureErreur n = donnees_creer_tableau(1, 65535.0f, 0.0f, &h);
        printf("  [D creation] EX %u, longueur %lu\n", erreurs_numero(n), objets_longueur(h));
        VERIFIER(n == ERR_AUCUNE);
        if (n == ERR_AUCUNE) {
            for (i = 0; i < 65535u; i++) {
                donnees_nombre_entier((long) i, &v);
                donnees_ecrire_element(h, i, v);
            }
            for (i = 0; i < 65535u; i++) {
                donnees_nombre_entier((long) i, &w);
                if (donnees_lire_element(h, i, &v) != ERR_AUCUNE || donnees_comparer_nombres(&v, &w) != 0) {
                    if (faux == 0) premier = i;
                    faux++;
                }
            }
            donnees_tableau_bornes(h, &dims, &d1, &d2);
            printf("  [D 65535 elements] %u faux (premier %u) ; bornes %u %u %u\n", faux, premier, dims, d1, d2);
            VERIFIER(faux == 0 && d1 == 65535u && d2 == 1u);
            objets_liberer(h);
        }
    }

    /* --- E. GARER puis CHARGER d'un tableau de plus de 64 Kio ---------------- */
    {
        static const char *const p[] = { "1 TABLEAU T[16000];T[1]_5;T[16000]_7",
                                         "2 GARER T,1,'#GT';CHARGER U,1,'#GT';A_U[1];B_U[16000]",
                                         "3 TERMINER", NULL };
        Identification id;
        VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBG1"));
        plateforme_espace_vider(ESPACE_TEMPORAIRES);
        plateforme_espace_vider(ESPACE_DONNEES);
        plateforme_espace_vider(ESPACE_PROGRAMMES);
        plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
        bibliotheque_identification_chercher(1, &id);
        bibliotheque_identifier(&id);
        VERIFIER(programme(p));
        f = lancer();
        printf("  [E GARER/CHARGER T[16000]] fin %u, EX %u ligne %u, A = %ld, B = %ld\n", f,
               erreurs_numero(diag.nature), diag.ligne, (long) valeur("A"), (long) valeur("B"));
        VERIFIER(f == FIN_TERMINER && valeur("A") == 5.0f && valeur("B") == 7.0f);
    }

    /* --- I. pile d'évaluation de plus de 64 Kio, sans interpréteur ------------ */
    {
        PileEvaluation q;
        ElementPile e;
        unsigned int i;
        unsigned int faux = 0;
        unsigned int premier = 0;
        float v;
        pile_initialiser(&q);
        for (i = 0; i < 12000u; i++) {
            donnees_nombre_entier((long) i, &v);
            if (pile_empiler_nombre_exact(&q, &v) != ERR_AUCUNE) break;
        }
        printf("  [I pile] %u elements empiles, capacite %u, %lu octets\n", q.sommet, q.capacite,
               (unsigned long) q.capacite * (unsigned long) sizeof(ElementPile));
        for (i = 0; i < q.sommet; i++) {
            donnees_nombre_entier((long) i, &v);
            if (!pile_lire(&q, q.sommet - 1 - i, &e) || e.type != PILE_NUMERIQUE
                || donnees_comparer_nombres(&e.contenu.numerique, &v) != 0) {
                if (faux == 0) premier = i;
                faux++;
            }
        }
        printf("  [I pile] %u element(s) faux, premier rang %u (rang * 7 = %lu)\n", faux, premier,
               (unsigned long) premier * 7ul);
        VERIFIER(faux == 0);
        pile_liberer(&q);
    }

    /* --- J. réserve des cases au-delà de 64 Kio, pile d'évaluation basse ------ */
    {
        static const char *const p1[] = { "10 K_0;&P(12000)", "20 TERMINER", "30 PROCEDURE &P(N) LOCAL N",
                                          "40 K_K+1;SI N>0 ALORS &P(N-1)", "50 RETOUR", NULL };
        static const char *const p2[] = { "10 K_0;&P(14000)", "20 TERMINER", "30 PROCEDURE &P(N) LOCAL N",
                                          "40 K_K+1;SI N>0 ALORS &P(N-1)", "50 RETOUR", NULL };
        VERIFIER(programme(p1));
        f = lancer();
        printf("  [J1 cases, 12000 appels] fin %u, EX %u ligne %u, K = %ld, pile %u\n", f,
               erreurs_numero(diag.nature), diag.ligne, (long) valeur("K"), pile.sommet);
        VERIFIER(f == FIN_TERMINER && valeur("K") == 12001.0f);
        VERIFIER(programme(p2));
        f = lancer();
        printf("  [J2 cases, 14000 appels] fin %u, EX %u ligne %u, K = %ld, pile %u\n", f,
               erreurs_numero(diag.nature), diag.ligne, (long) valeur("K"), pile.sommet);
        VERIFIER(f == FIN_TERMINER && valeur("K") == 14001.0f);
    }

    /* --- F. récursion profonde : pile d'évaluation et cases au-delà de 64 Kio - */
    {
        static const char *const p1[] = { "10 X_&F(9000)", "20 TERMINER", "30 PROCEDURE &F(N) LOCAL N",
                                          "40 SI N=0 ALORS RESULTAT 0", "50 RESULTAT 1+&F(N-1)", NULL };
        static const char *const p2[] = { "10 X_&F(15000)", "20 TERMINER", "30 PROCEDURE &F(N) LOCAL N",
                                          "40 SI N=0 ALORS RESULTAT 0", "50 RESULTAT 1+&F(N-1)", NULL };
        VERIFIER(programme(p1));
        f = lancer();
        printf("  [F1 profondeur 9000] fin %u, EX %u, X = %ld, capacite pile %u\n", f,
               erreurs_numero(diag.nature), (long) valeur("X"), pile.capacite);
        VERIFIER(f == FIN_TERMINER && valeur("X") == 9000.0f);
        VERIFIER(programme(p2));
        f = lancer();
        printf("  [F2 profondeur 15000] fin %u, EX %u, X = %ld, capacite pile %u\n", f,
               erreurs_numero(diag.nature), (long) valeur("X"), pile.capacite);
        VERIFIER(f == FIN_TERMINER && valeur("X") == 15000.0f);
    }

    VERIF_FIN();
}
