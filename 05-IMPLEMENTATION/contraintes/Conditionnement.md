# Conditionnement — Implémentation physique du L.S.E. sur Apple IIGS / ORCA-C

## 1. Objet

Tu es chargé de réaliser l'implémentation physique de la reconstruction historique du Langage Symbolique d'Enseignement (L.S.E.).

Cette implémentation constitue la première réalisation cible de la reconstruction.

La plateforme cible est :

* Apple IIGS ;
* processeur 65816 ;
* 4 Mio de mémoire vive ;
* disque dur de 32 Mio ;
* environnement système Apple IIGS ;
* lancement depuis le shell ORCA ;
* application exclusivement textuelle ;
* aucune interface graphique Toolbox ;
* langage d'implémentation : C ;
* compilateur cible : ORCA/C 2.2.1.

Le dépôt officiel de référence d'ORCA/C est :

https://github.com/byteworksinc/ORCA-C

Le dépôt décrit ORCA/C comme un compilateur C ANSI pour le 65816 avec bibliothèques Apple IIGS et contient notamment les sources du compilateur, les bibliothèques, les tests et les notes spécifiques à la version 2.2.1.

## 2. Nature de la mission

Tu ne dois pas reconcevoir le L.S.E.

Tu dois réaliser physiquement la machine définie par :

1. les conventions éditoriales ;
2. le Catalogue des exigences ;
3. le Tome I ;
4. le Tome II ;
5. le Tome III ;
6. le Tome IV.

La hiérarchie normative est impérative.

Une décision prise au niveau de l'implémentation ne peut pas modifier :

* une propriété historique ;
* une exigence EXI ;
* un mécanisme du Tome II ;
* une responsabilité du Tome III ;
* un contrat ou invariant du Tome IV.

Le Tome III définit explicitement les responsabilités logiques des composants : compilateur, programme courant, interpréteur, environnement d'exécution, gestionnaire des variables, gestionnaire mémoire, décompilateur, bibliothécaire, moniteur, analyseur de commandes et gestionnaire des erreurs.

Le Tome IV complète cette définition par les interfaces, formats logiques, algorithmes, contrats et invariants. Il ne constitue pas lui-même une implémentation.

## 3. Principe directeur

L'objectif est une **implémentation historiquement fidèle**, et non une réécriture moderne du langage.

Tu privilégieras, dans cet ordre :

1. fidélité aux propriétés historiques ;
2. conformité aux exigences EXI ;
3. conformité au comportement du Tome II ;
4. conformité à l'architecture du Tome III ;
5. conformité aux interfaces et algorithmes du Tome IV ;
6. simplicité de réalisation ;
7. économie de mémoire et de stockage ;
8. compatibilité ORCA/C 2.2.1 ;
9. facilité de maintenance.

Une technique moderne ne doit jamais être introduite simplement parce qu'elle est plus confortable.

## 4. Contraintes matérielles

L'implémentation doit être conçue pour fonctionner réellement dans la configuration cible :

### Mémoire

4 Mio de RAM.

La consommation mémoire doit être maîtrisée.

Éviter notamment :

* allocations massives inutiles ;
* copies intégrales inutiles ;
* structures surdimensionnées ;
* caches sans justification ;
* dépendance à une mémoire virtuelle moderne.

Les structures doivent être dimensionnées à partir des besoins réels de la machine virtuelle.

### Stockage

32 Mio de disque.

Le stockage doit distinguer logiquement :

* fichiers-programmes ;
* fichiers-données temporaires ;
* fichiers-données permanents ;
* données nécessaires à la conservation des tâches lorsque celle-ci est implémentée.

Cette distinction est normative dans la reconstruction. Les Tomes II et III distinguent notamment les fichiers-données temporaires et permanents ainsi que leur portée.

## 5. Contraintes d'exécution

L'application :

* est une application texte ;
* s'exécute dans une console texte ;
* reçoit les entrées utilisateur par le clavier ;
* produit ses sorties sur la console ;
* fonctionne depuis le shell ORCA ;
* ne dépend pas de la Toolbox graphique ;
* ne nécessite aucune fenêtre graphique ;
* ne doit pas transformer le L.S.E. en application événementielle moderne.

Le dialogue utilisateur doit conserver le caractère conversationnel du L.S.E.

Le moniteur constitue le point d'entrée logique du dialogue ; l'analyseur de commandes détermine ensuite la nature de la demande. Cette séparation est définie par le Tome III.

## 6. Contraintes de langage C

Le code doit être compatible avec ORCA/C 2.2.1.

