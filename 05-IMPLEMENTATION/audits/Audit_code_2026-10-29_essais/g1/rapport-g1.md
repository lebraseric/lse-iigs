# Audit du code LSE-IIGS — groupe 1 : interpréteur, pile, contexte d'exécution, données

Date : 2026-10-29. Modules audités : `src/noyau/interprete.c/.h`, `src/noyau/pile.c/.h`, `src/noyau/contexte_exec.c/.h`, `src/noyau/donnees.c/.h`.
Chapitres lus : Interpréteur, Données, État d'exécution (§3 bis à §7, §12, §13), Gestionnaire d'objets §3 à §11, Moniteur (IMP-0237 à IMP-0239), Gestionnaire des erreurs §9.8 ; manuel ORCA/C (modèle mémoire, `stacksize`, `debug`) ; mesures du 2026-10-25 et du 2026-10-29.

Aucun fichier du dépôt n'a été modifié. Les essais ont été écrits et exécutés dans la copie de travail :
- suite : `S/copie-g1/tests/composants/composants_audit_g1.c`, règle ajoutée en fin de `S/copie-g1/build/Makefile` (`make tests/composants_audit_g1`) ;
- sorties : `S/essai-g1-dev.out` (profil `dev`, essais A à J) et `S/essai-g1-cible.out` (profil `cible`, essais A à K, résultats identiques sur les points communs).

(S = `/private/tmp/claude-501/-Users-eric-Library-Mobile-Documents-com-apple-CloudDocs-code-lse-iigs/90554eb1-5f0f-4ace-a26d-a8635f85d4be/scratchpad`.)

---

## 1. Carte des chemins chauds

Les coûts unitaires sont ceux de `Mesures_performance_IIGS_2026-10-25.md` (profil R3 = `cible`).

### 1.1 Par ligne exécutée

| Étape | Fonction | Fréquence | Coût mesuré ou déduit |
|---|---|---|---|
| Fin de séquence | `cycle` (`interprete.c:1287`) → `contexte_fin_de_ligne` (`contexte_exec.c:507`) → `fin_de_ligne` (`:489`) → `programme_suivante` | une par ligne achevée | 0,100 ms |
| Reprise d'itération FAIRE | `fin_de_ligne` (`contexte_exec.c:494-498`) | une par tour | négligeable |
| Arrivée sur une ligne | `arrivee` (`interprete.c:1270`) → `charger_ligne` (`:1212`) → `programme_chercher` + `objets_copier` | une par changement de ligne (rien si même ligne) | 0,067 + 0,133 ms |
| Point d'arrêt | `point_d_arret` (`interprete.c:1248`) → `contexte_ligne_arret`, `console_interruption` | une par arrivée **en décalage 0** seulement | ≈ 0,017 ms (lecture de `$C000`, IMP-0242) |
| Branchement | `contexte_brancher` (`contexte_exec.c:513`) → `programme_chercher`, `desimbriquer` | par ALLER EN | 0,067 ms (puis une 2ᵉ recherche dans `charger_ligne`) |

### 1.2 Par élément RCI-M

`cycle` (`interprete.c:1276`) : `rcim_longueur_element` (segment `rcim`), puis `element` (`:1022`), grand `switch`.

| Élément | Chemin | Coût |
|---|---|---|
| valeur immédiate, entier court, entier, n° de ligne | `pousser_entier` (`:64`) → `donnees_nombre_entier` (`donnees.c:677`) → `pile_empiler_nombre_exact` (`pile.c:68`) | 0,167 + 0,133 ms |
| réel | `pile_empiler_nombre` (`pile.c:59`), `float` par valeur | 0,400 ms |
| référence simple | `contexte_resoudre` (`contexte_exec.c:172` : `est_alias`, `symboles_lire`, `assurer`) → `pile_empiler_reference` | 0,133 ms + empilement |
| référence indexée | `nombre` ×1 ou 2, `contexte_resoudre`, `contexte_entree`, `donnees_rang` (`donnees.c:591` → `donnees_tableau_bornes` : **objets_resoudre/relacher 1,1 ms** ; `arrondir` en `double` par indice) | voir G1-P-01 |
| + − × sur entiers exacts | `binaire` (`interprete.c:151`) → `donnees_operation_entiere` (`donnees.c:755`) | 0,567 ms |
| + − × ÷ ^ autres, fonctions | `binaire` / `fonction_1` (`:191`) : `float`→`double`, SANE, `pousser_nombre(double)` (`:57`, deux comparaisons de `double`), `pile_empiler_nombre` par valeur | plusieurs ms (G1-P-03) |
| relation | `relation` (`:232`) : `genre` ×2, `nombre` ×2, `donnees_comparer_nombres` | ≈ 0,1 ms + lectures |
| affectation | `affecter` (`:287`) : `genre`, `nombre`, `ecrire_reference(float)` (`:127`) → `contexte_affecter_nombre(float)` (`contexte_exec.c:226`) | 0,333 ms + un passage `float` de plus (G1-P-02) |
| lecture d'une variable dans un calcul | `pile_valeur_numerique` (`pile.c:113`) → `contexte_lire_nombre` (`contexte_exec.c:198`) | 0,067 ms |
| lecture d'un élément | `pile_valeur_numerique` → `contexte_entree` → `donnees_lire_element` (`donnees.c:611`, **1,1 ms** de verrouillage) | voir G1-P-01 |
| FAIRE : pas | `contexte_pas` (`contexte_exec.c:584`) : `contexte_lire_nombre`, `donnees_operation_entiere`, `contexte_affecter_nombre(float)` | ≈ 0,97 ms |
| FAIRE : test | `contexte_tester` (`:607`) : `pile_valeur_numerique`, `contexte_lire_nombre`, 2 × `donnees_comparer_nombres` | ≈ 0,4 ms |
| APPEL | `contexte_appeler` (`:684`) : `symboles_lire`, `programme_parametres`, `programme_chercher`, **objets_resoudre/relacher de la ligne d'entrée (1,1 ms)**, 6 `malloc`/`free` | voir G1-P-05 |
| chaîne : lecture d'une variable | `chaine` (`interprete.c:109`) → `contexte_lire_chaine` → `donnees_objet_vers_chaine` (`donnees.c:140`, 1,1 ms pour lire la longueur) | voir G1-P-04 |
| chaîne : opération | `donnees_concatener`, `extraire`, etc. : `objets_creer` + une paire résoudre/relâcher par opérande (1,1 ms chacune) | voir G1-P-04 |

