# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — Architecture physique du LSE-IIGS

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Environnement système :** GS/OS, lancement depuis le shell ORCA
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V (spécification détaillée complète de l'implémentation), niveau `05-IMPLEMENTATION/decisions/`
**Portée :** organisation physique générale de l'implémentation — découpage en un exécutable unique, réalisation physique des processeurs du Tome III, gestion logique des tâches en configuration mono-utilisateur, modèle mémoire et segmentation ORCA/C, stratégie d'allocation mémoire. Ce chapitre ne couvre pas le format physique de la représentation compilée (RCI-M, chapitre distinct, déjà finalisé) ni la représentation physique de l'état d'exécution (pile d'évaluation, cadres d'activation — chapitre distinct, non encore rédigé).
**Rang normatif :** subordonné au Catalogue des exigences, au Tome III (notamment VM-2201 à VM-2210, §3.11, §4 « Les espaces mémoire ») et au Tome IV. Aucune disposition du présent chapitre ne peut contredire ces sources. Il est complémentaire du chapitre RCI-M : RCI-M définit le format physique de la représentation compilée ; le présent chapitre définit l'organisation physique qui héberge cette représentation et les autres composants de la machine virtuelle.
**Place dans le Tome V :** ce chapitre précède logiquement RCI-M dans un plan de lecture (il fixe le cadre — exécutable, segments, mémoire — dans lequel RCI-M s'inscrit), mais les deux chapitres ont été rédigés dans l'ordre inverse et sont conçus pour être lus indépendamment. La numérotation `IMP-*` du présent chapitre poursuit celle de RCI-M (dernier identifiant utilisé : IMP-0128) à partir d'**IMP-0129**, afin de conserver un espace de décisions d'implémentation unique pour tout le Tome V.

---

## 0. Avertissement de statut

Comme pour RCI-M (cf. Tome IV §5.7, ANN-K-0044), aucune disposition du présent chapitre ne constitue une reconstruction historique. Le Tome III précise lui-même (préambule) que les notions de « processeur », « mémoire », « fichier » qu'il emploie désignent des abstractions architecturales, indépendantes de toute technologie d'implémentation. Tout ce qui est décrit ici — exécutable unique, segments ORCA/C, mécanismes d'allocation — relève exclusivement de décisions d'implémentation (`IMP-*`), prises au niveau 6 de la hiérarchie de résolution des ambiguïtés (conditionnement §18), lorsque les tomes I à IV ne fixaient aucune organisation physique particulière.

---

## 1. Rappel des contrats obligatoires (non renégociables)

### 1.1 Composants du Tome III concernés

| Composant (Tome III) | Identifiant | Contrainte physique imposée par ce chapitre |
|---|---|---|
| Environnement conversationnel | VM-2201 | Point d'entrée unique du dialogue ; ne doit dépendre d'aucune organisation interne des autres composants (Tome III §3.1, « Conséquences ») |
| Analyseur de commandes | VM-2202 | Orientation vers le processeur compétent par appel direct, sans rechargement de programme externe |
| Gestionnaire du programme courant | VM-2203 | Doit résider en mémoire pendant toute la session, indépendamment du texte source |
| Compilateur | VM-2204 | Doit pouvoir être invoqué à chaque ligne sans coût de chargement perceptible (EXI-0028) |
| Interpréteur / moteur d'exécution | VM-2205 / VM-2206 | Idem, invocation immédiate en mode machine de bureau comme en mode programmé |
| Environnement d'exécution | VM-2207 | Résolution de références sans dépendance à une adresse physique (Tome III §4.10) |
| Décompilateur | VM-2208 | Peut être sollicité de façon différée (commande LISTER), sans contrainte de latence conversationnelle aussi stricte que le compilateur |
| Bibliothécaire | VM-2209 | Conservation/restauration du programme courant sous forme compilée, indépendante du texte source |
| Gestionnaire des erreurs | VM-2210 | Mécanisme commun accessible depuis tous les composants |
| Gestionnaire des tâches (temps partagé) | VM-2105 (Tome III §3.11, §19.7) | Interface de conservation/restauration d'une tâche ; algorithme de sélection et représentation physique de la zone de swap non fixés (EXI-0058, EXI-0059) |

### 1.2 Exigences EXI directement concernées

| EXI | Objet | Conséquence physique imposée |
|---|---|---|
| EXI-0001 | Compilation indépendante des lignes | Aucune organisation physique ne peut introduire une dépendance inter-lignes à la compilation |
| EXI-0028 | Enchaînement immédiat compilation-exécution | Aucun mécanisme de chargement de composant ne doit introduire de latence perceptible entre la validation d'une ligne et son exécution |
| EXI-0052 | Traitement conversationnel des demandes | L'organisation physique retenue doit préserver le caractère conversationnel, quel que soit le composant sollicité |
| EXI-0058 | Gestion abstraite des tâches en temps partagé | L'état d'une tâche doit pouvoir être conservé et restauré indépendamment de son état d'activité, sans que l'algorithme de sélection ou la représentation physique de la zone de swap soient imposés |
| EXI-0059 | Moniteur de temps partagé et permutation des tâches | Un mécanisme permanent d'activation et de conservation des tâches doit exister, distinct du traitement fonctionnel des programmes et fichiers, sans que les règles exactes de sélection soient imposées |

### 1.3 Espaces mémoire logiques (Tome III §4)

Le Tome III distingue cinq espaces logiques (programme courant, exécution, objets dynamiques, persistant, conversationnel temporaire), auxquels s'ajoutent les deux espaces logiques des fichiers-données (temporaire, permanent — Tome III §4.6). Cette distinction est **fonctionnelle** : elle « n'implique aucune segmentation physique de la mémoire d'une implantation » (Tome III §4.1). Le présent chapitre réalise cette séparation fonctionnelle par une séparation physique effective (§6), choix qui va au-delà du minimum exigé par le Tome III mais qui rend certains invariants vérifiables structurellement plutôt que par convention de programmation (cf. IMP-0108, RCI-M §8, repris et étendu ci-dessous).

### 1.4 Interdictions du conditionnement (§4, §9, §19)

Aucune allocation massive ou spéculative ; aucune dépendance à une mémoire virtuelle moderne ; aucune hypothèse d'une mémoire abondante ; aucune Toolbox graphique ; les pointeurs C ne constituent pas un modèle architectural — toute référence inter-objets passe par un identifiant logique indépendant de l'emplacement mémoire courant (repris de RCI-M §1.4).

---

## 2. Contraintes de plateforme retenues (ORCA/C 2.2.1 / 65816)

Ce chapitre s'appuie sur les mêmes contraintes de plateforme que RCI-M §2 (tailles des types C, absence de bourrage de structure). Il ajoute les éléments suivants, spécifiques à l'organisation physique globale :

- ORCA/C alloue le code et les données statiques globales par blocs (*segments*) de 64 Kio au maximum, sauf usage explicite du grand modèle mémoire pour les tableaux/structures globaux (manuel ORCA/C, chap. 13 *memorymodel*, chap. 19 *Segment Statement*).
- L'allocation dynamique (`malloc`/`calloc`/`NewHandle`) n'est, elle, **pas** limitée à 64 Kio par bloc, quel que soit le modèle mémoire — la limite de 64 Kio ne porte que sur les structures globales *déclarées statiquement*.
- Sur Apple IIGS, `malloc` s'appuie sur le Memory Manager toolbox : toute demande de plus de 4096 octets devient une requête unique auprès du Memory Manager (bloc potentiellement déplaçable au niveau système, mais le pointeur rendu par `malloc` reste fixe pendant toute la durée de vie du bloc — ORCA/C n'expose pas de verrouillage/déverrouillage sur ces blocs) ; les demandes plus petites sont sous-découpées par le gestionnaire mémoire du runtime ORCA/C dans des blocs de 4 Kio (manuel ORCA/C, chap. 20, *malloc*).
- Le Memory Manager toolbox (`MEMORY.H`) est démarré dans tout programme C ORCA, y compris un programme purement texte, au même titre que le Tool Locator et SANE — il ne fait pas partie des outils graphiques (QuickDraw, Window Manager, Menu Manager, etc.) et son usage direct est donc compatible avec l'interdiction du conditionnement §19 portant sur la Toolbox graphique. `MMStartUp` et l'arrêt du Memory Manager sont faits par la bibliothèque d'exécution d'ORCA/C, non par l'application ; `userid()` rend l'identifiant maître avec un auxID égal à 1 (Apple IIGS Toolbox Reference, vol. 1, chap. 12 ; manuel ORCA/C 2.2 ; GS-20 *Toolbox C* ; détail dans Gestionnaire d'objets §11).
- Un segment dynamique ne peut jamais contenir `main` ; un échec de chargement d'un segment dynamique par manque de mémoire est non récupérable (notes de version ORCA/C 2.2.1, correctif #228).

---

## 3. Un seul exécutable, plusieurs modules et segments internes

### 3.1 Principe retenu

INRDP75 §3-3 et le Tome III (VM-2201 à VM-2210) décrivent les « processeurs » comme des responsabilités logiques distinctes, réalisées historiquement par des programmes système séparés, chargés l'un après l'autre par le moniteur de temps partagé, faute de mémoire suffisante pour les contenir tous simultanément sur T1600/Mitra 15. Cette contrainte n'existe plus sur un Apple IIGS à 4 Mio de RAM.

**IMP-0129** — *Un seul exécutable ORCA/C, processeurs réalisés comme modules internes*
- Sujet : faut-il réaliser les processeurs logiques (Tome III §3) comme des exécutables GS/OS séparés ou comme des composants internes d'un seul exécutable ?
- Choix : un seul exécutable texte (§5 du conditionnement — pas d'application desktop), dans lequel chaque processeur (VM-2201 à VM-2210) correspond à un module C et, autant que possible, à un segment de chargement ORCA/C distinct (§5 ci-dessous), mais jamais à un exécutable séparé.
- Alternatives envisagées :
  1. Un exécutable par processeur, communiquant par fichiers temporaires ou par la ligne de commande ORCA — écarté : romprait EXI-0028 (enchaînement immédiat compilation-exécution) et EXI-0052 (traitement conversationnel), en introduisant une latence et une sérialisation intermédiaire de l'état d'exécution, du programme courant et de la table des symboles à chaque transition entre processeurs.
  2. Un exécutable « moniteur » unique appelant des sous-programmes externes via le Program Loader GS/OS — écarté pour la v1 : complexité et risque d'échec de chargement disproportionnés par rapport au bénéfice réel, la segmentation ORCA/C (§5) couvrant déjà le besoin effectif, qui est la taille de code, non l'isolation de processus.
- Justification : VM-2201 (environnement conversationnel) et VM-2202 (analyseur de commandes) exigent un enchaînement immédiat entre orientation d'une entrée et traitement par le processeur compétent ; un seul espace d'adressage permet cet enchaînement par simple appel de fonction, sans aucune sérialisation intermédiaire de l'état d'exécution, du programme courant ou de la table des symboles.
- Impact mémoire : favorable — aucune duplication d'état entre processus, aucun tampon de communication inter-processus.
- Impact disque : un seul fichier exécutable à installer/lancer depuis ORCA (simplifie la documentation d'installation, §20.11 du conditionnement).
- Impact performances : élimine tout coût de chargement de programme au changement de processeur — coût qui existait historiquement parce que la mémoire du T1600/Mitra 15 ne pouvait pas contenir tous les processeurs simultanément ; ce n'est plus une contrainte réelle sur la cible retenue.
- Impact historique : neutre à favorable — le comportement observable (compilation transparente, EXECUTER explicite, enchaînement des processeurs décrit par INRDP75 fig. 06) est préservé à l'identique ; seul le mécanisme interne de mise en correspondance processeur → code change.
- Références normatives : VM-2201, VM-2202, EXI-0028, EXI-0052, Tome III préambule (« aucune référence à une organisation mémoire physique »), INRDP75 §3-3.
- Statut : **retenu**.

### 3.2 Conséquence sur le plan de fichiers source

Chaque processeur du Tome III (colonne « Architecture logique », §7 du conditionnement) correspond à une ou plusieurs unités de compilation C, regroupées ou réparties selon les besoins de réalisation, sans que cela modifie les responsabilités logiques (conditionnement §7, tolérance explicite). Le plan de segmentation (§5) précise la correspondance retenue entre ces unités et les segments ORCA/C.

---

## 4. Gestion logique des tâches en configuration mono-utilisateur

Le Tome III (§3.11, VM-2105) et EXI-0058/EXI-0059 exigent un gestionnaire des tâches capable de conserver et restaurer l'état d'une tâche inactive. EXI-0058 précise cependant explicitement, en observation, que l'exigence « ne fixe ni algorithme d'ordonnancement ni représentation physique de la zone de swap » ; EXI-0059 précise de même que « les règles exactes de sélection des tâches ne sont pas reconstruites à partir des sources disponibles ». Le conditionnement (§12) autorise, pour une première configuration mono-utilisateur, à isoler les fonctions non pertinentes sans les supprimer du modèle.

**IMP-0130** — *Gestionnaire des tâches minimal : une tâche unique, permanente, sans zone de swap*
- Sujet : réalisation physique du gestionnaire des tâches (VM-2105) pour une configuration mono-utilisateur, mono-session, mono-console.
- Choix : une seule tâche logique existe, créée à `BONJOUR` et détruite à `AU REVOIR` ; elle reste active en permanence (aucune permutation, aucun ordonnanceur). L'interface `SWAP_OUT(tache)` / `SWAP_IN(tache)` (Tome III §19.7) est implémentée mais réduite à des opérations sans effet, la tâche unique n'ayant jamais besoin d'être retirée de l'état actif ; le module correspondant existe et documente cette réduction plutôt que d'être absent. Aucune zone de swap sur disque n'est allouée en v1.
- Alternatives envisagées :
  1. Omettre entièrement le module gestionnaire des tâches — rejeté : le conditionnement §12 l'interdit explicitement (« elles ne doivent pas être supprimées du modèle ») et romprait la chaîne de traçabilité HIS → EXI-0058/EXI-0059 → VM-2105 → IMP → CODE exigée par le conditionnement §15.
  2. Implémenter un vrai swap disque dès la v1 par anticipation d'une extension multi-utilisateur — rejeté : coût de complexité et de performance disque sans bénéfice fonctionnel observable pour un seul utilisateur, contraire à la priorité « simplicité » (conditionnement §3) et à « fonctionnement fiable » en premier lieu (conditionnement §27).
- Justification : EXI-0058 et EXI-0059 autorisent explicitement cette latitude sur l'algorithme et la représentation physique ; conserver l'interface logique (créer / activer / suspendre / restaurer) permet une extension ultérieure vers une configuration multi-console sans reconception, conformément à l'exigence de traçabilité bidirectionnelle du Tome III (§1.3).
- Impact mémoire : favorable — aucune zone de swap réservée.
- Impact disque : aucune réservation de zone de swap dans ce périmètre ; point à réévaluer si une extension multi-console est un jour engagée.
- Impact performances : aucun surcoût d'ordonnancement.
- Impact historique : conforme — INRDP75 §3-1 décrit le temps partagé comme une optimisation nécessaire *parce que* plusieurs consoles se disputaient une mémoire exiguë ; en son absence (une seule console), le mécanisme dégénère naturellement vers l'identité, sans contredire le modèle logique.
- Références normatives : EXI-0058, EXI-0059, VM-2105, Tome III §3.11, §19.7, conditionnement §12.
- Statut : **retenu**.

---

## 5. Modèle mémoire ORCA/C et plan de segmentation

### 5.1 Choix du modèle mémoire

**IMP-0131** — *Petit modèle mémoire ORCA/C, code réparti par `segment`*
- Sujet : choix entre petit et grand modèle mémoire ORCA/C pour l'ensemble du programme.
- Choix : petit modèle mémoire (`#pragma memorymodel 0`, valeur par défaut — donc aucune directive nécessaire), avec répartition du code entre segments statiques nommés par grand composant logique (§5.2). Aucune variable globale scalaire de grande taille n'est déclarée ; toute structure susceptible de dépasser 64 Kio (table des objets compilés d'un gros programme, par exemple) est allouée dynamiquement par blocs, jamais comme tableau global fixe — cohérent avec RCI-M §2, qui a déjà retenu ce principe pour les mêmes raisons.
- Alternatives envisagées :
  1. Grand modèle mémoire pour tout le programme — rejeté par le manuel ORCA/C lui-même, qui recommande d'utiliser la directive `segment` plutôt que le grand modèle chaque fois que c'est possible, le compilateur générant alors un code plus compact et plus rapide.
  2. Grand modèle réservé aux seuls modules manipulant de grosses structures — rejeté, aucun module n'a besoin de dépasser la limite qui déclencherait cette exigence (64 Kio de variables globales scalaires, ou une allocation dynamique unique de plus de 64 Kio — ce dernier cas n'existe pas dans RCI-M, qui alloue par bloc-ligne, IMP-0101).
- Justification : cohérent avec RCI-M §2 ; le petit modèle produit un code plus compact et plus rapide sur 65816, ce qui sert la priorité « économie de mémoire et de stockage » (conditionnement §3, priorité 7) sans aucune perte fonctionnelle.
- Impact mémoire : code plus compact qu'en grand modèle (adressage absolu 2 octets au lieu d'adressage long 3 octets pour chaque référence à une variable globale ou fonction).
- Impact disque : exécutable plus petit.
- Impact performances : accès absolu aux globales, plus rapide que l'adressage long systématique du grand modèle.
- Impact historique : sans objet — décision de plateforme pure, aucune conséquence observable sur le comportement L.S.E.
- Références normatives : manuel ORCA/C chap. 13 (*memorymodel*), chap. 19 (*Segment Statement*) ; RCI-M §2, §3, IMP-0101.
- Statut : **retenu**.

### 5.2 Plan de segmentation

**IMP-0135** — *Segments statiques nommés par processeur, aucun segment dynamique en v1*
- Sujet : faut-il répartir les processeurs en segments ORCA/C statiques ou dynamiques ?
- Choix : segments **statiques**, nommés individuellement par grand composant logique, tous chargés au démarrage et résidents pendant toute la session. Aucun segment dynamique en v1.

| Processeur / responsabilité (Tome III) | Segment ORCA/C proposé | Chargement |
|---|---|---|
| Environnement conversationnel (VM-2201) | segment blanc (toujours résident) | statique |
| Analyseur de commandes (VM-2202) | segment blanc | statique |
| Gestionnaire du programme courant (VM-2203) | `"programme"` | statique |
| Compilateur (VM-2204) | `"compilateur"` et `"compil_ins"` (IMP-0197) | statique |
| Interpréteur / moteur d'exécution (VM-2205, VM-2206) | `"interprete"` et `"interp_es"` (IMP-0226) | statique |
| Environnement d'exécution (VM-2207) | `"contexte"` (choix d'Éric du 2026-10-23, E3 ; règle des 10 caractères, IMP-0197) | statique |
| Décompilateur (VM-2208) | `"decompil"` (IMP-0202) | statique |
| Bibliothécaire (VM-2209) | `"bibliothe"` (Bibliothécaire et fichiers IMP-0249) | statique |
| Gestionnaire des erreurs (VM-2210) | segment blanc | statique |
| Gestionnaire des variables / tableaux / chaînes / nombres | `"donnees"` | statique |
| Gestionnaire mémoire (objets, cf. §6) | `"objets"` | statique |
| Représentation compilée : éléments, table des symboles, table des constantes partagées (RCI-M) | `"rcim"` (IMP-0193, Organisation du dépôt §2.1) | statique |
| Fichiers-données, gestionnaire des tâches (§4) | `"fichiers"` | statique |

