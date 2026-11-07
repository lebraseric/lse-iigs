# Tome V — Spécification détaillée de l'implémentation Apple IIGS / ORCA-C

## Chapitre — Le bibliothécaire et les fichiers : bibliothèque, fichiers-programmes (RCI-S), fichiers-données

**Plateforme cible :** Apple IIGS / 65816 / 4 Mio RAM / 32 Mio disque
**Compilateur cible :** ORCA/C 2.2.1
**Statut du document :** chapitre du Tome V rédigé le 2026-10-25 (étape 7a de la feuille de route) ; étape 7b ajoutée le même jour (§7 bis, IMP-0251, IMP-0252 ; points B1 à B7) ; étape 7c ajoutée le même jour (§7 ter, IMP-0253 ; points C1 à C7). Décisions IMP-0245 à IMP-0250, sur le découpage 7a à 7e et les points F1, F2, A1 à A5 retenus par Éric le 2026-10-25 (A3 dans sa version « fichier en secteurs »).
**Portée :** organisation de la bibliothèque sur disque ; format persistant d'un fichier-programme (RCI-S) et d'un fichier-données ; catalogue des fichiers permanents ; interface entre le noyau et le système de fichiers ; classe d'erreurs F. Les commandes et instructions qui emploient ces formats sont réalisées aux étapes 7b (fichiers-programmes, EXECUTER), 7c (fichiers-données, GARER, CHARGER, SUPPRIMER, CATALOGUER), 7d (identification, TABLE DES FICHIERS, UTILISATION DISQUE, initialisateur) et 7e (ENCODER, DECODER, RUBAN, PERFORER).
**Rang normatif :** subordonné au Tome III (VM-2209, VM-3401 à VM-3407, §4.6), au Tome IV (IV-IF-0005 à IV-IF-0007, ANN-K) et aux chapitres RCI-M, Programme courant, Données et Gestionnaire des erreurs. Aucune disposition du présent chapitre ne peut contredire ces sources.

---

## 0. Avertissement de statut

Le Tome III fixe les responsabilités : le bibliothécaire conserve et restaure le programme sous sa forme compilée, sans texte source ni recompilation (VM-2209, VM-3401) ; « aucun format binaire particulier n'est imposé » ; une restauration échouée laisse le programme courant intact (VM-3403) ; le catalogue distingue fichier-programme et fichier-données, temporaire et permanent (VM-3404) ; un fichier-données est fait d'enregistrements numérotés, chacun une constante, un tableau ou une chaîne (VM-3405). Les manuels (INRDP75, 5ᵉ partie ; LSE15 §6-3, §9-3) fixent l'usage. Le présent chapitre fixe la réalisation sur l'Apple IIGS.

---

## 1. Sources

| Source | Apport |
|---|---|
| Tome III VM-2209, VM-3401 à VM-3403 | Conservation et restauration de la forme compilée ; remplacement atomique du programme courant |
| Tome III VM-3404, §4.6 | Catalogue ; espaces temporaire et permanent ; identité = catégorie et portée |
| Tome III VM-3405 à VM-3407 | Enregistrements, GARER, CHARGER, SUPPRIMER ; ENCODER, DECODER ; catalogage |
| INRDP75, 5ᵉ partie, §1 | Durées de vie (temporaire : la session ; fichier-programme : toujours permanent) ; secteur de 256 octets, « unité de base de stockage » ; enregistrements occupant des secteurs entiers ; nom d'une lettre suivie d'au plus 4 caractères alphanumériques ; priorité au fichier temporaire |
| INRDP75, 5ᵉ partie, §2 ; LSE15 §6-3, §9-3 | Instructions et commandes ; `'#…'` pour un fichier permanent ; protections `E` et `L` (T1600) ; TABLE DES FICHIERS, UTILISATION DISQUE |
| MEMENTO82, annexe III | Codes d'erreur « FICHIER » F 01 à F 29 |
| Sorties T1600 de Victor Duruy | `ERREUR F 03`, `ERREUR FL 06` |
| Manuel d'ORCA (§8, « Standard Prefixes ») | Le préfixe `@` vaut le préfixe d'ORCA.Sys16 sur disque dur, le dossier de travail en réseau |
| Sonde `tests/plateforme/sonde_fichiers.c` (2026-10-25, Golden Gate) | `CreateGS` et `DestroyGS` d'un répertoire, `fopen` en `wb` et `r+b`, `fseek`, ajout en fin, `rename`, `remove`, `GetDirEntryGS` : tous opérants ; un fichier écrit en binaire reçoit le type ProDOS BIN ($06) |

---

## 2. Organisation de la bibliothèque

**IMP-0245** — *Un répertoire de bibliothèque, désigné par `-BIB`, par défaut `@:LSE`*
- Sujet : où et comment ranger les fichiers-programmes, les fichiers-données permanents et temporaires, et leurs métadonnées ?
- Choix :
  - la bibliothèque est un répertoire ProDOS désigné au lancement par l'option `-BIB chemin` ; à défaut, `@:LSE` (le préfixe `@` est celui d'ORCA.Sys16 sur disque dur : manuel d'ORCA, §8) ;
  - elle contient trois sous-répertoires, `PROGRAMMES`, `DONNEES` (fichiers-données permanents) et `TEMPORAIRES`, et deux fichiers, `CATALOGUE` (IMP-0248) et `IDENTIFICATIONS` (IMP-0254) ;
  - au lancement, ce qui manque est créé (`CreateGS` de GS/OS : stdio ne crée pas de répertoire) ;
  - le LSE-IIGS n'ayant qu'une console (Moniteur IMP-0236), `TEMPORAIRES` est l'espace temporaire de la console 1 ; il est vidé à BONJOUR et à AU REVOIR (INRDP75, 5ᵉ partie, §1-1 : les fichiers temporaires « sont automatiquement détruits à la fin de la session ») ;
  - un nom de fichier L.S.E. (une lettre suivie d'au plus 4 caractères alphanumériques, INRDP75 §1-4 b) est un nom ProDOS valide et sert tel quel ; le `#` qui désigne un fichier permanent dans GARER et SUPPRIMER (INRDP75 §2-3) ne fait pas partie du nom rangé ;
  - CHARGER cherche d'abord dans `TEMPORAIRES`, puis dans `DONNEES` (« la priorité est alors donnée au fichier temporaire », INRDP75 §1-4 b).
- Alternatives envisagées : un seul fichier conteneur de toute la bibliothèque — écartée : il faudrait réécrire un système de fichiers ; ranger la bibliothèque à côté de l'exécutable — écartée (choix d'Éric du 2026-10-25) : `LSE` est installé dans le préfixe 17 des utilitaires d'ORCA.
- Justification : INRDP75, 5ᵉ partie, §1 ; Tome III §4.6 ; manuel d'ORCA, §8 ; choix d'Éric du 2026-10-25 (points F1, A1).
- Impact mémoire : nul. Impact disque : un répertoire et ses trois sous-répertoires. Impact performances : création au premier lancement seulement.
- Impact historique : la zone des fichiers temporaires et celle des fichiers permanents sont des répertoires.
- Références normatives : VM-3404 ; Tome III §4.6.
- Statut : retenu (Éric, 2026-10-25).

---

## 3. Fichier-programme : RCI-S

**IMP-0246** — *RCI-S : en-tête, table des symboles, constantes partagées, lignes RCI-M telles quelles, somme de contrôle*
- Sujet : quelle représentation persistante donner à un programme compilé (VM-3402), et comment la restaurer sans risque pour le programme courant (VM-3403) ?
- Choix : un fichier-programme est écrit d'un seul tenant par RANGER ou MODIFIER, et lu d'un seul tenant par APPELER ou EXECUTER. Les entiers sur deux octets sont rangés poids faible en tête, comme dans RCI-M.

| Partie | Contenu |
|---|---|
| En-tête (8 octets) | `L`, `S`, `E`, `P` ; version (1) ; profil (`PROFIL_MITRA15_V8`, `PROFIL_T1600_V4` ou `PROFIL_T1600_F4`) ; deux octets réservés à 0 |
| Symboles | nombre (u16) ; pour chacun, dans l'ordre des identifiants : longueur du nom (u8), nom sur 5 octets (complété de zéros), nature (u8), origine (u8) |
| Constantes partagées | nombre (u16) ; pour chacune, dans l'ordre des index : longueur (u8), texte |
| Lignes | nombre (u16) ; pour chacune, dans l'ordre des numéros : numéro (u8), longueur (u8), séquence RCI-M |
| Fin | somme de contrôle (u16) : somme modulo 65 536 de tous les octets qui précèdent |

  - Les séquences RCI-M désignent les symboles et les constantes par leur rang (RCI-M §6, §7) : les tables sont écrites et reconstruites dans le même ordre, les séquences sont copiées sans transformation. Les informations de procédure (`InfoProcedure`) ne sont pas rangées : le programme courant les rétablit à l'insertion des lignes (Programme courant IMP-0176).
  - **Restauration.** Le fichier entier est d'abord vérifié, sans toucher au programme courant : signature, version, profil égal à celui de la session, longueurs, somme de contrôle, numéros de ligne croissants dans les bornes du profil, chaque séquence parcourue élément par élément (`rcim_longueur_element`), chaque identifiant de symbole et chaque index de constante dans les tables du fichier. Ce n'est qu'ensuite que le programme courant et l'état d'exécution sont vidés, puis les tables et les lignes reconstruites. Un échec de vérification laisse le programme courant intact (VM-3403). Un manque de mémoire pendant la reconstruction laisse un programme courant vide, et non partiellement restauré, avec l'erreur F 23 « mémoire trop petite pour le programme » (MEMENTO82).
  - Un fichier rangé sous un autre profil est refusé (erreur F 100, IMP-0250) : il peut contenir des éléments propres à ce profil (Compilateur IMP-0180, IMP-0276).
