/* composants_compilateur_2d.c — compilateur, étape 2d : déclaration de
   procédure et clause LOCAL (§13), appel de procédure en instruction, UN et
   ZER (T1600)
   EXI: EXI-0001, EXI-0010, EXI-0015, EXI-0042, EXI-0051, EXI-0061
   VM: VM-1301, VM-3202, VM-3203  IV: IV-IF-0001, ALG-0901
   IMP: IMP-0121, IMP-0167, IMP-0172, IMP-0174 à IMP-0177, IMP-0180,
        IMP-0185, IMP-0187
   Spécification des tests : chapitre Compilateur §13, §19 ; fiche de codage
   RCI-M §11 (V11, V12).
   Niveau : composants. */

#include "compilateur_outils.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

int main(void)
{
    unsigned int id1;
    unsigned int id2;

    VERIF_DEBUT("composants/compilateur 2d");

    /* --- déclaration de procédure (IMP-0174, IMP-0175) ------------------------------- */
    SEQ("10 PROCEDURE &PGCD(A,B)", 0xCD,0x00,0x00, 0x02, 0x01,0x00, 0x02,0x00, 0x00);
    SEQ("20 PROCEDURE &PRO(A,B) LOCAL A,M,N;TABLEAU M[3,A]", 0xCD,0x00,0x00, 0x02,
        0x01,0x00, 0x02,0x00, 0x03, 0x01,0x00, 0x03,0x00, 0x04,0x00,
        0x26, 0x40,0x01,0x00, 0xA9,0x03,0x00,0x02);
    SEQ("1 PROCEDURE &FORM();RESULTAT RAC(SIN(X)^2+COS(Y)^2)", 0xCD,0x00,0x00, 0x00, 0x00,
        0x40,0x01,0x00, 0x8A, 0x25, 0x85, 0x40,0x02,0x00, 0x8B, 0x25, 0x85, 0x80, 0x86, 0xCB);
    SEQ("56 PROCEDURE &TRAC(&F,MIN,MAX,PA) LOCAL MIN,MAX,PA", 0xCD,0x00,0x00, 0x04,
        0x01,0x00, 0x02,0x00, 0x03,0x00, 0x04,0x00, 0x03, 0x02,0x00, 0x03,0x00, 0x04,0x00);
    VERIFIER(symboles_chercher("F", 1, SYM_PROCEDURE, &id1));
    VERIFIER(!symboles_chercher("F", 1, SYM_IDENTIFICATEUR, &id2));
    SEQ("1 PROCEDURE &UNIT(A,N) LOCAL N,I,J", 0xCD,0x00,0x00, 0x02, 0x01,0x00, 0x02,0x00,
        0x03, 0x02,0x00, 0x03,0x00, 0x04,0x00);
    VERIFIER(accepte("5 PROCEDURE &P(A) LOCAL A"));

    /* V11 : lignes 25 puis 27, compilées dans cet ordre */
    symboles_vider();
    SEQ_SUITE("25 PROCEDURE &FACT(N) LOCAL N", 0xCD,0x00,0x00, 0x01, 0x01,0x00, 0x01, 0x01,0x00);
    SEQ_SUITE("27 RESULTAT SI N=0 ALORS 1 SINON N*&FACT(N-1)", 0x40,0x01,0x00, 0x23, 0x90,
        0xC0,0x0A, 0x24, 0xC1,0x19, 0x40,0x01,0x00, 0x40,0x00,0x00, 0x40,0x01,0x00, 0x24, 0x81,
        0xA8,0x01,0x01, 0x83, 0xCB);

    /* appel compilé avant la déclaration : même symbole (EXI-0001, §13.5) */
    symboles_vider();
    VERIFIER(compiler("30 X_&P(1)"));
    recuperer();
    VERIFIER(symboles_chercher("P", 1, SYM_PROCEDURE, &id1));
    VERIFIER(compiler("40 PROCEDURE &P(Y) LOCAL Y"));
    recuperer();
    VERIFIER(symboles_chercher("P", 1, SYM_PROCEDURE, &id2));
    VERIFIER_EGAL(id1, id2);
    VERIFIER(accepte("8 A_&FACT(7,2)"));            /* arité vérifiée à l'exécution */

    /* --- appel en instruction (IMP-0121, IMP-0177 ; V12) ------------------------------ */
    SEQ("5 &TRAC(&F1,0,2,0.25)", 0x40,0x00,0x00, 0x40,0x01,0x00, 0x23, 0x25,
        0x02,0x00,0x00,0x80,0x3E, 0xA8,0x00,0x04);
    SEQ("5 &AXE(10)", 0x40,0x00,0x00, 0x2D, 0xA8,0x00,0x01);

    /* --- diagnostics de la déclaration (IMP-0175, IMP-0191) ---------------------------- */
    VERIFIER(rejet("5 A_1;PROCEDURE &P()", ERR_C_DEBUT_INSTRUCTION, 6));
    VERIFIER(rejet("PROCEDURE &P(A)", ERR_C_BUREAU_PROCEDURE, 0));
    VERIFIER(rejet("5 PROCEDURE P(A)", ERR_C_DECLARATION_NOM, 12));
    VERIFIER(rejet("5 PROCEDURE &P", ERR_C_DECLARATION_PARENTHESES, 14));
    VERIFIER(rejet("5 PROCEDURE &P(3)", ERR_C_PARAMETRES_DECLARATION, 15));
    VERIFIER(rejet("5 PROCEDURE &P(A B)", ERR_C_PARAMETRES_DECLARATION, 17));
    VERIFIER(rejet("5 PROCEDURE &P(A[1])", ERR_C_PARAMETRE_INDICE, 15));
    VERIFIER(rejet("5 PROCEDURE &P(A,A)", ERR_C_PARAMETRE_DOUBLE, 17));
    VERIFIER(rejet("5 PROCEDURE &P(&F,&F)", ERR_C_PARAMETRE_PROCEDURE_DOUBLE, 18));
    VERIFIER(rejet("5 PROCEDURE &P(&P)", ERR_C_PARAMETRE_HOMONYME, 15));
    VERIFIER(rejet("5 PROCEDURE &P(A) LOCAL &F", ERR_C_LOCAL_PROCEDURE, 24));
    VERIFIER(rejet("5 PROCEDURE &P(A) LOCAL I,I", ERR_C_LOCAL_DOUBLE, 26));
    VERIFIER(rejet("5 PROCEDURE &P(A) X", ERR_C_DECLARATION_SUITE, 18));
    VERIFIER(rejet("5 PROCEDURE &P(PAS)", ERR_C_MOT_RESERVE, 15));   /* LSE15 l. 2153 : PAS */
    VERIFIER(rejet("&FACT(5)", ERR_C_BUREAU_APPEL_INSTRUCTION, 0));

    /* --- UN, ZER (T1600, §8 bis) --------------------------------------------------------- */
    VERIFIER(rejet("5 UN T", ERR_C_HORS_PROFIL, 2));
    VERIFIER(accepte("5 UN_1"));
    configuration_session.profil = PROFIL_T1600_V4;
    SEQ("5 UN T", 0x40,0x00,0x00, 0x9A);
    SEQ("2 UN U;ZER T", 0x40,0x00,0x00, 0x9A, 0x40,0x01,0x00, 0x9B);
    configuration_session.profil = PROFIL_MITRA15_V8;

    symboles_vider();
    VERIF_FIN();
}
