# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — Le décompilateur : restitution textuelle d'une ligne compilée

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V rédigé le 2026-10-23 (étape 3 de la feuille de route, Conditionnement §17). Décisions IMP-0198 à IMP-0203, proposées en prose et retenues par Éric le 2026-10-23 (points D1 à D7). Amendements d'IMP-0183 retenus le même jour : blocs `DEBUT…FIN` (point D4), caractères non imprimables sous MITRA 15, libellés de format, ligne de commentaire collée au numéro (§11). Code et tests réalisés le 2026-10-23 (§9.1).
**Portée :** réalisation physique du décompilateur (module `src/noyau/decompilateur.c/.h`, Organisation du dépôt §2.1) : interface, algorithme de restitution d'une ligne, écriture des nombres réels, diagnostics, segment, tests. La forme du texte restitué est fixée par RCI-M IMP-0183 ; le présent chapitre la réalise sans la redéfinir.
**Rang normatif :** subordonné au Tome III (VM-2208, VM-1801, §2.3.8, §2.5.4, INV-06), au Tome IV (IV-IF-0004, §5.5, ALG-1001, ALG-1002, ANN-K-0014, ANN-K-0031), aux EXI-0003, EXI-0010, EXI-0015, EXI-0017, EXI-0022, et au chapitre RCI-M (§4.12, IMP-0183). Aucune disposition du présent chapitre ne peut contredire ces sources.

---

## 0. Avertissement de statut

