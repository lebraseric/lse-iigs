/* composants_donnees.c — chaînes, fonctions sur les chaînes, écriture des
   nombres, tableaux
   EXI: EXI-0039  VM: VM-1102, VM-1201, VM-1202
   IMP: IMP-0209 à IMP-0213
   Spécification des tests : chapitre Donnees §8 ; exemples de LSE15 §4-4.2,
   §5-2, §6-2.3, §6-2.4, §6-2.6.
   Niveau : composants. */

#include <string.h>
#include <float.h>
#include "../commun/verif.h"
#include "../../src/interfaces/types_communs.h"
#include "../../src/noyau/donnees.h"
#include "../../src/noyau/objets.h"
#include "../../src/noyau/rcim.h"

segment "suite";

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

/* LSE15 écrit l'espace « ∧ » ; les textes des tests l'écrivent ' '. */
static ChaineSurPile ch(const char *t)
{
    ChaineSurPile c;
    donnees_chaine_creer((const unsigned char *) t, (unsigned int) strlen(t), &c);
    return c;
}

static int egal(const ChaineSurPile *c, const char *attendu)
{
    unsigned char t[300];
    unsigned int n = donnees_texte(c, 0, t, sizeof t);
    int ok = n == strlen(attendu) && memcmp(t, attendu, n) == 0;
    if (!ok) { t[n] = 0; printf("  [%s], attendu [%s]\n", t, attendu); }
    objets_liberer(c->objet);
    return ok;
}

static int nombre(float v, unsigned char type, unsigned char e, unsigned char d, const char *attendu)
{
    char s[300];
    donnees_ecrire_nombre(v, type, e, d, s);
    if (strcmp(s, attendu) != 0) { printf("  nombre [%s], attendu [%s]\n", s, attendu); return 0; }
    return 1;
}

