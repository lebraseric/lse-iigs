# Inventaire des points de détection d'erreur de compilation — 2026-10-22

**Statut :** document de travail, subordonné au chapitre Compilateur du Tome V. Il ne fixe aucun code d'erreur, ne modifie aucune décision `IMP-*` et ne crée aucune règle de langage.
**Objet :** recenser chaque situation distincte dans laquelle `compilateur_compiler` rejette une ligne, afin de servir de grille de correspondance avec la liste des codes d'erreur de MEMENTO82 (« MEMENTO LSE », février 1982, réf. R-1017 F, implémentation sur MICRAL 80-22G, R2E ; versée au dépôt le 2026-10-22, `02-HISTORIQUE/MEMENTO82.pdf`, transcrite dans `02-HISTORIQUE/MEMENTO82-transcription.md` ; statut normatif à arbitrer).
**Origine :** exigence d'une granularité fine des diagnostics (décision d'Éric du 2026-10-22), fondée sur NOYELLE89 : « un outil doit faciliter au maximum la vie de celui qui l'emploie » ; « comme l'erreur est humaine, il fallait également que ce produit résiste à n'importe quelle fausse manipulation ».

---

## 1. Méthode

Trois sources ont été parcourues :

1. **Le chapitre Compilateur** (§4 à §13, §19) : points de détection énoncés explicitement, avec leur code provisoire au sens d'IMP-0168.
2. **La grammaire reconstruite** `LSE15Mitra.g4` (règles syntaxiques et lexicales) : chaque terminal attendu par une règle définit un point de détection implicite. Le chapitre les couvre par le principe général de la descente récursive (§5, « Organisation interne »), sans les nommer. La grammaire n'est pas une source historique (§1.3 du chapitre) : elle sert ici d'outil de recensement, pas de fondement.
3. **La fiche de référence de codage** (§8, bornes et limites) : bornes contrôlées à la compilation.

Les colonnes des tables sont les suivantes.

- **Fondement** : la source du corpus qui fonde le rejet, lorsqu'elle existe ; sinon la règle de la grammaire, ou « — ».
- **Tome V** : la section ou la décision du chapitre Compilateur qui énonce le point ; « implicite » s'il ne découle que de la grammaire.
- **Code actuel** : le code provisoire employé par le chapitre ; « non précisé » si le chapitre prévoit le rejet sans code, « non traité » si le chapitre ne prévoit pas le cas.
- **M82** : le code de compilation de MEMENTO82 (annexe I) qui couvre la situation. « ? » marque un rattachement plausible mais incertain (libellé plus large ou plus vague) ; « (attesté T1600) » signale que le code apparaît pour cette situation dans les sorties T1600 de Victor Duruy (`02-HISTORIQUE/Releve-sorties-T1600-Victor-Duruy.md`) ; « — » : aucun code.

Un point par situation que l'utilisateur doit pouvoir distinguer. Deux situations qui appellent une correction différente de la part de l'utilisateur sont deux points, même si l'analyseur les détecte au même endroit.

---

## 2. Ligne et numéro de ligne

| N° | Situation | Exemple | Fondement | Tome V | Code actuel | M82 |
|---|---|---|---|---|---|---|
| D-01 | Texte de plus de 79 caractères, hors fin de ligne | ligne de 80 caractères | INRDP75, « Composition d'une ligne » | §9, étape 1 | `ERR_LIGNE_TROP_LONGUE` | C 63 (attesté T1600, sens déduit, IMP-0185) |
| D-02 | Numéro non suivi d'un espace ni d'un `*` | `60` ; `5A←1` | INRDP75, « Composition d'une ligne » ; LSE15 §1 | §5 bis, IMP-0182 | `ERR_SYNTAXE` | C 07 |
| D-03 | Numéro de ligne nul | `0 A←1` | INRDP75 (« un numéro compris entre 1 et 250/255 ») | §9, étape 3 | `ERR_HORS_PROFIL` (observation O3) | C 52 |
| D-04 | Numéro admis par l'autre profil seulement (251 à 255 sous MITRA 15) | `251 AFFICHER 1` | INRDP75, « Composition d'une ligne » | §8 bis, IMP-0180 | `ERR_HORS_PROFIL` | nature « au-delà de la borne du profil » (à partir de 100) |
| D-05 | Numéro supérieur à 255 | `300 A←1` | INRDP75, « Composition d'une ligne » | §9, étape 3 | `ERR_HORS_PROFIL` (observation O3) | C 52 au-delà de 32000 ; nature « au-delà de la borne du profil » (à partir de 100) de 256 à 32000 |
| D-06 | Texte inattendu après une instruction complète | `5 A←1 B` ; `5 X←A+B)` | — | §9, étape 6 | `ERR_SYNTAXE` | C 45 si `)` ; C 19 si début d'opérande ; C 53 sinon (IMP-0191) |

## 3. Lexique

