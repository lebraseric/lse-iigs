# Tome V — Organisation du dépôt des fichiers sources

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Environnement de développement :** Golden Gate sous macOS (compilation et exécution croisées avec ORCA/C 2.2.1)
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V, niveau `05-IMPLEMENTATION/decisions/`
**Portée :** organisation physique du dépôt de code source — arborescence, séparation noyau/plateforme/interfaces, articulation avec l'environnement de développement Golden Gate, organisation des tests. Ne couvre pas le format physique de la représentation compilée (RCI-M, chapitre distinct, déjà finalisé), ni l'organisation physique générale de l'exécutable et des segments ORCA/C (chapitre Architecture physique, déjà finalisé), ni le détail de la construction (Makefile, commandes `iix compile`/`iix link`, ordre de liaison des segments, type de fichier de sortie) : chapitre distinct du Tome V, consacré à la construction.
**Rang normatif :** subordonné au Catalogue des exigences, au Tome III, au Tome IV, et aux chapitres RCI-M et Architecture physique du Tome V (notamment IMP-0129, IMP-0131, IMP-0132, IMP-0134, IMP-0135). Aucune disposition du présent chapitre ne peut contredire ces sources.
**Place dans le Tome V :** ce chapitre referme le point laissé ouvert par le chapitre Architecture physique (§3.2, « Conséquence sur le plan de fichiers source »). La numérotation `IMP-*` du présent chapitre poursuit celle d'Architecture physique (dernier identifiant utilisé : IMP-0135) à partir d'**IMP-0136**.

---

## 0. Avertissement de statut

Comme pour RCI-M et Architecture physique, aucune disposition du présent chapitre ne constitue une reconstruction historique. Tout ce qui est décrit ici — arborescence du dépôt, séparation noyau/plateforme, environnement de développement — relève exclusivement de décisions d'implémentation (`IMP-*`), prises au niveau 6 de la hiérarchie de résolution des ambiguïtés (conditionnement §18), les tomes I à IV ne fixant aucune organisation physique de dépôt de code.

---

## 1. Rappel du point ouvert

Le chapitre Architecture physique (§3.2) établit que chaque processeur du Tome III correspond à une ou plusieurs unités de compilation C, regroupées ou réparties selon les besoins de réalisation, et renvoie au présent chapitre le soin de préciser le plan de fichiers source correspondant. Le conditionnement §6 ajoute une exigence non encore traitée à ce stade : le cœur de la machine virtuelle doit rester aussi indépendant que possible du système de fichiers Apple IIGS, du terminal, des conventions du shell, des appels système et des particularités du compilateur, ces dépendances devant être isolées derrière une couche d'adaptation.

---

## 2. Séparation noyau / plateforme / interfaces

**IMP-0136** — *Séparation physique `noyau/` / `plateforme/` / `interfaces/`*

