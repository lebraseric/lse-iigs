/* representation_rcim.c — écriture et parcours des éléments RCI-M
   EXI: EXI-0011 (ordre postfixé), EXI-0016, EXI-0020
   VM: VM-2401  IV: ANN-K-0008 (parcours)
   IMP: IMP-0103 à IMP-0128, IMP-0165, IMP-0166, IMP-0174, IMP-0177, IMP-0184,
        IMP-0280 (arrondi des réels du T1600)
   Spécification des tests : fiche de codage RCI-M §11 (vecteurs V1 à V17),
   reconstruits élément par élément par l'API d'écriture, le compilateur
   n'existant pas encore. Chaque séquence est comparée octet par octet, puis
   validée par le parcours.
   Niveau : représentation. Liée avec src/noyau/rcim.c. */

#include <string.h>
#include <float.h>
#include "../commun/verif.h"
#include "../../src/interfaces/profil.h"
#include "../../src/noyau/rcim.h"

static unsigned char octets[RCIM_LONGUEUR_MAX_SEQUENCE];
static TamponRCIM t;

static void nouveau(void)
{
    rcim_tampon_initialiser(&t, octets, RCIM_LONGUEUR_MAX_SEQUENCE);
}

/* Compare la séquence écrite au vecteur attendu ; affiche l'écart éventuel. */
static int conforme(const char *nom, const unsigned char *attendu, unsigned int n,
                    unsigned int nb_elements)
{
    unsigned int i;
    unsigned int nb;
    int ok = 1;

    if (t.curseur != n || memcmp(octets, attendu, n) != 0) {
        ok = 0;
        printf("  %s : obtenu  ", nom);
        for (i = 0; i < t.curseur; i++) printf("%02X ", octets[i]);
        printf("\n  %s : attendu ", nom);
        for (i = 0; i < n; i++) printf("%02X ", attendu[i]);
        printf("\n");
    }
    if (!rcim_sequence_valide(octets, t.curseur, &nb) || nb != nb_elements) {
        ok = 0;
        printf("  %s : parcours invalide ou %u element(s) au lieu de %u\n",
               nom, nb, nb_elements);
    }
    return ok;
}

#define TXT(s) ((const unsigned char *) (s))