| N° | Situation | Exemple | Fondement | Tome V | Code actuel | M82 |
|---|---|---|---|---|---|---|
| D-07 | Lettre minuscule hors constante-chaîne et commentaire | `5 x←1` | — (IMP-0182) | §5 bis | `ERR_SYNTAXE` | C 01 ? |
| D-08 | Caractère étranger au jeu du langage | `5 A←B%C` | grammaire, jetons | implicite | `ERR_SYNTAXE` | C 01 |
| D-09 | Mot du langage non suivi du terminateur requis | `5 SI(A>0) ALORS B←1` ; `5 AFFICHERA` | LSE15 §1, Remarque 1 ; INRDP75, « Instruction de sortie. Format » | §5 bis, IMP-0182 | `ERR_SYNTAXE` | C 53 ? |
| D-10 | Identificateur de plus de cinq caractères | `5 ALPHAB←1` | LSE15 §3-1.2 ; INRDP75, « Règle pour les identificateurs » | — | non traité (observation O1) | C 03 (attesté T1600, LAFOND83) ; C 05 pour un nom de procédure |
| D-11 | Mot réservé employé comme identificateur | `5 TANT←1` | LSE15 §3-1.2 ; INRDP75, note 3, Annexe VI | §8 bis, IMP-0180 ; §19 | non précisé | — |
| D-12 | Mot réservé propre au profil employé comme identificateur | `5 DAT←1` (T1600) ; `5 REPRENDRE←1` (MITRA 15) | INRDP75, Annexe VI | §8 bis ; §19 | non précisé | — |
| D-13 | Constante-chaîne non terminée | `5 AFFICHER 'ABC` | INRDP75 (constantes-chaîne entre apostrophes) | implicite | `ERR_SYNTAXE` | C 08 |
| D-14 | Constante numérique mal formée (exposant sans chiffre) | `5 X←3E` ; `5 X←3E+` | LSE15 §3-1.1 | implicite | `ERR_SYNTAXE` | C 09 |
| D-15 | Constante numérique hors de la plage des réels | `5 X←1E99` | — | — | non traité (observation O2) | C 09 |
| D-16 | `&` non suivi d'une lettre | `5 X←&1(2)` | LSE15 §7-2 | §13.4 (déclaration) ; implicite (appel) | `ERR_SYNTAXE` | C 06 |

## 4. Expressions

| N° | Situation | Exemple | Fondement | Tome V | Code actuel | M82 |
|---|---|---|---|---|---|---|
| D-17 | Opérande attendu (expression vide ou tronquée) | `5 A←` ; `5 A←B+*` | — | §19 | `ERR_SYNTAXE` | C 19 |
| D-18 | Deux opérateurs adjacents | `5 X←A*-B` | LSE15 §3-3.3 ; INRDP75 | §5, « Précédence et associativité » | `ERR_SYNTAXE` | C 19 |
| D-19 | Deux opérandes adjacents (opérateur manquant) | `5 X←A B` | — | §9, étape 6 | `ERR_SYNTAXE`, confondu avec D-06 (observation O4) | C 19 |
| D-20 | Parenthèse fermante manquante | `5 X←(A+B` | — | implicite | `ERR_SYNTAXE` | C 20 |
| D-21 | Crochet fermant manquant | `5 X←A[1` | — | implicite | `ERR_SYNTAXE` | C 16 |
| D-22 | Plus de deux indices | `5 X←A[1,2,3]` | grammaire, `lvalue` ; RCI-M IMP-0120 | implicite | `ERR_SYNTAXE` | C 16 ? |
| D-23 | Fonction usuelle : nombre d'arguments incorrect | `5 X←SIN(1,2)` ; `5 X←POS('A',1)` | grammaire, `functionCall` | implicite | `ERR_SYNTAXE` | C 26 ? |
| D-24 | Fonction usuelle : argument facultatif final qui n'est pas un identificateur (SCH, GRL, CNB) | `5 X←CNB(C,1,2)` | grammaire, `functionCall` | implicite | `ERR_SYNTAXE` | C 26 ? |
| D-25 | Graphie de TEM ou DAT propre à l'autre profil | `5 X←TEM` (MITRA 15) ; `5 X←DAT()` (T1600) ; `TEM()` est admis sous T1600 (IMP-0189) | INRDP75 l. 850, l. 1442 ; relevé T1600 de Victor Duruy | §8 bis, IMP-0180, IMP-0189 | `ERR_HORS_PROFIL` | — |
| D-26 | Fonction propre au T1600 sous MITRA 15 | `5 X←SH(1)` | INRDP75, Annexe III §1, §4 | §8 bis | `ERR_HORS_PROFIL` | — |
| D-27 | Symbole `$` ou `@` dans un sens propre à l'autre profil | `5 ALLER EN @+3` (MITRA 15) ; `5 I←@50` (T1600) | INRDP75, Annexe III §3 | §8 bis | `ERR_HORS_PROFIL` | C 93 (en partie) |
| D-28 | Expression conditionnelle sans `SINON` | `5 X←SI A=0 ALORS 1` | RCI-M §4.8.5 ; grammaire, `condExpr` | implicite | `ERR_SYNTAXE` | C 21 |
| D-29 | Condition sans opérateur de relation | `5 SI A ALORS B←1` | grammaire, `condition` | implicite | `ERR_SYNTAXE` | C 17 |

