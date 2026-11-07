# Mesures de performance sur l'Apple IIGS — 2026-10-25

**Objet :** durées des primitives de l'interpréteur et de boucles `FAIRE`, mesurées sur l'Apple IIGS d'Éric (accélérateur à 16 MHz), pour choisir les options de compilation du profil `cible` (Construction, IMP-0143).

**Instrument :** `tests/plateforme/sonde_performance.c` (programme `PERF`), chronométrage par `clock()` (1/60 s), 500 répétitions par primitive, 200 tours par boucle ; console remplacée par le bouchon (sans consultation du pilote `.CONSOLE`). Sorties relevées sur l'image `build/PERF.po` par AppleCommander.

**Variantes de compilation** (`#pragma` de `src/interfaces/profil.h`, tous modules) :

| Variante | `#pragma debug` | `#pragma optimize` |
|---|---|---|
| C | 41 (bits 0, 3, 5) | aucun |
| R0 | 0 | aucun |
| R1 | 0 | 7 (bits 0, 1, 2) |
| R2 | 0 | 15 (bits 0 à 3) |
| R3 | 1 (bit 0) | 7 (bits 0, 1, 2) |

**Résultats (ms par opération ou par tour) :**

| Mesure | C | R0 | R1 | R2 | R3 |
|---|---|---|---|---|---|
| objets_resoudre + objets_relacher | 4,033 | 1,167 | 1,100 | 1,200 | 1,133 |
| programme_chercher | 0,733 | 0,100 | 0,067 | 0,100 | 0,067 |
| programme_suivante | 1,000 | 0,133 | 0,100 | 0,100 | 0,100 |
| pile_empiler_nombre + pile_consommer | 1,500 | 0,400 | 0,367 | 0,400 | 0,400 |
| contexte_lire_nombre | 1,033 | 0,500 | 0,067 | 0,033 | 0,067 |
| contexte_affecter_nombre | 1,300 | 0,800 | 0,333 | 0,333 | 0,333 |
| contexte_resoudre | 1,167 | 0,100 | 0,100 | 0,100 | 0,133 |
| conversion int -> float | 0,467 | 0,467 | 0,467 | 0,467 | 0,500 |
| comparaison float | 0,667 | 0,667 | 0,667 | 0,700 | 0,667 |
| (a) FAIRE 10 vide, par tour | 19,333 | 7,750 | 3,917 | 3,917 | 4,167 |
| (b) FAIRE 20, X_1, par tour | 54,750 | 16,167 | 9,667 | 9,833 | 10,333 |
| (d) FAIRE 20, X_I+1, par tour | 71,833 | 22,000 | 13,583 | 14,000 | 14,583 |

Boucles : (a) `10 FAIRE 10 POUR I_1 JUSQUA 200` ; (b) `10 FAIRE 20 POUR I_1 JUSQUA 200`, `20 X_1` ; (d) même boucle, `20 X_I+1`.

**Autres mesures (sonde `SONDE`, même machine) :** lecture sans attente du pilote `.CONSOLE` 0,517 ms ; lecture du registre du clavier `$C000` 0,0167 ms ; sinus en `double` 13,4 ms ; addition de `float` 0,767 ms ; conversion `float` → `double` 0,883 ms ; comparaison de `double` 0,450 ms ; addition de `long` inférieure à la résolution.

**Mesures en L.S.E. (programme `LSE`, profil C, `TEM()`, 200 tours) :** (a) 4 s ; (b) 13 s ; `X_I` 14 s ; `X_I+1` 17 s ; `X_SIN(I)` 19 s.

**Après la révision d'IMP-0143 (profil `cible` en R3), programme `LSE`, `TEM()`, 200 tours :** (b) 2 s au lieu de 13 s (mesure d'Éric du 2026-10-25).

