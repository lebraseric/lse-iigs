# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — Gestionnaire des erreurs : catalogue, numérotation, forme des messages

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V **en cours de rédaction**. Sont fixés : le relevé des attestations historiques (§2), la numérotation des codes d'erreur (§3, IMP-0185), la forme des messages (§4, IMP-0186) et la granularité (§5, IMP-0187). Le catalogue des natures de compilation (§7) et celui des natures d'exécution (§9, 2026-10-23) sont retenus. La structure des diagnostics est fixée (§8, IMP-0195). Reste à rédiger : le catalogue des natures de fichier (§10).
**Portée :** réalisation physique du gestionnaire des erreurs (module `src/noyau/erreurs.c/.h`, Organisation du dépôt §2.1) : catalogue des natures d'erreur, numéros affichés, forme des messages, structure du diagnostic transmis par les composants. Le chapitre ne redéfinit ni le moment où chaque composant détecte une erreur, qui relève du compilateur, de l'interpréteur ou du bibliothécaire, ni la classification en domaines du Tome IV (§17.1).
**Rang normatif :** subordonné au Tome IV (§3.3, Convention des erreurs ; §17, Algorithmes de gestion des erreurs, ALG-1701), à EXI-0004, EXI-0019 et EXI-0051, et au chapitre Compilateur (IMP-0168, structure provisoire du diagnostic de compilation). Aucune disposition du présent chapitre ne peut contredire ces sources.
**Place dans le Tome V :** ce chapitre referme le point laissé ouvert par le chapitre Compilateur (§18 : « forme définitive du diagnostic de compilation [et] énumération exhaustive des codes d'erreur »). Sa forme est préparée par l'inventaire `05-IMPLEMENTATION/audits/Inventaire_diagnostics_compilation_2026-10-22.md`.

---

## 0. Avertissement de statut

Le Tome IV (§3.3) dit que « la forme physique du diagnostic n'est pas imposée » et exige seulement qu'une erreur soit caractérisée par une catégorie, une opération, un contexte et une information de diagnostic. La numérotation et la forme des messages relèvent donc de décisions d'implémentation (`IMP-*`). Le corpus historique les documente cependant en partie (§2) : chaque décision du présent chapitre s'y rattache explicitement, et s'en écarte seulement là où il est muet ou divergent, en le disant.

---

## 1. Rappel des contrats obligatoires (non renégociables)

| Source | Contenu | Conséquence |
|---|---|---|
| Tome IV §3.3 | Une erreur est caractérisée au minimum par sa catégorie, l'opération concernée, son contexte et l'information permettant le diagnostic | Le catalogue (§3) associe à chaque nature d'erreur sa classe et son numéro ; le contexte (ligne, position) est porté par le diagnostic |
| Tome IV §17.1 | Quatre domaines principaux : `COMPILATION`, `EXECUTION`, `SERVICE`, `DIALOGUE` ; une erreur conserve son domaine pendant sa propagation | Chaque nature d'erreur appartient à un seul domaine |
| Tome IV §17.2, §17.3 | Une erreur syntaxique est détectée avant l'acceptation de la ligne ; une erreur d'exécution ne doit pas être présentée comme une erreur de compilation | Classes d'affichage distinctes (§2.1) |
| EXI-0019 | Distinction des erreurs de compilation et d'exécution | idem |
| EXI-0051 | Diagnostic immédiat des erreurs syntaxiques | Le diagnostic de compilation est produit au rejet de la ligne |
| Granularité (décision d'Éric, 2026-10-22) | Diagnostics fins ; un code générique ne doit pas couvrir des situations qui appellent des corrections différentes (NOYELLE89 : « un outil doit faciliter au maximum la vie de celui qui l'emploie ») | Le catalogue est organisé par nature d'erreur, non par point de détection grossier |

---

## 2. Attestations historiques

### 2.1 Classification

INRDP75 (l. 668-675) : « De façon générale le système détecte : soit des erreurs au niveau de la compilation : l'écriture de la ligne n'est pas conforme à la syntaxe LSE (erreur C.n) ; soit des erreurs au niveau de l'exécution : Mn en mode machine de bureau, En ou EX n en mode programmé. Il faudra consulter la liste des erreurs fournie par le constructeur pour en avoir le libellé exact. » L'Annexe II (l. 8217, 8230) répète : « une erreur de type C… », « une erreur de type E ou EX en ligne… », libellés dans le « code des erreurs ».

LAFOND83 (p. 19, note 5) : « Ces messages se présentent, sur les systèmes existants, sous forme de numéros d'erreur. Ces numéros sont propres à chaque type de matériel. Il faudra donc consulter la liste fournie par le constructeur. » La même page range dans les erreurs de compilation toute faute détectée à l'analyse d'une commande, d'une ligne LSE ou d'une instruction en mode machine de bureau.

MEMENTO82 (annexes I à III) est la seule liste complète connue : erreurs de compilation `C XX`, erreurs d'exécution, erreurs « fichier » (`02-HISTORIQUE/MEMENTO82-transcription.md`).

### 2.2 Messages attestés

| Message imprimé | Situation | Source | Numéro MEMENTO82 de même nature |
|---|---|---|---|
| `ERREUR  C 08` | `?''!'PIERRE` (chaîne non terminée, machine de bureau) | INRDP75, l. 995 | C 08 (même numéro) |
| `ERREUR  C 03` | `AFFICHEP`, identificateur de 8 lettres (T1600) | Relevé T1600, SPHIN `numerisation0022` | C 03 (même numéro) |
| `ERREUR  C 03` | `AFICHER 'IL FAIT BEAU.'` (identificateur de 7 lettres) | LAFOND83, p. 19 | C 03 (même numéro) |
| `ERREUR  C 36` | ligne de plus de 80 caractères coupée, `FIN` perdu (T1600) | Relevé T1600, SPHIN `numerisation0022` | C 36 (même numéro) |
| `ERREUR  C 38` | `PRE` ; `ALHER EN 37` (identificateur sans `←`) (T1600) | Relevé T1600, ANGLO V2, V3 | C 38 (même numéro) |
| `ERREUR  C 38` | `LIS` (commande mal frappée, lue comme instruction) | LAFOND83, p. 19 | C 38 (même numéro) |
| `ERREUR  C 63` | ligne de plus de 80 caractères lue depuis le ruban, suivie de `**** LIGNE PRECEDENTE IMPØSSIBLE A DECØMPILER : …` (T1600, feuille de 6ème, 1978-79, antérieure à F4 ; la séquence, `C 36`, `C 63`, message recouvrant le texte décompilé, premier caractère de la ligne suivante perdu, n'est pas reproduite : choix d'Éric du 2026-11-04, point 12) | Relevé T1600, SPHIN `numerisation0022` | absent de MEMENTO82 |
| `ERREUR (EX.) 30 EN LIGNE BUR` | `?EQC(130)` en machine de bureau ; 130 n'est invalide que sur T1600 (borne 128, INRDP75 l. 1416) | INRDP75, l. 1426 | 30 « EQC illégal » (même numéro) |
| `ERREUR (EX.) 03` | tables 20×20, « débordement mémoire » (INRDP75, l. 3484) | INRDP75, l. 3542 | 03 « Mémoire saturée » (même numéro) |
| `ERREUR (EX.) 10 EN LIGNE 011` | `SI I=N` après `EXECUTER A PARTIR DE 10` | INRDP75, l. 2059 | 10 « relation appliquée à 2 objets de type différents » (plausible) |
| `ERREUR F 03` | `DUPLIQUER $*,P` (T1600) | Relevé T1600, ANGLO V4, SPHIN | F 03 « Programme déjà existant » (même numéro) |
| `ERREUR FL 06` | `DUPLIQUER $*,D` (T1600) | Relevé T1600, ANGLO V4 | classe `FL` absente de MEMENTO82 ; feuille antérieure à F4, non reproduite |
| `ERREUR E362 LIGNE 25` | indice supérieur au maximum déclaré | INRDP75, Annexe II, l. 8281 | 49 (numéro différent) |
| `ERREUR E210 LIGNE 30` | « rangement d'une chaîne dans une variable non déclarée chaîne » | LAFOND83, p. 130 | 06 vraisemblablement (numéro différent) |
| `ERREUR E160 LIGNE 1` | programme sans `TERMINER` ni `PAUSE` | LAFOND83, p. 19 | 58 « Le programme n'a pas de n° de ligne supérieure » (numéro différent) |
| `ERREUR M200` | `?RADIC=`, variable sans valeur, machine de bureau | LAFOND83, p. 130 | 62 vraisemblablement (numéro différent) |

### 2.3 Constats

1. **Erreurs de compilation.** Les numéros `C` concordent partout où une comparaison est possible : T1600 (Victor Duruy), INRDP75, LAFOND83 et MEMENTO82, soit C 03, C 08, C 36 et C 38. Aucune contradiction n'est relevée.
2. **Erreurs d'exécution et de fichier : deux familles.** Les codes à deux chiffres (`(EX.) 03`, `(EX.) 30`, `(EX.) 10`, `F 03`) concordent avec MEMENTO82. L'exemple `EQC(130)` les situe sur T1600. Les codes à trois chiffres (`E160`, `E210`, `E362`, `M200`) suivent une autre numérotation. L'attribution de cette seconde famille au MITRA 15 est une conjecture : LAFOND83 imprime `TERMINE` sans numéro de ligne, forme MITRA 15 selon INRDP75 (l. 1972), et la commande `UTILISATION DISQUE A PARTIR DE` y figure (indice relevé par Éric). Le texte ne nomme pas la machine, et décrit vraisemblablement une version postérieure à LSE15.
3. **Forme.** Trois formes sont attestées pour l'exécution : `(EX.) nn EN LIGNE nnn` ou `EN LIGNE BUR`, `Ennn LIGNE n` et `Mnnn`. La forme retenue est fixée au §4 (IMP-0186).

---

## 3. Numérotation des codes d'erreur

**IMP-0185** — *Catalogue unique organisé par nature d'erreur ; numéros MEMENTO82 lorsqu'ils existent, numéros à partir de 100 sinon ; même numérotation pour les deux profils*

- **Sujet :** quel numéro afficher pour chaque erreur, sachant que la liste du constructeur à laquelle renvoient INRDP75 et LAFOND83 n'est pas dans le corpus (LSE15 se borne à indiquer qu'« une erreur sera détectée »), que MEMENTO82 donne une liste complète pour une autre implémentation, et que les attestations montrent au moins deux familles de numérotation pour l'exécution (§2.3) ?
- **Choix :**
  1. **Catalogue par nature.** Le gestionnaire des erreurs tient un catalogue unique. Chaque entrée est une nature d'erreur : une situation qui appelle une correction propre de la part de l'utilisateur. Elle porte une classe (compilation `C`, exécution, fichier `F`), un domaine du Tome IV (§17.1) et un numéro affiché. Les composants désignent une nature par un identifiant symbolique (`ERR_*`), jamais par un numéro.
  2. **Numéro MEMENTO82.** Lorsqu'une nature correspond à une entrée de MEMENTO82 de la même classe, elle reçoit le numéro de cette entrée. Le rattachement est noté au catalogue avec sa page.
  3. **Numéros à partir de 100.** Une nature sans correspondance dans MEMENTO82 reçoit un numéro supérieur ou égal à 100, attribué en séquence dans sa classe. Aucun numéro de 1 à 99 n'est attribué hors de MEMENTO82, y compris dans les plages que MEMENTO82 ne liste pas (C 59 à C 88 et autres numéros absents). Aucun numéro déjà attesté avec un autre sens n'est attribué : 160, 200, 210 et 362 sont exclus en exécution.
  4. **C 63.** La nature « ligne de plus de 79 caractères » (inventaire D-01) reçoit le numéro C 63, attesté sur T1600 dans cette seule situation. Le sens de C 63 est déduit du contexte, non documenté : le rattachement est noté « sous réserve » au catalogue.
  5. **Profils.** La même numérotation s'applique aux profils T1600 et MITRA 15. Les numéros d'exécution à trois chiffres attestés (§2.2) sont cités au catalogue en regard de la nature correspondante, comme divergence d'implantation, mais ne sont pas affichés.
  6. **Hors périmètre de cette décision :** la forme du message (§4, IMP-0186) et le partage d'un même numéro MEMENTO82 entre plusieurs natures plus fines (§5, IMP-0187).
