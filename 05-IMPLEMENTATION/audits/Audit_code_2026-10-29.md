# Audit de code du LSE-IIGS — défauts d'exécution et performances

**Date :** 2026-10-29
**Objet :** rechercher dans le code de `src/` (noyau, plateforme, interfaces ; 13 752 lignes) (A) les défauts d'exécution et (B) les coûts d'exécution et les algorithmes, sans rien corriger. Les corrections sont arbitrées par Éric, point par point, après lecture du rapport.
**Périmètre :** `src/noyau/` (interprete, pile, contexte_exec, donnees, compilateur, analyseur, rcim, symboles, decompilateur, programme, moniteur, erreurs, objets, bibliotheque, fichiers, codage, ruban, taches), `src/plateforme/` (main, console_iigs, fichiers_iigs, horloge_iigs, memoire_iigs), `src/interfaces/`. Le `build/Makefile` n'a été examiné que pour les dépendances et la pile des essais.
**Mode de traitement :** chaque point se traite un par un. Les points AC (défauts) et AP (performances) reçoivent, une fois tranchés, la référence de la correction ou de la décision `IMP-*` qui les referme. Aucun fichier de `src/`, `tests/`, `build/` ni de la spécification n'a été modifié par cet audit.

Les numéros de ligne cités sont ceux des fichiers à la date de l'audit.

---

## 1. Méthode et sources

### 1.1 Déroulement

1. **État de référence.** Avant l'audit, `make tests` a été construit et exécuté sous Golden Gate dans une copie du dépôt, dans les deux profils : 29 suites, 1 455 contrôles, aucun échec en `dev` comme en `cible` (journaux en annexe : `reference_make_tests_dev.log`, `reference_make_tests_cible.log`).
2. **Lecture répartie** en cinq groupes, chacun confié à un sous-agent travaillant sur sa propre copie du dépôt dans le bloc-notes de la session :
   - groupe 1 : interprete, pile, contexte_exec, donnees ;
   - groupe 2 : compilateur, analyseur, rcim, symboles ;
   - groupe 3 : decompilateur, programme, moniteur, erreurs ;
   - groupe 4 : objets, bibliotheque, fichiers, codage, ruban, taches ;
   - groupe 5 : plateforme, interfaces, et le budget de pile du 65816 pour tout le programme.
3. **Essais.** Les essais ont été écrits dans ces copies, jamais dans `tests/`. Ils ont été compilés par ORCA/C 2.2.1 et exécutés sous Golden Gate, en profil `cible` et souvent aussi en `dev`. Aucune image pour l'Apple IIGS n'a été construite.
4. **Consolidation.** Les doublons entre groupes sont fusionnés ; chaque point fusionné cite tous ses essais. Les constats bloquants ont été relus directement dans le dépôt : `bibliotheque.c:695`, `interprete.c:1253`, `interprete.c:889-891`, `contexte_exec.c:98-110`, `:661`, `:819`, `pile.c:47-55`. Le passage du manuel ORCA/C sur le modèle mémoire small (l. 6333 du texte extrait) a aussi été relu.

### 1.2 Statuts

- **CONFIRMÉ** : un essai reproduit le défaut, ou le chemin a été lu entièrement (la preuve est indiquée).
- **PLAUSIBLE** : le raisonnement est incomplet (ce qui manque est indiqué).

Les points seulement spéculatifs ont été écartés (§8).

### 1.3 Limites des instruments

- **Outils non comptés.** Golden Gate exécute SANE, le Memory Manager et GS/OS en code natif. Leurs cycles ne sont pas comptés, et la pile qu'ils emploient sur l'Apple IIGS non plus.
- **Cycles du code C.** Les cycles du code C, comptés par `iix --cycles`, sont convertis en temps à raison d'environ 12 000 cycles par milliseconde. Cet étalonnage (groupe 2, ±10 %) a été fait sur deux opérations chronométrées sur l'Apple IIGS, `donnees_operation_entiere` et `donnees_nombre_entier`.
- **Coûts unitaires.** Les seuls admis sont ceux des mesures sur l'Apple IIGS :
  - `05-IMPLEMENTATION/audits/Mesures_performance_IIGS_2026-10-25.md` ;
  - `05-IMPLEMENTATION/audits/Mesures_memoire_IIGS_2026-10-29.md`.
- **GS/OS de fichier.** Aucun coût d'appel GS/OS de fichier n'a été mesuré. Les points qui en dépendent sont chiffrés en nombre d'appels et marqués PLAUSIBLE.
- **Essai interrompu.** L'essai `audit_g3` du groupe 3 ne s'est pas terminé : il tournait encore après 2 h 24 et a été arrêté. Il devait contrôler, bit à bit, que la table de puissances de dix proposée en AP-03 donne la même écriture que le code actuel (§6, AP-03). Ce contrôle n'est donc pas établi. Tous ses autres résultats, D1 à D7 et le décompte des opérations, avaient déjà été écrits.

### 1.4 Sources consultées

- **Tome V**, chapitres de chaque module et décisions IMP de leurs en-têtes, notamment :
  - Construction §5 et §6 (IMP-0143, règle du `p++`, inclusions relatives) ;
  - Gestionnaire d'objets §8, §11 et §16 (IMP-0194, IMP-0243, IMP-0270, IMP-0271) ;
  - Interpréteur (IMP-0214 à IMP-0226), Données (IMP-0209 à IMP-0213), État d'exécution (§5.5, §7, §12 ; IMP-0145, IMP-0179) ;
  - RCI-M (IMP-0183), Compilateur (§2, §9, §18 ; IMP-0164, IMP-0165) ;
  - Décompilateur (IMP-0198 à IMP-0203), Programme courant, Moniteur et console (IMP-0227 à IMP-0242, IMP-0267) ;
  - Bibliothécaire et fichiers (IMP-0245 à IMP-0265), Tâches et reprise (IMP-0269), Gestionnaire des erreurs §7, §9, §9 ter.
- **ORCA/C** : manuel ORCA/C 2.2 (texte extrait de `04-ORCA-C/manuel/ORCA_C_2.2_Manual.docx`) : `stacksize` (l. 830 et 6438-6442), `memorymodel` (l. 6328-6334), `debug`, `SystemError` (chap. 20) ; `04-ORCA-C/notes/cc.notes.txt`.
- **Apple** : *Apple IIGS Toolbox Reference*, vol. 1, chap. 12, et vol. 3, chap. 36 ; *GS/OS Reference*, extraits versés dans `03-APPLE-IIGS/`.

### 1.5 Annexe

`05-IMPLEMENTATION/audits/Audit_code_2026-10-29_essais/` conserve :
- les essais et leurs sorties ;
- les rapports intermédiaires des cinq groupes (`gN/rapport-gN.md`, où « S » désigne le bloc-notes de la session) ;
- pour chaque copie de travail, le correctif de ses écarts avec le dépôt (`gN/ecarts_copie_gN.patch` : règles d'essai, instrumentation, variantes `AUDIT_OPT` du groupe 2) ;
- les journaux de référence de `make tests`.

---

## 2. Carte des chemins chauds

Coûts unitaires mesurés sur l'Apple IIGS, en profil `cible` :

| Opération | Coût |
|---|---|
| `objets_resoudre` + `objets_relacher` | 1,1 ms |
| `objets_copier` d'une ligne | 0,133 ms |
| `programme_chercher` | 0,067 ms |
| `programme_suivante` | 0,100 ms |
| empilement et consommation d'un nombre | 0,400 ms (0,133 ms en version exacte) |
| `contexte_affecter_nombre` | 0,333 ms |
| opération SANE | 0,45 à 0,9 ms |
| lecture de `$C000` | 0,0167 ms |
| lecture sans attente du pilote `.CONSOLE` | 0,517 ms |
| tour de la boucle (b) `FAIRE 20 POUR I_1 JUSQUA 200` / `20 X_1` | 7,1 ms |

### 2.1 Par ligne exécutée (interpréteur)

| Étape | Chemin | Fréquence | Coût |
|---|---|---|---|
| Fin de séquence | `cycle` (`interprete.c:1287`) → `contexte_fin_de_ligne` (`contexte_exec.c:507`) → `programme_suivante` (`programme.c:246`) | une par ligne achevée | 0,100 ms |
| Arrivée sur une ligne | `arrivee` (`interprete.c:1270`) → `charger_ligne` (`:1212`) → `programme_chercher` (`programme.c:231`, dichotomie) + `objets_copier` | une par changement de ligne | 0,067 + 0,133 ms |
| Consultation d'ESC | `point_d_arret` (`interprete.c:1248`) → `console_interruption` (`console_iigs.c:241-253`, lecture de `$C000`, IMP-0242) | une par arrivée **en début de ligne seulement** (voir AC-03) | 0,017 ms |
| Branchement | `contexte_brancher` (`contexte_exec.c:513`) → `programme_chercher` | par ALLER EN, retour, appel | 0,067 ms, puis une seconde recherche dans `charger_ligne` (AP-10) |
| Reprise d'itération FAIRE | `fin_de_ligne` (`contexte_exec.c:494-498`) | une par tour | négligeable |

La boucle (b), à 7,1 ms par tour, se décompose ainsi (groupe 1, §1) :
- deux chargements de ligne : 0,4 ms ;
- une ligne suivante : 0,1 ms ;
- la référence X, la valeur 1 et l'affectation : ≈ 0,9 ms ;
- le pas : ≈ 1,0 ms ;
- la valeur 200 : 0,3 ms ;
- le test : ≈ 0,4 ms ;
- la distribution de sept éléments.

### 2.2 Par élément et par opération

| Élément | Chemin | Coût |
|---|---|---|
| Constante entière | `pousser_entier` (`interprete.c:64`) → `donnees_nombre_entier` → `pile_empiler_nombre_exact` | 0,167 + 0,133 ms |
| + − × entiers exacts | `binaire` (`interprete.c:151`) → `donnees_operation_entiere` (IMP-0244) | 0,567 ms |
| Autres opérations et fonctions | `binaire`/`fonction_1` : `float`→`double`, SANE, `pousser_nombre(double)` | plusieurs ms (AP-04) |
| Affectation | `affecter` (`interprete.c:287`) → `ecrire_reference(float)` → `contexte_affecter_nombre(float)` | 0,333 ms + un passage de `float` par valeur (AP-07) |
| Lecture ou écriture de `T[I]` | `donnees_rang` (`donnees.c:591`) + `donnees_lire_element` (`:611`) : deux paires de verrouillage, `arrondir` en `double` | ≈ 4,5 à 6,5 ms (AP-02) |
| Chaîne | `donnees_objet_vers_chaine` (`donnees.c:140`), concaténation, extraction : `objets_creer` + une paire de verrouillage par opérande | 5 à 7 ms pour `C_C!'X'` (AP-01, AP-08) |
| Appel de procédure | `contexte_appeler` (`contexte_exec.c:684`) : verrouillage de la ligne d'entrée, six allocations | ≥ 1,1 ms (AP-09) |
| AFFICHER d'un nombre en U, CCA | `donnees_ecrire_nombre` → `decompilateur_ecrire_reel_chiffres` (`decompilateur.c:387`) | 59 à 1 054 opérations `double` par nombre (AP-03) |

### 2.3 Par commande

| Commande | Chemin et coût dominant |
|---|---|
| Ligne tapée (programme ou machine de bureau) | `ligne_programme`/`ligne_bureau` (`moniteur.c:1509`, `:1535`) → `compilateur_compiler` (`compilateur.c:1804`) : ≈ 22,8 ms de CPU par ligne du corpus, dont ≈ 80 % pour reconnaître des mots (AP-05) ; ≈ 24 appels SANE par constante entière (AP-11) ; puis `programme_inserer` → `decoder_declaration` (1,1 ms, AP-13). Total estimé : 32 à 35 ms, imperceptible pour une ligne tapée. |
| ENCODER, RUBAN | une compilation par ligne : ≈ 8 à 9 s pour 255 lignes (AP-05, AP-11) |
| LISTER, PERFORER, DECODER | `lister` (`moniteur.c:401`) ou `codage_decoder` (`codage.c:72`) → `decompilateur_decompiler` (`decompilateur.c:1025`) : 1,1 ms de verrouillage par ligne, trois `DWrite` par ligne listée, et 30 à 950 ms par constante réelle (AP-03) |
| APPELER, EXECUTER | `bibliotheque_appeler` → `verifier` (`bibliotheque.c:652`) → `reconstruire` (`:707`) : recherche quadratique des symboles et des constantes (AP-12), 10 couples `HLock`/`HUnlock` par ligne (AP-01), soit ≈ 0,77 s de verrouillage pour 200 lignes |
| GARER, CHARGER | `fichiers_garer` (`fichiers.c:335`) : 23 appels GS/OS, plus 13k + 3 pour le quota avec k fichiers temporaires (AP-15), secteur par secteur (AP-17) |
| TABLE DES FICHIERS, UTILISATION DISQUE, DUPLIQUER `*` | `espace_suivant` (`moniteur.c:903`) : relecture du répertoire pour chaque nom, 3N² appels GS/OS (AP-06) |

### 2.4 Gestionnaire d'objets : appels et fréquence

Les compteurs du groupe 4 ont été posés dans `memoire_iigs.c` de la copie.

| Opération | Appels toolbox | Coût |
|---|---|---|
| `objets_resoudre` + `objets_relacher` | 3 `HLock` + 3 `HUnlock` (l'existence est contrôlée deux fois) | 1,13 ms mesurée, soit ≈ 0,19 ms par appel |
| `objets_creer` avec k blocs pleins devant | 2(k+1) appels | 0,37 ms × (k+1) |
| `objets_liberer` | 4 appels | ≈ 0,75 ms |
| `objets_copier`, `objets_longueur` | aucun (IMP-0243) | 0,133 ms par ligne |

Appels les plus fréquents :
- `objets_copier` à chaque changement de ligne (`interprete.c:1216`) ;
- deux résolutions par accès à `T[I]` (`donnees.c:613`, `:629`, `:644`) ;
- `objets_creer` + `objets_liberer` pour chaque chaîne transitoire (`donnees.c:82`, `:171` ; `pile.c:32`) ;
- une résolution par ligne compilée (`compilateur.c:1867-1872`), par ligne insérée (`programme.c:82`), par ligne décompilée (`decompilateur.c:1043`) et par appel de procédure (`contexte_exec.c:716`).

---

## 3. Budget de la pile du 65816

**Faits de plateforme.**
- Le programme prend la pile du lanceur, 4 Kio sous le shell ORCA. Cette pile sert aussi à la page directe de SANE (manuel ORCA/C, l. 830 et 6442).
- Le dépôt ne contient aucun `#pragma stacksize` (vérifié sur `src/`, `tests/`, `build/` et `05-IMPLEMENTATION/`).
- Golden Gate fournit environ 16 Kio et honore `#pragma stacksize`. Avec 4 096, il reste 3 825 octets utilisables sous `main`.
- Le contrôle de `#pragma debug 1` se déclenche à l'entrée de chaque fonction C. Il exige les locales plus 200 octets de marge, puis appelle `SystemError(15)` : « Stack overflow », code de sortie 255, sans `atexit` (essais `essai_debordement`, `essai_atexit`).
- Ce contrôle ne couvre ni les fonctions de bibliothèque, ni SANE, ni GS/OS (manuel, `debug` : « It is still possible to overflow the stack … by calling a tool »).

**Locales.** Les plus grosses (relevé complet : `g5/rapport-g5.md` §2.2) :

| Fonction | Emplacement | Taille | Contenu principal |
|---|---|---|---|
| `decompilateur_decompiler` | `decompilateur.c:1028` | 1 564 o | `Decompilation d` |
| `corps_afficher` | `compilateur.c:1258-1269` | 902 o | `specs[40]`, `brouillon[255]` |
| `plateforme_fichier_lister` | `fichiers_iigs.c:171-175` | 587 o | — |
| `plateforme_fichier_renommer` | — | 400 o | — |
| `donnees_ecrire_nombre` | — | 366 o | — |
| `chiffres_entier` | — | 332 o | — |
| `element` (décompilateur) | — | 328 o | — |
| `ecrire_nombre` | — | 302 o | — |
| `compilateur_compiler` | — | 298 o | — |

Une vingtaine d'autres fonctions portent un tampon de 256 octets.

**Récursions.**
- La seule récursion profonde est la descente du compilateur (`compilateur.c:369-918`, 15 fonctions). Elle coûte, mesuré :
  - 238 octets par niveau de parenthèses ;
  - 284 octets par fonction imbriquée ;
  - 302 octets par indice imbriqué.
- Sa seule borne est la ligne de 79 caractères, soit 38 niveaux au plus.
- L'interpréteur n'est pas récursif en C : les cadres L.S.E. sont alloués dans le tas, et `&FACT(25)` atteint 1 957 octets, autant qu'une session minimale.
- Le décompilateur n'est pas récursif non plus : il emploie une pile de travail explicite.

**Profondeurs mesurées sous `main`** (profil `cible`, mesures concordantes des groupes 2, 3 et 5) :

| Scénario | Octets | Avec une pile de 4 Kio (3 825 o) |
|---|---|---|
| `BO`, `?1+1`, `AU` | 1 977 | marge 1 848 |
| `?` et k parenthèses | 1 977 + 238 k | **débordement à k = 8** sous Golden Gate ; **à k = 7 sur l'Apple IIGS** |
| `10 A_` et k parenthèses | 1 074 + 238 k | **débordement à k = 12** |
| `?ABS(` × 15 | 6 237 | **débordement** |
| Ligne de 22 parenthèses, puis LISTER et EXECUTER | 6 326 | **débordement** |
| ENCODER d'une ligne de 16 parenthèses | 5 204 | **débordement** |
| LISTER (réels, format) | 2 785 | marge 1 040 |
| GARER, CHARGER, CATALOGUER | 2 239 | marge 1 586 |
| Pire cas d'une ligne valide (`?` et 38 parenthèses) | 11 021 | 2,9 fois le budget |

**Mesure sur l'Apple IIGS** (Éric, 2026-10-29 ; image du profil `cible`, LSE lancé depuis le shell ORCA). `?((((((1))))))` (6 niveaux) passe. `?(((((((1)))))))`, 7 niveaux et 16 caractères, provoque le débordement. Sous Golden Gate, avec 4 096 octets imposés, il ne survient qu'à 8 niveaux.
- Pour un même exécutable, les cadres C sont identiques sur les deux machines. La pile disponible sous `main` est donc plus petite sur l'Apple IIGS que sous Golden Gate avec `stacksize 4096` (3 825 octets).
- 6 niveaux demandent 3 405 octets et 7 niveaux 3 643. La pile disponible sous `main` est donc comprise entre environ 3 405 et 3 643 octets sur la machine, soit un écart de 182 à 420 octets avec Golden Gate.
- Golden Gate ne reproduit donc pas fidèlement la pile de la cible, même avec `stacksize` imposée. L'essai de confirmation ci-dessous écarte la pile fournie par le shell comme cause de l'écart.

**Conclusion.**
- Les usages courants tiennent encore dans la pile de la cible. Leur marge mesurée sous Golden Gate, de 1 040 à 1 848 octets, diminue de 182 à 420 octets : il reste environ 0,6 à 1,6 Kio.
- **Essai de confirmation** (2026-10-29) : image `build/PILE.po` (volume `/PILE/`), construite en profil `cible` à partir d'une copie du dépôt et essayée par Éric le même jour. Elle porte :
  - `LSE16`, avec `#pragma stacksize 16384` ;
  - `LSE4`, identique au `LSE` du dépôt ;
  - trois sondes de la pile disponible sous `main` : `PILED` (pile du lanceur), `PILE4K` (4 096 octets) et `PILE16` (16 384 octets).

  Sources, modification de `main.c` et sorties sous Golden Gate : annexe `pile_iigs/`.

  Résultats, en octets sous la locale de `main` de la sonde :

  | Sonde | Golden Gate | Apple IIGS |
  |---|---|---|
  | `PILED` | 16 151 | 3 596 |
  | `PILE4K` | 3 866 | 3 596 |
  | `PILE16` | 16 151 | 15 881 |

  - Le shell ORCA fournit bien 4 Kio : `PILED` et `PILE4K` sont égaux sur la machine.
  - À taille de pile égale, la machine laisse 270 octets de moins que Golden Gate, à 4 Kio comme à 16 Kio. La cause de cet écart constant n'est pas établie.
  - Le cadre de `main` de LSE est plus grand de 41 octets que celui de la sonde (3 825 contre 3 866 sous Golden Gate avec 4 Kio). Sur la machine, LSE dispose donc de 3 555 octets sous `main` avec la pile du shell. Cela concorde avec le seuil observé : 6 niveaux demandent 3 405 octets, 7 niveaux 3 643.
  - Avec 16 Kio, LSE dispose de 15 840 octets, soit 4 819 octets de marge au pire cas d'une ligne valide (11 021).
  - Sur la machine, le chargeur lance `LSE16` sous le shell ORCA, et `LSE16` accepte `?` suivi de 7, 20 et 38 niveaux de parenthèses (affichage `1`).
- Une ligne valide de 16 caractères suffit à arrêter LSE (AC-01).
- Les appels SANE et GS/OS, non comptés sous Golden Gate, s'ajoutent au bas de la pile : la marge réelle au point le plus profond reste inconnue.
- Le corpus historique ne dépasse pas 3 niveaux de parenthèses : 502 lignes à 0 niveau, 386 à 1, 23 à 2, 1 à 3 (outil `g2/profondeur.py`).

---

## 4. Tableau de synthèse

| Id | Gravité | Module | Résumé | Statut |
|---|---|---|---|---|
| AC-01 | bloquant | plateforme/main, compilateur | Pile épuisée par 7 parenthèses (`?`) sur l'Apple IIGS ; sous Golden Gate avec 4 Kio imposés, par 8 (`?`) ou 12 (`10 A_`) : « Stack overflow », session perdue | CONFIRMÉ (sur l'Apple IIGS) ; corrigé par IMP-0272 |
| AC-02 | bloquant | moniteur | Une ligne de machine de bureau en erreur, ou interrompue par ESC pendant LIRE, laisse ses éléments sur la pile : CONTINUER range une valeur dans une autre variable | CONFIRMÉ ; corrigé |
| AC-03 | bloquant | interprete | ESC n'est jamais vue dans une boucle FAIRE tenue sur une seule ligne : boucle infinie impossible à interrompre | CONFIRMÉ ; corrigé |
| AC-04 | bloquant | contexte_exec | `RESULTAT T[I]` et un élément passé à un paramètre LOCAL donnent EX 96 | CONFIRMÉ ; corrigé |
| AC-05 | bloquant | interprete | GARER d'un tableau de plus de 65 535 octets d'image écrit des octets non initialisés | CONFIRMÉ ; corrigé |
| AC-06 | bloquant | pile | Pile d'évaluation au-delà de 9 362 éléments adressée modulo 64 Kio : corruption silencieuse | CONFIRMÉ ; corrigé |
| AC-07 | bloquant | contexte_exec | Réserve des cases au-delà de 8 192 cases : corruption de l'environnement | CONFIRMÉ ; corrigé |
| AC-08 | bloquant | bibliotheque | Un programme contenant une ligne vide est rangé, puis refusé par APPELER et EXECUTER (F 101) | CONFIRMÉ ; corrigé |
| AC-09 | bloquant | interprete | La description d'AFFICHER est perdue à travers un appel de fonction ou une suspension (pas à pas) | CONFIRMÉ ; corrigé |
| AC-10 | majeur | codage, compilateur | DECODER restitue des lignes de plus de 79 caractères qu'ENCODER refuse (C 63), sans le signalement exigé par IMP-0183 | CONFIRMÉ ; non corrigé (choix d'Éric) |
| AC-11 | majeur | build, tests | Golden Gate fournit 16 Kio de pile : aucune suite ne peut révéler un débordement de la cible | CONFIRMÉ ; corrigé |
| AC-12 | majeur | plateforme/main, console | Arrêt sur débordement sans rendre la console ni la mémoire ; le contrôle ne couvre ni SANE ni GS/OS | CONFIRMÉ ; corrigé |
| AC-13 | majeur | moniteur, interprete | Un LIRE de machine de bureau pendant une lecture suspendue perd les valeurs déjà lues par le programme | CONFIRMÉ ; corrigé |
| AC-14 | mineur | moniteur | Le reste d'une ligne tapée pour un programme terminé alimente le LIRE suivant de la machine de bureau | CONFIRMÉ ; corrigé |
| AC-15 | mineur | moniteur | Échec de `contexte_initialiser` après l'abandon des cadres : l'état reste « suspendu », CONTINUER sans cadre | PLAUSIBLE ; corrigé |
| AC-16 | mineur | bibliotheque | RANGER boucle sans fin si l'image RCI-S dépasse 65 535 octets | CONFIRMÉ ; corrigé |
| AC-17 | mineur | bibliotheque, contexte_exec | La vérification RCI-S ne contrôle ni identifiants, ni index, ni unicité (IMP-0246) | CONFIRMÉ ; corrigé |
| AC-18 | mineur | fichiers | GARER : une étendue est un moment à la fois vivante et libre | CONFIRMÉ ; corrigé |
| AC-19 | mineur | fichiers | Une étendue libre est prise entière : place perdue, EX 52 prématuré | CONFIRMÉ ; corrigé |
| AC-20 | mineur | fichiers | Numéros de secteur sur 16 bits sans borne : un fichier permanent de plus de 65 535 secteurs écrase son répertoire | CONFIRMÉ ; corrigé |
| AC-21 | mineur | bibliotheque, fichiers, moniteur | Chemins d'échec qui laissent la bibliothèque incohérente ou échouent sans message | CONFIRMÉ ; corrigé |
| AC-22 | mineur | donnees | Écriture F ou E d'un grand nombre : caractères autres que des chiffres | CONFIRMÉ ; corrigé |
| AC-23 | mineur | interprete | CHARGER d'un tableau : en-tête lu non validé | CONFIRMÉ ; corrigé |
| AC-24 | mineur | fichiers_iigs | `composer()` écrit sans borne dans `c[200]` | CONFIRMÉ ; corrigé |
| AC-25 | mineur | fichiers_iigs | L'espace du ruban est traité comme une unité amovible (défaut latent) | CONFIRMÉ ; corrigé |
| AC-26 | mineur | console_iigs | Repli stdio : une fin de fichier devient ESC, la session boucle sans fin | CONFIRMÉ ; corrigé |
| AC-27 | mineur | main | Options `-ID` et `-TEMP` non contrôlées ; option inconnue ignorée | CONFIRMÉ ; corrigé |
| AC-28 | mineur | main, fichiers_iigs | F 102 codé en dur, sans libellé ; repli silencieux sur `@:LSE` | CONFIRMÉ ; corrigé |
| AC-29 | mineur | fichiers_iigs | Diagnostics de plateforme confondus (support plein, inexistant) | CONFIRMÉ ; corrigé |
| AC-30 | mineur | build | Trois objets de plateforme ne dépendent pas de `profil.h` | CONFIRMÉ ; corrigé |
| AC-31 | mineur | spécification | Écarts rédactionnels entre la spécification et le code (contrat du compilateur, IMP-0194, IMP-0243) | CONFIRMÉ ; corrigé |
| AC-32 | mineur | symboles | Tables des symboles et des constantes : plus de 64 Kio au-delà de 6 553 et de 21 845 entrées (modèle small) | CONFIRMÉ ; corrigé |
| AC-33 | mineur | decompilateur, objets | Restitution d'une ligne : la ligne est déverrouillée par le relâchement d'une constante du même bloc, puis lue à travers des appels SANE | CONFIRMÉ ; corrigé |
| AC-34 | mineur | build | 26 règles du Makefile ne dépendent pas d'`objets.h`, inclus par `rcim.h`, `symboles.h` ou `pile.h` : construction incrémentale incohérente | CONFIRMÉ ; corrigé |
| AC-35 | mineur | contexte_exec | Pas de FAIRE hors du chemin exact : un dépassement de `FLT_MAX` rend la variable infinie au lieu d'EX 44 | CONFIRMÉ ; corrigé |
| AC-36 | mineur | fichiers | GARER dans une étendue reprise entière : secteurs en trop remplis d'octets lus au-delà du contenu | CONFIRMÉ ; corrigé |
| AP-01 | majeur | objets | Verrouillages superflus dans `objets_existe`, `objets_creer`, `objets_liberer` | CONFIRMÉ ; corrigé |
| AP-02 | majeur | donnees | Accès à `T[I]` : deux paires de verrouillage et un arrondi en `double`, ≈ 5 ms par accès | PLAUSIBLE ; corrigé |
| AP-03 | majeur | decompilateur, donnees | Écriture d'un réel : puissances de dix recalculées, 59 à 1 054 opérations `double` par nombre | CONFIRMÉ ; corrigé |
| AP-04 | majeur | interprete | Arithmétique non exacte : détour par `double`, conversions et comparaisons redondantes | PLAUSIBLE ; corrigé |
| AP-05 | majeur | compilateur, symboles | Reconnaissance des mots : ≈ 80 % du temps de compilation, −53 % possibles | CONFIRMÉ ; corrigé |
| AP-06 | majeur | moniteur, fichiers_iigs | TABLE DES FICHIERS, UTILISATION DISQUE, DUPLIQUER `*` : appels GS/OS quadratiques | PLAUSIBLE ; corrigé |
| AP-07 | mineur | interprete, contexte_exec | Affectation et pas de FAIRE : `float` passés par valeur | CONFIRMÉ ; corrigé |
| AP-08 | mineur | donnees | Chaînes : une paire de verrouillage pour lire une longueur et par opérande | CONFIRMÉ ; corrigé |
| AP-09 | mineur | contexte_exec | Appel de procédure : verrouillage de la ligne d'entrée et six allocations | CONFIRMÉ ; corrigé |
| AP-10 | mineur | interprete | Constante immédiate convertie à chaque exécution ; ligne recherchée deux fois | CONFIRMÉ ; constantes immédiates corrigées, second volet écarté |
| AP-11 | mineur | compilateur | Constantes numériques : ≈ 24 appels SANE par constante entière | CONFIRMÉ ; corrigé |
| AP-12 | mineur | symboles, bibliotheque | APPELER : reconstruction quadratique des symboles et constantes | CONFIRMÉ ; corrigé |
| AP-13 | mineur | programme | Chaque ligne insérée est verrouillée pour lire son premier octet | CONFIRMÉ ; corrigé |
| AP-14 | mineur | bibliotheque | `CATALOGUE` lu entrée par entrée, en deux passes à l'inscription | PLAUSIBLE ; corrigé |
| AP-15 | mineur | fichiers, fichiers_iigs | Le quota temporaire relit tout l'espace temporaire à chaque GARER | PLAUSIBLE ; corrigé |
| AP-16 | mineur | codage | DECODER rouvre et reparcourt le fichier pour chaque enregistrement | PLAUSIBLE ; clos sans changement |
| AP-17 | mineur | fichiers, fichiers_iigs | GARER secteur par secteur avec `fflush` ; 3 appels GS/OS par secteur par stdio | PLAUSIBLE ; corrigé (écriture par blocs) |

