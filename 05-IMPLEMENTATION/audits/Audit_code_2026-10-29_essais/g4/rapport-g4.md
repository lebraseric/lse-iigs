# Audit du code LSE-IIGS — groupe 4 : objets, bibliothèque, fichiers, codage, ruban, tâches

Date : 2026-10-29. Périmètre : `src/noyau/objets.c/.h`, `bibliotheque.c/.h`, `fichiers.c/.h`, `codage.c/.h`, `ruban.c/.h`, `taches.c/.h` (copie d'audit `S/copie-g4/`, identique au dépôt au début de l'audit : `diff -rq` sans différence).

Lectures : Gestionnaire d'objets (en entier), Bibliothécaire et fichiers §1 à §7 quater, Tâches et reprise, Gestionnaire des erreurs §9 ter, Architecture physique §2, §5.1, §10 ; mesures du 2026-10-25 et du 2026-10-29 ; manuel ORCA/C (`stacksize`, `debug`, `asm`, `size_t`).

Essais (copie d'audit seulement, ORCA/C 2.2.1 sous Golden Gate, profils `dev` et `cible`) :
- `S/copie-g4/tests/composants/essai_g4.c` : ligne vide, RCI-S altéré, compteurs d'appels, profondeur de pile, grand tableau, longueur restituée, boucle de chaînes ;
- `S/copie-g4/tests/composants/essai_g4b.c` et `essai_g4d.c` : RANGER d'une image RCI-S de plus / de moins de 65 535 octets ;
- `S/copie-g4/tests/composants/essai_g4c.c` : profondeur de pile d'une session complète du moniteur ; ligne vide par le moniteur ;
- instrumentation de la copie seulement : compteurs dans `S/copie-g4/src/plateforme/memoire_iigs.c` (HLock, HUnlock, NewHandle, DisposeHandle) et `fichiers_iigs.c` (ouvrir, lire, écrire, taille, fermer, lister, supprimer) ;
- règles ajoutées à `S/copie-g4/build/Makefile` ; sorties rassemblées dans `S/essais_g4_sorties.txt`.

Conversion des compteurs en temps : `objets_resoudre` + `objets_relacher` = 1,133 ms en profil `cible` (mesures du 2026-10-25) pour 3 `HLock` + 3 `HUnlock` (compté par l'essai, §4), soit **≈ 0,19 ms par appel toolbox `HLock` ou `HUnlock`** (borne haute : le surcoût C des deux fonctions y est inclus). Aucun coût d'appel GS/OS n'a été mesuré : les estimations portant sur les fichiers sont données en nombre d'appels, sans durée.

---

## 1. Carte des chemins chauds

### 1.1 Gestionnaire d'objets (`objets.c`)

Coût par opération, compté par l'essai (`essais_g4_sorties.txt`, sections 4, 5, 11) :

| Opération | Appels toolbox | Estimation |
|---|---|---|
| `objets_resoudre` (`objets.c:247`) + `objets_relacher` (`objets.c:312`) | 3 `HLock` + 3 `HUnlock` (deux `objets_existe` + l'accès) | 1,13 ms (mesuré) |
| `objets_creer` (`objets.c:179`), k blocs pleins devant | 2(k+1) appels, +2 et un `NewHandle` si un bloc est acquis | 0,37 ms × (k+1) |
| `objets_liberer` (`objets.c:325`) | 2 `HLock` + 2 `HUnlock` (`objets_existe` + mise à jour) | ≈ 0,75 ms |
| `objets_existe` (`objets.c:226`) | 1 `HLock` + 1 `HUnlock` (bloc-maître) ; 0 (hors-norme) | ≈ 0,37 ms |
| `objets_copier` (`objets.c:263`), `objets_longueur` (`objets.c:293`) | 0 (IMP-0243) | 0,133 ms par ligne (mesuré) |

`objets_existe` n'est appelé que dans `objets.c` (`:252`, `:316`, `:333`) : une fois par résolution, une fois par relâchement, une fois par libération.

Appelants dans tout `src/` (grep) et fréquence :

| Appelant | Opération | Fréquence |
|---|---|---|
| `interprete.c:1216` | `objets_copier` | à chaque changement de ligne (chemin le plus chaud, déjà sans verrou) |
| `donnees.c:613`, `:629`, `:644` (`donnees_rang`, `donnees_lire_element`, `donnees_ecrire_element`) | `objets_resoudre`/`relacher` | à **chaque accès à un élément de tableau** (deux résolutions par `T[I]` lu : rang puis lecture) |
| `donnees.c:82`, `:171` ; `:71`, `:142`, `:167`… | `objets_creer`, `objets_resoudre` | chaque chaîne transitoire (constante chaîne évaluée, `!`, extraction, fonctions de chaîne) ; 2 à 3 résolutions par opération |
| `pile.c:32`, `interprete.c:86`, `:304`, `:689`, `:955`, `:1134` | `objets_liberer` | chaque chaîne transitoire consommée |
| `donnees.c:564` | `objets_creer` | chaque déclaration de tableau |
| `contexte_exec.c:716` | `objets_resoudre` | chaque appel de procédure |
| `interprete.c:1344` | `objets_resoudre` | chaque ligne de machine de bureau |
| `interprete.c:889-891`, `:958-964` | `objets_longueur`, `objets_copier` ; `objets_creer`, `objets_resoudre` | GARER, CHARGER d'un tableau |
| `compilateur.c:1867-1872` | `objets_creer`, `objets_resoudre` | chaque ligne compilée (saisie, ENCODER, bureau) |
| `programme.c:82` (`decoder_declaration`) | `objets_resoudre` | chaque ligne insérée de longueur non nulle |
| `programme.c:206`, `:225`, `:285` | `objets_liberer` | ligne remplacée, effacée ; `programme_vider` |
| `symboles.c:115` (`meme_texte`) | `objets_resoudre` | à chaque constante partagée compilée, **pour chaque constante existante de même longueur** |
| `symboles.c:140-153`, `:182` | `objets_creer`/`resoudre`, `objets_liberer` | nouvelle constante partagée ; vidage |
| `decompilateur.c:1043`, `:707` | `objets_resoudre` | chaque ligne restituée (LISTER, DECODER, PERFORER) ; chaque constante partagée restituée |
| `bibliotheque.c:576`, `:584` | `objets_copier` | RANGER : par constante, par ligne |
| `bibliotheque.c:739-745` | `objets_creer`, `objets_resoudre`, `objets_liberer` | APPELER, EXECUTER : par ligne |
| `moniteur.c:1408`, `:1527`, `:1563` | `objets_liberer` | ligne refusée, ligne de bureau exécutée |
| `main.c:40`, `:52` | `objets_terminer` | fin de session |

Coûts agrégés mesurés (compteurs, profil `cible`) :
- saisie d'une ligne : 8,1 couples `HLock`/`HUnlock` (≈ 3 ms, imperceptible) ;
- APPELER d'un programme de 200 lignes : **2 020 `HLock` + 2 020 `HUnlock`** (10,1 couples par ligne : création ≈ 2, résolution 3, `decoder_declaration` 3, libération de l'ancien programme 2), soit ≈ 0,77 s ;
- DECODER : 3 couples par ligne (résolution par le décompilateur) ;
- boucle `220 C_C!'A'` : 15 couples par tour (≈ 5,7 ms), 18 couples avec 190 lignes devant (≈ 6,9 ms), à comparer à 7,1 ms par tour pour la boucle (b) sans chaîne.

### 1.2 Bibliothèque (`bibliotheque.c`)

| Fonction | Appels de fichier | Fréquence |
|---|---|---|
| `catalogue_chercher` `:211` | 1 `plateforme_fichier_lire` de 16 octets **par entrée** (fseek + fread) | appelée par `lire_dans` `:315` (droit, protection, lecture interdite), `inscrire` `:239` (deux passes `:267-268`), `bibliotheque_decataloguer` `:281` |
| `droit` `:402`, `bibliotheque_lecture_interdite` `:424` | une recherche de catalogue (N lectures) | chaque GARER, CHARGER, SUPPRIMER sur fichier permanent ; RANGER, MODIFIER |
| `bibliotheque_catalogue_suivant` `:340` | lecture par blocs de 16 entrées | une fois par ligne de TABLE DES FICHIERS |
| `image_rcis` `:535`, `ecrire_fichier` `:593` | écriture par blocs de 4 096 octets | RANGER, MODIFIER |
| `bibliotheque_appeler` `:756`, `verifier` `:652`, `reconstruire` `:707` | lecture par blocs de 4 096 octets | APPELER, EXECUTER |

Compté (catalogue de 4 entrées) : RANGER 5 ouvertures, 6 lectures, 2 écritures ; APPELER 2 ouvertures, 5 lectures.

### 1.3 Fichiers-données (`fichiers.c`)

| Fonction | Coût | Fréquence |
|---|---|---|
| `fichiers_garer` `:335` → `garer_ouvert` `:254` | ouverture, lecture du secteur 0, `parcourir` (1 lecture par secteur de répertoire), écriture de l'étendue **secteur par secteur avec `fflush`** (`:305-310`), relecture-écriture d'un secteur de répertoire | chaque GARER, chaque enregistrement de DECODER |
| `fichiers_secteurs_temporaires` `:235` | (T+1) × `plateforme_fichier_lister` (OpenGS + GetDirEntryGS + CloseGS) + T × (fopen, fseek, ftell, fclose) | **chaque GARER sur un fichier temporaire** (`:302`), deux fois s'il est créé (`:349`) |
| `fichiers_charger` `:404` → `charger_ouvert` `:373` | ouverture, secteur 0, `parcourir`, lecture par 4 096 octets ; sans `#` et temporaire absent : une recherche de catalogue | chaque CHARGER, chaque enregistrement lu par ENCODER |
| `fichiers_cataloguer` `:459` | par enregistrement : `charger_ouvert` + `garer_ouvert` | CATALOGUER |

Compté : GARER temporaire existant, 2 temporaires présents : 3 ouvertures, 4 lectures, 3 écritures, 3 parcours de répertoire GS/OS ; GARER permanent existant : 2 ouvertures, 9 lectures, 3 écritures ; CHARGER permanent : 2 ouvertures, 8 lectures ; DECODER de 200 lignes (12 enregistrements) : **25 ouvertures, 47 lectures, 37 écritures avec `fflush`, 25 parcours de répertoire** ; ENCODER de ce fichier : 12 ouvertures, 36 lectures.

### 1.4 Codage, ruban, tâches

- `codage_decoder` `codage.c:72` : une décompilation par ligne, un `fichiers_garer` par enregistrement de 252 caractères (`emettre` `:31`).
- `codage_ligne` `codage.c:195` → `caractere` `:160` : un `fichiers_charger` par enregistrement.
- `ruban.c` : une ouverture du ruban par bloc de 256 caractères lus (`caractere` `:44`) ; PERFORER : `plateforme_fichier_taille` + 2 écritures avec `fflush` par ligne (`:110-121`). Coût négligeable.
- `taches.c` : deux appels de fonction triviaux par saisie (`moniteur.c:206`, `:237`).

---

## 2. Budget de pile

Pile d'ORCA/C : celle du lanceur, 4 Kio sous le shell ORCA, sur laquelle la bibliothèque prend aussi la page directe de SANE (manuel ORCA/C, `stacksize`) ; le dépôt n'a pas de `#pragma stacksize`. Le bit 0 de `#pragma debug` (profils `dev` et `cible`) contrôle le débordement à l'entrée de chaque fonction C, pas dans les appels toolbox ou GS/OS (manuel, `debug`).

### 2.1 Locales de plus de 64 octets (groupe 4, lecture)

| Fonction | Taille estimée |
|---|---|
| `objets.c:88` `compacter` (`ordre[64]`) | ≈ 72 o |
| `bibliotheque.c:340` `bibliotheque_catalogue_suivant` (`t[256]`, `meilleur[16]`, `candidat[6]`) | ≈ 300 o |
| `fichiers.c:144` `ouvrir`, `:156` `fixer_taille`, `:165` `ecrire_place`, `:214` `nouveau_repertoire` (`s[256]` chacune) | ≈ 262 o chacune |
| `fichiers.c:177` `parcourir` (`s[256]`, `Entree`) | ≈ 280 o |
| `fichiers.c:254` `garer_ouvert` (`s[256]`, 5 `Place` de 17 o, 2 `Entree` de 11 o) | ≈ 380 o |
| `fichiers.c:335` `fichiers_garer` (`s[256]`) | ≈ 272 o |
| `fichiers.c:459` `fichiers_cataloguer` (`s[256]`, `Entree`) | ≈ 288 o |
| `fichiers.c:373` `charger_ouvert`, `:422` `fichiers_supprimer` (3 `Place`) | ≈ 66 et 72 o |
| `codage.c:72` `codage_decoder` (`texte[257]`, 2 `EntreeObjetCompile`) | ≈ 277 o |
| hors groupe, appelées sur ces chemins : `fichiers_iigs.c` `plateforme_fichier_ouvrir` (`c[200]`), `plateforme_fichier_lister` (`c[200]`, `ResultBuf255`, `DirEntryRecGS`) | ≈ 210 et ≈ 500 o |

Aucune récursion dans le groupe 4 ; l'interpréteur n'est pas récursif non plus (`cycle`, `interprete.c:1276`, est une boucle).

### 2.2 Profondeurs mesurées (peinture de la pile, profil `cible`, depuis `main`)

| Chemin | Octets |
|---|---|
| `fichiers_garer` avec nouveau secteur de répertoire | 1 372 (1 419 en `dev`) |
| `fichiers_cataloguer` | 1 124 |
| `codage_decoder` (DECODER) | 2 330 (2 369 en `dev`) |
| dont `decompilateur_decompiler` seul | 2 034 |
| interpréteur exécutant GARER puis CHARGER | 1 847 |
| `bibliotheque_appeler` / `bibliotheque_ranger` | 431 / 435 |
| `bibliotheque_catalogue_suivant` | 641 |
| **session complète du moniteur** (ID, programme avec procédure qui GARE, EXECUTER, DECODER, ENCODER, CATALOGUER) | **2 749** |

Sous Golden Gate, GS/OS est réalisé par l'hôte : la pile qu'emploient GS/OS et le FST sur l'Apple IIGS n'est pas comprise dans ces chiffres (incertitude). Voir G4-A-05.

---

## 3. Constats

### G4-A-01 — Un programme qui contient une ligne vide ne peut plus être appelé
- Gravité : **bloquant** (le fichier-programme rangé est inutilisable : APPELER et EXECUTER le refusent)
- Statut : **CONFIRMÉ** (essai `essai_g4.c` §1 et session du moniteur `essai_g4c.c`)
- Emplacement : `src/noyau/bibliotheque.c:695`
- Constat : `verifier` rejette toute ligne de longueur nulle (`if (lg == 0 || …) return ERR_F_FICHIER_ENDOMMAGE`). Or une ligne vide compile en une séquence de longueur 0 (`compilateur.c:1719-1721`, instruction vide ; IMP-0194) ; RANGER l'écrit sans difficulté (`image_rcis`, `bibliotheque.c:583`).
- Scénario : `10 FAIRE 20 POUR I_1 JUSQUA 3`, `20 `, `30 TERMINER` (forme attestée, INRDP75), `RANGER VIDE`, `APPELER VIDE` → `ERREUR F 101`. Même résultat pour `EXECUTER 'VIDE'`. Le programme courant reste intact, mais le fichier rangé est perdu pour l'utilisateur.
- Référence : IMP-0194 (ligne vide attestée, acceptée), IMP-0246 (format : longueur u8, aucune exclusion de 0), VM-3402.
- Proposition : `if (p + 2 + lg > n - 2) return ERR_F_FICHIER_ENDOMMAGE;` ; la boucle d'éléments et `reconstruire` traitent déjà une longueur nulle (`objets_creer(0)`, `memcpy` de 0). Les fichiers déjà rangés redeviennent lisibles (seul le lecteur est en faute). Ajouter le cas à `composants_bibliotheque`.
- Décision IMP nécessaire : non.
- Essai : `essais_g4_sorties.txt`, « -- 1. ligne vide » (`APPELER VIDE : nature 174 (F 101)`) et « session ligne vide » (`APPELER VIDE` / `ERREUR F 101`).

### G4-A-02 — RANGER boucle sans fin si l'image RCI-S dépasse 65 535 octets
- Gravité : mineur (déclenchement improbable ; conséquence : blocage complet de la machine, travail de la session perdu)
- Statut : **CONFIRMÉ** (essais `essai_g4b.c` / `essai_g4d.c`)
- Emplacement : `src/noyau/bibliotheque.c:587` (indice `i` déclaré `unsigned int` à `:543`)
- Constat : `for (i = 0; (unsigned long) i < p; i++)` : `i` repasse à 0 après 65 535 et la condition reste vraie. L'image dépasse 64 Kio quand les constantes partagées accumulées sont nombreuses : elles ne sont jamais retirées avant `programme_vider` (`symboles.c:120-160`), et RANGER les range toutes, même celles qu'aucune ligne n'emploie plus.
- Scénario : la ligne 10 retapée 1 200 fois avec des messages distincts de 40 à 72 caractères (image ≈ 68 Ko) → RANGER ne rend jamais la main (arrêté après plus de 3 min). Contre-essai avec 1 000 messages (image ≈ 57 Ko) : RANGER rend `ERR_AUCUNE` en 1 s.
- Référence : IMP-0246 (somme de contrôle sur tous les octets) ; `verifier` emploie déjà un indice `unsigned long` (`:656`, `:669`).
- Proposition : indice `unsigned long` pour la somme. L'accumulation de constantes mortes est un point distinct (point à arbitrer n° 6).
- Décision IMP nécessaire : non.
- Essai : `essais_g4_sorties.txt`, sections `essai_g4b` et `essai_g4d`.

### G4-A-03 — La vérification RCI-S ne contrôle ni les identifiants, ni les index, ni l'unicité : fichier altéré accepté ou programme courant perdu
- Gravité : mineur (fichier altéré dont la somme reste juste ; une somme additive ne voit pas une permutation d'octets)
- Statut : **CONFIRMÉ** (essai `essai_g4.c` §2 et §3)
- Emplacement : `src/noyau/bibliotheque.c:652-704` (`verifier`), `:719-753` (`reconstruire`)
- Constat : IMP-0246 annonce la vérification de « chaque identifiant de symbole et chaque index de constante dans les tables du fichier » ; `verifier` ne fait que parcourir les éléments (`rcim_longueur_element`, qui ignore `ns` et `nc`). La nature et l'origine des symboles, l'unicité des noms et des textes, la longueur des constantes (17 à 127, `symboles.c:131`) ne sont pas vérifiées : ces fautes ne sont découvertes que par `reconstruire`, **après** `contexte_vider` et `programme_vider`.
- Scénarios : (a) identifiant de symbole d'une ligne porté à 9 (`ns` = 2), somme recalculée → APPELER rend `ERR_AUCUNE`, la décompilation de la ligne échoue (`DECOMP_ERR_REFERENCE`) ; à l'exécution, `assurer` agrandit l'environnement jusqu'à `id + 1` (pour `id` = 0xFFFF, débordement : piste P-4). (b) symbole Y renommé X → `ERREUR F 23` « mémoire trop petite » et le programme courant (une ligne) est vidé, contrairement à VM-3403.
- Référence : IMP-0246 (« Restauration »), VM-3403, Gestionnaire des erreurs §9 ter (F 23, F 101).
- Proposition : compléter `verifier` : nature ∈ {0, 1}, origine ∈ {0, 1}, noms en majuscules et chiffres, unicité (nom, nature), constantes de 17 à 127 octets et uniques, et, pour chaque élément portant un identifiant ou un index de constante, borne `< ns` ou `< nc` (une fonction de `rcim.c` qui rend ces champs éviterait de dupliquer la table des étiquettes). Rendre F 101 dans tous ces cas.
- Décision IMP nécessaire : non (réalise IMP-0246 tel qu'écrit).
- Essai : `essais_g4_sorties.txt`, « -- 2 » (`APPELER DEUX altere : nature 0`, `decompilation ligne 10 : 3`) et « -- 3 » (`nature 169 (F 23), lignes 0`).

### G4-A-04 — DECODER produit des lignes que ENCODER refuse (C 63)
- Gravité : **majeur** (un programme valide ne fait pas l'aller-retour DECODER / ENCODER)
- Statut : **CONFIRMÉ** (essai pour la longueur restituée ; chemin ENCODER lu entièrement)
- Emplacement : `src/noyau/codage.c:97-107` (texte restitué rangé tel quel) ; refus dans `compilateur.c:1820-1823` (`COMPILATEUR_LONGUEUR_MAX` = 79), appelé par `moniteur.c:1401`
- Constat : le décompilateur peut restituer une ligne plus longue que sa source (nombres développés) ; `codage_ligne` la relit en entier (255 caractères au plus) et `compilateur_compiler` la refuse au-delà de 79 caractères ; ENCODER s'arrête à la première erreur (IMP-0261).
- Scénario : `1 X_1E5+1E5+…+1` (77 caractères) est restitué en 131 caractères (`100000+100000+…`) ; `1 X_1E-3+…` (78) en 93. `DECODER XX` puis `ENCODER XX` → la ligne est affichée suivie de `ERREUR C 63`, et (MITRA 15 sans n1) le programme courant a déjà été vidé.
- Référence : IMP-0260, IMP-0261 ; Compilateur (C 63, 80 caractères d'INRDP75) ; Décompilateur IMP-0198 (255 caractères).
- Proposition : à arbitrer (point n° 1). Pistes : compiler les lignes lues par ENCODER (et par le ruban) avec la borne du décompilateur ; ou décider que la restitution des nombres ne dépasse jamais la longueur de la source.
- Décision IMP nécessaire : oui (révision d'IMP-0261, ou du Compilateur, selon l'arbitrage).
- Essai : `essais_g4_sorties.txt`, « -- 10 ».

### G4-A-05 — Marge de pile d'environ 1 Kio sur la pile de 4 Kio, GS/OS non compté
- Gravité : **majeur** (un débordement dans un appel GS/OS n'est pas détecté et écrase la banque 0)
- Statut : PLAUSIBLE (profondeurs mesurées sous Golden Gate ; manque : la pile employée par GS/OS sur l'Apple IIGS, et la profondeur au-dessus de `main`)
- Emplacement : chaînes `fichiers.c` (`fichiers_garer` `:339` → `garer_ouvert` `:258` → `nouveau_repertoire` `:217` → `fixer_taille` `:158` : quatre tampons de 256 octets empilés), `codage.c:76` (`texte[257]`) ; hors groupe : `decompilateur.c:1028` (piste P-2), absence de `#pragma stacksize`
- Constat : une session ordinaire descend à 2 749 octets sous `main` (§2.2). Sur 4 096 octets, moins la page directe de SANE (256) et le cadre de lancement, il reste ≈ 1 Kio pour GS/OS, le FST et les pilotes, que Golden Gate n'exerce pas. Le contrôle du bit 0 ne couvre que l'entrée des fonctions C ; un débordement le déclenche par un arrêt du programme (session perdue) au mieux.
- Scénario : DECODER (2 330 o sous `main`, plus le cadre du moniteur) au moment d'un `fopen` GS/OS qui emploie la pile de l'appelant.
- Référence : manuel ORCA/C, `stacksize` (4 Kio du shell ORCA, page directe de SANE), `debug` bit 0 ; Construction IMP-0143.
- Proposition : (1) `#pragma stacksize` dans `main.c` (par exemple 8 192 : le manuel admet jusqu'à 32 Kio en programme texte) ; (2) dans le groupe 4, un tampon de secteur `static` commun à `fixer_taille`, `ecrire_place`, `nouveau_repertoire` et `parcourir` (appels séquentiels, aucune réentrance), et un second pour le contenu de `garer_ouvert` réemployé par `fichiers_garer` : gain ≈ 750 o sur la chaîne GARER ; `texte` `static` dans `codage_decoder` : − 257 o ; (3) hors groupe, `Decompilation d` `static` : − 1,5 Kio (piste P-2). Mesurer ensuite sur la cible.
- Décision IMP nécessaire : oui pour (1) (nouvelle IMP, chapitre Construction) ; non pour (2).
- Essai : `essais_g4_sorties.txt`, « -- 8 » et `essai_g4c` (`2749 octets sous main`).

### G4-A-06 — GARER : fenêtre où une étendue est à la fois vivante et libre
- Gravité : mineur (exige un échec d'écriture entre deux écritures consécutives)
- Statut : CONFIRMÉ (chemin lu entièrement)
- Emplacement : `src/noyau/fichiers.c:294-297`, `:317`, `:330`
- Constat : quand un enregistrement existant reçoit l'étendue libre trouvée (`rec.premier = libre.e.premier`) et que l'entrée de cette étendue libre sert ensuite à noter l'ancienne étendue (`pos_ancien = libre`), l'entrée de l'enregistrement est réécrite d'abord (`:317`) ; jusqu'à la réécriture de l'entrée libre (`:330`), la même étendue est décrite deux fois, vivante et libre.
- Scénario : erreur d'entrée-sortie à `:330` → le fichier garde l'étendue en entrée `ETAT_LIBRE` ; le GARER suivant la réemploie et écrase l'enregistrement vivant. VM-3405 (« un incident ne laisse jamais un enregistrement à moitié écrit ») est respecté pour l'enregistrement écrit, mais pas pour le suivant.
- Référence : IMP-0247 (ordre des écritures), VM-3405.
- Proposition : quand `pos_rec` et `pos_ancien` sont dans le même secteur de répertoire, une seule écriture de ce secteur portant les deux entrées ; sinon, marquer d'abord l'entrée libre `ETAT_INUTILISE` (un incident ne fait alors que perdre l'étendue), puis écrire l'enregistrement, puis l'ancienne étendue.
- Décision IMP nécessaire : non (précise l'ordre d'IMP-0247 sans changer le format).
- Essai : aucun.

### G4-A-07 — Une étendue libre est consommée entière : place perdue et EX 52 prématuré
- Gravité : mineur (efficacité disque ; effet observable : EX 52 plus tôt dans l'espace temporaire)
- Statut : CONFIRMÉ (lecture)
- Emplacement : `src/noyau/fichiers.c:282-284`
- Constat : `rec.nb = libre.e.nb` : la première étendue libre assez grande est prise en entier, sans découpe ; le reste est perdu jusqu'à la suppression de l'enregistrement.
- Scénario : GARER d'un tableau de 1 000 éléments (17 secteurs) en enregistrement 1, SUPPRIMER 1, puis GARER de nombres en 2 à 40 : l'enregistrement 2 occupe les 17 secteurs, 3 à 40 s'ajoutent en fin de fichier (38 secteurs) ; avec le quota de 200 secteurs, EX 52 survient plus tôt qu'avec une découpe.
- Référence : IMP-0247 (« la première étendue libre assez grande »), IMP-0253 (quota).
- Proposition : découper l'étendue quand une entrée inutilisée est disponible dans le même secteur de répertoire (le reste devient une étendue libre) — à arbitrer (point n° 2).
- Décision IMP nécessaire : oui (précision d'IMP-0247).
- Essai : aucun.

### G4-A-08 — Numéros de secteur sur 16 bits : un fichier permanent de plus de 65 535 secteurs se corrompt
- Gravité : mineur (16 Mio de fichier-données)
- Statut : CONFIRMÉ (lecture)
- Emplacement : `src/noyau/fichiers.c:219-223`, `:286-287`, `:309`, `:311-313`
- Constat : `taille`, `rec.premier`, `rec.nb` sont des `unsigned int` (format u16, IMP-0247). Aucune borne : `rec.premier + i` et `taille + rec.nb` repassent à 0. Les fichiers temporaires sont bornés par le quota ; les fichiers permanents ne le sont que par le support.
- Scénario : GARER répétés de tableaux de 65 535 éléments (270 Ko, 1 057 secteurs) dans un fichier permanent ; au 63ᵉ, l'étendue écrite déborde et écrase les secteurs 0, 1… (répertoire) : fichier endommagé.
- Référence : IMP-0247 (en-tête u16), IMP-0253 (permanents bornés par le support).
- Proposition : dans `garer_ouvert`, refuser si `(unsigned long) taille + croissance > 65535ul` (EX 52 « plus de place dans le fichier »), de même dans `nouveau_repertoire`.
- Décision IMP nécessaire : non.
- Essai : aucun (16 Mio sous Golden Gate).

### G4-A-09 — Chemins d'échec qui laissent la bibliothèque incohérente
- Gravité : mineur (support plein ou erreur d'entrée-sortie)
- Statut : CONFIRMÉ (lecture)
- Emplacement : `src/noyau/bibliotheque.c:641-645` (RANGER), `:636-638` (MODIFIER) ; `src/noyau/fichiers.c:363-367` (GARER créant un permanent), `:504-505` (CATALOGUER)
- Constat : le fichier est écrit avant l'inscription au catalogue ; si l'inscription échoue, l'erreur est rendue mais le fichier reste, sans propriétaire : `droit` (`:407`) laisse alors tout compte le modifier ou le supprimer. MODIFIER supprime l'original avant de renommer `TRANSIT` : un échec du renommage laisse le programme sous `TRANSIT` et une entrée de catalogue sans fichier.
- Scénario : support plein au moment d'écrire l'entrée de `CATALOGUE` → `ERREUR F 02`, fichier présent, non catalogué, modifiable sous toute identification.
- Référence : IMP-0248, IMP-0251, IMP-0255.
- Proposition : sur échec d'inscription, supprimer le fichier créé ; pour MODIFIER, renommer l'original sous un nom de sauvegarde, renommer `TRANSIT`, puis supprimer la sauvegarde (ou la rétablir en cas d'échec).
- Décision IMP nécessaire : non.
- Essai : aucun.

### G4-A-10 — Spécification du Gestionnaire d'objets en retard sur le code
- Gravité : mineur (documentation)
- Statut : CONFIRMÉ (lecture et compteurs)
- Emplacement : `src/noyau/objets.c:293-310`, `:312-323` ; Gestionnaire d'objets §11 (IMP-0194, IMP-0243)
- Constat : (a) IMP-0194 justifie l'octet réservé par « aucune fonction de l'interface ne renvoie la longueur d'un objet » ; `objets_longueur` existe (GARER) et rend 1 pour un objet de longueur nulle (sans effet aujourd'hui : seul GARER d'un tableau l'appelle). (b) IMP-0243 et les mesures du 2026-10-25 attribuent à `objets_resoudre` + `objets_relacher` « deux couples » `HLock`/`HUnlock` ; `objets_relacher` appelle aussi `objets_existe` : trois couples (compté).
- Proposition : corriger les deux textes ; mentionner `objets_longueur` au §11.
- Décision IMP nécessaire : oui (révision rédactionnelle d'IMP-0194 et d'IMP-0243).
- Essai : `essais_g4_sorties.txt`, « -- 4 ».

### G4-P-01 — Verrouillages superflus : existence, création, libération
- Gravité : **majeur** (lenteur perceptible sur les programmes à chaînes et à tableaux, et à APPELER)
- Statut : **CONFIRMÉ** (compteurs de l'essai ; durées estimées à partir de la mesure de 1,133 ms)
- Emplacement : `src/noyau/objets.c:241-243` (`objets_existe`), `:201-214`, `:220-222` (`objets_creer`), `:346-357` (`objets_liberer`) ; `:252`, `:316`, `:333`
- Constat : `objets_existe`, le balayage de `objets_creer` et `objets_liberer` verrouillent et déverrouillent le bloc pour lire ou modifier son en-tête, alors qu'aucun appel système ni toolbox n'a lieu entre la lecture du pointeur maître et le dernier accès (seuls `memmove` et du C) : c'est exactement la condition d'IMP-0243, déjà retenue pour `objets_copier` et `objets_longueur`. En prime, `objets_existe` déverrouille aujourd'hui le bloc d'une autre résolution ouverte (§11) ; sans verrou, ce n'est plus le cas.
- Estimation (≈ 0,19 ms par appel toolbox) :
  - résolution + relâchement : 6 → 2 appels, ≈ 1,13 → 0,4 ms ;
  - `objets_creer` : 2(k+1) → 0 appel ; le balayage de 255 blocs au pire ne coûte plus que des lectures en mémoire ;
  - `objets_liberer` : 4 → 0 appel (hors `DisposeHandle` d'un bloc vidé) ;
  - APPELER ou EXECUTER de 200 lignes : 4 040 → ≈ 800 appels, **≈ 0,77 → 0,15 s** ;
  - boucle `C_C!'A'` : 30 à 36 → ≈ 6 appels par tour, **≈ − 4,5 à − 5,7 ms par tour** (la boucle (b) entière coûte 7,1 ms) ;
  - lecture de `T[I]` (`donnees_rang` + `donnees_lire_element`) : 12 → 4 appels, **≈ − 1,5 ms par accès**.
- Référence : IMP-0243 et sa condition de validité (vol. 1, p. 12-5, 12-22) ; IMP-0159 (étape 1 « verrouiller b »), §7, §11.
- Proposition : lire et écrire l'en-tête par `plateforme_bloc_adresse` dans `objets_existe`, `objets_creer` (balayage, compaction, allocation) et `objets_liberer` ; garder `HLock`/`HUnlock` dans `objets_resoudre`/`objets_relacher` seuls. Commenter dans chaque fonction l'interdiction d'y introduire un appel toolbox. Risque : faible, celui d'IMP-0243 (un appel toolbox ajouté plus tard dans ces fonctions) ; le comportement L.S.E., les formats et `HandleObjet` sont inchangés. Option plus large, à arbitrer : supprimer aussi le verrou de `objets_resoudre` (la règle du §11 suffit), gain d'≈ 0,4 ms par résolution.
- Décision IMP nécessaire : oui (révision d'IMP-0243, d'IMP-0159 et du texte des §7 et §11).
- Essai : `essais_g4_sorties.txt`, « -- 4 », « -- 5 », « -- 11 ».

### G4-P-02 — Recherche dans `CATALOGUE` entrée par entrée, deux fois à l'inscription
- Gravité : mineur à majeur selon le coût GS/OS, non mesuré
- Statut : PLAUSIBLE (nombre d'appels lu et compté ; manque la durée d'un `fseek` + `fread` de 16 octets sur l'Apple IIGS)
- Emplacement : `src/noyau/bibliotheque.c:211-228`, `:267-268`
- Constat : `catalogue_chercher` fait un `plateforme_fichier_lire` de 16 octets par entrée ; `inscrire` fait deux passes (le nom, puis la première entrée libre). Avec N entrées : RANGER ≈ 3N lectures, GARER ou CHARGER ou SUPPRIMER sur un fichier permanent N lectures par instruction (droit ou protection), y compris dans une boucle L.S.E.
- Estimation : N = 50 → 150 lectures par RANGER, 50 par GARER `'#…'` ; lu par blocs de 16 entrées, 10 et 4.
- Référence : IMP-0248 (« une lecture séquentielle pour une recherche »), IMP-0255.
- Proposition : lire par blocs de 256 octets comme `bibliotheque_catalogue_suivant` (`:358-360`) ; une seule passe pour le nom et la première entrée libre. Gain : nombre d'appels divisé par environ 16 ; risque faible ; format inchangé.
- Décision IMP nécessaire : non.
- Essai : compteurs, « -- 5 » et « -- 7 ».

### G4-P-03 — Le quota temporaire relit tout l'espace temporaire à chaque GARER
- Gravité : mineur à majeur selon le coût GS/OS, non mesuré
- Statut : PLAUSIBLE (compté ; manque la durée d'un `OpenGS`/`GetDirEntryGS`/`CloseGS` et d'un `fopen`)
- Emplacement : `src/noyau/fichiers.c:235-250`, `:302`, `:349`
- Constat : chaque GARER sur un fichier temporaire appelle `fichiers_secteurs_temporaires` : (T+1) parcours de répertoire (3 appels GS/OS chacun) et T ouvertures de fichier, même quand l'écriture n'agrandit rien (`croissance` = 0) ; deux fois pour un fichier créé.
- Estimation : avec 2 temporaires, 2 des 3 ouvertures et les 3 parcours d'un GARER (9 appels GS/OS sur environ 20) ne servent qu'au quota ; DECODER de 200 lignes : 25 parcours, 13 ouvertures de quota.
- Référence : IMP-0253 (« une écriture qui le ferait dépasser donne EX 52 » ; « le contrôle de l'espace temporaire parcourt les fichiers temporaires »).
- Proposition : ne compter que si `croissance > 0` (conforme à la lettre d'IMP-0253, mais change le cas d'un espace déjà dépassé : point à arbitrer n° 4) ; ou tenir un total de session mis à jour par `fichiers.c` et recalculé aux événements qui vident l'espace (risque de désaccord). Mesurer d'abord les appels GS/OS par la sonde `SONDEF`.
- Décision IMP nécessaire : oui (précision d'IMP-0253) selon la variante.
- Essai : compteurs, « -- 6 » et « -- 7 ».

### G4-P-04 — DECODER rouvre et reparcourt le fichier pour chaque enregistrement
- Gravité : mineur (commande ponctuelle)
- Statut : PLAUSIBLE (compté ; durée GS/OS non mesurée)
- Emplacement : `src/noyau/codage.c:31-36`
- Constat : chaque enregistrement de 252 caractères passe par `fichiers_garer` : ouverture, secteur 0, répertoire, quota (G4-P-03), écriture, fermeture.
- Estimation : DECODER de 200 lignes : 25 ouvertures, 47 lectures, 37 écritures avec `fflush`, 25 parcours de répertoire, pour 12 enregistrements.
- Référence : IMP-0260.
- Proposition : ouvrir une fois, contrôler le quota une fois sur la taille finale, ajouter les enregistrements en fin, écrire le répertoire à la fin ; ou, plus simplement, ne traiter que G4-P-03.
- Décision IMP nécessaire : non.
- Essai : compteurs, « -- 6 ».

### G4-P-05 — GARER écrit l'étendue secteur par secteur, avec un `fflush` chacun
- Gravité : mineur
- Statut : PLAUSIBLE (lecture ; durée d'une écriture GS/OS non mesurée)
- Emplacement : `src/noyau/fichiers.c:305-310` (et `plateforme_fichier_ecrire`, `fichiers_iigs.c`, `fflush` par appel)
- Constat : un tableau de 20 000 éléments (≈ 80 Ko) donne 313 écritures de 256 octets, chacune suivie d'un `fflush`.
- Proposition : écrire directement depuis `contenu` par blocs de 4 096 octets, en complétant seulement le dernier secteur (comme `charger_ouvert` lit par 4 096). Appels divisés par 16 ; format inchangé ; risque faible.
- Décision IMP nécessaire : non.
- Essai : aucun.

---

## 4. Points écartés après vérification

- `objets_copier` : `debut + n` calculé en `unsigned long` (`objets.c:276`, `:287`), pas de débordement.
- `objets_creer` : conversion `unsigned long` → `unsigned int` faite seulement après le test du seuil de 3 833 octets (`objets.c:192-195`).
- Bornes de `table_blocs`, `table_hors_norme`, `slots` : boucles sur `unsigned char` qui se terminent (`:133`, `:164`, `:377`, `:383`) ; `i < nb_slots ≤ 64` contrôlé partout ; l'indice hors-norme 255 n'existe que dans `GESTOBJ_HANDLE_NUL`, écarté en tête.
- Comptabilité de `limite_donnees` / `octets_libres` (`objets_liberer`, `compacter`) : pas de double compte ; la compaction rend bien la place promise (IMP-0160).
- RCI-S : longueur de séquence ≤ 255 et de constante ≤ 127 tiennent sur un octet ; chaque lecture de `verifier` est précédée d'un contrôle de borne ; fichier ouvert fermé et tampon libéré sur tous les chemins de RANGER, APPELER, DUPLIQUER (`copier`).
- Noms : `inscrire` et `entree_lue` bornent le nom à 5 caractères ; `lu_nom[8]` (`codage.c:138`) reçoit un nom validé (`moniteur.c:524-540`).
- DECODER : `n1`, `n2` convertis en `unsigned char` après `numero`, qui borne à 255 (`moniteur.c:278`).
- Tampon de DECODER : `rempli < 252` avant chaque écriture (`ajouter_mitra`, `ajouter_t1600`).
- Troncature silencieuse de `codage_ligne` et `ruban_lire_ligne` : les appelants passent 255 et 256 octets, au moins la longueur maximale restituée.
- ENCODER, PERFORER : `codage_fermer` et `ruban_perforer_fermer` appelés sur tous les chemins, ESC compris (`moniteur.c:1419`, `:462`).
- `taches.c` : transitions conformes à IMP-0269 ; `lire_entree` suspend et réactive sur toutes ses sorties (`moniteur.c:206-237`).
- Boucle du répertoire dans `fichiers_cataloguer` (`fichiers.c:486-497`) sans garde de cycle : seul un fichier temporaire déjà corrompu la ferait boucler (spéculatif) ; `parcourir` a sa garde (`:208`).
- Longueur d'enregistrement non comparée à `nb × 256` dans `charger_ouvert` : seul un fichier corrompu est concerné, et la lecture échoue en fin de fichier (spéculatif).
- `besoin` en `unsigned int` (`fichiers.c:259`) : un tableau fait au plus 270 Ko, soit 1 057 secteurs.

---

## 5. Pistes hors périmètre

- **P-1 — GARER d'un tableau de plus de 65 535 octets d'image : enregistrement corrompu.** `interprete.c:889-891` : `objets_copier(…, (unsigned int) lg)` tronque la longueur (`objets_copier` prend un `n` de 16 bits) ; `fichiers_garer` écrit ensuite `lg` octets dont le reste est non initialisé. Un tableau de plus d'environ 15 400 éléments suffit (`ELEMENTS_MAX` = 65 535, `donnees.c:26`). **CONFIRMÉ par essai** (`essai_g4.c` §9) : `TABLEAU T[20000]`, GARER puis CHARGER → `U[1]` = 5 juste, `U[16000]` et `U[20000]` = 0 au lieu de 9 et 7. Gravité : bloquant (perte de données silencieuse). Correction : copier par tranches, ou résoudre l'objet.
- **P-2 — Le décompilateur emploie ≈ 2 Kio de pile** : `decompilateur.c:85-92`, `:1028` (`Decompilation d` : `pile[128]` de 12 octets = 1 536 o) et `:672` (`s[256]`) ; mesuré 2 034 o. Premier consommateur de pile de LISTER, DECODER, PERFORER (G4-A-05).
- **P-3 — Constantes partagées et symboles jamais retirés** avant `programme_vider` (`symboles.c:120-160`, `programme.c:280-291`) : RCI-S range des constantes mortes (cause de G4-A-02) ; `constantes_chercher_ou_creer` résout chaque constante de même longueur (`symboles.c:105-117`), d'où O(nc²/≈33) résolutions à APPELER (≈ 15 000 résolutions, ≈ 16 s, pour 1 000 constantes).
- **P-4 — `assurer`, `contexte_exec.c:139-141`** : `n = id + 1` vaut 0 pour `id` = 0xFFFF → `realloc(noms, 0)` puis `noms[id]` hors bornes ; atteignable par un RCI-S altéré (G4-A-03).
- **P-5 — Résolutions évitables** : `donnees.c:613-654` (rang et lecture d'élément : deux résolutions par `T[I]`), `contexte_exec.c:716` (un appel de procédure), `programme.c:82` (`decoder_declaration`, à chaque ligne insérée) pourraient employer `objets_copier` ; s'ajoute à G4-P-01.
- **P-6 — Échec d'ouverture assimilé à l'absence** : `fichiers_iigs.c:118-119` rend `PLAT_INEXISTANT` pour tout échec de `fopen` en lecture ou mise à jour ; `fichiers.c:347-350`, `bibliotheque.c:144-145` et `:230-235` tentent alors une création (`w+b`, qui vide le fichier). Seul le cas « `r+b` refusé, `w+b` accepté » serait destructeur : à vérifier sur GS/OS (fichier occupé, droits d'accès).
- **P-7 — Construction** : aucun `#pragma stacksize` (`main.c`) ; aucune décision du Tome V sur la pile du 65816 (G4-A-05).
- **P-8 — Mesures du 2026-10-25** : « deux couples » pour `objets_resoudre` + `objets_relacher` ; l'essai en compte trois (G4-A-10).
- **P-9 — Longueur des lignes restituées** : le décompilateur dépasse la borne d'entrée de 79 caractères (`1E5` → `100000`) ; même effet sur LISTER puis ressaisie (G4-A-04).

---

## 6. Points à arbitrer par Éric

1. **DECODER / ENCODER au-delà de 79 caractères** (G4-A-04) : ENCODER (et le ruban) compilent-ils avec la borne du décompilateur (255), ou la restitution des nombres doit-elle conserver la longueur de la source ? Essai proposé sur LSE 8086 : taper `1 X_1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1E5+1`, LISTER (la ligne apparaît-elle avec `100000` ?), puis DECODER et ENCODER.
2. **Découpe des étendues libres** (G4-A-07) : IMP-0247 ne dit pas si une étendue libre plus grande que le besoin est découpée.
3. **Accès sans verrou** (G4-P-01) : étendre IMP-0243 à `objets_existe`, `objets_creer` et `objets_liberer` ; et, au-delà, retirer le verrou de `objets_resoudre` (la règle du §11 suffisant) ?
4. **Quota quand l'écriture n'agrandit rien** (G4-P-03) : refuser un GARER sans croissance quand l'espace temporaire est déjà dépassé (code actuel), ou l'accepter (lettre d'IMP-0253) ?
5. **Taille de la pile** (G4-A-05) : valeur de `#pragma stacksize` (8 ou 16 Kio), à fixer après mesure sur l'Apple IIGS.
6. **Constantes et symboles morts dans RCI-S** (G4-A-02, P-3) : les conserver (aucune renumérotation, IMP-0246) ou les purger, ce qui exigerait de renuméroter les séquences (alternative écartée par IMP-0246) ; EXI-0016 (compacité) est en jeu.
