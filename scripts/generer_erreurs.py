#!/usr/bin/env python3
"""generer_erreurs.py — génère src/noyau/erreurs.h et src/noyau/erreurs.c à
partir des catalogues des natures d'erreur (chapitre Gestionnaire des erreurs,
§7 compilation, §9 exécution, 05-IMPLEMENTATION/tome-v/Gestionnaire_erreurs_LSE-IIGS.md).

IMP-0195 : les constantes ERR_C_* suivent l'ordre du catalogue ; la flèche
d'affectation des libellés est écrite '_' (ASCII 95). Outil de l'hôte, sans
effet sur le code livré autre que les deux fichiers produits.

Emploi, depuis la racine du dépôt : python3 scripts/generer_erreurs.py
"""

import os
import sys

RACINE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CHAPITRE = os.path.join(RACINE, "05-IMPLEMENTATION", "tome-v", "Gestionnaire_erreurs_LSE-IIGS.md")
SORTIE_H = os.path.join(RACINE, "src", "noyau", "erreurs.h")
SORTIE_C = os.path.join(RACINE, "src", "noyau", "erreurs.c")


# section, préfixe, classe, longueur maximale du libellé (IMP-0195, point 5)
SECTIONS = [("## 7. Catalogue", "\n## 8.", "ERR_C_", "C", 62),
            ("## 9. Catalogue", "\n## 9 bis.", "ERR_E_", "E", 48),
            ("## 9 bis. Catalogue", "\n## 9 ter.", "ERR_D_", "D", 48),
            ("## 9 ter. Catalogue", "\n## 10.", "ERR_F_", "F", 48)]


def lire_catalogue():
    texte = open(CHAPITRE, encoding="utf-8").read()
    natures = []
    for titre, suite, prefixe, classe, maximum in SECTIONS:
        debut = texte.index(titre)
        fin = texte.index(suite, debut)
        for ligne in texte[debut:fin].split("\n"):
            if not ligne.startswith("| `" + prefixe):
                continue
            cases = [c.strip() for c in ligne.split("|")]
            ident = cases[1].strip("`")
            numero = 0 if classe == "D" else int(cases[2].split()[1])
            libelle = cases[3].replace("←", "_")
            if any(ord(c) > 127 for c in libelle):
                sys.exit("libellé non ASCII : %s" % libelle)
            if len(libelle) > maximum:
                sys.exit("libellé de plus de %d caractères : %s" % (maximum, libelle))
            natures.append((ident, numero, libelle, classe))
    return natures


ENTETE_H = """/* erreurs.h — natures d'erreur, catalogue et mise en forme des messages
   EXI: EXI-0019, EXI-0051  VM: VM-2210  IV: Tome IV §3.3, §17 (ALG-1701)
   IMP: IMP-0185 (numérotation), IMP-0186 (forme des messages), IMP-0187
        (granularité), IMP-0195 (structure du diagnostic)
   Tests: tests/composants/composants_erreurs.c
   Spécification : chapitre Gestionnaire des erreurs §7 (catalogue de
   compilation), §8, §9 (catalogue d'exécution), §9 bis (dialogue), §9 ter (fichier).
   FICHIER GÉNÉRÉ par scripts/generer_erreurs.py à partir du §7 : ne pas le
   modifier à la main. */

#ifndef LSE_ERREURS_H
#define LSE_ERREURS_H

typedef unsigned int NatureErreur;

#define ERR_AUCUNE 0

/* --- compilation (classe C, domaine COMPILATION), §7 ; exécution (classe E,
   domaine EXECUTION), §9 ; dialogue (classe D, domaine DIALOGUE), §9 bis ;
   fichier (classe F, domaine SERVICE), §9 ter --- */
"""