Le Tome IV dit que « la méthode exacte de décompilation historique n'est pas documentée » (§10.3) : ALG-1001 ne décrit que la transformation minimale. Le corpus atteste seulement le principe : « Le décompilateur transforme, à l'inverse, la chaîne codée en une chaîne de caractères. Il permet donc en particulier de lister le programme » (INRDP75, l. 463) ; le texte source « était recréé, en cas de listage, à partir de la chaîne post-fixée » (NOYELLE89, l. 78, statut à vérifier sur l'original). Le relevé des sorties T1600 de Victor Duruy montre en outre que la décompilation pouvait échouer et que la machine le signalait : `**** LIGNE PRECEDENTE IMPØSSIBLE A DECØMPILER : PLUS DE…` (6EME - SPHIN, `numerisation0022.pdf`). Tout le reste relève de décisions d'implémentation.

---

## 1. Rappel des contrats obligatoires (non renégociables)

| Source | Contenu | Conséquence |
|---|---|---|
| EXI-0003 | Toute représentation compilée conservée doit pouvoir être transformée en une représentation textuelle suffisante pour le listage | Toute séquence produite par le compilateur se décompile (§4) |
| EXI-0010 | Une seule représentation, commune à l'exécution et à la décompilation | Le décompilateur lit la séquence RCI-M telle que l'interpréteur l'exécute ; aucune structure propre |
| EXI-0015 | Restitution textuelle déterministe ; forme normalisée admise | Forme canonique d'IMP-0183 ; même séquence et même profil, même texte |
| EXI-0017 | Distinction documentaire des représentations compilées | Le décompilateur ne lit que la RCI-M ; la RCI-S relève du bibliothécaire |
| EXI-0022 | Le texte source n'est pas conservé | Le décompilateur n'a aucun accès au texte saisi |
| Tome III VM-2208 | Parcourir les objets compilés, décoder les opérations, restituer valeurs et références, reconstruire les constructions, produire une sortie déterministe, **signaler les représentations impossibles à décoder** ; interfaces `decompiler(objet_compilé)` et `decompiler_programme(programme_courant)` ; sorties : texte d'une ligne, programme textuel, diagnostic de décompilation | §3, §6 ; `decompiler_programme` : §3.3 |
| Tome III §2.3.8, INV-06 | Le décompilateur n'intervient pas dans l'exécution, ne modifie ni le programme courant ni la représentation | Lecture seule (§3.2) |
| Tome III §2.5.4 | Sortie déterministe pour une même représentation et un même contexte de décompilation | Le contexte est le profil d'implantation (§3.2) |
| Tome IV IV-IF-0004, ALG-1001, ALG-1002 | Entrée : objet compilé ou programme courant ; sortie : représentation textuelle | §3, §4 |
| Tome IV ANN-K-0014 | La décompilation ne dépend pas du texte source ; une représentation canonique peut être produite | IMP-0183 |
| RCI-M IMP-0183 | Restitution fidèle de toute construction conservée, forme canonique de la présentation, jamais de troncature | Réalisée par §4 à §7 |

---

## 2. Contraintes de plateforme retenues

- Le noyau n'emploie ni `stdio.h` ni la boîte à outils SANE directement (Organisation du dépôt §2 ; `tests/commun/verif.h`) : l'écriture des nombres est faite par le module (§7).
- La pile C d'ORCA/C partage l'espace du runtime (Compilateur §2) : l'algorithme n'est pas récursif et sa pile de travail est bornée (§4.2).
- Sous `#pragma debug` bit 5, aucun pointeur n'est incrémenté en instruction (`p++`) : les parcours se font par indice (Construction §5).
- Un paramètre `float` est conservé au format SANE extended : la valeur est recopiée dans une variable locale avant tout usage de son adresse (RCI-M §2).
- Le segment de code est limité à 64 Kio et son nom à 10 caractères (IMP-0197) : §8.

---

## 3. Interface

### 3.1 Décision

**IMP-0198** — *Interface du décompilateur : une ligne compilée vers un texte borné*
- Sujet : quelle fonction C réalise `decompiler(objet_compilé)` (Tome III VM-2208, IV-IF-0004), avec quelle entrée, quelle sortie et quel diagnostic ?
- Choix :
  ```c
  #define DECOMPILATEUR_LONGUEUR_MAX 255u   /* caractères, hors zéro final */

  typedef unsigned char ResultatDecompilation;
  #define DECOMP_OK              0
  #define DECOMP_ERR_TROP_LONGUE 1   /* texte canonique de plus de 255 caractères */
  #define DECOMP_ERR_SEQUENCE    2   /* séquence impossible à décoder */
  #define DECOMP_ERR_REFERENCE   3   /* identifiant de symbole ou index de constante inconnu */

  ResultatDecompilation decompilateur_decompiler(const EntreeObjetCompile *ligne,
                                                 char *texte, unsigned int *longueur);
  ```
  - L'entrée est une entrée du programme courant (`EntreeObjetCompile`, RCI-M §5) : numéro de ligne, longueur, `HandleObjet` du bloc.
  - `texte` désigne un tableau d'au moins `DECOMPILATEUR_LONGUEUR_MAX + 1` caractères. En cas de succès, il reçoit la ligne restituée : le numéro, un espace, puis les instructions, sans caractère de fin de ligne et terminée par un zéro ; `*longueur` reçoit sa longueur.
  - En cas d'échec, `texte` est la chaîne vide et `*longueur` vaut 0 ; le résultat dit pourquoi (§6).
  - Le contexte de décompilation (Tome III §2.5.4) est le profil d'implantation de la session (`configuration_session.profil`, IMP-0196) : il fixe la graphie de `TEM` et de `DAT` (IMP-0180, IMP-0189) et l'écriture des caractères non imprimables (IMP-0190).
  - Les noms sont lus par `symboles_lire`, les chaînes partagées par `constantes_lire` (Compilateur §7, IMP-0181).
- Alternatives envisagées :
  1. Écrire le texte au fil de l'eau par une fonction de rappel fournie par l'appelant — écartée : le texte se construit par insertions (§4.3), il n'est connu qu'à la fin.
  2. Allouer le texte dans un objet du gestionnaire d'objets — écartée : LISTER et DECODER consomment la ligne aussitôt ; un tableau de l'appelant suffit et n'exige aucune libération.
  3. Une longueur maximale de 79 caractères, celle d'une ligne saisie — écartée : la forme canonique peut dépasser 79 caractères (IMP-0183, « Longueur des lignes restituées »), et elle ne doit jamais être tronquée.
- Justification : 255 caractères suffisent à toute ligne produite par le compilateur. La forme canonique n'ajoute au texte saisi que quelques caractères par constante réelle (`.5` devient `0.5`, `1E5` devient `100000`), et `DEBUT FIN` pour une branche vide ; une ligne de 79 caractères compte au plus une vingtaine de constantes. Une séquence plus longue ne peut venir que d'un fichier-programme produit ailleurs : elle est signalée (§6), jamais tronquée.
- Impact mémoire : 256 octets chez l'appelant, sur sa pile ou dans sa zone de travail.
- Impact disque : sans objet.
- Impact performances : une passe linéaire sur la séquence, plus des décalages de texte bornés par 255 octets (§4.3).
- Impact historique : réalise le rôle attesté du décompilateur (INRDP75, l. 463) ; le signalement d'une ligne impossible à décompiler est attesté sur T1600.
- Références normatives : Tome III VM-2208, §2.5.4, INV-06 ; Tome IV IV-IF-0004, ALG-1001 ; EXI-0003, EXI-0015 ; RCI-M §5, IMP-0183 ; IMP-0180, IMP-0189, IMP-0196.
- Statut : retenu (Éric, 2026-10-23, point D1).

### 3.2 Lecture seule

Le décompilateur résout le bloc de la ligne par `objets_resoudre`, le lit, puis le relâche par `objets_relacher`. Il ne crée ni ne libère aucun objet et ne modifie aucune table (Tome III INV-06). Les chaînes partagées sont résolues le temps de leur copie dans le texte.

### 3.3 Décompilation du programme (`decompiler_programme`, ALG-1002)

Tome III VM-2208 prévoit une seconde interface, `decompiler_programme(programme_courant)`, qui parcourt les lignes dans l'ordre (ALG-1002). Elle appartient au décompilateur, mais elle dépend de l'interface du programme courant (IV-IF-0002), dont le chapitre n'est pas encore rédigé. Elle sera ajoutée à ce module en même temps que le programme courant : elle parcourra les entrées dans l'ordre des numéros, appellera `decompilateur_decompiler` pour chacune et remettra chaque ligne à un consommateur fourni par l'appelant (LISTER, DECODER). L'étape 3 n'en a pas besoin : les tests appellent directement la décompilation d'une ligne.

---

## 4. Algorithme de restitution d'une ligne (réalisation d'ALG-1001)

### 4.1 Décision

**IMP-0199** — *Une passe en avant, un seul tampon de texte, une pile d'entrées*
- Sujet : comment reconstruire, sans arbre ni récursion, le texte infixé d'une séquence postfixée dont certaines constructions (instruction SI, format d'AFFICHER) ne se reconnaissent qu'après coup ?
- Choix : la séquence est parcourue une seule fois, élément par élément, dans l'ordre physique. Le texte se construit dans le tableau de sortie. Une pile d'entrées décrit les morceaux de texte en attente ; les textes de toutes les entrées se suivent, dans l'ordre de la pile, à la fin du tableau. Chaque élément combine les entrées du sommet (§4.3). En notation postfixée, les opérandes restent dans l'ordre où ils s'écrivent : combiner deux opérandes revient donc à insérer l'opérateur entre deux morceaux voisins, et une parenthèse se place par un décalage du texte.
- Alternatives envisagées :
  1. Construire un arbre de la ligne, puis l'écrire — écartée : un nœud par élément, de la mémoire en plus pour rien.
  2. Une descente récursive depuis la fin de la séquence — écartée : les éléments sont de longueur variable et ne se lisent qu'en avant ; la récursion consomme la pile C partagée (§2).
