/* composants_symboles.c — table des symboles et table des constantes partagées
   EXI: EXI-0001, EXI-0016  VM: VM-2401  IV: ANN-K-0009, ANN-K-0011
   IMP: IMP-0104, IMP-0107, IMP-0109, IMP-0114, IMP-0124, IMP-0127,
        IMP-0167, IMP-0181
   Spécification des tests : chapitre Compilateur §19 (IMP-0167, IMP-0181) ;
   RCI-M §6, §7.
   Niveau : composants. Liée avec src/noyau/symboles.c, src/noyau/objets.c
   et src/plateforme/memoire_iigs.c (aucun bouchon).

   Plafond de la table des symboles (6 553 entrées, IMP-0104) atteint par
   symboles_ajouter. Non couvert : le plafond de la table des constantes
   (21 845 entrées), un objet par constante. */

#include <string.h>
#include "../commun/verif.h"
#include "../../src/interfaces/profil.h"
#include "../../src/noyau/symboles.h"

#define TXT(s) ((const unsigned char *) (s))

int main(void)
{
    unsigned int a, b, a2, f_proc, f_ident, x, y, k1, k2, k3, k4, i;
    char nom[4];
    EntreeSymbole s;
    EntreeConstante cst;
    InfoProcedure info;
    unsigned char *p;
    HandleObjet texte;

    VERIF_DEBUT("composants/symboles");

    VERIFIER_EGAL(10, sizeof(EntreeSymbole));
    VERIFIER_EGAL(3, sizeof(EntreeConstante));

    /* Recherche et insertion (IMP-0167) : un nom déjà rencontré garde son id */
    VERIFIER_EGAL(SYMB_OK, symboles_chercher_ou_creer("A", 1, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &a));
    VERIFIER_EGAL(SYMB_OK, symboles_chercher_ou_creer("BETA", 4, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &b));
    VERIFIER_EGAL(SYMB_OK, symboles_chercher_ou_creer("A", 1, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &a2));
    VERIFIER_EGAL(0, a);
    VERIFIER_EGAL(1, b);
    VERIFIER_EGAL(a, a2);
    VERIFIER_EGAL(2, symboles_nombre());

    /* &F et F sont deux symboles distincts ; le & n'est pas stocké (IMP-0124, IMP-0167) */
    VERIFIER_EGAL(SYMB_OK, symboles_chercher_ou_creer("F", 1, SYM_PROCEDURE, ORIGINE_PROGRAMME, &f_proc));
    VERIFIER_EGAL(SYMB_OK, symboles_chercher_ou_creer("F", 1, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &f_ident));
    VERIFIER(f_proc != f_ident);
    VERIFIER(symboles_lire(f_proc, &s));
    VERIFIER_EGAL(SYM_PROCEDURE, s.nature);
    VERIFIER_EGAL(1, s.nom_longueur);
    VERIFIER_EGAL('F', s.nom[0]);
    VERIFIER_EGAL(0, s.procedure.numero_ligne_entree);   /* {0, 0} : aucune déclaration */
    VERIFIER_EGAL(0, s.procedure.arite);

    /* symboles_chercher ne crée rien */
    VERIFIER(!symboles_chercher("ZZ", 2, SYM_IDENTIFICATEUR, &x));
    VERIFIER_EGAL(4, symboles_nombre());

    /* Nom de 5 caractères ; noms de longueur invalide refusés */
    VERIFIER_EGAL(SYMB_OK, symboles_chercher_ou_creer("ALPHA", 5, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &x));
    VERIFIER(symboles_lire(x, &s));
    VERIFIER(memcmp(s.nom, "ALPHA", 5) == 0);
    VERIFIER_EGAL(SYMB_ERR_ARGUMENT, symboles_chercher_ou_creer("ALPHAB", 6, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &x));
    VERIFIER_EGAL(SYMB_ERR_ARGUMENT, symboles_chercher_ou_creer("", 0, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &x));

    /* InfoProcedure renseignée par le programme courant (IMP-0114, IMP-0176) */
    info.numero_ligne_entree = 25;
    info.arite = 1;
    symboles_definir_procedure(f_proc, info);
    VERIFIER(symboles_lire(f_proc, &s));
    VERIFIER_EGAL(25, s.procedure.numero_ligne_entree);
    VERIFIER_EGAL(1, s.procedure.arite);
    symboles_definir_procedure(f_ident, info);            /* sans effet sur un identificateur */
    VERIFIER(symboles_lire(f_ident, &s));
    VERIFIER_EGAL(0, s.procedure.numero_ligne_entree);

    /* Origine (IMP-0109) : machine de bureau, puis programme ; jamais l'inverse */
    symboles_chercher_ou_creer("VA", 2, SYM_IDENTIFICATEUR, ORIGINE_MACHINE_BUREAU, &x);
    VERIFIER(symboles_lire(x, &s));
    VERIFIER_EGAL(ORIGINE_MACHINE_BUREAU, s.origine);
    symboles_chercher_ou_creer("VA", 2, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &x);
    VERIFIER(symboles_lire(x, &s));
    VERIFIER_EGAL(ORIGINE_PROGRAMME, s.origine);
    symboles_chercher_ou_creer("VB", 2, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &y);
    symboles_chercher_ou_creer("VB", 2, SYM_IDENTIFICATEUR, ORIGINE_MACHINE_BUREAU, &y);
    VERIFIER(symboles_lire(y, &s));
    VERIFIER_EGAL(ORIGINE_PROGRAMME, s.origine);

    /* Lecture d'un identifiant inexistant */
    VERIFIER(!symboles_lire(symboles_nombre(), &s));

    /* Constantes partagées (IMP-0107, IMP-0181) : même texte, même index */
    VERIFIER_EGAL(SYMB_OK, constantes_chercher_ou_creer(TXT("MESSAGE TRES LONG POUR LE PARTAGE"), 33, &k1));
    VERIFIER_EGAL(SYMB_OK, constantes_chercher_ou_creer(TXT("MESSAGE TRES LONG POUR LE PARTAGE"), 33, &k2));
    VERIFIER_EGAL(k1, k2);
    VERIFIER_EGAL(1, constantes_nombre());
    /* même longueur, texte différent ; même début, longueur différente */
    VERIFIER_EGAL(SYMB_OK, constantes_chercher_ou_creer(TXT("MESSAGE TRES LONG POUR LE PARTAGF"), 33, &k3));
    VERIFIER_EGAL(SYMB_OK, constantes_chercher_ou_creer(TXT("MESSAGE TRES LONG POUR LE PARTAG"), 32, &k4));
    VERIFIER(k3 != k1);
    VERIFIER(k4 != k1 && k4 != k3);
    VERIFIER_EGAL(3, constantes_nombre());
    /* texte conservé dans un objet du gestionnaire d'objets */
    VERIFIER(constantes_lire(k1, &cst));
    VERIFIER_EGAL(33, cst.longueur);
    p = objets_resoudre(cst.texte);
    VERIFIER(p != NULL && memcmp(p, "MESSAGE TRES LONG POUR LE PARTAGE", 33) == 0);
    objets_relacher(cst.texte);
    texte = cst.texte;
    /* une chaîne courte n'est jamais partagée ; seuil 16 inclus */
    VERIFIER_EGAL(SYMB_ERR_ARGUMENT, constantes_chercher_ou_creer(TXT("COURT"), 5, &k2));
    VERIFIER_EGAL(SYMB_ERR_ARGUMENT, constantes_chercher_ou_creer(TXT("SEIZE CARACTERES"), 16, &k2));
    VERIFIER_EGAL(SYMB_OK, constantes_chercher_ou_creer(TXT("DIX-SEPT CARACTER"), 17, &k2));

    /* Vidage des deux tables, objets texte libérés (IMP-0107) */
    symboles_vider();
    VERIFIER_EGAL(0, symboles_nombre());
    VERIFIER_EGAL(0, constantes_nombre());
    VERIFIER(!objets_existe(texte));
    VERIFIER_EGAL(SYMB_OK, symboles_chercher_ou_creer("B", 1, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &b));
    VERIFIER_EGAL(0, b);
    symboles_vider();

    /* Plafonds : un tableau alloué ne dépasse pas 64 Kio en modèle small (IMP-0104 ;
       audit du code, AC-32) ; ajout sans recherche (IMP-0246 ; AP-12) */
    VERIFIER_EGAL(6553u, SYMBOLES_CAPACITE_MAX);
    VERIFIER_EGAL(21845u, CONSTANTES_CAPACITE_MAX);
    nom[3] = '\0';
    for (i = 0; i < SYMBOLES_CAPACITE_MAX; i++) {
        nom[0] = (char) ('A' + i / 676u % 26u);
        nom[1] = (char) ('A' + i / 26u % 26u);
        nom[2] = (char) ('A' + i % 26u);
        if (symboles_ajouter(nom, 3, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &x) != SYMB_OK || x != i) break;
    }
    VERIFIER_EGAL(SYMBOLES_CAPACITE_MAX, i);
    VERIFIER_EGAL(SYMB_ERR_CAPACITE, symboles_ajouter("ZZZZ", 4, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &x));
    VERIFIER_EGAL(SYMB_ERR_CAPACITE, symboles_chercher_ou_creer("ZZZZ", 4, SYM_IDENTIFICATEUR,
                                                                ORIGINE_PROGRAMME, &x));
    VERIFIER_EGAL(SYMB_OK, symboles_chercher_ou_creer(nom, 3, SYM_IDENTIFICATEUR, ORIGINE_PROGRAMME, &x));
    VERIFIER_EGAL(SYMBOLES_CAPACITE_MAX - 1, x);                /* le dernier, retrouvé */
    VERIFIER(symboles_lire(SYMBOLES_CAPACITE_MAX - 1, &s) && memcmp(s.nom, nom, 3) == 0);
    VERIFIER_EGAL(SYMB_ERR_ARGUMENT, constantes_ajouter(TXT("COURT"), 5, &k1));
    VERIFIER_EGAL(SYMB_OK, constantes_ajouter(TXT("DIX-SEPT CARACTER"), 17, &k1));
    VERIFIER_EGAL(SYMB_OK, constantes_ajouter(TXT("DIX-SEPT CARACTER"), 17, &k2));
    VERIFIER(k1 == 0 && k2 == 1);                                /* aucune recherche */
    symboles_vider();

    VERIF_FIN();
}