PIED_H = """
/* Classe d'une nature : 'C' (compilation), 'E' (exécution), 'D' (dialogue),
   'F' (fichier) ; 0 si la nature est inconnue. */
char          erreurs_classe(NatureErreur n);
/* Numéro affiché (IMP-0185) ; 0 si la nature est inconnue. */
unsigned int  erreurs_numero(NatureErreur n);
/* Libellé de la liste des erreurs ; NULL si la nature est inconnue. */
const char   *erreurs_libelle(NatureErreur n);
/* Message de IMP-0186 pour une nature de compilation : « ERREUR  C nn »,
   suivi de « : libellé » si avec_libelle est vrai. Écrit au plus taille - 1
   caractères et termine toujours la chaîne. Renvoie 0 si la nature est
   inconnue ou n'est pas de classe C (sortie vide). */
int           erreurs_formater(NatureErreur n, int avec_libelle,
                               char *sortie, unsigned int taille);
/* Message de IMP-0186 pour une nature d'exécution : « ERREUR (EX.) nn EN
   LIGNE lll », ou « EN LIGNE BUR » si ligne vaut 0, suivi de « : libellé »
   si avec_libelle est vrai. Mêmes règles de longueur ; 0 si la nature n'est
   pas de classe E. */
int           erreurs_formater_execution(NatureErreur n, unsigned char ligne, int avec_libelle,
                                         char *sortie, unsigned int taille);

/* Message d'une nature du dialogue (Moniteur IMP-0235) : « ???? », suivi de
   « : libellé » si avec_libelle est vrai ; 0 si la nature n'est pas de classe D. */
int           erreurs_formater_dialogue(NatureErreur n, int avec_libelle,
                                        char *sortie, unsigned int taille);

/* Message d'une nature de fichier (Bibliothécaire et fichiers IMP-0250) :
   « ERREUR F nn », suivi de « EN LIGNE lll » (« BUR » si ligne vaut 0) si
   en_execution est vrai, puis de « : libellé » si avec_libelle est vrai ;
   0 si la nature n'est pas de classe F. */
int           erreurs_formater_fichier(NatureErreur n, int en_execution, unsigned char ligne,
                                       int avec_libelle, char *sortie, unsigned int taille);

/* Diagnostic d'exécution (IMP-0195, point 2 bis) : ligne 0 = machine de bureau. */
typedef struct {
    NatureErreur  nature;
    unsigned char ligne;
} DiagnosticExecution;

#endif
"""

ENTETE_C = """/* erreurs.c — catalogue des natures d'erreur et mise en forme des messages
   EXI: EXI-0019, EXI-0051  VM: VM-2210  IV: Tome IV §3.3, §17 (ALG-1701)
   IMP: IMP-0185, IMP-0186, IMP-0187, IMP-0195
   Tests: tests/composants/composants_erreurs.c
   Spécification : chapitre Gestionnaire des erreurs §4, §7, §8, §9, §9 bis, §9 ter.
   Segment : principal (IMP-0135). Aucun affichage : la console s'en charge.
   FICHIER GÉNÉRÉ par scripts/generer_erreurs.py à partir du §7 : ne pas le
   modifier à la main. */

#pragma noroot

#include <string.h>
#include "erreurs.h"
#include "../interfaces/profil.h"

typedef struct {
    char          classe;
    unsigned int  numero;
    const char   *libelle;   /* « _ » note la flèche d'affectation (ASCII 95) */
} EntreeCatalogue;

/* Indice = nature - 1, dans l'ordre du catalogue (§7). */
static const EntreeCatalogue catalogue[ERR_NB_NATURES] = {
"""