**Après IMP-0242 (`$C000`) et IMP-0243 (`objets_copier`), profil R3, sonde `PERF` :** `objets_copier` d'une ligne 0,133 ms (contre 1,200 ms pour `objets_resoudre` + `objets_relacher`) ; (a) 4,333 ms ; (b) 8,583 ms ; (d) 12,833 ms par tour. Programme `LSE`, `TEM()`, boucle (b) de 1000 tours : 8 s, soit 8 ms par tour, consultation d'ESC comprise (mesure d'Éric du 2026-10-25), contre environ 65 ms au départ.

**Après IMP-0244, première écriture (sonde `PERF`, `P2.OUT` et `P3.OUT`) :** (a) 4,5 à 4,75 ms ; (b) 9,6 ms ; (d) 12,3 ms par tour, sans gain ; `LSE`, boucle (b) de 1000 tours : 9 s. Détail : `donnees_comparer_nombres` 0,100 ms (contre 0,700 ms pour une comparaison SANE) ; `pile_empiler_nombre_exact` et `pile_consommer` 0,133 ms (contre 0,400 ms par valeur) ; `donnees_opposer` 0,033 ms ; mais `donnees_nombre_entier(200)` 0,933 ms et `donnees_operation_entiere` (5 + 7) 1,533 ms, plus lents que SANE : un décalage de `long` d'une quantité variable coûte 0,033 ms (boucle d'ORCA/C), et la recherche du bit de poids fort en faisait jusqu'à 23. Correction : dichotomie sur une table de puissances de deux, un seul décalage par conversion.

**Après correction d'IMP-0244 (sonde `PERF`, `P4.OUT`) :** `donnees_nombre_entier(200)` 0,167 ms ; `donnees_operation_entiere` (5 + 7) 0,567 ms ; `donnees_comparer_nombres` 0,100 ms ; (a) 2,917 ms ; (b) 7,083 ms ; (d) 8,917 ms par tour, soit −33 %, −17 % et −30 % par rapport à `P.OUT` (avant IMP-0244).