### 1.3 Par instruction d'entrée-sortie ou par commande

- AFFICHER : `afficher` (`interprete.c:534`), `afficher_valeur` (`:491`), `ecrire_nombre` (`:473`) → `donnees_ecrire_nombre` (`donnees.c:491`) → `decompilateur_ecrire_reel_chiffres` (U) ou `chiffres_entier`/`chiffres_fixes` (F, E) : une vingtaine d'opérations SANE par nombre, puis un appel `console_ecrire` (DWrite) par morceau. Pour un tableau entier, une paire résoudre/relâcher (1,1 ms) par élément.
- LIRE : `lire` (`:696`), `saisir_nombre` (`:622`, `strtod`), `lire_chaine` (`:669`) ; une fin `FIN_LECTURE` et un retour au moniteur par ligne saisie.
- Commandes EXECUTER, REPRENDRE, CONTINUER (moniteur) → `contexte_initialiser` (`contexte_exec.c:397`, O(nombre de noms)), `contexte_reprendre` (`:429`), `interprete_executer` (`interprete.c:1312`).

Boucle de référence (b) (`FAIRE 20 POUR I_1 JUSQUA 200` / `20 X_1`), 7,1 ms par tour : deux chargements de ligne (0,4), une ligne suivante (0,1), une consultation d'ESC, REF X, valeur 1, affectation (≈ 0,9), pas (≈ 1,0), valeur 200 (0,3), test (≈ 0,4), plus le coût de distribution de sept éléments.

---

## 2. Budget de pile du 65816

Pile par défaut de 4 Kio, sans `#pragma stacksize`, qui accueille aussi la page directe de SANE (manuel ORCA/C, `stacksize`, l. 6442). Le contrôle de débordement à l'entrée de chaque fonction (`#pragma debug` bit 0) est actif dans les deux profils (`profil.h:14-16`).

Variables locales de plus de 64 octets (tailles ORCA/C : `int` 2, `long`/`float` 4, `double` 8, aucun bourrage) :

| Fonction | Locales | Taille |
|---|---|---|
| `interprete_executer` (`interprete.c:1312`) | `Cycle c` (`seq[255]` + 24 octets) | ≈ 280 o |
| `interprete_bureau` (`interprete.c:1331`) | `Cycle c` | ≈ 284 o |
| `ecrire_chaine` (`interprete.c:462`) | `char t[256]` | ≈ 262 o |
| `ecrire_nombre` (`interprete.c:473`) | `char t[300]` | ≈ 302 o |
| `lire_chaine` (`interprete.c:669`) | `texte[258]` | ≈ 272 o |
| `saisir_nombre` (`interprete.c:622`) | `tampon[80]`, un `double` | ≈ 100 o |
| `donnees_ecrire_nombre` (`donnees.c:491`) | `corps[320]`, cinq `double` | ≈ 375 o |
| `chiffres_entier` (`donnees.c:464`) | `t[320]`, un `double` | ≈ 335 o |
| `donnees_cnb` (`donnees.c:375`) | `tampon[80]`, un `double` | ≈ 100 o |

Aucune fonction de `pile.c` ni de `contexte_exec.c` ne dépasse 64 octets (`contexte_appeler` ≈ 60 o).

**Récursion C : aucune** dans le groupe. Le cycle est itératif, et un appel de procédure L.S.E. crée un cadre par `malloc` (IMP-0150) : la profondeur des appels L.S.E. ne consomme pas la pile du 65816.

**Profondeur maximale** dans le groupe : `interprete_executer` → `cycle` → `element` → `afficher` → `afficher_valeur` → `ecrire_nombre` → `donnees_ecrire_nombre` → `chiffres_entier` → `floor` (SANE). Cela fait ≈ 1 410 octets de locales et ≈ 100 octets de cadres (adresses de retour, registre D, paramètres), soit **≈ 1,5 Kio**. Avec les cadres du moniteur au-dessus (petits sur ce chemin : `executer`, `continuer`, la boucle de dialogue, `main`), on reste sous 2 Kio. La marge sur 4 Kio est confortable ; aucun constat n'est retenu.

---

## 3. Constats

