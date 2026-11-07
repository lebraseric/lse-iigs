/* plateforme_fichiers.c — réalisation de interface_fichiers.h
   IMP: IMP-0245, IMP-0249
   Niveau : plateforme, sous Golden Gate (fichiers de l'hôte) et sur cible.
   Bibliothèque de travail : tests:BIBTEST (sous build/, effacée par
   make clean). */

#include <string.h>
#include "../commun/verif.h"
#include "../../src/interfaces/interface_fichiers.h"

/* Rappel de plateforme_espace_parcourir : noms vus, mis bout à bout. */
static char vus[64];
static unsigned int nb_vus = 0;

static void voir(const char *nom, void *contexte)
{
    (void) contexte;
    if (strlen(vus) + strlen(nom) + 1 < sizeof vus) {
        strcat(vus, nom);
        strcat(vus, ",");
    }
    nb_vus++;
}

int main(void)
{
    FichierPlateforme f;
    unsigned char bloc[600];
    unsigned char lu[16];
    char nom[16];
    unsigned int i;

    VERIF_DEBUT("plateforme/fichiers");
    for (i = 0; i < sizeof bloc; i++) bloc[i] = (unsigned char) i;

    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBTEST"));
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBTEST"));   /* déjà créée */
    VERIFIER_EGAL(PLAT_OK, plateforme_espace_vider(ESPACE_TEMPORAIRES));
    VERIFIER_EGAL(PLAT_OK, plateforme_espace_vider(ESPACE_DONNEES));

    /* création, écriture, taille */
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_ouvrir(ESPACE_TEMPORAIRES, "FICH", FICHIER_CREATION, &f));
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_ecrire(f, 0, bloc, 600));
    VERIFIER(plateforme_fichier_taille(f) == 600ul);
    plateforme_fichier_fermer(f);

    /* mise à jour sur place, ajout en fin, relecture */
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_ouvrir(ESPACE_TEMPORAIRES, "FICH", FICHIER_MISE_A_JOUR, &f));
    memset(lu, 0xAA, sizeof lu);
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_ecrire(f, 256, lu, 4));
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_ecrire(f, 600, lu, 8));
    VERIFIER(plateforme_fichier_taille(f) == 608ul);
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_lire(f, 254, lu, 8));
    VERIFIER(lu[0] == 254 && lu[1] == 255 && lu[2] == 0xAA && lu[5] == 0xAA && lu[6] == 4);
    VERIFIER(plateforme_fichier_lire(f, 604, lu, 8) != PLAT_OK);     /* au-delà de la fin */
    plateforme_fichier_fermer(f);

    /* existence, liste, changement d'espace, suppression */
    VERIFIER(plateforme_fichier_existe(ESPACE_TEMPORAIRES, "FICH"));
    VERIFIER(!plateforme_fichier_existe(ESPACE_DONNEES, "FICH"));
    VERIFIER(plateforme_fichier_lister(ESPACE_TEMPORAIRES, 0, nom) && strcmp(nom, "FICH") == 0);
    VERIFIER(!plateforme_fichier_lister(ESPACE_TEMPORAIRES, 1, nom));
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_renommer(ESPACE_TEMPORAIRES, "FICH", ESPACE_DONNEES, "PERM"));
    VERIFIER(!plateforme_fichier_existe(ESPACE_TEMPORAIRES, "FICH"));
    VERIFIER(plateforme_fichier_existe(ESPACE_DONNEES, "PERM"));
    VERIFIER_EGAL(PLAT_INEXISTANT, plateforme_fichier_ouvrir(ESPACE_PROGRAMMES, "ABSENT", FICHIER_LECTURE, &f));
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_supprimer(ESPACE_DONNEES, "PERM"));
    VERIFIER_EGAL(PLAT_INEXISTANT, plateforme_fichier_supprimer(ESPACE_DONNEES, "PERM"));

    /* cause d'un échec demandée à GS/OS (IMP-0249 ; audit du code, AC-29) : un
       répertoire non vide tient lieu de fichier présent mais refusé, Golden Gate
       ne sachant pas verrouiller un fichier */
    VERIFIER_EGAL(PLAT_INEXISTANT, plateforme_fichier_ouvrir(ESPACE_PROGRAMMES, "ABSENT", FICHIER_MISE_A_JOUR, &f));
    plateforme_unite_designer("tests:BIBTEST:NULLEPART");               /* répertoire absent */
    VERIFIER_EGAL(PLAT_INEXISTANT, plateforme_fichier_ouvrir(ESPACE_UNITE_PROGRAMMES, "ABSENT", FICHIER_LECTURE, &f));
    plateforme_unite_designer(NULL);
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBTEST:DONNEES:REP"));
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBTEST"));
    VERIFIER_EGAL(PLAT_ERREUR, plateforme_fichier_ouvrir(ESPACE_DONNEES, "REP", FICHIER_MISE_A_JOUR, &f));
    VERIFIER_EGAL(PLAT_ERREUR, plateforme_fichier_supprimer(ESPACE_DONNEES, "REP"));
    if (plateforme_bibliotheque_ouvrir("tests:BIBTEST:DONNEES:REP") == PLAT_OK) {
        VERIFIER_EGAL(PLAT_OK, plateforme_espace_vider(ESPACE_RACINE));
    }
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:BIBTEST"));
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_supprimer(ESPACE_DONNEES, "REP"));

    /* vidage de l'espace temporaire (BONJOUR, AU REVOIR) */
    for (i = 0; i < 3; i++) {
        nom[0] = (char) ('A' + i);
        nom[1] = '\0';
        plateforme_fichier_ouvrir(ESPACE_TEMPORAIRES, nom, FICHIER_CREATION, &f);
        plateforme_fichier_fermer(f);
    }
    VERIFIER(plateforme_fichier_lister(ESPACE_TEMPORAIRES, 2, nom));
    /* parcours en une seule ouverture (audit du code, AP-06) : les trois noms */
    vus[0] = '\0';
    VERIFIER(plateforme_espace_parcourir(ESPACE_TEMPORAIRES, voir, NULL));
    VERIFIER(nb_vus == 3 && strstr(vus, "A,") != NULL && strstr(vus, "B,") != NULL
             && strstr(vus, "C,") != NULL);
    plateforme_unite_designer("tests:BIBTEST:NULLEPART");
    VERIFIER(!plateforme_espace_parcourir(ESPACE_UNITE_PROGRAMMES, voir, NULL));   /* répertoire absent */
    plateforme_unite_designer(NULL);
    VERIFIER_EGAL(PLAT_OK, plateforme_espace_vider(ESPACE_TEMPORAIRES));
    VERIFIER(!plateforme_fichier_lister(ESPACE_TEMPORAIRES, 0, nom));

    /* ruban ouvert en création : un fichier, jamais un répertoire (audit du code, AC-25) */
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_ouvrir(ESPACE_RUBAN, "RUBAN", FICHIER_CREATION, &f));
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_ecrire(f, 0, bloc, 4));
    plateforme_fichier_fermer(f);
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_ouvrir(ESPACE_RUBAN, "RUBAN", FICHIER_LECTURE, &f));
    VERIFIER(plateforme_fichier_taille(f) == 4ul);
    plateforme_fichier_fermer(f);
    VERIFIER_EGAL(PLAT_OK, plateforme_fichier_supprimer(ESPACE_RUBAN, "RUBAN"));

    VERIF_FIN();
}
