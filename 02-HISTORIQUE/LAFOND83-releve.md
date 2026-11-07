# LAFOND83 — relevé de l'extrait versé (p. 17 à 19, 101 à 110, 129 à 134)

**Source :** `02-HISTORIQUE/LAFOND83.pdf` (numérisation, 20 pages : couverture, pages 17 à 19 du chapitre 2, pages 101 à 110 du chapitre 9 « Les fichiers-données » versées par Éric le 2026-10-25, pages 129 à 134).
**Identification (couverture) :** *lire LSE — Le langage français de programmation — manuel de l'utilisateur*, Christian Lafond, Pierre Muller, préface de Jacques Hebenstreit, CEDIC / Fernand Nathan. L'année 1983 n'est pas visible sur la couverture numérisée.
**Statut :** source complémentaire de recoupement. L'implantation n'est pas nommée dans l'extrait. Indices d'un MITRA 15, relevés par Éric : `TERMINE` sans numéro de ligne (forme MITRA 15 selon INRDP75, l. 1972) et la commande `UTILISATION DISQUE A PARTIR DE`. La présence de structures absentes de LSE15 suggère une version plus récente. L'attribution au MITRA 15 reste une conjecture.

---

## 1. Contenu de l'extrait

- **p. 19, « Les erreurs »** (chapitre « Première rencontre avec L.S.E. »). Deux sortes de messages :
  - « Lorsque vous tapez une commande ou une ligne LSE ou une instruction LSE en mode "machine de bureau", le système fait une analyse syntaxique des caractères que vous avez tapés. […] Dans le cas contraire il signale ce qu'on appelle "une erreur de compilation" ; on retapera alors la commande ou les instructions LSE après avoir pris connaissance du message d'erreur. »
  - « En exécution de programme, il peut se produire une erreur d'exécution due à une erreur de logique dans la rédaction du programme. Il existe une liste d'erreurs de ce type. Dans l'exemple ci-dessous, le numéro d'erreur indiqué permet à l'utilisateur de savoir qu'il a oublié l'instruction de fin d'exécution (TERMINER ou PAUSE). »
  - Note 5 : « Ces messages se présentent, sur les systèmes existants, sous forme de numéros d'erreur. Ces numéros sont propres à chaque type de matériel. Il faudra donc consulter la liste fournie par le constructeur. »
  - Annulation : « Tant que le caractère de fin de message n'a pas été tapé, il est possible d'annuler les caractères précédents en utilisant autant de fois que cela est nécessaire la touche d'annulation », marquée d'une flèche vers la gauche.
- **p. 129, « Exemple de programme ».** Listing « VERSIØN ERRØNEE DE CFR2 » (lignes 1 à 215), où les auteurs ont « supprimé ou modifié quelques instructions ». Aucun message d'erreur sur la page.
- **p. 130, « Le mode machine de bureau ».** Session de mise au point (encadré ci-dessous) et commentaire : « Nous tentons une première exécution et nous butons très vite sur une erreur en ligne 30. Le code d'erreur nous renvoie à la liste fournie par le constructeur où nous lisons : "rangement d'une chaîne dans une variable non déclarée chaîne". »
- **p. 130-132, « Exécutions partielles ».** Commandes `EXECUTER A PARTIR DE 1 A 45`, `PØURSUIVRE JUSQU'EN 60`, `LISTER LIGNES 45`, `PAS A PAS`, `NORMAL`, `CONTINUER`.
- **p. 133, « L'éditeur de ligne ».** « Lorsqu'une ligne d'instructions a été refusée, il est possible, sur certains systèmes, de corriger uniquement la partie erronée au lieu de retaper toute la ligne. » La correction s'écrit `/groupe à corriger/groupe modifié`, `%` remplaçant `/` si le groupe contient `/`, et `Xn` désigne la n-ième occurrence.
- **p. 133, suite.** « La correction s'effectue sur la première occurrence du groupe indiqué. Pour corriger une autre occurrence que la première, on fera précéder les informations ci-dessus de Xn, où n indique le rang de l'occurrence à remplacer. » La forme complète : `/` ou `%`, le groupe à corriger, le même signe, le groupe modifié, le caractère de fin de message ; « On utilise % si le groupe à corriger contient lui-même le signe / et inversement. »
- **p. 134.** « Pour corriger plusieurs occurrences, on utilisera nX où n indique le nombre d'occurrences à remplacer. » Exemples :

