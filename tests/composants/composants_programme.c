/* composants_programme.c — programme courant
   EXI: EXI-0002, EXI-0004, EXI-0006, EXI-0042  VM: VM-2203, VM-3204
   IV: IV-IF-0002
   IMP: IMP-0106, IMP-0147, IMP-0176, IMP-0204 à IMP-0207
   Spécification des tests : chapitre Programme courant §10.
   Niveau : composants. Les lignes sont compilées par le compilateur réel. */

#include "compilateur_outils.h"
#include "../../src/noyau/programme.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

/* Compile une ligne et l'insère ; renvoie son bloc, ou GESTOBJ_HANDLE_NUL. */
static HandleObjet entrer(const char *texte)
{
    NatureErreur n;
    if (!compiler(texte)) {
        printf("  [%s] rejet inattendu : C %u\n", texte, erreurs_numero(d.nature));
        return GESTOBJ_HANDLE_NUL;
    }
    if (!programme_inserer(&r.resultat.ligne_programme, &n)) {
        printf("  [%s] insertion refusee\n", texte);
        objets_liberer(r.resultat.ligne_programme.bloc);
        return GESTOBJ_HANDLE_NUL;
    }
    return r.resultat.ligne_programme.bloc;
}

static unsigned char numero_de(int (*f)(unsigned char, EntreeObjetCompile *), unsigned char n)
{
    EntreeObjetCompile e;
    return f(n, &e) ? e.numero_ligne : 0;
}

static InfoProcedure info(const char *nom)
{
    unsigned int id;
    EntreeSymbole e;
    InfoProcedure vide = { 0, 0 };
    if (!symboles_chercher(nom, (unsigned char) strlen(nom), SYM_PROCEDURE, &id)) return vide;
    symboles_lire(id, &e);
    return e.procedure;
}

