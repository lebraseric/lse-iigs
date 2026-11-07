# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — Le gestionnaire des données : chaînes, tableaux, écriture et lecture des nombres

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V rédigé le 2026-10-24 (étape 5a de la feuille de route, découpage F0 retenu par Éric le 2026-10-24). Décisions IMP-0209 à IMP-0213, sur les points F1 à F3 retenus par Éric le 2026-10-24.
**Portée :** réalisation physique du module `src/noyau/donnees.c/.h` (Organisation du dépôt §2.1) :
- représentation des variables-chaîne et des tableaux dans leurs objets ;
- opérations sur les chaînes : concaténation, comparaison, fonctions de LSE15 §4-4.2 ;
- opérations sur les tableaux : création, accès indexé, élément sans valeur ;
- écriture d'un nombre selon les spécifications U, F et E, et `CCA` ;
- lecture d'un nombre (`CNB`).

Le chapitre ne couvre ni le cycle d'interprétation (chapitre de l'interpréteur), ni l'entrée-sortie sur la console (`AFFICHER`, `LIRE`), ni les fichiers-données (`GARER`, `CHARGER`).
**Rang normatif :** subordonné aux Tomes I-II (VM-1102, VM-1201, VM-1202), au Tome III, au Tome IV (§4.8), à EXI-0039, et aux chapitres État d'exécution (IMP-0179, IMP-0208) et Décompilateur (IMP-0201). Aucune disposition du présent chapitre ne peut contredire ces sources.

---

## 0. Avertissement de statut

Le comportement des chaînes, des tableaux et des fonctions est décrit en détail par LSE15 (§3-1.2, §4, §5-2, §6-2.3) et INRDP75 (§6) ; le présent chapitre s'y conforme et le cite. La représentation physique relève de décisions d'implémentation.

---

## 1. Rappel des contrats obligatoires

| Source | Contenu | Conséquence |
|---|---|---|
| LSE15 §4-1 | Une chaîne est une suite de caractères, positions numérotées à partir de 1 ; elle peut être vide | §2 |
| INRDP75 l. 817 | « Un caractère par octet » ; longueur « limitée à 255 caractères sur T1600 et théoriquement illimitée sur MITRA 15 » | IMP-0209 |
| FICH2, enr. 8 (`LSE F4`, Victor Duruy ; `numerisation0011`, relevé §3) | Quatre lignes de 68, 70, 67 et 66 caractères, soit 274 avec leurs espaces, dans un seul enregistrement de type chaîne | IMP-0209 |
| LSE15 §3-1.2 | Tableaux à une ou deux dimensions ; indice minimal 1 ; indice arrondi à l'entier le plus voisin ; borne déclarée strictement positive | IMP-0210 |
| LSE15 §6-3.4 | Un élément de tableau peut être « non défini » (`ZZZ[2] : non défini`) | IMP-0210 |
| LSE15 §6-1, §6-2.5 | Lecture et affichage d'un tableau entier ligne par ligne : `B[1,1]`, `B[1,2]`, …, `B[5,5]` | IMP-0210 |
| LSE15 §5-2 (l. 1052) ; INRDP75 l. 1853 | Comparaison des chaînes caractère par caractère sur le code ASCII ; à préfixe égal, la plus longue est la plus grande | §3.2 |
| LSE15 §4-4.2 | Définition des fonctions `SCH`, `GRL`, `CCA`, `EQC`, `DAT`, `LGR`, `POS`, `CNB`, `EQN`, `PTR`, `SKP` | IMP-0212 |
| LSE15 §6-2.3 | Spécifications F, E et U, et table d'exemples | IMP-0211 |
| État d'exécution IMP-0208 | Trois opérations dont dépend le contexte d'exécution | §6 |

---

## 2. Représentation des chaînes

