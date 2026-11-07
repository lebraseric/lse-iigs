/* bibliotheque.h — bibliothécaire : fichiers-programmes (RCI-S) et catalogue
   EXI: EXI-0009, EXI-0022, EXI-0025, EXI-0049
   VM: VM-2209, VM-3401 à VM-3404
   IV: IV-IF-0007
   IMP: IMP-0245 (bibliothèque), IMP-0246 (RCI-S), IMP-0248 (catalogue),
        IMP-0254 (identifications, droits, protections, DUPLIQUER)
   Tests: tests/composants/composants_bibliotheque.c
   Spécification : chapitre Bibliothécaire et fichiers §2, §3, §5. */

#ifndef LSE_BIBLIOTHEQUE_H
#define LSE_BIBLIOTHEQUE_H

#include "../../src/noyau/erreurs.h"
#include "../../src/interfaces/interface_fichiers.h"

/* Protections d'un fichier permanent (INRDP75, 5e partie, §2-4 ; T1600). */
#define PROTECTION_E 1   /* exécution seule */
#define PROTECTION_L 2   /* protection absolue */

/* Catégories du catalogue (IMP-0248). */
#define CATALOGUE_PROGRAMME 'P'
#define CATALOGUE_DONNEES   'D'

/* Nom de fichier L.S.E. : une lettre suivie d'au plus 4 lettres ou chiffres
   (INRDP75, 5e partie, §1-4 b). */
int          bibliotheque_nom_valide(const char *nom, unsigned int n);

/* Nom déjà pris pour créer un fichier permanent de la sorte donnée (programme
   si programme est vrai, fichier-données sinon), dans la bibliothèque ou sur
   l'unité : par un fichier de même sorte, ou, sous le profil MITRA 15 seul,
   par un fichier de l'autre sorte (LSE15 §9-3 ; ailleurs, INRDP75 : point B2
   révisé le 2026-11-06). Rend F 03 (programme), F 04 (fichier-données) ou
   ERR_AUCUNE. */
NatureErreur bibliotheque_nom_pris(int unite, int programme, const char *nom);

/* RANGER (remplacer faux) ou MODIFIER (remplacer vrai) le programme courant
   sous le nom donné (IMP-0246), dans la bibliothèque ou sur l'unité amovible
   si unite est vrai (« $ », IMP-0278). Erreurs : F 03, F 04 (MITRA 15,
   bibliotheque_nom_pris), F 05, F 02, F 27 ; unité absente : F 29. */
NatureErreur bibliotheque_ranger(const char *nom, unsigned char protection, int remplacer, int unite);

/* APPELER, ou EXECUTER si ligne_requise n'est pas nulle : le fichier est
   vérifié en entier, et la ligne requise doit y exister, avant que le
   programme courant et l'état d'exécution soient remplacés (VM-3403).
   Erreurs : F 05, F 100, F 101, F 27, ERR_E_LIGNE_INEXISTANTE (programme
   courant intact) ; F 23 (programme courant vide) ; unité absente : F 29. */
NatureErreur bibliotheque_appeler(const char *nom, unsigned char ligne_requise, int unite);

/* Supprime un fichier et son entrée de catalogue, dans la bibliothèque ou sur
   l'unité selon l'espace e. Erreurs : F 05 ; unité absente : F 29. */
NatureErreur bibliotheque_supprimer(EspaceFichier e, const char *nom);

/* --- identifications (IMP-0254 ; MEMENTO82 §3.6) ------------------------------ */
#define ATTR_P           0x01   /* fichiers-programmes */
#define ATTR_D           0x02   /* fichiers-données permanents */
#define ATTR_S           0x04   /* fichiers secrets (exécution seule) */
#define ATTR_PROTEGE     0x08   /* « ! » : fichiers créés protégés */
#define ATTR_PRIVILEGIE  0x10   /* « > » */
#define ATTR_ETENDU      0x20   /* « x » : espace temporaire fixé à IDENTIFICATION */
#define IDENTIFICATIONS_MAX 9    /* en plus de 0 et 1 */
#define IDENTIFICATION_MAX  65499u