- **Alternatives envisagées :**
  1. Une seule table aux numéros MEMENTO82, sans numéros complémentaires distincts — écartée : la granularité voulue exige des natures que MEMENTO82 ne distingue pas, et il faudrait leur donner des numéros pris dans les plages non listées.
  2. Une numérotation par profil, la famille à trois chiffres pour MITRA 15 — écartée : quatre codes seulement sont connus, l'attribution au MITRA 15 est conjecturale, et presque toute la table devrait être inventée.
  3. Une numérotation propre, avec correspondance vers MEMENTO82 — écartée : elle abandonnerait des numéros attestés sur T1600 et dans INRDP75.
  4. Des numéros complémentaires pris dans la plage C 59 à C 88, que MEMENTO82 réserve à des « tests de mise au point » — écartée : `C 63` montre que le T1600 range de vraies erreurs dans cette plage, et un numéro choisi là pourrait prendre silencieusement le sens d'un code historique inconnu.
- **Justification :** les numéros de compilation concordent entre T1600, INRDP75, LAFOND83 et MEMENTO82, et les numéros d'exécution à deux chiffres concordent entre T1600, INRDP75 et MEMENTO82 (§2.3). MEMENTO82 est donc la meilleure approximation disponible de la liste du constructeur à laquelle renvoient INRDP75 et LAFOND83. Les numéros à partir de 100 rendent visible, dans le message même, qu'un code n'est pas historique, sans rien attribuer en silence. LAFOND83 précise que les numéros « sont propres à chaque type de matériel » : le numéro n'est pas une propriété du langage, et le choisir ne modifie aucune EXI.
- **Impact mémoire :** une table constante d'une entrée par nature (classe, domaine, numéro), quelques centaines d'octets dans le segment du module `erreurs`.
- **Impact disque :** sans objet.
- **Impact performances :** une consultation de table à l'affichage d'une erreur.
- **Impact historique :** les numéros attestés sont restitués à l'identique pour le T1600. Pour le profil MITRA 15, la numérotation à trois chiffres attestée par LAFOND83 et l'Annexe II d'INRDP75 n'est pas reproduite : divergence assumée et documentée, faute de liste.
- **Références normatives :** Tome IV §3.3, §17 ; EXI-0019, EXI-0051 ; INRDP75 l. 668-675, 995, 1416, 1426, 2059, 3484, 3542, 8217-8230, 8281 ; LAFOND83 p. 19 (note 5) et p. 130 ; MEMENTO82, annexes I à III ; `02-HISTORIQUE/Releve-sorties-T1600-Victor-Duruy.md` ; NOYELLE89 ; chapitre Compilateur, IMP-0168 ; inventaire des diagnostics, §12.
- **Statut :** retenu (choix d'Éric du 2026-10-22).

---

## 4. Forme des messages

**IMP-0186** — *Forme T1600 pour les deux profils ; code seul par défaut, libellé sur option de lancement ; pas de ligne d'appel*

- **Sujet :** sous quelle forme afficher une erreur de compilation, d'exécution ou de fichier, sachant que deux familles de formes d'exécution sont attestées (§2.2), que LSE15 (l. 2309) décrit un message d'arrêt portant aussi la ligne d'appel d'une procédure, et qu'aucun message attesté n'affiche de libellé ?
- **Choix :**
  1. **Compilation :** `ERREUR  C nn`, soit le mot `ERREUR`, deux espaces, `C`, une espace, puis le numéro (IMP-0185). Un numéro de 1 à 99 s'écrit sur deux chiffres, complété d'un zéro (`C 03`) ; un numéro à partir de 100 s'écrit tel quel (`C 101`). Le message suit l'écho de la ligne rejetée et ne rappelle pas le numéro de ligne. La forme est la même pour une ligne de programme, une ligne en mode machine de bureau et une ligne reçue d'ENCODER.
  2. **Exécution en mode programmé :** `ERREUR (EX.) nn EN LIGNE lll`, où `lll` est le numéro de la ligne en cours d'exécution, écrit sur trois chiffres et complété de zéros (`EN LIGNE 011`), comme dans les messages `TERMINE EN LIGNE 017` et `PRET EN LIGNE 036` du T1600.
  3. **Exécution en mode machine de bureau :** `ERREUR (EX.) nn EN LIGNE BUR`.
  4. **Fichier :** `ERREUR F nn`, avec une seule espace avant la classe. La classe `FL`, attestée une fois sur T1600 et de sens inconnu, n'est pas employée.
  5. **Profils :** ces formes valent pour le profil T1600 comme pour le profil MITRA 15.
  6. **Ligne d'appel :** le numéro de la ligne d'où une procédure a été appelée n'est pas affiché. La propriété décrite par LSE15 (l. 2309, « éventuellement ») n'est pas réalisée, faute de forme attestée.
  7. **Libellé :** par défaut, le message porte le code seul. Le texte de chaque entrée du catalogue est livré sous la forme d'une « liste des erreurs », document joint à l'application, comme l'était la liste du constructeur. Un paramètre de la ligne de commande au lancement, `-LIBELLES`, fait suivre chaque message, sur la même ligne, de son libellé ; ce paramètre est lu par la couche plateforme comme le profil (IMP-0180) et ne change pas pendant la session. Le nom exact du paramètre peut être révisé sans changer la décision.
- **Alternatives envisagées :**
  1. Une forme par profil, `ERREUR Enn LIGNE n` et `ERREUR Mnn` sous MITRA 15 — écartée : elle afficherait des messages d'allure MITRA 15 avec des numéros MEMENTO82 qui ne sont pas ceux de la famille à trois chiffres. Par exemple, une variable non définie s'afficherait `M62`, alors que l'attestation est `M200`. Ce serait une attribution silencieuse, ce qu'IMP-0185 exclut. INRDP75 (l. 668-675) cite « En ou EX n » sans les répartir entre les machines, alors qu'il répartit les formes de `TERMINE` (l. 1972).
  2. Ajouter la ligne d'appel sous le profil MITRA 15 — écartée : aucune graphie n'est attestée, et il faudrait en inventer une.
  3. Toujours afficher le libellé — écartée comme comportement par défaut : elle modifierait le dialogue observable, que toutes les attestations montrent réduit au code.
  4. Une commande donnant le libellé d'un code — écartée : elle ajouterait une commande au langage de commande historique.
- **Justification :** la forme de compilation est uniforme dans toutes les sources (§2.3). Pour l'exécution et les fichiers, la forme retenue est celle qui accompagne, dans les attestations, la numérotation retenue par IMP-0185 : forme et numérotation restent cohérentes. L'option de lancement sert le principe de NOYELLE89 (« faciliter au maximum la vie de celui qui l'emploie ») sans modifier ni le langage, ni le langage de commande, ni le comportement par défaut.
- **Impact mémoire :** les libellés occupent quelques kilo-octets ; ils peuvent rester dans le segment du module `erreurs` ou être lus depuis le fichier de la liste des erreurs. Ce choix relève de l'écriture du module.
- **Impact disque :** un fichier texte pour la liste des erreurs.
- **Impact performances :** négligeable, à l'affichage d'une erreur seulement.
- **Impact historique :** formes T1600 restituées ; pour le profil MITRA 15, divergence documentée sur la forme d'exécution (§2.2) et sur la ligne d'appel (LSE15, l. 2309). Le libellé n'apparaît que sur demande explicite au lancement.
- **Références normatives :** Tome IV §3.3, §17 ; EXI-0019, EXI-0061 ; INRDP75 l. 668-675, 995, 1426, 1972, 2059, 3542 ; LSE15 l. 2309 ; LAFOND83 p. 19, p. 130 ; `02-HISTORIQUE/Releve-sorties-T1600-Victor-Duruy.md` ; NOYELLE89 ; IMP-0180, IMP-0185.
- **Statut :** retenu (choix d'Éric du 2026-10-22).

**Variante non reproduite :** INRDP75 (l. 3542) montre une fois `ERREUR (EX.) 03` sans `EN LIGNE`. Le texte ne dit pas pourquoi la ligne manque, et la forme avec ligne est retenue dans tous les cas.

---

## 5. Granularité : natures distinctes sous un même numéro

**IMP-0187** — *Une nature d'erreur par situation à corriger ; numéro MEMENTO82 partagé par les natures qu'il regroupe ; libellé propre à chaque nature*

- **Sujet :** plusieurs entrées de MEMENTO82 couvrent des situations qui appellent des corrections différentes. Par exemple, C 58 couvre toutes les interdictions du mode machine de bureau, C 19 toute expression incorrecte, C 41 tout facteur de répétition hors bornes. Comment concilier la granularité fine voulue (§1) et la numérotation retenue (IMP-0185) ?
- **Choix :**
  1. Le catalogue distingue une nature pour chaque situation qui appelle une correction propre. Chaque nature a son identifiant symbolique `ERR_*` et son libellé.
  2. Les natures qui relèvent d'une même entrée de MEMENTO82 affichent toutes le numéro de cette entrée. Par exemple, `ALLER EN 10` et, sous le profil MITRA 15, `TABLEAU M[3]` en mode machine de bureau affichent tous deux `ERREUR  C 58`.
  3. La distinction entre natures est portée :
     - par le libellé, affiché sur option de lancement (IMP-0186) ;
     - par la liste des erreurs, qui énumère sous chaque numéro les natures qu'il recouvre ;
     - par le diagnostic transmis entre composants, qui désigne la nature et non le numéro ;
     - par les tests, qui vérifient la nature.
  4. Une nature sans entrée correspondante dans MEMENTO82 reçoit un numéro propre à partir de 100 (IMP-0185). Deux natures ne partagent un numéro de 100 ou plus que si une décision le prévoit explicitement.
- **Alternatives envisagées :**
  1. Un numéro complémentaire, à partir de 100, pour chaque nature autre que la principale d'un groupe — écartée : des situations dont le numéro est attesté, comme C 58, s'afficheraient avec un numéro inventé.
  2. Le numéro partagé par défaut, avec des numéros complémentaires au cas par cas — écartée : le critère « nature vraiment différente » ne se fonde sur aucune source et rouvrirait la question à chaque regroupement ; les cas limites, comme C 41 (observation O3), sont traités par la nature et son libellé.
- **Justification :** le code affiché reste celui des attestations. La granularité, qui relève de l'aide à l'utilisateur (NOYELLE89), est portée par la nature, et IMP-0186 permet d'afficher son libellé sans changer le message par défaut.
- **Impact mémoire :** un libellé par nature, au lieu d'un par numéro.
- **Impact disque :** la liste des erreurs détaille les natures sous chaque numéro.
- **Impact performances :** sans objet.
- **Impact historique :** neutre pour le message par défaut, identique au message attesté.
- **Références normatives :** IMP-0185, IMP-0186 ; MEMENTO82, annexe I ; NOYELLE89 ; inventaire des diagnostics, §12 (R3).
- **Statut :** retenu (choix d'Éric du 2026-10-22).

---

## 6. Divergences consignées avec MEMENTO82

MEMENTO82 documente une autre implantation (§2.1). Lorsqu'une de ses entrées décrit un comportement que le corpus des deux profils contredit, le comportement des profils est conservé, et la divergence est consignée ici :

| Entrée MEMENTO82 | Comportement MEMENTO82 | Comportement des profils T1600 et MITRA 15 | Source |
|---|---|---|---|
| C 48, premier cas | « Double déclaration de nom de procédure » rejetée à la compilation | Pas d'erreur : la déclaration de plus grand numéro est prise en compte (chapitre Compilateur, IMP-0176). Le contrôle exigerait de lire d'autres lignes (EXI-0001) | LSE15 §7-6, Note |
| C 41 | Facteur de répétition « supérieur à 63 ou nul » rejeté | Bornes de 1 à 62 (T1600) et de 1 à 99 (MITRA 15) ; C 41 couvre les natures « facteur nul », « facteur hors de la borne du profil » et « facteur supérieur à 99 » (IMP-0187) | INRDP75, note 7 du chapitre « Instruction de sortie. Format » |

Les deux autres cas de C 48 (doublon de paramètres noms de procédure ; paramètre homonyme de la procédure déclarée) sont repris par le chapitre Compilateur (IMP-0175).

---

## 7. Catalogue des natures d'erreur de compilation

**Statut :** retenu (choix d'Éric du 2026-10-23), y compris les rattachements par proximité, les libellés et le regroupement des constructions propres à l'autre profil sous C 103.

Le catalogue applique IMP-0185 (numérotation), IMP-0186 (forme) et IMP-0187 (granularité) aux points de détection recensés par l'inventaire des diagnostics (D-01 à D-86), compte tenu des décisions du chapitre Compilateur (IMP-0175, IMP-0188 à IMP-0191). Toutes les natures relèvent du domaine `COMPILATION` (Tome IV §17.1) et s'affichent `ERREUR  C nn` (IMP-0186).

**Colonnes.**
- **Identifiant** : nom symbolique de la nature, employé par les composants (IMP-0185). Le préfixe `ERR_C_` désigne la classe compilation.
- **Code** : numéro affiché. Un numéro de 1 à 99 est celui de MEMENTO82 (annexe I) ; un numéro à partir de 100 est attribué ici, en séquence.
- **Libellé** : texte de la liste des erreurs, affiché après le code sur option `-LIBELLES` (IMP-0186). Il est écrit en majuscules sans accents, comme les messages historiques (`COMMANDE ERRONEE`, `TERMINE EN LIGNE`).
- **Points** : points de l'inventaire couverts.
- **Rattachement** : « attesté » (code observé dans cette situation, §2.2), « libellé » (la situation correspond au libellé de MEMENTO82), « proximité » (rattachement au libellé le plus proche, sans correspondance exacte), « nouveau » (aucune entrée de MEMENTO82).

### 7.1 Ligne et lexique

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_C_LIGNE_TROP_LONGUE` | C 63 | LIGNE DE PLUS DE 80 CARACTERES | D-01 | attesté, sens déduit (IMP-0185) |
| `ERR_C_NUMERO_SANS_ESPACE` | C 07 | NUMERO DE LIGNE NON SUIVI D'UN ESPACE | D-02 | libellé |
| `ERR_C_NUMERO_HORS_BORNES` | C 52 | NUMERO DE LIGNE NUL OU SUPERIEUR A 32000 | D-03, D-05 | libellé (IMP-0191) |
| `ERR_C_NUMERO_AU_DELA_PROFIL` | C 100 | NUMERO DE LIGNE AU-DELA DE LA BORNE DE CETTE MACHINE | D-04, D-05 | nouveau (IMP-0191) |
| `ERR_C_MINUSCULE` | C 01 | LETTRE MINUSCULE HORS D'UNE CHAINE | D-07 | proximité |
| `ERR_C_CARACTERE_INTERDIT` | C 01 | CARACTERE SPECIAL INTERDIT | D-08 | libellé |
| `ERR_C_MOT_MAL_TERMINE` | C 53 | MOT DU LANGAGE MAL TERMINE | D-09 | proximité |
| `ERR_C_IDENTIFICATEUR_TROP_LONG` | C 03 | IDENTIFICATEUR DE PLUS DE 5 CARACTERES | D-10 | attesté (IMP-0191) |
| `ERR_C_NOM_PROCEDURE_TROP_LONG` | C 05 | NOM DE PROCEDURE DE PLUS DE 5 CARACTERES | D-10 | libellé (IMP-0191) |
| `ERR_C_MOT_RESERVE` | C 101 | MOT RESERVE EMPLOYE COMME IDENTIFICATEUR | D-11 | nouveau |
| `ERR_C_MOT_RESERVE_PROFIL` | C 102 | MOT RESERVE DE CETTE MACHINE EMPLOYE COMME IDENTIFICATEUR | D-12 | nouveau |
| `ERR_C_CHAINE_NON_TERMINEE` | C 08 | APOSTROPHE DE FIN DE CHAINE MANQUANTE | D-13 | attesté |
| `ERR_C_EQUIVALENT_NON_TERMINE` | C 08 | POINT DE FIN D'EQUIVALENT NUMERIQUE MANQUANT | D-84 | libellé (IMP-0190) |
| `ERR_C_EQUIVALENT_HORS_BORNES` | C 109 | EQUIVALENT NUMERIQUE HORS DES BORNES DE EQC | D-85 | nouveau (IMP-0190) |
| `ERR_C_CONSTANTE_MAL_FORMEE` | C 09 | CONSTANTE NUMERIQUE INCORRECTE | D-14 | libellé |
| `ERR_C_CONSTANTE_HORS_PLAGE` | C 09 | CONSTANTE NUMERIQUE TROP GRANDE | D-15 | libellé (IMP-0191) |
| `ERR_C_NOM_PROCEDURE_INCORRECT` | C 06 | NOM DE PROCEDURE INCORRECT APRES & | D-16 | libellé |

### 7.2 Expressions et types

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_C_INSTRUCTION_INCORRECTE` | C 53 | INSTRUCTION INCORRECTE | D-06 | libellé (IMP-0191) |
| `ERR_C_PARENTHESE_EN_EXCES` | C 45 | PLUS DE ')' QUE DE '(' | D-06 | libellé (IMP-0191) |
| `ERR_C_OPERATEUR_MANQUANT` | C 19 | OPERATEUR MANQUANT ENTRE DEUX OPERANDES | D-06, D-19 | libellé (IMP-0191) |
| `ERR_C_OPERANDE_MANQUANT` | C 19 | OPERANDE MANQUANT | D-17, D-46 | libellé |
| `ERR_C_OPERATEURS_ADJACENTS` | C 19 | DEUX OPERATEURS ADJACENTS | D-18 | libellé |
| `ERR_C_PARENTHESE_MANQUANTE` | C 20 | PARENTHESE FERMANTE MANQUANTE | D-20 | libellé |
| `ERR_C_CROCHET_MANQUANT` | C 16 | CROCHET FERMANT MANQUANT | D-21 | libellé |
| `ERR_C_TROP_D_INDICES` | C 16 | PLUS DE DEUX INDICES | D-22 | proximité (« mauvais séparateur d'indices ») |
| `ERR_C_ARGUMENTS_FONCTION` | C 26 | NOMBRE D'ARGUMENTS INCORRECT POUR CETTE FONCTION | D-23 | proximité (« manque ')' dans une fonction ») |
| `ERR_C_ARGUMENT_FACULTATIF` | C 26 | DERNIER ARGUMENT DE SCH, GRL OU CNB : IDENTIFICATEUR ATTENDU | D-24 | proximité |
| `ERR_C_SINON_MANQUANT` | C 21 | SINON MANQUANT DANS UNE EXPRESSION CONDITIONNELLE | D-28 | libellé |
| `ERR_C_RELATION_MANQUANTE` | C 17 | OPERATEUR DE RELATION INCORRECT OU ABSENT | D-29 | libellé |
| `ERR_C_AROBASE_SANS_ENTIER` | C 93 | @ NON SUIVI D'UN ENTIER | D-27 | libellé (MITRA 15) |
| `ERR_C_CHAINE_AU_LIEU_NOMBRE` | C 27 | CHAINE AU LIEU D'UN NOMBRE | D-77, D-79 | libellé (IMP-0188) |
| `ERR_C_NOMBRE_AU_LIEU_CHAINE` | C 28 | NOMBRE AU LIEU D'UNE CHAINE | D-78, D-79 | libellé (IMP-0188) |
| `ERR_C_INDICE_NON_NUMERIQUE` | C 15 | INDICE NON NUMERIQUE | D-80 | libellé (IMP-0188) |
| `ERR_C_COMPARAISON_TYPES` | C 11 | COMPARAISON D'UN NOMBRE ET D'UNE CHAINE | D-81 | libellé (IMP-0188) |

### 7.3 Instructions

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_C_AFFECTATION_MANQUANTE` | C 38 | INSTRUCTION INCONNUE OU ← MANQUANT | D-30 | attesté |
| `ERR_C_CIBLE_AFFECTATION` | C 38 | LA CIBLE D'UNE AFFECTATION DOIT ETRE UNE VARIABLE | D-31 | libellé |
| `ERR_C_EN_MANQUANT` | C 37 | ALLER NON SUIVI DE EN | D-32 | libellé |
| `ERR_C_ALORS_MANQUANT` | C 22 | ALORS MANQUANT | D-33 | libellé |
| `ERR_C_FIN_MANQUANT` | C 36 | ';' OU FIN MANQUANT DANS DEBUT...FIN | D-34 | attesté |
| `ERR_C_DEBUT_INSTRUCTION` | C 55 | DEBUT D'INSTRUCTION INCORRECT | D-35, D-57 | libellé (D-57 : proximité) |
| `ERR_C_POUR_MANQUANT` | C 32 | POUR MANQUANT DANS FAIRE | D-36 | libellé (« variable de contrôle absente ») |
| `ERR_C_VARIABLE_BOUCLE` | C 32 | LA VARIABLE DE BOUCLE DOIT ETRE SIMPLE | D-37 | libellé |
| `ERR_C_AFFECTATION_BOUCLE` | C 33 | ← MANQUANT APRES LA VARIABLE DE BOUCLE | D-38 | libellé |
| `ERR_C_FIN_DE_BOUCLE` | C 34 | JUSQUA OU TANT QUE MANQUANT | D-39 | libellé |
| `ERR_C_TANT_SANS_QUE` | C 34 | TANT NON SUIVI DE QUE | D-40 | libellé |
| `ERR_C_NOM_TABLEAU` | C 39 | NOM DE TABLEAU INCORRECT OU ABSENT | D-41 | libellé |
| `ERR_C_DECLARATION_TABLEAU` | C 14 | DIMENSIONS DE TABLEAU INCORRECTES | D-41 | libellé |
| `ERR_C_DIMENSION_NON_NUMERIQUE` | C 14 | DIMENSION DE TABLEAU NON NUMERIQUE | D-82 | libellé (IMP-0188) |
| `ERR_C_NOM_DECLARATION` | C 35 | NOM DE VARIABLE INCORRECT DANS CHAINE OU LIBERER | D-42 | libellé |
| `ERR_C_CIBLE_LIRE` | C 104 | LA CIBLE DE LIRE DOIT ETRE UNE VARIABLE | D-43 | nouveau |
| `ERR_C_NOM_GARER_CHARGER` | C 44 | NOM DE VARIABLE INCORRECT OU ABSENT DANS GARER OU CHARGER | D-44 | libellé |
| `ERR_C_PARAMETRE_FICHIER` | C 29 | ERREUR DE PARAMETRE DANS GARER, CHARGER OU SUPPRIMER | D-44, D-45 | libellé |
| `ERR_C_ARGUMENTS_EXECUTER` | C 105 | EXECUTER : DEUX ARGUMENTS AU PLUS | D-45 | nouveau |
| `ERR_C_HORS_PROFIL` | C 103 | CONSTRUCTION ABSENTE DE CETTE MACHINE | D-25, D-26, D-27, D-47, D-86 | nouveau |
| `ERR_C_POINT_INTERROGATION` | C 106 | ? RESERVE AU MODE MACHINE DE BUREAU | D-48 | nouveau (IMP-0191) |

### 7.4 Format d'AFFICHER

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_C_FORMAT_CROCHET` | C 40 | ']' MANQUANT DANS UN FORMAT | D-49 | libellé |
| `ERR_C_FORMAT_SPECIFICATION` | C 42 | SPECIFICATION DE FORMAT INCORRECTE | D-50 | libellé |
| `ERR_C_FORMAT_CHIFFRES` | C 42 | E OU D DE PLUS DE DEUX CHIFFRES DANS UN FORMAT | D-51 | libellé |
| `ERR_C_FORMAT_ETOILE` | C 42 | '*' INTERDIT DEVANT UNE DESCRIPTION | D-55 | libellé |
| `ERR_C_FACTEUR_NUL` | C 41 | FACTEUR DE REPETITION NUL | D-52 | libellé |
| `ERR_C_FACTEUR_AU_DELA_PROFIL` | C 41 | FACTEUR DE REPETITION AU-DELA DE LA BORNE DE CETTE MACHINE | D-53 | libellé (IMP-0187) |
| `ERR_C_FACTEUR_SUPERIEUR_99` | C 41 | FACTEUR DE REPETITION SUPERIEUR A 99 | D-54 | libellé (IMP-0187) |
| `ERR_C_FORMAT_DESCRIPTIONS` | C 54 | PLUS DE DESCRIPTIONS QUE DE VALEURS | D-56 | libellé (IMP-0178) |

### 7.5 Procédures

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_C_DECLARATION_NOM` | C 49 | NOM DE PROCEDURE INCORRECT OU ABSENT | D-59 | libellé |
| `ERR_C_DECLARATION_PARENTHESES` | C 49 | NOM DE PROCEDURE NON SUIVI DE () | D-60 | libellé |
| `ERR_C_PARAMETRES_DECLARATION` | C 46 | LISTE DE PARAMETRES INCORRECTE | D-87 | libellé (ajoutée le 2026-10-23, IMP-0187) |
| `ERR_C_PARAMETRE_INDICE` | C 51 | PARAMETRE FORMEL INDICE | D-61 | libellé |
| `ERR_C_PARAMETRE_DOUBLE` | C 47 | DEUX PARAMETRES IDENTIQUES | D-62 | libellé (IMP-0175) |
| `ERR_C_PARAMETRE_PROCEDURE_DOUBLE` | C 48 | DEUX PARAMETRES PROCEDURE IDENTIQUES | D-62 | libellé (IMP-0175) |
| `ERR_C_PARAMETRE_HOMONYME` | C 48 | PARAMETRE IDENTIQUE AU NOM DE LA PROCEDURE | D-83 | libellé (IMP-0175) |
| `ERR_C_LOCAL_PROCEDURE` | C 35 | NOM DE PROCEDURE DANS LA LISTE LOCAL | D-63 | libellé |
| `ERR_C_LOCAL_DOUBLE` | C 47 | DEUX NOMS IDENTIQUES DANS LA LISTE LOCAL | D-64 | libellé (IMP-0175) |
| `ERR_C_DECLARATION_SUITE` | C 46 | TEXTE INATTENDU APRES LA DECLARATION DE PROCEDURE | D-65 | libellé |
| `ERR_C_APPEL_MAL_FORME` | C 46 | APPEL DE PROCEDURE MAL FORME | D-66 | libellé |

### 7.6 Mode machine de bureau

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_C_BUREAU_PROCEDURE` | C 58 | PROCEDURE INTERDITE EN MODE MACHINE DE BUREAU | D-58 | libellé |
| `ERR_C_BUREAU_NUMERO_LIGNE` | C 58 | ALLER EN, FAIRE, RETOUR EN INTERDITS EN MODE MACHINE DE BUREAU | D-67 | libellé |
| `ERR_C_BUREAU_RETOUR` | C 58 | RETOUR, RESULTAT INTERDITS EN MODE MACHINE DE BUREAU | D-68 | libellé |
| `ERR_C_BUREAU_APPEL_INSTRUCTION` | C 58 | APPEL DE PROCEDURE INTERDIT EN MODE MACHINE DE BUREAU | D-69 | libellé |
| `ERR_C_BUREAU_APPEL_FONCTION` | C 58 | APPEL DE FONCTION INTERDIT EN MODE MACHINE DE BUREAU | D-70 | libellé |
| `ERR_C_BUREAU_DECLARATION` | C 58 | DECLARATION INTERDITE EN MODE MACHINE DE BUREAU | D-71 | libellé |
| `ERR_C_BUREAU_EXECUTER` | C 58 | EXECUTER INTERDIT EN MODE MACHINE DE BUREAU | — | constaté sur LSE 8086 le 2026-10-25 (« Instruction interdite en mode calculateur de bureau ») ; Bibliothécaire et fichiers, point B6 |

### 7.7 Ressources

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_C_PLUS_DE_PLACE` | C 57 | PLUS DE PLACE DANS LA ZONE UTILISATEUR | D-73, D-74, D-75 | libellé (IMP-0191) |
| `ERR_C_LIGNE_TROP_COMPLEXE` | C 107 | LIGNE TROP COMPLEXE | D-72 | nouveau |
| `ERR_C_CAPACITE_TABLES` | C 108 | TROP DE NOMS OU DE CONSTANTES | D-76 | nouveau (IMP-0191) |

### 7.8 Bilan

- 83 natures à la rédaction (84 depuis le 2026-10-25, avec `ERR_C_BUREAU_EXECUTER`), couvrant les 87 points de l'inventaire (`ERR_C_PARAMETRES_DECLARATION`, D-87, ajoutée le 2026-10-23 à l'écriture de la déclaration de procédure).
- Numéros de 1 à 99 employés : 46, dont 45 pris dans MEMENTO82 (de C 01 à C 93) et C 63, attesté sur T1600 mais absent de MEMENTO82.
- Numéros nouveaux : 10, de C 100 à C 109. Les numéros réservés en exécution (160, 200, 210, 362) ne concernent pas la classe C.
- Rattachements par proximité, retenus par Éric le 2026-10-23 : `ERR_C_MINUSCULE` (C 01), `ERR_C_MOT_MAL_TERMINE` (C 53), `ERR_C_TROP_D_INDICES` (C 16), `ERR_C_ARGUMENTS_FONCTION` et `ERR_C_ARGUMENT_FACULTATIF` (C 26), et `PROCEDURE` ailleurs qu'en tête de ligne sous `ERR_C_DEBUT_INSTRUCTION` (C 55).

---

## 8. Structure du diagnostic et module `erreurs`

**IMP-0195** — *Nature d'erreur en `unsigned int` ; diagnostic de compilation réduit à la nature et à la position ; catalogue compilé dans `erreurs.c`*

- **Sujet :** sous quelle forme une erreur circule-t-elle entre les composants, et où le catalogue (§7) est-il conservé ? La structure provisoire `DiagnosticCompilation` du chapitre Compilateur (IMP-0168) portait un champ `code` dont la valeur restait à fixer.
- **Choix :**
  1. **Nature.** `typedef unsigned int NatureErreur;`. Chaque nature du catalogue est une constante `#define` de `src/noyau/erreurs.h`, numérotée à partir de 1 dans l'ordre du catalogue ; `ERR_AUCUNE` vaut 0. La valeur d'une constante n'est jamais affichée et ne signifie rien en dehors du programme : le numéro affiché est celui du catalogue (IMP-0185).
  2. **Diagnostic de compilation.** `typedef struct { NatureErreur nature; unsigned char position; } DiagnosticCompilation;`. Il remplace la structure provisoire d'IMP-0168 ; `position` suit la règle d'IMP-0191.
  2 bis. **Diagnostic d'exécution.** `typedef struct { NatureErreur nature; unsigned char ligne; } DiagnosticExecution;`. `ligne` est le numéro de la ligne en cours d'exécution, ou 0 en mode machine de bureau, qui s'affiche `EN LIGNE BUR` (IMP-0186). Il n'y a pas de ligne d'appel (IMP-0186). Ce point a été ajouté le 2026-10-23 (choix d'Éric, point E2 de l'étape 4).
  3. **Catalogue.** `src/noyau/erreurs.c`, dans le segment principal (IMP-0135), contient une table constante qui donne, pour chaque nature, sa classe (`'C'` pour la compilation), son numéro et son libellé. La classe vaut `'C'` pour la compilation et `'E'` pour l'exécution. Les libellés sont compilés dans le code, et non lus dans un fichier. La flèche d'affectation y est écrite `_` (ASCII 95), son code dans le jeu de caractères du L.S.E. (fiche de codage, restitution de `←`).
  4. **Fonctions.**
     - `erreurs_classe(n)`, `erreurs_numero(n)` et `erreurs_libelle(n)` rendent les propriétés d'une nature, ou 0 et `NULL` pour une nature inconnue ;
     - `erreurs_formater(n, avec_libelle, sortie, taille)` écrit le message de IMP-0186 pour une nature de compilation : `ERREUR  C nn`, suivi, si `avec_libelle` est vrai, de ` : ` et du libellé. Le texte est toujours terminé, et tronqué si `taille` ne suffit pas.

     - `erreurs_formater_execution(n, ligne, avec_libelle, sortie, taille)` écrit le message d'une nature d'exécution : `ERREUR (EX.) nn EN LIGNE lll`, le numéro de ligne sur trois chiffres complétés de zéros, ou `EN LIGNE BUR` si `ligne` vaut 0 ; un code de 1 à 99 s'écrit sur deux chiffres (`(EX.) 03`).

     Le module n'emploie ni `stdio.h` ni aucune fonction d'affichage : l'affichage relève de la console.
  5. **Longueur.** Un libellé de compilation compte au plus 62 caractères ; avec `ERREUR  C 101 : `, un message ne dépasse pas 78 caractères. Un libellé d'exécution compte au plus 48 caractères ; avec `ERREUR (EX.) 101 EN LIGNE 123 : `, un message ne dépasse pas 80 caractères.
- **Alternatives envisagées :**
  1. `unsigned char` pour la nature — écartée : avec les catalogues d'exécution et de fichier à venir, le nombre de natures dépassera vraisemblablement 255.
  2. Faire circuler le numéro affiché plutôt que la nature — écartée : plusieurs natures partagent un numéro (IMP-0187), et la nature, plus fine, serait perdue.
  3. Lire les libellés dans le fichier de la liste des erreurs — écartée pour l'instant : le module des fichiers n'existe pas encore, et environ 3 Kio de texte restent compatibles avec la limite de 64 Kio du segment principal. Le choix peut être révisé sans changer l'interface.
- **Justification :** la nature porte toute la granularité voulue (§1, IMP-0187) ; le numéro, la classe et le libellé s'en déduisent par une seule table, ce qui garde le catalogue en un seul endroit du code.
- **Impact mémoire :** environ 3 Kio de libellés et 7 octets de table par nature (classe, numéro, adresse du libellé), dans le segment principal.
- **Impact disque :** sans objet.
- **Impact performances :** un accès indexé par erreur affichée.
- **Impact historique :** neutre. Le message par défaut est celui des attestations.
- **Références normatives :** Tome IV §3.3, §17 ; IMP-0135, IMP-0168, IMP-0185, IMP-0186, IMP-0187, IMP-0191 ; §7.
- **Statut :** retenu (choix d'Éric du 2026-10-23).

---

## 9. Catalogue des natures d'erreur d'exécution

**Statut :** retenu (arbitrages d'Éric du 2026-10-23, points RX1 à RX7 de l'inventaire `05-IMPLEMENTATION/audits/Inventaire_diagnostics_execution_2026-10-23.md`).

Le catalogue applique IMP-0185, IMP-0186 et IMP-0187 aux points X-01 à X-63 de l'inventaire. Les points X-64 à X-66 (commandes `EXECUTER A PARTIR DE`, `CONTINUER`, `REPRENDRE EN`) relèvent du chapitre du moniteur, domaine `DIALOGUE`. Le point X-53 (réponse à `LIRE` mal formée) attend le chapitre de l'interpréteur, qui doit établir si la saisie est reprise ou si l'exécution s'arrête.

Toutes les natures relèvent du domaine `EXECUTION` (Tome IV §17.1). Elles s'affichent `ERREUR (EX.) nn EN LIGNE lll`, ou `EN LIGNE BUR` en mode machine de bureau (IMP-0186).

**Colonnes.** Elles sont celles du §7. Le préfixe `ERR_E_` désigne la classe exécution. Un code de 1 à 99 est celui de MEMENTO82 (annexe II) ; un code à partir de 100 est attribué ici, en séquence. Avec le préfixe `ERREUR (EX.) nn EN LIGNE lll : ` (32 caractères au plus), un libellé compte au plus 48 caractères, pour qu'un message tienne sur 80 colonnes.

**Arbitrages appliqués.**
- **RX1. Variable non définie.** Trois natures, selon l'endroit où la variable est lue : 05 dans un calcul, 62 pour une affectation ou un affichage, 10 dans une comparaison (attesté sur T1600, INRDP75 l. 2059).
- **RX2. Sorties de procédure.** Un `RETOUR` sans `EN` d'une procédure appelée dans une expression est une erreur (15). Un `RESULTAT` dans une procédure appelée en instruction est une erreur (22). Les deux erreurs ont été constatées par Éric sur LSE 8086 version IV le 2026-10-23. `RETOUR EN`, seul, abandonne l'expression sans erreur (LSE15 §7-5).
- **RX3. Paramètre `LOCAL` sans valeur.** L'erreur 14 est détectée à l'utilisation du paramètre, non à l'appel (MEMENTO82 ; constat d'Éric sur LSE 8086 version IV : `&P(Z)` sur `Z` non défini, puis `AFFICHER 'DANS P'`, s'exécute sans erreur).
- **RX4.** `n1` nul ou négatif : 37 ; `n1` inférieur au numéro de la ligne `FAIRE` : numéro à partir de 100.
- **RX5.** Fin du programme sans `TERMINER` (58) et branchement vers une ligne absente (75) : deux natures.
- **RX6.** Position au-delà de la longueur + 1 dans `SCH`, `PTR` ou `CNB` : 26 ; toute autre erreur de position de départ : 11.
- **RX7.** Le tableau entier employé dans un calcul relève de 05 ; la nouvelle déclaration d'un tableau passé en paramètre a sa nature (29) ; le module introuvable pour `EXECUTER` a une nature d'exécution à partir de 100.

### 9.1 Identificateurs et types

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_E_NON_DEFINI_CALCUL` | EX 05 | VARIABLE NON DEFINIE DANS UN CALCUL | X-01 | libellé (RX1) |
| `ERR_E_NON_DEFINI_AFFECTATION` | EX 62 | VARIABLE NON DEFINIE AFFECTEE OU AFFICHEE | X-02 | libellé (RX1) ; `M200` (LAFOND83), non affiché |
| `ERR_E_NON_DEFINI_COMPARAISON` | EX 10 | VARIABLE NON DEFINIE DANS UNE COMPARAISON | X-03 | attesté (INRDP75 l. 2059, RX1) |
| `ERR_E_CHAINE_SANS_VALEUR` | EX 62 | VARIABLE-CHAINE SANS VALEUR | X-04 | proximité |
| `ERR_E_CHAINE_NON_DECLAREE` | EX 06 | CHAINE RANGEE DANS UNE VARIABLE NON CHAINE | X-05 | libellé ; `E210` (LAFOND83), non affiché |
| `ERR_E_NOMBRE_DANS_CHAINE` | EX 06 | NOMBRE RANGE DANS UNE CHAINE OU UN TABLEAU | X-06 | libellé |
| `ERR_E_CALCUL_NON_NUMERIQUE` | EX 05 | CHAINE OU TABLEAU DANS UN CALCUL | X-10, X-25 | libellé (RX7) |
| `ERR_E_COMPARAISON_TYPES` | EX 10 | COMPARAISON D'UN NOMBRE ET D'UNE CHAINE | X-26 | libellé |
| `ERR_E_OPERANDE_NON_CHAINE` | EX 08 | OPERANDE NON CHAINE DANS UNE EXPRESSION CHAINE | X-27 | libellé |
| `ERR_E_DECLARATION_TABLEAU` | EX 29 | DECLARATION TABLEAU D'UNE VARIABLE OU CHAINE | X-07 | libellé |
| `ERR_E_DECLARATION_CHAINE` | EX 04 | DECLARATION CHAINE D'UNE VARIABLE OU TABLEAU | X-08 | libellé |
| `ERR_E_TABLEAU_PARAMETRE` | EX 29 | TABLEAU PARAMETRE DECLARE DE NOUVEAU | X-50 | proximité (RX7) |
| `ERR_E_LIBERER_NON_LOCAL` | EX 100 | LIBERER D'UN IDENTIFICATEUR NON LOCAL | X-09 | nouveau |

### 9.2 Tableaux et indices

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_E_INDICES_SANS_TABLEAU` | EX 47 | INDICES SUR UN IDENTIFICATEUR NON TABLEAU | X-11 | libellé |
| `ERR_E_INDICE_TROP_GRAND` | EX 49 | INDICE SUPERIEUR A LA BORNE DECLAREE | X-12 | libellé ; `E362` (INRDP75 l. 8281), non affiché |
| `ERR_E_INDICE_NUL` | EX 27 | INDICE NUL OU NEGATIF | X-13 | libellé |
| `ERR_E_NOMBRE_INDICES` | EX 46 | NOMBRE D'INDICES NON CONFORME A LA DECLARATION | X-14 | libellé |
| `ERR_E_DIMENSION_NULLE` | EX 27 | DIMENSION DE TABLEAU NULLE OU NEGATIVE | X-15 | libellé |
| `ERR_E_TABLEAU_TROP_GRAND` | EX 45 | TABLEAU DE PLUS DE 65535 ELEMENTS | X-16 | libellé |
| `ERR_E_ELEMENT_SANS_VALEUR` | EX 32 | ELEMENT DE TABLEAU SANS VALEUR | X-17 | libellé ; à confirmer au chapitre `donnees` |

### 9.3 Arithmétique

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_E_DIVISION_PAR_ZERO` | EX 31 | DIVISION PAR ZERO | X-18 | libellé |
| `ERR_E_ZERO_PUISSANCE_NEGATIVE` | EX 25 | ZERO ELEVE A UNE PUISSANCE NEGATIVE | X-19 | libellé |
| `ERR_E_PUISSANCE_NON_ENTIERE` | EX 26 | NOMBRE NEGATIF A UNE PUISSANCE NON ENTIERE | X-20 | libellé (second libellé de 26) |
| `ERR_E_RAC_NEGATIF` | EX 41 | RAC D'UN NOMBRE NEGATIF | X-21 | libellé |
| `ERR_E_LGN_NEGATIF` | EX 42 | LGN D'UN NOMBRE NEGATIF OU NUL | X-22 | libellé |
| `ERR_E_DEPASSEMENT` | EX 44 | DEPASSEMENT DE CAPACITE ARITHMETIQUE | X-23 | libellé |
| `ERR_E_ARGUMENT_ALE` | EX 09 | ARGUMENT DE ALE NEGATIF OU SUPERIEUR A 1 | X-24 | libellé |

### 9.4 Chaînes

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_E_EQC_HORS_BORNES` | EX 30 | ARGUMENT DE EQC HORS BORNES | X-28 | attesté (INRDP75 l. 1426) |
| `ERR_E_POSITION_AU_DELA` | EX 26 | POSITION SUPERIEURE A LA LONGUEUR + 1 | X-29 | libellé (RX6 : `SCH`, `PTR`, `CNB`) |
| `ERR_E_POSITION_ERRONEE` | EX 11 | POSITION DE DEPART ERRONEE | X-29 | libellé (RX6 : autres cas) |
| `ERR_E_LONGUEUR_NEGATIVE` | EX 17 | LONGUEUR DEMANDEE NEGATIVE | X-30 | proximité |
| `ERR_E_ARGUMENT_2_NON_NUMERIQUE` | EX 18 | DEUXIEME ARGUMENT NON NUMERIQUE | X-31 | libellé |
| `ERR_E_ARGUMENT_3_ERRONE` | EX 17 | TROISIEME ARGUMENT NI CHAINE NI NOMBRE | X-32 | libellé |
| `ERR_E_POSITION_EQN` | EX 21 | POSITION ERRONEE DANS EQN | X-33 | libellé |
| `ERR_E_CHAINE_TROP_LONGUE` | EX 101 | CHAINE TROP LONGUE | X-34 | nouveau ; borne au chapitre `donnees` |

### 9.5 Contrôle

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_E_LIGNE_INEXISTANTE` | EX 75 | LIGNE DE BRANCHEMENT INEXISTANTE | X-35 | libellé (RX5) |
| `ERR_E_LIGNE_NULLE` | EX 37 | NUMERO DE LIGNE DE BRANCHEMENT NUL | X-36 | libellé |
| `ERR_E_LIGNE_NON_ENTIERE` | EX 38 | NUMERO DE LIGNE NON CONVERTIBLE EN ENTIER | X-37 | libellé |
| `ERR_E_FIN_DE_PROGRAMME` | EX 58 | FIN DU PROGRAMME SANS TERMINER | X-38 | libellé (RX5, IMP-0154) |
| `ERR_E_FIN_BOUCLE_NULLE` | EX 37 | FIN DE BOUCLE NULLE OU NEGATIVE | X-39 | proximité (RX4) |
| `ERR_E_FIN_BOUCLE_AVANT` | EX 102 | FIN DE BOUCLE AVANT LA LIGNE FAIRE | X-40 | nouveau (RX4) |
| `ERR_E_VARIABLE_BOUCLE` | EX 43 | VARIABLE DE BOUCLE NON NUMERIQUE | X-41 | libellé |
| `ERR_E_LIGNE_DECLARATION` | EX 12 | EXECUTION D'UNE LIGNE DE DECLARATION | X-42 | libellé (IMP-0155) |

### 9.6 Procédures

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_E_PROCEDURE_NON_DECLAREE` | EX 103 | PROCEDURE NON DECLAREE | X-43 | nouveau |
| `ERR_E_NOMBRE_PARAMETRES` | EX 13 | NOMBRE DE PARAMETRES EFFECTIFS INCORRECT | X-44 | libellé |
| `ERR_E_PARAMETRE_REFERENCE` | EX 72 | EXPRESSION PASSEE A UN PARAMETRE REFERENCE | X-45 | libellé |
| `ERR_E_PARAMETRE_SANS_VALEUR` | EX 14 | PARAMETRE LOCAL SANS VALEUR | X-46 | libellé (RX3 : à l'utilisation) |
| `ERR_E_RETOUR_HORS_PROCEDURE` | EX 20 | RETOUR OU RESULTAT HORS PROCEDURE | X-47 | libellé |
| `ERR_E_RETOUR_DANS_FONCTION` | EX 15 | RETOUR SANS RESULTAT DANS UNE FONCTION | X-48 | libellé (RX2, LSE 8086) |
| `ERR_E_RESULTAT_SOUS_PROGRAMME` | EX 22 | RESULTAT DANS UNE PROCEDURE SOUS-PROGRAMME | X-49 | libellé (RX2, LSE 8086) |
| `ERR_E_RESULTAT_TABLEAU` | EX 59 | RESULTAT QUI N'EST NI UN NOMBRE NI UNE CHAINE | X-67 | libellé (ajouté le 2026-10-23 à l'écriture du retour de procédure) |

### 9.7 Entrée-sortie, fichiers, modules

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_E_FACTEUR_NEGATIF` | EX 104 | FACTEUR DE REPETITION NEGATIF | X-51 | nouveau ; un facteur nul est admis (LSE15 §6-2.6 : « si NBSP vaut 0, aucun espace ne sera mis »), corrigé le 2026-10-24 |
| `ERR_E_VALEUR_FORMAT` | EX 06 | VALEUR CONTRAIRE A LA DESCRIPTION DU FORMAT | X-52 | proximité ; à confirmer au chapitre de l'interpréteur |
| `ERR_E_NUMERO_ENREGISTREMENT` | EX 50 | NUMERO D'ENREGISTREMENT INCORRECT | X-54 | libellé |
| `ERR_E_FICHIER_PLEIN` | EX 52 | PLUS DE PLACE DANS LE FICHIER | X-55 | libellé ; aussi l'ajout au ruban impossible (IMP-0274) |
| `ERR_E_NOM_FICHIER` | EX 53 | NOM DE FICHIER ERRONE | X-56 | libellé |
| `ERR_E_FICHIER_INEXISTANT` | EX 55 | FICHIER INEXISTANT | X-57 | libellé |
| `ERR_E_FICHIER_PROTEGE` | EX 54 | FICHIER PROTEGE PAR UNE AUTRE IDENTIFICATION | CHARGER, GARER, SUPPRIMER sur un fichier protégé (`L`, `!`) d'un autre compte | libellé (MEMENTO82 54 : « Fichier protégé par un n° d'identification # de celui de travail ») ; étape 7d |
| `ERR_E_ENREGISTREMENT_INEXISTANT` | EX 56 | ENREGISTREMENT INEXISTANT | X-58 | libellé (second libellé de 56) |
| `ERR_E_TYPE_CHARGER` | EX 47 | CIBLE DE CHARGER D'UN AUTRE TYPE | X-59 | libellé (second libellé de 47) |
| `ERR_E_COMPTE_RENDU` | EX 56 | COMPTE RENDU NON NUMERIQUE | X-60 | libellé |
| `ERR_E_MODULE_INTROUVABLE` | EX 105 | MODULE INTROUVABLE POUR EXECUTER | X-61 | nouveau (RX7) |

### 9.8 Ressources et erreurs internes

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_E_MEMOIRE_SATUREE` | EX 03 | MEMOIRE SATUREE | X-62 | attesté (INRDP75 l. 3542) |
| `ERR_E_INTERNE` | EX 96 | PROBLEME INTERNE A LSE | X-63 | libellé |

---

## 9 bis. Catalogue des natures d'erreur du dialogue

Domaine `DIALOGUE` (Tome IV §17.1), classe `D`. Toutes ces natures se restituent par la même ligne, `????`, suivie de « : libellé » avec l'option `-LIBELLES` (chapitre Moniteur et console, IMP-0235 ; point M7). Aucune ne change l'état du programme ni le programme courant (VM-2202).

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_D_COMMANDE_ERRONEE` | ???? | COMMANDE ERRONEE | M6, M7 ; ligne vide validée par X-OFF | attesté (Victor Duruy, `???? (CØMMANDE ERRØNEE)`) |
| `ERR_D_PARAMETRE` | ???? | PARAMETRE DE COMMANDE ERRONE | `LISTER A PARTIR DE` sans numéro | attesté (Victor Duruy, `????`) |
| `ERR_D_LIGNE_DEPART_ABSENTE` | ???? | LIGNE DE DEPART INEXISTANTE | X-64, M5 | libellé |
| `ERR_D_CONTINUER_IMPOSSIBLE` | ???? | CONTINUER IMPOSSIBLE | X-65 | constaté (LSE 8086, `???`) |
| `ERR_D_REPRENDRE_IMPOSSIBLE` | ???? | REPRENDRE IMPOSSIBLE | X-66 | libellé |
| `ERR_D_CONSOLE_INEXISTANTE` | ???? | CONSOLE INEXISTANTE | `PRENDRE ETAT CONSOLE n`, n différent de 1 (IMP-0236) | libellé |
| `ERR_D_IDENTIFICATION_ILLEGALE` | ???? | IDENTIFICATION ILLEGALE | `IDENTIFICATION n`, n inconnu | attesté (LSE15 §9-3 : « ILLEGALE = ??? ») ; étape 7d |

---

## 9 ter. Catalogue des natures d'erreur de fichier

Classe `F` (chapitre Bibliothécaire et fichiers, IMP-0250). Numéros de MEMENTO82, annexe III, quand ils existent, à partir de 100 sinon (IMP-0185). Message : `ERREUR F nn`, suivi de ` EN LIGNE lll` ou ` EN LIGNE BUR` pour une erreur survenue pendant l'exécution d'une instruction, puis de ` : libellé` avec l'option `-LIBELLES`. Les codes 01 (« erreur PROLOGUE »), 09 et 28 (commandes ENTREE et SORTIE) de MEMENTO82 n'ont pas de correspondant dans les deux profils retenus et ne sont pas catalogués. La classe `FL` attestée à Victor Duruy n'a pas de nature (son sens n'est pas établi).

| Identifiant | Code | Libellé | Points | Rattachement |
|---|---|---|---|---|
| `ERR_F_SUPPORT_PLEIN` | F 02 | SUPPORT PLEIN | bibliothèque | libellé |
| `ERR_F_PROGRAMME_EXISTANT` | F 03 | PROGRAMME DEJA EXISTANT SUR LE SUPPORT | RANGER, DUPLIQUER ; CATALOGUER et GARER `'#…'` sous MITRA 15 (Bibliothécaire IMP-0251) | attesté (Victor Duruy, `ERREUR F 03`) |
| `ERR_F_DONNEES_EXISTANT` | F 04 | FICHIER DE DONNEES PERMANENT DEJA EXISTANT | CATALOGUER, DUPLIQUER ; RANGER sous MITRA 15 (Bibliothécaire IMP-0251) | libellé |
| `ERR_F_FICHIER_INEXISTANT` | F 05 | FICHIER INEXISTANT | APPELER, SUPPRIMER, CATALOGUER | libellé |
| `ERR_F_PROGRAMME_PROTEGE` | F 06 | FICHIER PROGRAMME PROTEGE PAR UNE IDENTIFICATION | APPELER, LISTER (protections `E`, `L`) | libellé |
| `ERR_F_DROIT_PROGRAMME` | F 07 | DROIT DE CREATION DE PROGRAMME ABSENT | RANGER, MODIFIER | libellé |
| `ERR_F_DROIT_DONNEES` | F 08 | DROIT DE CREATION DE DONNEES PERMANENTES ABSENT | GARER `'#…'`, CATALOGUER | libellé |
| `ERR_F_IDENTIFICATION_INEXISTANTE` | F 10 | IDENTIFICATION INEXISTANTE | SUPPRESSION ID | libellé |
| `ERR_F_DROIT_SECRET` | F 11 | DROIT DE CREATION DE FICHIER SECRET ABSENT | `RANGER ω,E` (T1600) sans l'attribut `S` | libellé (MEMENTO82 11) ; étape 7d |
| `ERR_F_EXTENSION_TEMPORAIRE` | F 13 | EXTENSION TROP IMPORTANTE DE L'ESPACE TEMPORAIRE | espace temporaire | libellé |
| `ERR_F_RACINE_INTERDITE` | F 17 | RACINE * INTERDITE POUR CETTE COMMANDE | `SUPPRIMER *`, DUPLIQUER | libellé |
| `ERR_F_CATALOGUE_SATURE` | F 19 | CATALOGUE SATURE | `CATALOGUE` | libellé |
| `ERR_F_MAUVAISE_IDENTIFICATION` | F 22 | REMPLACEMENT SOUS UNE MAUVAISE IDENTIFICATION | MODIFIER, SUPPRIMER | libellé |
| `ERR_F_MEMOIRE_PROGRAMME` | F 23 | MEMOIRE TROP PETITE POUR LE PROGRAMME | APPELER, EXECUTER (IMP-0246) | libellé |
| `ERR_F_AUTRE_NOM` | F 25 | AUTRE NOM QUE CELUI DU DERNIER APPEL | MODIFIER | libellé |
| `ERR_F_ENTREE_SORTIE` | F 27 | OPERATION D'ENTREE-SORTIE IMPOSSIBLE | toute opération sur disque | libellé |
| `ERR_F_UNITE` | F 29 | NUMERO D'UNITE INCORRECT | `DUPLIQUER` ou `$nom` sans unité amovible (`-UNITE`, IMP-0278) | libellé (MEMENTO82 29) ; étape 7d |
| `ERR_F_AUTRE_PROFIL` | F 100 | FICHIER RANGE SOUS UN AUTRE PROFIL | APPELER, EXECUTER (IMP-0246) | nouveau |
| `ERR_F_FICHIER_ENDOMMAGE` | F 101 | FICHIER ENDOMMAGE | vérification RCI-S, répertoire de fichier-données | nouveau |
| `ERR_F_BIBLIOTHEQUE` | F 102 | BIBLIOTHEQUE INACCESSIBLE | lancement (`-BIB`) | nouveau |

---

## 10. Points ouverts

Points repris de l'inventaire des diagnostics (§10 et §12), à trancher avant de terminer ce chapitre :


---

## 11. Traçabilité

```text
EXI-0019, EXI-0051, EXI-0061
  → Tome IV §3.3, §17 (ALG-1701)
    → IMP-0185 (numérotation), IMP-0186 (forme des messages), IMP-0187 (granularité), IMP-0195 (structure du diagnostic)
      → CODE : src/noyau/erreurs.h (catalogue), src/noyau/erreurs.c,
               src/plateforme/main.c (paramètre -LIBELLES)
        → TESTS : à définir avec le catalogue
```
