# Tome IV — Interfaces, formats et algorithmes

## Préface

Le présent tome appartient à la collection *Reconstruction historique du Langage Symbolique d'Enseignement (L.S.E.)*.

Il constitue le niveau de spécification détaillée situé immédiatement après le Tome III — *Machine virtuelle*.

La hiérarchie documentaire applicable est la suivante :

1. Conventions éditoriales ;
2. Catalogue des exigences consolidé ;
3. Tome I — Architecture historique du système L.S.E. ;
4. Tome II — Machine abstraite reconstruite ;
5. Tome III — Machine virtuelle ;
6. présent Tome IV.

Le Tome I constitue la référence historique.

Le Catalogue des exigences constitue la référence normative des propriétés à satisfaire.

Le Tome II constitue la spécification fonctionnelle.

Le Tome III constitue la spécification d'architecture logique.

Le présent tome définit les interfaces, formats, contrats, algorithmes et séquences nécessaires pour permettre une réalisation conforme au Tome III.

Il ne constitue pas une implémentation.

Il ne dépend d'aucun langage de programmation, système d'exploitation, processeur ou organisation matérielle particulière.

Lorsqu'une information historique est directement documentée, elle est conservée comme telle.

Lorsqu'une propriété résulte nécessairement des tomes précédents, elle est qualifiée de **Déduite**.

Lorsqu'un détail ne peut être déterminé historiquement mais doit être fixé pour rendre la réalisation possible, il est qualifié de **Choix de conception**.

Aucune décision du présent tome ne peut modifier une propriété historique, une exigence, un mécanisme du Tome II ou une responsabilité du Tome III.

---

# 0. Objet et positionnement

## 0.1 Objet

Le présent tome précise la réalisation logique des contrats définis par le Tome III.

Il définit :

- les interfaces ;
- les paramètres ;
- les résultats ;
- les préconditions ;
- les postconditions ;
- les erreurs ;
- les structures logiques ;
- les formats ;
- les règles de validation ;
- les transformations ;
- les algorithmes ;
- les séquences d'échanges ;
- les invariants.

## 0.2 Limites

Le présent tome ne définit pas :

- du code dans un langage particulier ;
- une API moderne ;
- une bibliothèque logicielle ;
- une organisation physique de la mémoire ;
- un format binaire historique non documenté ;
- une représentation matérielle particulière.

## 0.3 Principe de réalisation

Une réalisation conforme doit pouvoir être conçue à partir des Tomes I à IV sans devoir redéterminer :

- les responsabilités des composants ;
- leurs contrats ;
- les structures logiques ;
- les formats nécessaires ;
- les algorithmes fondamentaux ;
- les séquences d'échanges ;
- les erreurs ;
- les invariants.

Les choix techniques de réalisation restent hors périmètre.

## 0.4 Statut général

**Choix de conception.**

Ce statut concerne uniquement l'organisation détaillée introduite par le Tome IV. Les éléments historiques ou normatifs auxquels cette organisation se rattache conservent leur propre statut.

## 0.5 Traçabilité

`Conventions → EXI → Tome II → Tome III → Tome IV`

La traçabilité historique est conservée lorsque la source correspondante existe :

`HIS → EXI → VM → interface/format/algorithme`

---

# 1. Principes d'architecture détaillée

## 1.1 Conservation des responsabilités

Chaque interface appartient à une responsabilité définie au Tome III.

Le Tome IV ne crée pas de responsabilité fonctionnelle nouvelle lorsque celle-ci peut être couverte par un composant existant.

Une interface ne doit pas permettre à un composant de contourner la responsabilité d'un autre.

**Statut : Choix de conception.**

## 1.2 Distinction logique et physique

Toute structure est d'abord définie logiquement.

Une représentation physique n'est imposée que lorsque cela est nécessaire à la cohérence de la reconstruction.

Lorsqu'aucune représentation physique historique n'est documentée, la spécification définit seulement les propriétés logiques nécessaires.

**Statut : Choix de conception.**

## 1.3 Distinction des états

Les catégories suivantes restent distinctes :

- programme courant ;
- représentation compilée ;
- état d'exécution ;
- fichier-programme ;
- fichier-données temporaire ;
- fichier-données permanent ;
- état d'une tâche.

La conservation d'une tâche n'est pas une opération de rangement d'un fichier-programme.

Cette distinction est imposée par les Tomes I à III. Le système historique décrit notamment une zone de swap distincte des zones de fichiers et une conservation de l'état des tâches. [INRDP75 §3.1 ; §8-1]

## 1.4 Atomicité logique

Lorsqu'une opération modifie un objet architectural, la modification est considérée comme acquise seulement lorsque toutes les validations nécessaires ont réussi.

Une erreur ne doit pas laisser un état partiellement publié.

**Statut : Déduit.**

## 1.5 Conservation des invariants

Toute opération doit préserver :

- l'intégrité du programme courant ;
- l'intégrité de la représentation compilée ;
- la séparation programme/exécution ;
- la séparation fichier/tâche ;
- la cohérence du catalogue ;
- la validité des références ;
- la cohérence de l'état d'exécution.

---

# 2. Interfaces inter-composants

## 2.1 Principe

Les interfaces sont logiques.

Elles sont exprimées sous la forme :

`OPERATION(paramètres) → résultat`

Une erreur est un résultat possible de l'opération.

Aucune représentation physique de l'appel n'est imposée.

## 2.2 Interface de compilation

### IV-IF-0001 — COMPILER_LIGNE

**Fournisseur :** compilateur  
**Consommateur :** moniteur / analyseur de commandes

**Entrées :**

- ligne de programme ;
- contexte de compilation nécessaire.

**Résultat :**

- objet compilé accepté ;
- ou diagnostic.

**Préconditions :**

- la ligne est identifiée comme ligne de programme ;
- son contenu est disponible.

**Postconditions :**

En cas de succès, un objet compilé valide est produit.

En cas d'échec, aucun objet compilé n'est publié dans le programme courant.

**Erreurs :**

- erreur syntaxique ;
- représentation impossible ;
- référence syntaxiquement invalide.

**Invariants :**

Une erreur de compilation ne modifie pas le programme courant.

**Statut : Déduit.**

**Traçabilité :** HIS-6.2 → EXI-0001, EXI-0002 → VM-0301 à VM-0306.

## 2.3 Interface du programme courant

### IV-IF-0002 — INSERER_LIGNE

**Fournisseur :** gestionnaire du programme courant  
**Consommateur :** compilateur

**Entrées :**

- numéro de ligne ;
- objet compilé.

**Résultat :**

- programme courant modifié.

**Règles :**

- si le numéro n'existe pas, l'objet est ajouté ;
- s'il existe, l'objet précédent est remplacé ;
- si le numéro est supprimé selon les règles du langage, l'objet correspondant est retiré.

**Invariants :**

- unicité des numéros ;
- ordre logique par numéro ;
- intégrité de chaque objet compilé.

## 2.4 Interface d'exécution

### IV-IF-0003 — EXECUTER_PROGRAMME

**Fournisseur :** moteur d'exécution  
**Consommateur :** moniteur

**Entrées :**

- programme courant ;
- point de départ éventuel ;
- contexte d'exécution initial ou restauré.

**Résultat :**

- état d'exécution ;
- résultat ;
- suspension ;
- erreur ;
- terminaison.

**Précondition :**

Le programme courant doit être cohérent.

**Postcondition :**

Le programme courant demeure inchangé par la seule exécution.

## 2.5 Interface de décompilation

### IV-IF-0004 — DECOMPILER

**Fournisseur :** décompilateur  
**Consommateur :** moniteur / bibliothécaire

**Entrée :**

- objet compilé ou programme courant.

**Résultat :**

- représentation textuelle.

**Invariant :**

La représentation produite doit correspondre au programme compilé décompilé.

**Statut : Déduit.**

## 2.6 Interface des fichiers-données

### IV-IF-0005 — LIRE_DONNEE

**Fournisseur :** gestionnaire des fichiers-données  
**Consommateur :** instruction CHARGER / programme

**Entrée :**

- fichier.

**Résultat :**

- enregistrement désigné ;
- type ;
- contenu.

**Précondition :**

Le fichier doit être accessible dans la portée applicable.

**Postcondition :**

Aucun état du fichier n'est modifié par la seule lecture.

**Erreurs :**

- fichier inexistant ;
- enregistrement inexistant ;
- accès interdit ;
- contenu invalide.

**Invariant :**

La lecture restitue le type et le contenu de l'enregistrement désigné sans modifier le fichier.

### IV-IF-0006 — ECRIRE_DONNEE

**Fournisseur :** gestionnaire des fichiers-données  
**Consommateur :** instruction GARER / programme

**Entrées :**

- fichier ;
- enregistrement désigné ;
- valeur.

**Résultat :**

- succès ;
- ou diagnostic.

**Précondition :**

Le fichier doit être accessible et la valeur doit être représentable selon les contraintes logiques du fichier. Si l'écriture porte sur un fichier permanent, l'autorisation correspondante doit être satisfaite.

**Postcondition :**

L'enregistrement désigné contient la valeur publiée, ou aucune modification n'est publiée en cas d'échec.

**Erreurs :**

- fichier inaccessible ;
- désignation invalide ;
- valeur invalide ;
- impossibilité de publier la modification.

**Invariant :**

Une écriture échouée ne laisse pas d'enregistrement partiellement modifié.

## 2.7 Interface du catalogue

### IV-IF-0007 — RECHERCHER_FICHIER

**Fournisseur :** catalogue  
**Consommateur :** bibliothécaire / gestionnaire des fichiers-données

**Entrée :**

- identité logique du fichier.

**Résultat :**

- descripteur du fichier ;
- ou absence.

### IV-IF-0010 — TRANSFORMER_PROGRAMME_DONNEE

**Fournisseur :** transformateur programme-donnée  
**Consommateur :** bibliothécaire / moniteur

**Opérations :**

`DECODER(programme_courant, destination)`

`ENCODER(source, programme_courant)`

**Résultat :**

- fichier-données produit par DECODER ;
- programme courant produit ou mis à jour selon les règles d'acceptation par ENCODER ;
- ou diagnostic.

**Préconditions :**

- les objets d'entrée sont accessibles ;
- le programme courant est cohérent pour DECODER ;
- le fichier source est lisible pour ENCODER.

**Postconditions :**

Une transformation réussie publie uniquement un résultat valide.

Une transformation échouée ne publie pas de résultat partiellement valide.

**Erreurs :**

- source inaccessible ;
- contenu invalide ;
- erreur de décompilation ;
- erreur de compilation ;
- erreur de fichier.

**Invariant :**

DECODER ne modifie pas le programme courant ; ENCODER utilise les règles normales de compilation et d'acceptation.

**Traçabilité :**

HIS-11.5 → EXI-0057 → VM-3406.

### IV-IF-0011 — CATALOGUER_FICHIER_DONNEE

**Fournisseur :** catalogueur de fichier-données  
**Consommateur :** bibliothécaire / commande CATALOGUER

**Entrée :**

- fichier-données temporaire ;
- identité permanente.

**Résultat :**

- fichier-données permanent catalogué ;
- ou diagnostic.

**Préconditions :**

- le fichier temporaire appartient au contexte courant ;
- son contenu est valide ;
- l'identité permanente est admissible.

**Postconditions :**

En cas de succès :
1. la représentation permanente est créée ;
2. l'entrée est inscrite au catalogue ;
3. l'entrée temporaire est supprimée ;
4. l'espace temporaire est libéré.

En cas d'échec, aucune publication permanente partielle n'est autorisée.

**Invariant :**

Le contenu logique du fichier reste inchangé par le seul catalogage.

**Traçabilité :**

HIS-11.3 → EXI-0056 → VM-3407.

Le catalogue distingue :

- fichier-programme ;
- fichier-données ;
- temporaire ;
- permanent.

Les sources historiques établissent cette distinction et associent des espaces distincts aux fichiers temporaires, permanents et à la zone de swap. [INRDP75 §1-2 ; §1-3]

## 2.8 Interface de temps partagé

### IV-IF-0008 — SWAP_OUT

**Fournisseur :** gestionnaire des tâches  
**Consommateur :** conservation de l'état

**Entrée :**

- tâche active.

**Résultat :**

- tâche conservée.

**Postcondition :**

L'état nécessaire à la reprise est conservé.

**Invariant :**

La suspension ne détruit pas l'état d'exécution.

### IV-IF-0009 — SWAP_IN

**Fournisseur :** gestionnaire des tâches  
**Consommateur :** environnement d'exécution

**Entrée :**

- tâche conservée.

**Résultat :**

- tâche active.

**Postcondition :**

L'état précédemment conservé est restauré.

Le manuel historique décrit explicitement les opérations SWAP IN et SWAP OUT et la conservation de l'image des tâches dans la zone de swap. [INRDP75 §3.1 ; §8-1]


