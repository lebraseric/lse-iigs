# Inventaire des points de détection d'erreur d'exécution — 2026-10-23

**Objet :** recenser, avant le catalogue des natures d'exécution (chapitre Gestionnaire des erreurs §9), chaque situation où l'exécution d'un programme ou d'une ligne de machine de bureau doit s'arrêter sur une erreur. Chaque point est rattaché à sa source dans le corpus, au composant qui la détecte et, s'il y a lieu, à l'entrée de MEMENTO82, annexe II, de même sens.
**Méthode :** celle de l'inventaire de compilation du 2026-10-22, avec les règles déjà retenues :
- IMP-0185 : numéro MEMENTO82 s'il existe, numéro à partir de 100 sinon ; 160, 200, 210 et 362 exclus ;
- IMP-0186 : forme `ERREUR (EX.) nn EN LIGNE lll` ;
- IMP-0187 : une nature par situation à corriger, un numéro MEMENTO82 partagé par les natures qu'il regroupe.
**Sources :**
- LSE15 (`02-HISTORIQUE/LSE15-Manuel-Mitra15-EAT-1978.md`, numéros de ligne du fichier) ;
- INRDP75 (`02-HISTORIQUE/LSE-Manuel-d-utilisation-INRDP-1975.md`) ;
- MEMENTO82, annexe II (`02-HISTORIQUE/MEMENTO82-transcription.md`) ;
- LAFOND83 (`02-HISTORIQUE/LAFOND83-releve.md`) ;
- chapitre État d'exécution (`05-IMPLEMENTATION/tome-v/Etat_Execution_LSE-IIGS.md`).
**Légende de la colonne M82 :**
- `=` : même sens ;
- `≈` : rattachement par proximité ;
- `—` : aucune entrée ; numéro à partir de 100.

**Attestations d'exécution déjà relevées** (Gestionnaire des erreurs §2.2) :
- `ERREUR (EX.) 30 EN LIGNE BUR` pour `?EQC(130)` (INRDP75, l. 1426) ;
- `ERREUR (EX.) 03` pour un débordement de mémoire (INRDP75, l. 3542) ;
- `ERREUR (EX.) 10 EN LIGNE 011` pour une relation dont un membre n'est pas défini (INRDP75, l. 2059 ; l. 1991 : « la commande EX efface toutes les valeurs ») ;
- `E362` pour un indice supérieur au maximum (INRDP75, l. 8281) ;
- `E210` pour l'affectation d'une chaîne à une variable non déclarée chaîne (LAFOND83, p. 130) ;
- `M200` pour une variable non définie (LAFOND83).

---

## A. Identificateurs et environnement (État d'exécution §3 bis, IMP-0179)

| Point | Situation | Exemple | Source | Détecté par | M82 | Remarque |
|---|---|---|---|---|---|---|
| X-01 | Variable non définie comme opérande d'une opération arithmétique | `X←Y+1`, `Y` non défini | INRDP75 l. 797 ; HIS-12.3 ; LSE15 l. 398 | interpréteur, déréférencement | 05 `=` | « opérande non initialisé » |
| X-02 | Variable non définie affectée ou affichée | `X←Y` ; `AFFICHER Y` | INRDP75 l. 797 | interpréteur | 62 `=` | LAFOND83 : `M200` (famille à trois chiffres) |
| X-03 | Variable non définie comme membre d'une relation | `SI I=N`, `N` non défini | INRDP75 l. 1991, l. 2059 (`(EX.) 10`) | interpréteur | 10 `=` | attesté sur T1600 ; voir RX1 |
| X-04 | Variable-chaîne déclarée, sans valeur, lue | `CHAINE C;AFFICHER C` | LSE15 l. 972 ; §7-8.1 | interpréteur | 62 `≈`, ou 08 | voir RX1 |
| X-05 | Affectation d'une chaîne à un identificateur non déclaré chaîne | `C←'ABC'` sans `CHAINE C` | LSE15 §4-3 ; INRDP75 l. 1510 ; LAFOND83 p. 130 (`E210`) | interpréteur | 06 `=` | |
| X-06 | Affectation d'un nombre à une variable-chaîne ou à un tableau | `CHAINE C;C←1` | IMP-0179 (déduit) | interpréteur | 06 `=` | « mélange de type » |
| X-07 | `TABLEAU` sur une variable simple ou une variable-chaîne | `B←1;TABLEAU B[3]` | LSE15 l. 273 ; INRDP75 l. 3463 | interpréteur | 29 `=` | |
| X-08 | `CHAINE` sur une variable simple ou un tableau | `X←1;CHAINE X` | LSE15 §4-3 | interpréteur | 04 `=` | |
| X-09 | `LIBERER`, dans une procédure, d'un identificateur non `LOCAL` | `LIBERER G` dans `&P` | LSE15 §3-1.2, Remarque 2 | interpréteur | — | |
| X-10 | Tableau entier employé là où le langage ne l'admet pas | `TABLEAU T[3];X←T+1` | IMP-0179 (déduit) | interpréteur | 05 `≈` ou 47 `≈` (second libellé) | voir RX7 |