typedef struct {
    unsigned int  numero;        /* 100 × clé + compte */
    unsigned char attributs;
    unsigned int  secteurs;      /* espace temporaire « n » ; 0 : standard */
} Identification;

/* 0 (implicite) et 1 (P, D, S, X) existent toujours ; les autres sont dans
   le fichier IDENTIFICATIONS. 1 si trouvée. */
int          bibliotheque_identification_chercher(unsigned int numero, Identification *id);
/* rang 0 : identification 1 ; puis celles du fichier. 1 si trouvée. */
int          bibliotheque_identification_rang(unsigned int rang, Identification *id);
/* CREATION ID (remplace une identification de même numéro) ; F 19 au-delà
   de 9 ; SUPPRESSION ID : F 10 si absente. */
NatureErreur bibliotheque_identification_creer(const Identification *id);
NatureErreur bibliotheque_identification_supprimer(unsigned int numero);
/* Identification courante de la session ; compte = numéro modulo 100. */
void         bibliotheque_identifier(const Identification *id);
const Identification *bibliotheque_identification(void);
unsigned char bibliotheque_compte(void);
/* Programme courant appelé « exécution seule » sous un autre compte : ni
   listé ni modifié (INRDP75, 5e partie, §1-3). */
int          bibliotheque_programme_secret(void);
void         bibliotheque_programme_libre(void);

/* --- droits sur les fichiers permanents (IMP-0254) ----------------------------- */
/* Création ou modification du fichier-données permanent nom : D, puis
   propriété (F 08, F 22) ; *protection : protection d'un fichier créé. */
NatureErreur bibliotheque_droit_donnees(const char *nom, int unite, unsigned char *protection);
/* Lecture d'un fichier permanent : protection L sous un autre compte. */
int          bibliotheque_lecture_interdite(char categorie, const char *nom, int unite);

/* Entrée de catalogue : 1 si trouvée. */
int          bibliotheque_catalogue_lire(char categorie, const char *nom, unsigned char *compte,
                                         unsigned char *protection, unsigned char date[3]);
/* Entrée de rang donné (rang 0 : première) ; 1 si elle existe, *libre vrai
   pour une entrée libre ; nom : au moins 6 caractères. */
int          bibliotheque_catalogue_rang(unsigned int rang, int *libre, char *categorie, char *nom,
                                         unsigned char *compte, unsigned char *protection,
                                         unsigned char date[3]);
/* Entrée de la catégorie dont le nom est le plus petit au-delà de apres
   (strictement si strict est vrai) ; compte : -1 pour tous les comptes.
   nom : au moins 6 caractères. 1 si trouvée (ordre alphabétique, IMP-0257). */
int          bibliotheque_catalogue_suivant(int unite, char categorie, const char *apres, int strict,
                                            int compte, char *nom, unsigned char *compte_lu,
                                            unsigned char *protection, unsigned char date[3]);
/* DUPLIQUER d'un fichier : de l'unité amovible vers la bibliothèque
   (depuis_unite vrai, « $ ») ou l'inverse. Erreurs : F 29, F 05, F 03
   (programme), F 04 (fichier-données), F 07, F 08 (vers la bibliothèque), F 06 (fichier protégé d'un autre
   compte, vers l'unité), F 27. */
NatureErreur bibliotheque_dupliquer(int depuis_unite, int programme, const char *nom);

/* Entrée de catalogue (IMP-0248) : ajout ou retrait, dans le catalogue de la
   bibliothèque ou de l'unité. */
NatureErreur bibliotheque_cataloguer(int unite, char categorie, const char *nom, unsigned char compte,
                                     unsigned char protection);
void         bibliotheque_decataloguer(int unite, char categorie, const char *nom);

#endif
