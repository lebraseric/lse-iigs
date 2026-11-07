# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — L'interpréteur : cycle d'exécution, opérations, contrôle

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V rédigé le 2026-10-24 (étapes 5b et 5c de la feuille de route). Décisions IMP-0214 à IMP-0222, sur les points F4, F5, B1 à B6 et G1 à G5 retenus par Éric le 2026-10-24. Le point resté ouvert a été tranché par essai (§9).
**Portée :** réalisation physique du module `src/noyau/interprete.c/.h` (Organisation du dépôt §2.1) : cycle d'interprétation d'une ligne de programme ou de machine de bureau, opérations arithmétiques, relations, logique, affectation, déclarations, fonctions, contrôle (SI, branchements, FAIRE, procédures, TERMINER, PAUSE). L'entrée-sortie sur la console (`AFFICHER`, `LIRE`, `?`) est réalisée au §4 bis, `TEM`, `DAT` et `ALE` au §4 ter ; les fichiers (`GARER`, `CHARGER`, `SUPPRIMER`, `EXECUTER`) relèvent de l'étape « fichiers » (IMP-0219).
**Rang normatif :** subordonné au Tome III (VM-2205, VM-2206, VM-3101, VM-3102, §8), au Tome IV (ALG-1102, IV-IF-0003), et aux chapitres RCI-M, État d'exécution (§7.2, IMP-0144 à IMP-0155, IMP-0179, IMP-0208) et Données (IMP-0209 à IMP-0213). Aucune disposition du présent chapitre ne peut contredire ces sources.

---

## 0. Avertissement de statut

