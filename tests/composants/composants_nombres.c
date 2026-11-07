/* composants_nombres.c — chemin exact des nombres, sans SANE (IMP-0244) ;
   calcul en format étendu (IMP-0216)
   IMP: IMP-0216 (calcul en double, référence), IMP-0244
   Spécification des tests : chapitre Interpréteur, IMP-0244 : le chemin
   entier rend les mêmes bits que le calcul en double rangé en float ; les
   comparaisons par les bits rendent l'ordre des valeurs. IMP-0216 : le calcul
   en format étendu de + - * / rend les mêmes bits et les mêmes erreurs que le
   calcul en double (audit du code, AP-04).
   Niveau : composants. */

#include <string.h>
#include <float.h>
#include "../commun/verif.h"
#include "../../src/interfaces/types_communs.h"
#include "../../src/noyau/donnees.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

typedef union {
    float         f;
    unsigned long u;
} Bits;

static unsigned long bits(float f)
{
    Bits x;
    x.f = f;
    return x.u;
}

/* Référence d'IMP-0216 : calcul en double, rangé en float. */
static float reference(char op, float a, float b)
{
    double r;
    if (op == '+') r = (double) a + (double) b;
    else if (op == '-') r = (double) a - (double) b;
    else r = (double) a * (double) b;
    return (float) r;
}

static float de_bits(unsigned long u)
{
    Bits x;
    x.u = u;
    return x.f;
}

static unsigned long ecarts = 0;
static unsigned long rapides = 0;
static unsigned long ecarts_etendus = 0;
static unsigned long essais_etendus = 0;

/* Référence d'IMP-0216 pour + - * / : division par zéro (EX 31), calcul en
   double, dépassement au-delà de FLT_MAX (EX 44), rangé en float. */
static NatureErreur reference_etendue(char op, const float *pa, const float *pb, float *r)
{
    double a = *pa;
    double b = *pb;
    double x;
    if (op == '/' && b == 0.0) return ERR_E_DIVISION_PAR_ZERO;
    if (op == '+') x = a + b;
    else if (op == '-') x = a - b;
    else if (op == '*') x = a * b;
    else x = a / b;
    if (x > FLT_MAX || x < -FLT_MAX) return ERR_E_DEPASSEMENT;
    *r = (float) x;
    return ERR_AUCUNE;
}

static void confronter_etendue(char op, const float *a, const float *b)
{
    float r1 = 0.0f;
    float r2 = 0.0f;
    NatureErreur n1 = donnees_operation_etendue(op, a, b, &r1);
    NatureErreur n2 = reference_etendue(op, a, b, &r2);
    essais_etendus++;
    if (n1 != n2 || (n1 == ERR_AUCUNE && bits(r1) != bits(r2))) {
        ecarts_etendus++;
        if (ecarts_etendus < 5)
            printf("  ecart %08lX %c %08lX : %u %08lX au lieu de %u %08lX\n", bits(*a), op, bits(*b),
                   n1, bits(r1), n2, bits(r2));
    }
}

static void confronter_quatre(const float *a, const float *b)
{
    confronter_etendue('+', a, b);
    confronter_etendue('-', a, b);
    confronter_etendue('*', a, b);
    confronter_etendue('/', a, b);
}

static void confronter(char op, float pa, float pb)
{
    float a = pa;              /* un paramètre float est au format étendu */
    float b = pb;
    float r;
    if (donnees_operation_entiere(op, &a, &b, &r)) {
        rapides++;
        if (bits(r) != bits(reference(op, a, b))) {
            ecarts++;
            if (ecarts < 5)
                printf("  ecart %g %c %g : %08lX au lieu de %08lX\n", (double) a, op, (double) b,
                       bits(r), bits(reference(op, a, b)));
        }
    }
}

