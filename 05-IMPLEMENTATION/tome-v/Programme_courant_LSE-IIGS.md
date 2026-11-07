# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — Le programme courant : table des lignes compilées, déclarations de procédure

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V rédigé le 2026-10-23 (étape 4a de la feuille de route, Conditionnement §17). Décisions IMP-0204 à IMP-0207, proposées en prose et retenues par Éric le 2026-10-23 (points P1 à P5).
**Portée :** réalisation physique du gestionnaire du programme courant (module `src/noyau/programme.c/.h`, Organisation du dépôt §2.1) :
- la table des objets compilés et les recherches qu'elle offre ;
- l'insertion, le remplacement et la suppression d'une ligne (IV-IF-0002) ;
- la table des paramètres formels et la déclaration de procédure en vigueur (IMP-0147, IMP-0176) ;
- l'effacement du programme ;
- la trace des modifications.

Le chapitre ne couvre ni les commandes du moniteur qui appellent ces services (EFFACER, LISTER, APPELER…), ni la sérialisation du programme (RCI-S, bibliothécaire).
**Rang normatif :** subordonné au Tome III (VM-2203, VM-3204, §2.3.3, §9.4), au Tome IV (IV-IF-0002, ALG-1002), aux EXI-0002, EXI-0004, EXI-0006, EXI-0009, et aux chapitres RCI-M (§5, §6, IMP-0106, IMP-0109, IMP-0114, IMP-0125) et Compilateur (§12, §13 : IMP-0147, IMP-0170, IMP-0174, IMP-0176). Aucune disposition du présent chapitre ne peut contredire ces sources.

---

## 0. Avertissement de statut

Le corpus atteste le comportement du programme courant, sans décrire sa réalisation :
- une ligne remplace la ligne de même numéro : « Si on tape deux lignes portant le même numéro, la dernière ligne tapée, si elle est syntaxiquement correcte, prend la place de la précédente » (INRDP75, l. 1508) ;
- les lignes peuvent être effacées une à une ou toutes ensemble (`EFFACER LIGNES N1{,N2…}`, `N1 A N2`, `*` ; INRDP75, l. 521 ; LSE15 §9-2) ;
- les deux formes d'effacement ne libèrent pas la même chose : avec `EFFACER LIGNES 1 A 250`, « la commande EFFACER ne libère pas entièrement la zone-données-utilisateur » (INRDP75, note 16) ;
- une déclaration de procédure de plus grand numéro l'emporte (LSE15 §7-6, Note).

La structure de données et les algorithmes relèvent de décisions d'implémentation, prises ici ou déjà dans RCI-M et le chapitre Compilateur.

---

## 1. Rappel des contrats obligatoires (non renégociables)

| Source | Contenu | Conséquence |
|---|---|---|
| EXI-0002, EXI-0006 | Le programme est une suite de lignes identifiées par leur numéro, dans l'ordre des numéros | Table triée, numéros uniques (§2) |
| EXI-0004 | Une ligne rejetée ne modifie pas le programme courant | Le programme courant n'intègre que des lignes compilées avec succès ; une insertion qui échoue ne modifie rien (§3) |
| Tome IV IV-IF-0002 | Ajout si le numéro n'existe pas, remplacement sinon, retrait si la ligne est supprimée ; unicité, ordre, intégrité | §3 |
| Tome III §2.3.3, §9.4 (VM-3204) ; Compilateur IMP-0170 | Le compilateur remet son résultat ; le programme courant l'intègre | `programme_inserer` est appelé par le moniteur ou le bibliothécaire, jamais par le compilateur |
| RCI-M IMP-0106 | Recherche dichotomique sur table triée | §2 |
| RCI-M IMP-0109 | Une ligne de machine de bureau n'entre jamais dans le programme courant | `programme_inserer` ne reçoit que des lignes numérotées |
| Compilateur IMP-0147, IMP-0176 | Table des paramètres formels et `InfoProcedure` mis à jour par le programme courant à l'intégration d'une ligne (`INTEGRER_DECLARATION`) | §4 |
| Tome III §2.3.8, INV-06 | Le décompilateur et l'interpréteur lisent le programme courant sans le modifier | Les recherches rendent des copies d'entrées (§3) |

---

## 2. Table des lignes