- Justification : la ligne compte au plus 255 octets et le texte au plus 255 caractères ; les décalages restent bornés et la pile de travail tient en quelques centaines d'octets.
- Impact mémoire : pile de travail de 128 entrées de 12 octets (1 536 octets de pile C) et tampon de 256 octets pour un élément, sans allocation ; tableau de sortie de l'appelant.
- Impact disque : sans objet.
- Impact performances : quadratique au pire sur 255 caractères, c'est-à-dire négligeable.
- Impact historique : neutre ; la méthode historique n'est pas documentée (Tome IV §10.3).
- Références normatives : Tome IV ALG-1001 ; EXI-0011 (forme postfixée) ; Compilateur §2 ; fiche de codage §7.
- Statut : retenu (Éric, 2026-10-23, point D2).

### 4.2 Entrées de la pile

| Genre | Texte | Champs |
|---|---|---|
| **Valeur** | une expression, une référence, une condition | rang (§4.4) |
| **Suite d'instructions** | une ou plusieurs instructions déjà séparées par `;` | nombre d'instructions ; genre de la dernière ; indicateur « se termine par un SI ouvert » (§5.3) ; pour une dernière instruction encore ouverte (AFFICHER, LIRE, FAIRE), son état |
| **Marque SI** | `SI condition ALORS ` puis, après la branche ALORS, ` SINON ` | cible en attente ; phase (ALORS, SINON) ; forme (instruction ou expression) |

Une suite d'instructions qui arrive au-dessus d'une autre suite du même niveau s'y fond aussitôt, avec un `;` entre les deux. La pile ne contient donc jamais deux suites voisines : sa profondeur est bornée par le nombre de valeurs en attente et de SI imbriqués. Chaque entrée en attente correspond à au moins deux octets distincts de la séquence (une valeur et l'élément qui la consommera ; l'élément `C0` d'une marque) : 128 entrées suffisent à toute séquence valide de 255 octets. Un dépassement signale une séquence impossible à décoder (§6).

### 4.3 Restitution de chaque élément

Les notations suivent la fiche de codage (§2 à §5) ; `v`, `v1`… désignent les valeurs du sommet de la pile, dans l'ordre d'écriture. « Instruction » veut dire : le texte devient une suite d'une instruction, qui se fond dans la suite du dessous s'il y en a une (`;`).