int main(void)
{
    ChaineSurPile a, b, r, g;
    HandleObjet h, h2;
    float v, d;
    float attendu;
    unsigned int rang;
    unsigned char dims;
    unsigned int d1, d2;
    char long_texte[300];

    VERIF_DEBUT("composants/donnees");

    /* --- chaînes : création, concaténation, comparaison (LSE15 §4-4.1, §5-2) --- */
    a = ch("AB"); b = ch("CDE");
    VERIFIER_EGAL(ERR_AUCUNE, donnees_concatener(&a, &b, &r));
    VERIFIER(egal(&r, "ABCDE"));
    objets_liberer(a.objet); objets_liberer(b.objet);
    a = ch("ABC"); b = ch("C");   VERIFIER(donnees_comparer(&a, &b) < 0); objets_liberer(a.objet); objets_liberer(b.objet);
    a = ch("AB."); b = ch("AB "); VERIFIER(donnees_comparer(&a, &b) > 0); objets_liberer(a.objet); objets_liberer(b.objet);
    a = ch("ABC"); b = ch("ABC"); VERIFIER_EGAL(0, donnees_comparer(&a, &b)); objets_liberer(a.objet); objets_liberer(b.objet);
    a = ch("ABC "); b = ch("ABC"); VERIFIER(donnees_comparer(&a, &b) > 0); objets_liberer(a.objet); objets_liberer(b.objet);
    a = ch(""); b = ch("");       VERIFIER_EGAL(0, donnees_comparer(&a, &b)); objets_liberer(a.objet); objets_liberer(b.objet);
    /* borne de longueur, la même sous les deux profils (IMP-0209) : FICH2
       enr. 8 fait 274 caractères sous LSE F4 */
    memset(long_texte, 'X', sizeof long_texte);
    configuration_session.profil = PROFIL_T1600_V4;
    VERIFIER_EGAL(ERR_AUCUNE, donnees_chaine_creer((unsigned char *) long_texte, 274, &r));
    for (rang = 0; rang < 7; rang++) {            /* 274 × 128 = 35 072 */
        VERIFIER_EGAL(ERR_AUCUNE, donnees_concatener(&r, &r, &a));
        objets_liberer(r.objet);
        r = a;
    }
    VERIFIER_EGAL(35072u, r.longueur);
    VERIFIER_EGAL(ERR_E_CHAINE_TROP_LONGUE, donnees_concatener(&r, &r, &a));   /* 70 144 */
    objets_liberer(r.objet);
    configuration_session.profil = PROFIL_MITRA15_V8;
    VERIFIER_EGAL(ERR_AUCUNE, donnees_chaine_creer((unsigned char *) long_texte, 300, &r));
    VERIFIER_EGAL(300, r.longueur);
    /* objet propre, désignation, copie (IMP-0208) */
    VERIFIER(donnees_chaine_vers_objet(&r, &h) == ERR_AUCUNE && h == r.objet);   /* reprise */
    VERIFIER(donnees_objet_vers_chaine(h, &a) == ERR_AUCUNE && a.longueur == 300 && !a.transitoire);
    VERIFIER(donnees_copier(h, &h2) == ERR_AUCUNE && h2 != h);
    VERIFIER(donnees_objet_vers_chaine(h2, &b) == ERR_AUCUNE && donnees_comparer(&a, &b) == 0);
    objets_liberer(h); objets_liberer(h2);
    a = ch("OUI"); a.transitoire = 0;
    VERIFIER(donnees_chaine_vers_objet(&a, &h) == ERR_AUCUNE && h != a.objet);    /* copie */
    objets_liberer(h); objets_liberer(a.objet);

    /* --- SCH (LSE15 §4-4.2) ------------------------------------------------- */
    a = ch("ABCD");
    VERIFIER(donnees_sch(&a, 2, NULL, 2, &r, &d) == ERR_AUCUNE && egal(&r, "BC") && d == 4.0f);
    VERIFIER(donnees_sch(&a, 2, NULL, 0, &r, &d) == ERR_AUCUNE && egal(&r, "") && d == 2.0f);
    VERIFIER(donnees_sch(&a, 3, NULL, 4, &r, &d) == ERR_AUCUNE && egal(&r, "CD") && d == 5.0f);
    VERIFIER_EGAL(ERR_E_LONGUEUR_NEGATIVE, donnees_sch(&a, 2, NULL, -1, &r, &d));
    VERIFIER_EGAL(ERR_E_POSITION_AU_DELA, donnees_sch(&a, 6, NULL, 1, &r, &d));      /* RX6 */
    VERIFIER_EGAL(ERR_E_POSITION_ERRONEE, donnees_sch(&a, 0, NULL, 1, &r, &d));
    VERIFIER(donnees_sch(&a, 5, NULL, 1, &r, &d) == ERR_AUCUNE && egal(&r, "") && d == 5.0f);
    objets_liberer(a.objet);
    a = ch(" ABC "); g = ch(" ");
    VERIFIER(donnees_sch(&a, 2, &g, 0, &r, &d) == ERR_AUCUNE && egal(&r, "ABC") && d == 5.0f);
    VERIFIER(donnees_sch(&a, 1, &g, 0, &r, &d) == ERR_AUCUNE && egal(&r, "") && d == 1.0f);
    objets_liberer(a.objet); objets_liberer(g.objet);
    a = ch(" ABC.D"); g = ch(";.");
    VERIFIER(donnees_sch(&a, 1, &g, 0, &r, &d) == ERR_AUCUNE && egal(&r, " ABC") && d == 5.0f);
    objets_liberer(g.objet); g = ch("Q");
    VERIFIER(donnees_sch(&a, 2, &g, 0, &r, &d) == ERR_AUCUNE && egal(&r, "ABC.D") && d == 7.0f);
    objets_liberer(a.objet); objets_liberer(g.objet);

    /* --- GRL ------------------------------------------------------------------ */
    a = ch(" JE  SUIS,DONC");
    VERIFIER(donnees_grl(&a, 1, &r, &d) == ERR_AUCUNE && egal(&r, "JE") && d == 4.0f);
    VERIFIER(donnees_grl(&a, 4, &r, &d) == ERR_AUCUNE && egal(&r, "SUIS") && d == 10.0f);
    VERIFIER(donnees_grl(&a, 10, &r, &d) == ERR_AUCUNE && egal(&r, "DONC") && d == 15.0f);
    VERIFIER(donnees_grl(&a, 15, &r, &d) == ERR_AUCUNE && egal(&r, "") && d == 15.0f);   /* IMP-0212 */
    /* PTR : position de fin de SCH ou de GRL */
    VERIFIER(donnees_ptr(&a, 4, 0, NULL, 0, &v) == ERR_AUCUNE && v == 10.0f);
    VERIFIER(donnees_ptr(&a, 2, 1, NULL, 2, &v) == ERR_AUCUNE && v == 4.0f);
    objets_liberer(a.objet);

    /* --- CCA, EQC, LGR -------------------------------------------------------- */
    VERIFIER(donnees_cca(3.125f, &r) == ERR_AUCUNE && egal(&r, "3.125"));
    VERIFIER(donnees_cca(-3.0f, &r) == ERR_AUCUNE && egal(&r, "-3"));
    VERIFIER(donnees_cca(10500000.0f, &r) == ERR_AUCUNE && egal(&r, "1.05E+07"));      /* IMP-0211 */
    VERIFIER(donnees_cca(2.0f / 3.0f, &r) == ERR_AUCUNE && egal(&r, "0.6666667"));   /* G4 */
    VERIFIER(donnees_eqc(65, &r) == ERR_AUCUNE && egal(&r, "A"));
    VERIFIER_EGAL(ERR_AUCUNE, donnees_eqc(200, &r)); objets_liberer(r.objet);
    configuration_session.profil = PROFIL_T1600_V4;
    VERIFIER_EGAL(ERR_E_EQC_HORS_BORNES, donnees_eqc(130, &r));                       /* INRDP75 l. 1426 */
    configuration_session.profil = PROFIL_T1600_F4;                                   /* IMP-0276 */
    VERIFIER_EGAL(ERR_E_EQC_HORS_BORNES, donnees_eqc(130, &r));
    configuration_session.profil = PROFIL_MITRA15_V8;
    VERIFIER_EGAL(ERR_E_EQC_HORS_BORNES, donnees_eqc(-1, &r));
    a = ch("LA"); b = ch(" CHAINE");
    donnees_concatener(&a, &b, &r);
    VERIFIER_EGAL(9, r.longueur);
    objets_liberer(r.objet); objets_liberer(a.objet); objets_liberer(b.objet);

    /* --- POS ----------------------------------------------------------------- */
    a = ch("CABCABD"); g = ch("AB");
    VERIFIER(donnees_pos(&a, 1, &g, &v) == ERR_AUCUNE && v == 2.0f);
    VERIFIER(donnees_pos(&a, 3, &g, &v) == ERR_AUCUNE && v == 5.0f);
    objets_liberer(g.objet); g = ch("ABC");
    VERIFIER(donnees_pos(&a, 3, &g, &v) == ERR_AUCUNE && v == 0.0f);
    objets_liberer(g.objet); g = ch("");
    VERIFIER(donnees_pos(&a, 4, &g, &v) == ERR_AUCUNE && v == 4.0f);                 /* Cγ vide */
    objets_liberer(a.objet); objets_liberer(g.objet);

    /* --- CNB ----------------------------------------------------------------- */
    a = ch("23 OU 6");
    VERIFIER(donnees_cnb(&a, 1, &v, &d) == ERR_AUCUNE && v == 23.0f && d == 3.0f);
    VERIFIER(donnees_cnb(&a, 6, &v, &d) == ERR_AUCUNE && v == 6.0f && d == 8.0f);
    objets_liberer(a.objet);
    a = ch("   -33.14E+05 A");
    VERIFIER(donnees_cnb(&a, 1, &v, &d) == ERR_AUCUNE && v == -3314000.0f && d == 14.0f);
    VERIFIER(donnees_cnb(&a, 7, &v, &d) == ERR_AUCUNE && v == 0.0f && d == 7.0f);
    objets_liberer(a.objet);
    a = ch("34E 3");
    VERIFIER(donnees_cnb(&a, 2, &v, &d) == ERR_AUCUNE && v == 4.0f && d == 3.0f);
    objets_liberer(a.objet);
    a = ch("1E99");
    VERIFIER(donnees_cnb(&a, 1, &v, NULL) == ERR_AUCUNE && v == FLT_MAX);            /* LSE15 */
    objets_liberer(a.objet);

    /* --- EQN, SKP ------------------------------------------------------------ */
    a = ch("A");
    VERIFIER(donnees_eqn(&a, 1, &v) == ERR_AUCUNE && v == 65.0f);
    VERIFIER(donnees_eqn(&a, 2, &v) == ERR_AUCUNE && v == -1.0f);
    VERIFIER_EGAL(ERR_E_POSITION_EQN, donnees_eqn(&a, 3, &v));
    objets_liberer(a.objet);
    a = ch("+- A");
    VERIFIER(donnees_eqn(&a, 3, &v) == ERR_AUCUNE && v == 32.0f);
    objets_liberer(a.objet);
    a = ch("   3 "); g = ch(" ");
    VERIFIER(donnees_skp(&a, 1, &g, &v) == ERR_AUCUNE && v == 4.0f);
    objets_liberer(a.objet); objets_liberer(g.objet);
    a = ch(" . ;H"); g = ch(" ;.");
    VERIFIER(donnees_skp(&a, 1, &g, &v) == ERR_AUCUNE && v == 5.0f);
    objets_liberer(a.objet);
    a = ch("ABC");
    VERIFIER(donnees_skp(&a, 1, &g, &v) == ERR_AUCUNE && v == 1.0f);
    objets_liberer(a.objet); objets_liberer(g.objet);
    a = ch("423 AB");
    VERIFIER(donnees_skp(&a, 1, NULL, &v) == ERR_AUCUNE && v == 5.0f);
    objets_liberer(a.objet);

    /* --- écriture des nombres : table de LSE15 §6-2.3 ------------------------ */
    VERIFIER(nombre(1692.0f, RCIM_DESC_E, 3, 4, "  1.6920E+03"));
    VERIFIER(nombre(1692.0f, RCIM_DESC_F, 2, 1, "1692.0"));
    VERIFIER(nombre(1692.0f, RCIM_DESC_U, 0, 0, "1692 "));
    VERIFIER(nombre(-34.227f, RCIM_DESC_E, 3, 4, " -3.4227E+01"));
    VERIFIER(nombre(-34.227f, RCIM_DESC_F, 2, 1, "-34.2"));
    VERIFIER(nombre(-34.227f, RCIM_DESC_U, 0, 0, "-34.227 "));
    VERIFIER(nombre(-2.86f, RCIM_DESC_E, 3, 4, " -2.8600E+00"));
    VERIFIER(nombre(-2.86f, RCIM_DESC_F, 2, 1, "-2.9"));
    VERIFIER(nombre(-2.86f, RCIM_DESC_U, 0, 0, "-2.86 "));
    VERIFIER(nombre(0.055f, RCIM_DESC_E, 3, 4, "  5.5000E-02"));
    VERIFIER(nombre(0.055f, RCIM_DESC_F, 2, 1, " 0.1"));
    VERIFIER(nombre(0.055f, RCIM_DESC_U, 0, 0, "0.055 "));
    VERIFIER(nombre(-0.0001f, RCIM_DESC_E, 3, 4, " -1.0000E-04"));
    VERIFIER(nombre(-0.0001f, RCIM_DESC_F, 2, 1, "-0.0"));
    VERIFIER(nombre(-0.0001f, RCIM_DESC_U, 0, 0, "-1E-04 "));
    {   /* au-delà de 2^53, rien que des chiffres (Données §4 ; audit du code, AC-22) */
        char s[300];
        unsigned int i;
        int propre = 1;
        donnees_ecrire_nombre(1.0e20f, RCIM_DESC_F, 30, 2, s);
        VERIFIER(strlen(s) == 33);
        for (i = 0; s[i] != '\0'; i++)
            if (!(s[i] == ' ' || s[i] == '.' || (s[i] >= '0' && s[i] <= '9'))) propre = 0;
        donnees_ecrire_nombre(3.0e38f, RCIM_DESC_F, 45, 0, s);
        for (i = 0; s[i] != '\0'; i++)
            if (!(s[i] == ' ' || (s[i] >= '0' && s[i] <= '9'))) propre = 0;
        VERIFIER(propre);
    }
    VERIFIER(nombre(10400600.0f, RCIM_DESC_E, 3, 4, "  1.0401E+07"));
    VERIFIER(nombre(10400600.0f, RCIM_DESC_F, 2, 1, "10400600.0"));
    VERIFIER(nombre(10400600.0f, RCIM_DESC_U, 0, 0, "1.04006E+07 "));
    /* LSE15 §6-2.4, §6-2.6 */
    VERIFIER(nombre(34.0f, RCIM_DESC_F, 0, 0, "34"));
    VERIFIER(nombre(-52.625f, RCIM_DESC_E, 4, 6, "  -5.262500E+01"));
    VERIFIER(nombre(10.0f, RCIM_DESC_F, 3, 0, " 10"));
    VERIFIER(nombre(55.0f, RCIM_DESC_U, 0, 0, "55 "));
    VERIFIER(nombre(5.7f, RCIM_DESC_F, 0, 3, "5.700"));
    /* compléments */
    VERIFIER(nombre(9.99996f, RCIM_DESC_E, 2, 4, " 1.0000E+01"));        /* retenue de l'arrondi */
    VERIFIER(nombre(0.0f, RCIM_DESC_E, 2, 2, " 0.00E+00"));
    VERIFIER(nombre(25.0f, RCIM_DESC_E, 4, 0, "   3E+01"));

    /* --- tableaux (IMP-0210) -------------------------------------------------- */
    VERIFIER_EGAL(ERR_AUCUNE, donnees_creer_tableau(2, 3, 2, &h));
    VERIFIER(donnees_tableau_bornes(h, &dims, &d1, &d2) == ERR_AUCUNE && dims == 2 && d1 == 3 && d2 == 2);
    VERIFIER(donnees_rang(h, 2, 2, 1, &rang) == ERR_AUCUNE && rang == 2);    /* ligne par ligne */
    VERIFIER(donnees_rang(h, 2, 3, 2, &rang) == ERR_AUCUNE && rang == 5);
    VERIFIER(donnees_rang(h, 2, 1.4f, 1.6f, &rang) == ERR_AUCUNE && rang == 1);   /* arrondi */
    VERIFIER_EGAL(ERR_E_NOMBRE_INDICES, donnees_rang(h, 1, 1, 0, &rang));
    VERIFIER_EGAL(ERR_E_INDICE_NUL, donnees_rang(h, 2, 0.4f, 1, &rang));
    VERIFIER_EGAL(ERR_E_INDICE_TROP_GRAND, donnees_rang(h, 2, 4, 1, &rang));
    /* indice entier exact pris tel quel, les autres arrondis (IMP-0244 ; audit du code, AP-02) */
    VERIFIER(donnees_rang(h, 2, 2.5f, 1, &rang) == ERR_AUCUNE && rang == 4);    /* moitié vers le haut */
    VERIFIER_EGAL(ERR_E_INDICE_NUL, donnees_rang(h, 2, -2, 1, &rang));
    VERIFIER_EGAL(ERR_E_INDICE_NUL, donnees_rang(h, 2, -0.0f, 1, &rang));
    VERIFIER_EGAL(ERR_E_INDICE_TROP_GRAND, donnees_rang(h, 2, 16777215.0f, 1, &rang));
    VERIFIER_EGAL(ERR_E_ELEMENT_SANS_VALEUR, donnees_lire_element(h, 5, &v));
    VERIFIER_EGAL(ERR_AUCUNE, donnees_ecrire_element(h, 5, 3.71f));
    attendu = 3.71f;              /* constante rangée en float : ORCA/C garde sinon la précision étendue */
    VERIFIER(donnees_lire_element(h, 5, &v) == ERR_AUCUNE && v == attendu);
    VERIFIER_EGAL(ERR_E_ELEMENT_SANS_VALEUR, donnees_lire_element(h, 4, &v));
    VERIFIER(donnees_copier(h, &h2) == ERR_AUCUNE && donnees_lire_element(h2, 5, &v) == ERR_AUCUNE
             && v == attendu);
    objets_liberer(h); objets_liberer(h2);
    VERIFIER(donnees_creer_tableau(1, 5.5f, 0, &h) == ERR_AUCUNE
             && donnees_tableau_bornes(h, &dims, &d1, &d2) == ERR_AUCUNE && d1 == 6 && d2 == 1);
    VERIFIER_EGAL(ERR_E_NOMBRE_INDICES, donnees_rang(h, 2, 1, 1, &rang));
    objets_liberer(h);
    VERIFIER_EGAL(ERR_E_DIMENSION_NULLE, donnees_creer_tableau(1, 0, 0, &h));
    VERIFIER_EGAL(ERR_E_DIMENSION_NULLE, donnees_creer_tableau(2, 3, -1, &h));
    VERIFIER_EGAL(ERR_E_TABLEAU_TROP_GRAND, donnees_creer_tableau(2, 300, 300, &h));

    /* image d'un tableau relue par CHARGER : en-tête contrôlé (audit du code, AC-23) */
    {
        static unsigned char img[64];
        unsigned long lg;
        VERIFIER(donnees_creer_tableau(2, 3, 4, &h) == ERR_AUCUNE);
        lg = objets_longueur(h);                               /* 6 + 2 + 4 × 12 */
        VERIFIER(lg == 56 && objets_copier(h, 0, img, (unsigned int) lg));
        objets_liberer(h);
        VERIFIER(donnees_image_tableau_valide(img, lg));
        VERIFIER(!donnees_image_tableau_valide(img, lg - 1) && !donnees_image_tableau_valide(img, 5));
        img[1] = 1;                                            /* une dimension, d2 = 4 */
        VERIFIER(!donnees_image_tableau_valide(img, lg));
        img[1] = 3;
        VERIFIER(!donnees_image_tableau_valide(img, lg));
        img[1] = 2;
        img[0] = DONNEES_GENRE_CHAINE;
        VERIFIER(!donnees_image_tableau_valide(img, lg));
        img[0] = DONNEES_GENRE_TABLEAU;
        img[2] = 0;
        img[3] = 0;                                            /* d1 = 0 */
        VERIFIER(!donnees_image_tableau_valide(img, lg));
    }

    VERIF_FIN();
}
