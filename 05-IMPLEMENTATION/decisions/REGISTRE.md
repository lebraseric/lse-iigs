# Registre des décisions IMP

Les décisions sont rédigées dans les chapitres du Tome V (`../tome-v/`). Ce registre sert à éviter toute réutilisation d'identifiant.

| Plage | Chapitre d'origine | Remarque |
|---|---|---|
| IMP-0101 à IMP-0128 | RCI-M (spécification physique) | IMP-0109 bloc transitoire machine de bureau ; IMP-0112 représentation de FAIRE en trois éléments (ouverture 0x09, pas 0x0A/0x0E, test 0x0F/0x10), réécrite le 2026-10-22 (audit A1) ; IMP-0115 AFFICHER (marques de tête 0x37/0x38, tableau décidé à l'exécution par 0x36), réécrite le 2026-10-22 (audit A2) ; IMP-0107 table des constantes partagées (structure, durée de vie), réécrite le 2026-10-22 (audit B2) ; IMP-0110 valeur des variables portée par l'environnement des identificateurs (état d'exécution), réécrite le 2026-10-22 (audit A5) ; IMP-0125 numéro de ligne sur 1 octet |
| IMP-0130 à IMP-0136 | Architecture physique / Organisation du dépôt | IMP-0130 tâche unique ; IMP-0132/0133 allocation ; IMP-0134 mémoire ; IMP-0136 localisation des modules |
| IMP-0137 à IMP-0143 | Construction | — |
| IMP-0144 à IMP-0155 | État d'exécution | Statut « retenu » (2026-10-22) ; IMP-0147 (table des paramètres formels) est rédigée dans le chapitre Compilateur, §13.3 ; IMP-0148, IMP-0149 et IMP-0151 révisées le 2026-10-23 (point D1 : l'alias capture une case, une case neuve par identificateur LOCAL) ; IMP-0146 complétée le 2026-10-24 (base de cadre, point B2) |
| IMP-0157 à IMP-0163 | Gestionnaire d'objets | — |
| IMP-0164 à IMP-0173 | Compilateur (général) | IMP-0168 provisoire (diagnostic) |
| IMP-0174 à IMP-0176 | Compilateur, §13 (déclaration de procédure et clause `LOCAL`) | IMP-0174 élément de contrôle 0x0D (reporté dans RCI-M §4.7) ; IMP-0175 analyse de la déclaration, rejet des doublons et du paramètre homonyme de la procédure (révisée le 2026-10-22) ; IMP-0176 contrat d'intégration du programme courant ; statut « retenu » |
| IMP-0177 | RCI-M (spécification physique), §4.11.5 | Nombre d'arguments effectifs `k`, second octet de charge utile de l'élément APPEL (0x28) ; vérification de LSE15 §7-8 à l'exécution ; statut « retenu » |
| IMP-0178 | Compilateur, §5 (génération d'AFFICHER) | Contrôle de LSE15 §6-2.4 (pas plus de spécifications de description que de valeurs) ; statut « retenu » |
| IMP-0179 | État d'exécution, §3 bis | Environnement des identificateurs (état non défini, variable, chaîne, tableau ; valeur ou handle), indexé par l'identifiant de symbole ; règles de transition ; état « paramètre LOCAL sans valeur » et agrandissement paresseux ajoutés le 2026-10-23 ; nom séparé de la case le 2026-10-23 (D1) ; statut « retenu » |
| IMP-0180 | Compilateur, §8 bis | Profil d'implantation (`LSE F4`, T1600 version 4, MITRA 15 version 8) choisi au lancement, fixe pour la session ; profil strict ; mots réservés selon INRDP75 Annexe VI ; statut « retenu » (révisé le 2026-11-01) |
| IMP-0181 | Compilateur, §7 | Chaînes littérales : au-delà du seuil de partage, toujours par la table des constantes partagées (lignes de programme) ; toujours en ligne en mode machine de bureau ; statut « retenu » |
| IMP-0182 | Compilateur, §5 bis | Règles lexicales : reconnaissance des mots, signe des constantes, espace après le numéro, majuscules seules hors chaînes et commentaires ; statut « retenu » |
| IMP-0183 | RCI-M (spécification physique), §4.12 | Règles de restitution (décompilation) ; marque de tête de LIRE, sous-code de contrôle 0x11 ; amendée le 2026-10-23 : blocs `DEBUT…FIN` (branche vide, SI ouvert), chaînes sous MITRA 15 telles quelles, commentaire en tête collé au numéro ; amendée le 2026-11-01 : libellés de format restitués comme les constantes-chaîne ; statut « retenu » |
| IMP-0184 | RCI-M (spécification physique), §4.1 | Ordre des octets : octet de poids faible en tête pour tout champ de plusieurs octets (ordre natif du 65816 et d'ORCA/C) ; statut « retenu » |
| IMP-0185 | Gestionnaire des erreurs, §3 | Numérotation des codes d'erreur : catalogue par nature ; numéro MEMENTO82 s'il existe, sinon à partir de 100 ; C 63 pour la ligne trop longue, sous réserve ; même numérotation pour les deux profils ; statut « retenu » |
| IMP-0186 | Gestionnaire des erreurs, §4 | Forme des messages : `ERREUR  C nn`, `ERREUR (EX.) nn EN LIGNE lll` ou `EN LIGNE BUR`, `ERREUR F nn` pour les deux profils ; pas de ligne d'appel ; code seul par défaut, libellé sur paramètre de lancement `-LIBELLES` ; liste des erreurs livrée ; statut « retenu » |
| IMP-0187 | Gestionnaire des erreurs, §5 | Granularité : une nature par situation à corriger ; numéro MEMENTO82 partagé par les natures qu'il regroupe ; libellé propre à chaque nature ; statut « retenu » |
| IMP-0188 | Compilateur, §5 ter | Contrôle de type limité au texte de la ligne : type statique numérique, chaîne ou indéterminé ; rejet seulement quand deux types déterminés se contredisent (C 11, C 14, C 15, C 27, C 28) ; statut « retenu » |
| IMP-0189 | Compilateur, §8 bis | `TEM()` et `DAT()` admis sous le profil `LSE F4` en plus de `TEM` et `DAT` (`TEM()` attesté à Victor Duruy), rejetés sous le T1600 version 4 ; restitution avec parenthèses sous `LSE F4`, sans sous T1600 version 4 ; statut « retenu » (révisé le 2026-11-01) |
| IMP-0190 | Compilateur, §8 bis | Équivalents numériques `.n.` dans une constante-chaîne, en opérande et en libellé de format, profil `LSE F4` seulement ; bornes d'`EQC` ; restitution des caractères non imprimables (reportée dans RCI-M IMP-0183 et la fiche de codage §9) ; statut « retenu » (révisé le 2026-11-01) |
| IMP-0191 | Compilateur, §9 bis | Règles de détection complémentaires (identificateur trop long C 03/C 05, constante hors plage C 09, bornes C 52 et au-delà du profil, fin de ligne inattendue C 45/C 19/C 53, `?` sur ligne numérotée, ressources C 57) et position du diagnostic ; statut « retenu » |
| IMP-0192 | Construction, §7 bis | Suites de tests compilées et exécutées sous Golden Gate ; bouchons dans `tests/bouchons/` ; `make tests` ; contraintes Golden Gate constatées (FreeMem, RealFreeMem absents) ; statut « retenu » |
| IMP-0193 | Organisation du dépôt, §2.1 | Modules `rcim.c` et `symboles.c` dans un segment statique dédié `"rcim"` ; complète IMP-0135 et le Makefile ; statut « retenu » |
| IMP-0194 | Gestionnaire d'objets, §11 | Objet de longueur nulle : un octet réservé ; statut « retenu » |
| IMP-0195 | Gestionnaire des erreurs, §8 | `NatureErreur` (`unsigned int`), `DiagnosticCompilation` = nature + position (remplace IMP-0168), `DiagnosticExecution` = nature + ligne (0 : machine de bureau ; ajout du 2026-10-23), catalogue et libellés compilés dans `erreurs.c` ; statut « retenu » |
| IMP-0196 | Compilateur, §8 bis | `ConfigurationSession` (profil, libellés) dans `types_communs.h`, instance unique définie par `main.c`, propre à chaque suite de tests ; statut « retenu » |
| IMP-0197 | Architecture physique, §5.2 | Un processeur dont le code dépasse 64 Kio occupe plusieurs segments statiques nommés (noms de 10 caractères au plus) ; compilateur : `"compilateur"` et `"compil_ins"` ; statut « retenu » |

| IMP-0198 | Décompilateur, §3 | Interface `decompilateur_decompiler` : une entrée du programme courant vers un texte de 255 caractères au plus ; contexte de décompilation = profil ; statut « retenu » |
| IMP-0199 | Décompilateur, §4 | Restitution en une passe, un seul tampon de texte, une pile de 128 entrées ; ni arbre ni récursion ; statut « retenu » |
| IMP-0200 | Décompilateur, §6 | Échec de la décompilation : trois causes (texte trop long, séquence impossible à décoder, référence inconnue), jamais de troncature ; statut « retenu » |
| IMP-0201 | Décompilateur, §7 | Écriture des réels à six chiffres significatifs dans le noyau, sans `stdio` ni appel SANE ; statut « retenu » |
| IMP-0202 | Décompilateur, §8 | Segment `"decompil"` (règle des 10 caractères, IMP-0197) ; statut « retenu » |
| IMP-0203 | Décompilateur, §9 | Tests du décompilateur : vecteurs, cas ciblés, aller-retour sur le corpus INRDP75 et LSE15 ; statut « retenu » |
| IMP-0204 | Programme courant, §2 | Table des objets compilés : tableau trié de capacité fixe 255, alloué une seule fois ; statut « retenu » |
| IMP-0205 | Programme courant, §3 | Interface du programme courant (IV-IF-0002, recherches) ; le programme courant possède les blocs ; échec sans modification (C 57) ; statut « retenu » |
| IMP-0206 | Programme courant, §5 | Deux niveaux d'effacement : une ligne (symboles et constantes inertes), ou tout le programme avec ses tables ; statut « retenu » |
| IMP-0207 | Programme courant, §6 | Compteur des lignes entrées, que la suppression ne fait pas avancer (CONTINUER, REPRENDRE EN) ; statut « retenu » |
| IMP-0208 | État d'exécution, §7.5 | Interfaces C de `pile` et `contexte_exec` : nature rendue (`ERR_AUCUNE` en cas de succès), usage de lecture (EX 05, 62, 10), frontière avec `donnees` (trois opérations, bouchon de test), fichiers `pile.c` (segment `"interprete"`) et `contexte_exec.c` (segment `"contexte"`) ; statut « retenu » |
| IMP-0209 | Données, §2 | Variable-chaîne : genre, longueur sur 2 octets, texte ; 65 535 caractères au plus sous les deux profils (EX 101 au-delà) ; statut « retenu » |
| IMP-0210 | Données, §5 | Tableau : en-tête, un bit par élément défini, `float` rangés ligne par ligne ; EX 27, 32, 45, 46, 49 ; statut « retenu » |
| IMP-0211 | Données, §4 | Écriture U à sept chiffres significatifs arrondis (T1600, MITRA 15, essais LSE 8086), suivie d'une espace ; F et E ; `CCA` = U sans espace final ; la décompilation garde six chiffres ; statut « retenu » |
| IMP-0212 | Données, §3.3 | Fonctions sur les chaînes : LSE15 §4-4.2 et compléments déduits (GRL et SKP sans résultat, positions) ; statut « retenu » |
| IMP-0213 | Données, §6 | Interface du module `donnees` ; segment `"donnees"` ; remplace le bouchon de l'étape 4c ; statut « retenu » |
| IMP-0214 | Interpréteur, §1 | Interface : `interprete_executer`, `interprete_bureau`, cinq fins d'exécution, état laissé intact ; statut « retenu » |
| IMP-0215 | Interpréteur, §2 | Copie de la séquence de la ligne en cours dans un tampon local ; statut « retenu » |
| IMP-0216 | Interpréteur, §3.1 | Arithmétique en double, rangée en float ; EX 25, 26, 31, 41, 42, 44 ; `0^0` = 1 (LSE 8086) ; statut « retenu » |
| IMP-0217 | Interpréteur, §3.4 | Cible calculée : arrondi, < 1 EX 37, > 32 767 EX 38, sinon ligne absente EX 75 ; statut « retenu » |
| IMP-0218 | Interpréteur, §3.5 | `UN` et `ZER` sur un non-tableau : EX 47 ; statut « retenu » |
| IMP-0219 | Interpréteur, §4 | GARER, CHARGER, SUPPRIMER, EXECUTER : EX 96 jusqu'à l'étape « fichiers » ; statut « clos » le 2026-10-25 (IMP-0252, IMP-0253) |
| IMP-0220 | Interpréteur, §4 bis | Interface console : écrire, retour chariot, interligne, bip, lire une ligne ; `/` = CR LF, `C` = CR, `L` = LF, rendus par la console ; statut « retenu » |
| IMP-0221 | Interpréteur, §4 bis | AFFICHER et `?` : passage à la ligne sans format, U, chaînes sans espace (PG5), format, facteur variable (EX 104), chaîne sous F ou E (EX 06), tableau entier, passage à la ligne avant lui seulement hors du début de ligne (LSE 8086) ; statut « retenu » |
| IMP-0222 | Interpréteur, §4 bis | LIRE : bip (et à la reprise après ESC), saisie fournie par le moniteur, reste de la ligne conservé pour le LIRE suivant (PG3, LSE 8086), RETURN rangé dans une chaîne, signe admis, chaîne = reste de la ligne, réponse mal formée : bip et « ? » (LSE 8086) ; statut « retenu » |

| IMP-0223 | Interpréteur, §4 ter | Interface horloge : `horloge_lire` (année, mois, jour, heure, minute, seconde), réalisée par `time()` et `localtime()` d'ORCA/C, bouchon à instant fixe ; statut « retenu » |
| IMP-0224 | Interpréteur, §4 ter | TEM en secondes entières depuis zéro heure ; DAT `JJ/MM/AA HH:MM:SS`, année sur deux chiffres ; divergences LSE 8086 consignées ; statut « retenu » |
| IMP-0225 | Interpréteur, §4 ter | ALE : congruentiel modulo 2²⁴ (a = 1 664 525, c = 7 271 263), résultat x/2²⁴ dans ]0,1[, ALE(1) admis, ALE(0) amorcé par l'horloge, EX 09 ; exemple d'INRDP75 non reproductible ; statut « retenu » |
| IMP-0226 | Interpréteur, §5 | Deux segments `"interprete"` et `"interp_es"` (IMP-0197), `horloge_iigs.c` dans `"interp_es"` ; statut « retenu » |

| IMP-0227 | Moniteur, §2 | `moniteur.c` (VM-2201) et `analyseur.c` (VM-2202), segment blanc, point d'entrée `moniteur_session` ; statut « retenu » |
| IMP-0228 | Moniteur, §4 | Lancement en état dormant ; `BO` affiche la date ; `AU` efface et rend la main au shell ORCA ; statut « retenu » |
| IMP-0229 | Moniteur, §3 | Classement : chiffre en tête = ligne de programme, deux lettres = commande, sinon machine de bureau ; statut « retenu » |
| IMP-0230 | Moniteur, §5 | Deux lettres et RETURN (X-OFF), complétion selon le manuel du profil, paramètres lus ensuite ; ABREGER, IN EXTENSO ; table des commandes ; statut « retenu » |
| IMP-0231 | Moniteur, §5 | `\` annule le caractère précédent dans toute saisie, réponses à LIRE comprises ; SILENCE sous MITRA 15 ; statut « retenu » |
| IMP-0232 | Moniteur, §6 | EXECUTER, CONTINUER, POURSUIVRE, REPRENDRE (`contexte_reprendre`), PAS A PAS (`FIN_PAS`, `LIGNE lll`), NORMAL ; conditions par le compteur d'IMP-0207 ; statut « retenu » |
| IMP-0233 | Moniteur, §6 | `TERMINE EN LIGNE nnn` (T1600), `TERMINE` (MITRA 15), `PAUSE EN LIGNE nnn`, `PRET EN LIGNE nnn`, `ARRET EN LIGNE nnn` (LSE 8086), sans ligne d'appel ; machine de bureau dans le cadre courant ; statut « retenu » |
| IMP-0234 | Moniteur, §7 | LISTER, NUMERO (champs de huit colonnes, LSE 8086), EFFACER (`n1 A n2`, `*`) ; statut « retenu » |
| IMP-0235 | Moniteur, §8 | Domaine `DIALOGUE` (six natures, Gestionnaire des erreurs §9 bis), restitué par `????` sans changer d'état ; statut « retenu » |
| IMP-0236 | Moniteur, §9 | Commandes de fichiers, RUBAN, PERFORER : EX 96 jusqu'à l'étape « fichiers » (ruban = fichier texte) ; PRENDRE ETAT CONSOLE 1 seul admis ; statut « retenu, provisoire » |

| IMP-0237 | Moniteur, §6 bis | Attente d'une réponse à LIRE : fin `FIN_LECTURE`, saisie par le moniteur, état « en attente d'entrée » ; ESC : `PRET EN LIGNE lll`, CONTINUER émet un bip et reprend sur la valeur en attente (LSE 8086) ; statut « retenu » |
| IMP-0238 | Moniteur, §6 bis | X-OFF (Ctrl-S) et RETURN, touches de fin distinctes : ligne vide + X-OFF = `????`, + RETURN = rien ; PAS A PAS relancé par X-OFF ; RETURN rangé dans une chaîne lue, CR LF, et fin de la valeur (PG3 ; points L5, L7) ; statut « retenu » |
| IMP-0239 | Moniteur, §9 | ESC seule (Ctrl-X de LSE 8086 non retenu), consultée au début de chaque ligne et pendant toute saisie ; statut « retenu » |

| IMP-0240 | Moniteur, §9 bis | Console de l'Apple IIGS : pilote `.CONSOLE` de GS/OS, lecture brute caractère par caractère, CR sans LF (mot `$15` = `$0D`), lecture sans attente pour ESC avec frappe d'avance conservée ; repli sur stdio sous Golden Gate (pilote absent, `$0010`) ; statut « retenu », à vérifier sur cible |

| IMP-0241 | Construction, §7 ter | `make` produit `build/LSE.po`, image ProDOS vierge de 800 Ko (volume `/LSE/`, `-pro800`, `scripts/creer_image.sh`, AppleCommander) ; complément du 2026-11-06 : `LSE.po`, image de distribution, porte `LSE` (EXE) et les rubans `CHARG` et `PERFO` (TXT, rangement automatique, `scripts/creer_ruban_programme.sh`) ; les sondes passent sur `ESSAIS.po` ; `PERFO.po` supprimée, `PRONO`, `FICH`, `FICH2` sur `DURUY.po` ; statut « retenu » (option b) |

Révision : IMP-0143 (Construction §5), le 2026-10-25 : profil `cible` en `#pragma debug 1` et `#pragma optimize 7` (variante R3, mesures `05-IMPLEMENTATION/audits/Mesures_performance_IIGS_2026-10-25.md`).

| IMP-0242 | Moniteur, §9 bis | Consultation d'ESC : bit 7 du registre du clavier `$C000` lu d'abord (0,017 ms), pilote `.CONSOLE` appelé seulement si une touche attend (0,52 ms) ; statut « retenu » |

| IMP-0243 | Gestionnaire d'objets, §11 | Accès sans `HLock`/`HUnlock` (pointeur maître lu, aucune adresse conservée) : en-têtes de bloc dans `objets.c`, `objets_copier` et `objets_ecrire` (décalage `unsigned long`) ; lignes, tableaux, chaînes, ligne d'entrée d'une procédure, premier octet d'une ligne insérée, constantes restituées ; verrou maintenu dans `objets_resoudre` ; `plateforme_bloc_adresse` ; statut « retenu » |

| IMP-0244 | Interpréteur, §3 | Chemin exact sans SANE : comparaisons de `float` par leurs bits, `+ - *` en entiers sous 2²⁴ (mêmes bits qu'IMP-0216, vérifiés par `composants_nombres`), conversions d'entiers construites, nombres passés par adresse ; statut « retenu » |

| IMP-0245 | Bibliothécaire et fichiers, §2 | Bibliothèque : option `-BIB`, défaut `@:LSE`, sous-répertoires PROGRAMMES, DONNEES, TEMPORAIRES, fichiers CATALOGUE et IDENTIFICATIONS ; temporaires vidés à BONJOUR et AU REVOIR ; priorité au temporaire ; statut « retenu » |
| IMP-0246 | Bibliothécaire et fichiers, §3 | RCI-S : en-tête `LSEP`, version, profil ; symboles, constantes, lignes RCI-M telles quelles ; somme de contrôle ; vérification complète avant remplacement ; F 23, F 100 ; statut « retenu » |
| IMP-0247 | Bibliothécaire et fichiers, §4 | Fichier-données en secteurs de 256 octets : répertoire chaîné (en-tête 16 octets, 20 entrées de 12), étendues, GARER hors place puis secteurs de répertoire, étendues libres réutilisées et découpées ; statut « retenu » |
| IMP-0248 | Bibliothécaire et fichiers, §5 | `CATALOGUE` : entrées de 16 octets (catégorie, nom, compte, date, protection E/L), mises à jour sur place ; statut « retenu » (format) |
| IMP-0249 | Bibliothécaire et fichiers, §6 | `interface_fichiers.h` : espace et nom, accès par position ; `fichiers_iigs.c` par stdio et GS/OS ; modules `bibliotheque.c` (`"bibliothe"`), `fichiers.c` ; tests sur fichiers réels sous Golden Gate ; statut « retenu » |
| IMP-0250 | Bibliothécaire et fichiers, §7 | Classe F : numéros de MEMENTO82 annexe III, `ERREUR F nn`, ` EN LIGNE lll` à l'exécution ; catalogue §9 ter du Gestionnaire des erreurs ; statut « retenu » |

| IMP-0251 | Bibliothécaire et fichiers, §7 bis | RANGER (`,E`, `,L` sous T1600), APPELER, MODIFIER (nom provisoire TRANSIT, pas de F 25), SUPPRIMER (ω sous MITRA 15 ; ω,P|D|T et * sous T1600) ; droits par IMP-0255 ; noms uniques entre programmes et données permanentes sous MITRA 15 (RANGER, CATALOGUER, GARER `'#…'`), au sein de chaque sorte sous T1600 (révisé le 2026-11-06) ; statut « retenu » |
| IMP-0252 | Bibliothécaire et fichiers, §7 bis | Instruction EXECUTER : `FIN_EXECUTER` rendue au moniteur, ligne 1 par défaut (EX 75 si absente, appelant intact), EX 105 si module absent, interdite en machine de bureau (C 58, LSE 8086) ; statut « retenu » |

| IMP-0253 | Bibliothécaire et fichiers, §7 ter | GARER, CHARGER (δ, déclaration implicite, EX 47), SUPPRIMER (enregistrement ou fichier ; EX 55, EX 56), CATALOGUER (recopie sans trous, F 03, F 04, F 05) ; β entier 1 à 32 767 (EX 50) ; γ `#` facultatif (EX 53) ; espace temporaire 200 secteurs, `-TEMP n` (EX 52) ; statut « retenu » |

| IMP-0254 | Bibliothécaire et fichiers, §7 quater | Identifications : 0 et 1 fixes, au plus 9 autres dans `IDENTIFICATIONS` (6 octets chacune), persistantes (divergence MEMENTO82) ; compte = identification modulo 100 ; propriété par compte ; statut « retenu » |
| IMP-0255 | Bibliothécaire et fichiers, §7 quater | Droits P (F 07), D (F 08), propriété (F 22), `RANGER ω,E` sous S (F 11), protection L et `!` (F 06, EX 54), programme secret (F 06) ; statut « retenu » |
| IMP-0256 | Bibliothécaire et fichiers, §7 quater | IDENTIFICATION : sans écho, `????`, réponse = retour chariot interligne, `NOMBRE DE SECTEURS DISPONIBLES:n` et `ESPACE TEMPORAIRE DESIRE:` sous x (LAFOND83), « n », `ESPACE TEMPORAIRE REDUIT A x. SECTEURS.`, temporaires détruits sous MITRA 15 (LSE15), conservés sous T1600 (INRDP75 p. 192) ; statut « retenu » |
| IMP-0257 | Bibliothécaire et fichiers, §7 quater | Initialisateur (CREATION ID, SUPPRESSION ID, LISTE ID) en état dormant ; TABLE DES FICHIERS et UTILISATION DISQUE `A PARTIR DE XXXX[,P|D|T]` (`*` T1600, `A` MITRA 15 ; type seul ; ordre alphabétique ; ESC), mise en page d'INRDP75 p. 133-134 ; UTILISATION sous `>` (MITRA 15) ; secteurs libres par GetDevNumber, DInfo et Volume sur le seul périphérique de la bibliothèque (complétée le 2026-11-06), permis seulement avec `.CONSOLE` ; statut « retenu » |
| IMP-0258 | Bibliothécaire et fichiers, §7 quater | DUPLIQUER `[$]motif[,P|D]`, sous `LSE F4` seulement (nom, groupe suivi de `*`, `*`) : `$` de l'unité vers la bibliothèque, sinon l'inverse ; catalogue de l'unité ; F 29, F 03 à F 08 ; `-UNITE chemin` ; statut « retenu » |

| IMP-0259 | Bibliothécaire et fichiers, §7 quinquies | Fichier décodé : lignes suivies de X-OFF, NUL final, chaînes de 252 caractères ; MITRA 15 lignes coupées et tampon précédent en fin (PG66 bis), T1600 lignes entières et NUL au lieu du dernier X-OFF (PG66) ; statut « retenu » |
| IMP-0260 | Bibliothécaire et fichiers, §7 quinquies | DECODER ω{,n1{,n2}} : fichier temporaire créé ou remplacé, F 06 (secret), EX 52 ; module `codage.c` ; statut « retenu » |
| IMP-0261 | Bibliothécaire et fichiers, §7 quinquies | ENCODER ω{,n1{,n2}} : temporaire puis permanent, X-OFF ou CR, MITRA 15 remplace sans n1, sinon ajout ; première erreur arrête ; F 05, F 06, F 101 ; statut « retenu » |
| IMP-0262 | Bibliothécaire et fichiers, §7 sexies | Ruban = fichier texte TXT `RUBAN` (bibliothèque) ou `-RUBAN chemin` ; `ESPACE_RUBAN` ; module `ruban.c` ; statut « retenu » |
| IMP-0263 | Bibliothécaire et fichiers, §7 sexies | Lecture du ruban : RUBAN sous MITRA 15 (SILENCE), X-ON sous T1600 (imprimé) ; jusqu'à CR ou X-OFF, LF et NUL ignorés ; arrêt à la fin (retour au début), à ESC perforé ou frappé ; statut « retenu » |
| IMP-0264 | Bibliothécaire et fichiers, §7 sexies | PERFORER A PARTIR DE n1{,n2} : listage et ajout au ruban, lignes terminées par CR ; ESC ; F 06 ; statut « retenu » |
| IMP-0265 | Bibliothécaire et fichiers, §7 sexies | SILENCE : écho coupé jusqu'à ESC ; statut « retenu » |

| IMP-0266 | Moniteur et console, §9 | Bannières : MITRA 15 `BONJOUR` puis `LSE-MI15  CONSOLE NO.01  date heure`, `AU REVOIR  heure` ; T1600 `BONJOUR  date heure` ; `LSE F4` : `CONSOLE NO. 01.`, `LSE F4`, `RUB-OUT REMPLACE CTRL X-OFF`, `VOUS ETES SOUS ID c` et cinq lignes blanches, après BONJOUR et AU REVOIR ; statut « retenu » (révisé le 2026-11-01) |
| IMP-0267 | Moniteur et console, §7 bis | Éditeur de ligne `[n]X[p]/groupe/remplacement` (`%` possible), ligne refusée ou listée la plus récente, deux profils ; LAFOND83, MEMENTO82 ; statut « retenu » |

| IMP-0268 | Bibliothécaire et fichiers, §7 quater | Option `-ID n` : identification initiale de chaque session (0 par défaut, ignorée si non définie) ; `VOUS ETES SOUS ID c` (compte) sous `LSE F4` ; question de l'espace temporaire (x) sous MITRA 15 seul ; statut « retenu » |

| IMP-0269 | Gestionnaire des tâches et reprise, §2 | Tâche unique (console 1) à trois états (absente, active, en attente d'entrée) ; SUSPENDRE/SWAP_OUT à chaque demande d'entrée, REACTIVER/SWAP_IN à la réception ; SWAP sans copie (IMP-0130) ; module `taches.c`, segment `"fichiers"` ; statut « retenu » |

| IMP-0270 | Gestionnaire d'objets, §11 | `objets_terminer` : tous les blocs-maîtres et objets hors-norme rendus un par un (`DisposeHandle`) en fin de session et sur la sortie en échec de `main` ; Toolbox Reference vol. 1, p. 12-14, 12-18 ; statut « retenu » |
| IMP-0271 | Gestionnaire d'objets, §11 | `attrNoSpec` sur tout bloc du gestionnaire d'objets : jamais dans les banques $00, $01, $E0, $E1 ; statut « retenu » |
| IMP-0273 | Moniteur et console, §9 bis | `SystemError` remplacée dans `console_iigs.c` (sans contrôle de pile, comme `console_fermer`) : une erreur d'exécution d'ORCA/C rend la console avant le message et l'arrêt ; sonde `SONDEE` ; audit du code, AC-12 ; statut « retenu » |
| IMP-0274 | Bibliothécaire et fichiers, §7 sexies | Perforateur commandé par un programme : `EQC(18)` et `EQC(20)`, non perforés ; tout ce que le programme imprime est ajouté au ruban, tel quel ; messages et frappe non perforés ; arrêt à chaque retour au moniteur ; EX 52 ; statut « retenu » |
| IMP-0275 | Bibliothécaire et fichiers, §7 sexies | `EQC(17)` à `EQC(21)` ne vont pas à la console, sous les deux profils ; `EQC(17)` et `EQC(21)` sans effet sur le lecteur ; les autres codes de commande vont à la console ; statut « retenu » |
| IMP-0272 | Construction, §5 bis | `#pragma stacksize 16384` dans `src/plateforme/main.c` : pile du 65816 de 16 Kio (15 840 octets sous `main` sur l'Apple IIGS, contre 3 555 avec la pile du shell) ; mesures sur l'Apple IIGS du 2026-10-29 (audit du code, AC-01) ; statut « retenu » |
| IMP-0276 | Compilateur, §8 bis | Profil `LSE F4` (`PROFIL_T1600_F4`, valeur 2), troisième profil et profil par défaut ; `-F4`, `-T1600`, `-MITRA15` ; hérite du T1600 version 4 (`PROFIL_EST_T1600`) ; sorties de Victor Duruy d'abord ; identification 1 par défaut ; statut « retenu » |
| IMP-0277 | RCI-M (spécification physique), §4.8.2 | `FAIRE n1 [POUR V←v1 [PAS p]] TANT QUE c`, `POUR` facultatif, profil `LSE F4` (`SINUS` 105 ; MEMENTO82, LAFOND83, NOYELLE79, d'après Éric) ; ouverture sans variable 0x12 (`0xD2`), sans charge utile, `[n1] D2 [c] D0` ; test à l'entrée ; `JUSQUA` et `PAS` sans `POUR` rejetés ; statut « retenu » |
| IMP-0278 | Bibliothécaire et fichiers, §7 quater | `$` devant un nom de fichier, sous `LSE F4` : fichier sur l'unité amovible ; ne change que le lieu (mêmes droits, erreurs et protections, catalogue de l'unité) ; APPELER, RANGER, MODIFIER, SUPPRIMER, CATALOGUER, DECODER, ENCODER, TABLE, UTILISATION ; pas dans les instructions ; F 29 sans unité ; statut « retenu » |
| IMP-0279 | Moniteur et console, §9 | Démarrage de `LSE F4` : `LSE F4`, `DATE HEURES MINUTES :`, `EN LIGNE....` ; RETURN seul prend l'horloge de l'Apple IIGS ; horloge du système, heure sans retour à zéro ; `00/00/00` : champ date vide ; `00000000` non reproduit ; statut « retenu » |
| IMP-0280 | Interpréteur, §3.1 bis | Réels de 23 bits significatifs, la demie en s'éloignant de zéro, sous `LSE F4` et `-T1600` : constantes, `LIRE`, `CNB`, opérations, puissances, fonctions, pas de `FAIRE` ; entiers exacts jusqu'à 2²³ ; `ALE` tronqué dans ]0,1[ ; impression inchangée ; MITRA 15 inchangé ; statut « retenu » |
| IMP-0281 | Moniteur et console, §9 bis | À l'écran, `_` s'affiche `←` et `^` s'affiche `↑`, sous les trois profils, sans option : caractères MouseText `$48` et `$4B`, écrits `$C8` et `$CB` (en mode normal, `$C0` à `$DF` s'affichent toujours en MouseText) ; pilote `.CONSOLE` seul ; noyau, fichiers, ruban et repli stdio inchangés ; touches flèches du clavier non traduites ; statut « retenu », vérifié sur l'Apple IIGS |
| IMP-0282 | Bibliothécaire et fichiers, §7 sexies | Console à 7 bits : un octet de `$80` à `$FF` imprimé par un programme va à la console réduit à ses 7 bits (`EQC(200)` → `H`), sauf s'il devient un ordre `$11` à `$15` (IMP-0275) ; ruban, chaînes et fichiers gardent l'octet entier ; perforateur commandé par `EQC(18)` et `EQC(20)` seuls ; trois profils ; statut « retenu », vérifié sur l'Apple IIGS |

IMP-0134 (Architecture physique §7) révisée le 2026-10-29 : aucun budget ni interrogation au démarrage, l'échec d'allocation fait foi, `RealFreeMem` indicatif, mesure sur cible par la sonde `SONDE` ; mesurée le 2026-10-29 (`audits/Mesures_memoire_IIGS_2026-10-29.md`, `RealFreeMem` 3 589 887 octets) ; statut « retenu ».

Révisions du 2026-10-30 (audit du code du 2026-10-29, choix d'Éric après essais sur LSE 8086) :
- IMP-0232 et IMP-0239 (Moniteur et console) : le retour sur la ligne `FAIRE` au début de chaque itération compte comme une arrivée sur cette ligne (pas à pas, ESC, ligne d'arrêt) ; AC-03.
- IMP-0145 (État d'exécution §4.3) : pile d'évaluation plafonnée à 9 362 éléments, EX 03 au-delà ; AC-06.
- IMP-0179 (État d'exécution §3 bis) : réserve des cases plafonnée à 13 107 cases, EX 03 au-delà ; AC-07.
- IMP-0222 (Interpréteur) et IMP-0233 (Moniteur et console) : une ligne de machine de bureau part d'un état de lecture vierge, celui du programme étant rétabli à sa sortie ; LIRE y reste permis (MEMENTO82) ; AC-13, AC-14.
- IMP-0183 (RCI-M) : une ligne restituée de plus de 79 caractères est signalée par ENCODER (C 63), non par DECODER ; AC-10, non corrigé dans le code.
- IMP-0240 (fin de l'entrée sous le repli stdio : AU REVOIR implicite, AC-26) ; IMP-0250 (F 102 au lancement : message formaté, puis F 102 pour toute opération de fichiers, AC-28) ; IMP-0253 (quota contrôlé seulement si l'écriture agrandit, AP-15 ; `-TEMP` validé, AC-27) ; IMP-0268 (`-ID` validé, option invalide refusée au lancement, AC-27).
- IMP-0272 (Construction §5 bis), complétée : contrôle de non-régression par la suite `plateforme_pile` (profondeur au plus 14 816 octets) ; AC-11.
- IMP-0221 (Interpréteur) et IMP-0146 (État d'exécution §5.1) : la description en cours d'un AFFICHER est un état de l'interpréteur, rangé dans le cadre de l'appelant pendant un appel et conservé à travers les suspensions ; AC-09.
- IMP-0247 (Bibliothécaire et fichiers §4), complétée : une étendue libre plus longue que le besoin est découpée quand son secteur de répertoire porte une entrée inutilisée, la fin étant décrite par la même écriture ; ordre des écritures de GARER β ; AC-18, AC-19.
- IMP-0104 (RCI-M), complétée : table des symboles plafonnée à 6 553 entrées et table des constantes à 21 845, chacune ne dépassant pas 64 Kio en modèle small ; C 108 au-delà, F 101 pour un fichier-programme qui les dépasse ; IMP-0191 (point 6) mise en accord ; AC-32.
- IMP-0246 (Bibliothécaire et fichiers §3), complétée : vérification complète avant de vider le programme (noms, nature, origine, longueurs, doublons par une table de hachage temporaire, identifiants et index de chaque séquence, plafonds) ; reconstruction sans recherche ; AC-17, AP-12. IMP-0167 (Compilateur), complétée : tables agrandies par tranches de 32 entrées.
- IMP-0249 (Bibliothécaire et fichiers §6), complétée : la stdio d'ORCA/C ne distingue pas un fichier verrouillé d'un fichier absent (`ENOENT`), ni un volume plein d'une autre erreur d'écriture (`EIO`) ; après l'échec d'une ouverture ou d'une suppression, `GetFileInfoGS` fait foi (`$46`, `$44` : inexistant ; sinon erreur d'entrée-sortie) ; création seulement d'un fichier absent ; F 27 pour un fichier présent mais refusé ; tout échec d'écriture reste « support plein », limite consignée ; AC-29.
- IMP-0243 (Gestionnaire d'objets §11), étendue : accès sans verrouillage partout où la fenêtre se réduit à une copie ou à un calcul d'en-tête ; `objets_ecrire` ; verrou maintenu dans `objets_resoudre` ; AP-01, AP-02 (b), AP-08, AP-09 (verrouillage), AP-13, AC-33. IMP-0159 (Gestionnaire d'objets §6), révisée : parcours des blocs sans verrouillage.
- IMP-0140 (Construction §2), complétée : toute règle de compilation dépend de tous les en-têtes du dépôt (`$(ENTETES)`, par `$(wildcard …)`) et du témoin `build/profil.temoin`, réécrit seulement quand `PROFIL` change ; plus de `make clean` après un changement de profil ; AC-34.
- IMP-0216 (Interpréteur §3.1), révisée : `+ − × ÷` hors du chemin exact calculés en format étendu et arrondis directement en `float`, même résultat que le calcul en `double` ; dépassement décidé en `double` quand le `float` obtenu vaut ±`FLT_MAX` ou l'infini ; AP-04. IMP-0182 et IMP-0167 (Compilateur), complétées : reconnaissance des mots et constantes entières sans calcul flottant ; AP-05, AP-11.
- IMP-0244 (Interpréteur), complétée : indice entier exact pris sans arrondi, affectation et pas de FAIRE par adresse, constantes immédiates converties une fois ; AP-02 (a), AP-07, AP-10. IMP-0201 (Décompilateur), complétée : table des puissances de dix ; AP-03. IMP-0248 (Bibliothécaire et fichiers §5), complétée : catalogue lu par blocs de 16 entrées, une passe ; AP-14.
- IMP-0261 (Bibliothécaire et fichiers §7 quinquies), révisée : ENCODER ne met le programme à l'état « aucun » que s'il change (remplacement, ou ligne ajoutée ou remplacée) ; §9 point 13. IMP-0249 (§6), complétée : `plateforme_espace_parcourir`, liste triée des noms pour TABLE, UTILISATION, DUPLIQUER `*` ; AP-06. IMP-0246 (§3), complétée : symboles et constantes morts conservés ; §9 point 11. IMP-0150 (État d'exécution), complétée : tableaux temporaires d'un appel statiques ; AP-09. IMP-0216 (Interpréteur), complétée : pas de FAIRE en format étendu, EX 44 au-delà du plus grand `float` ; AC-35.
- IMP-0247 (Bibliothécaire et fichiers §4), complétée : GARER écrit le contenu par blocs de 16 secteurs au plus ; secteurs au-delà du contenu complétés de zéros ; AP-17, AC-36.

Révisions du 2026-10-31 (reconstitution de PERFO et CHARG, choix d'Éric) :
- IMP-0209 (Données §2), révisée : 65 535 caractères au plus sous les deux profils ; `LSE F4` à Victor Duruy range une chaîne de 274 caractères (FICH2, enr. 8).
- IMP-0221 (Interpréteur §4 bis), complétée : la sortie passe par `ruban_sortie` (IMP-0274, IMP-0275).
- IMP-0222 (Interpréteur §4 bis), complétée : une réponse vide à une variable-chaîne donne la chaîne vide, et le `LIRE` suivant demande une nouvelle ligne (LSE 8086, 2026-10-31).

Révisions du 2026-11-01 (profil `LSE F4`, choix d'Éric) :
- IMP-0180 (Compilateur §8 bis), révisée : trois profils, `LSE F4` par défaut, paramètres `-F4`, `-T1600`, `-MITRA15`.
- IMP-0268 (Bibliothécaire et fichiers §7 quater), complétée : identification 1 par défaut sous `LSE F4`.
- IMP-0180, IMP-0189, IMP-0190 (Compilateur §8 bis), révisées : le T1600 version 4 s'en tient à INRDP75, `TEM()` et `.n.` sont propres à `LSE F4` ; IMP-0190 étendue aux libellés de format (`MULT`, ligne 50).
- IMP-0183 (RCI-M §4.12), amendée : un libellé de format est restitué comme une constante-chaîne.
- IMP-0189 (Compilateur §8 bis), révisée : `TEM` restitué `TEM()` sous `LSE F4` (`SINUS` 104, 105), et `DAT` aligné sur `TEM`.
- IMP-0238, IMP-0240 (Moniteur et console), complétées : RUB-OUT (DELETE) vaut X-OFF sous `LSE F4`.

Révision du 2026-11-04 :
- IMP-0235 (Moniteur et console §8), complétée : `BO` en session répond `????` sans complétion, sous tous les profils (SPHIN, PA1).
- IMP-0236 (Moniteur et console §9), complétée : console unique confirmée (point 10), attestations de `PRENDRE ETAT CONSOLE`, évolution envisagée à plusieurs consoles par le port série.
- IMP-0276 (Compilateur §8 bis), précisée : rubans et fichiers décodés d'un profil à l'autre.
- IMP-0151 (État d'exécution §6.1), complétée : contexte de boucle sans variable (IMP-0277).
- IMP-0258, IMP-0268 (Bibliothécaire et fichiers §7 quater), révisées : `DUPLIQUER` et `VOUS ETES SOUS ID` propres à `LSE F4`.
- IMP-0223, IMP-0224 (Interpréteur §4 ter), complétées : `horloge_fixer`, heure au-delà de 24 (IMP-0279).
- IMP-0228, IMP-0266 (Moniteur et console), révisées : bannière de `LSE F4` après `BONJOUR` et après `AU REVOIR` ; IMP-0268 complétée : la ligne `VOUS ETES SOUS ID` annonce l'identification initiale.

Révision du 2026-11-05 (point 5, choix d'Éric ; `audits/Analyse_reels_T1600_2026-11-05.md`) :
- IMP-0216 (Interpréteur §3.1), complétée : choix du profil MITRA 15 ; sous les profils T1600, arrondi à 23 bits avant le rangement (IMP-0280).
- IMP-0244 (Interpréteur), complétée : sous les profils T1600, chemin entier limité aux résultats inférieurs à 2²³.
- IMP-0225 (Interpréteur §4 ter), complétée : sous les profils T1600, dernier bit de x′ tronqué quand x′ ≥ 2²³.
- IMP-0222 (Interpréteur §4 bis) et IMP-0212 (Données, `CNB`), complétées : nombre lu arrondi à 23 bits, plus grand nombre (2²³ − 1) × 2¹⁰⁵.
- IMP-0182 (Compilateur §5 bis), complétée : constante numérique arrondie à 23 bits avant le choix du sous-format.

Prochain identifiant libre : **IMP-0283**.
Il n'existe pas de chapitre distinct « Compilateur — clause LOCAL » : la déclaration de procédure et la clause `LOCAL` sont traitées par le chapitre Compilateur, §13.

Les plages ci-dessus ont été reconstituées à partir des renvois croisés des chapitres ; vérifier contre les fichiers avant de s'y fier (`grep -rhoE "IMP-[0-9]{4}" 05-IMPLEMENTATION/tome-v | sort -u | tail`).
