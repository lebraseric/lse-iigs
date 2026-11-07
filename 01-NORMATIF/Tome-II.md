# Tome II — Machine abstraite reconstruite

# 0. Introduction

Le présent tome appartient à la collection :

**Reconstruction historique du Langage Symbolique d’Enseignement (L.S.E.)**

Il constitue le tome consacré à la reconstruction de la machine abstraite du L.S.E.

Contrairement au Tome I, qui constitue une édition documentaire consacrée aux éléments établis à partir des sources historiques, le présent tome a pour objet de déterminer une architecture abstraite permettant de rendre compte de ces éléments sous la forme d’un modèle cohérent et exploitable. Le Tome I précise que les sources documentent des fonctions telles que la compilation, l’interprétation, la décompilation, la gestion des commandes ou la gestion des fichiers, sans nécessairement décrire leur organisation interne complète.

La reconstruction proposée dans ce tome ne constitue donc pas une description d’une implantation historique particulière. Elle définit une machine abstraite minimale satisfaisant les contraintes établies par les sources historiques et formalisées dans le Catalogue des exigences.

Toute reconstruction présentée dans ce tome doit respecter les principes suivants :

- fidélité aux propriétés documentées ;
- séparation entre faits historiques, déductions et choix de conception ;
- absence de dépendance envers une technologie d’implémentation ;
- minimisation des mécanismes introduits.

La machine abstraite reconstruite doit être comprise comme un modèle fonctionnel permettant d’expliquer comment les propriétés historiques du L.S.E. peuvent être réalisées conjointement.

Les sources historiques décrivent notamment :

- un environnement conversationnel dans lequel l’utilisateur dialogue avec le système ;
- une compilation réalisée ligne par ligne ;
- une représentation compilée exécutable par un interpréteur ;
- une possibilité de décompilation à partir de cette représentation ;
- une organisation fonctionnelle en processeurs spécialisés.

Ces propriétés constituent les contraintes fondamentales auxquelles la machine abstraite doit répondre.

Le présent tome introduit donc des mécanismes abstraits identifiés par des identifiants stables de la forme VM-xxxx. Chaque mécanisme est décrit selon le gabarit normatif défini pour le Tome II.

La reconstruction ne cherche pas à retrouver le code source historique des systèmes L.S.E. Les sources disponibles ne permettent pas d’établir une organisation interne complète des différentes versions historiques. Elles permettent en revanche d’établir les fonctions attendues et les relations nécessaires entre ces fonctions.

La machine abstraite reconstruite constitue ainsi un niveau intermédiaire entre :

- l’architecture historique documentée dans le Tome I ;
- les exigences normatives du Catalogue des exigences ;
- les futures descriptions de conception détaillée et d’implémentation.

# 1. Principes de reconstruction

## 1.1 Objet de la reconstruction

La reconstruction a pour objet de définir l’ensemble minimal des mécanismes nécessaires pour satisfaire les propriétés historiques du L.S.E.

Elle ne vise pas à reproduire une implantation matérielle particulière, mais à reconstruire une organisation abstraite compatible avec les descriptions historiques.

Le principe directeur retenu est le suivant :

**Toute propriété historique documentée doit être expliquée par un mécanisme abstrait ; tout mécanisme abstrait introduit doit être nécessaire à la satisfaction d’une contrainte historique ou d’une exigence.**

Cette règle interdit l’introduction de mécanismes uniquement parce qu’ils seraient courants dans des architectures informatiques modernes.

Les sources historiques montrent que le L.S.E. est décrit comme un système organisé autour de plusieurs fonctions distinctes. Le manuel de 1975 distingue notamment le compilateur, le décompilateur, l’interpréteur, le bibliothécaire et l’analyseur de commandes.

La machine abstraite devra donc conserver cette séparation fonctionnelle.

## 1.2 Méthode de reconstruction

Chaque mécanisme reconstruit suit les étapes suivantes :

1. identification des paragraphes historiques concernés ;
2. identification des exigences applicables ;
3. création éventuelle de nouvelles exigences ;
4. analyse des contraintes ;
5. séparation entre éléments documentés et éléments non documentés ;
6. définition du mécanisme abstrait minimal ;
7. justification du choix retenu.

La reconstruction distingue trois catégories d’éléments.

### Éléments documentés

Un élément est documenté lorsqu’il résulte directement des sources historiques.

Exemple :

La compilation ligne à ligne est documentée par les manuels d’utilisation. La ligne est analysée dès la réception du caractère de fin de ligne et une représentation codée est produite lorsqu’elle est correcte.

### Éléments déduits

Un élément est déduit lorsqu’il n’est pas explicitement décrit mais constitue la conséquence nécessaire de plusieurs propriétés documentées.

Une déduction doit toujours rester limitée au strict nécessaire.

### Choix de conception

Un choix de conception correspond à une décision prise pour compléter les éléments insuffisamment documentés.

Il ne constitue pas une propriété historique.

Il doit être identifié comme tel.

## 1.3 Principes architecturaux hérités du Tome I

La reconstruction repose sur les principes architecturaux dégagés dans le Tome I.

Le premier principe est celui du dialogue permanent avec l’utilisateur. Le L.S.E. est conçu comme un environnement conversationnel permettant l’introduction progressive de lignes de programme, l’obtention immédiate de diagnostics et l’intervention sur l’exécution.

Le deuxième principe est celui de la compilation ligne par ligne. La ligne constitue l’unité de traitement du programme. Une modification d’une ligne ne nécessite pas la recompilation de l’ensemble du programme.

Le troisième principe est celui d’une représentation compilée adaptée à l’interprétation. La chaîne post-fixée est historiquement associée à une représentation facilitant l’interprétation, occupant peu d’espace et pouvant être décompilée.

Le quatrième principe est celui d’une séparation fonctionnelle entre les processeurs du système. Le compilateur, l’interpréteur, le décompilateur, le bibliothécaire et l’analyseur de commandes sont considérés comme des fonctions distinctes.

Ces principes constituent les contraintes initiales de la machine abstraite.

## 1.4 Première exigence introduite par la reconstruction

Les exigences existantes du Catalogue des exigences constituent la référence normative initiale. Les nouvelles exigences sont ajoutées progressivement lorsque l’analyse d’un mécanisme révèle une propriété nécessaire qui n’est pas encore représentée.

La première exigence disponible est attribuée au premier identifiant libre du catalogue.

---

EXI-0001

Titre

Séparation fonctionnelle des processeurs

Description

La machine abstraite doit distinguer les fonctions de compilation, d’interprétation, de décompilation, de gestion des fichiers et d’analyse des commandes.

Origine

HIS-3.11

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.222]

Statut

Documenté

Justification

Les sources historiques décrivent plusieurs processeurs spécialisés remplissant des fonctions distinctes. Cette séparation constitue une propriété architecturale documentée du système L.S.E.

Conséquences

Impacte :

- l’architecture générale de la machine abstraite ;
- le compilateur ;
- l’interpréteur ;
- le décompilateur ;
- le bibliothécaire ;
- l’analyseur de commandes.

Utilisé par

VM-0001

Observations

Cette exigence ne préjuge pas de l’organisation matérielle ou logicielle interne des implantations historiques.

---

## 1.5 Principes de minimalité

La machine abstraite reconstruite doit contenir uniquement les mécanismes nécessaires.

Une propriété historique ne justifie pas automatiquement l’introduction d’un mécanisme interne détaillé.

Par exemple, les sources établissent l’existence d’une chaîne codée exécutable par l’interpréteur et d’une opération de décompilation permettant de retrouver une représentation textuelle. Elles ne décrivent cependant pas le format exact de cette représentation ni l’algorithme précis de transformation.

La reconstruction devra donc définir :

- l’existence d’une représentation compilée ;
- les opérations nécessaires sur cette représentation ;
- les relations fonctionnelles entre compilation, exécution et décompilation ;

mais ne devra pas imposer un format interne non documenté.

# 2. Architecture générale de la machine abstraite

## 2.1 Objet

La machine abstraite générale décrit l’organisation des composants nécessaires à l’exécution d’un programme L.S.E.

Elle définit les relations entre :

- l’utilisateur ;
- l’environnement conversationnel ;
- le compilateur ;
- la représentation compilée ;
- l’interpréteur ;
- le décompilateur ;
- l’environnement d’exécution ;
- les objets du programme.

## 2.2 Contraintes historiques

Les paragraphes historiques concernés sont :

- HIS-2.1 — Finalité générale ;
- HIS-2.3 — Modification progressive des programmes ;
- HIS-3.11 — Les processeurs du système ;
- HIS-7 — La chaîne post-fixée ;
- HIS-15 — Principes architecturaux dégagés.

Les sources principales sont :

[INRDP75 §3]

[INRDP75 §3.3]

[NOYELLE89 p.217-222]

[LSE15 §1]

## 2.3 Exigences concernées

EXI-0001 — Séparation fonctionnelle des processeurs.

EXI relative à la compilation ligne par ligne.

EXI relative à la représentation compilée.

EXI relative à l’interprétation du programme compilé.

(Les identifiants exacts seront repris après consolidation complète du catalogue.)

## 2.4 Analyse

### Éléments documentés

Les sources décrivent :

- un compilateur produisant une chaîne codée ;
- un interpréteur exécutant cette chaîne ;
- un décompilateur transformant la chaîne codée en caractères ;
- des processeurs spécialisés accessibles depuis l’environnement conversationnel.

### Éléments non documentés

Les sources ne décrivent pas :

- la structure interne complète de la chaîne codée ;
- le protocole interne entre les processeurs ;
- l’organisation exacte des données manipulées ;
- la structure mémoire complète du programme courant.



### Contraintes

La machine abstraite doit donc :

- séparer les fonctions principales ;
- conserver une représentation compilée intermédiaire ;
- permettre l’exécution par interprétation ;
- permettre la reconstruction textuelle ;
- rester indépendante d’une implantation particulière.

## 2.5 Reconstruction retenue

La machine abstraite est organisée autour des composants suivants :

- un environnement conversationnel ;
- un compilateur ;
- une représentation compilée ;
- un interpréteur ;
- un décompilateur ;
- un environnement d’exécution.

Le compilateur transforme les lignes de programme validées en objets compilés.

L’interpréteur consomme ces objets compilés afin d’exécuter le programme.

Le décompilateur exploite la représentation compilée afin de produire une représentation textuelle équivalente.

Aucun format interne n’est imposé à ce stade.

## 2.6 Statut

Déduit

## 2.7 Justification

Cette organisation est la reconstruction minimale compatible avec les descriptions historiques.

Elle reprend les fonctions explicitement documentées sans ajouter d’organisation interne non nécessaire.

## 2.8 Conséquences

La reconstruction influence :

- la compilation ;
- la Chaîne Post-Fixée ;
- l’interpréteur ;
- la décompilation ;
- la gestion du programme courant.

## 2.9 Traçabilité

HIS-3.11 → EXI-0001 → VM-0001

HIS-7 → exigences CPF → VM-0001

(La suite reprend au premier mécanisme VM détaillé.)

# 3. Objets compilés

Le système historique décrit par les sources ne conserve pas le programme uniquement sous sa forme textuelle. Une ligne acceptée par le compilateur donne lieu à une représentation codée, directement exploitable par l’interpréteur. Cette représentation peut être conservée, exécutée et transformée à nouveau en caractères par le décompilateur. [INRDP75 §3.3] [INRDP75 §4-2] [INRDP75 §4-4]

Le présent chapitre définit l’objet abstrait minimal nécessaire pour représenter cette situation dans la machine reconstruite. La reconstruction ne suppose ni un format binaire particulier, ni une structure mémoire historique, ni une identité certaine entre la « chaîne codée » du L.S.E. et la « chaîne post-fixée » décrite explicitement pour le LSD. Elle définit seulement les propriétés qu'un objet compilé doit posséder pour satisfaire l'ensemble des contraintes historiques retenues.

## 3.1 Objet compilé par ligne

### VM-0301
Objet compilé

## Objet

L'objet compilé représente, dans la machine abstraite, le résultat accepté de la compilation d'une ligne de programme.

Il constitue la forme non textuelle à partir de laquelle cette ligne peut être exécutée par l'interpréteur et, lorsque la décompilation est requise, être reconvertie en une représentation textuelle du langage.

L'objet compilé ne constitue pas nécessairement une copie du texte source. La reconstruction retenue sépare explicitement la représentation source de la représentation compilée.

## Contraintes historiques

Les sources historiques décrivent la compilation d'une ligne et indiquent que le programme obtenu après compilation ligne à ligne se présente sous la forme d'une chaîne codée exécutable par l'interpréteur. [INRDP75 §3.3]

Le décompilateur transforme cette chaîne codée en une chaîne de caractères, notamment pour permettre le listage du programme courant. [INRDP75 §3.3] [INRDP75 §4-2]

Le programme peut également être rangé sous forme codée dans un fichier-programme. [INRDP75 §4-4]

Dans le récit de la conception des langages LSx, la représentation compilée du LSD est décrite comme une chaîne post-fixée issue de la compilation ligne à ligne. [NOYELLE89 p.217-220]

## Exigences concernées

### EXI-0001

Titre

Compilation indépendante des lignes

Description

Chaque ligne de programme est compilée indépendamment des autres lignes dès sa validation.

Origine

HIS-6.2

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.217]

Statut

Documenté

Justification

Le système permet l'ajout, la modification ou la suppression d'une ligne sans recompilation du reste du programme.

Conséquences

Le compilateur travaille ligne par ligne.

Une représentation compilée est produite pour chaque ligne acceptée.

Utilisé par

VM-0301

VM-0302

VM-0304

VM-0501

Observations

Cette exigence est le point d'appui général de la reconstruction par objets compilés.

### EXI-0002

Titre

Représentation compilée exécutable

Description

Toute ligne de programme acceptée par la compilation possède une représentation compilée permettant son exécution par l'interpréteur.

Origine

HIS-4.5.1

HIS-5.2

Sources historiques

[INRDP75 §3.3]

Statut

Documenté

Justification

Le manuel décrit la chaîne codée comme le résultat de la compilation ligne à ligne et comme la forme exécutable par l'interpréteur.

Conséquences

La machine abstraite doit conserver, pour chaque ligne acceptée, une représentation compilée distincte du texte source.

Utilisé par

VM-0301

VM-0404

VM-0602

Observations

L'exigence ne fixe aucun format interne.

### EXI-0003

Titre

Décompilabilité de la représentation compilée

Description

Toute représentation compilée conservée comme programme courant doit pouvoir être transformée en une représentation textuelle du programme suffisamment complète pour permettre son listage.

Origine

HIS-4.5.2

HIS-5.3

Sources historiques

[INRDP75 §3.3]

[INRDP75 §4-2]

[NOYELLE89 p.220]

Statut

Documenté

Justification

Le décompilateur transforme la chaîne codée en caractères et permet notamment de lister le programme courant. Noyelle décrit également la possibilité de recréer le texte source à partir de la chaîne post-fixée.

Conséquences

La représentation compilée doit contenir, directement ou indirectement dans l'organisation abstraite du programme, les informations nécessaires à la restitution textuelle.

Utilisé par

VM-0301

VM-0405

VM-1801

Observations

L'exigence ne signifie pas que le texte source original doit être conservé séparément.

## Analyse

### Éléments documentés

La compilation transforme chaque ligne acceptée en une forme codée.

Cette forme codée est utilisable par l'interpréteur.

Elle peut être soumise au décompilateur.

Elle peut être conservée sous forme codée dans un fichier-programme.

Ces propriétés sont directement documentées. [INRDP75 §3.3] [INRDP75 §4-2] [INRDP75 §4-4]

Le récit historique de Noyelle fournit une information complémentaire : dans le LSD, la structure du programme permet une représentation en notation polonaise inverse, désignée comme une chaîne post-fixée, à la fois interprétable efficacement, compacte et décompilable. [NOYELLE89 p.219-220]

### Éléments non documentés

Les sources ne décrivent pas la structure interne d'un objet compilé du L.S.E.

Elles ne précisent pas si la chaîne codée contient à elle seule toutes les informations permettant la décompilation ou si certaines informations associées au programme courant sont conservées séparément.

Elles ne précisent pas davantage si le numéro de ligne est encodé dans la chaîne, conservé dans une structure externe, ou représenté selon une autre organisation.

Elles ne donnent ni le format exact des opérations codées, ni la représentation des constantes, ni celle des références aux variables ou aux lignes.

### Contraintes

La reconstruction doit permettre simultanément :

- l'exécution de la ligne compilée ;
- la conservation de la ligne dans le programme courant ;
- la restitution du programme par décompilation ;
- le remplacement d'une ligne sans recompilation des autres ;
- le rangement du programme sous une forme codée.

La reconstruction ne doit pas imposer la conservation du texte source original comme condition nécessaire du fonctionnement de la machine.

## Reconstruction retenue

La machine abstraite représente chaque ligne de programme acceptée par un objet compilé.

Un objet compilé comporte abstraitement deux parties :

- une identification de la ligne dans le programme courant ;
- une représentation compilée exécutable.

L'identification de la ligne n'est pas considérée comme appartenant nécessairement à la chaîne compilée elle-même. Elle appartient à l'objet abstrait qui associe la ligne de programme à sa représentation compilée.

La représentation compilée constitue le contenu transmis à l'interpréteur pour l'exécution de la ligne.

L'objet compilé ne conserve pas le texte source comme donnée obligatoire. Lorsque le programme doit être listé ou décodé, la machine utilise la représentation compilée et les informations abstraites qui lui sont associées pour produire la représentation textuelle.

Cette séparation est retenue parce qu'elle satisfait les contraintes documentées sans imposer une organisation interne non attestée par les sources.

Elle permet notamment de distinguer trois notions :

- le texte source fourni lors de l'entrée de la ligne ;
- l'objet compilé conservé dans le programme courant ;
- la représentation textuelle produite ultérieurement par décompilation.

La troisième forme peut être équivalente au texte source du point de vue du langage, sans être nécessairement identique caractère par caractère à la chaîne initialement saisie lorsque plusieurs écritures syntaxiques équivalentes seraient possibles.

La machine abstraite ne conserve donc pas, par principe, une copie indépendante du texte source.

## Statut

Déduit

## Justification

L'existence d'une représentation compilée exécutable et décompilable est documentée. La distinction abstraite entre l'identification de la ligne et son contenu compilé n'est pas documentée comme telle, mais constitue la manière la plus simple de représenter les deux fonctions nécessaires : l'organisation du programme selon les numéros de lignes et l'exécution de la représentation compilée.

Cette reconstruction évite d'imposer que le numéro de ligne soit nécessairement codé dans la chaîne elle-même, ce que les sources ne permettent pas d'établir.

Elle évite également de conserver systématiquement le texte source, puisque la possibilité de décompilation est précisément documentée comme permettant la restitution du programme à partir de sa représentation compilée.

## Conséquences

### CPF

La chaîne post-fixée ou, plus généralement, la chaîne codée constitue le contenu compilé de l'objet.

### Interpréteur

L'interpréteur reçoit la représentation compilée de l'objet.

### Compilation

La réussite de la compilation produit un objet compilé.

### Décompilation

Le décompilateur travaille sur la représentation compilée et sur les informations abstraites nécessaires à la restitution du programme.

### Mémoire

La machine abstraite doit distinguer le programme courant des représentations temporaires produites pendant la compilation ou la décompilation.

### Gestion des programmes

Le programme courant est une collection ordonnée d'objets compilés, et non une simple chaîne de caractères.

## Traçabilité

HIS-4.5.1 → EXI-0002 → VM-0301

HIS-4.5.2 → EXI-0003 → VM-0301

HIS-5.2 → EXI-0001, EXI-0002 → VM-0301

HIS-5.3 → EXI-0003 → VM-0301

CPF-7.1 → EXI-0002, EXI-0003 → VM-0301

---

## 3.2 Validation d'un objet compilé

### VM-0302
Validation d'un objet compilé

## Objet

Définir les conditions dans lesquelles une ligne nouvellement compilée peut être intégrée au programme courant.

## Contraintes historiques

Une ligne est analysée dès la réception du caractère de fin de ligne.

Une erreur de syntaxe est signalée immédiatement.

Une ligne incorrecte n'est pas prise en compte.

Une ligne correcte est mémorisée et placée à son rang selon son numéro.

