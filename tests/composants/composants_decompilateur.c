/* composants_decompilateur.c — décompilateur : vecteurs, cas ciblés,
   écriture des réels, diagnostics
   EXI: EXI-0003, EXI-0010, EXI-0015  VM: VM-2208  IV: IV-IF-0004, ALG-1001
   IMP: IMP-0183, IMP-0198 à IMP-0201, IMP-0203
   Spécification des tests : chapitre Décompilateur §4 à §7, §9 ; fiche de
   codage RCI-M §9, §11 (V1 à V16).
   Niveau : composants. Chaque texte est compilé, décompilé, comparé au
   texte canonique attendu, puis le texte restitué est recompilé : la
   séquence doit être identique à l'octet près (sauf mention). */

#include <stdlib.h>
#include <float.h>
#include "compilateur_outils.h"
#include "../../src/noyau/decompilateur.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

static char restitue[DECOMPILATEUR_LONGUEUR_MAX + 1];
static unsigned char premiere[256];
static unsigned int lg_premiere;

/* Compile texte (tables conservées), décompile, compare à attendu ; si
   recompiler, recompile la restitution et compare les séquences. */
static int verifier_restitution(const char *texte, const char *attendu, int recompiler)
{
    unsigned int lg;
    ResultatDecompilation rd;
    unsigned char *p;

    if (!compiler(texte)) {
        printf("  [%s] rejet inattendu : C %u, position %u\n", texte,
               erreurs_numero(d.nature), d.position);
        return 0;
    }
    rd = decompilateur_decompiler(&r.resultat.ligne_programme, restitue, &lg);
    p = objets_resoudre(r.resultat.ligne_programme.bloc);
    lg_premiere = r.resultat.ligne_programme.longueur_octets;
    memcpy(premiere, p, lg_premiere);
    objets_relacher(r.resultat.ligne_programme.bloc);
    objets_liberer(r.resultat.ligne_programme.bloc);
    if (rd != DECOMP_OK) {
        printf("  [%s] decompilation en echec : %u\n", texte, rd);
        return 0;
    }
    if (strcmp(restitue, attendu) != 0 || lg != strlen(attendu)) {
        printf("  [%s]\n    restitue [%s]\n    attendu  [%s]\n", texte, restitue, attendu);
        return 0;
    }
    if (!recompiler) return 1;
    if (!compiler(restitue)) {
        printf("  [%s] restitution rejetee : C %u, position %u\n", restitue,
               erreurs_numero(d.nature), d.position);
        return 0;
    }
    recuperer();
    if (lg_obtenu != lg_premiere || memcmp(obtenu, premiere, lg_premiere) != 0) {
        printf("  [%s] recompilation differente\n", restitue);
        afficher_octets("premiere", premiere, lg_premiere);
        afficher_octets("seconde ", obtenu, lg_obtenu);
        return 0;
    }
    return 1;
}

#define RESTITUE(texte, attendu)  VERIFIER(verifier_restitution((texte), (attendu), 1))
#define IDENTIQUE(texte)          VERIFIER(verifier_restitution((texte), (texte), 1))
#define NOUVEAU(texte, attendu)   do { symboles_vider(); RESTITUE((texte), (attendu)); } while (0)
#define NOUVEAU_ID(texte)         do { symboles_vider(); IDENTIQUE(texte); } while (0)

/* Décompile une séquence fabriquée ; renvoie le résultat. */
static ResultatDecompilation fabriquee(const unsigned char *o, unsigned int n)
{
    EntreeObjetCompile e;
    HandleObjet h;
    unsigned char *p;
    unsigned int lg;
    ResultatDecompilation rd;

    h = objets_creer(n);
    p = objets_resoudre(h);
    memcpy(p, o, n);
    objets_relacher(h);
    e.numero_ligne = 7;
    e.longueur_octets = n;
    e.bloc = h;
    rd = decompilateur_decompiler(&e, restitue, &lg);
    objets_liberer(h);
    if (rd != DECOMP_OK) {
        if (restitue[0] != '\0' || lg != 0) return 99;   /* texte vide exigé (IMP-0198) */
    }
    return rd;
}