### IV-IF-0013 — ANALYSER_COMMANDE

**Fournisseur :** analyseur de commandes

**Consommateur :** moniteur / processeur destinataire

**Entrées :**

- ligne de commande complète ;
- état du mode abrégé ;
- contexte de dialogue.

**Sorties :**

- commande reconnue ;
- paramètres éventuels ;
- processeur ou service destinataire ;
- diagnostic en cas de commande non reconnue.

**Préconditions :**

- la ligne est terminée par le caractère de fin de ligne ;
- le contexte de dialogue est valide.

**Postconditions :**

En cas de succès, la demande est suffisamment déterminée pour être routée sans nouvelle interprétation de sa nature.

En mode normal, les deux premières lettres constituent l'entrée abrégée ; le système complète le nom de la commande et reçoit ensuite les paramètres éventuels. Lorsque ABREGER est actif, la complétion d'affichage est supprimée sans modifier l'identification fonctionnelle de la commande. [INRDP75 §4]

**Erreurs :**

- commande inconnue ;
- paramètres incompatibles avec la commande ;
- contexte de dialogue incompatible.

**Invariant :**

L'analyse ne réalise pas l'opération demandée ; elle produit seulement la demande orientée.

**Statut : Déduit.**

**Traçabilité :** HIS-4.7.1, HIS-5.6 → EXI-0052 → VM-2001, VM-2502 → IV-IF-0013.

### IV-IF-0014 — GÉRER_IDENTIFICATION_ET_AUTORISATION

**Fournisseur :** gestionnaire des identifications et des autorisations

**Consommateur :** moniteur / bibliothécaire / gestionnaire des fichiers

**Entrées :**

- numéro de compte ;
- clé, lors de l'identification ;
- session ;
- fichier ;
- opération demandée.

**Sorties :**

- identification courante mise à jour ;
- autorisation accordée ou refusée ;
- diagnostic en cas d'identification illégale ou d'opération interdite.

**Préconditions :**

- la session est valide ;
- le fichier est identifié lorsqu'une opération sur fichier est demandée.

**Postconditions :**

En cas d'identification acceptée, l'identification courante devient celle qui vient d'être validée.

En cas de refus, l'identification courante antérieure reste inchangée.

En cas de contrôle d'accès réussi, l'opération peut être exécutée par le composant responsable. En cas de refus, aucune modification du fichier n'est publiée.

**Erreurs :**

- identification illégale ;
- identification absente pour une opération nécessitant un droit permanent ;
- pouvoir insuffisant ;
- protection du fichier incompatible avec l'opération ;
- identification différente de l'identification de création lorsqu'elle est requise.

**Invariant :**

Une clé d'identification n'est jamais produite dans les sorties ordinaires de l'interface.

**Statut :** Déduit.

**Traçabilité :** HIS-11.3.1 → EXI-0062 → VM-3408 → IV-IF-0014.

### IV-IF-0012 — REPRENDRE_EXECUTION

**Fournisseur :** gestionnaire de reprise  
**Consommateur :** environnement d'exécution / moniteur

**Entrée :**

- tâche suspendue ;
- état d'exécution conservé ;
- position logique de reprise.

**Résultat :**

- exécution reprise ;
- ou diagnostic.

**Préconditions :**

- une tâche suspendue est identifiée ;
- son état conservé est disponible ;
- la cohérence de cet état peut être vérifiée.

**Postconditions :**

En cas de succès, l'état d'exécution est restauré avant la poursuite à la position logique conservée.

En cas d'échec, la tâche ne devient pas active avec un état partiellement restauré.

**Effets de bord :**

- restauration de l'état d'exécution ;
- réactivation de la tâche ;
- poursuite de l'exécution.

**Erreurs :**

- tâche inexistante ;
- état conservé indisponible ;
- état incohérent ;
- position de reprise invalide.

**Invariant :**

La reprise ne modifie pas la représentation compilée du programme courant et ne recommence pas l'exécution avant la position conservée, sauf si cette position l'impose.

**Traçabilité :**

HIS-13.1, HIS-13.3, HIS-13.4 → EXI-0060 → VM-2106.

---

# 3. Contrats et conventions d'interface

## 3.1 Convention des paramètres

Les paramètres sont identifiés par leur rôle logique.

Un paramètre ne désigne jamais directement une adresse physique.

## 3.2 Convention des résultats

Une opération réussie produit un résultat conforme à son domaine.

Une opération échouée produit un diagnostic et ne publie pas de résultat partiellement valide.

## 3.3 Convention des erreurs

Une erreur est caractérisée au minimum par :

- catégorie ;
- opération concernée ;
- contexte ;
- information permettant le diagnostic.

La forme physique du diagnostic n'est pas imposée.

## 3.4 Convention d'identification des fichiers

L'identité logique d'un fichier comprend au minimum :

- son nom ;
- son type ;
- son statut temporaire ou permanent ;
- le contexte utilisateur nécessaire lorsque celui-ci est applicable.

Les sources historiques indiquent que les noms de fichiers suivent une règle de formation comportant une lettre suivie d'au plus quatre caractères alphanumériques. [INRDP75 §Fichiers]

## 3.5 Convention d'identification des tâches

Une tâche est identifiée indépendamment du programme qu'elle exécute.

L'identité d'une tâche ne peut donc pas être déduite du nom d'un fichier-programme.

## 3.6 Convention de publication

Une opération qui remplace une structure existante suit logiquement :

1. validation ;
2. préparation ;
3. publication ;
4. libération éventuelle de l'ancienne structure.

Cette règle est particulièrement applicable au remplacement du programme courant et au chargement d'un programme. Le Tome III impose déjà que le programme courant observable reste cohérent pendant ces opérations.

**Statut : Déduit.**

## 3.7 Invariants généraux

- une interface ne modifie pas la responsabilité d'un autre composant ;
- un résultat invalide n'est jamais publié ;
- une erreur est explicite ;
- programme, fichier et tâche restent distincts ;
- aucune interface ne dépend d'une technologie particulière.

---

# 4. Formats des objets

## 4.1 Objet compilé

Un objet compilé comprend logiquement :

- numéro de ligne ;
- représentation compilée de la ligne ;
- informations nécessaires à son exécution ;
- informations nécessaires à sa décompilation lorsque celles-ci ne peuvent être déduites.

**Statut : Déduit.**

Le format physique exact n'est pas historiquement documenté.

## 4.2 Référence

Une référence logique identifie un objet sans imposer sa représentation physique.

Elle peut désigner :

- variable ;
- tableau ;
- chaîne ;
- procédure ;
- fonction ;
- ligne ;
- objet compilé ;
- fichier.

## 4.3 Valeur

Une valeur logique possède :

- un type ;
- un contenu ;
- éventuellement une dimension ou une longueur ;
- les attributs nécessaires à sa validation.

## 4.4 Constante

Une constante est représentée logiquement par :

- type ;
- valeur.

Le Tome IV ne fixe pas un codage physique particulier des nombres.

## 4.5 Tableau

Un tableau possède logiquement :

- type des éléments ;
- nombre de dimensions ;
- bornes ;
- valeurs des éléments.

Les bornes doivent être conservées avec suffisamment de précision pour permettre :

- l'accès ;
- la validation des indices ;
- la décompilation lorsque nécessaire.

## 4.6 Chaîne

Une chaîne possède :

- longueur logique ;
- suite ordonnée de caractères.

La représentation physique des caractères n'est pas imposée au-delà des contraintes historiques explicitement documentées.

## 4.7 Cadre d'activation

Un cadre d'activation contient logiquement les informations nécessaires à :

- l'appel ;
- l'environnement local ;
- les paramètres ;
- le retour ;
- la poursuite de l'exécution.

Le cadre d'activation est distinct du programme courant.

## 4.8 État d'exécution

L'état d'exécution contient au minimum :

- position d'exécution ;
- cadres d'activation ;
- pile d'évaluation ;
- environnement d'exécution ;
- informations nécessaires à la reprise.

Cette structure est déduite du Tome II et du Tome III.

## 4.9 Descripteur de fichier

Le descripteur logique d'un fichier comprend :

- identité ;
- catégorie ;
- statut ;
- organisation logique ;
- accès autorisés ;
- contenu logique ou référence vers celui-ci.

Le format physique n'est pas déterminé historiquement.

## 4.10 Format logique de l'identification et des droits

Une identification logique comprend :

- un numéro de compte ;
- une clé associée, utilisée pour la validation mais non destinée à l'affichage ;
- les attributs de droits associés au numéro de code.

Pour un fichier permanent, le descripteur logique conserve :

- l'identification de création ;
- la catégorie du fichier ;
- les protections applicables ;
- les opérations autorisées selon l'identification courante et le profil d'implantation.

Les attributs historiques documentés concernent notamment la création, modification et suppression de fichiers-programmes, la création et suppression de fichiers-données permanents et certaines extensions de l'espace temporaire. [INRDP75 §Fichiers]

**Règle de validation :** une identification refusée ne devient pas l'identification courante.

**Règle de protection :** la modification ou la suppression d'un fichier permanent exige l'identification de création, conformément aux règles historiques. Les protections de lecture ou d'exécution sont appliquées selon le fichier et le profil historique concerné.

**Statut :** Déduit.

---

# 5. Format de la Chaîne Post-Fixée

## 5.1 Principe historique

Le manuel de 1975 documente une **chaîne codée** produite par compilation, exécutée par l'interpréteur et traitée par le décompilateur.

Yves Noyelle décrit explicitement, pour le LSD, une **chaîne post-fixée** en notation polonaise inverse, présentée comme adaptée à l'interprétation, compacte et décompilable. [INRDP75 §3.3 ; NOYELLE89 p.219-220]

Ces deux constatations doivent rester distinctes.

Le Tome IV ne présente donc pas comme historiquement établi que la chaîne codée L.S.E. de 1975 ou 1978 est identique à la chaîne post-fixée du LSD.

## 5.2 Représentation retenue

La représentation post-fixée constitue le modèle de reconstruction de la représentation compilée.

Elle est composée logiquement d'une séquence d'éléments ordonnés.

La séquence possède au minimum :

- une position logique courante ;
- un premier élément ;
- un élément suivant ;
- une indication de fin de séquence ;
- l'accès à l'élément courant.

Chaque élément appartient à une catégorie minimale :

- valeur ;
- référence ;
- opération codée ;
- information de contrôle lorsque nécessaire.

Le parcours logique peut être décrit par les opérations :

`PREMIER_ELEMENT(sequence)`

`ELEMENT_SUIVANT(sequence, position)`

`LIRE_ELEMENT(sequence, position)`

`FIN_SEQUENCE(sequence, position)`

Ces opérations sont logiques ; elles n'imposent aucune adresse physique ni aucun codage particulier.

**Statut : Déduit pour les propriétés de parcours ; Choix de conception pour les noms des opérations.**

## 5.3 Opération codée

Une opération codée identifie une opération sémantique déjà définie par le Tome II.

Elle ne constitue pas nécessairement une instruction machine.

Le codage physique de l'identifiant n'est pas fixé.

## 5.4 Déroulement

Une représentation post-fixée peut être parcourue séquentiellement.

Lorsqu'une valeur est rencontrée, elle est déposée sur la pile d'évaluation.

Lorsqu'une référence est rencontrée, elle est résolue conformément à l'environnement d'exécution.

Lorsqu'une opération est rencontrée, elle consomme les valeurs nécessaires et produit son résultat.

Cette organisation est la reconstruction minimale compatible avec l'utilisation d'une représentation post-fixée par un interpréteur.

**Statut : Déduit.**

## 5.5 Décompilation

Le décompilateur parcourt la représentation compilée et reconstruit une représentation textuelle conforme au langage.

Les éléments syntaxiques non conservés doivent être reconstruits à partir des informations disponibles.

## 5.6 Compacité

La représentation doit éviter la conservation redondante des éléments textuels inutiles à l'exécution.

La propriété quantitative historique d'une représentation environ deux fois moins encombrante que le texte source appartient à la description du LSD et ne constitue pas une garantie numérique générale du présent modèle. [NOYELLE89 p.220]

**Statut : Probable pour le principe ; non documenté pour un taux précis du L.S.E.**

## 5.7 Format physique

Aucun format physique historique complet n'est établi par les sources retenues.

Il est donc interdit de fixer :

- taille d'un élément ;
- taille d'un code ;
- organisation binaire ;
- octet de contrôle ;
- adresse physique ;

comme faits historiques.

**Statut : Non déterminé.**

## 5.8 Invariants

- toute opération codée appartient au domaine défini ;
- toute référence est valide dans son contexte ;
- la représentation possède un ordre total des éléments ;
- chaque position valide désigne au plus un élément ;
- le parcours séquentiel atteint la fin de séquence sans ambiguïté ;
- l'interpréteur et le décompilateur utilisent la même représentation logique ;
- aucune position physique n'est exigée par le format logique.