int main(void)
{
    unsigned int si;
    unsigned int saut;
    unsigned int params[1];
    unsigned int locaux[1];
    CurseurSequence c;
    unsigned int nb;

    VERIF_DEBUT("representation/rcim");

    /* V1 — 5 A←B+C */
    { static const unsigned char v[] = {0x40,0x00,0x00, 0x40,0x01,0x00, 0x40,0x02,0x00, 0x80, 0x99};
      nouveau();
      rcim_ecrire_reference(&t, 0); rcim_ecrire_reference(&t, 1); rcim_ecrire_reference(&t, 2);
      rcim_ecrire_tag(&t, RCIM_OP_ADD); rcim_ecrire_tag(&t, RCIM_OP_AFFECTER);
      VERIFIER(conforme("V1", v, sizeof v, 5)); }

    /* V2 — 5 X←2↑3↑2 */
    { static const unsigned char v[] = {0x40,0x00,0x00, 0x25, 0x26, 0x85, 0x25, 0x85, 0x99};
      nouveau();
      rcim_ecrire_reference(&t, 0);
      rcim_ecrire_entier(&t, 2); rcim_ecrire_entier(&t, 3); rcim_ecrire_tag(&t, RCIM_OP_PUIS);
      rcim_ecrire_entier(&t, 2); rcim_ecrire_tag(&t, RCIM_OP_PUIS);
      rcim_ecrire_tag(&t, RCIM_OP_AFFECTER);
      VERIFIER(conforme("V2", v, sizeof v, 7)); }

    /* V3 — 5 X←-A↑2 */
    { static const unsigned char v[] = {0x40,0x00,0x00, 0x40,0x01,0x00, 0x25, 0x85, 0x82, 0x99};
      nouveau();
      rcim_ecrire_reference(&t, 0); rcim_ecrire_reference(&t, 1);
      rcim_ecrire_entier(&t, 2); rcim_ecrire_tag(&t, RCIM_OP_PUIS);
      rcim_ecrire_tag(&t, RCIM_OP_NEG); rcim_ecrire_tag(&t, RCIM_OP_AFFECTER);
      VERIFIER(conforme("V3", v, sizeof v, 6)); }

    /* V4 — 5 SI A=0 ALORS B←1 SINON B←2 (IMP-0166) */
    { static const unsigned char v[] = {0x40,0x00,0x00, 0x23, 0x90, 0xC0,0x0E, 0x40,0x01,0x00,
                                        0x24, 0x99, 0xC1,0x13, 0x40,0x01,0x00, 0x25, 0x99};
      nouveau();
      rcim_ecrire_reference(&t, 0); rcim_ecrire_entier(&t, 0); rcim_ecrire_tag(&t, RCIM_OP_EGAL);
      rcim_ecrire_branchement(&t, RCIM_CTL_SI_FAUX, &si);
      rcim_ecrire_reference(&t, 1); rcim_ecrire_entier(&t, 1); rcim_ecrire_tag(&t, RCIM_OP_AFFECTER);
      rcim_ecrire_branchement(&t, RCIM_CTL_SAUT, &saut);
      rcim_corriger(&t, si);
      rcim_ecrire_reference(&t, 1); rcim_ecrire_entier(&t, 2); rcim_ecrire_tag(&t, RCIM_OP_AFFECTER);
      rcim_corriger(&t, saut);
      VERIFIER(conforme("V4", v, sizeof v, 11)); }

    /* V5 — 30 CH←SI N>1 ALORS ' LETTRES ' SINON ' LETTRE ' */
    { static const unsigned char v[] = {0x40,0x00,0x00, 0x40,0x01,0x00, 0x24, 0x94, 0xC0,0x17,
          0x03,0x09,0x20,0x4C,0x45,0x54,0x54,0x52,0x45,0x53,0x20, 0xC1,0x21,
          0x03,0x08,0x20,0x4C,0x45,0x54,0x54,0x52,0x45,0x20, 0x99};
      nouveau();
      rcim_ecrire_reference(&t, 0); rcim_ecrire_reference(&t, 1); rcim_ecrire_entier(&t, 1);
      rcim_ecrire_tag(&t, RCIM_OP_SUP);
      rcim_ecrire_branchement(&t, RCIM_CTL_SI_FAUX, &si);
      rcim_ecrire_chaine_en_ligne(&t, TXT(" LETTRES "), 9);
      rcim_ecrire_branchement(&t, RCIM_CTL_SAUT, &saut);
      rcim_corriger(&t, si);
      rcim_ecrire_chaine_en_ligne(&t, TXT(" LETTRE "), 8);
      rcim_corriger(&t, saut);
      rcim_ecrire_tag(&t, RCIM_OP_AFFECTER);
      VERIFIER(conforme("V5", v, sizeof v, 9)); }

    /* V6 — 3 FAIRE 8 POUR I←T PAS -1 JUSQUA 1 */
    { static const unsigned char v[] = {0x2B, 0x40,0x01,0x00, 0xC9,0x00,0x00, 0x22, 0xCA, 0x24, 0xCF};
      nouveau();
      rcim_ecrire_entier(&t, 8); rcim_ecrire_reference(&t, 1);
      rcim_ecrire_tag_id(&t, RCIM_CTL_FAIRE_OUVRIR, 0);
      rcim_ecrire_entier(&t, -1); rcim_ecrire_tag(&t, RCIM_CTL_FAIRE_PAS);
      rcim_ecrire_entier(&t, 1); rcim_ecrire_tag(&t, RCIM_CTL_FAIRE_JUSQUA);
      VERIFIER(conforme("V6", v, sizeof v, 7)); }

    /* V7 — 200 FAIRE 202 POUR I←1 JUSQUA N */
    { static const unsigned char v[] = {0x01,0xCA,0x00, 0x24, 0xC9,0x00,0x00, 0xCE, 0x40,0x01,0x00, 0xCF};
      nouveau();
      rcim_ecrire_entier(&t, 202); rcim_ecrire_entier(&t, 1);
      rcim_ecrire_tag_id(&t, RCIM_CTL_FAIRE_OUVRIR, 0);
      rcim_ecrire_tag(&t, RCIM_CTL_FAIRE_PAS_IMPLICITE);
      rcim_ecrire_reference(&t, 1); rcim_ecrire_tag(&t, RCIM_CTL_FAIRE_JUSQUA);
      VERIFIER(conforme("V7", v, sizeof v, 6)); }

    /* V8 — 20 FAIRE 20 POUR X←A PAS (A/X-X)/2 TANT QUE ABS(X↑2-A)>1E-3 */
    { static const unsigned char v[] = {0x37, 0x40,0x01,0x00, 0xC9,0x00,0x00, 0x40,0x01,0x00,
          0x40,0x00,0x00, 0x84, 0x40,0x00,0x00, 0x81, 0x25, 0x84, 0xCA, 0x40,0x00,0x00, 0x25, 0x85,
          0x40,0x01,0x00, 0x81, 0x87, 0x02,0x6F,0x12,0x83,0x3A, 0x94, 0xD0};
      nouveau();
      rcim_ecrire_entier(&t, 20); rcim_ecrire_reference(&t, 1);
      rcim_ecrire_tag_id(&t, RCIM_CTL_FAIRE_OUVRIR, 0);
      rcim_ecrire_reference(&t, 1); rcim_ecrire_reference(&t, 0); rcim_ecrire_tag(&t, RCIM_OP_DIV);
      rcim_ecrire_reference(&t, 0); rcim_ecrire_tag(&t, RCIM_OP_SUB);
      rcim_ecrire_entier(&t, 2); rcim_ecrire_tag(&t, RCIM_OP_DIV);
      rcim_ecrire_tag(&t, RCIM_CTL_FAIRE_PAS);
      rcim_ecrire_reference(&t, 0); rcim_ecrire_entier(&t, 2); rcim_ecrire_tag(&t, RCIM_OP_PUIS);
      rcim_ecrire_reference(&t, 1); rcim_ecrire_tag(&t, RCIM_OP_SUB); rcim_ecrire_tag(&t, RCIM_OP_ABS);
      rcim_ecrire_reel(&t, 1E-3f); rcim_ecrire_tag(&t, RCIM_OP_SUP);
      rcim_ecrire_tag(&t, RCIM_CTL_FAIRE_TANT_QUE);
      VERIFIER(conforme("V8", v, sizeof v, 20)); }

    /* V9 — 5 AFFICHER['RES=',3F2.0,E3.6] A,B,C,D,E */
    { static const unsigned char v[] = {0xB7, 0xB0,0x01,0x04,0x52,0x45,0x53,0x3D, 0xB5,0x02,0x02,0x00,
          0x40,0x00,0x00, 0xB6, 0x40,0x01,0x00, 0xB6, 0x40,0x02,0x00, 0xB6, 0xB5,0x40,0x03,0x06,
          0x40,0x03,0x00, 0xB6, 0x40,0x04,0x00, 0xB6};
      nouveau();
      rcim_ecrire_tag(&t, RCIM_OP_AFF_TETE_FORMAT);
      rcim_ecrire_libelle(&t, 1, TXT("RES="), 4);
      rcim_ecrire_description(&t, RCIM_DESC_F, 3, 2, 0);
      rcim_ecrire_reference(&t, 0); rcim_ecrire_tag(&t, RCIM_OP_AFF_VALEUR);
      rcim_ecrire_reference(&t, 1); rcim_ecrire_tag(&t, RCIM_OP_AFF_VALEUR);
      rcim_ecrire_reference(&t, 2); rcim_ecrire_tag(&t, RCIM_OP_AFF_VALEUR);
      rcim_ecrire_description(&t, RCIM_DESC_E, 1, 3, 6);
      rcim_ecrire_reference(&t, 3); rcim_ecrire_tag(&t, RCIM_OP_AFF_VALEUR);
      rcim_ecrire_reference(&t, 4); rcim_ecrire_tag(&t, RCIM_OP_AFF_VALEUR);
      VERIFIER(conforme("V9", v, sizeof v, 14)); }

    /* V10 — 2 AFFICHER 'D''UN TRIANGLE : ';LIRE A,B,C */
    { static const unsigned char v[] = {0xB8, 0x03,0x10,0x44,0x27,0x55,0x4E,0x20,0x54,0x52,0x49,
          0x41,0x4E,0x47,0x4C,0x45,0x20,0x3A,0x20, 0xB6, 0xD1, 0x40,0x00,0x00, 0xBB,
          0x40,0x01,0x00, 0xBB, 0x40,0x02,0x00, 0xBB};
      nouveau();
      rcim_ecrire_tag(&t, RCIM_OP_AFF_TETE_SANS_FORMAT);
      rcim_ecrire_chaine_en_ligne(&t, TXT("D'UN TRIANGLE : "), 16);
      rcim_ecrire_tag(&t, RCIM_OP_AFF_VALEUR);
      rcim_ecrire_tag(&t, RCIM_CTL_LIRE_TETE);
      rcim_ecrire_reference(&t, 0); rcim_ecrire_tag(&t, RCIM_OP_LIRE_VALEUR);
      rcim_ecrire_reference(&t, 1); rcim_ecrire_tag(&t, RCIM_OP_LIRE_VALEUR);
      rcim_ecrire_reference(&t, 2); rcim_ecrire_tag(&t, RCIM_OP_LIRE_VALEUR);
      VERIFIER(conforme("V10", v, sizeof v, 10)); }

    /* V11 — 25 PROCEDURE &FACT(N) LOCAL N ; 27 RESULTAT SI N=0 ALORS 1 SINON N*&FACT(N-1) */
    { static const unsigned char v25[] = {0xCD,0x00,0x00, 0x01, 0x01,0x00, 0x01, 0x01,0x00};
      static const unsigned char v27[] = {0x40,0x01,0x00, 0x23, 0x90, 0xC0,0x0A, 0x24, 0xC1,0x19,
          0x40,0x01,0x00, 0x40,0x00,0x00, 0x40,0x01,0x00, 0x24, 0x81, 0xA8,0x01,0x01, 0x83, 0xCB};
      nouveau();
      params[0] = 1; locaux[0] = 1;
      rcim_ecrire_declaration_procedure(&t, 0, 1, params, 1, locaux);
      VERIFIER(conforme("V11 l.25", v25, sizeof v25, 1));
      nouveau();
      rcim_ecrire_reference(&t, 1); rcim_ecrire_entier(&t, 0); rcim_ecrire_tag(&t, RCIM_OP_EGAL);
      rcim_ecrire_branchement(&t, RCIM_CTL_SI_FAUX, &si);
      rcim_ecrire_entier(&t, 1);
      rcim_ecrire_branchement(&t, RCIM_CTL_SAUT, &saut);
      rcim_corriger(&t, si);
      rcim_ecrire_reference(&t, 1);
      rcim_ecrire_reference(&t, 0); rcim_ecrire_reference(&t, 1); rcim_ecrire_entier(&t, 1);
      rcim_ecrire_tag(&t, RCIM_OP_SUB); rcim_ecrire_appel(&t, RCIM_APPEL_EXPRESSION, 1);
      rcim_ecrire_tag(&t, RCIM_OP_MUL);
      rcim_corriger(&t, saut);
      rcim_ecrire_tag(&t, RCIM_CTL_RESULTAT);
      VERIFIER(conforme("V11 l.27", v27, sizeof v27, 14)); }

    /* V12 — 5 &TRAC(&F1,0,2,0.25) */
    { static const unsigned char v[] = {0x40,0x00,0x00, 0x40,0x01,0x00, 0x23, 0x25,
                                        0x02,0x00,0x00,0x80,0x3E, 0xA8,0x00,0x04};
      nouveau();
      rcim_ecrire_reference(&t, 0); rcim_ecrire_reference(&t, 1);
      rcim_ecrire_entier(&t, 0); rcim_ecrire_entier(&t, 2); rcim_ecrire_reel(&t, 0.25f);
      rcim_ecrire_appel(&t, RCIM_APPEL_INSTRUCTION, 4);
      VERIFIER(conforme("V12", v, sizeof v, 6)); }

    /* V13 — 5 TABLEAU MAT[10,5];MAT[I,J]←0 */
    { static const unsigned char v[] = {0x2D, 0x28, 0xA9,0x00,0x00,0x02, 0x40,0x01,0x00,
                                        0x40,0x02,0x00, 0x41,0x00,0x00,0x02, 0x23, 0x99};
      nouveau();
      rcim_ecrire_entier(&t, 10); rcim_ecrire_entier(&t, 5); rcim_ecrire_tableau(&t, 0, 2);
      rcim_ecrire_reference(&t, 1); rcim_ecrire_reference(&t, 2);
      rcim_ecrire_reference_indexee(&t, 0, 2);
      rcim_ecrire_entier(&t, 0); rcim_ecrire_tag(&t, RCIM_OP_AFFECTER);
      VERIFIER(conforme("V13", v, sizeof v, 8)); }

    /* V14 — 1 * CALCUL DE L'AIRE */
    { static const unsigned char v[] = {0xCC,0x11,0x20,0x43,0x41,0x4C,0x43,0x55,0x4C,0x20,0x44,
                                        0x45,0x20,0x4C,0x27,0x41,0x49,0x52,0x45};
      nouveau();
      rcim_ecrire_commentaire(&t, TXT(" CALCUL DE L'AIRE"), 17);
      VERIFIER(conforme("V14", v, sizeof v, 1)); }

    /* V17 — 105 FAIRE 105 TANT QUE A<4 (LSE F4, IMP-0277) */
    { static const unsigned char v[] = {0x00,0x69, 0xD2, 0x40,0x00,0x00, 0x27, 0x91, 0xD0};
      nouveau();
      rcim_ecrire_entier(&t, 105);
      rcim_ecrire_tag(&t, RCIM_CTL_FAIRE_OUVRIR_SEUL);
      rcim_ecrire_reference(&t, 0); rcim_ecrire_entier(&t, 4);
      rcim_ecrire_tag(&t, RCIM_OP_INF);
      rcim_ecrire_tag(&t, RCIM_CTL_FAIRE_TANT_QUE);
      VERIFIER(conforme("V17", v, sizeof v, 6)); }

    /* V15 — T1600 : 10 ALLER EN @+3 ; MITRA 15 : 10 I←@50 */
    { static const unsigned char v1[] = {0x42, 0x26, 0x80, 0xC3};
      static const unsigned char v2[] = {0x40,0x00,0x00, 0x06,0x32, 0x99};
      nouveau();
      rcim_ecrire_tag(&t, RCIM_REF_LIGNE_COURANTE); rcim_ecrire_entier(&t, 3);
      rcim_ecrire_tag(&t, RCIM_OP_ADD); rcim_ecrire_tag(&t, RCIM_CTL_ALLER_EN_CALCULE);
      VERIFIER(conforme("V15 T1600", v1, sizeof v1, 4));
      nouveau();
      rcim_ecrire_reference(&t, 0); rcim_ecrire_tag_u8(&t, RCIM_VAL_LIGNE, 50);
      rcim_ecrire_tag(&t, RCIM_OP_AFFECTER);
      VERIFIER(conforme("V15 MITRA 15", v2, sizeof v2, 3)); }

    /* V16 — 5 X←A*(-1) ; 5 X←-200 */
    { static const unsigned char v1[] = {0x40,0x00,0x00, 0x40,0x01,0x00, 0x22, 0x83, 0x99};
      static const unsigned char v2[] = {0x40,0x00,0x00, 0x01,0x38,0xFF, 0x99};
      nouveau();
      rcim_ecrire_reference(&t, 0); rcim_ecrire_reference(&t, 1); rcim_ecrire_entier(&t, -1);
      rcim_ecrire_tag(&t, RCIM_OP_MUL); rcim_ecrire_tag(&t, RCIM_OP_AFFECTER);
      VERIFIER(conforme("V16 A*(-1)", v1, sizeof v1, 5));
      nouveau();
      rcim_ecrire_reference(&t, 0); rcim_ecrire_entier(&t, -200); rcim_ecrire_tag(&t, RCIM_OP_AFFECTER);
      VERIFIER(conforme("V16 -200", v2, sizeof v2, 3)); }

    /* Sous-format des constantes entières (fiche §2) */
    nouveau();
    rcim_ecrire_entier(&t, -28); rcim_ecrire_entier(&t, 28);
    rcim_ecrire_entier(&t, -29); rcim_ecrire_entier(&t, 127);
    rcim_ecrire_entier(&t, 128); rcim_ecrire_entier(&t, -32768L);
    rcim_ecrire_entier(&t, 32768L);
    VERIFIER_EGAL(0x07, octets[0]);
    VERIFIER_EGAL(0x3F, octets[1]);
    VERIFIER_EGAL(RCIM_VAL_ENTIER_COURT, octets[2]);
    VERIFIER_EGAL(RCIM_VAL_ENTIER_COURT, octets[4]);
    VERIFIER_EGAL(RCIM_VAL_ENTIER, octets[6]);
    VERIFIER_EGAL(RCIM_VAL_ENTIER, octets[9]);
    VERIFIER_EGAL(RCIM_VAL_REEL, octets[12]);
    VERIFIER_EGAL(-32768L, rcim_lire_s16(octets + 10));
    VERIFIER(rcim_lire_reel(octets + 13) == 32768.0f);

    /* Description étendue (répétition 64 à 99) et description U (fiche §6) */
    nouveau();
    rcim_ecrire_description(&t, RCIM_DESC_F, 99, 4, 1);
    rcim_ecrire_description(&t, RCIM_DESC_U, 1, 0, 0);
    VERIFIER_EGAL(7, t.curseur);
    VERIFIER_EGAL(0x3F, octets[1]);
    VERIFIER_EGAL(99, octets[2]);
    VERIFIER_EGAL(0x80, octets[6]);
    VERIFIER(rcim_sequence_valide(octets, t.curseur, &nb) && nb == 2);

    /* Chaîne par la table des constantes (IMP-0128) */
    nouveau();
    rcim_ecrire_chaine_index(&t, 0x0102u);
    VERIFIER_EGAL(4, t.curseur);
    VERIFIER_EGAL(0x80, octets[1]);
    VERIFIER_EGAL(0x0102u, rcim_lire_u16(octets + 2));
    VERIFIER(rcim_sequence_valide(octets, t.curseur, NULL));

    /* Contrôle de place (IMP-0165) : rien n'est écrit si l'élément ne tient pas */
    rcim_tampon_initialiser(&t, octets, 4);
    VERIFIER(rcim_ecrire_reference(&t, 7));
    VERIFIER(!rcim_ecrire_reference(&t, 8));
    VERIFIER_EGAL(3, t.curseur);
    VERIFIER(rcim_ecrire_tag(&t, RCIM_OP_ADD));
    VERIFIER(!rcim_ecrire_tag(&t, RCIM_OP_ADD));
    VERIFIER_EGAL(4, t.curseur);

    /* Parcours (RCI-M §9) sur V4 : étiquettes successives */
    { static const unsigned char v[] = {0x40,0x00,0x00, 0x23, 0x90, 0xC0,0x0E, 0x40,0x01,0x00,
                                        0x24, 0x99, 0xC1,0x13, 0x40,0x01,0x00, 0x25, 0x99};
      static const unsigned char tags[] = {0x40, 0x23, 0x90, 0xC0, 0x40, 0x24, 0x99, 0xC1, 0x40, 0x25, 0x99};
      unsigned int k = 0;
      int ok = 1;
      premier_element(&c, v, sizeof v);
      while (!fin_sequence(&c)) {
          if (k >= sizeof tags || lire_tag(&c) != tags[k]) ok = 0;
          k++;
          element_suivant(&c);
      }
      VERIFIER(ok);
      VERIFIER_EGAL(sizeof tags, k);
      VERIFIER_EGAL(sizeof v, c.position); }

    /* Étiquette réservée : séquence invalide, parcours arrêté */
    { static const unsigned char v[] = {0x23, 0xD3, 0x23};
      VERIFIER(!rcim_sequence_valide(v, sizeof v, NULL));
      premier_element(&c, v, sizeof v);
      element_suivant(&c);
      element_suivant(&c);
      VERIFIER(fin_sequence(&c)); }

    /* Élément tronqué : invalide */
    { static const unsigned char v[] = {0x40, 0x00};
      VERIFIER(!rcim_sequence_valide(v, sizeof v, NULL)); }

    /* Identifiants et index dans les bornes des tables (restauration d'un fichier-
       programme, IMP-0246 ; audit du code, AC-17) : PROCEDURE &2 de paramètre 1 et
       de LOCAL 3, référence 0, chaîne par l'index 1 */
    { static const unsigned char v[] = {0xCD, 0x02,0x00, 0x01, 0x01,0x00, 0x01, 0x03,0x00,
                                        0x40, 0x00,0x00, 0x03, 0x80, 0x01,0x00, 0x99};
      VERIFIER(rcim_sequence_conforme(v, sizeof v, 4, 2));
      VERIFIER(!rcim_sequence_conforme(v, sizeof v, 3, 2));    /* LOCAL 3 hors de la table */
      VERIFIER(!rcim_sequence_conforme(v, sizeof v, 2, 2));    /* procédure 2 hors de la table */
      VERIFIER(!rcim_sequence_conforme(v, sizeof v, 4, 1));    /* index 1 hors de la table */
      VERIFIER(!rcim_sequence_conforme(v, 5, 4, 2)); }         /* élément tronqué */

    /* Réels du T1600 : 23 bits significatifs, demie en s'éloignant de zéro
       (IMP-0280) ; les valeurs attendues sont des multiples entiers de l'unité
       du dernier rang gardé, 2^(e-22) pour 2^e <= |z| < 2^(e+1). */
    { long double z;
      float f;
      z = 8.2L; rcim_arrondir_23(&z);
      VERIFIER(z * 524288.0L == 4299162.0L);                 /* 8.2 x 2^19 = 4299161,6 */
      z = 3.6457513110645906L; rcim_arrondir_23(&z);       /* 1 + RAC(7) (NBOR) */
      VERIFIER(z * 2097152.0L == 7645695.0L);                 /* x 2^21 = 7645694,65 */
      z = 1.0L / 3.0L; rcim_arrondir_23(&z);
      VERIFIER(z * 16777216.0L == 5592405.0L);
      z = 8388607.0L; rcim_arrondir_23(&z);                 /* 2^23 - 1 : exact */
      VERIFIER(z == 8388607.0L);
      z = 8388609.0L; rcim_arrondir_23(&z);                 /* demie : en s'éloignant de zéro */
      VERIFIER(z == 8388610.0L);
      z = -8388609.0L; rcim_arrondir_23(&z);
      VERIFIER(z == -8388610.0L);
      z = 8388611.0L; rcim_arrondir_23(&z);
      VERIFIER(z == 8388612.0L);
      z = 8388610.4L; rcim_arrondir_23(&z);                 /* sous la demie */
      VERIFIER(z == 8388610.0L);
      z = 16777215.0L; rcim_arrondir_23(&z);                /* retenue : exposant suivant */
      VERIFIER(z == 16777216.0L);
      z = 0.0L; rcim_arrondir_23(&z);
      VERIFIER(z == 0.0L);
      z = FLT_MAX; rcim_arrondir_23(&z);                    /* (2^24 - 1) x 2^104 : 2^128 */
      VERIFIER(z > FLT_MAX);
      z = (long double) FLT_MAX - 2.0L * 20282409603651670423947251286016.0L;   /* - 2^105 */
      rcim_arrondir_23(&z);
      VERIFIER(z < FLT_MAX);                                /* (2^23 - 1) x 2^105 */
      f = (float) z;
      VERIFIER(f < FLT_MAX); }

    VERIF_FIN();
}
