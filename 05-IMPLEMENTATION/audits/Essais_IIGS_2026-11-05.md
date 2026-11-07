# Essais sur l'Apple IIGS — point 17 (préparés le 2026-11-05)

Ce qui n'a jamais tourné sur la machine depuis le dernier passage (2026-10-30) : le démarrage et la bannière de `LSE F4`, RUB-OUT, les réels de 23 bits, le filtre des ordres à la télétype (IMP-0275), la lecture des rubans de `PERFO.po` et de `DURUY.po`, PRO et PRON2. Pour chaque essai : ce qu'il faut taper, ce qui est attendu, et une case **Constaté** à remplir.

Conventions : `⊗` = fin de saisie, X-OFF (Ctrl-S) ou, sous `LSE F4`, DELETE ; `↵` = RETURN ; Ctrl-Q = X-ON, mise en route du lecteur de ruban.

## 0. Préparation

Images, toutes construites le 2026-11-05 en profil `cible` :

| Image | Volume | Contenu |
|---|---|---|
| `build/LSE.po` | `/LSE/` | `LSE` (247 983 octets ; 247 925 après la correction de 3.6), `SONDE`, `SONDEF`, `SONDEE`, `PERF` (mesure aussi l'arrondi à 23 bits), `NOMBRES` |
| `build/PERFO.po` | `/PERFO/` | rubans `CHARG`, `PERFO`, `PRONO`, `FICH`, `FICH2` |
| `build/DURUY.po` | `/DURUY/` | 14 rubans : ANGLO, CARTO, CARTOV1, DE31, GA, MORSE, MULT, NBOR, PA1, PRO, PRON2, PUISS, SINUS, SPHIN |

Tous les essais emploient une bibliothèque neuve, créée au premier lancement : `-BIB @:ESSAI17`. Elle ne touche pas à `@:LSE`.

## 1. Démarrage de `LSE F4` (Moniteur IMP-0279, IMP-0228, IMP-0266)

| # | Taper | Attendu | Constaté |
|---|---|---|---|
| 1.1 | `LSE -BIB @:ESSAI17` | `LSE F4`, puis `DATE HEURES MINUTES :` | OK |
| 1.2 | `↵` seul | `EN LIGNE....` ; l'horloge reste celle de l'Apple IIGS | OK |
| 1.3 | `BO⊗` | `BONJOUR jj/mm/aa hh:mm:ss`, **une seule** espace avant la date, puis `CONSOLE NO. 01.`, `LSE F4`, `RUB-OUT REMPLACE CTRL X-OFF`, `VOUS ETES SOUS ID 1`, puis cinq lignes blanches | OK |
| 1.4 | `BO⊗` (en session) | `????` sur la ligne suivante, sans compléter `NJOUR` | OK |
| 1.5 | `AU⊗` | `AU REVOIR jj/mm/aa hh:mm:ss`, puis la même bannière | OK |
| 1.6 | relancer, répondre `15/10/80 08:36↵`, puis `BO⊗` | `BONJOUR 15/10/80 08:36:0x` | OK |
| 1.7 | `?DAT()⊗` puis `?TEM()⊗` | `15/10/80 08:36:xx` ; environ 31 000 secondes | OK |
| 1.8 | `AU⊗`, relancer, répondre `00/00/00 00:00:00↵`, `BO⊗` | `BONJOUR  00:00:xx` : champ de date vide, deux espaces | OK |
| 1.9 | quitter `LSE`, regarder l'heure du tableau de bord | inchangée : la date saisie ne vit que dans `LSE` | OK |

## 2. RUB-OUT (Moniteur IMP-0238, IMP-0240)

| # | Taper | Attendu | Constaté |
|---|---|---|---|
| 2.1 | sous `LSE F4` : `?2+3` puis DELETE | `5` : DELETE termine la saisie comme Ctrl-S | OK |
| 2.2 | `?2+3` puis Ctrl-S | `5` | OK |
| 2.3 | `LSE -T1600 -BIB @:ESSAI17`, `BO⊗`, `?2+3` puis DELETE | rien : DELETE est ignoré ; Ctrl-S termine | OK |

## 3. Réels de 23 bits (Interpréteur IMP-0280)

Sous `LSE F4`, après `BO` :

| # | Taper | Attendu | Constaté |
|---|---|---|---|
| 3.1 | `?82/10⊗` | `8.200001` (INRDP75, PG16) | OK |
| 3.2 | `?0.1,8.3⊗` | `0.09999999 8.299999` | OK |
| 3.3 | `?11/(23/20)⊗` | `9.565216` (dossier EPI82) | OK |
| 3.4 | `A_8388609;?A⊗` | `8.38861E+06`, soit 8 388 610 : sous `U`, un nombre de 10⁶ ou plus s'imprime avec exposant (IMP-0211) ; l'attendu `8388610` de la première rédaction était faux | `8.38861E+06` — conforme |
| 3.5 | `LSE -MITRA15 -ID 1 -BIB @:ESSAI17`, `BO⊗`, `?82/10⊗` puis `?11/(23/20)⊗` | bannière `LSE-MI15  CONSOLE NO.01`, puis `8.2` et `9.565218` : MITRA 15 garde les 24 bits | OK |
| 3.6 | depuis le shell : `PERF` | comparer `donnees_operation_etendue (1.5/3)` et `… (1.5/3), F4` : environ 0,024 ms d'écart ; de même `(0.1+0.2)` et la boucle `(e)`, environ +0,3 % sous F4 | donnees_operation_etendue (1.5/3) et … (1.5/3), F4 : 0,033 ms d'écart (1.200 ms -> 1.233 ms), (0.1+0.2) : 0,067 ms d'écart. Boucle (e) : +1,1 % |
| 3.7 | depuis le shell : `NOMBRES` | `0 echec(s)` | OK |

## 4. `PERFO.po` et le filtre des ordres à la télétype (IMP-0274, IMP-0275)

Lecture des rubans, dans cet ordre, `CHARG` d'abord (les rubans de données l'appellent) ; chaque fois : `BO⊗`, Ctrl-Q sur une ligne vide, `AU⊗`.

