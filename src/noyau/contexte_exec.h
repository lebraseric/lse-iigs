/* contexte_exec.h — environnement des identificateurs, cadres d'activation,
   contextes de boucle FAIRE, position d'exécution
   EXI: EXI-0022 à EXI-0024, EXI-0029 à EXI-0039, EXI-0042 à EXI-0044
   VM: VM-0801 à VM-0803, VM-0901 à VM-0903, VM-1102, VM-1103, VM-1302,
       VM-1303, VM-3006, VM-3007
   IV: IV-FMT-0010, ALG-1101, ALG-1201 à ALG-1203
   IMP: IMP-0146, IMP-0148 à IMP-0155, IMP-0179, IMP-0208
   Tests: tests/composants/composants_contexte.c
   Spécification : chapitre État d'exécution §3 bis, §5, §6, §7.1. */

#ifndef LSE_CONTEXTE_EXEC_H
#define LSE_CONTEXTE_EXEC_H

#include "../../src/noyau/pile.h"

/* --- environnement des identificateurs (§3 bis, IMP-0179, D1) ---
   L'environnement associe à chaque nom une case ; une case porte l'état et
   le contenu d'une variable. Un identificateur LOCAL reçoit une case neuve
   pour la durée d'un appel ; un alias, une référence de la pile et la
   variable contrôlée d'une boucle désignent une case. */
typedef unsigned char EtatIdentificateur;
#define ID_NON_DEFINI             0
#define ID_VARIABLE               1
#define ID_CHAINE                 2
#define ID_TABLEAU                3
#define ID_PARAMETRE_SANS_VALEUR  4   /* paramètre LOCAL reçu sans valeur : EX 14 à la lecture */

typedef struct {
    EtatIdentificateur etat;
    union {
        float       valeur;
        HandleObjet objet;     /* GESTOBJ_HANDLE_NUL : variable-chaîne sans valeur */
    } contenu;
} EntreeEnvironnement;         /* une case : 5 octets */

/* Position d'exécution : ligne 0 = machine de bureau ; décalage dans la séquence. */
typedef struct {
    unsigned char ligne;
    unsigned int  decalage;
} PositionExecution;

/* --- environnement --- */
/* Case désignée par l'identificateur id dans le cadre courant (§5.3, D1). */
NatureErreur contexte_resoudre(unsigned int id, ReferenceResolue *r);
NatureErreur contexte_entree(unsigned int cellule, EntreeEnvironnement *copie);
NatureErreur contexte_lire_nombre(unsigned int cellule, UsageLecture usage, float *v);
NatureErreur contexte_lire_chaine(unsigned int cellule, UsageLecture usage, ChaineSurPile *c);
NatureErreur contexte_affecter_nombre(unsigned int cellule, float v);
/* Par adresse : aucune conversion au format étendu de SANE (IMP-0244 ;
   audit du code, AP-07). */
NatureErreur contexte_affecter_nombre_exact(unsigned int cellule, const float *v);
/* Prend possession de objet en cas de succès ; sinon il reste à l'appelant. */
NatureErreur contexte_affecter_chaine(unsigned int cellule, HandleObjet objet);
/* Déclarations : id est l'identificateur écrit, non résolu.
   Prend possession de tableau en cas de succès. */
NatureErreur contexte_declarer_tableau(unsigned int id, HandleObjet tableau);
NatureErreur contexte_declarer_chaine(unsigned int id);
NatureErreur contexte_liberer(unsigned int id);
/* CHARGER (Bibliothécaire et fichiers IMP-0253) : déclaration implicite de la
   case désignée, puis valeur ; EX 47 si la case porte déjà une quantité
   d'un autre type (LSE15 §6-3.4). Prend possession de l'objet en cas de
   succès. */
NatureErreur contexte_charger_nombre(unsigned int cellule, const float *v);
NatureErreur contexte_charger_chaine(unsigned int cellule, HandleObjet objet);
NatureErreur contexte_charger_tableau(unsigned int cellule, HandleObjet tableau);

/* --- exécution (§7.1) --- */
/* EXECUTER {A PARTIR DE depart{,arret}} ; 0 : paramètre absent. */
NatureErreur contexte_initialiser(unsigned char depart, unsigned char arret,
                                  PositionExecution *pos);
unsigned char contexte_ligne_arret(void);
/* Nouvelle ligne d'arrêt (POURSUIVRE JUSQU'EN n ; 0 : aucune). */
void         contexte_fixer_arret(unsigned char arret);

/* Description en cours d'un AFFICHER (Interpréteur IMP-0221), rangée dans le cadre
   de l'appelant pendant un appel de procédure (IMP-0146) ; opaque pour le contexte. */
typedef struct {
    unsigned char type;
    unsigned char e;
    unsigned char d;
    unsigned char reste;      /* valeurs encore couvertes par la description */
} DescriptionAffichage;
void         contexte_garder_affichage(const DescriptionAffichage *a);   /* cadre courant */
void         contexte_rendre_affichage(DescriptionAffichage *a);
/* REPRENDRE EN depart{,arret} (Moniteur IMP-0232) : retour au niveau
   principal sans toucher aux identificateurs ; la pile est vidée par
   l'appelant. ERR_E_LIGNE_INEXISTANTE si depart est absent. */
NatureErreur contexte_reprendre(unsigned char depart, unsigned char arret,
                                PositionExecution *pos);
/* Tout libérer, environnement compris (EXECUTER suivant, effacement total). */
void         contexte_vider(void);
unsigned int contexte_profondeur(void);    /* 0 : cadre racine seul */
/* Ligne de retour du cadre courant ($, profil T1600) ; 0 au niveau principal. */
unsigned char contexte_ligne_retour(void);

/* --- procédures (§5.5) --- */
/* pos : en entrée, position qui suit l'élément APPEL ; en sortie, entrée de
   la procédure. La pile porte la référence à la procédure puis k arguments. */
NatureErreur contexte_appeler(PileEvaluation *p, unsigned char k, unsigned char position_appel,
                              PositionExecution *pos);
/* RETOUR (a_resultat = 0, cible = 0), RETOUR EN cible, RESULTAT (a_resultat = 1 :
   valeur au sommet). *abandon : 1 si l'expression appelante doit être
   abandonnée (§7.3). */
NatureErreur contexte_retourner(PileEvaluation *p, int a_resultat, unsigned char cible,
                                PositionExecution *pos, int *abandon);

/* --- boucles FAIRE (§6) --- */
/* pos : position qui suit l'élément d'ouverture (ligne FAIRE, décalage de reprise). */
NatureErreur contexte_ouvrir_boucle(PileEvaluation *p, unsigned int id_v,
                                    const PositionExecution *pos);
/* FAIRE n1 TANT QUE c, sans variable (IMP-0277). */
NatureErreur contexte_ouvrir_boucle_seule(PileEvaluation *p, const PositionExecution *pos);
NatureErreur contexte_pas(PileEvaluation *p, int explicite);
/* JUSQUA (tant_que = 0) ou TANT QUE ; *saut : 1 si la position a changé. */
NatureErreur contexte_tester(PileEvaluation *p, int tant_que, PositionExecution *pos, int *saut);
/* Séquence de la ligne épuisée. */
NatureErreur contexte_fin_de_ligne(PositionExecution *pos);
/* ALLER EN n : ligne existante, puis désimbrication. */
NatureErreur contexte_brancher(unsigned char n, PositionExecution *pos);
unsigned int contexte_nb_boucles(void);

#endif
