# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — Gestionnaire d'objets : stratégie d'allocation interne, compaction, format de l'en-tête de bloc

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V (spécification détaillée complète de l'implémentation), niveau `05-IMPLEMENTATION/decisions/`
**Portée :** réalisation physique du gestionnaire d'objets lui-même — stratégie d'allocation interne aux blocs-maîtres de 4 Kio, algorithme de compaction, format de l'en-tête de bloc et encodage de `HandleObjet`. Ce chapitre ne redéfinit ni le contenu logique de RCI-M (chapitre RCI-M), ni la répartition noyau/plateforme/interfaces (chapitre Organisation du dépôt), ni les deux familles d'allocation (chapitre Architecture physique) : il en réalise la seconde famille (handles compactables) dans le détail.
**Rang normatif :** subordonné à ANN-K (Tome IV), à VM-1601/VM-1602 (Tome III §2.3.11, §2.5.8), à RCI-M (§3, note de dépendance ; §5 ; IMP-0101, IMP-0108) et à Architecture physique (§6.1, IMP-0132 à IMP-0134). Aucune disposition du présent chapitre ne peut contredire ces sources ; il se limite à préciser ce qu'elles ont explicitement laissé ouvert.
**Place dans le Tome V :** ce chapitre referme le point laissé ouvert par RCI-M (§3, note de dépendance ; §13) et repris sans être refermé par Architecture physique (§6.1, IMP-0132 ; §10) et par Organisation du dépôt (§2.1, qui localise seulement le module dans `src/noyau/objets.c/.h` et `src/plateforme/memoire_iigs.c/.h`, sans en préciser la réalisation interne, IMP-0136). Sa position exacte dans le sommaire final du Tome V n'est pas fixée par ce document.

---

## 0. Avertissement de statut

Comme pour RCI-M, Architecture physique, Organisation du dépôt et État d'exécution, aucune disposition du présent chapitre ne constitue une reconstruction historique. Le Tome IV (ANN-K-0032, points 12 et 13) précise que « la représentation sérialisée reste indépendante de l'organisation mémoire de l'implémentation » et que « la représentation physique d'une implémentation ne constitue pas le format logique RCI ». Tout ce qui suit — taille de bloc, format d'en-tête, algorithme de compaction, encodage de `HandleObjet` — est donc systématiquement une **décision d'implémentation (`IMP-*`)**, jamais une propriété logique de RCI-M ni une reconstruction du fonctionnement historique du L.S.E. Tome III (§2.3.11) le rappelle explicitement pour ce composant précis : « la stratégie interne de gestion de cet espace n'est pas imposée ».

---

## 1. Rappel des contrats obligatoires (non renégociables)

### 1.1 Exigences EXI directement concernées

| EXI | Objet | Conséquence physique imposée |
|---|---|---|
| EXI-0006 | Unicité du numéro de ligne | Le remplacement d'une ligne (RCI-M IMP-0101) libère l'ancien objet et en crée un nouveau ; le présent chapitre doit garantir qu'aucune confusion n'est possible entre l'ancien et le nouveau `HandleObjet` une fois l'ancien libéré |
| EXI-0016 | Compacité | L'en-tête de bloc et la table de slots ne doivent porter aucune information reconstructible autrement ; leur coût fixe doit rester mesuré et justifié (§5, §14) |
| EXI-0040 | Tableaux à une ou deux dimensions | Les tableaux d'exécution, dont la taille peut dépasser largement celle d'une ligne compilée, doivent pouvoir être hébergés par ce gestionnaire sans le contredire (§9, objets hors-norme) |
| EXI-0041 | Allocation à la déclaration du tableau | L'acquisition et la libération d'espace pour un tableau passent par les opérations logiques du présent chapitre (§11), sans allocation permanente |
| EXI-0049 | Séparation du stockage logique et de la mémoire d'exécution | Le gestionnaire d'objets n'héberge que des objets d'exécution ou des séquences compilées transitoires (RCI-M §3 bis) ; il n'est jamais le support d'une représentation persistante (RCI-S, fichiers-programmes) |
| EXI-0050 | Allocation et libération des objets d'exécution | Un objet dont la durée de vie est dynamique doit pouvoir être libéré sans qu'une allocation permanente pour la session entière soit requise ; le présent chapitre l'implémente au niveau du bloc-maître lui-même (§10, libération d'un bloc devenu vide) |

### 1.2 Responsabilité couverte (Tome III §2.3.11, §2.5.8 — VM-1601/VM-1602)

Tome III (§2.3.11) définit la responsabilité du gestionnaire mémoire : il « fournit l'espace abstrait nécessaire aux objets dont la durée de vie est dynamique » et prend en charge « la création, la réservation logique, la libération, la détection de l'absence de ressource disponible ». Tome III (§2.5.8, « Interface mémoire ») en fixe les cinq opérations logiques : créer un objet, obtenir un objet, libérer un objet, vérifier l'existence d'un objet, vérifier la capacité disponible — « le gestionnaire mémoire ne définit pas la sémantique des objets qu'il héberge ». Ces cinq opérations sont réalisées au §11 du présent chapitre.

### 1.3 Points explicitement laissés ouverts par les chapitres précédents

