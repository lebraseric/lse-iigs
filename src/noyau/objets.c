/* objets.c — gestionnaire d'objets : blocs-maîtres, slots, compaction,
   objets hors-norme
   EXI: EXI-0016, EXI-0049, EXI-0050
   VM: VM-1601, VM-1602 (Tome III §2.3.11, §2.5.8)
   IV: ANN-K, RCI-M §3 (stabilité de HandleObjet à travers une compaction)
   IMP: IMP-0157 (encodage), IMP-0158 (64 slots), IMP-0159 (allocation),
        IMP-0160 (compaction), IMP-0161 (hors-norme), IMP-0162 (table des
        blocs), IMP-0163 (libération d'un bloc vide), IMP-0194 (objet de
        longueur nulle), IMP-0243 (accès sans verrouillage), IMP-0270 (fin
        de session)
   Tests: tests/composants/composants_objets.c
   Spécification : Gestionnaire d'objets §5 à §11. */

#pragma noroot

#include <string.h>
#include "objets.h"
#include "../interfaces/interface_memoire.h"
#include "../interfaces/profil.h"

segment "objets";

typedef struct {
    unsigned int decalage;  /* occupé : décalage de l'objet dans le bloc ;
                               libre : slot libre suivant, ou GESTOBJ_SLOT_AUCUN */
    unsigned int longueur;  /* occupé : longueur de l'objet ; libre : 0 */
} EntreeSlot;

typedef struct {
    unsigned char nb_slots;
    unsigned char nb_occupes;
    unsigned char premier_slot_libre;
    unsigned int  limite_donnees;
    unsigned int  octets_libres;
    EntreeSlot    slots[GESTOBJ_NB_SLOTS_MAX_PAR_BLOC];
} EnTeteBlocMaitre;

/* Vérification à la compilation : en-tête de 263 octets, sans bourrage (§5).
   Un tableau de taille négative est refusé par le compilateur. */
typedef char verif_taille_entete[(sizeof(EnTeteBlocMaitre) == GESTOBJ_TAILLE_ENTETE) ? 1 : -1];

typedef struct {
    HandlePlateforme handle;    /* NULL si l'entrée n'est pas utilisée */
    unsigned long    longueur;
} DescripteurObjetHorsNorme;

static HandlePlateforme          table_blocs[GESTOBJ_NB_BLOCS_MAX];
static unsigned char             plafond_blocs = 0;
static DescripteurObjetHorsNorme table_hors_norme[GESTOBJ_NB_BLOCS_MAX];

/* --- outils internes --------------------------------------------------- */

static int slot_disponible(EnTeteBlocMaitre *e)
{
    return e->nb_slots < GESTOBJ_NB_SLOTS_MAX_PAR_BLOC
        || e->premier_slot_libre != GESTOBJ_SLOT_AUCUN;
}

static int place_contigue(EnTeteBlocMaitre *e, unsigned int longueur)
{
    return e->limite_donnees >= GESTOBJ_TAILLE_ENTETE + longueur;
}

/* Allocation dans un bloc verrouillé disposant de la place (IMP-0159). */
static HandleObjet allouer_dans_bloc(unsigned char b, EnTeteBlocMaitre *e,
                                     unsigned int longueur)
{
    unsigned char i;

    if (e->premier_slot_libre != GESTOBJ_SLOT_AUCUN) {
        i = e->premier_slot_libre;
        e->premier_slot_libre = (unsigned char) e->slots[i].decalage;
    } else {
        i = e->nb_slots;
        e->nb_slots++;
    }
    e->limite_donnees -= longueur;
    e->slots[i].decalage = e->limite_donnees;
    e->slots[i].longueur = longueur;
    e->nb_occupes++;
    return GESTOBJ_FAIRE_HANDLE(b, i);
}

/* Compaction d'un bloc verrouillé (IMP-0160) : les objets vivants sont tassés
   vers la fin du bloc, du plus proche de la fin au plus proche de
   limite_donnees. Seul le champ decalage change ; les HandleObjet restent
   valables. */
