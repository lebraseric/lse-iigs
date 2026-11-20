#!/bin/bash
# asr33.sh — lance l'émulateur de télétype ASR-33 asr33emu sur le port série,
# pour une session de LSE -TTY (IMP-0284) ou de LSE -SERIE (IMP-0283).
# Emploi : scripts/terminaux/asr33.sh [PORT] [--manuel] [options d'asr33emu…]
#   PORT : chemin du port (/dev/cu.usbserial-…), ou « gssquared » pour le pont
#          de scripts/pont_serie.py (/tmp/lse-serie/terminal). Sans PORT, le
#          seul /dev/cu.usbserial-* ou /dev/cu.usbmodem* présent.
#   --manuel : lecteur et perforateur conduits à la main seulement, comme dans
#          asr33emu (pour LSE -SERIE). Sans elle, la télétype obéit aux ordres
#          que LSE -TTY lui envoie (IMP-0284) : DC1 et NAK pour le lecteur, qui
#          s'arrête aussi après chaque X-OFF lu, DC2 et DC4 pour le perforateur.
#   Options utiles : --baud 300, --mute, --throttle_rate 30. Un adaptateur
#          FTDI FT232R ne descend pas sous 183 bauds environ : 110 bauds (ASR-33)
#          y sont refusés à l'ouverture du port ; sous GSSquared, toute vitesse.
# L'émulateur n'est pas dans le dépôt (aucune licence) : il est cloné à part,
# au commit 4428033, avec son environnement Python (.venv) ; son dossier est
# ~/Code/asr33emu, ou celui que désigne ASR33EMU. Installation et essais :
# 05-IMPLEMENTATION/audits/Essais_IIGS_2026-11-19.md, §6.
# Les rubans (build/rubans-asr33/*.pt) se préparent par
# scripts/terminaux/rubans_asr33.sh ; le lecteur et le perforateur y ouvrent.
set -e

RACINE=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/../.." &> /dev/null && pwd )
EMU="${ASR33EMU:-$HOME/Code/asr33emu}"
RUBANS="$RACINE/build/rubans-asr33"

if [ ! -x "$EMU/.venv/bin/python" ] || [ ! -f "$EMU/asr33emu.py" ]; then
  echo "asr33emu introuvable dans $EMU (voir l'en-tête de ce script)" >&2
  exit 1
fi

PORT=""
if [ $# -gt 0 ] && [ "${1#-}" = "$1" ]; then
  PORT="$1"
  shift
fi
if [ "$PORT" = "gssquared" ]; then
  PORT=/tmp/lse-serie/terminal
elif [ -z "$PORT" ]; then
  shopt -s nullglob
  PORTS=(/dev/cu.usbserial-* /dev/cu.usbmodem*)
  if [ ${#PORTS[@]} -ne 1 ]; then
    echo "Port série à préciser ; présents : ${PORTS[*]:-aucun}" >&2
    exit 1
  fi
  PORT="${PORTS[0]}"
fi
if [ ! -e "$PORT" ]; then
  echo "Port $PORT absent" >&2
  exit 1
fi

mkdir -p "$RUBANS"
CONFIG=$(mktemp -t asr33_lse)
trap 'rm -f "$CONFIG"' EXIT
sed -e "s|@PORT@|$PORT|" -e "s|@RUBANS@|$RUBANS|" \
  "$RACINE/scripts/terminaux/asr33_lse.yaml" > "$CONFIG"

echo "asr33emu sur $PORT"
# asr33_macos.py enregistre la police Teletype33 pour le processus, puis
# exécute asr33emu.py depuis son dossier.
"$EMU/.venv/bin/python" -I "$RACINE/scripts/terminaux/asr33_macos.py" "$EMU" \
  --config "$CONFIG" "$@"
