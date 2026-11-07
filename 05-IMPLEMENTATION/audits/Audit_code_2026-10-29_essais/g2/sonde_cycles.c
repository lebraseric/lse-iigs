/* sonde_cycles.c — essai d'audit G2 (2026-10-29), hors dépôt.
   Lancé par « iix --cycles tests/sonde_cycles MODE » : le nombre de cycles
   65816 comptés par Golden Gate, diminué de celui du mode de référence,
   donne le coût d'un traitement. Les modes d'étalonnage reprennent des
   opérations chronométrées sur l'Apple IIGS (Mesures_performance, P4.OUT)
   pour convertir les cycles en millisecondes de la machine d'Éric. */

#include <stdlib.h>
#include "../composants/compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/interprete.h"
#include "../../src/noyau/contexte_exec.h"
#include "../../src/noyau/pile.h"
#include "../../src/noyau/objets.h"
#include "../../src/noyau/donnees.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

void bouchon_console(const char *const *lignes);

#define M 2000

static PileEvaluation pile;
static DiagnosticExecution diag;
static PositionExecution pos;
static const char *const aucune[] = { NULL };
static char ligne[200];
static unsigned int limite = 65535u;   /* mode kl:N : N premières lignes */

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

static void boucle(const char *const *lignes, int executer)
{
    if (!charger(lignes)) { printf("rejet\n"); return; }
    bouchon_console(aucune);
    contexte_initialiser(0, 0, &pos);
    interprete_reinitialiser();
    if (executer && interprete_executer(&pile, &pos, &diag) != FIN_TERMINER) printf("fin anormale\n");
}

/* Corpus : compile chaque ligne (mode compiler) ou la lit seulement.
   vider : 1, tables vidées quand le numéro ne croît pas (un programme) ;
   0, jamais (table de plus en plus grande). */
static void corpus(const char *chemin, int compiler_, int vider, unsigned int *nl, unsigned int *na)
{
    FILE *f = fopen(chemin, "r");
    unsigned int n;
    unsigned int i;
    unsigned int numero;
    unsigned int precedent = 0;
    if (f == NULL) { printf("corpus absent\n"); return; }
    while (*nl < limite && fgets(ligne, sizeof ligne, f) != NULL) {
        n = (unsigned int) strlen(ligne);
        while (n > 0 && (ligne[n - 1] == '\n' || ligne[n - 1] == '\r')) ligne[--n] = '\0';
        if (n == 0) continue;
        numero = 0;
        for (i = 0; ligne[i] >= '0' && ligne[i] <= '9' && numero < 1000; i++)
            numero = numero * 10 + (unsigned int) (ligne[i] - '0');
        if (vider && numero <= precedent) symboles_vider();
        precedent = numero;
        (*nl)++;
        if (!compiler_) continue;
        configuration_session.profil = PROFIL_MITRA15_V8;
        if (!compilateur_compiler(ligne, n, &r, &d)) {
            configuration_session.profil = PROFIL_T1600_V4;
            if (!compilateur_compiler(ligne, n, &r, &d)) continue;
        }
        (*na)++;
        objets_liberer(r.resultat.ligne_programme.bloc);
    }
    fclose(f);
}

/* Programme synthétique : nl lignes, ns symboles distincts de 1 à 3
   lettres, chaque ligne « n Sx_Sy+Sz*Sw » (4 occurrences). */
