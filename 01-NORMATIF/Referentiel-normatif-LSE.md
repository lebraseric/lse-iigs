# Référentiel normatif LSE

## Partie A — Conventions éditoriales
============================================================
CONVENTIONS ÉDITORIALES
Collection
Reconstruction historique du Langage Symbolique d'Enseignement (L.S.E.)
Version 1.0
============================================================

1. OBJET

Le présent document définit les conventions rédactionnelles,
typographiques, terminologiques et de traçabilité applicables à
l'ensemble de la documentation de reconstruction historique du L.S.E.

Il s'applique à tous les tomes de la collection.

Il constitue la référence normative en matière de rédaction.

Les conventions définies ici ne doivent être modifiées que de façon
exceptionnelle, afin de préserver la stabilité de l'ensemble documentaire.


============================================================
2. PRINCIPES GÉNÉRAUX
============================================================

Toute la documentation poursuit quatre objectifs :

- fidélité historique ;
- traçabilité des informations ;
- stabilité des références ;
- indépendance vis-à-vis d'un langage d'implémentation.

La documentation distingue systématiquement :

• les faits historiquement documentés ;

• les déductions ;

• les hypothèses ;

• les choix de conception.

Ces quatre catégories ne doivent jamais être mélangées.


============================================================
3. LANGUE
============================================================

La langue de rédaction est exclusivement le français.

Les termes historiques français sont toujours privilégiés lorsqu'ils
existent.

Les anglicismes sont évités.

Exemples :

préférer

interpréteur

à

runtime

------------------------------

préférer

chaîne codée

à

bytecode

------------------------------

préférer

chaîne post-fixée

à

Reverse Polish Program

------------------------------

préférer

pile d'évaluation

à

evaluation stack

------------------------------

préférer

cadre d'activation

à

stack frame

------------------------------

préférer

processeur

à

processor

------------------------------

préférer

analyseur de commandes

à

command parser

------------------------------

préférer

bibliothécaire

à

file manager

------------------------------

préférer

zone données-utilisateur

à

user area

------------------------------

préférer

zone de swap

à

swap space

------------------------------

Lorsque la documentation historique emploie un terme particulier,
celui-ci est conservé.


============================================================
4. ORTHOGRAPHE DES NOMS
============================================================

Le langage est désigné par :

L.S.E.

lorsqu'il est cité comme nom officiel.

LSE

est réservé :

- aux identifiants ;
- aux noms de fichiers ;
- aux exemples de code.

Les autres variantes sont proscrites.

Exemples :

Langage Symbolique d'Enseignement (L.S.E.)

commande LISTER

instruction AFFICHER

fonction ALE

procédure &TRIER


============================================================
5. CITATIONS
============================================================

Les citations doivent être reproduites fidèlement.

Les modifications sont interdites.

Les citations longues sont présentées sous forme de bloc.

Les citations courtes peuvent être intégrées au texte.

Toute citation possède une référence.


============================================================
6. RÉFÉRENCES BIBLIOGRAPHIQUES
============================================================

Les références sont indiquées en fin de paragraphe.

Format :

[INRDP75 §3.3]

[INRDP75 §4.2]

[LSE15 §3-2.1]

[NOYELLE89 p.220]

Lorsque plusieurs sources décrivent un même mécanisme,
elles sont toutes citées.

Exemple :

Références

[INRDP75 §3.3]

[NOYELLE89 p.220]


============================================================
7. HIÉRARCHIE DES TITRES
============================================================

# Chapitre

## Section

### Sous-section

#### Sous-sous-section

Au-delà, éviter d'ajouter des niveaux.


============================================================
8. NUMÉROTATION
============================================================

Les chapitres sont numérotés.

Les sections suivent une numérotation hiérarchique.

Exemple

7

7.1

7.1.1

7.1.2

7.2

Cette numérotation est stable.


============================================================
9. IDENTIFIANTS STABLES
============================================================

Chaque élément important possède un identifiant permanent.

Les identifiants ne changent jamais.

Ils permettent les références croisées.

Préfixes :

HIS
Architecture historique

EXI
Exigence

VM
Machine abstraite

CPF
Chaîne Post-Fixée

DEC
Décision de conception

HYP
Hypothèse

SRC
Source documentaire

ALG
Algorithme

ANN
Annexe

Exemple

HIS-7.3

EXI-0042

VM-0131

DEC-0018


============================================================
10. STATUTS
============================================================

Les statuts autorisés sont :

Documenté

Déduit

Probable

Choix de conception

Hypothèse

Chaque élément du Tome II ou des tomes suivants possède exactement
un statut.

Le Tome I ne contient aucun statut autre que :

Documenté

ou

Non documenté.


============================================================
11. RÈGLES PROPRES AU TOME I
============================================================

Le Tome I est une édition documentaire.

Il ne contient jamais :

- de conception ;
- de proposition ;
- d'interprétation ;
- de comparaison avec des systèmes modernes.

Lorsqu'une information manque :

le document indique explicitement que les sources ne permettent pas
de conclure.

Aucune tentative de reconstitution n'est effectuée.


============================================================
12. RÈGLES PROPRES AUX TOMES DE CONCEPTION
============================================================

Chaque mécanisme doit préciser :

• son origine ;

• son statut ;

• sa justification.

Exemple

Origine

[INRDP75 §3.3]

Statut

Déduit

Justification

Le manuel décrit une chaîne directement interprétable.
L'exécution séquentielle est la conséquence la plus simple de cette
description.


============================================================
13. TERMINOLOGIE
============================================================

Les termes suivants sont normalisés.

analyseur de commandes

bibliothécaire

chaîne codée

chaîne post-fixée

commande

compilateur

console

décompilateur

environnement conversationnel

environnement d'exécution

fonction

identificateur

instruction

interpréteur

langage de commande

ligne

ligne de programme

machine de bureau

moniteur

moniteur de temps partagé

mot mémoire

objet

opération codée

procédure

processeur

programme

programme courant

swap

système

système résident

tableau

tâche

temps partagé

utilisateur

variable

zone données-utilisateur

zone de swap

Lorsqu'un terme historique existe, il est prioritaire.


============================================================
14. FIGURES
============================================================

Chaque figure possède :

- un numéro ;

- un titre ;

- une source.

Exemple

Figure 7-2

Enchaînement des processeurs

Source :
INRDP 1975 §3.3

Les figures reconstruites portent explicitement la mention :

Reconstruction de l'auteur.


============================================================
15. TABLEAUX
============================================================

Chaque tableau possède :

- un numéro ;

- un titre.

Exemple

Tableau 6-1

Organisation des processeurs


============================================================
16. RUBRIQUE « LIMITES DES SOURCES »
============================================================

Chaque chapitre du Tome I se termine par :

Limites des sources

Cette rubrique contient uniquement les informations que les documents
historiques ne permettent pas d'établir.

Elle ne contient jamais :

- de proposition ;

- de déduction ;

- de conception.


============================================================
17. VOCABULAIRE DE CONCEPTION
============================================================

Dans les tomes de reconstruction, privilégier un vocabulaire français.

Préférer :

opération codée

à

opcode

------------------------------

code opératoire

à

instruction code

------------------------------

objet compilé

à

compiled object

------------------------------

représentation compilée

à

intermediate representation

------------------------------

cadre d'activation

à

activation record

------------------------------

pile d'évaluation

à

evaluation stack

------------------------------

adresse de retour

à

return address

------------------------------

environnement d'exécution

à

runtime

------------------------------

mémoire de travail

à

working memory


