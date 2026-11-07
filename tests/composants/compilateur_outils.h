/* compilateur_outils.h — outils communs aux suites du compilateur
   IMP: IMP-0192 (forme des suites)
   Inclus une seule fois, par le fichier qui contient main. Les suites sont
   séparées parce que le segment principal d'un exécutable est limité à
   64 Kio sous le modèle mémoire small (données statiques comprises). */

#ifndef LSE_COMPILATEUR_OUTILS_H
#define LSE_COMPILATEUR_OUTILS_H

#include <string.h>
#include "../commun/verif.h"
#include "../../src/interfaces/profil.h"
#include "../../src/interfaces/types_communs.h"
#include "../../src/noyau/compilateur.h"
#include "../../src/noyau/symboles.h"

/* Code des suites hors du segment principal : celui-ci est limité à 64 Kio
   (modèle mémoire small), et le profil dev y ajoute le code de débogage. */
segment "suite";


static ResultatCompilation r;
static DiagnosticCompilation d;
static unsigned char obtenu[256];
static unsigned int lg_obtenu;

static int compiler(const char *texte)
{
    return compilateur_compiler(texte, (unsigned int) strlen(texte), &r, &d);
}

/* Copie la séquence produite et libère l'objet compilé. */
static void recuperer(void)
{
    HandleObjet h;
    unsigned char *p;

    if (r.type == LSE_LIGNE_PROGRAMME) {
        h = r.resultat.ligne_programme.bloc;
        lg_obtenu = r.resultat.ligne_programme.longueur_octets;
    } else {
        h = r.resultat.bloc_bureau;
        lg_obtenu = r.longueur_bureau;
    }
    p = objets_resoudre(h);
    memcpy(obtenu, p, lg_obtenu);
    objets_relacher(h);
    objets_liberer(h);
}

static void afficher_octets(const char *quoi, const unsigned char *o, unsigned int n)
{
    unsigned int i;
    printf("    %s ", quoi);
    for (i = 0; i < n; i++) printf("%02X ", o[i]);
    printf("\n");
}

/* Compile texte (tables vidées) et compare la séquence à attendu. */
static int seq(const char *texte, const unsigned char *attendu, unsigned int n)
{
    symboles_vider();
    if (!compiler(texte)) {
        printf("  [%s] rejet inattendu : C %u, position %u\n", texte,
               erreurs_numero(d.nature), d.position);
        return 0;
    }
    recuperer();
    if (lg_obtenu != n || memcmp(obtenu, attendu, n) != 0) {
        printf("  [%s]\n", texte);
        afficher_octets("obtenu ", obtenu, lg_obtenu);
        afficher_octets("attendu", attendu, n);
        return 0;
    }
    return 1;
}

/* Compile texte (tables vidées) et attend un rejet de la nature donnée, à la
   position donnée (position < 0 : non vérifiée). *r doit rester inchangé. */
static int rejet(const char *texte, NatureErreur nature, int position)
{
    ResultatCompilation copie;
    int ok;

    symboles_vider();
    memset(&r, 0xAA, sizeof r);
    memcpy(&copie, &r, sizeof r);
    if (compiler(texte)) {
        recuperer();
        printf("  [%s] accepte, rejet attendu (C %u)\n", texte, erreurs_numero(nature));
        return 0;
    }
    ok = d.nature == nature && (position < 0 || d.position == position)
         && memcmp(&r, &copie, sizeof r) == 0;
    if (!ok)
        printf("  [%s] nature %u (C %u) position %u ; attendu nature %u (C %u) position %d\n",
               texte, d.nature, erreurs_numero(d.nature), d.position,
               nature, erreurs_numero(nature), position);
    return ok;
}

/* Comme seq, sans vider les tables : la ligne voit les symboles des lignes
   précédentes (lignes compilées en séquence, V11). */
static int seq_suite(const char *texte, const unsigned char *attendu, unsigned int n)
{
    if (!compiler(texte)) {
        printf("  [%s] rejet inattendu : C %u, position %u\n", texte,
               erreurs_numero(d.nature), d.position);
        return 0;
    }
    recuperer();
    if (lg_obtenu != n || memcmp(obtenu, attendu, n) != 0) {
        printf("  [%s]\n", texte);
        afficher_octets("obtenu ", obtenu, lg_obtenu);
        afficher_octets("attendu", attendu, n);
        return 0;
    }
    return 1;
}

#define SEQ_SUITE(texte, ...) \
    do { static const unsigned char v_[] = { __VA_ARGS__ }; \
         VERIFIER(seq_suite((texte), v_, sizeof v_)); } while (0)

static int accepte(const char *texte)
{
    symboles_vider();
    if (!compiler(texte)) {
        printf("  [%s] rejet inattendu : C %u, position %u\n", texte,
               erreurs_numero(d.nature), d.position);
        return 0;
    }
    recuperer();
    return 1;
}

#define SEQ(texte, ...) \
    do { static const unsigned char v_[] = { __VA_ARGS__ }; \
         VERIFIER(seq((texte), v_, sizeof v_)); } while (0)


#endif
