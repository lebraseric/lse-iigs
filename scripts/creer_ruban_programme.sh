#!/bin/bash
# creer_ruban_programme.sh — ruban d'un programme L.S.E. qui se range lui-même
# Emploi : scripts/creer_ruban_programme.sh SOURCE.lse NOM RUBAN
#   SOURCE.lse : une ligne L.S.E. par ligne, fin de ligne LF, ← écrit _ ;
#   RUBAN      : fichier produit. Les lignes y sont terminées par CR (IMP-0262),
#                puis suivies de RA⊗NOM⊗EF⊗*⊗ (⊗ : X-OFF et deux nuls), comme
#                le ruban à rangement automatique d'INRDP75 (annexe II, l. 8582).
# Lecture : LSE -RUBAN chemin, puis BO, X-ON (Ctrl-Q), AU ; le programme est
# rangé sous NOM dans la bibliothèque, puis effacé du programme courant.
# Employé par build/Makefile pour les rubans CHARG et PERFO de LSE.po (IMP-0241).
set -e

if [ $# -ne 3 ]; then
  echo "Emploi : $(basename "$0") SOURCE.lse NOM RUBAN" >&2
  exit 1
fi

{ tr '\n' '\r' < "$1"
  printf 'RA\023\000\000%s\023\000\000EF\023\000\000*\023\000\000' "$2"
} > "$3"
