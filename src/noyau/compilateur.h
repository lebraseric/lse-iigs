/* compilateur.h — compilation d'une ligne (programme ou machine de bureau)
   EXI: EXI-0001, EXI-0004, EXI-0011, EXI-0018, EXI-0019, EXI-0020,
        EXI-0021, EXI-0022, EXI-0028, EXI-0051, EXI-0061
   VM: VM-3201 à VM-3203, VM-3205, VM-0702
   IV: IV-IF-0001, ALG-0901, ALG-0902
   IMP: IMP-0164 à IMP-0167, IMP-0169 à IMP-0173, IMP-0180 à IMP-0182,
        IMP-0188 à IMP-0191, IMP-0195, IMP-0196
   Tests: tests/composants/composants_compilateur.c
   Spécification : chapitre Compilateur §4 (interface). */

#ifndef LSE_COMPILATEUR_H
#define LSE_COMPILATEUR_H

#include "../../src/noyau/rcim.h"
#include "../../src/noyau/erreurs.h"

typedef unsigned char TypeLigneCompilee;
#define LSE_LIGNE_PROGRAMME 0   /* numérotée : EntreeObjetCompile complète (RCI-M §5) */
#define LSE_LIGNE_BUREAU    1   /* non numérotée : bloc transitoire (RCI-M IMP-0109) */

typedef struct {
    NatureErreur  nature;     /* catalogue, Gestionnaire des erreurs §7 (IMP-0195) */
    unsigned char position;   /* position dans la ligne, 0-79 (IMP-0191) */
} DiagnosticCompilation;

typedef struct {
    TypeLigneCompilee type;
    union {
        EntreeObjetCompile ligne_programme;
        HandleObjet        bloc_bureau;      /* jamais inséré au programme courant */
    } resultat;
    unsigned int longueur_bureau;            /* longueur du bloc transitoire */
} ResultatCompilation;

#define COMPILATEUR_LONGUEUR_MAX 79u   /* INRDP75 : 80 caractères, fin de ligne comprise */

/* IV-IF-0001 / VM-3201 (IMP-0171). texte : ligne reçue du moniteur ou du
   bibliothécaire, sans caractère de fin de ligne ; la flèche d'affectation
   est le caractère 95 (_), l'exponentiation le caractère 94 (^).
   Succès : renvoie 1, *resultat est renseigné ; le programme courant n'est
   pas modifié (IMP-0170).
   Échec : renvoie 0, *diagnostic est renseigné, *resultat n'est pas modifié
   et aucun objet compilé ni bloc transitoire n'a été créé ; seules des entrées
   de symboles ou de constantes partagées inertes, objet texte compris, peuvent
   subsister (§3, IMP-0181). */
int compilateur_compiler(const char *texte, unsigned int longueur,
                         ResultatCompilation *resultat,
                         DiagnosticCompilation *diagnostic);

#endif