| # | Taper | Attendu | Constaté |
|---|---|---|---|
| 4.1 | `LSE -BIB @:ESSAI17 -RUBAN /PERFO/CHARG` | le programme s'imprime ligne à ligne ; le ruban se termine par le rangement de CHARG (`RA`) et l'effacement du programme (`EF *`) ; aucune erreur | OK |
| 4.2 | idem `/PERFO/PERFO` | idem pour PERFO | OK |
| 4.3 | idem `/PERFO/PRONO`, `/PERFO/FICH`, `/PERFO/FICH2` | chacun appelle CHARG et crée le fichier-données ; aucune erreur | OK |
| 4.4 | `LSE -BIB @:ESSAI17`, `BO⊗`, `?'AB'!EQC(17)!EQC(19)!EQC(21)!' CD'⊗` | `AB CD` ; l'écran **n'est pas effacé** (`$13`) et reste en 80 colonnes (`$11`) | OK |
| 4.5 | `AP⊗PERFO⊗`, `EX⊗1⊗`, `PRONO⊗`, `NON⊗`, `1 9⊗` | la sortie `0042` de PERFO (`02-HISTORIQUE/Reconstitution-PERFO-CHARG/sorties/0042.txt`), `APCHARGEX1PRONO` compris, puis `TERMINE EN LIGNE 222` | OK |
| 4.6 | même chose avec `OUI⊗` | même impression ; l'écran n'est ni effacé ni passé en 40 colonnes ; le fichier `RUBAN` de `@:ESSAI17` reçoit la perforation | OK |

## 5. `DURUY.po` : les programmes de Victor Duruy