### G1-A-01 — ESC n'est jamais vue dans une boucle FAIRE tenue sur une seule ligne
- Gravité : bloquant. Une boucle infinie d'une ligne ne peut pas être interrompue : il faut redémarrer l'Apple IIGS et le programme non rangé est perdu.
- Statut : CONFIRMÉ par l'essai A1 (deux profils) et par la lecture complète du chemin.
- Emplacement : `interprete.c:1253` (`if (pos->decalage != 0) return FIN_AUCUNE;`), `interprete.c:1270-1274`, `contexte_exec.c:494-498`.
- Constat : `point_d_arret` ne consulte `console_interruption` qu'à l'arrivée en décalage 0. La reprise d'itération ramène la position à `(ligne_faire, decalage_reprise)`, décalage non nul. Quand la ligne de fin est la ligne FAIRE elle-même (forme attestée, LSE15 §5-5.5 : `100 FAIRE 100 POUR I←1 JUSQUA N; …`), aucun début de ligne n'est plus jamais atteint pendant la boucle.
- Scénario : `10 FAIRE 10 POUR I_1 JUSQUA 300;X_I` / `20 TERMINER`, ESC frappée dès la 2ᵉ consultation. Obtenu : `FIN_INTERRUPTION` en ligne 20, `X = 300` (boucle achevée). Avec `10 FAIRE 10 TANT QUE 1=1`, l'exécution ne s'arrête jamais.
- Référence : Moniteur IMP-0239 (« l'interpréteur consulte la console au début de chaque ligne ») ; INRDP75 §3-2 ; État d'exécution §7.2.
- Proposition : consulter aussi `console_interruption` à chaque reprise d'itération, quand `fin_de_ligne` ramène la position dans la ligne FAIRE. On rend `FIN_INTERRUPTION` avec la position `(ligne_faire, decalage_reprise)` ; CONTINUER reprend alors la boucle sans perte, puisque le contexte de boucle est intact. `ligne_signalee` doit retenir la position (ligne et décalage), pas seulement la ligne. Coût : 0,017 ms par tour.
- Décision IMP nécessaire : oui, révision d'IMP-0239 (point de consultation) et peut-être d'IMP-0233 (message `PRET EN LIGNE` donné en milieu de ligne).
- Essai : `composants_audit_g1.c`, bloc A ; sortie `[A1 boucle d'une ligne] fin 6, ligne 20, X = 300`.

### G1-A-02 — Une ligne de machine de bureau en erreur laisse des éléments sur la pile ; CONTINUER affecte alors la mauvaise variable
- Gravité : bloquant (corruption silencieuse : une autre variable reçoit la valeur).
- Statut : CONFIRMÉ par l'essai B (deux profils) et par la lecture de `moniteur.c:1535-1564` et `:358-366`.
- Emplacement : `interprete.c:1331-1349` (`interprete_bureau`), `moniteur.c:1553-1563` (`ligne_bureau`, piste hors périmètre), `moniteur.c:358-366` (`continuer`).
- Constat : après `FIN_ERREUR` (ou ESC pendant un LIRE de machine de bureau, `moniteur.c:1556-1558`), les éléments empilés par la ligne de machine de bureau restent sur la pile, au-dessus de l'état du programme suspendu. Or le cadre courant suppose qu'« entre deux instructions, la pile d'un cadre revient toujours à sa base » (État d'exécution §7.3, point B2). Si le programme est suspendu dans une fonction appelée en position d'expression, la valeur rendue par RESULTAT s'empile au-dessus de ces restes, et l'opération appelante consomme un reste à la place de son opérande.
- Scénario : `10 X_&F(2)` / `20 TERMINER` / `30 PROCEDURE &F(A) LOCAL A` / `40 PAUSE` / `50 RESULTAT A*10`. On exécute (PAUSE), on tape `?Z` (Z non défini → EX 62 ; pile 1 → 2), puis CONTINUER. Obtenu : `FIN_TERMINER`, **Z = 20, X non défini**. Le même scénario se produit après ESC dans une fonction (`FIN_INTERRUPTION`), cas courant en mise au point, suivi d'une faute de frappe en machine de bureau.
- Référence : État d'exécution §7.3, §7.4 ; Interpréteur IMP-0214 (« la pile … laissée telle quelle » vaut pour l'observation, non pour la reprise).
- Proposition : `ligne_bureau` mémorise `pile.sommet` avant la boucle et appelle `pile_ramener(&pile, hauteur)` après, quelle que soit la fin (erreur, ESC, fin normale). Variante : `interprete_bureau` le fait elle-même sur `FIN_ERREUR`, mais le cas ESC pendant LIRE relève alors toujours du moniteur.
- Décision IMP nécessaire : non (rétablit l'invariant déjà écrit). Une phrase est à ajouter à IMP-0214 ou au Moniteur.
- Essai : bloc B ; sorties `[B bureau ?Z] fin 3, EX 62 ; pile 1 -> 2` et `[B CONTINUER] fin 0, EX 0, X = -996, Z = 20, pile 1`.

### G1-A-03 — `RESULTAT T[I]` et un élément de tableau passé à un paramètre LOCAL donnent EX 96
- Gravité : bloquant (construction courante du langage refusée par « problème interne »).
- Statut : CONFIRMÉ par l'essai C (deux profils).
- Emplacement : `contexte_exec.c:819` (`resultat_independant` : `if (r->contenu.reference.indexee) return ERR_E_INTERNE; /* élément : étape 5 */`), `contexte_exec.c:661` (`valeur_locale`, même commentaire).
- Constat : deux branches laissées en attente de l'étape 5 (module `donnees`), qui est réalisée depuis, n'ont jamais été complétées. Le compilateur compile `T[N]` en référence indexée (`REF_ELEMENT` sur la pile), parce qu'il ne connaît pas le mode du paramètre formel.
- Scénario : `10 TABLEAU T[3];T[2]_7;X_&F(2)` / `30 PROCEDURE &F(N) LOCAL N;RESULTAT T[N]` → EX 96 en ligne 30. `Y_&G(T[2])` avec `PROCEDURE &G(A) LOCAL A;RESULTAT A+1` → EX 96 en ligne 10.
- Référence : État d'exécution §5.5 (appel, étape 4 : « déréférencement implicite si l'élément est lui-même une REFERENCE » ; retour, étape 2 : « une référence est déréférencée ») ; IMP-0208. Ce n'est pas une limite du §12.
- Proposition : dans les deux fonctions, traiter `REF_ELEMENT` par `contexte_entree` puis `donnees_lire_element`. Pour RESULTAT, un élément sans valeur donne EX 32. Pour un argument LOCAL, un élément sans valeur donne l'état `ID_PARAMETRE_SANS_VALEUR`, par cohérence avec RX3 (§6, point 6).
- Décision IMP nécessaire : non (réalisation de §5.5) ; le traitement de l'élément sans valeur passé en LOCAL est à confirmer.
- Essai : bloc C ; sorties `[C1 RESULTAT T[N]] fin 3, EX 96 ligne 30` et `[C2 &G(T[2]) LOCAL] fin 3, EX 96 ligne 10`.