**Décompte :** 53 points, soit 36 défauts et 17 points de performance. AC-32 à AC-36 ont été relevés pendant les corrections, le 2026-10-30.

| Gravité | Défauts | Performances | Total |
|---|---|---|---|
| Bloquant | 9 confirmés | — | 9 |
| Majeur | 4 confirmés | 3 confirmés, 3 plausibles | 10 |
| Mineur | 22 confirmés, 1 plausible | 6 confirmés, 5 plausibles | 34 |

---

## 5. Défauts d'exécution

### 5.1 Bloquants

#### AC-01 — La pile du programme est épuisée par une ligne moyennement imbriquée : « Stack overflow », session perdue
- **Statut :** CONFIRMÉ sur l'Apple IIGS par Éric, le 2026-10-29 : `?(((((((1)))))))` (7 niveaux) provoque le débordement. Trois essais indépendants sous Golden Gate, avec une pile de 4 096 octets imposée par `#pragma stacksize 4096`, avaient établi le coût de 238 octets par niveau et un débordement à 8 niveaux :
  - Groupe 2 : programme `LSE` complet, entrée `BO`, `?((((((((1))))))))`, `AU`.
  - Groupe 3 : sessions du moniteur.
  - Groupe 5 : sessions avec mesure de la profondeur.

  Le seuil est plus bas d'un niveau sur la machine : la pile y est plus petite que sous Golden Gate avec 4 Kio imposés (§3). Mesures sur la machine (§3) : LSE dispose de 3 555 octets sous `main` avec la pile du shell. Avec `#pragma stacksize 16384`, il en dispose de 15 840, et les lignes de 7, 20 et 38 niveaux passent.
- **Emplacement :**
  - `src/plateforme/main.c` : aucun `#pragma stacksize` ;
  - descente récursive `src/noyau/compilateur.c:588-830` (`atome` … `expression`), `facteur_logique` `:874`, `indices` `:368`, `appel_fonction` `:472` ;
  - grands cadres : `corps_afficher` `compilateur.c:1256-1269` (≈ 900 o), `compilateur_compiler` `:1808` (≈ 300 o), `decompilateur.c:1028` (1 546 o).
- **Constat :**
  - La profondeur croît de 238 à 302 octets par niveau d'imbrication, sur une base de 1 074 octets (ligne de programme) à 1 977 octets (machine de bureau, qui passe par l'affichage).
  - Compilateur §2 affirme que la longueur de ligne « borne mécaniquement » la récursion sans limite codée. La borne existe, mais elle vaut 11 Kio.
  - Compilateur §18 (« Limites explicites ») laissait cette mesure « à effectuer » : la voici, et elle infirme l'hypothèse de §2.
