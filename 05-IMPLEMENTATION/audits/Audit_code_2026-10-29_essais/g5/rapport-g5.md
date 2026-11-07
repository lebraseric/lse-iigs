# Audit du code LSE-IIGS — groupe 5 : plateforme, interfaces, budget de la pile du 65816

Date : 2026-10-29. Périmètre : `src/plateforme/*` (`main.c`, `console_iigs.c`, `fichiers_iigs.c`, `horloge_iigs.c`, `memoire_iigs.c` et leurs `.h`), `src/interfaces/*`, et le budget de pile de tout le programme (tâche transversale).

Aucun fichier du dépôt n'a été modifié. Les essais sont dans la copie de travail `S/copie-g5/` (S = bloc-notes de la session) :

- `S/copie-g5/tests/audit/essai_pile.c` : mesure de la profondeur de pile atteinte par une session du moniteur (bouchon de console). Une récursion de sonde va jusqu'au contrôle de `#pragma debug` bit 0 pour trouver l'adresse la plus basse admise ; `SystemError` est remplacée pour reprendre la main par `longjmp`. La zone libre est ensuite peinte d'un motif, la session est jouée, et l'octet peint le plus bas qui a changé donne la profondeur atteinte.
- `essai_pile4k.c` (`#pragma stacksize 4096`) et `essai_pile16k.c` (`#pragma stacksize 16384`) : le même essai avec une pile imposée.
- `essai_debordement.c` : comportement par défaut du contrôle de débordement.
- `essai_atexit.c` : un gestionnaire `atexit` est-il appelé après le débordement ?
- Règles ajoutées à la fin de `S/copie-g5/build/Makefile`.
- Sorties consignées dans `S/outils-g5/mesures_pile.txt`.
- Trace GS/OS (`iix --trace-gsos`) d'un GARER en boucle : `S/outils-g5/trace_garer_4temporaires.txt`.
- Analyse statique des locales et du graphe d'appel : `S/outils-g5/graphe.py`, `chemin.py`, `vers.py`.

Tout a été compilé par ORCA/C 2.2.1 en `PROFIL=cible` (contre-essai en `PROFIL=dev`) et exécuté sous Golden Gate.

---

## 1. Carte des chemins chauds du groupe

### Par ligne exécutée (consultation d'ESC)

- Appel : `src/noyau/interprete.c:1261` (`point_d_arret`) → `console_interruption` (`src/plateforme/console_iigs.c:241-253`). Il a lieu une fois par arrivée sur une ligne.
- Aucune touche frappée : une seule lecture de `$E0C000`, soit 0,0167 ms (IMP-0242). C'est 0,24 % d'un tour de boucle (b) à 7,1 ms. Rien à gagner.
- Une touche frappée : un DReadGS sans attente par touche, plus un DReadGS final vide (0,517 ms chacun), et un DControlGS SetWaitStatus au passage attente ↔ sans attente (`console_iigs.c:131-135`, coût non mesuré).
- Aucun autre appel de plateforme par ligne. Le chargement d'une ligne passe par `objets_copier`, sans `HLock` (IMP-0243).

### Par caractère affiché

L'écriture se fait **par bloc, non par caractère** : un DWriteGS par appel de `console_ecrire` (`console_iigs.c:66-83`).

