#!/bin/sh
# corpus.sh - commande git du dépôt privé du corpus (documents de tiers protégés).
#
# Le dépôt privé est superposé à l'arbre de travail : ses fichiers restent à
# leur place (02-HISTORIQUE/, 03-APPLE-IIGS/, 04-ORCA-C/), exclus du dépôt
# public par .gitignore (section « Corpus privé ») ; son répertoire git est
# .corpus.git à la racine. Il ne suit que les fichiers ajoutés explicitement
# (status.showUntrackedFiles = no).
#
# Usage : scripts/corpus.sh <commande git>, par exemple
#   scripts/corpus.sh status
#   scripts/corpus.sh add "02-HISTORIQUE/NOUVEAU.pdf"
#   scripts/corpus.sh commit -m "Versement de ..."
# Un document versé ici doit aussi être ajouté à la section « Corpus privé »
# de .gitignore.

RACINE=$(cd "$(dirname "$0")/.." && pwd)
exec git --git-dir="$RACINE/.corpus.git" --work-tree="$RACINE" "$@"
