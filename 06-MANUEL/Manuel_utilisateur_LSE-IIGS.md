# LSE-IIGS — Manuel de l'utilisateur

**Version 1.0 du logiciel — manuel du 6 novembre 2026**

---

## Introduction

### Le LSE en quelques mots

Le **LSE**, *Langage Symbolique d'Enseignement*, est un langage de programmation français. Il a été défini en 1971 à l'École supérieure d'électricité pour l'expérience dite « des 58 lycées », qui équipait pour la première fois des lycées français d'ordinateurs. Dès la fin de 1972, élèves et professeurs l'ont utilisé sur des mini-ordinateurs partagés entre plusieurs consoles, le **T1600** de Télémécanique et le **MITRA 15**. Il a ensuite été repris sur les micro-ordinateurs des années 1980 et a fait l'objet d'une norme AFNOR expérimentale.

Le LSE se reconnaît à quelques traits :

- ses mots sont français : `AFFICHER`, `LIRE`, `SI … ALORS … SINON`, `FAIRE … POUR … JUSQUA`, `TERMINER` ;
- on dialogue avec l'ordinateur : chaque ligne tapée est vérifiée aussitôt, et une erreur est signalée tout de suite ;
- un programme est une suite de lignes numérotées, que l'on corrige ligne par ligne ;
- il sait calculer, mais aussi travailler sur des textes (les « chaînes de caractères ») ;
- il range programmes et données dans des fichiers, protégés par des numéros d'identification.

### LSE-IIGS

LSE-IIGS fait revivre sur l'Apple IIGS un système LSE tel qu'on l'utilisait dans les lycées à la fin des années 1970 et au début des années 1980 : même langage, mêmes commandes, mêmes messages, même façon de travailler. L'écran de l'Apple IIGS joue le rôle de la console (une télétype ou un terminal de l'époque) ; un fichier texte remplace le ruban perforé ; un répertoire du disque remplace le disque du mini-ordinateur.

LSE-IIGS sait se comporter comme trois systèmes LSE de l'époque. On appelle **profil** chacune de ces façons de se comporter, et on le choisit au lancement :

| Profil | Ce qu'il reproduit | Option de lancement |
|---|---|---|
| **LSE F4** | le LSE du T1600 du lycée Victor Duruy, à Paris, au début des années 1980 (sa bannière affichait `LSE F4`) | `-F4`, ou rien : c'est le profil par défaut |
| **T1600 version 4** | le LSE du T1600 de Télémécanique, version 4 (1974-1975) | `-T1600` |
| **MITRA 15 version 8** | le LSE du MITRA 15, version 8 | `-MITRA15` |

Les trois profils parlent le même langage, à quelques détails près, que ce manuel signale chaque fois. Le profil LSE F4 sert de référence tout au long du manuel ; les différences propres aux deux autres sont présentées dans des encadrés comme celui-ci :

> **Sous `-T1600`** … ce qui change avec le profil T1600 version 4.
>
> **Sous `-MITRA15`** … ce qui change avec le profil MITRA 15.

L'annexe D rassemble toutes ces différences.

### À qui s'adresse ce manuel

À tous : à l'élève ou à l'adulte qui découvre la programmation, au professeur qui prépare une séance, au connaisseur qui veut retrouver le LSE de sa jeunesse. Il suffit de savoir allumer l'Apple IIGS et taper une commande dans le shell ORCA. Aucune connaissance en programmation n'est nécessaire.

### Comment lire ce manuel

