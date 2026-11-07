# Mesures de la mémoire disponible sur l'Apple IIGS — 2026-10-29

**Objet :** mémoire disponible sur l'Apple IIGS d'Éric après chargement de GS/OS et du shell ORCA, pour clore Architecture physique IMP-0134 (statut « incomplet » depuis sa rédaction).

**Instrument :** `tests/plateforme/sonde_console.c` (programme `SONDE`), bloc `SONDE MEMORY MANAGER`, lancé depuis le shell ORCA par `SONDE >mem.out` ; sortie relevée sur l'image `build/LSE.po` par AppleCommander (`MEM.OUT`, type TXT). Appels du Memory Manager : `TotalMem` ($1D02), `FreeMem` ($1B02), `MaxBlock` ($1C02) (Apple IIGS Toolbox Reference, vol. 1, p. 12-46, 12-26, 12-34) ; `RealFreeMem` ($2F02) (vol. 3, p. 36-10).

**Résultats :**

| Mesure | Octets | Mio | Signification |
|---|---|---|---|
| `userid()` | `$1103` | — | type 1 (application), auxID 1, mainID $03 |
| `TotalMem` | 4 456 448 | 4,250 | toute la mémoire, y compris les 256 Kio principaux |
| `FreeMem` | 3 265 881 | 3,115 | mémoire libre, hors blocs purgeables |
| `RealFreeMem` | 3 589 887 | 3,424 | mémoire libre plus mémoire récupérable par purge |
| `MaxBlock` | 2 948 817 | 2,812 | plus grand bloc libre, sans compactage ni purge |

**Constats :**

1. `TotalMem` vaut exactement $440000, soit 4 Mio d'extension plus les 256 Kio principaux (banques $00, $01, $E0, $E1) : la machine est bien une configuration à 4 Mio (conditionnement, plateforme cible).
2. `userid()` = `$1103` confirme la composition décrite par GS-20 *Toolbox C* : identifiant maître combiné avec `0x0100` (auxID = 1). Le découpage suit la figure 12-7 du vol. 1 corrigée par le vol. 3 (p. 36-2) : `mainID` occupe les bits 0 à 7, ici $03.
3. Écart `RealFreeMem` − `FreeMem` = 324 006 octets (environ 316 Kio) : blocs purgeables présents au lancement, vraisemblablement conservés par le shell ORCA ou le chargeur (niveau de purge 3, vol. 1, p. 12-10). Le Memory Manager les libère de lui-même avant de refuser une allocation (vol. 3, p. 36-3).
4. `MaxBlock` est inférieur de 0,3 Mio à `FreeMem` : la mémoire libre est fragmentée, ce qui est sans effet pour le gestionnaire d'objets, dont les blocs-maîtres font 4 Kio (Gestionnaire d'objets §5).

**Limites de la mesure :**

- La mesure est prise sous `SONDE` (12 Kio), non sous `LSE` (exécutable de 203 Kio en profil `cible`, segments statiques tous résidents, IMP-0135). La mémoire restant à la disposition des objets du LSE est donc inférieure d'environ la taille de son code et de ses données statiques : de l'ordre de 3,2 Mio en comptant la mémoire purgeable.
- Les valeurs incluent la mémoire spéciale (banques $00, $01 et parties gérées de $E0, $E1), que les blocs du gestionnaire d'objets n'emploient plus (IMP-0271). La part qu'elle représente n'est pas isolée par ces appels.
- Cette première mesure ne vérifie pas `attrNoSpec` ; la seconde (ci-dessous) le fait.
- La version de GS/OS et la liste des pilotes chargés ne sont pas relevées par la sonde.

**Incident (2026-10-29) :** deux versions de l'essai d'allocation de `SONDE`, la première allant jusqu'au refus du Memory Manager, la seconde bornée à 1 Mio, ont planté le système dans le moniteur de l'Apple IIGS dès le premier appel de `plateforme_bloc_allouer`. La liste des handles relevée par Éric ne montre aucun bloc de 4096 octets aux attributs `$0018` : le plantage précède l'allocation. Cause : l'image avait été construite en profil `dev` ; `memoire_iigs.c` inclut `profil.h`, qui impose alors `#pragma debug 43`, dont le bit 1 produit du code de débogage source-level qui plante sans débogueur installé (Construction, IMP-0143). La première version de `SONDE`, sans `memoire_iigs.c`, n'en contenait pas. Règle : une image destinée à l'Apple IIGS se construit en `PROFIL=cible` (`make clean`, puis `make PROFIL=cible`). L'essai borné (au plus 1 Mio, arrêt si `FreeMem` passe sous 1 Mio, lecture du mot d'attributs à l'octet 4 de l'enregistrement du pointeur maître, vol. 1, p. 12-30) est conservé par prudence.

**Seconde mesure (2026-10-29, après redémarrage, image construite en `PROFIL=cible`) :** `SONDE >mem.out`, sans plantage.

| Mesure | Valeur |
|---|---|
| `userid()` | `$1103` |
| `TotalMem` | 4 456 448 octets |
| `FreeMem` | 3 325 213 octets |
| `RealFreeMem` | 3 649 219 octets |
| `MaxBlock` | 3 014 609 octets |
| Blocs de 4096 octets alloués par `plateforme_bloc_allouer` | 256 (1 048 576 octets, plafond de l'essai) |
| Mot d'attributs du premier bloc | `$0018` : NoSpec, NoCross, purge 0, non verrouillé |
| Blocs sans `attrNoSpec` | 0 |
| Banques occupées | $2F à $3F |
| Blocs en mémoire spéciale ($00, $01, $E0, $E1) | 0 |
| `FreeMem` après allocation | 2 276 637 octets |
| `FreeMem` après restitution | 3 325 213 octets |

Constats :

5. Le Memory Manager a enregistré pour chaque bloc exactement les attributs demandés par `plateforme_bloc_allouer` (`attrNoPurge | attrNoSpec | attrNoCross` = `$0018`) : IMP-0271 est vérifiée sur la machine. Les blocs sont placés en haut de la mémoire (banques $2F à $3F), comme le prévoit le vol. 1 pour les blocs déplaçables (p. 12-7).
6. L'allocation de 1 Mio fait baisser `FreeMem` de 1 048 576 octets exactement, et la restitution le ramène à sa valeur initiale : aucune mémoire n'est perdue par l'aller-retour `NewHandle`/`DisposeHandle`, ce que suppose IMP-0270.
7. `FreeMem` et `RealFreeMem` sont supérieurs d'environ 58 Kio à ceux de la première mesure, prise sans redémarrage préalable ; l'écart purgeable (324 006 octets) est identique.
8. L'essai étant borné à 1 Mio, il ne montre pas le comportement à l'épuisement ; la garantie que la mémoire spéciale n'est jamais employée repose sur la définition de `attrNoSpec` (vol. 1, p. 12-13), dont la présence sur chaque bloc est constatée.

**Conséquence :** IMP-0134 passe au statut « retenu ». Avec environ 3,2 Mio disponibles pour les objets, aucune limite de taille de programme ou de données n'est à introduire pour la v1 ; le manque de mémoire reste signalé par l'échec d'allocation (`ERR_E_MEMOIRE_SATUREE`).
