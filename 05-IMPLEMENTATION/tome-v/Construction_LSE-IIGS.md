# Tome V — Construction (Makefile / Golden Gate / ORCA-C)

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Environnement de développement :** Golden Gate sous macOS (compilation et exécution croisées avec ORCA/C 2.2.1)
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V, niveau `05-IMPLEMENTATION/decisions/`
**Portée :** chaîne de construction complète du dépôt de code — Makefile, commandes `iix compile`/`iix link`, gestion du nommage des fichiers objets, gestion du code de débogage, type de fichier de l'exécutable produit. Constitue le livrable « scripts ou commandes de construction ORCA/C » prévu par le conditionnement (§20.3).
**Rang normatif :** subordonné au Catalogue des exigences, au Tome III, au Tome IV, et aux chapitres RCI-M, Architecture physique et Organisation du dépôt du Tome V (notamment IMP-0129 à IMP-0139). Aucune disposition du présent chapitre ne peut contredire ces sources.
**Place dans le Tome V :** ce chapitre suit le chapitre Organisation du dépôt, dont il referme la limite explicite relative à la construction. La numérotation `IMP-*` du présent chapitre poursuit celle d'Organisation du dépôt (dernier identifiant utilisé : IMP-0139) à partir d'**IMP-0140**.

---

## 0. Avertissement de statut

Comme pour les chapitres précédents du Tome V, aucune disposition du présent chapitre ne constitue une reconstruction historique. Tout ce qui est décrit ici — Makefile, options de compilation, gestion du nommage — relève exclusivement de décisions d'implémentation (`IMP-*`), prises au niveau 6 de la hiérarchie de résolution des ambiguïtés (conditionnement §18).

---

## 1. Segment des fichiers `plateforme/`

