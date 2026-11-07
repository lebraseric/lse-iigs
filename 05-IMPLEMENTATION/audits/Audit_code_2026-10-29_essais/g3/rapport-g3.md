# Audit du code LSE-IIGS — groupe 3 (moniteur, programme courant, décompilateur, gestionnaire des erreurs)

**Date :** 2026-10-29. **Périmètre :** `src/noyau/moniteur.c/.h`, `src/noyau/programme.c/.h`, `src/noyau/decompilateur.c/.h`, `src/noyau/erreurs.c/.h`.
**Méthode :** lecture complète des quatre modules, des chapitres du Tome V cités (Programme courant, Décompilateur, Moniteur et console, Gestionnaire des erreurs, Tâches et reprise, Gestionnaire d'objets §8, §11, §16) et des chemins d'appel sortants nécessaires. Essais compilés par ORCA/C 2.2.1 en profil `cible` (`#pragma debug 1`, `#pragma optimize 7`) et exécutés sous Golden Gate, dans la copie de travail `S/copie-g3/` :
- `S/copie-g3/tests/composants/audit_g3.c` (règle `tests/audit_g3` ajoutée à `S/copie-g3/build/Makefile`) : sessions complètes pilotées par le bouchon de console (défauts D1 à D7), comptage des opérations flottantes de l'écriture d'un réel, profondeur de pile mesurée par peinture de la zone de pile (fonction `zone_op`, cadre identique à la peinture et à la lecture) ;
- `S/copie-g3/tests/composants/audit_g3_pile.c` (règle `tests/audit_g3_pile`) : les mêmes sessions sous une pile de 4 Kio (`#pragma stacksize 4096`, taille de la pile du lanceur du shell ORCA, manuel ORCA/C 2.2, *stacksize* : « this is 4K for the System 6.0 Finder, PRIZM 2.0 and the ORCA/M 2.0 shell »).
Sorties : `S/audit_g3.out`, `S/pile_0.out` à `S/pile_19.out` (S = bloc-notes de la session).
Aucun fichier du dépôt n'a été modifié.

Mesures de référence (Apple IIGS d'Éric, accélérateur à 16 MHz, profil `cible`, `05-IMPLEMENTATION/audits/Mesures_performance_IIGS_2026-10-25.md`) : opération SANE 0,5 à 0,9 ms ; comparaison de `double` 0,45 ms ; `objets_resoudre` + `objets_relacher` 1,1 ms ; `objets_copier` 0,133 ms ; boucle (b) 7,1 ms par tour ; lecture sans attente du pilote `.CONSOLE` (un appel GS/OS) 0,517 ms. Sur un Apple IIGS sans accélérateur (2,8 MHz), les coûts processeur et SANE sont environ cinq fois plus élevés.

---

## 1. Carte des chemins chauds

### 1.1 Par ligne de programme exécutée (hors groupe, appels vers le groupe)

| Appelant | Appel dans le groupe | Fréquence | Coût mesuré |
|---|---|---|---|
| `contexte_fin_de_ligne` (`contexte_exec.c:500`) | `programme_suivante` (`programme.c:246`) | une fois par ligne en progression | 0,100 ms (mesuré sur 3 lignes) |
| `charger_ligne` (`interprete.c:1213-1222`) | `programme_chercher` (`programme.c:231`) | une fois par changement de ligne | 0,067 ms (mesuré sur 3 lignes) |
| `contexte_*` (branchement, `contexte_exec.c:516, 862`) | `programme_chercher` | par ALLER EN, RETOUR EN, appel | 0,067 ms |
| appel de procédure (`contexte_exec.c:714-715`) | `programme_parametres` (`programme.c:274`, recherche linéaire sur les déclarations) + `programme_chercher` | par appel | négligeable (quelques déclarations) |
| `afficher_valeur` → `ecrire_nombre` (`interprete.c:472-477`) → `donnees_ecrire_nombre` (`donnees.c:491`, forme U) | `decompilateur_ecrire_reel_chiffres` (`decompilateur.c:387`) | **par nombre affiché sans format ou en U** | 59 à 1 054 multiplications ou divisions `double` (G3-P-01) |
| `donnees_cca` (`donnees.c:442-447`) | `decompilateur_ecrire_reel_chiffres` | par appel de `CCA` | idem |
| `erreur` de l'interpréteur → moniteur | `erreurs_formater_execution` (`erreurs.c:271`) | par erreur | négligeable |

Sur 255 lignes, la dichotomie de `rang_au_moins` (`programme.c:35-47`) fait 8 itérations au lieu de 2 sur le programme de 3 lignes de la mesure (`tests/plateforme/sonde_performance.c:64`) : quelques centièmes de milliseconde de plus, sans effet perceptible.

### 1.2 Par commande ou par entrée

| Commande ou entrée | Chemin | Coût dominant |
|---|---|---|
| toute saisie | `moniteur_session` (`moniteur.c:1652`) → `lire_entree` (`moniteur.c:197`, `taches_suspendre`/`taches_reactiver`, correction `\`) → `analyseur_classer` → `correction` (`moniteur.c:1584`, rend 0 aussitôt hors correction) | vitesse de frappe |
| ligne de programme | `ligne_programme` (`moniteur.c:1509`) → `compilateur_compiler` → `programme_inserer` (`programme.c:185`) → `decoder_declaration` (`programme.c:68`, `objets_resoudre`/`objets_relacher` à chaque ligne) | 1,1 ms de verrouillage par ligne (G3-P-03) |
| ligne de bureau | `ligne_bureau` (`moniteur.c:1535`) → `compilateur_compiler` → `interprete_bureau` | AFFICHER d'un nombre (G3-P-01) |
| LISTER, PERFORER | `lister` (`moniteur.c:401`) : par ligne, `programme_rang` O(1), `console_interruption` (0,017 ms), `decompilateur_decompiler` (`decompilateur.c:1025` ; `objets_resoudre`/`objets_relacher` 1,1 ms), `console_ecrire` puis `a_la_ligne` (trois appels `DWrite`), `viser` (copie ≤ 255 octets) | 1,1 ms + 3 appels GS/OS par ligne ; + 30 à 950 ms par constante réelle de la ligne (G3-P-01) |
| NUMERO | `lister` mode 1 : un `console_ecrire` par numéro | ≤ 255 appels `DWrite` |
| EFFACER n1 A n2 | `effacer` (`moniteur.c:467`) → `programme_supprimer` par numéro (dichotomie, `memmove` ≤ 1 275 octets) | négligeable |
| APPELER | `bibliotheque_appeler` → `reconstruire` (hors groupe) → `programme_inserer` par ligne | 2,2 ms de verrouillage par ligne, dont 1,1 ms dans `programme.c` (G3-P-03) |
| ENCODER | `encoder` (`moniteur.c:1355`) : `codage_ligne`, `compilateur_compiler`, `programme_inserer` par ligne | compilation |
| TABLE DES FICHIERS, UTILISATION DISQUE | `table` → `section_mitra`/`lignes_t1600` → `fichier_suivant` → `espace_suivant` (`moniteur.c:903`) ou `bibliotheque_catalogue_suivant` ; `octets` par fichier (Open, GetEOF, Close) | O(N²) appels GS/OS (G3-P-02) |
| DUPLIQUER avec `*` | `dupliquer` (`moniteur.c:1238`) → `espace_suivant` par fichier | O(N²) appels GS/OS (G3-P-02) |

---

## 2. Budget de pile

### 2.1 Variables locales de plus de 64 octets (groupe 3)

| Fonction | Locales (estimation d'après les déclarations) |
|---|---|
| `decompilateur_decompiler` (`decompilateur.c:1025`) | `Decompilation d` : 4 + 2 + 128 × 12 + 2 + 1 + 1 = 1 546 octets, plus `numero[8]` : ≈ 1 565 |
| `element` (`decompilateur.c:669`) | `s[256]` et scalaires : ≈ 275 |
| `lister` (`moniteur.c:401`) | `texte[256]`, `EntreeObjetCompile` : ≈ 280 |
| `correction` (`moniteur.c:1584`) | `res[256]` : ≈ 280 |
| `commande` (`moniteur.c:1425`) | `t[256]` : ≈ 268 |
| `lire_entree` (`moniteur.c:197`) | `brut[256]` : ≈ 266 |
| `encoder` (`moniteur.c:1355`) | `m[100]`, `nom[8]`, `ResultatCompilation` (8), `DiagnosticCompilation` (3) : ≈ 135 |
| `ligne_bureau` (`moniteur.c:1535`) | `t[100]`, `r`, `d`, `b` : ≈ 125 |
| `section_mitra` (`moniteur.c:1015`) | `m[80]`, `cle[16]`, `nom[16]` : ≈ 125 |
| `ligne_programme` (`moniteur.c:1509`) | `t[100]`, `r`, `d` : ≈ 115 |
| `erreur_execution`, `erreur_fichier` (`moniteur.c:99, 110`) | `t[100]` |
| `erreur_dialogue`, `table_mitra`, `occupation`, `ligne_t1600` | `t[80]` ou `m[80]` |
| `identification`, `liste_id` (`moniteur.c:738, 863`) | `m[60]` et une `Identification` : ≈ 70 |

`programme.c` et `erreurs.c` n'ont aucune locale de plus de 64 octets (`TableParametresFormels` : 13 octets).

Hors groupe, sur les mêmes chaînes : `compilateur_compiler` (`octets[255]`, `Analyse`) ≈ 290 ; `interprete_executer`/`interprete_bureau` (`Cycle`, dont `seq[255]`) ≈ 280 ; `ecrire_nombre` (`interprete.c:474`, `t[300]`) ; `donnees_ecrire_nombre` (`donnees.c:497`, `corps[320]`) ; `chiffres_entier` (`donnees.c:466`, `t[320]`) ; `bibliotheque_catalogue_suivant` (`bibliotheque.c:345`, `t[16 * CATALOGUE_TAILLE]`).

### 2.2 Récursion

Aucune dans le groupe : le décompilateur emploie une pile de travail explicite (Décompilateur §4.2) ; le moniteur n'appelle jamais `moniteur_session` ; `correction` appelle `ligne_programme` ou `ligne_bureau` une seule fois. La seule récursion des chaînes mesurées est celle de l'analyse des expressions du compilateur (hors groupe) : `atome` → `expression` → `concatenation` → `somme` → `terme_signe` → `terme` → `puissance` → `atome` (`compilateur.c:588-803`), sept cadres par niveau de parenthèses.

### 2.3 Profondeur mesurée (essai `audit_g3`, profil `cible`)

Octets de pile employés sous le point de mesure, situé à 31 octets sous le cadre de `main` (équivalent de l'appel de `moniteur_session` par `main.c`) :

| Session | Octets |
|---|---|
| `BO` puis `AU` | 732 |
| ligne de programme `10 X_1` | 1 068 |
| ligne de programme, par niveau de parenthèses (4, 8, 12, 16, 24 niveaux : 2 020, 2 972, 3 924, 4 876, 6 780) | **+ 238 par niveau** |
| ligne de programme à quatre `SI` imbriqués | 1 517 |
| ligne de bureau `?1`, `?38-24+47` | 1 971, 1 973 |
| ligne de bureau, par niveau de parenthèses (8 : 3 875 ; 16 : 5 779) | **+ 238 par niveau** |
| EXECUTER, fonction affichant en F, E et U | 2 017 |
| EXECUTER, `CCA` | 1 951 |
| TABLE DES FICHIERS (MITRA 15) | 2 233 |
| LISTER de trois lignes (format, SI imbriqués, expression à 8 niveaux) | 2 777 |
| décompilation seule d'une ligne (sans le moniteur) | 2 056 à 2 137 |
| erreur d'exécution | 1 068 |

### 2.4 Sous une pile de 4 Kio (essai `audit_g3_pile`, `#pragma stacksize 4096`)

| Session | Résultat |
|---|---|
| ligne de programme à 0, 4, 6, 8, 9, 10, 11 niveaux de parenthèses | acceptée |
| `10 X_((((((((((((1))))))))))))` (12 niveaux, 31 caractères) | **`Stack overflow`, fin du programme (code 255)** |
| ligne de bureau à 0, 2, 4, 5, 6, 7 niveaux | exécutée |
| `?((((((((1))))))))` (8 niveaux, 18 caractères) | **`Stack overflow`, fin du programme** |
| LISTER, EXECUTER avec AFFICHER F/E/U, ENCODER d'une ligne à 8 niveaux, ligne réaliste à 4 niveaux et SI | sans erreur |

Profondeur maximale tolérée sous 4 Kio, d'après ces deux séries : entre 3 686 et 3 875 octets sous le point de mesure. Marge des usages courants : LISTER ≈ 1 000 octets ; une ligne de bureau simple ≈ 1 800 octets, soit sept niveaux de parenthèses. Profondeur maximale du langage : une ligne de 79 caractères peut porter 36 niveaux (ligne de programme, ≈ 9 640 octets) ou 38 (ligne de bureau, ≈ 11 020 octets).

---

## 3. Constats

### G3-A-01 — Une ligne de bureau en erreur, ou interrompue par ESC pendant LIRE, laisse ses éléments sur la pile d'évaluation du programme suspendu
- Gravité : bloquant (comportement L.S.E. faux et silencieux après CONTINUER : valeur lue rangée dans une autre variable, résultat de fonction perdu)
- Statut : CONFIRMÉ (essais D1 à D4 de `audit_g3.c`, lecture complète de `ligne_bureau`, `interprete_bureau`, `cycle`, `lire`)
- Emplacement : `src/noyau/moniteur.c:1535-1564` (`ligne_bureau`)
- Constat : `ligne_bureau` appelle `interprete_bureau(&pile, …)` sur la pile du programme suspendu, sans noter sa hauteur ni la rétablir. Après `FIN_ERREUR`, l'interpréteur laisse la pile en l'état (Interpréteur IMP-0214 : la remise en ordre « relève du moniteur ») ; après ESC pendant un `LIRE` de bureau, la référence de la variable à lire reste au sommet. La règle « entre deux instructions, la pile d'un cadre revient toujours à sa base » (État d'exécution §7.3) est alors rompue, et la reprise du programme lit ces éléments étrangers. Le bloc transitoire de la ligne de bureau est en outre libéré (`moniteur.c:1563`) alors qu'une chaîne en ligne restée sur la pile peut le désigner.
- Scénario :
  - D1 : `10 LIRE A,B`, `20 AFFICHER A,B`, `30 TERMINER` ; EX 10 ; `1` ; ESC (`PRET EN LIGNE 010`) ; `?A+C` (C non définie : `ERREUR (EX.) 05 EN LIGNE BUR`) ; CO ; `2` → `1` puis `ERREUR (EX.) 62 EN LIGNE 020` : la valeur 2 est rangée hors de B. Témoin avec `?A` : `1 2`, `TERMINE`.
  - D2 : même programme, `LIRE X` en bureau interrompu par ESC, puis CO et `2` → même erreur EX 62.
  - D3 : `10 X_&F(1)+1`, `20 AFFICHER X`, `40 PROCEDURE &F(A) LOCAL A`, `50 PAUSE`, `60 RESULTAT A*10` ; EX 10 ; `?A+C` (erreur) ; CO → `ERREUR (EX.) 62 EN LIGNE 020` au lieu de `11`. Témoin avec `?A` : `11`, `TERMINE`.
  - D4 : même programme, `LIRE Y` de bureau interrompu par ESC, puis CO → EX 62.
- Référence : Moniteur IMP-0233 (« elle ne change pas l'état du programme »), IMP-0237 ; Interpréteur IMP-0214 ; État d'exécution §7.3, §7.4 ; EXI-0029.
- Proposition : dans `ligne_bureau`, mémoriser `h = pile.sommet` avant le premier `interprete_bureau` et appeler `pile_ramener(&pile, h)` après la boucle, quelle que soit la fin (`FIN_BUREAU`, `FIN_ERREUR`, ESC), avant `objets_liberer` du bloc. `pile_ramener` libère les chaînes transitoires (`pile.c:108-111`). Coût nul.
- Décision IMP nécessaire : non (réalisation d'IMP-0233) ; une phrase d'IMP-0233 peut préciser que la pile est ramenée à sa hauteur d'entrée.
- Essai : `S/copie-g3/tests/composants/audit_g3.c` (D1 à D4) ; sortie `S/audit_g3.out`.

### G3-A-02 — Débordement de la pile du 65816 : une ligne à 12 niveaux de parenthèses, ou une ligne de bureau à 8, arrête le programme LSE
- Gravité : bloquant (arrêt du programme `LSE` par l'erreur d'exécution d'ORCA/C n° 15 `Stack overflow` ; programme courant et données non rangés perdus)
- Statut : CONFIRMÉ sous Golden Gate avec une pile de 4 Kio imposée (`audit_g3_pile`, scénarios 7 et 14) ; la taille de 4 Kio sur l'Apple IIGS vient du manuel ORCA/C (*stacksize* : pile du lanceur, 4K sous le shell ORCA) et de l'absence de `#pragma stacksize` dans le dépôt (vérifié : aucune occurrence) ; non essayé sur l'Apple IIGS.
- Emplacement : `src/plateforme/main.c` (pas de `#pragma stacksize`) ; appelants du groupe `moniteur.c:1509` (`ligne_programme`), `moniteur.c:1535` (`ligne_bureau`), `moniteur.c:1355` (`encoder`), `moniteur.c:1584` (`correction`) ; origine principale hors groupe : `compilateur.c:588-803` (238 octets par niveau de parenthèses).
- Constat : la profondeur mesurée croît de 238 octets par niveau de parenthèses (§2.3). Sous 4 Kio, le contrôle de débordement de `#pragma debug 1` (manuel ORCA/C, *debug* bit 0) arrête le programme entre 11 et 12 niveaux pour une ligne de programme, entre 7 et 8 pour une ligne de bureau (dont la base est plus haute de 900 octets à cause de l'affichage : `t[300]`, `corps[320]`, SANE). Une ligne de 79 caractères admet jusqu'à 36 ou 38 niveaux.
- Scénario : `10 X_((((((((((((1))))))))))))` tapé → `Stack overflow`, retour au shell ; `?((((((((1))))))))` → idem. Les usages courants passent (LISTER 2 777 octets, ligne réaliste à quatre niveaux).
- Référence : manuel ORCA/C 2.2, *stacksize*, *debug* ; Décompilateur §2 (« la pile C d'ORCA/C partage l'espace du runtime ») ; EXI-0004 (une ligne rejetée ne modifie pas le programme ; ici la session entière est perdue).
- Proposition : `#pragma stacksize 16384` dans `main.c` (12 Kio suffisent aux 38 niveaux mesurés ; le manuel indique jusqu'à 32 Kio en environnement texte), à vérifier sur l'Apple IIGS (place en banque 0 sous le shell ORCA). En complément, sans changer le comportement : rendre statique `Decompilation d` (`decompilateur.c:1028`, − 1,5 Kio sur LISTER et DECODER ; le décompilateur n'est jamais réentrant) ; réduire les cadres de la récursion du compilateur (groupe 1).
- Décision IMP nécessaire : oui, nouvelle (taille de pile du programme `LSE`, Construction ou Architecture physique).
- Essai : `S/copie-g3/tests/composants/audit_g3_pile.c` ; sorties `S/pile_7.out`, `S/pile_14.out` (`Stack overflow`) et `S/pile_0.out` à `S/pile_19.out`.

### G3-A-03 — Un LIRE de machine de bureau pendant une lecture suspendue perd les valeurs déjà lues par le programme
- Gravité : mineur (scénario rare ; comportement contraire à IMP-0237)
- Statut : CONFIRMÉ (essai D6)
- Emplacement : `src/noyau/moniteur.c:1535-1564` (`ligne_bureau`) ; état de lecture partagé hors groupe : `interprete.c:578-583`, `interprete.c:1184-1187` (`RCIM_CTL_LIRE_TETE` remet `rang_tableau` à 0)
- Constat : l'état de la lecture en cours (`saisie`, `saisie_pos`, `rang_tableau`) est statique et commun au programme et à la machine de bureau ; le moniteur ne le sauvegarde pas autour d'une ligne de bureau.
- Scénario : `10 TABLEAU T[3]`, `20 LIRE T`, `30 AFFICHER T` ; EX 10 ; `1 2` ; ESC ; `LIRE X` puis `9` ; CO → les trois éléments sont redemandés ; `3`, `4`, `5` → `3 4 5` au lieu de `1 2 3` après la seule saisie de `3`.
- Référence : Moniteur IMP-0237 (« les valeurs déjà lues étant conservées »), IMP-0233.
- Proposition : une paire `interprete_sauver_lecture` / `interprete_restaurer_lecture` (état de lecture, 260 octets) appelée par `ligne_bureau` quand `etat_programme == PROG_SUSPENDU` ; ou interdire `LIRE` en machine de bureau pendant une lecture suspendue (choix de dialogue).
- Décision IMP nécessaire : oui (révision d'IMP-0214 ou d'IMP-0237 ; point à arbitrer 2).
- Essai : `audit_g3.c`, D6.

### G3-A-04 — Le reste d'une ligne tapée pour un programme terminé alimente le LIRE suivant de la machine de bureau, sans attente
- Gravité : mineur
- Statut : CONFIRMÉ (essai D5) ; qualification de défaut à arbitrer
- Emplacement : `src/noyau/moniteur.c:298-355` (`executer`, aucun `interprete_reinitialiser` à la fin d'exécution) et `moniteur.c:1535` (`ligne_bureau`) ; contrat `src/noyau/interprete.h:41-42` (« nouvelle exécution (EXECUTER, REPRENDRE) ou ligne de machine de bureau »)
- Constat : `interprete_reinitialiser` n'est appelé qu'à EXECUTER, REPRENDRE, APPELER, ENCODER et après EXECUTER en cours d'exécution ; jamais pour une ligne de bureau, contrairement au commentaire de `interprete.h`, et l'en-tête d'IMP-0214 ne cite que EXECUTER et REPRENDRE.
- Scénario : `10 LIRE A`, `20 TERMINER` ; EX 10 ; `1 2` → `TERMINE` ; `LIRE X` → X reçoit 2 sans rien demander ; la saisie suivante `7` est lue comme une ligne (`ERREUR  C 07`) ; `?X` → `2`.
- Référence : Moniteur §11 point L6 (le reste est conservé pour le `LIRE` suivant, attesté pour deux instructions d'un même programme) ; `interprete.h:41-42`.
- Proposition : appeler `interprete_reinitialiser()` dans `executer()` quand la fin est `FIN_TERMINER` ou `FIN_ERREUR` (programme fini : sa saisie en suspens n'a plus d'objet), ce qui n'affecte pas la reprise après ESC (`FIN_INTERRUPTION`) ; aligner le commentaire de `interprete.h` sur IMP-0214.
- Décision IMP nécessaire : oui, à arbitrer (portée du point L6).
- Essai : `audit_g3.c`, D5.

### G3-A-05 — Échec de `contexte_initialiser` ou de `contexte_reprendre` après l'abandon des cadres : l'état du programme reste « suspendu »
- Gravité : mineur (manque de mémoire seulement)
- Statut : PLAUSIBLE (chemin lu entièrement : `moniteur.c:383-392`, `contexte_exec.c:403-410`, `contexte_exec.c:433-445` ; non essayé : il faudrait provoquer l'échec de `malloc` dans `creer_cadre`)
- Emplacement : `src/noyau/moniteur.c:389-392` (`lancer`)
- Constat : `contexte_initialiser` abandonne tous les cadres avant `creer_cadre` ; si cette allocation échoue (`ERR_E_MEMOIRE_SATUREE`), `courant` vaut `NULL`. `lancer` affiche l'erreur mais laisse `etat_programme` inchangé : après une PAUSE, CONTINUER reste admis (compteur des lignes inchangé) et relance l'interpréteur sans cadre courant.
- Scénario : programme suspendu ; mémoire presque épuisée ; EXECUTER → `ERREUR (EX.) 03` ; CONTINUER → exécution avec `courant == NULL` (lecture à l'adresse 0 en profil `cible`).
- Référence : Moniteur IMP-0232 ; État d'exécution §7.1.
- Proposition : dans `lancer`, `etat_programme = PROG_AUCUN` dès que `e != ERR_AUCUNE && e != ERR_E_LIGNE_INEXISTANTE`.
- Décision IMP nécessaire : non.
- Essai : aucun.

### G3-P-01 — Écriture d'un réel : les puissances de dix sont recalculées par multiplications successives à chaque mise à l'échelle (AFFICHER, CCA, LISTER)
- Gravité : majeur (lenteur perceptible : chaque nombre affiché coûte de 5 à 140 tours de boucle FAIRE)
- Statut : CONFIRMÉ pour le nombre d'opérations (réplique instrumentée de `decompilateur_ecrire_reel_chiffres`, dont la sortie est vérifiée identique à celle de la fonction réelle) ; durée PLAUSIBLE (estimée à partir du coût mesuré d'une opération SANE)
- Emplacement : `src/noyau/decompilateur.c:366-380` (`puissance_dix`, `echelle`), `decompilateur.c:403-412`
- Constat : chaque appel d'`echelle` calcule `puissance_dix(22)` (22 multiplications `double`) puis `puissance_dix(|n|)`, et la recherche de l'exposant appelle `echelle` à chaque pas. IMP-0201 annonce « une vingtaine d'opérations flottantes par constante réelle ».
- Estimation (7 chiffres, forme U de l'affichage) :

| Valeur | Mul./div. `double` actuelles | Avec une table | Durée actuelle estimée (0,5 à 0,9 ms par opération) |
|---|---|---|---|
| 3 ; 3.14159 | 59 | 2 | 30 à 53 ms |
| 37 ; 0.5 | 83 | 3 | 42 à 75 ms |
| 200 | 108 | — | 54 à 97 ms |
| 12345 ; 65535 | 161 | 6 | 80 à 145 ms |
| 0.001 | 136 | 5 | 68 à 122 ms |
| 1E+10 | 352 | 12 | 0,18 à 0,32 s |
| 1E+30 ; 1E-30 | 1 052 ; 1 054 | 42 | 0,53 à 0,95 s |

  À comparer à 7,1 ms par tour de la boucle (b). `FAIRE 10 POUR I_1 JUSQUA 200` avec `AFFICHER I` passe environ 10 à 20 s à mettre les nombres en forme ; un LISTER paie le même prix par constante réelle. Sans accélérateur, cinq fois plus.
- Référence : Décompilateur IMP-0201 (points 2 et 3, impact annoncé) ; Données IMP-0211 (U et CCA à sept chiffres) ; mesures du 2026-10-25.
- Proposition : une table constante `static const double puissances[23] = { 1.0, 1.0e1, …, 1.0e22 }` à la place de `puissance_dix`, et `bas` lu dans une table de `long`. Les puissances de 10⁰ à 10²² sont exactes en `double` et la fonction actuelle les calcule exactement : l'arithmétique d'`echelle` reste opération pour opération la même, donc le résultat aussi. Gain : environ 40 à 70 ms par nombre courant, jusqu'à 0,9 s pour les grands exposants. Risque : exactitude des constantes décimales converties par ORCA/C, vérifiée dans l'essai (égalité bit à bit des 23 entrées avec `puissance_dix`) et par la comparaison des deux écritures sur un échantillon (voir Essai).
- Décision IMP nécessaire : non (la réalisation rejoint l'impact annoncé par IMP-0201) ; le même calcul existe dans `donnees.c` (piste hors périmètre).
- Essai : `audit_g3.c`, fonctions `compter`, `compter_table`, `equivalence` ; sortie `S/audit_g3.out` (lignes `REEL`, `REEL TABLE`, `EQUIVALENCE`). RESULTAT_EQUIVALENCE

### G3-P-02 — TABLE DES FICHIERS, UTILISATION DISQUE et DUPLIQUER avec `*` : nombre d'appels GS/OS quadratique
- Gravité : majeur au-delà d'une vingtaine de fichiers (lenteur de plusieurs secondes), mineur en deçà
- Statut : CONFIRMÉ pour la complexité (chemin lu : `moniteur.c:903-922`, `972-980`, `1034-1058`, `1153-1162`, `1298-1303` ; `fichiers_iigs.c:169-204` ; `bibliotheque.c:340-378`) ; durée PLAUSIBLE (aucune mesure d'appel GS/OS de fichier sur l'Apple IIGS)
- Emplacement : `src/noyau/moniteur.c:903-922` (`espace_suivant`) et ses appelants `fichier_suivant` (`moniteur.c:972`), `dupliquer` (`moniteur.c:1298`)
- Constat : pour obtenir le nom suivant dans l'ordre alphabétique, `espace_suivant` relit tout le répertoire ; chaque `plateforme_fichier_lister(e, r, x)` fait Open, GetDirEntry et Close. Lister N fichiers coûte donc N² lectures, soit 3N² appels GS/OS. Pour les types P et D, `bibliotheque_catalogue_suivant` relit tout le catalogue à chaque nom (N ouvertures, N²/16 lectures).
- Estimation : N = 30 temporaires → 900 lectures, 2 700 appels GS/OS, au moins 1,4 s à 0,5 ms par appel (coût mesuré d'un appel de pilote), plusieurs secondes avec les accès au disque ; DUPLIQUER `$*` depuis une disquette multiplie ce coût par la lenteur du lecteur.
- Référence : Bibliothécaire et fichiers IMP-0257, IMP-0258 (ordre alphabétique, T4).
- Proposition : une seule passe sur le répertoire (ou le catalogue), les noms rangés dans un tableau alloué (N × 7 octets), triés par `strcmp`, puis parcourus ; même ordre, même sortie. Gain : O(N²) → O(N) appels GS/OS. Risque : une allocation de plus (refus propre si elle échoue).
- Décision IMP nécessaire : non.
- Essai : aucun (Golden Gate ne reflète pas la durée des appels GS/OS).

### G3-P-03 — Chaque ligne insérée est verrouillée puis déverrouillée pour lire son premier octet
- Gravité : mineur
- Statut : CONFIRMÉ (lecture : `programme.c:81-86`)
- Emplacement : `src/noyau/programme.c:82` (`decoder_declaration`)
- Constat : `programme_inserer` appelle `decoder_declaration`, qui fait `objets_resoudre`/`objets_relacher` (1,1 ms) sur toute ligne pour tester l'élément `0xCD`, alors que seules les déclarations de procédure en ont besoin.
- Estimation : APPELER ou ENCODER de 250 lignes : 0,27 s de verrouillage dans `programme.c` (à quoi s'ajoute 1,1 ms par ligne dans `reconstruire`, hors groupe).
- Proposition : lire le premier octet par `objets_copier(l->bloc, 0, &tete, 1)` (0,133 ms) et ne résoudre le bloc que si `tete == RCIM_CTL_PROCEDURE`. Gain : ≈ 1 ms par ligne, 0,24 s sur 250 lignes. Risque : nul (même règle d'adresse qu'IMP-0243).
- Décision IMP nécessaire : oui, révision d'IMP-0243 (qui réserve `objets_copier` au chargement d'une ligne par l'interpréteur).
- Essai : aucun.

---

## 4. Points écartés après vérification

- Texte d'une constante partagée lu pendant la résolution de la ligne (`decompilateur.c:707-711`) : cas décrit et admis au Gestionnaire d'objets §11 ; entre le relâchement et les lectures suivantes de `o`, aucun appel au Memory Manager (seulement `memmove`, `memcpy` et, pour un réel, des appels SANE, qui n'allouent rien).
- `sommet(d, 0)` calculé avant le test de `suite_ouverte` (`decompilateur.c:910-913`) : l'adresse n'est pas lue quand `l == NULL` (évaluation court-circuitée).
- `supprimer` sous T1600 (`moniteur.c:603`) : `t[n]` peut être lu quand des espaces suivent la virgule, au plus un octet au-delà ; le contrôle `i != n` rejette toujours l'entrée.
- Saisie de 256 caractères : `entree[255]` au plus est écrit (`moniteur.c:239-246`), `commande` copie dans `t[256]` ; le compilateur rejette au-delà de 79 caractères (C 63).
- `erreurs.c` : messages de 78 caractères au plus avec `-LIBELLES` (tampons de 80 et 100 octets) ; `ajouter` tronque sans débordement ; `ajouter_nombre` couvre 5 chiffres.
- `DECOMP_ERR_TROP_LONGUE` (ligne vide dans LISTER, `moniteur.c:450`) : inatteignable depuis un source de 79 caractères (la restitution n'allonge que de quelques caractères).
- `decoder_declaration` lit l'élément `0xCD` sans contrôler la longueur : les lignes viennent du compilateur ou d'une image RCI-S dont les longueurs d'éléments sont vérifiées (`bibliotheque.c:690-694`).
- `programme_chercher` sur 255 lignes : 6 itérations de dichotomie de plus que dans la mesure (3 lignes), quelques centièmes de milliseconde.
- ENCODER qui n'ajoute aucune ligne rend CONTINUER impossible (essai D7, `moniteur.c:1387-1389`) : conforme à IMP-0261 (« le programme courant passe à l'état aucun ») ; signalé au point à arbitrer 4.
- Remplacement d'une ligne pendant une suspension alors qu'une chaîne en ligne de la pile désigne son bloc : limite documentée (État d'exécution §12, Programme courant IMP-0207).
- EFFACER de la ligne de la PAUSE puis CONTINUER : `ERREUR (EX.) 75`, réponse cohérente (EFFACER ne compte pas comme ligne entrée, IMP-0207).

---

## 5. Pistes hors périmètre

- `src/noyau/compilateur.c:588-803` : récursion des expressions, 238 octets de pile par niveau de parenthèses ; cause principale de G3-A-02.
- `src/noyau/donnees.c:451-456` (`puissance_dix`), `donnees.c:491-530` (`donnees_ecrire_nombre` : boucles `x / 10.0` pour la forme E) : même classe de coût que G3-P-01 ; `corps[320]` (`donnees.c:497`) et `t[320]` (`donnees.c:466`) sur la pile.
- `src/noyau/interprete.c:578-583`, `1184-1187` : état de lecture statique partagé entre programme et machine de bureau (G3-A-03) ; `interprete.h:41-42` ne s'accorde pas avec IMP-0214 (G3-A-04).
- `src/noyau/contexte_exec.c:403-410`, `433-445` : cadres abandonnés avant l'allocation du cadre racine (G3-A-05).
- `src/noyau/contexte_exec.c:500` et `interprete.c:1215` : `programme_suivante` puis `programme_chercher` pour la même ligne, 0,067 ms par ligne (≈ 1 % de la boucle (b)).
- `src/plateforme/fichiers_iigs.c:169-204` : trois appels GS/OS par nom listé ; `src/noyau/bibliotheque.c:340-378` : relecture complète du catalogue par nom, tampon `t[16 * CATALOGUE_TAILLE]` sur la pile.
- `src/noyau/bibliotheque.c:731-735` (`reconstruire`) : `objets_resoudre`/`objets_relacher` par ligne chargée (1,1 ms).
- `src/plateforme/console_iigs.c:183-196` : `a_la_ligne` (`moniteur.c:76-81`) produit CR puis CR LF, soit deux `DWrite` par fin de ligne (≈ 0,5 ms de trop par ligne affichée).
- `src/plateforme/main.c` : aucun `#pragma stacksize` (G3-A-02).

---

## 6. Points à arbitrer par Éric

1. **Taille de la pile du programme `LSE`** (G3-A-02) : `#pragma stacksize` dans `main.c` (16 Kio proposés, à essayer sur l'Apple IIGS sous le shell ORCA), réduction des cadres de la récursion du compilateur, ou les deux. Une limite d'imbrication rejetée par une erreur de compilation modifierait le langage et n'est pas proposée.
2. **LIRE en machine de bureau pendant une lecture suspendue** (G3-A-03) : sauvegarder et restaurer l'état de lecture du programme autour de la ligne de bureau, ou refuser ce LIRE. Les sources consultées ne disent rien de ce cas.
3. **Reste d'une ligne tapée après la fin du programme** (G3-A-04) : conservé pour le LIRE suivant de la machine de bureau (comportement actuel), ou oublié à `TERMINER` et sur erreur. Le point L6 n'a été tranché que pour deux LIRE d'un même programme.
4. **ENCODER sans ligne ajoutée** (D7) : IMP-0261 met le programme à l'état « aucun » dans tous les cas, ce qui est plus strict que LSE15 §9-2 (CONTINUER refusé si une ligne a été entrée) ; conserver ou limiter au remplacement.
5. **Usage d'`objets_copier` hors de l'interpréteur** (G3-P-03) : étendre IMP-0243 à la lecture du premier octet d'une ligne insérée.
