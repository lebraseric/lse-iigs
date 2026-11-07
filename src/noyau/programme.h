/* programme.h — programme courant : table des lignes compilées, table des
   paramètres formels, déclaration de procédure en vigueur
   EXI: EXI-0002, EXI-0004, EXI-0006, EXI-0042
   VM: VM-2203, VM-3204, VM-1301
   IV: IV-IF-0002, ALG-1002
   IMP: IMP-0106 (recherche dichotomique), IMP-0147 (table des paramètres
        formels), IMP-0176 (INTEGRER_DECLARATION), IMP-0204 à IMP-0207
   Tests: tests/composants/composants_programme.c
   Spécification : chapitre Programme courant ; Compilateur §13.3, §13.5. */

#ifndef LSE_PROGRAMME_H
#define LSE_PROGRAMME_H

#include "../../src/noyau/rcim.h"
#include "../../src/noyau/erreurs.h"

#define PROGRAMME_CAPACITE 255u   /* IMP-0204 */

/* --- table des paramètres formels (Compilateur §13.3, IMP-0147) --- */
typedef unsigned char ModeParametre;
#define PARAM_LOCAL      0
#define PARAM_REFERENCE  1

typedef struct {
    unsigned short   symbole;   /* identifiant du nom formel */
    ModeParametre    mode;
} ParametreFormel;

typedef struct {
    unsigned char     numero_ligne_entree;   /* clé de correspondance avec InfoProcedure */
    unsigned short    symbole_procedure;     /* identifiant du nom &ID déclaré */
    ParametreFormel  *parametres;            /* ordre du texte */
    unsigned char     nb_parametres;
    unsigned short   *locaux_non_parametres; /* ordre du texte */
    unsigned char     nb_locaux_non_parametres;
} TableParametresFormels;

/* --- interface (IMP-0205) --- */
int  programme_initialiser(void);
/* IV-IF-0002. Succès : 1, le programme courant possède le bloc ; l'ancien
   bloc de même numéro est libéré. Échec : 0, *nature renseignée
   (ERR_C_PLUS_DE_PLACE, ou ERR_AUCUNE pour un numéro hors de 1 à 255),
   rien n'est modifié et le bloc reste à l'appelant. */
int  programme_inserer(const EntreeObjetCompile *ligne, NatureErreur *nature);
int  programme_supprimer(unsigned char numero);
int  programme_chercher(unsigned char numero, EntreeObjetCompile *copie);
int  programme_premiere(EntreeObjetCompile *copie);
int  programme_suivante(unsigned char numero, EntreeObjetCompile *copie);
int  programme_au_plus(unsigned char numero, EntreeObjetCompile *copie);
unsigned int programme_nombre(void);
int  programme_rang(unsigned int rang, EntreeObjetCompile *copie);
const TableParametresFormels *programme_parametres(unsigned char ligne_entree);
/* Efface tout le programme et vide les tables des symboles et des
   constantes (IMP-0206) ; l'appelant réinitialise l'environnement. */
void programme_vider(void);
unsigned long programme_lignes_entrees(void);   /* IMP-0207 */

#endif