- Alternatives envisagées : ranger le texte source et recompiler — écartée : contraire à VM-2209 (« ne recompile pas le texte lors d'une restauration ») ; renuméroter symboles et constantes à la restauration — écartée : il faudrait réécrire chaque séquence.
- Justification : VM-2209, VM-3401 à VM-3403 ; RCI-M §5 à §7 ; INRDP75 §1-4 a (fichier-programme « stocké sous forme binaire ») ; choix d'Éric du 2026-10-25 (point A2).
- Impact mémoire : un tampon de la taille du fichier pendant la lecture. Impact disque : environ la taille des séquences, plus 8 octets par symbole. Impact performances : une lecture, deux passes.
- Impact historique : nul ; le format physique n'est pas historique (VM-3402 : « aucun format matériel ou binaire historique n'est imposé »).
- Références normatives : VM-3402, VM-3403 ; RCI-M §5 à §7 ; Programme courant IMP-0176.
- Statut : retenu (Éric, 2026-10-25).
- Compléments du 2026-10-30 (Éric ; audit du code, AC-17, AP-12, AC-32) :
  - **Vérification.** Avant que le programme courant soit vidé, chaque entrée de symbole est contrôlée : nom de 1 à 5 caractères, une lettre puis des lettres ou des chiffres (Compilateur IMP-0191), complété de zéros ; nature et origine parmi les valeurs de RCI-M §6.2. Chaque constante a de 17 à 127 octets (IMP-0107, IMP-0128). Deux symboles de même nom et de même nature, ou deux constantes de même texte, rendent le fichier invalide, comme une table qui dépasse son plafond (6 553 symboles, 21 845 constantes ; RCI-M IMP-0104). Dans chaque séquence, tout identifiant de symbole et tout index de constante est comparé aux tables du fichier (`rcim_sequence_conforme`). Tout échec donne F 101 et laisse le programme courant intact.
  - **Doublons.** Ils sont cherchés par une table de hachage temporaire, construite pendant la vérification et rendue aussitôt : un passage linéaire sur chaque table du fichier, dans au plus 64 Kio (32 768 positions de 2 octets, la limite d'un tableau alloué en modèle small). IMP-0167 écarte une table de hachage pour la table des symboles du programme courant, consultée à chaque identificateur compilé ; la table temporaire ne sert qu'à vérifier un fichier entier, une fois par APPELER ou EXECUTER, là où une comparaison deux à deux coûterait un temps quadratique. Si elle ne peut être allouée, l'erreur est F 23, avant que le programme courant soit touché.
  - **Reconstruction.** L'unicité étant acquise, symboles et constantes sont ajoutés en fin de table sans recherche, dans l'ordre du fichier (`symboles_ajouter`, `constantes_ajouter`) ; les tables grandissent par tranches de 32 entrées (Compilateur IMP-0167).
  - Impact mémoire : la table temporaire, de 32 octets à 64 Kio selon la taille des tables du fichier. Impact performances : temps de restauration linéaire dans le nombre de symboles et de constantes.
- Complément du 2026-10-30 (Éric ; audit du code, §9 point 11) : les symboles et constantes que plus aucune ligne n'emploie (lignes remplacées ou effacées, machine de bureau) sont rangés et restaurés avec les autres ; ils ne sont pas purgés, une purge obligeant à renuméroter les identifiants et les index de toutes les séquences. Les plafonds des tables (RCI-M IMP-0104) bornent leur accumulation.

---

## 4. Fichier-données en secteurs

**IMP-0247** — *Fichier-données : secteurs de 256 octets, répertoire d'enregistrements, étendues réutilisées*
- Sujet : comment ranger les enregistrements d'un fichier-données (VM-3405) sans réécrire le fichier à chaque GARER ou SUPPRIMER ?
- Choix :
  - le fichier est une suite de **secteurs** de 256 octets, « unité de base de stockage » des systèmes historiques (INRDP75 §1-4 a) ; chaque enregistrement occupe un nombre entier de secteurs consécutifs, son **étendue** ; le nombre de secteurs d'un fichier est sa taille divisée par 256 ;
  - les **secteurs de répertoire** ont tous la même forme : un en-tête de 16 octets, puis 20 entrées de 12 octets ; le secteur 0 est le premier ; les suivants sont chaînés ;

| En-tête d'un secteur de répertoire (16 octets) | |
|---|---|
| octets 0 à 3 | `L`, `S`, `E`, `D` (secteur 0) ; zéros (secteurs suivants) |
| octet 4 | version (1) |
| octet 5 | réservé (0) |
| octets 6-7 | nombre de secteurs du fichier (secteur 0 seulement) |
| octets 8-9 | secteur de répertoire suivant (0 : aucun) |
| octets 10 à 15 | réservés (0) |

| Entrée de répertoire (12 octets) | |
|---|---|
| octets 0-1 | numéro d'enregistrement, de 1 à 32 767 (0 pour une entrée sans enregistrement) |
| octet 2 | état : 0 nombre, 1 tableau à une dimension, 2 tableau à deux dimensions, 3 chaîne (les valeurs de δ de LSE15 §6-3.4) ; `$FE` étendue libre ; `$FF` entrée inutilisée |
| octet 3 | réservé (0) |
| octets 4-5 | premier secteur de l'étendue |
| octets 6-7 | nombre de secteurs de l'étendue |
| octets 8 à 11 | longueur du contenu, en octets |

  - **contenu** d'un enregistrement : un nombre, ses 4 octets (`float`) ; un tableau, l'image de son objet (Données IMP-0210 : genre, dimensions, bits « élément défini », valeurs), ce qui conserve l'état « non défini » des éléments (LSE15 §6-3.3) ; une chaîne, son texte ;
  - **GARER β** : le contenu est écrit dans la première étendue libre assez grande, sinon à la fin du fichier. Une étendue libre plus longue que le besoin est **découpée** quand son secteur de répertoire porte une entrée inutilisée : l'enregistrement en prend les premiers secteurs, et cette entrée décrit le reste comme une étendue libre ; à défaut, l'étendue est prise entière. Les entrées sont ensuite réécrites dans cet ordre : l'entrée de l'étendue reprise cesse de la décrire comme libre, la fin d'une étendue découpée étant décrite par la même écriture de secteur ; puis l'entrée de l'enregistrement, qui ne fait qu'une avec la précédente quand l'enregistrement est nouveau ; puis l'ancienne étendue, s'il y en avait une, devient une étendue libre. Jusqu'à la réécriture de l'entrée de l'enregistrement, l'ancien contenu reste valide : un incident ne laisse jamais un enregistrement à moitié écrit (VM-3405), ni une étendue décrite à la fois comme vivante et comme libre ; il ne peut que perdre des secteurs ;
  - **SUPPRIMER β** : l'entrée devient une étendue libre, par la réécriture d'un secteur de répertoire ; le fichier ne rétrécit pas ;
  - **SUPPRIMER** sans β : le fichier est supprimé ;
  - **CHARGER β** : lecture de l'entrée, puis des secteurs de l'étendue ;
  - aucun tassement automatique ; CATALOGUER recopie le fichier temporaire sans ses étendues libres (étape 7c).
- Alternatives envisagées : réécrire le fichier entier à chaque GARER ou SUPPRIMER — écartée (Éric, 2026-10-25) : inefficace, et étranger aux systèmes historiques ; un fichier ProDOS par enregistrement — écartée : un fichier-données ne serait plus un fichier.
- Justification : VM-3405 ; INRDP75, 5ᵉ partie, §1-4 ; LSE15 §6-3 ; choix d'Éric du 2026-10-25 (point A3).
- Impact mémoire : un secteur de répertoire et le contenu d'un enregistrement. Impact disque : arrondi au secteur, comme sur les systèmes historiques ; étendues libres réutilisées, et découpées quand leur secteur de répertoire le permet. Impact performances : GARER écrit l'étendue, puis un à trois secteurs de répertoire ; une découpe relit en outre le secteur de l'étendue reprise.
- Impact historique : la comptabilité en secteurs des systèmes historiques est retrouvée telle quelle.
- Références normatives : VM-3405 ; Données IMP-0210.
- Statut : retenu (Éric, 2026-10-25).
- Complément du 2026-10-30 (Éric ; audit du code, AC-18, AC-19) : découpe d'une étendue libre plus longue que le besoin, et ordre des écritures de GARER β décrit ci-dessus.
- Complément du 2026-10-30 (Éric ; audit du code, AP-17, AC-36) : GARER écrit le contenu d'un enregistrement par blocs de 16 secteurs au plus (4 096 octets), directement depuis son image, au lieu d'un secteur à la fois ; le dernier secteur partiel, et les secteurs d'une étendue reprise entière qui dépassent le contenu, sont complétés de zéros. L'ordre des écritures (contenu, puis entrées) et les octets écrits dans l'étendue utile sont les mêmes. Mesuré sur l'Apple IIGS, GARER et CHARGER d'un tableau de 80 Ko : sur la disquette `/LSE/`, GARER 10,2 s par secteurs et 9,0 s par blocs, CHARGER 5,4 s ; sur le disque dur, par blocs, GARER 1,48 s et CHARGER 0,82 s ; un remplacement de même taille qui n'agrandit pas le fichier, 1,10 s. L'écart restant entre l'écriture et la lecture tient au disque ; la stdio n'y prend qu'environ 20 ms, et elle est conservée (IMP-0249).

---

## 5. Catalogue des fichiers permanents

**IMP-0248** — *`CATALOGUE` : une entrée de 16 octets par fichier permanent, mise à jour sur place*
- Sujet : où conserver le compte propriétaire, la date et la protection de chaque fichier permanent (VM-3404 ; INRDP75 §2-4, TABLE DES FICHIERS, UTILISATION DISQUE) ?
- Choix : le fichier `CATALOGUE` de la bibliothèque est une suite d'entrées de 16 octets : catégorie (`P` programme, `D` données permanent, 0 entrée libre), longueur du nom, nom sur 5 octets, compte (u8), date de création (jour, mois, deux chiffres de l'année), protection (bit 0 : `E` exécution seule ; bit 1 : `L` protection absolue), trois octets réservés. Une création occupe la première entrée libre ou s'ajoute à la fin ; une suppression libère l'entrée ; chaque modification réécrit une seule entrée. Les fichiers temporaires n'y figurent pas : ils appartiennent à la session.
- Alternatives envisagées : métadonnées dans l'en-tête de chaque fichier — écartée : TABLE DES FICHIERS devrait ouvrir chaque fichier ; attributs ProDOS (type auxiliaire) — écartée : ils ne portent ni compte ni protection.
- Justification : VM-3404 ; INRDP75, 5ᵉ partie, §2-2, §2-4.
- Impact mémoire : une entrée lue à la fois. Impact disque : 16 octets par fichier permanent. Impact performances : une lecture séquentielle pour une recherche.
- Impact historique : nul.
- Références normatives : VM-3404.
- Statut : retenu (format) ; employé aux étapes 7b à 7d (compte et protection : IMP-0255).
- Complément du 2026-10-30 (Éric ; audit du code, AP-14) : la recherche d'une entrée lit le catalogue par blocs de 16 entrées et trouve, dans la même passe, la première entrée libre ; l'inscription ne relit plus le catalogue pour la chercher.

---

## 6. Interface et modules