- **Scénario :** sur l'Apple IIGS, `?(((((((1)))))))` (7 niveaux, 16 caractères) donne « Stack overflow », le retour au shell et la perte du programme courant.

  Sous Golden Gate avec 4 Kio imposés (les seuils sont plus bas d'au moins un niveau sur la machine) :

  | Ligne | Résultat |
  |---|---|
  | `?(((((((1)))))))` (7 niveaux) | affiche 1 |
  | `?((((((((1))))))))` (8 niveaux) | « Stack overflow » |
  | `10 X_` et 12 niveaux | idem |
  | `?ABS(ABS(…))` imbriqués 7 fois ou plus | idem |
  | ENCODER d'un fichier contenant une ligne de 16 parenthèses | idem |

  Le corpus historique ne dépasse pas 3 niveaux : aucune ligne attestée n'est touchée, mais une ligne valide de 16 caractères suffit.
- **Référence :**
  - manuel ORCA/C, `stacksize` et `debug` bit 0 ;
  - Compilateur §2, §18 ; IMP-0143 (alternative 3 : risque connu, non chiffré) ; IMP-0164 ; IMP-0165 ; Architecture physique IMP-0134.
- **Proposition :** trois volets cumulables.
  - **(a)** `#pragma stacksize 16384` dans `src/plateforme/main.c`, seul `.root`.
    - Pire cas : 11 021 octets. La marge mesurée sur la machine est de 4 819 octets.
    - C'est la pile de Golden Gate, à 270 octets près (AC-11).
    - Le manuel admet couramment 32 Kio en environnement texte.
    - Vérifié sur l'Apple IIGS le 2026-10-29 : le chargeur accepte 16 Kio sous le shell ORCA, et le pire cas passe.
  - **(b)** Tampons `static` dans `corps_afficher` (−902 octets) et `Decompilation d` (−1 546 octets).
    - Le décompilateur n'est pas réentrant.
    - Pour `corps_afficher`, IMP-0165 a écarté le tampon statique au nom d'une réentrance future (Compilateur §6) : à arbitrer.
  - **(c)** Borne d'imbrication explicite, avec C 107 « ligne trop complexe ». Elle change l'acceptation de lignes valides et ne se justifie que si la pile ne peut pas être agrandie.

  **Recommandation :** (a). Les deux vérifications sur l'Apple IIGS sont faites (§3) ; (b) et (c) deviennent inutiles.
- **Suite donnée :** option (a) retenue par Éric le 2026-10-29, sous la forme d'IMP-0272 (Construction §5 bis) : `#pragma stacksize 16384` dans `src/plateforme/main.c`. Compilateur §2 et IMP-0164 reçoivent le chiffrage de la récursion. La limite « profondeur non mesurée » de Compilateur §18 est close.
- **Décision IMP nécessaire :** oui. (a) : nouvelle décision (Construction §5, taille de pile), avec mise à jour de Compilateur §2 et §18 et d'IMP-0164 (impact mémoire). (b) : révision d'IMP-0165 pour `corps_afficher`. (c) : nouvelle décision.
- **Essais :**
  - `g2/sonde_pile_4k.c`, `g2/sonde_pile_compilateur.c`, `g2/essai-g2-pile.txt` ;
  - `g3/audit_g3_pile.c`, `g3/pile/pile_7.out`, `g3/pile/pile_14.out` ;
  - `g5/essai_pile4k.c`, `g5/essai_pile.c`, `g5/essai_pile16k.c`, `g5/mesures_pile.txt` ;
  - `LSE` construit avec `PILE_4096` : `g2/ecarts_copie_g2.patch`.

#### AC-02 — Une ligne de machine de bureau en erreur, ou interrompue par ESC pendant LIRE, laisse ses éléments sur la pile : CONTINUER range une valeur dans une autre variable
- **Statut :** CONFIRMÉ par deux essais indépendants, et par la lecture de `ligne_bureau`, `interprete_bureau`, `cycle` et `lire` :
  - groupe 1, bloc B de `g1/composants_audit_g1.c` ;
  - groupe 3, D1 à D4 de `g3/audit_g3.c`.
- **Emplacement :** `src/noyau/moniteur.c:1535-1564` (`ligne_bureau`) ; `src/noyau/interprete.c:1331-1349` (`interprete_bureau`) ; `moniteur.c:358-366` (`continuer`).
- **Constat :**
  - `ligne_bureau` exécute la ligne sur la pile d'évaluation du programme suspendu, sans en noter ni en rétablir la hauteur.
  - Après `FIN_ERREUR`, ou après ESC pendant un LIRE de machine de bureau, des éléments restent au-dessus de l'état du programme.
  - L'invariant « entre deux instructions, la pile d'un cadre revient toujours à sa base » (État d'exécution §7.3) est rompu. À la reprise, une opération consomme un de ces restes à la place de son opérande.
  - Le bloc transitoire de la ligne est en outre libéré (`moniteur.c:1563`) alors qu'une chaîne en ligne restée sur la pile peut le désigner.
- **Scénario :**
  - (1) `10 X_&F(2)`, `20 TERMINER`, `30 PROCEDURE &F(A) LOCAL A`, `40 PAUSE`, `50 RESULTAT A*10`. On exécute, puis on tape `?Z` (Z non défini : EX 62), puis CONTINUER. Résultat : **Z = 20, X non défini**.
  - (2) `10 LIRE A,B`, `20 AFFICHER A,B`. On répond `1`, puis ESC, `?A+C` (EX 05), CO et `2`. Résultat : `1` puis `ERREUR (EX.) 62 EN LIGNE 020`. Avec `?A` à la place de `?A+C`, on obtient bien `1 2`.
  - Une faute de frappe en machine de bureau pendant une mise au point suffit donc.
- **Référence :** État d'exécution §7.3, §7.4 ; Moniteur IMP-0233 (« elle ne change pas l'état du programme »), IMP-0237 ; Interpréteur IMP-0214 ; EXI-0060 (reprise).
- **Proposition :** dans `ligne_bureau`, mémoriser `h = pile.sommet` avant le premier `interprete_bureau`. Après la boucle, quelle que soit la fin (`FIN_BUREAU`, `FIN_ERREUR`, ESC), appeler `pile_ramener(&pile, h)` avant `objets_liberer` du bloc. `pile_ramener` libère déjà les chaînes transitoires (`pile.c:108-111`). Coût nul.
- **Décision IMP nécessaire :** non (rétablit un invariant écrit) ; une phrase peut préciser IMP-0233.
- **Suite donnée :** corrigé le 2026-10-29, sans décision IMP : pile ramenée par `ligne_bureau` (`moniteur.c`) à sa hauteur d'entrée après toute fin, avant la libération du bloc transitoire ; quatre sessions de contrôle dans `composants_moniteur` (cas D1 à D4 du groupe 3).

#### AC-03 — ESC n'est jamais vue dans une boucle FAIRE tenue sur une seule ligne
- **Statut :** CONFIRMÉ par essai (bloc A de `g1/composants_audit_g1.c`, deux profils) et par lecture de `interprete.c:1248-1275`.
- **Emplacement :** `src/noyau/interprete.c:1253` (`if (pos->decalage != 0) return FIN_AUCUNE;`), `:1270-1274` ; `src/noyau/contexte_exec.c:494-498`.
- **Constat :**
  - `point_d_arret` ne consulte la console qu'à l'arrivée en décalage 0.
  - La reprise d'itération ramène la position à `(ligne_faire, decalage_reprise)`, décalage non nul.
  - Quand la ligne de fin est la ligne FAIRE elle-même (forme attestée, LSE15 §5-5.5 : `100 FAIRE 100 POUR I←1 JUSQUA N; …`), aucun début de ligne n'est plus atteint pendant la boucle.
  - Le PAS A PAS est touché de la même façon : une boucle d'une ligne s'exécute en un seul pas.
- **Scénario :**
  - `10 FAIRE 10 TANT QUE 1=1` ne s'arrête jamais : il faut redémarrer l'Apple IIGS, et le programme non rangé est perdu.
  - `10 FAIRE 10 POUR I_1 JUSQUA 300;X_I`, ESC frappée tôt : la boucle va à son terme (X = 300) avant l'arrêt en ligne 20.
- **Référence :** Moniteur IMP-0239 (« l'interpréteur consulte la console au début de chaque ligne ») ; État d'exécution §7.2.
- **Proposition :**
  - Consulter aussi `console_interruption` à chaque reprise d'itération qui ramène dans la ligne FAIRE, et rendre `FIN_INTERRUPTION` à la position de reprise. Le contexte de boucle est intact : CONTINUER reprend sans perte.
  - `ligne_signalee` doit retenir la position entière (ligne et décalage).
  - Coût : 0,017 ms par tour (lecture de `$C000`).
- **Décision IMP nécessaire :** oui, révision d'IMP-0239 (point de consultation), et peut-être d'IMP-0233 (message `PRET EN LIGNE` en milieu de ligne ; pas à pas) : §9, point 3.
- **Suite donnée :** corrigé le 2026-10-30 ; décision d'Éric du 2026-10-30, après essais sur LSE 8086 (pas à pas : arrêt sur la ligne `FAIRE` à chaque itération, boucles d'une et de deux lignes ; boucle infinie d'une ligne interrompue par ESC). Révision d'IMP-0232 et d'IMP-0239 : le retour sur la ligne `FAIRE` au début de chaque itération compte comme une arrivée sur cette ligne (pas à pas, ESC, ligne d'arrêt). Dans `interprete.c`, `point_d_arret` reçoit l'indication d'arrivée, et la reprise d'une position signalée est consommée quel que soit le décalage. Deux sessions de contrôle dans `composants_moniteur` (pas à pas, ESC à la troisième consultation).

#### AC-04 — `RESULTAT T[I]` et un élément de tableau passé à un paramètre LOCAL donnent EX 96 « problème interne »
- **Statut :** CONFIRMÉ par essai (bloc C de `g1/composants_audit_g1.c`, deux profils) et par lecture.
- **Emplacement :** `src/noyau/contexte_exec.c:819` (`resultat_independant`) et `:661` (`valeur_locale`), qui portent tous deux `if (…indexee) return ERR_E_INTERNE; /* élément : étape 5 */`.
- **Constat :**
  - Deux branches laissées en attente de l'étape 5 (module `donnees`, réalisé le 2026-10-24) n'ont jamais été complétées.
  - Le compilateur produit une référence indexée pour `T[N]`, faute de connaître le mode du paramètre formel.
- **Scénario :**
  - `10 TABLEAU T[3];T[2]_7;X_&F(2)` avec `30 PROCEDURE &F(N) LOCAL N;RESULTAT T[N]` donne EX 96 en ligne 30.
  - `Y_&G(T[2])` avec `PROCEDURE &G(A) LOCAL A;RESULTAT A+1` donne EX 96 en ligne 10.
- **Référence :** État d'exécution §5.5 (appel, étape 4 : « déréférencement implicite si l'élément est lui-même une REFERENCE » ; retour, étape 2) ; IMP-0208.
- **Proposition :**
  - Traiter `REF_ELEMENT` par `contexte_entree` puis `donnees_lire_element`.
  - Élément sans valeur passé à un paramètre LOCAL : état `ID_PARAMETRE_SANS_VALEUR`, déjà fixé par État d'exécution §12 et RX3 (EX 14 à l'utilisation).
  - Élément sans valeur rendu par RESULTAT : même nature que pour une variable simple sans valeur sur ce chemin.
- **Décision IMP nécessaire :** non (réalisation de §5.5).
- **Suite donnée :** corrigé le 2026-10-29, sans décision IMP : élément de tableau traité dans `valeur_locale` et `resultat_independant` (`contexte_exec.c`) par `contexte_entree` puis `donnees_lire_element`. Pour un élément sans valeur, l'argument LOCAL prend l'état `ID_PARAMETRE_SANS_VALEUR` (RX3) et RESULTAT donne EX 32, comme toute lecture d'élément. Quatre contrôles dans `composants_interprete`.

#### AC-05 — GARER d'un tableau de plus de 65 535 octets d'image écrit des octets non initialisés dans le fichier
- **Statut :** CONFIRMÉ par deux essais indépendants (bloc E de `g1/composants_audit_g1.c` ; §9 de `g4/essai_g4.c`) et par lecture.
- **Emplacement :** `src/noyau/interprete.c:889-891` (`objets_copier(t.contenu.objet, 0, contenu, (unsigned int) lg)`).
- **Constat :**
  - `lg`, la taille d'un tableau (jusqu'à 270 338 octets, Données IMP-0210), est tronquée à 16 bits pour la copie.
  - `malloc(lg)` réserve bien la taille entière, et `fichiers_garer` écrit `lg` octets. Seuls `lg mod 65 536` octets ont été copiés.
  - La valeur rendue par `objets_copier` n'est pas testée.
- **Scénario :** un tableau de plus d'environ 15 887 éléments suffit.
  - `TABLEAU T[16000];T[1]_5;T[16000]_7`, puis `GARER T,1,'#GT'`, `CHARGER U,1,'#GT'` : U[1] = 0 et U[16000] = 0, au lieu de 5 et 7.
  - Avec `T[20000]` : U[16000] et U[20000] valent 0.
- **Référence :** Bibliothécaire et fichiers IMP-0247, IMP-0253 ; Données IMP-0210.
- **Proposition :** `o = objets_resoudre(h); memcpy(contenu, o, (size_t) lg); objets_relacher(h);`.
  - Cette forme est permise par la règle du Gestionnaire d'objets §11 : `malloc` précède, et aucun appel n'a lieu entre l'adresse et la copie.
  - À défaut, copier par tranches avec un décalage `unsigned long`.
  - Dans les deux cas, tester le résultat.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-29, sans décision IMP : GARER copie l'image du tableau par `objets_resoudre`, `memcpy` sur toute la longueur et `objets_relacher` (`interprete.c`), comme CHARGER. Contrôle d'aller-retour sur `T[20000]` (82 506 octets) dans `composants_fichiers`.

#### AC-06 — Pile d'évaluation au-delà de 9 362 éléments : éléments adressés modulo 64 Kio
- **Statut :** CONFIRMÉ par essai (blocs I et F de `g1/composants_audit_g1.c`, deux profils).
- **Emplacement :** `src/noyau/pile.c:47-55` (agrandissement par doublement, sans plafond) ; accès `p->elements[…]` (`pile.c:54, 87, 95, 103` ; `interprete.c:308` ; `contexte_exec.c:782`).
- **Constat :**
  - En modèle small, « the compiler assumes that you will not allocate any single array … larger than 64K bytes using the Memory Manager or malloc » (manuel ORCA/C, `memorymodel`, l. 6333).
  - L'indice `sommet × 7` est donc calculé sur 16 bits, alors que la pile double jusqu'à 16 384 éléments (114 688 octets).
  - Au-delà de 9 362 éléments, chaque écriture tombe sur un élément déjà en place.
  - À 32 768 éléments, `2 * capacite` vaut en outre 0 (`pile.c:48`).
- **Scénario :**
  - 12 000 nombres empilés puis relus : 2 637 faux, exactement ceux qui suivent le 9 362ᵉ.
  - En L.S.E., une fonction récursive en position d'expression laisse un élément par niveau. Avec 15 000 niveaux, on obtient EX 96 au lieu de `X = 15000`.
  - Par raisonnement (non essayé jusqu'à épuisement de la mémoire) : une récursion qui ne se termine pas, faute fréquente d'un programme en mise au point, franchit ce seuil bien avant d'épuiser la mémoire. Elle corrompt donc la pile au lieu de produire « mémoire saturée ».
- **Référence :** manuel ORCA/C l. 6333 ; État d'exécution IMP-0145. Le §12 d'État d'exécution laisse le seuil de croissance « non figé numériquement » ; il n'admet pas une corruption.
- **Proposition :** plafonner la capacité à `65535 / sizeof(ElementPile)` = 9 362 éléments (doublement borné), et rendre EX 03 « mémoire saturée » au-delà (nature attestée, INRDP75). L'adressage en `unsigned long` ou `#pragma memorymodel 1` pénaliserait l'opération la plus fréquente.
- **Décision IMP nécessaire :** oui, révision d'IMP-0145 (plafond chiffré) et mise à jour du §12 (§9, point 4).
- **Suite donnée :** corrigé le 2026-10-30 ; décision d'Éric du 2026-10-30 (LSE 8086 : « erreur d'exécution 3 Mémoire pleine »). Révision d'IMP-0145 : capacité plafonnée à `65535 / sizeof(ElementPile)`, soit 9 362 éléments, et EX 03 au-delà (`pile.c`). Contrôles dans `composants_contexte` (empilement plafonné, éléments intacts) et `composants_interprete` (récursion de 9 000 niveaux réussie, de 15 000 niveaux : EX 03).

#### AC-07 — Réserve des cases au-delà de 8 192 cases : corruption de l'environnement
- **Statut :** CONFIRMÉ par essai (bloc J de `g1/composants_audit_g1.c`, deux profils) et par lecture.
- **Emplacement :** `src/noyau/contexte_exec.c:98-110` (`allouer_case`) et tous les accès `cases[c]`.
- **Constat :**
  - Même cause qu'AC-06, avec des cases de 5 octets.
  - Au passage de 8 192 à 16 384 cases (81 920 octets), la boucle de chaînage écrit les cases 13 108 à 16 383 modulo 64 Kio, c'est-à-dire sur les cases 0 à ≈ 3 276 déjà en usage.
  - La garde `if (n > 65535u)` est sans effet sur un `unsigned int`.
  - Le seuil est atteint par le nombre de cases simultanées : une procédure récursive à trois LOCAL l'atteint vers 2 700 niveaux.
- **Scénario :** `10 K_0;&P(12000)`, `30 PROCEDURE &P(N) LOCAL N`, `40 K_K+1;SI N>0 ALORS &P(N-1)`, `50 RETOUR`.
  - 12 000 appels : `FIN_TERMINER` avec **K = 0** au lieu de 12 001.
  - 14 000 appels : EX 05 en ligne 40.
- **Référence :** manuel ORCA/C l. 6333 ; État d'exécution IMP-0179, IMP-0149.
- **Proposition :** plafonner `nb_cases` à `65535 / sizeof(EntreeEnvironnement)` = 13 107, avec EX 03 au-delà.
- **Décision IMP nécessaire :** oui, révision d'IMP-0179 (§9, point 4).
- **Suite donnée :** corrigé le 2026-10-30 ; décision d'Éric du 2026-10-30. Révision d'IMP-0179 : réserve plafonnée à `65535 / sizeof(EntreeEnvironnement)`, soit 13 107 cases, et EX 03 au-delà (`contexte_exec.c`) ; limite du §12 d'État d'exécution réécrite. Contrôles dans `composants_interprete` : 12 000 appels à un `LOCAL` réussis (K = 12 001), 14 000 : EX 03.

#### AC-08 — Un programme contenant une ligne vide est rangé, puis refusé par APPELER et EXECUTER
- **Statut :** CONFIRMÉ par essai (§1 de `g4/essai_g4.c`, et session complète du moniteur dans `g4/essai_g4c.c`), et par lecture de `bibliotheque.c:652-704`.
- **Emplacement :** `src/noyau/bibliotheque.c:695` (`if (lg == 0 || p + 2 + lg > n - 2) return ERR_F_FICHIER_ENDOMMAGE;`).
- **Constat :**
  - Une ligne vide compile en une séquence de longueur 0 (`compilateur.c:1719-1721` ; Gestionnaire d'objets IMP-0194).
  - RANGER l'écrit sans difficulté, mais `verifier` rejette toute ligne de longueur nulle.
  - `reconstruire` traite pourtant déjà ce cas (`objets_creer(0)`, `memcpy` de 0 octet).
- **Scénario :** `10 FAIRE 20 POUR I_1 JUSQUA 3`, `20 ` (forme attestée, INRDP75), `30 TERMINER`, puis `RANGER VIDE` et `APPELER VIDE` : `ERREUR F 101`. Même résultat avec `EXECUTER 'VIDE'`. Le fichier rangé est inutilisable.
- **Référence :** IMP-0194 ; IMP-0246 (longueur u8, aucune exclusion de 0) ; VM-3402.
- **Proposition :** retirer `lg == 0` de la condition. Les fichiers déjà rangés redeviennent lisibles, car seul le lecteur est en faute. Ajouter le cas à `composants_bibliotheque`.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-29, sans décision IMP : `verifier` (`bibliotheque.c`) accepte une ligne de longueur nulle. Contrôle RANGER puis APPELER d'un programme à ligne vide dans `composants_bibliotheque`.

#### AC-09 — La description d'AFFICHER est perdue à travers un appel de fonction ou une suspension
- **Statut :** CONFIRMÉ par essai (bloc K de `g1/composants_audit_g1.c`, profil `cible`). Le résultat attendu du scénario (1) est à confirmer ; celui du scénario (2) ne fait pas de doute.
- **Emplacement :** `src/noyau/interprete.c:44-48` (`aff_type`, `aff_e`, `aff_d`, `aff_reste` dans `Cycle`), `:541-556`, `:1317`, `:1337` (`memset` du `Cycle` à chaque appel).
- **Constat :** l'état « description en cours » vit dans la structure `Cycle`, locale à `interprete_executer`.
  - (1) Un AFFICHER exécuté dans une fonction appelée depuis la liste d'un AFFICHER remet `aff_reste` à 0. Au retour, les valeurs restantes sortent en U.
  - (2) Toute suspension dans la fonction détruit le `Cycle` : `FIN_PAS`, `FIN_INTERRUPTION`, `FIN_LECTURE`, PAUSE. À la reprise, la description est perdue.
- **Scénario :**
  - (1) `10 AFFICHER[2F6.2] &F(1),2` avec `30 PROCEDURE &F(A) LOCAL A;AFFICHER['*'];RESULTAT A` affiche `*1 2 ` au lieu de `*     1.00     2.00` (Données §4 : `F e.d` donne e colonnes à la partie entière, puis le point et d décimales).
  - (2) Le même programme sans AFFICHER dans F, exécuté en PAS A PAS (F sur deux lignes), affiche `1 2 ` au lieu de `     1.00     2.00` : le pas à pas change la sortie.
- **Référence :** Interpréteur IMP-0221 (une description « s'applique aux r valeurs qui la suivent ») ; IMP-0214 (l'état est conservé pour la reprise).
- **Proposition :** sortir ces quatre octets du `Cycle` et les sauvegarder et restaurer autour d'un appel, dans un champ opaque de `CadreActivation` renseigné à `RCIM_OP_APPEL` et rendu par `retourner`. Les remettre à zéro à EXECUTER et à REPRENDRE.
- **Décision IMP nécessaire :** oui, révision d'IMP-0221, et d'IMP-0146 si le champ va dans le cadre (§9, point 5).
- **Suite donnée :** corrigé le 2026-10-30 ; décision d'Éric du même jour, après essais sur LSE 8086 (`AFFICHER[2F6.2] &F(1),3`, F exécutant `AFFICHER['*']` : `*     1.00     3.00`). Révision d'IMP-0221 et d'IMP-0146 : la description en cours devient un état de l'interpréteur (`DescriptionAffichage`), conservé à travers les suspensions. Elle est rangée dans le cadre de l'appelant pendant un appel (`contexte_garder_affichage`, `contexte_rendre_affichage`), et protégée pendant une ligne de machine de bureau (`interprete.c`, `contexte_exec.c`). Deux sessions de contrôle dans `composants_moniteur` : l'essai d'Éric, et le même AFFICHER en pas à pas.

### 5.2 Majeurs

#### AC-10 — DECODER restitue des lignes de plus de 79 caractères qu'ENCODER refuse, sans le signalement exigé par IMP-0183
- **Statut :** CONFIRMÉ par essai pour la longueur restituée (§10 de `g4/essai_g4.c`), et par lecture complète du chemin ENCODER.
- **Emplacement :** `src/noyau/codage.c:97-107` (texte restitué rangé tel quel) ; refus dans `src/noyau/compilateur.c:1820-1823` (`COMPILATEUR_LONGUEUR_MAX` = 79), appelé par `moniteur.c:1401`.
- **Constat :**
  - La forme canonique peut dépasser la ligne source : `1E5` devient `100000`.
  - RCI-M IMP-0183, « Longueur des lignes restituées », dispose : « DECODER signale toute ligne trop longue pour être recompilée par ENCODER (comportement exact : chapitre du bibliothécaire) ».
  - Le chapitre Bibliothécaire et fichiers ne fixe pas ce comportement (Décompilateur §13 y renvoie), et le code ne signale rien.
  - ENCODER s'arrête à la première erreur (IMP-0261).
- **Scénario :**
  - `1 X_1E5+1E5+…+1` (77 caractères) est restitué en 131 caractères.
  - `DECODER XX`, puis `ENCODER XX` : la ligne est affichée, suivie de `ERREUR C 63`. Sous MITRA 15 sans n1, le programme courant a déjà été vidé.
- **Référence :** RCI-M IMP-0183 ; Décompilateur IMP-0198, §13 ; Bibliothécaire et fichiers IMP-0260, IMP-0261 ; Compilateur (C 63).
- **Proposition :** fixer d'abord le comportement dans le chapitre Bibliothécaire, puis le réaliser. Deux voies :
  - signaler la ligne à DECODER, sans la refuser, conformément à la lettre d'IMP-0183 ;
  - ou faire compiler les lignes reçues par ENCODER et par le ruban avec la borne du décompilateur (255).
- **Décision IMP nécessaire :** oui, pour compléter IMP-0183 et IMP-0260 au chapitre Bibliothécaire (§9, point 7).
- **Suite donnée :** non corrigé, choix d'Éric du 2026-10-30. ENCODER affiche la ligne trop longue suivie de l'erreur C 63, ce qui suffit ; DECODER n'ajoute pas de message. IMP-0183 (RCI-M) est amendée en ce sens, avec la fiche de codage et Décompilateur §13.

#### AC-11 — Golden Gate fournit 16 Kio de pile : aucune suite ne peut révéler un débordement de la cible
- **Statut :** CONFIRMÉ par essai (`g5/essai_debordement.c`, `g5/essai_pile.c`) : 16 110 octets disponibles sous `main` sous Golden Gate, contre 3 825 avec 4 Kio.
- **Emplacement :** `build/Makefile` (toutes les suites) ; Construction IMP-0192.
- **Constat :**
  - Les 29 suites tournent avec plus de quatre fois la pile de la cible. Aucune ne mesure la pile, et les sondes portées sur l'Apple IIGS n'emploient que des lignes peu imbriquées.
  - Même avec `stacksize` imposée, Golden Gate laisse plus de pile que l'Apple IIGS : 270 octets de plus, à 4 Kio comme à 16 Kio (§3). Une suite de non-régression doit donc retrancher cet écart de sa borne.
- **Scénario :** une modification qui ajoute 1 Kio de locales sur le chemin du compilateur passe `make tests`, puis plante sur l'Apple IIGS.
- **Référence :** IMP-0192 ; manuel ORCA/C, `stacksize`.
- **Proposition :**
  - donner à `LSE` et aux suites la même `stacksize`, ce que fait déjà AC-01 (a) ;
  - ajouter une suite de non-régression de pile sur le modèle d'`essai_pile`, avec une borne vérifiée par scénario, pire cas de 38 parenthèses compris.
- **Décision IMP nécessaire :** oui, à joindre à la décision d'AC-01 (révision d'IMP-0192 pour la suite).
- **Suite donnée :** corrigé le 2026-10-30 ; décision d'Éric du 2026-10-30, complément d'IMP-0272 (Construction §5 bis). Nouvelle suite `plateforme_pile` dans `make tests`, construite avec `#pragma stacksize 16384` : onze sessions exigeantes, profondeur mesurée par peinture de la pile, borne de 14 816 octets (15 840 disponibles sur la machine, moins 1 Kio pour SANE et GS/OS). Profondeur maximale mesurée : 11 067 octets en `cible`, 11 131 en `dev`.

