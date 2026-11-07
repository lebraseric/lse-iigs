/* objets.h — gestionnaire d'objets (gestionnaire mémoire du noyau)
   EXI: EXI-0016, EXI-0049, EXI-0050
   VM: VM-1601, VM-1602 (Tome III §2.3.11, §2.5.8)
   IV: ANN-K (objets d'exécution), RCI-M §3
   IMP: IMP-0157 à IMP-0163, IMP-0194, IMP-0243, IMP-0270
   Tests: tests/composants/composants_objets.c
   Spécification : Gestionnaire d'objets §4, §11. */

#ifndef LSE_OBJETS_H
#define LSE_OBJETS_H

/* IMP-0157 : octet haut = indice de bloc-maître (0 à 254) ou 0xFF pour un
   objet hors-norme ; octet bas = indice de slot, ou indice dans la table des
   objets hors-norme. */
typedef unsigned int HandleObjet;   /* 16 bits, RCI-M IMP-0101 */

#define GESTOBJ_SENTINELLE        0xFFu
#define GESTOBJ_INDICE_HORS_NORME GESTOBJ_SENTINELLE
#define GESTOBJ_SLOT_AUCUN        GESTOBJ_SENTINELLE
#define GESTOBJ_HANDLE_NUL        ((HandleObjet) 0xFFFFu)

#define GESTOBJ_FAIRE_HANDLE(bloc, slot) \
    ((HandleObjet) (((unsigned int) (bloc) << 8) | (unsigned int) (slot)))
#define GESTOBJ_BLOC(h)  ((unsigned char) ((h) >> 8))
#define GESTOBJ_SLOT(h)  ((unsigned char) ((h) & 0xFFu))

#define GESTOBJ_TAILLE_BLOC           4096u  /* octets d'un bloc-maître (§5) */
#define GESTOBJ_NB_SLOTS_MAX_PAR_BLOC 64     /* IMP-0158 */
#define GESTOBJ_TAILLE_ENTETE         263u   /* sizeof(EnTeteBlocMaitre), §5 */
#define GESTOBJ_NB_BLOCS_MAX          255    /* IMP-0162 */
/* Au-delà de ce seuil, un objet est hors-norme (IMP-0161) : 3833 octets. */
#define GESTOBJ_SEUIL_HORS_NORME      (GESTOBJ_TAILLE_BLOC - GESTOBJ_TAILLE_ENTETE)

/* Crée un objet ; GESTOBJ_HANDLE_NUL si la ressource manque (Tome III §2.3.11). */
HandleObjet    objets_creer(unsigned long longueur_octets);
/* Verrouille l'objet et renvoie son adresse, valable jusqu'à objets_relacher.
   Aucune résolution ne doit rester ouverte à travers objets_creer ou
   objets_liberer (§8, §11). Le verrouillage porte sur le bloc et n'est pas
   compté : relâcher un objet déverrouille aussi les autres objets résolus du
   même bloc, dont l'adresse ne reste sûre qu'en l'absence d'appel système ou
   toolbox (§11). NULL si h ne désigne aucun objet. */
unsigned char *objets_resoudre(HandleObjet h);
void           objets_relacher(HandleObjet h);
/* Copie n octets de l'objet, à partir de l'octet debut, vers dest, sans
   verrouiller son bloc (IMP-0243) ; aucune adresse n'est conservée. Rend 0
   si h ne désigne aucun objet ou si la plage dépasse l'objet. Ne déverrouille
   aucun autre objet. */
int            objets_copier(HandleObjet h, unsigned long debut, unsigned char *dest,
                             unsigned int n);
/* Copie n octets de src dans l'objet, à partir de l'octet debut, sans
   verrouiller son bloc (IMP-0243) ; mêmes résultats qu'objets_copier. */
int            objets_ecrire(HandleObjet h, unsigned long debut, const unsigned char *src,
                             unsigned int n);
void           objets_liberer(HandleObjet h);
/* Contrôle défensif de cohérence, sans protection contre un handle obsolète. */
int            objets_existe(HandleObjet h);
/* Longueur de l'objet en octets ; 0 si h ne désigne aucun objet (GARER). */
unsigned long  objets_longueur(HandleObjet h);
unsigned long  objets_capacite_disponible(void);
/* Fin de session (IMP-0270) : rend au système tous les blocs encore attribués ;
   aucun objet n'existe plus ensuite, et le gestionnaire repart à vide. */
void           objets_terminer(void);

#endif