**IMP-0249** — *`interface_fichiers.h` : fichiers désignés par espace et nom, accès par position ; modules `bibliotheque.c` et `fichiers.c`*
- Sujet : comment le noyau accède-t-il aux fichiers sans dépendre du système de fichiers (Organisation du dépôt IMP-0136) ?
- Choix :
  - le noyau désigne un fichier par un **espace** (`ESPACE_RACINE`, `ESPACE_PROGRAMMES`, `ESPACE_DONNEES`, `ESPACE_TEMPORAIRES`) et un **nom** ; seule la plateforme compose le chemin GS/OS (préfixe de bibliothèque, sous-répertoire, nom, séparateur `:`) ;
  - opérations : `plateforme_fichier_ouvrir` (lecture, mise à jour, création), `plateforme_fichier_lire` et `plateforme_fichier_ecrire` à une position donnée, `plateforme_fichier_taille`, `plateforme_fichier_fermer`, `plateforme_fichier_existe`, `plateforme_fichier_supprimer`, `plateforme_fichier_renommer`, `plateforme_fichier_lister` (énumération d'un espace), `plateforme_bibliotheque_ouvrir` (préfixe de `-BIB`, création des répertoires manquants), `plateforme_espace_vider` ;
  - réalisation : `src/plateforme/fichiers_iigs.c`, par stdio d'ORCA/C (`fopen` en `rb`, `r+b`, `w+b` ; `fseek` ; `fread` ; `fwrite` ; `remove` ; `rename`) et, pour les répertoires, `CreateGS`, `OpenGS`, `GetDirEntryGS`, `CloseGS` (point F2) ;
  - noyau : `bibliotheque.c` (fichiers-programmes, RCI-S, catalogue ; segment `"bibliothe"`) et `fichiers.c` (fichiers-données ; segment `"fichiers"`), conformément à Organisation du dépôt §2.1 (le nom de segment `"bibliotheque"` dépasse 10 caractères, IMP-0197) ;
  - tests : sous Golden Gate, les suites emploient la réalisation réelle dans le répertoire `build/tests/travail`, désigné par `-BIB` ou par l'appel direct de `plateforme_bibliotheque_ouvrir` ; aucun bouchon de fichiers n'est nécessaire (sonde du 2026-10-25).
- Alternatives envisagées : un bouchon de fichiers en mémoire — écartée : Golden Gate fournit les vraies opérations (point A5) ; des opérations sur des fichiers entiers seulement — écartée : IMP-0247 demande l'accès par position.
- Justification : Organisation du dépôt IMP-0136, §2.1 ; sonde du 2026-10-25 ; choix d'Éric du 2026-10-25 (points F2, A5).
- Impact mémoire : un descripteur de fichier ouvert à la fois par opération. Impact disque : sans objet. Impact performances : un appel stdio par lecture ou écriture.
- Impact historique : nul.
- Références normatives : IMP-0136 ; VM-3401, VM-3405.
- Statut : retenu (Éric, 2026-10-25).
- Complément du 2026-10-30 (Éric ; audit du code, AC-29) : cause d'un échec.
  - Constat : la stdio d'ORCA/C ne dit pas pourquoi une ouverture, une suppression ou une écriture échoue. Essais sous Golden Gate, versés dans `audits/Audit_code_2026-10-29_essais/ac29/` :
    - `fopen` en `r+b` pose `ENOENT` pour un fichier verrouillé comme pour un fichier absent ;
    - `fwrite` pose `EIO` quand GS/OS rend `$48` (volume plein) ;
    - `toolerror()` reste nulle après ces échecs ;
    - `remove` rend une valeur non nulle sans poser `errno` (le manuel n'en promet pas davantage) ;
    - `fopen` en `w+b` d'un fichier verrouillé échoue (`EEXIST`) sans le vider : la stdio tente une ouverture, puis une création.
  - Après l'échec d'une ouverture en lecture ou en mise à jour, ou d'une suppression, la plateforme appelle `GetFileInfoGS` sur le chemin. `$46` (fichier absent) et `$44` (répertoire absent) donnent `PLAT_INEXISTANT`. Tout autre résultat donne `PLAT_ERREUR` : fichier présent mais refusé (verrouillé, occupé, répertoire), volume absent (`$45`).
  - Le noyau ne crée `IDENTIFICATIONS`, `CATALOGUE` ou le fichier-données d'un GARER que s'il est absent ; un fichier présent mais refusé n'est jamais vidé. SUPPRIMER (commande et instruction), GARER, CHARGER et la suppression d'une identification rendent alors F 27 (`ERR_F_ENTREE_SORTIE`), et non « fichier inexistant » (EX 55, F 05, F 10).
  - Tout échec d'écriture reste `PLAT_PLEIN` (EX 52, F 02), la stdio ne transmettant pas le code de GS/OS. Limite connue : une écriture refusée pour une autre cause (disquette protégée, erreur physique) est annoncée comme un support plein. Des appels GS/OS directs à la place de la stdio la lèveraient ; cette alternative est renvoyée à l'arbitrage d'AP-17.
  - Le code de GS/OS n'est demandé qu'en cas d'échec : aucun appel de plus sur le chemin ordinaire.
- Complément du 2026-10-30 (Éric ; audit du code, AP-06) : `plateforme_espace_parcourir(e, f, contexte)` appelle `f` pour chaque fichier de l'espace, dans l'ordre du répertoire, en une seule ouverture (`OpenGS`, un `GetDirEntryGS` par fichier, `CloseGS`). Le moniteur en tire, pour TABLE DES FICHIERS, UTILISATION DISQUE (espace temporaire) et DUPLIQUER avec `*`, une liste triée des noms valides, refaite au premier pas de chaque parcours et rendue à la fin de la commande : parcourir N fichiers coûte N + 2 appels au lieu de 3N². Si la liste ne peut être allouée, le répertoire est relu à chaque nom, sans erreur de plus.

---

## 7. Classe d'erreurs F

**IMP-0250** — *Classe F : numéros de MEMENTO82, annexe III ; forme `ERREUR F nn`, suivie de ` EN LIGNE lll` pendant l'exécution*
- Sujet : comment numéroter et restituer les erreurs propres aux fichiers ?
- Choix :
  - numérotation d'IMP-0185 : numéro de MEMENTO82, annexe III, quand il existe ; sinon, à partir de 100 ;
  - forme attestée à Victor Duruy : `ERREUR F nn`, une seule espace entre `ERREUR` et la classe ; pour une erreur survenue pendant l'exécution d'une instruction, ` EN LIGNE lll` (ou ` EN LIGNE BUR`) suit, comme pour la classe E (IMP-0186) ; avec l'option `-LIBELLES`, ` : libellé` suit ;
  - la classe `FL` attestée une fois à Victor Duruy (`ERREUR FL 06`, commande `DUPLIQUER $*,D`) est consignée sans nature : son sens n'est pas établi, et la feuille (ANGLO V4, 5ème, sans bannière) est antérieure à F4 ; elle n'est pas reproduite (choix d'Éric du 2026-11-05) ;
  - les erreurs des instructions GARER, CHARGER, SUPPRIMER et EXECUTER déjà cataloguées en classe E (EX 50 à 56, EX 47, EX 105 ; Gestionnaire des erreurs §9.7) le restent ;
  - le catalogue (Gestionnaire des erreurs §10) est complété à chaque étape ; il reçoit dès 7a les natures que réclament RCI-S et l'organisation de la bibliothèque.
- Alternatives envisagées : erreurs de fichier en classe E — écartée : MEMENTO82 et Victor Duruy attestent une classe propre.
- Justification : MEMENTO82, annexe III ; sorties de Victor Duruy ; IMP-0185, IMP-0186 ; choix d'Éric du 2026-10-25 (point A4).
- Impact mémoire : quelques entrées de catalogue. Impact disque : sans objet. Impact performances : nul.
- Impact historique : reproduit la forme attestée.
- Références normatives : Gestionnaire des erreurs §3, §4, §10.
- Statut : retenu (Éric, 2026-10-25).
- Complément du 2026-10-30 (Éric ; audit du code, AC-28) : si la bibliothèque n'a pas pu être ouverte au lancement (chemin trop long, ou répertoire impossible à créer ou à ouvrir), le message `ERREUR F 102` passe par `erreurs_formater_fichier`, avec son libellé sous `-LIBELLES`, et la session continue. `plateforme_bibliotheque_disponible` le retient. Les commandes de fichiers (RANGER, MODIFIER, APPELER, SUPPRIMER, CATALOGUER, IDENTIFICATION, TABLE DES FICHIERS, UTILISATION DISQUE, DUPLIQUER, DECODER, ENCODER, et les commandes de l'initialisateur) et les instructions GARER, CHARGER, SUPPRIMER et EXECUTER rendent alors F 102, au lieu d'agir sur la bibliothèque par défaut.

---

## 7 bis. Fichiers-programmes et EXECUTER (étape 7b)

**IMP-0251** — *Commandes RANGER, APPELER, MODIFIER, SUPPRIMER*
- Sujet : comment le moniteur réalise-t-il les commandes portant sur les fichiers-programmes (INRDP75, 5ᵉ partie, §2-4 ; LSE15 §9-3) ?
- Choix :
  - **droits** : ceux d'IMP-0255 (attribut `P`, propriété, `,E` sous `S`) ; le compte courant est inscrit au catalogue avec la protection `,E` ou `,L` de `RANGER ω,E` et `RANGER ω,L` (profil T1600 ; point B1) ;
  - **noms uniques** (point B2, révisé le 2026-11-06) : la règle dépend du profil.
    - **MITRA 15** : programmes et fichiers-données permanents partagent les mêmes noms. `RANGER ω` est refusé si un programme ω existe (F 03) ou si un fichier-données permanent ω existe (F 04). `CATALOGUER …,ω` est refusé si un fichier-données permanent ω existe (F 04) ou si un programme ω existe (F 03). L'instruction GARER sur `'#ω'` ne crée pas le fichier-données ω si un programme ω existe (F 03 ` EN LIGNE lll`). Sources : LSE15 §9-3, « les noms de fichiers permanents devront être uniques pour tous les utilisateurs » ; RANGER : « une erreur est détectée si le nom de fichier indiqué existe déjà dans la bibliothèque » ; `SUPPRIMER ω` élimine « le fichier-programme ou le fichier-donnée de nom ω », sans dire de quelle sorte il s'agit.
    - **T1600** (les deux versions) : les noms sont uniques au sein de chaque sorte. `RANGER ω` n'est refusé que si un programme ω existe (F 03), et `CATALOGUER …,ω` que si un fichier-données permanent ω existe (F 04). GARER sur `'#ω'` crée le fichier-données ω même si un programme ω existe. Un programme et un fichier-données de même nom coexistent ; `SUPPRIMER ω,P` et `ω,D` les distinguent. INRDP75, 5ᵉ partie, §2-4, ne pose aucune règle d'unicité et ne cite aucun refus pour RANGER ni pour CATALOGUER.
    - **Recoupement.** Constaté par Éric sur LSE 8086 le 2026-11-06 : `RANGER ω` accepte le nom d'un fichier-données permanent ω, et `GARER A,1,'#X'` après `RANGER X` crée le fichier-données X, le programme X restant intact (`CHARGER B,1,'#X'`, puis `APPELER X`). Les deux fichiers coexistent. L'essai du 2026-10-25 portait sur un programme déjà rangé sous le même nom : il confirmait l'unicité entre programmes, pas entre les deux sortes. Le comportement de LSE 8086 est retenu pour le T1600, et non pour MITRA 15, où la lettre de LSE15 prévaut (choix d'Éric du 2026-11-06, option b) ;
  - **MODIFIER ω** : refusé si le programme ω n'existe pas (F 05) ; le programme courant est écrit sous le nom provisoire `TRANSIT`, puis remplace ω : un échec ne détruit pas le programme rangé ; la règle F 25 de MEMENTO82 (remplacement sous un autre nom que celui du dernier APPELER) n'est pas appliquée, ni INRDP75 ni LSE15 ne la donnant (point B3) ;
  - **APPELER ω** : restauration d'IMP-0246 ; l'état du programme devient *aucun* ;
  - **SUPPRIMER** : sous MITRA 15, `SUPPRIMER ω` supprime le programme ω, sinon le fichier-données permanent ω (LSE15 §9-3) ; sous T1600, `SUPPRIMER ω,P`, `ω,D` ou `ω,T` désigne l'espace, et `SUPPRIMER *` vide l'espace temporaire (INRDP75 §2-4) ; un fichier absent donne F 05 ; l'entrée de catalogue d'un fichier permanent supprimé est libérée (point B4) ;
  - **erreurs** : `ERREUR F nn` (IMP-0250) ; un paramètre mal formé donne `????` (Moniteur IMP-0235).
- Justification : INRDP75, 5ᵉ partie, §2-4 ; LSE15 §9-3 ; essais d'Éric sur LSE 8086 du 2026-10-25 et du 2026-11-06 ; choix d'Éric du 2026-10-25 (points B1 à B4) et du 2026-11-06 (point B2 selon le profil).
- Impact mémoire : l'image RCI-S du programme pendant RANGER et MODIFIER. Impact disque : un fichier par programme. Impact performances : une écriture ou une lecture du fichier entier.
- Impact historique : commandes historiques ; droits par IMP-0255.
- Références normatives : VM-3401, VM-3403, VM-3404.
- Statut : retenu (Éric, 2026-10-25).

**IMP-0252** — *Instruction EXECUTER : fin d'exécution `FIN_EXECUTER`, ligne 1 par défaut, interdite en machine de bureau*
- Sujet : comment réaliser l'instruction de liaison `EXECUTER α{,β}` (LSE15 §5-7 ; INRDP75, 5ᵉ partie, §3) ?
- Choix :
  - l'interpréteur lit le nom α (EX 53 s'il n'est pas un nom de fichier L.S.E.) et la ligne β ; il demande au bibliothécaire la restauration du module, la ligne β devant y exister ; le programme courant est remplacé, toutes les données sont perdues, la pile est vidée, et la position est placée en tête de la ligne β, au niveau principal ; l'interpréteur rend alors la fin d'exécution `FIN_EXECUTER`, et le moniteur relance l'exécution, comme pour la commande EXECUTER ; l'interpréteur ne remplace donc pas le programme qu'il exécute ;
  - β absent vaut 1 : « si β est absent, on commence l'exécution à partir de la ligne 1 » (LSE15 §5-7.2) ; si la ligne β n'existe pas dans le module, l'erreur est EX 75 « ligne de branchement inexistante », et le module appelant reste en place (constaté sur LSE 8086 le 2026-10-25 : `EXECUTER 'M'` sur un module sans ligne 1 donne cette erreur ; point B5) ;
  - module absent : EX 105 « module introuvable pour EXECUTER » (LSE15 §5-7.3 : l'exécution du module appelant est terminée) ; fichier refusé (F 100, F 101, F 27) : erreur de classe F, ` EN LIGNE lll` ;
  - EXECUTER est interdit en mode machine de bureau : erreur de compilation C 58, nature `ERR_C_BUREAU_EXECUTER` (constaté sur LSE 8086 : « Instruction interdite en mode calculateur de bureau » ; point B6).
- Justification : LSE15 §5-7 ; INRDP75, 5ᵉ partie, §3, PG64, PG65 ; essais d'Éric sur LSE 8086 du 2026-10-25 ; choix d'Éric du 2026-10-25 (points B5, B6).
- Impact mémoire : l'image du module pendant sa lecture. Impact disque : sans objet. Impact performances : une lecture de fichier par EXECUTER.
- Impact historique : reproduit le chaînage de modules.
- Références normatives : VM-3401, VM-3403 ; Interpréteur IMP-0214 ; Gestionnaire des erreurs §7.6, §9.7.
- Statut : retenu (Éric, 2026-10-25).

**Tests (étape 7b).** `tests/composants/composants_bibliotheque.c` (43 contrôles, bibliothèque `tests:BIBLIO`) : noms ; RANGER puis APPELER, programme identique à la décompilation et exécutable (procédure, chaîne partagée) ; refus sans modification du programme courant (fichier absent, ligne absente, autre profil, octet altéré) ; MODIFIER ; noms uniques ; SUPPRIMER ; EXECUTER avec et sans β, données perdues, module absent (EX 105), ligne absente (EX 75, appelant intact) ; EXECUTER refusé en machine de bureau. `tests/composants/composants_moniteur.c` : session RANGER, F 03, EXECUTER, APPELER, LISTER, MODIFIER, SUPPRIMER, F 05, C 58. Ajouts de l'audit du code (2026-10-30) : ligne vide (AC-08) ; image de plus de 64 Kio (AC-16) ; sept fichiers altérés, somme de contrôle recalculée (identifiant ou index hors des tables, symboles ou constantes en double, nom invalide, nature inconnue), chacun refusé par F 101, le programme courant restant intact (AC-17) ; SUPPRIMER d'un programme présent mais refusé : F 27, un répertoire non vide tenant lieu de fichier verrouillé (AC-29). Ajout du 2026-11-06 (point B2 selon le profil) : sous `LSE F4`, RANGER sous le nom d'un fichier-données permanent accepté, les deux fichiers coexistant, F 03 pour un second RANGER.

---

## 7 ter. Fichiers-données (étape 7c)

**IMP-0253** — *GARER, CHARGER, SUPPRIMER, CATALOGUER ; espace temporaire de 200 secteurs, option `-TEMP`*
- Sujet : comment l'interpréteur et le moniteur réalisent-ils les instructions et commandes portant sur les fichiers-données (LSE15 §6-3 ; INRDP75, 5ᵉ partie, §2-3, §2-4), sur le format d'IMP-0247 ?
- Choix :
  - **espace temporaire standard** : 200 secteurs par défaut (50 Kio), réglable au lancement par `-TEMP n` ; une écriture qui le ferait dépasser donne EX 52 « plus de place dans le fichier » (LSE15 §6-3.3) ; les fichiers permanents ne sont bornés que par le support (point C1) ;
  - **β** : un entier de 1 à 32 767 ; sinon EX 50 « numéro d'enregistrement incorrect », sans arrondi (point C2) ;
  - **γ** : une expression chaîne, `#` facultatif (fichier permanent) puis un nom de fichier L.S.E. ; sinon EX 53 « nom de fichier erroné » (point C7) ;
  - **GARER α,β,γ** : une variable simple donne un enregistrement de type 0 (4 octets), un tableau de type 1 ou 2 (image de l'objet, éléments non définis conservés), une chaîne de type 3 ; une variable sans valeur donne l'erreur d'exécution ordinaire ; un fichier absent est créé ; un fichier permanent créé est inscrit au catalogue ; sous le profil MITRA 15, un programme de même nom empêche la création (F 03 ` EN LIGNE lll`, point B2, IMP-0251) ; les droits de `#` sont ceux d'IMP-0255 (point C3) ;
  - **CHARGER α,β,γ{,δ}** : le fichier temporaire est cherché d'abord, puis le permanent (`#` : le permanent seul) ; α est déclaré implicitement selon le type de l'enregistrement ; une case déjà affectée à une quantité d'un autre type donne EX 47 (LSE15 §6-3.4) ; avec δ, le compte rendu −2, −1, 0, 1, 2 ou 3 est rangé dans δ et aucune erreur « fichier inconnu » ou « enregistrement inexistant » n'est détectée ; sans δ, ce sont EX 55 et EX 56 (point C4) ;
  - **SUPPRIMER γ{,β}** (profil MITRA 15) : l'enregistrement β, ou le fichier entier sans β (INRDP75, 5ᵉ partie, §2-3-3) ; fichier absent : EX 55 (constaté sur LSE 8086 le 2026-10-25 : `SUPPRIMER 'ZZZ',1` donne « Fichier inexistant ») ; enregistrement absent : EX 56 ; LSE 8086 refuse `SUPPRIMER 'ZZZ'` sans β, divergence consignée : INRDP75 donne la forme (point C5) ;
  - **CATALOGUER ω1,ω2** : le fichier temporaire ω1 est recopié sous le nom permanent ω2 sans ses étendues libres, inscrit au catalogue, puis détruit ; erreurs F 05 (ω1 absent), F 04 (fichier-données permanent ω2 existant), F 03 (programme ω2 existant, sous le profil MITRA 15 seulement, point B2, IMP-0251) ; `CATALOGUER ω1,ω2,L` sous T1600 inscrit la protection `L` (point C6).
- Justification : LSE15 §6-3 ; INRDP75, 5ᵉ partie, §1 à §2-4 ; essais d'Éric sur LSE 8086 du 2026-10-25 ; choix d'Éric du 2026-10-25 (points C1 à C7) et du 2026-11-06 (point B2 selon le profil).
- Impact mémoire : le contenu d'un enregistrement pendant GARER et CHARGER. Impact disque : secteurs de 256 octets ; étendues libres réutilisées. Impact performances : GARER écrit l'étendue et un ou deux secteurs de répertoire ; le contrôle de l'espace temporaire parcourt les fichiers temporaires.
- Impact historique : reproduit les instructions et commandes ; l'espace temporaire est agrandi pour un disque de 32 Mio.
- Références normatives : VM-3405, VM-3407 ; Gestionnaire des erreurs §9.7, §9 ter.
- Statut : retenu (Éric, 2026-10-25).
- Compléments du 2026-10-30 (Éric ; audit du code) :
  - AP-15 : le contrôle de l'espace temporaire n'a lieu que pour une écriture qui agrandit le fichier, conformément à la lettre ci-dessus. Un GARER qui réutilise une étendue libre est accepté même quand l'espace est déjà dépassé.
  - AC-27 : la valeur de `-TEMP` est un nombre décimal strictement positif ; une valeur invalide arrête le lancement sur `OPTION INCORRECTE`.

**Tests (étape 7c).** `tests/composants/composants_fichiers.c` (42 contrôles, bibliothèque `tests:BIBFICH`) : exemples 1 et 2 de LSE15 §6-3 (tableau à éléments non définis, chaîne `MATINAL`) ; compte rendu δ (−2, −1, 1, 3) ; EX 55, EX 56, EX 47 ; variable simple ; remplacement par un contenu de 640 caractères ; EX 50, EX 53 ; SUPPRIMER d'un enregistrement et d'un fichier ; étendue libre réutilisée ; priorité au temporaire ; 45 enregistrements (répertoire chaîné) ; espace temporaire dépassé (EX 52) ; CATALOGUER et ses erreurs. `tests/composants/composants_moniteur.c` : commande CATALOGUER, F 04, temporaires vidés à AU REVOIR. Ajouts de l'audit du code (2026-10-30) : remplacement par une étendue libre reprise (AC-18) ; étendue libre découpée, pour un enregistrement remplacé comme pour un nouveau, sans que le fichier grossisse ; enregistrement de longueur nulle remplacé dans une étendue libre reprise (AC-19) ; SUPPRIMER d'un fichier et d'un enregistrement, et GARER, sur un fichier présent mais refusé : F 27 (AC-29). Ajout du 2026-11-06 (point B2 selon le profil) : sous MITRA 15, GARER sur `'#ω'` refusé par F 03 si un programme ω existe, sans créer le fichier ; sous `LSE F4`, GARER sur `'#ω'` puis CHARGER à côté du programme ω, CATALOGUER vers ω accepté, F 04 pour un second CATALOGUER. Golden Gate ne sachant pas verrouiller un fichier, un répertoire non vide en tient lieu. Ce répertoire est créé en ouvrant une bibliothèque dont le préfixe tombe dans l'espace visé, puis il est vidé et retiré.

## 7 quater. Identifications, droits, initialisateur, tables, DUPLIQUER (étape 7d)

**IMP-0254** — *Identifications : 0 et 1 fixes, au plus 9 autres dans le fichier `IDENTIFICATIONS`*
- Sujet : comment le système connaît-il les identifications légales, que LSE15 §9-3 dit « données au lancement du système » et INRDP75 note 11 « définies au moment de l'initialisation » ?
- Choix :
  - une identification est un nombre de 0 à 65 499, `100 × clé + compte` (LSE15 §9-3 ; MEMENTO82 §3.6.1) ; le compte est le reste de la division par 100 ;
  - l'identification 0 (aucun attribut) et l'identification 1 (attributs `P`, `D`, `S`, `x`) existent toujours (MEMENTO82 §3.6.2, remarque 1) ;
  - au plus 9 autres (remarque 2), dans le fichier `IDENTIFICATIONS` à la racine de la bibliothèque : 9 emplacements de 6 octets (occupé, numéro sur 2 octets, attributs, secteurs « n » sur 2 octets) ;
  - attributs : `P` 0x01, `D` 0x02, `S` 0x04, `!` 0x08, `>` 0x10, `x` 0x20 ; « n » est un nombre de secteurs ;
  - la session est sous l'identification 0 à BONJOUR et après AU REVOIR ;
  - la propriété d'un fichier permanent est celle du **compte** sous lequel il a été créé, inscrit au catalogue (IMP-0248) : « un numéro de compte identique » (LSE15 §9-3).
- Alternatives envisagées : identifications perdues à chaque lancement, comme le dit MEMENTO82 (remarque 3, « disparaissent au moment du retour au système Prologue ») — écartée : sur l'Apple IIGS, aucun opérateur ne les redéfinit à chaque démarrage. **Divergence consignée** avec MEMENTO82 (point E1).
- Justification : choix d'Éric du 2026-10-25 (point E1).
- Impact mémoire : une identification courante (6 octets). Impact disque : 54 octets. Impact performances : une lecture du fichier par IDENTIFICATION. Impact historique : reproduit les identifications ; leur persistance diverge de MEMENTO82.
- Références normatives : VM-3401 ; LSE15 §9-3 ; INRDP75, 5ᵉ partie, §2-2 ; MEMENTO82 §3.6.
- Statut : retenu (Éric, 2026-10-25).

**IMP-0255** — *Droits, propriété, protections `E` et `L`, programme courant secret*
- Sujet : quels droits les commandes et instructions de fichier vérifient-elles ?
- Choix :
  - **attribut `P`** : RANGER, MODIFIER, la suppression d'un fichier-programme, DUPLIQUER `,P` ; sinon F 07 ;
  - **attribut `D`** : GARER et SUPPRIMER sur un fichier permanent (`#`), CATALOGUER, la suppression d'un fichier-données permanent, DUPLIQUER `,D` ; sinon F 08 (` EN LIGNE lll` à l'exécution) ;
  - **propriété** : un fichier permanent existant n'est modifié ou supprimé que sous le compte inscrit au catalogue ; sinon F 22 (INRDP75, 5ᵉ partie, §2-2) ;
  - **`RANGER ω,E`** (T1600) exige l'attribut `S` ; sinon F 11 ;
  - **protection `L`** : `RANGER ω,L` et `CATALOGUER ω1,ω2,L` sous T1600, et tout fichier permanent créé sous une identification `!` (MEMENTO82) ; un tel fichier n'est exploitable que sous son compte : F 06 pour APPELER et EXECUTER, EX 54 pour CHARGER ; `!` l'emporte sur `,E` ;
  - **programme secret** : un fichier-programme `E` appelé (APPELER, EXECUTER) sous un autre compte, sans l'attribut `>`, ne peut qu'être exécuté (INRDP75, 5ᵉ partie, §2-4 ; MEMENTO82, `S` et `>`) ; LISTER, NUMERO, EFFACER une ligne, l'entrée d'une ligne, RANGER et MODIFIER donnent F 06 ; `EFFACER *` ou un autre APPELER libèrent le programme ; changer d'identification ne le libère pas ;
  - sous MITRA 15, que LSE15 ne dote d'aucune protection, `SUPPRIMER ω` vise le fichier-programme ω s'il existe, sinon le fichier-données permanent.
- Alternatives envisagées : droits fondés sur l'identification complète (clé comprise) — écartée : LSE15 parle d'un « numéro de compte identique » ; libérer le programme secret à IDENTIFICATION — écartée : il suffirait de changer d'identification pour lister un programme secret.
- Justification : choix d'Éric du 2026-10-25 (point E3) ; correction d'Éric du 2026-10-25 sur la transcription d'INRDP75 (`RANGER ω,E` et `,L`, non `;E`, `;L`).
- Impact mémoire : un indicateur. Impact disque : nul. Impact performances : une lecture du catalogue par opération sur un fichier permanent. Impact historique : reproduit les droits a et b d'INRDP75 et les protections du T1600.
- Références normatives : VM-3401, VM-3404 ; Gestionnaire des erreurs §9 ter (F 06, F 07, F 08, F 11, F 22) et §9.7 (EX 54).
- Statut : retenu (Éric, 2026-10-25, point E3 ; priorité de `!` sur `,E` et maintien du secret après IDENTIFICATION confirmés le 2026-10-27).

**IMP-0256** — *Commande IDENTIFICATION n*
- Sujet : forme et effets de la commande (LSE15 §9-3 ; INRDP75, 5ᵉ partie, §2-2 et §2-4 ; MEMENTO82 §3.6.1).
- Choix :
  - la saisie de n se fait **sans écho** (`console_echo(0)`) ; sous Golden Gate, le terminal de l'hôte fait écho lui-même ;
  - n inconnue, mal formée ou hors de 0 à 65 499 : `????` (nature `ERR_D_IDENTIFICATION_ILLEGALE`, réponse « ILLEGALE » de LSE15, rendue comme toutes les erreurs du dialogue, IMP-0235) ; la commande est sans effet ;
  - n acceptée : la réponse « ACCEPTEE » est le retour chariot et l'interligne (LSE15 §9-3 : « ACCEPTEE = retour chariot interligne »), c'est-à-dire l'écho de RETURN, ou le passage à la ligne après X-OFF ; rien d'autre n'est affiché ;
  - attribut `x`, **sous MITRA 15 seulement** (INRDP75, 5ᵉ partie, §2-2 : option c, « augmentation dynamique de la taille de l'espace temporaire standard (sur MITRA 15) » ; sous T1600, `x` est sans effet et aucune question n'est posée, ce que montrent les sessions de Victor Duruy, constat d'Éric du 2026-10-28) : le système affiche `NOMBRE DE SECTEURS DISPONIBLES:n` (secteurs libres du support) puis demande `ESPACE TEMPORAIRE DESIRE:` et lit un nombre de secteurs (INRDP75, annexe II, p. 189, lecture d'un ruban sur MITRA 15 : `NOMBRE DE SECTEURS DISPONIBLES:806` puis `ESPACE TEMPORAIRE DESIRE:10` ; INRDP75, 5ᵉ partie, §2-2, option c ; LAFOND83 p. 105 et 110) ; une réponse vide donne l'espace standard ; une réponse inférieure au standard donne le standard (« seul l'espace temporaire standard est assuré », LSE15) ; une réponse mal formée donne `????` et la commande est sans effet ;
  - attribut « n » : l'espace temporaire est de n secteurs, au moins le standard ;
  - si l'espace demandé dépasse les secteurs libres du support : `ESPACE TEMPORAIRE REDUIT A x. SECTEURS.` (LSE15, message 3), x étant les secteurs libres, au moins le standard ;
  - sous MITRA 15, les fichiers temporaires sont détruits (LSE15 §9-3 : « L'utilisation de la commande identification détruira les éventuels fichiers temporaires créés par l'utilisateur ») ; sous T1600, ils sont conservés : INRDP75, annexe II, p. 192-194, Listage 1, l'utilisateur « donne son identification puis catalogue le fichier » temporaire VOCA créé auparavant, ce que confirment, selon Éric, les sessions de Victor Duruy (IDENTIFICATION puis CATALOGUER) ; puis l'identification devient courante ; sous MITRA 15, un fichier temporaire à cataloguer doit donc être créé après l'identification. **Divergence consignée** : le Listage 2 d'INRDP75 (MITRA 15, état de 1974) crée VOCA sans identification, puis exécute IDENTIFICATION et `CATALOGUER VOCA,VOCA` sans erreur ; le profil MITRA 15 suit LSE15 (choix d'Éric du 2026-10-28, option a).
- Alternatives envisagées : afficher `ACCEPTEE` et `ILLEGALE` en toutes lettres — écartée : LSE15 donne ces mots comme noms des réponses, dont il précise la forme (`retour chariot interligne`, `???`) ; une question non attestée (`ESPACE TEMPORAIRE : `) — écartée dès que LAFOND83 a donné la forme du dialogue.
- Justification : choix d'Éric du 2026-10-25 (point E4) ; lecture de LSE15 §9-3 ; LAFOND83 p. 105 et 110 (pages versées le 2026-10-25).
- Impact mémoire : nul. Impact disque : destruction des temporaires. Impact performances : nul. Impact historique : reproduit la commande et ses trois réponses.
- Références normatives : VM-3401 ; Moniteur et console IMP-0235.
- Statut : retenu (Éric, 2026-10-25, point E4 ; lecture des réponses de LSE15 et règles de la réponse à `ESPACE TEMPORAIRE DESIRE:` confirmées le 2026-10-27 ; temporaires conservés sous T1600 et détruits sous MITRA 15, choix d'Éric du 2026-10-28).

**IMP-0257** — *Commandes de l'initialisateur ; TABLE DES FICHIERS et UTILISATION DISQUE A PARTIR DE*
- Sujet : comment définir les identifications (point N1 du Moniteur et console) et consulter le catalogue ?
- Choix :
  - **initialisateur** : en état dormant seulement, et dans les deux profils, `CR`, `SU` et `LI` désignent `CREATION ID : `, `SUPPRESSION ID : ` et `LISTE ID :` (MEMENTO82 §3.6.2 à §3.6.4) ; en session, `SU` et `LI` restent SUPPRIMER et LISTER ;
  - `CREATION ID : n,P,D,S,!,>,x|n` : attributs facultatifs dans cet ordre obligatoire ; `x` ou un nombre en dernier ; 0 et 1 ne se redéfinissent pas (`????`) ; un numéro déjà défini est remplacé ; une dixième identification donne F 19 ;
  - `SUPPRESSION ID : n` : F 10 si n n'est pas définie (0 et 1 compris) ;
  - `LISTE ID :` : une ligne par identification, 1 d'abord, le numéro à droite sur 5 colonnes puis `,P`, `,D`, `,SECRET`, `,PROTEGE`, `,PRIVILEGIE`, `,ETENDU`, `,n` (MEMENTO82 §3.6.4), par exemple `    1,P,D,SECRET,ETENDU` ;
  - **forme des commandes** : `TABLE DES FICHIERS A PARTIR DE XXXX[,P|D|T]` et `UTILISATION DISQUE A PARTIR DE XXXX[,P|D|T]` dans les deux profils (LAFOND83 p. 17, 18, 110 ; souvenir d'Éric pour le T1600 de Victor Duruy) ;
  - **liste complète** : `*` sous T1600, `A` sous MITRA 15, sans lettre de type (LAFOND83 : « A ou * suivant l'ordinateur ») ; `*` est refusé sous MITRA 15 (`????`) ; avec une lettre, `*` ou `A` désigne tous les fichiers de ce type ;
  - **liste partielle** : le nom de départ, suivi de `P`, `D` ou `T` (P si la lettre est omise) ; seul le type indiqué est listé, à partir du premier nom égal ou supérieur au nom de départ (choix d'Éric, T3 ; LAFOND83 p. 110, `A PARTIR DE CØPRI,D`) ; un paramètre absent ou mal formé donne `????` ;
  - **ordre** : alphabétique à l'intérieur de chaque section (LAFOND83 ; souvenir d'Éric) ; la section suivante est cherchée dans le catalogue par le plus petit nom au-delà du précédent, sans tableau en mémoire ;
  - **TABLE DES FICHIERS** : les fichiers permanents du compte courant et les fichiers temporaires ; **UTILISATION DISQUE** : tous les fichiers ;
  - **mise en page** : celle d'INRDP75 (5ᵉ partie, p. 133 et 134) propre à chaque profil, colonnes reproduites d'après les exemples ; sous MITRA 15, la date et l'heure sur la ligne qui suit la commande (LAFOND83 p. 17, 110 ; choix d'Éric, T5) ; les lignes `NOMBRE DE SECTEURS LIBRES:` et `ESPACE TEMPORAIRE ALLOUE` seulement dans la liste complète d'UTILISATION DISQUE ; sous T1600, la ligne `ESPACE …( nnnn SECT.) OCCUPE A ppp %` dans tous les cas ;
  - unités : sous MITRA 15, `NB.MOTS` des programmes et `NB.SECTEURS` des fichiers-données ; sous T1600, `LONGUEUR MOTS` partout ; un mot vaut 2 octets du fichier, un secteur 256 octets ; la console est toujours `01` (MITRA 15) ou `CONSOLE 1` (T1600) ;
  - T1600 : `ESPACE TEMPORAIRE(  nnn SECT.) OCCUPE A ppp %` (temporaires sur espace courant) ; `ESPACE PERMANENT (nnnn SECT.) OCCUPE A ppp %` (secteurs des fichiers permanents sur ces secteurs plus les secteurs libres) ; un total de plus de 4 chiffres élargit le champ ;
  - MITRA 15 : UTILISATION DISQUE exige l'attribut `>` (« ne peut être demandée que sous certains identificateurs », INRDP75 p. 134) ; sinon `????` ;
  - **interruption** : ESC, testée avant chaque ligne de fichier, arrête la liste et affiche `PRET` (IMP-0239 ; demande d'Éric du 2026-10-25) ;
  - **secteurs libres** : l'appel GS/OS Volume ($2008) demande un nom de périphérique (avec un nom de volume, erreur $0040 : sonde `SONDEF V` sur l'Apple IIGS, 2026-10-25) ; `plateforme_secteurs_libres` obtient par GetDevNumber ($2020) le numéro du périphérique qui porte le volume de la bibliothèque, nommé `:VOL` (sans le `:` initial, erreur $0040), puis son nom par DInfo ($202C), appelle Volume sur ce seul périphérique, vérifie que le volume lu porte le nom de celui de la bibliothèque (sans égard à la casse ; sinon 0, disque changé) et rend `freeBlocks × blockSize / 256` ; aucun autre lecteur n'est sollicité (sonde `SONDEF G` sous GSSquared, 2026-11-06 : GetDevNumber en moins d'un soixantième de seconde ; `Essais_IIGS_2026-11-06.md`) ; Golden Gate n'implémente pas Volume et arrête le programme à son appel : `main.c` ne le permet que si le pilote `.CONSOLE` est ouvert (Apple IIGS) ; sinon les secteurs libres valent 0.
- Alternatives envisagées : commandes de l'initialisateur en session — écartée : `SU` et `LI` y sont déjà SUPPRIMER et LISTER ; liste partielle poursuivie dans les sections suivantes — écartée (choix d'Éric, T3) ; refus de UTILISATION DISQUE par une erreur de classe F — écartée : aucune nature de MEMENTO82 ne correspond ; tri en mémoire — écarté : un tableau de noms supposerait une mémoire abondante ; espace permanent fixe (`-PERM n`), à la manière de la zone de 518 secteurs du T1600 d'INRDP75 — écarté (choix d'Éric du 2026-10-27) : la bibliothèque partage le volume, le total d'`ESPACE PERMANENT` est l'occupé plus les secteurs libres du volume, et le pourcentage reste voisin de 0 sur un disque de 32 Mio ; total pris sur tout le volume, fichiers étrangers compris — écarté : il mêlerait les fichiers d'ORCA à ceux du L.S.E. ; GetFileInfo sur le répertoire de volume — écartée : la sonde rend 0 bloc ; recherche du périphérique par DInfo et Volume sur chaque périphérique, retenu ensuite pour la session — écartée : Volume lit chaque support, et au premier UTILISATION DISQUE d'une session les lecteurs 5,25" tournaient plusieurs secondes (constat d'Éric sur l'Apple IIGS et sous GSSquared, 2026-11-06).
- Justification : choix d'Éric du 2026-10-25 (points E2, E5, T1 à T5) ; essai LSE 8086 d'Éric (`1,P,D,S,ETENDU`) ; LAFOND83 p. 17, 18, 109, 110.
- Impact mémoire : nul. Impact disque : nul. Impact performances : une lecture du catalogue par ligne listée (par blocs de 16 entrées) et une ouverture du fichier listé ; pour les secteurs libres, un appel GetDevNumber, DInfo et Volume, sans accès aux autres lecteurs. Impact historique : reproduit les deux formes d'INRDP75 avec le paramètre attesté par LAFOND83 ; les commandes de l'initialisateur viennent de MEMENTO82.
- Références normatives : VM-2201 ; Moniteur et console §11 (point N1), IMP-0239 ; Gestionnaire des erreurs §9 bis, §9 ter.
- Statut : retenu (Éric, 2026-10-25, points E2, E5, T1 à T5 ; refus de 0 et 1 et remplacement à CREATION ID, refus de UTILISATION DISQUE par `????` sous MITRA 15 confirmés le 2026-10-27).

**IMP-0258** — *DUPLIQUER [$]motif[,P|D] ; option `-UNITE`*
- Sujet : réalisation de DUPLIQUER, attestée par les sorties de Victor Duruy, le souvenir d'Éric (copie entre le disque et l'unité de disquette) et LAFOND83 p. 109.
- Choix :
  - **profil** : `LSE F4` seulement ; INRDP75 et LSE15 ignorent cette commande, et `DU` n'en est pas une sous T1600 version 4 ni sous MITRA 15 ;
  - `-UNITE chemin` désigne l'unité amovible, bibliothèque de même forme (`PROGRAMMES`, `DONNEES`, `CATALOGUE`), dont les répertoires sont créés à la première copie vers elle ; sur l'Apple IIGS, ce peut être un lecteur 3,5 pouces ou une seconde image ; sans unité : F 29 « numéro d'unité incorrect » ;
  - **forme** : `DUPLIQUER [$]motif[,P|D]` ; le motif est un nom, un groupe de caractères suivi de `*` (les fichiers dont le nom commence par ce groupe), ou `*` seul (tous) ; `,P` ou `,D` limite la copie aux fichiers-programmes ou aux fichiers-données ; sans lettre, les deux (LAFOND83 p. 109 ; D1) ;
  - **sens** : avec `$`, de l'unité vers la bibliothèque ; sans `$`, de la bibliothèque vers l'unité (souvenir d'Éric pour le T1600 ; D2) ; LAFOND83 désigne l'unité de départ par `.N` sur un système à deux unités de disquette ;
  - **vers la bibliothèque** : droits `P` ou `D` (F 07, F 08) ; un fichier de même sorte et de même nom présent donne F 03 (programme) ou F 04 (fichier-données), la règle du T1600 (IMP-0251, point B2) ; le fichier est inscrit au catalogue sous le compte courant, qui le crée, avec la protection inscrite au catalogue de l'unité (`L` sous `!`) (D3, D4) ;
  - **vers l'unité** : aucun attribut, la bibliothèque n'étant pas modifiée ; un fichier `L`, ou `E` sans l'attribut `>`, d'un autre compte n'est pas copié (F 06), la copie contournerait sinon la protection ; l'entrée du catalogue (compte, date, protection) est recopiée dans le `CATALOGUE` de l'unité (D3, D4) ;
  - **erreurs** : un fichier de même sorte et de même nom à destination donne F 03 ou F 04 ; avec un motif, les fichiers sont copiés par ordre alphabétique et la copie s'arrête à la première erreur, signalée ; un motif sans fichier correspondant donne F 05 (D5) ;
  - `FL` reste sans nature : l'hypothèse d'Éric (« floppy ») est consignée comme hypothèse.
- Alternatives envisagées : unité désignée par un numéro (`.N`, LAFOND83) — écartée pour le T1600, où la forme attestée est `$` ; unité sans catalogue — écartée : les protections seraient perdues à la copie.
- Justification : choix d'Éric du 2026-10-25 (points E6, D1 à D5) ; LAFOND83 p. 109.
- Impact mémoire : un tampon de 4 Kio pendant la copie. Impact disque : le fichier copié et une entrée de catalogue. Impact performances : copie par blocs de 4 Kio. Impact historique : restitue une commande attestée du T1600, dont le fonctionnement exact n'est connu que par LAFOND83 et le souvenir d'Éric.
- Références normatives : VM-3401 ; Gestionnaire des erreurs §9 ter (F 03 à F 08, F 29).
- Statut : retenu (Éric, 2026-10-25, points E6, D1 à D5 ; révisé le 2026-11-01 : `LSE F4` seulement).

**IMP-0278** — *`$` devant un nom de fichier, sous `LSE F4` : le fichier est sur l'unité amovible*
- Sujet : les sorties de Victor Duruy emploient `$` devant un nom : `RANGER $SPHIN`, précédé d'`IDENTIFICATIØN` (6EME - SPHIN, `numerisation0022.pdf` ; au dos, note manuscrite d'Éric : « ranger sur diskette »), `APPELER $SINUS` (4EME - SINUS, `numerisation0043.pdf`) et `DUPLIQUER $*,P` (IMP-0258). D'après Éric, `$` indique que le fichier est sur la disquette, et aucune autre source ne connaît cette notation. Comment la réaliser, et jusqu'où l'étendre ?
- Choix :
  - **Règle.** Sous `LSE F4`, `$` devant un nom désigne un fichier de l'unité amovible (`-UNITE`, IMP-0258). Il ne change que le lieu du fichier : la commande reste la même, avec les mêmes droits, les mêmes erreurs et les mêmes protections, appliqués aux répertoires et au `CATALOGUE` de l'unité.
  - **Portée.** Toutes les commandes qui attendent un nom de fichier :

    | Commande | Effet de `$` |
    |---|---|
    | `APPELER $nom` | lit sur l'unité |
    | `RANGER $nom{,E\|,L}` | écrit sur l'unité et inscrit le fichier à son catalogue |
    | `MODIFIER $nom` | remplace le fichier de l'unité |
    | `SUPPRIMER $nom,P\|D` | supprime le fichier de l'unité ; `,T` et `$*` sont refusés (`????`), aucun temporaire n'étant sur l'unité |
    | `CATALOGUER t,$nom{,L}` | le temporaire `t` devient un fichier-données de l'unité ; `$` porte sur le second nom seulement |
    | `DECODER $nom{,n1{,n2}}` | range le fichier décodé sur l'unité |
    | `ENCODER $nom{,n1{,n2}}` | lit le fichier décodé sur l'unité |
    | `TABLE DES FICHIERS A PARTIR DE $XXXX{,P\|D}`, `UTILISATION DISQUE A PARTIR DE $XXXX{,P\|D}` | listent l'unité, d'après son catalogue, avec les colonnes de la forme T1600 ; ni ligne `ESPACE …`, ni section des temporaires ; `,T` refusé |

    `DUPLIQUER` garde ses règles (IMP-0258). Les instructions d'un programme (`EXECUTER`, `GARER`, `CHARGER`, `SUPPRIMER`) n'admettent pas `$` : leur nom est une expression, déjà marquée par `#` pour un fichier permanent, et aucun programme de Victor Duruy ne l'y emploie.
  - **Unité absente.** Sans `-UNITE`, ces commandes donnent F 29, comme `DUPLIQUER`.
  - **Noms.** Un même nom peut exister dans la bibliothèque et sur l'unité. L'unicité des noms permanents (point B2) vaut pour chaque lieu.
  - **Droits.** `RANGER $`, `MODIFIER $` et `SUPPRIMER $nom,P` exigent l'attribut `P` et la propriété, et `SUPPRIMER $nom,D` l'attribut `D` et la propriété. Un fichier de l'unité étant un fichier permanent, `DECODER $nom` exige `D` et la propriété, alors que `DECODER nom`, qui écrit un temporaire, n'exige rien. `APPELER $` et `ENCODER $` respectent la protection `L`, et `APPELER $` la protection `E`, d'après le catalogue de l'unité.
  - **Autres profils.** Sous T1600 version 4 et sous MITRA 15, `$` devant un nom est refusé (`????`).
- Alternatives envisagées :
  1. S'en tenir aux formes attestées (`APPELER`, `RANGER`, `DUPLIQUER`) — écarté : choix d'Éric ; une notation qui ne vaudrait que pour deux commandes serait une règle de plus sans source.
  2. `RANGER $nom` sans attribut, comme `DUPLIQUER` vers l'unité — écarté : la règle serait moins simple, et `IDENTIFICATIØN` précède `RANGER $SPHIN` sur la sortie.
  3. Laisser `TABLE` et `UTILISATION` de côté, leur paramètre étant un nom de départ — écarté : rien d'autre ne montrerait le contenu de l'unité.
  4. Étendre `$` aux instructions — écarté (ci-dessus).
- Justification : IMP-0276 fait foi pour F4 quand les sorties attestent une forme ; l'extension aux autres commandes est un choix d'Éric, qui garde à `$` un seul sens. La règle reprend sans le dupliquer tout le mécanisme de la bibliothèque (droits, catalogue, RCI-S, format des fichiers-données).
- Impact mémoire : nul. Impact disque : les fichiers rangés sur l'unité et leurs entrées de catalogue. Impact performances : nul hors de ces commandes.
- Impact historique : restitue `APPELER $nom` et `RANGER $nom`, attestés ; les autres formes sont des extensions, propres à `LSE F4`.
- Tests : `composants_moniteur` (chaque commande, droits, protections, unité absente, autres profils), `composants_bibliotheque` (F 100 sur l'unité), `composants_codage`, `composants_fichiers`.
- Références normatives : EXI-0061 ; VM-3401, VM-3404 ; relevé T1600 de Victor Duruy, §2 ; IMP-0245, IMP-0246, IMP-0248, IMP-0255, IMP-0258, IMP-0276.
- Statut : retenu (choix d'Éric du 2026-11-01).

**IMP-0268** — *Identification initiale d'une session : option `-ID n`*
- Sujet : à Victor Duruy, on rangeait un programme sans taper IDENTIFICATION ; la bannière imprimait `VOUS ETES SOUS ID 1` (sorties T1600, 4EME - MORSE, 5EME - MULT). Comment le reproduire sans donner de droits à l'identification 0 ?
- Choix :
  - l'option de lancement `-ID n` désigne l'identification sous laquelle s'ouvre chaque session ; par défaut 1 sous le profil `LSE F4`, qui reproduit la configuration de Victor Duruy (Compilateur IMP-0276), et 0 sous T1600 version 4 et sous MITRA 15, conformément à INRDP75 §2-2 (« un utilisateur qui ne dispose pas d'un numéro d'identification […] n'a aucun accès écriture aux fichiers permanents ») ;
  - l'identification est cherchée à BONJOUR ; non définie, elle est remplacée par 0 ; l'espace temporaire est le standard, ou « n » ; AU REVOIR ramène l'identification 0 ;
  - sous `LSE F4`, si l'identification initiale n'est pas 0, la ligne `VOUS ETES SOUS ID c` termine le texte de la bannière, après `BONJOUR` comme après `AU REVOIR` (Moniteur et console IMP-0266), c étant le **compte** : la clé « n'apparaît jamais sur les consoles », le compte est imprimé (INRDP75 §2-2) ; pour l'identification 1, la forme est celle de Victor Duruy ; sous T1600 version 4 et sous MITRA 15, cette ligne, absente d'INRDP75 et de LSE15, n'est pas imprimée ;
  - cette ligne annonce l'identification initiale de la console, non celle de la session : `AU REVOIR` ramène l'identification 0, et `MORSE` et `MULT` impriment pourtant `VØUS ETES SØUS ID 1` après `AU REVØIR` ; une identification prise en cours de session par IDENTIFICATION n'y paraît pas ;
  - l'option relève du responsable du système, qui définit les identifications « au moment de l'initialisation » (INRDP75, note 11).
- Alternatives envisagées : donner P à l'identification 0 — écartée : contraire à INRDP75 et LSE15 ; LAFOND83 (p. 17 : identification 0 « par défaut » sur « certains systèmes ») cède devant eux.
- Justification : choix d'Éric du 2026-10-28 ; sorties de Victor Duruy ; INRDP75 §2-2 et note 11.
- Impact mémoire : un entier. Impact disque : nul. Impact performances : une recherche d'identification à BONJOUR. Impact historique : reproduit la configuration attestée d'un site.
- Références normatives : IMP-0254, IMP-0256 ; Moniteur et console IMP-0228, IMP-0266.
- Statut : retenu (Éric, 2026-10-28).
- Complément du 2026-10-30 (Éric ; audit du code, AC-27) : la valeur de `-ID` est un nombre décimal de 0 à 65 499 (`IDENTIFICATION_MAX`). Une valeur invalide, une option inconnue ou une option sans argument arrêtent le lancement sur `OPTION INCORRECTE : option valeur`, sans ouvrir de session.

**Vérification sur l'Apple IIGS (Éric, 2026-10-27).** T1600 : `UT` donne `ESPACE PERMANENT (9486 SECT.) OCCUPE A   0 %` (2 secteurs occupés et 9 484 libres) ; `TA` donne `ESPACE TEMPORAIRE( 200 SECT.) OCCUPE A   0 %` ; ESC interrompt la liste ; `IDENTIFICATION 1` affiche `NOMBRE DE SECTEURS DISPONIBLES:9484`. Les secteurs libres concordent avec `SONDEF V` (4 748 blocs libres de 512 octets au 2026-10-25).

**Tests (étape 7d).** `tests/composants/composants_moniteur.c` (77 contrôles, dans le segment `"suite"`) : initialisateur (création, ordre des attributs, refus de 0 et 1, LISTE ID, SUPPRESSION ID et F 10) ; IDENTIFICATION sans écho, `????`, question `ESPACE TEMPORAIRE :` ; F 07, F 08 ` EN LIGNE 001`, protection `!`, F 06, F 22, EX 54 ; programme secret sous T1600 (F 11, F 06 pour LISTER, une ligne, EFFACER, RANGER ; exécution permise ; secret après IDENTIFICATION) ; TABLE DES FICHIERS et UTILISATION DISQUE dans les deux profils, listes complètes (`*`, `A`) et partielles (type par défaut P, `PERM,D`), ordre alphabétique, `????` sans `>` ou pour `*` sous MITRA 15, interruption par ESC ; DUPLIQUER dans les deux sens (F 29, F 03, `$*,D`, F 05, `SE*`, F 06 pour un fichier `L` d'un autre compte, protection `E` gardée par le catalogue de l'unité). `tests/composants/composants_analyseur.c` (35) : commandes de l'initialisateur en état dormant seulement. `composants_bibliotheque.c` et `composants_fichiers.c` s'identifient sous 1.

## 7 quinquies. DECODER et ENCODER (étape 7e1)

**IMP-0259** — *Fichier décodé : lignes séparées par X-OFF, NUL final, enregistrements de 252 caractères selon le profil*
- Sujet : quelle forme a le fichier-données qui contient un programme (INRDP75, 5ᵉ partie, §4-1) ?
- Choix :
  - chaque ligne est le texte de LISTER (décompilateur), suivi de X-OFF (code 19) ; le fichier se termine par NUL (code 0) ; chaque enregistrement est une chaîne (type 3) ;
  - **MITRA 15** : exactement 252 caractères par enregistrement, une ligne pouvant continuer dans le suivant ; la dernière ligne est suivie de X-OFF puis de NUL ; le dernier enregistrement est complété à 252 par le contenu du tampon laissé par l'enregistrement précédent (blancs pour le premier) ; reproduit PG66 bis à l'octet près : la reprise `O ';LIRE I` du 2ᵉ enregistrement commence au 52ᵉ caractère, après `(Z) ALORS ALLER EN 2`, X-OFF, la ligne 6, X-OFF et NUL ;
  - **T1600** : autant de lignes complètes que possible dans 252 caractères (PG66 : 220 caractères, lignes 1 à 4 ; la capacité est comprise entre 220 et 272) ; une ligne plus longue est coupée ; la dernière ligne est suivie de NUL au lieu de X-OFF, ce que donne la longueur imprimée du 2ᵉ enregistrement de PG66 (82 = 53 + 29), contre la lettre d'INRDP75 (« en plus ») ;
  - un programme ou un intervalle vide donne un fichier réduit à NUL.
- Alternatives envisagées : X-OFF puis NUL sous T1600 aussi — écartée (choix d'Éric du 2026-10-27, G1) : la longueur attestée de PG66 serait 83 ; une capacité de 256 caractères — écartée : 252 est la seule valeur attestée (MITRA 15) et reste compatible avec PG66.
- Justification : INRDP75, 5ᵉ partie, §4-1, PG66 et PG66 bis (p. 151) ; LSE15 §9-3 ; choix d'Éric du 2026-10-27 (G1).
- Impact mémoire : un tampon de 252 octets. Impact disque : un secteur et une entrée de répertoire par enregistrement (IMP-0247). Impact performances : une écriture par enregistrement. Impact historique : reproduit les deux présentations attestées.
- Références normatives : VM-3404 ; IMP-0247 ; Décompilateur.
- Statut : retenu (Éric, 2026-10-27, G1).

**IMP-0260** — *DECODER ω{,n1{,n2}}*
- Sujet : réalisation de la commande (INRDP75, 5ᵉ partie, §4-1 ; LSE15 §9-3).
- Choix : n1 et n2 ont le sens de LISTER, n1 valant 1 s'il est omis ; le fichier temporaire ω est créé, ou remplacé s'il existe ; un programme secret donne F 06 (IMP-0255) ; un dépassement de l'espace temporaire donne la nature EX 52, rendue comme une erreur de la machine de bureau, et le fichier partiel est détruit ; module `src/noyau/codage.c` (segment `"codage"`), appelé par le moniteur.
- Alternatives envisagées : une erreur de classe F pour l'espace temporaire — écartée : EX 52 est la nature du dépassement (IMP-0253), et aucune forme n'est attestée pour une commande.
- Justification : choix d'Éric du 2026-10-27 (G2) ; INRDP75 : « DECODER fait appel au décompilateur […] et au bibliothécaire ».
- Impact mémoire : le texte d'une ligne. Impact disque : le fichier temporaire. Impact performances : une décompilation par ligne. Impact historique : commande historique.
- Références normatives : VM-2203 (décompilation), VM-3404 ; IV-IF-0004.
- Statut : retenu (Éric, 2026-10-27, G2).

**IMP-0261** — *ENCODER ω{,n1{,n2}}*
- Sujet : réalisation de la commande (INRDP75, 5ᵉ partie, §4-2 ; LSE15 §9-3).
- Choix :
  - le fichier est cherché temporaire d'abord, puis permanent (INRDP75 : ENCODER s'applique aux deux) ; absent : F 05 ; protection L d'un autre compte : F 06 ; un enregistrement qui n'est pas une chaîne : F 101 ;
  - les enregistrements 1, 2, … sont lus jusqu'au NUL ou jusqu'au premier enregistrement absent ; X-OFF et CR séparent les lignes (CR pour un fichier fabriqué par un programme L.S.E.) ; LF est ignoré ; une ligne vide est sautée ;
  - **MITRA 15** (LSE15) : sans n1, le programme courant est remplacé, comme par APPELER ; avec n1, seules les lignes de n1 à n2 sont ajoutées, et remplacent les lignes de même numéro (`ENCODER ω` diffère de `ENCODER ω,1,250`) ;
  - **T1600** (INRDP75) : les lignes de n1 à n2 (toutes sans n1) s'ajoutent toujours au programme courant, comme à la lecture d'un ruban ;
  - chaque ligne est compilée comme si elle était tapée ; la première erreur arrête l'encodage, les lignes déjà entrées restent (INRDP75) : le moniteur affiche la ligne fautive, puis le message d'erreur ; une ligne qui ne commence pas par un numéro arrête l'encodage par `????` ; un programme plein donne la ligne non entrée puis son message (LSE15 : « spécifiant le numéro de la première ligne qui n'a pu être entrée ») ;
  - le programme courant passe à l'état « aucun » quand il change : au remplacement, ou dès qu'une première ligne est ajoutée ou remplacée ; si aucune ligne n'est entrée (fichier vide, aucune ligne entre n1 et n2, première ligne refusée), son état est conservé, et CONTINUER reste possible (LSE15 §9-2 : CONTINUER est refusé si une ligne a été entrée ; révisé le 2026-10-30, choix d'Éric, audit du code, §9 point 13) ; le remplacement libère un programme secret, un ajout à un programme secret donne F 06.
