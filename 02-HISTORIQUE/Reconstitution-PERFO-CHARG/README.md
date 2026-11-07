# Reconstitution de PERFO et CHARG

**Statut :** reconstitution, non transcription. Les programmes originaux, « mis au point et diffusés par la Section Informatique et Enseignement de l'I.N.R.D.P. » (INRDP75, annexe II §4, l. 8614), ne sont pas conservés. Seul leur comportement est attesté : le texte LSE de `PERFO.lse` et de `CHARG.lse` en est une restitution, établie le 2026-10-31, et ne prétend pas retrouver le texte d'origine. `PERFO.lse` compte 55 lignes numérotées jusqu'à 222 ; l'original en comptait sans doute davantage.

**Conventions des fichiers :** celles de `Transcriptions-T1600-Victor-Duruy/README.md` : `←` est écrit `_`, `Ø` imprimé est la lettre O. Une ligne LSE par ligne, 72 colonnes au plus.

## 1. Résultat

Les quatre sorties conservées sont reproduites ligne à ligne, blancs de mise en page compris, jusqu'à `TERMINE EN LIGNE 222`, et le fichier relu par `CHARG` depuis le ruban émis par `PERFO` est identique à l'octet au fichier d'origine.

| Sortie | Fichier | Enr. | Lignes comparées | Ruban | Résultat |
|---|---|---|---|---|---|
| `numerisation0042` | PRONO | 9 | 62 | 902 octets | identique |
| `numerisation0012` | FICH | 8 | 56 | 1 562 octets | identique |
| `numerisation0029` | FICH (incomplet) | 4 | 32 | 1 102 octets | identique |
| `numerisation0011` | FICH2 | 10 | 68 | 2 190 octets | identique |

Ces résultats sont obtenus sous Golden Gate avec le `build/LSE` du dépôt, sous le profil `LSE F4`, qui perfore lui-même le fichier `RUBAN` (§7). Aucun n'a été obtenu sur l'Apple IIGS.

