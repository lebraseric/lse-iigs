/* historiques_decompilation.c — aller-retour compilation, décompilation,
   recompilation sur les lignes de programme d'INRDP75 et de LSE15
   EXI: EXI-0003, EXI-0015  VM: VM-2208, VM-3201  IV: IV-IF-0001, IV-IF-0004
   IMP: IMP-0183, IMP-0198 à IMP-0201, IMP-0203
   Spécification des tests : chapitre Décompilateur §9 (IMP-0203).
   Corpus : tests/historiques/corpus/, produit par scripts/extraire_corpus.py ;
   lu depuis build/, répertoire d'exécution de « make tests ».
   Une ligne est compilée sous MITRA 15, à défaut sous T1600 ; elle est
   décompilée, recompilée et décompilée de nouveau. La seconde séquence doit
   être identique à la première, sauf si la ligne contient une constante de
   plus de six chiffres significatifs (Décompilateur §7) ; la seconde
   restitution doit être identique à la première. Les tables sont vidées
   lorsqu'un numéro de ligne ne croît pas (début d'un autre programme).
   Niveau : historique. */

#include <stdio.h>
#include <string.h>
#include "../commun/verif.h"
#include "../../src/interfaces/types_communs.h"
#include "../../src/noyau/compilateur.h"
#include "../../src/noyau/decompilateur.h"
#include "../../src/noyau/symboles.h"

segment "suite";

ConfigurationSession configuration_session = { PROFIL_MITRA15_V8, 0 };

typedef struct {
    unsigned int lignes;
    unsigned int acceptees;
    unsigned int t1600;          /* acceptées sous T1600 seulement */
    unsigned int rejetees;
    unsigned int echecs;
    unsigned int longues;        /* séquence différente, constante de plus de 6 chiffres */
    unsigned int plus_de_79;     /* restitution de plus de 79 caractères */
    unsigned int normalisees;    /* restitution différente du texte saisi */
} Bilan;

static char ligne[200];
static char texte1[DECOMPILATEUR_LONGUEUR_MAX + 1];
static char texte2[DECOMPILATEUR_LONGUEUR_MAX + 1];
static unsigned char octets1[256];

/* Plus grand nombre de chiffres significatifs d'une constante numérique
   écrite hors chaîne et hors commentaire. */
static unsigned int chiffres_max(const char *t)
{
    unsigned int i = 0;
    unsigned int max = 0;
    unsigned int n;
    int significatif;

    while (t[i] >= '0' && t[i] <= '9') i++;   /* numéro de ligne */
    while (t[i] != '\0') {
        if (t[i] == '*') break;
        if (t[i] == '\'') {
            i++;
            while (t[i] != '\0' && t[i] != '\'') i++;
            if (t[i] != '\0') i++;
            continue;
        }
        if ((t[i] >= 'A' && t[i] <= 'Z')) {          /* identificateur */
            while ((t[i] >= 'A' && t[i] <= 'Z') || (t[i] >= '0' && t[i] <= '9')) i++;
            continue;
        }
        if ((t[i] >= '0' && t[i] <= '9') || t[i] == '.') {
            n = 0;
            significatif = 0;
            while ((t[i] >= '0' && t[i] <= '9') || t[i] == '.') {
                if (t[i] >= '1' && t[i] <= '9') significatif = 1;
                if (t[i] != '.' && significatif) n++;
                i++;
            }
            if (n > max) max = n;
            continue;
        }
        i++;
    }
    return max;
}

/* Copie et libère l'objet compilé de r. */
static unsigned int recuperer(ResultatCompilation *r, unsigned char *o)
{
    unsigned char *p = objets_resoudre(r->resultat.ligne_programme.bloc);
    unsigned int n = r->resultat.ligne_programme.longueur_octets;
    memcpy(o, p, n);
    objets_relacher(r->resultat.ligne_programme.bloc);
    return n;
}

