# Audit de conformité ciblé — RCI-M (plages entre sources, notation, couverture)

**Date :** 2026-10-22
**Document audité :** [RCI-M_Specification_physique.md](../tome-v/RCI-M_Specification_physique.md), dans son état du 2026-10-22, après le traitement de l'audit du 2026-10-21 ([Audit_RCI-M_Compilateur_2026-10-21.md](Audit_RCI-M_Compilateur_2026-10-21.md)).
**Sources confrontées :** INRDP75 (texte et récapitulatif final), LSE15, `LSE15Mitra.g4` (grammaire reconstruite, non source historique), NOYELLE89 (consulté).
**Origine :** l'audit de conformité initial de RCI-M (projet claude.ai) comptait dix points ; les points 1 et 2 sont clos, mais l'énoncé des points 3 à 10 n'a pas été exporté dans le dépôt. Seuls leurs thèmes sont connus : « ranges EQC contradictoires entre sources, incohérences de notation, couverture manquante ». Le présent audit reprend ces trois thèmes sur le texte actuel et **remplace la liste perdue** comme référence de travail.
**Mode de traitement :** les points marqués « décision » se traitent un par un, proposés d'abord en prose ; les points marqués « correction » sont des corrections de rédaction applicables directement. La table de suivi (§D) reçoit la référence de ce qui referme chaque point.

Les numéros de ligne cités sont ceux de RCI-M à la date de l'audit.

---

## A. Plages et limites : désaccords entre sources

### P1. Plage d'EQC : la justification de la préséance d'INRDP75 n'est pas sourcée — décision

- INRDP75 §6-10 : « La valeur de a doit être non négative et inférieure à 128 (sur T1600) ou 256 (sur M15). »
- LSE15 §4-4.2 (MITRA 15) : « cette valeur doit être non négative et inférieure à 128, sinon une erreur sera détectée à l'exécution ».
- RCI-M (sous-code 0x26, l. 800 environ, et §4.11.1, règle de préséance) retient 0–255 pour le MITRA 15 au nom de la préséance d'INRDP75, en affirmant qu'« une source plus récente » confirme cette plage et que « LSE15 documente vraisemblablement un état du langage antérieur à celui décrit par INRDP75 ».
- **Constat.** La « source plus récente » n'est nommée nulle part. Par ailleurs, INRDP75 a été achevé en décembre 1974 (« La rédaction de ce fascicule a été terminée en décembre 1974 ») et LSE15 est daté de 1978 : l'affirmation qu'il décrit un état antérieur demande une justification (version du système installée à l'École d'Application des Transmissions, par exemple), faute de quoi elle contredit l'ordre chronologique apparent des deux manuels. La règle de préséance elle-même a été appliquée depuis (Compilateur §8 bis, mots réservés) ; c'est sa justification qui doit être sourcée ou reformulée.

### P2. Bornes `e` et `d` des spécifications F et E : désaccord non signalé — décision

- INRDP75 §7-3 : « e et d sont compris entre 0 et 100 ».
- LSE15 §6-2.3 : « avec 0 ≤ e < 100 et 0 ≤ d < 100 ».
- RCI-M (sous-code 0x35, IMP-0115, IMP-0116) : « 1 octet `e` (0–100), 1 octet `d` (0–100) », sans mention du désaccord. La formule d'INRDP75 est elle-même ambiguë quant à l'inclusion de 100.
- **Constat.** La plage retenue doit être justifiée par la règle de préséance, explicitement, ou par une autre raison ; le désaccord doit être signalé à cet endroit, comme il l'est pour EQC.

### P3. Facteur de répétition : plage commune affichée sans la limite du T1600 — correction

- RCI-M (§4.10.1, 0x30–0x34, IMP-0116) énonce une plage de 1 à 99 pour toutes les spécifications, en citant LSE15.
- INRDP75 (note 7 du chapitre « Instruction de sortie. Format ») : « Ce facteur est limité à 62 sur T1600, à 99 sur Mitra 15. » La limite par profil est appliquée par le compilateur (Compilateur §8 bis, IMP-0180), mais RCI-M ne la mentionne pas.
- **Constat.** La représentation couvre les deux profils sans changement ; il manque seulement le renvoi à la limite de 62 du profil T1600.

### P4. Précision des nombres et écriture canonique des réels — décision