```
10. AFICHER A
ERREUR C 03
/F/FF
10 AFFICHER A

10 X←SCH(A,1,3)!SCH(B,1)
ERREUR C 29
X2/)/,3)
10 X←SCH(A,1,3)!SCH(B,1,3)

10 X←SCH(A,1)!SCH(B,1)
ERREUR C 29
2X/)/,3)
10 X←SCH(A,1,3)!SCH(B,1,3)
```

  (Le premier exemple imprime `10.` ; la ligne corrigée est affichée sans le point. Correction du 2026-10-27 : ce relevé donnait la page 134 pour blanche.)

## 2. Messages attestés

Page 19 :

```
ERREURS DE CØMPILATIØN
LIS
ERREUR  C 38

AFICHER 'IL FAIT BEAU.'
ERREUR  C 03

ERREUR D'EXECUTIØN
1 AFFICHER 'IL FAIT BEAU.'
EXECUTER A PARTIR DE 1
IL FAIT BEAU.
ERREUR E160 LIGNE 1
```

| Message | Classe | Contexte | Libellé donné par le texte |
|---|---|---|---|
| `ERREUR  C 38` | C (compilation) | commande mal frappée `LIS` | non donné ; MEMENTO82 C 38 « affectation "_" absent ou variable réceptrice incorrecte » |
| `ERREUR  C 03` | C | `AFICHER` (7 lettres) | non donné ; MEMENTO82 C 03 « Identification de plus de 5 caractères » |
| `ERREUR E160 LIGNE 1` | E | programme sans `TERMINER` ni `PAUSE` | « oublié l'instruction de fin d'exécution » ; MEMENTO82 : 58 |

Encadré « EXEMPLES DE MISE AU PØINT AVEC LE MØDE MACHINE DE BUREAU » (p. 130) :

```
EXECUTER A PARTIR DE 1
TAPEZ UN VERBE REGULIER DU PREMIER GRØUPE A L'INFINITIF : CHANTER
ERREUR E210 LIGNE 30
?LØNG=7
?INF='CHANTER'
?RADIC=
ERREUR M200
5 CHAINE DER,INF,RADIC,SUFFI,DESI,PRES,IMPAR,FUTUR,REP
```

| Message | Classe | Contexte | Libellé donné par le texte |
|---|---|---|---|
| `ERREUR E210 LIGNE 30` | E (exécution, mode programmé) | exécution du programme, ligne 30 | « rangement d'une chaîne dans une variable non déclarée chaîne » |
| `ERREUR M200` | M (exécution, machine de bureau) | `?RADIC=` sur une variable restée sans valeur | non donné |

Autres messages : `LIGNE 130 PRET`, `ARRET LIGNE 45`, `ARRET LIGNE 60`, `TERMINE` (sans numéro de ligne).

## 3. Observations

- Les codes de compilation (`C 38`, `C 03`) ont deux chiffres et le même sens que dans MEMENTO82 et sur T1600 ; seuls les codes d'exécution suivent une numérotation à trois chiffres.
- Les classes E et M confirment la classification d'INRDP75 (l. 668-675) : `C.n` pour la compilation, `Mn` en machine de bureau, `En` ou `EX n` en mode programmé.
- La numérotation d'exécution est à trois chiffres (160, 210, 200), alors que les sorties T1600 et MEMENTO82 numérotent sur deux chiffres. Les numéros d'erreur dépendent donc, au moins en partie, de l'implantation.
- Le message d'exécution en mode programmé rappelle le numéro de ligne (`LIGNE 30`), contrairement aux erreurs de compilation T1600, qui suivent l'écho de la ligne.
- L'éditeur de ligne est présenté comme propre à « certains systèmes ». Il ne fait partie d'aucun des deux profils retenus d'après le corpus versé.

---

## 4. Pages versées le 2026-10-25 : chapitre 2 (p. 17-18) et chapitre 9 (p. 101-110)