- **Sujet :** comment traduire physiquement l'isolation exigée par le conditionnement §6 dans l'arborescence du dépôt ?
- **Choix :** trois répertoires de premier niveau sous `src/` :
  1. **`src/noyau/`** — le cœur de la machine virtuelle : dialogue (logique, hors lecture/écriture physique), analyseur, programme courant, compilateur, interprète, contexte d'exécution, décompilateur, bibliothécaire (logique de conservation/restauration, hors accès disque), gestionnaire des erreurs, gestion des données (variables/tableaux/chaînes/nombres), gestionnaire d'objets (logique RCI-M et politique de compaction), fichiers-données et tâches (règles d'identité/portée). Aucun fichier de `noyau/` n'inclut directement `MEMORY.H`, `GSOS.H`, ni aucun en-tête ORCA spécifique GS/OS : tout accès système passe par une fonction déclarée dans `interfaces/`.
  2. **`src/plateforme/`** — réalisation Apple IIGS / ORCA/C des interfaces : console (lecture clavier, écriture console), accès GS/OS aux fichiers-programmes et fichiers-données, Memory Manager toolbox (réalisation de `HandleObjet`, cf. IMP-0132), point d'entrée `main`. C'est le seul répertoire autorisé à inclure `MEMORY.H` et les en-têtes GS/OS.
  3. **`src/interfaces/`** — contrats partagés uniquement : prototypes des fonctions d'adaptation (`interface_console.h`, `interface_fichiers.h`, `interface_memoire.h`, `types_communs.h`). Aucune implémentation, aucun appel système.
- **Alternatives envisagées :**
  1. Un seul répertoire `src/` plat, discipline d'isolation maintenue par convention (commentaire d'en-tête, revue de code) plutôt que par la structure — écarté : ne rend pas l'isolation vérifiable structurellement, contrairement au principe déjà retenu pour la séparation des espaces mémoire (Architecture physique §1.3, « rend certains invariants vérifiables structurellement plutôt que par convention de programmation »).
  2. Isolation par compilation conditionnelle (`#ifdef` de plateforme) à l'intérieur des mêmes fichiers plutôt que par séparation de fichiers — écarté : la cible est unique (Apple IIGS/ORCA-C), une compilation conditionnelle n'a pas de second cas d'usage réel ici et rendrait ambigu le rattachement d'un fichier à un segment ORCA/C donné (IMP-0135 nomme les segments par fichier logique).
- **Justification :** conditionnement §6 (isolation explicite) ; §7 (tolérance de répartition d'une responsabilité sur plusieurs fichiers C, appliquée ici entre `noyau/` et `plateforme/` pour une même responsabilité logique — par exemple le bibliothécaire, dont la logique vit dans `noyau/bibliotheque.c` et l'accès disque dans `plateforme/fichiers_iigs.c`).
- **Impact mémoire :** nul — la séparation est purement source ; les fichiers de `noyau/` et `plateforme/` d'un même processeur logique restent affectés au même segment ORCA/C nommé par IMP-0135.
- **Impact disque :** nul sur l'exécutable final ; légère augmentation du nombre de fichiers sources.
- **Impact performances :** nul — l'appel entre `noyau/` et `plateforme/` se fait par appel de fonction C direct (prototype partagé dans `interfaces/`, lié statiquement), sans indirection par pointeur de fonction.
- **Impact historique :** neutre — décision de réalisation pure, aucune conséquence sur le comportement L.S.E. observable.
- **Références normatives :** conditionnement §6, §7 ; Architecture physique §1.3, §3.2, IMP-0132, IMP-0135.
- **Statut : retenu.**

### 2.1 Correspondance fichier `noyau/` ↔ segment ORCA/C ↔ `plateforme/`

Cette table applique IMP-0135 (colonne segment, déjà normative) sans la modifier ; elle ajoute la localisation source et l'interface d'adaptation sollicitée.

| Processeur (Tome III) | `src/noyau/` | Segment (IMP-0135) | `src/plateforme/` sollicité | Interface (`src/interfaces/`) |
|---|---|---|---|---|
| Environnement conversationnel | `moniteur.c/.h` | `"moniteur"` (Moniteur IMP-0227, 2026-10-25 ; IMP-0197) | `console_iigs.c/.h` | `interface_console.h` |
| Analyseur de commandes | `analyseur.c/.h` | blanc | — | — |
| Gestionnaire du programme courant | `programme.c/.h` | `"programme"` | — | — |
| Compilateur | `compilateur.c/.h` | `"compilateur"`, `"compil_ins"` (IMP-0197) | — | — |
| Interpréteur / moteur d'exécution | `interprete.c/.h`, `pile.c/.h` (pile d'évaluation, choix d'Éric du 2026-10-23, E3) | `"interprete"` et `"interp_es"` (IMP-0226) | — | — |
| Environnement d'exécution | `contexte_exec.c/.h` | `"contexte"` (IMP-0197) | — | — |
| Décompilateur | `decompilateur.c/.h` | `"decompil"` (IMP-0202) | — | — |
| Bibliothécaire | `bibliotheque.c/.h` | `"bibliothe"` (Bibliothécaire et fichiers IMP-0249 ; règle des 10 caractères, IMP-0197) | `fichiers_iigs.c/.h` | `interface_fichiers.h` |
| Gestionnaire des erreurs | `erreurs.c/.h` | blanc | — | — |
| Variables / tableaux / chaînes / nombres | `donnees.c/.h` | `"donnees"` | — | — |
| Gestionnaire mémoire (objets) | `objets.c/.h` | `"objets"` | `memoire_iigs.c/.h` | `interface_memoire.h` |
| Fichiers-données | `fichiers.c/.h` | `"fichiers"` | `fichiers_iigs.c/.h` | `interface_fichiers.h` |
| Gestionnaire des tâches (VM-2105) | `taches.c/.h` (Gestionnaire des tâches et reprise IMP-0269 : la conservation d'une tâche est distincte de celle d'un fichier) | `"fichiers"` | — | — |
| Représentation compilée (RCI-M) : éléments, table des symboles, table des constantes partagées | `rcim.c/.h`, `symboles.c/.h` | `"rcim"` (IMP-0193) | — | — |
| Point d'entrée | — | blanc | `main.c` | — |