## 5. Instructions

| N° | Situation | Exemple | Fondement | Tome V | Code actuel | M82 |
|---|---|---|---|---|---|---|
| D-30 | Identificateur en tête d'instruction non suivi de `←` (instruction non reconnue) | `5 IMPRI A` | — | implicite | `ERR_SYNTAXE` | C 38 (attesté T1600) |
| D-31 | Cible d'affectation qui n'est pas une variable | `5 (A)←1` | grammaire, `lvalue` | implicite | `ERR_SYNTAXE` | C 38 ou C 55 |
| D-32 | `ALLER` non suivi de `EN` | `5 ALLER 10` | — | implicite | `ERR_SYNTAXE` | C 37 |
| D-33 | `SI` sans `ALORS` | `5 SI A=0 B←1` | LSE15 §5-2 | implicite | `ERR_SYNTAXE` | C 22 |
| D-34 | `DEBUT` sans `FIN` | `5 DEBUT A←1;B←2` | grammaire, `compoundStmt` | implicite | `ERR_SYNTAXE` | C 36 (attesté T1600) |
| D-35 | `FIN` sans `DEBUT` | `5 A←1;FIN` | grammaire, `compoundStmt` | implicite | `ERR_SYNTAXE` | C 55 |
| D-36 | `FAIRE` : `POUR` manquant | `5 FAIRE 8 I←1 JUSQUA 3` | LSE15 §5-5 | §6, « Application à FAIRE » | `ERR_SYNTAXE` | C 32 ? |
| D-37 | `FAIRE` : variable contrôlée non simple | `5 FAIRE 8 POUR A[1]←0 JUSQUA 3` | LSE15 §5-5.1 | §6 ; §19 | `ERR_SYNTAXE` | C 32 |
| D-38 | `FAIRE` : `←` manquant après la variable contrôlée | `5 FAIRE 8 POUR I 1 JUSQUA 3` | LSE15 §5-5 | §6 | `ERR_SYNTAXE` | C 33 |
| D-39 | `FAIRE` : ni `JUSQUA` ni `TANT QUE` | `5 FAIRE 8 POUR I←1 PAS 2` | LSE15 §5-5 | §6 | `ERR_SYNTAXE` | C 34 |
| D-40 | `TANT` non suivi de `QUE` | `5 FAIRE 8 POUR I←1 TANT I<3` | — | §6 | `ERR_SYNTAXE` | C 34 |
| D-41 | `TABLEAU` : dimension absente, plus de deux dimensions ou crochet manquant | `5 TABLEAU A` ; `5 TABLEAU A[1,2,3]` | grammaire, `tableauItem` | implicite | `ERR_SYNTAXE` | C 14, C 39 |
| D-42 | `CHAINE` ou `LIBERER` : identificateur attendu | `5 CHAINE 3` | grammaire, `chaineDecl`, `libererDecl` | implicite | `ERR_SYNTAXE` | C 35 |
| D-43 | `LIRE` : cible qui n'est pas une variable | `5 LIRE A+1` ; `5 LIRE 'X'` | grammaire, `lireItem` | implicite | `ERR_SYNTAXE` | — |
| D-44 | `GARER` ou `CHARGER` : argument manquant, ou premier argument qui n'est pas un identificateur | `5 GARER A,1` | grammaire, `garerStmt`, `chargerStmt` | implicite | `ERR_SYNTAXE` | C 29, C 44 |
| D-45 | `EXECUTER` ou `SUPPRIMER` : plus de deux arguments | `5 EXECUTER 'P',1,2` | grammaire, `executerStmt`, `suppressionStmt` | implicite | `ERR_SYNTAXE` | C 29 (SUPPRIMER) ; — (EXECUTER) |
| D-46 | `RESULTAT` sans expression | `5 RESULTAT` | grammaire, `resultatStmt` | implicite | `ERR_SYNTAXE` | C 19 ? |
| D-47 | Instruction propre à l'autre profil | `5 UN T` (MITRA 15) ; `5 SUPPRIMER 'F'` (T1600) | INRDP75, Annexe III §2, §4 | §8 bis | `ERR_HORS_PROFIL` | — |
| D-48 | `?` sur une ligne numérotée | `5 ?38-24+47` | LSE15 §9-5 | IMP-0173 ; §19 | `ERR_SYNTAXE` (observation O4) | nature propre (à partir de 100) |

## 6. Format d'AFFICHER

