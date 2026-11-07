/* audit_g3.c — essais de l'audit du groupe 3 (moniteur, programme,
   décompilateur, erreurs). Copie de travail seulement : ne fait pas partie
   du dépôt. Console : bouchon ; horloge : bouchon. */

#include <string.h>
#include "../commun/verif.h"
#include "../../src/noyau/moniteur.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/decompilateur.h"
#include "../../src/noyau/compilateur.h"
#include "../../src/noyau/contexte_exec.h"
#include "../../src/noyau/pile.h"
#include "../../src/interfaces/types_communs.h"
#include "../../src/interfaces/interface_fichiers.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

segment "suite";

extern char bouchon_affichage[];
void bouchon_console(const char *const *lignes);
void bouchon_interruption(unsigned int rang);

#define X "\x13"
#define ESC "\x1B"

static const char *session(const char *const *reponses)
{
    bouchon_console(reponses);
    moniteur_session();
    return bouchon_affichage;
}

static int contient(const char *attendu)
{
    if (strstr(bouchon_affichage, attendu) != NULL) return 1;
    printf("  [%s]\n    ne contient pas [%s]\n", bouchon_affichage, attendu);
    return 0;
}

#define SESSION(...) \
    do { static const char *const r_[] = { __VA_ARGS__, NULL }; session(r_); } while (0)

/* --- mesure de la pile du 65816 ------------------------------------------- */

#define ZONE 8000u

/* Peint (peindre = 1) ou mesure (0) la zone de pile située sous l'appelant.
   Une seule fonction : le cadre est le même aux deux appels. */
static unsigned int zone_op(int peindre, unsigned long *adresse)
{
    volatile unsigned char zone[ZONE];
    unsigned int i;
    *adresse = (unsigned long) &zone[0];
    if (peindre) {
        for (i = 0; i < ZONE; i++) zone[i] = 0xA5;
        return 0;
    }
    for (i = 0; i < ZONE && zone[i] == 0xA5; i++)
        ;
    return i;
}

static unsigned int sp_lire(void)
{
    unsigned int v;
    asm {
        tsc
        sta v
    }
    return v;
}

static unsigned int sp_main = 0;

static void mesurer(const char *nom, const char *const *reponses)
{
    unsigned long a0;
    unsigned long a1;
    unsigned int libres;
    unsigned int sp = sp_lire();

    zone_op(1, &a0);
    session(reponses);
    libres = zone_op(0, &a1);
    printf("  PILE %-28s : %4u octets sous la mesure (SP mesure %04X, SP main %04X, zone %06lX-%06lX%s)\n",
           nom, ZONE - libres, sp, sp_main, a0, a0 + ZONE - 1, a0 == a1 ? "" : " DECALEE");
}

#define MESURER(nom, ...) \
    do { static const char *const r_[] = { __VA_ARGS__, NULL }; mesurer(nom, r_); } while (0)

/* --- réplique instrumentée de decompilateur_ecrire_reel_chiffres ---------- */

static unsigned long n_mul;     /* multiplications et divisions double */
static unsigned long n_cmp;     /* comparaisons double */
static unsigned long n_autre;   /* additions, conversions */

static double r_puissance_dix(int k)
{
    double p = 1.0;
    while (k > 0) { p = p * 10.0; n_mul++; k--; }
    return p;
}

static double r_echelle(double x, int n)
{
    double dix22 = r_puissance_dix(22);
    while (n > 22)  { x = x * dix22; n_mul++; n -= 22; }
    while (n < -22) { x = x / dix22; n_mul++; n += 22; }
    n_mul++;
    return n < 0 ? x / r_puissance_dix(-n) : x * r_puissance_dix(n);
}

static long r_arrondi(double y)
{
    n_autre += 2;
    return (long) (y + 0.5);
}

