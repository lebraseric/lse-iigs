# Essais — périphérique de la bibliothèque (préparés le 2026-11-06)

## Constat

Au premier UTILISATION DISQUE d'une session, `LSE` accède au disque dur, puis aux lecteurs 5,25", pendant plusieurs secondes, avant d'afficher la liste. Les UTILISATION DISQUE suivants de la même session ne touchent pas aux lecteurs. TABLE DES FICHIERS répond immédiatement. Constaté par Éric le 2026-11-06, sur l'Apple IIGS et sous l'émulateur GSSquared, de façon sensiblement identique.

Cause, d'après le code :

- UTILISATION DISQUE imprime `ESPACE PERMANENT ( n SECT.) OCCUPE A p %`. Le total vaut l'occupé plus les secteurs libres du volume (IMP-0257).
- Seul GS/OS connaît ces secteurs libres. `plateforme_secteurs_libres` (`src/plateforme/fichiers_iigs.c`) les demande à l'appel Volume ($2008), qui exige un nom de périphérique (erreur $0040 avec un nom de volume, `SONDEF V`, 2026-10-25).
- La fonction parcourt donc les périphériques 1, 2, 3… : DInfo ($202C) donne le nom, Volume lit le support, jusqu'à trouver le volume de la bibliothèque. Le numéro trouvé est retenu pour la session.
- Sur un lecteur 5,25", Volume démarre le moteur et insiste jusqu'à l'échec quand le lecteur est vide.
- TABLE DES FICHIERS n'imprime que `ESPACE TEMPORAIRE( n SECT.)`, dont le total est le quota `-TEMP`, connu de LSE. Elle n'appelle pas Volume.

## Ce que la sonde doit établir

L'appel GetDevNumber ($2020), déjà employé pour `.CONSOLE` (`console_iigs.c`), rendrait directement le numéro du périphérique qui porte un volume donné. Ce comportement est à vérifier : l'extrait du *GS/OS Reference* versé dans `03-APPLE-IIGS/systeme/` ne couvre pas cet appel. Si GetDevNumber l'accepte, il ne reste qu'un DInfo et un Volume, sur le bon périphérique, sans toucher aux lecteurs de disquettes.

La sonde `SONDEF G [chemin]` (`tests/plateforme/sonde_fichiers.c`) fait l'essai de ces appels, et seulement de ceux-là : le mode G ne fait pas l'essai des fichiers. Chaque étape est annoncée à l'écran **avant** son appel, puis chronométrée (`clock()`, 1/60 s). On peut ainsi rattacher l'activité d'un lecteur à l'étape qui la cause.

Golden Gate n'implémente pas DInfo (« Unsupported GS/OS call: 202c », 2026-11-06) : la sonde ne s'essaie que sur l'Apple IIGS ou sous un émulateur qui fait tourner GS/OS.

## 0. Préparation

- Image : `build/LSE.po` (volume `/LSE/`), reconstruite le 2026-11-06 en profil `cible`. Seul `SONDEF` a changé ; `LSE` reste à 248 439 octets.
- Copier `SONDEF` sur le disque dur, à côté de `LSE`, et le lancer depuis là, comme `LSE`.
- `chemin` : celui de la bibliothèque employée avec `LSE`, c'est-à-dire l'argument de `-BIB`, ou `@:LSE` sans `-BIB`.
- Laisser les lecteurs 5,25" dans l'état où ils étaient au constat (vides ou non).
- Garder la sortie complète de l'écran (photo ou recopie) : la liste des périphériques de l'étape 2 servira à la décision.

## 1. Essais