| N° | Situation | Exemple | Fondement | Tome V | Code actuel | M82 |
|---|---|---|---|---|---|---|
| D-49 | Crochet fermant du format manquant | `5 AFFICHER[F2.0 A` | — | implicite | `ERR_SYNTAXE` | C 40 |
| D-50 | Spécification de format inconnue | `5 AFFICHER[Z] A` | LSE15 §6-2.2, §6-2.3 ; grammaire, `layoutSpec`, `descSpec` | implicite | `ERR_SYNTAXE` | C 42 |
| D-51 | `e` ou `d` de plus de deux chiffres dans `Fe.d` ou `Ee.d` | `5 AFFICHER[F100.1] A` | LSE15 §6-2.3 | §5, « Génération d'AFFICHER » ; §19 | `ERR_SYNTAXE` | C 42 ? |
| D-52 | Facteur de répétition fixe nul | `5 AFFICHER[0'*']` | LSE15 §6-2.1 (1 à 99) | — | non précisé (observation O3) | C 41 |
| D-53 | Facteur de répétition fixe au-delà de la borne du profil | `5 AFFICHER[63'*']` (T1600) | INRDP75, note 7 du chapitre « Instruction de sortie. Format » | §8 bis | `ERR_HORS_PROFIL` | C 41 (borne 63) |
| D-54 | Facteur de répétition fixe supérieur à 99 | `5 AFFICHER[100'*']` | LSE15 §6-2.1 ; INRDP75, note 7 | §8 bis | `ERR_HORS_PROFIL` (observation O3) | C 41 |
| D-55 | Facteur de répétition variable `*` devant une description | `5 AFFICHER[*F2.0] N,A` | grammaire, `descFormatSpec` (à vérifier dans LSE15 §6-2.6) | implicite | `ERR_SYNTAXE` | C 42 ? |
| D-56 | Plus de spécifications de description que de valeurs | `5 AFFICHER[2F4.1] A` | LSE15 §6-2.4 | IMP-0178 | `ERR_FORMAT_DESCRIPTIONS` | C 54 |

## 7. Procédures

| N° | Situation | Exemple | Fondement | Tome V | Code actuel | M82 |
|---|---|---|---|---|---|---|
| D-57 | `PROCEDURE` ailleurs qu'en tête de ligne | `5 A←1;PROCEDURE &P()` | LSE15 §7-6 ; grammaire, commentaire [H] | §13.4, IMP-0175 | `ERR_SYNTAXE` | C 53 ou C 55 ? |
| D-58 | `PROCEDURE` sur une ligne non numérotée | `PROCEDURE &P(A)` | grammaire, commentaire [H] | §8 ; §19 | `ERR_SYNTAXE` (observation O4) | C 58 |
| D-59 | Nom de procédure absent ou mal formé | `5 PROCEDURE P(A)` | LSE15 §7-2 ; INRDP75, chapitre Procédures, §2-1 | §13.4 | `ERR_SYNTAXE` | C 49 ; C 05, C 06 |
| D-60 | Parenthèses de la liste des paramètres absentes | `5 PROCEDURE &P` | LSE15 §7-2 | §13.4 | `ERR_SYNTAXE` | C 49 |
| D-61 | Paramètre formel indicé | `5 PROCEDURE &P(A[1])` | LSE15 §7-3 ; INRDP75, chapitre Procédures, §2-1, note 9 | §13.4 ; §19 | `ERR_SYNTAXE` | C 51 |
| D-62 | Paramètre formel en double | `5 PROCEDURE &P(A,A)` | — (rejet non attesté, IMP-0175) | §13.4 | `ERR_DECLARATION_PROCEDURE` | C 47 (paramètres variables) ; C 48 (paramètres noms de procédure) |
| D-63 | Nom de procédure dans la liste `LOCAL` | `5 PROCEDURE &P(A) LOCAL &F` | LSE15 §7-8.2 ; grammaire, commentaire [I] | §13.4 ; §19 | `ERR_SYNTAXE` | C 35 |
| D-64 | Nom en double dans la liste `LOCAL` | `5 PROCEDURE &P(A) LOCAL I,I` | — (rejet non attesté, IMP-0175) | §13.4 | `ERR_DECLARATION_PROCEDURE` | C 47 ? |
| D-65 | Texte inattendu après la déclaration (ni `;` ni fin de ligne) | `5 PROCEDURE &P(A) X` | LSE15 §7-7 | §13.4 | `ERR_SYNTAXE` | C 46 ? |
| D-66 | Appel de procédure : parenthèse fermante manquante ou liste d'arguments mal formée | `5 &P(1,` | — | implicite | `ERR_SYNTAXE` | C 46 |
| D-87 | Liste de paramètres formels mal formée : paramètre qui n'est pas un nom, séparateur ou parenthèse fermante manquants | `5 PROCEDURE &P(3)` ; `5 PROCEDURE &P(A B)` | LSE15 §7-3 | §13.4, IMP-0175 | `ERR_C_PARAMETRES_DECLARATION` (ajouté le 2026-10-23) | C 46 |
| D-83 | Paramètre formel homonyme de la procédure déclarée | `5 PROCEDURE &P(&P)` | — (rejet non attesté pour les deux profils ; MEMENTO82 C 48) | §13.4, IMP-0175 (révisée le 2026-10-22) | `ERR_DECLARATION_PROCEDURE` | C 48 |

## 8. Mode machine de bureau