### G1-A-04 — Pile d'évaluation au-delà de 64 Kio : éléments adressés modulo 64 Kio (modèle mémoire small)
- Gravité : bloquant (corruption silencieuse de la pile).
- Statut : CONFIRMÉ par l'essai I (profils `dev` et `cible`) et par l'essai F2.
- Emplacement : `pile.c:47-55` (agrandissement sans plafond), accès `p->elements[...]` en `pile.c:54, 87, 95, 103`, `interprete.c:308`, `contexte_exec.c:782`.
- Constat : en modèle small, ORCA/C suppose qu'aucun tableau alloué dynamiquement ne dépasse 64 Kio (manuel, `memorymodel`, l. 6333 : « the compiler assumes that you will not allocate any single array … larger than 64K bytes using … malloc »). L'indice `sommet × 7` est calculé sur 16 bits. Au-delà de 9 362 éléments (65 534 octets), l'élément n est écrit à l'emplacement de l'élément (7n − 65 536)/7. La pile double pourtant jusqu'à 16 384 éléments (114 688 octets).
- Scénario : 12 000 nombres empilés puis relus : 2 637 faux, soit exactement les éléments au-delà de 9 362 (essai I). En L.S.E., une fonction récursive en position d'expression (`RESULTAT 1+&F(N-1)` laisse un élément par niveau) atteint ce seuil vers 9 000 niveaux, et un peu plus tôt avec deux éléments par niveau. Essai F2 (15 000 niveaux) : EX 96 au lieu de `X = 15000`.
- Référence : manuel ORCA/C l. 6333, 6749 ; État d'exécution IMP-0145, §12 (« seuil … non figé »).
- Proposition : plafonner la capacité à `65535 / sizeof(ElementPile)` = 9 362 éléments (agrandissement `min(2 × capacite, MAX)`) et rendre EX 03 « mémoire saturée » au-delà (nature attestée, INRDP75 l. 3542). Ce plafond règle aussi le débordement `2 * capacite` → 0 à 32 768 (`pile.c:48`). L'alternative, `#pragma memorymodel 1` ou un adressage explicite en `unsigned long`, pénaliserait l'opération la plus fréquente.
- Décision IMP nécessaire : oui, révision d'IMP-0145 (plafond chiffré) et ajout au §12 d'État d'exécution.
- Essai : bloc I ; sortie `[I pile] 2637 element(s) faux, premier rang 1` ; bloc F ; sortie `[F2 profondeur 15000] fin 3, EX 96`.