`donnees.c/.h` peut être physiquement réparti en `donnees_variables.c`, `donnees_tableaux.c`, `donnees_chaines.c`, `donnees_nombres.c` (tous porteurs de la déclaration `segment "donnees";`, chapitre 19 du manuel ORCA/C — un statement du langage, non un `#pragma`) si la taille le justifie — latitude ouverte par le conditionnement §7, à trancher au moment de l'écriture de ce module.


**IMP-0193** — *Modules `rcim.c` et `symboles.c` dans un segment statique dédié, `"rcim"`*

- **Sujet :** RCI-M (§12, traçabilité) et le chapitre Compilateur (§5, §17) attribuent le code de la représentation compilée à deux modules : `rcim.c/.h`, pour l'écriture et la lecture des éléments, et `symboles.c/.h`, pour la table des symboles et la table des constantes partagées. Ni la table ci-dessus, ni IMP-0135, ni le Makefile de Construction (§7) ne les placent. Dans quel segment les ranger ?
- **Choix :** un segment statique dédié, `"rcim"`, qui contient `rcim.c` et `symboles.c`. Les deux fichiers portent la déclaration `segment "rcim";` (manuel ORCA/C, chap. 19). Aucun module de `src/plateforme/` ne leur est associé. La ligne correspondante est ajoutée à la table ci-dessus, à la table d'IMP-0135 (Architecture physique §5.2) et au Makefile (Construction §7).
- **Alternatives envisagées :**
  1. Le segment principal, sans nom, toujours résident — écartée : il contient déjà `main`, le moniteur, l'analyseur, les erreurs et la console, dans la limite de 64 Kio de code par segment.
  2. Le segment `"programme"` — écartée : la table des symboles appartient au programme courant, mais le format des éléments n'a rien à y faire, et les deux modules sont employés aussi par le compilateur, le décompilateur et l'interpréteur.
