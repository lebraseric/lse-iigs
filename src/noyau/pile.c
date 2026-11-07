/* pile.c — pile d'évaluation
   EXI: EXI-0011, EXI-0037  VM: VM-1001, VM-1002
   IV: IV-FMT-0010
   IMP: IMP-0144, IMP-0145, IMP-0208
   Tests: tests/composants/composants_contexte.c
   Spécification : chapitre État d'exécution §4. Segment "interprete"
   (Organisation du dépôt §2.1, choix E3 du 2026-10-23). */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include "pile.h"
#include "contexte_exec.h"
#include "donnees.h"
#include "../interfaces/profil.h"

segment "interprete";

#define PILE_CAPACITE_INITIALE 16
/* IMP-0145 : en modèle small, un tableau obtenu par malloc ne dépasse pas 64 Kio
   (manuel ORCA/C, memorymodel) : 9 362 éléments de 7 octets. */
#define PILE_CAPACITE_MAX ((unsigned int) (65535u / sizeof(ElementPile)))

void pile_initialiser(PileEvaluation *p)
{
    p->elements = NULL;
    p->sommet = 0;
    p->capacite = 0;
}

static void liberer_element(const ElementPile *e)
{
    if (e->type == PILE_CHAINE && e->contenu.chaine.transitoire)
        objets_liberer(e->contenu.chaine.objet);
}

void pile_liberer(PileEvaluation *p)
{
    pile_ramener(p, 0);
    free(p->elements);
    pile_initialiser(p);
}

NatureErreur pile_empiler(PileEvaluation *p, const ElementPile *e)
{
    ElementPile *t;
    unsigned int c;

    if (p->sommet == p->capacite) {                     /* IMP-0145 : doublement borné */
        if (p->capacite >= PILE_CAPACITE_MAX) return ERR_E_MEMOIRE_SATUREE;
        c = p->capacite == 0 ? PILE_CAPACITE_INITIALE : 2 * p->capacite;
        if (c > PILE_CAPACITE_MAX) c = PILE_CAPACITE_MAX;
        t = (ElementPile *) realloc(p->elements, c * sizeof(ElementPile));
        if (t == NULL) return ERR_E_MEMOIRE_SATUREE;
        p->elements = t;
        p->capacite = c;
    }
    p->elements[p->sommet] = *e;
    p->sommet++;
    return ERR_AUCUNE;
}

NatureErreur pile_empiler_nombre(PileEvaluation *p, float v)
{
    ElementPile e;
    e.type = PILE_NUMERIQUE;
    e.contenu.numerique = v;
    return pile_empiler(p, &e);
}

/* Par adresse : aucune conversion au format étendu de SANE (IMP-0244). */
NatureErreur pile_empiler_nombre_exact(PileEvaluation *p, const float *v)
{
    ElementPile e;
    e.type = PILE_NUMERIQUE;
    e.contenu.numerique = *v;
    return pile_empiler(p, &e);
}

NatureErreur pile_empiler_reference(PileEvaluation *p, const ReferenceResolue *r)
{
    ElementPile e;
    e.type = PILE_REFERENCE;
    e.contenu.reference = *r;
    return pile_empiler(p, &e);
}

int pile_lire(const PileEvaluation *p, unsigned int profondeur, ElementPile *copie)
{
    if (profondeur >= p->sommet) return 0;
    *copie = p->elements[p->sommet - 1 - profondeur];
    return 1;
}

NatureErreur pile_retirer(PileEvaluation *p, ElementPile *e)
{
    if (p->sommet == 0) return ERR_E_INTERNE;            /* VM-1002 : consommation impossible */
    p->sommet--;
    *e = p->elements[p->sommet];
    return ERR_AUCUNE;
}

void pile_consommer(PileEvaluation *p, unsigned int n)
{
    while (n > 0 && p->sommet > 0) {
        p->sommet--;
        liberer_element(&p->elements[p->sommet]);
        n--;
    }
}

void pile_ramener(PileEvaluation *p, unsigned int hauteur)
{
    if (p->sommet > hauteur) pile_consommer(p, p->sommet - hauteur);
}

NatureErreur pile_valeur_numerique(const PileEvaluation *p, unsigned int profondeur,
                                   UsageLecture usage, float *v)
{
    ElementPile e;

    if (!pile_lire(p, profondeur, &e)) return ERR_E_INTERNE;
    switch (e.type) {
    case PILE_NUMERIQUE:
        *v = e.contenu.numerique;
        return ERR_AUCUNE;
    case PILE_REFERENCE:
        if (e.contenu.reference.indexee == REF_ELEMENT) {        /* élément de tableau */
            EntreeEnvironnement t;
            NatureErreur n = contexte_entree(e.contenu.reference.cellule, &t);
            if (n != ERR_AUCUNE) return n;
            if (t.etat != ID_TABLEAU) return ERR_E_INDICES_SANS_TABLEAU;
            return donnees_lire_element(t.contenu.objet, e.contenu.reference.decalage, v);
        }
        if (e.contenu.reference.indexee != REF_SIMPLE) return ERR_E_INTERNE;
        return contexte_lire_nombre(e.contenu.reference.cellule, usage, v);
    case PILE_CHAINE:
        if (usage == USAGE_COMPARAISON) return ERR_E_COMPARAISON_TYPES;
        if (usage == USAGE_AFFECTATION) return ERR_E_CHAINE_NON_DECLAREE;
        return ERR_E_CALCUL_NON_NUMERIQUE;
    default:
        return ERR_E_INTERNE;
    }
}