Le cycle d'interprétation est fixé par le chapitre État d'exécution (§7.2, réalisation d'ALG-1102), le sens de chaque élément par RCI-M (§4) et la fiche de codage, les erreurs par le catalogue (Gestionnaire des erreurs §9). Le présent chapitre fixe l'interface, la conduite du cycle sur l'Apple IIGS et les points que ces sources laissent ouverts.

---

## 1. Interface

**IMP-0214** — *Une fonction d'exécution, qui s'arrête sur une fin d'exécution et laisse l'état intact*
- Sujet : comment le moniteur fait-il exécuter le programme courant (IV-IF-0003) ou une ligne de machine de bureau, et comment apprend-il pourquoi l'exécution s'est arrêtée ?
- Choix :

```c
typedef unsigned char FinExecution;
#define FIN_TERMINER  0   /* instruction TERMINER */
#define FIN_PAUSE     1   /* instruction PAUSE ; pos désigne l'élément suivant */
#define FIN_ARRET     2   /* ligne d'arrêt de EXECUTER A PARTIR DE n1,n2 atteinte, non exécutée */
#define FIN_ERREUR    3   /* *diagnostic renseigné ; pos désigne l'élément en cause */
#define FIN_BUREAU    4   /* fin de la ligne de machine de bureau */
#define FIN_PAS       5   /* mode pas à pas : début d'une ligne atteint, non exécutée (Moniteur IMP-0232) */
#define FIN_INTERRUPTION 6 /* ESC vue à l'arrivée sur une ligne (Moniteur IMP-0239) */
#define FIN_LECTURE   7   /* LIRE attend une ligne ; pos désigne l'élément de lecture (Moniteur IMP-0237) */
#define FIN_EXECUTER  8   /* EXECUTER : module chargé, pos en tête de la ligne de départ (Bibliothécaire IMP-0252) */

void         interprete_fournir_saisie(const char *ligne, unsigned int n, unsigned char touche_fin);
FinExecution interprete_executer(PileEvaluation *p, PositionExecution *pos,
                                 DiagnosticExecution *diagnostic);
FinExecution interprete_bureau(PileEvaluation *p, HandleObjet bloc, unsigned int longueur,
                               PositionExecution *pos, DiagnosticExecution *diagnostic);
void         interprete_pas_a_pas(int vrai);
void         interprete_reinitialiser(void);   /* EXECUTER, REPRENDRE : lecture oubliée */
```

  - `interprete_executer` part de la position donnée, normalement celle que rend `contexte_initialiser` (État d'exécution §7.1), et déroule le cycle jusqu'à une fin d'exécution.
  - `interprete_bureau` exécute le bloc transitoire d'une ligne de machine de bureau (RCI-M IMP-0109), en ligne 0, dans le cadre courant, depuis `pos->decalage` ; après `FIN_LECTURE`, le moniteur la rappelle avec la position rendue.
  - `FIN_PAS`, `FIN_INTERRUPTION` et `FIN_ARRET` sont vus à l'arrivée sur une ligne : au début d'une ligne, ou au retour sur la ligne `FAIRE` au début de chaque itération (Moniteur IMP-0232, IMP-0239) ; la ligne ainsi signalée n'est pas signalée de nouveau quand la reprise la fait exécuter.
  - `FIN_LECTURE` : le moniteur lit une ligne, la fournit par `interprete_fournir_saisie` avec sa touche de fin (Moniteur IMP-0237, IMP-0238), puis rappelle `interprete_executer` à la position rendue ; l'état de la lecture en cours est conservé entre les deux appels.
  - Après `FIN_PAUSE`, `FIN_ARRET`, `FIN_ERREUR`, `FIN_PAS`, `FIN_INTERRUPTION` ou `FIN_LECTURE`, la pile, les cadres, les boucles et l'environnement sont laissés tels quels, pour l'observation et la reprise, qui relèvent du moniteur (État d'exécution §7.4).
  - Le diagnostic porte la ligne en cours, ou 0 en mode machine de bureau (IMP-0195).
- Alternatives envisagées : exécuter un élément par appel — écartée : le moniteur n'en a pas besoin avant PAS À PAS, qui pourra ajouter un point d'arrêt sans changer cette interface.
- Justification : Tome IV ALG-1102 ; État d'exécution §7.2, §7.4 ; choix d'Éric du 2026-10-24 (point B1).
- Impact mémoire : nul.
- Impact disque : sans objet.
- Impact performances : nul.
- Impact historique : réalise les quatre causes d'arrêt de LSE15 §9-2 (TERMINER, PAUSE, erreur, ligne d'arrêt).
- Références normatives : Tome IV IV-IF-0003, ALG-1102 ; LSE15 §9-2 ; État d'exécution §7 ; IMP-0153, IMP-0195.
- Statut : retenu (Éric, 2026-10-24, point B1 ; fins `FIN_PAS`, `FIN_INTERRUPTION` et `FIN_LECTURE` ajoutées le 2026-10-24 par le chapitre Moniteur, points K4, K8, L1).

---

## 2. Conduite du cycle

**IMP-0215** — *Copie de la séquence de la ligne en cours dans un tampon local*
- Sujet : le gestionnaire d'objets interdit de garder un objet résolu à travers une création ou une libération d'objet (Gestionnaire d'objets §8, §11) ; or l'exécution d'une ligne crée et libère des objets (chaînes, tableaux). Comment l'interpréteur lit-il la séquence ?
- Choix : à l'entrée dans une ligne, sa séquence (255 octets au plus) est copiée dans un tampon local du cycle ; le bloc est aussitôt relâché. Une chaîne en ligne déposée sur la pile désigne le bloc de la ligne et la position de son texte (État d'exécution §4.2), jamais le tampon. Le programme courant ne change pas pendant une exécution, si bien que la copie reste exacte.
- Alternatives envisagées : résoudre le bloc pour chaque élément — écartée : un verrouillage par élément, sur l'opération la plus fréquente.
- Justification : Gestionnaire d'objets §8 ; RCI-M IMP-0105 (255 octets).
- Impact mémoire : 255 octets de pile C.
- Impact disque : sans objet.
- Impact performances : une copie par ligne entrée.
- Impact historique : neutre.
- Références normatives : Gestionnaire d'objets §8, §11 ; RCI-M IMP-0105 ; État d'exécution §4.2.
- Statut : retenu.

Le cycle suit État d'exécution §7.2. À chaque élément, la position avance d'abord jusqu'à l'élément suivant, puis l'élément est exécuté ; un appel retient ainsi la bonne position de retour. Quand la séquence est épuisée, `contexte_fin_de_ligne` donne la position suivante (reprise d'une boucle, ou ligne suivante). À l'arrivée au début d'une ligne par progression ou par branchement, la ligne d'arrêt programmée est comparée (IMP-0153).

**Abandon d'une expression (choix d'Éric du 2026-10-24, point B2).** Chaque cadre retient la hauteur de la pile à son entrée (sa base) ; entre deux instructions, la pile d'un cadre revient toujours à sa base. Quand un `RETOUR EN` abandonne l'expression appelante (État d'exécution §7.3), la pile est ramenée à la base du cadre appelant, et l'exécution reprend à la ligne cible. Le chapitre État d'exécution (IMP-0146, §7.3) est amendé en conséquence.

---

## 3. Opérations

### 3.1 Arithmétique

**IMP-0216** — *Calcul en double précision, résultat rangé en `float` ; dépassements et cas limites*
- Sujet : comment calculer, et que faire d'un résultat hors du `float` ou d'un cas que les sources ne fixent pas ?
- Choix (choix d'Éric du 2026-10-24, point F4) :
  - le résultat d'une opération est celui du calcul en `double` (SANE), rangé en `float` (IMP-0144 bis). Ce calcul définit le résultat ; le chemin exact d'IMP-0244 le reproduit bit à bit lorsqu'il s'applique ;
  - hors de ce chemin, `+ − × ÷` sont calculés en format étendu (`long double` de SANE, 64 bits de mantisse) et arrondis directement en `float`. Le `float` obtenu est le même : un double arrondi est sans effet pour ces quatre opérations dès que le format intermédiaire a au moins 2 × 24 + 2 bits, ce que remplissent le `double` (53) comme le format étendu (64), et les deux chemins donnent le `float` correctement arrondi du résultat exact (S. A. Figueroa, « When is double rounding innocuous? », *ACM SIGNUM Newsletter*, 30 (3), 1995, non versé) ;
  - le dépassement reste décidé sur le résultat en `double`, qui peut excéder le plus grand `float` alors que l'arrondi en `float` donne encore `FLT_MAX` (par exemple `FLT_MAX` + 2⁸⁰) : lorsque le `float` obtenu vaut ±`FLT_MAX` ou l'infini, l'opération est refaite en `double`, par le calcul même d'avant ; la division par zéro est reconnue sur les bits du diviseur. Sur l'Apple IIGS, ce calcul en `double` évalue l'expression en format étendu avant de la ranger en `double` ; sous Golden Gate, le format étendu n'ayant que 53 bits, il range directement en `double`. Les deux machines peuvent donc trancher différemment un cas limite comme `FLT_MAX` + 2⁷⁴ + 2⁵¹ (pas d'erreur sur l'Apple IIGS, EX 44 sous Golden Gate) ; sur chacune, l'ancien et le nouveau calcul concluent de même ;
  - `^` et les fonctions restent calculés en `double` : pour eux, l'absence d'effet du double arrondi n'est pas établie ;
  - le pas de FAIRE (`V ← V + p`, LSE15 §5-5.3) suit la même règle : hors du chemin exact, il est calculé en format étendu, et un résultat au-delà du plus grand `float` est l'erreur EX 44 (audit du code, AC-35, 2026-10-30) ;
  - un résultat de valeur absolue supérieure au plus grand `float` est l'erreur EX 44 ; un résultat non nul plus petit que le plus petit `float` devient 0, sans erreur, comme une constante (Compilateur IMP-0191) ;
  - division par zéro : EX 31 ; 0 élevé à une puissance négative : EX 25 ; nombre négatif élevé à une puissance non entière : EX 26 ; un nombre négatif élevé à une puissance entière est admis ; `0^0` vaut 1 (constaté par Éric sur LSE 8086 version IV le 2026-10-24) ;
  - `RAC` d'un nombre négatif : EX 41 ; `LGN` d'un nombre négatif ou nul : EX 42 ; `ENT` donne le plus grand entier inférieur ou égal (LSE15 §3-2.2) ; `SGN` donne −1, 0 ou 1 ; `SH`, `CH`, `TH` sont le sinus, le cosinus et la tangente hyperboliques (profil T1600).
- Alternatives envisagées : calculer en `float` — écartée : chaque opération perdrait de la précision avant l'arrondi final.
- Justification : IMP-0144 bis, IMP-0191 ; MEMENTO82 (25, 26, 31, 41, 42, 44).
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : une conversion par opération ; le format étendu épargne à `+ − × ÷` hors du chemin exact cinq à sept conversions et deux comparaisons ; mesuré sur l'Apple IIGS le 2026-10-30 (sonde `PERF`, les deux calculs dans le même essai) : `1.5/3`, 1,200 ms au lieu de 3,067 ; `0.1+0.2`, 1,033 ms au lieu de 2,533.
- Impact historique : conforme aux erreurs de MEMENTO82 et au constat sur LSE 8086.
- Réalisation et essais : `donnees_operation_etendue` (`donnees.c`) ; la suite `composants_nombres` confronte bit à bit, erreurs comprises, le calcul en format étendu et le calcul en `double` sur toutes les paires de 40 valeurs construites par leurs bits (extrêmes, sous-normaux, bande au-dessus de `FLT_MAX`, décimaux) et sur 20 000 paires tirées au hasard, pour les quatre opérations. Sous Golden Gate, le format étendu de SANE n'a que 53 bits (constaté le 2026-10-30) : la suite y éprouve l'aiguillage et le recalcul, et ne vérifie le double arrondi par 64 bits que passée sur l'Apple IIGS (programme `NOMBRES` de l'image `cible`). Passée sur l'Apple IIGS le 2026-10-30 : aucun écart entre les deux calculs ; deux cas explicites, écrits d'après Golden Gate (`±FLT_MAX ± (2⁷⁴ + 2⁵¹)`, EX 44 attendu), y concluent sans erreur, ancien et nouveau calcul ensemble ; ils sont remplacés par `±FLT_MAX ± 2⁸⁰`, qui donne EX 44 sur les deux machines.
- Références normatives : LSE15 §3-2 ; Gestionnaire des erreurs §9 ; IMP-0144.
- Statut : retenu (Éric, 2026-10-24, point F4) ; révisé le 2026-10-30 (Éric ; audit du code, AP-04, §9 point 9) : format étendu pour `+ − × ÷`.
- Complément du 2026-11-05 (Éric) : ce choix est celui du profil MITRA 15. Sous les profils `LSE F4` et `-T1600`, tout résultat réel est en outre arrondi à 23 bits significatifs avant d'être rangé (IMP-0280) : le calcul se fait de même, en format étendu ou en `double`, et le dépassement (EX 44) est décidé sur la valeur arrondie, sans recalcul en `double`.

### 3.1 bis Réels du profil T1600

**IMP-0280** — *Réels de 23 bits significatifs, la demie en s'éloignant de zéro, sous `LSE F4` et `-T1600`*
- Sujet : avec la réponse 7, le programme `NBOR` de Victor Duruy (5ème, 1979-80, `LSE F4`) imprime `14 ØPERATIØNS` ; avec les réels de 24 bits de l'IEEE (IMP-0216), LSE-IIGS en obtient 16, et 4 des 14 valeurs imprimées diffèrent. Quelle arithmétique reproduit le T1600 ?
- Choix (choix d'Éric du 2026-11-05) :
  - sous les profils `LSE F4` et `-T1600`, un réel a 23 bits significatifs : toute valeur réelle qui naît est arrondie à 23 bits, la demie en s'éloignant de zéro, puis rangée en `float`, où elle tient sans nouvel arrondi, le dernier bit de la mantisse étant nul ;
  - sont arrondis : les constantes compilées (`ecrire_nombre`, Compilateur), les nombres lus par `LIRE` et convertis par `CNB`, les résultats de `+ − × ÷` hors du chemin exact d'IMP-0244 et le pas de `FAIRE` (`donnees_operation_etendue`), ceux de `↑`, des fonctions et de `TEM` (`donnees_ranger_reel`, appelée par l'empilement) ;
  - l'arrondi porte sur le résultat du calcul en format étendu (`+ − × ÷`, 64 bits) ou en `double` (`↑` et fonctions, 53 bits) : on ajoute la demie unité du dernier rang gardé, puis on tronque, sur les mots du format étendu de SANE (`rcim_arrondir_23`, module `rcim`, que lient le compilateur et l'interpréteur) ; pour `+ − × ÷` d'opérandes de 23 bits, le résultat est celui de l'arrondi du résultat exact, le format intermédiaire ayant au moins 2 × 23 + 2 bits et le quotient de deux réels de 23 bits ne tombant jamais sur une demie (IMP-0216) ;
  - un résultat arrondi de valeur absolue supérieure au plus grand `float` est l'erreur EX 44 ; le plus grand réel est (2²³ − 1) × 2¹⁰⁵, borne d'un nombre trop grand lu par `LIRE` ou converti par `CNB` ;
  - entiers exacts : le chemin entier d'IMP-0244 ne sert que si le résultat reste sous 2²³ ; au-delà, l'opération passe par le format étendu et l'arrondi ; 8 388 609 devient 8 388 610 ;
  - puissance entière : `pow` en `double`, arrondi une fois ;
  - `ALE` : le résultat x′/2²⁴ d'IMP-0225 a 24 bits quand x′ ≥ 2²³ ; son dernier bit est alors tronqué, et non arrondi, pour qu'il reste dans ]0,1[ ;
  - l'impression (`U`, à 7 chiffres arrondis, IMP-0211) et la décompilation (6 chiffres) sont inchangées ; il en résulte qu'une constante décimale ne s'imprime pas toujours telle qu'elle a été écrite : `AFFICHER 0.1,8.3` imprime `0.09999999 8.299999` ;
  - les valeurs d'un fichier-données rappelées par `CHARGER` sont prises telles qu'elles ont été rangées ;
  - profil MITRA 15 : inchangé (IMP-0216, 24 bits, arrondi au pair).
- Alternatives envisagées :
  - 24 bits, arrondi au pair (IMP-0216, état antérieur) — écartée : 4 témoins d'INRDP75 sur 16, `NBOR` en 16 opérations ;
  - 23 bits, arrondi au pair — écartée : `NBOR` en 13 opérations ;
  - impression par mise à l'échelle dans les réels de la machine, puis troncature — écartée : 14 témoins d'INRDP75 sur 16, mais 9 valeurs de `NBOR` sur 14 ; rien n'établit ce procédé pour F4 ;
  - puissance entière par multiplications de gauche à droite, arrondies à chaque pas, comme le laisse entendre la note 4 d'INRDP75 §4-3 (« multiplications successives ») — écartée : `PSI*R1*(XA+2)/(A+B)↑(2*K)` y donne `-9.701531E-05` au lieu de `-9.701532E-05` ; les carrés successifs et l'arrondi unique reproduisent le témoin, et l'arrondi unique ne change pas le calcul d'IMP-0216 ;
  - puissance non entière par le logarithme et l'exponentielle arrondis à 23 bits (même note) — non retenue : elle n'explique pas le témoin `4.6089` de `LGN(ZP↑0.25)` (elle donne, comme `pow`, `4.608899`) ;
  - mantisse décimale, comme le suggère INRDP75 §3-3 (« un exposant (puissance de 10) ») — écartée : 82/10 y serait exact, et la machine imprime `8.200001` ;
  - arrondir aussi sous MITRA 15 — écartée : autre machine, aucun témoin.
- Justification : analyse du 2026-11-05 (`05-IMPLEMENTATION/audits/Analyse_reels_T1600_2026-11-05.md`, essais en annexe) : un simulateur en arithmétique exacte, confronté aux 16 témoins d'INRDP75 qui n'emploient que `+ − × ÷` et des puissances entières et à la sortie de `NBOR`, désigne 23 bits pour les deux corpus (de 20 à 26 bits essayés) ; la demie en s'éloignant de zéro donne exactement les 14 opérations de `NBOR`, et 13 valeurs sur 14 ; INRDP75 : 12 témoins sur 16. INRDP75 §3-3 : « Les constructeurs garantissent 6 chiffres significatifs » ; « il ne faudrait pas s'étonner que le nombre 0,4 soit donné par la machine sous la forme 0,3999999 ».
- Impact mémoire : aucune donnée de plus, un réel restant un `float` de 4 octets dans la pile, les variables, les tableaux, RCI-M et les fichiers ; environ 0,7 Kio de code (`LSE` : 247 983 octets au lieu de 247 269). Impact disque : aucun ; le format de RCI-M, des fichiers-programmes et des fichiers-données est inchangé. Impact performances : l'arrondi lui-même se fait sur les mots du format étendu, sans appel à SANE (277 cycles, 0,024 ms, `iix --cycles`, profil `cible`) ; le dépassement se reconnaît aux bits du `float` obtenu, sans comparaison SANE. Mesuré sur l'Apple IIGS le 2026-11-05 (`PERF`) dans une première réalisation qui comparait le résultat à `FLT_MAX` en format étendu : +0,433 ms par opération, boucle (e) `X_I/3` +5,8 % ; `iix --cycles` ne compte pas SANE, et l'estimation l'avait manqué ; comparaisons remplacées le même jour par le test des bits ; remesuré alors (`PERF`) : `1.5/3` +0,033 ms (1,200 → 1,233), `0.1+0.2` +0,067 ms, boucle (e) +1,1 %, à la résolution de la sonde près (0,033 ms). Aucun effet sur les boucles d'entiers, ni sous MITRA 15.
- Impact historique : rapproche les deux profils T1600 des témoins. Reproduits : `NBOR` en 14 opérations (sous `LSE F4` et `-T1600`) ; la note `9.565216 /20` de PRON2, soit `11/(23/20)`, dans le dossier dactylographié de PRO et PRON2 (`4EME - PRO/Exécution PRO-PRON2.pdf`, versé le 2026-11-05 ; réels de 24 bits : `9.565218`, calcul exact : `9.565217`) ; 12 des 16 témoins d'INRDP75 sur `+ − × ÷` et puissances, et 14 des 18 valeurs de la session du §4-3, fonctions comprises (9 avec les réels de 24 bits). Divergences consignées : la première valeur de `NBOR` (`3.645751` pour `3.645752`) ; INRDP75 PG24 (`10.16667` pour `10.16666`), PG4 (`2.2012E+09` et `-2.434333` pour `2.201199E+09` et `-2.434332`), §4-3 (`30.14375` pour `30.14374`, `4.608899` pour `4.6089`, `-1.416872` pour `-1.416871`, `-0.9562017` pour `-0.9561998`).
- Réalisation et essais : `rcim_arrondir_23` (`rcim.c`) ; `donnees_ranger_reel`, `donnees_ranger_reel_borne`, `donnees_operation_entiere`, `donnees_operation_etendue`, `donnees_nombre_entier`, `donnees_cnb` (`donnees.c`) ; `pousser_nombre`, `LIRE`, `ALE` (`interprete.c`) ; `ecrire_nombre` (`compilateur.c`). Suites : `representation_rcim` (arrondi, demie, retenue, plus grand réel) ; `composants_entrees_sorties` (témoins d'INRDP75, `NBOR`, constantes, `LIRE`, entiers autour de 2²³, `ALE` dans ]0,1[, profils `-T1600` et MITRA 15).
- Références normatives : INRDP75 §3-3, §3-4 (PG16), §4-3 et sa note 4, PG4, PG7, PG19, PG24, PG27, PG58 ; sorties de Victor Duruy, `NBOR` (`numerisation0024`) ; IMP-0216, IMP-0244, IMP-0225, IMP-0276 ; Compilateur IMP-0191 ; Données IMP-0212.
- Statut : retenu (Éric, 2026-11-05, point 5).

### 3.2 Relations et logique

Une relation compare deux nombres, ou deux chaînes (Données §3.2). Le genre d'un opérande est celui de sa valeur : une chaîne de la pile, ou une variable dans l'état `ID_CHAINE`, est une chaîne ; un nombre ou une variable `ID_VARIABLE` est un nombre. Un nombre comparé à une chaîne est l'erreur EX 10 ; une variable non définie dans une comparaison aussi (RX1). Le résultat est une valeur booléenne. `ET` et `OU` évaluent leurs deux opérandes (LSE15 §5-4 : les expressions booléennes « seront évaluées en entier »).

### 3.3 Affectation

La cible est une référence. Une cible `ID_CHAINE` reçoit une chaîne, dont l'objet propre est obtenu par `donnees_chaine_vers_objet` (reprise d'une chaîne transitoire, copie sinon) ; une autre cible reçoit un nombre ; un élément de tableau reçoit un nombre. Les erreurs sont celles des transitions d'IMP-0179 (EX 06).

### 3.4 Branchements calculés

**IMP-0217** — *Cible d'un `ALLER EN` ou d'un `RETOUR EN` calculé*
- Sujet : quelle erreur pour une cible calculée qui ne désigne pas une ligne ?
- Choix (choix d'Éric du 2026-10-24, point B3) : la valeur est arrondie à l'entier le plus voisin (IMP-0179) ; un résultat inférieur à 1 est l'erreur EX 37 ; un résultat supérieur à 32 767 est l'erreur EX 38 ; un résultat de 1 à 32 767 qui ne désigne aucune ligne, numéros de 256 à 32 767 compris, est l'erreur EX 75.
- Alternatives envisagées : une seule erreur pour toute cible invalide — écartée : MEMENTO82 distingue les trois cas (37, 38, 75).
- Justification : MEMENTO82, annexe II ; IMP-0187.
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : nul.
- Impact historique : bornes non documentées par le corpus ; les natures sont celles de MEMENTO82.
- Références normatives : INRDP75, 3ᵉ partie, §2 ; LSE15 §5-1, §7-5 ; Gestionnaire des erreurs §9.
- Statut : retenu (Éric, 2026-10-24, point B3).

### 3.5 `UN` et `ZER`

**IMP-0218** — *`UN` et `ZER` sur un identificateur qui n'est pas un tableau*
- Sujet : `UN X` et `ZER X` (profil T1600) mettent à 1 ou à 0 tous les éléments du tableau `X` (RCI-M IMP-0117, INRDP75 Annexe III) ; que faire si `X` n'est pas un tableau ?
- Choix : l'erreur EX 47, « indices sur un identificateur non tableau », nature la plus proche (choix d'Éric du 2026-10-24, point B4). Les éléments sont tous définis après l'instruction (fonction `donnees_remplir`, ajoutée au module `donnees`).
- Alternatives envisagées : une nature nouvelle à partir de 100 — écartée : la correction est la même que pour 47.
- Justification : IMP-0187.
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : linéaire en le nombre d'éléments.
- Impact historique : neutre.
- Références normatives : RCI-M IMP-0117 ; INRDP75 Annexe III ; Gestionnaire des erreurs §9.
- Statut : retenu (Éric, 2026-10-24, point B4).

---

**IMP-0244** — *Chemin exact sans SANE : comparaisons par les bits, entiers exacts calculés en entiers*
- Sujet : sur l'Apple IIGS, chaque opération SANE coûte de 0,45 à 0,9 ms (comparaison, addition, conversion d'entier en `float` ou de `float` en `double` ; mesures du 2026-10-25, `05-IMPLEMENTATION/audits/Mesures_performance_IIGS_2026-10-25.md`), et le pas et le test de `FAIRE`, les constantes entières et les compteurs en font plusieurs par ligne.
- Choix (module `donnees`, segment `"donnees"`) :
  - **I1 — comparaisons** : `donnees_comparer_nombres` compare deux `float` par leur représentation : une clé de signe et valeur absolue (`-0` et `+0` ont la même) suit l'ordre des valeurs finies ; elle sert aux six relations et au test de `FAIRE` (sortie de boucle et signe du pas) ; les infinis et les NaN n'apparaissent jamais (EX 44, IMP-0216) ;
  - **I2 — opérations** : `donnees_operation_entiere` calcule `+`, `-`, `*` en entiers quand les deux opérandes sont des entiers exacts de valeur absolue inférieure à 2²⁴, reconnus par leur exposant et leur mantisse, et que le résultat reste sous 2²⁴ ; le signe d'un résultat nul suit la règle IEEE (produit : signe du produit des signes ; somme : `-0` pour `-0 + -0` seulement) ; elle sert aux trois opérations et au pas de `FAIRE` ; le changement de signe passe par le bit de signe (`donnees_opposer`) ; tout autre cas (décimaux, grands nombres, division, puissance, fonctions) suit IMP-0216 ;
  - **I3 — conversions** : `donnees_nombre_entier` construit le `float` d'un entier de valeur absolue inférieure à 2²⁴ par décalage et assemblage de l'exposant et de la mantisse ; elle sert aux constantes entières, aux numéros de ligne et aux résultats de I2 ;
  - les nombres passent par adresse (`pile_empiler_nombre_exact`) : ORCA/C convertit au format étendu de SANE tout `float` passé ou rendu par valeur.
