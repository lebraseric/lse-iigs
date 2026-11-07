/* symboles.h — table des symboles et table des constantes partagées du
   programme courant
   EXI: EXI-0001, EXI-0015, EXI-0016
   VM: VM-2401  IV: ANN-K-0009 (nom conservé une seule fois), ANN-K-0011,
       ANN-K-0012 (constantes partagées)
   IMP: IMP-0104 (identifiant 16 bits), IMP-0107 (constantes partagées),
        IMP-0109 (origine), IMP-0114 (InfoProcedure), IMP-0124 (nom en ligne),
        IMP-0127 (unsigned char), IMP-0132 (realloc), IMP-0167 (recherche et
        insertion), IMP-0181 (partage des chaînes longues), IMP-0191
        (capacité), IMP-0193 (segment), IMP-0246 (ajout sans recherche)
   Tests: tests/composants/composants_symboles.c
   Spécification : RCI-M §6, §7 ; chapitre Compilateur §7. */

#ifndef LSE_SYMBOLES_H
#define LSE_SYMBOLES_H

#include "../../src/noyau/objets.h"

typedef unsigned char NatureSymbole;
#define SYM_IDENTIFICATEUR  0   /* catégorie effective tenue par l'état d'exécution */
#define SYM_PROCEDURE       1   /* nom introduit par & */

typedef unsigned char OrigineSymbole;
#define ORIGINE_PROGRAMME       0   /* employé par au moins une ligne de programme */
#define ORIGINE_MACHINE_BUREAU  1   /* employé uniquement en mode machine de bureau */

typedef struct {
    unsigned char numero_ligne_entree;  /* 0 : aucune déclaration en vigueur */
    unsigned char arite;
} InfoProcedure;

typedef struct {
    unsigned char  nom_longueur;   /* 1 à 5 */
    char           nom[5];         /* sans le & d'un nom de procédure */
    NatureSymbole  nature;
    OrigineSymbole origine;
    InfoProcedure  procedure;      /* {0, 0} pour SYM_IDENTIFICATEUR */
} EntreeSymbole;                   /* 10 octets ; aucune valeur d'exécution (IMP-0110) */

typedef struct {
    unsigned char longueur;        /* 17 à 127 (seuil de partage IMP-0107) */
    HandleObjet   texte;           /* jamais modifié après création */
} EntreeConstante;                 /* 3 octets */

#define SYMBOLES_NOM_MAX        5
/* Chaque table est un seul tableau alloué, qui ne dépasse pas 64 Kio en modèle
   small (IMP-0104) : 6 553 symboles, 21 845 constantes. */
#define SYMBOLES_CAPACITE_MAX   ((unsigned int) (65535u / sizeof(EntreeSymbole)))
#define CONSTANTES_CAPACITE_MAX ((unsigned int) (65535u / sizeof(EntreeConstante)))
#define CONSTANTES_SEUIL_PARTAGE 16      /* une chaîne plus longue est partagée (IMP-0107) */

/* Résultats */
#define SYMB_OK          0
#define SYMB_ERR_MEMOIRE 1   /* plus de place : nature C 57 (IMP-0191) */
#define SYMB_ERR_CAPACITE 2  /* plafond de la table atteint : C 108 (IMP-0191) */
#define SYMB_ERR_ARGUMENT 3  /* nom ou texte de longueur invalide (erreur de l'appelant) */

/* --- table des symboles (IMP-0167) ---------------------------------------
   Recherche linéaire par nom ET nature ; à défaut, insertion en fin de
   table. Un symbole employé par une ligne de programme prend définitivement
   l'origine ORIGINE_PROGRAMME (IMP-0109). */
int  symboles_chercher_ou_creer(const char *nom, unsigned char longueur,
                                NatureSymbole nature, OrigineSymbole origine,
                                unsigned int *id);
/* Ajout en fin de table, sans recherche : restauration d'un fichier-programme
   dont l'unicité des noms est déjà vérifiée (IMP-0246). */
int  symboles_ajouter(const char *nom, unsigned char longueur,
                      NatureSymbole nature, OrigineSymbole origine,
                      unsigned int *id);
/* 1 et *id si le symbole existe, 0 sinon ; ne crée rien. */
int  symboles_chercher(const char *nom, unsigned char longueur,
                       NatureSymbole nature, unsigned int *id);
unsigned int symboles_nombre(void);
/* Copie de l'entrée ; 0 si id n'existe pas. Aucun pointeur dans la table
   n'est jamais rendu : la table est agrandie par realloc. */
int  symboles_lire(unsigned int id, EntreeSymbole *copie);
/* Renseigné par le gestionnaire du programme courant (IMP-0176). */
void symboles_definir_procedure(unsigned int id, InfoProcedure info);

/* --- table des constantes partagées (IMP-0107, IMP-0181) ----------------- */
int  constantes_chercher_ou_creer(const unsigned char *texte, unsigned char longueur,
                                  unsigned int *index);
/* Ajout en fin de table, sans recherche (IMP-0246), comme symboles_ajouter. */
int  constantes_ajouter(const unsigned char *texte, unsigned char longueur,
                        unsigned int *index);
unsigned int constantes_nombre(void);
int  constantes_lire(unsigned int index, EntreeConstante *copie);

/* Vide les deux tables, objets texte compris, au remplacement ou à
   l'effacement du programme courant (IMP-0107). */
void symboles_vider(void);

#endif
