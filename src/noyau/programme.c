/* programme.c — programme courant
   EXI: EXI-0002, EXI-0004, EXI-0006, EXI-0042
   VM: VM-2203, VM-3204, VM-1301
   IV: IV-IF-0002, ALG-1002
   IMP: IMP-0106, IMP-0147, IMP-0176, IMP-0204 (table), IMP-0205 (interface),
        IMP-0206 (effacement), IMP-0207 (compteur), IMP-0243 (premier octet
        copié sans verrouillage)
   Tests: tests/composants/composants_programme.c
   Spécification : chapitre Programme courant §2 à §6. */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include "programme.h"
#include "symboles.h"
#include "../interfaces/profil.h"

segment "programme";

static EntreeObjetCompile *lignes = NULL;          /* IMP-0204 : trié, sans doublon */
static unsigned int nb_lignes = 0;
static TableParametresFormels *declarations = NULL; /* trié par numero_ligne_entree */
static unsigned int nb_declarations = 0;
static unsigned int capacite_declarations = 0;
static unsigned long lignes_entrees = 0;            /* IMP-0207 */

int programme_initialiser(void)
{
    if (lignes == NULL)
        lignes = (EntreeObjetCompile *) malloc(PROGRAMME_CAPACITE * sizeof(EntreeObjetCompile));
    return lignes != NULL;
}

/* Rang de la première ligne de numéro >= n (dichotomie, IMP-0106). */
static unsigned int rang_au_moins(unsigned char n)
{
    unsigned int bas = 0;
    unsigned int haut = nb_lignes;
    unsigned int milieu;

    while (bas < haut) {
        milieu = (bas + haut) / 2;
        if (lignes[milieu].numero_ligne < n) bas = milieu + 1;
        else haut = milieu;
    }
    return bas;
}

/* --- table des paramètres formels (IMP-0147) ---------------------------- */

static int rang_declaration(unsigned char ligne, unsigned int *rang)
{
    unsigned int i;
    for (i = 0; i < nb_declarations && declarations[i].numero_ligne_entree < ligne; i++)
        ;
    *rang = i;
    return i < nb_declarations && declarations[i].numero_ligne_entree == ligne;
}

static void liberer_declaration(TableParametresFormels *t)
{
    free(t->parametres);
    free(t->locaux_non_parametres);
}

/* Décode l'élément 0xCD en tête du bloc ; 1 si la ligne est une
   déclaration (alors *t est rempli), 0 sinon ; -1 si la mémoire manque. */
static int decoder_declaration(const EntreeObjetCompile *l, TableParametresFormels *t)
{
    const unsigned char *o;
    unsigned char tete;
    unsigned int n;
    unsigned int m;
    unsigned int i;
    unsigned int j;
    unsigned int q;
    unsigned int nl;
    unsigned short id;
    int local;
    int r = 0;

    if (l->longueur_octets == 0) return 0;
    /* premier octet copié sans verrouillage (IMP-0243) ; résolution pour une
       déclaration seulement, le bloc restant verrouillé à travers malloc */
    if (!objets_copier(l->bloc, 0, &tete, 1) || tete != RCIM_CTL_PROCEDURE) return 0;
    o = objets_resoudre(l->bloc);
    if (o == NULL) return 0;
    memset(t, 0, sizeof *t);
    t->numero_ligne_entree = l->numero_ligne;
    t->symbole_procedure = (unsigned short) rcim_lire_u16(o + 1);
    n = o[3];
    m = o[4 + 2 * n];
    t->parametres = (ParametreFormel *) malloc((n > 0 ? n : 1) * sizeof(ParametreFormel));
    t->locaux_non_parametres = (unsigned short *) malloc((m > 0 ? m : 1) * sizeof(unsigned short));
    if (t->parametres == NULL || t->locaux_non_parametres == NULL) {
        liberer_declaration(t);
        r = -1;
    } else {
        for (i = 0; i < n; i++) {
            id = (unsigned short) rcim_lire_u16(o + 4 + 2 * i);
            local = 0;
            for (j = 0; j < m; j++)
                if (rcim_lire_u16(o + 5 + 2 * n + 2 * j) == id) local = 1;
            t->parametres[i].symbole = id;
            t->parametres[i].mode = local ? PARAM_LOCAL : PARAM_REFERENCE;
        }
        nl = 0;
        for (j = 0; j < m; j++) {
            q = 5 + 2 * n + 2 * j;
            id = (unsigned short) rcim_lire_u16(o + q);
            local = 0;
            for (i = 0; i < n; i++)
                if (t->parametres[i].symbole == id) local = 1;
            if (!local) t->locaux_non_parametres[nl++] = id;
        }
        t->nb_parametres = (unsigned char) n;
        t->nb_locaux_non_parametres = (unsigned char) nl;
        r = 1;
    }
    objets_relacher(l->bloc);
    return r;
}

/* Déclaration en vigueur de P : celle de plus grand numéro (LSE15 §7-6, Note). */
static void recalculer_procedure(unsigned short p)
{
    InfoProcedure info;
    unsigned int i;

    info.numero_ligne_entree = 0;
    info.arite = 0;
    for (i = 0; i < nb_declarations; i++)
        if (declarations[i].symbole_procedure == p
            && declarations[i].numero_ligne_entree > info.numero_ligne_entree) {
            info.numero_ligne_entree = declarations[i].numero_ligne_entree;
            info.arite = declarations[i].nb_parametres;
        }
    symboles_definir_procedure(p, info);
}