- Alternatives envisagées : n'accepter que X-OFF comme séparateur — écartée : un programme de service écrit en L.S.E. (LSE15) peut construire ses lignes avec `EQC(13)`.
- Justification : choix d'Éric du 2026-10-27 (G3) ; INRDP75, PG67 et PG67 bis ; LSE15 §9-3.
- Impact mémoire : un enregistrement (252 octets) à la fois. Impact disque : nul. Impact performances : une lecture par enregistrement, une compilation par ligne. Impact historique : reproduit la différence entre les deux profils.
- Références normatives : VM-3404 ; IV-IF-0001 (compiler), IV-IF-0002 (insérer).
- Statut : retenu (Éric, 2026-10-27, G3).

**Tests (étape 7e1).** `tests/composants/composants_codage.c` (24 contrôles, bibliothèque `tests:BIBCOD`) : PG66 bis (MITRA 15, deux enregistrements de 252, reprise du tampon) et PG66 (T1600, 220 et 82 caractères, NUL final) à l'octet près ; relecture ligne à ligne ; n1, n2 ; programme vide ; CR et LF ; F 05, F 101. `tests/composants/composants_moniteur.c` : ENCODER sous MITRA 15 (remplacement, première erreur, ajout avec n1), sous T1600 (ajout, `????` pour une ligne sans numéro), DECODER puis ENCODER, F 05.