Le chapitre Organisation du dépôt affecte un segment ORCA/C à chaque fichier de `src/noyau/` (table de correspondance module ↔ segment) mais ne tranchait pas celui des fichiers de `src/plateforme/` eux-mêmes. Conformément au principe déjà posé (les fichiers `noyau/` et `plateforme/` d'un même processeur logique restent affectés au même segment), la table est complétée comme suit :

| Fichier `plateforme/` | Segment |
|---|---|
| `main.c` | blanc |
| `console_iigs.c` | blanc |
| `fichiers_iigs.c` | `"fichiers"` |
| `horloge_iigs.c` | `"interp_es"` |
| `memoire_iigs.c` | `"objets"` |

`fichiers_iigs.c` est également appelé par `bibliotheque.c` (segment `"bibliothe"`, Bibliothécaire et fichiers IMP-0249) : il s'agit d'un appel inter-segment ordinaire, sans conséquence — les segments ORCA/C sont des unités de chargement, pas des unités d'appel ; rien n'impose qu'un appelant et son appelé partagent le même segment.

---

## 2. Makefile orchestrant `iix compile` / `iix link`

**IMP-0140** — *Un Makefile hôte invoque directement `iix compile` et `iix link`*

- **Sujet :** comment automatiser la construction du programme à partir de l'arborescence `src/noyau/` / `src/plateforme/` ?
- **Choix :** un Makefile GNU standard (`build/Makefile`), une règle explicite par fichier source — pas de règle à motif générique, pour garder visible à la lecture du Makefile la correspondance entre chaque fichier source et son invocation de compilation — invoquant `iix compile` puis `iix link`. Chaque fichier est compilé depuis son propre répertoire (`cd ../src/noyau && iix compile … moniteur.c keep=../../build/moniteur`) : ORCA/C résout un `#include "…"` relatif par rapport au répertoire courant, et non par rapport au fichier qui l'inclut (constaté le 2026-10-22) ; ce n'est qu'ainsi que la convention d'en-tête du §6 (`#include "../interfaces/types_communs.h"`) est valide. Le chemin `keep=` est relatif, ce qui évite les espaces du chemin absolu du dépôt. L'option `-I` est toujours passée : aucun fichier `.sym` (en-têtes précompilés) n'est lu ni écrit, ce qui évite qu'un `.sym` conserve des macros `cc=` d'une compilation antérieure (notes ORCA/C 2.2.1, points 178 et 179) — risque réel lorsqu'on alterne les profils `dev` et `cible` (IMP-0143) — et qu'il soit écrit dans les répertoires de sources. ORCA/C accepte `/` comme séparateur de chemin. Le fichier contenant `main` (`plateforme/main.c`) est toujours listé en premier dans la liste d'objets passée à `iix link`, conformément à l'exigence documentée du manuel ORCA/C (chap. 7 : le premier nom de fichier objet doit contenir le programme principal ; les autres peuvent être donnés dans n'importe quel ordre). Le type de fichier de l'exécutable produit est fixé explicitement à `EXE` (`$B5`, « GS/OS Shell application ») via `-DKeepType=EXE`, cohérent avec le conditionnement §5 (lancement depuis le shell ORCA, pas depuis le Finder ni le bureau ORCA) — c'est déjà la valeur par défaut de Golden Gate, mais la fixer explicitement documente l'intention plutôt que de s'appuyer sur un défaut implicite.
- **Alternatives envisagées :**
  1. Règles à motif générique (`%.a: %.c`) plutôt qu'une règle par fichier — écarté : moins lisible pour un dépôt de cette taille (~16 fichiers), sans bénéfice réel ; la priorité « simplicité, traçabilité » (conditionnement §3, §27) favorise l'explicite.
  2. Un script de commandes ORCA shell natif (fichier EXEC) plutôt qu'un Makefile hôte — écarté : `iix compile`/`iix link` sont précisément conçus pour s'intégrer à l'outillage hôte, et un Makefile permet d'utiliser `make` normalement (dépendances, reconstruction incrémentale, parallélisme) sans dupliquer une logique de construction ORCA-native qui n'apporterait rien ici.
- **Justification :** conditionnement §20.3 (livrable) ; §3 (simplicité) ; §27 (fiabilité).
- **Impact mémoire / disque :** aucun sur l'exécutable produit.
- **Impact performances :** cycle de construction reproductible en une commande, parallélisable (`make -j`), chaque fichier objet étant indépendant jusqu'à l'édition de liens.
- **Impact historique :** neutre.
- **Références normatives :** conditionnement §5, §20.3 ; documentation Golden Gate (Built-in Commands — Compile, Link) ; manuel ORCA/C chap. 7 (Link, ordre des objets ; pathname separators), chap. 8 ({KeepType}).
- **Statut : retenu.**
- **Complément du 2026-10-30 (Éric ; audit du code, AC-34) : dépendances.**
  - Toute règle de compilation dépend de son fichier `.c`, de `$(DEPENDANCES)` et, pour une suite ou une sonde, des objets qu'elle lie.
  - `$(DEPENDANCES)` réunit `$(ENTETES)` et le témoin du profil. `$(ENTETES)` est la liste, obtenue par `$(wildcard …)`, de tous les en-têtes de `src/interfaces`, `src/noyau`, `src/plateforme`, `tests/commun` et `tests/composants`.
  - Raison : un en-tête en inclut d'autres (`rcim.h`, `symboles.h` et `pile.h` incluent `objets.h`), et ORCA/C ne produit pas la liste des inclusions d'un fichier. Les listes écrites à la main ne citaient que les inclusions directes : 31 règles sur 58 en manquaient au moins une. Après un changement de prototype dans `objets.h`, un objet périmé a fait s'arrêter deux suites sur `BRK 00`.
  - Le témoin `build/profil.temoin` contient le nom du profil. Le Makefile le réécrit à sa lecture seulement si `PROFIL` a changé, et tout objet en dépend : changer de profil recompile tout, sans `make clean`. Une lecture du Makefile par `make -n` ou `make -q` le réécrit aussi quand le profil change ; il en résulte au plus une recompilation superflue.
  - Coût : modifier un en-tête recompile tous les objets dont dépend la cible demandée.
  - Alternatives écartées : des listes exactes tenues par un script de contrôle (plus longues, un outil de plus) ; la seule consigne de construire après `make clean` (le défaut serait resté).

---

## 3. Nommage des fichiers objets

**IMP-0141** — *Les fichiers objets de construction conservent le nom du fichier source ; seul l'exécutable final respecte la contrainte de nommage GS/OS*

- **Sujet :** le nom passé en `KEEP=` doit-il être abrégé pour respecter une limite de longueur ?
- **Choix :** non. La limite de 15 caractères pour un nom de fichier (donc 10 pour la base d'un objet, l'extension `.ROOT` portant le total à 15) est une contrainte du système de fichiers ProDOS qu'utilise GS/OS sur cible réelle — pas une contrainte du compilateur ORCA/C lui-même, qui accepte des noms plus longs sur tout système de fichiers qui les autorise. Sous Golden Gate, les fichiers `.ROOT`/`.A` produits par la compilation sont stockés sur le système de fichiers hôte (macOS) et ne sont jamais transférés vers la cible — seul l'exécutable final l'est. Chaque fichier objet reprend donc simplement le nom de son fichier source comme paramètre `KEEP=` (`keep=compilateur`, `keep=contexte_exec`, `keep=bibliotheque`, `keep=console_iigs`, `keep=fichiers_iigs`, `keep=memoire_iigs`, etc.). Seul le nom de l'exécutable final (`LSE`, cf. IMP-0140) doit respecter la limite réelle de 15 caractères GS/OS, ce qu'il fait très largement.
- **Alternatives envisagées :** une table d'abréviations à 10 caractères maximum pour tous les objets de construction — écartée : contrainte non applicable aux fichiers manipulés uniquement pendant la construction sur système hôte ; aurait ajouté une correspondance à maintenir sans bénéfice réel.
- **Justification :** la contrainte de nommage est portée par le système de fichiers ProDOS/GS/OS de la cible, pas par le compilateur ; elle ne s'applique donc qu'aux artefacts effectivement destinés à y résider.
- **Impact mémoire / disque / performances :** sans objet.
- **Impact historique :** neutre.
- **Références normatives :** manuel ORCA/C chap. 7 (paramètre KEEP).
- **Statut : retenu.**

---

## 4. `#pragma noroot`

**IMP-0142** — *`#pragma noroot` sur tous les fichiers sauf celui contenant `main`*

- **Sujet :** ORCA/C génère par défaut un fichier `.ROOT` (code d'initialisation et appel à `main`) pour chaque fichier compilé ; un seul est réellement utile.
- **Choix :** chaque fichier de `src/noyau/` et `src/plateforme/`, à l'exception de `plateforme/main.c`, porte `#pragma noroot` en tête (après les `#include`, avant la déclaration `segment` le cas échéant) — il ne produit alors qu'un fichier `.A`, pas de `.ROOT`.
- **Alternatives envisagées :** laisser chaque fichier produire son `.ROOT` par défaut et simplement ne pas les lier — écarté : le manuel ORCA/C indique explicitement que `#pragma noroot` existe pour cet usage (« rendra le programme fini légèrement plus petit ») ; ignorer un mécanisme documenté au profit d'un contournement manuel irait à l'encontre du conditionnement §3 (« économie de mémoire et de stockage », priorité 7).
- **Justification :** manuel ORCA/C chap. 13, directive `noroot`.
- **Impact mémoire :** favorable — évite la duplication du code d'initialisation.
- **Impact disque :** exécutable et fichiers intermédiaires légèrement plus petits.
- **Impact performances :** sans objet.
- **Impact historique :** neutre.
- **Références normatives :** manuel ORCA/C chap. 13.
- **Statut : retenu.**

---

## 5. Profils de compilation et code de débogage

**IMP-0143** — *Deux profils de compilation : `dev` avec le débogage source-level, `cible` optimisé avec le seul contrôle de débordement de pile*

- **Sujet :** comment bénéficier du débogueur interactif de Golden Gate en développement, tout en livrant sur la cible un exécutable qui ne dépend d'aucun débogueur, reste protégé contre un débordement de pile et s'exécute assez vite pour un usage conversationnel ?
- **Contexte technique :** `#pragma debug` (manuel ORCA/C chap. 13) contrôle, bit à bit, les fonctionnalités de débogage :
  - bit 0 (valeur 1) — vérification de dépassement de pile à l'entrée de chaque fonction ; sûr partout, sans dépendance externe ;
  - bit 1 (valeur 2) — code de débogage source-level ; un programme contenant ce code plante s'il est exécuté depuis le shell texte ou le Finder sans débogueur source-level installé ; c'est ce bit qu'exige l'option `-g` de Golden Gate ;
  - bit 3 (valeur 8) — trace d'appels en cas d'erreur ; bit 5 (valeur 32) — détection de déréférencement de pointeur nul.
  `#pragma optimize` (même chapitre) active, bit à bit, les optimisations : bit 0 (code intermédiaire), bit 1 (judas du code 65816), bit 2 (registres), bit 3 (suppression de la réparation de pile), au-delà des optimisations non employées ici.
- **Choix :** `src/interfaces/profil.h`, inclus directement ou indirectement par chaque module de `src/` (y compris `src/plateforme/`), sélectionne le profil par compilation conditionnelle :
  ```c
  /* profil.h — sélection du profil de compilation (IMP-0143) */
  #ifdef LSE_PROFIL_DEV
  #pragma debug 43   /* bits 0+1+3+5 : + débogage source-level (Golden Gate -g) */
  #else
  #pragma debug 1    /* bit 0 : contrôle du débordement de pile */
  #pragma optimize 7 /* bits 0+1+2 : code intermédiaire, judas 65816, registres */
  #endif
  ```
  Le symbole `LSE_PROFIL_DEV` est défini depuis le Makefile via l'option compilateur `cc=-dLSE_PROFIL_DEV=1`, passée à `iix compile` uniquement lorsque `PROFIL=dev` (valeur par défaut du Makefile). Tout exécutable destiné à la cible ou à une livraison est construit avec `PROFIL=cible`. Les suites de tests passent dans les deux profils.
- **Alternatives envisagées :**
  1. Le commutateur global `+D`/`-D` de `iix compile` — écarté : il ne contrôle que les bits 1 et 2 ensemble.
  2. Profil `cible` en `debug 41` sans optimisation — écarté : cinq fois plus lent que le choix retenu (boucle `FAIRE` vide : 19,3 ms par tour contre 4,2 ms ; mesures du 2026-10-25, `05-IMPLEMENTATION/audits/Mesures_performance_IIGS_2026-10-25.md`).
  3. `debug 0` et `optimize 7` — écarté : 7 % plus rapide seulement, sans protection contre un débordement de pile ; le compilateur, récursif, peut épuiser la pile sur une expression très imbriquée.
  4. `optimize 15` (sans réparation de pile) — écarté : aucun gain mesuré, et un passage de paramètres erroné ferait planter le programme.
- **Justification :** conditionnement §21 (fonctionnement fiable sur cible), §27 (débogage possible en développement) ; manuel ORCA/C chap. 13, directives `debug` et `optimize` ; mesures sur l'Apple IIGS du 2026-10-25 ; choix d'Éric du 2026-10-25.
- **Impact mémoire :** exécutable `cible` environ deux fois plus petit que sous `debug 41` sans optimisation (sonde `PERF` : 251 Ko contre 148 Ko).
- **Impact disque :** exécutables `dev` plus volumineux que les exécutables `cible`.
- **Impact performances :** profil `cible` environ cinq fois plus rapide que sous `debug 41` sans optimisation ; le contrôle de débordement de pile coûte environ 7 %.
- **Impact historique :** neutre.
- **Références normatives :** conditionnement §21, §27 ; manuel ORCA/C chap. 8, 13 ; documentation Golden Gate (Debugging).
- **Statut :** retenu (révisé le 2026-10-25, choix d'Éric, variante R3).

**Défaut constaté du bit 5 (2026-10-23).** Sous `#pragma debug` avec le bit 5 (profil `dev`) (contrôle des pointeurs nuls), une incrémentation de pointeur écrite `p++` en instruction, y compris dans la clause d'avancement d'un `for`, provoque un arrêt (`BRK`) sous Golden Gate, même lorsque le pointeur n'est pas nul ; les formes `p = p + 1` et l'accès par indice `p[i]` fonctionnent. Règle de codage : dans `src/` et `tests/`, un parcours de tableau s'écrit par indice ; un pointeur n'est jamais incrémenté par `++` en instruction. L'incrémentation à l'intérieur d'une expression (`*p++`) n'a pas montré le défaut, mais n'est pas recherchée.

---

## 5 bis. Pile du 65816

**IMP-0272** — *Pile du 65816 de 16 Kio pour le programme `LSE`*

- **Sujet :** quelle pile d'appels C donner au programme `LSE` ?
  - Par défaut, un programme ORCA/C emploie la pile allouée par le lanceur, 4 Kio sous le shell ORCA. La bibliothèque d'ORCA/C y loge aussi la page directe de SANE (manuel ORCA/C 2.2, chap. 13, `stacksize`).
  - Le compilateur analyse une ligne par descente récursive (Compilateur §2, IMP-0164). Chaque niveau de parenthèses coûte 238 octets de pile, une fonction imbriquée 284, un indice imbriqué 302.
  - Une ligne valide demande ainsi jusqu'à 11 021 octets sous `main` : `?` suivi de 38 niveaux de parenthèses, 78 caractères.
- **Choix :** `#pragma stacksize 16384` dans `src/plateforme/main.c`, seul fichier `.root` (IMP-0142). Aucune borne d'imbrication n'est codée dans le compilateur : toute ligne valide est acceptée.
- **Alternatives envisagées :**
  1. Pile du lanceur (4 Kio) — écartée. Sur l'Apple IIGS, le programme dispose de 3 555 octets sous `main`, et `?(((((((1)))))))`, ligne valide de 16 caractères, déclenche le contrôle de débordement (`#pragma debug` bit 0, IMP-0143). Le programme s'arrête, et le programme courant non rangé est perdu.
  2. 12 Kio, avec les tampons de `corps_afficher` (compilateur) et de `decompilateur_decompiler` rendus statiques — écartée : marge plus faible au pire cas, et contraire à la justification de réentrance d'IMP-0165.
  3. 8 Kio, avec une borne d'imbrication signalée par l'erreur C 107 — écartée : des lignes valides seraient refusées, ce qui modifierait un comportement L.S.E. observable pour une raison de plateforme.
  4. 32 Kio — écartée : double la place prise en banque 0, sans besoin mesuré.
- **Justification :** mesures sur l'Apple IIGS du 2026-10-29 (`05-IMPLEMENTATION/audits/Audit_code_2026-10-29.md`, AC-01 et §3).
  - Avec la pile du lanceur, 3 555 octets sont disponibles sous `main` : 6 niveaux de parenthèses passent (3 405 octets), 7 débordent (3 643 octets).
  - Avec `#pragma stacksize 16384`, 15 840 octets sont disponibles sous `main`, soit 4 819 octets de marge au pire cas d'une ligne valide. Le chargeur lance le programme sous le shell ORCA, et `?` suivi de 7, 20 et 38 niveaux de parenthèses est accepté.
  - Le manuel ORCA/C admet couramment 32 Kio en environnement texte (chap. 13, `stacksize`).
  - Le conditionnement §21 demande un fonctionnement fiable sur cible.
- **Impact mémoire :** 16 Kio de banque 0 au lieu de 4 Kio, soit 12 Kio de plus.
- **Impact disque :** l'exécutable porte son segment de pile : 223 951 octets au lieu de 207 413 en profil `cible`.
- **Impact performances :** nul.
- **Impact historique :** neutre ; toute ligne attestée ou valide reste acceptée.
- **Références normatives :** conditionnement §21 ; Compilateur §2, IMP-0164, IMP-0165 ; IMP-0142, IMP-0143 ; manuel ORCA/C 2.2, chap. 13 (`stacksize`, `debug`) ; audit du code du 2026-10-29 (AC-01).
- **Statut :** retenu (Éric, 2026-10-29).

**Écart entre Golden Gate et l'Apple IIGS.**
- Pour une même `stacksize`, Golden Gate laisse au programme 270 octets de pile de plus que l'Apple IIGS. Cet écart a été mesuré à 4 Kio et à 16 Kio par les sondes `PILED`, `PILE4K` et `PILE16`, le 2026-10-29 ; leurs sources sont en annexe de l'audit.
- Sans `#pragma stacksize`, Golden Gate fournit environ 16 Kio, quatre fois la pile du shell ORCA.
- Une mesure de pile faite sous Golden Gate se corrige donc de 270 octets avant d'être appliquée à la cible.
- Les suites de tests ont chacune leur propre `main`, sans cette directive : elles s'exécutent avec la pile par défaut de Golden Gate.

**Contrôle de non-régression** (audit du code, AC-11 ; choix d'Éric du 2026-10-30).
- La suite `plateforme_pile` (`tests/plateforme/plateforme_pile.c`, IMP-0192) est construite avec `#pragma stacksize 16384`, comme `LSE`.
- Elle joue par `moniteur_session` les sessions les plus exigeantes :
  - `?` suivi de 38 parenthèses, le pire cas d'une ligne valide ;
  - une erreur de compilation au fond de 37 parenthèses ;
  - 15 `ABS` imbriqués et des `SI` imbriqués en expression ;
  - des lignes de programme de 36 et 33 niveaux, listées puis exécutées ;
  - des formats et des réels listés ;
  - GARER, CHARGER et CATALOGUER ;
  - ENCODER et DECODER ;
  - une fonction récursive.
- Avant chaque session, la pile libre est peinte d'un motif ; l'octet peint le plus bas qui a changé donne la profondeur atteinte sous `main`.
- Chaque profondeur doit rester sous 14 816 octets : les 15 840 octets disponibles pour LSE sur l'Apple IIGS, moins 1 Kio réservé à SANE et à GS/OS, que Golden Gate ne compte pas. La profondeur ne dépend pas de la machine ; la pile disponible, si. La borne porte donc sur la profondeur.

---

## 6. Convention d'en-tête de fichier source

```c
/* contexte_exec.c — Environnement d'exécution (VM-2207)
   EXI: (à compléter)  VM: VM-2207  IV: (à compléter)
   IMP: IMP-0136, IMP-0141, IMP-0142, IMP-0143 */
#pragma noroot

#include "contexte_exec.h"
#include "../interfaces/types_communs.h"
#include "../interfaces/profil.h"

segment "contexte_exec";

/* ... */
```

**Inclusions entre en-têtes.** ORCA/C résout un `#include "…"` relatif par rapport au répertoire courant de la compilation, y compris lorsqu'il figure dans un en-tête inclus (constaté le 2026-10-22). Tous les répertoires de compilation (`src/noyau`, `src/plateforme`, `tests/<niveau>`) sont à deux niveaux sous la racine du dépôt (IMP-0140) : dans un en-tête, une inclusion vers un autre en-tête du dépôt s'écrit donc `#include "../../src/<répertoire>/<fichier>.h"`, forme valable depuis chacun d'eux. Dans un fichier `.c` de `src/`, la forme courte ci-dessus (`"../interfaces/…"`, ou le nom seul pour un en-tête du même répertoire) reste valable, le fichier étant compilé depuis son propre répertoire.

`plateforme/main.c` est le seul fichier sans `#pragma noroot` et sans déclaration `segment` (il reste dans le segment blanc, comme `moniteur.c`, `analyseur.c`, `erreurs.c` et `console_iigs.c`).

---

## 7. Makefile

```makefile
# build/Makefile — construction du LSE-IIGS via Golden Gate / ORCA/C 2.2.1
# Décisions appliquées : IMP-0135, IMP-0136, IMP-0140 à IMP-0143, IMP-0192, IMP-0193.
# À lancer depuis build/ : « make tests » (suites de tests), « make » (exécutable LSE).
# Chaque fichier est compilé depuis son propre répertoire : ORCA/C résout les
# #include "..." relatifs par rapport au répertoire courant (Construction §7).
# -I : aucun fichier .sym (en-têtes précompilés) n'est lu ni écrit.

IIX       := iix
SRC       := ../src
TST       := ../tests
TESTS_BIN := tests
PROFIL    ?= dev
PROGRAMME := LSE
KEEPTYPE  := EXE
CFLAGS    := -P -I +T
LFLAGS    := -P

ifeq ($(PROFIL),dev)
PROFILFLAG := cc=-dLSE_PROFIL_DEV=1
else
PROFILFLAG :=
endif

# Sondes portées par l'image d'essai ESSAIS.po ; PERF ne mesure que le profil
# cible, et n'est pas construite dans le profil dev. NOMBRES est la suite
# composants_nombres, à passer sur cible : sous Golden Gate, le format étendu
# de SANE n'a que 53 bits (audit du code, AP-04).
SONDES_IMAGE := $(TESTS_BIN)/sonde_console:SONDE:EXE $(TESTS_BIN)/sonde_fichiers:SONDEF:EXE \
                $(TESTS_BIN)/sonde_erreur:SONDEE:EXE
SONDES_DEP   := $(TESTS_BIN)/sonde_console $(TESTS_BIN)/sonde_fichiers $(TESTS_BIN)/sonde_erreur
ifneq ($(PROFIL),dev)
SONDES_IMAGE += $(TESTS_BIN)/sonde_performance:PERF:EXE $(TESTS_BIN)/composants_nombres:NOMBRES:EXE
SONDES_DEP   += $(TESTS_BIN)/sonde_performance $(TESTS_BIN)/composants_nombres
endif

# Dépendances de toute compilation (audit du code, AC-34) : tous les en-têtes
# du dépôt, les inclusions indirectes n'étant pas suivies règle par règle, et
# le témoin du profil, réécrit seulement quand PROFIL change ; modifier un
# en-tête ou changer de profil recompile donc tout ce que la cible demande.
ENTETES := $(wildcard $(SRC)/interfaces/*.h $(SRC)/noyau/*.h $(SRC)/plateforme/*.h \
                      $(TST)/commun/*.h $(TST)/composants/*.h)
TEMOIN  := profil.temoin
$(shell test "`cat $(TEMOIN) 2>/dev/null`" = "$(PROFIL)" || echo "$(PROFIL)" > $(TEMOIN))
DEPENDANCES := $(ENTETES) $(TEMOIN)

IMAGE     := LSE.po
VOLUME    := LSE
ESSAIS    := ESSAIS.po
RECONST   := ../02-HISTORIQUE/Reconstitution-PERFO-CHARG
RUBANS    := ruban_CHARG ruban_PERFO

.PHONY: all tests clean sonde image

all: $(PROGRAMME) $(IMAGE) $(ESSAIS)

# --- point d'entrée (seul .root, cf. IMP-0142) ---
main.root main.a: $(SRC)/plateforme/main.c $(DEPENDANCES)
	cd $(SRC)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) main.c keep=../../build/main

# --- src/noyau/ ---
analyseur.a: $(SRC)/noyau/analyseur.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) analyseur.c keep=../../build/analyseur

moniteur.a: $(SRC)/noyau/moniteur.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) moniteur.c keep=../../build/moniteur

programme.a: $(SRC)/noyau/programme.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) programme.c keep=../../build/programme

compilateur.a: $(SRC)/noyau/compilateur.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) compilateur.c keep=../../build/compilateur

interprete.a: $(SRC)/noyau/interprete.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) interprete.c keep=../../build/interprete

pile.a: $(SRC)/noyau/pile.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) pile.c keep=../../build/pile

contexte_exec.a: $(SRC)/noyau/contexte_exec.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) contexte_exec.c keep=../../build/contexte_exec

decompilateur.a: $(SRC)/noyau/decompilateur.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) decompilateur.c keep=../../build/decompilateur

bibliotheque.a: $(SRC)/noyau/bibliotheque.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) bibliotheque.c keep=../../build/bibliotheque

erreurs.a: $(SRC)/noyau/erreurs.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) erreurs.c keep=../../build/erreurs

donnees.a: $(SRC)/noyau/donnees.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) donnees.c keep=../../build/donnees

objets.a: $(SRC)/noyau/objets.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) objets.c keep=../../build/objets

rcim.a: $(SRC)/noyau/rcim.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) rcim.c keep=../../build/rcim

symboles.a: $(SRC)/noyau/symboles.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) symboles.c keep=../../build/symboles

codage.a: $(SRC)/noyau/codage.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) codage.c keep=../../build/codage

ruban.a: $(SRC)/noyau/ruban.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) ruban.c keep=../../build/ruban

taches.a: $(SRC)/noyau/taches.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) taches.c keep=../../build/taches

fichiers.a: $(SRC)/noyau/fichiers.c $(DEPENDANCES)
	cd $(SRC)/noyau && $(IIX) compile $(CFLAGS) $(PROFILFLAG) fichiers.c keep=../../build/fichiers

# --- src/plateforme/ ---
console_iigs.a: $(SRC)/plateforme/console_iigs.c $(DEPENDANCES)
	cd $(SRC)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) console_iigs.c keep=../../build/console_iigs

fichiers_iigs.a: $(SRC)/plateforme/fichiers_iigs.c $(DEPENDANCES)
	cd $(SRC)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) fichiers_iigs.c keep=../../build/fichiers_iigs

horloge_iigs.a: $(SRC)/plateforme/horloge_iigs.c $(DEPENDANCES)
	cd $(SRC)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) horloge_iigs.c keep=../../build/horloge_iigs

memoire_iigs.a: $(SRC)/plateforme/memoire_iigs.c $(DEPENDANCES)
	cd $(SRC)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) memoire_iigs.c keep=../../build/memoire_iigs

# --- édition de liens : main en premier (manuel ORCA/C, chap. 7) ---
$(PROGRAMME): main.root main.a moniteur.a analyseur.a programme.a compilateur.a \
              interprete.a contexte_exec.a pile.a decompilateur.a bibliotheque.a fichiers.a codage.a ruban.a taches.a fichiers_iigs.a erreurs.a \
              donnees.a objets.a rcim.a symboles.a console_iigs.a horloge_iigs.a memoire_iigs.a
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) main moniteur analyseur programme compilateur interprete bibliotheque fichiers codage ruban taches fichiers_iigs contexte_exec pile decompilateur erreurs donnees objets rcim symboles console_iigs horloge_iigs memoire_iigs keep=$(PROGRAMME)

# --- images disque ProDOS pour l'Apple IIGS ou l'émulateur (IMP-0241) ---
# LSE.po, image de distribution (volume /LSE/) : LSE, de type EXE, et les
# rubans des programmes CHARG et PERFO, de type TXT, qui se rangent eux-mêmes
# à la lecture. ESSAIS.po (volume /ESSAIS/) : les sondes, de type EXE.
image: $(IMAGE) $(ESSAIS)

ruban_%: $(RECONST)/%.lse ../scripts/creer_ruban_programme.sh
	../scripts/creer_ruban_programme.sh $< $* $@

$(IMAGE): $(PROGRAMME) $(RUBANS) ../scripts/creer_image.sh
	../scripts/creer_image.sh $(IMAGE) $(VOLUME) $(PROGRAMME):LSE:EXE \
	    ruban_CHARG:CHARG:TXT ruban_PERFO:PERFO:TXT

$(ESSAIS): $(SONDES_DEP) ../scripts/creer_image.sh
	../scripts/creer_image.sh $(ESSAIS) ESSAIS $(SONDES_IMAGE)

# --- suites de tests (IMP-0192) ---
# Une règle explicite par suite : compilation du fichier de la suite (qui
# contient main), puis édition de liens, la suite en premier.

SUITES := types_plateforme composants_objets representation_rcim composants_symboles composants_erreurs \
          composants_compilateur_2a composants_compilateur_2b composants_compilateur_2c \
          composants_compilateur_2d composants_compilateur_t1600 \
          composants_compilateur_limites historiques_erreurs \
          composants_decompilateur historiques_decompilation \
          composants_programme composants_contexte composants_donnees \
          composants_interprete composants_entrees_sorties composants_horloge \
          plateforme_horloge composants_analyseur composants_moniteur composants_nombres plateforme_fichiers composants_bibliotheque composants_fichiers \
          composants_codage composants_taches plateforme_pile

$(TESTS_BIN)/types_plateforme: $(TST)/representation/types_plateforme.c $(DEPENDANCES)
	mkdir -p $(TESTS_BIN)
	cd $(TST)/representation && $(IIX) compile $(CFLAGS) $(PROFILFLAG) types_plateforme.c keep=../../build/$(TESTS_BIN)/types_plateforme
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/types_plateforme keep=$(TESTS_BIN)/types_plateforme

$(TESTS_BIN)/composants_objets: $(TST)/composants/composants_objets.c $(DEPENDANCES) objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_objets.c keep=../../build/$(TESTS_BIN)/composants_objets
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_objets objets memoire_iigs keep=$(TESTS_BIN)/composants_objets

$(TESTS_BIN)/representation_rcim: $(TST)/representation/representation_rcim.c $(DEPENDANCES) rcim.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/representation && $(IIX) compile $(CFLAGS) $(PROFILFLAG) representation_rcim.c keep=../../build/$(TESTS_BIN)/representation_rcim
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/representation_rcim rcim keep=$(TESTS_BIN)/representation_rcim

$(TESTS_BIN)/composants_symboles: $(TST)/composants/composants_symboles.c $(DEPENDANCES) symboles.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_symboles.c keep=../../build/$(TESTS_BIN)/composants_symboles
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_symboles symboles objets memoire_iigs keep=$(TESTS_BIN)/composants_symboles

$(TESTS_BIN)/composants_erreurs: $(TST)/composants/composants_erreurs.c $(DEPENDANCES) erreurs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_erreurs.c keep=../../build/$(TESTS_BIN)/composants_erreurs
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_erreurs erreurs keep=$(TESTS_BIN)/composants_erreurs

$(TESTS_BIN)/composants_compilateur_2a: $(TST)/composants/composants_compilateur_2a.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_2a.c keep=../../build/$(TESTS_BIN)/composants_compilateur_2a
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_2a compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_2a

$(TESTS_BIN)/composants_compilateur_2b: $(TST)/composants/composants_compilateur_2b.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_2b.c keep=../../build/$(TESTS_BIN)/composants_compilateur_2b
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_2b compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_2b

$(TESTS_BIN)/composants_compilateur_2c: $(TST)/composants/composants_compilateur_2c.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_2c.c keep=../../build/$(TESTS_BIN)/composants_compilateur_2c
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_2c compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_2c

$(TESTS_BIN)/composants_compilateur_t1600: $(TST)/composants/composants_compilateur_t1600.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_t1600.c keep=../../build/$(TESTS_BIN)/composants_compilateur_t1600
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_t1600 compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_t1600

$(TESTS_BIN)/composants_compilateur_2d: $(TST)/composants/composants_compilateur_2d.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_2d.c keep=../../build/$(TESTS_BIN)/composants_compilateur_2d
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_2d compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_2d

$(TESTS_BIN)/composants_compilateur_limites: $(TST)/composants/composants_compilateur_limites.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_compilateur_limites.c keep=../../build/$(TESTS_BIN)/composants_compilateur_limites
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_compilateur_limites compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_compilateur_limites

$(TESTS_BIN)/historiques_erreurs: $(TST)/historiques/historiques_erreurs.c $(DEPENDANCES) compilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/historiques && $(IIX) compile $(CFLAGS) $(PROFILFLAG) historiques_erreurs.c keep=../../build/$(TESTS_BIN)/historiques_erreurs
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/historiques_erreurs compilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/historiques_erreurs

$(TESTS_BIN)/composants_decompilateur: $(TST)/composants/composants_decompilateur.c $(DEPENDANCES) compilateur.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_decompilateur.c keep=../../build/$(TESTS_BIN)/composants_decompilateur
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_decompilateur compilateur decompilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_decompilateur

$(TESTS_BIN)/historiques_decompilation: $(TST)/historiques/historiques_decompilation.c $(DEPENDANCES) compilateur.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/historiques && $(IIX) compile $(CFLAGS) $(PROFILFLAG) historiques_decompilation.c keep=../../build/$(TESTS_BIN)/historiques_decompilation
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/historiques_decompilation compilateur decompilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/historiques_decompilation

$(TESTS_BIN)/composants_programme: $(TST)/composants/composants_programme.c $(DEPENDANCES) compilateur.a programme.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_programme.c keep=../../build/$(TESTS_BIN)/composants_programme
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_programme compilateur programme rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_programme

$(TESTS_BIN)/composants_contexte: $(TST)/composants/composants_contexte.c $(DEPENDANCES) compilateur.a programme.a contexte_exec.a pile.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a donnees.a decompilateur.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_contexte.c keep=../../build/$(TESTS_BIN)/composants_contexte
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_contexte compilateur programme contexte_exec pile donnees decompilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_contexte

$(TESTS_BIN)/composants_donnees: $(TST)/composants/composants_donnees.c $(DEPENDANCES) donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_donnees.c keep=../../build/$(TESTS_BIN)/composants_donnees
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_donnees donnees decompilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_donnees

$(TESTS_BIN)/composants_interprete: $(TST)/composants/composants_interprete.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_interprete.c keep=../../build/$(TESTS_BIN)/composants_interprete
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_interprete compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_interprete

$(TESTS_BIN)/console_bouchon.a: $(TST)/bouchons/console_bouchon.c $(DEPENDANCES)
	mkdir -p $(TESTS_BIN)
	cd $(TST)/bouchons && $(IIX) compile $(CFLAGS) $(PROFILFLAG) console_bouchon.c keep=../../build/$(TESTS_BIN)/console_bouchon

$(TESTS_BIN)/composants_entrees_sorties: $(TST)/composants/composants_entrees_sorties.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_entrees_sorties.c keep=../../build/$(TESTS_BIN)/composants_entrees_sorties
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_entrees_sorties compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_entrees_sorties

$(TESTS_BIN)/horloge_bouchon.a: $(TST)/bouchons/horloge_bouchon.c $(DEPENDANCES)
	mkdir -p $(TESTS_BIN)
	cd $(TST)/bouchons && $(IIX) compile $(CFLAGS) $(PROFILFLAG) horloge_bouchon.c keep=../../build/$(TESTS_BIN)/horloge_bouchon

$(TESTS_BIN)/composants_horloge: $(TST)/composants/composants_horloge.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_horloge.c keep=../../build/$(TESTS_BIN)/composants_horloge
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_horloge compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_horloge

$(TESTS_BIN)/plateforme_horloge: $(TST)/plateforme/plateforme_horloge.c $(DEPENDANCES) horloge_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) plateforme_horloge.c keep=../../build/$(TESTS_BIN)/plateforme_horloge
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/plateforme_horloge horloge_iigs keep=$(TESTS_BIN)/plateforme_horloge

$(TESTS_BIN)/composants_nombres: $(TST)/composants/composants_nombres.c $(DEPENDANCES) donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_nombres.c keep=../../build/$(TESTS_BIN)/composants_nombres
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_nombres donnees decompilateur rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_nombres

$(TESTS_BIN)/plateforme_fichiers: $(TST)/plateforme/plateforme_fichiers.c $(DEPENDANCES) fichiers_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) plateforme_fichiers.c keep=../../build/$(TESTS_BIN)/plateforme_fichiers
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/plateforme_fichiers fichiers_iigs keep=$(TESTS_BIN)/plateforme_fichiers

$(TESTS_BIN)/composants_bibliotheque: $(TST)/composants/composants_bibliotheque.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_bibliotheque.c keep=../../build/$(TESTS_BIN)/composants_bibliotheque
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_bibliotheque compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_bibliotheque

$(TESTS_BIN)/composants_fichiers: $(TST)/composants/composants_fichiers.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_fichiers.c keep=../../build/$(TESTS_BIN)/composants_fichiers
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_fichiers compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_fichiers

$(TESTS_BIN)/composants_codage: $(TST)/composants/composants_codage.c $(DEPENDANCES) codage.a compilateur.a programme.a bibliotheque.a fichiers.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_codage.c keep=../../build/$(TESTS_BIN)/composants_codage
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_codage codage compilateur programme bibliotheque fichiers fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_codage

$(TESTS_BIN)/composants_taches: $(TST)/composants/composants_taches.c $(DEPENDANCES) taches.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_taches.c keep=../../build/$(TESTS_BIN)/composants_taches
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_taches taches keep=$(TESTS_BIN)/composants_taches

$(TESTS_BIN)/composants_analyseur: $(TST)/composants/composants_analyseur.c $(DEPENDANCES) analyseur.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_analyseur.c keep=../../build/$(TESTS_BIN)/composants_analyseur
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_analyseur analyseur keep=$(TESTS_BIN)/composants_analyseur

$(TESTS_BIN)/composants_moniteur: $(TST)/composants/composants_moniteur.c $(DEPENDANCES) moniteur.a analyseur.a compilateur.a programme.a interprete.a bibliotheque.a fichiers.a codage.a ruban.a taches.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/composants && $(IIX) compile $(CFLAGS) $(PROFILFLAG) composants_moniteur.c keep=../../build/$(TESTS_BIN)/composants_moniteur
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/composants_moniteur moniteur analyseur compilateur programme interprete bibliotheque fichiers codage ruban taches fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/composants_moniteur

# Pile du 65816 (IMP-0272 ; audit du code, AC-11) : profondeur des sessions les plus exigeantes.
$(TESTS_BIN)/plateforme_pile: $(TST)/plateforme/plateforme_pile.c $(DEPENDANCES) moniteur.a analyseur.a compilateur.a programme.a interprete.a bibliotheque.a fichiers.a codage.a ruban.a taches.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a $(TESTS_BIN)/console_bouchon.a $(TESTS_BIN)/horloge_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) plateforme_pile.c keep=../../build/$(TESTS_BIN)/plateforme_pile
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/plateforme_pile moniteur analyseur compilateur programme interprete bibliotheque fichiers codage ruban taches fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon $(TESTS_BIN)/horloge_bouchon rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/plateforme_pile

# Sonde du pilote .CONSOLE (Moniteur IMP-0240) : hors de « make tests », car
# Golden Gate ne fournit pas le pilote ; à lancer sur cible.
sonde: $(TESTS_BIN)/sonde_console $(TESTS_BIN)/sonde_performance $(TESTS_BIN)/sonde_fichiers

$(TESTS_BIN)/sonde_fichiers: $(TST)/plateforme/sonde_fichiers.c $(DEPENDANCES)
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) sonde_fichiers.c keep=../../build/$(TESTS_BIN)/sonde_fichiers
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/sonde_fichiers keep=$(TESTS_BIN)/sonde_fichiers

$(TESTS_BIN)/sonde_performance: $(TST)/plateforme/sonde_performance.c $(DEPENDANCES) compilateur.a programme.a interprete.a bibliotheque.a fichiers.a ruban.a fichiers_iigs.a contexte_exec.a pile.a donnees.a decompilateur.a rcim.a symboles.a erreurs.a objets.a memoire_iigs.a horloge_iigs.a $(TESTS_BIN)/console_bouchon.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) sonde_performance.c keep=../../build/$(TESTS_BIN)/sonde_performance
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/sonde_performance compilateur programme interprete bibliotheque fichiers ruban fichiers_iigs contexte_exec pile donnees decompilateur $(TESTS_BIN)/console_bouchon horloge_iigs rcim symboles erreurs objets memoire_iigs keep=$(TESTS_BIN)/sonde_performance

$(TESTS_BIN)/sonde_console: $(TST)/plateforme/sonde_console.c $(DEPENDANCES) memoire_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) sonde_console.c keep=../../build/$(TESTS_BIN)/sonde_console
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/sonde_console memoire_iigs keep=$(TESTS_BIN)/sonde_console

# SystemError rend la console (IMP-0273 ; audit du code, AC-12) : hors de « make tests »
# (le programme s'arrête volontairement sur « Stack overflow », code 255).
$(TESTS_BIN)/sonde_erreur: $(TST)/plateforme/sonde_erreur.c $(DEPENDANCES) console_iigs.a
	mkdir -p $(TESTS_BIN)
	cd $(TST)/plateforme && $(IIX) compile $(CFLAGS) $(PROFILFLAG) sonde_erreur.c keep=../../build/$(TESTS_BIN)/sonde_erreur
	$(IIX) -DKeepType=$(KEEPTYPE) link $(LFLAGS) $(TESTS_BIN)/sonde_erreur console_iigs keep=$(TESTS_BIN)/sonde_erreur

tests: $(addprefix $(TESTS_BIN)/,$(SUITES))
	@echecs=0; \
	for s in $(SUITES); do \
	    $(IIX) $(TESTS_BIN)/$$s || echecs=$$((echecs + 1)); \
	done; \
	echo "BILAN : $(words $(SUITES)) suite(s), $$echecs suite(s) en echec"; \
	test $$echecs -eq 0

clean:
	rm -f *.root *.a *.sym $(PROGRAMME) $(IMAGE) $(ESSAIS) $(RUBANS) $(TEMOIN)
	rm -rf $(TESTS_BIN)
```

---

## 7 bis. Construction et exécution des tests

**IMP-0192** — *Suites de tests compilées et exécutées sous Golden Gate ; bouchons confinés à `tests/bouchons/` ; `make tests`*

- **Sujet :** comment construire et exécuter les suites de tests prévues par Organisation du dépôt (§4, §6, IMP-0138), et comment tester un composant dont les voisins ne sont pas encore écrits, l'ordre de développement amendé (conditionnement §17) commençant par la représentation, le compilateur et le décompilateur avant l'interpréteur ?
- **Choix :**
  1. **Environnement.** Toutes les suites sont compilées par ORCA/C 2.2.1 et exécutées sous Golden Gate (IMP-0137, IMP-0138). Aucune suite n'est compilée par un compilateur de l'hôte.
  2. **Forme d'une suite.** Une suite est un programme ORCA/C autonome, `tests/<niveau>/<nom>.c`, dont `main` exécute les vérifications et renvoie le nombre d'échecs comme code de sortie, borné à 255. Le niveau est l'un des répertoires d'Organisation du dépôt §6 (`representation`, `composants`, `interfaces`, `conformite`, `historiques`, `cible`). Les vérifications passent par l'unique en-tête `tests/commun/verif.h`, qui fournit des macros reposant sur `stdio.h`, `__FILE__` et `__LINE__`, sans aucune bibliothèque extérieure. `stdio.h` n'est employé que dans `tests/`, jamais dans `src/noyau/`.
  3. **Bouchons.** Un bouchon est une réalisation minimale d'une interface du noyau qui n'est pas encore écrite. Il implémente les mêmes fonctions, déclarées par le même en-tête `src/noyau/<module>.h`, dans un fichier `tests/bouchons/<module>_bouchon.c`. Son en-tête de fichier cite le contrat qu'il imite (par exemple IV-IF-0002 pour le programme courant) et le chapitre du Tome V qui le remplacera. Un bouchon n'est jamais placé dans `src/` ni lié dans l'exécutable `LSE`. Une suite est liée soit avec le module réel, soit avec son bouchon, jamais avec les deux. Les modules de `src/plateforme/` ne sont jamais bouchonnés : leur réalisation réelle s'exécute sous Golden Gate.
  4. **Construction.** La cible `tests` de `build/Makefile` compile les modules et les bouchons nécessaires, lie chaque suite par une règle explicite (IMP-0140) en plaçant le fichier de la suite en premier (manuel ORCA/C, chap. 7), puis exécute chaque suite par `iix` et affiche un bilan. Elle échoue si une suite échoue. Les exécutables de test sont produits dans `build/tests/`. Les suites sont construites dans le profil `dev` (IMP-0143), sauf celles de `tests/cible/`, construites dans le profil `cible`.
  5. **Contraintes de Golden Gate**, constatées le 2026-10-22 avec `iix` 2.1.0 :
     - `iix compile` renvoie un code non nul sur une erreur de compilation, et `make` s'arrête ;
     - le code de retour de `main` est transmis à l'hôte ;
     - le Memory Manager répond à `NewHandle`, `HLock`, `HUnlock`, `DisposeHandle`, `GetHandleSize`, `MaxBlock` et `TotalMem` ;
     - `FreeMem` et `RealFreeMem` ne sont pas implémentés et arrêtent le programme : aucun code exécuté sous Golden Gate ne les appelle, et la mesure d'IMP-0134 se fait sur cible.
- **Alternatives envisagées :**
  1. Compiler les tests du noyau avec un compilateur de l'hôte — écartée : elle introduirait un second compilateur, aux règles différentes de celles d'ORCA/C 2.2.1, alors que la chaîne Golden Gate est disponible et rapide ; IMP-0137 écarte déjà les substituts de l'hôte.
  2. Placer les bouchons dans `src/`, activés par compilation conditionnelle — écartée : le code livré contiendrait du code de test, et un oubli de configuration pourrait lier un bouchon dans l'exécutable `LSE`.
  3. Utiliser une bibliothèque de tests existante — écartée : aucune n'est disponible pour ORCA/C sur la cible, et quelques macros suffisent.
- **Justification :** conditionnement §16 (niveaux de tests) et §17 (chaque étape exécutable et testable) ; IMP-0137, IMP-0138, IMP-0140.
- **Impact mémoire :** sans objet pour l'exécutable `LSE`.
- **Impact disque :** des exécutables de test dans `build/tests/`.
- **Impact performances :** sans objet.
- **Impact historique :** neutre.
- **Références normatives :** conditionnement §16, §17 ; Organisation du dépôt §4, §6 ; IMP-0134, IMP-0137, IMP-0138, IMP-0140, IMP-0143 ; manuel ORCA/C chap. 7.
- **Statut :** retenu (choix d'Éric du 2026-10-22).

---

## 7 ter. Image disque pour l'Apple IIGS ou l'émulateur

**IMP-0241** — *`make` produit une image ProDOS de 800 Ko portant les fichiers cibles*
- Sujet : comment transférer les fichiers cibles sur l'Apple IIGS ou un émulateur (demande d'Éric du 2026-10-24) ?
- Choix : la cible `image` du Makefile, comprise dans `make` (`all`), appelle `scripts/creer_image.sh IMAGE VOLUME FICHIER:NOM:TYPE...`. Le script crée par AppleCommander 1.9.0 (`-pro800`) une image ProDOS vierge de 800 Ko, `build/LSE.po`, volume `/LSE/`, puis y ajoute (`-p`) le programme `LSE` et les sondes `SONDE` (pilote de console), `SONDEF` (opérations de fichier) et, dans le profil `cible` seulement, `PERF` (durées des primitives de l'interpréteur) et `NOMBRES` (suite `composants_nombres`, ajoutée le 2026-10-30 : sous Golden Gate, le format étendu de SANE n'a que 53 bits, et l'équivalence du calcul en format étendu d'IMP-0216 ne se vérifie que sur l'Apple IIGS), tous de type ProDOS `EXE`. Dans le profil `dev`, `LSE` (455 Ko) et `PERF` (420 Ko) ne tiennent pas ensemble sur 800 Ko, et `PERF` ne mesure que le profil `cible` (constat du 2026-10-25). Il reprend `03-APPLE-IIGS/outils/add-file.sh` et `get-binaries.sh` (téléchargement d'AppleCommander), avec des chemins entre guillemets (le chemin du dépôt contient des espaces) ; en cas d'erreur, l'image partielle est effacée. `make clean` efface l'image.
- Alternatives envisagées : copier l'image `03-APPLE-IIGS/outils/dist.po` — écartée (choix d'Éric du 2026-10-24, option b) : 140 Ko dont 121 blocs libres, quand `LSE` en demande 499, et ses fichiers `A3A2EMU` et `CONFIG` viennent d'un autre projet ; appeler `add-file.sh` tel quel — écartée : ses chemins non protégés échouent sur le chemin du dépôt.
- Justification : demande et choix d'Éric du 2026-10-24.
- Impact mémoire : sans objet. Impact disque : une image de 800 Ko dans `build/`. Impact performances : sans objet.
- Impact historique : nul.
- Références normatives : Construction §7 ; Moniteur et console IMP-0240.
- Statut : retenu (Éric, 2026-10-24, option b).
- Complément du 2026-11-06 (choix d'Éric, rédaction du manuel de l'utilisateur, version 1.0) : `LSE.po` devient l'image de distribution, et les sondes passent sur une image d'essai.
  - **`LSE.po`** (volume `/LSE/`) porte `LSE`, de type `EXE`, et les rubans `CHARG` et `PERFO`, de type `TXT`. Ces deux programmes font partie de la distribution. Chaque ruban est produit par `scripts/creer_ruban_programme.sh` à partir de `02-HISTORIQUE/Reconstitution-PERFO-CHARG/CHARG.lse` et `PERFO.lse` : lignes terminées par CR (Bibliothécaire et fichiers IMP-0262), puis `RA⊗nom⊗EF⊗*⊗`, ⊗ notant X-OFF et deux nuls. Lu une fois (`LSE -RUBAN :LSE:CHARG`, `BO`, X-ON, `AU`), le ruban range le programme dans la bibliothèque. Sous `-T1600` et `-MITRA15`, la session doit s'ouvrir sous une identification qui a l'attribut `P` (`-ID 1`).
  - **`ESSAIS.po`** (volume `/ESSAIS/`) porte les sondes `SONDE`, `SONDEF`, `SONDEE` et, dans le profil `cible`, `PERF` et `NOMBRES`. Le manuel de l'utilisateur ne les décrit pas.
  - `make` (`all`) et `make image` produisent les deux images ; `make clean` les efface, avec les rubans intermédiaires `ruban_CHARG` et `ruban_PERFO`.
  - **`PERFO.po` est supprimée**, avec `scripts/creer_image_perfo.sh`. Les rubans de données `PRONO`, `FICH` et `FICH2`, perforés par `PERFO` et vérifiés par `02-HISTORIQUE/Reconstitution-PERFO-CHARG/essais/verifier.py`, passent sur `DURUY.po` (`scripts/creer_image_duruy.sh`, qui demande `build/LSE` à jour). Ils appellent `CHARG`, qui doit avoir été rangé auparavant depuis `LSE.po`.
  - Essai sous Golden Gate, le 2026-11-06 : les rubans `CHARG` et `PERFO` de `LSE.po`, lus dans une bibliothèque vierge, rangent les deux programmes (`TABLE DES FICHIERS`, `APPELER PERFO`, `LISTER`).

## 8. Récapitulatif

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0140 | Makefile orchestrant `iix compile`/`iix link` | retenu |
| IMP-0141 | Noms `KEEP` identiques aux noms de fichiers source ; seul l'exécutable final respecte la contrainte GS/OS | retenu |
| IMP-0142 | `#pragma noroot` sur tous les fichiers sauf celui contenant `main` | retenu |
| IMP-0143 | Deux profils de compilation (`dev`/`cible`) via `cc=-dLSE_PROFIL_DEV` et `#pragma debug` | retenu |
| IMP-0192 | Suites de tests sous Golden Gate, bouchons dans `tests/bouchons/`, `make tests` | retenu |
| IMP-0272 | Pile du 65816 de 16 Kio pour le programme `LSE` (`#pragma stacksize 16384` dans `main.c`) | retenu |

---

## 9. Traçabilité

```text
conditionnement §5, §21, §27, §20.3
  → Organisation du dépôt : IMP-0135, IMP-0136, IMP-0137, IMP-0138 (limite explicite « construction »)
    → IMP-0140 à IMP-0143, IMP-0192, IMP-0272 (présent chapitre) ; IMP-0193 (Organisation du dépôt)
      → CODE : build/Makefile, src/interfaces/profil.h, src/plateforme/main.c (IMP-0272), tests/commun/verif.h, tests/bouchons/
```

---

## 10. Limites explicites

- Le code de retour de `iix compile` sur erreur a été vérifié le 2026-10-22 (non nul : `make` s'arrête) ; voir §7 bis.
- L'exécution des suites de tests est traitée au §7 bis (IMP-0192).
- Ne couvre pas la procédure de transfert de l'exécutable final vers une cible physique ou un émulateur système complet — livrable distinct (documentation d'installation, conditionnement §20.11).

---

## 11. Tests associés

- Construction complète (`make PROFIL=dev` puis `make PROFIL=cible`) sans erreur, sous Golden Gate.
- Vérification qu'un programme construit en `PROFIL=dev` s'exécute correctement sous `iix -g` (débogueur actif), et vérification séparée qu'un programme construit en `PROFIL=cible` s'exécute sans plantage lorsqu'il est copié et lancé directement depuis le shell ORCA sur cible réelle ou émulateur système complet, sans aucun débogueur installé.
- Vérification qu'aucun exécutable présent dans `tests/cible/` n'a été construit en `PROFIL=dev` (contrôle de non-régression sur IMP-0143).
- Vérification que `make clean` puis `make` reproduit un exécutable identique (construction reproductible).
- Pile (IMP-0272) : sur l'Apple IIGS, le programme `LSE` construit en `PROFIL=cible` se lance sous le shell ORCA et accepte `?` suivi de 38 niveaux de parenthèses (ligne de 78 caractères), qui affiche `1`. Vérifié le 2026-10-29 sur l'exécutable d'essai `LSE16`.
- Pile, non-régression : suite `plateforme_pile` dans `make tests` (§5 bis).