- INRDP75 : « Les constructeurs garantissent 6 chiffres significatifs, c'est-à-dire une précision de 10⁻⁶ » ; « Les nombres n'ayant en L.S.E. que 6 chiffres significatifs ».
- LSE15 §6-2.3 (spécification U) : un nombre est affiché avec « les décimales nécessaires pour représenter le nombre avec la précision avec laquelle il est représenté de manière interne » ; notation décimale si « la valeur absolue du nombre est nulle ou comprise entre 10⁻³ inclus et 10⁶ exclu », sinon mantisse et exposant, « cet exposant est toujours signé, et comporte toujours 2 chiffres » (exemples : `-1E-04`, `1.04006E+07`). INRDP75 donne les mêmes seuils (« 10⁻³ ≤ |N| < 10⁶ ») et le même exposant (« signe + ou -, suivi de 2 chiffres »).
- RCI-M IMP-0183 (§4.12) fixe pour la restitution des constantes réelles « au plus sept chiffres significatifs », la notation décimale entre 0,001 et 10 000 000, et un exposant « sans zéro de tête » (`1E-5`), en invoquant un exemple d'affichage à sept chiffres.
- **Constat.** La forme canonique de restitution diverge, sans raison documentée, des conventions d'écriture des nombres attestées par les deux manuels (six chiffres significatifs garantis ; seuils 10⁻³ et 10⁶ ; exposant signé à deux chiffres). Aligner la restitution sur ces conventions rapprocherait le listage de l'affichage historique ; le choix relève toutefois d'une décision, la restitution d'une constante n'étant pas son affichage.

### P5. Arrondi ou troncature des indices de tableau — décision (hors RCI-M, à reporter)