- AFFICHER : un appel par valeur (`interprete.c:428-433`) ; une chaîne est écrite par tranches de 256 octets (`interprete.c:463-471`).
- Passage à la ligne : 2 DWriteGS et 3 octets, CR puis CR LF (`interprete.c:441-446`, `moniteur.c:75-80`, `console_iigs.c:183-196`).
- LISTER : 3 DWriteGS par ligne listée (`moniteur.c:452` puis `a_la_ligne`).
- Saisie : un DReadGS en attente par touche, plus 2 DWriteGS (écho d'un octet, puis pavé de 4 octets ; `console_iigs.c:226-231`).
- Le coût d'un DWriteGS n'a pas été mesuré.

### Par opération de fichier

Comptes tirés de la trace GS/OS de Golden Gate (comptes exacts ; aucune durée d'appel GS/OS de fichier n'a été mesurée sur l'Apple IIGS) :

- **Accès à un secteur par stdio** (`plateforme_fichier_lire` / `_ecrire`, `fichiers_iigs.c:117-131`) : GetEOF + SetMark + Read ou Write, soit 3 appels. Une lecture-modification-écriture en coûte 6.
- **`plateforme_fichier_taille`** par nom (`fopen` `rb` + `fseek(SEEK_END)` + `ftell` + `fclose`) : 10 appels, soit Open, GetMark, SetMark, 2 × GetEOF, SetMark, 2 × GetMark, SetMark, Close.
- **`plateforme_fichier_lister`** (`fichiers_iigs.c:169-204`) : Open, GetDirEntry (rang absolu), Close, soit 3 appels par rang. L'énumération de k fichiers en coûte 3(k+1).
- **GARER d'un enregistrement dans un fichier temporaire existant** :
  - 23 appels pour l'opération elle-même ;
  - plus 13k + 3 appels pour le contrôle du quota, k étant le nombre de fichiers temporaires ;
  - total : 39 appels avec k = 1, 78 avec k = 4. Détail au constat G5-P-01.
- RANGER et APPELER : quelques dizaines d'appels chacun, pas de chemin chaud.

### Mémoire et horloge

- `memoire_iigs.c` : `HLock` et `HUnlock` à chaque `objets_resoudre` / `objets_relacher` (1,1 ms la paire, mesuré). Le chargement d'une ligne y échappe déjà (IMP-0243).
- `horloge_iigs.c` : `time()` + `localtime()` (ReadTimeHex) à chaque TEM, DAT, ALE(0). Non mesuré ; pas sur un chemin chaud.

---

## 2. Budget de pile de tout le programme

### 2.1 Directives et faits de plateforme

- **Aucune directive de taille de pile dans le dépôt.** `grep -rn stacksize` sur `src/`, `tests/`, `build/` et `05-IMPLEMENTATION/` ne trouve aucun `#pragma stacksize`.
- **Pile par défaut sur l'Apple IIGS : 4 Kio.** Le programme prend la pile du lanceur, soit 4 Kio sous le shell ORCA. Elle sert aussi à la page directe de SANE. Sources : manuel ORCA/C 2.2, chap. 3 p. 19 (« By default, your program has 4K of stack space ») et chap. 13 `stacksize` p. 283 (texte extrait l. 830 et 6438-6442).
- **Ce chiffre n'est pas mesuré sur la machine** : voir §2.6.
- **Coût fixe par appel, mesuré** : dans `essai_debordement`, chaque niveau consomme 45 octets pour 32 octets de locales et un paramètre de 2 octets. Le coût fixe est donc d'environ 11 octets par appel, en plus des paramètres :
  - adresse de retour de JSL : 3 octets ;
  - page directe sauvée : 2 octets ;
  - environ 6 octets de temporaires du compilateur.
- **Paramètres** (manuel ORCA/C chap. 5, l. 1520) : un `char` est passé sur 2 octets, un `float` ou un `double` sur 10 (format étendu de SANE), une structure par valeur à sa taille.

### 2.2 Locales de plus de 64 octets dans tout `src/`

Relevé automatique (`graphe.py`), avec les tailles ORCA/C : `int` 2, `long` 4, pointeur 4, `float` 4, `double` 8. Les blocs GS/OS ont la taille de `gsos.h` de Golden Gate. Seules les locales non `static` sont comptées. Le relevé est recoupé par la mesure : 1 994 octets en statique contre 1 977 mesurés pour `?1`.

| Fonction | Emplacement | Locales (octets) | Principal |
|---|---|---|---|
| `decompilateur_decompiler` | `src/noyau/decompilateur.c:1028` | 1 564 | `Decompilation d` (128 × 12 + 10) |
| `corps_afficher` | `src/noyau/compilateur.c:1258-1269` | 902 | `specs[40]` 440, `brouillon[255]`, `debuts[40]` 80, `texte[79]` |
| `plateforme_fichier_lister` | `src/plateforme/fichiers_iigs.c:171-175` | 587 | `c[200]`, `ResultBuf255` 259, `DirEntryRecGS` 62, `OpenRecGS` 58 |
| `plateforme_fichier_renommer` | `src/plateforme/fichiers_iigs.c:162-163` | 400 | `c1[200]`, `c2[200]` |
| `donnees_ecrire_nombre` | `src/noyau/donnees.c:497` | 366 | `corps[320]` |
| `chiffres_entier` | `src/noyau/donnees.c:466` | 332 | `t[320]` |
| `element` (décompilateur) | `src/noyau/decompilateur.c:672` | 328 | `s[256]` |
| `ecrire_nombre` | `src/noyau/interprete.c:475` | 302 | `t[300]` |
| `garer_ouvert` | `src/noyau/fichiers.c:258` | 300 | `s[256]` |
| `bibliotheque_catalogue_suivant` | `src/noyau/bibliotheque.c:345` | 300 | `t[256]`, `meilleur[16]` |
| `compilateur_compiler` | `src/noyau/compilateur.c:1808` | 298 | `octets[255]` |
| `fichiers_cataloguer` | `src/noyau/fichiers.c:462` | 293 | `s[256]` |
| `interprete_bureau` / `interprete_executer` | `src/noyau/interprete.c:1335` / `1316` | 283 / 280 | `Cycle c` (279, dont `seq[255]`) |
| `correction` | `src/noyau/moniteur.c:1586` | 279 | `res[256]` |
| `parcourir` | `src/noyau/fichiers.c:181` | 277 | `s[256]` |
| `codage_decoder` | `src/noyau/codage.c:76` | 277 | `texte[257]` |
| `lister` | `src/noyau/moniteur.c:410` | 275 | `texte[256]` |
| `commande` | `src/noyau/moniteur.c:1432` | 267 | `t[256]` |
| `lire_entree` | `src/noyau/moniteur.c:199` | 266 | `brut[256]` |
| `fichiers_garer` | `src/noyau/fichiers.c:339` | 266 | `s[256]` |
| `plateforme_bibliotheque_ouvrir` | `src/plateforme/fichiers_iigs.c:76-77` | 263 | `c[200]`, `OpenRecGS` |
| `ecrire_chaine` | `src/noyau/interprete.c:464` | 260 | `t[256]` |
| `nouveau_repertoire`, `fixer_taille`, `ecrire_place`, `ouvrir` | `src/noyau/fichiers.c:217`, `158`, `167`, `146` | 256 à 260 | `s[256]` |
| `plateforme_fichier_creer_texte` | `src/plateforme/fichiers_iigs.c:220` | 224 | `c[200]`, `CreateRecGS` |
| `plateforme_fichier_ouvrir` | `src/plateforme/fichiers_iigs.c:102` | 204 | `c[200]` |
| `plateforme_fichier_supprimer` | `src/plateforme/fichiers_iigs.c:154` | 200 | `c[200]` |
| `declaration_procedure` | `src/noyau/compilateur.c:1508-1510` | 175 | `parametres[32]`, `locaux[32]`, `est_proc[32]` |
| `encoder` | `src/noyau/moniteur.c:1358` | 132 | `m[100]` |
| `section_mitra` | `src/noyau/moniteur.c:1019-1024` | 121 | `m[80]` |
| `ligne_bureau`, `ligne_programme` | `src/noyau/moniteur.c:1542`, `1514` | 113, 112 | `t[100]` |
| `donnees_cnb` | `src/noyau/donnees.c:382` | 104 | `tampon[80]` |
| `erreur_fichier`, `erreur_execution` | `src/noyau/moniteur.c:112`, `101` | 100 | `t[100]` |
| `saisir_nombre` | `src/noyau/interprete.c:626` | 96 | `tampon[80]` |
| `constante_chaine`, `constante_numerique` | `src/noyau/compilateur.c:244`, `211` | 93, 92 | `texte[79]`, `tampon[80]` |
| `table_mitra`, `occupation`, `ligne_t1600` | `src/noyau/moniteur.c:1068`, `1103`, `1120` | 82 à 86 | `m[80]` |
| `identification`, `erreur_dialogue`, `liste_id` | `src/noyau/moniteur.c:746`, `93`, `868` | 69 à 81 | `m[60]`, `t[80]` |
| `compacter` | `src/noyau/objets.c:90` | 74 | `ordre[64]` |
| `plateforme_secteurs_libres` | `src/plateforme/fichiers_iigs.c:274-275` | 72 | `DInfoRecGS`, `VolumeRecGS` |
| `contexte_appeler` | `src/noyau/contexte_exec.c:686` | 71 | — |

### 2.3 Récursions et bornes

Composantes récursives du graphe d'appel (algorithme de Tarjan sur `graphe.json`) :

1. **Expressions du compilateur**, 15 fonctions de `src/noyau/compilateur.c:369-918` (`expression`, `concatenation`, `somme`, `terme_signe`, `terme`, `puissance`, `atome`, `appel_fonction`, `appel_procedure`, `indices`, `reference_variable`, `condition`, `ou_logique`, `et_logique`, `facteur_logique`).
   - Un niveau de parenthèses parcourt `atome → expression → concatenation → somme → terme_signe → terme → puissance → atome`.
   - Il coûte **238 octets, mesuré** (182 octets en statique, l'écart venant des temporaires du compilateur).
   - Un niveau d'appel de fonction (`ABS(`) coûte 284 octets.
   - La seule borne est la longueur de ligne : 79 caractères, soit **38 niveaux de parenthèses au plus** (`?` suivi de 38 `(`, de `1` et de 38 `)`).
   - Compilateur §2 (l. 74 du chapitre) et IMP-0164 tiennent cette borne pour suffisante, « sans qu'aucune limite explicite doive être codée en dur », mais n'en chiffrent jamais le coût en octets de pile.
2. **Instructions du compilateur** : `instruction`, `instruction_si`, `instruction_debut`, `instruction_mot_cle` (`compilateur.c:968-1712`).
   - Environ 97 octets par niveau d'imbrication de `SI … ALORS`.
   - Bornée à environ 5 niveaux par la ligne : moins de 0,5 Kio.
3. **Pas de récursion C** dans l'interpréteur, le décompilateur (pile explicite `PILE_MAX` 128) et les fichiers.
   - Mesure : la fonction L.S.E. récursive `&FACT(25)` atteint 1 957 octets, autant que la session minimale.
   - Les cadres L.S.E. sont alloués dans le tas (`contexte_exec.c:334`).

### 2.4 Chaînes les plus profondes

**Machine de bureau `?` suivie de k parenthèses** (statique, cadre en octets puis cumul) :

```text
main 17 → moniteur_session 53 (70) → correction 286 (356) → ligne_bureau 120 (476)
→ compilateur_compiler 317 (793) → instructions 9 → instruction 45 → instruction_mot_cle 26
→ corps_afficher 913 (1 786) → generer_expression_a 15
→ [expression 30 → concatenation 26 → somme 27 → terme_signe 18 → terme 27 → puissance 26 → atome 28] × (k+1)
→ constante_numerique 105 → strtod (SANE)
```

En statique, k = 0 donne environ 2 100 octets. La mesure donne **1 977 + 238 k** octets sous `main`. Pour k = 38 : **11 021 octets**, le pire cas d'une ligne valide.

**DECODER et LISTER** :

```text
main → moniteur_session → commande 274 → decoder 25 / lister 290 → codage_decoder 290
→ decompilateur_decompiler 1 581 → element 345 → decompilateur_ecrire_reel → … → SANE
```

Environ 2 700 octets en statique, **2 785 mesurés**.

**GARER** :

```text
… → interprete_bureau 304 → cycle → element → garer 54 → fichiers_garer 289 → garer_ouvert 323
→ fichiers_secteurs_temporaires 31 → plateforme_fichier_lister 600 → GS/OS
```

Environ 2 180 octets en statique, **2 239 mesurés** (scénario GARER, CHARGER, CATALOGUER).

### 2.5 Mesures

Profil `cible`, sous Golden Gate, en octets sous la locale de `main`. Le profil `dev` donne +63 octets à la base et le même coût par niveau.

Deux colonnes de marge :

- « Marge 4 Kio » : avec `#pragma stacksize 4096`, la pile utilisable sous `main` mesure 3 825 octets ;
- « Marge 16 Kio » : par défaut sous Golden Gate, ou avec `#pragma stacksize 16384`, elle mesure 16 110 octets.

| Scénario | Sous `main` | Marge 4 Kio | Marge 16 Kio |
|---|---|---|---|
| `BO`, `?1+1`, `AU` | 1 977 | 1 848 | 14 133 |
| `?` et k parenthèses | 1 977 + 238 k | k = 7 : 182 ; **k = 8 : débordement** | k = 38 : 11 021, marge 5 089 |
| `10 AFFICHER` et k parenthèses | 2 007 + 238 k | k = 5 : 628 ; débordement dès k = 8 (calcul) | — |
| `10 A_` et k parenthèses | 1 074 + 238 k | k = 11 : 133 ; **k = 12 : débordement** | — |
| `?SI…ALORS` (3 niveaux) et 5 parenthèses | 3 281 | 544 | 12 829 |
| `?ABS(` × 15 | 6 237 | **débordement** | 9 873 |
| Programme avec une ligne de 22 parenthèses, puis LISTER et EXECUTER | 6 326 | **débordement** | 9 784 |
| ENCODER d'une ligne de 16 parenthèses, puis DECODER | 5 204 | **débordement** | 10 906 |
| LISTER (réels, format) | 2 785 | 1 040 | 13 325 |
| GARER × 4, CHARGER, CATALOGUER | 2 239 | 1 586 | 13 871 |
| `&FACT(25)` récursive | 1 957 | 1 868 | 14 153 |
| Erreur C 19 au fond de 37 parenthèses | 10 762 | **débordement** | 5 348 |

Les appels SANE et GS/OS sont probablement réalisés en code natif par Golden Gate : leur consommation de pile sur l'Apple IIGS n'est donc pas comprise dans ces chiffres (incertitude). Les routines de bibliothèque d'ORCA/C, comme `strtod`, sont du code 65816 et sont comptées.

### 2.6 Contrôle `#pragma debug 1`

D'après le manuel ORCA/C, chap. 13 `debug` p. 269 (l. 6202) :

- le contrôle a lieu à l'entrée de chaque fonction compilée et vérifie qu'il reste la place des locales ;
- le manuel prévient lui-même : « It is still possible to overflow the stack with a very complicated expression or by calling a tool ».

Au déclenchement (manuel chap. 20, p. 365 et suivantes, `SystemError`, l. 8179-8215) :

- l'erreur d'exécution 15, « Stack overflow », passe par `SystemError` ;
- `SystemError` appelle `SystemPrintError` (message sur la sortie standard) puis `SystemErrorLocation`, qui « shuts down the program ».

Mesuré sous Golden Gate :

- message `Stack overflow`, code de sortie 255 (`essai_debordement`) ;
- **le gestionnaire `atexit` n'est pas appelé** (`essai_atexit`) ;
- `SystemError` peut être remplacée par une fonction du programme, et la reprise par `longjmp` fonctionne (`essai_pile`).

Pour LSE, un débordement signifie donc :

- arrêt immédiat, sans message L.S.E. ;
- `console_fermer` et `objets_terminer` non appelés ;
- programme courant perdu.

Coût du contrôle : 7 % (IMP-0143).

### 2.7 Golden Gate

- `iix` fournit environ **16 Kio** de pile (≈ `$0800`–`$47FF`), dont 16 110 octets sous `main`.
- Golden Gate honore `#pragma stacksize` : avec 4096, il reste 3 825 octets utilisables, soit environ 271 octets pour la page directe de SANE et le démarrage.
- Les suites de tests ne voient donc pas les débordements de la cible.

Vérification à faire sur l'Apple IIGS (aucune image n'a été construite ici) :

- taper `?((((((((1))))))))` (8 niveaux) dans LSE : on attend « Stack overflow » et le retour au shell ;
- avec 7 niveaux, on attend `1` ;
- ou lancer `essai_debordement` construit en `cible`, qui imprime l'adresse atteinte tous les 16 niveaux.

### 2.8 Conclusion chiffrée

- **Budget.** Sur l'Apple IIGS, avec la pile par défaut de 4 Kio (3 825 octets sous `main`), la session minimale en consomme 1 977. Il reste 1 848 octets, soit 48 %.
- **Ce qui épuise la marge :**
  - 8 parenthèses imbriquées dans `?` ou AFFICHER ;
  - 12 dans une affectation ;
  - environ 7 appels de fonction imbriqués dans `?`.

  Au-delà, LSE s'arrête (« Stack overflow ») et perd le programme courant.
- **Juste sous le seuil**, la marge restante est de 133 à 182 octets. Elle doit encore absorber la pile réelle de SANE et de GS/OS sur la machine (non mesurée) : un débordement non détecté, par corruption de la page directe de SANE ou de la mémoire de la banque 0, y est possible.
- **Pire cas d'une ligne valide** : 11 021 octets, soit 2,9 fois le budget.
- **Proposition (nouvelle décision IMP, Construction §5)** : `#pragma stacksize 16384` dans `src/plateforme/main.c`. C'est la pile même de Golden Gate, ce qui rend les essais représentatifs. La marge au pire cas est de 5 089 octets.
- **À vérifier sur la machine** : que le chargeur accepte 16 Kio en banque 0 sous le shell ORCA. Le manuel dit que l'on peut « souvent » allouer 32 Kio en environnement texte, et qu'à défaut le programme signale l'erreur au lancement.
- **Compléments**, indépendants :
  - rendre `static` les tampons de `corps_afficher` (−902 octets) et `Decompilation d` (−1 546 octets). Ni l'une ni l'autre fonction n'est réentrante ;
  - une suite de non-régression de pile, sur le modèle d'`essai_pile`, liée avec la même `stacksize` que la cible, avec une borne vérifiée.

---

## 3. Constats

### G5-A-01 — La pile de 4 Kio de la cible est épuisée par une expression moyennement imbriquée : arrêt de LSE

- **Gravité :** bloquant.
- **Statut :** CONFIRMÉ par essai : `essai_pile4k` sous Golden Gate avec `#pragma stacksize 4096` ; les débordements sont observés aux seuils annoncés. La taille de 4 Kio sur l'Apple IIGS vient du manuel et n'est pas mesurée sur la machine.
- **Emplacement :** `src/plateforme/main.c:1-54` (aucune `stacksize`) ; récursion `src/noyau/compilateur.c:587-829` ; grosses locales `compilateur.c:1258-1269` et `decompilateur.c:1028`.
- **Constat :** le compilateur, récursif, coûte 238 octets par niveau de parenthèses. La base atteint 1 977 octets en machine de bureau et 1 074 sur une ligne de programme, pour 3 825 octets disponibles. La borne « mécanique » de 38 niveaux (Compilateur §2) représente 11 Kio.
- **Scénario :** `?((((((((1))))))))` (8 niveaux), `10 A_` suivi de 12 niveaux, `?ABS(ABS(…))` × 7 ou plus, ou ENCODER d'un fichier contenant une telle ligne. Résultat : « Stack overflow », retour au shell, programme courant perdu. Avec 7 niveaux, il reste 182 octets, à partager avec SANE (`strtod` au niveau le plus profond).
- **Référence :** manuel ORCA/C chap. 3 p. 19, chap. 13 p. 269 et 283 ; IMP-0143 (alternative 3 : le risque est connu mais non chiffré) ; IMP-0164 ; Compilateur §2.
- **Proposition :** `#pragma stacksize 16384` dans `main.c`, pire cas mesuré 11 021 octets, marge 5 089. Compléments : tampons `static` (§2.8), suite de non-régression de pile.
- **Décision IMP nécessaire :** oui, nouvelle IMP (Construction §5, taille de pile). Révision de la phrase de Compilateur §2 et d'IMP-0164 (impact mémoire).
- **Essai :** `S/copie-g5/tests/audit/essai_pile4k.c`, `essai_pile.c`, `essai_pile16k.c` ; sorties dans `S/outils-g5/mesures_pile.txt`.

### G5-A-02 — Golden Gate donne 16 Kio de pile : aucune suite ne peut révéler un débordement de la cible

- **Gravité :** majeur.
- **Statut :** CONFIRMÉ (`essai_debordement`, `essai_pile`).
- **Emplacement :** `build/Makefile` (toutes les suites) ; Construction IMP-0192.
- **Constat :**
  - les 29 suites tournent avec 16 110 octets sous `main`, plus de quatre fois le budget de l'Apple IIGS ;
  - aucune suite ne mesure la pile ;
  - les essais sur la machine (sondes) n'emploient que des lignes peu imbriquées.
- **Scénario :** une régression qui ajoute 1 Kio de locales sur le chemin du compilateur passe `make tests` puis plante sur l'Apple IIGS.
- **Référence :** IMP-0192 ; manuel ORCA/C chap. 13 `stacksize`.
- **Proposition :** fixer la même `stacksize` dans `LSE` et dans une suite de pile (sur le modèle d'`essai_pile`), avec une borne vérifiée par scénario (pire cas `?` et 38 parenthèses inclus).
- **Décision IMP nécessaire :** oui, à joindre à la nouvelle IMP de G5-A-01 (révision d'IMP-0192 pour la suite).
- **Essai :** `essai_debordement`, sortie `niveau 352 adresse 0009EA … Stack overflow`.

### G5-A-03 — Arrêt sur débordement : la console n'est pas rendue, rien n'est libéré, le contrôle n'est pas étanche

- **Gravité :** majeur.
- **Statut :** CONFIRMÉ par essai pour le message, le code 255, `atexit` non appelé et le remplacement possible de `SystemError`. PLAUSIBLE pour l'état du shell après l'arrêt : il faudrait l'observer sur la machine.
- **Emplacement :** `src/plateforme/main.c:50-52` (`console_fermer` et `objets_terminer` sur le seul chemin normal) ; `console_iigs.c:85-113`.
- **Constat :**
  - `SystemErrorLocation` arrête le programme sans repasser par `main` ni par `atexit` ;
  - le pilote `.CONSOLE` reste en lecture brute, avec le mot de mouvement à `$0D` (CR sans LF) ;
  - les blocs ne sont pas rendus (IMP-0270) ;
  - aucun diagnostic L.S.E. n'est donné ;
  - le contrôle ne couvre pas les appels de bibliothèque, de SANE ni de GS/OS (manuel, `debug`) : près du seuil, un débordement peut être silencieux.
- **Scénario :** G5-A-01 sur l'Apple IIGS : retour au shell avec une console sans écho, ou CR sans interligne, si le shell ne rétablit pas lui-même les modes du pilote (à vérifier).
- **Référence :** manuel ORCA/C chap. 20 (`SystemError`, `SystemErrorLocation`), chap. 13 `debug` ; IMP-0240, IMP-0270.
- **Proposition :** définir `SystemError` dans `src/plateforme/` : `console_fermer()`, puis `SystemPrintError` et `SystemErrorLocation`, ou un message L.S.E. La correction de fond reste G5-A-01.
- **Décision IMP nécessaire :** oui, nouvelle IMP (ou révision d'IMP-0240).
- **Essai :** `S/copie-g5/tests/audit/essai_atexit.c` (sortie `Stack overflow`, code 255, pas de `gestionnaire atexit appele`).

### G5-A-04 — `composer()` écrit sans borne dans `c[200]` : débordement sur la pile possible

- **Gravité :** mineur.
- **Statut :** CONFIRMÉ (chemin entièrement lu).
- **Emplacement :** `src/plateforme/fichiers_iigs.c:23`, `33-53`, `82`, `194-198` ; appelants `src/noyau/fichiers.c:242-244` et `fichiers_iigs.c:323`.
- **Constat :**
  - le seul contrôle porte sur le préfixe : `strlen(chemin) + 20 >= 200` (l. 82) ;
  - dans `TEMPORAIRES`, un préfixe de 179 caractères, plus `:TEMPORAIRES:` (13) et le zéro final, ne laisse que 7 caractères au nom ;
  - or `plateforme_fichier_lister` rend des noms de 15 caractères au plus, ensuite réouverts ou supprimés par nom.
- **Scénario :** `-BIB` de 172 à 179 caractères et un fichier étranger au nom de 8 à 15 caractères dans `TEMPORAIRES`. À l'IDENTIFICATION, au GARER d'un temporaire ou à BONJOUR, jusqu'à 8 octets sont écrits au-delà de `c[200]`, sur la page directe sauvée et l'adresse de retour. Cas peu réaliste : il faut au moins 11 niveaux de répertoires.
- **Référence :** IMP-0245, IMP-0249.
- **Proposition :** contrôle de longueur dans `composer` avec un code d'échec, ou marge portée à 15 + 13 + 1 = 29.
- **Décision IMP nécessaire :** non.

### G5-A-05 — `plateforme_fichier_ouvrir` traite l'espace du ruban comme une unité amovible (défaut latent)

- **Gravité :** mineur.
- **Statut :** CONFIRMÉ (lecture). Latent : aucun appelant actuel n'ouvre le ruban en création.
- **Emplacement :** `src/plateforme/fichiers_iigs.c:105-109`.
- **Constat :** la condition `e >= ESPACE_UNITE_PROGRAMMES` inclut `ESPACE_RUBAN` (7). `composer(c, ESPACE_RUBAN, NULL)` rend le chemin du fichier ruban, et `creer_repertoire(c)` y crée un répertoire.
- **Scénario :** un futur `plateforme_fichier_ouvrir(ESPACE_RUBAN, …, FICHIER_CREATION)` crée un répertoire `RUBAN` (ou le chemin de `-RUBAN`) ; le `fopen` échoue ensuite et toute PERFORER suivante échoue.
- **Référence :** IMP-0258, IMP-0262.
- **Proposition :** condition `e >= ESPACE_UNITE_PROGRAMMES && e <= ESPACE_UNITE_RACINE`.
- **Décision IMP nécessaire :** non.

### G5-A-06 — Repli stdio : une fin de fichier sur l'entrée fait boucler la session sans fin

- **Gravité :** mineur (Golden Gate seulement).
- **Statut :** CONFIRMÉ par essai.
- **Emplacement :** `src/plateforme/console_iigs.c:125-129` (`EOF` → `CAR_ESC`).
- **Constat :** chaque lecture après la fin de l'entrée rend ESC. Le moniteur affiche `PRET` et relit, indéfiniment ; avant BONJOUR, la boucle est muette.
- **Scénario :** `printf 'BO\n' | iix LSE` donne `BONJOUR`, puis `PRET` répété sans fin. `iix LSE < /dev/null` donne une boucle sans sortie.
- **Référence :** IMP-0240 (repli stdio).
- **Proposition :** à la fin de fichier, rendre une fin de session : AU REVOIR implicite, ou fin du programme par `main`.
- **Décision IMP nécessaire :** oui, révision mineure d'IMP-0240.
- **Essai :** commande ci-dessus, avec le `LSE` de la copie.

### G5-A-07 — Options `-ID` et `-TEMP` non contrôlées

- **Gravité :** mineur.
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `src/plateforme/main.c:28-37`.
- **Constat :**
  - `(unsigned int) atol(…)` tronque à 16 bits ;
  - `-TEMP -5` donne un quota de 4 294 967 291 secteurs (quota supprimé), et `-TEMP X` un quota de 0 ;
  - une option sans argument ou inconnue (`-t1600` par exemple) est ignorée en silence.
- **Scénario :** `-ID 66937` devient l'identification 1401 (14,01) si elle existe ; `-TEMP X` fait échouer tout GARER temporaire.
- **Référence :** IMP-0253, IMP-0268.
- **Proposition :** analyse décimale avec bornes (`IDENTIFICATION_MAX` 65499, quota strictement positif) et message sur la console en cas d'option invalide.
- **Décision IMP nécessaire :** oui, précision d'IMP-0253 et d'IMP-0268 (comportement sur option invalide).

### G5-A-08 — F 102 au lancement : message codé en dur, et repli silencieux sur la bibliothèque par défaut

- **Gravité :** mineur.
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `src/plateforme/main.c:45-49` ; `src/plateforme/fichiers_iigs.c:81-84`.
- **Constat :**
  - `"ERREUR F 102"` ne passe pas par `erreurs_formater_fichier` et ignore donc `-LIBELLES` (IMP-0250) ;
  - si le chemin de `-BIB` est trop long, `prefixe` garde `@:LSE`, dont les sous-répertoires ne sont pas créés, et la session continue sur la bibliothèque par défaut.
- **Scénario :** `LSE -BIB <chemin de 185 caractères>` affiche `ERREUR F 102`, puis RANGER range dans `@:LSE:PROGRAMMES` s'il existe.
- **Référence :** IMP-0245, IMP-0250.
- **Proposition :** employer `erreurs_formater_fichier(ERR_F_BIBLIOTHEQUE, …)`. Sur échec, vider le préfixe ou refuser toute opération de fichier.
- **Décision IMP nécessaire :** non (application d'IMP-0250) ; le comportement après F 102 est à préciser par Éric.

### G5-A-09 — Diagnostics de plateforme confondus : toute erreur d'écriture devient « support plein », tout échec d'ouverture « inexistant »

- **Gravité :** mineur.
- **Statut :** PLAUSIBLE. Manque : vérifier que `fwrite` et `fflush` d'ORCA/C posent bien `errno = ENOSPC` sur un disque plein ; le manuel l'atteste pour `write` (l. 10442).
- **Emplacement :** `src/plateforme/fichiers_iigs.c:111-114` et `128-129`.
- **Constat :** un disque protégé en écriture (GS/OS `$2B`) ou une erreur d'entrée-sortie donnent `PLAT_PLEIN`. Un volume hors ligne donne `PLAT_INEXISTANT`.
- **Scénario :** GARER sur une disquette protégée affiche l'erreur « fichier plein » ou « support plein » au lieu d'une erreur d'entrée-sortie.
- **Référence :** IMP-0249, IMP-0250 ; `errno.h` d'ORCA/C (`ENOSPC` 11, `EIO` 5, `EACCES` 9).
- **Proposition :** `PLAT_PLEIN` seulement si `errno == ENOSPC`, sinon `PLAT_ERREUR`. À l'ouverture en lecture, `PLAT_INEXISTANT` seulement si `errno == ENOENT`.
- **Décision IMP nécessaire :** non (précision de réalisation d'IMP-0249).

### G5-A-10 — Makefile : trois objets de plateforme ne dépendent pas de `profil.h`

- **Gravité :** mineur.
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `build/Makefile:102`, `105`, `108` (`console_iigs.a`, `fichiers_iigs.a`, `horloge_iigs.a` sans `$(INTERFACES)`).
- **Constat :** une modification de `profil.h` (pragmas `debug` et `optimize`) ne recompile pas ces trois modules. Le changement de `PROFIL` n'est suivi par aucune règle ; il est connu et traité par `make clean`.
- **Scénario :** modifier `profil.h` puis `make` donne un `LSE` mixte, dont la console n'a pas le contrôle de pile attendu.
- **Référence :** IMP-0140, IMP-0143.
- **Proposition :** ajouter `$(INTERFACES)` aux trois règles.
- **Décision IMP nécessaire :** non.

### G5-P-01 — Le contrôle du quota fait 13k + 3 appels GS/OS à chaque GARER dans un fichier temporaire

- **Gravité :** mineur, à réévaluer après mesure. Si un appel GS/OS coûte de l'ordre de 1 ms sur la machine, une boucle de GARER est ralentie de façon perceptible.
- **Statut :** CONFIRMÉ pour les comptes (trace `iix --trace-gsos`). PLAUSIBLE pour la durée : aucun coût d'appel GS/OS de fichier n'a été mesuré sur l'Apple IIGS.
- **Emplacement :** plateforme `src/plateforme/fichiers_iigs.c:133-137`, `169-204` ; appelant `src/noyau/fichiers.c:235-249`, appelé aux l. 302 et 349.
- **Constat :** pour chaque fichier temporaire, on fait `plateforme_fichier_lister` (Open, GetDirEntry, Close), puis `fopen`, `fseek(END)`, `ftell` et `fclose` (10 appels). On finit par un `lister` final (3 appels).
- **Estimation :**
  - avec k = 1, 16 appels de quota sur 39 par GARER ;
  - avec k = 4 (trace `S/outils-g5/trace_garer_4temporaires.txt`, l. 514-591), 55 sur 78, soit 70 %.
- **Proposition :** une opération de plateforme qui totalise les secteurs d'un espace en une passe de répertoire : Open, GetDirEntry séquentiel (base 1, déplacement 1, `pCount` 8 pour `eof`), Close.
  - Coût : k + 3 appels. Avec k = 4, le GARER passe de 78 à 30 appels (−62 %) ; avec k = 1, de 39 à 27 (−31 %).
  - Comportement L.S.E. inchangé : même arrondi `(eof + 255) / 256`.
  - Risque : un fichier non ordinaire (sous-répertoire) dans `TEMPORAIRES` serait compté. Il faut filtrer par `fileType`.
- **Décision IMP nécessaire :** oui, révision d'IMP-0249 (opération d'interface nouvelle).
- **Essai :** trace ci-dessus. Durée à mesurer par `SONDEF` sur la machine.

### G5-P-02 — Accès positionnés par stdio : 3 appels GS/OS par secteur

- **Gravité :** mineur.
- **Statut :** CONFIRMÉ pour les comptes (trace). PLAUSIBLE pour le gain en temps (non mesuré).
- **Emplacement :** `src/plateforme/fichiers_iigs.c:117-142`.
- **Constat :**
  - chaque `fseek` d'ORCA/C émet GetEOF puis SetMark ;
  - une lecture ou une écriture de secteur coûte donc 3 appels, et la lecture-modification-écriture de `fixer_taille` et `ecrire_place` en coûte 6 ;
  - `plateforme_fichier_taille` coûte GetEOF, SetMark et GetMark ;
  - `plateforme_fichier_existe` ouvre le fichier, avec allocation d'un tampon de 1 Kio et Open/Close.
- **Estimation :** sur les 23 appels propres d'un GARER, environ 7 GetEOF sont superflus. Un appel GetFileInfo remplacerait les 10 appels de « taille par nom ».
- **Proposition :** appels GS/OS directs pour les fichiers (SetMark + Read/Write, avec le `refNum` mémorisé), et GetFileInfo pour l'existence et la taille. Gain : environ −30 % des appels par accès.
  - Risque : abandon de stdio, choisi par F2 (IMP-0249).
  - Risque : la règle IMP-0243 (aucun appel système entre l'obtention d'une adresse et son dernier usage) concerne `objets.c`, non ces appels, mais il faut le vérifier.
- **Décision IMP nécessaire :** oui, révision d'IMP-0249 (point F2).

---

## 4. Points écartés après vérification

- **Consultation d'ESC par ligne :** 0,0167 ms, soit 0,24 % d'un tour à 7,1 ms ; déjà optimisée (IMP-0242).
- **Écriture console :** par valeur, non par caractère. Le CR est doublé au passage à la ligne (2 DWriteGS). La fusion ferait gagner au plus un appel par ligne, non mesuré, imperceptible ; elle demanderait de réviser IMP-0220.
- **Écho de saisie :** 2 DWriteGS par touche, imperceptible à la frappe.
- **Pavé du curseur en 80ᵉ colonne :** le retour arrière remonte en fin de ligne précédente quand `consWrap` est vrai (GS/OS Reference chap. 9, « Backspace »). L'affichage est correct.
- **Blocs du pilote :** `DAccessRecGS` (`pCount` 5, `requestCount` 2 pour GetReadMode, SetReadMode, Get/SetWaitStatus), `DIORecGS` (`pCount` 6), `DevNumRecGS` (`pCount` 2) sont conformes à GS/OS Reference chap. 9.
- **Blocs de fichiers :** `CreateRecGS` (`pCount` 5), `OpenRecGS` (2 et 3), `DirEntryRecGS` (6), `ExpandPathRecGS` (2), `DInfoRecGS` (2), `VolumeRecGS` (6) sont conformes à `gsos.h`. `bufSize` = 259 et 36 incluent bien le mot de taille.
- **`plateforme_secteurs_libres` :** l'appel Volume est protégé par `volume_permis` ; les tampons statiques sont cohérents ; la reprise de la recherche est correcte ; vérifié sur l'Apple IIGS le 2026-10-27.
- **Fichiers GS/OS non refermés ou `refNum` réutilisé :** aucun dans `fichiers_iigs.c`. `lister` et `bibliotheque_ouvrir` ferment toujours ; `existe` ferme.
- **`memoire_iigs.c` :** `NewHandle`, les attributs `$0018` et `toolerror()` sont conformes (IMP-0271, vérifié sur la machine). L'absence de `MMStartUp` est voulue (§11).
- **`horloge_iigs.c` :** les valeurs de `localtime` tiennent dans les champs ; l'année est en `unsigned int`.
- **Interfaces :** types, signes et tailles concordent avec les réalisations et les bouchons (console, fichiers, horloge, mémoire).
- **Pas de récursion C** dans l'interpréteur ni dans le décompilateur (mesure `&FACT(25)`).
- **`plateforme_fichier_lister` en O(n²) :** la recherche par rang absolu ne coûte que pour de nombreux temporaires ; le gros du coût est couvert par G5-P-01.

---

## 5. Pistes hors périmètre

- **`src/noyau/fichiers.c:235-249`, `302`, `349`** : appel de `fichiers_secteurs_temporaires` à chaque GARER temporaire (moitié noyau de G5-P-01).
- **`src/noyau/compilateur.c:1258-1269`** : `corps_afficher`, 902 octets de locales non réentrantes ; candidates à `static` (budget de pile).
- **`src/noyau/decompilateur.c:1028`** : `Decompilation d`, 1 546 octets ; candidate à `static`.
- **`src/noyau/compilateur.c:587-829`** : aucune borne de profondeur. Le chapitre Compilateur §2 et IMP-0164 tiennent la borne de 80 caractères pour suffisante sans en chiffrer la pile.
- **`src/noyau/ruban.c:111-121`** : PERFORER fait, par ligne, une taille (`fseek` END) et deux écritures chacune suivie de `fflush`. Ce sont plusieurs appels GS/OS par ligne perforée (non tracé en détail).
- **`src/noyau/moniteur.c:595`** : le résultat de `plateforme_espace_vider` est ignoré (vidage partiel silencieux).
- **`src/noyau/interprete.c:441-446` et `src/noyau/moniteur.c:75-80`** : CR écrit deux fois par passage à la ligne (CR, puis CR LF).

---

## 6. Points à arbitrer par Éric

1. **Taille de pile.** Option a : `#pragma stacksize 16384`, recommandée ; c'est la pile de Golden Gate, avec 5 089 octets de marge au pire cas mesuré. Option b : 12 Kio et tampons `static` (pire cas d'environ 10 100 octets). Option c : 8 Kio et une borne de profondeur dans le compilateur. Dans tous les cas, à vérifier d'abord sur l'Apple IIGS : la taille réelle de la pile du shell (sonde ou `?((((((((1))))))))`), puis l'acceptation de 16 Kio par le chargeur.
2. **Borne de profondeur dans le compilateur**, avec l'erreur C 107 `LIGNE TROP COMPLEXE` : elle changerait un comportement observable, en refusant des lignes de 80 caractères valides. Elle n'est utile que si la pile reste petite.
3. **Conduite au débordement** : remplacer `SystemError` pour rendre la console et afficher un message, ou reprendre au moniteur par `longjmp`. La reprise est plus ambitieuse : symboles créés pendant la compilation et objets verrouillés seraient à remettre en état.
4. **Suite de non-régression de pile** dans `make tests` (borne par scénario, `stacksize` égale à celle de la cible).
5. **Fin de fichier sur l'entrée du repli stdio** : AU REVOIR implicite, ou fin du programme.
6. **Options `-ID`, `-TEMP` invalides et échec de `-BIB` (F 102)** : refus au lancement, message, ou session sans bibliothèque.
