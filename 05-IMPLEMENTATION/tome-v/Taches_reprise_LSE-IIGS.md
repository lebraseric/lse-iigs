# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — Le gestionnaire des tâches et la reprise

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V rédigé le 2026-10-28 (étape 8 de la feuille de route, Conditionnement §17). Décision IMP-0269, proposée en prose et retenue par Éric le 2026-10-28 (points T1 à T6) ; IMP-0130 (Architecture physique §4) en est le point de départ.
**Portée :**
- la réalisation physique du gestionnaire des tâches (VM-2105) : la tâche, ses états, les transitions SUSPENDRE_TACHE, REACTIVER_TACHE, SWAP_OUT, SWAP_IN, et le module `src/noyau/taches.c/.h` ;
- le recensement des mécanismes, déjà réalisés par d'autres chapitres, qui satisfont la reprise d'une exécution interrompue (VM-2106, EXI-0060) ;
- la place de PRENDRE ETAT CONSOLE parmi les opérations sur les tâches.

Le chapitre ne couvre ni l'état d'exécution lui-même (chapitre État d'exécution), ni les commandes du moniteur (chapitre Moniteur et console), ni la conservation des fichiers-programmes (chapitre Bibliothécaire et fichiers), dont le Tome III distingue la conservation d'une tâche.
**Rang normatif :** subordonné au Tome III (§3.11, VM-2105, VM-2106), au Tome IV (IV-IF-0008, IV-IF-0009, ALG-1501 à ALG-1503, §15.5), aux EXI-0058, EXI-0059, EXI-0060, et à IMP-0130. Aucune disposition du présent chapitre ne peut contredire ces sources.

---

## 0. Avertissement de statut

Le corpus décrit le temps partagé sans en donner la réalisation :
- « Chaque tâche sera **alternativement** implantée dans une partie de la mémoire appelée "zone-données-utilisateur" et effectuée […] il faut conserver l'état de la tâche au moment où le système l'abandonne, pour que cet état puisse être restauré lorsque la tâche sera réactivée. C'est la raison d'être, sur le disque, de la **zone de Swap** » (INRDP75 §3-1) ;
- « Une tâche cesse d'être active lorsque le temps limite […] est atteint, ou si préalablement il y a eu une demande d'entrée-sortie : un affichage, une lecture d'une ligne ou d'une donnée tapée par l'utilisateur » (INRDP75 §3-1) ; le diagramme d'évolution d'une tâche enchaîne tâche active, SWAP OUT, attente d'entrée-sortie, fin d'entrée-sortie, tâches éligibles, SWAP IN ;
- EXI-0058 « ne fixe ni algorithme d'ordonnancement ni représentation physique de la zone de swap » ; le Tome III (VM-2105) : « La conservation d'une tâche est distincte de la conservation d'un fichier-programme ».

Le LSE-IIGS n'a qu'une console (Architecture physique, IMP-0130) : le temps partagé s'y réduit à une tâche, mais ses transitions restent présentes et traçables.

---

## 1. Sources

| Source | Apport |
|---|---|
| INRDP75 §3-1 | Moniteur de temps partagé, zone données-utilisateur, zone de swap, SWAP IN, SWAP OUT, fin d'activité à une demande d'entrée-sortie |
| INRDP75 §4-1, §4-3 ; LSE15 §9-2, §9-4 | Commandes de reprise (CONTINUER, POURSUIVRE, REPRENDRE), touche d'interruption |
| Tome III §3.11, §19.7 (VM-2105), §19.8 (VM-2106) | Gestionnaire des tâches, gestionnaire de reprise, interfaces |
| Tome IV §2.8, §15 | IV-IF-0008, IV-IF-0009, ALG-1501 à ALG-1503, reprise |
| Architecture physique §4 (IMP-0130) | Tâche unique, permanente, sans zone de swap ; module présent |

---

## 2. La tâche unique et ses états

**IMP-0269** — *Gestionnaire des tâches : tâche unique à trois états, transitions aux demandes d'entrée, module `taches.c`*
- Sujet : comment réaliser VM-2105 pour une seule console, dans le cadre d'IMP-0130, de sorte que les transitions historiques restent présentes et vérifiables ?
- Choix :
  - **tâche** : une seule, numéro 1, celle de la console 1 (`TACHE_UNIQUE`) ; elle comprend logiquement la session, le programme courant, l'état d'exécution et ce qu'il faut pour la reprise (Tome III §3.11), qui restent dans leurs modules respectifs : la tâche ne les copie pas ;
  - **états** : *absente* (console dormante), *active*, *en attente d'entrée* ;
  - **création et destruction** : BONJOUR crée la tâche, active ; AU REVOIR la détruit (IMP-0130) ;
  - **transitions** : à chaque demande d'entrée — ligne au moniteur, paramètre de commande, réponse à LIRE, ligne lue sur le ruban — le moniteur appelle `taches_suspendre` (SUSPENDRE_TACHE, puis SWAP_OUT, ALG-1502), et, la ligne reçue, `taches_reactiver` (REACTIVER_TACHE, puis SWAP_IN, ALG-1503 : la tâche unique est la seule éligible) ; c'est la fin d'activité « à une demande d'entrée-sortie » d'INRDP75 §3-1 ; suspendre et réactiver sont sans effet quand la tâche est absente ;
  - **SWAP_OUT, SWAP_IN** (IV-IF-0008, IV-IF-0009) : sans copie ; l'état de la tâche reste dans la zone données-utilisateur, qui ne sert qu'à elle (IMP-0130) ; l'invariant « la suspension ne détruit pas l'état d'exécution » est donc tenu par construction ; les transitions sont comptées, pour que les tests vérifient leur enchaînement ;
  - **temps limite** : sans objet ; aucune autre tâche n'attend le processeur ;
  - **module** : `src/noyau/taches.c/.h`, segment `"fichiers"` ; la table d'Organisation du dépôt §2.1, qui rattachait le gestionnaire des tâches à `fichiers.c`, est corrigée : la conservation d'une tâche est distincte de celle d'un fichier (Tome III, VM-2105).
- Alternatives envisagées : une vraie zone de swap sur disque, avec sérialisation de l'état (programme, pile, cadres, objets) — écartée : sans effet observable avec une seule console, déjà écartée par IMP-0130 ; une configuration à plusieurs consoles est envisagée pour plus tard, dans une seule instance de `LSE`, par le port série (Moniteur et console IMP-0236) ; les transitions dans `fichiers.c` — écartée : responsabilités distinctes.
- Justification : choix d'Éric du 2026-10-28 (T1 à T4) ; IMP-0130 ; INRDP75 §3-1.
- Impact mémoire : un octet d'état et deux compteurs. Impact disque : nul. Impact performances : deux appels par saisie. Impact historique : les transitions du temps partagé sont présentes aux points où INRDP75 les place.
- Références normatives : EXI-0058, EXI-0059 ; VM-2105 ; IV-IF-0008, IV-IF-0009 ; ALG-1501 à ALG-1503.
- Statut : retenu (Éric, 2026-10-28).

```text
          BONJOUR                  demande d'entrée
 absente ─────────► active ───────────────────────────► en attente d'entrée
    ▲               ▲    SUSPENDRE_TACHE → SWAP_OUT          │
    │               └─────────────────────────────────────────┘
    │                     ligne reçue : REACTIVER_TACHE → SWAP_IN
    └──────── AU REVOIR
```

---

## 3. La reprise (VM-2106, EXI-0060)

EXI-0060 : « Une exécution interrompue peut être reprise sans réinitialiser les valeurs acquises avant l'interruption ; la reprise peut, lorsque la commande le permet, être effectuée à partir d'une ligne choisie. » Le Tome III confie cette responsabilité au gestionnaire de reprise (VM-2106, interface `REPRENDRE(execution, position)`). Elle est entièrement réalisée par des décisions déjà prises, que le présent chapitre rassemble (point T5) :

| Mécanisme | Réalisation | Décision |
|---|---|---|
| Interruption d'une exécution par ESC, `PRET EN LIGNE lll` ; l'état d'exécution est conservé | `interprete_executer` rend `FIN_INTERRUPTION` ; le moniteur garde la position | Moniteur et console IMP-0239 |
| CONTINUER : reprise sur place, valeurs conservées | `executer` relancé à la position rendue | IMP-0232 |
| POURSUIVRE JUSQU'EN n : reprise avec une ligne d'arrêt | `contexte_fixer_arret` | IMP-0232 |
| REPRENDRE A PARTIR DE n : reprise à une ligne choisie, valeurs conservées | `contexte_reprendre` : `REPRENDRE(execution, position)` de VM-2106 | IMP-0232 |
| Reprise d'une réponse à LIRE interrompue, bip sur la valeur en attente | lecture reprenable, `interprete_fournir_saisie` | IMP-0237 |
| Refus de reprendre après qu'une ligne a été entrée (`????`) | compteur des lignes entrées à l'arrêt | Programme courant IMP-0207 ; Moniteur IMP-0232 |
| PAUSE, ARRET, PAS A PAS : arrêts reprenables | fins `FIN_PAUSE`, `FIN_ARRET`, `FIN_PAS` | IMP-0232, IMP-0233 |

La contrainte de VM-2106, « une modification de la position de départ ne détruit pas les autres éléments conservés de l'état », est tenue par `contexte_reprendre`, qui ne touche que la position.

---

## 4. PRENDRE ETAT CONSOLE (point T6)

`PRENDRE ETAT CONSOLE n` donne à la tâche de la console le programme courant de la console n (INRDP75 §4-1 ; annexe II §3, perforation depuis la télétype). C'est une opération sur les tâches : elle lit l'état conservé d'une autre tâche. Avec une seule console, n = 1 désigne la tâche elle-même et l'opération est sans effet ; tout autre n est une erreur du dialogue (Moniteur et console IMP-0236). La complétion `PRENDRE ETAT CONSOLE ` suit les sorties de Victor Duruy.

---

## 5. Tests

- `tests/composants/composants_taches.c` (16 contrôles) : tâche absente (suspendre et réactiver sans effet), création, SWAP OUT à la suspension, SWAP IN à la réactivation, transitions répétées sans effet, numéro de tâche autre que 1 ignoré, destruction.
- `tests/composants/composants_moniteur.c` : une session complète (programme, EXECUTER, réponse à LIRE, AU REVOIR) suspend la tâche à chacune des six demandes d'entrée et la réactive autant de fois ; la tâche est absente après AU REVOIR ; les sessions déjà présentes vérifient la reprise (§3).

---

## 6. Observation

Le Tome III numérote le gestionnaire des tâches VM-2105 (§19.7) et le gestionnaire de reprise VM-2106 (§19.8), réalisant les mécanismes VM-2105 (état d'une tâche en temps partagé) et VM-2106 (reprise avec conservation de l'état) du Tome II ; VM-2401 y désigne la séquence compilée. Le Tome IV trace la reprise par « EXI-0060 → VM-2106 » (§2.8 et §15.5) et le temps partagé par VM-2105 (tableau de correspondance). Les interfaces entre composants du Tome III (§21) portent les numéros VM-2501 à VM-2510 et ne se confondent pas avec ces composants.

---

## 7. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0269 | Tâche unique à trois états, transitions aux demandes d'entrée, SWAP sans copie, module `taches.c` | retenu |

---

## 8. Traçabilité

| EXI | VM | IV | IMP | Code | Test |
|---|---|---|---|---|---|
| EXI-0058, EXI-0059 | VM-2105 | IV-IF-0008, IV-IF-0009 | IMP-0130, IMP-0269 | `src/noyau/taches.c/.h`, `src/noyau/moniteur.c` (`lire_entree`, BONJOUR, AU REVOIR) | `tests/composants/composants_taches.c`, `tests/composants/composants_moniteur.c` |
| EXI-0060 | VM-2106 | — | IMP-0207, IMP-0232, IMP-0237, IMP-0239 | `src/noyau/contexte_exec.c` (`contexte_reprendre`, `contexte_fixer_arret`), `src/noyau/moniteur.c`, `src/noyau/interprete.c` | `tests/composants/composants_moniteur.c`, `tests/composants/composants_contexte.c` |