- Équivalence : tout entier de valeur absolue inférieure à 2²⁴ est exact en `float` comme en `double` ; si les opérandes et le résultat restent sous 2²⁴, le calcul en `double` d'IMP-0216 produit l'entier mathématique, que son rangement en `float` conserve ; le chemin entier produit ce même entier, donc les mêmes bits, et le contrôle de dépassement (EX 44) ne peut s'y déclencher. La suite `tests/composants/composants_nombres.c` le vérifie bit à bit sur toutes les paires d'entiers de −150 à 150 et sur 24 valeurs aux limites (zéros signés, 2²⁴ − 1, 2²⁴, décimaux, grands nombres), ainsi que la conversion des entiers de −70 000 à 70 000, l'ordre des comparaisons et le changement de signe.
- Alternatives envisagées : calculer tous les nombres en entiers ou en virgule fixe — écartée : contraire à IMP-0216 et au réel de LSE15 ; garder SANE partout — écartée : coût mesuré.
- Justification : mesures du 2026-10-25 ; IMP-0216 ; choix d'Éric du 2026-10-25 (points I1 à I3).
- Réalisation : aucun décalage de `long` d'une quantité variable dans une boucle (0,033 ms chacun sous ORCA/C) : le bit de poids fort est trouvé par dichotomie sur une table de puissances de deux, et les masques y sont lus.
- Impact mémoire : une table de 25 entiers longs. Impact disque : sans objet. Impact performances : le pas, le test de `FAIRE` et les calculs sur les entiers exacts ne font plus aucun appel à SANE ; mesuré sur l'Apple IIGS le 2026-10-25 : comparaison 0,10 ms (SANE : 0,67), conversion d'un entier 0,17 ms (SANE : 0,47), empilement par adresse 0,13 ms (par valeur : 0,37) ; boucles `FAIRE` 17 à 33 % plus rapides.
- Impact historique : nul : les résultats sont identiques, bit à bit, à ceux d'IMP-0216.
- Références normatives : IMP-0216, IMP-0144 ; État d'exécution §6 (FAIRE).
- Statut : retenu (Éric, 2026-10-25).
- Complément du 2026-10-30 (Éric ; audit du code, AP-02 a, AP-07, AP-10) :
  - un indice de tableau entier exact de valeur absolue inférieure à 2²⁴ est pris tel quel ; les autres sont arrondis à l'entier le plus voisin, la moitié vers le haut (IMP-0179) ; le rang est le même dans les deux cas, `arrondir(n)` valant `n` (`donnees_rang_exact`, indices par adresse) ;
  - l'affectation d'un nombre, à une variable simple comme à un élément de tableau, et le pas de FAIRE passent la valeur par adresse (`contexte_affecter_nombre_exact`, `donnees_ecrire_element_exact`) ;
  - les 57 constantes immédiates (−28 à 28) sont converties une seule fois par `donnees_nombre_entier`, aux mêmes bits.
