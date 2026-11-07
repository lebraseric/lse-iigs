#!/bin/bash
# creer_image_duruy.sh — image disque ProDOS des programmes de Victor Duruy,
# un ruban par programme (IMP-0262 : fichier texte TXT, lignes terminées par CR),
# et des rubans de données PRONO, FICH et FICH2.
# Emploi : scripts/creer_image_duruy.sh [IMAGE.po]   (défaut : build/DURUY.po)
# Sources : 02-HISTORIQUE/Transcriptions-T1600-Victor-Duruy/*.lse (lignes LF) ;
# PRONO, FICH et FICH2 sont perforés par PERFO lui-même, sous Golden Gate, par
# 02-HISTORIQUE/Reconstitution-PERFO-CHARG/essais/verifier.py, et ne sont écrits
# que si toute la vérification réussit : build/LSE doit être à jour.
# Lecture : LSE -RUBAN /DURUY/NOM, puis X-ON (Ctrl-Q) sur une ligne vide.
# Les rubans de données appellent CHARG, rangé auparavant depuis le ruban
# CHARG de LSE.po (IMP-0241).
set -e

RACINE=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/.." &> /dev/null && pwd )
SOURCES="$RACINE/02-HISTORIQUE/Transcriptions-T1600-Victor-Duruy"
RECONST="$RACINE/02-HISTORIQUE/Reconstitution-PERFO-CHARG"
IMAGE="${1:-$RACINE/build/DURUY.po}"
TRAVAIL=$(mktemp -d)
trap 'rm -rf "$TRAVAIL"' EXIT

ENTREES=()
for SOURCE in "$SOURCES"/*.lse; do
  NOM=$(basename "$SOURCE" .lse)
  tr '\n' '\r' < "$SOURCE" > "$TRAVAIL/$NOM"
  ENTREES+=("$TRAVAIL/$NOM:$NOM:TXT")
done

python3 -I "$RECONST/essais/verifier.py" "$TRAVAIL/essai" "$RACINE/build/LSE" "$TRAVAIL/donnees"
for NOM in PRONO FICH FICH2; do
  ENTREES+=("$TRAVAIL/donnees/$NOM:$NOM:TXT")
done
"$RACINE/scripts/creer_image.sh" "$IMAGE" DURUY "${ENTREES[@]}"