- Alternatives envisagées : rendre dynamiques les segments les moins fréquemment sollicités (décompilateur, bibliothécaire), afin d'économiser de la RAM tant que LISTER/GARER/CHARGER ne sont pas invoqués — rejeté pour la v1 : les notes de version ORCA/C 2.2.1 (correctif #228) signalent qu'un échec de chargement d'un segment dynamique par manque de mémoire est **non récupérable**, et que `main` ne peut jamais figurer dans un segment dynamique. Sur une machine conversationnelle où le décompilateur et le bibliothécaire sont sollicités fréquemment et de façon imprévisible au fil du dialogue, le risque d'échec de chargement tardif l'emporte sur l'économie de RAM espérée.
- Justification : conforme à la priorité « fonctionnement fiable » en premier lieu (conditionnement §27) et à « simplicité » (conditionnement §3, priorité 6) ; un plan de segmentation entièrement statique est plus facile à valider et à déboguer sur cible (conditionnement §27, « possibilité de débogage sur cible »).
- Impact mémoire : tous les segments résidents en permanence ; à mesurer sur cible (§7) pour confirmer que le budget disponible l'autorise confortablement.
- Impact disque : sans objet — un seul exécutable, chargé en une fois.
- Impact performances : aucun coût de chargement différé, aucune latence imprévisible au moment de LISTER ou GARER.
- Impact historique : neutre — décision de réalisation pure.
- Références normatives : manuel ORCA/C chap. 19 (*Segment Statement*) ; notes de version ORCA/C 2.2.1, correctif #228 ; conditionnement §3, §27.
- Statut : **retenu** — à réévaluer en segments dynamiques uniquement si la mesure sur cible réelle (§7) montre un dépassement de budget mémoire. Complété par IMP-0197 (processeur réparti sur plusieurs segments).