Le C moderne peut être utilisé lorsque le compilateur le permet effectivement, mais **la compatibilité réelle avec ORCA/C 2.2.1 prime sur toute affirmation générale concernant le standard C**.

Ne jamais supposer qu'une fonctionnalité C moderne est disponible sans la vérifier dans le corpus technique ORCA/C.

Les dépendances au système doivent être isolées derrière une couche d'adaptation.

Le cœur de la machine virtuelle doit rester aussi indépendant que possible :

* du système de fichiers Apple IIGS ;
* du terminal ;
* des conventions du shell ;
* des appels système ;
* des particularités du compilateur.

## 7. Architecture physique

La réalisation doit conserver les responsabilités logiques du Tome III.

Il est permis de regrouper plusieurs responsabilités dans une même unité C lorsque cela ne modifie pas les responsabilités logiques.

Il est également permis de répartir une responsabilité sur plusieurs fichiers C lorsque cela améliore la réalisation.

La correspondance suivante doit toutefois être maintenue :

| Architecture logique          | Réalisation physique indicative      |
| ----------------------------- | ------------------------------------ |
| Moniteur                      | module du dialogue                   |
| Analyseur de commandes        | analyseur                            |
| Compilateur                   | compilateur LSE                      |
| Programme courant             | gestionnaire du programme            |
| Interpréteur                  | moteur d'exécution                   |
| Environnement d'exécution     | contexte d'exécution                 |
| Gestionnaire des variables    | gestion des variables                |
| Gestionnaire mémoire          | gestionnaire des objets              |
| Gestionnaire des tableaux     | gestion des tableaux                 |
| Gestionnaire des chaînes      | gestion des chaînes                  |
| Gestionnaire des nombres      | gestion numérique                    |
| Décompilateur                 | décompilateur                        |
| Bibliothécaire                | gestionnaire des programmes/fichiers |
| Gestionnaire fichiers-données | fichiers de données                  |
| Gestionnaire des tâches       | conservation/reprise                 |
| Gestionnaire des erreurs      | diagnostics                          |

Cette table est une proposition d'organisation physique, et non une modification de l'architecture logique.

## 8. Représentation interne

La représentation physique des objets doit être dérivée des formats logiques du Tome IV.

Le Tome IV distingue notamment :

* objet compilé ;
* représentation compilée ;
* élément post-fixé ;
* valeur ;
* tableau ;
* chaîne ;
* enregistrement ;
* fichier-programme ;
* fichier-données ;
* état d'exécution ;
* état de tâche.

Le registre des formats précise qu'aucun format physique historique complet n'est imposé.

Tu dois donc :

1. conserver les propriétés logiques ;
2. choisir une représentation physique adaptée au 65816 ;
3. documenter chaque choix ;
4. mesurer son coût mémoire ;
5. vérifier qu'il ne modifie aucun comportement normatif.

## 9. Mémoire

La gestion mémoire doit être conçue explicitement.

Elle doit distinguer au minimum :

* données statiques ;
* programme courant ;
* représentation compilée ;
* objets d'exécution ;
* cadres d'activation ;
* pile d'évaluation ;
* chaînes ;
* tableaux ;
* variables ;
* états de tâches ;
* tampons d'entrée/sortie.

Les pointeurs C ne doivent pas être utilisés comme modèle architectural implicite.

Une adresse physique ou un pointeur C n'est qu'une technique de réalisation.

Lorsque cela est préférable, utiliser des identifiants ou références internes indépendants des adresses physiques.

## 10. Stockage des programmes

Le stockage doit respecter la distinction :

**programme source → représentation compilée → programme courant → fichier-programme**

Le programme courant doit pouvoir être exécuté sans dépendre du texte source lorsque la spécification le prévoit.

Le Tome III impose explicitement l'indépendance de l'exécution vis-à-vis du texte source.

## 11. Fichiers-données

La réalisation doit prendre en charge les mécanismes définis par les Tomes II à IV, notamment :

* identification des fichiers ;
* catalogue ;
* portée ;
* fichiers temporaires ;
* fichiers permanents ;
* enregistrements ;
* GARER ;
* CHARGER ;
* SUPPRIMER ;
* CATALOGUER ;
* ENCODER ;
* DECODER.

Les corrections récentes du Tome III précisent notamment que GARER écrit un enregistrement, CHARGER le lit et restitue son type et son contenu, et que SUPPRIMER peut porter sur un enregistrement ou sur le fichier.

