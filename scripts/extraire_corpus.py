#!/usr/bin/env python3
"""extraire_corpus.py — extrait les lignes de programme des transcriptions
d'INRDP75 et de LSE15 (02-HISTORIQUE/) vers tests/historiques/corpus/.

IMP-0203 (chapitre Décompilateur §9) : sont retenues les lignes des blocs de
code qui commencent par un numéro de 1 à 255 suivi d'un espace ou d'un '*' ;
la flèche d'affectation est écrite '_' (ASCII 95), l'exponentiation '^'
(ASCII 94) ; une ligne qui contient un autre caractère non ASCII est écartée.
Les lignes ne sont pas filtrées autrement : celles que le compilateur rejette
(tableaux de nombres imprimés, fragments) sont comptées par la suite de tests.
Outil de l'hôte, sans effet sur le code livré.

Emploi, depuis la racine du dépôt : python3 -I scripts/extraire_corpus.py
"""

import os
import re

RACINE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCES = [
    ("LSE-Manuel-d-utilisation-INRDP-1975.md", "inrdp75.txt"),
    ("LSE15-Manuel-Mitra15-EAT-1978.md", "lse15.txt"),
]


def extraire(chemin):
    lignes = []
    ecartees = 0
    dans_bloc = False
    for ligne in open(chemin, encoding="utf-8"):
        ligne = ligne.rstrip("\n")
        if ligne.startswith("```"):
            dans_bloc = not dans_bloc
            continue
        if not dans_bloc:
            continue
        texte = ligne.strip()
        m = re.match(r"(\d+)[ *]", texte)
        if not m or not 1 <= int(m.group(1)) <= 255:
            continue
        texte = texte.replace("←", "_").replace("↑", "^")
        if any(ord(c) > 127 for c in texte):
            ecartees += 1
            continue
        lignes.append(texte)
    return lignes, ecartees


def main():
    for source, sortie in SOURCES:
        lignes, ecartees = extraire(os.path.join(RACINE, "02-HISTORIQUE", source))
        with open(os.path.join(RACINE, "tests", "historiques", "corpus", sortie), "w",
                  encoding="ascii", newline="\n") as f:
            for l in lignes:
                f.write(l + "\n")
        print("%s : %d lignes, %d écartées (caractère non ASCII)" % (sortie, len(lignes), ecartees))


if __name__ == "__main__":
    main()