---

# 6. Formats des fichiers-programmes

## 6.1 Nature

Le fichier-programme contient une représentation codée du programme courant.

La commande RANGER conserve le programme courant sous forme codée.

La commande APPELER recharge ce programme et le rend programme courant. [INRDP75 §4-4 ; LSE15 §9-5]

## 6.2 Structure logique

Un fichier-programme comprend logiquement :

- identité du fichier ;
- collection ordonnée d'objets compilés ;
- informations nécessaires à leur identification ;
- informations de contrôle nécessaires à la restauration.

**Statut : Déduit.**

## 6.3 Format physique

Le format physique historique détaillé n'est pas établi.

Le Tome IV ne définit donc pas de structure binaire.

## 6.4 RANGER

### ALG-0601 — RANGER

**Entrée :**

- programme courant ;
- identité du fichier.

**Sortie :**

- fichier-programme créé.

**Préconditions :**

- programme courant cohérent ;
- identité valide ;
- droits requis satisfaits lorsqu'ils sont nécessaires
- fichier-programme inexistant.

**Étapes :**

1. identifier le programme courant ;
2. vérifier l'autorisation de création du fichier permanent lorsque celle-ci est requise ;
3. préparer sa représentation de conservation ;
4. associer cette représentation à l'identité du fichier ;
5. publier le fichier ;
6. signaler le succès.

**Erreur :**

Le fichier existant ne doit pas être détruit si la nouvelle conservation échoue.

**Invariant :**

Le programme courant n'est pas modifié.

**Statut : Déduit.**

## 6.5 APPELER

### ALG-0602 — APPELER

**Entrée :** identité du fichier-programme.

**Sortie :** nouveau programme courant.

**Étapes :**

1. rechercher le fichier ;
2. vérifier qu'il s'agit d'un fichier-programme ;
3. vérifier l'autorisation de lecture lorsque le fichier est protégé ;
4. charger sa représentation ;
5. valider les objets compilés ;
6. préparer le programme courant ;
7. publier le nouveau programme courant.

**Erreur :**

Un fichier invalide ne remplace pas le programme courant.

## 6.6 MODIFIER

La commande MODIFIER est logiquement équivalente à une suppression du fichier-programme suivie d'un nouveau rangement du programme courant, sous réserve des règles historiques d'identification et d'accès. Le contrôle de l'identification de création et des pouvoirs requis précède la publication du remplacement. [INRDP75 §Fichiers]

## 6.7 SUPPRIMER

### ALG-0603 — SUPPRIMER_FICHIER_PROGRAMME

1. rechercher le fichier ;
2. vérifier son type ;
3. vérifier l'identification de création et les pouvoirs requis ;
4. supprimer l'entrée du catalogue ;
5. libérer sa conservation.

**Invariant :**

La suppression d'un fichier-programme ne modifie pas le programme courant.

---

# 7. Formats des fichiers-données

## 7.1 Distinction historique

Les fichiers-données sont distincts des fichiers-programmes.

Le manuel de 1975 indique que les instructions portant sur les fichiers sont principalement utilisées pour les fichiers-données, tandis que les commandes portant sur les fichiers concernent notamment les fichiers-programmes. [INRDP75 §Fichiers]

Les fichiers-données peuvent être :

- temporaires ;
- permanents.

## 7.2 Fichier-données temporaire

Un fichier-données temporaire est associé au contexte utilisateur et à la session correspondante.

Les sources indiquent que les fichiers temporaires peuvent porter le même nom sur plusieurs consoles. [INRDP75 §1-2–§1-3]

## 7.3 Fichier-données permanent

Un fichier-données permanent appartient à la bibliothèque et doit être distingué des autres fichiers permanents.

## 7.4 Enregistrement

L'enregistrement constitue l'unité logique de données manipulée par GARER et CHARGER.

Le Tome III retient qu'un enregistrement peut contenir logiquement :

- une constante ;
- un tableau à une ou deux dimensions ;
- une chaîne.

Le type d'un enregistrement est conservé.

**Statut : Déduit.**

## 7.5 Format logique d'un enregistrement

```text
ENREGISTREMENT
    identité
    type
    dimensions éventuelles
    longueur éventuelle
    contenu
```

Cette structure est logique.

Elle ne constitue pas un format physique historique.

## 7.6 Constantes

Une constante est conservée avec son type et sa valeur.

Le codage physique du type et de la valeur n'est pas documenté.

## 7.7 Tableaux

Un tableau conserve :

- type ;
- dimensions ;
- bornes ;
- éléments.

Le format physique des éléments n'est pas historiquement établi.

## 7.8 Chaînes

Une chaîne conserve :

- longueur ;
- caractères.

Elle peut être rangée dans un enregistrement et rechargée par CHARGER.

## 7.9 GARER

### ALG-0701 — GARER

**Entrées :**

- fichier ;
- numéro d'enregistrement ;
- valeur.

**Étapes :**

1. résoudre le fichier ;
2. vérifier son accessibilité ;
3. vérifier le numéro ;
4. vérifier la valeur ;
5. construire l'enregistrement logique ;
6. remplacer ou créer l'enregistrement selon les règles du Tome III ;
7. publier la modification.

**Erreur :**

Aucune écriture partielle ne doit être publiée.

**Traçabilité :**

HIS-11.4 → EXI-0056 → VM-3405 → ALG-0701.

## 7.10 CHARGER

### ALG-0702 — CHARGER

**Entrées :**

- fichier ;
- numéro d'enregistrement.

**Sorties :**

- valeur ;
- type.

**Étapes :**

1. résoudre le fichier ;
2. vérifier l'accès ;
3. rechercher l'enregistrement ;
4. vérifier son intégrité ;
5. restituer sa valeur.

**Erreur :**

- fichier inexistant ;
- enregistrement inexistant ;
- contenu invalide.

## 7.11 SUPPRIMER

### ALG-0703 — SUPPRIMER

Selon le contexte :

- supprimer un enregistrement ;
- supprimer un fichier-données temporaire ;
- supprimer un fichier-données permanent.

Le niveau de suppression doit être explicitement déterminé avant l'opération.

## 7.12 CATALOGUER

### ALG-0704 — CATALOGUER

**Entrée :**

- fichier-données temporaire ;
- identité permanente.

**Préconditions :**

- le fichier temporaire appartient au contexte courant ;
- son contenu est valide ;
- l'identité permanente est admissible.

**Étapes :**

1. valider le fichier temporaire ;
2. créer la représentation permanente ;
3. l'enregistrer dans le catalogue ;
4. supprimer l'entrée temporaire ;
5. libérer l'espace temporaire.

La commande CATALOGUER transforme historiquement un fichier données temporaire en fichier données permanent. [INRDP75 §Fichiers ; LSE15 §9-5]

**Postconditions :**

En cas de succès, le fichier est accessible comme fichier permanent et l'espace temporaire correspondant est réutilisable.

En cas d'échec, aucune représentation permanente partiellement cataloguée n'est publiée.

**Erreurs :**

- fichier temporaire inexistant ;
- contenu invalide ;
- identité permanente inadmissible ;
- impossibilité d'inscrire le fichier au catalogue.

**Invariant :**

Le contenu logique du fichier reste inchangé par le seul catalogage.

**Traçabilité :**

HIS-11.3 → EXI-0056 → VM-3407.

## 7.13 DECODER

### ALG-0705 — DECODER

**Entrée :**

- programme courant ;
- destination.

**Sortie :**

- fichier-données temporaire contenant la représentation textuelle du programme.

**Préconditions :**

- le programme courant est cohérent ;
- la destination est disponible.

**Étapes :**

1. parcourir le programme courant ;
2. décompiler chaque ligne ;
3. produire les lignes textuelles ;
4. créer le fichier-données temporaire ;
5. enregistrer les lignes ;
6. publier le fichier.

**Postcondition :**

Le fichier produit contient la représentation textuelle des lignes du programme courant dans leur ordre logique.

Cette chaîne est explicitement documentée : DECODER utilise le décompilateur et range le résultat comme fichier données temporaire. [INRDP75 Annexe II]

**Erreur :**

Une erreur de décompilation ou de fichier ne doit pas publier un fichier temporaire incomplet.

**Traçabilité :**

HIS-11.5 → EXI-0057 → VM-3406.

## 7.14 ENCODER

### ALG-0706 — ENCODER

**Entrée :**

- source ;
- programme courant.

**Sortie :**

- programme courant résultant de l'acceptation des lignes valides ;
- ou diagnostic.

**Préconditions :**

- la source est accessible ;
- chaque enregistrement est susceptible de fournir une ligne textuelle.

**Étapes :**

1. ouvrir le fichier ;
2. lire les enregistrements dans l'ordre ;
3. interpréter chaque enregistrement comme ligne textuelle ;
4. transmettre chaque ligne au compilateur ;
5. appliquer les règles normales d'acceptation ou de rejet ;
6. publier chaque modification acceptée selon ces règles.

**Postcondition :**

Aucune ligne rejetée n'est ajoutée au programme courant.

ENCODER utilise historiquement le bibliothécaire et le compilateur. [INRDP75 Annexe II]

**Traçabilité :**

HIS-11.5 → EXI-0057 → VM-3406.

## 7.15 AU REVOIR

La commande AU REVOIR supprime les fichiers temporaires associés à l'utilisateur.

Elle ne doit pas être assimilée à la conservation ou à la suppression d'un fichier permanent.

[INRDP75 §Fichiers]

---

# 8. Formats des états d'exécution

## 8.1 État minimal

L'état d'exécution comprend :

- état de contrôle ;
- position courante ;
- pile d'évaluation ;
- cadres d'activation ;
- environnement d'exécution ;
- objets dynamiques nécessaires ;
- état d'erreur éventuel.

## 8.2 Position d'exécution

La position identifie logiquement :

- l'objet compilé courant ;
- la position dans sa représentation ;
- le contexte d'activation correspondant.

## 8.3 Cadres d'activation

Chaque cadre conserve les informations nécessaires à la poursuite d'un appel.

Il ne contient pas une copie du programme.

## 8.4 Pile d'évaluation

La pile conserve les valeurs intermédiaires nécessaires à l'évaluation.

Chaque élément possède un type logique.

## 8.5 Conservation

Une conservation d'état doit permettre une restauration fonctionnellement équivalente.

**Statut : Déduit.**

## 8.6 Format physique

Le format physique de l'état d'exécution n'est pas historiquement documenté.

Il est donc laissé à l'implémentation.

## 8.7 Invariants

- restauration → reprise au même point logique ;
- aucun cadre d'activation n'est perdu ;
- aucune valeur nécessaire à la poursuite n'est perdue ;
- l'état d'exécution reste distinct du programme courant.

---

# 9. Algorithmes de compilation

## 9.1 Compilation d'une ligne

### ALG-0901

**Entrée :** ligne.

**Sortie :** objet compilé ou diagnostic.

**Précondition :** ligne reçue complètement.

**Étapes :**

1. transmettre la ligne à l'analyseur du compilateur `VM-3202` ;
2. analyser sa structure ;
3. détecter les erreurs syntaxiques ;
4. produire la structure logique validée ;
5. transmettre cette structure au générateur de représentation compilée `VM-3203` ;
6. générer les valeurs, références, opérations codées et informations de contrôle nécessaires ;
7. valider la représentation compilée ;
8. retourner l'objet compilé.

**Postcondition :**

Un objet compilé valide est disponible si l'opération réussit.

**Invariant :**

Aucun résultat partiel de `VM-3202` ou `VM-3203` n'est publié dans le programme courant.

**Traçabilité :**

HIS-5.2, HIS-6.5 → EXI-0018, EXI-0051 → VM-3202, VM-1901.

HIS-7 → EXI-0002, EXI-0010 à EXI-0016 → VM-3203.

## 9.2 Acceptation d'une ligne

### ALG-0902

1. compiler ;
2. si erreur, produire le diagnostic et terminer ;
3. si suppression, retirer la ligne ;
4. sinon remplacer ou insérer l'objet compilé ;
5. rétablir l'ordre logique ;
6. publier le programme courant.

## 9.3 Compilation indépendante

La compilation d'une ligne ne recompilé pas les autres lignes.

Cette propriété est imposée par EXI-0001.

## 9.4 Compilation issue d'ENCODER

ENCODER réutilise le même algorithme de compilation que l'entrée interactive.

La seule différence est la source des lignes.

**Statut : Déduit.**

## 9.5 Erreur de compilation

Une erreur détectée avant acceptation entraîne :

- rejet de la ligne ;
- production du diagnostic ;
- absence de modification du programme courant.

---

# 10. Algorithmes de décompilation

## 10.1 Décompilation d'un objet

### ALG-1001

