/* taches.c — gestionnaire des tâches (temps partagé, configuration à une
   console)
   EXI: EXI-0058, EXI-0059
   VM: VM-2105
   IV: IV-IF-0008 (SWAP_OUT), IV-IF-0009 (SWAP_IN)
   IMP: IMP-0130 (tâche unique, sans zone de swap), IMP-0269 (états,
        transitions, module)
   Tests: tests/composants/composants_taches.c, tests/composants/composants_moniteur.c
   Spécification : chapitre Gestionnaire des tâches et reprise. Segment
   "fichiers" (IMP-0269). */

#pragma noroot

#include "taches.h"

segment "fichiers";

static unsigned char etat = TACHE_ABSENTE;
static unsigned long nb_out = 0;
static unsigned long nb_in = 0;

void taches_creer(void)
{
    etat = TACHE_ACTIVE;
    nb_out = 0;
    nb_in = 0;
}

void taches_detruire(void)
{
    etat = TACHE_ABSENTE;
}

void taches_swap_out(unsigned char tache)
{
    if (tache == TACHE_UNIQUE) nb_out++;
}

void taches_swap_in(unsigned char tache)
{
    if (tache == TACHE_UNIQUE) nb_in++;
}

void taches_suspendre(void)
{
    if (etat != TACHE_ACTIVE) return;
    taches_swap_out(TACHE_UNIQUE);       /* ALG-1502 : l'état reste en place */
    etat = TACHE_EN_ATTENTE;
}

void taches_reactiver(void)
{
    if (etat != TACHE_EN_ATTENTE) return;
    taches_swap_in(TACHE_UNIQUE);        /* ALG-1503 : seule tâche éligible */
    etat = TACHE_ACTIVE;
}

unsigned char taches_active(void)
{
    return etat == TACHE_ACTIVE ? TACHE_UNIQUE : 0;
}

unsigned char taches_etat(void)
{
    return etat;
}

unsigned long taches_swap_outs(void)
{
    return nb_out;
}

unsigned long taches_swap_ins(void)
{
    return nb_in;
}
