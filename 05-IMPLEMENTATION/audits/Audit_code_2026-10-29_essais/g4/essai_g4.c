/* essai_g4.c — essais de l'audit du groupe 4 (copie d'audit seulement)
   objets.c, bibliotheque.c, fichiers.c, codage.c : défauts présumés,
   compteurs d'appels (verrouillages, appels de fichiers), profondeur de pile.
   Bibliothèque d'essai : tests:ESSAIG4 (vidée au début et à la fin). */

#include <stdlib.h>
#include "compilateur_outils.h"
#include "../../src/noyau/programme.h"
#include "../../src/noyau/interprete.h"
#include "../../src/noyau/contexte_exec.h"
#include "../../src/noyau/decompilateur.h"
#include "../../src/noyau/bibliotheque.h"
#include "../../src/noyau/fichiers.h"
#include "../../src/noyau/codage.h"
#include "../../src/noyau/objets.h"
#include "../../src/interfaces/interface_fichiers.h"

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

void bouchon_console(const char *const *lignes);

extern unsigned long essai_nb_alloc, essai_nb_liberer, essai_nb_verrou, essai_nb_deverrou;
extern unsigned long essai_f_ouvrir, essai_f_lire, essai_f_ecrire, essai_f_taille,
                     essai_f_fermer, essai_f_lister, essai_f_supprimer, essai_f_octets_lus;

static PileEvaluation pile;
static const char *const aucune[] = { NULL };

static void raz(void)
{
    essai_nb_alloc = essai_nb_liberer = essai_nb_verrou = essai_nb_deverrou = 0;
    essai_f_ouvrir = essai_f_lire = essai_f_ecrire = essai_f_taille = 0;
    essai_f_fermer = essai_f_lister = essai_f_supprimer = essai_f_octets_lus = 0;
}

static void compteurs(const char *quoi)
{
    printf("  [%s] HLock %lu HUnlock %lu NewHandle %lu Dispose %lu | ouvrir %lu lire %lu (%lu oct.) ecrire %lu taille %lu fermer %lu lister %lu supprimer %lu\n",
           quoi, essai_nb_verrou, essai_nb_deverrou, essai_nb_alloc, essai_nb_liberer,
           essai_f_ouvrir, essai_f_lire, essai_f_octets_lus, essai_f_ecrire, essai_f_taille,
           essai_f_fermer, essai_f_lister, essai_f_supprimer);
}

static int charger(const char *const *lignes)
{
    unsigned int i;
    NatureErreur n;
    contexte_vider();
    programme_vider();
    pile_ramener(&pile, 0);
    for (i = 0; lignes[i] != NULL; i++) {
        if (!compiler(lignes[i])) {
            printf("  [%s] rejet : C %u\n", lignes[i], erreurs_numero(d.nature));
            return 0;
        }
        if (!programme_inserer(&r.resultat.ligne_programme, &n)) return 0;
    }
    return 1;
}

static int inserer(const char *texte)
{
    NatureErreur n;
    if (!compiler(texte)) {
        printf("  [%s] rejet : C %u\n", texte, erreurs_numero(d.nature));
        return 0;
    }
    return programme_inserer(&r.resultat.ligne_programme, &n);
}

/* --- fichier RCI-S en mémoire ---------------------------------------------- */
static unsigned char img[4096];
static unsigned long img_n;

static int lire_fichier(const char *nom)
{
    FichierPlateforme f;
    if (plateforme_fichier_ouvrir(ESPACE_PROGRAMMES, nom, FICHIER_LECTURE, &f) != PLAT_OK) return 0;
    img_n = plateforme_fichier_taille(f);
    if (img_n > sizeof img) { plateforme_fichier_fermer(f); return 0; }
    plateforme_fichier_lire(f, 0, img, (unsigned int) img_n);
    plateforme_fichier_fermer(f);
    return 1;
}