**Entrée :** objet compilé.

**Sortie :** ligne textuelle.

**Étapes :**

1. parcourir la représentation compilée ;
2. identifier chaque élément ;
3. reconstruire les représentations lexicales nécessaires ;
4. reconstruire l'ordre syntaxique ;
5. ajouter les éléments de ligne nécessaires ;
6. produire la ligne.

## 10.2 Décompilation du programme

### ALG-1002

1. parcourir les lignes du programme courant dans l'ordre ;
2. décompiler chaque objet ;
3. restituer les lignes.

## 10.3 Limite historique

La méthode exacte de décompilation historique n'est pas documentée.

La présente procédure décrit uniquement la transformation minimale requise par les contrats du Tome II et du Tome III.

**Statut : Choix de conception.**

---

# 11. Algorithmes d'exécution

## 11.1 Initialisation

### ALG-1101

1. vérifier le programme ;
2. créer l'état d'exécution ;
3. positionner le point de départ ;
4. initialiser la pile ;
5. créer le contexte initial ;
6. passer à l'exécution.

## 11.2 Interprétation

### ALG-1102

```text
TANT QUE état = EXECUTION
    élément ← lire l'élément courant
    SI élément = valeur
        déposer valeur
    SINON SI élément = référence
        résoudre référence
        déposer résultat
    SINON SI élément = opération
        exécuter opération
    SINON
        produire erreur
    FIN SI
    mettre à jour la position
FIN TANT QUE
```

**Statut : Déduit.**

## 11.3 Appel de procédure ou fonction

1. préparer les paramètres ;
2. créer le cadre d'activation ;
3. établir l'environnement ;
4. transférer le contrôle ;
5. exécuter ;
6. récupérer le résultat éventuel ;
7. restaurer le cadre précédent.

## 11.4 Retour

1. vérifier le cadre courant ;
2. produire le résultat si nécessaire ;
3. restaurer le cadre appelant ;
4. restaurer la position de retour ;
5. poursuivre.

## 11.5 Erreur d'exécution

1. arrêter l'opération fautive ;
2. conserver l'état nécessaire au diagnostic ;
3. produire le diagnostic ;
4. placer l'exécution dans l'état d'erreur ;
5. ne pas reprendre automatiquement.

---

# 12. Algorithmes des procédures et fonctions

## 12.1 Appel

### ALG-1201

**Entrées :**

- identificateur ;
- paramètres.

**Étapes :**

1. résoudre la procédure ou fonction ;
2. vérifier les paramètres ;
3. créer le cadre ;
4. établir les liaisons ;
5. transférer le contrôle.

## 12.2 Retour d'une procédure

Le retour détruit le cadre courant après conservation des effets autorisés.

## 12.3 Retour d'une fonction

Le retour :

1. produit la valeur ;
2. vérifie son type ;
3. restaure le contexte appelant ;
4. dépose le résultat à l'endroit logique prévu.

## 12.4 Récursivité

Chaque appel récursif possède son propre cadre d'activation.

Le cadre précédent n'est pas remplacé.

**Statut : Déduit.**

---

# 13. Algorithmes de gestion des objets

## 13.1 Création d'une variable

1. vérifier l'identificateur ;
2. vérifier le contexte ;
3. créer la référence ;
4. associer son type ;
5. initialiser selon les règles du Tome II.

## 13.2 Résolution

1. rechercher dans le contexte actif ;
2. rechercher dans les contextes accessibles ;
3. appliquer les règles de portée ;
4. produire une référence ou une erreur.

## 13.3 Tableau

L'accès à un tableau :

1. vérifie le nombre de dimensions ;
2. vérifie les indices ;
3. calcule la position logique ;
4. accède à l'élément.

Aucune formule physique d'adressage n'est imposée.

## 13.4 Chaîne

Une opération sur une chaîne vérifie :

- l'existence ;
- la longueur ;
- les positions ;
- les limites.

## 13.5 Libération

La libération d'un objet rend celui-ci indisponible dans son contexte.

Une référence encore utilisée doit être détectée comme invalide selon les règles d'erreur du Tome III.

---

# 14. Algorithmes de gestion des fichiers

## 14.1 Recherche

### ALG-1401

```text
rechercher identité dans le catalogue
SI absente
    produire FICHIER_INEXISTANT
SINON
    retourner descripteur
FIN SI
```

## 14.2 Création temporaire

1. vérifier l'identité ;
2. vérifier l'absence de conflit dans le contexte ;
3. créer le descripteur ;
4. créer le contenu logique vide ;
5. publier.

## 14.3 Suppression

1. rechercher ;
2. vérifier le type ;
3. vérifier les autorisations ;
4. retirer du catalogue ;
5. libérer le contenu.

## 14.4 Catalogage

Le passage temporaire → permanent conserve le contenu logique et modifie son statut d'appartenance.

## 14.5 Rangement d'un programme

RANGER ne transforme pas le programme courant en texte.

Il conserve sa représentation codée.

## 14.6 Appel d'un programme

APPELER restaure une représentation compilée.

Il ne passe pas par une reconstruction textuelle intermédiaire.

## 14.7 ENCODER / DECODER

La relation fonctionnelle est :

```text
programme courant
       │
       ▼
   DECODER
       │
       ▼
fichier-données textuel
       │
       ▼
    ENCODER
       │
       ▼
programme courant
```

Cette relation ne signifie pas que les deux opérations constituent nécessairement une identité parfaite pour toute information interne non représentée dans le texte source.

---

## 14.8 Validation d'une identification

### ALG-1402

```text
VALIDER_IDENTIFICATION(session, compte, clé)

    vérifier la forme logique du compte
    vérifier l'identification dans la liste des identifications légales

    SI identification absente ou illégale
        retourner IDENTIFICATION_ILLEGALE
    FIN SI

    fixer l'identification courante de la session

    appliquer, si le profil historique le prévoit, les règles relatives
    à l'espace temporaire associé à l'identification

    détruire les fichiers temporaires de la session conformément à la règle historique

    retourner succès
```

La clé n'est pas reproduite dans le résultat. Le détail de la vérification interne n'est pas historiquement documenté. [INRDP75 §Fichiers]

**Statut :** Déduit.

## 14.9 Contrôle d'accès à un fichier

### ALG-1403

```text
VERIFIER_AUTORISATION(session, fichier, opération)

    lire l'identification courante

    SI opération porte sur une écriture permanente
        vérifier l'existence d'une identification courante
        vérifier le pouvoir requis
        vérifier l'identification de création lorsque la règle l'impose
    FIN SI

    SI opération porte sur une lecture protégée
        appliquer la protection du fichier
        appliquer la variante historique du profil d'implantation
    FIN SI

    SI autorisation refusée
        retourner ACCES_REFUSE
    SINON
        retourner AUTORISE
    FIN SI
```

Une opération refusée ne modifie pas le fichier. Les règles exactes de protection qui diffèrent entre T1600 et MITRA 15 restent celles documentées dans le profil correspondant. [INRDP75 §Fichiers]

**Statut :** Déduit.

# 15. Algorithmes du temps partagé

## 15.1 Activation

### ALG-1501

1. sélectionner une tâche éligible selon la politique du moniteur ;
2. conserver la tâche actuellement active si nécessaire ;
3. effectuer SWAP OUT ;
4. restaurer la tâche choisie ;
5. effectuer SWAP IN ;
6. activer son contexte ;
7. reprendre.

Le manuel historique indique une sélection généralement cyclique et mentionne également des priorités particulières. Le Tome IV ne fixe donc pas une politique unique au-delà des propriétés normatives déjà établies. [INRDP75 §3.1]

## 15.2 Suspension

### ALG-1502

```text
SI tâche active
    capturer l'état nécessaire
    conserver l'état
    état tâche ← CONSERVEE
FIN SI
```

## 15.3 SWAP OUT

Le SWAP OUT conserve l'état logique de la tâche active.

Il ne conserve pas seulement le fichier-programme.

L'état conservé comprend, selon les éléments effectivement présents dans l'exécution :

- position d'exécution ;
- registres virtuels ;
- pile d'évaluation ;
- cadres d'activation ;
- variables ;
- tableaux ;
- chaînes ;
- objets et références nécessaires ;
- contexte de session ;
- état de suspension ;
- informations nécessaires à la reprise.

**Invariant :**

La conservation maintient la cohérence entre position d'exécution, cadres d'activation, pile d'évaluation et objets nécessaires à la poursuite.

## 15.4 SWAP IN

### ALG-1503

1. identifier la tâche ;
2. obtenir son état conservé ;
3. restaurer la position d'exécution ;
4. restaurer les registres virtuels ;
5. restaurer la pile d'évaluation ;
6. restaurer les cadres d'activation ;
7. restaurer les variables, tableaux, chaînes, objets et références nécessaires ;
8. vérifier la cohérence de l'état ;
9. rendre la tâche active ;
10. reprendre.

## 15.5 Reprise

La reprise doit restituer la position logique antérieure.

Elle ne doit pas recommencer l'exécution au début sauf si l'état antérieur indiquait explicitement cette position.

**Traçabilité :**

HIS-13.1, HIS-13.3, HIS-13.4 → EXI-0060 → VM-2106 → ALG-1503.

## 15.6 Politique de sélection

La politique précise de sélection des tâches n'est pas imposée par le Tome IV lorsqu'elle n'est pas déterminée par les niveaux supérieurs.

**Statut : Choix de conception laissé ouvert.**

---

# 16. Algorithmes du dialogue

## 16.1 Cycle conversationnel

### ALG-1601

**Objet :** cycle conversationnel et routage des demandes.

```text
TANT QUE session active
    recevoir une ligne terminée
    déterminer sa nature
    SI commande
        reconnaître les deux premières lettres
        compléter le nom en mode normal
        recevoir les paramètres éventuels
        sélectionner le traitement correspondant
        exécuter la commande ou transmettre au processeur compétent
    SINON SI ligne de programme
        compiler
        accepter ou rejeter
        SI mode machine de bureau ET compilation acceptée
            interpréter immédiatement
        FIN SI
    SINON
        traiter selon le contexte conversationnel
    FIN SI
    restituer le résultat ou le diagnostic
FIN TANT QUE
```

Le routage fonctionnel minimal est :

- action immédiate → moniteur / dialogue ;
- listage ou restitution → décompilateur ;
- exécution ou poursuite → interpréteur / environnement d'exécution ;
- fichiers → bibliothécaire et services associés ;
- ligne de programme → compilateur ;
- expression immédiate → compilateur puis interpréteur.

DECODER et ENCODER peuvent combiner plusieurs processeurs. [INRDP75 §3.3 ; §4 ; Annexe II]

Le caractère conversationnel et le traitement ligne par ligne sont documentés dans les manuels historiques. [INRDP75 §3 ; LSE15 §1]

**Statut : Déduit à partir du diagramme historique et des descriptions de commandes.**

## 16.2 Machine de bureau

En mode machine de bureau :

1. compiler la ligne ;
2. accepter l'objet ;
3. transmettre l'objet à l'interpréteur ;
4. exécuter immédiatement ;
5. restituer le résultat.

Cette organisation est explicitement décrite par le manuel de 1975.

## 16.3 Mode programmé

En mode programmé :

1. compiler les lignes ;
2. conserver les objets compilés ;
3. attendre la commande d'exécution ;
4. lancer le programme courant.

## 16.4 LISTER

LISTER demande la décompilation des objets du programme courant puis leur restitution textuelle.

## 16.5 Commandes de fichiers

Le dialogue distingue les commandes :

- portant sur le programme ;
- portant sur les fichiers-données ;
- portant sur la bibliothèque ;
- portant sur l'exécution.

## 16.6 PAS À PAS

### ALG-1602

**Objet :** contrôler une exécution en l'arrêtant au début de chaque ligne.

**Entrées :**

- exécution active ;
- état PAS À PAS activé.

**Sorties :**

- exécution suspendue au début de la ligne suivante ;
- contrôle rendu au moniteur ;
- numéro de la ligne atteinte rendu observable.

**Préconditions :**

- une exécution est active ;
- l'état PAS À PAS est activé.

**Postconditions :**

- aucune instruction de la ligne atteinte n'est exécutée avant la poursuite ;
- l'état d'exécution nécessaire à la reprise est conservé ;
- le dialogue redevient disponible.

**Étapes :**

1. détecter l'atteinte de la frontière de ligne ;
2. arrêter l'avancement avant l'exécution de la ligne ;
3. conserver la position et l'état d'exécution nécessaires ;
4. passer l'exécution à l'état suspendu ;
5. rendre le contrôle au moniteur ;
6. afficher le numéro de la ligne atteinte ;
7. attendre une commande de poursuite ou une intervention autorisée ;
8. sur CONTINUER, reprendre à la position conservée ;
9. sur NORMAL, désactiver le mode PAS À PAS pour la suite de l'exécution.