PIED_C = """};

static const EntreeCatalogue *entree(NatureErreur n)
{
    if (n < 1 || n > ERR_NB_NATURES)
        return NULL;
    return &catalogue[n - 1];
}

char erreurs_classe(NatureErreur n)
{
    const EntreeCatalogue *e = entree(n);
    return e != NULL ? e->classe : 0;
}

unsigned int erreurs_numero(NatureErreur n)
{
    const EntreeCatalogue *e = entree(n);
    return e != NULL ? e->numero : 0;
}

const char *erreurs_libelle(NatureErreur n)
{
    const EntreeCatalogue *e = entree(n);
    return e != NULL ? e->libelle : NULL;
}

/* Ajoute texte à sortie sans dépasser taille - 1 caractères. */
static void ajouter(char *sortie, unsigned int taille, unsigned int *lg, const char *texte)
{
    unsigned int i = 0;
    while (texte[i] != '\\0' && *lg + 1 < taille)
        sortie[(*lg)++] = texte[i++];
    sortie[*lg] = '\\0';
}

/* Écrit v en décimal, sur au moins min chiffres complétés de zéros. */
static void ajouter_nombre(char *sortie, unsigned int taille, unsigned int *lg,
                           unsigned int v, unsigned int min)
{
    char chiffres[6];
    unsigned int i = sizeof chiffres - 1;

    chiffres[i] = '\\0';
    do {
        chiffres[--i] = (char) ('0' + v % 10);
        v /= 10;
    } while (v != 0);
    while (sizeof chiffres - 1 - i < min)
        chiffres[--i] = '0';
    ajouter(sortie, taille, lg, &chiffres[i]);
}

int erreurs_formater(NatureErreur n, int avec_libelle, char *sortie, unsigned int taille)
{
    const EntreeCatalogue *e = entree(n);
    unsigned int lg = 0;

    if (taille == 0)
        return 0;
    sortie[0] = '\\0';
    if (e == NULL || e->classe != 'C')
        return 0;
    ajouter(sortie, taille, &lg, "ERREUR  C ");
    ajouter_nombre(sortie, taille, &lg, e->numero, 2);   /* IMP-0186 */
    if (avec_libelle) {
        ajouter(sortie, taille, &lg, " : ");
        ajouter(sortie, taille, &lg, e->libelle);
    }
    return 1;
}

int erreurs_formater_execution(NatureErreur n, unsigned char ligne, int avec_libelle,
                               char *sortie, unsigned int taille)
{
    const EntreeCatalogue *e = entree(n);
    unsigned int lg = 0;

    if (taille == 0)
        return 0;
    sortie[0] = '\\0';
    if (e == NULL || e->classe != 'E')
        return 0;
    ajouter(sortie, taille, &lg, "ERREUR (EX.) ");
    ajouter_nombre(sortie, taille, &lg, e->numero, 2);
    ajouter(sortie, taille, &lg, " EN LIGNE ");
    if (ligne == 0)
        ajouter(sortie, taille, &lg, "BUR");
    else
        ajouter_nombre(sortie, taille, &lg, ligne, 3);
    if (avec_libelle) {
        ajouter(sortie, taille, &lg, " : ");
        ajouter(sortie, taille, &lg, e->libelle);
    }
    return 1;
}

int erreurs_formater_fichier(NatureErreur n, int en_execution, unsigned char ligne,
                             int avec_libelle, char *sortie, unsigned int taille)
{
    const EntreeCatalogue *e = entree(n);
    unsigned int lg = 0;

    if (taille == 0)
        return 0;
    sortie[0] = '\\0';
    if (e == NULL || e->classe != 'F')
        return 0;
    ajouter(sortie, taille, &lg, "ERREUR F ");                /* Victor Duruy */
    ajouter_nombre(sortie, taille, &lg, e->numero, 2);
    if (en_execution) {
        ajouter(sortie, taille, &lg, " EN LIGNE ");
        if (ligne == 0)
            ajouter(sortie, taille, &lg, "BUR");
        else
            ajouter_nombre(sortie, taille, &lg, ligne, 3);
    }
    if (avec_libelle) {
        ajouter(sortie, taille, &lg, " : ");
        ajouter(sortie, taille, &lg, e->libelle);
    }
    return 1;
}

int erreurs_formater_dialogue(NatureErreur n, int avec_libelle, char *sortie, unsigned int taille)
{
    const EntreeCatalogue *e = entree(n);
    unsigned int lg = 0;

    if (taille == 0)
        return 0;
    sortie[0] = '\\0';
    if (e == NULL || e->classe != 'D')
        return 0;
    ajouter(sortie, taille, &lg, "????");
    if (avec_libelle) {
        ajouter(sortie, taille, &lg, " : ");
        ajouter(sortie, taille, &lg, e->libelle);
    }
    return 1;
}
"""


def main():
    natures = lire_catalogue()
    largeur = max(len(n[0]) for n in natures)
    defines = []
    for i, (ident, numero, _, classe) in enumerate(natures):
        lettre = {"C": "C", "E": "EX", "F": "F"}.get(classe, "")
        if classe == "D":
            code = "????"
        else:
            code = "%s %02d" % (lettre, numero) if numero < 100 else "%s %d" % (lettre, numero)
        defines.append("#define %-*s %3d   /* %s */" % (largeur, ident, i + 1, code))
    h = ENTETE_H + "\n".join(defines) + "\n\n#define ERR_NB_NATURES %d\n" % len(natures) + PIED_H
    lignes = []
    for i, (ident, numero, libelle, classe) in enumerate(natures):
        sep = "," if i < len(natures) - 1 else ""
        lignes.append('    { \'%s\', %3d, "%s" }%s   /* %s */'
                      % (classe, numero, libelle.replace("\\", "\\\\").replace('"', '\\"'), sep, ident))
    c = ENTETE_C + "\n".join(lignes) + "\n" + PIED_C
    open(SORTIE_H, "w", encoding="utf-8").write(h)
    open(SORTIE_C, "w", encoding="utf-8").write(c)
    print("%d natures écrites" % len(natures))


if __name__ == "__main__":
    main()