Le mécanisme d'identification des fichiers est central : il ne doit pas être réduit à un simple nom de fichier du système hôte.

Le modèle logique comprend une identité et une portée.

## 12. Temps partagé et reprise

La réalisation doit préserver la distinction entre :

* fichier ;
* tâche ;
* état d'exécution.

SWAP IN et SWAP OUT ne doivent pas être implémentés comme de simples opérations de chargement/rangement de fichiers-programmes.

Le Tome III définit une gestion logique des tâches, avec conservation et restauration de l'état nécessaire à la reprise.

Si certaines fonctions historiques ne sont pas pertinentes dans une première configuration mono-utilisateur de l'Apple IIGS, elles ne doivent pas être supprimées du modèle.

Elles doivent être isolées et implémentées conformément au périmètre retenu pour la première version.

## 13. Compatibilité historique

L'implémentation cible Apple IIGS ne doit pas devenir une « version moderne » du L.S.E.

Elle doit conserver notamment :

* le langage de commande français ;
* le fonctionnement conversationnel ;
* la structure par lignes ;
* la compilation ligne par ligne ;
* la représentation compilée ;
* l'exécution de cette représentation ;
* la décompilation ;
* les commandes historiques ;
* la gestion des programmes ;
* la gestion des fichiers-données ;
* les mécanismes de procédures et fonctions ;
* les mécanismes d'erreur.

L'exigence EXI-0001 impose notamment la compilation indépendante des lignes ; le Tome III la réalise par le composant compilateur et le gestionnaire du programme courant.

## 14. Gestion des décisions d'implémentation

Chaque choix non imposé par les tomes doit être enregistré.

Format minimal :

* identifiant ;
* sujet ;
* choix ;
* alternatives envisagées ;
* justification ;
* impact mémoire ;
* impact disque ;
* impact performances ;
* impact historique ;
* références normatives ;
* statut.

Préfixe recommandé :

`IMP-XXXX`

Exemples :

`IMP-0001` — représentation physique d'un objet compilé

`IMP-0002` — allocation des chaînes

`IMP-0003` — format physique d'un fichier-programme

`IMP-0004` — représentation du catalogue

`IMP-0005` — gestion de la mémoire dynamique

Ces décisions ne doivent jamais être confondues avec les décisions `DEC` de la reconstruction documentaire.

## 15. Traçabilité obligatoire

Chaque composant significatif doit être traçable.

Chaîne minimale :

`HIS → EXI → VM → IV → IMP → CODE`

Lorsqu'il n'existe pas de maillon historique :

`EXI → VM → IV → IMP → CODE`

Chaque module important doit donc indiquer :

* exigences EXI satisfaites ;
* mécanismes VM réalisés ;
* interfaces IV utilisées ;
* décisions IMP appliquées ;
* tests associés.

La traçabilité bidirectionnelle doit être maintenue.

## 16. Tests

L'implémentation doit être construite avec une stratégie de tests dès le début.

Les tests doivent être organisés en niveaux :

### Tests de représentation

Vérification :

* objets compilés ;
* éléments post-fixés ;
* valeurs ;
* chaînes ;
* tableaux ;
* états d'exécution.

### Tests de composants

Vérification séparée :

* compilateur ;
* interpréteur ;
* décompilateur ;
* programme courant ;
* variables ;
* tableaux ;
* chaînes ;
* nombres ;
* fichiers ;
* catalogue ;
* erreurs.

### Tests d'interfaces

Chaque interface IV doit disposer de tests couvrant :

* cas normal ;
* précondition invalide ;
* erreur ;
* état limite ;
* invariant.

### Tests de conformité

Les exigences EXI doivent être testées explicitement.

### Tests historiques

Les exemples des documents historiques doivent être transformés autant que possible en programmes de test.

### Tests sur cible

Les tests doivent être exécutables sur :

* Apple IIGS réel ;
* ou émulateur Apple IIGS compatible avec l'environnement cible.

Le résultat doit être comparé à la spécification, et non à une implémentation moderne quelconque.

## 17. Discipline de développement

Ne pas développer « en partant du compilateur ».

Commencer par :

1. représentation des objets ;
2. gestion mémoire ;
3. programme courant ;
4. pile d'évaluation ;
5. état d'exécution ;
6. interpréteur minimal ;
7. dialogue ;
8. compilation ;
9. décompilation ;
10. fichiers ;
11. procédures/fonctions ;
12. mécanismes complémentaires.

Chaque étape doit produire un système exécutable et testable.

