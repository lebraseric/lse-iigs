# RCI-M — Référence de codage

**Objet :** base concise et complète pour coder le compilateur, l'interpréteur et le décompilateur sur la représentation compilée interne en mémoire (RCI-M).
**Statut :** document **dérivé et subordonné**. Il ne contient aucune justification et ne crée aucune décision. En cas d'écart, [RCI-M_Specification_physique.md](../tome-v/RCI-M_Specification_physique.md) prévaut ; toute modification de ce chapitre doit être reportée ici dans la même opération.
**Dérivé de :** RCI-M dans son état du 2026-10-22, décisions IMP-0101 à IMP-0128, IMP-0174 (élément 0x0D, chapitre Compilateur), IMP-0177, IMP-0183, IMP-0184 ; renvois ponctuels aux chapitres Compilateur et État d'exécution.
**Périmètre :** RCI-M seul. Les structures de l'état d'exécution (pile d'évaluation, cadres, environnement des identificateurs) et la table des paramètres formels font l'objet d'autres chapitres.

---

## 1. Conventions

**Notation des tables.**
- *Octet* : valeur complète de l'étiquette (catégorie et sous-code), à écrire telle quelle.
- *Charge utile* : `u8` = 1 octet non signé, `s8` = 1 octet signé, `s16` = 2 octets signés, `u16` = 2 octets non signés, `id` = identifiant de symbole `u16` (index de la table des symboles), `pos` = position `u8` comptée depuis le premier octet du bloc, `lig` = numéro de ligne `u8`, `txt[n]` = n octets de caractères.
- *Pile* : éléments consommés, dans l'ordre où ils ont été empilés, puis `→`, puis élément produit. `ref` = référence, `v` = valeur numérique, `c` = chaîne, `b` = valeur logique, `x` = valeur de type quelconque.
- *Profil* : `commun`, `T1600` ou `M15` (MITRA 15), selon le profil d'implantation (chapitre Compilateur §8 bis, IMP-0180). Une construction d'un profil est rejetée sous l'autre (`ERR_HORS_PROFIL`).

**Physique.**
- Types ORCA/C : `char` 1 octet ; `short`, `int` 2 octets ; `long` 4 ; pointeur 4 ; `float` 4 (SANE) ; aucun bourrage de structure ; un `enum` occupe 2 octets, d'où l'emploi de `unsigned char` et de `#define` (IMP-0127).
- Un paramètre `float` est conservé au format SANE *extended* (10 octets) : copier la valeur dans une variable locale `float` avant d'en prendre l'adresse (RCI-M §2).
- Ordre des octets : **octet de poids faible en tête** pour tout champ de plusieurs octets (`s16`, `u16`, `id`, `float`), comme ORCA/C range ses variables en mémoire (IMP-0184). Un champ se lit donc directement comme la variable C correspondante.
- Un bloc par ligne (IMP-0101) ; longueur de séquence ≤ 255 octets ; toute position intra-séquence ≤ longueur (IMP-0105).

**Étiquette.**

```c
typedef unsigned char TagRCIM;        /* bits 7-6 : catégorie ; bits 5-0 : sous-code */
#define RCIM_CAT_VALEUR      0x00
#define RCIM_CAT_REFERENCE   0x40
#define RCIM_CAT_OPERATION   0x80
#define RCIM_CAT_CONTROLE    0xC0
#define RCIM_CAT_MASQUE      0xC0
#define RCIM_SOUSCODE_MASQUE 0x3F
```

Les mnémoniques suivent la convention `RCIM_<CAT>_<NOM>` (`VAL`, `REF`, `OP`, `CTL`) ; chaque constante vaut l'octet complet.

---

## 2. Catégorie « valeur immédiate » (0x00–0x3F)

| Octet | Mnémonique | Charge utile | Pile | Restitution | Profil | IMP |
|---|---|---|---|---|---|---|
| `0x00` | `RCIM_VAL_ENTIER_COURT` | `s8` | `→ v` | décimal | commun | 0103 |
| `0x01` | `RCIM_VAL_ENTIER` | `s16` (complément à deux) | `→ v` | décimal | commun | 0103 |
| `0x02` | `RCIM_VAL_REEL` | `float` IEEE simple précision, 4 octets, octet faible en tête ; sous T1600, dernier bit de mantisse nul (`rcim_arrondir_23`, IMP-0280) | `→ v` | 6 chiffres significatifs (§8) | commun | 0103 |
| `0x03` | `RCIM_VAL_CHAINE` | `u8` mode ; si bit 7 = 0 : longueur = bits 6-0 (0–127), puis `txt[longueur]` ; si bit 7 = 1 : bits 6-0 à 0, puis `u16` index de la table des constantes | `→ c` (non transitoire) | `'texte'`, apostrophe doublée | commun | 0128, 0107 |
| `0x04` | `RCIM_VAL_FAUX` | — | `→ b` | — (jamais produit) | commun | 0103 |
| `0x05` | `RCIM_VAL_VRAI` | — | `→ b` | — (jamais produit) | commun | 0103 |
| `0x06` | `RCIM_VAL_LIGNE` | `lig` | `→ v` | `@n` | M15 | 0119 |
| `0x07`–`0x3F` | `RCIM_VAL_IMMEDIAT_BASE` (= `0x23`) | — ; valeur = octet − `0x23`, de −28 à +28 | `→ v` | décimal | commun | 0123 |

Choix du sous-format d'une constante entière : `0x07`–`0x3F` si −28 ≤ v ≤ 28 ; sinon `0x00` si −128 ≤ v ≤ 127 ; sinon `0x01` si −32 768 ≤ v ≤ 32 767 ; sinon `0x02`. Une constante non entière : `0x02`. Un changement de signe dont l'opérande est une constante seule est replié en constante négative (`-1` → `0x22`, `-200` → `01 38 FF`) ; sinon il reste une opération `0x82` (`-2↑2` → `25 25 85 82`).

Chaîne : en ligne si la ligne est une ligne machine de bureau ou si la longueur ≤ 16 ; sinon toujours par la table des constantes (Compilateur IMP-0181).

---

## 3. Catégorie « référence » (0x40–0x7F)

| Octet | Mnémonique | Charge utile | Pile | Restitution | Profil | IMP |
|---|---|---|---|---|---|---|
| `0x40` | `RCIM_REF_SIMPLE` | `id` | `→ ref` | nom (`&` si nature `SYM_PROCEDURE`) | commun | 0104, 0120 |
| `0x41` | `RCIM_REF_INDEXEE` | `id`, `u8 k` (1 ou 2) | `v1 [v2] → ref` | `NOM[i1{,i2}]` | commun | 0120 |
| `0x42` | `RCIM_REF_LIGNE_COURANTE` | — | `→ v` | `@` | T1600 | 0119 |
| `0x43` | `RCIM_REF_LIGNE_RETOUR` | — | `→ v` | `$` | T1600 | 0119 |
| `0x44`–`0x7F` | réservés | | | | | |

Une référence est toujours résolue entièrement à sa rencontre (alias de paramètre compris) et empilée comme `REFERENCE` ; une opération qui attend une valeur la déréférence (État d'exécution IMP-0144). Les indices précèdent la référence indexée.

---

## 4. Catégorie « opération codée » (0x80–0xBF)

Sans charge utile sauf mention. Pile : opérandes dans l'ordre d'écriture.

| Octet | Mnémonique | Charge utile | Pile | Restitution | Profil | IMP |
|---|---|---|---|---|---|---|
| `0x80` | `RCIM_OP_ADD` | | `v v → v` | `+` | commun | 0117 |
| `0x81` | `RCIM_OP_SUB` | | `v v → v` | `-` | commun | 0117 |
| `0x82` | `RCIM_OP_NEG` | | `v → v` | `-` en tête | commun | 0117 |
| `0x83` | `RCIM_OP_MUL` | | `v v → v` | `*` | commun | 0117 |
| `0x84` | `RCIM_OP_DIV` | | `v v → v` | `/` | commun | 0117 |
| `0x85` | `RCIM_OP_PUIS` | | `v v → v` | `↑` (ASCII 94) | commun | 0117 |
| `0x86` | `RCIM_OP_RAC` | | `v → v` | `RAC(…)` | commun | 0117 |
| `0x87` | `RCIM_OP_ABS` | | `v → v` | `ABS(…)` | commun | 0117 |
| `0x88` | `RCIM_OP_EXP` | | `v → v` | `EXP(…)` | commun | 0117 |
| `0x89` | `RCIM_OP_LGN` | | `v → v` | `LGN(…)` | commun | 0117 |
| `0x8A` | `RCIM_OP_SIN` | | `v → v` | `SIN(…)` | commun | 0117 |
| `0x8B` | `RCIM_OP_COS` | | `v → v` | `COS(…)` | commun | 0117 |
| `0x8C` | `RCIM_OP_ATG` | | `v → v` | `ATG(…)` | commun | 0117 |
| `0x8D` | `RCIM_OP_ENT` | | `v → v` | `ENT(…)` | commun | 0117 |
| `0x8E` | `RCIM_OP_ALE` | | `v → v` | `ALE(…)` | commun | 0117 |
| `0x8F` | `RCIM_OP_TEM` | | `→ v` | `TEM()` (M15, F4), `TEM` (T1600 V4) | commun | 0117 |
| `0x90` | `RCIM_OP_EGAL` | | `x x → b` | `=` | commun | 0117 |
| `0x91` | `RCIM_OP_INF` | | `x x → b` | `<` | commun | 0117 |
| `0x92` | `RCIM_OP_INF_EGAL` | | `x x → b` | `<=` | commun | 0117 |
| `0x93` | `RCIM_OP_DIFF` | | `x x → b` | `#` | commun | 0117 |
| `0x94` | `RCIM_OP_SUP` | | `x x → b` | `>` | commun | 0117 |
| `0x95` | `RCIM_OP_SUP_EGAL` | | `x x → b` | `>=` | commun | 0117 |
| `0x96` | `RCIM_OP_ET` | | `b b → b` | ` ET ` | commun | 0117 |
| `0x97` | `RCIM_OP_OU` | | `b b → b` | ` OU ` | commun | 0117 |
| `0x98` | `RCIM_OP_NON` | | `b → b` | — (non attesté, jamais produit) | commun | 0117 |
| `0x99` | `RCIM_OP_AFFECTER` | | `ref x →` | `←` (ASCII 95) | commun | 0118 |
| `0x9A` | `RCIM_OP_UN` | | `ref →` | `UN NOM` | T1600 | 0117 |
| `0x9B` | `RCIM_OP_ZER` | | `ref →` | `ZER NOM` | T1600 | 0117 |
| `0x9C` | `RCIM_OP_CONCAT` | | `c c → c` | `!` | commun | 0117 |
| `0x9D` | `RCIM_OP_LGR` | | `c → v` | `LGR(…)` | commun | 0117 |
| `0x9E` | `RCIM_OP_POS` | | `c v c → v` | `POS(…)` | commun | 0117 |
| `0x9F` | `RCIM_OP_SKP` | `u8 n` (2–3) | `c v [c] → v` | `SKP(…)` | commun | 0122 |
| `0xA0` | `RCIM_OP_PTR` | `u8 n` (2–3) | `c v [x] → v` | `PTR(…)` | commun | 0122 |
| `0xA1` | `RCIM_OP_CNB` | `u8 n` (2–3) | `c v [ref] → v` | `CNB(…)` | commun | 0122 |
| `0xA2` | `RCIM_OP_EQN` | `u8 n` (1–2) | `c [v] → v` | `EQN(…)` | commun | 0122 |
| `0xA3` | `RCIM_OP_SCH` | `u8 n` (3–4) | `c v x [ref] → c` | `SCH(…)` | commun | 0122 |
| `0xA4` | `RCIM_OP_GRL` | `u8 n` (2–3) | `c v [ref] → c` | `GRL(…)` | commun | 0122 |
| `0xA5` | `RCIM_OP_CCA` | | `v → c` | `CCA(…)` | commun | 0117 |
| `0xA6` | `RCIM_OP_EQC` | | `v → c` | `EQC(…)` ; 0–127 (T1600), 0–255 (M15) | commun | 0117 |
| `0xA7` | `RCIM_OP_DAT` | | `→ c` | `DAT()` (M15, F4), `DAT` (T1600 V4) | commun | 0117 |
| `0xA8` | `RCIM_OP_APPEL` | `u8 position` (`0x00` instruction, `0x01` expression), `u8 k` | `ref x1…xk → [x]` (`x` si position expression) | `&NOM(a1,…,ak)` | commun | 0121, 0177 |
| `0xA9` | `RCIM_OP_TABLEAU` | `id`, `u8 dim` (1–2) | `v1 [v2] →` | `TABLEAU NOM[d1{,d2}]` | commun | 0117 |
| `0xAA` | `RCIM_OP_CHAINE` | `id` | `→` | `CHAINE NOM` | commun | 0117 |
| `0xAB` | `RCIM_OP_LIBERER` | `id` | `→` | `LIBERER NOM` | commun | 0117 |
| `0xAC` | `RCIM_OP_GARER` | | `ref x x →` | `GARER NOM,e,e` | commun | 0117 |
| `0xAD` | `RCIM_OP_CHARGER` | `u8 n` (3–4) | `ref x x [ref] →` | `CHARGER NOM,e,e{,NOM}` | commun | 0122 |
| `0xAE` | `RCIM_OP_SUPPRIMER` | `u8 n` (1–2) | `x [x] →` | `SUPPRIMER e{,e}` | M15 | 0122 |
| `0xAF` | `RCIM_OP_EXECUTER` | `u8 n` (1–2) | `x [v] →` | `EXECUTER e{,e}` | commun | 0122 |
| `0xB0` | `RCIM_OP_AFF_LIBELLE` | `u8 facteur`, `u8 n`, `txt[n]` | `[v] →` (v si facteur variable) | `{r}'texte'` ou `*'texte'` | commun | 0115, 0116 |
| `0xB1` | `RCIM_OP_AFF_LIGNE` | `u8 facteur` | `[v] →` | `{r}/` | commun | 0115, 0116 |
| `0xB2` | `RCIM_OP_AFF_ESPACE` | `u8 facteur` | `[v] →` | `{r}X` | commun | 0115, 0116 |
| `0xB3` | `RCIM_OP_AFF_RETOUR` | `u8 facteur` | `[v] →` | `{r}C` | commun | 0115, 0116 |
| `0xB4` | `RCIM_OP_AFF_SAUT` | `u8 facteur` | `[v] →` | `{r}L` | commun | 0115, 0116 |
| `0xB5` | `RCIM_OP_AFF_DESCRIPTION` | forme compacte (§6) ; si F ou E : `u8 e`, `u8 d` (0–99) | `→` | `{r}Fe.d`, `{r}Ee.d`, `{r}U` | commun | 0115, 0116 |
| `0xB6` | `RCIM_OP_AFF_VALEUR` | | `x →` (tableau entier si `ref` de tableau) | valeur de la liste | commun | 0115 |
| `0xB7` | `RCIM_OP_AFF_TETE_FORMAT` | | `→` | `AFFICHER[` … | commun | 0115 |
| `0xB8` | `RCIM_OP_AFF_TETE_SANS_FORMAT` | | `→` (passage à la ligne) | `AFFICHER ` | commun | 0126 |
| `0xB9` | `RCIM_OP_LIRE_LIBELLE` | `u8 r`, `u8 n`, `txt[n]` | | réservé, non attesté | — | 0117 |
| `0xBA` | `RCIM_OP_LIRE_MISE_EN_PAGE` | `u8 car`, `u8 r` | | réservé, non attesté | — | 0117 |
| `0xBB` | `RCIM_OP_LIRE_VALEUR` | | `ref →` | cible de LIRE | commun | 0117 |
| `0xBC` | `RCIM_OP_SH` | | `v → v` | `SH(…)` | T1600 | 0119 |
| `0xBD` | `RCIM_OP_CH` | | `v → v` | `CH(…)` | T1600 | 0119 |
| `0xBE` | `RCIM_OP_TH` | | `v → v` | `TH(…)` | T1600 | 0119 |
| `0xBF` | `RCIM_OP_SGN` | | `v → v` | `SGN(…)` | T1600 | 0119 |

Catégorie entièrement occupée (64/64).

---

## 5. Catégorie « information de contrôle » (0xC0–0xFF)

| Octet | Mnémonique | Charge utile | Pile | Effet à l'exécution | Restitution | Profil | IMP |
|---|---|---|---|---|---|---|---|
| `0xC0` | `RCIM_CTL_SI_FAUX` | `pos` | `b →` | si FAUX : `position ← pos` | structure `SI` | commun | 0105 |
| `0xC1` | `RCIM_CTL_SAUT` | `pos` | | `position ← pos` | structure `SI` | commun | 0105 |
| `0xC2` | `RCIM_CTL_ALLER_EN` | `lig` | | branchement, désimbrication des boucles | `ALLER EN n` | commun | 0111 |
| `0xC3` | `RCIM_CTL_ALLER_EN_CALCULE` | | `v →` | idem, cible arrondie | `ALLER EN e` | commun | 0111 |
| `0xC4` | `RCIM_CTL_RETOUR` | | | retour au point d'appel | `RETOUR` | commun | — |
| `0xC5` | `RCIM_CTL_RETOUR_EN` | `lig` | | retour, puis branchement dans l'appelant | `RETOUR EN n` | commun | 0111 |
| `0xC6` | `RCIM_CTL_RETOUR_EN_CALCULE` | | `v →` | idem, cible arrondie | `RETOUR EN e` | commun | 0111 |
| `0xC7` | `RCIM_CTL_TERMINER` | | | fin d'exécution | `TERMINER` | commun | — |
| `0xC8` | `RCIM_CTL_PAUSE` | | | suspension | `PAUSE` | commun | — |
| `0xC9` | `RCIM_CTL_FAIRE_OUVRIR` | `id` (V) | `v v →` (n1, v1) | V ← v1 ; contexte de boucle, phase initiale | `FAIRE n1 POUR V←v1` | commun | 0112 |
| `0xCA` | `RCIM_CTL_FAIRE_PAS` | | `v →` | mémorise p ; en itération, V ← V + p | ` PAS p` | commun | 0112 |
| `0xCB` | `RCIM_CTL_RESULTAT` | | `x →` | retour avec valeur | `RESULTAT e` | commun | — |
| `0xCC` | `RCIM_CTL_COMMENTAIRE` | `u8 n`, `txt[n]` (tout ce qui suit `*`) | | aucun | `*texte` | commun | 0113 |
| `0xCD` | `RCIM_CTL_PROCEDURE` | `id`, `u8 n`, `n × id`, `u8 m`, `m × id` | | erreur si atteint (hors appel) | `PROCEDURE &NOM(…){ LOCAL …}` | commun | 0174 |
| `0xCE` | `RCIM_CTL_FAIRE_PAS_IMPLICITE` | | | p = 1 ; en itération, V ← V + 1 | (rien) | commun | 0112 |
| `0xCF` | `RCIM_CTL_FAIRE_JUSQUA` | | `v →` | sortie si (p>0 et V>v2) ou (p<0 et V<v2) | ` JUSQUA v2` | commun | 0112 |
| `0xD0` | `RCIM_CTL_FAIRE_TANT_QUE` | | `b →` | sortie si FAUX | ` TANT QUE cond` | commun | 0112 |
| `0xD1` | `RCIM_CTL_LIRE_TETE` | | | aucun | `LIRE ` | commun | 0183 |
| `0xD2` | `RCIM_CTL_FAIRE_OUVRIR_SEUL` | | `v →` (n1) | contexte de boucle sans variable, phase initiale | `FAIRE n1` | F4 | 0277 |
| `0xD3`–`0xFF` | réservés | | | | | | |

Effets d'exécution détaillés : chapitre État d'exécution (§5.5 appel et retour, §6 boucles, §7.2 cycle).

---

## 6. Champs composites

**Facteur des éléments `0xB0`–`0xB4`** (IMP-0116) : bit 7 = 1 → facteur variable (bits 6-0 à 0 ; le facteur est la valeur dépilée, arrondie) ; bit 7 = 0 → facteur fixe = bits 6-0 (1–99 en M15, 1–62 en T1600).

**Description `0xB5`** (IMP-0116) : octet 0 = type (bits 7-6 : `00` F, `01` E, `10` U, `11` réservé) | répétition − 1 (bits 5-0, si répétition ≤ 63) ; bits 5-0 = `0x3F` → octet 1 = répétition exacte (64–99) ; puis, pour F et E, `u8 e`, `u8 d`.

**Déclaration de procédure `0xCD`** (IMP-0174) : un paramètre est `LOCAL` si son `id` figure aussi dans la liste `LOCAL` ; sinon référence. `m = 0` : pas de clause `LOCAL`. Toujours premier élément de la ligne.

---

## 7. Séquences composées

Positions en notation `@p` (octet de départ dans le bloc).

```text
SI c ALORS i1 SINON i2      [c] C0 @S [i1] C1 @F  @S:[i2]  @F:
SI c ALORS i1               [c] C0 @F [i1]  @F:
x ← SI c ALORS e1 SINON e2  [ref x] [c] C0 @S [e1] C1 @F  @S:[e2]  @F: 99
FAIRE n1 POUR V←v1 PAS p JUSQUA v2          [n1] [v1] C9 idV [p] CA [v2] CF [corps…]
FAIRE n1 POUR V←v1 JUSQUA v2                [n1] [v1] C9 idV CE [v2] CF [corps…]
FAIRE n1 POUR V←v1 {PAS p} TANT QUE c       [n1] [v1] C9 idV {[p] CA | CE} [c] D0 [corps…]
FAIRE n1 TANT QUE c (LSE F4)                [n1] D2 [c] D0 [corps…]
   fin d'itération (après la ligne de fin effective) : reprise à l'octet qui suit C9 idV ou D2
AFFICHER sans format        B8 { [x] B6 }…
AFFICHER[format] liste      B7 puis, dans l'ordre du format : B0–B4 (facteur variable : [v] avant l'élément),
                            B5 suivi de r × ([x] B6) ; puis valeurs restantes ([x] B6)…
LIRE a1,…,an                D1 { [ref ai] BB }…
appel instruction           40 idP [a1]…[ak] A8 00 k
appel expression            40 idP [a1]…[ak] A8 01 k
PROCEDURE &P(…) LOCAL …     CD idP n id… m id… { [instructions après ';'] }
commentaire                 … CC n txt            (toujours dernier élément)
x[i{,j}] ← e                [i] {[j]} 41 idx k [e] 99
TABLEAU T[d1{,d2}]          [d1] {[d2]} A9 idT dim
```

Pile d'un élément : déterminée par son seul octet et sa charge utile (y compris `A8` grâce à `k`), ce qui permet au décompilateur de délimiter chaque sous-séquence.

---

## 7 bis. Structures C

```c
typedef struct {
    unsigned char  numero_ligne;      /* 1-250 (M15) ou 1-255 (T1600) */
    unsigned int   longueur_octets;   /* <= 255 */
    HandleObjet    bloc;              /* gestionnaire d'objets ; jamais un pointeur persistant */
} EntreeObjetCompile;                 /* table triée par numero_ligne, recherche dichotomique (IMP-0106) */

typedef unsigned char NatureSymbole;
#define SYM_IDENTIFICATEUR  0          /* catégorie effective tenue par l'état d'exécution */
#define SYM_PROCEDURE       1          /* nom introduit par & */

typedef unsigned char OrigineSymbole;
#define ORIGINE_PROGRAMME       0
#define ORIGINE_MACHINE_BUREAU  1

typedef struct {
    unsigned char numero_ligne_entree; /* 0 : aucune déclaration en vigueur */
    unsigned char arite;
} InfoProcedure;

typedef struct {
    unsigned char   nom_longueur;      /* 1-5 */
    char            nom[5];            /* sans le & */
    NatureSymbole   nature;
    OrigineSymbole  origine;
    InfoProcedure   procedure;         /* significatif pour SYM_PROCEDURE */
} EntreeSymbole;                       /* 10 octets ; index = id (u16) ; aucune valeur d'exécution */

typedef struct {
    unsigned char  longueur;           /* <= 78 */
    HandleObjet    texte;              /* jamais modifié */
} EntreeConstante;                     /* table jamais réduite en cours de session */

typedef struct {
    const unsigned char *base;         /* résolution temporaire du HandleObjet */
    unsigned int          longueur;
    unsigned int          position;    /* 0 <= position <= longueur */
} CurseurSequence;

void    premier_element(CurseurSequence *c, const unsigned char *base, unsigned int longueur);
int     fin_sequence(const CurseurSequence *c);
TagRCIM lire_tag(const CurseurSequence *c);
void    element_suivant(CurseurSequence *c);   /* longueur déduite de l'étiquette et de la charge utile */
```

---

## 8. Bornes et limites

| Grandeur | Borne | Source |
|---|---|---|
| Longueur de ligne (texte reçu, hors fin de ligne) | 79 caractères | Compilateur §9 ; INRDP75 |
| Séquence compilée | ≤ 255 octets (`ERR_LIGNE_TROP_COMPLEXE`) | IMP-0105, Compilateur IMP-0165 |
| Numéro de ligne | 1–250 (M15), 1–255 (T1600) | IMP-0125, Compilateur IMP-0180 |
| Identificateur | lettre + ≤ 4 lettres ou chiffres ; `&` en plus pour une procédure | IMP-0124 |
| Identifiants de symbole | 0–6 552 (table de 6 553 entrées au plus), codés sur 16 bits | IMP-0104 |
| Index de constantes | 0–21 844 (table de 21 845 entrées au plus), codés sur 16 bits | IMP-0104, IMP-0128 |
| Chaîne en ligne | 0–127 caractères ; seuil de partage : > 16 | IMP-0128, IMP-0107 |
| Facteur de répétition fixe | 1–99 (M15), 1–62 (T1600) | IMP-0116 |
| `e`, `d` de `Fe.d`, `Ee.d` | 0–99 | RCI-M 0x35 |
| Arité, `k` d'APPEL | 0–255 (≤ 32 en pratique) | IMP-0114, IMP-0177 |
| Indices d'une référence indexée | 1 ou 2 | IMP-0120 |
| Entier immédiat sans charge utile | −28 à +28 | IMP-0123 |

---

## 9. Restitution (IMP-0183)

- **Fidèle :** noms (`&` selon la nature) ; déclaration `0xCD` dans l'ordre écrit ; `PAS` présent (`CA`) ou absent (`CE`) ; frontières AFFICHER (`B7`, `B8`) et LIRE (`D1`) ; `TEM`/`DAT` selon le profil ; chaînes et libellés de format (apostrophe doublée ; sous `LSE F4`, caractère non imprimable 0–31 ou 127 en équivalent numérique `.n.`, consécutifs regroupés, Compilateur IMP-0190 ; sous T1600 version 4 et MITRA 15, tout caractère tel quel) ; commentaire à l'octet près.
- **Espaces :** `numéro␣`, sauf ligne de commentaire `numéro*texte` (texte à l'octet près) ; un espace autour de chaque mot du langage ; `AFFICHER[` et `FONCTION(` sans espace ; aucun autre espace.
- **Parenthèses :** seulement les nécessaires (rangs : fonction › `↑` › `*` `/` › `+` `-` et changement de signe ; gauche à droite à rang égal ; `ET` avant `OU`) : opérande droit parenthésé s'il est de rang égal ou inférieur, opérande gauche s'il est de rang inférieur. Pas de `+` en tête. Constante négative : `-n`, parenthésée comme un changement de signe (`A*(-1)`).
- **Nombres :** entiers en décimal sans point ; réels à 6 chiffres significatifs au plus, sans zéros superflus, décimaux si |v| = 0 ou 10⁻³ ≤ |v| < 10⁶, sinon `mE±ee` (1 ≤ m < 10, exposant signé à deux chiffres).
- **Normalisations :** `DEBUT…FIN` pour une branche de plusieurs instructions, pour une branche vide, et pour la branche ALORS (une seule instruction) d'un SI à SINON quand elle se termine par un SI ouvert (SI sans SINON, ou SI dont la branche SINON, sans bloc, se termine par un SI ouvert) ; sinon aucun bloc ; instructions vides non restituées (ligne vide : `numéro␣`) ; `A9`, `AA` ou `AB` consécutifs fusionnés en une instruction.
- **Longueur :** jamais de troncature ; une ligne restituée de plus de 79 caractères est écrite telle quelle par DECODER et signalée par ENCODER (C 63).

**Diagnostic du compilateur** (IMP-0195) : `DiagnosticCompilation { NatureErreur nature; unsigned char position; }` ; les 83 natures `ERR_C_*` et leurs numéros sont ceux du catalogue (chapitre Gestionnaire des erreurs §7, fichiers générés `src/noyau/erreurs.h` et `erreurs.c`).

---

## 10. Points à confirmer

Aucun. Les deux points relevés lors de la première rédaction (ordre des octets des champs `u16`, repli du changement de signe sur une constante) sont tranchés dans RCI-M (IMP-0184 ; §4.4, « Constante négative »).

---

## 11. Vecteurs de référence

Chaque vecteur suppose une table des symboles vide au départ de l'exemple, les identifiants étant attribués dans l'ordre de première apparition dans le texte ; sauf mention, profil MITRA 15. Octets en hexadécimal ; `|` sépare les éléments pour la lecture. Chaque texte restitué se recompile en la même séquence.

**V1 — affectation, postfixe.** `5 A←B+C` ; A=0, B=1, C=2.
`40 00 00 | 40 01 00 | 40 02 00 | 80 | 99` (11 octets). Restitution : `5 A←B+C`.

**V2 — associativité à gauche.** `5 X←2↑3↑2` ; X=0.
`40 00 00 | 25 | 26 | 85 | 25 | 85 | 99`. Valeur 64. Restitution : `5 X←2↑3↑2`.

**V3 — changement de signe au rang 4.** `5 X←-A↑2` ; X=0, A=1.
`40 00 00 | 40 01 00 | 25 | 85 | 82 | 99`. Restitution : `5 X←-A↑2`.

**V4 — SI avec SINON, positions.** `5 SI A=0 ALORS B←1 SINON B←2` ; A=0, B=1.
`40 00 00 | 23 | 90 | C0 0E | 40 01 00 | 24 | 99 | C1 13 | 40 01 00 | 25 | 99` (19 octets ; `0x0E` = début de SINON, `0x13` = fin).

**V5 — SI en position d'expression (INRDP75, PG26 l. 30).** `30 CH←SI N>1 ALORS ' LETTRES ' SINON ' LETTRE '` ; CH=0, N=1.
`40 00 00 | 40 01 00 | 24 | 94 | C0 17 | 03 09 20 4C 45 54 54 52 45 53 20 | C1 21 | 03 08 20 4C 45 54 54 52 45 20 | 99` (34 octets).

**V6 — FAIRE avec PAS, constante négative repliée (RCI-M §14).** `3 FAIRE 8 POUR I←T PAS -1 JUSQUA 1` ; I=0, T=1.
`2B | 40 01 00 | C9 00 00 | 22 | CA | 24 | CF`.

**V7 — FAIRE sans PAS (LSE15 §5-5.3).** `200 FAIRE 202 POUR I←1 JUSQUA N` ; I=0, N=1.
`01 CA 00 | 24 | C9 00 00 | CE | 40 01 00 | CF`. Restitution sans `PAS`.

**V8 — FAIRE TANT QUE, pas réévalué (LSE15 §5-5.3).** `20 FAIRE 20 POUR X←A PAS (A/X-X)/2 TANT QUE ABS(X↑2-A)>1E-3` ; X=0, A=1.
`37 | 40 01 00 | C9 00 00 | 40 01 00 | 40 00 00 | 84 | 40 00 00 | 81 | 25 | 84 | CA | 40 00 00 | 25 | 85 | 40 01 00 | 81 | 87 | 02 6F 12 83 3A | 94 | D0`.
Restitution : `20 FAIRE 20 POUR X←A PAS (A/X-X)/2 TANT QUE ABS(X↑2-A)>0.001`.

**V9 — AFFICHER à format explicite (LSE15 §6-2.4).** `5 AFFICHER['RES=',3F2.0,E3.6] A,B,C,D,E` ; A=0 … E=4.
`B7 | B0 01 04 52 45 53 3D | B5 02 02 00 | 40 00 00 | B6 | 40 01 00 | B6 | 40 02 00 | B6 | B5 40 03 06 | 40 03 00 | B6 | 40 04 00 | B6`.

**V10 — AFFICHER sans format, chaîne avec apostrophe, LIRE (INRDP75).** `2 AFFICHER 'D''UN TRIANGLE : ';LIRE A,B,C` ; A=0, B=1, C=2.
`B8 | 03 10 44 27 55 4E 20 54 52 49 41 4E 47 4C 45 20 3A 20 | B6 | D1 | 40 00 00 | BB | 40 01 00 | BB | 40 02 00 | BB`. Chaîne de 16 caractères, donc en ligne.

**V11 — déclaration et appel récursif (LSE15 §7-6).** Lignes compilées dans l'ordre : `25 PROCEDURE &FACT(N) LOCAL N` puis `27 RESULTAT SI N=0 ALORS 1 SINON N*&FACT(N-1)` ; &FACT=0, N=1.
Ligne 25 : `CD 00 00 01 01 00 01 01 00`.
Ligne 27 : `40 01 00 | 23 | 90 | C0 0A | 24 | C1 19 | 40 01 00 | 40 00 00 | 40 01 00 | 24 | 81 | A8 01 01 | 83 | CB`.

**V12 — appel en instruction, nom de procédure en argument (LSE15 §7-8.2).** `5 &TRAC(&F1,0,2,0.25)` ; &TRAC=0, &F1=1.
`40 00 00 | 40 01 00 | 23 | 25 | 02 00 00 80 3E | A8 00 04`.

**V13 — tableau et accès indexé.** `5 TABLEAU MAT[10,5];MAT[I,J]←0` ; MAT=0, I=1, J=2.
`2D | 28 | A9 00 00 02 | 40 01 00 | 40 02 00 | 41 00 00 02 | 23 | 99`.

**V14 — commentaire à l'octet près.** `1 * CALCUL DE L'AIRE`.
`CC 11 20 43 41 4C 43 55 4C 20 44 45 20 4C 27 41 49 52 45` (texte de 17 caractères, espace de tête compris). Restitution : `1* CALCUL DE L'AIRE`.

**V15 — constructions de profil.** T1600 : `10 ALLER EN @+3` → `42 | 26 | 80 | C3`. MITRA 15 : `10 I←@50` ; I=0 → `40 00 00 | 06 32 | 99`.

**V16 — constante négative et parenthèses.** `5 X←A*(-1)` ; X=0, A=1.
`40 00 00 | 40 01 00 | 22 | 83 | 99`. Restitution : `5 X←A*(-1)`. Et `5 X←-200` → `40 00 00 | 01 38 FF | 99`.

**V17 — FAIRE sans variable (`LSE F4`, IMP-0277).** `105 FAIRE 105 TANT QUE A<4` ; A=0.
`00 69 | D2 | 40 00 00 | 27 | 91 | D0`. Restitution à l'identique.
