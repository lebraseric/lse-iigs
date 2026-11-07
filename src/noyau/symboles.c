/* symboles.c — table des symboles et table des constantes partagées
   EXI: EXI-0001, EXI-0015, EXI-0016
   VM: VM-2401  IV: ANN-K-0009, ANN-K-0011, ANN-K-0012
   IMP: IMP-0104, IMP-0107, IMP-0109, IMP-0114, IMP-0124, IMP-0127,
        IMP-0132, IMP-0167, IMP-0181, IMP-0191, IMP-0193
   Tests: tests/composants/composants_symboles.c
   Spécification : RCI-M §6, §7 ; chapitre Compilateur §7. */

#pragma noroot

#include <stdlib.h>
#include <string.h>
#include "symboles.h"
#include "../interfaces/profil.h"

segment "rcim";

/* Vérifications à la compilation (RCI-M §6.2, §7 ; aucun bourrage, §2). */
typedef char verif_taille_symbole[(sizeof(EntreeSymbole) == 10) ? 1 : -1];
typedef char verif_taille_constante[(sizeof(EntreeConstante) == 3) ? 1 : -1];

#define TRANCHE 32     /* agrandissement des tables (audit du code, AP-12) */

static EntreeSymbole   *table_symboles = NULL;
static unsigned int     nb_symboles = 0;
static unsigned int     cap_symboles = 0;
static EntreeConstante *table_constantes = NULL;
static unsigned int     nb_constantes = 0;
static unsigned int     cap_constantes = 0;

/* Capacité agrandie d'une tranche, sans dépasser le plafond. */
static unsigned int agrandie(unsigned int cap, unsigned int plafond)
{
    return plafond - cap > TRANCHE ? cap + TRANCHE : plafond;
}

/* --- table des symboles ------------------------------------------------ */

/* Comparaison en ligne, premier caractère d'abord (audit du code, AP-05). */
int symboles_chercher(const char *nom, unsigned char longueur,
                      NatureSymbole nature, unsigned int *id)
{
    unsigned int i;
    const EntreeSymbole *e;
    char c0 = nom[0];

    for (i = 0; i < nb_symboles; i++) {
        e = &table_symboles[i];
        if (e->nom[0] == c0 && e->nom_longueur == longueur && e->nature == nature
            && (longueur == 1 || memcmp(e->nom + 1, nom + 1, longueur - 1) == 0)) {
            *id = i;
            return 1;
        }
    }
    return 0;
}

int symboles_chercher_ou_creer(const char *nom, unsigned char longueur,
                               NatureSymbole nature, OrigineSymbole origine,
                               unsigned int *id)
{
    if (longueur < 1 || longueur > SYMBOLES_NOM_MAX)
        return SYMB_ERR_ARGUMENT;

    if (symboles_chercher(nom, longueur, nature, id)) {
        if (origine == ORIGINE_PROGRAMME)
            table_symboles[*id].origine = ORIGINE_PROGRAMME;
        return SYMB_OK;
    }
    return symboles_ajouter(nom, longueur, nature, origine, id);
}

int symboles_ajouter(const char *nom, unsigned char longueur,
                     NatureSymbole nature, OrigineSymbole origine,
                     unsigned int *id)
{
    EntreeSymbole *nouvelle;
    EntreeSymbole *e;
    unsigned int c;

    if (longueur < 1 || longueur > SYMBOLES_NOM_MAX)
        return SYMB_ERR_ARGUMENT;
    if (nb_symboles >= SYMBOLES_CAPACITE_MAX)
        return SYMB_ERR_CAPACITE;
    if (nb_symboles == cap_symboles) {
        c = agrandie(cap_symboles, SYMBOLES_CAPACITE_MAX);
        nouvelle = (EntreeSymbole *) realloc(table_symboles, (size_t) c * sizeof(EntreeSymbole));
        if (nouvelle == NULL)
            return SYMB_ERR_MEMOIRE;
        table_symboles = nouvelle;
        cap_symboles = c;
    }

    e = &table_symboles[nb_symboles];
    memset(e, 0, sizeof(EntreeSymbole));
    e->nom_longueur = longueur;
    memcpy(e->nom, nom, longueur);
    e->nature = nature;
    e->origine = origine;
    e->procedure.numero_ligne_entree = 0;
    e->procedure.arite = 0;
    *id = nb_symboles;
    nb_symboles++;
    return SYMB_OK;
}