**IMP-0197** — *Un processeur dont le code dépasse 64 Kio occupe plusieurs segments statiques nommés*
- Sujet : sous le modèle mémoire small, un segment de chargement ne peut pas dépasser une banque de 64 Kio (manuel ORCA/C, chap. 19 ; message de l'éditeur de liens « Code exceeds code bank size »). Le 2026-10-23, le segment `"compilateur"` a atteint environ 75 Kio en profil `dev` (débogage au niveau source, IMP-0143) et approche cette limite en profil `cible`, avant même la compilation des procédures. Comment respecter la limite sans abandonner le plan d'IMP-0135 ?
- Choix : un processeur dont le code dépasse, ou approche, 64 Kio est réparti sur plusieurs segments statiques nommés, par des instructions `segment` successives dans son fichier source ; les appels entre ces segments sont des appels longs ordinaires. Les noms de segment comptent au plus 10 caractères, longueur des noms de segments de chargement (manuel ORCA/C, chap. 19 : `AUTOSEG~00`, nom du segment principal écrit avec dix espaces). Le compilateur occupe ainsi deux segments : `"compilateur"` (lexique, expressions, conditions) et `"compil_ins"` (instructions, point d'entrée). Tout nouveau segment est ajouté à la table ci-dessus et à celle d'Organisation du dépôt §2.1.
- Alternatives envisagées :
  1. Retirer le débogage au niveau source du profil `dev` — écartée : retarde seulement le problème et prive du débogage sous `iix -g`.
  2. Laisser l'éditeur de liens répartir le code (nom de segment spécial `AUTOSEG~~~`, segments `AUTOSEG~00`, `AUTOSEG~01`…) — écartée : abandonne le plan de segmentation nommé, et donc la lisibilité de la carte mémoire et la mesure par processeur (§7).
  3. Le modèle mémoire large — écartée : il ne lève pas la limite de 64 Kio par segment de code et produit un code plus gros et plus lent (manuel ORCA/C, chap. 13, `memorymodel`).
- Justification : c'est le moyen prévu par le manuel ORCA/C pour un programme dont le code dépasse 64 Kio ; le principe d'IMP-0135 (segments statiques nommés, tous résidents) est conservé.
- Impact mémoire : nul (même code, réparti autrement). Impact disque : sans objet. Impact performances : un appel long entre segments, négligeable.
- Impact historique : neutre.
- Références normatives : manuel ORCA/C 2.2, chap. 13 (`memorymodel`), chap. 19 (*Segment Statement*) ; IMP-0135, IMP-0143.
- Statut : retenu (choix d'Éric du 2026-10-23). Les noms de segment existants de plus de 10 caractères (`"compilateur"`, `"bibliotheque"`) se lient sans erreur ; leur traitement exact par l'éditeur de liens reste à vérifier.

---

## 6. Stratégie d'allocation mémoire

### 6.1 Deux familles d'allocation

Le chapitre RCI-M (§3, note de dépendance) suppose l'existence d'un gestionnaire d'objets « capable d'allouer, de libérer et, le cas échéant, de déplacer par compaction les blocs de la zone des séquences compilées », exposé par un type opaque `HandleObjet`, et renvoie explicitement au présent chapitre le soin de préciser sa réalisation physique (RCI-M §13, « chapitre distinct du Tome V, non encore rédigé »). Le présent paragraphe referme ce point.

Or `malloc`/`free` sur Apple IIGS, tels que documentés par le manuel ORCA/C (chap. 20), rendent un pointeur **fixe** pour toute la durée de vie du bloc — aucun mécanisme de verrouillage/déverrouillage n'est exposé sur ces blocs, ce qui les rend incompatibles avec une compaction. Le Memory Manager toolbox (`MEMORY.H`), en revanche, est directement accessible depuis ORCA/C et permet l'allocation de blocs déplaçables (`NewHandle`), verrouillables temporairement pour un accès physique (`HLock`/`HUnlock`), et compactables par le système lorsqu'ils sont déverrouillés.

**IMP-0132** — *Deux familles d'allocation : handles compactables pour les objets RCI-M/dynamiques, `malloc` fixe pour les structures de contrôle*
- Sujet : quel mécanisme d'allocation utiliser pour satisfaire la note de dépendance de RCI-M §3 (`HandleObjet` compactable) sans renoncer à la simplicité de `malloc` pour le reste ?
- Choix : deux allocateurs distincts.
  1. **Gestionnaire d'objets** (module `objets`, segment `"objets"`) : implémente `HandleObjet` directement au-dessus du Memory Manager toolbox (`NewHandle` / `DisposeHandle` / `HLock` / `HUnlock`, `MEMORY.H`). Il gère la zone des séquences compilées (RCI-M §4-5), les tableaux et chaînes d'exécution (espace des objets dynamiques, Tome III §4.4) — c'est-à-dire tout ce dont la durée de vie et la taille varient en cours de session et dont le Tome III (§4.4) exige une gestion explicite de création/libération. Chaque accès physique résout temporairement le handle en pointeur (`HLock` le temps de l'accès, `HUnlock` ensuite), conformément à la règle déjà posée dans RCI-M §3 : « résolution temporaire en adresse physique sans exposition au-delà de l'appel courant ». Le verrouillage porte sur le bloc et n'est pas compté ; la validité d'une adresse résolue repose sur l'absence d'appel système ou toolbox entre son obtention et son dernier usage (Gestionnaire d'objets §11).
  2. **Allocations fixes** (`malloc`/`realloc`/`free` standard) pour les structures dont la taille est bornée à l'avance ou dont la durée de vie couvre toute la session : table des objets compilés en tant que tableau d'entrées (RCI-M §5), table des symboles (RCI-M §6), table des constantes partagées (RCI-M §7), tampons d'entrée/sortie de la console (espace conversationnel temporaire, Tome III §4.6). Ces structures n'ont pas besoin d'être compactées : soit leur taille est stable, soit un redimensionnement se traite par `realloc` classique.
- Alternatives envisagées :
  1. Tout gérer par handles toolbox, y compris les petites structures de contrôle — rejeté : surcoût de verrouillage/déverrouillage disproportionné pour des structures qui n'ont jamais besoin d'être compactées et que le programme accède en continu.
  2. Tout gérer par `malloc`, y compris les blocs RCI-M — rejeté : contredirait directement la note de dépendance déjà actée dans RCI-M §3 et empêcherait toute compaction en cas de fragmentation, alors que le conditionnement (§4) signale explicitement que GS/OS gère un espace libre « morcelé et certainement pas continu ».
- Justification : le Tome III distingue déjà l'espace du programme courant / objets dynamiques (durée de vie variable, §4.4) de l'espace conversationnel et des structures de contrôle (§4.6) ; cette distinction logique se traduit naturellement par deux mécanismes physiques différents, sans qu'aucun des deux ne viole les invariants d'indépendance vis-à-vis des adresses physiques (Tome III §4.10, RCI-M §3).
- Impact mémoire : le gestionnaire d'objets peut restituer au système la mémoire fragmentée par compaction au fil d'une longue session (GARER/CHARGER répétés, création/destruction de tableaux) ; les structures fixes ont un coût prévisible et mesurable une fois pour toutes.
- Impact disque : sans objet.
- Impact performances : verrouillage/déverrouillage de handle à chaque accès à un objet dynamique ; coût mesuré sur l'Apple IIGS le 2026-10-25 : 1,1 ms par accès (`objets_resoudre` et `objets_relacher`, profil `cible`), à comparer au bénéfice d'éviter un échec d'allocation en session longue ; les accès réduits à une copie (chargement d'une ligne, élément de tableau, en-tête et texte d'une chaîne, ligne d'entrée d'une procédure) s'en dispensent, et le contrôle d'existence comme la création et la libération d'un objet se font sans verrou (Gestionnaire d'objets IMP-0243).
- Impact historique : neutre — décision de réalisation pure, aucun comportement L.S.E. observable n'en dépend.
- Références normatives : RCI-M §3 (note de dépendance, IMP-0101, IMP-0108), §13 (point laissé ouvert, désormais refermé par la présente décision) ; Tome III §4.4 (espace des objets dynamiques), §4.10 (contrôle des références) ; manuel ORCA/C chap. 20 (*malloc*), chap. 4 (Memory Manager, tool non graphique) ; conditionnement §4.4-4.5, §19.
- Statut : **retenu**. Referme le point laissé ouvert par RCI-M §13 (« Ne couvre pas la représentation physique du gestionnaire d'objets lui-même... chapitre distinct du Tome V, non encore rédigé »). La stratégie d'allocation interne au bloc de 4 Kio, l'algorithme de compaction et le format exact de l'en-tête de bloc restent à préciser dans un chapitre dédié au gestionnaire d'objets (cf. §11 « Limites explicites »).

### 6.2 Aucune réservation mémoire globale au démarrage

**IMP-0133** — *Aucune réservation mémoire globale au démarrage*
- Sujet : le programme doit-il réserver un grand bloc de mémoire (« toute la mémoire disponible », par exemple) dès le lancement ?
- Choix : non. Au démarrage, seules les structures de taille fixe et petite sont allouées (tables vides, tampons console). La table des objets compilés, la table des symboles et les objets dynamiques croissent à la demande, ligne par ligne / objet par objet, par `realloc`/`NewHandle` successifs de taille raisonnable — pas d'allocation spéculative de type « réserver pour 10 000 lignes par précaution ».
- Alternatives envisagées : réserver un grand tas applicatif unique dès le démarrage (modèle « arena ») — rejeté, contredit explicitement le conditionnement §4.1 (« Éviter... allocations massives inutiles... structures surdimensionnées ») et est incompatible avec un GS/OS dont l'espace libre est fragmenté et partagé avec d'autres besoins système (conditionnement §4.4) : une grosse réservation figée réduirait la marge de manœuvre du Memory Manager pour satisfaire d'autres allocations.
- Justification : cohérent avec le principe déjà retenu par RCI-M IMP-0101 (granularité par ligne, pas de zone contiguë globale) ; permet au Memory Manager de conserver la liberté de placement et de compaction que le gestionnaire d'objets (IMP-0132) requiert pour fonctionner correctement.
- Impact mémoire : consommation proportionnelle à l'usage réel, pas à un plafond arbitraire.
- Impact disque : sans objet.
- Impact performances : légère fragmentation potentielle par petites allocations successives, atténuée par la subdivision interne de blocs de 4 Kio que le runtime ORCA/C effectue déjà pour les petites structures fixes (`malloc`), et par la compaction du gestionnaire d'objets pour les gros blocs mobiles.
- Impact historique : neutre.
- Références normatives : conditionnement §4.1, §4.4 ; manuel ORCA/C chap. 20 (*malloc*, sous-allocateur de 4 Kio).
- Statut : **retenu**.

---

## 7. Budget mémoire — mesure sur cible, pas de valeur figée

**IMP-0134** — *Mémoire disponible : l'échec d'allocation fait foi, `RealFreeMem` à titre indicatif, mesure sur cible par la sonde*
- Sujet : comment le programme détermine-t-il la mémoire réellement disponible pour le gestionnaire d'objets (IMP-0132), sans coder en dur une hypothèse (par exemple « 3 Mio disponibles ») ?
- Choix :
  1. Aucun budget mémoire n'est câblé dans le code, et aucune interrogation n'a lieu au démarrage. Le seul critère qui fasse foi est l'échec d'une allocation : `NewHandle` ne rend l'erreur `memErr` ($0201) qu'après avoir épuisé tous ses recours, à savoir la file *out-of-memory*, le compactage, puis la purge des niveaux 3, 2 et 1 (Apple IIGS Toolbox Reference, vol. 3, p. 36-3). `objets_creer` rend alors `GESTOBJ_HANDLE_NUL`, que l'appelant traduit en diagnostic L.S.E. (`ERR_E_MEMOIRE_SATUREE`).
  2. L'opération « vérifier la capacité disponible » (Tome III §2.5.8, `objets_capacite_disponible`) rend `plateforme_memoire_disponible()`, réalisée par `RealFreeMem` ($2F02) : mémoire libre plus mémoire récupérable par purge (vol. 3, p. 36-10). La valeur est indicative ; à cause de la fragmentation, elle ne garantit pas qu'un bloc de cette taille puisse être alloué (vol. 1, p. 12-26).
  3. La mémoire réellement disponible sur l'Apple IIGS est mesurée par la sonde `SONDE` (`tests/plateforme/sonde_console.c`), qui affiche au lancement `userid()`, `TotalMem`, `FreeMem`, `RealFreeMem` et `MaxBlock` (vol. 1, p. 12-26, 12-34, 12-46). Les chiffres relevés sont consignés dans un audit de mesures. Le dimensionnement des tables (RCI-M §8, par exemple « ~7 octets/ligne ») sert de référence relative pour l'estimation des besoins.
- Alternatives envisagées :
  1. Figer un budget mémoire dans le code à partir d'une estimation documentaire — écartée : le conditionnement §21 exige que la conformité soit vérifiée « dans les contraintes matérielles de la cible », ce qui suppose une mesure réelle ; la part de RAM consommée par GS/OS et ses pilotes dépend de la configuration (version système, pilotes chargés, RAM disk éventuel).
  2. Interroger le Memory Manager au démarrage pour refuser ou limiter une session — écartée : la mémoire libre varie au cours de la session, et le Memory Manager purge et compacte de lui-même avant de refuser une allocation ; une valeur lue au démarrage n'apporte rien que l'échec d'allocation ne donne.
  3. `FreeMem` — écartée : ignore la mémoire occupée par des blocs purgeables déverrouillés, que le Memory Manager libère au besoin (vol. 3, p. 36-10).
  4. `MaxBlock` — écartée comme valeur de l'opération : ne compte ni la mémoire récupérable par compactage ni celle récupérable par purge (vol. 1, p. 12-34) ; elle est relevée par la sonde à titre de mesure.
- Justification : conditionnement §4 et §19 (« ne pas supposer une mémoire abondante ») ; Apple IIGS Toolbox Reference, vol. 1, chap. 12, vol. 3, chap. 36 ; choix d'Éric du 2026-10-29.
- Impact mémoire : dégradation contrôlée (diagnostic d'erreur L.S.E. explicite en cas de mémoire insuffisante) plutôt qu'un plantage sur une hypothèse fausse.
- Impact disque : sans objet.
- Impact performances : nul en session ; `RealFreeMem` n'est appelée que par `objets_capacite_disponible`.
- Impact historique : neutre.
- Références normatives : conditionnement §4, §16, §19, §21 ; RCI-M §8 (dimensionnement relatif) ; Tome III §2.5.8 ; Gestionnaire d'objets §11 ; Construction IMP-0192 (`FreeMem` et `RealFreeMem` absents de Golden Gate).
- Mesure : relevée le 2026-10-29 sur l'Apple IIGS d'Éric par `SONDE`, sous le shell ORCA (`05-IMPLEMENTATION/audits/Mesures_memoire_IIGS_2026-10-29.md`) : `TotalMem` 4 456 448 octets (4 Mio plus 256 Kio), `FreeMem` 3 265 881, `RealFreeMem` 3 589 887, `MaxBlock` 2 948 817. Déduction faite de l'exécutable `LSE` (profil `cible`), environ 3,2 Mio restent à la disposition des objets ; aucune limite de taille n'est introduite pour la v1.
- Statut : **retenu** (Éric, 2026-10-29). Golden Gate ne peut pas fournir cette mesure (Organisation du dépôt IMP-0138).

---

## 8. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0129 | Un seul exécutable ORCA/C, processeurs réalisés comme modules internes | retenu |
| IMP-0130 | Gestionnaire des tâches minimal (tâche unique, permanente, pas de swap) ; réalisé par le chapitre Gestionnaire des tâches et reprise (IMP-0269, `taches.c`) | retenu |
| IMP-0131 | Petit modèle mémoire ORCA/C, code réparti par `segment` | retenu |
| IMP-0132 | Deux familles d'allocation : handles compactables (Memory Manager) vs `malloc` fixe | retenu — referme le point ouvert de RCI-M §13 |
| IMP-0133 | Aucune réservation mémoire globale au démarrage | retenu |
| IMP-0134 | Aucun budget figé ; l'échec d'allocation fait foi ; `RealFreeMem` indicatif ; mesure sur cible par `SONDE` | retenu, mesuré sur l'Apple IIGS le 2026-10-29 |
| IMP-0135 | Segments statiques nommés par processeur, aucun segment dynamique en v1 | retenu, à réévaluer selon mesure |

---

## 9. Traçabilité

```text
HIS-3.1, HIS-3.3, HIS-3.11, HIS-4.1 à HIS-4.4.1, INRDP75 §3-3, INRDP75 §3-1
  → EXI-0001, EXI-0028, EXI-0052, EXI-0058, EXI-0059
    → VM-2201 à VM-2210 (Tome III §3), VM-2105 (Tome III §3.11, §19.7),
       espaces mémoire logiques (Tome III §4)
      → IMP-0129 à IMP-0135 (présent chapitre)
        → CODE : main.c, moniteur.c, analyseur.c, programme.c, compilateur.c,
                 interprete.c, contexte_exec.c, decompilateur.c, bibliotheque.c,
                 erreurs.c, donnees.c, objets.c, fichiers.c
```

Ce chapitre referme un point explicitement laissé ouvert par RCI-M :

```text
RCI-M §3 (note de dépendance), §13 (« Ne couvre pas la représentation physique
du gestionnaire d'objets lui-même... chapitre distinct du Tome V, non encore
rédigé »)
  → IMP-0132 (présent chapitre)
```

---

## 10. Limites explicites de ce chapitre

- Ne couvre pas le format physique de la représentation compilée : chapitre RCI-M, déjà finalisé.
- Ne couvre pas la représentation physique complète de l'état d'exécution (pile d'évaluation, cadres d'activation, piles de contextes de boucle) : chapitre distinct du Tome V, non encore rédigé.
- IMP-0132 fixe le principe (deux familles d'allocation, `HandleObjet` adossé au Memory Manager toolbox) mais ne détaille pas : la stratégie d'allocation interne au bloc de 4 Kio du gestionnaire d'objets, l'algorithme de compaction exact, ni le format précis de l'en-tête de bloc — ces points relèvent d'un chapitre dédié au gestionnaire d'objets, non encore rédigé.
- La mesure d'IMP-0134 est prise sous `SONDE`, non sous `LSE`, et inclut la mémoire spéciale que les objets n'emploient plus (IMP-0271) ; la mémoire effectivement disponible aux objets en est déduite, non mesurée directement.
- IMP-0135 retient un plan entièrement statique pour la v1 ; son remplacement partiel par des segments dynamiques (décompilateur, bibliothécaire) n'est pas exclu pour une version ultérieure, sous réserve d'une mesure montrant un dépassement de budget mémoire.
- Ne couvre pas le format physique des fichiers-programmes et fichiers-données (GARER/CHARGER/SUPPRIMER, catalogue) ni le protocole exact d'identification des fichiers (identité + portée, conditionnement §11) : chapitres distincts du Tome V.
- Ne couvre pas le détail des scripts et commandes de construction ORCA/C (Makefile ORCA, options du compilateur et de l'éditeur de liens) : livrable distinct du conditionnement (§20.3).

---

## 11. Tests associés

- Vérification que l'exécutable unique s'enchaîne correctement compilation → exécution en mode machine de bureau, sans latence perceptible (EXI-0028).
- Vérification que `BONJOUR` / `AU REVOIR` créent et détruisent correctement la tâche unique (IMP-0130), sans dépendre d'une zone de swap absente.
- Vérification de la taille de chaque segment statique (§5.2) au moment du lien, et absence de dépassement de la limite de 64 Kio par segment en petit modèle mémoire (IMP-0131).
- Test de compaction : allocation et libération répétées d'objets dynamiques de tailles variées (chaînes, tableaux) via le gestionnaire d'objets (IMP-0132), suivies d'une nouvelle allocation dans un espace fragmenté, pour vérifier que la compaction du Memory Manager permet effectivement de satisfaire la demande.
- Mesure sur l'Apple IIGS (conditionnement §16, « Tests sur cible ») de la mémoire disponible après chargement de GS/OS, par la sonde `SONDE` lancée depuis le shell ORCA (`TotalMem`, `FreeMem`, `RealFreeMem`, `MaxBlock`), pour clore IMP-0134 — réalisée le 2026-10-29 (`05-IMPLEMENTATION/audits/Mesures_memoire_IIGS_2026-10-29.md`).
- Test de non-régression : absence de toute allocation mémoire supérieure à la taille strictement nécessaire au démarrage (IMP-0133), vérifiable par observation de la mémoire consommée immédiatement après `BONJOUR`, avant toute ligne de programme saisie.