**Erreurs :**

- absence d'exécution active ;
- état d'exécution incohérent ;
- position de reprise invalide.

**Invariant :**

La ligne annoncée n'est pas exécutée avant la reprise. Le programme courant reste inchangé.

**Statut : Déduit pour l'algorithme à partir d'un comportement documenté.**

**Traçabilité :** HIS-13.1, HIS-13.3 → EXI-0060 → VM-2102, VM-2103 → ALG-1602.

## 16.7 IDENTIFICATION

La commande IDENTIFICATION transmet au système le numéro de compte et la clé permettant de sélectionner une identification légale. En cas d'acceptation, cette identification devient l'identification courante de la session. En cas de refus, l'identification courante n'est pas modifiée. [INRDP75 §Fichiers]

La commande peut être utilisée plusieurs fois au cours d'une session. Le changement d'identification entraîne la destruction des fichiers temporaires créés par l'utilisateur. [INRDP75 §Fichiers]

Le contrôle des opérations ultérieures sur les fichiers permanents s'effectue à partir de cette identification et des protections applicables au fichier.

**Statut :** Déduit.

**Traçabilité :** HIS-11.3.1 → EXI-0062 → VM-2001, VM-3408 → ALG-1402, ALG-1403.

---

# 17. Algorithmes de gestion des erreurs

## 17.1 Classification

Le Tome III impose quatre domaines principaux :

1. `COMPILATION` ;
2. `EXECUTION` ;
3. `SERVICE` ;
4. `DIALOGUE`.

La classification détaillée d'une erreur peut préciser sa nature, son contexte et, lorsque cela est applicable, sa position logique.

Une interruption volontaire n'est pas une erreur.

**Statut : Déduit à partir de la classification du Tome III.**

**Invariant :**

Une erreur possède un seul domaine principal et conserve ce domaine pendant sa propagation.

Les exemples suivants relèvent notamment de ces domaines :

- erreur syntaxique → `COMPILATION` ;
- erreur produite pendant l'interprétation → `EXECUTION` ;
- erreur de chargement, de sauvegarde ou d'opération du bibliothécaire → `SERVICE` ;
- entrée qui ne peut être orientée vers un traitement valide → `DIALOGUE`.

## 17.2 Erreur syntaxique

Une erreur syntaxique est détectée avant l'acceptation de la ligne.

La ligne fautive n'entre pas dans le programme courant.

## 17.3 Erreur d'exécution

Une erreur d'exécution intervient pendant l'interprétation.

Elle ne doit pas être présentée comme une erreur de compilation.

## 17.4 Erreur de fichier

Une opération de fichier peut échouer pour :

- absence du fichier ;
- mauvais type ;
- absence d'enregistrement ;
- accès interdit ;
- contenu invalide ;
- incohérence du catalogue.

## 17.5 Erreur de reprise

Une tâche dont l'état conservé est invalide ne doit pas être réactivée comme si son état était valide.

L'erreur doit être signalée.

## 17.6 Propagation

### ALG-1701

```text
détecter erreur
déterminer le domaine principal
identifier la nature de l'erreur
arrêter l'opération courante
préserver l'état requis
construire le diagnostic
transmettre le diagnostic
mettre à jour l'état d'erreur
```

Le domaine principal est conservé lors de la transmission du diagnostic.

## 17.7 Invariants

Une erreur ne doit :

- ni valider une donnée invalide ;
- ni publier un objet incomplet ;
- ni rendre incohérent le programme courant ;
- ni transformer une tâche en fichier ;
- ni transformer un fichier en tâche.

---

# 18. Séquences architecturales

## 18.1 Saisie d'une ligne

```text
Utilisateur
    │
    ▼
Moniteur
    │
    ▼
Analyseur de commandes
    │
    ▼
Compilateur
    │
    ├── erreur ──► diagnostic
    │
    ▼
Objet compilé
    │
    ▼
Programme courant
```

## 18.2 Exécution

```text
Utilisateur
    │
    ▼
Moniteur
    │
    ▼
Moteur d'exécution
    │
    ▼
Interpréteur
    │
    ├──► pile d'évaluation
    ├──► cadres d'activation
    ├──► variables
    ├──► tableaux
    └──► chaînes
```

## 18.3 LISTER

```text
Programme courant
       │
       ▼
Décompilateur
       │
       ▼
Représentation textuelle
       │
       ▼
Moniteur
       │
       ▼
Utilisateur
```

## 18.4 RANGER

```text
Programme courant
       │
       ▼
Bibliothécaire
       │
       ▼
Fichier-programme
       │
       ▼
Catalogue
```

## 18.5 APPELER

```text
Catalogue
    │
    ▼
Fichier-programme
    │
    ▼
Bibliothécaire
    │
    ▼
Validation
    │
    ▼
Programme courant
```

## 18.6 DECODER

```text
Programme courant
       │
       ▼
Décompilateur
       │
       ▼
Lignes textuelles
       │
       ▼
Fichier-données temporaire
```

## 18.7 ENCODER

```text
Fichier-données
       │
       ▼
Lecture des enregistrements
       │
       ▼
Lignes textuelles
       │
       ▼
Compilateur
       │
       ▼
Programme courant
```

## 18.8 GARER / CHARGER

```text
Programme
   │
   ▼
GARER
   │
   ▼
Fichier-données
   │
   ▼
CHARGER
   │
   ▼
Programme
```

La relation précédente représente uniquement les échanges de données ; elle ne signifie pas que GARER et CHARGER sont des opérations inverses dans tous les contextes.

## 18.9 Temps partagé

```text
Tâche active
     │
     ▼
SWAP OUT
     │
     ▼
État conservé
     │
     ▼
Sélection
     │
     ▼
SWAP IN
     │
     ▼
Tâche active
```

Le manuel historique décrit cette succession et précise que la zone de swap conserve les images des tâches. [INRDP75 §3.1]

---

# 19. Vérification de conformité

## 19.1 Objet

La vérification consiste à démontrer que la réalisation respecte les propriétés imposées par les Tomes I à IV.

## 19.2 Vérification des interfaces

Pour chaque interface, vérifier :

- fournisseur ;
- consommateur ;
- paramètres ;
- résultat ;
- erreurs ;
- invariants ;
- absence de dépendance technologique.

## 19.3 Vérification des formats

Pour chaque format, vérifier :

- structure logique ;
- types ;
- contraintes ;
- validation ;
- transformation ;
- distinction historique/reconstruction.

## 19.4 Vérification de la compilation

Vérifier :

1. compilation indépendante des lignes ;
2. rejet atomique ;
3. représentation compilée valide ;
4. conservation de l'ordre logique ;
5. absence de recompilation inutile.

## 19.5 Vérification de la représentation compilée

Vérifier :

1. exécutabilité ;
2. décompilabilité ;
3. cohérence des références ;
4. cohérence des opérations codées ;
5. absence de confusion entre chaîne codée L.S.E. et chaîne post-fixée historiquement attestée pour le LSD.

## 19.6 Vérification des fichiers

Vérifier :

- distinction programme/données ;
- distinction temporaire/permanent ;
- intégrité des enregistrements ;
- fonctionnement de GARER ;
- fonctionnement de CHARGER ;
- fonctionnement de SUPPRIMER ;
- fonctionnement de CATALOGUER ;
- fonctionnement de DECODER ;
- fonctionnement d'ENCODER.

## 19.7 Vérification du temps partagé

Vérifier :

- conservation d'une tâche ;
- SWAP OUT ;
- SWAP IN ;
- restauration ;
- reprise ;
- séparation tâche/fichier-programme.

## 19.8 Vérification des erreurs

Vérifier que :

- les erreurs sont classifiées ;
- elles sont transmises ;
- elles n'entraînent pas de publication partielle ;
- une erreur d'exécution n'est pas assimilée à une erreur de compilation.

## 19.9 Critère final

Une réalisation est conforme lorsque tous les invariants et contrats applicables sont satisfaits sans dépendre d'une identité de représentation physique.

---

# 20. Limites et degrés de reconstruction

## 20.1 Formats historiques

Les sources historiques établissent l'existence de représentations codées, de fichiers-programmes, de fichiers-données, d'enregistrements et de zones de conservation.

Elles ne permettent pas de déterminer dans tous les cas leur représentation physique détaillée.

## 20.2 Chaîne codée

La chaîne codée L.S.E. est historiquement attestée.

Son organisation interne complète n'est pas établie par les sources disponibles.

## 20.3 Chaîne post-fixée

La chaîne post-fixée est historiquement attestée pour le LSD dans le récit de Noyelle.

Son extension au L.S.E. constitue une reconstruction.

## 20.4 Formats de fichiers

Les catégories de fichiers et leurs opérations sont historiquement documentées.

Leur organisation physique détaillée n'est pas suffisamment documentée pour être reproduite comme fait historique.

La présente reconstruction distingue désormais la représentation compilée logique (RCI-M), indépendante de la plateforme, et la représentation sérialisée d'un programme (RCI-S). Le format RCI-S défini en Annexe K est une reconstruction et ne doit pas être présenté comme le format binaire historique original.

## 20.5 États des tâches

La conservation des états de tâches et les opérations SWAP IN / SWAP OUT sont documentées.

La composition physique détaillée de l'image d'une tâche ne l'est pas.

## 20.6 Algorithmes

Les sources documentent certains enchaînements fonctionnels.

Elles ne documentent pas nécessairement les algorithmes internes exacts.

Les algorithmes du présent tome sont donc, selon les cas :

- Documentés lorsqu'ils reprennent directement une séquence historique explicite ;
- Déduits lorsqu'ils résultent nécessairement des contrats ;
- Probables lorsqu'ils s'appuient sur plusieurs indications concordantes ;
- Choix de conception lorsqu'ils complètent une lacune.

## 20.7 Principe de prudence

Aucune précision du Tome IV ne doit être utilisée comme preuve historique d'une organisation interne que les sources ne décrivent pas.

---

# Annexes

# Annexe A — Registre des interfaces

## ANN-A-0001 — Registre général

| Identifiant | Interface | Fournisseur | Consommateur |
|---|---|---|---|
| IV-IF-0001 | COMPILER_LIGNE | Compilateur | Moniteur |
| IV-IF-0002 | INSERER_LIGNE | Programme courant | Compilateur |
| IV-IF-0003 | EXECUTER_PROGRAMME | Moteur d'exécution | Moniteur |
| IV-IF-0004 | DECOMPILER | Décompilateur | Moniteur |
| IV-IF-0005 | LIRE_DONNEE | Gestionnaire des fichiers-données | CHARGER |
| IV-IF-0006 | ECRIRE_DONNEE | Gestionnaire des fichiers-données | GARER |
| IV-IF-0007 | RECHERCHER_FICHIER | Catalogue | Bibliothécaire |
| IV-IF-0008 | SWAP_OUT | Gestionnaire des tâches | Conservation |
| IV-IF-0009 | SWAP_IN | Conservation | Gestionnaire des tâches |
| IV-IF-0010 | TRANSFORMER_PROGRAMME_DONNEE | Transformateur programme-donnée | Bibliothécaire / moniteur |
| IV-IF-0011 | CATALOGUER_FICHIER_DONNEE | Catalogueur de fichier-données | Bibliothécaire / commande CATALOGUER |
| IV-IF-0013 | ANALYSER_COMMANDE | Analyseur de commandes | Moniteur / processeur destinataire |
| IV-IF-0014 | GÉRER_IDENTIFICATION_ET_AUTORISATION | Gestionnaire des identifications et des autorisations | Moniteur / bibliothécaire / gestionnaire des fichiers |
| IV-IF-0015 | SERIALISER_PROGRAMME | Bibliothécaire / conservation | Programme courant |
| IV-IF-0016 | RESTAURER_PROGRAMME | Bibliothécaire / conservation | Programme courant |
| IV-IF-0017 | VERIFIER_FORMAT_PROGRAMME | Bibliothécaire / conservation | RESTAURER_PROGRAMME |
| IV-IF-0018 | VERIFIER_COMPATIBILITE_PROGRAMME | Bibliothécaire / conservation | RESTAURER_PROGRAMME |

## ANN-A-0002 — Règle

Chaque interface doit être reliée à une responsabilité du Tome III.

---

# Annexe B — Registre des formats

## ANN-B-0001 — Formats logiques

