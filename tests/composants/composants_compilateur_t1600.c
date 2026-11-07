/* composants_compilateur_t1600.c — compilateur, profil T1600 (§8 bis, IMP-0189, IMP-0190), toutes étapes
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

    VERIF_DEBUT("composants/compilateur t1600");

    /* --- profil T1600 version 4 (§8 bis) ---------------------------------------------------- */
    configuration_session.profil = PROFIL_T1600_V4;
    SEQ("5 X_SH(1)", 0x40,0x00,0x00, 0x24, 0xBC, 0x99);
    SEQ("5 X_TEM", 0x40,0x00,0x00, 0x8F, 0x99);
    SEQ("5 X_DAT", 0x40,0x00,0x00, 0xA7, 0x99);
    SEQ("5 Y_@+3", 0x40,0x00,0x00, 0x42, 0x26, 0x80, 0x99);
    SEQ("5 Y_$", 0x40,0x00,0x00, 0x43, 0x99);
    SEQ("10 ALLER EN @+3", 0x42, 0x26, 0x80, 0xC3);         /* V15 */
    SEQ("5 ALLER EN 255", 0xC2,0xFF);
    VERIFIER(accepte("255 A_1"));
    VERIFIER(accepte("TABLEAU M[3]"));                     /* déclaration en bureau admise */
    VERIFIER(accepte("5 AFFICHER[62'*']"));
    VERIFIER(rejet("5 AFFICHER[63'*']", ERR_C_FACTEUR_AU_DELA_PROFIL, 11));
    VERIFIER(rejet("5 SUPPRIMER 'F'", ERR_C_HORS_PROFIL, 2));
    VERIFIER(rejet("256 A_1", ERR_C_NUMERO_AU_DELA_PROFIL, 0));
    VERIFIER(rejet("5 DAT_1", ERR_C_MOT_RESERVE_PROFIL, 2));
    VERIFIER(rejet("5 REPRENDRE_1", ERR_C_IDENTIFICATEUR_TROP_LONG, 2));
    VERIFIER(rejet("5 X_DAT()", ERR_C_HORS_PROFIL, 4));
    VERIFIER(rejet("5 I_@50", ERR_C_HORS_PROFIL, 4));
    /* INRDP75 s'en tient à TEM et ignore .n. (IMP-0189, IMP-0190) */
    VERIFIER(rejet("5 X_TEM()", ERR_C_HORS_PROFIL, 4));
    VERIFIER(rejet("5 C_.27 12.", ERR_C_HORS_PROFIL, 4));
    VERIFIER(rejet("5 C_.65.", ERR_C_HORS_PROFIL, 4));
    VERIFIER(rejet("5 C_'A'.8.'B'", ERR_C_INSTRUCTION_INCORRECTE, 7));
    VERIFIER(rejet("5 AFFICHER[.7.]", ERR_C_HORS_PROFIL, 11));
    VERIFIER(rejet("5 AFFICHER[3.7.]", ERR_C_HORS_PROFIL, 12));
    VERIFIER(rejet("5 AFFICHER['A'.7.]", ERR_C_FORMAT_CROCHET, 14));
    VERIFIER(rejet("5 FAIRE 8 TANT QUE A<4", ERR_C_POUR_MANQUANT, 10));     /* IMP-0277 */

    /* --- profil MITRA 15 : .n. rejeté aussi dans un format ------------------------------- */
    configuration_session.profil = PROFIL_MITRA15_V8;
    VERIFIER(rejet("5 AFFICHER[.7.]", ERR_C_HORS_PROFIL, 11));
    VERIFIER(rejet("5 AFFICHER['A'.7.]", ERR_C_FORMAT_CROCHET, 14));
    VERIFIER(rejet("5 FAIRE 8 TANT QUE A<4", ERR_C_POUR_MANQUANT, 10));     /* IMP-0277 */

    /* --- profil F4 : les constructions du T1600 (IMP-0276) ------------------------------- */
    configuration_session.profil = PROFIL_T1600_F4;
    SEQ("5 X_SH(1)", 0x40,0x00,0x00, 0x24, 0xBC, 0x99);
    SEQ("5 X_TEM", 0x40,0x00,0x00, 0x8F, 0x99);
    SEQ("5 X_TEM()", 0x40,0x00,0x00, 0x8F, 0x99);
    SEQ("5 Y_$", 0x40,0x00,0x00, 0x43, 0x99);
    SEQ("10 ALLER EN @+3", 0x42, 0x26, 0x80, 0xC3);
    SEQ("5 C_.27 12.", 0x40,0x00,0x00, 0x03,0x02,0x1B,0x0C, 0x99);
    SEQ("5 C_'A'.8.'B'", 0x40,0x00,0x00, 0x03,0x03,0x41,0x08,0x42, 0x99);
    SEQ("5 C_.65.", 0x40,0x00,0x00, 0x03,0x01,0x41, 0x99);
    /* dans un format, même lecture qu'en opérande (IMP-0190 ; MULT 50) */
    SEQ("5 AFFICHER['A'.7.'U']", 0xB7, 0xB0,0x01,0x03,0x41,0x07,0x55);
    SEQ("5 AFFICHER['R'.7 7 7.]", 0xB7, 0xB0,0x01,0x04,0x52,0x07,0x07,0x07);
    SEQ("5 AFFICHER[.27 12.,/]", 0xB7, 0xB0,0x01,0x02,0x1B,0x0C, 0xB1,0x01);
    SEQ("5 AFFICHER[3.7.]", 0xB7, 0xB0,0x03,0x01,0x07);
    SEQ("5 AFFICHER[*'A'.7.] N", 0xB7, 0x40,0x00,0x00, 0xB0,0x80,0x02,0x41,0x07);
    SEQ("5 AFFICHER['L''A']", 0xB7, 0xB0,0x01,0x03,0x4C,0x27,0x41);
    VERIFIER(rejet("5 AFFICHER['A'.7]", ERR_C_EQUIVALENT_NON_TERMINE, 14));
    VERIFIER(rejet("5 AFFICHER[.128.]", ERR_C_EQUIVALENT_HORS_BORNES, 12));
    VERIFIER(rejet("5 AFFICHER['A'.]", ERR_C_EQUIVALENT_NON_TERMINE, 14));
    VERIFIER(rejet("5 AFFICHER['A", ERR_C_CHAINE_NON_TERMINEE, 11));
    VERIFIER(rejet("5 AFFICHER['A' .7.]", ERR_C_FORMAT_CROCHET, 15));
    VERIFIER(accepte("50 AFFICHER[/,' A'.7.'U'.7.' '.7.'R'.7.'E'.7.'V'.7.'O'.7.'I'.7.'R'.7 7 7.]"));
    /* FAIRE sans variable (IMP-0277 ; SINUS 105) */
    SEQ("105 FAIRE 105 TANT QUE A<4", 0x00,0x69, 0xD2, 0x40,0x00,0x00, 0x27, 0x91, 0xD0);  /* V17 */
    SEQ("5 FAIRE 8 TANT QUE A<4;B_1", 0x2B, 0xD2, 0x40,0x00,0x00, 0x27, 0x91, 0xD0,
        0x40,0x01,0x00, 0x24, 0x99);
    SEQ("5 FAIRE 8 POUR I_1 TANT QUE I<4", 0x2B, 0x24, 0xC9,0x00,0x00, 0xCE, 0x40,0x00,0x00, 0x27, 0x91, 0xD0);
    SEQ("5 FAIRE 8 POUR I_1 PAS 2 TANT QUE I<4", 0x2B, 0x24, 0xC9,0x00,0x00, 0x25, 0xCA,
        0x40,0x00,0x00, 0x27, 0x91, 0xD0);
    VERIFIER(accepte("105 FAIRE 105 TANT QUE TEM()-Q<4"));
    VERIFIER(rejet("5 FAIRE 8 JUSQUA 3", ERR_C_POUR_MANQUANT, 10));
    VERIFIER(rejet("5 FAIRE 8 PAS 2 TANT QUE A<4", ERR_C_POUR_MANQUANT, 10));
    VERIFIER(rejet("5 FAIRE 8 TANT A<4", ERR_C_TANT_SANS_QUE, 15));
    VERIFIER(rejet("5 FAIRE 8", ERR_C_POUR_MANQUANT, 9));
    VERIFIER(rejet("5 C_.27", ERR_C_EQUIVALENT_NON_TERMINE, 4));
    VERIFIER(rejet("5 C_..", ERR_C_EQUIVALENT_NON_TERMINE, 4));
    VERIFIER(rejet("5 C_.128.", ERR_C_EQUIVALENT_HORS_BORNES, 5));
    VERIFIER(accepte("255 A_1"));
    VERIFIER(accepte("TABLEAU M[3]"));
    VERIFIER(accepte("5 AFFICHER[62'*']"));
    VERIFIER(rejet("5 AFFICHER[63'*']", ERR_C_FACTEUR_AU_DELA_PROFIL, 11));
    VERIFIER(rejet("5 SUPPRIMER 'F'", ERR_C_HORS_PROFIL, 2));
    VERIFIER(rejet("256 A_1", ERR_C_NUMERO_AU_DELA_PROFIL, 0));
    SEQ("5 X_DAT", 0x40,0x00,0x00, 0xA7, 0x99);                /* IMP-0189 */
    SEQ("5 X_DAT()", 0x40,0x00,0x00, 0xA7, 0x99);
    VERIFIER(rejet("5 I_@50", ERR_C_HORS_PROFIL, 4));
    configuration_session.profil = PROFIL_MITRA15_V8;

    configuration_session.profil = PROFIL_MITRA15_V8;
    symboles_vider();
    VERIF_FIN();
}
