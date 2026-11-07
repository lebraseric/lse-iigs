/* pile.h — pile d'évaluation
   EXI: EXI-0011, EXI-0037  VM: VM-1001, VM-1002
   IV: IV-FMT-0010
   IMP: IMP-0144 (deux formes d'élément), IMP-0145 (tableau realloc),
        IMP-0208 (nature rendue, usage de lecture)
   Tests: tests/composants/composants_contexte.c
   Spécification : chapitre État d'exécution §4. */

#ifndef LSE_PILE_H
#define LSE_PILE_H

#include "../../src/noyau/objets.h"
#include "../../src/noyau/erreurs.h"

typedef unsigned char TypeElementPile;
#define PILE_NUMERIQUE   0
#define PILE_BOOLEEN     1
#define PILE_CHAINE      2
#define PILE_REFERENCE   3

#define REF_SIMPLE     0   /* variable : case désignée */
#define REF_ELEMENT    1   /* élément de tableau : case du tableau et rang */
#define REF_PROCEDURE  2   /* nom de procédure : identifiant de symbole */

typedef struct {
    unsigned short cellule;    /* case de l'environnement (alias traversé, §5.3, D1), ou
                                  identifiant de symbole pour REF_PROCEDURE */
    unsigned char  indexee;    /* REF_SIMPLE, REF_ELEMENT ou REF_PROCEDURE */
    unsigned int   decalage;   /* rang de l'élément, calculé par le module donnees */
} ReferenceResolue;

typedef struct {
    HandleObjet    objet;
    unsigned char  decalage;     /* position du texte dans l'objet */
    unsigned int   longueur;
    unsigned char  transitoire;  /* 1 : objet propre à l'élément, libéré à son retrait */
} ChaineSurPile;

typedef struct {
    TypeElementPile type;
    union {
        float            numerique;
        unsigned char    booleen;
        ChaineSurPile    chaine;
        ReferenceResolue reference;
    } contenu;
} ElementPile;

typedef struct {
    ElementPile  *elements;
    unsigned int  sommet;     /* nombre d'éléments */
    unsigned int  capacite;
} PileEvaluation;

/* Usage d'une lecture de valeur (IMP-0208) : choisit la nature d'erreur. */
typedef unsigned char UsageLecture;
#define USAGE_CALCUL       0   /* opérande d'un calcul : EX 05 */
#define USAGE_AFFECTATION  1   /* affectation ou affichage : EX 62 */
#define USAGE_COMPARAISON  2   /* membre d'une comparaison : EX 10 */

void        pile_initialiser(PileEvaluation *p);
/* Libère les chaînes transitoires restantes et le tableau. */
void        pile_liberer(PileEvaluation *p);
NatureErreur pile_empiler(PileEvaluation *p, const ElementPile *e);
NatureErreur pile_empiler_nombre(PileEvaluation *p, float v);
/* Même effet, nombre passé par adresse, sans conversion SANE (IMP-0244). */
NatureErreur pile_empiler_nombre_exact(PileEvaluation *p, const float *v);
NatureErreur pile_empiler_reference(PileEvaluation *p, const ReferenceResolue *r);
/* Copie de l'élément à la profondeur donnée (0 : sommet) ; 0 si absent. */
int         pile_lire(const PileEvaluation *p, unsigned int profondeur, ElementPile *copie);
/* Retire le sommet ; l'appelant devient propriétaire d'une chaîne transitoire. */
NatureErreur pile_retirer(PileEvaluation *p, ElementPile *e);
/* Retire n éléments, chaînes transitoires libérées. */
void        pile_consommer(PileEvaluation *p, unsigned int n);
/* Ramène la pile à la hauteur donnée (§7.3), chaînes transitoires libérées. */
void        pile_ramener(PileEvaluation *p, unsigned int hauteur);
/* Valeur numérique à la profondeur donnée, déréférencée si besoin (§4.1). */
NatureErreur pile_valeur_numerique(const PileEvaluation *p, unsigned int profondeur,
                                   UsageLecture usage, float *v);

#endif