| # | Étape annoncée à l'écran | Attendu | Constaté |
|---|---|---|---|
| 1.1 | `SONDEF G chemin` | `SONDE PERIPHERIQUES`, puis `ExpandPath … : erreur $0000` et le chemin complet `:VOL:…` | OK chemin complet : :BYTEWORKS:ORCA:LSE |
| 1.2 | `DInfo 1 a 32 ...` | Une ligne par périphérique (numéro, nom, caractéristiques, blocs, emplacement, unité, type), puis `erreur $0011, fin`. **Aucun lecteur ne s'allume** ; durée voisine de 0 | OK |
| 1.3 | `GetDevNumber :VOL ...` | `erreur $0000` et le numéro du périphérique qui porte la bibliothèque (à retrouver dans la liste de 1.2). **Aucun lecteur 5,25" ne tourne** ; durée voisine de 0 | `erreur $0000, numero 7` |
| 1.4 | `GetDevNumber VOL ...` (sans `:`) | À constater : nom de volume refusé, ou pris pour un nom de périphérique | OK |
| 1.5 | `GetDevNumber :VOL:… ...` (chemin complet) | À constater | `erreur $0000, numero 7` |
| 1.6 | `DInfo et Volume sur le numero obtenu ...` | Nom du volume = `VOL` ; `secteurs libres : n` ; seul le disque dur s'allume | OK n = 9220 |
| 1.7 | `GetDevNumber :ZZSONDEABSENT ...` | Volume absent : `$0045` probable. **Les lecteurs peuvent tourner** : noter lesquels et la durée | les 2 lecteurs 5,25" alternativement, 4 fois chacun, total = 45 s |
| 1.8 | Lancer `LSE` avec la même bibliothèque, `BO⊗`, `UT⊗` | Le premier nombre d'`ESPACE PERMANENT (` vaut l'occupé plus le `n` de 1.6 (à quelques secteurs près si un fichier a changé entre-temps) | `9336 SECT.` |

`⊗` = fin de saisie (Ctrl-S ou, sous `LSE F4`, DELETE).

## 2. Suite selon le résultat

- **1.3 ou 1.4 rend le bon numéro sans faire tourner les lecteurs** : remplacer le parcours de `plateforme_secteurs_libres` par GetDevNumber, puis DInfo et Volume sur ce seul numéro. La forme retenue du nom (avec ou sans `:`) sera celle qui réussit. IMP-0257 serait complétée en ce sens.
- **GetDevNumber refuse les noms de volume** : garder le parcours, mais écarter les lecteurs 5,25", reconnus à leur type (`deviceID`) dans la liste de 1.2. Le type exact sera relevé sur la machine.
- **1.7** dit ce qui arrive si le volume de la bibliothèque est absent. En session, ce cas ne devrait pas se présenter (F 102 est contrôlé avant toute opération de fichiers, IMP-0250) ; il reste à connaître.

## 3. Résultats (essai d'Éric, 2026-11-06)

L'essai a tourné sous l'émulateur GSSquared, pas sur l'Apple IIGS : `SONDEF` n'y a pas été lancée. Les appels essayés sont ceux de GS/OS, que l'émulateur exécute ; la liste des périphériques et les durées sont celles de la configuration de GSSquared.

Sortie de la sonde relevée dans `/BYTEWORKS/ORCA/SONDEF.OUT` (image `Byteworks.po`), recopiée dans [Essais_IIGS_2026-11-06/SONDEF.OUT.txt](Essais_IIGS_2026-11-06/SONDEF.OUT.txt). La bibliothèque est `@:LSE` = `:BYTEWORKS:ORCA:LSE`.

**Liste des périphériques (1.2).** Aucun lecteur sollicité. La durée relevée (135 tops, 2 250 ms) couvre aussi l'affichage des 32 lignes. La sonde s'arrête à 32 ; d'autres périphériques peuvent suivre.

| N° | Nom | Type (`deviceID`) | Emplacement, unité | Blocs |
|---|---|---|---|---|
| 1 | `.DEV1` | $0002 | 15, 1 | 65 532 |
| 2 | `.APPLEDISK3.5A` | $0003 | 5, 1 | 1 600 |
| 3 | `.APPLEDISK3.5B` | $0003 | 5, 2 | 1 600 |
| 4 | `.APPLEDISK5.25A` | $0000 | 6, 1 | 280 |
| 5 | `.APPLEDISK5.25B` | $0000 | 6, 2 | 280 |
| 6 | `.CONSOLE` | $000A | 32771, 1 | 0 |
| 7 | `.DEV2` (volume `BYTEWORKS`) | $0002 | 15, 2 | 65 535 |
| 8 à 32 | `.DEV3` à `.DEV27` | $0002 | 15, 3 à 27 | 65 535 |