### G1-A-05 — Réserve des cases au-delà de 64 Kio : corruption de l'environnement dès qu'elle passe 8 192 cases
- Gravité : bloquant (valeurs de variables fausses, sans erreur).
- Statut : CONFIRMÉ par l'essai J (deux profils).
- Emplacement : `contexte_exec.c:98-110` (`allouer_case` : doublement, puis boucle de chaînage `cases[i - 1]` pour i jusqu'à 16 384), et tous les accès `cases[c]`.
- Constat : même cause que G1-A-04, avec des cases de 5 octets. Au passage de 8 192 à 16 384 cases (81 920 octets), la boucle de chaînage écrit les cases 13 108 à 16 383 modulo 64 Kio, c'est-à-dire sur les cases 0 à ≈ 3 276 déjà en usage (noms globaux, locaux des premiers niveaux). Le seuil est atteint par le nombre de **cases simultanées** : noms, plus un LOCAL actif par niveau et par identificateur. Une procédure récursive à trois LOCAL l'atteint vers 2 700 niveaux.
- Scénario : `10 K_0;&P(12000)` / `30 PROCEDURE &P(N) LOCAL N` / `40 K_K+1;SI N>0 ALORS &P(N-1)` / `50 RETOUR` (pile basse, appels en instruction). Obtenu : `FIN_TERMINER`, **K = 0** au lieu de 12 001. Avec 14 000 appels : EX 05 en ligne 40. L'essai F1 (9 000 niveaux) passe par hasard : les cases écrasées n'y étaient plus relues.
- Référence : manuel ORCA/C l. 6333 ; IMP-0179 (« réserve de cases, agrandie par realloc ») ; IMP-0149.
- Proposition : plafonner `nb_cases` à `65535 / sizeof(EntreeEnvironnement)` = 13 107 (doublement borné), EX 03 au-delà. La garde actuelle `if (n > 65535u)` est sans effet, puisqu'un `unsigned int` ne dépasse pas 65 535. Le tableau `noms` (2 octets par nom) ne pose problème qu'au-delà de 32 767 symboles, ce qui est hors d'atteinte en pratique.
- Décision IMP nécessaire : oui, révision d'IMP-0179 (plafond) et ajout au §12.
- Essai : bloc J ; sorties `[J1 cases, 12000 appels] fin 0, EX 0 ligne 0, K = 0` et `[J2 cases, 14000 appels] fin 3, EX 5 ligne 40, K = 1`.

### G1-A-06 — GARER d'un tableau de plus de 65 535 octets écrit des octets non initialisés dans le fichier
- Gravité : bloquant (fichier-données corrompu sans diagnostic).
- Statut : CONFIRMÉ par l'essai E (deux profils) et par la lecture de `fichiers.c:254-309` (longueur en `unsigned long`, écriture par secteurs).
- Emplacement : `interprete.c:889-891` (`objets_copier(t.contenu.objet, 0, contenu, (unsigned int) lg)`).
- Constat : `lg` (taille de l'objet tableau, jusqu'à 270 338 octets) est tronqué à 16 bits pour la copie. `malloc(lg)` réserve bien la taille entière, et `fichiers_garer(…, lg)` écrit `lg` octets, dont seuls `lg mod 65 536` ont été copiés. La valeur rendue par `objets_copier` n'est pas testée.
- Scénario : un tableau de 15 887 éléments ou plus. Essai : `TABLEAU T[16000];T[1]_5;T[16000]_7`, `GARER T,1,'#GT';CHARGER U,1,'#GT';A_U[1];B_U[16000]` → `FIN_TERMINER`, **A = 0, B = 0** (attendu 5 et 7). Seuls 470 octets sur 66 006 ont été copiés.
- Référence : Bibliothécaire et fichiers IMP-0247, IMP-0253 ; Données IMP-0210 (« au plus 65 535 éléments, environ 270 Kio »).
- Proposition : `o = objets_resoudre(h); memcpy(contenu, o, (size_t) lg); objets_relacher(h);`, permis par la règle du §11 du Gestionnaire d'objets, puisque `malloc` précède et qu'aucun appel n'a lieu entre l'adresse et la copie. Sinon, `objets_copier` avec un décalage `unsigned long`, par tranches. Tester le résultat.
- Décision IMP nécessaire : non.
- Essai : bloc E ; sortie `[E GARER/CHARGER T[16000]] fin 0, EX 0 ligne 0, A = 0, B = 0`.

### G1-A-07 — La description d'AFFICHER est perdue ou écrasée par une fonction appelée dans la liste
- Gravité : bloquant au sens de la grille (affichage L.S.E. faux, sans erreur). Le scénario est courant : une fonction qui affiche une trace, ou le pas à pas.
- Statut : CONFIRMÉ par l'essai K (profil `cible`).
- Emplacement : `interprete.c:44-48` (`aff_type`, `aff_e`, `aff_d`, `aff_reste` dans `Cycle`), `interprete.c:541-556`, `interprete.c:1317` et `1337` (`memset` du `Cycle` à chaque appel).
- Constat : l'état « description en cours » vit dans la structure `Cycle`, locale à `interprete_executer`. (1) Un AFFICHER exécuté dans une fonction appelée depuis la liste d'un AFFICHER remet `aff_reste` à 0 ; au retour, les valeurs restantes sortent en U. (2) Toute suspension dans la fonction (`FIN_PAS`, `FIN_INTERRUPTION`, `FIN_LECTURE`, PAUSE) détruit le `Cycle` ; à la reprise, la description est perdue.
- Scénario : `10 AFFICHER[2F6.2] &F(1),2` avec `30 PROCEDURE &F(A) LOCAL A;AFFICHER['*'];RESULTAT A` → `*1 2 ` au lieu de `*  1.00  2.00`. Même programme sans AFFICHER dans F, en PAS A PAS (F sur deux lignes) → `1 2 ` au lieu de `  1.00  2.00`.
- Référence : Interpréteur IMP-0221 (une description « s'applique aux r valeurs qui la suivent ») ; IMP-0214 (l'état est conservé pour la reprise).
- Proposition : sortir ces quatre octets du `Cycle` (état statique, comme l'état de lecture) et les sauvegarder et restaurer autour d'un appel : champ opaque dans `CadreActivation`, renseigné par l'interpréteur à `RCIM_OP_APPEL` et rendu par `retourner`. On les remet à zéro à EXECUTER et à REPRENDRE.
- Décision IMP nécessaire : oui, révision d'IMP-0221 (et d'IMP-0146 si le champ va dans le cadre).
- Essai : bloc K ; sorties `[K1 AFFICHER imbrique] fin 0, affichage [*1 2 ]` et `[K2 pas a pas] fin 0 apres 3 pas, affichage [1 2 ]`.

### G1-A-08 — État de lecture partagé entre le programme suspendu et la machine de bureau
- Gravité : majeur (reprise après ESC pendant LIRE, chemin d'interruption réaliste).
- Statut : CONFIRMÉ par l'essai G.
- Emplacement : `interprete.c:579-584` (état statique `saisie`, `rang_tableau`), `interprete.c:1184-1187` (`RCIM_CTL_LIRE_TETE` remet `rang_tableau` à 0), `moniteur.c:1553-1560` ; contrat `interprete.h:41-43` (« ligne de machine de bureau » doit appeler `interprete_reinitialiser`, ce que le moniteur ne fait pas).
- Constat : un LIRE de machine de bureau, tapé pendant qu'un `LIRE T` (tableau entier) du programme est interrompu par ESC, remet à zéro le rang du tableau du programme et remplace sa ligne saisie. Inversement, le reste d'une ligne lue par le programme est consommé par un LIRE de machine de bureau.
- Scénario : `10 TABLEAU T[3];LIRE T`, saisie `1`, ESC, ligne de machine de bureau `LIRE Y` (réponse `9`), CONTINUER, saisie `2 3`. Obtenu : T = 2, 3, non défini, et une nouvelle lecture en attente. Attendu (IMP-0237 : « les valeurs déjà lues étant conservées ») : 1, 2, 3.
- Référence : Moniteur IMP-0237 ; Interpréteur IMP-0222 ; `interprete.h:41-43`.
- Proposition : sauvegarder l'état de lecture du programme à l'entrée de `ligne_bureau` et le restaurer à la sortie, la machine de bureau partant d'un état vierge ; corriger le commentaire d'`interprete.h`. Le comportement attendu d'un LIRE de machine de bureau face au reste d'une ligne du programme est à arbitrer (§6).
- Décision IMP nécessaire : oui, précision d'IMP-0222 ou d'IMP-0237.
- Essai : bloc G ; sortie `[G LIRE T repris] fin 7, T = 2 3 -996, Y = 9`.

### G1-A-09 — Écriture F ou E d'un grand nombre : des caractères autres que des chiffres apparaissent
- Gravité : mineur.
- Statut : CONFIRMÉ par l'essai H.
- Emplacement : `donnees.c:464-489` (`chiffres_entier`, `chiffres_fixes` : `'0' + (int) (x - q * 10.0 + 0.5)`).
- Constat : au-delà de 2⁵³, `x − q × 10` n'est plus exact et peut valoir 16 ou −3. La limite écrite au §4 du chapitre Données admet que « les chiffres au-delà du quinzième peuvent différer », mais pas des caractères `@` ou `-` au milieu du nombre.
- Scénario : `F30.2` de `1E20` → `10000000200408772@000.00` ; `F45.0` de `3E38` → `3000000005497755-0000…`.
- Référence : Données IMP-0211 et la limite du §4.
- Proposition : borner le chiffre obtenu à 0…9. Mieux : tirer les chiffres exacts du `float` (mantisse de 24 bits × 2ᵉ), ce qui supprimerait aussi la limite documentée.
- Décision IMP nécessaire : non pour le bornage ; oui (IMP-0211) pour une écriture exacte.
- Essai : bloc H.

### G1-A-10 — CHARGER d'un tableau : l'en-tête lu dans le fichier n'est pas validé
- Gravité : mineur (robustesse face à un fichier endommagé ou d'une autre provenance).
- Statut : CONFIRMÉ par lecture du chemin (`interprete.c:957-965`, `donnees.c:611-656`) ; pas d'essai.
- Emplacement : `interprete.c:957` (seule condition : `lg >= 6`).
- Constat : `d1`, `d2`, le genre et le nombre de dimensions sont repris tels quels. `donnees_lire_element`, `donnees_ecrire_element` et `donnees_remplir` calculent ensuite les décalages à partir de l'en-tête (`nb_elements`), sans les comparer à la taille de l'objet. Un en-tête incohérent avec `lg` fait lire et écrire hors de l'objet, dans les objets voisins du bloc-maître.
- Proposition : exiger `contenu[0] == DONNEES_GENRE_TABLEAU`, `dims` ∈ {1, 2}, `d1, d2 ≥ 1`, `d2 == 1` si `dims == 1`, et `lg == 6 + (n + 7)/8 + 4n` ; sinon `ERR_F_FICHIER_ENDOMMAGE`, nature déjà prévue.
- Décision IMP nécessaire : non.

### G1-P-01 — Accès indexé : deux paires de verrouillage et un arrondi en `double` par accès
- Gravité : majeur (un accès à `T[I]` coûte de l'ordre d'une instruction simple entière ; les programmes sur tableaux, tris et matrices, en sont dominés).
- Statut : CONFIRMÉ par lecture pour les deux paires `objets_resoudre`/`objets_relacher` (1,1 ms mesurée chacune) ; PLAUSIBLE pour le coût d'`arrondir`, estimé à partir des coûts unitaires et à mesurer par `PERF`.
- Emplacement : `donnees.c:591-609` (`donnees_rang` → `donnees_tableau_bornes` : résoudre/relâcher ; `arrondir` `donnees.c:31-40` pour chaque indice ; `i1`, `i2` passés par valeur) ; `donnees.c:611-639` (lecture et écriture d'un élément : résoudre/relâcher ; `donnees_ecrire_element` reçoit un `float` par valeur).
- Estimation : bornes 1,1 ms, élément 1,1 ms, deux ou trois passages de `float` par valeur (0,27 ms chacun, différence mesurée entre `pile_empiler_nombre` 0,400 et sa version exacte 0,133), et `arrondir` : conversion vers `double`, addition, deux comparaisons de `double` (0,45 chacune), conversion en `long`, conversion inverse et comparaison. Soit 2 à 4 ms d'après les coûts mesurés (`float`→`double` 0,883, addition 0,767, comparaison 0,45). Total : **≈ 4,5 à 6,5 ms par lecture ou écriture d'élément**, contre 0,2 ms pour une variable simple. Pour `FAIRE 20 POUR I_1 JUSQUA 1000` / `20 S_S+T[I]` : +5 à 6 s.
- Proposition :
  (a) chemin exact des indices : si l'indice est un entier exact sous 2²⁴, ce que reconnaît déjà `entier_exact` (`donnees.c:729`), on prend l'entier sans `arrondir`. Indices passés par adresse. Résultat identique bit à bit, puisque `arrondir(n) = n` pour un entier exact. Gain ≈ 2 à 4 ms par accès.
  (b) lecture de l'en-tête, du bit de définition et de l'élément sans verrouillage, et écriture de même, sous la condition d'IMP-0243 (aucun appel système entre la lecture du pointeur maître et la copie) : il faut un `objets_ecrire` symétrique d'`objets_copier` et un décalage `unsigned long` pour les tableaux hors norme. Gain ≈ 1,9 ms par accès.
  Avec (a) et (b), un accès passe de ≈ 5 ms à ≈ 0,6 ms. Risque : faible pour (a) ; pour (b), discipline à respecter dans `donnees.c`, qui n'appelle aucun service entre l'adresse et l'usage.
- Décision IMP nécessaire : oui ; (a) : révision d'IMP-0244 (« I4 — indices ») ; (b) : extension d'IMP-0243 aux accès aux tableaux (nouvelle IMP).

### G1-P-02 — Affectation numérique et pas de FAIRE : `float` passés par valeur
- Gravité : mineur (≈ 11 % sur les boucles de référence, mais sur toutes les affectations, pour une modification simple).
- Statut : CONFIRMÉ par lecture ; coût déduit des mesures (`contexte_affecter_nombre` 0,333 ms contre `contexte_lire_nombre` 0,067 ms).
- Emplacement : `interprete.c:127` (`ecrire_reference(…, float v)`, puis `float f = v`), `contexte_exec.c:226` (`contexte_affecter_nombre(…, float v)`, puis `float copie = v`), `contexte_exec.c:602`, `donnees.c:626`.
- Estimation : une affectation `X_…` fait deux passages par valeur (≈ 0,53 ms), le pas de FAIRE un (≈ 0,27 ms). Boucles (b) et (d) : ≈ 0,8 ms par tour sur 7,1 et 8,9 ms (−11 % et −9 %). Programme `LSE`, boucle (b) de 1000 tours : ≈ 7 s → 6,2 s.
- Proposition : versions par adresse (`contexte_affecter_nombre_exact(c, const float *)`, `ecrire_reference(c, prof, const float *)`, `donnees_ecrire_element(h, rang, const float *)`), comme `pile_empiler_nombre_exact`. Risque nul : mêmes bits.
- Décision IMP nécessaire : révision mineure d'IMP-0244 (le passage par adresse s'étend à l'affectation).

### G1-P-03 — Arithmétique non exacte : détour par `double` et double comparaison au bord du `float`
- Gravité : majeur pour les programmes de calcul décimal. PLAUSIBLE : gain à mesurer par `PERF`.
- Emplacement : `interprete.c:151-189` (`a = fa; b = fb;` en `double`, puis `pousser_nombre(c, r)`), `interprete.c:57-61` (`double` par valeur, deux comparaisons, `pile_empiler_nombre` par valeur), `interprete.c:191-228` (même schéma pour les fonctions).
- Estimation : ORCA/C calcule en format étendu. Chaque variable `double` impose une conversion à l'écriture et une à la lecture. Pour une addition décimale : deux `float`→`double` (0,883 mesurée chacune, chargement compris), relecture en étendu, rangement de `r`, passage de `r` par valeur, deux comparaisons de `double` (0,45 chacune), conversion en `float`, passage par valeur (0,27). On peut retirer environ cinq à sept conversions et deux comparaisons, soit **≈ 2 à 3 ms par opération**, sur un coût total estimé à 4 ms ou plus.
- Proposition : pour + − × ÷, variables intermédiaires `long double` (format étendu de SANE). Le résultat arrondi directement en `float` est identique au passage par `double`, car le double arrondi est sans effet pour ces quatre opérations dès que la précision intermédiaire p′ ≥ 2p + 2 (53 et 64 ≥ 50). Dépassement : tester l'exposant du `float` obtenu (255 → EX 44), et ne refaire la comparaison exacte que si le résultat vaut `FLT_MAX`. Empiler par adresse. Ne pas toucher à `pow` et aux fonctions, pour lesquelles le double arrondi peut changer un bit. Vérification bit à bit sur des paires de décimaux, comme `composants_nombres`.
- Risque : faible si la vérification bit à bit est faite ; la formulation d'IMP-0216 (« calculée en double ») est à amender.
- Décision IMP nécessaire : oui, révision d'IMP-0216 ou d'IMP-0244.

### G1-P-04 — Chaînes : verrouillage pour lire une longueur, une paire par opérande
- Gravité : mineur (programmes de traitement de texte : quelques ms par opération).
- Statut : CONFIRMÉ par lecture ; coût déduit (1,1 ms par paire).
- Emplacement : `donnees.c:140-151` (`donnees_objet_vers_chaine` résout l'objet pour lire 3 octets, à chaque lecture d'une variable-chaîne), `donnees.c:112-128`, `196-213`, `215-228` (une paire résoudre/relâcher par opérande, plus l'objet créé).
- Estimation : `C_C!'X'` ≈ 1,1 (longueur de C) + 3 × ≈ 1 (objet neuf, C, texte en ligne) + `objets_creer` + libération de l'ancienne valeur ≈ 5 à 7 ms, hors balayage d'`objets_creer` (piste hors périmètre).
- Proposition : en-tête lu par `objets_copier(objet, 0, tete, 3)` (0,133 ms) ; textes sources lus par le pointeur maître, sous la condition d'IMP-0243, l'objet destination restant résolu. Gain ≈ 1 ms par opérande.
- Décision IMP nécessaire : oui (même extension d'IMP-0243 que G1-P-01 b).

### G1-P-05 — Appel de procédure : verrouillage de la ligne d'entrée et six allocations
- Gravité : mineur. CONFIRMÉ par lecture pour le verrouillage (1,1 ms) ; PLAUSIBLE pour les allocations (coût de `malloc` d'ORCA/C non mesuré).
- Emplacement : `contexte_exec.c:715-718` (`objets_resoudre` + `rcim_longueur_element` + `objets_relacher` pour sauter l'élément 0xCD) ; `contexte_exec.c:724-727` et `332-346` (`creer_cadre` : 3 `malloc` ; `vals`, `repris`, `neuves` : 3 `malloc` libérés aussitôt).
- Proposition : `objets_copier(ligne.bloc, 0, tete, min(longueur, 8))` puis `rcim_longueur_element` sur la copie (−1 ms par appel) ; `vals`, `repris` et `neuves` en tableaux locaux bornés, l'arité et le nombre de LOCAL étant bornés par une ligne de déclaration (≈ 35 noms) ; ou un seul bloc par cadre.
- Décision IMP nécessaire : non pour le verrouillage ; à voir avec IMP-0150 pour le regroupement des allocations.

### G1-P-06 — Petits coûts répétés (optionnels)
- Gravité : mineur ; gains inférieurs à 5 % sur les boucles de référence. CONFIRMÉ par lecture.
- (a) Constante immédiate convertie à chaque exécution (`interprete.c:1041-1042`, `donnees_nombre_entier` 0,167 ms) : une table statique des 57 `float` de −28 à 28 → ≈ 0,13 ms par constante immédiate.
- (b) `charger_ligne` (`interprete.c:1215`) refait `programme_chercher` (0,067 ms) alors que `programme_suivante` ou `contexte_brancher` viennent de trouver l'entrée : la transmettre.
- Décision IMP nécessaire : non.

---

## 4. Points écartés après vérification

- Appel de procédure, ALLER EN, FAIRE, RETOUR ou RESULTAT en machine de bureau (transition sans rechargement dans `interprete_bureau`) : refusés par le compilateur (`compilateur.c:538`, `1619-1697`).
- Adressage des tableaux hors norme de plus de 64 Kio dans `donnees.c` (`:621, 636, 653`) : correct, le décalage étant calculé en `unsigned long`. Essai D : 65 535 éléments écrits et relus sans faute dans les deux profils.
- Troncature du nom de fichier (`nom_de_fichier` à 7 caractères, `executer_module` à 6) : toute troncature laisse plus de 5 caractères, donc EX 53 (`bibliotheque.c:46`).
- Débordement de `t[300]` (`ecrire_nombre`) ou de `corps[320]` : `e` et `d` sont bornés à 99 par le compilateur (`compilateur.c:1222-1228`), soit au plus ≈ 245 caractères.
- `nb_sauvegardes` et `nb_alias` en `unsigned char` : au plus ≈ 35 noms sur une ligne de déclaration.
- Deux objets du même bloc résolus en même temps dans `donnees.c` (`extraire`, `donnees_concatener`, `donnees_comparer`, `fin_sch`, `donnees_pos`, `donnees_skp`, `donnees_copier`) : aucun appel système entre l'obtention de l'adresse et son dernier usage ; conforme à la règle du §11 du Gestionnaire d'objets.
- `pile_empiler` à 32 768 éléments (`2 * capacite` = 0) et `allouer_case` à 32 768 cases : au-delà du seuil de G1-A-04 et G1-A-05, réglés par leur plafond.
- NaN ou infini sur la pile : impossible ; opérandes finis, cas de domaine contrôlés (EX 25, 26, 31, 41, 42), débordement EX 44.
- Accès à un élément après redéclaration du tableau pendant l'évaluation de l'instruction : borné par `rang < n` (EX 96), sans débordement.
- APPELER laissant l'environnement de l'ancien programme : non, `reconstruire` appelle `contexte_vider` (`bibliotheque.c:719`).
- `executer_module` qui échouerait dans `contexte_initialiser` après le remplacement du programme : impossible, `verifier` contrôle la ligne β avant tout remplacement (`bibliotheque.c:789`).
- Nombre de plus de 79 caractères dans `saisir_nombre` et `donnees_cnb` (`tampon[80]`) : seul un exposant écrit au-delà du 79ᵉ caractère serait perdu ; cas artificiel.
- `UN`/`ZER` sur 65 535 éléments (`donnees.c:652-653`, un `memcpy` par élément) : coût linéaire de l'ordre de quelques secondes, rare ; non retenu.
- Pile du 65816 : ≈ 1,5 Kio au plus dans le groupe, aucune récursion C (§2).

---

## 5. Pistes hors périmètre

- `moniteur.c:1535-1564` (`ligne_bureau`) et `moniteur.c:358-366` (`continuer`) : la pile n'est pas ramenée après une ligne de machine de bureau en erreur ou interrompue pendant LIRE (cause de G1-A-02) ; l'état de lecture n'est ni sauvegardé ni réinitialisé, contrairement à `interprete.h:41-43` (G1-A-08).
- `objets.c:198-215` (`objets_creer`) : `HLock`/`HUnlock` de chaque bloc-maître examiné, depuis le bloc 0, à chaque création (≈ 0,5 ms par bloc). L'en-tête pourrait être lu par `plateforme_bloc_adresse`, sous la condition d'IMP-0243. Ce coût s'ajoute à chaque chaîne transitoire créée par `donnees.c`.
- `objets.c:247-259` et `312-323` : `objets_resoudre` et `objets_relacher` refont chacun `objets_existe`, qui verrouille et déverrouille, d'où 1,1 ms par paire au lieu de ≈ 0,5 ms (mesure : `HLock`+`HUnlock` ≈ 0,5 ms).
- `fichiers.c:373-402` (`charger_ouvert`, `t + p` avec `p` long) et `fichiers.c:305-309` : adressage en `unsigned long`, correct par analogie avec l'essai D ; non vérifié par essai à cause de G1-A-06.

---

## 6. Points à arbitrer par Éric

1. **ESC et pas à pas dans les boucles** (G1-A-01). Faut-il consulter ESC à chaque reprise d'itération, avec une position en milieu de ligne pour `PRET EN LIGNE lll` ? Et le PAS A PAS doit-il s'arrêter à chaque retour sur la ligne FAIRE ? Aujourd'hui, il ne s'arrête qu'aux débuts de ligne : une boucle d'une ligne s'exécute entièrement en un pas, et le retour sur la ligne FAIRE d'une boucle de plusieurs lignes n'affiche pas `LIGNE 10`. Essais proposés sur LSE 8086, en PAS A PAS : `10 FAIRE 20 POUR I_1 JUSQUA 3` / `20 X_I`, puis `10 FAIRE 10 POUR I_1 JUSQUA 3;X_I`.
2. **Plafond de la pile d'évaluation et des cases** (G1-A-04, G1-A-05). Retenir EX 03 « MEMOIRE SATUREE » à 9 362 éléments et 13 107 cases (simple et sans coût), ou un adressage en `unsigned long` sans plafond (plus lent sur l'opération la plus fréquente) ? Essai proposé sur LSE 8086 : profondeur atteinte et message obtenu avec `10 X_&F(20000)` / `30 PROCEDURE &F(N) LOCAL N` / `40 SI N=0 ALORS RESULTAT 0` / `50 RESULTAT 1+&F(N-1)`.
3. **Description d'AFFICHER à travers un appel** (G1-A-07). Confirmer le résultat attendu de `AFFICHER[2F6.2] &F(1),2` quand F exécute `AFFICHER['*']` (`*  1.00  2.00` ?) ; essai proposé sur LSE 8086.
4. **LIRE en machine de bureau pendant une suspension** (G1-A-08). Une ligne de machine de bureau doit-elle partir d'un état de lecture vierge, et le programme retrouver le sien à CONTINUER ? Essai proposé sur LSE 8086 : `10 LIRE A;LIRE B`, réponse `1 2`, puis… plus simplement : `10 LIRE A` / `20 TERMINER`, réponse `1 2`, puis en machine de bureau `LIRE B` : la valeur 2 est-elle prise sans nouvelle saisie ?
5. **Élément sans valeur passé à un paramètre LOCAL** (G1-A-03) : état `ID_PARAMETRE_SANS_VALEUR` (erreur EX 14 à l'utilisation, comme RX3) ou EX 32 dès l'appel ?
6. **Arithmétique non exacte en format étendu** (G1-P-03) : accepter d'amender la formulation d'IMP-0216 (« calcul en double »), le résultat restant identique bit à bit pour + − × ÷.
7. **Extension d'IMP-0243** (lecture et écriture sans verrouillage) aux tableaux et aux chaînes (G1-P-01 b, G1-P-04) : principe et périmètre (fonctions de `donnees.c` seulement).