int main(void)
{
    static const float speciaux[] = {
        0.0f, -0.0f, 1.0f, -1.0f, 0.5f, -0.5f, 1.5f, 123.25f, 4095.0f, 4096.0f, 4097.0f,
        8388608.0f, 8388607.0f, 16777215.0f, -16777215.0f, 16777216.0f, 30000000.0f,
        0.001f, 1.0e10f, -1.0e10f, 4096.5f, 65536.0f, -65537.0f, 2.0f
    };
    unsigned int ns = sizeof speciaux / sizeof speciaux[0];
    unsigned int i;
    unsigned int j;
    long a;
    long b;
    float fa;
    float fb;
    float r;
    int ordre_ok = 1;
    int conv_ok = 1;
    int opp_ok = 1;
    int c;
    int attendu;

    VERIF_DEBUT("composants/nombres");

    /* donnees_nombre_entier : mêmes bits que la conversion du compilateur */
    for (a = -70000L; a <= 70000L; a++) {
        donnees_nombre_entier(a, &r);
        if (bits(r) != bits((float) a)) conv_ok = 0;
    }
    donnees_nombre_entier(16777215L, &r);
    if (bits(r) != bits(16777215.0f)) conv_ok = 0;
    donnees_nombre_entier(-16777215L, &r);
    if (bits(r) != bits(-16777215.0f)) conv_ok = 0;
    donnees_nombre_entier(16777216L, &r);
    if (bits(r) != bits(16777216.0f)) conv_ok = 0;
    VERIFIER(conv_ok);

    /* opérations : toutes les paires d'entiers de -150 à 150 */
    for (a = -150; a <= 150; a++)
        for (b = -150; b <= 150; b++) {
            fa = (float) a;
            fb = (float) b;
            confronter('+', fa, fb);
            confronter('-', fa, fb);
            confronter('*', fa, fb);
        }
    /* valeurs aux limites, zéros signés, décimaux, grands nombres */
    for (i = 0; i < ns; i++)
        for (j = 0; j < ns; j++) {
            confronter('+', speciaux[i], speciaux[j]);
            confronter('-', speciaux[i], speciaux[j]);
            confronter('*', speciaux[i], speciaux[j]);
        }
    VERIFIER_EGAL(0, ecarts);
    VERIFIER(rapides > 270000ul);                 /* le chemin entier a bien servi */

    /* le chemin entier se récuse hors de son domaine */
    fa = 0.5f; fb = 1.0f;
    VERIFIER(!donnees_operation_entiere('+', &fa, &fb, &r));
    fa = 16777216.0f;
    VERIFIER(!donnees_operation_entiere('+', &fa, &fb, &r));
    fa = 16777215.0f;
    VERIFIER(!donnees_operation_entiere('+', &fa, &fb, &r));   /* résultat 2^24 */
    fa = 4097.0f; fb = 4097.0f;
    VERIFIER(!donnees_operation_entiere('*', &fa, &fb, &r));   /* 16785409 > 2^24 */

    /* comparaisons : l'ordre des valeurs, +0 et -0 égaux */
    for (i = 0; i < ns; i++)
        for (j = 0; j < ns; j++) {
            c = donnees_comparer_nombres(&speciaux[i], &speciaux[j]);
            attendu = speciaux[i] < speciaux[j] ? -1 : speciaux[i] > speciaux[j] ? 1 : 0;
            if ((c < 0 ? -1 : c > 0 ? 1 : 0) != attendu) ordre_ok = 0;
        }
    VERIFIER(ordre_ok);

    /* changement de signe */
    for (i = 0; i < ns; i++) {
        donnees_opposer(&speciaux[i], &r);
        if (bits(r) != bits(-speciaux[i])) opp_ok = 0;
    }
    VERIFIER(opp_ok);

    /* format étendu (IMP-0216 ; audit du code, AP-04) : mêmes bits et mêmes
       erreurs que le calcul en double ; valeurs construites par leurs bits */
    {
        static const unsigned long motifs[] = {
            0x00000000ul, 0x80000000ul, 0x3F800000ul, 0xBF800000ul,   /* 0, -0, 1, -1 */
            0x3DCCCCCDul, 0xBDCCCCCDul, 0x3E4CCCCDul, 0x3E99999Aul,   /* 0,1 -0,1 0,2 0,3 */
            0x3EAAAAABul, 0x3F2AAAABul, 0x40490FDBul, 0x402DF854ul,   /* 1/3 2/3 pi e */
            0x40E00000ul, 0x41200000ul, 0x3727C5ACul, 0x42F6E979ul,   /* 7 10 1e-5 123,456 */
            0x501502F9ul, 0xD01502F9ul, 0x7149F2CAul, 0x7E967699ul,   /* 1e10 -1e10 1e30 1e38 */
            0x7F7FFFFFul, 0xFF7FFFFFul, 0x7F7FFFFEul, 0x7EFFFFFFul,   /* +-FLT_MAX, voisins */
            0x00800000ul, 0x80800000ul, 0x00000001ul, 0x00000003ul,   /* +-FLT_MIN, sous-normaux */
            0x007FFFFFul, 0x0000B6DBul, 0x3F000000ul, 0x3FC00000ul,   /* 0,5 1,5 */
            0x3F800001ul, 0x3F7FFFFFul, 0x3F400000ul, 0x64800000ul,   /* 1+u 1-u 0,75 2^74 */
            0x64800001ul, 0x73000000ul, 0x73800000ul, 0x4B800001ul,   /* 2^74+2^51 2^103 2^104 */
            0x67800000ul                                              /* 2^80 */
        };
        unsigned int nm = sizeof motifs / sizeof motifs[0];
        unsigned long alea = 12345ul;
        unsigned long u;
        unsigned int k;
        float x;
        float y;

        for (i = 0; i < nm; i++)
            for (j = 0; j < nm; j++) {
                x = de_bits(motifs[i]);
                y = de_bits(motifs[j]);
                confronter_quatre(&x, &y);
            }
        /* 20 000 paires tirées au hasard, finies ; une sur deux d'exposants
           voisins, où les arrondis sont les plus fins */
        for (k = 0; k < 20000u; k++) {
            alea = alea * 1664525ul + 1013904223ul;
            u = alea;
            if ((u & 0x7F800000ul) == 0x7F800000ul) u ^= 0x40000000ul;
            x = de_bits(u);
            alea = alea * 1664525ul + 1013904223ul;
            u = alea;
            if (k & 1u) u = (u & 0x807FFFFFul) | (bits(x) & 0x7F800000ul);
            if ((u & 0x7F800000ul) == 0x7F800000ul) u ^= 0x40000000ul;
            y = de_bits(u);
            confronter_quatre(&x, &y);
        }
        VERIFIER_EGAL(0, ecarts_etendus);
        VERIFIER(essais_etendus == 4ul * (nm * nm + 20000ul));

        /* bande au-dessus de FLT_MAX, cas valables sur les deux machines : le
           float obtenu y vaut FLT_MAX et le calcul en double décide. FLT_MAX + 2^74 :
           à égalité, arrondi pair à FLT_MAX ; FLT_MAX + 2^80 : EX 44. (Avec
           2^74 + 2^51, le résultat dépend de la machine : le calcul en double
           passe par le format étendu de SANE, qui n'a que 53 bits sous Golden
           Gate ; la confrontation ci-dessus le couvre.) */
        x = de_bits(0x7F7FFFFFul);
        y = de_bits(0x64800000ul);
        VERIFIER(donnees_operation_etendue('+', &x, &y, &r) == ERR_AUCUNE && bits(r) == 0x7F7FFFFFul);
        y = de_bits(0x67800000ul);
        VERIFIER_EGAL(ERR_E_DEPASSEMENT, donnees_operation_etendue('+', &x, &y, &r));
        y = de_bits(0x3F800001ul);
        VERIFIER_EGAL(ERR_E_DEPASSEMENT, donnees_operation_etendue('*', &x, &y, &r));
        x = de_bits(0xFF7FFFFFul);
        y = de_bits(0x67800000ul);
        VERIFIER_EGAL(ERR_E_DEPASSEMENT, donnees_operation_etendue('-', &x, &y, &r));
        /* division par zéro, signée ou non ; 0/0 aussi */
        x = 1.0f;
        y = de_bits(0x80000000ul);
        VERIFIER_EGAL(ERR_E_DIVISION_PAR_ZERO, donnees_operation_etendue('/', &x, &y, &r));
        x = 0.0f;
        y = 0.0f;
        VERIFIER_EGAL(ERR_E_DIVISION_PAR_ZERO, donnees_operation_etendue('/', &x, &y, &r));
        /* 1/3 et 0,1 + 0,2, valeurs attendues */
        x = 1.0f;
        y = 3.0f;
        VERIFIER(donnees_operation_etendue('/', &x, &y, &r) == ERR_AUCUNE && bits(r) == 0x3EAAAAABul);
        x = de_bits(0x3DCCCCCDul);
        y = de_bits(0x3E4CCCCDul);
        VERIFIER(donnees_operation_etendue('+', &x, &y, &r) == ERR_AUCUNE && bits(r) == 0x3E99999Aul);
        /* plus petit sous-normal divisé par 2 : à égalité, arrondi pair à 0 */
        x = de_bits(0x00000001ul);
        y = 2.0f;
        VERIFIER(donnees_operation_etendue('/', &x, &y, &r) == ERR_AUCUNE && bits(r) == 0);
    }

    VERIF_FIN();
}
