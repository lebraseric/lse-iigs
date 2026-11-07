/* interface_fichiers.h — interface entre le noyau et le système de fichiers
   EXI: EXI-0009, EXI-0053 à EXI-0057
   VM: VM-3401, VM-3404, VM-3405
   IMP: IMP-0136 (séparation noyau / plateforme), IMP-0245 (bibliothèque),
        IMP-0249 (interface)
   Tests: tests/plateforme/plateforme_fichiers.c
   Spécification : chapitre Bibliothécaire et fichiers §2, §6.
   Le noyau désigne un fichier par un espace et un nom ; seule la plateforme
   compose le chemin. Positions et tailles en octets. */

#ifndef LSE_INTERFACE_FICHIERS_H
#define LSE_INTERFACE_FICHIERS_H

typedef unsigned char EspaceFichier;
#define ESPACE_RACINE       0   /* CATALOGUE, IDENTIFICATIONS */
#define ESPACE_PROGRAMMES   1
#define ESPACE_DONNEES      2   /* fichiers-données permanents */
#define ESPACE_TEMPORAIRES  3
#define ESPACE_UNITE_PROGRAMMES 4   /* unité amovible (-UNITE ; DUPLIQUER, « $ », IMP-0278) */
#define ESPACE_UNITE_DONNEES    5
#define ESPACE_UNITE_RACINE     6   /* CATALOGUE de l'unité (IMP-0258) */
#define ESPACE_RUBAN            7   /* le ruban, quel que soit le nom (IMP-0262) */

typedef unsigned char ModeFichier;
#define FICHIER_LECTURE     0   /* lecture seule */
#define FICHIER_MISE_A_JOUR 1   /* lecture et écriture d'un fichier existant */
#define FICHIER_CREATION    2   /* crée ou vide, puis lecture et écriture */

typedef void *FichierPlateforme;

/* Résultats */
#define PLAT_OK          0
#define PLAT_INEXISTANT  1   /* fichier ou répertoire absent ; jamais un fichier présent mais refusé */
#define PLAT_PLEIN       2   /* écriture incomplète : support plein, cause supposée de tout échec d'écriture */
#define PLAT_ERREUR      3   /* autre échec d'entrée-sortie, dont un fichier verrouillé ou un volume absent */

/* Désigne la bibliothèque (option -BIB ; NULL : @:LSE) et crée ses
   répertoires manquants (IMP-0245). */
int           plateforme_bibliotheque_ouvrir(const char *chemin);

int           plateforme_fichier_ouvrir(EspaceFichier e, const char *nom, ModeFichier m,
                                        FichierPlateforme *f);
int           plateforme_fichier_lire(FichierPlateforme f, unsigned long position,
                                      void *dest, unsigned int n);
int           plateforme_fichier_ecrire(FichierPlateforme f, unsigned long position,
                                        const void *src, unsigned int n);
unsigned long plateforme_fichier_taille(FichierPlateforme f);
void          plateforme_fichier_fermer(FichierPlateforme f);

int           plateforme_fichier_existe(EspaceFichier e, const char *nom);
/* 1 si la dernière ouverture de la bibliothèque a réussi (IMP-0250) ; 1 avant toute ouverture. */
int           plateforme_bibliotheque_disponible(void);
int           plateforme_fichier_supprimer(EspaceFichier e, const char *nom);
/* Change d'espace ou de nom (CATALOGUER, MODIFIER). */
int           plateforme_fichier_renommer(EspaceFichier e1, const char *nom1,
                                          EspaceFichier e2, const char *nom2);
/* Nom du fichier de rang donné (0 : premier) dans l'espace ; 1 si trouvé.
   nom : au moins 16 caractères. */
int           plateforme_fichier_lister(EspaceFichier e, unsigned int rang, char *nom);
/* Appelle f(nom, contexte) pour chaque fichier de l'espace, dans l'ordre du
   répertoire, en une seule ouverture (audit du code, AP-06) ; nom : au plus
   15 caractères. 1 si le répertoire a pu être ouvert. */
int           plateforme_espace_parcourir(EspaceFichier e, void (*f)(const char *nom, void *contexte),
                                          void *contexte);
/* Supprime tous les fichiers de l'espace (temporaires, IMP-0245). */
int           plateforme_espace_vider(EspaceFichier e);
/* Désigne l'unité amovible (option -UNITE), bibliothèque de même forme ;
   NULL : aucune. 1 si une unité est désignée. Ses répertoires sont créés à
   la première création d'un fichier sur l'unité. */
void          plateforme_unite_designer(const char *chemin);
int           plateforme_unite_disponible(void);
/* Désigne le fichier du ruban (option -RUBAN) ; NULL : RUBAN dans la
   racine de la bibliothèque (IMP-0262). */
void          plateforme_ruban_designer(const char *chemin);
/* Crée un fichier texte vide (type ProDOS TXT), s'il n'existe pas. */
int           plateforme_fichier_creer_texte(EspaceFichier e, const char *nom);
/* Secteurs de 256 octets libres sur le support de la bibliothèque ; 0 si
   inconnu (UTILISATION DISQUE, IDENTIFICATION). */
unsigned long plateforme_secteurs_libres(void);

#endif
