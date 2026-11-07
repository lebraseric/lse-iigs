/* types_communs.h — types partagés entre src/noyau/ et src/plateforme/
   EXI: —  VM: —  IV: —
   IMP: IMP-0136 (séparation noyau / plateforme / interfaces), IMP-0180,
        IMP-0196 (configuration de session), IMP-0276 (profil F4)
   Tests: tests/representation/types_plateforme.c

   Contrat partagé uniquement : aucune implémentation, aucun appel système
   (Organisation du dépôt §2). Un type n'est placé ici que s'il est employé
   à la fois par le noyau et par la couche plateforme ; les types propres à
   un module sont déclarés dans l'en-tête de ce module (par exemple
   HandleObjet dans objets.h, Gestionnaire d'objets §4).

   Hypothèses de plateforme dont dépendent RCI-M (§2) et IMP-0184, vérifiées
   par tests/representation/types_plateforme.c :
     char 1 octet ; short et int 2 octets ; long 4 octets ; pointeur 4 octets ;
     float 4 octets (IEEE simple précision) ; double 8 octets ;
     aucun bourrage dans les structures ;
     octet de poids faible en tête pour tout champ de plusieurs octets ;
     un paramètre float est conservé au format SANE extended (10 octets) :
     ne jamais prendre l'adresse d'un paramètre float (RCI-M §2). */

#ifndef LSE_TYPES_COMMUNS_H
#define LSE_TYPES_COMMUNS_H

/* Profil d'implantation (Compilateur §8 bis, IMP-0180, IMP-0276). Les valeurs
   sont rangées dans les fichiers-programmes (RCI-S). */
typedef unsigned char ProfilImplantation;
#define PROFIL_MITRA15_V8  0
#define PROFIL_T1600_V4    1
#define PROFIL_T1600_F4    2   /* profil par défaut : LSE F4, Victor Duruy */

/* Vrai pour les deux versions du T1600 : F4 reprend tout ce qui vaut pour V4,
   sauf différence attestée, testée sur PROFIL_T1600_F4. */
#define PROFIL_EST_T1600(p) ((p) != PROFIL_MITRA15_V8)

/* Configuration de session (IMP-0196) : remplie une fois au lancement par
   src/plateforme/main.c, puis seulement lue par le noyau. */
typedef struct {
    ProfilImplantation profil;
    unsigned char      libelles;   /* 1 : libellé après le code d'erreur (IMP-0186) */
} ConfigurationSession;

extern ConfigurationSession configuration_session;

#endif