## 7 sexies. Ruban, PERFORER, SILENCE (étape 7e2)

**IMP-0262** — *Le ruban est un fichier texte*
- Sujet : comment réaliser sur l'Apple IIGS le perforateur et le lecteur de ruban de la télétype (INRDP75 §4-1, §4-2, annexe II §3 et §4 ; LSE15 §9-2) ?
- Choix : un seul fichier tient lieu de ruban pour le perforateur et le lecteur : `RUBAN` dans la racine de la bibliothèque, ou le fichier désigné par l'option `-RUBAN chemin` (une disquette, par exemple) ; il est créé de type ProDOS TXT, que l'éditeur d'ORCA ouvre : un programme peut ainsi être tapé hors du L.S.E. puis lu ; interface : espace `ESPACE_RUBAN`, `plateforme_ruban_designer`, `plateforme_fichier_creer_texte` ; module `src/noyau/ruban.c` (segment `"codage"`).
- Alternatives envisagées : deux fichiers, l'un perforé, l'autre lu — écartée : un seul permet de relire ce qu'on vient de perforer, comme on transportait le ruban du perforateur au lecteur ; le port série — écarté pour la v1.
- Justification : choix d'Éric du 2026-10-27 (H1).
- Impact mémoire : un bloc de lecture de 256 octets. Impact disque : le fichier du ruban. Impact performances : une ouverture par bloc de 256 octets lus. Impact historique : le support change, le comportement des commandes est conservé.
- Références normatives : VM-2201 ; Construction (option de lancement).
- Statut : retenu (Éric, 2026-10-27, H1).

