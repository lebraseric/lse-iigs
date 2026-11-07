# Relevé des sorties T1600 — Lycée Victor Duruy, 1978-1983

**Source :** `02-HISTORIQUE/Programmes LSE T-1600 Victor Duruy 1978-1983/`. 46 fichiers PDF (66 pages) : numérisations de sorties de télétype et de quelques brouillons manuscrits, produits par Éric Le Bras, élève, sur le système LSE du lycée.
**Machine :** T1600, d'après le nom du dossier. Les sorties ne nomment jamais la machine. La bannière de session imprime `LSE F4`.
**Datation :** la classe portée par chaque programme date sa feuille, de la 6ème (1978-79) à la 2nde (1982-83), Éric ne reprenant pas ses programmes d'une année sur l'autre (§2, Chronologie). Repères explicites : `BØNJØUR 15/10/80 08:36:17` (4EME - MORSE, `MORSE_V2_numerisation0008.pdf`) et `MARS1983` dans un commentaire de programme (2NDE - PA1, `numerisation0019.pdf`). Les autres sessions impriment une date nulle (`00/00/00`) ou pas de date.
**Statut :** attestations directes du comportement d'un système LSE en service, de première main. Le rapport avec la « version 4 du système LSE - T1600 » décrite par INRDP75 n'est pas établi. INRDP75 décrit l'état connu en décembre 1974 et annonce des « additifs » en cas de modifications (INRDP75, l. 79). Les sorties datent de 1980 à 1983, et le sens de `F4` n'est pas connu.

**Conventions :** les messages sont reproduits tels qu'imprimés. La lettre O est imprimée `Ø` par la télétype ; la flèche `←` est rendue `←`. La référence de chaque attestation donne le dossier et le fichier.

---

## 1. Codes d'erreur attestés