- Complément du 2026-11-05 (Éric ; IMP-0280) : sous les profils `LSE F4` et `-T1600`, le chemin entier ne sert que si le résultat reste sous 2²³ en valeur absolue, et `donnees_nombre_entier` construit les entiers inférieurs à 2²³ ; au-delà, le résultat passe par le format étendu et l'arrondi à 23 bits. L'équivalence est la même : un entier inférieur à 2²³ est exact sur 23 bits, et l'arrondi le conserve.

## 4. Éléments réalisés aux étapes suivantes

**IMP-0219** — *Éléments des fichiers : erreur interne jusqu'à leur étape*
- Sujet : que fait l'interpréteur des éléments `GARER`, `CHARGER` et `SUPPRIMER` (`0xAC` à `0xAE`) avant l'étape 7c ? (`EXECUTER`, `0xAF`, est réalisé à l'étape 7b : Bibliothécaire et fichiers IMP-0252.) (`AFFICHER` et `LIRE` sont réalisés à l'étape 5c, §4 bis ; `TEM`, `DAT` et `ALE` à l'étape 5d, §4 ter.)
- Choix : ils donnent provisoirement l'erreur EX 96, « problème interne à LSE ». Ils seront réalisés à l'étape « fichiers », derrière l'interface `fichiers.h` (choix d'Éric du 2026-10-24, points F0 et B5).
- Alternatives envisagées : des bouchons dès l'étape 5b — écartée : ils n'apportent rien aux tests de cette étape.
- Justification : Conditionnement §17 (chaque étape testable).
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : nul.
- Impact historique : provisoire, sans effet sur une version livrée.
- Références normatives : Gestionnaire des erreurs §9 (EX 96).
- Statut : retenu (Éric, 2026-10-24, point B5) ; clos le 2026-10-25 : tous ces éléments sont réalisés (EXECUTER : Bibliothécaire et fichiers IMP-0252 ; GARER, CHARGER, SUPPRIMER : IMP-0253).