unsigned int symboles_nombre(void)
{
    return nb_symboles;
}

int symboles_lire(unsigned int id, EntreeSymbole *copie)
{
    if (id >= nb_symboles)
        return 0;
    *copie = table_symboles[id];
    return 1;
}

void symboles_definir_procedure(unsigned int id, InfoProcedure info)
{
    if (id < nb_symboles && table_symboles[id].nature == SYM_PROCEDURE)
        table_symboles[id].procedure = info;
}

/* --- table des constantes partagées ------------------------------------ */

static int meme_texte(const EntreeConstante *e, const unsigned char *texte,
                      unsigned char longueur)
{
    unsigned char *p;
    int egal;

    if (e->longueur != longueur)
        return 0;
    p = objets_resoudre(e->texte);
    egal = p != NULL && memcmp(p, texte, longueur) == 0;
    objets_relacher(e->texte);
    return egal;
}

int constantes_chercher_ou_creer(const unsigned char *texte, unsigned char longueur,
                                 unsigned int *index)
{
    unsigned int i;

    if (longueur <= CONSTANTES_SEUIL_PARTAGE || longueur > 127)
        return SYMB_ERR_ARGUMENT;

    for (i = 0; i < nb_constantes; i++)
        if (meme_texte(&table_constantes[i], texte, longueur)) {
            *index = i;
            return SYMB_OK;
        }
    return constantes_ajouter(texte, longueur, index);
}

int constantes_ajouter(const unsigned char *texte, unsigned char longueur,
                       unsigned int *index)
{
    HandleObjet h;
    unsigned char *p;
    EntreeConstante *nouvelle;
    unsigned int c;

    if (longueur <= CONSTANTES_SEUIL_PARTAGE || longueur > 127)
        return SYMB_ERR_ARGUMENT;
    if (nb_constantes >= CONSTANTES_CAPACITE_MAX)
        return SYMB_ERR_CAPACITE;
    if (nb_constantes == cap_constantes) {
        c = agrandie(cap_constantes, CONSTANTES_CAPACITE_MAX);
        nouvelle = (EntreeConstante *) realloc(table_constantes, (size_t) c * sizeof(EntreeConstante));
        if (nouvelle == NULL)
            return SYMB_ERR_MEMOIRE;
        table_constantes = nouvelle;
        cap_constantes = c;
    }
    h = objets_creer(longueur);
    if (h == GESTOBJ_HANDLE_NUL)
        return SYMB_ERR_MEMOIRE;

    p = objets_resoudre(h);
    memcpy(p, texte, longueur);
    objets_relacher(h);

    table_constantes[nb_constantes].longueur = longueur;
    table_constantes[nb_constantes].texte = h;
    *index = nb_constantes;
    nb_constantes++;
    return SYMB_OK;
}

unsigned int constantes_nombre(void)
{
    return nb_constantes;
}

int constantes_lire(unsigned int index, EntreeConstante *copie)
{
    if (index >= nb_constantes)
        return 0;
    *copie = table_constantes[index];
    return 1;
}

/* --- vidage ------------------------------------------------------------ */

void symboles_vider(void)
{
    unsigned int i;

    for (i = 0; i < nb_constantes; i++)
        objets_liberer(table_constantes[i].texte);
    free(table_constantes);
    table_constantes = NULL;
    nb_constantes = 0;
    cap_constantes = 0;

    free(table_symboles);
    table_symboles = NULL;
    nb_symboles = 0;
    cap_symboles = 0;
}