- Pour **découvrir**, lisez les chapitres 1 à 4, puis le chapitre 6 (la machine de bureau) et le début du chapitre 5.
- Pour **écrire des programmes**, lisez le chapitre 5 (le langage), puis les chapitres 7 et 8 (écrire, corriger, mettre au point).
- Pour **travailler avec des fichiers**, des identifications ou des rubans, lisez la partie IV.
- Pour **retrouver un détail**, voyez le chapitre 9 (toutes les commandes), les annexes A (messages d'erreur), B (grammaire), E (limites) et F (aide-mémoire).

### Conventions

- `CE TEXTE` est écrit tel qu'il apparaît à l'écran ou tel qu'il faut le taper.
- ‹RETURN›, ‹X-OFF›, ‹ESC›, ‹X-ON› désignent des touches (voir le chapitre 4). Une touche ne laisse aucune trace à l'écran.
- Dans les exemples d'écran, chaque saisie se termine par ‹X-OFF›, sauf indication contraire.
- Le LSE écrit l'affectation avec une flèche `←` et la puissance avec une flèche `↑`. C'est ainsi que l'écran de LSE-IIGS les affiche, et c'est ainsi que ce manuel les écrit. Au clavier, on tape `_` pour `←` et `^` pour `↑` (voir le paragraphe 4.5).
- Le LSE ne connaît que les majuscules sans accent : les programmes de ce manuel n'ont donc pas d'accents.
- Dans les tableaux de syntaxe, ce qui est entre crochets `[ ]` est facultatif, et `|` sépare des choix possibles.

---

## Table des matières

- [Introduction](#introduction)
  - [Le LSE en quelques mots](#le-lse-en-quelques-mots)
  - [LSE-IIGS](#lse-iigs)
  - [À qui s'adresse ce manuel](#à-qui-sadresse-ce-manuel)
  - [Comment lire ce manuel](#comment-lire-ce-manuel)
  - [Conventions](#conventions)

**Partie I — Prise en main**

- [1. Installation sur l'Apple IIGS](#1-installation-sur-lapple-iigs)
  - [1.1 Ce qu'il faut](#11-ce-quil-faut)
  - [1.2 Contenu de la disquette](#12-contenu-de-la-disquette)
  - [1.3 Transférer l'image](#13-transférer-limage)
  - [1.4 Installer LSE comme commande du shell](#14-installer-lse-comme-commande-du-shell)
  - [1.5 Ou lancer LSE sans l'installer](#15-ou-lancer-lse-sans-linstaller)
  - [1.6 La bibliothèque : là où LSE range vos fichiers](#16-la-bibliothèque--là-où-lse-range-vos-fichiers)
  - [1.7 Installer les programmes PERFO et CHARG](#17-installer-les-programmes-perfo-et-charg)
  - [1.8 Utiliser un émulateur](#18-utiliser-un-émulateur)
- [2. Lancement et paramètres](#2-lancement-et-paramètres)
  - [2.1 Lancer LSE](#21-lancer-lse)
  - [2.2 La question de la date](#22-la-question-de-la-date)
  - [2.3 Les options](#23-les-options)
  - [2.4 Exemples de lancement](#24-exemples-de-lancement)
  - [2.5 Quitter LSE](#25-quitter-lse)
  - [2.6 Si la bibliothèque est inaccessible](#26-si-la-bibliothèque-est-inaccessible)
- [3. Premiers pas](#3-premiers-pas)
  - [3.1 Ouvrir une session](#31-ouvrir-une-session)
  - [3.2 L'ordinateur comme calculatrice](#32-lordinateur-comme-calculatrice)
  - [3.3 Un premier programme](#33-un-premier-programme)
  - [3.4 Le faire exécuter](#34-le-faire-exécuter)
  - [3.5 Le modifier](#35-le-modifier)
  - [3.6 Le ranger et le retrouver](#36-le-ranger-et-le-retrouver)
  - [3.7 Fermer la session](#37-fermer-la-session)
  - [3.8 Pour aller plus loin](#38-pour-aller-plus-loin)
- [4. La console et le clavier](#4-la-console-et-le-clavier)
  - [4.1 Les trois états de la console](#41-les-trois-états-de-la-console)
  - [4.2 Terminer une saisie : X-OFF et RETURN](#42-terminer-une-saisie--x-off-et-return)
  - [4.3 Corriger en cours de frappe : la barre oblique inverse](#43-corriger-en-cours-de-frappe--la-barre-oblique-inverse)
  - [4.4 Interrompre : la touche ESC](#44-interrompre--la-touche-esc)
  - [4.5 Les flèches ← et ↑](#45-les-flèches--et-)
  - [4.6 Majuscules et minuscules](#46-majuscules-et-minuscules)
  - [4.7 Les messages de LSE](#47-les-messages-de-lse)
  - [4.8 Le bip](#48-le-bip)
  - [4.9 Récapitulatif des touches](#49-récapitulatif-des-touches)

**Partie II — Le langage**

- [5. Description du langage](#5-description-du-langage)
  - [5.1 Lignes et instructions](#51-lignes-et-instructions)
  - [5.2 Les mots du langage](#52-les-mots-du-langage)
  - [5.3 Les nombres](#53-les-nombres)
  - [5.4 Les chaînes de caractères](#54-les-chaînes-de-caractères)
  - [5.5 Les noms](#55-les-noms)
  - [5.6 Les variables](#56-les-variables)
  - [5.7 Les expressions numériques](#57-les-expressions-numériques)
  - [5.8 Les expressions-chaînes](#58-les-expressions-chaînes)
  - [5.9 Les conditions](#59-les-conditions)
  - [5.10 L'affectation](#510-laffectation)
  - [5.11 Le déroulement d'un programme](#511-le-déroulement-dun-programme)
  - [5.12 L'instruction SI](#512-linstruction-si)
  - [5.13 Le bloc DEBUT … FIN](#513-le-bloc-debut--fin)
  - [5.14 Les boucles FAIRE](#514-les-boucles-faire)
  - [5.15 Lire des données : LIRE](#515-lire-des-données--lire)
  - [5.16 Afficher des résultats : AFFICHER](#516-afficher-des-résultats--afficher)
  - [5.17 Les fonctions numériques](#517-les-fonctions-numériques)
  - [5.18 Les fonctions sur les chaînes](#518-les-fonctions-sur-les-chaînes)
  - [5.19 Date et heure](#519-date-et-heure)
  - [5.20 Les procédures](#520-les-procédures)
  - [5.21 Les fichiers dans un programme](#521-les-fichiers-dans-un-programme)
  - [5.22 Les commentaires](#522-les-commentaires)
  - [5.23 Instructions propres au T1600](#523-instructions-propres-au-t1600)
- [6. Le mode machine de bureau](#6-le-mode-machine-de-bureau)
  - [6.1 Principe](#61-principe)
  - [6.2 Le point d'interrogation](#62-le-point-dinterrogation)
  - [6.3 Ce qui est permis, ce qui ne l'est pas](#63-ce-qui-est-permis-ce-qui-ne-lest-pas)
  - [6.4 Examiner et modifier un programme arrêté](#64-examiner-et-modifier-un-programme-arrêté)
  - [6.5 Lire, garer, charger au bureau](#65-lire-garer-charger-au-bureau)
  - [6.6 Les erreurs au bureau](#66-les-erreurs-au-bureau)

**Partie III — Travailler avec ses programmes**

- [7. L'éditeur : écrire et corriger un programme](#7-léditeur--écrire-et-corriger-un-programme)
  - [7.1 Entrer une ligne](#71-entrer-une-ligne)
  - [7.2 Remplacer, insérer, supprimer](#72-remplacer-insérer-supprimer)
  - [7.3 Bien numéroter](#73-bien-numéroter)
  - [7.4 Voir le programme : LISTER et NUMERO](#74-voir-le-programme--lister-et-numero)
  - [7.5 Corriger une ligne sans la retaper : l'éditeur de ligne](#75-corriger-une-ligne-sans-la-retaper--léditeur-de-ligne)
  - [7.6 Ce que fait LSE d'une ligne tapée](#76-ce-que-fait-lse-dune-ligne-tapée)
  - [7.7 Écrire un programme avec l'éditeur d'ORCA](#77-écrire-un-programme-avec-léditeur-dorca)
  - [7.8 ABREGER et IN EXTENSO](#78-abreger-et-in-extenso)
- [8. Exécuter et mettre au point](#8-exécuter-et-mettre-au-point)
  - [8.1 Lancer l'exécution : EXECUTER A PARTIR DE](#81-lancer-lexécution--executer-a-partir-de)
  - [8.2 Les fins d'exécution](#82-les-fins-dexécution)
  - [8.3 Interrompre et continuer](#83-interrompre-et-continuer)
  - [8.4 PAUSE](#84-pause)
  - [8.5 POURSUIVRE JUSQU'EN](#85-poursuivre-jusquen)
  - [8.6 REPRENDRE](#86-reprendre)
  - [8.7 Pas à pas](#87-pas-à-pas)
  - [8.8 Quand peut-on continuer ?](#88-quand-peut-on-continuer-)
  - [8.9 Une méthode de mise au point](#89-une-méthode-de-mise-au-point)
- [9. Référence des commandes](#9-référence-des-commandes)
  - [9.1 Comment taper une commande](#91-comment-taper-une-commande)
  - [9.2 Tableau récapitulatif](#92-tableau-récapitulatif)
  - [9.3 Commandes de session](#93-commandes-de-session)
  - [9.4 Commandes d'édition](#94-commandes-dédition)
  - [9.5 Commandes d'exécution](#95-commandes-dexécution)
  - [9.6 Commandes de fichiers](#96-commandes-de-fichiers)
  - [9.7 Commandes diverses](#97-commandes-diverses)
  - [9.8 Commandes de l'état dormant](#98-commandes-de-létat-dormant)

**Partie IV — Fichiers, identifications et rubans**

- [10. Les fichiers](#10-les-fichiers)
  - [10.1 Programmes et données, temporaires et permanents](#101-programmes-et-données-temporaires-et-permanents)
  - [10.2 Les noms de fichiers](#102-les-noms-de-fichiers)
  - [10.3 Ranger et rappeler un programme](#103-ranger-et-rappeler-un-programme)
  - [10.4 Les fichiers-données](#104-les-fichiers-données)
  - [10.5 Exemple complet : un carnet de téléphone](#105-exemple-complet--un-carnet-de-téléphone)
  - [10.6 Rendre un fichier permanent : CATALOGUER](#106-rendre-un-fichier-permanent--cataloguer)
  - [10.7 Voir ses fichiers : TABLE DES FICHIERS et UTILISATION DISQUE](#107-voir-ses-fichiers--table-des-fichiers-et-utilisation-disque)
  - [10.8 Enchaîner des programmes : EXECUTER](#108-enchaîner-des-programmes--executer)
  - [10.9 Un programme en texte : DECODER et ENCODER](#109-un-programme-en-texte--decoder-et-encoder)
  - [10.10 L'unité amovible : `$` et DUPLIQUER (profil LSE F4)](#1010-lunité-amovible---et-dupliquer-profil-lse-f4)
  - [10.11 Passer d'un profil à l'autre](#1011-passer-dun-profil-à-lautre)
  - [10.12 Sauvegarder son travail](#1012-sauvegarder-son-travail)
- [11. Identifications, droits et protections](#11-identifications-droits-et-protections)
  - [11.1 Pourquoi des identifications](#111-pourquoi-des-identifications)
  - [11.2 Numéro, clé et compte](#112-numéro-clé-et-compte)
  - [11.3 Les attributs](#113-les-attributs)
  - [11.4 Créer les identifications : l'état dormant](#114-créer-les-identifications--létat-dormant)
  - [11.5 Changer d'identification en session : IDENTIFICATION](#115-changer-didentification-en-session--identification)
  - [11.6 L'identification au lancement : -ID](#116-lidentification-au-lancement---id)
  - [11.7 La propriété des fichiers](#117-la-propriété-des-fichiers)
  - [11.8 Protéger un programme ou un fichier : ,E et ,L](#118-protéger-un-programme-ou-un-fichier--e-et-l)
  - [11.9 Le programme secret](#119-le-programme-secret)
  - [11.10 Exemple : une salle de classe](#1110-exemple--une-salle-de-classe)
  - [11.11 Récapitulatif des droits](#1111-récapitulatif-des-droits)
- [12. Les rubans](#12-les-rubans)
  - [12.1 Le ruban, d'hier à aujourd'hui](#121-le-ruban-dhier-à-aujourdhui)
  - [12.2 Où est le ruban](#122-où-est-le-ruban)
  - [12.3 Perforer un programme : PERFORER](#123-perforer-un-programme--perforer)
  - [12.4 Lire un ruban](#124-lire-un-ruban)
  - [12.5 Préparer un ruban avec l'éditeur d'ORCA](#125-préparer-un-ruban-avec-léditeur-dorca)
  - [12.6 Un ruban qui se range tout seul](#126-un-ruban-qui-se-range-tout-seul)
  - [12.7 Fournir les réponses d'un programme par le ruban](#127-fournir-les-réponses-dun-programme-par-le-ruban)
  - [12.8 Un programme qui perfore : EQC(18) et EQC(20)](#128-un-programme-qui-perfore--eqc18-et-eqc20)
  - [12.9 Sauvegarder des données sur ruban : PERFO et CHARG](#129-sauvegarder-des-données-sur-ruban--perfo-et-charg)
  - [12.10 Les caractères de commande du ruban](#1210-les-caractères-de-commande-du-ruban)

**Annexes**

- [Annexe A. Messages d'erreur](#annexe-a-messages-derreur)
  - [A.1 La forme des messages](#a1-la-forme-des-messages)
  - [A.2 Erreurs du dialogue : `????`](#a2-erreurs-du-dialogue--)
  - [A.3 Erreurs de compilation : `ERREUR  C nn`](#a3-erreurs-de-compilation--erreur--c-nn)
  - [A.4 Erreurs d'exécution : `ERREUR (EX.) nn`](#a4-erreurs-dexécution--erreur-ex-nn)
  - [A.5 Erreurs de fichiers : `ERREUR F nn`](#a5-erreurs-de-fichiers--erreur-f-nn)
  - [A.6 Messages du lancement](#a6-messages-du-lancement)
- [Annexe B. Grammaire](#annexe-b-grammaire)
  - [B.1 Notation](#b1-notation)
  - [B.2 Éléments lexicaux](#b2-éléments-lexicaux)
  - [B.3 Lignes](#b3-lignes)
  - [B.4 Instructions](#b4-instructions)
  - [B.5 Procédures](#b5-procédures)
  - [B.6 Expressions](#b6-expressions)
  - [B.7 Fonctions](#b7-fonctions)
  - [B.8 Conditions](#b8-conditions)
  - [B.9 Formats](#b9-formats)
  - [B.10 Langage de commande](#b10-langage-de-commande)
- [Annexe C. Table des caractères](#annexe-c-table-des-caractères)
  - [C.1 Les caractères imprimables](#c1-les-caractères-imprimables)
  - [C.2 Les caractères de commande](#c2-les-caractères-de-commande)
  - [C.3 Les codes 128 à 255](#c3-les-codes-128-à-255)
- [Annexe D. Différences entre les trois profils](#annexe-d-différences-entre-les-trois-profils)
- [Annexe E. Limites et capacités](#annexe-e-limites-et-capacités)
- [Annexe F. Aide-mémoire](#annexe-f-aide-mémoire)
  - [Touches](#touches)
  - [Commandes (deux lettres)](#commandes-deux-lettres)
  - [Instructions](#instructions)
  - [Opérateurs](#opérateurs)
  - [Fonctions](#fonctions)
  - [Formats](#formats)
- [Annexe G. Programmes d'exemple](#annexe-g-programmes-dexemple)
  - [G.1 DEVIN : deviner un nombre](#g1-devin--deviner-un-nombre)
  - [G.2 MULTI : table de multiplication](#g2-multi--table-de-multiplication)
  - [G.3 MOYEN : moyenne de notes](#g3-moyen--moyenne-de-notes)
  - [G.4 CONJU : conjugaison au présent](#g4-conju--conjugaison-au-présent)
  - [G.5 PALIN : palindromes](#g5-palin--palindromes)
  - [G.6 JOUR : le jour de la semaine d'une date](#g6-jour--le-jour-de-la-semaine-dune-date)
  - [G.7 HANOI : les tours de Hanoï](#g7-hanoi--les-tours-de-hanoï)
  - [G.8 TRI : trier des nombres](#g8-tri--trier-des-nombres)
- [Annexe H. Glossaire](#annexe-h-glossaire)
- [Annexe I. Questions fréquentes et dépannage](#annexe-i-questions-fréquentes-et-dépannage)
- [Annexe J. Bibliographie](#annexe-j-bibliographie)
  - [Le LSE](#le-lse)
  - [Le shell ORCA](#le-shell-orca)


---

# Partie I — Prise en main

## 1. Installation sur l'Apple IIGS

### 1.1 Ce qu'il faut

LSE-IIGS est un programme en mode texte, qui se lance depuis le **shell ORCA**, l'interpréteur de commandes fourni avec les environnements de programmation ORCA/C et ORCA/M de Byte Works. Il faut donc :

- un Apple IIGS, avec GS/OS ;
- le shell ORCA installé ;
- de préférence un disque dur, sur lequel LSE-IIGS range vos programmes et vos fichiers.

LSE-IIGS a été éprouvé sur la configuration suivante :

| Élément | Configuration éprouvée |
|---|---|
| Machine | Apple IIGS ROM 01, 4 Mo de mémoire |
| Disque | carte MicroDrive Turbo |
| Accélérateur | TransWarp GS à 16 MHz |
| Système | GS/OS 6.0.4 |
| Shell | ORCA/Shell 2.0.6 |

Ce n'est pas une configuration minimale : LSE-IIGS peut fonctionner sur d'autres configurations, mais elles n'ont pas été essayées. Sans accélérateur, il fonctionne de la même façon, plus lentement.

### 1.2 Contenu de la disquette

LSE-IIGS est livré sous la forme d'une image de disquette 3,5 pouces de 800 Ko, `LSE.po`, au format ProDOS. Le volume s'appelle `/LSE/` et contient trois fichiers :

| Fichier | Type | Rôle |
|---|---|---|
| `LSE` | EXE | le programme LSE-IIGS |
| `CHARG` | TXT | un ruban portant le programme CHARG (voir le paragraphe 12.9) |
| `PERFO` | TXT | un ruban portant le programme PERFO (voir le paragraphe 12.9) |

### 1.3 Transférer l'image

Pour utiliser `LSE.po` sur un Apple IIGS réel, écrivez l'image sur une disquette 3,5 pouces de 800 Ko, ou servez-vous d'un dispositif qui présente une image disque à l'Apple IIGS comme une disquette ou un disque. Suivez pour cela la notice de l'outil de transfert que vous employez. Pour un émulateur, voyez le paragraphe 1.8.

### 1.4 Installer LSE comme commande du shell

Installé comme commande, LSE se lance depuis n'importe quel répertoire, en tapant simplement `LSE`. L'installation se fait une fois pour toutes, en trois étapes.

1. **Copier le programme dans le répertoire des utilitaires d'ORCA.** Ce répertoire est désigné par le préfixe `17:`. La disquette étant dans le lecteur, tapez dans le shell :

   ```text
   COPY :LSE:LSE 17:
   ```

2. **Déclarer la commande.** Les commandes du shell sont décrites dans le fichier `SYSCMND`, dans le répertoire désigné par le préfixe `15:`. Ouvrez-le avec l'éditeur d'ORCA :

   ```text
   EDIT 15:SYSCMND
   ```

   Ajoutez une ligne qui commence en première colonne par le nom de la commande, suivi d'au moins un espace et de la lettre `U` (qui signifie « utilitaire ») :

   ```text
   LSE      U     Langage Symbolique d'Enseignement
   ```

   Le texte qui suit le `U` est un commentaire facultatif. Ne mettez pas d'astérisque devant le `U` : LSE n'est pas prévu pour rester en mémoire entre deux lancements. Enregistrez et quittez l'éditeur.

3. **Faire relire la table des commandes**, ou redémarrer :

   ```text
   COMMANDS 15:SYSCMND
   ```

Vous pouvez maintenant taper `LSE` dans le shell. Le manuel d'ORCA décrit ces opérations en détail (« Installing a New Utility »).

### 1.5 Ou lancer LSE sans l'installer

On peut aussi lancer LSE directement depuis la disquette, en tapant le chemin complet du programme :

```text
:LSE:LSE
```

Les options de lancement (chapitre 2) se placent de la même façon après le chemin.

### 1.6 La bibliothèque : là où LSE range vos fichiers

LSE range vos programmes et vos fichiers de données dans un répertoire appelé la **bibliothèque**. Par défaut, c'est le répertoire `LSE` du dossier d'ORCA, désigné dans le shell par `@:LSE` (sur un disque dur, le préfixe `@` désigne le dossier où se trouve `ORCA.Sys16`). Une autre bibliothèque peut être choisie au lancement par l'option `-BIB` (paragraphe 2.3).

Au premier lancement, LSE crée la bibliothèque s'il ne la trouve pas. Elle contient :

| Élément | Contenu |
|---|---|
| `PROGRAMMES` | les programmes rangés par `RANGER` |
| `DONNEES` | les fichiers de données permanents |
| `TEMPORAIRES` | les fichiers de données temporaires de la session en cours |
| `CATALOGUE` | la liste des fichiers permanents, avec leur propriétaire, leur date et leur protection |
| `IDENTIFICATIONS` | les identifications définies (chapitre 11) |
| `RUBAN` | le ruban, quand vous en avez perforé un (chapitre 12) |

Ces éléments sont créés au fur et à mesure des besoins. **Ne modifiez pas à la main** les fichiers de `PROGRAMMES`, `DONNEES`, `CATALOGUE` et `IDENTIFICATIONS` : LSE les contrôle et refuserait un fichier abîmé. Le fichier `RUBAN`, lui, est un simple fichier texte, que vous pouvez lire, copier, renommer ou supprimer avec les commandes du shell (chapitre 12).

Pour **sauvegarder** tout votre travail, il suffit de copier la bibliothèque entière, LSE étant arrêté :

```text
COPY @:LSE :SAUVEGARDE:LSE
```

### 1.7 Installer les programmes PERFO et CHARG

La disquette porte deux programmes LSE, PERFO et CHARG, sous la forme de rubans. Ils servent à sauvegarder un fichier de données sur un ruban, puis à le restituer (paragraphe 12.9). Pour les installer dans votre bibliothèque, lisez chaque ruban une fois. Le ruban contient, après les lignes du programme, les commandes qui le rangent dans la bibliothèque puis l'effacent de la mémoire.

1. Lancez LSE en désignant le ruban `CHARG` de la disquette :

   ```text
   LSE -RUBAN :LSE:CHARG
   ```

2. Répondez à la question de la date par ‹RETURN›, tapez `BO` puis ‹X-OFF› pour ouvrir la session, puis, sur la ligne vide qui suit la bannière, frappez ‹X-ON› (Ctrl-Q). LSE lit le ruban : les lignes du programme défilent, puis `RANGER CHARG` et `EFFACER LIGNES *`.
3. Tapez `AU` puis ‹X-OFF› pour quitter.
4. Recommencez avec le ruban `PERFO` (`LSE -RUBAN :LSE:PERFO`).

Ranger un programme demande le droit de créer des programmes. Sous le profil LSE F4, la session s'ouvre avec ce droit. Sous `-T1600` et `-MITRA15`, ajoutez l'option `-ID 1` ; sous `-MITRA15`, la lecture se lance par la commande `RUBAN` au lieu de ‹X-ON› (paragraphe 12.4). Un programme rangé sous un profil ne peut être rappelé que sous ce profil. Si vous vous servez de plusieurs profils, donnez à chacun sa bibliothèque (option `-BIB`) et installez-y PERFO et CHARG (paragraphe 10.11). Si le programme est déjà installé, LSE répond `ERREUR F 03` et rien n'est changé.

### 1.8 Utiliser un émulateur

`LSE.po` s'utilise aussi dans un émulateur d'Apple IIGS sur lequel GS/OS et le shell ORCA sont installés : montez l'image comme une disquette 3,5 pouces, puis suivez les paragraphes 1.4 à 1.7. Le comportement de LSE est le même que sur la machine réelle.

---

## 2. Lancement et paramètres

### 2.1 Lancer LSE

Dans le shell ORCA, tapez `LSE`, éventuellement suivi d'options (paragraphe 2.3), puis ‹RETURN›. Sous le profil LSE F4, LSE commence par demander la date et l'heure :

```text
LSE F4

DATE HEURES MINUTES : 
```

Répondez simplement par ‹RETURN› : LSE prend alors la date et l'heure de l'horloge de l'Apple IIGS (paragraphe 2.2). Il affiche :

```text
EN LIGNE....
```

LSE est alors prêt, mais **en sommeil** : il attend qu'on ouvre une session par la commande `BONJOUR` (chapitre 3). Cet état s'appelle l'**état dormant** (paragraphe 4.1).

> **Sous `-T1600` et `-MITRA15`** LSE ne demande pas la date : il passe directement à l'état dormant, sans rien afficher.

### 2.2 La question de la date

Sur le T1600 d'origine, l'opérateur donnait la date et l'heure à la machine en la mettant en marche. LSE-IIGS reproduit cette question sous le profil LSE F4. Les réponses possibles sont :

| Réponse | Effet |
|---|---|
| ‹RETURN› seul | la date et l'heure de l'Apple IIGS ; c'est le choix ordinaire |
| `jj/mm/aa hh:mm:ss` ou `jj/mm/aa hh:mm` | la date et l'heure données, par exemple `15/10/80 08:30` |
| `00/00/00 hh:mm` | une heure sans date : `BONJOUR` et `AU REVOIR` n'affichent alors que l'heure |

Une réponse incorrecte fait reposer la question. Quand vous donnez une date, elle reste fixe pendant toute l'utilisation de LSE, et l'heure avance à partir de celle que vous avez donnée, sans repasser à zéro à minuit (après 23:59:59 vient 24:00:00). Ce réglage ne touche pas l'horloge de l'Apple IIGS ; il vaut jusqu'au retour au shell.

### 2.3 Les options

Les options se placent après le nom du programme, séparées par des espaces. Elles peuvent être données dans n'importe quel ordre.

| Option | Effet | Par défaut |
|---|---|---|
| `-F4` | profil LSE F4 | oui |
| `-T1600` | profil T1600 version 4 | |
| `-MITRA15` | profil MITRA 15 version 8 | |
| `-LIBELLES` | fait suivre chaque message d'erreur d'une explication en clair | messages sans explication |
| `-BIB chemin` | désigne la bibliothèque | `@:LSE` |
| `-UNITE chemin` | désigne l'unité amovible (profil LSE F4, paragraphe 10.10) | aucune |
| `-RUBAN chemin` | désigne le fichier qui tient lieu de ruban | `RUBAN` dans la bibliothèque |
| `-ID n` | identification sous laquelle s'ouvre chaque session (chapitre 11) | 1 sous LSE F4, 0 sinon |
| `-TEMP n` | taille de l'espace temporaire, en secteurs de 256 octets | 200 |

Quelques précisions :

- **Profil.** Si plusieurs profils sont donnés, le dernier l'emporte. Le profil ne change pas pendant l'utilisation de LSE.
- **`-LIBELLES`.** Par défaut, LSE n'affiche qu'un code (`ERREUR  C 03`), comme les systèmes d'origine. Avec `-LIBELLES`, il ajoute une explication : `ERREUR  C 03 : IDENTIFICATEUR DE PLUS DE 5 CARACTERES`. C'est très utile pour apprendre.
- **`-BIB`.** Le chemin est un répertoire au sens de GS/OS, par exemple `:DUR:CLASSE:LSE` ou `@:ESSAIS`. S'il n'existe pas, LSE le crée, à condition que le répertoire qui le contient existe. Plusieurs bibliothèques permettent de séparer des travaux, ou des utilisateurs.
- **`-UNITE`.** Le chemin désigne un répertoire qui joue le rôle d'une disquette de l'époque, par exemple `:DISQUETTE:LSE` sur une disquette 3,5 pouces. Il n'a de sens que sous le profil LSE F4.
- **`-RUBAN`.** Le fichier désigné sert à la fois à la lecture et à la perforation. Il peut se trouver n'importe où, sur une disquette par exemple.
- **`-ID`.** Le nombre va de 0 à 65 499. Une identification qui n'a pas été définie (chapitre 11) est remplacée par 0.
- **`-TEMP`.** Le nombre est un entier strictement positif.

Une option inconnue, une option à laquelle manque sa valeur, ou une valeur incorrecte arrête le lancement, sans ouvrir de session :

```text
OPTION INCORRECTE : -ID 70000
```

### 2.4 Exemples de lancement

| Commande | Effet |
|---|---|
| `LSE` | profil LSE F4, bibliothèque `@:LSE`, identification 1 |
| `LSE -LIBELLES` | idem, avec l'explication des erreurs |
| `LSE -T1600 -ID 1` | profil T1600 version 4, sous l'identification 1 |
| `LSE -MITRA15 -BIB :DUR:MITRA` | profil MITRA 15, avec une bibliothèque à part |
| `LSE -RUBAN :LSE:CHARG` | lecture du ruban CHARG de la disquette |
| `LSE -UNITE :DISQUETTE:LSE` | une disquette 3,5 pouces sert d'unité amovible |
| `LSE -ID 0 -TEMP 400` | session sans droit d'écriture, espace temporaire de 400 secteurs |

### 2.5 Quitter LSE

La commande `AU REVOIR` (`AU` puis ‹X-OFF›) ferme la session et ramène au shell ORCA. Avant de quitter, pensez à ranger votre programme (`RANGER`, paragraphe 3.6) : le programme en mémoire, ses variables et les fichiers temporaires sont perdus à `AU REVOIR`.

### 2.6 Si la bibliothèque est inaccessible

Si LSE ne peut ni ouvrir ni créer la bibliothèque (disquette protégée, chemin trop long, volume absent), il affiche au lancement :

```text
ERREUR F 102
```

La session fonctionne quand même, mais toutes les commandes et instructions qui touchent aux fichiers répondent `ERREUR F 102`. Quittez, puis relancez avec une bibliothèque accessible (`-BIB`). Le chemin de `-BIB` ne doit pas dépasser 170 caractères.

---

## 3. Premiers pas

Ce chapitre vous guide pas à pas, de l'ouverture d'une session jusqu'au rangement d'un premier programme. Dans le tableau ci-dessous, la colonne de gauche dit ce que vous faites, celle de droite ce que montre l'écran. Une touche notée ‹X-OFF› se frappe, sous le profil LSE F4, avec la touche **DELETE** de l'Apple IIGS (ou avec Ctrl-S, voir le paragraphe 4.2).

### 3.1 Ouvrir une session

Lancez LSE depuis le shell (chapitre 2), puis :

| Vous tapez | L'écran affiche |
|---|---|
| `LSE` ‹RETURN› | `LSE F4`, puis `DATE HEURES MINUTES :` |
| ‹RETURN› | `EN LIGNE....` |
| `BO` ‹X-OFF› | `BONJOUR 06/11/26 14:30:05` et la bannière |

`BO` est l'abréviation de la commande `BONJOUR` : on tape toujours les deux premières lettres d'une commande, et LSE écrit lui-même la suite. L'écran montre alors :

```text
BONJOUR 06/11/26 14:30:05
CONSOLE NO. 01.
LSE F4
RUB-OUT REMPLACE CTRL X-OFF
VOUS ETES SOUS ID 1





```

La bannière rappelle le numéro de la console, le nom du système, que la touche RUB-OUT (DELETE) remplace X-OFF, et l'identification sous laquelle vous travaillez (chapitre 11). La session est ouverte : LSE attend vos ordres.

### 3.2 L'ordinateur comme calculatrice

Tapez un point d'interrogation suivi d'un calcul, puis ‹X-OFF› :

```text
?2+3
5
?RAC(2)
1.414214
?(17+4)/3
7
```

`RAC` est la racine carrée. C'est le **mode machine de bureau** : une ligne sans numéro est exécutée tout de suite (chapitre 6).

### 3.3 Un premier programme

Un programme est une suite de **lignes numérotées**. Tapez les quatre lignes suivantes, chacune terminée par ‹X-OFF› (rappel : `←` se tape `_`, `↑` se tape `^`) :

```text
10 AFFICHER 'TABLE DES CARRES'
20 FAIRE 30 POUR I←1 JUSQUA 5
30 AFFICHER[/,F3.0,' AU CARRE FAIT',F5.0] I,I↑2
40 TERMINER
```

Chaque ligne est vérifiée dès qu'elle est tapée. Si LSE répond par exemple `ERREUR  C 38`, la ligne contient une faute : retapez-la entièrement, avec le même numéro. Rien n'est exécuté pour l'instant : les lignes sont seulement gardées en mémoire.

Ce programme affiche un titre (ligne 10), puis répète la ligne 30 pour `I` allant de 1 à 5 (ligne 20). La ligne 30 affiche, à la ligne (`/`), la valeur de `I` sur 3 positions sans décimale (`F3.0`), un texte, puis `I↑2` (I au carré) sur 5 positions. La ligne 40 termine le programme.

Pour voir le programme, tapez `LI` ‹X-OFF› : LSE complète en `LISTER A PARTIR DE` et attend un numéro. Tapez `10` ‹X-OFF› :

```text
LISTER A PARTIR DE 10
10 AFFICHER 'TABLE DES CARRES'
20 FAIRE 30 POUR I←1 JUSQUA 5
30 AFFICHER[/,F3.0,' AU CARRE FAIT',F5.0]I,I↑2
40 TERMINER
```

LSE réécrit chaque ligne à sa façon : l'espace entre `]` et `I` a disparu. C'est normal : il ne garde pas le texte tapé, mais une forme traduite, qu'il réécrit à la demande (paragraphe 7.6).

### 3.4 Le faire exécuter

Tapez `EX` ‹X-OFF›, puis `10` ‹X-OFF› :

```text
EXECUTER A PARTIR DE 10

TABLE DES CARRES
  1 AU CARRE FAIT    1
  2 AU CARRE FAIT    4
  3 AU CARRE FAIT    9
  4 AU CARRE FAIT   16
  5 AU CARRE FAIT   25
TERMINE EN LIGNE 040
```

`TERMINE EN LIGNE 040` indique que le programme s'est arrêté normalement, sur l'instruction `TERMINER` de la ligne 40. Les variables gardent leur valeur après l'exécution, et on peut les consulter :

```text
?I
6
```

`I` vaut 6 : c'est la valeur qui a fait sortir de la boucle.

### 3.5 Le modifier

Pour **ajouter** une ligne, tapez-la avec un numéro qui la place au bon endroit. Pour **remplacer** une ligne, retapez-la avec le même numéro. Pour **supprimer** une ligne, utilisez `EFFACER LIGNES` (`EF`).

```text
25 AFFICHER[/,'---']
```

La ligne 25 s'insère entre les lignes 20 et 30 : elle fait partie de la boucle et s'affiche avant chaque résultat. On numérote donc de 10 en 10, pour garder de la place. Pour l'enlever :

```text
EFFACER LIGNES 25
```

### 3.6 Le ranger et le retrouver

Un programme en mémoire disparaît à la fin de la session. Pour le garder, rangez-le dans la bibliothèque sous un nom d'au plus cinq lettres ou chiffres, commençant par une lettre :

```text
RANGER CARRE
```

(`RA` ‹X-OFF›, puis `CARRE` ‹X-OFF›.) Lors d'une prochaine session, `APPELER CARRE` (`AP`) le remettra en mémoire. `TABLE DES FICHIERS A PARTIR DE *` (`TA`, puis `*`) liste vos fichiers :

```text
TABLE DES FICHIERS A PARTIR DE *
ESPACE TEMPORAIRE( 200 SECT.) OCCUPE A   0 %

 NOM       TYPE        LONGUEUR MOTS   PROTECTION
CARRE     PROGRAMME           50
```

### 3.7 Fermer la session

```text
AU REVOIR 06/11/26 14:52:40
CONSOLE NO. 01.
LSE F4
RUB-OUT REMPLACE CTRL X-OFF
VOUS ETES SOUS ID 1
```

(`AU` ‹X-OFF›.) LSE affiche la date et l'heure, puis rend la main au shell ORCA.

### 3.8 Pour aller plus loin

Vous savez maintenant l'essentiel : ouvrir une session, calculer, écrire, exécuter, corriger, ranger. La suite du manuel détaille chacun de ces gestes. Les programmes de l'annexe G sont de bons exercices : tapez-les, exécutez-les, puis modifiez-les.

---

## 4. La console et le clavier

### 4.1 Les trois états de la console

À tout moment, la console est dans l'un de ces trois états :

| État | Quand | Ce qu'on peut taper |
|---|---|---|
| **dormant** | entre le lancement et `BONJOUR` | `BO` pour ouvrir une session ; les commandes de gestion des identifications `CR`, `SU`, `LI` (paragraphe 11.4) ; toute autre entrée reçoit la réponse `????` |
| **moniteur** | pendant la session, quand aucun programme ne s'exécute | des lignes de programme, des commandes, des lignes de machine de bureau |
| **exécution** | pendant qu'un programme s'exécute | rien, sauf les réponses aux instructions `LIRE` du programme, et la touche ‹ESC› |

LSE distingue ce que vous tapez en état moniteur d'après son premier caractère :

- une entrée qui commence par un **chiffre** est une **ligne de programme** ;
- une entrée de **deux lettres exactement** est une **commande** ;
- une entrée qui commence par `/`, `%` ou par la forme `[n]X[p]/` est une **correction** par l'éditeur de ligne (paragraphe 7.5) ;
- toute autre entrée est une **ligne de machine de bureau**, exécutée aussitôt (chapitre 6).

### 4.2 Terminer une saisie : X-OFF et RETURN

Comme sur les consoles d'origine, une saisie se termine par l'une de deux touches :

- **‹X-OFF›**, la fin de message de la télétype. Sur l'Apple IIGS, on la frappe par **Ctrl-S** (Control et S ensemble). Sous le profil LSE F4, la touche **DELETE** (l'ancienne RUB-OUT) fait la même chose, comme l'annonce la bannière ; c'est la plus commode.
- **‹RETURN›**.

Pour une commande, une ligne de programme ou une ligne de machine de bureau, les deux touches ont le même effet. Elles diffèrent en trois cas :

| Situation | ‹X-OFF› | ‹RETURN› |
|---|---|---|
| saisie vide | `????` | rien ne se passe |
| en mode pas à pas, saisie vide | l'exécution continue d'une ligne | rien ne se passe |
| réponse à `LIRE` d'une chaîne | termine la chaîne | range un passage à la ligne dans la chaîne, puis la termine |

À l'écran, ‹RETURN› passe à la ligne, ‹X-OFF› laisse le curseur où il est. La présentation des exemples peut donc varier d'une ou deux lignes blanches selon la touche employée. Prenez l'habitude de ‹X-OFF› : c'est la touche de fin du LSE.

> **Sous `-T1600` et `-MITRA15`** la touche DELETE est sans effet : ‹X-OFF› se frappe seulement par Ctrl-S.

### 4.3 Corriger en cours de frappe : la barre oblique inverse

Tant que la saisie n'est pas terminée, chaque `\` annule le caractère qui le précède. L'écran garde la trace de ce qui a été tapé, mais LSE reçoit le texte corrigé :

```text
10 AFFICHER 'BONJOUT\R'
```

est reçu comme `10 AFFICHER 'BONJOUR'`. Plusieurs `\` de suite annulent autant de caractères : `ABCD\\\X` donne `AX`. La correction vaut pour toute saisie, réponses à `LIRE` comprises. Les touches flèches du clavier et la touche TAB sont sans effet. Pour abandonner toute la ligne en cours, frappez ‹ESC›.

### 4.4 Interrompre : la touche ESC

La touche ‹ESC› sert à interrompre :

- **pendant une saisie en état moniteur** : la ligne en cours est abandonnée, et LSE affiche `PRET` ;
- **pendant l'exécution d'un programme** : le programme s'arrête au début de la ligne suivante, ou au tour suivant d'une boucle, et LSE affiche `PRET EN LIGNE nnn` ; la commande `CONTINUER` reprend là où il s'était arrêté (paragraphe 8.3) ;
- **pendant qu'un programme attend une réponse à `LIRE`** : le programme s'arrête avec `PRET EN LIGNE nnn` ; `CONTINUER` émet un bip et redemande la valeur attendue, les valeurs déjà lues étant gardées ;
- **pendant un listage** (`LISTER`, `NUMERO`, `PERFORER`, `TABLE DES FICHIERS`…) : le listage s'arrête, avec `PRET` ;
- **pendant la lecture d'un ruban** : la lecture s'arrête (chapitre 12).

Le programme ne regarde le clavier qu'au début de chaque ligne et à chaque tour de boucle : une instruction très longue (l'affichage d'un grand tableau, par exemple) va jusqu'à son terme avant l'arrêt.

### 4.5 Les flèches ← et ↑

Le LSE note l'affectation par une flèche vers la gauche, `A←3`, et la puissance par une flèche vers le haut, `2↑10`. Les claviers de l'époque avaient ces flèches à la place de `_` et de `^`. LSE-IIGS fait de même :

| Vous tapez | L'écran affiche | Sens |
|---|---|---|
| `_` (Majuscule et tiret) | `←` | affectation |
| `^` (Majuscule et 6) | `↑` | puissance |

L'affichage est le même partout : lignes tapées, listages, résultats des programmes, y compris dans les chaînes. Dans les fichiers (rubans, fichiers texte), ces caractères restent `_` et `^`. Les touches flèches du clavier ne les remplacent pas.

### 4.6 Majuscules et minuscules

Le LSE est écrit en majuscules. Hors d'une chaîne entre apostrophes et d'un commentaire, une minuscule est une erreur (`ERREUR  C 01`). **Enclenchez la touche Caps Lock** avant de travailler. Les minuscules restent permises dans les chaînes et les commentaires, et dans les réponses aux `LIRE` de chaînes.

### 4.7 Les messages de LSE

LSE parle peu, et toujours de la même façon :

| Message | Sens |
|---|---|
| `PRET` | ‹ESC› a été frappée en état moniteur, ou un listage a été interrompu |
| `PRET EN LIGNE 036` | le programme a été interrompu par ‹ESC› en ligne 36 ; `CONTINUER` le reprend |
| `TERMINE EN LIGNE 040` | le programme s'est arrêté sur `TERMINER`, en ligne 40 |
| `PAUSE EN LIGNE 161` | le programme s'est arrêté sur `PAUSE`, en ligne 161 ; `CONTINUER` le reprend |
| `ARRET EN LIGNE 030` | la ligne d'arrêt demandée a été atteinte (paragraphe 8.1) ; la ligne 30 n'a pas été exécutée |
| `LIGNE 020` | en mode pas à pas, l'exécution s'est arrêtée au début de la ligne 20 |
| `????` | commande inconnue, paramètre incorrect ou ordre impossible dans l'état présent |
| `ERREUR  C 38` | la ligne tapée contient une erreur (erreur de compilation) ; elle n'a pas été prise |
| `ERREUR (EX.) 31 EN LIGNE 050` | une erreur s'est produite pendant l'exécution, en ligne 50 |
| `ERREUR (EX.) 31 EN LIGNE BUR` | une erreur s'est produite dans une ligne de machine de bureau |
| `ERREUR F 05` | une opération sur un fichier a échoué |

Les numéros de ligne des messages s'écrivent toujours sur trois chiffres. L'annexe A explique chaque code d'erreur. Avec l'option `-LIBELLES`, l'explication suit le code sur la même ligne.

> **Sous `-MITRA15`** l'arrêt sur `TERMINER` s'affiche `TERMINE`, sans numéro de ligne.

### 4.8 Le bip

LSE émet un bip :

- au début de chaque instruction `LIRE` : le programme attend une réponse ;
- quand `CONTINUER` reprend une lecture interrompue ;
- quand une réponse à `LIRE` est mal formée (paragraphe 5.15) ;
- quand une ligne tapée devient trop longue.

### 4.9 Récapitulatif des touches

| Touche | Sur l'Apple IIGS | Rôle |
|---|---|---|
| ‹X-OFF› | Ctrl-S ; aussi DELETE sous LSE F4 | termine une saisie |
| ‹RETURN› | RETURN | termine une saisie ; dans une chaîne lue, y range un passage à la ligne |
| ‹ESC› | ESC | abandonne la saisie, interrompt un programme, un listage ou la lecture d'un ruban |
| ‹X-ON› | Ctrl-Q | sur une ligne vide, met en marche le lecteur de ruban (LSE F4 et T1600) |
| `\` | `\` | annule le caractère précédent |
| `←` | `_` | affectation |
| `↑` | `^` | puissance |

---

# Partie II — Le langage

## 5. Description du langage

Ce chapitre décrit tout le langage. Les exemples peuvent être essayés tels quels, soit dans un programme, soit (pour les plus courts) en mode machine de bureau, avec `?` devant une expression (chapitre 6).

### 5.1 Lignes et instructions

Un programme LSE est une suite de lignes. Chaque ligne se compose :

1. d'un **numéro**, de 1 à 255 ;
2. d'**au moins un espace** ;
3. d'**une ou plusieurs instructions**, séparées par des points-virgules ;
4. d'une fin de saisie, ‹X-OFF› ou ‹RETURN›.

```text
10 A←3;B←A*2
20 AFFICHER A,B
30 TERMINER
```

Règles :

- Une ligne compte au plus **79 caractères** (au-delà, `ERREUR  C 63`). Pour une instruction trop longue, répartissez le travail sur plusieurs lignes.
- Le numéro doit être suivi d'un espace : `10A←3` est refusé (`ERREUR  C 07`). Seul un commentaire peut suivre le numéro sans espace : `10*TITRE` (paragraphe 5.22).
- Les lignes sont exécutées dans l'**ordre croissant des numéros**, quel que soit l'ordre dans lequel on les a tapées, et les instructions d'une même ligne de gauche à droite.
- Une ligne tapée avec un numéro qui existe déjà **remplace** l'ancienne.
- Une ligne réduite à son numéro suivi d'un espace (`50 `) est une **ligne vide** : elle ne fait rien, mais elle existe. Elle sert de fin de boucle (paragraphe 5.14).
- Les numéros n'ont pas besoin de se suivre : numérotez de 10 en 10 pour pouvoir insérer des lignes plus tard.

> **Sous `-MITRA15`** les numéros vont de 1 à 250.

### 5.2 Les mots du langage

Les instructions s'écrivent avec des mots français en majuscules, sans accent : `AFFICHER`, `LIRE`, `SI`, `ALORS`, `SINON`, `FAIRE`… Chaque mot doit être suivi d'un espace, d'un point-virgule ou de la fin de la ligne, sauf `AFFICHER`, qui peut être suivi d'un crochet `[`, et les noms de fonctions, qui peuvent être suivis d'une parenthèse `(`. Ainsi `SIA=0` n'est pas compris comme `SI A=0`. Hors de ces règles, les espaces sont libres : `A←B+C` et `A ← B + C` sont la même instruction. Il ne faut pas d'espace à l'intérieur d'un nom.

Certains mots sont **réservés** : ils ne peuvent pas servir de nom de variable.

| Mots réservés | Profils |
|---|---|
| `AFFICHER` `ALLER` `ALORS` `CHAINE` `CHARGER` `DEBUT` `ET` `EXECUTER` `FAIRE` `FIN` `GARER` `JUSQUA` `LIBERER` `LIRE` `LOCAL` `OU` `PAS` `PAUSE` `POUR` `PROCEDURE` `RESULTAT` `RETOUR` `SI` `SINON` `TABLEAU` `TANT` `TERMINER` | tous |
| `DAT` `TEM` | LSE F4 et T1600 |
| `SUPPRIMER` `REPRENDRE` | MITRA 15 |

Un nom de fonction comme `SIN` ou `LGR` n'est pas réservé. Suivi d'une parenthèse, il désigne la fonction ; sinon, c'est un nom de variable ordinaire. Mieux vaut cependant ne pas s'en servir comme nom de variable.

### 5.3 Les nombres

Un nombre s'écrit avec un **point** décimal, et peut porter un exposant de dix introduit par la lettre `E` :

| Écriture LSE | Valeur |
|---|---|
| `3` | 3 |
| `-5` | −5 |
| `56.24` | 56,24 |
| `3E-20` | 3 × 10⁻²⁰ |
| `-26.4E+07` | −26,4 × 10⁷ |

Un nombre ne contient que des chiffres, le point, la lettre `E` et un signe après `E`. Il n'y a ni espace ni virgule dans un nombre.

**Précision.** Tous les nombres sont des « réels » : il n'y a pas de type entier séparé. Un réel garde environ **six à sept chiffres significatifs**. Les entiers sont exacts jusqu'à 8 388 607 (2²³ − 1) ; au-delà, ils sont arrondis. Les nombres vont d'environ 1,2 × 10⁻³⁸ à 3,4 × 10³⁸ ; un résultat plus petit devient 0, un résultat plus grand provoque `ERREUR (EX.) 44`.

**Pourquoi `0.1` s'affiche `0.09999999`.** L'ordinateur compte en binaire, et la plupart des nombres décimaux, comme 0,1, n'y tombent pas juste : il en garde la valeur la plus proche. LSE affiche sept chiffres significatifs, et ce septième chiffre laisse voir l'écart :

```text
?0.1,8.3,2/3,0.001
0.09999999 8.299999 0.6666666 9.999999E-04
```

C'était déjà le cas sur le T1600 d'origine. Pour un affichage soigné, utilisez un format (paragraphe 5.16) : `AFFICHER[F1.2] 0.1` affiche `0.10`. Pour comparer des réels calculés, préférez un écart (`SI ABS(X-Y)<1E-5`) à une égalité.

> **Sous `-MITRA15`** les réels ont un chiffre binaire de plus : les entiers sont exacts jusqu'à 16 777 215, et `?0.1,8.3,2/3,0.001` affiche `0.1 8.3 0.6666667 0.001`.

### 5.4 Les chaînes de caractères

Une **chaîne** est une suite de caractères : un mot, une phrase, un nom. Dans un programme, une chaîne constante s'écrit entre **apostrophes** ; une apostrophe à l'intérieur se double :

```text
'BONJOUR'
'IL FAIT BEAU.'
'AUJOURD''HUI'
''
```

La dernière est la **chaîne vide**, qui ne contient aucun caractère. Une chaîne peut contenir des minuscules et tous les caractères du clavier. Sa longueur peut atteindre 65 535 caractères (dans la limite de la mémoire). Une chaîne constante, elle, est limitée par la longueur de la ligne.

Les positions des caractères se comptent à partir de 1 : dans `'LE SOIR'`, `L` est en position 1, l'espace en position 3, `R` en position 7.

**Caractères spéciaux : la notation `.n.`** Sous le profil LSE F4, une chaîne constante peut contenir des caractères désignés par leur **code** (annexe C), écrits entre deux points, et accolés aux morceaux entre apostrophes, sans espace :

```text
AFFICHER 'ATTENTION'.7.' !'
AFFICHER .7 7 7.
```

La première instruction affiche `ATTENTION !` en faisant biper la console (le code 7 est la sonnerie) ; la seconde bipe trois fois. Plusieurs codes se séparent par des espaces : `.27 12.` vaut deux caractères. Les codes vont de 0 à 127. En listant le programme, LSE réécrit les caractères non imprimables sous cette forme, et les autres en clair (`.65.` est relisté `'A'`).

> **Sous `-T1600` et `-MITRA15`** la notation `.n.` n'existe pas : on obtient ces caractères par la fonction `EQC` (paragraphe 5.18), par exemple `'ATTENTION'!EQC(7)!' !'`.

### 5.5 Les noms

Les variables, tableaux et chaînes sont désignés par des **noms** (on dit aussi *identificateurs*) :

- une **lettre**, suivie d'au plus **quatre** lettres ou chiffres, soit cinq caractères au plus ;
- sans espace, ni autre signe ;
- qui n'est pas un mot réservé.

```text
K   J2   TABLO   A0001   NOTE   SOMME
```

sont des noms corrects ; `3ZUT` (commence par un chiffre), `Z.Q` (contient un point), `MOYENNE` (sept caractères, `ERREUR  C 03`) ne le sont pas.

Les noms de **procédures** commencent par le caractère `&`, suivi d'un nom (paragraphe 5.20) : `&TRI`, `&JOUR`.

### 5.6 Les variables

Une variable est une case de la mémoire, désignée par un nom, qui contient une valeur. Il en existe trois sortes.

**Variable numérique.** C'est la sorte ordinaire : tout nom employé sans déclaration est une variable numérique. Elle reçoit une valeur par une affectation (`A←3`), par `LIRE` ou par `CHARGER`.

**Variable-chaîne.** Elle contient une chaîne. Il faut la **déclarer** par l'instruction `CHAINE` avant de s'en servir :

```text
10 CHAINE NOM,ADRES,Z1
20 NOM←'DUPONT'
```

Sans cette déclaration, `NOM←'DUPONT'` provoque `ERREUR (EX.) 06` : une chaîne ne peut pas être rangée dans une variable numérique.

**Tableau.** Un tableau est un ensemble de nombres désignés par un même nom et repérés par un ou deux **indices** entre crochets. Il se déclare par `TABLEAU`, avec la valeur maximale de chaque indice ; les indices commencent toujours à 1 :

```text
TABLEAU B[54]
TABLEAU MAT[3,3]
TABLEAU V1[N],V2[N+1],M[5,5]
```

`B[54]` a 54 éléments, `B[1]` à `B[54]` ; `MAT[3,3]` en a 9, `MAT[1,1]` à `MAT[3,3]`. Les dimensions peuvent être des expressions, calculées au moment où l'instruction `TABLEAU` est exécutée. Un indice non entier est arrondi à l'entier le plus proche : `MAT[3.14,1]` désigne `MAT[3,1]`. Un tableau compte au plus 65 535 éléments. Les tableaux ne contiennent que des nombres ; il n'existe pas de tableau de chaînes.

**Valeur non définie.** Tant qu'une variable n'a pas reçu de valeur, elle est **non définie**, et l'utiliser est une erreur (par exemple `ERREUR (EX.) 05` dans un calcul, `ERREUR (EX.) 62` dans un affichage). Les éléments d'un tableau sont non définis après `TABLEAU`, jusqu'à ce qu'on leur donne une valeur (`ERREUR (EX.) 32` sinon).

**Les déclarations s'exécutent.** `CHAINE` et `TABLEAU` sont des instructions comme les autres : elles prennent effet quand elles sont exécutées, et doivent l'être avant l'emploi de la variable. Exécuter de nouveau `TABLEAU T[…]` sur un tableau existant le recrée, vide. Déclarer `TABLEAU` sur une variable numérique ou une chaîne, ou `CHAINE` sur un nombre ou un tableau, est une erreur (`ERREUR (EX.) 29` ou `04`).

**Libérer.** L'instruction `LIBERER` rend la place occupée par des variables, tableaux ou chaînes, et rend leurs noms libres pour un autre usage :

```text
LIBERER A,B,MAT
```

Après `LIBERER T`, le nom `T` peut redevenir une variable simple ou une chaîne. Dans une procédure, on ne peut libérer que ses noms locaux (paragraphe 5.20).

### 5.7 Les expressions numériques

Une **expression** est une formule de calcul. Elle combine des nombres, des variables, des éléments de tableau, des appels de fonctions, des parenthèses et des opérateurs :

| Opérateur | Opération |
|---|---|
| `+` | addition |
| `-` | soustraction, et changement de signe |
| `*` | multiplication |
| `/` | division |
| `↑` | puissance (`2↑10` vaut 1024) |

**Ordre des calculs.** Sans parenthèses, LSE calcule d'abord les fonctions, puis les puissances, puis les multiplications et divisions, puis les additions, soustractions et changements de signe. À priorité égale, il calcule de gauche à droite :

| Expression | Calculée comme |
|---|---|
| `X/Y*Z` | `(X/Y)*Z` |
| `-A+3/D*E` | `(-A)+((3/D)*E)` |
| `2*X↑2` | `2*(X↑2)` |

Les parenthèses changent l'ordre : `X/(Y*Z)`.

Deux règles d'écriture :

- deux opérateurs ne se suivent jamais : `A*-B` s'écrit `A*(-B)` ;
- la multiplication s'écrit toujours : `(X-Y)(X+Y)` s'écrit `(X-Y)*(X+Y)`.

**Puissances.** `X↑N` admet un `X` négatif si `N` est entier (`(-2)↑3` vaut −8). Un nombre négatif élevé à une puissance non entière (`ERREUR (EX.) 26`) et zéro élevé à une puissance négative (`ERREUR (EX.) 25`) sont des erreurs ; `0↑0` vaut 1.

**Expression conditionnelle.** La forme `SI condition ALORS e1 SINON e2` donne la valeur de `e1` si la condition est vraie, celle de `e2` sinon :

```text
FONCT←SI X#0 ALORS SIN(X)/X SINON 1
B[I,J]←SI I=J ALORS 1 SINON 0
```

La partie après `SINON` s'étend aussi loin que possible : `Y←SI A>B ALORS A SINON A+K*B` donne à `Y`, si `A≤B`, la valeur de `A+K*B` tout entière. Placée après un opérateur, une expression conditionnelle doit être mise **entre parenthèses** : `Y←3*(SI A>B ALORS 1 SINON 2)`.

### 5.8 Les expressions-chaînes

Les chaînes se combinent avec un seul opérateur, la **concaténation** `!`, qui les met bout à bout :

```text
'AB'!'CDE'                  donne  'ABCDE'
'RABAIS '!'DE'!' 3%'        donne  'RABAIS DE 3%'
NOM!', BONJOUR !'           donne, si NOM vaut 'LEA',  'LEA, BONJOUR !'
```

Une expression-chaîne peut contenir des chaînes constantes, des variables-chaînes, des fonctions à résultat chaîne (paragraphe 5.18), des appels de procédures-fonctions, et la forme conditionnelle :

```text
'IL Y A '!CCA(N)!(SI N>1 ALORS ' POMMES' SINON ' POMME')
```

On ne mélange pas chaînes et nombres : `'A'+1` est refusé (`ERREUR  C 27`). Pour mettre un nombre dans une chaîne, on le convertit par `CCA` ; pour l'inverse, par `CNB` (paragraphe 5.18).

### 5.9 Les conditions

Une **condition** compare deux valeurs avec l'un de ces six opérateurs :

| Opérateur | Sens |
|---|---|
| `<` | plus petit que |
| `<=` | plus petit ou égal à |
| `=` | égal à |
| `#` | différent de |
| `>=` | plus grand ou égal à |
| `>` | plus grand que |

On compare deux nombres, ou deux chaînes, mais jamais un nombre et une chaîne (`ERREUR  C 11`). Les chaînes se comparent caractère par caractère, de gauche à droite, selon les codes des caractères (annexe C) ; si l'une est le début de l'autre, la plus longue est la plus grande :

| Comparaison | Résultat |
|---|---|
| `'ABC'<'C'` | vrai |
| `'ABC'='ABC'` | vrai |
| `'ABC '>'ABC'` | vrai |
| `'ZOE'<'ADAM'` | faux |

Les conditions se combinent avec `ET` et `OU` ; `ET` passe avant `OU`, et les parenthèses changent cet ordre :

```text
SI A>=0 ET A<=1 ALORS ALLER EN 200
SI A=B ET (B=C OU B>0) ALORS PAUSE
```

**Attention :** LSE calcule **toujours les deux côtés** d'un `ET` ou d'un `OU`. La condition `I<=N ET T[I]>0` provoque donc une erreur quand `I` dépasse `N`, car `T[I]` est calculé quand même. Séparez alors les tests : `SI I<=N ALORS SI T[I]>0 ALORS …`.

### 5.10 L'affectation

L'affectation range une valeur dans une variable. Elle s'écrit avec la flèche `←` (tapée `_`) :

```text
Y←3*X+4
MAT[I,J]←MAT[I,J]-1
MOT←'LE CHAT'
MOT2←MOT!' EST NOIR.'
```

L'expression de droite est calculée, puis sa valeur est rangée dans la variable de gauche. Une chaîne ne va que dans une variable déclarée `CHAINE`, un nombre que dans une variable numérique ou un élément de tableau (`ERREUR (EX.) 06` sinon).

### 5.11 Le déroulement d'un programme

**Ordre.** Le programme s'exécute à partir de la ligne demandée par `EXECUTER A PARTIR DE` (paragraphe 8.1), puis ligne après ligne dans l'ordre des numéros.

**TERMINER.** Tout programme doit s'arrêter sur l'instruction `TERMINER`. Un programme qui arrive au bout de sa dernière ligne sans l'avoir rencontrée s'arrête sur `ERREUR (EX.) 58`. Après `TERMINER`, LSE affiche `TERMINE EN LIGNE nnn` et l'on ne peut pas faire continuer le programme ; on peut le relancer.

**PAUSE.** L'instruction `PAUSE` arrête le programme avec `PAUSE EN LIGNE nnn`. On peut alors examiner les variables (chapitre 6), puis reprendre par `CONTINUER` (paragraphe 8.4).

**ALLER EN.** `ALLER EN n` fait continuer l'exécution au début de la ligne `n` :

```text
40 SI P#N ALORS ALLER EN 30
```

Le numéro peut être une expression ; il est arrondi à l'entier le plus proche. On peut ainsi choisir la ligne selon une valeur, comme dans un menu :

```text
40 LIRE R;ALLER EN 50+50*R
```

va en ligne 100, 150, 200 ou 250 selon que `R` vaut 1, 2, 3 ou 4. La ligne doit exister (`ERREUR (EX.) 75` sinon).

> **Sous `-T1600` (et LSE F4)** le symbole `@` vaut, dans une expression, le numéro de la ligne en cours : `10 ALLER EN @+3` va en ligne 13.
>
> **Sous `-MITRA15`** le symbole `@` marque au contraire un numéro de ligne rangé dans une variable : `10 I←@50` puis `20 ALLER EN I` va en ligne 50.

### 5.12 L'instruction SI

L'instruction conditionnelle a deux formes :

```text
SI condition ALORS instruction
SI condition ALORS instruction1 SINON instruction2
```

Si la condition est vraie, l'instruction qui suit `ALORS` est exécutée ; sinon, celle qui suit `SINON`, s'il y en a une. Dans tous les cas, l'exécution continue ensuite avec l'instruction suivante (après un point-virgule, ou sur la ligne suivante).

```text
SI I>10 ALORS ALLER EN 24
SI I=J ALORS A[I,I]←1 SINON A[I,J]←N↑2
SI SCH(CH,I,1)='.' ALORS NBPT←NBPT+1
```

Après `ALORS` ou `SINON` vient une seule instruction ; pour en mettre plusieurs, on les groupe par `DEBUT … FIN` (paragraphe 5.13). Quand deux `SI` sont imbriqués, chaque `SINON` se rapporte au `SI` le plus proche qui n'a pas encore le sien :

```text
SI A=B ALORS SI X=0 ALORS I1 SINON I2
```

Ici, `I2` est exécuté si `A=B` et `X#0` ; si `A#B`, rien n'est exécuté.

### 5.13 Le bloc DEBUT … FIN

`DEBUT` et `FIN` encadrent une suite d'instructions séparées par des points-virgules, qui compte alors pour une seule instruction :

```text
SI I#J ALORS DEBUT A[I]←I;B[J]←J FIN SINON DEBUT A[I]←0;B[J]←0 FIN
40 SI P<N ALORS DEBUT AFFICHER 'TROP PETIT.';ALLER EN 30 FIN
```

Un bloc tient sur une seule ligne.

### 5.14 Les boucles FAIRE

L'instruction `FAIRE` répète une partie du programme, la **boucle**, en faisant varier une variable :

```text
FAIRE n POUR V←début [PAS p] JUSQUA fin
FAIRE n POUR V←début [PAS p] TANT QUE condition
```

- `n` est le numéro de la **dernière ligne de la boucle** : la boucle va de l'instruction qui suit `FAIRE` jusqu'à la fin de la ligne `n` comprise ;
- `V` est la **variable de boucle**, une variable numérique simple ;
- `début` est sa valeur de départ ;
- `p` est le **pas** ajouté à `V` à chaque tour ; sans `PAS`, il vaut 1 ;
- avec `JUSQUA`, la boucle s'arrête quand `V` dépasse `fin` (quand `V` devient plus petit que `fin` si le pas est négatif) ;
- avec `TANT QUE`, la boucle continue tant que la condition est vraie.

```text
10 FAIRE 30 POUR I←1 JUSQUA 5
20 S←S+I
30 AFFICHER[U] I
40 AFFICHER[/,'FINI']
```

**Déroulement exact.** `V` reçoit sa valeur de départ, puis le test est fait **avant** chaque tour, y compris le premier : une boucle `FAIRE 30 POUR I←1 JUSQUA 0` n'est pas exécutée du tout. Après chaque tour, le pas est ajouté à `V`, puis le test refait. La fin et le pas sont recalculés à chaque tour. À la sortie, `V` garde la valeur qui a arrêté la boucle : après `FAIRE 30 POUR I←1 JUSQUA 5`, `I` vaut 6.

**Boucle sur une seule ligne.** Si `n` est le numéro de la ligne du `FAIRE` lui-même, la boucle se compose des instructions qui suivent `FAIRE` sur la même ligne :

```text
200 FAIRE 200 POUR K←N PAS -1 JUSQUA 1;A[K]←0
```

**La fin de la boucle doit exister.** Si la ligne `n` n'existe pas, la boucle s'arrête à la ligne précédente la plus proche. Pour sauter directement à la fin de la boucle, faites-la finir par une ligne vide :

```text
100 FAIRE 150 POUR I←1 JUSQUA N
110 SI T[I]=0 ALORS ALLER EN 150
120 S←S+1/T[I]
150 
```

**Boucles imbriquées.** Une boucle peut en contenir une autre, à condition que la boucle intérieure soit tout entière dans l'extérieure. Deux boucles peuvent finir sur la même ligne :

```text
100 FAIRE 104 POUR I←1 JUSQUA N
102 FAIRE 104 POUR J←1 JUSQUA N
104 A[I,J]←0
```

**Sortir avant la fin.** Un `ALLER EN` vers une ligne hors de la boucle fait sortir de la boucle ; la variable garde sa valeur du moment. Revenir par `ALLER EN` sur la ligne du `FAIRE` recommence la boucle depuis le début.

**FAIRE sans variable (profil LSE F4).** Sous LSE F4, la boucle `TANT QUE` peut se passer de variable :

```text
10 X←1;FAIRE 10 TANT QUE X<100;X←X*2
```

double `X` tant qu'il est inférieur à 100 (il vaut 128 à la sortie). La condition est testée avant chaque tour. On s'en sert aussi pour attendre :

```text
100 T←TEM();FAIRE 100 TANT QUE TEM()-T<5
```

attend cinq secondes.

> **Sous `-T1600` et `-MITRA15`** `POUR` est obligatoire : on écrit `FAIRE 10 POUR K←0 TANT QUE X<100`.

### 5.15 Lire des données : LIRE

`LIRE` attend que l'on tape des valeurs au clavier et les range dans des variables :

```text
LIRE A
LIRE EPS,TAB[1],NOM,RHO
LIRE NOTE
```

Chaque variable de la liste reçoit une valeur, dans l'ordre. Une variable déclarée `CHAINE` reçoit une chaîne ; toute autre reçoit un nombre ; un **nom de tableau** seul fait lire tous ses éléments, ligne par ligne (`T[1,1]`, `T[1,2]`, …, `T[2,1]`…).

**Le bip.** `LIRE` n'affiche aucune question : il émet seulement un bip. Faites donc précéder chaque `LIRE` d'un `AFFICHER` qui dit ce que l'on attend :

```text
30 AFFICHER 'TA PROPOSITION ? ';LIRE P
```

**Taper des nombres.** Un nombre s'écrit comme dans un programme (paragraphe 5.3), avec un signe facultatif. Plusieurs nombres se séparent par des espaces, sur une ou plusieurs lignes ; une ligne se termine par ‹X-OFF› ou ‹RETURN›. Pour `LIRE A,B,C`, on peut taper `1 2 3` ‹X-OFF›, ou `1` ‹X-OFF› `2 3` ‹X-OFF›.

**Taper une chaîne.** Une chaîne prend **tout le reste de la ligne**, espaces compris, jusqu'à la fin de la saisie. Terminez-la par ‹X-OFF› :

| Vous tapez | La chaîne reçoit |
|---|---|
| `PIERRE` ‹X-OFF› | `PIERRE` (6 caractères) |
| `PIERRE` ‹RETURN› | `PIERRE` suivi d'un passage à la ligne (8 caractères) |
| ‹X-OFF› seul | la chaîne vide |

Avec ‹RETURN›, la chaîne contient en plus un passage à la ligne : c'est voulu (on peut ainsi lire un texte de plusieurs lignes, morceau par morceau), mais c'est rarement ce que l'on cherche. Sous le profil LSE F4, la touche DELETE est la plus simple pour terminer une réponse.

**Le reste de la ligne.** Ce qui reste sur la ligne tapée après la dernière valeur demandée est gardé pour le `LIRE` suivant. Avec `10 LIRE A;LIRE B`, taper `1 2` ‹X-OFF› donne `A=1` et `B=2` (un second bip est émis, mais rien n'est redemandé).

**Réponse incorrecte.** Si l'on tape autre chose qu'un nombre là où un nombre est attendu, LSE émet un bip, affiche ` ? `, abandonne le reste de la ligne, et attend de nouveau cette valeur :

```text
COMBIEN ? DOUZE ? 12
```

**Interrompre une lecture.** ‹ESC› pendant une lecture arrête le programme avec `PRET EN LIGNE nnn`. `CONTINUER` reprend la lecture où elle en était : les valeurs déjà tapées sont gardées, et un bip annonce la valeur attendue.

**Nombre trop grand.** Un nombre tapé trop grand prend la plus grande valeur possible, avec son signe.

### 5.16 Afficher des résultats : AFFICHER

`AFFICHER` écrit des valeurs à l'écran. Il a deux formes : sans format, et avec un format entre crochets.

#### Sans format

```text
AFFICHER 'BONJOUR'
AFFICHER A,B,A+B
AFFICHER 'RESULTAT : ',X
```

LSE passe d'abord à la ligne, puis écrit chaque valeur : un nombre est suivi d'un espace, une chaîne est écrite telle quelle. `AFFICHER 1,'DEUX',3` écrit `1 DEUX3 `. Le mot `AFFICHER` doit être suivi d'un espace.

#### Avec un format

Le format se place entre crochets, collé au mot `AFFICHER`, avant la liste des valeurs. Il décrit, de gauche à droite, ce qu'il faut écrire. Il n'y a alors **aucun passage à la ligne automatique** : c'est le format qui les commande. Le format contient trois sortes d'éléments, séparés par des virgules.

**Les libellés** sont des textes entre apostrophes, écrits tels quels :

```text
AFFICHER['SOMME = '] 
AFFICHER[5'*','TRACE',5'*']
```

La seconde écrit `*****TRACE*****` : un nombre devant un élément le **répète**.

**La mise en page** :

| Élément | Effet |
|---|---|
| `/` | passer au début de la ligne suivante |
| `X` | laisser un espace |
| `C` | revenir au début de la ligne en cours, sans changer de ligne ; ce qui suit s'écrit par-dessus |
| `L` | passer à la ligne suivante (sur l'écran, comme `/`) |

`AFFICHER[3/]` saute trois lignes ; `AFFICHER[/,25X,'RESULTATS']` passe à la ligne, laisse 25 espaces, puis écrit `RESULTATS`.

**Les descriptions** disent comment écrire un nombre :

| Description | Effet |
|---|---|
| `U` | forme universelle : celle de l'affichage sans format (nombre suivi d'un espace, chaîne telle quelle) |
| `Fe.d` | forme décimale, avec `d` chiffres après le point ; `e` est le nombre de positions avant le point, complétées par des espaces à gauche |
| `Ee.d` | forme à exposant : un chiffre, `d` décimales, puis `E`, le signe et deux chiffres d'exposant ; `e` positions avant le point |

Le nombre est arrondi au dernier chiffre écrit. S'il faut plus de `e` positions, elles sont prises quand même. `F0.0` écrit un nombre entier sans point. Voici quelques nombres écrits sous trois descriptions (`|` marque les limites) :

| Nombre | `E3.4` | `F2.1` | `U` |
|---|---|---|---|
| 1692 | `\|  1.6920E+03\|` | `\|1692.0\|` | `\|1692 \|` |
| −34,227 | `\| -3.4227E+01\|` | `\|-34.2\|` | `\|-34.227 \|` |
| −2,86 | `\| -2.8600E+00\|` | `\|-2.9\|` | `\|-2.86 \|` |
| 0,055 | `\|  5.5000E-02\|` | `\| 0.1\|` | `\|0.055 \|` |
| −0,0001 | `\| -1.0000E-04\|` | `\|-0.0\|` | `\|-1E-04 \|` |
| 10 400 600 | `\|  1.0401E+07\|` | `\|10400600.0\|` | `\|1.04006E+07 \|` |

La forme `U` écrit les nombres sans exposant entre 0,001 et 999 999,9 ; au-delà, avec un exposant.

**Correspondance entre descriptions et valeurs.** Le format et la liste de valeurs sont parcourus ensemble, de gauche à droite. Chaque valeur prend la description suivante du format ; les libellés et la mise en page sont écrits au passage :

```text
AFFICHER[/,'N=',F3.0,X,'S=',U] 10,55
```

écrit, à la ligne, `N= 10 S=55 `. Une valeur qui n'a plus de description est écrite en `U`. Il ne doit pas y avoir plus de descriptions que de valeurs (`ERREUR  C 54`). Un nombre devant une description la répète : `[3F5.1]` vaut `[F5.1,F5.1,F5.1]`. Les chaînes s'écrivent seulement en `U` (`ERREUR (EX.) 06` sous `F` ou `E`).

**Facteur variable.** Une étoile `*` à la place du nombre de répétitions, devant un libellé ou une mise en page, prend ce nombre dans la liste des valeurs. Cette valeur n'est pas affichée :

```text
AFFICHER[/,*X,F0.3] 12,5.7
```

écrit douze espaces, puis `5.700`. Si la valeur vaut 0, rien n'est écrit ; si elle est négative, c'est une erreur (`ERREUR (EX.) 104`).

**Afficher un tableau entier.** Un nom de tableau dans la liste fait écrire tous ses éléments, avec la même description, une ligne de tableau par ligne d'écran :

```text
AFFICHER[/,'LES NOTES :',F6.1] NOTE
```

Le tableau commence sur une ligne neuve, et un passage à la ligne suit sa dernière ligne. Un élément non défini provoque une erreur (`ERREUR (EX.) 32`).

**Répétitions.** Le nombre de répétitions va de 1 à 62.

> **Sous `-MITRA15`** le nombre de répétitions va de 1 à 99.

**Lignes longues.** LSE ne coupe jamais une ligne trop longue : l'écran de 80 colonnes la continue sur la ligne suivante.

**Caractères de commande.** Les caractères de code 0 à 31, écrits par un programme (par `EQC` ou par `.n.`), agissent sur l'écran de l'Apple IIGS : le code 7 fait biper, le code 12 efface l'écran. L'annexe C en donne la liste. Les codes 17 à 21 ne sont jamais envoyés à l'écran : ils servent au ruban (paragraphe 12.10).

### 5.17 Les fonctions numériques

| Fonction | Résultat |
|---|---|
| `SIN(X)` | sinus de X (X en radians) |
| `COS(X)` | cosinus de X (X en radians) |
| `ATG(X)` | arc tangente de X, entre −π/2 et π/2 |
| `LGN(X)` | logarithme népérien de X (X > 0) |
| `EXP(X)` | exponentielle de X |
| `RAC(X)` | racine carrée de X (X ≥ 0) |
| `ABS(X)` | valeur absolue de X |
| `ENT(X)` | partie entière : le plus grand entier inférieur ou égal à X ; `ENT(3.7)` vaut 3, `ENT(-3.2)` vaut −4 |
| `ALE(X)` | nombre pseudo-aléatoire entre 0 et 1, bornes exclues (voir ci-dessous) |
| `SH(X)`, `CH(X)`, `TH(X)` | sinus, cosinus et tangente hyperboliques |
| `SGN(X)` | signe de X : −1, 0 ou 1 |

Le nom de la fonction est suivi de son argument entre parenthèses ; l'argument peut être une expression : `RAC(SIN(X)↑2+COS(Y)↑2)`. Pour π, écrivez `4*ATG(1)`. Pour arrondir X à l'entier le plus proche : `ENT(X+0.5)`. Pour le reste de la division de A par B : `A-B*ENT(A/B)`.

**Le hasard : ALE.**

- `ALE(0)` donne à chaque appel un nombre nouveau, imprévisible : c'est l'usage ordinaire. Pour un entier au hasard de 1 à 6 : `ENT(ALE(0)*6)+1`.
- `ALE(X)`, avec X entre 0 et 1, donne un nombre qui ne dépend que de X. En réutilisant chaque résultat comme argument suivant (`X←ALE(X)`), on obtient une suite qui se reproduit à l'identique d'une exécution à l'autre, ce qui est utile pour mettre un programme au point.
- Un argument négatif ou supérieur à 1 est une erreur (`ERREUR (EX.) 09`).

> **Sous `-MITRA15`** les fonctions `SH`, `CH`, `TH` et `SGN` n'existent pas.

### 5.18 Les fonctions sur les chaînes

Dans ce tableau, `C` est une chaîne, `P` une position de départ (de 1 à la longueur + 1), et `V` une variable numérique facultative qui reçoit une position. Les positions et longueurs non entières sont arrondies.

| Fonction | Résultat |
|---|---|
| `LGR(C)` | longueur de C : `LGR('BONJOUR')` vaut 7 |
| `SCH(C,P,N)` | sous-chaîne de C commençant en P, de N caractères : `SCH('BONJOUR',4,4)` vaut `'JOUR'` |
| `SCH(C,P,S)` | sous-chaîne de C commençant en P, arrêtée au premier caractère de la chaîne S (exclu) : `SCH(' ABC.D',2,'.')` vaut `'ABC'` |
| `SCH(C,P,N ou S,V)` | même chose, et V reçoit la position du premier caractère non pris |
| `GRL(C,P)`, `GRL(C,P,V)` | le premier groupe de lettres de C à partir de P (les autres caractères sont sautés) : `GRL('  LE CHAT',1)` vaut `'LE'` ; V reçoit la position qui suit le groupe |
| `POS(C,P,S)` | position de la première apparition de S dans C, à partir de P ; 0 si absente : `POS('ABRACADABRA',1,'CA')` vaut 5 |
| `PTR(C,P,N ou S)` | la position que `SCH` mettrait dans V ; `PTR(C,P)` : celle que `GRL` mettrait dans V |
| `SKP(C,P,S)` | position du premier caractère, à partir de P, qui n'est pas dans S : `SKP('   XYZ',1,' ')` vaut 4 |
| `SKP(C,P)` | position de la première lettre à partir de P : `SKP('423 AB',1)` vaut 5 |
| `CCA(X)` | le nombre X écrit en chaîne, comme par la description `U`, sans l'espace final : `CCA(3+0.125)` vaut `'3.125'` |
| `CNB(C,P)`, `CNB(C,P,V)` | le nombre écrit dans C à partir de P (les espaces en tête sont sautés) ; 0 s'il n'y en a pas ; V reçoit la position du caractère qui a arrêté la lecture : `CNB('12 OU 14',7)` vaut 14 |
| `EQC(X)` | la chaîne d'un caractère de code X (annexe C) : `EQC(65)` vaut `'A'` |
| `EQN(C,P)`, `EQN(C)` | le code du caractère de C en position P (1 si P est omis) : `EQN('A')` vaut 65 ; −1 si P vaut la longueur + 1 |

Quand une recherche atteint la fin de la chaîne sans trouver, `SCH` et `GRL` s'arrêtent à la fin, et `SKP` rend la longueur + 1.

**Exemple : couper une phrase en mots.**

```text
10 CHAINE PH,M;PH←'LE CHAT DORT SUR LE TOIT';P←1
20 M←GRL(PH,P,P);SI M='' ALORS TERMINER
30 AFFICHER M;ALLER EN 20
```

`GRL` rend le mot suivant et range dans `P` la position où chercher le mot d'après.

> **Sous `-MITRA15`** `EQC` accepte les codes de 0 à 255 (de 0 à 127 sinon). Voir l'annexe C pour l'affichage des codes 128 à 255.

### 5.19 Date et heure

| Fonction | Résultat |
|---|---|
| `TEM` ou `TEM()` | nombre de secondes écoulées depuis minuit, par exemple 52 245 à 14 h 30 min 45 s |
| `DAT` ou `DAT()` | la date et l'heure, en une chaîne de 17 caractères `jj/mm/aa hh:mm:ss`, par exemple `'06/11/26 14:30:45'` |

`TEM` sert à mesurer une durée, `DAT` à dater un résultat. Avec une date donnée au démarrage (paragraphe 2.2), l'heure ne repasse pas à zéro à minuit, et `TEM` peut dépasser 86 400.

> **Sous `-T1600`** on écrit `TEM` et `DAT`, sans parenthèses. Sous LSE F4, les deux formes sont admises, et `LISTER` les réécrit avec parenthèses.
>
> **Sous `-MITRA15`** on écrit `TEM()` et `DAT()`, avec parenthèses ; `TEM` seul serait un nom de variable.

### 5.20 Les procédures

Une **procédure** est un morceau de programme qui porte un nom et que l'on peut appeler de plusieurs endroits, avec des valeurs différentes. Il y en a deux sortes :

- la **procédure-fonction** calcule une valeur, qu'on utilise dans une expression, comme une fonction de LSE : `Y←&CARRE(3)` ;
- la **procédure-sous-programme** fait un travail, sans rendre de valeur ; on l'appelle comme une instruction : `&TRACE(10)`.

#### Déclaration

Une procédure commence par une ligne de **déclaration**, qui donne son nom et ses **paramètres** :

```text
100 PROCEDURE &CARRE(X);RESULTAT X*X
```

- La déclaration est toujours **en tête de ligne**, juste après le numéro.
- Le nom commence par `&`, suivi d'une lettre et d'au plus quatre lettres ou chiffres.
- Les parenthèses sont obligatoires, même sans paramètre : `PROCEDURE &BIP()`.
- Le corps de la procédure suit, sur la même ligne après un point-virgule, ou sur les lignes suivantes.

On ne passe pas dans une procédure en déroulant le programme : arriver sur une ligne de déclaration est une erreur (`ERREUR (EX.) 12`). On place donc les procédures **après** le `TERMINER` du programme principal :

```text
10 AFFICHER[U] &CARRE(4),&CARRE(5)
20 TERMINER
100 PROCEDURE &CARRE(X);RESULTAT X*X
```

#### Sortir d'une procédure

- `RESULTAT expression` sort d'une procédure-fonction en donnant sa valeur ;
- `RETOUR` sort d'une procédure-sous-programme ;
- `RETOUR EN n` sort et continue en ligne `n` du programme appelant, au lieu de revenir après l'appel. Si la procédure avait été appelée dans une expression, l'instruction qui l'appelait est abandonnée.

Une procédure peut avoir plusieurs `RETOUR` ou `RESULTAT` ; le premier rencontré la termine. `RETOUR` dans une procédure-fonction, ou `RESULTAT` dans une procédure-sous-programme, est une erreur (`ERREUR (EX.) 15` et `22`).

#### Paramètres

Dans la déclaration, les **paramètres formels** sont des noms ; à l'appel, les **paramètres effectifs** donnent les valeurs. Il doit y en avoir autant, dans le même ordre (`ERREUR (EX.) 13` sinon).

Un paramètre peut être de deux sortes :

- **local**, s'il figure dans la liste `LOCAL` de la déclaration : la procédure reçoit une **copie de la valeur** ; elle peut la modifier sans toucher à la variable de l'appelant. Le paramètre effectif peut être n'importe quelle expression.
- **référence**, sinon : la procédure travaille **directement sur la variable** de l'appelant, et peut la modifier. Le paramètre effectif doit être un nom de variable, de chaîne ou de tableau, pas une expression (`ERREUR (EX.) 72`).

```text
200 PROCEDURE &RAC2(A,B,C,D,X1,X2) LOCAL A,B,C
210 D←B↑2-4*A*C;SI D<0 ALORS RETOUR
220 X1←(-B+RAC(D))/(2*A);X2←(-B-RAC(D))/(2*A);RETOUR
```

Après `&RAC2(1,-1,-6,DISC,R1,R2)`, `DISC` vaut 25, `R1` vaut 3 et `R2` vaut −2 : `D`, `X1` et `X2` sont des références, `A`, `B` et `C` des paramètres locaux.

En règle générale, mettez dans `LOCAL` tous les paramètres dont seule la valeur compte. On passe un **tableau** de préférence par référence : c'est plus rapide et cela permet de le modifier. Il ne faut pas le redéclarer dans la procédure.

#### Variables locales et variables globales

La liste `LOCAL` peut aussi contenir des noms qui ne sont pas des paramètres : ce sont des **variables locales**, propres à la procédure. Elles n'ont aucun lien avec les variables de même nom de l'appelant, et disparaissent au retour. Un nom employé dans une procédure sans figurer dans sa déclaration est **global** : c'est la variable du programme appelant, que la procédure lit et modifie. Une variable globale créée par la procédure existe encore après son retour.

```text
100 PROCEDURE &PDIAG(A,N) LOCAL N,I,PROD
110 PROD←1;FAIRE 110 POUR I←1 JUSQUA N;PROD←PROD*A[I,I]
120 RESULTAT PROD
```

Grâce à `LOCAL I`, la procédure ne modifie pas une éventuelle variable `I` du programme appelant. Déclarez locales toutes les variables de travail d'une procédure : c'est la meilleure protection contre les mauvaises surprises.

#### Récursivité

Une procédure peut s'appeler elle-même. La factorielle, n! = n × (n−1)!, avec 0! = 1 :

```text
10 AFFICHER[/,'7! = ',U] &FACT(7)
20 TERMINER
30 PROCEDURE &FACT(N) LOCAL N
40 RESULTAT SI N=0 ALORS 1 SINON N*&FACT(N-1)
```

affiche `7! = 5040`. Chaque appel a sa propre copie des paramètres et variables locales. L'annexe G donne un exemple plus complet (les tours de Hanoï).

#### Procédure passée en paramètre

Un paramètre formel peut être un nom de procédure (commençant par `&`) ; on transmet alors une procédure à une autre :

```text
10 &TABLE(&CARRE,4);&TABLE(&CUBE,3);TERMINER
100 PROCEDURE &CARRE(X);RESULTAT X*X
110 PROCEDURE &CUBE(X);RESULTAT X*X*X
120 PROCEDURE &TABLE(&F,N) LOCAL N,I
130 FAIRE 130 POUR I←1 JUSQUA N;AFFICHER[U] &F(I)
140 AFFICHER[/];RETOUR
```

écrit `1 4 9 16` sur une ligne, puis `1 8 27` sur la suivante.

**Procédures et boucles.** Une procédure appelée dans une boucle `FAIRE` fait partie de la boucle : son corps est exécuté à chaque tour, à l'endroit de l'appel.

> **Sous `-T1600` (et LSE F4)** le symbole `$`, dans une procédure, vaut le numéro de la ligne d'où elle a été appelée : `RETOUR EN $+1` revient à la ligne qui suit celle de l'appel.

### 5.21 Les fichiers dans un programme

Quatre instructions permettent à un programme de garder des données sur le disque et d'appeler un autre programme. Elles sont décrites au chapitre 10 :

| Instruction | Effet |
|---|---|
| `GARER V,N,F` | range la variable, chaîne ou tableau `V` dans l'enregistrement `N` du fichier de nom `F` |
| `CHARGER V,N,F[,R]` | rappelle dans `V` l'enregistrement `N` du fichier `F` ; `R` reçoit un compte rendu |
| `SUPPRIMER F[,N]` | supprime l'enregistrement `N` du fichier `F`, ou tout le fichier (MITRA 15 seulement) |
| `EXECUTER P[,N]` | remplace le programme par le programme rangé `P` et l'exécute à partir de la ligne `N` (1 par défaut) |

### 5.22 Les commentaires

Un **commentaire** commence par une étoile `*` et va jusqu'à la fin de la ligne. Il se place juste après le numéro de ligne (l'espace est alors facultatif), ou après un point-virgule :

```text
1*JEU DU NOMBRE CACHE
30 S←S+I;*CUMUL DES VALEURS
```

Un commentaire n'a aucun effet sur l'exécution. Il peut contenir des minuscules et tous les caractères. Il est conservé tel quel, espaces compris.

### 5.23 Instructions propres au T1600

Sous les profils LSE F4 et `-T1600`, deux instructions mettent tous les éléments d'un tableau à une même valeur :

| Instruction | Effet |
|---|---|
| `UN T` | met tous les éléments du tableau `T` à 1 |
| `ZER T` | met tous les éléments du tableau `T` à 0 |

```text
10 TABLEAU U[10],T[5,8];UN U;ZER T
```

> **Sous `-MITRA15`** ces instructions n'existent pas ; on écrit une boucle `FAIRE`.

---

## 6. Le mode machine de bureau

### 6.1 Principe

Une ligne tapée **sans numéro** n'est pas gardée dans le programme : elle est compilée et exécutée tout de suite, puis oubliée. C'est le **mode machine de bureau** : l'ordinateur sert de calculatrice, et permet d'agir sur le programme en mémoire.

```text
A←7;B←A*6
?A,B
7 42
```

Les variables créées en mode machine de bureau restent en mémoire, avec celles du programme.

### 6.2 Le point d'interrogation

En mode machine de bureau, `?` remplace `AFFICHER`, avec exactement les mêmes possibilités, formats compris :

```text
?2+3
5
?RAC(2)*RAC(2)
2
?[/,F8.3] 4*ATG(1)
       3.142
?'LE CARRE DE 12 EST '!CCA(12↑2)
LE CARRE DE 12 EST 144
?SCH('BONJOUR',4,4)
JOUR
?DAT
06/11/26 14:30:45
```

Avec un format, pensez à commencer par `/` : une sortie avec format ne passe pas à la ligne d'elle-même, et s'écrirait à la suite de ce que vous venez de taper.

`?` n'existe qu'en mode machine de bureau : dans une ligne de programme, il est refusé (`ERREUR  C 106`).

### 6.3 Ce qui est permis, ce qui ne l'est pas

Une ligne de machine de bureau peut contenir plusieurs instructions séparées par des points-virgules, et toutes les instructions de calcul, de lecture et d'affichage. Sont **interdites**, avec `ERREUR  C 58` :

- les instructions qui désignent un numéro de ligne : `ALLER EN`, `FAIRE`, `RETOUR EN` ;
- `RETOUR` et `RESULTAT`, qui n'ont de sens que dans une procédure ;
- les appels de procédures, en instruction comme dans une expression ;
- `EXECUTER`.

Sont **permises** : l'affectation, `SI … ALORS … SINON`, `DEBUT … FIN`, `AFFICHER` et `?`, `LIRE`, `CHAINE`, `TABLEAU`, `LIBERER`, `GARER`, `CHARGER`, `PAUSE`, `TERMINER`.

> **Sous `-MITRA15`** les déclarations `CHAINE`, `TABLEAU` et `LIBERER` sont elles aussi interdites en mode machine de bureau.

### 6.4 Examiner et modifier un programme arrêté

Quand un programme est arrêté (par `PAUSE`, par ‹ESC›, par une ligne d'arrêt, en pas à pas, ou après une erreur), ses variables restent en mémoire. Le mode machine de bureau permet de les consulter, et même de les changer avant de continuer :

```text
EXECUTER A PARTIR DE 10
PAUSE EN LIGNE 040
?I,S
2 3
S←100
CONTINUER
```

Le programme repart avec `S` valant 100. Si le programme est arrêté à l'intérieur d'une procédure, ce sont les variables vues par la procédure (ses variables locales comprises) qui sont consultées. Une ligne de machine de bureau ne change pas l'état du programme : `CONTINUER` reste possible après elle.

### 6.5 Lire, garer, charger au bureau

`LIRE`, `GARER` et `CHARGER` fonctionnent aussi en mode machine de bureau, ce qui permet par exemple de regarder le contenu d'un fichier sans écrire de programme :

```text
CHARGER T,2,'MESUR';?[F8.2] T
```

### 6.6 Les erreurs au bureau

Une erreur de syntaxe est signalée comme pour une ligne de programme (`ERREUR  C nn`). Une erreur pendant l'exécution de la ligne est signalée avec `BUR` à la place du numéro de ligne :

```text
?1/0
ERREUR (EX.) 31 EN LIGNE BUR
```

Elle n'a aucun effet sur le programme en mémoire.

---

# Partie III — Travailler avec ses programmes

## 7. L'éditeur : écrire et corriger un programme

LSE n'a pas d'éditeur plein écran : on y travaille ligne par ligne, comme sur une télétype. On entre une ligne en la tapant avec son numéro, on la remplace en la retapant, on la supprime par `EFFACER`, on la corrige sans la retaper par l'éditeur de ligne. Un programme long peut aussi être préparé avec l'éditeur d'ORCA, puis lu par LSE comme un ruban (paragraphe 7.7).

### 7.1 Entrer une ligne

Tapez la ligne, numéro compris, puis ‹X-OFF›. LSE la vérifie aussitôt :

- si elle est correcte, elle entre dans le programme, sans message ;
- sinon, LSE affiche un message comme `ERREUR  C 38` et la ligne **n'entre pas** dans le programme. Une ancienne ligne de même numéro, s'il y en avait une, reste inchangée.

Une ligne refusée se corrige en la retapant entièrement, ou plus vite par l'éditeur de ligne (paragraphe 7.5). Avec l'option `-LIBELLES`, le message explique la faute : `ERREUR  C 38 : INSTRUCTION INCONNUE OU ← MANQUANT`.

### 7.2 Remplacer, insérer, supprimer

- **Remplacer** une ligne : retapez-la avec le même numéro.
- **Insérer** une ligne : donnez-lui un numéro compris entre ceux de ses voisines.
- **Supprimer** des lignes : `EFFACER LIGNES` (`EF`), suivi des numéros. On peut donner une liste de numéros et d'intervalles séparés par des virgules :

```text
EFFACER LIGNES 25
EFFACER LIGNES 7,12 A 31,40
EFFACER LIGNES *
```

La deuxième commande efface les lignes 7, 12 à 31 et 40. `*` efface **tout le programme**, ainsi que ses variables. Un numéro qui ne correspond à aucune ligne n'est pas une erreur.

### 7.3 Bien numéroter

LSE n'a pas de commande pour renuméroter un programme. Prenez donc l'habitude de laisser de la place :

- numérotez de 10 en 10 ;
- donnez des numéros ronds aux parties du programme (100, 150, 200…) et aux procédures ;
- gardez les petits numéros (1 à 9) pour des commentaires de titre.

Sous LSE F4 et T1600, les numéros vont jusqu'à 255 ; sous MITRA 15, jusqu'à 250.

### 7.4 Voir le programme : LISTER et NUMERO

`LISTER A PARTIR DE` (`LI`) écrit les lignes du programme à partir d'un numéro, jusqu'à la fin ou jusqu'à un second numéro :

```text
LISTER A PARTIR DE 1
LISTER A PARTIR DE 100,180
LISTER A PARTIR DE 30,30
```

La dernière commande liste la seule ligne 30. `NUMERO A PARTIR DE` (`NU`) n'écrit que les numéros des lignes, dix par ligne d'écran :

```text
NUMERO A PARTIR DE 1
       1      10      20      25      30      40     100     110     120     130
     140     150
```

‹ESC› arrête un listage en cours, avec `PRET`.

### 7.5 Corriger une ligne sans la retaper : l'éditeur de ligne

L'éditeur de ligne remplace un groupe de caractères par un autre dans une ligne, sans la retaper. Il agit sur la **ligne visée**, qui est la plus récente de ces deux lignes :

- la dernière ligne **refusée** par LSE, telle que vous l'aviez tapée ;
- la dernière ligne **écrite** par `LISTER` ou `PERFORER`.

Une correction s'écrit :

```text
/groupe/remplacement
```

Le premier caractère `/` indique une correction. Le groupe est le texte à chercher, le remplacement est le texte à mettre à sa place, et il n'y a pas de `/` final : tout ce qui suit le second `/` est le remplacement, espaces compris. Exemple, sur une ligne refusée :

```text
10 SI X=1 ALORS AFICHER 'UN'
ERREUR  C 03
/AFI/AFFI
10 SI X=1 ALORS AFFICHER 'UN'
```

LSE écrit la ligne corrigée, puis la traite exactement comme si vous l'aviez tapée : correcte, elle entre dans le programme ; refusée, elle devient la nouvelle ligne visée, et l'on peut la corriger encore.

**Formes complètes.** Devant le `/`, on peut indiquer combien d'occurrences remplacer, et à partir de laquelle :

| Forme | Effet |
|---|---|
| `/G/R` | remplace la première occurrence de G |
| `nX/G/R` | remplace au plus n occurrences de G, à partir de la première |
| `Xp/G/R` | remplace l'occurrence numéro p de G |
| `nXp/G/R` | remplace au plus n occurrences, à partir de l'occurrence numéro p |
| `%G%R` | comme `/G/R`, avec `%` comme séparateur : pour un groupe ou un remplacement qui contient `/` |

Les occurrences se comptent de gauche à droite ; un texte inséré n'est jamais repris dans le compte. Exemple, après `LISTER A PARTIR DE 1,1` :

```text
1 ALLER EN AYAYA
3X2/A/B
1 ALLER EN BYBYB
```

Les occurrences de `A` sont, dans l'ordre, celle d'`ALLER` puis les trois d'`AYAYA` ; `3X2` remplace trois occurrences à partir de la deuxième.

**Cas particuliers.**

- Le remplacement peut être vide : `/ENCORE /` supprime le mot `ENCORE` et l'espace qui le suit.
- Si le groupe n'est pas trouvé, rien ne se passe et rien n'est écrit.
- Un groupe vide, une forme incomplète, ou l'absence de ligne visée (au début d'une session) donnent `????`.
- Si la correction change le **numéro** de la ligne, elle crée une nouvelle ligne et l'ancienne reste : c'est un moyen commode de **recopier** une ligne sous un autre numéro (`/40 /45 `, après avoir listé la ligne 40).

### 7.6 Ce que fait LSE d'une ligne tapée

LSE ne garde pas le texte de vos lignes : il les traduit, dès qu'elles sont tapées, dans une forme compacte qu'il exécute rapidement, et il les **réécrit** à la demande (pour `LISTER`, `PERFORER`, `DECODER`). La ligne réécrite a le même sens, mais pas toujours la même allure :

```text
15   X  ←  3.14159265 + 1.50 ;  Y←TEM ;Z←1000000
LISTER A PARTIR DE 15,15
15 X←3.14159+1.5;Y←TEM();Z←1E+06
```

- les espaces inutiles disparaissent ;
- les nombres sont réécrits avec au plus six chiffres significatifs ;
- sous LSE F4, `TEM` et `DAT` sont réécrits `TEM()` et `DAT()` ;
- les caractères non imprimables d'une chaîne sont réécrits sous la forme `.n.` (profil LSE F4) ;
- les commentaires sont gardés tels quels.

C'est ce qu'on appelle la **compilation ligne par ligne** : chaque ligne est vérifiée seule, sans regarder les autres. LSE ne peut donc pas savoir, en compilant une ligne, si une variable sera une chaîne ou un tableau : ces erreurs-là n'apparaissent qu'à l'exécution.

### 7.7 Écrire un programme avec l'éditeur d'ORCA

Pour un long programme, ou pour retoucher beaucoup de lignes, on peut se servir de l'éditeur plein écran d'ORCA, puis faire lire le texte par LSE. Le fichier joue alors le rôle d'un ruban perforé (chapitre 12).

1. Dans le shell, choisissez le format « texte ProDOS », puis ouvrez un nouveau fichier :

   ```text
   PRODOS
   EDIT :DUR:PROGS:DEVIN
   ```

2. Tapez le programme, une ligne LSE par ligne, avec `_` pour `←` et `^` pour `↑`. N'utilisez pas la touche TAB. Enregistrez et quittez l'éditeur.
3. Lancez LSE en désignant ce fichier comme ruban :

   ```text
   LSE -RUBAN :DUR:PROGS:DEVIN
   ```

4. Ouvrez la session (`BO`), puis frappez ‹X-ON› (Ctrl-Q) sur une ligne vide : LSE lit le fichier comme si vous tapiez chaque ligne, et signale les lignes fautives au passage.

Dans l'autre sens, `PERFORER` écrit le programme en mémoire dans le fichier-ruban (paragraphe 12.3), que l'éditeur d'ORCA peut ouvrir. Un programme passe ainsi de LSE à l'éditeur, et retour. C'est aussi le moyen de **renuméroter** un programme : on le perfore, on change les numéros dans l'éditeur, on efface le programme (`EFFACER LIGNES *`) et on relit le ruban. Pensez alors à corriger aussi les numéros cités par les instructions `ALLER EN`, `FAIRE` et `RETOUR EN`.

### 7.8 ABREGER et IN EXTENSO

Après `ABREGER` (`AB`), LSE ne complète plus les commandes : il n'écrit qu'un espace après les deux lettres d'une commande à paramètres, et passe à la ligne après une commande sans paramètre. Les échanges sont plus courts. `IN EXTENSO` (`IN`) rétablit la complétion.

---

## 8. Exécuter et mettre au point

### 8.1 Lancer l'exécution : EXECUTER A PARTIR DE

```text
EXECUTER A PARTIR DE n1
EXECUTER A PARTIR DE n1,n2
```

`EXECUTER` (`EX`) **efface toutes les variables**, puis exécute le programme à partir de la ligne `n1`, qui doit exister (`????` sinon). Prenez l'habitude de commencer vos programmes par une ligne 1 ou 10 de titre, en commentaire.

Avec un second numéro `n2`, l'exécution s'arrête en arrivant sur la ligne `n2`, **avant** de l'exécuter :

```text
EXECUTER A PARTIR DE 10,30
ARRET EN LIGNE 030
```

On peut alors examiner les variables (chapitre 6), puis continuer.

### 8.2 Les fins d'exécution

Une exécution se termine toujours par l'un de ces messages (paragraphe 4.7) :

| Message | Cause | Suite possible |
|---|---|---|
| `TERMINE EN LIGNE nnn` | instruction `TERMINER` | `EXECUTER`, `REPRENDRE` |
| `PAUSE EN LIGNE nnn` | instruction `PAUSE` | `CONTINUER`, `POURSUIVRE`, `REPRENDRE` |
| `ARRET EN LIGNE nnn` | ligne d'arrêt atteinte | `CONTINUER`, `POURSUIVRE`, `REPRENDRE` |
| `PRET EN LIGNE nnn` | touche ‹ESC› | `CONTINUER`, `POURSUIVRE`, `REPRENDRE` |
| `LIGNE nnn` | mode pas à pas | ‹X-OFF›, `CONTINUER`, `POURSUIVRE`, `REPRENDRE` |
| `ERREUR (EX.) nn EN LIGNE nnn` | erreur d'exécution | `REPRENDRE`, `EXECUTER` |

Quand l'arrêt se produit dans une procédure, le numéro est celui de la ligne de la procédure.

### 8.3 Interrompre et continuer

‹ESC› arrête un programme qui s'exécute (paragraphe 4.4). La commande `CONTINUER` (`CO`) le fait repartir exactement où il s'était arrêté, avec toutes ses variables. C'est utile pour arrêter une boucle qui semble ne pas finir, examiner les variables, puis continuer.

### 8.4 PAUSE

Placez des `PAUSE` aux endroits délicats d'un programme en cours de mise au point :

```text
45 PAUSE
```

À chaque passage, le programme s'arrête avec `PAUSE EN LIGNE 045`. Examinez les variables, corrigez-les au besoin en mode machine de bureau, puis tapez `CONTINUER`. Effacez ces lignes une fois le programme au point.

### 8.5 POURSUIVRE JUSQU'EN

`POURSUIVRE JUSQU'EN n` (`PO`) fait comme `CONTINUER`, mais en fixant une nouvelle ligne d'arrêt `n` :

```text
PAUSE EN LIGNE 040
POURSUIVRE JUSQU'EN 50
ARRET EN LIGNE 050
```

### 8.6 REPRENDRE

`REPRENDRE A PARTIR DE n1[,n2]` (`RE`) relance l'exécution à la ligne `n1`, **sans effacer les variables**. Elle sert surtout après une erreur ou un `TERMINER`, quand `CONTINUER` n'est plus possible :

```text
EXECUTER A PARTIR DE 10
ERREUR (EX.) 31 EN LIGNE 050
?D
0
D←2
REPRENDRE A PARTIR DE 50
```

La reprise se fait toujours au niveau du programme principal : si l'arrêt avait eu lieu dans une procédure, celle-ci est abandonnée. Comme pour `EXECUTER`, `n2` est une ligne d'arrêt facultative.

> **Sous `-MITRA15`** la commande s'écrit `REPRENDRE EN n1[,n2]`.

### 8.7 Pas à pas

`PAS A PAS` (`PA`) fait s'arrêter le programme au début de **chaque ligne**, et à chaque nouveau tour d'une boucle `FAIRE`, sur la ligne du `FAIRE`. LSE affiche `LIGNE nnn`, la ligne n'étant pas encore exécutée. On peut alors :

- frapper ‹X-OFF› sur une ligne vide : la ligne est exécutée, et l'on s'arrête sur la suivante ;
- taper une ligne de machine de bureau, pour examiner ou modifier des variables ;
- taper `CONTINUER` : même effet que ‹X-OFF› ;
- taper `NORMAL` (`NO`) pour quitter le mode pas à pas, puis `CONTINUER` pour poursuivre normalement.

```text
PAS A PAS
EXECUTER A PARTIR DE 10
LIGNE 010
LIGNE 020
LIGNE 030
?I
1
LIGNE 040
```

(Chaque `LIGNE` suit une frappe de ‹X-OFF› sur une ligne vide.) Le mode pas à pas reste en vigueur pour les exécutions suivantes, jusqu'à `NORMAL`.

### 8.8 Quand peut-on continuer ?

`CONTINUER` et `POURSUIVRE` ne sont possibles qu'après un arrêt qui laisse le programme **suspendu** : `PAUSE`, ‹ESC›, ligne d'arrêt, pas à pas. Ils sont refusés, avec `????` :

- après `TERMINER`, une erreur, ou tant que rien n'a été exécuté ;
- dès qu'une **ligne de programme a été tapée** depuis l'arrêt, même si elle a été effacée ensuite : le programme a changé, LSE ne peut plus garantir la suite. Effacer des lignes, consulter ou modifier des variables au bureau ne gêne pas.

`REPRENDRE` est possible après tout arrêt, sauf si rien n'a encore été exécuté, ou si une ligne a été tapée depuis.

| Après… | `CONTINUER` | `REPRENDRE` |
|---|---|---|
| `PAUSE`, ‹ESC›, ligne d'arrêt, pas à pas | oui | oui |
| `TERMINER` | non | oui |
| une erreur d'exécution | non | oui |
| une ligne de programme tapée depuis l'arrêt | non | non |
| des lignes de machine de bureau | inchangé | inchangé |
| `EFFACER LIGNES` | inchangé | inchangé |
| `EFFACER LIGNES *`, `APPELER` | non | non |

### 8.9 Une méthode de mise au point

1. **Compilez proprement.** Tapez le programme avec l'option `-LIBELLES` : chaque faute de frappe est expliquée.
2. **Essayez par morceaux.** `EXECUTER A PARTIR DE 10,50` exécute le début, puis s'arrête : vérifiez les variables au bureau (`?A,B,C`).
3. **Surveillez une boucle.** Ajoutez une ligne `AFFICHER` provisoire, ou une `PAUSE`, dans la boucle.
4. **Suivez pas à pas** la partie suspecte (`PAS A PAS`), en examinant les variables entre deux lignes.
5. **Après une erreur**, lisez le code dans l'annexe A, examinez les variables au bureau, corrigez la ligne, puis relancez par `EXECUTER` (ou par `REPRENDRE`, si vous n'avez pas modifié le programme).
6. **Pour les nombres au hasard**, remplacez provisoirement `ALE(0)` par `ALE(0.5)` : le programme se comporte alors de la même façon à chaque essai.

---

## 9. Référence des commandes

### 9.1 Comment taper une commande

Une commande se tape par **ses deux premières lettres**, suivies de ‹X-OFF› ou de ‹RETURN›. LSE écrit alors la suite de son nom (on dit qu'il la **complète**). Si la commande demande des paramètres, il attend qu'on les tape, suivis de ‹X-OFF› ou de ‹RETURN› :

```text
LISTER A PARTIR DE 10
```

(on a tapé `LI` ‹X-OFF›, puis `10` ‹X-OFF›.) Après ‹X-OFF›, la complétion s'écrit à la suite des deux lettres ; après ‹RETURN›, qui a fait passer à la ligne, LSE écrit le nom entier de la commande sur la nouvelle ligne.

On ne tape **jamais** la commande en entier : `LISTER A PARTIR DE 10`, tapé lettre à lettre, serait pris pour une ligne de machine de bureau et refusé. Deux lettres qui ne désignent aucune commande, un paramètre incorrect, ou une commande impossible à ce moment donnent `????`, sans autre effet.

### 9.2 Tableau récapitulatif

| Lettres | Commande | Paramètres | Rôle | § |
|---|---|---|---|---|
| `BO` | `BONJOUR` | — | ouvrir la session | 9.3 |
| `AU` | `AU REVOIR` | — | fermer la session et quitter LSE | 9.3 |
| `LI` | `LISTER A PARTIR DE` | `n1[,n2]` | lister le programme | 9.4 |
| `NU` | `NUMERO A PARTIR DE` | `n1[,n2]` | lister les numéros de ligne | 9.4 |
| `EF` | `EFFACER LIGNES` | liste, ou `*` | effacer des lignes | 9.4 |
| `PE` | `PERFORER A PARTIR DE` | `n1[,n2]` | lister et perforer sur le ruban | 9.4 |
| `EX` | `EXECUTER A PARTIR DE` | `n1[,n2]` | exécuter | 9.5 |
| `CO` | `CONTINUER` | — | continuer après un arrêt | 9.5 |
| `PO` | `POURSUIVRE JUSQU'EN` | `n` | continuer jusqu'à une ligne | 9.5 |
| `RE` | `REPRENDRE A PARTIR DE` | `n1[,n2]` | reprendre à une ligne, sans effacer les variables | 9.5 |
| `PA` | `PAS A PAS` | — | exécuter ligne par ligne | 9.5 |
| `NO` | `NORMAL` | — | quitter le pas à pas | 9.5 |
| `AP` | `APPELER` | `nom` | rappeler un programme rangé | 9.6 |
| `RA` | `RANGER` | `nom[,E\|,L]` | ranger le programme | 9.6 |
| `MO` | `MODIFIER` | `nom` | remplacer un programme rangé | 9.6 |
| `SU` | `SUPPRIMER` | `nom,P\|D\|T` ou `*` | supprimer un fichier | 9.6 |
| `CA` | `CATALOGUER` | `t,p[,L]` | rendre permanent un fichier temporaire | 9.6 |
| `DE` | `DECODER` | `nom[,n1[,n2]]` | écrire le programme dans un fichier de données | 9.6 |
| `EN` | `ENCODER` | `nom[,n1[,n2]]` | relire un programme depuis un fichier de données | 9.6 |
| `TA` | `TABLE DES FICHIERS A PARTIR DE` | `nom[,P\|D\|T]` ou `*` | lister ses fichiers | 9.6 |
| `UT` | `UTILISATION DISQUE A PARTIR DE` | `nom[,P\|D\|T]` ou `*` | lister tous les fichiers | 9.6 |
| `DU` | `DUPLIQUER` | `[$]motif[,P\|D]` | copier entre la bibliothèque et l'unité amovible (LSE F4) | 9.6 |
| `ID` | `IDENTIFICATION` | `n` | changer d'identification | 9.6 |
| `AB` | `ABREGER` | — | ne plus compléter les commandes | 9.7 |
| `IN` | `IN EXTENSO` | — | compléter de nouveau les commandes | 9.7 |
| `PR` | `PRENDRE ETAT CONSOLE` | `n` | reprendre le programme d'une autre console | 9.7 |
| `RU` | `RUBAN` | — | lire le ruban (MITRA 15) | 9.7 |
| `SI` | `SILENCE` | — | ne plus afficher la frappe (MITRA 15) | 9.7 |
| `CR` | `CREATION ID :` | `n[,attributs]` | créer une identification (état dormant) | 9.8 |
| `SU` | `SUPPRESSION ID :` | `n` | supprimer une identification (état dormant) | 9.8 |
| `LI` | `LISTE ID :` | — | lister les identifications (état dormant) | 9.8 |

Dans les paramètres, un nom de fichier peut être précédé de `$` sous le profil LSE F4, pour désigner l'unité amovible (paragraphe 10.10).

### 9.3 Commandes de session

**BONJOUR** (`BO`). Ouvre une session, en état dormant seulement (en session, `BO` répond `????`). LSE écrit la date et l'heure, puis la bannière ; il vide l'espace temporaire et se place sous l'identification initiale (option `-ID`, par défaut 1 sous LSE F4).

**AU REVOIR** (`AU`). Ferme la session : le programme en mémoire, les variables et les fichiers temporaires sont perdus. LSE écrit la date et l'heure, la bannière, puis rend la main au shell ORCA.

> **Sous `-T1600`** `BONJOUR` et `AU REVOIR` écrivent seulement la date et l'heure, précédées de deux espaces.
>
> **Sous `-MITRA15`** `BONJOUR` écrit sur la ligne suivante `LSE-MI15  CONSOLE NO.01` et la date et l'heure ; `AU REVOIR` écrit l'heure seule.

### 9.4 Commandes d'édition

**LISTER A PARTIR DE** `n1[,n2]` (`LI`). Écrit les lignes de numéro `n1` à `n2`, ou de `n1` à la fin. Pour une seule ligne : `n1,n1`. ‹ESC› interrompt. Interdit sur un programme secret (`ERREUR F 06`, paragraphe 11.9).

**NUMERO A PARTIR DE** `n1[,n2]` (`NU`). Écrit les seuls numéros des lignes, dix par ligne d'écran, chacun sur huit colonnes.

**EFFACER LIGNES** liste (`EF`). Efface les lignes données : numéros et intervalles `n1 A n2`, séparés par des virgules. `*` efface tout le programme et ses variables.

**PERFORER A PARTIR DE** `n1[,n2]` (`PE`). Liste les lignes comme `LISTER`, et les ajoute à la fin du ruban (paragraphe 12.3).

### 9.5 Commandes d'exécution

**EXECUTER A PARTIR DE** `n1[,n2]` (`EX`). Efface les variables et exécute à partir de la ligne `n1`, avec une ligne d'arrêt `n2` facultative (paragraphe 8.1).

**CONTINUER** (`CO`). Reprend un programme suspendu là où il s'était arrêté (paragraphe 8.3). Refusé dans les cas du paragraphe 8.8.

**POURSUIVRE JUSQU'EN** `n` (`PO`). Comme `CONTINUER`, avec une nouvelle ligne d'arrêt `n`.

**REPRENDRE A PARTIR DE** `n1[,n2]` (`RE`). Reprend à la ligne `n1`, au niveau principal, sans effacer les variables (paragraphe 8.6).

**PAS A PAS** (`PA`), **NORMAL** (`NO`). Entrent dans le mode pas à pas et en sortent (paragraphe 8.7).

### 9.6 Commandes de fichiers

Ces commandes sont détaillées aux chapitres 10 et 11.

**APPELER** `nom` (`AP`). Remplace le programme en mémoire par le programme rangé sous ce nom. Les variables sont effacées.

**RANGER** `nom` (`RA`). Range le programme en mémoire dans la bibliothèque, sous un nom nouveau. Exige le droit `P`. `RANGER nom,E` le range comme programme secret (droit `S` exigé), `RANGER nom,L` le protège absolument (paragraphe 11.8).

**MODIFIER** `nom` (`MO`). Remplace un programme déjà rangé par le programme en mémoire. Exige le droit `P` et d'être le propriétaire du fichier.

**SUPPRIMER** `nom,P` | `nom,D` | `nom,T` | `*` (`SU`). Supprime le programme (`P`), le fichier de données permanent (`D`) ou le fichier temporaire (`T`) de ce nom. `SUPPRIMER *` supprime tous les fichiers temporaires.

**CATALOGUER** `t,p` (`CA`). Recopie le fichier de données temporaire `t` en fichier de données permanent `p`, puis supprime `t`. Exige le droit `D`. `CATALOGUER t,p,L` protège le fichier permanent.

**DECODER** `nom[,n1[,n2]]` (`DE`). Écrit le texte du programme (ou de ses lignes `n1` à `n2`) dans un fichier de données temporaire (paragraphe 10.9).

**ENCODER** `nom[,n1[,n2]]` (`EN`). Lit un fichier écrit par `DECODER` et ajoute ses lignes au programme en mémoire (paragraphe 10.9).

**TABLE DES FICHIERS A PARTIR DE** `*` | `nom[,P|D|T]` (`TA`). Liste vos fichiers : vos fichiers permanents et les fichiers temporaires de la session (paragraphe 10.7).

**UTILISATION DISQUE A PARTIR DE** `*` | `nom[,P|D|T]` (`UT`). Liste tous les fichiers de la bibliothèque, avec leur propriétaire, et l'occupation du disque (paragraphe 10.7).

**DUPLIQUER** `[$]motif[,P|D]` (`DU`). Copie des fichiers entre la bibliothèque et l'unité amovible (profil LSE F4, paragraphe 10.10).

**IDENTIFICATION** `n` (`ID`). Change l'identification de la session. Le numéro tapé ne s'affiche pas (paragraphe 11.5).

> **Sous `-T1600`** `DUPLIQUER` et `$` n'existent pas.
>
> **Sous `-MITRA15`** `RANGER` ne prend ni `,E` ni `,L`, et `CATALOGUER` ne prend pas `,L`. Un programme et un fichier de données permanent ne peuvent pas porter le même nom (paragraphe 10.1). `SUPPRIMER nom` supprime le programme de ce nom s'il existe, sinon le fichier de données permanent. La liste complète de `TABLE` et d'`UTILISATION` se demande par `A` au lieu de `*`, et `UTILISATION DISQUE` exige l'attribut `>`. `DUPLIQUER` et `$` n'existent pas.

### 9.7 Commandes diverses

**ABREGER** (`AB`), **IN EXTENSO** (`IN`). Suppriment et rétablissent la complétion des commandes (paragraphe 7.8).

**PRENDRE ETAT CONSOLE** `n` (`PR`). Sur les systèmes d'origine, plusieurs consoles partageaient l'ordinateur, et cette commande recopiait le programme d'une autre console. LSE-IIGS n'a qu'une console, la console 1 : `PRENDRE ETAT CONSOLE 1` ne change rien, tout autre numéro donne `????`.

**RUBAN** (`RU`) et **SILENCE** (`SI`), profil MITRA 15 seulement. `RUBAN` met en marche la lecture du ruban (paragraphe 12.4). `SILENCE` cesse d'afficher ce que l'on tape (les réponses de LSE restent affichées), jusqu'à la prochaine frappe de ‹ESC›. Sous les autres profils, `RU` et `SI` répondent `????`.

### 9.8 Commandes de l'état dormant

Avant `BONJOUR`, trois commandes servent au responsable du système pour gérer les identifications (paragraphe 11.4) :

| Lettres | Commande | Rôle |
|---|---|---|
| `CR` | `CREATION ID : n[,P][,D][,S][,!][,>][,X\|,s]` | crée ou redéfinit l'identification `n` avec ses attributs |
| `SU` | `SUPPRESSION ID : n` | supprime l'identification `n` |
| `LI` | `LISTE ID :` | liste les identifications définies |

En session, `SU` et `LI` redeviennent `SUPPRIMER` et `LISTER`.

---

# Partie IV — Fichiers, identifications et rubans

## 10. Les fichiers

### 10.1 Programmes et données, temporaires et permanents

LSE range deux sortes de fichiers dans la bibliothèque (paragraphe 1.6) :

- les **fichiers-programmes**, qui contiennent un programme, rangé par `RANGER` et rappelé par `APPELER` ou `EXECUTER` ;
- les **fichiers-données**, qui contiennent des données (nombres, tableaux, chaînes), écrites et relues par les instructions `GARER` et `CHARGER` d'un programme.

Les fichiers-données sont de deux espèces :

| | Temporaire | Permanent |
|---|---|---|
| Durée | la session : supprimé à `AU REVOIR` (et à `BONJOUR`) | jusqu'à ce qu'on le supprime |
| Nom dans un programme | `'NOM'` | `'#NOM'` |
| Droit nécessaire pour l'écrire | aucun | attribut `D` (chapitre 11) |
| Place | l'espace temporaire, 200 secteurs par défaut | tout le disque |
| Propriétaire | la session | le compte qui l'a créé |

Un fichier-programme est toujours permanent. Le fichier temporaire sert de brouillon, ou à passer des données d'un programme à un autre pendant la session ; `CATALOGUER` le rend permanent (paragraphe 10.6).

### 10.2 Les noms de fichiers

Un nom de fichier est formé d'une **lettre** suivie d'au plus **quatre** lettres ou chiffres : `CARNE`, `F1`, `NOTES`, `X2025`. Dans les commandes, on tape le nom tel quel (`RANGER CARNE`). Dans les instructions d'un programme, le nom est une chaîne, constante ou calculée, précédée de `#` pour un fichier permanent : `GARER T,1,'#NOTES'`, ou `GARER T,1,'#'!NOMF`.

Un programme et un fichier-données permanent peuvent porter le même nom. `RANGER X` est accepté même s'il existe un fichier-données permanent `X`. De même, `GARER` sur `'#X'` et `CATALOGUER T,X` sont acceptés même s'il existe un programme `X`. Les deux fichiers existent alors côte à côte, et `TABLE` les liste tous deux ; `SUPPRIMER X,P` et `SUPPRIMER X,D` les distinguent. Pour éviter toute confusion, donnez tout de même des noms distincts à vos programmes et à leurs fichiers.

> **Sous `-MITRA15`** un nom de fichier permanent ne sert qu'une fois, programmes et fichiers-données confondus :
> - `RANGER X` est refusé s'il existe un fichier-données permanent `X` (`ERREUR F 04`) ;
> - `CATALOGUER T,X` est refusé s'il existe un programme `X` (`ERREUR F 03`) ;
> - `GARER` sur `'#X'` ne crée pas le fichier s'il existe un programme `X` (`ERREUR F 03 EN LIGNE`, suivi du numéro de la ligne).
>
> C'est ce qui permet à la commande `SUPPRIMER X` de désigner un fichier sans préciser s'il s'agit d'un programme ou de données.

### 10.3 Ranger et rappeler un programme

| Commande | Effet | Erreurs courantes |
|---|---|---|
| `RANGER nom` | range le programme en mémoire sous un nom nouveau | `F 03` un programme porte déjà ce nom ; `F 07` pas d'attribut `P` ; sous `-MITRA15`, `F 04` un fichier de données porte ce nom |
| `MODIFIER nom` | remplace le programme rangé sous ce nom par celui en mémoire | `F 05` ce programme n'existe pas ; `F 22` vous n'en êtes pas le propriétaire |
| `APPELER nom` | remplace le programme en mémoire par le programme rangé | `F 05` inconnu ; `F 06` protégé ; `F 100` rangé sous un autre profil |
| `SUPPRIMER nom,P` | supprime le programme rangé | `F 05` inconnu ; `F 22` pas le propriétaire |

Quelques précisions :

- Seul le programme est rangé : ses variables ne le sont pas.
- `APPELER` efface le programme en mémoire et ses variables. Si le fichier est refusé, le programme en mémoire reste intact.
- Pour corriger un programme déjà rangé : `APPELER`, corriger, puis `MODIFIER` sous le même nom. `RANGER` sous le nom d'un programme existant est refusé, ce qui évite d'écraser un programme par mégarde.
- Un fichier-programme ne passe pas d'un profil à l'autre (paragraphe 10.11).

### 10.4 Les fichiers-données

Un fichier-données est une suite d'**enregistrements** numérotés de 1 à 32 767. Chaque enregistrement contient une seule chose : un nombre, un tableau (avec ses éléments non définis), ou une chaîne. Les numéros n'ont pas besoin de se suivre.

**GARER** range une variable dans un enregistrement :

```text
GARER V,N,F
```

- `V` est le nom d'une variable numérique, d'une chaîne ou d'un tableau ;
- `N` est le numéro de l'enregistrement (une expression) ;
- `F` est le nom du fichier (une expression-chaîne, `'#…'` pour un fichier permanent).

Le fichier est créé s'il n'existe pas. Si l'enregistrement existe déjà, son contenu est remplacé.

```text
10 TABLEAU A[4];A[1]←-2;A[3]←3.71;GARER A,2,'ES'
20 CHAINE M;M←'MATINAL';GARER M,3,'ES'
```

**CHARGER** rappelle un enregistrement :

```text
CHARGER V,N,F
CHARGER V,N,F,R
```

`V` devient automatiquement ce qu'était la variable garée : `CHARGER Z,2,'ES'` fait de `Z` un tableau de quatre éléments, `Z[2]` et `Z[4]` restant non définis ; `CHARGER C,3,'ES'` fait de `C` une chaîne valant `'MATINAL'`. Si `V` est déjà d'une autre sorte (un nombre, quand l'enregistrement contient une chaîne), c'est une erreur (`ERREUR (EX.) 47`) : faites précéder le `CHARGER` d'un `LIBERER V` en cas de doute.

Sans `#`, `CHARGER` cherche d'abord un fichier temporaire de ce nom, puis un fichier permanent ; avec `#`, seulement le fichier permanent.

**Le compte rendu.** Avec une quatrième variable `R`, `CHARGER` ne s'arrête jamais sur une erreur de fichier ou d'enregistrement absent, et range dans `R` :

| `R` | Signification |
|---|---|
| −2 | le fichier n'existe pas |
| −1 | l'enregistrement n'existe pas |
| 0 | l'enregistrement contenait un nombre |
| 1 | un tableau à un indice |
| 2 | un tableau à deux indices |
| 3 | une chaîne |

C'est le moyen de savoir si un fichier existe, ou de lire un fichier jusqu'au bout :

```text
20 CHARGER N,1,'#TELS',E;SI E<0 ALORS N←0
```

Sans `R`, un fichier absent provoque `ERREUR (EX.) 55`, un enregistrement absent `ERREUR (EX.) 56`.

**Place occupée.** Chaque enregistrement occupe un nombre entier de **secteurs** de 256 octets : une chaîne occupe un octet par caractère, un nombre 4 octets, un élément de tableau un peu plus de 4 octets. L'espace temporaire est limité, par défaut à 200 secteurs (50 Ko) : au-delà, `ERREUR (EX.) 52`. On l'agrandit au lancement par l'option `-TEMP`, ou par une identification (paragraphe 11.3).

**Supprimer.** La commande `SUPPRIMER nom,T` supprime un fichier temporaire, `SUPPRIMER nom,D` un fichier permanent, et `SUPPRIMER *` tous les fichiers temporaires.

> **Sous `-MITRA15`** l'instruction `SUPPRIMER F,N` supprime l'enregistrement `N` du fichier `F`, et `SUPPRIMER F` tout le fichier, depuis un programme. La commande s'écrit `SUPPRIMER nom` (programme de ce nom s'il existe, sinon fichier-données permanent).

**Erreurs fréquentes.** Numéro d'enregistrement qui n'est pas un entier de 1 à 32 767 : `EX 50`. Nom de fichier mal formé : `EX 53`. Fichier protégé d'un autre compte : `EX 54`. Fichier permanent sans l'attribut `D` : `ERREUR F 08`.

### 10.5 Exemple complet : un carnet de téléphone

Ce programme garde des noms et des numéros de téléphone dans le fichier permanent `TELS`. L'enregistrement 1 contient le nombre de fiches ; les enregistrements 2, 3… contiennent chacun une fiche `'NOM/TELEPHONE'`.

```text
1*CARNE : CARNET DE TELEPHONE, RANGE DANS LE FICHIER PERMANENT TELS
2*ENREGISTREMENT 1 : NOMBRE DE FICHES ; 2, 3... : 'NOM/TELEPHONE'
10 CHAINE NOM,TEL,C
20 CHARGER N,1,'#TELS',E;SI E<0 ALORS N←0
30 AFFICHER[/,'1 AJOUTER  2 LISTER  3 CHERCHER  4 FINIR',/,'CHOIX ? ']
40 LIRE R;ALLER EN 50+50*R
100*AJOUTER UNE FICHE
110 AFFICHER 'NOM : ';LIRE NOM;AFFICHER 'TELEPHONE : ';LIRE TEL
120 N←N+1;GARER N,1,'#TELS';C←NOM!'/'!TEL;GARER C,N+1,'#TELS'
130 ALLER EN 30
150*LISTER LES FICHES
160 FAIRE 180 POUR I←1 JUSQUA N;CHARGER C,I+1,'#TELS'
170 NOM←SCH(C,1,'/',P);TEL←SCH(C,P+1,LGR(C))
180 AFFICHER[/,U,' : ',U] NOM,TEL
190 ALLER EN 30
200*CHERCHER UN NOM
210 AFFICHER 'NOM CHERCHE : ';LIRE NOM
220 FAIRE 230 POUR I←1 JUSQUA N;CHARGER C,I+1,'#TELS'
230 SI POS(C,1,NOM!'/')=1 ALORS AFFICHER C
240 ALLER EN 30
250 TERMINER
```

Une session (chaque réponse terminée par ‹X-OFF›) :

```text
EXECUTER A PARTIR DE 1

1 AJOUTER  2 LISTER  3 CHERCHER  4 FINIR
CHOIX ? 1
NOM : DUPONT
TELEPHONE : 01 02 03 04 05
1 AJOUTER  2 LISTER  3 CHERCHER  4 FINIR
CHOIX ? 1
NOM : MARTIN
TELEPHONE : 06 07 08 09 10
1 AJOUTER  2 LISTER  3 CHERCHER  4 FINIR
CHOIX ? 2
DUPONT : 01 02 03 04 05
MARTIN : 06 07 08 09 10
1 AJOUTER  2 LISTER  3 CHERCHER  4 FINIR
CHOIX ? 3
NOM CHERCHE : MARTIN
MARTIN/06 07 08 09 10
1 AJOUTER  2 LISTER  3 CHERCHER  4 FINIR
CHOIX ? 4
TERMINE EN LIGNE 250
```

Les fiches restent dans `TELS` d'une session à l'autre. On remarquera la ligne 40, qui choisit la ligne suivante d'après la réponse, et la ligne 170, où `SCH` range dans `P` la position du `/`, aussitôt réutilisée.

### 10.6 Rendre un fichier permanent : CATALOGUER

```text
CATALOGUER TEMP,PERM
```

recopie le fichier-données temporaire `TEMP` dans un fichier permanent `PERM`, inscrit à votre compte, puis supprime `TEMP`. Il faut l'attribut `D`, et `PERM` ne doit pas déjà exister comme fichier de données (`F 04`) ; sous `-MITRA15`, ni comme programme (`F 03`). C'est la façon de garder le résultat d'un travail fait en fichier temporaire. `CATALOGUER TEMP,PERM,L` protège en plus le fichier (paragraphe 11.8).

### 10.7 Voir ses fichiers : TABLE DES FICHIERS et UTILISATION DISQUE

`TABLE DES FICHIERS A PARTIR DE` (`TA`) liste **vos** fichiers : les fichiers permanents de votre compte et les fichiers temporaires. `UTILISATION DISQUE A PARTIR DE` (`UT`) liste **tous** les fichiers de la bibliothèque, avec le compte de leur propriétaire. Le paramètre choisit ce qu'on liste :

| Paramètre | Liste |
|---|---|
| `*` | tous les fichiers, par espèce |
| `*,P` ou `*,D` ou `*,T` | tous les programmes, tous les fichiers-données permanents, ou tous les temporaires |
| `NOM` ou `NOM,P` | les programmes, par ordre alphabétique, à partir du premier nom égal ou supérieur à `NOM` |
| `NOM,D` ou `NOM,T` | de même pour les fichiers-données permanents, ou temporaires |

```text
TABLE DES FICHIERS A PARTIR DE *
ESPACE TEMPORAIRE( 200 SECT.) OCCUPE A   1 %

 NOM       TYPE        LONGUEUR MOTS   PROTECTION
PASSE     TEMPORAIRE         256
TELS       DONNEE            512
CARNE     PROGRAMME          336
ETAP1     PROGRAMME           50
```

```text
UTILISATION DISQUE A PARTIR DE *
ESPACE PERMANENT (9486 SECT.) OCCUPE A   0 %

 FICHIERS-PROGRAMMES

 NOM     NO COMPTE     LONGUEUR MOTS   PROTECTION
CARNE         01             336
ETAP1         01              50

 FICHIERS-DONNEES PERMANENTS

 NOM     NO COMPTE     LONGUEUR MOTS   PROTECTION
TELS          01             512

 CONSOLE 1

 NOM                   LONGUEUR MOTS
PASSE                        256
```

- La longueur est comptée en **mots** de 2 octets, comme sur les ordinateurs d'origine.
- La ligne `ESPACE TEMPORAIRE` indique la taille de l'espace temporaire et sa part occupée ; la ligne `ESPACE PERMANENT` indique la place des fichiers permanents plus la place libre sur le disque, et la part occupée.
- La colonne `PROTECTION` montre `E` (programme secret) ou `L` (protection absolue).
- La section `CONSOLE 1` liste les fichiers temporaires.
- ‹ESC› interrompt la liste.

Le premier `UTILISATION DISQUE` d'une session interroge le disque pour connaître la place libre, ce qui peut prendre un instant.

> **Sous `-MITRA15`** la liste complète se demande par `A` (`TABLE DES FICHIERS A PARTIR DE A`), et `*` est refusé. La date et l'heure s'affichent sous la commande, et les colonnes diffèrent : longueur des programmes en mots, des fichiers-données en secteurs. `UTILISATION DISQUE` exige l'attribut `>` (`????` sinon).

### 10.8 Enchaîner des programmes : EXECUTER

L'instruction `EXECUTER` remplace, en cours d'exécution, le programme par un autre programme rangé, et lance celui-ci :

```text
EXECUTER 'PASS2'
EXECUTER 'ZZZ',26
```

Le premier exemple exécute le programme `PASS2` à partir de sa ligne 1 ; le second, `ZZZ` à partir de sa ligne 26. Le nom est une expression-chaîne. Le programme appelant et **toutes ses variables** disparaissent. Pour transmettre des valeurs au programme suivant, on les gare dans un fichier temporaire, qui, lui, demeure :

```text
1*ETAP1 : PREMIERE ETAPE
10 CHAINE N;AFFICHER 'VOTRE NOM ? ';LIRE N
20 GARER N,1,'PASSE'
30 EXECUTER 'ETAP2'
```

```text
1*ETAP2 : DEUXIEME ETAPE
10 CHARGER N,1,'PASSE'
20 AFFICHER 'BONJOUR '!N!', VOUS VOICI DANS L''ETAPE 2.'
30 TERMINER
```

Après avoir rangé les deux programmes (`RANGER ETAP1`, `RANGER ETAP2`), puis rappelé le premier (`APPELER ETAP1`) :

```text
EXECUTER A PARTIR DE 1

VOTRE NOM ? LEA
BONJOUR LEA, VOUS VOICI DANS L'ETAPE 2.
TERMINE EN LIGNE 030
```

C'est ainsi qu'on découpe un gros travail en plusieurs programmes plus petits : un menu, puis un programme par choix. Un programme absent donne `ERREUR (EX.) 105` ; une ligne de départ absente, `ERREUR (EX.) 75`, et le programme appelant reste en place. `EXECUTER` est interdit en mode machine de bureau.

### 10.9 Un programme en texte : DECODER et ENCODER

`DECODER nom` écrit le **texte** du programme en mémoire, tel que le donnerait `LISTER`, dans un fichier-données **temporaire** de ce nom. `DECODER nom,n1,n2` n'écrit que les lignes `n1` à `n2`. Le fichier est formé d'enregistrements-chaînes d'au plus 252 caractères, numérotés à partir de 1, qui contiennent chacun autant de lignes entières qu'il en tient ; les lignes y sont séparées par le caractère X-OFF (code 19), et le texte se termine par le caractère de code 0.

`ENCODER nom` fait l'inverse : il lit un tel fichier (temporaire d'abord, sinon permanent), et **ajoute** ses lignes au programme en mémoire, comme si on les tapait. Une ligne de même numéro remplace l'ancienne. `ENCODER nom,n1,n2` ne prend que les lignes `n1` à `n2`. À la première ligne refusée, LSE l'affiche avec son message d'erreur, et s'arrête ; les lignes déjà prises restent.

À quoi cela sert-il ?

- à **garder un programme sous forme de texte**, lisible par un programme LSE : `CHARGER` en rappelle les enregistrements comme des chaînes ;
- à **écrire un programme qui fabrique ou transforme des programmes** : il construit les lignes dans des chaînes (séparées par `EQC(19)` ou `EQC(13)`), les gare, et `ENCODER` les fait entrer ;
- à **passer un programme d'un profil à l'autre** (paragraphe 10.11) ;
- à **fusionner** deux programmes : `APPELER` le premier, puis `ENCODER` le texte du second.

> **Sous `-MITRA15`** `ENCODER nom`, sans numéros, **remplace** tout le programme en mémoire ; avec `n1` (et `n2`), il ajoute les lignes, comme sous les autres profils. Les lignes y sont réparties sur les enregistrements de 252 caractères sans tenir compte des fins de lignes.

### 10.10 L'unité amovible : `$` et DUPLIQUER (profil LSE F4)

Sur le T1600 de Victor Duruy, les élèves pouvaient garder leurs programmes sur une disquette, l'**unité amovible**. LSE-IIGS la reproduit sous le profil LSE F4 : l'option de lancement `-UNITE chemin` désigne un répertoire qui joue ce rôle, par exemple sur une disquette 3,5 pouces :

```text
LSE -UNITE :DISQUETTE:LSE
```

L'unité est organisée comme la bibliothèque (elle a ses propres `PROGRAMMES`, `DONNEES` et `CATALOGUE`), et LSE crée ce qui manque.

**Le signe `$`.** Dans une commande, `$` devant un nom désigne un fichier de l'unité au lieu de la bibliothèque. Les droits, les protections et les erreurs sont les mêmes :

| Commande | Effet |
|---|---|
| `RANGER $NOM` | range le programme sur l'unité |
| `APPELER $NOM` | rappelle un programme de l'unité |
| `MODIFIER $NOM` | remplace un programme de l'unité |
| `SUPPRIMER $NOM,P` ou `$NOM,D` | supprime un fichier de l'unité |
| `CATALOGUER T,$NOM` | fait du fichier temporaire `T` un fichier-données de l'unité |
| `DECODER $NOM`, `ENCODER $NOM` | écrit ou lit le texte d'un programme sur l'unité |
| `TABLE DES FICHIERS A PARTIR DE $*` | liste le contenu de l'unité |

Les instructions d'un programme (`GARER`, `CHARGER`, `EXECUTER`) n'acceptent pas `$`.

**DUPLIQUER** copie des fichiers entre la bibliothèque et l'unité :

```text
DUPLIQUER NOM          copie NOM de la bibliothèque vers l'unité
DUPLIQUER $NOM         copie NOM de l'unité vers la bibliothèque
DUPLIQUER EX*          copie tous les fichiers dont le nom commence par EX
DUPLIQUER $*,P         copie tous les programmes de l'unité vers la bibliothèque
DUPLIQUER *,D          copie tous les fichiers-données permanents vers l'unité
```

Un `*` final désigne tous les noms qui commencent par ce qui précède ; `*` seul, tous les fichiers. `,P` ou `,D` limite la copie aux programmes ou aux fichiers-données. Les fichiers sont copiés par ordre alphabétique ; la copie s'arrête à la première erreur. Un fichier de même nom à l'arrivée n'est pas remplacé (`F 03` ou `F 04`). Vers la bibliothèque, il faut les droits `P` ou `D`, et le fichier copié vous appartient ; vers l'unité, un fichier protégé d'un autre compte n'est pas copié (`F 06`). Sans option `-UNITE`, ces commandes répondent `ERREUR F 29`.

> **Sous `-T1600` et `-MITRA15`** l'unité amovible, `$` et `DUPLIQUER` n'existent pas.

### 10.11 Passer d'un profil à l'autre

Un fichier-programme est rangé avec son profil, et seul ce profil peut le rappeler : sous un autre profil, `APPELER` répond `ERREUR F 100`. En revanche, les fichiers-données sont communs aux trois profils, et le texte d'un programme passe de l'un à l'autre. Pour faire passer un programme :

1. sous le profil d'origine, perforez-le (`PERFORER A PARTIR DE 1`, paragraphe 12.3) ou décodez-le (`DECODER`) ;
2. sous le profil d'arrivée, relisez le ruban, ou faites `ENCODER` ;
3. corrigez les lignes refusées : ce sont celles qui emploient des formes propres au premier profil (annexe D).

Comme les noms de programmes sont uniques dans une bibliothèque, le plus simple est de donner **une bibliothèque à chaque profil** que vous utilisez (`LSE -MITRA15 -BIB @:LSEM15`, par exemple).

### 10.12 Sauvegarder son travail

- **Toute la bibliothèque** : copiez son répertoire avec le shell, LSE étant arrêté (paragraphe 1.6).
- **Un programme** : `PERFORER` en fait un texte lisible, que l'on peut imprimer, relire, ou relire sous un autre profil (chapitre 12).
- **Un fichier-données** : le programme `PERFO` le perfore sur un ruban, le programme `CHARG` le restitue (paragraphe 12.9).

---

## 11. Identifications, droits et protections

### 11.1 Pourquoi des identifications

Les systèmes LSE d'origine étaient partagés par toute une classe, voire tout un lycée. Les **identifications** permettent de savoir qui a le droit de ranger des programmes, d'écrire des fichiers permanents, et à qui appartient chaque fichier. Sans identification, on peut écrire et exécuter des programmes, lire les fichiers, mais rien garder durablement.

### 11.2 Numéro, clé et compte

Une identification est un nombre de 0 à 65 499. Ses deux derniers chiffres forment le **compte**, les autres la **clé** :

| Identification | Clé | Compte |
|---|---|---|
| 1205 | 12 | 05 |
| 4810 | 48 | 10 |
| 1 | 0 | 01 |

Le **compte** désigne le propriétaire des fichiers : tous les fichiers permanents créés sous des identifications de même compte lui appartiennent. La **clé** joue le rôle d'un mot de passe : on la garde secrète, c'est pourquoi le numéro ne s'affiche pas quand on le tape.

Deux identifications existent toujours, et ne peuvent pas être modifiées :

- **0** : aucun droit ;
- **1** : tous les droits ordinaires (`P`, `D`, `S` et `x`, voir ci-dessous).

On peut en définir neuf autres (paragraphe 11.4).

### 11.3 Les attributs

Chaque identification porte des **attributs**, qui sont ses droits :

| Attribut | Nom dans `LISTE ID` | Droit donné |
|---|---|---|
| `P` | `P` | créer, modifier, supprimer des programmes (`RANGER`, `MODIFIER`, `SUPPRIMER nom,P`) |
| `D` | `D` | créer, modifier, supprimer des fichiers-données permanents (`GARER` sur `'#…'`, `CATALOGUER`, `SUPPRIMER nom,D`) |
| `S` | `SECRET` | ranger des programmes secrets (`RANGER nom,E`) |
| `!` | `PROTEGE` | tout fichier permanent créé sous cette identification est protégé absolument (`L`) |
| `>` | `PRIVILEGIE` | lire et modifier les programmes secrets des autres comptes |
| `X` | `ETENDU` | (MITRA 15) choisir la taille de l'espace temporaire à l'identification |
| un nombre `s` | `s` | espace temporaire de `s` secteurs (au moins l'espace standard) |

### 11.4 Créer les identifications : l'état dormant

Les identifications se définissent **avant d'ouvrir une session**, en état dormant, juste après le lancement. C'est en principe le rôle du responsable de la salle. Trois commandes y sont disponibles :

**CREATION ID** (`CR`). Crée ou redéfinit une identification. On tape le numéro, puis les attributs, séparés par des virgules, **dans cet ordre** : `P`, `D`, `S`, `!`, `>`, puis `X` ou un nombre de secteurs. Les attributs sont tous facultatifs.

```text
CREATION ID : 1205,P,D
CREATION ID : 1306,D,30
CREATION ID : 4810,P,D,S,>
```

**LISTE ID** (`LI`). Liste les identifications définies :

```text
LISTE ID :
    1,P,D,SECRET,ETENDU
 1205,P,D
 1306,D,30
 4810,P,D,SECRET,PRIVILEGIE
```

**SUPPRESSION ID** (`SU`). Supprime une identification : `SUPPRESSION ID : 1306`.

Règles : 0 et 1 ne peuvent être ni redéfinies ni supprimées (`????`, `F 10`) ; on peut en définir au plus neuf autres (`ERREUR F 19` au-delà) ; supprimer une identification inconnue donne `ERREUR F 10`. Les identifications sont gardées dans la bibliothèque, dans le fichier `IDENTIFICATIONS` : elles restent valables d'un lancement à l'autre.

### 11.5 Changer d'identification en session : IDENTIFICATION

```text
IDENTIFICATION 1205
```

(`ID`, puis le numéro, qui **ne s'affiche pas**, puis ‹X-OFF›.) Si le numéro est accepté, LSE passe simplement à la ligne ; sinon, il répond `????` et l'identification ne change pas.

- Les fichiers temporaires sont conservés.
- Un programme secret en mémoire le reste (paragraphe 11.9).
- L'attribut « nombre de secteurs » fixe la taille de l'espace temporaire.

> **Sous `-MITRA15`** `IDENTIFICATION` **supprime les fichiers temporaires** : créez-les après vous être identifié. Avec l'attribut `X`, LSE indique la place libre et demande la taille voulue :
>
> ```text
> IDENTIFICATION
> NOMBRE DE SECTEURS DISPONIBLES:9484
> ESPACE TEMPORAIRE DESIRE:400
> ```
>
> Une réponse vide, ou inférieure à l'espace standard, donne l'espace standard. Si la place libre ne suffit pas, LSE répond `ESPACE TEMPORAIRE REDUIT A 9484. SECTEURS.`

### 11.6 L'identification au lancement : -ID

Chaque session s'ouvre sous une identification initiale, fixée au lancement par l'option `-ID` :

- sous LSE F4, par défaut **1** : on peut ranger des programmes sans s'identifier, comme au lycée Victor Duruy ; la bannière l'annonce par `VOUS ETES SOUS ID 1` ;
- sous `-T1600` et `-MITRA15`, par défaut **0** : il faut taper `IDENTIFICATION` pour ranger quoi que ce soit.

La ligne `VOUS ETES SOUS ID` de la bannière donne le compte de l'identification initiale : avec `-ID 1205`, elle affiche `VOUS ETES SOUS ID 5`. Pour une salle de classe, lancez LSE avec `-ID 0`, afin que chacun doive s'identifier.

### 11.7 La propriété des fichiers

Un fichier permanent appartient au **compte** sous lequel il a été créé, inscrit au catalogue (colonne `NO COMPTE` d'`UTILISATION DISQUE`). Seul ce compte peut le modifier ou le supprimer : sinon, `ERREUR F 22`. Tout le monde peut appeler les programmes et lire les fichiers-données des autres, sauf protection.

### 11.8 Protéger un programme ou un fichier : ,E et ,L

| Protection | Comment | Effet pour les autres comptes |
|---|---|---|
| `E`, programme secret | `RANGER nom,E` (attribut `S` exigé, `F 11` sinon) | ils peuvent l'exécuter, mais ni le lister, ni le modifier, ni le copier |
| `L`, protection absolue | `RANGER nom,L`, `CATALOGUER t,p,L`, ou tout fichier créé sous une identification `!` | ils ne peuvent ni l'appeler, ni l'exécuter (`F 06`), ni le charger (`EX 54`) |

La protection est indiquée dans la colonne `PROTECTION` des tables.

> **Sous `-MITRA15`** ces protections n'existent pas.

### 11.9 Le programme secret

Un programme rangé par `RANGER nom,E` et appelé (par `APPELER` ou `EXECUTER`) sous un autre compte, sans l'attribut `>`, devient un **programme secret** : on peut l'exécuter, mais tout ce qui permettrait d'en voir ou d'en copier le texte est refusé par `ERREUR F 06` : `LISTER`, `NUMERO`, `PERFORER`, `DECODER`, `EFFACER LIGNES` d'une ligne, l'entrée d'une ligne, `RANGER`, `MODIFIER`. Le secret dure jusqu'à `EFFACER LIGNES *` ou un autre `APPELER` ; changer d'identification ne le lève pas. C'est ainsi qu'un professeur peut confier aux élèves un programme d'exercices sans en livrer les réponses.

### 11.10 Exemple : une salle de classe

Le professeur prépare les identifications une fois pour toutes, en état dormant :

```text
CREATION ID : 4810,P,D,S,>
CREATION ID : 1221,P,D
CREATION ID : 3722,P,D
```

`4810` (compte 10) est le sien ; `1221` (compte 21) et `3722` (compte 22) sont celles de deux élèves. Il range son programme d'exercices en secret :

```text
IDENTIFICATION 4810
APPELER QUIZ
RANGER EXERC,E
```

LSE est ensuite lancé avec `-ID 0` pour la classe. Chaque élève ouvre sa session, tape `IDENTIFICATION` et son numéro, appelle `EXERC`, l'exécute, mais ne peut pas le lister. Ses propres programmes, rangés sous son compte, ne peuvent être modifiés ou supprimés par personne d'autre que lui.

### 11.11 Récapitulatif des droits

| Opération | Droit nécessaire |
|---|---|
| écrire, exécuter, corriger un programme en mémoire | aucun |
| `GARER`, `CHARGER`, `SUPPRIMER` sur un fichier temporaire | aucun |
| `DECODER nom` (fichier temporaire) | aucun |
| `RANGER`, `MODIFIER`, `SUPPRIMER nom,P` | `P` (et la propriété, pour un fichier existant) |
| `GARER` sur `'#…'`, `CATALOGUER`, `SUPPRIMER nom,D` | `D` (et la propriété, pour un fichier existant) |
| `RANGER nom,E` | `P` et `S` |
| lister un programme secret d'un autre compte | `>` |
| `APPELER`, `CHARGER`, `ENCODER` d'un fichier `L` d'un autre compte | impossible |

---

## 12. Les rubans

### 12.1 Le ruban, d'hier à aujourd'hui

Sur les télétypes des années 1970, un **ruban perforé** de papier gardait un texte : le perforateur de la télétype le perçait de trous en même temps qu'elle imprimait, et son lecteur le relisait comme si l'on tapait au clavier. On s'en servait pour garder un programme, pour le transporter d'un ordinateur à un autre, ou pour le préparer à l'avance sans occuper l'ordinateur.

Dans LSE-IIGS, le ruban est un **fichier texte**. `PERFORER` y écrit ; la lecture le relit comme une frappe au clavier. Le même fichier sert aux deux, comme on reportait le ruban du perforateur au lecteur.

### 12.2 Où est le ruban

Par défaut, le ruban est le fichier `RUBAN` de la bibliothèque (`@:LSE:RUBAN`). L'option de lancement `-RUBAN chemin` en désigne un autre, n'importe où, par exemple sur une disquette :

```text
LSE -RUBAN :DISQUETTE:PROGS
```

Le fichier est un fichier texte ProDOS ordinaire (type TXT) : les lignes y sont séparées par le caractère de code 13 (CR). On peut le lire avec la commande `TYPE` du shell, l'ouvrir avec l'éditeur d'ORCA (`EDIT`), le copier ou le renommer.

### 12.3 Perforer un programme : PERFORER

```text
PERFORER A PARTIR DE 1
PERFORER A PARTIR DE 100,190
```

`PERFORER` (`PE`) liste les lignes comme `LISTER`, et les **ajoute à la fin** du ruban, chacune suivie d'un retour à la ligne. ‹ESC› l'interrompt. Un programme secret ne peut pas être perforé (`F 06`).

Le ruban s'allonge à chaque perforation : il n'est jamais effacé par LSE. Pour repartir d'un ruban vide, supprimez-le ou renommez-le avec le shell, LSE étant arrêté :

```text
DELETE @:LSE:RUBAN
RENAME @:LSE:RUBAN @:LSE:DEVIN.TXT
```

### 12.4 Lire un ruban

La lecture du ruban remplace le clavier : tout ce que LSE attend (une ligne de programme, une commande, ses paramètres, une réponse à `LIRE`, un numéro d'identification) est pris sur le ruban, une ligne à la fois.

**Sous LSE F4 et `-T1600`** : frappez ‹X-ON› (**Ctrl-Q**) sur une ligne vide. Le lecteur se met en marche, et chaque ligne lue s'affiche comme si vous l'aviez tapée.

**Sous `-MITRA15`** : tapez la commande `RUBAN` (`RU`). La lecture se fait en silence : les lignes lues ne s'affichent pas, seules les réponses de LSE apparaissent. ‹X-ON› est sans effet.

Le lecteur s'arrête :

- à la **fin du ruban** ; la lecture suivante reprendra au début ;
- sur un caractère ESC (code 27) écrit dans le ruban ;
- quand on frappe ‹ESC› au clavier.

Après un arrêt sur ESC, la lecture suivante reprend là où elle s'était arrêtée.

Une ligne refusée pendant la lecture est signalée par son message d'erreur, et la lecture continue. Le clavier reprend la main dès que le lecteur s'arrête.

Pour relire un programme perforé :

```text
EFFACER LIGNES *
```

puis ‹X-ON› sur une ligne vide : les lignes défilent et entrent dans le programme.

### 12.5 Préparer un ruban avec l'éditeur d'ORCA

Un ruban peut être tapé avec l'éditeur d'ORCA (paragraphe 7.7) : tapez `PRODOS` dans le shell, pour que le nouveau fichier soit un texte ProDOS, puis `EDIT` suivi du chemin. Écrivez une ligne de ruban par ligne de texte :

- une ligne de programme, avec son numéro ;
- une commande, par ses deux lettres seules, puis ses paramètres sur la ligne suivante ;
- une réponse à `LIRE`.

Le ruban est lu exactement comme une frappe terminée par ‹RETURN›. Rappel : `_` pour `←`, `^` pour `↑`, pas de tabulation.

### 12.6 Un ruban qui se range tout seul

Un ruban peut contenir, après les lignes d'un programme, les commandes qui le rangent puis l'effacent de la mémoire :

```text
10 AFFICHER 'BONJOUR'
20 TERMINER
RA
SALUT
EF
*
```

Lire ce ruban installe le programme `SALUT` dans la bibliothèque, sans rien laisser en mémoire. C'est ainsi que sont livrés PERFO et CHARG (paragraphe 1.7).

### 12.7 Fournir les réponses d'un programme par le ruban

Puisque la lecture remplace le clavier, un ruban peut aussi lancer un programme et répondre à ses `LIRE` :

```text
EX
1
5
12 15.5 8 17 11
```

Ce ruban lance le programme en mémoire à partir de la ligne 1, puis fournit `5`, puis les cinq notes, au programme MOYEN de l'annexe G. Pour les nombres, c'est sans piège. Pour une chaîne, souvenez-vous qu'une ligne de ruban finit comme une frappe de ‹RETURN› : la chaîne lue contient en plus un passage à la ligne (paragraphe 5.15). Un ruban écrit par un programme peut terminer ses chaînes par le caractère X-OFF (`EQC(19)`), qui ne les modifie pas : c'est ce que fait PERFO.

### 12.8 Un programme qui perfore : EQC(18) et EQC(20)

Sur la télétype, un programme pouvait mettre en marche le perforateur et l'arrêter, par deux caractères de commande. LSE-IIGS fait de même :

- `EQC(18)`, écrit par `AFFICHER`, met le **perforateur en marche** ;
- `EQC(20)` l'**arrête**.

Tant qu'il est en marche, **tout ce que le programme affiche est aussi ajouté au ruban**, tel quel : les retours à la ligne de `/` compris, mais sans les messages de LSE, ni ce que l'on tape au clavier. Ces deux caractères ne s'affichent pas et ne sont pas perforés. Le perforateur s'arrête aussi de lui-même dès que le programme s'arrête, quelle qu'en soit la raison.

Voici un programme qui écrit sur le ruban un autre programme, de la longueur qu'on lui demande :

```text
1*GENER : ECRIT UN PROGRAMME SUR LE RUBAN
10 CHAINE L;AFFICHER 'COMBIEN DE LIGNES ? ';LIRE N
20 AFFICHER[/,U] EQC(18)
30 FAIRE 40 POUR K←1 JUSQUA N;L←CCA(10*K)
40 AFFICHER[U,/] L!' AFFICHER ''CECI EST LA LIGNE '!L!''''
50 AFFICHER[U,U,/,U] CCA(10*K),' TERMINER',EQC(20)
60 TERMINER
```

Exécuté avec la réponse 3, il affiche, et ajoute au ruban :

```text
10 AFFICHER 'CECI EST LA LIGNE 10'
20 AFFICHER 'CECI EST LA LIGNE 20'
30 AFFICHER 'CECI EST LA LIGNE 30'
40 TERMINER
```

Après `EFFACER LIGNES *` et ‹X-ON›, ces lignes deviennent le programme en mémoire. On remarquera, ligne 40, comment une apostrophe se met dans une chaîne : `''''` est une chaîne qui contient une seule apostrophe. Une impossibilité d'écrire sur le ruban (disque plein) arrête le programme par `ERREUR (EX.) 52`.

### 12.9 Sauvegarder des données sur ruban : PERFO et CHARG

Les programmes **PERFO** et **CHARG**, fournis sur la disquette (installation : paragraphe 1.7), sauvegardent un fichier-données sur un ruban, sous forme de texte, et le restituent. On peut ainsi garder un fichier en clair, le transporter d'une bibliothèque à une autre, ou l'imprimer.

**Perforer un fichier.** Partez d'un ruban vide (paragraphe 12.3), puis :

```text
APPELER PERFO
EXECUTER A PARTIR DE 1

NOM : SEMA
PERFORATION ? (OUI/NON) OUI
PREMIER ET DERNIER ENREGISTREMENT : 1 3
```

- `NOM` : le nom du fichier-données (temporaire ou permanent, sans `#`) ;
- `PERFORATION ?` : `OUI` pour perforer ; `NON` pour un simple listage à l'écran ;
- les numéros du premier et du dernier enregistrement à traiter ; les enregistrements absents entre les deux sont sautés.

PERFO affiche alors chaque enregistrement, en même temps qu'il le perfore. Une chaîne occupe toujours quatre lignes, coupées vers la 72ᵉ colonne. Pour un tableau, PERFO ne peut pas deviner les dimensions, et les demande :

```text
DIMENSIONS DE L'ENREGISTREMENT 2 : 3
ENREGISTREMENT 2 1 3
1.5
-2
1E+06
```

Le ruban obtenu commence par les commandes qui appellent et lancent CHARG, et se termine par la commande `CATALOGUER` qui rendra le fichier permanent.

**Restituer le fichier.** Dans la bibliothèque d'arrivée, où CHARG doit être installé et où le fichier ne doit pas déjà exister, lancez LSE en désignant ce ruban, puis lisez-le :

```text
LSE -RUBAN :DISQUETTE:SEMA
```

puis `BO`, et ‹X-ON› sur la ligne vide. Le ruban appelle CHARG, l'exécute, lui fournit les données, que CHARG gare dans un fichier temporaire, puis `CATALOGUER SEMA,SEMA` le rend permanent.

Quelques limites :

- les nombres passent par leur écriture à sept chiffres : un nombre comme 1/3 ne revient pas exactement identique ;
- la restitution demande le droit `D` (pour `CATALOGUER`), et que le fichier permanent n'existe pas déjà (`F 04` sinon) ;
- sous `-MITRA15`, la lecture du ruban se lance par `RUBAN`.

### 12.10 Les caractères de commande du ruban

Les caractères de code 17 à 21 (annexe C) étaient, sur la télétype, des ordres au lecteur et au perforateur. Écrits par un programme, ils ne s'affichent jamais :

| Code | Nom | Effet dans LSE-IIGS |
|---|---|---|
| 17 | X-ON | aucun à l'écran ; perforé si le perforateur est en marche |
| 18 | perforateur en marche | met en marche le perforateur (paragraphe 12.8) |
| 19 | X-OFF | aucun à l'écran ; perforé si le perforateur est en marche ; dans un ruban lu, termine une saisie |
| 20 | perforateur arrêté | arrête le perforateur |
| 21 | — | aucun à l'écran ; perforé si le perforateur est en marche |

À la lecture, les caractères de code 10 (LF) et 0 (NUL) d'un ruban sont ignorés, le code 13 (CR) termine une saisie comme ‹RETURN›, le code 19 (X-OFF) comme ‹X-OFF›, et le code 27 (ESC) arrête le lecteur.

---

# Annexes

## Annexe A. Messages d'erreur

### A.1 La forme des messages

LSE signale les erreurs par un code, comme les systèmes d'origine. Il y a quatre familles :

| Famille | Forme du message | Quand |
|---|---|---|
| dialogue | `????` | commande inconnue ou impossible, paramètre incorrect |
| compilation | `ERREUR  C nn` | une ligne tapée contient une faute ; elle n'est pas prise |
| exécution | `ERREUR (EX.) nn EN LIGNE nnn` | une erreur se produit pendant l'exécution ; `EN LIGNE BUR` pour une ligne de machine de bureau |
| fichiers | `ERREUR F nn` | une opération sur un fichier échoue ; suivi de ` EN LIGNE nnn` si elle vient d'une instruction d'un programme |

Avec l'option de lancement `-LIBELLES`, chaque message est suivi d'une explication :

```text
ERREUR  C 03 : IDENTIFICATEUR DE PLUS DE 5 CARACTERES
ERREUR (EX.) 31 EN LIGNE 050 : DIVISION PAR ZERO
???? : CONTINUER IMPOSSIBLE
```

Un même code peut couvrir plusieurs situations voisines : les tableaux qui suivent donnent, pour chaque code, toutes les explications possibles (en capitales, telles que `-LIBELLES` les affiche), puis ce qu'il faut faire. Les numéros sont ceux des systèmes d'origine ; c'est pourquoi ils ne se suivent pas.

### A.2 Erreurs du dialogue : `????`

| Explication affichée par `-LIBELLES` | Cause et remède |
|---|---|
| `COMMANDE ERRONEE` | deux lettres qui ne sont pas une commande de ce profil, commande impossible dans l'état présent (`BO` en session, toute commande en état dormant), saisie vide terminée par ‹X-OFF›, ou correction de l'éditeur de ligne mal formée |
| `PARAMETRE DE COMMANDE ERRONE` | paramètre absent ou mal formé : numéro de ligne manquant, nom de fichier incorrect, liste mal écrite |
| `LIGNE DE DEPART INEXISTANTE` | `EXECUTER A PARTIR DE n` ou `REPRENDRE` vers une ligne qui n'existe pas |
| `CONTINUER IMPOSSIBLE` | le programme n'est pas suspendu, ou une ligne a été tapée depuis l'arrêt (paragraphe 8.8) ; utilisez `EXECUTER` ou `REPRENDRE` |
| `REPRENDRE IMPOSSIBLE` | rien n'a été exécuté, ou une ligne a été tapée depuis l'arrêt ; utilisez `EXECUTER` |
| `CONSOLE INEXISTANTE` | `PRENDRE ETAT CONSOLE` avec un numéro autre que 1 |
| `IDENTIFICATION ILLEGALE` | `IDENTIFICATION` d'un numéro qui n'est pas défini ; `CREATION ID` de 0 ou de 1 |

### A.3 Erreurs de compilation : `ERREUR  C nn`

La ligne n'est pas entrée dans le programme ; l'éventuelle ligne de même numéro reste inchangée. Corrigez-la (par l'éditeur de ligne, paragraphe 7.5) ou retapez-la.

| Code | Explication | Cause et remède |
|---|---|---|
| C 01 | `LETTRE MINUSCULE HORS D'UNE CHAINE` | enclenchez Caps Lock : seules les chaînes et les commentaires admettent les minuscules |
| C 01 | `CARACTERE SPECIAL INTERDIT` | un caractère qui n'a pas de sens ici (`"`, `{`, `%`…) |
| C 03 | `IDENTIFICATEUR DE PLUS DE 5 CARACTERES` | un nom a plus de cinq caractères ; c'est souvent un mot du langage mal orthographié (`AFICHER`) ou deux mots collés (`SIA`) |
| C 05 | `NOM DE PROCEDURE DE PLUS DE 5 CARACTERES` | après `&`, au plus cinq caractères |
| C 06 | `NOM DE PROCEDURE INCORRECT APRES &` | `&` doit être suivi d'une lettre |
| C 07 | `NUMERO DE LIGNE NON SUIVI D'UN ESPACE` | mettez un espace après le numéro |
| C 08 | `APOSTROPHE DE FIN DE CHAINE MANQUANTE` | une chaîne n'est pas refermée ; une apostrophe dans une chaîne s'écrit `''` |
| C 08 | `POINT DE FIN D'EQUIVALENT NUMERIQUE MANQUANT` | notation `.n.` incomplète (LSE F4) ; un nombre comme `.5` doit s'écrire `0.5` |
| C 09 | `CONSTANTE NUMERIQUE INCORRECTE` | nombre mal écrit (`1E`, `2..5`) |
| C 09 | `CONSTANTE NUMERIQUE TROP GRANDE` | au-delà de 3,4 × 10³⁸ |
| C 11 | `COMPARAISON D'UN NOMBRE ET D'UNE CHAINE` | on ne compare que deux nombres ou deux chaînes |
| C 14 | `DIMENSIONS DE TABLEAU INCORRECTES` | `TABLEAU T[…]` : une ou deux dimensions entre crochets |
| C 14 | `DIMENSION DE TABLEAU NON NUMERIQUE` | une dimension est une chaîne |
| C 15 | `INDICE NON NUMERIQUE` | un indice est une chaîne |
| C 16 | `CROCHET FERMANT MANQUANT` | fermez le crochet |
| C 16 | `PLUS DE DEUX INDICES` | les tableaux ont une ou deux dimensions |
| C 17 | `OPERATEUR DE RELATION INCORRECT OU ABSENT` | une condition s'écrit avec `<`, `<=`, `=`, `#`, `>=`, `>` ; « différent » s'écrit `#` |
| C 19 | `OPERATEUR MANQUANT ENTRE DEUX OPERANDES` | `2X` ou `(A)(B)` : écrivez `2*X` |
| C 19 | `OPERANDE MANQUANT` | une expression incomplète (`A+`) |
| C 19 | `DEUX OPERATEURS ADJACENTS` | `A*-B` : écrivez `A*(-B)` |
| C 20 | `PARENTHESE FERMANTE MANQUANTE` | comptez les parenthèses |
| C 21 | `SINON MANQUANT DANS UNE EXPRESSION CONDITIONNELLE` | dans une expression, `SI … ALORS …` doit avoir son `SINON` |
| C 22 | `ALORS MANQUANT` | `SI condition ALORS instruction` |
| C 26 | `NOMBRE D'ARGUMENTS INCORRECT POUR CETTE FONCTION` | voir le paragraphe 5.17 ou 5.18 |
| C 26 | `DERNIER ARGUMENT DE SCH, GRL OU CNB : IDENTIFICATEUR ATTENDU` | le dernier argument facultatif reçoit une position : ce doit être un nom de variable |
| C 27 | `CHAINE AU LIEU D'UN NOMBRE` | une chaîne là où il faut un nombre (`'A'+1`) ; convertissez par `CNB` |
| C 28 | `NOMBRE AU LIEU D'UNE CHAINE` | un nombre là où il faut une chaîne (`1!'A'`) ; convertissez par `CCA` |
| C 29 | `ERREUR DE PARAMETRE DANS GARER, CHARGER OU SUPPRIMER` | ces instructions ont trois paramètres (quatre pour `CHARGER` avec compte rendu) |
| C 32 | `POUR MANQUANT DANS FAIRE` | `FAIRE n POUR V←…` |
| C 32 | `LA VARIABLE DE BOUCLE DOIT ETRE SIMPLE` | pas d'élément de tableau comme variable de boucle |
| C 33 | `← MANQUANT APRES LA VARIABLE DE BOUCLE` | `POUR I←1` |
| C 34 | `JUSQUA OU TANT QUE MANQUANT` | une boucle `FAIRE` finit par `JUSQUA` ou `TANT QUE` (`JUSQUA` s'écrit sans apostrophe) |
| C 34 | `TANT NON SUIVI DE QUE` | `TANT QUE` |
| C 35 | `NOM DE VARIABLE INCORRECT DANS CHAINE OU LIBERER` | liste de noms séparés par des virgules |
| C 35 | `NOM DE PROCEDURE DANS LA LISTE LOCAL` | `LOCAL` ne contient que des noms de variables |
| C 36 | `';' OU FIN MANQUANT DANS DEBUT...FIN` | séparez les instructions par `;` et fermez par `FIN`, sur la même ligne |
| C 37 | `ALLER NON SUIVI DE EN` | `ALLER EN n` |
| C 38 | `INSTRUCTION INCONNUE OU ← MANQUANT` | mot du langage mal orthographié, ou affectation sans flèche (`A=3` au lieu de `A←3`) |
| C 38 | `LA CIBLE D'UNE AFFECTATION DOIT ETRE UNE VARIABLE` | à gauche de `←`, un nom ou un élément de tableau |
| C 39 | `NOM DE TABLEAU INCORRECT OU ABSENT` | dans `TABLEAU`, `UN` ou `ZER` |
| C 40 | `']' MANQUANT DANS UN FORMAT` | fermez le crochet du format |
| C 41 | `FACTEUR DE REPETITION NUL` | un facteur de répétition vaut au moins 1 |
| C 41 | `FACTEUR DE REPETITION AU-DELA DE LA BORNE DE CETTE MACHINE` | au plus 62 (99 sous MITRA 15) |
| C 41 | `FACTEUR DE REPETITION SUPERIEUR A 99` | au plus 99 |
| C 42 | `SPECIFICATION DE FORMAT INCORRECTE` | voir le paragraphe 5.16 |
| C 42 | `E OU D DE PLUS DE DEUX CHIFFRES DANS UN FORMAT` | `Fe.d` et `Ee.d` : e et d de 0 à 99 |
| C 42 | `'*' INTERDIT DEVANT UNE DESCRIPTION` | `*` ne se place que devant un libellé ou une mise en page |
| C 44 | `NOM DE VARIABLE INCORRECT OU ABSENT DANS GARER OU CHARGER` | le premier paramètre est un nom |
| C 45 | `PLUS DE ')' QUE DE '('` | parenthèse fermante en trop |
| C 46 | `LISTE DE PARAMETRES INCORRECTE` | dans une déclaration de procédure |
| C 46 | `TEXTE INATTENDU APRES LA DECLARATION DE PROCEDURE` | après la déclaration, `LOCAL …` ou `;` |
| C 46 | `APPEL DE PROCEDURE MAL FORME` | `&P(a,b)` |
| C 47 | `DEUX PARAMETRES IDENTIQUES` | dans une déclaration de procédure |
| C 47 | `DEUX NOMS IDENTIQUES DANS LA LISTE LOCAL` | |
| C 48 | `DEUX PARAMETRES PROCEDURE IDENTIQUES` | |
| C 48 | `PARAMETRE IDENTIQUE AU NOM DE LA PROCEDURE` | |
| C 49 | `NOM DE PROCEDURE INCORRECT OU ABSENT` | `PROCEDURE &NOM(…)` |
| C 49 | `NOM DE PROCEDURE NON SUIVI DE ()` | les parenthèses sont obligatoires, même vides : `&P()` |
| C 51 | `PARAMETRE FORMEL INDICE` | un paramètre est un nom simple, pas un élément de tableau |
| C 52 | `NUMERO DE LIGNE NUL OU SUPERIEUR A 32000` | numéro de ligne incorrect |
| C 53 | `MOT DU LANGAGE MAL TERMINE` | un mot doit être suivi d'un espace (`SI(A>0)` : écrivez `SI A>0`) |
| C 53 | `INSTRUCTION INCORRECTE` | texte en trop, ou instruction mal formée |
| C 54 | `PLUS DE DESCRIPTIONS QUE DE VALEURS` | le format décrit plus de valeurs qu'il n'y en a dans la liste |
| C 55 | `DEBUT D'INSTRUCTION INCORRECT` | une instruction ne peut pas commencer ainsi |
| C 57 | `PLUS DE PLACE DANS LA ZONE UTILISATEUR` | la mémoire est pleine : rangez le programme, effacez des lignes, libérez des tableaux |
| C 58 | `PROCEDURE INTERDITE EN MODE MACHINE DE BUREAU` et six autres explications | une instruction interdite en mode machine de bureau (paragraphe 6.3) : `ALLER EN`, `FAIRE`, `RETOUR`, `RESULTAT`, appels de procédures, `EXECUTER`, et déclarations sous MITRA 15 |
| C 63 | `LIGNE DE PLUS DE 80 CARACTERES` | une ligne compte au plus 79 caractères : répartissez le travail sur deux lignes |
| C 93 | `@ NON SUIVI D'UN ENTIER` | (MITRA 15) `@50` |
| C 100 | `NUMERO DE LIGNE AU-DELA DE LA BORNE DE CETTE MACHINE` | au plus 255 (250 sous MITRA 15) |
| C 101 | `MOT RESERVE EMPLOYE COMME IDENTIFICATEUR` | un mot réservé là où il faut un nom ; ou une expression `SI` après un opérateur, à mettre entre parenthèses (paragraphe 5.7) |
| C 102 | `MOT RESERVE DE CETTE MACHINE EMPLOYE COMME IDENTIFICATEUR` | `TEM`, `DAT` (LSE F4 et T1600), `SUPPRIMER`, `REPRENDRE` (MITRA 15) |
| C 103 | `CONSTRUCTION ABSENTE DE CETTE MACHINE` | une forme propre à un autre profil (annexe D) |
| C 104 | `LA CIBLE DE LIRE DOIT ETRE UNE VARIABLE` | `LIRE` est suivi de noms |
| C 105 | `EXECUTER : DEUX ARGUMENTS AU PLUS` | `EXECUTER nom[,ligne]` |
| C 106 | `? RESERVE AU MODE MACHINE DE BUREAU` | dans un programme, écrivez `AFFICHER` |
| C 107 | `LIGNE TROP COMPLEXE` | trop de parenthèses imbriquées ou une ligne trop chargée : décomposez |
| C 108 | `TROP DE NOMS OU DE CONSTANTES` | le programme utilise trop de noms (6 553) ou de chaînes constantes (21 845) |
| C 109 | `EQUIVALENT NUMERIQUE HORS DES BORNES DE EQC` | dans `.n.`, n va de 0 à 127 |

### A.4 Erreurs d'exécution : `ERREUR (EX.) nn`

Le programme s'arrête. Examinez les variables en mode machine de bureau, corrigez, puis relancez (`EXECUTER`) ou reprenez (`REPRENDRE`).

| Code | Explication | Cause et remède |
|---|---|---|
| EX 03 | `MEMOIRE SATUREE` | trop de données, ou une récursivité sans fin |
| EX 04 | `DECLARATION CHAINE D'UNE VARIABLE OU TABLEAU` | `CHAINE` sur un nom déjà employé comme nombre ou tableau : `LIBERER` d'abord, ou changez de nom |
| EX 05 | `VARIABLE NON DEFINIE DANS UN CALCUL` | une variable n'a pas reçu de valeur avant d'être utilisée |
| EX 05 | `CHAINE OU TABLEAU DANS UN CALCUL` | un nom de chaîne ou de tableau dans un calcul numérique |
| EX 06 | `CHAINE RANGEE DANS UNE VARIABLE NON CHAINE` | déclarez la variable par `CHAINE` |
| EX 06 | `NOMBRE RANGE DANS UNE CHAINE OU UN TABLEAU` | une variable-chaîne ne reçoit qu'une chaîne |
| EX 06 | `VALEUR CONTRAIRE A LA DESCRIPTION DU FORMAT` | une chaîne affichée sous `F` ou `E` : utilisez `U` |
| EX 08 | `OPERANDE NON CHAINE DANS UNE EXPRESSION CHAINE` | `!` relie deux chaînes : convertissez le nombre par `CCA` |
| EX 09 | `ARGUMENT DE ALE NEGATIF OU SUPERIEUR A 1` | `ALE(x)` avec x de 0 à 1 |
| EX 10 | `VARIABLE NON DEFINIE DANS UNE COMPARAISON` | |
| EX 10 | `COMPARAISON D'UN NOMBRE ET D'UNE CHAINE` | |
| EX 11 | `POSITION DE DEPART ERRONEE` | dans une fonction sur les chaînes, la position va de 1 à la longueur + 1 |
| EX 12 | `EXECUTION D'UNE LIGNE DE DECLARATION` | le programme est arrivé sur une ligne `PROCEDURE` : placez `TERMINER` avant les procédures |
| EX 13 | `NOMBRE DE PARAMETRES EFFECTIFS INCORRECT` | l'appel n'a pas autant de paramètres que la déclaration |
| EX 14 | `PARAMETRE LOCAL SANS VALEUR` | une variable sans valeur passée à un paramètre `LOCAL` |
| EX 15 | `RETOUR SANS RESULTAT DANS UNE FONCTION` | une procédure appelée dans une expression doit finir par `RESULTAT` |
| EX 17 | `LONGUEUR DEMANDEE NEGATIVE` | `SCH(C,P,N)` avec N négatif |
| EX 17 | `TROISIEME ARGUMENT NI CHAINE NI NOMBRE` | |
| EX 18 | `DEUXIEME ARGUMENT NON NUMERIQUE` | la position est un nombre |
| EX 20 | `RETOUR OU RESULTAT HORS PROCEDURE` | `RETOUR` ou `RESULTAT` exécuté hors d'une procédure |
| EX 21 | `POSITION ERRONEE DANS EQN` | |
| EX 22 | `RESULTAT DANS UNE PROCEDURE SOUS-PROGRAMME` | une procédure appelée comme instruction finit par `RETOUR` |
| EX 25 | `ZERO ELEVE A UNE PUISSANCE NEGATIVE` | |
| EX 26 | `NOMBRE NEGATIF A UNE PUISSANCE NON ENTIERE` | |
| EX 26 | `POSITION SUPERIEURE A LA LONGUEUR + 1` | dans `SCH`, `PTR` ou `CNB` |
| EX 27 | `INDICE NUL OU NEGATIF` | les indices commencent à 1 |
| EX 27 | `DIMENSION DE TABLEAU NULLE OU NEGATIVE` | |
| EX 29 | `DECLARATION TABLEAU D'UNE VARIABLE OU CHAINE` | `TABLEAU` sur un nom déjà employé autrement : `LIBERER` d'abord |
| EX 29 | `TABLEAU PARAMETRE DECLARE DE NOUVEAU` | ne redéclarez pas dans une procédure un tableau reçu en paramètre |
| EX 30 | `ARGUMENT DE EQC HORS BORNES` | de 0 à 127 (255 sous MITRA 15) |
| EX 31 | `DIVISION PAR ZERO` | |
| EX 32 | `ELEMENT DE TABLEAU SANS VALEUR` | un élément utilisé avant d'avoir reçu une valeur |
| EX 37 | `NUMERO DE LIGNE DE BRANCHEMENT NUL` | `ALLER EN` ou `RETOUR EN` vers un numéro inférieur à 1 |
| EX 37 | `FIN DE BOUCLE NULLE OU NEGATIVE` | le numéro de fin d'un `FAIRE` |
| EX 38 | `NUMERO DE LIGNE NON CONVERTIBLE EN ENTIER` | numéro calculé trop grand |
| EX 41 | `RAC D'UN NOMBRE NEGATIF` | |
| EX 42 | `LGN D'UN NOMBRE NEGATIF OU NUL` | |
| EX 43 | `VARIABLE DE BOUCLE NON NUMERIQUE` | la variable d'un `FAIRE` est une chaîne |
| EX 44 | `DEPASSEMENT DE CAPACITE ARITHMETIQUE` | un résultat dépasse 3,4 × 10³⁸ |
| EX 45 | `TABLEAU DE PLUS DE 65535 ELEMENTS` | |
| EX 46 | `NOMBRE D'INDICES NON CONFORME A LA DECLARATION` | `T[I]` pour un tableau à deux dimensions, ou l'inverse |
| EX 47 | `INDICES SUR UN IDENTIFICATEUR NON TABLEAU` | `T[I]` alors que `T` n'est pas un tableau (ou n'a pas été déclaré) |
| EX 47 | `CIBLE DE CHARGER D'UN AUTRE TYPE` | `CHARGER` dans une variable d'une autre sorte que l'enregistrement : `LIBERER` d'abord |
| EX 49 | `INDICE SUPERIEUR A LA BORNE DECLAREE` | |
| EX 50 | `NUMERO D'ENREGISTREMENT INCORRECT` | un entier de 1 à 32 767 |
| EX 52 | `PLUS DE PLACE DANS LE FICHIER` | espace temporaire plein (option `-TEMP`), disque plein, ou ruban impossible à écrire |
| EX 53 | `NOM DE FICHIER ERRONE` | une lettre suivie d'au plus quatre lettres ou chiffres, `#` devant pour un fichier permanent |
| EX 54 | `FICHIER PROTEGE PAR UNE AUTRE IDENTIFICATION` | fichier `L` d'un autre compte |
| EX 55 | `FICHIER INEXISTANT` | utilisez le compte rendu de `CHARGER` pour le tester |
| EX 56 | `ENREGISTREMENT INEXISTANT` | idem |
| EX 56 | `COMPTE RENDU NON NUMERIQUE` | le quatrième paramètre de `CHARGER` est une variable numérique |
| EX 58 | `FIN DU PROGRAMME SANS TERMINER` | le programme est arrivé au bout sans rencontrer `TERMINER` |
| EX 59 | `RESULTAT QUI N'EST NI UN NOMBRE NI UNE CHAINE` | `RESULTAT` d'un tableau |
| EX 62 | `VARIABLE NON DEFINIE AFFECTEE OU AFFICHEE` | une variable sans valeur |
| EX 62 | `VARIABLE-CHAINE SANS VALEUR` | une chaîne déclarée qui n'a pas reçu de valeur |
| EX 72 | `EXPRESSION PASSEE A UN PARAMETRE REFERENCE` | un paramètre qui n'est pas `LOCAL` reçoit un nom de variable, pas une expression ; ajoutez-le à `LOCAL` |
| EX 75 | `LIGNE DE BRANCHEMENT INEXISTANTE` | `ALLER EN`, `RETOUR EN` ou `EXECUTER` vers une ligne qui n'existe pas |
| EX 96 | `PROBLEME INTERNE A LSE` | ne devrait pas se produire : notez ce que vous faisiez |
| EX 100 | `LIBERER D'UN IDENTIFICATEUR NON LOCAL` | dans une procédure, on ne libère que ses noms locaux |
| EX 101 | `CHAINE TROP LONGUE` | au-delà de 65 535 caractères |
| EX 102 | `FIN DE BOUCLE AVANT LA LIGNE FAIRE` | le numéro de fin d'un `FAIRE` est plus petit que celui de sa ligne |
| EX 103 | `PROCEDURE NON DECLAREE` | appel d'une procédure qui n'a pas de ligne `PROCEDURE` |
| EX 104 | `FACTEUR DE REPETITION NEGATIF` | facteur variable `*` négatif |
| EX 105 | `MODULE INTROUVABLE POUR EXECUTER` | `EXECUTER` d'un programme qui n'est pas rangé |

### A.5 Erreurs de fichiers : `ERREUR F nn`

| Code | Explication | Cause et remède |
|---|---|---|
| F 02 | `SUPPORT PLEIN` | disque plein, ou écriture refusée (disquette protégée) |
| F 03 | `PROGRAMME DEJA EXISTANT SUR LE SUPPORT` | un programme porte déjà ce nom : `MODIFIER` pour le remplacer, ou un autre nom ; sous `-MITRA15`, aussi pour `CATALOGUER` ou `GARER` vers ce nom |
| F 04 | `FICHIER DE DONNEES PERMANENT DEJA EXISTANT` | un fichier-données permanent porte déjà ce nom (`CATALOGUER` ; sous `-MITRA15`, aussi `RANGER`) |
| F 05 | `FICHIER INEXISTANT` | vérifiez le nom (`TABLE DES FICHIERS`) |
| F 06 | `FICHIER PROGRAMME PROTEGE PAR UNE IDENTIFICATION` | fichier protégé `L` d'un autre compte, ou programme secret (paragraphe 11.9) |
| F 07 | `DROIT DE CREATION DE PROGRAMME ABSENT` | il faut l'attribut `P` : `IDENTIFICATION` |
| F 08 | `DROIT DE CREATION DE DONNEES PERMANENTES ABSENT` | il faut l'attribut `D` |
| F 10 | `IDENTIFICATION INEXISTANTE` | `SUPPRESSION ID` d'une identification inconnue |
| F 11 | `DROIT DE CREATION DE FICHIER SECRET ABSENT` | `RANGER nom,E` demande l'attribut `S` |
| F 19 | `CATALOGUE SATURE` | plus de neuf identifications |
| F 22 | `REMPLACEMENT SOUS UNE MAUVAISE IDENTIFICATION` | le fichier appartient à un autre compte |
| F 23 | `MEMOIRE TROP PETITE POUR LE PROGRAMME` | le programme rangé ne tient pas en mémoire |
| F 27 | `OPERATION D'ENTREE-SORTIE IMPOSSIBLE` | fichier verrouillé ou inaccessible |
| F 29 | `NUMERO D'UNITE INCORRECT` | `$` ou `DUPLIQUER` sans option `-UNITE` |
| F 100 | `FICHIER RANGE SOUS UN AUTRE PROFIL` | voir le paragraphe 10.11 |
| F 101 | `FICHIER ENDOMMAGE` | le fichier a été abîmé ou modifié hors de LSE ; il est refusé, sans rien changer en mémoire |
| F 102 | `BIBLIOTHEQUE INACCESSIBLE` | voir le paragraphe 2.6 |

Les codes F 13, F 17 et F 25 de la liste d'origine ne sont jamais produits par LSE-IIGS.

### A.6 Messages du lancement

| Message | Cause |
|---|---|
| `OPTION INCORRECTE : option valeur` | option inconnue, sans valeur, ou valeur incorrecte (paragraphe 2.3) ; LSE rend la main au shell |
| `ERREUR F 102` | la bibliothèque est inaccessible (paragraphe 2.6) |
| `Stack overflow` ou un autre message en anglais, suivi du retour au shell | erreur grave signalée par le système ; le programme en mémoire est perdu |

---

## Annexe B. Grammaire

Cette annexe décrit précisément la forme de tout ce que LSE accepte. Elle s'adresse au lecteur qui veut lever un doute ; les chapitres 5 à 9 suffisent pour programmer.

### B.1 Notation

| Notation | Sens |
|---|---|
| *nom* | un élément défini dans cette annexe |
| `TEXTE` | à écrire tel quel |
| `[ x ]` | x est facultatif |
| `{ x }` | x peut être répété, zéro, une ou plusieurs fois |
| `x \| y` | x ou y |
| ⟨F4⟩, ⟨T⟩, ⟨M⟩ | forme propre au profil LSE F4, aux profils LSE F4 et T1600, ou au profil MITRA 15 |

Les espaces sont libres entre les éléments, sauf : à l'intérieur d'un nom, d'un nombre ou d'un mot ; après un mot du langage (paragraphe 5.2) ; entre `AFFICHER` et son crochet ; entre les morceaux d'une chaîne ⟨F4⟩.

### B.2 Éléments lexicaux

```text
lettre          ::= A | B | … | Z
chiffre         ::= 0 | 1 | … | 9
nom             ::= lettre { lettre | chiffre }          (5 caractères au plus,
                                                           pas un mot réservé)
nom-procédure   ::= & nom
entier          ::= chiffre { chiffre }
nombre          ::= entier [ . { chiffre } ] [ E [ + | - ] entier ]
chaîne          ::= morceau-texte
                  | morceau { morceau }                    ⟨F4⟩
morceau         ::= morceau-texte | équivalent
morceau-texte   ::= ' { caractère | '' } '
équivalent      ::= . entier { espace { espace } entier } .        ⟨F4⟩
commentaire     ::= * { caractère }
```

- Un nombre commence toujours par un chiffre : `0.5`, et non `.5`.
- Dans `équivalent`, chaque entier va de 0 à 127.
- Une minuscule n'est permise que dans une chaîne et dans un commentaire.

**Mots réservés** : `AFFICHER ALLER ALORS CHAINE CHARGER DEBUT ET EXECUTER FAIRE FIN GARER JUSQUA LIBERER LIRE LOCAL OU PAS PAUSE POUR PROCEDURE RESULTAT RETOUR SI SINON TABLEAU TANT TERMINER`, plus `DAT TEM` ⟨T⟩, plus `SUPPRIMER REPRENDRE` ⟨M⟩.

### B.3 Lignes

```text
ligne-de-programme ::= numéro espace { espace } [ corps ] fin
                     | numéro [ espace ] commentaire fin
numéro             ::= entier                 (1 à 255 ; 1 à 250 ⟨M⟩)
corps              ::= déclaration-procédure { ; instruction }
                     | instruction { ; instruction }
ligne-de-bureau    ::= instruction-de-bureau { ; instruction-de-bureau } fin
fin                ::= X-OFF | RETURN
```

Une ligne compte au plus 79 caractères avant sa fin.

### B.4 Instructions

```text
instruction ::= vide
              | commentaire
              | affectation
              | SI condition ALORS instruction [ SINON instruction ]
              | DEBUT instruction { ; instruction } FIN
              | ALLER EN expression
              | faire
              | PAUSE
              | TERMINER
              | LIRE cible { , cible }
              | AFFICHER [ format ] [ expression { , expression } ]
              | TABLEAU nom [ expression [ , expression ] ]
                    { , nom [ expression [ , expression ] ] }
              | CHAINE nom { , nom }
              | LIBERER nom { , nom }
              | GARER nom , expression , expression
              | CHARGER nom , expression , expression [ , nom ]
              | SUPPRIMER expression [ , expression ]               ⟨M⟩
              | EXECUTER expression [ , expression ]
              | RETOUR [ EN expression ]
              | RESULTAT expression
              | appel-procédure
              | UN nom | ZER nom                                   ⟨T⟩

affectation ::= cible ← expression
cible       ::= nom [ [ expression [ , expression ] ] ]

faire       ::= FAIRE expression POUR nom ← expression [ PAS expression ] JUSQUA expression
              | FAIRE expression POUR nom ← expression [ PAS expression ] TANT QUE condition
              | FAIRE expression TANT QUE condition                  ⟨F4⟩
```

Dans `TABLEAU`, les crochets entourent les dimensions ; dans `cible`, les indices.

**Instructions de machine de bureau.** Ce sont les mêmes, sauf `ALLER EN`, `FAIRE`, `RETOUR`, `RESULTAT`, `EXECUTER`, les appels de procédures, et, sous MITRA 15, `TABLEAU`, `CHAINE` et `LIBERER`. S'y ajoute :

```text
instruction-de-bureau ::= ? [ format ] [ expression { , expression } ]
```

### B.5 Procédures

```text
déclaration-procédure ::= PROCEDURE nom-procédure ( [ formel { , formel } ] )
                              [ LOCAL nom { , nom } ]
formel                ::= nom | nom-procédure
appel-procédure       ::= nom-procédure ( [ effectif { , effectif } ] )
effectif              ::= expression | nom-procédure
```

La déclaration est en tête de ligne. Les parenthèses sont obligatoires, même vides.

### B.6 Expressions

```text
expression  ::= concaténation
              | SI condition ALORS expression SINON expression
concaténation ::= somme { ! somme }
somme       ::= [ + | - ] produit { ( + | - ) produit }
produit     ::= puissance { ( * | / ) puissance }
puissance   ::= primaire { ↑ primaire }
primaire    ::= nombre
              | chaîne
              | cible
              | fonction
              | appel-procédure
              | ( expression )
              | @                          (numéro de la ligne en cours) ⟨T⟩
              | $                          (ligne d'appel de la procédure) ⟨T⟩
              | @ entier                   (numéro de ligne) ⟨M⟩
```

- Les opérateurs d'une même règle s'appliquent de gauche à droite : `2↑3↑2` vaut `(2↑3)↑2`, soit 64.
- Le signe placé en tête d'une somme porte sur le produit qui suit : `-2↑2` vaut −4.
- Une expression `SI … ALORS … SINON` placée après un opérateur se met entre parenthèses.
- Les types (nombre ou chaîne) doivent concorder : `!` relie des chaînes, les autres opérateurs des nombres.

### B.7 Fonctions

```text
fonction ::= SIN ( e ) | COS ( e ) | ATG ( e ) | LGN ( e ) | EXP ( e )
           | RAC ( e ) | ABS ( e ) | ENT ( e ) | ALE ( e )
           | SH ( e ) | CH ( e ) | TH ( e ) | SGN ( e )              ⟨T⟩
           | LGR ( e ) | CCA ( e ) | EQC ( e )
           | SCH ( e , e , e [ , nom ] )
           | GRL ( e , e [ , nom ] )
           | POS ( e , e , e )
           | CNB ( e , e [ , nom ] )
           | EQN ( e [ , e ] )
           | PTR ( e , e [ , e ] )
           | SKP ( e , e [ , e ] )
           | TEM | DAT                                                ⟨T⟩
           | TEM ( ) | DAT ( )                                       ⟨F4⟩ ⟨M⟩
```

Ici `e` désigne une *expression*.

### B.8 Conditions

```text
condition ::= terme-ou { OU terme-ou }
terme-ou  ::= terme-et { ET terme-et }
terme-et  ::= ( condition ) | expression relation expression
relation  ::= < | <= | = | # | >= | >
```

### B.9 Formats

```text
format        ::= [ spécification { , spécification } ]
spécification ::= [ répétition ] chaîne                 (libellé)
                | [ répétition ] mise-en-page
                | [ entier ] description
répétition    ::= entier | *
mise-en-page  ::= / | X | C | L
description   ::= U | F entier . entier | E entier . entier
```

- Une répétition fixe va de 1 à 62 (1 à 99 ⟨M⟩).
- Dans `F` et `E`, chaque entier a au plus deux chiffres.
- `*` ne se met pas devant une description.
- `[` suit immédiatement `AFFICHER` ou `?`.

### B.10 Langage de commande

Une commande se tape par ses deux lettres, suivies de la fin de saisie ; ses paramètres se tapent ensuite, dans une seconde saisie.

```text
paramètres-lignes   ::= numéro [ , numéro ]                 (LI, NU, PE, EX, RE)
paramètres-effacer  ::= * | élément { , élément }
élément             ::= numéro [ A numéro ]
nom-fichier         ::= [ $ ] nom                           ($ : ⟨F4⟩)
paramètres-ranger   ::= nom-fichier [ , E | , L ]           (E, L : ⟨T⟩)
paramètres-supprimer ::= nom-fichier , ( P | D | T ) | *     ⟨T⟩
                       | nom                                 ⟨M⟩
paramètres-cataloguer ::= nom , nom-fichier [ , L ]          (L : ⟨T⟩)
paramètres-coder    ::= nom-fichier [ , numéro [ , numéro ] ]   (DE, EN)
paramètres-table    ::= [ $ ] ( * | nom ) [ , ( P | D | T ) ]   (TA, UT)
                      | ( A | nom ) [ , ( P | D | T ) ]          ⟨M⟩
paramètres-dupliquer ::= [ $ ] motif [ , ( P | D ) ]         ⟨F4⟩
motif               ::= nom | début-de-nom * | *
paramètres-création ::= entier [ , P ] [ , D ] [ , S ] [ , ! ] [ , > ] [ , X | , entier ]
```

---

## Annexe C. Table des caractères

### C.1 Les caractères imprimables

LSE emploie le code ASCII. `EQC(n)` donne le caractère de code `n`, `EQN(c)` le code du caractère `c`. Sous LSE F4, la notation `.n.` l'insère dans une chaîne constante.

| Code | Car. | Code | Car. | Code | Car. | Code | Car. |
|---|---|---|---|---|---|---|---|
| 32 | espace | 56 | `8` | 80 | `P` | 104 | `h` |
| 33 | `!` | 57 | `9` | 81 | `Q` | 105 | `i` |
| 34 | `"` | 58 | `:` | 82 | `R` | 106 | `j` |
| 35 | `#` | 59 | `;` | 83 | `S` | 107 | `k` |
| 36 | `$` | 60 | `<` | 84 | `T` | 108 | `l` |
| 37 | `%` | 61 | `=` | 85 | `U` | 109 | `m` |
| 38 | `&` | 62 | `>` | 86 | `V` | 110 | `n` |
| 39 | `'` | 63 | `?` | 87 | `W` | 111 | `o` |
| 40 | `(` | 64 | `@` | 88 | `X` | 112 | `p` |
| 41 | `)` | 65 | `A` | 89 | `Y` | 113 | `q` |
| 42 | `*` | 66 | `B` | 90 | `Z` | 114 | `r` |
| 43 | `+` | 67 | `C` | 91 | `[` | 115 | `s` |
| 44 | `,` | 68 | `D` | 92 | `\` | 116 | `t` |
| 45 | `-` | 69 | `E` | 93 | `]` | 117 | `u` |
| 46 | `.` | 70 | `F` | 94 | `↑` (touche `^`) | 118 | `v` |
| 47 | `/` | 71 | `G` | 95 | `←` (touche `_`) | 119 | `w` |
| 48 | `0` | 72 | `H` | 96 | accent grave | 120 | `x` |
| 49 | `1` | 73 | `I` | 97 | `a` | 121 | `y` |
| 50 | `2` | 74 | `J` | 98 | `b` | 122 | `z` |
| 51 | `3` | 75 | `K` | 99 | `c` | 123 | `{` |
| 52 | `4` | 76 | `L` | 100 | `d` | 124 | `\|` |
| 53 | `5` | 77 | `M` | 101 | `e` | 125 | `}` |
| 54 | `6` | 78 | `N` | 102 | `f` | 126 | `~` |
| 55 | `7` | 79 | `O` | 103 | `g` | 127 | DEL (RUB-OUT) |

Les codes 94 et 95 s'affichent en flèches à l'écran de LSE-IIGS (paragraphe 4.5). Les chaînes se comparent selon ces codes (paragraphe 5.9) : les chiffres viennent avant les majuscules, les majuscules avant les minuscules, et l'espace avant tout autre caractère imprimable.

### C.2 Les caractères de commande

Les codes 0 à 31 ne s'impriment pas : ce sont des ordres. Écrits par un programme (par `AFFICHER` d'une chaîne qui les contient), ils agissent sur l'écran de l'Apple IIGS comme l'indique la troisième colonne. Ces effets sont propres à l'écran de l'Apple IIGS et n'existaient pas sur les consoles d'origine. Certains codes prennent les caractères qui les suivent comme valeurs : n'employez ceux-là qu'à bon escient.

| Code | Nom | Effet à l'écran de l'Apple IIGS | Rôle dans LSE |
|---|---|---|---|
| 0 | NUL | aucun | fin d'un fichier décodé ; ignoré à la lecture du ruban |
| 1 | SOH | sauvegarde la fenêtre de texte | |
| 2 | STX | définit la fenêtre de texte (4 caractères suivent) | |
| 3 | ETX | efface du début de la ligne jusqu'au curseur | |
| 4 | EOT | rétablit la fenêtre de texte sauvegardée | |
| 5 | ENQ | décale la fenêtre horizontalement (1 caractère suit) | |
| 6 | ACK | place le curseur sur une ligne (1 caractère suit) | |
| 7 | BEL | sonnerie | le bip |
| 8 | BS | recule le curseur d'une position | |
| 9 | HT | aucun | |
| 10 | LF | descend d'une ligne | interligne ; ignoré à la lecture du ruban |
| 11 | VT | efface du curseur jusqu'à la fin de l'écran | |
| 12 | FF | efface l'écran et place le curseur en haut à gauche | |
| 13 | CR | ramène le curseur au début de la ligne | retour chariot ; termine une ligne de ruban |
| 14 | SO | affichage normal | |
| 15 | SI | affichage en vidéo inverse | |
| 16 | DLE | | |
| 17 | X-ON (DC1) | jamais envoyé à l'écran | met en marche le lecteur de ruban, frappé au clavier (Ctrl-Q) |
| 18 | DC2 | jamais envoyé à l'écran | met en marche le perforateur (paragraphe 12.8) |
| 19 | X-OFF (DC3) | jamais envoyé à l'écran | fin de saisie (Ctrl-S) ; sépare les lignes d'un fichier décodé |
| 20 | DC4 | jamais envoyé à l'écran | arrête le perforateur |
| 21 | NAK | jamais envoyé à l'écran | |
| 22 | SYN | fait défiler le texte vers le bas | |
| 23 | ETB | fait défiler le texte vers le haut | |
| 24 | CAN | | |
| 25 | EM | place le curseur en haut à gauche | |
| 26 | SUB | efface la ligne du curseur | |
| 27 | ESC | | touche d'interruption ; arrête le lecteur de ruban |
| 28 | FS | avance le curseur d'une position | |
| 29 | GS | efface du curseur jusqu'à la fin de la ligne | |
| 30 | RS | place le curseur (2 caractères suivent : colonne + 32, puis ligne + 32) | |
| 31 | US | remonte le curseur d'une ligne | |

Exemples, sous LSE F4 :

```text
AFFICHER .12.                       efface l'écran
AFFICHER .15.'ATTENTION'.14.        écrit ATTENTION en vidéo inverse
AFFICHER .30 42 37.'ICI'            écrit ICI en colonne 10, ligne 5 (en comptant de 0)
```

Sous les autres profils, on écrit ces caractères avec `EQC` : `AFFICHER EQC(12)`.

### C.3 Les codes 128 à 255

Sous MITRA 15, `EQC` accepte les codes 128 à 255. Écrits à l'écran, ils sont traités comme leur code diminué de 128 : `EQC(200)` s'affiche `H` (code 72). Dans les chaînes, les fichiers et le ruban, ils gardent leur code entier.

---

## Annexe D. Différences entre les trois profils

| Point | LSE F4 (`-F4`, défaut) | T1600 version 4 (`-T1600`) | MITRA 15 version 8 (`-MITRA15`) |
|---|---|---|---|
| **Lancement et session** | | | |
| Question de la date au lancement | oui (`DATE HEURES MINUTES :`) | non | non |
| Bannière de `BONJOUR` | `BONJOUR jj/mm/aa hh:mm:ss`, puis `CONSOLE NO. 01.`, `LSE F4`, `RUB-OUT REMPLACE CTRL X-OFF`, `VOUS ETES SOUS ID c` | `BONJOUR  jj/mm/aa hh:mm:ss` | `BONJOUR`, puis `LSE-MI15  CONSOLE NO.01  jj/mm/aa hh:mm:ss` |
| `AU REVOIR` | date, heure et bannière | date et heure | heure seule |
| Identification initiale par défaut | 1 | 0 | 0 |
| DELETE (RUB-OUT) vaut X-OFF | oui | non | non |
| Fin de programme | `TERMINE EN LIGNE nnn` | `TERMINE EN LIGNE nnn` | `TERMINE` |
| **Langage** | | | |
| Numéros de ligne | 1 à 255 | 1 à 255 | 1 à 250 |
| Réels | 23 chiffres binaires (`0.1` s'affiche `0.09999999`) | 23 chiffres binaires | 24 chiffres binaires (`0.1` s'affiche `0.1`) |
| Entiers exacts jusqu'à | 8 388 607 | 8 388 607 | 16 777 215 |
| `TEM`, `DAT` | `TEM` ou `TEM()`, `DAT` ou `DAT()`, listés avec parenthèses | `TEM`, `DAT` | `TEM()`, `DAT()` |
| Notation `.n.` dans les chaînes | oui | non | non |
| `FAIRE n TANT QUE c` sans `POUR` | oui | non | non |
| `SH`, `CH`, `TH`, `SGN`, `UN`, `ZER` | oui | oui | non |
| `@` | numéro de la ligne en cours | numéro de la ligne en cours | marque un numéro de ligne (`I←@50`) |
| `$` dans une procédure | ligne de l'appel | ligne de l'appel | non |
| Instruction `SUPPRIMER` | non | non | oui |
| `EQC` | 0 à 127 | 0 à 127 | 0 à 255 |
| Facteur de répétition d'un format | 1 à 62 | 1 à 62 | 1 à 99 |
| Mots réservés propres | `DAT`, `TEM` | `DAT`, `TEM` | `SUPPRIMER`, `REPRENDRE` |
| `CHAINE`, `TABLEAU`, `LIBERER` en machine de bureau | permis | permis | interdits |
| **Commandes** | | | |
| `REPRENDRE` | `REPRENDRE A PARTIR DE` | `REPRENDRE A PARTIR DE` | `REPRENDRE EN` |
| Lecture du ruban | ‹X-ON› (Ctrl-Q), lignes affichées | ‹X-ON›, lignes affichées | commande `RUBAN`, en silence |
| `SILENCE` | non | non | oui |
| `RANGER nom,E`, `RANGER nom,L`, `CATALOGUER t,p,L` | oui | oui | non |
| `SUPPRIMER` (commande) | `nom,P`, `nom,D`, `nom,T`, `*` | `nom,P`, `nom,D`, `nom,T`, `*` | `nom` |
| Programme et fichier de données permanent de même nom | permis | permis | refusé (`F 03`, `F 04`) |
| Liste complète de `TABLE`, `UTILISATION` | `*` | `*` | `A` ; `UTILISATION` exige l'attribut `>` |
| `IDENTIFICATION` et fichiers temporaires | conservés | conservés | supprimés |
| Attribut `X` (choix de l'espace temporaire) | sans effet | sans effet | dialogue `ESPACE TEMPORAIRE DESIRE:` |
| `ENCODER nom` sans numéros | ajoute les lignes | ajoute les lignes | remplace le programme |
| Unité amovible, `$nom`, `DUPLIQUER` | oui | non | non |
| **Fichiers** | | | |
| Fichiers-programmes | propres au profil | propres au profil | propres au profil |
| Fichiers-données, rubans | communs | communs | communs |

---

## Annexe E. Limites et capacités

| Élément | Limite |
|---|---|
| Numéro de ligne | 1 à 255 (1 à 250 sous MITRA 15) |
| Longueur d'une ligne de programme ou de machine de bureau | 79 caractères |
| Longueur d'une saisie (réponse à `LIRE`, paramètres) | 256 caractères |
| Nom de variable, de tableau, de procédure (sans `&`), de fichier | 5 caractères, le premier étant une lettre |
| Nombres | de ±1,2 × 10⁻³⁸ environ à ±3,4 × 10³⁸ ; 6 à 7 chiffres significatifs |
| Entiers exacts | jusqu'à 8 388 607 (16 777 215 sous MITRA 15) |
| Chaîne | 65 535 caractères |
| Tableau | 1 ou 2 dimensions, 65 535 éléments |
| Noms différents dans un programme | 6 553 |
| Chaînes constantes différentes dans un programme | 21 845 |
| Parenthèses imbriquées dans une ligne | une trentaine au moins |
| Facteur de répétition fixe d'un format | 62 (99 sous MITRA 15) |
| `Fe.d`, `Ee.d` | e et d de 0 à 99 |
| Enregistrements d'un fichier-données | numéros 1 à 32 767 |
| Taille d'un fichier-données | 65 535 secteurs de 256 octets |
| Espace temporaire | 200 secteurs par défaut (`-TEMP`) |
| Identifications | numéros 0 à 65 499 ; 0, 1 et au plus 9 autres |
| Chemin de la bibliothèque (`-BIB`) | 170 caractères |
| Consoles | une seule, la console 1 |
| Profondeur des appels de procédures, nombre de variables | limités par la mémoire (`ERREUR (EX.) 03`) |

---

## Annexe F. Aide-mémoire

### Touches

| Touche | Rôle |
|---|---|
| ‹X-OFF› | Ctrl-S (et DELETE sous LSE F4) : fin de saisie |
| ‹RETURN› | fin de saisie ; dans une chaîne lue, y ajoute un passage à la ligne |
| ‹ESC› | abandon de la saisie, interruption |
| ‹X-ON› | Ctrl-Q sur une ligne vide : lecture du ruban (LSE F4, T1600) |
| `\` | annule le caractère précédent |
| `_` `^` | `←` (affectation), `↑` (puissance) |

### Commandes (deux lettres)

| Famille | Commandes |
|---|---|
| Session | `BO` BONJOUR, `AU` AU REVOIR |
| Édition | `LI` LISTER, `NU` NUMERO, `EF` EFFACER LIGNES, `PE` PERFORER, `AB` ABREGER, `IN` IN EXTENSO |
| Exécution | `EX` EXECUTER, `CO` CONTINUER, `PO` POURSUIVRE, `RE` REPRENDRE, `PA` PAS A PAS, `NO` NORMAL |
| Fichiers | `AP` APPELER, `RA` RANGER, `MO` MODIFIER, `SU` SUPPRIMER, `CA` CATALOGUER, `DE` DECODER, `EN` ENCODER, `TA` TABLE DES FICHIERS, `UT` UTILISATION DISQUE, `DU` DUPLIQUER (F4) |
| Identification | `ID` IDENTIFICATION |
| Divers | `PR` PRENDRE ETAT CONSOLE, `RU` RUBAN (M), `SI` SILENCE (M) |
| État dormant | `CR` CREATION ID, `SU` SUPPRESSION ID, `LI` LISTE ID |

### Instructions

| Instruction | Exemple |
|---|---|
| affectation | `A←B+1` ; `T[I,J]←0` ; `C←'TEXTE'` |
| `SI … ALORS … [SINON …]` | `SI X>0 ALORS Y←1 SINON Y←-1` |
| `DEBUT … FIN` | `DEBUT A←1;B←2 FIN` |
| `ALLER EN` | `ALLER EN 100` |
| `FAIRE` | `FAIRE 50 POUR I←1 PAS 2 JUSQUA N` ; `FAIRE 50 POUR K←1 TANT QUE X<1` |
| `PAUSE`, `TERMINER` | |
| `LIRE` | `LIRE A,B,NOM,TAB` |
| `AFFICHER` | `AFFICHER 'X = ',X` ; `AFFICHER[/,'X = ',F6.2] X` |
| `TABLEAU`, `CHAINE`, `LIBERER` | `TABLEAU T[10],M[3,3]` ; `CHAINE NOM` ; `LIBERER T` |
| `GARER`, `CHARGER` | `GARER T,1,'#DONN'` ; `CHARGER T,1,'#DONN',R` |
| `EXECUTER` | `EXECUTER 'SUITE',10` |
| `PROCEDURE` | `PROCEDURE &F(X,Y) LOCAL X,I` |
| `RETOUR`, `RETOUR EN`, `RESULTAT` | `RESULTAT X*X` |
| appel | `&TRACE(10)` ; `Y←&F(2,Z)` |
| commentaire | `10*TITRE` ; `A←1;*REMARQUE` |
| (T1600, F4) `UN`, `ZER` | `UN T;ZER M` |
| (MITRA 15) `SUPPRIMER` | `SUPPRIMER 'F',3` |

### Opérateurs

| Opérateurs | Sens |
|---|---|
| `+ - * / ↑` | arithmétique ; priorité : fonctions, `↑`, `* /`, `+ -` |
| `!` | concaténation de chaînes |
| `< <= = # >= >` | comparaison (`#` : différent) |
| `ET`, `OU` | combinaison de conditions (`ET` avant `OU`) |
| `SI c ALORS e1 SINON e2` | expression conditionnelle |

### Fonctions

| Famille | Fonctions |
|---|---|
| Numériques | `SIN COS ATG LGN EXP RAC ABS ENT ALE` ; (T) `SH CH TH SGN` ; `TEM` |
| Sur les chaînes | `LGR(C)` `SCH(C,P,N\|S[,V])` `GRL(C,P[,V])` `POS(C,P,S)` `PTR(C,P[,N\|S])` `SKP(C,P[,S])` `CCA(X)` `CNB(C,P[,V])` `EQC(X)` `EQN(C[,P])` `DAT` |

### Formats

| Élément | Sens |
|---|---|
| `'texte'` | libellé |
| `/` `X` `C` `L` | à la ligne, espace, retour en début de ligne, à la ligne |
| `U` `Fe.d` `Ee.d` | universel, décimal, à exposant |
| `n` devant | répétition (1 à 62) |
| `*` devant un libellé ou une mise en page | répétition prise dans la liste |

---

## Annexe G. Programmes d'exemple

Ces programmes illustrent les principales possibilités du langage. Tapez-les (ou préparez-les comme un ruban, paragraphe 7.7), exécutez-les, puis modifiez-les. Ils fonctionnent sous les trois profils, sauf indication contraire ; sous MITRA 15, remplacez `TEM` par `TEM()`. Les exécutions montrées ont été obtenues sous LSE F4, chaque réponse étant terminée par ‹X-OFF›.

Les programmes CARNE (carnet de téléphone, paragraphe 10.5), ETAP1 et ETAP2 (enchaînement, paragraphe 10.8) et GENER (programme qui en écrit un autre, paragraphe 12.8) complètent cette liste.

### G.1 DEVIN : deviner un nombre

Notions : hasard (`ALE`), `LIRE`, `SI`, `DEBUT … FIN`, `ALLER EN`, compteur.

```text
1*DEVIN : DEVINE UN NOMBRE ENTRE 1 ET 100
10 N←ENT(ALE(0)*100)+1;C←0
20 AFFICHER 'J''AI CHOISI UN NOMBRE ENTRE 1 ET 100.'
30 AFFICHER 'TA PROPOSITION ? ';LIRE P;C←C+1
40 SI P<N ALORS DEBUT AFFICHER 'TROP PETIT.';ALLER EN 30 FIN
50 SI P>N ALORS DEBUT AFFICHER 'TROP GRAND.';ALLER EN 30 FIN
60 AFFICHER[/,'BRAVO ! TROUVE EN ',U,'COUPS.'] C
70 TERMINER
```

```text
EXECUTER A PARTIR DE 1

J'AI CHOISI UN NOMBRE ENTRE 1 ET 100.
TA PROPOSITION ? 50
TROP PETIT.
TA PROPOSITION ? 75
TROP PETIT.
TA PROPOSITION ? 90
TROP PETIT.
TA PROPOSITION ? 94
BRAVO ! TROUVE EN 4 COUPS.
TERMINE EN LIGNE 070
```

Le nombre change à chaque partie. Pour essayer toujours le même nombre, remplacez `ALE(0)` par `ALE(0.5)`.

### G.2 MULTI : table de multiplication

Notions : boucles imbriquées, formats `F`, `X`, `/`, répétition d'un libellé.

```text
1*MULTI : TABLE DE MULTIPLICATION
10 AFFICHER[/,'TABLE DE MULTIPLICATION',/]
20 AFFICHER[4X];FAIRE 20 POUR J←1 JUSQUA 9;AFFICHER[F4.0] J
30 AFFICHER[/,4X,36'-']
40 FAIRE 60 POUR I←1 JUSQUA 9
50 AFFICHER[/,F2.0,' |'] I
60 FAIRE 60 POUR J←1 JUSQUA 9;AFFICHER[F4.0] I*J
70 AFFICHER[/];TERMINER
```

```text
EXECUTER A PARTIR DE 1

TABLE DE MULTIPLICATION
       1   2   3   4   5   6   7   8   9
    ------------------------------------
 1 |   1   2   3   4   5   6   7   8   9
 2 |   2   4   6   8  10  12  14  16  18
 3 |   3   6   9  12  15  18  21  24  27
 4 |   4   8  12  16  20  24  28  32  36
 5 |   5  10  15  20  25  30  35  40  45
 6 |   6  12  18  24  30  36  42  48  54
 7 |   7  14  21  28  35  42  49  56  63
 8 |   8  16  24  32  40  48  56  64  72
 9 |   9  18  27  36  45  54  63  72  81
TERMINE EN LIGNE 070
```

### G.3 MOYEN : moyenne de notes

Notions : `TABLEAU` de taille calculée, lecture et affichage d'un tableau entier, plus grand et plus petit élément.

```text
1*MOYEN : MOYENNE, PLUS FORTE ET PLUS FAIBLE NOTE
10 AFFICHER 'NOMBRE DE NOTES ? ';LIRE N
20 TABLEAU NOTE[N]
30 AFFICHER 'TAPEZ LES NOTES : ';LIRE NOTE
40 S←0;MAX←NOTE[1];MIN←NOTE[1]
50 FAIRE 70 POUR I←1 JUSQUA N;S←S+NOTE[I]
60 SI NOTE[I]>MAX ALORS MAX←NOTE[I]
70 SI NOTE[I]<MIN ALORS MIN←NOTE[I]
80 AFFICHER[/,'MOYENNE     : ',F2.2] S/N
85 AFFICHER[/,'PLUS FORTE  : ',F2.1,/,'PLUS FAIBLE : ',F2.1] MAX,MIN
90 AFFICHER[/,'LES NOTES :',F6.1] NOTE
100 TERMINER
```

```text
EXECUTER A PARTIR DE 1

NOMBRE DE NOTES ? 5
TAPEZ LES NOTES : 12 15.5 8 17 11
MOYENNE     : 12.70
PLUS FORTE  : 17.0
PLUS FAIBLE :  8.0
LES NOTES :
    12.0
    15.5
     8.0
    17.0
    11.0
TERMINE EN LIGNE 100
```

### G.4 CONJU : conjugaison au présent

Notions : chaînes, `SCH`, `LGR`, `GRL` avec sa position de reprise, concaténation.

```text
1*CONJU : CONJUGUE AU PRESENT UN VERBE EN -ER
10 CHAINE V,R,P,T
20 P←'JE TU IL ELLE NOUS VOUS ILS ELLES'
30 T←'E ES E E ONS EZ ENT ENT'
40 AFFICHER 'VERBE EN -ER ? ';LIRE V
50 SI SCH(V,LGR(V)-1,2)='ER' ALORS ALLER EN 70
60 AFFICHER 'CE N''EST PAS UN VERBE EN -ER.';ALLER EN 40
70 R←SCH(V,1,LGR(V)-2);I←1;J←1
80 FAIRE 90 POUR K←1 JUSQUA 8
90 AFFICHER[/,U,X,U,U] GRL(P,I,I),R,GRL(T,J,J)
100 TERMINER
```

```text
EXECUTER A PARTIR DE 1

VERBE EN -ER ? CHANTER
JE CHANTE
TU CHANTES
IL CHANTE
ELLE CHANTE
NOUS CHANTONS
VOUS CHANTEZ
ILS CHANTENT
ELLES CHANTENT
TERMINE EN LIGNE 100
```

À chaque appel, `GRL(P,I,I)` rend le mot suivant de `P` et range dans `I` la position où chercher le prochain. Exercice : traiter `J'` devant une voyelle (`J'AIME`).

### G.5 PALIN : palindromes

Notions : parcours d'une chaîne, `SKP` pour reconnaître une lettre, boucle à pas négatif, expression conditionnelle de chaînes.

```text
1*PALIN : UNE PHRASE EST-ELLE UN PALINDROME ?
10 CHAINE PH,L,R;AFFICHER 'PHRASE ? ';LIRE PH
20 L←'';FAIRE 30 POUR I←1 JUSQUA LGR(PH)
30 SI SKP(PH,I)=I ALORS L←L!SCH(PH,I,1)
40 R←'';FAIRE 40 POUR I←LGR(L) PAS -1 JUSQUA 1;R←R!SCH(L,I,1)
50 AFFICHER SI L=R ALORS 'C''EST UN PALINDROME.' SINON 'CE N''EN EST PAS UN.'
60 TERMINER
```

```text
EXECUTER A PARTIR DE 1

PHRASE ? ESOPE RESTE ICI ET SE REPOSE
C'EST UN PALINDROME.
TERMINE EN LIGNE 060
```

La ligne 30 ne garde que les lettres : `SKP(PH,I)` donne la position de la première lettre à partir de `I`, qui vaut `I` si le caractère en `I` est une lettre. La ligne 40 écrit `L` à l'envers.

### G.6 JOUR : le jour de la semaine d'une date

Notions : procédure-fonction, paramètres et variables `LOCAL`, partie entière, reste d'une division.

```text
1*JOUR : JOUR DE LA SEMAINE D'UNE DATE (CALENDRIER GREGORIEN)
10 PROCEDURE &JOUR(Q,M,A) LOCAL Q,M,A,K,J,H
20 SI M<3 ALORS DEBUT M←M+12;A←A-1 FIN
30 K←A-100*ENT(A/100);J←ENT(A/100)
40 H←Q+ENT(13*(M+1)/5)+K+ENT(K/4)+ENT(J/4)+5*J
50 RESULTAT H-7*ENT(H/7)
100 CHAINE NOM;NOM←'SAMEDI  DIMANCHELUNDI   MARDI   MERCREDIJEUDI   VENDREDI'
110 AFFICHER 'JOUR, MOIS, ANNEE ? ';LIRE JJ,MM,AA
120 D←&JOUR(JJ,MM,AA)
130 AFFICHER[/,'C''ETAIT UN ',U] SCH(NOM,8*D+1,8)
140 TERMINER
```

Le programme principal commence en ligne 100 : la procédure, en tête, n'est jamais exécutée directement.

```text
EXECUTER A PARTIR DE 100

JOUR, MOIS, ANNEE ? 14 7 1789
C'ETAIT UN MARDI
TERMINE EN LIGNE 140
```

La procédure applique la formule de Zeller, qui numérote les jours à partir du samedi (0). Les noms des jours sont rangés dans une seule chaîne, par tranches de huit caractères.

### G.7 HANOI : les tours de Hanoï

Notions : procédure-sous-programme récursive, paramètres `LOCAL`.

```text
1*HANOI : LES TOURS DE HANOI
10 PROCEDURE &HANOI(N,A,B,C) LOCAL N,A,B,C
20 SI N=0 ALORS RETOUR
30 &HANOI(N-1,A,C,B)
40 AFFICHER[/,'DE ',U,'VERS ',U] A,B
50 &HANOI(N-1,C,B,A);RETOUR
100 AFFICHER 'NOMBRE DE DISQUES ? ';LIRE N
110 &HANOI(N,1,3,2)
120 TERMINER
```

```text
EXECUTER A PARTIR DE 100

NOMBRE DE DISQUES ? 3
DE 1 VERS 3
DE 1 VERS 2
DE 3 VERS 2
DE 1 VERS 3
DE 2 VERS 1
DE 2 VERS 3
DE 1 VERS 3
TERMINE EN LIGNE 120
```

Pour déplacer N disques de la tige A vers la tige B, on déplace les N−1 du dessus vers la tige C, puis le plus grand vers B, puis les N−1 de C vers B. Chaque appel a ses propres valeurs de `N`, `A`, `B`, `C`, grâce à `LOCAL`.

### G.8 TRI : trier des nombres

Notions : tableau passé à une procédure par référence, procédure-sous-programme, boucles imbriquées avec une ligne vide de fin de boucle, affichage d'un tableau.

```text
1*TRI : TRI D'UNE LISTE DE NOMBRES
10 PROCEDURE &TRI(T,N) LOCAL N,I,J,X
20 FAIRE 50 POUR I←1 JUSQUA N-1
30 FAIRE 50 POUR J←I+1 JUSQUA N
40 SI T[I]<=T[J] ALORS ALLER EN 50
45 X←T[I];T[I]←T[J];T[J]←X
50 
60 RETOUR
100 AFFICHER 'COMBIEN DE NOMBRES ? ';LIRE N;TABLEAU L[N]
110 AFFICHER 'TAPEZ-LES : ';LIRE L
120 &TRI(L,N)
130 AFFICHER[/,'DANS L''ORDRE :',F8.2] L
140 TERMINER
```

La ligne 50 se tape `50` suivi d'un espace : c'est la fin commune des deux boucles.

```text
EXECUTER A PARTIR DE 100

COMBIEN DE NOMBRES ? 6
TAPEZ-LES : 3.5 -2 17 0 8 1E3
DANS L'ORDRE :
      -2.00
       0.00
       3.50
       8.00
      17.00
    1000.00
TERMINE EN LIGNE 140
```

`T` n'est pas dans la liste `LOCAL` : c'est une référence, et la procédure trie directement le tableau `L` du programme. Les variables `I`, `J` et `X`, locales, n'existent plus après l'appel : `?I` répond `ERREUR (EX.) 62 EN LIGNE BUR`.

---

## Annexe H. Glossaire

**Affectation.** Instruction qui range une valeur dans une variable : `A←3`.

**Bibliothèque.** Répertoire du disque où LSE range programmes, fichiers-données, catalogue et identifications (par défaut `@:LSE`).

**Bip.** Signal sonore ; LSE l'émet au début de chaque `LIRE`.

**Chaîne (de caractères).** Suite de caractères : un mot, une phrase. Constante : entre apostrophes. Variable : déclarée par `CHAINE`.

**Clé.** Partie secrète d'une identification (tout sauf les deux derniers chiffres).

**Commande.** Ordre donné à LSE en état moniteur, par deux lettres : `LI`, `EX`, `RA`…

**Commentaire.** Texte libre, après `*`, sans effet sur l'exécution.

**Compilation.** Traduction d'une ligne tapée dans la forme que LSE exécute ; elle a lieu dès la frappe. Une erreur de compilation (`ERREUR  C nn`) signale une ligne mal écrite.

**Compte.** Les deux derniers chiffres d'une identification ; il désigne le propriétaire des fichiers.

**Compte rendu.** Quatrième paramètre de `CHARGER`, qui reçoit un code au lieu de provoquer une erreur.

**Console.** Le poste de travail : écran et clavier. Les systèmes d'origine en avaient plusieurs ; LSE-IIGS en a une, la console 1.

**Décompiler.** Réécrire en texte une ligne compilée, pour `LISTER`, `PERFORER` ou `DECODER`.

**Enregistrement.** Partie numérotée d'un fichier-données, qui contient un nombre, un tableau ou une chaîne.

**Espace temporaire.** Place réservée aux fichiers temporaires d'une session (200 secteurs par défaut).

**État dormant.** État de LSE entre son lancement et `BONJOUR`.

**Expression.** Formule qui donne une valeur : `A+2*B`, `NOM!'.'`.

**Fichier-données.** Fichier d'enregistrements écrit par `GARER` et lu par `CHARGER` ; temporaire ou permanent.

**Fichier-programme.** Fichier qui contient un programme, rangé par `RANGER`.

**Format.** Partie d'un `AFFICHER`, entre crochets, qui décrit la présentation des valeurs.

**Identificateur.** Nom d'une variable, d'une chaîne ou d'un tableau.

**Identification.** Numéro qui donne des droits pendant une session (`IDENTIFICATION`) ; il se compose d'une clé et d'un compte.

**Ligne d'arrêt.** Ligne où l'exécution s'arrête avant de l'exécuter (`EXECUTER A PARTIR DE n1,n2`, `POURSUIVRE`).

**Ligne vide.** Ligne réduite à son numéro et un espace ; elle ne fait rien.

**Machine de bureau.** Mode où une ligne tapée sans numéro est exécutée tout de suite.

**Moniteur.** La partie de LSE qui dialogue avec vous entre deux exécutions.

**Mot réservé.** Mot du langage qui ne peut pas servir de nom.

**Paramètre local, paramètre référence.** Un paramètre `LOCAL` reçoit une copie de la valeur ; un paramètre référence travaille sur la variable même de l'appelant.

**Pas à pas.** Mode où l'exécution s'arrête au début de chaque ligne.

**Perforer.** Écrire sur le ruban.

**Procédure.** Partie de programme nommée par `&`, appelée avec des paramètres ; procédure-fonction (avec `RESULTAT`) ou procédure-sous-programme (avec `RETOUR`).

**Profil.** Manière de se comporter de LSE-IIGS : LSE F4, T1600 version 4 ou MITRA 15 version 8.

**Programme secret.** Programme rangé par `RANGER nom,E`, que les autres comptes peuvent exécuter mais non lire.

**Ruban.** Autrefois, bande de papier perforé ; ici, fichier texte que LSE écrit (`PERFORER`) et relit comme une frappe.

**Secteur.** Unité de place sur le disque : 256 octets.

**Session.** Période de travail entre `BONJOUR` et `AU REVOIR`.

**Shell ORCA.** L'interpréteur de commandes depuis lequel on lance LSE.

**Tableau.** Ensemble de nombres repérés par un ou deux indices.

**Unité amovible.** Sous LSE F4, répertoire désigné par `-UNITE` qui joue le rôle d'une disquette ; désigné par `$` dans les commandes.

**Variable.** Case de la mémoire, désignée par un nom, qui contient une valeur.

**X-OFF, X-ON.** Caractères de commande de la télétype : X-OFF termine une saisie (Ctrl-S), X-ON met en marche le lecteur de ruban (Ctrl-Q).

---

## Annexe I. Questions fréquentes et dépannage

**LSE répond `????` à tout ce que je tape.**
Vous êtes sans doute en état dormant : tapez `BO` puis ‹X-OFF› pour ouvrir la session. En session, `????` signale une commande inconnue, ou une commande tapée en entier : on ne tape que ses deux lettres.

**Toutes mes lignes sont refusées par `ERREUR  C 01`.**
Les minuscules ne sont permises que dans les chaînes : enclenchez Caps Lock.

**J'ai tapé `A=3` et LSE répond `ERREUR  C 38`.**
L'affectation s'écrit avec la flèche : `A←3`, la flèche se tapant `_`.

**`EXECUTER A PARTIR DE 1` répond `????`.**
La ligne 1 n'existe pas : donnez le numéro de la première ligne de votre programme (`NUMERO A PARTIR DE 1` les liste).

**Mon programme s'arrête sur `ERREUR (EX.) 58`.**
Il est arrivé au bout sans rencontrer `TERMINER` : ajoutez `TERMINER` à la fin du programme principal.

**Mon programme s'arrête sur `ERREUR (EX.) 12`.**
Il est arrivé sur une ligne `PROCEDURE`. Placez les procédures après le `TERMINER` du programme principal.

**Pourquoi `0.1` s'affiche-t-il `0.09999999` ?**
Les réels sont gardés en binaire, et 0,1 n'y tombe pas juste (paragraphe 5.3). Utilisez un format : `AFFICHER[F1.2] X`.

**`?.5` est refusé.**
Un nombre commence par un chiffre : écrivez `0.5`.

**`Y←3*SI A>B ALORS 1 SINON 2` est refusé (`ERREUR  C 101`).**
Mettez l'expression conditionnelle entre parenthèses : `Y←3*(SI A>B ALORS 1 SINON 2)`.

**La chaîne lue par `LIRE` contient un passage à la ligne.**
La réponse a été terminée par ‹RETURN› : terminez-la par ‹X-OFF› (Ctrl-S, ou DELETE sous LSE F4).

**`LIRE` ne semble pas attendre ma réponse.**
Il a peut-être trouvé sa valeur dans le reste de la ligne tapée pour le `LIRE` précédent (paragraphe 5.15).

**Mon programme tourne sans fin.**
Frappez ‹ESC› : `PRET EN LIGNE nnn` indique où il en était. Examinez les variables (`?I`), puis `CONTINUER`, ou corrigez.

**`CONTINUER` répond `????`.**
Le programme s'est terminé, ou a rencontré une erreur, ou vous avez tapé une ligne depuis l'arrêt : utilisez `REPRENDRE A PARTIR DE n` ou `EXECUTER` (paragraphe 8.8).

**`RANGER` répond `ERREUR F 07`.**
Votre identification n'a pas le droit de ranger : `IDENTIFICATION 1` (ou une identification avec l'attribut `P`). Sous `-T1600` et `-MITRA15`, la session commence sous l'identification 0.

**`RANGER` répond `ERREUR F 03`.**
Un programme porte déjà ce nom. Pour le remplacer, utilisez `MODIFIER`.

**`APPELER` répond `ERREUR F 100`.**
Le programme a été rangé sous un autre profil (paragraphe 10.11).

**`LISTER` répond `ERREUR F 06`.**
Le programme en mémoire est un programme secret d'un autre compte (paragraphe 11.9).

**Au lancement, LSE affiche `ERREUR F 102`.**
La bibliothèque est inaccessible (paragraphe 2.6). Lancez LSE avec `-BIB` vers un répertoire où l'on peut écrire.

**En relisant le ruban, de vieux programmes reviennent.**
Le ruban s'allonge à chaque `PERFORER`. Supprimez-le depuis le shell (`DELETE @:LSE:RUBAN`) avant de perforer, ou désignez un autre fichier par `-RUBAN`.

**Comment effacer l'écran ?**
Sous LSE F4 : `AFFICHER .12.` ; sous les autres profils : `AFFICHER EQC(12)` (annexe C).

**La touche DELETE n'efface pas le caractère précédent.**
Sous LSE F4, elle termine la saisie, comme X-OFF. Pour effacer le dernier caractère, tapez `\`.

**Comment renuméroter un programme ?**
Par l'éditeur d'ORCA, à travers le ruban (paragraphe 7.7). Le mieux est de numéroter de 10 en 10 dès le départ.

**Comment quitter LSE ?**
`AU` puis ‹X-OFF›. Pensez d'abord à ranger votre programme.

---

## Annexe J. Bibliographie

### Le LSE

**Manuels**

- INRDP, Section Informatique et Enseignement (Christian Lafond), *Langage Symbolique d'Enseignement (L.S.E.) — Manuel d'utilisation*, 2ᵉ édition, bulletin de liaison « L'informatique et l'enseignement secondaire », numéro spécial, Paris, janvier 1975. Première édition : mars 1973.
- École d'Application des Transmissions, *Langage Symbolique d'Enseignement — LSE 15, manuel d'utilisation* (version MITRA 15), 1978 (diffusion interne).
- *MEMENTO LSE*, GG/NLG/Études Logiciel, février 1982, réf. R-1017 F (LSE du micro-ordinateur Micral 80-22G de R2E).
- Christian Lafond, Pierre Muller, *Lire LSE, le langage français de programmation — manuel de l'utilisateur*, préface de Jacques Hebenstreit, CEDIC / Fernand Nathan, Paris, 1983.
- *Le LSE graphique 8086/8088, version IV, selon la norme AF Z 65-020*, 1988 (pour MS-DOS).
- *LSE pour tous*, CNDP / EPI (cinq éditions).
- « Le système LSE, mieux connaître son fonctionnement sur micro-ordinateur », *Dossier EPI* n° 3, juin 1983.

**Norme**

- AFNOR, *Norme expérimentale Z 65-020 : LSE, Langage Symbolique d'Enseignement*, Paris, décembre 1983.

**Articles**

- Jacques Hebenstreit, Yves Noyelle, « Un langage symbolique destiné à l'enseignement : LSE », congrès AFCET, Grenoble, novembre 1972.
- Yves Noyelle, « Le LSE, son histoire, son développement », *Éducation 2000* n° 16, Institut supérieur de pédagogie, Paris, 1980.
- *L'informatique à l'école : concours de programmes AFCET-IFIP 1981*, numéro spécial du bulletin de l'EPI, juin 1982.
- Yves Noyelle, « String Handling Facilities of the LSE Language », *SIGPLAN Notices*, août 1984.
- Jacques Arsac, « LSE83 », bulletin de l'EPI n° 38, juin 1985, p. 116-139.
- Yves Noyelle, « La saga du LSE et de sa famille (LSD/LSG/LST) », *Actes du colloque sur l'histoire de l'informatique en France*, Grenoble, mai 1988, vol. II ; repris dans la *Revue de l'EPI* n° 54, 1989, p. 216-228, <https://www.epi.asso.fr/revue/54/b54p216.htm>.
- « Langage symbolique d'enseignement », article de l'encyclopédie *Wikipédia* en français.

### Le shell ORCA

- Mike Westerfield, Stephen Heumann, *ORCA/C 2.2 — A C Compiler and Development System for the Apple IIGS*, Byte Works. Le chapitre 6 (« Using the Shell ») présente le shell, le chapitre 8 (« The Command Processor ») ses commandes, ses préfixes et l'installation d'un utilitaire, et le chapitre 9 (« The Text Editor ») l'éditeur de texte.