## B. Tableaux et indices (module `donnees`)

| Point | Situation | Exemple | Source | Détecté par | M82 | Remarque |
|---|---|---|---|---|---|---|
| X-11 | Accès indexé à un identificateur qui n'est pas un tableau | `X←2;Y←X[1]` | LSE15 l. 269 | interpréteur | 47 `=` | « variable simple avec des indices » |
| X-12 | Indice supérieur à la borne déclarée | `TABLEAU A[5];A[6]←0` | LSE15 l. 235 ; INRDP75 l. 8281 (`E362`) | `donnees` | 49 `=` | |
| X-13 | Indice nul ou négatif après arrondi | `A[0.4]←1` | LSE15 §3-1.2 ; LAFOND83 | `donnees` | 27 `=` | |
| X-14 | Nombre d'indices différent de la déclaration | `TABLEAU A[3,3];A[1]←0` | IMP-0179 (déduit) | `donnees` | 46 `=` | |
| X-15 | Dimension déclarée nulle ou négative | `TABLEAU A[0]` | LSE15 l. 243 | `donnees` | 27 `=` | |
| X-16 | Tableau de plus de 65 535 éléments | `TABLEAU A[300,300]` | — | `donnees` | 45 `=` | borne d'implémentation |
| X-17 | Élément de tableau lu sans avoir reçu de valeur | `TABLEAU A[3];X←A[1]` | IMP-0179 (« éventuel état non défini de chaque élément », hors périmètre) | `donnees` | 32 `=` | à confirmer au chapitre `donnees` |

## C. Arithmétique et fonctions numériques

| Point | Situation | Exemple | Source | Détecté par | M82 | Remarque |
|---|---|---|---|---|---|---|
| X-18 | Division par zéro | `X←1/0` | — | interpréteur | 31 `=` | |
| X-19 | Zéro élevé à une puissance négative | `0^-1` | — | interpréteur | 25 `=` | |
| X-20 | Nombre négatif élevé à une puissance non entière | `(-2)^0.5` | — | interpréteur | 26 `=` (second libellé) | |
| X-21 | `RAC` d'un nombre négatif | `RAC(-1)` | — | interpréteur | 41 `=` | |
| X-22 | `LGN` d'un nombre négatif ou nul | `LGN(0)` | — | interpréteur | 42 `=` | |
| X-23 | Dépassement de capacité (résultat hors du `float`) | `1E30*1E30` | — | interpréteur | 44 `=` | |
| X-24 | `ALE` d'argument négatif ou supérieur à 1 | `ALE(2)` | LSE15 l. 328 (0 ≤ E < 1) | interpréteur | 09 `=` | |
| X-25 | Opérande chaîne dans une opération arithmétique | `X←'A'+1` (type indéterminé à la compilation) | IMP-0188 | interpréteur | 05 `=` | |
| X-26 | Relation entre un nombre et une chaîne | `SI X='A'` | IMP-0188 | interpréteur | 10 `=` | |

## D. Chaînes et fonctions sur les chaînes