| N° | Situation | Exemple | Fondement | Tome V | Code actuel | M82 |
|---|---|---|---|---|---|---|
| D-67 | Instruction faisant intervenir un numéro de ligne (`ALLER EN`, `FAIRE`, `RETOUR EN`), à toute profondeur | `ALLER EN 10` ; `SI X>0 ALORS ALLER EN 5` | INRDP75, « Mode machine de bureau » ; LSE15 §9-5 | IMP-0172 | `ERR_INSTRUCTION_INTERDITE_BUREAU` | C 58 |
| D-68 | `RETOUR` ou `RESULTAT` | `RESULTAT 1` | grammaire, commentaire [J] | IMP-0172 | `ERR_INSTRUCTION_INTERDITE_BUREAU` | C 58 |
| D-69 | Appel de procédure employé comme instruction | `&FACT(5)` | grammaire, commentaires [J], [K] | IMP-0172 | `ERR_INSTRUCTION_INTERDITE_BUREAU` (le §19 dit seulement « rejet ») | C 58 |
| D-70 | Appel de fonction dans une expression | `X←&FACT(5)` | grammaire, commentaire [K] | IMP-0172 ; §19 | `ERR_INSTRUCTION_INTERDITE_BUREAU` (le §19 dit seulement « rejet ») | C 58 |
| D-71 | Déclaration (`TABLEAU`, `CHAINE`, `LIBERER`) sous le profil MITRA 15 | `TABLEAU M[3]` | INRDP75 (« Sur MITRA certaines instructions ne sont pas autorisées en mode machine de bureau, notamment les déclarations ») | IMP-0172, §8 bis | `ERR_INSTRUCTION_INTERDITE_BUREAU` | C 58 |

## 9. Ressources

| N° | Situation | Exemple | Fondement | Tome V | Code actuel | M82 |
|---|---|---|---|---|---|---|
| D-72 | Séquence compilée de plus de 255 octets | aucune ligne du corpus (RCI-M IMP-0125) | — | IMP-0165 | `ERR_LIGNE_TROP_COMPLEXE` | — |
| D-73 | Mémoire insuffisante pour créer l'objet compilé | — | INRDP75, note 16 : message « zone données utilisateur pleine » | §9, étape 7 | `ERR_MEMOIRE_INSUFFISANTE` | C 57 |
| D-74 | Mémoire insuffisante pour le texte d'une constante partagée | — | idem | IMP-0181 | `ERR_MEMOIRE_INSUFFISANTE` | C 57 |
| D-75 | Mémoire insuffisante pour agrandir la table des symboles ou des constantes | — | idem | IMP-0167, IMP-0181 (`realloc`) | non précisé (observation O5) | C 57 |
| D-76 | Capacité des identifiants (65 535 symboles ou constantes) atteinte | — | — | RCI-M IMP-0104, IMP-0128 | non traité (observation O5) | nature propre (à partir de 100) |


## 9 bis. Contrôle de type (ajouté le 2026-10-22, IMP-0188)

| N° | Situation | Exemple | Fondement | Tome V | Code actuel | M82 |
|---|---|---|---|---|---|---|
| D-77 | Chaîne de type déterminé là où un nombre est requis : opérande arithmétique, changement de signe, numéro de ligne, valeurs de `FAIRE`, facteur `*` | `5 X←'A'+1` ; `5 ALLER EN 'A'` | LSE15, l. 972, l. 1678 | §5 ter, IMP-0188 | à créer | C 27 |
| D-78 | Nombre de type déterminé là où une chaîne est requise : opérande de `!` | `5 C←3!'A'` | LSE15 §4-4.1 | §5 ter, IMP-0188 | à créer | C 28 |
| D-79 | Argument de fonction usuelle de type déterminé contraire à sa signature | `5 X←SIN('A')` ; `5 X←LGR(3)` | RCI-M §4.11 (signatures) | §5 ter, IMP-0188 | à créer | C 27 ou C 28 |
| D-80 | Indice de type chaîne | `5 X←T['X']` | LSE15, l. 202 | §5 ter, IMP-0188 | à créer | C 15 |
| D-81 | Comparaison entre un nombre et une chaîne de types déterminés | `5 SI 'A'=1 ALORS X←1` | LSE15, l. 1048 | §5 ter, IMP-0188 | à créer | C 11 |
| D-82 | Dimension de `TABLEAU` de type chaîne | `5 TABLEAU T['A']` | LSE15 §3-1.2 | §5 ter, IMP-0188 | à créer | C 14 |

## 9 ter. Équivalents numériques (ajouté le 2026-10-22, IMP-0190)

| N° | Situation | Exemple | Fondement | Tome V | Code actuel | M82 |
|---|---|---|---|---|---|---|
| D-84 | Équivalent numérique sans point final, ou avec un caractère autre qu'un chiffre ou un espace | `5 AFFICHER .27` | MEMENTO82 C 08 | §8 bis, IMP-0190 | à créer | C 08 |
| D-85 | Équivalent numérique hors des bornes d'`EQC` du profil | `5 AFFICHER .128.` (T1600) | INRDP75 l. 1416 | §8 bis, IMP-0190 | à créer | — (à partir de 100) |
| D-86 | Équivalent numérique sous le profil MITRA 15 | `5 AFFICHER .27.` (MITRA 15) | LSE15 l. 517 | §8 bis, IMP-0190 | à créer | — (à partir de 100) |

