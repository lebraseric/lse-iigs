# Audit de complétude et de cohérence — RCI-M et Compilateur

**Date :** 2026-10-21
**Documents audités :** [RCI-M_Specification_physique.md](../tome-v/RCI-M_Specification_physique.md) ; [Compilateur_LSE-IIGS.md](../tome-v/Compilateur_LSE-IIGS.md)
**Documents de référence :** `LSE15Mitra.g4` (grammaire reconstruite, chaque règle examinée) ; LSE15 (§3-3, §5, §6, §7, §9) ; INRDP75 (opérateurs, procédures, Annexe III, décompilateur et DECODER) ; Référentiel normatif (EXI-0003, EXI-0015, EXI-0042) ; Tomes I-II §13.1 (VM-1301) ; [Etat_Execution_LSE-IIGS.md](../tome-v/Etat_Execution_LSE-IIGS.md), pour l'exécution.
**Question posée :** les spécifications permettent-elles (1) de compiler les constructions de la grammaire et les exemples de la documentation historique, (2) de décompiler, (3) d'exécuter ?
**Mode de traitement :** les points A et B se traitent un par un, chacun proposé d'abord en prose ; les points C sont des corrections rédactionnelles applicables directement. Tout point traité reçoit, dans la colonne « Statut » du tableau de suivi (§E), la référence de la décision `IMP-*` ou de la correction qui le referme.

Les numéros de ligne cités sont ceux des documents à la date de l'audit.

---

## A. Défauts bloquants (exécution ou décompilation impossible ou incorrecte)

### A1. FAIRE : l'élément d'ouverture n'est ni exécutable ni décompilable sans ambiguïté

Références : RCI-M §4.7, l. 320-321 (sous-codes 0x09/0x0A, « voir §4.8.2 ») ; RCI-M §4.8.2, IMP-0112 ; Etat_Execution §6.1, l. 446 (`decalage_condition`).

- **`PAS` optionnel.** IMP-0112 indique à la fois « constante 1 omise si absente » et « `p` omis compte pour 0 octet ». Si `p` n'est pas compilé, l'élément 0x09/0x0A consomme tantôt 3, tantôt 4 valeurs (`n1`, `v1`, `[p]`, `v2` ou condition), sans aucune marque physique de la présence de `p`. L'interpréteur ne peut pas savoir combien de valeurs dépiler ; le décompilateur ne peut ni délimiter les sous-séquences, ni restituer la présence ou l'absence de `PAS`.
- **`TANT QUE`.** La condition doit être réévaluée à chaque itération (LSE15 §5-5.4, exemple `FAIRE 2 POUR I←0 TANT QUE N#99`). Etat_Execution §6.1 suppose connu l'offset de début de la sous-séquence de la condition (`decalage_condition`), mais RCI-M ne le porte nulle part, et le début d'une sous-séquence postfixée ne peut pas être retrouvé en remontant la séquence à partir de l'élément qui la consomme.
- **Charge utile `V`.** RCI-M la décrit comme « une référence de variable (§4.5, sous-code 0x00) » : on ne sait pas s'il s'agit d'un élément complet incorporé (étiquette + identifiant) ou du seul identifiant de 2 octets.
- **Rédaction associée.** IMP-0112 indique que `n1` est « constante ou calculée, selon IMP-0111 », alors qu'aucun sous-code ne distingue ces deux cas pour FAIRE : `n1` est toujours une sous-séquence de valeur.

### A2. AFFICHER : aucune marque de début d'instruction lorsque le format est explicite

Référence : RCI-M §4.10.3, l. 581.

RCI-M remet à zéro l'état local du format (spécification courante, répétitions restantes) entre deux instructions AFFICHER, « chacune commençant par son propre élément de mise en page ou de format implicite ». Cette affirmation est fausse lorsque le format explicite commence par une description ou par un libellé (`AFFICHER[F2.0] A`, `AFFICHER['X='] B`).

- **Exécution :** dans `AFFICHER[2F4.1] A;AFFICHER['X='] B`, l'interpréteur ne détecte pas le début de la seconde instruction ; une répétition restant due à la première, `B` est affiché en F4.1 au lieu de la spécification U.
- **Décompilation :** `AFFICHER[F2.0] A;AFFICHER[F3.0] B` et `AFFICHER[F2.0,F3.0] A,B` compilent la même séquence d'octets ; la frontière entre instructions n'est pas restituable.

### A3. Convention des offsets de branchement intra-séquence contradictoire