| Identifiant | Format | Nature | Statut |
|---|---|---|---|
| IV-FMT-0001 | Objet compilé | logique | Déduit |
| IV-FMT-0002 | Représentation compilée | logique | Déduit |
| IV-FMT-0003 | Élément post-fixé | logique | Choix de conception |
| IV-FMT-0004 | Valeur | logique | Déduit |
| IV-FMT-0005 | Tableau | logique | Déduit |
| IV-FMT-0006 | Chaîne | logique | Déduit |
| IV-FMT-0007 | Enregistrement | logique | Déduit |
| IV-FMT-0008 | Fichier-programme | logique | Déduit |
| IV-FMT-0009 | Fichier-données | logique | Déduit |
| IV-FMT-0010 | État d'exécution | logique | Déduit |
| IV-FMT-0011 | État de tâche | logique | Déduit |
| IV-FMT-0012 | Profil d'implantation historique | logique | Choix de conception |
| IV-FMT-0013 | Identification et droits d'accès | logique | Déduit |
| IV-FMT-0014 | RCI-M | logique | Choix de conception |
| IV-FMT-0015 | Élément RCI-M | logique | Choix de conception |
| IV-FMT-0016 | Table des symboles | logique | Déduit / choix de conception |
| IV-FMT-0017 | RCI-S | logique sérialisé | Choix de conception |
| IV-FMT-0018 | En-tête RCI-S | logique sérialisé | Choix de conception |
| IV-FMT-0019 | Provenance minimale | logique sérialisé | Choix de conception |

## ANN-B-0002 — Formats physiques

Aucun format physique complet n'est fixé par le présent tome comme fait historique.

Tout format physique nécessaire à une implémentation constitue un choix de réalisation.

---

# Annexe C — Registre des algorithmes

| Identifiant | Algorithme |
|---|---|
| ALG-0601 | RANGER |
| ALG-0602 | APPELER |
| ALG-0603 | SUPPRIMER fichier-programme |
| ALG-0701 | GARER |
| ALG-0702 | CHARGER |
| ALG-0703 | SUPPRIMER fichier-données |
| ALG-0704 | CATALOGUER |
| ALG-0705 | DECODER |
| ALG-0706 | ENCODER |
| ALG-0901 | Compilation d'une ligne |
| ALG-0902 | Acceptation d'une ligne |
| ALG-1001 | Décompilation d'un objet |
| ALG-1002 | Décompilation du programme |
| ALG-1101 | Initialisation d'exécution |
| ALG-1102 | Interprétation |
| ALG-1201 | Appel procédure/fonction |
| ALG-1401 | Recherche fichier |
| ALG-1402 | Validation d'une identification |
| ALG-1403 | Contrôle d'accès à un fichier |
| ALG-1501 | Activation |
| ALG-1502 | Suspension |
| ALG-1503 | SWAP IN |
| ALG-1601 | Cycle conversationnel |
| ALG-1602 | PAS À PAS |
| ALG-1701 | Propagation d'erreur |
| ALG-1801 | Sérialisation d'un programme |
| ALG-1802 | Restauration d'un programme |
| ALG-1803 | Vérification du format d'un programme |
| ALG-1804 | Vérification de compatibilité d'un programme |

---

# Annexe D — Matrice de traçabilité

| Élément Tome IV | VM | EXI | HIS / source |
|---|---|---|---|
| Compilation ligne | VM-0301 à VM-0306, VM-3201 à VM-3203 | EXI-0001, EXI-0002, EXI-0018, EXI-0051 | HIS-6.2, HIS-6.5 ; INRDP75 §3.3 |
| Représentation compilée | VM-0401 à VM-0408 | EXI-0010 à EXI-0016 | HIS-7 ; INRDP75 §3.3 ; NOYELLE89 p.219-220 |
| Décompilation | VM-1801, VM-1802 | EXI-0003, EXI-0015 | HIS-4.5.2 ; INRDP75 §3.3 |
| Programme courant | VM-0301 à VM-0306 | EXI-0001 à EXI-0008 | HIS-6.2 |
| Fichiers-programmes | VM-1701, VM-1702 | EXI-0025 | HIS-11.5 ; INRDP75 §4-4 |
| Fichiers-données | VM-3404, VM-3405 | EXI-0053 à EXI-0057 | HIS-11 ; INRDP75 §Fichiers |
| GARER | VM-3405 | EXI-0056 | INRDP75 §Fichiers |
| CHARGER | VM-3405 | EXI-0056 | INRDP75 §Fichiers |
| CATALOGUER | VM-3405 | EXI-0056 | INRDP75 §Fichiers |
| DECODER | VM-1801, VM-3406 | EXI-0003, EXI-0057 | INRDP75 Annexe II |
| ENCODER | VM-3406 | EXI-0057 | INRDP75 Annexe II |
| TRANSFORMER_PROGRAMME_DONNEE | VM-3406 | EXI-0057 | HIS-11.5 ; INRDP75 Annexe II |
| CATALOGUER_FICHIER_DONNEE | VM-3407 | EXI-0056 | HIS-11.3 ; LSE15 §9-5 |
| État d'exécution | VM-0801 à VM-1002 | EXI-0023, EXI-0024, EXI-0031, EXI-0033 | HIS-8 ; INRDP75 §4.3 |
| Temps partagé | VM-2101 à VM-2104, VM-2105 | EXI-0058, EXI-0059 | HIS-3.10 ; HIS-4 ; INRDP75 §3.1 |
| SWAP IN / OUT | VM-2105 | EXI-0058, EXI-0059 | INRDP75 §3.1 ; §8-1 |
| RCI-M / représentation compilée | VM-0401 à VM-0408, VM-1801, VM-1802 | EXI-0002, EXI-0003, EXI-0010 à EXI-0016 | HIS-7 ; INRDP75 §3.3 ; NOYELLE89 p.219-220 |
| RCI-S / fichier-programme sérialisé | VM-1701, VM-1702, VM-1801 | EXI-0009, EXI-0025 | HIS-11.5 ; INRDP75 §4-4 |
| Provenance et compatibilité RCI-S | VM-1701, VM-1702 | EXI-0009, EXI-0025 | Reconstruction |

---

### ANN-D-EXI — Couverture complémentaire des EXI par mécanisme

La traçabilité complémentaire est établie par mécanisme. Une ligne peut
regrouper plusieurs EXI lorsque ces exigences sont satisfaites conjointement
par le même contrat, format ou algorithme. Chaque EXI reste néanmoins
explicitement identifiable dans la cellule « EXI ».

| Mécanisme | EXI couverts | Éléments Tome IV porteurs |
|---|---|---|
| Programme courant, compilation et acceptation | EXI-0004, EXI-0005, EXI-0006, EXI-0007, EXI-0009 | IV-IF-0001 ; IV-FMT-0001 ; ALG-0901 ; ALG-0902 |
| Représentation compilée et parcours | EXI-0011, EXI-0012, EXI-0013, EXI-0014, EXI-0017 | IV-FMT-0001 ; IV-FMT-0002 ; IV-FMT-0003 ; ALG-0902 |
| Exécution des opérations et transitions | EXI-0019, EXI-0020, EXI-0021, EXI-0022 | ALG-1101 ; ALG-1102 |
| Appels, cadres d'activation et retours | EXI-0026, EXI-0027, EXI-0028, EXI-0029, EXI-0030, EXI-0034, EXI-0035, EXI-0036 | IV-FMT-0010 ; IV-FMT-0011 ; ALG-1102 ; ALG-1201 |
| Objets d'exécution, variables, tableaux et références | EXI-0032, EXI-0037, EXI-0038, EXI-0039, EXI-0040, EXI-0041 | IV-FMT-0004 ; IV-FMT-0005 ; IV-FMT-0006 ; IV-FMT-0010 ; ALG-1102 |
| Procédures et fonctions | EXI-0042, EXI-0043, EXI-0044 | ALG-1201 ; IV-FMT-0010 ; IV-FMT-0011 |
| Compilation, flux et évaluation | EXI-0045, EXI-0046, EXI-0047, EXI-0048 | ALG-0901 ; ALG-0902 ; ALG-1102 ; IV-FMT-0003 |
| Séparation stockage / mémoire et gestion des objets | EXI-0049, EXI-0050 | IV-FMT-0004 ; IV-FMT-0005 ; IV-FMT-0006 ; IV-FMT-0010 ; IV-FMT-0011 |
| Dialogue | EXI-0052 | IV-IF-0013 ; ALG-1601 | HIS-4.7.1, HIS-5.6 ; INRDP75 §4 |
| PAS À PAS / reprise | EXI-0060 | ALG-1602 ; IV-IF-0012 ; ALG-1503 | HIS-13.1, HIS-13.3, HIS-13.4 ; INRDP75 §4-1, §4-3 |
| Profils d'implantation historiques | VM-2111 | EXI-0061 | IV-FMT-0012 ; IV-IF-0013 | HIS-4.8 ; INRDP75 « Remarque sur la présentation du fascicule », §4.1, Annexe II |
| Identifications et droits d'accès aux fichiers | VM-1709, VM-3404, VM-3405, VM-3408 | EXI-0062 | IV-IF-0014 ; IV-FMT-0013 ; ALG-1402 ; ALG-1403 | HIS-11.3.1 ; INRDP75 §Fichiers ; LSE15 §9-5 |
| Représentation compilée et sérialisation | EXI-0002, EXI-0003, EXI-0009, EXI-0010 à EXI-0016, EXI-0025 | IV-FMT-0014 à IV-FMT-0019 ; IV-IF-0015 à IV-IF-0018 ; ALG-1801 à ALG-1804 | HIS-7 ; HIS-11.5 ; INRDP75 §3.3 ; §4-4 ; NOYELLE89 p.219-220 |

**Règle de contrôle :** cette table ne crée aucune exigence nouvelle. Elle
complète uniquement la traçabilité EXI → Tome IV. Un EXI regroupé doit être
vérifiable à partir des éléments Tome IV indiqués dans la même ligne.

**Statut :** Choix de conception documentaire.

**Justification :** le regroupement par mécanisme évite de transformer
l'Annexe D en duplication du Catalogue des exigences, tout en conservant une
couverture explicite de chaque EXI.

**Traçabilité :** EXI → VM → interface, format ou algorithme Tome IV.

# Annexe E — Catalogue des invariants

## ANN-E-0001 — Invariants globaux

1. Le programme courant ne contient pas deux fois le même numéro de ligne.
2. Une ligne rejetée ne modifie pas le programme courant.
3. Tout objet compilé accepté est valide.
4. Le programme courant est distinct de l'état d'exécution.
5. L'exécution utilise la représentation compilée.
6. La représentation compilée reste décompilable selon les capacités définies.
7. Un fichier-programme reste distinct d'un fichier-données.
8. Un fichier temporaire reste distinct d'un fichier permanent.
9. Un fichier reste distinct d'une tâche.
10. La conservation d'une tâche conserve l'état nécessaire à sa reprise.
11. SWAP OUT ne détruit pas l'état nécessaire.
12. SWAP IN restaure cet état.
13. Une erreur ne publie pas d'état partiellement valide.
14. Une interface ne modifie pas la responsabilité d'un autre composant.
15. Aucun format physique reconstruit n'est présenté comme historique.
16. La chaîne codée L.S.E. reste distincte historiquement de la chaîne post-fixée du LSD.
17. Un profil d'implantation ne peut introduire qu'une différence historiquement attestée ou strictement nécessaire à sa représentation.

---

# Annexe F — Séquences d'échanges

## ANN-F-0001 — Compilation

```text
MONITEUR
  → ANALYSEUR
  → COMPILATEUR
  → PROGRAMME COURANT
```

## ANN-F-0002 — Exécution

```text
MONITEUR
  → MOTEUR D'EXÉCUTION
  → INTERPRÉTEUR
  → OBJETS D'EXÉCUTION
```

## ANN-F-0003 — Fichiers

```text
MONITEUR / PROGRAMME
  → BIBLIOTHÉCAIRE
  → CATALOGUE
  → FICHIER
```

## ANN-F-0003.1 — Identification puis opération sur fichier permanent

```text
UTILISATEUR
  → IDENTIFICATION
  → GESTIONNAIRE DES IDENTIFICATIONS
  → IDENTIFICATION COURANTE
  → OPÉRATION FICHIER
  → CONTRÔLE D'AUTORISATION
  → BIBLIOTHÉCAIRE / FICHIER
```

Une identification refusée interrompt la séquence sans modifier l'identification courante. Une opération de fichier refusée n'est pas publiée. [INRDP75 §Fichiers]

## ANN-F-0004 — DECODER

```text
PROGRAMME COURANT
  → DÉCOMPILATEUR
  → FICHIER-DONNÉES TEMPORAIRE
```

## ANN-F-0005 — ENCODER

```text
FICHIER-DONNÉES
  → BIBLIOTHÉCAIRE
  → COMPILATEUR
  → PROGRAMME COURANT
```

## ANN-F-0006 — Temps partagé

```text
TÂCHE ACTIVE
  → SWAP OUT
  → ÉTAT CONSERVÉ
  → SÉLECTION
  → SWAP IN
  → GESTIONNAIRE DE REPRISE
  → TÂCHE ACTIVE
```

La reprise est réalisée à partir de l'état conservé de la tâche ; elle ne
modifie pas la représentation compilée.

