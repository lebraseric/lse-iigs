# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — RCI-M : représentation compilée interne en mémoire

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V (spécification détaillée complète de l'implémentation), niveau `05-IMPLEMENTATION/decisions/`
**Portée :** RCI-M uniquement (représentation en mémoire vive). RCI-S (sérialisation) et l'état d'exécution (pile d'évaluation, cadres d'activation) font l'objet de chapitres distincts du Tome V.
**Rang normatif :** subordonné à ANN-K (Tome IV), au chapitre 6 du Tome III (VM-2401), à VM-1402 (Tomes I-II §14.2) et au Catalogue des exigences. Aucune disposition du présent chapitre ne peut contredire ces sources.
**Place dans le Tome V :** c'est le Tome V dans son ensemble qui constitue le dernier tome de la collection, celui qui doit permettre de réaliser directement l'implémentation en ORCA/C. Le présent chapitre en fait partie ; sa position exacte dans le sommaire du Tome V (avant ou après les chapitres consacrés à l'état d'exécution, à l'interpréteur ou au gestionnaire d'objets) reste à définir et n'est pas fixée par ce document.

---

## 0. Avertissement de statut

Le Tome IV (§5.7, ANN-K-0044) interdit explicitement de présenter un format physique d'élément, une taille de code, une organisation binaire ou un octet de contrôle comme un **fait historique**. Les tailles, codages et structures C décrits ici sont donc systématiquement des **décisions d'implémentation (`IMP-*`)**, jamais des reconstructions historiques. Aucun énoncé du présent document ne doit être lu comme une affirmation sur le format binaire réel du L.S.E. de 1975/1978.

---

## 1. Rappel des contrats obligatoires (non renégociables)

### 1.1 Exigences EXI directement concernées

| EXI | Objet | Conséquence physique imposée |
|---|---|---|
| EXI-0001 | Compilation indépendante des lignes | Le compilateur ne dispose, en compilant une ligne donnée, d'aucune information provenant d'une autre ligne ; aucune représentation physique ne peut donc dépendre d'un contexte inter-lignes établi au moment de la compilation |
| EXI-0002 | Représentation compilée exécutable | Chaque ligne acceptée a une séquence physique exécutable, distincte du texte source |
| EXI-0003 | Décompilabilité | La séquence physique doit porter — directement ou par table associée — l'information nécessaire à la reconstruction textuelle |
| EXI-0010 | Représentation commune exécution/décompilation | Une seule séquence physique par ligne ; pas de double structure exécution + décompilation |
| EXI-0011 | Ordre post-fixé | Les opérandes précèdent physiquement l'opération qui les consomme |
| EXI-0012 | Évaluation séquentielle | L'interpréteur doit pouvoir parcourir la séquence sans reconstruire le source |
| EXI-0013 | Opérations identifiables | Chaque opération codée doit être physiquement discriminable des valeurs/références |
| EXI-0014 | Données d'exécution | La séquence doit porter valeurs immédiates et références résolubles |
| EXI-0015 | Restitution textuelle déterministe | Le décompilateur doit produire un résultat déterminé à partir de la seule séquence physique |
| EXI-0016 | Compacité | Aucune information syntaxique reconstructible n'est dupliquée physiquement |
| EXI-0028 | Enchaînement immédiat compilation-exécution (mode machine de bureau) | Un objet compilé produit en mode machine de bureau n'est pas nécessairement conservé dans le programme courant ; il est transmis à l'interpréteur dès sa validation |
| EXI-0046 | Contrôle explicite du flux d'exécution | La représentation compilée doit pouvoir modifier la position logique d'exécution ; la destination est résolue par l'interpréteur, indépendamment de toute adresse mémoire |

### 1.2 Invariants ANN-K-0032 applicables à RCI-M

Sont repris ci-dessous, avec leur numéro dans ANN-K-0032, les invariants qui portent sur RCI-M ; les autres concernent RCI-S.

- (1) RCI-M ne dépend d'aucune adresse mémoire.
- (3) RCI-M est interprétable sans le texte source.
- (4) RCI-M reste décompilable selon les capacités définies.
- (6) Toute référence est résoluble dans son contexte.
- (7) Les tables sont cohérentes avec les références.
- (9) Une information reconstructible n'est pas dupliquée sans justification.

### 1.3 Responsabilités VM-2401 (Tome III, §6.1), VM-1402 (Tomes I-II §14.2) et VM-1502 (Tome III §18.3)

La séquence compilée doit : préserver l'ordre logique, permettre le parcours séquentiel, permettre la décompilation, permettre l'identification des transferts de contrôle, représenter valeurs immédiates, références et opérations codées — sans imposer de format binaire particulier (Tome III §6.9).

VM-1402 précise qu'une instruction de contrôle est représentée par une opération codée qui modifie la position d'exécution abstraite, qu'une destination de branchement est représentée logiquement (indépendamment d'une adresse mémoire), et que **c'est l'interpréteur qui résout cette destination à l'exécution**. Cette précision est structurante pour le §4.7 : elle autorise explicitement une destination dont la valeur n'est connue qu'à l'exécution.

VM-1502 (Formateur des sorties, Tome III §18.3) précise que le format d'une instruction AFFICHER est reconnu à partir d'une « description de format compilée ou structurée », et non ré-analysée comme texte à chaque exécution. Cette précision est structurante pour le §4.10 : elle impose que le format soit décomposé en éléments physiques dès la compilation.

### 1.4 Interdictions du conditionnement (§9, §19)

Les pointeurs C ne constituent pas un modèle architectural. Toute référence **inter-objets** (variable, tableau, procédure, ligne cible) doit passer par un identifiant logique indépendant de son emplacement mémoire courant, afin de survivre à une compaction ou à un déplacement mémoire du gestionnaire d'objets.

---

## 2. Contraintes de plateforme retenues (ORCA/C 2.2.1 / 65816)

| Type C | Taille | Remarque |
|---|---|---|
| `char` | 1 octet | |
| `short`, `int` | 2 octets | identiques sous ORCA/C |
| `long` | 4 octets | |
| pointeur (`*`) | 4 octets | coût élevé — à limiter |
| `float` | 4 octets | format SANE |
| `double` | 8 octets | format SANE |
| `extended`/`long double` | 10 octets | format SANE, coûteux |
| `comp` | 8 octets | entier signé SANE |
| `_Bool` | 2 octets | |

Points structurants :

- ORCA/C n'impose **aucun alignement de structure** (`alignment` = 1 pour tout type) : les `struct` C ne subissent pas de bourrage (*padding*). Ceci autorise un encodage compact champ-à-champ sans perte liée à l'alignement — propriété favorable à EXI-0016.
- Le modèle mémoire par défaut segmente le code et les données globales par blocs de 64 Kio (`segment`, banque `$10000`). Une structure applicative ne doit donc pas supposer un espace contigu global illimité ; les tables de grande taille (table des objets compilés, table des symboles) doivent être allouées dynamiquement plutôt que déclarées comme tableaux globaux fixes proches de la limite de segment.
- Un paramètre de fonction de type `float` est transmis et conservé au format SANE *extended* (10 octets) : l'adresse d'un tel paramètre ne désigne pas un `float` IEEE de 4 octets (constaté le 2026-10-22, `tests/representation/types_plateforme.c`). Pour ranger un réel dans une séquence (sous-code 0x02, IMP-0184), la valeur est d'abord copiée dans une variable locale `float`.
- Aucune contrainte d'alignement bit-à-bit n'existe pour les champs de bits (`bit-field`), ce qui permet, si nécessaire, un encodage sur moins d'un octet pour les catégories à faible cardinalité — non retenu ici par simplicité (cf. IMP-0101).

---

## 3. Architecture physique générale de RCI-M

```text
PROGRAMME COURANT (mémoire)
│
├── TABLE DES OBJETS COMPILÉS         (une entrée par ligne, indexée par numéro de ligne)
│      entrée[i] : { numéro_ligne, longueur, handle_bloc }
│
├── ZONE DES SÉQUENCES COMPILÉES      (blocs d'octets, un par ligne, gérés par le
│                                       gestionnaire d'objets — module distinct,
│                                       cf. §7 du conditionnement et note ci-dessous)
│      bloc : suite d'ÉLÉMENTS RCI-M (tag + charge utile)
│
├── TABLE DES SYMBOLES                (variables, tableaux, procédures, fonctions —
│                                       identifiants compacts réutilisables)
│
└── TABLE DES CONSTANTES PARTAGÉES    (constantes numériques/chaînes réutilisées)
```