static int ecrire_fichier_somme(const char *nom)
{
    FichierPlateforme f;
    unsigned long s = 0;
    unsigned long i;
    for (i = 0; i < img_n - 2; i++) s = s + img[i];
    img[img_n - 2] = (unsigned char) (s & 0xFF);
    img[img_n - 1] = (unsigned char) ((s >> 8) & 0xFF);
    if (plateforme_fichier_ouvrir(ESPACE_PROGRAMMES, nom, FICHIER_CREATION, &f) != PLAT_OK) return 0;
    plateforme_fichier_ecrire(f, 0, img, (unsigned int) img_n);
    plateforme_fichier_fermer(f);
    return 1;
}

/* --- profondeur de pile par peinture ------------------------------------- */
#define PEINTURE 2400u
static unsigned int sp_peinture;

static void peindre(void)
{
    unsigned int sp;
    unsigned int a;
    asm {
        tsc
        sta sp
    }
    sp_peinture = sp;
    for (a = sp - PEINTURE; a < sp - 32u; a++)
        *(unsigned char *) (unsigned long) a = 0xA5;
}

static unsigned int profondeur(void)
{
    unsigned int a;
    for (a = sp_peinture - PEINTURE; a < sp_peinture - 32u; a++)
        if (*(unsigned char *) (unsigned long) a != 0xA5) break;
    return sp_peinture - a;
}

static float valeur(const char *nom)
{
    ReferenceResolue ref;
    unsigned int id;
    float v = -999.0f;
    if (!symboles_chercher(nom, (unsigned char) strlen(nom), SYM_IDENTIFICATEUR, &id)) return -998.0f;
    if (contexte_resoudre(id, &ref) != ERR_AUCUNE) return -997.0f;
    if (contexte_lire_nombre(ref.cellule, USAGE_CALCUL, &v) != ERR_AUCUNE) return -996.0f;
    return v;
}

static unsigned int sp_main;