| Élément | Restitution |
|---|---|
| `0x00`, `0x01`, `0x07`–`0x3F` | entier en décimal ; négatif : `-n`, rang du changement de signe |
| `0x02` | réel (§7) ; négatif : rang du changement de signe |
| `0x03` | `'texte'`, apostrophe doublée ; sous `LSE F4`, caractères non imprimables en `.n.`, regroupés ; sous T1600 version 4 et MITRA 15, où `.n.` est hors profil, tout caractère tel quel (IMP-0183) ; texte partagé lu par `constantes_lire` |
| `0x06` | `@n` |
| `0x40` | nom, précédé de `&` si le symbole est de nature procédure |
| `0x41` | `NOM[v1]` ou `NOM[v1,v2]` |
| `0x42`, `0x43` | `@`, `$` |
| `0x80`–`0x85`, `0x9C` | `v1 op v2`, parenthèses selon §4.4 |
| `0x82` | `-v`, parenthèses selon §4.4 |
| `0x86`–`0x8E`, `0x9D`, `0x9E`, `0xA5`, `0xA6`, `0xBC`–`0xBF` | `NOM(v…)` |
| `0x9F`–`0xA4` | `NOM(v1,…,vn)`, `n` lu dans la charge utile |
| `0x8F`, `0xA7` | `TEM()`, `DAT()` sous MITRA 15 et sous `LSE F4` (IMP-0189) ; `TEM`, `DAT` sous T1600 version 4 |
| `0x90`–`0x95` | `v1 rel v2` : condition |
| `0x96`, `0x97` | `v1 ET v2`, `v1 OU v2`, parenthèses selon §4.4 |
| `0x99` | `v1_v2` : instruction |
| `0x9A`, `0x9B` | `UN v`, `ZER v` : instruction |
| `0xA8` | `&P(v1,…,vk)` ; instruction si la position vaut `0x00`, valeur sinon |
| `0xA9` | `TABLEAU NOM[v1{,v2}]` : instruction ; fusion (§4.5) |
| `0xAA`, `0xAB` | `CHAINE NOM`, `LIBERER NOM` : instruction ; fusion (§4.5) |
| `0xAC` | `GARER v1,v2,v3` : instruction |
| `0xAD` | `CHARGER v1,v2,v3{,v4}` : instruction |
| `0xAE`, `0xAF` | `SUPPRIMER v1{,v2}`, `EXECUTER v1{,v2}` : instruction |
| `0xB8` | ouvre l'instruction `AFFICHER` |
| `0xB7` | ouvre l'instruction `AFFICHER[]` ; les spécifications s'insèrent avant `]` |
| `0xB0`–`0xB5` | une spécification du format, séparée par `,` des précédentes : `{r}'texte'`, `{r}/`, `{r}X`, `{r}C`, `{r}L`, `{r}Fe.d`, `{r}Ee.d`, `{r}U` ; `r` n'est écrit que s'il dépasse 1 ; un facteur variable s'écrit `*` et la valeur du sommet devient l'élément suivant de la liste |
| `0xB6` | la valeur du sommet devient l'élément suivant de la liste : précédée d'un espace (sans format) ou de rien (après `]`) pour le premier, de `,` ensuite |
| `0xD1` | ouvre l'instruction `LIRE` |
| `0xBB` | la référence du sommet devient la cible suivante : ` A`, puis `,B` |
| `0xC9` | `FAIRE v1 POUR V_v2` : ouvre l'instruction |
| `0xD2` | `FAIRE v1` : ouvre l'instruction, sans variable (IMP-0277) |
| `0xCA` | ` PAS v` |
| `0xCE` | rien |
| `0xCF` | ` JUSQUA v` : ferme l'instruction |
| `0xD0` | ` TANT QUE v` : ferme l'instruction |
| `0xC0`, `0xC1` | structure SI (§5) |
| `0xC2`, `0xC5` | `ALLER EN n`, `RETOUR EN n` : instruction |
| `0xC3`, `0xC6` | `ALLER EN v`, `RETOUR EN v` : instruction |
| `0xC4`, `0xC7`, `0xC8` | `RETOUR`, `TERMINER`, `PAUSE` : instruction |
| `0xCB` | `RESULTAT v` : instruction |
| `0xCC` | `*` suivi du texte, à l'octet près, espaces compris : instruction ; en tête de ligne, le `*` est collé au numéro (`1*TEXTE`, IMP-0183) |
| `0xCD` | `PROCEDURE &P(a1,…,an)`, suivi de ` LOCAL l1,…,lm` si `m > 0` ; un paramètre de nature procédure s'écrit avec `&` : instruction ; admis seulement en premier élément |
| `0x04`, `0x05`, `0x98`, `0xB9`, `0xBA`, réservés | jamais produits par le compilateur : séquence impossible à décoder |

Les espaces suivent IMP-0183 : le numéro est suivi d'un espace, sauf devant le `*` d'une ligne de commentaire ; un espace de part et d'autre de chaque mot du langage, sauf en tête d'instruction et devant `;` ou la fin de ligne ; `AFFICHER[` et `NOM(` sans espace ; aucun autre espace, ni autour des opérateurs, ni après `;`, ni après le `]` d'un format. Le corpus confirme ce dernier point : dans INRDP75 et LSE15, les 35 instructions AFFICHER à format suivies d'une liste n'ont aucune espace entre `]` et la liste (`AFFICHER[/,'X=',F3.0]X`).

### 4.4 Rangs et parenthèses