---

## 4 bis. Entrée-sortie sur la console (étape 5c)

**IMP-0220** — *Interface console : commandes de télétype, rendues par la couche console*
- Sujet : par quelles primitives l'interpréteur écrit-il et lit-il sur la console, sans dépendre de l'Apple IIGS ?
- Choix : `src/interfaces/interface_console.h` déclare :

```c
void         console_ecrire(const char *texte, unsigned int n);
void         console_retour_chariot(void);   /* CR : début de la ligne courante */
void         console_interligne(void);       /* LF : ligne suivante */
void         console_bip(void);              /* BEL */
#define SAISIE_XOFF   0
#define SAISIE_RETURN 1
#define SAISIE_ESC    2
/* Lit une ligne jusqu'à X-OFF, RETURN ou ESC, non rangés ; rend sa longueur
   (au plus max) et la touche de fin dans *fin. */
unsigned int console_lire_ligne(char *tampon, unsigned int max, unsigned char *fin);
int          console_interruption(void);    /* ESC frappée depuis la dernière consultation */
```

  `console_lire_ligne` n'est appelée que par le moniteur (Moniteur IMP-0237) ; `console_interruption` est consultée par l'interpréteur à chaque arrivée sur une ligne, retour sur la ligne `FAIRE` compris, et par le moniteur pendant un listage (Moniteur IMP-0239).

  L'interpréteur émet les commandes d'une console de type télétype, comme les décrit LSE15 §6-2.2 : `/` donne un retour chariot puis un interligne, `C` un retour chariot, `L` un interligne. C'est la couche console qui les rend sur l'écran ; sur l'Apple IIGS, un interligne seul passe au début de la ligne suivante (LSE15 : `L` « ne fonctionne pas sur les consoles : se comporte comme `/` »). La réalisation sur l'Apple IIGS (`src/plateforme/console_iigs.c`) relève de l'étape du dialogue ; les tests emploient un bouchon (`tests/bouchons/console_bouchon.c`) qui enregistre l'affichage et fournit les réponses.
