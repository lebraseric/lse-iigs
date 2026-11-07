# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — Le moniteur et la console : environnement conversationnel, analyseur de commandes

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V rédigé le 2026-10-24 (étape 6a de la feuille de route). Décisions IMP-0227 à IMP-0236, sur le découpage de l'étape 6 et les points K1 à K8 retenus par Éric le 2026-10-24. Points M1 à M7 tranchés le 2026-10-24 (§11), par essais d'Éric sur LSE 8086 pour M1 à M4 ; le point N1 (§11) est tranché le 2026-10-25 (Bibliothécaire et fichiers, IMP-0257). Points L1 à L4 (attente d'entrée, X-OFF et RETURN, lecture d'une chaîne, touche d'interruption) retenus le 2026-10-24 : IMP-0237 à IMP-0239 ; point L5 tranché (§11). Étape 6b réalisée le 2026-10-24 : `analyseur.c`, `moniteur.c`, suites `composants_analyseur` et `composants_moniteur` (§10). Points L6 et L7 tranchés le 2026-10-24 (§11). Étape 6c réalisée le 2026-10-24 (§9 bis, IMP-0240) : `console_iigs.c`, `main.c`, programme `LSE` ; essai sur cible à faire.
**Portée :** réalisation physique de l'environnement conversationnel (VM-2201, module `src/noyau/moniteur.c/.h`) et de l'analyseur de commandes (VM-2202, module `src/noyau/analyseur.c/.h`) : états de la console, saisie, classement des entrées, langage de commande, messages du dialogue, interruption. La console réelle de l'Apple IIGS (`src/plateforme/console_iigs.c`) et le point d'entrée (`src/plateforme/main.c`) relèvent de l'étape 6c ; les commandes portant sur les fichiers, de l'étape « fichiers ».
**Rang normatif :** subordonné au Tome III (VM-2201, VM-2202, VM-2101 à VM-2104), au Tome IV (IV-IF-0001 à IV-IF-0004, ALG-1101, ALG-1602, §17.1), et aux chapitres Compilateur, Décompilateur, Programme courant (IMP-0207), État d'exécution (IMP-0153, §7.4), Interpréteur (IMP-0214, IMP-0220, IMP-0221) et Gestionnaire des erreurs. Aucune disposition du présent chapitre ne peut contredire ces sources.

---

## 0. Avertissement de statut

Le Tome III fixe les responsabilités : l'environnement conversationnel reçoit une entrée, la tient pour complète à sa fin, la transmet une seule fois et restitue le résultat ou le diagnostic, sans jamais rompre le dialogue (VM-2201) ; l'analyseur de commandes classe chaque entrée validée et l'oriente vers un seul traitement, sans modifier le programme courant (VM-2202). Le langage de commande est celui d'INRDP75 (§3-2, §4) et de LSE15 (§9). Le présent chapitre fixe la réalisation de ces deux composants sur l'Apple IIGS et les points que ces sources laissent ouverts.

---

## 1. Sources

| Source | Apport |
|---|---|
| INRDP75 §3-2 | États dormant, moniteur, exécution ; BONJOUR, AU REVOIR ; touche ESC et message `PRET` |
| INRDP75 §4 ; LSE15 §9-1, §9-2 | Langage de commande : deux lettres et X-OFF, complétion, paramètres ; commandes générales |
| INRDP75, récapitulatif « Commandes » | Liste des commandes générales et des commandes de fichiers, par machine |
| INRDP75 PG1 à PG5 | Dialogues : `EXECUTER A PARTIR DE 1`, `TERMINE EN LIGNE 007` (T1600), `TERMINE` (MITRA 15) |
| Sorties T1600 de Victor Duruy | `PRET`, `PRET EN LIGNE 036`, `PAUSE EN LIGNE 161`, `TERMINE EN LIGNE 222`, `????`, `  ???? (COMMANDE ERRONEE)`, ouverture et fin de session ; `\` annule le caractère précédent |
| LAFOND83 p. 19 | `LIS` tapé pour `LI` est lu comme une instruction : `ERREUR C 38` |
| LSE 8086 version IV, essais d'Éric du 2026-10-24 | `LI` puis RETURN : le système complète et attend les paramètres ; `LISTER A PARTIR DE` tapé en entier : erreur de compilation ; `CONTINUER` après `TERMINER` : `???` ; ESC pendant `LIRE A,B`, après la saisie de A : `PRET EN LIGNE 10`, puis `CONTINUER` : un bip et la seule saisie de B ; X-OFF (Ctrl-S) et RETURN valident tous deux une saisie, mais X-OFF sur une ligne vide donne `???` et RETURN n'a aucun effet ; en PAS A PAS, X-OFF relance, RETURN ne fait rien |
| NOYELLE79 (non versé ; citation d'Éric) | RETURN pendant la saisie d'une chaîne est mis dans la chaîne : « ceci peut permettre de frapper une longue chaîne en plusieurs fois, en terminant tous les morceaux (sauf le dernier) par RETURN, et le dernier par X-OFF ». Version de LSE plus évoluée qu'INRDP75 et LSE15 (structures absentes de ces deux manuels, communes avec LAFOND83) ; hors des deux profils retenus (arbitrage d'Éric du 2026-10-24, point L7) |

---

## 2. Organisation

**IMP-0227** — *Deux modules : `moniteur.c` réalise VM-2201, `analyseur.c` réalise VM-2202*
- Sujet : comment répartir le dialogue entre les modules prévus par Organisation du dépôt §2.1 ?
- Choix :
  - `analyseur.c` reçoit une entrée validée (texte et longueur) et rend sa catégorie (§3) ; pour une commande, il rend aussi son identifiant. Il ne modifie aucun état et n'écrit rien sur la console.
  - `moniteur.c` tient l'état de la console (§4), lit les entrées par l'interface console (IMP-0220), traite la correction `\` (IMP-0231), appelle l'analyseur, puis le traitement désigné : compilateur et programme courant (IV-IF-0001, IV-IF-0002), interpréteur (IV-IF-0003, IMP-0214), décompilateur (IV-IF-0004). Il restitue les messages et les diagnostics.
  - Le point d'entrée est `void moniteur_session(void)`, qui rend la main quand la session se termine (IMP-0228). `main.c` (étape 6c) lit les options de lancement puis l'appelle.
  - `analyseur.c` occupe le segment blanc, avec `erreurs.c` et `console_iigs.c` (Organisation du dépôt §2.1, Construction §1) ; `moniteur.c` occupe le segment nommé `"moniteur"` depuis le 2026-10-25 : avec les commandes de fichiers et le code stdio d'ORCA/C, le segment blanc dépassait 64 Kio (« Address is not in current bank » à l'édition de liens ; IMP-0197).
- Alternatives envisagées : un seul module — écartée : le Tome III sépare les deux responsabilités, et l'analyseur seul se teste sans console.
- Justification : Tome III, VM-2201, VM-2202 ; Organisation du dépôt §2.1.
- Impact mémoire : un tampon d'entrée de 256 octets ; l'état de la console, quelques octets. Impact disque : sans objet. Impact performances : nul.
- Impact historique : nul.
- Références normatives : VM-2201, VM-2202 ; Organisation du dépôt §2.1.
- Statut : retenu (Éric, 2026-10-24, découpage de l'étape 6).

---

## 3. Classement des entrées

**IMP-0229** — *Ligne numérotée, commande de deux lettres, sinon machine de bureau*
- Sujet : comment l'analyseur distingue-t-il les catégories qu'exige VM-2202 ?
- Choix : après la correction `\` (IMP-0231), l'entrée validée est classée ainsi :
  - elle commence par un chiffre : **ligne de programme**, transmise au compilateur ; c'est lui qui contrôle le numéro et le caractère qui le suit (espace ou `*`, LSE15 §9-1) et produit, le cas échéant, son diagnostic ;
  - elle se compose exactement de deux lettres : **commande**, cherchée dans la table du profil (IMP-0230) ; deux lettres qui ne désignent aucune commande du profil sont une erreur du dialogue (IMP-0235) ;
  - elle est vide : terminée par RETURN, aucun traitement ; terminée par X-OFF, une erreur du dialogue (`????`), sauf en mode PAS A PAS après un arrêt de ligne, où elle relance l'exécution (IMP-0232, IMP-0238) ;
  - toute autre entrée : **ligne de machine de bureau**, transmise au compilateur puis, si elle est acceptée, à l'interpréteur (IMP-0233).
- Alternatives envisagées : reconnaître une commande à son mot complet — écartée (point K1) : LAFOND83 et LSE 8086 montrent qu'une commande tapée en entier est lue comme une instruction et rejetée par le compilateur.
- Justification : LSE15 §9-1 (« la différence entre ces deux entités étant établie par l'existence d'un numéro ») ; INRDP75 §4 ; LAFOND83 p. 19 ; essais LSE 8086 du 2026-10-24 ; VM-2202 (catégories).
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : nul.
- Impact historique : reproduit le classement historique ; `LISTER A PARTIR DE 1` tapé en entier donne une erreur de compilation.
- Références normatives : VM-2202 ; LSE15 §9-1 ; INRDP75 §3-3, §4.
- Statut : retenu (Éric, 2026-10-24, points K1, K2).

---

## 4. États de la console et session

**IMP-0228** — *Lancement en état dormant ; BONJOUR ouvre la session ; AU REVOIR la ferme et rend la main au shell*
- Sujet : comment se réalisent les trois états d'INRDP75 §3-2 dans un programme lancé depuis le shell ORCA ?
- Choix :
  - au lancement, après le démarrage de `LSE F4` (IMP-0279), la console est **dormante** ; seules y sont admises la commande `BO` et les commandes de l'initialisateur (point N1, §11) ; toute autre entrée donne `????` et la console reste dormante (point M6) ;
  - `BO` complète `NJOUR`, affiche la bannière du profil (IMP-0266) et fait passer la console à l'état **moniteur** ;
  - `AU` complète `REVOIR`, affiche la date et l'heure et, sous `LSE F4`, la bannière (IMP-0266), efface le programme courant et l'état d'exécution (et, à l'étape « fichiers », les fichiers temporaires), puis la session se termine : `moniteur_session` rend la main et le programme `LSE` revient au shell ORCA ;
  - l'état **exécution** dure de la commande qui lance une exécution (EXECUTER, CONTINUER, POURSUIVRE, REPRENDRE) ou d'une ligne de machine de bureau jusqu'à la fin d'exécution rendue par l'interpréteur (IMP-0214).
- Alternatives envisagées : lancer directement en état moniteur — écartée : BONJOUR est la première commande de toute session (LSE15 §9-2) ; garder `LSE` actif après AU REVOIR, en attente d'un nouveau BONJOUR — écartée (point K6) : il n'y aurait alors aucun moyen historique de quitter le programme.
- Justification : INRDP75 §3-2 ; LSE15 §9-1, §9-2 ; sorties de Victor Duruy (`BØNJØUR 15/10/80 08:36:17`, `AU REVØIR 15/10/80 08:38:36`).
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : nul.
- Impact historique : l'état dormant correspond au programme arrêté. Les lignes propres au T1600 de Victor Duruy (`CØNSØLE NØ. 01.`, `LSE F4`, `RUB-ØUT REMPLACE CTRL X-ØFF`, `VØUS ETES SØUS ID 1`) sont reproduites sous `LSE F4` seulement (IMP-0266).
- Références normatives : INRDP75 §3-2 ; LSE15 §9-2 ; Interpréteur IMP-0224 (forme de la date).
- Statut : retenu (Éric, 2026-10-24, point K6 ; révisé le 2026-11-01 : bannière de `LSE F4`, IMP-0266).

---

## 5. Saisie

**IMP-0230** — *Une commande se tape par deux lettres et X-OFF ou RETURN ; le système complète puis lit les paramètres*
- Sujet : comment réaliser la saisie d'une commande ?
- Choix :
  - les deux lettres sont validées par X-OFF ou par RETURN (IMP-0238) ;
  - après la validation des deux lettres, le moniteur affiche la suite du nom de la commande, sur la même ligne (`LI` → `STER A PARTIR DE `) ; si la commande a des paramètres, il lit alors une seconde entrée, validée de même, qui les porte ;
  - sous ABREGER, il n'affiche qu'un espace avant les paramètres, ou passe à la ligne pour une commande sans paramètre (LSE15 §9-2) ; IN EXTENSO rétablit la complétion ;
  - la complétion de chaque commande est celle du manuel du profil : INRDP75 pour T1600, LSE15 pour MITRA 15. Table des commandes générales :

| Lettres | Complétion | Paramètres | Profils |
|---|---|---|---|
| `BO` | `NJOUR` | — | les deux |
| `AU` | ` REVOIR` | — | les deux |
| `LI` | `STER A PARTIR DE ` | `n1{,n2}` | les deux |
| `PE` | `RFORER A PARTIR DE ` | `n1{,n2}` | les deux |
| `NU` | `MERO A PARTIR DE ` | `n1{,n2}` | les deux |
| `EF` | `FACER LIGNES ` | `n1{,n2}...`, `n1 A n2`, `*` | les deux |
| `EX` | `ECUTER A PARTIR DE ` | `n1{,n2}` | les deux |
| `CO` | `NTINUER` | — | les deux |
| `PO` | `URSUIVRE JUSQU'EN ` | `n` | les deux |
| `RE` | `PRENDRE A PARTIR DE ` (T1600) ; `PRENDRE EN ` (MITRA 15) | `n1{,n2}` | les deux |
| `PA` | `S A PAS` | — | les deux |
| `NO` | `RMAL` | — | les deux |
| `PR` | `ENDRE ETAT CONSOLE ` | `n` | les deux |
| `AB` | `REGER` | — | les deux |
| `IN` | ` EXTENSO` | — | les deux |
| `RU` | `BAN` | — | MITRA 15 |
| `SI` | `LENCE` | — | MITRA 15 |

  Les commandes de fichiers (`AP`PELER, `RA`NGER, `ID`ENTIFICATION, `UT`ILISATION DISQUE, `TA`BLE DES FICHIERS, `MO`DIFIER, `CA`TALOGUER, `SU`PPRIMER, `DE`CODER, `EN`CODER, et `DU`PLIQUER, attesté à Victor Duruy, sous `LSE F4` seulement ; sous ce profil, `$` devant un nom désigne un fichier de l'unité amovible, IMP-0278) figurent dans la table avec leur complétion ; leur traitement relève de l'étape « fichiers » (IMP-0236).
- Alternatives envisagées : accepter aussi la commande tapée en entier — écartée (point K1) : ce serait une syntaxe moderne, contraire à LAFOND83 et à LSE 8086.
- Justification : INRDP75 §4 ; LSE15 §9-1, §9-2 ; essai LSE 8086 du 2026-10-24 (`LI` puis RETURN : complétion, puis attente des paramètres).
- Impact mémoire : une table constante d'une trentaine d'entrées. Impact disque : sans objet. Impact performances : nul.
- Impact historique : reproduit la saisie des manuels.
- Références normatives : INRDP75 §4, récapitulatif « Commandes » ; LSE15 §9-2.
- Statut : retenu (Éric, 2026-10-24, points K1, K3).

**IMP-0231** — *Correction par `\` ; SILENCE*
- Sujet : comment corriger une saisie, et comment réaliser SILENCE ?
- Choix :
  - le moniteur applique la correction à toute entrée lue sur la console, y compris les réponses à `LIRE` : chaque `\` annule le caractère qui le précède ; un `\` en tête d'entrée est sans effet ; la ligne affichée garde l'écho de ce qui a été tapé ;
  - toute saisie passe par la même routine de lecture du moniteur, réponses à `LIRE` comprises (IMP-0237) : l'interpréteur reçoit une ligne déjà corrigée ;
  - SILENCE (profil MITRA 15) coupe l'écho de la saisie jusqu'à la prochaine interruption (ESC) ; l'écho relève de la console (`console_echo`) ; réalisation : Bibliothécaire et fichiers IMP-0265 ; RUBAN entraîne SILENCE (IMP-0263).
- Alternatives envisagées : laisser la correction à l'édition de ligne de la console du IIGS — écartée : le comportement dépendrait de la plateforme ; LSE15 §1 en fait un trait du langage.
- Justification : LSE15 §1 ; sorties de Victor Duruy (`PRE\ENDRE`, `DIDI\`, `51S\`) ; INRDP75 §4-1 (SILENCE).
- Impact mémoire : nul (correction sur place). Impact disque : sans objet. Impact performances : nul.
- Impact historique : reproduit la correction attestée.
- Références normatives : LSE15 §1 ; INRDP75 §4-1.
- Statut : retenu (Éric, 2026-10-24, points K3, K7).

---

## 6. Commandes d'exécution

**IMP-0232** — *EXECUTER, CONTINUER, POURSUIVRE, REPRENDRE, PAS A PAS, NORMAL*
- Sujet : comment le moniteur réalise-t-il les commandes d'exécution et leurs conditions ?
- Choix :
  - le moniteur tient un **état du programme** : *aucun* (rien n'a été exécuté, ou après effacement total), *suspendu* (arrêt par `PAUSE`, par la ligne d'arrêt `n2`, par PAS A PAS ou par ESC), *terminé* (`TERMINER`, fin de programme), *en erreur* ; il mémorise, à chaque arrêt, la valeur du compteur des lignes entrées (`programme_lignes_entrees`, IMP-0207) ;
  - `EXECUTER A PARTIR DE n1{,n2}` : `contexte_vider` puis `contexte_initialiser(n1, n2)` (IMP-0153), puis `interprete_executer` ; `n1` est obligatoire ; `n2` absent vaut « aucune ligne d'arrêt » ;
  - `CONTINUER` : relance `interprete_executer` à la position mémorisée ; refusée (erreur du dialogue, IMP-0235) si l'état n'est pas *suspendu*, ou si le compteur des lignes entrées a avancé depuis l'arrêt (LSE15 §9-2) ;
  - `POURSUIVRE JUSQU'EN n` : comme CONTINUER, avec `n` pour nouvelle ligne d'arrêt ;
  - `REPRENDRE A PARTIR DE n1{,n2}` (T1600) ou `REPRENDRE EN n1{,n2}` (MITRA 15) : reprise au niveau principal, à la ligne `n1`, sans changer les identificateurs ni leurs valeurs ; admise après une erreur ou un `TERMINER`, refusée si l'état est *aucun* ou si le compteur des lignes entrées a avancé (« excepté par la commande EFFACER », que le compteur ne compte pas, IMP-0207). La reprise au niveau principal demande une opération nouvelle du contexte, `contexte_reprendre(n1, n2, &pos)`, qui dépile les cadres jusqu'au cadre racine en restaurant leurs identificateurs `LOCAL` (État d'exécution §5.4), vide la pile d'évaluation et les contextes de boucle du cadre racine, puis se positionne en `n1` ; elle complète le point laissé ouvert par État d'exécution §7.1 et §12 ;
  - `PAS A PAS` met le moniteur en mode pas à pas : l'interpréteur s'arrête à chaque arrivée sur une ligne (nouvelle fin d'exécution `FIN_PAS`, ajoutée à IMP-0214). Le retour sur la ligne `FAIRE` au début de chaque itération compte comme une arrivée sur cette ligne : une boucle s'arrête donc sur sa ligne `FAIRE` à chaque itération, qu'elle tienne sur une ligne ou sur plusieurs (constaté sur LSE 8086 le 2026-10-30). La ligne d'arrêt `n2` et ESC (IMP-0239) sont contrôlées aux mêmes points d'arrivée. Le moniteur affiche `LIGNE lll`, numéro de la ligne atteinte sur trois chiffres (point M3, constaté sur LSE 8086 : `LIGNE 10`) ; une entrée vide terminée par X-OFF relance l'exécution, une entrée vide terminée par RETURN ne fait rien (LSE15 §9-2 ; constaté sur LSE 8086, point L2) ; toute autre entrée est traitée normalement, et CONTINUER relance ensuite l'exécution (LSE15 §9-2) ; `NORMAL` annule le mode.
- Alternatives envisagées : réaliser REPRENDRE par EXECUTER sans réinitialiser l'environnement — écartée : les cadres d'une procédure interrompue resteraient actifs, contrairement à « toujours reprise au niveau principal ».
- Justification : INRDP75 §4-1, §4-3 ; LSE15 §9-2 ; État d'exécution IMP-0153, §7.4 ; Programme courant IMP-0207 ; choix d'Éric du 2026-10-24 (point K8) ; essai LSE 8086 du 2026-10-24 (`CONTINUER` après `TERMINER` : refus) ; essais LSE 8086 du 2026-10-30 (pas à pas dans des boucles `FAIRE` sur une et sur deux lignes : arrêt sur la ligne `FAIRE` à chaque itération).
- Impact mémoire : quelques octets d'état. Impact disque : sans objet. Impact performances : en mode pas à pas, un retour au moniteur par ligne et par itération.
- Impact historique : reproduit les commandes et leurs conditions telles que LSE15 les décrit.
- Références normatives : LSE15 §9-2 ; INRDP75 §4-3 ; ALG-1101, ALG-1602 ; EXI-0029.
- Statut : retenu (Éric, 2026-10-24, points K8, M1, M3, M5) ; révisé le 2026-10-30 (Éric ; audit du code, AC-03 : arrivée sur la ligne `FAIRE` à chaque itération).

**IMP-0233** — *Messages de fin d'exécution ; machine de bureau*
- Sujet : que restitue le moniteur à la fin d'une exécution, et comment traite-t-il une ligne de machine de bureau ?
- Choix :
  - `TERMINER` : `TERMINE EN LIGNE nnn` sous T1600, le numéro sur trois chiffres ; `TERMINE` seul sous MITRA 15 (INRDP75 PG1, PG3 ; sorties de Victor Duruy) ;
  - `PAUSE` : `PAUSE EN LIGNE nnn` ;
  - interruption par ESC : `PRET EN LIGNE nnn`, `nnn` étant la ligne en cours ; ESC en état moniteur : `PRET` (INRDP75 §3-2) ;
  - erreur : le message du Gestionnaire des erreurs (`erreurs_formater_execution`) ; arrêt dans une procédure : la ligne d'appel n'est pas donnée, conformément à IMP-0186 (point M2 ; constaté sur LSE 8086 : `En Ligne 40 : erreur d'Exécution 40`, ligne 40 dans la procédure, sans la ligne d'appel 10) ;
  - ligne d'arrêt `n2` atteinte : `ARRET EN LIGNE lll`, l'exécution étant *suspendue* (point M1 ; constaté sur LSE 8086 : `ARRET EN LIGNE 30` ; LSE15 §9-2 : « en imprimant ce numéro n2 ») ;
  - les numéros de ligne de ces messages s'écrivent sur trois chiffres, complétés de zéros, sous les deux profils, comme ceux des messages d'erreur (IMP-0186) ;
  - chaque message commence sur une ligne neuve : le moniteur passe à la ligne si la sortie n'est pas en début de ligne, et, après son message, appelle `interprete_debut_de_ligne(1)` (IMP-0221) ;
  - **machine de bureau** : la ligne compilée est exécutée par `interprete_bureau` dans le cadre courant, que le programme soit suspendu ou non (INRDP75, 2ᵉ partie §1 : « obtenir la valeur de variables » ; LSE15 §9-1 : « demander la valeur des variables du programme en cours d'exécution ») ; elle ne change pas l'état du programme : la pile d'évaluation est ramenée à sa hauteur d'entrée et l'état de lecture du programme est rétabli à la sortie de la ligne, quelle qu'en soit la fin, erreur et ESC comprises (Interpréteur IMP-0222 ; audit du code, AC-02, AC-13, AC-14) ; une erreur donne le message `EN LIGNE BUR` du Gestionnaire des erreurs.
- Alternatives envisagées : `TERMINE EN LIGNE` sous les deux profils — écartée (point K5) : INRDP75 attribue la forme sans numéro au MITRA 15.
- Justification : INRDP75 §3-2, PG1 à PG5 ; sorties de Victor Duruy ; choix d'Éric du 2026-10-24 (point K5).
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : nul.
- Impact historique : reproduit les messages attestés.
- Références normatives : INRDP75 §3-2 ; Gestionnaire des erreurs §4, §9 ; Interpréteur IMP-0214.
- Statut : retenu (Éric, 2026-10-24, points K5, M1, M2) ; complété le 2026-10-30 (audit du code, AC-02, AC-13, AC-14 : pile et lecture du programme rendues après une ligne de machine de bureau).

---

## 6 bis. Saisie pendant l'exécution

**IMP-0237** — *L'attente d'une réponse à LIRE est une fin d'exécution `FIN_LECTURE`, invisible pour l'utilisateur*
- Sujet : quand un programme attend une réponse à `LIRE`, qui lit la ligne, et dans quel état est l'exécution ?
- Choix :
  - pour l'utilisateur, la console reste dans l'état « exécution » (INRDP75 §3-2 ; LSE15 §9-1 : l'utilisateur « dialogue avec son programme ») : aucun message, la ligne tapée est une donnée et jamais une commande, la machine de bureau n'est pas disponible, la validation de la saisie suffit à poursuivre ;
  - pour le système, la tâche est en attente d'entrée, état distinct de la suspension (INRDP75 §3-1 : une tâche cesse d'être active sur « une lecture d'une ligne ou d'une donnée tapée par l'utilisateur », diagramme « Attente d'Entrée/Sortie ») ; le registre d'état du moniteur (IMP-0232) gagne l'état *en attente d'entrée*, que CONTINUER, REPRENDRE et les autres commandes ne voient jamais, puisque l'utilisateur n'a pas la main ;
  - quand `LIRE` a besoin d'une ligne, l'interpréteur rend la fin d'exécution `FIN_LECTURE` (IMP-0214), la position désignant l'élément de lecture en cours ; la pile, les cadres, les boucles et l'état de la lecture en cours (valeurs déjà lues, ligne en cours et position dans cette ligne) restent dans l'état d'exécution ;
  - le moniteur lit la ligne par sa routine unique de saisie (correction `\`, écho, SILENCE, ESC : IMP-0231, IMP-0238, IMP-0239), la fournit à l'interpréteur par `interprete_fournir_saisie`, puis relance `interprete_executer` à la position rendue ;
  - ESC pendant l'attente : `PRET EN LIGNE lll`, l'exécution devient *suspendue* ; CONTINUER émet un bip et reprend sur la valeur en attente, les valeurs déjà lues étant conservées (constaté sur LSE 8086 : après ESC pendant `LIRE A,B`, la saisie de A faite, CONTINUER émet un bip et ne demande que B, point L1).
- Alternatives envisagées : laisser l'interpréteur lire la console au milieu de l'instruction — écartée : la saisie contournerait l'environnement conversationnel, qui reçoit les entrées de l'utilisateur (VM-2201) ; ESC pendant la lecture ne pourrait pas suspendre l'exécution en conservant la valeur en attente ; une tâche en attente d'entrée ne pourrait pas être conservée (VM-2105, SWAP OUT).
- Justification : INRDP75 §3-1, §3-2 ; LSE15 §9-1 ; Tome III VM-2201, VM-2105, VM-3006 (états minimaux) ; sorties de Victor Duruy (`PRET EN LIGNE 036`) ; essais LSE 8086 du 2026-10-24 ; choix d'Éric du 2026-10-24 (point L1).
- Impact mémoire : le tampon de la ligne en cours de lecture (256 octets) passe dans l'état d'exécution. Impact disque : sans objet. Impact performances : un retour au moniteur par ligne saisie.
- Impact historique : reproduit le dialogue avec le programme et la reprise après interruption pendant une lecture.
- Références normatives : INRDP75 §3-1, §3-2 ; LSE15 §9-1, §9-2 ; VM-2201, VM-2105, VM-3006 ; Interpréteur IMP-0214, IMP-0222.
- Statut : retenu (Éric, 2026-10-24, point L1).

**IMP-0238** — *X-OFF et RETURN : deux touches de fin de saisie, de sens différents*
- Sujet : quelles touches terminent une saisie, et se distinguent-elles ?
- Choix :
  - `console_lire_ligne` rend la touche qui a terminé la saisie : `SAISIE_XOFF`, `SAISIE_RETURN` ou `SAISIE_ESC` (IMP-0220) ; sur l'Apple IIGS, X-OFF se frappe Ctrl-S, comme sur la console historique (étape 6c) ;
  - sous `LSE F4`, RUB-OUT (`$7F`, touche DELETE de l'Apple IIGS) termine la saisie exactement comme X-OFF, ainsi que l'annonce la bannière (`RUB-ØUT REMPLACE CTRL X-ØFF`, IMP-0266) ; Ctrl-S reste valable, rien n'établissant qu'il ait cessé de l'être, et `CTRL XØFF` figurant dans le texte de programmes de Victor Duruy (`ANGLO` 159, `DE31` 14) ; sous T1600 version 4 et sous MITRA 15, cette touche reste sans effet ;
  - X-OFF et RETURN valident tous deux une commande, ses paramètres, une ligne de programme et une ligne de machine de bureau ;
  - ligne vide : terminée par X-OFF, `????` ; terminée par RETURN, aucun effet ; en PAS A PAS, X-OFF relance l'exécution, RETURN ne fait rien (IMP-0229, IMP-0232) ;
  - réponse à `LIRE` : RETURN valide la saisie d'un nombre ; pendant la saisie d'une chaîne, RETURN est rangé dans la chaîne (CR LF) et termine la valeur (IMP-0222 ; INRDP75 PG3) ;
  - écho : RETURN passe à la ligne ; X-OFF ne déplace pas le curseur (INRDP75 PG3 : `4 PLUS⊗5 EGALE⊗EH OUI!⊗` sur une seule ligne) ; après X-OFF, la sortie n'est donc pas en début de ligne (IMP-0221).
- Alternatives envisagées : RETURN synonyme de X-OFF partout — écartée (points L2, L3) : LSE 8086 les distingue, et INRDP75 PG3 range RETURN dans une chaîne lue ; accumuler les morceaux terminés par RETURN jusqu'à X-OFF (NOYELLE79) — écartée (point L7) : NOYELLE79 décrit une version de LSE hors des deux profils retenus, et PG3 termine la valeur au RETURN.
- Justification : INRDP75 PG3 ; LSE15 §9-1, §9-2 ; essais LSE 8086 du 2026-10-24 ; choix d'Éric du 2026-10-24 (points L2, L3, L7) ; bannière de `LSE F4` (`MORSE`, `MULT`, `NBOR`), point 8 de la liste du 2026-11-01.
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : nul.
- Impact historique : rétablit la distinction historique entre X-OFF et RETURN.
- Références normatives : INRDP75 §1-3, PG3 ; LSE15 §9-1, §9-2.
- Statut : retenu (Éric, 2026-10-24, points L2, L3).

---

## 7. Commandes de listage et d'édition

**IMP-0234** — *LISTER, NUMERO, EFFACER*
- Sujet : comment le moniteur réalise-t-il les commandes qui lisent ou modifient le programme courant sans l'exécuter ?
- Choix :
  - `LISTER A PARTIR DE n1{,n2}` : chaque ligne de numéro compris entre `n1` et `n2` (ou au-delà de `n1`), dans l'ordre des numéros, est restituée par le décompilateur (IV-IF-0004) et écrite sur une ligne ; ESC arrête le listage (`PRET`) ;
  - `NUMERO A PARTIR DE n1{,n2}` : mêmes lignes, numéros seuls, chacun cadré à droite dans un champ de huit colonnes, dix par ligne de 80 colonnes (point M4 ; constaté sur LSE 8086 : `      10      20      30      40      50`) ;
  - `EFFACER LIGNES` : liste de numéros et d'intervalles `n1 A n2` séparés par des virgules (LSE15 : `7,12 A 31,40`), ou `*` pour tout le programme ; chaque ligne désignée est supprimée par `programme_supprimer` ; `*` appelle `programme_vider` et `contexte_vider`, et l'état du programme devient *aucun* ; un numéro absent n'est pas une erreur ;
  - un paramètre mal formé (`LISTER A PARTIR DE` sans numéro, numéro hors des bornes de numéro de ligne du profil) est une erreur du dialogue (IMP-0235), attestée à Victor Duruy.
- Alternatives envisagées : signaler un numéro absent d'EFFACER — écartée : aucune source ne le prévoit, et LSE15 donne `EFFACER LIGNES 33,27,142` sans condition.
- Justification : INRDP75 §4-1, §4-2 ; LSE15 §9-2 ; sorties de Victor Duruy (`LISTER A PARTIR DE` sans numéro : `????`).
- Impact mémoire : un tampon de ligne décompilée. Impact disque : sans objet. Impact performances : nul.
- Impact historique : reproduit les commandes.
- Références normatives : INRDP75 §4-1, §4-2 ; LSE15 §9-2 ; IV-IF-0004 ; Programme courant IMP-0205.
- Statut : retenu (Éric, 2026-10-24, découpage de l'étape 6, point M4).

---

## 7 bis. Éditeur de ligne

**IMP-0267** — *Éditeur de ligne : `[n]X[p]/groupe/remplacement`*
- Sujet : correction d'une ligne refusée ou listée sans la retaper. INRDP75 et LSE15 n'en parlent pas ; LAFOND83 (p. 133-134) et MEMENTO82 (§4) la décrivent ; Éric atteste son existence sur le T1600 de Victor Duruy.
- Choix :
  - **profils** : les deux (R1) ;
  - **reconnaissance** : une entrée qui commence par `/` ou `%`, ou par `[n]X[p]` suivi de `/` ou `%`, est une correction ; elle l'emporte sur une ligne de programme (`3X2/A/B` commence par un chiffre) et sur la machine de bureau, aucune instruction valide n'ayant cette forme (R2) ;
  - **ligne visée** : la plus récente de la dernière ligne refusée (texte tapé après correction par `\`, en programme ou en machine de bureau, ou ligne fautive d'ENCODER) et de la dernière ligne affichée par LISTER ou PERFORER (texte décompilé) ; sans ligne visée, `????` ; une session neuve n'en a pas (R3) ;
  - **remplacement** : le groupe est délimité par le signe initial (`%` permet un groupe contenant `/`, et inversement) ; à partir de l'occurrence p (1 par défaut), au plus n occurrences (1 par défaut) sont remplacées, de gauche à droite, sans revenir sur le texte inséré ; les blancs comptent ; le remplacement peut être vide ; un groupe vide ou une forme incomplète donne `????` ; un groupe introuvable, ou l'absence d'occurrence p, reste sans effet et sans affichage (MEMENTO82) (R4) ;
  - **effet** : la ligne corrigée est affichée (LAFOND83 : `/F/FF` puis `10 AFFICHER A`), puis traitée comme tapée : refusée, elle devient la ligne visée ; avec un autre numéro, elle crée une nouvelle ligne et l'ancienne reste (MEMENTO82) ; sur un programme secret, F 06 (R5) ;
  - X-OFF et RETURN valent fin de message (R6).
- Alternatives envisagées : la seule ligne refusée (LAFOND83) — écartée : MEMENTO82 vise aussi la dernière ligne listée, sans contredire LAFOND83 ; un seul profil — écartée : témoignage d'Éric pour le T1600, implantation voisine du MITRA 15 pour LAFOND83.
- Justification : LAFOND83 p. 133-134 (`/F/FF`, `X2/)/,3)`, `2X/)/,3)`) ; MEMENTO82 §4 (`3X2/A/B` : `1 ALLER EN AYAYA` donne `1 ALLER EN BYBYB`) ; témoignage d'Éric ; choix d'Éric du 2026-10-27 (R1 à R6). INRDP75 et LSE15 étant muets, ces sources comblent un silence sans contredire la norme.
- Impact mémoire : la ligne visée (256 octets). Impact disque : nul. Impact performances : nul. Impact historique : restitue un outil attesté sur des implantations voisines.
- Références normatives : VM-2201 ; IMP-0229 (classement), IMP-0231 (correction `\`).
- Statut : retenu (Éric, 2026-10-27).

**Tests.** `tests/composants/composants_moniteur.c` : sans ligne visée (`????`), `/F/FF` (LAFOND83), `X2/)/,3)` et `2X/)/,3)` (LAFOND83), `3X2/A/B` sur une ligne listée (MEMENTO82), groupe introuvable sans effet, groupe vide (`????`), changement de numéro (nouvelle ligne) ; bannières MITRA 15 et T1600.

---

## 8. Erreurs du dialogue

**IMP-0235** — *Natures du domaine `DIALOGUE`, restituées par `????`*
- Sujet : comment le moniteur signale-t-il une commande inconnue, un paramètre mal formé, ou une reprise refusée (points X-64 à X-66 de l'inventaire des diagnostics d'exécution) ?
- Choix :
  - le catalogue du Gestionnaire des erreurs reçoit un domaine `DIALOGUE` (Tome IV §17.1), avec six natures (Gestionnaire des erreurs §9 bis) : `ERR_D_COMMANDE_ERRONEE`, `ERR_D_PARAMETRE`, `ERR_D_LIGNE_DEPART_ABSENTE` (X-64, point M5), `ERR_D_CONTINUER_IMPOSSIBLE` (X-65), `ERR_D_REPRENDRE_IMPOSSIBLE` (X-66), `ERR_D_CONSOLE_INEXISTANTE` (IMP-0236) ; elles sont mises en forme par `erreurs_formater_dialogue` ;
  - toutes se restituent par la même ligne, `????` (point M7) ; avec l'option `-LIBELLES` (IMP-0186), leur libellé suit ;
  - une erreur du dialogue ne change ni le programme courant, ni l'état du programme (VM-2202 : « une entrée mal classée produit un diagnostic sans modification d'état ») ;
  - une commande refusée dans l'état courant n'est pas complétée : en état dormant (point M6), comme en session pour `BO`, qui répond `????` sur la ligne suivante, sans `NJOUR` et sans autre effet, sous tous les profils (`BØ`, `????`, 6EME - SPHIN, `numerisation0033.pdf` ; `BØ`, `???? (CØMMANDE ERRØNEE)`, 2NDE - PA1, `numerisation0019.pdf`, mars 1983 ; INRDP75 et LSE15 ne disent rien de ce cas).
- Alternatives envisagées : compléter `BO` en `BONJOUR`, puis répondre `????` — écartée : les deux sorties qui montrent ce cas, l'une antérieure à F4, l'autre postérieure, n'impriment pas `NJOUR` ; un code numéroté par erreur de commande — écartée : aucune attestation ne numérote une erreur de commande ; Victor Duruy et LSE 8086 répondent par des points d'interrogation.
- Justification : sorties de Victor Duruy ; essai LSE 8086 du 2026-10-24 (`???`) ; choix d'Éric du 2026-10-24 (point K8) ; Gestionnaire des erreurs §9 (renvoi des points X-64 à X-66 au présent chapitre).
- Impact mémoire : six entrées de catalogue. Impact disque : sans objet. Impact performances : nul.
- Impact historique : reproduit la réponse attestée.
- Références normatives : Tome IV §17.1 ; VM-2202 ; Gestionnaire des erreurs §4, §9.
- Statut : retenu (Éric, 2026-10-24, points K8, M5, M7 ; complété le 2026-11-04 : `BO` en session, point 9 de la liste des modifications vers le LSE de Victor Duruy).

---

## 9. Commandes différées et interruption

**IMP-0236** — *Commandes de fichiers, RUBAN, PERFORER, PRENDRE ETAT CONSOLE*
- Sujet : que font les commandes dont la réalisation dépend d'une étape ultérieure ou d'un matériel absent ?
- Choix :
  - les commandes de fichiers sont reconnues et complétées ; jusqu'à l'étape « fichiers », elles lisent leurs paramètres puis donnent l'erreur EX 96, « problème interne à LSE », comme les instructions de fichiers (IMP-0219) ;
  - `RUBAN` lira un programme depuis un fichier texte, chaque ligne étant traitée comme une entrée tapée ; `PERFORER` écrira le listage dans un fichier texte ; le fichier texte tient lieu de ruban perforé ; les deux sont réalisées à l'étape « fichiers » et donnent EX 96 jusque-là ;
  - `PRENDRE ETAT CONSOLE n` recopie la zone de swap de la console `n`, c'est-à-dire son programme courant, dans celle de la console qui la tape ; sous T1600, seul le programme passe, les variables étant non définies ; sous MITRA 15, les variables passent aussi ; la zone est blanchie par AU REVOIR (INRDP75 p. 26, p. 175, PG22, PG75, schéma p. 187). La commande suppose donc un autre utilisateur en session sur la console `n`. Le LSE-IIGS n'a qu'une console, numérotée 1 (Architecture physique, gestionnaire des tâches mono-console) : `n` = 1 ne change rien ; tout autre `n` est une erreur du dialogue (`????`).
  - Attestations : à Victor Duruy, la télétype est la console 1 (`CØNSØLE NØ. 01.`) ; une reprise réussie est silencieuse, `PRENDRE ETAT CØNSØLE 3`, `6`, `7` et `8` étant suivis directement de LISTER ou de PERFØRER (ANGLO V4, SPHIN `numerisation0003`, `0014`, `0033`, toutes antérieures à F4) ; `PRENDRE ETAT CØNSØLE 1`, suivi d'une ligne vide et de `PRET` (ANGLO V2, antérieure à F4), a un sens incertain et n'est pas reproduit.
  - Évolution envisagée par Éric (2026-11-04) : une version future pourrait faire fonctionner en parallèle, dans une seule instance de `LSE`, plusieurs consoles : la console intégrée de l'Apple IIGS, et un ou plusieurs émulateurs de terminal historique exécutés sur un Mac mini ou un PC et reliés au port série de l'Apple IIGS. `PRENDRE ETAT CONSOLE` y retrouverait son usage.
- Alternatives envisagées : supprimer RUBAN, PERFORER et PRENDRE — écartée : interdit de supprimer une fonctionnalité historique (CLAUDE.md, Interdictions) ; plusieurs consoles par une option `-CONSOLE n`, chacune avec une zone de swap sur disque réécrite à chaque demande d'entrée et partagée entre plusieurs instances de `LSE` lancées sous Golden Gate — écartée (choix d'Éric du 2026-11-04) : inutilisable sur l'Apple IIGS, qui ne fait tourner qu'un programme, et coûteuse à chaque ligne tapée ; l'évolution envisagée ci-dessus garde une seule instance.
- Justification : INRDP75 §4-1, §4-2 ; LSE15 §9-2 ; Architecture physique (configuration mono-console) ; choix d'Éric du 2026-10-24 (point K3).
- Impact mémoire : nul. Impact disque : à l'étape « fichiers ». Impact performances : nul.
- Impact historique : le ruban perforé devient un fichier texte.
- Références normatives : INRDP75 §4 ; LSE15 §9-2 ; Interpréteur IMP-0219.
- Statut : retenu (Éric, 2026-10-24, point K3) ; les commandes de fichiers sont réalisées par le chapitre Bibliothécaire et fichiers (IMP-0251 à IMP-0265 ; RUBAN et PERFORER : IMP-0262 à IMP-0264) ; la complétion `PRENDRE ETAT CONSOLE ` (sans `NO.`) est confirmée par Éric le 2026-10-27 d'après les sorties de Victor Duruy, INRDP75 imprimant `PRENDRE ETAT CONSOLE NO. 4` ; console unique confirmée par Éric le 2026-11-04 (point 10), après analyse des sources et des sorties.

**IMP-0279** — *Démarrage de `LSE F4` : date demandée au lancement, horloge du système*
- Sujet : les sorties de Victor Duruy montrent que la date et l'heure étaient données à la machine au démarrage (4EME - PRO, `FICH (incomplet) … numerisation0029.pdf`) : `LSE F4`, `DATE HEURES MINUTES : 00/00/00 00:00:00`, `EN LIGNE....`, puis `BØNJØUR  00:05:00`, et que l'heure comptait ensuite sans repasser à zéro (`BØNJØUR 00000000 28:26:42`, NBOR ; `BØNJØUR  00/00/00 36:11:12`, ANGLO V8A). L'Apple IIGS a une horloge toujours réglée : comment rendre ces formes possibles ?
- Choix :
  - **Séquence.** Sous `LSE F4`, au lancement, avant l'état dormant :

    ```text
    LSE F4

    DATE HEURES MINUTES : réponse
    EN LIGNE....
    ```

    `REDEMARRAGE SYSTEME` n'est pas imprimé : ce message annonce le redémarrage d'une machine en marche, et lancer `LSE` n'en est pas un.
  - **Réponses.** `jj/mm/aa hh:mm:ss`, ou `jj/mm/aa hh:mm`, d'après le libellé de la question. RETURN seul prend la date et l'heure de l'Apple IIGS : c'est une liberté, retenue pour ne pas avoir à les taper à chaque lancement, car sur la feuille une réponse vide fait reposer la question. Une réponse incorrecte fait reposer la question, précédée de `LSE F4`, comme sur la feuille.
  - **Horloge.** Après une réponse saisie, la date reste telle qu'elle a été donnée, et l'heure part de la valeur donnée et augmente sans repasser à zéro à 24 h. `horloge_lire` reste le seul point d'entrée : `BONJOUR`, `AU REVOIR`, `TEM`, `DAT`, l'amorce d'`ALE(0)` et les dates du catalogue la suivent. Aucune sortie ne montre le passage de minuit avec une date réglée, et cette règle unique est retenue ; l'heure imprimée garde deux chiffres, ceux de poids faible au-delà de 99 h.
  - **Date non réglée.** `00/00/00` laisse le champ date vide à `BONJOUR` et à `AU REVOIR` : `BONJOUR  00:05:00`, comme sur la feuille. `DAT` rend `00/00/00 hh:mm:ss`.
  - **`00000000`** n'est pas reproduit : MULT et NBOR l'attestent avec la bannière, mais aucune feuille ne montre comment on l'obtenait, et Éric n'en a pas le souvenir.
  - **Autres profils.** Le T1600 version 4 et le MITRA 15 n'ont pas cette séquence et gardent l'horloge de l'Apple IIGS.
- Alternatives envisagées :
  1. Garder l'horloge de l'Apple IIGS sans séquence — écarté : choix d'Éric ; ces formes resteraient impossibles à obtenir.
  2. Exiger une réponse, comme sur la feuille — écarté : la date serait à taper à chaque lancement alors que la machine la connaît.
  3. Imprimer `00000000` pour une date non réglée — écarté : la seule séquence attestée qui passe par une date nulle donne le champ vide.
  4. Faire repasser l'heure à zéro quand une date est donnée — écarté : aucune attestation, et une seconde règle.
- Justification : Tome IV, invariant 17 ; IMP-0276 (les sorties font foi pour F4) ; choix d'Éric du 2026-11-01 (points 7 et 11).
- Impact mémoire : un `InstantHorloge` et une origine. Impact disque : sans objet. Impact performances : nul. Impact historique : restitue le démarrage attesté, à la liberté de RETURN près.
- Tests : `composants_moniteur` (séquence, réponses, date nulle, autres profils), `composants_horloge`, `plateforme_horloge`.
- Références normatives : relevé T1600 de Victor Duruy, §2 ; IMP-0223, IMP-0224, IMP-0228, IMP-0266, IMP-0276.
- Statut : retenu (choix d'Éric du 2026-11-01).

**IMP-0266** — *Bannières de BONJOUR et d'AU REVOIR selon le profil*
- Sujet : que l'ordinateur imprime-t-il à l'ouverture et à la fermeture d'une session ?
- Choix :
  - **MITRA 15** : `BONJOUR`, puis sur la ligne suivante `LSE-MI15  CONSOLE NO.01  jj/mm/aa hh:mm:ss` ; `AU REVOIR  hh:mm:ss`, l'heure seule (INRDP75, annexe II, p. 189 : `LSE-MI15  CONSOLE NO.16  15/11/74 13:49:33`, `AU REVOIR  13:49:24`) ; la seule console du LSE-IIGS porte le numéro 01 ;
  - **T1600** : `BONJOUR  jj/mm/aa hh:mm:ss` et `AU REVOIR  jj/mm/aa hh:mm:ss` (INRDP75, p. 190 : `BONJOUR  00000000 01:31:37`, date non réglée ; §2 : `BONJOUR  30/08/74 11:28:49`).
  - **`LSE F4`** : `BONJOUR jj/mm/aa hh:mm:ss` et `AU REVOIR jj/mm/aa hh:mm:ss`, une seule espace devant la date (`BØNJØUR 15/10/80 08:36:17`, `MORSE` ; `BØNJØUR 00000000 28:26:42`, `NBOR`) ; elles sont suivies toutes deux des mêmes lignes, dans cet ordre : `CONSOLE NO. 01.`, `LSE F4`, `RUB-OUT REMPLACE CTRL X-OFF`, puis, si l'identification initiale n'est pas 0, `VOUS ETES SOUS ID c` (Bibliothécaire et fichiers IMP-0268), et enfin cinq lignes blanches. La seule console du LSE-IIGS porte le numéro 01.
- Alternatives envisagées : réserver ces lignes à `BONJOUR` — écartée : `MORSE` (trois fois) et `MULT` les impriment aussi après `AU REVØIR` ; les imprimer sous T1600 version 4 — écartée : elles sont absentes d'INRDP75 ; `LSE-M15` (INRDP75 p. 194) — écartée : `LSE-MI15` est la forme de la première attestation.
- Lignes blanches : `MORSE`, `MULT` et `NBOR` montrent six interlignes, mesurés sur les numérisations, soit cinq lignes blanches, entre `VØUS ETES SØUS ID 1` et la première commande de la session ; `NBOR` montre le même écart après la bannière d'`AU REVØIR`, avant `BØNJØUR`. Éric, auteur de ces sorties, n'a souvenir d'aucun geste qui l'expliquerait : elles font partie de la bannière.
- Date : le champ date est vide pour une date non réglée (`BONJOUR  00:05:00`, IMP-0279).
- Justification : INRDP75 annexe II p. 189, 190, 194 ; 4EME - MORSE, `MORSE_V2_numerisation0008.pdf` ; 5EME - MULT, `numerisation0005.pdf` ; choix d'Éric du 2026-10-27 (B1, B2) et du 2026-11-01 (point 6).
- Impact mémoire, disque, performances : nuls. Impact historique : reproduit la bannière attestée de chaque profil.
- Références normatives : IMP-0228.
- Statut : retenu (Éric, 2026-10-27 ; révisé le 2026-11-01 : bannière de `LSE F4`).

**IMP-0239** — *ESC seule interrompt ; l'interpréteur la consulte à chaque arrivée sur une ligne, retour sur la ligne `FAIRE` compris, le moniteur pendant toute saisie*
- Sujet : quelle touche interrompt l'exécution ou un listage, et quand est-elle vue ?
- Choix :
  - la touche d'interruption est ESC (INRDP75 §3-2) ;
  - l'interpréteur consulte la console à chaque arrivée sur une ligne, par une opération de l'interface console, `int console_interruption(void)`, qui rend 1 si ESC a été frappée depuis la dernière consultation. L'arrivée est le début d'une ligne, ou le retour sur la ligne `FAIRE` au début de chaque itération, qui compte comme une arrivée sur cette ligne ; une boucle tenue sur une seule ligne est donc interruptible. L'interpréteur rend alors la fin d'exécution `FIN_INTERRUPTION` (IMP-0214), la position désignant le point d'arrivée (début de la ligne, ou reprise de l'itération) ; le moniteur affiche `PRET EN LIGNE lll` (IMP-0233), et CONTINUER reprend à cette position ;
  - pendant une saisie, ESC termine la lecture : `console_lire_ligne` la rend comme touche de fin (IMP-0238) ; pendant l'attente d'une réponse à `LIRE`, l'exécution est alors suspendue avec `PRET EN LIGNE lll` (IMP-0237) ; dans le moniteur, ESC passe à la ligne et affiche `PRET` (constaté sur LSE 8086) ;
  - le moniteur consulte aussi `console_interruption` entre deux lignes d'un listage ;
  - sur l'Apple IIGS, la frappe est lue par le pilote `.CONSOLE` de GS/OS (IMP-0240) ; le bouchon de console simule la frappe d'ESC à un rang donné.
- Alternatives envisagées :
  1. Admettre aussi Ctrl-X, comme LSE 8086 — écartée (point L4) : aucune source historique ne l'atteste ; le message `RUB-ØUT REMPLACE CTRL X-ØFF` de Victor Duruy concerne une autre touche.
  2. Consulter la console au seul début d'une ligne — écartée : une boucle `FAIRE` tenue sur une seule ligne (forme attestée, LSE15 §5-5.5) ne serait jamais interrompue, alors que LSE 8086 l'interrompt (audit du code du 2026-10-29, AC-03 ; essai du 2026-10-30).
- Justification : INRDP75 §3-2 ; sorties de Victor Duruy (`PRET EN LIGNE 036`, « exécution interrompue pendant une lecture ») ; essais LSE 8086 du 2026-10-24 ; essais LSE 8086 du 2026-10-30 (boucle infinie d'une ligne interrompue par ESC ; pas à pas, IMP-0232) ; choix d'Éric du 2026-10-24 (points K4, L4) et du 2026-10-30.
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : une consultation du clavier par ligne exécutée et par itération, soit une lecture de `$C000` (0,017 ms, IMP-0242).
- Impact historique : reproduit l'interruption décrite par INRDP75.
- Références normatives : INRDP75 §3-2 ; LSE15 §5-5.5, §9-4.
- Statut : retenu (Éric, 2026-10-24, points K4, L4) ; révisé le 2026-10-30 (Éric ; audit du code, AC-03).

---

## 9 bis. Console de l'Apple IIGS (étape 6c)

**IMP-0240** — *La console passe par le pilote `.CONSOLE` de GS/OS, en lecture brute ; repli sur stdio sous Golden Gate*
- Sujet : comment `src/plateforme/console_iigs.c` réalise-t-il l'interface console (IMP-0220, IMP-0238, IMP-0239) sur l'Apple IIGS, sans la Toolbox (Conditionnement §19 ; points P1 à P6 retenus par Éric le 2026-10-24) ?
- Choix :
  - **Pilote.** Le pilote `.CONSOLE` de GS/OS, pilote de périphérique chargé par le système et non outil de la Toolbox, est désigné par `GetDevNumber` ; `console_ouvrir` (appelée par `main.c`) mémorise son mode de lecture (DStatus `$8005`) et son mode d'attente (DStatus `$0002`), le met en lecture brute (DControl SetReadMode `$8003`, valeur `$8000` : « no echo, no cursor, and no editing ») et fixe le mot de mouvement du curseur (code `$15`) à `$0D` : défilement, retour en fin de ligne et avance, mais `consLF` faux, de sorte que CR (`$0D`) ramène au début de la ligne sans interligne ; `console_fermer` rétablit l'état trouvé et le mot `$1F` (GS/OS Reference, chap. 9, « Screen control codes », « SetReadMode »).
  - **Écriture.** DWrite. `console_retour_chariot` écrit CR ; `console_interligne` écrit CR LF, l'interligne seul passant au début de la ligne suivante (IMP-0220) ; `console_bip` écrit BEL (`$07`, « Ring Bell »).
  - **Lecture d'une ligne.** Un caractère à la fois (DRead d'un octet, en mode d'attente) ; le moniteur fait l'écho des caractères imprimables (`$20` à `$7E`) ; RETURN (`$0D`) termine la saisie avec l'écho CR LF ; X-OFF (Ctrl-S, `$13`) et ESC (`$1B`) la terminent sans écho ; les autres touches sont sans effet, DELETE comprise hors de `LSE F4`, où elle vaut X-OFF (IMP-0238) : la correction reste `\` (IMP-0231, point P2) ; au-delà de la longueur maximale, un bip ; pas de conversion en majuscules (point P4). Pendant la saisie, la console affiche un curseur, pavé fixe (espace en vidéo inverse : codes `$0F`, `$20`, `$0E`, puis retour arrière `$08`), que l'écho de chaque caractère écrase et qui est redessiné après lui ; il est effacé à la fin de la saisie (la lecture brute n'affiche aucun curseur ; demande d'Éric du 2026-10-24, après essai sur l'Apple IIGS).
  - **Interruption.** `console_interruption` lit sans attendre (DControl SetWaitStatus `$0004`, valeur `$8000` ; « no-wait mode ») toutes les touches disponibles ; ESC rend 1 et abandonne la frappe d'avance ; toute autre touche est gardée, dans l'ordre, pour la lecture suivante (point P3).
  - **Repli.** Si `.CONSOLE` n'existe pas, la console passe par stdio : écriture par `fwrite`, CR seul sans effet (Golden Gate fait de CR un passage à la ligne), lecture par `getchar` avec l'écho du terminal de l'hôte, interruption jamais vue. Ce repli ne sert qu'au développement sous Golden Gate ; sous un terminal de l'hôte, Ctrl-S n'arrive au programme que si le contrôle de flux est désactivé (`stty -ixon`).
- Alternatives envisagées : le Text Tool Set — écartée : outil de la Toolbox (Conditionnement §19) ; la lecture directe des registres du clavier `$C000` et `$C010` — gardée en réserve (point P3) : elle court-circuiterait la frappe d'avance du pilote ; le mode d'entrée utilisateur du pilote (édition de ligne, terminateurs) — écartée : il ajoute des touches d'édition modernes et range le terminateur hors de la chaîne sans écho maîtrisé.
- Justification : GS/OS Reference, chap. 9 (`03-APPLE-IIGS/systeme/`) ; manuel ORCA/C (chap. 18, `toolerror`, `<gsos.h>`) ; points P1 à P6 ; sonde `tests/plateforme/sonde_console.c`, exécutée sous Golden Gate le 2026-10-24 : `GetDevNumber(".CONSOLE")` rend l'erreur `$0010` (`devNotFound`) : Golden Gate ne fournit pas le pilote.
- Impact mémoire : 64 octets de frappe d'avance. Impact disque : sans objet. Impact performances : trois appels au pilote par ligne exécutée, pour la consultation d'ESC.
- Impact historique : console de type télétype : CR et LF distincts, X-OFF, RETURN, ESC, écho sans édition moderne.
- Références normatives : IMP-0220, IMP-0231, IMP-0238, IMP-0239 ; GS/OS Reference, chap. 9.
- Vérification sur l'Apple IIGS (Éric, 2026-10-24) : la sonde rend $0000 pour tous les appels ; écho affiché, DELETE sans effet ; `BO` et Ctrl-S affiche `BONJOUR` et la date sur la même ligne, `BO` et RETURN sur la ligne suivante ; `AFFICHER['ABC',C,'X']` affiche `XBC` ; bip de LIRE ; ESC arrête l'exécution ; absence de curseur relevée, corrigée par le pavé ci-dessus.
- Statut : retenu (Éric, 2026-10-24, points P1 à P6) ; vérifié sur l'Apple IIGS le 2026-10-24, hormis le curseur et la frappe d'avance.
- Complément du 2026-10-30 (Éric ; audit du code, AC-26) : sous le repli stdio, la fin de l'entrée est rendue comme ESC, ce qui défait toute lecture en cours, et `console_fin_entree` la signale. Le moniteur clôt alors la session par un AU REVOIR implicite : message, programme oublié, temporaires vidés. Avant BONJOUR, il rend simplement la main. Avec le pilote `.CONSOLE`, l'entrée n'est jamais épuisée.

---

**IMP-0242** — *Consultation d'ESC : le registre du clavier `$C000` avant le pilote*
- Sujet : la consultation d'ESC à chaque arrivée sur une ligne (IMP-0239) par une lecture sans attente du pilote `.CONSOLE` coûte 0,52 ms, même quand aucune touche n'a été frappée (mesure du 2026-10-25 sur l'Apple IIGS, `05-IMPLEMENTATION/audits/Mesures_performance_IIGS_2026-10-25.md`).
- Choix : `console_interruption` lit d'abord le registre du clavier `$C000` (adresse `$E0C000`, lecture seule, `$C010` n'est jamais touché) ; si son bit 7 vaut 0, aucune touche n'attend et la fonction rend 0 sans appeler le pilote ; sinon, elle lit le pilote sans attendre, comme le décrit IMP-0240, ce qui consomme la touche. L'accès au registre reste confiné à `src/plateforme/console_iigs.c` et n'a lieu qu'avec le pilote ; le repli sur stdio n'est pas concerné.
- Alternatives envisagées : consulter le pilote une ligne sur N — écartée : ESC serait vu avec retard, et la frappe d'avance resterait coûteuse ; lire et effacer soi-même le registre (`$C010`) — écartée : la touche échapperait au pilote et à la frappe d'avance.
- Justification : mesures du 2026-10-25 (`SONDE` : lecture de `$C000` 0,017 ms, lecture sans attente du pilote 0,517 ms ; après la frappe de RETURN, `$C000` vaut `$0D`, bit 7 à 0 : le pilote efface l'indicateur en lisant la touche) ; manuel ORCA/C, chap. 18 (exemple `Keypress`, lecture de `$C000`) ; choix d'Éric du 2026-10-25.
- Impact mémoire : nul. Impact disque : sans objet. Impact performances : environ 0,5 ms gagnée par ligne exécutée.
- Impact historique : nul.
- Limite : si un logiciel résident prenait les touches sous interruption, l'indicateur pourrait être effacé avant la consultation et ESC serait manqué ; ce n'est pas le cas sous le shell ORCA en mode texte. Vérification : essai 7 (frappe d'avance puis ESC) sur l'Apple IIGS.
- Références normatives : IMP-0239, IMP-0240.
- Statut : retenu (Éric, 2026-10-25).

**IMP-0273** — *Une erreur d'exécution d'ORCA/C rend la console avant l'arrêt du programme*
- Sujet : une erreur d'exécution signalée par la bibliothèque d'ORCA/C arrête le programme sans repasser par `main`. C'est le cas d'un débordement de pile (erreur 15) ou d'une mémoire épuisée (erreur 5) (manuel ORCA/C 2.2, chap. 20, `SystemError`). La console reste alors dans l'état que lui donne `console_ouvrir` (IMP-0240) : lecture brute, CR sans LF. Sous le shell ORCA, la saisie suivante en souffre.
- Choix :
  - `pascal void SystemError(int e)`, que le manuel permet de remplacer par une routine du programme, est définie dans `src/plateforme/console_iigs.c`. Elle appelle `console_fermer()`, puis les deux routines de la bibliothèque : `SystemPrintError(e)`, qui imprime le message, et `SystemErrorLocation()`, qui arrête le programme.
  - `console_fermer` et `SystemError` sont les dernières fonctions du fichier, après `#pragma debug 0`. Elles n'ont pas de contrôle de pile et n'appellent aucune fonction qui en ait un. Appelées sur une pile épuisée, elles ne déclenchent donc pas un nouveau débordement.
  - La routine n'est liée que dans le programme `LSE` et dans la sonde `SONDEE` ; les suites de tests gardent celle de la bibliothèque.
