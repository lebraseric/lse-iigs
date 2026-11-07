/* types_plateforme.c — hypothèses de plateforme de RCI-M sous ORCA/C 2.2.1
   EXI: EXI-0016 (compacité de la représentation compilée)
   VM: —  IV: ANN-K-0032 (représentation physique propre à l'implémentation)
   IMP: RCI-M §2 (tailles des types, absence de bourrage), IMP-0184 (octet de
        poids faible en tête), IMP-0192 (forme des suites)
   Niveau : représentation (Organisation du dépôt §4, §6) */

#include <string.h>
#include "../commun/verif.h"
#include "../../src/interfaces/profil.h"
#include "../../src/interfaces/types_communs.h"

/* Instance propre à la suite (IMP-0196) : ORCA/C référence à l'édition de
   liens toute variable déclarée extern, même inemployée. */
ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

/* Structure sans bourrage attendu : 1 + 2 + 1 octets (RCI-M §2). */
typedef struct {
    unsigned char a;
    unsigned int b;
    unsigned char c;
} StructureEssai;

/* Copie les 4 premiers octets à l'adresse d'un paramètre float. */
static void octets_du_parametre(float x, unsigned char *sortie)
{
    memcpy(sortie, &x, 4);
}

int main(void)
{
    unsigned int entier16 = 0x1234u;
    unsigned long entier32 = 0x12345678ul;
    int negatif = -200;
    float un = 1.0f;
    unsigned char octets[4];

    VERIF_DEBUT("representation/types_plateforme");

    /* Tailles (RCI-M §2) */
    VERIFIER_EGAL(1, sizeof(char));
    VERIFIER_EGAL(2, sizeof(short));
    VERIFIER_EGAL(2, sizeof(int));
    VERIFIER_EGAL(4, sizeof(long));
    VERIFIER_EGAL(4, sizeof(void *));
    VERIFIER_EGAL(4, sizeof(float));
    VERIFIER_EGAL(8, sizeof(double));

    /* Absence de bourrage (RCI-M §2) */
    VERIFIER_EGAL(4, sizeof(StructureEssai));

    /* Octet de poids faible en tête (IMP-0184) */
    memcpy(octets, &entier16, 2);
    VERIFIER_EGAL(0x34, octets[0]);
    VERIFIER_EGAL(0x12, octets[1]);

    memcpy(octets, &entier32, 4);
    VERIFIER_EGAL(0x78, octets[0]);
    VERIFIER_EGAL(0x56, octets[1]);
    VERIFIER_EGAL(0x34, octets[2]);
    VERIFIER_EGAL(0x12, octets[3]);

    /* -200 sur 16 bits : 38 FF (vecteur du Compilateur §19, RCI-M §4.4) */
    memcpy(octets, &negatif, 2);
    VERIFIER_EGAL(0x38, octets[0]);
    VERIFIER_EGAL(0xFF, octets[1]);

    /* 1.0f en IEEE simple précision, octet faible en tête : 00 00 80 3F */
    memcpy(octets, &un, 4);
    VERIFIER_EGAL(0x00, octets[0]);
    VERIFIER_EGAL(0x00, octets[1]);
    VERIFIER_EGAL(0x80, octets[2]);
    VERIFIER_EGAL(0x3F, octets[3]);

    /* Un paramètre float est conservé au format extended (RCI-M §2) : ses
       quatre premiers octets ne sont pas ceux du float IEEE. Ce contrôle
       détecte un changement de ce comportement. */
    octets_du_parametre(1.0f, octets);
    VERIFIER(!(octets[0] == 0x00 && octets[1] == 0x00 && octets[2] == 0x80 && octets[3] == 0x3F));

    /* Valeurs des profils, rangées dans les fichiers-programmes (IMP-0276). */
    VERIFIER_EGAL(0, PROFIL_MITRA15_V8);
    VERIFIER_EGAL(1, PROFIL_T1600_V4);
    VERIFIER_EGAL(2, PROFIL_T1600_F4);
    VERIFIER(!PROFIL_EST_T1600(PROFIL_MITRA15_V8));
    VERIFIER(PROFIL_EST_T1600(PROFIL_T1600_V4));
    VERIFIER(PROFIL_EST_T1600(PROFIL_T1600_F4));

    VERIF_FIN();
}