Rangs des valeurs, du plus faible au plus fort : SI en position d'expression 1 ; `!` 2 ; `+`, `-` et changement de signe 3 ; `*`, `/` 4 ; `↑` 5 ; atome (constante, référence, appel de fonction ou de procédure, texte entre parenthèses) 6. Conditions : `OU` 1 ; `ET` 2 ; relation 3.

Pour un opérateur binaire de rang `R` : l'opérande gauche est mis entre parenthèses si son rang est inférieur à `R` ; l'opérande droit, si son rang est inférieur ou égal à `R` (associativité à gauche, IMP-0183). Le changement de signe porte sur un terme : son opérande est mis entre parenthèses si son rang est inférieur à 4 (`-(A+B)`, `-(-A)`). Une constante négative a le rang du changement de signe : `A*(-1)`, `X_-1`, `PAS -1`.

Un SI en position d'expression n'est jamais un opérande d'opérateur arithmétique ou de `!` sans parenthèses : le compilateur n'admet SI qu'au niveau d'une expression entière (`LSE15Mitra.g4`, règle `expression` ; Compilateur §5). Il n'en a jamais besoin comme opérande d'une relation, argument, indice, élément de liste ou membre droit d'une affectation, qui sont tous des expressions entières.

### 4.5 Fusion des déclarations successives

Lorsqu'une instruction `TABLEAU` (`0xA9`) se fond dans une suite dont la dernière instruction est aussi une instruction `TABLEAU` du même niveau, elle s'y ajoute après une virgule, sans répéter le mot (`TABLEAU A[2],B[3]`). Il en va de même pour `CHAINE` et `LIBERER`. Le compilateur produit la même séquence pour les deux écritures (IMP-0183, « Déclarations successives »). La fusion ne franchit jamais une marque SI : elle ne réunit que des instructions de la même branche.

---

## 5. Instructions et expressions conditionnelles

### 5.1 Reconnaissance

L'élément `C0` change la valeur du sommet, la condition, en une marque SI de texte `SI condition ALORS `, qui attend sa cible. Avant de traiter l'élément situé à la position `p`, et à la fin de la séquence, le décompilateur ferme chaque marque dont la cible vaut `p`, la plus intérieure d'abord.

L'élément `C1` à la position `q` appartient à la marque du sommet si sa cible vaut `q + 2` ; sinon la séquence est impossible à décoder. La branche ALORS se termine alors.
- S'il ne reste au-dessus de la marque qu'une seule valeur, le SI est en position d'expression.
- S'il ne reste que des instructions, ou rien, c'est une instruction SI.

Le texte devient `SI condition ALORS branche SINON ` et la marque attend la cible de `C1`. Une marque fermée sans `C1` est toujours une instruction SI sans SINON.

À la fermeture :
- **SI en position d'expression :** il doit rester une seule valeur au-dessus de la marque ; le tout devient une valeur de rang 1.
- **Instruction SI :** il ne doit rester que des instructions ; le tout devient une instruction.

Toute autre configuration est une séquence impossible à décoder.

### 5.2 Blocs `DEBUT…FIN` (IMP-0183 amendée)

Une branche d'instruction SI est restituée entre `DEBUT` et `FIN` dans trois cas :
- elle comporte plusieurs instructions (`SI A=1 ALORS DEBUT X_1;Y_2 FIN`) ;
- elle est vide (`SI A=1 ALORS DEBUT FIN SINON X_2`) ;
- c'est la branche ALORS d'un SI qui a une branche SINON, elle se compose d'une seule instruction, et cette instruction se termine par un SI ouvert (§5.3).

Dans tous les autres cas, la branche s'écrit sans `DEBUT…FIN`.

### 5.3 SI ouvert

Une instruction **se termine par un SI ouvert** dans deux cas :
- c'est une instruction SI sans SINON ;
- c'est une instruction SI dont la branche SINON, écrite sans `DEBUT…FIN`, se termine elle-même par un SI ouvert.

Un SINON écrit après une telle instruction se rattacherait au SI le plus proche (LSE15 §5-2). Exemple, vérifié par compilation le 2026-10-23 :
- `SI A=1 ALORS DEBUT SI B=1 ALORS X_1 FIN SINON X_2` et `SI A=1 ALORS SI B=1 ALORS X_1 SINON X_2` donnent deux séquences différentes : dans la première, la cible du `C0` intérieur est le `C1` extérieur.
- Écrire la première sans bloc la ferait recompiler en la seconde.
- La même règle a été constatée par Éric sur LSE 8086 version IV le 2026-10-23.

---

## 6. Diagnostics