int main(void)
{
    EntreeObjetCompile e;
    HandleObjet h20;
    HandleObjet h20b;
    HandleObjet h;
    unsigned long avant;
    unsigned int i;
    int ordre;
    const TableParametresFormels *t;
    unsigned int idA;
    unsigned int idB;
    unsigned int idM;
    unsigned int idN;
    NatureErreur nature;

    VERIF_DEBUT("composants/programme");
    VERIFIER(programme_initialiser());
    VERIFIER_EGAL(0, programme_nombre());
    VERIFIER(!programme_premiere(&e));

    /* --- ajout dans le désordre, ordre et unicité (IMP-0204) ------------------ */
    symboles_vider();
    VERIFIER(entrer("30 C_3") != GESTOBJ_HANDLE_NUL);
    VERIFIER((h20 = entrer("20 B_2")) != GESTOBJ_HANDLE_NUL);
    VERIFIER(entrer("250 TERMINER") != GESTOBJ_HANDLE_NUL);
    VERIFIER(entrer("1 A_1") != GESTOBJ_HANDLE_NUL);
    VERIFIER_EGAL(4, programme_nombre());
    ordre = 1;
    for (i = 1; i < programme_nombre(); i++) {
        EntreeObjetCompile a;
        programme_rang(i - 1, &a);
        programme_rang(i, &e);
        if (a.numero_ligne >= e.numero_ligne) ordre = 0;
    }
    VERIFIER(ordre);
    VERIFIER(!programme_rang(4, &e));
    VERIFIER_EGAL(4, programme_lignes_entrees());

    /* --- recherches -------------------------------------------------------- */
    VERIFIER(programme_premiere(&e) && e.numero_ligne == 1);
    VERIFIER(programme_chercher(20, &e) && e.bloc == h20);
    VERIFIER(!programme_chercher(21, &e));
    VERIFIER_EGAL(20, numero_de(programme_suivante, 1));
    VERIFIER_EGAL(30, numero_de(programme_suivante, 20));
    VERIFIER_EGAL(30, numero_de(programme_suivante, 25));
    VERIFIER_EGAL(1, numero_de(programme_suivante, 0));
    VERIFIER_EGAL(0, numero_de(programme_suivante, 250));
    VERIFIER_EGAL(0, numero_de(programme_suivante, 255));
    VERIFIER_EGAL(20, numero_de(programme_au_plus, 29));
    VERIFIER_EGAL(30, numero_de(programme_au_plus, 30));
    VERIFIER_EGAL(250, numero_de(programme_au_plus, 255));
    VERIFIER_EGAL(0, numero_de(programme_au_plus, 0));

    /* --- remplacement : ancien bloc libéré, compteur avancé (IMP-0205, 0207) -- */
    avant = programme_lignes_entrees();
    VERIFIER((h20b = entrer("20 B_22")) != GESTOBJ_HANDLE_NUL);
    VERIFIER_EGAL(4, programme_nombre());
    VERIFIER(!objets_existe(h20) || h20 == h20b);
    VERIFIER(programme_chercher(20, &e) && e.bloc == h20b);
    VERIFIER_EGAL(avant + 1, programme_lignes_entrees());

    /* --- suppression : compteur inchangé ----------------------------------- */
    avant = programme_lignes_entrees();
    VERIFIER(programme_supprimer(30));
    VERIFIER(!programme_supprimer(30));
    VERIFIER_EGAL(3, programme_nombre());
    VERIFIER(!programme_chercher(30, &e));
    VERIFIER_EGAL(avant, programme_lignes_entrees());

    /* --- numéro 0 refusé, sans modification ---------------------------------- */
    e.numero_ligne = 0;
    e.longueur_octets = 0;
    e.bloc = GESTOBJ_HANDLE_NUL;
    VERIFIER(!programme_inserer(&e, &nature));
    VERIFIER_EGAL(3, programme_nombre());

    /* --- déclaration : décodage de 0xCD (IMP-0147) --------------------------- */
    VERIFIER(entrer("40 PROCEDURE &PRO(A,B) LOCAL A,M,N") != GESTOBJ_HANDLE_NUL);
    t = programme_parametres(40);
    VERIFIER(t != NULL);
    symboles_chercher("A", 1, SYM_IDENTIFICATEUR, &idA);
    symboles_chercher("B", 1, SYM_IDENTIFICATEUR, &idB);
    symboles_chercher("M", 1, SYM_IDENTIFICATEUR, &idM);
    symboles_chercher("N", 1, SYM_IDENTIFICATEUR, &idN);
    if (t != NULL) {
        VERIFIER_EGAL(2, t->nb_parametres);
        VERIFIER_EGAL(idA, t->parametres[0].symbole);
        VERIFIER_EGAL(PARAM_LOCAL, t->parametres[0].mode);
        VERIFIER_EGAL(idB, t->parametres[1].symbole);
        VERIFIER_EGAL(PARAM_REFERENCE, t->parametres[1].mode);
        VERIFIER_EGAL(2, t->nb_locaux_non_parametres);
        VERIFIER_EGAL(idM, t->locaux_non_parametres[0]);
        VERIFIER_EGAL(idN, t->locaux_non_parametres[1]);
    }
    VERIFIER_EGAL(40, info("PRO").numero_ligne_entree);
    VERIFIER_EGAL(2, info("PRO").arite);
    VERIFIER(programme_parametres(20) == NULL);

    /* --- LSE15 §7-6, Note : la déclaration de plus grand numéro (IMP-0176) ---- */
    VERIFIER(entrer("50 PROCEDURE &P(X,Y)") != GESTOBJ_HANDLE_NUL);
    VERIFIER(entrer("10 PROCEDURE &P(X)") != GESTOBJ_HANDLE_NUL);
    VERIFIER_EGAL(50, info("P").numero_ligne_entree);
    VERIFIER_EGAL(2, info("P").arite);
    VERIFIER(programme_supprimer(50));
    VERIFIER_EGAL(10, info("P").numero_ligne_entree);
    VERIFIER_EGAL(1, info("P").arite);
    VERIFIER(programme_parametres(50) == NULL);
    VERIFIER(entrer("10 X_1") != GESTOBJ_HANDLE_NUL);          /* remplacement */
    VERIFIER_EGAL(0, info("P").numero_ligne_entree);
    VERIFIER(programme_parametres(10) == NULL);
    VERIFIER(entrer("10 PROCEDURE &P() LOCAL Z;RETOUR") != GESTOBJ_HANDLE_NUL);
    VERIFIER_EGAL(10, info("P").numero_ligne_entree);
    VERIFIER_EGAL(0, info("P").arite);
    VERIFIER((t = programme_parametres(10)) != NULL && t->nb_locaux_non_parametres == 1);

    /* --- effacement total (IMP-0206) ---------------------------------------- */
    programme_rang(0, &e);
    h = e.bloc;
    programme_vider();
    VERIFIER_EGAL(0, programme_nombre());
    VERIFIER(!objets_existe(h));
    VERIFIER(programme_parametres(10) == NULL);
    VERIFIER_EGAL(0, symboles_nombre());
    VERIFIER_EGAL(0, constantes_nombre());
    VERIFIER(entrer("5 A_1") != GESTOBJ_HANDLE_NUL);
    VERIFIER_EGAL(1, programme_nombre());
    programme_vider();

    VERIF_FIN();
}