static unsigned int r_ecrire(float valeur, unsigned int chiffres, char *sortie)
{
    float f = valeur;
    double x = (double) f;
    char ch[8];
    long m;
    long bas;
    long haut;
    int e = 0;
    int nb;
    int i;
    int c = (int) chiffres;
    unsigned int lg = 0;

    n_autre++;
    if (c < 1) c = 1;
    if (c > 7) c = 7;
    bas = (long) r_puissance_dix(c - 1); n_autre++;
    haut = bas * 10L;
    n_cmp++;
    if (x < 0.0) { sortie[lg++] = '-'; x = -x; n_autre++; }
    n_cmp++;
    if (x == 0.0) { sortie[lg++] = '0'; sortie[lg] = '\0'; return lg; }
    n_cmp++;
    if (x >= 1.0) { for (;;) { double t = r_echelle(x, -(e + 1)); n_cmp++; if (!(t >= 1.0)) break; e++; } }
    else          { for (;;) { double t = r_echelle(x, -e); n_cmp++; if (!(t < 1.0)) break; e--; } }
    m = r_arrondi(r_echelle(x, c - 1 - e));
    if (m >= haut) { e++; m = r_arrondi(r_echelle(x, c - 1 - e)); }
    if (m < bas)   { e--; m = r_arrondi(r_echelle(x, c - 1 - e)); }
    for (i = c - 1; i >= 0; i--) { ch[i] = (char) ('0' + (int) (m % 10)); m /= 10; }
    nb = c;
    while (nb > 1 && ch[nb - 1] == '0') nb--;
    if (e >= -3 && e <= 5) {
        if (e >= 0) {
            for (i = 0; i <= e; i++) sortie[lg++] = i < nb ? ch[i] : '0';
            if (nb > e + 1) { sortie[lg++] = '.'; for (i = e + 1; i < nb; i++) sortie[lg++] = ch[i]; }
        } else {
            sortie[lg++] = '0'; sortie[lg++] = '.';
            for (i = 0; i < -e - 1; i++) sortie[lg++] = '0';
            for (i = 0; i < nb; i++) sortie[lg++] = ch[i];
        }
    } else {
        sortie[lg++] = ch[0];
        if (nb > 1) { sortie[lg++] = '.'; for (i = 1; i < nb; i++) sortie[lg++] = ch[i]; }
        sortie[lg++] = 'E';
        sortie[lg++] = e < 0 ? '-' : '+';
        if (e < 0) e = -e;
        sortie[lg++] = (char) ('0' + e / 10);
        sortie[lg++] = (char) ('0' + e % 10);
    }
    sortie[lg] = '\0';
    return lg;
}

/* --- variante proposée : puissances de dix exactes en table --------------- */

static const double table_p10[23] = {
    1.0, 1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5, 1.0e6, 1.0e7, 1.0e8, 1.0e9, 1.0e10,
    1.0e11, 1.0e12, 1.0e13, 1.0e14, 1.0e15, 1.0e16, 1.0e17, 1.0e18, 1.0e19, 1.0e20,
    1.0e21, 1.0e22
};
static const long table_l10[8] = { 1L, 10L, 100L, 1000L, 10000L, 100000L, 1000000L, 10000000L };

static double t_echelle(double x, int n)
{
    while (n > 22)  { x = x * table_p10[22]; n_mul++; n -= 22; }
    while (n < -22) { x = x / table_p10[22]; n_mul++; n += 22; }
    n_mul++;
    return n < 0 ? x / table_p10[-n] : x * table_p10[n];
}

static unsigned int t_ecrire(float valeur, unsigned int chiffres, char *sortie)
{
    float f = valeur;
    double x = (double) f;
    char ch[8];
    long m;
    long bas;
    long haut;
    int e = 0;
    int nb;
    int i;
    int c = (int) chiffres;
    unsigned int lg = 0;

    if (c < 1) c = 1;
    if (c > 7) c = 7;
    bas = table_l10[c - 1];
    haut = bas * 10L;
    n_cmp++;
    if (x < 0.0) { sortie[lg++] = '-'; x = -x; }
    n_cmp++;
    if (x == 0.0) { sortie[lg++] = '0'; sortie[lg] = '\0'; return lg; }
    n_cmp++;
    if (x >= 1.0) { for (;;) { double t = t_echelle(x, -(e + 1)); n_cmp++; if (!(t >= 1.0)) break; e++; } }
    else          { for (;;) { double t = t_echelle(x, -e); n_cmp++; if (!(t < 1.0)) break; e--; } }
    m = r_arrondi(t_echelle(x, c - 1 - e));
    if (m >= haut) { e++; m = r_arrondi(t_echelle(x, c - 1 - e)); }
    if (m < bas)   { e--; m = r_arrondi(t_echelle(x, c - 1 - e)); }
    for (i = c - 1; i >= 0; i--) { ch[i] = (char) ('0' + (int) (m % 10)); m /= 10; }
    nb = c;
    while (nb > 1 && ch[nb - 1] == '0') nb--;
    if (e >= -3 && e <= 5) {
        if (e >= 0) {
            for (i = 0; i <= e; i++) sortie[lg++] = i < nb ? ch[i] : '0';
            if (nb > e + 1) { sortie[lg++] = '.'; for (i = e + 1; i < nb; i++) sortie[lg++] = ch[i]; }
        } else {
            sortie[lg++] = '0'; sortie[lg++] = '.';
            for (i = 0; i < -e - 1; i++) sortie[lg++] = '0';
            for (i = 0; i < nb; i++) sortie[lg++] = ch[i];
        }
    } else {
        sortie[lg++] = ch[0];
        if (nb > 1) { sortie[lg++] = '.'; for (i = 1; i < nb; i++) sortie[lg++] = ch[i]; }
        sortie[lg++] = 'E';
        sortie[lg++] = e < 0 ? '-' : '+';
        if (e < 0) e = -e;
        sortie[lg++] = (char) ('0' + e / 10);
        sortie[lg++] = (char) ('0' + e % 10);
    }
    sortie[lg] = '\0';
    return lg;
}