**IMP-0200** — *Échec de la décompilation : jamais de troncature, trois causes distinguées*
- Sujet : que fait le décompilateur d'une séquence qu'il ne peut pas restituer (Tome III VM-2208 : « signaler les représentations impossibles à décoder ») ?
- Choix : la fonction échoue, rend la chaîne vide et l'une de trois causes :
  - `DECOMP_ERR_TROP_LONGUE` : le texte canonique dépasse 255 caractères ;
  - `DECOMP_ERR_SEQUENCE` : la séquence ne se découpe pas en éléments (`rcim_sequence_valide`), ou contient un élément réservé ou jamais produit, une cible de branchement incohérente, un élément qui trouve sur la pile autre chose que ce qu'il attend, une déclaration de procédure ailleurs qu'en tête, ou laisse en fin de ligne autre chose qu'une suite d'instructions ;
  - `DECOMP_ERR_REFERENCE` : un identifiant de symbole ou un index de constante sans entrée dans les tables.

  Ces causes ne sont pas des natures du catalogue d'erreurs : la forme du message affiché relève du chapitre du moniteur. Ce chapitre dispose d'une attestation, `**** LIGNE PRECEDENTE IMPØSSIBLE A DECØMPILER : PLUS DE…` (relevé T1600, SPHIN), dont le déclencheur exact n'est pas établi.
- Alternatives envisagées : tronquer la ligne restituée — écartée par IMP-0183 (un programme différent serait produit en silence) ; ne pas distinguer les causes — écartée : une séquence abîmée et une ligne trop longue n'appellent pas la même action.
- Justification : Tome III VM-2208 ; IMP-0183 ; IMP-0187 (une nature par situation à corriger).
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : nul.
- Impact historique : conforme à l'attestation T1600 d'une ligne impossible à décompiler.
- Références normatives : Tome III VM-2208 ; Tome IV §4 (« erreur de décompilation ») ; RCI-M IMP-0183 ; relevé T1600.
- Statut : retenu (Éric, 2026-10-23, point D3).

---

## 7. Écriture des nombres réels

**IMP-0201** — *Écriture d'une constante réelle à six chiffres significatifs, sans bibliothèque d'entrée-sortie*
- Sujet : comment réaliser, dans le noyau, l'écriture fixée par IMP-0183 (« au plus six chiffres significatifs, sans zéros superflus », décimale si `|v|` est nul ou compris entre 10⁻³ inclus et 10⁶ exclu, sinon `mE±ee`) ?
- Choix : une fonction du module, `decompilateur_ecrire_reel`, que l'interpréteur pourra réemployer :
  1. La valeur `float` est copiée dans un `double`, ce qui est exact. Le signe est écrit à part.
  2. L'exposant décimal `e` est cherché par comparaison avec des puissances de dix. La mantisse est `m = arrondi(|v| × 10^(5−e))`, arrondie au plus proche, la moitié vers le haut. On corrige `e` d'une unité tant que `m` sort de [100000, 999999].
  3. Une puissance de dix jusqu'à 10²² est exacte en `double`. Le produit, ou le quotient pour une puissance négative, est donc correctement arrondi en une seule opération. Au-delà, l'échelle se fait en deux opérations.
  4. Les zéros de droite de `m` sont retirés. Si `−3 ≤ e ≤ 5`, l'écriture est décimale : `0.055`, `2.5`, `123456`, `100000`. Sinon, c'est un chiffre, éventuellement un point et les chiffres suivants, puis `E`, le signe et deux chiffres : `1E-04`, `3.01E+21`, `1.4013E-45`.
- Alternatives envisagées : `sprintf` d'ORCA/C — écartée : `stdio.h` est exclu du noyau, et le format `%g` ne suit pas IMP-0183 (exposant, bornes de la forme décimale) ; appel direct de SANE (`Num2Dec`, `Dec2Str`) — écartée : dépendance à une boîte à outils dans le noyau.
- Justification : six chiffres significatifs sont la précision documentée du L.S.E. (INRDP75 ; IMP-0183). Pour tout `float` normal, un nombre décimal d'au plus six chiffres converti en `float` puis réécrit ainsi retrouve son écriture exacte, car six chiffres restent sous la précision du format, qui dépasse sept chiffres. Les nombres dénormalisés, plus petits que 1,18 × 10⁻³⁸ en valeur absolue, n'ont plus six chiffres de précision : leur écriture ne garantit pas le retour à la même valeur.
- Impact mémoire : quelques dizaines d'octets de pile.
- Impact disque : sans objet.
- Impact performances : une vingtaine d'opérations flottantes par constante réelle.
- Impact historique : conforme aux conventions d'écriture attestées (LSE15 §6-2.3 ; INRDP75, 3ᵉ partie, §7-3).
- Références normatives : RCI-M IMP-0183, §2 ; IMP-0103.
- Statut : retenu (Éric, 2026-10-23, point D5).
- Complément du 2026-10-30 (Éric ; audit du code, AP-03) : les puissances de dix de la mise à l'échelle sont lues dans une table de 23 constantes `double` (10⁰ à 10²²), au lieu d'être formées par produits successifs à chaque appel ; ces puissances sont exactes, et la suite `composants_decompilateur` vérifie que ses littéraux égalent bit à bit les produits successifs par 10 : chaque mise à l'échelle fait donc la même opération sur les mêmes valeurs. Mesuré par `iix --trace-tools` pour une écriture à 7 chiffres : 3 ou 3.14159, 24 appels SANE au lieu de 145 ; 12345, 48 au lieu de 373 ; 1E+30, 214 au lieu de 2 301.

