/* donnees.h — gestionnaire des variables-chaîne, des tableaux et de
   l'écriture des nombres
   EXI: EXI-0039  VM: VM-1102, VM-1201, VM-1202
   IV: Tome IV §4.8
   IMP: IMP-0208 (frontière avec le contexte), IMP-0209 (chaînes), IMP-0210
        (tableaux), IMP-0211 (écriture U, F, E), IMP-0212 (fonctions sur les
        chaînes), IMP-0213 (interface)
   Tests: tests/composants/composants_donnees.c
   Spécification : chapitre Donnees (tome-v/Donnees_LSE-IIGS.md).
   Les positions, longueurs, bornes et indices sont reçus en float et arrondis
   à l'entier le plus voisin (IMP-0179). */

#ifndef LSE_DONNEES_H
#define LSE_DONNEES_H

#include "../../src/noyau/pile.h"

#define DONNEES_GENRE_CHAINE    0
#define DONNEES_GENRE_TABLEAU   1
#define DONNEES_DECALAGE_TEXTE  3      /* IMP-0209 */

/* --- opérations dont dépend le contexte d'exécution (IMP-0208) --- */
/* Objet propre à une variable-chaîne : reprise d'un objet transitoire, copie sinon. */
NatureErreur donnees_chaine_vers_objet(const ChaineSurPile *c, HandleObjet *objet);
/* Désignation, non transitoire, du texte d'un objet chaîne. */
NatureErreur donnees_objet_vers_chaine(HandleObjet objet, ChaineSurPile *c);
/* Copie indépendante d'un objet chaîne ou tableau. */
NatureErreur donnees_copier(HandleObjet objet, HandleObjet *copie);

/* --- chaînes (IMP-0209, §3) --- */
/* Chaîne transitoire neuve portant le texte donné. */
NatureErreur donnees_chaine_creer(const unsigned char *texte, unsigned int n, ChaineSurPile *res);
/* Copie au plus max caractères du texte, à partir du rang debut (0 : premier) ;
   renvoie le nombre copié. */
unsigned int donnees_texte(const ChaineSurPile *c, unsigned int debut, unsigned char *dest,
                           unsigned int max);
NatureErreur donnees_concatener(const ChaineSurPile *a, const ChaineSurPile *b, ChaineSurPile *res);
/* < 0, 0 ou > 0 selon LSE15 §5-2. */
int          donnees_comparer(const ChaineSurPile *a, const ChaineSurPile *b);

/* --- fonctions de LSE15 §4-4.2 (IMP-0212) ; un paramètre facultatif absent
   est NULL ; *delta reçoit la position de fin. --- */
NatureErreur donnees_sch(const ChaineSurPile *a, float beta, const ChaineSurPile *arret,
                         float longueur, ChaineSurPile *res, float *delta);
NatureErreur donnees_grl(const ChaineSurPile *a, float beta, ChaineSurPile *res, float *delta);
NatureErreur donnees_ptr(const ChaineSurPile *a, float beta, int avec_gamma,
                         const ChaineSurPile *arret, float longueur, float *res);
NatureErreur donnees_skp(const ChaineSurPile *a, float beta, const ChaineSurPile *liste, float *res);
NatureErreur donnees_pos(const ChaineSurPile *a, float beta, const ChaineSurPile *g, float *res);
NatureErreur donnees_cnb(const ChaineSurPile *a, float beta, float *res, float *gamma);
NatureErreur donnees_eqn(const ChaineSurPile *a, float beta, float *res);
NatureErreur donnees_eqc(float v, ChaineSurPile *res);
NatureErreur donnees_cca(float v, ChaineSurPile *res);

/* --- écriture d'un nombre (IMP-0211) : type RCIM_DESC_F, _E ou _U ;
   sortie d'au moins 256 caractères ; renvoie la longueur écrite. --- */
unsigned int donnees_ecrire_nombre(float v, unsigned char type, unsigned char e, unsigned char d,
                                   char *sortie);

/* --- nombres : réalisations exactes, sans SANE (IMP-0244) ; les nombres
   passent par adresse, ORCA/C convertissant au format étendu tout float
   passé ou rendu par valeur. --- */
/* *r reçoit le float de valeur v ; exact pour |v| < 2^24 (2^23 sous le
   profil T1600, IMP-0280). */
void         donnees_nombre_entier(long v, float *r);
/* Réel rangé en float : ERR_E_DEPASSEMENT (EX 44) au-delà du plus grand
   réel (IMP-0216) ; sous le profil T1600, arrondi d'abord à 23 bits
   significatifs, la demie en s'éloignant de zéro (IMP-0280). */
NatureErreur donnees_ranger_reel(double v, float *r);
/* Même rangement ; au-delà, le plus grand réel du profil, de même signe
   (nombre lu par LIRE ou converti par CNB : Données IMP-0212, Interpréteur
   IMP-0222). */
void         donnees_ranger_reel_borne(double v, float *r);
/* < 0, 0 ou > 0 selon l'ordre des valeurs ; +0 et -0 sont égaux. Nombres finis. */
int          donnees_comparer_nombres(const float *a, const float *b);
/* a op b (op : '+', '-', '*') calculé en entiers quand a, b et le résultat
   sont des entiers de valeur absolue inférieure à 2^24 (résultat inférieur à
   2^23 sous le profil T1600) : rend 1 et *r porte les mêmes bits que le
   calcul en double d'IMP-0216, ou que l'arrondi d'IMP-0280 ; sinon rend 0. */
int          donnees_operation_entiere(char op, const float *a, const float *b, float *r);
/* *r = -*a, par le bit de signe. */
void         donnees_opposer(const float *a, float *r);
/* a op b (op : '+', '-', '*', '/') hors du chemin exact : calcul en format
   étendu, *r portant les mêmes bits que le calcul en double d'IMP-0216 ;
   ERR_E_DIVISION_PAR_ZERO, ERR_E_DEPASSEMENT (EX 44) comme lui. Sous le
   profil T1600, le résultat étendu est arrondi à 23 bits (IMP-0280). */
NatureErreur donnees_operation_etendue(char op, const float *a, const float *b, float *r);

/* --- tableaux (IMP-0210) --- */
NatureErreur donnees_creer_tableau(unsigned char dims, float d1, float d2, HandleObjet *tableau);
NatureErreur donnees_rang(HandleObjet tableau, unsigned char nb_indices, float i1, float i2,
                          unsigned int *rang);
/* Par adresse ; un indice entier exact n'est pas arrondi (IMP-0244 ; AP-02). */
NatureErreur donnees_rang_exact(HandleObjet tableau, unsigned char nb_indices, const float *i1,
                                const float *i2, unsigned int *rang);
NatureErreur donnees_lire_element(HandleObjet tableau, unsigned int rang, float *v);
NatureErreur donnees_ecrire_element(HandleObjet tableau, unsigned int rang, float v);
/* Par adresse, sans conversion au format étendu (IMP-0244 ; AP-07). */
NatureErreur donnees_ecrire_element_exact(HandleObjet tableau, unsigned int rang, const float *v);
/* Tous les éléments reçoivent v (UN, ZER, IMP-0218). */
NatureErreur donnees_remplir(HandleObjet tableau, float v);
NatureErreur donnees_tableau_bornes(HandleObjet tableau, unsigned char *dims,
                                    unsigned int *d1, unsigned int *d2);
/* Image de tableau lue d'un fichier (CHARGER) : 1 si son en-tête est celui d'un
   tableau de donnees_creer_tableau et s'accorde avec sa longueur lg. */
int          donnees_image_tableau_valide(const unsigned char *t, unsigned long lg);

#endif