- RCI-M §3 (note de dépendance) : « les décisions physiques propres à ce gestionnaire (stratégie d'allocation, algorithme de compaction, format de son en-tête de bloc) ne sont pas prises par le présent chapitre ». RCI-M §13 le répète comme limite explicite.
- Architecture physique §6.1 (IMP-0132) : fixe le principe des deux familles d'allocation et le fait que le gestionnaire d'objets s'appuie sur le Memory Manager toolbox, mais indique explicitement, dans son propre statut : « la stratégie d'allocation interne au bloc de 4 Kio, l'algorithme de compaction et le format exact de l'en-tête de bloc restent à préciser dans un chapitre dédié au gestionnaire d'objets ». Architecture physique §10 (Limites explicites) répète ce même point.
- Organisation du dépôt §2.1 : localise le module (`src/noyau/objets.c/.h`, `src/plateforme/memoire_iigs.c/.h`, `src/interfaces/interface_memoire.h`, segment `"objets"`) mais ne fait, de son propre aveu, que le **localiser** — IMP-0136 ne referme aucun des points ci-dessus.

Le présent chapitre referme ces trois points simultanément.

### 1.4 Interdictions du conditionnement (§9, §19), reprises de RCI-M §1.4

Les pointeurs C ne constituent pas un modèle architectural. `HandleObjet` reste l'unique identifiant inter-objets ; toute résolution en adresse physique (`objets_resoudre`, §11) n'est valable que pour la durée de l'appel courant et ne doit jamais être conservée au-delà — en particulier, elle ne doit jamais être conservée à travers un appel susceptible de déclencher une allocation, une libération ou une compaction (§8), y compris à l'intérieur du même bloc-maître.

---

## 2. Contraintes de plateforme retenues (ORCA/C 2.2.1 / 65816)

Ce chapitre reprend intégralement RCI-M §2 (tailles des types C, absence de bourrage de structure — décisif pour le calcul exact de la taille d'en-tête au §5) et Architecture physique §2 et §6.1 (Memory Manager toolbox accessible via `MEMORY.H`, `NewHandle`/`DisposeHandle` rendant un bloc déplaçable, `HLock`/`HUnlock` en autorisant la résolution temporaire en pointeur physique). Il ajoute un seul élément spécifique :

- Architecture physique §6.1 établit que, sur Apple IIGS, `malloc` sous-alloue déjà en interne, pour ses propres besoins, des blocs de 4 Kio obtenus du Memory Manager (manuel ORCA/C, chap. 20). Le présent chapitre retient la **même granularité de 4 Kio** pour le bloc-maître du gestionnaire d'objets — non parce que cette valeur serait imposée à ce composant (elle ne l'est pas : Tome III §2.3.11 le dit explicitement), mais parce qu'elle est déjà celle que le runtime ORCA/C emploie pour un besoin structurellement analogue (beaucoup de petites allocations, un compromis fragmentation/overhead déjà éprouvé sur cette même plateforme), ce qui évite d'introduire une seconde granularité arbitraire (§14).

---

## 3. Architecture physique générale du gestionnaire d'objets

```text
GESTIONNAIRE D'OBJETS (module noyau/objets.c)
│
├── TABLE DES BLOCS-MAÎTRES            table_blocs[0..254]  (malloc, taille fixe, §10)
│      table_blocs[i] : HandlePlateforme (ou NULL si l'indice i n'est pas actif)
│      — l'indice i, une fois attribué à un bloc actif, n'est JAMAIS réaffecté
│        à un autre bloc tant que ce bloc reste actif (§10) : il fait partie
│        intégrante de tout HandleObjet en circulation qui désigne un objet
│        hébergé par ce bloc.
│
├── BLOCS-MAÎTRES (jusqu'à 255)         chacun : 1 HandlePlateforme Memory Manager,
│      chacun : 4096 octets utiles       verrouillé/déverrouillé pour la durée
│      ┌─────────────────────────┐       d'un seul accès (§11)
│      │ EN-TÊTE (§5)            │
│      │  nb_slots, nb_occupes,  │
│      │  premier_slot_libre,    │
│      │  limite_donnees,        │
│      │  octets_libres          │
│      │  slots[0..63]           │      table de slots, taille fixe (§5, IMP-0158)
│      ├─────────────────────────┤
│      │ (zone libre / trous)    │      croît/décroît au gré des allocations
│      ├─────────────────────────┤      et libérations (§6, §7) ; compactée
│      │ ZONE DE DONNÉES         │      paresseusement (§8)
│      │ (objets vivants,        │      grandit depuis la fin du bloc (offset
│      │  tassés depuis la fin)  │      4096) vers le début, au gré des créations
│      └─────────────────────────┘
│
└── TABLE DES OBJETS HORS-NORME        table_hors_norme[0..254]  (malloc, taille fixe, §9)
       table_hors_norme[i] : { HandlePlateforme, longueur }
       — objets dont la taille dépasse la capacité utile d'un bloc-maître vide ;
         chacun possède son propre bloc Memory Manager dédié, sans sous-allocation.
```

**Rappel du contrat déjà fixé par RCI-M §3.** `HandleObjet` reste un identifiant opaque de 16 bits (RCI-M IMP-0101), résolu temporairement en adresse physique sans jamais exposer cette adresse au-delà de l'appel courant, et stable à travers toute compaction — qu'elle soit décidée par le Memory Manager toolbox lui-même (déplacement d'un bloc-maître entier, transparent pour ce chapitre puisqu'il ne s'appuie jamais sur une adresse physique persistante) ou par l'algorithme de compaction interne du présent chapitre (§8, déplacement d'un objet à l'intérieur de son bloc-maître).

---

## 4. `HandleObjet` : encodage 16 bits

**IMP-0157** — *Encodage de `HandleObjet` en un octet d'indice de bloc-maître et un octet d'indice de slot*
- Sujet : RCI-M IMP-0101 fixe `HandleObjet` à 16 bits sans en préciser la structure interne. Quelle décomposition retenir ?
- Choix :
  ```c
  typedef unsigned int HandleObjet;   /* 16 bits, RCI-M IMP-0101 */

  #define GESTOBJ_SENTINELLE        0xFFu  /* valeur réservée, partagée par les deux octets */
  #define GESTOBJ_INDICE_HORS_NORME GESTOBJ_SENTINELLE  /* octet haut : désigne la table §9 plutôt qu'un bloc-maître */
  #define GESTOBJ_SLOT_AUCUN        GESTOBJ_SENTINELLE  /* octet bas : « pas de slot » / fin de liste des slots libres */
  #define GESTOBJ_HANDLE_NUL        ((HandleObjet)0xFFFFu)  /* les deux octets à la sentinelle : « aucun objet » */

  #define GESTOBJ_FAIRE_HANDLE(bloc, slot) \
      ((HandleObjet)(((unsigned int)(bloc) << 8) | (unsigned int)(slot)))
  #define GESTOBJ_BLOC(h)  ((unsigned char)((h) >> 8))
  #define GESTOBJ_SLOT(h)  ((unsigned char)((h) & 0xFFu))
  ```
  L'octet haut désigne soit l'indice d'un bloc-maître actif (0 à 254), soit, s'il vaut `GESTOBJ_INDICE_HORS_NORME`, indique que l'objet est hors-norme (§9) et que l'octet bas est alors un indice dans `table_hors_norme`. L'octet bas, dans le cas d'un bloc-maître ordinaire, désigne un indice de slot dans la table de ce bloc (§5) ; la valeur `GESTOBJ_SLOT_AUCUN` y est également utilisée comme sentinelle interne de chaînage (§5, §7) mais n'apparaît jamais comme second octet d'un `HandleObjet` réellement en circulation, sauf dans `GESTOBJ_HANDLE_NUL` lui-même.
- Alternatives envisagées :
  1. Un identifiant plat sur 16 bits, indexant une unique table globale d'objets (tous blocs confondus) — écarté : réintroduit exactement la table globale contiguë que RCI-M IMP-0101 a déjà rejetée pour la table des objets compilés, et pour la même raison (toute réorganisation locale devrait décaler des indices déjà distribués).
  2. Réserver plus de bits à l'indice de bloc qu'à l'indice de slot (par exemple 10 bits / 6 bits) pour permettre davantage de blocs au prix de moins de slots par bloc — écarté à ce stade : rien dans le corpus ne permet de savoir, avant mesure sur cible (Architecture physique IMP-0134), si la pression s'exercera plutôt sur le nombre de blocs ou sur le nombre d'objets par bloc ; un découpage symétrique en deux octets est la décision la plus simple et la plus lisible en C, et n'interdit pas une révision ultérieure si la mesure le justifie (§16).
- Justification : un découpage en deux octets égaux exploite directement la taille déjà fixée par RCI-M (16 bits, aucun bit supplémentaire disponible sans revenir sur ce chapitre) tout en restant trivialement décomposable par masquage/décalage sur 65816, sans division ni table de correspondance supplémentaire. La sentinelle unique `0xFF`, partagée par les deux octets, réduit à une seule constante nommée l'ensemble des cas particuliers (bloc hors-norme, absence de slot, chaînage de slots libres, handle nul).
- Impact mémoire : nul en soi — c'est un encodage, pas une structure.
- Impact disque : sans objet (RCI-M §3 : le gestionnaire d'objets n'est jamais sérialisé, EXI-0049).
- Impact performances : décodage par masquage et décalage, coût négligeable sur 65816.
- Impact historique : neutre — décision de réalisation pure, aucune conséquence sur le comportement L.S.E. observable (Tome IV ANN-K-0032, point 13).
- Références normatives : RCI-M IMP-0101 (16 bits, note de dépendance) ; conditionnement §9 (interdiction des pointeurs C comme modèle architectural).
- Statut : retenu.

---

## 5. Format de l'en-tête de bloc-maître et de la table de slots

**IMP-0158** — *Table de slots de taille fixe (64 entrées) par bloc-maître*
- Sujet : comment organiser, à l'intérieur des 4096 octets utiles d'un bloc-maître, la cohabitation entre la table de slots (qui doit rester accessible dès le verrouillage du bloc) et les octets des objets eux-mêmes ?
- Choix : un en-tête de taille **fixe**, occupant le début du bloc, contenant une table de slots de capacité fixe, et une zone de données occupant le reste du bloc, remplie depuis la fin (offset 4096) vers le début au fur et à mesure des créations (§6) :
  ```c
  #define GESTOBJ_TAILLE_BLOC             4096u  /* octets utiles d'un bloc-maître, cf. §2 */
  #define GESTOBJ_NB_SLOTS_MAX_PAR_BLOC   64     /* capacité fixe de la table de slots, cf. justification ci-dessous */

  typedef struct {
      unsigned int decalage;  /* slot occupé : décalage, depuis le début du bloc, du premier octet
                                  de l'objet ; slot libre : indice du slot libre suivant dans la
                                  liste chaînée des slots recyclables, ou GESTOBJ_SLOT_AUCUN si dernier */
      unsigned int longueur;  /* slot occupé : longueur en octets de l'objet ; 0 si le slot est libre */
  } EntreeSlot;

  typedef struct {
      unsigned char nb_slots;            /* nombre d'entrées de slots[] actuellement affectées
                                             (occupées ou recyclables), <= GESTOBJ_NB_SLOTS_MAX_PAR_BLOC */
      unsigned char nb_occupes;          /* nombre de slots actuellement occupés par un objet vivant ;
                                             0 => bloc entièrement vide, cf. §10 */
      unsigned char premier_slot_libre;  /* tête de la liste des slots libres recyclables,
                                             GESTOBJ_SLOT_AUCUN si aucun */
      unsigned int  limite_donnees;      /* borne basse de la zone de données : le premier octet du
                                             bloc déjà attribué à un objet ; la zone de données occupe
                                             [limite_donnees, GESTOBJ_TAILLE_BLOC[ */
      unsigned int  octets_libres;       /* octets récupérables par compaction (somme des trous laissés
                                             par des objets libérés hors bord, cf. §7) */
      EntreeSlot    slots[GESTOBJ_NB_SLOTS_MAX_PAR_BLOC];
  } EnTeteBlocMaitre;
  ```
  Avec les tailles fixées par RCI-M §2 (`unsigned char` = 1 octet, `unsigned int` = 2 octets, aucun bourrage de structure sous ORCA/C), `sizeof(EnTeteBlocMaitre)` vaut exactement 3 + 2 + 2 + 64 × 4 = **263 octets**, laissant **3833 octets** utiles pour les objets d'un bloc par ailleurs vide — valeur reprise comme seuil de bascule vers les objets hors-norme (§9).
- Alternatives envisagées :
  1. Table de slots dimensionnée à la capacité maximale que l'encodage 16 bits pourrait théoriquement adresser (255 entrées) — écarté : porterait l'en-tête fixe à 3 + 2 + 2 + 255 × 4 = 1027 octets, soit environ 25 % du bloc immobilisés en bookkeeping quel que soit le nombre d'objets réellement présents, contre 6,4 % pour 64 entrées — disproportionné au regard d'EXI-0016 (compacité) et du conditionnement §4.1 (« éviter les structures surdimensionnées »).
  2. Table de slots de taille variable, croissant depuis la fin de l'en-tête au fur et à mesure des créations (symétriquement à la zone de données) — écarté pour la première version : économiserait de la mémoire pour les blocs à peu d'objets volumineux, mais introduit une seconde frontière mobile à surveiller (en plus de `limite_donnees`) et complique la détection de collision entre les deux zones mobiles ; le conditionnement §3 place la simplicité de réalisation (priorité 6) avant l'économie de mémoire (priorité 7).
  3. Champ de bits pour compresser `nb_slots`/`nb_occupes`/`premier_slot_libre` sur moins d'un octet chacun — écarté par simplicité, comme déjà pour RCI-M (§2 du conditionnement plateforme, IMP-0101 RCI-M) : le gain (quelques octets par bloc) ne justifie pas la perte de lisibilité sur cette plateforme.
- Justification : une capacité fixe de 64 slots garde l'en-tête à un coût mesuré (6,4 % du bloc) tout en restant largement suffisante au regard des tailles d'objets attendues : un programme de 2000 lignes (RCI-M §8) tiendrait, pour ses seules séquences RCI-M, dans une trentaine de blocs à 64 slots chacun — un chiffre à confirmer par mesure sur corpus historique (§16), mais qui n'est structurellement pas disproportionné.
- Impact mémoire : 263 octets fixes par bloc-maître actif, quel que soit son taux de remplissage réel ; 3833 octets utiles pour les objets d'un bloc vide.
- Impact disque : sans objet.
- Impact performances : accès direct par indice à `slots[i]` une fois le bloc verrouillé, sans parcours ; coût de verrouillage du bloc entier (Architecture physique IMP-0132, « quelques cycles 65816 »).
- Impact historique : neutre.
- Références normatives : RCI-M §2 (tailles C, absence de bourrage) ; EXI-0016 ; conditionnement §4.1, §3 (priorités 6/7).
- Statut : retenu, capacité (64) à réviser par mesure sur corpus si elle se révèle contraignante (§16).

Chaque slot occupé porte donc `decalage` et `longueur` ; un slot libre réutilise le champ `decalage` pour chaîner vers le slot libre suivant (`longueur` vaut alors 0, ce qui suffit à distinguer les deux cas sans champ supplémentaire — EXI-0016).

---

## 6. Algorithme d'allocation interne à un bloc-maître

**IMP-0159** — *Allocation par recherche du premier bloc convenant, recyclage prioritaire des slots libres, compaction paresseuse avant échec*
- Sujet : algorithme retenu pour satisfaire une demande de création d'objet (`objets_creer`, §11) au sein des blocs-maîtres existants, avant de recourir à un nouveau bloc.
- Choix : pour une longueur demandée `L` inférieure ou égale au seuil des objets hors-norme (§9), balayer les blocs-maîtres actifs dans l'ordre de leur indice ; pour chaque bloc `b` :
  1. lire l'adresse de `b` dans son pointeur maître (`plateforme_bloc_adresse`), sans verrouiller le bloc (IMP-0243) ;
  2. si `limite_donnees(b) − L ≥ sizeof(EnTeteBlocMaitre)` **et** (`nb_slots(b) < 64` **ou** `premier_slot_libre(b) ≠ GESTOBJ_SLOT_AUCUN`) : la place contiguë suffit directement — allouer (ci-dessous), retourner le `HandleObjet` obtenu ;
  3. sinon, si `(limite_donnees(b) − sizeof(EnTeteBlocMaitre)) + octets_libres(b) ≥ L` et la même condition de disponibilité de slot : la place ne suffit pas *contiguë* mais suffirait *au total*, place contiguë et trous réunis — compacter `b` (§8), puis réappliquer l'étape 2 (désormais satisfaite par construction : la compaction ajoute `octets_libres(b)` à la place contiguë) ;
  4. sinon, passer au bloc suivant.

  Aucun appel système ni toolbox n'a lieu entre la lecture de l'adresse de `b` et son dernier usage : le parcours, la compaction (`memmove`) et l'allocation sont du calcul pur. Un bloc-maître neuf (§10) est initialisé de la même façon, par son adresse relue après `NewHandle`.

  Allocation dans un bloc disposant de la place nécessaire :
  ```text
  SI premier_slot_libre ≠ GESTOBJ_SLOT_AUCUN :
      i ← premier_slot_libre
      premier_slot_libre ← slots[i].decalage      /* chaînage, cf. §5 */
  SINON :
      i ← nb_slots ; nb_slots ← nb_slots + 1
  limite_donnees ← limite_donnees − L
  slots[i].decalage ← limite_donnees
  slots[i].longueur ← L
  nb_occupes ← nb_occupes + 1
  retourner GESTOBJ_FAIRE_HANDLE(indice_de_b, i)
  ```
  Si aucun bloc actif ne convient (y compris après compaction de chacun), acquérir un nouveau bloc-maître (§10) et y allouer directement, celui-ci étant par construction vide donc toujours capable de satisfaire une demande ne dépassant pas le seuil des objets hors-norme.
- Alternatives envisagées :
  1. Meilleur ajustement (*best-fit*) parmi tous les blocs, en comparant systématiquement l'espace libre de chacun avant de choisir — écarté : exige de verrouiller (ou de mettre en cache, ce qui duplique l'information déjà présente dans l'en-tête) tous les blocs candidats avant de décider, pour un gain incertain sur un nombre de blocs par programme resté modeste dans les scénarios du corpus historique (quelques dizaines) ; le premier bloc convenant est déjà presque toujours un choix raisonnable à cette échelle.
  2. Maintenir en mémoire résidente (hors des blocs, donc sans verrouillage nécessaire) une copie des champs `limite_donnees`/`octets_libres`/`nb_slots` de chaque bloc, pour choisir un candidat sans le verrouiller au préalable — écarté : introduirait une duplication de la même information à deux endroits (l'en-tête du bloc et cette copie), avec un risque de désynchronisation à chaque opération, pour un bénéfice de performance non mesuré (Architecture physique IMP-0132 a déjà chiffré le coût du verrouillage à « quelques cycles 65816 », jugé acceptable) ; le conditionnement §3 place la simplicité (priorité 6) avant l'économie de performance.
- Justification : le premier-bloc-convenant est l'algorithme le plus simple compatible avec la contrainte de verrouillage/déverrouillage déjà retenue par Architecture physique IMP-0132, et sa correction ne dépend d'aucune hypothèse non vérifiée sur la distribution des tailles d'objets. Le recyclage prioritaire des slots libres avant extension de `nb_slots` limite la croissance de la table de slots aux seuls cas où aucun slot déjà réservé n'est disponible, ce qui retarde autant que possible l'épuisement de la capacité fixe de 64 slots (IMP-0158).
- Impact mémoire : aucune structure supplémentaire ; réutilisation directe de l'en-tête déjà défini (§5).
- Impact disque : sans objet.
- Impact performances : recherche linéaire dans le nombre de blocs actifs (attendu modeste, quelques dizaines) ; verrouillage/déverrouillage borné par bloc examiné ; compaction déclenchée seulement à l'échec du cas contigu, jamais systématiquement (§8).
- Impact historique : neutre.
- Références normatives : Tome III §2.3.11 (« détection de l'absence de ressource disponible », réalisée par l'échec de ce balayage, §11) ; Architecture physique IMP-0132 ; conditionnement §3, §4.1.
- Statut : retenu.

---

## 7. Algorithme de libération et recyclage des slots

Libérer un objet (`objets_liberer`, §11) consiste à marquer son slot comme libre et à décider si les octets qu'il occupait sont immédiatement récupérés ou seulement comptabilisés comme fragmentation :

```text
objets_liberer(h) :
    (b, i) ← décoder h
    lire l'adresse de b dans son pointeur maître, sans verrou (IMP-0243)
    L ← slots[i].longueur ; D ← slots[i].decalage
    SI D = limite_donnees :          /* l'objet est le plus récemment alloué encore vivant */
        limite_donnees ← limite_donnees + L        /* récupération immédiate, sans trou */
    SINON :
        octets_libres ← octets_libres + L           /* trou, récupérable seulement par compaction, §8 */
    slots[i].longueur ← 0
    slots[i].decalage ← premier_slot_libre
    premier_slot_libre ← i
    nb_occupes ← nb_occupes − 1
    SI nb_occupes = 0 :
        libérer le bloc entier (§10, IMP-0163)
```

La mise à jour de l'en-tête ne fait aucun appel système ni toolbox ; `DisposeHandle`, qui libère un bloc devenu vide, vient après le dernier accès à l'en-tête.

Ce mécanisme ne suppose aucun ordre de libération particulier : un objet libéré alors qu'il n'est pas le plus récent du bloc laisse un trou comptabilisé, sans jamais corrompre la table de slots ni les objets voisins — seule une compaction ultérieure (§8) le résorbe.

---

## 8. Algorithme de compaction interne

**IMP-0160** — *Compaction déclenchée uniquement sur échec d'allocation localisée à un bloc, jamais systématiquement à chaque libération*
- Sujet : quand et comment défragmenter la zone de données d'un bloc-maître dont les trous cumulés (`octets_libres`) suffiraient à satisfaire une demande que la seule zone contiguë ne peut plus satisfaire ?
- Choix : la compaction n'est jamais déclenchée par une libération (§7 ne fait que comptabiliser le trou) ; elle n'est déclenchée que lorsque l'algorithme d'allocation (§6, étape 3) rencontre, pour le bloc examiné, une demande que la zone contiguë ne satisfait pas mais que `octets_libres` suffirait à satisfaire. L'algorithme, exécuté sur un bloc déjà verrouillé :
  ```text
  compacter(b) :
      trier les slots occupés de b par decalage décroissant
          (c'est-à-dire du plus proche de la fin du bloc vers limite_donnees)
      nouvelle_limite ← GESTOBJ_TAILLE_BLOC
      POUR chaque slot i occupé, dans cet ordre :
          nouvelle_limite ← nouvelle_limite − slots[i].longueur
          SI slots[i].decalage ≠ nouvelle_limite :
              déplacer (memmove) les slots[i].longueur octets depuis
                  (base_bloc + slots[i].decalage) vers (base_bloc + nouvelle_limite)
              slots[i].decalage ← nouvelle_limite
      limite_donnees ← nouvelle_limite
      octets_libres ← 0
  ```
  Le tri porte sur au plus 64 éléments (IMP-0158) ; un tri par insertion, trivial à écrire et à vérifier sur 65816, suffit à ce volume.
- Alternatives envisagées :
  1. Compaction systématique à chaque libération (tenir le bloc permanemment tassé) — écarté : imposerait un coût de déplacement à chaque libération, y compris lorsque le bloc dispose déjà de la place nécessaire pour la prochaine création sans jamais être compacté ; contredit le conditionnement §27 (« les optimisations prématurées sont secondaires »), qui s'applique ici à rebours (éviter un travail non nécessaire) autant qu'à l'économie de mémoire.
  2. Compaction incrémentale (déplacer un seul objet par appel, répartir le coût sur plusieurs opérations) — écarté pour la première version : complexifie l'algorithme et son test (état intermédiaire à maintenir entre deux appels) pour un gain de latence non mesuré ; à reconsidérer si une mesure sur cible (Architecture physique IMP-0134) révèle un coût de compaction perceptible dans le dialogue conversationnel.
- Justification : le déclenchement paresseux limite le coût de la compaction aux seuls cas où elle est réellement nécessaire pour satisfaire une demande, conformément à Tome III §2.3.11 (« la stratégie interne de gestion de cet espace n'est pas imposée ») et au conditionnement §27. Le déplacement physique des octets ne modifie que le champ `decalage` du slot concerné, jamais l'indice de slot ni l'indice de bloc qui composent `HandleObjet` (§4) : aucune référence externe au bloc n'est donc invalidée, conformément à RCI-M §3 (« stabilité de l'identifiant `HandleObjet` à travers une compaction »).
- Impact mémoire : nul — la compaction ne modifie que l'agencement interne d'un bloc déjà alloué.
- Impact disque : sans objet.
- Impact performances : O(n) déplacements au plus (n ≤ 64), plus un tri O(n²) au plus sur le même volume borné ; déclenché au plus une fois par échec local d'allocation, jamais en dehors de ce cas.
- Impact historique : neutre — aucun comportement L.S.E. observable n'en dépend (RCI-M §3, §9 du conditionnement : « le déplacement physique d'un bloc par le gestionnaire d'objets ne doit modifier aucune référence logique », déjà vérifié comme test de représentation RCI-M §14).
- Références normatives : RCI-M §3 (note de dépendance), §9 du conditionnement (interdiction des pointeurs C comme modèle architectural) ; Tome III §2.3.11 ; conditionnement §27.
- Statut : retenu.

**Discipline de verrouillage pendant la compaction.** La compaction d'un bloc déplace physiquement les octets d'objets vivants de ce même bloc. Elle ne peut donc être exécutée en sécurité que si aucun appelant ne détient, au moment de l'appel, un pointeur obtenu par `objets_resoudre` (§11) sur un objet de **ce même bloc** au-delà de la durée de l'appel qui l'a produit — exigence déjà posée de façon générale par le conditionnement §9 et rappelée au §1.4 ci-dessus, mais qui prend ici une importance concrète : une violation de cette règle par un appelant ne serait pas détectée par le gestionnaire d'objets lui-même, exactement comme une utilisation d'un pointeur C après un déplacement de bloc par le Memory Manager toolbox ne l'est pas non plus.

---

## 9. Objets hors-norme

**IMP-0161** — *Bascule directe vers un bloc Memory Manager dédié au-delà de la capacité utile d'un bloc-maître vide*
- Sujet : comment héberger un objet dont la taille dépasse ce qu'un bloc-maître pourrait offrir même entièrement vide (3833 octets, §5) ? Le cas se présente notamment pour un tableau d'exécution de grande taille (EXI-0040) — les séquences RCI-M elles-mêmes, bornées par la longueur maximale d'une ligne (RCI-M IMP-0125), n'atteignent jamais ce seuil.
- Choix : un objet de longueur `L > GESTOBJ_TAILLE_BLOC − sizeof(EnTeteBlocMaitre)` (3833 octets) ne passe jamais par la sous-allocation des §5 à §8. Il obtient directement, via `plateforme_bloc_allouer(L)`, un bloc Memory Manager qui lui est intégralement dédié, sans en-tête ni table de slots internes — le bloc entier est l'objet. Une table séparée, de capacité fixe (255 entrées, même largeur d'indexation que l'octet bas de `HandleObjet`), le référence :
  ```c
  typedef struct {
      HandlePlateforme handle;     /* NULL si l'entrée n'est pas utilisée */
      unsigned long    longueur;   /* longueur en octets de l'objet ; peut dépasser 65535 */
  } DescripteurObjetHorsNorme;

  static DescripteurObjetHorsNorme table_hors_norme[GESTOBJ_NB_BLOCS_MAX];  /* 255 entrées, §4 */
  ```
  Le `HandleObjet` correspondant s'obtient par `GESTOBJ_FAIRE_HANDLE(GESTOBJ_INDICE_HORS_NORME, i)`, où `i` est l'indice retenu dans `table_hors_norme`. `objets_resoudre`/`objets_relacher` (§11) verrouillent/déverrouillent alors directement le `HandlePlateforme` dédié, sans passer par le mécanisme de slot des §5 à §8, et `objets_liberer` appelle directement `plateforme_bloc_liberer` sur ce même handle.
- Alternatives envisagées :
  1. Autoriser un objet à s'étendre sur plusieurs blocs-maîtres chaînés — écarté : romprait la propriété centrale du bloc-maître (un objet occupe une plage contiguë d'octets d'un seul bloc, condition nécessaire à la fois pour `objets_resoudre` — qui doit rendre un unique pointeur contigu — et pour la compaction, §8) ; introduirait par ailleurs une distinction physique entre « petit objet » et « objet chaîné » que RCI-M et l'état d'exécution n'ont jamais eu besoin de connaître.
  2. Relever la taille du bloc-maître (par exemple 16 ou 64 Kio) pour repousser le seuil de bascule — écarté : contredit la granularité retenue par cohérence avec le sous-allocateur `malloc` d'ORCA/C (§2) et aggraverait le coût fixe de l'en-tête (§5) pour la grande majorité des objets, qui restent petits (séquences RCI-M, variables scalaires déjà exclues de ce mécanisme par RCI-M IMP-0110).
- Justification : ce seuil n'est pas une valeur arbitraire distincte — il se déduit directement de la taille de bloc déjà retenue (§2) et du format d'en-tête déjà fixé (§5), sans introduire de troisième paramètre à justifier séparément. Un objet hors-norme obtient exactement le comportement que Architecture physique IMP-0132 décrit déjà pour toute demande de plus de 4096 octets par `malloc` sur cette plateforme (« devient une requête unique auprès du Memory Manager ») : la présente décision applique la même logique à `HandleObjet`.
- Impact mémoire : aucun surcoût d'en-tête pour ces objets (contrairement aux objets sous-alloués, §5) ; coût de la table `table_hors_norme` elle-même : 255 × (4 + 4) = 2040 octets, alloués une seule fois, indépendamment du nombre d'objets hors-norme réellement créés.
- Impact disque : sans objet.
- Impact performances : un seul verrouillage/déverrouillage par accès, identique au cas d'un bloc-maître ordinaire ; aucune recherche de place, aucune compaction (le bloc appartient en entier à l'objet).
- Impact historique : neutre.
- Références normatives : EXI-0040, EXI-0050 ; Architecture physique IMP-0132 ; RCI-M IMP-0125 (longueur maximale d'une ligne, justifiant que ce cas ne concerne jamais une séquence RCI-M).
- Statut : retenu, seuil (3833 octets) dérivé mécaniquement de §5 et non révisable indépendamment de celui-ci.

---

## 10. Table des blocs-maîtres et cycle de vie d'un bloc

**IMP-0162** — *Table des blocs-maîtres à trous ; un indice de bloc actif n'est jamais réaffecté*
- Sujet : comment maintenir, en mémoire résidente (hors des blocs eux-mêmes, donc sans verrouillage nécessaire pour la seule recherche d'un indice disponible), la liste des blocs-maîtres actuellement actifs, sans jamais invalider un `HandleObjet` en circulation ?
- Choix :
  ```c
  #define GESTOBJ_NB_BLOCS_MAX 255   /* indices 0..254 utilisables, cf. §4 */

  static HandlePlateforme table_blocs[GESTOBJ_NB_BLOCS_MAX];  /* NULL = indice inactif */
  static unsigned char    plafond_blocs = 0;  /* nombre d'indices déjà attribués au moins une fois */
  ```
  L'acquisition d'un nouveau bloc-maître (§6, cas d'échec du balayage) cherche d'abord un indice `i < plafond_blocs` tel que `table_blocs[i] == NULL` (un « trou » laissé par un bloc précédemment libéré, §10 ci-dessous) ; à défaut, si `plafond_blocs < GESTOBJ_NB_BLOCS_MAX`, elle utilise l'indice `plafond_blocs` et l'incrémente. Si les deux échouent, la capacité de 255 blocs-maîtres est atteinte : `objets_creer` signale l'absence de ressource disponible (Tome III §2.3.11, réalisée par le mécanisme du gestionnaire des erreurs, hors périmètre de ce chapitre). Un indice, une fois attribué à un bloc actif, **n'est jamais réaffecté à un autre bloc physique tant que ce bloc reste actif** — le libérer (§10 ci-dessous) laisse un trou (`NULL`) à réutiliser plus tard pour un bloc différent, mais ne déplace jamais le contenu d'un autre bloc actif vers cet indice.
- Alternatives envisagées :
  1. Tasser la table à chaque libération de bloc (déplacer le dernier bloc actif vers l'indice libéré, pour garder la table dense) — écarté : romprait l'invariant central du présent chapitre — un indice de bloc fait partie intégrante de tout `HandleObjet` en circulation référençant un objet de ce bloc ; le déplacement du dernier bloc actif vers un autre indice invaliderait silencieusement tous les `HandleObjet` de ses objets encore vivants.
  2. Table croissante uniquement (jamais de trou, jamais de réutilisation d'indice avant `plafond_blocs = 255`) — écarté : gaspillerait des indices définitivement après quelques cycles de création/libération de gros ensembles d'objets (par exemple des tableaux temporaires créés et libérés en boucle), ce qui contredirait EXI-0050 (« sans imposer une allocation permanente pendant toute la durée de la session ») appliqué ici à la ressource « indice de bloc » elle-même.
- Justification : la recherche de trou avant extension du plafond concilie la contrainte d'immuabilité des indices actifs (obligatoire) avec la réutilisation de la ressource « indice » une fois un bloc entièrement libéré (souhaitable, EXI-0050) — sans jamais nécessiter de renuméroter quoi que ce soit.
- Impact mémoire : 255 × 4 octets (un pointeur `HandlePlateforme`) = 1020 octets, alloués une seule fois au démarrage — cohérent avec Architecture physique IMP-0133 (« seules les structures de taille fixe et petite sont allouées » au démarrage).
- Impact disque : sans objet.
- Impact performances : recherche de trou en O(plafond_blocs) au pire, bornée par 255 ; négligeable face au coût de l'allocation Memory Manager elle-même qui suit en cas de trou non trouvé.
- Impact historique : neutre.
- Références normatives : EXI-0050 ; Architecture physique IMP-0132, IMP-0133 ; §4 (encodage `HandleObjet`, immuabilité de l'octet de bloc).
- Statut : retenu.

**IMP-0163** — *Libération immédiate d'un bloc-maître devenu entièrement vide*
- Sujet : lorsque `nb_occupes` d'un bloc-maître atteint zéro (§7), faut-il conserver ce bloc en réserve pour une prochaine création, ou le restituer immédiatement au système ?
- Choix : restitution immédiate. Dès que `nb_occupes = 0` après une libération, le bloc est rendu au système (`plateforme_bloc_liberer`) et son entrée dans `table_blocs` redevient `NULL` (un trou, réutilisable par IMP-0162).
- Alternatives envisagées :
  1. Conserver un petit nombre de blocs vides en réserve (par exemple les deux derniers devenus vides), pour éviter d'aller-retour immédiat entre le Memory Manager et le gestionnaire d'objets lors d'une séquence de créations/libérations à la frontière d'un bloc — écarté pour la première version : ajoute un état supplémentaire à maintenir et à tester (combien de blocs garder en réserve, quand les restituer réellement) pour un gain de performance non mesuré ; conditionnement §27 place explicitement les optimisations de ce type comme secondaires pour la première version.
- Justification : cohérent avec Architecture physique IMP-0133 (« aucune réservation mémoire globale au démarrage ») étendu au fil de la session — le gestionnaire d'objets ne retient jamais plus de mémoire que ce que les objets actuellement vivants requièrent — et avec EXI-0050 (allocation et libération des objets d'exécution « sans imposer une allocation permanente pendant toute la durée de la session »).
- Impact mémoire : favorable — aucune mémoire retenue au-delà du strict besoin courant ; permet notamment à GS/OS de réutiliser l'espace libéré pour d'autres besoins du système pendant une session longue (Architecture physique §4, espace « morcelé »).
- Impact disque : sans objet.
- Impact performances : un aller-retour Memory Manager (`NewHandle`/`DisposeHandle`) à chaque franchissement de la frontière vide/non-vide d'un bloc, dans les scénarios défavorables (création/libération répétée d'objets au voisinage exact de la capacité d'un seul bloc) — limitation explicitement reconnue (§16), à réévaluer par mesure sur corpus si elle se révèle perceptible dans le dialogue conversationnel.
- Impact historique : neutre.
- Références normatives : EXI-0050 ; Architecture physique IMP-0133 ; conditionnement §27.
- Statut : retenu, politique simple, à réévaluer sans modification du format ni de l'encodage si la mesure sur cible le justifie. Révisé le 2026-10-30 (Éric ; audit du code, AP-01) : parcours sans verrouillage (étapes 1, 2 et 4).

---

## 11. Interface exposée au reste du noyau

Réalisation directe des cinq opérations logiques de Tome III §2.5.8 :

```c
/* noyau/objets.h — s'appuie uniquement sur interfaces/interface_memoire.h,
   jamais directement sur MEMORY.H (cf. Organisation du dépôt IMP-0136) */

HandleObjet    objets_creer(unsigned long longueur_octets);   /* « créer un objet » */
unsigned char *objets_resoudre(HandleObjet h);                 /* « obtenir un objet » — verrouille ;
                                                                    valable jusqu'à l'appel correspondant
                                                                    à objets_relacher, jamais au-delà */
void           objets_relacher(HandleObjet h);                 /* fin de la résolution temporaire */
int            objets_copier(HandleObjet h, unsigned long debut,
                             unsigned char *dest, unsigned int n); /* copie depuis l'objet, sans verrouillage (IMP-0243) */
int            objets_ecrire(HandleObjet h, unsigned long debut,
                             const unsigned char *src, unsigned int n); /* copie vers l'objet, sans verrouillage (IMP-0243) */
unsigned long  objets_longueur(HandleObjet h);                /* longueur allouée, sans verrouillage ;
                                                                    0 pour un handle invalide (GARER) */
void           objets_liberer(HandleObjet h);                  /* « libérer un objet » */
int            objets_existe(HandleObjet h);                   /* « vérifier l'existence d'un objet » */
unsigned long  objets_capacite_disponible(void);                /* « vérifier la capacité disponible » */
void           objets_terminer(void);                           /* fin de session : tous les blocs rendus (IMP-0270) */
```

`objets_creer` répartit entre §6 (longueur ≤ 3833 octets) et §9 (longueur supérieure), et renvoie `GESTOBJ_HANDLE_NUL` en cas d'échec (capacité de 255 blocs atteinte, ou échec de `plateforme_bloc_allouer` signalant l'absence de ressource — Tome III §2.3.11 ; la traduction de cet échec en diagnostic L.S.E. relève du gestionnaire des erreurs, hors périmètre de ce chapitre).

`objets_resoudre`/`objets_relacher` décodent `h` (§4), contrôlent son existence sans verrou (`objets_existe`), puis verrouillent/déverrouillent soit le bloc-maître désigné (retournant `base_bloc + slots[GESTOBJ_SLOT(h)].decalage`), soit, pour un objet hors-norme, le `HandlePlateforme` dédié directement. Ces deux appels doivent être strictement appariés et ne jamais laisser une résolution ouverte à travers un appel à `objets_creer`, `objets_liberer` ou toute opération susceptible de déclencher une compaction (§1.4, §8) — y compris lorsque les deux objets concernés appartiennent à des blocs différents, par simple discipline uniforme.

**Verrouillage non compté.** `HLock` et `HUnlock` agissent sur l'attribut `attrLocked` du bloc (bit 15) sans tenir de compte : un seul `HUnlock` déverrouille le bloc, quel que soit le nombre de `HLock` qui l'ont précédé (Apple IIGS Toolbox Reference, vol. 1, p. 12-30, 12-32). Le verrouillage du gestionnaire d'objets porte sur le bloc-maître, qui héberge jusqu'à 64 objets. Lorsque deux objets d'un même bloc sont résolus en même temps, le relâchement du premier déverrouille le bloc pendant que l'autre adresse est encore en usage. Ce cas se présente dans `donnees.c` (copie d'un objet, comparaison de chaînes, fonctions de LSE15 §4-4.2 qui lisent deux chaînes) : après le premier relâchement, la seconde adresse ne sert plus qu'à des copies et des comparaisons d'octets. Il ne produit pas d'erreur, parce que le Memory Manager ne déplace un bloc déverrouillé que pendant un appel système ou toolbox, et qu'entre la lecture d'une adresse et son dernier usage seuls des `HLock` et des `HUnlock` ont lieu, qui ne déplacent rien (vol. 1, p. 12-5). La règle qui garantit la validité d'une adresse résolue est donc la suivante : **entre l'obtention d'une adresse et son dernier usage, aucun appel système ni toolbox autre que `HLock` et `HUnlock` n'a lieu**, ce qui exclut en particulier `objets_creer`, `objets_liberer`, toute entrée-sortie (console, GS/OS), `malloc` et le calcul flottant, qui passe par SANE. Le verrouillage reste nécessaire pour l'objet seul résolu dans son bloc, quand sa fenêtre contient un tel appel (une allocation dans `decoder_declaration`, les réels écrits par le décompilateur pendant la restitution d'une ligne) ; la règle s'applique dans tous les cas.

Le décompilateur résolvait le texte d'une constante partagée pendant la restitution d'une ligne, puis le relâchait : quand les deux objets partageaient un bloc, la ligne restait déverrouillée pendant l'écriture des réels suivants, donc à travers des appels SANE. Il copie désormais ce texte par `objets_copier` (audit du code, AC-33). `objets_existe`, `objets_copier` et `objets_ecrire` ne verrouillent rien et ne déverrouillent donc aucun autre objet.

`objets_existe(h)` renvoie faux pour `GESTOBJ_HANDLE_NUL` ; pour toute autre valeur, il effectue un contrôle défensif de cohérence (indices dans les bornes actives, slot effectivement occupé) mais ne constitue **pas** une protection contre la réutilisation d'un `HandleObjet` devenu obsolète après une libération : comme pour tout schéma à base de handle — y compris celui du Memory Manager toolbox lui-même — un `HandleObjet` ne doit plus être lu par quiconque une fois que le seul pointeur logique qui le portait (par exemple `EntreeObjetCompile.bloc`, RCI-M §5) a été réaffecté ou effacé ; cette discipline appartient à l'appelant, pas à ce chapitre (§16).

`objets_capacite_disponible` retourne `plateforme_memoire_disponible()` (Architecture physique IMP-0134) sans transformation. La valeur est indicative : seul l'échec de `objets_creer` établit qu'une ressource manque, le Memory Manager ayant alors épuisé compactage et purge (IMP-0134) ; la décision d'employer cette valeur pour une opération donnée appartient à l'appelant.

```c
/* interfaces/interface_memoire.h — contrat entre noyau/objets.c et plateforme/memoire_iigs.c
   (Organisation du dépôt IMP-0136) */

typedef void *HandlePlateforme;  /* opaque pour noyau/objets.c, jamais interprété au-delà de sa transmission */

HandlePlateforme plateforme_bloc_allouer(unsigned long taille_octets);   /* NewHandle */
void             plateforme_bloc_liberer(HandlePlateforme h);            /* DisposeHandle */
unsigned char   *plateforme_bloc_verrouiller(HandlePlateforme h);        /* HLock, résout en pointeur */
void             plateforme_bloc_deverrouiller(HandlePlateforme h);      /* HUnlock */
unsigned long    plateforme_memoire_disponible(void);                   /* interrogation Memory Manager */
unsigned char   *plateforme_bloc_adresse(HandlePlateforme h);            /* pointeur maître, sans HLock (IMP-0243) */
```

`objets_terminer` rend au système, par `plateforme_bloc_liberer`, chaque bloc-maître et chaque objet hors-norme encore attribués, remet `table_blocs`, `plafond_blocs` et `table_hors_norme` à vide, et laisse le gestionnaire dans son état initial (IMP-0270).

**IMP-0243** — *Accès aux objets sans verrouillage de leur bloc*
- Sujet : un couple `HLock`/`HUnlock` coûte environ 0,4 ms sur l'Apple IIGS en profil `cible` ; la paire `objets_resoudre` + `objets_relacher` en faisait trois, soit 1,1 ms, environ dix fois une recherche de ligne (mesures du 2026-10-25, `05-IMPLEMENTATION/audits/Mesures_performance_IIGS_2026-10-25.md`). L'interpréteur l'exécutait à chaque changement de ligne (Interpréteur IMP-0215), et les données à chaque accès à un élément de tableau ou à l'en-tête d'une chaîne ; la création et la libération d'un objet verrouillaient en outre chaque bloc parcouru (audit du code du 2026-10-29, AP-01, AP-02, AP-08, AP-09, AP-13).
- Choix :
  - le gestionnaire d'objets lit et modifie l'en-tête d'un bloc par son pointeur maître (`plateforme_bloc_adresse`), sans verrou, dans `objets_creer` (parcours, compaction, allocation, initialisation d'un bloc neuf), `objets_existe`, `objets_longueur` et `objets_liberer` ; `objets_resoudre` et `objets_relacher` contrôlent l'existence de l'objet de la même façon, puis verrouillent ou déverrouillent le bloc par un seul appel ;
  - `objets_copier(h, debut, dest, n)` copie n octets de l'objet, à partir de l'octet `debut`, vers un tampon de l'appelant ; `objets_ecrire(h, debut, src, n)` copie n octets d'un tampon dans l'objet. Toutes deux lisent l'adresse courante du bloc dans son pointeur maître, contrôlent l'existence de l'objet et la plage demandée dans l'en-tête du bloc (ou dans la table des objets hors-norme), copient, et ne conservent aucune adresse. Elles rendent 0 pour un handle nul ou libéré, ou pour une plage qui dépasse l'objet. `debut` est un `unsigned long` : un objet hors-norme peut dépasser 64 Kio ; l'adresse se calcule alors sur 32 bits ;
  - emplois : chargement d'une ligne par l'interpréteur (IMP-0215) ; en-tête d'un tableau, marques et valeur d'un élément, en-tête d'une chaîne et texte source d'une chaîne copiée, extraite ou concaténée (`donnees.c`) ; ligne d'entrée d'une procédure, copiée dans un tampon statique avant d'y mesurer l'élément `0xCD` (`contexte_exec.c`) ; premier octet d'une ligne insérée, le bloc n'étant résolu que pour une déclaration de procédure (`programme.c`) ; texte d'une constante partagée restituée par le décompilateur ; image RCI-S (`bibliotheque.c`). Les autres accès restent sur `objets_resoudre` et `objets_relacher`, en particulier ceux dont la fenêtre contient une allocation ou un calcul flottant.
- Condition de validité : un bloc déverrouillé peut être lu par son pointeur maître tant qu'aucun appel système ou toolbox n'a lieu entre la lecture et l'usage ; ce sont ces appels qui peuvent provoquer un compactage, et le compactage ne survient pas de manière inattendue (Apple IIGS Toolbox Reference, vol. 1, p. 12-5). Dans chacun des emplois ci-dessus, aucun appel de ce genre n'a lieu entre la lecture du pointeur maître et le dernier accès : copie par `memcpy`, calcul d'en-tête, compaction par `memmove`. La règle du §1 est respectée : l'adresse ne vit que le temps de l'opération. Toute modification de ces fonctions doit préserver cette condition (commentaire en tête de l'interface dans `objets.c`).
- Alternatives envisagées : garder les blocs des lignes verrouillés en permanence — écartée : des blocs fixes fragmentent la mémoire du Memory Manager (IMP-0132) ; supprimer aussi le verrou de `objets_resoudre` — écartée : certaines fenêtres de résolution contiennent une allocation (`decoder_declaration`) ou un calcul flottant (restitution d'une ligne), et un manquement à la règle ne se verrait que sur l'Apple IIGS, mémoire sollicitée, jamais sous Golden Gate (audit du code, §9 point 8, option c).
- Justification : mesures du 2026-10-25 ; Architecture physique IMP-0132 ; choix d'Éric du 2026-10-25 (copie d'une ligne) et du 2026-10-30 (extension, option b du point 8 de l'audit du code).
- Impact mémoire : un tampon statique de 255 octets dans `contexte_exec.c` et un autre dans `decompilateur.c`. Impact disque : sans objet. Impact performances : environ 1,1 ms gagnée par changement de ligne ; une résolution et son relâchement passent de six appels toolbox à deux, soit 0,467 ms au lieu de 1,133 ms (sonde `PERF` sur l'Apple IIGS, 2026-10-30) ; `objets_creer` et `objets_liberer` n'en font plus, hors `NewHandle` et `DisposeHandle` d'un bloc entier ; la lecture ou l'écriture d'un élément de tableau n'en fait plus (estimation de l'audit : environ 2 ms gagnées par accès ; à mesurer sur l'Apple IIGS).
- Impact historique : nul.
- Interruptions : un appel toolbox fait par un gestionnaire d'interruption ne peut pas forcer un compactage (vol. 1, p. 12-5 ; `CompactMem` est sans effet sous interruption, p. 12-22) ; une interruption survenant pendant la copie ne déplace donc pas le bloc.
- Références normatives : Architecture physique IMP-0132 ; présent chapitre §1, §11 ; Interpréteur IMP-0215 ; Apple IIGS Toolbox Reference, vol. 1, p. 12-5, 12-22.
- Statut : retenu (Éric, 2026-10-25) ; étendu le 2026-10-30 (Éric ; audit du code, AP-01, AP-02 b, AP-08, AP-09, AP-13, AC-33).

**Réalisation (2026-10-22, validée par Éric ; complétée le 2026-10-29).** `plateforme_bloc_allouer` appelle `NewHandle(taille, userid(), attributs, NULL)` et renvoie `NULL` si `toolerror()` est non nul. Les attributs sont `attrNoPurge` et `attrNoSpec` (IMP-0271), plus `attrNoCross` pour un bloc de 64 Kio au plus : un objet n'y franchit jamais une frontière de banque du 65816. Au-delà de 64 Kio, `attrNoCross` rendrait l'allocation impossible (GS-20 *Toolbox C*, leçon sur `NewHandle`). Un objet hors-norme de plus de 64 Kio ne bénéficie donc pas de cette garantie, point à traiter avec les grands tableaux (module `donnees`). `locationPtr` vaut `NULL`, le Memory Manager l'ignorant sans `attrAddr` ni `attrBank` (vol. 1, p. 12-35). `plateforme_memoire_disponible` appelle `RealFreeMem`, qui compte aussi la mémoire récupérable par purge (Architecture physique IMP-0134) ; Golden Gate ne l'implémente pas (Construction IMP-0192) : elle ne s'exécute que sur cible.

`plateforme/memoire_iigs.c` est seul autorisé à inclure `MEMORY.H` (Organisation du dépôt §2, IMP-0136). Ses appels ont été confrontés le 2026-10-29 au chapitre 12 de l'*Apple IIGS Toolbox Reference*, vol. 1 (`03-APPLE-IIGS/manuels/Apple IIGS Toolbox Reference Volume 1 (glissés).pdf`), et au chapitre 36 du vol. 3 (`03-APPLE-IIGS/manuels/apple_iigs_toolbox_reference_volume_3 (glissés).pdf`) :

| Fonction | Appel | Référence | Paramètres |
|---|---|---|---|
| `plateforme_bloc_allouer` | `NewHandle` ($0902) | vol. 1, p. 12-35 à 12-37, tableau 12-4 | `blockSize` LongWord, `userID` Word, `attributes` Word, `locationPtr` Pointer |
| `plateforme_bloc_liberer` | `DisposeHandle` ($1002) | vol. 1, p. 12-24 | `Handle` ; libère le bloc même verrouillé |
| `plateforme_bloc_verrouiller` | `HLock` ($2002), puis lecture du pointeur maître | vol. 1, p. 12-5, 12-30 | `Handle` |
| `plateforme_bloc_deverrouiller` | `HUnlock` ($2202) | vol. 1, p. 12-32 | `Handle` |
| `plateforme_bloc_adresse` | lecture du pointeur maître | vol. 1, p. 12-5 | — |
| `plateforme_memoire_disponible` | `RealFreeMem` ($2F02) | vol. 3, p. 36-10 | résultat LongWord |

Le Memory Manager est démarré et arrêté par la bibliothèque d'exécution d'ORCA/C, non par l'application : `MMStartUp` (vol. 1, p. 12-17) est appelé au lancement de tout programme C (manuel ORCA/C 2.2, chapitre des bibliothèques, `startdesk` : « The Tool Locater, Memory Manager, Loader and SANE are started by all C programs ») et l'arrêt relève de `SystemMMShutDown`, « called just before a program exits » (même manuel, *System Functions*). Un appel à `MMShutDown` depuis `main` serait nuisible, la bibliothèque employant encore le Memory Manager après le retour de `main`. `userid()` rend l'identifiant maître combiné avec `0x0100`, soit un auxID égal à 1 (GS-20 *Toolbox C*, leçon sur `NewHandle`) : c'est la technique recommandée par le vol. 1 (p. 12-11), qui proscrit l'auxID 0.

Le vol. 3 (p. 36-3) précise l'ordre des recours du Memory Manager quand une allocation ne peut pas être satisfaite d'emblée : routines de la file *out-of-memory*, compactage, purge des niveaux 3, 2 puis 1, de nouveau la file ; un échec de `NewHandle` (`memErr`, $0201) signifie que tous ces recours ont été épuisés. Les blocs du gestionnaire d'objets ne sont pas purgeables (`attrNoPurge`) ; `RestoreHandle`, `ReAllocHandle`, `SetPurge` et `CompactMem` ne sont pas employés, et aucune routine *out-of-memory* n'est installée.

**IMP-0270** — *Restitution de tous les blocs en fin de session*
- Sujet : le vol. 1 de l'*Apple IIGS Toolbox Reference* exige qu'une application rende toute la mémoire qu'elle a obtenue avant de se terminer (p. 12-14, *Cleaning up memory*) et avant l'arrêt du Memory Manager (p. 12-18, `MMShutDown` ; p. 12-23, `DisposeAll`). Les blocs-maîtres et les objets hors-norme encore attribués à la fin d'une session n'étaient rendus par aucun code.
- Choix : une opération `objets_terminer` rend chaque bloc encore attribué par `DisposeHandle`, un par un, en parcourant `table_blocs` jusqu'à `plafond_blocs` puis `table_hors_norme`, et remet ces tables à vide. `main` l'appelle après `console_fermer`, à la fin de la session, ainsi que sur la sortie en échec de `programme_initialiser`. Le gestionnaire peut être réemployé ensuite comme au premier jour.
- Alternatives envisagées :
  1. Allouer sous un auxID propre (`(userid() & 0xF0FF) | 0x0200`) et rendre tout par un seul `DisposeAll` — écartée : repose sur la composition de l'identifiant rendu par `userid()`, fait de la bibliothèque d'ORCA/C, alors que les tables du gestionnaire connaissent déjà chaque bloc. `DisposeAll(userid())` est exclu : la bibliothèque d'ORCA/C emploie le même identifiant pour ses propres allocations, que le vol. 1 interdit de détruire ainsi (p. 12-14, 12-23).
  2. Ne rien faire, les identifiants dérivés de l'identifiant maître étant détruits avec lui à la sortie (vol. 1, p. 12-11) — écartée : contraire à la règle explicite des p. 12-14 et 12-18 ; un lanceur qui conserve le programme en mémoire pour le relancer (niveau de purge 3, p. 12-10) laisserait ces blocs occupés.
- Justification : vol. 1, p. 12-14 (« If you used the master user ID to allocate private memory, you must dispose of the private memory on a handle-by-handle basis »), p. 12-18 ; choix d'Éric du 2026-10-29.
- Impact mémoire : aucune mémoire du LSE ne survit à la session.
- Impact disque : sans objet.
- Impact performances : au plus 510 `DisposeHandle` en fin de session.
- Impact historique : neutre.
- Références normatives : EXI-0050 ; Architecture physique IMP-0132, IMP-0133 ; présent chapitre §10, §11 ; Apple IIGS Toolbox Reference, vol. 1, p. 12-11, 12-14, 12-18, 12-23, 12-24.
- Statut : retenu (Éric, 2026-10-29).

**IMP-0271** — *Blocs du gestionnaire d'objets hors de la mémoire spéciale*
- Sujet : sans `attrNoSpec`, le Memory Manager peut placer un bloc en mémoire spéciale, c'est-à-dire dans les banques $00 et $01 et dans les parties des banques $E0 et $E1 qu'il gère (vol. 1, p. 12-3, 12-4, 12-13). La banque $00 porte la page directe et la pile des outils et de GS/OS.
- Choix : `plateforme_bloc_allouer` pose toujours `attrNoSpec` ($0008). Aucun bloc du gestionnaire d'objets n'occupe la mémoire spéciale.
- Alternatives envisagées : laisser le Memory Manager libre d'y recourir en dernier ressort — écartée : la mémoire spéciale est réservée aux besoins du système et de l'émulation de l'Apple IIe ; une session longue qui l'occuperait pourrait priver GS/OS ou un outil de la mémoire de banque $00 qu'il demande.
- Justification : choix d'Éric du 2026-10-29 ; vol. 1, p. 12-3, 12-13.
- Impact mémoire : la mémoire disponible pour les objets est réduite de la part allouable des banques $00, $01, $E0 et $E1, au plus quelques centaines de Kio sur une machine de 4 Mio ; un manque de mémoire est signalé plus tôt (`ERR_E_MEMOIRE_SATUREE`) plutôt qu'aux dépens du système.
- Impact disque : sans objet.
- Impact performances : nul.
- Impact historique : neutre.
- Références normatives : Architecture physique IMP-0132, IMP-0133 ; Apple IIGS Toolbox Reference, vol. 1, p. 12-3, 12-4, 12-13, 12-47 (tableau 12-4).
- Vérification : sur l'Apple IIGS le 2026-10-29 (sonde `SONDE`, `05-IMPLEMENTATION/audits/Mesures_memoire_IIGS_2026-10-29.md`) : 256 blocs de 4 Kio alloués par `plateforme_bloc_allouer`, mot d'attributs `$0018` sur chacun, banques $2F à $3F, aucun en mémoire spéciale.
- Statut : retenu (Éric, 2026-10-29).

**IMP-0194** — *Objet de longueur nulle : un octet réservé*
- Sujet : le compilateur crée un objet de longueur nulle pour une ligne dont la séquence est vide (ligne `60 `, chapitre Compilateur §9, étape 7, `objets_creer(curseur_tampon)` avec `curseur_tampon = 0`). Or une longueur 0 dans un slot signifie « slot libre » (§5). Comment créer un tel objet ?
- Choix : `objets_creer(0)` crée un objet d'un octet. La longueur logique de l'objet reste connue de l'appelant (par exemple `EntreeObjetCompile.longueur_octets`, RCI-M §5) . `objets_longueur` (§11) rend la longueur allouée, soit 1 pour un objet créé de longueur nulle ; son seul appelant, GARER d'un tableau, ne l'emploie jamais sur un objet vide, de sorte que l'octet réservé n'est pas observable.
- Alternatives envisagées :
  1. Refuser `objets_creer(0)` — écarté : la ligne vide est attestée (INRDP75, fin de boucle `FAIRE`) et doit être acceptée.
  2. Un `HandleObjet` spécial « objet vide », sans allocation — écarté : il faudrait le traiter à part dans `objets_resoudre`, `objets_relacher`, `objets_liberer` et `objets_existe`, et réserver une valeur de l'encodage (IMP-0157).
  3. Un indicateur « occupé » distinct de la longueur dans `EntreeSlot` — écarté : il agrandirait l'en-tête de 64 octets (IMP-0158) pour ce seul cas.
- Justification : le coût est d'un octet par ligne vide, sans aucun cas particulier dans l'interface.
- Impact mémoire : un octet par objet de longueur nulle.
- Impact disque : sans objet.
- Impact performances : nul.
- Impact historique : neutre.
- Références normatives : §5, §11 ; RCI-M §5 ; chapitre Compilateur §9 ; INRDP75 (ligne vide).
- Statut : retenu (choix d'Éric du 2026-10-22).

---

## 12. Organisation mémoire physique (zones) — mise à jour

Ce paragraphe referme la ligne laissée en suspens par RCI-M §8 (« Zone des séquences compilées ... gérés par le gestionnaire d'objets — module distinct ») et par Architecture physique §10 :

| Zone | Contenu | Remarque de dimensionnement |
|---|---|---|
| Table des blocs-maîtres | `table_blocs[0..254]` (§10, IMP-0162) | 255 × 4 octets = 1020 octets, taille fixe, allouée une fois |
| Blocs-maîtres actifs | En-tête (263 octets) + zone de données (3833 octets utiles) par bloc, §5 | proportionnel au nombre de blocs réellement acquis (§6, §10) ; libérés dès qu'ils redeviennent vides (IMP-0163) |
| Table des objets hors-norme | `table_hors_norme[0..254]` (§9) | 255 × 8 octets = 2040 octets, taille fixe, allouée une fois |
| Objets hors-norme actifs | Un bloc Memory Manager dédié par objet, §9 | proportionnel au nombre et à la taille des tableaux/chaînes d'exécution de grande taille réellement créés |

Ces quatre éléments appartiennent, dans le plan de segmentation d'Architecture physique §5.2, au segment `"objets"` pour la partie `noyau/objets.c/.h`, cohérent avec la correspondance déjà établie par Organisation du dépôt §2.1 ; leur pendant `plateforme/memoire_iigs.c/.h` appartient au même segment `"objets"` (Construction §1 : les fichiers `noyau/` et `plateforme/` d'un même processeur logique partagent leur segment).

---

## 13. Correspondance avec les invariants

| Invariant / exigence | Réalisation physique correspondante |
|---|---|
| RCI-M §3 (stabilité de `HandleObjet` à travers une compaction) | La compaction (§8) ne modifie que le champ `decalage` d'un slot, jamais son indice ni l'indice de son bloc (§4) |
| RCI-M §3 (résolution temporaire sans exposition au-delà de l'appel courant) | `objets_resoudre`/`objets_relacher` appariés (§11) ; discipline explicite au §1.4 et §8 |
| EXI-0016 (compacité) | En-tête de bloc mesuré et justifié (263 octets sur 4096, §5) ; aucune information dupliquée entre l'en-tête et une structure externe (aucune table de cache n'a été introduite, §6) |
| EXI-0049 (séparation stockage logique / mémoire d'exécution) | Aucune structure de ce chapitre n'est jamais sérialisée (§12, absence de toute mention dans RCI-S) |
| EXI-0050 (allocation et libération sans allocation permanente) | Libération immédiate d'un bloc vide (IMP-0163) ; réutilisation des indices et des slots libérés (IMP-0162, §7) |
| Tome III §2.3.11 (détection de l'absence de ressource disponible) | Échec de `objets_creer` lorsque la capacité de 255 blocs est atteinte ou que `plateforme_bloc_allouer` échoue (§11) |
| Tome III §2.5.8 (le gestionnaire mémoire ne définit pas la sémantique des objets qu'il héberge) | `objets_creer`/`objets_resoudre` ne manipulent que longueur et adresse, jamais le contenu ou le type de l'objet (§11) |

---

## 14. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0157 | Encodage de `HandleObjet` : octet de bloc + octet de slot, sentinelle `0xFF` partagée | retenu |
| IMP-0158 | Table de slots de taille fixe (64 entrées) par bloc-maître | retenu, capacité à réviser par mesure |
| IMP-0159 | Allocation par premier bloc convenant, recyclage prioritaire des slots libres, compaction paresseuse | retenu |
| IMP-0160 | Compaction interne déclenchée uniquement sur échec d'allocation localisée | retenu |
| IMP-0161 | Objets hors-norme : bascule directe au-delà de 3833 octets, table dédiée | retenu, seuil dérivé mécaniquement de §5 |
| IMP-0162 | Table des blocs-maîtres à trous, indice actif jamais réaffecté | retenu |
| IMP-0163 | Libération immédiate d'un bloc-maître devenu vide | retenu, politique simple |
| IMP-0194 | Objet de longueur nulle : un octet réservé | retenu |
| IMP-0243 | Accès sans verrouillage : en-têtes de bloc, `objets_copier`, `objets_ecrire` ; lignes, tableaux, chaînes, ligne d'entrée, constantes restituées | retenu |
| IMP-0270 | `objets_terminer` : tous les blocs rendus un par un en fin de session | retenu |
| IMP-0271 | `attrNoSpec` : aucun bloc dans les banques $00, $01, $E0, $E1 | retenu |

Ce chapitre referme ainsi le point laissé ouvert par RCI-M §3/§13 et repris sans être refermé par Architecture physique §6.1 (IMP-0132)/§10 et par Organisation du dépôt IMP-0136 (localisation seule).

---

## 15. Traçabilité

```text
Tome III §2.3.11, §2.5.8 (VM-1601, VM-1602)
  → RCI-M §3 (note de dépendance), §5, IMP-0101, IMP-0108
    → Architecture physique §6.1, IMP-0132, IMP-0133, IMP-0134
      → Organisation du dépôt §2.1, IMP-0136 (localisation)
        → IMP-0157 à IMP-0163, IMP-0194, IMP-0243, IMP-0270, IMP-0271 (présent chapitre)
          → CODE : src/noyau/objets.c, src/noyau/objets.h, src/plateforme/main.c (IMP-0270),
                   src/plateforme/memoire_iigs.c, src/plateforme/memoire_iigs.h,
                   src/interfaces/interface_memoire.h
```

Ce chapitre referme trois points explicitement laissés ouverts par des chapitres antérieurs :

```text
RCI-M §3 (note de dépendance), §13
  → IMP-0157 à IMP-0163 (présent chapitre)

Architecture physique §6.1 (IMP-0132, dernier alinéa du statut), §10
  → IMP-0157 à IMP-0163 (présent chapitre)

Organisation du dépôt §2.1, IMP-0136 (« localise seulement »)
  → IMP-0157 à IMP-0163 (présent chapitre)
```

---

## 16. Limites explicites de ce chapitre

- Les appels toolbox de `plateforme/memoire_iigs.c` sont vérifiés sur le chapitre 12 du vol. 1 et le chapitre 36 du vol. 3 de l'*Apple IIGS Toolbox Reference* (§11) ; le comportement de Golden Gate n'est connu que par les suites de tests (`FreeMem` et `RealFreeMem` non implémentés, Construction IMP-0192) et a été confirmé sur l'Apple IIGS pour `attrNoSpec` le 2026-10-29 (IMP-0271 ; `05-IMPLEMENTATION/audits/Mesures_memoire_IIGS_2026-10-29.md`).
- La capacité fixe de 64 slots par bloc-maître (IMP-0158) et le seuil de bascule vers les objets hors-norme qui en découle (3833 octets, IMP-0161) ne sont validés par aucune mesure sur corpus de programmes historiques ; conditionnement §27 (« les optimisations prématurées sont secondaires ») autorise cette valeur provisoire, mais une mesure sur cible reste nécessaire avant de la considérer comme définitive.
- Ne détaille pas la traduction d'un échec de `objets_creer` (capacité atteinte, `plateforme_bloc_allouer` en échec) en diagnostic L.S.E. observable par l'utilisateur (par exemple un message d'erreur « mémoire insuffisante ») : ce point relève du gestionnaire des erreurs, chapitre distinct du Tome V, non encore rédigé.
- `objets_existe` (§11) n'offre aucune protection contre la réutilisation d'un `HandleObjet` devenu obsolète après une libération — cette discipline reste, comme pour tout schéma à base de handle, à la charge de l'appelant (gestionnaire des variables, gestionnaire des tableaux, gestionnaire des chaînes, programme courant) ; aucun mécanisme de détection (compteur de génération, par exemple) n'a été introduit, l'espace disponible dans `HandleObjet` (16 bits, déjà entièrement occupés par IMP-0157) ne le permettant pas sans revenir sur RCI-M IMP-0101.
- La politique de libération immédiate d'un bloc-maître devenu vide (IMP-0163) peut, dans un scénario défavorable de créations/libérations répétées à la frontière exacte de la capacité d'un bloc, produire des allers-retours Memory Manager répétés ; aucune mesure n'a confirmé ni infirmé le caractère perceptible de ce coût dans le dialogue conversationnel.
- Ne couvre pas l'algorithme précis de tri utilisé par la compaction (§8) au-delà du principe (tri par insertion suffisant pour 64 éléments au plus) ; son écriture effective relève de la rédaction du fichier `src/noyau/objets.c` lui-même.
- Ne couvre pas la gestion des tableaux et chaînes d'exécution elles-mêmes (validation de bornes, redéclaration, sémantique de `LIBERER`) au-delà de leur hébergement physique par ce gestionnaire : ces points relèvent du gestionnaire des variables et du gestionnaire des tableaux/chaînes (module `donnees`), chapitre distinct du Tome V, non encore rédigé (déjà signalé par État d'exécution §12).
- Ne couvre pas le cas — non rencontré dans le corpus historique examiné à ce stade — d'un objet hors-norme dont la longueur dépasserait la capacité d'un unique bloc Memory Manager telle que documentée pour cette plateforme ; ce point resterait, s'il se présentait, à instruire au niveau du gestionnaire des tableaux (module `donnees`), non de ce chapitre.

---

## 17. Tests associés

- Création de 64 objets de petite taille dans un même bloc-maître (par exemple 32 octets chacun) : vérifier qu'aucun second bloc n'est acquis avant la 65ᵉ création, et que la 65ᵉ acquiert effectivement un nouveau bloc-maître (IMP-0158, IMP-0159).
- Création puis libération immédiate du dernier objet d'un bloc, dans l'ordre inverse de leur création (libération du plus récent en premier, puis du suivant, etc.) : vérifier que `limite_donnees` est intégralement récupérée à chaque étape sans jamais incrémenter `octets_libres` (§7, cas « bord »).
- Création de trois objets A, B, C dans cet ordre (C étant le plus proche de `limite_donnees`), libération de B (le second), puis nouvelle création D de taille supérieure à l'espace laissé par B seul mais inférieure ou égale à `octets_libres` cumulé : vérifier que la compaction (§8) est déclenchée, que A et C se retrouvent physiquement contigus après compaction, que leurs `HandleObjet` respectifs restent inchangés, et que D peut alors être créé (IMP-0159, IMP-0160).
- Vérification directe de l'invariant de stabilité : après la compaction ci-dessus, `objets_resoudre` sur le `HandleObjet` de A (resp. C) renvoie un pointeur vers un contenu identique (octet à octet) à celui obtenu avant compaction, à une adresse potentiellement différente (RCI-M §3).
- Libération de tous les objets d'un bloc-maître un par un, dans un ordre quelconque : vérifier que le bloc est restitué (`plateforme_bloc_liberer` appelé) exactement lorsque `nb_occupes` atteint zéro, jamais avant, et que son indice redevient immédiatement réutilisable par une création ultérieure (IMP-0162, IMP-0163).
- Création d'un objet de longueur exactement égale à 3833 octets (seuil, §9) : vérifier qu'il est sous-alloué dans un bloc-maître ordinaire ; création d'un objet de longueur 3834 octets : vérifier qu'il bascule vers `table_hors_norme` (IMP-0161).
- Création de 255 objets hors-norme, puis tentative d'une 256ᵉ : vérifier que `objets_creer` échoue proprement (renvoie `GESTOBJ_HANDLE_NUL`) sans corrompre `table_hors_norme` ni les 255 objets déjà créés (§9, §11).
- Création de blocs-maîtres jusqu'à épuisement des 255 indices disponibles (aucun trou), puis tentative d'une création supplémentaire ne pouvant être satisfaite par aucun bloc existant même après compaction : vérifier l'échec propre de `objets_creer` (§10, §11 — réalisation de « détection de l'absence de ressource disponible », Tome III §2.3.11).
- Un objet dont le `HandleObjet` a été correctement libéré (`objets_liberer`), puis dont l'indice de slot est immédiatement recyclé par une création différente et sans rapport : vérifier que l'ancien `HandleObjet` (non réutilisé par construction ailleurs dans le noyau, cf. RCI-M IMP-0101 pour le cas des lignes remplacées) n'est plus jamais lu par aucun composant — test de discipline d'appelant plutôt que de mécanisme interne (§11, §16).
- Séquence intercalée de créations et de libérations d'objets de tailles variées dans un unique bloc-maître, suivie d'une demande dont la taille dépasse la place contiguë disponible mais pas `octets_libres` : vérifier que la compaction produit un bloc sans aucun trou résiduel (`octets_libres = 0` après l'opération) et que la somme des longueurs des objets encore vivants plus `octets_libres` avant compaction égale exactement `GESTOBJ_TAILLE_BLOC − limite_donnees` avant compaction (cohérence comptable, §7, §8).
- `objets_copier` et `objets_ecrire` (IMP-0243) : copie et écriture d'une plage, refus d'une plage qui dépasse l'objet, d'un handle nul ou libéré ; objet hors-norme de 70 000 octets écrit à cheval sur 65 536 et en fin d'objet, relu à l'identique, sans que rien ne retombe au début de l'objet (adresses calculées sur 32 bits) ; suite `composants_objets`, sections 10 bis et 10 ter.
- `objets_terminer()` après la création d'objets répartis sur deux blocs-maîtres et d'un objet hors-norme : vérifier qu'aucun de ces objets n'existe plus, et qu'une création ultérieure reprend au premier indice de bloc (IMP-0270 ; suite `composants_objets`, section 11).
- `objets_capacite_disponible()` appelé immédiatement après `BONJOUR` (aucun objet créé) : vérifier qu'il reflète directement `plateforme_memoire_disponible()` sans transformation (§11) — test de non-régression sur la frontière avec Architecture physique IMP-0134.
- Verrouillage simultané, par erreur délibérée du test (violation intentionnelle de la discipline du §1.4/§8), d'un pointeur obtenu par `objets_resoudre` sur un objet d'un bloc, suivi d'une création dans ce même bloc déclenchant une compaction : ce test documente le comportement non garanti dans ce cas (le pointeur détenu devient invalide), plutôt que de vérifier une protection qui n'existe pas — utile comme test de non-régression du danger lui-même, à ne jamais exécuter en dehors d'un banc de test contrôlé (§16).