static void compacter(unsigned char *base, EnTeteBlocMaitre *e)
{
    unsigned char ordre[GESTOBJ_NB_SLOTS_MAX_PAR_BLOC];
    unsigned char n;
    unsigned char i;
    unsigned char j;
    unsigned char courant;
    unsigned int nouvelle_limite;

    n = 0;
    for (i = 0; i < e->nb_slots; i++)
        if (e->slots[i].longueur != 0)
            ordre[n++] = i;

    /* tri par insertion, decalage décroissant */
    for (i = 1; i < n; i++) {
        courant = ordre[i];
        j = i;
        while (j > 0 && e->slots[ordre[j - 1]].decalage < e->slots[courant].decalage) {
            ordre[j] = ordre[j - 1];
            j--;
        }
        ordre[j] = courant;
    }

    nouvelle_limite = GESTOBJ_TAILLE_BLOC;
    for (i = 0; i < n; i++) {
        EntreeSlot *s = &e->slots[ordre[i]];
        nouvelle_limite -= s->longueur;
        if (s->decalage != nouvelle_limite) {
            memmove(base + nouvelle_limite, base + s->decalage, s->longueur);
            s->decalage = nouvelle_limite;
        }
    }
    e->limite_donnees = nouvelle_limite;
    e->octets_libres = 0;
}

/* Acquisition d'un bloc-maître (IMP-0162) : d'abord un trou, sinon le plafond. */
static int acquerir_bloc(unsigned char *indice)
{
    unsigned char b;
    HandlePlateforme h;
    EnTeteBlocMaitre *e;

    for (b = 0; b < plafond_blocs; b++)
        if (table_blocs[b] == NULL)
            break;
    if (b == plafond_blocs) {
        if (plafond_blocs >= GESTOBJ_NB_BLOCS_MAX)
            return 0;
    }
    h = plateforme_bloc_allouer(GESTOBJ_TAILLE_BLOC);
    if (h == NULL)
        return 0;
    if (b == plafond_blocs)
        plafond_blocs++;
    table_blocs[b] = h;

    e = (EnTeteBlocMaitre *) plateforme_bloc_adresse(h);   /* après NewHandle, sans HLock (IMP-0243) */
    e->nb_slots = 0;
    e->nb_occupes = 0;
    e->premier_slot_libre = GESTOBJ_SLOT_AUCUN;
    e->limite_donnees = GESTOBJ_TAILLE_BLOC;
    e->octets_libres = 0;

    *indice = b;
    return 1;
}

static HandleObjet creer_hors_norme(unsigned long longueur)
{
    unsigned char i;
    HandlePlateforme h;

    for (i = 0; i < GESTOBJ_NB_BLOCS_MAX; i++)
        if (table_hors_norme[i].handle == NULL)
            break;
    if (i == GESTOBJ_NB_BLOCS_MAX)
        return GESTOBJ_HANDLE_NUL;
    h = plateforme_bloc_allouer(longueur);
    if (h == NULL)
        return GESTOBJ_HANDLE_NUL;
    table_hors_norme[i].handle = h;
    table_hors_norme[i].longueur = longueur;
    return GESTOBJ_FAIRE_HANDLE(GESTOBJ_INDICE_HORS_NORME, i);
}

/* --- interface (§11) --------------------------------------------------- */

/* IMP-0243 : objets_creer, objets_existe, objets_copier, objets_ecrire,
   objets_longueur et objets_liberer lisent et modifient l'en-tête d'un bloc
   par son pointeur maître, sans HLock. Entre la lecture de ce pointeur et le
   dernier accès, aucun appel système ni toolbox n'a lieu : n'y en introduire
   aucun (allocation, entrée-sortie, calcul flottant compris). */

