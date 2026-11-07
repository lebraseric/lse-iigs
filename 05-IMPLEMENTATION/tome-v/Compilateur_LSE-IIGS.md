# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — Le compilateur : réception de la ligne (programme ou machine de bureau), analyse et génération en une seule passe, frontière avec le programme courant

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V (spécification détaillée complète de l'implémentation), niveau `05-IMPLEMENTATION/decisions/`
**Portée :** réalisation physique du composant compilateur lui-même — réception d'une ligne déjà reconnue comme relevant du domaine du programme (par opposition à une commande, Tome III §2.5.1), **distinction entre ligne de programme numérotée et ligne machine de bureau non numérotée** (Tome III §9.1, VM-3201), analyse syntaxique et génération de la représentation compilée en une seule passe pour l'une et l'autre forme, tampon de génération temporaire, correction différée des branchements intra-séquence, recherche/insertion dans la table des symboles, restrictions propres au mode machine de bureau, **compilation de la ligne de déclaration de procédure — liste des paramètres formels, clause `LOCAL`, élément compilé de déclaration et table des paramètres formels qui en est dérivée (§13)**, structure provisoire du diagnostic de compilation, et frontière exacte avec le gestionnaire d'objets, le programme courant et l'interpréteur. Ce chapitre ne redéfinit ni le format physique produit (RCI-M, à l'exception du seul élément de déclaration de procédure, sous-code de contrôle 0x0D, fixé en §13 et reporté dans RCI-M §4.7), ni l'allocation des blocs compilés (Gestionnaire d'objets, déjà clos), ni l'intégration d'un objet compilé accepté dans le programme courant (VM-3204, `programme.c`, chapitre distinct non encore rédigé — dont le présent chapitre fixe seulement le contrat attendu pour une ligne de déclaration de procédure, §13.5), ni l'exécution elle-même — immédiate ou programmée (VM-0702/VM-0703, `interprete.c`, chapitre distinct non encore rédigé : **le compilateur ne exécute jamais rien, y compris en mode machine de bureau**), ni l'usage de la table des paramètres formels à l'appel d'une procédure (Etat d'exécution §5), ni la réception des caractères et la détection de fin de ligne côté console (Moniteur, chapitre distinct non encore rédigé).
**Rang normatif :** subordonné à Tome III §2.3.3, §2.5.1, §7.2, §9 (VM-0501 à VM-0507, VM-0702, VM-3201 à VM-3205), à Tomes I-II §13.1 (VM-1301, EXI-0042 — définition compilée d'une procédure), à Tome IV §2.2 (IV-IF-0001), §9 (ALG-0901 à ALG-0905), Annexe F (ANN-F-0001, ANN-F-0005), Annexe I (ANN-I-0001, ANN-I-0002), à RCI-M (format cible complet de la représentation compilée, IMP-0101 à IMP-0128, et en particulier IMP-0109 — objets compilés transitoires du mode machine de bureau — et IMP-0114 — identification des procédures par numéro de ligne d'entrée), à Gestionnaire d'objets (interface `objets_creer`/`objets_resoudre`, IMP-0157 à IMP-0163) et à Architecture physique (§6.1, IMP-0132/IMP-0133 — allocation fixe de la table des symboles). Aucune disposition du présent chapitre ne peut contredire ces sources ; il se limite à préciser ce qu'elles ont explicitement laissé ouvert.
**Place dans le Tome V :** ce chapitre referme le point laissé ouvert par RCI-M §13 (« Ne couvre pas... l'algorithme de reconnaissance » — implicite dans tout le chapitre, chaque sous-format y étant défini comme cible, non comme procédé de production), par RCI-M IMP-0109 (le format cible du bloc transitoire du mode machine de bureau est fixé, mais pas l'algorithme qui le distingue et le produit — refermé ici, §4 à §11), par RCI-M §6.3 (IMP-0114 fixe l'entrée `SYM_PROCEDURE`, mais ni la représentation compilée de la ligne `PROCEDURE &ID(...) LOCAL ...` elle-même, ni le support physique des paramètres formels et de la clause `LOCAL` — refermé ici, §13) et par Organisation du dépôt §2.1 (qui localise seulement le module dans `src/noyau/compilateur.c/.h`, sans en préciser la réalisation interne, IMP-0136). Sa position exacte dans le sommaire final du Tome V n'est pas fixée par ce document.

---

## 0. Avertissement de statut

Comme pour RCI-M, Architecture physique, Organisation du dépôt, État d'exécution et Gestionnaire d'objets, aucune disposition du présent chapitre ne constitue une reconstruction historique. Tome III (§9, préambule) le dit explicitement pour ce composant précis : « la séparation fonctionnelle ne signifie pas que ces fonctions doivent être réalisées par des modules indépendants dans une implémentation. Elle signifie qu'elles possèdent des responsabilités et des interfaces distinctes. » Tomes I-II (VM-0502, « Implémentation ») le confirme : « une réalisation peut fusionner plusieurs étapes internes sans modifier le comportement abstrait ». Tout ce qui suit — fusion de l'analyse et de la génération en une seule passe, algorithme de distinction entre les deux formes de ligne, taille et format du tampon de génération, algorithme de correction différée des offsets, algorithme de recherche dans la table des symboles, format de l'élément de déclaration de procédure et de la table des paramètres formels, algorithme d'analyse de la clause `LOCAL`, structure du diagnostic — est donc systématiquement une **décision d'implémentation (`IMP-*`)**, jamais une propriété logique de RCI-M, du Tome III ou du Tome IV, ni une reconstruction de l'algorithme historique de compilation, dont RCI-M §13 rappelle qu'il « n'est pas documenté ».

---

## 1. Rappel des contrats obligatoires (non renégociables)

### 1.1 Exigences EXI directement concernées

| EXI | Objet | Conséquence physique imposée |
|---|---|---|
| EXI-0001 | Compilation indépendante des lignes | Aucune structure interne du compilateur ne conserve d'état entre deux appels à `compilateur_compiler` (§9) ; en particulier, aucune information relative à une ligne FAIRE n'est mémorisée d'une compilation à l'autre (RCI-M §10, IMP-0112) |
| EXI-0004 | Rejet atomique d'une ligne incorrecte | Toute écriture de code généré a lieu dans un tampon temporaire (§6) jamais exposé au programme courant avant succès complet (§9) ; vaut également pour une ligne machine de bureau rejetée (§8, aucun bloc transitoire créé) |
| EXI-0011 / EXI-0047 | Ordre post-fixé des expressions | La génération en une seule passe (§5) doit produire les opérandes avant l'opération qui les consomme, par construction de la descente récursive plutôt que par un réordonnancement a posteriori |
| EXI-0016 | Compacité de la représentation compilée | Le tampon de génération (§6) et la table des symboles (§7) ne portent, l'un comme l'autre, aucune information reconstructible autrement que celle déjà fixée par RCI-M |
| EXI-0018 | Analyse syntaxique préalable à la compilation | Le tampon de génération n'est proposé au gestionnaire d'objets (§9, étape 7) qu'après que l'analyseur a intégralement reconnu la ligne sans erreur, quelle que soit sa forme (programme ou machine de bureau) |
| EXI-0019 | Distinction erreurs de compilation / d'exécution | Le compilateur ne tente jamais de résoudre, au sens de leur existence effective, les références aux symboles (§7) : il leur attribue un identifiant, sans vérifier qu'un objet d'exécution existera réellement à ce nom au moment considéré |
| EXI-0020 | Production d'une représentation compilée par ligne | Une compilation réussie d'une ligne de programme produit toujours exactement une `EntreeObjetCompile` (RCI-M §5), jamais une séquence répartie sur plusieurs objets |
| EXI-0021 | Compilation uniforme des lignes indirectes | Le point d'entrée `compilateur_compiler` (§4, §11) est unique et ignore la provenance physique de la ligne |
| EXI-0022 | Indépendance d'exécution vis-à-vis du texte source | Le texte de la ligne n'est conservé nulle part au-delà de la durée de l'appel de compilation (§4, §6) |
| EXI-0026 | Prise en compte immédiate des lignes acceptées | Le compilateur transmet son résultat au programme courant de façon synchrone, sans file d'attente ni traitement différé (§12) |
| **EXI-0028** | **Enchaînement immédiat compilation-exécution (mode machine de bureau)** | **Le compilateur produit, pour une ligne machine de bureau, un bloc transitoire distinct d'une `EntreeObjetCompile` de programme (§4, §8) ; il ne l'exécute jamais lui-même (Note 1, §0) — cette exécution relève exclusivement de l'interpréteur, invoqué par le moniteur immédiatement après un appel réussi à `compilateur_compiler`** |
| EXI-0045 / EXI-0046 | Compilation des instructions en opérations exécutables et contrôle du flux | La correction différée des offsets (§6) garantit que tout branchement produit par le générateur cible effectivement l'élément voulu avant que le tampon ne quitte le compilateur ; en mode machine de bureau, aucune instruction de contrôle de flux à numéro de ligne n'est produite, ce type d'instruction y étant interdit (§8) |
| EXI-0051 | Diagnostic immédiat des erreurs syntaxiques | Toute sortie en échec de `compilateur_compiler` renseigne la structure `DiagnosticCompilation` (§8) — y compris le rejet d'une instruction interdite en mode machine de bureau |
| EXI-0010 / EXI-0015 | Représentation commune exécution/décompilation ; restitution textuelle déterministe | La ligne `PROCEDURE &ID(...) LOCAL ...` compile un élément de déclaration (§13, IMP-0174) qui porte, à lui seul, tout ce que le décompilateur doit restituer de la déclaration, y compris l'ordre textuel de la liste `LOCAL` ; aucune structure auxiliaire n'est nécessaire à la décompilation |
| EXI-0042 | Représentation compilée des procédures et fonctions | La déclaration crée une définition compilée (VM-1301, « Conséquences — Compilation ») : identificateur, liste ordonnée des paramètres, règle de passage de chacun, objets locaux — tous portés par l'élément de déclaration (§13, IMP-0174) |
| EXI-0061 | Profils des implantations historiques | Le compilateur admet ou rejette les constructions propres au T1600 version 4 ou au MITRA 15 version 8 selon le profil choisi au lancement (§8 bis, IMP-0180) |
| EXI-0052 | Traitement conversationnel des demandes | Le compilateur ne décide jamais qu'une entrée est une commande : cette classification reste un préalable assuré par l'analyseur de commandes (§4), hors périmètre de ce chapitre ; il décide en revanche, lui-même, si une entrée ainsi reconnue comme relevant du domaine du programme est une ligne numérotée ou une ligne machine de bureau (VM-3201, §4) |

### 1.2 Responsabilité couverte (Tome III §2.3.3, §2.5.1, §7.2, §9 — VM-0501 à VM-0507, VM-0702, VM-3201 à VM-3205)

Tome III (§2.3.3) fixe la responsabilité logique : le compilateur « effectue l'analyse syntaxique, la génération de la représentation compilée, la production d'un objet compilé, la production d'un diagnostic en cas d'échec » et « ne modifie pas directement le programme courant ». Tome III (§2.5.1, Interface de compilation) en fixe le contrat entrée/sortie, repris tel quel par Tome IV IV-IF-0001. Tome III (§9) détaille ce composant en cinq mécanismes : VM-3201 (réception d'une ligne), VM-3202 (analyseur), VM-3203 (générateur), VM-3204 (intégration au programme courant — **hors périmètre de ce chapitre**, cf. §12, et qui ne s'applique de toute façon qu'à une ligne de programme), VM-3205 (compilation depuis une source indirecte). Tome III (§7.2, VM-0702) fixe par ailleurs le mécanisme abstrait d'exécution immédiate du mode machine de bureau : « une expression ou une opération introduite dans ce mode est compilée, interprétée puis exécutée dès la réception de la fin de ligne » — **compilée par le compilateur, puis interprétée et exécutée par l'interpréteur** : deux composants distincts, enchaînés par le moniteur, jamais fusionnés (Note 1, §0).

Le présent chapitre réalise physiquement VM-3202, VM-3203 et VM-3205 intégralement, VM-3201 **en totalité pour la partie qui relève effectivement du module `compilateur.c`** selon la correspondance physique d'Organisation du dépôt §2.1 — c'est-à-dire la réception d'une ligne déjà accumulée et déjà reconnue comme relevant du domaine du programme, **et la distinction entre ligne de programme et ligne machine de bureau elle-même**, que VM-3201 énumère explicitement parmi les responsabilités du compilateur (« distingue ligne de programme et entrée immédiate »). Seules l'accumulation de caractères et l'écho console restent hors périmètre, relevant du module `moniteur.c` (chapitre distinct, non encore rédigé).

Il réalise en outre la partie « Compilation » de VM-1301 (Tomes I-II §13.1 : « La déclaration crée une définition compilée »), dont la reconstruction retenue énumère abstraitement le contenu — « un identificateur ; une liste de paramètres ; un ensemble de règles de passage des paramètres ; un corps compilé ; une définition des objets locaux ; une règle de retour » : l'identificateur, la liste de paramètres, les règles de passage et la définition des objets locaux sont produits par la compilation de la ligne de déclaration (§13) ; le corps compilé est constitué des objets compilés ordinaires des lignes qui suivent (RCI-M §6.3, IMP-0114) ; la règle de retour est portée par les éléments `RETOUR`/`RETOUR EN`/`RESULTAT` (RCI-M §4.7).

### 1.3 Points explicitement laissés ouverts par les chapitres précédents

- RCI-M, tout le chapitre : fixe exhaustivement la **cible** produite par le générateur (format des éléments, de `EntreeObjetCompile`, de `EntreeSymbole`) sans jamais prescrire l'algorithme qui la produit — RCI-M §13 le confirme a contrario en ne mentionnant aucune limite sur ce point : le silence de RCI-M sur l'algorithme de compilation n'est pas un oubli mais une conséquence de son périmètre (format, non procédé).
- RCI-M §3 bis (IMP-0109) : fixe que l'objet compilé d'une ligne machine de bureau est « construit dans un bloc transitoire isolé... transmis directement à l'interpréteur, puis libéré après exécution », sans fixer ni l'algorithme qui reconnaît qu'une ligne est de cette forme, ni celui qui produit ce bloc — les deux sont refermés ici (§4, §8, §9).
- RCI-M §6.3 (IMP-0114) : fixe l'entrée `SYM_PROCEDURE` de la table des symboles (numéro de ligne d'entrée, arité) et précise que « les identificateurs locaux et les paramètres formels eux-mêmes... ne sont pas portés par `EntreeSymbole` », sans fixer ni la représentation compilée de la ligne `PROCEDURE &ID(...) LOCAL ...` elle-même, ni le support physique de la liste des paramètres formels, de leur mode de passage (`LOCAL` ou référence) et des identificateurs `LOCAL` non-paramètres. Or l'interpréteur a besoin de ces informations à chaque appel (Etat d'exécution §5.5) et le décompilateur doit pouvoir restituer la déclaration sans texte source (EXI-0010, EXI-0015, EXI-0022). Ce point est refermé ici, en totalité, au §13 : élément compilé de déclaration (IMP-0174), table des paramètres formels qui en est dérivée (IMP-0147), algorithme d'analyse de la ligne de déclaration et de sa clause `LOCAL` (IMP-0175), frontière avec le programme courant (IMP-0176). Une ligne de déclaration ne peut jamais apparaître en mode machine de bureau (§8).
- Architecture physique §6.1 (IMP-0132) : classe la table des symboles parmi les « allocations fixes » (`malloc`/`realloc`), sans fixer l'algorithme de recherche ni la politique de croissance au-delà de « un redimensionnement se traite par `realloc` classique ».
- Organisation du dépôt §2.1 : localise le module (`src/noyau/compilateur.c/.h`, segment `"compilateur"`) mais, de son propre aveu, ne fait que le **localiser**.
- `LSE15Mitra.g4` (grammaire reconstruite, dérivée du corpus historique, non elle-même une source historique de premier rang) propose déjà, dans ses commentaires normatifs [J] et [K], une analyse grammaticale complète de la distinction entre `line` et `bureauLine` (restrictions, récursivité, synonymie `?`/`AFFICHER`) : le présent chapitre s'appuie sur cette analyse pour les décisions de réalisation (§8), sans la remettre en cause ni en reconstituer l'origine historique, qui reste de la responsabilité du corpus documentaire Niveau A/B (conditionnement §22).

Le présent chapitre referme l'ensemble de ces points.

### 1.4 Interdictions du conditionnement (§9, §19), reprises de RCI-M §1.4

Les pointeurs C ne constituent pas un modèle architectural. Toute référence à un symbole ou à une ligne, produite par le compilateur, reste un identifiant logique (index 16 bits, numéro de ligne 1 octet) au sens déjà fixé par RCI-M — jamais une adresse. Le compilateur ne construit ni ne conserve d'arbre syntaxique chaîné par pointeurs (§5) : c'est une conséquence directe de cette interdiction autant que du principe d'économie de mémoire du conditionnement (§4.1).

---

## 2. Contraintes de plateforme retenues (ORCA/C 2.2.1 / 65816)

Ce chapitre reprend intégralement RCI-M §2 (tailles des types C, absence de bourrage de structure) et Gestionnaire d'objets §2 (Memory Manager toolbox pour tout objet de durée de vie dynamique). Il ajoute deux éléments spécifiques :

- ORCA/C 2.2.1 ne fournit ni générateur d'analyseur syntaxique (pas d'équivalent `yacc`/`bison` documenté dans le corpus technique de plateforme, §22 du conditionnement) ni bibliothèque d'expressions régulières. L'analyseur du présent chapitre est donc nécessairement un programme C écrit à la main, et non un automate généré — ce qui oriente directement le choix retenu au §5 (descente récursive), la technique la plus directement transposable en C ANSI sans outil externe.
- ORCA/C alloue la pile d'appels C du compilateur lui-même dans le même espace que le reste du runtime (manuel ORCA/C, modèle mémoire, chap. 4) ; une profondeur de récursivité proportionnelle à la profondeur d'imbrication *syntaxique* d'une ligne (parenthésage d'expression, imbrication de `SI`) doit donc rester bornée par la limite de 80 caractères par ligne (INRDP75, cf. RCI-M IMP-0125) — une ligne imbrique au plus 38 niveaux de parenthèses (`?` suivi de 38 parenthèses ouvrantes, d'un chiffre et de 38 fermantes : 78 caractères), ce qui borne mécaniquement la profondeur de récursion de l'analyseur sans qu'aucune limite explicite soit codée en dur (§5). Mesurée le 2026-10-29, cette récursion coûte 238 octets de pile par niveau de parenthèses, 284 par fonction imbriquée et 302 par indice imbriqué, soit au pire 11 021 octets sous `main`. Ce pire cas dépasse la pile de 4 Kio du lanceur ; le programme `LSE` reçoit donc une pile de 16 Kio (Construction IMP-0272). Cette borne s'applique identiquement aux deux formes de ligne (§4) : le mode machine de bureau ne relâche ni n'alourdit cette contrainte, puisqu'il reste soumis à la même limite de 80 caractères (INRDP75, « Mode machine de bureau », LSE15 §9-5 : même convention de fin de ligne `X-OFF`).

---

## 3. Architecture physique générale du compilateur

```text
COMPILATEUR (module noyau/compilateur.c)
│
├── POINT D'ENTRÉE UNIQUE                    compilateur_compiler() (§4, §9)
│      — reçoit une ligne déjà validée et déjà reconnue, par l'analyseur
│        de commandes, comme relevant du domaine du programme (Tome III
│        §2.5.1, précondition) — c'est-à-dire ni une commande du moniteur
│        ni une identification, etc. ;
│      — distingue ELLE-MÊME, à l'intérieur de ce domaine, ligne de
│        programme numérotée et ligne machine de bureau non numérotée
│        (VM-3201, §4, IMP-0171) ;
│      — ne lit ni n'écrit jamais directement sur la console (§4) ;
│      — n'exécute jamais rien (§0, Note 1).
│
├── ANALYSEUR-GÉNÉRATEUR (une seule passe)    analyser_generer_ligne() (§5)
│      descente récursive, une fonction par production de la grammaire
│      reconstruite (LSE15Mitra.g4) ; chaque fonction, en reconnaissant
│      sa construction, écrit directement dans le tampon de génération
│      ci-dessous — aucune structure syntaxique intermédiaire n'est
│      matérialisée (IMP-0164). Un paramètre `mode_bureau` traverse
│      l'ensemble de la descente (IMP-0172), réalisant la restriction
│      récursive des instructions de contrôle de séquence en mode
│      machine de bureau (grammaire [J]/[K]) et la reconnaissance de `?`
│      comme synonyme strict d'AFFICHER dans ce seul mode (IMP-0173).
│      Une déclaration `PROCEDURE &ID(...) LOCAL ...`, reconnue en tête
│      de ligne de programme uniquement, est traitée par
│      analyser_generer_declaration_procedure() (§13, IMP-0175), qui
│      écrit l'élément de déclaration 0x0D (IMP-0174) en tête du tampon.
│
├── TAMPON DE GÉNÉRATION                      unsigned char tampon[255] (§6, IMP-0165)
│      — recevoir la séquence RCI-M en cours de construction, qu'elle
│        provienne d'une ligne de programme ou d'une ligne machine de
│        bureau : le format des éléments eux-mêmes ne diffère jamais
│        selon la forme de la ligne (RCI-M §3 bis) ;
│      — les branchements internes (0x00/0x01) y sont patchés a posteriori
│        (§6, IMP-0166) une fois leur cible connue.
│
├── TABLE DES SYMBOLES DU PROGRAMME COURANT   recherche/insertion (§7, IMP-0167)
│      — allocation fixe par realloc (Architecture physique IMP-0132/0133) ;
│      — jamais réinitialisée entre deux lignes (persiste pour tout le
│        programme, à la différence du tampon de génération) ;
│      — consultée et alimentée identiquement par une ligne de programme
│        et par une ligne machine de bureau (RCI-M §3 bis, IMP-0109,
│        dernier alinéa : « une ligne machine de bureau peut néanmoins
│        créer ou référencer des symboles »).
│
├── DIAGNOSTIC DE COMPILATION                 DiagnosticCompilation (§8, IMP-0168)
│      structure provisoire, forme définitive : gestionnaire des erreurs
│      (chapitre distinct, non encore rédigé) ; porte désormais aussi les
│      codes propres au rejet d'une construction interdite en mode
│      machine de bureau (§8).
│
└── FINALISATION                              (§9, étapes 7-8 ; §12)
       appelle objets_creer() (Gestionnaire d'objets §11) pour matérialiser
       le tampon accepté en HandleObjet — pour une ligne de programme,
       construit l'EntreeObjetCompile correspondante (RCI-M §5) et la
       transmet à l'interface du programme courant (VM-3204, IV-IF-0002
       INSERER_LIGNE) ; pour une ligne machine de bureau, produit
       directement le bloc transitoire (RCI-M IMP-0109) sans jamais le
       proposer au programme courant — dans les deux cas sans jamais
       manipuler elle-même la table des objets compilés, et sans jamais
       appeler l'interpréteur (§0, Note 1 ; §12).
```

**Rappel des frontières déjà fixées.** Le tampon de génération n'est *jamais* résolu en `HandleObjet` avant la fin complète et réussie de l'analyse-génération (§9) — c'est la réalisation directe de l'invariant déjà posé par Tome III §9.2 (VM-3202) : « aucune représentation compilée partielle ne devient le programme courant ». La table des symboles, en revanche, *peut* recevoir de nouvelles entrées avant que la ligne entière ne soit acceptée (§7) : ce n'est pas une violation de cet invariant, puisqu'une entrée de symbole non encore référencée par aucun objet compilé accepté n'est observable par aucun autre composant (ni l'interpréteur, ni le décompilateur, qui ne parcourent jamais la table des symboles autrement qu'à partir d'une référence effectivement présente dans un objet compilé du programme courant, ou, pour une ligne machine de bureau, dans le bloc transitoire remis à l'interpréteur) — un symbole créé pour une ligne finalement rejetée reste simplement une entrée inerte, sans coût fonctionnel, seulement un coût mémoire mineur (§16).

---

## 4. Interface d'entrée : `compilateur_compiler` et distinction des deux formes de ligne (VM-3201)

Tome III §9.1 (VM-3201) décrit la réception d'une ligne comme : « reçoit les caractères de la ligne, reconnaît la fin de validation, distingue ligne de programme et entrée immédiate, transmet les données au traitement approprié. » La correspondance physique d'Organisation du dépôt §2.1 place cependant l'accumulation de caractères et l'écho console dans `moniteur.c`/`console_iigs.c` — chapitre distinct, non encore rédigé. Le module `compilateur.c` réalise donc, de VM-3201, toutes les étapes qui ne relèvent pas spécifiquement de la console : il reçoit une ligne **déjà** accumulée et **déjà** reconnue comme relevant du domaine du programme (par opposition à une commande du moniteur), mais c'est lui, et lui seul, qui **distingue** ensuite, à l'intérieur de ce domaine, une ligne de programme numérotée d'une ligne machine de bureau — exactement la responsabilité que VM-3201 énumère en propres termes.

**IMP-0171** — *Point d'entrée unique ; distinction entre ligne de programme et ligne machine de bureau par examen du premier caractère significatif*
- Sujet : VM-3201 exige que le compilateur « distingue ligne de programme et entrée immédiate ». Faut-il, pour cela, un second point d'entrée dédié au mode machine de bureau (par exemple `compilateur_compiler_bureau`, distinct de `compilateur_compiler_ligne`), ou un point d'entrée unique qui opère lui-même cette distinction ?
- Choix : un point d'entrée unique, `compilateur_compiler`, qui commence par examiner le premier caractère non-espace du texte reçu. S'il s'agit d'un chiffre décimal, la ligne est traitée comme une ligne de programme : le chiffre et ceux qui suivent sont reconnus comme le numéro de ligne (RCI-M IMP-0125 ; de 1 à 250 sous le profil MITRA 15, de 1 à 255 sous le profil T1600, §8 bis), puis le reste de la ligne suit la grammaire `line`. Dans tous les autres cas (lettre, `?`, parenthèse, etc.), la ligne est traitée comme une ligne machine de bureau, suivant la grammaire `bureauLine` — sans numéro, et avec les restrictions du §8. Le résultat produit (§9) porte lui-même un champ `type` qui indique sans ambiguïté laquelle des deux formes a été reconnue, de sorte que l'appelant (moniteur) n'a jamais à reconstituer cette information par ses propres moyens.
- Alternatives envisagées :
  1. Deux fonctions d'entrée distinctes, le choix entre les deux étant fait par le moniteur avant l'appel (par exemple en inspectant lui-même le premier caractère) — écarté : cela reviendrait à dupliquer, dans `moniteur.c`, exactement la logique que VM-3201 attribue explicitement au compilateur, créant une dépendance de `moniteur.c` envers une connaissance de bas niveau (« un chiffre en tête signifie ceci ») qui n'a aucune raison d'exister ailleurs que dans le compilateur lui-même — et romprait, de fait, l'unicité du compilateur que le présent chapitre doit garantir (comme pour ENCODER, §11, IMP-0169) : deux points d'entrée, même proches, ouvrent la porte à une divergence future entre les deux chemins.
  2. Un paramètre explicite (`int forcer_bureau`) transmis par l'appelant pour indiquer la forme attendue, plutôt qu'une distinction interne — écarté pour la même raison : le moniteur n'a historiquement aucune information *a priori* sur la forme de la ligne avant de l'avoir lue (l'utilisateur tape indifféremment l'une ou l'autre au même prompt moniteur, INRDP75 « Mode machine de bureau » ; LSE15 §9-5) — exiger ce paramètre reviendrait à exiger que l'appelant devine ce que seul le contenu de la ligne détermine.
- Justification : le corpus historique fixe lui-même ce critère de distinction de façon univoque — toute ligne de programme commence, par construction (INRDP75, « Composition d'une ligne » : « un numéro compris entre 1 et 250/255 »), par son numéro, et ce numéro est nécessairement une suite de chiffres décimaux ; aucune construction valide du mode machine de bureau (`bureauStatementList`, grammaire `LSE15Mitra.g4`, commentaire [J]) ne peut commencer par un chiffre — une expression arithmétique machine de bureau s'introduit soit par `?` (LSE15 §9-5), soit par le nom d'une variable suivi de `←` (affectation), jamais par une constante numérique nue en tête de ligne. Le critère « premier caractère non-espace est un chiffre décimal » est donc à la fois nécessaire et suffisant, et ne requiert aucune heuristique supplémentaire.
- Impact mémoire : nul — un test sur un seul caractère, avant toute allocation.
- Impact disque : sans objet.
- Impact performances : O(1), un test de type de caractère (`isdigit`, disponible en C ANSI, `<ctype.h>`).
- Impact historique : réalise fidèlement VM-3201 et préserve la distinction documentée par HIS-3.7/HIS-3.8 entre les deux modalités, sans en introduire de troisième ni en fusionner aucune.
- Références normatives : Tome III §9.1 (VM-3201) ; `LSE15Mitra.g4`, règles `line`/`bureauLine`, commentaire [J] ; INRDP75, « Composition d'une ligne », « Mode machine de bureau » ; LSE15 §9-5.
- Statut : retenu.

```c
/* noyau/compilateur.h */

typedef enum {
    LSE_LIGNE_PROGRAMME = 0,   /* numérotée : résultat porte une EntreeObjetCompile complète (RCI-M §5) */
    LSE_LIGNE_BUREAU    = 1    /* non numérotée : résultat porte un bloc transitoire (RCI-M IMP-0109) */
} TypeLigneCompilee;

typedef struct {
    NatureErreur   nature;     /* nature d'erreur du catalogue (erreurs.h) :
                                   chapitre Gestionnaire des erreurs §7, §8, IMP-0195 */
    unsigned char  position;   /* position dans la ligne source, 0-79 (IMP-0191) */
} DiagnosticCompilation;

typedef struct {
    TypeLigneCompilee  type;
    union {
        EntreeObjetCompile ligne_programme;   /* type = LSE_LIGNE_PROGRAMME, RCI-M §5 */
        HandleObjet        bloc_bureau;       /* type = LSE_LIGNE_BUREAU — longueur disponible
                                                  via objets_resoudre + la valeur retournée
                                                  à l'étape 7 de l'algorithme, §9 ; jamais
                                                  inséré dans la table des objets compilés */
    } resultat;
} ResultatCompilation;

/* IV-IF-0001 / VM-3201 : réalise ALG-0901 et la distinction de forme (IMP-0171).
   Précondition (Tome III §2.5.1) : `texte` relève déjà du domaine du programme
   (l'analyseur de commandes a écarté les commandes du moniteur) ; `longueur`
   ne dépasse jamais 79 : INRDP75 limite la ligne à 80 caractères « y compris le caractère de fin
   de ligne », lequel est consommé par le moniteur et ne figure pas dans `texte`.
   Postcondition de succès : *resultat est entièrement renseigné selon son
   `type` ; aucune autre structure du programme courant n'est modifiée par
   cet appel (VM-3204 reste hors périmètre pour une ligne de programme ;
   une ligne machine de bureau ne touche jamais le programme courant, §12).
   Postcondition d'échec : *resultat n'est pas modifié ; *diagnostic est
   renseigné ; aucun objet compilé ni bloc transitoire n'a été créé (§9, étape 8) ;
   une constante partagée créée en cours d'analyse subsiste, inerte (IMP-0181). */
int compilateur_compiler(const char *texte, unsigned int longueur,
                          ResultatCompilation *resultat,
                          DiagnosticCompilation *diagnostic);
```

**Réalisation (2026-10-23).** `ResultatCompilation` comporte en outre un champ `longueur_bureau`, renseigné pour une ligne machine de bureau : le gestionnaire d'objets ne rend pas la longueur d'un objet (Gestionnaire d'objets §11, IMP-0194), et l'interpréteur doit connaître celle du bloc transitoire pour le parcourir. La flèche d'affectation est le caractère 95 (`_`) et l'exponentiation le caractère 94 (`^`) du texte reçu, comme dans la restitution (fiche de codage RCI-M). La structure `DiagnosticCompilation` est celle d'IMP-0195.

Le paramètre `texte` est une chaîne C ordinaire, propriété de l'appelant (moniteur ou bibliothécaire, §11) : le compilateur ne la conserve ni ne la modifie au-delà de la durée de l'appel — réalisation directe d'EXI-0022 (le texte source n'est jamais une donnée conservée par le compilateur lui-même) et de l'interdiction du conditionnement §9 sur les pointeurs conservés au-delà de leur durée de validité.

---

## 5. Analyse et génération en une seule passe (VM-3202 / VM-3203)

**IMP-0164** — *Analyse et génération fusionnées en une seule passe par descente récursive, sans arbre syntaxique intermédiaire matérialisé*
- Sujet : Tome III §9.1 (VM-3202) et §9.3 (VM-3203) distinguent logiquement l'analyse syntaxique (production d'une « structure logique ») et la génération (production de la représentation compilée à partir de cette structure). Faut-il matérialiser physiquement cette structure intermédiaire (un arbre syntaxique chaîné par pointeurs, ou une liste de nœuds) avant de la parcourir pour générer le code, ou fusionner les deux étapes ?
- Choix : une seule passe. Chaque fonction de l'analyseur correspond directement à une production de la grammaire reconstruite (`LSE15Mitra.g4`) — une expression, un facteur, une instruction, une liste de paramètres — et, en reconnaissant sa construction, écrit **immédiatement** les éléments RCI-M correspondants dans le tampon de génération (§6), par appel direct aux fonctions `rcim_ecrire_*` du module `rcim.c` (RCI-M §4). Aucune structure de nœuds n'est allouée : la structure syntaxique de la ligne n'existe, à un instant donné, que sous la forme de la pile d'appels C elle-même (une fonction active par niveau d'imbrication syntaxique non encore réduit). Cette organisation s'applique identiquement aux deux formes de ligne (§4) : `analyser_generer_instructions` (appelée pour `statementList`) et `analyser_generer_instructions_bureau` (appelée pour `bureauStatementList`, §8) partagent la totalité des fonctions de reconnaissance d'expression, de facteur et d'opérande, seule la liste des productions d'instruction autorisées différant entre les deux (§8, IMP-0172).
- Alternatives envisagées :
  1. Analyseur produisant un arbre syntaxique complet, générateur séparé le parcourant ensuite — écarté : imposerait une allocation dynamique par nœud (`malloc` d'une structure de taille variable pour chaque expression, chaque instruction), alors qu'une ligne de 80 caractères ne peut de toute façon contenir qu'un nombre très borné de nœuds ; le conditionnement §4.1 (« éviter les allocations massives inutiles ») et le principe VM-0502 (« une réalisation peut fusionner plusieurs étapes internes sans modifier le comportement abstrait ») autorisent et recommandent explicitement cette fusion. Une telle structure devrait de plus être entièrement libérée après chaque ligne, qu'elle soit acceptée ou non, sans aucun bénéfice fonctionnel puisqu'elle ne survit jamais au-delà de l'appel de compilation (EXI-0022).
  2. Analyseur produisant une forme intermédiaire tabulaire (jetons pré-découpés, table de lexèmes) suivie d'une génération séparée — écarté pour une raison symétrique : la grammaire du L.S.E. reconstruite ne requiert aucun découpage lexical préalable distinct de l'analyse syntaxique elle-même (pas de mots-clés ambigus nécessitant un examen contextuel, chaque instruction étant introduite par un mot-clé ou un identificateur reconnaissable au premier caractère) ; une table de jetons intermédiaire n'apporterait donc, ici encore, aucun bénéfice mesurable.
- Justification : ce choix applique directement le principe de simplicité et d'économie de mémoire du conditionnement (§3, priorités 6 et 7) à un point que le Tome III lui-même signale comme laissé ouvert (« la séparation fonctionnelle ne signifie pas que ces fonctions doivent être réalisées par des modules indépendants »). Il reste conforme à EXI-0011/EXI-0047 (ordre post-fixé) parce que l'ordre d'écriture dans le tampon découle directement de l'ordre de retour des appels récursifs : une fonction qui analyse une opération binaire appelle d'abord la reconnaissance de son opérande gauche (qui écrit sa propre sous-séquence), puis celle de son opérande droit, puis écrit elle-même l'opérateur — l'ordre post-fixé est donc une conséquence mécanique de la structure de la descente récursive elle-même, jamais un réordonnancement a posteriori d'une structure intermédiaire. Le partage intégral des fonctions d'expression entre les deux formes de ligne (grammaire `LSE15Mitra.g4`, commentaire [J] : « les expressions... sont IDENTIQUES pour le reste ») garantit en outre qu'aucune divergence de comportement ne peut apparaître entre une expression évaluée en ligne de programme et la même expression évaluée en mode machine de bureau.
- Impact mémoire : élimine toute allocation dynamique par nœud syntaxique ; le seul coût est celui de la pile d'appels C, borné par la profondeur de récursion (§2) et par le tampon de génération lui-même (§6), tous deux de taille fixe et connue à la compilation. Mesuré le 2026-10-29 : 238 octets par niveau de parenthèses, et 11 021 octets au pire sous `main`, d'où la pile de 16 Kio du programme `LSE` (Construction IMP-0272).
- Impact disque : sans objet.
- Impact performances : élimine une passe de parcours complète (celle qui, dans une architecture à deux passes, relierait l'arbre au tampon) ; le compilateur ne parcourt le texte de la ligne qu'une seule fois de gauche à droite (hors retours en arrière ponctuels pour la correction différée des offsets, §6, qui n'impliquent aucune relecture du texte source).
- Impact historique : neutre — aucune propriété logique du Tome II ou du Tome III n'exige de structure intermédiaire matérialisée ; VM-0502 le confirme explicitement pour ce point précis.
- Références normatives : Tome III §9.1, §9.2 (VM-3201, VM-3202) ; Tomes I-II §5.2 (VM-0502, « Implémentation ») ; conditionnement §3 (priorités), §4.1, §9 ; `LSE15Mitra.g4`, commentaire [J].
- Statut : retenu.

**Organisation interne.** Chaque fonction d'analyse suit la même convention : elle reçoit un curseur mutable sur la position courante dans `texte` (un simple `unsigned int *position`, jamais une copie du texte), tente de reconnaître sa production, et renvoie un indicateur de succès/échec ; en cas d'échec, elle renseigne `*diagnostic` (§8) sans avoir modifié le tampon de génération au-delà de ce qu'elle a elle-même déjà écrit — la remontée d'échec jusqu'à `compilateur_compiler` (§9) est ce qui garantit que rien de ce contenu partiel n'est jamais transmis plus loin, conformément à l'invariant déjà cité au §3.

```text
analyser_generer_expression(texte, position, tampon, curseur_tampon, mode_bureau, diagnostic)
1. analyser_generer_terme(...)                { premier opérande, EXI-0011 }
2. TANT QUE le caractère courant est un opérateur binaire de même niveau
       analyser_generer_terme(...)             { opérande suivant, avant l'opération }
       rcim_ecrire_operation(tampon, curseur_tampon, code_operateur)
   FIN TANT QUE
3. RETOURNER succès
```

`mode_bureau` est transmis sans action propre par `analyser_generer_expression` elle-même (une expression a rigoureusement la même forme dans les deux modes, §5 ci-dessus) : il n'est consulté que par les fonctions de reconnaissance d'instruction et d'opérande d'appel (§8), auxquelles il est systématiquement propagé à travers tous les niveaux d'imbrication.

**Précédence et associativité.** L'ordre postfixé produit par la descente récursive (IMP-0164) applique les règles suivantes, qui sont des faits du corpus et non des choix d'implémentation :

- **Rangs arithmétiques** (LSE15 §3-3.4 ; INRDP75, « Ordre d'exécution dans les opérations ») : « 1) appel de fonction ; 2) exponentiation ; 3) multiplication et division ; 4) addition et soustraction, changement de signe ». Le changement de signe est au même rang que l'addition et la soustraction : `-A↑2` vaut `-(A↑2)` et se compile `A`, `2`, `↑`, changement de signe ; l'exemple de LSE15 §3-3.4 `-A+3/DELTA*ALPHA-X↑ABS(D)-1` se calcule comme `((-A)+((3/DELTA)*ALPHA))-(X↑(ABS(D)))-1`.
- **Associativité** : « Dans une suite d'opérations de même valeur hiérarchique où ne figure aucune parenthèse, les calculs sont effectués de gauche à droite » (LSE15 §3-3.4 ; INRDP75 : « À priorité égale, les opérations se font de gauche à droite »), y compris pour l'exponentiation : `2↑3↑2` se compile `2`, `3`, `↑`, `2`, `↑` et vaut 64 (RCI-M §4.11.2, sous-code 0x05). Chaque niveau de la descente récursive réalise cette associativité par une boucle qui écrit chaque opérateur aussitôt après son opérande droit, comme le fait `analyser_generer_expression` ci-dessus.
- **Opérateurs adjacents** : « Une expression ne doit pas contenir deux opérateurs adjacents. Ainsi, `A*-B` est incorrect et doit s'écrire `A*(-B)` » (LSE15 §3-3.3 ; INRDP75) : le changement de signe n'est reconnu qu'en tête d'une suite d'additions et de soustractions (règle `signedFactor` de la grammaire reconstruite), jamais comme opérande direct d'un opérateur de rang supérieur ; `A*-B` produit `ERR_SYNTAXE`.
- **Repli sur une constante** : lorsque l'opérande d'un changement de signe se réduit à une seule constante numérique, le compilateur écrit directement la constante négative correspondante, dans le plus petit sous-format qui la représente (RCI-M §4.4, « Constante négative ») : `X←-1` compile la valeur `0x22`, `PAS -1` de même (RCI-M §14). Sinon, le changement de signe est écrit comme opération (`0x02`) après son opérande : `-2↑2` compile `2`, `2`, `↑`, changement de signe. Ce repli ne modifie ni la valeur (le changement de signe est exact sur un nombre) ni le texte restitué.
- **Signe `+` en tête** : la grammaire reconstruite admet un `+` à la même place que le changement de signe ; le corpus ne mentionne que le changement de signe, sans exclure le `+`. Le compilateur l'accepte, sans produire aucun élément ; il n'est donc pas restitué au listage (règles de décompilation, audit du 2026-10-21, point B3).
- **Expressions booléennes** : `ET` a priorité sur `OU` (INRDP75, « Pour évaluer une expression booléenne comportant plusieurs opérateurs logiques » ; LSE15 §5), les parenthèses pouvant forcer un autre ordre.
- **Concaténation** : l'opérateur `!` ne relie que des éléments de chaîne (LSE15 §4-4.1, §4-5) ; plusieurs `!` successifs s'évaluent de gauche à droite.
- **Expression conditionnelle** : `SI condition ALORS e1 SINON e2` n'est reconnue qu'en tête d'une expression, donc aussi en tête d'une expression entre parenthèses (règle `expression` de la grammaire reconstruite ; RCI-M §4.8.5) ; la branche `SINON` s'étend aussi loin que possible (LSE15 §3-3.5). Opérande d'un opérateur, elle doit être mise entre parenthèses : `Y←3*(SI A>B ALORS A-K*B SINON A+K*B)` est accepté, mais l'exemple de LSE15 §3-3.5, `Y←3*SI A>B ALORS A-K*B SINON A+K*B`, est refusé (`C 101`) sous les trois profils. Divergence consignée : LSE 8086 accepte cette ligne sans parenthèses et la liste avec parenthèses (essai d'Éric du 2026-11-06). Elle n'est pas reprise (choix d'Éric du 2026-11-06). La forme parenthésée est celle que restitue le décompilateur (Décompilateur §5.1 : l'expression conditionnelle est une valeur de rang 1), comme le listage de LSE 8086.

**Génération d'AFFICHER.** La fonction qui reconnaît l'instruction AFFICHER (règle `afficherStmt` de la grammaire reconstruite) — et, en mode machine de bureau, la forme `?` qui lui est strictement synonyme (§8, IMP-0173) — produit la représentation fixée par RCI-M IMP-0115 en une seule passe, dans l'ordre du texte :

```text
analyser_generer_afficher(...)
1. SI le caractère suivant est '['
       écrire 0x37                                  { marque de tête, format explicite }
       analyser le format jusqu'à ']' en relevant, dans l'ordre, chaque spécification :
         libellé, mise en page (facteur fixe ou '*'), description (facteur fixe seulement)
   SINON
       écrire 0x38                                  { marque de tête, format implicite [/,U] }
2. analyser la liste d'affichage (éventuellement vide) en relevant la position de chaque
   expression dans le texte — sans encore rien écrire
3. parcourir le format et la liste simultanément, de gauche à droite (LSE15 §6-2.4) :
     libellé ou mise en page à facteur fixe → écrire l'élément 0x30–0x34
     libellé ou mise en page à facteur '*'  → générer l'expression courante de la liste, puis
                                              l'élément 0x30–0x34 en mode variable
                                              { expression consommée comme facteur, LSE15 §6-2.6 }
     description de répétition r           → écrire 0x35 ; puis, r fois : générer l'expression
                                              courante de la liste, écrire 0x36
   puis, pour chaque expression restante de la liste : la générer, écrire 0x36
4. contrôle IMP-0178 (ci-dessous) ; en cas d'échec, ERR_FORMAT_DESCRIPTIONS
```

  L'étape 2 relève les bornes des expressions en analysant chacune dans un tampon de brouillon, dont rien n'est conservé : c'est le seul moyen de trouver où finit une expression. Chaque expression est ensuite analysée de nouveau et générée à l'étape 3, à la position que lui assigne l'exploration simultanée, car l'ordre des éléments suit le format et non la liste. Un libellé est de même lu à l'étape 1, puis relu à l'étape 3. Un libellé a la syntaxe d'une constante-chaîne (§5 bis, règle 6) et une seule routine, `lire_chaine`, lit l'un et l'autre ; seule la génération diffère : un libellé produit l'élément 0x30, toujours en ligne et porteur de son facteur, une constante-chaîne l'élément 0x03, qui passe par la table des constantes au-delà du seuil (IMP-0181). Un nom figurant seul dans la liste est généré comme une référence simple suivie de 0x36, sans que le compilateur cherche à savoir s'il désignera un tableau à l'exécution (RCI-M IMP-0115). Dans une spécification de description `Fe.d` ou `Ee.d`, `e` et `d` s'écrivent chacun avec un ou deux chiffres, soit de 0 à 99 (LSE15 §6-2.3 ; RCI-M sous-code 0x35) ; un troisième chiffre produit `ERR_SYNTAXE`.

**Génération de LIRE.** L'instruction `LIRE id1,…,idn` (règles `lireStmt` et `lireItem` de la grammaire reconstruite) compile la marque de tête 0x11 (RCI-M §4.12, IMP-0183), puis, pour chaque cible dans l'ordre du texte, sa référence — simple (0x00) ou indexée (0x01), précédée de ses indices — suivie de l'élément 0x3B (RCI-M §4.11.6). Une cible désignant un tableau entier est une référence simple ; sa catégorie est établie à l'exécution (Etat d'exécution §3 bis).

**IMP-0178** — *Contrôle, à la compilation, qu'une instruction AFFICHER ne comporte pas plus de spécifications de description que de valeurs à afficher*
- Sujet : LSE15 §6-2.4 énonce : « Il ne doit pas y avoir plus de spécifications de description (compte tenu des éventuels facteurs de répétition) que de valeurs à afficher, mais il peut y en avoir moins. » Où et comment cette règle est-elle appliquée ?
- Choix : le compilateur rejette, avec le diagnostic `ERR_FORMAT_DESCRIPTIONS`, toute instruction AFFICHER (ou `?`) dont le nombre de spécifications de description, facteurs de répétition compris, dépasse le nombre de valeurs à afficher. Le nombre de valeurs à afficher est le nombre d'expressions de la liste, chacune comptant pour une valeur — un nom de tableau compris, puisqu'un tableau entier utilise une seule spécification (LSE15 §6-2.5) —, diminué du nombre d'expressions consommées comme facteur de répétition variable, qui ne sont pas affichées (LSE15 §6-2.6). Les deux nombres sont entièrement déterminés par le texte de l'instruction ; aucune information d'une autre ligne n'est nécessaire (EXI-0001). Le code `ERR_FORMAT_DESCRIPTIONS` est provisoire au sens d'IMP-0168.
- Alternatives envisagées :
  1. Laisser l'excédent de descriptions sans contrôle — écarté : une instruction comme `AFFICHER[2F4.1] A` laisserait une répétition due dans l'état du format, qui s'appliquerait à tort à la première valeur de l'instruction AFFICHER suivante ; et la règle documentée par LSE15 ne serait appliquée nulle part.
  2. Vérifier la règle à l'exécution — écarté : la violation est entièrement déterminable à la compilation ; EXI-0018 et EXI-0051 demandent qu'une erreur ainsi détectable soit diagnostiquée dès l'analyse de la ligne.
- Justification : la règle est énoncée par le corpus et ne dépend que du texte de l'instruction. Son application à la compilation garantit en outre qu'à la fin de toute instruction AFFICHER acceptée, toutes les répétitions de description ont été consommées : l'état du format revient de lui-même à son état initial, sans remise à zéro explicite (RCI-M IMP-0115).
- Impact mémoire : deux compteurs locaux pendant l'analyse de l'instruction.
- Impact disque : sans objet.
- Impact performances : négligeable.
- Impact historique : applique une règle documentée par LSE15 §6-2.4, sans restreindre aucune forme valide.
- Références normatives : LSE15 §6-2.4, §6-2.5, §6-2.6 ; EXI-0001, EXI-0018, EXI-0051 ; RCI-M IMP-0115 ; IMP-0168.
- Statut : retenu.

---

## 5 bis. Règles lexicales

Les règles suivantes sont des faits du corpus ; l'analyseur-générateur (§5) les applique en découpant le texte de la ligne au fur et à mesure de la descente récursive, sans passe lexicale préalable (IMP-0164).

1. **Composition d'une ligne de programme** (INRDP75, « Composition d'une ligne » ; LSE15 §1) : « un numéro […] ; au moins un espace (sauf dans le cas d'un commentaire) ; une ou plusieurs instructions séparées par des points-virgules ; un caractère signalant la fin de la ligne ». La « ligne vide », qui ne comporte que le numéro suivi d'un espace, est admise : « taper un espace avant ⓧ : `60∧`ⓧ » (INRDP75, ligne vide de fin de boucle). Le caractère de fin de ligne est consommé par le moniteur et ne figure pas dans `texte` (§4).
2. **Fin des mots du langage** (LSE15 §1, Remarque 1) : « Ces mots doivent tous se terminer par un espace, un point-virgule ou un `X-OFF`, sauf `AFFICHER` et les noms de fonctions usuelles (`SIN`, `LGR`, etc) qui doivent se terminer par un espace ou une ouverture de parenthèse (ouverture de crochet pour `AFFICHER`). » « Il n'y a pas de blanc entre AFFICHER et le format » (INRDP75, « Instruction de sortie. Format ») ; « le mot AFFICHER doit être suivi d'un blanc » lorsqu'aucun format ne suit (INRDP75).
3. **Espaces** : « Des espaces en nombre quelconque, en plus des éventuels espaces obligatoires, peuvent apparaître entre les différents éléments du langage » (LSE15 §1, Remarque 1). « Il ne doit pas y avoir d'espace entre les caractères composant un identificateur » (LSE15 §3-1.2).
4. **Identificateurs** : une lettre, suivie d'au plus quatre lettres ou chiffres (LSE15 §3-1.2 ; INRDP75, « Règle pour les identificateurs ») ; un nom de procédure est formé de `&` suivi d'un identificateur (LSE15 §7-2). Un identificateur n'est jamais un mot réservé (§8 bis, IMP-0180).
5. **Constantes numériques** (LSE15 §3-1.1) : « la virgule décimale est remplacée par le point décimal et la lettre `E` indique la puissance de 10 par laquelle il faut éventuellement multiplier le nombre. Dans un nombre ne doivent figurer que les chiffres de 0 à 9 et les caractères `+`, `-`, `.`, `E` ; tout autre caractère terminera le nombre » (exemples : `56.24`, `3E-20`, `-26.4E+07`, `+003.01E21`).
6. **Constantes-chaîne** : une suite de caractères entre apostrophes, une apostrophe du texte s'écrivant doublée (INRDP75, `'D''UN TRIANGLE'`) ; le texte stocké contient l'apostrophe simple (IMP-0181). Sous le profil `LSE F4`, une constante-chaîne peut aussi contenir des équivalents numériques `.n.`, en opérande comme en libellé de format (§8 bis, IMP-0190).
7. **Commentaires** : introduits par `*`, en tête de ligne immédiatement après le numéro ou après un point-virgule, jusqu'à la fin de la ligne (RCI-M §4.9) ; le texte conservé est tout ce qui suit le `*`, espaces de tête compris, afin d'être restitué à l'octet près (RCI-M IMP-0183).
8. **Caractères de saisie** : `\` (annulation du dernier caractère) et `ESC` (annulation de la ligne en cours) sont traités par la console à la saisie (LSE15 §1) et n'atteignent jamais le compilateur.

**IMP-0182** — *Interprétation des règles lexicales : reconnaissance des mots, signe des constantes, espace après le numéro, lettres majuscules*
- Sujet : les règles lexicales du corpus laissent ouverts quatre points nécessaires à la réalisation de l'analyseur : la délimitation d'un mot du langage, l'appartenance d'un signe à une constante numérique, le traitement d'un numéro de ligne non suivi d'un espace, et l'admission des lettres minuscules, absentes du corpus.
- Choix :
  - **Reconnaissance des mots.** L'analyseur lit la plus longue suite de lettres et de chiffres commençant par une lettre. Si cette suite est un mot du langage (mot réservé du profil, ou mot d'instruction attendu à cette position, comme `EN`, `QUE`, `POUR`), le caractère qui la suit doit être un espace, un point-virgule ou la fin du texte — un crochet ouvrant ou un espace pour `AFFICHER`, une parenthèse ouvrante ou un espace pour un nom de fonction — sinon la compilation échoue avec `ERR_SYNTAXE`. Ainsi `SIA=0` n'est pas lu `SI A=0`, et `SI(A>0)` est rejeté. Pour un nom de fonction qui n'est pas un mot réservé du profil (§8 bis), l'analyseur saute les espaces éventuels : une parenthèse ouvrante désigne la fonction (`SIN (X)`), tout autre caractère désigne un identificateur (`SIN←3`, `SIN[2]`).
  - **Signe d'une constante numérique.** Un `+` ou un `-` n'appartient au lexème d'une constante que s'il suit immédiatement la lettre `E` de l'exposant. En tête d'une constante, il est reconnu comme opérateur — changement de signe ou signe `+` en tête (§5, « Précédence et associativité ») —, le changement de signe d'une constante seule étant ensuite replié en constante négative à la génération (§5, « Repli sur une constante »), ce qui produit la même valeur (`-26.4E+07`) sans qu'une expression `A-5` puisse être lue comme `A` suivi d'une constante `-5`, soit deux opérandes adjacents.
  - **Espace après le numéro de ligne.** Le numéro d'une ligne de programme doit être suivi d'au moins un espace, ou immédiatement d'un `*` de commentaire ; sinon la compilation échoue avec `ERR_SYNTAXE`. En particulier, un texte réduit à un numéro sans espace (`60`) est rejeté : aucune source ne lui attribue d'autre sens, et le compilateur ne lui en invente pas.
  - **Lettres.** Hors des constantes-chaîne et des commentaires, seules les lettres majuscules `A` à `Z` sont admises ; une lettre minuscule produit `ERR_SYNTAXE`. Les minuscules restent admises, telles quelles, dans les constantes-chaîne et les commentaires. Une éventuelle conversion en majuscules à la saisie, liée au clavier de l'Apple IIGS, relève de la console (chapitre du moniteur, non encore rédigé), non du compilateur.
- Alternatives envisagées : reconnaître un mot du langage comme préfixe d'une suite plus longue (`SIA=0` lu `SI A=0`) — écarté : contredit l'exigence d'un terminateur après chaque mot (LSE15 §1) ; rattacher le signe de tête à la constante — écarté : rendrait ambiguë toute soustraction d'une constante ; admettre les minuscules comme équivalentes des majuscules dans le compilateur — écarté : introduirait dans le langage une équivalence que le corpus n'atteste pas, alors que la question relève du dispositif de saisie.
- Justification : chaque choix applique au plus près la règle du corpus là où elle est énoncée, et ne fait qu'en préciser la portée là où elle est muette, sans ajouter de construction.
- Impact mémoire : nul.
- Impact disque : sans objet.
- Impact performances : négligeable (un test de caractère après chaque mot).
- Impact historique : réalise les règles lexicales de LSE15 §1 et §3-1.1 et d'INRDP75 ; les quatre précisions ne changent le sens d'aucune ligne valide du corpus.
- Références normatives : LSE15 §1 (Remarque 1), §3-1.1, §3-1.2, §7-2 ; INRDP75, « Composition d'une ligne », « Instruction de sortie. Format » ; RCI-M §4.9 ; IMP-0164, IMP-0180, IMP-0181.
- Statut : retenu.
- Complément du 2026-10-30 (Éric ; audit du code, AP-05, AP-11, §9 point 10) : réalisation, sans effet sur aucune séquence ni aucun diagnostic.
  - L'accès au texte (`car`, `car_a`) et les classes de caractères (`est_lettre`, `est_chiffre`) sont des macros, dont aucun argument ne porte d'effet de bord.
  - Un mot est comparé à un mot du langage par son premier caractère avant toute mesure de longueur (`mot_est`, `dans_liste`, `prendre_mot`, `voir_mot`). Un nom de fonction n'est cherché que pour un mot de 2 ou 3 lettres, longueurs de tous les noms de la table.
  - Une constante numérique de cinq chiffres au plus, sans point ni exposant, et d'au plus 32 767, est lue en entier et écrite dans le sous-format entier, celui qu'aurait choisi l'écriture générale, sans `strtod` ni calcul flottant ; sa valeur est gardée en `double` pour le repli d'une constante négative.
  - Mesure sous Golden Gate, profil `cible`, sonde de cycles de l'audit sur les 867 lignes acceptées du corpus : 284 800 cycles par ligne avant, 127 100 après (−55 %), cycles de SANE non comptés ; compiler `1 A_1` fait 2 appels SANE au lieu de 24 (`iix --trace-tools`).
- Complément du 2026-11-05 (Éric ; Interpréteur IMP-0280) : sous les profils `LSE F4` et `-T1600`, la valeur d'une constante numérique est arrondie à 23 bits significatifs, la demie en s'éloignant de zéro (`rcim_arrondir_23`), avant le contrôle de plage (C 09) et le choix du sous-format ; une constante entière de valeur absolue inférieure à 2²³ n'est pas modifiée ; `8388609` est rangée 8 388 610 ; la restitution, à 6 chiffres, est inchangée.

---

## 5 ter. Contrôle de type limité au texte de la ligne

**IMP-0188** — *Type statique à trois valeurs propagé par la descente récursive ; rejet d'une ligne seulement quand deux types déterminés se contredisent*

- **Sujet :** une ligne dont le texte suffit à montrer qu'un opérande a le mauvais type, comme `'A'+1` ou `T['X']`, doit-elle être rejetée à la compilation, sachant que le compilateur ne connaît pas la catégorie d'un identificateur (§7, « Redéclaration incompatible » ; EXI-0001) ?
- **Choix :**
  1. **Type statique.** Chaque fonction d'analyse d'expression (§5) rend, en plus de son indicateur de succès, un type statique parmi trois : `TYPE_NUMERIQUE`, `TYPE_CHAINE`, `TYPE_INDETERMINE`. Ce type n'est écrit nulle part dans RCI-M ; il ne vit que le temps de l'analyse de la ligne.
  2. **Types déterminés par le texte :**
     - `TYPE_NUMERIQUE` : constante numérique ; constante numéro de ligne `@n` (MITRA 15) ; atomes `@` et `$` (T1600) ; élément de tableau (variable indicée), les tableaux étant des ensembles « de données numériques » (LSE15, l. 202) ; résultat d'un opérateur arithmétique ou d'un changement de signe ; résultat d'une fonction dont la signature produit `v` dans la colonne « Pile » de la fiche de codage RCI-M, §4 (`RAC` à `TEM`, `LGR`, `POS`, `SKP`, `PTR`, `CNB`, `EQN`, `SH`, `CH`, `TH`, `SGN`) ;
     - `TYPE_CHAINE` : constante-chaîne ; résultat de `!` ; résultat d'une fonction dont la signature produit `c` (`SCH`, `GRL`, `CCA`, `EQC`, `DAT`) ;
     - `TYPE_INDETERMINE` : identificateur simple, dont la catégorie n'est établie qu'à l'exécution (Etat d'exécution §3 bis) ; appel de procédure-fonction ; expression conditionnelle `SI…ALORS e1 SINON e2` dont les deux branches n'ont pas le même type déterminé. Si les deux branches ont le même type déterminé, l'expression prend ce type.
  3. **Contrôles.** Une ligne est rejetée, sans rien publier (EXI-0004), lorsqu'un opérande de type déterminé contredit le type requis à sa place :
     - opérande `TYPE_CHAINE` d'un opérateur arithmétique ou d'un changement de signe, ou argument `TYPE_CHAINE` à une place de signature `v` : nature « chaîne au lieu de nombre », C 27 ;
     - opérande `TYPE_NUMERIQUE` de `!`, ou argument `TYPE_NUMERIQUE` à une place de signature `c` : nature « nombre au lieu de chaîne », C 28 ;
     - indice d'une variable indicée de type `TYPE_CHAINE` : nature « indice non numérique », C 15 ;
     - dimension d'une déclaration `TABLEAU` de type `TYPE_CHAINE` : nature « dimension non numérique », C 14 ;
     - comparaison entre un opérande `TYPE_NUMERIQUE` et un opérande `TYPE_CHAINE` (LSE15, l. 1048 : « on ne pourra pas comparer une chaîne à une valeur numérique, ni vice-versa ») : nature « comparaison de types différents », C 11 ;
     - expression `TYPE_CHAINE` à une place où une instruction requiert un nombre : numéro de ligne d'`ALLER EN` et de `RETOUR EN`, numéro de fin de boucle et valeurs `v1`, pas et `v2` de `FAIRE`, facteur de répétition variable `*` d'un format (LSE15, l. 1678) : nature « chaîne au lieu de nombre », C 27.

     Un opérande `TYPE_INDETERMINE` n'est jamais rejeté. Une place de signature `x` ou `ref` (`AFFICHER`, affectation, `GARER`, `CHARGER`, `SUPPRIMER`, `EXECUTER`, arguments d'une procédure, `RESULTAT`) n'est pas contrôlée.
  4. **Numéros et natures** : IMP-0185 et IMP-0187 (chapitre Gestionnaire des erreurs). Ces natures complètent l'inventaire des diagnostics (D-77 à D-82).
- **Alternatives envisagées :**
  1. Aucun contrôle de type à la compilation — écartée : une erreur entièrement déterminée par le texte de la ligne ne serait signalée qu'à l'exécution, contrairement à l'esprit d'EXI-0018 et d'EXI-0051 et au principe de NOYELLE89.
  2. Typage des identificateurs d'après les déclarations `CHAINE` et `TABLEAU` du programme — écartée : il faudrait lire d'autres lignes (EXI-0001), et une déclaration ne prend effet qu'à son exécution (LSE15 §3-1.2).
- **Justification :** seules sont rejetées les lignes dont l'erreur est certaine quel que soit le reste du programme et quel que soit l'ordre d'exécution. MEMENTO82 (C 11, C 14, C 15, C 27, C 28) atteste qu'une implantation LSE faisait ces contrôles à la compilation.
- **Impact mémoire :** un octet de type par niveau de récursion, sur la pile C ; aucune structure persistante.
- **Impact disque :** sans objet.
- **Impact performances :** une comparaison par opérande.
- **Impact historique :** le corpus des deux profils ne dit pas si ces erreurs étaient détectées à la compilation ou à l'exécution ; elles l'étaient au moins à l'exécution (LSE15, l. 972, l. 1678). Une ligne comme `?'A'+1` est rejetée à la saisie, alors qu'une implantation historique l'acceptait peut-être pour échouer à l'exécution. L'erreur est signalée dans les deux cas ; seuls le moment et le code changent. Aucune ligne qui s'exécute sans erreur n'est rejetée.
- **Références normatives :** EXI-0001, EXI-0004, EXI-0018, EXI-0019, EXI-0051 ; LSE15 §3-1.2, l. 202, l. 972, l. 1048, l. 1678 ; RCI-M §4.11 et fiche de codage §4 (colonne « Pile ») ; MEMENTO82, annexe I (C 11, C 14, C 15, C 27, C 28) ; IMP-0164, IMP-0185, IMP-0187 ; NOYELLE89.
- **Statut :** retenu (choix d'Éric du 2026-10-22).

---

## 6. Tampon de génération temporaire et correction différée des branchements

**IMP-0165** — *Tampon de génération de taille fixe (255 octets), alloué sur la pile de l'appel de compilation, avec contrôle de débordement à chaque écriture*
- Sujet : où le générateur écrit-il la séquence RCI-M en cours de construction, avant qu'elle ne soit acceptée et transmise au gestionnaire d'objets, et comment garantit-il que cette séquence respecte la borne de 255 octets fixée par RCI-M IMP-0105 ?
- Choix : un tableau C de taille fixe, `unsigned char tampon[255]`, déclaré localement dans `compilateur_compiler` (§9) et transmis par pointeur à l'analyseur-générateur — jamais alloué dynamiquement, et strictement identique que la ligne soit une ligne de programme ou une ligne machine de bureau. Toute écriture dans le tampon passe par les fonctions `rcim_ecrire_*` (RCI-M §4), qui vérifient, avant d'écrire, que l'élément complet (étiquette et charge utile) tient dans la place restante. Si ce n'est pas le cas, rien n'est écrit et la compilation échoue avec le diagnostic `ERR_LIGNE_TROP_COMPLEXE` (code provisoire au sens d'IMP-0168), positionné sur la construction en cours d'analyse ; comme pour toute autre erreur, aucun objet compilé n'est publié (EXI-0004, §9).
- Alternatives envisagées :
  1. Tampon alloué dynamiquement (`malloc`) à chaque compilation — écarté : une allocation et une libération par ligne compilée, alors qu'une taille fixe suffit, contredirait le conditionnement §4.1 sans aucun bénéfice.
  2. Tampon statique global (une seule instance, réutilisée d'un appel à l'autre) — écarté : romprait EXI-0001 en cas de compilation ré-entrante (une ligne fournie par ENCODER, §11, ou une expression machine de bureau tapée pendant une mise au point PAS À PAS, LSE15 §9-5, pourrait en principe être compilée alors qu'une autre compilation est elle-même suspendue dans un contexte d'appel imbriqué non exclu par le corpus) ; un tampon automatique (pile C) élimine ce risque sans coût supplémentaire.
  3. Tampon de 256 octets — écarté : une séquence de 256 octets ne pourrait pas être la cible d'un branchement vers sa fin, cette position (256) n'étant pas représentable sur l'octet de charge utile de 0x00/0x01 (RCI-M IMP-0105).
  4. Se fier au seul majorant de RCI-M IMP-0125, sans contrôle — écarté : ce majorant est une estimation, établie sur les constructions recensées à la date de sa rédaction ; une ligne qui le dépasserait corromprait la pile C du compilateur au lieu de produire un diagnostic.
- Justification de la taille : la borne de 255 octets est imposée par RCI-M IMP-0105, toute position intra-séquence, fin de séquence comprise, devant tenir sur un octet. RCI-M IMP-0125 estime à environ 155 octets la séquence produite par la construction source la plus dense identifiée pour une ligne de 80 caractères ; le diagnostic `ERR_LIGNE_TROP_COMPLEXE` n'est donc attendu pour aucune ligne du corpus historique, et le contrôle ne fait que rendre la borne certaine. La même borne vaut pour une ligne machine de bureau, également limitée à 80 caractères (§2).
- Impact mémoire : 255 octets par appel de compilation, sur la pile C, libérés automatiquement au retour de `compilateur_compiler` — aucune fragmentation, aucune libération explicite à oublier.
- Impact disque : sans objet.
- Impact performances : accès direct par index ; une comparaison par élément écrit.
- Impact historique : neutre — aucune ligne historiquement valide n'est rejetée par le contrôle de débordement, selon le majorant de RCI-M IMP-0125.
- Références normatives : RCI-M IMP-0105 (borne de 255 octets), IMP-0125 (majorant de 155 octets) ; INRDP75 (limite de 80 caractères par ligne, y compris en mode machine de bureau) ; EXI-0001, EXI-0004 ; IMP-0168 ; conditionnement §4.1.
- Statut : retenu.

**IMP-0166** — *Correction différée (backpatching) des positions cibles de branchement intra-séquence*
- Sujet : RCI-M §4.7 (sous-codes 0x00/0x01) exige qu'un élément de branchement conditionnel ou inconditionnel porte, en charge utile, la position de sa cible comptée depuis le début du bloc (RCI-M IMP-0105). Au moment où le générateur écrit l'élément `0x00` d'un `SI...ALORS`, la longueur du code de la branche `ALORS` — et donc la position de la cible en cas d'échec de la condition — n'est pas encore connue, puisque cette branche n'a pas encore été générée. Comment le générateur en une seule passe (§5) peut-il néanmoins produire une position correcte ?
- Choix : à l'écriture d'un élément `0x00` ou `0x01` dont la cible n'est pas encore connue, le générateur réserve l'octet de charge utile avec une valeur provisoire (`0xFF`) et mémorise la position de cet octet dans le tampon (une simple variable locale `unsigned int position_a_corriger`, jamais une pile séparée : la structure de la descente récursive elle-même — un appel imbriqué par branche — porte cette information exactement là où elle est nécessaire). Lorsque le générateur atteint la cible, il écrit directement, par un accès indexé ordinaire dans `tampon[position_a_corriger]`, **la position courante du curseur d'écriture**, qui est exactement la position de l'élément cible depuis le début du bloc :
  - pour le `0x00` d'un `SI` à deux branches, la position atteinte juste après l'écriture du `0x01` qui termine la branche `ALORS`, c'est-à-dire le début de la branche `SINON` ;
  - pour le `0x00` d'un `SI` sans `SINON`, la position atteinte juste après la branche `ALORS` ;
  - pour le `0x01`, la position atteinte juste après la branche `SINON`.

  Lorsque la cible est la fin de la séquence, la valeur écrite est la longueur de la séquence, au plus 255 (IMP-0165). La valeur provisoire `0xFF` n'est qu'un bouche-trou : 255 pouvant être une position cible légitime, aucun code ne doit tester cette valeur pour savoir si la correction a eu lieu. Le tampon étant entièrement en mémoire, la correction est une simple écriture d'un octet déjà alloué, sans déplacement d'aucun autre octet.
- Alternatives envisagées :
  1. Deux passes : une passe de reconnaissance pure pour calculer à l'avance la longueur de chaque branche, puis une passe de génération — écarté : réintroduirait la structure intermédiaire déjà rejetée en IMP-0164, pour un problème que la correction différée résout à un coût strictement local (un octet, une variable).
  2. Générer la branche `SINON` avant la branche `ALORS` — écarté : romprait la correspondance entre ordre de reconnaissance et ordre d'écriture qui fait l'intérêt de la fusion analyse/génération (IMP-0164), pour un bénéfice nul.
  3. Écrire un décalage relatif à l'élément de branchement — écarté : RCI-M IMP-0105 fixe une position comptée depuis le début du bloc, seule convention de position intra-ligne du Tome V.
- Justification : cette technique réalise directement le format fixé par RCI-M §4.7 et IMP-0105, sans en prescrire d'autre ; la valeur écrite étant la position courante du curseur, aucun calcul n'est nécessaire.
- Impact mémoire : nul au-delà d'une variable locale par branchement en cours de génération (jamais plus d'une poignée simultanément, bornée par la profondeur de récursion, §2).
- Impact disque : sans objet.
- Impact performances : une écriture supplémentaire d'un octet par branchement généré, négligeable sur 65816.
- Impact historique : neutre — technique de réalisation pure, invisible dans le comportement observable du L.S.E.
- Références normatives : RCI-M §4.7 (IMP-0105, IMP-0111), §14 (exemples `SI...ALORS...SINON`, en position d'instruction et d'expression) ; IMP-0164, IMP-0165 ; conditionnement §9 (position logique, non adresse).
- Statut : retenu.

**Application à `FAIRE`.** Les trois éléments de contrôle d'une instruction FAIRE (RCI-M §4.8.2, IMP-0112) ne portent, à la différence de `SI`, aucun offset de branchement à corriger : la charge utile de l'ouverture (identifiant de la variable contrôlée) est entièrement connue au moment de son écriture, les éléments de pas et de test n'ont pas de charge utile, et la corrélation avec la fin de la boucle relève exclusivement de l'état d'exécution (Etat d'exécution §6). La fonction qui reconnaît l'instruction FAIRE (règle `faireStmt` de la grammaire reconstruite) génère, dans l'ordre de reconnaissance du texte, sans retour en arrière :

```text
analyser_generer_faire(...)
1. consommer FAIRE ; analyser_generer_expression → sous-séquence de n1
   SI le mot POUR ne suit pas, sous LSE F4, et que les mots TANT QUE suivent
       écrire 0x12                                                   { ouverture sans variable,
                                                                       IMP-0277 }
       aller à la branche TANT QUE de l'étape 6
2. consommer POUR ; lire un identificateur simple V, sans indice    { « variable simple »,
                                                                      LSE15 §5-5.1 ; sinon ERR_SYNTAXE }
   symbole_V ← chercher_ou_creer(V, sans '&')                       { §7, IMP-0167 }
3. consommer ← ; analyser_generer_expression → sous-séquence de v1
4. rcim_ecrire_ouverture_faire(symbole_V)                           { 0x09 + 2 octets }
5. SI le mot PAS suit
       consommer PAS ; analyser_generer_expression → sous-séquence de p
       écrire 0x0A                                                   { pas explicite }
   SINON
       écrire 0x0E                                                   { pas implicite, aucune
                                                                       sous-séquence ; jamais la
                                                                       constante 1 }
6. SI le mot JUSQUA suit
       consommer JUSQUA ; analyser_generer_expression → sous-séquence de v2
       écrire 0x0F                                                   { test JUSQUA }
   SINON SI les mots TANT QUE suivent
       consommer TANT QUE ; analyser_generer_expression_booleenne → sous-séquence de la condition
       écrire 0x10                                                   { test TANT QUE }
   SINON ERR_SYNTAXE
```

**Cible d'ALLER EN et de RETOUR EN (réalisation d'IMP-0111, 2026-10-23).** La cible est compilée comme une expression numérique (IMP-0188). Si cette expression se réduit à une seule constante entière comprise entre 1 et la borne des numéros de ligne du profil (250 sous MITRA 15, 255 sous T1600), sa sous-séquence est remplacée par l'élément à cible constante (0x02 ou 0x05, charge utile d'un octet) ; sinon, l'élément à cible calculée (0x03 ou 0x06) suit la sous-séquence. `ALLER EN 300` n'est donc pas une erreur de compilation : la cible est vérifiée et arrondie à l'exécution, et la restitution se recompile en la même séquence.

**Condition commençant par une parenthèse.** Une parenthèse ouvrante en tête d'un facteur logique peut ouvrir une condition (`(A=1 OU B=2) ET C=3`) ou une expression arithmétique (`(A+B)>C`). L'analyseur essaie d'abord la condition entre parenthèses ; si elle échoue, ou si la parenthèse fermante est suivie d'une relation ou d'un opérateur, il revient à la parenthèse (position dans le texte, curseur du tampon et diagnostic restaurés) et analyse une condition ordinaire. Les symboles éventuellement créés pendant l'essai sont les mêmes, dans le même ordre, que ceux de la seconde analyse.

  L'ordre d'écriture coïncide avec l'ordre du texte source, qui est aussi l'ordre d'évaluation prescrit par LSE15 §5-5.3 : la descente récursive à génération directe (IMP-0164) n'a donc à réordonner aucune sous-séquence. L'instruction FAIRE est interdite en mode machine de bureau (§8, IMP-0172). La correction différée décrite ici ne s'applique donc qu'aux sous-codes 0x00/0x01 — lesquels restent possibles en mode machine de bureau par le biais d'un `SI...ALORS...SINON` en position d'expression (§8 : seule l'instruction `SI` comme contrôle de séquence à numéro de ligne est interdite, pas la construction conditionnelle en position d'expression, RCI-M §4.8.5, qui ne porte aucun numéro de ligne et reste donc parfaitement compatible avec le mode machine de bureau).

---

## 7. Table des symboles du programme courant : algorithme de recherche et d'insertion

Architecture physique §6.1 (IMP-0132) fixe déjà l'allocation de la table des symboles (`malloc`/`realloc`, sans compaction) et RCI-M §6 en fixe déjà le format de chaque entrée (`EntreeSymbole`, IMP-0104, IMP-0110, IMP-0124, IMP-0127). Ni l'un ni l'autre ne fixe l'algorithme par lequel le compilateur retrouve, pour un nom rencontré dans le texte source, l'index 16 bits de son entrée — ou en crée une nouvelle si ce nom n'a encore jamais été rencontré dans le programme courant.

**IMP-0167** — *Recherche linéaire par nom, insertion en fin de table*
- Sujet : quel algorithme utiliser pour retrouver ou créer l'entrée `EntreeSymbole` correspondant à un identificateur rencontré à la compilation, qu'il provienne d'une ligne de programme ou d'une ligne machine de bureau ?
- Choix : recherche linéaire, comparant le nom rencontré (au plus 5 caractères, RCI-M IMP-0124) aux `nom`/`nom_longueur` de chaque entrée déjà présente, dans leur ordre de création. La clé de comparaison comprend, outre le nom, sa **nature** (RCI-M §6.2) : RCI-M IMP-0124 stocke le nom d'un symbole `SYM_PROCEDURE` sans son `&` initial, de sorte que `&F` et `F` partagent le même contenu `nom`/`nom_longueur` alors qu'ils désignent, selon LSE15 §7-2, deux identificateurs distincts. Un nom rencontré avec `&` (`PROC_IDENT` de la grammaire reconstruite) n'est donc comparé qu'aux entrées de nature `SYM_PROCEDURE`, et un nom rencontré sans `&` qu'aux entrées de nature `SYM_IDENTIFICATEUR`. Cette partition est stable : un identificateur sans `&` ne peut jamais devenir un nom de procédure, ni l'inverse. Si aucune entrée ne correspond, une nouvelle entrée est ajoutée en fin de table (`realloc` d'une case supplémentaire), avec la nature que donne la présence ou l'absence du `&` — `SYM_PROCEDURE`, avec `InfoProcedure` initialisée à `{0, 0}` (« aucune déclaration en vigueur », §13.5), ou `SYM_IDENTIFICATEUR` — et l'origine de la ligne (programme ou machine de bureau, RCI-M IMP-0109) ; son index (la position ainsi obtenue) est celui utilisé par toute référence RCI-M ultérieure vers ce symbole (RCI-M §4.5, IMP-0104). Le compilateur n'attribue jamais à un identificateur la catégorie variable simple, variable-chaîne ou tableau : elle n'est établie qu'à l'exécution, dans l'environnement des identificateurs (Etat d'exécution §3 bis, IMP-0179), qui reçoit une entrée `ID_NON_DEFINI` pour tout nouveau symbole. Une ligne machine de bureau qui introduit une variable nouvelle (LSE15 §9-5 : « affecter une valeur à une nouvelle variable ») suit exactement la même procédure d'insertion, sans traitement distinct.
- Alternatives envisagées :
  1. Table triée alphabétiquement, recherche dichotomique (par analogie directe avec RCI-M IMP-0106 pour la table des objets compilés) — écarté : une table triée exigerait, à chaque insertion d'un nouveau symbole, de décaler toutes les entrées de rang supérieur pour préserver l'ordre — coût O(n) par insertion, alors que l'insertion est justement l'opération la plus fréquente pendant la frappe d'un programme (chaque identificateur nouvellement introduit) ; la recherche linéaire en O(n) sur une table non triée, suivie d'une insertion en O(1) en fin de table, offre un coût total comparable ou inférieur pour ce profil d'usage, sans la complexité de gestion du décalage.
  2. Table de hachage — écarté pour la même raison qu'en RCI-M IMP-0106 : complexité d'implémentation disproportionnée en C ANSI sur cible contrainte, pour un nombre de symboles par programme qui reste, dans tout le corpus historique examiné, de l'ordre de quelques dizaines à quelques centaines — la recherche linéaire y reste largement praticable en contexte interactif (une recherche par ligne compilée, non par accès en boucle serrée : la boucle serrée accède directement à `EntreeSymbole.contenu.valeur`, RCI-M IMP-0110, sans jamais retraverser cette recherche).
- Justification : ce choix retient, pour la table des symboles, la solution la plus simple compatible avec son profil d'usage réel — une recherche à chaque occurrence d'un identificateur *pendant la compilation*, jamais pendant l'exécution (RCI-M IMP-0104 : toute référence compilée porte déjà l'index résolu, l'interpréteur n'a plus jamais besoin de rechercher par nom) — ce qui écarte le besoin d'une structure optimisée pour la recherche répétée que justifierait, à l'inverse, la table des objets compilés (IMP-0106, consultée à chaque exécution d'un `ALLER EN`).
- Impact mémoire : aucune structure supplémentaire (pas d'index auxiliaire, pas de table de hachage) ; coût strictement égal à celui déjà compté par RCI-M §6/§8 pour la table elle-même.
- Impact disque : sans objet.
- Impact performances : O(n) par occurrence d'identificateur à la compilation, n restant petit (quelques centaines au plus) et la compilation n'étant jamais sur le chemin critique de l'exécution — conforme au conditionnement §27 (« les optimisations prématurées sont secondaires »).
- Impact historique : neutre.
- Références normatives : RCI-M §6, IMP-0104, IMP-0106 (comparaison explicite avec la table des objets compilés), IMP-0124 ; LSE15 §7-2 (nom de procédure introduit par `&`) ; Architecture physique IMP-0132, IMP-0133 (allocation par `realloc`) ; conditionnement §3, §27 ; RCI-M §3 bis/IMP-0109 (symboles référencés en mode machine de bureau).
- Statut : retenu.
- Complément du 2026-10-30 (Éric ; audit du code, AP-12, AC-32) : les deux tables sont agrandies par tranches de 32 entrées, jusqu'à leur plafond de 6 553 symboles et de 21 845 constantes (RCI-M IMP-0104), au lieu d'une entrée à la fois ; au plus 31 entrées restent réservées sans emploi. La restauration d'un fichier-programme ajoute ses entrées sans recherche, leur unicité ayant été vérifiée (Bibliothécaire et fichiers IMP-0246).
- Complément du 2026-10-30 (Éric ; audit du code, AP-05) : la recherche compare en ligne le premier caractère du nom, puis sa longueur et sa nature, puis le reste du nom.

**Redéclaration incompatible.** Le compilateur ne contrôle jamais la compatibilité entre l'usage d'un identificateur et une déclaration `TABLEAU` ou `CHAINE` portant sur le même nom, que celle-ci figure dans la même ligne ou dans une autre : une telle déclaration ne prend effet qu'à son exécution (LSE15 §3-1.2 : « quand on exécute une instruction `TABLEAU` » ; RCI-M §4.11.5, remarque générale), et l'ordre d'exécution des lignes n'est pas connu à la compilation (EXI-0001). Un même nom rencontré dans des contextes syntaxiques différents — par exemple `1 B←1` et `2 TABLEAU B[3]` — retourne toujours la même entrée de symbole ; l'incompatibilité éventuelle est une erreur d'exécution, détectée selon les règles de transition de l'environnement des identificateurs (Etat d'exécution §3 bis, IMP-0179). C'est la réalisation directe d'EXI-0019 (distinction entre erreurs de compilation et erreurs d'exécution). La même règle s'applique, sans distinction, lorsque le nom a été rencontré pour la première fois en mode machine de bureau puis réutilisé dans une ligne de programme, ou inversement.

**Chaînes littérales et table des constantes partagées.** RCI-M IMP-0107 réserve la table des constantes partagées aux chaînes littérales longues ; le compilateur la tient comme la table des symboles (recherche linéaire, insertion en fin de table, entrée inerte si la ligne est finalement rejetée).

**IMP-0181** — *Toute chaîne littérale d'une ligne de programme plus longue que le seuil passe par la table des constantes partagées ; les chaînes d'une ligne machine de bureau sont toujours en ligne*
- Sujet : à quel moment et selon quelle règle une chaîne littérale est-elle placée dans la table des constantes partagées plutôt qu'encodée en ligne (RCI-M §4.4, sous-code 0x03, IMP-0128) ?
- Choix : lorsque l'analyseur-générateur reconnaît une chaîne littérale (texte entre apostrophes, une apostrophe doublée représentant une apostrophe du texte), il en calcule le texte et la longueur ; puis :
  - sur une **ligne machine de bureau**, ou si la longueur ne dépasse pas le seuil de partage (16 caractères, RCI-M IMP-0107), il écrit l'élément 0x03 en mode « texte en ligne » ;
  - sinon, il recherche dans la table des constantes partagées une entrée de même longueur et de même texte, par comparaison linéaire dans l'ordre de création ; à défaut, il ajoute une entrée en fin de table (`realloc`, texte confié au gestionnaire d'objets par `objets_creer`) ; il écrit l'élément 0x03 en mode « index » avec l'index de l'entrée trouvée ou créée.

  Le compilateur ne retire jamais une entrée. Une entrée créée pour une ligne finalement rejetée reste inerte, comme une entrée de symbole (§3). Si la création de l'objet texte échoue faute de mémoire, la compilation échoue avec `ERR_MEMOIRE_INSUFFISANTE`, sans rien publier (§9).
- Alternatives envisagées : ne placer une chaîne dans la table qu'à sa deuxième occurrence (rejeté : la première occurrence, déjà compilée en ligne dans une autre ligne, ne pourrait plus être partagée, et la détection supposerait de conserver un historique des chaînes rencontrées, contraire à EXI-0001) ; partager aussi les chaînes d'une ligne machine de bureau (rejeté : le bloc transitoire est libéré aussitôt exécuté, le partage n'apporte rien et créerait des entrées sans aucune ligne persistante qui les référence, RCI-M §7.1) ; table de hachage (rejeté pour les mêmes raisons qu'IMP-0167 : une recherche par chaîne longue à la compilation, jamais à l'exécution).
- Justification : le passage systématique par la table au-delà du seuil est la seule règle qui réalise le partage voulu par ANN-K-0011 sans qu'une compilation dépende d'une autre ligne (EXI-0001) : deux occurrences identiques, quelles que soient les lignes et l'ordre de compilation, désignent la même entrée.
- Impact mémoire : une entrée de 3 octets plus l'objet texte par chaîne longue distincte ; 3 octets dans la séquence par occurrence partagée, au lieu de 1 + longueur.
- Impact disque : sans objet ici (RCI-S filtre les entrées non référencées, RCI-M IMP-0107).
- Impact performances : une recherche linéaire par chaîne longue rencontrée, à la compilation seulement.
- Impact historique : neutre — le texte restitué et le comportement sont identiques quelle que soit la forme de l'élément 0x03.
- Références normatives : ANN-K-0011, ANN-K-0012 ; EXI-0001, EXI-0004 ; RCI-M §4.4, §7, §7.1, IMP-0107, IMP-0109, IMP-0128 ; IMP-0167.
- Statut : retenu.

---

## 8. Restrictions propres au mode machine de bureau (réalisation de EXI-0028 / VM-0702, `LSE15Mitra.g4` [J]/[K])

Le corpus historique (INRDP75, « Mode machine de bureau » ; LSE15 §9-5) et la grammaire reconstruite (`LSE15Mitra.g4`, commentaire [J]) établissent qu'une ligne machine de bureau partage avec une ligne de programme l'intégralité de la syntaxe des expressions, des conditions et de la plupart des instructions, à trois différences près, que le présent paragraphe réalise physiquement.

**IMP-0172** — *Paramètre `mode_bureau` traversant récursivement l'analyseur-générateur d'instructions, interdisant les instructions à numéro de ligne*
- Sujet : certaines instructions — celles qui font intervenir un numéro de ligne (`ALLER EN`, `FAIRE...JUSQUA`/`TANT QUE`, `RETOUR EN`), celles qui ne prennent sens que dans le corps d'une procédure (`RETOUR`, `RESULTAT`), l'appel de procédure-fonction employé comme instruction, et, sous le profil MITRA 15 seulement (§8 bis, IMP-0180), les déclarations de variables (`TABLEAU`, `CHAINE`, `LIBERER`) — sont interdites en mode machine de bureau (INRDP75 : « les instructions LSE faisant intervenir les numéros de ligne... ne peuvent être utilisées en mode machine de bureau » ; `LSE15Mitra.g4` [J]). Comment le générateur en une seule passe (§5), qui partage l'intégralité de ses fonctions de reconnaissance d'instruction entre les deux formes de ligne, applique-t-il cette restriction — y compris lorsque l'instruction interdite est nichée sous un `SI...ALORS` ou à l'intérieur d'un `DEBUT...FIN` ?
- Choix : un paramètre booléen `mode_bureau`, fixé une seule fois par `compilateur_compiler` (§9, dès que la forme de la ligne a été déterminée, IMP-0171) et transmis, sans jamais être modifié, à travers tous les niveaux de la descente récursive — y compris, explicitement, aux fonctions qui reconnaissent une instruction composée (`DEBUT...FIN`) ou une branche de `SI...ALORS...SINON` en position d'instruction, de sorte que la restriction s'applique identiquement à la profondeur zéro et à toute profondeur d'imbrication. La fonction qui reconnaît une instruction consulte `mode_bureau` avant de reconnaître l'instruction elle-même : si `mode_bureau` est vrai et que le mot-clé ou la construction rencontrée est l'une de celles énumérées ci-dessus, elle produit immédiatement un diagnostic (`ERR_INSTRUCTION_INTERDITE_BUREAU`, §8) sans écrire aucun élément dans le tampon de génération, et sans tenter de reconnaître le reste de l'instruction. Ce même paramètre désactive la reconnaissance de l'appel de procédure-fonction comme opérande d'expression (conséquence directe de l'interdiction de `RESULTAT`, `LSE15Mitra.g4` [K] : une fonction ne produit sa valeur que par `RESULTAT`, inexécutable ici) : la fonction de reconnaissance d'un opérande primaire (`atom`) refuse, lorsque `mode_bureau` est vrai, la forme « nom de procédure suivi d'une liste d'arguments entre parenthèses ».
- Alternatives envisagées :
  1. Deux jeux de fonctions de reconnaissance d'instruction entièrement séparés (un pour `statementList`, un pour `bureauStatementList`, sans paramètre partagé) — écarté : dupliquerait l'intégralité de la logique de reconnaissance des instructions autorisées dans les deux modes (affectation, `SI...ALORS...SINON`, `AFFICHER`, `LIRE`, `GARER`, `CHARGER`, `SUPPRIMER`, `PAUSE`, `TERMINER`), pour ne faire varier, au fond, que la poignée d'instructions effectivement interdites — contraire au conditionnement §3 (simplicité) et risquant une divergence future entre les deux copies à chaque évolution du catalogue d'instructions (RCI-M §4.11).
  2. Vérification *a posteriori*, après génération complète de la ligne, par un parcours du tampon à la recherche d'éléments de contrôle à numéro de ligne interdits — écarté : contredirait directement EXI-0018 (la ligne doit être analysée syntaxiquement *avant* son intégration, ce qui inclut le rejet d'une construction invalide dès sa reconnaissance, non après coup) et obligerait à distinguer, dans le tampon déjà généré, un élément de contrôle légitimement présent (la forme postfixée de `SI...ALORS...SINON` en expression, RCI-M §4.8.5, licite en mode machine de bureau) d'un élément qui aurait dû être rejeté — distinction que le contexte syntaxique au moment de la reconnaissance fournit immédiatement et sans ambiguïté, alors qu'elle devrait être reconstituée après coup à partir du seul tampon binaire.
- Justification : ce mécanisme réalise exactement, au niveau de l'implémentation, le paramètre `allowProc` déjà documenté par la grammaire reconstruite (`LSE15Mitra.g4`, commentaires [J]/[K]) comme traversant « toute la chaîne d'expression », et la nature récursive de la restriction que cette même grammaire signale explicitement (« cette interdiction est RÉCURSIVE... d'où les règles `bureauConditionalStmt` et `bureauCompoundStmt`... qui rebouclent sur `bureauStatement` et non sur `statement` ») — la descente récursive du présent chapitre (IMP-0164) retrouve naturellement cette même propriété dès lors que `mode_bureau` est propagé sans exception à tout appel récursif, sans qu'aucune structure de données supplémentaire (pile de portée, table de drapeaux) ne soit nécessaire : un seul paramètre scalaire, constant pour la durée de toute la compilation d'une ligne, suffit.
- Impact mémoire : nul — un paramètre scalaire supplémentaire par appel de fonction, sans structure propre.
- Impact disque : sans objet.
- Impact performances : un test booléen par instruction reconnue, négligeable.
- Impact historique : réalise fidèlement la restriction documentée par INRDP75 et reprise par LSE15 §9-5, y compris sa portée récursive, sans l'étendre ni la restreindre.
- Références normatives : INRDP75, « Mode machine de bureau » ; LSE15 §9-5 ; MEMENTO82, imprimé (le mode machine de bureau « permet des instructions complexes comme GARER, CHARGER, LIRE, AFFICHER », d'après Éric, 2026-10-30 ; passage non transcrit) ; `LSE15Mitra.g4`, commentaires [J], [K], règles `bureauStatement`, `bureauConditionalStmt`, `bureauCompoundStmt` ; EXI-0018, EXI-0028. LSE 8086 refuse LIRE en machine de bureau (essai d'Éric du 2026-10-30) : divergence non retenue, MEMENTO82 attestant le contraire.
- Statut : retenu.

**IMP-0173** — *`?` reconnu uniquement en mode machine de bureau, comme synonyme strict d'`AFFICHER`*
- Sujet : LSE15 §9-5 et INRDP75 décrivent le caractère `?` comme introduisant, en mode machine de bureau, l'évaluation et l'affichage d'une expression, avec la même syntaxe que l'instruction `AFFICHER` — y compris ses formats (RCI-M §4.10). Comment le générateur réalise-t-il cette synonymie sans dupliquer la logique de génération d'`AFFICHER` ?
- Choix : la fonction de reconnaissance d'instruction, lorsque `mode_bureau` est vrai (IMP-0172) et que le premier caractère significatif de l'instruction est `?`, consomme ce caractère puis appelle directement la **même** fonction de reconnaissance et de génération qu'utilise `AFFICHER` pour le reste de son instruction (y compris les crochets de format, RCI-M §4.10) — produisant exactement la même séquence RCI-M que si l'instruction avait été écrite `AFFICHER` in extenso. Lorsque `mode_bureau` est faux (ligne de programme), le caractère `?` n'est reconnu par aucune production d'instruction : sa rencontre produit le diagnostic de nature « `?` réservé au mode machine de bureau » (§9 bis, IMP-0191).
- Alternatives envisagées : introduire un sous-code RCI-M distinct pour « `?` » — écarté : RCI-M §4.10/§4.11 ne distingue déjà aucun sous-code selon la graphie d'origine de l'instruction (seul le format effectivement spécifié importe), et la grammaire reconstruite elle-même documente `?` comme un synonyme *strict*, syntaxiquement interchangeable avec `AFFICHER` (`LSE15Mitra.g4` [J] : « synonyme strict d'`AFFICHER`... même syntaxe, y compris les formats »), de sorte qu'un sous-code distinct introduirait une information physique (laquelle des deux graphies a été employée) qu'aucun composant en aval (interpréteur, décompilateur) n'a jamais besoin de reconstituer — contraire à EXI-0016 (compacité).
- Justification : réutiliser la fonction de génération d'`AFFICHER` telle quelle garantit, par construction, qu'aucune divergence de comportement ne peut apparaître entre les deux graphies, et évite de maintenir deux implémentations du formatage (RCI-M §4.10, dont la complexité — libellés, facteurs de répétition, types F/E/U — ne doit exister qu'une seule fois dans le compilateur).
- Impact mémoire : nul — aucune fonction supplémentaire, un seul branchement de reconnaissance avant l'appel partagé.
- Impact disque : sans objet.
- Impact performances : négligeable.
- Impact historique : réalise fidèlement LSE15 §9-5, y compris la précision que `?` n'existe que dans ce mode (confirmée empiriquement par `LSE15Mitra.g4` [J] : « un test empirique sur un environnement LSE réel... confirme que `?` y est rejeté à la compilation » en ligne de programme).
- Références normatives : LSE15 §9-5 ; INRDP75, « Mode machine de bureau » ; RCI-M §4.10 (format d'AFFICHER) ; `LSE15Mitra.g4` [J] ; EXI-0016.
- Statut : retenu.

**Déclaration de procédure.** Aucune disposition supplémentaire n'est nécessaire pour exclure `PROCEDURE &ID(...)` du mode machine de bureau : cette déclaration n'est, de toute façon, reconnue par aucune production autre que celle de tête de ligne numérotée (§13 ; `LSE15Mitra.g4` [H]) — une ligne machine de bureau, qui ne possède structurellement pas de « tête de ligne » au sens où `PROCEDURE` y serait recevable, ne peut donc jamais la rencontrer, sans qu'aucun test explicite sur `mode_bureau` ne soit requis à cet endroit précis.

**Structure du diagnostic (complément à §9/§15).** Le code `ERR_INSTRUCTION_INTERDITE_BUREAU` introduit par IMP-0172, comme tout autre code de `DiagnosticCompilation` (§9), reste une valeur provisoire au sens d'IMP-0168 : son énumération exacte et son libellé définitif relèvent du gestionnaire des erreurs (chapitre distinct, non encore rédigé) ; seule sa distinction d'avec un diagnostic de syntaxe ordinaire est fixée ici, pour que ce chapitre et ses tests (§19) ne dépendent pas de la valeur numérique précise qui lui sera finalement attribuée.

## 8 bis. Profil d'implantation (`LSE F4`, T1600 version 4, MITRA 15 version 8)

EXI-0061 et Tomes I-II §20.3 imposent de distinguer « les deux implantations historiques documentées dans le périmètre : **version 4 sur T1600** et **version 8 sur MITRA 15** » (INRDP75, « Remarque sur la présentation du fascicule »), les propriétés communes étant portées par une machine abstraite unique et les différences documentées représentées « comme des variantes de profil et non comme deux machines abstraites indépendantes ». Le Tome IV (invariant 17) précise qu'« un profil d'implantation ne peut introduire qu'une différence historiquement attestée ou strictement nécessaire à sa représentation ». Les tomes ne fixent pas le mode de sélection du profil (IV-FMT-0012, « choix de conception »). LSE15, « version mitra 15 » du manuel imprimée en 1978, décrit un système Mitra 15 dont la version n'est pas indiquée ; il sert de source complémentaire pour le profil MITRA 15, subordonnée à INRDP75 — source de référence des deux profils — en cas de désaccord (règle de préséance, RCI-M §4.11.1).

**IMP-0180** — *Profil d'implantation choisi au lancement, fixe pour la session, consulté par le compilateur ; profil strict*
- Sujet : comment le profil d'implantation est-il choisi, où est-il porté, et quelles constructions le compilateur admet-il ou rejette-t-il selon le profil ?
- Choix :
  - **Sélection.** Le profil est choisi une seule fois, au lancement de l'application depuis le shell ORCA, par un paramètre de la ligne de commande : `-F4` pour le `LSE F4` de Victor Duruy, `-T1600` pour le T1600 version 4, `-MITRA15` pour le MITRA 15 version 8 ; si plusieurs sont donnés, le dernier l'emporte. En l'absence de paramètre, ou si l'application n'est pas lancée depuis le shell (`argc` nul, `cc.notes.txt` point 69), le profil est **`LSE F4`** (IMP-0276). La lecture de la ligne de commande appartient à la couche plateforme (`src/plateforme/`), qui transmet le profil à `main.c`. Le profil ne change jamais en cours de session : un programme compilé sous un profil contient des éléments propres à ce profil (les deux sens de `@`, RCI-M IMP-0119), qu'un autre profil ne saurait ni exécuter ni décompiler.
  - **Support.** Le profil est porté par une structure de configuration de session, en lecture seule une fois initialisée :

```c
typedef unsigned char ProfilImplantation;
#define PROFIL_MITRA15_V8  0
#define PROFIL_T1600_V4    1
#define PROFIL_T1600_F4    2   /* profil par défaut : LSE F4, Victor Duruy */
#define PROFIL_EST_T1600(p) ((p) != PROFIL_MITRA15_V8)

typedef struct {
    ProfilImplantation  profil;
} ConfigurationSession;
```

    Le compilateur, l'interpréteur et le décompilateur la consultent ; elle ne constitue pas un état conservé d'une compilation à l'autre (EXI-0001) et ne distingue pas la provenance d'une ligne (IMP-0169).
  - **Profil strict.** Chaque profil n'admet que les constructions et graphies documentées pour sa machine : INRDP75 pour le T1600 version 4, INRDP75 et LSE15 pour le MITRA 15 version 8, INRDP75 et les sorties de Victor Duruy pour `LSE F4`. Une construction propre à un autre profil est rejetée avec le diagnostic `ERR_HORS_PROFIL` (code provisoire au sens d'IMP-0168), positionné sur la construction, sans rien publier (EXI-0004). Les différences retenues sont exclusivement celles du tableau ci-dessous.
- Alternatives envisagées :
  1. Une commande du moniteur pour changer de profil — écarté : aucune commande de ce type n'est attestée, et en introduire une modifierait le langage de commande historique ; un changement de profil en cours de session rendrait en outre inexécutables les lignes déjà compilées sous l'autre profil.
  2. Un profil fixé à la construction de l'application — écarté : obligerait à produire deux exécutables pour deux variantes d'une même machine abstraite, alors qu'EXI-0061 demande que les différences « puissent être sélectionnées ».
  3. Un profil tolérant, admettant les graphies des deux machines (`TEM` et `TEM()`, les deux sens de `@`…) — écarté : chaque manuel documente une seule graphie par machine, et les deux sens de `@` sont incompatibles. Une graphie supplémentaire n'est admise que si une source de première main l'atteste sur la machine même (IMP-0189).
- Justification : la sélection au lancement réalise EXI-0061 sans ajouter aucun élément au langage ni au langage de commande ; le profil strict garantit qu'un programme accepté sous un profil n'emploie que des constructions attestées pour la machine correspondante, conformément au Tome IV (invariant 17).
- Impact mémoire : un octet de configuration.
- Impact disque : sans objet ici ; le profil sous lequel un programme a été compilé devra figurer dans la provenance d'un fichier-programme (IV-FMT-0019), point renvoyé au chapitre RCI-S.
- Impact performances : une comparaison par construction propre à un profil.
- Impact historique : restitue les implantations documentées, sans en fusionner les différences.
- Références normatives : EXI-0061 ; Tomes I-II §20.3 ; Tome IV, invariant 17, IV-FMT-0012, IV-FMT-0019 ; INRDP75, « Remarque sur la présentation du fascicule », Annexe III, Annexe VI ; RCI-M §4.11.1 (préséance), IMP-0119 ; `cc.notes.txt` (ORCA/C), points 68 et 69 ; EXI-0001, EXI-0004 ; IMP-0168, IMP-0169.
- Statut : retenu (révisé le 2026-11-01 : troisième profil et profil par défaut, IMP-0276 ; T1600 version 4 strictement conforme à INRDP75, choix d'Éric).

**IMP-0196** — *Configuration de session : une instance unique, définie par `main.c`, déclarée dans `types_communs.h`*
- Sujet : où vit la structure `ConfigurationSession` d'IMP-0180, que le compilateur, l'interpréteur et le décompilateur consultent sans la recevoir en paramètre (la signature de `compilateur_compiler`, §4, n'en comporte pas) ? Le paramètre `-LIBELLES` d'IMP-0186 relève de la même configuration.
- Choix : `src/interfaces/types_communs.h` définit
  ```c
  typedef struct {
      ProfilImplantation profil;     /* PROFIL_MITRA15_V8, PROFIL_T1600_V4 ou PROFIL_T1600_F4 (IMP-0180) */
      unsigned char      libelles;   /* 1 : libellé après le code d'erreur (IMP-0186) */
  } ConfigurationSession;

  extern ConfigurationSession configuration_session;
  ```
  L'instance est définie dans `src/plateforme/main.c`, qui la remplit une seule fois au lancement à partir de la ligne de commande ; le noyau ne fait que la lire. Une suite de tests définit sa propre instance, ce qui lui permet de compiler sous l'un ou l'autre profil (IMP-0192).
- Alternatives envisagées : un module `session.c` du noyau, avec des fonctions d'accès — écarté : un fichier de plus, absent de la table d'Organisation du dépôt §2.1, sans protection réelle supplémentaire ; un paramètre ajouté à `compilateur_compiler` — écarté : il modifierait l'interface fixée au §4 et devrait être propagé à chaque composant.
- Justification : `types_communs.h` porte les types employés à la fois par le noyau et par la plateforme (IMP-0136) ; c'est le cas de la configuration, remplie par la plateforme et lue par le noyau.
- Impact mémoire : 2 octets. Impact disque : sans objet. Impact performances : nul.
- Impact historique : neutre.
- Références normatives : IMP-0136, IMP-0180, IMP-0186, IMP-0192.
- Statut : retenu (choix d'Éric du 2026-10-23).

**IMP-0276** — *Profil `LSE F4` : le T1600 de Victor Duruy, troisième profil et profil par défaut*
- Sujet : le LSE-IIGS vise la reconstitution la plus fidèle possible du LSE utilisé sur le T1600 du lycée Victor Duruy, dont la bannière imprime `LSE F4`. Le profil T1600 version 4 reproduit la machine décrite par INRDP75, et plusieurs comportements des sorties de Victor Duruy s'en écartent (bannière, messages, formes de langage). Où porter ces comportements, et quel profil s'ouvre au lancement ?
- Choix :
  - **Troisième profil.** `PROFIL_T1600_F4`, de valeur 2, s'ajoute à `PROFIL_MITRA15_V8` (0) et à `PROFIL_T1600_V4` (1). Ces deux profils gardent leur définition : le T1600 version 4 s'en tient à INRDP75, sans `TEM()` ni `.n.` (IMP-0189, IMP-0190).
  - **Profil par défaut.** Sans paramètre, ou hors du shell, le profil est F4. Les paramètres sont `-F4`, `-T1600` et `-MITRA15` (IMP-0180).
  - **Héritage.** F4 reprend tout ce qui vaut pour le T1600 version 4 et y ajoute ses formes propres. Le code le teste par `PROFIL_EST_T1600(p)`, vrai pour les deux versions ; une différence propre à F4 se teste sur `PROFIL_T1600_F4`. N'existent que sous F4 : `TEM()` et `DAT()` (IMP-0189), `.n.` (IMP-0190), `FAIRE n1 TANT QUE c` (RCI-M IMP-0277), `DUPLIQUER` (IMP-0258), `$` devant un nom de fichier (IMP-0278) et `VOUS ETES SOUS ID` (IMP-0268). Dans le Tome V, « profil T1600 » et « sous T1600 » désignent les deux versions, sauf si la version est nommée, et « les deux profils » désigne le MITRA 15 et le T1600.
  - **Sources.** Pour F4, les sorties de Victor Duruy font foi ; là où elles se taisent, le profil suit INRDP75, comme le T1600 version 4. Une différence entre F4 et V4 doit être attestée ou strictement nécessaire à sa représentation (Tome IV, invariant 17), et reçoit sa propre décision.
  - **État reconstitué.** Les sorties montrent plusieurs états du système : une ouverture sans bannière (`BØNJØUR   10:47:16`, `BØNJØUR  00/00/00 19:19:44`), la bannière à quatre lignes, datée du 15/10/80 sur MORSE, et `  ???? (CØMMANDE ERRØNEE)` en mars 1983. Le profil vise l'état à bannière.
  - **Identification.** Sans `-ID`, une session F4 s'ouvre sous l'identification 1 (`VØUS ETES SØUS ID 1`) ; `-ID 0` reste possible (complément d'IMP-0268). Sous T1600 version 4 et MITRA 15, le défaut reste 0.
  - **Fichiers.** La valeur 2 occupe l'octet de profil de l'en-tête RCI-S. Un fichier-programme rangé sous un autre profil, T1600 version 4 compris, est refusé (F 100, IMP-0246) : F4 recevra des éléments propres. Les rubans et les fichiers décodés, qui sont du texte, passent d'un profil à l'autre s'ils n'emploient que des formes admises par le profil qui les lit : sous T1600 version 4, un texte produit sous F4 qui porte `TEM()` ou `DAT()` est refusé (C 103).
- Alternatives envisagées :
  1. Modifier le profil T1600 version 4 — écarté : il perdrait sa fidélité à INRDP75, et le corpus de ce manuel ne pourrait plus servir de témoin.
  2. Ajouter à `-T1600` une option de site — écarté : deux réglages pour une seule machine, et des combinaisons qu'aucune source n'atteste.
  3. Garder le MITRA 15 par défaut — écarté : choix d'Éric ; le profil par défaut est celui que vise le projet.
  4. Accepter sous F4 un fichier-programme rangé sous V4 — écarté : la règle d'égalité du profil reste simple, et un programme passe d'un profil à l'autre par ruban ou par ENCODER.
- Justification : EXI-0061 demande de distinguer les implantations sans dupliquer la machine abstraite commune, chaque différence étant sélectionnée par le profil correspondant ; les sorties de Victor Duruy sont une documentation de première main. La machine abstraite n'est pas touchée.
- Impact mémoire : nul. Impact disque : une valeur de plus pour un octet existant. Impact performances : nul.
- Impact historique : ajoute une implantation attestée ; le T1600 version 4 et le MITRA 15 version 8 restent tels que les manuels les décrivent, le second se demande désormais par `-MITRA15`.
- Tests : `tests/representation/types_plateforme.c` (valeurs rangées, `PROFIL_EST_T1600`) ; contrôles F4 dans `composants_compilateur_t1600`, `composants_decompilateur`, `composants_analyseur`, `composants_moniteur`, `composants_donnees`, `composants_codage` ; refus croisé entre profils dans `composants_bibliotheque` ; `02-HISTORIQUE/Reconstitution-PERFO-CHARG/essais/verifier.py`, sous F4.
- Références normatives : EXI-0061 ; Tomes I-II §20.3 ; Tome IV, invariant 17, IV-FMT-0012, IV-FMT-0019 ; `02-HISTORIQUE/Releve-sorties-T1600-Victor-Duruy.md` ; IMP-0180, IMP-0189, IMP-0190, IMP-0196, IMP-0246, IMP-0268.
- Statut : retenu (choix d'Éric du 2026-11-01).

**Différences attestées qui touchent la compilation.**

| Construction | T1600 version 4 | MITRA 15 version 8 | Source |
|---|---|---|---|
| Fonctions `SH`, `CH`, `TH`, `SGN` | admises (RCI-M 0x3C–0x3F) | rejetées | INRDP75 Annexe III §1, §4 |
| Instructions `UN` et `ZER` | admises (RCI-M 0x1A, 0x1B) | rejetées | idem |
| Symbole `$` (ligne de retour d'une procédure) | admis (RCI-M référence 0x03) | rejeté | idem |
| Symbole `@` comme atome d'expression, numéro de la ligne courante (`10 ALLER EN @+3`) | admis (RCI-M référence 0x02) | rejeté | Annexe III §3 |
| Symbole `@` marquant une constante numéro de ligne (`10 I←@50`) | rejeté | admis (RCI-M valeur 0x06) | Annexe III §3 |
| Instruction `SUPPRIMER` | rejetée | admise (RCI-M 0x2E) | Annexe III §2, §4 |
| Fonctions `TEM` et `DAT` | sans parenthèses : `TEM`, `DAT` ; `LSE F4` admet en outre `TEM()` et `DAT()`, et les restitue ainsi (IMP-0189) | avec parenthèses : `TEM()`, `DAT()` | INRDP75 l. 850 (« Sur T1600 la fonction TEM est sans parenthésage »), l. 1442 (« s'écrit `DAT()` en MITRA 15 et `DAT` en T1600 ») ; relevé T1600 de Victor Duruy, §3 |
| Équivalents numériques `.n.` | rejetés ; `LSE F4` les admet (IMP-0190) | rejetés | INRDP75 et LSE15 les ignorent ; relevé T1600 de Victor Duruy, §3 |
| `FAIRE n1 TANT QUE c`, sans `POUR` | rejeté ; `LSE F4` l'admet (RCI-M 0x12, IMP-0277) | rejeté | INRDP75 §4-2, LSE15 §5-5.1 ; relevé T1600 de Victor Duruy, §3 |
| Numéro de ligne | 1 à 255 | 1 à 250 | INRDP75, « Composition d'une ligne » |
| Facteur de répétition fixe dans un format d'AFFICHER | 1 à 62 | 1 à 99 | INRDP75, note 7 du chapitre « Instruction de sortie. Format » (« Ce facteur est limité à 62 sur T1600, à 99 sur Mitra 15 ») |
| Mots réservés propres au profil | `DAT`, `TEM` | `SUPPRIMER`, `REPRENDRE` | INRDP75 Annexe VI |
| Déclarations (`TABLEAU`, `CHAINE`, `LIBERER`) en mode machine de bureau | admises | rejetées (`ERR_INSTRUCTION_INTERDITE_BUREAU`, §8) | INRDP75 : « Sur MITRA certaines instructions ne sont pas autorisées en mode machine de bureau, notamment les déclarations » |

**Productions propres au T1600.** La grammaire reconstruite ne couvre que le MITRA 15 (RCI-M §4.11.1) ; sous le profil T1600, l'analyseur-générateur reconnaît en outre, avec la même technique de descente récursive (IMP-0164) :
- `SH(e)`, `CH(e)`, `TH(e)`, `SGN(e)`, au rang des appels de fonction (§5, « Précédence et associativité »), générés comme les fonctions élémentaires : sous-séquence de `e`, puis l'opération 0x3C à 0x3F ;
- l'instruction `UN id` ou `ZER id` (INRDP75 Annexe III, `2 UN U;ZER T`), générée comme une référence simple à `id` (RCI-M §4.5, 0x00) suivie de l'opération 0x1A ou 0x1B ; la catégorie tableau de `id` est vérifiée à l'exécution ;
- `$` et `@` comme atomes d'expression, générés comme les références 0x03 et 0x02 (RCI-M §4.5) ;
- `TEM` et `DAT` sans parenthèses, au rang des appels de fonction (RCI-M 0x0F, 0x27) ;
- sous `LSE F4`, `TEM()` et `DAT()` avec parenthèses, générés comme `TEM` et `DAT` (IMP-0189) ;
- sous `LSE F4`, une constante-chaîne composée de morceaux entre apostrophes et d'équivalents numériques `.n.`, en opérande comme en libellé de format (IMP-0190) ;
- sous `LSE F4`, `FAIRE n1 TANT QUE c`, sans `POUR` (RCI-M IMP-0277).

  Sous le profil MITRA 15, `@` suivi d'un entier littéral, à la place d'une constante (`I←@50`), est généré comme la valeur 0x06 portant ce numéro de ligne (RCI-M §4.4, règle `lineRefLiteral` de la grammaire reconstruite).

**Mots réservés.** Un identificateur ne peut pas être un mot réservé (LSE15 §3-1.2 ; INRDP75, note 3 : « TANT ne peut être un identificateur »). La liste appliquée est celle d'INRDP75, Annexe VI : `AFFICHER`, `ALLER`, `ALORS`, `CHAINE`, `CHARGER`, `DEBUT`, `ET`, `EXECUTER`, `FAIRE`, `FIN`, `GARER`, `JUSQUA`, `LIBERER`, `LIRE`, `LOCAL`, `OU`, `PAS`, `PAUSE`, `POUR`, `PROCEDURE`, `RESULTAT`, `RETOUR`, `SI`, `SINON`, `TABLEAU`, `TANT`, `TERMINER`, plus `DAT` et `TEM` sous le profil T1600, `SUPPRIMER` et `REPRENDRE` sous le profil MITRA 15. LSE15 §3-1.2 déclare au contraire tous les noms de fonctions réservés (« SIN — Identificateur illégal car mot réservé du langage ») ; ce désaccord factuel entre les deux manuels est résolu en faveur d'INRDP75, conformément à la règle de préséance (RCI-M §4.11.1). Un nom de fonction qui n'appartient pas à la liste (`SIN`, `LGR`…) peut donc être un identificateur : suivi d'une parenthèse ouvrante, il désigne la fonction ; sinon, un identificateur ordinaire (`SIN←3`, `SIN[2]`). Sous le profil T1600, `DAT` et `TEM`, qui s'écrivent sans parenthèses, sont réservés et désignent toujours la fonction. La grammaire reconstruite, qui suit LSE15 sur ce point, traite les noms de fonctions comme des jetons réservés ; le compilateur s'en écarte ici, la grammaire n'étant pas une source historique (§1.3).

**Bornes dépendantes du profil.** Le numéro d'une ligne de programme (§9, étape 3) et le facteur de répétition fixe d'une spécification de format (§5, génération d'AFFICHER) sont contrôlés selon le profil ; un dépassement produit `ERR_HORS_PROFIL`. La représentation physique (RCI-M IMP-0116, IMP-0125) couvre les deux profils sans changement.

**Différences d'exécution répertoriées.** Les différences suivantes, documentées par le corpus mais sans effet sur la compilation, sont répertoriées ici pour les chapitres qui en ont la charge : plage de l'argument d'`EQC` (« inférieure à 128 (sur T1600) ou 256 (sur M15) », INRDP75 l. 1416 ; RCI-M 0x26) ; longueur maximale des chaînes (INRDP75 Annexe III §4) ; message de terminaison (`TERMINE EN LIGNE 017` sur T1600, `TERMINE` sur MITRA 15, INRDP75 l. 1972) ; affichage d'un tableau entier selon une spécification de format, présenté par INRDP75 comme une possibilité du T1600 (« sur T1600 on peut remplacer les lignes 4 et 5 de ce programme par l'instruction `AFFICHER[F5.0]A` »), à confronter à LSE15 §6-2.5 dans le chapitre du formateur des sorties.


**Formes propres à `LSE F4`.** Les sorties du T1600 du lycée Victor Duruy (1980-1983, bannière `LSE F4`, `02-HISTORIQUE/Releve-sorties-T1600-Victor-Duruy.md`) attestent trois formes absentes d'INRDP75 : `TEM()` (IMP-0189, avec `DAT()`, qui la suit), `.n.` (IMP-0190) et `FAIRE n1 TANT QUE c` (RCI-M IMP-0277). Le profil `LSE F4` les admet en plus des formes d'INRDP75, sans en retirer aucune ; le profil T1600 version 4 s'en tient à INRDP75 et les rejette (choix d'Éric du 2026-11-01). On ne sait pas si `F4` désigne la « version 4 » d'INRDP75. Les autres évolutions attestées par LAFOND83 et MEMENTO82 restent hors du périmètre des profils : type `BOOLEEN`, tableaux de chaînes ou de booléens, `LIRE` avec format, constantes `.VRAI.` et `.FAUX.`. Une ligne qui les emploie est rejetée comme toute construction non reconnue.

**IMP-0189** — *`TEM()` et `DAT()` admis sous le profil `LSE F4`, en plus de `TEM` et `DAT`*
- Sujet : le T1600 de Victor Duruy accepte `TEM()` avec parenthèses (`104 Q←TEM()`, `105 FAIRE 105 TANT QUE TEM()-Q<4`, 4EME - SINUS), alors qu'INRDP75 (l. 850) écrit : « Sur T1600 la fonction TEM est sans parenthésage ». Quel profil accepte cette forme ?
- Choix : les deux fonctions ont le même comportement dans chaque profil. Sous le profil `LSE F4`, `TEM` et `TEM()`, `DAT` et `DAT()` sont acceptés et compilent le même élément (RCI-M opérations TEM et DAT). La décompilation restitue `TEM()` et `DAT()` : `TEM()` est la seule forme attestée à Victor Duruy, `SINUS` 104 et 105 étant des sorties de `PERFØRER`, donc du décompilateur de la machine (restitution selon le profil, RCI-M IMP-0183). Aucune sortie n'emploie `DAT`, qui suit `TEM`. La forme sans parenthèses est ainsi une graphie d'entrée, héritée d'INRDP75 et normalisée à la restitution. Sous le profil T1600 version 4, `TEM()` et `DAT()` sont rejetés (C 103), comme le veut INRDP75. Sous le profil MITRA 15, `TEM()` et `DAT()` restent les graphies de ces fonctions.
- Alternatives envisagées :
  1. Rejeter `TEM()` sous tous les profils T1600 — écarté : une forme attestée de première main sur la machine serait refusée, et les programmes conservés ne se rechargeraient pas.
  2. Conserver la graphie écrite pour la restituer — écarté : il faudrait une information supplémentaire dans RCI-M pour une différence sans effet.
  3. Restituer `TEM` sous `LSE F4`, forme d'INRDP75 — écarté : le listing de `SINUS` restitue `TEM()`.
  4. Laisser `DAT` sans parenthèses sous `LSE F4`, faute d'attestation — écarté : ce serait une incohérence entre deux fonctions de même forme (choix d'Éric du 2026-11-01).
  5. Admettre `TEM()` aussi sous le T1600 version 4 — écarté : INRDP75 décrit ce profil sans parenthèses, et il reste le témoin de ce manuel (choix d'Éric du 2026-11-01).
- Justification : EXI-0061 demande que « les différences explicitement documentées pour le T1600 » puissent être sélectionnées par le profil ; les sorties de Victor Duruy en sont, pour `LSE F4`, une documentation explicite de première main. L'ajout ne retire aucune forme d'INRDP75.
- Impact mémoire : nul.
- Impact disque : sans objet.
- Impact performances : nul.
- Impact historique : un programme de Victor Duruy qui écrit `TEM()` est accepté sous `LSE F4` et restitué à l'identique ; le listing de `SINUS` est reproduit.
- Références normatives : EXI-0061 ; INRDP75 l. 850 ; relevé T1600 de Victor Duruy, §3 ; IMP-0180, IMP-0276 ; RCI-M IMP-0183.
- Statut : retenu (choix d'Éric du 2026-10-22 ; révisé le 2026-11-01 : profil `LSE F4` seulement, restitution avec parenthèses, point 4 de la liste des modifications vers le LSE de Victor Duruy, et `DAT` aligné sur `TEM`).

**IMP-0190** — *Équivalents numériques `.n.` dans une constante-chaîne, sous le profil `LSE F4`*
- Sujet : le T1600 de Victor Duruy admet, dans une constante-chaîne, des caractères désignés par leur équivalent numérique entre deux points : `AFFICHER .27 12.` (ESC puis FF), et des morceaux accolés sans `!` (`'…APRECIAY'.8.'TIØN…'`, 8 étant le retour arrière). La notation vaut aussi dans un format : `50 AFFICHER[/,' A'.7.'U'.7.' '.7.'R'.7.'E'.7.'V'.7.'Ø'.7.'I'.7.'R'.7 7 7.]` (5EME - MULT, `numerisation0015.pdf`, sortie de `PERFØRER`, qui atteste à la fois l'acceptation et la restitution). MEMENTO82 (C 08) et LAFOND83, d'après Éric, attestent la même notation. INRDP75 et LSE15 l'ignorent ; LSE15 (l. 517) renvoie à `EQC` pour les caractères non imprimables. Comment l'admettre ?
- Choix :
  1. **Syntaxe (profil `LSE F4`).** Une constante-chaîne est une suite d'un ou plusieurs morceaux accolés, sans espace entre eux. Chaque morceau est :
     - soit un texte entre apostrophes (§5 bis, règle 6) ;
     - soit un équivalent numérique : un point, un ou plusieurs entiers décimaux séparés par un ou plusieurs espaces, puis un point (`.8.`, `.27 12.`).

     Le point en tête d'opérande est sans ambiguïté, une constante numérique commençant toujours par un chiffre (règle `NUMBER` de la grammaire reconstruite ; LSE15 §3-1.1). Il l'est aussi en tête d'une spécification de format, qu'aucune autre forme ne commence par un point.
  2. **Portée.** La notation est admise partout où figure une constante-chaîne : opérande d'expression et libellé de format. Un libellé suit exactement les règles d'une constante-chaîne : même lecture (`lire_chaine`, §5), même restitution (point 7). Il peut donc commencer par un équivalent (`[.7 7.]`) et porter un facteur (`[3.7.]`), qui s'applique au libellé entier.
  3. **Bornes.** Chaque entier a les bornes de l'argument d'`EQC` sous le profil, soit de 0 à 127 (INRDP75, l. 1416).
  4. **Valeur.** La constante est la concaténation des morceaux : le texte entre apostrophes, et un caractère par entier, de code égal à l'entier. En opérande, elle est compilée comme toute constante-chaîne (RCI-M élément 0x03 ; IMP-0181) ; en libellé, comme tout libellé (RCI-M élément 0x30). RCI-M n'est pas modifiée.
  5. **Profils T1600 version 4 et MITRA 15.** Un point en tête d'opérande ou de spécification de format y est rejeté comme construction propre à un autre profil (C 103).
  6. **Diagnostics** (natures, chapitre Gestionnaire des erreurs) :
     - point final manquant, ou caractère autre qu'un chiffre ou un espace entre les deux points : C 08 (« caractère de fin d'équivalent numérique » manquant) ;
     - entier hors des bornes d'`EQC` : C 109 ;
     - notation hors de `LSE F4` : C 103.
  7. **Restitution.** Sous `LSE F4`, dans le texte d'une constante-chaîne ou d'un libellé, tout caractère non imprimable (code 0 à 31, ou 127) est restitué sous forme d'équivalent numérique. Des caractères non imprimables consécutifs sont regroupés en un seul morceau (`.27 12.`). Les autres caractères sont restitués entre apostrophes, l'apostrophe étant doublée. Un texte sans caractère imprimable est restitué sans apostrophes (`.27 12.`). Un caractère imprimable écrit par son équivalent (`.65.`) est restitué en clair (`'A'`) : c'est une forme canonique (RCI-M IMP-0183). Sous les deux autres profils, tout caractère est restitué tel quel entre les apostrophes.
- Alternatives envisagées :
  1. Rejeter la notation sous tous les profils T1600 — écarté : une forme attestée de première main sur la machine serait refusée.
  2. L'admettre aussi sous le T1600 version 4 — écarté : INRDP75 l'ignore, et ce profil reste le témoin de ce manuel (choix d'Éric du 2026-11-01).
  3. L'admettre aussi sous MITRA 15 — écarté : rien ne l'atteste sur cette machine, et LSE15 renvoie à `EQC`.
  4. La réserver aux opérandes d'expression — écarté : `MULT` l'atteste dans un format, et aucune source ne distingue les deux emplois.
  5. N'admettre dans un format qu'un libellé commençant par une apostrophe, seule forme attestée — écarté : ce serait une seconde règle pour un même objet, sans source.
  6. Admettre des espaces entre les morceaux accolés — écarté : rien ne l'atteste, et la séparation de deux opérandes deviendrait ambiguë.
- Justification : la notation est attestée de première main à Victor Duruy, et par deux autres sources. Elle ne change que le lexique, sans toucher la représentation compilée ni la sémantique des chaînes. Ses bornes s'alignent sur `EQC`, la fonction qui produit les mêmes caractères à l'exécution.
- Impact mémoire : nul en représentation (une chaîne ordinaire).
- Impact disque : sans objet.
- Impact performances : négligeable, à la compilation et à la restitution.
- Impact historique : les programmes de Victor Duruy qui emploient `.n.` se compilent sous `LSE F4`, et leur restitution redonne la notation pour les caractères non imprimables, comme le listing de `MULT`.
- Références normatives : EXI-0061 ; relevé T1600 de Victor Duruy, §3 ; MEMENTO82, annexe I (C 08) ; LAFOND83 (d'après Éric, page non versée) ; INRDP75 l. 1416 ; LSE15 l. 517, §3-1.1 ; RCI-M IMP-0128, IMP-0183 ; IMP-0180, IMP-0181, IMP-0182, IMP-0185, IMP-0276.
- Statut : retenu (choix d'Éric du 2026-10-22 ; révisé le 2026-11-01 : profil `LSE F4` seulement, et libellés de format).

---

## 9. Algorithme complet de compilation d'une ligne (réalisation d'ALG-0901 / ALG-0902, pour les deux formes de ligne)

```text
compilateur_compiler(texte, longueur, resultat, diagnostic)

1.  SI longueur > 79                                            { INRDP75 : 80 caractères, fin de
                                                                   ligne comprise, IMP-0125 }
        diagnostic->position ← 0 ; diagnostic->code ← ERR_LIGNE_TROP_LONGUE
        RETOURNER échec
    FIN SI

2.  position ← 0 ; sauter les espaces initiaux éventuels

3.  SI texte[position] est un chiffre décimal                    { VM-3201, IMP-0171 }
        mode_bureau ← FAUX
        numero_ligne ← analyser_numero_ligne(texte, &position, diagnostic)
                                                                 { 1 à 250 ou 1 à 255 selon le
                                                                   profil, §8 bis ; sinon
                                                                   ERR_HORS_PROFIL }
        exiger au moins un espace, ou un '*' immédiatement   { §5 bis, IMP-0182 ;
                                                                   sinon ERR_SYNTAXE }
        SI échec  RETOURNER échec                                { EXI-0018 }
        FIN SI
    SINON
        mode_bureau ← VRAI                                       { ligne machine de bureau }
    FIN SI

4.  tampon[255] ← non initialisé                                 { §6, IMP-0165 ; toute écriture
                                                                   contrôle la place restante :
                                                                   ERR_LIGNE_TROP_COMPLEXE }
    curseur_tampon ← 0

5.  SI position < longueur ET texte[position] n'est pas un espace ET pas un commentaire
        SI NON mode_bureau ET mot-clé PROCEDURE en tête de ligne   { §13, LSE15 §7-6, [H] }
            analyser_generer_declaration_procedure(texte, &position, tampon,
                                                    &curseur_tampon, diagnostic)
                                                                   { §13.4, IMP-0175 : écrit
                                                                     l'élément 0x0D, IMP-0174 }
            SI échec  RETOURNER échec
            FIN SI
            SI texte[position] = ';'                               { suite ordinaire de la ligne,
                                                                     LSE15 §7-6 }
                position ← position + 1
                analyser_generer_instructions(texte, &position, tampon, &curseur_tampon,
                                               mode_bureau, diagnostic)
                SI échec  RETOURNER échec
                FIN SI
            FIN SI
        SINON
            analyser_generer_instructions(texte, &position, tampon, &curseur_tampon,
                                           mode_bureau, diagnostic)  { §5, §6, §8 — VM-3202/VM-3203 }
            SI échec  RETOURNER échec                              { EXI-0004 : tampon abandonné, rien publié }
            FIN SI
        FIN SI
    SINON
        { ligne vide après le numéro, ou commentaire seul : §5, RCI-M §4.9 — ligne machine
          de bureau entièrement vide après d'éventuels espaces : aucun élément généré,
          ce qui est accepté et ne produit aucun effet observable }
        analyser_generer_commentaire_eventuel(...)
    FIN SI

6.  SI position < longueur                                        { du texte inattendu subsiste ;
                                                                   nature selon §9 bis, IMP-0191 }
        diagnostic->position ← position ; diagnostic->code ← ERR_SYNTAXE
        RETOURNER échec
    FIN SI

7.  bloc ← objets_creer(curseur_tampon)                            { Gestionnaire d'objets §11 }
    SI bloc = GESTOBJ_HANDLE_NUL
        diagnostic->position ← 0 ; diagnostic->code ← ERR_MEMOIRE_INSUFFISANTE
        RETOURNER échec                                            { Tome III §2.3.11 }
    FIN SI

8.  contenu ← objets_resoudre(bloc)
    copier tampon[0 .. curseur_tampon-1] dans contenu
    objets_relacher(bloc)

9.  SI mode_bureau
        resultat->type ← LSE_LIGNE_BUREAU                          { RCI-M IMP-0109 }
        resultat->resultat.bloc_bureau ← bloc                      { jamais inséré au programme courant }
    SINON
        resultat->type ← LSE_LIGNE_PROGRAMME
        resultat->resultat.ligne_programme.numero_ligne ← numero_ligne
        resultat->resultat.ligne_programme.longueur_octets ← curseur_tampon
        resultat->resultat.ligne_programme.bloc ← bloc              { EXI-0020 : un objet compilé exactement }
    FIN SI
    RETOURNER succès
```

**Étapes 7-8 : pourquoi la copie a lieu ici et pas ailleurs, pour les deux formes de ligne.** C'est le compilateur, seul détenteur du tampon de génération et seul à en connaître la longueur exacte au moment où elle est enfin connue, qui appelle `objets_creer` — jamais le programme courant (`programme.c`, VM-3204) ni l'interpréteur (`interprete.c`, VM-0702), qui ne reçoivent jamais qu'un résultat déjà entièrement finalisé (une `EntreeObjetCompile` ou un `HandleObjet` transitoire selon le cas), ni le tampon brut sous-jacent. Cette frontière est formalisée au §12 (IMP-0170), distinctement pour chacune des deux formes.

**Diagnostic et atomicité, pour les deux formes de ligne.** Toute sortie en échec, à quelque étape que ce soit des étapes 1 à 7, laisse `*resultat` inchangé et ne laisse subsister, au pire, que des entrées inertes ajoutées en table : un symbole (§7), ou une constante partagée de plus de 16 caractères avec son objet texte (IMP-0181, IMP-0107) — jamais un objet compilé, jamais un bloc transitoire (l'échec de l'étape 7 elle-même est le seul cas où `objets_creer` a été appelé sans qu'un résultat ne soit finalement publié ; comme cet appel échoue par construction dans ce cas précis — code retour `GESTOBJ_HANDLE_NUL` — aucun `HandleObjet` réel n'est jamais laissé sans propriétaire). C'est la réalisation directe d'EXI-0004 et de l'invariant IV-IF-0001 (« en cas d'échec, aucun objet compilé n'est publié dans le programme courant »), étendue sans exception à l'échec d'une ligne machine de bureau : une instruction interdite détectée à l'étape 5 (§8, IMP-0172) produit un diagnostic exactement comme une erreur de syntaxe ordinaire, sans jamais atteindre l'étape 7.

---

## 9 bis. Natures de diagnostic : précisions et position

**IMP-0191** — *Règles de détection complémentaires (identificateur trop long, constante hors plage, bornes, fin de ligne inattendue, ressources) et position du diagnostic*

- **Sujet :** l'inventaire des diagnostics (`05-IMPLEMENTATION/audits/Inventaire_diagnostics_compilation_2026-10-22.md`, observations O1 à O6) relève des situations que le présent chapitre ne traitait pas, ou traitait sous un code qui ne correspond pas à la correction attendue. Quelles règles appliquer, et où placer la position du diagnostic ?
- **Choix :**
  1. **Identificateur trop long (O1).** Une suite de lettres et de chiffres commençant par une lettre, plus longue que cinq caractères, qui n'est pas un mot du langage (§5 bis, IMP-0182), est rejetée : nature « identificateur de plus de 5 caractères », C 03. Un nom de procédure dont la partie qui suit `&` dépasse cinq caractères est rejeté : nature « nom de procédure trop long », C 05. Aucune troncature n'est faite.
  2. **Constante hors plage (O2).** Une constante numérique dont la valeur absolue dépasse le plus grand nombre représentable en `float` SANE est rejetée : nature « constante réelle hors plage », C 09. Une constante non nulle plus petite, en valeur absolue, que le plus petit nombre représentable n'est pas rejetée : elle prend la valeur que donne la conversion SANE.
  3. **Bornes (O3).**
     - Numéro de ligne nul, ou supérieur à 32000 : nature « numéro de ligne hors des bornes », C 52.
     - Numéro de 251 à 32000 sous MITRA 15, de 256 à 32000 sous T1600 : nature « numéro de ligne au-delà de la borne du profil », numéro à partir de 100.
     - Facteur de répétition nul, au-delà de la borne du profil, ou supérieur à 99 : trois natures, toutes C 41 (IMP-0187).
  4. **Texte qui subsiste après une instruction complète (O4, §9 étape 6).** Le caractère rencontré détermine la nature :
     - `)` : nature « parenthèse fermante en excès », C 45 ;
     - un caractère qui peut commencer un opérande (lettre, chiffre, `'`, `(`, `&`, ou `.` sous T1600) : nature « opérateur manquant », C 19 ;
     - une lettre minuscule : nature « lettre minuscule hors d'une chaîne », C 01 ;
     - un caractère étranger au jeu du langage (`5 A←B%C`, inventaire D-08) : nature « caractère spécial interdit », C 01 ;
     - tout autre caractère : nature « instruction incorrecte », C 53.
  5. **`?` sur une ligne numérotée (O4).** Nature « `?` réservé au mode machine de bureau », numéro à partir de 100, au lieu d'un diagnostic de syntaxe ordinaire (IMP-0173). `PROCEDURE` sur une ligne non numérotée relève de C 58 (inventaire D-58).
  6. **Ressources (O5).** L'échec d'une création d'objet (§9, étape 7), de la création du texte d'une constante partagée (IMP-0181) ou de l'agrandissement de la table des symboles ou de la table des constantes (IMP-0167, IMP-0181) donne la nature « plus de place dans la zone utilisateur », C 57, domaine `COMPILATION`. L'ajout d'une entrée au-delà du plafond de l'une de ces tables, 6 553 symboles ou 21 845 constantes (RCI-M IMP-0104) donne la nature « capacité des tables atteinte », numéro à partir de 100.
  7. **Position (O6).** `DiagnosticCompilation.position` est la position, dans le texte reçu, du premier caractère que l'analyseur ne peut pas accepter à l'endroit où il le rencontre. Pour une erreur constatée sur une construction déjà lue (identificateur trop long, constante hors plage, doublon, contrôle de type, interdiction du mode machine de bureau), c'est la position du premier caractère de cette construction. Pour une erreur de ressource, la position est 0. La position n'est jamais affichée (IMP-0186) : elle sert aux tests et à la mise au point.
- **Alternatives envisagées :**
  1. Tronquer un identificateur trop long à cinq caractères — écarté : deux identificateurs distincts seraient confondus en silence ; le rejet est attesté sur T1600 (`AFFICHEP`, `ERREUR  C 03`) et par LAFOND83 (`AFICHER`, `ERREUR  C 03`).
  2. Saturer une constante hors plage au plus grand nombre représentable, comme `CNB` (LSE15, l. 825) — écarté : l'erreur est entièrement déterminée par le texte de la ligne (EXI-0018, EXI-0051), et MEMENTO82 la range parmi les erreurs de compilation (C 09).
  3. Afficher la position — écarté : aucune attestation (IMP-0186).
- **Justification :** chaque règle sépare des situations qui appellent des corrections différentes (IMP-0187). Elle les rattache au numéro MEMENTO82 dont le libellé les couvre, ou à un numéro à partir de 100 lorsqu'aucun ne convient (IMP-0185).
- **Impact mémoire :** nul.
- **Impact disque :** sans objet.
- **Impact performances :** négligeable.
- **Impact historique :** les rejets C 03 et C 52 sont attestés ou documentés ; les autres précisent des situations sur lesquelles le corpus des deux profils est muet, sans rejeter aucune ligne valide du corpus.
- **Références normatives :** LSE15 §3-1.1, §3-1.2, §7-2, l. 825 ; INRDP75, « Règle pour les identificateurs », « Composition d'une ligne » ; LAFOND83 p. 19 ; relevé T1600 de Victor Duruy ; MEMENTO82, annexe I (C 03, C 05, C 09, C 19, C 41, C 45, C 52, C 53, C 57) ; EXI-0018, EXI-0051 ; IMP-0168, IMP-0173, IMP-0181, IMP-0182, IMP-0185, IMP-0186, IMP-0187 ; RCI-M IMP-0104, IMP-0128.
- **Statut :** retenu (choix d'Éric du 2026-10-22).

---

## 10. Compilation indépendante des lignes (réalisation d'EXI-0001 / Tome IV §9.3)

L'algorithme du §9 ne lit, à aucune étape, une autre entrée de la table des objets compilés que celle en cours de production (lorsqu'il s'agit d'une ligne de programme), et ne consulte la table des symboles (§7) que pour les noms effectivement présents dans le texte de la ligne courante. Aucune variable statique ou globale du module `compilateur.c` ne conserve d'état d'une invocation à l'autre de `compilateur_compiler` — le tampon de génération (§6) est local à chaque appel (IMP-0165), la variable `mode_bureau` (§8) est elle-même locale à l'appel en cours et ne survit jamais au-delà de lui, et la table des symboles, bien que persistante pour tout le programme courant, n'est jamais lue en fonction de la ligne précédemment compilée mais uniquement en fonction du texte de la ligne courante. La compilation de la ligne `n1` d'une boucle `FAIRE` (RCI-M §4.8.2, IMP-0112) produit ainsi rigoureusement la même séquence, que la ligne `FAIRE` correspondante soit déjà compilée, pas encore compilée, ou compilée après elle — cas déjà couvert comme test de non-régression par RCI-M §14, directement applicable ici puisqu'aucune structure propre à ce chapitre n'introduit de dépendance supplémentaire entre lignes. Une ligne machine de bureau compilée entre deux lignes de programme (par exemple pour interroger une variable en cours de mise au point, §12, PAS À PAS) ne modifie, de la même façon, aucun état susceptible d'influencer la compilation ultérieure d'une ligne de programme, hormis les entrées de symboles qu'elle a pu créer — lesquelles sont, par construction (§7), les mêmes entrées que si le même identificateur avait été introduit par une ligne de programme.

---

## 11. Compilation issue d'ENCODER (réalisation de VM-3205 / Tome IV §9.4)

**IMP-0169** — *Point d'entrée unique `compilateur_compiler`, partagé sans distinction par le moniteur et le bibliothécaire*
- Sujet : Tome III §9.5 (VM-3205) exige que « la provenance physique de la ligne ne modifie pas sa sémantique de compilation » et que « ENCODER peut être architecturalement réalisé sans introduire un second compilateur ». Comment garantir physiquement cette absence de second compilateur ?
- Choix : `compilateur.h` n'expose qu'une seule fonction de compilation (§4), sans variante ni paramètre distinguant l'origine de la ligne. Le bibliothécaire (`bibliotheque.c`, chapitre distinct non encore rédigé), lorsqu'il traite `ENCODER`, lit chaque ligne d'un fichier-données et appelle `compilateur_compiler` exactement comme le fait le moniteur pour une ligne frappée au clavier — avec la seule différence que la chaîne `texte` provient d'un enregistrement lu (Gestionnaire des fichiers-données, chapitre distinct) plutôt que d'un tampon clavier. Comme pour le moniteur, la distinction entre ligne de programme et ligne machine de bureau (§4, IMP-0171) reste entièrement à la charge de `compilateur_compiler` lui-même : un fichier-programme destiné à `ENCODER` ne contient normalement que des lignes numérotées, mais rien dans ce chapitre n'empêche physiquement une ligne non numérotée d'y figurer — son traitement suit alors exactement le même chemin qu'une ligne machine de bureau tapée au clavier, la décision d'accepter ou non une telle ligne dans ce contexte précis relevant du bibliothécaire, hors périmètre de ce chapitre.
- Alternatives envisagées : une seconde fonction d'entrée, par exemple `compilateur_compiler_encodage`, identique en tout point sauf le nom — écarté : dupliquerait l'interface sans aucune différence de comportement, ce qui contredirait directement le texte même de VM-3205 et introduirait un risque de divergence future entre deux points d'entrée censés être rigoureusement équivalents.
- Justification : l'absence de toute branche conditionnelle sur l'origine, à l'intérieur même de `compilateur_compiler` (§9), est la preuve la plus directe possible qu'aucun second compilateur n'existe — la fonction elle-même ne peut pas distinguer ses deux appelants, puisqu'elle ne reçoit jamais cette information.
- Impact mémoire : nul — pas de code dupliqué.
- Impact disque : sans objet.
- Impact performances : identique dans les deux cas d'usage.
- Impact historique : réalise fidèlement VM-3205 et, par construction, EXI-0021.
- Références normatives : Tome III §9.5 (VM-3205) ; EXI-0021 ; Tome IV §9.4 (« ENCODER réutilise le même algorithme de compilation que l'entrée interactive. La seule différence est la source des lignes »).
- Statut : retenu.

---

## 12. Acceptation atomique et frontière avec le programme courant et l'interpréteur (VM-3204, VM-0702 — hors périmètre)

**IMP-0170** — *Le compilateur ne livre jamais qu'un résultat finalisé ; il ne manipule jamais la table des objets compilés et n'exécute jamais rien*
- Sujet : où placer exactement la frontière entre le module `compilateur.c` et, d'une part, le module `programme.c` (VM-3204, gestion du programme courant) pour une ligne de programme, d'autre part le module `interprete.c` (VM-0702, exécution immédiate) pour une ligne machine de bureau — tous distincts selon Organisation du dépôt §2.1 ?
- Choix : `compilateur_compiler` (§9) s'arrête très précisément à la production d'un `ResultatCompilation` valide et complet (§4), quelle que soit sa forme :
  - pour une ligne de programme (`type = LSE_LIGNE_PROGRAMME`), elle ne recherche jamais un numéro de ligne existant, ne compare jamais deux objets compilés, ne décide jamais d'un ajout ou d'un remplacement : ces décisions (RCI-M §5, IMP-0106 ; Tome III §9.4, VM-3204) appartiennent exclusivement à `programme.c`, que l'appelant (moniteur ou bibliothécaire) invoque séparément, après un appel réussi à `compilateur_compiler`, via l'interface logique déjà fixée par Tome IV (IV-IF-0002, `INSERER_LIGNE`) ;
  - pour une ligne machine de bureau (`type = LSE_LIGNE_BUREAU`), elle ne transmet jamais elle-même le bloc transitoire à l'interpréteur et ne déclenche aucune exécution : c'est le moniteur qui, recevant un `ResultatCompilation` de ce type, invoque séparément l'interpréteur (VM-0702, interface distincte de celle du mode programmé VM-0703, chapitre `interprete.c` non encore rédigé) en lui passant le `HandleObjet` transitoire, puis, une fois l'exécution terminée, appelle `objets_liberer` sur ce même handle (RCI-M IMP-0109 : « puis libéré après exécution ») — ni cette invocation, ni cette libération, ne relèvent du compilateur.
- Alternatives envisagées :
  1. Faire appeler directement `programme_inserer_ligne` depuis l'intérieur même de `compilateur_compiler`, en fin de traitement réussi d'une ligne de programme — écarté : romprait la séparation de responsabilités déjà actée par Tome III (§2.3.3, « il remet son résultat au gestionnaire du programme courant » — c'est-à-dire qu'il le *remet*, sans l'intégrer lui-même) et par Organisation du dépôt (deux fichiers sources distincts), et empêcherait un appelant de choisir de ne pas insérer un objet compilé pourtant valide (cas hypothétique d'une future commande de simple vérification syntaxique sans effet, non exclue par le corpus).
  2. Faire exécuter directement une ligne machine de bureau par `compilateur_compiler` elle-même, en appelant l'interpréteur en fin de traitement réussi — écarté explicitement par Note 1 (§0) : Tome III §7.2 (VM-0702) et §2.3.3 distinguent sans ambiguïté compilation et interprétation comme deux processus successifs, enchaînés par un tiers (le moniteur), jamais fusionnés en un seul composant ; fusionner les deux romprait de plus la frontière déjà fixée par Organisation du dépôt entre `compilateur.c` et `interprete.c`, et interdirait toute réutilisation du compilateur par un futur composant qui voudrait seulement vérifier la validité syntaxique d'une ligne machine de bureau sans l'exécuter.
- Justification : cette frontière reprend au mot près la formulation de Tome III §2.3.3 et du diagramme ANN-F-0001 (Tome IV), qui placent explicitement une étape entre COMPILATEUR et PROGRAMME COURANT plutôt qu'une fusion des deux, et applique la même discipline de séparation, par analogie directe, à la frontière entre COMPILATEUR et MOTEUR D'EXÉCUTION/INTERPRÉTEUR que trace ANN-F-0002 (Tome IV) pour le mode d'exécution en général.
- Impact mémoire : sans objet propre — cette décision ne fixe qu'une frontière d'appel, aucune structure de données.
- Impact disque : sans objet.
- Impact performances : un appel de fonction supplémentaire pour l'appelant, négligeable.
- Impact historique : réalise fidèlement Tome III §2.4 (« En cas de succès : Compilateur → Programme courant ») et §7.2 (VM-0702 : compilation puis interprétation puis exécution, comme deux étapes distinctes).
- Références normatives : Tome III §2.3.3, §2.4, §7.2 (VM-0702), §9.4 (VM-3204) ; Tome IV ANN-F-0001, ANN-F-0002, IV-IF-0002 ; RCI-M §3 bis, IMP-0109 ; Organisation du dépôt §2.1.
- Statut : retenu. La réalisation physique de `programme_inserer_ligne` et de l'invocation de l'interpréteur en mode machine de bureau restent, l'une comme l'autre, hors périmètre de ce chapitre (§18).

---

## 13. Déclaration de procédure et clause `LOCAL`

### 13.1 Propriétés imposées par le corpus et par les tomes

Le corpus historique fixe la forme et la portée de la déclaration de procédure sans ambiguïté sur les points suivants :

- **Forme.** « Une déclaration de procédure se compose du mot `PROCEDURE`, suivi du nom de la procédure que l'on veut définir, suivi d'une liste de paramètres (qui peut être vide...), encadrée par des parenthèses, suivie éventuellement d'une liste de `LOCAL` » (LSE15 §7-2) ; forme générale `n1 PROCEDURE &ID(pf1,pf2,...,pfn) LOCAL id1,id2,...,idk` (INRDP75, chapitre Procédures, §4 et §5). Une procédure « peut n'avoir aucun paramètre » (INRDP75, chapitre Procédures, §2, « Remarques importantes », 2 ; LSE15 §7-1, `PROCEDURE &FORM()`).
- **Nom.** Le caractère `&`, « suivi d'une lettre, puis d'au plus quatre lettres ou chiffres sans espace intermédiaire » (LSE15 §7-2 ; INRDP75 §2-1).
- **Paramètre formel.** « Un identificateur seul ou un nom de procédure, mais non, par exemple, une variable indicée » (LSE15 §7-3 ; INRDP75 §2-1 et sa note 9).
- **Clause `LOCAL`.** Le mot `LOCAL` « doit, lui-même, suivre, à des espaces près, la parenthèse fermante qui termine la liste de paramètres formels » (LSE15 §7-7). La liste qui le suit contient les paramètres que l'on veut locaux et, « de la même façon », les variables ou tableaux locaux à la procédure (LSE15 §7-9 ; INRDP75 §4, §5). Un nom de procédure n'y figure jamais : un paramètre formel nom de procédure est nécessairement un paramètre référence (LSE15 §7-8.2 ; `LSE15Mitra.g4` [I], règle `localItem`).
- **Mode de passage.** « Nous appellerons paramètre local tout paramètre formel d'une procédure figurant dans la liste qui suit le mot `LOCAL`... Tout paramètre formel ne figurant pas dans la liste de `LOCAL` sera appelé paramètre de référence » (LSE15 §7-7) ; « tout identificateur figurant dans la liste qui suit le mot LOCAL et non dans la liste des paramètres formels est un identificateur de variable locale » (INRDP75 §4). Le mode de passage est donc **entièrement déterminé par le texte de la seule ligne de déclaration**, ce qui rend sa compilation compatible avec EXI-0001.
- **Ordre de la liste `LOCAL`.** Aucune source n'impose d'ordre entre paramètres locaux et variables locales dans cette liste : la forme d'INRDP75 §5 (`LOCAL pf1,...,pfi,id1,...,idk`) est schématique, comme le montrent `&UNIT(A,N) LOCAL N,I,J` (le paramètre local n'est pas le premier paramètre formel) et `&TRAC(&F,MIN,MAX,PAS) LOCAL MIN,MAX,PAS,...` (LSE15 §7-8.2). L'ordre écrit est donc conservé tel quel, sans normalisation.
- **Position.** « Une déclaration de procédure doit se trouver au début d'une ligne qui doit être numérotée comme les autres lignes, bien qu'on ne puisse jamais l'exécuter directement, ou par l'intermédiaire d'un `ALLER EN` » ; « toutes les instructions LSE sont permises à la suite d'une déclaration de procédure, sur la même ligne (en mettant un point-virgule pour séparer la déclaration de procédure de la suite) » (LSE15 §7-6 ; `LSE15Mitra.g4` [H], règle `statementList`). « L'instruction de déclaration d'une procédure n'est pas exécutable » (INRDP75, chapitre Procédures, §2, « Remarques importantes », 1).
- **Déclarations multiples.** « Si deux procédures de même nom sont déclarées sur deux lignes différentes, seule la déclaration située sur la ligne de plus grand numéro sera prise en compte » (LSE15 §7-6, Note).

Les tomes de conception fixent, de leur côté, le contenu abstrait de ce que la compilation de cette ligne doit produire : « La déclaration crée une définition compilée », laquelle contient « un identificateur ; une liste de paramètres ; un ensemble de règles de passage des paramètres ; un corps compilé ; une définition des objets locaux ; une règle de retour » (Tomes I-II §13.1, VM-1301, EXI-0042). RCI-M, qui fixe l'entrée `SYM_PROCEDURE` (IMP-0114 : numéro de ligne d'entrée et arité), ne prévoit en revanche aucun élément pour porter la liste des paramètres, leurs règles de passage et les objets locaux. Le présent paragraphe comble cette lacune (§13.2), en dérive la structure consultée à l'appel (§13.3), fixe l'algorithme d'analyse de la ligne (§13.4) et la frontière avec le programme courant (§13.5).

### 13.2 Élément compilé de déclaration

**IMP-0174** — *Élément de contrôle « déclaration de procédure » (sous-code 0x0D), premier élément de la séquence compilée de la ligne de déclaration*
- Sujet : quelle représentation compilée produit la ligne `n PROCEDURE &ID(pf1,...,pfn) LOCAL l1,...,lm` ? Trois exigences convergent : VM-1301 (la déclaration crée une définition compilée comprenant liste de paramètres, règles de passage et objets locaux) ; EXI-0010 et EXI-0015 (LISTER doit restituer la déclaration à partir de la seule représentation compilée, le texte source n'étant pas conservé, EXI-0022) ; LSE15 §7-6, Note (après la suppression d'une ligne de déclaration, les autres déclarations du même nom doivent rester identifiables pour que la déclaration de plus grand numéro restante soit prise en compte).
- Choix : la ligne compile, comme **premier élément** de sa séquence, un élément de la catégorie information de contrôle (`RCIM_CAT_CONTROLE`), sous-code **0x0D**, de charge utile :

```text
tag 0xCD                       RCIM_CAT_CONTROLE | 0x0D
2 octets                       identifiant du symbole &ID (SYM_PROCEDURE, RCI-M IMP-0104)
1 octet   n                    nombre de paramètres formels — l'arité (0 à 255)
n × 2 octets                   identifiants des paramètres formels, dans l'ordre du texte
1 octet   m                    nombre d'identificateurs de la liste LOCAL (0 = clause absente)
m × 2 octets                   identifiants de la liste LOCAL, dans l'ordre du texte
```

  Le mode de passage n'est pas codé séparément : il découle de la définition même de LSE15 §7-7. Un paramètre formel est `LOCAL` si et seulement si son identifiant figure aussi dans la liste `LOCAL` ; sinon, c'est un paramètre référence. Un identifiant de la liste `LOCAL` absent de la liste des paramètres désigne une variable ou un tableau local (LSE15 §7-9 ; INRDP75 §4). La valeur `m = 0` ne peut désigner que l'absence de clause, la grammaire exigeant au moins un identificateur après `LOCAL`. Les instructions écrites après `;` sur la même ligne suivent cet élément dans la même séquence, compilées normalement (§9, étape 5).

  L'élément n'a aucun effet exécutable. L'appel positionne l'exécution sur l'élément qui le suit immédiatement (Etat d'exécution §5.5, étape 7) ; le cycle d'interprétation ne le rencontre donc que si la ligne de déclaration est atteinte autrement que par un appel — exécution directe interdite par LSE15 §7-6 et signalée comme erreur d'exécution (Etat d'exécution IMP-0155). Le décompilateur restitue `PROCEDURE &ID(pf1,...,pfn)`, suivi de ` LOCAL l1,...,lm` si `m > 0`, chaque nom étant lu dans la table des symboles et précédé de `&` lorsque sa nature est `SYM_PROCEDURE` (RCI-M IMP-0124, §6.2).
- Alternatives envisagées :
  1. Aucun élément dans la séquence : informations portées seulement par la table des paramètres formels (§13.3) et par `InfoProcedure` — écarté. LISTER dépendrait d'une structure distincte de la représentation compilée, contrairement à EXI-0010. RCI-S devrait sérialiser cette structure à part. Une ligne de déclaration ne pourrait pas non plus être reconstruite après restauration, faute de texte source à recompiler (EXI-0022). C'est exactement l'argument qui a fait placer le commentaire dans la séquence plutôt que dans une table indexée par numéro de ligne (RCI-M IMP-0113).
  2. Un indicateur de mode par paramètre (un bit ou un octet), suivi des seules variables locales non-paramètres, au lieu de la liste `LOCAL` complète — écarté. Cette forme perd l'ordre textuel de la liste `LOCAL` et l'entrelacement des paramètres locaux et des variables locales (`LOCAL N,I,J` n'est pas `LOCAL I,J,N`), qu'aucune source ne déclare indifférent pour LISTER (§13.1). Le gain, deux octets par paramètre local au plus, ne justifie pas une restitution infidèle.
  3. Un sous-code de la catégorie « opération codée » — écarté. Cette catégorie est entièrement occupée (RCI-M §4.11.8), et une déclaration n'est pas une opération : comme le commentaire (0x0C), c'est une information de contrôle et de décompilation (VM-2401, « informations nécessaires à la décompilation »).
  4. Porter la liste des paramètres et des locaux dans l'entrée `SYM_PROCEDURE` — écarté. RCI-M §6.3 exclut explicitement cette option, et une entrée de symbole unique ne pourrait pas porter plusieurs déclarations simultanément présentes d'un même nom (LSE15 §7-6, Note).
- Justification : l'élément réalise directement la « définition compilée » de VM-1301 dans la représentation compilée elle-même, seule source de vérité du contenu d'une ligne (RCI-M IMP-0114, justification de robustesse). Il prélève le premier sous-code libre de la catégorie information de contrôle (13 sous-codes sur 64 occupés, RCI-M §4.11.8). Il ne modifie aucun sous-code existant.
- Impact mémoire : 5 + 2(n + m) octets par ligne de déclaration — 5 octets pour `&FORM()`, 15 octets pour `&PRO(A,B) LOCAL A,M,N`. La limite de 80 caractères borne n + m à 32, soit au plus 69 octets, très en deçà du tampon de génération (§6, IMP-0165). Aucun coût pour les lignes qui ne sont pas des déclarations.
- Impact disque : l'élément est sérialisé avec la séquence de la ligne, comme tout élément RCI-M (RCI-S, chapitre distinct) ; aucune structure auxiliaire n'est à sérialiser (§13.3).
- Impact performances : nul pendant l'exécution normale (l'appel saute l'élément) ; un décodage par insertion de la ligne dans le programme courant (§13.5).
- Impact historique : restitue fidèlement la déclaration au listage, y compris l'ordre de la liste `LOCAL`, sans aucune incidence sur le comportement observable.
- Références normatives : Tomes I-II §13.1 (VM-1301, EXI-0042) ; EXI-0010, EXI-0015, EXI-0016, EXI-0022 ; LSE15 §7-2, §7-3, §7-6, §7-7, §7-9 ; INRDP75, chapitre Procédures, §2-1, §4, §5 ; RCI-M §4.7, §4.9.2 (IMP-0113), §4.11.8, §6.3 (IMP-0114), IMP-0104, IMP-0124.
- Statut : retenu. Le sous-code 0x0D est reporté dans la table de RCI-M §4.7.

### 13.3 Table des paramètres formels

**IMP-0147** — *Table des paramètres formels, index dérivé de l'élément de déclaration, distinct de RCI-M*
- Sujet : à chaque appel, l'interpréteur doit savoir, paramètre par paramètre, si l'argument déjà déposé sur la pile d'évaluation doit être copié dans une sauvegarde (paramètre `LOCAL`) ou lié dans un alias (paramètre référence), et quels identificateurs `LOCAL` non-paramètres doivent être sauvegardés puis libérés (Etat d'exécution §5.4, §5.5). Sous quelle forme cette information est-elle consultée à l'appel ?
- Choix : une structure auxiliaire, tenue par le gestionnaire du programme courant (`programme.c`) en parallèle de la table des symboles (RCI-M §6). Elle contient une entrée par ligne de déclaration présente dans le programme courant, indexée par le numéro de cette ligne, c'est-à-dire par la même clé que `InfoProcedure.numero_ligne_entree` (RCI-M IMP-0114) :

```c
typedef unsigned char ModeParametre;
#define PARAM_LOCAL      0
#define PARAM_REFERENCE  1

typedef struct {
    unsigned short   symbole;   /* identifiant du nom formel, table des symboles du programme courant */
    ModeParametre     mode;
} ParametreFormel;

typedef struct {
    unsigned char     numero_ligne_entree;   /* ligne portant la déclaration ; clé de correspondance
                                                  avec InfoProcedure, IMP-0114 */
    unsigned short    symbole_procedure;     /* identifiant du nom &ID déclaré sur cette ligne (§13.5) */
    ParametreFormel  *parametres;             /* n = arité de la déclaration ; ordre du texte */
    unsigned char      nb_parametres;
    unsigned short   *locaux_non_parametres;  /* identificateurs LOCAL du corps qui ne sont pas
                                                  des paramètres formels (LSE15 §7-9), ordre du texte */
    unsigned char      nb_locaux_non_parametres;
} TableParametresFormels;
```

  Cette table est **construite par le gestionnaire du programme courant** lorsqu'il intègre une ligne dont la séquence commence par l'élément 0x0D (§13.5, IMP-0176), par simple décodage de cet élément (IMP-0174). Chaque paramètre reçoit `PARAM_LOCAL` si son identifiant figure dans la liste `LOCAL`, `PARAM_REFERENCE` sinon ; `locaux_non_parametres` reçoit la liste `LOCAL` privée des paramètres, dans son ordre. L'entrée est retirée lorsque la ligne est supprimée ou remplacée. Le compilateur n'y écrit jamais (§13.5). L'interpréteur la consulte au moment de l'appel (Etat d'exécution §5.5, étape 2) à partir du `numero_ligne_entree` de l'`InfoProcedure` en vigueur. Une déclaration masquée par une déclaration de même nom et de plus grand numéro (LSE15 §7-6, Note) conserve son entrée : c'est elle qui redevient en vigueur si la ligne qui la masque disparaît (§13.5).
- Alternatives envisagées :
  1. Coder le mode de passage directement dans le sous-code de la référence RCI-M compilée à l'intérieur du corps — écarté. Ce choix romprait IMP-0120 : une référence identifie *quel* symbole est visé, non le rôle qu'il joue dans la déclaration englobante. Rouvrir RCI-M pour cette distinction imposerait en outre de renuméroter une catégorie déjà entièrement occupée (RCI-M §4.11.8).
  2. Déduire le mode de passage, à l'exécution, de la forme physique de l'argument déjà empilé (`VALEUR` ou `REFERENCE`) plutôt que de le lire dans la déclaration — écarté. Un argument réduit à une variable simple non indexée compile toujours une `REFERENCE`, quel que soit le mode attendu par le paramètre formel. LSE15 admet justement qu'une expression quelconque, y compris réduite à une simple variable, soit passée à un paramètre `LOCAL`. La forme de l'argument ne permet donc pas de déduire sans ambiguïté le mode attendu : celui-ci est fixé par la déclaration, non déduit de l'appel.
  3. Pas de table : redécoder l'élément 0x0D de la ligne d'entrée à chaque appel — écarté. Chaque appel devrait résoudre et verrouiller le bloc compilé de la ligne d'entrée, puis tester l'appartenance de chaque paramètre à la liste `LOCAL`, soit O(n·m) par appel, sur l'opération la plus coûteuse du cycle d'interprétation. La table énumère en outre directement les lignes de déclaration d'un même nom, nécessaires à l'application de LSE15 §7-6, Note, sans parcours de tout le programme courant (§13.5).
- Justification : cette table applique au mode de passage le principe déjà retenu par RCI-M IMP-0114 pour l'identification des procédures, à savoir une clé de correspondance stable (`numero_ligne_entree`) vers une structure auxiliaire plutôt qu'une extension de l'entrée de symbole. Elle n'est jamais une source de vérité : elle est redondante par construction avec les éléments 0x0D des lignes de déclaration présentes, et peut être reconstruite à tout moment à partir d'eux.
- Impact mémoire : proportionnel au nombre de lignes de déclaration présentes et à leur arité, comparable à celui de la table des symboles elle-même, jamais un majorant global.
- Impact disque : la table n'est pas sérialisée. À la restauration d'un fichier-programme, elle est reconstruite par décodage des éléments 0x0D des lignes restaurées, sans aucune recompilation (point à reprendre par le chapitre RCI-S, non encore rédigé).
- Impact performances : à chaque appel, une recherche par `numero_ligne_entree`, dichotomique si la table est triée par cette clé (même base que RCI-M IMP-0106), ou par correspondance directe si le nombre de procédures reste petit (cas pédagogique typique) ; un décodage d'élément par insertion de ligne de déclaration.
- Impact historique : couvre fidèlement la distinction `LOCAL`/référence documentée par LSE15 §7-7 à §7-9, sans laquelle ce comportement central du langage (passage de paramètres, modification d'une variable du programme appelant) serait irréalisable.
- Références normatives : LSE15 §7-6, §7-7, §7-8, §7-8.1, §7-8.2, §7-9 ; INRDP75, chapitre Procédures, §4, §5 ; RCI-M §6.3, IMP-0104, IMP-0106, IMP-0114, IMP-0120 ; IMP-0174, IMP-0176 ; Etat d'exécution §5.4, §5.5 ; conditionnement §2, §15.
- Statut : retenu.

### 13.4 Analyse de la ligne de déclaration et de sa clause `LOCAL`

**IMP-0175** — *Analyse de la déclaration de procédure en tête de ligne de programme, génération de l'élément 0x0D après reconnaissance complète, rejet des doublons et d'un paramètre homonyme de la procédure*
- Sujet : par quel algorithme le compilateur reconnaît-il la déclaration `PROCEDURE &ID(...) LOCAL ...`, vérifie-t-il les contraintes syntaxiques du §13.1 et produit-il l'élément de déclaration (IMP-0174) ? Que faire d'un nom répété dans la liste des paramètres ou dans la liste `LOCAL`, ou d'un paramètre formel qui porte le nom de la procédure déclarée (`PROCEDURE &P(&P)`), cas sur lesquels le corpus des deux profils est muet ?
- Choix : l'étape 5 de l'algorithme général (§9) appelle `analyser_generer_declaration_procedure` lorsque, sur une ligne de programme, le premier élément après le numéro commence par le mot-clé `PROCEDURE`. Le mot `PROCEDURE` rencontré à toute autre position (après un `;`, ou sur une ligne machine de bureau) n'est reconnu par aucune production et produit un diagnostic de syntaxe ordinaire (`LSE15Mitra.g4` [H] ; §8). L'algorithme est le suivant :

```text
analyser_generer_declaration_procedure(texte, position, tampon, curseur_tampon, diagnostic)
{ précondition : ligne de programme (mode_bureau faux) ; *position sur le mot-clé PROCEDURE,
  première instruction de la ligne (§9, étape 5) ; *curseur_tampon = 0 }

1.  consommer PROCEDURE
2.  lire un nom de procédure : '&', une lettre, au plus quatre lettres    { LSE15 §7-2 }
    ou chiffres ; sinon : ERR_SYNTAXE
    sym_proc ← chercher_ou_creer(nom, avec '&')                            { §7, IMP-0167 }
3.  exiger '(' ; n ← 0
    SI le caractère suivant n'est pas ')'                                  { liste vide admise }
        RÉPÉTER
            lire un identificateur, avec ou sans '&' ; sinon : ERR_SYNTAXE { LSE15 §7-3 }
            SI le caractère suivant est '['  → ERR_SYNTAXE                 { « non... une variable
                                                                             indicée » }
            s ← chercher_ou_creer(nom, avec ou sans '&')
            SI s = sym_proc  → ERR_DECLARATION_PROCEDURE                    { paramètre homonyme
                                                                             de la procédure }
            SI s ∈ parametres[0 .. n-1]  → ERR_DECLARATION_PROCEDURE        { doublon }
            parametres[n] ← s ; n ← n + 1
        TANT QUE le caractère suivant est ',' (consommé)
    exiger ')'
4.  sauter les espaces ; m ← 0                                              { « à des espaces près »,
    SI le mot LOCAL suit                                                      LSE15 §7-7 }
        consommer LOCAL
        RÉPÉTER
            lire un identificateur sans '&'                                 { [I] : un nom de procédure
              ; sinon : ERR_SYNTAXE                                           → ERR_SYNTAXE }
            l ← chercher_ou_creer(nom, sans '&')
            SI l ∈ locaux[0 .. m-1]  → ERR_DECLARATION_PROCEDURE            { doublon }
            locaux[m] ← l ; m ← m + 1
        TANT QUE le caractère suivant est ',' (consommé)
5.  sauter les espaces ; SI ni fin de ligne ni ';'  → ERR_SYNTAXE            { la suite éventuelle,
                                                                               après ';', est traitée
                                                                               par l'appelant, §9 }
6.  rcim_ecrire_declaration_procedure(tampon, curseur_tampon,
                                      sym_proc, n, parametres, m, locaux)  { IMP-0174 }
7.  RETOURNER succès
```

  `parametres[]` et `locaux[]` sont deux tableaux automatiques de 32 identifiants 16 bits, taille que la limite de 80 caractères par ligne ne permet pas de dépasser (§13.2, impact mémoire). Comme toute autre fonction de l'analyseur-générateur (§5), celle-ci ne conserve aucun état au-delà de l'appel (EXI-0001). Elle n'écrit dans le tampon qu'à l'étape 6, une fois la déclaration entièrement reconnue. Les identificateurs qu'elle crée en table des symboles avant un éventuel échec restent des entrées inertes (§3). Un identificateur sans `&` nouvellement créé reçoit la nature `SYM_IDENTIFICATEUR` (§7) : la catégorie effective d'un paramètre, qui peut désigner une variable simple, un tableau ou une variable-chaîne, n'est connue qu'à l'exécution, par liaison à l'argument effectif (RCI-M §6.2 ; Etat d'exécution §3 bis ; INRDP75, chapitre Procédures, §2, « Remarques importantes », 3).

  **Doublons.** Un même nom figurant deux fois dans la liste des paramètres formels, ou deux fois dans la liste `LOCAL`, produit le diagnostic `ERR_DECLARATION_PROCEDURE`. Ce code est provisoire au sens d'IMP-0168, comme `ERR_INSTRUCTION_INTERDITE_BUREAU` (§8). La présence d'un même nom dans les deux listes n'est pas un doublon : c'est précisément la marque d'un paramètre local (LSE15 §7-7).

  **Paramètre homonyme de la procédure.** Un paramètre formel nom de procédure identique au nom de la procédure déclarée (`PROCEDURE &P(&P)`) produit le même diagnostic. Seul un paramètre écrit avec `&` peut être concerné, la table des symboles séparant les noms avec et sans `&` (IMP-0167).

  **Natures et numéros** (chapitre Gestionnaire des erreurs, IMP-0185, IMP-0187) :
  - doublon de paramètres variables, ou doublon dans la liste `LOCAL` : C 47 ;
  - doublon de paramètres noms de procédure, ou paramètre homonyme de la procédure : C 48.

  La répartition suit MEMENTO82 (annexe I, C 47 et C 48), qui atteste ces rejets à la compilation sur une autre implantation. Chaque cas est une nature distincte.
- Alternatives envisagées :
  1. Accepter silencieusement un paramètre formel répété — écarté. Le modèle à entrée de symbole unique par nom (ANN-K-0009, Etat d'exécution §5.4) associerait alors deux arguments effectifs au même identificateur, par deux sauvegardes ou deux alias successifs. La valeur visible dans le corps dépendrait de l'ordre de liaison, et l'argument effectif modifié par un paramètre référence répété serait indéterminé. Aucune source ne décrit ce comportement, et le corpus ne contient aucun exemple de déclaration de ce type.
  2. Accepter un nom répété dans la liste `LOCAL` — envisagé, car sans danger à l'exécution : deux sauvegardes du même identificateur, restaurées dans l'ordre inverse, rendent la valeur initiale. Écarté par cohérence avec le cas précédent et parce que la répétition n'a aucun sens défini par le corpus. Ce point est à confronter au corpus de niveau B s'il devient accessible (§18).
  3. Accepter un paramètre homonyme de la procédure — écarté : dans le corps, `&P` désignerait le paramètre et non plus la procédure, ce qui rend la récursion impossible sans qu'aucune source ne décrive ce masquage. MEMENTO82 (C 48) rejette cette forme.
  4. Différer la vérification des doublons à l'exécution — écarté. Le doublon est entièrement déterminable par le texte de la seule ligne. EXI-0018 et EXI-0051 demandent qu'une erreur détectable à la compilation y soit diagnostiquée immédiatement.
- Justification : l'algorithme transcrit directement les règles de LSE15 §7-2, §7-3, §7-6 et §7-7, ainsi que les règles `procedureDecl`, `formalParam` et `localItem` de la grammaire reconstruite, dans la même technique de descente récursive que le reste du compilateur (IMP-0164). Il ne constitue pas une exception à la passe unique : il est invoqué depuis l'intérieur de celle-ci et écrit dans le même tampon.
- Impact mémoire : 128 octets de pile C au plus (deux tableaux de 32 identifiants), libérés au retour.
- Impact disque : sans objet.
- Impact performances : O(n² + m²) comparaisons d'identifiants pour la détection des doublons, n + m ≤ 32 — négligeable.
- Impact historique : réalise la syntaxe documentée. Le rejet des doublons et du paramètre homonyme n'est pas attesté par le corpus des deux profils : il refuse des formes dont aucune source de ces profils ne décrit le sens, sans retirer aucune construction historiquement documentée. MEMENTO82 atteste les mêmes rejets sur une autre implantation.
- Références normatives : LSE15 §7-2, §7-3, §7-6, §7-7, §7-8.2, §7-9 ; INRDP75, chapitre Procédures, §2-1, §2 (« Remarques importantes ») ; `LSE15Mitra.g4`, règles `statementList`, `procedureDecl`, `formalParam`, `localClause`, `localItem`, commentaires [H], [I] ; MEMENTO82, annexe I (C 47, C 48) ; EXI-0001, EXI-0018, EXI-0051 ; IMP-0164, IMP-0167, IMP-0168, IMP-0174, IMP-0185, IMP-0187.
- Statut : retenu (révisé le 2026-10-22 : paramètre homonyme de la procédure ; natures et numéros).

### 13.5 Frontière avec le programme courant ; vérifications laissées à l'exécution

**IMP-0176** — *La compilation d'une déclaration ne modifie ni `InfoProcedure` ni la table des paramètres formels ; le programme courant les met à jour à l'intégration de la ligne*
- Sujet : qui fixe le contenu de l'entrée `SYM_PROCEDURE` (`InfoProcedure` : numéro de ligne d'entrée, arité, RCI-M IMP-0114) et de la table des paramètres formels (IMP-0147), et à quel moment ? Une écriture au moment de la compilation contredirait IMP-0170 (le compilateur ne modifie pas le programme courant) et EXI-0004 : une ligne rejetée, ou acceptée par le compilateur mais jamais insérée par l'appelant, aurait déjà changé la procédure en vigueur. Elle ne permettrait pas non plus d'appliquer LSE15 §7-6, Note (la déclaration de plus grand numéro est prise en compte), qui suppose de connaître les autres lignes du programme et entrerait donc en conflit avec EXI-0001.
- Choix : à la compilation, le compilateur se limite à chercher ou créer les entrées de symboles des noms de la ligne (§7). Une entrée `SYM_PROCEDURE` nouvellement créée reçoit `InfoProcedure = {0, 0}`, valeur qui signifie « aucune déclaration en vigueur » (le numéro de ligne 0 est hors de la plage 1-255 des lignes, RCI-M IMP-0125). Le compilateur écrit ensuite l'élément 0x0D en tête du tampon (IMP-0174). La description de la déclaration voyage ainsi dans le `ResultatCompilation` lui-même (§4), comme premier élément de `ligne_programme.bloc`, sans champ supplémentaire. Le gestionnaire du programme courant (`programme.c`, VM-3204) applique ensuite, au moment où il insère, remplace ou supprime la ligne `L`, le contrat suivant :

```text
INTEGRER_DECLARATION(L, ancien_objet, nouvel_objet)
{ ancien_objet : objet compilé précédemment présent en ligne L, ou absent ;
  nouvel_objet : objet compilé inséré en ligne L, ou absent (suppression) }

1. SI ancien_objet présent ET son premier élément est 0x0D (procédure P)
       retirer de la table des paramètres formels l'entrée de la ligne L
       SI InfoProcedure(P).numero_ligne_entree = L
           InfoProcedure(P) ← { L', n' } de la déclaration de P de plus grand numéro L'
                               restant dans la table (entrées où symbole_procedure = P),
                               ou { 0, 0 } s'il n'en reste aucune
       FIN SI
   FIN SI
2. SI nouvel_objet présent ET son premier élément est 0x0D (procédure Q, arité n)
       ajouter l'entrée de la ligne L, décodée de l'élément (IMP-0147)
       SI InfoProcedure(Q).numero_ligne_entree < L        { 0 compris : aucune déclaration }
           InfoProcedure(Q) ← { L, n }                     { LSE15 §7-6, Note }
       FIN SI
   FIN SI
```

  L'appel d'une procédure dont `InfoProcedure.numero_ligne_entree` vaut 0 est une erreur d'exécution (Etat d'exécution §5.5, étape 1). Cette situation se produit lorsque la procédure n'est déclarée nulle part dans le programme courant, ou lorsque toutes ses déclarations ont été supprimées.

  **Vérifications laissées à l'exécution.** Lorsqu'il compile un appel `&P(e1,...,ek)`, le compilateur ne vérifie ni que `k` égale l'arité de `&P`, ni que l'argument `ei` est une variable, un nom de tableau ou un nom de procédure lorsque le paramètre formel correspondant est un paramètre référence (LSE15 §7-8.2, Remarque 1). Sous EXI-0001, la ligne d'appel peut en effet être compilée avant la ligne de déclaration, après elle ou en son absence. La déclaration en vigueur peut en outre changer après la compilation de l'appel (LSE15 §7-6, Note), sans que la ligne d'appel soit recompilée. Ces vérifications relèvent exclusivement de l'interpréteur, au moment de l'appel (Etat d'exécution §5.5, étapes 1 et 4 ; EXI-0019).

  **Production de l'élément APPEL.** Pour que la vérification du nombre d'arguments (LSE15 §7-8) soit possible à l'exécution, le compilateur grave dans l'élément APPEL le nombre `k` de paramètres effectifs qu'il vient de reconnaître entre les parenthèses de l'appel (RCI-M 0x28, IMP-0177). Il produit, dans l'ordre : la référence simple à `&P` (RCI-M §4.5, sous-code 0x00) ; les sous-séquences des `k` arguments, dans l'ordre du texte ; puis l'élément `0x28` suivi de l'octet indicateur de position (0x00 en position d'instruction, 0x01 en position d'expression, RCI-M IMP-0121) et de l'octet `k`. Les règles de grammaire `procedureCall` et `effectiveParamList` fixent `k` sans ambiguïté ; `&P()` donne `k = 0`. Une ligne de 80 caractères ne peut pas porter plus de quelques dizaines d'arguments, bien en deçà de la limite de 255 représentable par l'octet.
- Alternatives envisagées :
  1. Faire écrire `InfoProcedure` par le compilateur dès la compilation de la déclaration — écarté pour les raisons exposées dans le sujet (IMP-0170, EXI-0004, EXI-0001).
  2. Retenir la dernière déclaration compilée ou insérée plutôt que celle de plus grand numéro — écarté : contredit LSE15 §7-6, Note.
  3. Rejeter, à la compilation ou à l'insertion, une seconde déclaration d'un nom déjà déclaré — écarté : LSE15 §7-6, Note, admet explicitement cette situation et en fixe la règle de résolution.
- Justification : la frontière reste celle d'IMP-0170 (le compilateur remet un résultat finalisé, le programme courant l'intègre). La règle de résolution des déclarations multiples s'applique là où toutes les lignes sont connues. La table des paramètres formels et `InfoProcedure` sont mises à jour dans la même opération que la table des objets compilés, ce qui garantit leur cohérence mutuelle.
- Impact mémoire : nul au-delà de la table des paramètres formels (IMP-0147).
- Impact disque : sans objet ; à la restauration d'un fichier-programme, le même contrat s'applique à chaque ligne restaurée.
- Impact performances : un test sur le premier élément de chaque ligne insérée ou supprimée ; en cas de suppression de la déclaration en vigueur, un parcours en O(d) des entrées de la table, d étant le nombre de lignes de déclaration présentes.
- Impact historique : réalise LSE15 §7-6, Note, y compris lorsque la ligne de plus grand numéro est supprimée ou remplacée par une ligne qui n'est pas une déclaration.
- Références normatives : LSE15 §7-6 (et Note), §7-8, §7-8.2 ; Tome III §2.3.3, §9.4 (VM-3204) ; Tome IV IV-IF-0002 ; RCI-M §6.3 (IMP-0114), IMP-0125 ; EXI-0001, EXI-0004, EXI-0019 ; IMP-0147, IMP-0170, IMP-0174.
- Statut : retenu. La réalisation de `INTEGRER_DECLARATION` relève du chapitre consacré au programme courant (`programme.c`, non encore rédigé) ; le présent chapitre en fixe le contrat.

---

## 14. Organisation mémoire physique (zones) — mise à jour

| Zone | Contenu | Remarque de dimensionnement |
|---|---|---|
| Tampon de génération | `unsigned char tampon[255]` (§6, IMP-0165) | Automatique (pile C), un par appel de compilation, jamais persistant — identique pour les deux formes de ligne |
| Table des symboles du programme courant | `EntreeSymbole[]` (RCI-M §6), croissance par `realloc` (Architecture physique IMP-0132/0133) | Proportionnelle au nombre de noms distincts réellement déclarés, qu'ils proviennent d'une ligne de programme ou d'une ligne machine de bureau ; recherche linéaire (§7, IMP-0167) |
| Table des constantes partagées | `EntreeConstante[]` (RCI-M §7, IMP-0107), alimentée par le compilateur (§7, IMP-0181) | Une entrée par chaîne littérale distincte de plus de 16 caractères des lignes de programme ; jamais réduite en cours de session |
| Diagnostic de compilation | `DiagnosticCompilation` (§8, IMP-0168) | Automatique (pile C), sur la pile de l'appelant, jamais persistant |
| Bloc transitoire machine de bureau | `HandleObjet` isolé (§9, RCI-M IMP-0109) | Un par ligne machine de bureau compilée, jamais inséré dans la table des objets compilés, libéré par le moniteur après exécution (§12) — ne figure dans aucune zone du programme courant |
| Listes de travail de la déclaration | `parametres[32]`, `locaux[32]` (§13.4, IMP-0175) | Automatiques (pile C), 128 octets au plus, uniquement pendant l'analyse d'une ligne de déclaration |
| Table des paramètres formels | `TableParametresFormels[]` (§13.3, IMP-0147) | Une entrée par ligne de déclaration présente, proportionnelle à l'arité ; tenue par le gestionnaire du programme courant, jamais écrite par le compilateur (IMP-0176) |

Ces éléments appartiennent, dans le plan de segmentation d'Architecture physique §5.2, au segment `"compilateur"` pour la partie `noyau/compilateur.c/.h`, cohérent avec la correspondance déjà établie par Organisation du dépôt §2.1 — à l'exception de la table des paramètres formels, qui relève du segment `"programme"` du gestionnaire du programme courant (`programme.c/.h`, Organisation du dépôt §2.1), seul à l'écrire.

---

## 15. Correspondance avec les invariants

| Invariant / exigence | Réalisation physique correspondante |
|---|---|
| EXI-0001 (compilation indépendante des lignes) | Aucun état statique persistant entre deux appels de `compilateur_compiler`, y compris `mode_bureau` (§10) |
| EXI-0004 (rejet atomique) | Tampon de génération local, jamais résolu en `HandleObjet` avant succès complet, pour les deux formes de ligne (§9, étapes 1-7) |
| EXI-0011 / EXI-0047 (ordre post-fixé) | Conséquence mécanique de l'ordre des appels de la descente récursive (§5, IMP-0164) |
| EXI-0016 (compacité) | Aucune structure supplémentaire de ce chapitre ne duplique une information déjà portée par RCI-M ou par la table des symboles ; `?` ne produit aucun sous-code distinct d'`AFFICHER` (§8, IMP-0173) |
| EXI-0018 (analyse préalable) | Le tampon n'est proposé à `objets_creer` qu'après reconnaissance intégrale et sans erreur de la ligne, y compris le respect des restrictions du mode machine de bureau (§9, étape 7) |
| EXI-0020 (un objet par ligne de programme) | Une seule `EntreeObjetCompile` produite par appel réussi sur une ligne de programme (§9, étape 9) |
| EXI-0021 (compilation uniforme) | Point d'entrée unique, sans branche selon l'origine (§11, IMP-0169) |
| EXI-0022 (indépendance du texte source) | `texte` n'est jamais conservé au-delà de la durée de l'appel (§4) |
| **EXI-0028 (enchaînement immédiat, mode machine de bureau)** | **Résultat distinct (`LSE_LIGNE_BUREAU`) jamais inséré au programme courant (§9, §12) ; compilation et exécution restent deux appels distincts, jamais fusionnés dans le compilateur (§12, IMP-0170)** |
| Tome III §2.3.3 (le compilateur ne modifie pas directement le programme courant) | `compilateur_compiler` ne touche jamais la table des objets compilés (§12, IMP-0170) |
| Tome III §7.2 (VM-0702 : compilation puis interprétation, deux étapes distinctes) | Le compilateur ne contient aucun appel vers `interprete.c` (§12, IMP-0170) |
| Tome III §9.1 (VM-3201 : distinction ligne de programme/entrée immédiate) | Réalisée par examen du premier caractère significatif, à l'intérieur même de `compilateur_compiler` (§4, IMP-0171) |
| Tome III §9.2 (VM-3202, aucune représentation partielle publiée) | Réalisé conjointement par le tampon local (§6) et l'arrêt immédiat sur échec à toute étape (§9) |
| RCI-M §3 bis (IMP-0109 : objets compilés transitoires du mode machine de bureau) | Bloc `HandleObjet` distinct, jamais indexé par numéro de ligne (§9, §14) |
| VM-1301 / EXI-0042 (la déclaration crée une définition compilée) | Élément 0x0D en tête de la séquence de la ligne de déclaration : identificateur, paramètres ordonnés, règles de passage, objets locaux (§13.2, IMP-0174) |
| EXI-0010 / EXI-0015 (décompilation depuis la seule représentation compilée) | La déclaration, y compris l'ordre de la liste `LOCAL`, est restituable à partir de l'élément 0x0D et de la table des symboles (§13.2) ; la table des paramètres formels n'est jamais nécessaire au décompilateur |
| LSE15 §7-6, Note (déclaration de plus grand numéro prise en compte) | Contrat d'intégration du programme courant (§13.5, IMP-0176) |
| EXI-0001 appliqué aux procédures | Le mode de passage est entièrement déterminé par le texte de la ligne de déclaration (§13.1) ; aucune vérification inter-lignes au site d'appel (§13.5) |
| LSE15 §6-2.4 (pas plus de spécifications de description que de valeurs à afficher) | Contrôle à la compilation de toute instruction AFFICHER ou `?` (§5, IMP-0178) |
| LSE15 §7-8 (autant de paramètres effectifs que de paramètres formels) | Le nombre `k` d'arguments écrits au site d'appel est gravé dans l'élément APPEL (§13.5 ; RCI-M IMP-0177) et comparé à l'arité en vigueur par l'interpréteur (Etat d'exécution §5.5, étape 1) |

---

## 16. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0164 | Analyse et génération fusionnées en une seule passe, sans arbre syntaxique matérialisé | retenu |
| IMP-0165 | Tampon de génération de taille fixe (255 octets), automatique, avec contrôle de débordement (`ERR_LIGNE_TROP_COMPLEXE`) | retenu |
| IMP-0166 | Correction différée (backpatching) des positions cibles de branchement intra-séquence, comptées depuis le début du bloc | retenu |
| IMP-0167 | Table des symboles : recherche linéaire, insertion en fin de table | retenu |
| IMP-0168 | Structure minimale provisoire du diagnostic de compilation | remplacée le 2026-10-23 par IMP-0195 (chapitre Gestionnaire des erreurs §8) ; les codes provisoires `ERR_*` du présent chapitre correspondent aux natures `ERR_C_*` du catalogue (§7 de ce chapitre) |
| IMP-0169 | Point d'entrée unique `compilateur_compiler`, partagé moniteur/bibliothécaire | retenu |
| IMP-0170 | Frontière compilateur/programme courant/interpréteur : livraison d'un résultat finalisé uniquement, jamais d'exécution | retenu |
| **IMP-0171** | **Distinction ligne de programme / ligne machine de bureau par examen du premier caractère (un seul point d'entrée)** | **retenu** |
| **IMP-0172** | **Paramètre `mode_bureau` récursif : interdiction des instructions à numéro de ligne et de l'appel de fonction en opérande, en mode machine de bureau** | **retenu** |
| **IMP-0173** | **`?` reconnu uniquement en mode machine de bureau, synonyme strict d'`AFFICHER`, même générateur** | **retenu** |
| IMP-0147 | Table des paramètres formels, index dérivé de l'élément de déclaration, tenu par le programme courant | retenu |
| IMP-0174 | Élément de contrôle « déclaration de procédure » (sous-code 0x0D) en tête de la ligne de déclaration | retenu |
| IMP-0175 | Analyse de la déclaration de procédure et de sa clause `LOCAL` ; rejet des doublons | retenu |
| IMP-0176 | La compilation d'une déclaration ne modifie ni `InfoProcedure` ni la table des paramètres formels ; contrat d'intégration du programme courant | retenu |
| IMP-0178 | Contrôle, à la compilation, qu'une instruction AFFICHER ne comporte pas plus de spécifications de description que de valeurs à afficher (LSE15 §6-2.4) | retenu |
| IMP-0180 | Profil d'implantation (`LSE F4`, T1600 version 4, MITRA 15 version 8) choisi au lancement, fixe pour la session ; profil strict ; constructions et mots réservés propres à chaque profil | retenu |
| IMP-0181 | Chaînes littérales : au-delà du seuil, toujours par la table des constantes partagées (ligne de programme) ; toujours en ligne en mode machine de bureau | retenu |
| IMP-0182 | Règles lexicales : reconnaissance des mots, signe des constantes, espace après le numéro, lettres majuscules seulement hors chaînes et commentaires | retenu |

Ce chapitre referme ainsi le point laissé implicite par RCI-M (algorithme de production non prescrit, y compris pour le bloc transitoire d'IMP-0109), le point laissé ouvert par RCI-M §6.3 (représentation compilée de la ligne de déclaration de procédure, paramètres formels et clause `LOCAL`, §13), et le point de simple localisation d'Organisation du dépôt §2.1 (IMP-0136). IMP-0147, numérotée dans la plage du chapitre État d'exécution (IMP-0144 à IMP-0155), est rédigée dans le présent chapitre (§13.3), lieu de la déclaration dont la table est dérivée.

---

## 17. Traçabilité

```text
Tome III §2.3.3, §2.5.1, §7.2, §9 (VM-0501 à VM-0507, VM-0702, VM-3201 à VM-3205)
  → Tome IV §2.2 (IV-IF-0001), §9 (ALG-0901 à ALG-0905), ANN-F-0001, ANN-F-0002, ANN-F-0005, ANN-I-0001, ANN-I-0002
    → RCI-M (format cible complet, IMP-0101 à IMP-0128, et IMP-0109 — bloc transitoire machine de bureau)
    → Gestionnaire d'objets (IMP-0157 à IMP-0163, interface §11)
    → Architecture physique §6.1 (IMP-0132, IMP-0133 — allocation de la table des symboles)
        → Organisation du dépôt §2.1 (IMP-0136, localisation)
        → `LSE15Mitra.g4` (commentaires [J], [K] — distinction `line`/`bureauLine`, `allowProc`)
          → IMP-0164 à IMP-0173, IMP-0178, IMP-0180 à IMP-0182 (présent chapitre)
            → CODE : src/noyau/compilateur.c, src/noyau/compilateur.h

HIS-13.1, HIS-13.2 (LSE15 §7-2 à §7-9 ; INRDP75, chapitre Procédures, §2, §4, §5)
  → EXI-0042, EXI-0010, EXI-0015, EXI-0001
    → VM-1301 (Tomes I-II §13.1, « La déclaration crée une définition compilée »)
      → RCI-M §4.7, §6.3 (IMP-0114 — entrée SYM_PROCEDURE)
        → `LSE15Mitra.g4` (règles procedureDecl, formalParam, localClause, localItem ; [H], [I])
          → IMP-0147, IMP-0174 à IMP-0176 (présent chapitre, §13)
            → CODE : src/noyau/compilateur.c (analyse, élément 0x0D), src/noyau/rcim.c
                     (écriture/décodage de 0x0D), src/noyau/programme.c (table des paramètres
                     formels, INTEGRER_DECLARATION)
              → usage à l'appel : Etat d'exécution §5.5 (src/noyau/interprete.c, contexte_exec.c)
```

---

## 18. Limites explicites de ce chapitre

- Ne couvre pas la réalisation de `INTEGRER_DECLARATION` ni la gestion physique de la table des paramètres formels (allocation, tri, recherche) par le gestionnaire du programme courant : chapitre `programme.c`, non encore rédigé. Le présent chapitre en fixe la structure (§13.3, IMP-0147) et le contrat (§13.5, IMP-0176).
- Le rejet des doublons dans la liste des paramètres formels et dans la liste `LOCAL` (§13.4, IMP-0175) n'est attesté par aucune source examinée ; il est à confronter, le cas échéant, au corpus de niveau B (conditionnement §22).
- Ne couvre pas les procédures externes (LSE15 §7-7, dernier alinéa : « Dans une procédure externe, les identificateurs non paramètres sont tous locaux ») : le mécanisme de chargement d'un module externe relève du bibliothécaire et du gestionnaire des tâches, chapitres distincts non encore rédigés.
- Ne couvre pas la réception des caractères, l'écho console, ni la détection de la fin de ligne au sens matériel (Tome III §9.1, VM-3201, partie non réalisée ici) : module `moniteur.c`/`console_iigs.c`, chapitre distinct du Tome V, non encore rédigé.
- Ne couvre pas l'intégration effective d'une `EntreeObjetCompile` produite dans la table des objets compilés du programme courant (Tome III §9.4, VM-3204 ; RCI-M §5, IMP-0106) : module `programme.c`, chapitre distinct du Tome V, non encore rédigé. Le présent chapitre en fixe uniquement la frontière d'appel (§12, IMP-0170).
- Ne couvre pas l'invocation de l'interpréteur pour une ligne machine de bureau, ni l'affichage du résultat d'une expression interrogée par `?` ou d'une affectation (LSE15 §9-5 : « cette valeur est également affichée »), ni la libération du bloc transitoire après exécution (Tome III §7.2, VM-0702 ; RCI-M IMP-0109) : module `interprete.c`/`moniteur.c`, chapitres distincts du Tome V, non encore rédigés. Le présent chapitre en fixe uniquement la frontière d'appel (§12, IMP-0170) et le format du bloc produit (§4, §9).
- Ne couvre pas la forme définitive du diagnostic de compilation ni l'énumération exhaustive des codes d'erreur, y compris `ERR_INSTRUCTION_INTERDITE_BUREAU` (§8), `ERR_DECLARATION_PROCEDURE` (§13.4), `ERR_FORMAT_DESCRIPTIONS` (§5, IMP-0178), `ERR_LIGNE_TROP_COMPLEXE` (§6, IMP-0165) et `ERR_HORS_PROFIL` (§8 bis, IMP-0180) : gestionnaire des erreurs, module `erreurs.c/.h`, chapitre distinct du Tome V, non encore rédigé. Le présent chapitre en fixe uniquement une structure minimale provisoire (§8, IMP-0168), non renumérotée lors de sa révision future.
- La grammaire précise reconnue par l'analyseur-générateur (§5, §8) n'est pas détaillée production par production dans ce chapitre : elle relève de `LSE15Mitra.g4` (grammaire reconstruite, dérivée du corpus historique, non elle-même une source historique de premier rang) et de RCI-M §4 (catalogue verrouillé des constructions couvertes) ; le présent chapitre fixe la technique de réalisation (descente récursive à génération directe, paramètre `mode_bureau`) sans reproduire ici l'ensemble des productions.
- La borne de 255 octets par séquence compilée (§6, IMP-0165 ; RCI-M IMP-0105) est garantie par le contrôle de débordement ; le majorant de RCI-M IMP-0125 (environ 155 octets pour la construction la plus dense identifiée) indique qu'aucune ligne du corpus historique examiné ne devrait l'atteindre, sans que cela ait été mesuré sur l'ensemble des programmes du corpus.
- La liste exacte des instructions interdites en mode machine de bureau (§8, IMP-0172) reprend celle établie par `LSE15Mitra.g4` [J], elle-même déduite d'INRDP75 et de LSE15 §9-5 ; une lecture complémentaire du corpus historique de niveau B (conditionnement §22, Niveau B) pourrait, le cas échéant, affiner ou confirmer cette liste — ce travail documentaire reste hors périmètre de ce chapitre d'implémentation (conditionnement §24).
- La structure `ConfigurationSession` (§8 bis, IMP-0180) n'est pas encore localisée dans l'organisation du dépôt (Organisation du dépôt §2.1) ; la lecture du paramètre de lancement relève de la couche plateforme, chapitre distinct. L'inscription du profil dans la provenance d'un fichier-programme (IV-FMT-0019) et le traitement d'un fichier-programme compilé sous un autre profil relèvent du chapitre RCI-S.
- La liste des mots réservés (§8 bis) applique INRDP75 Annexe VI, en écart avec LSE15 §3-1.2 sur les noms de fonctions ; une source complémentaire, s'il en apparaît, pourrait confirmer ou infirmer ce choix.
- Ne couvre pas la vérification de compatibilité de redéclaration d'un identificateur (`TABLEAU`, `CHAINE`, LSE15 §3-1.2, §4-3) : il s'agit d'une vérification d'exécution, fixée par les règles de transition de l'environnement des identificateurs (Etat d'exécution §3 bis, IMP-0179) ; l'allocation des tableaux et des chaînes relève du module `donnees`, chapitre distinct, non encore rédigé.

---

## 19. Tests associés

- Compilation de `5 A←B+C` : vérifier que le tampon de génération contient, dans l'ordre, les sous-séquences de `B`, de `C`, puis l'élément d'addition, puis la référence de `A` et l'élément d'affectation, conformément à RCI-M §14 — vérification directe de l'ordre post-fixé produit mécaniquement par la descente récursive (IMP-0164).
- Compilation de `200 FAIRE 202 POUR I←1 JUSQUA N`, puis de `200 FAIRE 202 POUR I←1 PAS 1 JUSQUA N` : vérifier que la première séquence contient l'élément 0x0E sans aucune sous-séquence de pas, et la seconde la valeur immédiate `1` suivie de 0x0A ; vérifier dans les deux cas l'ordre `n1`, `v1`, 0x09, [pas], `v2`, 0x0F (RCI-M IMP-0112).
- Compilation de `20 FAIRE 20 POUR X←A PAS (A/X-X)/2 TANT QUE ABS(X↑2-A)>1E-3` (LSE15 §5-5.3) : vérifier que les sous-séquences du pas et de la condition suivent l'élément d'ouverture 0x09, et que la séquence se termine par 0x10.
- Sous `LSE F4`, compilation de `105 FAIRE 105 TANT QUE A<4` : vérifier la séquence de V17, sans élément de pas (RCI-M IMP-0277) ; `5 FAIRE 8 JUSQUA 3` est rejeté (C 32). Sous T1600 version 4 et MITRA 15, `5 FAIRE 8 TANT QUE A<4` est rejeté (C 32).
- Compilation de `5 FAIRE 8 POUR A[1]←0 JUSQUA 3` : vérifier le rejet avec `ERR_SYNTAXE`, la variable contrôlée devant être une variable simple (LSE15 §5-5.1).
- Compilation de `3 SI A=0 ALORS DEBUT FAIRE 5 POUR I←0 JUSQUA 9; J←I+1 FIN SINON ALLER EN 206` (LSE15 §5-5.2, déjà cité RCI-M §14) : vérifier que la position cible du branchement `0x00` écrite par correction différée (IMP-0166) est exactement celle du premier élément de la sous-séquence `ALLER EN 206`, et que celle du branchement `0x01` est la longueur de la séquence (fin du bloc, RCI-M IMP-0105) — comparaison octet à octet avec la séquence déjà spécifiée par RCI-M §14.
- Compilation de `SI A=B ALORS SI XX=0 ALORS i1 SINON i2` (LSE15 §5-2) : vérifier que les deux paires d'offsets corrigés (externe et interne, IMP-0166) sont indépendantes et que l'offset de la paire externe saute par-dessus l'intégralité de la paire interne, conformément au test de non-régression déjà posé par RCI-M §14.
- Compilation de `5 AFFICHER[F2.1] A` : vérifier l'acceptation ; `5 AFFICHER[F100.1] A` et `5 AFFICHER[E2.100] A` sont rejetés avec `ERR_SYNTAXE` (`e` et `d` de 0 à 99, RCI-M 0x35).
- Compilation de `5 X←2↑3↑2` : vérifier la séquence `2`, `3`, `0x05`, `2`, `0x05`, puis l'affectation — associativité à gauche (LSE15 §3-3.4), valeur 64 à l'exécution.
- Compilation de `5 X←-A↑2` : vérifier la séquence `A`, `2`, `0x05`, `0x02` (changement de signe appliqué au résultat de l'exponentiation, rang 4).
- Compilation de `5 X←A*-B` : vérifier le rejet avec `ERR_SYNTAXE` (LSE15 §3-3.3) ; par contraste, `5 X←A*(-B)` est accepté.
- Compilation de `5 X←-A+3/DELTA*ALPHA-X↑ABS(D)-1` (LSE15 §3-3.4) : vérifier que la séquence postfixée correspond exactement au parenthésage documenté `((-A)+((3/DELTA)*ALPHA))-(X↑(ABS(D)))-1`.
- Compilation de `5 X←-1` : vérifier une seule valeur immédiate `0x22` ; `5 X←-200` : vérifier la valeur `0x01` suivie de `38 FF` (−200, octet faible en tête) ; `5 X←-2↑2` : vérifier `2`, `2`, `0x05`, `0x02` (pas de repli) ; `5 X←A*(-1)` : vérifier `A`, `0x22`, `0x03`, et une restitution `5 X←A*(-1)` (§5, « Repli sur une constante » ; RCI-M §4.12).
- Compilation de `5 X←+A` : vérifier l'acceptation et une séquence identique à celle de `5 X←A`.
- Compilation de `5 AFFICHER[3F2.0,E3.6] A,B,C,D,E,A+B+C+D+E,'FIN'` (LSE15 §6-2.4, exemple 4) : vérifier la séquence `0x37`, `0x35` (F, répétition 3), `A`, `0x36`, `B`, `0x36`, `C`, `0x36`, `0x35` (E), `D`, `0x36`, puis `E`, `A+B+C+D+E` et `'FIN'` chacune suivie de `0x36` (RCI-M IMP-0115).
- Compilation de `5 AFFICHER[*X,F0.3] NBSP,RESUL` (LSE15 §6-2.6) : vérifier la séquence `0x37`, `NBSP`, `0x32` en mode variable, `0x35`, `RESUL`, `0x36`, et que le contrôle d'IMP-0178 compte une seule valeur à afficher pour une description.
- Compilation de `5 AFFICHER[2F4.1] A` : vérifier le rejet avec `ERR_FORMAT_DESCRIPTIONS` (deux descriptions pour une valeur, IMP-0178) ; par contraste, `5 AFFICHER[F4.1] A,B` est accepté (moins de descriptions que de valeurs). Vérifier aussi le rejet de `5 AFFICHER[*X,F2.0] N` : la seule expression de la liste sert de facteur variable, il ne reste aucune valeur pour la description.
- Compilation de `5 AFFICHER[E4.6] A` puis de `3 TABLEAU A[3,2]` : vérifier que la ligne 5 compile une référence simple à `A` suivie de `0x36`, identique octet pour octet à la même ligne compilée après la ligne 3 (EXI-0001, RCI-M IMP-0115).
- Compilation de `5 AFFICHER[F2.0] A;AFFICHER[F3.0] B` : vérifier deux marques de tête `0x37`, une par instruction.
- Compilation de `5 SI A=0 ALORS B←1 SINON B←2` : vérifier que l'octet de charge utile du `0x00` vaut la position, depuis le début du bloc, du premier élément de la branche `SINON`, et que celui du `0x01` vaut la longueur de la séquence (branche se terminant en fin de ligne), conformément à RCI-M IMP-0105 (IMP-0166).
- Test du générateur par appel direct des fonctions `rcim_ecrire_*` sur un tampon presque plein (aucune ligne de 80 caractères n'étant censée atteindre la borne, RCI-M IMP-0125) : vérifier qu'un élément qui ne tient pas dans la place restante n'est pas écrit et que la compilation échoue avec `ERR_LIGNE_TROP_COMPLEXE`, sans écriture au-delà du tampon ni `HandleObjet` créé (IMP-0165).
- Compilation de `5 AFFICHER 'MESSAGE TRES LONG POUR LE PARTAGE'` puis de `8 AFFICHER 'MESSAGE TRES LONG POUR LE PARTAGE'` : vérifier que les deux lignes compilent un élément 0x03 en mode index portant le même index, et qu'une seule entrée a été créée dans la table des constantes partagées (IMP-0181). Vérifier que `5 AFFICHER 'COURT'` compile un élément 0x03 en ligne.
- Compilation de la même chaîne longue en mode machine de bureau (`?'MESSAGE TRES LONG POUR LE PARTAGE'`) : vérifier un élément 0x03 en ligne, sans recherche ni création dans la table (IMP-0181).
- Compilation de `5 AFFICHER 'L''APOSTROPHE EST DOUBLEE DANS CE TEXTE'` : vérifier que le texte stocké contient une seule apostrophe et que sa longueur est calculée sur le texte, non sur l'écriture source.
- Compilation de `5 LIRE A,B[2]` : vérifier la séquence `0x11`, référence de `A`, `0x3B`, valeur `2`, référence indexée de `B`, `0x3B` ; puis `5 LIRE A;LIRE B` : vérifier deux marques `0x11` (RCI-M IMP-0183).
- Compilation de `1 * CALCUL DE L'AIRE` : vérifier que l'élément `0x0C` porte le texte ` CALCUL DE L'AIRE`, espace de tête compris (RCI-M IMP-0183).
- Règles lexicales (§5 bis, IMP-0182) : `5 SI A=0 ALORS B←1` est accepté ; `5 SIA=0 ALORS B←1` et `5 SI(A>0) ALORS B←1` sont rejetés avec `ERR_SYNTAXE` ; `5 AFFICHER[F2.0] A` et `5 AFFICHER A` sont acceptés, `5 AFFICHERA` est rejeté ; `5 X←SIN (1)` compile l'opération SIN ; `5 X←3E-2` compile la valeur 0,03 ; `5 X←A-5` compile `A`, `5`, soustraction ; `5   X ←  A  +  1` compile la même séquence que `5 X←A+1`.
- `60 ` (numéro suivi d'un espace) compile une séquence vide ; `60` sans espace est rejeté avec `ERR_SYNTAXE` ; `1*COMMENTAIRE` est accepté (IMP-0182, LSE15 §8).
- `5 x←1` est rejeté avec `ERR_SYNTAXE` (minuscule hors chaîne) ; `5 AFFICHER 'texte en minuscules'` est accepté et le texte conservé tel quel (IMP-0182).
- Compilation d'une ligne dont le texte, hors caractère de fin de ligne, dépasse 79 caractères : vérifier que `compilateur_compiler` échoue à l'étape 1 (§9) sans avoir invoqué l'analyseur-générateur ni créé aucune entrée de symbole.
- Compilation d'une ligne syntaxiquement incorrecte après qu'un ou plusieurs identificateurs valides ont déjà été rencontrés (par exemple `5 A←B+*`) : vérifier que `*resultat` reste inchangé, qu'aucun objet compilé ni bloc transitoire n'a été créé (instrumentation du gestionnaire d'objets), et que la ou les entrées de symbole déjà créées pour `A` et `B` restent en table sans effet observable (§7, dernier alinéa du §3).
- Compilation successive de deux lignes utilisant le même identificateur `I` (par exemple une variable simple, puis une référence dans une autre ligne) : vérifier que la seconde compilation retrouve l'entrée déjà créée par la première (même index 16 bits), sans création d'une seconde entrée (IMP-0167).
- Compilation de la même ligne `FAIRE` deux fois de suite (simulant une modification suivie d'une recompilation, EXI-0006) dans deux appels distincts à `compilateur_compiler` : vérifier que les deux séquences produites sont rigoureusement identiques, confirmant l'absence d'état résiduel entre deux appels (§10).
- Compilation d'une ligne fournie comme si elle provenait d'ENCODER (même texte, même appel, sans distinction d'origine) : vérifier que la séquence produite est identique, octet pour octet, à celle produite pour la même ligne fournie comme entrée interactive (IMP-0169).
- Épuisement simulé de la capacité du gestionnaire d'objets (`objets_creer` renvoyant `GESTOBJ_HANDLE_NUL`) au moment de l'étape 7 : vérifier que `compilateur_compiler` renvoie un échec avec `diagnostic->code = ERR_MEMOIRE_INSUFFISANTE`, sans laisser `*resultat` modifié.
- Compilation de `10 PROCEDURE &PGCD(A,B)` : vérifier que la séquence produite se réduit à l'unique élément `0xCD`, suivi de l'identifiant de `&PGCD`, de `n = 2`, des identifiants de `A` puis `B`, et de `m = 0` (IMP-0174) — 9 octets au total. Vérifier aussi que l'entrée `SYM_PROCEDURE` de `&PGCD`, si elle vient d'être créée, porte `InfoProcedure = {0, 0}` à la sortie de `compilateur_compiler`, le compilateur ne la renseignant jamais (IMP-0176).
- Compilation de `20 PROCEDURE &PRO(A,B) LOCAL A,M,N;TABLEAU M[3,A]` (LSE15 §7-9) : vérifier l'élément `0xCD` avec `n = 2` (`A`, `B`) et `m = 3` (`A`, `M`, `N`, dans cet ordre), suivi de la séquence ordinaire de `TABLEAU M[3,A]` (RCI-M 0x29). Vérifier que le décodage de l'élément (IMP-0147) donne `A` en `PARAM_LOCAL`, `B` en `PARAM_REFERENCE`, et `M`, `N` comme locaux non-paramètres.
- Compilation de `1 PROCEDURE &FORM();RESULTAT RAC(SIN(X)↑2+COS(Y)↑2)` (LSE15 §7-1) : vérifier l'élément `0xCD` avec `n = 0` et `m = 0`, suivi de la séquence de l'expression puis de l'élément `0x0B` (RESULTAT).
- Compilation de `56 PROCEDURE &TRAC(&F,MIN,MAX,PA) LOCAL MIN,MAX,PA` (forme réduite de LSE15 §7-8.2, l. 2153, où le dernier paramètre s'écrit `PAS` ; `PAS` étant un mot réservé selon INRDP75 Annexe VI, qui prévaut sur LSE15 (RCI-M §4.11.1), la forme `…,PAS)` est rejetée avec C 101 — désaccord entre les deux manuels, signalé à Éric le 2026-10-23) : vérifier que `&F` reçoit une entrée de nature `SYM_PROCEDURE` distincte de toute variable `F` éventuellement présente (IMP-0167, clé avec nature de nom de procédure), et que son décodage donne `PARAM_REFERENCE`.
- Compilation de `1 PROCEDURE &UNIT(A,N) LOCAL N,I,J` (LSE15 §7-8.2) : vérifier que la liste `LOCAL` est conservée dans l'ordre `N, I, J`, sans réordonnancement entre le paramètre local `N` et les variables locales `I`, `J` (§13.1).
- Rejets de syntaxe, chacun avec `ERR_SYNTAXE`, sans aucun élément écrit ni `HandleObjet` créé : `5 A←1;PROCEDURE &P()` (déclaration hors tête de ligne, [H]) ; `5 PROCEDURE &P(A[1])` (paramètre indicé, LSE15 §7-3) ; `5 PROCEDURE &P(A) LOCAL &F` (nom de procédure dans la liste `LOCAL`, [I]) ; `5 PROCEDURE &P(A) X` (texte inattendu après la déclaration, ni `;` ni fin de ligne) ; `PROCEDURE &P(A)` sans numéro (ligne machine de bureau, §8).
- Rejets de doublons, avec `ERR_DECLARATION_PROCEDURE` : `5 PROCEDURE &P(A,A)` et `5 PROCEDURE &P(A) LOCAL I,I` (IMP-0175). Par contraste, vérifier que `5 PROCEDURE &P(A) LOCAL A` est accepté, `A` figurant légitimement dans les deux listes. Rejets de `5 PROCEDURE &P(&F,&F)` (doublon de noms de procédure, C 48) et de `5 PROCEDURE &P(&P)` (paramètre homonyme de la procédure, C 48) ; `5 PROCEDURE &P(A,A)` et `5 PROCEDURE &P(A) LOCAL I,I` relèvent de C 47 (IMP-0175, IMP-0185).
- Compilation de `30 X←&P(1)` avant toute déclaration de `&P`, puis de `40 PROCEDURE &P(Y) LOCAL Y` : vérifier que les deux lignes désignent la même entrée `SYM_PROCEDURE` (même identifiant 16 bits) et que la séquence de la ligne 30 est identique, octet pour octet, à celle obtenue en compilant la ligne 30 après la ligne 40 (EXI-0001, §13.5).
- Compilation de `5 &AXE(10)` (LSE15 §7-6.2), puis de `6 Y←&RAC(3,20)` (LSE15 §7-5) et de `7 Z←&FORM()+1` (LSE15 §7-1) : vérifier que chaque élément `0x28` porte respectivement l'indicateur `0x00` et `k = 1`, l'indicateur `0x01` et `k = 2`, l'indicateur `0x01` et `k = 0` (RCI-M IMP-0121, IMP-0177), et que la référence à la procédure précède, dans la séquence, les sous-séquences des arguments.
- Compilation de `8 A←&FACT(7,2)` alors que `&FACT` est déclarée avec un seul paramètre : vérifier que la compilation réussit, avec `k = 2` dans l'élément `0x28`, et qu'elle ne consulte ni `InfoProcedure` ni la table des paramètres formels (§13.5, EXI-0001) — l'erreur relève de l'exécution (Etat d'exécution §5.5, étape 1).
- Contrat d'intégration (IMP-0176), à exécuter sur `programme.c` dès sa rédaction : insertion de `10 PROCEDURE &P(A)` puis de `50 PROCEDURE &P(A,B)` ⇒ `InfoProcedure(&P) = {50, 2}` (LSE15 §7-6, Note) ; suppression de la ligne 50 ⇒ `{10, 1}` ; suppression de la ligne 10 ⇒ `{0, 0}` ; remplacement de la ligne 10 par `10 AFFICHER 1` alors qu'elle portait la seule déclaration ⇒ `{0, 0}` et retrait de son entrée de la table des paramètres formels.
- Compilation de `?38-24+47` (INRDP75, exemple du mode machine de bureau) : vérifier que `compilateur_compiler` reconnaît l'absence de chiffre en tête (IMP-0171), produit `resultat->type = LSE_LIGNE_BUREAU`, et que la séquence générée dans le bloc transitoire est strictement identique, octet pour octet, à celle que produirait `AFFICHER 38-24+47` compilée comme ligne de programme (IMP-0173) — à l'exception, bien sûr, de l'absence de `numero_ligne`.
- Compilation de `VA←60` sans numéro de ligne (LSE15 §9-5) : vérifier que `resultat->type = LSE_LIGNE_BUREAU`, que l'entrée de symbole pour `VA` est créée ou retrouvée exactement comme pour une affectation en ligne de programme (§7), et qu'aucune entrée n'est ajoutée à la table des objets compilés.
- Compilation de `ALLER EN 10` sans numéro de ligne : vérifier que la ligne est reconnue comme machine de bureau (IMP-0171), que l'instruction `ALLER EN` est rejetée dès sa reconnaissance avec `diagnostic->code = ERR_INSTRUCTION_INTERDITE_BUREAU` (IMP-0172), et qu'aucun élément n'a été écrit dans le tampon de génération ni aucun `HandleObjet` créé.
- Compilation de `SI X>0 ALORS ALLER EN 5` sans numéro de ligne : vérifier que le rejet d'`ALLER EN` intervient malgré son imbrication sous `SI...ALORS`, confirmant le caractère récursif de la restriction (IMP-0172) — à comparer avec la compilation réussie de `SI X>0 ALORS X←X-1` sans numéro de ligne, qui ne contient aucune instruction interdite.
- Compilation de `TABLEAU M[3]` sans numéro de ligne sous le profil MITRA 15 : vérifier le rejet avec `diagnostic->code = ERR_INSTRUCTION_INTERDITE_BUREAU` (IMP-0172, §8 bis) ; vérifier, par contraste, que le profil T1600 accepte cette même ligne (§8 bis, IMP-0180).
- Sous le profil MITRA 15 : `5 X←SH(1)`, `5 UN T`, `5 ALLER EN @+3`, `251 AFFICHER 1` et `5 AFFICHER[63'*']` sont rejetés (constructions propres à l'autre profil ou bornes du profil, IMP-0191) ; `5 X←TEM`, sans parenthèses, est accepté et désigne la variable `TEM`, ce nom n'étant pas réservé sous ce profil (IMP-0180, IMP-0182 ; choix d'Éric du 2026-10-23) ; `5 SUPPRIMER 'F'`, `5 I←@50`, `5 X←TEM()` et `250 AFFICHER 1` sont acceptés (IMP-0180).
- Sous le profil T1600 : `5 SUPPRIMER 'F'` et `5 I←@50` sont rejetés avec `ERR_HORS_PROFIL` ; `5 X←TEM()` est rejeté sous le T1600 version 4 ; sous `LSE F4`, il est accepté, compile 0x0F comme `5 X←TEM`, et l'un et l'autre se restituent `5 X←TEM()` (IMP-0189) ; de même `5 C←DAT` et `5 C←DAT()`, qui se restituent `5 C←DAT()` sous `LSE F4` ; `5 C←DAT()` est rejeté sous le T1600 version 4 ; `5 X←SH(1)` compile la valeur 1 puis 0x3C ; `5 UN T` compile une référence à `T` puis 0x1A ; `10 ALLER EN @+3` compile la référence 0x02, la valeur 3, l'addition, puis 0x03 ; `5 X←TEM` compile 0x0F ; `255 AFFICHER 1` est accepté ; `5 AFFICHER[62'*']` est accepté et `5 AFFICHER[63'*']` rejeté (IMP-0180).
- Mots réservés (IMP-0180) : `5 TANT←1` est rejeté sous les deux profils ; `5 SIN←3` et `5 SIN[2]←1` sont acceptés (nom de fonction non réservé selon INRDP75 Annexe VI) et `5 X←SIN(SIN)` compile la référence à l'identificateur `SIN` puis l'opération SIN ; `5 DAT←1` est accepté sous le profil MITRA 15 et rejeté sous le profil T1600 ; `5 REPRENDRE←1` est rejeté sous le profil MITRA 15 (mot réservé de ce profil, C 102) et sous le profil T1600 (identificateur de plus de cinq caractères, C 03, IMP-0191).
- Compilation de `&FACT(5)` sans numéro de ligne, employé comme instruction seule : vérifier le rejet (IMP-0172, appel de procédure comme instruction interdit) ; puis compilation de `X←&FACT(5)` sans numéro de ligne : vérifier également le rejet (IMP-0172, appel de fonction comme opérande d'expression interdit, conséquence de l'interdiction de `RESULTAT`), avec un diagnostic positionné sur l'occurrence de `&FACT` et non sur `X←`.
- Équivalents numériques (IMP-0190), profil `LSE F4` : `5 AFFICHER .27 12.` compile une constante-chaîne de deux caractères (codes 27 et 12) et se restitue à l'identique ; `5 AFFICHER 'A'.8.'B'` compile la chaîne de trois caractères `A`, retour arrière, `B`, et se restitue `5 AFFICHER 'A'.8.'B'` ; `5 X←.65.` se restitue `5 X←'A'` ; `5 AFFICHER .27` (point final manquant) est rejeté (C 08) ; `5 AFFICHER .128.` est rejeté (borne d'`EQC`) ; `5 AFFICHER 'A' .8.` est rejeté (morceaux non accolés). Dans un format : `5 AFFICHER['A'.7.'U']` compile un libellé de trois caractères et se restitue à l'identique, comme la ligne 50 de `MULT` ; `5 AFFICHER[3.7.]` porte le facteur 3 ; `5 AFFICHER[.27 12.,/]` commence par un équivalent ; `5 AFFICHER['A' .7.]` est rejeté (C 40). Sous T1600 version 4 et sous MITRA 15, `5 AFFICHER .27.` et `5 AFFICHER[.7.]` sont rejetés (C 103).
- Contrôle de type (IMP-0188) : `5 X←'A'+1` et `5 X←SIN('A')` sont rejetés (nature « chaîne au lieu de nombre », C 27) ; `5 C←3!'A'` et `5 X←LGR(3)` sont rejetés (nature « nombre au lieu de chaîne », C 28) ; `5 X←T['X']` est rejeté (nature « indice non numérique », C 15) ; `5 TABLEAU T['A']` est rejeté (nature « dimension non numérique », C 14) ; `5 SI 'A'=1 ALORS X←1` est rejeté (nature « comparaison de types différents », C 11) ; `5 ALLER EN 'A'` est rejeté (C 27). Sont acceptés : `5 X←A+B`, `5 X←'A'`, `5 X←A!B`, `5 X←T[A]`, `5 SI A=B ALORS X←1`, `5 X←SI A=0 ALORS 1 SINON 'A'` (types indéterminés ou branches de types différents), `5 X←&F('A')+1`.
- Compilation de `5 ?38-24+47` (tentative d'emploi de `?` sur une ligne numérotée) : vérifier le rejet avec la nature « `?` réservé au mode machine de bureau » (IMP-0173, IMP-0191).
- Natures complémentaires (IMP-0191) : `5 ALPHAB←1` (C 03, position 2) ; `5 X←&PROCED(1)` (C 05) ; `5 X←1E99` (C 09) ; `0 A←1` et `32001 A←1` (C 52) ; `251 A←1` sous MITRA 15 (nature « au-delà de la borne du profil ») ; `5 X←A+B)` (C 45) ; `5 X←A B` (C 19) ; `5 X←A %` (C 01, caractère étranger au langage) ; `5 X←A]` (C 53) ; échec simulé d'agrandissement de la table des symboles (C 57, position 0). Vérifier dans chaque cas la position : premier caractère non accepté, ou premier caractère de la construction déjà lue.