/* Équivalence de la variante sur un échantillon de réels. */
static void equivalence(void)
{
    char a[20];
    char b[20];
    unsigned long i;
    unsigned long ecarts = 0;
    unsigned long essais = 0;
    unsigned int c;
    unsigned long graine = 12345ul;
    float v;
    double r;
    int k;

    for (k = 0; k <= 22; k++)
        if (table_p10[k] != r_puissance_dix(k)) { printf("  TABLE 10^%d differente\n", k); ecarts++; }
    for (i = 0; i < 1500ul; i++) {
        graine = graine * 1103515245ul + 12345ul;
        r = (double) ((graine >> 8) & 0xFFFFFul) / 1048576.0;      /* [0,1) */
        k = (int) ((graine >> 3) % 77ul) - 38;                     /* exposant -38..38 */
        v = (float) (r * 10.0 * (k >= 0 ? r_puissance_dix(k > 22 ? 22 : k) : 1.0 / r_puissance_dix(-k > 22 ? 22 : -k)));
        if (k > 22) v = (float) ((double) v * r_puissance_dix(k - 22));
        if (k < -22) v = (float) ((double) v / r_puissance_dix(-k - 22));
        if (i & 1) v = -v;
        for (c = 6; c <= 7; c++) {
            decompilateur_ecrire_reel_chiffres(v, c, a);
            t_ecrire(v, c, b);
            essais++;
            if (strcmp(a, b) != 0) { if (ecarts < 10) printf("  ECART %s %s\n", a, b); ecarts++; }
        }
    }
    {   /* bords : puissances de dix, demi-unités, extrêmes */
        static const float bords[] = { 1.0f, 10.0f, 100.0f, 999999.5f, 9999999.0f, 0.1f, 0.001f,
                                       0.0001f, 9.9999995f, 99999.95f, 3.4028235E38f, 1.1754944E-38f,
                                       1.0E-38f, 5.0E-39f, 1.0E38f, 123456.75f, 0.00099999f };
        for (k = 0; k < (int) (sizeof bords / sizeof bords[0]); k++)
            for (c = 1; c <= 7; c++) {
                decompilateur_ecrire_reel_chiffres(bords[k], c, a);
                t_ecrire(bords[k], c, b);
                essais++;
                if (strcmp(a, b) != 0) { if (ecarts < 20) printf("  ECART bord %s %s\n", a, b); ecarts++; }
            }
    }
    printf("  EQUIVALENCE table : %lu essais, %lu ecart(s)\n", essais, ecarts);
    VERIFIER_EGAL(0, ecarts);
}

static void compter_table(float v)
{
    char b[20];
    float w = v;
    n_mul = 0; n_cmp = 0; n_autre = 0;
    t_ecrire(w, 7, b);
    printf("  REEL TABLE %-12s : %3lu mul/div, %2lu comparaisons\n", b, n_mul, n_cmp);
}

static void compter(float v)
{
    char a[20];
    char b[20];
    float w = v;
    n_mul = 0; n_cmp = 0; n_autre = 0;
    r_ecrire(w, 7, b);
    decompilateur_ecrire_reel_chiffres(w, 7, a);
    printf("  REEL %-12s : %3lu mul/div, %2lu comparaisons, %2lu autres (replique %s)\n",
           a, n_mul, n_cmp, n_autre, strcmp(a, b) == 0 ? "conforme" : b);
    VERIFIER(strcmp(a, b) == 0);
}

