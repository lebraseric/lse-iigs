#!/bin/bash
# rubans_asr33.sh — rubans à lire au lecteur de l'émulateur asr33emu.
# Emploi : scripts/terminaux/rubans_asr33.sh [DOSSIER]   (défaut : build/rubans-asr33)
# Un fichier NOM.pt par programme transcrit de Victor Duruy et par programme
# reconstitué (CHARG, PERFO), sous la forme que PERFORER donne sous -TTY
# (IMP-0284, dessin du ruban d'INRDP75 p. 188) : 20 nuls de bande vierge,
# chaque ligne sous la forme CR LF texte X-OFF, 20 nuls. Le lecteur de
# l'émulateur saute l'amorce et s'arrête sur les nuls de fin
# (skip_leading_nulls, auto_stop) ; il ne s'arrête pas sur X-OFF. Les lignes
# arrivent à LSE comme si elles étaient frappées ; sous LSE F4 et -T1600, une
# ligne blanche les sépare à l'impression (INRDP75 p. 194, Listage 1).
set -e

RACINE=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/../.." &> /dev/null && pwd )
DOSSIER="${1:-$RACINE/build/rubans-asr33}"
mkdir -p "$DOSSIER"

for SOURCE in "$RACINE"/02-HISTORIQUE/Transcriptions-T1600-Victor-Duruy/*.lse \
              "$RACINE"/02-HISTORIQUE/Reconstitution-PERFO-CHARG/*.lse; do
  NOM=$(basename "$SOURCE" .lse)
  { head -c 20 /dev/zero
    LC_ALL=C awk 'length($0) > 0 { printf "\r\n%s\023", $0 }' "$SOURCE"
    head -c 20 /dev/zero
  } > "$DOSSIER/$NOM.pt"
done
ls "$DOSSIER"