============================================================
18. STABILITÉ
============================================================

Les conventions définies par le présent document constituent la norme
éditoriale de la collection.

Toute évolution ultérieure doit préserver :

- la stabilité des références ;

- la cohérence terminologique ;

- la distinction explicite entre histoire, reconstruction et conception.

Aucun document de la collection ne doit s'écarter de ces conventions.

## Partie B — Catalogue des exigences
============================================================
CATALOGUE DES EXIGENCES
Collection

Reconstruction historique du Langage Symbolique
d'Enseignement (L.S.E.)

Version 1.3 — Consolidation EXI-0001 à EXI-0062
============================================================


1. OBJET

Le présent document définit le catalogue des exigences de la
reconstruction du système L.S.E.

Une exigence représente une propriété stable du système.

Elle constitue l'unité fondamentale de traçabilité de toute la
collection documentaire.

Les exigences sont indépendantes :

- des tomes ;

- des choix de conception ;

- de toute implémentation.

Elles servent de lien entre :

- les sources historiques ;

- la reconstruction ;

- la conception ;

- l'implémentation.


============================================================
2. IDENTIFIANT
============================================================

Chaque exigence possède un identifiant permanent.

Format :

EXI-0001

EXI-0002

...

Les identifiants sont attribués séquentiellement.

Ils ne sont jamais réutilisés.

Ils ne changent jamais.


============================================================
3. STRUCTURE D'UNE EXIGENCE
============================================================

Chaque exigence comporte les rubriques suivantes.

------------------------------------------------------------

Identifiant

------------------------------------------------------------

Titre

------------------------------------------------------------

Description

------------------------------------------------------------

Origine

(référence vers le Tome I ou, lorsque la propriété est dégagée
au cours de la reconstruction, vers l'élément normatif concerné)

------------------------------------------------------------

Sources historiques

------------------------------------------------------------

Statut

------------------------------------------------------------

Justification

------------------------------------------------------------

Conséquences

------------------------------------------------------------

Utilisé par

------------------------------------------------------------

Observations

------------------------------------------------------------


============================================================
4. TITRE
============================================================

Le titre est court.

Il décrit une propriété.

Exemple

Compilation indépendante des lignes

Jamais

Le compilateur compile indépendamment chaque ligne lorsqu'elle
est saisie.


============================================================
5. DESCRIPTION
============================================================

La description exprime une propriété normative.

Elle doit être :

- précise ;

- indépendante de toute implémentation ;

- stable dans le temps.

Elle ne décrit jamais une solution technique.

Elle décrit uniquement ce qui doit être satisfait.


============================================================
6. ORIGINE
============================================================

L'origine référence les paragraphes du Tome I.

Exemple

HIS-6.2

HIS-7.1

HIS-9.3

Une exigence peut posséder plusieurs origines.

Lorsque la reconstruction du Tome II introduit une exigence dont
la formulation s'appuie directement sur une structure déjà identifiée
dans le Tome I, la référence d'origine du Tome I est conservée. Les
références CPF utilisées dans certaines exigences ci-dessous désignent
les éléments de la chaîne post-fixée analysés dans le Tome I et sont
maintenues pour assurer la continuité de la traçabilité existante.


============================================================
7. SOURCES HISTORIQUES
============================================================

Les références bibliographiques sont rappelées.

Exemple

[INRDP75 §3.3]

[NOYELLE89 p.220]


============================================================
8. STATUT
============================================================

Les statuts autorisés sont :

Documenté

Déduit

Probable

Choix de conception

Hypothèse

Le statut est déterminé conformément aux conventions éditoriales.


============================================================
9. JUSTIFICATION
============================================================

La justification explique pourquoi l'exigence existe.

Elle peut rappeler :

- une contrainte historique ;

- une nécessité fonctionnelle ;

- une conséquence logique.

La justification ne contient jamais de conception détaillée.


============================================================
10. CONSÉQUENCES
============================================================

Cette rubrique décrit les effets de l'exigence.

Exemple

Impacte :

- la compilation ;

- la chaîne post-fixée ;

- l'interpréteur.


============================================================
11. UTILISÉ PAR
============================================================

Cette rubrique contient les références vers :

VM

CPF

ALG

DEC

ou tout autre élément utilisant cette exigence.

Elle est enrichie progressivement.


============================================================
12. OBSERVATIONS
============================================================

Informations complémentaires.

Historique.

Commentaires.

Limites éventuelles.


============================================================
13. RÈGLES DE RÉDACTION
============================================================

Une exigence doit :

exprimer une propriété ;

ne contenir aucune ambiguïté ;

être vérifiable ;

être indépendante de toute implémentation.

Une exigence ne doit jamais :

décrire un algorithme ;

décrire une structure mémoire ;

décrire un format binaire.

Ces éléments relèvent des tomes de conception.


============================================================
14. TRAÇABILITÉ
============================================================

Toute exigence doit pouvoir être reliée :

aux sources historiques ;

au Tome I ;

au Tome II ;

aux décisions de conception ;

aux algorithmes ;

aux futures implémentations.

La traçabilité est bidirectionnelle.


============================================================
15. ÉVOLUTION
============================================================

Une exigence ne disparaît jamais.

Elle peut uniquement :

être précisée ;

être complétée ;

changer de statut.

Toute évolution conserve le même identifiant.


============================================================
16. CLASSEMENT
============================================================

Les exigences sont regroupées par thèmes.

Compilation

CPF

Interpréteur

Mémoire

Procédures

Fonctions

Variables

Expressions

Commandes

Dialogue

Gestion des erreurs

Décompilation

Gestion des programmes

Temps partagé

Autres


============================================================
17. EXIGENCES CONSOLIDÉES
============================================================

La présente section constitue l'état consolidé du catalogue à la fin
de la rédaction actuelle des Tomes I à IV.

Les identifiants EXI-0001 à EXI-0062 sont attribués et ne doivent
plus être réutilisés.

Le prochain identifiant libre est EXI-0063.


------------------------------------------------------------
EXI-0001

Titre

Compilation indépendante des lignes

Description

Chaque ligne de programme est compilée indépendamment des autres
lignes dès sa validation.

Origine

HIS-6.2

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.217]

Statut

Documenté

Justification

Le système permet l'ajout, la modification ou la suppression
d'une ligne sans recompilation du reste du programme.

Conséquences

Le compilateur travaille ligne par ligne.

Une représentation compilée est produite pour chaque ligne acceptée.

Utilisé par

VM-0301

VM-0302

VM-0304

VM-0501

Observations

Cette exigence est le point d'appui général de la reconstruction
par objets compilés.


------------------------------------------------------------
EXI-0002

Titre

Représentation compilée exécutable

Description

Toute ligne de programme acceptée par la compilation possède une
représentation compilée permettant son exécution par l'interpréteur.

Origine

HIS-4.5.1

HIS-5.2

Sources historiques

[INRDP75 §3.3]

Statut

Documenté

Justification

Le manuel décrit la chaîne codée comme le résultat de la compilation
ligne à ligne et comme la forme exécutable par l'interpréteur.

Conséquences

La machine abstraite doit conserver, pour chaque ligne acceptée, une
représentation compilée distincte du texte source.

Utilisé par

VM-0301

VM-0404

VM-0602

Observations

L'exigence ne fixe aucun format interne.


------------------------------------------------------------
EXI-0003

Titre

Décompilabilité de la représentation compilée

Description

Toute représentation compilée conservée comme programme courant doit
pouvoir être transformée en une représentation textuelle du programme
suffisamment complète pour permettre son listage.

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