static void traiter(const char *texte, Bilan *b)
{
    ResultatCompilation r1;
    ResultatCompilation r2;
    DiagnosticCompilation d;
    ResultatDecompilation rd;
    unsigned int lg;
    unsigned int n1;
    unsigned char *p;

    configuration_session.profil = PROFIL_MITRA15_V8;
    if (!compilateur_compiler(texte, (unsigned int) strlen(texte), &r1, &d)) {
        configuration_session.profil = PROFIL_T1600_V4;
        if (!compilateur_compiler(texte, (unsigned int) strlen(texte), &r1, &d)) {
            b->rejetees++;
            return;
        }
        b->t1600++;
    }
    b->acceptees++;
    rd = decompilateur_decompiler(&r1.resultat.ligne_programme, texte1, &lg);
    n1 = recuperer(&r1, octets1);
    if (rd != DECOMP_OK) {
        printf("  [%s] decompilation en echec : %u\n", texte, rd);
        b->echecs++;
        objets_liberer(r1.resultat.ligne_programme.bloc);
        return;
    }
    if (lg > COMPILATEUR_LONGUEUR_MAX) b->plus_de_79++;
    if (strcmp(texte1, texte) != 0) b->normalisees++;
    if (!compilateur_compiler(texte1, lg, &r2, &d)) {
        printf("  [%s]\n    restitution rejetee : [%s] C %u\n", texte, texte1, erreurs_numero(d.nature));
        b->echecs++;
        objets_liberer(r1.resultat.ligne_programme.bloc);
        return;
    }
    p = objets_resoudre(r2.resultat.ligne_programme.bloc);
    if (r2.resultat.ligne_programme.longueur_octets != n1 || memcmp(p, octets1, n1) != 0) {
        if (chiffres_max(texte) > 6)
            b->longues++;
        else {
            printf("  [%s]\n    [%s] : sequence differente\n", texte, texte1);
            b->echecs++;
        }
    }
    objets_relacher(r2.resultat.ligne_programme.bloc);
    rd = decompilateur_decompiler(&r2.resultat.ligne_programme, texte2, &lg);
    if (rd != DECOMP_OK || strcmp(texte1, texte2) != 0) {
        printf("  [%s]\n    [%s] puis [%s] : restitution instable\n", texte, texte1, texte2);
        b->echecs++;
    }
    objets_liberer(r1.resultat.ligne_programme.bloc);
    objets_liberer(r2.resultat.ligne_programme.bloc);
}

static int corpus(const char *chemin, Bilan *b)
{
    FILE *f;
    unsigned int lg;
    unsigned int i;
    unsigned int numero;
    unsigned int precedent = 0;

    memset(b, 0, sizeof *b);
    f = fopen(chemin, "r");
    if (f == NULL) {
        printf("  %s : ouverture impossible\n", chemin);
        return 0;
    }
    symboles_vider();
    while (fgets(ligne, sizeof ligne, f) != NULL) {
        lg = (unsigned int) strlen(ligne);
        while (lg > 0 && (ligne[lg - 1] == '\n' || ligne[lg - 1] == '\r')) ligne[--lg] = '\0';
        if (lg == 0) continue;
        b->lignes++;
        numero = 0;
        for (i = 0; ligne[i] >= '0' && ligne[i] <= '9' && numero < 1000; i++)
            numero = numero * 10 + (unsigned int) (ligne[i] - '0');
        if (numero <= precedent) symboles_vider();
        precedent = numero;
        traiter(ligne, b);
    }
    fclose(f);
    printf("  %s : %u lignes, %u acceptees (%u sous T1600 seulement), %u rejetees, "
           "%u a constante longue, %u restituees sur plus de 79 caracteres, %u normalisees\n",
           chemin, b->lignes, b->acceptees, b->t1600, b->rejetees, b->longues, b->plus_de_79,
           b->normalisees);
    return 1;
}

int main(void)
{
    Bilan b;

    VERIF_DEBUT("historiques/decompilation");

    /* nombres constatés le 2026-10-23 : les rejets sont des tableaux de
       nombres imprimés et des fragments de lignes, non des lignes de programme */
    VERIFIER(corpus("../tests/historiques/corpus/inrdp75.txt", &b));
    VERIFIER_EGAL(0, b.echecs);
    VERIFIER_EGAL(810, b.acceptees);
    VERIFIER_EGAL(33, b.rejetees);
    VERIFIER_EGAL(1, b.normalisees);

    VERIFIER(corpus("../tests/historiques/corpus/lse15.txt", &b));
    VERIFIER_EGAL(0, b.echecs);
    VERIFIER_EGAL(57, b.acceptees);
    VERIFIER_EGAL(12, b.rejetees);
    VERIFIER_EGAL(6, b.normalisees);

    configuration_session.profil = PROFIL_MITRA15_V8;
    VERIF_FIN();
}