| Point | Situation | Exemple | Source | Détecté par | M82 | Remarque |
|---|---|---|---|---|---|---|
| X-27 | Opérande non chaîne dans une concaténation ou une fonction chaîne | `C←X!'A'`, `X` numérique | LSE15 l. 972 | interpréteur | 08 `=` | |
| X-28 | `EQC` hors bornes (0-127 sous T1600, 0-255 sous MITRA 15) | `?EQC(130)` | INRDP75 l. 1416, l. 1426 ; LSE15 l. 722 | interpréteur | 30 `=` | attesté ; LSE15 l. 722 dit « inférieure à 128 » pour le MITRA 15 : divergence déjà consignée (plages EQC) |
| X-29 | Position de départ hors de [1, longueur + 1] (`SCH`, `PTR`, `CNB`, `SKP`, `GRL`) | `SCH('AB',4,1)` | LSE15 l. 594 | interpréteur | 26 `=` ou 11 `≈` | voir RX6 |
| X-30 | Longueur demandée négative (`SCH`) | `SCH('AB',1,-1)` | LSE15 l. 602 | interpréteur | 17 `≈` | |
| X-31 | Deuxième argument non numérique (`SCH`, `GRL`, `PTR`, `CNB`) | `SCH(C,'A',1)` | — | interpréteur | 18 `=` | type indéterminé à la compilation |
| X-32 | Troisième argument ni chaîne ni nombre (`SCH`, `PTR`) | tableau en argument | — | interpréteur | 17 `=` | |
| X-33 | Position hors bornes dans `EQN` | `EQN('AB',5)` | LSE15 l. 865, l. 867 (β = longueur + 1 donne −1) | interpréteur | 21 `=` | |
| X-34 | Chaîne résultat trop longue | concaténation au-delà de la longueur maximale | — | `donnees` | 03 `≈` ou — | longueur maximale à fixer au chapitre `donnees` |

## E. Contrôle : branchements, boucles, fin de programme (État d'exécution §6, §7)

| Point | Situation | Exemple | Source | Détecté par | M82 | Remarque |
|---|---|---|---|---|---|---|
| X-35 | Branchement vers une ligne inexistante | `ALLER EN 99` | LSE15 l. 994 | interpréteur | 75 `=` | IMP-0154 : voir RX5 |
| X-36 | Cible de branchement nulle après arrondi | `ALLER EN 0.2` | — | interpréteur | 37 `=` | |
| X-37 | Cible de branchement non convertible en entier (hors de 1 à 255) | `ALLER EN 1E10` | — | interpréteur | 38 `=` | |
| X-38 | Fin du programme atteinte sans `TERMINER` ni `PAUSE` | dernière ligne sans `TERMINER` | IMP-0154 | interpréteur | 58 `=` | voir RX5 |
| X-39 | `n1` de `FAIRE` nul ou négatif après arrondi | `FAIRE 0 POUR I←1 JUSQUA 3` | LSE15 l. 1215 | contexte (OUVRIR) | — ou 37 `≈` | voir RX4 |
| X-40 | `n1` de `FAIRE` inférieur au numéro de la ligne `FAIRE` | `10 FAIRE 5 POUR…` | LSE15 §5-5.1 | contexte (OUVRIR) | 39 `≈` | voir RX4 |
| X-41 | Variable contrôlée qui n'est pas une variable simple à l'exécution | `TABLEAU I[3];FAIRE 5 POUR I←1…` | LSE15 l. 1203 | contexte (OUVRIR) | 43 `=` | |
| X-42 | Ligne de déclaration de procédure atteinte hors appel | `ALLER EN 10`, ligne 10 = `PROCEDURE` | LSE15 l. 1946 ; IMP-0155 | interpréteur | 12 `=` | |

## F. Procédures (État d'exécution §5)

