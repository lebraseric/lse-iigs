/* sonde_performance.c — durées des primitives de l'interpréteur sur cible
   Niveau : sur cible. Chronomètre (clock(), 1/60 s) les opérations faites à
   chaque ligne ou chaque tour de FAIRE, puis les boucles (a) et (b) des
   essais d'Éric du 2026-10-25, console remplacée par le bouchon (pas de
   consultation du pilote). Le calcul en double d'IMP-0216, tel que
   l'interpréteur le faisait avant le format étendu (audit du code, AP-04),
   est reproduit ici pour en mesurer le gain dans le même essai. Les mesures
   marquées F4 refont les mêmes calculs sous le profil LSE F4, qui arrondit à
   23 bits (Interpréteur IMP-0280), pour en mesurer le coût. */

#include <time.h>
#include <float.h>
#include "../composants/compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/interprete.h"
#include "../../src/noyau/contexte_exec.h"
#include "../../src/noyau/pile.h"
#include "../../src/noyau/objets.h"
#include "../../src/noyau/donnees.h"
#include "../../src/noyau/fichiers.h"
#include "../../src/interfaces/interface_fichiers.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

void bouchon_console(const char *const *lignes);

#define N 500

static PileEvaluation pile;
static DiagnosticExecution diag;
static PositionExecution pos;
static const char *const aucune[] = { NULL };
static clock_t t0;

static void debut(void) { t0 = clock(); }

static void fin(const char *quoi, unsigned int n)
{
    clock_t t1 = clock();
    printf("  %-40s %8.3f ms\n", quoi,
           (double) (t1 - t0) * 1000.0 / (double) CLOCKS_PER_SEC / (double) n);
}

/* Calcul d'IMP-0216 tel que l'interpréteur le faisait avant AP-04 : variables
   double, résultat passé par valeur, comparé à FLT_MAX, rangé en float. */
static int ranger_double(double v, float *f)
{
    if (v > FLT_MAX || v < -FLT_MAX) return 0;
    *f = (float) v;
    return 1;
}

static int calcul_double(char op, const float *pa, const float *pb, float *pr)
{
    double a = *pa;
    double b = *pb;
    double r;
    if (op == '+') r = a + b;
    else if (op == '-') r = a - b;
    else if (op == '*') r = a * b;
    else {
        if (b == 0.0) return 0;
        r = a / b;
    }
    return ranger_double(r, pr);
}

static int charger(const char *const *lignes)
{
    unsigned int i;
    NatureErreur n;
    programme_vider();
    contexte_vider();
    pile_ramener(&pile, 0);
    for (i = 0; lignes[i] != NULL; i++) {
        if (!compiler(lignes[i])) return 0;
        if (!programme_inserer(&r.resultat.ligne_programme, &n)) return 0;
    }
    return 1;
}

static void boucle(const char *quoi, const char *const *lignes, unsigned int tours)
{
    if (!charger(lignes)) { printf("  %s : rejet\n", quoi); return; }
    bouchon_console(aucune);
    contexte_initialiser(0, 0, &pos);
    interprete_reinitialiser();
    debut();
    if (interprete_executer(&pile, &pos, &diag) != FIN_TERMINER) printf("  %s : fin anormale\n", quoi);
    fin(quoi, tours);
}