- RCI-M IMP-0105 (l. 354) : offset logique compté **depuis le début du bloc** de la ligne.
- RCI-M §11, récapitulatif (l. 1096) : « Branchements intra-ligne en offset **relatif** ».
- Compilateur IMP-0166 (l. 259) : offset calculé comme « position courante du curseur **moins position de l'élément de branchement** lui-même », donc relatif.

Le compilateur, l'interpréteur et le décompilateur doivent appliquer une seule et même convention, qui reste à fixer.

### A4. Associativité de l'exponentiation contraire aux sources

Référence : RCI-M §4.11.2, l. 675 (sous-code 0x05).

RCI-M pose une associativité à droite (`a↑b↑c = a↑(b↑c)`). Or LSE15 §3-3.4 énonce : « Dans une suite d'opérations de même valeur hiérarchique où ne figure aucune parenthèse, les calculs sont effectués de gauche à droite », sans exception pour `↑` ; INRDP75 (l. 869) dit de même : « À priorité égale, les opérations se font de gauche à droite ». La grammaire reconstruite applique l'associativité à gauche (commentaire [D]).

- **Exécution :** `2↑3↑2` vaut 64 selon les sources et 512 selon RCI-M.
- **Décompilation :** le placement des parenthèses restituées en dépend.
- **Compilation :** le chapitre Compilateur ne dit rien de l'associativité, alors que c'est lui qui la détermine en produisant l'ordre postfixé.

### A5. État des variables à l'exécution non représenté

Références : RCI-M §6.2, l. 907 et l. 943 (`EntreeSymbole`) ; Etat_Execution l. 211 et l. 303 (IMP-0149) ; RCI-M IMP-0108, IMP-0110.