**Programme `LSE` après IMP-0244, `TEM()`, boucle (b) de 1000 tours :** 7 s (mesure d'Éric du 2026-10-25), contre 13 s pour 200 tours au départ, soit environ 9 fois plus rapide.

**Après l'extension d'IMP-0243 (audit du code, point 8 ; sonde `PERF`, sortie du 2026-10-30 versée dans `Audit_code_2026-10-29_essais/point8/PERF_2026-10-30.OUT`) :** `objets_resoudre` + `objets_relacher` 0,467 ms au lieu de 1,133 ms (−59 %), soit deux appels toolbox au lieu de six, environ 0,17 ms par appel ; `objets_copier` d'une ligne 0,133 ms ; (a) 3,000 ms, (b) 7,000 ms, (d) 9,000 ms par tour, égaux à `P4.OUT` à la résolution près (1/60 s pour 200 tours, soit 0,083 ms par tour), ces boucles n'employant aucune résolution. Les accès aux tableaux et aux chaînes ne sont pas mesurés par la sonde.

**Format étendu d'IMP-0216 (audit du code, point 9 ; sonde `PERF`, sortie du 2026-10-30 à 22 h 14, `Audit_code_2026-10-29_essais/point9/PERF_2026-10-30_soir.OUT`) :** dans le même essai, `1.5/3` coûte 3,067 ms par l'ancien calcul en `double` et 1,200 ms par `donnees_operation_etendue` (−61 %) ; `0.1+0.2`, 2,533 ms et 1,033 ms (−59 %). Boucle (e) `X_I/3` : 10,000 ms par tour. Les boucles (a), (b), (d) et des primitives que les points 9 et 10 ne touchent pas (`objets_copier`, décalages) y sont environ 5 % plus lentes qu'à 20 h 25 : la machine a tourné un peu plus lentement pendant cet essai ; les comparaisons faites dans un même essai restent valables.

**Après les lots du soir du 2026-10-30 (AP-02 a, AP-03, AP-06, AP-07, AP-09, AP-10, AP-14, AC-35 ; sonde `PERF`, sortie de 23 h 18, `Audit_code_2026-10-29_essais/point17/PERF_2026-10-30_nuit.OUT`) :** (a) 2,250 ms, (b) 4,417 ms, (d) 6,083 ms, (e) 7,417 ms par tour, contre 3,000, 7,000, 9,000 ms à 20 h 25 et 3,417, 7,333, 9,417, 10,000 ms à 22 h 14 : effet de l'affectation et du pas de FAIRE par adresse (AP-07) et des constantes immédiates converties une fois (AP-10), auquel l'écart de vitesse de la machine d'un essai à l'autre peut prendre part. La boucle (b) est passée de 7,1 ms (`P4.OUT`) à 4,4 ms par tour au cours de l'audit. Fichiers, sur la disquette `/LSE/` (`/LSE/BIBPERF`) : `TABLEAU T[20000]` seul 17 ms ; le même suivi d'un GARER de 80 Ko, 10,2 s ; CHARGER du même tableau, 5,4 s.

**Après l'écriture de GARER par blocs (AP-17 ; sonde `PERF`) :**
- depuis la disquette `/LSE/` (sortie de 23 h 52, `Audit_code_2026-10-29_essais/point17/PERF_2026-10-31.OUT`) : GARER du tableau de 80 Ko 9,0 s au lieu de 10,2 s (−12 %) ; CHARGER 5,4 s ; boucles (a) 2,333, (b) 4,667, (d) 6,250, (e) 7,500 ms par tour, à 2 à 5 % des valeurs de 23 h 18 (écart d'un essai à l'autre) ;
- depuis le disque dur de la machine (sortie de 0 h 23, `PERF_2026-10-31_disque_dur.OUT`) : GARER 1,48 s, CHARGER 0,82 s ; boucles (a) 2,333, (b) 4,667, (d) 6,333, (e) 7,583 ms. Sur la disquette, le support domine les fichiers, six fois plus lents ; sur le disque dur, GARER prend encore 1,8 fois le temps de CHARGER (0,67 s de plus).
- depuis le disque dur, remplacements de même taille (sortie de 0 h 30, `PERF_2026-10-31_remplacement.OUT`) : premier GARER, fichier créé, 1,35 s ; CHARGER 0,83 s ; remplacement qui agrandit le fichier (324 à 647 secteurs) 1,17 s ; remplacement dans l'étendue libérée, sans agrandir, 1,10 s. L'allocation à l'agrandissement ne coûte qu'environ 70 ms, la création du fichier environ 0,25 s ; sans agrandir, GARER reste 0,27 s plus lent que CHARGER (environ 73 Ko/s en écriture, 98 Ko/s en lecture), écart propre à l'écriture sur le disque. La stdio n'y ajoute qu'un repositionnement par écriture de 4 Ko, de l'ordre de 20 ms en tout : l'abandonner ne rapporterait presque rien.

**Conclusions :**
- le code de débogage (`debug 41`) multiplie la durée par 3,3 environ ; `optimize 7` la divise encore par 1,7 environ ; R1 est cinq fois plus rapide que C ;
- R2 n'apporte rien sur R1 et retire la réparation de pile : écartée ;
- le contrôle de débordement de pile (`debug 1`, R3) coûte environ 7 % sur R1 ;
- retenu : R3 (choix d'Éric du 2026-10-25), IMP-0143 ;
- chemin exact sans SANE (IMP-0244) : −17 à −33 % sur les boucles, après remplacement des décalages variables par une table ; au total, la boucle (b) passe de 54,8 ms (profil C) à 7,1 ms par tour ;
- coûts restants notables en R3 : `HLock` et `HUnlock` au chargement d'une ligne (1,1 ms), opérations SANE (0,5 à 0,9 ms), consultation du pilote pour ESC (0,5 ms par ligne).