- LSE15 §3-1.2 : « la valeur de l'indice sera prise égale à l'entier le plus voisin de ce résultat » (`RESUL[5.69]` désigne le sixième élément).
- INRDP75 §5 : un indice peut être « une expression arithmétique simple dont l'évaluation donnera la valeur entière d'indice cherchée (après éventuellement suppression de la partie décimale du résultat) ».
- **Constat.** Désaccord factuel entre les deux manuels sur une règle d'exécution (arrondi ou troncature). RCI-M n'est pas concerné (l'indice est une valeur consommée en pile), mais le désaccord n'est consigné nulle part ; il relève du gestionnaire des tableaux (module `donnees`) et, le cas échéant, du profil d'implantation.

---

## B. Notation et renvois

| Réf. | Endroit (RCI-M) | Problème | Nature |
|---|---|---|---|
| N1 | Titres des §4.8 à §4.12 | Niveau `##`, alors que §4.1 à §4.7 sont au niveau `###` sous le §4 : la hiérarchie des titres est rompue | correction |
| N2 | §4.8.4.3, l. 484 | « cas également illustré au §4.8.5.2 pour FAIRE » : le §4.8.5.2 traite du `SI` en position d'expression ; l'illustration de FAIRE suivi d'instructions sur la même ligne est au §4.8.2 | correction |
| N3 | §4.11.1, l. 701 | « (§4.11.5 pour LIRE) » : LIRE est traité au §4.11.6 | correction |
| N4 | IMP-0124, l. 1009 et l. 1014 | « LSE15 §1-3 » : ce paragraphe n'existe pas ; la règle des identificateurs est en LSE15 §3-1.2 | correction |
| N5 | Sous-code 0x2A (CHAINE), l. 826 | « LSE15 §3-3 » : la déclaration CHAINE est en LSE15 §4-3 | correction |
| N6 | IMP-0111, l. 343 | « §7.5 sortie extraordinaire de procédure » : notation du corpus `§7-5`, et source non nommée (LSE15) | correction |
| N7 | §4.8.1, §4.8.2, IMP-0111, IMP-0112 | Sources désignées par « corpus, chapitre 2 — INSTRUCTION ALLER EN », « corpus historique, chapitre 4 (INSTRUCTION FAIRE, §4-1 et §4-2) », sans nommer le manuel (INRDP75) | correction |
| N8 | §4.11.1, §4.11.6 (4 occurrences) | La grammaire reconstruite est désignée tantôt « `SRC-0004` », tantôt « `LSE15Mitra.g4` », tantôt les deux ; le reste du Tome V emploie `LSE15Mitra.g4` | correction |
| N9 | l. 243, l. 709 ; 2 occurrences de « MITRA 15 » contre 26 de « Mitra 15 » | Graphies « T 1600 » et « MITRA 15 » minoritaires, à uniformiser (« T1600 », « Mitra 15 ») | correction |
| N10 | IMP-0125, l. 348 | « `InfoProcedure.numero_ligne_entree` (§6.2/IMP-0114) » : la structure est au §6.2, la décision IMP-0114 au §6.3 | correction |
| N11 | §3 bis, IMP-0109 | « Tomes I-II §HIS-4.7.2 » : un identifiant HIS n'est pas un numéro de paragraphe | correction |

---

## C. Couverture

Le catalogue a été confronté au récapitulatif final d'INRDP75 (« Expressions », « Instructions », « Procédures », « Instructions portant sur les fichiers ») et aux listes de fonctions de LSE15 §3-2.2 et §4-4.2.

**Couverture vérifiée.** Les dix fonctions numériques, les onze fonctions chaînes (signatures et paramètres optionnels identiques dans les deux manuels ; `EQN(ec)` et `EQN(ec,ea)` dans le texte d'INRDP75, le récapitulatif n'en donnant qu'une forme), les six opérateurs de relation (`< = <= > >= #`), `ET` et `OU`, toutes les instructions du récapitulatif (`LIRE`, `AFFICHER` et ses formats, `TERMINER`, `PAUSE`, `ALLER EN`, `SI` sous ses trois formes, `FAIRE` sous ses deux formes, déclaration et appel de procédure, `RETOUR`, `RETOUR EN`, `RESULTAT`, `GARER`, `CHARGER` avec son paramètre optionnel, `EXECUTER ω{,ea}`), les déclarations `CHAINE`, `TABLEAU` et `LIBERER`, et les spécificités T1600 et MITRA 15 d'INRDP75 Annexe III ont chacune une représentation dans RCI-M. La forme `GARER id,ea,'#…'` (fichier permanent, INRDP75 : « le nom du fichier permanent doit être précédé de `#` ») ne requiert aucune représentation particulière : le `#` fait partie de la valeur chaîne du nom de fichier, interprétée à l'exécution par le gestionnaire des fichiers-données. Les parenthèses de `TABLEAU id1(ea1)` dans le récapitulatif d'INRDP75 sont une erreur de transcription : le texte (§5-1) et ses 46 exemples emploient des crochets.

| Réf. | Constat | Nature |
|---|---|---|
| V1 | Les sous-codes de valeur 0x04 et 0x05 (FAUX, VRAI) ne sont produits par aucune construction : le corpus n'atteste aucune constante logique et la grammaire n'en a pas. Comme pour `NON` (0x18, déjà signalé non attesté), le statut de ces sous-codes doit être indiqué (conservés, jamais produits par le compilateur), ou leur usage précisé. | correction |
| V2 | Les tests de représentation (§14) ne couvrent pas les constructions ajoutées ou précisées par la grille de profil : `SH`/`CH`/`TH`/`SGN`, `UN`/`ZER` et `$` n'y ont aucun exemple de séquence (les tests correspondants figurent au chapitre Compilateur). | correction |

---

## D. Suivi

| Point | Objet | Nature | Statut |
|---|---|---|---|
| P1 | Justification de la préséance d'INRDP75 (EQC, « source plus récente ») | Décision | Traité le 2026-10-22 : règle conservée, fondée sur le rôle d'INRDP75 comme source de référence des deux profils définis par les tomes (EXI-0061) ; affirmations non sourcées retirées (« source plus récente », « état antérieur ») ; date de LSE15 (1978) indiquée ; RCI-M §4.11.1, 0x26, §13 et Compilateur §8 bis |
| P2 | Bornes `e`, `d` des spécifications F et E (0–100 contre 0–99) | Décision | Traité le 2026-10-22 : 0 à 99, l'ambiguïté d'INRDP75 (« compris entre 0 et 100 ») étant levée par LSE15 (« 0 ≤ e < 100 ») sans application de la préséance ; RCI-M 0x35 et §4.11.1 ; Compilateur §5 (contrôle et test) |
| P3 | Limite du facteur de répétition à 62 sur T1600, non mentionnée | Correction | Corrigé le 2026-10-22 (§4.10.1, IMP-0116) |
| P4 | Écriture canonique des réels à la restitution (chiffres significatifs, seuils, exposant) | Décision | Traité le 2026-10-22 : IMP-0183 (puce « Constantes numériques ») alignée sur INRDP75 et LSE15 §6-2.3 — six chiffres significatifs, notation décimale entre 10⁻³ inclus et 10⁶ exclu, exposant signé à deux chiffres ; tests du §14 complétés |
| P5 | Arrondi ou troncature des indices de tableau (hors RCI-M) | Décision | Traité le 2026-10-22 : arrondi à l'entier le plus voisin, désaccord INRDP75/LSE15 résolu par recoupement avec deux sources tierces consultées par Éric — LAFOND83 (*Lire LSE, le langage français de programmation, manuel de l'utilisateur*, 1983, même auteur qu'INRDP75 : « arrondie à l'entier le plus voisin », « L'indice nul est interdit ») et LSE 8086 version IV (1988, norme AF Z 65-020 : `RESUL[5.69]` désigne le sixième élément) ; Etat_Execution §3 bis (tableau « Conversion en entier »), §6.1 ; RCI-M §4.11.1 |
| N1 à N11 | Notation et renvois | Correction | Corrigés le 2026-10-22 |
| V1, V2 | Couverture : sous-codes FAUX/VRAI, tests des constructions T1600 | Correction | Corrigés le 2026-10-22 (§4.4, §14) |