int main(void)
{
    static const char *const prog[] = { "10 X_1", "20 Y_2", "30 TERMINER", NULL };
    static const char *const a[] = { "10 FAIRE 10 POUR I_1 JUSQUA 200", "30 TERMINER", NULL };
    static const char *const b[] = { "10 FAIRE 20 POUR I_1 JUSQUA 200", "20 X_1", "30 TERMINER", NULL };
    static const char *const c[] = { "10 FAIRE 20 POUR I_1 JUSQUA 200", "20 X_I+1", "30 TERMINER", NULL };
    static const char *const e3[] = { "10 FAIRE 20 POUR I_1 JUSQUA 200", "20 X_I/3", "30 TERMINER", NULL };
    static const char *const f0[] = { "10 TABLEAU T[20000];T[1]_5", "20 TERMINER", NULL };
    static const char *const f1[] = { "10 TABLEAU T[20000];T[1]_5", "20 GARER T,1,'PERFT'", "30 TERMINER", NULL };
    static const char *const g1[] = { "10 CHARGER U,1,'PERFT'", "20 TERMINER", NULL };
    EntreeObjetCompile e;
    ReferenceResolue ref;
    unsigned int id;
    unsigned int i;
    unsigned char *o;
    volatile float f = 1.0f;
    volatile float g = 0.0f;
    volatile int k = 3;
    float v;
    unsigned char tampon[256];

    printf("SONDE PERFORMANCE (ms par operation)\n");
    programme_initialiser();
    pile_initialiser(&pile);
    charger(prog);
    contexte_initialiser(0, 0, &pos);
    interprete_executer(&pile, &pos, &diag);
    programme_chercher(20, &e);
    symboles_chercher("X", 1, SYM_IDENTIFICATEUR, &id);
    contexte_resoudre(id, &ref);

    debut(); for (i = 0; i < N; i++) { o = objets_resoudre(e.bloc); objets_relacher(e.bloc); }
    fin("objets_resoudre + objets_relacher", N);
    debut(); for (i = 0; i < N; i++) objets_copier(e.bloc, 0, tampon, e.longueur_octets);
    fin("objets_copier (une ligne)", N);
    debut(); for (i = 0; i < N; i++) programme_chercher(20, &e);
    fin("programme_chercher", N);
    debut(); for (i = 0; i < N; i++) programme_suivante(10, &e);
    fin("programme_suivante", N);
    debut(); for (i = 0; i < N; i++) { pile_empiler_nombre(&pile, 1.0f); pile_consommer(&pile, 1); }
    fin("pile_empiler_nombre + pile_consommer", N);
    debut(); for (i = 0; i < N; i++) contexte_lire_nombre(ref.cellule, USAGE_CALCUL, &v);
    fin("contexte_lire_nombre", N);
    debut(); for (i = 0; i < N; i++) contexte_affecter_nombre(ref.cellule, 2.0f);
    fin("contexte_affecter_nombre", N);
    debut(); for (i = 0; i < N; i++) contexte_resoudre(id, &ref);
    fin("contexte_resoudre", N);
    debut(); for (i = 0; i < N; i++) g = (float) k;
    fin("conversion int -> float", N);
    debut(); for (i = 0; i < N; i++) k = f > g;
    fin("comparaison float", N);

    {                                         /* chemin exact (IMP-0244) */
        float x1 = 5.0f;
        float x2 = 7.0f;
        float x3 = 0.0f;
        volatile int dec = 20;
        volatile unsigned long ul = 0;
        volatile int res = 0;
        debut(); for (i = 0; i < N; i++) res = donnees_comparer_nombres(&x1, &x2);
        fin("donnees_comparer_nombres", N);
        debut(); for (i = 0; i < N; i++) res = donnees_operation_entiere('+', &x1, &x2, &x3);
        fin("donnees_operation_entiere (5+7)", N);
        debut(); for (i = 0; i < N; i++) donnees_nombre_entier(200L, &x3);
        fin("donnees_nombre_entier (200)", N);
        debut(); for (i = 0; i < N; i++) donnees_opposer(&x1, &x3);
        fin("donnees_opposer", N);
        debut(); for (i = 0; i < N; i++) { pile_empiler_nombre_exact(&pile, &x1); pile_consommer(&pile, 1); }
        fin("pile_empiler_nombre_exact + consommer", N);
        debut(); for (i = 0; i < N; i++) ul = 1ul << dec;
        fin("decalage long variable (20)", N);
        debut(); for (i = 0; i < N; i++) ul = ul >> 3;
        fin("decalage long constant (3)", N);
    }

    {                                         /* format étendu (AP-04) : avant, après */
        float x1 = 1.5f;
        float x2 = 3.0f;
        float d1 = 0.1f;
        float d2 = 0.2f;
        float x3 = 0.0f;
        volatile int res = 0;
        debut(); for (i = 0; i < N; i++) res = calcul_double('/', &x1, &x2, &x3);
        fin("calcul en double (1.5/3), avant AP-04", N);
        debut(); for (i = 0; i < N; i++) res = donnees_operation_etendue('/', &x1, &x2, &x3);
        fin("donnees_operation_etendue (1.5/3)", N);
        debut(); for (i = 0; i < N; i++) res = calcul_double('+', &d1, &d2, &x3);
        fin("calcul en double (0.1+0.2), avant AP-04", N);
        debut(); for (i = 0; i < N; i++) res = donnees_operation_etendue('+', &d1, &d2, &x3);
        fin("donnees_operation_etendue (0.1+0.2)", N);
        configuration_session.profil = PROFIL_T1600_F4;          /* IMP-0280 */
        debut(); for (i = 0; i < N; i++) res = donnees_operation_etendue('/', &x1, &x2, &x3);
        fin("donnees_operation_etendue (1.5/3), F4", N);
        debut(); for (i = 0; i < N; i++) res = donnees_operation_etendue('+', &d1, &d2, &x3);
        fin("donnees_operation_etendue (0.1+0.2), F4", N);
        configuration_session.profil = PROFIL_MITRA15_V8;
    }

    boucle("(a) FAIRE 10 vide, par tour", a, 200);
    boucle("(b) FAIRE 20, X_1, par tour", b, 200);
    boucle("(d) FAIRE 20, X_I+1, par tour", c, 200);
    boucle("(e) FAIRE 20, X_I/3, par tour", e3, 200);
    configuration_session.profil = PROFIL_T1600_F4;              /* IMP-0280 */
    boucle("(e) FAIRE 20, X_I/3, par tour, F4", e3, 200);
    configuration_session.profil = PROFIL_MITRA15_V8;

    /* GARER et CHARGER d'un tableau de 80 Ko (audit du code, AP-17), dans une
       bibliothèque à côté de la sonde ; le fichier est ensuite supprimé */
    if (plateforme_bibliotheque_ouvrir("9:BIBPERF") == PLAT_OK) {
        fichiers_fixer_quota(1000);
        plateforme_espace_vider(ESPACE_TEMPORAIRES);
        boucle("(f0) TABLEAU T[20000] seul", f0, 1);
        boucle("(f) TABLEAU T[20000] et GARER (80 Ko)", f1, 1);
        printf("  PERFT : %lu secteurs\n", (unsigned long) fichiers_secteurs(0, "PERFT"));
        boucle("(g) CHARGER (80 Ko)", g1, 1);
        /* remplacement de même taille : (h) écrit en fin de fichier, qui grandit ;
           (i) reprend l'étendue libérée par (h), sans agrandir (IMP-0247) */
        boucle("(h) TABLEAU et GARER, remplacement", f1, 1);
        printf("  PERFT : %lu secteurs\n", (unsigned long) fichiers_secteurs(0, "PERFT"));
        boucle("(i) TABLEAU et GARER, sans agrandir", f1, 1);
        printf("  PERFT : %lu secteurs\n", (unsigned long) fichiers_secteurs(0, "PERFT"));
        plateforme_espace_vider(ESPACE_TEMPORAIRES);
    } else {
        printf("  bibliotheque 9:BIBPERF indisponible\n");
    }
    printf("FIN SONDE\n");
    return 0;
}