---

# Annexe G — Formats de fichiers

## ANN-G-0001 — Fichier-programme

```text
FICHIER-PROGRAMME
    identité
    identification de création
    protections éventuelles
    représentation du programme
        ligne
        objet compilé
        représentation compilée
```

Statut : **Déduit**.

## ANN-G-0002 — Fichier-données

```text
FICHIER-DONNÉES
    identité
    identification de création lorsqu'il est permanent
    protections éventuelles
    statut
    enregistrements
        numéro
        type
        dimensions éventuelles
        longueur éventuelle
        contenu
```

Statut : **Déduit**.

## ANN-G-0003 — Format physique

Non déterminé historiquement.

---

# Annexe H — Formats de la chaîne compilée

## ANN-H-0001 — Élément logique

```text
ÉLÉMENT
    catégorie
    contenu
```

Catégories minimales :

- valeur ;
- référence ;
- opération codée ;
- information de contrôle.

Statut : **Choix de conception** pour la catégorisation minimale.

## ANN-H-0002 — Chaîne compilée

```text
CHAÎNE COMPILÉE
    séquence ordonnée
        position 1 → élément
        position 2 → élément
        ...
        position n → élément
    fin de séquence
```

Opérations logiques de parcours :

```text
PREMIER_ELEMENT(sequence)
ELEMENT_SUIVANT(sequence, position)
LIRE_ELEMENT(sequence, position)
FIN_SEQUENCE(sequence, position)
```

La séquence est totalement ordonnée et possède une position logique courante.

Cette représentation constitue le modèle logique de reconstruction.

Elle ne constitue pas une transcription d'un format binaire historique.

**Statut : Déduit pour les propriétés de parcours ; Choix de conception pour les noms des opérations.**

---

# Annexe I — Pseudocode des algorithmes

## ANN-I-0001 — Compilation

```text
COMPILER_LIGNE(ligne)

    structure ← ANALYSER(ligne)

    SI structure = diagnostic
        retourner diagnostic
    FIN SI

    représentation ← GENERER(structure)

    SI représentation invalide
        retourner diagnostic
    FIN SI

    retourner objet_compilé(représentation)
```

L'analyse relève de `VM-3202`.

La génération de la représentation compilée relève de `VM-3203`.

## ANN-I-0002 — Acceptation

```text
ACCEPTER_LIGNE(objet)

    vérifier objet

    SI suppression
        supprimer ligne correspondante
    SINON
        remplacer ou insérer objet
    FIN SI

    rétablir ordre logique

    retourner succès
```

## ANN-I-0003 — Exécution

```text
EXECUTER(programme, position)

    créer état d'exécution

    TANT QUE état = EXECUTION

        SI FIN_SEQUENCE(programme, position)
            terminer
        FIN SI

        élément ← LIRE_ELEMENT(programme, position)

        SI élément = valeur
            empiler(élément)

        SINON SI élément = référence
            valeur ← résoudre(élément)
            empiler(valeur)

        SINON SI élément = opération
            exécuter(élément)

        SINON SI élément = contrôle
            appliquer le contrôle
        SINON
            signaler erreur d'exécution
        FIN SI

        position ← ELEMENT_SUIVANT(programme, position)

    FIN TANT QUE
```

## ANN-I-0004 — GARER

```text
GARER(fichier, numéro, valeur)

    vérifier fichier
    vérifier numéro
    vérifier valeur

    préparer enregistrement

    publier enregistrement

    retourner succès
```

## ANN-I-0005 — CHARGER

```text
CHARGER(fichier, numéro)

    vérifier fichier
    vérifier l'autorisation de lecture si le fichier est protégé
    rechercher enregistrement

    SI absent
        retourner erreur
    FIN SI

    vérifier intégrité

    retourner type, valeur
```

## ANN-I-0006 — DECODER

```text
DECODER(programme)

    créer fichier temporaire

    POUR chaque ligne du programme
        texte ← décompiler(ligne)
        enregistrer texte
    FIN POUR

    publier fichier
```

## ANN-I-0007 — ENCODER

```text
ENCODER(fichier)

    lire les enregistrements dans l'ordre

    POUR chaque enregistrement
        ligne ← interpréter comme texte
        résultat ← compiler(ligne)

        SI résultat = erreur
            produire diagnostic
        SINON
            accepter résultat
        FIN SI
    FIN POUR
```

## ANN-I-0008 — SWAP OUT

```text
SWAP_OUT(tâche)

    vérifier tâche active

    capturer état nécessaire

    conserver état

    marquer tâche conservée
```

## ANN-I-0009 — SWAP IN

```text
SWAP_IN(tâche)

    vérifier état conservé

    restaurer état

    vérifier cohérence

    marquer tâche active

    reprendre
```

---

# Annexe J — Tableau de conformité

| Domaine | Exigence de conformité | Vérification |
|---|---|---|
| Compilation | lignes indépendantes | test d'ajout/modification |
| Programme | unicité des numéros | contrôle structurel |
| Représentation | exécutable | exécution d'un programme compilé |
| Décompilation | représentation textuelle | comparaison sémantique |
| Fichiers-programmes | distinction avec fichiers-données | contrôle du catalogue |
| Fichiers-données | enregistrements typés | essais GARER/CHARGER |
| GARER | conservation de la valeur | lecture ultérieure |
| CHARGER | restitution correcte | comparaison type/valeur |
| CATALOGUER | passage temporaire/permanent | contrôle du catalogue |
| SUPPRIMER | suppression correcte | recherche après suppression |
| DECODER | programme → texte | lecture du fichier produit |
| ENCODER | texte → programme | exécution du programme obtenu |
| Exécution | état distinct du programme | suspension et observation |
| Procédures | cadres distincts | appel imbriqué/récursif |
| Erreurs | séparation compilation/exécution | essais fautifs |
| Temps partagé | conservation de tâche | SWAP OUT / SWAP IN |
| Reprise | position conservée | reprise après suspension |
| Indépendance | absence de dépendance technologique | examen architectural |
| Identifications | identification courante et contrôle des droits | essais ID puis opérations permanentes |
| Propriété des fichiers | identification de création | tentative de modification/suppression sous une autre identification |
| Protections | règles T1600 / MITRA 15 | essais selon le profil historique |
| Traçabilité | HIS → EXI → VM → IV | contrôle des références |
| Historique | absence de faux format historique | audit documentaire |

## J.1 Critère d'acceptation final

Le Tome IV est conforme à son objectif lorsque la réalisation indépendante peut déterminer :

- les interfaces à fournir ;
- les données à échanger ;
- les structures logiques nécessaires ;
- les règles de validation ;
- les algorithmes fondamentaux ;
- les séquences d'échanges ;
- les erreurs ;
- les invariants ;

sans avoir à introduire une décision contraire aux Tomes I à III.

Toute représentation physique laissée ouverte par le présent tome demeure une décision de réalisation et non une propriété historique du L.S.E.

La représentation compilée logique RCI-M et le format sérialisé logique RCI-S définis en Annexe K constituent désormais les contrats de représentation communs aux implémentations ; leur encodage physique reste une décision de réalisation.
---

# Annexe K — Représentation compilée et format des programmes

## ANN-K-0001 — Objet

La présente annexe définit la représentation compilée utilisée par la machine virtuelle et le format logique permettant de conserver un programme sous forme sérialisée.

Elle complète les registres et formats du présent tome et définit une frontière entre :

1. la représentation compilée utilisée en mémoire ;
2. la représentation sérialisée d'un programme ;
3. la représentation physique choisie par une implémentation.

Elle ne définit ni langage d'implémentation, ni architecture matérielle, ni organisation particulière de la mémoire d'une machine.

**Statut : Choix de conception.**

## ANN-K-0002 — Principes directeurs

La représentation doit satisfaire simultanément les principes suivants :

- indépendance de la plateforme ;
- compacité ;
- exécutabilité ;
- décompilabilité ;
- conservation ;
- identifiabilité ;
- évolutivité contrôlée ;
- traçabilité de la provenance ;
- absence de dépendance aux adresses mémoire ;
- séparation entre représentation logique et représentation physique.

Toute information redondante ou reconstructible ne doit pas être conservée sans justification.

**Statut : Choix de conception.**

## ANN-K-0003 — Distinction des représentations

Trois niveaux sont distingués :

```text
SOURCE L.S.E.
     │
     ▼
RCI-M — représentation compilée en mémoire
     │
     ├──► interprétation
     └──► décompilation
     │
     ▼
RCI-S — représentation sérialisée du programme
```

La représentation physique effectivement utilisée en mémoire par une implémentation est distincte de RCI-M et peut dépendre du processeur, du compilateur et de l'organisation mémoire.

**Statut : Choix de conception.**

## ANN-K-0004 — Représentation compilée en mémoire — RCI-M

RCI-M est la représentation logique du programme compilé utilisée par le programme courant, l'interpréteur et le décompilateur. Elle est indépendante de toute adresse mémoire.

Elle est organisée comme une collection ordonnée d'objets compilés correspondant aux lignes du programme.

**Statut : Déduit.**

## ANN-K-0005 — Objet compilé

Un objet compilé contient logiquement :

```text
OBJET COMPILE
    numéro de ligne
    représentation compilée
```

Le numéro de ligne identifie l'objet dans le programme courant. Aucune adresse mémoire, pointeur ou position physique n'appartient à l'objet logique.

**Statut : Déduit.**

## ANN-K-0006 — Élément de représentation compilée

La représentation compilée est constituée d'une séquence ordonnée d'éléments. Un élément appartient aux catégories nécessaires au modèle d'exécution :

- valeur ;
- référence ;
- opération codée ;
- information de contrôle.

Le codage concret de la catégorie et du contenu n'est pas fixé à ce niveau.

**Statut : Choix de conception.**

## ANN-K-0007 — Chaîne post-fixée

La représentation retenue pour la reconstruction des expressions est une représentation post-fixée. Elle permet un traitement séquentiel par l'interpréteur :

```text
élément → pile
élément → pile
opération → consommation des opérandes
          → production du résultat
```

Cette représentation est une reconstruction. Elle ne doit pas être présentée comme la reproduction certaine du format interne historique de la chaîne codée L.S.E.

La chaîne post-fixée est historiquement attestée pour le LSD dans le récit de Noyelle ; son extension au L.S.E. est retenue comme reconstruction.

**Statut : Probable / Choix de conception selon l'élément considéré.**

## ANN-K-0008 — Parcours

Le parcours logique d'une représentation compilée doit permettre :

```text
PREMIER_ELEMENT
ELEMENT_SUIVANT
LIRE_ELEMENT
FIN_SEQUENCE
```

Ces opérations sont abstraites et ne supposent ni pointeur, ni index physique, ni taille fixe d'élément.

**Statut : Choix de conception.**

## ANN-K-0009 — Table des symboles

La représentation sérialisée peut utiliser une table de symboles permettant de représenter de manière compacte les références utilisées par la représentation compilée.

Elle peut notamment représenter les variables, tableaux, procédures, fonctions et autres objets nommés nécessaires. Une référence peut désigner un symbole par un identifiant compact.

Les noms textuels complets ne doivent être conservés qu'une seule fois lorsqu'ils sont nécessaires.

**Statut : Déduit pour la nécessité fonctionnelle ; Choix de conception pour la structure.**

## ANN-K-0010 — Identifiants internes

Les références internes ne doivent pas dépendre d'adresses mémoire. Une référence logique peut être représentée par un identifiant de symbole ou une combinaison compacte permettant d'identifier la catégorie et l'objet concerné.

**Statut : Choix de conception.**

## ANN-K-0011 — Constantes

Une constante utilisée par la représentation compilée doit être représentée une seule fois lorsque sa réutilisation est possible et avantageuse.

Une constante peut être immédiate ou référencée par une table de constantes. Le choix dépend de la taille globale, de la fréquence de réutilisation et du coût d'accès.

**Statut : Choix de conception.**

## ANN-K-0012 — Chaînes

Les chaînes doivent éviter la duplication inutile des données. Lorsqu'une chaîne constante est utilisée plusieurs fois, une représentation partagée peut être retenue, sans introduire de partage sémantique non prévu par le modèle.

**Statut : Choix de conception.**

## ANN-K-0013 — Informations de contrôle

Les informations nécessaires au contrôle du flux doivent être représentées dans RCI-M lorsqu'elles sont nécessaires à l'exécution, notamment pour les branchements, répétitions, appels et retours.

Les informations syntaxiques reconstructibles ne doivent pas être conservées.

**Statut : Déduit pour la nécessité ; Choix de conception pour la représentation.**

## ANN-K-0014 — Décompilation

RCI-M doit fournir au décompilateur les informations nécessaires à la production d'une représentation textuelle valide. La décompilation ne dépend pas du texte source original.