- Alternatives envisagées :
  1. Rendre aussi la mémoire par `objets_terminer` (IMP-0270) — écartée : jusqu'à 510 appels toolbox sur une pile épuisée. Le système libère la mémoire de l'identifiant du programme à sa sortie (Apple IIGS Toolbox Reference, vol. 1, p. 12-11).
  2. Reprendre au moniteur par `longjmp` au lieu d'arrêter — écartée : il faudrait remettre en état les symboles en cours de création et les blocs verrouillés. Avec une pile de 16 Kio (Construction IMP-0272), un débordement n'est plus atteint par une ligne valide.
- Justification : manuel ORCA/C 2.2, chap. 20 (« By defining your own version of SystemError, you can intercept and handle run-time errors within your own program ») ; audit du code du 2026-10-29 (AC-12) ; choix d'Éric du 2026-10-30.
- Impact mémoire : nul. Impact disque : quelques dizaines d'octets de code. Impact performances : nul.
- Impact historique : neutre.
- Vérification : la sonde `SONDEE` (`tests/plateforme/sonde_erreur.c`, portée par l'image) ouvre la console comme LSE, puis provoque un débordement. Sur l'Apple IIGS, la console doit être normale au retour au shell. Sous Golden Gate, la sonde affiche « Stack overflow » et rend le code 255. Vérifié sur l'Apple IIGS par Éric le 2026-10-30 : message, « Stack overflow », retour au shell ; la frappe s'affiche et RETURN passe à la ligne suivante.
- Références normatives : IMP-0240, IMP-0270 ; Construction IMP-0143, IMP-0272 ; manuel ORCA/C 2.2, chap. 13 (`debug`), chap. 20 (`SystemError`, `SystemPrintError`, `SystemErrorLocation`).
- Statut : retenu (Éric, 2026-10-30).

**IMP-0281** — *À l'écran, `_` s'affiche `←` et `^` s'affiche `↑`*
- Sujet : le LSE note l'affectation `←` et la puissance `↑`. Les terminaux employés avec le LSE (SINTRA, ADM-3A, Tektronix, télétype) affichaient ces deux signes aux codes `$5F` et `$5E`, où l'écran de l'Apple IIGS affiche `_` et `^` (Éric, 2026-11-05). La télétype de Victor Duruy imprime `←` (relevé des sorties, conventions).
- Choix :
  - Avec le pilote `.CONSOLE` (IMP-0240), tout `$5F` écrit à l'écran s'affiche `←` et tout `$5E` s'affiche `↑`, sous les trois profils, sans option. Cela vaut pour tout ce qui passe par `console_ecrire` (messages, listages, sorties d'un programme, chaînes comprises) et pour l'écho de la frappe. Les touches `_` et `^` du clavier de l'Apple IIGS donnent donc une flèche à l'écran.
  - Les flèches sont les caractères MouseText `$48` (flèche vers la gauche) et `$4B` (flèche vers le haut). `ecrire_ecran` remplace `$5F` par `$C8` et `$5E` par `$CB`. En mode normal, le pilote affiche les octets `$C0` à `$DF` en MouseText, que MouseText soit activé ou non (GS/OS Reference, chap. 9, tableau 9-3). Le texte est copié dans un tampon de 96 octets, écrit d'un seul DWrite par tampon ; une écriture sans flèche passe directement au pilote.
  - La traduction n'existe qu'à l'écran. Le noyau, la représentation compilée, les fichiers-programmes, les fichiers-données, le ruban (PERFORER, IMP-0274) et les fichiers décodés gardent `$5F` et `$5E`. Le repli sur stdio écrit aussi le texte tel quel : il ne sert qu'au développement, et le vérificateur des exécutions compare des transcriptions où `←` est noté `_`.