| Point | Situation | Exemple | Source | Détecté par | M82 | Remarque |
|---|---|---|---|---|---|---|
| X-43 | Appel d'une procédure non déclarée dans le programme courant | `&Q(1)` | IMP-0176 | contexte (APPELER) | — | 97 « procédure binaire » n'a pas le même sens |
| X-44 | Nombre d'arguments différent de l'arité | `&FACT(7,2)` | LSE15 l. 2016 | contexte (APPELER) | 13 `=` | |
| X-45 | Expression passée à un paramètre référence | `&RAC2(1,-1,-6,ALPHA,1+1,G)` | LSE15 l. 2075 | contexte (APPELER) | 72 `=` | |
| X-46 | Paramètre effectif `LOCAL` sans valeur | `&P(Y)`, `Y` non défini | LSE15 §7-8.1 | contexte (APPELER) | 14 `=` | MEMENTO82 : détectée « à l'utilisation du paramètre et non à l'entrée » ; voir RX3 |
| X-47 | `RETOUR`, `RETOUR EN` ou `RESULTAT` hors de toute procédure | `RETOUR` au niveau principal | — ; État d'exécution §5.5 étape 1 | contexte (RETOURNER) | 20 `=` | |
| X-48 | `RETOUR` sans `EN` dans une procédure appelée en position d'expression | `Y←&F(1)`, `&F` sort par `RETOUR` | MEMENTO82 15 ; LSE15 §7-5 ne vise que `RETOUR EN` | contexte (RETOURNER) | 15 `=` | voir RX2 |
| X-49 | `RESULTAT` dans une procédure appelée en position d'instruction | `&F(1)` en instruction, `&F` sort par `RESULTAT` | MEMENTO82 22 ; LSE15 §7-4 muet | contexte (RETOURNER) | 22 `=` | voir RX2 |
| X-50 | Redéclaration, dans la procédure, d'un tableau passé en paramètre | `TABLEAU T[5]` sur paramètre référence `T` | LSE15 l. 2104 | interpréteur | 29 `≈` ou 04 `≈` | |
| X-67 | `RESULTAT` d'un tableau entier | `RESULTAT T`, `T` tableau | MEMENTO82 59 ; LSE15 §7-4 (« la valeur que la procédure aura ») | contexte (RETOURNER) | 59 `=` | relevé le 2026-10-23, à l'écriture du retour de procédure |

## G. Entrée et sortie (AFFICHER, LIRE)