/* Profondeur de la seule décompilation d'une ligne (compilée hors mesure). */
static void decomp_isolee(const char *ligne)
{
    ResultatCompilation r;
    DiagnosticCompilation d;
    char texte[DECOMPILATEUR_LONGUEUR_MAX + 1];
    unsigned int lg;
    unsigned long a0;
    unsigned long a1;
    unsigned int libres;

    if (!compilateur_compiler(ligne, (unsigned int) strlen(ligne), &r, &d)) {
        printf("  DECOMP compilation refusee (%u)\n", d.nature);
        return;
    }
    zone_op(1, &a0);
    decompilateur_decompiler(&r.resultat.ligne_programme, texte, &lg);
    libres = zone_op(0, &a1);
    printf("  PILE decompilation seule        : %4u octets [%s]\n", ZONE - libres, texte);
    objets_liberer(r.resultat.ligne_programme.bloc);
}

/* --- pile d'évaluation laissée par une ligne de bureau --------------------- */

int main(void)
{
    sp_main = sp_lire();
    VERIF_DEBUT("audit/g3");
    programme_initialiser();
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBG3"));
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_espace_vider(ESPACE_DONNEES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
    plateforme_fichier_supprimer(ESPACE_RACINE, "IDENTIFICATIONS");

    printf("  sizeof EntreeObjetCompile %u, ResultatCompilation %u, DiagnosticCompilation %u, "
           "ElementPile %u, PileEvaluation %u\n",
           (unsigned) sizeof(EntreeObjetCompile), (unsigned) sizeof(ResultatCompilation),
           (unsigned) sizeof(DiagnosticCompilation), (unsigned) sizeof(ElementPile),
           (unsigned) sizeof(PileEvaluation));

    /* D1 : ESC pendant LIRE A,B ; ligne de bureau en erreur ; CONTINUER */
    SESSION("BO" X, "10 LIRE A,B" X, "20 AFFICHER A,B" X, "30 TERMINER" X, "EX" X, "10" X,
            "1", ESC, "?A" X, "CO" X, "2", "AU" X);
    printf("  D1 temoin : [%s]\n", bouchon_affichage);
    VERIFIER(contient("CONTINUER\r\n\a2\r\n\r\n1 2 \r\nTERMINE"));
    SESSION("BO" X, "10 LIRE A,B" X, "20 AFFICHER A,B" X, "30 TERMINER" X, "EX" X, "10" X,
            "1", ESC, "?A+C" X, "CO" X, "2", "AU" X);
    printf("  D1 : [%s]\n", bouchon_affichage);
    VERIFIER(contient("CONTINUER\r\n\a2\r\n\r\n1 2 \r\nTERMINE"));

    /* D2 : ESC pendant LIRE A,B ; LIRE X en bureau interrompu ; CONTINUER */
    SESSION("BO" X, "10 LIRE A,B" X, "20 AFFICHER A,B" X, "30 TERMINER" X, "EX" X, "10" X,
            "1", ESC, "LIRE X" X, ESC, "CO" X, "2", "AU" X);
    printf("  D2 : [%s]\n", bouchon_affichage);
    VERIFIER(contient("CONTINUER\r\n\a2\r\n\r\n1 2 \r\nTERMINE"));

    /* D3 : PAUSE dans une fonction ; ligne de bureau en erreur ; CONTINUER */
    SESSION("BO" X, "10 X_&F(1)+1" X, "20 AFFICHER X" X, "30 TERMINER" X,
            "40 PROCEDURE &F(A) LOCAL A" X, "50 PAUSE" X, "60 RESULTAT A*10" X,
            "EX" X, "10" X, "?A" X, "CO" X, "AU" X);
    printf("  D3 temoin : [%s]\n", bouchon_affichage);
    SESSION("BO" X, "10 X_&F(1)+1" X, "20 AFFICHER X" X, "30 TERMINER" X,
            "40 PROCEDURE &F(A) LOCAL A" X, "50 PAUSE" X, "60 RESULTAT A*10" X,
            "EX" X, "10" X, "?A+C" X, "CO" X, "AU" X);
    printf("  D3 : [%s]\n", bouchon_affichage);
    VERIFIER(contient("CONTINUER\r\n11 \r\nTERMINE"));

    /* D4 : SI en bureau dans un programme suspendu à une PAUSE de fonction :
       LIRE en bureau interrompu, puis CONTINUER */
    SESSION("BO" X, "10 X_&F(1)+1" X, "20 AFFICHER X" X, "30 TERMINER" X,
            "40 PROCEDURE &F(A) LOCAL A" X, "50 PAUSE" X, "60 RESULTAT A*10" X,
            "EX" X, "10" X, "LIRE Y" X, ESC, "CO" X, "AU" X);
    printf("  D4 : [%s]\n", bouchon_affichage);
    VERIFIER(contient("CONTINUER\r\n11 \r\nTERMINE"));

    /* D5 : reste de la ligne tapée pour un programme terminé, puis LIRE en bureau */
    SESSION("BO" X, "10 LIRE A" X, "20 TERMINER" X, "EX" X, "10" X, "1 2", "LIRE X" X, "7", "?X" X, "AU" X);
    printf("  D5 : [%s]\n", bouchon_affichage);
    /* D6 : ESC pendant la lecture d'un tableau, LIRE en bureau, CONTINUER */
    SESSION("BO" X, "10 TABLEAU T[3]" X, "20 LIRE T" X, "30 AFFICHER T" X, "40 TERMINER" X,
            "EX" X, "10" X, "1 2", ESC, "LIRE X" X, "9", "CO" X, "3", "4", "5", "AU" X);
    printf("  D6 : [%s]\n", bouchon_affichage);
    VERIFIER(contient("CONTINUER\r\n\a3\r\n"));
    VERIFIER(contient("1 2 3"));
    /* D7 : ENCODER qui n'ajoute aucune ligne, puis CONTINUER */
    SESSION("BO" X, "1 CHAINE C;C_'100 X_1'!EQC(0);GARER C,1,'EE'" X, "2 PAUSE" X, "3 AFFICHER 'FIN'" X,
            "4 TERMINER" X, "EX" X, "1" X, "EN" X, "EE,200" X, "CO" X, "AU" X);
    printf("  D7 : [%s]\n", bouchon_affichage);
    VERIFIER(contient("CONTINUER\r\nFIN") || contient("CONTINUER\r\n\r\nFIN"));

    /* --- coût de l'écriture d'un réel ------------------------------------- */
    compter(3.0f);
    compter(37.0f);
    compter(200.0f);
    compter(12345.0f);
    compter(3.14159f);
    compter(0.001f);
    compter(0.5f);
    compter(65535.0f);
    compter(1.0E10f);
    compter(1.0E30f);
    compter(1.0E-30f);
    compter_table(3.0f);
    compter_table(37.0f);
    compter_table(12345.0f);
    compter_table(0.001f);
    compter_table(1.0E10f);
    compter_table(1.0E30f);
    equivalence();

    /* --- profondeur de pile ------------------------------------------------ */
    MESURER("session vide", "BO" X, "AU" X);
    MESURER("bureau ?38-24+47", "BO" X, "?38-24+47" X, "AU" X);
    MESURER("bureau 38 parentheses",
            "BO" X, "?((((((((((((((((((((((((((((((((((((((1))))))))))))))))))))))))))))))))))))))" X,
            "AU" X);
    MESURER("ligne 36 parentheses",
            "BO" X, "10 X_((((((((((((((((((((((((((((((((((((1))))))))))))))))))))))))))))))))))))" X,
            "AU" X);
    MESURER("ligne SI imbriques",
            "BO" X, "10 SI A=1 ALORS SI B=2 ALORS SI C=3 ALORS SI D=4 ALORS X_1 SINON X_2" X,
            "AU" X);
    MESURER("LISTER lignes variees",
            "BO" X, "10 X_((((((((((((((((((((((((((((((((((((1))))))))))))))))))))))))))))))))))))" X,
            "20 AFFICHER['ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789',F12.3,2X,E12.4]1.5,2.75" X,
            "30 SI A=1 ALORS SI B=2 ALORS SI C=3 ALORS SI D=4 ALORS X_1.25 SINON X_2" X,
            "40 X_A+B*(C-D/(E+F^(G-H*(I+J/(K-L*(M+N/(O-P^(Q+R))))))))" X,
            "LI" X, "1" X, "AU" X);
    MESURER("EXECUTER AFFICHER F et U",
            "BO" X, "10 A_&F(2)" X, "20 TERMINER" X, "30 PROCEDURE &F(B)" X,
            "40 AFFICHER[F12.3,2X,E12.4,2X,U]B*1.5,B/3,B" X, "50 RESULTAT B" X,
            "EX" X, "10" X, "AU" X);
    MESURER("EXECUTER chaine CCA",
            "BO" X, "10 CHAINE C;C_CCA(3.14159)!CCA(1E30)" X, "20 AFFICHER C" X, "30 TERMINER" X,
            "EX" X, "10" X, "AU" X);
    MESURER("correction vers 36 parentheses",
            "BO" X, "10 X_((((((((((((((((((((((((((((((((((((1)))))))))))))))))))))))))))))))))))))" X,
            "/))/)" X, "AU" X);
    MESURER("ENCODER ligne profonde",
            "BO" X, "1 CHAINE C;C_'10 X_';FAIRE 2 POUR I_1 JUSQUA 36" X, "2 C_C!'('" X,
            "3 C_C!'1';FAIRE 4 POUR I_1 JUSQUA 36" X, "4 C_C!')'" X, "5 C_C!EQC(0);GARER C,1,'EE'" X,
            "6 TERMINER" X, "EX" X, "1" X, "EF" X, "*" X, "EN" X, "EE" X, "LI" X, "1" X, "AU" X);
    printf("  ENCODER : [%s]\n", bouchon_affichage + (bouchon_affichage[0] ? 0 : 0));
    MESURER("TABLE et UTILISATION",
            "BO" X, "ID" X, "1" X, "", "1 A_1;GARER A,1,'TT'" X, "2 TERMINER" X, "EX" X, "1" X,
            "RA" X, "PP" X, "TA" X, "A" X, "AU" X);
    MESURER("erreur d'execution",
            "BO" X, "10 A_1/0" X, "EX" X, "10" X, "AU" X);

    /* --- balayage de l'imbrication des parenthèses ------------------------ */
    MESURER("ligne 0 parenthese", "BO" X, "10 X_1" X, "AU" X);
    MESURER("ligne 4 parentheses", "BO" X, "10 X_((((1))))" X, "AU" X);
    MESURER("ligne 8 parentheses", "BO" X, "10 X_((((((((1))))))))" X, "AU" X);
    MESURER("ligne 12 parentheses", "BO" X, "10 X_((((((((((((1))))))))))))" X, "AU" X);
    MESURER("ligne 16 parentheses", "BO" X, "10 X_((((((((((((((((1))))))))))))))))" X, "AU" X);
    MESURER("ligne 24 parentheses", "BO" X, "10 X_((((((((((((((((((((((((1))))))))))))))))))))))))" X, "AU" X);
    MESURER("bureau 0 parenthese", "BO" X, "?1" X, "AU" X);
    MESURER("bureau 8 parentheses", "BO" X, "?((((((((1))))))))" X, "AU" X);
    MESURER("bureau 16 parentheses", "BO" X, "?((((((((((((((((1))))))))))))))))" X, "AU" X);
    MESURER("lignes 20-40 sans LISTER",
            "BO" X, "20 AFFICHER['ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789',F12.3,2X,E12.4]1.5,2.75" X,
            "30 SI A=1 ALORS SI B=2 ALORS SI C=3 ALORS SI D=4 ALORS X_1.25 SINON X_2" X,
            "40 X_A+B*(C-D/(E+F^(G-H*(I+J/(K-L*(M+N/(O-P^(Q+R))))))))" X, "AU" X);
    MESURER("lignes 20-40 avec LISTER",
            "BO" X, "20 AFFICHER['ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789',F12.3,2X,E12.4]1.5,2.75" X,
            "30 SI A=1 ALORS SI B=2 ALORS SI C=3 ALORS SI D=4 ALORS X_1.25 SINON X_2" X,
            "40 X_A+B*(C-D/(E+F^(G-H*(I+J/(K-L*(M+N/(O-P^(Q+R))))))))" X, "LI" X, "1" X, "AU" X);
    decomp_isolee("30 SI A=1 ALORS SI B=2 ALORS SI C=3 ALORS SI D=4 ALORS X_1.25 SINON X_2");
    decomp_isolee("20 AFFICHER['ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789',F12.3,2X,E12.4]1.5,2.75");
    decomp_isolee("40 X_A+B*(C-D/(E+F^(G-H*(I+J/(K-L*(M+N/(O-P^(Q+R))))))))");

    VERIF_FIN();
}