Transcription des passages qui touchent aux commandes de fichiers ; `Ø` est rendu tel qu'imprimé dans les encadrés. Les soulignés de l'original (frappe de l'utilisateur) ne sont pas reproduits.

### 4.1 Chapitre 2, « Conservation des programmes » (p. 17-18)

- « Il faut d'abord donner votre numéro d'identification, c'est-à-dire une sorte de clé qui vous permet d'avoir accès à la bibliothèque. Ceci se fait au moyen de la commande **IDENTIFICATION N**. N est un nombre que vous tapez sur le clavier mais qui n'apparaît pas sur le papier ou l'écran de la console. Sur certains systèmes, il n'est pas nécessaire d'utiliser cette commande. On travaille alors avec un numéro d'identification pris par défaut qui est 0. »
- **RANGER XXXXX** ; **UTILISATION DISQUE A PARTIR DE XXXXX{,P}**, note 4 : « Le paramètre P, qui peut être omis, indique que la consultation doit avoir lieu dans le catalogue des fichiers-programmes, par opposition à D pour les fichiers-données (cf. Chapitre 9). »

```
UTILISATIØN DISQUE A PARTIR DE CFR,P
12/12/79 14:48:35

FICHIERS-PRØGRAMME
******************

 NØM NØ.CØMPTE  DATE  NB.MØTS

CFR      11    12/12/79  403
CFR2     11    05/12/79  814
...
```

- p. 18 : « On connaîtra le contenu complet de cette bibliothèque en tapant A ou * suivant les sytèmes [sic]. » Exemple `UTILISATIØN DISQUE A PARTIR DE A`, date sur la ligne suivante, puis la section `FICHIERS-PRØGRAMME` de `ANARE` à `CHI` par ordre alphabétique, comptes 01, 10, 11, 97.
- **REMPLACER XXXXX** (« la nouvelle version du programme remplacera l'ancienne », exemple `IDENTIFICATIØN`, `RANGER CFR`, `100* FIN DU TRAITEMENT`, `REMPLACER CFR`) ; **SUPPRIMER XXXXX,P** ; `AU REVØIR 05/12/79 11:15:08`.

### 4.2 Chapitre 9, « Les fichiers-données » (p. 101-110)

- p. 101 : « Un numéro d'enregistrement est un nombre compris entre 1 et 32000, bornes incluses. »
- p. 102 : noms de 1 à 5 caractères alphanumériques, le premier une lettre ; fichiers temporaires supprimés à AU REVOIR ; `#` pour les permanents. Note 1 : « il peut être nécessaire de faire suivre ces noms d'un point et d'un numéro d'unité (.1 pour l'unité de gauche, .2 pour l'unité de droite) ». Note 2 : temporaires propres à un terminal, permanents accessibles à tous.
- p. 102, « Accès aux fichiers » : « Les fichiers sont généralement accessibles en lecture sans aucune protection. Par contre, ils ne peuvent être créés si l'on n'a pas donné un numéro d'identification. Pour les supprimer ou les modifier, il faut en outre que ce numéro soit le même que celui sous lequel ils ont été créés. […] On sera alors invité éventuellement à indiquer le nombre de secteurs d'espace temporaire désiré. Cet espace n'est utile que dans le cas où l'on crée des fichiers temporaires, car il est réservé automatiquement pour les fichiers permanents. » **CATALOGUER NOFIT,NOFIP**.
- p. 102-103 : **GARER α, β, γ {,δ}** ; δ reçoit un compte rendu : `0` réussite, `−3` identification courante différente de celle de création, `−4` nom de fichier syntaxiquement incorrect, `−5` espace insuffisant, `−10 − n` fichier réservé par le poste n (temps partagé), `−100 − n` erreur physique. « Dans la même situation, l'absence du paramètre δ provoquerait une erreur d'exécution. »
- p. 105, dialogue d'identification avec espace temporaire :

```
IDENTIFICATIØN
NØMBRE DE SECTEURS DISPØNIBLES:294
ESPACE TEMPØRAIRE DESIRE:0
```

- p. 106 : **CHARGER α, β, γ {,δ}** ; δ : le type de l'enregistrement (comme la fonction TYP), `−1` enregistrement inexistant, `−2` fichier inexistant. p. 107 : sans δ, `ERREUR E200 LIGNE 2` sur un enregistrement inexistant.
- p. 108 : **SUPPRIMER γ, β {,δ}** ; « Lorsque le paramètre β est un astérisque, cela signifie qu'on veut supprimer tout le fichier » (`SUPPRIMER 'TEXTE',*`).
- p. 109 : commande **SUPPRIMER XXXXX,D** (fichier entier ; « le nom du fichier doit être suivi de ,D pour indiquer qu'il s'agit d'un fichier-données et non d'un fichier programme ») ; note 5 : « On supprimera les fichiers temporaires en tapant SUPPRIMER XXXXX,T pour un fichier particulier ou SUPPRIMER * pour tous les fichiers d'une console. »
- p. 109, « Copie de fichiers » : « On peut avoir besoin de recopier un fichier d'une disquette sur une autre. On utilise pour cela soit un programme spécifique qui enchaîne les instructions CHARGER et GARER, soit, lorsqu'elle existe, la commande **DUPLIQUER XXXXX.N**. On peut recopier un seul fichier, tous les fichiers commençant par un groupe de caractères donnés (dans ce cas ce groupe est suivi d'un astérisque). N donne le numéro de la disquette de départ : si N = 1 on les recopie sur l'unité 2 et inversement. La mention de ,P ou ,D limite l'opération aux fichiers-programmes ou aux fichiers-données. Exemple : DUPLIQUER *.1,P recopie tous les programmes de la disquette 1 sur la disquette 2. »
- p. 109-110, « Consultation du catalogue » : « UTILISATION DISQUE, déjà vue dans le chapitre II, permet de connaître tous les fichiers du disque. TABLE DES FICHIERS fournit seulement la liste des fichiers correspondant à un numéro d'identification donné, préalablement tapé avec la commande IDENTIFICATION. Dans un cas comme dans l'autre on peut avoir soit la liste complète soit une liste partielle. Pour avoir la liste complète on tape A ou * suivant l'ordinateur dont on dispose. […] Pour avoir une liste partielle, on tape le nom du fichier programme ou du fichier-donnée qui servira de point de départ, suivi de la lettre P, D ou T suivant les cas. Si ce paramètre est omis, le système considérera qu'il s'agit d'un programme. »

```
IDENTIFICATIØN
NØMBRE DE SECTEURS DISPØNIBLES:340
ESPACE TEMPØRAIRE DESIRE: 10

TABLE DES FICHIERS A PARTIR DE A
12/12/79 16:50:55

FICHIERS-PRØGRAMME
******************

 NØM NØ.CØMPTE  DATE  NB.MØTS

ANARE   11    28/11/79  510
...

UTILISATIØN DISQUE A PARTIR DE CØPRI,D
12/12/79 16:49:11

FICHIERS-DØNNEE PERMANENTS
**************************

 NØM NØ.CØMPTE  DATE  NB.SECTEURS

CØPRI    11   12/12/79     5
CREFI    11   12/12/79     5
DFRP     99   10/10/79     3
...
```

### 4.3 Observations

- Les formes de TABLE DES FICHIERS et d'UTILISATION DISQUE (titres, colonnes, astérisques) sont celles du MITRA 15 d'INRDP75 p. 133-134, avec le paramètre `A PARTIR DE` et la date sur la ligne suivant la commande. La liste partielle `CØPRI,D` ne montre que la section des fichiers-données : retenu pour IMP-0257 (choix d'Éric du 2026-10-25, T3).
- Le dialogue `NOMBRE DE SECTEURS DISPONIBLES:` / `ESPACE TEMPORAIRE DESIRE:` donne la forme du message qu'INRDP75 (5ᵉ partie, §2-2, option c) décrit sans la citer : retenu pour IMP-0256.
- DUPLIQUER y désigne l'unité de départ par `.N` (micro-ordinateur à deux unités de disquette) ; le T1600 de Victor Duruy l'écrit `$` (témoignage d'Éric). Les deux formes partagent le groupe de caractères suivi de `*` et la restriction `,P` ou `,D` (IMP-0258).
- **Règle (Éric, 2026-10-27)** : lorsque LAFOND83 diverge d'INRDP75 ou de LSE15, ces deux manuels font la norme ; LAFOND83 ne sert qu'à recouper et à combler ce qu'ils ne disent pas (forme d'un message, paramètre d'une commande).
- Divergences, donc non appliquées : numéros d'enregistrement de 1 à 32 000 (32 767 retenus, IMP-0253) ; δ de GARER ; `SUPPRIMER γ,*` pour le fichier entier (INRDP75 : β omis) ; commande `SUPPRIMER XXXXX,D` et `,T` hors T1600 ; `REMPLACER` pour MODIFIER ; identification 0 par défaut « sur certains systèmes ».