#define FABRIQUEE(attendu, ...) \
    do { static const unsigned char v_[] = { __VA_ARGS__ }; \
         VERIFIER_EGAL((attendu), fabriquee(v_, sizeof v_)); } while (0)

/* Écriture d'un réel : texte attendu. */
static int reel(float v, const char *attendu)
{
    char s[16];
    decompilateur_ecrire_reel(v, s);
    if (strcmp(s, attendu) != 0) {
        printf("  reel : [%s], attendu [%s]\n", s, attendu);
        return 0;
    }
    return 1;
}

/* Aller-retour sur un échantillon : mantisses à six chiffres, exposants du
   domaine normal du float (IMP-0201). Renvoie le nombre d'écarts. */
static unsigned int echantillon_reels(void)
{
    static const int exposants[] = { -37, -20, -5, -4, -3, -2, -1, 0, 1, 4, 5, 6, 7, 20, 37, 38 };
    char source[24];
    char s[16];
    unsigned long m;
    unsigned int i;
    unsigned int ecarts = 0;
    float f;
    float g;
    double v;
    int e;

    for (i = 0; i < sizeof exposants / sizeof exposants[0]; i++) {
        e = exposants[i];
        for (m = 100000ul; m <= 999999ul; m += 1913ul) {
            sprintf(source, "%lu.%05luE%+03d", m / 100000ul, m % 100000ul, e);
            v = strtod(source, NULL);
            if (v > FLT_MAX || v < FLT_MIN) continue;
            f = (float) v;
            decompilateur_ecrire_reel(f, s);
            g = (float) strtod(s, NULL);
            if (g != f || strlen(s) > 12) {
                if (ecarts < 5) printf("  reel %s -> %s\n", source, s);
                ecarts++;
            }
        }
    }
    return ecarts;
}