Le volume de la bibliothèque est sur le périphérique 7. Le parcours de `plateforme_secteurs_libres` appelle donc Volume sur `.DEV1` (le disque dur), puis sur les lecteurs 3,5" et 5,25", puis sur `.CONSOLE`, avant d'atteindre `.DEV2`. C'est la séquence constatée : disque dur, puis lecteurs 5,25".

**GetDevNumber (1.3 à 1.5).**

| Nom | Erreur | Numéro | Durée |
|---|---|---|---|
| `:BYTEWORKS` | $0000 | 7 | 0 |
| `BYTEWORKS` | $0040 (syntaxe du chemin) | 0 | 1 top |
| `:BYTEWORKS:ORCA:LSE` | $0000 | 7 | 0 |

GetDevNumber accepte un nom de volume précédé de `:`, et aussi un chemin complet. Il refuse un nom sans `:`. Aucun lecteur n'est sollicité.

**DInfo et Volume sur le numéro 7 (1.6).** Durée 0. `.DEV2`, volume `:ByteWorks` (en minuscules, alors qu'ExpandPath rend `BYTEWORKS`), 65 535 blocs de 512 octets, 4 610 libres, soit 9 220 secteurs de 256 octets.

**Volume absent (1.7).** `$0045` (volume introuvable) après 1 794 tops (29,9 s). Les deux lecteurs 5,25" ont tourné tour à tour, quatre fois chacun ; Éric a mesuré 45 s en tout. L'écart vient probablement de ce que le compteur de `clock()` (interruption de 1/60 s) n'avance pas pendant les accès aux lecteurs 5,25", qui masquent les interruptions. GS/OS cherche donc le volume absent sur les lecteurs de disquettes, comme le faisait le parcours.

**UTILISATION DISQUE (1.8).** `ESPACE PERMANENT ( 9336 SECT.)` = 116 secteurs occupés + 9 220 libres, comme à l'étape 1.6.

**Conclusion.** Le cas favorable du §2 est atteint : GetDevNumber sur `:VOL` rend le périphérique de la bibliothèque sans toucher aux autres lecteurs.

## 4. Correction (2026-11-06, accord d'Éric)

`plateforme_secteurs_libres` (`src/plateforme/fichiers_iigs.c`) ne parcourt plus les périphériques. Elle obtient par GetDevNumber le numéro du périphérique qui porte `:VOL`, son nom par DInfo, puis appelle Volume sur ce seul périphérique. Elle vérifie encore que le volume lu porte le nom de celui de la bibliothèque (sinon 0 : disque changé entre deux appels). Le numéro n'est plus retenu pour la session : chaque appel est immédiat, et un volume qui change de périphérique est retrouvé sans traitement à part. IMP-0257 complétée (Bibliothécaire et fichiers §7 quater).

Ce que LSE affiche ne change pas. Golden Gate n'exécute pas ces appels (`plateforme_volume_permettre`, seulement avec `.CONSOLE`) ; la vérification se fait sur la machine.

| # | Taper | Attendu | Constaté |
|---|---|---|---|
| 4.1 | lancer `LSE` (nouvelle image), `BO⊗`, `UT⊗` | aucun lecteur de disquettes ne tourne ; `ESPACE PERMANENT (` comme en 1.8, aux fichiers changés près | OK |
| 4.2 | `IDENTIFICATION` d'un compte dont l'espace temporaire dépasse l'espace standard (sous `-MITRA15`, attribut `x` : `NOMBRE DE SECTEURS DISPONIBLES:`) | même nombre de secteurs libres qu'en 4.1, aucun lecteur de disquettes ne tourne | OK |

Essais 4.1 et 4.2 passés par Éric sous GSSquared le 2026-11-06 : conformes. Pour 4.2, l'identification créée en état dormant par `CR 2,P,D,S,>,X` (`>` exigé par UTILISATION DISQUE sous MITRA 15, `X` pour la question des secteurs disponibles), puis `LSE -MITRA15`, `BO`, `ID 2`, `UT`. Les essais restent à refaire sur l'Apple IIGS.