[LSE15 §1] [INRDP75 §« Présentation d'un programme L.S.E. »]

## Exigences concernées

### EXI-0004

Titre

Rejet atomique d'une ligne incorrecte

Description

Une ligne dont la compilation échoue pour une erreur syntaxique ne modifie pas le programme courant.

Origine

HIS-5.2

HIS-6.5

Sources historiques

[INRDP75 §3.3]

[INRDP75 Annexe II]

[LSE15 §1]

Statut

Documenté

Justification

Les sources indiquent qu'une ligne syntaxiquement incorrecte n'est pas prise en compte et doit être corrigée puis réintroduite.

Conséquences

L'état antérieur du programme courant est préservé lorsqu'une ligne échoue à la compilation.

Utilisé par

VM-0302

VM-0303

VM-0602

Observations

L'exigence porte sur le résultat observable de la compilation et non sur l'algorithme interne du compilateur.

## Analyse

### Éléments documentés

Le système traite la ligne au moment où la fin de ligne est reçue.

Une erreur syntaxique est signalée immédiatement.

La ligne incorrecte n'est pas intégrée au programme courant.

La correction doit être effectuée en réintroduisant la ligne.

Les lignes précédemment introduites restent disponibles.

### Éléments non documentés

Les sources ne décrivent pas sous quelle forme le compilateur produit un résultat intermédiaire avant l'acceptation définitive de la ligne.

Elles ne précisent pas si la représentation compilée est construite progressivement ou entièrement avant validation.

Elles ne décrivent pas la gestion interne des ressources engagées pendant une compilation qui échoue.

### Contraintes

Une erreur de compilation ne doit pas laisser dans le programme courant une représentation partiellement compilée.

Une ligne incorrecte ne doit pas remplacer une ligne correcte portant le même numéro.

## Reconstruction retenue

La compilation d'une ligne est considérée comme une opération transactionnelle au niveau de la machine abstraite.

Le compilateur reçoit la ligne.

Il produit soit :

- un objet compilé valide ;
- un diagnostic d'erreur.

L'objet compilé n'est intégré au programme courant que lorsque la compilation est achevée avec succès.

En cas d'échec, aucun changement durable n'est apporté au programme courant.

Cette notion de transaction est une propriété abstraite du résultat observable. Elle ne préjuge pas de la manière dont une réalisation particulière gère ses zones de travail.

## Statut

Déduit

## Justification

Les sources documentent directement l'absence d'intégration d'une ligne syntaxiquement incorrecte. La notion d'une opération atomique est la reconstruction minimale de cette propriété lorsqu'elle est transposée dans une machine abstraite indépendante de toute implémentation.

## Conséquences

### Compilation

Le compilateur doit distinguer le résultat temporaire de compilation du programme courant accepté.

### Programme courant

Le programme courant n'est modifié qu'après validation.

### Gestion des erreurs

Un diagnostic syntaxique ne détruit pas l'état antérieur du programme.

### Exécution

Une ligne rejetée ne peut pas être exécutée comme partie du programme courant.

## Traçabilité

HIS-5.2 → EXI-0004 → VM-0302

HIS-6.5 → EXI-0004 → VM-0302

---

## 3.3 Programme courant

### VM-0303
Programme courant

## Objet

Définir le programme courant comme l'ensemble abstrait des objets compilés actuellement associés à la session de l'utilisateur.

## Contraintes historiques

Le programme est constitué de lignes numérotées.

Les lignes sont compilées au fur et à mesure de leur introduction.

Une ligne correcte est mémorisée.

Le programme peut être exécuté, listé, modifié, interrompu puis repris.

[LSE15 §1] [INRDP75 §3] [INRDP75 §4]

## Exigences concernées

### EXI-0005

Titre

Programme courant constitué de représentations compilées

Description

Le programme courant conserve, pour chaque ligne acceptée, une représentation compilée permettant son exécution et sa décompilation.

Origine

HIS-4.5.1

HIS-4.5.2

HIS-6.2

Sources historiques

[INRDP75 §3.3]

[INRDP75 §4-2]

[INRDP75 §4-4]

[LSE15 §1]

Statut

Déduit

Justification

Les sources décrivent un programme courant qui est compilé ligne par ligne, exécuté à partir de la chaîne codée et décompilé pour le listage. La reconstruction doit donc disposer d'un état abstrait conservant les représentations compilées des lignes acceptées.

Conséquences

Le programme courant devient l'objet central partagé par la compilation, l'interprétation, la décompilation et la gestion des programmes.

Utilisé par

VM-0303

VM-0304

VM-0305

VM-0404

VM-0405

VM-0501

VM-0602

Observations

Cette exigence ne définit pas la structure de stockage.

## Analyse

### Éléments documentés

Le programme courant existe comme objet de travail de l'utilisateur.

Il peut être modifié ligne par ligne.

Il peut être exécuté.

Il peut être listé.

Il peut être rangé sous une forme codée.

Il peut être remplacé par un programme chargé depuis la bibliothèque.

### Éléments non documentés

Les sources ne décrivent pas la structure interne du programme courant.

Elles ne disent pas si celui-ci est une liste, une table, un ensemble d'entrées, une chaîne unique ou une autre structure.

Elles ne décrivent pas non plus si l'ordre des lignes est une propriété du stockage lui-même ou une propriété reconstruite lors de l'exécution et du listage.

### Contraintes

La machine doit pouvoir :

- retrouver une ligne par son numéro ;
- remplacer une ligne ;
- supprimer une ligne ;
- parcourir les lignes dans l'ordre d'exécution ;
- décompiler les lignes dans leur ordre de présentation ;
- transmettre l'ensemble au mécanisme d'exécution.

## Reconstruction retenue

Le programme courant est reconstruit comme une collection d'objets compilés indexés par leur numéro de ligne.

Chaque entrée possède :

- un numéro de ligne ;
- une représentation compilée.

La collection est ordonnée logiquement par numéro de ligne.

La représentation ordonnée est une propriété abstraite ; elle n'impose pas qu'une implémentation conserve physiquement les objets dans cet ordre.

L'absence d'une ligne correspondant à un numéro donné signifie que cette ligne n'appartient pas au programme courant.

Le programme courant ne contient pas de ligne dont la compilation a échoué.

Le numéro de ligne constitue l'identité logique de l'objet compilé au sein du programme courant.

## Statut

Déduit

## Justification

Cette organisation est la plus simple qui permette de satisfaire simultanément la modification ligne par ligne, le remplacement par numéro, l'exécution dans l'ordre croissant et la décompilation du programme.

Elle n'impose aucun format historique particulier.

## Conséquences

### Compilation

La compilation réussie fournit une entrée du programme courant.

### Interpréteur

L'interpréteur parcourt les objets compilés dans l'ordre défini par leurs numéros.

### Décompilateur

Le décompilateur parcourt le même ordre pour restituer le programme.

### Mémoire

Le programme courant constitue un état persistant de la session entre deux opérations.

### Bibliothécaire

Le rangement d'un programme porte sur l'état abstrait du programme courant, et non sur une copie obligatoire du texte source.

## Traçabilité

HIS-6.2 → EXI-0001, EXI-0005 → VM-0303

HIS-4.5.1 → EXI-0002, EXI-0005 → VM-0303

HIS-4.5.2 → EXI-0003, EXI-0005 → VM-0303

---

## 3.4 Remplacement d'une ligne

### VM-0304
Remplacement par numéro de ligne

## Objet

Définir la modification d'un programme courant lorsqu'une nouvelle ligne porte le numéro d'une ligne déjà existante.

## Contraintes historiques

Deux lignes de même numéro ne coexistent pas.

Une nouvelle ligne portant un numéro déjà présent remplace la précédente lorsqu'elle est correctement introduite.

[LSE15 §1]

## Exigences concernées

### EXI-0006

Titre

Unicité du numéro de ligne

Description

Le programme courant ne contient au plus qu'une ligne pour chaque numéro de ligne.

Origine

HIS-6.2

Sources historiques

[INRDP75 §« Présentation d'un programme L.S.E. »]

[LSE15 §1]

Statut

Documenté

Justification

Les sources indiquent qu'une nouvelle ligne portant le même numéro remplace la précédente et que deux lignes de même numéro ne coexistent pas.

Conséquences

Le numéro de ligne peut servir d'identifiant logique stable d'une ligne du programme courant.

Utilisé par

VM-0303

VM-0304

VM-0601

Observations

La façon dont cette unicité est réalisée en mémoire n'est pas définie.

## Analyse

### Éléments documentés

Une nouvelle ligne correctement compilée peut remplacer une ligne existante.

Le remplacement porte sur la ligne entière.

Les autres lignes du programme ne doivent pas être réintroduites.

### Éléments non documentés

Les sources ne précisent pas comment l'ancienne représentation compilée est éliminée.

Elles ne décrivent pas si elle est immédiatement libérée, conservée temporairement ou remplacée par partage de ressources.

### Contraintes

Après acceptation d'une nouvelle ligne, le programme courant doit refléter la nouvelle représentation compilée à ce numéro.

L'ancienne représentation ne doit plus être utilisée pour l'exécution normale du programme courant.

## Reconstruction retenue

Lorsqu'une nouvelle ligne est validée et qu'un objet compilé portant le même numéro existe déjà, l'ancien objet est remplacé logiquement par le nouveau.

L'opération est considérée comme indivisible au niveau du programme courant.

L'état résultant contient une seule ligne à ce numéro.

Aucune autre ligne du programme courant n'est modifiée.

## Statut

Déduit

## Justification

Le remplacement atomique est la reconstruction minimale de la propriété historique selon laquelle une nouvelle ligne correcte prend immédiatement la place de la précédente.

## Conséquences

### Programme courant

Le remplacement est local au numéro concerné.

### Interpréteur

L'exécution ultérieure utilise la nouvelle représentation.

### Décompilateur

Le listage restitue la nouvelle ligne.

### Gestion mémoire

Les règles de récupération des ressources de l'ancien objet relèvent du mécanisme de gestion mémoire.

## Traçabilité

HIS-6.2 → EXI-0006 → VM-0304

---

## 3.5 Suppression d'une ligne

### VM-0305
Suppression d'une ligne du programme courant

## Objet

Définir l'état du programme courant après suppression d'une ligne existante.

## Contraintes historiques

Les sources historiques décrivent l'ajout et la modification des lignes et mentionnent la possibilité de supprimer une ligne dans le cadre de la compilation ligne à ligne des langages de la famille LSx.

[NOYELLE89 p.217-218]

Le programme courant peut également être détruit globalement par des commandes du système.

[INRDP75 §1-1]

## Exigences concernées

### EXI-0007

Titre

Suppression logique d'une ligne

Description

La suppression d'une ligne retire l'objet compilé correspondant du programme courant sans modifier les autres lignes.

Origine

HIS-6.2

Sources historiques

[NOYELLE89 p.217-218]

Statut

Documenté

Justification

La possibilité d'ajouter, modifier ou supprimer une ligne sans reprendre la traduction de l'ensemble du programme est explicitement rapportée dans le récit historique de Noyelle.

Conséquences

La structure du programme courant doit permettre la disparition indépendante d'un objet compilé.

Utilisé par

VM-0303

VM-0305

VM-0601

Observations

Les sources ne précisent pas la syntaxe exacte de suppression dans toutes les implantations.

## Analyse

### Éléments documentés

La suppression d'une ligne est historiquement compatible avec la compilation ligne à ligne.

Elle ne nécessite pas la recompilation des autres lignes.

### Éléments non documentés

Les sources ne décrivent pas le mécanisme interne de suppression.

Elles ne précisent pas le moment où l'espace précédemment associé à l'objet devient disponible.

### Contraintes

La suppression doit être locale.

Les autres objets compilés doivent rester inchangés.

La ligne supprimée ne doit plus être exécutée.

Elle ne doit plus apparaître dans le listage courant.

## Reconstruction retenue

La suppression retire l'entrée correspondant au numéro de ligne du programme courant.

L'objet compilé associé cesse d'appartenir au programme courant.

Les autres objets sont conservés sans modification.

La récupération des ressources précédemment utilisées par l'objet est différée au mécanisme de gestion mémoire.

## Statut

Déduit

## Justification

Cette reconstruction est la conséquence directe de la propriété historique de modification indépendante des lignes.

## Conséquences

### Compilation

La compilation d'une ligne n'est jamais nécessaire pour maintenir la validité des autres objets compilés.

### Interpréteur

Une ligne supprimée est absente du parcours d'exécution.

### Décompilateur

Une ligne supprimée n'est pas restituée.

### Mémoire

La suppression peut rendre des ressources réutilisables.

## Traçabilité

HIS-6.2 → EXI-0007 → VM-0305

---

## 3.6 Ordre logique du programme courant

### VM-0306
Ordre des objets compilés

## Objet

Définir l'ordre logique selon lequel les objets compilés d'un programme courant sont parcourus pour l'exécution et la décompilation.

## Contraintes historiques

Les programmes sont constitués de lignes numérotées.

L'ordre des numéros détermine l'ordre du programme.

L'exécution se fait selon l'ordre croissant des numéros de lignes, indépendamment de l'ordre dans lequel les lignes ont été introduites.

[INRDP75 §« Présentation d'un programme L.S.E. »]

[LSE15 §1]

[NOYELLE89 p.218-219]

## Exigences concernées

### EXI-0008

Titre

Ordre logique croissant des lignes

Description

Les objets compilés d'un programme courant sont parcourus selon l'ordre croissant de leurs numéros de ligne lorsqu'aucun mécanisme de contrôle du programme ne provoque un autre transfert d'exécution.

Origine

HIS-6.2

HIS-8.2

Sources historiques

[INRDP75 §« Présentation d'un programme L.S.E. »]

[LSE15 §1]

Statut

Documenté

Justification

Le numéro de ligne constitue dans les sources l'identification permettant de replacer les lignes dans l'ordre du programme.

Conséquences

Le programme courant doit fournir un ordre logique déterminé indépendamment de l'ordre d'entrée des lignes.

Utilisé par

VM-0306

VM-0603

VM-2101

Observations

Les instructions de contrôle et les procédures peuvent modifier le flux d'exécution ; elles ne remettent pas en cause l'ordre de base du programme.

## Analyse

### Éléments documentés

Une ligne peut être introduite après une autre sans respecter l'ordre numérique.

Le système la replace selon son numéro.

Le programme est exécuté selon l'ordre croissant des numéros.

### Éléments non documentés

Les sources ne précisent pas l'organisation interne permettant le parcours ordonné.

Elles ne définissent pas la représentation d'une adresse d'exécution.

Elles ne précisent pas comment les branchements sont représentés dans la chaîne compilée.

### Contraintes

La machine abstraite doit fournir un ordre total des lignes existantes.

Cet ordre est fondé sur les numéros de ligne.

Les ajouts ou remplacements ne doivent pas modifier l'ordre logique des autres lignes.

## Reconstruction retenue

Le programme courant est ordonné logiquement par numéro de ligne.

L'exécution séquentielle commence au premier numéro existant et progresse vers les numéros suivants, sauf lorsqu'une instruction ou un mécanisme de contrôle modifie explicitement le flux.

La représentation interne de cet ordre n'est pas fixée.

Une implantation pourra conserver les objets dans une structure directement ordonnée ou maintenir une structure d'identification distincte de l'ordre d'exécution.

## Statut

Déduit

## Justification

L'ordre croissant des numéros est explicitement documenté. La seule reconstruction porte sur la représentation abstraite de cet ordre, nécessaire pour séparer la propriété du langage de toute structure de données particulière.

## Conséquences

### Interpréteur

L'interpréteur dispose d'un ordre de parcours initial.

### Compilation

L'ordre d'arrivée des lignes n'affecte pas l'ordre logique du programme.

### Décompilation

Le programme est restitué selon l'ordre des numéros.

### Contrôle

Les branchements doivent pouvoir remplacer le parcours séquentiel par un autre flux.

## Traçabilité

HIS-6.2 → EXI-0008 → VM-0306

HIS-8.2 → EXI-0008 → VM-0306

---

## 3.7 Conservation sous forme compilée

### VM-0307
Conservation du programme compilé

## Objet

Définir la relation entre le programme courant et sa conservation sous forme de fichier-programme.

## Contraintes historiques

La commande RANGER conserve le programme courant sur disque sous forme codée et non sous forme de chaîne de caractères L.S.E.

La commande APPELER charge ensuite ce programme et en fait le programme courant.

[INRDP75 §4-4]

[LSE15 §9-5]

## Exigences concernées

### EXI-0009

Titre

Persistance du programme sous forme compilée

Description

Un programme courant peut être conservé sous une forme compilée permettant son rechargement ultérieur comme programme courant sans nécessiter la conservation obligatoire de son texte source.

Origine

HIS-11.2

HIS-11.5

Sources historiques

[INRDP75 §4-4]

[LSE15 §9-5]

Statut

Documenté

Justification

Le manuel INRDP indique explicitement que le programme est rangé sous forme codée et non sous forme d'une chaîne de caractères L.S.E.

Conséquences

La forme de conservation du programme doit être compatible avec la représentation compilée utilisée par l'interpréteur.

Utilisé par

VM-0307

VM-0404

VM-1701

Observations

La nature exacte du fichier-programme historique n'est pas reconstruite au niveau binaire.

## Analyse

### Éléments documentés

Le programme courant peut être rangé sous forme codée.

Le programme peut être rechargé.

Après chargement, il devient le programme courant.

Le rangement et le chargement ne nécessitent pas, dans la documentation, une recompilation du programme source.

### Éléments non documentés

Les sources ne précisent pas si le fichier-programme est une copie exacte de la représentation présente en mémoire.

Elles ne décrivent pas son format.

Elles ne précisent pas les éventuelles informations d'en-tête ou d'identification.

### Contraintes

Le programme sauvegardé doit conserver une représentation exécutable.

Le rechargement doit restaurer un programme courant fonctionnel.

## Reconstruction retenue

Le rangement sérialise abstraitement le programme courant sous sa représentation compilée.

Le chargement restitue une collection d'objets compilés équivalente au programme courant précédemment rangé.

Le mécanisme de conservation ne dépend pas du texte source.

Le format de sérialisation n'est pas fixé par la machine abstraite.

## Statut

Déduit

## Justification

La propriété de conservation sous forme codée est documentée. La définition d'une opération abstraite de sérialisation et de restauration est nécessaire pour donner un sens indépendant de toute technologie au rangement et au chargement.

## Conséquences

### Bibliothèque des programmes

Le fichier-programme contient une représentation compilée du programme.

### Interpréteur

Un programme rechargé peut être directement exécuté.

### Compilation

Aucune recompilation du texte source n'est requise lors du chargement d'un fichier-programme.

### Décompilation

Le programme rechargé reste décompilable.

## Traçabilité

HIS-11.2 → EXI-0009 → VM-0307

HIS-11.5 → EXI-0009 → VM-0307

---

# 4. La Chaîne Post-Fixée

La documentation historique emploie deux formulations qu'il convient de conserver distinctes.

Le manuel de 1975 décrit le L.S.E. comme produisant, après compilation, une « chaîne codée » exécutable par l'interpréteur et transformable par le décompilateur.

Le récit de Yves Noyelle décrit explicitement, pour le LSD, une représentation en notation polonaise inverse appelée « chaîne post-fixée ». Cette représentation est présentée comme facile à interpréter efficacement, peu encombrante et décompilable. [INRDP75 §3.3] [NOYELLE89 p.219-220]

La continuité historique entre LSD et L.S.E. rend plausible une reconstruction de la représentation compilée du L.S.E. sous une forme post-fixée. Elle ne permet cependant pas d'affirmer historiquement que les chaînes codées des versions L.S.E. documentées en 1975 et 1978 sont identiques à la représentation du LSD décrite en 1989. [NOYELLE89 p.223-225] [INRDP75 §3.3] [LSE15 §9-5]

Le présent chapitre établit donc la représentation post-fixée comme modèle abstrait de reconstruction, tout en maintenant explicitement son statut de reconstruction lorsqu'elle dépasse les propriétés directement documentées du L.S.E.

## 4.1 Nature de la représentation compilée

### VM-0401
Représentation compilée abstraite

## Objet

Définir la représentation abstraite produite par la compilation et utilisée comme support commun de l'exécution et de la décompilation.

## Contraintes historiques

La chaîne codée résulte de la compilation.

Elle est exécutable par l'interpréteur.

Elle peut être décompilée.

La représentation post-fixée du LSD possède ces propriétés et est décrite comme adaptée à une interprétation efficace et à la limitation de l'occupation mémoire.

[INRDP75 §3.3]

[NOYELLE89 p.219-220]

## Exigences concernées

### EXI-0010

Titre

Représentation compilée commune à l'exécution et à la décompilation

Description

La représentation compilée d'un programme doit pouvoir être utilisée à la fois comme support de son exécution et comme source de sa décompilation.

Origine

HIS-4.5.1

HIS-4.5.2

Sources historiques

[INRDP75 §3.3]

Statut

Déduit

Justification

Le manuel décrit une même chaîne codée comme forme exécutable par l'interpréteur et transformable en caractères par le décompilateur.

Conséquences

La machine abstraite ne doit pas maintenir obligatoirement deux représentations compilées indépendantes, l'une destinée à l'exécution et l'autre à la décompilation.

Utilisé par

VM-0401

VM-0404

VM-0405

Observations

Cette exigence n'interdit pas qu'une implantation réalise des transformations temporaires.

## Analyse

### Éléments documentés

La chaîne codée est produite par le compilateur.

L'interpréteur l'exécute.

Le décompilateur la transforme en caractères.

### Éléments non documentés

Les sources ne permettent pas de savoir si l'interpréteur et le décompilateur parcourent exactement les mêmes unités élémentaires.

Elles ne précisent pas la représentation des opérandes, des constantes, des noms ou des références.

### Contraintes

Une représentation compilée unique doit être suffisante pour les deux fonctions.

La reconstruction doit rester indépendante d'un format matériel.

## Reconstruction retenue

La machine abstraite considère la représentation compilée comme une séquence structurée d'opérations codées et de valeurs associées.

Cette séquence constitue la représentation canonique de l'objet compilé.

La même séquence est utilisée :

- par l'interpréteur pour l'exécution ;
- par le décompilateur pour la restitution textuelle ;
- par le mécanisme de conservation des programmes.

Aucune copie du texte source n'est requise.

## Statut

Déduit

## Justification

La reconstruction généralise la relation explicitement documentée entre chaîne codée, interpréteur et décompilateur.

Elle constitue la solution minimale permettant de réunir les trois propriétés sans introduire une seconde représentation compilée non documentée.

## Conséquences

### Compilation

Le compilateur produit une séquence structurée.

### Interpréteur

L'interpréteur parcourt cette séquence.

### Décompilateur

Le décompilateur interprète la même séquence dans le sens inverse.

### Bibliothèque

La séquence peut être conservée sans conversion en texte.

## Traçabilité

HIS-4.5.1 → EXI-0010 → VM-0401

HIS-4.5.2 → EXI-0010 → VM-0401

---

## 4.2 Représentation post-fixée

### VM-0402
Séquence post-fixée

## Objet

Définir une représentation abstraite des expressions et opérations compilées dans laquelle les opérandes précèdent l'opération qui les consomme.

## Contraintes historiques

Noyelle décrit explicitement la représentation des programmes du LSD sous forme de notation polonaise inverse, appelée chaîne post-fixée.

Il indique que cette représentation est plus facile à interpréter efficacement.

[NOYELLE89 p.219-220]

## Exigences concernées

### EXI-0011

Titre

Ordre post-fixé des opérations

Description

La représentation compilée reconstruite utilise, pour les expressions, une organisation post-fixée dans laquelle les éléments nécessaires à une opération apparaissent avant l'opération qui les consomme.

Origine

CPF-7.1

CPF-7.2

Sources historiques

[NOYELLE89 p.219-220]

Statut

Probable

Justification

La représentation post-fixée est explicitement documentée pour le LSD, dont le L.S.E. est présenté historiquement comme le successeur. Les sources L.S.E. documentent une chaîne codée exécutable et décompilable mais ne permettent pas d'établir avec certitude son identité avec la chaîne post-fixée du LSD.

Conséquences

La machine abstraite peut définir les expressions compilées sous la forme d'une séquence post-fixée.

Utilisé par

VM-0402

VM-0403

VM-0404

VM-0405

VM-1502

Observations

Le statut « Probable » interdit de présenter l'organisation post-fixée comme un fait documentaire certain pour toutes les implantations L.S.E.

## Analyse

### Éléments documentés

La notation polonaise inverse est explicitement mentionnée dans le récit de Noyelle.

Elle est appelée chaîne post-fixée.

Elle est associée à une interprétation efficace et à la décompilation.

### Éléments non documentés

Les sources ne donnent pas la liste des codes opératoires.

Elles ne précisent pas la représentation des nombres ou des chaînes.

Elles ne décrivent pas la représentation des variables.

Elles ne précisent pas le mécanisme des appels de procédures.

Elles ne permettent pas d'affirmer que chaque version du L.S.E. utilise exactement cette organisation.

### Contraintes

La représentation reconstruite doit :

- pouvoir être parcourue séquentiellement ;
- permettre l'évaluation d'une expression ;
- permettre la restitution de l'expression ;
- ne pas nécessiter une transformation globale avant chaque opération.

## Reconstruction retenue

La chaîne post-fixée abstraite est définie comme une suite ordonnée d'éléments compilés.

Un élément peut être :

- une valeur ;
- une référence à un objet d'exécution ;
- une opération codée ;
- une information de contrôle nécessaire à l'exécution.

Dans le cas des expressions, les opérandes sont placés avant l'opération qui les combine.

La machine abstraite ne fixe pas la représentation matérielle de ces éléments.

Elle fixe seulement leur ordre logique.

## Statut

Probable

## Justification

La représentation post-fixée constitue la reconstruction la plus simple compatible avec la description historique du LSD et avec les propriétés documentées de la chaîne codée : interprétation efficace et décompilation.

Elle est retenue comme modèle de la machine abstraite, mais son attribution historique exacte à toutes les versions L.S.E. reste non démontrée.

## Conséquences

### Pile d'évaluation

L'interpréteur peut utiliser une pile d'évaluation pour les expressions.

### Interpréteur

L'exécution peut se faire par parcours séquentiel de la représentation.

### Décompilateur

Le décompilateur peut reconstruire les expressions à partir de leur ordre post-fixé.

### Compilation

Le compilateur doit produire cette organisation abstraite.

## Traçabilité

CPF-7.1 → EXI-0011 → VM-0402

CPF-7.2 → EXI-0011 → VM-0402

---

## 4.3 Évaluation de la séquence post-fixée

### VM-0403
Évaluation séquentielle

## Objet

Définir le principe d'interprétation de la chaîne post-fixée reconstruite.

## Contraintes historiques

La chaîne post-fixée est décrite comme plus facile à interpréter efficacement.

Le programme compilé est exécuté par l'interpréteur.

[NOYELLE89 p.220]

[INRDP75 §3.3]

## Exigences concernées

### EXI-0012

Titre

Évaluation séquentielle de la représentation compilée

Description

L'interpréteur doit pouvoir exécuter une représentation compilée en parcourant séquentiellement les éléments codés selon leur ordre logique, sans nécessiter une phase de reconstruction du texte source.

Origine

HIS-4.5.1

CPF-7.2

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.220]

Statut

Déduit

Justification

L'interpréteur exécute directement la chaîne codée et la représentation post-fixée est explicitement présentée comme adaptée à une interprétation efficace.

Conséquences

L'interpréteur peut fonctionner directement sur la représentation compilée.

Utilisé par

VM-0403

VM-0404

VM-0603

Observations

L'exigence ne fixe pas l'utilisation matérielle d'une pile.

## Analyse

### Éléments documentés

La représentation compilée est directement exécutée.

La représentation post-fixée est adaptée à une interprétation efficace.

### Éléments non documentés

Les sources ne décrivent pas l'algorithme exact de l'interpréteur.

Elles ne précisent pas si une pile d'évaluation matérielle ou logicielle était employée.

Elles ne donnent pas la structure des contextes d'exécution.

### Contraintes

Le texte source ne doit pas être nécessaire pour exécuter le programme compilé.

La représentation doit pouvoir être parcourue dans son ordre logique.

## Reconstruction retenue

L'interpréteur lit successivement les éléments de la représentation compilée.

Les valeurs et références rencontrées sont rendues disponibles pour les opérations suivantes.

Lorsqu'une opération codée est rencontrée, elle consomme les éléments nécessaires et produit son résultat selon sa sémantique.

Pour les expressions, la reconstruction utilise abstraitement une pile d'évaluation.

Cette pile n'est pas une obligation matérielle : elle constitue un modèle fonctionnel minimal de l'évaluation post-fixée.

## Statut

Déduit

## Justification

La pile d'évaluation découle directement de la structure post-fixée retenue. Elle permet d'expliquer l'évaluation séquentielle sans imposer d'organisation matérielle.

## Conséquences

### Interpréteur

Le mécanisme d'évaluation est directement dérivable de la séquence compilée.

### Expressions

Les résultats intermédiaires sont conservés dans la pile d'évaluation.

### Mémoire

La pile d'évaluation fait partie de l'état temporaire d'une exécution.

### Décompilation

La pile d'évaluation n'est pas nécessaire à la restitution du texte ; elle est spécifique à l'interprétation.

## Traçabilité

HIS-4.5.1 → EXI-0012 → VM-0403

CPF-7.2 → EXI-0012 → VM-0403

---

## 4.4 Opérations codées

### VM-0404
Opération codée

## Objet

Définir l'unité abstraite représentant une opération directement interprétable dans la représentation compilée.

## Contraintes historiques

Le système produit une chaîne codée exécutable par l'interpréteur.

Les sources ne donnent pas la liste des codes opératoires ni leur représentation.

[INRDP75 §3.3]

[NOYELLE89 p.220]

## Exigences concernées

### EXI-0013

Titre

Représentation abstraite des opérations exécutables

Description

La représentation compilée contient des éléments identifiables par l'interpréteur comme des opérations exécutables, indépendamment de leur représentation matérielle.

Origine

HIS-4.5.1

CPF-7.3

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.220]

Statut

Déduit

Justification

Une chaîne directement exécutable doit contenir ou référencer des éléments permettant d'identifier les opérations à effectuer. Les sources ne documentent pas leur forme, mais la fonction qu'ils remplissent est nécessairement présente dans la chaîne exécutable.

Conséquences

Le modèle abstrait distingue les valeurs des opérations exécutables.

Utilisé par

VM-0404

VM-0405

VM-0502

VM-1502

Observations

Aucun code opératoire historique concret n'est attribué à cette exigence.

## Analyse

### Éléments documentés

La chaîne codée est exécutable.

### Éléments non documentés

Les sources ne donnent :

- ni les codes opératoires ;
- ni leur valeur numérique ;
- ni leur taille ;
- ni leur représentation mémoire ;
- ni leur mécanisme de délimitation.

### Contraintes

L'interpréteur doit pouvoir distinguer une opération d'une valeur.

La représentation doit permettre d'identifier l'opération à exécuter.

## Reconstruction retenue

La machine abstraite définit une opération codée comme un élément logique de la représentation compilée.

Chaque opération codée possède une identité abstraite.

Cette identité sélectionne une sémantique d'exécution.

Les opérandes nécessaires à cette opération apparaissent, dans la forme post-fixée, avant l'opération.

La machine abstraite ne définit aucun numéro de code opératoire.

La représentation concrète d'une opération relève d'un tome ultérieur ou d'une implémentation.

## Statut

Déduit

## Justification

L'identité abstraite d'une opération est indispensable à toute représentation exécutable, mais sa représentation matérielle n'est pas documentée.

## Conséquences

### Compilation

Le compilateur doit produire des opérations codées correspondant aux constructions du langage.

### Interpréteur

L'interpréteur doit associer chaque opération codée à sa sémantique.

### Décompilateur

Le décompilateur doit associer les opérations codées aux constructions textuelles correspondantes.

### Portabilité historique

La machine abstraite reste indépendante des codages propres aux matériels.

## Traçabilité

HIS-4.5.1 → EXI-0013 → VM-0404

CPF-7.3 → EXI-0013 → VM-0404

---

## 4.5 Valeurs et références

### VM-0405
Valeurs compilées et références

## Objet

Définir les éléments non opératoires nécessaires à la représentation compilée.

## Contraintes historiques

La chaîne codée doit permettre l'exécution d'expressions et d'instructions portant sur des constantes, variables, tableaux, chaînes, procédures et fonctions.

Les sources ne donnent pas leur représentation interne dans la chaîne codée.

[INRDP75 §3.3]

[NOYELLE89 p.218-220]

## Exigences concernées

### EXI-0014

Titre

Représentation compilée des données utilisées par l'exécution

Description

La représentation compilée doit permettre à l'interpréteur d'obtenir les valeurs constantes et les références nécessaires aux objets d'exécution utilisés par les instructions et expressions du programme.

Origine

HIS-7.4

HIS-10.1

HIS-10.2

HIS-10.3

HIS-10.5

Sources historiques

[INRDP75 §3.3]

[INRDP75 §3-3]

[INRDP75 §4-1]

[NOYELLE89 p.218-220]

Statut

Déduit

Justification

L'exécution de programmes compilés portant sur des variables, des nombres, des chaînes et des tableaux nécessite que la représentation compilée fournisse les informations permettant d'accéder aux données correspondantes.

Conséquences

La représentation compilée distingue les valeurs immédiates des références vers des objets d'exécution.

Utilisé par

VM-0405

VM-0406

VM-0901

VM-1001

VM-1101

VM-1201

Observations

La structure de ces références n'est pas historiquement documentée.

## Analyse

### Éléments documentés

Le langage utilise différents types d'objets.

Le compilateur produit une représentation exécutable.

L'interpréteur doit accéder aux objets utilisés par le programme.

### Éléments non documentés

La chaîne codée ne permet pas de savoir, à partir des sources, si les variables sont représentées par leur nom, par une référence symbolique, par une position relative ou par une autre forme.

La représentation des constantes n'est pas définie.

### Contraintes

Une représentation compilée doit distinguer une valeur immédiate d'une référence à un objet d'exécution.

Elle doit permettre l'utilisation ultérieure de l'objet associé.

## Reconstruction retenue

La machine abstraite distingue deux catégories :

- les valeurs compilées, qui portent directement une valeur nécessaire à l'exécution ;
- les références compilées, qui désignent un objet d'exécution.

Une référence compilée ne désigne pas obligatoirement une adresse mémoire.

Elle représente abstraitement un identificateur résolu au moment de l'exécution ou une référence vers un emplacement logique.

Cette décision évite d'introduire dans la machine abstraite une dépendance à une organisation mémoire particulière.

## Statut

Déduit

## Justification

La distinction entre valeurs et références est nécessaire pour représenter des expressions et instructions exécutables tout en conservant l'indépendance vis-à-vis de l'implantation.

Elle ne prétend pas décrire la représentation historique effective.

## Conséquences

### Interpréteur

L'interpréteur doit résoudre les références lorsqu'elles sont rencontrées.

### Variables

Le mécanisme de résolution sera défini dans le chapitre consacré aux variables.

### Tableaux

Les références peuvent désigner des objets composés.

### Procédures

Les références peuvent également désigner des procédures ou fonctions lorsque le langage le requiert.

### Mémoire

Les références restent indépendantes des adresses physiques.

## Traçabilité

HIS-10.1 → EXI-0014 → VM-0405

HIS-10.2 → EXI-0014 → VM-0405

HIS-10.3 → EXI-0014 → VM-0405

HIS-10.5 → EXI-0014 → VM-0405

---

## 4.6 Décompilation de la chaîne compilée

### VM-0406
Décompilation structurée

## Objet

Définir la restitution d'une représentation textuelle à partir de la représentation compilée.

## Contraintes historiques

Le décompilateur transforme la chaîne codée en une chaîne de caractères.

La commande LISTER utilise ce mécanisme.

La commande DECODER peut produire un fichier de données contenant la représentation textuelle du programme.

[INRDP75 §3.3]

[INRDP75 §4-2]

[INRDP75 Annexe II]

[LSE15 §9-5]

## Exigences concernées

### EXI-0015

Titre

Restitution textuelle déterministe

Description

La décompilation d'un objet compilé doit produire une représentation textuelle valide correspondant à la construction de langage représentée par cet objet.

Origine

HIS-5.3

CPF-7.3

Sources historiques

[INRDP75 §3.3]

[INRDP75 §4-2]

[INRDP75 Annexe II]

Statut

Déduit

Justification

La documentation décrit explicitement la transformation de la chaîne codée en caractères et son utilisation pour le listage. La reconstruction précise que le résultat doit être une représentation textuelle valide du même objet compilé.

Conséquences

La décompilation constitue une transformation déterminée de la représentation compilée vers une forme textuelle.

Utilisé par

VM-0406

VM-1801

VM-1802

Observations

La forme textuelle produite peut être normalisée ; l'identité caractère par caractère avec l'entrée initiale n'est pas exigée.

## Analyse

### Éléments documentés

Le décompilateur produit des caractères.

Le résultat peut être affiché.

Il peut également être stocké sous forme de données.

### Éléments non documentés

Les sources ne définissent pas l'algorithme de décompilation.

Elles ne disent pas si le texte produit reproduit exactement les espaces ou la mise en forme de l'entrée initiale.

### Contraintes

Le texte produit doit permettre de reconnaître la construction représentée.

La décompilation doit être possible sans source textuelle indépendante.

## Reconstruction retenue

Le décompilateur parcourt l'objet compilé.

Il reconstruit une représentation textuelle canonique.

Pour les expressions post-fixées, il reconstruit une expression en respectant les relations opératoires et les priorités nécessaires à une lecture valide.

Pour les instructions, il restitue les mots-clés et les paramètres sous une forme syntaxiquement valide.

Lorsqu'un objet compilé contient une référence, le décompilateur utilise l'identité logique de cet objet pour restituer son identificateur.

La représentation produite est une forme source reconstruite ; elle n'est pas nécessairement une copie exacte des caractères introduits lors de la compilation.

## Statut

Déduit

## Justification

La décompilation est documentée comme une opération inverse du compilateur au niveau fonctionnel.

La reconstruction d'une forme source canonique est la manière minimale d'obtenir cette propriété sans prétendre connaître le format historique exact.

## Conséquences

### Lister

LISTER peut afficher le résultat de la décompilation.

### DECODER

DECODER peut convertir le programme courant en données textuelles.

### Compilation

Un texte produit par décompilation doit pouvoir être recompilé lorsque les conditions du langage le permettent.

### Conservation

Le programme codé reste la représentation principale ; le texte décompilé est une représentation dérivée.

## Traçabilité

HIS-5.3 → EXI-0015 → VM-0406

CPF-7.3 → EXI-0015 → VM-0406

---

## 4.7 Compacité de la représentation

### VM-0407
Représentation compilée compacte

## Objet

Définir la propriété de compacité retenue pour la reconstruction de la représentation compilée.

## Contraintes historiques

Noyelle indique que la chaîne post-fixée occupe environ deux fois moins de place que le texte source dans le contexte historique décrit.

[NOYELLE89 p.220]

## Exigences concernées

### EXI-0016

Titre

Compacité de la représentation compilée

Description

La représentation compilée doit éviter la conservation répétée des éléments syntaxiques textuels qui ne sont pas nécessaires à l'exécution, tout en conservant les informations nécessaires à l'exécution et à la décompilation.

Origine

CPF-7.2

Sources historiques

[NOYELLE89 p.220]

Statut

Probable

Justification

La représentation post-fixée est historiquement décrite comme environ deux fois moins encombrante que le texte source. La reconstruction conserve cette propriété comme objectif architectural sans imposer un taux exact indépendant du programme.

Conséquences

La représentation compilée privilégie les opérations codées et les références structurées plutôt que la conservation directe de la syntaxe textuelle.

Utilisé par

VM-0402

VM-0407

VM-1601

Observations

Aucune garantie quantitative universelle n'est retenue pour toutes les implantations.

## Analyse

### Éléments documentés

La compacité est un objectif explicite de la représentation post-fixée du LSD.

Elle est mise en relation avec les contraintes de mémoire des machines historiques.

### Éléments non documentés

Les sources ne donnent pas une méthode générale de calcul de la taille.

Elles ne permettent pas de déterminer quelles parties du texte source sont supprimées.

Elles ne donnent pas la taille de chaque opération codée.

### Contraintes

La reconstruction doit éviter le stockage redondant du texte source.

Elle doit conserver les informations nécessaires à l'exécution et à la décompilation.

## Reconstruction retenue

La représentation compilée ne conserve pas les mots-clés, séparateurs et parenthèses du texte source lorsqu'ils peuvent être déduits de la structure compilée.

Les opérations sont représentées par des identités codées.

Les valeurs sont conservées sous leur forme d'exécution.

Les références sont conservées comme références logiques.

Le décompilateur reconstitue les éléments textuels nécessaires.

La compacité est donc obtenue par élimination de la redondance syntaxique, et non par une compression générale du contenu.

## Statut

Probable

## Justification

Cette reconstruction reproduit le principe historique de compacité avec le minimum de mécanismes supplémentaires.

Elle ne prétend pas reproduire le taux historique d'environ deux pour un pour chaque programme.

## Conséquences

### Mémoire

Le programme courant occupe principalement des représentations compilées.

### Décompilation

Le texte source est reconstruit à la demande.

### Compilation

Les structures syntaxiques peuvent être transformées en opérations sans conservation des caractères inutiles à l'exécution.

## Traçabilité

CPF-7.2 → EXI-0016 → VM-0407

---

## 4.8 Relation entre chaîne codée et chaîne post-fixée

### VM-0408
Distinction entre chaîne codée et chaîne post-fixée

## Objet

Fixer la portée normative des termes « chaîne codée » et « chaîne post-fixée » dans la reconstruction.

## Contraintes historiques

Le manuel de 1975 emploie le terme « chaîne codée » pour le L.S.E.

Noyelle emploie le terme « chaîne post-fixée » pour la représentation des programmes du LSD.

Le corpus ne permet pas d'établir une identité exacte entre ces deux représentations pour toutes les versions du L.S.E.

[INRDP75 §3.3]

[NOYELLE89 p.219-220]

## Exigences concernées

### EXI-0017

Titre

Distinction documentaire des représentations compilées

Description

La machine abstraite doit conserver la distinction entre la chaîne codée attestée pour le L.S.E. et la chaîne post-fixée explicitement attestée pour le LSD, tout en pouvant utiliser la représentation post-fixée comme reconstruction de la forme compilée du L.S.E.

Origine

CPF-7.1

CPF-7.4

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.219-220]

Statut

Documenté

Justification

Les sources emploient deux termes différents et ne permettent pas d'établir leur identité historique complète.

Conséquences

Les affirmations historiques doivent rester prudentes.

La représentation post-fixée de la machine abstraite ne doit pas être présentée comme un fait documentaire certain pour toutes les versions du L.S.E.

Utilisé par

VM-0408

VM-0409

VM-2201

Observations

La distinction est obligatoire pour la traçabilité historique.

## Analyse

### Éléments documentés

L'existence d'une chaîne codée L.S.E. est documentée.

L'existence d'une chaîne post-fixée LSD est documentée.

La filiation historique LSD → LSE est documentée.

### Éléments non documentés

L'identité exacte entre les deux représentations n'est pas établie.

### Contraintes

La reconstruction ne doit pas transformer une analogie historique en certitude documentaire.

Elle doit néanmoins fournir une représentation abstraite exploitable pour permettre l'implémentation fidèle recherchée.

## Reconstruction retenue

Dans la machine abstraite, le terme « représentation compilée » désigne la notion générique.

La « chaîne codée » désigne cette représentation lorsqu'elle est considérée sous l'angle historique des manuels L.S.E.

La « chaîne post-fixée » désigne le modèle structurel retenu par la reconstruction lorsque la représentation compilée est exprimée selon l'ordre post-fixé.

La relation normative retenue est donc :

chaîne codée historique du L.S.E.
→ représentation compilée abstraite
→ forme post-fixée reconstruite.

Cette relation est une reconstruction et non une identité historique démontrée.

## Statut

Choix de conception

## Justification

La machine abstraite doit disposer d'une représentation compilée précise.

La forme post-fixée est la solution la plus simple compatible avec la représentation explicitement décrite dans l'ascendance historique du L.S.E., avec l'objectif d'interprétation efficace et avec la décompilabilité.

La distinction terminologique protège toutefois la rigueur historique.

## Conséquences

### Documentation

Les descriptions historiques continuent à employer les termes des sources.

### Machine abstraite

La représentation compilée est spécifiée sous une forme post-fixée.

### Implémentation

Une future implémentation devra respecter la sémantique de la représentation abstraite, mais pourra choisir une autre représentation interne si elle conserve les propriétés nécessaires.

### Traçabilité

Les choix de reconstruction restent distingués des faits historiques.

## Traçabilité

CPF-7.1 → EXI-0017 → VM-0408

CPF-7.4 → EXI-0017 → VM-0408

---

## 4.9 Limites normatives de la chaîne post-fixée

### VM-0409
Abstraction du format de la chaîne

## Objet

Définir les limites de la spécification de la chaîne post-fixée dans le Tome II.

## Contraintes historiques

Les sources ne donnent pas :

- le format complet de la chaîne codée ;
- le format complet de la chaîne post-fixée ;
- la liste des opérations codées ;
- leur représentation binaire ;
- les mécanismes exacts de délimitation ;
- les mécanismes exacts de référencement interne.

[INRDP75 §3.3]

[NOYELLE89 p.220]

## Exigences concernées

EXI-0011

EXI-0013

EXI-0014

EXI-0017

## Analyse

### Éléments documentés

La représentation compilée doit être exécutable.

Elle doit être décompilable.

La forme post-fixée est un modèle historique plausible et fonctionnellement adapté.

### Éléments non documentés

Le format concret n'est pas connu.

La taille des unités n'est pas connue.

Les codes numériques ne sont pas connus.

La représentation exacte des références n'est pas connue.

### Contraintes

Le Tome II doit spécifier la machine abstraite sans reconstruire arbitrairement un format matériel historique.

## Reconstruction retenue

La chaîne post-fixée est définie au niveau sémantique.

La spécification impose :

- l'ordre logique des éléments ;
- la distinction entre valeurs, références et opérations ;
- la possibilité d'une évaluation séquentielle ;
- la possibilité d'une décompilation ;
- la stabilité de la représentation durant l'exécution.

La spécification n'impose pas :

- une taille d'unité ;
- une représentation binaire ;
- une adresse mémoire ;
- un alignement ;
- un encodage numérique particulier ;
- une longueur maximale déterminée par une machine historique.

Ces propriétés relèvent de l'implémentation concrète et ne font pas partie de la machine abstraite.

## Statut

Choix de conception

## Justification

Le format exact n'est pas documenté.

Le reproduire arbitrairement irait au-delà des sources.

Le niveau sémantique retenu est suffisamment précis pour permettre une implémentation, tout en laissant ouvertes les propriétés que les sources ne permettent pas d'établir.

## Conséquences

### Compilation

Le compilateur est spécifié par sa production sémantique.

### Interpréteur

L'interpréteur est spécifié par le comportement obtenu à partir de la séquence.

### Décompilateur

Le décompilateur est spécifié par la restitution textuelle.

### Implémentation

Le format concret est libre tant qu'il respecte la machine abstraite.

## Traçabilité

CPF-7.4 → EXI-0011, EXI-0013, EXI-0014, EXI-0017 → VM-0409

---

# 5. Compilation

La compilation constitue le mécanisme qui transforme une ligne de programme en objet compilé. Dans l'environnement historique, elle est déclenchée automatiquement à la réception de la fin de ligne et constitue l'unité de traitement qui permet la modification locale des programmes. [INRDP75 §3.3] [LSE15 §1]

Le Tome II ne reconstruit pas l'algorithme lexical ou syntaxique historique. Il définit le comportement abstrait minimal que doit produire le compilateur pour que l'ensemble de la machine satisfasse les propriétés documentées.

## 5.1 Déclenchement de la compilation

### VM-0501
Compilation à la validation de la ligne

## Objet

Définir le point de déclenchement de la compilation dans l'environnement conversationnel.

## Contraintes historiques

La ligne est analysée lorsque le caractère de fin de ligne est envoyé.

La compilation est automatique.

L'utilisateur n'a pas à demander explicitement une commande de compilation.

[INRDP75 §3.3]

[LSE15 §1]

## Exigences concernées

EXI-0001

## Analyse

### Éléments documentés

La compilation intervient à la validation de la ligne.

Elle est automatique.

Elle s'applique à l'entrée interactive du programme.

Elle est également utilisée par ENCODER lorsque des lignes sont fournies au compilateur depuis un fichier données.

### Éléments non documentés

Les sources ne précisent pas le protocole interne de transfert entre la console et le compilateur.

Elles ne décrivent pas la nature du tampon d'entrée.

### Contraintes

La machine abstraite doit considérer la fin de ligne comme l'événement de validation de l'entrée.

Avant cet événement, la ligne n'est pas encore soumise au compilateur.

Après cet événement, le résultat de compilation doit être déterminé.

## Reconstruction retenue

La machine abstraite définit une entrée de ligne composée d'un flux de caractères.

La réception du caractère de fin de ligne provoque la validation de l'entrée.

La ligne validée est soumise au compilateur.

Le compilateur produit un résultat :

- objet compilé ;
- ou diagnostic d'erreur.

La machine abstraite ne définit pas la manière dont les caractères sont accumulés avant validation.

## Statut

Déduit

## Justification

La compilation automatique et le rôle de la fin de ligne sont documentés.

La formalisation de la validation comme événement abstrait est nécessaire pour définir précisément l'interface entre l'environnement conversationnel et le compilateur.

## Conséquences

### Console

La fin de ligne constitue une frontière entre saisie et compilation.

### Compilation

Le compilateur ne traite qu'une ligne validée.

### Programme courant

Seule une compilation réussie peut modifier son état.

### ENCODER

Les lignes issues d'un fichier données doivent pouvoir être présentées au compilateur comme des lignes validées.

## Traçabilité

HIS-5.2 → EXI-0001 → VM-0501

HIS-6.5 → EXI-0001 → VM-0501

---

## 5.2 Analyse d'une ligne

### VM-0502
Analyse syntaxique de la ligne

## Objet

Définir la fonction abstraite de l'analyse syntaxique préalable à la production de la représentation compilée.

## Contraintes historiques

Le compilateur analyse la ligne L.S.E.

Il détecte les erreurs de syntaxe.

Une erreur est signalée immédiatement lorsque la ligne est traitée.

[INRDP75 §3.3]

[LSE15 §1]

## Exigences concernées

### EXI-0018

Titre

Analyse syntaxique préalable à la compilation

Description

Toute ligne de programme doit être analysée syntaxiquement avant son intégration au programme courant sous forme compilée.

Origine

HIS-5.2

HIS-6.5

Sources historiques

[INRDP75 §3.3]

[LSE15 §1]

Statut

Documenté

Justification

Le rôle du compilateur est explicitement décrit comme comprenant l'analyse de la ligne et la détection des erreurs de syntaxe.

Conséquences

Une ligne syntaxiquement incorrecte ne produit pas d'objet compilé accepté.

Utilisé par

VM-0502

VM-0503

VM-0302

Observations

L'algorithme d'analyse syntaxique n'est pas documenté.

## Analyse

### Éléments documentés

La syntaxe est vérifiée à la compilation.

La vérification porte sur la ligne entrée.

Les lignes sont indépendantes syntaxiquement.

Certaines erreurs peuvent néanmoins n'être détectées qu'à l'exécution.

### Éléments non documentés

Les sources ne décrivent pas la grammaire interne utilisée par le compilateur dans le cadre architectural du Tome I.

Elles ne décrivent pas l'algorithme de reconnaissance.

### Contraintes

L'analyse syntaxique doit précéder l'acceptation de la ligne.

Elle ne doit pas être assimilée à la vérification de toutes les propriétés sémantiques.

## Reconstruction retenue

Le compilateur réalise une analyse syntaxique abstraite.

Cette analyse :

- reconnaît la construction de la ligne ;
- identifie ses composants ;
- détecte les erreurs syntaxiques ;
- produit une structure interne temporaire destinée à la génération de l'objet compilé.

Le résultat intermédiaire de l'analyse n'est pas conservé dans le programme courant.

## Statut

Déduit

## Justification

La fonction générale de l'analyse est documentée.

La notion d'un résultat intermédiaire abstrait est introduite uniquement parce qu'une reconstruction directe de la chaîne compilée sans étape logique intermédiaire empêcherait de distinguer l'analyse syntaxique de la génération de la représentation.

Elle ne préjuge pas de l'organisation du compilateur historique.

## Conséquences

### Compilation

La compilation comporte une phase conceptuelle d'analyse.

### Erreurs

Les erreurs syntaxiques sont détectées avant la production d'un objet accepté.

### Chaîne post-fixée

La représentation est générée à partir de la structure reconnue.

### Implémentation

Une réalisation peut fusionner plusieurs étapes internes sans modifier le comportement abstrait.

## Traçabilité

HIS-5.2 → EXI-0018 → VM-0502

HIS-6.5 → EXI-0018 → VM-0502

---

## 5.3 Séparation entre syntaxe et sémantique d'exécution

### VM-0503
Validation syntaxique et erreurs différées

## Objet

Définir la limite entre les erreurs détectables à la compilation et celles qui ne peuvent être détectées qu'à l'exécution.

## Contraintes historiques

Les sources indiquent qu'une ligne peut être syntaxiquement correcte tout en contenant une erreur qui n'est détectée qu'au moment de l'exécution.

Le manuel de 1975 donne notamment l'exemple d'une variable chaîne dont la déclaration a été omise.

[INRDP75 §« Présentation d'un programme L.S.E. »]

[LSE15 §1]

## Exigences concernées

### EXI-0019

Titre

Distinction des erreurs de compilation et d'exécution

Description

La validation syntaxique d'une ligne ne garantit pas l'absence de toutes les erreurs susceptibles d'apparaître lors de son exécution.

Origine

HIS-5.2

HIS-8.2

HIS-12.1

Sources historiques

[INRDP75 §« Présentation d'un programme L.S.E. »]

[LSE15 §1]

Statut

Documenté

Justification

Les sources distinguent explicitement les erreurs syntaxiques détectées lors de la compilation des erreurs qui ne peuvent être détectées qu'à l'exécution.

Conséquences

La machine abstraite ne doit pas exiger que le compilateur résolve toutes les références ou vérifie toutes les conditions d'exécution.

Utilisé par

VM-0503

VM-0602

VM-1901

Observations

Le partage exact des vérifications entre compilation et exécution n'est pas entièrement documenté.

## Analyse

### Éléments documentés

Une ligne peut être acceptée syntaxiquement et néanmoins échouer à l'exécution.

Certaines erreurs concernent l'utilisation des objets du programme.

### Éléments non documentés

Les sources ne donnent pas la liste exhaustive des contrôles effectués à la compilation.

Elles ne donnent pas davantage la liste exhaustive des contrôles différés.

### Contraintes

La reconstruction doit préserver la possibilité d'erreurs d'exécution malgré la réussite de la compilation.

## Reconstruction retenue

Le compilateur vérifie les propriétés syntaxiques nécessaires à l'interprétation de la ligne.

Les propriétés dépendant de l'état d'exécution peuvent être laissées à l'interpréteur.

Une référence dont la validité dépend de l'état du programme ou de l'existence d'un objet au moment de l'exécution peut donc être représentée dans l'objet compilé sans que sa résolution définitive soit requise lors de la compilation.

Le compilateur ne doit pas être considéré comme un vérificateur universel du programme.

## Statut

Documenté

## Justification

La distinction entre erreurs syntaxiques et erreurs d'exécution est explicitement documentée.

La reconstruction précise seulement la séparation fonctionnelle nécessaire pour respecter cette distinction.

## Conséquences

### Compilation

Le compilateur n'a pas à résoudre toutes les dépendances dynamiques.

### Interpréteur

L'interpréteur doit pouvoir produire des diagnostics d'exécution.

### Variables

La validité de certaines références peut être déterminée lors de l'exécution.

### Tableaux

L'existence d'un tableau ou son état d'allocation peut être vérifié à l'exécution lorsque la documentation l'exige.

## Traçabilité

HIS-5.2 → EXI-0019 → VM-0503

HIS-8.2 → EXI-0019 → VM-0503

HIS-12.1 → EXI-0019 → VM-0503

---

## 5.4 Génération de la représentation compilée

### VM-0504
Génération de l'objet compilé

## Objet

Définir la transformation abstraite d'une ligne syntaxiquement acceptée en objet compilé.

## Contraintes historiques

Le compilateur codifie la ligne.

La compilation ligne à ligne produit une chaîne codée.

[INRDP75 §3.3]

Noyelle associe la compilation du LSD à la génération d'une chaîne post-fixée.

[NOYELLE89 p.222]

## Exigences concernées

### EXI-0020

Titre

Production d'une représentation compilée par ligne

Description

Toute ligne syntaxiquement acceptée produit un objet compilé contenant une représentation exécutable de cette ligne.

Origine

HIS-5.2

HIS-6.5

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.217]

Statut

Documenté

Justification

Le résultat de la compilation ligne à ligne est explicitement décrit comme une chaîne codée exécutable par l'interpréteur.

Conséquences

La compilation transforme une entrée textuelle en représentation compilée.

Utilisé par

VM-0301

VM-0504

VM-0401

Observations

La structure exacte de la représentation est reconstruite dans le chapitre 4.

## Analyse

### Éléments documentés

La compilation codifie la ligne.

Le résultat est exécutable.

### Éléments non documentés

Le processus de génération est inconnu.

Le format des opérations est inconnu.

Le lien entre l'analyse syntaxique et les éléments de la chaîne n'est pas décrit.

### Contraintes

Chaque construction reconnue doit pouvoir être représentée dans l'objet compilé.

L'objet doit être exécutable.

Il doit également être décompilable.

## Reconstruction retenue

Le compilateur transforme la structure syntaxique abstraite de la ligne en une séquence de représentation compilée.

Les expressions sont converties en forme post-fixée.

Les opérations sont représentées par des opérations codées.

Les constantes sont représentées comme valeurs compilées.

Les identificateurs et les autres objets du programme sont représentés par des références logiques.

Le résultat est encapsulé dans l'objet compilé associé au numéro de ligne.

## Statut

Déduit

## Justification

Cette reconstruction satisfait le résultat documenté sans prétendre reconstruire l'algorithme historique exact.

## Conséquences

### CPF

La compilation constitue la source de la chaîne post-fixée.

### Interpréteur

L'interpréteur reçoit directement le résultat.

### Décompilateur

Le résultat contient suffisamment d'informations pour permettre la reconstruction du texte.

### Programmes

Chaque ligne produit un objet distinct.

## Traçabilité

HIS-5.2 → EXI-0020 → VM-0504

HIS-6.5 → EXI-0020 → VM-0504

---

## 5.5 Intégration dans le programme courant

### VM-0505
Acceptation d'un objet compilé

## Objet

Définir l'intégration d'un objet compilé dans le programme courant après réussite de la compilation.

## Contraintes historiques

Une ligne correcte est mémorisée.

Elle est replacée selon l'ordre croissant des numéros.

Une ligne incorrecte n'est pas prise en compte.

[INRDP75 §« Présentation d'un programme L.S.E. »]

[LSE15 §1]

## Exigences concernées

EXI-0004

EXI-0006

EXI-0008

EXI-0020

## Analyse

### Éléments documentés

La ligne est intégrée après compilation réussie.

Son numéro détermine sa position logique.

### Éléments non documentés

La méthode d'insertion est inconnue.

### Contraintes

Une ligne validée doit immédiatement devenir disponible comme partie du programme courant.

## Reconstruction retenue

Après production d'un objet compilé valide :

1. si aucun objet ne porte le numéro de ligne, le nouvel objet est ajouté ;
2. si un objet porte déjà ce numéro, il est remplacé ;
3. l'ordre logique du programme est recalculé ou maintenu par numéro ;
4. l'état résultant devient le nouveau programme courant.

L'intégration ne déclenche pas la recompilation des autres objets.

## Statut

Déduit

## Justification

La reconstruction rassemble en une opération abstraite les propriétés documentées de mémorisation, remplacement et classement.

## Conséquences

### Programme courant

Il évolue localement.

### Compilation

La compilation d'une ligne est indépendante du reste du programme.

### Interpréteur

Une nouvelle exécution utilise immédiatement la version acceptée.

### Décompilateur

Le listage reflète l'état le plus récent.

## Traçabilité

HIS-6.2 → EXI-0001, EXI-0006, EXI-0008 → VM-0505

HIS-6.5 → EXI-0020 → VM-0505

---

## 5.6 Compilation depuis une source de données

### VM-0506
Compilation de lignes fournies par un fichier données

## Objet

Définir l'utilisation du même mécanisme de compilation pour des lignes provenant d'une source de données.

## Contraintes historiques

La commande ENCODER fait intervenir le bibliothécaire et le compilateur.

Les lignes à compiler sont fournies par un fichier données au lieu d'être frappées directement au clavier ou lues sur un ruban.

[INRDP75 Annexe II]

[LSE15 §9-5]

## Exigences concernées

### EXI-0021

Titre

Compilation uniforme des lignes entrées indirectement

Description

Les lignes de programme fournies par une source de données destinée à l'encodage sont soumises au même mécanisme logique de compilation que les lignes entrées interactivement.

Origine

HIS-6.5

HIS-11.5

Sources historiques

[INRDP75 Annexe II]

[LSE15 §9-5]

Statut

Déduit

Justification

Le manuel décrit ENCODER comme faisant intervenir le compilateur et comme fournissant à celui-ci des lignes provenant d'un fichier données.

Conséquences

Le mécanisme de compilation ne dépend pas de la provenance physique des caractères.

Utilisé par

VM-0506

VM-1101

VM-1702

Observations

Le protocole précis d'ENCODER n'est pas reconstruit dans ce mécanisme.

## Analyse

### Éléments documentés

Le compilateur peut traiter des lignes provenant d'un fichier données.

### Éléments non documentés

Le traitement exact des erreurs dans ce contexte n'est pas décrit en détail.

Il n'est pas établi si les lignes sont intégrées une par une ou si une opération globale intervient avant leur intégration.

### Contraintes

Le résultat doit respecter les mêmes propriétés fondamentales que la compilation interactive.

## Reconstruction retenue

Le compilateur reçoit une ligne abstraite quelle que soit son origine.

L'entrée peut provenir :

- de la console ;
- d'une source de données destinée à l'encodage.

Dans les deux cas, une ligne validée produit soit un objet compilé, soit un diagnostic.

Le mécanisme d'entrée ne fait donc pas partie du compilateur lui-même.

## Statut

Déduit

## Justification

Cette séparation est la reconstruction minimale du fait que le même compilateur est utilisé par ENCODER.

## Conséquences

### Compilation

Le compilateur est indépendant de la source d'entrée.

### Bibliothécaire

Le bibliothécaire fournit les lignes provenant du fichier.

### Programme courant

Les objets obtenus sont soumis aux mêmes règles d'intégration.

## Traçabilité

HIS-6.5 → EXI-0021 → VM-0506

HIS-11.5 → EXI-0021 → VM-0506

---

## 5.7 Compilation et conservation du texte source

### VM-0507
Absence de dépendance au texte source pour l'exécution

## Objet

Définir la séparation entre la conservation éventuelle du texte source et l'exécution du programme compilé.

## Contraintes historiques

La chaîne codée est directement exécutable.

Le programme rangé par RANGER est conservé sous forme codée et non sous forme de chaîne de caractères L.S.E.

La représentation post-fixée est décrite comme permettant la décompilation, évitant ainsi de conserver séparément le texte source pour le listage.

[INRDP75 §3.3]

[INRDP75 §4-4]

[NOYELLE89 p.220]

## Exigences concernées

### EXI-0022

Titre

Indépendance d'exécution vis-à-vis du texte source

Description

L'exécution d'un programme compilé ne dépend pas de la présence d'une copie séparée de son texte source.

Origine

HIS-4.5.1

HIS-11.2

CPF-7.2

Sources historiques

[INRDP75 §3.3]

[INRDP75 §4-4]

[NOYELLE89 p.220]

Statut

Déduit

Justification

Le programme compilé est directement exécutable et peut être conservé sous forme codée.

Conséquences

Le texte source n'est pas une donnée obligatoire de l'environnement d'exécution.

Utilisé par

VM-0401

VM-0507

VM-1701

VM-1801

Observations

Le système peut néanmoins produire ou conserver une représentation textuelle par DECODER.

## Analyse

### Éléments documentés

Le programme codé peut être rangé.

Il est rechargeable et exécutable.

Le texte source peut être recréé par décompilation.

### Éléments non documentés

Les sources ne permettent pas d'établir si une implantation conservait éventuellement une copie auxiliaire du texte.

### Contraintes

La machine abstraite ne doit pas en dépendre pour l'exécution.

## Reconstruction retenue

Le texte source est considéré comme une représentation d'entrée et de sortie du système.

L'objet compilé constitue la représentation nécessaire à l'exécution.

Le texte peut être reproduit par décompilation, mais sa conservation séparée n'est pas une condition de fonctionnement.

## Statut

Déduit

## Justification

Cette organisation reprend la fonction historique de la chaîne codée et de la décompilation tout en évitant d'introduire une conservation du texte qui contredirait l'objectif historique de compacité.

## Conséquences

### Mémoire

Le programme courant n'a pas besoin de conserver une seconde copie textuelle.

### Bibliothèque

Un fichier-programme peut contenir uniquement la représentation compilée abstraite.

### Décompilation

La restitution textuelle est une opération dérivée.

## Traçabilité

HIS-4.5.1 → EXI-0022 → VM-0507

HIS-11.2 → EXI-0022 → VM-0507

CPF-7.2 → EXI-0022 → VM-0507

---

# 6. Organisation du programme courant

Le programme courant est le support abstrait reliant les mécanismes de compilation, d'exécution, de décompilation et de modification du programme. Sa propriété essentielle est de conserver une collection cohérente d'objets compilés indépendants, identifiés par leurs numéros de ligne.

Le présent chapitre formalise cette organisation au niveau de la machine abstraite, sans introduire de structure mémoire historique non documentée.

## 6.1 Identité d'un objet compilé

### VM-0601
Identité logique d'une ligne

## Objet

Définir l'identité logique permettant d'identifier un objet compilé dans le programme courant.

## Contraintes historiques

Les lignes sont identifiées par des numéros.

Deux lignes de même numéro ne coexistent pas.

Une nouvelle ligne portant un numéro existant remplace la précédente.

[LSE15 §1]

[INRDP75 §« Présentation d'un programme L.S.E. »]

## Exigences concernées

EXI-0006

## Analyse

### Éléments documentés

Le numéro de ligne identifie la ligne dans le programme.

### Éléments non documentés

Les sources ne décrivent pas si ce numéro est utilisé comme clé interne.

### Contraintes

Une ligne doit pouvoir être identifiée sans ambiguïté.

## Reconstruction retenue

La machine abstraite utilise le numéro de ligne comme identité logique de l'objet compilé dans le programme courant.

Cette identité n'est pas assimilée à une adresse mémoire.

Elle permet :

- la recherche ;
- le remplacement ;
- la suppression ;
- le classement.

## Statut

Déduit

## Justification

L'utilisation du numéro comme identité abstraite est la représentation la plus simple des règles historiques observées.

Elle n'impose aucun choix d'implantation.

## Conséquences

### Programme courant

Chaque ligne possède une identité unique.

### Compilation

La validation d'une nouvelle ligne détermine l'identité de l'objet à produire.

### Gestion mémoire

Les identités logiques ne sont pas des adresses physiques.

## Traçabilité

HIS-6.2 → EXI-0006 → VM-0601

---

## 6.2 Cohérence du programme courant

### VM-0602
Cohérence du programme courant

## Objet

Définir les invariants applicables au programme courant.

## Contraintes historiques

Le programme courant contient les lignes acceptées.

Les lignes incorrectes ne sont pas prises en compte.

Une ligne est unique par numéro.

Le programme peut être exécuté ou listé.

## Exigences concernées

EXI-0004

EXI-0006

EXI-0008

EXI-0020

## Analyse

### Éléments documentés

Le programme courant est modifiable.

Il peut être exécuté.

### Éléments non documentés

Les invariants internes ne sont pas décrits.

### Contraintes

La reconstruction doit garantir que l'état du programme reste cohérent après chaque opération.

## Reconstruction retenue

À tout instant, un programme courant satisfait les invariants suivants :

1. chaque numéro de ligne appartient à au plus un objet compilé ;
2. chaque objet du programme courant correspond à une compilation réussie ;
3. chaque objet possède une représentation exécutable ;
4. chaque objet est décompilable ;
5. aucun objet supprimé n'appartient encore au programme courant ;
6. le parcours logique est défini par les numéros de ligne.

Toute modification du programme courant doit préserver ces invariants.

## Statut

Déduit

## Justification

Ces invariants regroupent les propriétés documentées dans une forme normative nécessaire à la spécification de la machine abstraite.

## Conséquences

### Compilation

L'intégration doit préserver la cohérence.

### Interpréteur

L'exécution peut supposer que les objets parcourus sont valides du point de vue de la compilation.

### Décompilateur

Le décompilateur peut parcourir uniquement les objets appartenant au programme courant.

### Gestion mémoire

La suppression d'un objet ne doit pas laisser une référence logique utilisable par le programme courant.

## Traçabilité

HIS-5.2 → EXI-0004, EXI-0020 → VM-0602

HIS-6.2 → EXI-0006, EXI-0008 → VM-0602

---

## 6.3 Parcours initial du programme

### VM-0603
Point initial d'exécution

## Objet

Définir le point à partir duquel l'interpréteur commence l'exécution d'un programme courant.

## Contraintes historiques

Le programme est exécuté dans l'ordre croissant des numéros de lignes.

[INRDP75 §3.3]

[LSE15 §1]

## Exigences concernées

EXI-0008

### EXI-0023

Titre

Début de l'exécution au premier objet logique

Description

Lorsqu'une exécution programmée commence sans point de reprise ni transfert explicite, elle débute au premier objet compilé selon l'ordre logique des numéros de ligne.

Origine

HIS-8.2

Sources historiques

[INRDP75 §3.3]

[LSE15 §1]

Statut

Déduit

Justification

Le programme est exécuté dans l'ordre croissant des numéros de lignes ; le début normal de l'exécution doit donc correspondre au premier numéro existant.

Conséquences

L'interpréteur doit pouvoir déterminer le premier objet exécutable.

Utilisé par

VM-0603

VM-2101

Observations

Les mécanismes de reprise et de branchement peuvent modifier le point d'exécution ultérieur.

## Analyse

### Éléments documentés

L'ordre d'exécution est croissant.

### Éléments non documentés

Le mode interne de positionnement de l'interpréteur n'est pas décrit.

### Contraintes

L'exécution normale doit être déterministe quant à son point de départ.

## Reconstruction retenue

L'interpréteur initialise son parcours sur le premier numéro de ligne présent dans le programme courant.

Ce point constitue le point initial d'exécution.

Une instruction de contrôle peut ensuite modifier le flux.

## Statut

Déduit

## Justification

La règle est la conséquence directe de l'ordre d'exécution documenté.

## Conséquences

### Interpréteur

L'état d'exécution comprend une position logique dans le programme.

### Contrôle

Les branchements remplacent cette position.

### Débogage

Une interruption peut conserver cette position pour permettre la reprise.

## Traçabilité

HIS-8.2 → EXI-0008, EXI-0023 → VM-0603

---

## 6.4 Position d'exécution

### VM-0604
Position logique d'exécution

## Objet

Définir l'abstraction permettant de représenter l'endroit où l'interpréteur se trouve dans le programme courant.

## Contraintes historiques

Le programme peut être interrompu.

L'utilisateur peut savoir où il a été arrêté et poursuivre l'exécution.

[NOYELLE89 p.221]

Le manuel décrit également les mécanismes d'arrêt et de reprise.

[INRDP75 §4.3]

[LSE15 §9-3]

## Exigences concernées

### EXI-0024

Titre

Conservation d'une position d'exécution interrompue

Description

Une interruption d'un programme en cours d'exécution doit permettre la conservation d'une position logique suffisante pour reprendre l'exécution ultérieurement lorsque le mécanisme de reprise le prévoit.

Origine

HIS-3.13

HIS-8.3

Sources historiques

[NOYELLE89 p.221]

[INRDP75 §4.3]

[LSE15 §9-3]

Statut

Déduit

Justification

Les sources décrivent la possibilité d'interrompre un programme, d'examiner son état puis de poursuivre son exécution.

Conséquences

L'état d'exécution doit contenir au minimum une position logique de reprise.

Utilisé par

VM-0604

VM-0801

VM-2102

Observations

La forme exacte de cette position n'est pas documentée.

## Analyse

### Éléments documentés

L'exécution peut être interrompue.

Elle peut être reprise.

Le contexte d'exécution doit être conservé entre ces deux opérations.

### Éléments non documentés

Les sources ne décrivent pas une structure de compteur ordinal.

Elles ne précisent pas comment la reprise fonctionne au niveau du code compilé.

### Contraintes

La position de reprise doit être suffisamment précise pour éviter de recommencer arbitrairement l'exécution.

Elle doit rester cohérente avec l'état des variables et objets d'exécution.

## Reconstruction retenue

La machine abstraite associe à l'exécution une position logique.

Cette position désigne l'opération ou l'objet compilé suivant à exécuter.

Lorsqu'une interruption survient, la position est conservée avec le reste de l'état d'exécution.

Lors d'une reprise, l'interpréteur restitue cet état et poursuit à partir de la position conservée.

La représentation de cette position n'est pas fixée.

## Statut

Déduit

## Justification

La possibilité historique de reprise impose la conservation d'un état d'exécution cohérent.

La notion abstraite de position d'exécution est le modèle minimal nécessaire.

## Conséquences

### Interpréteur

L'état d'exécution inclut une position.

### Pile d'évaluation

La position peut être associée à une pile d'évaluation non vide lors d'une interruption.

### Cadres d'activation

Une procédure active peut nécessiter une position propre à son cadre.

### Mémoire

L'état d'exécution doit être conservable lors d'un changement de tâche.

## Traçabilité

HIS-3.13 → EXI-0024 → VM-0604

HIS-8.3 → EXI-0024 → VM-0604

---

## 6.5 Programme courant et représentation source

### VM-0605
Indépendance du programme courant vis-à-vis du texte source

## Objet

Définir la relation entre la collection d'objets compilés et la représentation textuelle du programme.

## Contraintes historiques

Le programme courant peut être listé par décompilation.

Il peut être stocké sous forme codée.

La commande DECODER peut produire une représentation textuelle sous forme de fichier données.

[INRDP75 §3.3]

[INRDP75 §4-4]

[INRDP75 Annexe II]

[LSE15 §9-5]

## Exigences concernées

EXI-0003

EXI-0009

EXI-0022

## Analyse

### Éléments documentés

La forme codée et la forme textuelle sont distinguées.

### Éléments non documentés

Les sources ne précisent pas si le texte de la ligne est conservé parallèlement à sa forme codée pendant la session.

### Contraintes

Le programme courant doit pouvoir fonctionner sans dépendre du texte source.

La représentation textuelle doit pouvoir être produite à la demande.

## Reconstruction retenue

Le programme courant est constitué exclusivement, au niveau normatif, d'objets compilés.

Le texte source est considéré comme une représentation externe pouvant être :

- fournie au compilateur ;
- produite par décompilation ;
- rangée comme fichier données.

Il n'est pas une composante obligatoire de l'état du programme courant.

## Statut

Déduit

## Justification

Cette organisation est la plus simple compatible avec le fonctionnement documenté de la chaîne codée et du décompilateur.

Elle évite de faire dépendre l'exécution d'une représentation que les sources présentent comme reconstructible.

## Conséquences

### Compilation

Le texte entre dans la machine et en ressort sous une forme différente.

### Décompilation

La forme textuelle est reconstruite.

### Bibliothèque

Le fichier-programme conserve la forme compilée.

### Fichiers données

DECODER peut produire séparément une forme textuelle.

## Traçabilité

HIS-4.5.2 → EXI-0003 → VM-0605

HIS-11.2 → EXI-0009, EXI-0022 → VM-0605

---

## 6.6 Remplacement complet du programme courant

### VM-0606
Chargement d'un nouveau programme courant

## Objet

Définir l'effet abstrait du chargement d'un fichier-programme.

## Contraintes historiques

APPELER charge en mémoire un programme contenu dans un fichier-programme.

Le programme chargé devient le programme courant.

[INRDP75 §4-4]

[LSE15 §9-5]

## Exigences concernées

### EXI-0025

Titre

Chargement comme remplacement du programme courant

Description

Le chargement d'un fichier-programme établit comme programme courant le programme compilé contenu dans ce fichier.

Origine

HIS-11.5

Sources historiques

[INRDP75 §4-4]

[LSE15 §9-5]

Statut

Documenté

Justification

Les sources décrivent explicitement le programme chargé comme devenant le programme courant.

Conséquences

Le programme courant antérieur cesse d'être le programme courant actif.

Utilisé par

VM-0606

VM-1701

Observations

Les sources ne détaillent pas les conditions de remplacement lorsqu'une exécution est en cours.

## Analyse

### Éléments documentés

APPELER charge un programme.

Le programme devient le programme courant.

### Éléments non documentés

Le traitement précis du programme précédent n'est pas décrit.

### Contraintes

Le chargement doit produire un état cohérent.

Le nouveau programme doit être exécutable et décompilable.

## Reconstruction retenue

Le chargement d'un fichier-programme produit un nouveau programme courant composé des objets compilés restaurés depuis le fichier.

Le précédent programme courant est remplacé comme état courant.

Les objets du programme précédent ne sont plus accessibles par les mécanismes ordinaires du nouveau programme courant.

La récupération éventuelle de leurs ressources relève de la gestion mémoire.

## Statut

Déduit

## Justification

La substitution abstraite du programme courant est la conséquence nécessaire du fait que le programme chargé devient le nouveau programme courant.

## Conséquences

### Bibliothèque

Le bibliothécaire fournit une représentation compilée.

### Interpréteur

Le nouveau programme peut être exécuté sans recompilation.

### Décompilateur

Le nouveau programme peut être listé.

### Mémoire

Les ressources du programme précédent doivent être libérées ou rendues disponibles selon les mécanismes mémoire.

## Traçabilité

HIS-11.5 → EXI-0025 → VM-0606

---

## 6.7 Cohérence entre compilation et exécution

### VM-0607
Visibilité immédiate des modifications compilées

## Objet

Définir le moment auquel une modification du programme devient effective pour les opérations ultérieures.

## Contraintes historiques

La compilation est effectuée dès la validation de la ligne.

Une ligne correcte est mémorisée.

Une ligne portant le même numéro remplace la précédente.

Le programme peut ensuite être exécuté.

[INRDP75 §3.3]

[LSE15 §1]

## Exigences concernées

### EXI-0026

Titre

Prise en compte immédiate des lignes acceptées

Description

Une ligne de programme acceptée par la compilation devient la représentation du numéro de ligne correspondant dans le programme courant avant toute exécution ultérieure demandée sur ce programme.

Origine

HIS-6.5

HIS-8.2

Sources historiques

[INRDP75 §3.3]

[LSE15 §1]

Statut

Déduit

Justification

La compilation est réalisée lors de l'entrée et la ligne correcte est immédiatement mémorisée ; l'exécution ultérieure porte sur le programme courant ainsi modifié.

Conséquences

La machine ne doit pas conserver une version compilée obsolète comme version courante après l'acceptation d'une modification.

Utilisé par

VM-0505

VM-0607

VM-2101

Observations

L'exigence ne traite pas des exécutions déjà en cours au moment d'une modification.

## Analyse

### Éléments documentés

La modification d'une ligne est effectuée par réintroduction et compilation.

La nouvelle ligne prend la place de l'ancienne.

### Éléments non documentés

Le comportement précis d'une modification pendant une exécution interrompue ou suspendue n'est pas uniforme dans les sources.

### Contraintes

Une nouvelle exécution doit voir l'état courant du programme.

## Reconstruction retenue

L'état courant du programme est mis à jour dès l'acceptation de la ligne.

Une exécution ultérieure commence à partir de cette nouvelle version.

Une exécution déjà suspendue conserve son propre état d'exécution jusqu'à sa reprise ou sa terminaison, conformément aux mécanismes de reprise et d'interruption.

Cette dernière distinction est nécessaire pour ne pas confondre :

- le programme courant ;
- l'état d'une exécution en cours.

## Statut

Déduit

## Justification

La distinction entre programme courant et état d'exécution est nécessaire dès lors que les sources décrivent la possibilité d'interrompre puis de reprendre une exécution tout en permettant à l'utilisateur d'intervenir dans la session.

## Conséquences

### Programme courant

Il représente la dernière version acceptée.

### Exécution

Une exécution active possède son propre état.

### Mise au point

La modification du programme et la reprise d'une exécution ne sont pas assimilées à une seule opération.

### Mémoire

Les deux états peuvent avoir des durées de vie différentes.

## Traçabilité

HIS-6.5 → EXI-0026 → VM-0607

HIS-8.2 → EXI-0026 → VM-0607

---

# 7. Exécution de la représentation compilée

Le système historique distingue l'exécution immédiate, associée au mode machine de bureau, et l'exécution différée d'un programme préalablement constitué. Dans les deux cas, l'interpréteur exécute la représentation produite par le compilateur. [INRDP75 §3.2] [INRDP75 §3.3]

Le présent chapitre définit le mécanisme abstrait d'exécution de la représentation compilée. Il ne fixe pas les conditions matérielles historiques de chargement de l'interpréteur.

## 7.1 Interpréteur abstrait

### VM-0701
Interpréteur de la représentation compilée

## Objet

Définir le rôle de l'interpréteur dans la machine abstraite.

## Contraintes historiques

L'interpréteur exécute le programme ou la ligne sous la forme fournie par le compilateur.

Il détecte les erreurs d'exécution.

En mode machine de bureau, il est appelé immédiatement après compilation.

En mode programmé, il est appelé à la demande d'EXECUTER.

[INRDP75 §3.3]

## Exigences concernées

### EXI-0027

Titre

Exécution directe de la représentation compilée

Description

L'interpréteur exécute directement la représentation compilée du programme ou de la ligne, sans nécessiter une recompilation préalable du texte source lors de chaque exécution.

Origine

HIS-4.5.3

HIS-8.1

HIS-8.2

Sources historiques

[INRDP75 §3.3]

[INRDP75 §3.2]

Statut

Documenté

Justification

Les sources décrivent explicitement l'interpréteur comme exécutant la forme fournie par le compilateur.

Conséquences

La représentation compilée est l'entrée normative de l'interpréteur.

Utilisé par

VM-0701

VM-0702

VM-0703

Observations

La structure interne de l'interpréteur n'est pas documentée.

## Analyse

### Éléments documentés

L'interpréteur reçoit la représentation compilée.

Il exécute cette représentation.

Il détecte les erreurs d'exécution.

### Éléments non documentés

Les sources ne décrivent pas l'algorithme précis.

Elles ne décrivent pas les fonctions internes de l'interpréteur.

### Contraintes

Le comportement de l'interpréteur doit être défini indépendamment de toute représentation matérielle.

## Reconstruction retenue

L'interpréteur est défini comme un processeur abstrait qui reçoit :

- un programme courant ou une ligne compilée ;
- un environnement d'exécution ;
- un état d'exécution initial.

Il produit :

- des modifications de l'environnement d'exécution ;
- des sorties ou interactions ;
- des diagnostics d'exécution ;
- un nouvel état d'exécution.

La représentation compilée est parcourue selon sa structure logique.

## Statut

Déduit

## Justification

Le rôle général est documenté.

La définition abstraite des entrées, de l'état et des résultats est nécessaire pour spécifier la machine indépendamment des processeurs matériels historiques.

## Conséquences

### Pile d'évaluation

L'interpréteur utilise l'état d'évaluation nécessaire aux expressions.

### Cadres d'activation

L'exécution des procédures utilise les cadres correspondants.

### Variables

L'interpréteur résout les références vers les objets d'exécution.

### Gestion des erreurs

Les erreurs d'exécution sont produites par ce processeur.

## Traçabilité

HIS-4.5.3 → EXI-0027 → VM-0701

---

## 7.2 Exécution immédiate

### VM-0702
Exécution immédiate d'une ligne compilée

## Objet

Définir le déroulement abstrait du mode machine de bureau.

## Contraintes historiques

Une expression ou une opération introduite dans ce mode est compilée, interprétée puis exécutée dès la réception de la fin de ligne.

[INRDP75 §Mode machine de bureau]

[LSE15 §9-5]

## Exigences concernées

### EXI-0028

Titre

Enchaînement immédiat compilation-exécution

Description

Une entrée destinée à l'exécution immédiate est compilée puis exécutée par l'interpréteur dès que sa validation et sa compilation sont achevées avec succès.

Origine

HIS-3.8

HIS-8.1

Sources historiques

[INRDP75 §3.2]

[INRDP75 §Mode machine de bureau]

[LSE15 §9-5]

Statut

Documenté

Justification

Le manuel décrit explicitement l'enchaînement compilation, interprétation et exécution immédiate.

Conséquences

Le mode machine de bureau ne nécessite pas l'intégration préalable de la ligne dans un programme à exécuter ultérieurement.

Utilisé par

VM-0702

VM-0501

Observations

Le traitement des lignes de programme numérotées reste distinct du traitement des expressions exécutées immédiatement.

## Analyse

### Éléments documentés

L'exécution immédiate intervient après compilation.

La valeur peut être affichée.

### Éléments non documentés

La frontière exacte entre une ligne interactive et une ligne intégrée au programme dépend de la syntaxe et du mode de la console.

### Contraintes

Une erreur de compilation empêche l'exécution.

Une exécution immédiate ne doit pas modifier le programme courant sauf si la construction entrée est explicitement une ligne de programme.

## Reconstruction retenue

Le mode machine de bureau est modélisé comme un chemin d'exécution court :

entrée → validation → compilation → interprétation → résultat.

Le programme courant n'est pas modifié par une expression purement immédiate.

Lorsqu'une instruction immédiate modifie explicitement l'état d'exécution, ses effets portent sur l'environnement d'exécution courant.

## Statut

Déduit

## Justification

L'enchaînement principal est documenté.

La distinction entre l'état du programme courant et l'état d'exécution immédiate permet de conserver la séparation historique entre programmation et calcul direct.

## Conséquences

### Variables

Une expression immédiate peut lire ou modifier des variables accessibles.

### Pile d'évaluation

Elle est créée ou utilisée pour la durée de l'opération.

### Programme courant

Il reste inchangé sauf opération explicitement destinée à le modifier.

## Traçabilité

HIS-3.8 → EXI-0028 → VM-0702

HIS-8.1 → EXI-0028 → VM-0702

---

## 7.3 Exécution différée

### VM-0703
Exécution programmée

## Objet

Définir le mécanisme par lequel le programme courant est exécuté après une demande explicite.

## Contraintes historiques

Le programme est préalablement constitué de plusieurs lignes.

La commande EXECUTER demande son exécution.

L'interpréteur est alors chargé et exécute le programme.

[INRDP75 §3.3]

[INRDP75 §4-3]

## Exigences concernées

### EXI-0029

Titre

Exécution explicite du programme courant

Description

L'exécution programmée du programme courant commence sur demande explicite de l'utilisateur et porte sur l'état courant du programme au moment où l'exécution est engagée.

Origine

HIS-8.2

HIS-3.13

Sources historiques

[INRDP75 §3.3]

[INRDP75 §4-3]

Statut

Documenté

Justification

Le mode programmé est explicitement décrit comme une exécution demandée par la commande EXECUTER.

Conséquences

Une phase d'exécution distincte de l'entrée des lignes est nécessaire.

Utilisé par

VM-0703

VM-0607

VM-2101

Observations

Les détails du chargement matériel de l'interpréteur ne sont pas reconstruits.

## Analyse

### Éléments documentés

Le programme courant peut être exécuté après son entrée.

L'exécution est différée.

EXECUTER déclenche l'exécution.

### Éléments non documentés

La procédure exacte de préparation mémoire n'est pas décrite au niveau abstrait.

### Contraintes

L'exécution doit utiliser le programme courant.

La compilation des lignes doit être achevée avant l'exécution.

## Reconstruction retenue

La commande EXECUTER crée un état d'exécution initial à partir du programme courant.

L'interpréteur initialise :

- la position d'exécution ;
- l'environnement d'exécution ;
- la pile d'évaluation vide ;
- l'état des cadres d'activation.

Il commence ensuite l'interprétation.

Le programme courant n'est pas modifié par la simple demande d'exécution.

## Statut

Déduit

## Justification

Cette reconstruction sépare l'état du programme et l'état d'exécution tout en respectant l'enchaînement documenté.

## Conséquences

### Interpréteur

Il reçoit une vue du programme courant.

### Mémoire

L'état d'exécution est distinct du stockage logique du programme.

### Mise au point

L'état d'exécution peut être suspendu.

## Traçabilité

HIS-8.2 → EXI-0029 → VM-0703

---

## 7.4 Séquence d'exécution d'une représentation post-fixée

### VM-0704
Parcours de la chaîne post-fixée

## Objet

Définir le déroulement abstrait du parcours d'une représentation post-fixée.

## Contraintes historiques

La représentation post-fixée est conçue pour une interprétation efficace.

[NOYELLE89 p.220]

## Exigences concernées

EXI-0011

EXI-0012

EXI-0013

EXI-0014

## Analyse

### Éléments documentés

La représentation est destinée à être interprétée.

### Éléments non documentés

La méthode précise n'est pas connue.

### Contraintes

L'évaluation doit être cohérente avec l'ordre post-fixé.

## Reconstruction retenue

L'interpréteur parcourt la séquence dans son ordre.

Lorsqu'il rencontre :

- une valeur, il la place dans le contexte d'évaluation ;
- une référence, il résout l'objet correspondant et rend sa valeur ou sa désignation disponible ;
- une opération codée, il applique la sémantique de cette opération aux éléments requis.

Le nombre d'éléments consommés par une opération est déterminé par la sémantique de cette opération.

Le résultat éventuel est placé dans le contexte d'évaluation.

Cette règle constitue le modèle abstrait général de l'exécution post-fixée.

## Statut

Déduit

## Justification

La reconstruction découle directement de la nature post-fixée retenue.

Elle ne préjuge d'aucune représentation de pile ou de registre historique.

## Conséquences

### Interpréteur

Le parcours est séquentiel.

### Expressions

Les opérations ont une arité déterminée.

### Fonctions

Les appels peuvent être représentés comme des opérations contrôlant le flux d'exécution.

### Procédures

Les appels peuvent créer un nouveau cadre d'activation.

## Traçabilité

CPF-7.2 → EXI-0011, EXI-0012 → VM-0704

---

## 7.5 Fin normale d'exécution

### VM-0705
Terminaison de l'exécution

## Objet

Définir la condition normale de fin d'une exécution.

## Contraintes historiques

Le langage comporte des mécanismes permettant de terminer l'exécution.

Le système distingue l'exécution d'un programme de la gestion du programme courant.

[INRDP75 §4.3]

[LSE15 §9]

## Exigences concernées

### EXI-0030

Titre

Terminaison explicite ou naturelle de l'exécution

Description

Une exécution se termine lorsque le flux d'exécution atteint la fin du programme ou lorsqu'une instruction ou commande de terminaison demande explicitement son arrêt.

Origine

HIS-8.2

HIS-14.1

Sources historiques

[INRDP75 §4.3]

[LSE15 §9]

Statut

Déduit

Justification

La machine abstraite doit distinguer la fin du flux d'exécution de la conservation du programme courant.

Conséquences

La fin d'une exécution ne détruit pas automatiquement le programme courant.

Utilisé par

VM-0705

VM-2103

VM-0303

Observations

Les sources ne permettent pas d'unifier toutes les variantes historiques de terminaison.

## Analyse

### Éléments documentés

L'exécution et le programme courant constituent deux notions distinctes.

### Éléments non documentés

Les détails de la terminaison normale ne sont pas décrits comme une transition abstraite unique.

### Contraintes

Une exécution terminée doit laisser le programme courant disponible pour les opérations ultérieures, sauf commande explicite de destruction ou de remplacement.

## Reconstruction retenue

La terminaison détruit l'état temporaire de l'exécution sans détruire automatiquement le programme courant.

Les ressources d'exécution temporaires sont libérées selon les mécanismes mémoire.

Le programme courant reste disponible pour :

- le listage ;
- une nouvelle exécution ;
- une modification ;
- un rangement.

## Statut

Déduit

## Justification

Cette séparation est nécessaire pour rendre cohérent le fonctionnement conversationnel dans lequel un même programme peut être exécuté plusieurs fois sans être réintroduit.

## Conséquences

### Programme courant

Il survit à la fin d'une exécution.

### Mémoire

Les objets propres à l'exécution peuvent être libérés.

### Dialogue

L'utilisateur revient à un état conversationnel permettant de poursuivre le travail.

## Traçabilité

HIS-8.2 → EXI-0030 → VM-0705

---

# 8. Environnement d'exécution

L'exécution d'un programme compilé ne se réduit pas à la seule chaîne post-fixée. Les sources historiques décrivent des variables, des tableaux, des chaînes, des procédures, des objets locaux, des paramètres et des mécanismes de mise au point. L'interpréteur doit donc disposer d'un environnement d'exécution distinct de la représentation compilée elle-même.

Le Tome II reconstruit cet environnement comme un état abstrait associé à une exécution.

## 8.1 État d'exécution

### VM-0801
État d'exécution

## Objet

Définir l'état abstrait nécessaire pour poursuivre l'exécution d'un programme compilé.

## Contraintes historiques

Le système permet l'interruption et la reprise.

L'utilisateur peut examiner ou modifier des valeurs pendant la mise au point.

Noyelle décrit la possibilité d'arrêter un programme, de connaître l'endroit d'arrêt, d'interroger ou modifier un objet, puis de continuer l'exécution.

[NOYELLE89 p.221]

[INRDP75 Annexe II]

[LSE15 §9-3-5]

## Exigences concernées

### EXI-0031

Titre

État d'exécution distinct du programme compilé

Description

Une exécution possède un état distinct de la représentation compilée du programme, contenant les informations nécessaires à sa progression et à sa reprise.

Origine

HIS-3.13

HIS-8.3

Sources historiques

[NOYELLE89 p.221]

[INRDP75 Annexe II]

Statut

Déduit

Justification

La possibilité d'interrompre puis de reprendre une exécution impose la conservation d'un état qui ne peut être réduit au seul texte ou code du programme.

Conséquences

L'environnement d'exécution comprend une position d'exécution et l'état des objets nécessaires.

Utilisé par

VM-0801

VM-0604

VM-0802

VM-0901

VM-2102

Observations

La structure exacte de cet état n'est pas documentée.

## Analyse

### Éléments documentés

L'exécution possède un état observable.

Cet état peut être examiné et modifié en mise au point.

### Éléments non documentés

La répartition précise entre programme compilé, variables, pile et contexte n'est pas donnée.

### Contraintes

Une interruption doit pouvoir conserver cet état.

Une reprise doit le restaurer.

## Reconstruction retenue

L'état d'exécution abstrait comprend au minimum :

- la position d'exécution ;
- la pile d'évaluation ;
- les cadres d'activation actifs ;
- l'environnement des variables accessibles ;
- les informations nécessaires à la communication avec le système ;
- l'état permettant la poursuite après interruption.

La chaîne post-fixée ne contient pas elle-même cet état dynamique.

## Statut

Déduit

## Justification

Cette séparation constitue le modèle minimal permettant de distinguer le programme immuable pendant son exécution de l'état dynamique créé par l'interpréteur.

## Conséquences

### Interpréteur

Il manipule l'état d'exécution.

### Programme courant

Il reste une donnée compilée séparée.

### Mise au point

Elle peut agir sur l'état dynamique.

### Temps partagé

L'état peut être suspendu et restauré.

## Traçabilité

HIS-3.13 → EXI-0031 → VM-0801

HIS-8.3 → EXI-0031 → VM-0801

---

## 8.2 Environnement des objets

### VM-0802
Environnement d'accès aux objets

## Objet

Définir l'abstraction permettant à l'interpréteur de retrouver les objets désignés par la représentation compilée.

## Contraintes historiques

Les langages LSx utilisent des objets, notamment variables et objets locaux.

Le système permet l'accès aux objets en cours d'exécution et en mode de mise au point.

[NOYELLE89 p.218-221]

[INRDP75 §3-2]

## Exigences concernées

### EXI-0032

Titre

Résolution des références vers les objets d'exécution

Description

Toute référence compilée à un objet doit pouvoir être résolue dans l'environnement d'exécution applicable au moment de son utilisation.

Origine

HIS-10.1

HIS-10.2

HIS-13.1

Sources historiques

[INRDP75 §3-1]

[INRDP75 §4-1]

[NOYELLE89 p.218-221]

Statut

Déduit

Justification

La représentation compilée doit permettre l'accès aux objets utilisés lors de l'exécution.

Conséquences

L'environnement d'exécution fournit une résolution abstraite des références.

Utilisé par

VM-0802

VM-0405

VM-1001

VM-1101

VM-1301

Observations

La structure des tables internes n'est pas spécifiée.

## Analyse

### Éléments documentés

Les programmes utilisent des objets identifiés.

Les variables et objets locaux peuvent être accessibles.

### Éléments non documentés

Le système ne documente pas la table interne associant les noms aux objets.

### Contraintes

La résolution doit respecter la portée des objets applicable à l'exécution.

Elle doit produire une erreur si aucun objet valide ne peut être trouvé alors que la construction l'exige.

## Reconstruction retenue

L'environnement d'exécution possède une opération abstraite de résolution.

Cette opération reçoit une identité logique et un contexte d'exécution.

Elle renvoie :

- l'objet correspondant ;
- ou une erreur si aucun objet applicable n'existe.

La résolution tient compte de la portée des cadres d'activation lorsque ceux-ci existent.

Elle ne dépend pas directement d'une adresse mémoire.

## Statut

Déduit

## Justification

La reconstruction permet de séparer la notion sémantique d'identité d'un objet de son emplacement matériel.

## Conséquences

### Variables

Les variables sont accessibles par résolution.

### Procédures

Les paramètres et variables locales utilisent le même mécanisme abstrait.

### Mémoire

Une référence ne constitue pas une adresse physique.

## Traçabilité

HIS-10.1 → EXI-0032 → VM-0802

HIS-13.1 → EXI-0032 → VM-0802

---

## 8.3 Accès contrôlé à l'environnement

### VM-0803
Accès à l'état d'exécution

## Objet

Définir la capacité du système conversationnel à examiner l'état d'une exécution suspendue.

## Contraintes historiques

L'utilisateur peut examiner des valeurs et savoir où un programme a été arrêté.

[NOYELLE89 p.221]

Le mode machine de bureau permet d'interroger des variables dans certaines conditions.

[INRDP75 §2]

[LSE15 §9-5]

## Exigences concernées

### EXI-0033

Titre

Observation de l'état d'exécution

Description

Une exécution suspendue doit pouvoir exposer à l'environnement conversationnel les informations d'état autorisées par le langage et le mode d'exécution.

Origine

HIS-3.13

HIS-8.3

Sources historiques

[NOYELLE89 p.221]

[INRDP75 §2]

[LSE15 §9-5]

Statut

Documenté

Justification

Les sources décrivent explicitement l'accès aux valeurs et à l'état du programme pendant la mise au point.

Conséquences

La machine abstraite doit offrir une interface entre l'état d'exécution et le dialogue utilisateur.

Utilisé par

VM-0803

VM-2001

VM-2102

Observations

La liste exacte des informations accessibles peut dépendre de l'implantation ou du mode.

## Analyse

### Éléments documentés

L'utilisateur peut examiner des objets.

Il peut connaître la position d'arrêt.

Il peut modifier certaines valeurs.

### Éléments non documentés

Les sources ne définissent pas une interface abstraite unique à toutes les implantations.

### Contraintes

L'accès ne doit pas nécessiter la reconstruction du programme source.

## Reconstruction retenue

L'environnement conversationnel peut demander à l'exécution suspendue :

- la valeur d'un objet accessible ;
- la position logique d'exécution ;
- les informations de contexte nécessaires au diagnostic.

Toute opération de consultation est réalisée sur l'état d'exécution courant.

## Statut

Déduit

## Justification

La reconstruction rend explicite la relation fonctionnelle documentée entre la mise au point et l'état interne de l'exécution.

## Conséquences

### Mise au point

L'état d'exécution devient observable.

### Interpréteur

Il doit permettre la suspension de manière cohérente.

### Variables

Les valeurs peuvent être interrogées.

## Traçabilité

HIS-3.13 → EXI-0033 → VM-0803

HIS-8.3 → EXI-0033 → VM-0803

---

# 9. Cadres d'activation

Les sources historiques relatives aux premiers langages LSx décrivent les procédures, fonctions, objets locaux et modes de passage des paramètres. Elles fournissent donc les contraintes nécessaires pour reconstruire un mécanisme d'activation distinct de la représentation compilée du programme.

Le niveau de détail historique ne permet pas de déterminer la forme matérielle des cadres d'activation. Le présent chapitre ne reconstruit donc qu'un cadre abstrait.

## 9.1 Activation d'une procédure

### VM-0901
Cadre d'activation

## Objet

Définir l'état abstrait créé lors de l'activation d'une procédure ou fonction.

## Contraintes historiques

Le récit de Noyelle mentionne les procédures, fonctions et sous-programmes, ainsi que les objets locaux et les modes de passage des paramètres.

[NOYELLE89 p.218-220]

Le manuel LSE 15 décrit les procédures, les paramètres et les variables locales.

[LSE15 §6]

## Exigences concernées

### EXI-0034

Titre

Cadre d'activation des appels

Description

L'activation d'une procédure ou fonction crée un contexte d'exécution distinct permettant d'associer les paramètres et les objets locaux à cet appel.

Origine

HIS-13.1

HIS-13.2

Sources historiques

[NOYELLE89 p.218-220]

[LSE15 §6]

Statut

Déduit

Justification

La présence de paramètres et de variables locales implique un contexte associé à chaque activation.

Conséquences

Les variables locales et les paramètres ne doivent pas être assimilés aux variables globales du programme courant.

Utilisé par

VM-0901

VM-0902

VM-0903

VM-1301

Observations

La structure mémoire concrète n'est pas documentée.

## Analyse

### Éléments documentés

Les procédures possèdent des paramètres.

Les procédures peuvent utiliser des variables locales.

### Éléments non documentés

La mémoire réservée à ces variables n'est pas décrite.

Les mécanismes de création et destruction des appels ne sont pas détaillés.

### Contraintes

Deux activations successives ou imbriquées doivent pouvoir posséder des états locaux distincts.

Les paramètres doivent être associés à l'appel qui les utilise.

## Reconstruction retenue

Chaque appel de procédure ou fonction crée un cadre d'activation abstrait.

Le cadre contient :

- les paramètres de l'appel ;
- les variables locales ;
- l'état de retour ;
- la référence vers le cadre appelant ;
- la position de retour ;
- les informations nécessaires à la restitution du résultat, lorsqu'il existe.

Le cadre est actif jusqu'à la sortie de l'appel.

## Statut

Déduit

## Justification

Le cadre d'activation est le modèle minimal permettant de séparer les objets locaux et les paramètres de différentes activations.

Il ne prétend pas reconstituer la structure mémoire historique.

## Conséquences

### Pile d'évaluation

Les valeurs temporaires de l'appel peuvent être associées au cadre.

### Variables

La résolution distingue les objets locaux des objets extérieurs.

### Procédures

L'appel devient une transition vers un nouveau cadre.

### Mémoire

Les cadres sont des objets dynamiques.

## Traçabilité

HIS-13.1 → EXI-0034 → VM-0901

HIS-13.2 → EXI-0034 → VM-0901

---

## 9.2 Empilement des cadres

### VM-0902
Chaînage des cadres d'activation

## Objet

Définir la relation entre une activation et l'activation qui l'a appelée.

## Contraintes historiques

Les procédures peuvent être appelées dans le cadre de l'exécution d'autres procédures.

Les variables locales sont associées aux procédures.

[LSE15 §6]

[NOYELLE89 p.218-220]

## Exigences concernées

### EXI-0035

Titre

Chaînage des activations

Description

Chaque activation active doit pouvoir retrouver le contexte d'activation qui l'a appelée jusqu'à l'activation initiale.

Origine

HIS-13.1

Sources historiques

[LSE15 §6]

[NOYELLE89 p.218-220]

Statut

Déduit

Justification

La gestion des appels imbriqués exige la conservation du contexte appelant.

Conséquences

L'environnement d'exécution forme une chaîne d'activations.

Utilisé par

VM-0902

VM-0903

VM-1301

VM-2102

Observations

La forme concrète de cette chaîne n'est pas spécifiée.

## Analyse

### Éléments documentés

Les procédures peuvent être appelées.

Les variables locales sont propres à leur contexte.

### Éléments non documentés

L'organisation interne des activations imbriquées n'est pas décrite.

### Contraintes

Une activation doit retrouver son contexte appelant.

La sortie d'une activation doit restaurer le contexte précédent.

## Reconstruction retenue

Les cadres d'activation actifs forment une chaîne.

Le cadre courant désigne le cadre appelant.

Lorsqu'un appel est effectué :

1. un nouveau cadre est créé ;
2. le cadre courant devient son appelant ;
3. le nouveau cadre devient courant.

Lors du retour :

1. le cadre courant est terminé ;
2. son appelant redevient courant.

## Statut

Déduit

## Justification

Cette reconstruction est la conséquence minimale de l'existence d'activations distinctes et de variables locales.

## Conséquences

### Interpréteur

L'exécution des appels modifie le cadre courant.

### Variables

La résolution peut parcourir les cadres selon les règles de portée.

### Mémoire

Les cadres ont une durée de vie liée aux appels.

## Traçabilité

HIS-13.1 → EXI-0035 → VM-0902

---

## 9.3 Adresse de retour abstraite

### VM-0903
Position de retour

## Objet

Définir la position à laquelle une activation retourne après la fin d'une procédure ou fonction.

## Contraintes historiques

Les procédures peuvent comporter des instructions de retour.

Le manuel LSE 15 décrit les instructions RETOUR et RESULTAT.

[LSE15 §6]

## Exigences concernées

### EXI-0036

Titre

Retour au point d'appel

Description

La fin normale ou explicite d'une activation restitue le contrôle au point d'exécution suivant l'appel.

Origine

HIS-13.2

Sources historiques

[LSE15 §6]

Statut

Documenté

Justification

Les instructions de retour décrites par le manuel établissent une continuité entre l'appel d'une procédure et la poursuite du programme appelant.

Conséquences

Le cadre d'activation conserve une position de retour abstraite.

Utilisé par

VM-0903

VM-1302

VM-2101

Observations

Le mécanisme concret de représentation de cette position n'est pas documenté.

## Analyse

### Éléments documentés

Une procédure peut retourner.

Une fonction peut produire un résultat.

### Éléments non documentés

La représentation interne de l'adresse de retour n'est pas connue.

### Contraintes

Le retour doit restaurer le flux du programme appelant.

## Reconstruction retenue

Chaque cadre d'activation conserve une position de retour abstraite.

Cette position désigne l'endroit du programme appelant où l'exécution doit reprendre.

La position n'est pas une adresse mémoire.

## Statut

Déduit

## Justification

La position de retour abstraite est nécessaire à la sémantique des appels et permet d'éviter toute dépendance matérielle.

## Conséquences

### Interpréteur

Le retour restaure une position d'exécution.

### Pile d'évaluation

Le résultat d'une fonction peut être rendu à l'appelant.

### Cadres

Le cadre appelant redevient courant.

## Traçabilité

HIS-13.2 → EXI-0036 → VM-0903

---

# 10. Pile d'évaluation

La représentation post-fixée retenue pour la machine abstraite conduit naturellement à un mécanisme d'évaluation séquentielle. La pile d'évaluation constitue ici un modèle abstrait permettant de représenter les valeurs intermédiaires nécessaires à l'application des opérations.

Ce mécanisme ne prétend pas reproduire une pile matérielle historique.

## 10.1 Pile d'évaluation abstraite

### VM-1001
Pile d'évaluation

## Objet

Définir le support abstrait des valeurs intermédiaires produites pendant l'évaluation d'une représentation post-fixée.

## Contraintes historiques

La chaîne post-fixée est présentée comme adaptée à une interprétation efficace.

La représentation post-fixée place les opérandes avant les opérations qui les consomment.

[NOYELLE89 p.220]

## Exigences concernées

### EXI-0037

Titre

Évaluation par valeurs intermédiaires

Description

L'exécution d'une représentation post-fixée doit pouvoir conserver les résultats intermédiaires nécessaires aux opérations suivantes.

Origine

CPF-7.2

Sources historiques

[NOYELLE89 p.220]

Statut

Déduit

Justification

Une évaluation post-fixée d'expressions composées nécessite la conservation de résultats intermédiaires.

Conséquences

La machine abstraite dispose d'une pile d'évaluation.

Utilisé par

VM-1001

VM-1002

VM-1502

Observations

La pile est un mécanisme abstrait.

## Analyse

### Éléments documentés

L'utilisation d'une forme post-fixée est documentée pour le LSD.

### Éléments non documentés

L'existence historique d'une pile d'évaluation explicitement nommée n'est pas établie dans les sources consultées.

### Contraintes

Les expressions composées doivent être évaluables.

Les opérations doivent disposer de leurs opérandes.

## Reconstruction retenue

La machine abstraite utilise une pile d'évaluation.

La pile est une séquence ordonnée de valeurs temporaires.

Les opérations peuvent :

- empiler une valeur ;
- consommer une ou plusieurs valeurs ;
- produire une valeur.

Le sommet de la pile représente la dernière valeur produite qui n'a pas encore été consommée.

## Statut

Choix de conception

## Justification

La pile d'évaluation est le modèle abstrait le plus simple correspondant à une représentation post-fixée.

Elle est indépendante de l'architecture matérielle et permet de spécifier précisément l'interpréteur.

## Conséquences

### Interpréteur

Les opérations de la chaîne post-fixée manipulent la pile.

### Expressions

Les sous-expressions produisent des valeurs intermédiaires.

### Fonctions

Les résultats peuvent être transmis à l'appelant.

### Mémoire

La pile d'évaluation constitue une mémoire temporaire.

## Traçabilité

CPF-7.2 → EXI-0037 → VM-1001

---

## 10.2 Opérations sur la pile

### VM-1002
Opérations de pile

## Objet

Définir les opérations abstraites accessibles à l'interpréteur pour manipuler la pile d'évaluation.

## Contraintes historiques

La représentation post-fixée doit pouvoir être interprétée.

Les opérations compilées sont exécutées dans l'ordre.

## Exigences concernées

EXI-0037

## Analyse

### Éléments documentés

La représentation est exécutable.

### Éléments non documentés

Les primitives de manipulation ne sont pas décrites.

### Contraintes

L'évaluation doit être déterministe.

## Reconstruction retenue

La pile fournit au minimum les opérations abstraites suivantes :

- EMPILER valeur ;
- LIRE sommet ;
- RETIRER sommet ;
- REMPLACER sommet ;
- CONSOMMER n valeurs ;
- PRODUIRE valeur.

Ces opérations sont des primitives de la machine abstraite et non des opérations du langage L.S.E. exposées à l'utilisateur.

## Statut

Choix de conception

## Justification

Ces primitives constituent le minimum nécessaire pour spécifier l'évaluation post-fixée.

Elles n'impliquent aucune représentation matérielle.

## Conséquences

### Interpréteur

Chaque opération codée indique implicitement les opérations de pile nécessaires.

### Expressions

La composition des expressions devient déterministe.

### Erreurs

Une consommation impossible constitue une erreur interne de la représentation compilée ou une situation qui ne devrait jamais apparaître pour un objet correctement compilé.

## Traçabilité

EXI-0037 → VM-1002

---

# 11. Variables

Le système L.S.E. manipule des variables numériques et des variables chaînes, ainsi que des tableaux. Les procédures introduisent également des objets locaux. Les sources montrent que la déclaration et l'utilisation des variables peuvent intervenir dans des contextes distincts de la compilation syntaxique.

La machine abstraite doit donc distinguer l'identité d'une variable, sa définition dans un environnement et sa valeur courante.

## 11.1 Identité d'une variable

### VM-1101
Identificateur de variable

## Objet

Définir l'identité logique utilisée pour accéder à une variable.

## Contraintes historiques

Les variables sont désignées par des identificateurs.

Les sources décrivent des variables numériques et chaînes.

[INRDP75 §3-1]

[LSE15 §3]

## Exigences concernées

### EXI-0038

Titre

Identité stable des variables

Description

Chaque variable déclarée est associée à un identificateur logique permettant son accès dans le contexte d'exécution où elle est visible.

Origine

HIS-10.1

Sources historiques

[INRDP75 §3-1]

[LSE15 §3]

Statut

Documenté

Justification

Le langage emploie des identificateurs pour désigner les variables.

Conséquences

La machine abstraite doit pouvoir résoudre un identificateur en objet variable.

Utilisé par

VM-1101

VM-0802

VM-1001

Observations

La représentation interne des identificateurs n'est pas documentée.

## Analyse

### Éléments documentés

Les variables sont identifiées par des noms.

### Éléments non documentés

Le mécanisme de correspondance entre nom et objet n'est pas précisé.

### Contraintes

Un identificateur doit permettre de retrouver une variable visible.

## Reconstruction retenue

La machine abstraite définit l'identificateur comme une identité logique.

La résolution d'un identificateur dépend de l'environnement courant et des cadres d'activation actifs.

L'identificateur n'est pas assimilé à une adresse mémoire.

## Statut

Déduit

## Justification

La reconstruction sépare la sémantique de nommage de la représentation matérielle.

## Conséquences

### Compilation

L'identificateur peut être transformé en référence compilée.

### Interpréteur

La référence peut être résolue à l'exécution.

### Procédures

La résolution tient compte des objets locaux.

## Traçabilité

HIS-10.1 → EXI-0038 → VM-1101

---

## 11.2 Valeur d'une variable

### VM-1102
État courant d'une variable

## Objet

Définir la valeur courante associée à une variable pendant l'exécution.

## Contraintes historiques

Les variables peuvent être interrogées et modifiées.

Le mode machine de bureau permet notamment d'obtenir et, dans certaines conditions, de modifier la valeur de variables.

[INRDP75 §2]

[LSE15 §9-5]

## Exigences concernées

### EXI-0039

Titre

Valeur mutable des variables

Description

Une variable déclarée possède une valeur courante susceptible d'être consultée et modifiée par les opérations autorisées du langage ou de l'environnement d'exécution.

Origine

HIS-10.1

HIS-3.13

Sources historiques

[INRDP75 §2]

[LSE15 §9-5]

[NOYELLE89 p.221]

Statut

Documenté

Justification

Les sources décrivent explicitement la consultation et la modification de valeurs dans le cadre du fonctionnement conversationnel et de la mise au point.

Conséquences

Les variables doivent être représentées comme des objets d'état plutôt que comme de simples constantes compilées.

Utilisé par

VM-1102

VM-0803

VM-1501

Observations

La représentation interne de la valeur n'est pas fixée ici.

## Analyse

### Éléments documentés

Une variable possède une valeur.

Cette valeur peut évoluer.

### Éléments non documentés

La structure mémoire de la variable n'est pas documentée.

### Contraintes

Une référence vers une variable doit accéder à sa valeur courante.

## Reconstruction retenue

Une variable est un objet d'exécution comportant :

- un identificateur logique ;
- un type ;
- un état de déclaration ;
- une valeur courante.

La valeur est mutable lorsqu'aucune règle du langage ne l'interdit.

La représentation de la valeur dépend du type.

## Statut

Déduit

## Justification

Cette structure minimale correspond au comportement documenté sans imposer de représentation mémoire.

## Conséquences

### Interpréteur

Une opération de lecture obtient la valeur courante.

### Affectation

Une opération d'écriture remplace la valeur.

### Mise au point

L'environnement conversationnel peut consulter ou modifier la valeur lorsque le mode le permet.

## Traçabilité

HIS-10.1 → EXI-0039 → VM-1102

HIS-3.13 → EXI-0039 → VM-1102

---

## 11.3 Déclaration différée et résolution à l'exécution

### VM-1103
Résolution différée des variables

## Objet

Permettre qu'une erreur liée à une variable soit détectée à l'exécution lorsqu'elle n'est pas déterminable par la seule syntaxe de la ligne.

## Contraintes historiques

Le manuel de 1975 donne l'exemple d'une variable chaîne dont la déclaration peut manquer sans provoquer une erreur lors de l'entrée de la ligne.

L'erreur est alors détectée au moment de l'exécution.

[INRDP75 §« Présentation d'un programme L.S.E. »]

## Exigences concernées

EXI-0019

## Analyse

### Éléments documentés

Certaines erreurs de référence ne sont pas détectées lors de la compilation syntaxique.

### Éléments non documentés

La résolution exacte des identificateurs n'est pas décrite.

### Contraintes

La machine abstraite doit autoriser une référence compilée à être résolue lors de l'exécution.

## Reconstruction retenue

La compilation encode une référence logique à l'objet désigné.

L'interpréteur résout cette référence dans l'environnement courant lorsqu'elle est nécessaire.

Si aucun objet valide n'est disponible et que la construction l'exige, l'interpréteur produit une erreur d'exécution.

## Statut

Déduit

## Justification

Cette reconstruction explique directement le comportement documenté des erreurs différées.

## Conséquences

### Compilation

Elle n'a pas à résoudre toutes les références.

### Interpréteur

Il devient responsable de la validité dynamique des accès.

### Erreurs

Le diagnostic peut survenir au moment exact où la référence est utilisée.

## Traçabilité

HIS-5.2 → EXI-0019 → VM-1103

HIS-10.1 → EXI-0032 → VM-1103

---

# 12. Tableaux

Les tableaux constituent les objets composés explicitement retenus dans la conception initiale des langages LSx. Le L.S.E. documente des tableaux à une ou deux dimensions, dont la déclaration réserve l'espace correspondant. Les mécanismes de mémoire associés doivent toutefois rester distincts des représentations concrètes employées par les systèmes historiques.

## 12.1 Objet tableau

### VM-1201
Tableau

## Objet

Définir le tableau comme un objet composé indexable.

## Contraintes historiques

Les tableaux peuvent posséder une ou deux dimensions.

La déclaration précise l'espace à réserver.

L'utilisation d'un tableau non déclaré provoque une erreur d'exécution.

[INRDP75 §4-1]

[LSE15 §3-1]

[NOYELLE89 p.219]

## Exigences concernées

### EXI-0040

Titre

Tableaux à une ou deux dimensions

Description

La machine abstraite doit prendre en charge des tableaux possédant une ou deux dimensions.

Origine

HIS-10.6

Sources historiques

[INRDP75 §4-1]

[LSE15 §3-1]

[NOYELLE89 p.219]

Statut

Documenté

Justification

Les sources décrivent explicitement ce nombre maximal de dimensions.

Conséquences

Les tableaux disposent d'un ou deux indices.

Utilisé par

VM-1201

VM-1202

VM-1601

Observations

Aucune structure mémoire particulière n'est imposée.

## Analyse

### Éléments documentés

Les tableaux ont une ou deux dimensions.

Ils doivent être déclarés.

Ils occupent un espace dépendant de leur taille.

### Éléments non documentés

La représentation mémoire exacte n'est pas donnée.

### Contraintes

La machine abstraite doit vérifier la validité des indices selon les règles du langage.

## Reconstruction retenue

Un tableau est un objet d'exécution composé d'un ensemble de cases.

Le tableau possède :

- une dimension de 1 ou 2 ;
- des bornes ou tailles correspondant à sa déclaration ;
- une valeur pour chaque élément.

L'accès à un élément reçoit un ou deux indices.

## Statut

Documenté

## Justification

Le comportement général du tableau est directement documenté.

La structure abstraite des cases constitue la formalisation minimale nécessaire.

## Conséquences

### Variables

Les tableaux sont distincts des variables simples.

### Mémoire

L'allocation du tableau est dynamique au niveau abstrait.

### Interpréteur

L'accès vérifie la validité du tableau et des indices.

## Traçabilité

HIS-10.6 → EXI-0040 → VM-1201

---

## 12.2 Allocation dynamique du tableau

### VM-1202
Allocation du tableau à la déclaration

## Objet

Définir le moment auquel un tableau acquiert son espace d'exécution.

## Contraintes historiques

La déclaration TABLEAU permet de préciser l'espace à réserver.

LSE 15 indique que la déclaration peut intervenir à différents endroits du programme sous réserve qu'elle ait été exécutée avant l'utilisation.

[INRDP75 §4-1]

[LSE15 §3-1]

## Exigences concernées

### EXI-0041

Titre

Allocation à la déclaration du tableau

Description

Un tableau acquiert son espace d'exécution lorsque sa déclaration est exécutée et demeure disponible jusqu'à sa libération ou la fin du contexte qui le possède.

Origine

HIS-10.6

Sources historiques

[INRDP75 §4-1]

[LSE15 §3-1]

Statut

Documenté

Justification

Les sources décrivent la déclaration du tableau comme déterminant l'espace à réserver et l'utilisation avant déclaration comme une erreur.

Conséquences

L'existence du tableau dépend de son état de déclaration.

Utilisé par

VM-1202

VM-1203

VM-1601

Observations

La forme de l'espace réservé n'est pas documentée.

## Analyse

### Éléments documentés

Un tableau est déclaré.

La déclaration réserve l'espace.

Un tableau peut être libéré.

### Éléments non documentés

Le moment exact de l'allocation physique n'est pas connu.

### Contraintes

Un tableau non déclaré ne doit pas être accessible.

## Reconstruction retenue

La déclaration exécutée crée l'objet tableau et ses cases.

L'objet est alors associé à l'identificateur applicable.

Une nouvelle déclaration du même tableau remplace ou réinitialise l'objet selon les règles documentées du langage.

La libération retire l'objet de l'environnement d'exécution.

## Statut

Déduit

## Justification

La reconstruction transpose au niveau abstrait la notion historique d'espace réservé sans spécifier son emplacement concret.

## Conséquences

### Mémoire

L'allocation et la libération deviennent des opérations de gestion d'objets.

### Interpréteur

L'accès vérifie l'existence de l'objet.

### Erreurs

Un accès avant déclaration produit une erreur d'exécution.

## Traçabilité

HIS-10.6 → EXI-0041 → VM-1202

---

# 13. Procédures et fonctions

Les procédures et fonctions occupent une place importante dans l'architecture historique des langages LSx. Les sources décrivent les paramètres, les variables locales, les retours et les résultats. La machine abstraite doit donc séparer l'objet compilé de la définition d'une procédure de l'état créé lors de son activation.

## 13.1 Définition compilée d'une procédure

### VM-1301
Objet procédure compilé

## Objet

Définir la représentation abstraite d'une procédure ou fonction compilée.

## Contraintes historiques

Les procédures, fonctions et sous-programmes sont décrits dans la famille LSx.

Le manuel LSE 15 définit les déclarations et appels de procédures, les paramètres, les variables locales, RETOUR et RESULTAT.

[NOYELLE89 p.218-220]

[LSE15 §6]

## Exigences concernées

### EXI-0042

Titre

Représentation compilée des procédures et fonctions

Description

Une procédure ou fonction déclarée doit posséder une représentation compilée permettant son activation par un appel et la création du contexte d'exécution correspondant.

Origine

HIS-13.1

HIS-13.2

Sources historiques

[NOYELLE89 p.218-220]

[LSE15 §6]

Statut

Déduit

Justification

Les procédures et fonctions font partie du langage et doivent pouvoir être appelées pendant l'exécution du programme compilé.

Conséquences

La représentation compilée contient ou référence le corps compilé des procédures et fonctions.

Utilisé par

VM-1301

VM-0901

VM-1302

VM-1303

Observations

L'emplacement concret des définitions n'est pas documenté.

## Analyse

### Éléments documentés

Les procédures sont déclarées.

Elles possèdent un corps.

Elles sont appelées.

Elles disposent de paramètres et de variables locales.

### Éléments non documentés

Les sources ne décrivent pas l'organisation interne des définitions.

### Contraintes

L'appel doit pouvoir identifier la procédure cible.

Le corps doit être exécutable.

## Reconstruction retenue

Une définition compilée de procédure contient abstraitement :

- un identificateur ;
- une liste de paramètres ;
- un ensemble de règles de passage des paramètres ;
- un corps compilé ;
- une définition des objets locaux ;
- une règle de retour.

Une fonction possède en plus une règle de production du résultat.

## Statut

Déduit

## Justification

Cette organisation est la plus simple permettant de représenter les propriétés documentées sans imposer de format interne.

## Conséquences

### Compilation

La déclaration crée une définition compilée.

### Interpréteur

L'appel recherche cette définition.

### Cadres d'activation

L'activation reçoit les paramètres et les objets locaux.

## Traçabilité

HIS-13.1 → EXI-0042 → VM-1301

HIS-13.2 → EXI-0042 → VM-1301

---

## 13.2 Appel de procédure

### VM-1302
Activation d'une procédure

## Objet

Définir le passage d'une référence compilée d'appel à une nouvelle activation.

## Contraintes historiques

Les procédures sont appelées explicitement.

Les paramètres sont transmis selon plusieurs modes historiques.

[LSE15 §6]

[NOYELLE89 p.219]

## Exigences concernées

### EXI-0043

Titre

Création d'une activation lors d'un appel

Description

L'exécution d'un appel de procédure ou de fonction crée une activation distincte correspondant à cet appel.

Origine

HIS-13.1

Sources historiques

[LSE15 §6]

[NOYELLE89 p.219]

Statut

Déduit

Justification

Les paramètres et variables locales doivent être associés à l'appel actif.

Conséquences

Un appel entraîne la création d'un cadre d'activation.

Utilisé par

VM-1302

VM-0901

VM-0902

Observations

Les modes précis de passage des paramètres sont traités séparément.

## Analyse

### Éléments documentés

Les procédures possèdent des paramètres et des variables locales.

### Éléments non documentés

Le mécanisme d'appel interne n'est pas décrit.

### Contraintes

Chaque appel doit disposer de son propre contexte.

## Reconstruction retenue

L'appel :

1. identifie la définition ;
2. évalue les arguments nécessaires ;
3. crée un cadre d'activation ;
4. initialise les paramètres ;
5. crée les objets locaux ;
6. établit la position de retour ;
7. transfère l'exécution au corps compilé.

## Statut

Déduit

## Justification

Ce mécanisme est la conséquence directe des notions de paramètres, objets locaux et retour.

## Conséquences

### Cadres d'activation

Un nouveau cadre devient courant.

### Pile d'évaluation

Les arguments sont évalués avant l'activation.

### Interpréteur

Le flux est transféré au corps de la procédure.

## Traçabilité

HIS-13.1 → EXI-0043 → VM-1302

---

## 13.3 Retour d'une procédure ou fonction

### VM-1303
Retour d'activation

## Objet

Définir la terminaison d'une procédure ou fonction et la restitution du contrôle à l'appelant.

## Contraintes historiques

Le manuel LSE 15 décrit RETOUR et RESULTAT.

[LSE15 §6]

## Exigences concernées

### EXI-0044

Titre

Restitution du contrôle après retour

Description

La terminaison d'une procédure ou fonction restitue le contrôle au contexte appelant et, pour une fonction, rend le résultat défini par l'exécution.

Origine

HIS-13.2

Sources historiques

[LSE15 §6]

Statut

Documenté

Justification

Les instructions de retour et de résultat sont explicitement documentées.

Conséquences

Le cadre d'activation courant est terminé.

Utilisé par

VM-1303

VM-0903

VM-1502

Observations

Le mode exact de transport du résultat n'est pas documenté.

## Analyse

### Éléments documentés

Une procédure peut retourner.

Une fonction peut produire un résultat.

### Éléments non documentés

La représentation interne du résultat n'est pas connue.

### Contraintes

Le contexte appelant doit être restauré.

Le résultat doit être disponible si une fonction en produit un.

## Reconstruction retenue

Lors d'un retour :

- le cadre courant est terminé ;
- son cadre appelant devient courant ;
- la position de retour est restaurée ;
- le résultat éventuel est placé dans le contexte d'évaluation de l'appelant.

## Statut

Déduit

## Justification

La reconstruction traduit la sémantique documentée de RETOUR et RESULTAT en une transition abstraite.

## Conséquences

### Cadres

Le cadre courant disparaît.

### Pile

Le résultat peut être empilé.

### Interpréteur

L'exécution reprend au point d'appel.

## Traçabilité

HIS-13.2 → EXI-0044 → VM-1303

---

# 14. Instructions

Les instructions du L.S.E. sont exécutées à partir de leur représentation compilée. Le présent chapitre définit leur place dans la chaîne de traitement sans reprendre leur syntaxe détaillée, qui relève des sources de langage et des chapitres spécialisés.

## 14.1 Instruction compilée

### VM-1401
Objet instruction compilé

## Objet

Définir la représentation abstraite d'une instruction du programme.

## Contraintes historiques

Le compilateur traite une ou plusieurs instructions présentes dans une ligne.

Le résultat est intégré à la chaîne codée.

[INRDP75 §« Présentation d'un programme L.S.E. »]

[INRDP75 §3.3]

## Exigences concernées

### EXI-0045

Titre

Compilation des instructions en opérations exécutables

Description

Chaque instruction acceptée par le compilateur est représentée dans l'objet compilé par une séquence d'opérations permettant son exécution par l'interpréteur.

Origine

HIS-5.2

HIS-7.3

Sources historiques

[INRDP75 §3.3]

[INRDP75 §« Présentation d'un programme L.S.E. »]

Statut

Déduit

Justification

La chaîne codée est directement exécutable et la compilation porte sur les lignes contenant une ou plusieurs instructions.

Conséquences

La représentation compilée ne conserve pas nécessairement les frontières syntaxiques textuelles exactes des instructions.

Utilisé par

VM-1401

VM-0404

VM-0701

Observations

La représentation des instructions composées reste abstraite.

## Analyse

### Éléments documentés

Une ligne peut comporter plusieurs instructions séparées par des points-virgules.

La ligne est compilée en une représentation codée.

### Éléments non documentés

La représentation interne de la séparation entre instructions n'est pas connue.

### Contraintes

L'exécution doit respecter l'ordre des instructions dans la ligne.

## Reconstruction retenue

Une ligne compilée contient une séquence d'opérations correspondant à l'ordre logique des instructions acceptées.

L'ordre d'exécution des instructions d'une même ligne est conservé dans la représentation compilée.

Les séparateurs syntaxiques sont supprimés de la représentation compilée lorsqu'ils ne sont plus nécessaires à l'exécution.

## Statut

Déduit

## Justification

Cette reconstruction est la conséquence minimale du fait qu'une ligne peut contenir plusieurs instructions et que le programme compilé doit les exécuter dans leur ordre.

## Conséquences

### Compilation

Une ligne devient une séquence d'opérations.

### Interpréteur

Il exécute ces opérations dans l'ordre.

### Décompilation

Le décompilateur doit restituer les séparateurs lorsque plusieurs instructions appartiennent à la même ligne.

## Traçabilité

HIS-5.2 → EXI-0045 → VM-1401

HIS-7.3 → EXI-0045 → VM-1401

---

## 14.2 Instruction de contrôle du flux

### VM-1402
Modification du flux d'exécution

## Objet

Définir la représentation abstraite des instructions qui modifient la progression séquentielle.

## Contraintes historiques

Le L.S.E. dispose d'instructions de contrôle permettant les branchements, conditions et boucles.

[INRDP75 §3]

Les programmes du système sont exécutés selon l'ordre des numéros sauf lorsqu'une construction du langage modifie le flux.

## Exigences concernées

### EXI-0046

Titre

Contrôle explicite du flux d'exécution

Description

La représentation compilée doit pouvoir modifier la position logique d'exécution lorsqu'une instruction de contrôle du langage le demande.

Origine

HIS-8.2

HIS-14.1

Sources historiques

[INRDP75 §3]

[INRDP75 §4-3]

Statut

Documenté

Justification

Les instructions de contrôle constituent une propriété du langage documentée dans les sources.

Conséquences

L'état d'exécution doit comporter une position modifiable.

Utilisé par

VM-1402

VM-0603

VM-0604

Observations

La représentation interne des branchements n'est pas documentée.

## Analyse

### Éléments documentés

Le programme n'est pas limité à l'exécution séquentielle.

### Éléments non documentés

Les références internes aux destinations de branchement ne sont pas décrites.

### Contraintes

Un branchement doit conduire à une position valide du programme ou produire une erreur conforme aux règles du langage.

## Reconstruction retenue

Une instruction de contrôle est représentée par une opération codée qui modifie la position d'exécution abstraite.

Une destination de branchement est représentée logiquement, indépendamment d'une adresse mémoire.

Lors de l'exécution, l'interpréteur résout cette destination et modifie la position courante.

## Statut

Déduit

## Justification

Cette représentation est nécessaire pour séparer la sémantique des branchements de l'organisation physique du code.

## Conséquences

### Compilation

Les destinations sont converties en références logiques.

### Interpréteur

La position d'exécution peut être modifiée.

### Décompilation

Le décompilateur doit restituer les références de contrôle sous forme textuelle.

## Traçabilité

HIS-8.2 → EXI-0046 → VM-1402

---

# 15. Expressions

Les expressions constituent une forme privilégiée de la représentation post-fixée. La compilation transforme leur structure syntaxique en une séquence d'opérations exécutables par l'interpréteur.

## 15.1 Expression compilée

### VM-1501
Expression sous forme post-fixée

## Objet

Définir la transformation abstraite d'une expression en séquence post-fixée.

## Contraintes historiques

La famille LSx utilise une représentation post-fixée.

Le L.S.E. manipule des expressions numériques et chaînes.

[NOYELLE89 p.219-224]

[INRDP75 §2]

## Exigences concernées

### EXI-0047

Titre

Compilation post-fixée des expressions

Description

Toute expression compilée est représentée selon un ordre post-fixé dans lequel les opérandes nécessaires à une opération apparaissent avant cette opération.

Origine

CPF-7.1

CPF-7.2

HIS-15.1

Sources historiques

[NOYELLE89 p.219-220]

[INRDP75 §2]

Statut

Probable

Justification

La notation post-fixée est explicitement documentée dans l'histoire du LSD et constitue la reconstruction retenue pour la représentation compilée du L.S.E.

Conséquences

Les expressions peuvent être évaluées par parcours séquentiel et pile d'évaluation.

Utilisé par

VM-1501

VM-1001

VM-0402

Observations

Cette exigence fixe la forme abstraite reconstruite et non un format binaire historique.

## Analyse

### Éléments documentés

La représentation post-fixée est historiquement attestée.

### Éléments non documentés

Le traitement exact de toutes les constructions du L.S.E. n'est pas décrit dans les sources historiques consultées.

### Contraintes

Les relations entre opérandes et opérations doivent être conservées.

## Reconstruction retenue

Le compilateur transforme une expression en arbre syntaxique abstrait puis parcourt cet arbre selon un ordre post-fixé.

Chaque valeur ou référence est produite avant l'opération qui la consomme.

Les parenthèses et autres éléments syntaxiques de groupement ne sont pas conservés lorsqu'ils sont devenus implicites dans l'ordre compilé.

## Statut

Probable

## Justification

La forme post-fixée satisfait directement les objectifs documentés d'interprétation efficace et de décompilation.

Le passage par une structure syntaxique abstraite constitue une étape de reconstruction et ne prétend pas décrire l'algorithme historique réel.

## Conséquences

### Pile d'évaluation

Elle exécute naturellement la séquence obtenue.

### Décompilateur

Il peut reconstituer l'expression en utilisant les relations de priorité.

### Interpréteur

Il traite les opérations dans l'ordre.

## Traçabilité

CPF-7.1 → EXI-0047 → VM-1501

CPF-7.2 → EXI-0047 → VM-1501

---

## 15.2 Résultat d'une expression

### VM-1502
Valeur résultante

## Objet

Définir le résultat abstrait d'une expression évaluée.

## Contraintes historiques

Les expressions peuvent être évaluées en mode machine de bureau et leur valeur affichée.

[INRDP75 §Mode machine de bureau]

[LSE15 §9-5]

## Exigences concernées

### EXI-0048

Titre

Production d'une valeur par l'évaluation

Description

L'évaluation d'une expression valide produit une valeur conforme au type et à la sémantique de l'expression.

Origine

HIS-8.1

HIS-15.1

Sources historiques

[INRDP75 §Mode machine de bureau]

[LSE15 §9-5]

Statut

Documenté

Justification

Le système évalue des expressions et peut afficher leur valeur.

Conséquences

La pile d'évaluation doit pouvoir produire une valeur finale.

Utilisé par

VM-1502

VM-1001

VM-0702

Observations

La forme interne des valeurs est définie dans les chapitres spécialisés.

## Analyse

### Éléments documentés

Une expression possède une valeur.

Cette valeur peut être affichée.

### Éléments non documentés

La représentation interne des valeurs est variable selon leur type.

### Contraintes

Une expression correctement évaluée doit produire une valeur ou un effet défini.

## Reconstruction retenue

Après exécution complète d'une expression, la pile d'évaluation contient une valeur finale correspondant à son résultat.

L'interpréteur peut :

- retourner cette valeur au contexte appelant ;
- l'affecter à une variable ;
- la transmettre à une instruction ;
- l'afficher dans le mode machine de bureau.

## Statut

Déduit

## Justification

La reconstruction formalise la notion de valeur d'expression sans imposer de représentation matérielle.

## Conséquences

### Expressions

Les expressions ont une sémantique de valeur.

### Affectation

Le résultat peut alimenter une variable.

### Affichage

Le mode machine de bureau peut présenter ce résultat.

## Traçabilité

HIS-8.1 → EXI-0048 → VM-1502

HIS-15.1 → EXI-0048 → VM-1502

---

# 16. Gestion mémoire

Les sources historiques décrivent des contraintes fortes de mémoire, de disque et de temps de réponse. Elles mentionnent notamment une zone données-utilisateur, une zone de swap et des techniques d'échange et de recouvrement dynamique. Ces descriptions appartiennent aux implantations historiques et ne permettent pas d'en déduire une carte mémoire universelle.

La machine abstraite doit néanmoins définir une mémoire de travail logique capable de représenter le programme courant, les objets d'exécution et l'état dynamique.

## 16.1 Mémoire abstraite

### VM-1601
Mémoire de travail

## Objet

Définir l'espace abstrait dans lequel sont représentés les objets nécessaires à l'exécution.

## Contraintes historiques

Les systèmes historiques disposent d'une zone données-utilisateur.

Ils utilisent une zone de swap.

Les objets et programmes sont soumis à des contraintes de mémoire.

[INRDP75 §3.1]

[INRDP75 §3.2]

[NOYELLE89 p.225-227]

## Exigences concernées

### EXI-0049

Titre

Séparation du stockage logique et de la mémoire d'exécution

Description

La machine abstraite doit distinguer les représentations persistantes des programmes et données de l'état temporaire nécessaire à leur exécution.

Origine

HIS-3.14

HIS-11.4

Sources historiques

[INRDP75 §1-2]

[INRDP75 §3.1]

[NOYELLE89 p.225-227]

Statut

Déduit

Justification

Les sources distinguent les programmes conservés dans la bibliothèque, la zone de swap et la zone données-utilisateur.

Conséquences

Le programme courant, les objets d'exécution et les fichiers persistants ne sont pas un seul espace abstrait.

Utilisé par

VM-1601

VM-1701

VM-2102

Observations

La géographie physique des espaces historiques n'est pas reconstruite.

## Analyse

### Éléments documentés

Le système distingue plusieurs zones.

La zone de swap conserve l'état des tâches.

La mémoire utilisateur contient l'état nécessaire à l'exécution.

### Éléments non documentés

La carte mémoire exacte n'est pas connue.

### Contraintes

La machine abstraite doit permettre :

- la conservation du programme compilé ;
- la création d'objets d'exécution ;
- la suspension et la reprise ;
- la conservation persistante séparée.

## Reconstruction retenue

La machine abstraite distingue :

1. le stockage persistant des programmes ;
2. le programme courant ;
3. la mémoire de travail de l'exécution ;
4. l'état suspendu d'une tâche.

Cette distinction est logique et ne représente pas nécessairement quatre zones matérielles séparées.

## Statut

Déduit

## Justification

La reconstruction généralise les distinctions fonctionnelles observées dans les sources tout en évitant d'attribuer une structure matérielle universelle au système.

## Conséquences

### Mémoire

Les mécanismes de durée de vie peuvent être définis séparément.

### Temps partagé

L'état d'une tâche peut être suspendu.

### Bibliothèque

Les fichiers persistants sont indépendants de la mémoire d'exécution.

## Traçabilité

HIS-3.14 → EXI-0049 → VM-1601

HIS-11.4 → EXI-0049 → VM-1601

---

## 16.2 Allocation des objets

### VM-1602
Allocation dynamique des objets

## Objet

Définir la création et la libération abstraites des objets d'exécution.

## Contraintes historiques

Les tableaux peuvent être déclarés et libérés dynamiquement.

La mémoire est limitée.

[INRDP75 §4-1]

[LSE15 §3-1]

[NOYELLE89 p.219]

## Exigences concernées

### EXI-0050

Titre

Allocation et libération des objets d'exécution

Description

Les objets d'exécution dont la durée de vie est dynamique doivent pouvoir être créés et libérés sans imposer une allocation permanente pendant toute la durée de la session.

Origine

HIS-10.6

Sources historiques

[INRDP75 §4-1]

[LSE15 §3-1]

[NOYELLE89 p.219]

Statut

Documenté

Justification

Le langage permet explicitement de libérer des tableaux afin de récupérer de l'espace.

Conséquences

La mémoire de travail doit gérer des objets dont la durée de vie est variable.

Utilisé par

VM-1602

VM-1202

VM-1203

VM-0901

Observations

L'algorithme d'allocation historique n'est pas documenté.

## Analyse

### Éléments documentés

Des objets peuvent être créés.

Des objets peuvent être libérés.

La libération est utile pour réutiliser l'espace.

### Éléments non documentés

Les algorithmes d'allocation et de libération ne sont pas décrits.

### Contraintes

Un objet libéré ne doit plus être accessible comme objet actif.

L'espace abstrait qu'il occupait doit pouvoir être réutilisé.

## Reconstruction retenue

La mémoire de travail fournit une allocation abstraite d'objets.

Chaque objet possède une durée de vie.

Lorsqu'un objet est libéré :

- il cesse d'être accessible ;
- les références actives qui le désignent deviennent invalides selon les règles du langage ;
- ses ressources peuvent être réutilisées.

Aucun mécanisme automatique de ramasse-miettes n'est introduit.

## Statut

Choix de conception

## Justification

Les sources documentent la libération explicite d'objets et les contraintes de mémoire.

Un mécanisme automatique de gestion mémoire n'est pas nécessaire à la satisfaction de ces contraintes et constituerait une hypothèse supplémentaire.

## Conséquences

### Tableaux

La libération est explicite.

### Procédures

Les objets locaux peuvent être libérés à la fin d'une activation.

### Mémoire

La réutilisation des ressources est possible.

## Traçabilité

HIS-10.6 → EXI-0050 → VM-1602

---

# 17. Bibliothèque des programmes

La bibliothèque conserve les programmes sous forme codée. Le bibliothécaire est historiquement un processeur distinct chargé de la gestion des fichiers utilisateurs.

## 17.1 Fichier-programme compilé

### VM-1701
Fichier-programme

## Objet

Définir la représentation abstraite d'un programme conservé de manière persistante.

## Contraintes historiques

RANGER conserve le programme courant sous forme codée.

APPELER recharge le programme.

[INRDP75 §4-4]

[LSE15 §9-5]

## Exigences concernées

EXI-0009

EXI-0022

EXI-0049

## Analyse

### Éléments documentés

Le fichier-programme conserve une forme codée.

### Éléments non documentés

Le format exact n'est pas connu.

### Contraintes

Le fichier doit pouvoir être chargé comme programme courant.

## Reconstruction retenue

Un fichier-programme contient une représentation persistante du programme compilé.

Cette représentation est équivalente, au niveau abstrait, à une collection d'objets compilés ordonnée logiquement.

Le fichier ne contient pas nécessairement le texte source.

## Statut

Déduit

## Justification

La reconstruction traduit la propriété historique de conservation sous forme codée en un objet persistant indépendant du stockage physique.

## Conséquences

### Bibliothécaire

Il gère la persistance et le chargement.

### Compilation

Le chargement n'exige pas de recompilation.

### Décompilation

Le programme reste décompilable après chargement.

## Traçabilité

HIS-11.2 → EXI-0009, EXI-0022 → VM-1701

---

## 17.2 Chargement d'un fichier-programme

### VM-1702
Restauration d'un programme compilé

## Objet

Définir l'opération abstraite de restauration du programme courant à partir d'un fichier-programme.

## Contraintes historiques

APPELER charge le programme contenu dans un fichier-programme et en fait le programme courant.

[INRDP75 §4-4]

[LSE15 §9-5]

## Exigences concernées

EXI-0025

## Analyse

### Éléments documentés

Le chargement établit un nouveau programme courant.

### Éléments non documentés

La validation interne du fichier n'est pas décrite.

### Contraintes

Le programme restauré doit être cohérent.

## Reconstruction retenue

Le bibliothécaire lit la représentation persistante.

La machine vérifie que la représentation peut être reconnue comme programme compilé valide.

Elle reconstruit le programme courant.

Une erreur de chargement empêche l'établissement d'un nouveau programme courant valide.

## Statut

Déduit

## Justification

Une validation minimale est nécessaire pour éviter qu'un fichier invalide ne devienne un programme courant incohérent.

## Conséquences

### Bibliothèque

Le bibliothécaire doit signaler les fichiers non chargeables.

### Programme courant

L'ancien état est préservé en cas d'échec de chargement.

### Interpréteur

Seul un programme courant valide peut être exécuté.

## Traçabilité

HIS-11.5 → EXI-0025 → VM-1702

---


## 17.3 Fichiers-données temporaires

### VM-1703
Fichier-données temporaire

## Objet

Définir le fichier-données associé à l'espace temporaire d'une session.

## Contraintes historiques

Les fichiers-données temporaires sont associés à une console et sont
détruits à la fin de la session. Une console dispose d'un espace
temporaire qui lui est propre.

[INRDP75 §5-1.1]

[INRDP75 §5-1.2]

[LSE15 §9-5]

## Exigences concernées

EXI-0053

EXI-0054

EXI-0056

## Reconstruction retenue

Un fichier-données temporaire possède :

- un nom logique ;
- une portée attachée à la console ;
- un contenu de données ;
- une durée de vie limitée à la session.

Sa destruction libère les ressources logiques qu'il occupe dans l'espace
temporaire.

## Statut

Documenté

## Traçabilité

HIS-11.3, HIS-4.4.3 → EXI-0053, EXI-0054, EXI-0056 → VM-1703

---

## 17.4 Fichiers-données permanents

### VM-1704
Fichier-données permanent

## Objet

Définir le fichier-données conservé au-delà de la session.

## Contraintes historiques

Les fichiers-données permanents appartiennent à la zone des fichiers
permanents et sont conservés au-delà de la session.

[INRDP75 §5-1.1]

[INRDP75 §5-1.2]

[LSE15 §9-5]

## Exigences concernées

EXI-0053

EXI-0054

## Reconstruction retenue

Un fichier-données permanent possède :

- un nom logique ;
- une identité dans la bibliothèque ;
- un contenu de données ;
- une durée de vie indépendante de la session.

Il est distinct d'un fichier-données temporaire, même si les deux
contiennent des données de même nature.

## Statut

Documenté

## Traçabilité

HIS-11.3, HIS-4.4.4 → EXI-0053, EXI-0054 → VM-1704

---

## 17.5 Catalogue des fichiers

### VM-1705
Catalogue des fichiers

## Objet

Définir le mécanisme abstrait permettant d'identifier les fichiers de
la bibliothèque.

## Contraintes historiques

Les opérations sur les fichiers s'appuient sur leur identification
et sur le catalogue de la bibliothèque.

[INRDP75 §Fichiers]

[LSE15 §9-5]

## Exigences concernées

EXI-0055

## Reconstruction retenue

Le catalogue associe une identité logique de fichier à sa représentation
persistante et à sa catégorie.

Il permet au bibliothécaire de rechercher un fichier et de déterminer
la ressource correspondant à une commande.

La structure interne du catalogue n'est pas imposée.

## Statut

Documenté

## Traçabilité

HIS-11.1, HIS-11.3 → EXI-0055 → VM-1705

---

## 17.6 Opérations sur les fichiers-données

### VM-1706
Gestion abstraite des fichiers-données

## Objet

Définir les opérations fonctionnelles du bibliothécaire portant sur les
fichiers-données.

## Contraintes historiques

Le bibliothécaire est appelé par les commandes et instructions portant
sur les fichiers-programmes ou les fichiers-données.

[INRDP75 §3.3]

[INRDP75 §5]

Les commandes ENCODER, DECODER, CATALOGUER et SUPPRIMER utilisent les fichiers
de données selon les enchaînements documentés.

[INRDP75 §5-1.2]

[LSE15 §9-5]

## Exigences concernées

EXI-0057

## Reconstruction retenue

Le bibliothécaire fournit les opérations abstraites suivantes :

- créer ou identifier un fichier-données ;
- lire un fichier-données ;
- écrire un fichier-données ;
- cataloguer un fichier temporaire ;
- supprimer un fichier selon sa catégorie ;
- transmettre une donnée à DECODER ;
- transmettre une donnée à ENCODER.

Les instructions de programme portant sur les fichiers-données sont
rattachées à ces opérations :

- GARER transfère une valeur dans l'enregistrement de numéro demandé ;
- CHARGER restitue le contenu de l'enregistrement demandé dans un objet
  d'exécution ;
- SUPPRIMER supprime l'enregistrement demandé ou, lorsque la désignation
  d'enregistrement est absente, le fichier concerné.

Un enregistrement constitue un sous-ensemble logique d'informations d'un
fichier. Il contient une constante, un tableau à une ou deux dimensions
ou une chaîne ; les informations de types différents ne sont pas
mélangées dans un même enregistrement.

GARER remplace le contenu d'un enregistrement existant de même numéro.
CHARGER restitue le type et le contenu correspondant à l'enregistrement.
Les commandes de gestion des fichiers portent, quant à elles, sur le
fichier dans son ensemble.

[INRDP75 §2-3]
[INRDP75 §2-3.1]
[INRDP75 §2-3.2]

Le détail du stockage reste hors du modèle abstrait.

## Statut

Déduit

## Traçabilité

HIS-4.5.4, HIS-11.1, HIS-11.3, HIS-11.5 → EXI-0057 → VM-1706

---

## 17.7 Transformation ENCODER / DECODER

### VM-1707
Transformation entre programme et fichier-données

## Objet

Définir les relations fonctionnelles entre le programme courant et les
fichiers-données dans les commandes ENCODER et DECODER.

## Contraintes historiques

DECODER transforme le programme courant en une donnée stockée dans un
fichier temporaire.

ENCODER transforme une telle donnée en nouveau programme courant.

[INRDP75 §5-1.2]

## Exigences concernées

EXI-0057

## Reconstruction retenue

DECODER réalise :

`programme courant → représentation de données → fichier-données`

ENCODER réalise :

`fichier-données → représentation de programme → programme courant`

Une erreur de transformation ne doit pas publier un programme courant
invalide.

## Statut

Documenté

## Traçabilité

HIS-11.5 → EXI-0057 → VM-1707

---

## 17.8 Catalogage d'un fichier-données temporaire

### VM-1708
Catalogage d'un fichier-données

## Objet

Définir la transformation d'un fichier-données temporaire en fichier
permanent.

## Contraintes historiques

CATALOGUER rend permanent un fichier-données temporaire et libère
l'espace temporaire correspondant.

[LSE15 §9-5]

## Exigences concernées

EXI-0056

## Reconstruction retenue

Le catalogage :

1. identifie le fichier temporaire ;
2. crée sa représentation permanente ;
3. publie cette représentation dans la bibliothèque ;
4. supprime l'identité temporaire ;
5. rend son espace temporaire réutilisable.

Une opération échouée ne doit pas produire un fichier permanent
partiellement constitué.

## Statut

Documenté

## Traçabilité

HIS-11.3 → EXI-0056 → VM-1708


## 17.9 Gestion abstraite des identifications et des droits d'accès

### VM-1709
Gestion abstraite des identifications et des droits d'accès aux fichiers

## Objet

Définir le mécanisme abstrait permettant de représenter l'identification courante d'une session et de contrôler les opérations autorisées sur les fichiers.

## Contraintes historiques

La commande IDENTIFICATION permet de présenter un numéro de compte et une clé que le système vérifie. Une seule identification est valide à un instant donné ; le dernier numéro indiqué devient l'identification courante. Un fichier permanent créé sous une identification ne peut être modifié ou supprimé que sous cette même identification. Les droits associés à une identification déterminent notamment les opérations possibles sur les fichiers-programmes et les fichiers-données permanents. [INRDP75 §Fichiers]

Les fichiers temporaires restent attachés au contexte de session et leur utilisation ne dépend pas d'une identification permanente. Le changement d'identification détruit les fichiers temporaires créés par l'utilisateur. [INRDP75 §Fichiers]

Les sources documentent en outre des protections de lecture et d'exécution différentes selon les fichiers et selon l'implantation. [INRDP75 §Fichiers]

## Exigences concernées

EXI-0053

EXI-0054

EXI-0055

EXI-0057

EXI-0062

EXI-0061 pour les différences propres à l'implantation.

## Reconstruction retenue

La machine abstraite maintient, pour chaque session :

- une identification courante ou l'absence d'identification ;
- les attributs de droits associés à cette identification ;
- le contexte de session auquel appartiennent les fichiers temporaires.

Chaque fichier permanent conserve logiquement :

- l'identification sous laquelle il a été créé ;
- sa catégorie ;
- les protections applicables ;
- les droits nécessaires aux opérations documentées.

Toute opération sur un fichier permanent passe par un contrôle d'autorisation adapté à l'opération. La lecture et l'écriture ne sont pas assimilées : l'accès en lecture peut être ouvert alors que la création, la modification ou la suppression restent soumises aux pouvoirs de l'identification.

Le contrôle d'accès doit appliquer les différences historiquement documentées par le profil T1600 ou MITRA 15 sans généraliser une protection propre à une implantation.

## Opérations abstraites

`VALIDER_IDENTIFICATION(compte, clé)`

`FIXER_IDENTIFICATION_COURANTE(identification)`

`LIRE_IDENTIFICATION_COURANTE(session)`

`VERIFIER_AUTORISATION(identification, fichier, opération)`

## Invariants

- une session possède au plus une identification courante ;
- une identification refusée ne modifie pas l'identification courante ;
- un fichier permanent ne peut être modifié ou supprimé sous une identification différente de celle de création, sauf règle historique explicitement documentée ;
- l'absence d'identification interdit les écritures permanentes ;
- une opération refusée ne modifie pas le fichier ;
- une clé d'identification n'est pas une donnée affichable par les opérations ordinaires de consultation.

## Statut

Déduit.

## Traçabilité

HIS-11.3.1 → EXI-0062 → VM-1709, VM-3404, VM-3405, VM-3408


# 18. Décompilation

Le décompilateur est l'un des processeurs explicitement distingués par le manuel de 1975. Son rôle est inverse de celui du compilateur au niveau fonctionnel : il transforme une représentation codée en caractères. Il intervient pour LISTER, PERFORER et DECODER selon des chaînes de traitement différentes.

## 18.1 Décompilation du programme courant

### VM-1801
Décompilation du programme

## Objet

Définir la transformation abstraite du programme compilé en représentation textuelle.

## Contraintes historiques

Le décompilateur transforme la chaîne codée en caractères.

LISTER permet le listage.

DECODER permet de ranger la représentation textuelle dans un fichier données.

[INRDP75 §3.3]

[INRDP75 §4-2]

[INRDP75 Annexe II]

## Exigences concernées

EXI-0003

EXI-0015

EXI-0022

## Analyse

### Éléments documentés

Le programme peut être décompilé.

Le résultat peut être affiché ou stocké.

### Éléments non documentés

Le processus exact n'est pas décrit.

### Contraintes

La décompilation doit produire une représentation textuelle cohérente avec le programme.

## Reconstruction retenue

La décompilation parcourt les objets compilés du programme dans leur ordre logique.

Pour chaque ligne :

1. le numéro de ligne est restitué ;
2. la représentation compilée est transformée en construction textuelle ;
3. les instructions d'une même ligne sont séparées selon la syntaxe correspondante ;
4. la ligne résultante est produite comme texte.

Le résultat global est une séquence de lignes.

## Statut

Déduit

## Justification

Cette procédure constitue la reconstruction minimale du fonctionnement du listage à partir d'un programme dont le texte source n'est pas conservé séparément.

## Conséquences

### LISTER

Le résultat est destiné à l'affichage.

### DECODER

Le résultat peut être écrit dans un fichier données.

### PERFORER

Le même flux textuel peut être transmis à un mécanisme de sortie approprié.

## Traçabilité

HIS-5.3 → EXI-0003, EXI-0015 → VM-1801

---

# 19. Gestion des erreurs

Le système historique accorde une place centrale à la détection immédiate des erreurs syntaxiques et à la détection des erreurs d'exécution. Le mécanisme d'erreur doit donc être séparé de la représentation compilée tout en restant accessible dans l'environnement conversationnel.

## 19.1 Diagnostic de compilation

### VM-1901
Erreur de compilation

## Objet

Définir le résultat abstrait d'une compilation qui ne peut pas produire un objet compilé valide.

## Contraintes historiques

Les erreurs syntaxiques sont signalées immédiatement.

Une ligne incorrecte doit être corrigée puis refrappée.

[INRDP75 §« Présentation d'un programme L.S.E. »]

[LSE15 §1]

## Exigences concernées

### EXI-0051

Titre

Diagnostic immédiat des erreurs syntaxiques

Description

Lorsqu'une ligne validée présente une erreur syntaxique détectable par le compilateur, un diagnostic est produit avant son intégration au programme courant.

Origine

HIS-5.2

HIS-6.5

HIS-12.1

Sources historiques

[INRDP75 §3.3]

[LSE15 §1]

Statut

Documenté

Justification

Les sources indiquent que l'erreur syntaxique est signalée dès l'analyse de la ligne.

Conséquences

Le diagnostic appartient au traitement de compilation.

Utilisé par

VM-1901

VM-0302

VM-0502

Observations

Le format exact des messages n'est pas reconstruit.

## Analyse

### Éléments documentés

L'erreur est immédiate.

La ligne est rejetée.

L'utilisateur doit la corriger.

### Éléments non documentés

Les sources ne permettent pas de déterminer la structure interne d'un diagnostic.

### Contraintes

Le diagnostic doit identifier l'échec de compilation.

Le programme courant doit rester cohérent.

## Reconstruction retenue

Le diagnostic de compilation est un objet abstrait contenant au minimum :

- une indication d'erreur ;
- une position logique ou un contexte permettant de situer la ligne ;
- une description destinée à l'utilisateur.

Le diagnostic est transmis à l'environnement conversationnel.

## Statut

Déduit

## Justification

La nature exacte du message n'est pas documentée, mais la production d'un diagnostic est explicitement attestée.

## Conséquences

### Dialogue

L'erreur est visible immédiatement.

### Programme courant

L'état précédent reste inchangé.

### Compilation

Une compilation échouée ne produit aucun objet accepté.

## Traçabilité

HIS-5.2 → EXI-0051 → VM-1901

HIS-6.5 → EXI-0051 → VM-1901

HIS-12.1 → EXI-0051 → VM-1901

---

# 20. Dialogue avec le système

L'environnement conversationnel constitue un niveau de fonctionnement transversal. La machine abstraite doit permettre d'enchaîner entrée, compilation, exécution, diagnostic et mise au point sans séparer artificiellement ces activités.

## 20.1 Cycle conversationnel d'une ligne

### VM-2001
Cycle de traitement conversationnel

## Objet

Définir le cycle abstrait de traitement d'une entrée utilisateur.

## Contraintes historiques

Le système lit une ligne.

Il l'identifie.

Il peut appeler le compilateur, l'interpréteur, le décompilateur ou le bibliothécaire selon la nature de la demande.

[INRDP75 §3.3]

Le système est utilisé en mode conversationnel.

[INRDP75 §3]

[LSE15 §1]

## Exigences concernées

### EXI-0052

Titre

Traitement conversationnel des demandes

Description

Toute entrée utilisateur validée doit être orientée vers le traitement approprié selon sa nature : commande, ligne de programme, expression immédiate ou demande adressée à un processeur.

Origine

HIS-3.1

HIS-3.11

HIS-4.5.6

Sources historiques

[INRDP75 §3]

[INRDP75 §3.3]

[LSE15 §1]

Statut

Documenté

Justification

Le manuel décrit l'analyse de la ligne et l'enchaînement vers le processeur compétent.

Conséquences

La machine abstraite possède un point d'entrée conversationnel capable d'orienter les demandes.

Utilisé par

VM-2001

VM-2002

VM-0501

VM-0702

VM-1701

Observations

Le mécanisme d'analyseur de commandes est traité comme un mécanisme distinct.

## Analyse

### Éléments documentés

Une ligne peut être une commande.

Elle peut être une ligne de programme.

Elle peut être une expression immédiate.

Le processeur compétent est mobilisé.

### Éléments non documentés

Les règles internes de classification ne sont pas entièrement décrites.

### Contraintes

Une entrée doit être traitée selon sa nature.

Une ligne de programme doit être compilée.

Une expression immédiate doit être exécutée.

Une commande doit être traitée par le langage de commande ou le processeur approprié.

## Reconstruction retenue

Le cycle conversationnel est défini comme suit :

1. réception des caractères ;
2. validation par la fin de ligne ;
3. classification abstraite de la demande ;
4. orientation vers le mécanisme approprié ;
5. production d'un résultat ou d'un diagnostic ;
6. retour à l'état conversationnel.

Cette séquence définit le comportement abstrait, sans imposer une architecture de processus particulière.

## Statut

Déduit

## Justification

Le schéma historique d'enchaînement des processeurs permet de reconstruire ce cycle fonctionnel.

## Conséquences

### Analyseur de commandes

Il détermine la catégorie de traitement.

### Compilateur

Il reçoit les lignes de programme.

### Interpréteur

Il reçoit les expressions et programmes exécutables.

### Bibliothécaire

Il reçoit les opérations de fichiers.

### Décompilateur

Il reçoit les opérations de restitution textuelle.

## Traçabilité

HIS-3.1 → EXI-0052 → VM-2001

HIS-3.11 → EXI-0052 → VM-2001

HIS-4.5.6 → EXI-0052 → VM-2001

---


## 20.2.1 Routage historique des commandes

### VM-2001 — extension du cycle conversationnel

Le cycle conversationnel doit conserver la distinction historique entre :

- commandes à action immédiate ;
- demandes de décompilation ;
- demandes d'exécution ;
- opérations de fichiers ;
- lignes de programme ;
- expressions du mode machine de bureau.

La reconnaissance d'une commande doit tenir compte du mode d'entrée documenté : deux premières lettres suivies du caractère de fin de ligne, puis complétion du nom et réception éventuelle des paramètres, sauf lorsque ABREGER supprime la complétion affichée. [INRDP75 §4]

Le routage fonctionnel minimal reconstruit est :

```text
commande à action immédiate → traitement du moniteur
commande de listage/restitution → décompilateur
commande d'exécution/poursuite → interpréteur
commande de fichier → bibliothécaire et/ou transformation nécessaire
ligne de programme → compilateur
expression immédiate → compilateur puis interpréteur
```

DECODER et ENCODER constituent des chaînes de traitement combinant plusieurs fonctions ; elles ne doivent pas être réduites à un appel unique à un processeur. [INRDP75 §3.3 ; Annexe II]

**Statut : Déduit.**

**Traçabilité :** HIS-5.6, HIS-4.7.1 → EXI-0052 → VM-2001.

## 20.2.2 Mode PAS À PAS

Le mode PAS À PAS est une propriété du contrôle de l'exécution et non une simple variante de l'interprétation d'une instruction. Il provoque un arrêt au début de chaque ligne, rend le contrôle au moniteur et permet à l'utilisateur d'intervenir avant la poursuite. [INRDP75 §4-1 ; Annexe II ; LSE15 §9-3]

La machine abstraite doit donc conserver :

- l'état d'exécution ;
- la position logique de la ligne à exécuter ;
- l'état du moniteur ;
- les valeurs nécessaires à la poursuite.

`CONTINUER` reprend au point d'arrêt ; `REPRENDRE` peut choisir une autre ligne dans les limites documentées, sans réinitialiser les affectations conservées. `NORMAL` désactive le mode PAS À PAS. [INRDP75 §4-3 ; LSE15 §9-3]

**Statut : Déduit.**

Le comportement est documenté ; la représentation abstraite de l'état est déduite.

**Traçabilité :** HIS-13.1, HIS-13.3, HIS-13.4 → EXI-0060 → mécanismes d'exécution et de dialogue.

## 20.2 Commandes portant sur les fichiers-données

### VM-2002
Traitement des commandes de fichiers-données

## Objet

Définir l'orientation des commandes de gestion des fichiers-données.

## Exigences concernées

EXI-0055

EXI-0057

## Reconstruction retenue

Le langage de commande reconnaît les opérations documentées portant sur
les fichiers-données et les transmet au bibliothécaire.

Le traitement distingue notamment :

- l'identification d'un fichier ;
- sa lecture ou son écriture ;
- son catalogage ;
- sa suppression ;
- les transformations ENCODER et DECODER.

Le langage de commande ne réalise pas lui-même les opérations de
stockage.

## Statut

Déduit

## Traçabilité

HIS-4.5.5, HIS-11.1, HIS-11.3, HIS-11.5 → EXI-0055, EXI-0057 → VM-2002


## 20.3 Variantes d'implantation historiques

La reconstruction fonctionnelle doit permettre de distinguer les deux implantations historiques documentées dans le périmètre : **version 4 sur T1600** et **version 8 sur MITRA 15**. [INRDP75 §« Remarque sur la présentation du fascicule »]

Les propriétés communes doivent être portées par la machine abstraite commune. Les différences explicitement documentées doivent être représentées comme des variantes de profil et non comme deux machines abstraites indépendantes.

Exemples documentés :

- RUBAN existe sur MITRA 15 ; sur T1600, la lecture du ruban ne nécessite pas cette commande ;
- SILENCE est documentée pour MITRA 15 ;
- ENCODER n'a pas le même effet sur le programme courant selon MITRA 15 et T1600.

Aucune différence non documentée ne doit être ajoutée à un profil. [INRDP75 §4.1 ; Annexe II]

**Statut : Choix de conception.**

**Traçabilité :** INRDP75 « Remarque sur la présentation du fascicule », §4.1, Annexe II → contrainte de réalisation de la machine abstraite.


# 21. Modèle d'exécution

Le modèle d'exécution rassemble les mécanismes définis précédemment dans une séquence cohérente. Il ne constitue pas un nouveau composant mais une définition du comportement global de la machine abstraite.

## 21.1 État global de la machine abstraite

### VM-2101
État global d'exécution

## Objet

Définir les catégories d'état nécessaires au fonctionnement global de la machine abstraite.

## Contraintes historiques

Les sources distinguent :

- le programme courant ;
- l'état de la console ;
- l'exécution ;
- les tâches ;
- les fichiers ;
- les processeurs.

[INRDP75 §3.1]

[INRDP75 §3.2]

[INRDP75 §3.3]

## Exigences concernées

EXI-0049

EXI-0052

EXI-0029

## Analyse

### Éléments documentés

Le système distingue plusieurs états fonctionnels.

### Éléments non documentés

Aucune représentation abstraite globale n'est fournie par les sources.

### Contraintes

La machine abstraite doit pouvoir représenter simultanément :

- le travail courant ;
- l'état du programme ;
- l'exécution éventuelle ;
- le dialogue.

## Reconstruction retenue

L'état global d'une session comprend :

1. un programme courant ;
2. un environnement d'exécution, éventuellement actif ;
3. un état conversationnel ;
4. des références vers les ressources de bibliothèque accessibles ;
5. les informations nécessaires à une exécution suspendue.

L'état global du système comprend également l'ensemble des sessions ou tâches actives lorsque le temps partagé est modélisé.

## Statut

Déduit

## Justification

La séparation fonctionnelle des éléments est directement imposée par les descriptions historiques.

La structure abstraite retenue permet d'expliquer les transitions entre programmation, exécution et mise au point.

## Conséquences

### Interpréteur

Il agit sur une session déterminée.

### Programme courant

Il est attaché à cette session.

### Temps partagé

Chaque tâche peut posséder un état suspendu.

### Bibliothèque

Elle est partagée selon les règles d'accès du système.

## Traçabilité

HIS-3.1 → EXI-0052 → VM-2101

HIS-3.14 → EXI-0049 → VM-2101

---

## 21.2 Transition vers l'exécution

### VM-2102
Création d'une exécution

## Objet

Définir la transition entre un programme courant stable et une exécution active.

## Contraintes historiques

EXECUTER provoque le chargement de l'interpréteur et l'exécution du programme.

[INRDP75 §3.3]

[INRDP75 §4-3]

## Exigences concernées

EXI-0029

EXI-0031

EXI-0037

## Analyse

### Éléments documentés

L'exécution utilise le programme courant.

### Éléments non documentés

La création des structures internes n'est pas décrite.

### Contraintes

L'exécution doit commencer dans un état cohérent.

## Reconstruction retenue

La transition crée :

- un état d'exécution ;
- une position initiale ;
- un cadre d'activation racine ;
- une pile d'évaluation vide ;
- les références vers le programme courant.

La représentation compilée du programme courant n'est pas modifiée par cette transition.

## Statut

Déduit

## Justification

Cette transition est nécessaire pour distinguer l'état stable du programme de l'état dynamique de son exécution.

## Conséquences

### Programme courant

Il reste disponible.

### Interpréteur

Il reçoit une exécution nouvelle.

### Mémoire

Les ressources dynamiques sont allouées.

## Traçabilité

HIS-8.2 → EXI-0029, EXI-0031 → VM-2102

---

## 21.3 Interruption et reprise

### VM-2103
Suspension et reprise de l'exécution

## Objet

Définir la suspension temporaire d'une exécution et sa reprise.

## Contraintes historiques

L'exécution peut être interrompue.

Elle peut être reprise.

L'état du programme peut être examiné et modifié.

[NOYELLE89 p.221]

[INRDP75 §4.3]

[LSE15 §9-3-5]

## Exigences concernées

EXI-0024

EXI-0031

EXI-0033

## Analyse

### Éléments documentés

Une exécution peut être suspendue.

La position et l'état peuvent être conservés.

La reprise est possible.

### Éléments non documentés

Le détail de la sauvegarde n'est pas décrit.

### Contraintes

Une suspension ne doit pas détruire l'état d'exécution.

La reprise doit continuer à partir de l'état conservé.

## Reconstruction retenue

La suspension transforme une exécution active en exécution suspendue.

L'état conservé comprend :

- la position d'exécution ;
- les cadres d'activation ;
- la pile d'évaluation ;
- l'environnement d'exécution ;
- les informations nécessaires à la poursuite.

La reprise restaure cet état et réactive l'interpréteur.

Le programme courant peut être observé pendant la suspension.

Toute modification du programme courant est distincte de la restauration de l'état suspendu.

## Statut

Déduit

## Justification

La reconstruction formalise la capacité documentée de mise au point tout en maintenant la séparation entre programme et exécution.

## Conséquences

### Temps partagé

Une tâche suspendue peut être inactive sans perdre son état.

### Mémoire

L'état peut être déplacé vers une zone de conservation.

### Mise au point

L'utilisateur peut interagir avec l'état suspendu.

## Traçabilité

HIS-3.13 → EXI-0024, EXI-0031, EXI-0033 → VM-2103

HIS-8.3 → EXI-0024 → VM-2103

---


## 21.4 Temps partagé et conservation des tâches

### VM-2105
État d'une tâche en temps partagé

## Objet

Définir la représentation abstraite d'une tâche utilisateur et de son
état lorsqu'elle n'est pas active.

## Contraintes historiques

Le système résident et le moniteur de temps partagé assurent l'activation
successive des tâches et la conservation de leurs images dans la zone
de swap.

[INRDP75 §3.1]

[INRDP75 §5-1.2]

## Exigences concernées

EXI-0058

EXI-0059

## Reconstruction retenue

Une tâche possède un état comprenant notamment :

- son état de session ;
- son programme courant ;
- son état d'exécution éventuel ;
- les ressources nécessaires à sa reprise.

Lorsqu'une tâche devient inactive, son état est conservé.

Le moniteur peut réactiver une tâche en restaurant cet état.

Les opérations historiques SWAP IN et SWAP OUT sont modélisées comme
des changements d'état entre tâche active et tâche conservée.

Le choix précis de la tâche suivante n'est pas fixé.

## Statut

Documenté

## Traçabilité

HIS-4.1, HIS-4.2, HIS-4.3, HIS-4.4.1 → EXI-0058, EXI-0059 → VM-2105

---

## 21.5 Reprise de mise au point

### VM-2106
Reprise avec conservation de l'état

## Objet

Préciser le comportement de reprise après interruption de mise au point.

## Contraintes historiques

REPRENDRE permet de poursuivre une exécution en conservant les valeurs
acquises avant l'interruption et, dans les cas documentés, de choisir
une nouvelle ligne de départ.

[INRDP75 §4-1]

[INRDP75 §4-3]

[INRDP75 Annexe II]

## Exigences concernées

EXI-0060

## Reconstruction retenue

La reprise conserve l'état des variables et des objets d'exécution
acquis avant l'interruption.

Lorsque la commande autorise une nouvelle ligne de départ, celle-ci
remplace la position d'exécution sans réinitialiser les autres éléments
conservés de l'état.

## Statut

Documenté

## Traçabilité

HIS-13.1, HIS-13.3, HIS-13.4 → EXI-0060 → VM-2106


## 21.4 Fin d'exécution

### VM-2104
Destruction de l'état d'exécution

## Objet

Définir la fin de vie de l'état créé pour une exécution.

## Contraintes historiques

La fin d'un programme rend la console disponible pour poursuivre le dialogue et permet d'engager une nouvelle exécution.

[INRDP75 §4]

[LSE15 §9]

## Exigences concernées

EXI-0030

## Analyse

### Éléments documentés

L'exécution et le programme courant sont distincts.

### Éléments non documentés

Les détails de libération des ressources ne sont pas donnés.

### Contraintes

La fin d'une exécution ne doit pas détruire implicitement le programme courant.

## Reconstruction retenue

À la fin d'une exécution :

- les cadres d'activation sont détruits ;
- la pile d'évaluation est vidée ;
- les ressources temporaires de l'exécution sont libérées ;
- l'état d'exécution disparaît ;
- le programme courant reste disponible.

## Statut

Déduit

## Justification

Cette séparation est nécessaire pour préserver le caractère conversationnel du système.

## Conséquences

### Mémoire

Les ressources dynamiques sont récupérables.

### Dialogue

La session revient à un état permettant une nouvelle commande ou modification.

### Programme courant

Il reste présent.

## Traçabilité

HIS-8.2 → EXI-0030 → VM-2104

---

# 22. Synthèse de la machine abstraite

Le Tome II a maintenant établi les principes suivants pour la machine abstraite reconstruite.

Cette synthèse ne constitue pas un nouveau mécanisme ; elle rassemble les éléments normatifs précédemment définis.

La machine abstraite repose sur une séparation fondamentale entre :

- le texte source ;
- les objets compilés ;
- la représentation compilée ;
- le programme courant ;
- l'état d'exécution.

La ligne constitue l'unité de compilation.

Une ligne validée est analysée syntaxiquement.

Une compilation réussie produit un objet compilé.

Une compilation échouée ne modifie pas le programme courant.

Le programme courant est une collection d'objets compilés identifiés logiquement par leurs numéros de ligne.

Un numéro de ligne n'identifie qu'un seul objet dans le programme courant.

Une nouvelle ligne correcte remplace l'objet existant portant le même numéro.

La suppression d'une ligne retire uniquement l'objet correspondant.

L'ordre logique du programme est défini par l'ordre croissant des numéros de ligne.

La représentation compilée est la forme utilisée par l'interpréteur.

Cette même représentation est également la source de la décompilation.

La machine abstraite retient une forme post-fixée comme modèle de la représentation compilée.

Cette décision est une reconstruction ; elle ne constitue pas une affirmation historique selon laquelle toutes les chaînes codées du L.S.E. seraient identiques à la chaîne post-fixée du LSD.

La forme post-fixée est organisée comme une séquence d'éléments comprenant abstraitement :

- des valeurs ;
- des références ;
- des opérations codées ;
- les éléments de contrôle nécessaires.

Les expressions sont évaluées séquentiellement au moyen d'une pile d'évaluation abstraite.

Les opérations codées sont indépendantes de toute représentation binaire particulière.

Les références compilées désignent des objets logiques et non directement des adresses mémoire.

Le programme compilé peut être conservé sous une forme persistante.

Le chargement d'un programme restauré établit un nouveau programme courant sans nécessiter une recompilation du texte source.

L'exécution crée un état distinct du programme compilé.

Cet état comprend au minimum :

- une position d'exécution ;
- une pile d'évaluation ;
- des cadres d'activation ;
- un environnement d'accès aux objets.

Les procédures et fonctions créent des cadres d'activation.

Les cadres sont chaînés.

Chaque cadre possède une position de retour.

Le retour restaure le contexte appelant.

Les variables sont des objets d'exécution possédant une identité logique, un type, un état de déclaration et une valeur courante.

Les tableaux sont des objets composés à une ou deux dimensions.

Les objets dynamiques peuvent être alloués et libérés.

L'interpréteur exécute directement la représentation compilée.

Le mode machine de bureau enchaîne compilation et exécution immédiate.

Le mode programmé crée une exécution à partir du programme courant sur demande explicite.

Une exécution peut être suspendue et reprise.

La suspension conserve l'état d'exécution.

La fin de l'exécution détruit l'état temporaire sans détruire automatiquement le programme courant.

L'environnement conversationnel constitue l'interface de ces mécanismes avec l'utilisateur.

Le cycle abstrait de traitement d'une entrée est :

réception de la ligne → validation → classification → traitement approprié → résultat ou diagnostic → retour au dialogue.

La machine abstraite ne fixe pas :

- la représentation binaire des opérations ;
- la taille des unités de la chaîne compilée ;
- l'organisation physique de la mémoire ;
- les adresses des objets ;
- la structure matérielle des cadres ;
- la forme matérielle de la pile ;
- l'organisation exacte des fichiers ;
- la répartition du logiciel entre matériels historiques.

Ces éléments ne sont pas déterminés par les sources disponibles et ne sont pas nécessaires à la spécification de la machine abstraite.

La reconstruction retenue est ainsi fondée sur une chaîne de traitement conceptuelle continue :

texte source
→ compilation ligne par ligne
→ objet compilé
→ représentation compilée
→ programme courant
→ interprétation
→ état d'exécution
→ résultats, diagnostics ou interactions
→ décompilation éventuelle.

Cette organisation constitue le socle des mécanismes qui doivent être spécifiés dans les chapitres suivants du plan normatif.

Les chapitres ultérieurs devront notamment préciser, sans modifier les mécanismes déjà établis :

- les règles détaillées de la mémoire et de la durée de vie des objets ;
- les types de variables et leurs représentations abstraites ;
- les chaînes de caractères ;
- les paramètres et modes de passage ;
- les instructions particulières ;
- les expressions complètes ;
- la gestion des erreurs ;
- le dialogue avec le système ;
- le modèle de temps partagé ;
- les opérations de compilation et de décompilation qui restent à formaliser.

La présente reconstruction établit donc la frontière normative essentielle entre ce qui est imposé par les sources et ce qui relève de la conception abstraite.

La représentation compilée doit être exécutable et décompilable.

Elle est reconstruite sous forme post-fixée.

Le programme courant conserve les objets compilés indépendamment les uns des autres.

L'interpréteur agit sur cette représentation sans dépendre du texte source.

L'environnement d'exécution porte l'état dynamique.

La mémoire de travail porte les objets temporaires.

La bibliothèque porte les représentations persistantes.

Le dialogue permet de passer continuellement de l'un à l'autre sans rupture conceptuelle de l'environnement de programmation.

La machine abstraite reconstruite est ainsi définie comme une machine à compilation locale et exécution interprétée, dont l'unité fondamentale de compilation est la ligne, dont la représentation compilée est une séquence structurée reconstruite sous forme post-fixée, et dont l'état d'exécution est distinct du programme courant.

## Traçabilité globale

HIS-4.5.1 → EXI-0002 → VM-0301, VM-0401, VM-0701

HIS-4.5.2 → EXI-0003 → VM-0301, VM-0401, VM-0406, VM-1801

HIS-4.5.3 → EXI-0027 → VM-0701

HIS-5.2 → EXI-0001, EXI-0004, EXI-0018, EXI-0019, EXI-0020 → VM-0301, VM-0302, VM-0502, VM-0503, VM-0504, VM-1901

HIS-5.3 → EXI-0003, EXI-0015 → VM-0301, VM-0406, VM-1801

HIS-6.2 → EXI-0001, EXI-0006, EXI-0007, EXI-0008 → VM-0303, VM-0304, VM-0305, VM-0306

HIS-6.5 → EXI-0001, EXI-0004, EXI-0018, EXI-0020, EXI-0021, EXI-0026 → VM-0302, VM-0501, VM-0502, VM-0504, VM-0505, VM-0506, VM-0607

CPF-7.1 → EXI-0011, EXI-0017, EXI-0047 → VM-0402, VM-0408, VM-1501

CPF-7.2 → EXI-0011, EXI-0012, EXI-0016, EXI-0037, EXI-0047 → VM-0402, VM-0403, VM-0407, VM-1001, VM-1501

CPF-7.3 → EXI-0002, EXI-0003, EXI-0010, EXI-0013, EXI-0015 → VM-0301, VM-0401, VM-0404, VM-0406, VM-1801

CPF-7.4 → EXI-0017 → VM-0408, VM-0409

HIS-8.1 → EXI-0028, EXI-0048 → VM-0702, VM-1502

HIS-8.2 → EXI-0008, EXI-0019, EXI-0023, EXI-0026, EXI-0029, EXI-0030 → VM-0306, VM-0503, VM-0603, VM-0607, VM-0703, VM-0705, VM-2101, VM-2104

HIS-11.2 → EXI-0009, EXI-0022 → VM-0307, VM-0507, VM-0605, VM-1701

HIS-11.5 → EXI-0009, EXI-0021, EXI-0025 → VM-0307, VM-0506, VM-0606, VM-1702

HIS-13.1 → EXI-0034, EXI-0035, EXI-0042, EXI-0043 → VM-0901, VM-0902, VM-1301, VM-1302

HIS-13.2 → EXI-0036, EXI-0042, EXI-0044 → VM-0903, VM-1301, VM-1303

HIS-10.1 → EXI-0014, EXI-0032, EXI-0038 → VM-0405, VM-0802, VM-1101

HIS-10.6 → EXI-0040, EXI-0041, EXI-0050 → VM-1201, VM-1202, VM-1602

HIS-12.1 → EXI-0019, EXI-0051 → VM-0503, VM-1901

HIS-3.1 → EXI-0052 → VM-2001

HIS-3.13 → EXI-0024, EXI-0031, EXI-0033 → VM-0604, VM-0801, VM-0803, VM-2103

HIS-3.14 → EXI-0049 → VM-1601, VM-2101

HIS-11.3, HIS-4.4.3 → EXI-0053, EXI-0054, EXI-0056 → VM-1703

HIS-4.4.4 → EXI-0053, EXI-0054 → VM-1704

HIS-11.1, HIS-11.3 → EXI-0055 → VM-1705

HIS-4.5.4, HIS-11.1, HIS-11.3, HIS-11.5 → EXI-0057 → VM-1706, VM-2002

HIS-11.5 → EXI-0057 → VM-1707

HIS-11.3 → EXI-0056 → VM-1708

HIS-11.3.1 → EXI-0062 → VM-1709

HIS-4.1, HIS-4.2, HIS-4.3, HIS-4.4.1 → EXI-0058, EXI-0059 → VM-2105

HIS-13.1, HIS-13.3, HIS-13.4 → EXI-0060 → VM-2106

# État du catalogue des exigences créé dans le présent Tome II

Les exigences utilisées ou introduites par le présent Tome II couvrent
désormais EXI-0001 à EXI-0062.

Les exigences nouvelles sont :

EXI-0053 — Distinction des fichiers-données temporaires et permanents

EXI-0054 — Accès des fichiers-données selon leur portée

EXI-0055 — Catalogue des fichiers

EXI-0056 — Conservation et réutilisation de l'espace des fichiers temporaires

EXI-0057 — Traitement des commandes portant sur les fichiers-données

EXI-0058 — Gestion abstraite des tâches en temps partagé

EXI-0059 — Moniteur de temps partagé et permutation des tâches

EXI-0060 — Reprise d'une exécution conservant les données de l'interruption

EXI-0061 — Profils des implantations historiques

EXI-0062 — Gestion des identifications et des droits d'accès aux fichiers

Les références EXI-0001 à EXI-0052 demeurent inchangées.

Le prochain identifiant libre du catalogue est :

EXI-0063