static void nom(unsigned int k, char *t)
{
    static const char l[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (k < 26) { t[0] = l[k]; t[1] = '\0'; return; }
    k -= 26;
    t[0] = l[(k / 26) % 26]; t[1] = l[k % 26]; t[2] = '\0';
}

static void synthetique(unsigned int nl, unsigned int ns, int compiler_)
{
    unsigned int i;
    char a[4], b[4], c[4], e[4];
    for (i = 0; i < nl; i++) {
        nom((i * 7u) % ns, a); nom((i * 13u + 5u) % ns, b);
        nom((i * 17u + 11u) % ns, c); nom((i * 29u + 3u) % ns, e);
        sprintf(ligne, "%u %s_%s+%s*%s", (i % 250u) + 1u, a, b, c, e);
        if (!compiler_) continue;
        if (!compiler(ligne)) { printf("rejet [%s]\n", ligne); continue; }
        objets_liberer(r.resultat.ligne_programme.bloc);
    }
}

int main(int argc, char *argv[])
{
    static const char *const prog[] = { "10 X_1", "20 Y_2", "30 TERMINER", NULL };
    static const char *const a[] = { "10 FAIRE 10 POUR I_1 JUSQUA 200", "30 TERMINER", NULL };
    static const char *const b[] = { "10 FAIRE 20 POUR I_1 JUSQUA 200", "20 X_1", "30 TERMINER", NULL };
    const char *mode = argc > 1 ? argv[1] : "0";
    unsigned int i;
    unsigned int nl = 0;
    unsigned int na = 0;
    unsigned int id;
    float x1 = 5.0f;
    float x2 = 7.0f;
    float x3 = 0.0f;
    volatile int res = 0;
    char t[8];

    programme_initialiser();
    pile_initialiser(&pile);
    charger(prog);

    if (strcmp(mode, "op") == 0)
        for (i = 0; i < M; i++) res = donnees_operation_entiere('+', &x1, &x2, &x3);
    else if (strcmp(mode, "ne") == 0)
        for (i = 0; i < M; i++) donnees_nombre_entier(200L, &x3);
    else if (strcmp(mode, "cmp") == 0)
        for (i = 0; i < M; i++) res = donnees_comparer_nombres(&x1, &x2);
    else if (strcmp(mode, "a0") == 0) boucle(a, 0);
    else if (strcmp(mode, "a") == 0) boucle(a, 1);
    else if (strcmp(mode, "b0") == 0) boucle(b, 0);
    else if (strcmp(mode, "b") == 0) boucle(b, 1);
    else if (strcmp(mode, "k0") == 0 || strcmp(mode, "k1") == 0 || strcmp(mode, "k2") == 0) {
        symboles_vider();
        corpus("../tests/historiques/corpus/inrdp75.txt", mode[1] != '0', mode[1] == '1', &nl, &na);
        corpus("../tests/historiques/corpus/lse15.txt", mode[1] != '0', mode[1] == '1', &nl, &na);
        printf("lignes %u, acceptees %u, symboles en fin %u, constantes %u\n", nl, na,
               symboles_nombre(), constantes_nombre());
    } else if (mode[0] == 'x' && argc > 2) {      /* x TEXTE : 100 compilations */
        for (i = 0; i < 100; i++) {
            if (!compiler(argv[2])) { printf("rejet\n"); break; }
            objets_liberer(r.type == LSE_LIGNE_PROGRAMME ? r.resultat.ligne_programme.bloc : r.resultat.bloc_bureau);
        }
    } else if (mode[0] == 'k' && mode[1] == 'l') {
        limite = (unsigned int) atoi(mode + 3);
        symboles_vider();
        corpus("../tests/historiques/corpus/inrdp75.txt", 1, 1, &nl, &na);
        printf("lignes %u, acceptees %u\n", nl, na);
    } else if (mode[0] == 's') {                 /* sNL,NS : synthétique */
        unsigned int nl_ = (unsigned int) atoi(mode + 3);
        unsigned int ns_ = (unsigned int) atoi(strchr(mode, ',') + 1);
        symboles_vider();
        if (mode[1] == '0' || mode[1] == '1') {   /* préremplir la table : ns symboles */
            for (i = 0; i < ns_; i++) { nom(i, t); symboles_chercher_ou_creer(t, (unsigned char) strlen(t), SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &id); }
        }
        synthetique(nl_, ns_, mode[1] == '1' || mode[1] == '2');
        printf("symboles %u\n", symboles_nombre());
    } else if (mode[0] == 'r') {                 /* rNS : reconstruction d'une table de NS symboles (APPELER) */
        unsigned int ns_ = (unsigned int) atoi(mode + 1);
        symboles_vider();
        for (i = 0; i < ns_; i++) { nom(i, t); symboles_chercher_ou_creer(t, (unsigned char) strlen(t), SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &id); }
        printf("symboles %u\n", symboles_nombre());
    }
    return 0;
}