Le décompilateur transforme la chaîne codée en caractères et permet
notamment de lister le programme courant. Noyelle décrit également la
possibilité de recréer le texte source à partir de la chaîne post-fixée.

Conséquences

La représentation compilée doit contenir, directement ou indirectement
dans l'organisation abstraite du programme, les informations nécessaires
à la restitution textuelle.

Utilisé par

VM-0301

VM-0405

VM-1801

Observations

L'exigence ne signifie pas que le texte source original doit être
conservé séparément.


------------------------------------------------------------
EXI-0004

Titre

Rejet atomique d'une ligne incorrecte

Description

Une ligne dont la compilation échoue pour une erreur syntaxique ne
modifie pas le programme courant.

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

Les sources indiquent qu'une ligne syntaxiquement incorrecte n'est
pas prise en compte et doit être corrigée puis réintroduite.

Conséquences

L'état antérieur du programme courant est préservé lorsqu'une ligne
échoue à la compilation.

Utilisé par

VM-0302

VM-0303

VM-0602

Observations

L'exigence porte sur le résultat observable de la compilation et non
sur l'algorithme interne du compilateur.


------------------------------------------------------------
EXI-0005

Titre

Programme courant constitué de représentations compilées

Description

Le programme courant conserve, pour chaque ligne acceptée, une
représentation compilée permettant son exécution et sa décompilation.

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

Les sources décrivent un programme courant qui est compilé ligne par
ligne, exécuté à partir de la chaîne codée et décompilé pour le listage.
La reconstruction doit donc disposer d'un état abstrait conservant les
représentations compilées des lignes acceptées.

Conséquences

Le programme courant devient l'objet central partagé par la compilation,
l'interprétation, la décompilation et la gestion des programmes.

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


------------------------------------------------------------
EXI-0006

Titre

Unicité du numéro de ligne

Description

Le programme courant ne contient au plus qu'une ligne pour chaque
numéro de ligne.

Origine

HIS-6.2

Sources historiques

[INRDP75 §« Présentation d'un programme L.S.E. »]

[LSE15 §1]

Statut

Documenté

Justification

Les sources indiquent qu'une nouvelle ligne portant le même numéro
remplace la précédente et que deux lignes de même numéro ne coexistent
pas.

Conséquences

Le numéro de ligne peut servir d'identifiant logique stable d'une ligne
du programme courant.

Utilisé par

VM-0303

VM-0304

VM-0601

Observations

La façon dont cette unicité est réalisée en mémoire n'est pas définie.


------------------------------------------------------------
EXI-0007

Titre

Suppression logique d'une ligne

Description

La suppression d'une ligne retire l'objet compilé correspondant du
programme courant sans modifier les autres lignes.

Origine

HIS-6.2

Sources historiques

[NOYELLE89 p.217-218]

Statut

Documenté

Justification

La possibilité d'ajouter, modifier ou supprimer une ligne sans reprendre
la traduction de l'ensemble du programme est explicitement rapportée
dans le récit historique de Noyelle.

Conséquences

La structure du programme courant doit permettre la disparition
indépendante d'un objet compilé.

Utilisé par

VM-0303

VM-0305

VM-0601

Observations

Les sources ne précisent pas la syntaxe exacte de suppression dans
toutes les implantations.


------------------------------------------------------------
EXI-0008

Titre

Ordre logique croissant des lignes

Description

Les objets compilés d'un programme courant sont parcourus selon l'ordre
croissant de leurs numéros de ligne lorsqu'aucun mécanisme de contrôle
du programme ne provoque un autre transfert d'exécution.

Origine

HIS-6.2

HIS-8.2

Sources historiques

[INRDP75 §« Présentation d'un programme L.S.E. »]

[LSE15 §1]

Statut

Documenté

Justification

Le numéro de ligne constitue dans les sources l'identification permettant
de replacer les lignes dans l'ordre du programme.

Conséquences

Le programme courant doit fournir un ordre logique déterminé
indépendamment de l'ordre d'entrée des lignes.

Utilisé par

VM-0306

VM-0603

VM-2101

Observations

Les instructions de contrôle et les procédures peuvent modifier le
flux d'exécution ; elles ne remettent pas en cause l'ordre de base du
programme.


------------------------------------------------------------
EXI-0009

Titre

Persistance du programme sous forme compilée

Description

Un programme courant peut être conservé sous une forme compilée
permettant son rechargement ultérieur comme programme courant sans
nécessiter la conservation obligatoire de son texte source.

Origine

HIS-11.2

HIS-11.5

Sources historiques

[INRDP75 §4-4]

[LSE15 §9-5]

Statut

Documenté

Justification

Le manuel INRDP indique explicitement que le programme est rangé sous
forme codée et non sous forme d'une chaîne de caractères L.S.E.

Conséquences

La forme de conservation du programme doit être compatible avec la
représentation compilée utilisée par l'interpréteur.

Utilisé par

VM-0307

VM-0404

VM-1701

Observations

La nature exacte du fichier-programme historique n'est pas reconstruite
au niveau binaire.


------------------------------------------------------------
EXI-0010

Titre

Représentation compilée commune à l'exécution et à la décompilation

Description

La représentation compilée d'un programme doit pouvoir être utilisée
à la fois comme support de son exécution et comme source de sa
décompilation.

Origine

HIS-4.5.1

HIS-4.5.2

Sources historiques

[INRDP75 §3.3]

Statut

Déduit

Justification

Le manuel décrit une même chaîne codée comme forme exécutable par
l'interpréteur et transformable en caractères par le décompilateur.

Conséquences

La machine abstraite ne doit pas maintenir obligatoirement deux
représentations compilées indépendantes, l'une destinée à l'exécution
et l'autre à la décompilation.

Utilisé par

VM-0401

VM-0404

VM-0405

Observations

Cette exigence n'interdit pas qu'une implantation réalise des
transformations temporaires.


------------------------------------------------------------
EXI-0011

Titre

Ordre post-fixé des opérations

Description

La représentation compilée reconstruite utilise, pour les expressions,
une organisation post-fixée dans laquelle les éléments nécessaires à une
opération apparaissent avant l'opération qui les consomme.

Origine

CPF-7.1

CPF-7.2

Sources historiques

[NOYELLE89 p.219-220]

Statut

Probable

Justification

La représentation post-fixée est explicitement documentée pour le LSD,
dont le L.S.E. est présenté historiquement comme le successeur. Les
sources L.S.E. documentent une chaîne codée exécutable et décompilable
mais ne permettent pas d'établir avec certitude son identité avec la
chaîne post-fixée du LSD.

Conséquences

La machine abstraite peut définir les expressions compilées sous la
forme d'une séquence post-fixée.

Utilisé par

VM-0402

VM-0403

VM-0404

VM-0405

VM-1502

Observations

Le statut « Probable » interdit de présenter l'organisation post-fixée
comme un fait documentaire certain pour toutes les implantations L.S.E.


------------------------------------------------------------
EXI-0012

Titre

Évaluation séquentielle de la représentation compilée

Description

L'interpréteur doit pouvoir exécuter une représentation compilée en
parcourant séquentiellement les éléments codés selon leur ordre logique,
sans nécessiter une phase de reconstruction du texte source.

Origine

HIS-4.5.1

CPF-7.2

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.220]

Statut

Déduit

Justification

L'interpréteur exécute directement la chaîne codée et la représentation
post-fixée est explicitement présentée comme adaptée à une interprétation
efficace.

Conséquences

L'interpréteur peut fonctionner directement sur la représentation
compilée.

Utilisé par

VM-0403

VM-0404

VM-0603

Observations

L'exigence ne fixe pas l'utilisation matérielle d'une pile.


------------------------------------------------------------
EXI-0013

Titre

Représentation abstraite des opérations exécutables

Description

La représentation compilée contient des éléments identifiables par
l'interpréteur comme des opérations exécutables, indépendamment de leur
représentation matérielle.

Origine

HIS-4.5.1

CPF-7.3

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.220]