int main(void)
{
    VERIF_DEBUT("composants/decompilateur");

    /* --- vecteurs de la fiche de codage §11 ------------------------------ */
    NOUVEAU_ID("5 A_B+C");                                                  /* V1 */
    NOUVEAU_ID("5 X_2^3^2");                                                /* V2 */
    NOUVEAU_ID("5 X_-A^2");                                                 /* V3 */
    NOUVEAU_ID("5 SI A=0 ALORS B_1 SINON B_2");                             /* V4 */
    NOUVEAU_ID("30 CH_SI N>1 ALORS ' LETTRES ' SINON ' LETTRE '");          /* V5 */
    NOUVEAU_ID("3 FAIRE 8 POUR I_T PAS -1 JUSQUA 1");                       /* V6 */
    NOUVEAU_ID("200 FAIRE 202 POUR I_1 JUSQUA N");                          /* V7 */
    NOUVEAU("20 FAIRE 20 POUR X_A PAS (A/X-X)/2 TANT QUE ABS(X^2-A)>1E-3",  /* V8 */
            "20 FAIRE 20 POUR X_A PAS (A/X-X)/2 TANT QUE ABS(X^2-A)>0.001");
    NOUVEAU("5 AFFICHER['RES=',3F2.0,E3.6] A,B,C,D,E",                      /* V9 */
            "5 AFFICHER['RES=',3F2.0,E3.6]A,B,C,D,E");
    NOUVEAU_ID("2 AFFICHER 'D''UN TRIANGLE : ';LIRE A,B,C");                /* V10 */
    symboles_vider();                                                       /* V11 */
    IDENTIQUE("25 PROCEDURE &FACT(N) LOCAL N");
    IDENTIQUE("27 RESULTAT SI N=0 ALORS 1 SINON N*&FACT(N-1)");
    NOUVEAU_ID("5 &TRAC(&F1,0,2,0.25)");                                    /* V12 */
    NOUVEAU_ID("5 TABLEAU MAT[10,5];MAT[I,J]_0");                           /* V13 */
    NOUVEAU("1 * CALCUL DE L'AIRE", "1* CALCUL DE L'AIRE");                 /* V14 */
    NOUVEAU_ID("1*  DEUX ESPACES ET 'APOSTROPHE' ");
    NOUVEAU_ID("10 I_@50");                                                 /* V15 */
    NOUVEAU_ID("5 X_A*(-1)");                                               /* V16 */
    NOUVEAU_ID("5 X_-200");

    /* --- parenthèses et rangs (§4.4) ------------------------------------- */
    NOUVEAU_ID("5 X_(A+B)*C");
    NOUVEAU_ID("5 X_A-(B-C)");
    NOUVEAU("5 X_(A-B)-C", "5 X_A-B-C");
    NOUVEAU("5 X_((A))", "5 X_A");
    NOUVEAU_ID("5 X_A^(B^C)");
    NOUVEAU("5 X_(2^3)^2", "5 X_2^3^2");
    NOUVEAU_ID("5 X_-(A+B)");
    NOUVEAU_ID("5 X_(-A)*B");
    NOUVEAU_ID("5 X_-A*B");
    NOUVEAU_ID("5 X_A+(-B)");
    NOUVEAU_ID("5 X_(-2)^2");
    NOUVEAU_ID("5 X_-(-A)");
    NOUVEAU_ID("5 X_A/(B*C)");
    NOUVEAU("5 X_+A", "5 X_A");
    NOUVEAU_ID("5 C_A!B!'X'");
    NOUVEAU_ID("5 C_A!(SI X=1 ALORS 'A' SINON 'B')");
    NOUVEAU_ID("5 X_RAC(A+B)*ENT(-C)");
    NOUVEAU_ID("5 SI (A=1 OU B=2) ET C=3 ALORS X_1");
    NOUVEAU_ID("5 SI A=1 OU B=2 ET C=3 ALORS X_1");
    NOUVEAU_ID("5 SI A=1 OU (B=2 OU C=3) ALORS X_1");
    NOUVEAU("5 SI (A+B)>C ALORS X_1", "5 SI A+B>C ALORS X_1");
    NOUVEAU_ID("5 SI A<=B ET C#D ALORS X_1 SINON X_2");

    /* --- espaces et nombres -------------------------------------------- */
    NOUVEAU("5   X _ 1 ;;  Y_2;", "5 X_1;Y_2");
    NOUVEAU("5 X_0.50", "5 X_0.5");
    NOUVEAU("5 X_1E5", "5 X_100000");
    NOUVEAU("5 X_1E-4", "5 X_1E-04");
    NOUVEAU("5 X_2.0", "5 X_2");
    NOUVEAU("5 X_123456", "5 X_123456");
    symboles_vider();
    VERIFIER(verifier_restitution("5 X_123456789", "5 X_1.23457E+08", 0));
    VERIFIER(verifier_restitution("5 X_3.1415927", "5 X_3.14159", 0));   /* §7 */

    /* --- SI et blocs DEBUT...FIN (§5) ------------------------------------ */
    NOUVEAU_ID("5 SI A=1 ALORS X_1;Y_2");
    NOUVEAU_ID("5 SI A=1 ALORS DEBUT X_1;Y_2 FIN");
    NOUVEAU("5 SI A=1 ALORS DEBUT X_1 FIN", "5 SI A=1 ALORS X_1");
    NOUVEAU_ID("5 SI A=1 ALORS DEBUT SI B=1 ALORS X_1 FIN SINON X_2");
    NOUVEAU_ID("5 SI A=1 ALORS SI B=1 ALORS X_1 SINON X_2");
    NOUVEAU_ID("5 SI A=1 ALORS DEBUT SI B=1 ALORS X_1 SINON SI C=1 ALORS X_3 FIN SINON X_2");
    NOUVEAU("5 SI A=1 ALORS SI B=1 ALORS X_1 SINON DEBUT SI C=1 ALORS X_3 FIN SINON X_2",
            "5 SI A=1 ALORS DEBUT SI B=1 ALORS X_1 SINON SI C=1 ALORS X_3 FIN SINON X_2");
    NOUVEAU("5 SI A=1 ALORS SINON X_2", "5 SI A=1 ALORS DEBUT FIN SINON X_2");
    NOUVEAU("5 SI A=1 ALORS", "5 SI A=1 ALORS DEBUT FIN");
    NOUVEAU("5 SI A=1 ALORS X_1 SINON", "5 SI A=1 ALORS X_1 SINON DEBUT FIN");
    NOUVEAU_ID("5 SI A=1 ALORS X_1 SINON DEBUT X_2;Y_3 FIN;Z_4");
    NOUVEAU_ID("5 X_SI A=1 ALORS SI B=1 ALORS 1 SINON 2 SINON 3");
    NOUVEAU_ID("5 SI A=1 ALORS AFFICHER A SINON LIRE B");
    NOUVEAU_ID("5 SI A=1 ALORS AFFICHER SINON AFFICHER[/]");
    NOUVEAU_ID("5 SI A=1 ALORS *COMMENTAIRE");
    NOUVEAU_ID("5 ALLER EN SI A=1 ALORS 10 SINON 20");

    /* --- instructions ------------------------------------------------------ */
    NOUVEAU("5 TABLEAU A[2];TABLEAU B[3,4]", "5 TABLEAU A[2],B[3,4]");
    NOUVEAU_ID("5 CHAINE A,B;LIBERER C");
    NOUVEAU_ID("5 AFFICHER");
    NOUVEAU_ID("5 AFFICHER[/]");
    NOUVEAU_ID("5 AFFICHER[*'X',2/,3X,C,L,U]A,B");
    NOUVEAU_ID("5 AFFICHER A;AFFICHER B,'X'");
    NOUVEAU_ID("5 LIRE A;LIRE B[1],C");
    NOUVEAU_ID("5 FAIRE 9 POUR I_1 TANT QUE I<10");
    NOUVEAU_ID("5 FAIRE 9 POUR I_1 PAS 2 TANT QUE I<10 ET J=0");
    NOUVEAU_ID("5 ALLER EN N+1");
    NOUVEAU_ID("5 RETOUR EN 10");
    NOUVEAU_ID("5 RETOUR EN N");
    NOUVEAU_ID("5 RETOUR;TERMINER;PAUSE");
    NOUVEAU_ID("5 X_SKP(C,1,'A')+LGR(C)+POS(C,1,'X')+CNB(C,1,K)");
    NOUVEAU_ID("5 C_SCH(C,1,' ',K)!GRL(C,2)!CCA(65)!EQC(66)");
    NOUVEAU_ID("5 X_TEM()+ALE(0)");
    NOUVEAU_ID("5 C_DAT()");
    NOUVEAU_ID("5 GARER A,1,'F'");
    NOUVEAU_ID("5 CHARGER A,1,'F',E");
    NOUVEAU_ID("5 SUPPRIMER 'F',1");
    NOUVEAU_ID("5 EXECUTER 'P',10");
    NOUVEAU_ID("5 PROCEDURE &P(A,&F) LOCAL A,B;X_&F(A)+&Q()");
    NOUVEAU_ID("5 &P()");
    NOUVEAU_ID("5 X_1;*FIN");
    NOUVEAU_ID("5*X");
    NOUVEAU("5   *X", "5*X");
    NOUVEAU_ID("5 SI A=1 ALORS * X");
    NOUVEAU_ID("60 ");
    NOUVEAU_ID("250 C_'IL FAUT L''AIDE DE TOUS'");
    NOUVEAU_ID("5 C_''");
    {
        /* caractère non imprimable sous MITRA 15 : restitué tel quel */
        static const char avec_bip[] = { '5', ' ', 'C', '_', '\'', 'A', 7, 'B', '\'', 0 };
        symboles_vider();
        IDENTIQUE(avec_bip);
    }

    /* --- profil T1600 -------------------------------------------------- */
    configuration_session.profil = PROFIL_T1600_F4;                         /* IMP-0276 */
    NOUVEAU_ID("10 ALLER EN @+3");
    NOUVEAU_ID("104 Q_TEM()");                                              /* SINUS, IMP-0189 */
    NOUVEAU_ID("5 X_TEM()+SGN(A)+SH(1)");
    NOUVEAU("5 X_TEM", "5 X_TEM()");
    NOUVEAU_ID("5 C_DAT()");
    NOUVEAU("5 C_DAT", "5 C_DAT()");
    NOUVEAU_ID("5 C_'A'.8.'B'");
    NOUVEAU_ID("5 C_.27 12.");
    NOUVEAU("5 C_.65.", "5 C_'A'");
    /* libellé de format : mêmes règles qu'une constante (IMP-0190 ; MULT 50) */
    NOUVEAU_ID("50 AFFICHER[/,' A'.7.'U'.7.' '.7.'R'.7.'E'.7.'V'.7.'O'.7.'I'.7.'R'.7 7 7.]");
    NOUVEAU_ID("5 AFFICHER[.27 12.,3.7.,2'A'.8.'B',*.7 7.]N");
    NOUVEAU("5 AFFICHER['A'.65.]", "5 AFFICHER['AA']");
    /* FAIRE sans variable (IMP-0277) */
    NOUVEAU_ID("105 FAIRE 105 TANT QUE A<4");
    NOUVEAU_ID("5 FAIRE 8 TANT QUE A<4 ET B=1;X_1");
    NOUVEAU_ID("5 X_1;FAIRE 8 TANT QUE A#1");
    NOUVEAU_ID("5 SI A=0 ALORS DEBUT FAIRE 5 TANT QUE B<1;B_B+1 FIN");
    NOUVEAU_ID("105 FAIRE 105 TANT QUE TEM()-Q<4");
    NOUVEAU_ID("5 AFFICHER['L''A','']");
    {
        /* caractère non imprimable dans un libellé : restitué en .n. */
        static const char avec_bip[] = { '5', ' ', 'A', 'F', 'F', 'I', 'C', 'H', 'E', 'R',
                                         '[', '\'', 'A', 7, 'B', '\'', ']', 0 };
        symboles_vider();
        NOUVEAU(avec_bip, "5 AFFICHER['A'.7.'B']");
    }
    configuration_session.profil = PROFIL_T1600_V4;
    NOUVEAU_ID("10 ALLER EN @+3");                                          /* V15 */
    NOUVEAU_ID("5 X_TEM+SGN(A)+SH(1)");
    NOUVEAU_ID("5 C_DAT");
    NOUVEAU_ID("5 UN X;ZER Y");
    NOUVEAU_ID("255 RETOUR EN $");
    {
        /* .n. hors profil : caractère non imprimable restitué tel quel */
        static const char avec_bip[] = { '5', ' ', 'C', '_', '\'', 'A', 7, 'B', '\'', 0 };
        static const char libelle_bip[] = { '5', ' ', 'A', 'F', 'F', 'I', 'C', 'H', 'E', 'R',
                                            '[', '\'', 'A', 7, 'B', '\'', ']', 0 };
        symboles_vider();
        IDENTIQUE(avec_bip);
        IDENTIQUE(libelle_bip);
    }
    configuration_session.profil = PROFIL_MITRA15_V8;

    /* --- réels (IMP-0201) ---------------------------------------------- */
    VERIFIER(reel(0.0f, "0"));
    VERIFIER(reel(0.055f, "0.055"));
    VERIFIER(reel(1E-4f, "1E-04"));
    VERIFIER(reel(0.001f, "0.001"));
    VERIFIER(reel(999999.0f, "999999"));
    VERIFIER(reel(1E6f, "1E+06"));
    VERIFIER(reel(3.01E21f, "3.01E+21"));
    VERIFIER(reel(-2.5f, "-2.5"));
    VERIFIER(reel(0.4f, "0.4"));
    VERIFIER(reel(FLT_MAX, "3.40282E+38"));
    VERIFIER(reel(1234565.0f, "1.23457E+06"));
    VERIFIER_EGAL(0, echantillon_reels());

    /* --- diagnostics (IMP-0200) ---------------------------------------- */
    symboles_vider();
    VERIFIER(accepte("5 A_1"));                  /* symbole 0 : A */
    FABRIQUEE(DECOMP_OK, 0x40, 0x00, 0x00, 0x24, 0x99);
    VERIFIER(strcmp(restitue, "7 A_1") == 0);
    FABRIQUEE(DECOMP_ERR_SEQUENCE, 0x80);                        /* pile vide */
    FABRIQUEE(DECOMP_ERR_SEQUENCE, 0x04);                        /* jamais produit */
    FABRIQUEE(DECOMP_ERR_SEQUENCE, 0x01, 0x00);                  /* élément coupé */
    FABRIQUEE(DECOMP_ERR_SEQUENCE, 0x40, 0x00, 0x00);            /* valeur laissée */
    FABRIQUEE(DECOMP_ERR_SEQUENCE, 0x24, 0x24, 0x90, 0xC0, 0x09); /* cible hors ligne */
    FABRIQUEE(DECOMP_ERR_SEQUENCE, 0x24, 0x24, 0x99);            /* cible non variable */
    FABRIQUEE(DECOMP_ERR_SEQUENCE, 0xC7, 0xCD, 0x00, 0x00, 0x00, 0x00); /* PROCEDURE */
    FABRIQUEE(DECOMP_ERR_SEQUENCE, 0x24, 0xC0, 0x03, 0xC7);      /* condition absente */
    FABRIQUEE(DECOMP_ERR_REFERENCE, 0x40, 0x09, 0x00, 0x24, 0x99);
    FABRIQUEE(DECOMP_ERR_REFERENCE, 0xB8, 0x03, 0x80, 0x05, 0x00, 0xB6);
    {
        /* texte de plus de 255 caractères : quatre chaînes partagées de 100 */
        unsigned char t[100];
        unsigned int index;
        unsigned char o[1 + 4 * 5];
        unsigned int i;
        memset(t, 'Z', sizeof t);
        VERIFIER_EGAL(SYMB_OK, constantes_chercher_ou_creer(t, 100, &index));
        o[0] = 0xB8;
        for (i = 0; i < 4; i++) {
            o[1 + 5 * i] = 0x03;
            o[2 + 5 * i] = 0x80;
            o[3 + 5 * i] = (unsigned char) (index & 0xFF);
            o[4 + 5 * i] = (unsigned char) (index >> 8);
            o[5 + 5 * i] = 0xB6;
        }
        VERIFIER_EGAL(DECOMP_ERR_TROP_LONGUE, fabriquee(o, sizeof o));
        VERIFIER_EGAL(DECOMP_OK, fabriquee(o, 1 + 2 * 5));
        VERIFIER_EGAL(216, strlen(restitue));
    }

    /* Table des puissances de dix de l'écriture des réels (audit du code,
       AP-03) : les mêmes littéraux, convertis par le même compilateur, valent
       exactement les produits successifs par 10 que le calcul d'avant formait. */
    {
        static const double t[23] = {
            1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11,
            1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22
        };
        double p = 1.0;
        int exactes = 1;
        int k;
        for (k = 0; k < 23; k++) {
            if (memcmp(&t[k], &p, sizeof p) != 0) exactes = 0;
            p = p * 10.0;
        }
        VERIFIER(exactes);
    }

    VERIF_FIN();
}