**IMP-0263** — *Lecture du ruban : RUBAN (MITRA 15), X-ON (T1600)*
- Sujet : mise en marche du lecteur et lecture.
- Choix :
  - **MITRA 15** : la commande RUBAN met le lecteur en marche et entraîne SILENCE : rien de ce qui est lu ou tapé n'est affiché, les complétions et les messages du système le sont (INRDP75, annexe II §3.3 et p. 189) ;
  - **T1600** : il n'y a pas de commande ; « il suffit de mettre en route le lecteur » ; X-ON (Ctrl-Q), frappé sur une ligne vide, met le lecteur en marche, comme sur la télétype ASR-33 où X-ON démarrait le lecteur et X-OFF l'arrêtait ; tout ce qui est lu est imprimé ; sous MITRA 15, X-ON est sans effet ;
  - toute demande de saisie (ligne, commande, paramètre, réponse à LIRE, identification) lit le ruban jusqu'au prochain CR (fin RETURN) ou X-OFF (fin X-OFF) ; LF et NUL sont ignorés (amorce, inertie du lecteur, INRDP75 p. 192) ; une erreur de compilation est affichée et la lecture continue ;
  - le lecteur s'arrête à la fin du ruban, qui revient alors au début, à un ESC perforé (INRDP75 p. 191), ou à ESC frappé au clavier ; sinon la lecture reprend où elle s'était arrêtée ;
  - ESC, d'où qu'il vienne, arrête le lecteur et annule SILENCE.
