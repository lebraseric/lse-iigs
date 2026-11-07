/* essai_g4c.c — essai de l'audit du groupe 4 (copie d'audit seulement)
   Profondeur de pile d'une session complète du moniteur (DECODER, ENCODER,
   EXECUTER avec GARER dans une procédure, CATALOGUER), mesurée depuis main. */

#include <string.h>
#include "../commun/verif.h"
#include "../../src/noyau/moniteur.h"
#include "../../src/noyau/programme.h"
#include "../../src/interfaces/types_communs.h"
#include "../../src/interfaces/interface_fichiers.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

segment "suite";

extern char bouchon_affichage[];
void bouchon_console(const char *const *lignes);

#define X "\x13"
#define PEINTURE 3200u
static unsigned int sp_peinture;

static void peindre(void)
{
    unsigned int sp;
    unsigned int a;
    asm {
        tsc
        sta sp
    }
    sp_peinture = sp;
    for (a = sp - PEINTURE; a < sp - 32u; a++)
        *(unsigned char *) (unsigned long) a = 0xA5;
}

static unsigned int profondeur(void)
{
    unsigned int a;
    for (a = sp_peinture - PEINTURE; a < sp_peinture - 32u; a++)
        if (*(unsigned char *) (unsigned long) a != 0xA5) break;
    return sp_peinture - a;
}

int main(void)
{
    static const char *const s1[] = { "BO" X, "ID" X, "1" X, "" X,
        "10 X_1;&P(3)" X, "20 TERMINER" X, "30 PROCEDURE &P(N) LOCAL N" X,
        "40 GARER N,1,'TG';CHARGER Y,1,'TG';RETOUR" X,
        "EX" X, "10" X, "DE" X, "XX" X, "EN" X, "XX" X, "CA" X, "TG,PG" X, "AU" X, NULL };
    static const char *const s2[] = { "BO" X, "ID" X, "1" X, "" X, "10 X_1" X, "AU" X, NULL };
    unsigned int sp;

    asm {
        tsc
        sta sp
    }
    VERIF_DEBUT("essai_g4c");
    printf("  SP a l'entree de main : $%04X\n", sp);
    programme_initialiser();
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:ESSAIG4C"));
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_espace_vider(ESPACE_DONNEES);
    plateforme_espace_vider(ESPACE_TEMPORAIRES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");

    bouchon_console(s2);
    peindre();
    moniteur_session();
    printf("  session simple : %u octets sous main\n", profondeur() + (sp - sp_peinture));

    bouchon_console(s1);
    peindre();
    moniteur_session();
    printf("  session DECODER/ENCODER/EXECUTER/CATALOGUER : %u octets sous main\n",
           profondeur() + (sp - sp_peinture));
    printf("%s\n", bouchon_affichage);

    {
        static const char *const s3[] = { "BO" X, "ID" X, "1" X, "" X,
            "10 FAIRE 20 POUR I_1 JUSQUA 3" X, "20 " X, "30 TERMINER" X,
            "LI" X, "1" X, "RA" X, "VIDE" X, "AP" X, "VIDE" X, "LI" X, "1" X, "AU" X, NULL };
        bouchon_console(s3);
        moniteur_session();
        printf("--- session ligne vide ---\n%s\n", bouchon_affichage);
        VERIFIER(strstr(bouchon_affichage, "APPELER VIDE\r\nERREUR") == NULL);
    }
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_espace_vider(ESPACE_DONNEES);
    plateforme_espace_vider(ESPACE_TEMPORAIRES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
    VERIF_FIN();
}