Sont reproduits aussi, sans témoin imprimé : la réponse `NON`, un fichier absent, des enregistrements absents, le profil MITRA 15 (mêmes programmes, ruban identique à l'octet, ruban T1600 lu sous MITRA 15).

## 2. Sources

- Quatre sorties de `PERFO` (`4EME - PRO/`), transcrites dans `sorties/` depuis la ligne `EXECUTER A PARTIR DE 1` ; `0012` et `0029` portent en outre `APPELER PERFO`, et `0029` une séquence de redémarrage du système, non reprises.
- `numerisation0001` : autre listage de PRONO, par un programme non identifié, qui imprime `ENR n TYPE 3 LGR:n`. Il donne la longueur exacte des neuf enregistrements : les neuf longueurs de la transcription concordent. Il coupe les enregistrements 1 et 8 aux mêmes endroits que `PERFO`. Il paraît antérieur à `0042` (enregistrement 9 : `ECT...` au lieu de `ETC...`, même longueur).
- INRDP75 : annexe II §3 et §4 (ruban, X-OFF, nuls, perforateur, FLIBU) ; annexe V (`EQC(17)` à `EQC(21)`) ; 5ᵉ partie §2-3-2 (δ de CHARGER) ; PG62 (lecture d'un fichier quelconque) ; §1-2 et §1-3 (AFFICHER, LIRE).
- `5EME - PUISS/numerisation0006` : position de la sortie après le `1` de `EXECUTER`.

## 3. Le ruban

Tout ce que `PERFO` imprime est perforé, sauf les passages à la ligne ; chaque champ est suivi de X-OFF et de deux nuls, notés `⊗` :

```
AP⊗CHARG⊗EX⊗1⊗PRONO⊗
ENREGISTREMENT 1 3 0 ⊗
INDEFINIS/RELATIFS/…/DEMONSTRATIFS ⊗RELATIFS/PERSONNELS/…/INTERROGATIFS/⊗⊗⊗
ENREGISTREMENT 2 3 0 ⊗
…
& FIN DE FICHIER⊗
CA⊗PRONO,PRONO⊗
```

La présentation sur plusieurs lignes est celle de ce document : le ruban ne contient ni CR ni LF. Un enregistrement occupe toujours son en-tête et quatre champs, éventuellement vides.

## 4. Ce que les sorties imposent

1. **Fin.** `TERMINE EN LIGNE 222` dans les quatre sorties : `TERMINER` est en ligne 222, numéro compatible avec la limite de 250 lignes du MITRA 15 (INRDP75, annexe III §4, l. 8906).

2. **Commandes de tête.** `APCHARGEX1PRONO` s'imprime d'un seul tenant : X-OFF ne déplace pas le chariot (INRDP75 l. 8624, « ceci n'entraîne pas un retour à la ligne suivante »). INRDP75 recommande « 2 espaces ou 2 nuls » après chaque X-OFF (l. 8582, l. 8634) et perfore `EX⊗∧∧1⊗` (l. 8632) ; l'absence de tout blanc imprimé écarte les espaces.

3. **Pilotage du perforateur.** `EQC(18)` met le perforateur en route, `EQC(20)` l'arrête (INRDP75, annexe V, l. 8988-8992). La question `PERFORATION ? (OUI/NON)` s'explique ainsi : à `NON`, ces codes ne sont pas émis. INRDP75 l. 8628-8630 décrit le même geste à la main : « Perfo OFF - Return - Line feed - Perfo ON […] les caractères RC-LF ne figurent pas sur le ruban ». Sur MITRA 15, RETURN entre dans la chaîne lue (l. 1558) : un ruban compatible ne peut pas en contenir.

4. **En-tête.** `3`, deuxième nombre, est δ (CHARGER, δ = 3 : chaîne de caractères ; `numerisation0001` : `TYPE 3`). Les espaces simples sont ceux du format `U`, qui suit un nombre d'un blanc (l. 1564, l. 1304) : `AFFICHER[…'ENREGISTREMENT ',3U]`.

5. **Coupure des lignes.** Essais sur les 31 enregistrements des quatre sorties :

   | Règle : couper après… | Fenêtre | Enregistrements reproduits |
   |---|---|---|
   | **la dernière non-lettre** | **72** | **31 / 31** |
   | la dernière non-lettre | 70 | 17 / 31 |
   | la dernière non-lettre | 71 | 22 / 31 |
   | la dernière non-lettre | 73 | 23 / 31 |
   | le dernier espace | 72 | 26 / 31 |
   | le dernier espace ou `/` | 72 | 28 / 31 |
   | le dernier espace, `/` ou `'` | 72 | 30 / 31 |

   Trois témoins fixent la règle. `…, C'` / `EST ABOMINABLE` (FICH2 enr. 6) : `C'` finit en colonne 69, `C'EST` tiendrait exactement dans les 72 colonnes, mais le test porte sur la fenêtre de 72 caractères et non sur le mot suivant. `…LES TIENS,` / `ETC` (PRONO enr. 8) : `LGR:73` prouve qu'aucun espace ne suit la virgule. `…QU IL/` / `IL A/` (FICH enr. 7) : coupure après `/`. Le découpage se fait donc en reculant depuis la colonne 72 jusqu'à la première non-lettre, qui reste en fin de ligne ; `SKP(Z,B)=B` vaut « le caractère B est une lettre » (l. 1216). La liste `espace , / '` donne aussi 31 / 31 ; la non-lettre est retenue parce qu'elle convient à un programme général, qui ignore les séparateurs propres à `PRO`. `PRO` lui-même emploie une méthode voisine (lignes 105 à 108) : recul depuis la colonne 79 jusqu'à un espace.

6. **Quatre lignes par enregistrement.** D'un en-tête au suivant : toujours 6 lignes (27 cas), même pour `SE,SOI.`. Un enregistrement court est donc complété par des lignes vides : la boucle compte quatre lignes, elle ne s'arrête pas à la fin de la chaîne. Sur ruban, cela donne à `CHARG` une lecture de longueur fixe.

7. **Ligne blanche.** Le dernier enregistrement diffère : `& FIN DE FICHIER` suit à 5 lignes de l'en-tête (4 cas sur 4), au lieu de 6. Aucune condition n'est nécessaire pour l'expliquer : chaque enregistrement se termine par un passage à la ligne, l'en-tête en commence par un autre, et `& FIN DE FICHIER` n'en commence pas. La sortie `0029`, dont le dernier enregistrement remplit ses quatre lignes, le montre : `& FIN` vient juste après `HABITUDE, CE BESOIN: ELLE`.

8. **Première question.** `EXECUTER A PARTIR DE 1`, une ligne blanche, puis `NOM :`. `PUISS` (`5 AFFICHER '…'`, un seul passage à la ligne) imprime la même ligne blanche : le système passe lui-même à la ligne après le `1`. La première question n'a donc qu'un passage à la ligne, ce que donne la forme sans format de PG62, retenue pour les trois questions.

## 5. CHARG

`CHARG` n'est connu que par son nom sur le ruban et par sa fonction. Il lit le nom, puis, pour chaque enregistrement, l'en-tête et le contenu, et gare le tout par `GARER … ,NOM` en fichier temporaire : c'est ce que suppose `CAPRONO,PRONO` à la fin du ruban, et ce que décrit INRDP75 l. 8610 (« garer les données en fichier temporaire puis en fichier permanent »).

Un en-tête se lit comme une chaîne, parce que `&` n'est pas un nombre : `SI SCH(L,1,1)='&' ALORS TERMINER`, puis les nombres par `CNB(L,15,Q)`, Q donnant la reprise (l. 1352). `TERMINER` rend la main au moniteur, qui lit `CA` et `PRONO,PRONO` sur le ruban.

## 6. Conjectures

| Point | Choix | Autre lecture | Ce qui le trancherait |
|---|---|---|---|
| Mots imprimés sur le ruban | tout est perforé, sauf les passages à la ligne | `ENREGISTREMENT ` et ` FIN DE FICHIER` imprimés perforateur arrêté ; `CNB(L,1,Q)` au lieu de `CNB(L,15,Q)` | un ruban conservé ou le listage des originaux |
| `& FIN DE FICHIER` | la marque est le premier caractère | `L='&'`, le reste imprimé seul | idem |
| Troisième nombre (`0`) | première dimension d'un tableau, 0 pour une chaîne ; un quatrième nombre pour une matrice | toute autre grandeur nulle pour une chaîne ; seul `0` est attesté | une sortie de `PERFO` sur un tableau |
| Tableaux, variable simple | un nombre par ligne, écrit par `CCA` ; `PERFO` demande les dimensions, qu'aucune fonction de LSE ne fournit | lecture en bloc par `LIRE A`, quelques nombres par ligne | idem ; INRDP75 l. 8614 atteste seulement « tableaux et chaînes » |
| Réponse `NON` | même imprimé, rien de perforé | listage seul, sans les commandes `AP…` et `CA…` | une sortie avec `NON` |
| Fichier ou enregistrement absent | `FICHIER INEXISTANT` et arrêt ; enregistrement absent sauté | aucune sortie | idem |
| Chaîne qui ne tient pas sur 4 lignes | la 4ᵉ ligne prend tout le reste : le papier s'écrase en colonne 72, le ruban reste juste | la fin perdue, ou une 5ᵉ ligne | une chaîne plus difficile que celles conservées |
| Caractère de fin | X-OFF, `EQC(19)` | RUB-OUT : la bannière de Victor Duruy porte `RUB-OUT REMPLACE CTRL X-OFF`, et ni l'un ni l'autre ne s'imprime ; INRDP75 fait arrêter le lecteur par X-OFF (l. 8642) | un ruban conservé |
| Réponse vide à `LIRE` | chaîne vide, et le `LIRE` suivant demande une nouvelle réponse | refus de la réponse vide : écarté sur LSE 8086 (essai d'Éric du 2026-10-31 : X-OFF seul à deux `LIRE L` successifs, une demande à chaque `LIRE`, `LGR(L)` = 0 chaque fois), non attesté sur le T1600 ; tout le schéma à quatre champs en dépend | un essai sur le T1600 |

Un nombre passe par sa forme imprimée à 7 chiffres : un tableau contenant `1/3` ne revient pas identique (essai fait). Les valeurs de l'essai mixte (`-2.5`, `1.5`, `-7`, `1E+10`, entiers) reviennent exactement.

## 7. Conséquences pour le LSE-IIGS

La reconstitution a mis au jour trois écarts, corrigés le 2026-10-31 sur arbitrage d'Éric.

1. **`LIRE`.** Après une réponse vide à `LIRE` sur une chaîne, les `LIRE` suivants rendaient la chaîne vide sans rien demander, au clavier comme au ruban : `saisie_pos` restait à 0 et la ligne vide était relue. Sur `AAA`, vide, vide, `BBB`, vide, `CC`, six `LIRE` donnaient les longueurs 3, 0, 0, 0, 0, 0. LSE 8086 (essai d'Éric du 2026-10-31) demande une réponse à chaque `LIRE` et donne une chaîne vide. Une ligne de `lire_chaine` corrige le défaut (longueurs 3, 0, 0, 3, 0, 2 ; Interpréteur IMP-0222).

2. **Longueur des chaînes.** FICH2 enr. 8 occupe quatre lignes de 68, 70, 67 et 66 caractères, soit 274 avec les trois espaces, sous `LSE F4`. Le maximum de 255 caractères sur T1600 (INRDP75 l. 817) ne vaut donc pas pour la version de Victor Duruy. L'attestation est indirecte : les espaces et la longueur sont déduits, seul PRONO a ses longueurs imprimées ; même sans espaces, la chaîne ferait 271 caractères. La vraie borne de `LSE F4` n'est pas connue. Le LSE-IIGS borne désormais les chaînes à 65 535 caractères sous les deux profils (Données IMP-0209).

3. **Perforateur et console.** Un programme peut perforer : `EQC(18)` et `EQC(20)` commandent le perforateur (Bibliothécaire et fichiers IMP-0274). `EQC(17)` à `EQC(21)` ne vont plus à la console (IMP-0275), où ils ont un autre sens (*GS/OS Reference*, chap. 9, p. 244) : `$12` passe en 80 colonnes, `$13` (X-OFF) efface depuis le début de la fenêtre de texte jusqu'au curseur, `$14` prend l'octet suivant pour une position horizontale. `EQC(17)` et `EQC(21)` restent sans effet sur le lecteur. Le filtre n'a pas été essayé sur l'Apple IIGS : Golden Gate n'a pas le pilote `.CONSOLE`.

## 8. Vérification

`essais/verifier.py` rejoue tout sous Golden Gate avec `build/LSE`. Le ruban est le fichier `RUBAN` de la bibliothèque, que `LSE` perfore lui-même. Le ruban d'entrée de la première étape est coupé tous les 72 caractères, sans la règle de `PERFO`, pour que la coupure observée vienne du programme LSE.

```
python3 -I essais/verifier.py TRAVAIL ../../build/LSE
```

Les textes de `sorties/` sont le seul attendu ; les espaces en fin de ligne, invisibles sur le papier, ne sont pas comparés.

Avec un troisième argument, `verifier.py` écrit dans ce répertoire les rubans perforés par `PERFO`, si toute la vérification a réussi.

## 9. Images disque

Depuis le 2026-11-06 (choix d'Éric ; Construction IMP-0241, complément), `CHARG` et `PERFO` font partie de la distribution du LSE-IIGS, et `PERFO.po` n'existe plus. Tous les fichiers ci-dessous sont des rubans de type TXT :

| Ruban | Image | Contenu |
|---|---|---|
| `CHARG`, `PERFO` | `build/LSE.po` (`make`) | le programme, lignes terminées par CR, puis `RA⊗nom⊗EF⊗*⊗` : le ruban se range lui-même, comme celui d'INRDP75 l. 8582 (`scripts/creer_ruban_programme.sh`) |
| `PRONO`, `FICH`, `FICH2` | `build/DURUY.po` (`scripts/creer_image_duruy.sh`) | perforés par `PERFO`, depuis les sorties `0042`, `0012` et `0011`, par `essais/verifier.py`, et écrits seulement si toute la vérification réussit |

La sortie `0029` n'a pas de ruban : elle porte le même nom que `0012`, version complète de FICH.

Chaque ruban se lit dans une session, sous le profil `LSE F4`, profil par défaut, qui ouvre la session sous l'identification 1, avec le droit de ranger et de cataloguer ; `CHARG` passe le premier, puisque les rubans de données commencent par l'appeler :

```
LSE -RUBAN /LSE/CHARG       puis BO, Ctrl-Q, AU
LSE -RUBAN /LSE/PERFO       puis BO, Ctrl-Q, AU
LSE -RUBAN /DURUY/PRONO     puis BO, Ctrl-Q, AU
```

Un ruban de programme déjà rangé donne `ERREUR F 03`, sans autre effet. `PERFO` ajoute à la fin du ruban désigné : pour perforer, il se lance sans `-RUBAN`.

Essais sous Golden Gate : les cinq rubans, lus dans cet ordre dans une bibliothèque vierge, sans erreur ; `PERFO` relit alors PRONO, FICH et FICH2 comme sur les sorties (2026-10-31, depuis l'ancienne `PERFO.po`) ; `CHARG` et `PERFO` extraits de `LSE.po` se rangent (2026-11-06). La lecture sur l'Apple IIGS reste à faire.