- Alternatives envisagées : la commande RUBAN aussi sous T1600 — écartée : elle n'y existe pas (INRDP75 §4-1).
- Justification : choix d'Éric du 2026-10-27 (H3) ; INRDP75 §4-1, annexe II §3.3, §4.
- Impact mémoire : nul. Impact disque : nul. Impact performances : nul. Impact historique : reproduit les deux manières de lire un ruban.
- Références normatives : VM-2201 ; Moniteur et console IMP-0237 à IMP-0239 ; interface console : `SAISIE_XON`.
- Statut : retenu (Éric, 2026-10-27, H3).

**IMP-0264** — *PERFORER A PARTIR DE n1{,n2}*
- Sujet : réalisation de la commande (INRDP75 §4-2, annexe II §3.2 ; LSE15 §9-2).
- Choix : le programme est listé comme par LISTER (la télétype imprimait ce qu'elle perforait) et ses lignes sont ajoutées à la fin du ruban, chacune suivie de CR, à la façon d'un texte ProDOS, sans LF ni amorce de NUL ; ESC interrompt (LSE15) ; un programme secret donne F 06 ; un ruban plein F 02, une erreur d'écriture F 27.
- Alternatives envisagées : CR LF et amorce de NUL comme sur le ruban d'INRDP75 (p. 188) — écartée : un fichier texte ProDOS sépare les lignes par CR, et le lecteur ignore LF et NUL.
- Justification : choix d'Éric du 2026-10-27 (H2).
- Impact mémoire : nul. Impact disque : le ruban s'allonge. Impact performances : une écriture par ligne. Impact historique : commande historique, sur un autre support.
- Références normatives : VM-2201, VM-2203 ; IV-IF-0004.
- Statut : retenu (Éric, 2026-10-27, H2).

**IMP-0265** — *SILENCE réalisé*
- Sujet : réalisation de SILENCE (MITRA 15), qu'IMP-0231 définit.
- Choix : SILENCE coupe l'écho de la saisie (`console_echo(0)`) jusqu'à ESC ; les complétions, les résultats et les messages restent affichés ; une saisie sans écho terminée par X-OFF ne marque pas la ligne comme commencée ; l'écho rétabli après IDENTIFICATION respecte SILENCE.
- Alternatives envisagées : aucune.
- Justification : INRDP75 §4-1 (« supprime l'affichage de tout ce que l'utilisateur tape au clavier ; l'effet est annulé par la touche ESC ») ; choix d'Éric du 2026-10-27 (H4).
- Impact mémoire, disque, performances : nuls. Impact historique : commande historique réalisée.
- Références normatives : VM-2201 ; Moniteur et console IMP-0231.
- Statut : retenu (Éric, 2026-10-27, H4).

**Tests (étape 7e2).** `tests/composants/composants_moniteur.c` (94 contrôles) : PERFORER (listage et contenu du ruban), RUBAN sous MITRA 15 (lecture silencieuse, complétion seule affichée, fin du ruban, ESC et fin de SILENCE), X-ON sous T1600 (lignes imprimées), commande EXECUTER et réponse à LIRE lues sur le ruban, ESC perforé puis reprise, SILENCE.

**IMP-0274** — *Perforateur commandé par un programme : EQC(18) et EQC(20)*
- Sujet : comment réaliser le perforateur que commande un programme (INRDP75 annexe V, l. 8989 et l. 8991 : `EQC(18)` « mise en route du perforateur », `EQC(20)` « arrêt du perforateur ») ?
- Choix :
  - sous les deux profils, dans ce qu'imprime un programme (AFFICHER, `?`), `EQC(18)` met le perforateur en marche et `EQC(20)` l'arrête ; ces deux caractères ne sont pas perforés ;
  - en marche, tout ce que le programme imprime est ajouté à la fin du ruban, tel quel : X-OFF, NUL, et le CR et le LF de `/`, `C` et `L` ; aucune fin de ligne n'est ajoutée, à la différence de PERFORER (IMP-0264) ;
  - ni les messages du moniteur ni ce qui est frappé ne sont perforés ;
  - l'attente d'une réponse à `LIRE` n'arrête pas le perforateur ; tout retour au moniteur l'arrête : TERMINER, PAUSE, pas à pas, erreur, ESC, fin d'une ligne de machine de bureau ;
  - chaque ajout ouvre et ferme le ruban : aucun fichier ne reste ouvert entre deux ajouts ;
  - un ajout impossible arrête le perforateur et le programme : EX 52 ;
  - module `src/noyau/ruban.c` : `ruban_sortie`, `ruban_sortie_commande`, `ruban_sortie_en_echec`, `ruban_perforateur_arreter`.
- Alternatives envisagées : perforer aussi les messages et la frappe, comme la télétype — écartée : un message sur le ruban serait relu comme une donnée ; laisser le perforateur en marche au retour au moniteur, comme la télétype — écartée : le LSE-IIGS n'a pas de bouton d'arrêt, et un programme interrompu par ESC perforerait aussi l'exécution suivante ; perforer `EQC(18)` et `EQC(20)` — écartée : le lecteur ne les ignore pas et ils entreraient dans les chaînes lues ; garder le ruban ouvert de `EQC(18)` à `EQC(20)`, ou un tampon — écartée : le lecteur et PERFORER ouvrent le même fichier, et un programme arrêté laisserait un fichier ouvert ou un tampon à vider.
- Justification : INRDP75 annexe V ; LSE15 l. 2281 (la touche d'interruption arrête une perforation) ; reconstitution de PERFO, `02-HISTORIQUE/Reconstitution-PERFO-CHARG/README.md` ; choix d'Éric du 2026-10-31.
- Impact mémoire : deux indicateurs. Impact disque : le ruban s'allonge. Impact performances : une ouverture par suite de caractères perforés, et un parcours du texte imprimé, perforateur arrêté ou non. Impact historique : rend possibles PERFO et tout programme qui perfore ; la télétype, elle, perforait aussi les messages.
- Références normatives : VM-2201, VM-2203 ; INRDP75 annexe V.
- Statut : retenu (Éric, 2026-10-31).

**IMP-0275** — *EQC(17) à EQC(21) ne s'impriment pas*
- Sujet : que deviennent ces cinq caractères dans ce qu'imprime un programme ? Sur la télétype, ce sont des ordres au lecteur et au perforateur, qui n'impriment rien (INRDP75 l. 8988-8992 ; `APCHARGEX1PRONO` dans les sorties de PERFO) ; le pilote `.CONSOLE` leur donne un autre sens (*GS/OS Reference*, chap. 9, p. 244) : `$11` 40 colonnes, `$12` 80 colonnes, `$13` efface du début de la fenêtre de texte au curseur, `$14` et `$15` prennent l'octet suivant pour opérande.
- Choix :
  - sous les deux profils, ces cinq caractères ne sont pas écrits à la console et ne comptent pas pour la colonne ; le filtre est dans le noyau, `ruban_sortie` ;
  - `EQC(17)`, `EQC(19)` et `EQC(21)` sont perforés comme tout caractère quand le perforateur est en marche ;
  - `EQC(17)` et `EQC(21)` sont sans effet sur le lecteur, que commandent RUBAN, X-ON frappé et ESC (IMP-0263) ;
  - tous les autres codes de commande vont à la console (`102 AFFICHER .27 12.`, SINUS).
- Alternatives envisagées : filtrer dans `console_iigs.c` — écartée : la règle tient à la télétype, non à l'Apple IIGS, et le bouchon de console ne la verrait pas ; retirer tous les codes sauf BEL, CR et LF — écartée : retirerait des codes qu'un programme attesté emploie ; faire commander le lecteur par `EQC(17)` et `EQC(21)` — non retenue : aucun programme attesté ne l'emploie, et l'arbitrage du 2026-10-31 porte sur le perforateur.
- Justification : INRDP75 annexe V ; *GS/OS Reference*, chap. 9 ; choix d'Éric du 2026-10-31.
- Impact mémoire : nul. Impact disque : nul. Impact performances : celui du parcours (IMP-0274). Impact historique : rétablit le silence de ces caractères à l'impression.
- Références normatives : VM-2201 ; Interpréteur IMP-0220, IMP-0221.
- Statut : retenu (Éric, 2026-10-31).

**IMP-0282** — *La console reçoit les octets de `$80` à `$FF` réduits à leurs 7 bits*
- Sujet : sous MITRA 15, `EQC` rend un caractère de code 128 à 255 (INRDP75 §6-10 ; RCI-M, sous-code `0x26`). Que voit-on à la console quand un programme l'imprime ? Le pilote `.CONSOLE` affiche `$80` à `$BF` et `$E0` à `$FF` en vidéo inverse, et `$C0` à `$DF` en MouseText (*GS/OS Reference*, chap. 9, tableau 9-3) : `?EQC(200)` affichait une flèche (essai 8.5 du 2026-11-05, `05-IMPLEMENTATION/audits/Essais_IIGS_2026-11-05.md`). Aucune source versée ne décrit ce qu'imprimaient les consoles du MITRA 15 pour ces codes.
- Choix :
  - La console est traitée comme un terminal ASCII à 7 bits, qui ignore le huitième. Dans `ruban_sortie`, un octet de `$80` à `$FF` imprimé par un programme va à la console réduit à ses 7 bits : `EQC(200)` s'affiche `H`, `EQC(141)` agit comme `EQC(13)`. Il compte pour la colonne comme le caractère obtenu.
  - S'il devient l'un des ordres à la télétype `$11` à `$15` (`EQC(145)` à `EQC(149)`), il n'est pas écrit à la console, comme `EQC(17)` à `EQC(21)` (IMP-0275). Les autres codes de commande obtenus vont à la console.
  - `EQC(223)` et `EQC(222)` donnent `$5F` et `$5E`, qui s'affichent `←` et `↑` (Moniteur et console, IMP-0281).
  - Le ruban reçoit l'octet entier, et seuls `EQC(18)` et `EQC(20)` commandent le perforateur (IMP-0274). Les chaînes, les fichiers-données et la représentation compilée gardent aussi l'octet entier.
  - La règle vaut sous les trois profils ; seul MITRA 15 produit de tels octets par `EQC`.
- Alternatives envisagées :
  1. Laisser le pilote afficher ces octets à sa manière — écartée (Éric, 2026-11-05) : vidéo inverse et MouseText ne correspondent à aucun terminal de l'époque.
  2. Faire la réduction dans `console_iigs.c` — écartée : la règle tient au terminal historique, non à l'Apple IIGS, et le filtre d'IMP-0275, dans le noyau, devrait y être répété. Le bouchon de console ne la verrait pas.
  3. Faire aussi commander le perforateur par `$92` et `$94` — non retenue : aucun programme attesté ne l'emploie, et la décision ne porte que sur l'affichage.
- Justification : choix d'Éric du 2026-11-05, option (b). C'est une conjecture : un terminal ASCII de l'époque recevait 7 bits de données, le huitième servant de parité.
- Impact mémoire : nul. Impact disque : nul. Impact performances : nul hors de ces octets, l'octet étant examiné par le parcours d'IMP-0274.
- Impact historique : remplace un affichage propre à l'Apple IIGS par celui d'un terminal à 7 bits ; le contenu imprimé, le ruban et les fichiers sont inchangés.
- Références normatives : VM-2201 ; Interpréteur IMP-0220, IMP-0221 ; IMP-0274, IMP-0275 ; Moniteur et console IMP-0281.
- Statut : retenu (Éric, 2026-11-05) ; vérifié sur l'Apple IIGS le 2026-11-05 (essais 8.10 à 8.12 : `H` en vidéo normale, `A←↑B`, `AB` sans effacement).

**Tests (IMP-0274, IMP-0275, IMP-0282).** `tests/composants/composants_moniteur.c` : perforé, non perforé, ajout à la fin, X-OFF, NUL, CR et LF de `/`, deux profils, frappe et messages non perforés, `LIRE` sans arrêt, arrêt après TERMINER, une erreur et une ligne de machine de bureau, EX 52. Golden Gate range CR en LF dans un fichier TXT et relit tout LF en CR : la suite lit `0D 0D` pour le CR LF d'un ruban. `tests/composants/composants_entrees_sorties.c` : rien à la console, colonne inchangée ; sous MITRA 15, `EQC(200)`, `EQC(223)`, `EQC(222)`, `EQC(255)`, `EQC(141)` réduits à 7 bits, `EQC(145)` à `EQC(149)` écartés (IMP-0282). `02-HISTORIQUE/Reconstitution-PERFO-CHARG/essais/verifier.py` : PERFO et CHARG de bout en bout. Sur l'Apple IIGS, le filtre reste à essayer.

---

## 8. Points renvoyés aux étapes suivantes

- Étape distincte : éditeur de texte ligne (MEMENTO82 §4 ; témoignage d'Éric pour le T1600).

---

## 9. Récapitulatif des décisions `IMP-*` de ce chapitre

| Identifiant | Sujet | Statut |
|---|---|---|
| IMP-0245 | Bibliothèque : `-BIB`, défaut `@:LSE`, sous-répertoires, temporaires vidés à BONJOUR et AU REVOIR | retenu |
| IMP-0246 | RCI-S : en-tête, symboles, constantes, lignes RCI-M, somme de contrôle ; restauration vérifiée d'abord | retenu |
| IMP-0247 | Fichier-données en secteurs de 256 octets, répertoire chaîné, étendues réutilisées | retenu |
| IMP-0248 | `CATALOGUE` : entrées de 16 octets mises à jour sur place | retenu (format) |
| IMP-0249 | `interface_fichiers.h` par espace et nom, accès par position ; `bibliotheque.c`, `fichiers.c` | retenu |
| IMP-0250 | Classe F : numéros de MEMENTO82, `ERREUR F nn`, ` EN LIGNE lll` à l'exécution | retenu |
| IMP-0251 | RANGER, APPELER, MODIFIER, SUPPRIMER ; droits par IMP-0255 ; noms uniques par sorte (T1600) ou toutes sortes confondues (MITRA 15) ; pas de F 25 | retenu |
| IMP-0252 | EXECUTER : `FIN_EXECUTER`, ligne 1 par défaut, EX 75, EX 105, interdit en machine de bureau (C 58) | retenu |
| IMP-0253 | GARER, CHARGER, SUPPRIMER, CATALOGUER ; espace temporaire de 200 secteurs (`-TEMP`) | retenu |
| IMP-0254 | Identifications 0 et 1 fixes, 9 autres dans `IDENTIFICATIONS`, persistantes ; propriété par compte | retenu |
| IMP-0255 | Droits P, D, propriété (F 22), `,E` et S (F 11), protection L et `!`, programme secret | retenu |
| IMP-0256 | IDENTIFICATION : sans écho, `????`, retour chariot interligne, espace `x` et « n », temporaires détruits | retenu |
| IMP-0257 | Initialisateur en état dormant (CR, SU, LI) ; TABLE et UTILISATION `A PARTIR DE XXXX[,P|D|T]`, ordre alphabétique, ESC ; secteurs libres par GetDevNumber, DInfo et Volume (complétée le 2026-11-06) | retenu |
| IMP-0258 | DUPLIQUER `[$]motif[,P|D]` dans les deux sens, sous `LSE F4` ; catalogue de l'unité ; `-UNITE` ; F 29 | retenu |
| IMP-0278 | `$` devant un nom de fichier, sous `LSE F4` : fichier sur l'unité amovible, pour toutes les commandes à nom de fichier | retenu |
| IMP-0268 | Option `-ID n` : identification initiale ; `VOUS ETES SOUS ID c` sous `LSE F4` | retenu |
| IMP-0259 | Fichier décodé : X-OFF, NUL final, 252 caractères ; MITRA 15 lignes coupées, T1600 lignes entières | retenu |
| IMP-0260 | DECODER ω{,n1{,n2}} : fichier temporaire, F 06, EX 52 | retenu |
| IMP-0261 | ENCODER ω{,n1{,n2}} : remplacement (MITRA 15 sans n1) ou ajout ; première erreur arrête | retenu |
| IMP-0262 | Ruban = fichier texte `RUBAN` ou `-RUBAN chemin` | retenu |
| IMP-0263 | Lecture du ruban : RUBAN (MITRA 15, SILENCE), X-ON (T1600, imprimé) ; arrêt à la fin, à ESC | retenu |
| IMP-0264 | PERFORER : listage et ajout au ruban, lignes terminées par CR | retenu |
| IMP-0265 | SILENCE : écho coupé jusqu'à ESC | retenu |
| IMP-0274 | Perforateur commandé par un programme : `EQC(18)`, `EQC(20)` ; arrêt à chaque retour au moniteur ; EX 52 | retenu |
| IMP-0275 | `EQC(17)` à `EQC(21)` ne vont pas à la console | retenu |
| IMP-0282 | Console à 7 bits : un octet de `$80` à `$FF` imprimé par un programme y va réduit à ses 7 bits, sauf s'il devient `$11` à `$15` ; ruban et fichiers inchangés | retenu |

---

## 10. Traçabilité

| EXI | VM | IV | IMP | Code | Test |
|---|---|---|---|---|---|
| EXI-0009, EXI-0022, EXI-0025, EXI-0049 | VM-2209, VM-3401 à VM-3403 | IV-IF-0007 | IMP-0245, IMP-0246, IMP-0249, IMP-0251, IMP-0252 | `src/noyau/bibliotheque.c`, `src/noyau/moniteur.c` (commandes), `src/noyau/interprete.c` (EXECUTER) | `tests/composants/composants_bibliotheque.c`, `tests/composants/composants_moniteur.c` |
| EXI-0053 à EXI-0057 | VM-3404, VM-3405, VM-3407 | IV-IF-0005, IV-IF-0006 | IMP-0247, IMP-0248, IMP-0253 | `src/noyau/fichiers.c`, `src/noyau/interprete.c` (GARER, CHARGER, SUPPRIMER), `src/noyau/moniteur.c` (CATALOGUER), `src/noyau/contexte_exec.c` (déclaration implicite) | `tests/composants/composants_fichiers.c`, `tests/composants/composants_moniteur.c` |
| EXI-0009, EXI-0049, EXI-0053 à EXI-0057 | VM-2201, VM-3401, VM-3404 | IV-IF-0007 | IMP-0254 à IMP-0258 | `src/noyau/bibliotheque.c` (identifications, droits, DUPLIQUER), `src/noyau/fichiers.c` (droits sur `#`), `src/noyau/moniteur.c` (IDENTIFICATION, initialisateur, tables), `src/noyau/analyseur.c`, `src/plateforme/main.c` (`-UNITE`), `src/plateforme/fichiers_iigs.c` (Volume) | `tests/composants/composants_moniteur.c`, `tests/composants/composants_analyseur.c` |
| EXI-0057 | VM-2203, VM-3404 | IV-IF-0001, IV-IF-0002, IV-IF-0004 | IMP-0259 à IMP-0261 | `src/noyau/codage.c`, `src/noyau/moniteur.c` (DECODER, ENCODER) | `tests/composants/composants_codage.c`, `tests/composants/composants_moniteur.c` |
| EXI-0028 | VM-2201 | IV-IF-0004 | IMP-0262 à IMP-0265, IMP-0274, IMP-0275, IMP-0282 | `src/noyau/ruban.c`, `src/noyau/interprete.c` (sortie d'un programme), `src/noyau/moniteur.c` (RUBAN, PERFORER, SILENCE, lecture), `src/plateforme/console_iigs.c` (X-ON), `src/plateforme/fichiers_iigs.c` (`ESPACE_RUBAN`, TXT), `src/plateforme/main.c` (`-RUBAN`) | `tests/composants/composants_moniteur.c`, `tests/composants/composants_entrees_sorties.c` |
| — | VM-3401, VM-3405 | — | IMP-0249 | `src/interfaces/interface_fichiers.h`, `src/plateforme/fichiers_iigs.c` | `tests/plateforme/sonde_fichiers.c`, `tests/plateforme/plateforme_fichiers.c` |