Lorsque plusieurs formulations syntaxiques sont possibles, une représentation canonique peut être produite.

**Statut : Déduit.**

## ANN-K-0015 — Principe de compacité

La représentation compilée doit être compacte. Une implémentation ne doit pas conserver simultanément plusieurs copies inutiles du programme, ni conserver systématiquement le texte source avec la représentation compilée, ni surdimensionner les éléments ou multiplier les métadonnées répétitives.

La compacité constitue une contrainte de conception de premier ordre, sans perte des informations nécessaires à l'exécution et à la décompilation.

**Statut : Choix de conception.**

## ANN-K-0016 — Représentation sérialisée — RCI-S

RCI-S est la représentation persistante d'un programme destinée notamment à RANGER, APPELER, la conservation, le transfert et la restauration.

Elle est construite à partir de RCI-M mais ne constitue pas une simple image brute de la mémoire. Elle contient les informations d'identification et de format, les informations minimales de provenance, les tables nécessaires et les objets compilés.

**Statut : Choix de conception.**

## ANN-K-0017 — En-tête RCI-S

L'en-tête doit permettre d'identifier le format sans analyser le programme. Il comprend logiquement :

```text
signature
version du format
version de RCI-M
caractéristiques nécessaires
```

Les versions et identifiants sont représentés par des codes courts.

**Statut : Choix de conception.**

## ANN-K-0018 — Identification du langage

RCI-S doit permettre d'identifier le langage auquel appartient le programme, avec au minimum un identifiant de langage et une version lorsque celle-ci est nécessaire à la compatibilité.

**Statut : Choix de conception.**

## ANN-K-0019 — Provenance du programme

RCI-S peut conserver une provenance minimale destinée à l'identification de l'environnement d'origine, à la détection d'incompatibilités, au diagnostic et à la traçabilité.

Elle peut comprendre :

```text
implantation d'origine
plateforme d'origine
environnement d'origine
version de l'implémentation si nécessaire
```

**Statut : Choix de conception.**

## ANN-K-0020 — Économie des métadonnées de provenance

La provenance est soumise au même principe de compacité que le reste du format :

1. les informations sont codées et non répétées sous forme de libellés longs ;
2. les valeurs courantes utilisent des identifiants courts ;
3. les informations reconstructibles ne sont pas répétées ;
4. les informations optionnelles peuvent être absentes ;
5. aucune donnée documentaire longue n'est embarquée par défaut.

La provenance doit rester raisonnable au regard de la taille du programme.

**Statut : Choix de conception.**

## ANN-K-0021 — Provenance optionnelle

Une implémentation peut ajouter des informations de provenance utiles à son exploitation. Elles doivent être facultatives, identifiées et ignorables lorsqu'elles ne concernent pas la compatibilité.

Aucune donnée personnelle, commentaire libre ou journal de génération ne doit être ajouté par défaut.

**Statut : Choix de conception.**

## ANN-K-0022 — Table des symboles sérialisée

Lorsque RCI-M utilise une table des symboles, RCI-S doit pouvoir la conserver. Chaque nom nécessaire à la décompilation n'est conservé qu'une seule fois ; les références utilisent des identifiants compacts.

**Statut : Choix de conception.**

## ANN-K-0023 — Organisation logique de RCI-S

```text
PROGRAMME SERIALISE
│
├── EN-TÊTE
├── IDENTIFICATION DU LANGAGE
├── PROVENANCE MINIMALE
├── TABLES
│   ├── symboles
│   └── constantes lorsque nécessaire
└── PROGRAMME
    ├── ligne → objet compilé → chaîne compilée
    └── ...
```

La présence effective des tables dépend de leur nécessité. Une table inutile ou vide ne doit pas être conservée.

**Statut : Choix de conception.**

## ANN-K-0024 — Sérialisation et restauration

La sérialisation réalise `RCI-M → RCI-S`. La restauration réalise `RCI-S → RCI-M`.

La restauration vérifie au minimum la signature, les versions, le langage, les caractéristiques nécessaires, l'intégrité structurelle, les références et la cohérence des tables. En cas d'échec, le programme courant n'est pas partiellement publié.

**Statut : Choix de conception.**

## ANN-K-0025 — Compatibilité

Une version ultérieure peut introduire des champs ou informations facultatifs sans modifier silencieusement la signification d'un champ existant.

Une implémentation doit pouvoir déterminer qu'un programme est directement compatible, compatible après conversion ou incompatible.

**Statut : Choix de conception.**

## ANN-K-0026 — Absence de dépendance aux adresses

RCI-S ne contient aucune adresse mémoire, pointeur C, adresse d'objet d'exécution ou position dépendante d'une implantation. Une référence persistante est exprimée par un identifiant logique ou une position définie par le format.

**Statut : Choix de conception.**

## ANN-K-0027 — Relation entre RCI-M et RCI-S

RCI-S conserve suffisamment d'informations pour reconstruire RCI-M sans dépendre de l'état mémoire de l'implémentation qui a produit le fichier.

```text
adresse mémoire ≠ identité logique
représentation physique d'exécution ≠ RCI-S
```

**Statut : Choix de conception.**

## ANN-K-0028 — Optimisations de stockage

Les optimisations sont autorisées lorsqu'elles ne modifient pas la représentation logique : tables partagées, références compactes, codage compact des numéros de ligne, regroupement des informations communes et suppression des informations reconstructibles.

Elles ne doivent pas rendre RCI-S dépendant d'une organisation mémoire particulière.

**Statut : Choix de conception.**

## ANN-K-0029 — Mémoire d'exécution

RCI-M n'impose pas que tous les objets soient matérialisés simultanément sous une forme physique volumineuse. Une implémentation peut utiliser des représentations compactes et reconstruire certaines structures à la demande.

**Statut : Choix de conception.**

## ANN-K-0030 — Conservation du texte source

Le texte source n'est pas une composante obligatoire de RCI-M ni de RCI-S. Il peut être conservé séparément pour l'édition, le diagnostic ou l'archivage, mais ne doit pas être ajouté automatiquement au fichier-programme compilé lorsque la représentation compilée suffit.

**Statut : Déduit.**

## ANN-K-0031 — Informations nécessaires à la décompilation

La compacité ne doit pas conduire à supprimer une information nécessaire à la décompilation. Une information n'est supprimée que si elle peut être reconstruite, obtenue par les tables ou déduite du contexte.

**Statut : Choix de conception.**

## ANN-K-0032 — Invariants RCI

1. RCI-M ne dépend d'aucune adresse mémoire.
2. RCI-S ne contient aucune adresse mémoire.
3. RCI-M est interprétable sans le texte source.
4. RCI-M reste décompilable selon les capacités définies.
5. RCI-S permet de reconstruire RCI-M.
6. Toute référence est résoluble dans son contexte.
7. Les tables sont cohérentes avec les références.
8. Toute information conservée dans RCI-S possède une utilité fonctionnelle, de compatibilité ou de provenance.
9. Une information reconstructible n'est pas dupliquée sans justification.
10. Les métadonnées de provenance ne sont pas nécessaires à l'exécution lorsqu'elles ne concernent pas la compatibilité.
11. Une incompatibilité de format est détectée avant l'acceptation du programme courant.
12. La représentation sérialisée reste indépendante de l'organisation mémoire de l'implémentation.
13. La représentation physique d'une implémentation ne constitue pas le format logique RCI.

## ANN-K-0033 — Registre des formats complémentaires

| Identifiant | Format | Nature | Statut |
|---|---|---|---|
| IV-FMT-0014 | RCI-M | logique | Choix de conception |
| IV-FMT-0015 | Élément RCI-M | logique | Choix de conception |
| IV-FMT-0016 | Table des symboles | logique | Déduit / choix de conception |
| IV-FMT-0017 | RCI-S | logique sérialisé | Choix de conception |
| IV-FMT-0018 | En-tête RCI-S | logique sérialisé | Choix de conception |
| IV-FMT-0019 | Provenance minimale | logique sérialisé | Choix de conception |

Aucun de ces formats ne constitue un format binaire historiquement attesté.

## ANN-K-0034 — Interfaces complémentaires

Les opérations logiques suivantes sont introduites :

```text
SERIALISER_PROGRAMME(programme)
RESTAURER_PROGRAMME(fichier)
VERIFIER_FORMAT(fichier)
VERIFIER_COMPATIBILITE(fichier)
```

Elles relèvent du bibliothécaire et des services de conservation du programme et ne modifient pas la responsabilité du compilateur ou de l'interpréteur.

**Statut : Choix de conception.**

## ANN-K-0035 — Algorithme de sérialisation

```text
SERIALISER_PROGRAMME(programme)

    vérifier programme
    construire en-tête
    construire identification du langage
    construire provenance minimale si disponible
    construire les tables nécessaires
    POUR chaque ligne du programme
        sérialiser numéro de ligne
        sérialiser objet compilé
        sérialiser représentation compilée
    FIN POUR
    vérifier cohérence
    produire fichier-programme
```

**Statut : Choix de conception.**

## ANN-K-0036 — Algorithme de restauration

```text
RESTAURER_PROGRAMME(fichier)

    vérifier signature
    vérifier version du format
    vérifier compatibilité RCI-M
    vérifier langage
    lire les tables
    vérifier les références
    POUR chaque objet compilé
        reconstruire représentation compilée
        vérifier objet
    FIN POUR
    vérifier ordre et unicité des lignes
    SI une vérification échoue
        produire diagnostic
        ne pas publier le programme courant
    FIN SI
    retourner programme courant
```

**Statut : Choix de conception.**

## ANN-K-0037 — Provenance et compatibilité

La provenance ne doit pas être confondue avec la compatibilité. Une plateforme d'origine différente ne rend pas nécessairement un programme incompatible ; la décision dépend des propriétés effectivement nécessaires à l'exécution.

**Statut : Choix de conception.**

## ANN-K-0038 — Évolution du format

Les versions du format doivent être distinguées des versions du langage, de RCI-M et de l'implémentation :

```text
version du langage ≠ version RCI-M ≠ version RCI-S ≠ version de l'implémentation
```

**Statut : Choix de conception.**

## ANN-K-0039 — Principe d'économie des moyens

L'ensemble RCI doit rester compatible avec l'objectif historique général d'une réalisation utilisant des moyens limités. La représentation compilée, le format sérialisé et les métadonnées doivent rester compacts et réalisables sur une architecture contrainte.

**Statut : Choix de conception.**

## ANN-K-0040 — Limites historiques

Les sources historiques établissent l'existence d'une chaîne codée L.S.E., son utilisation comme résultat de compilation, son utilisation par l'interpréteur, sa décompilation et la conservation du programme sous forme compilée.

Elles ne permettent pas d'établir complètement la structure interne des éléments, leur codage binaire, la structure exacte d'une table des symboles, le format binaire exact des fichiers-programmes ni l'existence historique des métadonnées de provenance définies ici.

Le format RCI-S ne doit donc pas être présenté comme le format historique original d'un fichier-programme L.S.E.

**Statut : Documenté pour les limites ; reconstruction pour les éléments définis ici.**

## ANN-K-0041 — Traçabilité

```text
HIS → EXI → VM → IV → RCI-M / RCI-S → implémentations
```

Les références principales sont EXI-0002, EXI-0003, EXI-0010 à EXI-0016 et EXI-0025. Les éléments VM et IV associés sont ceux du registre des formats et de la matrice de traçabilité du présent tome.

## ANN-K-0042 — Critères de conformité

Une implémentation conforme doit démontrer :

1. la création d'une RCI-M valide ;
2. son exécution ;
3. sa décompilation ;
4. sa sérialisation ;
5. sa restauration ;
6. l'absence de dépendance aux adresses mémoire ;
7. la vérification de la version du format ;
8. la vérification de compatibilité ;
9. la cohérence des tables ;
10. la conservation des informations nécessaires à la décompilation ;
11. la compacité raisonnable des métadonnées ;
12. la présence d'une provenance minimale lorsqu'elle est disponible ;
13. l'absence de dépendance fonctionnelle injustifiée à la provenance.

## ANN-K-0043 — Critère d'acceptation final

Deux implémentations indépendantes doivent pouvoir produire et interpréter une représentation compilée conforme au même modèle logique, la décompiler, sérialiser un programme et restaurer un programme sérialisé, sans dépendre d'une adresse mémoire, d'un pointeur, d'une organisation interne particulière du langage C ou d'une architecture processeur particulière.

La représentation physique reste libre tant qu'elle respecte RCI-M et RCI-S.

## ANN-K-0044 — Statut de l'annexe

La présente annexe constitue la spécification logique de la représentation compilée et de la conservation des programmes. Elle ne constitue ni une description historique du format binaire original, ni une spécification C, ni une spécification Apple IIGS ou ORCA/C.

Toute décision propre à une implantation relève de la documentation d'implémentation correspondante.

