# Audit de code LSE-IIGS — groupe 2 (compilateur, analyseur, RCI-M, symboles)

Date : 2026-10-29. Périmètre : `src/noyau/compilateur.c` (+ .h), `src/noyau/analyseur.c` (+ .h), `src/noyau/rcim.c` (+ .h), `src/noyau/symboles.c` (+ .h). Aucun fichier du dépôt n'a été modifié ; les essais sont dans la copie de travail `S/copie-g2/` (S = bloc-notes de la session).

Instruments employés :
- **Peinture de pile** (`S/copie-g2/tests/composants/sonde_pile_compilateur.c`, pile portée à 16 Kio pour l'essai) : la zone sous le pointeur de pile est remplie d'un motif avant `compilateur_compiler`, puis l'on cherche l'octet le plus bas modifié. La mesure couvre les fonctions de bibliothèque et les outils. Sortie : `S/essai-g2-pile.txt`.
- **Pile de 4 Kio imposée** (`#pragma stacksize 4096`, ce que le manuel ORCA/C, `stacksize`, donne pour le shell ORCA) :
  - dans la sonde `S/copie-g2/tests/composants/sonde_pile_4k.c` ;
  - dans le programme `LSE` lui-même, construit dans la copie avec `cc=-dPILE_4096=1` (`S/copie-g2/src/plateforme/main.c`), profil `cible`, et alimenté par l'entrée standard sous Golden Gate.
- **Cycles 65816** : `iix --cycles` sur `S/copie-g2/tests/plateforme/sonde_cycles.c`.
  - Étalonnage sur deux opérations en C pur chronométrées sur l'Apple IIGS (Mesures_performance, `P4.OUT`) : `donnees_operation_entiere` = 6 485 cycles ↔ 0,567 ms (11 400 cycles/ms) ; `donnees_nombre_entier(200)` = 2 076 cycles ↔ 0,167 ms (12 400 cycles/ms). On retient **12 000 cycles ≈ 1 ms de la machine d'Éric**, à ±10 %.
  - Golden Gate exécute les outils (Memory Manager, SANE) en code natif : leurs cycles ne sont **pas** comptés. On les ajoute à part, à partir des mesures sur l'Apple IIGS.
- **Profil par fonction** : trace `iix --trace-cpu --trace-debug-names`, avec un exécutable compilé sous `#pragma debug 0x8001` + `optimize 7` (noms en ligne). Elle est agrégée par `S/outils/profil_trace.py` (sortie `S/profil_k120.txt`, 120 premières lignes du corpus INRDP75). Les appels d'outils sont comptés par `iix --trace-tools` et `S/outils/outils_par_fonction.py`.
- **Variantes optimisées** (copie seulement), sous `#ifdef AUDIT_OPT`, `AUDIT_OPT2` et `AUDIT_OPT3` dans `S/copie-g2/src/noyau/compilateur.c` et `symboles.c`. Avec `AUDIT_OPT2`, profil `cible`, les suites `composants_symboles`, `composants_compilateur_2a`, `2b`, `2c`, `2d`, `t1600`, `limites`, `historiques_erreurs`, `composants_decompilateur`, `historiques_decompilation` et `composants_programme` passent toutes (0 échec) : les optimisations proposées ne changent aucune séquence produite ni aucun diagnostic de ces suites.

---

## 1. Carte des chemins chauds

| Appelant | Appelé | Fréquence |
|---|---|---|
| `moniteur.c:1520` `ligne_programme`, `moniteur.c:1544` `ligne_bureau` | `compilateur_compiler` (`compilateur.c:1804`) | une fois par ligne tapée ou reçue du ruban (RUBAN passe par `lire_entree`) |
| `moniteur.c:1399` `encoder` (ENCODER : texte → programme) | `compilateur_compiler` | une fois par ligne du fichier, jusqu'à 255 lignes (borne du profil) |
| `bibliotheque.c:723` `reconstruire` (APPELER, CHARGER d'un programme) | `symboles_chercher_ou_creer` (`symboles.c:49`) | une fois par symbole du fichier, table vide au départ : O(n²) |
| `bibliotheque.c:731` | `constantes_chercher_ou_creer` (`symboles.c:121`) | une fois par constante partagée : O(n²) |
| `bibliotheque.c:697` (vérification RCI-S), `interprete.c:1296`, `decompilateur.c:1045` et `:1053`, `contexte_exec.c:717` | `rcim_longueur_element` (`rcim.c:249`), `rcim_sequence_valide` (`rcim.c:365`) | une fois par élément RCI-M parcouru |
| `moniteur.c:1643` et `:1676` | `analyseur_classer` (`analyseur.c:89`) | une fois par entrée validée |

Dans `compilateur_compiler`, pour une ligne du corpus (4,9 mots ou identificateurs et 1,47 constante numérique par ligne en moyenne sur les 912 lignes) :

- **par caractère examiné** : `car`/`car_a` (`compilateur.c:82`, `:87`), `est_lettre`/`est_chiffre` (`:92`, `:93`), `sauter_espaces` (`:96`). Ce sont autant d'appels de fonction, chacun suivi d'un appel au contrôle de pile d'ORCA/C (`debug 1`). Sur 120 lignes : 7 893 appels de `car`, 9 363 de `car_a`, 8 357 de `est_lettre`.
- **par mot ou identificateur** :
  - `longueur_mot` (`:103`) ;
  - `mot_reserve` (`:144`) → `dans_liste` (`:131`) → `mot_est` (`:113`), soit `strlen` + `memcmp` sur 27 + 2 mots ; `mot_reserve` est appelé deux fois pour la cible d'une affectation (`:1759` puis `:397`) ;
  - `chercher_fonction` (`:461`), 26 noms, pour chaque identificateur en position d'opérande ;
  - `symbole` (`:175`) → `symboles_chercher` (`symboles.c:36`), recherche linéaire avec un appel de `meme_nom` par entrée ;
- **par instruction introduite par un mot** : `instruction_mot_cle` (`:1608`) essaie en série jusqu'à 17 `prendre_mot`/`voir_mot`, et chacun relit le mot entier (`longueur_mot` + `mot_est`). AFFICHER, l'instruction la plus fréquente, est trouvée au 9ᵉ essai.
- **par constante numérique** :
  - `constante_numerique` (`:208`) → `strtod` ;
  - `ecrire_nombre` (`:191`), avec ses conversions et comparaisons en `double`/`float` ;
  - en plus, le repli `terme_signe` (`:739`) et `cible_ligne` (`:1013`) ;
  - en tout, **24 appels SANE par constante entière** (`--trace-tools`, « 1 A_1 » : 23 appels FP816 + 1 Str2Dec), environ 39 pour `ALLER EN 20` et 45 pour `A_-3`.
- **par chaîne longue de ligne de programme** : `constantes_chercher_ou_creer` → `meme_texte` (`symboles.c:107`), avec `objets_resoudre` + `objets_relacher` (1,1 ms) pour chaque constante de même longueur.
- **par ligne acceptée** : `objets_creer` + `objets_resoudre` + `objets_relacher` (`compilateur.c:1867-1872`), soit environ 1,4 paire `HLock`/`HUnlock` par compilation mesurée sous `--trace-tools`.

**Profil mesuré** (120 lignes du corpus, instructions exécutées sous `compilateur_compiler`, `S/profil_k120.txt`) :

| Part | Fonctions |
|---|---|
| 14,4 % (exclusif) | contrôle de pile d'ORCA/C, appelé à l'entrée de chaque fonction |
| ≈ 36 % (exclusif) | `car`, `car_a`, `est_lettre`, `est_chiffre`, `longueur_mot`, `sauter_espaces` |
| ≈ 31 % (exclusif) | `mot_est` + `strlen` + `memcmp` + `dans_liste` |
| 28,9 % (inclusif) | `prendre_mot` |
| 22 % (inclusif) | `mot_reserve` |
| 10,4 % (inclusif) | `chercher_fonction` |

Le compilateur passe donc la majeure partie de son temps à reconnaître des mots, non à générer.

**Coût mesuré d'une ligne** (profil `cible`, code actuel) :

| Charge | Cycles par ligne | Temps CPU |
|---|---|---|
| corpus, tables vidées par programme | 273 900 | ≈ 22,8 ms |
| corpus, table de 136 symboles et 59 constantes | 297 900 | ≈ 24,8 ms |
| programme synthétique `n X_Y+Z*W`, 20 symboles | 230 900 | ≈ 19,2 ms |
| programme synthétique `n X_Y+Z*W`, 150 symboles | 359 000 | ≈ 29,9 ms |

À ces temps CPU s'ajoutent, sur la machine :
- SANE, environ 35 appels par ligne du corpus × 0,2 à 0,3 ms ≈ 7 à 10 ms ; c'est une estimation, Golden Gate ne compte pas SANE ;
- environ 1 à 2 ms d'outils Memory Manager.

Total estimé : **≈ 32 à 35 ms par ligne** sur l'Apple IIGS d'Éric. C'est imperceptible pour une ligne tapée. ENCODER d'un programme de 255 lignes coûte ≈ 8 à 9 s de compilation, lecture du fichier et `programme_inserer` non compris.

## 2. Budget de pile

**Locales de plus de 64 octets (groupe 2)**, estimées d'après le code ; le cadre de `corps_afficher` est confirmé par la mesure (?B − A_B = 899 octets) :

| Fonction | Locales notables | Taille du cadre |
|---|---|---|
| `corps_afficher` (`compilateur.c:1256`) | `specs[40]` (11 o × 40 = 440), `debuts[40]` (80), `texte[79]`, `brouillon[255]` | ≈ 890 o |
| `compilateur_compiler` (`:1804`) | `octets[255]`, `Analyse`, `TamponRCIM` | ≈ 300 o |
| `declaration_procedure` (`:1506`) | `parametres[32]`, `locaux[32]`, `est_proc[32]` | ≈ 175 o |
| `constante_numerique` (`:208`) | `tampon[80]` | ≈ 92 o |
| `constante_chaine` (`:242`) | `texte[79]` | ≈ 92 o |

`analyseur.c`, `rcim.c` et `symboles.c` n'ont aucune locale de plus de 64 octets.

**Hors groupe, sur le même chemin d'appel** : `moniteur.c:1432` `commande` (`t[256]`), `moniteur.c:1355` `encoder` (`m[100]`, `nom[8]`…), `moniteur.c:1511` `ligne_programme` et `:1537` `ligne_bureau` (`t[100]`).

**Récursions du compilateur**, sans aucune borne codée ; la seule limite est la ligne de 79 caractères (Compilateur §2) :

| Récursion | Chaîne d'appels | Mesure (profil `cible`) |
|---|---|---|
| parenthèses d'expression | `expression` → `concatenation` → `somme` → `terme_signe` → `terme` → `puissance` → `atome` → `expression`, 7 cadres pour 2 caractères | **238 o par niveau** |
| fonctions imbriquées (`ABS(`) | idem + `appel_fonction` | 284 o par niveau |
| indices imbriqués (`A[`) | idem + `reference_variable` + `indices` | 302 o par niveau |
| conditions entre parenthèses | `facteur_logique` → `ou_logique` → `et_logique` | 62 o par niveau |
| SI…ALORS…SINON en expression, SI et DEBUT en instruction | — | quelques niveaux seulement en 79 caractères |

Le retour en arrière de `facteur_logique` (`:874`) peut redoubler le travail par niveau de « SI » en expression placé entre parenthèses. En 79 caractères, cela fait au plus environ 2³ ; c'est sans conséquence.

**Profondeurs mesurées** (octets consommés sous l'appelant de `compilateur_compiler`, `S/essai-g2-pile.txt`) :

| Ligne | Pile (o) |
|---|---|
| `A_B` | 847 |
| `A_1` | 897 |
| `10 PROCEDURE &P(A,B) LOCAL C` | 658 |
| `?B` | 1 746 |
| `10 AFFICHER 1` | 1 830 |
| `A_` + 12 parenthèses | 3 753 |
| `?` + 8 parenthèses | 3 700 |
| `A_` + 38 parenthèses (79 car.) | 9 941 |
| `?` + 38 parenthèses (78 car.) | **10 840** |
| `B_` + 24 indices imbriqués | 8 145 |
| `B_` + 15 `ABS(` imbriqués | 5 157 |

**Pile disponible sur l'Apple IIGS.** Le shell ORCA donne 4 096 octets ; le dépôt n'a pas de `#pragma stacksize`. ORCA/C y loge en plus la page directe de SANE (manuel ORCA/C, `stacksize`). Son contrôle de débordement (`debug` bit 0) exige, à l'entrée de chaque fonction, les locales plus 200 octets de marge (`sbc #00c8` relevé dans la trace). Il reste donc environ 3,4 Kio pour `compilateur_compiler` sur le chemin `ligne_programme`/`ligne_bureau`, et environ 400 octets de moins sur le chemin ENCODER (`commande` + `encoder`). Voir G2-A-01.

---

## 3. Constats

### G2-A-01 — Débordement de pile du 65816 par l'imbrication d'une seule ligne : « Stack overflow » et fin brutale de LSE
- **Gravité :** bloquant (plantage : la session LSE se termine et le programme courant non rangé est perdu).
- **Statut :** CONFIRMÉ par essai, dans le programme `LSE` complet (profil `cible`, pile de 4 096 octets imposée comme dans le shell ORCA), sous Golden Gate. La pile par défaut de Golden Gate est plus grande et masque le défaut : sans `stacksize 4096`, les 38 niveaux passent (`sonde_pile_4k`, SP initial $47E5). L'essai sur l'Apple IIGS lui-même reste à faire (ligne proposée ci-dessous).
- **Emplacement :** descente récursive `compilateur.c:588-830` (`atome` … `expression`), `facteur_logique` `:874`, `indices` `:368`, `appel_fonction` `:472` ; grands cadres `corps_afficher` `:1256-1269` et `compilateur_compiler` `:1808` ; aucun `#pragma stacksize` dans `src/plateforme/main.c`.
- **Constat :**
  - La profondeur de pile croît de 238 à 302 octets par niveau d'imbrication, sur une base de 850 à 1 830 octets.
  - Compilateur §2 affirme que la longueur de la ligne « borne mécaniquement » la récursion « sans qu'aucune limite explicite doive être codée en dur ». La borne mécanique existe (10,8 Kio au pire), mais elle dépasse de loin les 4 Kio de pile.
  - §18 laissait la mesure « à effectuer » ; la voici, et elle infirme l'hypothèse de §2.
  - Le contrôle de pile d'ORCA/C (`debug 1`, IMP-0143) transforme la corruption en arrêt propre, mais l'arrêt reste un arrêt du programme (manuel ORCA/C : erreur 15 « Stack overflow », puis `SystemErrorLocation` « shuts down the program »).
  - Une fonction de bibliothèque ou un outil (`strtod` → SANE, `realloc` → Memory Manager) appelé avec moins de 200 octets de marge peut encore déborder sans contrôle (manuel ORCA/C, `debug` bit 0 : « It is still possible to overflow the stack … by calling a tool »).
- **Scénario :** sous `LSE` (pile de 4 Kio), après `BO` :

  | Ligne saisie | Résultat |
  |---|---|
  | `?(((((((1)))))))` (7 niveaux) | affiche 1 |
  | `?((((((((1))))))))` (8 niveaux, 18 caractères) | **« Stack overflow »**, fin du programme |
  | `20 AFFICHER (((((((1)))))))` (7 niveaux) | accepté |
  | `20 AFFICHER ((((((((1))))))))` (8 niveaux) | « Stack overflow » |
  | `20 A_(((((((((((1)))))))))))` (11 niveaux) | accepté |
  | 12 niveaux | « Stack overflow » |
  | `20 AFFICHER ENT(ABS(SIN(COS(EXP(LGN(1))))))` (6 fonctions) | accepté |

  Par le calcul, la marge est d'environ 400 octets de moins par ENCODER (cadres de `commande` et `encoder`), soit environ 6 niveaux sous AFFICHER pour une ligne d'un fichier décodé. Le corpus historique ne dépasse pas 3 niveaux (outil `S/outils/profondeur.py` : 502 lignes à 0, 386 à 1, 23 à 2, 1 à 3) : le défaut ne touche aucune ligne attestée, mais une ligne valide de 18 caractères suffit à le déclencher.
- **Référence :** Compilateur §2, §18 (dernier point) ; IMP-0143 ; IMP-0165 (tampon sur la pile) ; Architecture physique IMP-0134 ; manuel ORCA/C, `#pragma stacksize` et `#pragma debug` bit 0 ; table des erreurs d'exécution, erreur 15.
- **Proposition de correction**, en trois volets cumulables :
  - (a) `#pragma stacksize 16384` dans `src/plateforme/main.c`, seul `.root`. Le pire cas du compilateur (≈ 10,9 Kio) plus le moniteur (≈ 0,7 Kio), la page directe de SANE et la marge de contrôle tiennent dans 16 Kio. Le manuel admet couramment 32 Kio en environnement texte. Cela coûte 12 Kio de banque 0, à vérifier sur l'Apple IIGS : le chargement doit réussir sous le shell ORCA. Il faut aussi vérifier le besoin de pile de l'interpréteur (autre groupe), qui partage cette pile.
  - (b) Réduire les grands cadres. Rendre statiques `specs`, `debuts`, `texte` et `brouillon` de `corps_afficher` ôte environ 880 octets à toute ligne AFFICHER ou `?`. `corps_afficher` n'est jamais réentrant, puisqu'une compilation n'en appelle pas une autre. Mais IMP-0165 a écarté le tampon statique au nom d'une réentrance future : le volet relève donc d'une décision.
  - (c) Ajouter une borne explicite d'imbrication, avec un compteur dans `Analyse` incrémenté par `expression` et `facteur_logique`, au-delà de laquelle la ligne reçoit C 107 « ligne trop complexe ». La borne doit être calculée avec la pile retenue. Ce volet change l'acceptation de lignes valides : à arbitrer.

  Recommandation : (a) au minimum, essai sur l'Apple IIGS avec `?((((((((1))))))))`, puis avec 38 niveaux ; (c) seulement si la pile ne peut pas être agrandie.
- **Décision IMP nécessaire :** oui. Pour (a), nouvelle décision ou révision d'IMP-0134 et de Construction, avec mise à jour de Compilateur §2 et §18. Pour (b), révision d'IMP-0165. Pour (c), nouvelle décision.
- **Essai :**
  - `S/copie-g2/tests/composants/sonde_pile_compilateur.c` (sortie `S/essai-g2-pile.txt`) ;
  - `S/copie-g2/tests/composants/sonde_pile_4k.c` (avec `PILE_4096` : `A_` + 13 et `?` + 9 → « Stack overflow ») ;
  - `S/copie-g2/src/plateforme/main.c` (`PILE_4096`) + `build/LSE`, entrée `printf "BO\r?((((((((1))))))))\rAU\r" | iix LSE`.

### G2-A-02 — Contrat de `compilateur_compiler` : « aucun HandleObjet n'a été créé » est faux pour une constante partagée
- **Gravité :** mineur (documentaire ; aucun effet observable, l'entrée reste inerte et sera libérée par `symboles_vider`).
- **Statut :** CONFIRMÉ par lecture complète du chemin.
  - `constante_chaine` `compilateur.c:297-301` → `constantes_chercher_ou_creer` → `objets_creer` (`symboles.c:140`) a lieu avant la fin de l'analyse.
  - Une erreur ultérieure, par exemple `10 AFFICHER 'TEXTE DE PLUS DE SEIZE',+`, rend 0 alors que l'objet texte de la constante a été créé et reste en table.
- **Emplacement :** `compilateur.h:42-44` ; Compilateur §9, « Diagnostic et atomicité » (« jamais un `HandleObjet` créé »).
- **Constat :** le texte contredit IMP-0181 (Compilateur §7 : « Une entrée créée pour une ligne finalement rejetée reste inerte »), qui crée bien l'objet texte. Le comportement est voulu (IMP-0107, IMP-0181) ; la formulation de §9 et du commentaire d'en-tête ne l'est pas.
- **Scénario :** ligne refusée contenant une chaîne de plus de 16 caractères → `constantes_nombre()` augmente de 1 et un objet du gestionnaire est occupé jusqu'au prochain `symboles_vider`. Une suite de lignes erronées à chaînes distinctes accumule ces objets ; RANGER les écrit dans le fichier (RCI-S, IMP-0246).
- **Référence :** IMP-0107, IMP-0181 ; Programme courant, ligne 147 (entrées inertes).
- **Proposition de correction :** remplacer, dans §9 et `compilateur.h`, « aucun HandleObjet n'a été créé » par « aucun objet compilé ni bloc transitoire n'a été créé ; des entrées de symboles ou de constantes partagées (objet texte compris) peuvent subsister, inertes ».
- **Décision IMP nécessaire :** non (correction rédactionnelle).
- **Essai :** aucun (lecture).

### G2-P-01 — Reconnaissance des mots : appels de fonction par caractère et recherches répétées (≈ 50 % du temps de compilation récupérable)
- **Gravité :** majeur (lenteur perceptible sur ENCODER ou RUBAN d'un programme complet ; imperceptible pour une ligne tapée).
- **Statut :** CONFIRMÉ par la mesure des cycles et la trace profilée (§1), et par l'essai des variantes `AUDIT_OPT`/`AUDIT_OPT2`, suites de non-régression vertes.
- **Emplacement :** `compilateur.c:82-93` (`car`, `car_a`, `est_lettre`, `est_chiffre`), `:113` `mot_est` (`strlen` à chaque comparaison), `:131` `dans_liste`, `:144` `mot_reserve` (appelé deux fois pour une cible d'affectation, `:1759` et `:397`), `:461` `chercher_fonction`, `:317` et `:338` `prendre_mot`/`voir_mot`, `:1608-1700` (essais en série) ; `symboles.c:36-47` (un appel de `meme_nom` par entrée).
- **Constat :** voir le profil au §1 ; le code de génération proprement dit pèse peu.
- **Estimation chiffrée :**
  - Code actuel : 273 900 cycles par ligne du corpus ≈ 22,8 ms CPU ; 359 000 cycles (≈ 30 ms) pour `n X_Y+Z*W` avec 150 symboles.
  - Avec `AUDIT_OPT2` : 127 600 cycles par ligne du corpus (≈ 10,6 ms, **−53 %**) ; 182 900 (−49 %) avec 150 symboles ; 88 400 (−62 %) avec 20 symboles.
  - `AUDIT_OPT2` combine : `car`/`car_a`/`est_lettre`/`est_chiffre` en macros ; premier caractère comparé avant `strlen` dans `mot_est`, `dans_liste`, `prendre_mot`, `voir_mot` ; `chercher_fonction` limitée aux mots de 2 ou 3 lettres ; comparaison en ligne dans `symboles_chercher`.
  - ENCODER de 255 lignes : ≈ 5,8 s de CPU → ≈ 2,7 s, soit un gain d'environ 3 s.
- **Référence :** IMP-0164, IMP-0167, IMP-0182 ; Construction §5 (pas de `p++` sur un pointeur : les macros proposées n'en utilisent pas) ; IMP-0143 (contrôle de pile, appelé à chaque fonction).
- **Proposition de correction :** reprendre les modifications de `AUDIT_OPT2` (`S/copie-g2/src/noyau/compilateur.c`, `symboles.c`, blocs `#ifdef`), en remplaçant les `#ifdef` par le code définitif.
  - Les macros n'évaluent que des expressions sans effet de bord ; vérifié par `grep` : aucun `car(…)` ni `car_a(…)` ne porte de `++` en argument.
  - Les autres tests ne font qu'avancer le rejet quand le premier caractère diffère, ce qui laisse le résultat identique.
  - Variante plus profonde, non mesurée : dans `instruction_mot_cle`, lire le mot une fois et le comparer à la liste, au lieu de 17 `prendre_mot`.
  - Risque : faible. Le comportement est couvert par les 11 suites citées et par l'aller-retour du corpus (867 lignes). Les essais ont été faits en profil `cible` ; une passe en profil `dev` reste à faire.
- **Décision IMP nécessaire :** non (réalisation interne, sans effet sur la représentation ni sur les diagnostics) ; mention souhaitable dans Compilateur §5 bis (« Impact performances »).
- **Essai :** `S/copie-g2/tests/plateforme/sonde_cycles.c`. Modes `k0`, `k1`, `k2` (corpus), `s0:200,N`, `s1:200,N` (synthétique) ; exécutables `build/tests/sonde_cycles_ref` (code actuel) et `sonde_cycles_opt2`. Trace : `S/profil_k120.txt`.

### G2-P-02 — Constantes numériques : 24 appels SANE par constante entière
- **Gravité :** mineur (≈ 7 à 10 ms par ligne ; ≈ 2 à 2,5 s sur ENCODER de 255 lignes).
- **Statut :**
  - CONFIRMÉ pour le nombre d'appels : `iix --trace-tools`, 100 compilations de « 1 A_1 » → 23 appels FP816 + 1 Str2Dec par constante ; « 1 ALLER EN 20 » ≈ 39 ; « 1 A_-3 » ≈ 45.
  - PLAUSIBLE pour la durée : Golden Gate ne compte pas les cycles de SANE. Les 0,2 à 0,3 ms par appel se déduisent des mesures sur l'Apple IIGS (conversion int→float 0,5 ms pour 2 appels, addition float 0,767 ms pour 4, comparaison double 0,45 ms pour 3). La durée sur machine reste à mesurer.
- **Emplacement :** `compilateur.c:191-205` (`ecrire_nombre` : comparaisons en `double`, conversions `float`/`double`/`long`), `:208-238` (`strtod`, `v > FLT_MAX`), `:724-749` (repli en `double`), `:1013-1033` (`cible_ligne`, comparaisons en `double`).
- **Constat :** dans le corpus, 1 322 des 1 337 constantes numériques sont des entiers sans point ni exposant, inférieurs ou égaux à 32 767 (outil de comptage au §3). Toutes passent par `strtod` et une dizaine de conversions et comparaisons en virgule flottante, alors que le sous-format choisi (`rcim_ecrire_entier`) est entier.
- **Estimation chiffrée :** 1,47 constante par ligne × 24 appels × 0,2 à 0,3 ms ≈ 7 à 10 ms par ligne, soit à peu près autant que le CPU restant après G2-P-01.
- **Proposition de correction :** chemin entier exact dans `constante_numerique`. Quand le lexème ne contient que des chiffres et vaut au plus 32 767, accumuler en `long` et écrire par `rcim_ecrire_entier`. Garder dans `InfoExpression` la valeur entière à côté du `double`, pour que `terme_signe` et `cible_ligne` replient sans SANE. Les autres formes conservent le chemin actuel.
  - Résultat : séquences identiques, par construction (même sous-format, même valeur).
  - Gain : ≈ 7 à 10 ms par ligne.
  - Risque : faible à modéré (repli `-0`, valeurs limites, `cible_ligne` à bornes réelles), couvert par les suites 2a à 2d.
- **Décision IMP nécessaire :** non (aucun changement de représentation) ; mention dans Compilateur §5, « Repli sur une constante ».
- **Essai :** `sonde_cycles x "<ligne>"` sous `iix --trace-tools`, profil `cible`.

### G2-P-03 — Reconstruction de la table des symboles à APPELER : recherche quadratique et `realloc` d'une entrée à la fois
- **Gravité :** mineur (≈ 0,3 s pour 100 symboles, 1,1 s pour 200, 4,5 s pour 400).
- **Statut :** CONFIRMÉ par la mesure des cycles. Le mode `rN` reproduit exactement la boucle de `bibliotheque.c:722-727` : `symboles_chercher_ou_creer`, table vide, N noms distincts de 1 ou 2 lettres.
- **Emplacement :** `symboles.c:41-45` (recherche linéaire par appel de `meme_nom`), `symboles.c:67-71` (`realloc` à chaque symbole) ; appelant `bibliotheque.c:723` (hors groupe). Même schéma pour les constantes (`symboles.c:132-136`, `:143`), avec en plus `objets_resoudre`/`objets_relacher` (1,1 ms) dans `meme_texte` pour chaque constante de même longueur.
- **Estimation chiffrée :**

  | Symboles | Code actuel | Comparaison en ligne | + agrandissement par tranches de 32 |
  |---|---|---|---|
  | 50 | 0,96 M cycles ≈ 0,08 s | — | — |
  | 100 | 3,48 M ≈ 0,29 s | — | — |
  | 200 | 13,65 M ≈ 1,14 s | ≈ 0,59 s | 4,34 M ≈ 0,36 s |
  | 400 | 53,8 M ≈ 4,5 s | — | 14,8 M ≈ 1,2 s |

  Pendant la compilation, la même recherche coûte ≈ +10,7 ms par ligne à 150 symboles par rapport à 20 (programme synthétique, 4 identificateurs par ligne).
- **Proposition de correction :**
  - la comparaison en ligne de G2-P-01 ;
  - l'agrandissement par tranches (capacité tenue à côté de `nb_symboles`), mesuré −38 % de plus sur `r200` ;
  - pour APPELER, une primitive `symboles_reserver(n)` suivie d'insertions sans recherche, le bibliothécaire contrôlant l'unicité des noms autrement s'il le faut (la somme de contrôle RCI-S protège déjà contre l'altération accidentelle) : coût ≈ 0.
  - Gain : 0,3 à 4 s par APPELER selon la taille.
  - Risque : faible pour les deux premières mesures ; la troisième retire la détection actuelle des doublons (`id != k`).
- **Décision IMP nécessaire :** non pour les deux premières (réalisation interne d'IMP-0167 et IMP-0132) ; révision mineure d'IMP-0246 si l'on retire la détection des doublons.
- **Essai :** `sonde_cycles r50`, `r100`, `r200`, `r400` avec `sonde_cycles_ref`, `sonde_cycles_opt` (comparaison en ligne) et `sonde_cycles_opt3` (+ tranches de 32).

---

## 4. Points écartés après vérification

- **Débordement du tampon de génération (C 107)** : chaque écriture passe par `ECRIRE` et `place()` (`rcim.c:24`). Une ligne de 79 caractères produit au plus ≈ 160 octets (au plus 2 octets par caractère, par exemple `A+A+…`). Le brouillon d'AFFICHER a la même capacité. Rien n'est écrit au-delà.
- **`texte[79]` de `constante_chaine` et de `corps_afficher`, `tampon[80]` de `constante_numerique`** : la longueur produite est strictement inférieure au nombre de caractères source, lui-même ≤ 79 (contrôle en `compilateur.c:1820`).
- **`parametres[32]`, `locaux[32]`** : le contrôle `n == DECL_MAX` précède l'écriture ; 32 paramètres distincts ne tiennent pas en 79 caractères.
- **`specs[40]`, `debuts[40]`** : contrôles à `compilateur.c:1171` et `:1283` avant l'écriture ; `besoin ≤ nb_expr` borne l'indice `k`.
- **`rcim_longueur_element`** : toute charge utile variable est lue par `LIRE(i)` borné, y compris la déclaration de procédure (`n` recalculé puis comparé à `reste`), et le résultat est rejeté si `n > reste`. Aucune lecture hors de la séquence. `lire_tag` ne contrôle pas la position, mais son contrat suppose `fin_sequence` testé avant.
- **`rcim_ecrire_description`** : répétition 1 à 99 garantie par `format()` ; 63 reste en forme courte et 64 passe en forme étendue, de façon cohérente avec la relecture.
- **Conversions vers 16 bits** :
  - `symboles.c:67` calcule la taille en `size_t` (32 bits) après un `+1` sur 16 bits (≤ 65 535) ;
  - les positions ≤ 79 sont rangées dans un `unsigned char` ;
  - `rcim_corriger` écrit un curseur ≤ 255.
- **`symbole()` rendant 0 sans diagnostic sur `SYMB_ERR_ARGUMENT`** (`compilateur.c:183`) : inatteignable, car tous les appelants vérifient 1 ≤ lg ≤ 5 auparavant.
- **Résolution ouverte à travers un appel système** : `compilateur.c:1870-1872` et `symboles.c:115-117`, `:151-153` ne font que `memcpy`/`memcmp` entre `objets_resoudre` et `objets_relacher`, ce qui est conforme au Gestionnaire d'objets §11.
- **Pollution de la table par le retour en arrière de `facteur_logique`** : la reprise relit le même texte, donc crée les mêmes noms ; seules des entrées inertes subsistent sur une ligne refusée, ce qui est conforme à IMP-0107 et IMP-0181 (voir G2-A-02 pour la formulation).
- **Coût exponentiel du retour en arrière** (`SI` en expression entre parenthèses) : au plus environ 2³ en 79 caractères ; négligeable.
- **`analyseur.c`** :
  - aucune paire de commandes ne partage ses deux premières lettres dans un même état (veille ou session) d'un même profil ; les doublons apparents (SUPPRIMER et SUPPRESSION ID, LISTER et LISTE ID) sont séparés par l'état dormant ;
  - le signe des `char` est sans effet, puisqu'il n'y a ni indexation par caractère ni minuscules ;
  - le double parcours (`analyseur_nom` dans la boucle, 31 × 31) est négligeable, environ 1 ms par entrée ;
  - `objets_creer(0)` pour une ligne vide ou faite d'espaces est traité (`objets.c:189`, IMP-0194).
- **Recherche dans la table des constantes** (`meme_texte`, 1,1 ms par constante de même longueur) : quelques millisecondes par chaîne longue compilée ; marginal. `objets_copier` (IMP-0243) le réduirait si besoin.
- **Contrôle de pile (`debug 1`) = 14,4 % du temps de compilation** : à conserver. G2-A-01 montre que c'est lui qui transforme une corruption de la banque 0 en arrêt propre.
- **`2ET` refusé (C 07) alors que `2OU` est accepté** (`SI A=2ET B=1`) : cela suit littéralement LSE15 §3-1.1 (« E » appartient au lexème numérique) ; ce n'est pas un défaut du code.

## 5. Pistes hors périmètre

- **`src/noyau/objets.c:198-209`** : `objets_creer` parcourt les blocs-maîtres en premier ajustement et verrouille chacun (`plateforme_bloc_verrouiller`, environ 0,5 ms la paire) jusqu'à trouver de la place. Le coût d'une ligne compilée (et d'APPELER, `bibliotheque.c:752`) croît avec le nombre de blocs. On compte 1,4 paire `HLock`/`HUnlock` par compilation sur un petit programme ; non mesuré sur un gros.
- **`src/noyau/moniteur.c:1432`, `:1355`** : `commande` (`t[256]`) et `encoder` (`m[100]`) restent sur la pile pendant toute la compilation ENCODER, soit environ 400 octets de marge en moins (G2-A-01). Ces tampons pourraient être statiques.
- **Interpréteur** : il partage la même pile de 4 Kio ; sa consommation n'a pas été mesurée ici. La méthode de peinture (`sonde_pile_compilateur.c`) s'y applique telle quelle.
- **`src/noyau/bibliotheque.c:690-700`** : la vérification d'un fichier RCI-S ne contrôle que la longueur des éléments, pas les identifiants (< nombre de symboles), les index de constantes (< nombre de constantes) ni les cibles de branchement (≤ longueur). Un fichier altéré qui garde une somme de contrôle valide (somme simple sur 16 bits) passerait jusqu'à l'interpréteur et au décompilateur. PLAUSIBLE ; à examiner par le groupe concerné.
- **`src/noyau/bibliotheque.c:722-731`** : la reconstruction appelle `symboles_chercher_ou_creer` et `constantes_chercher_ou_creer` ; coût chiffré en G2-P-03.

## 6. Points à arbitrer par Éric

1. **Pile (G2-A-01)** : agrandir la pile par `#pragma stacksize` (taille à fixer ; 16 Kio proposés, à essayer sur l'Apple IIGS sous le shell ORCA), ou imposer une limite d'imbrication explicite (diagnostic C 107, ce qui change l'acceptation de lignes valides), ou les deux. Faut-il en outre alléger `corps_afficher` par des tableaux statiques, contre la justification de réentrance d'IMP-0165 ?
2. **Rédaction de Compilateur §2 et §18** : la borne « mécanique » de §2 est réfutée par la mesure ; le point de §18 est à clore avec les chiffres ci-dessus, une fois la solution retenue.
3. **Optimisations G2-P-01 et G2-P-02** : les admettre maintenant, ou les différer après la v1 (priorité : fiabilité). Elles divisent environ par trois le temps d'ENCODER ou de RUBAN d'un programme complet (≈ 8 à 9 s → ≈ 3 s estimés) sans changer aucune séquence produite.
4. **APPELER (G2-P-03)** : faut-il garder la détection des noms en double dans un fichier RCI-S (`id != k`), qui impose aujourd'hui la recherche quadratique, ou la confier à une autre vérification ?