Statut

Déduit

Justification

Une chaîne directement exécutable doit contenir ou référencer des
éléments permettant d'identifier les opérations à effectuer. Les sources
ne documentent pas leur forme, mais la fonction qu'ils remplissent est
nécessairement présente dans la chaîne exécutable.

Conséquences

Le modèle abstrait distingue les valeurs des opérations exécutables.

Utilisé par

VM-0404

VM-0405

VM-0502

VM-1502

Observations

Aucun code opératoire historique concret n'est attribué à cette
exigence.


------------------------------------------------------------
EXI-0014

Titre

Représentation compilée des données utilisées par l'exécution

Description

La représentation compilée doit permettre à l'interpréteur d'obtenir
les valeurs constantes et les références nécessaires aux objets
d'exécution utilisés par les instructions et expressions du programme.

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

L'exécution de programmes compilés portant sur des variables, des
nombres, des chaînes et des tableaux nécessite que la représentation
compilée fournisse les informations permettant d'accéder aux données
correspondantes.

Conséquences

La représentation compilée distingue les valeurs immédiates des
références vers des objets d'exécution.

Utilisé par

VM-0405

VM-0406

VM-0901

VM-1001

VM-1101

VM-1201

Observations

La structure de ces références n'est pas historiquement documentée.


------------------------------------------------------------
EXI-0015

Titre

Restitution textuelle déterministe

Description

La décompilation d'un objet compilé doit produire une représentation
textuelle valide correspondant à la construction de langage représentée
par cet objet.

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

La documentation décrit explicitement la transformation de la chaîne
codée en caractères et son utilisation pour le listage. La reconstruction
précise que le résultat doit être une représentation textuelle valide du
même objet compilé.

Conséquences

La décompilation constitue une transformation déterminée de la
représentation compilée vers une forme textuelle.

Utilisé par

VM-0406

VM-1801

VM-1802

Observations

La forme textuelle produite peut être normalisée ; l'identité caractère
par caractère avec l'entrée initiale n'est pas exigée.


------------------------------------------------------------
EXI-0016

Titre

Compacité de la représentation compilée

Description

La représentation compilée doit éviter la conservation répétée des
éléments syntaxiques textuels qui ne sont pas nécessaires à l'exécution,
tout en conservant les informations nécessaires à l'exécution et à la
décompilation.

Origine

CPF-7.2

Sources historiques

[NOYELLE89 p.220]

Statut

Probable

Justification

La représentation post-fixée est historiquement décrite comme environ
deux fois moins encombrante que le texte source. La reconstruction
conserve cette propriété comme objectif architectural sans imposer un
taux exact indépendant du programme.

Conséquences

La représentation compilée privilégie les opérations codées et les
références structurées plutôt que la conservation directe de la syntaxe
textuelle.

Utilisé par

VM-0402

VM-0407

VM-1601

Observations

Aucune garantie quantitative universelle n'est retenue pour toutes les
implantations.


------------------------------------------------------------
EXI-0017

Titre

Distinction documentaire des représentations compilées

Description

La machine abstraite doit conserver la distinction entre la chaîne
codée attestée pour le L.S.E. et la chaîne post-fixée explicitement
attestée pour le LSD, tout en pouvant utiliser la représentation
post-fixée comme reconstruction de la forme compilée du L.S.E.

Origine

CPF-7.1

CPF-7.4

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.219-220]

Statut

Documenté

Justification

Les sources emploient deux termes différents et ne permettent pas
d'établir leur identité historique complète.

Conséquences

Les affirmations historiques doivent rester prudentes.

La représentation post-fixée de la machine abstraite ne doit pas être
présentée comme un fait documentaire certain pour toutes les versions
du L.S.E.

Utilisé par

VM-0408

VM-0409

VM-2201

Observations

La distinction est obligatoire pour la traçabilité historique.


------------------------------------------------------------
EXI-0018

Titre

Analyse syntaxique préalable à la compilation

Description

Toute ligne de programme doit être analysée syntaxiquement avant son
intégration au programme courant sous forme compilée.

Origine

HIS-5.2

HIS-6.5

Sources historiques

[INRDP75 §3.3]

[LSE15 §1]

Statut

Documenté

Justification

Le rôle du compilateur est explicitement décrit comme comprenant
l'analyse de la ligne et la détection des erreurs de syntaxe.

Conséquences

Une ligne syntaxiquement incorrecte ne produit pas d'objet compilé
accepté.

Utilisé par

VM-0502

VM-0503

VM-0302

Observations

L'algorithme d'analyse syntaxique n'est pas documenté.


------------------------------------------------------------
EXI-0019

Titre

Distinction des erreurs de compilation et d'exécution

Description

La validation syntaxique d'une ligne ne garantit pas l'absence de
toutes les erreurs susceptibles d'apparaître lors de son exécution.

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

Les sources distinguent explicitement les erreurs syntaxiques détectées
lors de la compilation des erreurs qui ne peuvent être détectées qu'à
l'exécution.

Conséquences

La machine abstraite ne doit pas exiger que le compilateur résolve toutes
les références ou vérifie toutes les conditions d'exécution.

Utilisé par

VM-0503

VM-0602

VM-1901

Observations

Le partage exact des vérifications entre compilation et exécution n'est
pas entièrement documenté.


------------------------------------------------------------
EXI-0020

Titre

Production d'une représentation compilée par ligne

Description

Toute ligne syntaxiquement acceptée produit un objet compilé contenant
une représentation exécutable de cette ligne.

Origine

HIS-5.2

HIS-6.5

Sources historiques

[INRDP75 §3.3]

[NOYELLE89 p.217]

Statut

Documenté

Justification

Le résultat de la compilation ligne à ligne est explicitement décrit
comme une chaîne codée exécutable par l'interpréteur.

Conséquences

La compilation transforme une entrée textuelle en représentation
compilée.

Utilisé par

VM-0301

VM-0504

VM-0401

Observations

La structure exacte de la représentation est reconstruite dans le
chapitre 4 du Tome II.


------------------------------------------------------------
EXI-0021

Titre

Compilation uniforme des lignes entrées indirectement

Description

Les lignes de programme fournies par une source de données destinée à
l'encodage sont soumises au même mécanisme logique de compilation que
les lignes entrées interactivement.

Origine

HIS-6.5

HIS-11.5

Sources historiques

[INRDP75 Annexe II]

[LSE15 §9-5]

Statut

Déduit

Justification

Le manuel décrit ENCODER comme faisant intervenir le compilateur et
comme fournissant à celui-ci des lignes provenant d'un fichier données.

Conséquences

Le mécanisme de compilation ne dépend pas de la provenance physique des
caractères.

Utilisé par

VM-0506

VM-1101

VM-1702

Observations

Le protocole précis d'ENCODER n'est pas reconstruit dans ce mécanisme.


------------------------------------------------------------
EXI-0022

Titre

Indépendance d'exécution vis-à-vis du texte source

Description

L'exécution d'un programme compilé ne dépend pas de la présence d'une
copie séparée de son texte source.

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

Le programme compilé est directement exécutable et peut être conservé
sous forme codée.