int main(void)
{
    static const char *const vide[] = { "10 X_1", "20 ", "30 TERMINER", NULL };
    static const char *const deux[] = { "10 X_5", "20 Y_X+1", "30 TERMINER", NULL };
    static const char *const autre[] = { "10 Z_9", NULL };
    static EntreeObjetCompile e;
    static char texte[DECOMPILATEUR_LONGUEUR_MAX + 2];
    unsigned int lg;
    unsigned long i;
    unsigned int k;
    NatureErreur n;
    static HandleObjet h[200];
    HandleObjet hx;
    unsigned char *o;
    static char ligne[80];
    static unsigned char contenu[600];
    unsigned char type;
    unsigned char *lu;
    unsigned long lu_lg;

    {
        unsigned int sp;
        asm {
            tsc
            sta sp
        }
        sp_main = sp;
    }
    VERIF_DEBUT("essai_g4");
    printf("  SP a l'entree de main : $%04X\n", sp_main);
    programme_initialiser();
    pile_initialiser(&pile);
    bouchon_console(aucune);
    VERIFIER_EGAL(PLAT_OK, plateforme_bibliotheque_ouvrir("tests:ESSAIG4"));
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_espace_vider(ESPACE_DONNEES);
    plateforme_espace_vider(ESPACE_TEMPORAIRES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
    {
        Identification id;
        bibliotheque_identification_chercher(1, &id);
        bibliotheque_identifier(&id);
    }

    /* 1. Ligne vide : RANGER puis APPELER (IMP-0194, IMP-0246) */
    printf("-- 1. ligne vide\n");
    VERIFIER(charger(vide));
    VERIFIER_EGAL(3, programme_nombre());
    programme_rang(1, &e);
    printf("  ligne %u : longueur compilee %u\n", e.numero_ligne, e.longueur_octets);
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("VIDE", 0, 0));
    contexte_vider();
    programme_vider();
    n = bibliotheque_appeler("VIDE", 0);
    printf("  APPELER VIDE : nature %u (%c %u), lignes %u\n", n, erreurs_classe(n),
           erreurs_numero(n), programme_nombre());
    VERIFIER_EGAL(ERR_AUCUNE, n);              /* attendu par IMP-0246 */
    VERIFIER_EGAL(3, programme_nombre());

    /* 2. Identifiant de symbole hors de la table, somme recalculée */
    printf("-- 2. identifiant de symbole hors table\n");
    VERIFIER(charger(deux));
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("DEUX", 0, 0));
    VERIFIER(lire_fichier("DEUX"));
    {
        /* première ligne : après en-tête, symboles, constantes */
        unsigned long p = 8;
        unsigned int ns = img[p] | (img[p + 1] << 8);
        unsigned int nc;
        p = p + 2 + 8ul * ns;
        nc = img[p] | (img[p + 1] << 8);
        p = p + 2;
        for (k = 0; k < nc; k++) p = p + 1 + img[p];
        p = p + 2;                               /* nombre de lignes */
        printf("  ns %u nc %u ; ligne %u, longueur %u :", ns, nc, img[p], img[p + 1]);
        for (k = 0; k < img[p + 1]; k++) printf(" %02X", img[p + 2 + k]);
        printf("\n");
        for (k = 0; k < img[p + 1]; k++)
            if (img[p + 2 + k] == 0x40) {        /* RCIM_REF_SIMPLE id */
                img[p + 3 + k] = 0x09;           /* symbole 9 : absent de la table */
                img[p + 4 + k] = 0x00;
                break;
            }
    }
    VERIFIER(ecrire_fichier_somme("DEUX"));
    VERIFIER(charger(autre));
    n = bibliotheque_appeler("DEUX", 0);
    printf("  APPELER DEUX altere : nature %u (%c %u), lignes %u\n", n, erreurs_classe(n),
           erreurs_numero(n), programme_nombre());
    VERIFIER(n != ERR_AUCUNE);                 /* IMP-0246 : identifiants vérifiés */
    if (n == ERR_AUCUNE) {
        programme_rang(0, &e);
        printf("  decompilation ligne 10 : %d\n", decompilateur_decompiler(&e, texte, &lg));
    }

    /* 3. Symbole en double : la restauration échoue après avoir vidé le programme */
    printf("-- 3. symbole en double\n");
    VERIFIER(charger(deux));
    plateforme_fichier_supprimer(ESPACE_PROGRAMMES, "DEUX");
    bibliotheque_decataloguer(CATALOGUE_PROGRAMME, "DEUX");
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("DEUX", 0, 0));
    VERIFIER(lire_fichier("DEUX"));
    printf("  symbole 0 : %c, symbole 1 : %c\n", img[11], img[19]);
    img[19] = img[11];                           /* Y devient X */
    VERIFIER(ecrire_fichier_somme("DEUX"));
    VERIFIER(charger(autre));
    n = bibliotheque_appeler("DEUX", 0);
    printf("  APPELER DEUX (X en double) : nature %u (%c %u), lignes %u\n", n, erreurs_classe(n),
           erreurs_numero(n), programme_nombre());
    VERIFIER_EGAL(1, programme_nombre());       /* VM-3403 : programme courant intact */

    /* 4. Verrouillages : objets_creer selon le nombre de blocs pleins */
    printf("-- 4. objets_creer et blocs pleins\n");
    contexte_vider();
    programme_vider();
    for (k = 0; k < 192; k++) h[k] = objets_creer(16);
    raz();
    hx = objets_creer(16);
    compteurs("objets_creer apres 3 blocs pleins");
    objets_liberer(hx);
    raz();
    o = objets_resoudre(h[0]);
    objets_relacher(h[0]);
    compteurs("objets_resoudre + objets_relacher");
    raz();
    objets_liberer(h[191]);
    compteurs("objets_liberer");
    for (k = 0; k < 191; k++) objets_liberer(h[k]);

    /* 5. Programme de 200 lignes : saisie, RANGER, APPELER */
    printf("-- 5. programme de 200 lignes\n");
    contexte_vider();
    programme_vider();
    raz();
    for (k = 1; k <= 200; k++) {
        sprintf(ligne, "%u X%u_X%u+%u", k, k % 40, (k + 1) % 40, k);
        if (!inserer(ligne)) break;
    }
    compteurs("saisie de 200 lignes");
    raz();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_ranger("P200", 0, 0));
    compteurs("RANGER P200");
    raz();
    VERIFIER_EGAL(ERR_AUCUNE, bibliotheque_appeler("P200", 0));
    compteurs("APPELER P200");
    VERIFIER_EGAL(200, programme_nombre());

    /* 6. DECODER du programme de 200 lignes */
    printf("-- 6. DECODER, ENCODER\n");
    raz();
    VERIFIER_EGAL(ERR_AUCUNE, codage_decoder("DP", 1, 0));
    compteurs("DECODER P200");
    raz();
    VERIFIER_EGAL(ERR_AUCUNE, codage_ouvrir("DP"));
    k = 0;
    while (codage_ligne(texte, 255, &lg, &n)) k++;
    codage_fermer();
    compteurs("ENCODER (lecture seule) DP");
    printf("  %u lignes relues\n", k);

    /* 7. GARER, CHARGER : appels par enregistrement */
    printf("-- 7. GARER, CHARGER\n");
    memset(contenu, 'A', sizeof contenu);
    raz();
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_garer("T1", 0, 1, 3, contenu, 10));
    compteurs("GARER temporaire, fichier cree");
    raz();
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_garer("T1", 0, 2, 3, contenu, 10));
    compteurs("GARER temporaire, fichier existant (2 temporaires presents)");
    raz();
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_garer("PERM", 1, 1, 3, contenu, 10));
    compteurs("GARER permanent, fichier cree");
    raz();
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_garer("PERM", 1, 2, 3, contenu, 10));
    compteurs("GARER permanent, fichier existant");
    raz();
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_charger("PERM", 1, 2, &type, &lu, &lu_lg));
    free(lu);
    compteurs("CHARGER permanent");
    raz();
    VERIFIER_EGAL(ERR_AUCUNE, fichiers_charger("PERM", 0, 2, &type, &lu, &lu_lg));
    free(lu);
    compteurs("CHARGER sans # (temporaire absent)");

    /* 8. Profondeur de pile */
    printf("-- 8. profondeur de pile (sous le point de mesure)\n");
    for (k = 3; k <= 21; k++) fichiers_garer("T2", 0, k, 3, contenu, 10);
    peindre();
    n = fichiers_garer("T2", 0, 22, 3, contenu, 10);    /* 21e entree : nouveau secteur de repertoire */
    printf("  GARER (nouveau repertoire) : %u octets, nature %u\n", profondeur(), n);
    peindre();
    n = fichiers_garer("T2", 0, 3, 3, contenu, 600);    /* remplacement, ancienne etendue a placer */
    printf("  GARER (remplacement) : %u octets, nature %u\n", profondeur(), n);
    peindre();
    n = fichiers_cataloguer("T2", "PERM2", 0);
    printf("  CATALOGUER : %u octets, nature %u\n", profondeur(), n);
    peindre();
    n = codage_decoder("DQ", 1, 0);
    printf("  DECODER : %u octets, nature %u\n", profondeur(), n);
    peindre();
    n = bibliotheque_appeler("P200", 0);
    printf("  APPELER : %u octets, nature %u\n", profondeur(), n);
    peindre();
    n = bibliotheque_ranger("P201", 0, 0);
    printf("  RANGER : %u octets, nature %u\n", profondeur(), n);
    {
        char nom[6];
        unsigned char compte;
        unsigned char prot;
        unsigned char date[3];
        peindre();
        bibliotheque_catalogue_suivant(CATALOGUE_PROGRAMME, "", 0, -1, nom, &compte, &prot, date);
        printf("  catalogue_suivant : %u octets\n", profondeur());
    }
    peindre();
    programme_rang(0, &e);
    k = decompilateur_decompiler(&e, texte, &lg);
    printf("  decompilation d'une ligne de P200 : %u octets\n", profondeur());
    {
        static const char *const pg[] = { "10 X_1", "20 GARER X,1,'TG'", "30 CHARGER Y,1,'TG'", "40 TERMINER", NULL };
        static DiagnosticExecution dg;
        static PositionExecution ps;
        FinExecution f;
        VERIFIER(charger(pg));
        contexte_initialiser(0, 0, &ps);
        interprete_reinitialiser();
        peindre();
        f = interprete_executer(&pile, &ps, &dg);
        printf("  execution GARER/CHARGER par l'interpreteur : %u octets, fin %u\n", profondeur(), f);
    }
    printf("  SP au point de mesure : $%04X, soit %u octets sous main\n",
           sp_peinture, sp_main - sp_peinture);

    /* 9. GARER puis CHARGER d'un tableau de 20000 elements (image > 65535 octets) */
    printf("-- 9. tableau de 20000 elements\n");
    {
        static const char *const pt[] = { "10 TABLEAU T[20000]", "20 T[1]_5;T[20000]_7;T[16000]_9",
            "30 GARER T,1,'#TT'", "40 CHARGER U,1,'#TT'", "50 A_U[1];B_U[20000];C_U[16000]", "60 TERMINER", NULL };
        static DiagnosticExecution dg;
        static PositionExecution ps;
        FinExecution f;
        VERIFIER(charger(pt));
        contexte_initialiser(0, 0, &ps);
        interprete_reinitialiser();
        f = interprete_executer(&pile, &ps, &dg);
        printf("  fin %u, EX %u ligne %u ; A=%g B=%g C=%g\n", f, erreurs_numero(dg.nature), dg.ligne,
               (double) valeur("A"), (double) valeur("B"), (double) valeur("C"));
        VERIFIER(f == FIN_TERMINER && valeur("A") == 5.0f && valeur("B") == 7.0f && valeur("C") == 9.0f);
    }

    /* 10. Longueur restituee d'une ligne de 79 caracteres (DECODER puis ENCODER) */
    printf("-- 10. lignes restituees plus longues que la source\n");
    {
        static const char *const src[] = {
            "1 X_1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1",
            "1 X_.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5+.5",
            "1 X_1E-3+1E-3+1E-3+1E-3+1E-3+1E-3+1E-3+1E-3+1E-3+1E-3+1E-3+1E-3+1E-3+1E-3+1E-3",
            "1 X_A+B+C+D+E+F+G+H+I+J+K+L+M+N+O+P+Q+R+S+T+U+V+W+X+Y+Z+A+B+C+D+E+F+G+H+I+J+K+L",
            NULL };
        unsigned int q;
        for (q = 0; src[q] != NULL; q++) {
            contexte_vider();
            programme_vider();
            if (!inserer(src[q])) continue;
            programme_rang(0, &e);
            decompilateur_decompiler(&e, texte, &lg);
            printf("  source %u car. -> restituee %u car. : %s\n", (unsigned int) strlen(src[q]), lg, texte);
            VERIFIER(lg <= COMPILATEUR_LONGUEUR_MAX);
        }
    }

    /* 11. Verrouillages par tour d'une boucle de chaines, avec et sans 190 lignes devant */
    printf("-- 11. boucle de chaines\n");
    {
        static DiagnosticExecution dg;
        static PositionExecution ps;
        FinExecution f;
        unsigned int avec;
        for (avec = 0; avec <= 1; avec++) {
            contexte_vider();
            programme_vider();
            if (avec)
                for (k = 1; k <= 190; k++) {
                    sprintf(ligne, "%u X%u_%u", k, k % 40, k);
                    inserer(ligne);
                }
            inserer("200 CHAINE C;C_''");
            inserer("210 FAIRE 220 POUR I_1 JUSQUA 100");
            inserer("220 C_C!'A'");
            inserer("230 TERMINER");
            contexte_initialiser(200, 0, &ps);
            interprete_reinitialiser();
            raz();
            f = interprete_executer(&pile, &ps, &dg);
            printf("  %s : fin %u ; par tour : HLock %lu.%02lu NewHandle %lu Dispose %lu\n",
                   avec ? "190 lignes devant" : "programme seul", f,
                   essai_nb_verrou / 100, essai_nb_verrou % 100, essai_nb_alloc, essai_nb_liberer);
        }
    }

    contexte_vider();
    programme_vider();
    plateforme_espace_vider(ESPACE_PROGRAMMES);
    plateforme_espace_vider(ESPACE_DONNEES);
    plateforme_espace_vider(ESPACE_TEMPORAIRES);
    plateforme_fichier_supprimer(ESPACE_RACINE, "CATALOGUE");
    objets_terminer();
    VERIF_FIN();
}