**IMP-0204** — *Table des objets compilés : tableau trié de capacité fixe 255, alloué une seule fois*
- Sujet : comment ranger les `EntreeObjetCompile` (RCI-M §5) du programme courant ?
- Choix :
  - un tableau d'`EntreeObjetCompile`, trié par numéro de ligne croissant, sans doublon ;
  - capacité fixe de 255 entrées, soit le plus grand nombre de lignes que les numéros permettent (1 à 255 sous T1600, 1 à 250 sous MITRA 15 ; RCI-M IMP-0125, Compilateur IMP-0180) ;
  - le tableau est alloué par `malloc` une seule fois, à l'initialisation (`programme_initialiser`, appelée au lancement, et à défaut par la première insertion), puis conservé jusqu'à la fin de la session ;
  - la recherche est dichotomique (RCI-M IMP-0106) ; l'insertion et la suppression décalent les entrées suivantes.
- Alternatives envisagées : tableau agrandi par `realloc` — écartée : l'économie est d'un Kio au plus et chaque insertion aurait un cas d'échec de plus ; accès direct par le numéro (tableau indexé par 1 à 255) — écartée : contraire à IMP-0106, et le parcours ordonné y serait parsemé de trous.
- Justification : la borne est celle du langage, non un majorant arbitraire ; la structure est la plus simple qui respecte IMP-0106.
- Impact mémoire : 1 275 octets (255 × 5), plus les blocs des lignes (gestionnaire d'objets).
- Impact disque : sans objet.
- Impact performances : recherche en O(log n), insertion et suppression en O(n) octets décalés, n ≤ 255.
- Impact historique : neutre.
- Références normatives : RCI-M §5, IMP-0106, IMP-0125 ; EXI-0002, EXI-0006 ; Conditionnement §4.1.
- Statut : retenu (Éric, 2026-10-23, point P1).

---

## 3. Interface

**IMP-0205** — *Interface du programme courant ; le programme courant possède les blocs de ses lignes*
- Sujet : quelles fonctions réalisent IV-IF-0002 et les recherches dont l'interpréteur, le moniteur et le décompilateur ont besoin, et qui libère le bloc d'une ligne ?
- Choix :

```c
#define PROGRAMME_CAPACITE 255u

int  programme_initialiser(void);            /* 1 ; 0 si la mémoire manque */
/* IV-IF-0002 : ajout, ou remplacement de la ligne de même numéro, puis
   INTEGRER_DECLARATION (§4). Succès : 1 ; le programme courant possède le
   bloc ; l'ancien bloc est libéré. Échec : 0, *nature renseignée, rien n'est
   modifié et le bloc reste à l'appelant. */
int  programme_inserer(const EntreeObjetCompile *ligne, NatureErreur *nature);
/* EFFACER LIGNES : 1 si la ligne existait ; son bloc est libéré. */
int  programme_supprimer(unsigned char numero);
/* Recherches : 1 et une copie de l'entrée, 0 si aucune ligne ne convient. */
int  programme_chercher(unsigned char numero, EntreeObjetCompile *copie);
int  programme_premiere(EntreeObjetCompile *copie);
int  programme_suivante(unsigned char numero, EntreeObjetCompile *copie);  /* numéro > n */
int  programme_au_plus(unsigned char numero, EntreeObjetCompile *copie);   /* numéro ≤ n, le plus grand */
unsigned int programme_nombre(void);
int  programme_rang(unsigned int rang, EntreeObjetCompile *copie);         /* 0 ≤ rang < nombre */
/* Table des paramètres formels de la ligne de déclaration (IMP-0147) ; NULL
   si la ligne n'est pas une déclaration. Lecture seule. */
const TableParametresFormels *programme_parametres(unsigned char ligne_entree);
void programme_vider(void);                  /* §5 */
unsigned long programme_lignes_entrees(void);/* §6 */
```

  - Une recherche rend une **copie** de l'entrée, jamais un pointeur dans la table, qui se décale à chaque insertion.
  - Le bloc d'une ligne appartient au programme courant dès que `programme_inserer` réussit. Il est libéré par le remplacement de la ligne, par `programme_supprimer` ou par `programme_vider`.
  - Une insertion échoue dans deux cas : un numéro hors de 1 à 255 (erreur de l'appelant, jamais produite par le compilateur) ; le manque de mémoire pour l'entrée de la table des paramètres formels d'une déclaration. Dans le second cas, la nature est `ERR_C_PLUS_DE_PLACE` (C 57, « plus de place dans la zone utilisateur »), la même que si la compilation de la ligne avait manqué de place : pour l'utilisateur, c'est la ligne tapée qui n'a pas pu être prise.
  - L'entrée de la table des paramètres est préparée **avant** toute modification, ce qui garantit EXI-0004.
- Alternatives envisagées :
  1. Rendre un pointeur sur l'entrée — écartée : invalidé par le décalage suivant ; contraire au Conditionnement §19 (pointeurs C comme modèle).
  2. Laisser à l'appelant la libération de l'ancien bloc — écartée : l'appelant devrait chercher la ligne avant de l'insérer, et le remplacement ne serait plus une seule opération.
- Justification : IV-IF-0002 ; IMP-0170 (le compilateur remet, le programme courant intègre) ; EXI-0004.
- Impact mémoire : nul au-delà d'IMP-0204 et IMP-0147.
- Impact disque : sans objet.
- Impact performances : O(log n) par recherche.
- Impact historique : réalise le remplacement d'une ligne par une ligne de même numéro (INRDP75, l. 1508).
- Références normatives : Tome IV IV-IF-0002 ; Tome III §2.3.3, §9.4 ; EXI-0004 ; IMP-0170, IMP-0185 ; RCI-M §5.
- Statut : retenu (Éric, 2026-10-23, point P2).

---

## 4. Déclarations de procédure

Le programme courant réalise le contrat `INTEGRER_DECLARATION` d'IMP-0176 (chapitre Compilateur §13.5) à chaque insertion, remplacement ou suppression d'une ligne `L`.
- L'ancienne ligne, si elle commençait par l'élément `0xCD`, perd son entrée dans la table des paramètres formels.
- Si elle portait la déclaration en vigueur de sa procédure `P`, `InfoProcedure(P)` passe à la déclaration de `P` de plus grand numéro qui reste, ou à `{0, 0}`.
- La nouvelle ligne, si elle commence par `0xCD`, reçoit son entrée. Elle devient la déclaration en vigueur de sa procédure `Q` si son numéro dépasse celui de la déclaration en vigueur (LSE15 §7-6, Note).

La table des paramètres formels a la structure fixée par IMP-0147 :
- les entrées sont rangées dans un tableau agrandi par `realloc`, trié par `numero_ligne_entree` ;
- les listes de paramètres et d'identificateurs `LOCAL` non paramètres sont des blocs `malloc` propres à chaque entrée.

L'entrée est décodée de l'élément `0xCD` (fiche de codage §5, §6) : un paramètre est `PARAM_LOCAL` si son identifiant figure dans la liste `LOCAL`, `PARAM_REFERENCE` sinon. La liste des locaux non paramètres reprend la liste `LOCAL` privée des paramètres, dans son ordre. `InfoProcedure` est écrite par `symboles_definir_procedure` (RCI-M IMP-0114), seul endroit où une écriture dans la table des symboles est faite après la compilation.

---

## 5. Effacement

**IMP-0206** — *Deux niveaux d'effacement : une ligne, ou tout le programme courant avec ses tables*
- Sujet : que libère la suppression d'une ligne, et que libère l'effacement de tout le programme ?
- Choix :
  - **`programme_supprimer(n)`** (EFFACER LIGNES) :
    - retire la ligne et libère son bloc ;
    - si la ligne est une déclaration, retire son entrée de la table des paramètres formels et applique IMP-0176 ;
    - laisse les symboles et les constantes partagées que la ligne employait comme entrées inertes, encore désignables par les autres lignes et par l'environnement des identificateurs.
  - **`programme_vider()`** (EFFACER `*`, APPELER, ENCODER sous MITRA 15) :
    - libère toutes les lignes ;
    - vide la table des paramètres formels ;
    - vide la table des symboles et la table des constantes partagées (`symboles_vider`, RCI-M IMP-0107).

    L'appelant doit alors réinitialiser l'environnement des identificateurs, indexé par les identifiants de symbole (État d'exécution IMP-0179) ; la séquence exacte relève du chapitre du moniteur.
- Alternatives envisagées : vider les tables à chaque suppression de la dernière ligne qui emploie un symbole — écartée : exigerait un compte des emplois par symbole, sans attestation ; ne jamais vider les tables pendant la session — écartée : les symboles et les textes de programmes successifs s'accumuleraient.
- Justification : INRDP75 (note 16) distingue précisément ces deux niveaux : `EFFACER LIGNES 1 A 250` « ne libère pas entièrement la zone-données-utilisateur », au point de la saturer après quelques modules, alors que `EFFACER *` la libère. INRDP75 (l. 6704) range APPELER et l'encodage sous MITRA 15 parmi les opérations qui suppriment « totalement le programme courant antérieur ».
- Impact mémoire : les entrées inertes coûtent 10 octets par symbole et 3 octets plus le texte par constante, jusqu'au prochain effacement total.
- Impact disque : sans objet.
- Impact performances : nul.
- Impact historique : reproduit la distinction attestée par INRDP75 entre les deux formes d'EFFACER ; le message « zone données utilisateur pleine » n'est reproduit que si la mémoire est réellement épuisée.
- Références normatives : INRDP75, l. 521, l. 6704, note 16 ; LSE15 §9-2 ; RCI-M IMP-0107 ; IMP-0176, IMP-0179.
- Statut : retenu (Éric, 2026-10-23, point P3).

---

## 6. Trace des modifications

**IMP-0207** — *Compteur des lignes entrées, que la suppression ne fait pas avancer*
- Sujet : comment le moniteur sait-il qu'une ligne a été entrée depuis l'arrêt d'une exécution ?
- Choix : le programme courant tient un compteur (`unsigned long`) incrémenté par chaque insertion ou remplacement réussi. `programme_supprimer` et `programme_vider` ne le font pas avancer. Le moniteur mémorise sa valeur à l'arrêt d'une exécution et la compare à la reprise.
- Alternatives envisagées : un simple indicateur « modifié » — écartée : il faudrait que le moniteur le remette à zéro, et deux composants écriraient le même état ; compter aussi les suppressions — écartée : contraire à l'exception de LSE15 §9-2 pour EFFACER.
- Justification : LSE15 §9-2 fixe deux règles. CONTINUER est refusé « si depuis l'arrêt de l'exécution, on a entré une ligne de programme ». REPRENDRE EN est refusé « si l'on a modifié le programme (excepté par la commande EFFACER) ». Le compteur sert aussi la limite notée au chapitre État d'exécution (§12) : une chaîne de la pile qui désigne le texte d'une ligne remplacée pendant une suspension.
- Impact mémoire : 4 octets.
- Impact disque : sans objet.
- Impact performances : nul.
- Impact historique : rend réalisables les règles de CONTINUER et de REPRENDRE EN de LSE15 §9-2.
- Références normatives : LSE15 §9-2 ; État d'exécution §7.4, §12.
- Statut : retenu (Éric, 2026-10-23, point P4).

---

## 7. Segment

Le module occupe le segment statique `"programme"`, déjà prévu par Architecture physique §5.2 et Organisation du dépôt §2.1 (IMP-0135) ; son nom compte moins de 10 caractères (IMP-0197).

---

## 8. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0204 | Table des objets compilés : tableau trié de capacité fixe 255, alloué une seule fois | retenu |
| IMP-0205 | Interface du programme courant ; possession des blocs ; échec sans modification (C 57) | retenu |
| IMP-0206 | Deux niveaux d'effacement : une ligne, ou tout le programme avec ses tables | retenu |
| IMP-0207 | Compteur des lignes entrées, que la suppression ne fait pas avancer | retenu |

Le chapitre réalise en outre IMP-0147 et IMP-0176 (chapitre Compilateur §13), dont il ne modifie ni la structure ni le contrat.

---

## 9. Traçabilité

| EXI | VM | IV | IMP | Code | Test |
|---|---|---|---|---|---|
| EXI-0002, EXI-0004, EXI-0006, EXI-0042 | VM-2203, VM-3204, VM-1301 | IV-IF-0002 | IMP-0106, IMP-0147, IMP-0176, IMP-0204 à IMP-0207 | `src/noyau/programme.c`, `src/noyau/programme.h` | `tests/composants/composants_programme.c` |

---

## 10. Tests

Suite `tests/composants/composants_programme.c`, sous Golden Gate, lignes compilées par le compilateur réel :
- ajout de lignes dans le désordre : ordre croissant, unicité, `programme_nombre`, parcours par rang ;
- remplacement : l'ancien bloc est libéré (instrumentation par `objets_existe`) et le compteur avance ;
- suppression : le bloc est libéré, le compteur n'avance pas, une ligne absente rend 0 ;
- recherches `programme_chercher`, `programme_premiere`, `programme_suivante`, `programme_au_plus`, aux bords (avant la première ligne, après la dernière) ;
- déclaration : décodage de `20 PROCEDURE &PRO(A,B) LOCAL A,M,N` en `A` local, `B` référence, `M` et `N` locaux non paramètres ; `InfoProcedure(&PRO) = {20, 2}` ;
- LSE15 §7-6, Note : déclarations de `&P` en lignes 10 et 50 ; la ligne 50 est en vigueur ; sa suppression rend la ligne 10 en vigueur ; le remplacement de la ligne 10 par une ligne qui n'est pas une déclaration donne `{0, 0}` ;
- `programme_vider` : aucune ligne, aucun bloc, tables des symboles et des constantes vides ;
- refus d'un numéro 0, sans modification.