---

## 10. Observations

**O1 — Identificateur trop long (D-10). Résolu le 2026-10-22 : IMP-0191** (C 03, C 05). Texte initial : Le corpus limite un identificateur à cinq caractères (LSE15 §3-1.2 ; INRDP75). Le chapitre ne dit pas ce que fait l'analyseur devant une suite de six lettres ou chiffres qui n'est pas un mot du langage : il lit « la plus longue suite » (IMP-0182), puis ne la rejette nulle part explicitement. Une troncature silencieuse confondrait deux identificateurs distincts ; un rejet est vraisemblable, mais il n'est pas écrit.

**O2 — Constante hors de la plage des réels (D-15). Résolu le 2026-10-22 : IMP-0191** (rejet, C 09 ; un dépassement vers zéro suit la conversion SANE). Texte initial : Ni le chapitre ni RCI-M ne disent ce que produit une constante dont la valeur dépasse la plage du `float` SANE. Le corpus documente le comportement de `CNB` dans ce cas (LSE15, l. 825 : la valeur devient « ce plus grand nombre représentable »), mais rien pour une constante écrite dans le texte. C'est un choix à faire : rejet à la compilation, ou alignement sur la règle de `CNB`.

**O3 — `ERR_HORS_PROFIL` employé pour des valeurs invalides sous les deux profils (D-03, D-05, D-52, D-54). Résolu le 2026-10-22 : IMP-0187, IMP-0191** (C 52, nature « au-delà de la borne du profil », C 41). Texte initial : Le §9 (étape 3) et le §8 bis appliquent `ERR_HORS_PROFIL` à tout numéro de ligne ou facteur de répétition hors de la plage du profil. Un numéro 0 ou 300, un facteur 0 ou 100, n'est valide sous aucun profil. Le message « hors profil » orienterait l'utilisateur vers une fausse piste. Il faut distinguer « valeur admise seulement par l'autre implantation » et « valeur hors de la plage du langage ».

**O4 — Situations distinctes confondues sous un même code. Résolu le 2026-10-22 : IMP-0187, IMP-0191** (C 45, C 19, C 53 ; nature propre pour `?` ; C 58). Texte initial : Quelques cas où la correction attendue de l'utilisateur diffère de ce que suggère le code actuel :
- D-19 (opérateur manquant) et D-06 (texte en trop) sont détectés au même endroit. L'utilisateur qui a oublié un opérateur reçoit un diagnostic de texte superflu.
- D-48 (`?` sur une ligne numérotée) et D-58 (`PROCEDURE` sans numéro) relèvent de la distinction entre les deux formes de ligne, pas d'une faute de syntaxe ordinaire. Le chapitre leur attribue `ERR_SYNTAXE`, alors qu'il réserve `ERR_INSTRUCTION_INTERDITE_BUREAU` au cas symétrique.
- D-67 à D-71 sont cinq natures d'interdiction sous un seul code.

Ces regroupements sont à revoir à la lumière de la granularité de MEMENTO82.

**O5 — Ressources (D-75, D-76). Résolu le 2026-10-22 : IMP-0191** (C 57 ; nature propre pour la capacité ; domaine `COMPILATION`). Texte initial : L'échec d'un `realloc` de la table des symboles ou des constantes n'a pas de code. La capacité de 65 535 entrées n'est pas contrôlée ; elle est sans doute inatteignable en 4 Mio, mais ce n'est pas démontré. Par ailleurs, la répartition de ces erreurs entre les domaines `COMPILATION` et `SERVICE` du Tome IV (§17.1) n'est pas tranchée. Le seul libellé attesté pour un manque de place, « zone données utilisateur pleine » (INRDP75, note 16), apparaît à la lecture d'un programme.

**O6 — Position du diagnostic. Résolu le 2026-10-22 : IMP-0191** (premier caractère non accepté, ou début de la construction déjà lue ; jamais affichée). Texte initial : `DiagnosticCompilation` porte une position (IMP-0168), mais aucune règle générale ne dit où elle se place : sur le caractère fautif, ou au début de la construction en cours. Le chapitre ne la fixe qu'en deux endroits (§8 bis, « positionné sur la construction » ; §19, `&FACT`). Une granularité fine suppose une règle uniforme.

**O7 — Classe historique. Résolu le 2026-10-22 :** INRDP75 (l. 995) montre `?''!'PIERRE` → `ERREUR  C 08`, et LAFOND83 (p. 19) range dans les erreurs de compilation toute faute d'analyse d'une instruction en mode machine de bureau. Une erreur de syntaxe sur une ligne machine de bureau relève donc de la classe C. Texte initial : INRDP75 (l. 668-675, l. 1495, l. 8217) range toute erreur de compilation dans la classe `C.n`, et en donne le comportement : la ligne « n'est pas prise en compte », et l'utilisateur doit la refrapper en totalité. Le texte ne dit pas explicitement si une erreur de syntaxe sur une ligne machine de bureau relève de `C.n` ou de `Mn`. La réalisation actuelle (EXI-0004, rejet atomique) correspond au comportement décrit.