#### AC-12 — Arrêt sur débordement : la console et la mémoire ne sont pas rendues, et le contrôle ne couvre ni SANE ni GS/OS
- **Statut :** CONFIRMÉ par essai (`g5/essai_atexit.c` : message `Stack overflow`, code 255, gestionnaire `atexit` non appelé) et par lecture. L'état du shell après l'arrêt reste à observer sur la machine.
- **Emplacement :** `src/plateforme/main.c:50-52` (`console_fermer` et `objets_terminer` sur le seul chemin normal) ; `src/plateforme/console_iigs.c:85-113`.
- **Constat :**
  - `SystemErrorLocation` arrête le programme sans repasser par `main` (manuel ORCA/C, chap. 20).
  - Le pilote `.CONSOLE` reste en lecture brute, avec le mot de mouvement à `$0D` (CR sans LF).
  - Les blocs ne sont pas rendus (IMP-0270), et aucun diagnostic L.S.E. n'est donné.
  - Près du seuil, un débordement dans SANE ou GS/OS n'est pas détecté.
- **Scénario :** AC-01 sur l'Apple IIGS. Le retour au shell peut se faire avec une console sans écho ou un CR sans interligne, si le shell ne rétablit pas lui-même les modes du pilote (à vérifier).
- **Référence :** manuel ORCA/C, chap. 20 (`SystemError`, `SystemErrorLocation`) et chap. 13 (`debug`) ; IMP-0240, IMP-0270.
- **Proposition :** définir `SystemError` dans `src/plateforme/`. Elle appelle `console_fermer()`, puis `SystemPrintError` et `SystemErrorLocation`. La reprise au moniteur par `longjmp` est possible (essayée par le groupe 5), mais il faudrait remettre en état les symboles créés et les objets verrouillés : à arbitrer.
- **Décision IMP nécessaire :** oui, nouvelle décision ou révision d'IMP-0240 (§9, point 1).
- **Suite donnée :** corrigé le 2026-10-30 ; décision d'Éric du 2026-10-30, IMP-0273 (Moniteur et console §9 bis). `SystemError` est remplacée dans `console_iigs.c` : `console_fermer`, puis `SystemPrintError` et `SystemErrorLocation`. Les deux fonctions sont placées en fin de fichier, après `#pragma debug 0`, sans contrôle de pile et sans appel à une fonction qui en aurait un. Sonde `SONDEE` (`tests/plateforme/sonde_erreur.c`, sur l'image) : sous Golden Gate, « Stack overflow » et arrêt, sans boucle. Vérifié sur l'Apple IIGS par Éric le 2026-10-30 : après « Stack overflow », la frappe s'affiche et RETURN passe à la ligne.

#### AC-13 — Un LIRE de machine de bureau pendant une lecture suspendue perd les valeurs déjà lues par le programme
- **Statut :** CONFIRMÉ par deux essais (bloc G de `g1/composants_audit_g1.c` ; D6 de `g3/audit_g3.c`).
- **Emplacement :**
  - état de lecture statique `interprete.c:578-584` (`saisie`, `saisie_pos`, `rang_tableau`) ;
  - `interprete.c:1184-1187` (`RCIM_CTL_LIRE_TETE` remet `rang_tableau` à 0) ;
  - `moniteur.c:1535-1564` ;
  - contrat de `interprete.h:41-43`, qui demande `interprete_reinitialiser` pour une ligne de machine de bureau, ce que le moniteur ne fait pas.