Conséquence pour l'aller-retour : une constante écrite avec plus de six chiffres significatifs (`3.1415927`) est restituée avec six (`3.14159`). Recompilée, elle donne un `float` différent, donc une séquence différente au-delà du sixième chiffre (IMP-0183). Seule une telle ligne peut ne pas se recompiler à l'octet près ; sa restitution est en revanche stable dès la deuxième génération (§9).

---

## 8. Segment

**IMP-0202** — *Segment `"decompil"`*
- Sujet : quel segment de chargement reçoit le code du décompilateur ?
- Choix : un segment statique nommé `"decompil"`. Il remplace le nom `"decompilateur"` des tables d'Architecture physique §5.2 et d'Organisation du dépôt §2.1, qui dépasse les 10 caractères des noms de segments (IMP-0197).
- Alternatives envisagées : garder `"decompilateur"` — écartée : contraire à la règle d'IMP-0197 ; segment blanc (principal) — écartée : le segment principal porte déjà les données statiques et le moniteur (IMP-0135).
- Justification : IMP-0135, IMP-0197.
- Impact mémoire : nul (segment statique, résident). Impact disque : sans objet. Impact performances : appels longs, négligeables.
- Impact historique : neutre.
- Références normatives : IMP-0135, IMP-0197 ; manuel ORCA/C, chap. 19.
- Statut : retenu (Éric, 2026-10-23, point D6).

---

## 9. Tests

**IMP-0203** — *Tests du décompilateur : vecteurs, cas ciblés, aller-retour sur le corpus historique*
- Sujet : comment vérifier qu'une séquence produite par le compilateur se restitue en un texte qui se recompile en la même séquence ?
- Choix :
  1. **Suite de composants** `tests/composants/composants_decompilateur.c` :
     - les vecteurs V1 à V16 de la fiche de codage, comparés à leur texte canonique ;
     - des cas ciblés : parenthèses et rangs, constantes négatives, SI en instruction et en expression, blocs `DEBUT…FIN` (§5.2, dont la règle du SI ouvert), AFFICHER avec et sans format, LIRE, fusion des déclarations, procédures, profils, chaînes ;
     - l'écriture des réels sur un échantillon de mantisses à six chiffres et d'exposants du domaine normal du `float` ;
     - les trois diagnostics, sur des séquences fabriquées.
  2. **Suite historique** `tests/historiques/historiques_decompilation.c` :
     - chaque ligne du corpus, `tests/historiques/corpus/inrdp75.txt` et `lse15.txt`, est compilée sous MITRA 15, à défaut sous T1600 ;
     - elle est décompilée, puis recompilée ;
     - la séquence obtenue est comparée octet par octet ; une ligne qui diffère doit contenir une constante de plus de six chiffres significatifs (§7) ;
     - la restitution de la deuxième génération doit être identique à la première ;
     - les nombres de lignes acceptées et rejetées sont vérifiés, pour détecter toute régression.
  3. **Corpus.** Les fichiers du corpus sont produits par `scripts/extraire_corpus.py` à partir des transcriptions d'INRDP75 et de LSE15 (`02-HISTORIQUE/`). Le script retient les lignes numérotées des blocs de code et écrit la flèche `_` et l'exponentiation `^`. Il écarte les lignes qui contiennent un autre caractère non ASCII. La suite les lit par `fopen`, que Golden Gate fournit ; `stdio.h` n'est employé que dans `tests/`.
- Alternatives envisagées : comparer à des textes écrits à la main seulement — écartée : ne couvre pas la variété du corpus ; inclure le corpus dans le source C — écartée : plus de 30 Kio de données statiques dans le segment principal, limité à 64 Kio.
- Justification : la boucle compilation → décompilation → recompilation est la propriété exigée par DECODER puis ENCODER (RCI-M §4.12) ; le corpus historique en est le meilleur échantillon.
- Impact mémoire : sans objet (tests).
- Impact disque : environ 35 Kio de corpus dans `tests/`.
- Impact performances : sans objet.
- Impact historique : les exemples des manuels deviennent des tests (Conditionnement, niveau « historiques »).
- Références normatives : Conditionnement §17 ; IMP-0192 ; RCI-M IMP-0183 ; fiche de codage §11.
- Statut : retenu (Éric, 2026-10-23, point D7).

### 9.1 Résultats du 2026-10-23

