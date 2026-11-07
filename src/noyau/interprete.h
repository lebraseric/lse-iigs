/* interprete.h — interpréteur : cycle d'exécution d'une ligne de programme ou
   de machine de bureau
   EXI: EXI-0011, EXI-0022 à EXI-0030, EXI-0037
   VM: VM-2205, VM-2206, VM-3101, VM-3102
   IV: IV-IF-0003, ALG-1102
   IMP: IMP-0214 (interface), IMP-0215 à IMP-0219 ; Moniteur IMP-0232,
        IMP-0237, IMP-0239
   Tests: tests/composants/composants_interprete.c
   Spécification : chapitre Interpréteur (tome-v/Interpreteur_LSE-IIGS.md). */

#ifndef LSE_INTERPRETE_H
#define LSE_INTERPRETE_H

#include "../../src/noyau/contexte_exec.h"

typedef unsigned char FinExecution;
#define FIN_TERMINER  0   /* instruction TERMINER */
#define FIN_PAUSE     1   /* instruction PAUSE ; pos désigne l'élément suivant */
#define FIN_ARRET     2   /* ligne d'arrêt atteinte, non exécutée */
#define FIN_ERREUR    3   /* *diagnostic renseigné ; pos désigne l'élément en cause */
#define FIN_BUREAU    4   /* fin de la ligne de machine de bureau */
#define FIN_PAS       5   /* pas à pas : début de ligne atteint, non exécuté (Moniteur IMP-0232) */
#define FIN_INTERRUPTION 6 /* ESC vue au début d'une ligne (Moniteur IMP-0239) */
#define FIN_LECTURE   7   /* LIRE attend une ligne ; pos désigne l'élément de lecture
                             (Moniteur IMP-0237) */
#define FIN_EXECUTER  8   /* EXECUTER : module chargé, pos en tête de la ligne de
                             départ ; le moniteur relance l'exécution (point B5) */

/* Exécution du programme courant depuis pos (IV-IF-0003). */
FinExecution interprete_executer(PileEvaluation *p, PositionExecution *pos,
                                 DiagnosticExecution *diagnostic);
/* Exécution d'une ligne de machine de bureau, dans le cadre courant, depuis
   pos->decalage (0 au départ ; position rendue après FIN_LECTURE). */
FinExecution interprete_bureau(PileEvaluation *p, HandleObjet bloc, unsigned int longueur,
                               PositionExecution *pos, DiagnosticExecution *diagnostic);
/* Ligne tapée en réponse à LIRE, avec sa touche de fin (SAISIE_*), à fournir
   après FIN_LECTURE avant de relancer l'exécution (Moniteur IMP-0237). */
void         interprete_fournir_saisie(const char *ligne, unsigned int n, unsigned char touche_fin);
/* Mode pas à pas (Moniteur IMP-0232). */
void         interprete_pas_a_pas(int vrai);
/* Oublie la lecture en cours, la ligne déjà signalée et la description
   d'AFFICHER en cours : nouvelle exécution (EXECUTER, REPRENDRE). */
void         interprete_reinitialiser(void);
/* Ligne de machine de bureau (IMP-0222, IMP-0233) : met de côté l'état de
   lecture du programme et part d'un état vierge ; puis le rétablit, quelle
   que soit la fin de la ligne. */
void         interprete_lecture_mettre_de_cote(void);
void         interprete_lecture_retablir(void);
/* Fixe l'état « sortie en début de ligne » (IMP-0221), par exemple après un
   message du moniteur terminé par un passage à la ligne. */
void         interprete_debut_de_ligne(int vrai);
int          interprete_en_debut_de_ligne(void);

#endif