---

## 11. Synthèse

| Code actuel | Nombre de points |
|---|---|
| `ERR_SYNTAXE` | 48 |
| `ERR_HORS_PROFIL` | 9 |
| `ERR_INSTRUCTION_INTERDITE_BUREAU` | 5 |
| `ERR_MEMOIRE_INSUFFISANTE` | 2 |
| `ERR_DECLARATION_PROCEDURE` | 2 |
| `ERR_LIGNE_TROP_LONGUE` | 1 |
| `ERR_LIGNE_TROP_COMPLEXE` | 1 |
| `ERR_FORMAT_DESCRIPTIONS` | 1 |
| Non précisé | 4 |
| Non traité | 3 |
| **Total** | **76** |

Chaque point est compté une fois, sous le code que lui attribue le chapitre, même lorsqu'une observation le remet en cause.

**Usage prévu.**
1. Remplir la colonne M82 et relever les erreurs de MEMENTO82 sans point correspondant : fait le 2026-10-22 (§12).
2. Trancher les observations O1 à O6 et les points R1 à R7 (§12).
3. Rédiger la partie « compilation » du chapitre Gestionnaire des erreurs, qui fixera les codes définitifs.
4. Réviser le chapitre Compilateur : remplacer chaque code provisoire du point de détection par le code définitif, et ajouter au §19 un test par point.


---

## 12. Recoupement avec MEMENTO82 et les attestations (2026-10-22)

Sources : `02-HISTORIQUE/MEMENTO82-transcription.md`, `02-HISTORIQUE/Releve-sorties-T1600-Victor-Duruy.md` et `02-HISTORIQUE/LAFOND83-releve.md`.

### 12.1 Couverture

- **66 des 76 points** reçoivent un code MEMENTO82, dont 15 avec réserve (« ? »). Les 10 points sans code sont la longueur de ligne (D-01), les mots réservés (D-11, D-12), les différences de profil (D-04, D-25, D-26, D-47), la cible de `LIRE` (D-43), `?` sur une ligne numérotée (D-48) et le débordement de la séquence compilée (D-72). D-27 n'est couvert qu'en partie, par C 93.
- **MEMENTO82 est bien plus fin que le chapitre Compilateur.** `ERR_SYNTAXE`, appliqué à 48 points, s'y répartit sur une trentaine de codes distincts. Les cinq interdictions du mode machine de bureau (D-67 à D-71), en revanche, y partagent un seul code, C 58.

### 12.2 Codes MEMENTO82 sans point correspondant

| Code | Libellé (abrégé) | Nature |
|---|---|---|
| C 04 | Plus de 101 identificateurs de plus d'une lettre | capacité propre à la machine |
| C 05 | Nom de procédure trop long | distinct de C 03 : à séparer dans D-10 |
| C 11, C 27, C 28 | Mélange de types ; chaîne au lieu d'arithmétique, et inversement | contrôle de type à la compilation (R4) |
| C 13, C 23, C 24, C 30 | Plus de cinq indices imbriqués, cinq expressions conditionnelles, neuf expressions, cinq fonctions | bornes d'imbrication propres à la machine |
| C 15 | Expression indicée non arithmétique | contrôle de type (R4) |
| C 25 | Plus de seize paramètres dans une procédure | capacité |
| C 43 | Dans un format, séparateur incorrect | à rattacher à D-49 ou à créer |
| C 45 | Plus de ")" que de "(" | aujourd'hui confondu avec D-06 |
| C 48 | Double déclaration de nom de procédure… | contredit le corpus (R5) |
| C 50 | Trop de noms de procédures | capacité |
| C 89 | `.` suivi d'une majuscule, ni `.VRAI.` ni `.FAUX.` | booléens littéraux, hors des deux profils |
| C 91 | `RETOUR EN` deux paramètres (provisoire) | hors des deux profils |
| C 92 | Spécification F ou E dans un format de lecture | `LIRE` avec format, hors du corpus des deux profils |
| C 93 | `@` non suivi d'entier dans une expression | rejoint le sens MITRA 15 de `@` (D-27) |

### 12.3 Points à arbitrer

**R1 — Statut de MEMENTO82 et numérotation. Tranché le 2026-10-22 : IMP-0185** (chapitre Gestionnaire des erreurs, §3). L'option retenue est un catalogue par nature, numéroté selon MEMENTO82 quand le numéro existe et à partir de 100 sinon, avec C 63 pour D-01 sous réserve. Ce choix s'appuie sur les attestations complémentaires d'INRDP75 (`C 08`, `(EX.) 30`, `(EX.) 03`, `(EX.) 10`) et de LAFOND83 p. 19 (`C 38`, `C 03`, `E160`), relevées au §2 de ce chapitre. Analyse ayant précédé la décision : Cinq rejets des sorties T1600 de Victor Duruy reçoivent un libellé MEMENTO82 qui convient :
- C 03 sur `AFFICHEP` ;
- C 36 sur une ligne coupée qui perd son `FIN` ;
- C 38 sur `PRE`, puis sur `ALHER EN 37` ;
- F 03 sur `DUPLIQUER $*,P`.

