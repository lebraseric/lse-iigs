# Construction

Construction du LSE-IIGS par Golden Gate (`iix`) et ORCA/C 2.2.1. La spécification se trouve dans `05-IMPLEMENTATION/tome-v/Construction_LSE-IIGS.md` (IMP-0140 à IMP-0143, IMP-0192, IMP-0193).

Toutes les commandes se lancent depuis ce répertoire :

| Commande | Effet |
|---|---|
| `make tests` | Construit et exécute toutes les suites de tests (profil `dev`), puis affiche un bilan ; échoue si une suite échoue |
| `make tests PROFIL=cible` | Idem dans le profil `cible` |
| `make` | Construit l'exécutable `LSE` ; échoue tant que les modules de `src/` ne sont pas tous écrits |
| `make clean` | Supprime les fichiers objets, les exécutables et `tests/` |

Toute compilation dépend de tous les en-têtes du dépôt et du témoin `profil.temoin`, réécrit seulement quand `PROFIL` change (Construction IMP-0140, complément du 2026-10-30) : modifier un en-tête ou changer de profil recompile ce qu'il faut, sans `make clean`.

Produits de la construction, tous dans ce répertoire : fichiers objets (`*.root`, `*.a`), témoin du profil (`profil.temoin`), exécutable `LSE`, rubans `ruban_CHARG` et `ruban_PERFO`, images `LSE.po` (distribution : `LSE`, `CHARG`, `PERFO`) et `ESSAIS.po` (sondes), exécutables de test dans `tests/` (Construction IMP-0241). L'image `DURUY.po` (programmes de Victor Duruy, rubans de données `PRONO`, `FICH`, `FICH2`) se construit à part, par `scripts/creer_image_duruy.sh`, `LSE` étant à jour.

Une suite renvoie son nombre d'échecs comme code de sortie (`tests/commun/verif.h`). Les bouchons se trouvent dans `tests/bouchons/` et ne sont jamais liés dans `LSE`.

Golden Gate n'implémente ni `FreeMem` ni `RealFreeMem` : aucun code exécuté sous Golden Gate ne doit les appeler.