- **État « non défini ».** `EntreeSymbole` ne peut pas le représenter. Il est pourtant exigé par LSE15 §7-8.1 (« une erreur sera détectée si cette variable simple ou variable-chaîne est utilisée comme paramètre effectif... local » alors qu'aucune valeur ne lui a été affectée) et par `EXECUTER`, qui remet tous les identificateurs à l'état non défini (LSE15 §9-2 ; Etat_Execution §7.1, étape 2).
- **Catégorie effective.** La catégorie d'un symbole à un instant de l'exécution (après `LIBERER`, une redéclaration, ou la libération d'un identificateur `LOCAL`) n'a aucun emplacement défini. RCI-M §6.2 présente `categorie` comme un instantané de compilation et renvoie la catégorie effective au gestionnaire des variables, alors qu'Etat_Execution IMP-0149 sauvegarde et efface `categorie` dans `EntreeSymbole` pendant l'exécution.
- **Séparation programme / exécution.** La table des symboles appartient au programme courant (RCI-M §3) mais porte les valeurs des variables (IMP-0110), ce qui affaiblit la séparation physique revendiquée par IMP-0108.

---

## B. Lacunes de spécification du compilateur

### B1. Choix de la machine cible (T1600 ou Mitra 15)

RCI-M IMP-0119 suppose un compilateur « configuré pour une cible T1600 ou Mitra 15 ». Le chapitre Compilateur ne définit aucun mécanisme de choix de cible. Les constructions propres au T1600 sont absentes de la grammaire reconstruite, limitée à Mitra 15, et du chapitre Compilateur :

- fonctions `SH`, `CH`, `TH`, `SGN` ;
- instructions `UN` et `ZER` (INRDP75 Annexe III, exemple `2 UN U;ZER T`) ;
- symboles `$`, et `@` au sens T1600 (numéro de la ligne courante) ;
- `TEM` et `DAT` écrits sans parenthèses.

Plusieurs limites dépendent en outre de la cible et ne sont pas réparties par machine :

- numéro de ligne maximal : 250 sur Mitra 15, 255 sur T1600 (INRDP75 ; RCI-M IMP-0125) ;
- facteur de répétition fixe dans un format : limité à 62 par la règle de compatibilité d'INRDP75 Annexe III §4, alors que RCI-M IMP-0116 admet 1 à 99 pour toutes les cibles.

### B2. Table des constantes partagées

RCI-M IMP-0107 et IMP-0128 prévoient qu'une chaîne littérale longue (seuil provisoire : 16 caractères), réutilisée à l'identique, soit stockée une seule fois dans la table des constantes partagées. Le chapitre Compilateur ne définit pas l'algorithme de recherche et d'insertion correspondant, et aucun chapitre ne définit le retrait d'une constante lorsque les lignes qui la référencent sont supprimées. Le test de RCI-M l. 1178 suppose pourtant ce mécanisme en place.

### B3. Règles de décompilation non écrites

EXI-0015 exige seulement « une représentation textuelle valide correspondant à la construction de langage représentée ». La commande DECODER (INRDP75 l. 6820) range le résultat de la décompilation dans un fichier-données destiné à être recompilé par ENCODER : le texte restitué doit donc se recompiler en une construction équivalente, chaque ligne restant dans la limite de 80 caractères. Plusieurs informations ne sont pas conservées par RCI-M, sans qu'une décision ne fixe la forme restituée :

- regroupement des listes : `LIRE A,B` et `LIRE A;LIRE B` compilent la même séquence (l'équivalence de comportement de ces deux formes est à vérifier dans le corpus) ; de même pour `TABLEAU`, `CHAINE` et `LIBERER` ;
- `DEBUT…FIN` superflus (bloc d'une seule instruction, ou bloc hors de toute branche de `SI`), parenthèses redondantes, `+` unaire, instructions vides (`;;`) ;
- forme des constantes numériques : `3.0` restitué `3`, notation exponentielle, précision du `float` SANE (environ 7 chiffres significatifs, par exemple pour `0.1`) ;
- espaces de tête du texte d'un commentaire.

Ces règles relèvent du chapitre du décompilateur, non encore rédigé, mais RCI-M doit garantir que chacune est réalisable ; le cas de `LIRE` doit être tranché avant d'admettre la fusion.

---

## C. Incohérences de rédaction

| Réf. | Endroit | Problème |
|---|---|---|
| C1 | RCI-M §4.6, l. 288 | Affirme qu'aucune charge utile n'est associée aux opérations d'appel, de déclaration et de gestion de fichiers : faux pour 0x28, 0x29–0x2B, 0x2D–0x2F et pour les six fonctions chaînes d'IMP-0122 |
| C2 | RCI-M IMP-0120, l. 246 | « les seuls sous-codes distincts restants (0x01 à 0x04) » : la table s'arrête à 0x03, 0x04 étant réservé |
| C3 | RCI-M §13, l. 1144 | Le catalogue des sous-codes est dit « indicatif », alors qu'IMP-0117 le déclare verrouillé |
| C4 | RCI-M §4.11.3, l. 718 (0x18 NON) | Opérateur absent d'INRDP75, de LSE15 et de la grammaire, mais présenté comme attesté (IMP-0117) ; le compilateur ne peut jamais le produire |
| C5 | RCI-M §14, test l. 1161 | Place `8` (valeur de `n1`) « hors de la séquence de la ligne FAIRE », ce que contredisent IMP-0112 et le test l. 1166 |
| C6 | RCI-M §14, test l. 1166 | « la ligne 1 (la ligne `n1`) » : `n1` vaut 8 |
| C7 | RCI-M §14, test l. 1173 | `5 S+B*H/2` n'est pas une affectation (`S←B*H/2` attendu) |
| C8 | RCI-M §14, test l. 1189 | `GARER OUTIL,1,"OUTIL"` : les chaînes L.S.E. s'écrivent entre apostrophes, non entre guillemets doubles |
| C9 | RCI-M §12 | La traçabilité s'arrête à IMP-0128, sans IMP-0174 (élément 0x0D) ni IMP-0177 (octet `k` d'APPEL) |
| C10 | RCI-M §10 et §1.2 | Numérotation des invariants ANN-K-0032 incohérente entre les deux sections (1, 3, 4, 6, 9 contre 1 à 5) |
| C11 | Compilateur §9, l. 340 | `SI longueur > 80` : INRDP75 compte le caractère de fin de ligne dans les 80 caractères ; la limite applicable à `texte` est 79 ou 80 selon qu'il inclut ce caractère, ce qui n'est pas précisé |
| C12 | Compilateur IMP-0167, l. 281 | La catégorie attribuée à la création d'un symbole ne mentionne pas `CHAINE` (`SYM_CHAINE`) |

---

## D. Couverture vérifiée

Hors les points A et B, chaque règle de la grammaire reconstruite dispose d'une représentation RCI-M compilable, décompilable et exécutable :

- expressions arithmétiques, de chaîne et booléennes (`ET`, `OU`, relations, parenthèses), changement de signe, concaténation ;
- fonctions élémentaires et fonctions chaînes, à arité fixe ou variable, y compris leurs paramètres de sortie compilés comme références (`CNB`, `GRL`, `SCH`) ;
- `SI…ALORS…SINON` en position d'instruction (avec ou sans `SINON`, imbriqué, avec `DEBUT…FIN`) et en position d'expression ;
- `ALLER EN` et `RETOUR EN` à cible constante ou calculée, `RETOUR`, `RESULTAT`, `PAUSE`, `TERMINER` ;
- `TABLEAU`, `CHAINE`, `LIBERER` ; affectation simple et indexée ;
- `LIRE`, `GARER`, `CHARGER` (avec ou sans compte-rendu), `SUPPRIMER`, `EXECUTER` ;
- commentaires en tête de ligne et après `;` ;
- mode machine de bureau, y compris `?` et les restrictions récursives ;
- déclaration de procédure avec clause `LOCAL` (élément 0x0D) et appels en position d'instruction ou d'expression, y compris le passage d'un nom de procédure en argument (`&TRAC(&F1,0,2,0.25)`) ;
- `@` (T1600 et Mitra 15) et `$`, au niveau de RCI-M.

Les exemples historiques cités dans les deux chapitres (`&FACT`, `&RAC`, `&RAC2`, `&PRO`, `&UNIT`, PG26, `AFFICHER[*X,F0.3] NBSP,RESUL`, `AFFICHER['RES=',3F2.0,E3.6]…`, `CNB(PH,25,T)`, `RAC(SIN(X)↑2+COS(Y)↑2)`…) sont couverts, à l'exception de ceux qui relèvent de A1 (boucles FAIRE) et de A2 (AFFICHER à format explicite suivi d'un autre AFFICHER).

Une fois A1 corrigé, chaque élément porte de quoi calculer son effet sur la pile d'évaluation à partir de lui seul ; le décompilateur peut donc retrouver les limites des instructions, sauf dans les cas de regroupement relevés en B3.

---

## E. Suivi

| Point | Objet | Nature | Statut |
|---|---|---|---|
| A1 | FAIRE : présence de `PAS`, début de la condition `TANT QUE`, forme de la charge utile `V` | Bloquant (exécution, décompilation) | Traité le 2026-10-22 : RCI-M IMP-0112 réécrite (trois éléments : ouverture 0x09, pas 0x0A/0x0E, test 0x0F/0x10, dans l'ordre d'évaluation de LSE15 §5-5.3) ; Compilateur §6 (ordre de génération) ; Etat_Execution IMP-0151 et IMP-0152 réécrites (réévaluation du pas et de la limite, ligne de fin effective, boucles de même ligne de fin) |
| A2 | AFFICHER : marque de début d'instruction à format explicite | Bloquant (exécution, décompilation) | Traité le 2026-10-22 : RCI-M IMP-0115 réécrite (marques de tête 0x37 pour le format explicite et 0x38 sans format ; 0x36 affiche un tableau entier lorsque la référence consommée désigne un tableau à l'exécution, la distinction ne pouvant être faite à la compilation sous EXI-0001) ; Compilateur §5 (génération d'AFFICHER) et IMP-0178 (contrôle de LSE15 §6-2.4, qui garantit le retour de l'état du format à son état initial à la fin de chaque instruction) |
| A3 | Convention des offsets de branchement intra-séquence | Bloquant (incohérence RCI-M / Compilateur) | Traité le 2026-10-22 : position comptée depuis le début du bloc, fin de séquence = longueur ; séquence bornée à 255 octets (RCI-M IMP-0105 réécrite, IMP-0125 complétée) ; Compilateur IMP-0166 réécrite (écriture de la position courante du curseur) et IMP-0165 réécrite (tampon de 255 octets, contrôle de débordement `ERR_LIGNE_TROP_COMPLEXE`) |
| A4 | Associativité de `↑` | Bloquant (résultats d'exécution) | Traité le 2026-10-22 : RCI-M 0x05 corrigé (associativité à gauche, LSE15 §3-3.4, INRDP75) et test §14 précisé ; Compilateur §5, paragraphe « Précédence et associativité » (rangs, gauche à droite, opérateurs adjacents, `+` en tête accepté, ET avant OU, `!`) et tests. Entrée pour B3 : le décompilateur rétablit les parenthèses selon ces règles (ex. `A-(B-C)`, `A↑(B↑C)`) et ne restitue pas un `+` en tête |
| A5 | État « non défini » et catégorie effective des variables | Bloquant (exécution) | Traité le 2026-10-22 : Etat_Execution §3 bis, IMP-0179 (environnement des identificateurs indexé par l'identifiant de symbole, quatre états, règles de transition sourcées ; `CHAINE` sur une variable-chaîne ne conserve pas sa valeur, règle retenue non attestée) ; IMP-0149 et `EXECUTER` (§7.1) adaptés ; RCI-M IMP-0110 réécrite et `EntreeSymbole` réduite à nom, nature, origine et `InfoProcedure` (§6.2, IMP-0127) ; Compilateur IMP-0167 (nature) et règle de redéclaration (contrôle uniquement à l'exécution) |
| B1 | Choix de la machine cible et constructions T1600 | Lacune (compilation) | Traité le 2026-10-22 : Compilateur §8 bis, IMP-0180 (profil T1600 version 4 ou MITRA 15 version 8, choisi au lancement par paramètre du shell ORCA, MITRA 15 par défaut, fixe pour la session ; profil strict, `ERR_HORS_PROFIL` ; tableau des différences attestées ; productions T1600 ; mots réservés selon INRDP75 Annexe VI, préséance sur LSE15 ; bornes du numéro de ligne et du facteur de répétition selon le profil) |
| B2 | Table des constantes partagées | Lacune (compilation, gestion du programme courant) | Traité le 2026-10-22 : RCI-M IMP-0107 réécrite (structure `EntreeConstante`, entrées jamais retirées en cours de session, table vidée avec le programme, filtrage en RCI-S) et §7.1 (aucune constante issue du mode machine de bureau) ; Compilateur IMP-0181 (au-delà du seuil, toute chaîne d'une ligne de programme passe par la table ; toujours en ligne en mode machine de bureau) ; Etat_Execution §4.2 et §4.4 (`ChaineSurPile` avec indicateur `transitoire` : littéraux et valeurs de variables non transitoires, jamais libérés) |
| B3 | Règles de décompilation | Lacune (décompilation, DECODER/ENCODER) | Traité le 2026-10-22 : RCI-M §4.12, IMP-0183 (restitution fidèle de toute construction conservée ; forme canonique des espaces, parenthèses, constantes numériques, blocs `DEBUT…FIN`, instructions vides, déclarations successives ; commentaires conservés à l'octet près ; marque de tête de LIRE, sous-code de contrôle 0x11 ; DECODER signale une ligne restituée trop longue) ; Compilateur (génération de LIRE, texte des commentaires) ; Etat_Execution §7.2 (0x11 sans effet) |
| B4 | Règles lexicales (fin des mots du langage, espaces) — LSE15 §2 : « Ces mots doivent tous se terminer par un espace, un point-virgule ou un `X-OFF`, sauf `AFFICHER` et les noms de fonctions usuelles […] qui doivent se terminer par un espace ou une ouverture de parenthèse (ouverture de crochet pour `AFFICHER`). Des espaces en nombre quelconque […] peuvent apparaître entre les différents éléments du langage » ; règles absentes du chapitre Compilateur. Ajouté le 2026-10-22 | Lacune (compilation) | Traité le 2026-10-22 : Compilateur §5 bis (règles lexicales sourcées) et IMP-0182 (mot du langage suivi d'un terminateur, signe rattaché à la constante seulement après `E`, espace obligatoire après le numéro, majuscules seules hors chaînes et commentaires) |
| C5, C6 | Tests FAIRE de RCI-M §14 | Correction directe | Corrigés le 2026-10-22 avec A1 |
| C12 | Catégorie à la création d'un symbole (Compilateur IMP-0167) | Correction directe | Sans objet depuis A5 : le compilateur n'attribue plus qu'une nature (`SYM_PROCEDURE` ou `SYM_IDENTIFICATEUR`) |
| C1 à C4, C7 à C11 | Incohérences de rédaction | Correction directe | Corrigés le 2026-10-22 : C1 (§4.6, liste des opérations à charge utile) ; C2 (0x01 à 0x03) ; C3 (catalogue verrouillé par IMP-0117) ; C4 (`NON` marqué non attesté, conservé, jamais produit) ; C7 (`S←B*H/2`, INRDP75) ; C8 (`GARER OUTIL,1,'OUTIL'`, INRDP75) ; C9 (traçabilité, IMP-0177 et 0x0D) ; C10 (invariants repris avec leur numéro d'ANN-K-0032, invariant 7 ajouté au §10) ; C11 (texte limité à 79 caractères hors fin de ligne) |