| Message imprimé | Contexte (ce qui précède) | Fichier | Correspondance MEMENTO82 |
|---|---|---|---|
| `ERREUR  C 03` | ligne lue `59 AFFICHEP 'L@ BØNNE REPØNSE ETAIT VADUZ.';@LLER EN 61` (faute de frappe `AFFICHEP`, 8 lettres) | 6EME - SPHIN, `numerisation0022.pdf` | C 03 « Identification de plus de 5 caractères » : compatible |
| `ERREUR  C 03` | commande `LI`, réponse `????`, puis texte `TER A PARTIR DE 1` | 5EME - ANGLO, `ANGLO_V2_numerisation0032.pdf` et `ANGLO_V3_numerisation0004.pdf` | C 03 : compatible si `TER` suivi de `A PARTIR…` est lu comme ligne machine de bureau ; lecture incertaine. Feuilles de 5ème sans bannière, antérieures à F4 : non reproduit ; LSE-IIGS répond `ERREUR  C 38` (choix d'Éric du 2026-11-05, point 13) |
| `ERREUR  C 36` | ligne lue `41 SI R3='TEHERAN' ALØRS DEBUT AFFICHER 'EXACT. C"EST EXELLENT.';ALLER ▮`, coupée à la marge (ligne de plus de 80 caractères) | 6EME - SPHIN, `numerisation0022.pdf` (deux occurrences : lignes 41 et 51) | C 36 « Manque ";" ou "FIN" dans une instruction "DEBUT"…"FIN" » : compatible, la coupure supprime le `FIN` |
| `ERREUR  C 63` | aussitôt après le `ERREUR  C 36` précédent, après deux lignes blanches, suivi de `**** LIGNE PRECEDENTE IMPØSSIBLE A DECØMPILER : PLUS DELENT.';ALLER EN ▮` ; ce début fait exactement 55 caractères, et la suite est le texte de la ligne 41, sans son numéro, à partir de la position 55 (le message a vraisemblablement été écrit par-dessus le texte décompilé, dans un même tampon) ; la ligne suivante, lue sur le ruban, perd son premier caractère (`3 SI R3#…` pour `43 SI R3#…`) ; feuille de 6ème (1978-79), antérieure à F4 : non reproduit, LSE-IIGS refusant toute ligne de plus de 79 caractères par `C 63` | 6EME - SPHIN, `numerisation0022.pdf` (deux occurrences) | absent de MEMENTO82 (59 à 88 non listés) |
| `ERREUR  C 38` | commande `PRE` (abréviation incomplète) en début de session | 5EME - ANGLO, `ANGLO_V2_numerisation0032.pdf` | C 38 « Dans une instruction d'affectation "_" absent ou variable réceptrice incorrecte » : compatible (identificateur `PRE` sans `←`, ligne machine de bureau) |
| `ERREUR  C 38` | ligne lue `33 SI R1='"S' ALØRS DEBUT ALHER EN 37;I←I+1 FIN` (faute de frappe `ALHER`) | 5EME - ANGLO, `ANGLO_V3_numerisation0004.pdf` | C 38 : compatible (`ALHER` lu comme identificateur, sans `←`) |
| `ERREUR F 03` | commande `DUPLIQUER $*,P` (trois fois de suite ; puis une fois dans un autre fichier) | 5EME - ANGLO, `ANGLO_V4_numerisation0030.pdf` ; 6EME - SPHIN, `numerisation0003.pdf` | F 03 « Programme déjà existant sur le support » : compatible |
| `ERREUR FL 06` | commande `DUPLIQUER $*,D` | 5EME - ANGLO, `ANGLO_V4_numerisation0030.pdf` | classe `FL` absente de MEMENTO82 ; F 06 y est « Fichier programme protégé par une identification ». Feuille de 5ème sans bannière, antérieure à F4 : non reproduit (choix d'Éric du 2026-11-05, point 14) |

**Forme du message :** `ERREUR`, deux espaces, la lettre de classe, une espace, le numéro sur deux chiffres (`ERREUR  C 03`). Pour les classes F et FL, une seule espace sépare `ERREUR` de la classe (`ERREUR F 03`, `ERREUR FL 06`). Le numéro de ligne n'est pas rappelé : le message suit immédiatement l'écho de la ligne rejetée.

**Lignes rejetées dans un listing :** quand le programme est lu depuis un ruban, l'écho de chaque ligne est imprimé avant son analyse. Une ligne rejetée reste donc visible dans la sortie, suivie de son message d'erreur (`ANGLO_V3_numerisation0004.pdf`, ligne 33).

## 2. Messages de commande et de dialogue

| Message imprimé | Contexte | Fichier |
|---|---|---|
| `  ???? (CØMMANDE ERRØNEE)` (deux espaces de tête) | commande `BØ` | 2NDE - PA1, `numerisation0019.pdf` (mars 1983) |
| `????` | commandes `BØ` (en session, sans complétion en `BØNJØUR`), `PR`, `AP`, `LI` ; commande incomplète `LISTER A PARTIR DE` sans numéro | 6EME - SPHIN, `numerisation0014.pdf`, `numerisation0033.pdf` ; 5EME - ANGLO, `ANGLO_V2`, `V3`, `V4` |
| `PRET` | attente d'une commande | nombreux fichiers |
| `PRET EN LIGNE 036` | exécution interrompue pendant une lecture (`ØU JØUEZ-VØUS ?` laissé sans réponse) | 4EME - GA, `GA_V1_EXEC_numerisation0044.pdf` |
| aucun message après `PRENDRE ETAT CØNSØLE 3`, `6`, `7`, `8` : la commande suivante (`LISTER`, `PERFØRER`) vient aussitôt | reprise du programme d'une autre console, depuis la télétype (console 1) | 5EME - ANGLO, `ANGLO_V4` ; 6EME - SPHIN, `numerisation0003.pdf`, `0014`, `0033` |
| ligne vide, puis `PRET` | après `PRENDRE ETAT CØNSØLE 1` ; sens incertain | 5EME - ANGLO, `ANGLO_V2_numerisation0032.pdf` |
| `PAUSE EN LIGNE 161`, reprise par `CØNTINUER` | instruction `PAUSE` | 5EME - ANGLO, `ANGLO_V9_EXEC_numerisation0039.pdf` |
| `TERMINE EN LIGNE 222` (et 029, 052, 055, 061, 090, 177) | fin d'exécution par `TERMINER` | nombreux fichiers |
| `**** LIGNE PRECEDENTE IMPØSSIBLE A DECØMPILER : …` | après `ERREUR C 63`, ligne de plus de 80 caractères lue depuis le ruban | 6EME - SPHIN, `numerisation0022.pdf` |
| `REDEMARRAGE SYSTEME` / `LSE F4` / `DATE HEURES MINUTES : 00/00/00 00:00:00` / `EN LIGNE....` / `BØNJØUR  00:05:00` et la bannière | redémarrage du système ; `LSE F4` et `DATE HEURES MINUTES :` paraissent trois fois, la réponse vide une fois | 4EME - PRO, `Fichier de données FICH (incomplet) … numerisation0029.pdf` |
| `BØNJØUR 15/10/80 08:36:17` / `CØNSØLE NØ. 01.` / `LSE F4` / `RUB-ØUT REMPLACE CTRL X-ØFF` / `VØUS ETES SØUS ID 1` | ouverture de session | 4EME - MORSE, `MORSE_V2_numerisation0008.pdf` ; aussi 5EME - MULT, `numerisation0005.pdf` |
| `AU REVØIR 15/10/80 08:38:36`, puis les quatre mêmes lignes | fin de session : `AU REVØIR 15/10/80 08:25:26` et `08:38:36` (MØRSE) ; `BØNJØUR 00000000 01:39:11` et `AU REVØIR 00000000 01:44:20` (MULT, date non réglée) | idem |
| cinq lignes blanches | entre `VØUS ETES SØUS ID 1` et la commande ou le `BØNJØUR` qui suit : six interlignes mesurés sur `MORSE_V2`, MULT et, après les deux bannières, 5EME - NBOR, `numerisation0024.pdf`. Éric, auteur de ces sorties, n'a souvenir d'aucun geste qui l'expliquerait : elles font partie de la bannière. Sur `MORSE_V2`, quatorze interlignes après `AU REVØIR 15/10/80 08:25:26` | idem |

Commandes attestées : `APPELER`, `LISTER A PARTIR DE n`, `EXECUTER A PARTIR DE n`, `PERFØRER A PARTIR DE n`, `IDENTIFICATIØN`, `DUPLIQUER $nom,P|D`, `PRENDRE ETAT CØNSØLE n`, `RANGER $nom` (`RANGER $SPHIN`, précédé d'`IDENTIFICATIØN`, 6EME - SPHIN, `numerisation0022.pdf` ; au dos, note manuscrite d'Éric : « ranger sur diskette »), `APPELER $nom` (`APPELER $SINUS`, 4EME - SINUS, `numerisation0043.pdf`), `MØDIFIER nom`, `CØNTINUER`. Le caractère `\` annule le caractère précédent à la saisie (`PRE\ENDRE`, `DIDI\`, `51S\`), comme le décrit LSE15 §1.

### Formes de la date

Relevées le 2026-11-01 par lecture optique des 46 numérisations (moteur Vision de macOS), puis contrôlées à l'œil pour les feuilles marquées ✓. Les espaces des autres lignes restent à vérifier.

| Forme | Bannière | Feuilles |
|---|---|---|
| `BØNJØUR 15/10/80 08:36:17`, `AU REVØIR 15/10/80 08:25:26`, `08:38:36` | oui | 4EME - MORSE, `MORSE_V2_numerisation0008.pdf` ✓ |
| `BØNJØUR 00000000 01:39:11`, `AU REVØIR 00000000 01:44:20` | oui | 5EME - MULT, `numerisation0005.pdf` ✓ |
| `AU REVØIR 00000000 28:12:01`, `BØNJØUR 00000000 28:26:42` | oui | 5EME - NBOR, `numerisation0024.pdf` ✓ |
| `BØNJØUR  00:05:00` | oui | 4EME - PRO, `FICH (incomplet) … numerisation0029.pdf` ✓, cinq minutes après `DATE HEURES MINUTES : 00/00/00 00:00:00` |
| `BØNJØUR  00/00/00 19:19:44` | non | 5EME - ANGLO, `ANGLO_V6A_numerisation0031.pdf` ✓ ; même forme `00:28:48` (V7), `36:11:12` (V8A), `21:23:21` (CARTO, `numerisation0028.pdf`), `21:40:24` et `AU REVØIR … 21:42:10` (PUISS) |
| `BØNJØUR  000000 17:04:16` | non | 5EME - CARTO, `CARTO_V1_numerisation0009.pdf` ✓ |
| `BØNJØUR   10:47:16` | non | 6EME - SPHIN, `numerisation0014.pdf` ✓ ; `AU REVØIR   32:42:35`, `BØNJØUR   32:47:26` (ANGLO V3 ✓) ; `21:53:06`, `21:53:14` (ANGLO V2) |

Lecture : avec la bannière, la forme est `BØNJØUR`, une espace, le champ date, une espace, l'heure, le champ date pouvant être vide (`BØNJØUR  00:05:00`). L'heure compte depuis le redémarrage et ne repasse pas à zéro à 24 h quand la date n'est pas réglée (`28:26:42`, `32:47:26`, `36:11:12`). Le rapport entre `00000000` et le champ vide n'est pas établi : aucune feuille ne montre comment on obtenait `00000000`, et Éric n'en a pas le souvenir.

Chronologie : Éric ne reprenait pas ses programmes d'une année sur l'autre ; la classe portée par chaque programme date donc sa feuille : 6ème (`6.8`) 1978-79, 5ème (`5.8`) 1979-80, 4ème (`4.2`) 1980-81, 3ème 1981-82, 2nde 1982-83. Le T1600 a ensuite été retiré et remplacé par des micro-ordinateurs MICRAL (Éric, 2026-11-04). La bannière `LSE F4` paraît en 5ème (MULT, NBOR) et en 4ème (MORSE), mais pas sur les feuilles de 6ème (SPHIN) ni sur toutes celles de 5ème (ANGLO, CARTO, PUISS) : la version F4 a donc été installée au cours de l'année 1979-80. ANGLO date de l'année scolaire 1979-80 (Éric). D'après les messages et cette date, les formes sans bannière viennent d'une version antérieure à la version F4 attestée plus tard ; elles ne sont pas reproduites. C'est aussi le cas de `B` suivi de `PRET` avant `BØNJØUR` (ANGLO V3, `numerisation0004`) : une saisie annulée avant `BONJOUR` n'imprime rien sous `LSE F4`.

## 3. Constructions de langage à confronter aux tomes

1. **`TEM()` avec parenthèses** : `104 Q←TEM()` et `105 FAIRE 105 TANT QUE TEM()-Q<4` (4EME - SINUS, `numerisation0035.pdf` et `numerisation0043.pdf`). INRDP75 (l. 850) dit pourtant : « Sur T1600 la fonction TEM est sans parenthésage », et le profil T1600 version 4 du Compilateur s'en tient à ce texte. Ces listings sont des sorties de `PERFØRER` : le décompilateur de la machine restituait donc `TEM()`. Le profil `LSE F4` admet cette forme et la restitue (IMP-0189). `TEM` sans parenthèses n'apparaît sur aucune sortie transcrite, et `DAT` sur aucune ; `LSE F4` lui donne le comportement de `TEM`.
2. **Équivalents numériques dans les constantes-chaîne et les libellés de format**, notation `.n.` ou `.n n.` :
   - `102 AFFICHER .27 12.` (4EME - SINUS) ;
   - des morceaux de chaîne accolés à un équivalent numérique sans opérateur `!` : `11 AFFICHER '...APRECIAY'.8.'TIØN.(DANS CERTAINE'`, `76 AFFICHER '...DE "A'.8.'TØ HAVE"?='` (5EME - ANGLO, `ANGLO_V5_numerisation0026.pdf`) ;
   - dans un libellé de format : `50 AFFICHER[/,' A'.7.'U'.7.' '.7.'R'.7.'E'.7.'V'.7.'Ø'.7.'I'.7.'R'.7 7 7.]` (5EME - MULT, `numerisation0015.pdf`, sortie de `PERFØRER A PARTIR DE 1`, fin de ligne en surimpression à la colonne 72).

   La notation n'apparaît ni dans INRDP75 ni dans LSE15 ; MEMENTO82 C 08 la mentionne (« caractère de fin d'équivalent numérique »). Elle n'est pas couverte par la grammaire reconstruite ; le profil `LSE F4` l'admet (Compilateur §8 bis, IMP-0190).
3. **`FAIRE` sans variable contrôlée** : `105 FAIRE 105 TANT QUE TEM()-Q<4` (4EME - SINUS, `numerisation0035.pdf` et `numerisation0043.pdf`), attente de quatre secondes. INRDP75 §4-2 et LSE15 §5-5.1 exigent `POUR` ; MEMENTO82, LAFOND83 et NOYELLE79, d'après Éric, le rendent facultatif avec `TANT QUE`. Le profil `LSE F4` l'admet (RCI-M IMP-0277).
4. **Apostrophe dans une constante-chaîne** : écrite doublée (`D''UNE`, 2NDE - PA1, 1983), conformément à INRDP75. Les programmes antérieurs écrivent `"` à la place de l'apostrophe typographique (`D"ANGLAIS`, `QU"EST`), ce qui n'est qu'un autre caractère.
5. **Déclaration suivie d'un appel sur la même ligne** : `100 PRØCEDURE &AFTEX();&NEN()` (4EME - PRO, `Listing PRO - numerisation0046.pdf`), conforme à LSE15 §7-6.
6. **Ligne vide** comme fin de boucle (`121`, `122`, `18`, `29`…), conforme à INRDP75.
7. **Chaîne de plus de 255 caractères** : `FICH2`, enregistrement 8 (4EME - PRO, `Fichier de données FICH2 - Sortie du programme PERFO - numerisation0011.pdf`) : type chaîne, quatre lignes de 68, 70, 67 et 66 caractères, soit 274 avec leurs espaces, 271 sans. INRDP75 (l. 817) limite les chaînes à 255 caractères sur T1600 ; la borne de `LSE F4` n'est pas connue. Voir Données IMP-0209 et `Reconstitution-PERFO-CHARG/README.md`.
8. **Réels de 23 bits** : avec la réponse 7, `NBOR` (5EME - NBOR, `numerisation0024.pdf`) imprime quatorze valeurs, de `3.645752` à `2.618034`, puis `1.618034` et `14 ØPERATIØNS`. Des réels de 24 bits (IEEE) donnent 16 opérations ; 23 bits significatifs, la demie arrondie en s'éloignant de zéro, en donnent 14 et reproduisent 13 des 14 valeurs (`3.645751` au lieu de `3.645752`) ; les mêmes réels reproduisent 12 des 16 témoins d'INRDP75 sur `+ − × ÷` et puissances entières. Réalisé sous `LSE F4` et `-T1600` (Interpréteur IMP-0280 ; `05-IMPLEMENTATION/audits/Analyse_reels_T1600_2026-11-05.md`). Confirmé par le dossier dactylographié de PRO et PRON2 (`4EME - PRO/Exécution PRO-PRON2.pdf`) : note `9.565216 /20` = `11/(23/20)`, que donnent les réels de 23 bits (24 bits : `9.565218`).
