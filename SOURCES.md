# Sources

Le projet s'appuie sur des documents historiques et techniques dont les droits appartiennent à des tiers. Ils ne sont pas publiés avec ce dépôt. Les spécifications (`01-NORMATIF/`, `05-IMPLEMENTATION/`) les citent par sigle (`INRDP75`, `LSE15`…) et par leur chemin dans le dépôt : un chemin vers un document de cette liste désigne un fichier absent d'une copie publique.

Les seules reproductions publiées sont de courtes citations, sourcées, faites pour l'analyse.

## Documents historiques sur le LSE

| Sigle | Référence | Chemin | Disponibilité |
|---|---|---|---|
| INRDP75 | INRDP, Section Informatique et Enseignement (Christian Lafond), *Langage Symbolique d'Enseignement (L.S.E.) — Manuel d'utilisation*, 2ᵉ édition, bulletin de liaison « L'informatique et l'enseignement secondaire », numéro spécial, janvier 1975 | `02-HISTORIQUE/LSE-Manuel-d-utilisation-INRDP-1975.md` (OCR corrigé), `02-HISTORIQUE/images/` (figures) | Droits : INRDP, aujourd'hui Réseau Canopé, et ses auteurs. Publication de la transcription soumise à autorisation |
| LSE15 | École d'Application des Transmissions, *Langage Symbolique d'Enseignement — LSE 15, manuel d'utilisation* (MITRA 15), 1978, « diffusion interne à des fins pédagogiques seulement » | `02-HISTORIQUE/LSE15-Manuel-Mitra15-EAT-1978.md` (transcription) | Droits : ministère des Armées. Publication de la transcription soumise à autorisation |
| NOYELLE89 | Yves Noyelle, « La saga du LSE et de sa famille (LSD/LSG/LST) », *Revue de l'EPI* n° 54, 1989, p. 216-228 | `02-HISTORIQUE/La-saga-du-LSE-Noyelle.md` (copie) | En ligne : <https://www.epi.asso.fr/revue/54/b54p216.htm> |
| LAFOND83 | Christian Lafond, Pierre Muller, *Lire LSE, le langage français de programmation — manuel de l'utilisateur*, CEDIC / Fernand Nathan, 1983 | `02-HISTORIQUE/LAFOND83.pdf` (extraits numérisés) | Ouvrage épuisé. Relevé publié : `02-HISTORIQUE/LAFOND83-releve.md` |
| MEMENTO82 | *MEMENTO LSE*, GG/NLG/Études Logiciel, février 1982, réf. R-1017 F (Micral 80-22G, R2E) | `02-HISTORIQUE/MEMENTO82.pdf` (extraits numérisés), `02-HISTORIQUE/MEMENTO82-transcription.md` | Document d'entreprise, non publié |
| EPI82 | Dossier de PRO et PRON2, avril 1982, dans *L'informatique à l'école : concours de programmes AFCET-IFIP 1981*, numéro spécial du bulletin de l'EPI, juin 1982, p. 80-82 | `02-HISTORIQUE/Programmes LSE T-1600 Victor Duruy 1978-1983/4EME - PRO/Exécution PRO-PRON2.pdf` | Droits : EPI |

## Sorties du T1600 du lycée Victor Duruy (1978-1983)

Les numérisations des listages et exécutions imprimés par Éric Le Bras, alors élève, se trouvent dans `02-HISTORIQUE/Programmes LSE T-1600 Victor Duruy 1978-1983/`. Elles ne sont pas publiées pour l'instant. Leurs transcriptions, le relevé et les outils de vérification sont publiés :

- `02-HISTORIQUE/Transcriptions-T1600-Victor-Duruy/` (programmes `.lse`, exécutions, `essais/`) ;
- `02-HISTORIQUE/Releve-sorties-T1600-Victor-Duruy.md` ;
- `02-HISTORIQUE/Reconstitution-PERFO-CHARG/` (texte restitué de PERFO et CHARG, sorties transcrites).

`essais/encre.py` lit les numérisations : il ne fonctionne qu'avec le corpus privé.

## Documentation Apple IIGS

| Référence | Chemin | Disponibilité |
|---|---|---|
| Apple Computer, *Apple IIGS Toolbox Reference*, volume 1 (chap. 12, Memory Manager) | `03-APPLE-IIGS/manuels/` | © Apple |
| Apple Computer, *Apple IIGS Toolbox Reference*, volume 3 (chap. 36) | `03-APPLE-IIGS/manuels/` | © Apple |
| Apple Computer, *Apple IIGS GS/OS Reference* (chap. 9, pilote `.CONSOLE`) | `03-APPLE-IIGS/systeme/` | © Apple |

## ORCA/C

| Référence | Chemin | Disponibilité |
|---|---|---|
| Mike Westerfield, Stephen Heumann, *ORCA/C 2.2 — A C Compiler and Development System for the Apple IIGS*, Byte Works | `04-ORCA-C/manuel/ORCA_C_2.2_Manual.docx` | © Byte Works ; fourni avec ORCA/C |
| *ORCA/C 2.2.1 release notes* (`cc.notes.txt`) | `04-ORCA-C/notes/cc.notes.txt` | Fourni avec ORCA/C 2.2.1 |
| Mike Westerfield, *Learn to Program in C* ; *Toolbox Programming in C*, Byte Works | `04-ORCA-C/manuel/GS-19 LTP C.txt`, `GS-20 Toolbox C.txt` | © Byte Works |

## Outils

- **ORCA/C 2.2.1** (Byte Works) : compilateur, bibliothèques et shell ORCA.
- **Golden Gate** (Kelvin Sherlock) : exécution des outils ORCA sur l'hôte (`iix`).
- **AppleCommander** 1.9.0 (GPL) : téléchargé au besoin par `scripts/creer_image.sh` dans `03-APPLE-IIGS/outils/`.

## Corpus privé

Pour qui dispose du dépôt privé : il est superposé à l'arbre de travail (répertoire git `.corpus.git`) et se manipule par `scripts/corpus.sh`, qui prend les mêmes arguments que `git`. Un document versé doit être ajouté au dépôt privé (`scripts/corpus.sh add …`) **et** à la section « Corpus privé » de `.gitignore`.