Conséquences

Le texte source n'est pas une donnée obligatoire de l'environnement
d'exécution.

Utilisé par

VM-0401

VM-0507

VM-1701

VM-1801

Observations

Le système peut néanmoins produire ou conserver une représentation
textuelle par DECODER.


------------------------------------------------------------
EXI-0023

Titre

Début de l'exécution au premier objet logique

Description

Lorsqu'une exécution programmée commence sans point de reprise ni
transfert explicite, elle débute au premier objet compilé selon l'ordre
logique des numéros de ligne.

Origine

HIS-8.2

Sources historiques

[INRDP75 §3.3]

[LSE15 §1]

Statut

Déduit

Justification

Le programme est exécuté dans l'ordre croissant des numéros de lignes ;
le début normal de l'exécution doit donc correspondre au premier numéro
existant.

Conséquences

L'interpréteur doit pouvoir déterminer le premier objet exécutable.

Utilisé par

VM-0603

VM-2101

Observations

Les mécanismes de reprise et de branchement peuvent modifier le point
d'exécution ultérieur.


------------------------------------------------------------
EXI-0024

Titre

Conservation d'une position d'exécution interrompue

Description

Une interruption d'un programme en cours d'exécution doit permettre la
conservation d'une position logique suffisante pour reprendre l'exécution
ultérieurement lorsque le mécanisme de reprise le prévoit.

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

Les sources décrivent la possibilité d'interrompre un programme,
d'examiner son état puis de poursuivre son exécution.

Conséquences

L'état d'exécution doit contenir au minimum une position logique de
reprise.

Utilisé par

VM-0604

VM-0801

VM-2102

Observations

La forme exacte de cette position n'est pas documentée.


------------------------------------------------------------
EXI-0025

Titre

Chargement comme remplacement du programme courant

Description

Le chargement d'un fichier-programme établit comme programme courant
le programme compilé contenu dans ce fichier.

Origine

HIS-11.5

Sources historiques

[INRDP75 §4-4]

[LSE15 §9-5]

Statut

Documenté

Justification

Les sources décrivent explicitement le programme chargé comme devenant
le programme courant.

Conséquences

Le programme courant antérieur cesse d'être le programme courant actif.

Utilisé par

VM-0606

VM-1701

Observations

Les sources ne détaillent pas les conditions de remplacement lorsqu'une
exécution est en cours.


------------------------------------------------------------
EXI-0026

Titre

Prise en compte immédiate des lignes acceptées

Description

Une ligne de programme acceptée par la compilation devient la
représentation du numéro de ligne correspondant dans le programme
courant avant toute exécution ultérieure demandée sur ce programme.

Origine

HIS-6.5

HIS-8.2

Sources historiques

[INRDP75 §3.3]

[LSE15 §1]

Statut

Déduit

Justification

La compilation est réalisée lors de l'entrée et la ligne correcte est
immédiatement mémorisée ; l'exécution ultérieure porte sur le programme
courant ainsi modifié.

Conséquences

La machine ne doit pas conserver une version compilée obsolète comme
version courante après l'acceptation d'une modification.

Utilisé par

VM-0505

VM-0607

VM-2101

Observations

L'exigence ne traite pas des exécutions déjà en cours au moment d'une
modification.


------------------------------------------------------------
EXI-0027

Titre

Exécution directe de la représentation compilée

Description

L'interpréteur exécute directement la représentation compilée du
programme ou de la ligne, sans nécessiter une recompilation préalable
du texte source lors de chaque exécution.

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

Les sources décrivent explicitement l'interpréteur comme exécutant la
forme fournie par le compilateur.

Conséquences

La représentation compilée est l'entrée normative de l'interpréteur.

Utilisé par

VM-0701

VM-0702

VM-0703

Observations

La structure interne de l'interpréteur n'est pas documentée.


------------------------------------------------------------
EXI-0028

Titre

Enchaînement immédiat compilation-exécution

Description

Une entrée destinée à l'exécution immédiate est compilée puis exécutée
par l'interpréteur dès que sa validation et sa compilation sont achevées
avec succès.

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

Le manuel décrit explicitement l'enchaînement compilation,
interprétation et exécution immédiate.

Conséquences

Le mode machine de bureau ne nécessite pas l'intégration préalable de
la ligne dans un programme à exécuter ultérieurement.

Utilisé par

VM-0702

VM-0501

Observations

Le traitement des lignes de programme numérotées reste distinct du
traitement des expressions exécutées immédiatement.


------------------------------------------------------------
EXI-0029

Titre

Exécution explicite du programme courant

Description

L'exécution programmée du programme courant commence sur demande
explicite de l'utilisateur et porte sur l'état courant du programme au
moment où l'exécution est engagée.

Origine

HIS-8.2

HIS-3.13

Sources historiques

[INRDP75 §3.3]

[INRDP75 §4-3]

Statut

Documenté

Justification

Le mode programmé est explicitement décrit comme une exécution demandée
par la commande EXECUTER.

Conséquences

Une phase d'exécution distincte de l'entrée des lignes est nécessaire.

Utilisé par

VM-0703

VM-0607

VM-2101

Observations

Les détails du chargement matériel de l'interpréteur ne sont pas
reconstruits.


------------------------------------------------------------
EXI-0030

Titre

Terminaison explicite ou naturelle de l'exécution

Description

Une exécution se termine lorsque le flux d'exécution atteint la fin du
programme ou lorsqu'une instruction ou commande de terminaison demande
explicitement son arrêt.

Origine

HIS-8.2

HIS-14.1

Sources historiques

[INRDP75 §4.3]

[LSE15 §9]

Statut

Déduit

Justification

La machine abstraite doit distinguer la fin du flux d'exécution de la
conservation du programme courant.

Conséquences

La fin d'une exécution ne détruit pas automatiquement le programme
courant.

Utilisé par

VM-0705

VM-2103

VM-0303

Observations

Les sources ne permettent pas d'unifier toutes les variantes historiques
de terminaison.


------------------------------------------------------------
EXI-0031

Titre

État d'exécution distinct du programme compilé

Description

Une exécution possède un état distinct de la représentation compilée
du programme, contenant les informations nécessaires à sa progression
et à sa reprise.

Origine

HIS-3.13

HIS-8.3

Sources historiques

[NOYELLE89 p.221]

[INRDP75 Annexe II]

Statut

Déduit

Justification

La possibilité d'interrompre puis de reprendre une exécution impose la
conservation d'un état qui ne peut être réduit au seul texte ou code du
programme.

Conséquences

L'environnement d'exécution comprend une position d'exécution et l'état
des objets nécessaires.

Utilisé par

VM-0801

VM-0604

VM-0802

VM-0901

VM-2102

Observations

La structure exacte de cet état n'est pas documentée.


------------------------------------------------------------
EXI-0032

Titre

Résolution des références vers les objets d'exécution

Description

Toute référence compilée à un objet doit pouvoir être résolue dans
l'environnement d'exécution applicable au moment de son utilisation.

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

La représentation compilée doit permettre l'accès aux objets utilisés
lors de l'exécution.

Conséquences

L'environnement d'exécution fournit une résolution abstraite des
références.

Utilisé par

VM-0802

VM-0405

VM-1001

VM-1101

VM-1301

Observations

La structure des tables internes n'est pas spécifiée.


------------------------------------------------------------
EXI-0033

Titre

Observation de l'état d'exécution

Description

Une exécution suspendue doit pouvoir exposer à l'environnement
conversationnel les informations d'état autorisées par le langage et le
mode d'exécution.

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

