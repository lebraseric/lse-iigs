#!/bin/bash
# creer_image.sh — image disque ProDOS portant les fichiers cibles du LSE-IIGS
# Emploi : scripts/creer_image.sh IMAGE.po VOLUME FICHIER:NOM:TYPE...
#   Crée IMAGE.po, image ProDOS vierge de 800 Ko nommée /VOLUME/, puis y ajoute
#   chaque fichier sous le nom ProDOS NOM avec le type TYPE (par exemple EXE).
# Reprend 03-APPLE-IIGS/outils/add-file.sh et get-binaries.sh (AppleCommander),
# avec des chemins entre guillemets : le dépôt contient des espaces.
set -e

if [ $# -lt 3 ]; then
  echo "Emploi : $(basename "$0") IMAGE.po VOLUME FICHIER:NOM:TYPE..." >&2
  exit 1
fi

RACINE=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/.." &> /dev/null && pwd )
OUTILS="$RACINE/03-APPLE-IIGS/outils"
AC_VER="1.9.0"
AC_JAR="$OUTILS/AppleCommander-ac-${AC_VER}.jar"

if ! command -v java > /dev/null; then
  echo "java introuvable : AppleCommander demande Java 11 au moins." >&2
  exit 1
fi
if [ ! -f "$AC_JAR" ]; then
  curl -s -L -o "$AC_JAR" "https://github.com/AppleCommander/AppleCommander/releases/download/${AC_VER}/AppleCommander-ac-${AC_VER}.jar"
fi

IMAGE="$1"
VOLUME="$2"
shift 2
rm -f "$IMAGE"
trap 'rm -f "$IMAGE"' ERR            # pas d'image partielle
java -jar "$AC_JAR" -pro800 "$IMAGE" "$VOLUME"
for ENTREE in "$@"; do
  FICHIER="${ENTREE%%:*}"
  RESTE="${ENTREE#*:}"
  NOM="${RESTE%%:*}"
  TYPE="${RESTE#*:}"
  if [ ! -f "$FICHIER" ]; then
    echo "Fichier introuvable : $FICHIER" >&2
    exit 1
  fi
  java -jar "$AC_JAR" -p "$IMAGE" "$NOM" "$TYPE" < "$FICHIER"
done
java -jar "$AC_JAR" -ll "$IMAGE"