Les 14 suites passent sous Golden Gate, dans les profils `dev` et `cible` (622 contrôles, dont 134 pour `composants_decompilateur` et 10 pour `historiques_decompilation`).

**Corpus.**
- INRDP75 : 843 lignes, dont 810 acceptées par le compilateur (2 seulement sous T1600).
- LSE15 : 69 lignes, dont 57 acceptées.
- Les 45 lignes rejetées sont des tableaux de nombres imprimés et des fragments de lignes coupées, non des lignes de programme.

**Aller-retour.**
- Les 867 lignes acceptées se recompilent toutes, à partir de leur restitution, en la même séquence à l'octet près.
- Leur restitution est identique à la deuxième génération.
- Aucune ne contient de constante de plus de six chiffres significatifs, et aucune restitution ne dépasse 79 caractères.

**Écarts avec le texte saisi.** 7 restitutions diffèrent du texte du manuel :
- 6 normalisations de présentation : double espace après le numéro, `;` final, `;` devant `FIN` ;
- la ligne `1 *PROGRAMME SIMPLE` de LSE15, restituée `1*PROGRAMME SIMPLE` (§11, point 3).

Les 81 autres lignes de commentaire du corpus sont restituées exactement comme le manuel les écrit.

---

## 10. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0198 | Interface `decompilateur_decompiler`, texte de 255 caractères au plus, contexte = profil | retenu |
| IMP-0199 | Une passe, un tampon, une pile d'entrées ; pas d'arbre ni de récursion | retenu |
| IMP-0200 | Trois causes d'échec, jamais de troncature | retenu |
| IMP-0201 | Écriture des réels à six chiffres significatifs dans le noyau | retenu |
| IMP-0202 | Segment `"decompil"` | retenu |
| IMP-0203 | Tests : vecteurs, cas ciblés, aller-retour sur le corpus | retenu |

Amendements appliqués ailleurs : RCI-M IMP-0183, « Blocs `DEBUT…FIN` » (§5.2 du présent chapitre), texte des chaînes et espace après le numéro (§11) ; reportés dans la fiche de codage §9.

---

## 11. Points tranchés après la première rédaction

Trois points, laissés ouverts à la première rédaction, ont été tranchés par Éric et reportés dans IMP-0183 :

1. **Caractère non imprimable dans une chaîne.**
   - Sous `LSE F4`, un caractère non imprimable (code 0 à 31, ou 127) d'une constante-chaîne est restitué en `.n.`, seule écriture qui se recompile.
   - Sous T1600 version 4 et sous MITRA 15, `.n.` est hors profil (IMP-0190, C 103). Le caractère, saisi tel quel, est donc restitué tel quel entre les apostrophes.
2. **Libellé de format.** Un libellé d'AFFICHER à format (`0xB0`) a la syntaxe d'une constante-chaîne (Compilateur §5, « Génération d'AFFICHER ») : la même routine, `ecrire_chaine`, restitue l'un et l'autre, selon les règles du point 1. Le listing de `MULT` atteste cette restitution sous `LSE F4` (`' A'.7.'U'.7.`…, ligne 50).
3. **Ligne de commentaire.**
   - Un commentaire en tête de ligne est collé au numéro : `1*SOMME DES CARRES`.
   - C'est la forme des listings T1600 de Victor Duruy (constat d'Éric), et celle de 81 des 82 lignes de commentaire d'INRDP75 et de LSE15.
   - Tout ce qui suit le `*` est restitué tel qu'entré, espaces compris (`1* CALCUL DE L'AIRE`).
   - Un commentaire qui suit d'autres instructions s'écrit après le `;`, sans espace (`5 X_1;*FIN`).

---

## 12. Traçabilité

| EXI | VM | IV | IMP | Code | Test |
|---|---|---|---|---|---|
| EXI-0003, EXI-0010, EXI-0015, EXI-0017, EXI-0022 | VM-2208, VM-1801, VM-0406 | IV-IF-0004, ALG-1001, ANN-K-0014, ANN-K-0031 | IMP-0183, IMP-0198 à IMP-0203 | `src/noyau/decompilateur.c`, `src/noyau/decompilateur.h` | `tests/composants/composants_decompilateur.c`, `tests/historiques/historiques_decompilation.c` |

ALG-1002 (`decompiler_programme`) : §3.3, à réaliser avec le programme courant.

---

## 13. Limites explicites de ce chapitre

- Ne couvre ni LISTER ni DECODER (moniteur, bibliothécaire) : ni la mise en page de plusieurs lignes, ni le message affiché en cas d'échec, ni le sort d'une ligne restituée de plus de 79 caractères, écrite telle quelle par DECODER et signalée par ENCODER avec C 63 (IMP-0183).
- Ne couvre pas la décompilation d'une ligne de machine de bureau : son bloc transitoire n'est jamais conservé ni listé (RCI-M IMP-0109).
- Ne couvre pas `decompiler_programme` (§3.3).
