# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — État d'exécution : pile d'évaluation, cadres d'activation, piles de contextes de boucle

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V (spécification détaillée complète de l'implémentation), niveau `05-IMPLEMENTATION/decisions/`
**Portée :** représentation physique de l'état d'exécution au sens d'IV-FMT-0010 — pile d'évaluation, cadres d'activation (paramètres, identificateurs `LOCAL`, résolution des références, appel et retour), piles de contextes de boucle `FAIRE`. Ne couvre pas RCI-M (représentation compilée, chapitre distinct, déjà finalisé), ni la compilation de la déclaration de procédure — élément de déclaration, structure et construction de la table des paramètres formels consultée à l'appel (chapitre Compilateur, §13, IMP-0147, IMP-0174 à IMP-0176), dont le présent chapitre ne fixe que l'usage à l'exécution, ni l'organisation physique générale de l'exécutable et des segments ORCA/C (chapitre Architecture physique, déjà finalisé), ni le protocole détaillé de conservation/restauration d'une tâche (état de tâche, IV-FMT-0011, SWAP OUT/SWAP IN), pour lequel le présent chapitre se borne à fournir les structures que ce protocole devra sérialiser le jour où il sera écrit.
**Rang normatif :** subordonné à ANN-K (Tome IV), aux chapitres 6 (§6.6, §6.7, VM-2401), 7 et 8 du Tome III, aux chapitres 8, 9, 10 et 13 des Tomes I-II (VM-0801 à VM-0903, VM-1001, VM-1002, VM-1301 à VM-1303), et au Catalogue des exigences. Aucune disposition du présent chapitre ne peut contredire ces sources, ni le chapitre RCI-M (notamment IMP-0108, IMP-0110, IMP-0112, IMP-0114, IMP-0120 à IMP-0122, IMP-0125) et le chapitre Architecture physique (notamment IMP-0130, IMP-0132), tous deux déjà finalisés.
**Place dans le Tome V :** ce chapitre referme le point explicitement laissé ouvert par RCI-M (§8, §13 : « Ne couvre pas la représentation physique complète de l'état d'exécution... chapitre distinct du Tome V, non encore rédigé ») et par Architecture physique (§10, même limite). La numérotation `IMP-*` du présent chapitre poursuit celle du chapitre Construction (dernier identifiant utilisé : IMP-0143) à partir d'**IMP-0144**.

---

## 0. Avertissement de statut

Comme pour les chapitres précédents du Tome V, aucune disposition du présent chapitre ne constitue une reconstruction historique. Le Tome III (§4.9, « État d'exécution ») fixe une durée de vie logique et le Tome IV (§8.6, ANN-K-0044) précise explicitement que « le format physique de l'état d'exécution n'est pas historiquement documenté » et qu'« il est donc laissé à l'implémentation ». Tout ce qui est décrit ici — structures C, algorithmes de résolution, gestion mémoire de la pile et des cadres — relève exclusivement de décisions d'implémentation (`IMP-*`), prises au niveau 6 de la hiérarchie de résolution des ambiguïtés (conditionnement §18), les Tomes I à IV ne fixant, pour ce périmètre, que des propriétés abstraites (contenu minimal, invariants, responsabilités) et non une organisation physique.

---

## 1. Rappel des contrats obligatoires (non renégociables)

### 1.1 Exigences EXI directement concernées

| EXI | Objet | Conséquence physique imposée |
|---|---|---|
| EXI-0022 | Indépendance d'exécution vis-à-vis du texte source | Aucune structure de ce chapitre ne référence ni ne suppose disponible le texte source d'une ligne |
| EXI-0023 | Début de l'exécution au premier objet logique | L'initialisation (§7.1) doit pouvoir déterminer le premier objet compilé en l'absence de point de départ explicite |
| EXI-0024 | Conservation d'une position d'exécution interrompue | L'état d'exécution porte une position de reprise suffisante (§3, §7.4) |
| EXI-0029 | Exécution explicite du programme courant | §7.1 : `INITIALISER` réalise `EXECUTER`/`EXECUTER A PARTIR DE`, avec point de départ et point d'arrêt programmables (IMP-0153) |
| EXI-0030 | Terminaison explicite ou naturelle de l'exécution | §7.2 : `TERMINER`, `PAUSE` et la ligne d'arrêt programmée sont les seules terminaisons non erronées (IMP-0154) |
| EXI-0031 | État d'exécution distinct du programme compilé | Aucune structure de ce chapitre ne partage d'allocation avec la zone RCI-M (IMP-0108, rappelé §3) |
| EXI-0032 | Résolution des références vers les objets d'exécution | §5.3 (algorithme de résolution) |
| EXI-0033 | Observation de l'état d'exécution | Les structures définies ici doivent rester lisibles depuis le dialogue (mise au point, PAS À PAS) sans reconstruction du texte source |
| EXI-0034 | Cadre d'activation des appels | §5.1 |
| EXI-0035 | Chaînage des activations | §5.1 (champ `appelant`) |
| EXI-0036 | Retour au point d'appel | §5.5 |
| EXI-0037 | Évaluation par valeurs intermédiaires | §4 |
| EXI-0038 | Identité stable des variables | §5.3 (un identifiant de symbole désigne toujours le même objet logique le temps de sa portée) |
| EXI-0039 | Valeur mutable des variables | §5.4 (sauvegarde/restauration ne change jamais le mécanisme de mutation lui-même, cf. gestionnaire des variables, hors périmètre) |
| EXI-0042 | Représentation compilée des procédures et fonctions | Réutilise `InfoProcedure` (RCI-M §6.3, IMP-0114) et la table des paramètres formels (Compilateur §13.3, IMP-0147) sans modification |
| EXI-0043 | Création d'une activation lors d'un appel | §5.5 |
| EXI-0044 | Restitution du contrôle après retour | §5.5 |
| EXI-0058, EXI-0059 | Gestion abstraite des tâches, moniteur de temps partagé | Aucune conséquence physique directe dans ce chapitre (IMP-0130 : tâche unique) ; les structures d'§4 à §6 sont toutefois précisément celles qu'un futur SWAP OUT/SWAP IN devrait sérialiser (§8, §12) |
| EXI-0060 | PAS À PAS et reprise | §7.4 |
| EXI-0001 | Compilation indépendante des lignes | Aucune structure de ce chapitre ne modifie RCI-M ; en particulier, la corrélation de boucle `FAIRE` (§6) reste exclusivement portée par l'état d'exécution, jamais par la ligne `n1` (rappel RCI-M §4.8.2, IMP-0112) |

### 1.2 Invariants applicables (Tome III, ANN-K)

1. Une opération de contrôle ne modifie jamais la représentation compilée du programme courant, seulement l'état d'exécution (Tome III §6.7 ; RCI-M §10 ligne 1).
2. La pile d'évaluation n'est pas une seconde représentation compilée : elle est un état d'exécution temporaire (Tome III §6.6).
3. Le cadre courant est toujours le dernier cadre actif de la chaîne d'activations ; aucun cadre actif ne peut être son propre appelant (Tome III §7.7, invariants).
4. Toute position de retour appartient à un contexte appelant valide (Tome III §7.7).
5. Une référence résolue désigne un objet accessible ; un objet libéré ne peut rester une cible valide (Tome III §7.5).
6. La fin d'une exécution ne détruit pas le programme courant (Tome III §4.8, §6.9 a contrario).

### 1.3 Responsabilités VM couvertes par ce chapitre

| Identifiant | Objet | Source |
|---|---|---|
| VM-0801 | État d'exécution | Tomes I-II §8.1 |
| VM-0802 | Environnement d'accès aux objets (résolution des références) | Tomes I-II §8.2 |
| VM-0803 | Accès à l'état d'exécution (observation, mise au point) | Tomes I-II §8.3 |
| VM-0901 | Cadre d'activation | Tomes I-II §9.1 |
| VM-0902 | Chaînage des cadres d'activation | Tomes I-II §9.2 |
| VM-0903 | Position de retour | Tomes I-II §9.3 |
| VM-1001 | Pile d'évaluation | Tomes I-II §10.1 |
| VM-1002 | Opérations de pile | Tomes I-II §10.2 |
| VM-1103 | Résolution différée des variables | Tomes I-II §11.3 |
| VM-1302 | Activation d'une procédure (appel) | Tomes I-II §13.2 |
| VM-1303 | Retour d'activation | Tomes I-II §13.3 |
| VM-3006 | Registre d'état d'exécution | Tome III §7.6 |
| VM-3007 | Registres de contexte d'activation courant | Tome III §7.7 |
| VM-3101, VM-3102 | Moteur d'exécution, sélecteur d'opération | Tome III §8.1, §8.2 |

Ce chapitre réalise l'état de déclaration et la valeur courante des identificateurs (VM-1102) par l'environnement des identificateurs (§3 bis, IMP-0179), avec leurs règles de transition. Il ne couvre pas la représentation interne d'un objet tableau ou chaîne, son allocation ni la vérification des bornes d'un accès indexé (VM-1201/VM-1202) : ces responsabilités relèvent du gestionnaire des variables, des tableaux et des chaînes (module `donnees`, chapitre distinct du Tome V, non encore rédigé), qui opère sur les objets désignés par l'environnement. Le présent chapitre fournit en outre le mécanisme de *résolution* (quel objet un identificateur désigne dans le contexte courant) et de *sauvegarde/restauration* (quand un identificateur redevient celui du contexte appelant).

### 1.4 Interdictions du conditionnement (§9, §19), reprises de RCI-M §1.4

Les pointeurs C ne constituent pas un modèle architectural. Toute référence portée par une structure de ce chapitre et susceptible de survivre à un déplacement mémoire (identifiant de symbole, position de retour) passe par un identifiant logique déjà défini par RCI-M (index 16 bits dans la table des symboles, IMP-0104 ; numéro de ligne 1 octet, IMP-0125) — jamais par un pointeur C brut vers une structure RCI-M. Un pointeur C local et volatile reste acceptable pour le seul parcours interne d'une structure de ce chapitre pendant la durée d'un appel de fonction (même principe que RCI-M §3, deuxième alinéa).

---

## 2. Contraintes de plateforme retenues (ORCA/C 2.2.1 / 65816)

Ce chapitre reprend intégralement RCI-M §2 (tailles des types C, absence de bourrage de structure, segmentation par blocs de 64 Kio) et Architecture physique §2 et §6 (deux familles d'allocation : Memory Manager toolbox pour les blocs compactables, `malloc`/`realloc` fixes pour les structures bornées ou à croissance simple). Il ajoute un seul élément spécifique :

- La pile d'évaluation et la chaîne des cadres d'activation croissent et décroissent selon une discipline strictement LIFO (dernier empilé, premier dépilé), tant au niveau de l'expression compilée en cours d'évaluation qu'au niveau des appels de procédures. Cette propriété, directement héritée de la postfixation (EXI-0011, Tome III §6.5) et du chaînage des cadres (VM-0902), écarte d'emblée tout besoin de compaction pour ces deux structures : aucune référence externe (RCI-M, table des symboles) ne pointe jamais vers un élément interne à la pile d'évaluation ou vers un cadre autre que le cadre courant. Ce constat conditionne directement IMP-0145 et IMP-0150 ci-dessous.

---

## 3. Architecture physique générale de l'état d'exécution

```text
ÉTAT D'EXÉCUTION (mémoire, zone distincte de RCI-M — IMP-0108, RCI-M §8)
│
├── REGISTRE D'ÉTAT (VM-3006)             prête / active / suspendue / terminée / en erreur
│
├── LIGNE D'ARRÊT PROGRAMMÉE (§7.1)       optionnelle — EXECUTER A PARTIR DE n1,n2 (IMP-0153)
│
├── POSITION D'EXÉCUTION COURANTE          { numéro de ligne (table des objets compilés, RCI-M §5),
│                                             curseur intra-séquence (CurseurSequence, RCI-M §9) }
│
├── ENVIRONNEMENT DES IDENTIFICATEURS (§3 bis)   pour chaque nom, sa case courante ; chaque case
│                                                porte un état et une valeur (IMP-0179, D1)
│
├── PILE D'ÉVALUATION (§4)                 tableau dynamique d'ÉLÉMENTS PILE, réalloué, jamais compacté
│
└── CADRE D'ACTIVATION COURANT ────────┐   (chaînage VM-0902 vers l'appelant, jusqu'au cadre racine)
                                        │
                                        ├── sauvegardes LOCAL (§5.4)
                                        ├── alias de paramètres référence (§5.2, §5.3)
                                        └── pile de contextes de boucle FAIRE (§6)
```

Séparation stricte imposée, dans la continuité directe de RCI-M §3 et §8 :

- Aucune structure de ce chapitre ne réside dans la zone RCI-M (table des objets compilés, zone des séquences compilées, table des symboles, table des constantes partagées). Un cadre d'activation ou un élément de pile d'évaluation peut *référencer* un identifiant de symbole (index 16 bits, IMP-0104) ou un numéro de ligne (IMP-0125), jamais l'inverse : RCI-M ne contient et ne contiendra jamais de référence vers l'état d'exécution (RCI-M §10, ligne EXI-0001).
- La table des symboles elle-même (RCI-M §6) n'est *pas* dupliquée par ce chapitre, et l'exécution n'y écrit jamais. L'état et la valeur de chaque identificateur sont portés par l'environnement des identificateurs (§3 bis, IMP-0179), qui associe à chaque nom, désigné par le même identifiant que la table des symboles, une case. Un nom n'a qu'une entrée (ANN-K-0009, « chaque nom n'est conservé qu'une seule fois »), mais il peut changer de case : un identificateur `LOCAL` reçoit une case neuve pour la durée d'une activation, et un paramètre référence désigne la case de l'argument effectif (§5.2 à §5.4, D1).
- Un cadre d'activation possède sa **propre** pile de contextes de boucle `FAIRE` (§6), et non une pile globale unique : ce choix, déjà annoncé par RCI-M §4.8.2 comme alternative rejetée pour la table des symboles (« empêcherait la réentrance en cas d'appel récursif d'une procédure contenant une boucle »), est ici rendu physique.

---

## 3 bis. Environnement des identificateurs

Les Tomes I-II (§11.2, VM-1102) définissent la variable comme « un objet d'exécution comportant : un identificateur logique ; un type ; un état de déclaration ; une valeur courante », et le Tome IV (§4.8) range l'« environnement d'exécution » parmi les composantes minimales de l'état d'exécution. Le corpus fixe les états qu'un identificateur peut prendre et les transitions entre eux :

- un identificateur peut être « non encore utilisé », nom de variable simple, nom de tableau, nom de variable-chaîne, ou « libéré » (LSE15 §3-1.2, §4-3) ;
- « une variable utilisée dans une expression doit avoir reçu préalablement une valeur déterminée. Dans le cas contraire, un message d'erreur est produit » (HIS-12.3, INRDP75 §3-3) ;
- quand on exécute `TABLEAU` sur un identificateur « qui était déjà un nom de tableau, ce tableau est libéré implicitement ; il y a, par contre, détection d'une erreur si cet identificateur était un nom de variable simple ou de variable-chaîne » (LSE15 §3-1.2) ;
- « on ne peut définir comme variable-chaîne qu'un identificateur non encore utilisé, ou qui a été libéré, ou qui est déjà un nom de variable-chaîne » ; pour indiquer qu'il s'agit d'une variable-chaîne, « il faut la déclarer » (LSE15 §4-3) ;
- après `LIBERER`, « on peut réutiliser l'identificateur correspondant comme un nom de tableau ou une variable simple, quelle qu'ait été son utilisation précédente ; la valeur précédente de l'identificateur est définitivement perdue » ; « dans une procédure, on ne peut libérer que les identificateurs déclarés locaux à la procédure » (LSE15 §3-1.2, Remarques 1 et 2 ; §4-3 pour les variables-chaîne) ;
- « toute tentative d'utilisation d'un élément de tableau non déclaré se soldera par un diagnostic d'erreur à l'exécution » (LSE15 §3-1.2) ;
- `EXECUTER` « lance le programme en mettant ou en remettant ses identificateurs dans l'état non-défini » (LSE15 §9-2) ;
- `CHARGER` définit implicitement la cible « comme étant un tableau ou une variable-chaîne (éventuellement une variable simple) » (LSE15 §6-3).

**IMP-0179** — *Environnement des identificateurs : état et contenu de chaque identificateur, tenus par l'état d'exécution, indexés par l'identifiant de symbole*
- Sujet : où sont portés, pendant l'exécution, l'état de déclaration (non défini, variable simple, variable-chaîne, tableau) et la valeur courante de chaque identificateur, et quelles transitions d'état les instructions du langage provoquent-elles ?
- Choix : deux tableaux appartenant à l'état d'exécution (choix d'Éric du 2026-10-23, point D1, après constat sur LSE 8086 version IV) :
  - un tableau de numéros de case, parallèle à la table des symboles du programme courant (RCI-M §6) et **indexé par le même identifiant 16 bits** (RCI-M IMP-0104) : il donne la case courante de chaque nom ;
  - une réserve de cases, agrandie par `realloc`, avec une liste de cases libres ; une case porte l'état et le contenu d'une variable. La réserve est plafonnée à `65535 / sizeof(EntreeEnvironnement)` cases, soit 13 107 cases de 5 octets : en modèle small, ORCA/C suppose qu'aucun tableau obtenu par `malloc` ne dépasse 64 Kio (manuel ORCA/C 2.2, `memorymodel`). Au-delà, l'allocation d'une case rend EX 03 « MEMOIRE SATUREE », comme LSE 8086 (« erreur d'exécution 3 Mémoire pleine », essai d'Éric du 2026-10-30) :

```c
typedef unsigned char EtatIdentificateur;
#define ID_NON_DEFINI  0   /* jamais utilisé depuis EXECUTER, ou libéré */
#define ID_VARIABLE    1   /* variable simple ayant reçu une valeur */
#define ID_CHAINE      2   /* déclarée par CHAINE ; objet nul tant qu'aucune valeur n'est affectée */
#define ID_TABLEAU     3   /* déclaré par TABLEAU, ou défini par CHARGER */
#define ID_PARAMETRE_SANS_VALEUR 4   /* paramètre LOCAL ayant reçu un argument sans valeur */

typedef struct {
    EtatIdentificateur  etat;
    union {
        float        valeur;   /* ID_VARIABLE : valeur directe, RCI-M IMP-0110 */
        HandleObjet  objet;    /* ID_CHAINE, ID_TABLEAU : objet du gestionnaire d'objets ;
                                  GESTOBJ_HANDLE_NUL pour une variable-chaîne sans valeur */
    } contenu;
} EntreeEnvironnement;                 /* une case : 5 octets, aucun bourrage sous ORCA/C */

unsigned short *noms;                  /* nom -> numéro de case */
```

  Le tableau est agrandi par `realloc` à la première opération qui désigne un identifiant au-delà de sa taille, jusqu'au nombre de symboles de la table (`symboles_nombre()`), chaque nouvelle entrée valant `ID_NON_DEFINI` (choix d'Éric du 2026-10-23, point E4). Le compilateur n'a ainsi jamais à connaître l'environnement (Compilateur IMP-0170, EXI-0031) ; un symbole créé par une ligne machine de bureau reçoit son entrée dès sa première utilisation.

  **Paramètre `LOCAL` sans valeur.** Un paramètre `LOCAL` dont l'argument effectif n'a pas de valeur reçoit l'état `ID_PARAMETRE_SANS_VALEUR` (choix d'Éric du 2026-10-23, point C1). Cet état se comporte comme `ID_NON_DEFINI` dans toutes les transitions du tableau ci-dessous, sauf à la lecture de la valeur, qui donne l'erreur « paramètre LOCAL sans valeur » (EX 14) au lieu de celle d'une variable non définie (EX 05, 62 ou 10). Il disparaît au retour de la procédure, par la restauration de la sauvegarde (§5.4). Cet état réalise la détection de l'erreur à l'utilisation du paramètre (Gestionnaire des erreurs §9, RX3). Les entrées des symboles de nature `SYM_PROCEDURE` (RCI-M §6.2) ne sont jamais utilisées. `EXECUTER` (§7.1, étape 2) libère tous les objets désignés par l'environnement et remet toutes les entrées à `ID_NON_DEFINI`.

  Transitions, appliquées par l'interpréteur à l'exécution de chaque opération (elles réalisent les règles citées ci-dessus ; un cas marqué « déduit » découle de la distinction des types posée par LSE15 §3 et §4 sans y être énoncé mot pour mot) :

  | Opération | `ID_NON_DEFINI` | `ID_VARIABLE` | `ID_CHAINE` | `ID_TABLEAU` |
  |---|---|---|---|---|
  | Lecture de la valeur | erreur (HIS-12.3) : EX 05 dans un calcul, EX 62 pour une affectation ou un affichage, EX 10 dans une comparaison (Gestionnaire des erreurs §9, RX1) | la valeur | la chaîne ; erreur si aucune valeur n'a été affectée (LSE15 §7-8.1) | le tableau entier là où le langage l'admet (AFFICHER, LIRE, GARER, argument effectif, RCI-M IMP-0115) ; erreur ailleurs (déduit) |
  | Affectation d'un nombre | → `ID_VARIABLE` | remplace la valeur | erreur (déduit) | erreur (déduit) |
  | Affectation d'une chaîne | erreur : la variable-chaîne doit être déclarée (LSE15 §4-3) | erreur (déduit) | remplace la valeur | erreur (déduit) |
  | `TABLEAU` | → `ID_TABLEAU` | erreur (LSE15 §3-1.2) | erreur (LSE15 §3-1.2) | libération implicite, puis → `ID_TABLEAU` (LSE15 §3-1.2) |
  | `CHAINE` | → `ID_CHAINE`, sans valeur | erreur (LSE15 §4-3) | admis ; la valeur précédente est libérée : → `ID_CHAINE`, sans valeur | erreur (LSE15 §4-3) |
  | `LIBERER` | admis, sans effet | → `ID_NON_DEFINI` | objet libéré, → `ID_NON_DEFINI` | objet libéré, → `ID_NON_DEFINI` |
  | Accès indexé (lecture ou affectation d'un élément) | erreur (LSE15 §3-1.2) | erreur (LSE15 §3-1.2) | erreur (LSE15 §3-1.2) | élément, bornes vérifiées (LSE15 §3-1.2) |
  | `CHARGER` sur la cible | → selon le contenu de l'enregistrement (LSE15 §6-3) | idem, l'ancien contenu étant perdu | idem | idem |

  Règles complémentaires :
  - **`LIBERER` dans une procédure.** Dans un cadre autre que le cadre racine, `LIBERER` d'un identificateur qui n'est pas `LOCAL` à la procédure — c'est-à-dire pour lequel le cadre courant ne porte aucune `SauvegardeLocale` (§5.1) — est une erreur d'exécution (LSE15 §3-1.2, Remarque 2).
  - **`CHAINE` sur une variable-chaîne.** LSE15 §4-3 admet la déclaration d'un identificateur « qui est déjà un nom de variable-chaîne » sans dire si sa valeur est conservée. La valeur **n'est pas conservée** : l'objet est libéré et la variable-chaîne redevient sans valeur, comme après une première déclaration. Ce comportement n'est pas attesté par le corpus ; il est retenu comme règle de réalisation.
  - **Conversion en entier.** Lorsqu'une opération attend un entier et reçoit une valeur non entière, la valeur est **arrondie à l'entier le plus voisin**, la moitié exacte étant arrondie à l'entier supérieur (LSE15 §3-1.2 : `A[4.5]` désigne le cinquième élément) :

    | Valeur convertie | Sources |
    |---|---|
    | Cible d'`ALLER EN` | INRDP75, 3ᵉ partie, §2 : « éventuellement arrondie à l'entier le plus voisin » ; LSE15 §5-1 |
    | Cible de `RETOUR EN` | INRDP75, 4ᵉ partie, §2-2 : « éventuellement arrondie » ; LSE15 §7-5 : « arrondie éventuellement à l'entier le plus voisin » |
    | `n1` de `FAIRE` (§6.1) | LSE15 §5-5.1 ; INRDP75 muet |
    | Facteur de répétition variable `*` d'AFFICHER | LSE15 §6-2.6 : « arrondie éventuellement à l'entier le plus voisin » ; INRDP75 muet |
    | Indice de tableau | LSE15 §3-1.2 : « la valeur de l'indice sera prise égale à l'entier le plus voisin » (`MAT[3.14]` : troisième élément ; `RESUL[5.69]` : sixième) ; LAFOND83 : « la valeur de l'indice sera la valeur de la variable (ou de l'expression) arrondie à l'entier le plus voisin » ; LSE 8086 version IV (1988) : `RESUL[5.69]` désigne le sixième élément. INRDP75 (3ᵉ partie, §5) indique au contraire « après éventuellement suppression de la partie décimale du résultat » ; ce désaccord est résolu par recoupement avec ces deux sources tierces, sans application de la règle de préséance (RCI-M §4.11.1) |

    Un indice, après arrondi, doit être au moins égal à 1 et ne pas dépasser la borne déclarée, sinon une erreur d'exécution est signalée (LSE15 §3-1.2 ; LAFOND83 : « En LSE les indices sont des nombres entiers strictement positifs. L'indice nul est interdit »). Les sources LAFOND83 (Christian Lafond, *Lire LSE, le langage français de programmation, manuel de l'utilisateur*, 1983, même auteur qu'INRDP75) et LSE 8086 version IV (« Le LSE graphique 8086/8088 version IV, selon la norme AF Z 65-020 », implémentation MS-DOS de 1988) ont été consultées directement le 2026-10-22 ; elles ne sont pas versées au dépôt et ne sont citées qu'à titre de recoupement.
  - **Paramètres référence.** Une opération portant sur un paramètre référence s'applique à l'entrée de l'identificateur que désigne l'alias (§5.3), jamais à celle du nom formel.
  - **Hors périmètre.** L'allocation physique des tableaux et des chaînes, la vérification des bornes d'un accès indexé et l'éventuel état non défini de chaque élément de tableau relèvent du gestionnaire des variables, des tableaux et des chaînes (module `donnees`, chapitre distinct, non encore rédigé), qui opère sur les objets désignés par `contenu.objet`.
- Alternatives envisagées : porter l'état et la valeur dans `EntreeSymbole` (rejeté : le programme courant serait écrit à chaque affectation, contrairement à Tome IV §4.8 et à RCI-M IMP-0108 ; la table des symboles mêlerait des données de compilation et des données d'exécution) ; conserver dans la table des symboles une catégorie fixée à la compilation (rejeté : jamais fiable sous EXI-0001, une déclaration TABLEAU ou CHAINE ne prenant effet qu'à l'exécution, RCI-M §4.11.5) ; un objet `HandleObjet` par variable simple (rejeté : double indirection sur l'opération la plus fréquente, RCI-M IMP-0110) ; représenter l'état non défini par une valeur numérique particulière, par exemple un NaN SANE (rejeté : ne distingue pas les catégories, et ferait dépendre une règle du langage d'une propriété du format flottant).
- Justification : la structure réalise directement VM-1102 (état de déclaration et valeur courante) dans l'environnement d'exécution prévu par Tome IV §4.8, sans coût d'accès supplémentaire : l'identifiant porté par la référence compilée indexe à la fois la table des symboles et l'environnement. Les transitions transcrivent les règles du corpus ; les cas qui n'y figurent pas explicitement sont signalés.
- Impact mémoire : 7 octets par nom distinct rencontré dans le programme courant ou en mode machine de bureau (2 pour le numéro de case, 5 pour la case), plus 5 octets par identificateur `LOCAL` actif, en plus des objets chaînes et tableaux eux-mêmes ; 13 107 cases au plus (65 535 octets), ce qui limite par exemple une procédure récursive à un `LOCAL` à 13 000 niveaux environ.
- Impact disque : sans objet — l'environnement n'est jamais sérialisé dans un fichier-programme (RCI-M IMP-0108) ; il fait partie de ce qu'un futur SWAP OUT devra sérialiser (§12).
- Impact performances : accès en O(1), par deux indexations ; un test d'état par opération.
- Impact historique : réalise la détection des objets non initialisés (HIS-12.3), les règles de déclaration et de libération de LSE15 §3-1.2 et §4-3, et la remise à l'état non défini par `EXECUTER` (LSE15 §9-2).
- Références normatives : Tomes I-II §11.1, §11.2 (VM-1101, VM-1102, EXI-0038, EXI-0039), HIS-12.3 ; Tome IV §4.8 ; LSE15 §3-1.2, §4-3, §6-3, §7-8.1, §9-2 ; INRDP75 §3-3 ; RCI-M §4.11.5, §6, IMP-0104, IMP-0108, IMP-0110 ; §5, §7.1 du présent chapitre.
- Statut : retenu ; complété et révisé le 2026-10-23 (points C1, E4 et D1, arbitrages d'Éric : état « paramètre LOCAL sans valeur », agrandissement paresseux, séparation du nom et de la case) ; révisé le 2026-10-30 (Éric ; audit du code du 2026-10-29, AC-07 : plafond de 64 Kio).

---

## 4. Pile d'évaluation

### 4.1 Principe : deux formes d'élément, valeur et référence résolue

Le Tome III (§6.2, §6.3) distingue une valeur immédiate (« l'interpréteur doit pouvoir la déposer sur la pile d'évaluation sans résolution d'identificateur ») d'une référence (« ne contient pas obligatoirement l'objet final ; l'objet final est obtenu par résolution dans l'environnement d'exécution »). RCI-M (§4.5, §4.11.3, §4.11.5) montre que plusieurs opérations codées consomment explicitement une **référence** plutôt qu'une valeur : l'affectation (0x99, IMP-0118 — « consomme une référence et une valeur ») ; GARER (0xAC — « consomme... une référence... désignant la variable/le tableau/la chaîne à garer ») ; CHARGER (0xAD) et LIRE (0xBB — « consomme une référence cible ») ; les paramètres optionnels de sortie de CNB/GRL/SCH (§4.11.4 — « compilés comme une référence... la fonction leur écrit une valeur en retour »). Une opération arithmétique, relationnelle ou de concaténation, à l'inverse, ne consomme que des valeurs.

**IMP-0144** — *Élément de pile d'évaluation à deux formes, déréférencement implicite à la consommation*
- Sujet : un élément RCI-M de catégorie « référence » (§4.5), rencontré par l'interpréteur, doit-il être résolu immédiatement en une *valeur* (perdant la possibilité d'écrire à cet emplacement), ou déposé sur la pile sous une forme qui reste utilisable aussi bien comme cible d'écriture que comme source de lecture ?
- Choix : la pile d'évaluation porte des éléments à deux formes explicites, `VALEUR` et `REFERENCE`. Lorsque l'interpréteur rencontre un élément RCI-M de catégorie référence (0x40/0x41), il **résout entièrement** l'identité logique visée (résolution transitive incluant, le cas échéant, un alias de paramètre référence, §5.3) et dépose sur la pile un élément de forme `REFERENCE` désignant cette identité déjà résolue — jamais la valeur qu'elle contient à cet instant. Une opération qui a besoin d'une valeur (arithmétique, relationnelle, concaténation, argument obligatoire d'une fonction chaîne) *déréférence* implicitement, au moment où elle consomme l'élément : si l'élément est déjà une `VALEUR`, elle l'utilise telle quelle ; si c'est une `REFERENCE`, elle lit la valeur actuellement stockée à l'identité désignée. Une opération qui a besoin d'une cible d'écriture (AFFECTATION 0x99, GARER 0xAC, CHARGER 0xAD, LIRE 0xBB, paramètre optionnel de sortie) exige que l'élément consommé soit de forme `REFERENCE` ; le rencontrer sous forme `VALEUR` à cet emplacement constitue une incohérence de la séquence compilée (ne peut se produire pour un objet compilé valide, cf. Tomes I-II §10.2, VM-1002 : « une consommation impossible constitue une erreur interne... ou une situation qui ne devrait jamais apparaître »).
- Alternatives envisagées : résoudre systématiquement toute référence en valeur dès sa rencontre, et introduire un second sous-code physique RCI-M pour « référence-cible » distinct de « référence-source » (rejeté : modifierait RCI-M, chapitre déjà finalisé, pour une distinction qui n'a jamais besoin d'être connue avant l'exécution de l'opération consommatrice — la même occurrence syntaxique `A` peut se voir consommée comme cible ou comme source selon l'opération qui la suit, ex. `A←A+1` où le premier `A` est cible et le second source ; ces deux occurrences compilent d'ailleurs le même élément 0x40, RCI-M IMP-0120) ; conserver un registre de résolution unique hors pile plutôt qu'un élément empilé (rejeté : contredit directement Tome III §6.6, « la pile ne constitue pas une seconde représentation compilée ; elle est un état d'exécution temporaire » — un registre unique ne permettrait pas d'empiler plusieurs références en attente, situation qui se produit dès qu'une expression contient plus d'un opérande de type référence non encore consommé, ex. `A←B`).
- Justification : ce choix ne modifie aucune propriété logique déjà fixée par RCI-M (§4.5 : « la référence ne contient jamais l'objet final ; elle contient un identifiant logique résolu... à l'exécution ») ; il en précise seulement le mécanisme physique côté pile. Il respecte Tome III §6.3 (« l'objet final est obtenu par résolution dans l'environnement d'exécution ») en faisant de cette résolution une étape explicite et unique, réalisée une seule fois par occurrence de référence, quel que soit l'usage qui en sera fait ensuite.
- Impact mémoire : un élément de pile porte au pire le plus grand des deux contenus possibles (valeur ou référence, §4.2) ; pas de structure supplémentaire.
- Impact disque : sans objet (état d'exécution, jamais sérialisé dans un fichier-programme).
- Impact performances : une résolution par occurrence de référence, au lieu d'une résolution paresseuse potentiellement répétée si la même référence était relue plusieurs fois avant consommation — cas qui ne se produit d'ailleurs jamais en pratique puisque chaque référence n'est empilée qu'une fois et consommée qu'une fois par la séquence postfixée qui la suit immédiatement (EXI-0011).
- Impact historique : sans objet — mécanisme interne de l'interpréteur, aucun comportement L.S.E. observable n'en dépend (le résultat de `A←A+1` reste rigoureusement identique quel que soit le mécanisme interne retenu).
- Références normatives : Tome III §6.2, §6.3, §6.6 ; Tomes I-II §10.1 (VM-1001), §10.2 (VM-1002) ; RCI-M §4.5, §4.11.3 (IMP-0118), §4.11.5.
- Statut : retenu.

### 4.2 Format physique d'un élément de pile

```c
typedef unsigned char TypeElementPile;
#define PILE_NUMERIQUE   0   /* float SANE, même format que RCIM_VAL_REEL 0x02 (RCI-M IMP-0103) */
#define PILE_BOOLEEN     1   /* 0 ou 1, même convention que RCIM_VAL_FAUX/RCIM_VAL_VRAI 0x04/0x05 */
#define PILE_CHAINE      2   /* chaîne désignée par ChaineSurPile, transitoire ou non (ci-dessous) */
#define PILE_REFERENCE   3   /* identité résolue, jamais une valeur */

#define REF_SIMPLE     0   /* variable (RCI-M 0x40) : case désignée */
#define REF_ELEMENT    1   /* élément de tableau (RCI-M 0x41) : case du tableau et rang */
#define REF_PROCEDURE  2   /* nom de procédure : identifiant de symbole, aucune case */

typedef struct {
    unsigned short   cellule;        /* case de l'environnement (D1) ; identifiant de symbole
                                         pour REF_PROCEDURE */
    unsigned char     indexee;        /* REF_SIMPLE, REF_ELEMENT ou REF_PROCEDURE */
    unsigned int      decalage;       /* offset résolu dans le tableau si indexee = 1 ; non significatif sinon
                                          — le calcul et la validation de bornes appartiennent au gestionnaire
                                          des tableaux, module distinct, non encore rédigé ; ce champ porte
                                          uniquement le résultat déjà validé */
} ReferenceResolue;

typedef struct {
    HandleObjet    objet;        /* objet qui contient le texte : chaîne produite par une opération,
                                    texte d'une constante partagée (RCI-M IMP-0107), valeur d'une
                                    variable-chaîne, ou bloc compilé de la ligne en cours pour une
                                    chaîne littérale en ligne (RCI-M §4.4, 0x03) */
    unsigned char  decalage;     /* position du premier caractère dans l'objet : position du texte
                                    dans le bloc pour une chaîne en ligne ; 0 sinon */
    unsigned int   longueur;     /* nombre de caractères */
    unsigned char  transitoire;  /* 1 : objet créé pour ce résultat, à libérer au retrait (§4.4) ;
                                    0 : texte appartenant à autrui, en lecture seule, jamais libéré */
} ChaineSurPile;

typedef struct {
    TypeElementPile  type;
    union {
        float             numerique;
        unsigned char      booleen;
        ChaineSurPile      chaine;
        ReferenceResolue   reference;
    } contenu;
} ElementPile;
```

**IMP-0144 bis — Promotion uniforme des valeurs numériques en `float`.** Une valeur immédiate entière (RCI-M 0x00, 0x01, 0x06, 0x07–0x3F) est promue en `float` SANE au moment où elle est déposée sur la pile, quel que soit le sous-format compact utilisé par RCI-M pour l'économie d'espace en mémoire (IMP-0103, IMP-0123). Cette promotion est cohérente avec IMP-0110 (RCI-M §6.1) qui stocke déjà toute variable arithmétique scalaire sous forme `float` unique dans `EntreeSymbole` : l'économie de représentation de RCI-M (entier court, entier, réel, ou valeur nulle en charge utile) est un choix de compacité de la représentation *compilée*, sans conséquence sur la représentation *d'exécution*, qui reste uniformément numérique conformément au Tome II (le L.S.E. ne distingue pas de type entier/réel au niveau du langage). Statut : retenu ; rattaché à IMP-0144 plutôt que numéroté séparément, s'agissant d'une simple précision de format et non d'un choix de structure.

### 4.3 Gestion mémoire de la pile

**IMP-0145** — *Pile d'évaluation : tableau dynamique redimensionné par `realloc`, jamais géré par le gestionnaire d'objets*
- Sujet : la pile d'évaluation doit-elle être allouée via le gestionnaire d'objets (handles compactables du Memory Manager toolbox, IMP-0132) ou via `malloc`/`realloc` standard ?
- Choix : un tableau `ElementPile *` alloué par `malloc` et agrandi par `realloc` lorsque le sommet atteint la capacité courante, jamais géré par handle. Un compteur `sommet` (index du premier emplacement libre) et une `capacite` (nombre d'éléments alloués) suffisent ; aucun élément interne à la pile n'est jamais référencé de l'extérieur de l'interpréteur (§2), donc aucune compaction n'est requise. La capacité est plafonnée à `65535 / sizeof(ElementPile)` éléments, soit 9 362 éléments de 7 octets : en modèle small, ORCA/C suppose qu'aucun tableau obtenu par `malloc` ne dépasse 64 Kio, et calcule sur 16 bits l'adresse d'un élément (manuel ORCA/C 2.2, `memorymodel`). Au-delà du plafond, empiler rend EX 03 « MEMOIRE SATUREE » ; LSE 8086 signale de même « erreur d'exécution 3 Mémoire pleine » sur une récursion de 20 000 niveaux (essai d'Éric du 2026-10-30) et un déplacement physique lors d'un `realloc` ne pose aucun problème d'invalidation de référence externe.
- Alternatives envisagées : adressage des éléments en `unsigned long`, sans plafond (rejeté : il ralentirait l'opération la plus fréquente de l'interpréteur, et la mémoire finirait de toute façon par manquer, comme sur LSE 8086) ; handle compactable (Memory Manager toolbox), par cohérence avec les objets dynamiques RCI-M (rejeté : le verrouillage/déverrouillage (`HLock`/`HUnlock`) exigé à chaque accès introduirait un coût sur l'opération la plus fréquente de l'interpréteur — chaque élément d'une expression passe par la pile — sans aucun bénéfice, puisque rien n'a besoin de compacter une structure strictement LIFO et jamais référencée depuis l'extérieur) ; pile de taille fixe allouée une fois pour toutes à une profondeur maximale arbitraire (rejeté : contredit le conditionnement §4.1, « éviter... structures surdimensionnées », pour une profondeur dont rien dans le corpus ne fixe de borne supérieure raisonnable).
- Justification : la propriété LIFO démontrée en §2 rend la compaction sans objet ; `realloc` sur un bloc de moins de 4 Kio est pris en charge par le sous-allocateur du runtime ORCA/C (manuel ORCA/C chap. 20), cohérent avec Architecture physique §6.1.
- Impact mémoire : croissance par doublement, de 16 éléments à 9 362 au plus (65 534 octets), sans réservation mémoire globale au démarrage (Architecture physique IMP-0133) ; aucune fragmentation liée à la compaction puisqu'aucune compaction n'est nécessaire. Une récursion en position d'expression, qui laisse un élément par niveau, est ainsi limitée à 9 000 niveaux environ.
- Impact disque : sans objet.
- Impact performances : accès au sommet en O(1), sans verrouillage de handle ; coût amorti du redimensionnement négligeable devant la fréquence des opérations de pile.
- Impact historique : neutre — décision de réalisation pure.
- Références normatives : Tome III §6.6 ; Architecture physique §6.1 (IMP-0132), §6.2 (IMP-0133) ; conditionnement §4.1, §9.
- Statut : retenu ; révisé le 2026-10-30 (Éric ; audit du code du 2026-10-29, AC-06 : plafond de 64 Kio).

### 4.4 Opérations primitives (réalisation physique de VM-1002)

Tomes I-II §10.2 (VM-1002) définit les primitives abstraites `EMPILER`, `LIRE sommet`, `RETIRER sommet`, `REMPLACER sommet`, `CONSOMMER n valeurs`, `PRODUIRE valeur`. Leur réalisation physique directe :

```c
void   pile_empiler(PileEvaluation *p, ElementPile e);
ElementPile pile_lire_sommet(const PileEvaluation *p);
ElementPile pile_retirer(PileEvaluation *p);
void   pile_consommer(PileEvaluation *p, unsigned int n);   /* dépile n éléments sans les retourner */
float  pile_lire_valeur_numerique(const PileEvaluation *p, unsigned int profondeur);
                                    /* déréférence implicitement si l'élément est une REFERENCE, §4.1 */
```

**Chaînes constantes et chaînes transitoires.** Une chaîne déposée sur la pile porte un indicateur `transitoire` (`ChaineSurPile`, §4.2). Une chaîne littérale (élément RCI-M 0x03) est déposée comme **non transitoire** : en mode « texte en ligne », l'élément désigne le texte à sa position dans le bloc compilé de la ligne en cours ; en mode « index », il désigne le texte de l'entrée de la table des constantes partagées (RCI-M IMP-0107). Aucune copie n'est faite au dépôt, et ce texte n'est jamais modifié ni libéré par l'exécution. La valeur d'une variable-chaîne, obtenue par déréférencement d'une référence (§4.1), est elle aussi déposée comme non transitoire. Seul un résultat produit par une opération — concaténation `!`, fonction chaîne (RCI-M §4.11.4) — est un objet **transitoire**, créé par le gestionnaire des chaînes via le gestionnaire d'objets (IMP-0132). Lorsqu'un élément transitoire est retiré de la pile (`pile_retirer`) sans avoir été repris par une affectation, il doit être libéré immédiatement par l'appelant de `pile_retirer`, faute de quoi il devient une fuite mémoire à chaque expression-chaîne évaluée ; un élément non transitoire n'est jamais libéré. Une affectation à une variable-chaîne copie toujours le texte dans un objet propre à la variable (§3 bis), sauf lorsqu'elle reprend un élément transitoire, dont elle peut alors devenir propriétaire sans copie. L'algorithme précis de création, de copie et de libération des chaînes relève du chapitre consacré au module `donnees`, non encore rédigé (cf. §12).

---

## 5. Cadres d'activation

### 5.1 Structure C

**IMP-0146** — *Structure physique du cadre d'activation*
- Sujet : quelles informations un cadre d'activation doit-il porter physiquement pour satisfaire VM-0901 (paramètres, objets locaux, appelant, position de retour, restitution du résultat) et VM-0903 (position de retour abstraite) ?
- Choix :

```c
typedef struct SauvegardeLocale {
    unsigned short       symbole;          /* IMP-0104 : identifiant dans la table des symboles */
    unsigned short       ancienne_cellule; /* case du nom avant l'appel (§3 bis, IMP-0179, D1) */
} SauvegardeLocale;

typedef struct AliasReference {
    unsigned short     symbole_formel;  /* identifiant du paramètre référence, tel que compilé dans le
                                            corps de la procédure (RCI-M §4.5, élément 0x40) */
    ReferenceResolue    cible;           /* case de l'argument effectif, capturée à l'appel (§5.3, D1)
                                            — jamais un second niveau d'alias à suivre */
} AliasReference;

typedef struct CadreActivation {
    struct CadreActivation *appelant;             /* VM-0902 ; NULL pour le cadre racine (§7.1) */
    unsigned char            numero_ligne_retour;   /* IMP-0125 ; position de retour, VM-0903 */
    unsigned int             offset_retour;         /* offset intra-séquence de reprise sur cette ligne */
    unsigned char            position_appel;        /* 0 = instruction, 1 = expression — recopié de
                                                         l'indicateur RCI-M IMP-0121 au moment de l'appel */
    unsigned char             numero_ligne_procedure;/* InfoProcedure.numero_ligne_entree, RCI-M IMP-0114 —
                                                         identifie la procédure dont ce cadre est l'activation */
    unsigned int              hauteur_base;          /* hauteur de la pile d'évaluation à l'entrée du
                                                         cadre (0 pour le cadre racine) ; entre deux
                                                         instructions, la pile du cadre y revient (§7.3,
                                                         choix d'Éric du 2026-10-24, point B2) */

    SauvegardeLocale         *sauvegardes;          /* tableau, une entrée par identificateur LOCAL actif */
    unsigned char              nb_sauvegardes;

    AliasReference            *alias;                /* tableau, une entrée par paramètre référence lié */
    unsigned char               nb_alias;

    ContexteBoucle            *pile_boucles;         /* §6.1 — pile propre à ce cadre */
    unsigned char               nb_boucles_actives;
    unsigned char               capacite_boucles;

    DescriptionAffichage        affichage;           /* description de l'AFFICHER de ce cadre,
                                                         suspendue pendant un appel qu'il fait
                                                         (Interpréteur IMP-0221) ; 4 octets opaques */
} CadreActivation;
```

- Alternatives envisagées : un unique tableau `CadreActivation[]` de profondeur maximale fixe, indexé par un compteur d'appels imbriqués (rejeté : impose une limite arbitraire de récursivité, contraire au conditionnement §4.1 « éviter... structures surdimensionnées », et rend la libération d'un cadre plus complexe qu'une simple désallocation en tête de chaîne) ; fusionner `sauvegardes` et `alias` en une seule table taguée par un discriminant supplémentaire (rejeté : les deux mécanismes ont des durées de vie et des sémantiques distinctes — une sauvegarde restaure une valeur, un alias redirige une résolution — et leur fusion physique n'apporterait aucune économie mesurable pour un gain de clarté perdu).
- Justification : le chaînage par pointeur `appelant` réalise directement VM-0902 (« le cadre courant désigne le cadre appelant ») sans limite de profondeur autre que la mémoire disponible ; `numero_ligne_retour`/`offset_retour` réalisent VM-0903 (« la position n'est pas une adresse mémoire ») en réutilisant exactement les mêmes types que RCI-M (IMP-0125) pour la position d'une ligne — aucune conversion n'est nécessaire entre une position RCI-M et une position de retour.
- Impact mémoire : un cadre coûte un pointeur (4 octets) + 2 octets de position + 2 octets d'indicateurs + trois paires pointeur/compteur (tableaux à croissance dynamique, alloués séparément) — dimensionné par le nombre réel de paramètres/locaux/boucles actives de la procédure concernée, jamais par un majorant global.
- Impact disque : sans objet à ce stade (état d'exécution) ; ces champs sont précisément ceux qu'un futur protocole SWAP OUT (ALG-1502, Tome IV §15.3 : « position d'exécution, ..., cadres d'activation ») devra sérialiser (cf. §12, limite explicite).
- Impact performances : accès au cadre courant en O(1) (pointeur direct, jamais de recherche) ; résolution d'un alias ou d'une sauvegarde en O(k) où k est le nombre de paramètres/locaux de la procédure — borné par l'arité déclarée (RCI-M IMP-0114), donc petit en pratique pour un langage pédagogique.
- Impact historique : neutre — représentation physique pure, sans conséquence sur le comportement observable (LSE15 §6, §7).
- Références normatives : Tomes I-II §9.1 à §9.3 (VM-0901 à VM-0903) ; RCI-M IMP-0104, IMP-0114, IMP-0121, IMP-0125 ; conditionnement §4.1, §9.
- Statut : retenu ; complété le 2026-10-24 par le champ `hauteur_base` (point B2) ; complété le 2026-10-30 par le champ `affichage` (Éric ; audit du code du 2026-10-29, AC-09).

### 5.2 Paramètres formels : `LOCAL` et référence

Le corpus (LSE15 §7-7, §7-8, §7-9) établit une distinction précise, déjà résumée en §1 :

- un identificateur figurant dans la liste `LOCAL` d'une déclaration `PROCEDURE` — qu'il s'agisse d'un paramètre formel ou d'une simple variable/tableau de travail — désigne, à l'intérieur du corps de la procédure, un objet dont la valeur est **sauvegardée** à l'entrée et **restaurée** à la sortie (LSE15 §7-9 : « on retrouvera dans I la valeur qui y était au moment de l'appel ») ; s'il s'agit d'un paramètre, sa valeur est en outre initialisée à celle du paramètre effectif correspondant (passage par valeur, LSE15 §7-8.1 : « on ne peut... avoir aucune influence sur cette valeur du paramètre effectif ») ;
- un paramètre formel **non** présent dans la liste `LOCAL` — dit « paramètre de référence » — désigne, pendant toute la durée de l'appel, le **même objet** que le paramètre effectif correspondant, lequel doit être une variable, une variable-chaîne ou un nom de tableau (jamais une constante ni une expression, LSE15 §7-8.2, Remarque 1 : « la procédure a besoin d'avoir l'adresse... les constantes et expressions... n'ont... qu'une valeur ») ; toute modification à l'intérieur de la procédure est directement visible dans le programme appelant après le retour (LSE15 §7-8.2, exemple `&RAC2`) ;
- un identificateur ni paramètre ni présent dans `LOCAL` est dit *global* : sa valeur avant l'appel est celle qui lui correspondait avant l'appel, et toute modification lui survit après le retour (LSE15 §7-7).

Les identificateurs `LOCAL` et les paramètres référence utilisent tous deux l'**unique** identifiant de symbole attribué par le compilateur au nom textuel considéré (RCI-M, ANN-K-0009). Ce qui change pendant une activation est la **case** que l'environnement associe à ce nom (§3 bis) : un identificateur `LOCAL` reçoit une case neuve, et un paramètre référence est lié à la case de l'argument effectif. Un paramètre référence continue ainsi de désigner la variable de l'appelant même lorsque la procédure déclare `LOCAL` un identificateur de même nom que cette variable (D1) :

```
10 A←1;&P(A);AFFICHER A;TERMINER
20 PROCEDURE &P(B) LOCAL A;A←2;B←3;RETOUR
```

affiche `3` : `B` désigne la case de `A` du programme principal, et `A←2` écrit dans la case locale. C'est le comportement constaté par Éric sur LSE 8086 version IV le 2026-10-23.

**Mode de passage : information de compilation, consultée à l'appel.** Le mode de passage de chaque paramètre et la liste des identificateurs `LOCAL` non-paramètres sont fixés par le seul texte de la ligne de déclaration. La compilation de cette ligne produit un élément de déclaration (RCI-M élément de contrôle 0xCD ; Compilateur §13.2, IMP-0174), dont le gestionnaire du programme courant dérive la **table des paramètres formels** (`TableParametresFormels`, Compilateur §13.3, IMP-0147), une entrée par ligne de déclaration indexée par `numero_ligne_entree`. Le présent chapitre n'en utilise que ce qui suit : pour la déclaration en vigueur d'une procédure (`InfoProcedure`, RCI-M IMP-0114), la liste ordonnée des paramètres formels avec leur mode, `PARAM_LOCAL` ou `PARAM_REFERENCE`, et la liste des identificateurs `LOCAL` non-paramètres. À l'appel (§5.5), ce mode indique, paramètre par paramètre, si l'argument déjà déposé sur la pile d'évaluation (§4.1) doit être copié dans une sauvegarde (§5.4) ou lié dans un alias (§5.3). Le mode n'est jamais déduit de la forme physique, `VALEUR` ou `REFERENCE`, de l'argument (Compilateur IMP-0147, alternative 2). Les références compilées à l'intérieur du corps restent de simples éléments 0x40/0x41 génériques (RCI-M IMP-0120), indépendants du mode de passage.

### 5.3 Résolution des références (réalisation de VM-0802, EXI-0032, EXI-0038)

L'algorithme de résolution, appliqué chaque fois que l'interpréteur rencontre un élément RCI-M de catégorie référence (0x40/0x41, IMP-0144) :

```text
RESOUDRE(symbole_id, index_eventuels)
1. cadre ← cadre d'activation courant
2. SI cadre existe ET symbole_id figure parmi les alias de cadre (§5.2)
       cible ← alias correspondant (§5.1, AliasReference.cible)      { déjà pleinement résolue,
                                                                         un seul niveau, cf. IMP-0148 }
       RETOURNER cible, éventuellement réindexée si index_eventuels et cible désigne un tableau
   SINON SI symbole_id est un nom de procédure
       RETOURNER { cellule: symbole_id, indexee: REF_PROCEDURE }
   SINON
       RETOURNER { cellule: case courante du nom symbole_id (§3 bis, D1),
                   indexee: (index_eventuels ≠ vide), decalage: calculé si indexee }
   FIN SI
```

**IMP-0148** — *Résolution eagère et à un seul niveau des alias de paramètres référence*
- Sujet : lorsqu'un identificateur `symbole_id` désigne, dans le cadre courant, un paramètre référence dont la cible *effective* est elle-même, dans le cadre appelant, un autre paramètre référence (chaînage d'appels transmettant un même paramètre de proche en proche — cas illustré par LSE15 §7-8.2, exemple `&TRAC` recevant `&F1` comme paramètre référence de type nom de procédure, potentiellement retransmis plus loin), la résolution doit-elle conserver une chaîne d'alias à suivre à chaque lecture, ou capturer immédiatement la cible finale au moment de la liaison ?
- Choix : capture eagère. Au moment où `AliasReference` est constitué (§5.5, lors de l'appel), la cible enregistrée est le résultat déjà entièrement résolu de l'évaluation de l'argument effectif au site d'appel — laquelle a elle-même, si cet argument était à son tour un paramètre référence du cadre appelant, déjà traversé sa propre résolution (§4.1 : toute rencontre d'un élément référence par l'interpréteur applique immédiatement l'algorithme `RESOUDRE` ci-dessus, y compris lorsqu'elle a lieu à l'intérieur du corps d'une procédure). Chaque `AliasReference` ne contient donc jamais, par construction, la cible d'un *autre* alias, mais toujours l'identité finale : la **case** de l'argument effectif, avec ou sans indexation résolue, ou le symbole d'une procédure passée en argument (choix d'Éric du 2026-10-23, point D1, après constat sur LSE 8086 version IV). Parce que la cible est une case et non un nom, un identificateur `LOCAL` de même nom déclaré par la procédure n'en détourne pas l'alias (§5.2).
- Alternatives envisagées : chaîne d'alias paresseuse, suivie à chaque lecture jusqu'à une entrée non-alias (rejeté : nécessiterait de porter, dans `AliasReference`, une indication du cadre appelant à consulter ensuite — information supplémentaire pour un résultat strictement identique, puisque la capture eagère produit exactement la même cible finale, sans jamais avoir besoin de retraverser la chaîne à chaque accès) ; interdire le passage d'un paramètre référence comme argument d'un autre paramètre référence (rejeté : contredit directement l'exemple `&TRAC`/`&F1` du corpus, qui repose précisément sur cette possibilité).
- Justification : la capture eagère découle directement du principe déjà posé en IMP-0144 (résolution complète à la rencontre de l'élément référence, quel que soit le contexte d'exécution dans lequel cette rencontre a lieu) ; elle simplifie la structure de `AliasReference` (un seul niveau, jamais récursif) sans perte de généralité, puisque le résultat de la résolution transitive est identique dans les deux approches — seul le moment du calcul diffère.
- Impact mémoire : `AliasReference` reste de taille fixe (§5.1), sans indirection supplémentaire.
- Impact disque : sans objet.
- Impact performances : résolution d'un accès à un paramètre référence en O(1) (lecture directe de l'alias déjà capturé), quelle que soit la profondeur de la chaîne d'appels ayant transmis ce paramètre de proche en proche.
- Impact historique : couvre fidèlement l'exemple `&TRAC` du corpus (LSE15 §7-8.2).
- Références normatives : LSE15 §7-8.2 ; IMP-0144 ; Tomes I-II §8.2 (VM-0802, EXI-0032) ; Tome III §7.5 (« le registre est vidé ou invalidé lorsqu'il cesse d'être utilisé »).
- Statut : retenu ; révisé le 2026-10-23 : l'alias capture une case (point D1).

**Portée d'un alias ou d'une sauvegarde.** Un alias (§5.1, `AliasReference`) ou une sauvegarde (`SauvegardeLocale`) n'est consulté que pour le cadre qui le porte : la fonction `RESOUDRE` ne parcourt jamais la chaîne des cadres appelants au-delà du cadre courant, puisque, par construction (IMP-0148), une cible d'alias a toujours déjà été résolue jusqu'à sa forme finale au moment de la liaison. Ceci réalise directement EXI-0038 (« chaque variable... est associée à un identificateur logique permettant son accès dans le contexte d'exécution où elle est visible ») sans jamais avoir besoin d'une recherche multi-niveaux dans la chaîne d'activation — contrairement à un modèle de portée lexicale imbriquée classique, dont le corpus (LSE15 §7-7 à §7-9) ne documente à aucun moment l'existence pour le L.S.E.

### 5.4 Sauvegarde et restauration des identificateurs `LOCAL`

**IMP-0149** — *Une case neuve pour chaque identificateur `LOCAL`, l'ancienne case du nom étant sauvegardée*
- Sujet : un identificateur `LOCAL` (paramètre ou variable de travail) doit-il réutiliser, pour la durée de l'appel, le stockage déjà associé à son nom, ou recevoir un stockage distinct ?
- Choix : un stockage distinct, sous la forme d'une case neuve de l'environnement (§3 bis) (choix d'Éric du 2026-10-23, point D1, après constat sur LSE 8086 version IV). À l'entrée d'un appel (§5.5), pour chaque identificateur `LOCAL` de la procédure (paramètre ou non, table des paramètres formels, Compilateur §13.3, IMP-0147), l'interpréteur alloue une case neuve et empile sur `CadreActivation.sauvegardes` une `SauvegardeLocale` qui garde le numéro de la case précédente du nom ; le nom désigne ensuite la case neuve. S'il s'agit d'un paramètre, la case reçoit la valeur (ou, pour une chaîne ou un tableau, une copie indépendante, faite par le module `donnees`) de l'argument effectif déjà évalué, l'état prenant la catégorie de cette valeur (`ID_VARIABLE`, `ID_CHAINE` ou `ID_TABLEAU`), ou `ID_PARAMETRE_SANS_VALEUR` si l'argument n'a pas de valeur ; s'il s'agit d'un identificateur `LOCAL` non-paramètre, la case est `ID_NON_DEFINI`, conformément à LSE15 §7-9. À la sortie de l'appel (§5.5), chaque case locale est rendue à la réserve, l'objet qu'elle désigne étant libéré (LSE15 §7-9 : les tableaux et variables locaux « sont libérés automatiquement à la sortie de la procédure »), et chaque nom retrouve sa case précédente, dans l'ordre inverse des sauvegardes. Les cases non locales — en particulier celles que désignent les paramètres référence — ne sont jamais touchées par l'appel ni par le retour.
- Alternatives envisagées :
  1. Réutiliser l'unique case du nom, son contenu étant sauvegardé à l'appel puis restauré au retour — écartée : un paramètre référence désignant une variable de même nom qu'un identificateur `LOCAL` de la procédure écrirait dans la valeur locale, puis perdrait cette écriture au retour. Le programme `10 A←1;&P(A);AFFICHER A` / `20 PROCEDURE &P(B) LOCAL A;A←2;B←3;RETOUR` afficherait `1`, alors que LSE 8086 version IV affiche `3`.
  2. Une case par nom, avec un compteur de masquages que l'alias capturerait — écartée : la lecture d'une variable masquée par l'alias devrait rechercher la bonne sauvegarde dans la chaîne des cadres, en O(profondeur), pour une économie de 2 octets par nom.
  3. Allouer, pour chaque identificateur `LOCAL`, une entrée distincte dans la table des symboles — écartée : romprait ANN-K-0009 et ferait dépendre la représentation compilée de l'activation.
- Justification : séparer le nom de la case est la réalisation la plus simple qui donne au paramètre référence le sens que lui donne LSE15 §7-8.2 (« la procédure a besoin d'avoir l'adresse » de la variable) et que montre LSE 8086. La récursivité (LSE15 §7-6, exemple `&FACT`) découle de la discipline LIFO : chaque appel récursif reçoit sa propre case, et chaque retour rend la case de l'appel englobant.
- Impact mémoire : 5 octets par identificateur `LOCAL` actif, plus une `SauvegardeLocale` de 4 octets ; jamais un tableau dimensionné a priori.
- Impact disque : sans objet à ce stade ; élément à sérialiser par un futur SWAP OUT (cf. §12).
- Impact performances : coût en O(k) par appel et par retour, k étant le nombre d'identificateurs `LOCAL` de la procédure ; une indexation de plus par accès à une variable (§3 bis).
- Impact historique : reproduit le mécanisme de LSE15 §7-9, y compris pour les tableaux et la récursivité (§7-6), et le passage par référence de LSE15 §7-8.2 dans le cas de noms homonymes, constaté sur LSE 8086 version IV.
- Références normatives : LSE15 §7-6, §7-7, §7-8.1, §7-8.2, §7-9 ; ANN-K-0009 ; Tomes I-II §13.1 (VM-1301, EXI-0042) ; IMP-0148, IMP-0179.
- Statut : retenu ; révisé le 2026-10-23 : une case neuve par identificateur `LOCAL` (point D1, constat sur LSE 8086 version IV).

### 5.5 Appel et retour (réalisation physique d'ALG-1201/12.2/12.3 et de RCI-M §4.8.3, §4.11.5)

**Appel (élément APPEL, RCI-M 0xA8, IMP-0121, IMP-0177).**

```text
APPELER(indicateur_position, k)
{ indicateur_position, k : les deux octets de charge utile de l'élément APPEL (RCI-M IMP-0121,
  IMP-0177). Au moment de l'appel, la pile d'évaluation porte, du sommet vers la base, les k
  arguments effectifs (le dernier écrit au sommet), puis la référence à la procédure, déposée
  avant eux par le site d'appel (EXI-0011). }
1. symbole_procedure ← élément de la pile à la profondeur k (lu sans dépiler ; forme REFERENCE,
                         déjà résolue par RESOUDRE, §5.3, alias compris — cas d'un paramètre
                         référence nom de procédure, LSE15 §7-8.2)
   resoudre symbole_procedure → InfoProcedure (numero_ligne_entree, arite ; RCI-M IMP-0114)
   SI numero_ligne_entree = 0 → erreur d'exécution (procédure non déclarée dans le programme
                                 courant ; Compilateur §13.5, IMP-0176)
   SI k ≠ arite → erreur d'exécution (LSE15 §7-8 : « il doit toujours y avoir autant de
                   paramètres effectifs... que de paramètres formels » ; RCI-M IMP-0177)
   { ces deux erreurs sont signalées avant toute création de cadre et sans rien dépiler : la
     pile d'évaluation reste observable dans l'état du site d'appel, EXI-0033 }
2. resoudre numero_ligne_entree → entrée correspondante de la table des paramètres formels
   (Compilateur §13.3, IMP-0147)
3. créer un nouveau CadreActivation :
     appelant ← cadre courant
     numero_ligne_retour, offset_retour ← position d'exécution courante, immédiatement après
                                            l'élément APPEL (EXI-0011 : la reprise après un appel
                                            en position d'instruction suit la même convention que
                                            pour SI/FAIRE, RCI-M §4.8.2 — même ligne après ';' ou
                                            ligne suivante)
     position_appel ← indicateur_position (0x00/0x01, RCI-M IMP-0121)
     numero_ligne_procedure ← numero_ligne_entree
     pile_boucles ← vide (une procédure démarre sans contexte de boucle actif, §6)
4. POUR chaque paramètre formel de rang i (1 ≤ i ≤ k), associé à l'argument effectif de même
   rang, situé à la profondeur k − i de la pile d'évaluation (les k arguments sont dépilés,
   du dernier au premier, à mesure de leur liaison) :
     SI mode = PARAM_REFERENCE
         vérifier que l'élément dépilé est de forme REFERENCE (sinon, erreur d'exécution —
         un argument effectif de paramètre référence est toujours une variable ou un nom de
         tableau, jamais une expression, LSE15 §7-8.2 Remarque 1 ; cette règle n'est vérifiable
         qu'ici : le compilateur ne connaît pas, au site d'appel, le mode des paramètres
         formels, la ligne de déclaration pouvant être compilée avant, après ou sans la ligne
         d'appel, EXI-0001 — Compilateur §13.5, IMP-0176 ; EXI-0019)
         ajouter { symbole_formel, cible: élément.reference } à alias du nouveau cadre (§5.3)
     SINON  { PARAM_LOCAL }
         { toutes les valeurs sont calculées avant toute liaison, puis : }
         empiler une SauvegardeLocale pour symbole_formel (case actuelle du nom, §3 bis, D1)
         le nom symbole_formel reçoit une case neuve, qui reçoit la valeur/l'objet déréférencé de
         l'élément (§4.1 : déréférencement implicite si l'élément est lui-même une REFERENCE ; copie
         d'une chaîne ou d'un tableau par le module donnees)
         { si l'argument n'a pas de valeur, aucune erreur n'est signalée à l'appel : l'entrée reste
           sans valeur et l'erreur « paramètre LOCAL sans valeur » (ERR_E_PARAMETRE_SANS_VALEUR,
           EX 14) n'est signalée qu'à l'utilisation du paramètre — MEMENTO82, annexe II ; constat
           d'Éric sur LSE 8086 version IV, 2026-10-23 ; Gestionnaire des erreurs §9, RX3 }
     FIN SI
   puis retirer de la pile la référence à la procédure, désormais au sommet
5. POUR chaque identificateur LOCAL non-paramètre de l'entrée de la table des paramètres formels :
     empiler une SauvegardeLocale ; le nom reçoit une case neuve, ID_NON_DEFINI (D1)
6. cadre courant ← nouveau CadreActivation
7. position d'exécution ← numero_ligne_entree, élément qui suit immédiatement l'élément de
   déclaration 0xCD en tête de sa séquence (Compilateur §13.2, IMP-0174) — c'est-à-dire la
   première instruction écrite après ';' sur la ligne de déclaration, ou, s'il n'y en a pas,
   la fin de cette séquence, d'où l'exécution passe à la ligne suivante
```

**Retour (RETOUR, RETOUR EN, RESULTAT — RCI-M éléments 0xC4, 0xC5/0xC6, 0xCB, §4.7, §4.8.3).**

```text
RETOURNER(a_resultat, resultat_eventuel, cible_retour_en)
1. SI cadre courant = cadre racine (§7.1) → erreur d'exécution (retour hors de toute procédure)
1 bis. SI a_resultat ET cadre_courant.position_appel = 0x00 (appel en position d'instruction)
       → erreur d'exécution « RESULTAT dans une procédure sous-programme »
         (ERR_E_RESULTAT_SOUS_PROGRAMME, EX 22 ; MEMENTO82, annexe II ; constat d'Éric sur
         LSE 8086 version IV, 2026-10-23 ; LSE15 §7-4 est muet)
   SI NON a_resultat ET cible_retour_en absente ET cadre_courant.position_appel = 0x01
       → erreur d'exécution « RETOUR sans RESULTAT dans une fonction »
         (ERR_E_RETOUR_DANS_FONCTION, EX 15 ; MEMENTO82 ; constat d'Éric sur LSE 8086 version IV,
         2026-10-23 ; LSE15 §7-5 ne prévoit l'abandon de l'expression que pour RETOUR EN)
   SI cible_retour_en fournie ET aucune ligne ne porte ce numéro → erreur d'exécution (EX 75)
   { ces erreurs sont signalées dans le cadre de la procédure, avant toute restauration }
2. SI a_resultat
       retirer resultat_eventuel du sommet et le rendre indépendant des cases locales : une
       référence est déréférencée, une chaîne qui n'est pas propre au résultat est copiée ; un
       tableau entier est une erreur (ERR_E_RESULTAT_TABLEAU, EX 59)
3. POUR chaque entrée de sauvegardes du cadre courant, dans l'ordre inverse de son empilement :
       rendre à la réserve la case courante de entrée.symbole, l'objet qu'elle désigne étant libéré
       case du nom entrée.symbole ← entrée.ancienne_cellule                         { IMP-0149, D1 }
4. SI cible_retour_en fournie { RETOUR EN, sortie extraordinaire }
       position d'exécution ← cible_retour_en, dans le CONTEXTE DU CADRE APPELANT
       { la ligne cible de RETOUR EN se situe nécessairement dans le corps du programme ou de la
         procédure appelante, jamais dans le corps de la procédure qui retourne, LSE15 §7-5 }
   SINON
       position d'exécution ← cadre_courant.numero_ligne_retour, cadre_courant.offset_retour
5. cadre courant ← cadre_courant.appelant
   SI cible_retour_en fournie
       DESIMBRIQUER(cible_retour_en) sur la pile de boucles du cadre désormais courant (§6.2 ;
       LSE15 §7-5, Note : les règles de sortie extraordinaire de la boucle FAIRE s'appliquent à
       ce branchement implicite)
   SI a_resultat { position_appel = 0x01, seul cas restant après l'étape 1 bis }
       déposer resultat_eventuel sur la pile d'évaluation du cadre appelant (forme VALEUR)
6. SI cadre_courant.position_appel = 0x01 (appel en position d'expression) ET cible_retour_en fournie
       { sortie par RETOUR EN depuis un appel en position d'expression — le RETOUR simple y est une
         erreur (étape 1 bis) : LSE15 §7-5 — « l'évaluation de l'expression est interrompue, et la
         variable à laquelle la valeur devait être affectée n'est pas changée » }
       abandonner l'exécution de l'instruction englobante du cadre appelant (§7.3, désimbrication)
```

Les étapes 1 bis et 6 illustrent directement l'usage de l'indicateur de position d'appel introduit par RCI-M IMP-0121 : sans cette information, portée par le cadre depuis l'étape 4 de l'appel, l'interpréteur ne pourrait distinguer ni un retour ordinaire (branchement simple, position instruction) d'un abandon complet de l'expression englobante (`RETOUR EN`, position expression, LSE15 §7-5), ni les deux sorties contraires à la position d'appel, que MEMENTO82 et LSE 8086 version IV traitent comme des erreurs. L'algorithme exact de cet abandon — remonter jusqu'à la fin de l'instruction qui contenait l'appel, sans exécuter les opérations qui restaient à consommer les valeurs déjà empilées pour cette expression — est développé en §7.3, en lien direct avec la désimbrication des contextes de boucle éventuellement actifs dans le cadre appelant.

**IMP-0150** — *Cadres d'activation : chaîne de blocs `malloc`, jamais gérée par le gestionnaire d'objets*
- Sujet : les cadres d'activation successifs doivent-ils être alloués dans un bloc contigu unique (susceptible de nécessiter compaction) ou par blocs individuels chaînés ?
- Choix : chaque `CadreActivation` est un bloc `malloc` individuel ; la création d'un cadre alloue un nouveau bloc et l'insère en tête de chaîne (`appelant` pointe vers l'ancien sommet) ; le retour désalloue le cadre courant (ainsi que ses tableaux `sauvegardes`, `alias`, `pile_boucles`) après restauration et fait redevenir courant le cadre `appelant`. Aucun handle compactable, par le même raisonnement que pour la pile d'évaluation (§4.3, IMP-0145) : un cadre n'est jamais référencé que par le cadre qu'il appelle (son unique successeur dans la chaîne) et par la variable « cadre courant » elle-même — jamais par RCI-M ni par la table des symboles.
- Alternatives envisagées : tableau contigu de cadres, réalloué par doublement (rejeté : un cadre contient lui-même des pointeurs internes vers ses propres tableaux `sauvegardes`/`alias`/`pile_boucles` ; un déplacement du tableau de cadres par `realloc` invaliderait ces pointeurs, sauf à les recalculer systématiquement à chaque redimensionnement — complexité inutile face au chaînage individuel, qui ne bouge jamais un cadre déjà créé) ; allocation via le gestionnaire d'objets (rejeté : même argument que IMP-0145, absence de besoin de compaction pour une structure strictement LIFO et jamais référencée de l'extérieur).
- Justification : cohérent avec Architecture physique §6.1 (IMP-0132, deux familles d'allocation) — un cadre d'activation est une structure de contrôle à durée de vie bornée par l'appel qui l'a créé, exactement la catégorie que ce chapitre réserve à `malloc`/`free` plutôt qu'au Memory Manager toolbox.
- Impact mémoire : un bloc par activation, libéré immédiatement au retour — aucune accumulation au-delà de la profondeur d'appel réellement atteinte.
- Impact disque : sans objet à ce stade.
- Impact performances : création/destruction d'un cadre en O(1) (allocation/libération simples), sans recopie d'un tableau contigu.
- Impact historique : neutre.
- Références normatives : Architecture physique §6.1, IMP-0132 ; §2 du présent chapitre.
- Statut : retenu.
- Complément du 2026-10-30 (Éric ; audit du code, AP-09) : les tableaux temporaires de la préparation d'un appel (valeurs des paramètres, reprises, cases neuves) sont statiques, de 125 entrées : l'élément `0xCD` d'une ligne de 255 octets (5 + 2n + 2m) borne à 125 les paramètres et les locaux. Seul le cadre est alloué ; `contexte_appeler` n'est jamais réentré.

---

## 6. Piles de contextes de boucle `FAIRE`

### 6.1 Contexte de boucle et exécution des trois éléments FAIRE

RCI-M §4.8.2 (IMP-0112) compile l'instruction FAIRE en trois éléments de contrôle portés par la seule ligne FAIRE, dans l'ordre d'évaluation prescrit par LSE15 §5-5.3 : `[n1] [v1] OUVERTURE 0xC9 (V) [p] PAS (0xCA explicite ou 0xCE implicite) [v2 ou condition] TEST (0xCF JUSQUA ou 0xD0 TANT QUE)`. RCI-M renvoie au présent chapitre le contenu du contexte de boucle empilé par l'ouverture, la reprise en fin d'itération et la désimbrication. Le présent paragraphe fixe le contexte et l'effet de chacun des trois éléments ; le §6.2 fixe la fin d'itération, la sortie ordinaire et la désimbrication.

**IMP-0151** — *Contexte de boucle `FAIRE` : variable contrôlée, dernier pas, phase, point de reprise et ligne de fin effective*
- Sujet : représentation physique d'un contexte de boucle empilé par l'élément d'ouverture RCI-M 0xC9, et effet à l'exécution des éléments d'ouverture, de pas et de test.
- Choix :

```c
typedef unsigned char PhaseBoucle;
#define BOUCLE_INITIALE   0   /* entre l'ouverture et le premier test */
#define BOUCLE_ITERATION  1   /* après au moins une reprise de fin d'itération */

typedef struct {
    unsigned short   cellule_V;        /* case de la variable contrôlée V, variable arithmétique
                                           simple (LSE15 §5-5.1), résolue à l'ouverture (D1) ;
                                           CASE_AUCUNE après l'ouverture 0xD2 (RCI-M IMP-0277) */
    float            pas;              /* dernier pas évalué : valeur consommée par l'élément 0xCA,
                                           ou 1 pour l'élément 0xCE ; son signe gouverne le test
                                           JUSQUA (LSE15 §5-5.3) */
    PhaseBoucle      phase;
    unsigned char    ligne_faire;      /* IMP-0125 : ligne portant l'instruction FAIRE */
    unsigned int     decalage_reprise; /* offset, dans la séquence de ligne_faire, de l'élément qui
                                           suit immédiatement l'ouverture 0xC9 : premier élément de la
                                           sous-séquence du pas, ou élément 0xCE si le pas est omis */
    unsigned char    ligne_fin;        /* ligne de fin effective de la boucle (ci-dessous) */
} ContexteBoucle;
```

  Ni la limite `v2`, ni la condition TANT QUE, ni le pas ne sont conservés comme données à réutiliser : ils sont réévalués à chaque itération par la réexécution de leurs sous-séquences compilées, qui suivent l'ouverture dans la ligne FAIRE (RCI-M IMP-0112 ; LSE15 §5-5.3). Le champ `pas` ne sert qu'à transmettre, de l'élément de pas à l'élément de test qui le suit, la valeur qui vient d'être évaluée.

  Effet des éléments :

```text
OUVRIR(n1, v1)                                   { élément 0xC9, ligne_faire = ligne en cours }
1. dépiler v1 puis n1 (déréférencement implicite, §4.1)
2. n ← n1 arrondi à l'entier le plus voisin       { LSE15 §5-5.1 ; conversion en entier, §3 bis }
   SI n < 1            → erreur d'exécution        { « ε1 doit être strictement positive » }
   SI n < ligne_faire  → erreur d'exécution        { « ce numéro ne doit donc pas être
                                                     inférieur à celui de la ligne où se trouve
                                                     l'instruction FAIRE » }
3. ligne_fin ← plus grand numéro de ligne existant dans le programme courant qui ne dépasse
               pas n (recherche dichotomique, RCI-M IMP-0106) ; il existe toujours, puisque
               ligne_faire ≤ n                    { LSE15 §5-5.1 : « S'il n'y a pas de ligne
                                                     portant le numéro ε1, la boucle s'arrête à la
                                                     ligne dont le numéro est le plus proche de ε1
                                                     par valeurs inférieures » }
4. V ← v1                                          { sémantique de l'affectation, RCI-M 0x99 }
5. SI le sommet de la pile de boucles du cadre courant a la même ligne_faire et le même
      decalage_reprise                             { nouvelle exécution de la même ouverture :
                                                     branchement vers la ligne FAIRE, LSE15
                                                     §5-5.4, Remarque 2 }
       réinitialiser ce contexte en place
   SINON
       empiler un nouveau contexte
   contexte ← { cellule_V, pas : non significatif, phase : BOUCLE_INITIALE, ligne_faire,
                decalage_reprise : position de l'élément qui suit l'ouverture, ligne_fin }
6. poursuivre à l'élément suivant

OUVRIR_SEUL(n1)                                  { élément 0xD2, LSE F4, RCI-M IMP-0277 }
   comme OUVRIR, sans v1 ni étape 4 ; cellule_V ← CASE_AUCUNE ; aucun élément de pas ne suit

PAS(p)                                             { élément 0xCA : p dépilé ; 0xCE : p = 1 }
1. sommet.pas ← p
2. SI sommet.phase = BOUCLE_ITERATION
       V ← valeur courante de V + p                { LSE15 §5-5.3 ; V peut avoir été modifiée
                                                     dans le corps, « la variable contrôlée est une
                                                     variable comme une autre », Remarque 1 }
3. poursuivre à l'élément suivant

TESTER                                             { élément 0xCF : v2 dépilé ; 0xD0 : c dépilé }
1. JUSQUA  : sortie ← (sommet.pas > 0 ET V > v2) OU (sommet.pas < 0 ET V < v2)
   TANT QUE: sortie ← NON c
2. SI sortie  SORTIE_ORDINAIRE(sommet)            { §6.2 }
   SINON      poursuivre à l'élément suivant, premier élément du corps
```

  **Pas nul.** Le corpus ne prévoit pas le cas d'un pas nul. La règle de sortie de l'option JUSQUA est appliquée littéralement (« si V > ε4 (et que le pas est positif), ou V < ε4 (et que le pas est négatif) », LSE15 §5-5.3) : avec un pas nul, le test JUSQUA ne provoque jamais la sortie, qui ne peut alors résulter que d'une sortie extraordinaire. Ce comportement n'est pas attesté par le corpus ; il est signalé comme tel (§12).
- Alternatives envisagées : conserver dans le contexte la limite `v2` et le pas évalués une fois pour toutes à l'ouverture (rejeté : contredit LSE15 §5-5.3, « les expressions arithmétiques correspondant au pas (ε3), et à la limite (ε4) sont évaluées à la fin de chaque boucle ») ; conserver l'offset de la sous-séquence de la condition TANT QUE pour la réévaluer séparément (rejeté : inutile, la reprise juste après l'ouverture réexécute dans l'ordre le pas, la limite ou la condition et le test, RCI-M IMP-0112) ; comparer la ligne exécutée à `n1` lui-même plutôt qu'à une ligne de fin effective (rejeté : contredit LSE15 §5-5.1 lorsque la ligne `n1` n'existe pas) ; empiler systématiquement un nouveau contexte à chaque exécution de l'ouverture (rejeté : un branchement répété vers la ligne FAIRE depuis le corps de la boucle accumulerait des contextes identiques, alors que LSE15 §5-5.4, Remarque 2, décrit une réinitialisation de la même boucle).
- Justification : le contexte ne conserve que ce que les éléments compilés ne peuvent pas redonner — l'identité de `V`, le dernier pas, la phase, le point de reprise et la ligne de fin effective. Tout ce que le corpus fait réévaluer l'est par la réexécution des sous-séquences compilées, dans l'ordre prescrit.
- Impact mémoire : taille fixe d'environ 11 octets par contexte (aucun bourrage sous ORCA/C, RCI-M §2) ; un programme ne comporte en pratique qu'un faible niveau d'imbrication de boucles simultanément actives.
- Impact disque : sans objet (état d'exécution).
- Impact performances : accès au contexte actif (sommet de `CadreActivation.pile_boucles`) en O(1) ; une recherche dichotomique par ouverture pour la ligne de fin effective.
- Impact historique : couvre l'évaluation unique de `n1` et `v1`, la réévaluation du pas et de la limite (signe du pas compris), la valeur de `V` à la sortie (« la valeur qui a fait jouer le test de sortie »), la ligne de fin la plus proche par valeurs inférieures et la réinitialisation par branchement vers l'instruction FAIRE.
- Références normatives : RCI-M §4.8.2 (IMP-0112), §9 (`CurseurSequence`), IMP-0104, IMP-0106, IMP-0125 ; LSE15 §5-5.1, §5-5.3 (et Remarque 1), §5-5.4 (Remarque 2) ; INRDP75, §4-1, §4-2.
- Statut : retenu ; révisé le 2026-10-23 : le contexte désigne la case de la variable contrôlée (point D1) ; complété le 2026-11-01 : boucle sans variable, sous `LSE F4` (RCI-M IMP-0277).

### 6.2 Fin d'itération, sortie ordinaire et désimbrication

Une boucle se termine à sa ligne de fin effective ; elle est quittée soit par une sortie ordinaire (test de l'élément 0xCF ou 0xD0), soit par une sortie extraordinaire (branchement vers une ligne extérieure à la boucle, LSE15 §5-5.4 ; `RETOUR EN` depuis une procédure appelée dans la boucle, LSE15 §7-5, Note). Le corpus n'atteste aucune instruction dédiée de sortie de boucle.

**IMP-0152** — *Fin d'itération à l'achèvement de la ligne de fin, sortie ordinaire propagée aux boucles de même ligne de fin, désimbrication par l'intervalle `[ligne_faire, ligne_fin]`*
- Sujet : quand l'interpréteur reprend-il la boucle pour une nouvelle itération, que fait la sortie ordinaire, et comment dépile-t-il les contextes des boucles quittées par un branchement ?
- Choix : trois procédures, appliquées au contexte au sommet de la pile de boucles du cadre courant.

```text
FIN_DE_LIGNE(L)
{ appelée lorsque la séquence de la ligne L est épuisée — quelle que soit la façon dont son
  dernier élément a été atteint, y compris par un branchement intra-séquence 0xC0/0xC1 —, et par
  SORTIE_ORDINAIRE ; jamais lorsque la ligne est quittée par ALLER EN, RETOUR EN ou un appel }
1. SI la pile de boucles n'est pas vide ET sommet.ligne_fin = L
       sommet.phase ← BOUCLE_ITERATION
       position ← (sommet.ligne_faire, sommet.decalage_reprise)   { élément de pas, §6.1 }
       RETOURNER
2. SI aucune ligne de numéro supérieur à L n'existe → erreur d'exécution (IMP-0154)
   position ← début de la première ligne de numéro supérieur à L
   DESIMBRIQUER(numéro de cette ligne)

SORTIE_ORDINAIRE(contexte)                         { élément de test, sortie, §6.1 }
1. L ← contexte.ligne_fin ; dépiler le contexte
2. FIN_DE_LIGNE(L)                                 { une boucle englobante de même ligne de fin
                                                     passe à son itération suivante ; sinon
                                                     l'exécution va à la première ligne qui suit
                                                     la boucle, LSE15 §5-5.3 }

DESIMBRIQUER(N)
{ appelée lorsque la position d'exécution est portée au début de la ligne N par progression
  (FIN_DE_LIGNE, étape 2) ou par un branchement extra-ligne (ALLER EN, 0xC2/0xC3), et, dans le
  cadre appelant, après un RETOUR EN (§5.5) }
TANT QUE la pile de boucles n'est pas vide
         ET (N < sommet.ligne_faire OU N > sommet.ligne_fin)
    dépiler le sommet                              { sortie extraordinaire : ni pas, ni test }
```

  Une ligne `N` comprise dans `[ligne_faire, ligne_fin]` laisse le contexte actif. Si `N` est la ligne de fin, la boucle reprendra à l'achèvement de cette ligne (cas de la « ligne vide » de fin de boucle, LSE15 §5-5.3, Remarque 2). Si `N` est la ligne FAIRE, l'exécution de cette ligne depuis son début réexécute l'ouverture, qui réinitialise le contexte en place (§6.1, étape 5).

  **Boucles de même ligne de fin.** LSE15 §5-5.5 atteste des boucles imbriquées terminées par la même ligne (`100 FAIRE 104 POUR I←1 JUSQUA N` / `102 FAIRE 104 POUR J←1 JUSQUA N` / `104 A[I,J]←0`, ainsi que `100 FAIRE 100 POUR I←1 JUSQUA N; FAIRE 100 POUR J←1 JUSQUA N;A[I,J]←0`). La sortie ordinaire de la boucle intérieure ne doit pas faire quitter la boucle extérieure : elle signifie que la ligne de fin commune est achevée pour la boucle extérieure, qui passe à son itération suivante. C'est ce que réalise l'appel de `FIN_DE_LIGNE` par `SORTIE_ORDINAIRE`. Une sortie dès le premier test (boucle exécutée zéro fois) a le même effet, l'exécution allant « à la première ligne qui suit la boucle » (LSE15 §5-5.3).

  **Sortie par branchement depuis la ligne de fin.** Lorsque la ligne de fin est quittée par un branchement extra-ligne, la boucle ne reprend pas : `FIN_DE_LIGNE` n'est pas appelée et `DESIMBRIQUER` s'applique à la ligne cible. C'est l'exemple de LSE15 §5-5.4 : `1 FAIRE 3 POUR I←1 JUSQUA N` / `3 SI A[I]=0 ALORS ALLER EN 20` va en ligne 20 avec `I` égal à l'indice de l'élément nul, alors que, si la condition est fausse, la ligne 3 s'achève et la boucle reprend.

  **Retour de procédure.** La pile de contextes de boucle étant propre au cadre d'activation (§5.1, §3), elle est détruite avec le cadre à l'étape 5 de `RETOURNER` (§5.5) : toute boucle encore active dans une procédure qui retourne est abandonnée. Un `RETOUR EN` applique ensuite `DESIMBRIQUER` à la ligne cible dans le cadre appelant, les règles de sortie extraordinaire de la boucle s'appliquant à ce « branchement implicite » (LSE15 §7-5, Note).
- Alternatives envisagées : déclencher la fin d'itération à l'arrivée sur la ligne de fin plutôt qu'à son achèvement (rejeté : la ligne de fin fait partie de la boucle, « ces deux lignes extrêmes comprises », INRDP75 §4-1, et doit être exécutée avant l'ajout du pas) ; faire aller la sortie ordinaire directement à la première ligne qui suit la ligne de fin, sans réexaminer la pile (rejeté : la désimbrication dépilerait alors la boucle extérieure de même ligne de fin, contrairement aux exemples de LSE15 §5-5.5) ; ne comparer la ligne cible qu'à la borne supérieure de la boucle (rejeté : un branchement vers une ligne antérieure à la ligne FAIRE sort tout autant de la boucle, « ces deux lignes extrêmes comprises », et un contexte laissé actif fausserait un passage ultérieur par sa ligne de fin) ; ajouter une vérification de désimbrication dédiée à chaque instruction de contrôle (rejeté : redondant avec le test unique appliqué à toute arrivée au début d'une ligne).
- Justification : la boucle est délimitée sur ses deux bords par le corpus ; la fin d'itération suit l'achèvement de sa dernière ligne ; la sortie ordinaire et la sortie extraordinaire sont distinguées comme le fait LSE15 §5-5.4. Les trois procédures n'utilisent que le numéro de la ligne et les champs du contexte (§6.1), jamais une marque physique portée par la ligne de fin (EXI-0001).
- Impact mémoire : aucune structure supplémentaire ; la désimbrication maintient la pile bornée par le niveau d'imbrication réellement actif.
- Impact disque : sans objet.
- Impact performances : une comparaison entière à l'achèvement de chaque ligne ; deux comparaisons par niveau dépilé à l'arrivée au début d'une ligne.
- Impact historique : couvre la fin d'itération, la sortie ordinaire, la sortie extraordinaire par l'un ou l'autre bord, les boucles imbriquées de même ligne de fin et la sortie par `RETOUR EN`, conformément à LSE15 §5-5.3 à §5-5.5 et §7-5 ; aucun comportement nouveau n'est introduit au niveau du langage.
- Références normatives : RCI-M §4.8.2 (IMP-0112) ; LSE15 §5-5.3 (et Remarque 2), §5-5.4, §5-5.5, §7-5 (Note) ; INRDP75, §4-1, §4-4 ; EXI-0001 ; §5.1, §5.5, §6.1 du présent chapitre ; IMP-0154.
- Statut : retenu.

### 6.3 Intégration dans le cycle d'interprétation

Les éléments 0xC9, 0xCA, 0xCE, 0xCF et 0xD0 sont traités par le cycle d'interprétation (§7.2) selon `OUVRIR`, `PAS` et `TESTER` (§6.1). `FIN_DE_LIGNE` remplace, dans ce cycle, le simple passage à la ligne suivante lorsque la séquence d'une ligne est épuisée ; `DESIMBRIQUER` suit tout branchement extra-ligne et tout `RETOUR EN`. Aucun de ces tests n'intervient à l'intérieur d'une ligne : les branchements intra-séquence (0xC0/0xC1) ne déplacent la position qu'à l'intérieur de la ligne en cours.

---

## 7. Algorithmes d'exécution — réalisation physique

### 7.1 Initialisation (réalisation d'ALG-1101)

Le corpus (LSE15 §9-2) documente deux commandes distinctes du moniteur qui déclenchent une exécution programmée : `EXECUTER A PARTIR DE n1{,n2}`, qui « fait passer la console de l'état "moniteur" à l'état "exécution" » à partir de la ligne `n1` (« le numéro de la ligne où il faut commencer l'exécution »), et jusqu'à la ligne optionnelle `n2` (« le numéro de la ligne où il faut arrêter l'exécution » ; à défaut, l'exécution se poursuit « jusqu'à ce que l'ordinateur rencontre une instruction `PAUSE` ou `TERMINER`, ou qu'il détecte une erreur ») ; et sa forme sans paramètre, `EXECUTER`, qui « lance le programme en mettant ou en remettant ses identificateurs dans l'état "non-défini"; aucune information n'est... conservée sur l'état précédent du programme » — ce qui revient à démarrer à la première ligne du programme courant. ALG-1101 (Tome IV §11.1, étape 3 : « positionner le point de départ ») anticipe déjà cette latitude ; le présent paragraphe la rend physique.

```text
INITIALISER(programme_courant, ligne_depart, ligne_arret)
{ ligne_depart : optionnelle ; absente ⇒ première ligne du programme, EXI-0023 }
{ ligne_arret  : optionnelle ; absente ⇒ aucun arrêt programmé (seuls PAUSE/TERMINER/erreur/fin
                  anormale de programme arrêtent l'exécution, §7.2) }
1. SI ligne_depart fournie
       vérifier qu'une ligne portant ce numéro existe dans programme_courant (recherche
       dichotomique, RCI-M IMP-0106) ; sinon, erreur (LSE15 §9-2 : « s'il n'y a pas de ligne
       portant le numéro n1, une erreur sera détectée »)
   SINON
       vérifier que programme_courant contient au moins un objet compilé (EXI-0023 : l'exécution
       programmée sans point de reprise ni transfert explicite débute au premier objet logique)
2. réinitialiser l'environnement des identificateurs (§3 bis, IMP-0179) : libérer tout objet
   chaîne ou tableau qu'il désigne, puis remettre toutes ses entrées à ID_NON_DEFINI
   (LSE15 §9-2 : « en mettant ou en remettant ses identificateurs dans l'état non-défini »)
3. créer l'état d'exécution :
     registre d'état ← PRÊTE
     pile d'évaluation ← vide (§4.3)
     ligne_arret_courante ← ligne_arret (mémorisée pour le test du §7.2 ; non significative si absente)
4. créer le cadre d'activation racine :
     appelant ← NULL
     numero_ligne_retour, offset_retour ← non significatifs (aucun retour possible depuis ce cadre)
     position_appel ← non significatif
     numero_ligne_procedure ← non significatif { ce cadre ne correspond à aucune PROCEDURE déclarée ;
                                                    il existe uniquement pour porter la pile de
                                                    contextes de boucle du programme principal, §3 }
     sauvegardes, alias ← vides
     pile_boucles ← vide
5. cadre courant ← cadre racine
6. position d'exécution ← ligne_depart si fournie, sinon le premier objet compilé de
   programme_courant selon l'ordre logique des numéros de ligne (recherche dichotomique,
   RCI-M IMP-0106, sur le plus petit numero_ligne présent)
7. registre d'état ← ACTIVE
```

Le cadre racine, introduit ici, réalise la remarque de §3 : la pile de contextes de boucle `FAIRE` étant rattachée au cadre d'activation courant (§5.1, §6.1) plutôt qu'à une pile globale, une boucle `FAIRE` écrite au niveau du programme principal (hors de toute procédure) a besoin d'un cadre pour porter sa propre pile de contextes, même en l'absence de tout appel de procédure. Ce cadre racine n'est jamais détruit avant la terminaison de l'exécution (§7.2, TERMINER) et ne peut faire l'objet d'aucun `RETOUR`/`RETOUR EN`/`RESULTAT` (§5.5, étape 1 : erreur d'exécution).

**IMP-0153** — *Point de départ et point d'arrêt programmables, portés par l'état d'exécution*
- Sujet : `INITIALISER` doit-elle toujours démarrer au premier objet compilé du programme courant, ou doit-elle accepter un point de départ et un point d'arrêt explicites, conformément à la commande `EXECUTER A PARTIR DE n1{,n2}` ?
- Choix : `INITIALISER` accepte deux paramètres optionnels, `ligne_depart` et `ligne_arret`, directement issus des paramètres `n1`/`n2` de la commande du moniteur ; à défaut de `ligne_depart`, le comportement par défaut déjà décrit en §7.1 (première ligne) s'applique, conformément à EXI-0023 (« lorsqu'une exécution programmée commence **sans point de reprise ni transfert explicite**, elle débute au premier objet compilé »), qui anticipe explicitement l'existence d'un tel transfert explicite comme alternative au cas par défaut qu'elle décrit. `ligne_arret`, lorsqu'elle est fournie, est mémorisée dans l'état d'exécution et consultée par la boucle d'interprétation (§7.2) pour un arrêt programmé distinct de `PAUSE`/`TERMINER`.
- Alternatives envisagées : traiter `EXECUTER A PARTIR DE n1` comme un simple enchaînement de `INITIALISER()` (démarrage normal) suivi d'un branchement `ALLER EN n1` (rejeté : réinitialiserait le programme courant en exécutant effectivement sa toute première ligne avant de brancher vers `n1`, ce qui contredit LSE15 §9-2 — la ligne `n1` est la première effectivement exécutée, aucune ligne antérieure ne l'est) ; ignorer `n2` et laisser l'utilisateur insérer manuellement une instruction `PAUSE` à la ligne voulue (rejeté : `n2` est un paramètre de la commande elle-même, distinct et indépendant du texte du programme — l'exiger reviendrait à modifier silencieusement le programme courant pour satisfaire une simple commande de mise au point, contraire au conditionnement §19).
- Justification : ce choix réalise directement l'étape 3 d'ALG-1101 (Tome IV §11.1, « positionner le point de départ »), restée jusqu'ici non détaillée par ce chapitre, et couvre fidèlement la commande `EXECUTER A PARTIR DE` documentée par LSE15 §9-2, y compris son cas d'erreur (`n1` inexistant) et son comportement par défaut sur `n2` (absence de `n2`, ou `n2` jamais atteint : poursuite jusqu'à `PAUSE`/`TERMINER`/erreur, §7.2).
- Impact mémoire : un champ supplémentaire (`ligne_arret_courante`, 1 octet, IMP-0125) dans l'état d'exécution.
- Impact disque : sans objet.
- Impact performances : une comparaison entière supplémentaire par changement de ligne (§7.2), négligeable.
- Impact historique : couvre fidèlement `EXECUTER A PARTIR DE n1{,n2}` (LSE15 §9-2), y compris la réinitialisation des identificateurs à l'état non défini propre à la commande `EXECUTER` (étape 2 ci-dessus).
- Références normatives : LSE15 §9-2 (« EXECUTER A PARTIR DE ») ; Tome IV §11.1 (ALG-1101, étape 3) ; EXI-0023, EXI-0029.
- Statut : retenu.

**Point apparenté, non détaillé ici.** La commande `REPRENDRE EN n1{,n2}` (LSE15 §9-2) partage la même signature de paramètres que `EXECUTER A PARTIR DE`, mais « ne change pas l'affectation des identificateurs... ni leur valeur » et reprend « toujours... au niveau principal (même si le programme avait été interrompu dans une procédure) » — un comportement distinct à la fois de `INITIALISER` (qui réinitialise les identificateurs, étape 2 ci-dessus) et de `CONTINUER`/PAS À PAS (§7.4, qui reprend exactement où l'exécution avait été arrêtée, y compris dans une procédure). Sa réalisation physique précise — reprise à une ligne donnée tout en forçant le cadre courant au cadre racine sans détruire les valeurs de variables — n'est pas détaillée par le présent chapitre (cf. §12, limite explicite).

**Réalisation retenue le 2026-10-24 (chapitre Moniteur et console, IMP-0232).** `contexte_reprendre(n1, n2, &pos)` vérifie que la ligne `n1` existe (sinon `ERR_E_LIGNE_INEXISTANTE`, que le moniteur restitue par `????`), abandonne les cadres des procédures jusqu'au cadre racine en restaurant leurs identificateurs `LOCAL` (§5.4), oublie les contextes de boucle du cadre racine, fixe la ligne d'arrêt `n2` et se positionne au début de la ligne `n1` ; l'environnement des identificateurs et les valeurs sont conservés ; le moniteur vide la pile d'évaluation. `contexte_fixer_arret(n)` change la ligne d'arrêt (`POURSUIVRE JUSQU'EN n`).

### 7.2 Interprétation (réalisation d'ALG-1102)

```text
TANT QUE registre d'état = ACTIVE
    élément ← lire l'élément courant à la position d'exécution (CurseurSequence, RCI-M §9)
    SI catégorie(élément) = VALEUR
        pile_empiler(pile, forme VALEUR à partir de la charge utile décodée, §4.2)
    SINON SI catégorie(élément) = RÉFÉRENCE
        cible ← RESOUDRE(symbole, index éventuels, §5.3)
        pile_empiler(pile, forme REFERENCE = cible)
    SINON SI catégorie(élément) = OPÉRATION CODÉE
        exécuter l'opération (§4.11 RCI-M — déréférencement implicite des opérandes REFERENCE
        consommés en tant que valeurs, §4.1 ; consommation directe des opérandes REFERENCE exigés
        en tant que cible, §4.1)
    SINON SI catégorie(élément) = INFORMATION DE CONTRÔLE
        SI élément = 0xC9 { ouverture de boucle FAIRE }
            OUVRIR (§6.1)
        SINON SI élément ∈ {0xCA, 0xCE} { pas de boucle FAIRE, explicite ou implicite }
            PAS (§6.1)
        SINON SI élément ∈ {0xCF, 0xD0} { test de boucle FAIRE, JUSQUA ou TANT QUE }
            TESTER (§6.1) — en cas de sortie, SORTIE_ORDINAIRE (§6.2)
        SINON SI élément = 0xA8 { APPEL : opération codée, RCI-M IMP-0121, traitée ici avec le contrôle }
            APPELER(...) selon §5.5
        SINON SI élément ∈ {0xC4, 0xC5/0xC6, 0xCB} { RETOUR, RETOUR EN, RESULTAT }
            RETOURNER(...) selon §5.5 et §7.3
        SINON SI élément ∈ {0xC0, 0xC1} { branchements intra-séquence }
            position d'exécution ← cible, dans la ligne en cours (RCI-M §4.7, IMP-0105)
        SINON SI élément ∈ {0xC2, 0xC3} { ALLER EN }
            position d'exécution ← début de la ligne cible (RCI-M §4.7, IMP-0111)
            DESIMBRIQUER(ligne cible) (§6.2)
        SINON SI élément = 0xC7 { TERMINER }
            registre d'état ← TERMINÉE ; SORTIR
        SINON SI élément = 0xC8 { PAUSE }
            registre d'état ← SUSPENDUE ; SORTIR (§7.4)
        SINON SI élément = 0xCD { DÉCLARATION DE PROCÉDURE, Compilateur IMP-0174 }
            { jamais atteint par un appel, qui positionne l'exécution après cet élément
              (§5.5, étape 7) : ligne de déclaration atteinte hors appel, IMP-0155 }
            erreur d'exécution
        FIN SI
    FIN SI

    SI registre d'état = ACTIVE ET la séquence de la ligne en cours est épuisée
        FIN_DE_LIGNE(ligne en cours) (§6.2)   { reprise d'itération d'une boucle FAIRE, ou passage
                                                au début de la ligne suivante suivi de
                                                DESIMBRIQUER ; erreur IMP-0154 s'il n'existe aucune
                                                ligne suivante }
    FIN SI

    SI la position d'exécution vient d'être portée au début d'une ligne (nouvelle_ligne)
        { progression, ALLER EN ou RETOUR EN ; non la reprise d'itération, qui se fait au milieu
          de la ligne FAIRE }
        SI ligne_arret_courante définie ET nouvelle_ligne = ligne_arret_courante
            { LSE15 §9-2 : « cette ligne ne sera pas exécutée et la console repassera dans
              l'état "moniteur", en imprimant ce numéro n2 » }
            registre d'état ← SUSPENDUE ; afficher nouvelle_ligne ; SORTIR
        FIN SI
    FIN SI
FIN TANT QUE
```

L'élément COMMENTAIRE (0xCC) et la marque de tête de LIRE (0xD1) n'apparaissent pas dans cette boucle : sans effet à l'exécution, le curseur les enjambe simplement (RCI-M IMP-0113, IMP-0183). Si une différence de comportement devait être établie entre `LIRE A,B` et `LIRE A;LIRE B`, c'est sur l'élément 0xD1 que l'interpréteur la réaliserait. L'élément DÉCLARATION DE PROCÉDURE (0xCD), à l'inverse, n'est jamais enjambé : sa rencontre par le cycle suffit à établir que la ligne de déclaration a été atteinte autrement que par un appel (IMP-0155).

Cette réalisation respecte l'invariant de Tome III §8.1 (« à chaque cycle... une position courante est définie ; une opération courante est déterminée... une terminaison interrompt le cycle ») et conserve, comme l'exige Tome III §6.7, la représentation compilée du programme courant strictement inchangée à chaque cycle : seules la pile d'évaluation, le cadre courant et son contenu (sauvegardes, alias, contextes de boucle) sont modifiés.

**IMP-0154** — *Dépassement de la dernière ligne sans terminaison explicite : erreur d'exécution*
- Sujet : que doit faire l'interpréteur lorsque, après avoir exécuté la ligne du plus grand numéro présent dans le programme courant (ou, plus généralement, après avoir exécuté une ligne à la suite de laquelle aucune ligne suivante n'existe dans la table des objets compilés, la position d'exécution avançant séquentiellement), aucune instruction `TERMINER`, `PAUSE`, ni ligne d'arrêt programmée (`ligne_arret`, IMP-0153) n'a mis fin à l'exécution ?
- Choix : ce cas constitue une **erreur d'exécution**, et non une terminaison normale silencieuse.
- Alternatives envisagées : terminer silencieusement l'exécution (registre d'état ← `TERMINÉE`), en traitant la fin de la table des objets compilés comme une fin de programme implicite (rejeté : LSE15 §9-2 énumère de façon exhaustive les conditions d'arrêt d'une exécution lancée sans `n2` — « jusqu'à ce que l'ordinateur rencontre une instruction `PAUSE` ou `TERMINER`, ou qu'il détecte une erreur » — sans mentionner l'épuisement des lignes du programme comme une quatrième condition ; une terminaison silencieuse dans ce cas masquerait typiquement l'oubli d'une instruction `TERMINER` en fin de programme, alors que le corpus atteste au contraire la convention inverse — placer explicitement `TERMINER` en dernière ligne, cf. l'exemple `3 TERMINER;*FIN DU PROGRAMME` — ce qui suggère que l'absence de cette instruction n'est jamais présumée intentionnelle) ; confondre ce cas avec un branchement vers une ligne inexistante sous une seule nature d'erreur (rejeté : MEMENTO82 les distingue, « le programme n'a pas de n° de ligne supérieure » (58) et « ligne de branchement inexistante » (75), et les deux situations appellent des corrections différentes, IMP-0187 ; arbitrage d'Éric du 2026-10-23, RX5).
- Justification : dans les deux cas — progression au-delà de la dernière ligne, branchement explicite vers un numéro absent — la position d'exécution devrait désigner une ligne qui n'existe pas ; les traiter l'un et l'autre comme des erreurs évite une exception silencieuse au principe selon lequel la position d'exécution désigne toujours une ligne existante. Ce sont deux natures distinctes du catalogue : `ERR_E_FIN_DE_PROGRAMME` (EX 58) et `ERR_E_LIGNE_INEXISTANTE` (EX 75) (Gestionnaire des erreurs §9).
- Impact mémoire : sans objet.
- Impact disque : sans objet.
- Impact performances : ce test se confond avec la recherche, déjà nécessaire à chaque avancement séquentiel de ligne, de la ligne suivante dans la table des objets compilés — aucun coût supplémentaire.
- Impact historique : ne contredit aucune disposition explicite du corpus (LSE15 §9-2 n'énumère pas ce cas comme une terminaison valide) ; reste cohérent avec la convention historique documentée de placer `TERMINER` en fin de programme.
- Références normatives : LSE15 §9-2 (« EXECUTER A PARTIR DE », énumération des conditions d'arrêt) ; LSE15 §8 (exemple `3 TERMINER;*FIN DU PROGRAMME`) ; §5-6.2 (« instruction TERMINER ») ; EXI-0030 (observations : la présente lecture retient que « atteindre la fin du programme » y désigne l'atteinte d'une ligne d'arrêt programmée, IMP-0153, plutôt que l'épuisement pur et simple des lignes existantes, ce dernier cas n'étant énuméré nulle part par LSE15 §9-2 parmi les arrêts valides).
- Statut : retenu.

**IMP-0155** — *Ligne de déclaration de procédure atteinte hors appel : erreur d'exécution, détectée par la rencontre de l'élément de déclaration*
- Sujet : que doit faire l'interpréteur lorsque l'exécution atteint une ligne de déclaration de procédure autrement que par un appel ? C'est le cas d'une progression séquentielle depuis la ligne précédente, d'un `ALLER EN` ou d'un `RETOUR EN` ciblant cette ligne, ou d'un `EXECUTER A PARTIR DE` qui la désigne. Le corpus l'interdit : la déclaration « ne peut jamais [s']exécuter directement, ou par l'intermédiaire d'un `ALLER EN` » (LSE15 §7-6) ; « l'instruction de déclaration d'une procédure n'est pas exécutable. En conséquence le programme ne doit pas rencontrer en cours d'exécution une telle instruction » (INRDP75, chapitre Procédures, §2, « Remarques importantes », 1). Il ne précise pas la réaction du système. Comment, en outre, détecter cette situation ?
- Choix : ce cas constitue une **erreur d'exécution**. L'exécution d'une ligne `PROCEDURE &ID(...) LOCAL ...` (et, plus généralement, du corps qui la suit) n'a de sens défini par le corpus que dans le contexte créé par `APPELER` (§5.5) : établissement d'un cadre d'activation et liaison des paramètres selon la table des paramètres formels (Compilateur §13.3, IMP-0147). Elle n'est jamais une simple instruction exécutée séquentiellement au fil du programme principal ou d'une autre procédure. La détection repose sur l'élément de déclaration (sous-code de contrôle 0xCD, Compilateur IMP-0174), premier élément de toute ligne de déclaration. `APPELER` positionne l'exécution sur l'élément qui le suit (§5.5, étape 7) ; le cycle d'interprétation ne rencontre donc cet élément que si la ligne a été atteinte autrement que par un appel, et sa rencontre déclenche l'erreur (§7.2). Le critère vaut pour toute ligne de déclaration, y compris une déclaration masquée par une déclaration de même nom et de plus grand numéro (LSE15 §7-6, Note), qui n'est la ligne d'entrée d'aucune `InfoProcedure`.
- Alternatives envisagées :
  1. Exécuter la ligne de déclaration comme une instruction sans effet et laisser l'exécution se poursuivre dans le corps comme du code ordinaire du niveau appelant — écarté. Le corps d'une procédure utilise typiquement, pour ses paramètres et ses identificateurs `LOCAL`, des noms employés par ailleurs dans le programme (LSE15 §7-8.2 : « le nom d'un paramètre effectif... peut être le même que celui d'un paramètre formel »), sans qu'aucune sauvegarde ou restauration (§5.4) ni aucun alias (§5.3) n'ait été établi en l'absence d'un véritable appel. Le comportement obtenu serait arbitraire et sans rapport avec la sémantique documentée d'une procédure. En outre, un `RESULTAT` ou un `RETOUR` rencontré dans ce contexte échouerait de toute façon, faute de cadre d'activation à quitter (§5.5, étape 1) : l'erreur ne serait que déplacée de quelques lignes.
  2. Ignorer silencieusement la ligne et sauter au-delà du corps de la procédure — écarté. Rien dans la représentation compilée ne délimite physiquement la fin du corps d'une procédure : seule une instruction `RETOUR`, `RETOUR EN` ou `RESULTAT` la termine sémantiquement. Un tel saut supposerait une analyse que RCI-M ne fournit pas et introduirait un mécanisme non documenté par le corpus.
  3. Détecter la situation à chaque changement de ligne, en comparant le nouveau numéro aux `numero_ligne_entree` des `InfoProcedure`, avec une exemption pour la ligne atteinte par l'appel qui vient de s'exécuter — écarté. Ce test ne couvre pas une déclaration masquée (LSE15 §7-6, Note), qui n'est la ligne d'entrée d'aucune `InfoProcedure`. Il coûte en outre une recherche à chaque changement de ligne et exige un indicateur d'exemption, alors que l'élément de déclaration porte déjà, en tête de la ligne, l'information nécessaire.
- Justification : ce choix découle directement de VM-1302 (Tomes I-II §13.2, EXI-0043 : « l'exécution d'un appel... crée une activation distincte correspondant à cet appel »), qui fait de la création du cadre d'activation la condition nécessaire d'une exécution valide du corps d'une procédure. Une simple chute séquentielle ou un branchement direct ne la satisfont jamais. Signaler l'erreur dès la ligne de déclaration, plutôt que de laisser l'exécution se poursuivre dans un état incohérent (cadre absent, paramètres non liés), respecte le principe de diagnostic au plus près du point de défaillance déjà retenu ailleurs dans ce chapitre (§5.5, étape 4, vérification de la forme d'un argument référence).
- Impact mémoire : sans objet ; aucune structure ni variable supplémentaire.
- Impact disque : sans objet.
- Impact performances : nul en exécution normale. Le test est celui du décodage du sous-code de l'élément courant, déjà effectué à chaque cycle (§7.2) ; aucune recherche n'est ajoutée au changement de ligne.
- Impact historique : réalise l'interdiction documentée par LSE15 §7-6 et par INRDP75 (« le programme ne doit pas rencontrer en cours d'exécution une telle instruction ») ; cette erreur rend visible, plutôt que silencieusement tolérée, une situation que le corpus ne prévoit jamais comme valide.
- Références normatives : LSE15 §7-6 (et Note) ; INRDP75, chapitre Procédures, §2, « Remarques importantes », 1 ; Tomes I-II §13.2 (VM-1302, EXI-0043) ; RCI-M §6.3 (IMP-0114) ; Compilateur §13 (IMP-0147, IMP-0174) ; §5.5, §7.2 du présent chapitre.
- Statut : retenu.

### 7.3 `RETOUR EN` en position d'expression : désimbrication combinée

Le cas signalé par LSE15 §7-5 (§5.5, étape 6) — un appel en position d'expression qui sort par `RETOUR EN` sans jamais exécuter `RESULTAT` ; le `RETOUR` simple y est une erreur (§5.5, étape 1 bis) — exige d'« interrompre l'évaluation de l'expression » et d'« annuler l'instruction qui l'utilisait ». Physiquement, ceci se traduit par :

1. le cadre appelant (redevenu courant à l'étape 5 de `RETOURNER`, §5.5) a nécessairement déposé sur la pile d'évaluation, avant l'élément `APPEL` qui vient de retourner, un certain nombre d'éléments déjà évalués appartenant à la même expression (opérandes d'un opérateur englobant, ou la cible d'affectation elle-même si l'appel figurait à droite d'une affectation) ;
2. l'abandon consiste à **dépiler tous les éléments de la pile d'évaluation qui appartiennent à l'expression compilée en cours** — c'est-à-dire tous les éléments empilés depuis le début de la sous-séquence postfixée de l'instruction actuellement en cours d'exécution sur la ligne du cadre appelant — puis à faire avancer la position d'exécution jusqu'à la fin de cette instruction (la même règle de convention déjà retenue pour la reprise après `SI`/`FAIRE`, §4.8.4/§4.8.2 RCI-M : l'instruction suivante, sur la même ligne après `;` ou sur la ligne suivante), **sans exécuter aucun des éléments intermédiaires restants de cette instruction** (notamment, aucune affectation n'a lieu si l'appel figurait en position de source d'une affectation).

Le repère du « début de la sous-séquence postfixée de l'instruction en cours » est la base du cadre appelant (`CadreActivation.hauteur_base`, §5.1) : entre deux instructions, la pile d'un cadre revient toujours à sa base, si bien que l'instruction appelante a commencé à cette hauteur. `RETOURNER` ramène donc la pile à la base du cadre appelant, chaînes transitoires libérées, puis l'exécution reprend à la ligne cible du `RETOUR EN` ; il n'y a pas à chercher la fin de l'instruction abandonnée (choix d'Éric du 2026-10-24, point B2 ; chapitre Interpréteur §2).

### 7.4 Compatibilité avec PAS À PAS et la suspension (VM-0803, ALG-1602)

Le registre d'état d'exécution (VM-3006, Tome III §7.6) et les structures définies aux §4 à §6 suffisent, sans extension, à réaliser la suspension requise par ALG-1602 (PAS À PAS) et par PAUSE (RCI-M §4.7, élément 0xC8) : suspendre une exécution (§7.2, transition vers `SUSPENDUE`) ne modifie ni ne libère ni la pile d'évaluation, ni la chaîne des cadres d'activation, ni les piles de contextes de boucle qu'ils portent — ces structures restent intactes, adressables depuis le dialogue (moniteur) pour observation (VM-0803, EXI-0033), et une reprise (`REPRENDRE`, ALG-1602 étape 8) se contente de faire repasser le registre d'état à `ACTIVE` et de relancer la boucle d'interprétation (§7.2) sans aucune reconstruction. Ceci réalise directement l'invariant Tomes I-II §8.1 (VM-0801) : « la suspension préserve l'état nécessaire à la reprise » — sans qu'aucune structure supplémentaire ne soit nécessaire au-delà de celles déjà définies par ce chapitre.

### 7.5 Interfaces C des modules `pile` et `contexte_exec`

**IMP-0208** — *Erreur rendue sous forme de nature, usage de lecture, frontière avec le module `donnees`*
- Sujet : comment les modules de l'état d'exécution signalent-ils une erreur à l'interpréteur ? Comment choisissent-ils entre les natures de la variable non définie (RX1) ? Que réalisent-ils des chaînes et des tableaux avant le chapitre `donnees` ?
- Choix :
  1. **Nature rendue (C2).** Toute fonction de `pile.c` et de `contexte_exec.c` susceptible d'échouer rend une `NatureErreur`, `ERR_AUCUNE` en cas de succès. Une fonction qui échoue ne modifie rien, sauf mention dans son en-tête. L'interpréteur ajoute le numéro de la ligne en cours pour former le `DiagnosticExecution` (Gestionnaire des erreurs §8), puisque lui seul connaît la position.
  2. **Usage de lecture (C3).** La lecture d'une valeur numérique, ou d'une chaîne, reçoit un usage : `USAGE_CALCUL`, `USAGE_AFFECTATION` (affectation ou affichage) ou `USAGE_COMPARAISON`. Il choisit la nature : EX 05, EX 62 ou EX 10 pour une variable non définie ; « chaîne ou tableau dans un calcul » (EX 05), « chaîne rangée dans une variable non chaîne » (EX 06) ou « comparaison d'un nombre et d'une chaîne » (EX 10) pour une valeur d'un autre type.
  3. **Frontière avec `donnees` (E5).**
     - Le contexte d'exécution manipule un objet chaîne ou tableau comme un `HandleObjet` opaque. Il le libère par `objets_liberer`, mais n'en connaît pas la représentation.
     - Trois opérations, déclarées dans `src/noyau/donnees.h`, lui suffisent : faire d'un texte de la pile l'objet propre d'une variable-chaîne (`donnees_chaine_vers_objet`), désigner le texte d'une variable-chaîne (`donnees_objet_vers_chaine`), copier un objet chaîne ou tableau pour un paramètre `LOCAL` (`donnees_copier`).
     - Le module `donnees` les réalise depuis l'étape 5a (chapitre Donnees, IMP-0213). À l'étape 4c, les tests les réalisaient par un bouchon, sans valeur normative, supprimé depuis.
     - L'accès à un élément de tableau (référence indexée) relève aussi de l'étape 5 : le contexte le signale comme erreur interne s'il le rencontre.
  4. **Fichiers et segments (E3).** La pile d'évaluation occupe `pile.c/.h`, segment `"interprete"` ; l'environnement, les cadres et les boucles occupent `contexte_exec.c/.h`, segment `"contexte"` (règle des 10 caractères, IMP-0197).
- Alternatives envisagées :
  1. Un diagnostic complet rendu par chaque fonction — écartée : le contexte ne connaît pas la ligne en cours, et l'interpréteur devrait de toute façon la renseigner.
  2. Trois fonctions de lecture distinctes, une par usage — écartée : elles ne diffèrent que par la nature rendue.
  3. Fixer dès maintenant la représentation des chaînes et des tableaux dans le contexte — écartée : elle relève du chapitre `donnees` (§12) ; le contexte n'en a pas besoin.
- Justification : choix d'Éric du 2026-10-23 (points C2, C3, E3, E5) ; IMP-0187 (nature fine portée entre composants) ; IMP-0195.
- Impact mémoire : nul.
- Impact disque : sans objet.
- Impact performances : un octet d'usage par lecture.
- Impact historique : réalise les distinctions de numéros de MEMENTO82 et d'INRDP75 (RX1).
- Références normatives : Gestionnaire des erreurs §8, §9 ; IMP-0179, IMP-0187, IMP-0195, IMP-0197 ; §3 bis, §4, §12.
- Statut : retenu (Éric, 2026-10-23).

---

## 8. Organisation mémoire physique (zones) — mise à jour

Ce paragraphe referme les deux points explicitement laissés ouverts par RCI-M §8 et Architecture physique §10 :

| Zone (RCI-M §8, rappel) | Contenu | Remarque de dimensionnement |
|---|---|---|
| Pile d'évaluation | `ElementPile[]`, §4.2 | tableau `malloc`/`realloc`, §4.3 (IMP-0145) ; taille proportionnelle à la profondeur d'imbrication d'expressions réellement atteinte |
| Environnement des identificateurs | `EntreeEnvironnement[]`, §3 bis (IMP-0179), indexé comme la table des symboles | tableau `malloc`/`realloc`, agrandi en même temps que la table des symboles ; 5 octets par nom distinct ; objets chaînes et tableaux alloués par le gestionnaire d'objets |
| Cadres d'activation, contextes de boucle | `CadreActivation` chaînés, chacun portant `sauvegardes`, `alias`, `pile_boucles` (§5.1, §6.1) | blocs `malloc` individuels, §5.5 (IMP-0150) ; taille proportionnelle à la profondeur d'appel réellement atteinte et au nombre de paramètres/locaux/boucles actives par activation |

Ces trois zones appartiennent, dans le plan de segmentation d'Architecture physique §5.2, au segment `"contexte"` (VM-2207, environnement d'exécution) pour l'environnement des identificateurs et les cadres (`contexte_exec.c/.h`), et au segment `"interprete"` (VM-2205/VM-2206) pour la pile d'évaluation (`pile.c/.h`) (IMP-0208, Organisation du dépôt §2.1).

---

## 9. Correspondance avec les invariants

| Invariant / exigence | Réalisation physique correspondante |
|---|---|
| EXI-0031 (état distinct du programme compilé) | Aucune structure de ce chapitre ne réside dans la zone RCI-M (§3) |
| EXI-0032 (résolution des références) | Algorithme `RESOUDRE`, §5.3 |
| EXI-0034 à EXI-0036 (cadre, chaînage, retour) | `CadreActivation` (§5.1), `RETOURNER` (§5.5) |
| EXI-0038 (identité stable des variables) | Un identifiant de symbole unique par nom, indexant à la fois la table des symboles et l'environnement des identificateurs (§3, §3 bis, §5.4) |
| HIS-12.3 (objet non initialisé) ; VM-1102 (état de déclaration, valeur courante) | Environnement des identificateurs et ses règles de transition (§3 bis, IMP-0179) |
| EXI-0001 (compilation indépendante des lignes) | Aucune structure de ce chapitre ne modifie RCI-M ; la corrélation de boucle `FAIRE` reste hors RCI-M (§6, prolongeant RCI-M IMP-0112) |
| Tome III §6.7 (une opération de contrôle ne modifie jamais le programme courant) | §7.2 : seuls l'état d'exécution (pile, cadre, position) sont modifiés à chaque cycle |
| Tome III §7.7 (le cadre courant est toujours le dernier actif) | Chaînage par pointeur `appelant`, jamais de structure indexée pouvant se désynchroniser (§5.1) |

---

## 10. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0144 | Élément de pile d'évaluation à deux formes (valeur/référence), déréférencement implicite | retenu |
| IMP-0145 | Pile d'évaluation : tableau `malloc`/`realloc`, jamais géré par le gestionnaire d'objets | retenu |
| IMP-0146 | Structure physique du cadre d'activation | retenu |
| IMP-0148 | Résolution eagère et à un seul niveau des alias de paramètres référence ; l'alias capture une case (D1) | retenu |
| IMP-0149 | Une case neuve pour chaque identificateur `LOCAL`, l'ancienne case du nom étant sauvegardée (D1) | retenu |
| IMP-0150 | Cadres d'activation : chaîne de blocs `malloc`, jamais gérée par le gestionnaire d'objets | retenu |
| IMP-0151 | Contexte de boucle `FAIRE` (variable contrôlée, dernier pas, phase, point de reprise, ligne de fin effective) et effet des éléments d'ouverture, de pas et de test | retenu |
| IMP-0152 | Fin d'itération à l'achèvement de la ligne de fin, sortie ordinaire propagée aux boucles de même ligne de fin, désimbrication par l'intervalle `[ligne_faire, ligne_fin]` | retenu |
| IMP-0153 | Point de départ et point d'arrêt programmables (`EXECUTER A PARTIR DE`), portés par l'état d'exécution | retenu |
| IMP-0154 | Dépassement de la dernière ligne sans terminaison explicite : erreur d'exécution | retenu |
| IMP-0155 | Ligne de déclaration de procédure atteinte hors appel : erreur d'exécution, détectée par la rencontre de l'élément de déclaration | retenu |
| IMP-0179 | Environnement des identificateurs : état et contenu de chaque identificateur, tenus par l'état d'exécution, indexés par l'identifiant de symbole ; règles de transition ; état « paramètre LOCAL sans valeur », agrandissement paresseux | retenu |
| IMP-0208 | Interfaces C : nature rendue, usage de lecture, frontière avec le module `donnees`, fichiers et segments | retenu |

La plage IMP-0144 à IMP-0155 de ce chapitre ne comprend pas IMP-0147 : cette décision, qui fixe la table des paramètres formels consultée à l'appel (§5.5), est rédigée au chapitre Compilateur (§13.3), avec l'élément de déclaration dont la table est dérivée (IMP-0174) et le contrat d'intégration qui la construit (IMP-0176). La table des paramètres formels est tenue par le gestionnaire du programme courant et ne fait pas partie de l'état d'exécution.

---

## 11. Traçabilité

```text
HIS-3.13, HIS-8.1 à HIS-8.3, HIS-10.1, HIS-10.2, HIS-13.1, HIS-13.2, HIS-13.4
  → EXI-0001, EXI-0022 à EXI-0024, EXI-0031 à EXI-0039, EXI-0042 à EXI-0044, EXI-0060
    → VM-0801 à VM-0803 (Tomes I-II §8), VM-0901 à VM-0903 (§9), VM-1001, VM-1002 (§10),
       VM-1103 (§11.3), VM-1302, VM-1303 (§13.2, §13.3), VM-3006, VM-3007, VM-3101, VM-3102
       (Tome III §7.6, §7.7, §8.1, §8.2)
      → IV-FMT-0010 (Tome IV, ANN-B-0001)
        → IMP-0144 à IMP-0146, IMP-0148 à IMP-0155, IMP-0179, IMP-0208 (présent chapitre)
          → CODE : pile.c/.h, contexte_exec.c/.h (étape 4c), interprete.c/.h (étape 5)
            → TESTS : tests/composants/composants_contexte.c

LSE15 §9-2 (EXECUTER, EXECUTER A PARTIR DE, REPRENDRE EN)
  → EXI-0023, EXI-0029, EXI-0030
    → ALG-1101 (Tome IV §11.1, étape 3)
      → IMP-0153, IMP-0154 (présent chapitre, §7.1, §7.2)
```

Ce chapitre referme les points explicitement laissés ouverts par les chapitres précédents :

```text
RCI-M §8 (« Pile d'évaluation... Cadres d'activation, contextes de boucle... hors périmètre
           RCI-M — état d'exécution »), §13 (« Ne couvre pas la représentation physique complète
           de l'état d'exécution... chapitre distinct du Tome V, non encore rédigé »)
  → §3, §4, §5, §6, §8 (présent chapitre)

RCI-M §4.8.2, IMP-0112 (« La gestion précise de la pile de contextes de boucle [...] relève du
                          chapitre du Tome V consacré à l'état d'exécution/interpréteur »)
  → §6 (présent chapitre), notamment IMP-0151, IMP-0152

Architecture physique §10 (« Ne couvre pas la représentation physique complète de l'état
                             d'exécution [...] chapitre distinct du Tome V, non encore rédigé »)
  → §3 à §8 (présent chapitre)
```

---

## 12. Limites explicites de ce chapitre

- Ne couvre pas la représentation interne d'un objet tableau ou chaîne désigné par l'environnement des identificateurs (§3 bis, IMP-0179) : l'allocation et la libération effectives d'un tableau ou d'une chaîne, l'éventuel état non défini de chaque élément de tableau, et le calcul de validation des bornes d'un accès indexé (`decalage` de `ReferenceResolue`, §4.2) relèvent du gestionnaire des variables, des tableaux et des chaînes (module `donnees`), chapitre distinct du Tome V, non encore rédigé. Les règles de transition d'état (déclaration, libération, redéclaration) sont, elles, fixées au §3 bis.
- Ne couvre pas la compilation de la déclaration de procédure ni la construction de la table des paramètres formels (élément de déclaration, structure de la table, contrat d'intégration) : chapitre Compilateur, §13 (IMP-0147, IMP-0174 à IMP-0176). Le présent chapitre n'en fixe que l'usage à l'appel (§5.2, §5.5) et la détection d'une ligne de déclaration atteinte hors appel (IMP-0155).
- Ne couvre pas le sort d'un élément de pile non transitoire qui désigne le texte d'une ligne modifiée ou supprimée pendant une suspension de l'exécution (PAS À PAS, PAUSE suivis d'une édition du programme) : la validité de l'état d'exécution après une modification du programme courant relève des chapitres du moniteur et du programme courant, non encore rédigés.
- Ne couvre pas la libération effective des objets dynamiques transitoires (chaînes intermédiaires retirées de la pile d'évaluation sans être consommées, tableaux `LOCAL` libérés à la sortie d'un appel, §4.4, §5.4) : le point physique d'appel à cette libération est signalé, mais l'algorithme du gestionnaire des chaînes/tableaux lui-même relève du module `donnees`, non encore rédigé.
- Ne couvre pas le format de sérialisation d'un état d'exécution suspendu vers une zone de swap (état de tâche, IV-FMT-0011, ALG-1502/ALG-1503) : les structures définies ici (§4, §5, §6) sont celles qu'un tel protocole devrait sérialiser, mais Architecture physique IMP-0130 ayant retenu une tâche unique sans swap pour la première version, ce point reste explicitement hors périmètre, cohérent avec EXI-0058/EXI-0059 (« ne fixe ni algorithme d'ordonnancement ni représentation physique de la zone de swap »).
- Ne couvre pas le cas d'une procédure externe (LSE15 §7-7, dernier alinéa : « dans une procédure externe, les identificateurs non paramètres sont tous locaux ») ni les mécanismes associés à `EXECUTER` (chargement d'un module de programme, RCI-M §4.11.5, élément 0xAF) : ce mécanisme relève du bibliothécaire et du gestionnaire des tâches, chapitres distincts du Tome V, non encore rédigés.
- Un argument effectif sans valeur passé à un paramètre `LOCAL` ne provoque pas d'erreur à l'appel ; l'erreur `ERR_E_PARAMETRE_SANS_VALEUR` (EX 14) est signalée à l'utilisation du paramètre (§5.5, étape 4 ; Gestionnaire des erreurs §9, RX3). Le paramètre reçoit l'état `ID_PARAMETRE_SANS_VALEUR` (§3 bis, IMP-0179).
- La pile d'évaluation croît par doublement de 16 à 9 362 éléments au plus, et la réserve des cases jusqu'à 13 107 cases (§4.3, IMP-0145 ; §3 bis, IMP-0179) ; au-delà, EX 03. La chaîne de cadres n'a pas de plafond propre : chaque cadre est un bloc distinct, borné par la mémoire disponible (§5.5).
- Ne détaille pas la réalisation physique de la commande `REPRENDRE EN n1{,n2}` (LSE15 §9-2), distincte à la fois d'`INITIALISER` (§7.1, qui réinitialise les identificateurs) et de `CONTINUER`/PAS À PAS (§7.4, qui reprend sans changer de ligne) : cette commande combine un changement de ligne explicite, la préservation des valeurs de variables, et un retour forcé au cadre racine (§7.1) sans dépiler explicitement les cadres intermédiaires selon `RETOURNER` (§5.5) — sa réalisation exacte (notamment le sort des sauvegardes `LOCAL` et alias non restaurés des cadres ainsi abandonnés) est laissée à un travail ultérieur.
- Le comportement de la boucle FAIRE...JUSQUA avec un pas nul (§6.1) n'est pas attesté par le corpus : la règle de sortie de LSE15 §5-5.3 est appliquée littéralement, de sorte que le test ne provoque jamais la sortie. Ce comportement est retenu ; il reste à confronter au corpus de niveau B si celui-ci devient accessible.
- Le format exact du message affiché lors d'un arrêt programmé sur `ligne_arret` (IMP-0153), d'une erreur de dépassement de fin de programme (IMP-0154) ou d'une chute dans une déclaration de procédure (IMP-0155) — y compris, pour ce dernier cas, l'indication du numéro de ligne d'où l'appel aurait dû provenir, mentionnée par LSE15 §9-2 pour les arrêts survenus dans une procédure — relève du gestionnaire des erreurs et du moniteur, chapitres distincts du Tome V, non encore rédigés.

---

## 13. Tests associés

- Évaluation de `A+B*C` (Tome III §6.5) : vérifier que la pile d'évaluation atteint une hauteur maximale de trois éléments avant que `*` ne les réduise à deux, puis que `+` ne les réduise à un — jamais davantage, conformément à l'ordre postfixé.
- `A←B` où `B` est une variable simple non encore affectée : vérifier qu'une `REFERENCE` (jamais une `VALEUR`) est déposée pour le second `A`... — plus précisément, vérifier que la référence à `A` (cible) et la référence à `B` (source, ensuite déréférencée par l'opération d'affectation) sont toutes deux empilées sous forme `REFERENCE`, et que seule l'opération 0x99 distingue leur usage (IMP-0144).
- `&FACT(N) LOCAL N` appelée récursivement (`&FACT(7)`, LSE15 §7-6) : vérifier qu'à chaque appel récursif, une nouvelle `SauvegardeLocale` est empilée pour `N`, que la valeur restaurée à chaque retour correspond exactement à celle laissée par l'appel immédiatement englobant (jamais celle du tout premier appel), et que le résultat final (`5040`) est correct (IMP-0149).
- `&RAC2(1,-1,-6,ALPHA,BETA,GAMMA)` (LSE15 §7-8.2) : vérifier qu'après l'appel, `ALPHA` contient `25`, `BETA` contient `3`, `GAMMA` contient `-2`, et qu'aucune `SauvegardeLocale` n'a été créée pour `DISCR`, `X1`, `X2` (paramètres référence, Compilateur IMP-0147 ; IMP-0148) — puis, avec `&RAC2(3,2,1,DISCR,RAC1,RAC2)`, vérifier que `RAC1` et `RAC2` restent inchangés lorsque le discriminant est négatif (sortie par `RETOUR` avant les affectations à `X1`/`X2`).
- `&TRAC(&F1,0,2,0.25)` (LSE15 §7-8.2) : vérifier qu'un appel à `&F(X)` à l'intérieur du corps de `&TRAC` résout effectivement `&F1`, y compris après un second appel `&TRAC(&F2,5,5.5,0.1)` où `&F` doit alors résoudre `&F2` — vérification directe d'IMP-0148 (capture eagère, pas de confusion entre deux activations successives de `&TRAC`).
- `PROCEDURE &RAC(ARG,ERRE) LOCAL ARG,ERRE; SI ARG<0 ALORS RETOUR EN ERRE` appelée en position d'expression (LSE15 §7-5) : vérifier que, lorsque la condition est vraie, l'affectation qui utilisait cet appel n'a pas lieu, et que la pile d'évaluation retrouve exactement la hauteur qu'elle avait avant le début de l'instruction englobante (§7.3).
- `3 SI A=0 ALORS DEBUT FAIRE 5 POUR I←0 JUSQUA 9; J←I+1 FIN SINON ALLER EN 206` (LSE15 §5-5.2, déjà cité RCI-M §14) : vérifier que le contexte de boucle empilé lors du premier passage porte `ligne_faire=3`, `ligne_fin=5`, `decalage_reprise` désignant l'élément de pas implicite 0xCE qui suit l'ouverture — puis que chaque achèvement de la ligne 5 reprend à cet élément (ajout du pas, réévaluation de la limite, test) et poursuit à l'affectation `J←I+1` à l'intérieur de la ligne 3 ; après dix itérations, vérifier la sortie ordinaire vers la ligne suivant 5, avec `I` valant 10 et le contexte dépilé (IMP-0151, IMP-0152).
- `FAIRE 8 POUR I←T PAS -1 JUSQUA 1` suivi d'un `ALLER EN` explicite vers une ligne strictement postérieure à `8` avant que la borne ne soit atteinte : vérifier que le contexte de boucle est dépilé par `DESIMBRIQUER`, sans ajout du pas ni test (IMP-0152).
- Deux boucles `FAIRE` imbriquées, sortie anticipée par un `ALLER EN` ciblant une ligne postérieure aux deux lignes de fin : vérifier que les deux contextes sont dépilés en une seule séquence de transferts de contrôle, dans l'ordre du sommet vers la base (IMP-0152).
- Une boucle `FAIRE` active au moment d'un `RETOUR` de la procédure qui la contient : vérifier que le cadre — et donc sa pile de contextes de boucle — est intégralement détruit sans qu'aucun test de désimbrication explicite ne soit nécessaire (IMP-0152, retour de procédure).
- `PROCEDURE &UNIT(A,N) LOCAL N,I,J` appelée alors qu'une variable globale `I` de même nom est active dans le programme appelant : vérifier qu'`I` retrouve sa valeur antérieure après le retour, y compris lorsque `&UNIT` est elle-même appelée depuis une autre procédure ayant également déclaré `I` comme `LOCAL` (empilement/dépilement correct à travers deux niveaux, IMP-0149).
- `PROCEDURE &PRO(A,B) LOCAL A,M,N; TABLEAU M[3,A]` (LSE15 §7-9) : vérifier qu'à la sortie de `&PRO`, le tableau alloué pour `M` est libéré (le `HandleObjet` transitoire n'est plus référencé) et que `M` redevient, dans le contexte appelant, dans l'état où il se trouvait avant l'appel (IMP-0149 ; entrée d'environnement `ID_TABLEAU`, §3 bis).
- Suspension par `PAUSE` au milieu d'une expression comportant un appel de procédure non encore résolu, puis reprise : vérifier que la pile d'évaluation, la chaîne de cadres et les contextes de boucle actifs sont retrouvés à l'identique après reprise, sans reconstruction (§7.4).
- `FAIRE 8 POUR I←T PAS -1 JUSQUA 1` suivi d'un `ALLER EN` explicite vers une ligne strictement antérieure à `ligne_faire` (donc avant le début de la boucle) avant que la limite ne soit atteinte : vérifier que le contexte de boucle est dépilé exactement comme pour une sortie par le bord supérieur (IMP-0152, bord inférieur) — puis, si l'exécution retraverse ensuite séquentiellement `ligne_faire`, vérifier qu'un unique contexte frais est actif au sommet de la pile (absence de contexte fantôme résiduel de la tentative abandonnée).
- `10 FAIRE 12 POUR K←1 TANT QUE X[K]>=0 ET K<=50` / `12 SOMME←SOMME+X[K]` (LSE15 §5-5.3), avec `K` valant initialement 99 : vérifier que la condition est évaluée au premier passage avec `K` valant 1, non 99, et que `K` contient, à la sortie, l'indice du premier terme négatif, ou 51 s'il n'y en a pas (IMP-0151).
- `20 FAIRE 20 POUR X←A PAS (A/X-X)/2 TANT QUE ABS(X↑2-A)>1E-3` avec `A` valant 2 (LSE15 §5-5.3) : vérifier qu'à chaque achèvement de la ligne 20 le pas est réévalué avec la valeur courante de `X`, ajouté à `X`, puis la condition réévaluée, et que `X` contient à la sortie une valeur dont le carré diffère de 2 de moins de 10⁻³ (IMP-0151).
- `100 FAIRE 104 POUR I←1 JUSQUA N` / `102 FAIRE 104 POUR J←1 JUSQUA N` / `104 A[I,J]←0` (LSE15 §5-5.5), puis la forme `100 FAIRE 100 POUR I←1 JUSQUA N; FAIRE 100 POUR J←1 JUSQUA N;A[I,J]←0` : vérifier que la sortie ordinaire de la boucle sur `J` fait passer la boucle sur `I` à son itération suivante, sans la dépiler, et que les N² éléments sont mis à zéro (IMP-0152, boucles de même ligne de fin).
- `1 FAIRE 3 POUR I←1 JUSQUA N` / `3 SI A[I]=0 ALORS ALLER EN 20` (LSE15 §5-5.4) : vérifier que, si aucun élément n'est nul, l'exécution atteint la ligne qui suit 3 avec `I` valant N+1, et que, sinon, elle atteint la ligne 20 avec `I` égal à l'indice de l'élément nul et le contexte dépilé (IMP-0152, sortie par branchement depuis la ligne de fin).
- `10 FAIRE 15 POUR I←1 JUSQUA 3` sur un programme sans ligne 15 mais comportant les lignes 12 et 20 : vérifier que la ligne de fin effective est 12 (LSE15 §5-5.1, IMP-0151). Puis `10 FAIRE 5 POUR I←1 JUSQUA 3` et `10 FAIRE 0 POUR I←1 JUSQUA 3` : vérifier une erreur d'exécution dans les deux cas, avant toute affectation de `I`.
- Branchement répété vers la ligne FAIRE depuis le corps de sa boucle : vérifier que `V` reçoit de nouveau `v1` (LSE15 §5-5.4, Remarque 2) et que la pile de boucles ne contient toujours qu'un seul contexte pour cette boucle (IMP-0151, réinitialisation en place).
- `200 FAIRE 202 POUR I←1 PAS P JUSQUA 5` où le corps modifie `P` de 1 à -1 : vérifier que le signe du pas réévalué gouverne le test suivant (LSE15 §5-5.3, « le signe du pas est réévalué avant comparaison de V à ε4 »).
- `1 TABLEAU RESUL[10]` puis `2 RESUL[5.69]←1` : vérifier que c'est le sixième élément qui reçoit 1 ; `2 RESUL[0.4]←1` : vérifier une erreur d'exécution (indice arrondi à 0, interdit) ; `ALLER EN 4.6` : vérifier le branchement à la ligne 5 (§3 bis, conversion en entier).
- `5 AFFICHER X` exécuté juste après `EXECUTER`, sans affectation préalable de `X` : vérifier une erreur d'exécution (HIS-12.3, IMP-0179) ; puis `5 X←3;AFFICHER X` : vérifier que l'entrée de `X` passe à `ID_VARIABLE` et que 3 est affiché.
- `1 TABLEAU A[3]` puis `2 TABLEAU A[5]` : vérifier la libération implicite du premier tableau et un tableau de 5 éléments ; `1 B←1` puis `2 TABLEAU B[3]` : vérifier une erreur d'exécution (LSE15 §3-1.2) ; `1 CHAINE C` puis `2 TABLEAU C[3]` : idem.
- `1 CHAINE C;C←'ABC'` puis `2 CHAINE C;AFFICHER C` : vérifier que la seconde déclaration libère la valeur et que l'affichage provoque une erreur d'exécution (variable-chaîne sans valeur, IMP-0179).
- `1 X←1` puis `2 CHAINE X` : vérifier une erreur d'exécution (LSE15 §4-3) ; puis `1 X←1;LIBERER X` et `2 CHAINE X;X←'OUI'` : vérifier que la déclaration est admise après libération (LSE15 §3-1.2, Remarque 1).
- `1 C←'ABC'` sans déclaration `CHAINE` préalable : vérifier une erreur d'exécution (LSE15 §4-3).
- `1 X←2` puis `2 Y←X[1]` : vérifier une erreur d'exécution (accès indexé à un identificateur qui n'est pas un tableau, LSE15 §3-1.2).
- Procédure `10 PROCEDURE &P() LOCAL M` dont le corps exécute `LIBERER M` puis `LIBERER G`, `G` étant global : vérifier que la première libération est admise et que la seconde provoque une erreur d'exécution (LSE15 §3-1.2, Remarque 2).
- Valeurs affectées en mode machine de bureau (`X←5`), puis `EXECUTER` : vérifier qu'après l'initialisation l'entrée de `X` vaut `ID_NON_DEFINI` (LSE15 §9-2, §7.1 étape 2).
- Vérifier, par instrumentation, qu'aucune exécution d'instruction n'écrit dans la table des symboles du programme courant (RCI-M IMP-0108, IMP-0110).
- `5 C←'ABC'!'DEF';AFFICHER 'TEXTE CONSTANT'` (avec `CHAINE C` préalable) : vérifier que les deux littéraux sont déposés sur la pile comme non transitoires et ne sont jamais libérés, que le résultat de `!` est transitoire et devient la valeur de `C` sans copie, et qu'après l'exécution de la ligne aucun objet transitoire ne subsiste (instrumentation du gestionnaire d'objets ; §4.4).
- `EXECUTER A PARTIR DE 100` sur un programme dont la plus petite ligne existante porte le numéro `200` : vérifier qu'une erreur est détectée avant tout début d'exécution, sans modification de l'état des identificateurs (IMP-0153, LSE15 §9-2).
- `EXECUTER A PARTIR DE 1,50` sur un programme dont l'exécution atteint effectivement la ligne `50` : vérifier que la ligne `50` elle-même n'est pas exécutée, que la console repasse en état « moniteur » en affichant `50`, et que la pile d'évaluation, la chaîne de cadres et les identificateurs sont dans l'état laissé par l'exécution des lignes `1` à `49` (IMP-0153).
- `EXECUTER A PARTIR DE 1,50` sur un programme où la ligne `50` n'existe pas et n'est jamais atteinte par un branchement : vérifier que l'exécution se poursuit normalement jusqu'à `PAUSE`/`TERMINER`/erreur, sans jamais déclencher l'arrêt programmé (LSE15 §9-2, IMP-0153).
- Un programme dont la dernière ligne physique ne comporte ni `TERMINER` ni `PAUSE` et dont l'exécution y parvient par simple progression séquentielle : vérifier qu'une erreur d'exécution est signalée (IMP-0154), plutôt qu'une terminaison silencieuse.
- Un `ALLER EN` ciblant explicitement un numéro de ligne strictement supérieur à toutes les lignes existantes du programme courant : vérifier une erreur d'exécution de nature `ERR_E_LIGNE_INEXISTANTE` (EX 75), distincte de celle du dépassement séquentiel de la dernière ligne, `ERR_E_FIN_DE_PROGRAMME` (EX 58) (IMP-0154).
- `10 Y←&F(1)` avec `20 PROCEDURE &F(A);RETOUR` : vérifier l'erreur `ERR_E_RETOUR_DANS_FONCTION` (EX 15) ; `10 &G(1)` avec `20 PROCEDURE &G(A);RESULTAT A` : vérifier l'erreur `ERR_E_RESULTAT_SOUS_PROGRAMME` (EX 22) (§5.5, étape 1 bis).
- `10 &P(Z);TERMINER` avec `Z` non défini et `20 PROCEDURE &P(A) LOCAL A;AFFICHER 'DANS P';RETOUR` : vérifier que `DANS P` est affiché sans erreur ; puis, si le corps lit `A`, l'erreur `ERR_E_PARAMETRE_SANS_VALEUR` (EX 14) (§5.5, étape 4).
- Un programme principal dont l'exécution atteint séquentiellement, sans passer par un `APPEL`, la ligne `1 PROCEDURE &RAC(ARG,ERRE) LOCAL ARG,ERRE` (LSE15 §7-8.2) : vérifier qu'une erreur d'exécution est signalée dès cette ligne, avant toute tentative d'exécution du corps de la procédure (IMP-0155).
- Appel de `&RAC2` dont la ligne de déclaration `200 PROCEDURE &RAC2(A,B,C,DISCR,X1,X2) LOCAL A,B,C` ne porte aucune instruction après la déclaration : vérifier que `APPELER` positionne l'exécution après l'élément 0xCD (§5.5, étape 7), que l'exécution passe à la ligne suivante sans erreur, et qu'une chute séquentielle ultérieure dans la ligne de déclaration d'une autre procédure `&AUTRE`, si le corps de `&RAC2` ne se termine pas par un `RETOUR`/`RESULTAT` avant de l'atteindre, déclenche bien l'erreur (IMP-0155).
- Deux déclarations de `&P`, en lignes 10 et 50 (LSE15 §7-6, Note) : vérifier qu'un appel exécute la déclaration de la ligne 50, et qu'un `ALLER EN 10` déclenche l'erreur d'IMP-0155 bien que la ligne 10 ne soit la ligne d'entrée d'aucune `InfoProcedure` (déclaration masquée).
- Appel `&Q(1)` d'une procédure jamais déclarée dans le programme courant (`InfoProcedure = {0, 0}`, Compilateur IMP-0176) : vérifier qu'une erreur d'exécution est signalée à l'étape 1 d'`APPELER`, avant toute création de cadre.
- `25 PROCEDURE &FACT(N) LOCAL N` puis `A←&FACT(7,2)` (deux arguments pour une arité de 1) et `A←&FACT()` (aucun argument) : vérifier que chaque appel produit une erreur d'exécution à l'étape 1 d'`APPELER` (LSE15 §7-8, RCI-M IMP-0177), sans création de cadre, sans modification de `N` ni de `A`, et avec une pile d'évaluation inchangée par rapport à l'instant qui précède l'élément APPEL.
- `A←B+&FACT(3)` : vérifier que la référence à `&FACT` est lue à la profondeur `k = 1`, sous l'argument `3` et au-dessus des éléments déjà déposés pour `B` (RCI-M IMP-0177), et qu'au retour par `RESULTAT` la pile ne contient plus, sous la valeur produite, que les éléments propres à l'expression englobante — ni l'argument, ni la référence à la procédure.
- Appel `&TRAC(&F1,0,2,0.25)` (LSE15 §7-8.2) dans lequel le corps appelle `&F(X)` : vérifier que la référence à `&F`, lue à la profondeur `k = 1`, est résolue par l'alias du cadre de `&TRAC` vers `&F1`, et que la comparaison de `k` porte sur l'arité de `&F1` (§5.3, IMP-0148).