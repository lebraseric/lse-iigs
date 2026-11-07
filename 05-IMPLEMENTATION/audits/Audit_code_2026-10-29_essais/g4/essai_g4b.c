/* essai_g4b.c — essai de l'audit du groupe 4 (copie d'audit seulement)
   RANGER d'un programme dont l'image RCI-S dépasse 65 535 octets : la somme
   de contrôle d'image_rcis est calculée avec un indice unsigned int.
   Les constantes partagées s'accumulent au fil des remplacements d'une même
   ligne (aucune n'est retirée avant programme_vider). */

#include "compilateur_outils.h"
#ifndef NB_CONSTANTES
#define NB_CONSTANTES 1200
#endif
#include "../../src/noyau/programme.h"
#include "../../src/noyau/contexte_exec.h"
#include "../../src/noyau/bibliotheque.h"
#include "../../src/interfaces/interface_fichiers.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

void bouchon_console(const char *const *lignes);
static const char *const aucune[] = { NULL };

int main(void)
{
    static char ligne[90];
    unsigned int k;
    unsigned int j;
    NatureErreur n;

    VERIF_DEBUT("essai_g4b");
    programme_initialiser();
    bouchon_console(aucune);
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:ESSAIG4B"));
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
    {
        Identification id;
        bibliotheque_identification_chercher(1, &id);
        bibliotheque_identifier(&id);
    }
    /* La ligne 10 est retapée 1200 fois avec un message différent de 40 à
       72 caractères : 1200 constantes partagées (IMP-0107), une seule ligne. */
    for (k = 0; k < NB_CONSTANTES; k++) {
        unsigned int lgc = 40 + k % 33;
        strcpy(ligne, "10 C_'");
        sprintf(ligne + 6, "%04u", k);
        for (j = 4; j < lgc; j++) ligne[6 + j] = (char) ('A' + (k + j) % 26);
        ligne[6 + lgc] = '\'';
        ligne[7 + lgc] = '\0';
        if (!compiler(ligne)) { printf("  rejet %u : C %u\n", k, erreurs_numero(d.nature)); break; }
        if (!programme_inserer(&r.resultat.ligne_programme, &n)) { printf("  insertion %u\n", k); break; }
    }
    printf("  lignes %u, constantes %u\n", programme_nombre(), constantes_nombre());
    printf("  RANGER...\n");
    fflush(stdout);
    n = bibliotheque_ranger("GROS", 0, 0);
    printf("  RANGER rendu : %u\n", n);
    VERIFIER_EGAL(ERR_AUCUNE, n);
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
    VERIF_FIN();
}