HandleObjet objets_creer(unsigned long longueur_octets)
{
    unsigned char b;
    unsigned int longueur;
    EnTeteBlocMaitre *e;
    HandleObjet resultat;

    /* IMP-0194 : un objet de longueur nulle occupe 1 octet, une
       longueur 0 marquant un slot libre (§5). La longueur logique est
       conservée par l'appelant (par exemple EntreeObjetCompile). */
    if (longueur_octets == 0)
        longueur_octets = 1;

    if (longueur_octets > GESTOBJ_SEUIL_HORS_NORME)
        return creer_hors_norme(longueur_octets);

    longueur = (unsigned int) longueur_octets;

    /* premier bloc convenant (IMP-0159) */
    for (b = 0; b < plafond_blocs; b++) {
        if (table_blocs[b] == NULL)
            continue;
        e = (EnTeteBlocMaitre *) plateforme_bloc_adresse(table_blocs[b]);
        if (slot_disponible(e)) {
            /* étape 3 : la place suffirait au total (contiguë + trous) */
            if (!place_contigue(e, longueur)
                && (unsigned long) (e->limite_donnees - GESTOBJ_TAILLE_ENTETE)
                   + e->octets_libres >= longueur)
                compacter((unsigned char *) e, e);
            if (place_contigue(e, longueur))
                return allouer_dans_bloc(b, e, longueur);
        }
    }

    /* aucun bloc actif ne convient : nouveau bloc-maître */
    if (!acquerir_bloc(&b))
        return GESTOBJ_HANDLE_NUL;
    e = (EnTeteBlocMaitre *) plateforme_bloc_adresse(table_blocs[b]);
    resultat = allouer_dans_bloc(b, e, longueur);
    return resultat;
}

int objets_existe(HandleObjet h)
{
    unsigned char b;
    unsigned char i;
    EnTeteBlocMaitre *e;
    int occupe;

    if (h == GESTOBJ_HANDLE_NUL)
        return 0;
    b = GESTOBJ_BLOC(h);
    i = GESTOBJ_SLOT(h);
    if (b == GESTOBJ_INDICE_HORS_NORME)
        return i < GESTOBJ_NB_BLOCS_MAX && table_hors_norme[i].handle != NULL;
    if (b >= plafond_blocs || table_blocs[b] == NULL)
        return 0;
    e = (EnTeteBlocMaitre *) plateforme_bloc_adresse(table_blocs[b]);
    occupe = i < e->nb_slots && e->slots[i].longueur != 0;
    return occupe;
}

unsigned char *objets_resoudre(HandleObjet h)
{
    unsigned char b;
    unsigned char *base;

    if (!objets_existe(h))
        return NULL;
    b = GESTOBJ_BLOC(h);
    if (b == GESTOBJ_INDICE_HORS_NORME)
        return plateforme_bloc_verrouiller(table_hors_norme[GESTOBJ_SLOT(h)].handle);
    base = plateforme_bloc_verrouiller(table_blocs[b]);
    return base + ((EnTeteBlocMaitre *) base)->slots[GESTOBJ_SLOT(h)].decalage;
}

/* Adresse des octets debut à debut + n - 1 de l'objet, lue dans le pointeur
   maître sans HLock (IMP-0243) ; NULL pour un handle nul ou libéré, ou pour
   une plage qui dépasse l'objet. L'adresse ne vit que le temps d'une copie. */
static unsigned char *plage(HandleObjet h, unsigned long debut, unsigned int n)
{
    unsigned char b;
    unsigned char i;
    unsigned char *base;
    EnTeteBlocMaitre *e;

    if (h == GESTOBJ_HANDLE_NUL)
        return NULL;
    b = GESTOBJ_BLOC(h);
    i = GESTOBJ_SLOT(h);
    if (b == GESTOBJ_INDICE_HORS_NORME) {
        if (i >= GESTOBJ_NB_BLOCS_MAX || table_hors_norme[i].handle == NULL
            || debut + n > table_hors_norme[i].longueur)
            return NULL;
        return plateforme_bloc_adresse(table_hors_norme[i].handle) + debut;
    }
    if (b >= plafond_blocs || table_blocs[b] == NULL)
        return NULL;
    base = plateforme_bloc_adresse(table_blocs[b]);
    e = (EnTeteBlocMaitre *) base;
    if (i >= e->nb_slots || e->slots[i].longueur == 0 || debut + n > e->slots[i].longueur)
        return NULL;
    return base + e->slots[i].decalage + debut;
}

