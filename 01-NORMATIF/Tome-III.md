# Tome III — Machine virtuelle

# Préface

Le présent tome appartient à la collection *Reconstruction historique du Langage Symbolique d'Enseignement (L.S.E.)*.

Il constitue le tome consacré à la spécification de l'architecture logique de la machine virtuelle reconstruite du L.S.E.

Le Tome I constitue la référence historique de la collection. Il établit les propriétés et les mécanismes qui peuvent être retenus à partir des sources historiques.

Le Tome II constitue la spécification fonctionnelle de la machine abstraite reconstruite. Il définit les comportements, les mécanismes abstraits et les relations fonctionnelles retenus pour satisfaire les contraintes historiques et les exigences du Catalogue des exigences.

Le présent tome constitue la spécification d'architecture logique. Il transforme les mécanismes fonctionnels définis dans le Tome II en composants cohérents, en interfaces explicites, en structures manipulées, en échanges et en invariants.

Le présent tome ne décrit aucune implémentation particulière.

Il ne dépend :

- d'aucun processeur ;
- d'aucun langage d'implémentation ;
- d'aucun système d'exploitation ;
- d'aucune représentation matérielle particulière ;
- d'aucune organisation mémoire physique particulière.

Toute référence au processeur, à la mémoire, au fichier, au périphérique ou à la console désigne ici une abstraction architecturale définie par la collection documentaire, et non une technologie d'implémentation.

Les décisions du Tome II sont normatives pour le présent tome. Le Tome III ne peut ni les contredire, ni les remplacer, ni leur attribuer un comportement différent.

Lorsqu'une propriété est déjà déterminée par le Tome II, le présent tome en définit uniquement la réalisation architecturale logique nécessaire.

Lorsqu'une organisation interne n'est pas déterminée par le Tome II, le présent tome introduit un choix d'architecture. Ce choix est identifié comme tel et limité au strict nécessaire pour rendre l'architecture cohérente et exploitable.

La distinction entre faits historiques, exigences, mécanismes abstraits et décisions d'architecture est conservée.

La traçabilité est assurée dans les deux directions.

Une décision architecturale peut être reliée :

- à une ou plusieurs contraintes historiques `HIS` ;
- à une ou plusieurs exigences `EXI` ;
- à un ou plusieurs mécanismes fonctionnels `VM`.

Inversement, chaque mécanisme architectural important doit permettre d'identifier les exigences qu'il satisfait et les mécanismes abstraits qu'il réalise.

Le modèle architectural retenu repose sur une séparation stricte entre :

- le programme courant ;
- la représentation compilée ;
- l'état d'exécution ;
- les objets d'exécution ;
- les services de compilation et de décompilation ;
- les services de conservation et de restauration ;
- le dialogue avec l'utilisateur.

Cette séparation permet de préserver les propriétés établies dans le Tome II :

- compilation indépendante des lignes ;
- programme courant constitué de représentations compilées ;
- exécution directe de la représentation compilée ;
- décompilation à partir de cette représentation ;
- distinction entre programme compilé et état d'exécution ;
- conservation d'une position d'exécution ;
- séparation des contextes d'activation ;
- allocation et libération d'objets d'exécution ;
- traitement conversationnel des demandes.

Le présent tome adopte exclusivement la terminologie normalisée de la collection.

Les termes « chaîne codée », « chaîne post-fixée », « objet compilé », « représentation compilée », « interpréteur », « décompilateur », « bibliothécaire », « cadre d'activation », « pile d'évaluation », « environnement d'exécution » et « mémoire de travail » sont employés dans leur sens défini par les tomes précédents.

La spécification est organisée selon le plan normatif suivant :

1. Objet du tome
2. Architecture générale
3. Organisation de la machine virtuelle
4. Les espaces mémoire
5. Les objets
6. Représentation interne de la Chaîne Post-Fixée
7. Les registres virtuels
8. Le moteur d'exécution
9. Le compilateur
10. Le décompilateur
11. Le bibliothécaire
12. Le gestionnaire des variables
13. Les procédures
14. Les fonctions
15. Les tableaux
16. Les chaînes
17. Les nombres
18. Les entrées-sorties
19. Le dialogue avec le moniteur
20. Les erreurs
21. Les interfaces entre composants
22. Contraintes de conformité
23. Synthèse

Les annexes définissent les éléments nécessaires à l'exploitation de la spécification d'architecture.

---

# 1. Objet du tome

## 1.1. Périmètre

Le présent tome définit l'architecture logique de la machine virtuelle reconstruite du L.S.E.

Il décrit les composants nécessaires à la réalisation des mécanismes fonctionnels définis dans le Tome II.

Il définit notamment :

- les composants de la machine virtuelle ;
- la responsabilité de chaque composant ;
- les données abstraites manipulées ;
- les interfaces entre composants ;
- les flux d'information ;
- les dépendances ;
- les invariants ;
- les conditions de création et de destruction des états ;
- les relations entre programme courant et exécution ;
- les relations entre représentation compilée et exécution ;
- les relations entre représentation compilée et décompilation ;
- les relations entre état d'exécution et mémoire de travail.

Le présent tome ne définit pas :

- le langage source lui-même ;
- la syntaxe complète du L.S.E. ;
- une organisation matérielle ;
- un format binaire historique ;
- un protocole de communication matériel ;
- une stratégie d'implantation dans un langage particulier.

## 1.2. Position dans la collection

La hiérarchie normative applicable est la suivante :

1. Conventions éditoriales ;
2. Catalogue des exigences ;
3. Tome I — Architecture historique ;
4. Tome II — Machine abstraite reconstruite ;
5. Tome III — Machine virtuelle.

Le Tome III doit donc être interprété à la lumière des documents de niveau supérieur.

Une décision prise dans le présent tome ne peut pas modifier :

- une propriété historique établie dans le Tome I ;
- une exigence du Catalogue ;
- un comportement normatif défini dans le Tome II.

Lorsqu'une ambiguïté apparaît, la décision retenue doit être celle qui préserve ces trois niveaux normatifs.

## 1.3. Finalité

La finalité du Tome III est de fournir une architecture suffisamment précise pour permettre la réalisation d'une machine virtuelle conforme sans imposer une technique particulière.

Une implémentation conforme doit pouvoir dériver de la présente spécification :

- les structures de données nécessaires ;
- les interfaces publiques des composants ;
- les séquences d'appel ;
- les règles de conservation d'état ;
- les règles d'allocation et de libération ;
- les conditions d'erreur ;
- les mécanismes de reprise ;
- les opérations de décompilation ;
- les opérations de conservation et de restauration.

La présente architecture constitue donc la frontière normative entre la spécification fonctionnelle et toute réalisation ultérieure.

## 1.4. Principe de séparation des responsabilités

Chaque responsabilité fonctionnelle est attribuée à un composant unique au niveau logique.

Un composant peut utiliser les services d'un autre composant, mais il ne doit pas reproduire sa responsabilité normative.

Ainsi :

- le compilateur produit les objets compilés ;
- le gestionnaire du programme courant conserve ces objets ;
- l'interpréteur les exécute ;
- le gestionnaire de l'environnement d'exécution résout les références nécessaires ;
- le gestionnaire des variables administre les variables d'exécution ;
- le gestionnaire mémoire administre les objets dont la durée de vie est dynamique ;
- le décompilateur transforme une représentation compilée en représentation textuelle ;
- le bibliothécaire conserve et restaure les programmes ;
- le moniteur dialogue avec l'utilisateur ;
- l'analyseur de commandes oriente les demandes ;
- le gestionnaire des erreurs normalise les diagnostics et leur propagation.

Cette séparation ne préjuge pas de la séparation physique ou des unités de programme d'une implémentation.

Elle définit uniquement les responsabilités logiques.

## 1.5. Principe d'indépendance du texte source

Le programme courant est constitué, au niveau fonctionnel, d'objets compilés.

Le texte source intervient comme représentation d'entrée lors de la compilation et comme représentation de sortie lors de la décompilation.

Le texte source n'est donc pas une dépendance obligatoire de l'exécution.

Une implémentation peut conserver temporairement ou auxiliairement le texte source, mais aucune fonction de la machine virtuelle ne doit en dépendre lorsque le Tome II prévoit son fonctionnement à partir de la représentation compilée.

Cette règle découle directement de la distinction entre :

- source ;
- représentation compilée ;
- représentation textuelle reconstruite.

## 1.6. Principe d'état explicite

La machine virtuelle distingue au minimum deux catégories d'état :

- l'état stable du programme courant ;
- l'état dynamique d'une exécution.

Le programme courant contient les représentations compilées qui définissent le programme.

L'état d'exécution contient les informations nécessaires pour exécuter ce programme à un instant donné.

Une modification du programme courant n'est donc pas assimilée à une modification de l'état d'une exécution déjà active.

Cette séparation est nécessaire pour préserver :

- l'identité du programme courant ;
- la position d'exécution ;
- les variables et objets d'exécution ;
- les cadres d'activation ;
- la possibilité de suspendre et de reprendre une exécution.

## 1.7. Principe de minimalité

La présente architecture ne définit que les composants nécessaires aux mécanismes du Tome II.

Aucun composant n'est introduit en raison de son existence dans des architectures modernes.

Tout composant nouveau doit pouvoir être justifié par :

- une contrainte historique ;
- une exigence ;
- un mécanisme du Tome II ;
- ou une dépendance architecturale strictement nécessaire.

Tout composant qui ne possède aucune justification de cette nature est hors du périmètre de la présente machine virtuelle.

## 1.8. Traçabilité

La traçabilité architecturale est maintenue à trois niveaux.

### Niveau historique

Les contraintes historiques sont référencées par les identifiants `HIS`.

### Niveau fonctionnel

Les propriétés normatives sont référencées par les identifiants `EXI`.

### Niveau abstrait

Les mécanismes fonctionnels reconstruits sont référencés par les identifiants `VM`.

Toute décision d'architecture doit donc pouvoir être exprimée sous la forme :

`HIS → EXI → VM → composant architectural`

Lorsqu'une exigence ne possède pas d'origine historique directe, le premier maillon historique peut être absent. La traçabilité vers l'exigence et le mécanisme fonctionnel reste alors obligatoire.

## 1.9. Résultat attendu

À l'issue du présent tome, une implémentation devra pouvoir être conçue sans qu'il soit nécessaire de déterminer à nouveau :

- quels composants existent ;
- quelles sont leurs responsabilités ;
- quelles données ils échangent ;
- quelles opérations ils offrent ;
- quels états ils maintiennent ;
- quelles propriétés doivent rester invariantes.

Les choix laissés ouverts concernent uniquement les aspects qui ne peuvent être déterminés sans choisir une technologie d'implémentation.

---

# 2. Architecture générale

## 2.1. Objet

L'architecture générale définit l'organisation logique de la machine virtuelle et les relations entre ses composants.

Elle transforme les mécanismes du Tome II en un ensemble cohérent de services spécialisés.

L'organisation retenue est centrée sur le programme courant et sur l'état d'exécution.

Le programme courant constitue la source stable des objets compilés.

L'état d'exécution constitue la représentation dynamique de l'exécution en cours.

Les composants de service utilisent l'un ou l'autre de ces états selon leur responsabilité.

## 2.2. Vue logique d'ensemble

La machine virtuelle est organisée en cinq ensembles fonctionnels.

### 2.2.1. Ensemble conversationnel

Il comprend :

- le moniteur ;
- l'analyseur de commandes ;
- les interfaces d'entrée et de sortie ;
- le gestionnaire des erreurs.

Cet ensemble reçoit les demandes de l'utilisateur et restitue les résultats.

### 2.2.2. Ensemble de transformation

Il comprend :

- le compilateur ;
- le décompilateur.

Le compilateur transforme une entrée textuelle validée en représentation compilée.

Le décompilateur transforme une représentation compilée en représentation textuelle.

Ces deux transformations sont distinctes mais reposent sur la même représentation compilée.

### 2.2.3. Ensemble de gestion du programme

Il comprend :

- le programme courant ;
- le bibliothécaire ;
- les mécanismes de chargement et de conservation.

Cet ensemble assure la cohérence de la collection des objets compilés.

### 2.2.4. Ensemble d'exécution

Il comprend :

- le moteur d'exécution ;
- l'interpréteur ;
- l'environnement d'exécution ;
- les cadres d'activation ;
- la pile d'évaluation ;
- les registres virtuels ;
- le contrôle du flux.

Cet ensemble réalise l'exécution de la représentation compilée.

### 2.2.5. Ensemble de gestion des objets

Il comprend :

- le gestionnaire des variables ;
- le gestionnaire des tableaux ;
- le gestionnaire des chaînes ;
- le gestionnaire des nombres ;
- le gestionnaire mémoire ;
- les services d'entrée-sortie.

Cet ensemble fournit les objets et les opérations nécessaires à l'exécution.

## 2.3. Composants principaux

L'architecture logique comporte les composants suivants.

### 2.3.1. Moniteur

Le moniteur constitue le point d'échange avec l'utilisateur.

Il fournit une entrée et une sortie abstraites.

Il ne connaît pas les détails internes de la compilation ou de l'interprétation.

### 2.3.2. Analyseur de commandes

L'analyseur de commandes détermine la nature d'une entrée validée.

Une entrée peut constituer notamment :

- une ligne de programme ;
- une commande ;
- une expression immédiate ;
- une demande adressée à un processeur spécialisé.

L'analyseur ne réalise pas lui-même la fonction finale de traitement.

Il oriente la demande vers le composant compétent.

### 2.3.3. Compilateur

Le compilateur traite les lignes destinées au programme.

Il effectue :

- l'analyse syntaxique ;
- la génération de la représentation compilée ;
- la production d'un objet compilé ;
- la production d'un diagnostic en cas d'échec.

Le compilateur ne modifie pas directement le programme courant.

Il remet son résultat au gestionnaire du programme courant.

### 2.3.4. Programme courant

Le programme courant conserve les objets compilés acceptés.

Il garantit notamment :

- l'unicité des numéros de ligne ;
- l'ordre logique des lignes ;
- la cohérence des remplacements ;
- la cohérence des suppressions ;
- la stabilité de l'ensemble après une opération acceptée.

### 2.3.5. Interpréteur

L'interpréteur exécute la représentation compilée.

Il utilise l'état d'exécution pour conserver :

- la position courante ;
- le contexte actif ;
- la pile d'évaluation ;
- les références résolues ;
- les résultats intermédiaires ;
- les conditions nécessaires à la reprise.

### 2.3.6. Moteur d'exécution

Le moteur d'exécution coordonne les opérations de l'interpréteur et des services d'exécution.

Il ne définit pas la sémantique propre de chaque objet.

Il assure le déroulement général :

1. sélection de l'élément à exécuter ;
2. lecture de l'opération codée ;
3. obtention des données nécessaires ;
4. exécution de l'opération ;
5. mise à jour de l'état d'exécution ;
6. sélection de l'élément suivant ;
7. détection d'une interruption ou d'une terminaison.

### 2.3.7. Environnement d'exécution

L'environnement d'exécution fournit le contexte dans lequel les références compilées sont résolues.

Il permet notamment d'associer :

- un identificateur à une variable ;
- une référence d'appel à une procédure ou fonction ;
- une référence d'objet à l'objet d'exécution correspondant.

### 2.3.8. Décompilateur

Le décompilateur parcourt une représentation compilée et produit une représentation textuelle déterministe.

Il n'intervient pas dans l'exécution.

Il ne modifie pas le programme courant.

### 2.3.9. Bibliothécaire

Le bibliothécaire assure la conservation et la restauration des programmes compilés.

Il manipule la représentation persistante du programme.

Il ne compile pas le programme restauré.

### 2.3.10. Gestionnaire des variables

Le gestionnaire des variables assure :

- la création logique des variables ;
- leur résolution ;
- la lecture de leur valeur ;
- la modification de leur valeur ;
- la détection des références invalides à l'exécution.

### 2.3.11. Gestionnaire mémoire

Le gestionnaire mémoire fournit l'espace abstrait nécessaire aux objets dont la durée de vie est dynamique.

Il prend notamment en charge :

- la création ;
- la réservation logique ;
- la libération ;
- la détection de l'absence de ressource disponible.

La stratégie interne de gestion de cet espace n'est pas imposée.

## 2.4. Relations entre composants

Les relations principales sont les suivantes.

### Entrée d'une ligne de programme

`Moniteur → Analyseur de commandes → Compilateur → Programme courant`

En cas d'échec :

`Compilateur → Gestionnaire des erreurs → Moniteur`

En cas de succès :

`Compilateur → Programme courant`

### Exécution d'un programme

`Moniteur → Analyseur de commandes → Moteur d'exécution → Interpréteur`

L'interpréteur utilise :

- le programme courant ;
- l'environnement d'exécution ;
- les cadres d'activation ;
- la pile d'évaluation ;
- le gestionnaire des variables ;
- le gestionnaire mémoire ;
- les services d'entrée-sortie.

### Décompilation

`Moniteur → Analyseur de commandes → Décompilateur → Moniteur`

Le décompilateur lit :

`Programme courant → Objet compilé → Représentation textuelle`

### Conservation

`Programme courant → Bibliothécaire → Représentation persistante`

### Restauration

`Bibliothécaire → Programme courant`

### Interruption

`Interpréteur → État d'exécution suspendu`

Puis :

`Moniteur → Moteur d'exécution → État d'exécution suspendu`

### Erreur d'exécution

`Composant exécutant → Gestionnaire des erreurs → Moniteur`

L'erreur ne modifie le programme courant que si le mécanisme fonctionnel concerné le prévoit explicitement. En particulier, une erreur d'exécution ne constitue pas une modification implicite du code compilé.

## 2.5. Interfaces générales

Les interfaces sont définies au niveau logique.

Aucune interface ne suppose une représentation particulière des données.

### 2.5.1. Interface de compilation

Entrée :

- ligne validée ;
- contexte nécessaire à la compilation.

Sortie en cas de succès :

- objet compilé.

Sortie en cas d'échec :

- diagnostic de compilation.

Précondition :

- la ligne est complète et validée.

Postcondition de succès :

- un objet compilé valide est produit ;
- aucun état du programme courant n'est modifié par le compilateur lui-même.

Postcondition d'échec :

- aucun objet compilé accepté n'est produit.

### 2.5.2. Interface du programme courant

Opérations logiques :

- ajouter une ligne ;
- remplacer une ligne ;
- supprimer une ligne ;
- rechercher une ligne ;
- parcourir les lignes ;
- obtenir le premier objet logique ;
- obtenir l'objet suivant ;
- vider le programme courant ;
- remplacer l'ensemble du programme courant.

Chaque opération doit préserver les invariants du programme courant.

### 2.5.3. Interface d'exécution

Entrées :

- programme courant ;
- état d'exécution initial ou suspendu ;
- mode d'exécution.

Sorties :

- état d'exécution terminé ;
- état d'exécution suspendu ;
- diagnostic d'exécution.

L'interface doit permettre :

- le démarrage ;
- l'exécution séquentielle ;
- l'interruption ;
- la reprise ;
- la terminaison.

### 2.5.4. Interface de décompilation

Entrée :

- représentation compilée.

Sortie :

- représentation textuelle.

Propriété obligatoire :

- la sortie doit être déterministe pour une même représentation compilée et un même contexte de décompilation.

### 2.5.5. Interface de conservation

Entrée :

- programme courant.

Sortie :

- représentation persistante du programme.

L'opération ne doit pas modifier le programme courant.

### 2.5.6. Interface de restauration

Entrée :

- représentation persistante valide d'un programme.

Sortie :

- nouveau programme courant.

L'opération doit produire un état cohérent et exécutable.

### 2.5.7. Interface de résolution

Entrée :

- référence compilée ;
- environnement d'exécution courant.

Sortie :

- objet d'exécution correspondant ;
- ou diagnostic.

### 2.5.8. Interface mémoire

Opérations logiques :

- créer un objet ;
- obtenir un objet ;
- libérer un objet ;
- vérifier l'existence d'un objet ;
- vérifier la capacité disponible.

Le gestionnaire mémoire ne définit pas la sémantique des objets qu'il héberge.

## 2.6. Flux de données

Les flux architecturaux sont répartis en quatre catégories.

### Flux source

Ils transportent :

- caractères ;
- lignes ;
- commandes ;
- expressions.

### Flux compilé

Ils transportent :

- objets compilés ;
- opérations codées ;
- valeurs immédiates ;
- références.

### Flux d'exécution

Ils transportent :

- valeurs ;
- références résolues ;
- contrôles ;
- états de retour ;
- diagnostics.

### Flux persistant

Ils transportent :

- programmes compilés conservés ;
- programmes compilés restaurés.

Aucun flux ne doit imposer qu'un composant connaisse la représentation interne d'un autre composant au-delà du contrat de son interface.

## 2.7. Dépendances

Les dépendances principales sont orientées comme suit.

Le moniteur dépend de l'analyseur de commandes.

L'analyseur de commandes dépend des interfaces des services qu'il sollicite.

Le compilateur dépend des règles de représentation compilée.

Le programme courant dépend de la validité des objets compilés reçus.

L'interpréteur dépend :

- du programme courant ;
- de la représentation compilée ;
- de l'environnement d'exécution ;
- des services de manipulation des objets.

Le décompilateur dépend de la représentation compilée.

Le bibliothécaire dépend du contrat de persistance des programmes.

Le gestionnaire des variables dépend de l'environnement d'exécution.

Le gestionnaire mémoire est utilisé par les composants qui créent des objets dynamiques.

Le gestionnaire des erreurs est transversal.

Aucune dépendance circulaire n'est nécessaire au niveau logique.

## 2.8. Invariants généraux

Les invariants suivants s'appliquent à l'ensemble de la machine virtuelle.

### INV-01 — Programme courant cohérent

À tout instant, le programme courant contient uniquement des objets compilés acceptés.

### INV-02 — Unicité des lignes

Un numéro de ligne ne désigne jamais simultanément deux objets compilés distincts dans le programme courant.

### INV-03 — Ordre logique

Le parcours normal du programme courant est défini par l'ordre croissant des numéros de ligne.

### INV-04 — Indépendance de l'exécution

L'état d'exécution est distinct du programme compilé.

### INV-05 — Exécution directe

L'interpréteur exécute la représentation compilée sans dépendre de la restitution préalable du texte source.

### INV-06 — Décompilation indépendante

Le décompilateur lit la représentation compilée sans modifier son contenu.

### INV-07 — Cohérence des références

Toute référence utilisée pendant l'exécution est soit résolue, soit rejetée avec un diagnostic.

### INV-08 — Cohérence des activations

Chaque cadre d'activation possède un contexte d'appel cohérent et, lorsqu'il est nécessaire, une relation explicite avec le cadre appelant.

### INV-09 — Cohérence de la position d'exécution

La position d'exécution désigne toujours un état interprétable ou un état terminal.

### INV-10 — Cohérence mémoire

Tout objet dynamique utilisé par l'exécution est soit présent dans l'espace mémoire d'exécution, soit déclaré invalide.

### INV-11 — Absence de modification implicite

Une opération qui ne possède pas explicitement la responsabilité de modifier le programme courant ne le modifie pas.

### INV-12 — Reprise cohérente

Une reprise restitue l'état d'exécution nécessaire à la continuité de l'exécution interrompue.

## 2.9. Cycle de vie global

Le cycle de vie général d'une demande est :

1. réception d'une entrée ;
2. validation de l'entrée ;
3. classification ;
4. orientation ;
5. traitement ;
6. production d'un résultat ;
7. restitution au moniteur.

Le cycle de vie d'un programme est :

1. saisie d'une ligne ;
2. compilation ;
3. validation ;
4. intégration au programme courant ;
5. conservation éventuelle ;
6. exécution éventuelle ;
7. décompilation éventuelle ;
8. remplacement ou suppression éventuelle.

Le cycle de vie d'une exécution est :

1. création de l'état d'exécution ;
2. initialisation de la position ;
3. exécution ;
4. suspension éventuelle ;
5. reprise éventuelle ;
6. terminaison ;
7. libération de l'état dynamique.

## 2.10. Correspondance avec le Tome II

L'architecture générale réalise notamment les mécanismes suivants :

| Mécanisme du Tome II | Composant architectural principal |
|---|---|
| VM-0301 à VM-0307 | Gestionnaire du programme courant |
| VM-0401 à VM-0409 | Représentation compilée et gestionnaire de la Chaîne Post-Fixée |
| VM-0501 à VM-0507 | Compilateur |
| VM-0601 à VM-0607 | Gestionnaire du programme courant |
| VM-0701 à VM-0705 | Interpréteur et moteur d'exécution |
| VM-0801 à VM-0803 | Environnement d'exécution |
| VM-0901 à VM-0903 | Gestionnaire des cadres d'activation |
| VM-1001 à VM-1002 | Pile d'évaluation |
| VM-1101 à VM-1103 | Gestionnaire des variables |
| VM-1201 à VM-1202 | Gestionnaire des tableaux |
| VM-1301 à VM-1303 | Gestionnaire des procédures et fonctions |
| VM-1401 à VM-1402 | Gestionnaire des instructions et contrôle du flux |
| VM-1501 à VM-1502 | Gestionnaire des expressions |
| VM-1601 à VM-1602 | Gestionnaire mémoire |
| VM-1701 à VM-1702 | Bibliothécaire |
| VM-1801 | Décompilateur |
| VM-1901 | Gestionnaire des erreurs |
| VM-2001 | Moniteur et analyseur de commandes |
| VM-2101 à VM-2104 | État global et gestionnaire du cycle d'exécution |

## 2.11. Traçabilité

HIS-3.11 → EXI-0001 → VM-0001

HIS-4.5.1 → EXI-0002, EXI-0010 → VM-0301, VM-0401

HIS-4.5.2 → EXI-0003, EXI-0015 → VM-0406, VM-1801

HIS-6.2 → EXI-0001, EXI-0006, EXI-0007, EXI-0008 → VM-0301 à VM-0306

HIS-7 → EXI-0011, EXI-0012, EXI-0013, EXI-0014 → VM-0401 à VM-0405

HIS-8 → EXI-0023, EXI-0024, EXI-0027, EXI-0028, EXI-0029, EXI-0030, EXI-0045, EXI-0046 → VM-0603, VM-0604, VM-0701 à VM-0705, VM-1402, VM-2101 à VM-2104

HIS-10 → EXI-0032, EXI-0037, EXI-0038, EXI-0039, EXI-0040, EXI-0049, EXI-0050 → VM-0802, VM-1101 à VM-1202, VM-1601, VM-1602

HIS-11 → EXI-0009, EXI-0025 → VM-0307, VM-0606, VM-1701, VM-1702

HIS-13 → EXI-0034, EXI-0035, EXI-0036, EXI-0041, EXI-0042, EXI-0043, EXI-0044 → VM-0901 à VM-0903, VM-1301 à VM-1303

HIS-3.1, HIS-3.11, HIS-4.5.6 → EXI-0052 → VM-2001

---

# 3. Organisation de la machine virtuelle

## 3.1. Environnement conversationnel

### VM-2201 — Environnement conversationnel

## OBJET

L'environnement conversationnel constitue le point d'entrée général de la machine virtuelle.

Il reçoit les entrées de l'utilisateur, fournit leur validation, transmet les demandes au composant approprié et restitue les résultats.

**Statut : Choix de conception.**

## RESPONSABILITÉS

L'environnement conversationnel est responsable de :

- recevoir une entrée utilisateur ;
- déterminer la fin de l'entrée ;
- transmettre une entrée complète à l'analyseur de commandes ;
- recevoir le résultat du traitement ;
- restituer un résultat textuel ;
- restituer un diagnostic ;
- conserver la continuité du dialogue.

Il ne prend pas en charge :

- l'analyse syntaxique du langage ;
- la compilation ;
- l'interprétation ;
- la résolution des variables ;
- la gestion interne des objets.

## EXIGENCES SATISFAITES

EXI-0052 — Traitement conversationnel des demandes.

EXI-0001 — Compilation indépendante des lignes.

EXI-0028 — Enchaînement immédiat compilation-exécution.

EXI-0051 — Diagnostic immédiat des erreurs syntaxiques.

## CONTRAINTES

L'environnement doit respecter le caractère conversationnel du système.

Une ligne destinée au programme doit être considérée comme validée à la réception de sa fin.

Le traitement doit distinguer les entrées destinées au programme des demandes immédiates et des commandes.

Une erreur doit pouvoir être restituée sans interrompre la capacité générale de dialogue.

## INTERFACES

### Entrée

`recevoir_caractère(caractère)`

Ajoute un caractère à l'entrée courante.

`valider_entrée()`

Termine l'entrée courante et la transmet pour traitement.

### Sortie

`afficher_texte(texte)`

Restitue un texte.

`afficher_diagnostic(diagnostic)`

Restitue un diagnostic.

### Dépendances

- Analyseur de commandes ;
- Gestionnaire des erreurs ;
- Services d'entrée-sortie.

## DONNÉES MANIPULÉES

- flux de caractères ;
- entrée courante ;
- entrée validée ;
- résultat de traitement ;
- diagnostic.

L'environnement conversationnel ne manipule pas directement les objets compilés.

## ALGORITHMES UTILISÉS

- accumulation séquentielle des caractères ;
- détection de la validation ;
- transfert d'une entrée complète ;
- restitution séquentielle des résultats.

Aucun mécanisme de compilation ou d'interprétation n'est incorporé à ce composant.

## INVARIANTS

Une entrée non validée n'est jamais présentée comme une ligne complète au compilateur.

Une entrée validée est transmise une seule fois au traitement correspondant.

Une erreur de traitement ne détruit pas l'état général du dialogue.

## CONSÉQUENCES

Le dialogue reste indépendant de la réalisation interne de la machine virtuelle.

La compilation et l'interprétation sont activées par le dialogue, mais ne dépendent pas de l'organisation interne du moniteur.

## TRAÇABILITÉ

HIS-3.1 → EXI-0052 → VM-2001

HIS-3.3 → EXI-0001 → VM-0501

HIS-6.5 → EXI-0051 → VM-1901

---

## 3.2. Analyseur de commandes

### VM-2202 — Analyseur de commandes

## OBJET

L'analyseur de commandes détermine le traitement applicable à une entrée validée.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Il est responsable de :

- reconnaître la catégorie fonctionnelle d'une entrée ;
- orienter une ligne de programme vers le compilateur ;
- orienter une commande vers le traitement approprié ;
- orienter une expression immédiate vers l'exécution immédiate ;
- orienter une demande de lecture ou d'écriture de programme vers le bibliothécaire ;
- orienter une demande de listage ou de décompilation vers le décompilateur ;
- transmettre les erreurs de classement au gestionnaire des erreurs.

Il ne réalise pas la fonction choisie.

## EXIGENCES SATISFAITES

EXI-0052 — Traitement conversationnel des demandes.

EXI-0028 — Enchaînement immédiat compilation-exécution.

EXI-0029 — Exécution explicite du programme courant.

EXI-0009 — Persistance du programme sous forme compilée.

## CONTRAINTES

La classification doit permettre de distinguer au minimum :

- une ligne numérotée ;
- une instruction immédiate ;
- une commande ;
- une demande adressée à un processeur spécialisé.

L'analyseur ne doit pas modifier le programme courant.

## INTERFACES

### Entrée

`classifier(entrée_validée)`

Résultat :

- `LIGNE_PROGRAMME` ;
- `EXPRESSION_IMMEDIATE` ;
- `COMMANDE` ;
- `DEMANDE_SPECIALISEE` ;
- `INCONNUE`.

`orienter(catégorie, entrée)`

Transmet l'entrée au composant désigné.

### Sortie

Une demande traitée ou un diagnostic.

## DONNÉES MANIPULÉES

- entrée validée ;
- catégorie d'entrée ;
- commande reconnue ;
- paramètres textuels.

## ALGORITHMES UTILISÉS

- reconnaissance du premier niveau de structure ;
- sélection déterministe du processeur destinataire ;
- dispatch séquentiel.

L'analyseur n'exécute pas d'opération spécifique au langage au-delà de la reconnaissance nécessaire à l'orientation.

## INVARIANTS

Une entrée n'est orientée que vers un seul traitement principal.

L'analyseur ne modifie jamais directement le programme courant.

Une entrée mal classée produit un diagnostic sans modification d'état.

## CONSÉQUENCES

La séparation fonctionnelle des processeurs est maintenue.

Le moniteur reste indépendant du compilateur et de l'interpréteur.

Les processeurs spécialisés peuvent être remplacés ou réorganisés sans modifier le protocole d'entrée du moniteur.

## TRAÇABILITÉ

HIS-3.11 → EXI-0001 → VM-0001

HIS-3.1, HIS-3.11, HIS-4.5.6 → EXI-0052 → VM-2001

HIS-8.1 → EXI-0028 → VM-0702

---

## 3.3. Gestionnaire du programme courant

### VM-2203 — Gestionnaire du programme courant

## OBJET

Le gestionnaire du programme courant conserve et administre l'ensemble cohérent des objets compilés constituant le programme courant.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Il est responsable de :

- conserver les objets compilés acceptés ;
- identifier chaque ligne par son numéro ;
- garantir l'unicité des numéros de ligne ;
- insérer une ligne nouvelle ;
- remplacer une ligne existante ;
- supprimer une ligne ;
- fournir l'ordre logique du programme ;
- fournir le premier objet exécutable ;
- fournir l'objet suivant ;
- remplacer le programme courant lors d'un chargement ;
- vider le programme courant ;
- fournir les objets nécessaires à la décompilation ;
- fournir les objets nécessaires à la conservation.

Il ne compile pas les lignes.

Il ne décompile pas les objets.

Il ne réalise pas leur exécution.

## EXIGENCES SATISFAITES

EXI-0002 — Représentation compilée exécutable.

EXI-0003 — Décompilabilité de la représentation compilée.

EXI-0005 — Programme courant constitué de représentations compilées.

EXI-0006 — Unicité du numéro de ligne.

EXI-0007 — Suppression logique d'une ligne.

EXI-0008 — Ordre logique croissant des lignes.

EXI-0009 — Persistance du programme sous forme compilée.

EXI-0025 — Chargement comme remplacement du programme courant.

EXI-0026 — Prise en compte immédiate des lignes acceptées.

EXI-0031 — État d'exécution distinct du programme compilé.

## CONTRAINTES

Le programme courant doit contenir uniquement des objets compilés valides.

Une ligne rejetée ne peut pas être ajoutée.

Une ligne portant un numéro déjà présent remplace l'objet correspondant.

Une suppression ne modifie aucune autre ligne.

L'ordre logique doit être indépendant de l'ordre de réception des lignes.

Le programme courant doit pouvoir être parcouru sans dépendre du texte source.

## INTERFACES

### Entrées

`ajouter(objet_compilé)`

Ajoute un nouvel objet.

`remplacer(objet_compilé)`

Remplace l'objet portant le même numéro de ligne.

`supprimer(numéro_ligne)`

Supprime l'objet correspondant.

`remplacer_programme(programme_compilé)`

Installe un nouveau programme courant.

`vider()`

Supprime tous les objets du programme courant.

### Requêtes

`obtenir(numéro_ligne)`

Retourne l'objet correspondant.

`premier()`

Retourne le premier objet selon l'ordre logique.

`suivant(position)`

Retourne l'objet suivant.

`parcourir()`

Fournit la séquence logique des objets.

### Sorties

- objet compilé ;
- séquence ordonnée d'objets ;
- absence d'objet ;
- diagnostic de cohérence.

## DONNÉES MANIPULÉES

- identifiant logique de ligne ;
- objet compilé ;
- ensemble des objets compilés ;
- position logique de parcours.

L'organisation matérielle de cet ensemble n'est pas définie.

## ALGORITHMES UTILISÉS

- recherche par identifiant logique ;
- insertion ordonnée ou logique équivalente ;
- remplacement par identité ;
- suppression par identité ;
- parcours dans l'ordre croissant des numéros de ligne.

La réalisation peut utiliser toute organisation interne produisant exactement les mêmes propriétés.

## INVARIANTS

À tout instant :

1. deux objets du programme courant ne possèdent pas le même numéro de ligne ;
2. chaque objet présent est valide ;
3. chaque objet présent possède une représentation compilée exécutable ;
4. le parcours logique est totalement déterminé ;
5. la suppression d'un objet ne supprime pas implicitement les autres ;
6. le programme courant ne dépend pas du texte source pour son exécution.

## CONSÉQUENCES

Le compilateur produit un résultat que le gestionnaire peut accepter ou rejeter.

L'interpréteur lit le programme courant sans modifier son organisation.

Le décompilateur parcourt les objets sans les modifier.

Le bibliothécaire peut demander l'ensemble des objets pour assurer leur conservation.

Une modification du programme courant est immédiatement visible aux opérations ultérieures conformément à EXI-0026.

## TRAÇABILITÉ

HIS-6.2 → EXI-0001, EXI-0006, EXI-0007, EXI-0008 → VM-0301 à VM-0306

HIS-4.5.1 → EXI-0002, EXI-0005 → VM-0301, VM-0303

HIS-4.5.2 → EXI-0003 → VM-0301, VM-0605

HIS-11.2, HIS-11.5 → EXI-0009, EXI-0025 → VM-0307, VM-0606

HIS-6.5 → EXI-0026 → VM-0505, VM-0607

---

## 3.4. Compilateur

### VM-2204 — Compilateur

## OBJET

Le compilateur transforme une ligne validée en objet compilé accepté ou produit un diagnostic.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Il est responsable de :

- recevoir une ligne validée ;
- analyser sa syntaxe ;
- distinguer les erreurs de syntaxe des erreurs différées ;
- produire une représentation compilée ;
- associer cette représentation au numéro de ligne ;
- produire un objet compilé ;
- rejeter atomiquement une ligne incorrecte.

Le compilateur ne :

- modifie pas directement le programme courant ;
- n'exécute pas la ligne ;
- ne conserve pas obligatoirement le texte source ;
- ne résout pas toutes les références dynamiques.

## EXIGENCES SATISFAITES

EXI-0001.

EXI-0002.

EXI-0004 — Rejet atomique d'une ligne incorrecte.

EXI-0018 — Analyse syntaxique préalable à la compilation.

EXI-0019 — Distinction des erreurs de compilation et d'exécution.

EXI-0020 — Production d'une représentation compilée par ligne.

EXI-0021 — Compilation uniforme des lignes entrées indirectement.

EXI-0022 — Indépendance d'exécution vis-à-vis du texte source.

EXI-0045 — Compilation des instructions en opérations exécutables.

EXI-0047 — Compilation post-fixée des expressions.

EXI-0051 — Diagnostic immédiat des erreurs syntaxiques.

## CONTRAINTES

Toute ligne validée doit recevoir un résultat.

Une ligne incorrecte ne produit aucun objet accepté.

Une ligne correcte produit un objet compilé exécutable.

La représentation compilée doit être compatible avec :

- l'interpréteur ;
- le décompilateur ;
- la conservation sous forme compilée.

Les erreurs dépendant de l'état d'exécution peuvent être différées.

## INTERFACES

### Entrée

`compiler(ligne_validée)`

Paramètres :

- numéro de ligne éventuel ;
- texte de la ligne ;
- contexte syntaxique nécessaire.

### Sortie

En cas de succès :

`RESULTAT_COMPILATION_SUCCES`

contenant :

- numéro de ligne ;
- représentation compilée ;
- objet compilé.

En cas d'échec :

`RESULTAT_COMPILATION_ERREUR`

contenant :

- catégorie de l'erreur ;
- emplacement logique ;
- message ;
- informations nécessaires au diagnostic.

### Dépendances

- représentation compilée ;
- gestionnaire des erreurs.

Le compilateur ne dépend pas du stockage concret du programme courant.

## DONNÉES MANIPULÉES

- ligne source ;
- éléments syntaxiques reconnus ;
- opérations codées ;
- valeurs immédiates ;
- références compilées ;
- objet compilé ;
- diagnostic.

Les structures intermédiaires utilisées durant la compilation sont transitoires.

## ALGORITHMES UTILISÉS

Le processus logique de compilation est :

1. valider la ligne ;
2. analyser la structure ;
3. vérifier les contraintes syntaxiques ;
4. construire les opérations compilées ;
5. produire les références et valeurs nécessaires ;
6. construire la représentation compilée ;
7. encapsuler cette représentation dans un objet compilé ;
8. retourner le résultat.

Pour les expressions, l'algorithme doit produire une représentation compatible avec l'ordre post-fixé défini dans le Tome II.

## INVARIANTS

Un succès de compilation implique :

- la validité syntaxique de la ligne ;
- l'existence d'une représentation compilée ;
- la possibilité d'exécuter cette représentation ;
- la possibilité de la décompiler.

Un échec de compilation implique :

- l'absence de nouvel objet compilé accepté ;
- l'absence de modification du programme courant.

## CONSÉQUENCES

Le gestionnaire du programme courant reçoit uniquement des objets compilés acceptés.

L'interpréteur peut traiter une ligne indépendamment de la conservation du texte source.

Le décompilateur doit pouvoir restituer une représentation textuelle à partir de la représentation produite.

Le compilateur peut différer certaines vérifications dont le résultat dépend de l'environnement d'exécution.

## TRAÇABILITÉ

HIS-5.2, HIS-6.5 → EXI-0018, EXI-0051 → VM-0502, VM-1901

HIS-5.2, HIS-8.2 → EXI-0019 → VM-0503

HIS-6.2 → EXI-0001, EXI-0004, EXI-0020 → VM-0302, VM-0501, VM-0504

HIS-7 → EXI-0011, EXI-0047 → VM-0402, VM-1501

HIS-10.1, HIS-13.1 → EXI-0014, EXI-0032, EXI-0042 → VM-0405, VM-0504, VM-1301

---

## 3.5. Interpréteur

### VM-2205 — Interpréteur

## OBJET

L'interpréteur exécute directement les représentations compilées du programme courant dans le contexte d'un état d'exécution.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Il est responsable de :

- lire la représentation compilée ;
- identifier l'opération courante ;
- obtenir les valeurs et références nécessaires ;
- déclencher les opérations d'exécution ;
- mettre à jour l'état d'exécution ;
- assurer la progression normale du flux ;
- appliquer les transferts de contrôle ;
- détecter les terminaisons ;
- provoquer les diagnostics d'exécution.

Il ne :

- compile pas le texte source ;
- ne décompile pas la représentation ;
- ne modifie pas le programme courant pour les besoins ordinaires de l'exécution.

## EXIGENCES SATISFAITES

EXI-0002.

EXI-0010 — Représentation compilée commune à l'exécution et à la décompilation.

EXI-0012 — Évaluation séquentielle de la représentation compilée.

EXI-0013 — Représentation abstraite des opérations exécutables.

EXI-0014 — Représentation compilée des données utilisées par l'exécution.

EXI-0027 — Exécution directe de la représentation compilée.

EXI-0031 — État d'exécution distinct du programme compilé.

EXI-0045 — Compilation des instructions en opérations exécutables.

EXI-0046 — Contrôle explicite du flux d'exécution.

EXI-0047 — Compilation post-fixée des expressions.

## CONTRAINTES

L'interpréteur doit exécuter directement la représentation compilée.

La progression normale est séquentielle.

Un transfert de contrôle peut modifier la position d'exécution.

Une référence compilée peut nécessiter une résolution différée.

Une erreur d'exécution ne doit pas transformer implicitement le programme courant.

## INTERFACES

### Entrées

`demarrer(programme_courant)`

Crée un nouvel état d'exécution.

`reprendre(etat_suspendu)`

Restaure un état suspendu.

`executer_pas(etat_execution)`

Exécute l'unité logique suivante.

`arreter(etat_execution)`

Demande l'arrêt de l'exécution.

### Sorties

- état actif ;
- état suspendu ;
- état terminé ;
- diagnostic d'exécution.

### Dépendances

- programme courant ;
- représentation compilée ;
- moteur d'exécution ;
- environnement d'exécution ;
- pile d'évaluation ;
- cadres d'activation ;
- gestionnaire des variables ;
- gestionnaire mémoire ;
- services d'entrée-sortie.

## DONNÉES MANIPULÉES

- opérations codées ;
- valeurs ;
- références ;
- position d'exécution ;
- état d'exécution ;
- cadres d'activation ;
- pile d'évaluation ;
- résultats.

## ALGORITHMES UTILISÉS

Le parcours logique est :

1. déterminer la position courante ;
2. obtenir la représentation compilée correspondante ;
3. lire l'opération suivante ;
4. résoudre les références nécessaires ;
5. obtenir les valeurs requises ;
6. exécuter l'opération ;
7. modifier l'état d'exécution ;
8. déterminer la prochaine position ;
9. poursuivre ou terminer.

La progression est séquentielle sauf lorsqu'une opération de contrôle modifie explicitement la position.

## INVARIANTS

L'interpréteur n'exécute jamais une représentation qui n'appartient pas au programme courant actif.

La position d'exécution correspond toujours à un état défini.

Toute référence utilisée est résolue ou produit un diagnostic.

Toute valeur consommée par une opération possède un état valide.

Une suspension conserve l'état nécessaire à la reprise.

## CONSÉQUENCES

L'exécution est indépendante du texte source.

La représentation compilée constitue l'unité directe de traitement de l'interpréteur.

La modification du programme courant ne doit pas corrompre l'état d'une exécution déjà active.

L'interpréteur doit pouvoir fonctionner aussi bien pour une exécution programmée que pour l'évaluation immédiate lorsqu'une telle exécution est demandée par le dialogue.

## TRAÇABILITÉ

HIS-4.5.1 → EXI-0002, EXI-0010 → VM-0401, VM-0404

HIS-7.2 → EXI-0012, EXI-0047 → VM-0403, VM-1501

HIS-8.2 → EXI-0023, EXI-0027, EXI-0045, EXI-0046 → VM-0603, VM-0701 à VM-0705, VM-1402

HIS-8.3 → EXI-0024 → VM-0604, VM-2103

HIS-10.1, HIS-10.2 → EXI-0014, EXI-0032 → VM-0405, VM-0802

---

## 3.6. Moteur d'exécution

### VM-2206 — Moteur d'exécution

## OBJET

Le moteur d'exécution coordonne le déroulement général d'une exécution et assure la transition entre les différentes unités de traitement de l'interpréteur.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Il est responsable de :

- créer l'état d'exécution ;
- initialiser la position ;
- appeler l'interpréteur ;
- surveiller l'état de l'exécution ;
- traiter les demandes de suspension ;
- traiter les demandes de reprise ;
- détecter la terminaison ;
- déclencher la libération des ressources d'exécution.

Il ne définit pas la représentation interne des opérations codées.

## EXIGENCES SATISFAITES

EXI-0023.

EXI-0024.

EXI-0029.

EXI-0030.

EXI-0031.

EXI-0033 — Observation de l'état d'exécution.

EXI-0050 — Allocation et libération des objets d'exécution.

## CONTRAINTES

Le moteur doit conserver la séparation entre :

- programme courant ;
- état d'exécution.

Une suspension doit préserver l'état nécessaire à une reprise.

Une terminaison doit libérer les ressources appartenant exclusivement à l'exécution.

La fin d'une exécution ne détruit pas automatiquement le programme courant.

## INTERFACES

### Entrées

`creer_execution(programme_courant)`

`demarrer(etat_execution)`

`suspendre(etat_execution)`

`reprendre(etat_suspendu)`

`terminer(etat_execution)`

`observer(etat_execution)`

### Sorties

- état actif ;
- état suspendu ;
- état terminé ;
- description de l'état ;
- diagnostic.

## DONNÉES MANIPULÉES

- programme courant référencé ;
- état d'exécution ;
- position ;
- cadres d'activation ;
- pile d'évaluation ;
- objets dynamiques ;
- statut d'exécution.

## ALGORITHMES UTILISÉS

- initialisation d'un état ;
- boucle d'exécution ;
- suspension ;
- restauration ;
- terminaison ;
- libération ordonnée des ressources.

Le moteur n'est pas autorisé à réinterpréter le contenu sémantique des opérations ; cette responsabilité appartient à l'interpréteur et aux composants spécialisés.

## INVARIANTS

Un état d'exécution actif possède un programme courant de référence.

Un état suspendu conserve une position cohérente.

Un état terminé n'est plus exécutable.

La fin de l'état d'exécution n'entraîne pas la suppression du programme courant.

## CONSÉQUENCES

Le système peut conserver plusieurs états logiques correspondant à des exécutions distinctes lorsque le contexte d'utilisation l'exige, sous réserve des règles de partage et d'isolement définies par le modèle d'exécution.

Le moteur devient le point de coordination des mécanismes de reprise et de terminaison.

## TRAÇABILITÉ

HIS-3.13 → EXI-0024 → VM-0604, VM-2103

HIS-8.2 → EXI-0029, EXI-0030 → VM-0705, VM-2104

HIS-8.3 → EXI-0033 → VM-0803

HIS-10.6 → EXI-0050 → VM-1602, VM-2104

---

## 3.7. Environnement d'exécution

### VM-2207 — Environnement d'exécution

## OBJET

L'environnement d'exécution fournit le contexte de résolution et d'accès aux objets nécessaires à l'exécution d'une représentation compilée.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Il est responsable de :

- résoudre les références compilées ;
- rechercher les variables accessibles ;
- rechercher les procédures et fonctions ;
- fournir le contexte courant d'activation ;
- fournir le contexte appelant lorsque nécessaire ;
- distinguer les objets globaux des objets locaux.

## EXIGENCES SATISFAITES

EXI-0031.

EXI-0032 — Résolution des références vers les objets d'exécution.

EXI-0034 — Cadre d'activation des appels.

EXI-0035 — Chaînage des activations.

EXI-0038 — Identité stable des variables.

EXI-0042 — Représentation compilée des procédures et fonctions.

## CONTRAINTES

Une référence compilée ne doit pas dépendre d'une adresse physique.

La résolution doit être effectuée dans le contexte d'exécution applicable.

Une variable locale doit être résolue dans le cadre d'activation approprié.

Une référence vers une procédure ou fonction doit désigner une définition compilée valide.

## INTERFACES

### Entrées

`resoudre_reference(reference, contexte)`

`resoudre_variable(identificateur, contexte)`

`resoudre_procedure(identificateur, contexte)`

`resoudre_fonction(identificateur, contexte)`

### Sorties

- objet d'exécution ;
- objet variable ;
- définition compilée ;
- diagnostic.

## DONNÉES MANIPULÉES

- références compilées ;
- identificateurs ;
- cadres d'activation ;
- objets variables ;
- définitions de procédures et fonctions.

## ALGORITHMES UTILISÉS

- résolution dans le contexte courant ;
- remontée éventuelle dans la chaîne des activations ;
- sélection de l'objet visible ;
- rejet en cas d'absence de résolution.

## INVARIANTS

Toute référence résolue désigne un objet compatible avec l'opération qui l'utilise.

Une variable locale n'est pas confondue avec une variable d'un autre cadre.

Une procédure ou fonction résolue possède une représentation compilée valide.

## CONSÉQUENCES

La représentation compilée peut contenir des références abstraites.

L'adresse ou l'emplacement réel d'un objet n'est jamais une propriété de la chaîne compilée au niveau architectural.

La résolution est compatible avec les erreurs différées prévues par le Tome II.

## TRAÇABILITÉ

HIS-10.1, HIS-10.2 → EXI-0032, EXI-0038 → VM-0802, VM-1101

HIS-13.1, HIS-13.2 → EXI-0034, EXI-0035, EXI-0042 → VM-0901 à VM-0903, VM-1301

---

## 3.8. Décompilateur

### VM-2208 — Décompilateur

## OBJET

Le décompilateur transforme une représentation compilée en une représentation textuelle déterministe du programme ou de l'objet demandé.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Il est responsable de :

- parcourir les objets compilés ;
- décoder les opérations codées ;
- restituer les valeurs et références ;
- reconstruire les constructions textuelles ;
- produire une sortie déterministe ;
- signaler les représentations impossibles à décoder.

Il ne :

- modifie pas le programme courant ;
- réexécute pas le programme ;
- ne compile pas la sortie produite.

## EXIGENCES SATISFAITES

EXI-0003.

EXI-0010.

EXI-0015 — Restitution textuelle déterministe.

EXI-0017 — Distinction documentaire des représentations compilées.

EXI-0022.

## CONTRAINTES

La décompilation doit être possible à partir de la représentation compilée conservée.

Le texte source original n'est pas nécessairement conservé.

La sortie doit représenter le même programme au niveau du langage.

La décompilation d'un même objet doit être déterministe.

## INTERFACES

### Entrées

`decompiler(objet_compilé)`

`decompiler_programme(programme_courant)`

### Sorties

- texte d'une ligne ;
- programme textuel ;
- diagnostic de décompilation.

## DONNÉES MANIPULÉES

- objets compilés ;
- opérations codées ;
- valeurs immédiates ;
- références ;
- numéros de ligne ;
- constructions textuelles.

## ALGORITHMES UTILISÉS

- parcours des objets dans l'ordre logique ;
- parcours séquentiel de la représentation compilée ;
- inversion déterministe des constructions représentées ;
- reconstruction syntaxique.

La décompilation ne nécessite aucune exécution de la représentation.

## INVARIANTS

La décompilation ne modifie pas le programme courant.

Une représentation compilée valide produit une représentation textuelle valide.

La même représentation compilée produit toujours la même représentation textuelle dans un même contexte.

## CONSÉQUENCES

Le programme courant peut être listé sans dépendre du texte source conservé.

La représentation compilée est réutilisée à la fois par l'interpréteur et par le décompilateur.

Le format interne exact de la représentation reste une décision d'architecture détaillée traitée au chapitre 6.

## TRAÇABILITÉ

HIS-4.5.2, HIS-5.3 → EXI-0003, EXI-0015 → VM-0406, VM-1801

HIS-7.3 → EXI-0010, EXI-0017 → VM-0401, VM-0408

HIS-11.2 → EXI-0009, EXI-0022 → VM-0307, VM-0507

---

## 3.9. Bibliothécaire

### VM-2209 — Bibliothécaire

## OBJET

Le bibliothécaire assure la conservation et la restauration du programme courant sous sa forme compilée.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Il est responsable de :

- conserver un programme compilé ;
- produire une représentation persistante ;
- restaurer une représentation persistante ;
- vérifier la validité logique de la représentation restaurée ;
- fournir un programme compilé au gestionnaire du programme courant.

Il ne :

- recompilé pas le texte lors d'une restauration ;
- n'interprète pas le programme ;
- ne décompile pas le programme.

## EXIGENCES SATISFAITES

EXI-0009.

EXI-0025.

EXI-0022.

## CONTRAINTES

La conservation porte sur la forme compilée du programme.

La restauration doit produire un programme courant exécutable.

La restauration remplace le programme courant lorsqu'elle est acceptée.

Le bibliothécaire ne doit pas imposer la conservation du texte source.

## INTERFACES

### Entrées

`conserver(programme_courant)`

`restaurer(representation_persistante)`

### Sorties

- représentation persistante ;
- programme compilé restauré ;
- diagnostic.

## DONNÉES MANIPULÉES

- programme courant ;
- objets compilés ;
- représentation persistante ;
- informations de validité.

## ALGORITHMES UTILISÉS

- parcours ordonné des objets ;
- sérialisation logique ;
- restauration logique ;
- contrôle de cohérence ;
- installation du programme restauré.

Le terme « sérialisation » désigne ici une transformation abstraite vers une forme persistante ; aucun format binaire particulier n'est imposé.

## INVARIANTS

Une conservation réussie ne modifie pas le programme courant.

Une restauration réussie produit uniquement un programme cohérent.

Une restauration échouée ne remplace pas un programme courant valide par un état partiellement restauré.

## CONSÉQUENCES

L'interpréteur peut exécuter un programme restauré sans nouvelle compilation.

Le décompilateur peut décompiler un programme restauré.

Le texte source n'est pas une donnée obligatoire de la bibliothèque des programmes.

## TRAÇABILITÉ

HIS-11.2, HIS-11.5 → EXI-0009, EXI-0025 → VM-0307, VM-0606, VM-1701, VM-1702

HIS-4.5.2 → EXI-0022 → VM-0507, VM-0605

---

## 3.10. Gestionnaire des erreurs

### VM-2210 — Gestionnaire des erreurs

## OBJET

Le gestionnaire des erreurs fournit le mécanisme commun de production, de propagation et de restitution des diagnostics.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Il est responsable de :

- représenter un diagnostic ;
- distinguer les catégories d'erreur ;
- transporter le diagnostic entre composants ;
- associer un diagnostic à son contexte ;
- transmettre le diagnostic au moniteur ;
- empêcher la transformation silencieuse d'un échec en succès.

## EXIGENCES SATISFAITES

EXI-0004.

EXI-0019.

EXI-0051.

EXI-0052.

## CONTRAINTES

Une erreur syntaxique doit être signalée avant modification du programme courant.

Une erreur d'exécution doit être distincte d'une erreur de compilation.

Un diagnostic doit être restituable au dialogue.

Une erreur ne doit pas être absorbée silencieusement.

## INTERFACES

### Création

`creer_diagnostic(catégorie, contexte, information)`

### Propagation

`transmettre(diagnostic)`

### Restitution

`rendre_visible(diagnostic)`

## DONNÉES MANIPULÉES

- catégorie d'erreur ;
- contexte ;
- position logique ;
- message ;
- informations complémentaires.

## ALGORITHMES UTILISÉS

- classification ;
- propagation ;
- transformation en représentation textuelle ;
- restitution.

## INVARIANTS

Un diagnostic est associé à une catégorie identifiable.

Un échec de compilation ne produit pas de nouvel objet accepté.

Un diagnostic d'exécution ne transforme pas le programme courant.

## CONSÉQUENCES

Tous les composants disposent d'une méthode uniforme de signalement.

Les diagnostics peuvent être affichés par le même environnement conversationnel quel que soit leur composant d'origine.

## TRAÇABILITÉ

HIS-5.2, HIS-6.5 → EXI-0004, EXI-0051 → VM-0302, VM-1901

HIS-8.2, HIS-12.1 → EXI-0019 → VM-0503, VM-1103

HIS-3.1, HIS-3.11 → EXI-0052 → VM-2001

---


## 3.11. Gestion logique des tâches

La machine virtuelle distingue l'exécution active d'une tâche de l'état
conservé d'une tâche inactive.

Une tâche comprend logiquement :

- une session ;
- un programme courant ;
- un état d'exécution éventuel ;
- les ressources nécessaires à la reprise.

Le gestionnaire des tâches assure l'activation et la suspension.

Les opérations SWAP IN et SWAP OUT désignent les transitions historiques
de conservation et de restauration de l'état d'une tâche. Elles ne sont
pas assimilées au chargement ou au rangement d'un fichier-programme.

### EXIGENCES SATISFAITES

- EXI-0058 ;
- EXI-0059.

### TRAÇABILITÉ

HIS-4.1, HIS-4.2, HIS-4.3, HIS-4.4.1 → EXI-0058, EXI-0059 → VM-2105.


# 4. Les espaces mémoire

## 4.1. Principes généraux

La machine virtuelle distingue plusieurs espaces logiques.

Cette distinction est fonctionnelle.

Elle n'implique aucune segmentation physique de la mémoire d'une implantation.

Un espace logique regroupe des données partageant :

- une même responsabilité ;
- une même durée de vie fonctionnelle ;
- des mêmes règles d'accès ;
- des mêmes invariants.

Les espaces logiques sont :

1. l'espace du programme courant ;
2. l'espace d'exécution ;
3. l'espace des objets dynamiques ;
4. l'espace persistant ;
5. l'espace conversationnel temporaire.

Ces espaces doivent être conceptuellement séparés même lorsqu'une implémentation les représente au moyen d'une organisation matérielle commune.

---

## 4.2. Espace du programme courant

L'espace du programme courant contient les objets compilés constituant le programme courant.

Il contient au minimum :

- le numéro logique de chaque ligne ;
- la représentation compilée correspondante ;
- les informations nécessaires à l'ordre logique.

Il peut contenir les définitions compilées des procédures et fonctions lorsque celles-ci appartiennent au programme courant.

Il ne contient pas, au niveau normatif :

- l'état courant des variables d'exécution ;
- la position actuelle de l'interpréteur ;
- la pile d'évaluation ;
- les cadres d'activation actifs ;
- les objets temporaires créés pendant l'exécution.

### Invariant

Une modification de l'espace du programme courant ne doit pas être confondue avec une modification de l'état dynamique d'une exécution.

### Traçabilité

HIS-6.2 → EXI-0005, EXI-0006, EXI-0008 → VM-0303, VM-0304, VM-0306

HIS-4.5.1 → EXI-0002, EXI-0010 → VM-0301, VM-0401

---

## 4.3. Espace d'exécution

L'espace d'exécution contient les données nécessaires au déroulement d'une exécution.

Il comprend logiquement :

- la position d'exécution ;
- les registres virtuels ;
- la pile d'évaluation ;
- les cadres d'activation ;
- l'environnement d'exécution ;
- l'état des variables ;
- les références vers les objets dynamiques ;
- l'état de suspension éventuel ;
- le statut de l'exécution.

L'espace d'exécution appartient à une exécution déterminée.

Il ne constitue pas une partie du programme compilé.

### Invariant

La fin de l'exécution entraîne la destruction de l'état spécifique de l'exécution, mais ne détruit pas le programme courant.

### Traçabilité

HIS-8.3 → EXI-0024, EXI-0031 → VM-0604, VM-0801, VM-2103

HIS-13.2 → EXI-0034, EXI-0035, EXI-0036 → VM-0901 à VM-0903

---

## 4.4. Espace des objets dynamiques

L'espace des objets dynamiques contient les objets dont la durée de vie ne coïncide pas nécessairement avec celle du programme courant ou d'une seule opération.

Il contient notamment :

- tableaux ;
- chaînes d'exécution ;
- objets créés par les procédures ;
- objets créés par les fonctions ;
- objets temporaires nécessaires à l'évaluation ;
- objets dont la durée de vie est explicitement contrôlée.

La création et la libération relèvent du gestionnaire mémoire.

### Règle

Un objet dynamique n'est accessible que tant qu'il est valide et associé à un contexte d'exécution applicable.

### Invariant

Une référence vers un objet libéré ne peut plus être utilisée comme une référence valide.

### Traçabilité

HIS-10.6 → EXI-0040, EXI-0041, EXI-0049, EXI-0050 → VM-1201, VM-1202, VM-1601, VM-1602

---

## 4.5. Espace persistant

L'espace persistant contient les représentations conservées des programmes.

Il est hors de l'état actif de la machine virtuelle.

La représentation persistante n'est pas un espace d'exécution.

Elle ne contient pas nécessairement :

- la position d'exécution ;
- les cadres d'activation ;
- la pile d'évaluation ;
- l'état courant des variables d'une exécution.

Le chargement transforme une représentation persistante valide en programme courant.

### Invariant

Une représentation persistante ne peut être installée comme programme courant que si elle satisfait les contraintes de cohérence de la représentation compilée.

### Traçabilité

HIS-11.2, HIS-11.5 → EXI-0009, EXI-0025 → VM-0307, VM-0606, VM-1701, VM-1702

---

## 4.6. Espace conversationnel temporaire

L'espace conversationnel temporaire contient les données associées au dialogue en cours.

Il peut contenir :

- la ligne en cours de saisie ;
- la commande en cours ;
- les paramètres textuels d'une demande ;
- les diagnostics à restituer ;
- les résultats textuels à afficher.

Ces données ne font pas partie du programme courant tant qu'elles n'ont pas produit un objet compilé accepté.

### Invariant

La présence d'un texte dans l'espace conversationnel ne signifie pas qu'il appartient au programme courant.

### Traçabilité

HIS-3.1, HIS-3.11 → EXI-0052 → VM-2001

HIS-6.5 → EXI-0004, EXI-0051 → VM-0302, VM-1901

---


## 4.6. Espaces des fichiers-données

La machine virtuelle distingue deux espaces logiques pour les
fichiers-données :

- l'espace temporaire attaché à une session ;
- l'espace permanent de la bibliothèque.

Cette distinction ne préjuge pas de leur représentation physique.

### 4.6.1. Espace temporaire

L'espace temporaire contient les fichiers-données temporaires d'une
session.

Un fichier temporaire est associé à la console qui l'utilise et cesse
d'exister à la fin de la session.

Une extension temporaire accordée à une session reste attachée à cette
session jusqu'à sa libération.

### 4.6.2. Espace permanent

L'espace permanent contient les fichiers-données permanents et les
fichiers-programmes.

Un fichier-données permanent survit à la fin de la session qui l'a créé.

### 4.6.3. Portée des fichiers

L'identité logique d'un fichier comprend sa catégorie et sa portée.

Deux sessions peuvent donc posséder des fichiers temporaires portant le
même nom sans constituer le même fichier logique.

### EXIGENCES SATISFAITES

- EXI-0053 ;
- EXI-0054.

### TRAÇABILITÉ

HIS-11.3, HIS-4.4.3, HIS-4.4.4 → EXI-0053, EXI-0054 → VM-3404, VM-3405.


## 4.7. Relations entre espaces

Les relations autorisées sont les suivantes.

### Programme courant → Exécution

L'exécution lit le programme courant.

Elle ne transforme pas son contenu par les opérations ordinaires d'interprétation.

### Programme courant → Décompilation

Le décompilateur lit le programme courant et produit une représentation textuelle.

### Programme courant → Persistance

Le bibliothécaire transforme le programme courant en représentation persistante.

### Persistance → Programme courant

Le bibliothécaire restaure une représentation persistante en programme courant.

### Exécution → Objets dynamiques

L'exécution crée, utilise et libère des objets dynamiques.

### Conversation → Programme courant

Le compilateur transmet un objet compilé accepté au gestionnaire du programme courant.

### Conversation → Exécution

Le dialogue peut demander une exécution programmée ou immédiate.

### Conversation → Persistance

Le dialogue peut demander une conservation ou une restauration.

Aucune relation directe n'est nécessaire entre :

- espace persistant et espace d'exécution ;
- texte source et variables d'exécution ;
- texte source et position d'exécution.

---

## 4.8. Isolement logique

Les espaces sont isolés par responsabilité.

Une opération sur un espace ne modifie pas implicitement les autres.

Ainsi :

- décompiler ne modifie pas le programme courant ;
- conserver ne modifie pas l'exécution ;
- terminer une exécution ne supprime pas le programme courant ;
- rejeter une ligne ne modifie pas le programme courant ;
- libérer un objet dynamique ne modifie pas la représentation compilée ;
- restaurer un programme remplace le programme courant, mais ne doit pas laisser subsister un état de programme partiellement restauré.

---

## 4.9. Gestion de la durée de vie

La durée de vie des données dépend de leur espace logique.

### Programme courant

Durée de vie :

- de l'acceptation d'une ligne ;
- à son remplacement ou sa suppression ;
- ou au remplacement global du programme.

### État d'exécution

Durée de vie :

- du démarrage ;
- à la terminaison ou à l'abandon de l'exécution.

### Objet dynamique

Durée de vie :

- de sa création ;
- à sa libération explicite ou à la fin du contexte qui le possède selon les règles définies par les chapitres spécialisés.

### Représentation persistante

Durée de vie :

- indépendante de la session d'exécution ;
- jusqu'à son remplacement ou sa suppression hors du périmètre de la machine virtuelle.

### Donnée conversationnelle

Durée de vie :

- jusqu'à son traitement ;
- ou jusqu'à son abandon en cas d'erreur.

---

## 4.10. Contrôle des références

Les références entre espaces doivent respecter les règles suivantes.

Une représentation compilée peut contenir une référence abstraite vers un objet d'exécution.

Une référence compilée ne doit pas contenir une adresse physique dépendante de l'implantation.

Un état d'exécution peut conserver une référence vers :

- un objet du programme courant ;
- une variable ;
- un tableau ;
- une chaîne ;
- une procédure ;
- une fonction.

Toute référence d'exécution doit être invalidée lorsque l'objet correspondant n'est plus accessible selon les règles de son contexte.

Le gestionnaire de l'environnement d'exécution est responsable de la résolution.

Le gestionnaire mémoire est responsable de la validité des objets dynamiques.

Le gestionnaire du programme courant est responsable de la validité des objets compilés.

---

## 4.11. Interactions avec le temps partagé

Le modèle logique ne confond pas :

- suspension d'une exécution ;
- conservation du programme ;
- conservation d'un fichier-programme.

Une suspension conserve un état d'exécution.

Une conservation produit une représentation persistante du programme.

Les deux opérations ont donc des objets et des durées de vie différents.

Une tâche suspendue peut être reprise à partir de son état d'exécution.

Un programme conservé peut être restauré sans restaurer une exécution interrompue.

### Traçabilité

HIS-3.13 → EXI-0024, EXI-0033 → VM-0604, VM-0803, VM-2103

HIS-11.2, HIS-11.5 → EXI-0009, EXI-0025 → VM-1701, VM-1702

---

# 5. Les objets

## 5.1. Classification générale

La machine virtuelle distingue les objets selon leur fonction architecturale.

Les catégories principales sont :

1. objets compilés ;
2. opérations codées ;
3. valeurs ;
4. références ;
5. variables ;
6. tableaux ;
7. chaînes ;
8. nombres ;
9. procédures compilées ;
10. fonctions compilées ;
11. cadres d'activation ;
12. objets dynamiques ;
13. représentations persistantes.

Cette classification est logique.

Elle ne signifie pas que chaque catégorie doit correspondre à une structure distincte dans une implémentation.

Une même structure interne peut représenter plusieurs catégories lorsque les invariants et interfaces restent respectés.

---

## 5.2. Objet compilé

### VM-2301 — Objet compilé de la machine virtuelle

## OBJET

L'objet compilé constitue l'unité logique de programme conservée par le programme courant.

Il associe l'identité logique d'une ligne à sa représentation compilée.

**Statut : Choix de conception.**

## RESPONSABILITÉS

L'objet compilé est responsable de la conservation logique :

- du numéro de ligne ;
- de la représentation compilée correspondante ;
- des informations nécessaires à son exécution ;
- des informations nécessaires à sa décompilation.

Il ne conserve pas obligatoirement le texte source original.

## EXIGENCES SATISFAITES

EXI-0002.

EXI-0003.

EXI-0005.

EXI-0006.

EXI-0010.

EXI-0014.

EXI-0022.

EXI-0042.

## CONTRAINTES

L'objet doit être autonome du point de vue logique pour :

- être identifié dans le programme courant ;
- être exécuté ;
- être décompilé.

Il ne doit pas dépendre d'une adresse physique stable.

Il ne doit pas nécessiter la conservation permanente du texte source.

## INTERFACES

### Création

`creer_objet_compilé(numéro_ligne, représentation_compilée)`

### Consultation

`obtenir_numéro_ligne(objet)`

`obtenir_représentation(objet)`

### Exécution

`fournir_représentation_exécutable(objet)`

### Décompilation

`fournir_représentation_décompilable(objet)`

## DONNÉES MANIPULÉES

- identité logique de ligne ;
- représentation compilée ;
- références compilées ;
- valeurs immédiates ;
- opérations codées.

## ALGORITHMES UTILISÉS

- association identité/représentation ;
- validation de cohérence ;
- accès séquentiel ou logique à la représentation.

## INVARIANTS

Un objet compilé possède une identité logique unique dans le programme courant.

Une représentation compilée valide est associée à l'objet.

L'objet est exécutable et décompilable conformément aux exigences applicables.

## CONSÉQUENCES

Le programme courant peut gérer les lignes sans dépendre du texte source.

L'interpréteur et le décompilateur travaillent sur la même représentation.

Le remplacement d'une ligne remplace l'objet logique correspondant.

## TRAÇABILITÉ

HIS-6.2 → EXI-0001, EXI-0006, EXI-0008 → VM-0301, VM-0304, VM-0306

HIS-4.5.1, HIS-4.5.2 → EXI-0002, EXI-0003, EXI-0010 → VM-0401, VM-0406

HIS-10.1, HIS-13.1 → EXI-0014, EXI-0042 → VM-0405, VM-1301

---

## 5.3. Opération codée

### VM-2302 — Opération codée

## OBJET

L'opération codée constitue l'unité logique minimale qu'un interpréteur peut identifier comme une action à effectuer dans une représentation compilée.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Elle définit :

- l'identité abstraite de l'opération ;
- les données qu'elle consomme ;
- les données qu'elle produit ;
- les éventuelles références qu'elle requiert ;
- son effet sur l'état d'exécution.

Elle ne définit pas la représentation matérielle d'un code opératoire.

## EXIGENCES SATISFAITES

EXI-0013.

EXI-0014.

EXI-0045.

EXI-0046.

EXI-0047.

## CONTRAINTES

Une opération codée doit être identifiable par l'interpréteur.

Elle doit être suffisamment définie pour que son exécution soit déterministe dans un état donné.

Elle peut être :

- une opération de valeur ;
- une opération de référence ;
- une opération arithmétique ;
- une opération logique ;
- une opération de contrôle ;
- une opération d'appel ;
- une opération d'entrée-sortie.

## INTERFACES

### Consultation

`obtenir_code_opératoire(opération)`

`obtenir_paramètres(opération)`

### Exécution

`executer(opération, état_execution)`

Résultat :

- nouvel état ;
- résultat intermédiaire ;
- transfert de contrôle ;
- diagnostic.

## DONNÉES MANIPULÉES

- code opératoire abstrait ;
- paramètres ;
- valeurs ;
- références.

## ALGORITHMES UTILISÉS

- identification ;
- consommation des opérandes ;
- production du résultat ;
- mise à jour de l'état.

## INVARIANTS

Une opération codée valide possède une signification unique dans le modèle de la machine virtuelle.

Une opération ne consomme pas un nombre d'opérandes incompatible avec sa définition.

Une opération de contrôle ne modifie la position d'exécution que conformément à sa sémantique.

## CONSÉQUENCES

La représentation interne détaillée de la Chaîne Post-Fixée peut évoluer sans modifier les responsabilités des composants supérieurs.

Le chapitre 6 précisera la relation entre opération codée, valeurs et références.

## TRAÇABILITÉ

HIS-4.5.1 → EXI-0013 → VM-0404

HIS-7.1, HIS-7.2 → EXI-0011, EXI-0012, EXI-0047 → VM-0402, VM-0403, VM-1501

HIS-8.2 → EXI-0045, EXI-0046 → VM-1401, VM-1402

---

## 5.4. Valeur

### VM-2303 — Valeur d'exécution

## OBJET

La valeur représente une donnée produite ou consommée par l'exécution.

**Statut : Choix de conception.**

## RESPONSABILITÉS

Une valeur doit pouvoir :

- être produite par une opération ;
- être stockée temporairement ;
- être placée sur la pile d'évaluation ;
- être affectée à une variable ;
- être fournie à une procédure ou fonction ;
- être restituée par une expression.

## EXIGENCES SATISFAITES

EXI-0037.

EXI-0039.

EXI-0047.

EXI-0048.

## CONTRAINTES

Une valeur possède une représentation abstraite indépendante de son emplacement matériel.

Elle doit être associée à une catégorie de données déterminée par le langage.

Une valeur produite par une expression doit pouvoir être consommée par l'instruction ou l'opération suivante.

## INTERFACES

`produire(valeur)`

`lire(valeur)`

`copier(valeur)`

`convertir(valeur, contexte)`

`comparer(valeur1, valeur2)`

Les conversions autorisées sont limitées aux opérations prévues par la sémantique du L.S.E.

## DONNÉES MANIPULÉES

- valeurs numériques ;
- valeurs chaînes ;
- valeurs logiques lorsqu'elles sont nécessaires à l'évaluation ;
- références vers des objets.

## ALGORITHMES UTILISÉS

- production ;
- consommation ;
- propagation ;
- comparaison ;
- conversion conforme au contexte.

## INVARIANTS

Une valeur valide peut être consommée par toute opération qui accepte son type.

Une valeur invalide ne peut pas être introduite dans la pile d'évaluation.

## CONSÉQUENCES

Les valeurs peuvent être manipulées indépendamment des variables qui les stockent.

La pile d'évaluation peut contenir des valeurs intermédiaires sans modifier les variables.

## TRAÇABILITÉ

HIS-8.1 → EXI-0047, EXI-0048 → VM-1502

HIS-10.1 → EXI-0037, EXI-0039 → VM-1001, VM-1102

---

## 5.5. Référence

### VM-2304 — Référence compilée et référence résolue

## OBJET

La référence désigne un objet sans incorporer nécessairement son emplacement concret.

La machine virtuelle distingue :

- la référence compilée ;
- la référence résolue d'exécution.

**Statut : Choix de conception.**

## RESPONSABILITÉS

La référence compilée :

- identifie logiquement un objet ;
- reste indépendante de son emplacement d'exécution.

La référence résolue :

- désigne l'objet effectivement accessible ;
- est valide dans un contexte donné.

## EXIGENCES SATISFAITES

EXI-0014.

EXI-0032.

EXI-0038.

EXI-0042.

## CONTRAINTES

Une référence compilée peut être conservée dans le programme courant.

Une référence résolue appartient au contexte d'exécution.

La résolution peut dépendre de :

- l'environnement d'exécution ;
- l'activation courante ;
- l'existence effective de l'objet.

## INTERFACES

`resoudre(reference_compilée, environnement)`

`verifier(reference_résolue)`

`obtenir_objet(reference_résolue)`

## DONNÉES MANIPULÉES

- identificateur logique ;
- contexte de visibilité ;
- référence compilée ;
- référence résolue.

## ALGORITHMES UTILISÉS

- recherche dans le contexte courant ;
- recherche dans les contextes appelants lorsque nécessaire ;
- validation de l'objet obtenu.

## INVARIANTS

Une référence compilée ne contient jamais une adresse physique normative.

Une référence résolue ne reste valide que tant que son objet est accessible.

## CONSÉQUENCES

La représentation compilée peut être conservée et restaurée sans préserver les adresses d'exécution.

Le chargement d'un programme nécessite une nouvelle résolution des références d'exécution.

## TRAÇABILITÉ

HIS-10.1, HIS-10.2 → EXI-0014, EXI-0032 → VM-0405, VM-0802

HIS-10.1 → EXI-0038 → VM-1101

HIS-13.1 → EXI-0042 → VM-1301

---

# 6. Représentation interne de la Chaîne Post-Fixée

La représentation interne de la Chaîne Post-Fixée constitue le niveau architectural qui relie directement le compilateur, l'interpréteur et le décompilateur.

Le Tome II établit :

- l'existence d'une représentation compilée ;
- son emploi commun pour l'exécution et la décompilation ;
- l'organisation post-fixée des expressions ;
- la distinction entre opérations codées et éléments non opératoires ;
- la compacité recherchée ;
- la portée limitée de l'affirmation historique selon laquelle la chaîne codée du L.S.E. serait nécessairement identique à la chaîne post-fixée décrite pour le LSD.

Le présent chapitre ne doit donc pas transformer la propriété probable de la chaîne post-fixée en fait historique universel.

La chaîne post-fixée constitue ici la représentation interne reconstruite retenue par la machine virtuelle.

Elle est normative pour la reconstruction.

Elle n'est pas présentée comme la description certaine de toutes les implantations historiques du L.S.E.

---

## 6.1. Unité de représentation

### VM-2401 — Séquence compilée

## OBJET

La séquence compilée constitue la représentation abstraite ordonnée d'un objet compilé.

Elle est composée d'éléments successifs dont chacun peut être traité par l'interpréteur.

**Statut : Choix de conception.**

## RESPONSABILITÉS

La séquence compilée doit :

- préserver l'ordre logique des opérations ;
- permettre le parcours séquentiel ;
- permettre la décompilation ;
- permettre l'identification des transferts de contrôle ;
- représenter les valeurs immédiates ;
- représenter les références ;
- représenter les opérations codées.

## EXIGENCES SATISFAITES

EXI-0010.

EXI-0011.

EXI-0012.

EXI-0013.

EXI-0014.

EXI-0015.

EXI-0016.

EXI-0017.

EXI-0045.

EXI-0047.

## CONTRAINTES

La séquence doit être directement exploitable par l'interpréteur.

Elle doit être parcourable sans reconstruction du texte source.

Elle doit être décompilable.

Elle doit rester indépendante de l'organisation mémoire physique.

La relation exacte entre cette séquence reconstruite et les formats historiques de « chaîne codée » reste celle définie par VM-0408 et VM-0409.

## INTERFACES

`premier_élément(séquence)`

`élément_suivant(position)`

`lire_élément(position)`

`obtenir_élément(position)`

`fin_séquence(position)`

`decompiler(séquence)`

## DONNÉES MANIPULÉES

Une séquence compilée contient logiquement des éléments parmi :

- opérations codées ;
- valeurs ;
- références ;
- informations de contrôle ;
- informations nécessaires à la décompilation.

## ALGORITHMES UTILISÉS

Le parcours de base est séquentiel.

L'interpréteur obtient :

1. l'élément courant ;
2. son interprétation ;
3. ses opérandes éventuels ;
4. son effet ;
5. la position suivante.

Une opération de contrôle peut remplacer la position suivante par une nouvelle position logique.

## INVARIANTS

La séquence possède un ordre total.

Toute position valide désigne au plus un élément.

La fin de séquence est identifiable.

Chaque élément est interprétable par le composant qui en a la responsabilité.

## CONSÉQUENCES

La représentation interne de la chaîne peut être compacte.

L'interpréteur n'a pas besoin de reconstruire la syntaxe source.

Le décompilateur lit la même séquence.

## TRAÇABILITÉ

HIS-4.5.1, HIS-4.5.2 → EXI-0010, EXI-0015 → VM-0401, VM-0406

HIS-7.1, HIS-7.2 → EXI-0011, EXI-0012 → VM-0402, VM-0403

HIS-7.3 → EXI-0013, EXI-0014, EXI-0016, EXI-0017 → VM-0404, VM-0405, VM-0407, VM-0408

---

## 6.2. Représentation des valeurs

Les valeurs immédiates sont intégrées à la séquence lorsque leur présence est nécessaire à l'exécution.

Une valeur immédiate n'est pas une variable.

Elle n'est pas soumise aux règles de résolution d'une variable.

Elle peut toutefois être utilisée comme opérande d'une opération.

Une représentation compilée doit pouvoir distinguer une valeur immédiate d'une référence.

Cette distinction est nécessaire pour éviter qu'une valeur constante soit traitée comme une adresse logique ou comme un identificateur.

### Règle

Lorsqu'un élément de la séquence représente une valeur immédiate, l'interpréteur doit pouvoir la déposer sur la pile d'évaluation sans résolution d'identificateur.

### Traçabilité

HIS-7.2 → EXI-0014 → VM-0405

HIS-8.1 → EXI-0037, EXI-0047, EXI-0048 → VM-1001, VM-1502

---

## 6.3. Représentation des références

Une référence compilée constitue un élément non immédiat de la représentation.

Elle désigne logiquement :

- une variable ;
- un tableau ;
- une procédure ;
- une fonction ;
- un objet nécessaire à l'exécution.

Une référence peut également porter les informations nécessaires à une opération indexée ou à un appel.

La référence ne contient pas obligatoirement l'objet final.

L'objet final est obtenu par résolution dans l'environnement d'exécution.

### Règle

La compilation produit une référence dès lors que l'identité logique de l'objet peut être représentée sans en connaître l'adresse ou l'emplacement d'exécution.

### Traçabilité

HIS-10.1, HIS-10.2 → EXI-0032, EXI-0038 → VM-0802, VM-1101

HIS-13.1 → EXI-0042 → VM-1301

---

## 6.4. Représentation des opérations

Une opération codée est représentée comme un élément identifiable de la séquence.

Elle possède logiquement :

- un code opératoire abstrait ;
- éventuellement des paramètres ;
- une cardinalité d'opérandes ;
- un effet ;
- un résultat éventuel ;
- une règle de progression.

Les opérations sont classées selon leur fonction.

### Opérations de valeur

Elles produisent ou chargent une valeur.

### Opérations de référence

Elles résolvent ou chargent une référence.

### Opérations arithmétiques

Elles consomment des valeurs et produisent une valeur.

### Opérations logiques

Elles évaluent des relations et produisent une valeur logique.

### Opérations d'appel

Elles provoquent la création d'un cadre d'activation.

### Opérations de contrôle

Elles modifient la position d'exécution.

### Opérations d'entrée-sortie

Elles sollicitent les interfaces d'entrée-sortie.

Cette classification est fonctionnelle.

Elle ne définit pas un jeu de codes opératoires historique.

### Traçabilité

HIS-7.2 → EXI-0013 → VM-0404

HIS-8.2 → EXI-0045, EXI-0046 → VM-1401, VM-1402

HIS-13.1 → EXI-0042 → VM-1301

---

## 6.5. Postfixation des expressions

La compilation des expressions produit une séquence dans laquelle les opérandes nécessaires à une opération sont placés avant l'opération qui les consomme.

Pour une expression conceptuelle :

`A + B`

la représentation logique est :

`A B +`

Pour une expression conceptuelle :

`A + B * C`

la représentation respecte la priorité des opérations :

`A B C * +`

Pour une expression conceptuelle :

`A ^ B ^ C`

la représentation doit respecter l'associativité définie par le Tome II et par la reconstruction du langage.

La représentation post-fixée est utilisée pour :

- l'évaluation ;
- le parcours séquentiel ;
- la décompilation.

La pile d'évaluation constitue le support logique des valeurs intermédiaires.

### Invariant

Une opération arithmétique ou logique ne peut être exécutée avant que ses opérandes compilés nécessaires aient été fournis à l'ordre prévu.

### Traçabilité

HIS-7.1, HIS-7.2 → EXI-0011, EXI-0012 → VM-0402, VM-0403

HIS-8.1 → EXI-0037, EXI-0047, EXI-0048 → VM-1001, VM-1501, VM-1502

---

## 6.6. Relation avec la pile d'évaluation

La Chaîne Post-Fixée et la pile d'évaluation ont des responsabilités distinctes.

La Chaîne Post-Fixée définit l'ordre des éléments compilés.

La pile d'évaluation conserve les valeurs produites pendant le parcours.

Une opération codée peut :

- consommer zéro ou plusieurs valeurs ;
- produire zéro ou plusieurs valeurs ;
- demander la résolution d'une référence ;
- modifier l'état de contrôle.

La pile ne constitue donc pas une seconde représentation compilée.

Elle est un état d'exécution temporaire.

### Traçabilité

HIS-7.2 → EXI-0012 → VM-0403

HIS-8.1 → EXI-0037, EXI-0047, EXI-0048 → VM-1001, VM-1002, VM-1502

---

## 6.7. Contrôle du flux

Les instructions qui modifient la progression du programme sont représentées par des opérations de contrôle.

Elles peuvent notamment :

- modifier la prochaine position ;
- provoquer un appel ;
- provoquer un retour ;
- terminer l'exécution ;
- suspendre l'exécution.

Une opération de contrôle ne modifie pas la représentation compilée du programme courant.

Elle modifie uniquement l'état d'exécution.

### Invariant

Le programme courant reste inchangé lors d'un transfert de contrôle.

### Traçabilité

HIS-8.2 → EXI-0045, EXI-0046 → VM-1401, VM-1402

HIS-13.2 → EXI-0035, EXI-0036, EXI-0043, EXI-0044 → VM-0903, VM-1303

---

## 6.8. Décompilation de la représentation interne

Le décompilateur parcourt la séquence compilée et reconstitue les éléments textuels.

La décompilation ne doit pas supposer que les éléments compilés sont disposés dans le même ordre que dans le texte source original.

Le décompilateur reconstruit une représentation textuelle conforme au langage.

La représentation textuelle produite peut différer du texte source initial tout en représentant la même construction de langage.

### Invariant

La décompilation ne produit jamais une exécution.

### Traçabilité

HIS-4.5.2, HIS-5.3 → EXI-0003, EXI-0015 → VM-0406, VM-1801

---

## 6.9. Limite de la représentation normative

Le Tome III fixe :

- les catégories d'éléments ;
- leur ordre logique ;
- leur rôle ;
- les relations entre eux ;
- les règles d'utilisation.

Il ne fixe pas :

- la largeur d'un code opératoire ;
- la taille d'une unité mémoire ;
- l'encodage des valeurs ;
- le format physique d'une référence ;
- la disposition binaire des éléments ;
- le mécanisme de compression concret.

Ces éléments appartiennent à une éventuelle spécification d'implémentation.

La machine virtuelle doit toutefois préserver les propriétés abstraites suivantes :

- exécution directe ;
- parcours séquentiel ;
- décompilation ;
- compacité relative ;
- indépendance vis-à-vis du texte source.

### Traçabilité

HIS-4.5.1, HIS-4.5.2 → EXI-0010, EXI-0015 → VM-0401, VM-0406

HIS-7 → EXI-0011 à EXI-0017 → VM-0402 à VM-0409

---

## 6.10. Traçabilité globale du chapitre

HIS-4.5.1 → EXI-0002, EXI-0010, EXI-0013 → VM-0401, VM-0404

HIS-4.5.2 → EXI-0003, EXI-0015 → VM-0406

HIS-7.1 → EXI-0011, EXI-0047 → VM-0402, VM-1501

HIS-7.2 → EXI-0012, EXI-0013, EXI-0014 → VM-0403, VM-0404, VM-0405

HIS-7.3 → EXI-0015, EXI-0016, EXI-0017 → VM-0406, VM-0407, VM-0408

HIS-8.2 → EXI-0045, EXI-0046 → VM-1401, VM-1402

HIS-10.1, HIS-10.2 → EXI-0032 → VM-0802

HIS-13.1, HIS-13.2 → EXI-0042, EXI-0043, EXI-0044 → VM-0901 à VM-0903, VM-1301 à VM-1303

---

# 7. Les registres virtuels

Le Tome II définit l'état d'exécution comme un état distinct de la représentation compilée. Cet état comprend notamment une position d'exécution, une pile d'évaluation, des cadres d'activation, un environnement d'accès aux objets et les informations nécessaires à la suspension et à la reprise. Le présent chapitre transforme ces éléments fonctionnels en composants d'architecture logique.

Les registres virtuels ne constituent pas une reconstruction d'un ensemble de registres matériels historiques. Ils constituent des éléments d'état nommés et stables de la machine virtuelle. Leur finalité est de rendre explicites les informations que le moteur d'exécution doit conserver entre deux opérations successives.

Un registre virtuel possède une valeur logique. Cette valeur peut être modifiée par les composants autorisés de la machine virtuelle. Aucune adresse mémoire, aucun emplacement matériel et aucune organisation propre à un processeur déterminé ne sont imposés.

Les registres virtuels sont considérés comme faisant partie de l'état immédiat de l'exécution. Ils ne font pas partie de la représentation persistante du programme courant, sauf lorsqu'une information conservée dans un registre désigne indirectement un objet appartenant à cette représentation.

Les identifiants VM-3001 et suivants utilisés dans le présent tome désignent des composants d'architecture du Tome III. Ils ne remplacent pas les identifiants VM déjà attribués aux mécanismes fonctionnels du Tome II. Les références VM indiquées dans les rubriques de traçabilité renvoient aux mécanismes fonctionnels correspondants du Tome II.

## 7.1 Jeu des registres virtuels

### VM-3001
Jeu des registres virtuels

## OBJET

Le jeu des registres virtuels constitue l'état de référence directement accessible au moteur d'exécution pour représenter la progression courante d'une exécution.

Il regroupe les informations suivantes :

- la position logique d'exécution ;
- l'objet compilé actuellement parcouru ;
- la position dans la représentation compilée de cet objet ;
- le contexte d'activation courant ;
- l'objet courant lorsqu'une opération exige une résolution explicite ;
- l'état de l'exécution ;
- la nature de la transition à effectuer lorsque l'opération courante modifie le flux.

Le jeu de registres ne constitue pas une structure de mémoire historique. Il constitue une décomposition logique de l'état dynamique défini dans le Tome II.

## RESPONSABILITÉS

Le composant est responsable de :

1. conserver les informations de progression nécessaires au moteur d'exécution ;
2. fournir une lecture cohérente de l'état courant aux composants autorisés ;
3. recevoir les modifications de position produites par les instructions de contrôle ;
4. distinguer l'état courant de l'exécution de la représentation compilée elle-même ;
5. permettre la sauvegarde de l'état nécessaire à une suspension ;
6. permettre la restauration d'un état précédemment suspendu ;
7. empêcher qu'une position d'exécution invalide soit considérée comme courante.

Le composant ne décide pas de la sémantique des opérations exécutées. Il conserve l'état produit par cette sémantique.

Il ne résout pas lui-même les identificateurs et ne gère pas lui-même la durée de vie des objets.

## EXIGENCES SATISFAITES

Le composant satisfait ou contribue directement aux exigences suivantes :

- EXI-0023 — Début de l'exécution au premier objet logique ;
- EXI-0024 — Conservation d'une position d'exécution interrompue ;
- EXI-0027 — Exécution directe de la représentation compilée ;
- EXI-0029 — Exécution explicite du programme courant ;
- EXI-0030 — Terminaison explicite ou naturelle de l'exécution ;
- EXI-0031 — État d'exécution distinct du programme compilé ;
- EXI-0046 — Contrôle explicite du flux d'exécution.

## CONTRAINTES

Le jeu des registres doit respecter les contraintes suivantes.

La représentation compilée reste distincte de l'état dynamique.

Une valeur conservée dans un registre de position désigne une position logique. Elle ne constitue pas nécessairement une adresse mémoire.

Une position d'exécution doit être interprétable par le moteur d'exécution sans reconstruction préalable du texte source.

Une suspension doit permettre de préserver toutes les informations nécessaires à une reprise conforme à EXI-0024.

La fin d'une exécution doit pouvoir être représentée sans détruire le programme courant.

Une modification du programme courant ne doit pas modifier rétroactivement l'état d'une exécution déjà créée. Le traitement précis de cette situation dépend des règles de coexistence entre programme courant et exécution active définies par la machine virtuelle.

## INTERFACES

Le composant fournit au moteur d'exécution les opérations abstraites suivantes :

`LIRE_ETAT()`

Retourne une vue cohérente de l'ensemble des registres courants.

`LIRE_POSITION()`

Retourne la position logique actuelle.

`POSITIONNER(position)`

Établit une nouvelle position logique après vérification de sa validité.

`LIRE_CONTEXTE()`

Retourne le cadre d'activation courant.

`FIXER_CONTEXTE(cadre)`

Établit un nouveau cadre d'activation courant.

`LIRE_ETAT_EXECUTION()`

Retourne l'état courant : actif, suspendu, terminé ou en erreur.

`FIXER_ETAT_EXECUTION(etat)`

Modifie l'état courant selon les transitions autorisées.

`CAPTURER()`

Produit une représentation abstraite de l'état nécessaire à une suspension.

`RESTAURER(etat)`

Restaure un état suspendu valide.

`TERMINER(etat_terminal)`

Place l'exécution dans un état terminal sans modifier le programme courant.

Les composants extérieurs ne doivent pas modifier directement une partie interne du jeu de registres. Toute modification passe par une opération définie par l'interface.

## DONNÉES MANIPULÉES

Le composant manipule :

- une position d'exécution ;
- une référence logique vers l'objet compilé courant ;
- une position interne dans cet objet ;
- une référence vers le cadre d'activation courant ;
- un état d'exécution ;
- éventuellement une indication de transfert de flux ;
- les informations nécessaires à la suspension et à la reprise.

Le jeu de registres ne conserve pas le texte source.

Il ne contient pas la totalité du programme courant.

Il ne contient pas les fichiers persistants.

## ALGORITHMES UTILISÉS

Le composant utilise un algorithme abstrait de cohérence d'état.

Avant toute modification de position :

1. la position cible est identifiée ;
2. la position est comparée aux objets compilés accessibles ;
3. la position est rejetée si elle ne désigne aucun point valide ;
4. la nouvelle position devient courante si la validation réussit.

Lors d'une capture :

1. la position courante est enregistrée ;
2. le cadre courant est enregistré ;
3. l'état d'exécution est enregistré ;
4. les informations nécessaires à la reprise sont associées à l'état capturé ;
5. l'exécution passe à l'état suspendu.

Lors d'une restauration :

1. l'état capturé est vérifié ;
2. la cohérence entre programme, position et cadres est vérifiée ;
3. les registres sont restaurés ;
4. l'état passe à actif ;
5. le moteur d'exécution reprend à la position restaurée.

L'algorithme ne dépend d'aucune représentation physique.

## INVARIANTS

Les invariants suivants sont obligatoires :

1. une exécution active possède toujours une position logique valide ;
2. une exécution active possède toujours un cadre courant valide ou un contexte d'exécution racine ;
3. un état suspendu conserve une position restaurable ;
4. un état terminé ne peut plus être repris comme une exécution active ;
5. la modification des registres d'exécution ne modifie pas par elle-même la représentation compilée du programme ;
6. aucun registre ne contient obligatoirement une adresse matérielle ;
7. un registre ne peut désigner un objet qui n'est pas accessible selon les règles de résolution applicables.

## CONSÉQUENCES

Le moteur d'exécution peut être défini comme un processeur qui transforme un état de registres en un nouvel état de registres.

Le mécanisme de suspension peut être spécifié sans imposer une mémoire physique particulière.

Le dialogue avec le moniteur peut observer l'état sans connaître sa représentation interne.

La conservation de l'état devient indépendante du programme courant persistant.

## TRAÇABILITÉ

HIS-3.13 → EXI-0024, EXI-0031 → VM-0801, VM-0604.

HIS-8.2 → EXI-0023, EXI-0029, EXI-0030 → VM-0603, VM-0703, VM-0705.

HIS-14.1 → EXI-0046 → VM-1402.

CPF-7.2 → EXI-0012, EXI-0037 → VM-0403, VM-1001.

## 7.2 Registre de position d'exécution

### VM-3002
Registre de position d'exécution

## OBJET

Le registre de position d'exécution désigne le point logique auquel le moteur doit poursuivre le traitement.

Il constitue la référence principale pour le parcours séquentiel du programme et pour les transferts explicites de flux.

La position est définie indépendamment de toute adresse mémoire.

Elle doit pouvoir désigner au minimum :

- un objet compilé ;
- une position à l'intérieur de cet objet ;
- le cas échéant, une position de reprise associée à un cadre d'activation.

## RESPONSABILITÉS

Le composant est responsable de :

- conserver la position courante ;
- permettre son avancement normal ;
- recevoir une nouvelle position lors d'un transfert de contrôle ;
- permettre la restauration d'une position suspendue ;
- détecter une position terminale.

Il ne détermine pas la sémantique de l'instruction qui provoque le transfert.

## EXIGENCES SATISFAITES

- EXI-0008 — Ordre logique croissant des lignes ;
- EXI-0023 — Début de l'exécution au premier objet logique ;
- EXI-0024 — Conservation d'une position d'exécution interrompue ;
- EXI-0036 — Retour au point d'appel ;
- EXI-0046 — Contrôle explicite du flux d'exécution.

## CONTRAINTES

La position initiale d'une exécution programmée sans transfert explicite est le premier objet compilé selon l'ordre logique du programme.

L'avancement normal doit respecter l'ordre des instructions compilées.

Un transfert de contrôle peut remplacer cet avancement.

Une position de retour doit être distincte de la position du corps appelé.

## INTERFACES

`LIRE_POSITION()`

`AVANCER()`

`SAUTER(position)`

`DETERMINER_DEBUT(programme)`

`DETERMINER_SUIVANT(position)`

`DETECTER_FIN()`

`SAUVEGARDER_POSITION()`

`RESTAURER_POSITION(position)`

## DONNÉES MANIPULÉES

Le registre contient :

- une désignation d'objet compilé ;
- une position dans cet objet ;
- éventuellement une indication de position de retour ;
- une indication de fin lorsque le parcours est arrivé à son terme.

## ALGORITHMES UTILISÉS

Pour l'avancement séquentiel :

1. la position courante est examinée ;
2. le prochain élément logique est déterminé ;
3. si cet élément existe, il devient courant ;
4. sinon, la position suivante est recherchée dans l'objet compilé suivant ;
5. si aucun objet n'existe, l'exécution atteint sa terminaison naturelle.

Pour un transfert :

1. l'opération de contrôle produit une position cible ;
2. la position cible est validée ;
3. le registre est remplacé ;
4. l'exécution reprend à la nouvelle position.

## INVARIANTS

Une position courante doit toujours être interprétable.

Le transfert de contrôle ne doit pas produire une position indéterminée.

L'ordre logique du programme reste la règle par défaut lorsque aucun transfert n'est demandé.

## CONSÉQUENCES

Le moteur d'exécution peut séparer l'opération de lecture d'une opération compilée de la décision de poursuivre à la position suivante.

Les instructions de branchement, de boucle, de retour et d'appel peuvent modifier la position sans connaître la représentation du programme courant.

## TRAÇABILITÉ

HIS-6.2 → EXI-0008 → VM-0306.

HIS-8.2 → EXI-0023, EXI-0046 → VM-0603, VM-1402.

HIS-13.2 → EXI-0036 → VM-0903, VM-1303.

## 7.3 Registre de représentation compilée courante

### VM-3003
Registre de représentation courante

## OBJET

Le registre de représentation compilée courante désigne l'objet compilé actuellement interprété.

Il fait le lien entre le programme courant et la position d'exécution.

## RESPONSABILITÉS

Le composant est responsable de :

- fournir au moteur la représentation associée à la position courante ;
- permettre le changement d'objet lors du passage à la ligne suivante ;
- distinguer l'objet compilé courant d'une représentation temporaire utilisée pour une expression immédiate ;
- maintenir l'association entre la position logique et la représentation exécutée.

## EXIGENCES SATISFAITES

- EXI-0002 ;
- EXI-0005 ;
- EXI-0010 ;
- EXI-0027 ;
- EXI-0045.

## CONTRAINTES

La représentation fournie au moteur doit être compilée et valide.

Le registre ne doit jamais obliger le moteur à revenir au texte source.

Une représentation exécutée dans le cadre d'une exécution immédiate peut être temporaire et ne pas appartenir au programme courant.

## INTERFACES

`CHARGER_OBJET(objet_compilé)`

`LIRE_OBJET()`

`CHARGER_PROGRAMME(programme_courant)`

`CHARGER_LIGNE(ligne_compilée)`

`DECHARGER_OBJET()`

## DONNÉES MANIPULÉES

Le composant manipule :

- une référence d'objet compilé ;
- les informations nécessaires à la navigation dans sa représentation ;
- le contexte d'appartenance de l'objet.

## ALGORITHMES UTILISÉS

Lors d'une exécution programmée :

1. le programme courant fournit son premier objet logique ;
2. le registre le désigne comme objet courant ;
3. le registre de position est initialisé ;
4. le moteur commence le parcours.

Lors du passage à la ligne suivante :

1. le registre de position détermine la ligne suivante ;
2. le registre de représentation est mis à jour ;
3. le parcours recommence au début de l'objet suivant.

## INVARIANTS

L'objet désigné par le registre doit appartenir au programme ou au contexte temporaire de l'exécution.

Une exécution programmée ne peut utiliser une représentation qui n'est pas celle du programme courant au moment de sa création, sauf mécanisme explicitement défini de reprise ou de substitution.

## CONSÉQUENCES

Le programme courant devient une source stable de représentations compilées.

Le moteur d'exécution peut travailler sans conserver le texte source.

## TRAÇABILITÉ

HIS-4.5.1 → EXI-0002, EXI-0010 → VM-0301, VM-0401.

HIS-6.2 → EXI-0005 → VM-0303.

HIS-8.2 → EXI-0027 → VM-0701, VM-0703.

## 7.4 Registre d'opération courante

### VM-3004
Registre d'opération courante

## OBJET

Le registre d'opération courante désigne l'élément de la représentation compilée actuellement traité par le moteur.

Il permet de séparer :

- l'identité de l'objet compilé ;
- la position d'exécution dans cet objet ;
- l'opération effectivement sélectionnée pour exécution.

## RESPONSABILITÉS

Le composant est responsable de :

- recevoir l'élément courant sélectionné par le moteur ;
- conserver son identité pendant son traitement ;
- permettre l'interprétation selon sa nature ;
- fournir l'information nécessaire à la progression après exécution.

## EXIGENCES SATISFAITES

- EXI-0012 ;
- EXI-0013 ;
- EXI-0014 ;
- EXI-0045 ;
- EXI-0047.

## CONTRAINTES

L'opération courante doit appartenir à la représentation compilée valide.

Sa signification est déterminée par la sémantique abstraite de la machine.

Une opération codée ne doit pas être confondue avec une instruction textuelle.

## INTERFACES

`SELECTIONNER(element)`

`LIRE_OPERATION()`

`CLASSER_OPERATION()`

`CONSOMMER_OPERATION()`

`TERMINER_OPERATION()`

## DONNÉES MANIPULÉES

Le registre manipule :

- une opération codée ;
- ses paramètres structurés ;
- ses références ;
- ses valeurs immédiates ;
- les informations de contrôle nécessaires.

## ALGORITHMES UTILISÉS

Le moteur :

1. lit l'élément situé à la position courante ;
2. détermine sa catégorie ;
3. charge l'élément dans le registre ;
4. transmet son traitement au composant compétent ;
5. avance ou modifie la position selon le résultat.

## INVARIANTS

Toute opération courante est interprétable.

Aucune opération n'est exécutée deux fois à cause d'une absence d'avancement de position, sauf lorsqu'une instruction de contrôle demande explicitement de revenir à cette position.

## CONSÉQUENCES

Le moteur d'exécution peut séparer le mécanisme de lecture de la sémantique des opérations.

Cette séparation permet de traiter les opérations arithmétiques, les références, les appels et les transferts de contrôle selon des composants différents.

## TRAÇABILITÉ

CPF-7.2 → EXI-0012 → VM-0403.

CPF-7.3 → EXI-0013 → VM-0404.

HIS-7.4 → EXI-0014 → VM-0405.

HIS-7.3 → EXI-0045 → VM-1401.

## 7.5 Registre de référence d'objet

### VM-3005
Registre de référence d'objet

## OBJET

Le registre de référence d'objet conserve temporairement la désignation logique d'un objet utilisé par l'opération courante.

Il permet de distinguer une référence compilée de l'objet qu'elle désigne effectivement dans l'environnement d'exécution.

## RESPONSABILITÉS

Le composant est responsable de :

- recevoir une référence logique ;
- transmettre cette référence au gestionnaire des variables ou au mécanisme de résolution approprié ;
- conserver temporairement l'objet résolu pendant l'opération ;
- permettre à l'opération courante de lire ou modifier l'objet lorsque cela est autorisé.

## EXIGENCES SATISFAITES

- EXI-0014 ;
- EXI-0032 ;
- EXI-0038 ;
- EXI-0039.

## CONTRAINTES

Une référence logique ne constitue pas une adresse.

La résolution dépend du contexte d'exécution.

La résolution doit respecter les règles de visibilité et de portée.

## INTERFACES

`CHARGER_REFERENCE(reference)`

`RESOUDRE_REFERENCE(contexte)`

`LIRE_OBJET_RESOLU()`

`VIDER_REFERENCE()`

## DONNÉES MANIPULÉES

Le registre contient :

- une identité logique ;
- un objet résolu ;
- éventuellement une information de mode d'accès : lecture, écriture ou désignation.

## ALGORITHMES UTILISÉS

Lorsqu'une référence est rencontrée :

1. la référence est chargée ;
2. le contexte courant est transmis au mécanisme de résolution ;
3. la portée applicable est examinée ;
4. l'objet correspondant est obtenu ;
5. le résultat est placé dans le registre ;
6. l'opération courante utilise l'objet.

## INVARIANTS

Une référence résolue désigne un objet accessible.

Un objet libéré ne peut rester une cible valide du registre.

Le registre est vidé ou invalidé lorsqu'il cesse d'être utilisé.

## CONSÉQUENCES

Le compilateur peut produire des références logiques sans connaître l'emplacement d'exécution final.

Le gestionnaire des variables reste responsable de la résolution.

Le moteur d'exécution reste indépendant de la structure de stockage des objets.

## TRAÇABILITÉ

HIS-10.1 → EXI-0032, EXI-0038 → VM-0802, VM-1101.

HIS-10.2 → EXI-0032 → VM-0405, VM-0802.

## 7.6 Registre d'état d'exécution

### VM-3006
Registre d'état d'exécution

## OBJET

Le registre d'état d'exécution représente la phase logique dans laquelle se trouve une exécution.

Les états minimaux sont :

- prête ;
- active ;
- suspendue ;
- terminée ;
- en erreur.

## RESPONSABILITÉS

Le composant est responsable de :

- conserver l'état courant ;
- autoriser uniquement les transitions valides ;
- signaler la suspension ;
- signaler la terminaison ;
- distinguer une erreur récupérable d'une terminaison normale lorsque la sémantique de l'exécution le permet.

## EXIGENCES SATISFAITES

- EXI-0024 ;
- EXI-0030 ;
- EXI-0031 ;
- EXI-0033.

## CONTRAINTES

Une exécution suspendue ne doit pas être parcourue par le moteur comme une exécution active.

Une exécution terminée ne peut pas être reprise sans création d'un nouvel état d'exécution.

Une erreur d'exécution doit interrompre le parcours au point défini par sa sémantique.

La fin de l'exécution ne détruit pas le programme courant.

## INTERFACES

`ACTIVER()`

`SUSPENDRE()`

`REPRENDRE()`

`TERMINER_NORMALEMENT()`

`TERMINER_EN_ERREUR(erreur)`

`LIRE_ETAT()`

## DONNÉES MANIPULÉES

Le composant manipule un état logique et, le cas échéant, une indication de la cause de suspension ou de terminaison.

## ALGORITHMES UTILISÉS

Les transitions sont contrôlées par une machine d'états abstraite.

Une transition non autorisée est rejetée.

Lorsqu'une exécution suspendue est reprise :

1. l'état doit être suspendu ;
2. l'état dynamique doit être disponible ;
3. les registres doivent être cohérents ;
4. l'état passe à actif.

## INVARIANTS

Une exécution possède un unique état courant.

La suspension préserve l'état nécessaire à la reprise.

La terminaison est irréversible pour l'activation concernée.

## CONSÉQUENCES

Le moniteur peut connaître le statut d'une exécution.

Le temps partagé peut suspendre une tâche sans modifier le programme courant.

Le moteur d'exécution peut traiter explicitement les erreurs.

## TRAÇABILITÉ

HIS-3.13 → EXI-0024, EXI-0031, EXI-0033 → VM-0604, VM-0801, VM-0803.

HIS-8.2 → EXI-0030 → VM-0705.

## 7.7 Registres du cadre d'activation courant

### VM-3007
Registres de contexte d'activation

## OBJET

Les registres de contexte d'activation regroupent les informations nécessaires à l'exécution du cadre actuellement actif.

Ils représentent notamment :

- le cadre courant ;
- son appelant ;
- la position de retour ;
- les objets locaux ;
- les paramètres ;
- le contexte nécessaire à la restitution d'un résultat.

## RESPONSABILITÉS

Le composant est responsable de :

- désigner le cadre courant ;
- permettre le changement de cadre lors d'un appel ;
- permettre la restauration du cadre appelant lors d'un retour ;
- exposer la position de retour au moteur ;
- maintenir la cohérence entre cadre courant et position d'exécution.

## EXIGENCES SATISFAITES

- EXI-0034 ;
- EXI-0035 ;
- EXI-0036 ;
- EXI-0042 ;
- EXI-0043 ;
- EXI-0044.

## CONTRAINTES

Chaque activation possède un contexte distinct.

Le cadre courant doit pouvoir retrouver son appelant.

La sortie d'un cadre restaure son appelant.

La position de retour est une information logique et non une adresse matérielle.

## INTERFACES

`LIRE_CADRE_COURANT()`

`CREER_CADRE(definition, parametres, appelant, retour)`

`ACTIVER_CADRE(cadre)`

`LIRE_APPELANT(cadre)`

`LIRE_POSITION_RETOUR(cadre)`

`RESTAURER_APPELANT()`

`DETRUIRE_CADRE()`

## DONNÉES MANIPULÉES

Un cadre contient abstraitement :

- des paramètres ;
- des variables locales ;
- un appelant ;
- une position de retour ;
- des informations de résultat éventuel ;
- les références nécessaires au corps compilé.

## ALGORITHMES UTILISÉS

Lors d'un appel :

1. la définition appelée est identifiée ;
2. les arguments sont évalués ;
3. le cadre courant est mémorisé comme appelant ;
4. un nouveau cadre est créé ;
5. les paramètres sont initialisés ;
6. les objets locaux sont créés lorsqu'ils sont nécessaires ;
7. la position de retour est enregistrée ;
8. le nouveau cadre devient courant.

Lors d'un retour :

1. le résultat éventuel est préparé ;
2. la position de retour est lue ;
3. le cadre courant est terminé ;
4. le cadre appelant devient courant ;
5. la position de retour devient la nouvelle position d'exécution ;
6. le résultat est remis au contexte appelant.

## INVARIANTS

Le cadre courant est toujours le dernier cadre actif de la chaîne d'activations.

Aucun cadre actif ne peut être son propre appelant.

Toute position de retour appartient à un contexte appelant valide.

## CONSÉQUENCES

Les procédures et fonctions peuvent être imbriquées.

La résolution des variables peut parcourir les contextes selon les règles de portée.

Le moteur d'exécution n'a pas besoin de connaître la structure physique des activations.

## TRAÇABILITÉ

HIS-13.1 → EXI-0034, EXI-0035, EXI-0043 → VM-0901, VM-0902, VM-1302.

HIS-13.2 → EXI-0036, EXI-0044 → VM-0903, VM-1303.

---

# 8. Le moteur d'exécution

Le moteur d'exécution est le composant central chargé de transformer une représentation compilée en effets d'exécution.

Le Tome II le définit fonctionnellement à travers l'interpréteur, l'évaluation séquentielle, la pile d'évaluation, les cadres d'activation, la résolution des références, les transferts de contrôle, la suspension et la terminaison.

Le Tome III définit son organisation logique comme un ensemble coordonné de fonctions. Cette organisation ne constitue pas une reconstitution de l'organisation interne d'une implantation historique particulière.

## 8.1 Processeur d'exécution

### VM-3101
Moteur d'exécution

## OBJET

Le moteur d'exécution est le composant qui reçoit une représentation compilée et un état d'exécution, sélectionne les éléments à traiter et provoque leur interprétation.

Il constitue le composant architectural correspondant à l'interpréteur abstrait du Tome II.

## RESPONSABILITÉS

Le moteur :

- sélectionne l'élément courant ;
- le transmet au mécanisme de traitement approprié ;
- assure l'avancement normal ;
- prend en compte les transferts de contrôle ;
- coordonne les appels et retours ;
- coordonne la pile d'évaluation ;
- provoque la résolution des références ;
- produit les transitions d'état ;
- signale les erreurs d'exécution ;
- produit les sorties demandées par le langage.

Il ne compile pas.

Il ne décompile pas.

Il ne conserve pas les programmes persistants.

## EXIGENCES SATISFAITES

- EXI-0012 ;
- EXI-0013 ;
- EXI-0014 ;
- EXI-0027 ;
- EXI-0029 ;
- EXI-0030 ;
- EXI-0031 ;
- EXI-0037 ;
- EXI-0045 ;
- EXI-0046 ;
- EXI-0048.

## CONTRAINTES

Le moteur reçoit uniquement des représentations compilées valides.

Il ne doit pas exiger le texte source pour exécuter une opération.

Le programme courant reste distinct de l'état d'exécution.

Toute erreur d'exécution doit laisser l'état dans une situation explicitement définie.

Une exécution programmée commence au premier objet logique lorsque aucun transfert initial n'est spécifié.

## INTERFACES

Entrée :

`EXECUTER(programme, etat_initial)`

`REPRENDRE(etat_suspendu)`

`EXECUTER_LIGNE(ligne_compilée, contexte)`

Sortie :

`ETAT_EXECUTION`

`RESULTAT`

`SORTIES`

`DIAGNOSTICS`

`ETAT_SUSPENDU`

Le moteur appelle les interfaces des composants suivants :

- jeu des registres ;
- pile d'évaluation ;
- gestionnaire des variables ;
- gestionnaire des cadres ;
- gestionnaire des entrées-sorties ;
- gestionnaire des erreurs ;
- dialogue avec le moniteur.

## DONNÉES MANIPULÉES

Le moteur manipule :

- les objets compilés ;
- les opérations codées ;
- les valeurs ;
- les références ;
- la pile d'évaluation ;
- les cadres d'activation ;
- les registres virtuels ;
- les diagnostics ;
- les résultats.

## ALGORITHMES UTILISÉS

Le fonctionnement abstrait est :

1. initialiser ou restaurer l'état d'exécution ;
2. vérifier que l'état est actif ;
3. lire la position courante ;
4. sélectionner l'opération courante ;
5. classer l'opération ;
6. exécuter l'opération par le composant compétent ;
7. appliquer la transition de position résultante ;
8. vérifier l'état d'exécution ;
9. recommencer tant que l'état est actif.

Le moteur ne réalise aucune transformation globale du programme avant l'exécution.

## INVARIANTS

À chaque cycle :

- une position courante est définie ;
- une opération courante est déterminée ;
- l'état des registres reste cohérent ;
- toute modification de l'état est produite par une opération définie ;
- une terminaison interrompt le cycle.

## CONSÉQUENCES

L'interpréteur peut être implémenté indépendamment du format physique de la représentation compilée.

La même architecture peut prendre en charge l'exécution immédiate et l'exécution programmée.

La suspension peut être réalisée à tout point où le langage ou le moniteur l'autorise.

## TRAÇABILITÉ

HIS-4.5.3 → EXI-0027 → VM-0701.

HIS-8.1 → EXI-0028, EXI-0048 → VM-0702, VM-1502.

HIS-8.2 → EXI-0029, EXI-0030 → VM-0703, VM-0705.

HIS-3.13 → EXI-0031 → VM-0801.

## 8.2 Sélecteur des opérations

### VM-3102
Sélecteur d'opération

## OBJET

Le sélecteur d'opération détermine le traitement à appliquer à l'élément courant de la représentation compilée.

## RESPONSABILITÉS

Il distingue notamment :

- valeur immédiate ;
- référence ;
- opération de calcul ;
- opération de contrôle ;
- opération d'appel ;
- opération de retour ;
- opération d'entrée-sortie ;
- opération de déclaration ;
- opération de gestion d'objet.

## EXIGENCES SATISFAITES

- EXI-0013 ;
- EXI-0045 ;
- EXI-0046.

## CONTRAINTES

Le classement doit être déterministe.

Le sélecteur ne doit pas dépendre du texte source.

Il ne doit pas interpréter directement une représentation binaire non définie par la machine abstraite.

## INTERFACES

`CLASSER(element)`

Retourne la catégorie logique de l'élément.

`SELECTIONNER_TRAITEMENT(categorie)`

Retourne le composant compétent.

## DONNÉES MANIPULÉES

Le composant manipule les catégories abstraites de la représentation compilée et les références de traitement.

## ALGORITHMES UTILISÉS

Le sélecteur applique une classification déterministe.

Une catégorie inconnue provoque une erreur de représentation compilée.

## INVARIANTS

Tout élément d'une représentation compilée valide possède une catégorie reconnue.

## CONSÉQUENCES

Le moteur d'exécution reste indépendant des détails internes de chaque opération.

## TRAÇABILITÉ

CPF-7.3 → EXI-0013 → VM-0404.

HIS-7.3 → EXI-0045 → VM-1401.

HIS-14.1 → EXI-0046 → VM-1402.

## 8.3 Évaluateur de la chaîne post-fixée

### VM-3103
Évaluateur post-fixé

## OBJET

L'évaluateur post-fixé applique la sémantique d'évaluation aux valeurs, références et opérations présentées selon l'ordre logique de la représentation compilée.

## RESPONSABILITÉS

Il :

- empile les valeurs ;
- résout les références ;
- consomme les opérandes ;
- applique les opérations ;
- produit les valeurs intermédiaires ;
- produit la valeur finale lorsqu'une expression est terminée.

## EXIGENCES SATISFAITES

- EXI-0011 ;
- EXI-0012 ;
- EXI-0037 ;
- EXI-0047 ;
- EXI-0048.

## CONTRAINTES

Les opérandes nécessaires précèdent l'opération qui les consomme.

La pile d'évaluation est abstraite.

Le nombre d'opérandes consommés est déterminé par la sémantique de l'opération.

## INTERFACES

`EVALUER(element, contexte)`

`EMPILE(valeur)`

`CONSOMMER(nombre)`

`PRODUIRE(valeur)`

`RESULTAT_FINAL()`

## DONNÉES MANIPULÉES

- valeurs intermédiaires ;
- références résolues ;
- opérations ;
- résultat final.

## ALGORITHMES UTILISÉS

Pour chaque élément :

1. valeur → empilement ;
2. référence → résolution puis empilement ou désignation ;
3. opération → consommation des opérandes, application de la sémantique, production du résultat ;
4. fin d'expression → production de la valeur finale.

## INVARIANTS

Une opération ne consomme jamais un nombre de valeurs supérieur au nombre disponible.

Une expression valide laisse une valeur finale conforme à sa sémantique.

## CONSÉQUENCES

Le compilateur peut produire une représentation post-fixée sans imposer une structure arborescente conservée à l'exécution.

Le décompilateur peut exploiter la même représentation sans réutiliser la pile d'exécution.

## TRAÇABILITÉ

CPF-7.1, CPF-7.2 → EXI-0011, EXI-0037, EXI-0047 → VM-0402, VM-1001, VM-1501.

HIS-8.1 → EXI-0048 → VM-1502.

## 8.4 Gestionnaire du flux d'exécution

### VM-3104
Gestionnaire du flux

## OBJET

Le gestionnaire du flux prend en charge les transitions normales et explicites de la position d'exécution.

## RESPONSABILITÉS

Il traite :

- l'avancement séquentiel ;
- les transferts de contrôle ;
- les boucles ;
- les retours ;
- les appels ;
- la terminaison du flux.

## EXIGENCES SATISFAITES

- EXI-0008 ;
- EXI-0023 ;
- EXI-0036 ;
- EXI-0046.

## CONTRAINTES

Le flux séquentiel est la règle par défaut.

Une instruction de contrôle peut remplacer la position suivante.

Un retour utilise la position de retour du cadre courant.

Un appel crée un nouveau contexte.

## INTERFACES

`SUIVRE()`

`TRANSFERER(position)`

`APPELER(definition, arguments)`

`RETOURNER(resultat)`

`TERMINER()`

## DONNÉES MANIPULÉES

Le composant manipule les positions logiques et les références de cadres.

## ALGORITHMES UTILISÉS

Le gestionnaire applique une priorité logique :

1. transfert explicite ;
2. appel ;
3. retour ;
4. terminaison ;
5. avancement séquentiel.

Cette priorité ne signifie pas que les mécanismes soient nécessairement exécutés dans cet ordre dans une implantation. Elle définit uniquement la règle de sélection du prochain état logique.

## INVARIANTS

Une seule transition de flux est appliquée à chaque opération.

La position après transition est toujours déterminée ou terminale.

## CONSÉQUENCES

Les opérations de contrôle n'ont pas besoin de connaître la structure mémoire du programme.

## TRAÇABILITÉ

HIS-8.2 → EXI-0008, EXI-0023, EXI-0046 → VM-0603, VM-1402.

HIS-13.2 → EXI-0036 → VM-0903.

## 8.5 Suspension et reprise

### VM-3105
Gestionnaire de suspension

## OBJET

Le gestionnaire de suspension transforme un état d'exécution actif en état suspendu et restaure ultérieurement cet état.

## RESPONSABILITÉS

Il :

- capture l'état dynamique ;
- conserve la position ;
- conserve les cadres ;
- conserve la pile ;
- conserve l'environnement des objets ;
- restaure l'ensemble de ces éléments.

## EXIGENCES SATISFAITES

- EXI-0024 ;
- EXI-0031 ;
- EXI-0033.

## CONTRAINTES

La suspension ne modifie pas le programme courant.

La reprise doit retrouver un état cohérent.

## INTERFACES

`SUSPENDRE()`

`CAPTURER_ETAT()`

`RESTAURER_ETAT(etat)`

`REPRENDRE()`

## DONNÉES MANIPULÉES

- état des registres ;
- pile d'évaluation ;
- chaîne de cadres ;
- environnement d'accès aux objets ;
- statut d'exécution.

## ALGORITHMES UTILISÉS

La capture est atomique au niveau logique.

La restauration est validée avant activation.

## INVARIANTS

Un état suspendu est autonome du point de vue des informations nécessaires à la reprise.

## CONSÉQUENCES

Le temps partagé peut suspendre une tâche.

Le moniteur peut examiner une exécution sans la détruire.

## TRAÇABILITÉ

HIS-3.13 → EXI-0024, EXI-0031, EXI-0033 → VM-0604, VM-0801, VM-0803.

## 8.6 Terminaison d'une exécution

### VM-3106
Gestionnaire de terminaison

## OBJET

Le gestionnaire de terminaison met fin à l'état d'exécution courant tout en conservant le programme courant.

## RESPONSABILITÉS

Il distingue :

- terminaison naturelle ;
- terminaison explicite ;
- terminaison par erreur.

## EXIGENCES SATISFAITES

- EXI-0030 ;
- EXI-0031.

## CONTRAINTES

La terminaison de l'exécution ne supprime pas le programme courant.

Les ressources d'exécution temporaire doivent devenir libérables.

## INTERFACES

`TERMINER_NORMALEMENT()`

`TERMINER_EXPLICITEMENT()`

`TERMINER_EN_ERREUR(erreur)`

## DONNÉES MANIPULÉES

- état d'exécution ;
- informations de cause ;
- ressources temporaires.

## ALGORITHMES UTILISÉS

1. arrêt du parcours ;
2. fermeture des cadres actifs ;
3. conservation du programme courant ;
4. libération des ressources temporaires ;
5. production éventuelle d'un diagnostic.

## INVARIANTS

Après terminaison, aucune opération d'exécution ne peut continuer à modifier l'état de l'exécution terminée.

## CONSÉQUENCES

Le même programme courant peut être relancé après terminaison.

## TRAÇABILITÉ

HIS-8.2, HIS-14.1 → EXI-0030 → VM-0705, VM-2103.

---

# 9. Le compilateur

Le compilateur reçoit une ligne validée et produit un objet compilé. Il traite également les lignes provenant d'une source de données lorsque celles-ci sont soumises au mécanisme d'encodage.

Le Tome III définit ici le compilateur comme un composant architectural composé de fonctions distinctes mais coordonnées :

- validation syntaxique ;
- construction de la représentation compilée ;
- intégration au programme courant.

La séparation fonctionnelle ne signifie pas que ces fonctions doivent être réalisées par des modules indépendants dans une implémentation. Elle signifie qu'elles possèdent des responsabilités et des interfaces distinctes.

## 9.1 Interface d'entrée du compilateur

### VM-3201
Réception d'une ligne à compiler

## OBJET

Le composant reçoit une ligne validée et la soumet au processus de compilation.

## RESPONSABILITÉS

Il :

- reçoit les caractères de la ligne ;
- reconnaît la fin de validation ;
- distingue ligne de programme et entrée immédiate ;
- transmet les données au traitement approprié.

## EXIGENCES SATISFAITES

- EXI-0001 ;
- EXI-0018 ;
- EXI-0021 ;
- EXI-0052.

## CONTRAINTES

La fin de ligne constitue la frontière de validation.

Une ligne non validée ne modifie pas le programme courant.

Une erreur syntaxique entraîne un diagnostic.

## INTERFACES

`RECEVOIR_LIGNE(caracteres)`

`VALIDER_LIGNE()`

`CLASSER_ENTREE()`

`SOUMETTRE_A_COMPILATION()`

## DONNÉES MANIPULÉES

- caractères de la ligne ;
- numéro éventuel ;
- construction syntaxique ;
- nature de l'entrée.

## ALGORITHMES UTILISÉS

1. accumulation logique des caractères ;
2. détection de la fin ;
3. classification ;
4. transfert au traitement adapté.

## INVARIANTS

Une ligne non validée n'est jamais considérée comme compilée.

## CONSÉQUENCES

Le compilateur peut être utilisé aussi bien par l'environnement conversationnel que par le bibliothécaire lorsqu'il fournit des lignes indirectes.

## TRAÇABILITÉ

HIS-6.2 → EXI-0001 → VM-0501.

HIS-6.5, HIS-11.5 → EXI-0021 → VM-0506.

HIS-3.1 → EXI-0052 → VM-2001, VM-2002.

## 9.2 Analyse syntaxique et construction logique

### VM-3202
Analyseur du compilateur

## OBJET

Le composant transforme une ligne valide en une structure logique exploitable par le générateur de représentation compilée.

## RESPONSABILITÉS

Il :

- reconnaît les constructions valides ;
- détecte les erreurs syntaxiques ;
- identifie les instructions ;
- identifie les expressions ;
- distingue les références, valeurs et opérations ;
- prépare les informations de contrôle nécessaires.

## EXIGENCES SATISFAITES

- EXI-0018 ;
- EXI-0051 ;
- EXI-0045 ;
- EXI-0047.

## CONTRAINTES

Le résultat doit être indépendant du texte original après compilation.

Le processus ne doit pas modifier le programme courant tant que la compilation n'est pas terminée avec succès.

La forme post-fixée est appliquée aux expressions conformément au modèle reconstruit du Tome II.

## INTERFACES

`ANALYSER(ligne)`

Retourne une structure logique ou un diagnostic.

`ANALYSER_EXPRESSION(expression)`

Retourne une structure d'expression.

`ANALYSER_INSTRUCTION(instruction)`

Retourne une structure d'instruction.

`ANALYSER_REFERENCE(reference)`

Retourne une référence logique.

## DONNÉES MANIPULÉES

- constructions syntaxiques ;
- identificateurs ;
- constantes ;
- expressions ;
- instructions ;
- contrôles de flux ;
- paramètres.

## ALGORITHMES UTILISÉS

L'analyse est réalisée en respectant les règles syntaxiques autorisées.

Les expressions sont organisées selon leur ordre post-fixé.

Les constructions incorrectes produisent un diagnostic sans générer d'objet accepté.

## INVARIANTS

Une structure logique produite par l'analyse est syntaxiquement valide.

Aucune représentation compilée partielle ne devient le programme courant.

## CONSÉQUENCES

Le générateur peut être indépendant de la syntaxe textuelle.

Le diagnostic est produit avant toute modification du programme courant.

## TRAÇABILITÉ

HIS-5.2, HIS-6.5 → EXI-0018, EXI-0051 → VM-0502, VM-1901.

CPF-7.1, CPF-7.2 → EXI-0047 → VM-0402, VM-1501.

## 9.3 Génération de la représentation compilée

### VM-3203
Générateur de représentation compilée

## OBJET

Le générateur produit la représentation compilée à partir de la structure logique validée.

## RESPONSABILITÉS

Il :

- produit les valeurs compilées ;
- produit les références ;
- produit les opérations codées ;
- produit les informations de contrôle ;
- conserve l'ordre logique des instructions ;
- produit une représentation compatible avec l'interpréteur et le décompilateur.

## EXIGENCES SATISFAITES

- EXI-0002 ;
- EXI-0010 ;
- EXI-0011 ;
- EXI-0013 ;
- EXI-0014 ;
- EXI-0045 ;
- EXI-0047.

## CONTRAINTES

Le générateur ne doit pas conserver la syntaxe textuelle inutile à l'exécution.

Il doit conserver les informations nécessaires à la décompilation.

Les expressions doivent respecter l'ordre post-fixé reconstruit.

Les références doivent rester résolubles dans le contexte d'exécution.

## INTERFACES

`GENERER(structure_logique)`

Retourne une représentation compilée.

`GENERER_VALEUR(valeur)`

`GENERER_REFERENCE(reference)`

`GENERER_OPERATION(operation)`

`GENERER_CONTROLE(transition)`

## DONNÉES MANIPULÉES

- valeurs ;
- références ;
- opérations ;
- informations de contrôle ;
- informations nécessaires à la décompilation.

## ALGORITHMES UTILISÉS

Pour une expression :

1. générer récursivement les opérandes ;
2. générer les sous-expressions ;
3. générer l'opération finale ;
4. concaténer dans l'ordre post-fixé.

Pour une séquence d'instructions :

1. générer chaque instruction dans son ordre ;
2. conserver l'ordre d'exécution ;
3. insérer les éléments de contrôle nécessaires.

## INVARIANTS

Toute représentation produite est exécutable.

Toute représentation produite est décompilable selon les exigences applicables.

Les références produites possèdent une identité logique stable.

## CONSÉQUENCES

La représentation compilée devient l'interface entre compilateur, interpréteur, décompilateur et bibliothécaire.

## TRAÇABILITÉ

HIS-4.5.1, HIS-5.2 → EXI-0002 → VM-0301.

HIS-4.5.2, HIS-5.3 → EXI-0003, EXI-0015 → VM-0405, VM-1801.

CPF-7.1, CPF-7.2 → EXI-0011, EXI-0047 → VM-0402, VM-1501.

HIS-7.4 → EXI-0014 → VM-0405.

## 9.4 Acceptation atomique

### VM-3204
Intégration de la ligne compilée

## OBJET

Le composant intègre un objet compilé valide dans le programme courant.

## RESPONSABILITÉS

Il :

- ajoute une ligne nouvelle ;
- remplace une ligne existante ;
- maintient l'ordre logique ;
- conserve l'état antérieur en cas d'échec.

## EXIGENCES SATISFAITES

- EXI-0004 ;
- EXI-0005 ;
- EXI-0006 ;
- EXI-0007 ;
- EXI-0008 ;
- EXI-0020 ;
- EXI-0026.

## CONTRAINTES

L'intégration est atomique.

Une compilation échouée ne modifie pas le programme courant.

Un numéro de ligne est unique.

Le remplacement ne provoque pas la recompilation des autres lignes.

## INTERFACES

`AJOUTER(objet_compilé)`

`REMPLACER(numero, objet_compilé)`

`SUPPRIMER(numero)`

`LIRE_PROGRAMME_COURANT()`

## DONNÉES MANIPULÉES

- numéro de ligne ;
- objet compilé ;
- programme courant.

## ALGORITHMES UTILISÉS

Pour une ligne acceptée :

1. rechercher son numéro ;
2. créer ou remplacer l'objet correspondant ;
3. maintenir l'ordre croissant ;
4. publier le nouvel état du programme courant.

## INVARIANTS

Le programme courant ne contient jamais deux objets pour le même numéro.

Une opération d'intégration réussie laisse un programme courant cohérent.

## CONSÉQUENCES

Le moteur d'exécution lit immédiatement la nouvelle version lors d'une exécution ultérieure.

Le décompilateur voit la version courante.

## TRAÇABILITÉ

HIS-6.2 → EXI-0001, EXI-0006, EXI-0007, EXI-0008 → VM-0303, VM-0305, VM-0306.

HIS-6.5 → EXI-0004, EXI-0020, EXI-0026 → VM-0302, VM-0504, VM-0505, VM-0607.

## 9.5 Compilation depuis une source indirecte

### VM-3205
Compilation de lignes fournies indirectement

## OBJET

Le composant permet au bibliothécaire ou à un dispositif d'entrée de fournir des lignes au compilateur sans modifier le mécanisme logique de compilation.

## RESPONSABILITÉS

Il :

- reçoit des lignes provenant d'une source indirecte ;
- les soumet une à une au compilateur ;
- relaie les diagnostics ;
- applique les mêmes règles d'acceptation que pour l'entrée conversationnelle.

## EXIGENCES SATISFAITES

- EXI-0021 ;
- EXI-0051.

## CONTRAINTES

La provenance physique de la ligne ne modifie pas sa sémantique de compilation.

Une ligne incorrecte est rejetée.

## INTERFACES

`FOURNIR_LIGNE(ligne)`

`RECEVOIR_RESULTAT_COMPILATION()`

`RECEVOIR_DIAGNOSTIC()`

## DONNÉES MANIPULÉES

- lignes source ;
- objets compilés ;
- diagnostics.

## ALGORITHMES UTILISÉS

Le traitement est identique au traitement d'une ligne interactive après validation.

## INVARIANTS

Une source indirecte ne peut injecter directement un objet compilé non validé.

## CONSÉQUENCES

ENCODER peut être architecturalement réalisé sans introduire un second compilateur.

## TRAÇABILITÉ

HIS-6.5, HIS-11.5 → EXI-0021 → VM-0506.

---

# 10. Le décompilateur

Le décompilateur exploite la représentation compilée conservée par le programme courant ou restaurée depuis un fichier-programme.

Il ne reconstruit pas nécessairement le texte source original caractère par caractère. Il produit une représentation textuelle valide et suffisamment complète pour le listage et les opérations documentées.

## 10.1 Décompilation d'un objet compilé

### VM-3301
Décompilateur

## OBJET

Le décompilateur transforme un objet compilé en représentation textuelle du langage.

## RESPONSABILITÉS

Il :

- parcourt la représentation compilée ;
- restitue les constructions ;
- restitue les expressions ;
- restitue les instructions ;
- restitue les informations nécessaires au listage ;
- produit un texte déterministe.

## EXIGENCES SATISFAITES

- EXI-0003 ;
- EXI-0010 ;
- EXI-0015 ;
- EXI-0022.

## CONTRAINTES

Le décompilateur travaille sur la représentation compilée.

Il ne dépend pas d'une copie obligatoire du texte source.

Il doit restituer une forme textuelle valide.

## INTERFACES

`DECOMPILER(objet_compilé)`

Retourne une ligne textuelle.

`DECOMPILER_PROGRAMME(programme)`

Retourne une séquence ordonnée de lignes textuelles.

## DONNÉES MANIPULÉES

- objets compilés ;
- opérations ;
- valeurs ;
- références ;
- informations de contrôle.

## ALGORITHMES UTILISÉS

Le décompilateur parcourt la représentation compilée.

Les éléments sont transformés dans l'ordre inverse de leur production syntaxique.

Pour une expression post-fixée :

1. les valeurs sont transformées en fragments ;
2. les références sont transformées en identificateurs ;
3. les opérations combinent les fragments ;
4. le résultat est produit selon une syntaxe valide.

## INVARIANTS

Un objet compilé décompilable produit une représentation textuelle valide.

La décompilation ne modifie pas l'objet compilé.

## CONSÉQUENCES

Le listage peut être produit sans texte source conservé séparément.

Le programme chargé depuis un fichier-programme reste décompilable.

## TRAÇABILITÉ

HIS-4.5.2, HIS-5.3 → EXI-0003, EXI-0015 → VM-0405, VM-1801.

CPF-7.3 → EXI-0015 → VM-0406.

HIS-11.2 → EXI-0022 → VM-1701.

## 10.2 Reconstruction des expressions

### VM-3302
Reconstruteur d'expressions

## OBJET

Le composant reconstruit les expressions textuelles à partir de la représentation compilée.

## RESPONSABILITÉS

Il :

- reconnaît les valeurs ;
- reconnaît les références ;
- respecte la précédence logique ;
- restitue les parenthèses nécessaires ;
- produit une expression valide.

## EXIGENCES SATISFAITES

- EXI-0011 ;
- EXI-0015 ;
- EXI-0047.

## CONTRAINTES

La représentation post-fixée est interprétée comme une représentation abstraite de l'expression.

La restitution doit préserver la sémantique.

## INTERFACES

`DECOMPILER_EXPRESSION(sequence)`

`PRODUIRE_OPERANDE(operande)`

`PRODUIRE_OPERATION(operation, operandes)`

## DONNÉES MANIPULÉES

- expressions ;
- opérateurs ;
- valeurs ;
- références.

## ALGORITHMES UTILISÉS

Le composant utilise une structure abstraite de reconstruction d'expression.

Chaque élément produit un fragment.

Une opération combine les fragments selon sa priorité et son associativité.

Lorsque la suppression des parenthèses serait ambiguë, le décompilateur les réintroduit.

## INVARIANTS

La représentation produite conserve la sémantique de l'expression compilée.

## CONSÉQUENCES

Le texte décompilé peut différer de l'entrée originale tout en restant équivalent au niveau du langage.

## TRAÇABILITÉ

CPF-7.1, CPF-7.3 → EXI-0011, EXI-0015 → VM-0402, VM-0406.

## 10.3 Services de décompilation

### VM-3303
Service de restitution textuelle

## OBJET

Le service adapte la décompilation aux usages du système : listage, sortie textuelle ou production d'un fichier de données.

## RESPONSABILITÉS

Il :

- demande la décompilation ;
- organise les lignes produites ;
- les transmet au destinataire ;
- garantit que la décompilation ne modifie pas le programme.

## EXIGENCES SATISFAITES

- EXI-0003 ;
- EXI-0015 ;
- EXI-0022.

## CONTRAINTES

La sortie textuelle est produite à partir de la forme compilée.

Les usages de sortie ne modifient pas la représentation compilée.

## INTERFACES

`LISTER(programme)`

`PRODUIRE_SORTIE_TEXTUELLE(programme)`

`DECODER(programme, destination)`

## DONNÉES MANIPULÉES

- programme compilé ;
- lignes textuelles ;
- destination logique.

## ALGORITHMES UTILISÉS

1. parcourir le programme dans son ordre logique ;
2. décompiler chaque objet ;
3. produire les lignes ;
4. les transmettre au destinataire.

## INVARIANTS

Le listage respecte l'ordre logique des numéros de ligne.

## CONSÉQUENCES

LISTER et DECODER peuvent partager le même décompilateur.

PERFORER peut utiliser la même représentation textuelle intermédiaire sans modifier le programme.

## TRAÇABILITÉ

HIS-4.5.2 → EXI-0003 → VM-1801.

HIS-11.2 → EXI-0022 → VM-1701, VM-1801.

---

# 11. Le bibliothécaire

Le bibliothécaire assure la persistance et la restauration des programmes sous forme compilée.

La représentation persistante est distincte de la mémoire d'exécution et du programme courant actif, même si elle représente logiquement les mêmes objets compilés.

## 11.1 Gestionnaire de programme persistant

### VM-3401
Bibliothécaire

## OBJET

Le bibliothécaire gère la conservation et la récupération des programmes compilés.

## RESPONSABILITÉS

Il :

- range un programme courant ;
- localise une représentation persistante ;
- lit un fichier-programme ;
- restitue des objets compilés ;
- transmet les erreurs de bibliothèque.

## EXIGENCES SATISFAITES

- EXI-0009 ;
- EXI-0022 ;
- EXI-0025 ;
- EXI-0049.

## CONTRAINTES

Le rangement ne nécessite pas le texte source.

Le chargement ne nécessite pas une nouvelle compilation.

Le programme chargé devient le programme courant uniquement après validation de sa cohérence.

## INTERFACES

`RANGER(programme_courant, destination)`

`CHERCHER(source)`

`LIRE(source)`

`VALIDER(representation)`

`RESTAURER(representation)`

## DONNÉES MANIPULÉES

- représentation persistante ;
- objets compilés ;
- programme courant ;
- diagnostics de chargement.

## ALGORITHMES UTILISÉS

Rangement :

1. lire le programme courant ;
2. produire sa représentation persistante ;
3. transmettre cette représentation au stockage logique.

Chargement :

1. lire la représentation ;
2. vérifier sa cohérence ;
3. reconstruire la collection d'objets compilés ;
4. publier le nouveau programme courant.

## INVARIANTS

Un programme non validé ne devient jamais programme courant.

Un échec de chargement conserve le programme courant précédent.

## CONSÉQUENCES

La bibliothèque est indépendante du moteur d'exécution.

Un programme persistant peut être exécuté sans recompilation.

## TRAÇABILITÉ

HIS-11.2, HIS-11.5 → EXI-0009, EXI-0022, EXI-0025 → VM-0307, VM-0606, VM-1701, VM-1702.

## 11.2 Représentation persistante

### VM-3402
Fichier-programme logique

## OBJET

Le fichier-programme logique représente la forme persistante d'un programme compilé.

## RESPONSABILITÉS

Il conserve :

- les objets compilés ;
- leur ordre logique ;
- les informations nécessaires à leur restauration ;
- les informations nécessaires à leur décompilation.

## EXIGENCES SATISFAITES

- EXI-0009 ;
- EXI-0022 ;
- EXI-0049.

## CONTRAINTES

Aucun format matériel ou binaire historique n'est imposé.

La représentation doit rester exploitable par le bibliothécaire.

## INTERFACES

`ECRIRE(programme_compilé)`

`LIRE()`

`VALIDER()`

## DONNÉES MANIPULÉES

- collection ordonnée d'objets compilés ;
- identifiants de lignes ;
- représentation compilée.

## ALGORITHMES UTILISÉS

La conservation est une transformation de la représentation compilée vers une représentation persistante.

La restauration réalise la transformation inverse.

## INVARIANTS

La restauration d'un fichier valide produit un programme compilé équivalent au programme rangé.

## CONSÉQUENCES

Le texte source n'est pas une donnée obligatoire.

## TRAÇABILITÉ

HIS-11.2 → EXI-0009 → VM-0307.

HIS-3.14, HIS-11.4 → EXI-0049 → VM-1601, VM-1701.

## 11.3 Remplacement du programme courant

### VM-3403
Restaurateur de programme courant

## OBJET

Le composant remplace le programme courant par le programme restauré à partir d'un fichier-programme valide.

## RESPONSABILITÉS

Il :

- prépare le nouveau programme ;
- vérifie sa cohérence ;
- publie le nouveau programme courant ;
- conserve l'ancien programme tant que le nouveau n'est pas validé.

## EXIGENCES SATISFAITES

- EXI-0025 ;
- EXI-0026.

## CONTRAINTES

Le remplacement doit être atomique.

Un échec laisse le programme courant antérieur inchangé.

## INTERFACES

`PREPARER_REMPLACEMENT(programme)`

`VALIDER_REMPLACEMENT()`

`PUBLIER_REMPLACEMENT()`

`ANNULER_REMPLACEMENT()`

## DONNÉES MANIPULÉES

- ancien programme courant ;
- nouveau programme ;
- état de validation.

## ALGORITHMES UTILISÉS

1. charger ;
2. valider ;
3. préparer le remplacement ;
4. publier ;
5. libérer l'ancien programme selon les règles mémoire.

## INVARIANTS

Le programme courant observable est toujours cohérent.

## CONSÉQUENCES

Le bibliothécaire ne dépend pas d'une mémoire physique particulière.

## TRAÇABILITÉ

HIS-11.5 → EXI-0025 → VM-0606, VM-1702.

---


## 11.4 Gestionnaire du catalogue des fichiers

### VM-3404
Catalogue logique des fichiers

### OBJET

Le composant maintient l'identification logique des fichiers de la
bibliothèque.

### RESPONSABILITÉS

Il :

- recherche un fichier par son identité logique ;
- distingue fichier-programme et fichier-données ;
- distingue fichier temporaire et permanent ;
- conserve l'identification de création d'un fichier permanent et les protections logiques qui lui sont associées ;
- fournit au bibliothécaire la ressource correspondant à une demande.

Le contrôle des droits n'est pas réalisé implicitement par le seul catalogue : il relève du gestionnaire des identifications et des autorisations.

### EXIGENCES SATISFAITES

- EXI-0053 ;
- EXI-0054 ;
- EXI-0055 ;
- EXI-0062.

### INTERFACES

`RECHERCHER_FICHIER(identité)`

`ENREGISTRER_FICHIER(fichier)`

`SUPPRIMER_FICHIER(identité)`

`CATALOGUER_FICHIER(fichier)`

Le descripteur retourné contient l'identification de création et les attributs de protection nécessaires au contrôle d'accès.

### INVARIANTS

Un fichier temporaire reste attaché à sa session.

Un fichier permanent appartient à la bibliothèque.

### TRAÇABILITÉ

HIS-11.1, HIS-11.3 → EXI-0053, EXI-0054, EXI-0055 → VM-3404.

---

## 11.5 Gestionnaire des fichiers-données

### VM-3405
Gestionnaire des fichiers-données

### OBJET

Le composant réalise les opérations logiques sur les fichiers-données.

### RESPONSABILITÉS

Il :

- crée ou identifie un fichier-données ;
- lit et écrit son contenu ;
- lit et écrit les enregistrements du fichier ;
- détruit un fichier temporaire ;
- transforme un fichier temporaire en fichier permanent ;
- libère l'espace temporaire après catalogage ;
- demande la vérification des droits avant toute opération permanente soumise à autorisation.

Les instructions de programme portant sur les fichiers-données sont
réalisées par ce composant :

- GARER écrit une valeur dans l'enregistrement désigné ;
- CHARGER lit l'enregistrement désigné et restitue son type et son contenu ;
- SUPPRIMER supprime l'enregistrement désigné ou, lorsque la désignation
  d'enregistrement est absente, le fichier concerné.

### EXIGENCES SATISFAITES

- EXI-0053 ;
- EXI-0056 ;
- EXI-0057 ;
- EXI-0062.

### INTERFACES

`LIRE_DONNEE(fichier)`

`ECRIRE_DONNEE(fichier, donnée)`

`GARER(fichier, numéro_enregistrement, valeur)`

`CHARGER(fichier, numéro_enregistrement)`

`SUPPRIMER(fichier, numéro_enregistrement)`

`CATALOGUER(fichier_temporaire)`

### INVARIANTS

Un enregistrement est une unité logique du fichier contenant une constante,
un tableau à une ou deux dimensions ou une chaîne. Les informations de
types différents ne sont pas mélangées dans un même enregistrement.

GARER remplace le contenu d'un enregistrement existant de même numéro.

CHARGER restitue le type et le contenu correspondant à l'enregistrement.

Lorsque le numéro d'enregistrement n'est pas fourni à SUPPRIMER,
l'opération porte sur le fichier dans son ensemble.

Un catalogage réussi ne laisse pas subsister le fichier dans la catégorie
temporaire.

Une opération échouée ne produit pas de fichier permanent partiellement
constitué.

### TRAÇABILITÉ

HIS-11.3, HIS-11.5 → EXI-0053, EXI-0056, EXI-0057 → VM-3405.

---

## 11.6 Traitement ENCODER et DECODER

### VM-3406
Transformateur programme-donnée

### OBJET

Le composant assure les transformations documentées entre programme
courant et fichier-données.

### RESPONSABILITÉS

Il réalise :

`DECODER : programme courant → fichier-données`

`ENCODER : fichier-données → programme courant`

### EXIGENCES SATISFAITES

- EXI-0057.

### INTERFACES

`DECODER(programme_courant, destination)`

`ENCODER(source, programme_courant)`

### INVARIANTS

Une transformation échouée ne publie pas un programme courant invalide.

### TRAÇABILITÉ

HIS-11.5 → EXI-0057 → VM-3406, VM-1707.

---

## 11.7 Catalogage et réutilisation de l'espace temporaire

### VM-3407
Catalogueur de fichier-données

### OBJET

Le composant réalise le passage d'un fichier-données temporaire à la
bibliothèque permanente.

### RESPONSABILITÉS

Il :

1. valide le fichier temporaire ;
2. crée la représentation permanente ;
3. l'enregistre dans le catalogue ;
4. supprime l'entrée temporaire ;
5. libère l'espace temporaire.

### EXIGENCES SATISFAITES

- EXI-0056.

### TRAÇABILITÉ

HIS-11.3 → EXI-0056 → VM-3407.


## 11.8 Gestion des identifications et des autorisations

### VM-3408
Gestionnaire des identifications et des autorisations

### OBJET

Le composant maintient l'identification courante de chaque session et applique les règles d'autorisation documentées pour les opérations sur les fichiers.

### RESPONSABILITÉS

Il :

- reçoit les demandes d'identification ;
- vérifie qu'une identification est légale ;
- établit l'identification courante de la session ;
- conserve les attributs de droits associés à cette identification ;
- vérifie l'autorisation d'une opération sur un fichier ;
- distingue l'identification courante de l'identification sous laquelle un fichier permanent a été créé ;
- applique les protections spécifiques au profil historique T1600 ou MITRA 15.

### EXIGENCES SATISFAITES

- EXI-0062 ;
- EXI-0061 pour les différences de profil explicitement documentées.

### INTERFACES

`VALIDER_IDENTIFICATION(compte, clé)`

`FIXER_IDENTIFICATION_COURANTE(session, identification)`

`LIRE_IDENTIFICATION_COURANTE(session)`

`VERIFIER_AUTORISATION(identification, fichier, opération)`

### CONTRAINTES

Une identification refusée ne modifie pas l'identification courante.

Une opération permanente soumise à autorisation est refusée si l'identification courante ne possède pas le pouvoir requis.

La modification ou la suppression d'un fichier permanent est refusée si l'identification courante diffère de l'identification de création.

Les protections de lecture et d'exécution sont appliquées selon les règles historiques du fichier et du profil d'implantation.

### INVARIANTS

- une session possède au plus une identification courante ;
- une clé n'est pas réémise dans les sorties ordinaires ;
- une opération refusée ne modifie pas l'objet protégé ;
- aucun droit non documenté n'est ajouté par le gestionnaire.

### STATUT

Déduit.

### TRAÇABILITÉ

HIS-11.3.1 → EXI-0062 → VM-3408.


# 12. Le gestionnaire des variables

Le gestionnaire des variables fournit les mécanismes permettant de résoudre les identificateurs et d'accéder aux valeurs des objets.

Il constitue l'interface entre :

- la représentation compilée ;
- les cadres d'activation ;
- les objets globaux ;
- les objets locaux ;
- le moteur d'exécution ;
- le dialogue avec le moniteur.

## 12.1 Résolution des identificateurs

### VM-3501
Résolveur d'objets

## OBJET

Le résolveur transforme une identité logique en objet d'exécution accessible dans le contexte courant.

## RESPONSABILITÉS

Il :

- recherche une identité ;
- respecte la portée applicable ;
- retourne l'objet trouvé ;
- signale l'absence d'objet valide.

## EXIGENCES SATISFAITES

- EXI-0032 ;
- EXI-0038.

## CONTRAINTES

La résolution dépend du contexte.

Les objets locaux sont recherchés selon les règles des cadres actifs.

Un identificateur n'est pas une adresse.

## INTERFACES

`RESOUDRE(identite, contexte)`

Retourne un objet ou une erreur.

`RECHERCHER_DANS_CADRE(cadre, identite)`

`RECHERCHER_EXTERIEUR(identite)`

## DONNÉES MANIPULÉES

- identificateurs ;
- cadres ;
- objets ;
- règles de portée.

## ALGORITHMES UTILISÉS

1. rechercher dans le contexte le plus proche ;
2. remonter les cadres selon les règles applicables ;
3. rechercher dans l'environnement extérieur lorsque cela est permis ;
4. signaler l'échec.

## INVARIANTS

Une résolution réussie fournit un objet unique pour le contexte considéré.

## CONSÉQUENCES

La compilation peut produire une référence logique plutôt qu'une adresse.

## TRAÇABILITÉ

HIS-10.1, HIS-10.2 → EXI-0032, EXI-0038 → VM-0802, VM-1101.

HIS-13.1 → EXI-0032 → VM-1301.

## 12.2 Accès aux valeurs

### VM-3502
Gestionnaire de valeur variable

## OBJET

Le composant fournit les opérations de lecture et d'écriture sur les valeurs des variables.

## RESPONSABILITÉS

Il :

- lit une valeur ;
- modifie une valeur ;
- vérifie que la variable est accessible ;
- vérifie que la valeur est compatible avec le type logique de l'objet.

## EXIGENCES SATISFAITES

- EXI-0039.

## CONTRAINTES

Une variable possède une valeur courante.

La modification est possible uniquement lorsque l'opération l'autorise.

L'écriture ne doit pas détruire l'identité logique de la variable.

## INTERFACES

`LIRE_VALEUR(variable)`

`ECRIRE_VALEUR(variable, valeur)`

`VERIFIER_ACCESS(variable)`

## DONNÉES MANIPULÉES

- variables ;
- valeurs ;
- types logiques.

## ALGORITHMES UTILISÉS

Lecture :

1. résoudre ;
2. vérifier l'accès ;
3. retourner la valeur.

Écriture :

1. résoudre ;
2. vérifier l'accès ;
3. vérifier la compatibilité ;
4. remplacer la valeur courante.

## INVARIANTS

Une variable accessible possède une valeur courante.

Une modification réussie ne modifie pas l'identité de la variable.

## CONSÉQUENCES

Les mêmes mécanismes peuvent être utilisés par :

- les instructions du programme ;
- les expressions ;
- le dialogue avec le moniteur.

## TRAÇABILITÉ

HIS-10.1 → EXI-0039 → VM-1102.

HIS-3.13 → EXI-0039 → VM-0803, VM-2001.

## 12.3 Cycle de vie des objets variables

### VM-3503
Gestionnaire de durée de vie des variables

## OBJET

Le composant gère la création, l'existence et la disparition des objets variables dans leur contexte.

## RESPONSABILITÉS

Il :

- crée les objets nécessaires ;
- associe les identificateurs ;
- détruit les objets devenus inaccessibles ;
- respecte la durée de vie des cadres.

## EXIGENCES SATISFAITES

- EXI-0034 ;
- EXI-0035 ;
- EXI-0050.

## CONTRAINTES

Les variables locales appartiennent au cadre qui les possède.

La fin d'une activation entraîne la fin logique des objets locaux dont la durée de vie dépend de cette activation.

Un objet libéré ne reste pas accessible.

## INTERFACES

`CREER_VARIABLE(definition)`

`SUPPRIMER_VARIABLE(variable)`

`FERMER_CONTEXTE(cadre)`

`OUVRIR_CONTEXTE(cadre)`

## DONNÉES MANIPULÉES

- variables ;
- identificateurs ;
- cadres ;
- valeurs.

## ALGORITHMES UTILISÉS

Lors de l'ouverture d'un cadre :

1. créer les objets locaux ;
2. associer les identités ;
3. initialiser les valeurs selon les règles du langage.

Lors de la fermeture :

1. désactiver les identités locales ;
2. libérer les objets concernés ;
3. invalider les références locales.

## INVARIANTS

Un objet local n'est accessible que pendant sa durée de vie.

La fermeture d'un cadre invalide les références qui ne peuvent plus être résolues.

## CONSÉQUENCES

La durée de vie des objets est indépendante de leur représentation matérielle.

## TRAÇABILITÉ

HIS-13.1 → EXI-0034, EXI-0035 → VM-0901, VM-0902.

HIS-10.6 → EXI-0050 → VM-1602.

---

# 13. Les procédures

Les procédures sont des unités appelables qui peuvent recevoir des paramètres et posséder des objets locaux.

Le Tome III sépare :

- la définition compilée ;
- la résolution de l'appel ;
- la création du cadre ;
- le transfert du flux ;
- le retour.

## 13.1 Définition compilée d'une procédure

### VM-3601
Répertoire des procédures

## OBJET

Le répertoire des procédures associe l'identité logique d'une procédure à sa définition compilée.

## RESPONSABILITÉS

Il :

- identifie les procédures ;
- fournit leur représentation compilée ;
- fournit la liste logique des paramètres ;
- fournit les informations nécessaires à la création d'un cadre.

## EXIGENCES SATISFAITES

- EXI-0042.

## CONTRAINTES

La définition doit être disponible lorsque l'appel est exécuté.

La représentation ne dépend pas du texte source original.

## INTERFACES

`RESOUDRE_PROCEDURE(identite)`

`LIRE_DEFINITION(procedure)`

## DONNÉES MANIPULÉES

- identité de procédure ;
- corps compilé ;
- paramètres ;
- informations de contexte.

## ALGORITHMES UTILISÉS

La résolution recherche la définition applicable dans l'environnement courant.

## INVARIANTS

Une procédure résolue possède un corps compilé valide.

## CONSÉQUENCES

Le moteur peut appeler une procédure sans analyser de nouveau son texte.

## TRAÇABILITÉ

HIS-13.1, HIS-13.2 → EXI-0042 → VM-1301.

## 13.2 Appel de procédure

### VM-3602
Gestionnaire d'appel de procédure

## OBJET

Le gestionnaire d'appel transforme une opération d'appel en activation d'une procédure.

## RESPONSABILITÉS

Il :

- résout la procédure ;
- évalue les arguments ;
- crée le cadre ;
- initialise les paramètres ;
- établit la position de retour ;
- transfère le flux.

## EXIGENCES SATISFAITES

- EXI-0042 ;
- EXI-0043 ;
- EXI-0034 ;
- EXI-0035 ;
- EXI-0036.

## CONTRAINTES

Le cadre appelant reste identifiable.

Les arguments sont évalués avant l'activation.

Le corps appelé devient la nouvelle cible d'exécution.

## INTERFACES

`APPELER_PROCEDURE(procedure, arguments)`

`CREER_CADRE_APPEL()`

`INITIALISER_PARAMETRES()`

`TRANSFERER_AU_CORPS()`

## DONNÉES MANIPULÉES

- définition ;
- arguments ;
- paramètres ;
- cadre appelant ;
- position de retour.

## ALGORITHMES UTILISÉS

1. résoudre ;
2. évaluer les arguments ;
3. créer le cadre ;
4. initialiser ;
5. enregistrer le retour ;
6. activer ;
7. transférer.

## INVARIANTS

Une procédure appelée possède toujours un cadre actif.

La position de retour est définie avant le transfert.

## CONSÉQUENCES

Les appels imbriqués sont possibles.

Le moteur d'exécution n'a pas besoin de gérer directement les variables locales.

## TRAÇABILITÉ

HIS-13.1 → EXI-0043 → VM-1302.

HIS-13.1, HIS-13.2 → EXI-0034, EXI-0035, EXI-0036 → VM-0901, VM-0902, VM-0903.

## 13.3 Retour d'une procédure

### VM-3603
Gestionnaire de retour de procédure

## OBJET

Le gestionnaire de retour termine l'activation courante et restitue le contrôle au contexte appelant.

## RESPONSABILITÉS

Il :

- identifie le cadre appelant ;
- récupère la position de retour ;
- ferme le cadre courant ;
- restaure l'appelant ;
- restaure le flux.

## EXIGENCES SATISFAITES

- EXI-0035 ;
- EXI-0036 ;
- EXI-0044.

## CONTRAINTES

Le retour doit restaurer le contexte appelant.

Les objets locaux du cadre terminé cessent d'être accessibles.

## INTERFACES

`RETOUR_PROCEDURE()`

`LIRE_POSITION_RETOUR()`

`FERMER_CADRE()`

`RESTAURER_APPELANT()`

## DONNÉES MANIPULÉES

- cadre courant ;
- cadre appelant ;
- position de retour.

## ALGORITHMES UTILISÉS

1. lire la position ;
2. fermer le contexte ;
3. restaurer l'appelant ;
4. positionner le moteur ;
5. poursuivre.

## INVARIANTS

Après retour, le cadre terminé n'est plus courant.

## CONSÉQUENCES

Le mécanisme de retour est indépendant de la représentation matérielle des cadres.

## TRAÇABILITÉ

HIS-13.2 → EXI-0036, EXI-0044 → VM-0903, VM-1303.

---

# 14. Les fonctions

Une fonction est une unité appelable dont l'exécution produit une valeur utilisable par le contexte appelant.

L'architecture d'une fonction réutilise les mécanismes communs des procédures :

- résolution de définition ;
- évaluation des arguments ;
- création de cadre ;
- exécution du corps ;
- retour.

La différence architecturale essentielle réside dans la restitution d'un résultat.

## 14.1 Définition compilée d'une fonction

### VM-3701
Répertoire des fonctions

## OBJET

Le répertoire des fonctions associe une identité logique de fonction à sa définition compilée et aux informations nécessaires à son invocation.

## RESPONSABILITÉS

Il :

- résout une fonction ;
- fournit son corps compilé ;
- fournit ses paramètres ;
- indique qu'un résultat est attendu.

## EXIGENCES SATISFAITES

- EXI-0042 ;
- EXI-0043 ;
- EXI-0044.

## CONTRAINTES

La définition doit être exécutable.

Le résultat doit pouvoir être remis au contexte appelant.

## INTERFACES

`RESOUDRE_FONCTION(identite)`

`LIRE_DEFINITION(fonction)`

`LIRE_PARAMETRES(fonction)`

## DONNÉES MANIPULÉES

- identité ;
- corps compilé ;
- paramètres ;
- informations de résultat.

## ALGORITHMES UTILISÉS

La résolution est analogue à celle d'une procédure, avec validation supplémentaire de la production d'un résultat.

## INVARIANTS

Toute fonction résolue possède une représentation exécutable et une définition de résultat.

## CONSÉQUENCES

Le compilateur peut représenter un appel de fonction comme une opération compilée qui ouvre un cadre et attend une valeur.

## TRAÇABILITÉ

HIS-13.1, HIS-13.2 → EXI-0042 → VM-1301, VM-1302, VM-1303.

## 14.2 Appel d'une fonction

### VM-3702
Gestionnaire d'appel de fonction

## OBJET

Le gestionnaire transforme un appel de fonction en activation produisant une valeur.

## RESPONSABILITÉS

Il :

- résout la fonction ;
- évalue les arguments ;
- crée le cadre ;
- initialise les paramètres ;
- établit la position de retour ;
- prépare le contexte de résultat ;
- exécute le corps.

## EXIGENCES SATISFAITES

- EXI-0034 ;
- EXI-0042 ;
- EXI-0043 ;
- EXI-0044 ;
- EXI-0048.

## CONTRAINTES

L'appel de fonction doit produire exactement une valeur lorsque la fonction atteint normalement son résultat.

Le résultat doit pouvoir être placé dans la pile d'évaluation du contexte appelant.

## INTERFACES

`APPELER_FONCTION(fonction, arguments)`

`CREER_CADRE_FONCTION()`

`PREPARER_RESULTAT()`

`RETOURNER_RESULTAT(valeur)`

## DONNÉES MANIPULÉES

- fonction ;
- arguments ;
- paramètres ;
- résultat ;
- cadre ;
- position de retour.

## ALGORITHMES UTILISÉS

1. résolution ;
2. évaluation des arguments ;
3. création du cadre ;
4. initialisation ;
5. transfert ;
6. exécution ;
7. production du résultat ;
8. restauration de l'appelant ;
9. remise du résultat.

## INVARIANTS

Une fonction active possède un contexte distinct.

Un résultat est associé au cadre qui l'a produit.

Le résultat est remis à l'appelant avant fermeture logique complète du contexte lorsque cela est nécessaire à sa transmission.

## CONSÉQUENCES

Les fonctions peuvent être utilisées :

- dans les expressions ;
- comme arguments d'autres appels ;
- dans les affectations ;
- dans les instructions qui consomment une valeur.

## TRAÇABILITÉ

HIS-13.1 → EXI-0043 → VM-1302.

HIS-13.2 → EXI-0044 → VM-1303.

HIS-8.1 → EXI-0048 → VM-1502.

## 14.3 Production et restitution du résultat

### VM-3703
Gestionnaire de résultat de fonction

## OBJET

Le gestionnaire de résultat assure le transfert de la valeur produite par une fonction vers le contexte appelant.

## RESPONSABILITÉS

Il :

- reçoit la valeur produite ;
- vérifie sa validité ;
- la conserve pendant la transition de retour ;
- la remet à la pile d'évaluation ou au destinataire logique du contexte appelant.

## EXIGENCES SATISFAITES

- EXI-0044 ;
- EXI-0048.

## CONTRAINTES

Le résultat doit être disponible après le retour.

La transmission du résultat ne doit pas dépendre d'une adresse mémoire.

## INTERFACES

`RECEVOIR_RESULTAT(valeur)`

`VALIDER_RESULTAT()`

`TRANSMETTRE_A_APPELANT()`

## DONNÉES MANIPULÉES

- valeur résultante ;
- cadre appelant ;
- contexte d'évaluation.

## ALGORITHMES UTILISÉS

1. recevoir la valeur ;
2. vérifier sa validité ;
3. terminer l'activation ;
4. restaurer l'appelant ;
5. empiler le résultat dans le contexte appelant lorsque celui-ci attend une valeur.

## INVARIANTS

Le résultat remis au contexte appelant est celui produit par l'activation correspondante.

Une fonction correctement terminée produit une valeur finale utilisable.

## CONSÉQUENCES

Le résultat d'une fonction peut être combiné avec d'autres opérations dans une expression post-fixée.

La pile d'évaluation constitue le mécanisme de transfert abstrait du résultat lorsque la fonction est utilisée comme opérande.

## TRAÇABILITÉ

HIS-13.2 → EXI-0044 → VM-1303.

CPF-7.2 → EXI-0037 → VM-1001.

HIS-8.1, HIS-15.1 → EXI-0048 → VM-1502.

## 14.4 Fonction comme opérande d'expression

### VM-3704
Intégration d'une fonction dans l'évaluation

## OBJET

Le composant assure l'intégration du résultat d'une fonction au mécanisme général d'évaluation des expressions.

## RESPONSABILITÉS

Il :

- reconnaît l'appel comme producteur de valeur ;
- suspend l'évaluation de l'expression appelante lorsque cela est nécessaire ;
- exécute la fonction ;
- récupère son résultat ;
- reprend l'évaluation avec la valeur produite.

## EXIGENCES SATISFAITES

- EXI-0037 ;
- EXI-0047 ;
- EXI-0048.

## CONTRAINTES

La valeur résultante doit remplacer logiquement l'appel dans l'expression.

L'ordre d'évaluation doit être compatible avec la représentation post-fixée reconstruite.

## INTERFACES

`EVALUER_APPEL_FONCTION(appel)`

`SUSPENDRE_EVALUATION()`

`REPRENDRE_EVALUATION(resultat)`

## DONNÉES MANIPULÉES

- appel de fonction ;
- pile d'évaluation ;
- résultat ;
- cadre d'activation.

## ALGORITHMES UTILISÉS

1. atteindre l'opération d'appel ;
2. évaluer les arguments ;
3. créer l'activation ;
4. exécuter le corps ;
5. obtenir le résultat ;
6. restaurer le contexte appelant ;
7. produire le résultat sur la pile ;
8. poursuivre l'expression.

## INVARIANTS

L'exécution d'une fonction ne détruit pas les valeurs intermédiaires appartenant à l'expression appelante qui doivent subsister.

Le résultat de la fonction occupe la position logique attendue par la suite de l'expression.

## CONSÉQUENCES

Les fonctions sont intégrées au même moteur d'exécution que les autres opérations.

Aucun mécanisme distinct d'interprétation des fonctions n'est nécessaire.

## TRAÇABILITÉ

CPF-7.2 → EXI-0037, EXI-0047 → VM-0402, VM-1001, VM-1501.

HIS-8.1 → EXI-0048 → VM-1502.

---

# Point de cohérence architectural à la fin du chapitre 14

À ce stade, la machine virtuelle possède les composants logiques suivants :

- un ensemble de registres virtuels représentant l'état dynamique ;
- un moteur d'exécution chargé du parcours de la représentation compilée ;
- un mécanisme d'évaluation post-fixée ;
- un gestionnaire de flux ;
- un mécanisme de suspension et de reprise ;
- un mécanisme de terminaison ;
- un compilateur produisant les objets compilés ;
- un décompilateur exploitant ces mêmes objets ;
- un bibliothécaire assurant leur persistance ;
- un gestionnaire des variables assurant la résolution et la modification des objets ;
- un mécanisme d'activation des procédures ;
- un mécanisme d'activation et de restitution des fonctions.

Les interfaces de ces composants reposent sur des objets abstraits communs :

- objet compilé ;
- programme courant ;
- représentation persistante ;
- état d'exécution ;
- registre de position ;
- pile d'évaluation ;
- cadre d'activation ;
- variable ;
- tableau ;
- chaîne ;
- valeur numérique ;
- résultat ;
- diagnostic.

La séparation établie ici ne modifie aucune propriété fonctionnelle du Tome II. Elle en précise l'organisation logique et fixe les responsabilités nécessaires à une implémentation future, sans prescrire de langage, de processeur, de système d'exploitation, de représentation binaire ou de mécanisme matériel.

Le prochain composant du plan normatif est :

# 15. Les tableaux

Les tableaux constituent des objets d'exécution composés dont la durée de vie dépend de leur déclaration et de leur libération. Le Tome II établit qu'ils possèdent une ou deux dimensions, que leur espace d'exécution est acquis lorsque leur déclaration est exécutée, qu'un tableau non déclaré ne peut être utilisé et que la libération restitue l'espace associé. Le présent tome transforme ces propriétés en une organisation de composants indépendants de toute représentation mémoire particulière. [INRDP75 §4-1] [LSE15 §3-1] [NOYELLE89 p.219]

Le composant chargé des tableaux ne constitue pas un espace mémoire autonome. Il coordonne les objets tableaux avec le gestionnaire des variables, l'environnement d'exécution et la mémoire de travail.

## 15.1 Gestionnaire des tableaux

### OBJET

Le gestionnaire des tableaux assure la création, l'identification, l'accès, le remplacement et la libération des objets tableaux nécessaires à l'exécution d'un programme.

Il constitue l'interface logique entre les opérations du langage portant sur les tableaux et les mécanismes généraux de l'environnement d'exécution.

Le gestionnaire ne connaît ni la représentation physique de la mémoire, ni l'organisation matérielle des objets. Il manipule uniquement des objets tableaux abstraits.

### RESPONSABILITÉS

Le gestionnaire des tableaux assume les responsabilités suivantes :

- reconnaître une déclaration de tableau comme une demande de création ou de redéfinition d'un objet tableau ;
- déterminer la dimension du tableau à partir de la déclaration exécutée ;
- conserver les bornes valides des indices ;
- demander à la mémoire de travail les ressources nécessaires ;
- associer l'objet créé à son identificateur dans l'environnement d'exécution ;
- fournir l'accès à un élément désigné par un ou deux indices ;
- vérifier la validité du nombre d'indices fournis ;
- vérifier la validité de chaque indice ;
- signaler une erreur lorsque le tableau n'existe pas dans le contexte applicable ;
- libérer un tableau sur demande explicite ;
- rendre possible la réutilisation ultérieure de l'identificateur après libération selon les règles du langage.

Le gestionnaire ne réalise pas lui-même l'allocation physique. La création de l'objet tableau et la réservation de ses ressources sont coordonnées avec la mémoire de travail.

### EXIGENCES SATISFAITES

EXI-0040 — Tableaux à une ou deux dimensions.

EXI-0041 — Allocation à la déclaration du tableau.

EXI-0050 — Allocation et libération des objets d'exécution.

EXI-0038 — Identité stable des variables.

EXI-0039 — Valeur mutable des variables.

### CONTRAINTES

Un tableau possède une ou deux dimensions, et aucune troisième dimension ne doit être introduite dans l'architecture de la machine virtuelle.

Les indices valides commencent à la valeur minimale définie par le langage. Dans les descriptions documentées du L.S.E. retenues par le Tome II, cette valeur est 1. La limite maximale de chaque dimension est issue de la déclaration exécutée. [LSE15 §3-1]

Une déclaration de tableau peut être rencontrée pendant l'exécution du programme. L'existence effective du tableau est donc une propriété de l'état d'exécution et non une propriété permanente du programme compilé.

Le nom d'un tableau est résolu dans le contexte applicable. Une identité de tableau ne doit jamais être confondue avec une adresse mémoire.

Le tableau doit rester accessible tant qu'il appartient au contexte qui le possède et qu'il n'a pas été explicitement libéré.

La libération d'un tableau invalide toute référence d'accès ultérieure qui ne correspondrait plus à un objet valide.

### INTERFACES

Le gestionnaire des tableaux expose les interfaces logiques suivantes :

**CréerTableau**

Entrées :

- identificateur logique ;
- dimension ;
- limites maximales des indices.

Résultat :

- objet tableau créé et associé au contexte ;
- ou erreur d'allocation ou de conflit d'identité.

**RésoudreTableau**

Entrées :

- identificateur logique ;
- contexte d'exécution.

Résultat :

- objet tableau applicable ;
- ou erreur si aucun tableau accessible ne correspond.

**LireÉlément**

Entrées :

- objet tableau ;
- indice ou couple d'indices.

Résultat :

- valeur de l'élément ;
- ou erreur d'indice.

**ÉcrireÉlément**

Entrées :

- objet tableau ;
- indice ou couple d'indices ;
- nouvelle valeur.

Résultat :

- mise à jour de l'élément ;
- ou erreur.

**LibérerTableau**

Entrée :

- identificateur ou objet tableau.

Résultat :

- libération de l'objet ;
- ou erreur si l'objet n'est pas libérable dans le contexte courant.

**DécrireTableau**

Entrée :

- objet tableau.

Résultat :

- dimension ;
- bornes ;
- état d'existence ;
- contexte propriétaire.

Cette dernière interface est réservée aux composants qui doivent contrôler ou observer l'état d'exécution. Elle ne constitue pas une interface de langage supplémentaire.

### DONNÉES MANIPULÉES

Le gestionnaire manipule :

- l'identificateur logique du tableau ;
- la dimension ;
- les bornes des indices ;
- la référence abstraite vers l'objet tableau ;
- les valeurs des éléments ;
- le contexte d'appartenance ;
- l'état d'allocation ;
- l'état de libération.

Une représentation abstraite minimale d'un tableau est :

**Tableau**

- Identité logique ;
- Dimension ∈ {1, 2} ;
- Limite 1 ;
- Limite 2, lorsqu'applicable ;
- Collection logique de valeurs ;
- Contexte propriétaire ;
- État d'existence.

Aucune adresse physique, aucun emplacement de mots mémoire et aucune technique d'allocation ne font partie de cette structure logique.

### ALGORITHMES UTILISÉS

Le traitement d'une déclaration de tableau suit l'enchaînement logique suivant :

1. résoudre l'identité applicable ;
2. déterminer la dimension ;
3. évaluer les bornes déclarées ;
4. vérifier que chaque borne est valide ;
5. demander à la mémoire de travail la création de l'objet ;
6. enregistrer l'objet dans l'environnement d'exécution ;
7. rendre le tableau accessible aux opérations suivantes.

L'accès à un élément suit l'enchaînement :

1. résoudre le tableau ;
2. vérifier que le tableau existe ;
3. vérifier que le nombre d'indices correspond à la dimension ;
4. évaluer les indices ;
5. vérifier chaque indice ;
6. calculer la position logique de l'élément ;
7. lire ou modifier la valeur.

La position logique d'un élément est déterminée par les indices et les limites déclarées. L'algorithme ne dépend pas de l'ordre physique dans lequel les valeurs sont conservées.

La libération suit l'enchaînement :

1. résoudre l'identificateur ;
2. vérifier que l'objet est un tableau libérable ;
3. retirer son association active au contexte ;
4. demander à la mémoire de travail de libérer les ressources ;
5. rendre l'identificateur réutilisable conformément aux règles du langage.

### INVARIANTS

Un tableau valide satisfait simultanément les invariants suivants :

- sa dimension est égale à 1 ou 2 ;
- toutes ses limites sont valides ;
- chaque élément accessible possède un ensemble d'indices conforme à sa dimension ;
- tout indice accepté appartient au domaine déclaré ;
- tout accès à un tableau suppose que celui-ci existe dans le contexte applicable ;
- un tableau libéré ne peut plus être résolu comme objet actif ;
- un tableau appartient à un contexte d'exécution déterminé ;
- la libération d'un tableau ne libère pas les autres tableaux.

### CONSÉQUENCES

Le gestionnaire des tableaux dépend du gestionnaire des variables pour la résolution des identificateurs et de la mémoire de travail pour la création et la libération des objets.

L'interpréteur ne manipule pas directement l'organisation interne des tableaux. Il invoque les interfaces du gestionnaire.

La mémoire de travail ne décide pas de la signification d'un tableau. Elle reçoit une demande portant sur un objet abstrait.

La décompilation n'a pas accès directement aux objets tableaux d'exécution lorsqu'elle décompile le programme courant. Elle restitue les déclarations et accès à partir de la représentation compilée.

### TRAÇABILITÉ

HIS-10.6 → EXI-0040, EXI-0041 → VM-1201, VM-1202, VM-1601.

HIS-10.6 → EXI-0050 → VM-1202, VM-1203, VM-1602.

EXI-0038, EXI-0039 → VM-0802, VM-1101, VM-1102.

---

## 15.2 Résolution et accès indexé

### OBJET

Définir le composant logique chargé de transformer une référence compilée à un tableau et ses indices en un accès effectif à un élément de l'objet d'exécution.

### RESPONSABILITÉS

Le composant :

- reçoit une référence compilée ;
- obtient l'identité logique du tableau ;
- demande au gestionnaire des tableaux la résolution de l'objet ;
- évalue les expressions d'indices ;
- vérifie la cohérence entre nombre d'indices et dimension ;
- vérifie les bornes ;
- fournit l'accès abstrait à l'élément.

Il ne crée ni ne détruit l'objet tableau.

### EXIGENCES SATISFAITES

EXI-0032 — Résolution des références vers les objets d'exécution.

EXI-0040 — Tableaux à une ou deux dimensions.

EXI-0041 — Allocation à la déclaration du tableau.

### CONTRAINTES

L'indice d'un tableau est une expression. Son évaluation doit précéder la vérification des limites.

Une référence compilée à un tableau ne constitue jamais directement une adresse mémoire.

La résolution doit être effectuée dans le contexte d'exécution courant, ce qui permet notamment de distinguer un tableau global d'un objet homonyme appartenant à un cadre d'activation.

### INTERFACES

**RésoudreRéférenceTableau**

Entrées :

- référence compilée ;
- contexte actif.

Résultat :

- objet tableau.

**ÉvaluerIndices**

Entrées :

- expressions d'indices compilées ;
- état d'exécution.

Résultat :

- une ou deux valeurs numériques.

**VérifierIndices**

Entrées :

- objet tableau ;
- valeurs d'indices.

Résultat :

- succès ;
- ou diagnostic d'erreur d'exécution.

**AccéderÉlément**

Entrées :

- objet tableau ;
- indices validés.

Résultat :

- valeur lue ou élément modifiable.

### DONNÉES MANIPULÉES

- référence compilée ;
- identificateur logique ;
- contexte actif ;
- indices ;
- limites du tableau ;
- position logique de l'élément.

### ALGORITHMES UTILISÉS

Le traitement est séquentiel :

1. résolution ;
2. évaluation ;
3. validation ;
4. accès.

L'architecture n'impose aucun calcul physique d'adresse. La position logique d'un élément peut être transmise à la mémoire de travail sous une forme abstraite.

### INVARIANTS

Aucun élément n'est accessible si le tableau n'est pas déclaré.

Aucun indice hors limites n'est accepté.

Un tableau à une dimension ne reçoit qu'un indice.

Un tableau à deux dimensions reçoit deux indices.

### CONSÉQUENCES

Le traitement des tableaux reste indépendant de leur allocation concrète.

Toute erreur d'accès relève de l'exécution et non de la compilation lorsque la validité dépend de valeurs qui ne sont connues qu'à l'exécution.

### TRAÇABILITÉ

HIS-10.1, HIS-10.6 → EXI-0032, EXI-0040, EXI-0041 → VM-0802, VM-1001, VM-1201.

---

## 15.3 Cycle de vie des tableaux

### OBJET

Définir le cycle de vie logique d'un objet tableau depuis sa création jusqu'à sa libération.

### RESPONSABILITÉS

Le composant :

- crée le tableau lors de l'exécution de la déclaration ;
- maintient son état d'existence ;
- permet sa réutilisation selon les règles du langage ;
- libère les ressources à la demande ;
- garantit que les références vers un objet libéré ne restent pas actives comme références valides.

### EXIGENCES SATISFAITES

EXI-0041.

EXI-0050.

EXI-0039.

### CONTRAINTES

Une déclaration de tableau exécutée crée un objet d'exécution.

Une nouvelle déclaration du même identificateur ne doit pas produire deux tableaux actifs portant simultanément la même identité dans un même contexte.

La libération explicite doit rendre les ressources réutilisables.

La libération ne doit pas modifier le programme compilé.

### INTERFACES

**Créer**

**Remplacer**

**Libérer**

**TesterExistence**

**RéutiliserIdentité**

Ces opérations sont invoquées par l'interpréteur et par le gestionnaire des variables.

### DONNÉES MANIPULÉES

- identité ;
- état d'existence ;
- contexte ;
- référence vers la mémoire de travail ;
- bornes ;
- valeurs.

### ALGORITHMES UTILISÉS

Le cycle de vie est défini par quatre états :

- absent ;
- actif ;
- libération en cours ;
- absent après libération.

Une transition directe d'absent vers libéré est interdite.

La création d'un objet déjà actif implique l'application des règles de remplacement définies par le Tome II.

### INVARIANTS

Un identificateur ne désigne pas simultanément deux tableaux actifs dans le même contexte.

Un objet libéré n'est plus accessible comme objet actif.

La mémoire de travail ne conserve pas comme ressource active un objet dont l'environnement a terminé la libération.

### CONSÉQUENCES

La durée de vie d'un tableau est indépendante de la durée de vie du programme compilé.

Elle dépend de l'exécution du programme et du contexte qui possède l'objet.

### TRAÇABILITÉ

HIS-10.6 → EXI-0041, EXI-0050 → VM-1202, VM-1203, VM-1602.

---

# 16. Les chaînes

Les chaînes constituent une catégorie d'objets d'exécution distincte des valeurs numériques et des tableaux. La documentation historique distingue explicitement les variables chaîne et les fonctions destinées à leur traitement. Elle documente notamment la longueur d'une chaîne, l'extraction de sous-chaînes, la recherche de positions, les opérations de parcours, la conversion numérique et la conversion en caractères. [INRDP75 §6] [LSE15 §4]

Le Tome III définit une organisation logique commune pour ces opérations. La représentation matérielle des caractères et la méthode de stockage ne sont pas fixées.

## 16.1 Objet chaîne

### OBJET

Définir l'objet d'exécution représentant une valeur chaîne.

### RESPONSABILITÉS

L'objet chaîne :

- possède une séquence ordonnée de caractères ;
- possède une longueur logique ;
- peut être consulté par position ;
- peut être utilisé comme opérande d'une expression chaîne ;
- peut être produit par une constante, une affectation ou une fonction à résultat chaîne ;
- peut être remplacé par une nouvelle valeur ;
- peut être libéré lorsqu'il constitue un objet d'exécution libérable.

Le gestionnaire des chaînes ne définit pas la sémantique des fonctions particulières. Il fournit les primitives communes nécessaires à leur exécution.

### EXIGENCES SATISFAITES

EXI-0038 — Identité stable des variables.

EXI-0039 — Valeur mutable des variables.

EXI-0014 — Représentation compilée des données utilisées par l'exécution.

EXI-0048 — Production d'une valeur par l'évaluation.

### CONTRAINTES

Une chaîne est une valeur composée de caractères ordonnés.

La longueur d'une chaîne est déterminée par son nombre logique de caractères.

Une chaîne vide est une valeur valide.

Les positions utilisées par les fonctions chaînes suivent la convention du langage. Les sources de référence documentent des positions commençant à 1. [INRDP75 §6-1]

La valeur chaîne ne doit pas être confondue avec sa représentation textuelle dans une commande ou avec la représentation source d'un littéral chaîne.

### INTERFACES

**CréerChaîne**

Entrée :

- séquence de caractères.

Résultat :

- objet chaîne.

**LireCaractère**

Entrées :

- chaîne ;
- position.

Résultat :

- caractère ;
- ou erreur de position.

**Longueur**

Entrée :

- chaîne.

Résultat :

- valeur numérique.

**Comparer**

Entrées :

- deux chaînes.

Résultat :

- relation définie par la sémantique du langage.

**Concaténer**

Entrées :

- deux chaînes ou plus.

Résultat :

- nouvelle valeur chaîne.

**RemplacerValeur**

Entrées :

- objet chaîne ;
- nouvelle valeur.

Résultat :

- objet modifié.

### DONNÉES MANIPULÉES

Une chaîne logique contient :

- longueur ;
- caractères ;
- état de validité.

L'architecture n'impose pas :

- un encodage ;
- une taille fixe de caractère ;
- une terminaison spéciale ;
- une adresse mémoire.

### ALGORITHMES UTILISÉS

Les opérations élémentaires sont définies sur une séquence logique.

La concaténation produit une nouvelle séquence correspondant à la juxtaposition ordonnée des séquences opérandes.

La longueur est obtenue par comptage logique des caractères.

L'accès par position vérifie :

1. que la position est entière ;
2. qu'elle appartient au domaine valide ;
3. puis extrait le caractère correspondant.

### INVARIANTS

La longueur d'une chaîne est toujours égale au nombre de caractères qu'elle contient.

Une chaîne valide possède une séquence de caractères cohérente avec sa longueur.

Une opération qui produit une chaîne produit une valeur pouvant être utilisée comme expression chaîne.

### CONSÉQUENCES

Toutes les fonctions de traitement des chaînes utilisent le même objet logique.

Le compilateur identifie les opérations spécialisées ; l'interpréteur les exécute en utilisant l'objet chaîne.

Le décompilateur produit une représentation textuelle indépendante de la représentation interne de la chaîne.

### TRAÇABILITÉ

HIS-10.3 → EXI-0014, EXI-0038, EXI-0039 → VM-0405, VM-1101, VM-1102.

HIS-15.1 → EXI-0048 → VM-1001, VM-1502.

---

## 16.2 Gestionnaire des opérations chaînes

### OBJET

Définir le composant chargé d'exécuter les fonctions et opérations spécialisées portant sur les chaînes.

### RESPONSABILITÉS

Le composant fournit les services logiques correspondant aux familles d'opérations attestées par les sources :

- longueur ;
- sous-chaîne ;
- recherche de position ;
- saut ;
- pointeur ;
- groupe de lettres ;
- conversion numérique ;
- conversion en caractères ;
- équivalence numérique ;
- équivalence caractère ;
- date lorsque celle-ci est utilisée comme valeur chaîne ou opération de conversion.

Il assure également la vérification des paramètres et le traitement des erreurs d'exécution.

### EXIGENCES SATISFAITES

EXI-0032.

EXI-0039.

EXI-0048.

EXI-0014.

### CONTRAINTES

Les fonctions doivent recevoir des valeurs du type attendu.

Une fonction à résultat numérique doit produire une valeur numérique.

Une fonction à résultat chaîne doit produire une valeur chaîne.

La fonction ne doit pas modifier implicitement la valeur d'une chaîne opérande lorsqu'elle est définie comme fonction à résultat.

Les fonctions dont les paramètres comprennent des positions doivent respecter la convention de positions du langage.

### INTERFACES

**LGR**

Entrée :

- expression chaîne.

Sortie :

- longueur numérique.

**SCH**

Entrées :

- chaîne origine ;
- position de départ ;
- critère de fin ;
- paramètres optionnels.

Sortie :

- sous-chaîne.

**POS**

Entrées :

- chaîne recherchée ou critère ;
- position de départ ;
- chaîne origine.

Sortie :

- position numérique selon la sémantique de la fonction.

**SKP**

Entrées :

- chaîne origine ;
- position ;
- critère.

Sortie :

- position numérique.

**PTR**

Entrées :

- chaîne origine ;
- position ;
- critère éventuel.

Sortie :

- position numérique.

**GRL**

Entrées :

- chaîne origine ;
- position ;
- paramètres définissant le groupe.

Sortie :

- chaîne ou position selon la forme utilisée.

**CCA**

Entrée :

- valeur numérique.

Sortie :

- chaîne.

**CNB**

Entrées :

- chaîne contenant une représentation numérique ;
- position ou paramètres de sélection.

Sortie :

- valeur numérique.

**EQC**

Entrée :

- caractère ou chaîne selon la forme documentaire.

Sortie :

- valeur correspondante.

**EQN**

Entrée :

- chaîne ou caractère représentant une valeur numérique.

Sortie :

- valeur numérique.

### DONNÉES MANIPULÉES

Le composant manipule :

- objets chaînes ;
- positions ;
- longueurs ;
- caractères ;
- valeurs numériques ;
- critères d'arrêt ;
- ensembles logiques de caractères ;
- résultats temporaires.

### ALGORITHMES UTILISÉS

Chaque opération est définie comme une transformation abstraite :

**Longueur**

Compter les caractères de la séquence.

**Sous-chaîne**

Évaluer la position de départ, déterminer la longueur ou le critère d'arrêt, extraire la séquence correspondante.

**Recherche**

Parcourir la chaîne selon la position de départ et déterminer la première occurrence répondant au critère.

**Saut**

Parcourir les caractères jusqu'à rencontrer celui qui ne satisfait plus le critère.

**Pointeur**

Dériver la position du premier caractère qui interrompt le parcours défini.

**Conversion numérique**

Extraire la séquence concernée et la soumettre au service numérique.

**Conversion caractère**

Transformer une valeur numérique conformément à la représentation textuelle définie par le langage.

Ces opérations sont décrites au niveau logique. Aucune méthode de recherche ou de conversion particulière n'est imposée.

### INVARIANTS

Chaque opération respecte le type attendu de ses arguments.

Les positions retournées sont cohérentes avec la convention de positionnement du langage.

Une opération produisant une chaîne produit une chaîne valide.

Une erreur de paramètre ne modifie pas les objets opérandes.

### CONSÉQUENCES

Le gestionnaire des chaînes dépend du service numérique pour CNB, EQN et CCA.

Il dépend de l'évaluateur pour l'évaluation préalable des expressions.

Il fournit à l'interpréteur des fonctions exécutables.

Le décompilateur conserve le nom logique de l'opération lorsqu'il restitue le programme.

### TRAÇABILITÉ

HIS-10.3 → EXI-0014, EXI-0032, EXI-0039 → VM-0405, VM-1001, VM-1102.

HIS-15.1 → EXI-0048 → VM-1502.

---

## 16.3 Déclaration et durée de vie des variables chaîne

### OBJET

Définir la gestion architecturale des variables déclarées comme chaînes.

### RESPONSABILITÉS

Le composant :

- crée l'identité logique de la variable chaîne ;
- associe cette identité au contexte applicable ;
- conserve sa valeur courante ;
- permet son remplacement ;
- répond aux demandes de résolution ;
- participe à sa libération.

### EXIGENCES SATISFAITES

EXI-0038.

EXI-0039.

EXI-0032.

EXI-0050.

### CONTRAINTES

Une variable chaîne appartient à un contexte.

Une variable chaîne locale ne doit pas être confondue avec une variable globale de même nom lorsque la portée les distingue.

La valeur courante peut changer sans changer l'identité logique de la variable.

La libération d'une variable entraîne la disparition de sa valeur courante.

### INTERFACES

**DéclarerChaîne**

**RésoudreChaîne**

**LireValeurChaîne**

**ÉcrireValeurChaîne**

**LibérerChaîne**

### DONNÉES MANIPULÉES

- identité logique ;
- contexte ;
- valeur chaîne ;
- état déclaré/libéré.

### ALGORITHMES UTILISÉS

La résolution suit la chaîne des contextes d'activation à partir du contexte courant conformément aux règles de portée établies par le Tome II.

La mise à jour remplace la valeur sans modifier l'identité.

La libération supprime l'association active et rend les ressources disponibles.

### INVARIANTS

Une variable chaîne active possède une identité unique dans son contexte.

Une variable libérée ne peut plus être lue ou modifiée comme variable active.

Une mise à jour réussie modifie la valeur courante de la variable sans créer une nouvelle identité.

### CONSÉQUENCES

Le gestionnaire des chaînes et le gestionnaire des variables utilisent une représentation commune des identités logiques.

La mémoire de travail reste responsable des ressources.

### TRAÇABILITÉ

HIS-10.1, HIS-10.3 → EXI-0038, EXI-0039, EXI-0050 → VM-0802, VM-1101, VM-1102, VM-1602.

---

# 17. Les nombres

Les nombres sont des valeurs d'exécution utilisées par les expressions arithmétiques, les indices, les conditions, les fonctions numériques et les opérations de conversion.

Le Tome III ne définit pas une représentation matérielle du nombre. Il définit un objet numérique abstrait et les services qui permettent son évaluation, son calcul, sa comparaison et sa conversion.

Les sources attestent notamment des opérations d'addition, de soustraction, de multiplication, de division et d'exponentiation ainsi que de fonctions numériques telles que SIN, COS, ATG, LGN, EXP, RAC, ABS, ENT, ALE et TEM. [INRDP75 §3-2] [LSE15 §3-2]

## 17.1 Valeur numérique

### OBJET

Définir la valeur numérique abstraite manipulée par la machine virtuelle.

### RESPONSABILITÉS

Le composant numérique :

- représente une valeur numérique ;
- fournit les opérations arithmétiques ;
- fournit les comparaisons ;
- fournit les conversions nécessaires ;
- fournit les fonctions numériques du langage ;
- restitue une erreur lorsque le calcul demandé n'est pas défini dans les conditions de l'exécution.

### EXIGENCES SATISFAITES

EXI-0048.

EXI-0014.

EXI-0037.

EXI-0022.

### CONTRAINTES

La valeur numérique doit être indépendante du texte qui l'a produite.

Une constante numérique compilée doit pouvoir être évaluée sans analyser à nouveau son écriture textuelle.

Une valeur numérique produite par une expression peut être utilisée comme opérande d'une autre expression.

Le modèle doit permettre les opérations numériques documentées.

La représentation exacte des nombres n'est pas fixée.

### INTERFACES

**CréerValeurNumérique**

**Additionner**

**Soustraire**

**Multiplier**

**Diviser**

**ÉleverÀLaPuissance**

**Comparer**

**AppliquerFonctionNumérique**

**ConvertirEnChaîne**

**ConvertirDepuisChaîne**

### DONNÉES MANIPULÉES

- valeurs numériques abstraites ;
- opérandes ;
- résultat ;
- paramètres de fonction ;
- état éventuel nécessaire aux fonctions déterministes ou pseudo-aléatoires.

### ALGORITHMES UTILISÉS

L'évaluation des opérations arithmétiques suit le principe :

1. évaluer les opérandes ;
2. vérifier leur validité ;
3. appliquer l'opération ;
4. produire une valeur numérique.

Pour les opérations binaires, deux valeurs sont extraites de la pile d'évaluation.

Pour une fonction unaire :

1. évaluer l'argument ;
2. appliquer la fonction ;
3. empiler le résultat.

Pour une fonction sans argument telle que TEM :

1. demander au service correspondant la valeur courante ;
2. produire la valeur numérique.

### INVARIANTS

Toute opération numérique consomme des valeurs numériques conformes à sa signature.

Le résultat d'une opération numérique est une valeur numérique.

La représentation compilée n'est pas modifiée par l'évaluation.

Une erreur numérique ne transforme pas silencieusement une valeur invalide en valeur valide.

### CONSÉQUENCES

Le moteur numérique est utilisé par :

- l'interpréteur ;
- la pile d'évaluation ;
- le gestionnaire des indices ;
- le gestionnaire des chaînes pour les conversions ;
- le formateur des sorties.

Le moteur numérique ne connaît pas la syntaxe du programme.

### TRAÇABILITÉ

HIS-8.1, HIS-15.1 → EXI-0048 → VM-1001, VM-1502, VM-0702.

HIS-10.1 → EXI-0014, EXI-0032 → VM-0405, VM-1001.

---

## 17.2 Opérations arithmétiques et fonctions numériques

### OBJET

Définir le composant responsable de la mise à disposition des opérations numériques exécutables.

### RESPONSABILITÉS

Le composant prend en charge :

- addition ;
- soustraction ;
- multiplication ;
- division ;
- exponentiation ;
- fonctions numériques élémentaires ;
- production de résultats numériques.

Il fournit également le mécanisme logique par lequel l'interpréteur différencie une opération arithmétique d'une référence à une valeur.

### EXIGENCES SATISFAITES

EXI-0045.

EXI-0047.

EXI-0048.

EXI-0013.

### CONTRAINTES

Les opérations sont exécutées à partir de la représentation compilée.

Les expressions sont évaluées dans l'ordre post-fixé reconstruit.

La précédence syntaxique a déjà été résolue par le compilateur.

L'interpréteur ne doit pas réanalyser l'expression source pour déterminer la priorité des opérations.

### INTERFACES

**ExécuterOpérationArithmétique**

Entrées :

- opération codée ;
- pile d'évaluation.

Sortie :

- pile d'évaluation modifiée ;
- ou erreur.

**ExécuterFonctionNumérique**

Entrées :

- identifiant de fonction ;
- arguments évalués.

Sortie :

- résultat numérique.

### DONNÉES MANIPULÉES

- opérations codées ;
- arguments ;
- valeurs numériques ;
- résultats ;
- erreurs.

### ALGORITHMES UTILISÉS

L'interpréteur lit séquentiellement la représentation compilée.

Lorsqu'il rencontre une valeur numérique, il l'empile.

Lorsqu'il rencontre une opération arithmétique :

1. il détermine le nombre d'opérandes requis ;
2. il retire ces valeurs de la pile ;
3. il les transmet au moteur numérique ;
4. il empile le résultat.

Lorsqu'il rencontre une fonction :

1. les arguments sont déjà évalués ;
2. la fonction est identifiée ;
3. elle est appelée avec les valeurs disponibles ;
4. son résultat est empilé.

### INVARIANTS

Une opération ne s'exécute que si toutes ses opérandes sont disponibles.

Une expression correctement compilée laisse à son terme une seule valeur de résultat sur la pile d'évaluation.

Une fonction numérique à résultat numérique ne dépose jamais une valeur chaîne dans la pile.

### CONSÉQUENCES

Le moteur numérique peut être remplacé ou réorganisé sans modifier le compilateur, tant que ses interfaces logiques sont respectées.

Les fonctions arithmétiques et les fonctions de traitement des chaînes restent distinctes, même si certaines fonctions assurent des conversions entre ces domaines.

### TRAÇABILITÉ

CPF-7.2 → EXI-0037, EXI-0047 → VM-0403, VM-1001, VM-1501.

HIS-15.1 → EXI-0048 → VM-1502.

---

## 17.3 Conversion entre nombres et chaînes

### OBJET

Définir le service de conversion entre les valeurs numériques et leurs représentations sous forme de chaînes.

### RESPONSABILITÉS

Le service :

- convertit une valeur numérique en chaîne ;
- convertit une chaîne représentant un nombre en valeur numérique ;
- respecte les règles du langage ;
- signale les conversions invalides ;
- distingue la valeur numérique obtenue de sa représentation textuelle.

### EXIGENCES SATISFAITES

EXI-0048.

EXI-0014.

EXI-0039.

### CONTRAINTES

Une conversion ne doit pas confondre la chaîne source avec la valeur numérique qu'elle représente.

Le résultat d'une conversion numérique vers chaîne est une valeur chaîne.

Le résultat inverse est une valeur numérique.

La conversion doit être utilisable pendant l'exécution d'une expression.

### INTERFACES

**NumériqueVersChaîne**

**ChaîneVersNumérique**

**VérifierConversion**

### DONNÉES MANIPULÉES

- valeur numérique ;
- chaîne ;
- position de conversion ;
- résultat ;
- diagnostic.

### ALGORITHMES UTILISÉS

La conversion numérique vers chaîne produit une représentation textuelle déterminée par les règles du langage.

La conversion chaîne vers numérique :

1. identifie la partie pertinente de la chaîne ;
2. vérifie sa conformité ;
3. interprète sa valeur ;
4. produit l'objet numérique.

### INVARIANTS

Une conversion réussie respecte le type de résultat annoncé.

Une conversion échouée ne produit pas de valeur numérique arbitraire.

La conversion ne modifie pas l'opérande d'origine.

### CONSÉQUENCES

Le service est partagé par :

- le gestionnaire des chaînes ;
- le moteur numérique ;
- le système d'entrée-sortie.

### TRAÇABILITÉ

HIS-10.3 → EXI-0014, EXI-0039 → VM-0405, VM-1102.

HIS-15.1 → EXI-0048 → VM-1001, VM-1502.

---

# 18. Les entrées-sorties

Les entrées-sorties assurent le passage des informations entre l'environnement d'exécution et les sources ou destinations externes au programme.

Dans la machine virtuelle, le composant d'entrée-sortie ne connaît ni le dispositif physique utilisé, ni le mécanisme matériel par lequel les caractères sont transmis. Il fournit des interfaces abstraites vers :

- une entrée utilisateur ;
- une sortie utilisateur ;
- une source de données ;
- une destination de données.

Les instructions LIRE et AFFICHER appartiennent à cette architecture. Les formats de présentation sont traités par un composant spécialisé.

## 18.1 Gestionnaire des entrées

### OBJET

Définir le composant chargé de fournir à l'exécution les valeurs saisies par l'utilisateur ou provenant d'une source d'entrée autorisée.

### RESPONSABILITÉS

Le gestionnaire :

- reçoit une demande de lecture ;
- détermine le type attendu ;
- demande la saisie ou la lecture de données ;
- interprète les caractères reçus selon le format applicable ;
- produit une valeur numérique, une valeur chaîne ou une valeur structurée ;
- affecte la valeur obtenue aux objets désignés ;
- signale les erreurs de conversion ou d'entrée.

### EXIGENCES SATISFAITES

EXI-0052.

EXI-0039.

EXI-0048.

EXI-0032.

### CONTRAINTES

Le gestionnaire d'entrée est invoqué par l'interpréteur à la suite d'une opération codée.

Une demande de lecture doit pouvoir être associée à plusieurs références.

Les références sont résolues dans le contexte d'exécution courant.

Une donnée entrée ne modifie l'état d'un objet qu'après validation de sa valeur.

### INTERFACES

**LireValeur**

Entrées :

- type attendu ;
- spécification de lecture éventuelle.

Sortie :

- valeur.

**LireDansVariable**

Entrées :

- destination ;
- type attendu.

Résultat :

- variable modifiée ;
- ou erreur.

**LireSéquence**

Entrées :

- liste de destinations ;
- spécifications de lecture.

Résultat :

- mise à jour atomique de la séquence ou diagnostic.

### DONNÉES MANIPULÉES

- caractères d'entrée ;
- valeurs numériques ;
- valeurs chaînes ;
- références ;
- spécifications de lecture ;
- diagnostics.

### ALGORITHMES UTILISÉS

Le traitement d'une instruction LIRE suit :

1. évaluer ou reconnaître les spécifications de lecture ;
2. résoudre chaque destination ;
3. obtenir les données ;
4. convertir chaque donnée ;
5. vérifier la validité ;
6. effectuer les affectations.

Lorsqu'une entrée comporte plusieurs destinations, l'architecture conserve l'ordre logique des destinations.

### INVARIANTS

Une affectation issue d'une lecture n'est effectuée que sur un objet résolu et valide.

Une conversion invalide produit une erreur d'exécution.

Une erreur ne doit pas produire une valeur silencieuse de remplacement.

Le gestionnaire ne modifie pas directement le programme compilé.

### CONSÉQUENCES

Le gestionnaire d'entrée dépend :

- de l'environnement conversationnel pour l'origine des caractères ;
- du moteur numérique pour les conversions ;
- du gestionnaire des chaînes pour les valeurs chaîne ;
- du gestionnaire des variables pour les affectations.

### TRAÇABILITÉ

HIS-3.1 → EXI-0052 → VM-2001, VM-2002.

HIS-10.1, HIS-10.3 → EXI-0032, EXI-0039 → VM-0802, VM-1101, VM-1102.

---

## 18.2 Gestionnaire des sorties

### OBJET

Définir le composant chargé de transformer les valeurs produites par l'exécution en une séquence de sortie conforme aux spécifications du langage.

### RESPONSABILITÉS

Le gestionnaire :

- reçoit une liste d'expressions à afficher ;
- évalue ces expressions ;
- détermine le type de chaque résultat ;
- transmet les valeurs au formateur ;
- assemble les fragments de sortie ;
- transmet la sortie au destinataire logique.

### EXIGENCES SATISFAITES

EXI-0048.

EXI-0052.

EXI-0033.

### CONTRAINTES

L'expression à afficher est évaluée avant sa mise en forme.

Le format d'affichage ne modifie pas la valeur d'origine.

Les instructions d'affichage doivent pouvoir produire des chaînes, des valeurs numériques et des séquences combinées.

Le résultat est envoyé à une destination abstraite.

### INTERFACES

**AfficherExpression**

Entrées :

- expressions compilées ;
- format éventuel.

Sortie :

- séquence de sortie.

**AfficherValeur**

Entrée :

- valeur ;
- spécification de format éventuelle.

Sortie :

- texte formaté.

**ÉmettreSortie**

Entrée :

- séquence de sortie.

Résultat :

- émission vers la destination.

### DONNÉES MANIPULÉES

- expressions compilées ;
- valeurs ;
- formats ;
- caractères de sortie ;
- positions de mise en page.

### ALGORITHMES UTILISÉS

1. évaluer l'expression ;
2. déterminer son type ;
3. sélectionner la représentation par défaut ou la spécification fournie ;
4. produire le texte correspondant ;
5. appliquer les règles de positionnement ;
6. transmettre la séquence.

### INVARIANTS

Une valeur affichée correspond à la valeur évaluée à l'instant de l'instruction.

Le formatage n'altère jamais l'objet source.

Chaque opération de sortie produit une séquence ordonnée de caractères.

### CONSÉQUENCES

Le gestionnaire de sortie dépend du formateur et du dialogue avec le moniteur.

Il peut être utilisé lors d'une exécution normale, d'une exécution immédiate ou d'une observation de l'état d'exécution.

### TRAÇABILITÉ

HIS-8.1 → EXI-0048 → VM-0702, VM-1502.

HIS-3.13 → EXI-0033 → VM-0803, VM-2001.

---

## 18.3 Formateur des sorties

### OBJET

Définir le composant responsable de l'interprétation des spécifications de format associées à une opération d'affichage.

### RESPONSABILITÉS

Le formateur :

- reconnaît les spécifications de format ;
- associe chaque spécification à une valeur ;
- applique les conversions appropriées ;
- réalise les commandes de mise en page ;
- gère les répétitions ;
- produit une séquence de caractères conforme.

Les spécifications documentées comprennent notamment les formes universelles, les commandes de mise en page et les formats numériques. [INRDP75 §4-5] [INRDP75 §5]

### EXIGENCES SATISFAITES

EXI-0048.

EXI-0052.

### CONTRAINTES

La spécification de format est distincte de la valeur affichée.

Le formateur ne modifie pas la valeur.

Les spécifications sont appliquées dans l'ordre dans lequel elles apparaissent.

Une répétition doit produire le nombre de sorties prévu par la spécification.

Une spécification incompatible avec la valeur reçue doit produire une erreur ou un diagnostic conformément aux règles de l'exécution.

### INTERFACES

**AnalyserFormat**

Entrée :

- description de format compilée ou structurée.

Sortie :

- séquence de spécifications abstraites.

**FormaterValeur**

Entrées :

- valeur ;
- spécification.

Sortie :

- caractères.

**AppliquerMiseEnPage**

Entrées :

- spécification de mise en page ;
- état courant de la sortie.

Sortie :

- nouvel état de positionnement.

### DONNÉES MANIPULÉES

- spécifications de format ;
- valeurs ;
- nombre de répétitions ;
- position de sortie ;
- caractères.

### ALGORITHMES UTILISÉS

Le formateur traite séquentiellement les spécifications.

Pour chaque spécification :

1. identifier sa catégorie ;
2. déterminer la valeur correspondante ;
3. effectuer la conversion appropriée ;
4. produire le fragment de caractères ;
5. mettre à jour l'état de sortie.

Les spécifications de répétition réutilisent une sous-séquence logique selon le nombre demandé.

### INVARIANTS

Le formateur conserve l'ordre des éléments.

Une valeur ne peut être associée à une spécification incompatible sans produire de diagnostic.

Le formateur ne modifie pas l'état d'exécution des variables.

### CONSÉQUENCES

Le formateur est utilisé aussi bien pour l'affichage interactif que pour les opérations d'entrée-sortie dont la destination logique n'est pas la console.

Il constitue un composant distinct du gestionnaire de sorties.

### TRAÇABILITÉ

HIS-8.1 → EXI-0048 → VM-1502.

HIS-3.1 → EXI-0052 → VM-2001.

---

## 18.4 Interface avec les tableaux

### OBJET

Définir l'organisation logique des entrées-sorties portant sur des tableaux ou des ensembles de valeurs indexées.

### RESPONSABILITÉS

Le composant :

- résout le tableau ;
- détermine la plage logique concernée ;
- parcourt les éléments selon l'ordre demandé ;
- transmet chaque valeur au formateur ou au gestionnaire d'entrée ;
- maintient l'association entre indices et valeurs.

### EXIGENCES SATISFAITES

EXI-0040.

EXI-0041.

EXI-0048.

### CONTRAINTES

Le tableau doit être déclaré et actif avant son utilisation.

Les dimensions du tableau déterminent le parcours.

Les indices sont produits selon les bornes déclarées.

Aucune lecture ou écriture ne doit dépasser les limites valides.

### INTERFACES

**LireTableau**

**AfficherTableau**

**ParcourirTableau**

Ces interfaces utilisent les services génériques du gestionnaire de tableaux.

### DONNÉES MANIPULÉES

- objet tableau ;
- dimension ;
- bornes ;
- indices ;
- valeurs ;
- formats.

### ALGORITHMES UTILISÉS

Le parcours suit l'ordre logique des indices.

Pour deux dimensions, le composant utilise deux niveaux de parcours.

Le choix de l'ordre exact est celui défini par la sémantique du langage et les formats associés ; le composant ne le remplace pas par une organisation mémoire.

### INVARIANTS

Chaque élément traité possède des indices valides.

Le parcours est déterministe pour un tableau donné et un ensemble de bornes donné.

### CONSÉQUENCES

Les entrées-sorties sont découplées du stockage physique du tableau.

### TRAÇABILITÉ

HIS-10.6 → EXI-0040, EXI-0041 → VM-1201, VM-1202.

HIS-8.1 → EXI-0048 → VM-1502.

---

# 19. Le dialogue avec le moniteur

Le dialogue avec le moniteur constitue l'interface de contrôle transversal de la machine virtuelle. Il permet d'introduire des lignes, d'exécuter un programme, d'observer l'état d'une exécution, d'interrompre celle-ci et de reprendre le contrôle.

Les sources distinguent explicitement l'état d'exécution et l'état moniteur. Le manuel LSE 15 indique qu'une interruption peut faire repasser la console à l'état moniteur et afficher le message PRET, avec indication de la ligne interrompue lorsque l'exécution était active. Le manuel de 1975 documente également les fonctions de mise au point, notamment l'arrêt, le pas à pas, la continuation et l'interrogation. [LSE15 §9-3] [LSE15 §9-4] [INRDP75 §4.1] [INRDP75 Annexe II]

## 19.1 Moniteur de dialogue

### OBJET

Le moniteur de dialogue est le composant architectural qui maintient l'état de contrôle de la session et décide quelle opération doit être exécutée à la suite d'une entrée ou d'un événement.

### RESPONSABILITÉS

Le moniteur :

- reçoit les entrées utilisateur ;
- reçoit les événements d'interruption ;
- maintient l'état courant de la session ;
- distingue le dialogue normal de l'exécution ;
- transfère une demande au processeur compétent ;
- récupère les diagnostics ;
- restitue le contrôle à l'utilisateur ;
- permet la reprise d'une exécution suspendue ;
- coordonne les transitions entre programme courant, exécution et état moniteur.

### EXIGENCES SATISFAITES

EXI-0052 — Traitement conversationnel des demandes.

EXI-0033 — Observation de l'état d'exécution.

EXI-0024 — Conservation d'une position d'exécution interrompue.

EXI-0029 — Exécution explicite du programme courant.

EXI-0030 — Terminaison explicite ou naturelle de l'exécution.

### CONTRAINTES

Le moniteur doit rester disponible comme niveau de dialogue.

L'exécution d'un programme ne doit pas supprimer le programme courant.

Une interruption ne doit pas détruire l'état d'exécution lorsqu'une reprise est prévue.

Une demande utilisateur peut être traitée alors qu'une exécution est suspendue.

Le moniteur ne doit pas exécuter lui-même les opérations du langage. Il les transmet à l'interpréteur ou au processeur approprié.

### INTERFACES

**RecevoirEntrée**

Entrée :

- ligne ou événement.

Sortie :

- demande classifiable.

**ClassifierDemande**

Entrée :

- entrée reçue.

Sortie :

- type de demande.

**ExécuterDemande**

Entrée :

- demande classifiée.

Sortie :

- résultat ;
- diagnostic ;
- changement d'état.

**InterrompreExécution**

Entrée :

- événement d'interruption.

Sortie :

- exécution suspendue ;
- état moniteur actif.

**ReprendreExécution**

Entrée :

- exécution suspendue ;
- mode de reprise.

Sortie :

- exécution active.

**TerminerExécution**

Entrée :

- exécution active.

Sortie :

- état d'exécution détruit ;
- programme courant conservé.

### DONNÉES MANIPULÉES

Le moniteur manipule :

- état de session ;
- demande utilisateur ;
- programme courant ;
- référence à l'exécution active ou suspendue ;
- diagnostics ;
- résultat d'une opération ;
- état de la console.

Les états logiques sont :

- **MONITEUR** ;
- **EXÉCUTION** ;
- **SUSPENDU**.

La transition entre ces états est gérée par le moniteur.

### ALGORITHMES UTILISÉS

Le cycle général est :

1. attendre une entrée ou un événement ;
2. déterminer l'état courant ;
3. classifier la demande ;
4. sélectionner le processeur ;
5. exécuter l'opération ;
6. recueillir le résultat ou le diagnostic ;
7. actualiser l'état ;
8. rendre le contrôle au moniteur ou à l'exécution.

Lorsqu'une interruption survient :

1. interrompre l'avancement de l'interpréteur ;
2. conserver l'état d'exécution ;
3. identifier la position logique courante ;
4. passer à l'état suspendu ;
5. transférer le contrôle au moniteur ;
6. rendre possible l'observation ou la modification autorisée.

### INVARIANTS

Le moniteur ne possède pas lui-même le programme courant.

Une exécution suspendue conserve son état tant qu'elle n'est pas explicitement terminée.

Une demande traitée par le moniteur ne modifie le programme courant que si le processeur appelé le prévoit explicitement.

Une erreur de compilation ne crée pas une nouvelle version valide du programme courant.

La fin d'une exécution ne détruit pas le programme courant.

### CONSÉQUENCES

Le moniteur dépend de tous les processeurs accessibles par le dialogue.

Il constitue donc un composant transversal, mais ne doit pas devenir une couche contenant les responsabilités des autres composants.

Il fournit le point de synchronisation logique entre :

- compilation ;
- exécution ;
- décompilation ;
- bibliothèque ;
- gestion des variables ;
- mise au point.

### TRAÇABILITÉ

HIS-3.1 → EXI-0052 → VM-2001.

HIS-3.13 → EXI-0024, EXI-0033 → VM-0604, VM-0803, VM-2103.

HIS-8.2 → EXI-0029, EXI-0030 → VM-0703, VM-0705, VM-2101.

---

## 19.2 Analyseur des demandes du moniteur

### OBJET

Définir le composant chargé de déterminer le traitement associé à une entrée utilisateur ou à une demande du moniteur.

### RESPONSABILITÉS

L'analyseur :

- reconnaît une ligne de programme ;
- reconnaît une expression immédiate ;
- reconnaît une commande ;
- reconnaît une demande adressée à un processeur ;
- transmet la demande sans en modifier la signification ;
- retourne un diagnostic lorsqu'aucune interprétation valide n'est possible.

### EXIGENCES SATISFAITES

EXI-0052.

EXI-0018.

EXI-0051.

### CONTRAINTES

La classification est préalable au traitement.

Une ligne de programme est remise au compilateur.

Une expression immédiate est remise au mécanisme d'exécution immédiate.

Une commande est remise au processeur de commande ou au bibliothécaire selon sa nature.

Une erreur de classification ne doit pas être confondue avec une erreur d'exécution.

### INTERFACES

**AnalyserEntrée**

Entrée :

- caractères d'une ligne validée.

Sortie :

- catégorie de demande ;
- ou diagnostic.

**OrienterDemande**

Entrées :

- catégorie ;
- contenu de la demande.

Sortie :

- invocation du composant compétent.

**AnalyserCommande**

Entrée :

- demande de commande.

Sortie :

- commande structurée.

### DONNÉES MANIPULÉES

- caractères d'entrée ;
- catégorie ;
- structure de demande ;
- diagnostic.

### ALGORITHMES UTILISÉS

Le traitement suit l'ordre :

1. réception de la ligne ;
2. reconnaissance de sa forme générale ;
3. classification ;
4. transmission.

L'analyseur ne compile pas lui-même les lignes de programme.

Il ne détermine pas les résultats des expressions.

Il n'exécute pas les commandes.

### INVARIANTS

Une entrée ne doit être remise qu'à un seul traitement principal.

La classification doit être déterministe pour une entrée donnée et un état de session donné.

Le contenu transmis au processeur doit préserver l'information nécessaire à son traitement.

### CONSÉQUENCES

Le compilateur reste indépendant de la console.

L'exécution immédiate réutilise le même moteur d'interprétation que l'exécution programmée lorsqu'une représentation compilée compatible est produite.

Les commandes d'accès aux fichiers sont transmises au bibliothécaire.

### TRAÇABILITÉ

HIS-3.1, HIS-3.11 → EXI-0052 → VM-2001, VM-2002, VM-0501, VM-1701.

HIS-5.2 → EXI-0018, EXI-0051 → VM-0502, VM-1901.

---

## 19.3 Gestion des interruptions

### OBJET

Définir le composant chargé de transférer le contrôle de l'état d'exécution vers l'état moniteur sans destruction de l'état conservé.

### RESPONSABILITÉS

Le composant :

- reçoit l'événement d'interruption ;
- arrête l'avancement logique de l'interpréteur ;
- capture l'état nécessaire à la reprise ;
- identifie la position d'exécution ;
- conserve les cadres d'activation ;
- conserve la pile d'évaluation ;
- marque l'exécution comme suspendue ;
- transmet le contrôle au moniteur.

### EXIGENCES SATISFAITES

EXI-0024.

EXI-0031.

EXI-0033.

EXI-0035.

EXI-0037.

### CONTRAINTES

L'interruption ne doit pas être assimilée à une terminaison.

Les objets du programme courant restent inchangés.

L'état d'exécution doit rester cohérent.

Une interruption survenant dans une procédure doit conserver la chaîne d'activations.

Une interruption survenant pendant l'évaluation d'une expression doit préserver l'état nécessaire à une reprise cohérente lorsque cette reprise est autorisée.

### INTERFACES

**Suspendre**

Entrée :

- exécution active.

Sortie :

- exécution suspendue.

**ObtenirÉtatSuspendu**

Entrée :

- exécution suspendue.

Sortie :

- état observable autorisé.

**ModifierÉtatSuspendu**

Entrées :

- modification autorisée ;
- contexte de l'exécution suspendue.

Sortie :

- état mis à jour.

### DONNÉES MANIPULÉES

- position d'exécution ;
- cadres d'activation ;
- pile d'évaluation ;
- variables ;
- tableaux ;
- chaînes ;
- état de sortie nécessaire ;
- état de lecture nécessaire.

### ALGORITHMES UTILISÉS

La suspension est atomique au niveau logique :

1. arrêter l'avancement ;
2. figer l'état ;
3. enregistrer la position ;
4. enregistrer les contextes ;
5. enregistrer les valeurs temporaires ;
6. changer l'état de l'exécution ;
7. rendre le contrôle au moniteur.

La reprise inverse le processus :

1. vérifier que l'exécution est suspendue ;
2. restaurer l'état logique ;
3. réactiver l'exécution ;
4. reprendre à la position conservée ou à la position explicitement demandée par une commande de reprise autorisée.

### INVARIANTS

Une suspension conserve la cohérence entre position d'exécution, cadres d'activation et pile d'évaluation.

Une exécution suspendue ne progresse pas tant qu'elle n'est pas reprise.

Une interruption ne modifie pas le programme courant.

La reprise ne doit pas créer une seconde exécution concurrente du même état suspendu.

### CONSÉQUENCES

Le composant dépend directement de l'environnement d'exécution.

Il dépend du moniteur pour les commandes de reprise.

Il doit pouvoir fonctionner indépendamment du mécanisme physique utilisé pour conserver l'état.

### TRAÇABILITÉ

HIS-3.13 → EXI-0024, EXI-0031, EXI-0033 → VM-0604, VM-0801, VM-0803, VM-2103.

HIS-13.1 → EXI-0035 → VM-0902, VM-2102.

HIS-15.1 → EXI-0037 → VM-1001, VM-1502.

---

## 19.3.1 Gestion du mode PAS À PAS

Le mode PAS À PAS constitue un mode de contrôle de l'exécution. Il ne correspond pas à une interruption arbitraire de l'interpréteur : l'arrêt attendu intervient au début de chaque ligne à exécuter. [INRDP75 §4-1 ; Annexe II]

À chaque étape :

1. l'exécution atteint la frontière de la ligne suivante ;
2. si le mode PAS À PAS est actif, l'avancement est suspendu avant l'exécution de cette ligne ;
3. la console repasse dans l'état moniteur ;
4. le numéro de la ligne atteinte est rendu observable ;
5. l'utilisateur peut effectuer les opérations de dialogue autorisées ;
6. CONTINUER réactive l'exécution ;
7. NORMAL désactive le mode PAS À PAS.

La position de reprise et l'état d'exécution doivent rester cohérents avec les cadres d'activation et la pile d'évaluation. [INRDP75 §4-1 ; §4-3 ; LSE15 §9-3]

**Statut : Déduit.**

**Traçabilité :** HIS-13.1, HIS-13.3 → EXI-0060 → VM-2102, VM-2103.

## 19.4 Gestion de la continuation et de la reprise

### OBJET

Définir les services de reprise d'une exécution suspendue.

### RESPONSABILITÉS

Le composant distingue au minimum :

- la continuation au point d'interruption ;
- la reprise à une position explicitement demandée lorsque le langage ou le moniteur l'autorise ;
- la terminaison volontaire de l'exécution suspendue.

Il restaure l'état de l'exécution et réactive l'interpréteur.

### EXIGENCES SATISFAITES

EXI-0024.

EXI-0030.

EXI-0031.

EXI-0033.

EXI-0046.

### CONTRAINTES

La continuation doit utiliser l'état conservé.

Une reprise à une nouvelle position ne doit pas être confondue avec une nouvelle exécution du programme courant lorsque les mécanismes historiques distinguent ces opérations.

Le programme courant reste inchangé par une simple continuation.

Une modification des variables effectuée pendant une suspension doit être prise en compte par l'exécution ultérieure conformément aux règles de l'environnement d'exécution.

### INTERFACES

**Continuer**

Entrée :

- exécution suspendue.

Résultat :

- exécution active au point conservé.

**ReprendreÀ**

Entrées :

- exécution suspendue ;
- position logique demandée.

Résultat :

- exécution active à la position demandée si celle-ci est valide.

**Abandonner**

Entrée :

- exécution suspendue.

Résultat :

- destruction de l'état d'exécution ;
- programme courant conservé.

### DONNÉES MANIPULÉES

- état suspendu ;
- position conservée ;
- nouvelle position éventuelle ;
- variables ;
- cadres d'activation ;
- pile d'évaluation.

### ALGORITHMES UTILISÉS

**CONTINUER**

1. vérifier l'état suspendu ;
2. vérifier l'intégrité de l'état ;
3. restaurer l'interpréteur ;
4. reprendre au point conservé.

**REPRENDRE**

1. vérifier la position demandée ;
2. conserver les objets d'exécution selon les règles du langage ;
3. modifier la position logique ;
4. remettre l'interpréteur en état actif.

**ABANDONNER**

1. interrompre définitivement l'exécution ;
2. libérer les objets d'exécution ;
3. conserver le programme courant ;
4. revenir à l'état moniteur.

### INVARIANTS

Une continuation ne recompilé pas le programme courant.

Une continuation ne réinitialise pas les variables conservées par l'exécution.

Une reprise à une nouvelle position ne modifie la représentation compilée.

Un abandon détruit l'état d'exécution mais non le programme courant.

### CONSÉQUENCES

La gestion de la reprise exige une séparation stricte entre :

- programme courant ;
- état d'exécution ;
- état du moniteur.

Cette séparation constitue une contrainte fondamentale de l'architecture logique.

### TRAÇABILITÉ

HIS-3.13 → EXI-0024, EXI-0031, EXI-0033 → VM-0604, VM-0801, VM-0803, VM-2103.

HIS-8.2 → EXI-0030, EXI-0046 → VM-0705, VM-0603, VM-0604.

HIS-14.1 → EXI-0046 → VM-1402.

---


## 19.5 Commandes de fichiers-données

### VM-2003
Orienteur des commandes et instructions de fichiers-données

### OBJET

Le composant oriente les commandes CATALOGUER, SUPPRIMER, ENCODER et DECODER ainsi
que les instructions GARER, CHARGER et SUPPRIMER vers les services
compétents.

### RESPONSABILITÉS

Il :

- reconnaît la commande ;
- vérifie sa forme ;
- détermine le fichier concerné ;
- appelle le bibliothécaire ou le service de transformation approprié ;
- transmet le diagnostic éventuel.

### EXIGENCES SATISFAITES

- EXI-0055 ;
- EXI-0057.

### INTERFACES

`TRAITER_COMMANDE_FICHIER(demande)`

### TRAÇABILITÉ

HIS-4.5.5, HIS-11.1, HIS-11.3, HIS-11.5 → EXI-0055, EXI-0057 → VM-2003.


## 19.6 Interface générale du dialogue

### OBJET

Définir l'interface d'interaction entre le moniteur et l'ensemble des composants de la machine virtuelle.

### RESPONSABILITÉS

L'interface :

- reçoit les entrées ;
- fournit les sorties ;
- transmet les commandes ;
- transmet les diagnostics ;
- expose l'état autorisé ;
- commande l'interruption ;
- commande la reprise ;
- permet de revenir à l'état moniteur.

### EXIGENCES SATISFAITES

EXI-0052.

EXI-0033.

EXI-0024.

EXI-0030.

EXI-0049.

### CONTRAINTES

Aucun composant interne ne doit accéder directement au dispositif de dialogue.

Toutes les interactions passent par les interfaces logiques du moniteur.

Le dialogue doit pouvoir fonctionner avant, pendant et après une exécution.

Une exécution suspendue doit pouvoir produire des résultats de diagnostic sans modifier le programme compilé.

Le stockage persistant ne doit pas être confondu avec l'espace de dialogue.

### INTERFACES

**EntréeUtilisateur → Moniteur**

**Moniteur → Compilateur**

**Moniteur → Interpréteur**

**Moniteur → Décompilateur**

**Moniteur → Bibliothécaire**

**Moniteur → Gestionnaire des variables**

**Moniteur → État d'exécution**

**Composant → Moniteur**

pour :

- résultats ;
- diagnostics ;
- demandes de suspension ;
- demandes de reprise.

### DONNÉES MANIPULÉES

- lignes ;
- commandes ;
- expressions ;
- résultats ;
- diagnostics ;
- état moniteur ;
- identifiants de programme courant ;
- références vers les exécutions.

### ALGORITHMES UTILISÉS

Le dialogue suit une architecture en boucle logique :

1. le moniteur attend une entrée ;
2. l'analyseur détermine la nature de la demande ;
3. le moniteur sélectionne le processeur ;
4. le processeur exécute son traitement ;
5. le résultat est transmis au moniteur ;
6. le moniteur produit la sortie ;
7. le contrôle revient au moniteur ou à l'exécution.

Lorsqu'un composant demande une suspension :

1. le moniteur reçoit l'événement ;
2. l'état d'exécution est conservé ;
3. la console devient disponible pour les commandes autorisées ;
4. une commande de continuation ou de reprise réactive l'exécution.

### INVARIANTS

Le moniteur demeure l'unique point logique de dialogue utilisateur.

Les composants internes ne dépendent pas d'un dispositif particulier.

Une sortie ne modifie pas l'état logique du programme.

Un diagnostic reste associé au traitement qui l'a produit.

Un changement d'état du moniteur n'entraîne pas automatiquement une modification du programme courant.

### CONSÉQUENCES

Cette interface est la limite architecturale entre la machine virtuelle et son environnement de dialogue.

Elle permet de conserver l'indépendance du Tome III vis-à-vis :

- d'un processeur particulier ;
- d'un langage d'implémentation ;
- d'un système d'exploitation ;
- d'un dispositif d'entrée-sortie particulier.

### TRAÇABILITÉ

HIS-3.1 → EXI-0052 → VM-2001.

HIS-3.13 → EXI-0024, EXI-0033 → VM-0803, VM-2103.

HIS-3.14 → EXI-0049 → VM-2101.

---

## 19.7 Gestionnaire des tâches en temps partagé

### VM-2105
Gestionnaire des tâches

### OBJET

Le composant conserve l'état logique des tâches utilisateurs et permet
leur activation ou leur suspension.

### RESPONSABILITÉS

Il :

- conserve l'état d'une tâche inactive ;
- désigne la tâche active ;
- suspend une tâche ;
- restaure l'état d'une tâche ;
- coordonne les transitions correspondant à SWAP OUT et SWAP IN.

### EXIGENCES SATISFAITES

- EXI-0058 ;
- EXI-0059.

### DONNÉES MANIPULÉES

- tâches ;
- état d'exécution ;
- état de session ;
- état de suspension.

### CONTRAINTES

Le mécanisme ne fixe pas l'algorithme de sélection des tâches.

La conservation d'une tâche est distincte de la conservation d'un
fichier-programme.

### INTERFACES

`SUSPENDRE_TACHE(tache)`

`REACTIVER_TACHE(tache)`

`SWAP_OUT(tache)`

`SWAP_IN(tache)`

### TRAÇABILITÉ

HIS-4.1, HIS-4.2, HIS-4.3, HIS-4.4.1 → EXI-0058, EXI-0059 → VM-2105.

---

## 19.8 Reprise de mise au point

### VM-2106
Gestionnaire de reprise

### OBJET

Le composant assure la reprise d'une exécution interrompue en conservant
son état.

### RESPONSABILITÉS

Il :

- conserve l'état de l'exécution interrompue ;
- restaure les variables et objets nécessaires ;
- restaure la position d'exécution ;
- applique, lorsque la commande le permet, une nouvelle ligne de départ.

### EXIGENCES SATISFAITES

- EXI-0060.

### CONTRAINTES

La reprise ne réinitialise pas les valeurs acquises avant l'interruption.

Une modification de la position de départ ne détruit pas les autres
éléments conservés de l'état.

### INTERFACES

`REPRENDRE(execution, position)`

### TRAÇABILITÉ

HIS-13.1, HIS-13.3, HIS-13.4 → EXI-0060 → VM-2106.


# 20. Les erreurs

Les erreurs constituent un mécanisme transversal de la machine virtuelle.

Elles peuvent survenir :

- lors de l'analyse et de la compilation d'une ligne ;
- lors de l'exécution d'une représentation compilée ;
- lors de la résolution d'une référence ;
- lors de l'accès à un objet d'exécution ;
- lors d'une opération sur un tableau, une chaîne ou une donnée ;
- lors d'une opération de fichier ou de restauration d'un programme ;
- lors du traitement d'une commande.

Le système historique distingue explicitement les erreurs de syntaxe, détectées lors de l'entrée et de la compilation d'une ligne, des erreurs qui ne peuvent être détectées qu'au moment de l'exécution. Une ligne syntaxiquement incorrecte est rejetée, tandis qu'une ligne syntaxiquement correcte peut encore conduire ultérieurement à une erreur d'exécution. Le compilateur est chargé de détecter les erreurs syntaxiques ; l'interpréteur détecte les erreurs d'exécution.

La machine virtuelle doit donc distinguer :

- l'erreur de compilation ;
- l'erreur d'exécution ;
- l'erreur de service ou de traitement d'une opération demandée ;
- l'interruption volontaire de l'exécution.

Cette distinction ne constitue pas seulement une classification documentaire. Elle détermine le composant responsable du traitement, le moment du diagnostic, l'état pouvant être modifié et la possibilité de poursuite.

Les sources historiques ne permettent pas d'établir la représentation interne exacte des erreurs et des diagnostics. La machine virtuelle définit donc une représentation logique minimale, indépendante du format des messages historiques.

---

## 20.1 Diagnostic d'une erreur de compilation

### VM-1901
Diagnostic de compilation

### OBJET

Définir le composant chargé de produire et de transmettre le diagnostic résultant de l'échec de compilation d'une ligne.

Le diagnostic de compilation appartient au traitement de la ligne validée.

Il ne constitue pas un objet du programme courant.

### RESPONSABILITÉS

Le composant doit :

1. recevoir le résultat négatif de l'analyse ou de la compilation ;
2. identifier la nature générale de l'échec ;
3. associer le diagnostic à la ligne en cours de traitement ;
4. fournir les informations nécessaires à sa présentation à l'utilisateur ;
5. transmettre le diagnostic à l'environnement conversationnel ;
6. ne provoquer aucune modification du programme courant ;
7. permettre la correction et la réintroduction ultérieure de la ligne.

Le composant ne doit pas :

- insérer une ligne partiellement compilée ;
- remplacer une ligne existante à partir d'une compilation échouée ;
- modifier l'état d'une autre ligne du programme courant ;
- transformer une erreur de compilation en erreur d'exécution.

### EXIGENCES SATISFAITES

EXI-0004 — Rejet atomique d'une ligne incorrecte.

EXI-0018 — Analyse syntaxique préalable à la compilation.

EXI-0051 — Diagnostic immédiat des erreurs syntaxiques.

EXI-0020 — Production d'une représentation compilée par ligne.

### CONTRAINTES

Les sources historiques établissent que la ligne est analysée lors de son entrée, que les erreurs de syntaxe sont signalées immédiatement et qu'une ligne incorrecte n'est pas prise en compte. Le manuel LSE 15 précise que la ligne incorrecte doit être corrigée puis refrappée, les lignes antérieurement introduites n'ayant pas à être réintroduites.

Le Tome II fixe en conséquence :

- la détection préalable des erreurs syntaxiques ;
- la production d'un diagnostic immédiat ;
- le rejet de la ligne incorrecte ;
- la préservation de l'état antérieur du programme courant.

Le format exact des messages historiques n'est pas reconstruit. Le catalogue indique explicitement que le format exact des messages n'est pas fixé.

### INTERFACES

#### Interface avec VM-0501 — Validation de la ligne

Entrée :

- ligne validée ;
- contexte de saisie ;
- état du programme courant avant traitement.

Sortie :

- diagnostic de compilation ;
- maintien de l'état antérieur du programme courant.

#### Interface avec VM-0502 — Analyse syntaxique

Entrée :

- résultat négatif de l'analyse.

Sortie :

- nature abstraite de l'erreur ;
- position logique disponible ;
- informations nécessaires au diagnostic.

#### Interface avec VM-0504 — Génération de la représentation compilée

Aucune invocation de VM-0504 ne doit conduire à l'intégration d'un objet compilé lorsque VM-1901 reçoit un échec de compilation.

#### Interface avec VM-2001 — Cycle conversationnel

Le diagnostic est remis au cycle conversationnel.

Le cycle conversationnel restitue le contrôle à l'utilisateur après présentation du diagnostic.

### DONNÉES MANIPULÉES

Le diagnostic logique comprend au minimum :

- une catégorie d'erreur ;
- une indication de localisation lorsque celle-ci est déterminable ;
- une description destinée à l'utilisateur ;
- une référence à l'opération ayant échoué.

La catégorie d'erreur peut être :

- syntaxique ;
- lexicale, lorsque cette distinction est pertinente ;
- structurelle ;
- sémantique précoce, uniquement si elle est détectée dans la phase de compilation.

La représentation concrète de ces informations n'est pas fixée.

Le diagnostic peut être enrichi par une information historique équivalente à un numéro ou à un code d'erreur, sans que ce code soit imposé par la machine virtuelle.

### ALGORITHMES UTILISÉS

Le traitement abstrait est :

1. recevoir l'échec de compilation ;
2. déterminer la catégorie abstraite du diagnostic ;
3. associer la ligne concernée ;
4. construire le diagnostic ;
5. transmettre le diagnostic à l'environnement conversationnel ;
6. abandonner le résultat compilé temporaire ;
7. conserver le programme courant inchangé.

Aucun algorithme historique précis de construction du message n'est imposé.

### STATUT

Déduit.

### INVARIANTS

À l'issue d'un échec traité par VM-1901 :

- le programme courant est identique à son état antérieur ;
- aucun objet compilé nouveau n'est observable dans le programme courant ;
- une ligne existante n'est pas remplacée ;
- le diagnostic est disponible dans l'environnement conversationnel ;
- le compilateur peut recevoir ultérieurement une nouvelle saisie.

### CONSÉQUENCES

La gestion des erreurs de compilation est transactionnelle au niveau logique de la ligne.

Le programme courant ne peut être partiellement modifié par une compilation échouée.

La frontière entre « ligne en cours de compilation » et « programme courant » est donc normative.

Cette propriété permet de garantir simultanément l'indépendance des lignes et la stabilité du programme courant.

### TRAÇABILITÉ

HIS-5.2 → EXI-0018, EXI-0051 → VM-0502, VM-1901.

HIS-6.5 → EXI-0004, EXI-0001, EXI-0051 → VM-0302, VM-0502, VM-1901.

HIS-12.1 → EXI-0051 → VM-1901.

VM-0502 → VM-1901.

VM-1901 → VM-2001.

---

## 20.2 Diagnostic d'une erreur d'exécution

### VM-1902
Diagnostic d'exécution

### OBJET

Définir le composant chargé de traiter les erreurs détectées pendant l'exécution d'une représentation compilée.

Le diagnostic d'exécution intervient après que la représentation compilée a été acceptée par le programme courant.

Il ne remet pas en cause la validité syntaxique de la ligne qui a produit l'objet compilé.

### RESPONSABILITÉS

Le composant doit :

1. recevoir l'indication d'une erreur produite par l'interpréteur ou par un composant appelé par celui-ci ;
2. identifier le contexte d'exécution dans lequel l'erreur est survenue ;
3. déterminer la position logique d'exécution disponible ;
4. préserver les informations d'état nécessaires à l'observation ou à la reprise lorsque le mécanisme applicable le permet ;
5. produire un diagnostic d'exécution ;
6. transmettre le diagnostic à l'environnement conversationnel ;
7. placer l'exécution dans l'état correspondant à son arrêt ou à sa terminaison.

Le composant ne doit pas :

- modifier le programme courant en raison du seul diagnostic ;
- confondre une erreur d'exécution avec une erreur de compilation ;
- effacer arbitrairement l'état d'exécution avant que le protocole applicable ait déterminé sa conservation.

### EXIGENCES SATISFAITES

EXI-0019 — Distinction des erreurs de compilation et d'exécution.

EXI-0031 — État d'exécution distinct du programme compilé.

EXI-0033 — Observation de l'état d'exécution.

EXI-0024 — Conservation d'une position d'exécution interrompue, lorsque le mécanisme de reprise applicable le prévoit.

EXI-0032 — Résolution des références vers les objets d'exécution.

### CONTRAINTES

Le manuel de 1975 distingue les erreurs de syntaxe détectées lors de l'entrée des erreurs qui ne peuvent être détectées qu'à l'exécution. L'exemple d'une variable chaîne non déclarée montre qu'une ligne peut rester syntaxiquement acceptable alors qu'une erreur apparaît ultérieurement lors de son utilisation.

Le Tome II reconstruit en conséquence la résolution des références comme une opération de l'environnement d'exécution. Une référence peut être compilée sans que l'existence de l'objet soit définitivement validée ; l'interpréteur effectue la résolution lorsque l'objet est effectivement requis.

Les sources documentent également la possibilité d'interrompre une exécution, d'examiner l'état du programme et, dans les mécanismes prévus, de reprendre l'exécution.

### INTERFACES

#### Interface avec VM-0701 — Interpréteur

Entrée :

- erreur d'exécution ;
- position d'exécution ;
- état courant de l'exécution.

Sortie :

- diagnostic ;
- état de suspension ou de terminaison.

#### Interface avec VM-0801 — État d'exécution

VM-1902 peut demander :

- la position d'exécution ;
- le cadre d'activation courant ;
- l'état de la pile d'évaluation ;
- le contexte de visibilité applicable.

VM-1902 peut demander la conservation de cet état lorsque la reprise est autorisée.

#### Interface avec VM-0803 — Accès à l'état d'exécution

Lorsque l'exécution demeure observable après l'erreur, le diagnostic doit pouvoir être accompagné de l'état autorisé à l'utilisateur.

#### Interface avec VM-2001 — Cycle conversationnel

Le diagnostic est transmis au dialogue.

Le cycle conversationnel reprend selon l'état produit :

- exécution terminée ;
- exécution suspendue ;
- attente d'une commande de reprise.

### DONNÉES MANIPULÉES

Le composant manipule :

- une catégorie d'erreur d'exécution ;
- une position logique ;
- une référence éventuelle vers l'objet concerné ;
- le contexte d'activation ;
- le diagnostic destiné à l'utilisateur ;
- un état de l'exécution.

Les catégories concrètes ne sont pas imposées.

Elles doivent toutefois permettre de distinguer au minimum :

- référence non résolue ;
- objet non déclaré ou indisponible ;
- accès invalide à un objet ;
- indice de tableau invalide ;
- opération arithmétique impossible ;
- opération sur chaîne invalide ;
- erreur de fichier transmise par le bibliothécaire ;
- erreur générale signalée par un composant d'exécution.

### ALGORITHMES UTILISÉS

Le traitement abstrait est :

1. l'interpréteur détecte une condition d'erreur ;
2. il interrompt l'opération courante ;
3. il capture la position logique disponible ;
4. il transmet l'erreur à VM-1902 ;
5. VM-1902 construit le diagnostic ;
6. VM-1902 détermine l'état terminal ou suspendu ;
7. le contrôle est rendu à VM-2001.

La gestion de l'erreur doit intervenir avant toute poursuite de l'opération fautive.

### STATUT

Déduit.

### INVARIANTS

Une erreur d'exécution :

- n'ajoute aucun objet au programme courant ;
- ne modifie pas le numéro logique des lignes ;
- ne transforme pas une représentation compilée acceptée en représentation invalide ;
- arrête au minimum l'opération fautive ;
- conserve l'état d'exécution lorsque la reprise est autorisée ;
- ne reprend jamais automatiquement l'opération fautive sans traitement explicite.

### CONSÉQUENCES

L'interpréteur et la gestion des erreurs sont distincts.

L'interpréteur détecte la condition.

VM-1902 la transforme en état architectural observable.

La distinction est nécessaire pour préserver la possibilité de :

- notifier l'utilisateur ;
- examiner l'état ;
- continuer ;
- reprendre à un autre point lorsque le langage et le contexte le permettent ;
- terminer l'exécution.

### TRAÇABILITÉ

HIS-5.2 → EXI-0019 → VM-0503, VM-0701, VM-1902.

HIS-8.3 → EXI-0031, EXI-0033 → VM-0801, VM-0803, VM-1902.

HIS-3.13 → EXI-0024, EXI-0031, EXI-0033 → VM-0604, VM-0801, VM-0803, VM-1902.

VM-0701 → VM-1902.

VM-0801 → VM-1902.

---

## 20.3 Classification logique des erreurs

### VM-1903
Classification des erreurs

### OBJET

Définir la séparation abstraite entre les familles d'erreurs afin de déterminer le composant responsable du diagnostic et le traitement applicable.

### RESPONSABILITÉS

Le composant doit :

- distinguer les erreurs de compilation des erreurs d'exécution ;
- distinguer les erreurs résultant d'une commande ou d'une opération de service des erreurs produites pendant l'interprétation ;
- fournir une catégorie stable au mécanisme de diagnostic ;
- ne pas imposer un code numérique historique ;
- ne pas confondre une interruption volontaire avec une erreur.

### EXIGENCES SATISFAITES

EXI-0019.

EXI-0051.

EXI-0030.

EXI-0031.

EXI-0052.

### CONTRAINTES

La documentation historique sépare le compilateur, l'interpréteur, le décompilateur, le bibliothécaire et l'analyseur de commandes. Les sources ne permettent pas d'établir que tous ces composants utilisaient une représentation interne commune des erreurs.

La classification définie par la machine virtuelle sert donc à organiser l'architecture reconstruite ; elle ne prétend pas reproduire un codage historique interne.

### INTERFACES

Le composant reçoit un événement d'échec depuis :

- VM-0502 ;
- VM-0503 ;
- VM-0701 ;
- VM-1702 ;
- VM-1801 ;
- VM-2002.

Il fournit une catégorie à :

- VM-1901 ;
- VM-1902 ;
- VM-1904 ;
- VM-2001.

### DONNÉES MANIPULÉES

Une erreur abstraite possède :

- un domaine ;
- une phase ;
- une localisation éventuelle ;
- une description ;
- un contexte ;
- un état de traitement.

Les domaines minimaux sont :

1. COMPILATION ;
2. EXECUTION ;
3. SERVICE ;
4. DIALOGUE.

Une interruption volontaire n'est pas une erreur.

### ALGORITHMES UTILISÉS

La classification est déterminée par le composant qui constate l'échec.

Une erreur détectée avant acceptation de l'objet compilé appartient au domaine de compilation.

Une erreur détectée pendant l'interprétation appartient au domaine d'exécution.

Une erreur constatée lors du chargement, de la sauvegarde ou d'une opération du bibliothécaire appartient au domaine de service.

Une entrée qui ne peut être orientée vers un traitement valide relève du domaine de dialogue.

### STATUT

Choix de conception.

### INVARIANTS

Une erreur possède un seul domaine principal.

Une erreur ne change pas de domaine lorsqu'elle est transmise d'un composant à un autre.

Le diagnostic peut être enrichi lors de sa propagation, mais sa cause principale demeure stable.

### CONSÉQUENCES

La machine virtuelle évite une centralisation excessive des responsabilités.

Chaque composant connaît la classe d'échec qu'il est capable de détecter.

La gestion commune des diagnostics ne supprime pas la séparation fonctionnelle des processeurs.

### TRAÇABILITÉ

HIS-3.11 → EXI-0001 → VM-0001, VM-1903.

HIS-5.2 → EXI-0019, EXI-0051 → VM-0502, VM-1901, VM-1903.

HIS-8.2 → EXI-0019, EXI-0030 → VM-0701, VM-1902, VM-1903.

HIS-3.1 → EXI-0052 → VM-2001, VM-1903.

---

## 20.4 État d'erreur d'une exécution

### VM-1904
État d'erreur

### OBJET

Définir l'état abstrait dans lequel une exécution se trouve lorsqu'une erreur d'exécution a interrompu le flux normal.

### RESPONSABILITÉS

Le composant doit :

- enregistrer qu'une erreur a interrompu l'exécution ;
- préserver la position logique disponible ;
- conserver l'état d'exécution nécessaire aux opérations autorisées ;
- empêcher la poursuite implicite du flux ;
- permettre à l'environnement conversationnel de consulter l'état lorsque celui-ci reste accessible ;
- permettre soit une reprise autorisée, soit une terminaison.

### EXIGENCES SATISFAITES

EXI-0031.

EXI-0033.

EXI-0024.

EXI-0030.

### CONTRAINTES

Les sources décrivent une continuité entre exécution et mise au point. L'utilisateur peut interrompre l'exécution, examiner les valeurs, puis continuer ou reprendre selon les commandes disponibles.

La machine abstraite distingue donc :

- l'exécution active ;
- l'exécution suspendue ;
- l'exécution en erreur ;
- l'exécution terminée.

Ces états ne doivent pas être confondus avec le programme courant, qui demeure un état distinct.

### INTERFACES

#### Interface avec VM-0801

VM-1904 conserve :

- la position ;
- les cadres d'activation ;
- la pile d'évaluation ;
- les informations d'accès nécessaires.

#### Interface avec VM-0803

L'état d'erreur peut être interrogé si le mode courant l'autorise.

#### Interface avec VM-2102

Une commande de reprise peut être appliquée à l'état d'erreur si elle est compatible avec celui-ci.

### DONNÉES MANIPULÉES

L'état d'erreur comprend :

- le diagnostic principal ;
- la position d'arrêt ;
- l'état dynamique de l'exécution ;
- le contexte d'activation ;
- l'information indiquant si une reprise est possible.

### ALGORITHMES UTILISÉS

À la détection d'une erreur :

1. suspendre le flux ;
2. conserver la position ;
3. enregistrer le diagnostic ;
4. déterminer les opérations disponibles ;
5. rendre le contrôle au dialogue.

Aucune reprise automatique n'est exécutée.

### STATUT

Déduit.

### INVARIANTS

Une exécution en état d'erreur n'exécute aucune opération supplémentaire tant qu'une transition explicite n'a pas été demandée.

L'état d'erreur n'altère pas le programme courant.

Une exécution en état d'erreur peut être détruite sans supprimer le programme courant.

### CONSÉQUENCES

La possibilité de reprendre une exécution n'est jamais déduite du seul fait qu'une erreur a été signalée.

La reprise dépend de la conservation effective de l'état et des règles de la commande utilisée.

### TRAÇABILITÉ

HIS-3.13 → EXI-0031, EXI-0033 → VM-0801, VM-0803, VM-1904.

HIS-12.5 → EXI-0024 → VM-0604, VM-1904.

HIS-14.1 → EXI-0030 → VM-0705, VM-1904.

---

## 20.5 Interruption volontaire distincte de l'erreur

### VM-1905
Interruption d'exécution

### OBJET

Définir la représentation architecturale d'un arrêt provoqué volontairement par l'utilisateur ou par une instruction de mise au point.

### RESPONSABILITÉS

Le composant doit :

- arrêter le flux d'exécution ;
- conserver la position d'exécution ;
- maintenir l'état dynamique applicable ;
- ne produire aucun diagnostic d'erreur lorsqu'aucune erreur n'est survenue ;
- permettre l'examen ou la modification autorisés de l'état ;
- permettre une continuation ou une reprise selon le mécanisme utilisé.

### EXIGENCES SATISFAITES

EXI-0024.

EXI-0031.

EXI-0033.

EXI-0036.

### CONTRAINTES

La touche d'interruption permet à l'utilisateur de reprendre le contrôle de sa console pendant l'exécution. CONTINUER reprend l'exécution à l'endroit de l'arrêt ; REPRENDRE peut permettre de repartir d'une autre ligne tout en conservant les valeurs antérieurement affectées dans les conditions documentées.

L'instruction PAUSE et la commande PAS À PAS constituent également des mécanismes de mise au point entraînant un arrêt contrôlé.

### INTERFACES

Le composant interagit avec :

- VM-0801 pour la conservation de l'état ;
- VM-0803 pour l'observation ;
- VM-2102 pour CONTINUER ;
- VM-2103 pour REPRENDRE ;
- VM-2001 pour le retour au dialogue.

### DONNÉES MANIPULÉES

- cause de l'interruption ;
- position logique ;
- état d'exécution ;
- mode de reprise disponible.

### ALGORITHMES UTILISÉS

À l'événement d'interruption :

1. suspendre l'exécution ;
2. conserver la position ;
3. placer la console dans l'état conversationnel approprié ;
4. exposer l'état autorisé ;
5. attendre une commande de contrôle.

### STATUT

Déduit.

### INVARIANTS

Une interruption volontaire n'est pas une erreur.

L'état du programme courant demeure inchangé.

L'état d'exécution est conservé jusqu'à la transition suivante.

### CONSÉQUENCES

La machine virtuelle distingue formellement :

- l'arrêt par erreur ;
- l'arrêt volontaire ;
- la fin normale ;
- la fin explicite demandée par le programme.

Cette distinction permet de conserver la cohérence du traitement des diagnostics et des reprises.

### TRAÇABILITÉ

HIS-12.5 → EXI-0024 → VM-0604, VM-1905.

HIS-13.1 → EXI-0033 → VM-0803, VM-1905.

HIS-13.2 → EXI-0031, EXI-0033 → VM-0801, VM-0803, VM-1905.

---

# 21. Les interfaces entre composants

Le Tome I documente une séparation fonctionnelle entre plusieurs processeurs, tout en indiquant que les sources ne permettent pas d'établir leurs interfaces internes exactes ni les structures de données qu'ils s'échangent. Le Tome III doit donc définir des interfaces logiques explicites sans les présenter comme des reproductions historiques certaines.

La règle générale est la suivante :

**Chaque interface expose une fonction abstraite, des données d'entrée, des données de sortie, les préconditions, les postconditions et les invariants de cohérence.**

Les interfaces décrites ci-dessous constituent des interfaces d'architecture. Elles ne sont ni des interfaces de langage d'implémentation, ni des interfaces de communication dépendant d'une machine particulière.

---

## 21.1 Interface du cycle conversationnel

### VM-2501
Interface conversationnelle

### OBJET

Définir le point d'entrée logique de la machine virtuelle pour une demande utilisateur validée.

### RESPONSABILITÉS

L'interface doit :

- recevoir une demande ;
- orienter cette demande ;
- recueillir le résultat ;
- restituer un diagnostic ou un résultat ;
- maintenir la continuité du dialogue.

### EXIGENCES SATISFAITES

EXI-0052.

EXI-0033.

EXI-0028.

EXI-0029.

### CONTRAINTES

Le système historique fonctionne en mode conversationnel et distingue plusieurs catégories de traitement : ligne de programme, commande, mode machine de bureau et demande adressée à un processeur.

### INTERFACES

Entrée :

- caractères formant une demande validée.

Sortie :

- résultat ;
- diagnostic ;
- état conversationnel ;
- état d'exécution suspendu, lorsque nécessaire.

### DONNÉES MANIPULÉES

- demande ;
- catégorie ;
- contexte de session ;
- résultat ;
- diagnostic.

### ALGORITHMES UTILISÉS

1. recevoir ;
2. classifier ;
3. orienter ;
4. exécuter le traitement ;
5. restituer ;
6. rendre le contrôle au dialogue.

### STATUT

Déduit.

### INVARIANTS

Toute demande validée est orientée vers un traitement déterminé.

Une demande non reconnue ne provoque pas une exécution indéterminée.

Le retour au dialogue est possible après un traitement terminé ou suspendu.

### CONSÉQUENCES

Cette interface devient le point de convergence entre les processeurs.

### TRAÇABILITÉ

HIS-3.1 → EXI-0052 → VM-2001, VM-2501.

HIS-3.11 → EXI-0001, EXI-0052 → VM-0001, VM-2501.

HIS-3.13 → EXI-0033 → VM-0803, VM-2501.

---

## 21.2 Interface entre l'analyseur de commandes et les processeurs

### VM-2502
Interface d'orientation des demandes

### OBJET

Définir la relation logique entre l'analyseur de commandes et les composants fonctionnels.

### RESPONSABILITÉS

L'interface doit permettre de transmettre :

- une ligne de programme au compilateur ;
- une expression immédiate à l'interpréteur ;
- une demande de décompilation au décompilateur ;
- une opération de fichier au bibliothécaire ;
- une commande de contrôle à l'environnement d'exécution.

### EXIGENCES SATISFAITES

EXI-0052.

EXI-0001.

### CONTRAINTES

Le manuel décrit un enchaînement dans lequel la demande utilisateur est reconnue puis dirigée vers le processeur compétent. Le découpage exact des appels internes n'est cependant pas documenté.

### INTERFACES

L'analyseur produit une demande structurée abstraite contenant :

- la catégorie ;
- les paramètres ;
- la référence éventuelle au programme courant ;
- la référence éventuelle à l'état d'exécution.

Le processeur appelé produit :

- succès ;
- résultat ;
- diagnostic ;
- nouvel état.

### DONNÉES MANIPULÉES

- demande analysée ;
- paramètres ;
- contexte ;
- résultat.

### ALGORITHMES UTILISÉS

Classification par catégorie fonctionnelle.

Aucun mécanisme de classement matériel n'est imposé.

### STATUT

Déduit.

### INVARIANTS

Une demande n'est adressée qu'à un processeur dont la responsabilité couvre la catégorie déterminée.

### CONSÉQUENCES

Les processeurs restent fonctionnellement indépendants.

Leur coopération est organisée par contrats d'interface.

### TRAÇABILITÉ

HIS-4.5.5 → EXI-0052 → VM-2502.

HIS-4.5.6 → EXI-0052 → VM-2502.

HIS-3.11 → EXI-0001 → VM-2502.

### Routage détaillé des commandes

Le contrat conserve la classification historique des commandes et le routage fonctionnel correspondant au diagramme INRDP75 :

| Domaine | Commandes / opérations | Processeur ou service principal |
|---|---|---|
| Action immédiate | BO, AU, PA, NO, PR, EF, AB, IN, UT, TA | moniteur / environnement de dialogue |
| Décompilation | LI, PE, NU, DE | décompilateur, avec bibliothécaire pour la destination de DECODER |
| Exécution / poursuite | EX, CO, RE, PO | interpréteur / environnement d'exécution |
| Fichiers | AP, RA, MO, SU, CA, EN, ID | bibliothécaire et services associés |

`GARER` et `CHARGER` sont des opérations d'instructions et non des commandes autonomes ; leur routage relève du bibliothécaire et des services de fichiers. [INRDP75 §3.3 ; §4 ; Annexe II]

Le système complète l'affichage du nom d'une commande après réception des deux premières lettres et du caractère de fin de ligne, sauf lorsque le mode abrégé est actif. Les paramètres sont ensuite reçus dans le contexte de la commande. [INRDP75 §4]

**Statut : Déduit à partir du diagramme et des descriptions de commandes.**

---

## 21.3 Interface compilateur-programme courant

### VM-2503
Interface d'intégration compilée

### OBJET

Définir la remise d'un objet compilé accepté au gestionnaire du programme courant.

### RESPONSABILITÉS

L'interface doit :

- recevoir un objet compilé validé ;
- localiser la ligne correspondante ;
- remplacer l'objet de même numéro s'il existe ;
- conserver l'ordre logique ;
- signaler l'acceptation.

### EXIGENCES SATISFAITES

EXI-0004.

EXI-0006.

EXI-0008.

EXI-0020.

EXI-0026.

### CONTRAINTES

Les lignes acceptées sont mémorisées et ordonnées selon leur numéro. Une ligne nouvelle portant le même numéro remplace l'ancienne.

### INTERFACES

Entrée :

- objet compilé validé.

Sortie :

- programme courant mis à jour ;
- résultat d'acceptation.

En cas d'échec :

- aucun changement.

### DONNÉES MANIPULÉES

- numéro de ligne ;
- objet compilé ;
- collection ordonnée du programme courant.

### ALGORITHMES UTILISÉS

1. vérifier que l'objet est accepté ;
2. rechercher le numéro ;
3. remplacer ou insérer ;
4. maintenir l'ordre logique ;
5. publier le nouvel état.

### STATUT

Déduit.

### INVARIANTS

Après acceptation :

- un numéro identifie au plus un objet ;
- l'ordre logique croissant est conservé ;
- les autres lignes demeurent inchangées.

### CONSÉQUENCES

Le programme courant peut être modifié ligne par ligne.

### TRAÇABILITÉ

HIS-6.2 → EXI-0001, EXI-0006, EXI-0008 → VM-0303, VM-2503.

HIS-6.5 → EXI-0004, EXI-0020, EXI-0026 → VM-0302, VM-0505, VM-2503.

---

## 21.4 Interface programme courant-interpréteur

### VM-2504
Interface d'exécution du programme

### OBJET

Définir la transmission du programme courant à l'interpréteur.

### RESPONSABILITÉS

L'interface doit fournir :

- la représentation compilée ;
- l'ordre logique des lignes ;
- les informations nécessaires au démarrage ;
- les références vers l'environnement d'exécution.

### EXIGENCES SATISFAITES

EXI-0005.

EXI-0008.

EXI-0023.

EXI-0027.

EXI-0029.

EXI-0031.

### CONTRAINTES

Le Tome II distingue clairement le programme courant, qui constitue l'état compilé, et l'état d'exécution créé au lancement de l'interprétation.

### INTERFACES

Entrée :

- programme courant ;
- point initial éventuel ;
- contexte d'exécution.

Sortie :

- résultat d'exécution ;
- nouvel état d'exécution ;
- diagnostic.

### DONNÉES MANIPULÉES

- objet compilé ;
- position logique ;
- environnement ;
- état d'exécution.

### ALGORITHMES UTILISÉS

L'interpréteur initialise l'exécution puis parcourt les objets selon l'ordre logique ou selon le contrôle explicite du flux.

### STATUT

Déduit.

### INVARIANTS

Le programme courant n'est pas modifié par la simple exécution.

L'exécution manipule un état distinct.

### CONSÉQUENCES

Une modification ultérieure du programme courant ne transforme pas rétroactivement l'état dynamique déjà créé pour une exécution suspendue.

### TRAÇABILITÉ

HIS-8.2 → EXI-0023, EXI-0029 → VM-0603, VM-0703, VM-2504.

HIS-3.13 → EXI-0031 → VM-0801, VM-2504.

---

## 21.5 Interface interpréteur-environnement d'exécution

### VM-2505
Interface d'environnement d'exécution

### OBJET

Définir l'ensemble des services abstraits dont dispose l'interpréteur pour exécuter une représentation compilée.

### RESPONSABILITÉS

L'interface doit fournir :

- résolution d'identificateur ;
- accès aux variables ;
- accès aux tableaux ;
- accès aux chaînes ;
- gestion des cadres d'activation ;
- gestion de la pile d'évaluation ;
- accès aux entrées-sorties ;
- production de diagnostics ;
- modification de la position d'exécution.

### EXIGENCES SATISFAITES

EXI-0031.

EXI-0032.

EXI-0034.

EXI-0035.

EXI-0037.

EXI-0038.

EXI-0039.

EXI-0040.

EXI-0041.

EXI-0050.

### CONTRAINTES

Le Tome II définit un environnement d'exécution distinct de la représentation compilée et une résolution abstraite des références.

### INTERFACES

L'interpréteur invoque les opérations :

- `résoudre` ;
- `lire` ;
- `écrire` ;
- `allouer` ;
- `libérer` ;
- `empiler` ;
- `dépiler` ;
- `entrer_activation` ;
- `sortir_activation` ;
- `demander_entree` ;
- `produire_sortie`.

Les noms ci-dessus sont des désignations abstraites et ne constituent pas une interface d'implémentation.

### DONNÉES MANIPULÉES

- identificateurs logiques ;
- objets d'exécution ;
- valeurs ;
- cadres d'activation ;
- références ;
- tableaux ;
- chaînes ;
- état de contrôle.

### ALGORITHMES UTILISÉS

Résolution selon le contexte actif.

Création d'objets selon leur déclaration.

Gestion de la durée de vie selon le contexte.

### STATUT

Choix de conception.

### INVARIANTS

Une référence compilée ne constitue jamais directement une adresse matérielle.

Un objet inaccessible dans le contexte courant ne peut pas être résolu.

### CONSÉQUENCES

L'interpréteur reste indépendant de l'organisation mémoire concrète.

### TRAÇABILITÉ

HIS-10.1 → EXI-0032, EXI-0038 → VM-0802, VM-1101, VM-2505.

HIS-13.1 → EXI-0032 → VM-0802, VM-2505.

HIS-10.6 → EXI-0040, EXI-0041, EXI-0050 → VM-1201, VM-1202, VM-1602, VM-2505.

---

## 21.6 Interface programme compilé-décompilateur

### VM-2506
Interface de décompilation

### OBJET

Définir l'accès du décompilateur au programme compilé.

### RESPONSABILITÉS

L'interface doit :

- fournir les objets compilés ;
- fournir leur ordre logique ;
- permettre la restitution de leur représentation textuelle ;
- signaler les informations insuffisantes pour une décompilation valide.

### EXIGENCES SATISFAITES

EXI-0003.

EXI-0010.

EXI-0015.

EXI-0017.

EXI-0022.

### CONTRAINTES

La représentation compilée est utilisée à la fois pour l'exécution et la décompilation. Le Tome II conserve toutefois la distinction documentaire entre chaîne codée attestée pour le L.S.E. et représentation post-fixée reconstruite.

### INTERFACES

Entrée :

- objet compilé ;
- identification de ligne.

Sortie :

- représentation textuelle ;
- diagnostic éventuel.

### DONNÉES MANIPULÉES

- opération codée ;
- valeur compilée ;
- référence logique ;
- information de ligne ;
- structure nécessaire à la restitution.

### ALGORITHMES UTILISÉS

Parcours de la représentation compilée.

Reconstruction déterministe de la forme textuelle.

### STATUT

Déduit.

### INVARIANTS

La décompilation ne modifie pas le programme courant.

Elle ne modifie pas l'état d'exécution.

Elle produit une représentation textuelle valide de la construction représentée.

### CONSÉQUENCES

Le décompilateur dépend de la représentation compilée mais n'a pas besoin du texte source initial.

### TRAÇABILITÉ

HIS-4.5.2 → EXI-0003, EXI-0010 → VM-0405, VM-1801, VM-2506.

CPF-7.3 → EXI-0015, EXI-0017 → VM-0406, VM-2506.

---

## 21.7 Interface bibliothécaire-stockage logique

### VM-2507
Interface de persistance

### OBJET

Définir le contrat entre le bibliothécaire et les représentations persistantes des programmes et des données.

### RESPONSABILITÉS

L'interface doit permettre :

- le rangement d'un programme compilé ;
- la lecture d'un programme compilé ;
- la validation d'une représentation chargée ;
- le transfert d'un programme vers le programme courant ;
- la signalisation des erreurs de chargement.

### EXIGENCES SATISFAITES

EXI-0009.

EXI-0022.

EXI-0025.

EXI-0049.

### CONTRAINTES

Le manuel décrit le rangement des programmes sous forme codée et le chargement d'un programme faisant de celui-ci le programme courant. Le bibliothécaire est le processeur chargé de la gestion des fichiers.

### INTERFACES

Entrée :

- opération de fichier ;
- identifiant de fichier ;
- programme ou données.

Sortie :

- succès ;
- représentation persistante ;
- programme restauré ;
- diagnostic.

### DONNÉES MANIPULÉES

- programme compilé ;
- représentation persistante ;
- fichier-programme ;
- fichier-données.

### ALGORITHMES UTILISÉS

Les opérations sont abstraitement :

- ranger ;
- charger ;
- valider ;
- remplacer le programme courant.

### STATUT

Déduit.

### INVARIANTS

Un chargement échoué ne remplace pas le programme courant valide.

Une représentation persistante de programme ne doit pas être confondue avec le texte source.

### CONSÉQUENCES

Le stockage logique est séparé de la mémoire d'exécution.

### TRAÇABILITÉ

HIS-11.2 → EXI-0009, EXI-0022 → VM-1701, VM-2507.

HIS-11.5 → EXI-0025 → VM-1702, VM-2507.

HIS-3.14 → EXI-0049 → VM-1601, VM-2507.

---

## 21.8 Interface mémoire-composants

### VM-2508
Interface de gestion des espaces mémoire

### OBJET

Définir les services abstraits par lesquels les composants demandent ou libèrent des objets d'exécution.

### RESPONSABILITÉS

L'interface doit :

- allouer un objet ;
- libérer un objet ;
- maintenir la distinction entre espaces ;
- garantir qu'un objet reste accessible pendant sa durée de vie logique ;
- permettre la récupération d'espace.

### EXIGENCES SATISFAITES

EXI-0041.

EXI-0049.

EXI-0050.

### CONTRAINTES

La machine abstraite distingue le stockage logique, le programme courant et l'état d'exécution. Le langage permet également la libération explicite de certains objets tels que les tableaux.

### INTERFACES

Les composants adressent des opérations abstraites :

- création ;
- allocation ;
- libération ;
- accès ;
- transfert de propriété logique.

### DONNÉES MANIPULÉES

- objet ;
- durée de vie ;
- contexte propriétaire ;
- taille logique ;
- état d'accessibilité.

### ALGORITHMES UTILISÉS

Les algorithmes historiques d'allocation ne sont pas reconstruits.

### STATUT

Déduit.

### INVARIANTS

La libération d'un objet ne doit pas rendre invalide une référence encore légitime selon la sémantique du contexte.

Un objet libéré n'est plus accessible par son identificateur courant.

Le programme persistant ne partage pas la même durée de vie que l'état d'exécution.

### CONSÉQUENCES

Les composants ne manipulent pas directement une représentation mémoire physique.

### TRAÇABILITÉ

HIS-10.6 → EXI-0050 → VM-1602, VM-2508.

HIS-3.14 → EXI-0049 → VM-1601, VM-2508.

---

## 21.9 Interface des registres virtuels

### VM-2509
Interface des registres virtuels

### OBJET

Définir les points de lecture et de modification des registres virtuels utilisés par l'exécution.

### RESPONSABILITÉS

L'interface doit permettre à l'interpréteur et aux composants de contrôle d'accéder aux registres abstraits sans dépendre d'un processeur réel.

### EXIGENCES SATISFAITES

EXI-0031.

EXI-0046.

EXI-0024.

### CONTRAINTES

Le Tome II distingue explicitement la position d'exécution et les informations dynamiques nécessaires à la progression et à la reprise.

### INTERFACES

Les registres virtuels exposent au minimum :

- registre de position de programme ;
- registre de position dans l'objet compilé ;
- registre de retour ;
- registre d'état d'exécution ;
- registre de résultat.

### DONNÉES MANIPULÉES

Les valeurs des registres sont abstraites.

Elles ne correspondent pas à des registres physiques.

### ALGORITHMES UTILISÉS

Lecture et écriture contrôlées.

Mise à jour atomique des transitions de flux.

### STATUT

Choix de conception.

### INVARIANTS

Les registres virtuels sont cohérents avec l'état d'exécution actif.

Un état suspendu conserve les registres nécessaires à la reprise.

### CONSÉQUENCES

Le moteur d'exécution peut être implémenté sur toute organisation matérielle compatible.

### TRAÇABILITÉ

HIS-8.2 → EXI-0046 → VM-1402, VM-2509.

HIS-8.3 → EXI-0024, EXI-0031 → VM-0604, VM-0801, VM-2509.

---

## 21.10 Contrat général des interfaces

### VM-2510
Contrat d'interface intercomposants

### OBJET

Définir les règles communes applicables à toutes les interfaces de la machine virtuelle.

### RESPONSABILITÉS

Toute interface doit :

- définir son entrée ;
- définir sa sortie ;
- définir ses préconditions ;
- définir ses postconditions ;
- préciser les erreurs possibles ;
- préciser le composant propriétaire de chaque état ;
- distinguer les données persistantes des données temporaires.

### EXIGENCES SATISFAITES

EXI-0001.

EXI-0049.

EXI-0052.

### CONTRAINTES

Les sources historiques établissent la séparation fonctionnelle des processeurs mais ne fournissent pas leurs interfaces internes. La présente architecture définit donc des contrats logiques reconstruits, et non des interfaces historiques affirmées.

### INTERFACES

Tout appel intercomposant doit posséder :

- une origine ;
- une destination ;
- un type de demande ;
- un contexte ;
- un résultat ;
- un état d'erreur éventuel.

### DONNÉES MANIPULÉES

Les données sont toujours décrites par leur rôle logique.

Aucune interface ne doit nécessiter une adresse mémoire physique.

### ALGORITHMES UTILISÉS

Le traitement d'une demande respecte le cycle :

1. vérification des préconditions ;
2. exécution de la responsabilité du composant ;
3. production du résultat ;
4. mise à jour de l'état propriétaire ;
5. vérification des invariants ;
6. transmission du résultat.

### STATUT

Choix de conception.

### INVARIANTS

Une interface ne transfère pas la propriété d'un état sans le préciser.

Une opération échouée ne produit pas un résultat présenté comme valide.

Une erreur est propagée sans perdre sa cause principale.

### CONSÉQUENCES

La machine virtuelle peut être découpée en composants sans que cette séparation crée des dépendances implicites.

### TRAÇABILITÉ

HIS-3.11 → EXI-0001 → VM-0001, VM-2510.

EXI-0049 → VM-1601, VM-2508, VM-2510.

EXI-0052 → VM-2001, VM-2501, VM-2510.

---

### VM-2111
Profil d'implantation historique

### OBJET

Définir la représentation logique d'une variante d'implantation historique du L.S.E. lorsqu'une différence entre les systèmes documentés doit être conservée par l'architecture.

### RESPONSABILITÉS

Le profil :

- identifie l'implantation et la version concernées ;
- indique les commandes ou comportements explicitement différents ;
- ne redéfinit pas les mécanismes communs ;
- permet au dialogue et aux services concernés d'appliquer la variante documentée.

### EXIGENCES SATISFAITES

EXI-0061.

### CONTRAINTES

Les profils sont limités aux différences historiquement attestées. Le profil T1600 correspond à la version 4 documentée ; le profil MITRA 15 correspond à la version 8 documentée. [INRDP75 §« Remarque sur la présentation du fascicule »]

Exemples :

- RUBAN est disponible sur MITRA 15 ;
- SILENCE est disponible sur MITRA 15 ;
- le comportement de ENCODER vis-à-vis du programme courant diffère entre MITRA 15 et T1600. [INRDP75 §4.1 ; Annexe II]

### STATUT

Choix de conception.

### INVARIANTS

Un profil ne peut modifier une propriété commune sans justification historique explicite.

Une différence non documentée ne peut être ajoutée à un profil.

### TRAÇABILITÉ

INRDP75 « Remarque sur la présentation du fascicule », §4.1, Annexe II → EXI-0061 → VM-2111.


# 22. Contraintes de conformité

Le présent chapitre définit les contraintes permettant de vérifier qu'une architecture ou une implémentation demeure conforme à la reconstruction établie.

Une contrainte de conformité n'ajoute pas une fonctionnalité.

Elle interdit une organisation qui rendrait impossible une propriété normative déjà définie.

La conformité est appréciée à trois niveaux :

1. conformité historique ;
2. conformité fonctionnelle ;
3. conformité architecturale.

---

## 22.1 Conformité historique

### VM-2601
Préservation des faits historiques

### OBJET

Garantir que la machine virtuelle ne présente aucune déduction ou décision de conception comme un fait historique établi.

### RESPONSABILITÉS

Le composant documentaire doit :

- conserver la distinction entre fait, déduction, probabilité et choix de conception ;
- préserver les références aux sources ;
- empêcher qu'une représentation post-fixée reconstruite soit présentée comme une certitude historique générale.

### EXIGENCES SATISFAITES

EXI-0011.

EXI-0017.

### CONTRAINTES

Le catalogue impose une distinction explicite entre les catégories de statut. Il précise que la représentation post-fixée est une reconstruction probable et qu'il faut conserver la distinction documentaire entre la chaîne codée du L.S.E. et la chaîne post-fixée explicitement documentée pour le LSD.

### INTERFACES

La documentation de chaque composant doit pouvoir remonter vers :

- HIS ;
- EXI ;
- VM.

### DONNÉES MANIPULÉES

- référence historique ;
- exigence ;
- mécanisme ;
- statut.

### ALGORITHMES UTILISÉS

Aucun.

La conformité est contrôlée par inspection documentaire et vérification de traçabilité.

### STATUT

Documenté.

### INVARIANTS

Aucun choix de conception ne peut être cité comme preuve historique.

### CONSÉQUENCES

La documentation d'implémentation doit conserver les distinctions du Tome III.

### TRAÇABILITÉ

HIS-* → EXI-* → VM-2601.

---

## 22.2 Conformité fonctionnelle au Tome II

### VM-2602
Préservation de la machine abstraite

### OBJET

Garantir qu'une architecture de machine virtuelle réalise les mécanismes fonctionnels définis dans le Tome II sans en modifier les invariants.

### RESPONSABILITÉS

Toute architecture doit préserver :

- le programme courant constitué de représentations compilées ;
- l'état d'exécution distinct ;
- la compilation ligne par ligne ;
- l'interprétation directe de la représentation compilée ;
- la décompilation ;
- la séparation des fonctions ;
- la possibilité d'interruption et de reprise dans les cas prévus.

### EXIGENCES SATISFAITES

EXI-0001 à EXI-0052, selon les mécanismes concernés.

### CONTRAINTES

Le Tome II constitue la spécification fonctionnelle.

Aucun composant du Tome III ne peut modifier une décision du Tome II.

### INTERFACES

Toute interface du Tome III doit se conformer aux contrats fonctionnels du Tome II.

### DONNÉES MANIPULÉES

Les structures introduites au Tome III représentent les structures du Tome II sans leur ajouter une sémantique contradictoire.

### ALGORITHMES UTILISÉS

L'architecture doit implémenter un comportement équivalent au comportement abstrait.

Une optimisation n'est conforme que si elle préserve les mêmes effets observables.

### STATUT

Choix de conception.

### INVARIANTS

La conformité architecturale ne peut modifier la sémantique normative.

### CONSÉQUENCES

Une implémentation particulière peut choisir une organisation interne différente tant que les invariants sont préservés.

### TRAÇABILITÉ

Tome II → VM-2602.

EXI-0001 à EXI-0052 → VM-2602.

---

## 22.3 Indépendance vis-à-vis d'une technologie d'implémentation

### VM-2603
Indépendance technologique

### OBJET

Garantir que la spécification de la machine virtuelle demeure indépendante d'un processeur, d'un langage et d'un système d'exploitation.

### RESPONSABILITÉS

La spécification doit :

- définir des objets logiques ;
- définir des opérations abstraites ;
- définir des états ;
- définir des interfaces ;
- définir des invariants.

Elle ne doit pas :

- imposer une architecture matérielle ;
- imposer un langage source d'implémentation ;
- imposer un système d'exploitation ;
- imposer un format binaire.

### EXIGENCES SATISFAITES

EXI-0017.

EXI-0049.

### CONTRAINTES

Les sources historiques décrivent des implantations matérielles particulières, mais le Tome III doit en extraire une architecture logique indépendante. Le Tome I lui-même distingue les fonctions documentées de leur organisation interne qui n'est pas entièrement connue.

### INTERFACES

Les interfaces manipulent des objets abstraits.

### DONNÉES MANIPULÉES

Les données sont définies sémantiquement.

### ALGORITHMES UTILISÉS

Les algorithmes décrits sont abstraits.

### STATUT

Choix de conception.

### INVARIANTS

Aucune définition du Tome III ne dépend d'une adresse matérielle ou d'une primitive spécifique d'un système particulier.

### CONSÉQUENCES

Une implémentation peut être portée sur différentes organisations matérielles sans modifier l'architecture logique.

### TRAÇABILITÉ

Tome III — Conditionnement → VM-2603.

HIS-3.11 → EXI-0001 → VM-2603.

---

## 22.4 Atomicité des opérations

### VM-2604
Atomicité des transitions de programme

### OBJET

Garantir qu'une opération modifiant un état architectural ne laisse pas cet état dans une combinaison intermédiaire observable comme valide.

### RESPONSABILITÉS

Les opérations suivantes doivent être atomiques au niveau logique :

- acceptation d'une ligne compilée ;
- remplacement d'une ligne ;
- suppression d'une ligne ;
- remplacement du programme courant par chargement ;
- création d'une exécution ;
- terminaison d'une exécution ;
- création ou libération d'un objet déclaré.

### EXIGENCES SATISFAITES

EXI-0004.

EXI-0006.

EXI-0007.

EXI-0025.

EXI-0026.

EXI-0041.

EXI-0050.

### CONTRAINTES

Le Tome II fixe notamment le rejet atomique d'une ligne incorrecte et le remplacement logique du programme courant lors du chargement. 
### INTERFACES

Les interfaces concernées sont :

- VM-2503 ;
- VM-2507 ;
- VM-2508.

### DONNÉES MANIPULÉES

- programme courant ;
- objets compilés ;
- objets d'exécution ;
- état d'exécution.

### ALGORITHMES UTILISÉS

Une transition est préparée, validée, puis publiée.

En cas d'échec, l'état initial reste la valeur de référence.

### STATUT

Déduit.

### INVARIANTS

Une opération échouée ne publie pas d'état partiel.

### CONSÉQUENCES

Le principe d'atomicité devient une propriété globale de l'architecture.

### TRAÇABILITÉ

EXI-0004 → VM-0302, VM-1901, VM-2604.

EXI-0025 → VM-0606, VM-1702, VM-2507, VM-2604.

---

## 22.5 Cohérence du programme courant

### VM-2605
Invariants du programme courant

### OBJET

Garantir la cohérence permanente du programme courant.

### RESPONSABILITÉS

Garantir :

- unicité du numéro de ligne ;
- ordre logique ;
- représentation compilée valide ;
- absence de ligne partiellement compilée ;
- décompilabilité ;
- indépendance du texte source.

### EXIGENCES SATISFAITES

EXI-0003.

EXI-0005.

EXI-0006.

EXI-0008.

EXI-0022.

EXI-0026.

### CONTRAINTES

Les lignes correctes sont mémorisées et rangées selon l'ordre des numéros ; une ligne incorrecte n'est pas prise en compte. Le programme courant repose sur les représentations compilées. 
### INTERFACES

VM-2503 doit préserver ces invariants.

VM-2504 ne doit pas les modifier par l'exécution.

VM-2506 doit pouvoir les reconstruire après décompilation.

### DONNÉES MANIPULÉES

Collection des objets compilés.

### ALGORITHMES UTILISÉS

Contrôle :

1. numéro unique ;
2. représentation valide ;
3. ordre ;
4. cohérence.

### STATUT

Déduit.

### INVARIANTS

À tout instant où le programme courant est observable, il est cohérent selon les règles définies.

### CONSÉQUENCES

Le programme courant peut être utilisé simultanément par :

- le décompilateur ;
- l'interpréteur ;
- le bibliothécaire ;
- l'environnement conversationnel.

### TRAÇABILITÉ

HIS-6.2 → EXI-0001, EXI-0006, EXI-0008 → VM-2605.

HIS-4.5.1, HIS-4.5.2 → EXI-0002, EXI-0003 → VM-2605.

---

## 22.6 Cohérence de l'état d'exécution

### VM-2606
Invariants de l'état d'exécution

### OBJET

Garantir la cohérence d'une exécution active ou suspendue.

### RESPONSABILITÉS

Garantir la cohérence de :

- position d'exécution ;
- cadres d'activation ;
- pile d'évaluation ;
- objets accessibles ;
- registres virtuels.

### EXIGENCES SATISFAITES

EXI-0024.

EXI-0031.

EXI-0034.

EXI-0035.

EXI-0037.

EXI-0043.

EXI-0044.

### CONTRAINTES

L'état d'exécution est distinct du programme compilé et contient les informations nécessaires à la progression et à la reprise.

### INTERFACES

VM-2504.

VM-2505.

VM-2509.

### DONNÉES MANIPULÉES

État dynamique.

### ALGORITHMES UTILISÉS

Vérification avant publication de chaque transition.

### STATUT

Déduit.

### INVARIANTS

Une position d'exécution doit être interprétable.

Chaque activation active possède un contexte valide.

La pile d'évaluation correspond au point de traitement courant.

### CONSÉQUENCES

Une exécution suspendue peut être reprise sans reconstituer son état à partir du texte source.

### TRAÇABILITÉ

HIS-3.13 → EXI-0031, EXI-0033 → VM-2606.

HIS-13.1, HIS-13.2 → EXI-0034, EXI-0035, EXI-0043, EXI-0044 → VM-2606.

---

## 22.7 Traçabilité obligatoire

### VM-2607
Traçabilité des décisions

### OBJET

Garantir que toute décision d'architecture puisse être reliée à son origine.

### RESPONSABILITÉS

Chaque composant doit identifier :

- les références historiques ;
- les exigences satisfaites ;
- les mécanismes VM dépendants ;
- les interfaces concernées.

### EXIGENCES SATISFAITES

Toutes les exigences de traçabilité du Catalogue.

### CONTRAINTES

Le Catalogue définit la traçabilité comme bidirectionnelle : les exigences doivent pouvoir être reliées aux sources, à la conception, aux algorithmes et aux futures implémentations.

### INTERFACES

Toute référence doit utiliser un identifiant stable.

### DONNÉES MANIPULÉES

- HIS ;
- EXI ;
- VM ;
- CPF ;
- DEC ;
- ALG.

### ALGORITHMES UTILISÉS

Aucun.

### STATUT

Documenté.

### INVARIANTS

Aucun mécanisme normatif ne doit être dépourvu d'une origine ou d'une justification.

### CONSÉQUENCES

Toute extension future du Tome III doit enrichir la matrice de traçabilité.

### TRAÇABILITÉ

HIS-* ↔ EXI-* ↔ VM-*.

---

## 22.8 Neutralité des optimisations

### VM-2608
Préservation de la sémantique par optimisation

### OBJET

Définir la limite dans laquelle une optimisation d'implémentation demeure conforme à la machine virtuelle.

### RESPONSABILITÉS

Toute optimisation doit préserver :

- les résultats ;
- l'ordre observable ;
- les diagnostics requis ;
- l'état du programme courant ;
- l'état d'exécution ;
- les possibilités de reprise prévues.

### EXIGENCES SATISFAITES

EXI-0012.

EXI-0027.

EXI-0030.

EXI-0031.

EXI-0045.

EXI-0047.

### CONTRAINTES

La représentation post-fixée est une organisation reconstruite du traitement des expressions, mais l'architecture fonctionnelle repose sur sa sémantique et non sur une disposition matérielle particulière.

### INTERFACES

Les interfaces restent inchangées du point de vue abstrait.

### DONNÉES MANIPULÉES

Toute représentation interne optimisée doit être équivalente à la représentation normative du Tome II.

### ALGORITHMES UTILISÉS

Aucun algorithme d'optimisation n'est imposé.

### STATUT

Choix de conception.

### INVARIANTS

L'optimisation ne peut :

- changer le résultat ;
- supprimer un diagnostic obligatoire ;
- modifier le moment logique observable d'une erreur ;
- supprimer une possibilité de reprise normative.

### CONSÉQUENCES

Les choix techniques futurs restent subordonnés à la machine virtuelle.

### TRAÇABILITÉ

Tome II → VM-2608.

EXI-0012, EXI-0027, EXI-0030, EXI-0031 → VM-2608.

---

## 22.9 Conformité aux profils historiques

Une réalisation visant une implantation historique doit sélectionner un profil correspondant à l'implantation documentée.

Les propriétés communes du L.S.E. restent communes aux profils. Seules les différences explicitement documentées peuvent modifier le comportement correspondant.

**Traçabilité :** EXI-0061 → VM-2111.


# 23. Synthèse

Le Tome III décrit la machine virtuelle comme une architecture logique composée de composants spécialisés coopérant autour de trois états principaux :

- le programme courant ;
- l'état d'exécution ;
- l'état conversationnel.

À ces états s'ajoutent les représentations persistantes et les ressources de mémoire.

La machine virtuelle n'est donc pas organisée autour d'un unique objet central.

Elle est organisée autour d'un ensemble cohérent de responsabilités.

---

## 23.1 Architecture synthétique

### VM-2701
Architecture globale de la machine virtuelle

### OBJET

Décrire l'architecture logique complète obtenue par la composition des composants précédemment définis.

### RESPONSABILITÉS

L'architecture globale doit :

- recevoir les demandes ;
- compiler les lignes ;
- maintenir le programme courant ;
- exécuter les représentations compilées ;
- maintenir l'état d'exécution ;
- permettre la décompilation ;
- assurer la persistance ;
- gérer les objets d'exécution ;
- produire les diagnostics ;
- maintenir le dialogue.

### EXIGENCES SATISFAITES

EXI-0001 à EXI-0052.

### CONTRAINTES

Les fonctions historiquement distinguées demeurent séparées.

Le Tome I documente notamment le compilateur, le décompilateur, l'interpréteur, le bibliothécaire et l'analyseur de commandes. Le Tome II reconstruit leurs relations fonctionnelles dans une machine abstraite.

### INTERFACES

L'architecture globale est organisée comme suit :

```text
Utilisateur
    │
    ▼
Environnement conversationnel
    │
    ▼
Analyseur de demandes
    │
    ├──────────────► Compilateur
    │                    │
    │                    ▼
    │              Programme courant
    │                    │
    │                    ├────────► Décompilateur
    │                    │
    │                    └────────► Interpréteur
    │                                  │
    │                                  ▼
    │                         Environnement d'exécution
    │                                  │
    │                                  ├────► Mémoire de travail
    │                                  ├────► Entrées-sorties
    │                                  └────► Gestion des erreurs
    │
    └──────────────► Bibliothécaire
                         │
                         ▼
                 Stockage logique
```

Ce schéma constitue une reconstruction architecturale.

Il ne représente pas une organisation historique matérielle.

### DONNÉES MANIPULÉES

Les principaux objets sont :

- demande conversationnelle ;
- ligne validée ;
- objet compilé ;
- programme courant ;
- représentation persistante ;
- état d'exécution ;
- cadre d'activation ;
- objet variable ;
- tableau ;
- chaîne ;
- valeur ;
- diagnostic.

### ALGORITHMES UTILISÉS

L'architecture globale suit le cycle :

1. réception ;
2. classification ;
3. traitement ;
4. production du résultat ;
5. éventuelle transition d'état ;
6. restitution au dialogue.

### STATUT

Déduit.

### INVARIANTS

Aucun composant ne peut modifier directement un état dont il n'est pas responsable.

Le programme courant reste distinct de l'état d'exécution.

La persistance reste distincte de la mémoire de travail.

Les erreurs sont distinguées par leur phase de détection.

### CONSÉQUENCES

L'architecture obtenue est suffisamment complète pour constituer la base directe d'une implémentation sans imposer une technologie.

### TRAÇABILITÉ

HIS-3.11 → EXI-0001 → VM-2701.

HIS-3.13 → EXI-0031, EXI-0033 → VM-2701.

HIS-4.5.6 → EXI-0052 → VM-2701.

EXI-0001 à EXI-0052 → VM-2701.

---

## 23.2 Cycle complet de traitement

### VM-2702
Cycle architectural complet

### OBJET

Définir le déroulement logique de bout en bout d'une opération traitée par la machine virtuelle.

### RESPONSABILITÉS

Le cycle doit permettre de passer :

- de l'entrée utilisateur au programme courant ;
- du programme courant à l'exécution ;
- de l'exécution à la suspension ;
- de la suspension au dialogue ;
- du programme courant à la persistance ;
- de la persistance au programme courant.

### EXIGENCES SATISFAITES

EXI-0001.

EXI-0004.

EXI-0026.

EXI-0027.

EXI-0029.

EXI-0030.

EXI-0031.

EXI-0051.

EXI-0052.

### CONTRAINTES

Le traitement conversationnel décrit par le Tome II repose sur une orientation vers le mécanisme approprié, suivie d'une restitution du résultat ou du diagnostic.

### INTERFACES

#### Cas A — Ligne de programme correcte

```text
Utilisateur
→ VM-2501
→ VM-2502
→ VM-0501
→ VM-0502
→ VM-0504
→ VM-0302
→ VM-2503
→ Programme courant
```

#### Cas B — Ligne incorrecte

```text
Utilisateur
→ VM-2501
→ VM-2502
→ VM-0501
→ VM-0502
→ VM-1901
→ VM-2001
```

Le programme courant reste inchangé.

#### Cas C — Exécution programmée

```text
Programme courant
→ VM-2504
→ VM-2505
→ Interpréteur
→ Environnement d'exécution
→ Résultat / erreur / suspension
→ VM-2001
```

#### Cas D — Sauvegarde

```text
Programme courant
→ VM-2507
→ Stockage logique
```

#### Cas E — Chargement

```text
Stockage logique
→ VM-2507
→ validation
→ Programme courant
```

### DONNÉES MANIPULÉES

Les données passent d'un état à un autre selon les contrats d'interface.

### ALGORITHMES UTILISÉS

Les transitions sont déterministes du point de vue architectural.

### STATUT

Déduit.

### INVARIANTS

Une ligne rejetée ne modifie pas le programme courant.

Une exécution ne modifie pas implicitement la représentation compilée.

Une erreur de chargement ne remplace pas un programme courant valide.

### CONSÉQUENCES

La machine virtuelle est entièrement décrivable comme un ensemble de transitions entre états.

### TRAÇABILITÉ

EXI-0004 → VM-0302, VM-1901, VM-2702.

EXI-0025 → VM-0606, VM-1702, VM-2507, VM-2702.

EXI-0051 → VM-1901, VM-2702.

EXI-0052 → VM-2001, VM-2501, VM-2702.

---

## 23.3 Invariants globaux

### VM-2703
Invariants de la machine virtuelle

### OBJET

Établir l'ensemble minimal d'invariants qui doit être vrai à tout état architectural stable.

### RESPONSABILITÉS

Les invariants globaux couvrent :

- le programme courant ;
- l'exécution ;
- la mémoire ;
- la persistance ;
- les erreurs ;
- les interfaces.

### EXIGENCES SATISFAITES

EXI-0003.

EXI-0004.

EXI-0005.

EXI-0006.

EXI-0022.

EXI-0031.

EXI-0049.

EXI-0050.

### CONTRAINTES

Les exigences consolidées imposent notamment que le programme courant soit constitué de représentations compilées, que le texte source ne soit pas une dépendance obligatoire de l'exécution, et que les espaces logiques soient séparés de l'état temporaire d'exécution. 
### INTERFACES

Tous les composants doivent préserver les invariants lors de leurs transitions.

### DONNÉES MANIPULÉES

Les invariants portent sur les états, et non sur les formats.

### ALGORITHMES UTILISÉS

Vérification avant publication de toute transition.

### STATUT

Déduit.

### INVARIANTS

#### Invariant I-1 — Unicité

Un numéro de ligne correspond à au plus un objet compilé.

#### Invariant I-2 — Cohérence

Tout objet du programme courant est compilé et accepté.

#### Invariant I-3 — Ordre

Le programme courant possède un ordre logique déterminé par les numéros de ligne.

#### Invariant I-4 — Indépendance

L'exécution ne dépend pas de la conservation d'un texte source séparé.

#### Invariant I-5 — Séparation

Le programme courant et l'état d'exécution sont distincts.

#### Invariant I-6 — Persistance

Une représentation persistante ne constitue pas une mémoire d'exécution active.

#### Invariant I-7 — Erreur

Une erreur de compilation ne produit aucun objet accepté.

#### Invariant I-8 — Suspension

Une exécution suspendue conserve les informations nécessaires à la reprise lorsque celle-ci est autorisée.

#### Invariant I-9 — Diagnostic

Une erreur traitée possède un diagnostic identifiable.

#### Invariant I-10 — Traçabilité

Toute décision architecturale est rattachable à une exigence et, lorsque l'origine est historique, à une référence HIS.

### CONSÉQUENCES

Ces invariants forment la base normative des vérifications futures.

### TRAÇABILITÉ

EXI-0004, EXI-0005, EXI-0006 → VM-2703.

EXI-0022, EXI-0031, EXI-0049 → VM-2703.

---

## 23.4 Séparation entre architecture et implémentation

### VM-2704
Frontière de la spécification

### OBJET

Définir la limite entre ce qui relève du Tome III et ce qui relève d'une implémentation future.

### RESPONSABILITÉS

Le Tome III définit :

- les composants ;
- les responsabilités ;
- les interfaces ;
- les structures logiques ;
- les invariants ;
- les relations ;
- les contraintes.

Une implémentation future définit :

- les représentations concrètes ;
- les algorithmes d'allocation ;
- le format physique des objets ;
- les mécanismes d'accès ;
- le langage d'implémentation ;
- l'organisation du système hôte.

### EXIGENCES SATISFAITES

EXI-0017.

EXI-0049.

### CONTRAINTES

Aucune technologie moderne ne doit être introduite comme composant normatif du Tome III.

### INTERFACES

Le contrat abstrait reste stable entre les implémentations.

### DONNÉES MANIPULÉES

Les structures décrites sont logiques.

### ALGORITHMES UTILISÉS

Les algorithmes d'implémentation sont hors périmètre lorsqu'ils ne sont pas nécessaires à la définition du comportement architectural.

### STATUT

Choix de conception.

### INVARIANTS

Une implémentation ne peut supprimer une responsabilité normative sous prétexte qu'elle est réalisée autrement.

### CONSÉQUENCES

Le Tome III devient la spécification d'architecture logique.

Il ne constitue pas une spécification de code.

### TRAÇABILITÉ

Conditionnement du Tome III → VM-2704.

EXI-0017, EXI-0049 → VM-2704.

---

# Annexes


## Compléments de traçabilité

Les mécanismes ajoutés ou complétés pour les fichiers-données, le temps
partagé et la reprise sont reliés comme suit :

HIS-11.3, HIS-4.4.3 → EXI-0053, EXI-0054, EXI-0056 → VM-1703, VM-3405, VM-3407

HIS-4.4.4 → EXI-0053, EXI-0054 → VM-1704

HIS-11.1, HIS-11.3 → EXI-0055 → VM-1705, VM-3404, VM-2003

HIS-11.3.1 → EXI-0062 → VM-1709, VM-3404, VM-3405, VM-3408

HIS-4.5.4, HIS-11.1, HIS-11.3, HIS-11.5 → EXI-0057 → VM-1706, VM-1707, VM-2002, VM-2003, VM-3405, VM-3406

HIS-4.1, HIS-4.2, HIS-4.3, HIS-4.4.1 → EXI-0058, EXI-0059 → VM-2105

HIS-13.1, HIS-13.3, HIS-13.4 → EXI-0060 → VM-2106

# Annexe A — Matrice de traçabilité architecturale

## ANN-A-0001
Matrice globale HIS–EXI–VM

### OBJET

Fournir une vue condensée de la traçabilité entre les faits historiques, les exigences normatives et les composants de la machine virtuelle.

### RESPONSABILITÉS

La matrice permet :

- de vérifier qu'une propriété historique est représentée ;
- de vérifier qu'une exigence est utilisée ;
- de retrouver le composant architectural concerné ;
- d'identifier les zones nécessitant une justification supplémentaire.

### EXIGENCES SATISFAITES

Toutes les exigences utilisées dans le Tome III.

### CONTRAINTES

La traçabilité est bidirectionnelle.

Toute référence doit utiliser des identifiants stables.

### INTERFACES

La matrice relie :

HIS ↔ EXI ↔ VM.

### DONNÉES MANIPULÉES

Chaque entrée possède :

- origine historique ;
- exigence ;
- mécanisme architectural ;
- statut ;
- observation éventuelle.

### ALGORITHMES UTILISÉS

Aucun.

### STATUT

Choix de conception.

### INVARIANTS

Une relation de traçabilité doit rester cohérente avec le texte du composant référencé.

Une exigence ne doit pas être associée à un mécanisme qui ne l'utilise pas effectivement.

### CONSÉQUENCES

La matrice permet la vérification croisée des trois niveaux documentaires.

### TRAÇABILITÉ

Les lignes principales sont les suivantes.

| Référence historique | Exigences principales | Composants architecturaux |
|---|---|---|
| HIS-3.1 | EXI-0052 | VM-2001, VM-2501 |
| HIS-3.11 | EXI-0001, EXI-0052 | VM-0001, VM-1903, VM-2502, VM-2701 |
| HIS-3.13 | EXI-0024, EXI-0031, EXI-0033 | VM-0604, VM-0801, VM-0803, VM-1902, VM-1904, VM-1905 |
| HIS-3.14 | EXI-0049 | VM-1601, VM-2507, VM-2508, VM-2701 |
| HIS-4.5.1 | EXI-0002 | VM-0301, VM-0404 |
| HIS-4.5.2 | EXI-0003 | VM-0301, VM-0405, VM-1801, VM-2506 |
| HIS-4.5.6 | EXI-0052 | VM-2001, VM-2502 |
| HIS-5.2 | EXI-0018, EXI-0019, EXI-0051 | VM-0502, VM-0503, VM-1901, VM-1902 |
| HIS-6.2 | EXI-0001, EXI-0006, EXI-0008 | VM-0303, VM-2503, VM-2605 |
| HIS-6.5 | EXI-0004, EXI-0020, EXI-0021, EXI-0026, EXI-0051 | VM-0302, VM-0504, VM-0506, VM-0505, VM-0607, VM-1901, VM-2503 |
| HIS-8.2 | EXI-0008, EXI-0023, EXI-0029, EXI-0030 | VM-0603, VM-0703, VM-0705, VM-1904, VM-2504 |
| HIS-8.3 | EXI-0024, EXI-0031, EXI-0033 | VM-0604, VM-0801, VM-0803, VM-1904, VM-1905 |
| HIS-10.1 | EXI-0032, EXI-0038 | VM-0802, VM-1101, VM-2505 |
| HIS-10.6 | EXI-0040, EXI-0041, EXI-0050 | VM-1201, VM-1202, VM-1203, VM-1602, VM-2505, VM-2508 |
| HIS-11.2 | EXI-0009, EXI-0022 | VM-1701, VM-2507 |
| HIS-11.5 | EXI-0009, EXI-0021, EXI-0025 | VM-0506, VM-0606, VM-1702, VM-2507 |
| HIS-12.1 | EXI-0019, EXI-0051 | VM-0503, VM-1901, VM-1902 |
| HIS-12.5 | EXI-0024 | VM-0604, VM-1905 |
| HIS-13.1 | EXI-0032, EXI-0034, EXI-0035, EXI-0042, EXI-0043 | VM-0802, VM-0901, VM-0902, VM-1301, VM-1905 |
| HIS-13.2 | EXI-0036, EXI-0042, EXI-0044 | VM-0903, VM-1301, VM-1303 |
| HIS-14.1 | EXI-0030, EXI-0046 | VM-0705, VM-1402, VM-1904 |
| CPF-7.1 | EXI-0011, EXI-0017, EXI-0047 | VM-0402, VM-0408, VM-1501, VM-2601 |
| CPF-7.2 | EXI-0011, EXI-0012, EXI-0037, EXI-0047 | VM-0402, VM-0403, VM-1001, VM-1501 |
| CPF-7.3 | EXI-0002, EXI-0003, EXI-0013, EXI-0015 | VM-0404, VM-0405, VM-0406, VM-1801 |
| CPF-7.4 | EXI-0017 | VM-0408, VM-0409, VM-2601 |

Cette matrice constitue une synthèse architecturale.

Elle ne remplace pas les références détaillées placées dans les composants.

---

# Annexe B — Contrats d'état

## ANN-B-0001
États stables de la machine virtuelle

### OBJET

Définir les états stables dans lesquels la machine virtuelle peut être observée.

### RESPONSABILITÉS

Les états principaux sont :

- CONVERSATION ;
- COMPILATION ;
- EXECUTION ;
- SUSPENSION ;
- ERREUR ;
- TERMINAISON.

### EXIGENCES SATISFAITES

EXI-0030.

EXI-0031.

EXI-0051.

EXI-0052.

### CONTRAINTES

La distinction entre dialogue, exécution et état d'exécution est imposée par les mécanismes reconstruits.

### INTERFACES

Les transitions principales sont :

```text
CONVERSATION
    │
    ├── ligne de programme ──► COMPILATION
    │                             │
    │                             ├── succès ──► CONVERSATION
    │                             └── erreur ──► ERREUR
    │
    ├── expression immédiate ──► EXECUTION
    │
    ├── EXECUTER ──► EXECUTION
    │
    └── commande ──► TRAITEMENT DE COMMANDE
```

Puis :

```text
EXECUTION
    │
    ├── fin naturelle ──► TERMINAISON
    ├── terminaison explicite ──► TERMINAISON
    ├── interruption ──► SUSPENSION
    └── erreur ──► ERREUR
```

Enfin :

```text
SUSPENSION
    ├── CONTINUER ──► EXECUTION
    ├── REPRENDRE ──► EXECUTION
    └── fin ──► TERMINAISON
```

### DONNÉES MANIPULÉES

Chaque état possède :

- un état propre ;
- un contexte ;
- des transitions autorisées.

### ALGORITHMES UTILISÉS

Machine à états abstraite.

### STATUT

Déduit.

### INVARIANTS

Une seule transition est publiée à la fois pour un contexte d'exécution donné.

Une exécution en erreur ne reprend pas automatiquement.

Une fin d'exécution ne supprime pas le programme courant.

### CONSÉQUENCES

La machine virtuelle peut être décrite comme un ensemble fini de classes d'état, même si les structures internes sont plus riches.

### TRAÇABILITÉ

HIS-3.13 → EXI-0031, EXI-0033 → ANN-B-0001.

HIS-8.2 → EXI-0030 → ANN-B-0001.

HIS-12.5 → EXI-0024 → ANN-B-0001.

---

# Annexe C — Règles de conformité d'une future implémentation

## ANN-C-0001
Vérification de conformité

### OBJET

Définir les propriétés qu'une implémentation devra démontrer pour être considérée comme conforme à l'architecture du Tome III.

### RESPONSABILITÉS

Une implémentation conforme doit démontrer que :

1. toute ligne acceptée possède une représentation compilée ;
2. toute ligne rejetée laisse inchangé le programme courant ;
3. les numéros de lignes sont uniques ;
4. l'ordre logique est conservé ;
5. l'exécution utilise la représentation compilée ;
6. le texte source n'est pas nécessaire à l'exécution ;
7. le programme courant est distinct de l'état d'exécution ;
8. une erreur syntaxique est traitée avant l'acceptation de la ligne ;
9. une erreur d'exécution n'est pas confondue avec une erreur de compilation ;
10. une exécution suspendue conserve l'état nécessaire à une reprise autorisée ;
11. la persistance et la mémoire d'exécution restent distinctes ;
12. les interfaces définies dans le Tome III sont respectées au niveau logique.

### EXIGENCES SATISFAITES

EXI-0001 à EXI-0052.

### CONTRAINTES

Une implémentation peut différer de la reconstruction dans son organisation interne uniquement si elle conserve les propriétés observables et les invariants du Tome II et du Tome III.

### INTERFACES

Les interfaces vérifiées sont celles définies par VM-2501 à VM-2510.

### DONNÉES MANIPULÉES

- états ;
- objets ;
- transitions ;
- diagnostics ;
- représentations compilées.

### ALGORITHMES UTILISÉS

Les méthodes de vérification ne sont pas imposées par le présent tome.

### STATUT

Choix de conception.

### INVARIANTS

La conformité est évaluée sur les propriétés architecturales.

La conformité ne dépend pas d'une identité de code ou de structure interne.

### CONSÉQUENCES

Le Tome III peut servir de référence pour l'évaluation d'une réalisation sans imposer une implémentation unique.

### TRAÇABILITÉ

VM-2602 → ANN-C-0001.

VM-2603 → ANN-C-0001.

VM-2605 → ANN-C-0001.

VM-2607 → ANN-C-0001.

---


## Compléments

Les composants ajoutés dans le présent tome comprennent :

- le catalogue logique des fichiers ;
- le gestionnaire des fichiers-données ;
- le transformateur programme-donnée ;
- le catalogueur de fichiers-données ;
- l'orienteur des commandes de fichiers-données ;
- le gestionnaire des tâches en temps partagé ;
- le gestionnaire de reprise.

# Annexe D — Synthèse des responsabilités des composants

## ANN-D-0001
Répartition fonctionnelle

### OBJET

Présenter la répartition finale des responsabilités entre les composants de la machine virtuelle.

### RESPONSABILITÉS

| Composant | Responsabilité principale |
|---|---|
| VM-0501 | Validation d'une ligne |
| VM-0502 | Analyse syntaxique |
| VM-0503 | Séparation syntaxe / erreurs différées |
| VM-0504 | Génération de la représentation compilée |
| VM-0505 | Acceptation de l'objet compilé |
| VM-0601 | Gestion du programme courant |
| VM-0602 | Validation du programme |
| VM-0603 | Détermination du point initial |
| VM-0604 | Position d'exécution |
| VM-0605 | Indépendance vis-à-vis du texte source |
| VM-0606 | Remplacement du programme courant |
| VM-0607 | Cohérence compilation/exécution |
| VM-0701 | Interprétation |
| VM-0702 | Exécution immédiate |
| VM-0703 | Exécution programmée |
| VM-0704 | Parcours post-fixé |
| VM-0705 | Terminaison |
| VM-0801 | État d'exécution |
| VM-0802 | Résolution des références |
| VM-0803 | Observation de l'état |
| VM-0901 à VM-0903 | Activations |
| VM-1001 et VM-1002 | Évaluation |
| VM-1101 à VM-1103 | Variables et références |
| VM-1201 à VM-1203 | Tableaux |
| VM-1301 à VM-1303 | Procédures et fonctions |
| VM-1401 et suivants | Contrôle du flux |
| VM-1501 à VM-1502 | Expressions et valeurs |
| VM-1601 et VM-1602 | Espaces mémoire et gestion des objets |
| VM-1701 et VM-1702 | Persistance et restauration |
| VM-1801 et VM-1802 | Décompilation |
| VM-1901 | Diagnostic de compilation |
| VM-1902 | Diagnostic d'exécution |
| VM-1903 | Classification des erreurs |
| VM-1904 | État d'erreur |
| VM-1905 | Interruption d'exécution |
| VM-2001 | Cycle conversationnel |
| VM-2501 à VM-2510 | Interfaces intercomposants |
| VM-2601 à VM-2608 | Contraintes de conformité |
| VM-2701 à VM-2704 | Synthèse architecturale |

### EXIGENCES SATISFAITES

EXI-0001 à EXI-0052.

### CONTRAINTES

La répartition fonctionnelle conserve la séparation des processeurs historiquement documentée sans prétendre reproduire leur organisation matérielle exacte.

### INTERFACES

Les relations entre composants sont définies par VM-2501 à VM-2510.

### DONNÉES MANIPULÉES

Les composants partagent uniquement les états et objets que leurs contrats d'interface autorisent.

### ALGORITHMES UTILISÉS

Aucun algorithme particulier n'est imposé par cette synthèse.

### STATUT

Déduit.

### INVARIANTS

La responsabilité d'un composant ne doit pas être dupliquée de manière contradictoire dans un autre composant.

Les responsabilités peuvent être regroupées dans une implantation, mais leurs contrats logiques demeurent distincts.

### CONSÉQUENCES

La machine virtuelle est décomposable en composants cohérents tout en restant une machine logique unique.

### TRAÇABILITÉ

HIS-3.11 → EXI-0001 → VM-0001, VM-2502, ANN-D-0001.

HIS-3.13 → EXI-0031, EXI-0033 → VM-0801, VM-0803, VM-1904, VM-2001, ANN-D-0001.

---

# Annexe E — Principe directeur de conformité

## ANN-E-0001
Principe de conservation architecturale

### OBJET

Formuler le principe général qui gouverne toute extension ultérieure du Tome III.

### RESPONSABILITÉS

Toute modification future doit démontrer simultanément :

- qu'elle est compatible avec le Tome I ;
- qu'elle satisfait les exigences applicables ;
- qu'elle ne contredit pas le Tome II ;
- qu'elle s'insère dans les interfaces du Tome III ;
- qu'elle conserve les invariants globaux.

### EXIGENCES SATISFAITES

EXI-0001 à EXI-0052.

### CONTRAINTES

Aucune décision de niveau inférieur ne peut contredire une décision normative de niveau supérieur.

Les nouveaux composants ne doivent être introduits que lorsqu'une responsabilité architecturale n'est pas correctement couverte par un composant existant.

### INTERFACES

Toute extension doit fournir ses relations avec les interfaces existantes.

### DONNÉES MANIPULÉES

Les nouvelles structures doivent être justifiées par une responsabilité normative.

### ALGORITHMES UTILISÉS

Aucun.

### STATUT

Choix de conception.

### INVARIANTS

La hiérarchie normative reste :

1. Conventions éditoriales ;
2. Catalogue des exigences ;
3. Tome I ;
4. Tome II ;
5. Tome III.

Une décision de Tome III ne peut modifier rétroactivement une propriété historique ou fonctionnelle déjà établie.

### CONSÉQUENCES

Toute poursuite de la rédaction doit compléter la spécification sans réviser les mécanismes déjà définis.

### TRAÇABILITÉ

Conventions éditoriales → VM-2601, VM-2603, VM-2607.

Catalogue des exigences → VM-2602, VM-2605, VM-2607.

Tome I → contraintes historiques.

Tome II → comportement fonctionnel.

Tome III → architecture logique.

---

# Annexe F — Registre des interfaces architecturales

## ANN-F-0001
Registre des interfaces de la machine virtuelle

### OBJET

Le présent registre définit les interfaces logiques nécessaires aux échanges entre les composants de la machine virtuelle.

Il complète les interfaces définies dans le corps du Tome III sans modifier les responsabilités attribuées aux composants.

Il constitue une vue contractuelle des échanges entre composants.

Les interfaces définies ici sont logiques. Elles ne déterminent :

- aucun langage d'implémentation ;
- aucune représentation matérielle ;
- aucun protocole de communication particulier ;
- aucune organisation physique de la mémoire ;
- aucune technologie de stockage.

### RESPONSABILITÉS

Le registre doit :

- identifier chaque interface intercomposant ;
- préciser son initiateur ;
- préciser son destinataire ;
- identifier les données échangées ;
- préciser le résultat attendu ;
- préserver les responsabilités définies dans le Tome III ;
- permettre la vérification de la conformité architecturale.

### EXIGENCES SATISFAITES

EXI-0001 à EXI-0062, selon les interfaces concernées.

En particulier :

- EXI-0002 — Représentation compilée exécutable ;
- EXI-0003 — Décompilabilité de la représentation compilée ;
- EXI-0004 — Rejet atomique d'une ligne incorrecte ;
- EXI-0005 — Programme courant constitué de représentations compilées ;
- EXI-0052 — Traitement conversationnel des demandes ;
- EXI-0053 — Distinction des fichiers-données temporaires et permanents ;
- EXI-0054 — Accès des fichiers-données selon leur portée ;
- EXI-0055 — Catalogue des fichiers ;
- EXI-0056 — Conservation et réutilisation de l'espace des fichiers temporaires ;
- EXI-0057 — Traitement des commandes portant sur les fichiers-données ;
- EXI-0058 — Gestion abstraite des tâches en temps partagé ;
- EXI-0059 — Moniteur de temps partagé et permutation des tâches ;
- EXI-0060 — Reprise d'une exécution conservant les données de l'interruption.
- EXI-0061 — Profils des implantations historiques.
- EXI-0062 — Gestion des identifications et des droits d'accès aux fichiers.

Les exigences EXI-0053 à EXI-0060 sont déjà établies dans le Tome II et ne sont pas redéfinies par la présente annexe.

### CONTRAINTES

Une interface ne doit pas transférer à un composant une responsabilité qui appartient à un autre composant.

Une interface ne doit pas introduire une dépendance envers le texte source lorsque le Tome II définit le fonctionnement à partir de la représentation compilée.

Une interface de persistance ne doit pas confondre :

- programme courant ;
- fichier-programme ;
- fichier-données temporaire ;
- fichier-données permanent ;
- état d'une tâche ;
- état d'une exécution.

Une interface de temps partagé ne doit pas être assimilée à une interface de fichier.

Une interface de reprise doit conserver la distinction entre :

- l'état du programme courant ;
- l'état de l'exécution ;
- l'état de la tâche.

### INTERFACES

Les interfaces sont classées par domaine fonctionnel.

---

## F.1 Interface de compilation

### ANN-F-0002
Interface de compilation d'une ligne

### OBJET

Définir l'échange entre l'environnement conversationnel et le compilateur lorsqu'une ligne de programme est soumise à compilation.

### RESPONSABILITÉS

L'environnement conversationnel remet au compilateur une ligne complète.

Le compilateur :

- analyse la ligne ;
- produit une représentation logique ;
- produit une représentation compilée si la ligne est valide ;
- produit un diagnostic si la ligne est invalide.

Le programme courant n'est modifié qu'après acceptation de la représentation compilée.

Cette organisation conserve le principe historique de compilation ligne par ligne et de validation immédiate de la ligne. [INRDP75 §3.3] [LSE15 §1]

### EXIGENCES SATISFAITES

EXI-0001.

EXI-0002.

EXI-0004.

EXI-0018.

EXI-0051.

### CONTRAINTES

Une erreur de compilation ne doit pas produire d'objet accepté.

Une erreur syntaxique ne doit pas modifier le programme courant.

### INTERFACES

`COMPILER(ligne)`

Entrée :

- ligne de programme.

Sortie :

- objet compilé accepté ;
- ou diagnostic.

### DONNÉES MANIPULÉES

- numéro de ligne ;
- constructions syntaxiques ;
- représentation compilée ;
- diagnostic.

### ALGORITHMES UTILISÉS

L'interface ne prescrit pas l'algorithme d'analyse.

Elle impose uniquement le résultat contractuel.

### INVARIANTS

Une ligne acceptée produit un objet compilé.

Une ligne rejetée ne modifie pas le programme courant.

### CONSÉQUENCES

Le gestionnaire du programme courant peut traiter séparément chaque résultat de compilation.

### TRAÇABILITÉ

HIS-6.2 → EXI-0001, EXI-0004 → VM-0501 à VM-0505 → ANN-F-0002.

---

## F.2 Interface d'intégration au programme courant

### ANN-F-0003
Interface d'acceptation d'un objet compilé

### OBJET

Définir l'échange entre le compilateur et le gestionnaire du programme courant.

### RESPONSABILITÉS

Le compilateur fournit un objet compilé validé.

Le gestionnaire du programme courant :

- identifie la ligne ;
- recherche éventuellement une ligne existante portant le même numéro ;
- remplace celle-ci lorsque cela est nécessaire ;
- maintient l'ordre logique du programme.

Les sources historiques établissent qu'une ligne nouvelle portant le même numéro remplace la ligne précédente et que les lignes sont replacées selon l'ordre croissant de leur numéro. [LSE15 §1]

### EXIGENCES SATISFAITES

EXI-0005.

EXI-0006.

EXI-0007.

EXI-0008.

### INTERFACES

`ACCEPTER(objet_compilé)`

`REMPLACER(numero, objet_compilé)`

`SUPPRIMER(numero)`

`OBTENIR(numero)`

`PARCOURIR_ORDRE_LOGIQUE()`

### DONNÉES MANIPULÉES

- objets compilés ;
- numéros de ligne ;
- ordre logique.

### ALGORITHMES UTILISÉS

La méthode d'organisation interne n'est pas imposée.

Le résultat doit respecter l'unicité et l'ordre logique.

### INVARIANTS

Deux objets compilés du programme courant ne portent jamais le même numéro logique.

Le remplacement d'une ligne ne modifie pas les autres lignes.

### CONSÉQUENCES

Le programme courant reste indépendant de l'ordre dans lequel les lignes ont été introduites.

### TRAÇABILITÉ

HIS-6.2 → EXI-0006, EXI-0007, EXI-0008 → VM-0601 à VM-0606 → ANN-F-0003.

---

## F.3 Interface d'exécution

### ANN-F-0004
Interface de lancement de l'exécution

### OBJET

Définir l'échange entre le moniteur, le gestionnaire du programme courant et le moteur d'exécution.

### RESPONSABILITÉS

Le moniteur demande l'exécution du programme courant.

Le gestionnaire du programme courant fournit une vue ordonnée des objets compilés.

Le moteur d'exécution crée l'état d'exécution et initialise sa position.

### EXIGENCES SATISFAITES

EXI-0002.

EXI-0008.

EXI-0012.

EXI-0029.

EXI-0030.

### INTERFACES

`CREER_EXECUTION(programme_courant)`

`INITIALISER_POSITION(execution, position_initiale)`

`EXECUTER(execution)`

`TERMINER(execution)`

### DONNÉES MANIPULÉES

- programme courant ;
- objets compilés ;
- position d'exécution ;
- état d'exécution.

### CONTRAINTES

Le moteur d'exécution ne doit pas nécessiter le texte source.

La fin de l'exécution ne doit pas supprimer le programme courant.

### INVARIANTS

L'état d'exécution est distinct du programme courant.

Une exécution terminée ne demeure pas active.

Le programme courant peut subsister après la terminaison.

### TRAÇABILITÉ

HIS-8.2, HIS-8.3 → EXI-0029, EXI-0030, EXI-0031 → VM-0603, VM-0604, VM-0701 à VM-0705 → ANN-F-0004.

---

## F.4 Interface de résolution des références

### ANN-F-0005
Interface de résolution d'une référence

### OBJET

Définir l'échange entre la représentation compilée et l'environnement d'exécution lorsqu'une référence doit être résolue.

### RESPONSABILITÉS

La représentation compilée fournit une référence logique.

L'environnement d'exécution fournit le contexte nécessaire.

Le gestionnaire des variables, des tableaux, des procédures ou des fonctions fournit l'objet correspondant lorsque celui-ci est accessible.

### EXIGENCES SATISFAITES

EXI-0014.

EXI-0032.

EXI-0038.

EXI-0042.

### INTERFACES

`RESOUDRE(reference_compilee, environnement)`

Résultat :

- référence résolue ;
- ou diagnostic.

`ACCEDER(reference_resolue)`

Résultat :

- objet accessible ;
- ou diagnostic.

### DONNÉES MANIPULÉES

- identificateur ;
- référence compilée ;
- contexte ;
- référence résolue ;
- objet.

### CONTRAINTES

Une référence compilée ne doit pas dépendre d'une adresse physique.

Une référence résolue est dépendante du contexte d'exécution.

### INVARIANTS

Une référence non résolue ne peut être utilisée comme référence valide.

La résolution ne modifie pas le programme compilé.

### TRAÇABILITÉ

HIS-10.1, HIS-10.2 → EXI-0014, EXI-0032, EXI-0038 → VM-0802, VM-1101 → ANN-F-0005.

---

## F.5 Interface d'évaluation

### ANN-F-0006
Interface de la pile d'évaluation

### OBJET

Définir l'échange entre l'interpréteur, le gestionnaire des expressions et la pile d'évaluation.

### RESPONSABILITÉS

L'interpréteur demande :

- l'empilement d'une valeur ;
- le retrait d'une valeur ;
- l'application d'une opération ;
- la production d'une valeur résultante.

### EXIGENCES SATISFAITES

EXI-0011.

EXI-0012.

EXI-0037.

EXI-0047.

EXI-0048.

### INTERFACES

`EMPILER(valeur)`

`DEPILER()`

`APPLIQUER(operation)`

`LIRE_SOMMET()`

### DONNÉES MANIPULÉES

- valeurs ;
- opérations ;
- résultats intermédiaires.

### CONTRAINTES

L'interface ne doit pas imposer une structure matérielle particulière.

### INVARIANTS

Une opération ne consomme que les valeurs nécessaires à sa définition.

Une valeur résultante peut devenir opérande d'une opération ultérieure.

### TRAÇABILITÉ

CPF-7.1, CPF-7.2 → EXI-0011, EXI-0012, EXI-0047 → VM-1001, VM-1501, VM-1502 → ANN-F-0006.

---

## F.6 Interface des cadres d'activation

### ANN-F-0007
Interface d'activation

### OBJET

Définir l'échange entre le moteur d'exécution et le gestionnaire des cadres d'activation.

### RESPONSABILITÉS

Le gestionnaire :

- crée un cadre ;
- initialise ses informations ;
- conserve le contexte d'appel ;
- restitue le cadre appelant ;
- détruit le cadre lorsqu'il n'est plus nécessaire.

### EXIGENCES SATISFAITES

EXI-0034.

EXI-0035.

EXI-0036.

EXI-0042.

EXI-0043.

EXI-0044.

### INTERFACES

`CREER_CADRE(appel)`

`ACTIVER_CADRE(cadre)`

`OBTENIR_CADRE_COURANT()`

`RETOUR_CADRE()`

`DETRUIRE_CADRE(cadre)`

### DONNÉES MANIPULÉES

- cadre d'activation ;
- paramètres ;
- variables locales ;
- adresse logique de retour ;
- contexte d'appel.

### INVARIANTS

Le cadre courant correspond à l'activation courante.

Un retour retire le cadre correspondant à l'activation terminée.

### TRAÇABILITÉ

HIS-13.1, HIS-13.2 → EXI-0034 à EXI-0036 → VM-0901 à VM-0903 → ANN-F-0007.

---

## F.7 Interface du contrôle du flux

### ANN-F-0008
Interface de transfert d'exécution

### OBJET

Définir l'échange entre l'interpréteur et le gestionnaire du contrôle du flux.

### RESPONSABILITÉS

Le gestionnaire doit pouvoir demander :

- la poursuite séquentielle ;
- un transfert vers une ligne ;
- l'entrée dans une structure de contrôle ;
- la sortie d'une structure ;
- la terminaison.

### EXIGENCES SATISFAITES

EXI-0008.

EXI-0023.

EXI-0029.

EXI-0030.

EXI-0045.

EXI-0046.

### INTERFACES

`SUIVRE_POSITION(execution)`

`TRANSFERER(execution, destination)`

`TERMINER_EXECUTION(execution)`

`DETERMINER_SUIVANTE(execution)`

### DONNÉES MANIPULÉES

- position ;
- numéro de ligne ;
- destination ;
- état du contrôle.

### CONTRAINTES

Le transfert ne doit pas modifier le programme courant.

Une destination inexistante doit produire un diagnostic conforme au mécanisme d'erreur.

### INVARIANTS

La position d'exécution désigne un emplacement logique du programme courant.

### TRAÇABILITÉ

HIS-8.2 → EXI-0008, EXI-0023, EXI-0029, EXI-0030 → VM-0604, VM-1401, VM-1402 → ANN-F-0008.

---

# F.8 Interface de persistance

### ANN-F-0009
Interface du bibliothécaire

### OBJET

Définir l'échange entre les commandes ou instructions portant sur les fichiers et le bibliothécaire.

Le bibliothécaire est historiquement le processeur chargé de la gestion des fichiers utilisateurs. Il intervient pour les fichiers-programmes et pour les fichiers-données. [INRDP75 §3.3] [INRDP75 §Fichiers]

### RESPONSABILITÉS

Le bibliothécaire doit :

- rechercher un fichier ;
- créer un fichier lorsque l'opération l'autorise ;
- lire un fichier ;
- écrire un fichier ;
- supprimer un fichier ;
- cataloguer un fichier-données temporaire ;
- conserver un fichier-programme ;
- restituer un fichier-programme ;
- distinguer les fichiers selon leur portée.

### EXIGENCES SATISFAITES

EXI-0009.

EXI-0022.

EXI-0025.

EXI-0053.

EXI-0054.

EXI-0055.

EXI-0056.

EXI-0057.

### INTERFACES

`RECHERCHER_FICHIER(identification, contexte)`

`LIRE_FICHIER(fichier)`

`ECRIRE_FICHIER(fichier, contenu)`

`SUPPRIMER_FICHIER(fichier)`

`CATALOGUER(fichier_temporaire)`

`RANGER_PROGRAMME(programme, identification)`

`APPELER_PROGRAMME(identification)`

### DONNÉES MANIPULÉES

- identification de fichier ;
- portée ;
- contenu ;
- fichier-programme ;
- fichier-données temporaire ;
- fichier-données permanent ;
- catalogue.

### CONTRAINTES

Le bibliothécaire ne doit pas confondre la persistance d'un programme avec l'état d'une exécution.

Un fichier-données temporaire reste attaché à la console à laquelle il appartient.

Un fichier-données permanent appartient à la bibliothèque.

Les sources historiques distinguent explicitement ces deux catégories et précisent que les fichiers temporaires sont détruits avec la session. [INRDP75 §1-1–§1-3] [LSE15 §9-5]

### INVARIANTS

Un fichier temporaire ne devient permanent que par l'opération prévue à cet effet.

La destruction d'un fichier temporaire libère son espace logique.

Le chargement d'un fichier-programme produit un programme courant distinct du fichier conservé.

### CONSÉQUENCES

Les commandes de fichiers peuvent rester indépendantes de la représentation interne du stockage.

### TRAÇABILITÉ

HIS-11.1, HIS-11.3, HIS-11.5 → EXI-0053 à EXI-0057 → VM-1703 à VM-1708 → ANN-F-0009.

---

# F.9 Interface des fichiers-données

### ANN-F-0010
Interface de manipulation des fichiers-données

### OBJET

Définir l'interface particulière utilisée lorsqu'un programme ou une commande manipule un fichier-données.

### RESPONSABILITÉS

L'interface permet :

- d'identifier un fichier-données ;
- d'obtenir le contenu d'un fichier ou d'un enregistrement ;
- de modifier le contenu d'un fichier ou d'un enregistrement lorsque l'opération l'autorise ;
- de distinguer sa portée ;
- de rendre permanent un fichier temporaire ;
- de supprimer un fichier.

### EXIGENCES SATISFAITES

EXI-0053.

EXI-0054.

EXI-0056.

EXI-0057.

### INTERFACES

`OUVRIR_DONNEES(identification, contexte)`

`LIRE_DONNEES(fichier)`

`ECRIRE_DONNEES(fichier, donnees)`

`GARER(fichier, numéro_enregistrement, valeur)`

`CHARGER(fichier, numéro_enregistrement)`

`SUPPRIMER(fichier, numéro_enregistrement)`

`CATALOGUER_DONNEES(fichier_temporaire)`

`SUPPRIMER_DONNEES(fichier)`

### DONNÉES MANIPULÉES

- fichier-données ;
- contenu ;
- nom logique ;
- portée ;
- état de conservation ;
- numéro d'enregistrement ;
- type et contenu d'un enregistrement.

### CONTRAINTES

Un fichier temporaire appartenant à une autre console ne peut pas être directement utilisé comme fichier temporaire de la console courante.

Un fichier permanent est distingué du fichier temporaire par sa durée de conservation et sa portée.

Ces propriétés sont directement établies dans le Tome I.

### ALGORITHMES UTILISÉS

Aucun algorithme de stockage n'est imposé.

### INVARIANTS

L'identité logique d'un fichier est conservée pendant une opération de lecture ou d'écriture.

Un enregistrement est une unité logique identifiée dans le fichier.

GARER remplace le contenu d'un enregistrement existant de même numéro.

CHARGER restitue le type et le contenu de l'enregistrement demandé.

SUPPRIMER porte sur l'enregistrement désigné ou, en l'absence de désignation d'enregistrement, sur le fichier.

La transformation d'un fichier temporaire en fichier permanent ne produit pas deux fichiers permanents identiques.

### CONSÉQUENCES

La gestion des fichiers-données peut être réalisée sans dépendre de l'organisation physique de la zone de stockage.

### TRAÇABILITÉ

HIS-11.3, HIS-4.4.3, HIS-4.4.4 → EXI-0053, EXI-0054, EXI-0056 → VM-1703, VM-1704, VM-1708 → ANN-F-0010.

---

# F.10 Interface d'encodage

### ANN-F-0011
Interface ENCODER

### OBJET

Définir l'échange entre le bibliothécaire, le fichier-données et le compilateur lors de la transformation d'une représentation textuelle de programme en programme courant.

### RESPONSABILITÉS

Le bibliothécaire fournit les données du fichier.

Le composant d'encodage remet les lignes au compilateur.

Le compilateur traite les lignes comme des entrées de programme.

Le gestionnaire du programme courant reçoit les objets compilés acceptés.

Le mécanisme historique est explicitement décrit : ENCODER utilise un fichier-données contenant une représentation textuelle du programme et soumet les lignes au compilateur. [INRDP75 Annexe II]

### EXIGENCES SATISFAITES

EXI-0057.

EXI-0052.

EXI-0001.

EXI-0004.

### INTERFACES

`ENCODER(fichier_donnees)`

Entrée :

- fichier-données contenant une représentation textuelle.

Sortie :

- programme courant résultant ;
- ou diagnostics.

### DONNÉES MANIPULÉES

- lignes textuelles ;
- fichier-données ;
- objets compilés ;
- diagnostics ;
- programme courant.

### CONTRAINTES

ENCODER ne charge pas directement une représentation compilée comme le fait l'appel d'un fichier-programme.

Il réintroduit les lignes textuelles dans le processus de compilation.

### INVARIANTS

Une ligne incorrecte ne devient pas une ligne du programme courant.

Le résultat de l'opération est soumis aux mêmes règles de compilation que les lignes introduites dans le dialogue.

### TRAÇABILITÉ

HIS-11.5 → EXI-0057, EXI-0001, EXI-0004 → VM-1706, VM-0501 à VM-0505 → ANN-F-0011.

---

# F.11 Interface de décodage

### ANN-F-0012
Interface DECODER

### OBJET

Définir l'échange entre le programme courant, le décompilateur et le bibliothécaire lors de la production d'un fichier-données contenant une représentation textuelle du programme.

### RESPONSABILITÉS

Le décompilateur transforme la représentation compilée du programme courant en représentation textuelle.

Le bibliothécaire conserve cette représentation sous forme de fichier-données temporaire lorsque l'opération le prévoit.

Le mécanisme historique est décrit comme l'opération inverse d'ENCODER : le programme courant est transformé en caractères puis rangé comme donnée. [INRDP75 Annexe II]

### EXIGENCES SATISFAITES

EXI-0003.

EXI-0015.

EXI-0057.

### INTERFACES

`DECODER(programme_courant)`

Sortie :

- représentation textuelle ;
- fichier-données résultant ;
- ou diagnostic.

### DONNÉES MANIPULÉES

- programme courant ;
- objets compilés ;
- représentation textuelle ;
- fichier-données temporaire.

### CONTRAINTES

DECODER ne doit pas modifier le programme courant.

### INVARIANTS

La représentation textuelle produite correspond aux objets compilés décompilés.

### CONSÉQUENCES

La représentation textuelle peut être traitée ultérieurement comme donnée.

### TRAÇABILITÉ

HIS-11.3, HIS-11.5 → EXI-0003, EXI-0015, EXI-0057 → VM-1704, VM-1801 → ANN-F-0012.

---

# F.12 Interface du temps partagé

### ANN-F-0013
Interface du gestionnaire des tâches

### OBJET

Définir les échanges nécessaires à la gestion logique des tâches utilisateur dans le temps partagé.

### RESPONSABILITÉS

Le gestionnaire :

- conserve l'état des tâches ;
- désigne la tâche active ;
- suspend une tâche ;
- restaure une tâche ;
- coordonne le passage d'une tâche à une autre.

Le manuel de 1975 décrit le temps partagé comme une alternance d'exécution des tâches et précise qu'à un instant donné une seule tâche est active. L'état d'une tâche abandonnée est conservé dans la zone de swap. [INRDP75 §3.1]

### EXIGENCES SATISFAITES

EXI-0058.

EXI-0059.

### INTERFACES

`ENREGISTRER_TACHE(tache)`

`ACTIVER_TACHE(tache)`

`SUSPENDRE_TACHE(tache)`

`RESTAURER_TACHE(tache)`

`OBTENIR_TACHE_ACTIVE()`

### DONNÉES MANIPULÉES

- identité de tâche ;
- état de tâche ;
- état d'exécution ;
- état de session ;
- informations nécessaires à la reprise.

### CONTRAINTES

Le mécanisme ne définit pas l'algorithme de sélection des tâches.

Une seule tâche est considérée comme active dans le contexte logique concerné.

La conservation d'une tâche n'est pas une opération de persistance de fichier.

### INVARIANTS

La suspension conserve l'état nécessaire à la reprise.

La restauration ne transforme pas la tâche en nouveau programme courant.

### CONSÉQUENCES

Le temps partagé peut être réalisé sans modifier la sémantique du programme courant.

### TRAÇABILITÉ

HIS-3.10, HIS-4.1, HIS-4.2, HIS-4.3, HIS-4.4.1 → EXI-0058, EXI-0059 → VM-2105 → ANN-F-0013.

---

# F.13 Interface de permutation

### ANN-F-0014
Interface de permutation d'une tâche

### OBJET

Définir les opérations logiques correspondant à la conservation et à la restauration de l'état d'une tâche.

### RESPONSABILITÉS

La permutation permet :

1. de retirer une tâche de l'état actif ;
2. de conserver son état ;
3. de rendre disponible une autre tâche ;
4. de restaurer l'état de la tâche choisie ;
5. de reprendre son exécution.

### EXIGENCES SATISFAITES

EXI-0058.

EXI-0059.

### INTERFACES

`SWAP_OUT(tache)`

`SWAP_IN(tache)`

### DONNÉES MANIPULÉES

- état d'exécution ;
- registres virtuels ;
- pile d'évaluation ;
- cadres d'activation ;
- variables ;
- références vers objets ;
- état de suspension.

### CONTRAINTES

La permutation ne doit pas modifier le contenu logique du programme courant.

Elle ne doit pas être confondue avec RANGER ou APPELER.

### INVARIANTS

Après `SWAP_OUT`, l'état nécessaire à la reprise est conservé.

Après `SWAP_IN`, l'état logique restauré correspond à celui conservé.

### TRAÇABILITÉ

HIS-3.10, HIS-4.4.1 → EXI-0058, EXI-0059 → VM-2105 → ANN-F-0014.

---

# F.14 Interface de reprise

### ANN-F-0015
Interface de reprise d'une exécution

### OBJET

Définir l'échange entre le moniteur, le gestionnaire de reprise et l'état d'exécution suspendu.

### RESPONSABILITÉS

Le gestionnaire de reprise :

- identifie l'exécution suspendue ;
- vérifie que son état est disponible ;
- restaure l'état nécessaire ;
- restaure les variables et objets ;
- restaure la position d'exécution ;
- applique, lorsqu'elle est prévue, la modification de position demandée ;
- restitue l'exécution au moteur.

### EXIGENCES SATISFAITES

EXI-0060.

EXI-0024.

EXI-0031.

EXI-0033.

### INTERFACES

`REPRENDRE(execution, position)`

Entrées :

- exécution suspendue ;
- position éventuellement demandée.

Sortie :

- exécution active ;
- ou diagnostic.

### DONNÉES MANIPULÉES

- état d'exécution ;
- position ;
- variables ;
- objets ;
- cadres d'activation ;
- pile d'évaluation ;
- statut.

### CONTRAINTES

La reprise ne doit pas réinitialiser les valeurs acquises avant l'interruption lorsque le comportement du Tome II prévoit leur conservation.

La reprise ne doit pas modifier le programme courant.

### INVARIANTS

Une exécution suspendue conserve l'état nécessaire à sa reprise.

Une exécution reprise possède une position d'exécution définie.

Une erreur de reprise ne transforme pas une exécution suspendue en exécution valide.

### CONSÉQUENCES

La reprise est une opération sur l'état d'exécution et non sur la représentation compilée du programme.

### TRAÇABILITÉ

HIS-13.1, HIS-13.3, HIS-13.4 → EXI-0060 → VM-2106 → ANN-F-0015.

---

# F.15 Interface des erreurs

### ANN-F-0016
Interface de diagnostic

### OBJET

Définir l'échange entre tout composant producteur d'erreur et le gestionnaire des erreurs.

### RESPONSABILITÉS

Tout composant pouvant détecter une anomalie fournit :

- la catégorie de l'erreur ;
- le contexte ;
- les informations nécessaires au diagnostic.

Le gestionnaire des erreurs :

- classe l'erreur ;
- construit le diagnostic ;
- détermine son effet sur l'état de la machine ;
- transmet le diagnostic au moniteur.

### EXIGENCES SATISFAITES

EXI-0004.

EXI-0018.

EXI-0019.

EXI-0020.

EXI-0021.

EXI-0051.

### INTERFACES

`SIGNALER_ERREUR(categorie, contexte)`

`CLASSIFIER_ERREUR(erreur)`

`PRODUIRE_DIAGNOSTIC(erreur)`

`TRAITER_ERREUR(erreur)`

### DONNÉES MANIPULÉES

- erreur ;
- catégorie ;
- contexte ;
- diagnostic ;
- état de la machine.

### CONTRAINTES

Une erreur de compilation ne doit pas être confondue avec une erreur d'exécution.

Une erreur de compilation doit être traitée avant l'acceptation de la ligne.

Une erreur d'exécution doit être traitée dans le contexte de l'exécution concernée.

### INVARIANTS

Un diagnostic correspond à une erreur identifiée.

Le traitement d'une erreur ne modifie pas arbitrairement le programme courant.

### TRAÇABILITÉ

HIS-5.2, HIS-6.5, HIS-12.1 → EXI-0018, EXI-0019, EXI-0051 → VM-1901 à VM-1904 → ANN-F-0016.

---

# F.16 Interface du moniteur

### ANN-F-0017
Interface du cycle conversationnel

### OBJET

Définir le point de coordination entre l'utilisateur et les composants de la machine virtuelle.

### RESPONSABILITÉS

Le moniteur :

- reçoit une entrée ;
- la classe ;
- sélectionne le composant compétent ;
- transmet la demande ;
- recueille le résultat ;
- restitue le résultat ;
- maintient l'état conversationnel.

Le Tome II définit le traitement conversationnel comme une fonction d'orientation des demandes vers le traitement approprié selon qu'il s'agit notamment d'une commande, d'une ligne de programme, d'une expression immédiate ou d'une demande adressée à un processeur.

### EXIGENCES SATISFAITES

EXI-0052.

EXI-0030.

EXI-0033.

### INTERFACES

`RECEVOIR_ENTREE(entree)`

`CLASSIFIER(entree)`

`TRANSMETTRE(demande)`

`RECEVOIR_RESULTAT(resultat)`

`RESTITUER(resultat)`

`INTERROMPRE(execution)`

`REPRENDRE(execution)`

### DONNÉES MANIPULÉES

- entrée ;
- demande ;
- état de session ;
- résultat ;
- diagnostic ;
- référence vers une exécution.

### CONTRAINTES

Le moniteur ne réalise pas lui-même la compilation, l'interprétation ou la gestion des fichiers.

Il coordonne ces fonctions.

### INVARIANTS

Toute demande validée est orientée vers un traitement déterminé.

Le contrôle conversationnel reste disponible lorsque l'état de la machine le permet.

### TRAÇABILITÉ

HIS-3.1, HIS-3.11, HIS-4.5.6 → EXI-0052 → VM-2001 → ANN-F-0017.

---

# F.17 Interface de décompilation

### ANN-F-0018
Interface du décompilateur

### OBJET

Définir l'échange entre le programme courant et le décompilateur.

### RESPONSABILITÉS

Le décompilateur :

- reçoit une représentation compilée ;
- restitue une représentation textuelle ;
- produit un diagnostic lorsqu'une décompilation ne peut être réalisée.

### EXIGENCES SATISFAITES

EXI-0003.

EXI-0010.

EXI-0015.

### INTERFACES

`DECOMPILER(objet_compile)`

`DECOMPILER_PROGRAMME(programme_courant)`

### DONNÉES MANIPULÉES

- objet compilé ;
- opérations codées ;
- valeurs ;
- références ;
- représentation textuelle.

### CONTRAINTES

Le décompilateur ne modifie pas le programme courant.

Il exploite la représentation compilée et non une copie obligatoire du texte source.

### INVARIANTS

La représentation textuelle produite correspond à l'objet compilé soumis à décompilation.

### CONSÉQUENCES

Le même objet compilé peut servir à l'exécution et à la décompilation.

### TRAÇABILITÉ

HIS-4.5.2, HIS-5.3 → EXI-0003, EXI-0010, EXI-0015 → VM-1801 → ANN-F-0018.

---

# F.18 Interface globale de l'état d'exécution

### ANN-F-0019
Interface de l'état d'exécution

### OBJET

Définir l'accès contrôlé aux éléments constituant l'état dynamique d'une exécution.

### RESPONSABILITÉS

L'état d'exécution fournit :

- la position courante ;
- les registres virtuels ;
- la pile d'évaluation ;
- les cadres d'activation ;
- les variables ;
- les objets accessibles ;
- le statut ;
- les informations nécessaires à la suspension et à la reprise.

### EXIGENCES SATISFAITES

EXI-0024.

EXI-0031.

EXI-0033.

EXI-0034 à EXI-0036.

EXI-0042.

EXI-0043.

EXI-0044.

### INTERFACES

`LIRE_POSITION(execution)`

`MODIFIER_POSITION(execution, position)`

`LIRE_REGISTRE(execution, registre)`

`ECRIRE_REGISTRE(execution, registre, valeur)`

`LIRE_ETAT(execution)`

`SUSPENDRE(execution)`

`RESTAURER(execution)`

### DONNÉES MANIPULÉES

- position ;
- registres ;
- pile ;
- cadres ;
- variables ;
- objets ;
- état de suspension.

### CONTRAINTES

L'état d'exécution ne contient pas le programme courant lui-même.

Il contient des références vers les éléments du programme courant lorsque l'exécution en a besoin.

### INVARIANTS

La modification de l'état d'exécution ne modifie pas la définition du programme courant.

La position d'exécution appartient à une exécution déterminée.

### TRAÇABILITÉ

HIS-8.3, HIS-13.1 à HIS-13.4 → EXI-0024, EXI-0031, EXI-0033 à EXI-0036 → VM-0801, VM-0901 à VM-0903, VM-2103, VM-2106 → ANN-F-0019.

---

# F.19 Règles générales d'utilisation des interfaces

### ANN-F-0020
Règles d'interconnexion

### OBJET

Définir les règles communes applicables à toutes les interfaces de la machine virtuelle.

### RESPONSABILITÉS

Toute interconnexion doit :

- identifier son initiateur ;
- identifier son destinataire ;
- définir les données échangées ;
- définir le résultat ;
- définir les erreurs possibles ;
- préserver les invariants du composant appelé.

### EXIGENCES SATISFAITES

EXI-0001 à EXI-0062 selon les mécanismes concernés.

### CONTRAINTES

Une interface ne doit jamais constituer un moyen implicite de contourner une responsabilité normative.

Une donnée transmise doit appartenir au domaine logique défini par le composant destinataire.

Une erreur doit être transmise explicitement.

Une interface ne doit pas imposer une structure physique.

### INTERFACES

Toute interface respecte la forme logique :

```text
INITIATEUR
    │
    │ demande + données
    ▼
DESTINATAIRE
    │
    │ résultat ou diagnostic
    ▼
INITIATEUR
```

Lorsqu'une opération modifie un état, l'état modifié fait partie du résultat architectural observable.

### DONNÉES MANIPULÉES

Les données échangées appartiennent aux catégories suivantes :

- ligne ;
- objet compilé ;
- représentation compilée ;
- référence ;
- valeur ;
- état d'exécution ;
- diagnostic ;
- fichier ;
- tâche ;
- demande utilisateur ;
- résultat.

### ALGORITHMES UTILISÉS

Aucun protocole d'implémentation n'est imposé.

### INVARIANTS

**I-F-1 — Responsabilité unique**

Une responsabilité architecturale appartient à un composant déterminé.

**I-F-2 — État explicite**

Toute modification d'état relevant d'une interface est identifiable.

**I-F-3 — Séparation**

Le programme courant, l'exécution, les fichiers et les tâches restent des catégories distinctes.

**I-F-4 — Traçabilité**

Toute interface est rattachée à au moins une exigence ou à un mécanisme du Tome II.

**I-F-5 — Indépendance technologique**

Aucune interface ne dépend d'un langage ou d'un système d'exploitation.

**I-F-6 — Préservation**

Une interface du Tome III ne peut modifier la sémantique définie par le Tome II.

### CONSÉQUENCES

Les interfaces constituent le contrat entre les composants.

Une implémentation peut regrouper plusieurs composants physiques ou logiciels, mais elle doit conserver leurs contrats logiques distincts.

### TRAÇABILITÉ

Conditionnement du Tome III → ANN-E-0001 → ANN-F-0020.

Tome II → VM-2501 à VM-2510 → ANN-F-0002 à ANN-F-0019.

HIS → EXI → VM → ANN-F.

---

# Annexe G — Registre des invariants globaux

## ANN-G-0001
Invariants de la machine virtuelle

### OBJET

Regrouper les invariants qui doivent être conservés par l'ensemble des composants de la machine virtuelle.

### RESPONSABILITÉS

Le présent registre fournit une référence commune pour :

- la conception des composants ;
- la définition des interfaces ;
- la vérification d'une implémentation ;
- l'analyse des écarts.

### EXIGENCES SATISFAITES

EXI-0001 à EXI-0062.

### CONTRAINTES

Les invariants ne constituent pas des algorithmes.

Ils définissent des propriétés qui doivent rester vraies pendant les transitions autorisées de la machine virtuelle.

---

## G.1 Programme courant

### I-G-1 — Unicité des lignes

Le programme courant ne contient pas deux objets compilés portant le même numéro de ligne.

### I-G-2 — Intégrité

Toute ligne présente dans le programme courant possède une représentation compilée valide.

### I-G-3 — Rejet atomique

Une ligne rejetée par le compilateur ne modifie pas le programme courant.

### I-G-4 — Ordre logique

L'ordre logique des lignes est déterminé par leur numéro.

### I-G-5 — Indépendance

La modification d'une ligne ne nécessite pas la recompilation des autres lignes.

### TRAÇABILITÉ

HIS-6.2 → EXI-0001, EXI-0004, EXI-0005, EXI-0006, EXI-0007, EXI-0008 → VM-0301 à VM-0306 → ANN-G-0001.

---

## G.2 Représentation compilée

### I-G-6 — Exécutabilité

Tout objet compilé accepté est exécutable par l'interpréteur.

### I-G-7 — Décompilabilité

Tout objet compilé conservé comme programme courant reste décompilable conformément aux exigences applicables.

### I-G-8 — Indépendance du texte

L'exécution ne dépend pas de la conservation obligatoire du texte source.

### I-G-9 — Stabilité

La représentation compilée ne doit pas être modifiée par une opération de décompilation.

### TRAÇABILITÉ

HIS-4.5.1, HIS-4.5.2, HIS-5.3 → EXI-0002, EXI-0003, EXI-0010, EXI-0015 → VM-0401 à VM-0409, VM-1801 → ANN-G-0001.

---

## G.3 Exécution

### I-G-10 — Séparation de l'état

L'état d'exécution est distinct du programme courant.

### I-G-11 — Position

Une exécution active possède une position logique déterminée.

### I-G-12 — Terminaison

La terminaison détruit l'état spécifique de l'exécution sans supprimer le programme courant.

### I-G-13 — Suspension

Une exécution suspendue conserve les éléments nécessaires à une reprise autorisée.

### I-G-14 — Reprise

Une reprise restaure l'état nécessaire avant de poursuivre l'exécution.

### TRAÇABILITÉ

HIS-8.2, HIS-8.3, HIS-13.1, HIS-13.3, HIS-13.4 → EXI-0024, EXI-0030, EXI-0031, EXI-0033, EXI-0060 → VM-0801, VM-2103, VM-2106 → ANN-G-0001.

---

## G.4 Variables et objets

### I-G-15 — Référence valide

Une référence utilisée par l'exécution doit être résolue dans un contexte valide.

### I-G-16 — Cohérence

Une référence résolue désigne un objet accessible dans son contexte.

### I-G-17 — Isolation

Une modification d'un objet d'exécution ne modifie pas la représentation compilée qui l'a référencé.

### TRAÇABILITÉ

HIS-10.1, HIS-10.2 → EXI-0014, EXI-0032, EXI-0038 → VM-0802, VM-1101 à VM-1203 → ANN-G-0001.

---

## G.5 Procédures et fonctions

### I-G-18 — Activation

Une procédure ou une fonction en cours d'exécution possède un contexte d'activation correspondant.

### I-G-19 — Retour

Le retour restitue le contexte appelant correspondant à l'activation terminée.

### I-G-20 — Isolement des contextes

Les variables locales d'une activation ne sont pas confondues avec celles d'une autre activation.

### TRAÇABILITÉ

HIS-13.1, HIS-13.2 → EXI-0034 à EXI-0036, EXI-0042 à EXI-0044 → VM-0901 à VM-0903, VM-1301 à VM-1303 → ANN-G-0001.

---

## G.6 Fichiers

### I-G-21 — Distinction des portées

Un fichier-données temporaire et un fichier-données permanent sont des objets distincts.

### I-G-22 — Portée temporaire

Un fichier-données temporaire appartient à la console qui l'utilise.

### I-G-23 — Durée temporaire

Un fichier-données temporaire cesse d'exister à la fin de la session conformément aux règles du système.

### I-G-24 — Persistance

Un fichier-données permanent subsiste au-delà de la session.

### I-G-25 — Transformation

La transformation d'un fichier-données temporaire en fichier-données permanent entraîne la disparition du fichier temporaire correspondant et la réutilisabilité de son espace logique.

Cette propriété est explicitement documentée dans le Tome I.

### I-G-26 — Programme persistant

Un fichier-programme conservé ne doit pas être confondu avec le programme courant.

### I-G-27 — État de tâche

Un état de tâche conservé dans la zone de swap n'est pas un fichier utilisateur.

### I-G-27a — Unité d'enregistrement

Un enregistrement constitue une unité logique identifiée à l'intérieur d'un fichier-données.

### I-G-27b — Cohérence des opérations sur les enregistrements

GARER remplace le contenu de l'enregistrement désigné ; CHARGER restitue son type et son contenu ; SUPPRIMER supprime l'enregistrement désigné ou le fichier lorsqu'aucun enregistrement n'est désigné.

### TRAÇABILITÉ

HIS-11.1, HIS-11.3, HIS-11.4, HIS-11.5 → EXI-0009, EXI-0022, EXI-0025, EXI-0053 à EXI-0057 → VM-1701 à VM-1708 → ANN-G-0001. 

---

## G.7 Temps partagé

### I-G-28 — Tâche active

Une seule tâche est active à un instant donné dans le modèle logique du temps partagé.

### I-G-29 — Conservation

Une tâche suspendue conserve son état.

### I-G-30 — Restauration

La restauration d'une tâche restitue son état logique.

### I-G-31 — Indépendance

La permutation des tâches ne modifie pas le contenu logique des programmes courants.

### TRAÇABILITÉ

HIS-3.10 → EXI-0058, EXI-0059 → VM-2105 → ANN-G-0001.

---

## G.8 Dialogue

### I-G-32 — Orientation

Toute entrée validée est orientée vers le traitement approprié.

### I-G-33 — Diagnostic

Tout traitement pouvant produire une erreur fournit un diagnostic identifiable.

### I-G-34 — Retour au moniteur

Une opération terminée rend le contrôle au niveau conversationnel lorsque le cycle correspondant le prévoit.

### TRAÇABILITÉ

HIS-3.1, HIS-3.11 → EXI-0052 → VM-2001 → ANN-G-0001.

---

## G.9 Traçabilité

### I-G-35 — Traçabilité architecturale

Toute décision architecturale doit pouvoir être rattachée :

- à une contrainte historique lorsque celle-ci existe ;
- à une exigence ;
- à un mécanisme du Tome II ou à une décision architecturale du Tome III.

### I-G-36 — Stabilité des identifiants

Les identifiants HIS, EXI, VM, DEC et ANN utilisés par les documents de la collection sont stables.

### I-G-37 — Hiérarchie normative

Aucun invariant du Tome III ne peut contredire une propriété du Tome II.

Aucune propriété du Tome II ne peut contredire le Catalogue des exigences.

Aucune exigence ne peut contredire un fait historique documenté auquel elle est explicitement rattachée.

### TRAÇABILITÉ

Conventions éditoriales → EXI → Tome II → Tome III → ANN-G-0001.

---

# Annexe H — Matrice de conformité des interfaces

## ANN-H-0001
Matrice interfaces–responsabilités

### OBJET

Fournir une correspondance synthétique entre les principales interfaces et les responsabilités architecturales qu'elles réalisent.

### RESPONSABILITÉS

| Interface | Initiateur principal | Destinataire principal | Responsabilité |
|---|---|---|---|
| ANN-F-0002 | Moniteur | Compilateur | Compilation d'une ligne |
| ANN-F-0003 | Compilateur | Programme courant | Acceptation |
| ANN-F-0004 | Moniteur | Moteur d'exécution | Lancement |
| ANN-F-0005 | Interpréteur | Gestionnaires d'objets | Résolution |
| ANN-F-0006 | Interpréteur | Pile d'évaluation | Évaluation |
| ANN-F-0007 | Interpréteur | Gestionnaire d'activation | Appel et retour |
| ANN-F-0008 | Interpréteur | Contrôle du flux | Transfert |
| ANN-F-0009 | Moniteur | Bibliothécaire | Persistance |
| ANN-F-0010 | Programme / commande | Bibliothécaire | Fichiers-données et enregistrements |
| ANN-F-0011 | Bibliothécaire | Compilateur | ENCODER |
| ANN-F-0012 | Programme courant | Décompilateur / bibliothécaire | DECODER |
| ANN-F-0013 | Moniteur de temps partagé | Gestionnaire des tâches | Activation/suspension |
| ANN-F-0014 | Gestionnaire des tâches | État de tâche | Permutation |
| ANN-F-0015 | Moniteur | Gestionnaire de reprise | Reprise |
| ANN-F-0016 | Tous composants | Gestionnaire des erreurs | Diagnostic |
| ANN-F-0017 | Utilisateur | Moniteur | Dialogue |
| ANN-F-0018 | Programme courant | Décompilateur | Décompilation |
| ANN-F-0019 | Tous composants d'exécution | État d'exécution | État dynamique |

### EXIGENCES SATISFAITES

EXI-0001 à EXI-0062.

### CONTRAINTES

La matrice ne crée aucun composant supplémentaire.

Elle décrit les relations entre composants déjà définis.

### INTERFACES

Les interfaces sont celles du registre ANN-F.

### DONNÉES MANIPULÉES

Les données sont celles définies par les interfaces correspondantes.

### ALGORITHMES UTILISÉS

Aucun.

### STATUT

Choix de conception.

### INVARIANTS

Toute responsabilité majeure de la machine virtuelle possède au moins une interface d'entrée ou de sortie permettant son intégration au reste de l'architecture.

Aucune interface ne doit créer une dépendance circulaire incompatible avec les responsabilités définies.

### CONSÉQUENCES

La matrice constitue un support de vérification de l'architecture logique.

### TRAÇABILITÉ

VM-2501 à VM-2510 → ANN-F-0002 à ANN-F-0019 → ANN-H-0001.

---

# Annexe I — Cycle architectural de traitement

## ANN-I-0001
Cycle global d'une demande

### OBJET

Définir les transitions logiques successives d'une demande depuis son entrée dans la machine virtuelle jusqu'à la restitution du résultat.

### RESPONSABILITÉS

Le cycle doit permettre de distinguer :

- entrée ;
- classification ;
- traitement ;
- diagnostic éventuel ;
- modification d'état ;
- restitution.

### EXIGENCES SATISFAITES

EXI-0052.

EXI-0001.

EXI-0029.

EXI-0030.

EXI-0051.

### INTERFACES

```text
ENTRÉE
  │
  ▼
CLASSIFICATION
  │
  ├── ligne de programme ──► COMPILATION
  │                             │
  │                             ├── succès ──► PROGRAMME COURANT
  │                             └── erreur ──► DIAGNOSTIC
  │
  ├── expression immédiate ──► EXÉCUTION
  │
  ├── demande d'exécution ──► EXÉCUTION
  │
  └── commande ──► TRAITEMENT DE COMMANDE
```

Après traitement :

```text
TRAITEMENT
    │
    ├── résultat ──► RESTITUTION
    ├── diagnostic ──► RESTITUTION
    └── état modifié ──► MONITEUR
```

### DONNÉES MANIPULÉES

- entrée ;
- demande ;
- objet compilé ;
- programme courant ;
- état d'exécution ;
- fichier ;
- diagnostic ;
- résultat.

### ALGORITHMES UTILISÉS

Le cycle est un enchaînement logique d'états.

Il ne définit pas l'algorithme interne de classification.

### INVARIANTS

Une demande n'est traitée que par le composant correspondant à sa nature.

Une erreur de compilation ne produit pas de modification du programme courant.

### TRAÇABILITÉ

HIS-3.1, HIS-3.11 → EXI-0052 → VM-2001 → ANN-I-0001.

---

## ANN-I-0002
Cycle de vie d'une ligne de programme

### OBJET

Définir le cycle logique d'une ligne de programme.

### INTERFACES

```text
SAISIE
  │
  ▼
ANALYSE
  │
  ├── erreur ──► DIAGNOSTIC
  │
  └── valide
       │
       ▼
GÉNÉRATION
       │
       ▼
OBJET COMPILÉ
       │
       ▼
ACCEPTATION
       │
       ▼
PROGRAMME COURANT
```

### INVARIANTS

L'objet compilé n'est accepté qu'après validation.

La présence d'une ligne dans le programme courant implique l'existence de sa représentation compilée.

### TRAÇABILITÉ

HIS-6.2, HIS-6.5 → EXI-0001, EXI-0002, EXI-0004 → VM-0501 à VM-0505 → ANN-I-0002.

---

## ANN-I-0003
Cycle de vie d'une exécution

### OBJET

Définir le cycle logique d'une exécution.

### INTERFACES

```text
CRÉATION
   │
   ▼
INITIALISATION
   │
   ▼
EXÉCUTION
   │
   ├── poursuite ───────────────┐
   │                            │
   ├── interruption ──► SUSPENSION
   │                         │
   │                         └──► REPRISE
   │                                  │
   │                                  └──► EXÉCUTION
   │
   ├── erreur ──► ERREUR
   │
   └── fin ──► TERMINAISON
```

### DONNÉES MANIPULÉES

- état d'exécution ;
- position ;
- registres ;
- pile ;
- cadres ;
- variables ;
- objets.

### INVARIANTS

La suspension ne détruit pas l'état nécessaire à la reprise.

La terminaison ne détruit pas le programme courant.

Une erreur ne produit pas automatiquement une reprise.

### TRAÇABILITÉ

HIS-8.2, HIS-8.3, HIS-13.1 à HIS-13.4 → EXI-0024, EXI-0030, EXI-0031, EXI-0033, EXI-0060 → VM-0801, VM-2103, VM-2106 → ANN-I-0003.

---

## ANN-I-0004
Cycle de vie d'un fichier-données temporaire

### OBJET

Définir le cycle logique d'un fichier-données temporaire.

### INTERFACES

```text
CRÉATION
   │
   ▼
FICHIER TEMPORAIRE
   │
   ├── LECTURE / ÉCRITURE
   │
   ├── SUPPRESSION ──► FIN
   │
   ├── CATALOGAGE ──► FICHIER PERMANENT
   │
   └── FIN DE SESSION ──► DESTRUCTION
```

### CONTRAINTES

La fin de session détruit les fichiers-données temporaires conformément au comportement historique.

La commande CATALOGUER transforme la conservation temporaire en conservation permanente et rend réutilisable l'espace correspondant. [LSE15 §9-5]

### INVARIANTS

Un fichier temporaire appartient à une portée déterminée.

Un fichier détruit ne peut plus être utilisé.

### TRAÇABILITÉ

HIS-11.3 → EXI-0053, EXI-0054, EXI-0056 → VM-1703, VM-1704, VM-1708 → ANN-I-0004.

---

# Annexe J — Synthèse finale de conformité architecturale

## ANN-J-0001
Critères de conformité

### OBJET

Définir les critères minimaux permettant de vérifier qu'une architecture respecte le présent Tome III.

### RESPONSABILITÉS

Une réalisation conforme doit démontrer :

1. la présence logique des composants requis ;
2. la conservation de leurs responsabilités ;
3. la conformité de leurs interfaces ;
4. la conservation des invariants ;
5. la séparation du programme courant et de l'état d'exécution ;
6. la gestion des fichiers conformément aux catégories définies et la manipulation des enregistrements selon les opérations prévues ;
7. la conservation de l'état nécessaire au temps partagé ;
8. la possibilité de reprise lorsque celle-ci est prévue ;
9. la production de diagnostics identifiables ;
10. la traçabilité des décisions architecturales.

### EXIGENCES SATISFAITES

EXI-0001 à EXI-0062.

### CONTRAINTES

La conformité est évaluée au niveau architectural.

Elle ne nécessite pas l'identité de l'implémentation avec la reconstruction interne.

Une organisation différente est acceptable uniquement si elle conserve les mêmes responsabilités, interfaces, invariants et effets observables définis par les niveaux normatifs supérieurs.

### INTERFACES

Les interfaces de référence sont celles du registre ANN-F.

### DONNÉES MANIPULÉES

La vérification porte notamment sur :

- objets compilés ;
- programme courant ;
- représentations compilées ;
- état d'exécution ;
- fichiers ;
- enregistrements de fichiers-données ;
- tâches ;
- diagnostics ;
- états de dialogue.

### ALGORITHMES UTILISÉS

Aucune méthode particulière de vérification n'est imposée.

### STATUT

Choix de conception.

### INVARIANTS

**C-1 — Hiérarchie normative**

La conformité doit respecter :

```text
Conventions éditoriales
        │
        ▼
Catalogue des exigences
        │
        ▼
Tome I
        │
        ▼
Tome II
        │
        ▼
Tome III
```

**C-2 — Conservation fonctionnelle**

Aucune architecture ne peut supprimer une fonction définie dans le Tome II.

**C-3 — Conservation historique**

Aucune architecture ne peut présenter comme historiquement attestée une décision qui relève uniquement de la reconstruction.

**C-4 — Indépendance technologique**

Aucune technologie particulière ne constitue une condition de conformité.

**C-5 — Traçabilité**

Toute décision architecturale importante doit être rattachable à sa justification.

**C-6 — Cohérence**

Une interface, un invariant ou une responsabilité ne peut être défini de manière contradictoire dans deux parties du Tome III.

### CONSÉQUENCES

Le présent registre complète la spécification d'architecture sans modifier la machine abstraite définie dans le Tome II.

Il fournit les contrats nécessaires à une implémentation future.

Il ne définit ni code, ni représentation binaire, ni choix de processeur, ni système d'exploitation.

### TRAÇABILITÉ

Conventions éditoriales → ANN-E-0001.

Catalogue des exigences → EXI-0001 à EXI-0062.

Tome I → contraintes historiques.

Tome II → mécanismes fonctionnels.

Tome III → architecture logique.

ANN-F → interfaces.

ANN-G → invariants.

ANN-H → correspondances interfaces/responsabilités.

ANN-I → cycles de traitement.

ANN-J → critères de conformité.

---

## État de la rédaction

Le présent segment complète les annexes de la spécification d'architecture logique par :

- le registre des interfaces ;
- le registre des invariants globaux ;
- la matrice de conformité des interfaces ;
- les cycles architecturaux ;
- les critères finaux de conformité.

La rédaction est arrêtée ici, à la fin de **l'Annexe J — Synthèse finale de conformité architecturale**.