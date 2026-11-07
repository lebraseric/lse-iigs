/* composants_horloge.c — TEM, DAT, ALE
   EXI: EXI-0011, EXI-0027  VM: VM-2205, VM-2206
   IMP: IMP-0223 à IMP-0225
   Spécification des tests : chapitre Interpréteur §4 ter ; exemple de DAT de
   LSE15 §4-4.2 ; propriétés d'ALE d'INRDP75 §4-2 et LSE15 §3-2.2.
   Niveau : composants. Horloge : tests/bouchons/horloge_bouchon.c ;
   console : tests/bouchons/console_bouchon.c. */

#include "compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/interprete.h"
#include "../../src/noyau/donnees.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

extern char bouchon_affichage[];
void bouchon_console(const char *const *lignes);
void bouchon_horloge(unsigned int annee, unsigned int mois, unsigned int jour,
                     unsigned int heure, unsigned int minute, unsigned int seconde);

static PileEvaluation pile;
static DiagnosticExecution diag;
static PositionExecution pos;
static const char *const aucune[] = { NULL };

/* Compile et exécute le programme ; renvoie la fin d'exécution, 99 si rejet. */
static FinExecution executer(const char *const *lignes)
{
    unsigned int i;
    NatureErreur n;
    programme_vider();
    contexte_vider();
    pile_ramener(&pile, 0);
    for (i = 0; lignes[i] != NULL; i++) {
        if (!compiler(lignes[i])) {
            printf("  [%s] rejet : C %u\n", lignes[i], erreurs_numero(d.nature));
            return 99;
        }
        if (!programme_inserer(&r.resultat.ligne_programme, &n)) return 99;
    }
    bouchon_console(aucune);
    interprete_debut_de_ligne(1);
    contexte_initialiser(0, 0, &pos);
    return interprete_executer(&pile, &pos, &diag);
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

#define EXECUTER(fin, ...) \
    do { static const char *const p_[] = { __VA_ARGS__, NULL }; \
         VERIFIER(executer(p_) == (fin)); } while (0)

int main(void)
{
    float attendu;
    float a;
    float b;

    VERIF_DEBUT("composants/horloge");
    programme_initialiser();
    pile_initialiser(&pile);

    /* --- TEM (IMP-0224) ---------------------------------------------------------- */
    bouchon_horloge(1972, 11, 3, 13, 3, 52);
    EXECUTER(FIN_TERMINER, "10 T_TEM()", "20 TERMINER");
    attendu = 47032.0f;                                 /* 13 h 3 mn 52 s */
    VERIFIER(valeur("T") == attendu);
    bouchon_horloge(2026, 10, 24, 0, 0, 0);
    EXECUTER(FIN_TERMINER, "10 T_TEM()", "20 TERMINER");
    VERIFIER(valeur("T") == 0.0f);
    bouchon_horloge(2026, 10, 24, 23, 59, 59);
    EXECUTER(FIN_TERMINER, "10 T_TEM()", "20 TERMINER");
    attendu = 86399.0f;
    VERIFIER(valeur("T") == attendu);

    /* --- DAT (IMP-0224 ; exemple de LSE15 §4-4.2) ---------------------------------- */
    bouchon_horloge(1972, 11, 3, 13, 3, 52);
    EXECUTER(FIN_TERMINER, "10 CHAINE D;D_DAT()", "20 TERMINER");
    VERIFIER(chaine_vaut("D", "03/11/72 13:03:52"));
    EXECUTER(FIN_TERMINER, "10 AFFICHER DAT()", "20 TERMINER");
    VERIFIER(strcmp(bouchon_affichage, "\r\n03/11/72 13:03:52") == 0);
    bouchon_horloge(2026, 10, 24, 15, 20, 42);          /* année : deux chiffres de poids faible */
    EXECUTER(FIN_TERMINER, "10 CHAINE D;D_DAT();L_LGR(D)", "20 TERMINER");
    VERIFIER(chaine_vaut("D", "24/10/26 15:20:42"));
    VERIFIER(valeur("L") == 17.0f);
    bouchon_horloge(2000, 1, 1, 0, 0, 0);
    EXECUTER(FIN_TERMINER, "10 CHAINE D;D_DAT()", "20 TERMINER");
    VERIFIER(chaine_vaut("D", "01/01/00 00:00:00"));

    /* --- horloge du système : heure au-delà de 24, date non réglée (IMP-0279 ; NBOR) -- */
    bouchon_horloge(0, 0, 0, 28, 26, 42);
    EXECUTER(FIN_TERMINER, "10 T_TEM()", "20 TERMINER");
    attendu = 102402.0f;
    VERIFIER(valeur("T") == attendu);
    EXECUTER(FIN_TERMINER, "10 CHAINE D;D_DAT()", "20 TERMINER");
    VERIFIER(chaine_vaut("D", "00/00/00 28:26:42"));

    /* --- ALE(E), 0 < E <= 1 (IMP-0225) ---------------------------------------------- */
    EXECUTER(FIN_TERMINER, "10 A_ALE(0.5);B_ALE(0.5)", "20 TERMINER");
    attendu = (float) (15659871.0 / 16777216.0);
    VERIFIER(valeur("A") == attendu && valeur("B") == attendu);   /* dépend seulement de E */
    EXECUTER(FIN_TERMINER, "10 A_ALE(0.5);B_ALE(A)", "20 TERMINER");
    attendu = (float) (13642034.0 / 16777216.0);
    VERIFIER(valeur("B") == attendu);                              /* série reproductible */
    EXECUTER(FIN_TERMINER, "10 A_ALE(1)", "20 TERMINER");
    attendu = (float) (7271263.0 / 16777216.0);
    VERIFIER(valeur("A") == attendu);                              /* point H6 */
    EXECUTER(FIN_TERMINER, "10 X_0.3;N_0", "20 FAIRE 30 POUR I_1 JUSQUA 200",
             "30 X_ALE(X);SI X<=0 OU X>=1 ALORS N_1", "40 TERMINER");
    VERIFIER(valeur("N") == 0.0f && valeur("I") == 201.0f);        /* toujours dans ]0,1[ */

    /* --- ALE(0) (IMP-0225) ------------------------------------------------------------- */
    EXECUTER(FIN_TERMINER, "10 A_ALE(0);B_ALE(0)", "20 TERMINER");
    a = valeur("A");
    b = valeur("B");
    VERIFIER(a > 0.0f && a < 1.0f && b > 0.0f && b < 1.0f && a != b);
    EXECUTER(FIN_TERMINER, "10 A_ALE(0)", "20 TERMINER");
    VERIFIER(valeur("A") != a && valeur("A") != b);                /* l'état continue */

    /* --- erreurs ---------------------------------------------------------------------- */
    EXECUTER(FIN_ERREUR, "10 A_ALE(-0.1)", "20 TERMINER");
    VERIFIER(diag.nature == ERR_E_ARGUMENT_ALE && diag.ligne == 10);
    EXECUTER(FIN_ERREUR, "10 A_ALE(1.5)", "20 TERMINER");
    VERIFIER(diag.nature == ERR_E_ARGUMENT_ALE);

    contexte_vider();
    programme_vider();
    pile_liberer(&pile);
    VERIF_FIN();
}