int objets_copier(HandleObjet h, unsigned long debut, unsigned char *dest, unsigned int n)
{
    unsigned char *p = plage(h, debut, n);
    if (p == NULL)
        return 0;
    memcpy(dest, p, n);
    return 1;
}

int objets_ecrire(HandleObjet h, unsigned long debut, const unsigned char *src, unsigned int n)
{
    unsigned char *p = plage(h, debut, n);
    if (p == NULL)
        return 0;
    memcpy(p, src, n);
    return 1;
}

unsigned long objets_longueur(HandleObjet h)
{
    unsigned char b;
    unsigned char i;
    EnTeteBlocMaitre *e;

    if (h == GESTOBJ_HANDLE_NUL)
        return 0;
    b = GESTOBJ_BLOC(h);
    i = GESTOBJ_SLOT(h);
    if (b == GESTOBJ_INDICE_HORS_NORME)
        return i < GESTOBJ_NB_BLOCS_MAX && table_hors_norme[i].handle != NULL
               ? table_hors_norme[i].longueur : 0;
    if (b >= plafond_blocs || table_blocs[b] == NULL)
        return 0;
    e = (EnTeteBlocMaitre *) plateforme_bloc_adresse(table_blocs[b]);   /* lecture seule (IMP-0243) */
    return i < e->nb_slots ? e->slots[i].longueur : 0;
}

void objets_relacher(HandleObjet h)
{
    unsigned char b;

    if (!objets_existe(h))
        return;
    b = GESTOBJ_BLOC(h);
    if (b == GESTOBJ_INDICE_HORS_NORME)
        plateforme_bloc_deverrouiller(table_hors_norme[GESTOBJ_SLOT(h)].handle);
    else
        plateforme_bloc_deverrouiller(table_blocs[b]);
}

void objets_liberer(HandleObjet h)
{
    unsigned char b;
    unsigned char i;
    EnTeteBlocMaitre *e;
    EntreeSlot *s;
    int vide;

    if (!objets_existe(h))
        return;
    b = GESTOBJ_BLOC(h);
    i = GESTOBJ_SLOT(h);

    if (b == GESTOBJ_INDICE_HORS_NORME) {
        plateforme_bloc_liberer(table_hors_norme[i].handle);
        table_hors_norme[i].handle = NULL;
        table_hors_norme[i].longueur = 0;
        return;
    }

    /* §7 */
    e = (EnTeteBlocMaitre *) plateforme_bloc_adresse(table_blocs[b]);
    s = &e->slots[i];
    if (s->decalage == e->limite_donnees)
        e->limite_donnees += s->longueur;
    else
        e->octets_libres += s->longueur;
    s->longueur = 0;
    s->decalage = e->premier_slot_libre;
    e->premier_slot_libre = i;
    e->nb_occupes--;
    vide = (e->nb_occupes == 0);

    if (vide) {
        /* IMP-0163 : bloc vide rendu au système, indice réutilisable */
        plateforme_bloc_liberer(table_blocs[b]);
        table_blocs[b] = NULL;
    }
}

unsigned long objets_capacite_disponible(void)
{
    return plateforme_memoire_disponible();
}

/* IMP-0270 : chaque bloc encore attribué est rendu un par un, l'identifiant
   userid() étant partagé avec la bibliothèque d'ORCA/C. */
void objets_terminer(void)
{
    unsigned char i;

    for (i = 0; i < plafond_blocs; i++)
        if (table_blocs[i] != NULL) {
            plateforme_bloc_liberer(table_blocs[i]);
            table_blocs[i] = NULL;
        }
    plafond_blocs = 0;
    for (i = 0; i < GESTOBJ_NB_BLOCS_MAX; i++)
        if (table_hors_norme[i].handle != NULL) {
            plateforme_bloc_liberer(table_hors_norme[i].handle);
            table_hors_norme[i].handle = NULL;
            table_hors_norme[i].longueur = 0;
        }
}