Les codes T1600 et MICRAL ont donc vraisemblablement une numérotation commune, au moins pour la compilation et les fichiers.

Deux réserves :
- `C 63` et la classe `FL` (T1600) sont absents de MEMENTO82 ;
- LAFOND83 atteste des codes à trois chiffres (`E210`, `M200`) sur une implantation non nommée.

Il reste à décider si la numérotation de MEMENTO82 est adoptée pour les deux profils, avec mention de sa provenance, et si oui dans quelles limites (codes de compilation seulement, ou aussi exécution et fichiers).

**R2 — Forme du message. Tranché le 2026-10-22 : IMP-0186** (chapitre Gestionnaire des erreurs, §4). La forme T1600 vaut pour les deux profils ; la ligne d'appel n'est pas affichée ; le code est seul par défaut et le libellé est disponible sur option de lancement. Texte initial : La forme T1600 attestée est `ERREUR  C 36`. Aucune attestation directe ne montre l'affichage d'une erreur d'exécution sur T1600. LAFOND83 donne `ERREUR E210 LIGNE 30` et `ERREUR M200`. Il faut choisir la forme affichée pour chaque classe, C, E (ou EX), M et F.

**R3 — Granularité du mode machine de bureau. Tranché le 2026-10-22 : IMP-0187** (chapitre Gestionnaire des erreurs, §5). Les natures sont distinctes, le numéro MEMENTO82 est partagé et chaque nature a son libellé. Texte initial : MEMENTO82 regroupe toutes les interdictions sous C 58. La granularité fine voulue par Éric irait au-delà ; il faut choisir entre le code unique attesté et des codes distincts.

**R4 — Contrôles de type à la compilation (C 11, C 15, C 27, C 28). Tranché le 2026-10-22 : IMP-0188** (chapitre Compilateur, §5 ter). Le contrôle est limité à ce que le texte de la ligne détermine ; il crée les points D-77 à D-82. Texte initial : Le chapitre Compilateur (§7, « Redéclaration incompatible ») ne contrôle aucun type à la compilation, au nom d'EXI-0019 : la catégorie d'un identificateur n'est connue qu'à l'exécution. Le LSE de MEMENTO82 contrôle au moins la nature des expressions, chaîne ou arithmétique. Une partie de ce contrôle ne dépend que du texte de la ligne : `'A'+1`, ou un indice chaîne littérale. Il faut décider si ce contrôle est repris, et dans quelle mesure, sans contredire EXI-0001 ni EXI-0019.

**R5 — Double déclaration de procédure (C 48). Tranché le 2026-10-22.** La double déclaration n'est pas rejetée et la divergence est consignée (chapitre Gestionnaire des erreurs, §6). Les doublons de paramètres se répartissent entre C 47 et C 48. Le paramètre homonyme de la procédure est rejeté (IMP-0175 révisée, D-83). Texte initial : MEMENTO82 la rejette à la compilation. LSE15 §7-6, Note, retient au contraire la déclaration de plus grand numéro (chapitre Compilateur, IMP-0176). C'est une divergence entre implantations, à documenter ; les deux profils restent régis par LSE15 et INRDP75.

**R6 — Borne du facteur de répétition (C 41). Tranché le 2026-10-22 :** les bornes d'INRDP75 sont conservées et la divergence est consignée (chapitre Gestionnaire des erreurs, §6). Texte initial : MEMENTO82 rejette un facteur « supérieur à 63 ou nul », soit une plage de 1 à 63. INRDP75 donne 1 à 62 sur T1600 et 1 à 99 sur MITRA 15. Les trois valeurs sont à garder distinctes ; c'est une donnée de plus pour le point O3 (`ERR_HORS_PROFIL`).

**R7 — Constats de langage issus des sorties T1600, hors du périmètre des erreurs mais touchant le compilateur. Tranché le 2026-10-22 (choix β) : IMP-0189 (`TEM()` admis sous T1600) et IMP-0190 (`.n.` sous T1600, bornes d'`EQC`)** (chapitre Compilateur, §8 bis). Les autres évolutions (`BOOLEEN`, tableaux de chaînes ou de booléens, `LIRE` avec format, `.VRAI.` et `.FAUX.`) restent hors du périmètre. Texte initial :
- `TEM()` avec parenthèses est accepté sur le T1600 de Victor Duruy (4EME - SINUS). INRDP75 (l. 850) dit le contraire, et IMP-0180 le rejette sous le profil T1600.
- La notation `.n.` (équivalent numérique dans une constante-chaîne, `AFFICHER .27 12.`, `'…'.8.'…'`) est attestée sur T1600 et dans MEMENTO82 (C 08), mais absente d'INRDP75 et de LSE15 ; ni la grammaire ni le chapitre Compilateur ne la couvrent.

Les sorties datent de 1980 à 1983, sous un système qui s'annonce `LSE F4`. Il faut établir si elles relèvent du profil « T1600 version 4 » d'INRDP75 (état de décembre 1974) ou d'une version ultérieure.