- Alternatives envisagées : une primitive « ligne suivante » choisie par l'interpréteur — écartée : `L` se distinguerait de `/` dans l'interpréteur selon la console, ce qui est l'affaire de la console.
- Justification : LSE15 §6-2.2 ; Organisation du dépôt (`src/interfaces/`, `src/plateforme/`) ; choix d'Éric du 2026-10-24 (point G1).
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : un appel par écriture.
- Impact historique : restitue les quatre spécifications de mise en page de LSE15.
- Références normatives : LSE15 §6-2.2 ; Tome III (services d'entrée-sortie) ; IMP-0136.
- Statut : retenu (Éric, 2026-10-24, point G1 ; touche de fin et `console_interruption` ajoutées le 2026-10-24, points K4, L2, L4).

**IMP-0221** — *AFFICHER et `?`*
- Sujet : comment l'interpréteur exécute-t-il les éléments `0xB0` à `0xB8` ?
- Choix :
  - toute la sortie passe par `ruban_sortie` et `ruban_sortie_commande` : `EQC(17)` à `EQC(21)` ne vont pas à la console, `EQC(18)` et `EQC(20)` commandent le perforateur (Bibliothécaire et fichiers IMP-0274, IMP-0275) ;
  - **Sans format** (`0xB8`, et `?` en mode machine de bureau) : un retour chariot et un interligne en tête (LSE15 §6-2.4), puis chaque valeur en U : un nombre suivi d'une espace, une chaîne telle quelle (INRDP75, PG5 : `BON COURAGE, CORALIE, POUR L'ETUDE DU L S E !`) (point G3).
  - **Avec format** (`0xB7`) : aucun passage à la ligne automatique. Les libellés et mises en page s'écrivent dans l'ordre ; une spécification de description (`0xB5`) s'applique aux r valeurs qui la suivent ; une valeur sans description est écrite en U (LSE15 §6-2.4).
  - **Facteur variable** : la valeur, arrondie, donne le nombre de répétitions ; zéro n'écrit rien ; une valeur négative est l'erreur EX 104 (LSE15 §6-2.6).
  - **Chaîne sous F ou E** : erreur EX 06 (point G5).
  - **Tableau entier** : une seule spécification pour tous les éléments ; un passage à la ligne à la fin de chaque ligne du tableau, celui de la dernière ligne valant pour « après la dernière ligne » ; le passage à la ligne « avant de commencer l'affichage du tableau » n'a lieu que si la sortie n'est pas déjà en début de ligne (LSE15 §6-2.5 ; constaté par Éric sur LSE 8086 version IV le 2026-10-24, §9) ; un élément sans valeur est l'erreur EX 32.
  - **Début de ligne** : l'interpréteur sait si la sortie est en début de ligne : elle l'est après un retour chariot et après une saisie terminée par RETURN ; un interligne seul ne change pas la colonne d'une console de type télétype. Le moniteur fixe cet état après ses propres messages (`interprete_debut_de_ligne`).
  - Aucun passage à la ligne n'est inséré au bout d'une ligne trop longue (LSE15 §6-2.4).
  - **Description en cours** : la spécification courante et le nombre de valeurs qu'elle couvre encore forment un état de l'interpréteur (`DescriptionAffichage`, 4 octets), remis à zéro en tête de chaque AFFICHER, à EXECUTER et à REPRENDRE (`interprete_reinitialiser`). Cet état survit à toute suspension de l'exécution (pas à pas, ESC, PAUSE, LIRE). À l'appel d'une procédure, il est rangé dans le cadre de l'appelant (`contexte_garder_affichage`, État d'exécution IMP-0146), et la procédure part sans description ; au retour, il est rétabli (`contexte_rendre_affichage`) : un AFFICHER dont la liste appelle une fonction garde sa description, même si la fonction affiche elle-même. Une ligne de machine de bureau laisse intact l'état du programme suspendu. Constaté sur LSE 8086 le 2026-10-30 : `AFFICHER[2F6.2] &F(1),3`, F exécutant `AFFICHER['*']`, donne `*     1.00     3.00`.
- Alternatives envisagées : garder la description dans l'état local d'une exécution — écartée : elle se perdait à toute suspension et à tout AFFICHER exécuté dans une fonction appelée (audit du code du 2026-10-29, AC-09) ; écrire une espace après une chaîne sans format (INRDP75 : « chacune étant suivie d'un blanc ») — écartée : contredit la sortie de PG5 et la règle de U pour les chaînes (LSE15).
- Justification : LSE15 §6-2 ; INRDP75 PG4, PG5 ; choix d'Éric du 2026-10-24 (points G3, G5) ; essais LSE 8086 du 2026-10-30 et choix d'Éric du même jour (description à travers un appel).
- Impact mémoire : un tampon de 256 caractères par nombre écrit. Impact disque : sans objet. Impact performances : nul.
- Impact historique : reproduit les exemples de LSE15 §6-2 et les sorties d'INRDP75 (tests).
- Références normatives : LSE15 §6-2.1 à §6-2.6 ; INRDP75 §1-2, PG4, PG5 ; Données IMP-0211.
- Statut : retenu (Éric, 2026-10-24, points G3, G5) ; règle des passages à la ligne d'un tableau confirmée le 2026-10-24 (§9) ; révisé le 2026-10-30 (Éric ; audit du code, AC-09 : description en cours conservée à travers un appel et une suspension).

**IMP-0222** — *LIRE*
- Sujet : comment l'interpréteur exécute-t-il les éléments `0xD1` et `0xBB` ?
- Choix :
  - chaque instruction `LIRE` émet un bip, et aucune invite (INRDP75, PG2 : « un sifflement par instruction LIRE ») ; la reprise par CONTINUER d'une lecture interrompue par ESC émet aussi un bip (constaté sur LSE 8086, Moniteur IMP-0237) ;
  - la saisie est lue ligne par ligne par le moniteur, qui la fournit à l'interpréteur (fin d'exécution `FIN_LECTURE`, Moniteur IMP-0237) ; la touche de fin, X-OFF ou RETURN, n'est pas rangée, sauf dans le cas de la chaîne ci-dessous (Moniteur IMP-0238) ;
  - **nombre** : les espaces de tête sont sautés ; le nombre s'arrête à une espace ou à la fin de la ligne ; il a la forme d'une constante (LSE15 §3-1.1), précédée d'un signe `+` ou `-` facultatif (point G2 ; INRDP75, PG4 : `-967`) ; un nombre trop grand prend le plus grand nombre représentable, comme `CNB` ; sous les profils `LSE F4` et `-T1600`, le nombre lu est arrondi à 23 bits (IMP-0280) ; si la ligne est épuisée, une nouvelle ligne est lue, sans bip ;
  - **variable-chaîne** : le reste de la ligne, espaces compris (INRDP75 §1-3) ; si la ligne se termine par RETURN, la chaîne reçoit en outre un passage à la ligne, CR LF, et la valeur est terminée ; la valeur suivante est lue sur la ligne suivante (INRDP75 PG3, second passage : « le retour à la ligne fait alors partie de la chaîne lue » ; points L3, L5, L7) ;
  - **réponse vide** à une variable-chaîne (X-OFF seul) : la chaîne vide ; l'instruction `LIRE` suivante demande une nouvelle ligne (constaté sur LSE 8086 le 2026-10-31 : une demande à chaque `LIRE`, `LGR` = 0) ;
  - **tableau entier** : ses éléments dans l'ordre ligne par ligne (LSE15 §6-1) ;
  - **réponse mal formée** : un bip, puis ` ? ` (un point d'interrogation entre deux espaces) ; le reste de la ligne est abandonné et la même valeur est saisie de nouveau (constat d'Éric sur LSE 8086 version IV, 2026-10-24, point F5) ;
  - ce qui reste sur la ligne après la dernière valeur demandée est conservé pour l'instruction `LIRE` suivante, qui émet néanmoins son bip (INRDP75 PG3 : `3 LIRE B,E` lit `EGALE` sur la ligne `5 EGALE` tapée pour `2 LIRE C,A,D` ; constaté sur LSE 8086 le 2026-10-24 : `10 LIRE A;LIRE B`, saisie `1 2`, B vaut 2, un bip précédant la lecture de B ; point L6) ; une nouvelle exécution (EXECUTER, REPRENDRE) oublie ce reste ;
  - **ligne de machine de bureau** (MEMENTO82 y admet LIRE) : elle part d'un état de lecture vierge ; celui du programme (ligne saisie, position, rang dans un tableau) est mis de côté à son entrée et rétabli à sa sortie, quelle qu'en soit la fin (`interprete_lecture_mettre_de_cote`, `interprete_lecture_retablir`, appelées par le moniteur, IMP-0233). Une lecture suspendue du programme reprend donc à CONTINUER avec les valeurs déjà lues (Moniteur IMP-0237), et le reste d'une ligne tapée pour un programme terminé n'est pas servi à un LIRE de bureau. LSE 8086 lit au fil de la frappe, une valeur se terminant à l'espace : aucun reste n'y survit à un LIRE (essai d'Éric du 2026-10-30).
- Alternatives envisagées : abandonner le reste de la ligne à la fin de l'instruction (point G2, retenu d'abord) — écartée le 2026-10-24 (point L6) : contraire à PG3 et à LSE 8086.
- Justification : LSE15 §6-1 ; INRDP75 §1-1, §1-3, PG2, PG4 ; choix d'Éric du 2026-10-24 (points F5, G2).
- Impact mémoire : un tampon de saisie de 256 caractères. Impact disque : sans objet. Impact performances : nul.
- Impact historique : reproduit la lecture d'INRDP75, y compris RETURN rangé dans une chaîne (PG3) ; la réponse mal formée et le bip à la reprise suivent LSE 8086, le corpus n'en disant rien.
- Références normatives : LSE15 §6-1 ; INRDP75 §1-1, §1-3 ; Gestionnaire des erreurs §9 (X-53).
- Statut : retenu (Éric, 2026-10-24, points F5, G2 ; révisé le 2026-10-24, points L1, L3, L6) ; complété le 2026-10-30 (Éric ; audit du code, AC-13, AC-14 : état de lecture d'une ligne de machine de bureau).

---

## 4 ter. Horloge et valeurs pseudo-aléatoires (étape 5d)

**IMP-0223** — *Interface horloge : une lecture de la date et de l'heure*
- Sujet : comment l'interpréteur obtient-il la date et l'heure sans dépendre de l'Apple IIGS ?
- Choix : `src/interfaces/interface_horloge.h` déclare `void horloge_lire(InstantHorloge *instant)`, seul point d'entrée de la lecture, et `void horloge_fixer(const InstantHorloge *instant)`, qui fixe l'horloge du système au démarrage de `LSE F4` (Moniteur et console IMP-0279). `InstantHorloge` porte l'année complète, le mois, le jour, l'heure, la minute et la seconde. Deux réalisations :
  - sur la cible, `src/plateforme/horloge_iigs.c` appelle `time()` et `localtime()` de la bibliothèque d'ORCA/C, qui lisent l'horloge du IIGS (ReadTimeHex, `cc.notes.txt` n° 231), sans appel direct à la Toolbox ;
  - pour les tests, `tests/bouchons/horloge_bouchon.c` rend un instant fixé par la suite, par défaut celui de l'exemple de LSE15 §4-4.2 (3 novembre 1972, 13 h 3 mn 52 s).
- Alternatives envisagées : appeler ReadTimeHex de l'outil Misc — écartée : la bibliothèque d'ORCA/C le fait déjà, et le code reste du C standard ; une opération par fonction L.S.E. — écartée : `TEM`, `DAT` et l'amorce d'`ALE` lisent la même grandeur.
- Justification : IMP-0136 (séparation noyau / plateforme) ; Conditionnement §17 (étape testable par un bouchon).
- Impact mémoire : 7 octets par lecture, sur la pile C. Impact disque : sans objet. Impact performances : un appel système par `TEM`, `DAT` ou premier `ALE(0)`.
- Impact historique : nul.
- Références normatives : Tome III (services du système) ; manuel ORCA/C, `<time.h>`.
- Statut : retenu (Éric, 2026-10-24, point H1).

**IMP-0224** — *TEM : secondes entières depuis zéro heure ; DAT : « JJ/MM/AA HH:MM:SS »*
- Sujet : quelle valeur rendent `TEM` (`0x8F`) et `DAT` (`0xA7`) ?
- Choix :
  - `TEM` rend le nombre entier de secondes écoulées depuis zéro heure, de 0 à 86 399, exact en float ; après une heure saisie au démarrage de `LSE F4`, l'heure ne repassant pas à zéro, cette borne ne vaut plus (Moniteur et console IMP-0279) ;
  - `DAT` rend une chaîne de 17 caractères `JJ/MM/AA HH:MM:SS` : jour, mois et deux chiffres de poids faible de l'année, séparés par `/` ; un espace ; heure, minute et seconde, séparées par `:` ; chaque nombre sur deux chiffres.
- Alternatives envisagées : `TEM` au soixantième de seconde — écartée : les deux manuels disent « en secondes » et LSE 8086 rend un entier ; l'année sur quatre chiffres — écartée : LSE15 fixe « les deux chiffres de poids faible ».
- Justification : INRDP75 §4-2 et §6-11 ; LSE15 §3-2.2 et §4-4.2 (`'03/11/72∧13:03:52'`, où `∧` note l'espace) ; essais d'Éric sur LSE 8086 version IV du 2026-10-24 : `?TEM()` → `5321`, entier ; `?DAT()` → `24/10/<6 15:20:42`, même disposition, l'année y étant mal calculée après 1999.
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : négligeable.
- Impact historique : reproduit les deux manuels. Divergences consignées : le `5321` de LSE 8086, relevé vers 15 h 20, ne compte pas les secondes depuis zéro heure ; l'origine de ce temps n'est pas connue et la présente réalisation suit les manuels. Le `<6` de LSE 8086 est un défaut de calcul de l'année, non reproduit : `DAT` rend `26` en 2026.
- Références normatives : INRDP75 §4-2, §6-11 ; LSE15 §3-2.2, §4-4.2 ; RCI-M §4 (`0x0F`, `0x27`).
- Statut : retenu (Éric, 2026-10-24, points H2, H3).

**IMP-0225** — *ALE : générateur congruentiel modulo 2²⁴*
- Sujet : comment `ALE(E)` (`0x8E`) produit-il une valeur « comprise entre 0 et 1 (bornes exclues) » qui, pour 0 < E < 1, « dépend uniquement de E », et pour E = 0 est « différente à chaque appel » (INRDP75 §4-2, LSE15 §3-2.2) ?
- Choix :
  - suivant(x) = (a·x + c) mod 2²⁴, avec a = 1 664 525 et c = 1 013 904 223 mod 2²⁴ = 7 271 263 ; a ≡ 1 (mod 4) et c impair donnent la période complète 2²⁴ ; le calcul se fait en `unsigned long`, modulo 2³², multiple de 2²⁴ ;
  - si suivant(x) vaut 0, le générateur avance d'un pas de plus : le résultat reste dans ]0,1[ ;
  - le résultat est x'/2²⁴, exact en float (24 bits de mantisse) ; une série `X←ALE(X)` est donc reproductible à l'identique ;
  - 0 < E ≤ 1 : x = ⌊E·2²⁴⌋ mod 2²⁴ ; le résultat ne dépend que de E ; `ALE(1)` rend la valeur fixe suivant(0) = 7 271 263/2²⁴ (point H6) ;
  - E = 0 : un état propre à la session est amorcé par l'horloge au premier `ALE(0)` (jour, heure, minute, seconde), puis avance d'un pas à chaque appel ; deux appels dans la même seconde rendent donc deux valeurs différentes ; `ALE(E)` avec E ≠ 0 ne lit ni ne modifie cet état, qui n'appartient pas à l'état d'exécution d'une tâche ;
  - E < 0 ou E > 1 : erreur EX 09, « ARGUMENT DE ALE NEGATIF OU SUPERIEUR A 1 » (MEMENTO82, annexe II).
- Alternatives envisagées : reproduire l'exemple d'INRDP75 §4-3 — impossible : `?ALE(0.5)` y rend `0.25` deux fois, et trois `?ALE(0)` successifs `0.6124945`, `0.8062472`, `0.9031236`, chaque valeur valant (précédente + 1)/2 tronquée ; aucun générateur congruentiel modulo 2²⁴ ne rend 0,25 pour 0,5 tout en prolongeant cette suite (recherche exhaustive des couples (a, c), 2026-10-24) ; `rand()` de la bibliothèque C — écartée : sa période et sa forme dépendent de la bibliothèque ; rejeter E = 1 comme LSE 8086 — écartée : MEMENTO82 ne rejette que l'argument « supérieur à 1 ».
- Justification : INRDP75 §4-2, §4-3 ; LSE15 §3-2.2 ; MEMENTO82, annexe II (EX 09) ; choix d'Éric du 2026-10-24 (point F6, points H4 à H6).
- Impact mémoire : 6 octets statiques. Impact disque : sans objet. Impact performances : une multiplication 32 bits par appel.
- Impact historique : les propriétés décrites par les manuels sont respectées ; les valeurs numériques de l'exemple d'INRDP75 §4-3 ne sont pas reproduites. Divergences consignées : sur LSE 8086 version IV (essais d'Éric du 2026-10-24), `?ALE(1)` donne l'erreur d'exécution 44 « Argument de ALE hors [0,1[ », et `?ALE(0.5)` rend deux fois `0.2522812486495809`, voisin du `0.25` d'INRDP75 ; la présente réalisation rend `0.9334010` pour `ALE(0.5)`.
- Références normatives : INRDP75 §4-2 ; LSE15 §3-2.2 ; Gestionnaire des erreurs §9 (EX 09, X-24) ; RCI-M §4 (`0x0E`).
- Statut : retenu (Éric, 2026-10-24, points F6, H4 à H6).
- Complément du 2026-11-05 (Éric ; IMP-0280) : sous les profils `LSE F4` et `-T1600`, un réel a 23 bits ; quand x′ ≥ 2²³, le dernier bit de x′ est tronqué avant la division par 2²⁴ ; le résultat reste dans ]0,1[ (x′ = 2²⁴ − 1, arrondi, donnerait 1) ; le générateur et son état sont inchangés.

---

## 5. Segments

**IMP-0226** — *L'interpréteur occupe deux segments, `"interprete"` et `"interp_es"`*
- Sujet : l'ajout de `TEM`, `DAT` et `ALE` porte le segment `"interprete"` (avec `pile.c`, IMP-0208) au-delà de 64 Kio : l'éditeur de liens refuse le programme (« Code exceeds code bank size », 2026-10-24).
- Choix : en application d'IMP-0197 et du choix d'Éric du 2026-10-24 (point B6), `interprete.c` porte une seconde déclaration `segment "interp_es";`. Pile, arithmétique, relations, affectation et fonctions sur les chaînes restent dans `"interprete"` ; AFFICHER, LIRE, horloge, `ALE`, distribution des éléments et cycle sont dans `"interp_es"`. `horloge_iigs.c`, appelé depuis `"interp_es"`, y est placé (Construction §1).
- Alternatives envisagées : déplacer `pile.c` dans un autre segment — écartée : la marge gagnée serait faible et la pile est le compagnon naturel des opérations ; un fichier source de plus — écartée : le découpage dans le même fichier suit le précédent du compilateur (`"compil_ins"`).
- Justification : IMP-0197 ; manuel ORCA/C, chap. 19 (déclaration `segment`).
- Impact mémoire : un segment statique de plus, chargé au lancement. Impact disque : un en-tête de segment. Impact performances : appels inter-segments longs, sans effet mesurable.
- Impact historique : nul.
- Références normatives : IMP-0135, IMP-0197 ; Architecture physique §5.2.
- Statut : retenu (Éric, 2026-10-24, point B6).

---

## 6. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0214 | Interface : `interprete_executer`, `interprete_bureau`, cinq fins d'exécution | retenu |
| IMP-0215 | Copie de la séquence de la ligne en cours dans un tampon local | retenu |
| IMP-0216 | Arithmétique en double, rangée en float ; dépassements, `0^0` = 1 | retenu |
| IMP-0217 | Cible calculée : EX 37, 38, 75 | retenu |
| IMP-0218 | `UN` et `ZER` sur un non-tableau : EX 47 | retenu |
| IMP-0219 | Fichiers : EX 96 jusqu'à leur étape | clos (2026-10-25) |
| IMP-0220 | Interface console : écrire, retour chariot, interligne, bip, lire une ligne | retenu |
| IMP-0221 | AFFICHER et `?` : passage à la ligne, U, format, mise en page, tableaux | retenu |
| IMP-0244 | Chemin exact sans SANE : comparaisons par les bits, entiers exacts en entiers, conversions construites | retenu |
| IMP-0222 | LIRE : bip, saisie fournie par le moniteur, espaces, signe, réponse mal formée, reste de la ligne conservé, RETURN rangé dans une chaîne | retenu |
| IMP-0223 | Interface horloge : `horloge_lire`, réalisée par `time()` et `localtime()` d'ORCA/C | retenu |
| IMP-0224 | TEM en secondes entières depuis zéro heure ; DAT `JJ/MM/AA HH:MM:SS` | retenu |
| IMP-0225 | ALE : générateur congruentiel modulo 2²⁴, résultat dans ]0,1[, ALE(0) amorcé par l'horloge | retenu |
| IMP-0226 | Deux segments : `"interprete"` et `"interp_es"` | retenu |
| IMP-0280 | Réels de 23 bits, demie en s'éloignant de zéro, sous `LSE F4` et `-T1600` | retenu |

Amendement appliqué ailleurs : État d'exécution IMP-0146 et §7.3 (base de cadre, point B2).

---

## 7. Tests

Suite `tests/composants/composants_horloge.c` (étape 5d) : `TEM` à 13 h 3 mn 52 s, à zéro heure et à 23 h 59 mn 59 s ; `DAT` de l'exemple de LSE15 §4-4.2, en 2026 et en 2000 ; `ALE(0.5)` deux fois, série `X←ALE(X)`, `ALE(1)`, 200 valeurs dans ]0,1[, `ALE(0)` différent à chaque appel ; EX 09 pour −0,1 et 1,5. Suite `tests/plateforme/plateforme_horloge.c` : l'horloge réelle, sous Golden Gate, rend un instant plausible.

Suite `tests/composants/composants_entrees_sorties.c` (étape 5c) : exemples de LSE15 §6-2 et d'INRDP75 (PG4, PG5), saisies par le bouchon de console, réponse mal formée ; réels du T1600 (IMP-0280) : 12 témoins d'INRDP75 reproduits et 4 non reproduits, `NBOR` (réponse 7, 14 opérations), constantes et `LIRE`, entiers autour de 2²³, `ALE` dans ]0,1[, mêmes calculs sous `-T1600` et sous MITRA 15. Suite `tests/representation/representation_rcim.c` : `rcim_arrondir_23` (demie en s'éloignant de zéro, retenue sur l'exposant, plus grand réel).


Suite `tests/composants/composants_interprete.c` : des programmes compilés par le compilateur, insérés dans le programme courant et exécutés ; les résultats sont lus dans l'environnement :
- arithmétique, priorités, fonctions, erreurs EX 25, 26, 31, 41, 42, 44 ;
- relations sur les nombres et les chaînes, `ET`, `OU`, `SI` en instruction et en expression ;
- chaînes : déclaration, affectation, concaténation, fonctions ;
- tableaux : déclaration, accès, `UN`, `ZER`, erreurs ;
- boucles `FAIRE` de LSE15 §5-5 (dont deux boucles de même ligne de fin), branchements, ligne d'arrêt, fin de programme sans `TERMINER` ;
- procédures : `&FACT(7)` (5040, LSE15 §7-6), paramètre référence (LSE15 §7-8.2), `RETOUR EN` en position d'expression (LSE15 §7-5), le programme du point D1 (3) ;
- ligne de machine de bureau ;
- erreurs : ligne et nature du diagnostic.

---

## 8. Traçabilité

| EXI | VM | IV | IMP | Code | Test |
|---|---|---|---|---|---|
| EXI-0011, EXI-0022 à EXI-0030, EXI-0037 | VM-2205, VM-2206, VM-3101, VM-3102 | IV-IF-0003, ALG-1102 | IMP-0214 à IMP-0219, IMP-0226 | `src/noyau/interprete.c`, `src/noyau/interprete.h` | `tests/composants/composants_interprete.c` |
| EXI-0045, EXI-0046 | VM-2205, VM-2206 | IV-IF-0003 | IMP-0220 à IMP-0222 | `src/noyau/interprete.c`, `src/interfaces/interface_console.h` | `tests/composants/composants_entrees_sorties.c` |
| EXI-0011, EXI-0022 à EXI-0030 | VM-2205, VM-2206 | ALG-1102 | IMP-0280 | `src/noyau/rcim.c`, `src/noyau/donnees.c`, `src/noyau/interprete.c`, `src/noyau/compilateur.c` | `tests/representation/representation_rcim.c`, `tests/composants/composants_entrees_sorties.c` |
| EXI-0011, EXI-0027 | VM-2205, VM-2206 | ALG-1102 | IMP-0223 à IMP-0225 | `src/noyau/interprete.c`, `src/interfaces/interface_horloge.h`, `src/plateforme/horloge_iigs.c` | `tests/composants/composants_horloge.c`, `tests/plateforme/plateforme_horloge.c` |

## 9. Point tranché après la première rédaction

Passages à la ligne autour d'un tableau affiché (IMP-0221). LSE15 §6-2.5 dit qu'un retour à la ligne est inséré « à la fin de chaque ligne du tableau », et « également [...] avant de commencer l'affichage du tableau, et après la dernière ligne ». Deux essais d'Éric sur LSE 8086 version IV, le 2026-10-24, l'ont précisé :
- `AFFICHER T;AFFICHER['X']` (T de deux éléments) affiche une seule ligne vide, celle d'`AFFICHER` sans format, puis `1`, `2`, et `X` sur la ligne suivante : un seul passage à la ligne après la dernière ligne ;
- `AFFICHER[/,'A',/] T;AFFICHER['X']` n'affiche aucune ligne vide entre `A` et `1` : le passage à la ligne avant le tableau n'a pas lieu quand la sortie est déjà en début de ligne.
