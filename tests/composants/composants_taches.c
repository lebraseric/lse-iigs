/* composants_taches.c — gestionnaire des tâches : états et transitions
   EXI: EXI-0058, EXI-0059  VM: VM-2105  IV: IV-IF-0008, IV-IF-0009
   IMP: IMP-0130, IMP-0269 ; points T2, T3
   Spécification des tests : chapitre Gestionnaire des tâches et reprise §4.
   Niveau : composants. */

#include "../commun/verif.h"
#include "../../src/noyau/taches.h"
#include "../../src/interfaces/types_communs.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

int main(void)
{
    VERIF_DEBUT("composants/taches");

    /* console dormante : aucune tâche, suspendre et réactiver sans effet */
    VERIFIER_EGAL(TACHE_ABSENTE, taches_etat());
    VERIFIER_EGAL(0, taches_active());
    taches_suspendre();
    taches_reactiver();
    VERIFIER_EGAL(TACHE_ABSENTE, taches_etat());

    /* BONJOUR : tâche 1 active */
    taches_creer();
    VERIFIER_EGAL(TACHE_ACTIVE, taches_etat());
    VERIFIER_EGAL(TACHE_UNIQUE, taches_active());
    VERIFIER_EGAL(0ul, taches_swap_outs());

    /* demande d'entrée : SWAP OUT, attente ; ligne reçue : SWAP IN, active */
    taches_suspendre();
    VERIFIER_EGAL(TACHE_EN_ATTENTE, taches_etat());
    VERIFIER_EGAL(0, taches_active());
    VERIFIER_EGAL(1ul, taches_swap_outs());
    taches_suspendre();                                  /* déjà en attente */
    VERIFIER_EGAL(1ul, taches_swap_outs());
    taches_reactiver();
    VERIFIER_EGAL(TACHE_ACTIVE, taches_etat());
    VERIFIER_EGAL(1ul, taches_swap_ins());
    taches_reactiver();                                  /* déjà active */
    VERIFIER_EGAL(1ul, taches_swap_ins());

    /* une autre tâche que la tâche unique n'existe pas */
    taches_swap_out(2);
    taches_swap_in(2);
    VERIFIER_EGAL(1ul, taches_swap_outs());
    VERIFIER_EGAL(1ul, taches_swap_ins());

    /* AU REVOIR */
    taches_detruire();
    VERIFIER_EGAL(TACHE_ABSENTE, taches_etat());

    VERIF_FIN();
}
