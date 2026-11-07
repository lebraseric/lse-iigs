/* composants_compilateur_limites.c — compilateur : caractère étranger au
   langage, épuisement de la mémoire des objets
   EXI: EXI-0019, EXI-0051  VM: VM-3201  IV: IV-IF-0001
   IMP: IMP-0161, IMP-0162, IMP-0170, IMP-0191, IMP-0195
   Spécification des tests : chapitre Compilateur §19 ; Gestionnaire des
   erreurs §7 (C 01, C 57).
   Non couverts ici : C 107 (ligne trop complexe), inatteignable avec une
   ligne de 79 caractères au plus — la borne de 255 octets est vérifiée au
   niveau de rcim (representation_rcim) ; C 108 (65 535 entrées de table),
   trop long à atteindre dans une suite.
   Niveau : composants. */

#include "compilateur_outils.h"
#include "../../src/noyau/objets.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

#define NB_MAX_OCCUPANTS 600
static HandleObjet occupants[NB_MAX_OCCUPANTS];

int main(void)
{
    unsigned int nb = 0;
    unsigned int i;
    unsigned long taille;
    HandleObjet h;
    int tout_alloue;

    VERIF_DEBUT("composants/compilateur limites");

    /* --- C 01 : caractère étranger au jeu du langage (IMP-0191) --- */
    VERIFIER(rejet("5 X_%", ERR_C_CARACTERE_INTERDIT, 4));
    VERIFIER(accepte("5 SI A#2 ALORS B_1"));
    VERIFIER(rejet("X_{", ERR_C_CARACTERE_INTERDIT, 2));
    /* après une instruction complète (IMP-0191 point 4) */
    VERIFIER(rejet("X_1{", ERR_C_CARACTERE_INTERDIT, 3));
    VERIFIER(rejet("5 A_B%C", ERR_C_CARACTERE_INTERDIT, 5));     /* inventaire D-08 */
    VERIFIER(rejet("5 A_B x", ERR_C_MINUSCULE, 6));
    VERIFIER(rejet("5 A_B]", ERR_C_INSTRUCTION_INCORRECTE, 5));
    /* dans une chaîne ou un commentaire, tout caractère imprimable est permis */
    VERIFIER(accepte("5 X_'%{}'"));
    VERIFIER(accepte("5*100 % ET {}"));

    /* --- C 57 : plus de place pour l'objet compilé (IMP-0170, IMP-0191) ---
       On occupe la mémoire des objets, des plus gros objets aux plus petits,
       jusqu'à ce qu'aucun octet ne puisse plus être alloué. */
    symboles_vider();
    taille = GESTOBJ_SEUIL_HORS_NORME;
    tout_alloue = 0;
    while (nb < NB_MAX_OCCUPANTS && !tout_alloue) {
        h = objets_creer(taille);
        if (h != GESTOBJ_HANDLE_NUL)
            occupants[nb++] = h;
        else if (taille > 1)
            taille = taille / 2;
        else
            tout_alloue = 1;
    }
    VERIFIER(tout_alloue);
    VERIFIER(rejet("10 A_1", ERR_C_PLUS_DE_PLACE, 0));
    VERIFIER(rejet("10 *COMMENTAIRE", ERR_C_PLUS_DE_PLACE, 0));
    VERIFIER(rejet("?1+1", ERR_C_PLUS_DE_PLACE, 0));
    /* après libération, la même ligne est compilée */
    for (i = 0; i < nb; i++)
        objets_liberer(occupants[i]);
    VERIFIER(accepte("10 A_1"));
    VERIFIER(accepte("?1+1"));

    VERIF_FIN();
}