**IMP-0209** — *Variable-chaîne : un objet portant sa longueur sur 2 octets, puis le texte ; 65 535 caractères au plus*
- Sujet : comment ranger le texte d'une variable-chaîne ou d'un résultat de chaîne dans un objet du gestionnaire d'objets, et jusqu'à quelle longueur ?
- Choix :
  - un objet chaîne commence par un octet de genre (`0` : chaîne), puis la longueur sur 2 octets (poids faible en tête, IMP-0184), puis les caractères, un par octet ; le texte commence donc au décalage 3 de l'objet ;
  - sous les deux profils, la longueur n'est bornée que par la mémoire et par les 2 octets de la longueur, soit 65 535 caractères ;
  - une chaîne plus longue est l'erreur EX 101, « chaîne trop longue » ;
  - un texte de la pile (`ChaineSurPile`, État d'exécution §4.2) désigne un objet et le décalage de son texte : 3 pour un objet chaîne, la position du texte dans le bloc pour une constante en ligne, 0 pour le texte d'une constante partagée (RCI-M IMP-0107).
- Alternatives envisagées : déduire la longueur de la taille de l'objet — écartée : le gestionnaire d'objets ne la donne pas, et un objet de longueur nulle occupe un octet (IMP-0194) ; 255 caractères sous le profil T1600 (INRDP75 l. 817) — écartée : `LSE F4` à Victor Duruy range une chaîne de 274 caractères (FICH2, enr. 8), et le profil T1600 réunit INRDP75 et les formes attestées à Victor Duruy, par ajout seulement ; une autre borne, 512 ou 1 024 caractères — écartée : aucune source ne donne celle de `LSE F4`.
- Justification : INRDP75 l. 817 pour MITRA 15 ; FICH2 pour le T1600 ; choix d'Éric du 2026-10-24 (point F1) et du 2026-10-31. L'octet de genre permet à `donnees_copier` de copier un objet chaîne ou tableau sans que l'appelant en connaisse la nature (IMP-0208).
- Impact mémoire : 3 octets d'en-tête par chaîne.
- Impact disque : sans objet (la représentation sur disque relève des fichiers-données).
- Impact performances : longueur en O(1).
- Impact historique : restitue la borne attestée du T1600.
- Références normatives : INRDP75 l. 817 ; LSE15 §4-1 ; IMP-0107, IMP-0184, IMP-0194, IMP-0208.
- Statut : retenu (Éric, 2026-10-24, point F1).

---

## 3. Opérations sur les chaînes

### 3.1 Concaténation

Le résultat de `a!b` est un objet chaîne transitoire neuf (État d'exécution §4.4), de longueur `LGR(a) + LGR(b)`, sous réserve de la borne d'IMP-0209.

### 3.2 Comparaison

Les chaînes sont comparées caractère par caractère, de gauche à droite, sur le code de chaque caractère (0 à 255) ; à la première différence, la chaîne au plus grand code est la plus grande ; si l'une est un préfixe de l'autre, la plus longue est la plus grande ; deux chaînes ne sont égales que si elles ont même longueur et mêmes caractères (LSE15 l. 1052-1054 ; INRDP75 l. 1853-1855).

### 3.3 Fonctions

**IMP-0212** — *Fonctions sur les chaînes : définition de LSE15 §4-4.2, compléments déduits pour les cas que LSE15 ne décrit pas*
- Sujet : LSE15 définit les onze fonctions par leur texte et des exemples ; quelques cas limites n'y figurent pas.
- Choix : chaque fonction suit LSE15 §4-4.2. Les positions et longueurs sont arrondies à l'entier le plus voisin (État d'exécution IMP-0179, conversion en entier). Les compléments suivants sont déduits :
  1. **Position de départ** β (SCH, GRL, PTR, CNB, SKP, POS, EQN) : 1 ≤ β ≤ longueur + 1 (LSE15 l. 594). β au-delà de longueur + 1 donne EX 26 pour `SCH`, `PTR` et `CNB` (MEMENTO82 26), EX 21 pour `EQN`, EX 11 dans tous les autres cas, y compris β < 1 (Gestionnaire des erreurs §9, RX6).
  2. **`SCH` à longueur** γ : γ < 0 donne EX 17 (proximité, Gestionnaire des erreurs §9) ; une longueur qui dépasse la fin de la chaîne s'arrête à la fin (`SCH('ABCD',3,4,L)` donne `'CD'`, L = 5). δ reçoit la position du premier caractère non inclus.
  3. **`GRL`** sans lettre à partir de β : résultat vide, δ = longueur + 1.
  4. **`SKP`** sans caractère hors de la liste d'arrêt : résultat longueur + 1. Sans γ, `SKP` donne la position de la première lettre, ou longueur + 1.
  5. **`PTR`** : la valeur que `SCH` (avec γ) ou `GRL` (sans γ) mettrait dans δ.
  6. **`POS`** avec Cγ vide : β (LSE15).
  7. **`EQN`** avec β = longueur + 1 : −1 (LSE15).
  8. **`CNB`** : espaces de tête ignorés ; sans chiffre, ni `+` ou `-` suivi d'un chiffre, résultat 0 et arrêt sur ce caractère ; un nombre hors du `float` donne le plus grand nombre représentable affecté du signe, un nombre trop petit donne 0 (LSE15) ; sous les profils `LSE F4` et `-T1600`, le nombre est arrondi à 23 bits, et le plus grand nombre représentable est (2²³ − 1) × 2¹⁰⁵ (Interpréteur IMP-0280). γ reçoit la position du caractère qui a arrêté la conversion.
  9. **`CCA`** : l'écriture U d'IMP-0211, sans l'espace final (§4).
  10. **`EQC`** : un caractère dont le code est l'argument arrondi ; bornes 0 à 127 sous T1600 et 0 à 255 sous MITRA 15 (fiche de codage §4, plages EQC), sinon EX 30.
  11. **`LGR`** : la longueur.
- Alternatives envisagées : signaler une erreur pour `GRL` sans lettre ou `SKP` sans caractère trouvé — écartée : par analogie avec `SCH` sans caractère d'arrêt, LSE15 prolonge l'exploration jusqu'à la fin de la chaîne et rend longueur + 1.
- Justification : LSE15 §4-4.2 ; Gestionnaire des erreurs §9 (RX6).
- Impact mémoire : un objet transitoire par résultat chaîne.
- Impact disque : sans objet.
- Impact performances : linéaire en la longueur des chaînes.
- Impact historique : réalise les exemples de LSE15 §4-4.2 (tests, §8).
- Références normatives : LSE15 §4-4.2 ; INRDP75 §6 ; IMP-0179, IMP-0211.
- Statut : retenu.

---

## 4. Écriture des nombres

**IMP-0211** — *Spécifications U, F et E ; U et `CCA` à sept chiffres significatifs, arrondis*
- Sujet : comment écrire un nombre selon les spécifications de LSE15 §6-2.3, et avec quelle précision écrire U et `CCA` ? Le texte du format U demande les décimales « nécessaires pour représenter le nombre avec la précision avec laquelle il est représenté de manière interne » ; les exemples de `CCA` (`CCA(2/3)` donne `'0.6666667'`) et la sortie de PG4 sur T1600 (`TOTO=2.201199E+09 A/B=-2.434332`, INRDP75) montrent sept chiffres.
- Choix :
  - **U** : au plus **sept** chiffres significatifs, **arrondis** au plus proche, sans zéros superflus ; écriture décimale si la valeur absolue est nulle ou comprise entre 10⁻³ inclus et 10⁶ exclu, sinon un chiffre de mantisse non nul, éventuellement un point et les chiffres suivants, puis `E`, le signe et l'exposant sur deux chiffres (LSE15 §6-2.3 : « toujours signé, et comporte toujours 2 chiffres ») ; puis une espace. Une chaîne sous U est écrite telle quelle, sans espace.
  - **F e.d** : la valeur arrondie à d décimales, la moitié s'éloignant de zéro ; le signe et les chiffres de la partie entière (c caractères) sont précédés de e − c espaces si c < e ; puis, si d > 0, un point et d chiffres. Un nombre arrondi à zéro garde son signe (`-0.0` pour −0,0001 sous `F2.1`, LSE15).
  - **E e.d** : une mantisse d'un chiffre non nul (de 1 à 9), arrondie à d décimales, puis `E`, le signe et l'exposant sur deux chiffres ; le signe et le chiffre de la mantisse sont précédés de e − c espaces si c < e ; le point n'est écrit que si d > 0. Zéro s'écrit avec une mantisse nulle et l'exposant `+00`.
  - **`CCA`** : l'écriture U sans l'espace final (texte de LSE15 §4-4.2 ; choix d'Éric du 2026-10-24, point F3).
  - La décompilation garde six chiffres (Décompilateur IMP-0201, RCI-M IMP-0183) : une constante restituée doit se recompiler.
- Constats à l'appui (choix d'Éric du 2026-10-24, point G4) :
  - sept chiffres : T1600 (INRDP75, PG4) et MITRA 15 (LSE15, `CCA(2/3)`) ; le `float` de l'Apple IIGS a environ 7,2 chiffres de précision ;
  - LSE 8086 version IV (essais d'Éric, 2026-10-24), qui calcule sur 64 bits, écrit 16 chiffres, la précision de son format interne : `?5/9` donne `0.5555555555555556` (arrondi), `?0.1+0.2` donne `0.3`, et les seuils de la forme décimale sont ceux de LSE15 (`999999.9`, `0.001` ; `1E6`, `1E-4`) ;
  - l'arrondi est retenu parce que, sur un `float` IEEE, la troncature écrirait faux des nombres simples (0,7 est rangé 0,699999988…, tronqué `0.6999999`).
- Divergences consignées :
  - le T1600 tronquait : PG4 montre `2.201199E+09` et `-2.434332` (2 201 199 506 et −2,4343329… exactement) ; avec le `float` et l'arrondi, les mêmes calculs s'écrivent `2.2012E+09` et `-2.434333` ;
  - LSE 8086 écrit l'exposant sans signe ni zéro (`1E6`), comme l'exemple `CCA(10↑7+500000)` = `'1.05E7'` de LSE15 ; la règle écrite de LSE15 (`E+06`) est suivie.
- Alternatives envisagées : six chiffres, comme la décompilation — écartée : contraire aux sorties attestées et au texte du format U ; la troncature du T1600 — écartée : voir ci-dessus.
- Justification : LSE15 §6-2.3, §4-4.2 ; INRDP75 PG4 ; essais sur LSE 8086 ; IMP-0201.
- Impact mémoire : nul.
- Impact disque : sans objet.
- Impact performances : une vingtaine d'opérations flottantes par nombre écrit.
- Impact historique : reproduit la table de LSE15 §6-2.3 (tests, §8) et le nombre de chiffres attesté ; divergences consignées ci-dessus.
- Références normatives : LSE15 §6-2.3, §4-4.2 ; INRDP75 PG4 ; IMP-0183, IMP-0201.
- Statut : retenu (Éric, 2026-10-24, points F3 et G4).

Limite : la partie entière d'un nombre de plus de quinze chiffres, écrit sous F, est calculée en `double` ; ses chiffres au-delà du quinzième peuvent différer de la valeur exacte du `float`.

---

## 5. Représentation des tableaux

**IMP-0210** — *Tableau : en-tête, table d'un bit par élément défini, éléments `float` rangés ligne par ligne*
- Sujet : comment ranger un tableau numérique, sachant qu'un élément peut être non défini (LSE15 §6-3.4) ?
- Choix :
  - un objet tableau contient un octet de genre (`1` : tableau), le nombre de dimensions (1 ou 2), les bornes d1 et d2 sur 2 octets chacune (d2 = 1 pour une dimension), une table d'un bit par élément (1 : l'élément a reçu une valeur), puis les éléments, chacun un `float` de 4 octets ;
  - les éléments sont rangés ligne par ligne : le rang de `T[i,j]` est (i − 1) × d2 + (j − 1) (LSE15 §6-1, §6-2.5) ;
  - à la déclaration (`TABLEAU`), les bornes sont arrondies à l'entier le plus voisin (IMP-0179) ; une borne nulle ou négative donne EX 27, un tableau de plus de 65 535 éléments EX 45 ; tous les éléments sont non définis ;
  - un accès indexé arrondit les indices ; un nombre d'indices différent de la déclaration donne EX 46, un indice nul ou négatif EX 27, un indice au-delà de la borne EX 49 ; la lecture d'un élément sans valeur donne EX 32.
- Alternatives envisagées : coder « non défini » par une valeur flottante particulière — écartée, pour la raison donnée par IMP-0179 (une règle du langage ne dépend pas d'une propriété du format flottant) ; un octet d'état par élément — écartée : 1 octet au lieu d'un bit par élément.
- Justification : LSE15 §3-1.2, §6-1, §6-2.5, §6-3.4 ; choix d'Éric du 2026-10-24 (point F2).
- Impact mémoire : 4,125 octets par élément, plus 6 octets d'en-tête ; au plus 65 535 éléments, soit environ 270 Kio (objet hors-norme, IMP-0161).
- Impact disque : sans objet.
- Impact performances : accès en O(1).
- Impact historique : réalise l'élément « non défini » attesté par LSE15.
- Références normatives : LSE15 §3-1.2, §6-1, §6-2.5, §6-3.4 ; IMP-0161, IMP-0179 ; Gestionnaire des erreurs §9.
- Statut : retenu (Éric, 2026-10-24, point F2).

---

## 6. Interface et segment

**IMP-0213** — *Interface du module `donnees` ; segment `"donnees"`*
- Sujet : quelles fonctions le module offre-t-il à l'interpréteur et au contexte d'exécution ?
- Choix : `src/noyau/donnees.h` déclare, chaque fonction rendant une `NatureErreur` (IMP-0208) :
  - les trois opérations d'IMP-0208 : `donnees_chaine_vers_objet`, `donnees_objet_vers_chaine`, `donnees_copier` ;
  - `donnees_chaine_creer` (un texte devient une chaîne transitoire), `donnees_concatener`, `donnees_comparer`, `donnees_texte` (copie du texte d'une chaîne de la pile) ;
  - une fonction par fonction de chaîne de LSE15 §4-4.2 (`donnees_sch`, `donnees_grl`, `donnees_ptr`, `donnees_skp`, `donnees_pos`, `donnees_cnb`, `donnees_eqn`, `donnees_eqc`, `donnees_cca`), `LGR` se lisant dans le texte de la pile ;
  - `donnees_ecrire_nombre` (U, F, E) ;
  - `donnees_creer_tableau`, `donnees_rang`, `donnees_lire_element`, `donnees_ecrire_element`, `donnees_tableau_bornes`, et `donnees_remplir` pour `UN` et `ZER` (Interpréteur IMP-0218).

  Le module occupe le segment statique `"donnees"` (Architecture physique §5.2). Il remplace le bouchon de l'étape 4c dans les tests.
- Alternatives envisagées : placer les fonctions de chaîne dans l'interpréteur — écartée : elles dépendent de la représentation des chaînes, que seul ce module connaît.
- Justification : IMP-0208 ; Organisation du dépôt §2.1.
- Impact mémoire : nul.
- Impact disque : sans objet.
- Impact performances : un appel long entre segments par opération.
- Impact historique : neutre.
- Références normatives : IMP-0135, IMP-0197, IMP-0208.
- Statut : retenu.

---

## 7. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0209 | Variable-chaîne : genre, longueur sur 2 octets, texte ; 65 535 caractères au plus sous les deux profils | retenu |
| IMP-0210 | Tableau : en-tête, bit par élément défini, `float` rangés ligne par ligne | retenu |
| IMP-0211 | Écriture U (sept chiffres arrondis), F, E ; `CCA` = U sans espace final | retenu |
| IMP-0212 | Fonctions sur les chaînes : LSE15 §4-4.2 et compléments déduits | retenu |
| IMP-0213 | Interface du module `donnees` ; segment `"donnees"` | retenu |

---

## 8. Tests

Suite `tests/composants/composants_donnees.c`, sous Golden Gate :
- chaînes : création, borne de 65 535 caractères sous les deux profils, concaténation, comparaison (exemples de LSE15 l. 1056 et suivantes), copie ;
- les exemples de LSE15 §4-4.2 pour chacune des onze fonctions, et les compléments d'IMP-0212 ;
- la table de LSE15 §6-2.3 (`E3.4`, `F2.1`, `U` pour 1692, −34,227, −2,86, 0,055, −0,0001, 1,04006 × 10⁷) et les exemples de LSE15 §6-2.4 (`F0.0`, `E4.6`, `F3.0`) ;
- tableaux : création, rang ligne par ligne, élément non défini, erreurs EX 27, 32, 45, 46, 49.

---

## 9. Traçabilité

| EXI | VM | IV | IMP | Code | Test |
|---|---|---|---|---|---|
| EXI-0039 | VM-1102, VM-1201, VM-1202 | Tome IV §4.8 | IMP-0209 à IMP-0213 | `src/noyau/donnees.c`, `src/noyau/donnees.h` | `tests/composants/composants_donnees.c` |