Pour chaque programme : `LSE -BIB @:ESSAI17 -RUBAN /DURUY/NOM`, `BO⊗`, Ctrl-Q sur une ligne vide (le programme s'imprime), puis `EX⊗1⊗` et les réponses de la feuille. Les sorties attendues sont transcrites dans `02-HISTORIQUE/Transcriptions-T1600-Victor-Duruy/executions/` : la frappe y est entre accolades, et la mise en page est celle de la télétype (72 colonnes).

| # | Programme | Réponses | Attendu | Constaté |
|---|---|---|---|---|
| 5.1 | NBOR | `7` | 14 valeurs, la première `3.645751` (la feuille : `3.645752`), puis `14  OPERATIONS.` | OK |
| 5.2 | PUISS | `4`, `6` | `LE RESULTAT EST 4096` | OK |
| 5.3 | MULT | `12`, `7`, `7`, `15`, `14`, `21`, `28`, `35`, `43`, `42`, `49`, `56`, `63`, `NON` | `MULT_0005.txt` ; bips (`.7.`) après chaque erreur (lignes 16 et 28) et onze pendant ` AU REVOIR` (ligne 50) | OK |
| 5.4 | SPHIN | `800`, `1515`, `TEHERAN`, `MOSCOU` | `SPHIN_0040.txt` | OK |
| 5.5 | ANGLO | voir `ANGLO_0039.txt`, dont `DIDI\` (la barre efface le `I`) et `CO` à la pause | `PAUSE EN LIGNE 161`, `CONTINUER`, `VOICI TA NOTE SUR 10: 7.5` | OK |
| 5.6 | CARTO, CARTOV1 | `ERIC LE BRAS`, `INFORMATICIEN`, l'adresse, `G` | `CARTO_0037.txt`, `CARTOV1_0009.txt` | OK |

## 6. PRO et PRON2 (dossier EPI82)

PRO enchaîne sur PRON2 par `EXECUTER 'PRON2'` : ranger d'abord PRON2 dans la bibliothèque de l'essai 4, qui contient déjà FICH et PRONO.

| # | Taper | Attendu | Constaté |
|---|---|---|---|
| 6.1 | `LSE -BIB @:ESSAI17 -RUBAN /DURUY/PRON2`, `BO⊗`, Ctrl-Q, `RA⊗PRON2⊗`, `AU⊗` | PRON2 rangé, aucune erreur | OK |
| 6.2 | `LSE -BIB @:ESSAI17 -RUBAN /DURUY/PRO`, `BO⊗`, Ctrl-Q, `EX⊗1⊗` | début de PRO : titre, `NOM DU FICHIER TEXTE :` | OK |
| 6.3 | `FICH`, `2`, `PERSONELS`, `PERSONNELS`, `RELATIFS`, `DEMONSTRATIFS`, `FIN` | le texte de Michelet, `TES CONNAISSANCES (?) NECESSITENT…`, `CETTE CATEGORIE N"EXISTE QUE DANS VOTRE IMAGINATION`, puis `CORRECTION:` et `PAUSE EN LIGNE 038` | OK |
| 6.4 | `CO`, `OUI`, `PERSONNELS`, `INTERROGATIFS`, `OUI`, `POSSESSIFS`, `OUI`, `REFLECHIS`, `NON` | `ERREUR !!!`, `NON`, les tableaux des possessifs et des réfléchis, puis `...ET VOICI TA NOTE PROVISOIRE : 3.75 /20` (dossier : `3.125`, écart consigné) | OK |
| 6.5 | PRON2 démarre : `FICH`, puis `CO` terminé par **DELETE** (`TAPE CO PUIS LA TOUCHE RUB-OUT.`) | consignes de PRON2, puis le texte | OK |
| 6.6 | `QUICONQUE VOIT`, `SES PREMIERS`, `UN DE`, `QUI LA`, `CE QU`, `ON PEUT`, `IL NE`, `QUI FACILITENT`, `QUI EST`, `ELLE FAIT`, `FIN`, `OUI`, `INTERROGATIFS`, `NON`, `OUI`, `ELLE EN`, `EN PRENDRA`, `FIN`, `NON`, `NON` | `REPONSE ERRONNEE.` après `SES PREMIERS` ; le tableau des interrogatifs ; `VOICI LA LISTE COMPLETE :` ; **`...ET VOICI TA NOTE : 9.565216 /20`** ; `TERMINE EN LIGNE 095` | OK |

## 7. Autres constructions de `LSE F4`, à la machine de bureau

| # | Taper | Attendu | Constaté |
|---|---|---|---|
| 7.1 | `10 Q_TEM()⊗`, `20 FAIRE 20 TANT QUE TEM()-Q<4⊗`, `30 AFFICHER TEM()-Q⊗`, `40 TERMINER⊗`, `EX⊗10⊗` | environ quatre secondes d'attente, puis `4` (SINUS, ligne 105) | OK |
| 7.2 | `?'A'.7.'B'⊗` | `AB` et un bip | OK |
| 7.3 | `PR⊗1⊗`, puis `PR⊗3⊗` | rien ; puis `????` (une seule console, IMP-0236) | OK |

## 8. Flèches à l'écran (Moniteur IMP-0281), ajouté le 2026-11-05 après les essais

Image `build/LSE.po` reconstruite le 2026-11-05 (`LSE` 248 330 octets). Golden Gate ne fournit pas le pilote `.CONSOLE` : cet essai n'a jamais tourné.

| # | Taper | Attendu | Constaté |
|---|---|---|---|
| 8.1 | `LSE -BIB @:ESSAI17`, `↵`, `BO⊗`, puis taper `?'A_B^C'` sans terminer | l'écho montre `?'A←B↑C'` au fur et à mesure | OK |
| 8.2 | `⊗` | `A←B↑C` | OK |
| 8.3 | `10 X_2^3⊗`, `LI⊗1⊗` | `LISTER A PARTIR DE 1`, puis `10 X←2↑3` | OK |
| 8.4 | `?EQC(95)!EQC(94)⊗` | `←↑` | OK |
| 8.5 | `AU⊗`, relancer `LSE -MITRA15 -BIB @:ESSAI17`, `BO⊗`, `?EQC(200)⊗` | `H` en vidéo inverse, et non une flèche : MouseText n'est pas resté activé | Affiche la flèche gauche |
| 8.6 | `AU⊗`, puis taper `_` et `^` dans le shell ORCA | `_` et `^` : le shell n'est pas touché | OK |

Vérifié sous Golden Gate le 2026-11-05 (repli stdio, donc `_` et `^`) : 8.2 à 8.5 donnent `A_B^C`, `10 X_2^3`, `_^` et l'octet `$C8`.

Si un autre signe que `←` ou `↑` apparaît en 8.1, noter lequel : la place des flèches dans MouseText (`$48`, `$4B`) vient de l'Apple IIe amélioré, aucun document versé ne la donne.

**Analyse de 8.5 (2026-11-05).** L'attendu était faux. `EQC(200)` écrit l'octet `$C8` tel quel, sans la traduction des flèches et sans `$1B` : la flèche gauche ne vient donc pas d'un MouseText resté activé. En mode normal, le pilote affiche toujours `$C0` à `$DF` en MouseText, comme le dit le tableau 9-3 de la GS/OS Reference, chap. 9. L'activation (`$1B`) ne règle que les majuscules en vidéo inverse. Ce comportement existe depuis IMP-0240, indépendamment d'IMP-0281. Conséquence : l'encadrement `$1B`…`$18` était inutile. IMP-0281 est simplifiée : `_` est écrit `$C8` et `^` `$CB`, un octet chacun. Image `build/LSE.po` reconstruite (`LSE` 248 283 octets) ; 30 suites, profils `dev` et `cible`.

| # | Taper | Attendu | Constaté |
|---|---|---|---|
| 8.7 | refaire 8.1 et 8.2 avec la nouvelle image | `?'A←B↑C'` à l'écho, puis `A←B↑C` | OK |
| 8.8 | refaire 8.3 et 8.4 | `10 X←2↑3` ; `←↑` | OK |
| 8.9 | `?'ABC'⊗`, puis `AU⊗` et `_^` dans le shell ORCA | `ABC` en majuscules normales ; `_^` dans le shell | OK |

**Console à 7 bits (Bibliothécaire et fichiers IMP-0282, choix d'Éric du 2026-11-05, option b).** Un octet de `$80` à `$FF` imprimé par un programme va à la console réduit à ses 7 bits, sauf s'il devient un ordre `$11` à `$15` (IMP-0275). Image `build/LSE.po` reconstruite (`LSE` 248 439 octets) ; 30 suites, profils `dev` et `cible` ; les deux vérificateurs passent. Sous Golden Gate, 8.10 à 8.12 donnent `H`, `A_^B` et `AB`.

| # | Taper | Attendu | Constaté |
|---|---|---|---|
| 8.10 | `LSE -MITRA15 -BIB @:ESSAI17`, `BO⊗`, `?EQC(200)⊗` | `H` en vidéo normale | OK |
| 8.11 | `?'A'!EQC(223)!EQC(222)!'B'⊗` | `A←↑B` | OK |
| 8.12 | `?'A'!EQC(147)!'B'⊗` | `AB` ; l'écran n'est pas effacé (`$93` réduit à `$13` n'est pas écrit) | OK |

Section 8 close le 2026-11-05 : IMP-0281 et IMP-0282 vérifiées sur l'Apple IIGS.

## Résultats (Éric, 2026-11-05)

Tous les essais conformes, sauf :
- **3.4** : l'attendu était faux (voir la ligne) ; `LSE` est conforme ;
- **3.6** : l'arrondi coûte 0,433 ms par opération, et non 0,024 ms ; la boucle (e) passe de 7,250 à 7,667 ms par tour (+5,8 %). L'estimation venait de `iix --cycles`, qui ne compte pas les appels à SANE ; or la branche T1600 comparait le résultat arrondi à `FLT_MAX` par deux comparaisons en format étendu. Corrigé le 2026-11-05 : le dépassement se reconnaît aux bits du `float` obtenu (`ranger_23`, `donnees.c`), comme sur le chemin de MITRA 15 ; 30 suites, profils `dev` et `cible`, et les deux vérificateurs passent ; image `LSE.po` reconstruite (`LSE` 247 925 octets). Remesuré par Éric le 2026-11-05 : `1.5/3` 1,200 → 1,233 ms (+0,033), `0.1+0.2` +0,067 ms, boucle (e) +1,1 %. La sonde chronomètre 500 opérations au 1/60 de seconde, soit 0,033 ms par tic : ces écarts font un et deux tics, cohérents avec les 0,024 ms de l'arrondi.

## Après les essais

Rapporter la colonne **Constaté**, et l'impression de `PERF` (essai 3.6). Les écarts seront traités point par point.

Vérifié sous Golden Gate le 2026-11-05, avec le même `LSE` : 1.4, 1.8, 3.1 à 3.5, 4.4, 4.5, 5.1 à 5.6, 6.1 à 6.6, 7.1 à 7.3 (sauf l'effet sur l'écran et les bips, propres à la machine). Golden Gate ne fournit pas le pilote `.CONSOLE` : RUB-OUT (2), l'effacement par `$13` (4.4, 4.6), la lecture de `/PERFO/` et `/DURUY/` et l'horloge réelle n'y sont pas éprouvés.