/* INTEGRER_DECLARATION, étape 1 : retrait de l'entrée de la ligne L. */
static void retirer_declaration(unsigned char ligne)
{
    unsigned int rang;
    unsigned short p;

    if (!rang_declaration(ligne, &rang)) return;
    p = declarations[rang].symbole_procedure;
    liberer_declaration(&declarations[rang]);
    memmove(&declarations[rang], &declarations[rang + 1],
            (nb_declarations - rang - 1) * sizeof(TableParametresFormels));
    nb_declarations--;
    recalculer_procedure(p);
}

/* Place pour une entrée de plus ; 0 si la mémoire manque. */
static int reserver_declaration(void)
{
    TableParametresFormels *t;
    unsigned int c;

    if (nb_declarations < capacite_declarations) return 1;
    c = capacite_declarations == 0 ? 8 : 2 * capacite_declarations;
    t = (TableParametresFormels *) realloc(declarations, c * sizeof(TableParametresFormels));
    if (t == NULL) return 0;
    declarations = t;
    capacite_declarations = c;
    return 1;
}

/* INTEGRER_DECLARATION, étape 2 (place déjà réservée). */
static void ajouter_declaration(const TableParametresFormels *t)
{
    unsigned int rang;

    rang_declaration(t->numero_ligne_entree, &rang);
    memmove(&declarations[rang + 1], &declarations[rang],
            (nb_declarations - rang) * sizeof(TableParametresFormels));
    declarations[rang] = *t;
    nb_declarations++;
    recalculer_procedure(t->symbole_procedure);
}

/* --- interface (IMP-0205) ------------------------------------------------- */

int programme_inserer(const EntreeObjetCompile *ligne, NatureErreur *nature)
{
    TableParametresFormels t;
    unsigned int rang;
    int decl;

    *nature = ERR_AUCUNE;
    if (ligne->numero_ligne == 0) return 0;
    if (!programme_initialiser()) { *nature = ERR_C_PLUS_DE_PLACE; return 0; }

    /* tout ce qui peut échouer, avant toute modification (EXI-0004) */
    decl = decoder_declaration(ligne, &t);
    if (decl < 0 || (decl > 0 && !reserver_declaration())) {
        if (decl > 0) liberer_declaration(&t);
        *nature = ERR_C_PLUS_DE_PLACE;
        return 0;
    }

    rang = rang_au_moins(ligne->numero_ligne);
    if (rang < nb_lignes && lignes[rang].numero_ligne == ligne->numero_ligne) {
        retirer_declaration(ligne->numero_ligne);              /* ancienne ligne */
        objets_liberer(lignes[rang].bloc);
    } else {
        memmove(&lignes[rang + 1], &lignes[rang], (nb_lignes - rang) * sizeof(EntreeObjetCompile));
        nb_lignes++;
    }
    lignes[rang] = *ligne;
    if (decl > 0) ajouter_declaration(&t);
    lignes_entrees++;
    return 1;
}

int programme_supprimer(unsigned char numero)
{
    unsigned int rang;

    if (lignes == NULL) return 0;
    rang = rang_au_moins(numero);
    if (rang >= nb_lignes || lignes[rang].numero_ligne != numero) return 0;
    retirer_declaration(numero);
    objets_liberer(lignes[rang].bloc);
    memmove(&lignes[rang], &lignes[rang + 1], (nb_lignes - rang - 1) * sizeof(EntreeObjetCompile));
    nb_lignes--;
    return 1;
}

int programme_chercher(unsigned char numero, EntreeObjetCompile *copie)
{
    unsigned int rang;
    if (lignes == NULL) return 0;
    rang = rang_au_moins(numero);
    if (rang >= nb_lignes || lignes[rang].numero_ligne != numero) return 0;
    *copie = lignes[rang];
    return 1;
}

int programme_premiere(EntreeObjetCompile *copie)
{
    return programme_rang(0, copie);
}

int programme_suivante(unsigned char numero, EntreeObjetCompile *copie)
{
    if (lignes == NULL || numero == 255) return 0;
    return programme_rang(rang_au_moins((unsigned char) (numero + 1)), copie);
}

int programme_au_plus(unsigned char numero, EntreeObjetCompile *copie)
{
    unsigned int rang;
    if (lignes == NULL) return 0;
    rang = numero == 255 ? nb_lignes : rang_au_moins((unsigned char) (numero + 1));
    if (rang == 0) return 0;
    *copie = lignes[rang - 1];
    return 1;
}

unsigned int programme_nombre(void)
{
    return nb_lignes;
}

int programme_rang(unsigned int rang, EntreeObjetCompile *copie)
{
    if (rang >= nb_lignes) return 0;
    *copie = lignes[rang];
    return 1;
}

const TableParametresFormels *programme_parametres(unsigned char ligne_entree)
{
    unsigned int rang;
    return rang_declaration(ligne_entree, &rang) ? &declarations[rang] : NULL;
}

void programme_vider(void)
{
    unsigned int i;

    for (i = 0; i < nb_lignes; i++)
        objets_liberer(lignes[i].bloc);
    nb_lignes = 0;
    for (i = 0; i < nb_declarations; i++)
        liberer_declaration(&declarations[i]);
    nb_declarations = 0;
    symboles_vider();                                         /* IMP-0206 */
}

unsigned long programme_lignes_entrees(void)
{
    return lignes_entrees;
}