- Alternatives envisagées :
  1. Encadrer chaque flèche de `$1B` (Enable MouseText Mapping) et `$18` (Disable) — première réalisation, abandonnée le 2026-11-05 : l'activation ne règle que l'affichage des majuscules en vidéo inverse (tableau 9-2), non celui de `$C0` à `$DF` en mode normal. L'essai 8.5 l'a montré : sous MITRA 15, `?EQC(200)` écrit `$C8` sans `$1B` et affiche la flèche gauche.
  2. Écrire `$0F`, `H` ou `K`, puis `$0E` (vidéo inverse le temps d'un caractère), MouseText activé — écartée : quatre octets de plus par flèche, pour le même résultat.
  3. Une option de lancement pour garder `_` et `^` — écartée : non demandée ; les terminaux historiques n'affichaient pas ces deux signes.
  4. Traduire aussi sous stdio, en UTF-8 — écartée : les transcriptions et les essais comparent des octets.
  5. Faire taper `_` et `^` par les touches flèches gauche (`$08`) et haute (`$0B`) du clavier de l'Apple IIGS — écartée (Éric, 2026-11-05) : la saisie continue de les ignorer.
- Justification : choix d'Éric du 2026-11-05 (affichage par défaut, quel que soit le profil) ; GS/OS Reference, chap. 9 (`03-APPLE-IIGS/systeme/`). La place de la flèche gauche (`$48`) et de la flèche haute (`$4B`) dans MouseText n'est donnée par aucun document versé : elle vient du jeu de caractères de l'Apple IIe amélioré (Apple IIe Technical Reference, non versé), et l'essai sur l'Apple IIGS l'a confirmée.
- Impact mémoire : 96 octets de tampon. Impact disque : quelques dizaines d'octets de code. Impact performances : nul pour une écriture sans flèche, hormis un parcours du texte ; une copie dans le tampon sinon, un DWrite par tampon de 96 octets.
- Impact historique : rétablit à l'écran l'aspect des terminaux de l'époque ; la frappe, la représentation et les fichiers sont inchangés.
- Vérification : sur l'Apple IIGS seulement, Golden Gate ne fournissant pas le pilote (essais de `05-IMPLEMENTATION/audits/Essais_IIGS_2026-11-05.md`, §8). Essais 8.1 à 8.4 et 8.6 conformes sur l'Apple IIGS le 2026-11-05, avec la première réalisation ; 8.5 a montré que `$C0` à `$DF` s'affichent toujours en MouseText (alternative 1). Les octets `$80` à `$FF` qu'un programme sort par `EQC` sous MITRA 15 étaient affichés par le pilote selon son tableau 9-3, indépendamment de cette décision : `$C0` à `$DF` en MouseText, les autres en vidéo inverse, `EQC(200)` donnant `←`. Ils vont désormais à la console réduits à leurs 7 bits (Bibliothécaire et fichiers, IMP-0282).
- Références normatives : IMP-0220, IMP-0240, IMP-0274, IMP-0275 ; GS/OS Reference, chap. 9.
- Statut : retenu (Éric, 2026-11-05) ; vérifié sur l'Apple IIGS le 2026-11-05 (essais 8.1 à 8.4 et 8.6 avec la première réalisation, 8.7 à 8.9 avec la réalisation à un octet).

---

## 10. Tests (étape 6b)

Suites réalisées le 2026-10-24, avec le bouchon de console (une réponse se termine par X-OFF, RETURN ou ESC) et le bouchon d'horloge, profils `dev` et `cible` :
- `composants_analyseur` (30 contrôles) : classement des entrées (`LIS` et `LISTER A PARTIR DE 1` en machine de bureau), état dormant, complétion selon le profil (`REPRENDRE EN` sous MITRA 15, `REPRENDRE A PARTIR DE` sous T1600), commandes propres à un profil ;
- `composants_moniteur` (24 contrôles) : sessions complètes, de BONJOUR à AU REVOIR : machine de bureau (`?38-24+47`, INRDP75), état dormant, PG1 d'INRDP75 sous T1600 (`TERMINE EN LIGNE 007`) et `TERMINE` sous MITRA 15, erreurs de compilation et du dialogue, LISTER, NUMERO, EFFACER, EXECUTER avec ligne d'arrêt, CONTINUER refusé après TERMINER ou après une ligne entrée, POURSUIVRE, REPRENDRE après une erreur dans une procédure, PAS A PAS relancé par X-OFF, ESC pendant LIRE puis CONTINUER (bip, seule B redemandée), ESC au début d'une ligne et dans le moniteur, correction `\`, ABREGER, PRENDRE ETAT CONSOLE, commande de fichier différée.

Règles de mise en page appliquées par le moniteur, en complément d'IMP-0230 et d'IMP-0238 :
- après X-OFF, la complétion d'une commande s'écrit à la suite des deux lettres ; après RETURN, dont l'écho a ouvert une ligne neuve, le moniteur écrit le nom entier de la commande ;
- une commande validée par X-OFF, ou dont les paramètres le sont, est suivie d'un passage à la ligne ; après le traitement d'une ligne de programme ou de machine de bureau, le moniteur repasse à la ligne si la sortie n'est pas en début de ligne, de sorte que chaque entrée commence une ligne (INRDP75 : chaque ligne de programme et chaque `?` sur sa ligne) ;
- une saisie vide terminée par X-OFF ou par ESC ne change pas la colonne.

---

## 11. Points tranchés après la première rédaction, point ouvert

Essais d'Éric sur LSE 8086 version IV, le 2026-10-24, et arbitrages du même jour. LSE 8086 accepte aussi les commandes tapées en entier et en minuscules (`exécuter à partir de 10 a 30`) ; cette tolérance propre à LSE 8086 n'est pas retenue (IMP-0229, IMP-0230).
- **M1 — Ligne d'arrêt atteinte.** LSE 8086 affiche `ARRET EN LIGNE 30`. Retenu : `ARRET EN LIGNE lll` (IMP-0233).
- **M2 — Arrêt dans une procédure.** LSE 8086 donne la ligne de l'erreur (40, dans la procédure), pas la ligne d'appel. Retenu : pas de ligne d'appel, déjà fixé par IMP-0186.
- **M3 — PAS A PAS.** LSE 8086 affiche `LIGNE 10` et attend. Retenu : `LIGNE lll` (IMP-0232).
- **M4 — NUMERO.** LSE 8086 aligne les numéros à droite dans des champs de huit colonnes, sur une même ligne. Retenu (IMP-0234), dix par ligne de 80 colonnes.
- **M5 — Ligne de départ absente.** Retenu : `????` (IMP-0235).
- **M6 — État dormant.** Retenu : `????` pour toute entrée autre que `BO` et les commandes de l'initialisateur. LSE 8086 répond `???` à `ap`, `nd`, `ch` et `co` en état dormant.
- **M7 — Forme du refus.** Retenu : `????` sous les deux profils ; `COMMANDE ERRONEE` seulement avec `-LIBELLES`.
- **L5 — Caractères rangés pour RETURN dans une chaîne lue.** INRDP75 (PG3) dit que « le retour à la ligne fait alors partie de la chaîne lue », et l'affichage de la chaîne passe à la ligne ; NOYELLE79 dit que RETURN « est mis dans la chaîne ». LSE 8086 ne range pas RETURN : il y termine la saisie (essai d'Éric du 2026-10-24, `LGR` = 2) et ne peut donc pas trancher. Retenu : deux caractères, CR et LF, qui reproduisent l'affichage de PG3 sur une console de type télétype (IMP-0222, IMP-0238).
- **L6 — Reste de la ligne après une instruction LIRE.** IMP-0222 (point G2) abandonnait ce qui reste de la ligne tapée à la fin de l'instruction `LIRE`. PG3 d'INRDP75 le contredit : après `2 LIRE C,A,D` et `3 LIRE B,E`, la saisie `4 PLUS⊗5 EGALE⊗EH OUI!⊗` donne à B la valeur `EGALE`, lue sur la ligne tapée pour l'instruction précédente. Essai d'Éric sur LSE 8086 le 2026-10-24 : avec `10 LIRE A;LIRE B`, la saisie `1 2` donne A = 1 et B = 2 ; un bip est émis au début de chaque instruction, celui de `LIRE B` dès la frappe de l'espace qui suit `1`. Retenu : le reste est conservé pour l'instruction `LIRE` suivante, qui émet son bip (IMP-0222). LSE 8086 lit caractère par caractère, le présent moniteur ligne par ligne : le second bip suit donc la frappe de la ligne entière au lieu de suivre l'espace.
- **L7 — RETURN dans une chaîne lue : fin de la valeur, ou suite de la saisie ?** NOYELLE79 accumule les morceaux terminés par RETURN jusqu'à X-OFF (« en terminant tous les morceaux (sauf le dernier) par RETURN, et le dernier par X-OFF »). Le second passage de PG3 d'INRDP75 montre autre chose : avec `4 PLUS⊗5 EGALE(RET)` puis `EH OUI!⊗`, l'affichage est `4 PLUS 5 EGALE`, puis `9 EH OUI!` à la ligne : B vaut `EGALE` suivi du passage à la ligne, et E vaut `EH OUI!`. NOYELLE79 décrit une version de LSE plus évoluée, aux structures absentes d'INRDP75 et de LSE15 et présentes dans LAFOND83 ; elle pourra faire l'objet d'un profil ultérieur. Retenu (Éric, 2026-10-24) : PG3 ; RETURN est rangé dans la chaîne (CR LF) et termine la valeur (IMP-0222, IMP-0238).
- **N1 — Commandes de l'initialisateur (tranché le 2026-10-25 : Bibliothécaire et fichiers, IMP-0254 et IMP-0257, d'après MEMENTO82 §3.6, pages versées le 2026-10-25).** En état dormant, LSE 8086 complète `li` en `liste ID :` et affiche `1,P,D,S,ETENDU` : la même paire de lettres désigne une autre commande qu'en état moniteur. MEMENTO82 range trois commandes « de l'initialisateur » : `CREATION ID`, `SUPPRESSION ID`, `LISTE ID` (signalement d'Éric ; pages non versées). INRDP75 §2-2 (5ᵉ partie) les suppose sans les nommer : les numéros d'identification et leurs pouvoirs sont définis « au moment de l'initialisation du système L.S.E. » (note 11). Ces commandes gèrent la liste des identifications, persistante, et relèvent de l'étape « fichiers » ; leur syntaxe, leurs réponses et le sens des pouvoirs affichés (`P`, `D`, `S`, `ETENDU`) restent à établir.

---

## 12. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0227 | `moniteur.c` (VM-2201) et `analyseur.c` (VM-2202), segment blanc, `moniteur_session` | retenu |
| IMP-0228 | Lancement dormant ; BONJOUR ; AU REVOIR rend la main au shell | retenu |
| IMP-0229 | Classement : ligne numérotée, deux lettres, machine de bureau | retenu |
| IMP-0230 | Deux lettres et X-OFF ou RETURN, complétion par profil, ABREGER, table des commandes | retenu |
| IMP-0231 | Correction `\` sur toute saisie ; SILENCE | retenu |
| IMP-0232 | Commandes d'exécution, état du programme, `contexte_reprendre`, `FIN_PAS` à chaque arrivée sur une ligne (retour sur la ligne `FAIRE` compris), `LIGNE lll` | révisé (2026-10-30) |
| IMP-0233 | Messages de fin d'exécution (`ARRET EN LIGNE lll`), machine de bureau dans le cadre courant | retenu |
| IMP-0234 | LISTER, NUMERO (champs de huit colonnes), EFFACER | retenu |
| IMP-0235 | Domaine `DIALOGUE`, restitué par `????` | retenu |
| IMP-0236 | Commandes de fichiers, RUBAN, PERFORER : EX 96 jusqu'à l'étape « fichiers » ; PRENDRE ETAT CONSOLE | retenu, provisoire |
| IMP-0237 | Attente d'entrée : `FIN_LECTURE`, saisie par le moniteur, reprise sur la valeur en attente avec un bip | retenu |
| IMP-0238 | X-OFF (Ctrl-S) et RETURN, touches de fin distinctes ; RETURN rangé dans une chaîne lue (CR LF) | retenu |
| IMP-0239 | ESC seule ; consultée à chaque arrivée sur une ligne (retour sur la ligne `FAIRE` compris) et pendant toute saisie | révisé (2026-10-30) |
| IMP-0240 | Console IIGS : pilote `.CONSOLE` en lecture brute, CR sans LF, curseur pavé, lecture sans attente pour ESC ; repli sur stdio sous Golden Gate | retenu ; vérifié sur l'Apple IIGS (sauf curseur) |
| IMP-0242 | ESC : registre du clavier `$C000` lu avant le pilote | retenu |
| IMP-0273 | Erreur d'exécution d'ORCA/C : `SystemError` rend la console avant l'arrêt | retenu |
| IMP-0281 | À l'écran, `_` s'affiche `←` et `^` s'affiche `↑` (MouseText `$C8`, `$CB`, pilote `.CONSOLE` seul), sous les trois profils | retenu ; vérifié sur l'Apple IIGS |
| IMP-0279 | Démarrage de `LSE F4` : `LSE F4`, `DATE HEURES MINUTES :`, `EN LIGNE....` ; heure sans retour à zéro ; `00/00/00` : champ date vide | retenu |
| IMP-0266 | Bannières : `LSE-MI15  CONSOLE NO.01` et AU REVOIR à l'heure seule (MITRA 15) ; date et heure (T1600) ; sous `LSE F4`, `CONSOLE NO. 01.`, `LSE F4`, `RUB-OUT REMPLACE CTRL X-OFF`, `VOUS ETES SOUS ID c` et cinq lignes blanches, après BONJOUR et AU REVOIR | retenu |
| IMP-0267 | Éditeur de ligne `[n]X[p]/groupe/remplacement`, ligne refusée ou listée, deux profils | retenu |

Amendements appliqués ailleurs : Interpréteur IMP-0214 (fins `FIN_PAS`, `FIN_INTERRUPTION`, `FIN_LECTURE` ; `interprete_fournir_saisie`, `interprete_pas_a_pas`, `interprete_reinitialiser` ; position de reprise d'`interprete_bureau`), IMP-0220 (`console_interruption`, touche de fin de `console_lire_ligne`), IMP-0222 (RETURN dans une chaîne, bip à la reprise) ; État d'exécution §7.1 (`contexte_reprendre`, `contexte_fixer_arret`) ; Gestionnaire des erreurs §9 bis (domaine `DIALOGUE`).

---

## 13. Traçabilité

| EXI | VM | IV | IMP | Code | Test |
|---|---|---|---|---|---|
| EXI-0028, EXI-0029, EXI-0051, EXI-0052 | VM-2201 | IV-IF-0001, IV-IF-0003 | IMP-0227, IMP-0228, IMP-0231, IMP-0232, IMP-0233 | `src/noyau/moniteur.c/.h` | `tests/composants/composants_moniteur.c` |
| EXI-0009, EXI-0028, EXI-0029, EXI-0052 | VM-2202 | — | IMP-0229, IMP-0230, IMP-0236 | `src/noyau/analyseur.c/.h` | `tests/composants/composants_analyseur.c` |
| EXI-0052 | VM-2201, VM-2202 | IV-IF-0004 | IMP-0234, IMP-0235 | `src/noyau/moniteur.c` | `tests/composants/composants_moniteur.c` |
| EXI-0028, EXI-0052 | VM-2201 | IV-IF-0001, IV-IF-0004 | IMP-0266, IMP-0267 | `src/noyau/moniteur.c` (bannières, `correction`, `viser`) | `tests/composants/composants_moniteur.c` |
