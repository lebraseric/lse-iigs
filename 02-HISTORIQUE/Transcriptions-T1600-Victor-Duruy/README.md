# Transcriptions des programmes T1600 de Victor Duruy

**Source :** `02-HISTORIQUE/Programmes LSE T-1600 Victor Duruy 1978-1983/` (numérisations de sorties de télétype ; voir aussi `Releve-sorties-T1600-Victor-Duruy.md`).
**Usage :** un fichier `.lse` par programme, une ligne L.S.E. par ligne (fin de ligne LF). `scripts/creer_image_duruy.sh` en fait l'image `build/DURUY.po` (volume `/DURUY/`) : chaque fichier y devient un ruban de type TXT, lignes terminées par CR (IMP-0262). L'image porte aussi, depuis le 2026-11-06, les rubans de données `PRONO`, `FICH` et `FICH2` perforés par `PERFO` (`02-HISTORIQUE/Reconstitution-PERFO-CHARG/README.md`, §9). Lecture : `LSE -RUBAN /DURUY/ANGLO`, `BO`, puis X-ON (Ctrl-Q) sur une ligne vide (IMP-0263).
**Transcription :** le 2026-10-28, par lecture des images ; à relire sur les originaux.

## Conventions

- `Ø` imprimé est la lettre O ; `←` est écrit `_`, `↑` est écrit `^` (caractères du compilateur LSE-IIGS).
- Les espaces sont relevés d'après les colonnes imprimées et vérifiés colonne par colonne sur l'encre de la numérisation (« Relecture à l'encre », plus bas).
- Une ligne vide porte l'espace qui suit le numéro (`122 `), comme le demande INRDP75.
- Commentaires sur plusieurs lignes imprimées (titres décalés vers la droite, saut de ligne sans retour chariot dans le texte) : mis sur une seule ligne, chaque mot à sa colonne imprimée ; le lecteur de ruban ignore LF (IMP-0263), le saut de ligne n'est donc pas reproductible.
- **Lignes coupées.** La télétype imprime 72 colonnes ; au-delà, tout le reste de la ligne s'imprime en surimpression dans la 72ᵉ colonne. Le listage de PRON2 (ruban rouge) en imprime 73 : sa ligne 67, de 73 caractères, est entière. La fin de ces lignes est restituée ci-dessous, avec sa source. Aucune n'est attestée telle quelle.

## Fichiers

| Fichier | Version transcrite | Restitutions (lignes coupées) |
|---|---|---|
| `ANGLO.lse` | `ANGLO_V9_LISTING_numerisation0038.pdf` | chaînes de 25, 39, 43, 55, 63, 73, 87, 101, 114, 116, 129, 143, 147, 155, 159, 167 d'après l'exécution `ANGLO_V9_EXEC_numerisation0039.pdf` (25 sans point final, les autres avec) ; branchements de 23 (`ALLER EN 27`), 53 (`ALLER EN 57`) et 113 (`ALLER EN 117`) d'après le schéma des questions (ligne suivant la ligne « EXACT ») |
| `CARTO.lse` | `CARTO_V2_numerisation0018.pdf` (identique à 0028 et 0036) | aucune |
| `CARTOV1.lse` | `CARTO_V1_numerisation0009.pdf` (première version, 5ème, sans bannière) : lignes 37 et 51 sans `' CHEZ ELLE.'` ni `' CHEZ LUI.'`, le reste identique à CARTO | aucune |
| `DE31.lse` | `DE31_numerisation0021.pdf` | aucune |
| `GA.lse` | `GA_V2_LISTING_numerisation0016.pdf` | aucune |
| `MORSE.lse` | `MORSE_V2_numerisation0008.pdf` ; ligne 4, illisible au pli, lue sur `MORSE_V1_numerisation0007.pdf` | aucune ; programme inachevé (lignes 162 + code et 252 absentes) |
| `MULT.lse` | `numerisation0015.pdf` (dernière version) | 4 : commentaire arrêté à `N` ; 12 : `];LIRE A` (A testé en 14 ; l'exécution 0005 montre la réponse sur la même ligne) ; 26 : `ALLER EN 40` ; 50 : `.7 7 7.]` (un chiffre sous la surimpression) |
| `NBOR.lse` | `numerisation0024.pdf` | fins des chaînes 10, 20, 40 d'après l'exécution imprimée sur la même page |
| `PA1.lse` | `numerisation0019.pdf` | aucune ; fragment (lignes 1 à 20), seul conservé ; sortie sur 80 colonnes |
| `PRO.lse` | `4EME - PRO/Listing PRO - numerisation0046.pdf` (72 colonnes) | 19 lignes, voir « PRO et PRON2 » ; début de la ligne 12 (`12 &`), imprimé en une seule tache au retour du chariot |
| `PRON2.lse` | `4EME - PRO/Listing PRON2 - numerisation0045.pdf` (73 colonnes) | 13 lignes, voir « PRO et PRON2 » |
| `PUISS.lse` | `numerisation0006.pdf` | aucune |
| `SINUS.lse` | `numerisation0043.pdf` (= 0035) | aucune ; la procédure `&TRACE` n'est pas dans le listing |
| `SPHIN.lse` | `numerisation0025.pdf` (exécution : 0040) | 31, 41, 51 : `ALLER EN 37 FIN`, `47 FIN`, `57 FIN`, d'après la ligne 21 et le brouillon `numerisation0003.pdf` |

Non transcrits, par choix d'Éric (2026-11-05) : versions antérieures d'ANGLO (V1 à V8), de GA (V1), de MORSE (V1) et de SPHIN, toutes sans exécution comparable ; leurs messages sont au relevé.

## PRO et PRON2

Transcrits le 2026-11-05. Les deux programmes partagent les lignes 1 à 7 (sauf 3 et 5) et 100 à 180 ; PRO enchaîne sur PRON2 par `60 EXECUTER 'PRON2'`. Ils lisent le fichier-texte choisi par l'élève (FICH, FICH2) et `PRONO`, imprimés par PERFO et transcrits dans `../Reconstitution-PERFO-CHARG/sorties/` : enregistrement 1, `NE/` suivi du texte ; `NE+1`, `TITRE/AUTEUR/` ; `NE+2`, `NEN/ZPRO/` suivi des paires pronom-mot ; `NE+NEN+2`, `NC/` suivi des catégories absentes ; `PRONO` 1, la liste des catégories, 2 à 9, leurs tableaux.

Les fins surimprimées sont illisibles. Chaque restitution a été confrontée, colonne par colonne, à l'encre de la numérisation (présence d'un caractère ou d'une espace dans chaque colonne, à 200 points par pouce) : le préfixe imprimé et la colonne de la surimpression concordent pour toutes. Degré : **attestée** (par l'autre listage, ou par le dossier décrit plus bas), **déduite** (par la logique du programme), **conjecturale**.

| Ligne | Imprimé (PRO, sauf mention) | Fin restituée | Degré et source |
|---|---|---|---|
| 11 | `…TROUVE: ';L▮` | `IRE NPRO` | déduite : `NPRO` est comparé dès la ligne 12 |
| 12 | `▮NBPRO();…` | début `12 &` | déduite : numéro entre 11 et 13 ; procédure `&NBPRO` (l. 170), appelée de même par PRON2 l. 69 |
| 13 | `…ALORS ALLER▮` | ` EN 19` | déduite : réponse proche, « ASSEZ BONNE REPONSE » ; 14 et 15 renvoient à 21 et 23 |
| 19 | `…SUPPLEMENTA▮` | `IRE.'` | mot déduit ; point conjectural (comme 18, 21, 24), retenu par Éric le 2026-11-05 |
| 21 | `…NECESSAIRE.';ALLER E▮` | `N 26` | conjecturale : 26 comme 18 et 20 ; 25 possible ; retenue par Éric le 2026-11-05 |
| 26 | `…CATEGORIES DE PRONOMS ▮` | `QUE TU']` | attestée par le dossier (`…DE PRONOMS QUE TU / CONNAIS.`) ; le format doit être fermé |
| 31 | `…ALLER EN 3▮` | `7 FIN` | déduite : fin de la boucle (37) et `FIN` du `DEBUT` |
| 33 | `…DANS VOTRE IMAGI▮` | `NATION'` | attestée par le dossier, sans point |
| 40 | `…DANS CE T▮` | `EXTE ?']` | attestée par le dossier (`…DANS CE TEXTE ?OUI`) ; format fermé |
| 50 | `…DIFFERENTES']CO▮` | `R` | déduite : variable `CORR` (l. 45, 47) |
| 101 | `…+2,'▮` ; PRON2 `…+2,'/▮` | `/')` | attestée : `/` par PRON2, `)` vu en surimpression ; `SCH(…,'/')` comme en 154, 174 |
| 125 | `…VEUX CONSU▮` ; PRON2 `…CONSUL▮` | `LTER'` | `L` attesté par PRON2, la suite par le dossier, trois fois sans point |
| 128 à 137 | `…AFFICHER CAT ▮`, `…AFFICHER ▮`… | `FIN`, `CAT FIN` | attestée : PRON2 imprime la ligne 129 entière, `…AFFICHER CAT FIN` (73 caractères), et une colonne de plus pour les autres |
| 240 | `…ALLER EN 2▮` | `45 FIN` | déduite : comme 238, `ALLER EN 245 FIN` |
| PRON2 55 | `…DANS L"O▮` | `RDRE']` | attestée par le dossier (`…DANS L'ORDRE`) ; seule ligne de la feuille surimprimée en colonne 72 et non 73, anomalie non expliquée |
| PRON2 56 | `…CHAQUE PRONOM ▮` | `LE MOT'` | attestée par le dossier (`…CHAQUE PRONOM LE MOT`) ; FICH range les paires pronom-mot (`QUICONQUE VOIT/QUI PREND/…`) |
| PRON2 58 | `…SEULE LETTRE▮` | `.'` | attestée par le dossier (`…D'UNE SEULE LETTRE.`) ; la tache a la forme d'une apostrophe sur un point |
| PRON2 73 | `…PRO_PRO+1;ALLER EN ▮` | `79 FIN` | déduite : bonne réponse, ligne 79 (réponse suivante) ; `FIN` du `DEBUT` |
| PRON2 81 | `…TABLEAU ?';LIRE▮` | ` QUIZ` | déduite : 82 teste `QUIZ` ; le dossier montre la réponse (`?OUI`) suivie du tableau |

Autres faits relevés :
- PRON2, ligne 82 : `ALLER EN 85`, ligne absente du programme, attesté tel quel ; il n'est pris que si `PRO>ZPRO-11`, et LSE-IIGS y répond `ERREUR (EX.) 75` (MEMENTO82 : « ligne de branchement inexistante ») ;
- la ligne 2 porte `N.218` en colonne 52, mesurée sur les deux listages.

**Dossier dactylographié** (`4EME - PRO/Exécution PRO-PRON2.pdf`, versé par Éric le 2026-11-05) : dossier daté d'avril 1982, publié dans *L'informatique à l'école, concours de programmes, AFCET - IFIP, 1981*, numéro spécial du bulletin trimestriel de l'EPI (association Enseignement Public et Informatique), juin 1982 (d'après Éric) ; pages 80 à 82, « EXEMPLES D'EXECUTION », suivies de « Ce programme a fait l'objet de tests en fonctionnement réel avec une classe de 4ème ». L'exécution de PRO puis de PRON2 sur FICH y est retapée à la machine à écrire, avec des coupures (`....`), une note de l'auteur (`*IL N Y A PAS DE PRON.POSSESSIF NI INTERROGATIF *`), `...` pour `!!!`, et des libellés qui s'écartent du listage : `DIS MOI COMBIEN TU EN A TROUVE :` (l. 11 : `DIS-MOI … AS TROUVE:`), `TAPE FIN.` (l. 27 : `TAPES FIN.`), `NOMME LA OU LES CATEGORIES NON REPRESENTEES.` (l. 228 : `NOMME LES CATEGORIES NON REPRESENTEES:`), `VOULEZ-VOUS RAJOUTER` (PRON2 l. 83 : ` VEUX-TU RAJOUTER`), liste complète sans `1/23/` et avec apostrophes (`QU'IL`, `S'HABITUER`). C'est donc une copie non littérale, peut-être d'une autre version du programme ; elle ne remplace pas le listage. Elle atteste les fins restituées des lignes 26, 33, 40, 125 de PRO et 55, 56, 58 de PRON2, et l'enchaînement de 11, 81 et 128 à 137.

Rejouée avec les mêmes réponses (2026-11-05, `LSE F4`), la session suit le dossier message pour message ; la note finale de PRON2 y est la même, `9.565216 /20` (11 bonnes réponses, `11/(23/20)`) : c'est la valeur des réels de 23 bits (Interpréteur IMP-0280) ; des réels de 24 bits donnent `9.565218`, le calcul exact `9.565217`. Deux écarts : la note provisoire de PRO (`3.125` dans le dossier, `3.75` ici : 6 points au lieu de 5) et le `NON` de la ligne 240, attesté par le listage (`AFFICHER 'NON';ALLER EN 2…`) mais absent du dossier, après `PRONOMS PERSONNELS` ; avec le listage, la session vaut 2 + 3 catégories − 1 (`NON`) + 2 (`INTERROGATIFS`) = 6 points ; le dossier en totalise 5, ce qui suppose une autre version des lignes 238 et 240, ou une copie inexacte.

Vérification (2026-11-05, Golden Gate, `LSE F4`) : les deux programmes se compilent et se relistent à l'identique ; avec FICH et PRONO chargés par CHARG, PRO s'exécute jusqu'à `EXECUTER 'PRON2'`, et PRON2 jusqu'à `TERMINE EN LIGNE 095`. Il n'existe pas de sortie de télétype de leur exécution.

## Lignes refusées par le compilateur LSE-IIGS (profil `LSE F4`)

Aucune : toutes les lignes transcrites se compilent sous `LSE F4`. Les transcriptions restent fidèles aux sorties.

## Relecture à l'encre

`essais/encre.py` relit une transcription sur la numérisation de son listage, colonne par colonne : chaque colonne imprimée porte de l'encre ou non, et la transcription y attend un caractère ou une espace ; une espace en trop ou en moins décale toute la suite de la ligne. Le pas de la page et la phase de chaque ligne sont calés sur les blancs entre caractères ; les lignes transcrites sont appariées aux lignes imprimées par un alignement global ; les signes maigres (`. , ' : ; -`) ne sont jamais comptés comme manquants ; les colonnes au-delà de la 71ᵉ ne sont pas comparées. L'outil vérifie la place des caractères, pas leur identité.

```
python3 -I essais/encre.py NBOR.lse "../Programmes LSE T-1600 Victor Duruy 1978-1983/5EME - NBOR/numerisation0024.pdf:1"
```

Résultat du 2026-11-05, sur les listages de la table « Fichiers » : 610 lignes concordantes sur 627.
- NBOR 23/23, PUISS 10/10, MULT 27/27, CARTO 27/27, CARTOV1 27/27, DE31 35/35, SINUS 23/23 ;
- SPHIN 31/32, ANGLO 93/95, GA 72/79, MORSE 32/33, PA1 10/11, PRO 112/115, PRON2 88/90.

Les 17 lignes restantes ont toutes été regardées sur l'image : trait de pli ou poussière sur une espace (SPHIN 23, ANGLO 35 et 81, GA 2 et 70, PRO 109 et 129, PRON2 75 et 128) ; début de la ligne 12 de PRO imprimé en une tache au retour du chariot ; ligne 4 de MORSE, illisible au pli sur V2 et lue sur V1 ; commentaires imprimés sur deux lignes, titres décalés vers la droite (GA 14, 34, 44, 64, 130 ; PA1 2). Aucune transcription n'est corrigée.

## Exécutions

Le répertoire `executions/` transcrit les exécutions imprimées des programmes ci-dessus ; `essais/verifier.py` les rejoue sous Golden Gate et les compare ligne à ligne (transcription et vérificateur du 2026-11-05, à relire sur les originaux) :

```
python3 -I essais/verifier.py TRAVAIL ../../build/LSE
```

| Exécution | Feuille | Programme exécuté | Résultat (2026-11-05) |
|---|---|---|---|
| `NBOR_0024.txt` | `5EME - NBOR/numerisation0024.pdf`, `LSE F4` | `NBOR.lse` | 42 lignes identiques ; écart consigné : `3.645751` pour `3.645752` (Interpréteur IMP-0280) |
| `PUISS_0006.txt` | `5EME - PUISS/numerisation0006.pdf`, sans bannière | `PUISS.lse` | 9 lignes identiques |
| `MULT_0005.txt` | `5EME - MULT/numerisation0005.pdf`, `LSE F4` | version antérieure à `MULT.lse` (0015) : `FAIRE` des lignes 17 et 29 sur les lignes 16 et 28 ; même impression | 55 lignes identiques |
| `SPHIN_0040.txt` | `6EME - SPHIN/numerisation0040.pdf` | `SPHIN.lse` | 41 lignes identiques |
| `ANGLO_0039.txt` | `5EME - ANGLO/ANGLO_V9_EXEC_numerisation0039.pdf` (deux numérisations qui se recouvrent, réunies) | `ANGLO.lse` | 97 lignes identiques, `DIDI\` compris (INRDP75 l. 666 : `\` efface le dernier caractère) et `PAUSE`, `CONTINUER` |
| `CARTO_0037.txt` | `5EME - CARTO/numerisation0037.pdf` | `CARTO.lse` | 45 lignes identiques |
| `CARTOV1_0009.txt` | `5EME - CARTO/CARTO_V1_numerisation0009.pdf`, sans bannière | `CARTOV1.lse` | 34 lignes identiques |

Conventions :
- chaque fichier va de `EXECUTER A PARTIR DE 1` à `TERMINE EN LIGNE nnn`, une ligne imprimée par ligne ; les lignes blanches et les colonnes sont relevées sur la grille de la télétype (10 caractères et 6 lignes par pouce, numérisations à 200 points par pouce) ;
- `Ø` imprimé est la lettre O ; les espaces de fin de ligne ne sont pas significatifs ; BEL et NUL ne s'impriment pas ;
- la frappe de l'élève est notée entre accolades : `{EX}ECUTER A PARTIR DE {1}`, `VOTRE NOMBRE S.V.P.  {7}` ; chaque groupe est terminé par X-OFF, que la télétype n'imprime pas : la sortie suivante commence sur une nouvelle ligne par le passage à la ligne d'`AFFICHER`, et les réponses aux variables-chaînes sont reconnues (`R1='800'`), ce que RETURN, rangé dans la chaîne (Interpréteur IMP-0222), empêcherait ;
- une ligne `~` marque un changement de feuille, où le nombre de lignes blanches n'est pas lisible ;
- le vérificateur tape le programme après `BONJOUR` sous `LSE F4`, fournit la frappe dans l'ordre, retire de la ligne attendue la frappe (dont le repli stdio ne renvoie pas l'écho) et compare ; tout écart non consigné dans `ECARTS` le fait échouer.

Hors d'atteinte : GA (`GA_V1_EXEC_numerisation0044.pdf`) et DE31 emploient `ALE`, dont le générateur d'origine est inconnu ; SINUS appelle `&TRACE`, absente du listing ; MORSE n'a pas d'exécution imprimée ; PA1 est un fragment.