Les sources décrivent explicitement l'accès aux valeurs et à l'état du
programme pendant la mise au point.

Conséquences

La machine abstraite doit offrir une interface entre l'état d'exécution
et le dialogue utilisateur.

Utilisé par

VM-0803

VM-2001

VM-2102

Observations

La liste exacte des informations accessibles peut dépendre de
l'implantation ou du mode.


------------------------------------------------------------
EXI-0034

Titre

Cadre d'activation des appels

Description

L'activation d'une procédure ou fonction crée un contexte d'exécution
distinct permettant d'associer les paramètres et les objets locaux à cet
appel.

Origine

HIS-13.1

HIS-13.2

Sources historiques

[NOYELLE89 p.218-220]

[LSE15 §6]

Statut

Déduit

Justification

La présence de paramètres et de variables locales implique un contexte
associé à chaque activation.

Conséquences

Les variables locales et les paramètres ne doivent pas être assimilés
aux variables globales du programme courant.

Utilisé par

VM-0901

VM-0902

VM-0903

VM-1301

Observations

La structure mémoire concrète n'est pas documentée.


------------------------------------------------------------
EXI-0035

Titre

Chaînage des activations

Description

Chaque activation active doit pouvoir retrouver le contexte d'activation
qui l'a appelée jusqu'à l'activation initiale.

Origine

HIS-13.1

Sources historiques

[LSE15 §6]

[NOYELLE89 p.218-220]

Statut

Déduit

Justification

La gestion des appels imbriqués exige la conservation du contexte
appelant.

Conséquences

L'environnement d'exécution forme une chaîne d'activations.

Utilisé par

VM-0902

VM-0903

VM-1301

VM-2102

Observations

La forme concrète de cette chaîne n'est pas spécifiée.


------------------------------------------------------------
EXI-0036

Titre

Retour au point d'appel

Description

La fin normale ou explicite d'une activation restitue le contrôle au
point d'exécution suivant l'appel.

Origine

HIS-13.2

Sources historiques

[LSE15 §6]

Statut

Documenté

Justification

Les instructions de retour décrites par le manuel établissent une
continuité entre l'appel d'une procédure et la poursuite du programme
appelant.

Conséquences

Le cadre d'activation conserve une position de retour abstraite.

Utilisé par

VM-0903

VM-1302

VM-2101

Observations

Le mécanisme concret de représentation de cette position n'est pas
documenté.


------------------------------------------------------------
EXI-0037

Titre

Évaluation par valeurs intermédiaires

Description

L'exécution d'une représentation post-fixée doit pouvoir conserver les
résultats intermédiaires nécessaires aux opérations suivantes.

Origine

CPF-7.2

Sources historiques

[NOYELLE89 p.220]

Statut

Déduit

Justification

Une évaluation post-fixée d'expressions composées nécessite la
conservation de résultats intermédiaires.

Conséquences

La machine abstraite dispose d'une pile d'évaluation.

Utilisé par

VM-1001

VM-1002

VM-1502

Observations

La pile est un mécanisme abstrait.


------------------------------------------------------------
EXI-0038

Titre

Identité stable des variables

Description

Chaque variable déclarée est associée à un identificateur logique
permettant son accès dans le contexte d'exécution où elle est visible.

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

La machine abstraite doit pouvoir résoudre un identificateur en objet
variable.

Utilisé par

VM-1101

VM-0802

VM-1001

Observations

La représentation interne des identificateurs n'est pas documentée.


------------------------------------------------------------
EXI-0039

Titre

Valeur mutable des variables

Description

Une variable déclarée possède une valeur courante susceptible d'être
consultée et modifiée par les opérations autorisées du langage ou de
l'environnement d'exécution.

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

Les sources décrivent explicitement la consultation et la modification
de valeurs dans le cadre du fonctionnement conversationnel et de la mise
au point.

Conséquences

Les variables doivent être représentées comme des objets d'état plutôt
que comme de simples constantes compilées.

Utilisé par

VM-1102

VM-0803

VM-1501

Observations

La représentation interne de la valeur n'est pas fixée ici.


------------------------------------------------------------
EXI-0040

Titre

Tableaux à une ou deux dimensions

Description

La machine abstraite doit prendre en charge des tableaux possédant une
ou deux dimensions.

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


------------------------------------------------------------
EXI-0041

Titre

Allocation à la déclaration du tableau

Description

Un tableau acquiert son espace d'exécution lorsque sa déclaration est
exécutée et demeure disponible jusqu'à sa libération ou la fin du contexte
qui le possède.

Origine

HIS-10.6

Sources historiques

[INRDP75 §4-1]

[LSE15 §3-1]

Statut

Documenté

Justification

Les sources décrivent la déclaration du tableau comme déterminant
l'espace à réserver et l'utilisation avant déclaration comme une erreur.

Conséquences

L'existence du tableau dépend de son état de déclaration.

Utilisé par

VM-1202

VM-1203

VM-1601

Observations

La forme de l'espace réservé n'est pas documentée.


------------------------------------------------------------
EXI-0042

Titre

Représentation compilée des procédures et fonctions

Description

Une procédure ou fonction déclarée doit posséder une représentation
compilée permettant son activation par un appel et la création du
contexte d'exécution correspondant.

Origine

HIS-13.1

HIS-13.2

Sources historiques

[NOYELLE89 p.218-220]

[LSE15 §6]

Statut

Déduit

Justification

Les procédures et fonctions font partie du langage et doivent pouvoir
être appelées pendant l'exécution du programme compilé.

Conséquences

La représentation compilée contient ou référence le corps compilé des
procédures et fonctions.

Utilisé par

VM-1301

VM-0901

VM-1302

VM-1303

Observations

L'emplacement concret des définitions n'est pas documenté.


------------------------------------------------------------
EXI-0043

Titre

Création d'une activation lors d'un appel

Description

L'exécution d'un appel de procédure ou de fonction crée une activation
distincte correspondant à cet appel.

Origine

HIS-13.1

Sources historiques

[LSE15 §6]

[NOYELLE89 p.219]

Statut

Déduit

Justification

Les paramètres et variables locales doivent être associés à l'appel
actif.

Conséquences

Un appel entraîne la création d'un cadre d'activation.

Utilisé par

VM-1302

VM-0901

VM-0902

Observations

Les modes précis de passage des paramètres sont traités séparément.


------------------------------------------------------------
EXI-0044

Titre

Restitution du contrôle après retour

Description

La terminaison d'une procédure ou fonction restitue le contrôle au
contexte appelant et, pour une fonction, rend le résultat défini par
l'exécution.

Origine

HIS-13.2

Sources historiques

[LSE15 §6]

Statut

Documenté

Justification

Les instructions de retour et de résultat sont explicitement
documentées.

Conséquences

Le cadre d'activation courant est terminé.

Utilisé par

VM-1303

VM-0903

VM-1502

Observations

Le mode exact de transport du résultat n'est pas documenté.


------------------------------------------------------------
EXI-0045

Titre

Compilation des instructions en opérations exécutables

Description

Chaque instruction acceptée par le compilateur est représentée dans
l'objet compilé par une séquence d'opérations permettant son exécution
par l'interpréteur.

Origine

HIS-5.2

HIS-7.3

Sources historiques

[INRDP75 §3.3]