| Point | Situation | Exemple | Source | Détecté par | M82 | Remarque |
|---|---|---|---|---|---|---|
| X-51 | Facteur de répétition variable négatif après arrondi | `AFFICHER[*'X'] -1` | LSE15 l. 1678 (« Une erreur sera détectée [...] si sa valeur est négative » ; un facteur nul n'affiche rien) | interpréteur | — | corrigé le 2026-10-24 : un facteur nul est admis |
| X-52 | Valeur de type contraire à la spécification de description (chaîne sous `F`, `E`) | `AFFICHER[F3.0] 'A'` | LSE15 §6-2 | interpréteur | 06 `≈` | à confirmer au chapitre de l'interpréteur |
| X-53 | Réponse à `LIRE` mal formée | lettres pour une variable numérique | LSE15 §6-1 | interpréteur | — | comportement à établir (reprise de la saisie ou erreur) au chapitre de l'interpréteur |

## H. Fichiers et modules (GARER, CHARGER, SUPPRIMER, EXECUTER)

| Point | Situation | Exemple | Source | Détecté par | M82 | Remarque |
|---|---|---|---|---|---|---|
| X-54 | Numéro d'enregistrement nul, négatif ou supérieur à 32 767 | `GARER A,0,'F'` | LSE15 l. 1716 | fichiers | 50 `=` | |
| X-55 | Fichier plein, ou `GARER` impossible faute de place | | LSE15 l. 1741 | fichiers | 52 `=` | |
| X-56 | Nom de fichier incorrect | `GARER A,1,''` | — | fichiers | 53 `=` | |
| X-57 | Fichier inexistant, `CHARGER` sans 4ᵉ paramètre | | LSE15 l. 1789 | fichiers | 55 `=` | |
| X-58 | Enregistrement inexistant, `CHARGER` sans 4ᵉ paramètre | | LSE15 l. 1789 | fichiers | 56 `=` (second libellé) | |
| X-59 | Cible de `CHARGER` déjà d'un autre type que l'enregistrement | | LSE15 l. 1751, l. 1768 | fichiers, `donnees` | 47 `=` (second libellé) | |
| X-60 | Paramètre compte rendu non numérique | | LSE15 l. 1718 | fichiers | 56 `=` | |
| X-61 | Module introuvable pour `EXECUTER` | `EXECUTER 'XYZ'` | LSE15 l. 1444 | bibliothécaire | — ou F 05 | voir RX7 |

## I. Ressources et erreurs internes

| Point | Situation | Exemple | Source | Détecté par | M82 | Remarque |
|---|---|---|---|---|---|---|
| X-62 | Mémoire saturée (objet, pile, cadre) | tableaux 20×20 (PG34) | INRDP75 l. 3485, l. 3542 (`(EX.) 03`) | gestionnaire d'objets, contexte | 03 `=` | attesté |
| X-63 | Séquence compilée impossible à exécuter (incohérence interne) | | Tomes I-II §10.2 (VM-1002) | interpréteur | 96 `=` | ne doit jamais apparaître |

## J. Commandes du moniteur

| Point | Situation | Exemple | Source | Détecté par | M82 | Remarque |
|---|---|---|---|---|---|---|
| X-64 | `EXECUTER A PARTIR DE n1`, ligne `n1` absente | | LSE15 l. 2307 ; INRDP75 l. 586 | contexte (INITIALISER) | 75 `≈` | voir RX7 |
| X-65 | `CONTINUER` après une erreur, un `TERMINER`, ou une ligne entrée | | LSE15 l. 2321 | moniteur | — | relève du chapitre du moniteur |
| X-66 | `REPRENDRE EN` après une modification du programme | | LSE15 l. 2329 | moniteur | — | relève du chapitre du moniteur |

---

## Points à arbitrer

- **RX1. Variable non définie : un numéro, ou un par contexte ?** MEMENTO82 et INRDP75 donnent des numéros différents selon l'endroit où la variable est lue : 05 dans une expression arithmétique, 62 pour une affectation ou un affichage, et 10 (attesté sur T1600, INRDP75 l. 2059) dans une relation. La correction est la même : donner une valeur à la variable.
- **RX2. Sorties de procédure contraires à la position d'appel.**
  - LSE15 §7-5 ne décrit l'abandon de l'expression que pour `RETOUR EN`. Le chapitre État d'exécution (§5.5, étape 6) l'a étendu à `RETOUR`, alors que MEMENTO82 (15) en fait une erreur.
  - Pour `RESULTAT` dans une procédure appelée en instruction, le chapitre jette la valeur sans erreur ; MEMENTO82 (22) en fait une erreur, LSE15 est muet.
- **RX3. Paramètre `LOCAL` sans valeur.** MEMENTO82 (14) dit l'erreur détectée « à l'utilisation du paramètre et non à l'entrée de la procédure » ; LSE15 §7-8.1 dit seulement qu'« une erreur sera détectée ». Le chapitre État d'exécution (§12) laisse le moment ouvert.
- **RX4. `n1` de `FAIRE` invalide.** MEMENTO82 n'a pas d'entrée de même sens. Les candidats par proximité sont 37 (numéro de ligne nul) et 39 (boucles mal imbriquées).
- **RX5. Fin de programme et ligne inexistante.** IMP-0154 range le dépassement de la dernière ligne dans « la même catégorie d'erreur » qu'un branchement vers une ligne inexistante, mais MEMENTO82 les distingue : 58 et 75.
- **RX6. Position de départ dans les fonctions chaîne.** MEMENTO82 a deux entrées voisines : 11 « erreur de position de départ dans fonction chaîne » et 26 « position de départ supérieure à la longueur + 1 dans SCH ou PTR ou CNB ».
- **RX7. Rattachements faibles.**
  - X-10 : tableau entier hors contexte admis.
  - X-50 : redéclaration d'un tableau passé en paramètre.
  - X-61 : module introuvable pour `EXECUTER` : nature d'exécution ou erreur de fichier (`F 05`) ?
  - X-64 : `EXECUTER A PARTIR DE` vers une ligne absente : nature d'exécution affichée sans `EN LIGNE`, ou erreur du dialogue ?