- **Constat :** l'état de la lecture en cours est commun au programme et à la machine de bureau, et n'est pas sauvegardé autour d'une ligne de bureau.
- **Scénario :** `10 TABLEAU T[3];LIRE T`. On répond `1`, puis ESC, `LIRE Y` (réponse `9`), CONTINUER et `2 3`. Résultat : T = 2, 3, non défini, avec une nouvelle lecture en attente. IMP-0237 attend 1, 2, 3 : « les valeurs déjà lues étant conservées ».
- **Référence :** Moniteur IMP-0237, IMP-0233 ; Interpréteur IMP-0222 ; `interprete.h:41-43`.
- **Proposition :** quand le programme est suspendu, sauvegarder l'état de lecture (≈ 260 octets) à l'entrée de `ligne_bureau` et le restaurer à la sortie. La machine de bureau part alors d'un état vierge. Corriger aussi le commentaire d'`interprete.h`. Autre voie : refuser LIRE en machine de bureau pendant une lecture suspendue.
- **Décision IMP nécessaire :** oui, précision d'IMP-0222 ou d'IMP-0237 (§9, point 6).
- **Suite donnée :** corrigé le 2026-10-30. LIRE reste permis en machine de bureau (MEMENTO82, d'après Éric). IMP-0222 et IMP-0233 sont complétées : une ligne de bureau part d'un état de lecture vierge, et celui du programme est rétabli à sa sortie (`interprete_lecture_mettre_de_cote`, `interprete_lecture_retablir`, appelées par `ligne_bureau`). Contrôle dans `composants_moniteur` : `LIRE T` interrompu après `1 2`, `LIRE X` en bureau ; CONTINUER ne demande que la troisième valeur, T = 1, 2, 3 et X = 9.

### 5.3 Mineurs

#### AC-14 — Le reste d'une ligne tapée pour un programme terminé alimente le LIRE suivant de la machine de bureau
- **Statut :** CONFIRMÉ par essai (D5 de `g3/audit_g3.c`). Sa qualification comme défaut est à arbitrer.
- **Emplacement :** `src/noyau/moniteur.c:298-355` (`executer` : aucun `interprete_reinitialiser` en fin d'exécution) ; contrat `src/noyau/interprete.h:41-42`.
- **Scénario :** `10 LIRE A`, `20 TERMINER`, réponse `1 2` : `TERMINE`. Puis `LIRE X` en machine de bureau : X reçoit 2 sans rien demander. La saisie suivante, `7`, est prise pour une ligne et donne `ERREUR C 07`.
- **Référence :** Moniteur §11, point L6 (reste conservé, attesté pour deux LIRE d'un même programme) ; IMP-0214.
- **Proposition :** appeler `interprete_reinitialiser()` quand l'exécution finit par `FIN_TERMINER` ou `FIN_ERREUR` (pas pour `FIN_INTERRUPTION`), et aligner le commentaire de `interprete.h`.
- **Décision IMP nécessaire :** oui, portée du point L6 (§9, point 6).
- **Suite donnée :** corrigé le 2026-10-30 par la même règle qu'AC-13 : un LIRE de bureau après un programme terminé attend une nouvelle saisie. LSE 8086 lit au fil de la frappe, et aucun reste n'y survit à un LIRE (essai d'Éric). Contrôle dans `composants_moniteur`.

#### AC-15 — Échec de `contexte_initialiser` après l'abandon des cadres : l'état reste « suspendu »
- **Statut :** PLAUSIBLE. Le chemin a été lu entièrement (`moniteur.c:383-392`, `contexte_exec.c:403-410`, `:433-445`), mais l'échec de `malloc` dans `creer_cadre` n'a pas été provoqué.
- **Emplacement :** `src/noyau/moniteur.c:389-392` (`lancer`).
- **Constat :** si `creer_cadre` échoue après l'abandon de tous les cadres, `courant` vaut `NULL`. `lancer` affiche l'erreur mais laisse `etat_programme` inchangé : CONTINUER relance alors l'interpréteur sans cadre.
- **Scénario :** programme suspendu et mémoire presque épuisée. EXECUTER donne `ERREUR (EX.) 03`, puis CONTINUER lit à l'adresse 0.
- **Référence :** Moniteur IMP-0232 ; État d'exécution §7.1.
- **Proposition :** `etat_programme = PROG_AUCUN` dès que `lancer` reçoit une autre erreur que `ERR_E_LIGNE_INEXISTANTE`.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-29, sans décision IMP : `lancer` (`moniteur.c`) remet l'état du programme à « aucun » dès que l'initialisation ou la reprise échoue après l'abandon des cadres, seule possibilité étant la saturation de la mémoire. Aucun contrôle ajouté : il faudrait provoquer l'échec d'une allocation.

#### AC-16 — RANGER boucle sans fin si l'image RCI-S dépasse 65 535 octets
- **Statut :** CONFIRMÉ par essais :
  - `g4/essai_g4b.c` : 1 200 constantes, RANGER ne rend plus la main, arrêté après 3 min ;
  - `g4/essai_g4d.c` : contre-essai à 1 000 constantes, RANGER rend la main en 1 s.
- **Emplacement :** `src/noyau/bibliotheque.c:587` (`for (i = 0; (unsigned long) i < p; i++)`, avec `i` déclaré `unsigned int` en `:543`).
- **Constat :**
  - `i` repasse à 0 après 65 535, et la condition reste vraie.
  - L'image dépasse 64 Kio quand les constantes partagées s'accumulent. Elles ne sont jamais retirées avant `programme_vider` (`symboles.c:120-160`), et RANGER les range toutes, même les mortes.
  - Le déclenchement est improbable, mais la conséquence est un blocage complet.
- **Référence :** IMP-0246 ; `verifier` emploie déjà un indice `unsigned long` (`:656`, `:669`).
- **Proposition :** indice `unsigned long`. L'accumulation des constantes mortes est un point distinct (§9, point 11).
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-29, sans décision IMP : somme de contrôle d'`image_rcis` (`bibliotheque.c`) parcourue avec un indice `unsigned long`. Contrôle dans `composants_bibliotheque` : 1 200 constantes partagées (image de plus de 64 Kio), RANGER puis APPELER. L'accumulation des constantes mortes reste au §9, point 11.

#### AC-17 — La vérification RCI-S ne contrôle ni identifiants, ni index, ni unicité
- **Statut :** CONFIRMÉ par essais (§2 et §3 de `g4/essai_g4.c`).
- **Emplacement :** `src/noyau/bibliotheque.c:652-704` (`verifier`), `:719-753` (`reconstruire`) ; conséquence possible `src/noyau/contexte_exec.c:139-141` (`n = id + 1` vaut 0 pour `id` = 0xFFFF, puis `noms[id]` hors bornes).
- **Constat :**
  - IMP-0246 annonce la vérification de « chaque identifiant de symbole et chaque index de constante » ; `verifier` ne fait que parcourir les longueurs d'éléments.
  - Les fautes de ce genre ne sont découvertes que par `reconstruire`, après `contexte_vider` et `programme_vider`.
  - La somme de contrôle est additive : une permutation d'octets ne la change pas.
- **Scénario :**
  - (a) Identifiant de symbole porté à 9 alors que `ns` = 2, somme recalculée : APPELER réussit, puis la décompilation de la ligne échoue.
  - (b) Un symbole renommé en doublon donne `ERREUR F 23` et vide le programme courant, contrairement à VM-3403.
- **Référence :** IMP-0246 (« Restauration ») ; VM-3403 ; Gestionnaire des erreurs §9 ter.
- **Proposition :** compléter `verifier`, et rendre F 101 dans tous ces cas :
  - nature et origine des symboles dans leurs plages ;
  - noms valides et uniques ;
  - constantes de 17 à 127 octets, uniques ;
  - bornes `< ns` et `< nc` pour chaque élément qui porte un identifiant ou un index (une fonction de `rcim.c` rendrait ces champs).
- **Décision IMP nécessaire :** non (réalise IMP-0246 tel qu'écrit).
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, §9 point 11), complément d'IMP-0246. `verifier` (`bibliotheque.c`) contrôle, avant de vider le programme : noms (une lettre puis des lettres ou des chiffres, complétés de zéros), nature et origine, constantes de 17 à 127 octets, plafonds des tables (AC-32), absence de doublons parmi les symboles (même nom, même nature) et parmi les constantes, et, par la nouvelle fonction `rcim_sequence_conforme` (`rcim.c`), chaque identifiant et chaque index de chaque séquence. Les doublons sont cherchés par une table de hachage temporaire de 64 Kio au plus. Contrôles : `composants_bibliotheque` (sept fichiers altérés, somme de contrôle recalculée : identifiant 9 pour 2 symboles, index 5 pour 2 constantes, deux symboles A, deux constantes égales, minuscule, nom non complété de zéros, nature 2 ; F 101 et programme courant intact à chaque fois), `representation_rcim` (bornes, PROCEDURE comprise).

#### AC-18 — GARER : une étendue est un moment à la fois vivante et libre
- **Statut :** CONFIRMÉ (chemin lu entièrement).
- **Emplacement :** `src/noyau/fichiers.c:294-297`, `:317`, `:330`.
- **Constat :** quand l'enregistrement reçoit l'étendue libre et que l'entrée libre sert ensuite à noter l'ancienne étendue, l'entrée de l'enregistrement est réécrite d'abord (`:317`). Jusqu'à la réécriture de l'entrée libre (`:330`), la même étendue est décrite deux fois.
- **Scénario :** une erreur d'entrée-sortie en `:330` laisse l'étendue marquée libre. Le GARER suivant la réemploie et écrase l'enregistrement vivant.
- **Référence :** IMP-0247 (ordre des écritures) ; VM-3405.
- **Proposition :**
  - si les deux entrées sont dans le même secteur de répertoire, une seule écriture ;
  - sinon, marquer d'abord l'entrée libre `ETAT_INUTILISE`, puis écrire l'enregistrement, puis l'ancienne étendue.
- **Décision IMP nécessaire :** non (précise l'ordre d'IMP-0247 sans changer le format).
- **Suite donnée :** corrigé le 2026-10-30, sans décision IMP : dans `garer_ouvert` (`fichiers.c`), quand l'entrée d'une étendue libre reprise doit décrire l'ancienne étendue, elle est d'abord marquée `ETAT_INUTILISE`. Jusqu'à la fin de GARER, un incident ne peut que perdre l'ancienne étendue ; la nouvelle n'est jamais décrite à la fois vivante et libre (VM-3405). Contrôle de ce chemin dans `composants_fichiers` : remplacement par une étendue libre reprise, valeur relue juste, fichier de même taille.

#### AC-19 — Une étendue libre est prise entière : place perdue, EX 52 prématuré
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `src/noyau/fichiers.c:282-284` (`rec.nb = libre.e.nb`).
- **Scénario :**
  - GARER d'un tableau de 1 000 éléments (17 secteurs) en enregistrement 1, puis SUPPRIMER 1 : l'étendue de 17 secteurs devient libre.
  - GARER de nombres dans les enregistrements 2 à 40 : l'enregistrement 2 prend les 17 secteurs, et les suivants s'ajoutent en fin de fichier.
  - Avec le quota de 200 secteurs, EX 52 arrive plus tôt.
- **Référence :** IMP-0247 (« la première étendue libre assez grande », sans règle de découpe) ; IMP-0253.
- **Proposition :** découper l'étendue quand une entrée inutilisée est disponible dans le même secteur de répertoire.
- **Décision IMP nécessaire :** oui, précision d'IMP-0247 (§9, point 12).
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, §9 point 12), complément d'IMP-0247. Dans `garer_ouvert` (`fichiers.c`), une étendue libre plus longue que le besoin est découpée quand son secteur de répertoire porte une entrée inutilisée (`inutilisee_voisine`). Cette entrée décrit la fin de l'étendue, dans la même écriture de secteur que l'entrée qui cesse de décrire l'étendue reprise (`ecrire_place` à deux entrées). À défaut d'entrée inutilisée, l'étendue est prise entière.
  - **Défaut voisin, trouvé à la correction :** quand l'enregistrement existait avec une longueur nulle (chaîne vide), l'étendue libre reprise restait décrite comme libre tout en portant le nouveau contenu ; le GARER suivant l'écrasait. Exemple : `GARER V,2,'ZL'` avec `V_''`, puis `V_'XYZ'` rangé en 2 et `'ABC'` en 3 : CHARGER de l'enregistrement 2 rendait `ABC`. Corrigé par la même étape : dès qu'une étendue est reprise pour un enregistrement existant, son entrée cesse de la décrire avant l'écriture de l'entrée de l'enregistrement.
  - **Contrôles** dans `composants_fichiers` : découpe pour un enregistrement remplacé et pour un nouveau, fichier de même taille, valeurs relues ; enregistrement de longueur nulle remplacé. Les deux premiers échouaient avant la correction.

#### AC-20 — Numéros de secteur sur 16 bits sans borne
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `src/noyau/fichiers.c:219-223`, `:286-287`, `:309`, `:311-313`.
- **Constat :** `taille`, `rec.premier` et `rec.nb` sont sur 16 bits (format u16, IMP-0247). `rec.premier + i` et `taille + rec.nb` repassent à 0 au-delà. Les fichiers permanents ne sont bornés que par le support.
- **Scénario :** des GARER répétés de tableaux de 65 535 éléments (1 057 secteurs) dans un fichier permanent. Au 63ᵉ, l'étendue écrase les secteurs 0, 1… (le répertoire).
- **Référence :** IMP-0247, IMP-0253.
- **Proposition :** refuser si `(unsigned long) taille + croissance > 65535ul` (EX 52), dans `garer_ouvert` et dans `nouveau_repertoire`.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-30, sans décision IMP : `garer_ouvert` refuse avec EX 52 toute écriture qui porterait le fichier au-delà de 65 535 secteurs, numéros sur 16 bits, avant toute écriture. Contrôle par lecture seulement : un essai demanderait un fichier de 16 Mio.

#### AC-21 — Chemins d'échec des fichiers : bibliothèque incohérente ou échec muet
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `src/noyau/bibliotheque.c:641-645` (RANGER), `:636-638` (MODIFIER) ; `src/noyau/fichiers.c:363-367` (GARER créant un fichier permanent), `:504-505` (CATALOGUER) ; `src/noyau/moniteur.c:595` (`SUPPRIMER *` sous T1600 : résultat de `plateforme_espace_vider` ignoré).
- **Constat :**
  - Le fichier est écrit avant l'inscription au catalogue. Si l'inscription échoue, il reste sans propriétaire, et `droit` (`:407`) laisse alors tout compte le modifier.
  - MODIFIER supprime l'original avant de renommer `TRANSIT`.
  - `SUPPRIMER *` ne signale pas un vidage incomplet.
- **Scénario :** support plein au moment d'écrire l'entrée de `CATALOGUE` : `ERREUR F 02`, avec un fichier présent mais non catalogué.
- **Référence :** IMP-0248, IMP-0251, IMP-0255.
- **Proposition :**
  - en cas d'échec d'inscription, supprimer le fichier créé ;
  - pour MODIFIER, renommer d'abord l'original sous un nom de sauvegarde ;
  - pour `SUPPRIMER *`, afficher l'erreur rendue.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-30, sans décision IMP : RANGER et GARER d'un fichier permanent nouveau suppriment le fichier si son inscription au catalogue échoue. CATALOGUER inscrit avant de supprimer le temporaire, et retire le permanent en cas d'échec. MODIFIER renomme l'original en `ANCIEN`, met `TRANSIT` à sa place, puis supprime `ANCIEN` ; en cas d'échec, l'original est rétabli. `SUPPRIMER *` (T1600) affiche F 27 si le vidage échoue. Contrôle : aucun `ANCIEN` ne subsiste après MODIFIER (`composants_bibliotheque`). Les échecs d'entrée-sortie eux-mêmes ne sont pas provoqués.

#### AC-22 — Écriture F ou E d'un grand nombre : caractères autres que des chiffres
- **Statut :** CONFIRMÉ par essai (bloc H de `g1/composants_audit_g1.c`).
- **Emplacement :** `src/noyau/donnees.c:464-489` (`'0' + (int) (x - q * 10.0 + 0.5)`).
- **Constat :** au-delà de 2⁵³, `x − q × 10` n'est plus exact. Données §4 admet que « les chiffres au-delà du quinzième peuvent différer », mais pas des caractères `@` ou `-` au milieu d'un nombre.
- **Scénario :** `F30.2` de `1E20` donne `10000000200408772@000.00`.
- **Référence :** Données IMP-0211, §4.
- **Proposition :** borner le chiffre à 0…9. Mieux : tirer les chiffres exacts de la mantisse du `float`.
- **Suite donnée :** corrigé le 2026-10-30, sans décision IMP : chiffre borné à 0…9 dans `chiffres_entier` et `chiffres_fixes` (`donnees.c`), sans fonction auxiliaire, pour ne pas ajouter de conversion SANE par chiffre. Contrôles dans `composants_donnees` : `F30.2` de `1E20` et `F45.0` de `3E38` ne contiennent que des chiffres, des espaces et le point. L'écriture exacte des chiffres au-delà du quinzième reste la limite documentée de Données §4.
- **Décision IMP nécessaire :** non pour le bornage ; oui (IMP-0211) pour une écriture exacte.

#### AC-23 — CHARGER d'un tableau : l'en-tête lu dans le fichier n'est pas validé
- **Statut :** CONFIRMÉ (lecture de `interprete.c:957-965` et `donnees.c:611-656`).
- **Emplacement :** `src/noyau/interprete.c:957` (seule condition : `lg >= 6`).
- **Constat :** un en-tête incohérent avec la longueur (genre, dimensions, bornes) fait lire et écrire hors de l'objet, dans les objets voisins du bloc-maître.
- **Proposition :** exiger le genre tableau, `dims` ∈ {1, 2}, `d1, d2 ≥ 1`, `d2 == 1` si `dims == 1`, et `lg == 6 + (n + 7)/8 + 4n`. Sinon, `ERR_F_FICHIER_ENDOMMAGE`.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-29, sans décision IMP : nouvelle fonction `donnees_image_tableau_valide` (`donnees.c`) : genre, nombre de dimensions, bornes et longueur exacte. CHARGER (`interprete.c`) la consulte, vérifie aussi l'accord du type d'enregistrement avec les dimensions, et rend F 101 sinon. Contrôles dans `composants_donnees`.

#### AC-24 — `composer()` écrit sans borne dans `c[200]`
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `src/plateforme/fichiers_iigs.c:33-53`, `:82`, `:194-198`.
- **Constat :** seul le préfixe est contrôlé (`strlen(chemin) + 20 >= 200`), alors que `plateforme_fichier_lister` rend des noms de 15 caractères qui sont ensuite rouverts par nom.
- **Scénario :** un `-BIB` de 172 à 179 caractères et un fichier étranger de 8 à 15 caractères dans `TEMPORAIRES`. Jusqu'à 8 octets sont alors écrits au-delà de `c[200]`, sur la pile. Cas peu réaliste : il faut au moins 11 niveaux de répertoires.
- **Référence :** IMP-0245, IMP-0249.
- **Proposition :** contrôler la longueur dans `composer`, ou porter la marge à 29.
- **Suite donnée :** corrigé le 2026-10-30, sans décision IMP : marge des chemins portée à 29 caractères (`MARGE_CHEMIN`, `fichiers_iigs.c`) pour la bibliothèque et l'unité : « : », `TEMPORAIRES`, « : », un nom de 15 caractères au plus, zéro final. Contrôle par lecture seulement : un essai demanderait une arborescence de plus de 170 caractères.
- **Décision IMP nécessaire :** non.

#### AC-25 — L'espace du ruban est traité comme une unité amovible (défaut latent)
- **Statut :** CONFIRMÉ (lecture). Aucun appelant actuel n'ouvre le ruban en création.
- **Emplacement :** `src/plateforme/fichiers_iigs.c:105-109` : la condition `e >= ESPACE_UNITE_PROGRAMMES` inclut `ESPACE_RUBAN`.
- **Proposition :** `e >= ESPACE_UNITE_PROGRAMMES && e <= ESPACE_UNITE_RACINE`.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-29, sans décision IMP : condition limitée aux espaces de l'unité amovible (`fichiers_iigs.c`). Contrôle dans `plateforme_fichiers` : le ruban ouvert en création est un fichier.

#### AC-26 — Repli stdio : une fin de fichier sur l'entrée fait boucler la session
- **Statut :** CONFIRMÉ par essai : `printf 'BO\n' | iix LSE` affiche `BONJOUR`, puis `PRET` sans fin.
- **Emplacement :** `src/plateforme/console_iigs.c:125-129` (`EOF` → `CAR_ESC`).
- **Constat :** ce défaut ne touche que Golden Gate, sans le pilote `.CONSOLE`. Il gêne les essais pilotés par l'entrée standard.
- **Référence :** IMP-0240.
- **Proposition :** à la fin de fichier, terminer la session (AU REVOIR implicite, ou retour de `main`).
- **Décision IMP nécessaire :** oui, révision mineure d'IMP-0240 (§9, point 14).
- **Suite donnée :** corrigé le 2026-10-30, choix d'Éric : complément d'IMP-0240. La fin de l'entrée est rendue comme ESC, `console_fin_entree` la signale, et le moniteur clôt la session par un AU REVOIR implicite (`fin_de_session`). Vérifié sous Golden Gate : `BO`, `?1+1`, fin de l'entrée → `2`, puis `AU REVOIR`, et le programme rend la main.

#### AC-27 — Options `-ID` et `-TEMP` non contrôlées
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `src/plateforme/main.c:28-37`.
- **Constat :**
  - `(unsigned int) atol(…)` tronque à 16 bits.
  - `-TEMP -5` donne un quota de 4 294 967 291 secteurs, `-TEMP X` un quota de 0.
  - Une option inconnue (`-t1600`, par exemple) ou sans argument est ignorée en silence.
- **Référence :** IMP-0253, IMP-0268.
- **Proposition :** analyse décimale bornée, et message en cas d'option invalide.
- **Décision IMP nécessaire :** oui, comportement sur option invalide (§9, point 14).
- **Suite donnée :** corrigé le 2026-10-30, choix d'Éric : compléments d'IMP-0253 et d'IMP-0268. `main.c` vérifie chaque option : `-ID` de 0 à 65 499, `-TEMP` strictement positif, argument présent. Une option inconnue ou invalide affiche `OPTION INCORRECTE : …` et arrête le lancement. Vérifié sous Golden Gate avec `-t1600`, `-TEMP X`, `-ID 66937` et `-BIB` sans argument.

#### AC-28 — F 102 au lancement : message codé en dur, et repli silencieux sur la bibliothèque par défaut
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `src/plateforme/main.c:45-49` ; `src/plateforme/fichiers_iigs.c:81-84`.
- **Constat :**
  - `"ERREUR F 102"` ne passe pas par `erreurs_formater_fichier` et ignore donc `-LIBELLES` (IMP-0250).
  - Avec un `-BIB` trop long, la session continue sur `@:LSE`.
- **Proposition :** employer `erreurs_formater_fichier(ERR_F_BIBLIOTHEQUE, …)`. Après un échec, refuser les opérations de fichier ou vider le préfixe.
- **Décision IMP nécessaire :** non pour le message ; le comportement après F 102 est à préciser (§9, point 14).
- **Suite donnée :** corrigé le 2026-10-30, choix d'Éric : complément d'IMP-0250. Le message passe par `erreurs_formater_fichier`. `plateforme_bibliotheque_disponible` retient l'échec, et les commandes et instructions de fichiers rendent F 102. Vérifié sous Golden Gate : avec `-BIB` trop long, `ERREUR F 102` au lancement, puis RANGER rend F 102.

#### AC-29 — Diagnostics de plateforme confondus
- **Statut :** CONFIRMÉ (essais sous Golden Gate du 2026-10-30, annexe `Audit_code_2026-10-29_essais/ac29/`). Il manquait deux vérifications au moment de l'audit : que `fwrite` et `fflush` d'ORCA/C posent `errno = ENOSPC` sur un disque plein, et le cas « `r+b` refusé, `w+b` accepté » sur GS/OS. Elles infirment la proposition et l'un des deux risques (voir « Suite donnée »).
- **Emplacement :** `src/plateforme/fichiers_iigs.c:111-114`, `:118-119`, `:128-129` ; appelants qui retentent en création : `fichiers.c:347-350`, `bibliotheque.c:144-145`, `:230-235`.
- **Constat :**
  - Toute erreur d'écriture devient « support plein ».
  - Tout échec d'ouverture devient « inexistant », et les appelants tentent alors une création `w+b`, qui vide un fichier existant.
- **Scénario :** GARER sur une disquette protégée annonce « support plein ». Un fichier occupé ou protégé, s'il était ouvrable en `w+b`, serait vidé (à vérifier).
- **Référence :** IMP-0249, IMP-0250 ; `errno.h` d'ORCA/C.
- **Proposition :** `PLAT_PLEIN` seulement pour `ENOSPC`, `PLAT_INEXISTANT` seulement pour `ENOENT`, `PLAT_ERREUR` sinon.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, option a), complément d'IMP-0249.
  - Essais (sondes `essai_ac29.c` et `essai_ac29w.c`) :
    - fichier absent, en `rb` ou `r+b` : `ENOENT`, GetFileInfo `$46` ; répertoire absent : `ENOENT` en `r+b`, `EIO` en `w+b`, GetFileInfo `$44` ;
    - fichier verrouillé (accès `$01`, posé depuis l'hôte : Golden Gate ignore SetFileInfo, et `chmod` d'ORCA/C appelle ClearBackupBit, qu'il n'implémente pas) : `r+b` pose `ENOENT`, comme pour un fichier absent ; `w+b` échoue sur `EEXIST` et laisse le fichier intact ;
    - volume plein (image HFS de 1 Mo montée sur l'hôte) : WriteGS rend `$48`, mais `fwrite` pose `EIO` ;
    - `toolerror()` reste nulle après ces échecs ; `remove` rend une valeur non nulle sans poser `errno`.
  - La proposition est donc inapplicable : elle aurait changé chaque « support plein » en erreur d'entrée-sortie, et n'aurait pas distingué un fichier verrouillé d'un fichier absent. Le risque d'effacement par `w+b` n'existe pas.
  - Correction : après l'échec d'une ouverture en lecture ou en mise à jour, ou d'une suppression, `fichiers_iigs.c` appelle `GetFileInfoGS` (`$46`, `$44` : `PLAT_INEXISTANT` ; sinon `PLAT_ERREUR`). `bibliotheque.c` (identifications, catalogue, SUPPRIMER) et `fichiers.c` (`ouvrir`, SUPPRIMER) ne créent qu'un fichier absent et rendent F 27 pour un fichier présent mais refusé.
  - Limite consignée dans IMP-0249 : tout échec d'écriture reste « support plein ». Des appels GS/OS directs la lèveraient ; cette alternative est renvoyée à l'arbitrage d'AP-17.
  - Essais ajoutés, un répertoire non vide tenant lieu de fichier verrouillé : `plateforme_fichiers` (41 contrôles), `composants_fichiers` (66), `composants_bibliotheque` (82).

#### AC-30 — Makefile : trois objets de plateforme ne dépendent pas de `profil.h`
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `build/Makefile:102`, `:105`, `:108` (`console_iigs.a`, `fichiers_iigs.a`, `horloge_iigs.a`).
- **Constat :** une modification de `profil.h` ne recompile pas ces trois modules. Le changement de `PROFIL` reste traité par `make clean` (`build/README.md`).
- **Proposition :** ajouter `$(INTERFACES)` aux trois règles.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-29, sans décision IMP : `$(INTERFACES)` ajouté aux règles de `console_iigs.a`, `fichiers_iigs.a` et `horloge_iigs.a`, dans `build/Makefile` et dans sa reproduction au chapitre Construction §7.

#### AC-31 — Écarts rédactionnels entre la spécification et le code
- **Statut :** CONFIRMÉ (lecture ; compteurs du groupe 4 pour le point c).
- **Constats :**
  - (a) `compilateur.h:42-44` et Compilateur §9 disent : « aucun HandleObjet n'a été créé ». Pourtant, une chaîne partagée de plus de 16 caractères crée son objet texte avant qu'une erreur ultérieure rejette la ligne. Ce comportement est voulu par IMP-0107 et IMP-0181 ; c'est la formulation qui est fausse.
  - (b) IMP-0194 justifie l'octet réservé par « aucune fonction de l'interface ne renvoie la longueur d'un objet ». Or `objets_longueur` existe (`objets.c:293-310`).
  - (c) IMP-0243 et les mesures du 2026-10-25 attribuent deux couples `HLock`/`HUnlock` à `objets_resoudre` + `objets_relacher`. Le code en fait trois (`objets_relacher` contrôle aussi l'existence).
- **Proposition :** corriger les trois textes, et mentionner `objets_longueur` au §11 du Gestionnaire d'objets.
- **Suite donnée :** corrigé le 2026-10-30, sans décision IMP : (a) contrat de `compilateur_compiler` réécrit dans `compilateur.h` et Compilateur §5, §9, §17 : aucun objet compilé ni bloc transitoire, mais une constante partagée inerte peut subsister (IMP-0181) ; (b) IMP-0194 mentionne `objets_longueur`, ajoutée à l'interface du Gestionnaire d'objets §11 ; (c) IMP-0243 compte trois couples `HLock`/`HUnlock`.
- **Décision IMP nécessaire :** révisions rédactionnelles d'IMP-0194 et d'IMP-0243.

#### AC-32 — Tables des symboles et des constantes : plus de 64 Kio au-delà de 6 553 et de 21 845 entrées
- **Statut :** CONFIRMÉ (lecture ; manuel ORCA/C 2.2, `memorymodel`). Relevé le 2026-10-30, pendant la correction d'AC-17.
- **Emplacement :** `src/noyau/symboles.c` (`realloc` de `table_symboles` et de `table_constantes`) ; `src/noyau/symboles.h` (`SYMBOLES_CAPACITE_MAX` de 65 535 pour les deux tables).
- **Constat :**
  - En modèle small, ORCA/C suppose qu'aucun tableau obtenu par `malloc` ne dépasse 64 Kio.
  - La table des symboles (entrées de 10 octets) dépasse 64 Kio à partir de 6 554 entrées ; la table des constantes (3 octets), à partir de 21 846.
  - IMP-0104 et IMP-0191 annonçaient 65 535 entrées. Au-delà des seuils, `table_symboles[i]` est adressée hors de la table : même cas qu'AC-06 et AC-07.
- **Scénario :** une session qui introduit 6 554 noms distincts, en comptant ceux des lignes remplacées et du mode machine de bureau, que la table conserve.
- **Référence :** RCI-M IMP-0104 ; Compilateur IMP-0191, point 6 ; État d'exécution IMP-0145, IMP-0179 (même règle).
- **Proposition :** plafonner chaque table à `65535 / sizeof(entrée)` entrées, avec la nature C 108 « capacité des tables atteinte » ; refuser par F 101 un fichier-programme qui dépasse ces plafonds.
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric) : `SYMBOLES_CAPACITE_MAX` vaut 6 553 et `CONSTANTES_CAPACITE_MAX` 21 845 ; complément d'IMP-0104, IMP-0191 et la fiche de codage mis à jour ; `verifier` refuse un fichier au-delà (AC-17). Contrôle : `composants_symboles` remplit la table des symboles jusqu'au plafond par `symboles_ajouter`, puis constate C 108 (`SYMB_ERR_CAPACITE`) à l'entrée suivante. Le plafond des constantes n'est contrôlé que par lecture : il faudrait 21 845 objets.
- **Décision IMP nécessaire :** oui, complément d'IMP-0104.

#### AC-33 — Restitution d'une ligne : la ligne lue déverrouillée à travers des appels SANE
- **Statut :** CONFIRMÉ (lecture). Relevé le 2026-10-30, en instruisant le point 8 du §9. Défaut latent : aucun cas observé, SANE n'allouant rien en pratique.
- **Emplacement :** `src/noyau/decompilateur.c`, `element` (constante partagée : `objets_resoudre` puis `objets_relacher` du texte) et `decompilateur_ligne` (ligne résolue pendant toute la restitution).
- **Constat :**
  - La ligne et le texte d'une constante partagée peuvent occuper le même bloc-maître ; le verrouillage n'est pas compté (Gestionnaire d'objets §11).
  - Le relâchement du texte, et déjà le contrôle d'existence de sa résolution, déverrouillent donc la ligne, dont la restitution continue sur la même adresse.
  - Les réels qui suivent sont écrits par `decompilateur_ecrire_reel`, en `double`, donc par des appels SANE. La *Toolbox Reference* (vol. 1, p. 12-5) ne garantit l'adresse d'un bloc déverrouillé qu'en l'absence de tout appel système ou toolbox.
  - Le §11 affirmait que seuls des `HLock` et des `HUnlock` avaient lieu dans ce cas : c'est inexact.
- **Scénario :** `10 A_'TEXTE DE PLUS DE SEIZE CARACTERES';B_1.5`, décompilée (LISTER, RANGER d'un listage, ENCODER) quand la ligne et la constante partagent un bloc.
- **Référence :** Gestionnaire d'objets §11 (verrouillage non compté), IMP-0243.
- **Proposition et suite donnée :** corrigé le 2026-10-30 (Éric, option b du point 8) : le texte de la constante est copié par `objets_copier` dans un tampon statique ; la ligne reste verrouillée pendant toute la restitution. §11 réécrit.
- **Décision IMP nécessaire :** oui, extension d'IMP-0243.

#### AC-34 — Makefile : dépendances transitives envers `objets.h` absentes
- **Statut :** CONFIRMÉ (essai du 2026-10-30). Relevé en corrigeant le point 8 du §9.
- **Emplacement :** `build/Makefile` et sa reproduction au chapitre Construction §7. Règles sans `objets.h` alors que leur source l'inclut par `rcim.h`, `symboles.h` ou `pile.h` : `moniteur.a`, `interprete.a`, `contexte_exec.a`, `codage.a`, et 22 programmes de tests (`composants_compilateur_*`, `historiques_*`, `composants_decompilateur`, `composants_programme`, `composants_contexte`, `composants_donnees`, `composants_interprete`, `composants_entrees_sorties`, `composants_horloge`, `composants_nombres`, `composants_bibliotheque`, `composants_fichiers`, `composants_codage`, `composants_moniteur`, `plateforme_pile`, `sonde_performance`).
- **Constat :** les règles ne citent que les en-têtes inclus directement. Après le changement de prototype d'`objets_copier` (décalage `unsigned long`), une construction sans `make clean` a gardé l'ancien `interprete.a` : `composants_interprete` et `composants_fichiers` se sont arrêtés sur `BRK 00` à l'adresse 3. Le même défaut vaut sans doute pour d'autres en-têtes inclus indirectement (`rcim.h`, `pile.h`, `erreurs.h`).
- **Scénario :** toute modification d'un en-tête inclus indirectement, suivie de `make` sans `make clean`.
- **Référence :** AC-30 (même nature, pour `profil.h`) ; Construction §7 ; `build/README.md` (`make clean` avant un changement de profil).
- **Proposition :** soit une variable regroupant les en-têtes du noyau, dont dépend chaque objet du noyau et chaque suite ; soit la règle écrite de toujours construire après `make clean`.
- **Décision IMP nécessaire :** non (construction), sauf si la règle de construction change (Construction IMP-0192).
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, option a, avec le témoin du profil), complément d'IMP-0140.
  - Mesure complète avant correction : 31 règles sur 58 manquaient au moins un en-tête inclus indirectement, sur 22 en-têtes (`objets.h` 26 fois, `rcim.h` 25, `erreurs.h` 22, `pile.h` et `symboles.h` 10).
  - Toute règle de compilation dépend désormais de `$(DEPENDANCES)` : tous les en-têtes du dépôt (`$(wildcard …)`, 29 fichiers) et le témoin `build/profil.temoin`, réécrit seulement quand `PROFIL` change. La variable `INTERFACES` disparaît ; `make clean` efface le témoin. Copie du chapitre Construction §7 et `build/README.md` mises à jour.
  - Vérifié : `make clean`, suites en `dev` (30 suites, 1 611 contrôles) ; puis, **sans** `make clean`, suites en `cible` (53 compilations, 30 suites, 1 611 contrôles) et image (`LSE` de 230 525 octets, taille d'une construction `cible`) ; ensuite, aucune compilation si rien ne change, et 24 compilations pour `LSE` si `objets.h` change (`make -n -W`).
  - La consigne « `make clean` après un changement de profil » de `build/README.md` est levée.

#### AC-35 — Pas de FAIRE hors du chemin exact : dépassement de `FLT_MAX` sans EX 44
- **Statut :** CONFIRMÉ (lecture). Relevé le 2026-10-30, en traitant AP-07.
- **Emplacement :** `src/noyau/contexte_exec.c`, `contexte_pas` : `somme = v + pas` quand `donnees_operation_entiere` se récuse.
- **Constat :** la somme est rangée en `float` sans contrôle. Au-delà du plus grand `float`, la variable de boucle devient infinie, alors qu'IMP-0216 fait d'un tel résultat l'erreur EX 44. L'arrondi, lui, est celui d'IMP-0216 (somme en format étendu, arrondie en `float`).
- **Scénario :** `10 FAIRE 20 POUR V_3E38 PAS 1E38 JUSQUA 3.4E38` : à la deuxième itération, V vaut l'infini et la boucle s'arrête sans erreur.
- **Proposition :** calculer la somme par `donnees_operation_etendue('+', …)`, qui rend EX 44 comme toute addition. Changement de comportement dans ce seul cas ; à arbitrer, et à confronter si possible à LSE 8086.
- **Décision IMP nécessaire :** non (application d'IMP-0216).
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric) : la somme passe par `donnees_operation_etendue('+', …)`, qui rend EX 44 ; IMP-0216 le précise. Essai ajouté à `composants_interprete` (EX 44 en ligne 10 pour l'exemple ci-dessus).

---

## 6. Performances

### 6.1 Majeurs

#### AC-36 — GARER dans une étendue reprise entière : lecture au-delà du contenu
- **Statut :** CONFIRMÉ (lecture). Relevé le 2026-10-30, en traitant AP-17.
- **Emplacement :** `src/noyau/fichiers.c`, `garer_ouvert`, étape 1 (écriture du contenu).
- **Constat :** quand une étendue libre est reprise entière, faute d'entrée inutilisée pour la découper (AC-19), `rec.nb` dépasse le besoin. Pour les secteurs au-delà du contenu, `reste = longueur - i × 256` passe sous zéro (`unsigned long`), et 256 octets lus au-delà de la fin de l'image de l'enregistrement sont copiés dans le secteur. Rien n'est visible en L.S.E., CHARGER lisant d'après la longueur ; mais la lecture sort du tampon alloué et les secteurs inutilisés reçoivent des octets quelconques au lieu de zéros.
- **Suite donnée :** corrigé le 2026-10-30 avec AP-17 : ces secteurs sont complétés de zéros, sans lecture du contenu. Non couvert par un essai dédié : la lecture hors du tampon n'est observable ni en L.S.E. ni sous Golden Gate.
- **Décision IMP nécessaire :** non (précision d'IMP-0247).

#### AP-01 — Verrouillages superflus dans `objets_existe`, `objets_creer` et `objets_liberer`
- **Statut :** CONFIRMÉ. Les appels toolbox ont été comptés par instrumentation de `memoire_iigs.c` dans la copie (`g4/essais_g4_sorties.txt`, sections 4, 5 et 11). Les durées en sont déduites, à 1,133 ms mesurée pour 6 appels.
- **Emplacement :** `src/noyau/objets.c:241-243` (`objets_existe`), `:201-222` (balayage de `objets_creer`), `:346-357` (`objets_liberer`) ; appels de contrôle en `:252`, `:316`, `:333`.
- **Constat :**
  - Ces fonctions verrouillent puis déverrouillent le bloc pour lire ou modifier son en-tête, alors qu'aucun appel système ni toolbox n'a lieu entre la lecture du pointeur maître et le dernier accès. C'est exactement la condition d'IMP-0243, déjà retenue pour `objets_copier` et `objets_longueur`.
  - `objets_existe` déverrouille aujourd'hui, au passage, le bloc d'une autre résolution ouverte (Gestionnaire d'objets §11).
- **Estimation** (≈ 0,19 ms par appel toolbox) :

  | Opération | Actuel | Proposé |
  |---|---|---|
  | Résolution + relâchement | 6 appels, 1,13 ms | 2 appels, ≈ 0,4 ms |
  | `objets_creer` | 2(k+1) appels | aucun |
  | `objets_liberer` | 4 appels | aucun, hors `DisposeHandle` d'un bloc vidé |
  | APPELER ou EXECUTER de 200 lignes | 4 040 appels, ≈ 0,77 s | ≈ 800 appels, ≈ 0,15 s |
  | Boucle `C_C!'A'`, par tour | 30 à 36 appels | ≈ 6 appels, soit −4,5 à −5,7 ms (la boucle (b) entière coûte 7,1 ms) |
  | Lecture de `T[I]` | 12 appels | 4 appels, soit ≈ −1,5 ms par accès |
- **Proposition :** lire et écrire l'en-tête par `plateforme_bloc_adresse` dans ces trois fonctions, et garder `HLock`/`HUnlock` dans `objets_resoudre`/`objets_relacher` seuls. Commenter dans chaque fonction l'interdiction d'y introduire un appel toolbox.
  - Option plus large : retirer aussi le verrou de `objets_resoudre`, la règle du §11 suffisant. Gain : ≈ 0,4 ms par résolution.
  - Risque faible : celui d'IMP-0243, un appel toolbox ajouté plus tard. Comportement, formats et `HandleObjet` sont inchangés.
- **Décision IMP nécessaire :** oui, extension d'IMP-0243, révision d'IMP-0159 et du texte des §7 et §11 du Gestionnaire d'objets (§9, point 8).
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, option b du point 8 du §9), extension d'IMP-0243 et révision d'IMP-0159 : `objets_existe`, `objets_creer`, `objets_longueur` et `objets_liberer` lisent l'en-tête par `plateforme_bloc_adresse` ; une résolution et son relâchement font deux appels toolbox au lieu de six. L'option plus large (verrou retiré de `objets_resoudre`) est écartée : des fenêtres de résolution contiennent `malloc` (`decoder_declaration`) ou des appels SANE (restitution d'une ligne, AC-33). Mesuré sur l'Apple IIGS le 2026-10-30 (sonde `PERF`, `Audit_code_2026-10-29_essais/point8/`) : une résolution et son relâchement coûtent 0,467 ms au lieu de 1,133 ms.

#### AP-02 — Accès à `T[I]` : deux paires de verrouillage et un arrondi en `double` par accès
- **Statut :** PLAUSIBLE. Les deux paires de verrouillage sont confirmées par lecture (1,1 ms mesurée chacune). Le coût d'`arrondir` est estimé à partir des coûts unitaires ; il reste à mesurer par `PERF`.
- **Emplacement :** `src/noyau/donnees.c:591-609` (`donnees_rang` → `donnees_tableau_bornes` ; `arrondir` `:31-40` pour chaque indice ; indices passés par valeur) ; `:611-639` (lecture et écriture d'un élément ; `donnees_ecrire_element` reçoit un `float` par valeur).
- **Estimation :**
  - Le coût se décompose en : bornes 1,1 ms, élément 1,1 ms, deux ou trois passages de `float` par valeur (≈ 0,27 ms chacun), et `arrondir` 2 à 4 ms (`float`→`double` 0,883 ms, addition 0,767 ms, deux comparaisons de `double` à 0,45 ms).
  - Soit ≈ 4,5 à 6,5 ms par accès, contre ≈ 0,2 ms pour une variable simple.
  - `FAIRE 20 POUR I_1 JUSQUA 1000` / `20 S_S+T[I]` coûte ainsi 5 à 6 s de plus.
- **Proposition :**
  - **(a)** Chemin exact des indices. Si l'indice est un entier exact sous 2²⁴ (déjà reconnu par `entier_exact`, `donnees.c:729`), on le prend sans `arrondir`, et les indices passent par adresse. Le résultat est identique bit à bit, puisque `arrondir(n) = n`. Gain : 2 à 4 ms.
  - **(b)** Lecture et écriture de l'en-tête et de l'élément sans verrouillage, sous la condition d'IMP-0243. Il faut une primitive `objets_ecrire` symétrique d'`objets_copier`, avec un décalage `unsigned long` pour les tableaux hors norme. Gain : ≈ 1,9 ms.
  - Ensemble, (a) et (b) ramènent un accès de ≈ 5 ms à ≈ 0,6 ms.
  - Risque : faible pour (a) ; pour (b), une discipline à tenir dans `donnees.c`.
- **Décision IMP nécessaire :** oui. (a) : révision d'IMP-0244. (b) : extension d'IMP-0243 (§9, point 8).
- **Suite donnée :** (b) corrigé le 2026-10-30 (choix d'Éric, option b du point 8 du §9), extension d'IMP-0243 : bornes, marques et valeur d'un élément lues et écrites par `objets_copier` et la nouvelle `objets_ecrire` (décalage `unsigned long`) ; plus aucun appel toolbox par accès. (a) corrigé le 2026-10-30 (complément d'IMP-0244) : un indice entier exact de valeur absolue inférieure à 2²⁴ est pris tel quel, sans `arrondir`, les indices passant par adresse (`donnees_rang_exact`) ; même rang, `arrondir(n)` valant `n` ; cas ajoutés à `composants_donnees` (moitié vers le haut, indice négatif, `-0`, grand indice). Gain non mesuré.

#### AP-03 — Écriture d'un réel : puissances de dix recalculées à chaque mise à l'échelle
- **Statut :** CONFIRMÉ pour le nombre d'opérations. Le décompte vient d'une réplique instrumentée de `decompilateur_ecrire_reel_chiffres`, dont la sortie a été vérifiée identique (`g3/audit_g3.out`, lignes `REEL`). La durée est estimée à partir du coût mesuré d'une opération SANE.
- **Emplacement :** `src/noyau/decompilateur.c:366-380` (`puissance_dix`, `echelle`), `:403-412`. Même calcul dans `src/noyau/donnees.c:451-456` et `:491-530` (forme E), non chiffré.
- **Constat :** chaque appel d'`echelle` calcule `puissance_dix(22)` (22 multiplications), puis `puissance_dix(|n|)`, et la recherche de l'exposant appelle `echelle` à chaque pas. Le Décompilateur annonce pourtant « une vingtaine d'opérations flottantes par constante réelle » (IMP-0201).
- **Estimation** (7 chiffres, forme U) :

  | Valeur | Opérations actuelles | Avec une table | Durée actuelle estimée |
  |---|---|---|---|
  | 3 ; 3.14159 | 59 | 2 | 30 à 53 ms |
  | 200 | 108 | — | 54 à 97 ms |
  | 12345 ; 65535 | 161 | 6 | 80 à 145 ms |
  | 1E+10 | 352 | 12 | 0,18 à 0,32 s |
  | 1E+30 ; 1E-30 | ≈ 1 053 | 42 | 0,53 à 0,95 s |

  Un `AFFICHER I` dans une boucle de 200 tours passe ainsi 10 à 20 s à mettre les nombres en forme. LISTER paie le même prix par constante réelle.
- **Proposition :** une table constante `static const double puissances[23]` (10⁰ à 10²²) à la place de `puissance_dix`. Ces puissances sont exactes en `double`, et la fonction actuelle les calcule exactement : l'arithmétique d'`echelle` reste la même, opération pour opération. Gain : 40 à 70 ms par nombre courant, jusqu'à 0,9 s pour les grands exposants.
  - Risque : l'exactitude des 23 constantes converties par ORCA/C. L'essai d'équivalence prévu (égalité bit à bit avec `puissance_dix`, puis comparaison des écritures sur un échantillon) ne s'est pas terminé (§1.3) : **il reste à faire**.
- **Décision IMP nécessaire :** non (la réalisation rejoint l'impact annoncé par IMP-0201).
- **Suite donnée :** corrigé le 2026-10-30, complément d'IMP-0201 : table `puissances[23]` dans `decompilateur.c` (et `puissances_entieres[7]` pour la borne des chiffres) ; `composants_decompilateur` vérifie que ses littéraux égalent bit à bit les produits successifs par 10, donc que chaque mise à l'échelle fait la même opération sur les mêmes valeurs ; suites du décompilateur, de l'aller-retour du corpus et d'AFFICHER inchangées dans leurs résultats. `iix --trace-tools`, écriture à 7 chiffres : 3 et 3.14159, 24 appels SANE au lieu de 145 ; 200, 36 au lieu de 255 ; 12345, 48 au lieu de 373 ; 1E+10, 84 au lieu de 791 ; 1E+30, 214 au lieu de 2 301 ; 1E-30, 222 au lieu de 2 315. La forme E de `donnees.c` procède par divisions successives par 10 : la changer modifierait les résultats, elle n'est pas touchée.

#### AP-04 — Arithmétique non exacte : détour par `double`, conversions et comparaisons redondantes
- **Statut :** PLAUSIBLE (gain estimé à partir des coûts unitaires, à mesurer par `PERF`).
- **Emplacement :** `src/noyau/interprete.c:151-189` (`a = fa; b = fb;` en `double`, puis `pousser_nombre(c, r)`), `:57-61` (`double` par valeur, deux comparaisons, `pile_empiler_nombre` par valeur), `:191-228` (fonctions).
- **Estimation :** ORCA/C calcule en format étendu, et chaque variable `double` impose une conversion à l'écriture et une autre à la lecture. Une addition décimale fait :
  - deux conversions `float`→`double` (0,883 ms chacune) ;
  - deux comparaisons de `double` (0,45 ms chacune) ;
  - plusieurs passages par valeur.

  On peut retirer cinq à sept conversions et deux comparaisons, soit ≈ 2 à 3 ms par opération.
- **Proposition :** pour + − × ÷, des intermédiaires `long double` (format étendu de SANE) arrondis directement en `float`.
  - Le double arrondi est sans effet pour ces quatre opérations, puisque 64 ≥ 2 × 24 + 2.
  - Dépassement : tester l'exposant du `float` obtenu (EX 44), et ne refaire la comparaison exacte qu'au voisinage de `FLT_MAX`.
  - `pow` et les fonctions restent sur le chemin actuel.
  - Risque faible, sous réserve d'une vérification bit à bit sur des paires de décimaux, sur le modèle de `composants_nombres`.
- **Décision IMP nécessaire :** oui, amender la formulation d'IMP-0216 (« calculée en double ») ou d'IMP-0244 (§9, point 9).
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, §9 point 9), IMP-0216 révisée.
  - `donnees_operation_etendue` (`donnees.c`) calcule `+ − × ÷` hors du chemin exact en `long double` et arrondit directement en `float`. Si le `float` obtenu vaut ±`FLT_MAX` ou l'infini, l'opération est refaite en `double`, qui décide du dépassement : juste au-dessus de `FLT_MAX` (par exemple `FLT_MAX` + 2⁷⁴ + 2⁵¹), le `double` donne EX 44 et le format étendu non. La division par zéro est reconnue sur les bits du diviseur. `^` et les fonctions restent en `double`. (Exemple corrigé après l'essai sur l'Apple IIGS : avec `FLT_MAX` + 2⁷⁴ + 2⁵¹, le calcul en `double` de l'Apple IIGS passe lui-même par le format étendu et ne donne pas EX 44 ; `FLT_MAX` + 2⁸⁰ donne EX 44 sur les deux machines alors que le `float` obtenu vaut `FLT_MAX`.)
  - `composants_nombres` confronte bit à bit, erreurs comprises, les deux calculs : 40 valeurs construites par leurs bits, toutes paires, et 20 000 paires tirées au hasard, quatre opérations (86 400 confrontations), plus des cas explicites ; aucun écart.
  - Constat : sous Golden Gate, le `long double` de SANE n'a que 53 bits (`1 + 2⁻⁶⁰ − 1` y vaut 0). La suite y éprouve l'aiguillage et le recalcul, non le double arrondi par 64 bits. Elle est portée sur l'image `cible` (`NOMBRES`) pour être passée sur l'Apple IIGS.
  - Passée sur l'Apple IIGS le 2026-10-30 (`NOMBRES`, `Audit_code_2026-10-29_essais/point9/NOMBRES_2026-10-30.OUT`) : aucun écart entre les deux calculs, ce qui confirme l'équivalence sur le vrai SANE. Deux contrôles explicites échouent (lignes 241 et 246) : écrits d'après Golden Gate, ils attendaient EX 44 pour `±FLT_MAX ± (2⁷⁴ + 2⁵¹)`, cas que l'Apple IIGS tranche sans erreur, par l'ancien calcul comme par le nouveau. Ils sont remplacés par `±FLT_MAX ± 2⁸⁰` ; 2⁸⁰ rejoint les valeurs confrontées.
  - Mesuré sur l'Apple IIGS le 2026-10-30 (sonde `PERF`, les deux calculs dans le même essai, `Audit_code_2026-10-29_essais/point9/`) : `1,5 / 3`, 3,067 ms en `double` et 1,200 ms en format étendu (−61 %) ; `0,1 + 0,2`, 2,533 ms et 1,033 ms (−59 %) ; boucle (e) `X_I/3`, 10,0 ms par tour.

#### AP-05 — Compilation : ≈ 80 % du temps sert à reconnaître des mots
- **Statut :** CONFIRMÉ :
  - cycles comptés par `iix --cycles` et profil par fonction sur 120 lignes du corpus (`g2/profil_k120.txt`) ;
  - variante `AUDIT_OPT2` essayée : les 11 suites concernées passent en profil `cible`, aucune séquence ni aucun diagnostic ne change (`g2/ecarts_copie_g2.patch`).
- **Emplacement :**
  - `src/noyau/compilateur.c:82-93` (`car`, `car_a`, `est_lettre`, `est_chiffre`, un appel de fonction par caractère, chacun avec son contrôle de pile) ;
  - `:113` (`mot_est`, un `strlen` par comparaison), `:131`, `:144` (`mot_reserve`, appelé deux fois pour une cible d'affectation), `:461`, `:317`, `:338` ;
  - `:1608-1700` (jusqu'à 17 essais en série ; AFFICHER est trouvé au 9ᵉ) ;
  - `src/noyau/symboles.c:36-47`.
- **Estimation :**
  - 273 900 cycles par ligne du corpus, soit ≈ 22,8 ms de CPU, ramenés à 127 600 (≈ 10,6 ms, −53 %) ;
  - −49 % avec 150 symboles, −62 % avec 20 ;
  - ENCODER ou RUBAN de 255 lignes : ≈ 5,8 s de CPU, ramenés à ≈ 2,7 s.
- **Proposition :** reprendre `AUDIT_OPT2` en code définitif :
  - macros pour `car`, `car_a`, `est_lettre`, `est_chiffre`, sans effet de bord (aucun argument ne porte de `++`) et sans `p++` sur un pointeur (Construction §5) ;
  - premier caractère comparé avant `strlen` ;
  - `chercher_fonction` limitée aux mots de 2 ou 3 lettres ;
  - comparaison en ligne dans `symboles_chercher`.

  Variante plus profonde, non mesurée : lire le mot une fois dans `instruction_mot_cle`. Risque faible ; une passe en profil `dev` reste à faire.
- **Décision IMP nécessaire :** non (réalisation interne) ; mention souhaitable au Compilateur, « Impact performances » (§9, point 10).
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, §9 point 10), sans les `#ifdef` d'essai : macros `car`, `car_a`, `est_lettre`, `est_chiffre` (aucun argument à effet de bord, vérifié) ; premier caractère comparé d'abord dans `mot_est`, `dans_liste`, `prendre_mot`, `voir_mot` ; `chercher_fonction` limitée aux mots de 2 ou 3 lettres (25 noms, tous de 2 ou 3 lettres) ; comparaison en ligne dans `symboles_chercher`. Avec AP-11, mesuré par `sonde_cycles` (mode `k1`, profil `cible`) : 284 800 cycles par ligne du corpus avant, 127 100 après (−55 %). Compléments d'IMP-0182 et d'IMP-0167. La variante plus profonde (lire le mot une fois dans `instruction_mot_cle`) n'est pas faite.

#### AP-06 — TABLE DES FICHIERS, UTILISATION DISQUE et DUPLIQUER `*` : appels GS/OS quadratiques
- **Statut :** PLAUSIBLE. La complexité est confirmée par lecture (`moniteur.c:903-922`, `:972-980`, `:1034-1058`, `:1153-1162`, `:1298-1303` ; `fichiers_iigs.c:169-204` ; `bibliotheque.c:340-378`). La durée ne l'est pas : aucun appel GS/OS de fichier n'a été mesuré.
- **Emplacement :** `src/noyau/moniteur.c:903-922` (`espace_suivant`) et ses appelants `fichier_suivant` (`:972`) et `dupliquer` (`:1298`).
- **Constat :**
  - Pour obtenir le nom suivant dans l'ordre alphabétique, `espace_suivant` relit tout le répertoire. Chaque lecture fait Open, GetDirEntry et Close.
  - Lister N fichiers coûte donc 3N² appels.
  - Pour les types P et D, `bibliotheque_catalogue_suivant` relit tout le catalogue à chaque nom.
- **Estimation :** 30 temporaires donnent 2 700 appels GS/OS, soit au moins 1,4 s à 0,5 ms par appel (coût mesuré d'un appel de pilote). Les accès au disque s'y ajoutent, et une disquette multiplie le tout.
- **Proposition :** une seule passe, les noms rangés dans un tableau alloué (N × 7 octets) et triés par `strcmp`. Même ordre et même sortie ; on passe de O(N²) à O(N) appels. Risque : une allocation de plus, avec refus propre si elle échoue.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric), complément d'IMP-0249 : nouvelle opération `plateforme_espace_parcourir` (une ouverture du répertoire, un `GetDirEntryGS` par fichier) ; `espace_suivant` s'appuie sur une liste triée des noms valides, refaite au premier pas de chaque parcours, cherchée par dichotomie et rendue à la fin de chaque commande ; si l'allocation échoue, le répertoire est relu à chaque nom, sans erreur de plus. Parcourir N fichiers temporaires coûte N + 2 appels GS/OS au lieu de 3N². Le parcours du `CATALOGUE` pour les types P et D (`bibliotheque_catalogue_suivant`) relit encore le catalogue par blocs de 16 entrées à chaque nom : non changé. Essais : `plateforme_fichiers` (parcours, répertoire absent) ; sorties de TABLE, UTILISATION et DUPLIQUER de `composants_moniteur` identiques.

### 6.2 Mineurs

#### AP-07 — Affectation et pas de FAIRE : `float` passés par valeur
- **Statut :** CONFIRMÉ par lecture. Le coût est déduit des mesures : `contexte_affecter_nombre` 0,333 ms contre `contexte_lire_nombre` 0,067 ms ; un passage par valeur ≈ 0,27 ms.
- **Emplacement :** `src/noyau/interprete.c:127` (`ecrire_reference(…, float v)`) ; `src/noyau/contexte_exec.c:226` (`contexte_affecter_nombre(…, float v)`), `:602` ; `src/noyau/donnees.c:626`.
- **Estimation :** une affectation fait deux passages par valeur, le pas de FAIRE un. Cela fait ≈ 0,8 ms par tour des boucles (b) et (d), soit −11 % et −9 %. Programme `LSE`, boucle (b) de 1 000 tours : ≈ 7 s ramenées à ≈ 6,2 s.
- **Proposition :** versions par adresse (`const float *`), sur le modèle de `pile_empiler_nombre_exact`. Risque nul : mêmes bits.
- **Décision IMP nécessaire :** révision mineure d'IMP-0244.
- **Suite donnée :** corrigé le 2026-10-30, complément d'IMP-0244 : `contexte_affecter_nombre_exact` et `donnees_ecrire_element_exact` (par adresse ; les formes par valeur y renvoient) ; `ecrire_reference` reçoit l'adresse du nombre ; le pas de FAIRE range la somme par adresse. Gain non mesuré.

#### AP-08 — Chaînes : une paire de verrouillage pour lire une longueur et pour chaque opérande
- **Statut :** CONFIRMÉ par lecture ; coût déduit de 1,1 ms par paire.
- **Emplacement :** `src/noyau/donnees.c:140-151` (`donnees_objet_vers_chaine` résout l'objet pour lire 3 octets) ; `:112-128`, `:196-213`, `:215-228`.
- **Estimation :** `C_C!'X'` coûte ≈ 5 à 7 ms, balayage d'`objets_creer` non compris (AP-01).
- **Proposition :** lire l'en-tête par `objets_copier(objet, 0, tete, 3)` (0,133 ms), et les textes sources par le pointeur maître sous la condition d'IMP-0243. Gain : ≈ 1 ms par opérande.
- **Décision IMP nécessaire :** oui, même extension d'IMP-0243 qu'AP-02 (b).
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, option b du point 8 du §9), extension d'IMP-0243 : en-tête lu par `objets_copier` (`donnees_objet_vers_chaine`) ; textes sources copiés par `objets_copier` dans `extraire`, `donnees_texte` et `donnees_concatener`, l'objet destination restant résolu.

#### AP-09 — Appel de procédure : verrouillage de la ligne d'entrée et six allocations
- **Statut :** CONFIRMÉ par lecture pour le verrouillage (1,1 ms). Le coût de `malloc` d'ORCA/C n'a pas été mesuré.
- **Emplacement :** `src/noyau/contexte_exec.c:715-718` (résolution de la ligne pour sauter l'élément `0xCD`) ; `:724-727` et `:332-346` (trois `malloc` pour le cadre, trois autres libérés aussitôt).
- **Proposition :**
  - `objets_copier` des premiers octets, puis `rcim_longueur_element` sur la copie (−1 ms par appel) ;
  - tableaux locaux bornés pour `vals`, `repris` et `neuves`, l'arité étant bornée par une ligne de déclaration.
- **Décision IMP nécessaire :** non pour le verrouillage ; à voir avec IMP-0150 pour les allocations.
- **Suite donnée (allocations) :** corrigé le 2026-10-30 (choix d'Éric), complément d'IMP-0150 : `vals`, `repris` et `neuves` sont des tableaux statiques de 125 entrées (borne tirée de la ligne de 255 octets), au lieu de trois `malloc` par appel ; le cadre reste alloué. Gain non mesuré (coût de `malloc` d'ORCA/C non mesuré).
- **Suite donnée :** verrouillage corrigé le 2026-10-30 (choix d'Éric, option b du point 8 du §9), extension d'IMP-0243 : la ligne d'entrée est copiée par `objets_copier` dans un tampon statique de 255 octets, puis `rcim_longueur_element` y mesure l'élément `0xCD`. Les six allocations restent ouvertes (IMP-0150).

#### AP-10 — Constante immédiate convertie à chaque exécution ; ligne recherchée deux fois
- **Statut :** CONFIRMÉ (lecture).
- **Emplacement :** `src/noyau/interprete.c:1041-1042` (`donnees_nombre_entier`, 0,167 ms par constante immédiate) ; `interprete.c:1215` et `contexte_exec.c:500` (`programme_suivante` ou `contexte_brancher`, puis `programme_chercher` sur la même ligne, 0,067 ms).
- **Proposition :**
  - une table statique des 57 `float` de −28 à 28 (≈ −0,13 ms par constante immédiate) ;
  - transmettre l'entrée trouvée à `charger_ligne`.

  Gains inférieurs à 5 % sur les boucles de référence.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** le 2026-10-30, premier volet corrigé : les 57 constantes immédiates sont converties une seule fois, par `donnees_nombre_entier`, aux mêmes bits (`pousser_immediat`, complément d'IMP-0244). Second volet non fait : la première recherche de la ligne a lieu dans `contexte_exec.c` ou `programme.c`, et transmettre l'entrée trouvée demanderait d'étendre l'interface du contexte, ou un cache dans `programme.c` à invalider à chaque modification du programme, pour environ 0,05 ms par changement de ligne. Écarté le 2026-10-30 (choix d'Éric).

#### AP-11 — Compilation des constantes numériques : ≈ 24 appels SANE par constante entière
- **Statut :** PLAUSIBLE. Le nombre d'appels est confirmé par `iix --trace-tools` : 23 appels FP816 et 1 Str2Dec pour `1 A_1`. La durée sur la machine ne l'est pas, Golden Gate ne comptant pas SANE.
- **Emplacement :** `src/noyau/compilateur.c:191-238` (`ecrire_nombre`, `constante_numerique`, `strtod`), `:724-749`, `:1013-1033`.
- **Constat :** 1 322 des 1 337 constantes du corpus sont des entiers d'au plus 32 767 sans point ni exposant. Toutes passent par `strtod` et une dizaine d'opérations flottantes, alors que le sous-format retenu est entier.
- **Estimation :** 1,47 constante par ligne × 24 appels × 0,2 à 0,3 ms ≈ 7 à 10 ms par ligne, soit ≈ 2 à 2,5 s sur ENCODER de 255 lignes.
- **Proposition :** un chemin entier dans `constante_numerique` : accumulation en `long` et `rcim_ecrire_entier`, avec la valeur entière gardée dans `InfoExpression` pour les replis. Les séquences sont identiques par construction. Risque faible à modéré (repli de `-0`, valeurs limites), couvert par les suites 2a à 2d.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, §9 point 10) : une constante de cinq chiffres au plus, sans point ni exposant, d'au plus 32 767, est accumulée en `long` et écrite par `rcim_ecrire_entier`, sous-format que choisirait `ecrire_nombre` ; `info->valeur` reçoit `(double) n` pour le repli d'une constante négative. `iix --trace-tools` : compiler `1 A_1` fait 2 appels SANE (FL2X, FX2D) au lieu de 23 FP816 et un Str2Dec. Durée sur l'Apple IIGS non mesurée.

#### AP-12 — APPELER : reconstruction quadratique des symboles et des constantes
- **Statut :** CONFIRMÉ (cycles comptés par le mode `rN` de `g2/sonde_cycles.c`, qui reproduit la boucle de `bibliotheque.c:722-727`).
- **Emplacement :** `src/noyau/symboles.c:41-45` (recherche linéaire), `:67-71` (un `realloc` par symbole), `:132-136`, `:143`, `:105-117` (`meme_texte`, une résolution de 1,1 ms par constante de même longueur) ; appelant `src/noyau/bibliotheque.c:722-731`.
- **Estimation :**

  | Symboles | Code actuel | Avec la comparaison en ligne et l'agrandissement par tranches de 32 |
  |---|---|---|
  | 100 | 0,29 s | — |
  | 200 | 1,14 s | 0,36 s |
  | 400 | 4,5 s | 1,2 s |

  Les constantes partagées coûtent en outre O(nc²/33) résolutions, de l'ordre de 16 s pour 1 000 constantes, estimation du groupe 4.
- **Proposition :**
  - comparaison en ligne (AP-05) ;
  - capacité tenue à part et agrandie par tranches ;
  - pour APPELER, éventuellement, insertion sans recherche après une vérification d'unicité faite par `verifier` (AC-17) ;
  - `objets_copier` dans `meme_texte`.
- **Décision IMP nécessaire :** non pour les deux premières mesures ; révision mineure d'IMP-0246 si la détection des doublons quitte `reconstruire` (§9, point 11).
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, §9 point 11). La vérification garantissant l'unicité (AC-17), `reconstruire` ajoute symboles et constantes en fin de table sans recherche (`symboles_ajouter`, `constantes_ajouter`) ; les deux tables grandissent par tranches de 32 entrées au lieu d'un `realloc` par entrée, ce qui profite aussi à la compilation. Compléments d'IMP-0246 et d'IMP-0167. La comparaison en ligne et `objets_copier` dans `meme_texte` relèvent d'AP-05 et du point 8 du §9 : ils ne servent plus à APPELER. Gain non mesuré sur l'Apple IIGS ; la reconstruction est désormais linéaire.

#### AP-13 — Chaque ligne insérée est verrouillée pour lire son premier octet
- **Statut :** CONFIRMÉ (lecture de `programme.c:81-86`).
- **Emplacement :** `src/noyau/programme.c:82` (`decoder_declaration`).
- **Estimation :** 1,1 ms par ligne, soit 0,27 s sur APPELER ou ENCODER de 250 lignes.
- **Proposition :** `objets_copier(l->bloc, 0, &tete, 1)` (0,133 ms), et ne résoudre le bloc que pour un élément `RCIM_CTL_PROCEDURE`. Risque nul.
- **Décision IMP nécessaire :** oui, extension d'IMP-0243, qui réserve aujourd'hui `objets_copier` à l'interpréteur (§9, point 8).
- **Suite donnée :** corrigé le 2026-10-30 (choix d'Éric, option b du point 8 du §9), extension d'IMP-0243 : premier octet lu par `objets_copier` ; le bloc n'est résolu, et donc verrouillé à travers les deux `malloc`, que pour une déclaration de procédure.

#### AP-14 — `CATALOGUE` lu entrée par entrée, en deux passes à l'inscription
- **Statut :** PLAUSIBLE (appels comptés ; durée d'un `fseek` + `fread` de 16 octets non mesurée).
- **Emplacement :** `src/noyau/bibliotheque.c:211-228` (`catalogue_chercher`), `:267-268` (`inscrire`).
- **Estimation :** avec 50 entrées, 150 lectures par RANGER et 50 par GARER ou CHARGER sur un fichier permanent, y compris dans une boucle L.S.E. Par blocs de 16 entrées : 10 et 4.
- **Proposition :** lire par blocs de 256 octets, comme `bibliotheque_catalogue_suivant` (`:358-360`), et chercher le nom et la première entrée libre en une seule passe. Format inchangé.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** corrigé le 2026-10-30, complément d'IMP-0248 : `catalogue_chercher` lit par blocs de 16 entrées et rend, dans la même passe, la première entrée libre ; `inscrire` ne fait plus qu'une passe. Suites des fichiers, de la bibliothèque et du moniteur passées.

#### AP-15 — Le quota temporaire relit tout l'espace temporaire à chaque GARER
- **Statut :** PLAUSIBLE. Les comptes sont confirmés (compteurs du groupe 4, trace `iix --trace-gsos` du groupe 5) ; la durée ne l'est pas.
- **Emplacement :** `src/noyau/fichiers.c:235-250` (`fichiers_secteurs_temporaires`), appelé en `:302` et `:349` ; `src/plateforme/fichiers_iigs.c:133-137`, `:169-204`.
- **Constat :** chaque GARER sur un fichier temporaire fait 13k + 3 appels GS/OS pour le quota, avec k fichiers temporaires, même quand l'écriture n'agrandit rien.
- **Estimation :** avec k = 4, 55 appels sur 78 servent au quota (`g5/trace_garer_4temporaires.txt`). DECODER de 200 lignes : 25 parcours de répertoire.
- **Proposition :**
  - une opération de plateforme qui totalise les secteurs d'un espace en une seule passe (Open, GetDirEntry séquentiel avec `eof`, Close), soit k + 3 appels : le GARER passe de 78 à 30 appels. Il faudra filtrer les fichiers non ordinaires.
  - et/ou ne compter que si l'écriture agrandit le fichier (§9, point 12).
- **Décision IMP nécessaire :** oui, révision d'IMP-0249 (nouvelle opération d'interface), et précision d'IMP-0253 pour la seconde variante.
- **Suite donnée :** corrigé le 2026-10-30, choix d'Éric : complément d'IMP-0253. Le quota n'est contrôlé que si l'écriture agrandit le fichier, ce qui épargne aussi les appels GS/OS du comptage dans ce cas. Contrôle dans `composants_fichiers` : avec un quota d'un secteur, un GARER qui réutilise une étendue libre est accepté. Le comptage lui-même, en une passe de répertoire (révision d'IMP-0249), reste à faire avec les autres points de performance.

#### AP-16 — DECODER rouvre et reparcourt le fichier pour chaque enregistrement
- **Statut :** PLAUSIBLE (appels comptés, durée non mesurée).
- **Emplacement :** `src/noyau/codage.c:31-36` (`emettre` → `fichiers_garer`).
- **Estimation :** DECODER de 200 lignes (12 enregistrements) fait 25 ouvertures, 47 lectures, 37 écritures avec `fflush` et 25 parcours de répertoire.
- **Proposition :** ouvrir une fois, contrôler le quota une fois, et écrire le répertoire à la fin. Plus simplement, se contenter d'AP-15.
- **Décision IMP nécessaire :** non.
- **Suite donnée :** clos sans changement le 2026-10-30 (choix d'Éric) : AP-15 a retiré le contrôle de quota des écritures qui n'agrandissent rien ; à rouvrir si une mesure sur l'Apple IIGS montre DECODER trop lent.

#### AP-17 — GARER écrit secteur par secteur avec `fflush` ; stdio coûte 3 appels GS/OS par secteur
- **Statut :** PLAUSIBLE (comptes tirés de la trace GS/OS ; durée non mesurée).
- **Emplacement :** `src/noyau/fichiers.c:305-310` ; `src/plateforme/fichiers_iigs.c:117-142` (chaque `fseek` émet GetEOF puis SetMark ; la taille d'un fichier par son nom coûte 10 appels).
- **Estimation :** un tableau de 80 Ko donne 313 écritures de 256 octets, chacune avec `fflush`. Sur les 23 appels propres d'un GARER, environ 7 GetEOF sont superflus.
- **Proposition :**
  - écrire par blocs de 4 096 octets en ne complétant que le dernier secteur, comme `charger_ouvert` lit par 4 096 ;
  - plus tard, éventuellement, des appels GS/OS directs et GetFileInfo, ce qui reviendrait sur le choix de stdio (F2, IMP-0249).
- **Décision IMP nécessaire :** non pour l'écriture par blocs ; oui (IMP-0249) pour l'abandon de stdio.
- **Suite donnée :** le 2026-10-30 (choix d'Éric), mesure d'abord : la sonde `PERF` chronomètre un `TABLEAU T[20000]` seul (f0), le même suivi d'un GARER de 80 Ko (f) et un CHARGER (g), dans une bibliothèque `9:BIBPERF` à côté de la sonde ; le choix entre l'écriture par blocs et l'abandon de stdio se fera sur cette mesure. Mesuré sur l'Apple IIGS le 2026-10-30 à 23 h 18, depuis la disquette `/LSE/` (`Audit_code_2026-10-29_essais/point17/`) : tableau seul 17 ms ; GARER de 80 Ko 10,2 s ; CHARGER 5,4 s, CHARGER lisant déjà par blocs de 4 096 octets. Choix d'Éric du 2026-10-30 : écriture par blocs (option a), complément d'IMP-0247 ; le contenu part en écritures de 16 secteurs au plus, directement depuis l'image de l'enregistrement (313 écritures avec `fflush` ramenées à une vingtaine pour 80 Ko). L'abandon de stdio n'est pas retenu. Mesuré après le changement : depuis la disquette (2026-10-30, 23 h 52, `PERF_2026-10-31.OUT`), GARER de 80 Ko 9,0 s au lieu de 10,2 s (−12 %), CHARGER 5,4 s ; depuis le disque dur (2026-10-31, 0 h 23, `PERF_2026-10-31_disque_dur.OUT`), GARER 1,48 s, CHARGER 0,82 s. Sur la disquette le support domine ; sur le disque dur, GARER prend encore 1,8 fois le temps de CHARGER. Remplacements de même taille, depuis le disque dur (2026-10-31, 0 h 30, `PERF_2026-10-31_remplacement.OUT`) : avec agrandissement 1,17 s, sans agrandir 1,10 s, CHARGER 0,83 s ; l'allocation ne coûte qu'environ 70 ms, et l'écart restant tient à l'écriture sur le disque, la stdio n'y ajoutant qu'environ 20 ms (un repositionnement par écriture de 4 Ko). Clos le 2026-10-31 : l'abandon de stdio n'est pas retenu, faute de gain.
- **Lien :** l'abandon de stdio lèverait aussi la limite laissée par la correction d'AC-29 (le 2026-10-30) : tout échec d'écriture y vaut support plein, `fwrite` posant `EIO` pour le code `$48` de GS/OS comme pour les autres.

---

## 7. Observation de méthode

Les tests passent sous Golden Gate avec une pile de 16 Kio et sans le coût réel de SANE ni de GS/OS. Plusieurs défauts de cet audit ne pouvaient donc pas apparaître dans `make tests` :
- tous les seuils de 64 Kio (AC-05, AC-06, AC-07, AC-16), qu'aucune suite n'atteint ;
- la pile (AC-01, AC-11).

Les suites de non-régression proposées en AC-01 et AC-11 et le plafonnement d'AC-06 et AC-07 comblent une partie de ce manque.

---

## 8. Points écartés après vérification

| Point examiné | Raison de l'écarter |
|---|---|
| Deux objets d'un même bloc résolus en même temps (`donnees.c` : `extraire`, `donnees_concatener`, `donnees_comparer`… ; `decompilateur.c:707-711`) | Aucun appel système entre l'obtention de l'adresse et son dernier usage ; conforme à la règle du Gestionnaire d'objets §11. |
| Adressage des tableaux hors norme de plus de 64 Kio dans `donnees.c` (`:621`, `:636`, `:653`) | Correct, le décalage est calculé en `unsigned long`. Essai D du groupe 1 : 65 535 éléments écrits et relus sans faute. |
| Tampons du compilateur : tampon de génération (C 107), `texte[79]`, `tampon[80]`, `specs[40]`, `parametres[32]` | Toutes les écritures sont précédées d'un contrôle ; une ligne de 79 caractères produit au plus ≈ 160 octets. |
| `rcim_longueur_element` | Toute charge utile variable est lue par un accès borné ; aucune lecture hors de la séquence. |
| `objets_copier` | `debut + n` est calculé en `unsigned long`. |
| Bornes de `table_blocs`, `table_hors_norme` et `slots` | Contrôlées partout. |
| Comptabilité de la compaction | Sans double compte (IMP-0160). |
| Fichiers GS/OS non refermés, `refNum` réutilisé | Aucun cas. |
| Blocs de paramètres GS/OS (`DAccessRecGS`, `DIORecGS`, `DevNumRecGS`, `CreateRecGS`, `OpenRecGS`, `DirEntryRecGS`, `ExpandPathRecGS`, `DInfoRecGS`, `VolumeRecGS`) | Conformes (`pCount`, tailles, mot de taille des tampons). |
| `memoire_iigs.c` | Conforme (IMP-0270, IMP-0271, vérifié sur la machine le 2026-10-29). |
| Fichiers fermés et tampons libérés sur les chemins d'erreur de RANGER, APPELER, DUPLIQUER, ENCODER, PERFORER (ESC compris) | Vérifié. |
| Consultation d'ESC par ligne | 0,0167 ms, soit 0,24 % d'un tour de la boucle (b) ; déjà optimisée (IMP-0242). |
| Écriture console | Par bloc et non par caractère. Le CR est écrit deux fois par passage à la ligne (CR puis CR LF, `moniteur.c:75-80`, `interprete.c:441-446`), soit un `DWrite` de trop par ligne, coût non mesuré et imperceptible ; la fusion demanderait de réviser IMP-0220. |
| `programme_chercher` | Dichotomie O(log n) : 8 itérations sur 255 lignes ; écart négligeable avec la mesure faite sur 3 lignes. |
| Interpréteur et décompilateur | Pas de récursion C (mesure `&FACT(25)`). |
| Contrôle de pile (`debug 1`), 14,4 % du temps de compilation | À conserver : c'est lui qui transforme la corruption d'AC-01 en arrêt propre. |
| `2ET` refusé alors que `2OU` est accepté | Conforme à LSE15 §3-1.1. |
| ENCODER sans ligne ajoutée mettant le programme à l'état « aucun » | Conforme à IMP-0261 ; la règle elle-même est soumise au §9, point 13. |
| NaN ou infini sur la pile d'évaluation | Impossible : les cas de domaine sont contrôlés. |
| Débordement de `t[300]` et `corps[320]` | Impossible : `e` et `d` sont bornés à 99 par le compilateur. |
| `nom_de_fichier` tronqué | Rend EX 53. |
| Boucle de `fichiers_cataloguer` sans garde de cycle ; longueur d'enregistrement non comparée dans `charger_ouvert` | Seul un fichier déjà corrompu est concerné ; spéculatif. |
| Remplacement d'une ligne pendant une suspension | Limite documentée (État d'exécution §12 ; Programme courant IMP-0207). |

**Piste signalée, non instruite :** PERFORER fait plusieurs appels GS/OS par ligne perforée : une taille et deux écritures suivies de `fflush` (`src/noyau/ruban.c:111-121`).

---

## 9. Points à arbitrer

1. **Pile du 65816** (AC-01, AC-11, AC-12).
   - **Mesure sur l'Apple IIGS, faite le 2026-10-29 :** 6 niveaux passent ; le débordement survient dès 7 niveaux (`?(((((((1)))))))`), un de moins que sous Golden Gate avec 4 Kio imposés. Le défaut est confirmé sur la cible, avec une pile plus petite que prévu. L'image `build/PILE.po` (§3) a établi que le chargeur accepte 16 Kio et que le pire cas passe. Pile disponible pour LSE sous `main` : 3 555 octets avec la pile du shell, 15 840 avec 16 Kio. **Tranché le 2026-10-29 :** option (a), IMP-0272. La conduite au débordement (AC-12, IMP-0273) et la suite de non-régression de pile (AC-11, point 2, complément d'IMP-0272) ont été tranchées le 2026-10-30.
   - Puis choisir entre trois options :
     - (a) `#pragma stacksize 16384`, recommandé ; vérifier que le chargeur l'accepte sous le shell ORCA ;
     - (b) 12 Kio et des tampons `static`, contre la justification d'IMP-0165 pour `corps_afficher` ;
     - (c) 8 Kio et une borne d'imbrication avec C 107, qui refuserait des lignes valides.
   - Choisir aussi la conduite au débordement : `SystemError` remplacée pour rendre la console, ou reprise au moniteur.
2. **Suite de non-régression de pile** dans `make tests`, avec la `stacksize` de la cible (AC-11).
3. **ESC et PAS A PAS dans une boucle FAIRE d'une ligne** (AC-03). Consulter ESC à chaque reprise d'itération, avec `PRET EN LIGNE` en milieu de ligne ? Le pas à pas doit-il s'arrêter à chaque retour sur la ligne FAIRE ? Essais proposés sur LSE 8086, en PAS A PAS : `10 FAIRE 20 POUR I_1 JUSQUA 3` / `20 X_I`, puis `10 FAIRE 10 POUR I_1 JUSQUA 3;X_I`. **Résultat sur LSE 8086** (Éric, 2026-10-30) : en PAS A PAS, l'exécution s'arrête à la ligne 10, ligne de début de boucle, à chaque itération, sur une ligne comme sur deux ; une boucle infinie d'une ligne est interrompue par ESC. **Tranché le 2026-10-30 :** révision d'IMP-0232 et d'IMP-0239 (AC-03).
4. **Plafond de la pile d'évaluation et des cases** (AC-06, AC-07). EX 03 à 9 362 éléments et 13 107 cases, ou adressage en `unsigned long` sans plafond, plus lent ? Essai proposé sur LSE 8086 : `10 X_&F(20000)` / `30 PROCEDURE &F(N) LOCAL N` / `40 SI N=0 ALORS RESULTAT 0` / `50 RESULTAT 1+&F(N-1)` (profondeur atteinte et message). **Résultat sur LSE 8086** (Éric, 2026-10-30) : le programme s'arrête sur « erreur d'exécution 3 Mémoire pleine ». C'est une erreur d'exécution propre, de même numéro que notre EX 03 « MEMOIRE SATUREE ». **Tranché le 2026-10-30 :** plafonds et EX 03, révision d'IMP-0145 et d'IMP-0179 (AC-06, AC-07).
5. **Description d'AFFICHER à travers un appel** (AC-09). Résultat attendu de `AFFICHER[2F6.2] &F(1),2` quand F exécute `AFFICHER['*']` : `*     1.00     2.00` ? Essai proposé sur LSE 8086. **Résultats sur LSE 8086** (Éric, 2026-10-30) :
   - premier essai : `*     2.00     2.00`, première valeur inexpliquée ;
   - `AFFICHER[2F6.2] 1,2` : `     1.00     2.00` ;
   - `AFFICHER[2F6.2] &G(1),2`, G sans AFFICHER : `     1.00     2.00` ;
   - `AFFICHER[2F6.2] &F(1),3`, F exécutant `AFFICHER['*']` : `*     1.00     3.00`.

   La description survit à l'appel ; l'astérisque sort pendant l'évaluation de la fonction ; les valeurs sont justes. Le premier essai venait donc d'une autre saisie. La largeur de 9 colonnes pour `F6.2` est celle de Données §4 : pas de divergence. **Tranché le 2026-10-30 :** révision d'IMP-0221 et d'IMP-0146 (AC-09).
6. **LIRE et machine de bureau** (AC-13, AC-14).
   - Une ligne de bureau part-elle d'un état de lecture vierge, le programme retrouvant le sien à CONTINUER ? Ou faut-il refuser LIRE pendant une lecture suspendue ?
   - Le reste d'une ligne tapée pour un programme terminé est-il oublié à TERMINER et sur erreur ?
   - Essai proposé sur LSE 8086 : `10 LIRE A` / `20 TERMINER`, réponse `1 2`, puis `LIRE B` en machine de bureau. La valeur 2 est-elle prise sans nouvelle saisie ?
   - **Résultat sur LSE 8086** (Éric, 2026-10-30) : LIRE n'est pas admis en mode machine de bureau. INRDP75 (2ᵉ partie, § 1) et LSE15 §9-5 n'énumèrent d'ailleurs, pour ce mode, que `?`, AFFICHER et l'affectation. **Choix d'Éric du 2026-10-30 :** LIRE reste permis en machine de bureau. MEMENTO82, imprimé, établit que ce mode « permet des instructions complexes comme GARER, CHARGER, LIRE, AFFICHER » (d'après Éric ; passage non transcrit). Aucune restriction attestée seulement par LSE 8086 n'est introduite. AC-13 et AC-14 se corrigent donc par l'état de lecture, non par une interdiction.
7. **DECODER et lignes de plus de 79 caractères** (AC-10). Comportement exact prévu par IMP-0183 : signalement par DECODER, ou ENCODER et ruban compilant avec la borne de 255. Essai proposé sur LSE 8086 : `1 X_1E5+1E5+…+1` (19 termes), LISTER (la ligne apparaît-elle avec `100000` ?), puis DECODER et ENCODER. **Résultat sur LSE 8086** (Éric, 2026-10-30) : les lignes saisies y sont admises jusqu'à 250 caractères ; `1 X_1E5+1E5+…` de 247 caractères donne l'erreur de compilation 4 « Pile interne trop petite » ; DECODER et ENCODER n'existent pas. LSE 8086 ne tranche donc pas ce point. **Choix d'Éric du 2026-10-30 :** voie (a), IMP-0183 appliquée telle qu'elle est écrite : DECODER écrit toutes les lignes et signale chaque ligne restituée de plus de 79 caractères ; ENCODER continue de la refuser (C 63).
8. **Extension d'IMP-0243** (AP-01, AP-02 (b), AP-08, AP-09, AP-13). Principe : accès sans verrouillage partout où aucun appel système ni toolbox n'a lieu entre l'adresse et son usage. Périmètre : `objets.c` seul, ou aussi `donnees.c`, `contexte_exec.c` et `programme.c`. Faut-il retirer aussi le verrou de `objets_resoudre` ? **Tranché le 2026-10-30 (option b) :** en-têtes de bloc lus sans verrou dans `objets.c` ; `objets_copier` et `objets_ecrire` (décalage `unsigned long`) pour les tableaux, les chaînes, la ligne d'entrée d'une procédure, le premier octet d'une ligne insérée et le texte d'une constante restituée (AC-33) ; verrou maintenu dans `objets_resoudre`, certaines fenêtres contenant `malloc` ou des appels SANE. Révisions d'IMP-0243 et d'IMP-0159.
9. **Arithmétique en format étendu** (AP-04). Amender la formulation d'IMP-0216, le résultat restant identique bit à bit pour + − × ÷. **Tranché le 2026-10-30 :** fait ; dépassement décidé en `double` près de `FLT_MAX` ; équivalence à vérifier sur l'Apple IIGS (`NOMBRES`), Golden Gate n'ayant que 53 bits en format étendu.
10. **Optimisations du compilateur** (AP-05, AP-11). Maintenant, ou après la v1 ? Elles divisent environ par trois le temps d'ENCODER ou de RUBAN d'un programme complet, sans changer aucune séquence produite. **Tranché le 2026-10-30 :** maintenant ; AP-05 et AP-11 corrigés, −55 % de cycles par ligne compilée sous Golden Gate, 2 appels SANE au lieu de 24 par constante entière.
11. **RCI-S** (AC-16, AC-17, AP-12). Faut-il conserver les constantes et symboles morts (aucune renumérotation, IMP-0246) ou les purger (EXI-0016, compacité) ? Où placer la détection des doublons : `verifier` ou `reconstruire` ? **Tranché en partie le 2026-10-30 :** doublons détectés par `verifier`, avant de vider le programme (AC-17), puis reconstruction sans recherche (AP-12) ; plafonds des tables (AC-32). **Tranché le 2026-10-30 (Éric) :** les symboles et constantes morts sont conservés, sans purge (complément d'IMP-0246).
12. **Fichiers-données** (AC-19, AP-15). Découper une étendue libre plus grande que le besoin ? Un GARER qui n'agrandit rien est-il refusé quand l'espace temporaire est déjà dépassé (code actuel) ou accepté (lettre d'IMP-0253) ? **Tranché le 2026-10-30 :** découpe quand une entrée inutilisée est disponible dans le même secteur de répertoire (complément d'IMP-0247, AC-19) ; quota contrôlé seulement pour une écriture qui agrandit (complément d'IMP-0253, AP-15).
13. **ENCODER sans ligne ajoutée.** IMP-0261 met le programme à l'état « aucun » dans tous les cas, ce qui est plus strict que LSE15 §9-2 (CONTINUER refusé si une ligne a été entrée). Faut-il conserver cette règle, ou la limiter au remplacement ? **Tranché le 2026-10-30 (Éric) :** état « aucun » seulement si le programme change (remplacement, ou première ligne ajoutée ou remplacée) ; IMP-0261 révisée ; essais dans `composants_moniteur` (`ENCODER EE,90` puis CONTINUER reprend ; `ENCODER EE,6` puis CONTINUER donne `????`).
14. **Lancement et repli stdio** (AC-26, AC-27, AC-28). Que faire d'une option invalide (refus au lancement, message) ? Après F 102, la session continue-t-elle sans bibliothèque ? Une fin de fichier sur l'entrée stdio vaut-elle AU REVOIR implicite ou fin du programme ? **Tranché le 2026-10-30 :** option invalide refusée au lancement avec `OPTION INCORRECTE` (AC-27) ; après F 102, la session continue et toute opération de fichiers rend F 102 (AC-28) ; fin de l'entrée = AU REVOIR implicite (AC-26). Compléments d'IMP-0240, IMP-0250, IMP-0253 et IMP-0268.

**Ordre de traitement suggéré :**
1. La taille de pile (point 1) : tranchée le 2026-10-29 (IMP-0272) ; AC-11 et AC-12 tranchés le 2026-10-30 (complément d'IMP-0272, IMP-0273).
2. Les corrections sans décision IMP et à faible risque : AC-08, AC-02, AC-05, AC-04, AC-16, AC-23.
3. Les points qui demandent un arbitrage ou un essai sur LSE 8086 (points 3 à 7).
4. Les performances (points 8 à 10), une fois la fiabilité rétablie, conformément au conditionnement §27.