**Amendement du 2026-10-22 (arbitrage d'Éric).** L'ordre ci-dessus est remplacé par le suivant :

1. représentation des objets ;
2. gestion mémoire ;
3. programme courant (réalisation minimale, ou bouchon conforme à son interface, IMP-0192) ;
4. compilation ;
5. décompilation ;
6. pile d'évaluation et état d'exécution ;
7. interpréteur ;
8. dialogue ;
9. fichiers ;
10. procédures/fonctions ;
11. mécanismes complémentaires.

Le principe « ne pas développer en partant du compilateur » vise le risque que le compilateur impose la représentation interne. Ce risque est écarté : la représentation compilée (RCI-M) a été entièrement spécifiée et auditée avant toute ligne de code, et le compilateur la réalise sans la définir. L'avancement de la compilation et de la décompilation permet de valider RCI-M sur la cible le plus tôt possible, par la boucle compilation → décompilation → recompilation. La règle « chaque étape doit produire un système exécutable et testable » est maintenue.

## 18. Règle de résolution des ambiguïtés

Lorsqu'une difficulté apparaît :

### Niveau 1

Chercher dans le Tome IV.

### Niveau 2

Chercher dans le Tome III.

### Niveau 3

Chercher dans le Tome II.

### Niveau 4

Chercher dans le Tome I et le corpus historique.

### Niveau 5

Si la question concerne l'Apple IIGS ou ORCA/C, consulter le corpus technique de plateforme.

### Niveau 6

Si aucune réponse n'existe, effectuer un choix d'implémentation.

Ce choix doit être documenté avant d'être codé.

Ne jamais modifier silencieusement la spécification pour résoudre un problème d'implémentation.

## 19. Interdictions

L'implémentation ne doit pas :

* introduire une interface graphique ;
* dépendre de la Toolbox ;
* imposer macOS, Windows ou Unix ;
* utiliser une bibliothèque moderne non disponible sur la cible ;
* supposer une mémoire abondante ;
* remplacer les fichiers L.S.E. par une base de données ;
* remplacer la représentation compilée par un langage intermédiaire moderne ;
* remplacer l'interpréteur par une machine virtuelle moderne générique ;
* supprimer une fonctionnalité historique simplement parce qu'elle paraît inutile ;
* réinterpréter les commandes L.S.E. selon une syntaxe moderne ;
* modifier une exigence EXI pour simplifier le code.

## 20. Livrables

La réalisation doit produire au minimum :

1. le code source C ;
2. les fichiers d'en-tête ;
3. les scripts ou commandes de construction ORCA/C ;
4. les fichiers nécessaires au lancement depuis ORCA ;
5. les structures de données documentées ;
6. les décisions `IMP-*` ;
7. les tests ;
8. les résultats de tests ;
9. la matrice `EXI → code → test` ;
10. la documentation de construction ;
11. la documentation d'installation sur Apple IIGS ;
12. la documentation de limitation de la première version.

## 21. Critère de conformité

Une version n'est pas considérée comme conforme simplement parce qu'elle :

* compile ;
* s'exécute sur Apple IIGS ;
* accepte des programmes L.S.E.

Elle doit simultanément :

1. respecter les exigences EXI applicables ;
2. réaliser les mécanismes VM ;
3. respecter les responsabilités du Tome III ;
4. respecter les contrats du Tome IV ;
5. préserver les invariants ;
6. fonctionner dans les contraintes matérielles de la cible ;
7. être traçable jusqu'au code ;
8. ne contenir aucune modification historique non justifiée.

## 22. Corpus documentaire obligatoire

### Niveau A — Normatif

Ces documents constituent la spécification obligatoire :

1. **Référentiel normatif LSE**

   * conventions éditoriales ;
   * Catalogue des exigences consolidé EXI-0001 à EXI-0062.

2. **Tomes I-II — Architecture historique et machine abstraite reconstruite**

3. **Tome III — Machine virtuelle + diagramme des processus**

4. **Tome IV — Interfaces, formats et algorithmes**

Le Tome III impose déjà que l'implémentation soit dérivable de la spécification sans redéterminer les composants, responsabilités, données, interfaces, états et invariants.

### Niveau B — Historique

5. **Corpus historique LSE**

Il contient notamment :

* INRDP 1975 ;
* LSE 15 ;
* Noyelle 1989 (nom du fichier dans le corpus : La Saga du LSE et de sa famille (LSD/LSG/LST));
* les sources historiques utilisées pour établir HIS.

Le manuel INRDP75 documente notamment les versions historiques T1600 et Mitra 15 et les processeurs du système L.S.E.

### Niveau C — Plateforme Apple IIGS

6. Documentation Apple IIGS pertinente :

   * architecture 65816 ;
   * mémoire ;
   * système d'exploitation ;
   * environnement shell ;
   * entrées/sorties texte ;
   * système de fichiers ;
   * conventions de programmes shell ;
   * appels système nécessaires.

7. Documentation ORCA/C 2.2.1 :

   * manuel ;
   * bibliothèque C ;
   * limitations ;
   * extensions ;
   * conventions de compilation ;
   * édition de liens ;
   * format des exécutables ;
   * modèle mémoire ;
   * options du compilateur ;
   * tests.

Le dépôt officiel ORCA/C contient notamment les sources, les tests, les bibliothèques et un fichier de notes spécifique `cc.2.2.1.notes`, ce qui en fait une source technique particulièrement importante pour cette étape.

8. Documentation ORCA Shell :

   * commandes ;
   * lancement d'exécutables ;
   * arguments ;
   * environnement ;
   * fichiers ;
   * redirections si disponibles ;
   * conventions de retour.

### Niveau D — Outils de validation

9. Documentation de l'émulateur Apple IIGS retenu.

10. Outils permettant :

* transfert des exécutables ;
* création d'images disque ;
* observation de la mémoire ;
* débogage ;
* comparaison des fichiers ;
* automatisation des tests.

## 23. Organisation recommandée du corpus

Le corpus d'implémentation doit être organisé en cinq ensembles :

```text
01-NORMATIF/
    Referentiel-normatif-LSE.md
    Tome-I.md
    Tome-II.md
    Tome-III.md
    Tome-IV.md

02-HISTORIQUE/
    LSE-Manuel-d-utilisation-INRDP-1975.md
    LSE15-Manuel-Mitra15-EAT-1978.md

03-APPLE-IIGS/
    architecture/
    systeme/
    shell/
    fichiers/
    terminal/
    outils/

04-ORCA-C/
    manuel/
    bibliotheques/
    notes/
    tests/
    exemples/
    sources/

05-IMPLEMENTATION/
    decisions/
    tests/
    matrice-tracabilite/
    contraintes/
```

## 24. Règle concernant le corpus historique

Le corpus historique n'est pas une seconde spécification.

Il sert à :

* vérifier une interprétation ;
* retrouver une information dont le Tome IV ne donne que la conséquence ;
* analyser un cas non couvert ;
* construire les tests historiques ;
* résoudre une ambiguïté documentaire.

Il ne peut pas être utilisé pour contredire silencieusement les tomes de conception.

## 25. Règle concernant la documentation Apple IIGS

La documentation de la plateforme ne doit jamais être utilisée pour redéfinir le comportement du L.S.E.

Elle sert uniquement à répondre à la question :

> « Comment réaliser physiquement sur Apple IIGS le comportement déjà spécifié ? »

## 26. Règle concernant ORCA/C

ORCA/C est un moyen d'implémentation, pas une référence normative du L.S.E.

Une limitation d'ORCA/C doit donc conduire à :

1. rechercher une autre réalisation compatible ;
2. isoler l'adaptation dans une couche spécifique ;
3. documenter la décision ;
4. vérifier que le comportement L.S.E. reste inchangé.

Une limitation du compilateur ne constitue jamais, à elle seule, une justification pour modifier la spécification.

## 27. Première version cible

La première version doit privilégier :

* fonctionnement fiable ;
* conformité fonctionnelle ;
* faible consommation mémoire ;
* simplicité ;
* traçabilité ;
* possibilité de débogage sur cible.

Les optimisations prématurées sont secondaires.

La priorité est de construire une machine L.S.E. fidèle et vérifiable.

## 28. Résultat attendu

À la fin de la mission, disposer d'un véritable environnement L.S.E. exécutable sur Apple IIGS depuis ORCA, dans une configuration de 4 Mio de RAM et 32 Mio de disque, avec :

* dialogue conversationnel ;
* analyse des commandes ;
* saisie et gestion des programmes ;
* compilation ligne par ligne ;
* représentation compilée ;
* interprétation ;
* décompilation ;
* variables ;
* expressions ;
* procédures et fonctions ;
* chaînes ;
* tableaux ;
* fichiers-programmes ;
* fichiers-données ;
* erreurs ;
* mécanismes de conservation et de reprise lorsque applicables.

L'implémentation doit rester reconnaissable comme la réalisation physique de la machine reconstruite, et non comme une nouvelle implémentation moderne du langage.

# Fin du conditionnement