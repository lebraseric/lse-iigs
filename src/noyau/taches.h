/* taches.h — gestionnaire des tâches (temps partagé, configuration à une
   console)
   EXI: EXI-0058, EXI-0059
   VM: VM-2105
   IV: IV-IF-0008 (SWAP_OUT), IV-IF-0009 (SWAP_IN)
   IMP: IMP-0130 (tâche unique, sans zone de swap), IMP-0269 (états,
        transitions, module)
   Tests: tests/composants/composants_taches.c, tests/composants/composants_moniteur.c
   Spécification : chapitre Gestionnaire des tâches et reprise. */

#ifndef LSE_TACHES_H
#define LSE_TACHES_H

/* États de la tâche (IMP-0269). */
#define TACHE_ABSENTE     0   /* console dormante */
#define TACHE_ACTIVE      1
#define TACHE_EN_ATTENTE  2   /* attente d'entrée (INRDP75 §3-1) */

#define TACHE_UNIQUE      1   /* numéro de la seule tâche, celle de la console 1 */

/* BONJOUR : la tâche est créée et active ; AU REVOIR : elle est détruite. */
void          taches_creer(void);
void          taches_detruire(void);

/* SUSPENDRE_TACHE et REACTIVER_TACHE (VM-2105) : une demande d'entrée met
   fin à l'activité (INRDP75 §3-1) ; sans effet si la tâche est absente. */
void          taches_suspendre(void);
void          taches_reactiver(void);

/* SWAP_OUT et SWAP_IN (IV-IF-0008, IV-IF-0009) : l'état de la tâche unique
   reste dans la zone données-utilisateur ; aucune copie (IMP-0130). */
void          taches_swap_out(unsigned char tache);
void          taches_swap_in(unsigned char tache);

/* Tâche active (0 : aucune) et état. */
unsigned char taches_active(void);
unsigned char taches_etat(void);
/* Nombre de SWAP OUT et de SWAP IN depuis la création (tests). */
unsigned long taches_swap_outs(void);
unsigned long taches_swap_ins(void);

#endif