**Note de dépendance.** Le présent chapitre suppose l'existence d'un gestionnaire d'objets (responsabilité « gestionnaire mémoire » du Tome III, §7 du conditionnement) capable d'allouer, de libérer et, le cas échéant, de déplacer par compaction les blocs de la zone des séquences compilées, en exposant ce service par un type opaque `HandleObjet` (§5). Les décisions physiques propres à ce gestionnaire (stratégie d'allocation, algorithme de compaction, format de son en-tête de bloc) ne sont pas prises par le présent chapitre : elles relèvent d'un chapitre dédié du Tome V, non encore rédigé à ce stade. Le présent chapitre se limite à définir le contrat que ce gestionnaire doit satisfaire pour RCI-M : allocation d'un bloc par ligne (IMP-0101), stabilité de l'identifiant `HandleObjet` à travers une compaction, résolution temporaire en adresse physique sans exposition au-delà de l'appel courant (§3, §9).

Séparation stricte imposée :

- La **table des objets compilés** ne connaît que des numéros de ligne et des identifiants de bloc (`handle`), jamais d'adresse physique persistante à travers un déplacement mémoire.
- La **zone des séquences compilées** est le seul endroit où l'exécution/décompilation opère un parcours séquentiel physique par avancement d'octet — parcours qui, lui, peut légitimement utiliser un pointeur C local et volatile (technique de réalisation, cf. §9 du conditionnement), car il ne traverse jamais la frontière d'un objet.
- Toute traversée **inter-objets** (référencer une variable, sauter vers une autre ligne, appeler une procédure) passe obligatoirement par un identifiant de la table des symboles ou par un numéro de ligne logique — jamais par un pointeur.

**IMP-0101** — *Choix de granularité de représentation physique*
- Sujet : unité physique de stockage de la séquence compilée.
- Choix : un bloc d'octets contigu par ligne, alloué/libéré par le gestionnaire d'objets, référencé par un handle 16 bits.
- Alternatives envisagées : (a) une unique zone contiguë pour tout le programme avec index d'offsets ; (b) bit-fields densifiés.
- Justification : (a) complique le remplacement d'une ligne unique (EXI-0006, unicité et remplacement du numéro de ligne : « une nouvelle ligne portant le même numéro remplace la précédente ») car il faudrait décaler tout le reste du programme à chaque modification ; le découpage par ligne rend le remplacement local, une seule entrée de la table des objets compilés étant réaffectée à un nouveau bloc. (b) gagne peu de mémoire pour un coût de lisibilité/maintenance disproportionné sur cible 65816.
- Impact mémoire : +4 octets d'en-tête de bloc par ligne (à la charge du gestionnaire d'objets, cf. note de dépendance ci-dessus).
- Impact disque : sans objet (RCI-M uniquement).
- Impact performances : accès direct O(1) à une ligne via la table des objets compilés.
- Impact historique : neutre (le format physique n'est pas fixé historiquement, §5.7 Tome IV).
- Références normatives : EXI-0006, EXI-0016, ANN-K-0004, VM-2401.
- Statut : retenu.

---

## 3 bis. Mode machine de bureau et objets compilés transitoires (EXI-0028)

Le mode machine de bureau (Tome IV §16.2 ; Tomes I-II, HIS-4.7.2) fait exception au cycle normal « compiler → conserver dans le programme courant → exécuter sur demande » : une ligne saisie sans numéro (ou précédée du symbole « ? » selon les sources) est **compilée puis immédiatement transmise à l'interpréteur**, sans jamais devenir une entrée persistante du programme courant (§16.3 par contraste : le mode programmé conserve les objets compilés et attend la commande EXECUTER). Ce point conditionne directement la gestion physique de la table des objets compilés (§5) et de la table des symboles (§6).

**IMP-0109** — *Objets compilés transitoires du mode machine de bureau*
- Sujet : faut-il enregistrer, dans la table des objets compilés du programme courant (§5), l'objet produit par une ligne exécutée en mode machine de bureau ?
- Choix : non. L'objet compilé produit en mode machine de bureau est construit dans un **bloc transitoire isolé**, hors de la table des objets compilés indexée par numéro de ligne, transmis directement à l'interpréteur, puis libéré après exécution (ou après restitution du résultat, en cas d'utilisation en expression, cf. §2 Tome IV et LSE15 §9-5 pour la consultation/modification de variables). Il ne reçoit pas de `numero_ligne` au sens d'EXI-0002 puisqu'il n'appartient à aucune ligne de programme numérotée.
- Alternatives envisagées : enregistrer l'objet transitoire sous un numéro de ligne conventionnel réservé (ex. 0 ou une plage hors du domaine utilisateur) ; conserver systématiquement l'historique des expressions machine de bureau.
- Justification : EXI-0028 dissocie explicitement le traitement des lignes de programme numérotées du traitement des instructions exécutées en mode machine de bureau (« Observations » d'EXI-0028). Faire apparaître ces objets dans la table des objets compilés introduirait une confusion entre les deux régimes et complexifierait inutilement LISTER, la décompilation (EXI-0003) et la future sérialisation RCI-S, qui ne doivent porter que sur le programme courant réellement conservé.
- Impact mémoire : favorable — aucune accumulation d'objets transitoires au fil d'une session interactive.
- Impact disque : sans objet.
- Impact performances : neutre à favorable (pas de recherche dichotomique inutile dans la table des objets compilés pour une exécution ponctuelle).
- Impact historique : conforme à EXI-0028 et à la distinction Tome IV §16.2/§16.3.
- Références normatives : EXI-0028, EXI-0002 (a contrario), Tome IV §16.2, §16.3, Tomes I-II, HIS-4.7.2.
- Statut : retenu.
- **Conséquence sur la table des symboles :** une ligne machine de bureau peut néanmoins créer ou référencer des symboles (consultation/modification de variables du programme courant, LSE15 §9-5). Ces symboles, lorsqu'ils sont nouveaux (variable définie uniquement en machine de bureau, jamais utilisée par une ligne du programme courant), doivent rester distingués dans la table des symboles — cf. §6 et IMP-0104.

---

## 4. Format physique d'un ÉLÉMENT RCI-M

### 4.1 Principe d'encodage

**IMP-0102** — *Encodage des éléments de la séquence compilée*
- Sujet : format physique d'un élément (ANN-K-0006, IV-FMT-0015).
- Choix : chaque élément commence par un **octet d'étiquette** (`tag`), suivi d'une charge utile de longueur variable selon la catégorie et le sous-type. Aucun élément n'a de taille fixe imposée globalement ; chaque catégorie fixe sa propre règle de longueur.
- Alternatives envisagées : structure C à taille fixe (union discriminée de taille maximale, ex. 8 octets pour tous les éléments) ; encodage par bit-fields.
- Justification : une union à taille fixe gaspillerait de la mémoire pour les opérations codées (qui n'ont besoin que d'un octet) — contraire à EXI-0016 et au principe de compacité ANN-K-0015/ANN-K-0039. L'encodage variable respecte le principe historique documenté (chaîne compacte, Tome IV §5.6) sans revendiquer d'exactitude historique.
- Impact mémoire : réduction significative par rapport à un encodage à taille fixe, notamment pour les opérations codées (1 octet contre 8).
- Impact disque : sans objet ici (concerne RCI-S).
- Impact performances : léger surcoût de décodage (lecture du tag avant de connaître la longueur) ; acceptable sur 65816 car le parcours reste sur un octet de tête.
- Impact historique : conforme à l'esprit documenté d'une chaîne codée compacte (Tome IV §5.6), sans validation historique du détail binaire.
- Références normatives : ANN-K-0006, ANN-K-0015, EXI-0016, VM-2401 (§6.1, §6.9).
- Statut : retenu.

**IMP-0184** — *Ordre des octets : octet faible en tête pour tout champ de plusieurs octets*
- Sujet : dans quel ordre sont rangés les octets d'un champ de charge utile de plus d'un octet — entier 16 bits (§4.4, 0x01), réel `float` (0x02), identifiant de symbole (§4.5, IMP-0104), index de la table des constantes partagées (0x03 en mode index, IMP-0128) ?
- Choix : **octet de poids faible en tête** (*little-endian*) pour tous ces champs, sans exception. Un `float` est rangé tel qu'ORCA/C le range en mémoire : format IEEE simple précision, octet de poids faible en tête.
- Alternatives envisagées : octet de poids fort en tête (rejeté : contraire à l'ordre natif de la machine, il imposerait de permuter les octets à chaque écriture par le compilateur et à chaque lecture par l'interpréteur et le décompilateur, sur l'opération la plus fréquente, la lecture d'une valeur ou d'une référence) ; ordre différent selon le type de champ (rejeté : aucune raison, et source d'erreurs de codage).
- Justification : c'est l'ordre natif du 65816 et d'ORCA/C. Le manuel ORCA/C 2.2 l'énonce pour les entiers — « All of the integers that occupy more than one byte of storage are stored with the least significant byte first, proceeding to the most significant byte » — et pour les réels — « Float numbers in ORCA/C are stored in the IEEE floating point number format, with the least significant byte first. They require four bytes of storage » —, et le qualifie de « normal order for bytes on the Apple IIGS ». Un champ de la séquence compilée a ainsi exactement la représentation de la variable C correspondante (`unsigned int`, `int`, `float`) et peut être copié sans transformation.
- Impact mémoire : nul.
- Impact disque : sans objet (RCI-M) ; l'ordre des octets de RCI-S est fixé par son propre chapitre.
- Impact performances : aucune permutation d'octets à l'écriture ni à la lecture.
- Impact historique : neutre — choix de représentation physique pur.
- Références normatives : manuel ORCA/C 2.2 (représentation des entiers et des réels) ; IMP-0102, IMP-0103, IMP-0104, IMP-0128 ; conditionnement §9.
- Statut : retenu.

### 4.2 Structure du tag

```text
bit 7  bit 6 | bit 5 bit 4 bit 3 bit 2 bit 1 bit 0
[ CATÉGORIE ]|[            SOUS-CODE            ]
```

| Bits 7-6 | Catégorie | Correspond à (ANN-K-0006 / VM-2401) |
|---|---|---|
| `00` | Valeur immédiate | « valeur » |
| `01` | Référence | « référence » |
| `10` | Opération codée | « opération codée » |
| `11` | Information de contrôle | « information de contrôle » (et, par extension retenue en §4.9, « informations nécessaires à la décompilation » — cinquième composante logique explicitement énumérée par VM-2401, DONNÉES MANIPULÉES) |

64 sous-codes (0–63) sont disponibles par catégorie.

### 4.3 Type C de représentation en mémoire vive

```c
typedef unsigned char TagRCIM;   /* 1 octet : catégorie (2 bits) + sous-code (6 bits) */

#define RCIM_CAT_VALEUR      0x00
#define RCIM_CAT_REFERENCE   0x40
#define RCIM_CAT_OPERATION   0x80
#define RCIM_CAT_CONTROLE    0xC0
#define RCIM_CAT_MASQUE      0xC0
#define RCIM_SOUSCODE_MASQUE 0x3F
```

La charge utile n'est **pas** représentée par une `union` C de taille fixe, mais lue de manière procédurale par le gestionnaire de séquence, qui connaît la longueur de charge utile associée à chaque sous-code. C'est cohérent avec ANN-K-0008 (le parcours logique n'impose ni taille fixe d'élément, ni structure C particulière).

### 4.4 Sous-format : Valeur immédiate (`RCIM_CAT_VALEUR`)

| Sous-code | Signification | Charge utile physique | Notes |
|---|---|---|---|
| 0x00 | Entier court | 1 octet signé (-128..127) | Utilisé pour toute valeur hors de la plage [−28, 28] couverte par 0x07–0x3F ci-dessous, conformément au principe du plus petit sous-format capable de représenter la valeur exactement (IMP-0103) |
| 0x01 | Entier | 2 octets signés (complément à deux, octet faible en tête, IMP-0184) | |
| 0x02 | Nombre réel | 4 octets, `float` ORCA/C (IEEE simple précision, octet faible en tête, IMP-0184) | Sous les profils `LSE F4` et `-T1600`, la valeur a 23 bits significatifs : le dernier bit de la mantisse est nul (Interpréteur IMP-0280) ; le format est le même |
| 0x03 | Chaîne littérale, en ligne ou partagée | 1 octet **mode** (bit 7 = indicateur : 0 = texte en ligne, 1 = index de table ; bits 6-0 = longueur `n` du texte, 0–127, si mode en ligne, ignorés si mode index) ; puis, en mode en ligne, `n` octets de caractères, ou, en mode index, 2 octets d'index dans la table des constantes partagées (§7) | Voir IMP-0128 pour la justification de cette fusion |
| 0x04 / 0x05 | Valeur logique FAUX / VRAI | 0 octet — portée par le sous-code | **non produits par le compilateur** : le corpus n'atteste aucune constante logique et la grammaire reconstruite n'en a pas ; les valeurs logiques ne naissent que des opérations relationnelles et logiques (§4.11.3). Sous-codes conservés, comme `NON` (0x18), pour ne rien supprimer silencieusement |
| 0x06 | Constante numérique marquée « numéro de ligne » (symbole `@`) | 1 octet numéro de ligne (0–255) | **Mitra 15 uniquement** [INRDP75, Annexe III §3] : distincte d'une constante numérique ordinaire (0x00/0x01) bien que de même valeur — la marque `@` sur `I←@50` signale au système que la variable affectée représentera un numéro de ligne (ex. pour un usage ultérieur dans `ALLER EN I`), ce qui est signalé par le corpus comme utile en cas de renumérotation (cf. IMP-0119) ; absente sur T1600, où `@` a la signification distincte donnée en §4.5, sous-code 0x02 |
| 0x07–0x3F | Entier immédiat sans charge utile | 0 octet — la valeur est portée intégralement par le sous-code : `valeur = sous-code − 0x23` | Couvre exactement l'intervalle [−28, +28] (57 valeurs) ; sous-code 0x23 = valeur 0. Choisi par le compilateur en priorité sur 0x00 pour toute valeur entière dans cet intervalle (IMP-0123). Réserve entièrement consommée par cette extension : plus aucun sous-code disponible dans `RCIM_CAT_VALEUR`. |

**IMP-0123** — *Entier immédiat porté entièrement par le sous-code, sans charge utile*
- Sujet : peut-on représenter les constantes entières les plus fréquentes (petites valeurs, positives ou négatives) sans aucun octet de charge utile, en exploitant les sous-codes encore réservés de `RCIM_CAT_VALEUR` ?
- Choix : les 57 sous-codes 0x07–0x3F, jusqu'ici réservés, codent chacun directement une valeur entière de l'intervalle [−28, +28] par la relation `valeur = sous-code − 0x23`, sans aucune charge utile — sur le même principe déjà appliqué à 0x04/0x05 (FAUX/VRAI, valeur portée par le seul sous-code). Le compilateur choisit ce sous-format en priorité sur 0x00 (entier court, 1 octet) chaque fois que la valeur à représenter tombe dans cet intervalle.
- Alternatives envisagées : plage asymétrique [0, 56] (rejetée : le corpus atteste l'usage fréquent de petites constantes négatives, notamment le pas `-1` d'une boucle FAIRE décroissante — cf. test `FAIRE 8 POUR I←T PAS -1 JUSQUA 1`, §14 — qu'une plage purement positive aurait exclue de ce sous-format) ; conserver une partie de la réserve pour une extension future non spécifiée de `RCIM_CAT_VALEUR` plutôt que d'affecter la totalité des 57 sous-codes disponibles (rejeté : aucun besoin futur identifié ne justifie de renoncer à un gain de compacité immédiat et mesurable ; si un besoin apparaissait, la technique de forme compacte à extension déjà retenue en IMP-0116 resterait applicable à l'un des sous-codes existants).
- Justification : ce choix prolonge directement le principe déjà posé par IMP-0103 (le plus petit sous-format capable de représenter exactement une valeur) d'un palier supplémentaire, strictement plus compact que 0x00. Les constantes de petite magnitude — bornes de boucle, pas d'incrémentation, petits indices — sont abondantes dans le corpus historique examiné, ce qui rend ce gain significatif à l'échelle d'un programme complet plutôt que marginal.
- Impact mémoire : économie d'un octet par occurrence d'une constante entière dans [−28, +28], par rapport à l'encodage 0x00 (entier court) qu'elle aurait sinon utilisé.
- Impact disque : sans objet (RCI-M).
- Impact performances : décodage trivial (une comparaison de plage sur le sous-code, une soustraction) ; aucun octet supplémentaire à lire, donc légèrement plus rapide que 0x00 pour les valeurs concernées.
- Impact historique : neutre — choix de représentation physique pur, sans conséquence observable (§26 du conditionnement) ; la valeur décompilée est strictement identique à celle produite par 0x00 pour la même constante.
- Références normatives : EXI-0016, EXI-0015 (décompilabilité) ; IMP-0103 (principe du plus petit sous-format) ; IMP-0116 (précédent d'exploitation d'une réserve pour un gain de compacité).
- Statut : retenu.

**Constante négative.** Un changement de signe appliqué directement à une constante numérique — l'opérande du changement de signe se réduisant à cette seule constante — est replié par le compilateur en la constante négative correspondante, codée dans le plus petit sous-format qui la représente exactement (`-1` → sous-code 0x22 ; `-200` → 0x01). Lorsque l'opérande du changement de signe n'est pas une constante seule, aucun repli n'a lieu : `-2↑2` se compile `2`, `2`, `0x05`, `0x02` et vaut −4, le changement de signe étant au rang le plus bas (chapitre Compilateur §5, « Précédence et associativité »). Le repli ne change ni la valeur ni le texte restitué (§4.12).

**Distinction avec le symbole `@` de référence (§4.5, 0x02).** Il s'agit ici de deux constructions physiquement et sémantiquement distinctes portant le même caractère source, propres chacune à une seule machine : le sous-code 0x06 de la présente table (valeur immédiate) représente la marque Mitra 15 apposée à une constante ; le sous-code 0x02 de la table des références (§4.5) représente la valeur T1600 « numéro de la ligne courante ». Le compilateur les distingue sans ambiguïté à la compilation, la syntaxe des deux emplois étant elle-même disjointe (l'un préfixe une constante dans une affectation, l'autre est un atome d'expression autonome).

**IMP-0103** — *Encodage numérique des valeurs immédiates*
- Sujet : représentation physique des nombres du L.S.E. dans RCI-M.
- Choix : trois formats immédiats distincts pour les valeurs numériques hors intervalle [−28, +28] (entier court 1 octet, entier 2 octets, réel `float` SANE 4 octets), plutôt qu'un format unique ; un quatrième palier, sans charge utile, est réservé aux entiers de faible magnitude (IMP-0123).
- Alternatives envisagées : format unique `float`/`extended` pour toute valeur numérique (uniforme mais coûteux) ; format unique entier long 4 octets (perd la partie fractionnaire).
- Justification : le Tome IV (§4.3, §4.4) ne fixe aucun codage physique des nombres. Un encodage à granularité variable maximise la compacité (EXI-0016) sans jamais perdre de précision.
- Impact mémoire : gain net sur les programmes pédagogiques typiques du L.S.E. (indices de boucle, petites constantes).
- Impact disque : sans objet (RCI-M).
- Impact performances : décodage trivial, coût négligeable sur 65816.
- Impact historique : le format n'est pas historiquement attesté (Tome IV §5.7).
- Références normatives : EXI-0014, EXI-0016, IV-FMT-0004, ANN-K-0011.
- Statut : retenu.

Ordre des octets : octet de poids faible en tête pour tout champ de plusieurs octets (IMP-0184).

### 4.5 Sous-format : Référence (`RCIM_CAT_REFERENCE`)

Une référence ne contient jamais l'objet final (ANN-K-0006, Tome III §6.3) ; elle contient un identifiant logique résolu par le gestionnaire des variables/tableaux/programmes à l'exécution.

**Principe de conception — pas de sous-code par catégorie de symbole.** Le sous-code d'une référence ne distingue jamais la catégorie déclarée du symbole visé (variable simple, chaîne, procédure, fonction). Cette absence de distinction physique se justifie pour deux raisons : (1) l'interpréteur ne peut de toute façon jamais s'y fier de manière fiable, puisque la catégorie d'un symbole peut légitimement changer entre la compilation d'une référence et son exécution (LIBERER, puis redéclaration sous une autre catégorie ; cf. §4.11.5, note sur CHAINE) — il doit donc, dans tous les cas, consulter la catégorie *effective* de l'identificateur au moment de l'exécution, tenue par l'environnement des identificateurs (chapitre État d'exécution, IMP-0179) ; (2) le décompilateur n'a besoin que du nom du symbole (porté par la table des symboles, jamais par le sous-code de la référence) pour reproduire fidèlement l'occurrence source — une variable-chaîne et une variable arithmétique s'écrivent de façon rigoureusement identique dans le texte source, et un nom de procédure porte déjà sa marque distinctive (`&`) dans le nom lui-même (LSE15 §7-2). Seule reste physiquement pertinente une distinction qui correspond à une différence réelle de **forme** (charge utile, nombre de valeurs consommées en pile) — pas de simple catégorie déclarée.

| Sous-code | Signification | Charge utile physique | Notes |
|---|---|---|---|
| 0x00 | Référence simple à un symbole (variable arithmétique, variable-chaîne, tableau entier non indexé, procédure ou fonction) | 2 octets : identifiant de symbole (index table des symboles) | Un tableau référencé sans indexation (ex. `AFFICHER A`, `LIRE A` où `A` est un tableau) utilise ce sous-code, désignant le tableau entier lorsque le symbole est un tableau à l'exécution — voir §4.10.3 (0x36) et §4.11.6 (0x3B) |
| 0x01 | Référence de tableau (accès indexé) | 2 octets identifiant de symbole + 1 octet nombre d'indices `k` à consommer sur la pile d'évaluation, `k` ∈ {1, 2} | Distinct du cas précédent par une différence physique réelle : charge utile supplémentaire (`k`) et consommation de `k` valeurs d'index en pile, nécessaires à l'interpréteur (combien dépiler) et au décompilateur (combien de sous-expressions reproduire entre crochets) |
| 0x02 | Référence au numéro de la ligne courante (symbole `@`) | 0 octet — résolu par l'interpréteur à partir de la ligne en cours d'exécution, déjà connue via `EntreeObjetCompile` (§5), sans consultation d'aucune table | **T1600 uniquement** [INRDP75, Annexe III §3] : « sur T1600, `@` représente le numéro de la ligne courante », utilisable dans une expression arithmétique (ex. `10 ALLER EN @+3`, équivalent à `ALLER EN 13`) — compilé ici comme une sous-séquence ordinaire précédant l'opération qui en consomme la valeur (EXI-0011) |
| 0x03 | Référence à la ligne de retour de la procédure courante (symbole `$`) | 0 octet — résolu par l'interpréteur à partir du cadre d'activation courant (état d'exécution, hors périmètre RCI-M) | **T1600 uniquement** [INRDP75, Annexe III §1] : « Symbole $ : indicateur de la ligne de retour dans une procédure » ; absent de Mitra 15 (règle de compatibilité : « ne pas utiliser les symboles $ et @ ») |
| 0x04–0x3F | réservé | — | |

**IMP-0120** — *Fusion des sous-codes de référence par forme physique, non par catégorie déclarée*
- Sujet : faut-il donner un sous-code distinct à chaque catégorie déclarée de symbole (variable simple, chaîne, procédure, fonction), ou regrouper toutes les références de forme physique identique sous un seul sous-code ?
- Choix : un unique sous-code générique (0x00) pour toute référence simple à un symbole (variable arithmétique, variable-chaîne, tableau entier non indexé, procédure ou fonction), quelle que soit sa nature ou sa catégorie à l'exécution ; les seuls sous-codes distincts restants (0x01 à 0x03) correspondent chacun à une différence physique réelle — charge utile supplémentaire, ou absence de résolution par symbole.
- Alternatives envisagées : un sous-code par catégorie déclarée (rejeté : aucune des deux exploitations possibles d'une telle distinction, l'exécution et la décompilation, n'en a l'usage, cf. paragraphe précédent) ; conserver la distinction procédure/fonction uniquement, en la faisant porter sur le site d'appel plutôt que sur la déclaration (rejeté comme solution intermédiaire : si le sous-code de la référence elle-même ne sert ni à l'exécution ni à la décompilation, le faire varier selon le site d'appel n'apporterait toujours rien physiquement — la distinction instruction/expression, si elle doit être portée quelque part, revient logiquement à l'élément `APPEL`, §4.11.5, qui détermine le comportement attendu après l'appel, non à la référence qui l'identifie).
- Justification : ce choix découle directement de l'exigence de compacité (EXI-0016) combinée à l'absence de tout usage démontré, par aucun des deux composants qui auraient pu la consommer (interpréteur, décompilateur), d'une distinction de sous-code par catégorie déclarée. Il procure aussi, comme bénéfice direct, une forme dédiée pour une référence de tableau sans indexation (utilisée par AFFICHER/LIRE sur un tableau entier), qui évite ainsi d'emprunter le sous-code de l'accès indexé avec un octet `k` toujours nul.
- Impact mémoire : économie d'un octet par référence de tableau entier non indexé, par rapport à une représentation qui emprunterait le sous-code de l'accès indexé avec un octet `k` toujours nul (hypothèse écartée ci-dessus) ; aucun changement pour les autres références (toutes déjà à 2 octets).
- Impact disque : sans objet (RCI-M).
- Impact performances : neutre à légèrement positif (une comparaison de moins pour le décodeur, qui n'a plus à distinguer cinq sous-codes de forme identique).
- Impact historique : neutre — aucune propriété logique n'est perdue, puisque la catégorie d'un identificateur reste intégralement représentée, à l'exécution, dans l'environnement des identificateurs (chapitre État d'exécution, IMP-0179), seule structure qui en a l'usage réel.
- Références normatives : EXI-0016, ANN-K-0009 ; §6.2 (nature du symbole) ; LSE15 §7-2 (nom de procédure préfixé par `&`).
- Statut : retenu.

**Symbole `@` : sémantique dépendante de la machine.** Le corpus historique (INRDP75, Annexe III §3, « SYMBOLE @ ») indique que `@` n'a pas la même signification sur les deux machines. Sur T1600, `@` désigne le numéro de la ligne courante, comme décrit ci-dessus. Sur Mitra 15, `@` n'est pas un atome d'expression au sens de la grammaire mais un **marqueur** apposé à une constante numérique dans une affectation, pour signaler que la variable ainsi affectée représente un numéro de ligne — voir §4.4 (valeur immédiate, sous-code 0x06) pour sa représentation physique, distincte de celle-ci.

**IMP-0119** — *Représentation versionnée des symboles `@` et `$`*
- Sujet : représentation physique des symboles `@` (T1600 : numéro de ligne courante ; Mitra 15 : marqueur de constante-ligne) et `$` (T1600 : ligne de retour de procédure), tous deux absents de la grammaire reconstruite ou incomplètement représentés par elle.
- Choix : trois sous-codes distincts, chacun explicitement annoté de sa version : §4.5 sous-code 0x02 (`@`, T1600, référence), §4.4 sous-code 0x06 (`@`, Mitra 15, valeur marquée), §4.5 sous-code 0x03 (`$`, T1600, référence). Aucun de ces trois sous-codes n'est disponible sur l'autre machine que celle indiquée ; il appartient au compilateur, selon le profil d'implantation de la session (chapitre Compilateur §8 bis, IMP-0180), de refuser leur emploi hors de leur version.
- Alternatives envisagées : sous-code unique `@` couvrant les deux significations, désambiguïsé par le contexte syntaxique (rejeté : les deux significations produisent des éléments physiques de catégories différentes — référence pour T1600, valeur marquée pour Mitra 15 — une fusion aurait dissimulé cette différence de nature plutôt que de simple valeur) ; absence de sous-code dédié pour `$`, le traitant comme une variable ordinaire (rejeté : `$` ne désigne aucun symbole déclaré, mais une information portée par le cadre d'activation courant, ce qui exige un sous-code de référence propre, au même titre que `@` sur T1600).
- Justification : ces trois éléments sont attestés par une source historique de premier rang (INRDP75, Annexe III, consacrée précisément à la compatibilité et aux spécificités des deux machines) et absents de la grammaire reconstruite ; leur omission aurait rendu RCI-M incapable de représenter des programmes T1600 ou Mitra 15 pourtant conformes aux sources historiques.
- Impact mémoire : 0 octet pour `@`(T1600) et `$` (résolution par contexte d'exécution) ; 1 octet pour `@` (Mitra 15, identique à une constante de ligne ordinaire, IMP-0125).
- Impact disque : sans objet à ce stade.
- Impact performances : résolution triviale (numéro de ligne courant déjà connu de l'interpréteur pour `@`/T1600 ; sommet du cadre d'activation pour `$`).
- Impact historique : restitue exactement la distinction documentée par Annexe III, sans fusionner deux significations distinctes sous un même symbole source.
- Références normatives : INRDP75, Annexe III §1 et §3 ; §4.4 (valeur immédiate) ; §4.5 (référence).
- Statut : retenu.

**IMP-0104** — *Identifiants de symboles compacts (handles) et frontière RCI-S*
- Sujet : nature physique d'un identifiant de symbole référencé par un élément RCI-M ; conséquences sur la future sérialisation RCI-S.
- Choix : entier non signé 16 bits, index dans la table des symboles du programme courant. Aucune adresse mémoire n'est jamais encodée dans RCI-M.
- Alternatives envisagées : pointeur C direct vers la structure de symbole en mémoire (rejeté, §9 du conditionnement) ; nom textuel complet répété à chaque référence (rejeté, EXI-0016/ANN-K-0009).
- Justification : un identifiant 16 bits couvre toute la table des symboles, plafonnée à 6 553 entrées (ci-dessous), ce qui suffit largement sur une cible pédagogique ; il reste stable si le gestionnaire d'objets déplace ou réorganise la mémoire (compaction).
- Impact mémoire : 2 octets par référence, contre 4 octets pour un pointeur C.
- Impact disque : la table des symboles, sérialisée, sert de base à la table des symboles de RCI-S (ANN-K-0009, ANN-K-0022), mais **pas telle quelle** : la sérialisation devra filtrer au minimum
  1. les variables non référencées par le programme courant (notamment celles définies uniquement en mode machine de bureau, cf. IMP-0109, qui n'ont aucune raison de survivre dans un fichier-programme puisqu'elles n'appartiennent à aucune ligne conservée) ;
  2. les entrées qui ne portent qu'une valeur transitoire sans utilité de compatibilité ou de décompilation (à distinguer strictement d'un symbole réellement référencé) ;
  3. éventuellement la nature du symbole (`SYM_PROCEDURE` ou `SYM_IDENTIFICATEUR`, §6.2), si elle s'avère reconstructible à la restauration à partir du seul usage qui en est fait dans la séquence compilée — point laissé ouvert et à trancher dans le chapitre RCI-S du Tome V. La catégorie effective d'un identificateur (variable, chaîne, tableau) n'est, elle, jamais portée par la table des symboles, et n'est donc pas concernée (chapitre État d'exécution, IMP-0179).
  Ce filtrage est un point d'attention à traiter dans le chapitre RCI-S, pas dans le présent chapitre ; il est mentionné ici parce que la structure physique de la table des symboles (§6) doit porter dès maintenant l'information nécessaire à ce futur filtrage (champ `origine`, cf. §6.2).
- Impact performances : résolution en O(1) par indexation directe dans la table des symboles.
- Impact historique : neutre.
- Références normatives : EXI-0014, ANN-K-0009, ANN-K-0010, ANN-K-0028 (« suppression des informations reconstructibles » côté RCI-S), Tome III §6.3, conditionnement §9.
- Statut : retenu ; volet RCI-S renvoyé au chapitre dédié.
- Complément du 2026-10-30 (Éric ; audit du code, AC-32) : la table des symboles et la table des constantes partagées (§7, IMP-0107) sont chacune un seul tableau obtenu par `malloc` et agrandi par `realloc` (Architecture physique IMP-0132). En modèle small, ORCA/C suppose qu'aucun tableau alloué ne dépasse 64 Kio (manuel ORCA/C 2.2, `memorymodel`). La table des symboles est donc plafonnée à `65535 / sizeof(EntreeSymbole)` entrées, soit 6 553 entrées de 10 octets, et la table des constantes à `65535 / sizeof(EntreeConstante)`, soit 21 845 entrées de 3 octets. Les identifiants et les index restent codés sur 16 bits. Au-delà du plafond, la compilation rend la nature « capacité des tables atteinte », C 108 (Compilateur IMP-0191) ; un fichier-programme dont une table dépasse son plafond est refusé par F 101 (Bibliothécaire et fichiers IMP-0246).

### 4.6 Sous-format : Opération codée (`RCIM_CAT_OPERATION`)

Une opération codée tient physiquement, dans le cas général, dans le seul octet de tag : catégorie `10` + sous-code identifiant l'opération sémantique (Tome III §6.4). Les opérations arithmétiques, relationnelles et logiques, l'affectation et les fonctions chaînes à arité fixe n'ont aucune charge utile. Portent en revanche une charge utile : l'appel de procédure (0x28, indicateur de position et nombre d'arguments, IMP-0121, IMP-0177) ; les déclarations TABLEAU, CHAINE et LIBERER (0x29–0x2B, identifiant de symbole) ; les fonctions chaînes et instructions de fichiers à arité variable (0x1F–0x24, 0x2D–0x2F, octet d'arité, IMP-0122) ; les spécifications du format d'AFFICHER (§4.10) et de LIRE (§4.11.6). Le détail exhaustif de chaque sous-code, catégorie par catégorie, fait l'objet du §4.11.

| Plage de sous-codes | Classe fonctionnelle (Tome III §6.4) |
|---|---|
| 0x00–0x0F | Opérations arithmétiques (opérateurs + fonctions élémentaires communes) — voir §4.11.2 |
| 0x10–0x1B | Opérations relationnelles, logiques, affectation — voir §4.11.3 |
| 0x1C–0x27 | Opérations de valeur : concaténation et fonctions chaînes — voir §4.11.4 |
| 0x28–0x2F | Opérations d'appel, de déclaration et de gestion de fichiers — voir §4.11.5 |
| 0x30–0x37 | Opérations d'entrée-sortie liées à AFFICHER : spécifications du format, affichage d'une valeur, marque de tête d'une instruction à format explicite — voir §4.10 (charge utile variable) |
| 0x38 | Marque de tête d'une instruction AFFICHER sans format (format implicite) — voir §4.10 (à la suite de la série 0x30–0x37) |
| 0x39–0x3B | Opérations d'entrée liées à LIRE et à son format — voir §4.11.6 |
| 0x3C–0x3F | Fonctions arithmétiques propres au T1600 (SH, CH, TH, SGN) — voir §4.11.2 |

Les opérations de **contrôle de flux** au sens strict nécessitent une cible et relèvent donc de la catégorie « information de contrôle » (§4.7), conformément à Tome III §6.7.

### 4.7 Sous-format : Information de contrôle (`RCIM_CAT_CONTROLE`)

La cible d'un branchement n'est pas toujours connue à la compilation. La documentation de l'instruction ALLER EN indique explicitement que le numéro de ligne cible « est en général une constante numérique entière, mais ce peut être également la valeur d'une variable ou d'une expression arithmétique » [INRDP75, 3ᵉ partie, §2 « Instruction ALLER EN »]. La même règle s'applique à RETOUR EN pour la sortie extraordinaire de procédure, et au numéro de ligne de fin `n1` de l'instruction FAIRE (le corpus précise que « n1, v1, v2 et p sont des expressions arithmétiques »). VM-1402 (§1.3) confirme d'ailleurs que c'est l'interpréteur qui résout la destination, pas la représentation compilée elle-même.

Le sous-code 0x02 ci-dessous (« saut vers une autre ligne ») ne couvre donc que le cas où la ligne cible est exprimée par une constante arithmétique repérable à la compilation ; il est complété par des sous-codes à cible calculée (0x03/0x06).

| Sous-code | Signification | Charge utile physique |
|---|---|---|
| 0x00 | Branchement conditionnel intra-séquence (consomme 1 valeur logique en pile) — généré par le compilateur pour SI...ALORS...SINON | 1 octet : offset logique cible dans le même bloc |
| 0x01 | Saut inconditionnel intra-séquence — généré par le compilateur pour sauter la branche SINON après exécution de ALORS | 1 octet : offset logique cible dans le même bloc |
| 0x02 | ALLER EN — cible exprimée par une constante arithmétique repérable à la compilation | 1 octet : numéro de ligne logique |
| 0x03 | ALLER EN — cible calculée (variable ou expression arithmétique générale) | 0 octet — la cible est consommée sur la pile d'évaluation, déposée par la séquence postfixée qui précède immédiatement cet élément (EXI-0011) |
| 0x04 | RETOUR (retour de procédure/fonction au point d'appel) | 0 octet |
| 0x05 | RETOUR EN — cible constante (sortie extraordinaire de procédure) | 1 octet : numéro de ligne logique |
| 0x06 | RETOUR EN — cible calculée (sortie extraordinaire de procédure) | 0 octet — cible consommée en pile |
| 0x07 | Fin d'exécution (TERMINER) | 0 octet |
| 0x08 | Suspension (PAUSE) | 0 octet |
| 0x09 | Ouverture de boucle FAIRE (JUSQUA ou TANT QUE) — voir §4.8.2, IMP-0112 | 2 octets : identifiant du symbole de la variable contrôlée `V` (IMP-0104), porté nu, sans étiquette de référence — consomme `n1` puis `v1`, déposés par les sous-séquences qui précèdent immédiatement cet élément |
| 0x0A | Pas de boucle FAIRE explicite (`PAS p`) — voir §4.8.2 | 0 octet — consomme `p`, déposé par la sous-séquence qui précède immédiatement cet élément |
| 0x0B | RESULTAT (retour de fonction avec valeur) | 0 octet — la valeur de retour est déjà en pile, déposée par la sous-séquence postfixée qui précède immédiatement cet élément (EXI-0011) ; distinct de RETOUR (0x04), qui ne transmet aucune valeur |
| 0x0C | Commentaire (`*` jusqu'à fin de ligne) — voir §4.9 | 1 octet de longueur `n` + `n` octets de texte |
| 0x0D | Déclaration de procédure (`PROCEDURE &ID(...) LOCAL ...`) — premier élément de la ligne de déclaration ; aucun effet exécutable, sa rencontre par l'interpréteur hors appel est une erreur d'exécution (chapitre Compilateur §13.2, IMP-0174 ; État d'exécution IMP-0155) | 2 octets identifiant du symbole de la procédure ; 1 octet `n` (arité) ; `n` × 2 octets identifiants des paramètres formels ; 1 octet `m` (0 = pas de clause `LOCAL`) ; `m` × 2 octets identifiants de la liste `LOCAL`, dans l'ordre du texte |
| 0x0E | Pas de boucle FAIRE implicite (`PAS p` omis, pas égal à 1) — voir §4.8.2 | 0 octet — ne consomme rien |
| 0x0F | Test de boucle FAIRE...JUSQUA — voir §4.8.2 | 0 octet — consomme la limite `v2` |
| 0x10 | Test de boucle FAIRE...TANT QUE — voir §4.8.2 | 0 octet — consomme la valeur logique de la condition |
| 0x11 | Marque de tête d'une instruction LIRE — voir §4.12, IMP-0183 | 0 octet — aucun effet à l'exécution autre que de marquer le début de l'instruction |
| 0x12 | Ouverture de boucle FAIRE sans variable contrôlée (`FAIRE n1 TANT QUE c`, profil `LSE F4`) — voir §4.8.2, IMP-0277 | 0 octet — consomme `n1`, déposé par la sous-séquence qui précède immédiatement cet élément |
| 0x13–0x3F | réservé | — |

**IMP-0111** — *Cibles de branchement calculées*
- Sujet : représentation physique d'une cible de branchement (ALLER EN, RETOUR EN) exprimée par une expression arithmétique générale plutôt qu'une constante.
- Choix : lorsque le compilateur ne peut pas réduire la cible à une constante arithmétique à la compilation, il compile l'expression cible comme une sous-séquence postfixée ordinaire (§4.4/§4.5/§4.6) précédant immédiatement un élément d'information de contrôle « à cible calculée » (sous-codes 0x03/0x06) sans charge utile propre ; la cible est consommée sur la pile d'évaluation au moment où l'opération de branchement est atteinte.
- Alternatives envisagées : imposer une cible toujours constante et rejeter à la compilation les expressions non constantes (rejeté : contredirait la documentation historique de ALLER EN et RETOUR EN, qui autorise explicitement les cibles calculées — modifier ce comportement reviendrait à modifier une propriété historique, interdit par §2 et §19 du conditionnement) ; encoder systématiquement une cible constante « factice » convertie en référence de variable (rejeté : perd la distinction entre constante et calcul, contraire à EXI-0013/EXI-0014).
- Justification : ce choix respecte simultanément EXI-0011 (postfixe : la cible, comme tout opérande, est calculée avant l'opération qui la consomme) et VM-1402 (résolution de la destination par l'interpréteur, pas par une adresse figée dans la représentation compilée). Il ne modifie aucune propriété historique : il couvre un cas déjà documenté (ALLER EN/RETOUR EN à cible calculée).
- Impact mémoire : les cibles constantes sont codées sur 1 octet (IMP-0125), cas majoritaire dans les exemples historiques examinés, où la cible est très souvent un numéro de ligne littéral ; les cibles calculées n'ajoutent aucun octet à l'élément de contrôle lui-même, le coût étant celui, déjà prévu, de la sous-expression compilée.
- Impact disque : la distinction constante/calculée devra être conservée dans RCI-S (chapitre dédié), car elle conditionne la vérification de validité de la cible à la restauration (ANN-K-0032 invariant 4).
- Impact performances : un branchement à cible constante reste résolu en une lecture directe d'un octet ; un branchement à cible calculée dépotte une valeur de pile déjà présente, coût marginal.
- Impact historique : couvre la documentation historique de ALLER EN et RETOUR EN sans introduire de comportement nouveau.
- Références normatives : EXI-0011, EXI-0046, VM-1402, INRDP75, 3ᵉ partie, §2 « Instruction ALLER EN » ; LSE15 §7-5 (sortie extraordinaire de procédure).
- Statut : retenu.

**IMP-0125** — *Numéro de ligne logique et offset intra-séquence codés sur 1 octet*
- Sujet : taille physique de deux charges utiles : le numéro de ligne logique constant (§4.7 sous-codes 0x02/0x05 ; §4.4 sous-code 0x06 ; `EntreeObjetCompile.numero_ligne` et `InfoProcedure.numero_ligne_entree`) et l'offset de branchement intra-séquence (§4.7 sous-codes 0x00/0x01).
- Choix : 1 octet non signé (0–255) pour les deux charges utiles, partout où elles apparaissent dans ce chapitre, y compris `EntreeObjetCompile.numero_ligne` (§5) et `InfoProcedure.numero_ligne_entree` (structure, §6.2 ; IMP-0114, §6.3), pour une représentation uniforme du concept « numéro de ligne » dans l'ensemble du chapitre.
- Alternatives envisagées : conserver 2 octets par prudence, sans justification chiffrée (rejeté : le corpus fixe explicitement la plage valide d'un numéro de ligne — « un numéro compris entre 1 et 250 (sur Mitra 15), 1 et 255 (sur T1600) » [INRDP75, « Présentation d'un programme L.S.E. »] — entièrement contenue dans un octet non signé ; conserver 2 octets sans raison reviendrait à ignorer une contrainte déjà posée par le corpus lui-même) ; ne réduire que les sous-codes de §4.7 en laissant §5/§6.2 à 2 octets (rejeté : introduirait deux représentations physiques différentes du même concept « numéro de ligne » dans un même chapitre, sans justification).
- Justification, offset intra-séquence : le corpus borne également la ligne source elle-même — « L'ensemble, y compris le caractère de fin de ligne ⓧ, comporte au plus 80 caractères » [INRDP75, même section], numéro de ligne et terminateur inclus. Le motif source le plus dense en octets compilés identifié (chaîne d'opérandes à un seul caractère séparés par un opérateur à charge utile nulle, ex. `A+B+C+...`) produit un ratio croissant vers 2 octets compilés par caractère source, chaque paire `Xopérateur` produisant une référence de 3 octets (tag + identifiant 16 bits, IMP-0104/IMP-0120) et un opérateur d'1 octet. Pour les quelque 77 caractères d'instructions disponibles après numéro de ligne, espace et terminateur, ce majorant donne environ 155 octets de bloc compilé — sous la borne de 255 avec une marge confortable. Ce majorant n'est qu'une estimation : la borne de 255 octets est garantie, pour toute ligne, par le contrôle de débordement du compilateur (IMP-0105 ; chapitre Compilateur, IMP-0165). Aucune construction plus dense n'a été identifiée (listes séparées par virgules dans un appel, arguments d'AFFICHER/LIRE : ratio inférieur, faute d'opérateur à charge utile).
- Justification, numéro de ligne : la plage documentée (1–250/1–255) est entièrement contenue dans un octet non signé sans aucune marge à sacrifier.
- Impact mémoire : économie d'un octet par occurrence de chacune des charges utiles concernées par rapport à un encodage sur 2 octets ; pour `EntreeObjetCompile` (§5, ~7 octets/ligne avec ce choix), un programme de 2000 lignes occupe environ 14 Kio pour cette seule table.
- Impact disque : sans objet (RCI-M) ; une future RCI-S héritera directement de ce format réduit.
- Impact performances : lecture d'un seul octet au lieu de deux pour chacune de ces charges utiles, légèrement plus rapide sur 65816.
- Impact historique : neutre — aucune ligne valide du langage ne peut porter un numéro hors de la plage représentable, et le majorant construit pour l'offset intra-séquence couvre toute ligne conforme à la limite de 80 caractères du corpus ; aucune perte de capacité de représentation pour un programme historiquement valide.
- Références normatives : INRDP75, « Présentation d'un programme L.S.E. » (plage des numéros de ligne, limite de 80 caractères) ; IMP-0101 (un bloc par ligne) ; IMP-0105 (offset logique, non une adresse) ; IMP-0104, IMP-0120 (coût d'une référence simple : identifiant 16 bits, sous-code générique).
- Statut : retenu.

**IMP-0105** — *Cible des branchements intra-ligne : position comptée depuis le début du bloc, séquence bornée à 255 octets*
- Sujet : nature physique de la « position cible » d'un branchement à l'intérieur d'une même séquence compilée (sous-codes 0x00/0x01 ci-dessus), et borne de longueur de séquence qui en découle.
- Choix : l'octet de charge utile de 0x00 et de 0x01 est la **position de l'élément cible comptée en octets depuis le premier octet du bloc** de la ligne (position 0 = premier octet de la séquence), c'est-à-dire la valeur que prend `CurseurSequence.position` (§9) lorsque le curseur est placé sur cet élément ; l'interpréteur réalise le branchement par la seule affectation `position ← cible`. Lorsque la cible est la fin de la séquence (branche se terminant en fin de ligne), l'octet vaut la longueur de la séquence. Cette position n'est jamais un pointeur C, ni un décalage relatif à l'élément de branchement. Pour que toute position, fin de séquence comprise, soit représentable sur un octet, **la séquence compilée d'une ligne ne dépasse jamais 255 octets** ; le compilateur le garantit par un contrôle à chaque écriture (chapitre Compilateur, IMP-0165).
- Alternatives envisagées : décalage relatif à l'élément de branchement (rejeté : impose de fixer une origine — début de l'élément ou fin de sa charge utile — et une addition à chaque branchement, pour une capacité identique, les branchements de `SI` étant toujours dirigés vers l'avant ; et diverge de la convention de position déjà employée par `CurseurSequence`, par le point de reprise d'une boucle FAIRE et par la position de retour d'un appel, chapitre État d'exécution) ; offset sur 2 octets (rejeté : un octet suffit pour toute ligne conforme, cf. IMP-0125).
- Justification : le bloc peut être déplacé en mémoire par le gestionnaire d'objets (compaction) sans invalider les branchements internes, qui restent repérés depuis le début du bloc et ne franchissent jamais une frontière d'objet. Une seule convention de position intra-ligne est employée dans tout le Tome V.
- Impact mémoire : 1 octet par branchement intra-ligne.
- Impact disque : sans objet (RCI-M) ; la position se sérialise sans transformation, le bloc étant sérialisé tel quel.
- Impact performances : branchement par simple affectation de la position du curseur.
- Impact historique : neutre — choix de représentation physique pur.
- Références normatives : EXI-0011, EXI-0012, Tome III §6.7 ; §9 (`CurseurSequence`) ; IMP-0125 ; chapitre Compilateur, IMP-0165, IMP-0166.
- Statut : retenu.

---

### 4.8 Couverture des instructions de contrôle du L.S.E.

Cette section vérifie explicitement, instruction par instruction, que le format défini en §4.7 (et le format d'opération §4.6) couvre les besoins des instructions de contrôle attestées par le corpus historique.

#### 4.8.1 ALLER EN ea

`ALLER EN ea` (INRDP75, 3ᵉ partie, §2 « Instruction ALLER EN ») : branchement à la ligne dont le numéro est la valeur de l'expression arithmétique `ea`, « en général une constante numérique entière, mais ce peut être également la valeur d'une variable ou d'une expression arithmétique ».

**Couverture :** sous-codes 0x02 (constante) / 0x03 (calculée), §4.7, complétés par IMP-0111. **Couvert.**

#### 4.8.2 FAIRE n1 POUR V←v1 PAS p JUSQUA v2 / TANT QUE ⟨expr. booléenne⟩

Le corpus définit la boucle FAIRE comme « la partie de programme comprise entre l'instruction FAIRE et la ligne de numéro n1 (ces deux lignes extrêmes comprises) » (INRDP75, §4-1), où `n1`, `v1`, `p` et `v2` sont des expressions arithmétiques et `V` « une variable arithmétique simple » ; la variante TANT QUE remplace la limite `v2` par une expression booléenne (INRDP75, §4-2). LSE15 précise l'ordre et la fréquence des évaluations, sur lesquels INRDP75 est moins détaillé sans le contredire :

- **Évaluation unique de `n1` et `v1`.** `n1` « n'est évalué qu'une seule fois » ; `v1` « n'est évalué qu'une seule fois, comme ε1, à l'entrée de la boucle » (LSE15 §5-5.1).
- **Réévaluation du pas et de la limite.** « Les expressions arithmétiques correspondant au pas (ε3), et à la limite (ε4) sont évaluées à la fin de chaque boucle, et non seulement au début de la première boucle. En particulier, le signe du pas est réévalué avant comparaison de V à ε4 » (LSE15 §5-5.3).
- **Ordre au premier passage.** « La valeur de l'expression arithmétique ε2 est rangée dans la variable contrôlée V, le pas est évalué, et l'on compare la valeur de V avec la valeur de la limite ε4 » ; pour TANT QUE : « La valeur ε2 est rangée dans V, le pas est évalué et l'on regarde si la condition indiquée après `TANT QUE` est vérifiée » (LSE15 §5-5.3).
- **Ordre en fin d'itération.** « La boucle est exécutée une fois, puis le pas (ε3) est réévalué et ajouté à la variable contrôlée V ; on compare alors la nouvelle valeur de V à ε4, comme plus haut » ; pour TANT QUE : « on réévalue le pas (ε3), on l'ajoute à la variable contrôlée, et l'on va de nouveau vérifier la condition » (LSE15 §5-5.3).
- **Pas omis.** « On peut omettre le mot `PAS` et ε3 ; le pas est alors pris égal à 1 » (LSE15 §5-5.1 ; INRDP75, §4-1 : « Si p=1, `PAS p` peut être omis »).
- **Réinitialisation.** « Si, dans une boucle FAIRE, on se branche vers l'instruction FAIRE elle-même, ceci a pour effet de recommencer l'exécution de la boucle, en ré-initialisant tous les paramètres » (LSE15 §5-5.4, Remarque 2).

Ces règles ont une conséquence directe sur la représentation : la limite (ou la condition) et le pas peuvent dépendre de `V` lui-même. Les deux exemples TANT QUE de LSE15 §5-5.3 en dépendent : `FAIRE 12 POUR K←1 TANT QUE X[K]>=0 ET K<=50` (condition portant sur `K`, qui vient de recevoir `v1`) et `FAIRE 20 POUR X←A PAS (A/X-X)/2 TANT QUE ABS(X↑2-A)>1E-3` (pas et condition portant sur `X`). Ces expressions doivent donc être évaluées **après** l'affectation `V←v1` au premier passage, **après** l'ajout du pas en fin d'itération, et **à chaque** itération.

**Analyse de couverture — point de vigilance sur EXI-0001.** Faire porter à la ligne `n1` elle-même un élément physique de « fermeture de boucle », corrélé par le numéro de la ligne FAIRE, serait incompatible avec EXI-0001 : le compilateur, lorsqu'il compile la ligne `n1`, ne dispose d'aucune information sur les autres lignes du programme, ni sur l'existence d'une instruction FAIRE qui la désignerait comme borne — a fortiori si la ligne `n1` est compilée avant la ligne FAIRE correspondante. Un tel marqueur n'apporterait d'ailleurs rien à la décompilation : le texte de la ligne `n1` ne contient aucune trace de son appartenance à une boucle, et LISTER la restitue comme n'importe quelle autre ligne. La fin de boucle est donc reconnue par l'état d'exécution, jamais par la ligne `n1`.

**IMP-0112** — *Représentation physique de FAIRE : trois éléments sur la seule ligne FAIRE (ouverture, pas, test), corrélation par l'état d'exécution*
- Sujet : éléments RCI-M nécessaires à la compilation de FAIRE...JUSQUA et de FAIRE...TANT QUE, de façon que l'ordre et la fréquence des évaluations prescrits par LSE15 §5-5.1 et §5-5.3 soient respectés, que l'instruction soit décompilable sans ambiguïté (y compris la présence ou l'absence de `PAS`), et sans violer EXI-0001.
- Choix : la ligne portant l'instruction FAIRE compile, dans cet ordre, cinq parties dont trois éléments de contrôle :

```text
[sous-séquence de n1] [sous-séquence de v1]
OUVERTURE 0x09 (identifiant de V)
[sous-séquence de p]  PAS EXPLICITE 0x0A        — ou, si PAS est omis : PAS IMPLICITE 0x0E seul
[sous-séquence de v2] TEST JUSQUA 0x0F          — ou : [sous-séquence de la condition] TEST TANT QUE 0x10
[corps : instructions qui suivent sur la même ligne, puis lignes suivantes jusqu'à n1]
```

  | Élément | Charge utile | Consomme | Effet à l'exécution |
  |---|---|---|---|
  | OUVERTURE 0x09 | 2 octets : identifiant du symbole `V` (IMP-0104), sans étiquette de référence | `n1`, `v1` | affecte `v1` à `V` ; empile, dans le cadre d'activation courant, un contexte de boucle en phase « initiale » (état d'exécution, hors périmètre RCI-M) ; poursuit à l'élément suivant |
  | PAS EXPLICITE 0x0A | aucune | `p` | mémorise `p` dans le contexte ; en phase « itération », ajoute `p` à `V` ; poursuit |
  | PAS IMPLICITE 0x0E | aucune | rien | même effet, avec un pas égal à 1 |
  | TEST JUSQUA 0x0F | aucune | `v2` | sortie de boucle si `V > v2` avec un pas positif, ou `V < v2` avec un pas négatif ; sinon poursuit à l'élément suivant, premier élément du corps |
  | TEST TANT QUE 0x10 | aucune | valeur logique | sortie de boucle si la condition est fausse ; sinon poursuit au corps |

  **Aucun élément n'est compilé sur la ligne `n1` au titre de la boucle** : cette ligne est compilée exactement comme si elle n'était la cible d'aucune instruction FAIRE, en stricte conformité avec EXI-0001.

  Mécanisme de corrélation (relevant de l'interpréteur, non de RCI-M) :
  - **Premier passage.** Les sous-séquences et les éléments s'exécutent dans leur ordre physique, qui est exactement l'ordre d'évaluation prescrit : `n1` et `v1` évalués, `V←v1`, pas évalué, limite ou condition évaluée avec la nouvelle valeur de `V`, test.
  - **Fin d'itération.** Après l'exécution de la ligne qui termine la boucle — sans que cette ligne porte la moindre marque physique, qu'il s'agisse d'une ligne ordinaire ou d'une « ligne vide » ne comportant qu'un numéro (LSE15 §5-5.3, Remarque 2 ; INRDP75, l. 3064) —, l'interpréteur fait passer le contexte de boucle actif en phase « itération » et reprend l'exécution **sur la ligne FAIRE, à l'élément qui suit immédiatement l'OUVERTURE**. Le pas est alors réévalué et ajouté à `V` (élément de pas), puis la limite ou la condition est réévaluée et testée (élément de test) — exactement l'ordre de LSE15 §5-5.3. Si la boucle continue, l'exécution se poursuit au premier élément du corps, sur la même ligne si des instructions y suivent l'élément de test, sur la ligne suivante sinon (convention générale de l'instruction suivante, LSE15 §5-2 et §5-5.2, illustrée par `3 SI A=0 ALORS DEBUT FAIRE 5 POUR I←0 JUSQUA 9; J←I+1 FIN SINON ALLER EN 206`).
  - **Sortie ordinaire.** L'élément de test dépile le contexte et poursuit à la première ligne qui suit la ligne de fin de boucle ; `V` conserve « la valeur qui a fait jouer le test de sortie » (LSE15 §5-5.3).
  - **Réinitialisation.** L'élément d'OUVERTURE n'est jamais réexécuté par la fin d'itération ; il ne l'est que si l'exécution atteint de nouveau la ligne FAIRE par son début (branchement vers cette ligne, LSE15 §5-5.4, Remarque 2), auquel cas il réaffecte `v1` à `V` et réinitialise la boucle sans condition. L'affectation unique de `v1` au premier passage découle ainsi de la seule disposition physique des éléments, sans aucun test sur l'élément d'ouverture lui-même.

  La détermination de la ligne de fin de boucle effective (arrondi de `n1`, ligne existante la plus proche par valeurs inférieures, LSE15 §5-5.1), la gestion de la pile des contextes et la désimbrication en cas de sortie extraordinaire relèvent de l'état d'exécution (chapitre État d'exécution, §6).
- Alternatives envisagées :
  1. Marqueur de fermeture physique sur la ligne `n1`, corrélé par le numéro de la ligne FAIRE — écarté : viole EXI-0001 (analyse de couverture ci-dessus).
  2. Un élément d'ouverture unique placé après toutes les sous-séquences (`n1`, `v1`, `p`, `v2` ou condition) — écarté. Le pas et la limite seraient évalués avant l'affectation `V←v1`, donc avec l'ancienne valeur de `V`, ce qui fausse les deux exemples TANT QUE de LSE15 §5-5.3 ; leur réévaluation à chaque itération exigerait de retrouver le début de leurs sous-séquences, que rien ne permet de localiser en remontant une séquence postfixée ; et le nombre de valeurs consommées (3 ou 4 selon la présence de `PAS`) ne serait marqué nulle part.
  3. Un élément unique portant, en charge utile, les offsets des sous-séquences du pas et de la limite, réexécutées par saut — écarté : ajoute deux octets d'offset par boucle pour un résultat que la disposition retenue obtient sans aucun offset, l'ordre physique des éléments coïncidant avec l'ordre d'évaluation, et la fin d'itération se réduisant à une reprise juste après l'OUVERTURE.
  4. Compiler la constante 1 lorsque `PAS` est omis — écarté : le décompilateur ne pourrait plus distinguer `PAS 1` écrit de `PAS` omis.
  5. Signaler la présence de `PAS` et la variante JUSQUA/TANT QUE par un octet de charge utile plutôt que par des sous-codes distincts — écarté : la catégorie « information de contrôle » dispose d'une réserve large, et des sous-codes distincts suivent le précédent déjà retenu pour ALLER EN (0x02/0x03) et RETOUR EN (0x05/0x06) ; chaque élément a ainsi un effet sur la pile déterminé par son seul sous-code, ce que le décompilateur exploite directement.
  6. Conserver l'état de boucle dans la table des symboles plutôt que dans l'état d'exécution — écarté : violerait la séparation IMP-0108 et empêcherait la réentrance en cas d'appel récursif d'une procédure contenant une boucle.
  7. Reprendre, en fin d'itération, à la ligne FAIRE elle-même, l'élément d'ouverture détectant un contexte déjà actif pour s'abstenir de réaffecter `v1` — écarté : contredit LSE15 §5-5.4, Remarque 2, qui n'admet aucune exception à la réinitialisation lors d'un passage par l'instruction FAIRE.
- Justification : la disposition retenue rend l'ordre physique des éléments identique à l'ordre d'évaluation prescrit par LSE15 §5-5.3, ce qui garantit la réévaluation du pas et de la limite, avec la valeur courante de `V`, sans aucun mécanisme spécifique. Chaque élément porte, par son sous-code, l'effet exact qu'il a sur la pile d'évaluation, ce qui rend la décompilation de l'instruction déterministe et fidèle, y compris pour la présence ou l'absence de `PAS`. La ligne `n1` n'est jamais concernée (EXI-0001).
- Impact mémoire : 5 octets d'éléments de contrôle par instruction FAIRE (OUVERTURE 3, PAS 1, TEST 1), en plus des sous-séquences de `n1`, `v1`, `p` (absente si `PAS` est omis) et de la limite ou de la condition ; aucun coût sur la ligne `n1`.
- Impact disque : sans objet à ce stade (RCI-M) ; en RCI-S, la ligne `n1` se sérialise sans aucune particularité liée à la boucle.
- Impact performances : à chaque itération, le pas et la limite (ou la condition) sont réévalués, comme l'exige le corpus ; le test de fin d'itération est une comparaison entière par changement de ligne (chapitre État d'exécution, §6.2).
- Impact historique : couvre fidèlement les deux variantes JUSQUA et TANT QUE, l'évaluation unique de `n1` et `v1`, la réévaluation du pas et de la limite (signe du pas compris), l'ordre des évaluations au premier passage et en fin d'itération, le pas implicite, les cibles et expressions calculées, la réinitialisation par branchement vers l'instruction FAIRE et la valeur de `V` à la sortie.
- Références normatives : EXI-0001, EXI-0011, EXI-0015, EXI-0046 ; VM-1402 ; LSE15 §5-2, §5-5.1, §5-5.2, §5-5.3 (et Remarque 2), §5-5.4 (Remarque 2) ; INRDP75, chapitre 4 (§4-1, §4-2) et l. 3064 (ligne vide de fin de boucle) ; IMP-0104, IMP-0108 ; §5/IMP-0106 (résolution d'un numéro de ligne).
- Statut : retenu. La gestion de la pile des contextes de boucle (phase, ligne de fin effective, désimbrication en cas de sortie extraordinaire) relève du chapitre État d'exécution (§6, IMP-0151, IMP-0152) ; l'ordre de génération relève du chapitre Compilateur (§6).

**IMP-0277** — *FAIRE sans variable contrôlée, sous le profil `LSE F4` : ouverture 0x12*
- Sujet : le T1600 de Victor Duruy admet `FAIRE n1 TANT QUE c`, sans `POUR` : `105 FAIRE 105 TANT QUE TEM()-Q<4` (4EME - SINUS, `numerisation0035.pdf` et `numerisation0043.pdf`, sorties de `PERFØRER`), une attente de quatre secondes entre deux tracés. MEMENTO82, LAFOND83 et NOYELLE79, d'après Éric (pages non versées), attestent la même forme ; MEMENTO82 écrit `FAIRE n1 [POUR idn←ea1 [PAS ea2]] TANT QUE e?` et précise que la condition est testée à l'entrée de la boucle. INRDP75 §4-2 et LSE15 §5-5.1 exigent `POUR V←v1`. L'ouverture 0x09 consomme `n1` et `v1` et porte un identifiant de symbole : elle ne représente pas cette forme.
- Choix :
  - **Syntaxe (profil `LSE F4`).** `FAIRE n1 [POUR V←v1 [PAS p]] TANT QUE c` : `POUR` est facultatif, et `PAS` n'existe qu'à l'intérieur de `POUR`. `FAIRE n1 PAS p TANT QUE c` est donc rejeté (C 32). Avec `JUSQUA`, `POUR` reste obligatoire, `JUSQUA` comparant la variable à sa limite : `FAIRE n1 JUSQUA v2` est rejeté (C 32).
  - **Représentation.**

```text
[sous-séquence de n1]
OUVERTURE SANS VARIABLE 0x12
[sous-séquence de la condition] TEST TANT QUE 0x10
[corps : instructions qui suivent sur la même ligne, puis lignes suivantes jusqu'à n1]
```

    L'élément 0x12 n'a aucune charge utile et consomme `n1`. Il n'y a pas d'élément de pas : l'élément qui suit 0x12 est le premier de la condition.
  - **Exécution.** L'élément 0x12 détermine la ligne de fin effective et empile un contexte de boucle comme 0x09, sans variable. La fin d'itération reprend à l'élément qui suit 0x12, ce qui réévalue la condition. Le reste est le mécanisme de `FAIRE … TANT QUE` (IMP-0112, LSE15 §5-5.3), la variable en moins : condition testée à l'entrée de la boucle (MEMENTO82, d'après Éric), de sorte qu'une condition fausse à l'arrivée saute le corps, sortie ordinaire vers la ligne qui suit `n1`, désimbrication, réinitialisation par branchement vers la ligne FAIRE.
  - **Autres profils.** Sous T1600 version 4 et MITRA 15, `FAIRE n1 TANT QUE c` est rejeté (C 32) : INRDP75 et LSE15 exigent `POUR`.
- Alternatives envisagées :
  1. Réutiliser 0x09 avec un identifiant réservé — écarté : il faudrait compiler une valeur `v1` fictive, soit au moins trois octets de plus par boucle, et un élément de pas sans effet.
  2. Une variable cachée, la forme étant compilée comme `POUR` — écarté : un symbole sans nom dans la table, et le décompilateur ne pourrait plus restituer le texte écrit (EXI-0003).
  3. Admettre aussi `FAIRE n1 JUSQUA v2` ou `FAIRE n1 PAS p TANT QUE c` — écarté : ni l'un ni l'autre n'a d'objet sans variable, et la syntaxe de MEMENTO82 imbrique `PAS` dans `POUR`.
  4. Laisser `SINUS` 105 refusée — écarté : une forme attestée de première main sur la machine serait refusée (IMP-0276).
- Justification : un sous-code de contrôle distinct suit le précédent de 0x0A et 0x0E (IMP-0112, alternative 5) : l'effet sur la pile est fixé par le seul sous-code, et le décompilateur le lit directement. La disposition physique continue de coïncider avec l'ordre d'évaluation, et le mécanisme de corrélation par l'état d'exécution reste celui d'IMP-0112, en conformité avec EXI-0001.
- Impact mémoire : un octet d'ouverture au lieu de trois, sans élément de pas ; pas de champ supplémentaire dans le contexte de boucle.
- Impact disque : sans objet ; la version de RCI-S ne change pas, le sous-code n'apparaissant que dans un fichier-programme de `LSE F4`, refusé par les autres profils (F 100, IMP-0276).
- Impact performances : nul pour les autres formes de FAIRE ; une évaluation de la condition par itération.
- Impact historique : les programmes de Victor Duruy qui emploient cette forme se compilent et s'exécutent sous `LSE F4`, et se restituent à l'identique.
- Tests : `tests/representation/representation_rcim.c` (V17) ; `composants_compilateur_t1600`, `composants_decompilateur`, `composants_interprete`, `composants_moniteur`, `composants_bibliotheque`.
- Références normatives : EXI-0001, EXI-0003, EXI-0011, EXI-0015, EXI-0061 ; relevé T1600 de Victor Duruy, §3 ; INRDP75 §4-1, §4-2 ; LSE15 §5-5.1, §5-5.3 ; MEMENTO82, annexe I (C 32, C 34), et sa description de FAIRE, d'après Éric ; LAFOND83 et NOYELLE79, d'après Éric ; IMP-0112, IMP-0151, IMP-0180, IMP-0276.
- Statut : retenu (2026-11-01, point 1 de la liste des modifications vers le LSE de Victor Duruy ; syntaxe et test à l'entrée précisés par Éric d'après MEMENTO82, LAFOND83 et NOYELLE79).

**Couverture :** OUVERTURE (0x09), PAS (0x0A ou 0x0E) et TEST (0x0F ou 0x10), portés exclusivement par la ligne FAIRE, et réutilisation des formats valeur/référence existants pour `n1`, `v1`, `p`, `v2` et la condition ; la ligne `n1` ne porte physiquement rien de spécifique à la boucle. **Couvert, sans violation d'EXI-0001.**

#### 4.8.3 RETOUR EN ea (sortie extraordinaire de procédure)

Le corpus (§7.5, SORTIE « EXTRAORDINAIRE » D'UNE PROCÉDURE) : « il faut utiliser la forme RETOUR EN ea, ea étant une expression arithmétique dont la valeur [...] donne le numéro de la ligne vers laquelle il faut revenir ». Le retour normal (RETOUR / RESULTAT) reprend au point d'appel ; RETOUR EN reprend à une ligne différente, dans le contexte de l'appelant.

**Couverture :** sous-codes 0x04 (retour normal), 0x05 (RETOUR EN constante), 0x06 (RETOUR EN calculée), §4.7. **Couvert.**

Un point est cependant signalé pour le chapitre consacré à l'interpréteur : RETOUR EN ne se contente pas de modifier la position d'exécution comme ALLER EN, il doit aussi provoquer la sortie du cadre d'activation courant (dépilement), avant que la nouvelle position ne soit appliquée dans le contexte de l'appelant. Ce comportement relève de l'état d'exécution, pas du format RCI-M lui-même, qui n'a besoin de distinguer que le sous-code de l'opération (0x04/0x05/0x06 sont physiquement suffisants pour porter cette distinction).

#### 4.8.4 SI ⟨expr. booléenne⟩ ALORS ⟨instruction⟩ {SINON ⟨instruction⟩}

##### 4.8.4.1 Rappel historique — deux formes distinctes

Le corpus (LSE15 §5-2) documente explicitement **deux formes** de l'instruction conditionnelle, et non une seule :

1. `SI condition ALORS i1` (**sans SINON**) — « si la condition est vérifiée, i1 est exécuté et l'on passe à l'instruction suivante [...]. Si la condition n'est pas vérifiée, i1 n'est pas exécuté et l'on passe à l'instruction suivante » ;
2. `SI condition ALORS i1 SINON i2` (**avec SINON**) — « si la condition est vérifiée, i1 est exécuté et l'on passe à l'instruction suivante. Si la condition n'est pas vérifiée, on exécute i2 et l'on passe à l'instruction suivante ».

Les deux formes font l'objet d'une analyse de couverture dédiée ci-dessous (§4.8.4.2 et §4.8.4.3). Le corpus précise également que l'instruction conditionnelle ne peut pas s'étaler sur plusieurs lignes et que les branches i1/i2 peuvent être des instructions composées délimitées par DEBUT...FIN, ces deux règles s'appliquant indifféremment aux deux formes.

##### 4.8.4.2 Forme à deux branches — analyse de couverture

**Analyse de couverture.** Cette contrainte (une seule ligne) est directement compatible avec IMP-0101 (un bloc physique par ligne) : l'intégralité de l'instruction SI...ALORS...SINON, y compris ses blocs DEBUT...FIN, tient dans un unique bloc de séquence compilée. DEBUT...FIN ne nécessite aucun élément physique dédié : un bloc composé se compile simplement comme la concaténation séquentielle des séquences postfixées de chacune de ses instructions, dans l'ordre du texte — propriété déjà garantie par le parcours séquentiel de base (§9 du présent chapitre, ANN-K-0008). La structure conditionnelle elle-même se compile avec les branchements intra-séquence déjà définis :

```text
[expression booléenne]
[0x00 branchement conditionnel → étiquette SINON]
[séquence ALORS (i1), y compris DEBUT...FIN éventuel]
[0x01 saut inconditionnel → étiquette FIN]
SINON:
[séquence SINON (i2), y compris DEBUT...FIN éventuel]
FIN:
```

Le corpus documente également l'imbrication de SI dans une branche ALORS ou SINON (« L'instruction qui suit un ALORS ou un SINON peut être elle-même une instruction conditionnelle ») : cette imbrication ne pose aucune difficulté supplémentaire, chaque SI imbriqué générant à son tour la même paire de branchements intra-séquence, à des offsets distincts dans le même bloc. Le corpus règle par ailleurs explicitement l'ambiguïté de rattachement d'un SINON en cas d'imbrication sans instruction composée (LSE15 §5-2, Remarque : « chaque SINON est associé au SI immédiatement précédent qui n'avait pas encore de SINON associé »), illustrée par l'exemple `SI A=B ALORS SI XX=0 ALORS i1 SINON i2` où le SINON se rattache au second SI. Cette règle est une règle d'**associativité syntaxique**, résolue intégralement par l'analyseur à la compilation (elle détermine quelle paire d'éléments 0x00/0x01 encadre quelle sous-séquence) ; elle n'a strictement aucune conséquence sur la représentation physique elle-même, qui reste, dans tous les cas, la simple concaténation de paires 0x00/0x01 imbriquées décrite ci-dessus.

**Couverture :** entièrement couverte par les sous-codes 0x00/0x01 déjà définis (IMP-0105) et par la propriété de concaténation séquentielle. **Couvert.**

##### 4.8.4.3 Forme à une seule branche (sans SINON) — analyse de couverture

**Analyse de couverture.** La forme 1 (`SI condition ALORS i1`, sans SINON) ne modifie aucune des deux règles de couverture déjà établies (une seule ligne, IMP-0101 ; imbrication de blocs composés) : elle s'en déduit directement par absence de branche SINON. Physiquement, elle se compile avec le seul élément 0x00, dont la cible de branchement en cas de condition fausse n'est plus une étiquette SINON mais directement l'étiquette de fin — c'est-à-dire l'instruction qui suit `i1`, qu'elle se trouve sur la même ligne (après un `;`, cas également illustré au §4.8.2 pour FAIRE) ou sur la ligne suivante, conformément à la convention générale du corpus déjà citée pour la forme 2. Aucun élément 0x01 (saut inconditionnel) n'est émis, faute de séquence SINON à sauter :

```text
[expression booléenne]
[0x00 branchement conditionnel → étiquette FIN]
[séquence ALORS (i1), y compris DEBUT...FIN éventuel]
FIN:
```

Cette forme est physiquement indiscernable, pour le seul élément 0x00, d'un branchement conditionnel ordinaire (§4.7) ciblant l'instruction suivante — ce qui est exactement le comportement documenté (« l'on passe à l'instruction suivante »). Elle ne requiert donc, comme la forme 2, aucun sous-code supplémentaire : le sous-code 0x01 reste simplement inutilisé pour cette forme, sans que cela constitue une exception ou un cas particulier au niveau de RCI-M — c'est le compilateur, non RCI-M, qui décide de l'émission ou non de l'élément 0x01 selon la présence ou l'absence de SINON dans le texte source.

**Couverture :** entièrement couverte par le seul sous-code 0x00 déjà défini (IMP-0105). **Couvert.**

##### 4.8.4.4 Synthèse

Les deux formes de l'instruction conditionnelle (avec ou sans SINON) sont donc **toutes deux entièrement couvertes** par les primitives de branchement déjà établies au §4.7, sans qu'aucun sous-code, élément physique ou convention supplémentaire ne soit nécessaire. La seule différence entre les deux formes est la présence ou l'absence de l'élément 0x01 et de la sous-séquence SINON qui le précède, décision prise exclusivement par le compilateur à partir du texte source.

#### 4.8.5 SI ⟨expr. booléenne⟩ ALORS ⟨expression⟩ SINON ⟨expression⟩ (position d'expression)

##### 4.8.5.1 Rappel historique

Cette construction — nommée « sous-expression arithmétique conditionnelle » ou « sous-expression-chaîne conditionnelle » selon le type de résultat — est attestée par les deux sources historiques disponibles pour ce chapitre, indépendamment l'une de l'autre :

- **INRDP75**, PG26 ligne 30 : `CH←SI N>1 ALORS ' LETTRES ' SINON ' LETTRE '`, avec la remarque explicite (« l'expression conditionnelle est employée ici dans une expression chaîne, on peut procéder de même pour des expressions arithmétiques »).
- **LSE15** §3-3.1 (cas arithmétique, `SI condition ALORS ea1 SINON ea2`) et §4-4.1 (cas chaîne, `SI condition ALORS ec1 SINON ec2`, y compris imbriquée dans une concaténation) ; §7-4 pour son emploi après RESULTAT.

La forme expression impose systématiquement les deux branches : `ea1`/`ec1` et `ea2`/`ec2` doivent toutes deux être présentes, chacune produisant une valeur du même type que le résultat attendu de la sous-expression — à la différence de la forme instruction (§4.8.4), pour laquelle le corpus admet également une forme à une seule branche (`SI condition ALORS i1`, sans SINON, couverte au §4.8.4.3). Le corpus (LSE15 §4-4.1) précise par ailleurs, pour le cas chaîne, une règle de portée de l'analyseur — la branche SINON s'étend « de manière à obtenir l'expression-chaîne la plus longue possible » en l'absence de parenthèses — qui ne concerne que l'analyse syntaxique effectuée par le compilateur et n'a aucune incidence sur la représentation physique une fois les deux sous-expressions délimitées.

##### 4.8.5.2 Analyse de couverture

Aucun sous-code physique dédié n'est nécessaire : les primitives de branchement déjà définies pour la forme instruction (§4.7, sous-codes 0x00/0x01 ; IMP-0105) suffisent à l'identique, la seule différence étant que chaque branche dépose une valeur en pile (consommée par la suite de la séquence postfixée englobante, EXI-0011) plutôt que d'exécuter une instruction à effet de bord :

```text
[expression booléenne]
[0x00 branchement conditionnel → étiquette SINON]
[sous-séquence de la branche ALORS, produisant une valeur en pile]
[0x01 saut inconditionnel → étiquette FIN]
SINON:
[sous-séquence de la branche SINON, produisant une valeur en pile]
FIN:
```

Cette structure peut apparaître à n'importe quelle position où une sous-séquence de valeur est attendue (opérande d'un opérateur, argument de fonction, valeur affectée, valeur retournée par RESULTAT, élément de concaténation `!`), sans qu'aucune convention supplémentaire ne soit requise au-delà de celles déjà posées pour les opérateurs et fonctions (§4.11) : le compilateur, ayant résolu à la compilation le type (arithmétique ou chaîne) commun aux deux branches, émet une séquence physiquement indiscernable d'un branchement d'instruction ordinaire.

**Couverture :** entièrement couverte par les sous-codes 0x00/0x01 déjà définis (IMP-0105), au même titre que la forme instruction (§4.8.4), sans élément physique ni sous-code supplémentaire. **Couvert.**

#### 4.8.6 Synthèse de couverture

| Instruction | Sous-codes RCI-M mobilisés | Statut |
|---|---|---|
| ALLER EN ea | 0x02 (constante) / 0x03 (calculée) | Couvert |
| FAIRE n1 POUR V←v1 PAS p JUSQUA v2 | 0x09 (ouverture) + 0x0A ou 0x0E (pas explicite ou implicite) + 0x0F (test JUSQUA), ligne FAIRE uniquement, + corrélation par l'état d'exécution | Couvert, sans marque physique sur la ligne `n1` (EXI-0001) |
| FAIRE n1 POUR V←v1 PAS p TANT QUE ⟨cond⟩ | 0x09 (ouverture) + 0x0A ou 0x0E (pas explicite ou implicite) + 0x10 (test TANT QUE), ligne FAIRE uniquement, + corrélation par l'état d'exécution | Couvert, sans marque physique sur la ligne `n1` (EXI-0001) |
| RETOUR EN ea | 0x05 (constante) / 0x06 (calculée) | Couvert |
| RETOUR / RESULTAT | 0x04 (RETOUR, sans valeur) / 0x0B (RESULTAT, avec valeur) | Couvert |
| SI ⟨cond⟩ ALORS ⟨i⟩ (instruction, sans SINON) | 0x00 seul (pas de 0x01) | Couvert |
| SI ⟨cond⟩ ALORS ⟨i⟩ SINON ⟨i⟩, avec DEBUT...FIN (instruction) | 0x00 + 0x01, concaténation séquentielle | Couvert |
| SI ⟨cond⟩ ALORS ⟨e⟩ SINON ⟨e⟩ (expression, arithmétique ou chaîne) | 0x00 + 0x01, identiques à la forme instruction | Couvert |

---

### 4.9 Sous-format : Commentaire

#### 4.9.1 Rappel historique

Le corpus historique documente précisément le commentaire du L.S.E. :

- il est introduit par le caractère `*` ;
- il peut être placé au tout début d'une ligne (immédiatement après le numéro de ligne, sans espace obligatoire), ou après un point-virgule, c'est-à-dire après une instruction déjà écrite sur la même ligne [INRDP75, chapitre « Commentaire * » ; LSE15] ;
- le texte qui suit l'astérisque est ignoré par l'interpréteur et n'a **aucun effet sur l'exécution** ; il n'apparaît que lors du listage du programme (LISTER), c'est-à-dire lors de la décompilation ;
- **aucune instruction ne peut suivre un commentaire sur la même ligne** : le commentaire s'étend nécessairement jusqu'à la fin de la ligne (fin marquée par le caractère de fin de ligne, X-OFF, dans la documentation historique).

Ce besoin correspond exactement à la cinquième composante logique explicitement énumérée par VM-2401 (Tome III §6.1, DONNÉES MANIPULÉES) : « informations nécessaires à la décompilation », distincte des quatre catégories mobilisées par l'exécution (valeurs, références, opérations codées, informations de contrôle). Un commentaire est le cas le plus direct de cette composante : il n'est **d'aucune utilité à l'exécution**, mais sa suppression romprait EXI-0003 (décompilabilité) et EXI-0015 (restitution textuelle déterministe), puisqu'un commentaire est par nature un texte libre, non reconstructible à partir d'aucune autre information de la ligne (à la différence, par exemple, d'un mot-clé de langage, que le décompilateur peut régénérer sans le stocker).

#### 4.9.2 Encodage retenu

**IMP-0113** — *Représentation physique du commentaire*
- Sujet : représentation physique du texte de commentaire (`*` ... fin de ligne) dans RCI-M.
- Choix : un sous-code dédié de la catégorie « information de contrôle » (`RCIM_CAT_CONTROLE`), sous-code 0x0C, avec pour charge utile un octet de longueur `n` (0–255) suivi de `n` octets de texte — même principe de longueur préfixée que la forme en ligne de la chaîne littérale (§4.4, sous-code 0x03), un octet de longueur suffisant très largement compte tenu de la longueur de ligne pratiquée historiquement (l'instruction conditionnelle, à titre de comparaison, est explicitement bornée à 79 caractères par le corpus). Un commentaire n'est jamais candidat au partage de constantes (§7) : il compile toujours sous la forme longueur+texte la plus simple, sans indicateur de mode. L'élément COMMENTAIRE n'a **aucun effet lorsqu'il est rencontré par l'interpréteur** : celui-ci le traite comme un élément à ignorer, en avançant simplement le curseur de sa longueur (§9), exactement comme il le ferait pour tout autre élément inconnu de son point de vue sémantique.
- Alternatives envisagées : ne pas conserver le commentaire dans RCI-M et le rejeter dès la compilation (rejeté : romprait EXI-0003/EXI-0015, puisque LISTER doit pouvoir restituer le commentaire, or celui-ci est un texte arbitraire non reconstructible) ; conserver le commentaire dans une table séparée indexée par numéro de ligne plutôt que dans la séquence elle-même (rejeté : contredirait EXI-0010, qui impose une représentation unique commune à l'exécution et à la décompilation, et introduirait une deuxième structure à maintenir en cohérence avec la première) ; introduire une cinquième catégorie de tag sur 3 bits (rejeté : romprait la compacité de l'encodage à 1 octet de tag retenue en IMP-0102 pour un besoin marginal, un seul sous-code suffisant).
- Contrainte de compilation associée : conformément à la règle historique (« aucune instruction ne peut suivre sur la même ligne un commentaire »), le compilateur doit garantir qu'un élément COMMENTAIRE, lorsqu'il est émis, est **toujours le dernier élément** de la séquence compilée de la ligne — que le commentaire figure en tête de ligne (immédiatement après le numéro de ligne) ou après un point-virgule à la suite d'instructions déjà compilées. Cette contrainte est vérifiable à la compilation (le compilateur refuse toute instruction supplémentaire après un `*` sur la même ligne) et n'a besoin d'aucun marqueur physique supplémentaire dans RCI-M : `fin_sequence()` (§9) coïncide naturellement avec la fin du commentaire.
- Impact mémoire : 2 à 257 octets par commentaire selon sa longueur ; nul pour les lignes qui n'en contiennent pas — conforme à EXI-0016 (pas de coût pour les lignes sans commentaire).
- Impact disque : le commentaire devra être conservé dans RCI-S (chapitre dédié), car il n'est reconstructible par aucun mécanisme de restauration — à la différence des symboles filtrables identifiés en IMP-0104, un commentaire n'est jamais candidat à exclusion.
- Impact performances : coût nul à l'exécution (élément ignoré, jamais interprété sémantiquement) ; coût de décodage strictement identique à celui d'une chaîne littérale lors de la décompilation.
- Impact historique : couvre fidèlement la règle documentée (introduction par `*`, portée jusqu'à fin de ligne, absence d'effet sur l'exécution, nécessité pour LISTER).
- Références normatives : EXI-0003, EXI-0010, EXI-0015, EXI-0016, VM-2401 (DONNÉES MANIPULÉES : « informations nécessaires à la décompilation »), ANN-K-0014, ANN-K-0031, corpus historique (INRDP75, chapitre « Commentaire * » ; LSE15, chapitre « Commentaires »).
- Statut : retenu.

Table complémentaire à la catégorie « information de contrôle » (§4.7) :

| Sous-code | Signification | Charge utile physique |
|---|---|---|
| 0x0C | Commentaire (`*` jusqu'à fin de ligne) — aucun effet à l'exécution, nécessaire à la décompilation | 1 octet de longueur `n` (0–255) + `n` octets de texte |

(Le sous-code 0x0C de la table de §4.7 est affecté à cet usage ; le sous-code 0x0D y est affecté à la déclaration de procédure, chapitre Compilateur §13.2, IMP-0174 ; les sous-codes 0x0E à 0x10 y sont affectés aux éléments de pas et de test de la boucle FAIRE, §4.8.2, IMP-0112 ; le sous-code 0x11 y est affecté à la marque de tête de LIRE, §4.12, IMP-0183 ; la plage réservée restante est 0x12–0x3F.)

---

### 4.10 Sous-format : instruction AFFICHER et son format

#### 4.10.1 Rappel historique

L'instruction AFFICHER peut être suivie d'un **format**, noté entre crochets, sans blanc entre `AFFICHER` et le format [corpus, chapitre « Instruction de sortie. Format »] : `AFFICHER[format] liste_d_expressions`. Le format contient une suite de spécifications séparées par des virgules, de trois sortes :

- **spécification de libellé** : un texte entre apostrophes, éventuellement précédé d'un facteur de répétition fixe (1 à 99 sur Mitra 15, 1 à 62 sur T1600, voir la spécification de description ci-dessous) ou variable (`*`) ;
- **spécification de mise en page** : l'un des quatre caractères `/` (passage en début de ligne suivante), `X` (espace), `C` (retour en début de ligne courante), `L` (passage à la ligne suivante sans retour), éventuellement précédée d'un facteur de répétition fixe ou variable, 1 par défaut si omis ;
- **spécification de description** : `Fe.d` (décimal), `Ee.d` (mantisse + exposant) ou `U` (universelle, seule forme admettant l'affichage d'une chaîne), éventuellement précédée d'un facteur de répétition **fixe uniquement** (1 à 99 sur Mitra 15, 1 à 62 sur T1600 : « Ce facteur est limité à 62 sur T1600, à 99 sur Mitra 15 », INRDP75, note 7 du chapitre « Instruction de sortie. Format » ; la limite est contrôlée par le compilateur selon le profil d'implantation, chapitre Compilateur §8 bis, IMP-0180, la représentation couvrant les deux plages).

Le format et la liste des valeurs à afficher sont explorés **simultanément, de gauche à droite** : chaque spécification de description rencontrée s'applique à la valeur suivante de la liste (ou aux `r` valeurs suivantes si elle est répétée `r` fois) ; les spécifications de libellé et de mise en page ne consomment aucune valeur de la liste. S'il y a moins de spécifications de description que de valeurs, les valeurs excédentaires sont affichées selon la spécification implicite `U`.

**Point central signalé pour ce chapitre :** s'il n'y a **aucun format du tout**, toutes les valeurs sont affichées selon la spécification `U`, précédées d'un passage à la ligne suivante inséré automatiquement devant la première valeur — autrement dit, l'absence de format équivaut exactement au format explicite `[/,U]` **quant à l'effet produit**. `AFFICHER v1,v2,...` et `AFFICHER[/,U] v1,v2,...` doivent donc produire un affichage rigoureusement identique, sans qu'il soit pour autant nécessaire que RCI-M les représente par la même séquence physique (cf. §4.10.3, sous-code 0x38).

#### 4.10.2 Analyse de couverture

Le format d'AFFICHER est un sous-langage à part entière, avec libellés, mise en page et spécifications de description à facteurs de répétition fixes ou variables ; sa représentation physique doit être détaillée pour que RCI-M couvre effectivement l'instruction AFFICHER, au-delà du simple classement dans la catégorie « opérations d'entrée-sortie » (§4.6). Cette exigence est cohérente avec la responsabilité déjà identifiée en VM-1502 (Tome III §18.3, *Formateur des sorties*), dont l'interface `AnalyserFormat` prend en entrée une « description de format compilée » : le format n'est donc pas destiné à être laissé sous forme de texte à réanalyser à chaque exécution, mais bien à faire partie de la représentation compilée, conformément à EXI-0002 et EXI-0016.

#### 4.10.3 Représentation retenue

**IMP-0115** — *Représentation physique d'AFFICHER : marque de tête, format et valeurs en une suite plate d'éléments, affichage d'un tableau décidé à l'exécution*
- Sujet : éléments RCI-M nécessaires à la compilation d'AFFICHER, avec ou sans format explicite, de façon que l'instruction soit exécutable sans réanalyse de texte (VM-1502), que l'affichage d'un tableau entier ne dépende d'aucune information inconnue à la compilation (EXI-0001), et que chaque instruction AFFICHER soit restituable telle qu'elle a été écrite (EXI-0015).
- Choix : toute instruction AFFICHER commence par une **marque de tête** — 0x38 si elle n'a pas de format, 0x37 si elle a un format explicite —, suivie d'une **suite plate d'éléments physiques exécutés séquentiellement dans leur ordre d'apparition**, chaque sorte de spécification recevant un sous-code dédié de la catégorie « opération codée » (§4.6, plage 0x30–0x38) :

  | Sous-code | Spécification | Charge utile physique |
  |---|---|---|
  | 0x30 | Libellé | 1 octet **facteur** (bit 7 = indicateur : 0 = fixe, 1 = variable ; bits 6-0 = valeur 1–99 si fixe, ignorés et mis à 0 si variable — cf. IMP-0116) ; puis 1 octet longueur `n` + `n` octets de texte |
  | 0x31 | Mise en page `/` | 1 octet **facteur** (même format que 0x30, sans texte) |
  | 0x32 | Mise en page `X` | idem 0x31 |
  | 0x33 | Mise en page `C` | idem 0x31 |
  | 0x34 | Mise en page `L` | idem 0x31 |
  | 0x35 | Description | 1 ou 2 octets, forme compacte ou étendue — cf. IMP-0116 ; si type ∈ {F, E} : 1 octet `e` (0–99), 1 octet `d` (0–99) supplémentaires — LSE15 §6-2.3 : « avec 0 ≤ e < 100 et 0 ≤ d < 100 » ; INRDP75 §7-3 : « e et d sont compris entre 0 et 100 », formule ambiguë quant à la borne 100, levée par la formulation exacte de LSE15 (recoupement, sans application de la règle de préséance, §4.11.1) ; aucun exemple du corpus n'emploie plus d'un chiffre |
  | 0x36 | Affichage d'une valeur de la liste selon la spécification de description courante | 0 octet — consomme l'élément déposé en pile par la sous-séquence qui précède immédiatement cet élément (EXI-0011) ; si cet élément est une référence qui désigne, à l'exécution, un tableau, le tableau entier est affiché selon LSE15 §6-2.5 ; sinon la valeur est affichée |
  | 0x37 | Marque de tête d'une instruction AFFICHER à format explicite | 0 octet — aucun effet d'affichage |
  | 0x38 | Marque de tête d'une instruction AFFICHER sans format : format implicite `[/,U]` — voir IMP-0126 | 0 octet |

**Principe d'exécution : quatre rôles disjoints.**

1. **Marques de tête**, exécutées une fois au début de l'instruction : 0x37 n'a aucun effet d'affichage ; 0x38 insère le passage en début de ligne suivante que LSE15 §6-2.4 prévoit en l'absence de format, la spécification `U` étant de toute façon celle qui s'applique aux valeurs dépourvues de description.
2. **Éléments à effet immédiat**, sans consommation de valeur à afficher : le libellé (0x30) imprime son texte ; la mise en page (0x31–0x34) effectue son déplacement. Un facteur de répétition variable (`*`) dépile la valeur que la sous-séquence précédente vient de déposer, au lieu de l'afficher (LSE15 §6-2.6).
3. **Élément de sélection d'état**, sans aucun affichage par lui-même : la description (0x35) enregistre, dans un état local à l'exécution de l'instruction AFFICHER en cours, la spécification courante (type F/E/U, plus `e`/`d` le cas échéant) et un compteur de répétitions restantes.
4. **Élément consommateur** : 0x36 applique la spécification enregistrée à l'élément qu'il consomme et décrémente le compteur ; lorsque le compteur est épuisé, ou qu'aucune description n'a été rencontrée, c'est la spécification `U` qui s'applique (LSE15 §6-2.4). Un tableau entier consomme une seule spécification pour tous ses éléments (LSE15 §6-2.5).

**État du format d'une instruction à l'autre.** Le compilateur n'accepte une instruction AFFICHER que si elle ne comporte pas plus de spécifications de description, facteurs de répétition compris, que de valeurs à afficher (LSE15 §6-2.4 ; chapitre Compilateur, IMP-0178). À la fin de toute instruction acceptée, toutes les répétitions de description ont donc été consommées : l'état local revient de lui-même à son état initial (aucune répétition restante, spécification `U` par défaut), sans qu'aucune remise à zéro explicite soit nécessaire au début de l'instruction suivante.

**Ordre de compilation.** Le compilateur émet la marque de tête, puis les éléments **strictement dans l'ordre du texte source du format**, en faisant suivre chaque élément de description (0x35) directement des `r` paires `[valeur][0x36]` qu'il consomme (`r` étant sa répétition), avant de passer à la spécification de format suivante — conformément à l'exploration simultanée du format et de la liste « de gauche à droite » (LSE15 §6-2.4). Un libellé ou une mise en page rencontré entre deux descriptions s'intercale exactement à sa position dans le texte. Une fois le format entièrement épuisé, les valeurs restantes de la liste, s'il y en a, sont compilées à la suite, chacune suivie de son seul 0x36, sans aucun 0x35 supplémentaire.

- Le facteur de répétition **variable** (`*`) tire sa valeur, à l'exécution, de « l'expression arithmétique courante dans la liste d'affichage » (LSE15 §6-2.6) : le compilateur, qui traite la ligne en une seule passe, sait positionnellement quelle expression de la liste fournit ce facteur ; il compile cette expression normalement (elle est poussée en pile), puis l'élément de libellé ou de mise en page en mode « variable », qui dépile cette valeur comme facteur de répétition au lieu de l'afficher.
- **Absence de format.** `AFFICHER A,B` compile `[0x38] [séquence de A][0x36] [séquence de B][0x36]` : un seul octet de format pour l'ensemble de l'instruction (IMP-0126). `AFFICHER[/,U] A,B` produit le même effet observable, mais se compile `[0x37][0x31][0x35]` suivi des mêmes sous-séquences, et se restitue donc avec son format explicite.
- **Affichage d'un tableau entier.** Le compilateur ne peut pas savoir, en compilant `AFFICHER A`, si `A` désignera un tableau au moment de l'exécution : sous EXI-0001, la ligne `TABLEAU A[...]` peut être compilée avant ou après, et une déclaration TABLEAU ne prend effet qu'à l'exécution (§4.11.5, remarque générale). Un nom figurant seul dans la liste compile donc toujours une référence simple (§4.5, sous-code 0x00) suivie de 0x36, et c'est 0x36 qui détermine, d'après la catégorie effective du symbole à l'exécution, s'il affiche une valeur ou un tableau entier.
- **Frontières d'instructions.** Chaque instruction AFFICHER commençant par 0x37 ou 0x38, deux instructions successives restent distinctes dans la séquence compilée : `AFFICHER[F2.0] A;AFFICHER[F3.0] B` se compile `[0x37][0x35][A][0x36][0x37][0x35][B][0x36]` et `AFFICHER[F2.0,F3.0] A,B` se compile `[0x37][0x35][A][0x36][0x35][B][0x36]`. Le décompilateur restitue chacune telle qu'elle a été écrite.
- Alternatives envisagées :
  1. Compiler le format comme un bloc de données opaque attaché à un seul élément « AFFICHER », réanalysé à l'exécution — écarté : contredit VM-1502, qui attend une « description de format compilée », et EXI-0016, qui interdit de conserver sous une forme à réanalyser une information déjà décomposable sans coût supplémentaire.
  2. Unifier les quatre spécifications de mise en page sous un seul sous-code avec un octet de caractère — écarté : quatre sous-codes distincts coûtent le même unique octet de tag qu'un sous-code générique plus un octet de caractère, tout en rendant le décompilateur trivial.
  3. Un sous-code distinct pour l'affichage d'un tableau entier, choisi par le compilateur — écarté : le compilateur ne connaît pas la catégorie effective du symbole au moment de l'exécution (EXI-0001 ; §4.11.5) ; un tel sous-code serait faux pour toute instruction compilée avant la déclaration TABLEAU correspondante.
  4. Pas de marque de tête pour le format explicite — écarté : deux instructions AFFICHER successives à format explicite produiraient la même séquence qu'une instruction unique regroupant leurs formats et leurs listes ; le décompilateur ne pourrait restituer qu'une forme fusionnée, valide au sens d'EXI-0015 mais différente du texte saisi.
  5. Remettre explicitement à zéro l'état du format au début de chaque instruction — écarté : inutile dès lors que la règle de LSE15 §6-2.4 est appliquée à la compilation (IMP-0178), l'état revenant de lui-même à son état initial à la fin de toute instruction acceptée.
- Justification : cette représentation reproduit exactement l'exploration simultanée décrite par le corpus, en la rendant triviale à l'exécution (un parcours séquentiel d'éléments déjà résolus) ; elle respecte EXI-0011 (valeurs et facteurs variables poussés en pile avant l'élément qui les consomme), EXI-0016 (aucun octet pour un facteur implicite, résolu à la compilation), EXI-0001 (aucune dépendance à la catégorie d'un symbole déclarée sur une autre ligne) et EXI-0015 (chaque instruction AFFICHER restituée telle qu'elle a été écrite).
- Impact mémoire : 1 octet de marque de tête par instruction AFFICHER (0x37 ou 0x38) ; 1 à 2 octets par spécification de mise en page ou de description (IMP-0116) ; `n` octets par libellé ; 1 octet par valeur affichée (0x36) ; l'instruction sans format se réduit à 1 octet de format au total (IMP-0126).
- Impact disque : ces éléments se sérialisent en RCI-S sans transformation.
- Impact performances : parcours linéaire d'éléments déjà résolus, sans réanalyse de texte ; une consultation de la catégorie effective du symbole par 0x36 lorsque l'élément consommé est une référence simple.
- Impact historique : couvre les trois sortes de spécifications, les deux formes de facteur de répétition, l'exploration simultanée, le repli sur `U`, l'affichage d'un tableau entier avec une seule spécification, l'équivalence d'effet entre absence de format et format implicite `[/,U]`, et la restitution fidèle de chaque instruction.
- Références normatives : LSE15 §6-2 à §6-2.6 ; INRDP75, chapitre « Instruction de sortie. Format » ; EXI-0001, EXI-0002, EXI-0011, EXI-0015, EXI-0016 ; Tome III §18.2, §18.3 (VM-1502), §18.4 ; §4.5 (référence simple, sous-code 0x00) ; §4.11.5 (prise d'effet de TABLEAU à l'exécution) ; IMP-0116, IMP-0126 ; chapitre Compilateur, IMP-0178.
- Statut : retenu pour la représentation physique ; l'algorithme précis de formatage des nombres selon F/E/U (arrondi, alignement, gestion des espaces) et la mise en page d'un tableau entier relèvent du Formateur des sorties (VM-1502), chapitre distinct du Tome V.

**IMP-0116** — *Compaction en un seul octet des champs indicateur/facteur/type*
- Sujet : réduction du nombre d'octets de charge utile des sous-codes 0x30–0x35, dont les premiers octets (indicateur fixe/variable, valeur du facteur, type de description) laissaient une marge d'optimisation.
- Choix pour 0x30–0x34 (libellé, mise en page) : fusion de l'octet indicateur et de l'octet valeur en un **octet unique** — bit 7 comme indicateur (0 = fixe, 1 = variable), bits 6-0 comme valeur (0–127, dont la plage utile 1–99 est strictement respectée à la compilation ; les bits 6-0 sont ignorés par l'interpréteur, et mis à 0 par le compilateur, lorsque le bit 7 signale un facteur variable). Ce choix est sans perte : le nombre total d'états à représenter est de 99 (facteur fixe, valeurs 1 à 99) + 1 (facteur variable) = 100 états, très en-deçà des 256 valeurs disponibles sur un octet — la fusion ne sacrifie donc aucune information et n'entraîne aucune restriction par rapport au format en deux octets.
- Choix pour 0x35 (description) : la même fusion complète n'est **pas possible sans perte**. Le type de description a 3 valeurs (F, E, U) et le facteur de répétition en a 99 (1 à 99, plage confirmée par LSE15 pour les spécifications de description comme pour les libellés et la mise en page) ; le nombre total d'états à représenter est donc de 3 × 99 = 297, ce qui dépasse les 256 valeurs disponibles sur un octet. Une fusion stricte sur un seul octet imposerait donc soit de réduire artificiellement la plage du facteur de répétition en-deçà de 99 pour au moins un type, ce qui romprait la fidélité historique (un programme utilisant légitimement un facteur supérieur à la plage réduite deviendrait irreprésentable), soit de réduire le nombre de types représentables, ce qui est impossible (3 types distincts attestés). Une **forme compacte à extension optionnelle** est retenue à la place :
  - octet 0 : bits 7-6 = type (`00` = F, `01` = E, `10` = U, `11` = réservé) ; bits 5-0 = facteur de répétition moins 1, si celui-ci est compris entre 1 et 63 (soit les valeurs 0 à 62 codées sur 6 bits) ; la valeur `63` (0b111111) dans ces mêmes bits 5-0 sert de **marqueur d'extension**, signalant que le facteur réel dépasse 63 ;
  - octet 1, présent **uniquement** si le marqueur d'extension est positionné : facteur de répétition exact, codé directement sur 1 octet (valeur 1–99, la plage réduite 1–63 n'étant alors plus pertinente puisque ce second octet porte la valeur complète).
  - Ce codage préserve intégralement la plage 1–99 pour les trois types sans exception, au prix d'un octet supplémentaire dans le seul cas, marginal en pratique, d'un facteur de répétition supérieur à 63 associé à une spécification de description — cas nettement plus rare qu'un facteur de répétition de libellé ou de mise en page élevé (ces derniers servant typiquement à la mise en page de tableaux ou de séparateurs, ce qui n'est pas l'usage habituel d'une spécification de description).
- Alternatives envisagées : réduire uniformément la plage du facteur de répétition des spécifications de description à 1–63 pour permettre une fusion sans exception (rejeté : romprait silencieusement la fidélité historique au plafond documenté de 99, ce que le conditionnement interdit — une propriété historique ne peut être modifiée pour la commodité de l'implémentation) ; conserver deux octets systématiques pour 0x35, quelle que soit la valeur du facteur (rejeté : n'exploite pas la faible probabilité d'un facteur élevé pour une spécification de description, alors que la forme compacte à extension atteint le même résultat pour l'immense majorité des cas réels sans jamais restreindre la plage historique).
- Justification : la fusion de 0x30–0x34 est un gain net, sans aucune contrepartie. La forme compacte à extension de 0x35 respecte le principe de compacité (EXI-0016) au sens fort — elle ne dépense un octet supplémentaire que lorsque c'est réellement nécessaire — tout en respectant la fidélité historique (aucune restriction du facteur de répétition documenté), conformément au principe directeur du conditionnement (§3) qui place la fidélité historique avant l'économie de mémoire.
- Impact mémoire : 0x30–0x34 passent de 2 à 1 octet dans le cas fixe (aucun changement dans le cas variable, déjà à 1 octet) ; 0x35 passe de 2 octets systématiques à 1 octet dans le cas très majoritaire (répétition ≤ 63), 2 octets seulement pour une répétition de 64 à 99.
- Impact disque : la forme compacte se sérialise directement en RCI-S, à l'identique.
- Impact performances : décodage par un simple masquage de bits (`& 0x80`, `& 0x7F` pour 0x30–0x34 ; `& 0xC0`, `& 0x3F` pour 0x35), coût strictement inférieur à la lecture de deux octets séparés — gain net, y compris dans le cas étendu de 0x35.
- Impact historique : aucun — la plage 1–99 du profil Mitra 15 est intégralement préservée pour les trois types de spécification de description (la plage 1–62 du profil T1600, contrôlée par le compilateur, chapitre Compilateur §8 bis, y est incluse), contrairement à toute solution de fusion stricte sur un octet.
- Références normatives : corpus historique, LSE15 §6-2.1 et §6-2.3 (plafond 1–99 du facteur de répétition, y compris pour les spécifications de description) ; EXI-0016.
- Statut : retenu.

**IMP-0126** — *Sous-code unique pour le format implicite `[/,U]` d'AFFICHER*
- Sujet : le format implicite (absence de format) produit un effet strictement identique à celui du format explicite `[/,U]`, mais celui-ci est un motif entièrement fixe — aucune variation d'aucune sorte —, répété potentiellement dans une large proportion des instructions AFFICHER d'un programme. Une représentation plus compacte que la paire `[0x31,0x35]` est-elle possible pour ce cas particulier ?
- Choix : un sous-code dédié, 0x38 (catégorie « opération codée »), 0 octet de charge utile, placé en tête de toute instruction AFFICHER sans format (marque de tête, IMP-0115) et produisant à lui seul l'effet complet du couple `[0x31,0x35]` : passage à la ligne, les valeurs étant affichées selon la spécification `U`. Aucune modification n'est nécessaire au mécanisme déjà en place pour les valeurs excédentaires d'une liste de spécifications épuisée (§4.10.3) : une fois 0x38 exécuté, la seconde valeur d'une liste `AFFICHER A,B` retombe sur ce même mécanisme, sans qu'aucun second élément de format ne soit compilé, quelle que soit la longueur de la liste.
- Alternatives envisagées : faire porter à 0x38 la répétition explicite du couple `[0x31,0x35]` pour chaque valeur de la liste au-delà de la première (rejetée : inutilement coûteuse, puisque le mécanisme d'épuisement de spécification déjà retenu produit exactement le même effet observable sans aucun octet supplémentaire) ; conserver `[0x31,0x35]` sans introduire de sous-code dédié (rejetée : ce motif est entièrement fixe et vraisemblablement très fréquent, ce qui en fait un candidat direct au principe déjà appliqué en IMP-0123/IMP-0125 — la représentation la plus compacte pour le cas le plus courant).
- Justification : ce choix ramène le coût du format implicite de 4 octets (deux éléments compacts, IMP-0116) à 1 octet, quelle que soit la longueur de la liste de valeurs — un gain net et sans perte, le format explicite `[/,U]` restant disponible à l'identique pour qui l'écrit explicitement. Le sous-code est prélevé sur le seul emplacement réservé de la catégorie « opération codée », placé en 0x38, immédiatement à la suite de la série 0x30–0x37 déjà consacrée à AFFICHER ; ce prélèvement sature entièrement cette catégorie (§4.11.8).
- Impact mémoire : économie de 3 octets par instruction AFFICHER sans format, quel que soit le nombre de valeurs affichées — gain cumulatif potentiellement important sur un programme pédagogique typique, où ce cas est vraisemblablement majoritaire.
- Impact disque : sans objet (RCI-M).
- Impact performances : un élément de moins à décoder pour toute instruction AFFICHER sans format.
- Impact historique : neutre — l'effet produit est rigoureusement identique à celui du format explicite `[/,U]`, seule la représentation physique du cas implicite change.
- Références normatives : §4.10.3 (IMP-0115, mécanisme d'épuisement de spécification) ; EXI-0016 ; IMP-0116, IMP-0123, IMP-0125 (précédents de compaction du cas le plus fréquent).
- Statut : retenu.

#### 4.10.4 Couverture

| Élément du corpus | Sous-code(s) RCI-M | Statut |
|---|---|---|
| Spécification de libellé, facteur fixe ou variable | 0x30 | Couvert |
| Spécifications de mise en page `/`, `X`, `C`, `L`, facteur fixe ou variable | 0x31–0x34 | Couvert |
| Spécifications de description F/E/U, facteur fixe uniquement | 0x35 | Couvert |
| Affichage d'une valeur selon la spécification courante | 0x36 | Couvert |
| Affichage d'un tableau entier selon une seule spécification (LSE15 §6-2.5) | 0x36 consommant une référence simple (§4.5, 0x00), catégorie déterminée à l'exécution | Couvert au niveau RCI-M ; itération renvoyée à VM-1502/Tome III §18.4 |
| Absence de format = format implicite `[/,U]` | 0x38, marque de tête (IMP-0126) | Couvert |
| Instruction à format explicite | 0x37, marque de tête (IMP-0115) | Couvert |
| Pas plus de spécifications de description que de valeurs (LSE15 §6-2.4) | Contrôle à la compilation (chapitre Compilateur, IMP-0178), aucun élément physique | Couvert |
| Moins de spécifications de description que de valeurs | Comportement par défaut de l'interpréteur (spec U), aucun élément physique | Couvert |

---

### 4.11 Catalogue complet des opérations codées

#### 4.11.1 Méthode et source

Les §4.6 à §4.10 établissent le principe de représentation des opérations codées et détaillent le seul cas d'AFFICHER. Ce qui suit énumère, catégorie par catégorie, **tous** les sous-codes nécessaires pour représenter l'ensemble des opérations, fonctions et instructions attestées, afin que RCI-M puisse servir de document de référence pour le compilateur sans renvoi ultérieur pour une opération manquante.

**Avertissement sur la source.** La grammaire formelle `LSE15Mitra.g4` (référencée `SRC-0004` dans la reconstruction documentaire) est un document de **reconstruction**, non une source historique au sens du Référentiel normatif (Niveau A/B, §22 du conditionnement) : elle est bâtie exclusivement à partir des deux manuels historiques INRDP75 et LSE15, et ne couvre que la variante Mitra 15 du langage (elle ne modélise donc, par construction, aucune des spécificités T1600 — cf. remarque ci-dessous). Elle a servi de point de départ méthodique pour ne manquer aucune construction syntaxique, mais chacune de ses règles a été confrontée individuellement aux sources historiques (INRDP75, LSE15, ainsi que NOYELLE89 pour ce qui relève de son propos) avant d'être traduite en sous-code. Cette confrontation n'est toutefois pas uniforme sur tout le catalogue : pour les constructions communes aux deux machines et à Mitra 15 spécifiquement, la grammaire a pu servir de recoupement méthodologique en plus des deux manuels ; pour les spécificités **T1600** en revanche (`$`, absence du symbole `@`, fonctions SH/CH/TH/SGN, etc. — §4.11.2), la grammaire, limitée à Mitra 15, ne peut par construction ni les confirmer ni les infirmer, et leur vérification technique repose donc exclusivement sur INRDP75 (seule source du corpus à documenter le T1600), sans recoupement possible par la grammaire de reconstruction. NOYELLE89 (Y. Noyelle, « La saga du LSE et de sa famille », EPI, 1988) est un récit historique et biographique de la conception du langage, et non un manuel de référence syntaxique : il ne contient aucun détail technique sur le format de LIRE, sur la construction SI en position d'expression, ni plus généralement sur la syntaxe précise des instructions traitées dans ce chapitre ; sa consultation n'a donc ni confirmé ni infirmé aucun des points ci-dessous, dont la vérification technique repose exclusivement sur INRDP75 et LSE15. Une seule construction de cette grammaire n'a **pas** été retrouvée dans INRDP75 ni dans LSE15 et est donc traitée comme une évolution ultérieure possible du langage, non comme un fait historique (l'opérateur `NON`, sous-code 0x18, absent à la fois du corpus et de la grammaire, est signalé à part au §4.11.3) :

1. **`LIRE` avec format entre crochets** (règle `format_lire`). Le corpus historique documente uniquement `LIRE` suivi d'une simple liste de références, sans aucune mention d'un format analogue à celui d'AFFICHER.

Conformément à la règle de traçabilité (§15 du conditionnement — aucune modification silencieuse), cette construction n'est pas supprimée du catalogue mais explicitement signalée non historique dans la colonne « Notes » de la table concernée (§4.11.6 pour LIRE), les sous-codes correspondants restant réservés au cas où le compilateur souhaiterait, à titre d'extension déclarée et non silencieuse, prendre en charge cette forme évoluée.

**`SI ... ALORS ... SINON` comme alternative d'expression — construction historiquement attestée.** Cette construction est attestée par les deux sources historiques disponibles, indépendamment l'une de l'autre : INRDP75 (PG26, ligne 30, `CH←SI N>1 ALORS ' LETTRES ' SINON ' LETTRE '`) commente explicitement que « l'expression conditionnelle est employée ici dans une expression chaîne (on peut procéder de même pour des expressions arithmétiques) » ; LSE15 la nomme et la définit formellement, à la fois pour les expressions arithmétiques (§3-3.1, « sous-expression arithmétique conditionnelle », `SI condition ALORS ea1 SINON ea2`, avec les exemples `B[I,J]←SI I=J ALORS 1 SINON B[I,J]/DETER` et `FONCT←SI X#0 ALORS SIN(X)/X SINON 1`) et pour les expressions-chaîne (§4-4.1, « sous-expression-chaîne conditionnelle », y compris imbriquée dans une concaténation : `SCH(CA,J+3,'∧')!(SI J=0 ALORS '∧' SINON 'S∧')!'.'`) ; LSE15 §7-4 l'atteste également en position de RESULTAT (`RESULTAT SI N=0 ALORS 1 SINON N*&FACT(N-1)`). Cette construction est donc traitée comme n'importe quelle autre construction historique du présent chapitre ; sa couverture physique est détaillée au §4.8.5.

**Règle de préséance en cas de désaccord entre INRDP75 et LSE15.** Les deux manuels historiques sont en désaccord factuel sur plusieurs points techniques précis — plage de l'argument d'EQC sur Mitra 15 (§4.11.4, sous-code 0x26), liste des mots réservés (chapitre Compilateur §8 bis), arrondi ou troncature des indices de tableau (chapitre État d'exécution §3 bis), ce dernier désaccord étant résolu par recoupement avec deux sources tierces, LAFOND83 et l'implémentation LSE 8086 version IV de 1988, qui confirment l'arrondi de LSE15. Les tomes de conception définissent les deux profils d'implantation d'après INRDP75 : « version 4 sur T1600 et version 8 sur MITRA 15 » (EXI-0061 ; Tomes I-II §20.3 ; INRDP75, « Remarque sur la présentation du fascicule »). INRDP75, dont la rédaction « a été terminée en décembre 1974 », est donc la source de référence de chacun des deux profils. LSE15 est la « version mitra 15 » du manuel, imprimée par l'École d'Application des Transmissions en 1978 ; il décrit un système Mitra 15 dont la version n'est pas indiquée, sans que rien permette d'établir s'il s'agit de la version 8 décrite par INRDP75, d'une version antérieure ou d'une version ultérieure. En conséquence, et à titre de règle méthodologique : **en cas de désaccord factuel entre INRDP75 et LSE15 sur un point technique précis, non résolu par recoupement avec une troisième source, la description d'INRDP75 prévaut pour le profil qu'elle définit** ; LSE15 reste une source complémentaire, déterminante lorsque INRDP75 est muet (par exemple l'ordre d'évaluation des expressions de la boucle FAIRE, §4.8.2). Chaque application de cette règle est explicitement signalée au point concerné plutôt que silencieuse (§15 du conditionnement). Cette règle est une décision de reconstruction documentaire relevant, par sa portée, du Référentiel normatif (Niveau A) plutôt que du seul présent chapitre d'implémentation ; elle est consignée ici pour la traçabilité immédiate de ce chapitre, mais devrait être répercutée dans le Référentiel normatif lui-même (catalogue des décisions `DEC`, distinct des décisions `IMP` de ce chapitre, cf. §14 du conditionnement).

**Précision sur les caractères `↑` et `←` (exponentiation, affectation).** Les terminaux utilisés dans les années 1970 pour l'usage du L.S.E. affichaient les codes ASCII 94 et 95 respectivement par les glyphes `↑` (flèche vers le haut) et `←` (flèche vers la gauche), et non par les glyphes `^` (accent circonflexe) et `_` (souligné) qu'affiche un terminal moderne pour ces deux mêmes codes — évolution postérieure de la norme d'affichage des terminaux, sans rapport avec le langage lui-même. Ce fait est confirmé par INRDP75, Annexe V, « Équivalents numériques des caractères (code ASCII) » (p. 201), dont la table recense explicitement le code 94 comme `↑` et le code 95 comme `←`. Il ne s'agit donc pas d'une particularité de L.S.E. : le langage utilise deux opérateurs fixés par leur code ASCII (94 pour l'exponentiation, 95 pour l'affectation), quel que soit le glyphe que le terminal employé choisit d'en afficher. Pour la présente documentation, destinée à être lue sur un terminal ou un éditeur moderne, chaque occurrence de ces deux opérateurs est identifiée sans ambiguïté par son code ASCII, son glyphe historique et son glyphe moderne : **exponentiation = ASCII 94, historiquement affiché `↑`, affiché `^` sur un terminal moderne** ; **affectation = ASCII 95, historiquement affiché `←`, affiché `_` sur un terminal moderne**. Les tables et exemples du présent chapitre conservent la notation `↑`/`←`, fidèle à l'affichage historique déjà utilisée dans les citations du corpus, chaque première occurrence dans une table rappelant le code ASCII correspondant. Pour l'opérateur d'affectation (§4.11.3), les notations `←`, `_` et le glyphe moderne du code ASCII 95 désignent rigoureusement le même opérateur — non des graphies distinctes issues d'une dégradation de numérisation des documents historiques.

Par ailleurs, l'annexe consacrée aux spécificités T1600/Mitra 15 (INRDP75, Annexe III, « Spécificités MITRA 15 - T1600, problèmes de compatibilité ») révèle plusieurs opérations propres à une seule des deux machines historiques, absentes de la grammaire reconstruite. Elles sont intégrées ci-dessous, chacune annotée de la version concernée.

**IMP-0117** — *Catalogue verrouillé des opérations codées*
- Sujet : liste exhaustive et numérotation définitive des sous-codes des catégories « opération codée » couvertes par le présent chapitre (arithmétique, relationnel/logique, valeur, appel/déclaration/fichiers, entrée-sortie), y compris les opérations propres à une seule version historique.
- Choix : les tables des §4.11.2 à §4.11.6 constituent, avec celles déjà données aux §4.6, §4.7 et §4.10, le catalogue verrouillé des opérations codées de RCI-M : chaque sous-code utile y reçoit une affectation définitive, assortie d'une colonne « Notes » signalant toute restriction de version ou tout écart par rapport aux sources historiques strictes.
- Justification : le conditionnement (§20, §21) exige une documentation complète des structures de données avant codage ; un compilateur ne peut être écrit à partir d'un catalogue partiel sans revenir constamment à la spécification. L'exhaustivité est vérifiée par confrontation systématique de la grammaire reconstruite avec les sources historiques du corpus documentaire de ce chapitre (INRDP75, LSE15 ; NOYELLE89 consulté mais sans contenu technique directement applicable, cf. §4.11.1), chaque écart étant explicitement documenté plutôt que silencieusement absorbé.
- Impact mémoire : aucun changement par rapport aux principes déjà posés — chaque opération reste sur 1 octet, sans charge utile sauf exceptions déjà signalées (AFFICHER, LIRE ; APPEL, IMP-0121 et IMP-0177 ; les neuf sous-codes à arité variable, IMP-0122).
- Impact disque : sans objet.
- Impact performances : sans objet — la numérotation définitive ne change rien au coût d'exécution, déjà O(1) par élément.
- Impact historique : couvre, sans exception connue, l'ensemble des opérateurs, fonctions élémentaires, fonctions chaînes, opérateurs relationnels et logiques, instructions et spécificités de version attestés par le corpus, y compris `SI...ALORS...SINON` en position d'expression (§4.8.5), historiquement attestée ; les constructions non attestées — LIRE avec format (§4.11.6) et l'opérateur `NON` (§4.11.3) — sont explicitement marquées comme telles, jamais présentée comme un fait historique ; le désaccord factuel entre INRDP75 et LSE15 sur la plage de EQC (§4.11.4, 0x26) est résolu conformément à la règle de préséance §4.11.1, plutôt que silencieusement absorbé dans un choix arbitraire.
- Références normatives : corpus historique (INRDP75, LSE15, NOYELLE89), grammaire reconstruite `LSE15Mitra.g4` (source méthodologique, non normative, limitée à Mitra 15) ; INRDP75 Annexe III (spécificités et compatibilité T1600/Mitra 15) ; EXI-0013 (identifiabilité des opérations) ; §5.7 Tome IV et §26 du conditionnement (ne jamais présenter un choix d'implémentation comme un fait historique, ni l'inverse).
- Statut : retenu.

#### 4.11.2 Opérations arithmétiques (0x00–0x0F, communes) et fonctions T1600 (0x3C–0x3F)

Six opérateurs et dix fonctions élémentaires communes aux deux machines historiques, soit exactement seize sous-codes remplissant 0x00–0x0F sans reste. Quatre fonctions supplémentaires, propres au seul T1600, sont logées dans la plage d'extension (§4.11.8) faute de place restante dans 0x00–0x0F.

| Sous-code | Opération | Notes |
|---|---|---|
| 0x00 | Addition binaire `+` | |
| 0x01 | Soustraction binaire `-` | |
| 0x02 | Changement de signe (`-` unaire) | opérande unique |
| 0x03 | Multiplication `*` | |
| 0x04 | Division `/` | |
| 0x05 | Exponentiation (`↑`, ASCII 94 — affiché `^` sur un terminal moderne, cf. §4.11.1) | associativité à gauche, comme toute opération de même rang (`a↑b↑c = (a↑b)↑c` ; LSE15 §3-3.4 : « Dans une suite d'opérations de même valeur hiérarchique où ne figure aucune parenthèse, les calculs sont effectués de gauche à droite » ; INRDP75 : « À priorité égale, les opérations se font de gauche à droite »), résolue à la compilation par l'ordre de la séquence postfixée — aucune conséquence physique |
| 0x06 | RAC(E) — racine carrée | |
| 0x07 | ABS(E) — valeur absolue | |
| 0x08 | EXP(E) — exponentielle | |
| 0x09 | LGN(E) — logarithme népérien | |
| 0x0A | SIN(E) — sinus (radians) | |
| 0x0B | COS(E) — cosinus (radians) | |
| 0x0C | ATG(E) — arc tangente | |
| 0x0D | ENT(E) — partie entière | |
| 0x0E | ALE(E) — valeur pseudo-aléatoire | |
| 0x0F | TEM — temps écoulé depuis 0h | sans argument ; s'écrit sans parenthèses sur T1600 (`TEM`) et avec parenthèses sur Mitra 15 / LSE15 (`TEM()`), et sous `LSE F4` avec ou sans parenthèses, restitué `TEM()` (chapitre Compilateur, IMP-0189) — différence purement syntaxique, absorbée par l'analyseur du compilateur, sans conséquence sur ce sous-code |
| 0x3C | SH(E) — sinus hyperbolique | **T1600 uniquement** [INRDP75, Annexe III §1] ; absente sur Mitra 15 (incompatibilité déclarée) |
| 0x3D | CH(E) — cosinus hyperbolique | **T1600 uniquement**, mêmes conditions que SH |
| 0x3E | TH(E) — tangente hyperbolique | **T1600 uniquement**, mêmes conditions que SH |
| 0x3F | SGN(E) — signe d'une expression | **T1600 uniquement** [INRDP75, Annexe III §1] ; absente sur Mitra 15 |

#### 4.11.3 Opérations relationnelles, logiques, affectation et instructions T1600 UN/ZER (0x10–0x1B)

Six opérateurs relationnels, trois opérateurs logiques (dont `NON`, non attesté par le corpus), l'opérateur d'affectation et deux instructions propres au T1600 — les douze sous-codes de la plage sont tous affectés.

**Opérateur d'affectation.** L'affectation (`reference ← expression`) — l'opération la plus fréquente de tout programme L.S.E. — est représentée par le sous-code 0x19. Le caractère `←` correspond au code ASCII 95, affiché `_` (souligné) sur un terminal moderne (cf. §4.11.1) ; les deux notations sont donc rigoureusement le même opérateur.

**IMP-0118** — *Opérateur d'affectation*
- Sujet : représentation physique de l'affectation `reference ← expression`.
- Choix : sous-code 0x19, sans charge utile, consommant une référence (§4.5) et une valeur (déjà en pile, conformément à EXI-0011) déposées par les deux sous-séquences qui précèdent immédiatement cet élément.
- Justification : l'affectation est l'opération centrale du langage (corps de toute instruction simple, initialisation de boucle FAIRE) ; son absence du catalogue aurait rendu celui-ci non suffisant pour un compilateur, contrairement à l'objectif du présent chapitre. L'affectation `V←v1` effectuée par l'élément d'ouverture de boucle FAIRE (0x09, §4.8.2, IMP-0112) en partage exactement la sémantique, sans qu'aucun élément 0x19 ne soit compilé pour elle : l'ouverture porte elle-même l'identifiant de `V` et consomme `v1`.
- Impact mémoire : 1 octet par affectation.
- Impact disque : sans objet.
- Impact performances : consommation directe de pile, coût minimal.
- Impact historique : couvre l'opérateur d'affectation, code ASCII 95, quel que soit le glyphe utilisé pour son affichage (`←` historique, `_` moderne — cf. §4.11.1).
- Références normatives : corpus historique (grammaire, règle `affectation`) ; EXI-0011 ; §4.5 ; §4.8.2/IMP-0112.
- Statut : retenu.

| Sous-code | Opération | Notes |
|---|---|---|
| 0x10 | `=` égal | opérandes arithmétiques ou chaînes (comparaison caractère par caractère pour les chaînes) |
| 0x11 | `<` plus petit | idem |
| 0x12 | `<=` inférieur ou égal | idem |
| 0x13 | `#` différent | idem |
| 0x14 | `>` plus grand | idem |
| 0x15 | `>=` supérieur ou égal | idem |
| 0x16 | ET (conjonction) | opérandes booléens |
| 0x17 | OU (disjonction) | opérandes booléens |
| 0x18 | NON (négation) | opérande unique — **non attesté** : absent d'INRDP75, de LSE15 (qui ne connaissent que `ET` et `OU` dans les expressions booléennes) et de la grammaire reconstruite ; le compilateur ne le produit jamais ; sous-code conservé, comme les sous-codes de LIRE avec format (§4.11.6), pour ne rien supprimer silencieusement |
| 0x19 | **AFFECTATION** (`←`, ASCII 95 — affiché `_` sur un terminal moderne, cf. §4.11.1) | 0 octet — consomme une référence (§4.5) et une valeur déjà en pile (EXI-0011) ; l'affectation `V←v1` effectuée par l'élément d'ouverture de boucle FAIRE (0x09, §4.8.2, IMP-0112) en partage exactement la sémantique, sans élément 0x19 distinct |
| 0x1A | UN — met à 1 tous les éléments d'un tableau | **T1600 uniquement** [INRDP75, Annexe III §1] ; consomme une référence simple à un tableau (§4.5, sous-code 0x00) ; absente sur Mitra 15, où l'équivalent s'écrit par une boucle FAIRE explicite |
| 0x1B | ZER — met à 0 tous les éléments d'un tableau | **T1600 uniquement**, mêmes conditions que UN |

La précédence de `ET` sur `OU` (INRDP75 ; LSE15 §5), comme celle des opérateurs arithmétiques, est intégralement résolue à la compilation par l'ordre postfixé de la séquence (EXI-0011) ; aucune information de précédence n'est portée physiquement par ces sous-codes.

#### 4.11.4 Opérations de valeur : concaténation et fonctions chaînes (0x1C–0x27)

Un opérateur de concaténation et onze fonctions chaînes, communs aux deux machines historiques, soit exactement douze sous-codes — la plage est remplie sans reste.

| Sous-code | Opération | Arité | Charge utile physique | Résultat | Notes |
|---|---|---|---|---|---|
| 0x1C | `!` concaténation de chaînes | 2 (fixe) | 0 octet | chaîne | |
| 0x1D | LGR(ec) — longueur | 1 (fixe) | 0 octet | numérique | |
| 0x1E | POS(ec1,ea,ec2) — position | 3 (fixe) | 0 octet | numérique | |
| 0x1F | SKP(ec1,ea{,ec2}) — saut | 2 ou 3 | 1 octet — nombre d'arguments réels (2 ou 3), IMP-0122 | numérique | |
| 0x20 | PTR(ec,ea{,e}) — pointeur | 2 ou 3 | 1 octet — nombre d'arguments réels (2 ou 3), IMP-0122 | numérique | |
| 0x21 | CNB(ec,ea{,va}) — conversion numérique | 2 ou 3 | 1 octet — nombre d'arguments réels (2 ou 3), IMP-0122 | numérique | |
| 0x22 | EQN(ec{,ea}) — équivalent numérique | 1 ou 2 | 1 octet — nombre d'arguments réels (1 ou 2), IMP-0122 | numérique | |
| 0x23 | SCH(ec,ea,e{,va}) — sous-chaîne | 3 ou 4 | 1 octet — nombre d'arguments réels (3 ou 4), IMP-0122 | chaîne | |
| 0x24 | GRL(ec,ea{,va}) — groupe de lettres | 2 ou 3 | 1 octet — nombre d'arguments réels (2 ou 3), IMP-0122 | chaîne | |
| 0x25 | CCA(ea) — conversion en caractères | 1 (fixe) | 0 octet | chaîne | |
| 0x26 | EQC(ea) — équivalent caractère | 1 (fixe) | 0 octet | chaîne | valeur admissible 0–127 sur T1600, 0–255 sur Mitra 15 (INRDP75 §6-10 : « non négative et inférieure à 128 (sur T1600) ou 256 (sur M15) ») ; LSE15 §4-4.2 documente une plage 0–127 pour le Mitra 15 — désaccord résolu en faveur d'INRDP75, source de référence du profil Mitra 15 version 8, conformément à la règle de préséance (§4.11.1). Contrôle de plage laissé au gestionnaire des erreurs selon le profil d'implantation (chapitre Compilateur §8 bis), sans effet sur le sous-code |
| 0x27 | DAT — date et heure | 0 | 0 octet | chaîne | s'écrit `DAT` sans parenthèses sur T1600, `DAT()` sur Mitra 15, et sous `LSE F4` avec ou sans parenthèses, restitué `DAT()` (même remarque syntaxique que TEM, 0x0F) |

**Convention d'arité variable.** Pour les fonctions à arité variable (SKP, PTR, CNB, EQN, SCH, GRL), le nombre d'arguments effectivement présents dans un appel donné est porté par un octet de charge utile explicite (IMP-0122). Les paramètres optionnels de sortie (`va` de CNB, GRL ; `va` de SCH) sont, lorsqu'ils sont présents, compilés comme une **référence** (§4.5, sous-code 0x00) et non comme une expression : la fonction leur écrit une valeur en retour, ce qui exige une cible adressable plutôt qu'une valeur.

**IMP-0122** — *Octet d'arité explicite pour les opérations à arité variable*
- Sujet : comment l'interpréteur détermine-t-il, pour une opération à arité variable, combien d'éléments consommer sur la pile — six fonctions chaînes (SKP, PTR, CNB, EQN, SCH, GRL, §4.11.4) et trois instructions de fichiers/module (CHARGER, SUPPRIMER, EXECUTER, §4.11.5), soit neuf sous-codes au total ?
- Choix : chacun de ces neuf sous-codes porte un octet de charge utile explicite indiquant le nombre réel d'éléments présents pour cette occurrence précise (ex. 2 ou 3 pour SKP ; 3 ou 4 pour CHARGER), déterminé par le compilateur à partir de la syntaxe de l'appel et gravé dans la séquence compilée.
- Alternatives envisagées : aucune marque physique, l'interpréteur étant supposé inférer l'arité dans un ordre fixe déterminé par le sous-code lui-même (rejetée : ceci ne répond pas à la question, puisque justement l'arité elle-même n'est pas fixe ; un tel renvoi au chapitre de l'interpréteur, sans indiquer quel mécanisme ce chapitre pourrait employer, laisse la question ouverte) ; inspection de type à l'exécution, en s'appuyant sur le fait que le paramètre optionnel est une référence là où les paramètres obligatoires sont des valeurs, ou réciproquement (rejeté : fonctionne par coïncidence pour certains sous-codes (ex. CHARGER, dont le `δ` optionnel est une référence) mais pas pour d'autres (ex. SKP, PTR, GRL, dont le paramètre optionnel n'est pas systématiquement d'une catégorie physique distincte des paramètres obligatoires) — une solution non uniforme, dépendant au cas par cas de la signature de chaque fonction, est une source d'erreur et contredit le principe déjà appliqué en IMP-0120 de ne jamais faire reposer RCI-M sur une propriété non garantie explicitement par RCI-M lui-même) ; sous-codes distincts par arité (ex. `SKP2`/`SKP3`) plutôt qu'un octet sur un sous-code unique (rejeté : consommerait de nombreux sous-codes supplémentaires dans une catégorie « opération codée » où un seul reste réservé, cf. §4.11.8, alors qu'un octet de charge utile n'en coûte aucun — même arbitrage que pour IMP-0121).
- Justification : le compilateur connaît toujours cette arité avec certitude à la compilation (il vient de parser la liste d'arguments) ; c'est la restitution de cette information à l'interpréteur, au moment de l'exécution, qui manquait. Un octet est la façon la plus directe et la plus uniforme de le faire, cohérente avec la technique déjà retenue pour le facteur de répétition d'AFFICHER (IMP-0116) et pour l'indicateur d'APPEL (IMP-0121).
- Impact mémoire : +1 octet par occurrence compilée de l'un de ces neuf sous-codes — coût marginal au regard de la taille totale d'un programme pédagogique typique.
- Impact disque : sans objet (RCI-M).
- Impact performances : négligeable (un octet supplémentaire à décoder, une comparaison pour l'interpréteur, en lieu et place d'un mécanisme non spécifié qui aurait de toute façon dû exister).
- Impact historique : neutre — l'arité de chaque appel reste exactement celle écrite par le programmeur dans le texte source ; RCI-M se contente de la rendre explicite plutôt que de la laisser implicite et non spécifiée.
- Références normatives : LSE15 §4-4.2 (signatures de SKP, PTR, CNB, EQN, SCH, GRL) ; LSE15 §6-3.2 (CHARGER) ; LSE15 (EXECUTER, SUPPRIMER) ; EXI-0011 ; IMP-0116, IMP-0120, IMP-0121 (précédents de méthode).
- Statut : retenu.

#### 4.11.5 Opérations d'appel, de déclaration et de gestion de fichiers (0x28–0x2F)

Cette plage regroupe les opérations qui créent, libèrent ou mobilisent une entrée de la table des symboles ou du catalogue de fichiers — un point commun justifiant leur regroupement dans une même plage compacte de huit sous-codes, exactement remplie.

**Un marqueur instruction/expression est nécessaire pour APPEL.** La question n'est pas de savoir si l'appel « a besoin » d'une valeur de retour au sens trivial (question sans grand enjeu physique), mais si l'interpréteur peut, en toute généralité, se dispenser de savoir dans quel contexte syntaxique l'appel a eu lieu. Le corpus répond explicitement par la négative, pour une raison plus profonde qu'un simple choix d'optimisation : LSE15 §7-5 précise que si une procédure appelée **à l'intérieur d'une expression** sort par `RETOUR EN` (donc sans jamais exécuter `RESULTAT`), *« l'évaluation de l'expression est interrompue, et la variable à laquelle la valeur de l'expression devait être affectée n'est pas changée »* — l'exemple donné (`1 PROCEDURE &RAC(ARG,ERRE) LOCAL ARG,ERRE; 3 SI ARG<0 ALORS RETOUR EN ERRE; 5 RESULTAT RAC(ARG)`) montre qu'un appel en position d'expression peut se terminer par un abandon complet de l'instruction englobante (l'affectation prévue n'a jamais lieu), alors que ce même comportement, en position d'instruction, ne serait qu'un simple branchement ordinaire (§4.8.3). L'interpréteur ne peut décider laquelle des deux réactions appliquer à un `RETOUR`/`RETOUR EN` rencontré pendant l'exécution du corps de la procédure qu'en sachant, dès l'entrée dans l'appel, si celui-ci a eu lieu en position d'instruction ou d'expression — information que rien d'autre dans la séquence compilée ne porte, puisque le corps de la procédure (partagé entre tous ses appels, quelle que soit leur position syntaxique, §6.3) est strictement identique dans les deux cas.

| Sous-code | Opération | Charge utile physique | Notes |
|---|---|---|---|
| 0x28 | APPEL (procédure ou fonction utilisateur, `&NOM(...)`) | 2 octets : 1 octet indicateur (0x00 = position d'instruction, résultat ignoré ; 0x01 = position d'expression, `RESULTAT` exigé, IMP-0121), puis 1 octet `k`, nombre d'arguments effectifs écrits au site d'appel (IMP-0177) — consomme une référence simple déjà résolue (§4.5, sous-code 0x00), déposée avant les arguments, puis les `k` arguments déjà poussés en pile ; `k` doit égaler l'arité de la déclaration en vigueur (IMP-0114), faute de quoi l'interpréteur signale une erreur d'exécution (LSE15 §7-8) | L'indicateur et `k` sont déterminés sans ambiguïté par le compilateur à partir de la syntaxe du site d'appel (deux règles de grammaire distinctes, `procedureCallStmt` contre position d'opérande d'expression ; liste `effectiveParamList`) ; ils ne dépendent d'aucune information sur la procédure appelée, dont le corps reste unique et partagé (§6.3). Cf. remarque ci-dessus, IMP-0121 et IMP-0177. |
| 0x29 | TABLEAU — déclaration d'un tableau | 2 octets identifiant de symbole (déjà créé en table des symboles par le compilateur lors du traitement de cette ligne, EXI-0001) ; 1 octet nombre de dimensions (1 ou 2) ; les expressions de dimension sont compilées normalement et précèdent cet élément | Le contrôle de compatibilité de redéclaration (LSE15 §3-1.2 : erreur si l'identificateur est déjà une variable simple ou une variable-chaîne ; libération implicite s'il est déjà un tableau) est une vérification d'**exécution**, effectuée par le gestionnaire des variables lorsque cet élément s'exécute réellement — non par le compilateur lors du simple traitement de la ligne. Voir remarque générale ci-dessous. |
| 0x2A | CHAINE — déclaration d'une variable chaîne | 2 octets identifiant de symbole | Même remarque que pour TABLEAU ci-dessus : le contrôle de compatibilité (LSE15 §4-3 : autorisé seulement sur un identificateur non encore utilisé, libéré, ou déjà variable-chaîne) est une vérification d'**exécution**. Voir remarque générale ci-dessous. |
| 0x2B | LIBERER — libération d'un identificateur | 2 octets identifiant de symbole ; une instruction `LIBERER id1,id2,...` compile une suite d'éléments 0x2B, un par identificateur | |
| 0x2C | GARER — écriture d'un enregistrement de fichier-données | 0 octet — consomme, dans l'ordre, une référence (§4.5) désignant la variable/le tableau/la chaîne à garer, puis les deux expressions d'identification/portée compilées normalement | |
| 0x2D | CHARGER — lecture d'un enregistrement de fichier-données | 1 octet — nombre d'éléments réels consommés (3 ou 4), IMP-0122 — consomme une référence cible, deux expressions, et, si l'octet vaut 4, une référence supplémentaire pour le paramètre optionnel de compte-rendu d'erreur | |
| 0x2E | SUPPRIMER — suppression d'un enregistrement ou d'un fichier | 1 octet — nombre d'expressions réelles consommées (1 ou 2), IMP-0122 | **Mitra 15 uniquement** [INRDP75, Annexe III §2, §4] : « Instruction SUPPRIMER permet de supprimer un fichier données » est listée parmi les spécificités Mitra 15, et la règle de compatibilité impose explicitement « ne pas utiliser l'instruction SUPPRIMER » pour tout programme destiné à rester portable vers T1600 |
| 0x2F | EXECUTER — chargement et lancement d'un module de programme (forme instruction, distincte de la commande moniteur homonyme, cf. §4.11.7) | 1 octet — nombre d'expressions réelles consommées (1 ou 2), IMP-0122 | |

Le format exact des enregistrements de fichiers-données (GARER/CHARGER) et le protocole de recherche/chargement de module (EXECUTER) relèvent du gestionnaire des programmes/fichiers et du gestionnaire des tâches (Tome III), chapitres distincts du Tome V ; le présent chapitre garantit seulement que RCI-M dispose d'un sous-code identifiable et d'une convention de pile pour chacune de ces quatre instructions, condition nécessaire et suffisante pour que le compilateur puisse les traduire sans attendre la rédaction de ces chapitres.

**Remarque générale — TABLEAU et CHAINE ne prennent effet qu'à l'exécution.** Le corpus est explicite sur ce point : la vérification de compatibilité d'une (re)déclaration se produit *« quand on exécute une instruction TABLEAU »* (LSE15 §3-1.2), formulation qui s'applique de la même façon à CHAINE (LSE15 §4-3). L'expression « déjà créé en table des symboles par le compilateur lors du traitement de cette ligne », employée pour 0x29, désigne donc la seule création de l'entrée de symbole, nécessaire pour attribuer un identifiant stable aux références (§4.5, IMP-0104) — non l'effet de la déclaration, qui ne se produit qu'à l'exécution de l'élément, sur l'environnement des identificateurs (chapitre État d'exécution, IMP-0179). La table des symboles ne porte d'ailleurs aucune catégorie variable/chaîne/tableau (§6.2). Le compilateur peut avoir compilé, ailleurs dans le programme, d'autres références au même identificateur, par exemple une ligne `3 AFFICHER A` et une ligne `1 CHAINE A;A←'ERIC'` compilées dans un ordre quelconque : aucune de ces lignes ne présume de l'ordre d'exécution réel, et aucune ne constitue une erreur de compilation. L'erreur, si elle doit se produire, est détectée à l'exécution, au moment où l'incompatibilité se manifeste réellement — à l'exécution de la déclaration elle-même en cas de redéclaration incompatible, ou plus tard, à l'exécution d'une référence incohérente avec la catégorie effective de l'identificateur (règles de transition, chapitre État d'exécution, IMP-0179). Ni 0x29 ni 0x2A n'ont besoin de connaître la catégorie *précédente* de l'identificateur visé.

**IMP-0121** — *Indicateur de position d'appel (instruction/expression) sur APPEL*
- Sujet : l'interpréteur a-t-il besoin de savoir, lors de l'exécution de l'élément APPEL, si l'appel a eu lieu en position d'instruction ou en position d'expression ?
- Choix : un octet indicateur porté par l'élément APPEL (0x28) lui-même : 0x00 pour un appel en position d'instruction, 0x01 pour un appel en position d'expression. Déterminé sans ambiguïté par le compilateur à partir de la syntaxe du site d'appel (§4.5, IMP-0120), jamais à partir d'une propriété du symbole appelé.
- Alternatives envisagées : aucun marqueur, en supposant que l'interpréteur peut se passer de cette information (rejeté : LSE15 §7-5 montre qu'un appel en position d'expression sorti par `RETOUR`/`RETOUR EN` — sans `RESULTAT` — doit interrompre l'évaluation de l'expression englobante et annuler l'instruction qui l'utilisait, alors qu'un appel identique en position d'instruction, sortant de la même façon, ne fait qu'un branchement ordinaire ; le corps de la procédure étant partagé et strictement identique dans les deux cas (§6.3), rien d'autre que ce marqueur ne permet à l'interpréteur de distinguer les deux réactions) ; porter cette information sur la référence plutôt que sur l'opération (rejeté par IMP-0120 : la référence identifie *quel* symbole appeler, une information stable, non le contexte syntaxique de *cet* appel précis, qui varie d'une occurrence à l'autre du même symbole) ; deux sous-codes distincts pour APPEL-instruction et APPEL-expression plutôt qu'un octet indicateur sur un sous-code unique (rejeté : la catégorie « opération codée » ne compte qu'une réserve extrêmement restreinte, cf. §4.11.8 ; consommer un sous-code supplémentaire pour une distinction représentable par un simple octet de charge utile contredirait la technique déjà retenue en IMP-0116, préférée à toute renumérotation pour une extension de ce type).
- Justification : ce choix restitue une distinction comportementale réellement attestée par le corpus (LSE15 §7-5), que la fusion des sous-codes de référence par forme physique (IMP-0120) ne suffit pas à porter à elle seule. Sans cet indicateur, RCI-M serait dans l'incapacité de représenter correctement l'exemple `&RAC(ARG,ERRE)` du corpus lorsqu'il est appelé en position d'expression.
- Impact mémoire : +1 octet par appel de procédure/fonction (0x28), quelle que soit sa position — coût minime au regard de la fréquence de cette opération dans les programmes historiques examinés.
- Impact disque : sans objet (RCI-M).
- Impact performances : négligeable (un octet supplémentaire à décoder, une comparaison pour l'interpréteur).
- Impact historique : restitue un comportement documenté (LSE15 §7-5) qui aurait sinon été perdu ; l'algorithme précis d'interruption/annulation de l'expression englobante relève de l'interpréteur (hors périmètre RCI-M), mais ne peut être écrit sans cette information physique.
- Références normatives : LSE15 §7-4, §7-5 ; EXI-0011 ; §4.8.3 (RETOUR EN, sortie extraordinaire) ; §4.11.8 (occupation de la catégorie « opération codée ») ; IMP-0120.
- Statut : retenu.

**IMP-0177** — *Nombre d'arguments effectifs porté par l'élément APPEL*
- Sujet : LSE15 §7-8 exige qu'il y ait « toujours... autant de paramètres effectifs... que de paramètres formels ». Avec une charge utile réduite à l'indicateur de position (IMP-0121), l'élément APPEL ne dit pas combien d'arguments le site d'appel a déposés sur la pile. Trois conséquences en découlent :
  1. La règle de LSE15 §7-8 ne peut être vérifiée nulle part. Le compilateur ne connaît pas l'arité au site d'appel, la ligne de déclaration pouvant être compilée avant, après ou sans la ligne d'appel (EXI-0001 ; chapitre Compilateur §13.5). L'interpréteur, de son côté, ne connaît que l'arité de la déclaration en vigueur.
  2. Un appel compilé avec un nombre d'arguments différent de cette arité ferait dépiler par l'interpréteur des éléments étrangers à l'appel, ou en laisserait sur la pile. L'état d'exécution serait corrompu sans diagnostic.
  3. La référence à la procédure se trouve sous les arguments dans la pile (ordre postfixé, EXI-0011). L'interpréteur ne peut la retrouver qu'en connaissant le nombre d'arguments déposés par-dessus, alors que l'arité n'est connue qu'après résolution de cette même référence. De même, le décompilateur ne peut pas délimiter, dans la séquence postfixée, les sous-séquences qui forment les arguments de l'appel sans connaître leur nombre (EXI-0015) : l'arité de la déclaration en vigueur ne convient pas, puisqu'elle peut différer de l'appel écrit ou être absente.
- Choix : la charge utile de l'élément APPEL (0x28) comporte deux octets :

```text
tag 0xA8                       RCIM_CAT_OPERATION | 0x28
1 octet   indicateur           0x00 = position d'instruction, 0x01 = position d'expression (IMP-0121)
1 octet   k                    nombre d'arguments effectifs écrits au site d'appel (0 à 255)
```

  Le compilateur renseigne `k` en comptant les paramètres effectifs reconnus entre les parenthèses de l'appel ; un appel `&P()` donne `k = 0`. À l'exécution, l'interpréteur lit `k` avant toute consommation de la pile. Il trouve la référence à la procédure à la profondeur `k` sous le sommet et la résout. Si `k` diffère de l'arité de la déclaration en vigueur (`InfoProcedure.arite`, IMP-0114), il signale une erreur d'exécution, avant toute création de cadre d'activation et sans rien dépiler (chapitre État d'exécution, §5.5, étape 1). Le décompilateur restitue la référence à la procédure, puis les `k` sous-séquences d'arguments qui précèdent l'élément.
- Alternatives envisagées :
  1. Vérifier le nombre d'arguments à la compilation — écarté. EXI-0001 interdit au compilateur de dépendre d'une autre ligne. La déclaration en vigueur peut en outre changer après la compilation de l'appel, par ajout ou suppression d'une ligne de déclaration (LSE15 §7-6, Note), sans que la ligne d'appel soit recompilée.
  2. Regrouper l'indicateur de position et `k` dans un seul octet (bit 0 pour la position, bits 7-1 pour `k`, jusqu'à 127 arguments, borne largement suffisante pour une ligne de 80 caractères) — écarté. L'économie se limiterait à un octet par site d'appel, alors que ce regroupement redéfinirait l'encodage, déjà retenu, de l'octet indicateur d'IMP-0121. Un octet distinct laisse IMP-0121 intacte et suit la convention d'IMP-0122, un octet de compte par élément dont le nombre d'opérandes n'est pas fixe. Le conditionnement place la simplicité avant l'économie mémoire.
  3. Un sous-code par nombre d'arguments — écarté. La catégorie « opération codée » n'a plus aucune réserve (§4.11.8).
  4. Insérer avant l'APPEL une valeur immédiate portant `k` — écarté. Ce choix ferait d'une donnée de structure une valeur de la pile d'évaluation, en contradiction avec Tome III §6.6, et coûterait au moins autant qu'un octet de charge utile.
- Justification : l'appel de procédure est un élément dont le nombre d'opérandes est déterminé par le texte du site d'appel. C'est exactement la situation qu'IMP-0122 traite déjà pour neuf autres sous-codes, à ceci près que ce nombre n'est même pas borné par une signature fixe. Le compilateur connaît `k` avec certitude, puisqu'il vient d'analyser la liste des arguments. Le transmettre est la seule façon de rendre vérifiable, à l'exécution, la règle de LSE15 §7-8 sans contrevenir à EXI-0001.
- Impact mémoire : +1 octet par site d'appel de procédure ou de fonction. Le majorant de 155 octets d'IMP-0125 n'est pas affecté : un appel `&A(B,C,...)` produit environ 1,5 octet compilé par caractère source (3 octets par argument d'une lettre suivi de sa virgule, 6 octets pour la référence à la procédure et l'élément APPEL), en dessous du ratio de 2 retenu par IMP-0125.
- Impact disque : sans objet (RCI-M) ; l'octet est sérialisé avec l'élément.
- Impact performances : un octet supplémentaire à décoder et une comparaison par appel.
- Impact historique : rend effective la règle documentée de LSE15 §7-8 et rend décompilable tout appel, quel que soit l'état des déclarations dans le programme courant. Aucune construction historique n'est modifiée.
- Références normatives : LSE15 §7-6 (Note), §7-8 ; EXI-0001, EXI-0011, EXI-0015, EXI-0016, EXI-0019 ; Tome III §6.6 ; IMP-0114, IMP-0121, IMP-0122, IMP-0125 ; §4.11.8 ; chapitre Compilateur §13.5 ; chapitre État d'exécution §5.5.
- Statut : retenu.

#### 4.11.6 Complément : LIRE et son format (0x39–0x3B) — construction non attestée par les sources historiques strictes

Selon la grammaire reconstruite `LSE15Mitra.g4`, `LIRE` admet un format optionnel entre crochets, syntaxiquement proche de celui d'AFFICHER (labels, mise en page `/ X C L`, spécification `U`) mais plus restreint : ni spécification `F`/`E`, ni facteur de répétition variable (`*`) n'y sont admis, seul un facteur fixe l'est. **Ce format n'a été retrouvé ni dans INRDP75, ni dans LSE15, ni dans NOYELLE89** : les deux premières sources ne documentent que la forme `LIRE liste_de_références`, sans crochets ; la troisième, récit historique de la conception du langage, ne descend pas au niveau de détail syntaxique nécessaire pour se prononcer sur ce point (cf. §4.11.1). Il s'agit donc, comme signalé en §4.11.1, d'une construction de la grammaire de reconstruction, potentiellement une évolution ultérieure non couverte par le corpus disponible, et non d'un fait historique. Conformément à la consigne de ne rien supprimer silencieusement, les sous-codes correspondants sont réservés ci-dessous plutôt qu'omis, chacun explicitement marqué non historique.

| Sous-code | Spécification | Charge utile physique | Notes |
|---|---|---|---|
| 0x39 | Libellé (LIRE) | 1 octet répétition (1–99, fixe uniquement) ; 1 octet longueur `n` + `n` octets de texte | **non attesté** dans INRDP75/LSE15/NOYELLE89 ; réservé pour la forme `format_lire` de la grammaire reconstruite `LSE15Mitra.g4` |
| 0x3A | Mise en page (LIRE) — `/`, `X`, `C` ou `L` | 1 octet caractère (0=`/`,1=`X`,2=`C`,3=`L`) ; 1 octet répétition (1–99, fixe uniquement) | **non attesté**, mêmes conditions que 0x39 |
| 0x3B | Lecture d'une valeur selon la spécification `U` (par défaut ou explicite) | 0 octet — consomme une référence cible déjà compilée (§4.5) ; l'interpréteur y affecte la valeur saisie | seule forme attestée par le corpus (LIRE sans format) ; réutilisée telle quelle si le format entre crochets venait à être pris en charge |

**Absence de format pour LIRE.** La forme historiquement attestée est `LIRE liste_de_références`, qui compile la marque de tête 0x11 (§4.12, IMP-0183), puis la séquence des références de chaque cible, chacune suivie d'un élément 0x3B, sans qu'aucun élément 0x39/0x3A ne soit jamais synthétisé — à la différence d'AFFICHER, aucune équivalence implicite n'a lieu d'être ici puisque le format entre crochets lui-même n'est pas un fait historique.

#### 4.11.7 Instructions du moniteur explicitement hors RCI-M

Le conditionnement (§11) cite GARER, CHARGER, SUPPRIMER, CATALOGUER, ENCODER, DECODER parmi les mécanismes de fichiers-données à prendre en charge. La grammaire reconstruite confirme, et le corpus historique corrobore, que **GARER, CHARGER, SUPPRIMER et EXECUTER sont des instructions du langage**, compilables dans une ligne de programme et donc représentées en RCI-M (§4.11.5) — mais que **CATALOGUER, ENCODER et DECODER n'apparaissent pas parmi les alternatives de la règle `instruction`** : ce sont des commandes du moniteur, saisies en dialogue direct (hors ligne numérotée), et non des instructions compilables. Le corpus le confirme explicitement pour GARER, en les opposant nommément : « Instruction GARER » contre « Commande RANGER » [corpus, tableau comparatif des commandes]. De la même façon, **RANGER** (sauvegarde du programme courant entier en fichier-programme permanent) et **APPELER** (chargement d'un fichier-programme comme nouveau programme courant) sont des commandes du moniteur, distinctes respectivement de l'instruction GARER (qui écrit un enregistrement de fichier-données, non un programme entier) et de l'instruction EXECUTER (qui lance un module depuis un programme en cours d'exécution).

Ces sept commandes (CATALOGUER, ENCODER, DECODER, RANGER, APPELER, LISTER et EXECUTER employée en tête de dialogue) sont donc **volontairement absentes** du catalogue d'opérations codées de RCI-M : elles relèvent de l'analyseur de commandes et du moniteur (Tome III), qui agissent directement sur le programme courant, le catalogue de fichiers ou l'état de la tâche, sans jamais produire d'élément RCI-M. Leur traitement appartient au chapitre du Tome V consacré au moniteur et à l'analyseur de commandes, non au présent chapitre.

**REPRENDRE.** À cette liste s'ajoute une huitième commande, **REPRENDRE** (`REPRENDRE EN n1{,n2}`), également volontairement absente de RCI-M. Il s'agit sans ambiguïté d'une commande du moniteur et non d'une instruction du langage : LSE15 la documente dans la même section que les commandes EXECUTER A PARTIR DE, CONTINUER/POURSUIVRE JUSQU'EN et PAS A PAS, comme un moyen de faire repasser la console de l'état « moniteur » à l'état « exécution » à un numéro de ligne donné, sans modifier l'affectation ni la valeur des identificateurs du programme en cours. `REPRENDRE` figure par ailleurs, aux côtés de `SUPPRIMER`, parmi les mots réservés propres à Mitra 15 (Annexe VI) ; `SUPPRIMER` (l'instruction, homonyme mais distincte de la commande SUPPRIMER du moniteur pour les fichiers) est, elle, traitée en détail au §4.11.5.

#### 4.11.8 Récapitulatif d'occupation de la catégorie « opération codée »

| Plage | Occupation | Réserve |
|---|---|---|
| 0x00–0x0F | 16/16 (arithmétique commune) | 0 |
| 0x10–0x1B | 12/12 (relationnel/logique, affectation, UN/ZER T1600) | 0 |
| 0x1C–0x27 | 12/12 (valeur / chaînes) | 0 |
| 0x28–0x2F | 8/8 (appel/déclaration/fichiers) | 0 |
| 0x30–0x37 | 8/8 (AFFICHER) | 0 |
| 0x38 | 1/1 (format implicite d'AFFICHER — IMP-0126) | 0 |
| 0x39–0x3B | 3/3 (LIRE, non historique — §4.11.6) | 0 |
| 0x3C–0x3F | 4/4 (fonctions T1600 SH/CH/TH/SGN) | 0 |

Sur les 64 sous-codes disponibles dans la catégorie « opération codée », les 64 sont affectés ou explicitement marqués non historiques ; aucun sous-code n'y est disponible pour une construction supplémentaire, historique ou non. **Politique d'extension retenue pour toute construction future dans cette catégorie :** faute de réserve, toute opération supplémentaire qui viendrait à être identifiée dans cette catégorie est portée par un octet de sous-type ajouté à la charge utile d'un sous-code existant apparenté, selon la même technique de forme compacte à extension déjà employée en IMP-0116 (0x35), IMP-0121 (APPEL) et IMP-0122 (arité variable), plutôt que par la création d'un nouveau sous-code ou par une renumérotation de cette table. Cette politique n'appelle aucune restructuration du format des tags (§4.2) : les catégories `RCIM_CAT_REFERENCE` (4/64 affectés) et `RCIM_CAT_CONTROLE` (18/64 affectés) conservent une réserve large, mais leur nature (résolution de symbole ; cible de branchement) ne se prête à aucune des constructions actuellement cataloguées dans « opération codée » — un classement qui y serait forcé pour la seule raison d'un déséquilibre d'occupation entre catégories resterait artificiel, contrairement au principe déjà appliqué en IMP-0120.

---

### 4.12 Règles de restitution (décompilation)

EXI-0015 exige que « la décompilation d'un objet compilé [produise] une représentation textuelle valide correspondant à la construction de langage représentée par cet objet ». La commande DECODER range le résultat de la décompilation dans un fichier-données (INRDP75 : « La commande DECODER fait appel au décompilateur. Mais le résultat de cette décompilation, au lieu d'être simplement affiché (commande LISTER) […], est rangé en fichier-données »), que ENCODER recompile (Tome IV §9.4) : le texte restitué doit donc se recompiler en un programme équivalent, dans la limite de longueur d'une ligne. Le présent paragraphe fixe ce que la représentation compilée conserve exactement, et la forme canonique de ce qu'elle ne conserve pas. La réalisation du décompilateur fait l'objet du chapitre Décompilateur (`Decompilateur_LSE-IIGS.md`).

**IMP-0183** — *Restitution fidèle de toute construction conservée, forme canonique pour la présentation non conservée, marque de tête de LIRE*
- Sujet : quelles informations du texte saisi la représentation compilée conserve-t-elle, sous quelle forme le décompilateur restitue-t-il les autres, et quelle marque physique est nécessaire pour qu'aucune différence de construction ne soit perdue ?
- Choix :
  - **Restitution fidèle**, à partir des éléments de la séquence et des tables :
    - noms des identificateurs et des procédures, `&` compris (§6.2, IMP-0124) ;
    - déclaration de procédure, paramètres et liste `LOCAL` dans l'ordre écrit (élément 0x0D, chapitre Compilateur IMP-0174) ;
    - présence ou absence de `PAS` dans une instruction FAIRE (0x0A ou 0x0E, IMP-0112) ;
    - format explicite ou absent, et frontières de chaque instruction AFFICHER (0x37, 0x38, IMP-0115) ;
    - frontières de chaque instruction LIRE (marque de tête 0x11, ci-dessous) ;
    - graphie de `TEM` et `DAT` selon le profil d'implantation (chapitre Compilateur §8 bis, IMP-0180) ;
    - texte des chaînes, une apostrophe du texte étant restituée doublée (`'D''UN TRIANGLE'`) ; sous le profil `LSE F4`, un caractère non imprimable (code 0 à 31, ou 127) est restitué sous forme d'équivalent numérique `.n.`, les caractères non imprimables consécutifs étant regroupés (`'A'.8.'B'`, `.27 12.`), et un caractère imprimable est toujours restitué en clair (chapitre Compilateur, IMP-0190) ; sous les profils T1600 version 4 et MITRA 15, où `.n.` est hors profil, tout caractère est restitué tel quel entre les apostrophes ; le texte d'un libellé de format d'AFFICHER suit les mêmes règles ;
    - texte des commentaires à l'octet près : le texte conservé (0x0C, IMP-0113) est tout ce qui suit le `*` jusqu'à la fin de la ligne, espaces de tête compris.
  - **Forme canonique** pour la présentation que la représentation compilée ne conserve pas et qui n'a aucun effet sur l'exécution :
    - **Espaces.** Le numéro de ligne est suivi d'un espace, sauf pour une ligne de commentaire : le `*` est alors collé au numéro (`1*SOMME DES CARRES`), comme dans les listings T1600 de Victor Duruy (constat d'Éric, 2026-10-23) et dans 81 des 82 lignes de commentaire d'INRDP75 et de LSE15 ; le texte qui suit le `*` est restitué à l'octet près, espaces compris (`1* CALCUL DE L'AIRE`). Chaque mot du langage est séparé par un espace des mots, identificateurs et constantes qui l'entourent (`SI A=0 ALORS B←1 SINON ALLER EN 20`), sauf `AFFICHER` immédiatement suivi de son format et un nom de fonction immédiatement suivi de sa parenthèse (règles lexicales, chapitre Compilateur §5 bis). Aucun autre espace : ni autour des opérateurs et signes de ponctuation, ni après `;`.
    - **Parenthèses.** Seules sont restituées les parenthèses nécessaires selon les règles de précédence et d'associativité à gauche (chapitre Compilateur §5) : l'opérande droit d'une opération est parenthésé lorsqu'il est une opération de même rang ou de rang inférieur (`A-(B-C)`, `A↑(B↑C)`), l'opérande gauche lorsqu'il est une opération de rang inférieur (`(A+B)*C`) ; de même pour `ET` et `OU`. Le signe `+` en tête d'un terme n'est pas restitué. Une constante négative est restituée `-n` et traitée, pour le parenthésage, comme un changement de signe appliqué à `n` : `A*(-1)` garde ses parenthèses, `X←-1` n'en a pas.
    - **Constantes numériques.** Une constante de sous-format entier (§4.4, 0x00, 0x01, 0x07 à 0x3F) s'écrit en décimal, sans point. Une constante réelle (0x02) s'écrit selon les conventions d'écriture des nombres attestées par le corpus : au plus **six chiffres significatifs**, sans zéros superflus (« Les constructeurs garantissent 6 chiffres significatifs » ; « Les nombres n'ayant en L.S.E. que 6 chiffres significatifs », INRDP75) ; en notation décimale si sa valeur absolue est nulle ou comprise entre 10⁻³ inclus et 10⁶ exclu, sinon sous la forme d'une mantisse comprise entre 1 et 10 (10 exclu), suivie de `E` et d'un exposant « toujours signé, et [qui] comporte toujours 2 chiffres » (LSE15 §6-2.3, spécification U ; INRDP75, 3ᵉ partie, §7-3) : `0.055`, `1E-04`, `3.01E+21`. Six chiffres significatifs sont aussi la précision que le format `float` SANE restitue sans altération : un nombre décimal d'au plus six chiffres significatifs, converti en `float` puis réécrit avec six chiffres, retrouve exactement son écriture (`0.4` est restitué `0.4`). Une constante écrite avec davantage de chiffres (`3.1415927`) est restituée avec six (`3.14159`) ; sa recompilation par ENCODER donne une valeur qui ne diffère qu'au-delà du sixième chiffre, c'est-à-dire au-delà de la précision documentée. Une constante marquée numéro de ligne (0x06, profil Mitra 15) s'écrit `@` suivi du numéro.
    - **Blocs `DEBUT…FIN`.** Une branche `ALORS` ou `SINON` d'une instruction SI est restituée entre `DEBUT` et `FIN` lorsqu'elle comporte plus d'une instruction, lorsqu'elle est vide (`SI A=1 ALORS DEBUT FIN SINON X←2`), et lorsque, branche `ALORS` d'un SI qui a une branche `SINON`, elle se compose d'une seule instruction qui se termine par un SI ouvert : une instruction SI sans `SINON`, ou une instruction SI dont la branche `SINON`, écrite sans bloc, se termine elle-même par un SI ouvert. Un `SINON` écrit après un SI ouvert se rattacherait à lui (LSE15 §5-2) : `SI A=1 ALORS DEBUT SI B=1 ALORS X←1 FIN SINON X←2` et `SI A=1 ALORS SI B=1 ALORS X←1 SINON X←2` sont deux séquences différentes. Cette règle est celle de LSE 8086 version IV (constat d'Éric, 2026-10-23). Dans tous les autres cas, une branche, ou une suite d'instructions hors de toute branche, est restituée sans `DEBUT…FIN`.
    - **Instructions vides.** Une suite `;;` ou un `;` final ne sont pas restitués ; une ligne dont la séquence est vide est restituée comme le numéro suivi d'un espace (« ligne vide », INRDP75).
    - **Déclarations successives.** Des éléments TABLEAU (0x29), CHAINE (0x2A) ou LIBERER (0x2B) consécutifs, de même sous-code, sont restitués en une seule instruction dont la liste reprend les identificateurs dans l'ordre (`TABLEAU A[2],B[3]`) ; l'effet est identique à celui des instructions séparées.
  - **Marque de tête de LIRE.** Toute instruction LIRE commence par l'élément de contrôle 0x11, sans charge utile, suivi, pour chaque cible, de sa référence et d'un élément 0x3B (§4.11.6). `LIRE A,B` compile ainsi `[0x11][A][0x3B][B][0x3B]` et `LIRE A;LIRE B` compile `[0x11][A][0x3B][0x11][B][0x3B]` : les deux écritures restent distinctes, et l'interpréteur dispose du début de chaque instruction si une différence de comportement entre elles devait être établie (le corpus ne précise pas si `LIRE A;LIRE B` se comporte exactement comme `LIRE A,B`). L'élément 0x11 est placé dans la catégorie « information de contrôle » : la catégorie « opération codée » n'a plus aucune réserve (§4.11.8), et 0x11 porte, comme 0x0C et 0x0D, une information de structure utile à la décompilation.
  - **Longueur des lignes restituées.** Une ligne restituée sous forme canonique peut, exceptionnellement, dépasser la limite de 79 caractères hors fin de ligne (parenthèses rétablies, écriture d'une constante réelle). Le décompilateur ne la tronque jamais. DECODER l'écrit telle quelle ; c'est ENCODER qui la signale à la recompilation, en l'affichant suivie de l'erreur C 63 (choix d'Éric du 2026-10-30 ; audit du code, AC-10).
- Alternatives envisagées : conserver le texte source (rejeté : contraire à EXI-0022 et à la propriété même du L.S.E., dont le texte est « recréé, en cas de listage, à partir de la chaîne post-fixée », NOYELLE89) ; conserver les espaces ou les parenthèses redondantes par des éléments supplémentaires (rejeté : coût mémoire pour une information sans effet, contraire à EXI-0016) ; fusionner `LIRE A;LIRE B` en `LIRE A,B` sans marque (rejeté : le corpus ne permet pas d'établir l'équivalence de comportement des deux écritures) ; marque de tête de LIRE dans la catégorie « opération codée » (impossible : catégorie saturée) ; tronquer une ligne restituée trop longue (rejeté : produirait silencieusement un programme différent).
- Justification : toute différence de construction susceptible d'avoir un effet — boucle, format, frontières d'instructions d'entrée-sortie, déclaration de procédure, profil — est conservée physiquement ; seule la présentation sans effet est normalisée, ce qu'EXI-0015 admet (« une représentation textuelle valide correspondant à la construction »). Le texte canonique respecte les règles lexicales et se recompile donc en la même séquence.
- Impact mémoire : 1 octet par instruction LIRE (0x11) ; 1 octet par espace de tête conservé dans un commentaire.
- Impact disque : sans objet (RCI-M) ; les éléments se sérialisent sans transformation.
- Impact performances : nul à l'exécution (0x11 sans effet, §4.7) ; la décompilation est un parcours linéaire.
- Impact historique : la restitution conserve toute construction écrite ; la présentation (espaces, parenthèses redondantes, écriture des constantes) peut différer du texte saisi, comme dans tout système qui recrée le texte à partir de la chaîne post-fixée (NOYELLE89 : « il n'y avait pas besoin de sauvegarder le texte source […] : il était recréé, en cas de listage, à partir de la chaîne post-fixée »).
- Références normatives : EXI-0003, EXI-0010, EXI-0015, EXI-0016, EXI-0022 ; Tome IV §9.4 ; INRDP75 (décompilateur, DECODER) ; NOYELLE89 (chaîne post-fixée décompilable) ; §4.4, §4.7, §4.9, §4.11.6, §4.11.8, §6.2, IMP-0112, IMP-0113, IMP-0115, IMP-0124 ; chapitre Compilateur §5, §5 bis, §8 bis, IMP-0174, IMP-0180.
- Statut : retenu ; amendée le 2026-10-23 par choix d'Éric : blocs `DEBUT…FIN` (chapitre Décompilateur §5.2), caractères non imprimables sous MITRA 15, ligne de commentaire collée au numéro ; amendée le 2026-11-01 par choix d'Éric : `.n.` propre à `LSE F4`, libellés de format restitués comme les constantes-chaîne. La réalisation du décompilateur fait l'objet du chapitre Décompilateur ; le comportement de DECODER pour une ligne trop longue relève du chapitre du bibliothécaire, non encore rédigé. Amendée le 2026-10-30 par choix d'Éric (audit du code, AC-10) : une ligne restituée de plus de 79 caractères est signalée par ENCODER, non par DECODER.

---

## 5. Format physique de l'OBJET COMPILÉ

Conforme à ANN-K-0005 (« numéro de ligne + représentation compilée », aucune adresse ni pointeur au niveau logique).

```c
typedef struct {
    unsigned char  numero_ligne;      /* identifiant logique de la ligne, EXI-0002 (IMP-0125) */
    unsigned int   longueur_octets;   /* taille en octets de la séquence compilée */
    HandleObjet    bloc;              /* identifiant opaque résolu par le gestionnaire
                                          d'objets (cf. note de dépendance, §3) ;
                                          jamais un pointeur brut persistant */
} EntreeObjetCompile;
```

Rappel (IMP-0109) : cette structure ne représente que les lignes du programme courant conservées. Une ligne exécutée en mode machine de bureau n'y figure jamais.

`HandleObjet` est un type opaque géré exclusivement par le gestionnaire d'objets, qui peut le résoudre temporairement en adresse physique pour la durée d'un accès, sans jamais exposer cette adresse au-delà de l'appel courant.

**IMP-0106** — *Recherche d'une ligne par numéro*
- Sujet : algorithme d'accès à `EntreeObjetCompile` à partir d'un numéro de ligne.
- Choix : recherche dichotomique sur table triée par `numero_ligne`.
- Alternatives envisagées : table de hachage (complexité d'implémentation disproportionnée en C ANSI sur cible contrainte) ; parcours linéaire (coût O(n) inacceptable pour un programme de plusieurs centaines de lignes en dialogue interactif).
- Justification : compromis simplicité/performance adapté au 65816 et à la taille de mémoire disponible, sans dépendance à une bibliothèque non standard.
- Impact mémoire : nul (pas de structure supplémentaire).
- Impact disque : sans objet.
- Impact performances : O(log n) par accès, acceptable pour un usage conversationnel.
- Impact historique : neutre.
- Références normatives : EXI-0006, VM-2401.
- Statut : retenu.

---

## 6. Table des symboles

### 6.1 Contenu de la table des symboles et accès direct aux variables arithmétiques simples

La table des symboles appartient au programme courant : elle est produite par la compilation et ne porte que ce que la compilation peut établir. L'état d'un identificateur pendant l'exécution — non défini, variable simple, variable-chaîne ou tableau — et sa valeur courante appartiennent à l'environnement d'exécution, composante de l'état d'exécution (Tome IV §4.8 ; Tomes I-II §11.2, VM-1102 : « un état de déclaration ; une valeur courante »). Ils ne sont donc pas portés par `EntreeSymbole`, mais par l'**environnement des identificateurs** défini au chapitre État d'exécution (IMP-0179), indexé par le même identifiant 16 bits que la table des symboles (IMP-0104).

Faire résoudre systématiquement une variable vers un `HandleObjet` opaque imposerait une double indirection (index, puis résolution du handle) pour accéder à une valeur numérique. Or les programmes historiques examinés utilisent très abondamment ce type de variable, y compris dans des boucles serrées (FAIRE...JUSQUA parcourant des tableaux, compteurs) ; une double indirection à chaque lecture ou écriture représenterait un coût en cycles significatif sur 65816.

**IMP-0110** — *Accès direct aux variables arithmétiques simples par l'identifiant de symbole, valeur portée par l'environnement des identificateurs*
- Sujet : où est stockée la valeur courante d'une variable arithmétique simple, et comment une référence compilée y accède-t-elle sans indirection superflue ?
- Choix : la valeur numérique d'une variable simple est stockée **directement** (4 octets, `float` SANE, IMP-0103) dans l'entrée de l'environnement des identificateurs dont l'index est l'identifiant de symbole porté par la référence compilée (§4.5, IMP-0104) ; les variables-chaîne et les tableaux, de taille variable, y sont désignés par un `HandleObjet` géré par le gestionnaire d'objets (gestion des chaînes, gestion des tableaux). L'environnement des identificateurs est une structure de l'état d'exécution (chapitre État d'exécution, IMP-0179), distincte de la table des symboles du programme courant (§6.2), qui ne porte aucune valeur.
- Alternatives envisagées : handle systématique pour toute variable, y compris les variables simples (rejeté pour le coût en cycles exposé ci-dessus) ; valeur portée par `EntreeSymbole` dans la table des symboles (rejeté : le programme courant serait écrit à chaque affectation, contrairement à Tome IV §4.8, qui range l'environnement d'exécution dans l'état d'exécution, et à IMP-0108 ; l'état « non défini » exigé par HIS-12.3 et la catégorie effective d'un identificateur, qui ne sont connus qu'à l'exécution, y côtoieraient des données de compilation) ; valeur directe pour les tableaux et chaînes (rejeté : leur taille variable est incompatible avec une entrée de taille fixe).
- Justification : une variable simple a une taille physique fixe et connue, ce qui permet de l'intégrer directement dans une entrée de taille fixe, indexée par l'identifiant de symbole que porte déjà la référence compilée : l'accès reste en O(1), sans recherche ni handle. La séparation entre table des symboles (programme courant) et environnement des identificateurs (état d'exécution) réalise Tome IV §4.8 et IMP-0108 sans coût d'accès supplémentaire.
- Impact mémoire : 5 octets par identificateur dans l'environnement (état et valeur ou handle, IMP-0179) ; aucune allocation séparée par variable simple.
- Impact disque : sans effet sur RCI-S — la valeur courante d'une variable n'est jamais sérialisée dans un fichier-programme (IMP-0108).
- Impact performances : une indexation directe par accès à une variable simple — gain net dans les boucles FAIRE, très fréquentes dans le corpus.
- Impact historique : neutre — n'affecte que la réalisation physique.
- Références normatives : Tome IV §4.8 ; Tomes I-II §11.2 (VM-1102, EXI-0039) ; HIS-12.3 ; IMP-0103, IMP-0104, IMP-0108 ; chapitre État d'exécution, IMP-0179 ; conditionnement §3 ; corpus historique (chapitre FAIRE, usage massif de variables de contrôle arithmétiques).
- Statut : retenu.

### 6.2 Structure C

**Nature du symbole : seule distinction établie à la compilation.** Un nom rencontré par le compilateur est soit un nom de procédure, introduit par `&` (LSE15 §7-2), soit un identificateur ordinaire. Cette **nature** est certaine dès la compilation et ne change jamais : un identificateur sans `&` ne peut devenir un nom de procédure, ni l'inverse. Elle est la seule distinction portée par `EntreeSymbole` ; elle sert au décompilateur (restitution du `&`, IMP-0124) et à la recherche par nom du compilateur, qui distingue `&F` de `F` (chapitre Compilateur, IMP-0167). En revanche, le fait qu'un identificateur ordinaire désigne une variable simple, une variable-chaîne ou un tableau n'est jamais établi à la compilation : sous EXI-0001, la ligne `TABLEAU` ou `CHAINE` qui le déclare peut être compilée avant ou après une ligne qui l'utilise, et une telle déclaration ne prend effet qu'à son exécution (§4.11.5, remarque générale ; LSE15 §3-1.2, §4-3). Cette catégorie effective est tenue par l'environnement des identificateurs (chapitre État d'exécution, IMP-0179) ; une incompatibilité entre l'usage qu'une référence fait d'un identificateur et sa catégorie effective est une erreur d'exécution, du même ordre que celles documentées par le corpus pour la redéclaration incompatible (LSE15 §3-1.2 pour TABLEAU, §4-3 pour CHAINE). Les références compilées utilisent uniformément le sous-code 0x00, ou 0x01 pour un accès indexé (§4.5, IMP-0120), choisi d'après la seule syntaxe du site.

**IMP-0124** — *Nom du symbole stocké en ligne, sans pointeur*
- Sujet : le nom textuel d'un symbole doit-il être atteint par un pointeur vers une allocation séparée, ou stocké directement dans `EntreeSymbole` ?
- Choix : un tableau `char nom[5]` en ligne dans `EntreeSymbole`, accompagné de `nom_longueur` (1 octet, valeurs 1 à 5) indiquant le nombre d'octets réellement significatifs. Aucune allocation séparée, aucun pointeur. Pour un symbole de nature `SYM_PROCEDURE`, le tableau ne porte que le corps de l'identificateur (après le `&`) ; le décompilateur restitue `&` à l'affichage à partir de `nature`, sans qu'il soit nécessaire de le stocker.
- Alternatives envisagées : `char *nom` vers une allocation externe dimensionnée à `nom_longueur` (rejeté : voir justification chiffrée ci-dessous) ; nom terminé par un octet nul plutôt que préfixé par sa longueur (rejeté : romprait la convention déjà retenue dans tout le chapitre pour les chaînes physiques, §4.4 sous-code 0x03, et n'aurait de toute façon pas réduit la taille du champ — 6 octets dans les deux cas, `nom_longueur` + `nom[5]` ou `nom[6]` avec terminateur).
- Justification : sur cette cible, un pointeur C occupe 4 octets (« pointers are the same size as unsigned long », manuel ORCA/C 2.2) — presque autant que les 5 caractères maximum d'un identificateur L.S.E. (LSE15 §3-1.2, INRDP75 « Règle pour les identificateurs » : au plus 5 caractères alphanumériques, y compris pour le corps d'un nom de procédure après le `&`). Le champ `nom_longueur` + pointeur coûte déjà 5 octets dans `EntreeSymbole`, avant même de compter l'allocation externe (1 à 5 octets supplémentaires) et son éventuel surcoût d'allocateur ; le stockage en ligne coûte un total fixe de 6 octets (1 + 5), inférieur ou égal au coût du pointeur dans tous les cas et strictement inférieur dès qu'un nom dépasse un caractère. Il élimine en outre une indirection à chaque accès (même principe qu'IMP-0110) et une catégorie entière d'allocation à la charge du gestionnaire d'objets, supprimant du même coup tout risque qu'un pointeur C brut soit invalidé par une compaction — le nom voyage désormais avec la structure `EntreeSymbole` elle-même, conformément au principe général du conditionnement (§9) qui déconseille les pointeurs C comme modèle architectural implicite.
- Impact mémoire : coût fixe de 6 octets par symbole (1 + 5), contre 5 octets en ligne + 1 à 5 octets externes + surcoût d'allocateur ≥ 0 pour la conception à pointeur — gain net dans tous les cas, jusqu'à 4 octets par symbole pour un nom de 5 caractères.
- Impact disque : simplifie une future sérialisation RCI-S (champ de taille fixe, copié tel quel, sans résolution de pointeur préalable).
- Impact performances : élimine une indirection à chaque accès au nom (décompilation, dialogue conversationnel) ; élimine également une allocation/libération par symbole créé/détruit.
- Impact historique : neutre — choix de représentation physique pur (§26 du conditionnement) ; le texte restitué par le décompilateur est strictement identique.
- Références normatives : manuel ORCA/C 2.2 (taille des pointeurs) ; LSE15 §3-1.2, INRDP75 (règle des 5 caractères, identificateurs et corps de nom de procédure) ; IMP-0110 (précédent : stockage direct plutôt qu'indirection) ; conditionnement §9 (pointeurs C non structurants).
- Statut : retenu.

```c
typedef unsigned char NatureSymbole;     /* 1 octet — cf. IMP-0127 : un enum C occupe
                                             toujours 2 octets sous ORCA/C (int),
                                             quel que soit le nombre de valeurs */
#define SYM_IDENTIFICATEUR  0   /* identificateur sans `&` : variable simple, variable-chaîne
                                    ou tableau selon l'exécution — catégorie effective tenue
                                    par l'environnement des identificateurs (État
                                    d'exécution, IMP-0179), jamais par cette table */
#define SYM_PROCEDURE       1   /* nom de procédure ou de fonction, introduit par `&`
                                    (un seul mot-clé de déclaration, PROCEDURE ; la
                                    distinction « fonction » ne porte que sur l'usage au site
                                    d'appel — cf. §4.5) ; entrée par numéro de ligne,
                                    cf. IMP-0114, §6.3 */

typedef unsigned char OrigineSymbole;    /* 1 octet — même raison, cf. IMP-0127 */
#define ORIGINE_PROGRAMME       0   /* symbole référencé par au moins une ligne conservée */
#define ORIGINE_MACHINE_BUREAU  1   /* symbole créé/modifié uniquement en mode machine de
                                        bureau, cf. IMP-0109 — candidat à exclusion de RCI-S */

typedef struct {
    unsigned char numero_ligne_entree;  /* ligne portant PROCEDURE &ID(...) ; cf. §6.3, IMP-0125 */
    unsigned char arite;                /* nombre de paramètres formels déclarés */
} InfoProcedure;

typedef struct {
    unsigned char     nom_longueur;   /* longueur réelle du nom, 1-5 (IMP-0124) */
    char              nom[5];         /* corps de l'identificateur L.S.E. (sans le `&`
                                          d'un nom de procédure, restitué par le
                                          décompilateur à partir de `nature`),
                                          nécessaire au décompilateur (EXI-0015) et
                                          au dialogue conversationnel ; seuls les
                                          `nom_longueur` premiers octets sont valides */
    NatureSymbole     nature;         /* §6.2 : fixée à la création, ne change jamais */
    OrigineSymbole    origine;        /* support du filtrage futur en RCI-S, cf. IMP-0104 */
    InfoProcedure     procedure;      /* significatif pour SYM_PROCEDURE seulement : cf. IMP-0114,
                                          §6.3 ; {0, 0} pour SYM_IDENTIFICATEUR */
} EntreeSymbole;                      /* 10 octets ; aucune valeur d'exécution (IMP-0110) */
```

**IMP-0127** — *`NatureSymbole` et `OrigineSymbole` en `unsigned char` plutôt qu'en `enum`*
- Sujet : deux champs de `EntreeSymbole` (`nature`, 2 valeurs possibles ; `origine`, 2 valeurs possibles) sont déclarés comme des `enum` C. Cette déclaration a-t-elle un coût physique caché ?
- Choix : `typedef unsigned char` accompagné de constantes `#define`, pour les deux types, plutôt qu'un véritable `enum` C.
- Alternatives envisagées : conserver le type `enum` (rejeté : voir justification chiffrée ci-dessous) ; champs de bits (`bit-field`) pour descendre sous l'octet (rejeté par cohérence avec IMP-0101, qui écarte déjà cette technique pour un gain marginal au prix de la lisibilité).
- Justification : le manuel ORCA/C est explicite — *« Enumerations are displayed as 2-byte integers »* et *« enumeration variables are treated as type int »* — un `enum` C occupe donc toujours 2 octets sur cette cible, quel que soit le nombre de valeurs qu'il distingue, y compris pour une énumération à seulement 2 valeurs. Un `unsigned char` avec des constantes `#define` porte exactement la même information sur 1 octet. §2 confirme qu'ORCA/C n'impose aucun alignement de structure, donc ce gain n'est pas absorbé par du bourrage : c'est une économie nette.
- Impact mémoire : −1 octet pour `nature`, −1 octet pour `origine`, soit 2 octets économisés par entrée de la table des symboles, sans aucune perte d'information.
- Impact disque : réduit d'autant la taille d'une future sérialisation RCI-S de la table des symboles.
- Impact performances : neutre (une comparaison sur `unsigned char` est aussi rapide que sur `int`).
- Impact historique : sans objet — choix de représentation physique pur.
- Références normatives : manuel ORCA/C 2.2 (taille des énumérations) ; §2 (absence d'alignement de structure) ; IMP-0103 (principe du plus petit sous-format suffisant), dont ce choix applique le même principe à une structure C du chapitre plutôt qu'à un élément de la séquence compilée.
- Statut : retenu.

### 6.3 Procédures et fonctions : numéro de ligne d'entrée plutôt que handle

La question se pose de savoir ce que doit désigner physiquement le champ `contenu` d'une entrée `SYM_PROCEDURE` : un numéro de ligne, ou un `HandleObjet` vers un objet compilé séparé ? Le corpus historique montre qu'une procédure est déclarée par une instruction `PROCEDURE &ID(pf1, pf2, ..., pfn)` qui occupe une ligne numérotée du programme courant exactement comme n'importe quelle autre instruction — le corps de la procédure étant constitué des lignes suivantes, jusqu'à ce qu'une instruction RETOUR ou RESULTAT rende la main à l'appelant (§4.8.3). Aucune structure physique distincte n'est donc historiquement nécessaire pour représenter « l'objet procédure » : son contenu exécutable est déjà entièrement représenté par les objets compilés ordinaires de la table des objets compilés (§5), indexés par numéro de ligne. Une seule entrée `SYM_PROCEDURE` sert indifféremment à un appel en position d'instruction ou en position d'expression — cette distinction relève exclusivement du site d'appel (§4.5), jamais de la déclaration.

**IMP-0114** — *Identification des procédures et fonctions par numéro de ligne d'entrée*
- Sujet : nature physique du champ `contenu` d'une entrée de symbole `SYM_PROCEDURE`.
- Choix : le champ `contenu` porte directement, en ligne, le numéro de la ligne où figure l'instruction `PROCEDURE &ID(...)` (1 octet, même type que les autres numéros de ligne du chapitre, IMP-0125), accompagné de l'arité déclarée (1 octet), sans passer par un `HandleObjet`. Appeler la procédure revient à résoudre ce numéro de ligne dans la table des objets compilés (§5), exactement comme pour un `ALLER EN` à cible constante (§4.7, sous-code 0x02), puis à empiler un cadre d'activation (état d'exécution, hors périmètre RCI-M).
- Alternatives envisagées :
  1. `HandleObjet` vers un objet compilé de procédure géré séparément par le gestionnaire d'objets — écarté : ce handle ferait doublon avec l'information déjà disponible via `EntreeObjetCompile` (§5), instaurerait une deuxième source de vérité pour le même contenu, et devrait être tenu à jour à chaque remplacement de la ligne d'entrée de la procédure (EXI-0006) — un risque de désynchronisation qu'une simple référence de ligne n'introduit pas, puisque la table des objets compilés est déjà l'unique source de vérité pour le contenu exécutable de toute ligne, y compris celle d'une procédure.
  2. `HandleObjet` vers une structure dédiée contenant à la fois le numéro de ligne et des métadonnées de paramètres — écarté : n'apporte aucun bénéfice par rapport à l'inclusion directe de ces mêmes informations (numéro de ligne, arité) dans l'entrée de symbole, tout en ajoutant une indirection et une allocation dynamique inutiles pour une information de taille fixe et de durée de vie égale à celle du symbole lui-même.
- Justification de robustesse et d'efficacité : un numéro de ligne est une donnée de taille fixe (1 octet, IMP-0125), déjà la représentation retenue dans tout le reste du chapitre pour désigner une cible de ligne constante (§4.7, sous-codes 0x02/0x05) ; réutiliser ce même format pour les procédures/fonctions évite d'introduire un second système d'identification. La robustesse est directement liée à l'absence de duplication : puisque le contenu exécutable de la procédure est déjà celui de sa ligne d'entrée (et des lignes suivantes) dans la table des objets compilés, aucune synchronisation supplémentaire n'est nécessaire lorsque cette ligne est modifiée — le symbole continue de désigner la bonne ligne par construction, la table des objets compilés restant l'unique source de vérité. L'efficacité tient à l'absence de toute résolution de handle : appeler une procédure ne nécessite qu'une recherche dichotomique déjà prévue (IMP-0106), sans passage supplémentaire par le gestionnaire d'objets.
- Impact mémoire : 2 octets par procédure/fonction déclarée (numéro de ligne + arité, IMP-0125), contre 2 à 4 octets pour un `HandleObjet` qui aurait de toute façon dû être résolu vers un numéro de ligne en interne — coût comparable ou inférieur, sans allocation dynamique associée.
- Impact disque : ce champ se sérialise directement dans RCI-S sans transformation (un numéro de ligne reste valide tel quel dans la représentation sérialisée, ANN-K-0022).
- Impact performances : élimine une résolution de handle à chaque appel de procédure/fonction — mécanisme identique et déjà optimisé pour ALLER EN à cible constante.
- Impact historique : conforme à la structure documentée (une procédure est une ligne numérotée comme les autres, cf. corpus historique, exemples `n; PROCEDURE &ID(...)`).
- Références normatives : corpus historique (déclarations `PROCEDURE &ID(...)` sur ligne numérotée, §4.8.3 RETOUR) ; EXI-0006 ; §4.7 (ALLER EN à cible constante) ; §5 / IMP-0106 (table des objets compilés, recherche par numéro de ligne).
- Statut : retenu.

Les identificateurs locaux et les paramètres formels eux-mêmes (autres que leur seule arité, nécessaire à la validation d'un appel) ne sont pas portés par `EntreeSymbole` : ils appartiennent au cadre d'activation créé à chaque appel (état d'exécution, IV-FMT-0007), hors périmètre du présent chapitre. Leur déclaration — liste ordonnée des paramètres formels et liste `LOCAL` — est portée par l'élément de contrôle 0x0D placé en tête de la ligne de déclaration (§4.7 ; chapitre Compilateur §13.2, IMP-0174). Les champs `numero_ligne_entree` et `arite` d'`InfoProcedure` sont renseignés par le gestionnaire du programme courant lors de l'intégration de cette ligne, selon la règle de LSE15 §7-6, Note (déclaration de plus grand numéro), et valent `{0, 0}` tant qu'aucune déclaration n'est en vigueur (chapitre Compilateur §13.5, IMP-0176).

Chaque nom n'est conservé qu'une seule fois (ANN-K-0009) ; toute référence RCI-M à ce symbole utilise l'index 16 bits défini en IMP-0104, jamais le nom textuel répété.

---

## 7. Table des constantes partagées

**IMP-0107** — *Table des constantes partagées : chaînes littérales longues des lignes de programme, entrées jamais retirées en cours de session*
- Sujet : quelles constantes sont partagées plutôt qu'encodées immédiatement dans la séquence (§4.4), sous quelle forme physique la table est-elle tenue, et quelle est la durée de vie d'une entrée ?
- Choix :
  - **Constantes concernées.** Toute valeur est encodée immédiatement (§4.4), sauf les **chaînes littérales d'une ligne de programme dont la longueur dépasse le seuil de partage** (16 caractères) : celles-ci sont toujours placées dans la table des constantes partagées et désignées par le sous-code 0x03 en mode index (IMP-0128). La compilation recherche d'abord un texte identique dans la table et ne l'insère que s'il en est absent (chapitre Compilateur, IMP-0181) : sous EXI-0001, le compilateur ne peut savoir, à la première occurrence d'une chaîne, si elle sera réutilisée ailleurs, et c'est le passage systématique par la table qui rend le partage effectif dès la deuxième occurrence. Les chaînes d'une ligne machine de bureau sont toujours encodées en ligne (§7.1).
  - **Structure.** Un tableau d'entrées, agrandi par `realloc` et indexé par un entier non signé de 16 bits porté par l'élément 0x03 :

```c
typedef struct {
    unsigned char  longueur;   /* longueur du texte, au plus 78 (ligne de 80 caractères, guillemets exclus) */
    HandleObjet    texte;      /* objet du gestionnaire d'objets ; jamais modifié après création */
} EntreeConstante;
```

  - **Durée de vie.** Une entrée n'est jamais retirée en cours de session, même lorsque plus aucune ligne ne la référence, selon la même politique que la table des symboles, qui n'est elle-même jamais réduite. La table est vidée, avec la table des symboles, lorsque le programme courant est remplacé ou effacé (gestionnaire du programme courant, chapitre distinct) ; les entrées qui ne sont plus référencées sont écartées à la sérialisation (RCI-S, chapitre distinct, comme pour les symboles, IMP-0104). Une entrée créée pour une ligne finalement rejetée reste inerte, comme une entrée de symbole (chapitre Compilateur §3).
- Alternatives envisagées : table de constantes systématique pour toute valeur, y compris les petits entiers (rejeté : le coût de l'indirection dépasse le coût de l'encodage immédiat pour les petites valeurs, très majoritaires) ; aucune table (rejeté pour les grandes chaînes répétées : contraire à ANN-K-0011) ; ne placer dans la table qu'une chaîne déjà rencontrée ailleurs (rejeté : la première occurrence, déjà compilée en ligne, ne pourrait plus être partagée sans recompiler sa ligne, ce qu'EXI-0001 exclut) ; compteur de références par entrée, tenu par le programme courant à chaque insertion ou suppression de ligne (rejeté : plus économe en mémoire au cours d'une longue session d'édition, mais impose de parcourir chaque ligne insérée ou retirée, pour un gain limité ; la politique retenue est celle, plus simple, de la table des symboles).
- Justification : ANN-K-0011 impose la représentation unique d'une constante réutilisée « lorsque sa réutilisation est possible et avantageuse » ; le seuil de taille rend ce compromis explicite, et le passage systématique par la table au-delà du seuil le rend réalisable sous EXI-0001.
- Impact mémoire : évite la duplication des grandes chaînes ; pour une chaîne longue employée une seule fois, surcoût de 3 octets par entrée plus l'en-tête de l'objet texte ; entrées non référencées conservées jusqu'au remplacement du programme.
- Impact disque : la table, filtrée des entrées non référencées, devient une table de RCI-S.
- Impact performances : une recherche linéaire par chaîne longue à la compilation ; une indirection à l'exécution pour les seules chaînes partagées.
- Impact historique : neutre.
- Références normatives : ANN-K-0011, ANN-K-0012, EXI-0001, EXI-0016 ; IMP-0104, IMP-0109, IMP-0128 ; chapitre Compilateur, IMP-0181.
- Statut : retenu ; seuil exact (16 caractères) à valider par mesure sur corpus de programmes historiques, la règle restant correcte quel que soit le seuil.

**IMP-0128** — *Chaîne littérale et chaîne partagée réunies sous un seul sous-code, distinguées par un bit de mode*
- Sujet : IMP-0107 exige un mécanisme physique pour référencer une chaîne partagée par index de table plutôt que par texte en ligne. `RCIM_CAT_VALEUR` étant saturée à 64/64 depuis IMP-0123, aucun sous-code n'est disponible pour l'introduire séparément.
- Choix : le sous-code 0x03 (chaîne littérale) porte désormais un octet de mode : bit 7 = 0 pour le texte en ligne (bits 6-0 = longueur, 0–127), bit 7 = 1 pour un index de 2 octets dans la table des constantes partagées (§7). Aucun sous-code supplémentaire n'est consommé.
- Alternatives envisagées : consommer le seul sous-code encore disponible ailleurs dans le catalogue pour cet usage (rejeté : aucun sous-code n'est disponible dans aucune catégorie à ce stade du chapitre, hormis par un choix équivalent de réinterprétation d'un sous-code existant — cf. §4.11.8 pour la même politique appliquée à `RCIM_CAT_OPERATION`) ; renoncer au partage de constantes plutôt que de réinterpréter 0x03 (rejeté : romprait ANN-K-0011, déjà invoquée par IMP-0107, sans justification physique réelle).
- Justification : la longueur maximale d'une chaîne littérale en ligne descend de 255 à 127 caractères, sans aucune perte pratique : aucune ligne L.S.E. ne peut dépasser 80 caractères au total (INRDP75, cf. IMP-0125), donc aucune chaîne littérale ne peut historiquement atteindre 127 caractères. Le coût de cette fusion est donc nul en pratique, et le gain (aucun sous-code supplémentaire consommé) résout une incompatibilité qui aurait sinon rendu IMP-0107 inapplicable.
- Impact mémoire : aucun changement pour une chaîne en ligne (toujours 1 octet de mode/longueur + `n` octets, exactement comme avant la fusion) ; une chaîne partagée coûte 1 octet de mode + 2 octets d'index, contre `n` octets pour chaque occurrence dupliquée évitée.
- Impact disque : le bit de mode se sérialise sans transformation en RCI-S.
- Impact performances : un masquage de bit (`& 0x80`) pour déterminer le mode, coût négligeable.
- Impact historique : neutre — choix de représentation physique pur, aucune chaîne littérale historiquement valide ne peut dépasser la longueur en ligne désormais disponible.
- Références normatives : IMP-0107 ; IMP-0123 (origine de la saturation de `RCIM_CAT_VALEUR`) ; IMP-0125 (limite de 80 caractères par ligne) ; §4.11.8 (même politique de réinterprétation appliquée à `RCIM_CAT_OPERATION`).
- Statut : retenu.

### 7.1 Constantes introduites en mode machine de bureau

Une ligne machine de bureau n'introduit jamais de constante partagée : ses chaînes littérales sont toujours encodées en ligne dans le bloc transitoire (IMP-0109), quelle que soit leur longueur, le partage n'apportant rien à un bloc libéré aussitôt après son exécution (IMP-0107 ; chapitre Compilateur, IMP-0181). La table des constantes partagées ne contient donc que des chaînes issues de lignes de programme.

---

## 8. Organisation mémoire physique (zones)

| Zone | Contenu | Remarque de dimensionnement |
|---|---|---|
| Table des objets compilés | `EntreeObjetCompile[]` du programme courant (hors machine de bureau, cf. IMP-0109) | ~7 octets/ligne ; un programme de 2000 lignes ≈ 14 Kio |
| Blocs de séquences compilées | Charges utiles RCI-M (§4) | variable selon compacité effective du programme |
| Bloc transitoire machine de bureau | Un objet compilé temporaire à la fois, hors table (IMP-0109) | taille d'une ligne, libéré après exécution |
| Table des symboles | `EntreeSymbole[]` (nom, nature, origine, `InfoProcedure` ; aucune valeur) | 10 octets par nom distinct rencontré dans le programme courant ou en mode machine de bureau |
| Environnement des identificateurs | `EntreeEnvironnement[]`, indexé comme la table des symboles | hors périmètre RCI-M — état d'exécution (chapitre État d'exécution, IMP-0179) |
| Table des constantes partagées | `EntreeConstante[]` (§7, IMP-0107) : chaînes littérales de plus de 16 caractères des lignes de programme ; textes portés par le gestionnaire d'objets | 3 octets par entrée plus l'objet texte ; jamais réduite en cours de session, vidée avec le programme courant |
| Pile d'évaluation | valeurs intermédiaires pendant l'interprétation (Tome III §6.6) | hors périmètre RCI-M — état d'exécution |
| Cadres d'activation, contextes de boucle (§4.8.2) | hors périmètre RCI-M — état d'exécution | cf. IV-FMT-0007/0010, chapitre dédié du Tome V |

**IMP-0108** — *Séparation physique programme courant / état d'exécution*
- Sujet : faut-il allouer RCI-M et l'état d'exécution dans une zone mémoire commune ?
- Choix : zones physiquement distinctes, avec un gestionnaire d'objets par zone.
- Alternatives envisagées : allocation indifférenciée dans un tas unique.
- Justification : Tome III §6.7 impose qu'une opération de contrôle ne modifie jamais la représentation compilée du programme courant, seulement l'état d'exécution ; une séparation physique rend cette garantie vérifiable structurellement (une écriture accidentelle dans l'état d'exécution ne peut pas atteindre les blocs RCI-M).
- Impact mémoire : léger surcoût de gestion (deux allocateurs plutôt qu'un), négligeable devant le gain de sûreté.
- Impact disque : sans objet.
- Impact performances : neutre.
- Impact historique : neutre.
- Références normatives : Tome III §6.7 (invariant), conditionnement §9.
- Statut : retenu.

---

## 9. Algorithmes de parcours (réalisation physique des interfaces ANN-K-0008)

```c
typedef struct {
    const unsigned char *base;      /* résolution temporaire du HandleObjet */
    unsigned int          longueur; /* = EntreeObjetCompile.longueur_octets */
    unsigned int          position; /* offset courant, 0 <= position <= longueur */
} CurseurSequence;

void premier_element(CurseurSequence *c, const unsigned char *base, unsigned int longueur);
int  fin_sequence(const CurseurSequence *c);
TagRCIM lire_tag(const CurseurSequence *c);
void element_suivant(CurseurSequence *c);
```

`element_suivant` détermine la longueur à sauter uniquement à partir du tag courant, sans jamais consulter le texte source (EXI-0012).

---

## 10. Correspondance avec les invariants ANN-K-0032 et EXI-0001

| Invariant / exigence | Réalisation physique correspondante |
|---|---|
| EXI-0001 (compilation indépendante des lignes) | Aucun élément RCI-M d'une ligne ne dépend de l'existence, du contenu ou de l'ordre de compilation d'une autre ligne ; en particulier, la boucle FAIRE (§4.8.2, IMP-0112) ne porte aucune marque physique sur sa ligne de fin `n1`, la corrélation étant assurée exclusivement par l'état d'exécution |
| 1. RCI-M indépendant de toute adresse mémoire | Références (§4.5), branchements (§4.7), ouverture de boucle (§4.8.2) utilisent des identifiants/offsets logiques |
| 3. Interprétable sans texte source | Le texte source n'est jamais un champ de `EntreeObjetCompile` ni d'`EntreeSymbole` (ANN-K-0030) |
| 4. Décompilable selon les capacités définies | Chaque sous-format porte l'information nécessaire à sa restitution textuelle |
| 6. Toute référence résoluble dans son contexte | Résolution via `EntreeSymbole` par index 16 bits |
| 7. Tables cohérentes avec les références | Tout identifiant de symbole porté par une référence indexe une entrée existante de la table des symboles (créée par le compilateur avant toute écriture de la référence, chapitre Compilateur §7) ; toute position intra-séquence ne dépasse pas la longueur du bloc (IMP-0105) |
| 9. Information reconstructible non dupliquée | Constantes partagées (§7), noms non répétés (§6), objets transitoires machine de bureau non conservés (§3 bis) — *a contrario*, le commentaire (§4.9), non reconstructible, est conservé sans arbitrage |

---

## 11. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0101 | Granularité physique : un bloc par ligne | retenu |
| IMP-0102 | Encodage tag + charge utile variable | retenu |
| IMP-0103 | Trois formats numériques immédiats | retenu |
| IMP-0104 | Identifiants de symboles 16 bits ; frontière de filtrage RCI-S | retenu, volet RCI-S renvoyé au chapitre dédié |
| IMP-0105 | Branchements intra-ligne : position comptée depuis le début du bloc ; séquence bornée à 255 octets | retenu |
| IMP-0106 | Recherche dichotomique des lignes | retenu |
| IMP-0107 | Table des constantes partagées : chaînes littérales de plus de 16 caractères des lignes de programme, toujours placées dans la table ; entrées jamais retirées en cours de session | retenu, seuil à valider par mesure |
| IMP-0108 | Séparation physique RCI-M / état d'exécution | retenu |
| IMP-0109 | Objets compilés transitoires du mode machine de bureau (EXI-0028) | retenu |
| IMP-0110 | Accès direct aux variables arithmétiques simples par l'identifiant de symbole, valeur portée par l'environnement des identificateurs (état d'exécution) | retenu |
| IMP-0111 | Cibles de branchement calculées (ALLER EN / RETOUR EN) | retenu |
| IMP-0112 | Représentation physique de FAIRE : trois éléments sur la seule ligne FAIRE (ouverture, pas, test), corrélation par l'état d'exécution (conforme EXI-0001) | retenu |
| IMP-0113 | Représentation physique du commentaire (`*` jusqu'à fin de ligne) | retenu |
| IMP-0114 | Identification des procédures et fonctions par numéro de ligne d'entrée | retenu |
| IMP-0115 | Représentation physique d'AFFICHER : marque de tête (0x37 ou 0x38), format et valeurs en suite plate, affichage d'un tableau décidé à l'exécution (0x36) | retenu |
| IMP-0116 | Compaction en un seul octet des champs indicateur/facteur/type (0x30–0x35) | retenu |
| IMP-0117 | Catalogue verrouillé des opérations codées (arithmétique, relationnel/logique, valeur, appel/déclaration/fichiers, LIRE) | retenu |
| IMP-0118 | Opérateur d'affectation (`←`) | retenu |
| IMP-0119 | Représentation versionnée des symboles `@` et `$` (T1600/Mitra 15) | retenu |
| IMP-0120 | Fusion des sous-codes de référence par forme physique, non par catégorie déclarée | retenu |
| IMP-0121 | Indicateur de position d'appel (instruction/expression) sur APPEL | retenu |
| IMP-0122 | Octet d'arité explicite pour les opérations à arité variable | retenu |
| IMP-0123 | Entier immédiat [−28, +28] porté entièrement par le sous-code, sans charge utile | retenu |
| IMP-0124 | Nom du symbole stocké en ligne dans `EntreeSymbole`, sans pointeur | retenu |
| IMP-0125 | Numéro de ligne logique et offset intra-séquence codés sur 1 octet | retenu |
| IMP-0126 | Sous-code unique (0x38) pour le format implicite `[/,U]` d'AFFICHER | retenu |
| IMP-0127 | `NatureSymbole`/`OrigineSymbole` en `unsigned char` plutôt qu'en `enum` | retenu |
| IMP-0128 | Chaîne littérale et chaîne partagée réunies sous 0x03, distinguées par un bit de mode | retenu |
| IMP-0184 | Ordre des octets : octet faible en tête pour tout champ de plusieurs octets | retenu |
| IMP-0183 | Règles de restitution : fidélité de toute construction conservée, forme canonique pour la présentation non conservée, marque de tête de LIRE (0x11) | retenu |
| IMP-0177 | Nombre d'arguments effectifs porté par l'élément APPEL (second octet de charge utile de 0x28) | retenu |

---

## 12. Traçabilité

```text
HIS-4.5.1, HIS-4.5.2, HIS-7, HIS-8.2, HIS-14.1, HIS-4.7.2, HIS-6.2, INRDP75 Annexe III
  → EXI-0001, EXI-0002, EXI-0003, EXI-0010 à EXI-0016, EXI-0028, EXI-0046
    → VM-2401 (Tome III §6.1 à §6.9, dont DONNÉES MANIPULÉES : « informations
       nécessaires à la décompilation »), VM-1402 (Tomes I-II §14.2), VM-1502
       (Tome III §18.3, Formateur des sorties)
      → ANN-K-0004 à ANN-K-0032 (Tome IV)
        → IMP-0101 à IMP-0128, IMP-0177, IMP-0183, IMP-0184 (présent chapitre) ; sous-code 0x0D reporté de
          IMP-0174 (chapitre Compilateur)
          → CODE : rcim.h, rcim.c, symboles.h, symboles.c,
                   objets.h (module distinct, Organisation du dépôt §2.1)
```

---

## 13. Limites explicites de ce chapitre

- Ne couvre pas RCI-S (représentation sérialisée / fichier-programme) : chapitre distinct du Tome V, s'appuyant sur ANN-K-0016 à ANN-K-0028, sur les points de filtrage identifiés en §4.5 (IMP-0104) et §7.1, et sur la conservation obligatoire, non filtrable, des commentaires (§4.9, IMP-0113).
- Ne couvre pas la représentation physique complète de l'état d'exécution (pile d'évaluation, cadres d'activation, piles de contextes de boucle, IV-FMT-0010) : chapitre distinct du Tome V. L'algorithme exact de recherche et de désimbrication des contextes de boucle actifs (§4.8.2, IMP-0112) y est explicitement renvoyé ; le présent chapitre garantit seulement que RCI-M lui fournit l'information nécessaire sans jamais violer EXI-0001.
- Le catalogue des sous-codes d'opérations est verrouillé par IMP-0117 (§4.11) ; la sémantique détaillée de chaque opération à l'exécution relève du chapitre consacré à l'interpréteur (module « moteur d'exécution »).
- Le seuil de partage des constantes (§7) doit être confirmé par mesure sur le corpus de tests historiques.
- Ne couvre pas la représentation physique du gestionnaire d'objets lui-même (stratégie d'allocation, algorithme de compaction, format de l'en-tête de bloc) dont dépend le type opaque `HandleObjet` utilisé dans tout ce chapitre (§3, note de dépendance) : chapitre distinct du Tome V, non encore rédigé.
- La question, laissée ouverte en IMP-0104, de la reconstructibilité de la nature d'un symbole à la restauration RCI-S reste à trancher dans le chapitre dédié.
- Le catalogue des opérations codées (§4.11) fixe les sous-codes et les conventions de pile, mais ne détaille pas : l'algorithme de formatage numérique F/E/U (renvoyé à VM-1502, cf. §4.10), la sémantique exacte d'un libellé ou d'une mise en page dans le format de LIRE (point ouvert explicite, §4.11.6, faute de documentation source suffisante), le format des enregistrements de fichiers-données et le protocole de GARER/CHARGER/SUPPRIMER/EXECUTER (gestionnaire des programmes/fichiers, gestionnaire des tâches), et le traitement des commandes du moniteur volontairement exclues de RCI-M (§4.11.7). Ces points relèvent de chapitres distincts du Tome V.
- L'algorithme exact de formatage numérique (arrondi, alignement des espaces selon `e`/`d`, calcul de mantisse/exposant pour E et U) relève du Formateur des sorties (VM-1502), chapitre distinct du Tome V ; le présent chapitre garantit seulement que RCI-M porte l'information nécessaire à son application (§4.10, IMP-0115).
- NOYELLE89 est disponible dans le corpus documentaire de ce chapitre et a été consulté pour la confrontation d'exhaustivité du catalogue des opérations codées (IMP-0117) ; il s'agit toutefois d'un récit historique de la conception du langage, sans contenu syntaxique ou technique directement exploitable pour ce chapitre (aucune mention du format de LIRE, de SI en position d'expression, ni des sous-codes ou plages numériques discutés ici). Les vérifications techniques de ce chapitre reposent donc, dans les faits, sur INRDP75 et LSE15.
- La règle de préséance d'INRDP75 sur LSE15 en cas de désaccord factuel (§4.11.1) repose sur le rôle d'INRDP75 comme source de référence des deux profils d'implantation définis par les tomes ; la version du système Mitra 15 décrite par LSE15 n'est pas identifiée. Une revue systématique de l'ensemble du corpus pour détecter d'éventuels autres désaccords entre les deux manuels reste à effectuer ; cette règle étant de portée documentaire plutôt qu'implémentatoire, sa consignation définitive relève du Référentiel normatif (catalogue `DEC`), non du présent chapitre.

---

## 14. Tests de représentation associés

- Construction et parcours d'un objet compilé minimal (une ligne, une valeur immédiate entière).
- Postfixation vérifiée sur `A + B`, `A + B * C` (Tome III §6.5) et `A ↑ B ↑ C`, qui compile `A`, `B`, `0x05`, `C`, `0x05` — associativité à gauche (LSE15 §3-3.4) : `2↑3↑2` vaut 64 à l'exécution.
- Vérification qu'un branchement conditionnel (§4.7) ne modifie jamais les octets de la séquence compilée (invariant Tome III §6.7).
- Vérification qu'une valeur immédiate entière tient sur le plus petit sous-format capable de la représenter exactement (IMP-0103).
- `FAIRE 8 POUR I←T PAS -1 JUSQUA 1` (déjà cité pour IMP-0112) illustre également IMP-0123 : les constantes `-1` et `1` compilent chacune un unique élément de valeur immédiate sans charge utile (`-1` → sous-code 0x22 ; `1` → sous-code 0x24), tandis que `8` (valeur de `n1`, compilée en tête de la séquence de la ligne FAIRE) compile de la même façon un sous-code 0x2B, sans qu'aucun octet supplémentaire ne soit nécessaire pour aucune de ces trois constantes. `100` compilerait en revanche un sous-code 0x00 suivi de l'octet `64` (entier court, hors de l'intervalle [−28, +28]), et `200`, qui dépasse la plage d'un octet signé, un sous-code 0x01 suivi des octets `C8 00` (entier, octet faible en tête, IMP-0184).
- Vérification de non-duplication d'un symbole référencé plusieurs fois dans la même ligne (IMP-0104, ANN-K-0009).
- Vérification que le déplacement physique (compaction) d'un bloc par le gestionnaire d'objets ne modifie aucune référence logique.
- Une ligne exécutée en mode machine de bureau ne crée aucune entrée dans la table des objets compilés (IMP-0109).
- `ALLER EN 4` (cible constante) compile un sous-code 0x02 ; `ALLER EN I` (cible variable) compile la référence de `I` suivie d'un sous-code 0x03 (IMP-0111).
- `FAIRE 8 POUR I←T PAS -1 JUSQUA 1` compile, dans cet ordre : la valeur immédiate `8` (`n1`), la référence de `T` (`v1`), l'élément d'ouverture 0x09 portant l'identifiant de `I`, la valeur immédiate `-1`, l'élément 0x0A (pas explicite), la valeur immédiate `1` (`v2`), puis l'élément 0x0F (test JUSQUA) — aucun élément physique n'est ajouté à la ligne 8 (la ligne `n1`), quel que soit son propre contenu (IMP-0112).
- `200 FAIRE 202 POUR I←1 JUSQUA N` (LSE15 §5-5.3, pas omis) compile `202`, `1`, l'ouverture 0x09 (`I`), l'élément 0x0E (pas implicite, sans aucune sous-séquence de pas), la référence de `N`, puis 0x0F ; le décompilateur restitue l'instruction sans `PAS`, et `PAS 1` écrit explicitement compilerait au contraire la valeur `1` suivie de 0x0A.
- `105 FAIRE 105 TANT QUE A<4` (profil `LSE F4`, IMP-0277) compile `105`, l'ouverture 0x12 sans charge utile, la sous-séquence de la condition, puis 0x10, sans élément de pas.
- `10 FAIRE 12 POUR K←1 TANT QUE X[K]>=0 ET K<=50` (LSE15 §5-5.3) compile la sous-séquence de la condition **après** l'ouverture 0x09 et l'élément de pas 0x0E, puis l'élément 0x10 : à l'exécution, la condition est évaluée avec `K` valant 1 au premier passage, puis avec la valeur incrémentée à chaque fin d'itération. De même, `20 FAIRE 20 POUR X←A PAS (A/X-X)/2 TANT QUE ABS(X↑2-A)>1E-3` réévalue le pas `(A/X-X)/2` avec la valeur courante de `X` à chaque itération, avant de l'ajouter à `X` puis de réévaluer la condition (IMP-0112).
- La compilation de la ligne `n1` d'une boucle FAIRE produit exactement la même séquence RCI-M, que la ligne FAIRE correspondante existe déjà, n'existe pas encore, ou soit compilée après elle — vérification directe du respect d'EXI-0001 (IMP-0112).
- `3 SI A=0 ALORS DEBUT FAIRE 5 POUR I←0 JUSQUA 9; J←I+1 FIN SINON ALLER EN 206` (LSE15 §5-5.2) : la ligne 3 compile un unique bloc de séquence (§4.8.4) portant la condition, `0x00`, la séquence FAIRE (valeur `5`, valeur `0`, ouverture 0x09 portant `I`, pas implicite 0x0E, valeur `9`, test 0x0F), immédiatement suivie — sans séparateur physique, simple concaténation — de la sous-séquence de `J←I+1`, puis `0x01`, puis la sous-séquence de `ALLER EN 206` (IMP-0112). Test de non-régression du mécanisme de corrélation : après exécution de la ligne 5, la reprise doit cibler l'élément 0x0E qui suit l'ouverture (ajout du pas, puis réévaluation de la limite et test 0x0F), puis, si la boucle continue, l'élément d'affectation `J←I+1` à l'intérieur de la ligne 3 — jamais l'élément d'ouverture 0x09 lui-même.
- Un `ALLER EN` explicite ciblant la ligne d'une instruction FAIRE déjà active (boucle en cours) doit réexécuter l'élément d'ouverture et réaffecter `v1` à `V` sans condition, conformément à LSE15 §5-5.4 Remarque 2 (IMP-0112) — à la différence d'une reprise normale de fin d'itération, qui ne passe jamais par cet élément.
- Une variable arithmétique simple modifiée en boucle serrée (FAIRE...JUSQUA sur 1000 itérations) n'entraîne aucune résolution de `HandleObjet` (IMP-0110) — vérifiable par instrumentation du gestionnaire d'objets.
- Une instruction `SI ... ALORS DEBUT ... FIN SINON ...` tient intégralement dans un seul bloc de séquence compilée, quelle que soit la profondeur d'imbrication de SI internes (§4.8.4).
- `1 * CALCUL DE L'AIRE` compile un unique élément COMMENTAIRE (0x0C) portant le texte ` CALCUL DE L'AIRE`, espace de tête compris (tout ce qui suit le `*`, IMP-0183), sans aucun autre élément dans la séquence (IMP-0113) ; l'exécution de cette ligne n'a aucun effet observable.
- `5 S←B*H/2; * SURFACE DU TRIANGLE` compile la séquence normale de l'affectation suivie d'un élément COMMENTAIRE terminal ; le compilateur rejette toute tentative de faire suivre ce commentaire d'une nouvelle instruction sur la même ligne.
- LISTER restitue le texte exact du commentaire, espaces de tête compris, y compris pour une ligne ne contenant que ce commentaire (IMP-0183).
- `5 LIRE A,B` compile `[0x11][A][0x3B][B][0x3B]` et `5 LIRE A;LIRE B` compile `[0x11][A][0x3B][0x11][B][0x3B]` ; le décompilateur restitue chacune telle qu'elle a été écrite (IMP-0183).
- Restitution canonique (IMP-0183) : `5   X ← ( A + B ) * ( C )` est restitué `5 X←(A+B)*C` ; `5 X←A-(B-C)` et `5 X←(A-B)-C` sont restitués respectivement `5 X←A-(B-C)` et `5 X←A-B-C` ; `5 X←+003.01E21` est restitué `5 X←3.01E+21` ; `5 X←0.0001` est restitué `5 X←1E-04` et `5 X←1234567.5` est restitué `5 X←1.23457E+06` (six chiffres significatifs, exposant signé à deux chiffres) ; `5 X←0.4` est restitué `5 X←0.4` ; `5 SI A=0 ALORS DEBUT B←1 FIN` est restitué `5 SI A=0 ALORS B←1` ; `5 TABLEAU A[2];TABLEAU B[3]` est restitué `5 TABLEAU A[2],B[3]` ; chaque texte restitué se recompile en la même séquence.
- `10 PROCEDURE &PGCD(A,B)`, une fois intégrée au programme courant, donne une entrée `SYM_PROCEDURE` dont `procedure.numero_ligne_entree` vaut 10 et `procedure.arite` vaut 2, sans allocation d'aucun `HandleObjet` (IMP-0114 ; chapitre Compilateur IMP-0176) ; un remplacement ultérieur de la ligne 10 par une déclaration de la même procédure de même arité (EXI-0006) laisse cette entrée inchangée. La séquence compilée de la ligne se réduit à l'élément 0x0D (§4.7).
- La même déclaration `10 PROCEDURE &PGCD(A,B)` stocke `nom_longueur = 4` et `nom = "PGCD"` (sans le `&`) dans l'entrée de symbole (IMP-0124) ; le décompilateur restitue `&PGCD` en préfixant `&` à partir de `nature = SYM_PROCEDURE`. Une variable `I` stocke `nom_longueur = 1` et `nom[0] = 'I'`, les quatre octets restants de `nom[5]` n'étant pas significatifs.
- `AFFICHER['RES=',3F2.0,E3.6] A,B,C,D,E,A+B+C+D+E` compile, dans l'ordre du texte du format (principe d'ordre de compilation, §4.10.3) : la marque de tête 0x37, un élément 0x30 (libellé `RES=`, facteur fixe 1), un élément 0x35 (forme compacte : type F, répétition 3) suivi des sous-séquences de `A`, `B`, `C` (chacune suivie de 0x36), un élément 0x35 (type E, répétition 1) suivi de la sous-séquence de `D` (suivie de 0x36), puis, la liste de spécifications de description étant épuisée, `E` et `A+B+C+D+E` sont affichées selon la spécification U par défaut de l'interpréteur, sans qu'aucun élément 0x35 supplémentaire n'ait été compilé. **Contre-exemple à ne pas confondre avec un libellé** : `AFFICHER[3F2.0,E3.6] A,B,'FIN'` place `'FIN'` **après** le crochet fermant, dans la liste des valeurs — ce n'est donc jamais un libellé (0x30), qui n'existe que dans le format lui-même ; `'FIN'` y est une valeur-chaîne ordinaire (§4.4, sous-code 0x03), affichée comme `E` et `A+B+C+D+E` ci-dessus selon la spécification U par défaut, une fois les deux 0x35 épuisés par `A` et `B`.
- `AFFICHER 'OUI'` compile une valeur immédiate 0x03 dont l'octet de mode a le bit 7 à 0 et les bits 6-0 à `3` (longueur), suivi des trois octets `O`,`U`,`I` (IMP-0128) — forme en ligne, identique en tout point à celle d'avant la fusion avec le mécanisme de partage. Si la chaîne `'MESSAGE D'ERREUR TROP LONG POUR TENIR EN LIGNE'` (au-delà du seuil de partage, §7) apparaît à plusieurs endroits du même programme, chaque occurrence — la première comprise — compile en revanche un élément 0x03 dont l'octet de mode a le bit 7 à 1, suivi de 2 octets portant le même index dans la table des constantes partagées (IMP-0107, IMP-0128) — le texte lui-même n'étant stocké qu'une seule fois, dans cette table.
- `AFFICHER[*X,F0.3] NBSP,RESUL` compile la marque de tête 0x37, la sous-séquence de `NBSP` (poussant sa valeur en pile) suivie d'un élément 0x32 dont l'octet facteur a le bit 7 positionné (mode variable, bits 6-0 ignorés), puis un élément 0x35 (forme compacte : type F, e=0, d=3, répétition 1) suivi de la sous-séquence de `RESUL` et d'un élément 0x36 ; la valeur de `NBSP` n'est jamais affichée, conformément au corpus.
- `AFFICHER A,B,C` compile `[0x38]` suivi des trois sous-séquences de `A`, `B`, `C` chacune suivie d'un élément 0x36 (IMP-0126) — un seul octet de format pour l'ensemble de l'instruction. `AFFICHER[/,U] A,B,C` produit, à titre de comparaison, le même effet observable (même passage à la ligne, mêmes trois valeurs affichées selon U) mais via `[0x37,0x31,0x35]` suivi des mêmes trois sous-séquences : les deux formes ne sont physiquement identiques qu'à partir de la première sous-séquence de valeur, et le décompilateur restitue chacune telle qu'elle a été écrite (IMP-0115, IMP-0126).
- `AFFICHER[F2.0] A;AFFICHER[F3.0] B` compile `[0x37][0x35][A][0x36][0x37][0x35][B][0x36]`, alors que `AFFICHER[F2.0,F3.0] A,B` compile `[0x37][0x35][A][0x36][0x35][B][0x36]` : la marque de tête 0x37 distingue les deux écritures (IMP-0115).
- `AFFICHER MAT` compile `[0x38]` suivi d'une unique référence simple à `MAT` (§4.5, sous-code 0x00 — IMP-0120, sans octet `k`) et d'un élément 0x36, que la ligne `TABLEAU MAT[...]` ait été compilée avant ou après elle (EXI-0001) : à l'exécution, 0x36 affiche le tableau entier si `MAT` est alors un tableau, sa valeur sinon (IMP-0115).
- Au niveau de l'encodage seul (fonction d'écriture de RCI-M, indépendamment de la compilation d'une ligne complète) : une description `U` de facteur 80, supérieur à 63, s'écrit en un élément 0x35 dont l'octet 0 porte le marqueur d'extension (bits 5-0 = 0b111111) et dont l'octet 1 porte la valeur exacte 80 ; une description de facteur 50 s'écrit sur un seul octet, bits 5-0 = 49 (IMP-0116). La règle de LSE15 §6-2.4 (IMP-0178) imposant au moins autant de valeurs que de répétitions, une ligne de 80 caractères ne peut porter assez de valeurs pour qu'une ligne acceptée produise la forme étendue ; celle-ci reste définie pour couvrir la plage 1–99 documentée.
- `AFFICHER[E4.6] A` où `A` est un tableau à deux dimensions compile la marque de tête 0x37, un élément 0x35 (type E), une référence simple à `A` (§4.5, sous-code 0x00) et un élément 0x36, qui affiche à l'exécution les six éléments selon la seule spécification E4.6 (LSE15 §6-2.5) ; aucune itération sur les éléments du tableau n'apparaît physiquement dans RCI-M (IMP-0115).
- `A+B*C` compile la séquence `A` (référence), `B` (référence), `C` (référence), `0x03` (multiplication), `0x00` (addition) — ordre postfixé conforme à EXI-0011, avec les opérandes de plus haute précédence évalués en premier sans qu'aucune information de précédence ne soit portée physiquement.
- `RAC(SIN(X)↑2+COS(Y)↑2)` compile : référence de `X`, `0x0A` (SIN), valeur immédiate 2, `0x05` (exponentiation), référence de `Y`, `0x0B` (COS), valeur immédiate 2, `0x05`, `0x00` (addition), `0x06` (RAC) — chaque fonction élémentaire consommant exactement un opérande déjà en pile (IMP-0117).
- `TABLEAU MAT[10,5]` compile un élément `0x29` dont la charge utile porte l'identifiant de symbole déjà créé pour `MAT` et le nombre de dimensions (2), précédé des sous-séquences des deux expressions de dimension (10 et 5).
- `POS('JE TRAVAILLE',1,'AILLE')` compile trois sous-séquences de valeur immédiate chaîne suivies d'un élément `0x1E` (POS), consommant exactement 3 valeurs, conformément à l'arité fixe de cette fonction (§4.11.4).
- `CNB(PH,25,T)` où `T` reçoit la position d'arrêt compile les sous-séquences de `PH` et de la valeur immédiate 25, suivies d'une **référence** (§4.5, sous-code 0x00) de `T` — non d'une valeur — puis d'un élément `0x21` (CNB) portant l'octet d'arité `3` (IMP-0122) : le paramètre de sortie `va` est physiquement une référence, conformément à la convention signalée en §4.11.4. `CNB(PH,25)` (sans `T`) compilerait la même chose sans la référence, avec un octet d'arité `2` — la différence est explicite dans la séquence compilée elle-même, non laissée à la charge de l'interpréteur.
- `GARER OUTIL,1,'OUTIL'` (INRDP75, programme de gestion de fichiers) compile une référence de `OUTIL` (§4.5), les sous-séquences des deux expressions, puis un élément `0x2C` (GARER).
- Une commande `CATALOGUER W1,W2` saisie au moniteur ne produit aucun élément RCI-M : elle est traitée entièrement par l'analyseur de commandes, hors compilation de ligne (§4.11.7).
- `A←B+1` compile la référence de `A`, la référence de `B`, la valeur immédiate 1, un élément `0x00` (addition), puis un élément `0x19` (AFFECTATION) (IMP-0118).
- `10 ALLER EN @+3` (cible T1600) compile un élément de référence 0x02 (§4.5, symbole `@`), la valeur immédiate 3, un élément d'addition (`0x00`), puis un élément de contrôle 0x03 (§4.7, ALLER EN à cible calculée). Le même programme, compilé pour Mitra 15, est rejeté par le compilateur puisque `@` n'y a pas cette signification (IMP-0119).
- `10 I←@50` (Mitra 15) compile la référence de `I`, un élément de valeur immédiate `0x06` (constante marquée numéro de ligne 50), puis un élément `0x19` (AFFECTATION) ; la même ligne compilée pour T1600 est rejetée, `@` n'y admettant pas cet emploi (IMP-0119).
- `SI X>1 ALORS ALLER EN 18` (forme 1, sans SINON — §4.8.4.3) compile l'expression booléenne `X>1`, puis un unique élément `0x00` (branchement conditionnel intra-séquence) ciblant directement l'instruction suivante si la condition est fausse, puis la compilation normale de `ALLER EN 18` — **aucun** élément `0x01` n'est émis dans cet exemple, faute de branche SINON.
- `SI A=B ALORS SI XX=0 ALORS i1 SINON i2` (LSE15 §5-2, exemple d'associativité — §4.8.4.2) : le SINON se rattachant au second SI (règle d'associativité résolue par l'analyseur), la compilation produit deux paires imbriquées 0x00/0x01, la paire externe (SI A=B) n'ayant qu'une branche ALORS — donc un seul `0x00` externe, ciblant directement la fin —, et la paire interne (SI XX=0) ayant les deux branches — donc `0x00`/`0x01` complets pour i1/i2. Test de non-régression : vérifier que la cible du `0x00` externe (en cas d'échec de A=B) saute par-dessus l'intégralité de la paire interne, sans jamais atteindre i2 ni i1, conformément à « si A≠B, ni i1 ni i2 ne seront exécutées ».
- `CH←SI N>1 ALORS ' LETTRES ' SINON ' LETTRE '` (INRDP75, PG26 l.30) compile, en position d'expression (§4.8.5) : la référence de `CH`, la sous-séquence de la condition `N>1`, un élément `0x00` (branchement conditionnel → étiquette SINON), la valeur immédiate chaîne `' LETTRES '`, un élément `0x01` (saut inconditionnel → étiquette FIN), la valeur immédiate chaîne `' LETTRE '`, puis un élément `0x19` (AFFECTATION) consommant la valeur déposée par la branche exécutée — physiquement indiscernable, pour les sous-codes 0x00/0x01 eux-mêmes, de la forme instruction (§4.8.4).
- `27 RESULTAT SI N=0 ALORS 1 SINON N*&FACT(N-1)` (LSE15 §7-4) compile la sous-séquence conditionnelle (§4.8.5 : `0x00`, valeur immédiate `1`, `0x01`, puis la sous-séquence postfixée de `N*&FACT(N-1)`), suivie d'un élément `0x0B` (RESULTAT) consommant la valeur laissée en pile par la branche exécutée. Ce même exemple compile également, pour `&FACT(N-1)`, une référence simple à `&FACT` (§4.5, 0x00), la sous-séquence de `N-1`, puis un élément `0x28` (APPEL) avec indicateur `0x01` (position d'expression, IMP-0121) — puisque cet appel figure comme opérande de `*` — et `k = 1` (IMP-0177).
- `Y←&RAC(3,20);i1` (LSE15 §7-5) compile une référence à `Y` (§4.5, 0x00), une référence simple à `&RAC`, la sous-séquence des arguments `3` et `20`, un élément `0x28` avec indicateur `0x01` (position d'expression, IMP-0121) et `k = 2` (IMP-0177), puis un élément `0x19` (AFFECTATION) consommant la valeur laissée par l'appel, puis la sous-séquence de `i1`. Test de non-régression : si le corps de `&RAC` sort par `RETOUR EN ERRE` plutôt que par `RESULTAT` (cas `ARG<0`), l'interpréteur doit reconnaître, grâce à l'indicateur `0x01`, qu'aucune valeur n'a été produite pour `0x19`, et annuler l'affectation à `Y` plutôt que de tenter de la réaliser avec une valeur absente — conformément à LSE15 §7-5 (« la variable à laquelle la valeur de l'expression devait être affectée n'est pas changée »).
- Profil T1600 (chapitre Compilateur §8 bis, IMP-0180) : `5 X←SH(A)+SGN(B)` compile la référence de `A`, `0x3C` (SH), la référence de `B`, `0x3F` (SGN), puis l'addition `0x00` et l'affectation `0x19` ; `2 UN U;ZER T` compile la référence simple de `U` suivie de `0x1A`, puis la référence simple de `T` suivie de `0x1B` (INRDP75 Annexe III §1) ; `5 RETOUR EN $` compile la référence `0x03` suivie de l'élément de contrôle `0x06` (RETOUR EN à cible calculée).
- Un programme utilisant `SUPPRIMER` compile normalement pour une cible Mitra 15 (élément `0x2E`) mais le compilateur signale une incompatibilité de portabilité si la cible est déclarée T1600, conformément à la règle de compatibilité d'Annexe III (§4.11.5).