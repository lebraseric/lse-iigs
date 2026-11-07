/* composants_compilateur_2a.c — compilateur, étape 2a : point d'entrée, lexique, expressions, affectation, commentaire
   EXI: EXI-0001, EXI-0004, EXI-0011, EXI-0018, EXI-0051, EXI-0061
   VM: VM-3201 à VM-3203  IV: IV-IF-0001, ALG-0901
   IMP: IMP-0164 à IMP-0191, IMP-0195, IMP-0196
   Spécification des tests : chapitre Compilateur §19 ; fiche de codage RCI-M
   §11 ; catalogue des erreurs (Gestionnaire des erreurs §7). La flèche
   d'affectation s'écrit _ et l'exponentiation ^.
   Niveau : composants. Liée avec compilateur, rcim, symboles, erreurs,
   objets, memoire_iigs (aucun bouchon : IMP-0170). */

#include "compilateur_outils.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

int main(void)
{
    unsigned int index1;
    unsigned int index2;
    unsigned int id;
    EntreeSymbole s;
    char longue[90];
    unsigned char premier[64];
    unsigned int lg_premier;

    VERIF_DEBUT("composants/compilateur 2a");


    /* --- vecteurs de la fiche (affectations) ------------------------------ */
    SEQ("5 A_B+C", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x40,0x02,0x00, 0x80, 0x99);
    VERIFIER_EGAL(LSE_LIGNE_PROGRAMME, r.type);
    VERIFIER_EGAL(5, r.resultat.ligne_programme.numero_ligne);
    SEQ("5 X_2^3^2", 0x40,0x00,0x00, 0x25, 0x26, 0x85, 0x25, 0x85, 0x99);
    SEQ("5 X_-A^2", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x25, 0x85, 0x82, 0x99);
    SEQ("5 X_A*(-1)", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x22, 0x83, 0x99);
    SEQ("5 X_-200", 0x40,0x00,0x00, 0x01,0x38,0xFF, 0x99);
    SEQ("10 I_@50", 0x40,0x00,0x00, 0x06,0x32, 0x99);

    /* --- précédence, associativité, repli du signe (§5) ------------------- */
    SEQ("5 X_-2^2", 0x40,0x00,0x00, 0x25, 0x25, 0x85, 0x82, 0x99);
    SEQ("5 X_+A", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x99);
    SEQ("5 X_-A+3/DELTA*ALPHA-X^ABS(D)-1",
        0x40,0x00,0x00, 0x40,0x01,0x00, 0x82, 0x26, 0x40,0x02,0x00, 0x84,
        0x40,0x03,0x00, 0x83, 0x80, 0x40,0x00,0x00, 0x40,0x04,0x00, 0x87, 0x85,
        0x81, 0x24, 0x81, 0x99);
    SEQ("5 X_A-5", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x28, 0x81, 0x99);
    SEQ("5   X _  A  +  1", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x24, 0x80, 0x99);
    SEQ("5 X_-(5)", 0x40,0x00,0x00, 0x1E, 0x99);
    SEQ("5 X_-(-5)", 0x40,0x00,0x00, 0x28, 0x99);

    /* --- constantes numériques (fiche §2) ----------------------------------- */
    SEQ("5 X_3E-2", 0x40,0x00,0x00, 0x02,0x8F,0xC2,0xF5,0x3C, 0x99);
    SEQ("5 X_1000", 0x40,0x00,0x00, 0x01,0xE8,0x03, 0x99);
    SEQ("5 X_1E3", 0x40,0x00,0x00, 0x01,0xE8,0x03, 0x99);
    SEQ("5 X_100000", 0x40,0x00,0x00, 0x02,0x00,0x50,0xC3,0x47, 0x99);
    SEQ("5 X_1E-50", 0x40,0x00,0x00, 0x23, 0x99);   /* dépassement vers zéro accepté */

    /* --- fonctions et appels ------------------------------------------------ */
    SEQ("5 X_SIN (1)", 0x40,0x00,0x00, 0x24, 0x8A, 0x99);
    SEQ("5 X_SIN(SIN)", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x8A, 0x99);
    SEQ("5 SIN_3", 0x40,0x00,0x00, 0x26, 0x99);
    SEQ("5 SIN[2]_1", 0x25, 0x41,0x00,0x00,0x01, 0x24, 0x99);
    SEQ("5 X_POS(C,1,'A')", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x24, 0x03,0x01,0x41, 0x9E, 0x99);
    SEQ("5 X_SCH(C,2,3,P)", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x25, 0x26, 0x40,0x02,0x00,
        0xA3,0x04, 0x99);
    SEQ("5 Y_&RAC(3,20)", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x26, 0x37, 0xA8,0x01,0x02, 0x99);
    SEQ("5 Z_&FORM()+1", 0x40,0x00,0x00, 0x40,0x01,0x00, 0xA8,0x01,0x00, 0x24, 0x80, 0x99);
    SEQ("5 X_&TRAC(&F1,0)", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x40,0x02,0x00, 0x23,
        0xA8,0x01,0x02, 0x99);
    SEQ("5 X_TEM()", 0x40,0x00,0x00, 0x8F, 0x99);
    SEQ("5 X_DAT()", 0x40,0x00,0x00, 0xA7, 0x99);

    /* --- chaînes (IMP-0181) -------------------------------------------------- */
    SEQ("5 A_'D''UN'", 0x40,0x00,0x00, 0x03,0x04,0x44,0x27,0x55,0x4E, 0x99);
    symboles_vider();
    VERIFIER(compiler("5 A_'MESSAGE TRES LONG POUR LE PARTAGE'"));
    recuperer();
    VERIFIER_EGAL(0x80, obtenu[4]);
    index1 = rcim_lire_u16(obtenu + 5);
    VERIFIER(compiler("8 B_'MESSAGE TRES LONG POUR LE PARTAGE'"));
    recuperer();
    index2 = rcim_lire_u16(obtenu + 5);
    VERIFIER_EGAL(index1, index2);
    VERIFIER_EGAL(1, constantes_nombre());
    VERIFIER(compiler("A_'MESSAGE TRES LONG POUR LE PARTAGE'"));   /* bureau : en ligne */
    recuperer();
    VERIFIER_EGAL(LSE_LIGNE_BUREAU, r.type);
    VERIFIER_EGAL(33, obtenu[4]);
    VERIFIER_EGAL(1, constantes_nombre());

    /* --- commentaire, instructions multiples, ligne vide ---------------------- */
    SEQ("1 * CALCUL DE L'AIRE", 0xCC,0x11,0x20,0x43,0x41,0x4C,0x43,0x55,0x4C,0x20,0x44,
        0x45,0x20,0x4C,0x27,0x41,0x49,0x52,0x45);
    SEQ("1*COMMENTAIRE", 0xCC,0x0B,0x43,0x4F,0x4D,0x4D,0x45,0x4E,0x54,0x41,0x49,0x52,0x45);
    SEQ("5 A_1;B_2", 0x40,0x00,0x00, 0x24, 0x99, 0x40,0x01,0x00, 0x25, 0x99);
    SEQ("5 A_1;;", 0x40,0x00,0x00, 0x24, 0x99);
    symboles_vider();
    VERIFIER(compiler("60 "));
    recuperer();
    VERIFIER_EGAL(0, lg_obtenu);

    /* --- ligne machine de bureau (IMP-0171) ----------------------------------- */
    SEQ("VA_60", 0x40,0x00,0x00, 0x00,0x3C, 0x99);
    VERIFIER_EGAL(LSE_LIGNE_BUREAU, r.type);
    VERIFIER(symboles_chercher("VA", 2, SYM_IDENTIFICATEUR, &id) && symboles_lire(id, &s));
    VERIFIER_EGAL(ORIGINE_MACHINE_BUREAU, s.origine);
    VERIFIER(rejet("X_&FACT(5)", ERR_C_BUREAU_APPEL_FONCTION, 2));

    /* --- EXI-0001 : même texte, même séquence ------------------------------------ */
    symboles_vider();
    VERIFIER(compiler("5 X_A+1"));
    recuperer();
    lg_premier = lg_obtenu;
    memcpy(premier, obtenu, lg_obtenu);
    VERIFIER(compiler("7 B_2"));
    recuperer();
    VERIFIER(compiler("5 X_A+1"));
    recuperer();
    VERIFIER(lg_obtenu == lg_premier && memcmp(obtenu, premier, lg_premier) == 0);

    /* --- types (IMP-0188) ---------------------------------------------------------- */
    VERIFIER(rejet("5 X_'A'+1", ERR_C_CHAINE_AU_LIEU_NOMBRE, 4));
    VERIFIER(rejet("5 X_SIN('A')", ERR_C_CHAINE_AU_LIEU_NOMBRE, 8));
    VERIFIER(rejet("5 C_3!'A'", ERR_C_NOMBRE_AU_LIEU_CHAINE, 4));
    VERIFIER(rejet("5 X_LGR(3)", ERR_C_NOMBRE_AU_LIEU_CHAINE, 8));
    VERIFIER(rejet("5 X_T['X']", ERR_C_INDICE_NON_NUMERIQUE, 6));
    VERIFIER(accepte("5 X_A+B"));
    VERIFIER(accepte("5 X_'A'"));
    VERIFIER(accepte("5 X_A!B"));
    VERIFIER(accepte("5 X_T[A]"));
    VERIFIER(accepte("5 X_&F('A')+1"));

    /* --- ligne et numéro (IMP-0191) ------------------------------------------------- */
    memset(longue, 'A', 80);
    longue[0] = '5'; longue[1] = ' '; longue[2] = 'A'; longue[3] = '_';
    longue[80] = '\0';
    VERIFIER(rejet(longue, ERR_C_LIGNE_TROP_LONGUE, 0));
    VERIFIER(rejet("60", ERR_C_NUMERO_SANS_ESPACE, 2));
    VERIFIER(rejet("5A_1", ERR_C_NUMERO_SANS_ESPACE, 1));
    VERIFIER(rejet("0 A_1", ERR_C_NUMERO_HORS_BORNES, 0));
    VERIFIER(rejet("32001 A_1", ERR_C_NUMERO_HORS_BORNES, 0));
    VERIFIER(rejet("251 A_1", ERR_C_NUMERO_AU_DELA_PROFIL, 0));
    VERIFIER(accepte("250 A_1"));

    /* --- lexique ------------------------------------------------------------------- */
    VERIFIER(rejet("5 x_1", ERR_C_MINUSCULE, 2));
    VERIFIER(rejet("5 ALPHAB_1", ERR_C_IDENTIFICATEUR_TROP_LONG, 2));
    VERIFIER(rejet("5 TANT_1", ERR_C_MOT_RESERVE, 2));
    VERIFIER(rejet("5 REPRENDRE_1", ERR_C_MOT_RESERVE_PROFIL, 2));
    VERIFIER(accepte("5 DAT_1"));
    VERIFIER(rejet("5 X_1E99", ERR_C_CONSTANTE_HORS_PLAGE, 4));
    VERIFIER(rejet("5 X_3E", ERR_C_CONSTANTE_MAL_FORMEE, 4));
    VERIFIER(rejet("5 X_'ABC", ERR_C_CHAINE_NON_TERMINEE, 4));
    VERIFIER(rejet("5 X_&1(2)", ERR_C_NOM_PROCEDURE_INCORRECT, 4));
    VERIFIER(rejet("5 X_&PROCED(1)", ERR_C_NOM_PROCEDURE_TROP_LONG, 4));
    VERIFIER(rejet("5 X_&P(1", ERR_C_APPEL_MAL_FORME, 8));

    /* --- expressions ------------------------------------------------------------------- */
    VERIFIER(rejet("5 X_A+B)", ERR_C_PARENTHESE_EN_EXCES, 7));
    VERIFIER(rejet("5 X_A B", ERR_C_OPERATEUR_MANQUANT, 6));
    VERIFIER(rejet("5 X_A %", ERR_C_CARACTERE_INTERDIT, 6));
    VERIFIER(rejet("5 X_A]", ERR_C_INSTRUCTION_INCORRECTE, 5));
    VERIFIER(rejet("5 X_A*-B", ERR_C_OPERATEURS_ADJACENTS, 6));
    VERIFIER(rejet("5 A_B+*", ERR_C_OPERATEURS_ADJACENTS, 6));
    VERIFIER(rejet("5 A_", ERR_C_OPERANDE_MANQUANT, 4));
    VERIFIER(rejet("5 X_(A+B", ERR_C_PARENTHESE_MANQUANTE, 8));
    VERIFIER(rejet("5 X_A[1", ERR_C_CROCHET_MANQUANT, 7));
    VERIFIER(rejet("5 X_A[1,2,3]", ERR_C_TROP_D_INDICES, 9));
    VERIFIER(rejet("5 X_SIN(1,2)", ERR_C_ARGUMENTS_FONCTION, 10));
    VERIFIER(rejet("5 X_POS('A',1)", ERR_C_ARGUMENTS_FONCTION, 4));
    VERIFIER(rejet("5 X_CNB(C,1,2)", ERR_C_ARGUMENT_FACULTATIF, 12));
    VERIFIER(rejet("5 X_SH(1)", ERR_C_HORS_PROFIL, 4));
    VERIFIER(rejet("5 I_@X", ERR_C_AROBASE_SANS_ENTIER, 4));
    VERIFIER(rejet("5 Y_$", ERR_C_HORS_PROFIL, 4));
    VERIFIER(rejet("5 C_.27.", ERR_C_HORS_PROFIL, 4));

    /* --- instructions ---------------------------------------------------------------------- */
    VERIFIER(rejet("5 FIN", ERR_C_DEBUT_INSTRUCTION, 2));
    VERIFIER(rejet("5 (A)_1", ERR_C_CIBLE_AFFECTATION, 2));
    VERIFIER(rejet("5 IMPRI A", ERR_C_AFFECTATION_MANQUANTE, 8));

    configuration_session.profil = PROFIL_MITRA15_V8;
    symboles_vider();
    VERIF_FIN();
}
