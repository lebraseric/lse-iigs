/* moniteur.h — environnement conversationnel : session, saisie, commandes
   EXI: EXI-0028, EXI-0029, EXI-0051, EXI-0052
   VM: VM-2201
   IV: IV-IF-0001, IV-IF-0002, IV-IF-0003, IV-IF-0004
   IMP: IMP-0227, IMP-0228, IMP-0231 à IMP-0239, IMP-0268
   Tests: tests/composants/composants_moniteur.c
   Spécification : chapitre Moniteur et console. */

#ifndef LSE_MONITEUR_H
#define LSE_MONITEUR_H

/* Session complète, de l'état dormant à AU REVOIR (IMP-0228). Le programme
   courant doit être initialisé (programme_initialiser). */
void moniteur_session(void);
/* Identification sous laquelle s'ouvre chaque session (option -ID ; 0 par
   défaut ; ignorée si elle n'est pas définie à BONJOUR ; IMP-0268). */
void moniteur_fixer_identification(unsigned int numero);
/* Option de lancement : sous LSE F4, demander la date avant la session (IMP-0279). */
void moniteur_fixer_demarrage(int actif);

#endif