- **Justification :** IMP-0135 retient un segment statique par grand composant logique ; RCI-M en est un, spécifié par son propre chapitre. Les appels entre segments statiques sont des appels longs ordinaires, d'un coût négligeable.
- **Impact mémoire :** un segment de plus, résident ; sa taille sera mesurée avec les autres (IMP-0134).
- **Impact disque :** sans objet (un seul exécutable).
- **Impact performances :** négligeable.
- **Impact historique :** neutre.
- **Références normatives :** RCI-M §12 ; chapitre Compilateur §5, §17 ; Architecture physique §5.2 (IMP-0135) ; Construction §7 (IMP-0140) ; manuel ORCA/C chap. 19.
- **Statut :** retenu (choix d'Éric du 2026-10-22).

---

## 3. Environnement de développement

**IMP-0137** — *Golden Gate (macOS) comme environnement de compilation et d'exécution croisée*

- **Sujet :** quel environnement utiliser pour compiler et exécuter le code pendant le développement, en amont de tout accès à une cible physique ?
- **Choix :** Golden Gate, sous macOS, comme environnement de travail principal. Golden Gate n'est pas un émulateur Apple IIGS généraliste : c'est une couche de compatibilité qui exécute nativement les compilateurs ORCA (dont ORCA/C 2.2.1) et les programmes shell texte qu'ils produisent, en implémentant nativement les appels GS/OS et Toolbox sollicités par ce type de programme — sans émulation graphique, vidéo ou son, ce qui correspond exactement au périmètre de l'application visée (conditionnement §5, application exclusivement textuelle). Le compilateur invoqué via `iix compile` / `iix link` est ORCA/C 2.2.1 lui-même, pas une réimplémentation ; l'exécutable produit (type GS/OS Shell application, `$B5`) est le même artefact binaire, qu'il soit ensuite lancé sur macOS via `iix mon_executable` ou copié et lancé depuis le shell ORCA sur un Apple IIGS réel ou un émulateur système complet. L'absence de contrainte de nommage ProDOS sous Golden Gate permet en outre de retenir directement les noms de fichiers du plan §2 (noms descriptifs complets, répertoires imbriqués), sans couche de correspondance vers des noms courts.
- **Alternatives envisagées :**
  1. Développer uniquement sur émulateur Apple IIGS système complet (type GSplus/KEGS) dès le premier cycle — écarté pour le développement courant : cycle de compilation/exécution nettement plus lent (démarrage d'un système complet, transfert de fichiers via image disque), sans bénéfice pour la majorité des tests, qui ne sollicitent aucun comportement Toolbox graphique ni matériel réel.
  2. Développer sur machine hôte avec des implémentations de substitution des interfaces système (répertoire dédié aux stand-ins) — écarté : Golden Gate exécute la véritable implémentation `plateforme/` (compilée par le vrai ORCA/C), rendant toute implémentation de substitution redondante et porteuse d'un risque de divergence entre code testé et code livré.
- **Justification :** conditionnement §3 (priorité 6, simplicité) et §27 (« possibilité de débogage sur cible » — Golden Gate fournit un débogueur GSBug compatible, avec trace CPU et vérification mémoire, utilisable dès le développement).
- **Impact mémoire :** sans objet pour le code livré — outil de développement uniquement.
- **Impact disque :** sans objet.
- **Impact performances :** cycle de développement nettement accéléré (pas de transfert d'image disque, pas de démarrage système) ; sans effet sur les performances de l'exécutable final, produit par le même compilateur.
- **Impact historique :** neutre — outil de développement, aucune incidence sur le comportement L.S.E. observable.
- **Limite explicite :** l'émulateur 65816 interne de Golden Gate dispose de 16 Mio de RAM et n'a pas vocation à reproduire la configuration mémoire réelle de la cible (4 Mio, conditionnement §4). Toute mesure de mémoire disponible effectuée sous Golden Gate ne peut pas servir à clore IMP-0134 : cette mesure exige un Apple IIGS réel ou un émulateur système complet explicitement configuré à 4 Mio (§4 ci-après).
- **Références normatives :** conditionnement §3, §5, §16, §27 ; IMP-0134.
- **Statut : retenu.**

---

## 4. Organisation des tests

**IMP-0138** — *Deux niveaux d'exécution pour les tests*

- **Sujet :** comment répartir les six niveaux de tests du conditionnement §16 entre Golden Gate et une cible physique ou un émulateur système complet ?
- **Choix :** deux niveaux d'exécution :
  1. **Golden Gate (`iix`)** — exécute le compilateur ORCA/C réel et les exécutables réels produits. Sert à tous les niveaux fonctionnels : représentation, composants, interfaces, conformité (EXI), historiques. C'est le niveau d'exécution par défaut pendant le développement, y compris pour les tests de conformité — Golden Gate implémentant nativement GS/OS et le Toolbox non graphique sollicités par une application texte, le comportement observé y est celui du programme réel.
  2. **Cible physique ou émulateur système complet** (Apple IIGS réel, ou émulateur explicitement configuré à 4 Mio de RAM) — réservé à ce que Golden Gate ne peut pas valider par construction : la mesure de mémoire disponible d'IMP-0134, et une validation d'acceptation finale avant chaque version livrée, dans la configuration matérielle réellement visée par le conditionnement (§4).
- **Alternatives envisagées :** traiter Golden Gate comme équivalent en tout point à la cible et supprimer tout test sur matériel réel ou émulateur système complet — écarté : romprait le conditionnement §21 (« fonctionner dans les contraintes matérielles de la cible ») sur le seul point où Golden Gate diverge réellement de la cible (le volume de RAM disponible, cf. IMP-0137).
- **Justification :** conditionnement §16 (six niveaux de tests, dont « Tests sur cible ») ; §21 ; IMP-0134 (mesure mémoire non figée dans le code, à clore sur cible réelle).
- **Impact mémoire / disque / performances :** sans objet — organisation des tests uniquement.
- **Impact historique :** neutre.
- **Références normatives :** conditionnement §16, §21 ; IMP-0134, IMP-0137.
- **Statut : retenu.**

---

## 5. Traçabilité des décisions dans le dépôt

**IMP-0139** — *Le dépôt de code renvoie aux décisions IMP par identifiant*

- **Sujet :** le dépôt de code doit-il recopier le texte des décisions `IMP-*` (par exemple en commentaire de tête de fichier), ou seulement y renvoyer ?
- **Choix :** chaque fichier de `src/` porte en tête un commentaire de traçabilité minimal (identifiants EXI/VM/IV/IMP concernés, conditionnement §15), sans jamais reprendre le texte intégral d'une décision IMP — celui-ci reste unique dans le chapitre Tome V correspondant (`05-IMPLEMENTATION/decisions/`, conditionnement §23). `doc/tracabilite/` contient la matrice consolidée, tenue à jour au fil du développement, et renvoie par identifiant plutôt que par copie.
- **Alternatives envisagées :** dupliquer le texte des décisions dans les fichiers sources pour une lisibilité immédiate — écarté : crée deux sources potentiellement divergentes pour un même énoncé normatif, ce que le conditionnement proscrit implicitement en imposant une hiérarchie normative unique et impérative (§2).
- **Justification :** cohérence avec le principe déjà appliqué entre les chapitres du Tome V (un seul lieu normatif par décision, référencé ailleurs par identifiant).
- **Impact mémoire / disque / performances :** sans objet.
- **Impact historique :** neutre.
- **Références normatives :** conditionnement §2, §14, §15, §23.
- **Statut : retenu.**

---

## 6. Arborescence retenue

```text
lse-iigs/
├── src/
│   ├── noyau/
│   │   ├── moniteur.c / moniteur.h
│   │   ├── analyseur.c / analyseur.h
│   │   ├── programme.c / programme.h
│   │   ├── compilateur.c / compilateur.h
│   │   ├── interprete.c / interprete.h
│   │   ├── contexte_exec.c / contexte_exec.h
│   │   ├── decompilateur.c / decompilateur.h
│   │   ├── bibliotheque.c / bibliotheque.h
│   │   ├── erreurs.c / erreurs.h
│   │   ├── donnees.c / donnees.h
│   │   ├── objets.c / objets.h
│   │   ├── rcim.c / rcim.h                    (IMP-0193)
│   │   ├── symboles.c / symboles.h            (IMP-0193)
│   │   └── fichiers.c / fichiers.h
│   ├── plateforme/
│   │   ├── main.c
│   │   ├── console_iigs.c / console_iigs.h
│   │   ├── fichiers_iigs.c / fichiers_iigs.h
│   │   └── memoire_iigs.c / memoire_iigs.h
│   └── interfaces/
│       ├── interface_console.h
│       ├── interface_fichiers.h
│       ├── interface_memoire.h
│       └── types_communs.h
├── build/                                  (Makefile, commandes iix compile / iix link — contenu détaillé : chapitre distinct du Tome V, consacré à la construction)
├── tests/
│   ├── commun/                             (verif.h, macros de vérification, IMP-0192)
│   ├── bouchons/                           (réalisations minimales d'interfaces non encore écrites, IMP-0192)
│   ├── representation/                     (sous Golden Gate)
│   ├── composants/                         (sous Golden Gate)
│   ├── interfaces/                         (sous Golden Gate)
│   ├── conformite/                         (sous Golden Gate — EXI-0001 à EXI-0062)
│   ├── historiques/                        (sous Golden Gate — issus d'INRDP75 / LSE15-78)
│   │   └── corpus/                         (lignes de programme d'INRDP75 et de LSE15, produites par scripts/extraire_corpus.py, IMP-0203)
│   └── cible/                              (Apple IIGS réel ou émulateur système à 4 Mio uniquement — mesure IMP-0134, validation d'acceptation)
├── scripts/                                (outils de l'hôte : generer_erreurs.py, IMP-0195 ; extraire_corpus.py, IMP-0203)
├── doc/
│   ├── tracabilite/                        (matrice EXI → VM → IMP → CODE → test)
│   └── resultats-tests/
└── README.md
```

Les extensions `.c`/`.h` retenues correspondent directement aux types de fichiers ORCA/C reconnus par Golden Gate par extension, sans manipulation de type de fichier nécessaire dans le flux normal de compilation.

---

## 7. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0136 | Séparation physique `noyau/` / `plateforme/` / `interfaces/` | retenu |
| IMP-0137 | Golden Gate comme environnement de compilation et d'exécution croisée | retenu |
| IMP-0138 | Deux niveaux d'exécution pour les tests (Golden Gate / cible physique ou émulateur à 4 Mio) | retenu |
| IMP-0139 | Le dépôt de code renvoie aux décisions IMP par identifiant | retenu |
| IMP-0193 | Modules `rcim.c` et `symboles.c` dans un segment statique dédié `"rcim"` | retenu |

---

## 8. Traçabilité

```text
conditionnement §6, §7, §16, §20, §23
  → Architecture physique §3.2 (point ouvert), IMP-0129, IMP-0131, IMP-0132, IMP-0134, IMP-0135
    → IMP-0136 à IMP-0139 (présent chapitre)
      → CODE : src/noyau/*, src/plateforme/*, src/interfaces/* (correspondance §2.1)
```

---

## 9. Limites explicites

- Ne fixe pas l'éclatement éventuel de `donnees.c` en quatre fichiers (variables/tableaux/chaînes/nombres) — laissé ouvert jusqu'à l'écriture de ce module.
- Ne couvre pas le détail de la construction (Makefile, commandes `iix compile`/`iix link`, ordre de liaison des segments, type de fichier de sortie) : chapitre distinct du Tome V, consacré à la construction.
- Golden Gate ne peut pas servir à clore IMP-0134 (mesure mémoire) : sa RAM émulée (16 Mio) ne reflète pas la contrainte cible de 4 Mio.
- Golden Gate ne couvre que les applications texte sans Toolbox graphique — cohérent avec le périmètre du conditionnement §5, signalé ici comme limite structurelle si ce périmètre venait à évoluer.

---

## 10. Tests associés

- Vérification que le même exécutable, produit par `iix compile`/`iix link`, s'exécute correctement à la fois via `iix` sous Golden Gate et depuis le shell ORCA sur cible physique ou émulateur système complet.
- Vérification que la suite de tests de représentation, composants, interfaces, conformité et historiques s'exécute intégralement sous Golden Gate.
- Vérification qu'aucune mesure de budget mémoire (IMP-0134) n'est acceptée si elle provient d'une exécution sous Golden Gate plutôt que d'une cible réelle ou d'un émulateur système configuré à 4 Mio.
- Vérification qu'aucun fichier de `src/noyau/` n'inclut directement un en-tête GS/OS ou Toolbox (contrôle structurel de l'isolation posée par IMP-0136).