[INRDP75 §« Présentation d'un programme L.S.E. »]

Statut

Déduit

Justification

La chaîne codée est directement exécutable et la compilation porte sur
les lignes contenant une ou plusieurs instructions.

Conséquences

La représentation compilée ne conserve pas nécessairement les frontières
syntaxiques textuelles exactes des instructions.

Utilisé par

VM-1401

VM-0404

VM-0701

Observations

La représentation des instructions composées reste abstraite.


------------------------------------------------------------
EXI-0046

Titre

Contrôle explicite du flux d'exécution

Description

La représentation compilée doit pouvoir modifier la position logique
d'exécution lorsqu'une instruction de contrôle du langage le demande.

Origine

HIS-8.2

HIS-14.1

Sources historiques

[INRDP75 §3]

[INRDP75 §4-3]

Statut

Documenté

Justification

Les instructions de contrôle constituent une propriété du langage
documentée dans les sources.

Conséquences

L'état d'exécution doit comporter une position modifiable.

Utilisé par

VM-1402

VM-0603

VM-0604

Observations

La représentation interne des branchements n'est pas documentée.


------------------------------------------------------------
EXI-0047

Titre

Compilation post-fixée des expressions

Description

Toute expression compilée est représentée selon un ordre post-fixé dans
lequel les opérandes nécessaires à une opération apparaissent avant cette
opération.

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

La forme post-fixée est explicitement documentée dans l'histoire du LSD
et constitue la reconstruction retenue pour la représentation compilée
du L.S.E.

Conséquences

Les expressions peuvent être évaluées par parcours séquentiel et pile
d'évaluation.

Utilisé par

VM-1501

VM-1001

VM-0402

Observations

Cette exigence fixe la forme abstraite reconstruite et non un format
binaire historique.


------------------------------------------------------------
EXI-0048

Titre

Production d'une valeur par l'évaluation

Description

L'évaluation d'une expression valide produit une valeur conforme au
type et à la sémantique de l'expression.

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


------------------------------------------------------------
EXI-0049

Titre

Séparation du stockage logique et de la mémoire d'exécution

Description

La machine abstraite doit distinguer les représentations persistantes
des programmes et données de l'état temporaire nécessaire à leur
exécution.

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

Les sources distinguent les programmes conservés dans la bibliothèque,
la zone de swap et la zone données-utilisateur.

Conséquences

Le programme courant, les objets d'exécution et les fichiers persistants
ne sont pas un seul espace abstrait.

Utilisé par

VM-1601

VM-1701

VM-2102

Observations

La géographie physique des espaces historiques n'est pas reconstruite.


------------------------------------------------------------
EXI-0050

Titre

Allocation et libération des objets d'exécution

Description

Les objets d'exécution dont la durée de vie est dynamique doivent
pouvoir être créés et libérés sans imposer une allocation permanente
pendant toute la durée de la session.

Origine

HIS-10.6

Sources historiques

[INRDP75 §4-1]

[LSE15 §3-1]

[NOYELLE89 p.219]

Statut

Documenté

Justification

Le langage permet explicitement de libérer des tableaux afin de récupérer
de l'espace.

Conséquences

La mémoire de travail doit gérer des objets dont la durée de vie est
variable.

Utilisé par

VM-1602

VM-1202

VM-1203

VM-0901

Observations

L'algorithme d'allocation historique n'est pas documenté.


------------------------------------------------------------
EXI-0051

Titre

Diagnostic immédiat des erreurs syntaxiques

Description

Lorsqu'une ligne validée présente une erreur syntaxique détectable par
le compilateur, un diagnostic est produit avant son intégration au
programme courant.

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

Les sources indiquent que l'erreur syntaxique est signalée dès l'analyse
de la ligne.

Conséquences

Le diagnostic appartient au traitement de compilation.

Utilisé par

VM-1901

VM-0302

VM-0502

Observations

Le format exact des messages n'est pas reconstruit.


------------------------------------------------------------
EXI-0052

Titre

Traitement conversationnel des demandes

Description

Toute entrée utilisateur validée doit être orientée vers le traitement
approprié selon sa nature : commande, ligne de programme, expression
immédiate ou demande adressée à un processeur.

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

Le manuel décrit l'analyse de la ligne et l'enchaînement vers le
processeur compétent.

Conséquences

La machine abstraite possède un point d'entrée conversationnel capable
d'orienter les demandes.

Utilisé par

VM-2001

VM-2002

VM-0501

VM-0702

VM-1701

Observations

Le mécanisme d'analyseur de commandes est traité comme un mécanisme
distinct.


------------------------------------------------------------
EXI-0053

Titre

Distinction des fichiers-données temporaires et permanents

Description

Le système doit distinguer les fichiers-données temporaires, associés
à l'espace de travail d'une console, des fichiers-données permanents
conservés dans la bibliothèque.

Origine

HIS-11.3

HIS-4.4.3

Sources historiques

[INRDP75 §1-2–§1-3]

[LSE15 §9-5]

Statut

Documenté

Justification

Les sources distinguent explicitement les fichiers-données temporaires
et permanents et leur associent des durées de vie et des espaces de
stockage différents.

Conséquences

Les opérations de création, accès, catalogage et suppression doivent
pouvoir distinguer ces deux catégories de fichiers-données.

Utilisé par

VM-1703
VM-1704

Observations

La représentation physique des deux espaces n'est pas spécifiée par
cette exigence.


------------------------------------------------------------
EXI-0054

Titre

Accès des fichiers-données selon leur portée

Description

L'accès à un fichier-données doit respecter sa portée : un fichier
temporaire est attaché à la console qui l'utilise, tandis qu'un fichier
permanent appartient à la bibliothèque et peut être accessible selon
les règles prévues par le système.

Origine

HIS-11.3

Sources historiques

[INRDP75 §1-3]

[LSE15 §9-5]

Statut

Documenté

Justification

Les sources distinguent l'espace temporaire propre à une console de
l'espace permanent de la bibliothèque et décrivent des règles d'accès
différentes.

Conséquences

Le modèle abstrait doit distinguer l'identité d'un fichier et sa portée
d'accès.

Utilisé par

VM-1703
VM-1704

Observations

Les mécanismes internes de protection ne sont pas reconstruits au-delà
des propriétés documentées.


------------------------------------------------------------
EXI-0055

Titre

Catalogue des fichiers

Description

Le système doit pouvoir identifier les fichiers de la bibliothèque
afin de permettre leur recherche et les opérations de gestion prévues
par le langage de commande.

Origine

HIS-11.1

HIS-11.3

Sources historiques

[INRDP75 §Fichiers]

[LSE15 §9-5]

Statut

Documenté

Justification

Les opérations de gestion des fichiers s'appuient sur l'identification
des fichiers par leur nom ; la documentation décrit également le
catalogage des fichiers.

Conséquences

La machine abstraite doit disposer d'un mécanisme logique d'identification
des fichiers et de consultation du catalogue nécessaire aux opérations
prévues.

Utilisé par

VM-1705
VM-2002

Observations

La structure interne du catalogue et l'algorithme de recherche ne sont
pas documentés.


------------------------------------------------------------
EXI-0056

Titre

Conservation et réutilisation de l'espace des fichiers temporaires

Description

La transformation d'un fichier-données temporaire en fichier-données
permanent entraîne la disparition du fichier temporaire et rend son
espace réutilisable pour d'autres fichiers temporaires.

Origine

HIS-11.3

Sources historiques

[LSE15 §9-5]

Statut

Documenté

Justification

La commande CATALOGUER est décrite comme rendant permanent un fichier
données temporaire et libérant l'espace temporaire correspondant.

Conséquences

Le modèle abstrait doit distinguer la durée de vie du fichier temporaire
et celle du fichier permanent et permettre la réutilisation de l'espace
temporaire après catalogage.

Utilisé par

VM-1703
VM-1708

Observations

La gestion interne de l'allocation de l'espace temporaire n'est pas
reconstruite.


------------------------------------------------------------
EXI-0057

Titre

Traitement des commandes portant sur les fichiers-données

Description

Le langage de commande doit permettre les opérations documentées sur
les fichiers-données, notamment CATALOGUER, SUPPRIMER, ENCODER et DECODER,
en orientant chaque opération vers les mécanismes nécessaires.

Origine

HIS-5.5

HIS-5.6

HIS-11.1

HIS-11.3

HIS-11.5

Sources historiques

[INRDP75 §3.3]

[INRDP75 §Fichiers]

[INRDP75 Annexe II]

[LSE15 §9-5]

Statut

Documenté

Justification

Les sources décrivent le bibliothécaire comme processeur des opérations
sur les fichiers et documentent les enchaînements particuliers entre
bibliothécaire, décompilateur et compilateur.

Conséquences

Le langage de commande et l'architecture abstraite doivent représenter
ces opérations sans les confondre avec les opérations portant sur les
fichiers-programmes.

Utilisé par

VM-1706
VM-1707
VM-2002

Observations

Les exigences existantes EXI-0021 et EXI-0025 couvrent déjà certaines
propriétés de ENCODER et de gestion des fichiers-programmes ; la présente
exigence couvre leur rattachement au langage de commande et aux
fichiers-données.


------------------------------------------------------------
EXI-0058

Titre

Gestion abstraite des tâches en temps partagé

Description

Le système doit pouvoir conserver l'état d'une tâche inactive et
restaurer cet état lorsqu'elle redevient active, de manière à permettre
le partage d'une machine entre plusieurs tâches utilisateurs.

Origine

HIS-4.1

HIS-4.2

Sources historiques

[INRDP75 §3.1]

[LSE15 §1]

Statut

Documenté

Justification

Les sources décrivent des tâches utilisateurs successivement actives,
des images conservées dans la zone de swap et les opérations SWAP IN et
SWAP OUT.

Conséquences

La machine abstraite doit représenter l'état d'une tâche indépendamment
de son état d'activité et permettre son activation et sa suspension.

Utilisé par

VM-2105

Observations

Cette exigence ne fixe ni algorithme d'ordonnancement ni représentation
physique de la zone de swap.


------------------------------------------------------------
EXI-0059

Titre

Moniteur de temps partagé et permutation des tâches

Description

Le système doit disposer d'un mécanisme permanent assurant l'activation
des tâches et la conservation de l'état des tâches non actives, selon
les opérations historiques de SWAP IN et SWAP OUT.

Origine

HIS-4.3

HIS-4.4.1

Sources historiques

[INRDP75 §3.1]

[INRDP75 §5-1.2]

Statut

Documenté

Justification

Le manuel décrit explicitement le système résident et le moniteur de
temps partagé comme assurant les échanges entre mémoire et zone de swap
et l'activation des tâches.

Conséquences

Le modèle abstrait doit distinguer la gestion des tâches du traitement
fonctionnel des programmes et fichiers.

Utilisé par

VM-2105

Observations

Les règles exactes de sélection des tâches ne sont pas reconstruites à
partir des sources disponibles.


------------------------------------------------------------
EXI-0060

Titre

Reprise d'une exécution conservant les données de l'interruption

Description

Une exécution interrompue peut être reprise sans réinitialiser les
valeurs acquises avant l'interruption ; la reprise peut, lorsque la
commande le permet, être effectuée à partir d'une ligne choisie.

Origine

HIS-13.1

HIS-13.3

HIS-13.4

Sources historiques

[INRDP75 §4-1]

[INRDP75 §4-3]

[INRDP75 Annexe II]

Statut

Documenté

Justification

Les mécanismes de mise au point décrivent PAS À PAS, CONTINUER et
REPRENDRE. REPRENDRE conserve les affectations antérieures et permet de
choisir une nouvelle ligne de départ dans les limites documentées.

Conséquences

L'état d'exécution doit être distinct du programme courant et conserver
les valeurs nécessaires entre l'interruption et la reprise.

Utilisé par

VM-2106

Observations

Cette exigence ne fixe pas la représentation interne de l'état conservé.
Le comportement particulier de REPRENDRE dans une procédure reste défini
par les règles historiques correspondantes.


------------------------------------------------------------
EXI-0061

Titre

Profils des implantations historiques

Description

La reconstruction doit permettre de distinguer les comportements propres aux implantations historiques documentées du L.S.E. sans dupliquer la machine abstraite commune. Les différences explicitement documentées pour le T1600 et le MITRA 15 doivent pouvoir être sélectionnées par le profil correspondant.

Origine

HIS-4.8

Sources historiques

[INRDP75 §« Remarque sur la présentation du fascicule »]

[INRDP75 §4.1]

[INRDP75 Annexe II]

Statut

Déduit.

Justification

Le manuel de 1975 documente explicitement deux systèmes L.S.E. existants, version 4 sur T1600 et version 8 sur Mitra 15, et signale des différences entre ces matériels. La reconstruction doit conserver ces différences sans présenter une variante comme une propriété générale du L.S.E.

Conséquences

Les tomes de conception doivent distinguer les propriétés communes des variantes d'implantation explicitement documentées.

Utilisé par

VM-2001

VM-2111

Observations

Le profil ne peut contenir que des différences attestées ou nécessaires à leur représentation. Il ne constitue pas une reconstitution exhaustive des implantations.



------------------------------------------------------------
EXI-0062

Titre

Gestion des identifications et des droits d'accès aux fichiers

Description

La reconstruction doit représenter une identification courante de session permettant de contrôler les opérations autorisées sur les fichiers permanents, en distinguant les droits associés à l'identification, l'identification de création du fichier et les protections d'accès documentées.

Origine

HIS-11.3.1

Sources historiques

[INRDP75 §Fichiers]

[LSE15 §9-5]

Statut

Documenté

Justification

Les sources documentent la commande IDENTIFICATION, l'identification courante, les pouvoirs associés aux numéros de code, la propriété des fichiers permanents et plusieurs protections de lecture et d'écriture. Ces propriétés doivent être prises en compte par les opérations du bibliothécaire.

Conséquences

La machine abstraite doit distinguer l'identification de session, l'identification de création d'un fichier permanent et les droits d'accès applicables à l'opération demandée.

Utilisé par

VM-1709
VM-3404
VM-3405
VM-3408

Observations

La représentation de la clé et son stockage interne sont des éléments de conception non établis par les sources. Les différences de protection entre T1600 et MITRA 15 restent rattachées aux profils historiques documentés.



============================================================
18. ÉTAT DU CATALOGUE
============================================================

Exigences consolidées : EXI-0001 à EXI-0062

Nombre total d'exigences : 62

Prochain identifiant libre : EXI-0063

Les identifiants EXI-0001 à EXI-0062 sont permanents.

Aucune exigence de cette plage ne peut être supprimée ni réattribuée.
Toute évolution ultérieure doit conserver l'identifiant existant et
modifier ou compléter l'exigence concernée conformément aux règles du
présent catalogue.


============================================================
19. GESTION DU CATALOGUE
============================================================

Le catalogue constitue un document vivant.

Les exigences sont ajoutées progressivement au fur et à mesure de la
rédaction des tomes.

Aucune exigence ne peut être supprimée.

Toute nouvelle exigence reçoit le premier identifiant libre.

Le catalogue constitue la référence unique de toutes les exigences de
la collection documentaire.
