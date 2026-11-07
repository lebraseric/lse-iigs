# LSE-IIGS

Reconstruction historique du **Langage Symbolique d'Enseignement (L.S.E.)** — réalisation physique sur Apple IIGS (65816, 4 Mio RAM, 32 Mio disque) avec ORCA/C 2.2.1, en application texte lancée depuis le shell ORCA.

LSE-IIGS reproduit trois systèmes LSE de l'époque, choisis au lancement :

| Profil | Système reproduit | Option |
|---|---|---|
| **LSE F4** | LSE du T1600 du lycée Victor Duruy (Paris), début des années 1980 | `-F4` (par défaut) |
| **T1600 version 4** | LSE du T1600 de Télémécanique, version 4 (1974-1975) | `-T1600` |
| **MITRA 15 version 8** | LSE du MITRA 15, version 8 | `-MITRA15` |

## Organisation

```text
01-NORMATIF/         Référentiel normatif (EXI), Tomes I, II, III, IV
02-HISTORIQUE/       Transcriptions de Victor Duruy, relevés, reconstitution de PERFO et CHARG, grammaire reconstruite
03-APPLE-IIGS/       Documentation de plateforme (non publiée) et outils (AppleCommander, téléchargé)
04-ORCA-C/           Documentation ORCA/C 2.2.1 (non publiée)
05-IMPLEMENTATION/
    contraintes/         Conditionnement (instructions de mission)
    decisions/           Registre des décisions IMP-*
    tome-v/              Tome V : spécification détaillée d'implémentation (15 chapitres)
    references/          Fiche de référence de codage RCI-M
    matrice-tracabilite/ EXI → VM → IV → IMP → code → test
    audits/              Audits, inventaires, mesures et essais sur l'Apple IIGS
    exemples/            Programmes LSE d'exemple
06-MANUEL/           Manuel de l'utilisateur (version 1.0)
src/
    noyau/               Machine virtuelle (indépendante de la plateforme)
    plateforme/          Couche d'adaptation Apple IIGS / GS/OS / ORCA
    interfaces/          Contrats noyau ↔ plateforme
tests/                   Suites de tests (composants, historiques, plateforme) et bouchons
build/                   Makefile ORCA/C, produits de construction, images disque
scripts/                 Images disque, rubans, catalogue d'erreurs, extraction du corpus
```

## Sources

Les manuels historiques (INRDP 1975, LSE 15 de 1978, etc.) et la documentation Apple et ORCA/C appartiennent à des tiers et ne sont pas publiés ici. `SOURCES.md` en donne les références complètes et indique où se les procurer. Les spécifications les citent par sigle.

## Construction

La construction emploie ORCA/C 2.2.1 sous Golden Gate (`iix`) ; AppleCommander (Java 11 ou plus) fabrique les images ProDOS. Depuis `build/` :

- `make tests` (ou `make tests PROFIL=cible`) : construit et exécute toutes les suites ;
- `make PROFIL=cible` : construit `LSE` et l'image de distribution `build/LSE.po`, la seule à employer sur un Apple IIGS.

Détails : `build/README.md` et `05-IMPLEMENTATION/tome-v/Construction_LSE-IIGS.md`. Mode d'emploi du système LSE : `06-MANUEL/Manuel_utilisateur_LSE-IIGS.md`.

## Statut

**Version 1.0 du logiciel** (novembre 2026).

- Tous les chapitres du Tome V sont rédigés ; les décisions d'implémentation vont d'IMP-0101 à IMP-0282 (`05-IMPLEMENTATION/decisions/REGISTRE.md`).
- Le système est complet : compilation ligne par ligne, décompilation, interpréteur, dialogue conversationnel, fichiers-programmes et fichiers-données, identifications, ENCODER/DECODER, ruban (PERFORER, RUBAN), procédures et fonctions, éditeur de ligne.
- 30 suites de tests, 1 883 contrôles, réussies dans les profils de construction `dev` et `cible` sous Golden Gate ; aller-retour compilation/décompilation à l'octet près sur le corpus d'INRDP75 et de LSE15.
- Programmes et exécutions imprimés au lycée Victor Duruy (1978-1983) transcrits et rejoués à l'identique, à quelques écarts près, consignés.
- Un audit de code (2026-10-29, 53 points) a été entièrement traité.
- Essayé sur Apple IIGS ROM 01, 4 Mo, MicroDrive Turbo, TransWarp GS, GS/OS 6.0.4, ORCA/Shell 2.0.6.

Le journal détaillé du chantier figure dans `CLAUDE.md` (« État du chantier »).

## Licences

© 2026 Éric Le Bras.

- **Code** : licence [CeCILL v2.1](LICENSE), compatible avec la GNU GPL. Elle couvre `src/`, `tests/`, `scripts/`, `build/Makefile` et les scripts Python de `02-HISTORIQUE/`.
- **Documentation** : licence [Creative Commons Attribution - Partage dans les mêmes conditions 4.0 International (CC BY-SA 4.0)](LICENSE-DOC). Elle couvre `01-NORMATIF/`, `05-IMPLEMENTATION/`, `06-MANUEL/`, les transcriptions, relevés et programmes LSE de `02-HISTORIQUE/`, la grammaire reconstruite `02-HISTORIQUE/LSE15Mitra.g4`, et les fichiers Markdown de la racine.

Les citations de sources historiques et les lignes de programmes extraites d'INRDP75 et de LSE15 (`tests/historiques/corpus/`) restent la propriété de leurs auteurs ; elles sont reproduites à titre de courte citation (voir `SOURCES.md`).
