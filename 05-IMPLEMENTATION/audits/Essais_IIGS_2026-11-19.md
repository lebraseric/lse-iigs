# Essais — port série, terminal série (préparés le 2026-11-19)

## Objet

Éric a retenu le 2026-11-17 de pouvoir conduire une session de `LSE` depuis un terminal relié au port modem de l'Apple IIGS. Le premier terminal est l'application Terminal du Mac, reliée par un adaptateur USB / RS-232 et un câble RS-232 / mini-DIN 8. Les terminaux historiques émulés viendront ensuite. Les choix validés sont les suivants :

- **rôle A** : la console de la session passe entière sur le port série, une seule console. Le code reste organisé pour qu'une évolution à plusieurs consoles (rôle C) reste possible ;
- **voie a′** : le pilote que GS/OS engendre pour le port modem ;
- **réglages du port au tableau de bord**, sans code de configuration ;
- **option `-SERIE`** : le clavier du IIGS est ignoré pendant la session série ;
- **sonde d'abord**, puis la décision IMP, puis le code.

## Ce que l'on sait déjà

- **Pas de pilote `.MODEM`.** Sous System 6.0.4, `*:SYSTEM:DRIVERS:MODEM` et `PRINTER` ont le type `$BB` et l'auxtype `$0002`. Ce sont des pilotes de port du Print Manager (*Toolbox Reference*, chap. 15), pas des pilotes de périphérique de GS/OS, qui ont l'auxtype `$01xx` (*GS/OS Device Driver Reference*, chap. 8, fig. 8-2). Vérifié sur l'image `System6.0.4.po` par AppleCommander.
- **Pilote engendré.** Pour un slot sans pilote chargé dont le microprogramme suit le protocole Pascal 1.1, GS/OS engendre au démarrage un pilote de caractères (*Device Driver Reference*, chap. 7 et annexe A). Ce pilote :
  - n'accepte que l'attente : `SetWaitStatus` sans attente rend `$22` ;
  - n'a aucun appel qui lui soit propre ;
  - transmet chaque caractère par un appel au microprogramme.
- **Bit 5 du mot d'état.** Le mot d'état d'un périphérique de caractères a un bit 5, « receive buffer not empty » (chap. 1, fig. 1-2). On ne sait pas si le pilote engendré le remplit. **C'est la question principale de la sonde** : de ce bit dépend la détection d'ESC sans assembleur.
- **XON/XOFF.** Quand la prise de contact XON/XOFF est active, le microprogramme garde `$13` et `$11` pour lui et ne les met pas dans la file d'entrée (*Firmware Reference*, chap. 5, « Buffering »). Or L.S.E. termine une saisie par X-OFF et appelle le lecteur de ruban par X-ON : cette prise de contact doit être désactivée.
- **Tampon.** Le microprogramme gère une file d'entrée de 2 048 caractères, remplie par interruptions, si le tampon est activé.

## La sonde `SONDES`

Le source est `tests/plateforme/sonde_serie.c`. Elle est sur `build/ESSAIS.po` (volume `/ESSAIS/`), reconstruite en profil `cible` le 2026-11-19.

Usage : `SONDES [S|L] [n]`.

- **S** (par défaut) : la sonde lit un octet seulement quand le bit 5 est à 1. Taper `!` sur le Mac suspend la lecture 5 s, puis la file est vidée. ESC tapé sur le Mac, ou n'importe quelle touche du IIGS, termine.
- **L** : la sonde lit sans condition et relève le bit 5 avant chaque lecture. **Seul ESC tapé sur le Mac termine ce mode.**
- **n** : impose un numéro de périphérique. Par défaut, la sonde prend le premier pilote de caractères du slot 2.

Chaque octet reçu est affiché à l'écran du IIGS en hexadécimal, avec son nom (X-OFF, X-ON, RUB-OUT…), et renvoyé en écho au Mac : en clair s'il est imprimable, sous la forme `<$xx>` sinon.

## 0. Préparation

**Tableau de bord du IIGS** :

- Slots, slot 2 : *Modem Port* (pas *Your Card*).
- Port modem :
  - 9 600 bauds, 8 bits de données, 1 bit d'arrêt, sans parité ;
  - *XON/XOFF* : **non** ;
  - *DCD Handshake* : non ;
  - *DSR/DTR* : non pour commencer ;
  - *Buffering* : **oui** ;
  - *Add LF after CR* : non ;
  - *Line Length* : unlimited.
- AppleTalk ne doit pas occuper le port modem.

Une modification des réglages du port modem est prise en compte sans redémarrer l'Apple IIGS, mais seulement au lancement suivant de la sonde, ou de `LSE` (constat d'Éric, 2026-11-19). Un changement du slot 2 dans *Slots* (*Modem Port* ou *Your Card*) demande en revanche de redémarrer : le pilote engendré est créé au démarrage (essai 5.14).

**Mac** : dans Terminal, repérer le port par `ls /dev/cu.*`, puis lancer :

```
screen -fn /dev/cu.usbserial-XXXX 9600,cs8,-ixon,-ixoff
```

`-fn` est indispensable : sans lui, `screen` garde Ctrl-S et Ctrl-Q pour son propre contrôle de flux, quelles que soient les options de la ligne (constaté sous GSSquared, §3). `Ctrl-A f` doit annoncer « flow off ». On quitte `screen` par `Ctrl-A k`.

**IIGS** : copier `SONDES` sur le disque dur et le lancer depuis le shell ORCA.

**Relevé** : garder la sortie complète de l'écran du IIGS (photo ou recopie), surtout la liste des périphériques.

## 1. Essais

| # | Action | Attendu | Constaté |
|---|---|---|---|
| 1.1 | `SONDES` | `SONDE PERIPHERIQUES (DInfo)` : une ligne par périphérique, dont un pilote `caracteres engendre` d'emplacement `$000A` ou `$0002`. Relever son **nom** et son **type** | `23 .DEV20` `type $0009` |
| 1.2 | (suite) | `SONDE PORT SERIE : peripherique n NOM, mode S` | OK |
| 1.3 | (suite) | Ligne `GetDeviceStatus` : relever le mot et les bits ouvert, en ligne, recu, emission vide | `mot $0010 (ouvert 0, en ligne 1, recu 0, emission vide 0)` |
| 1.4 | (suite) | `GetWaitStatus` : `$0000`. `SetWaitStatus sans attente` : `$0022` | OK |
| 1.5 | (suite) | `DWrite banniere : erreur $0000, transfert 31`, et `SONDE LSE-IIGS : PORT SERIE` apparaît sur le Mac. Si `$0023` s'affiche (attendu d'après GSSquared, §3), relever aussi la ligne `OpenGS` | `erreur $0023`, puis `OpenGS .DEV20 : erreur $0000, ref 6`, et `DWrite banniere apres OpenGS : erreur $0000, transfert 31` |
| 1.6 | (suite) | `1000 GetDeviceStatus : x ms chacun`, à relever | `0.600 ms chacun` |
| 1.7 | (suite) | `5 lignes de 80 car. : x ms par ligne`, et 5 lignes de 78 caractères apparaissent sur le Mac. Environ 83 ms par ligne signifie une écriture synchrone ; beaucoup moins, un tampon de sortie | `50.0 ms par ligne` |
| 1.8 | Taper `A`, `z`, `1` sur le Mac | Au IIGS : `$41 A`, `$7A z`, `$31 1`. Au Mac : écho `Az1`. **Si rien n'arrive, le bit 5 est muet : passer à 1.13** | OK |
| 1.9 | Taper RETURN, Ctrl-S, Ctrl-Q, DELETE (ou BACKSPACE si DELETE envoie autre chose), Ctrl-G | `$0D CR`, `$13 X-OFF`, `$11 X-ON`, `$7F RUB-OUT` (ou `$08`), `$07 BEL`. Au Mac : retour à la ligne, `<$13>`, `<$11>`, `<$7F>`, `<$07>`. **`$13` et `$11` doivent arriver** | OK |
| 1.10 | Taper `!`, puis coller ou taper une ligne d'au moins 100 caractères pendant les 5 s | `pause de 5 s`, puis la vidange en hexadécimal et `n octets lus apres la pause` : n = longueur de la ligne. Le Mac affiche `RECUS APRES LA PAUSE : n` | OK `243 octets lus` |
| 1.11 | Taper ESC sur le Mac | `$1B ESC`, `interrogations …, dont bit 5 a 1 : …`, `octets recus : …`, `FIN SONDE` (au Mac aussi) | OK |
| 1.12 | `SONDES`, puis une touche du clavier du IIGS | `touche du IIGS : fin`, `FIN SONDE` | OK |
| 1.13 | Seulement si le bit 5 est muet : `SONDES L`, taper quelques caractères, puis ESC sur le Mac | Chaque octet arrive, avec `[bit 5 : 0]` ou `[bit 5 : 1]`. Relever la valeur du bit 5 | |

## 2. Décision attendue

- **Le bit 5 fonctionne (1.8 à 1.10)** : la voie a′ suffit. IMP-0283 (Moniteur et console) décrit la voie série, et `console_iigs.c` reçoit sa structure « voie ». ESC est détecté par `GetDeviceStatus`, au coût mesuré en 1.6.
- **Le bit 5 est muet** : on écrit toujours par le pilote engendré, mais on interroge l'entrée par le microprogramme (`Status`, requête 1, ou `InQStatus`), en assembleur (`asm` d'ORCA/C). Il faut d'abord une seconde sonde.
- **`$13` ou `$11` absents en 1.9** : vérifier d'abord que `screen` a été lancé avec `-fn` (`Ctrl-A f`) et `-ixon,-ixoff`, puis revoir le tableau de bord, puis l'adaptateur.
- **Moins de caractères que tapés en 1.10** : le tampon est inactif ou trop petit. Revoir *Buffering* au tableau de bord.

## 3. Essai préalable sous GSSquared (2026-11-19)

Dispositif : GSSquared 1.0, port modem relié par `[[connections]]` (`device = "serial"`) à un pseudo-terminal du pont `scripts/pont_serie.py`, et `screen` sur l'autre côté du pont. Les durées (1.6, 1.7) n'ont pas de sens sous l'émulateur (vitesse `ludicrous`, pas de débit en bauds) ; elles restent à mesurer sur l'Apple IIGS.

Constaté par Éric :

- 1.5 : le pilote engendré du port modem s'appelle `.DEV33`. `DWrite banniere : erreur $0023, transfert 0` : le pilote doit d'abord être ouvert. Après `OpenGS .DEV33` (erreur `$0000`, ref 3), `DWrite` rend `$0000`, transfert 30. La réalisation devra donc ouvrir le périphérique par le gestionnaire de fichiers de caractères avant `DRead` et `DWrite`. Elle devra le trouver par `DInfo` (slot 2, périphérique de caractères, engendré), et non par son nom, qui n'est pas garanti sur l'Apple IIGS.
- Bit 5 du mot d'état : il fonctionne. Les octets arrivent en mode S, et la pause de 5 s suivie de la vidange (1.10) rend tout ce qui a été tapé : le tampon du microprogramme conserve la frappe d'avance.
- 1.9 : `$13` et `$11` n'arrivent pas, bien que la prise de contact XON/XOFF soit désactivée au tableau de bord émulé. Les autres octets arrivent.

Diagnostic de 1.9 : relancer le pont avec `-v`. La trace montre chaque passage et les drapeaux IXON des deux côtés.

| Trace du pont | Lieu de la perte | Concerne le IIGS ? |
|---|---|---|
| Pas de `terminal -> emulateur : … 13 …` | `screen` (`Ctrl-A f`) ou côté terminal en IXON | non |
| `13` transmis, `[emulateur] IXON` | GSSquared règle son côté du pseudo-terminal en IXON | non, artefact de l'essai |
| `13` transmis, côté émulateur `brut` | dans le IIGS émulé : microprogramme série (réglage non appliqué, ou XON/XOFF retenus quand le tampon est actif) | oui : seconde sonde, `GetModeBits` par l'interface étendue |

Trace du pont relevée par Éric le 2026-11-19 : `terminal -> emulateur` ne montre que `41` puis `1b`. Ctrl-S et Ctrl-Q ne sont jamais sortis de `screen`, lancé sans `-fn` : la perte relève du premier cas, et le IIGS n'est pas en cause à ce stade. 1.9 refait par Éric le 2026-11-19 avec `screen -fn` : `$13` et `$11` arrivent. Sous l'émulateur, la prise de contact XON/XOFF désactivée au tableau de bord est donc respectée par le microprogramme série, et la seconde sonde (`GetModeBits`) n'est pas nécessaire. Tous les essais 1.1 à 1.12 sont conformes sous GSSquared. Restent à faire sur l'Apple IIGS : la liste entière, et en particulier les durées (1.6, 1.7), le câble et l'adaptateur, ainsi que le tableau de bord réel.

La même trace a révélé deux défauts de la sonde, corrigés le 2026-11-19 : la bannière était écrite sur 30 octets au lieu de 31, d'où un LF manquant ; la ligne de l'essai 1.7 était suivie d'un NUL, sa longueur étant de 79 caractères et non de 80. Elle compte désormais 78 caractères, plus CR LF. La trace confirme aussi que le microprogramme n'ajoute pas de LF après CR et transmet CR LF tel quel.

## 4. Essai sur l'Apple IIGS (2026-11-19)

Essais 1.1 à 1.12 passés par Éric sur l'Apple IIGS. Tous sont conformes, et 1.13 est sans objet. Le relevé est dans la colonne « Constaté » du §1.

- **Pilote.** Le pilote engendré s'appelle `.DEV20` (périphérique 23) sur l'Apple IIGS, et `.DEV33` sous GSSquared : le nom n'est pas stable, ce qui confirme qu'il faut le chercher par `DInfo`. Son type, `$0009`, est le type « Serial modem » de la table des types de périphériques (*GS/OS Reference*, chap. 7, `DInfo`). Le critère de recherche est donc : un périphérique de caractères, de type `$0009`, dans le slot 2.
- **Ouverture.** Comme sous GSSquared, `DWrite` rend `$0023` tant que le périphérique n'est pas ouvert. Après `OpenGS` (ref 6), la bannière passe entière (31 octets).
- **Bit 5.** Il fonctionne : les essais 1.8 à 1.11 se déroulent en mode S. La frappe d'avance est gardée : 243 octets ont été tapés pendant la pause de 5 s, et 243 ont été relus.
- **X-OFF et X-ON** arrivent avec `screen -fn` et la prise de contact XON/XOFF coupée au tableau de bord.
- **Coût d'une interrogation.** Un `GetDeviceStatus` coûte 0,600 ms, soit plus du quart d'un tour de la boucle (a) de `PERF` (2,25 ms). L'interpréteur demande `console_interruption` à chaque arrivée sur une ligne : il ne faut pas interroger le port à chaque fois.
- **Écriture.** Une ligne de 80 octets (78 caractères, CR, LF) s'écrit en 50 ms, alors que sa transmission dure 83 ms à 9 600 bauds. L'écriture est donc rendue avant la fin de l'émission : le microprogramme range la sortie dans sa file d'émission, de 2 048 octets par défaut, vidée par interruptions (*Firmware Reference*, chap. 5, « Buffering »). Les 50 ms sont le coût de l'appel au microprogramme, environ 0,62 ms par octet. Une sortie longue est donc limitée par la ligne à 9 600 bauds (960 octets par seconde), mais le serait par le processeur au-delà de 19 200 bauds (environ 1 600 octets par seconde). La vitesse de 9 600 bauds suffit.

Décision : la voie a′ convient (§2, premier cas). IMP-0283 (Moniteur et console §9 ter), points S1 à S6, validée par Éric le 2026-11-19 et réalisée le même jour. Constat d'Éric pendant ces essais : un changement des réglages du port modem au tableau de bord est pris en compte sans redémarrer l'Apple IIGS ; il fallait seulement relancer la sonde si elle était en cours d'exécution. Le choix du slot 2 demande, lui, un redémarrage (essai 5.14).

## 5. Essais de `LSE -SERIE` (IMP-0283)

Image : `build/LSE.po`, reconstruite en profil `cible` le 2026-11-19. Mêmes réglages qu'au §0. Sous GSSquared d'abord (pont `scripts/pont_serie.py`, `screen -fn /tmp/lse-serie/terminal 9600,cs8,-ixon,-ixoff`), puis sur l'Apple IIGS. Sous Golden Gate, `LSE -SERIE` répond `PORT SERIE INDISPONIBLE` et rend 1 (vérifié le 2026-11-19).

| # | Action | Attendu | Constaté |
|---|---|---|---|
| 5.1 | `LSE -SERIE` dans le shell | Écran du IIGS : `LSE : SESSION SUR LE PORT SERIE`. Terminal : `LSE F4`, une ligne blanche, `DATE HEURES MINUTES : ` | OK |
| 5.2 | Au terminal : RETURN, puis `BO` et Ctrl-S | `EN LIGNE....` ; puis `BONJOUR` complété sur la même ligne, la date, la bannière de `LSE F4` (`CONSOLE NO. 01.` … `VOUS ETES SOUS ID 1`) et cinq lignes blanches | OK |
| 5.3 | `10 A_2^10` RETURN, `20 AFFICHER A` RETURN, `30 TERMINER` RETURN, puis `LI` Ctrl-S `10` Ctrl-S | Chaque caractère en écho une seule fois, `_` et `^` tels quels ; `LISTER A PARTIR DE 10` et les trois lignes | OK |
| 5.4 | `EX` Ctrl-S `10` Ctrl-S | `1024`, puis `TERMINE EN LIGNE 030` | OK |
| 5.5 | `EF` Ctrl-S `*` Ctrl-S ; `10 LIRE B;AFFICHER B` RETURN, `20 TERMINER` RETURN ; `EX` Ctrl-S `10` Ctrl-S ; répondre `12` Ctrl-S | Bip au terminal à `LIRE` ; `12` ; `TERMINE EN LIGNE 020` | OK |
| 5.6 | `EX` Ctrl-S `10` Ctrl-S ; répondre `7` puis DELETE (touche du Mac) | DELETE termine la réponse comme Ctrl-S (`LSE F4`) : `7` | OK |
| 5.7 | `EF` Ctrl-S `*` Ctrl-S ; `10 I_0` RETURN, `20 I_I+1` RETURN, `30 ALLER EN 20` RETURN ; `EX` Ctrl-S `10` Ctrl-S ; après quelques secondes, ESC | `PRET EN LIGNE 020` ou `030`, presque aussitôt | OK |
| 5.8 | `CO` Ctrl-S, puis ESC ; puis `?I` RETURN | Reprise, puis arrêt ; I continue de croître d'un arrêt à l'autre | OK |
| 5.9 | `EF` Ctrl-S `*` Ctrl-S ; `10 FAIRE 20 POUR I_1 JUSQUA 300` RETURN, `20 AFFICHER I` RETURN, `30 TERMINER` RETURN ; `EX` Ctrl-S `10` Ctrl-S ; pendant l'affichage, taper `NU` Ctrl-S `10` Ctrl-S | L'affichage va à son terme (sortie limitée par la ligne, quelques secondes), puis `NUMERO A PARTIR DE 10` s'exécute, avec son écho, sans être retapé : `10 20 30` en champs de huit colonnes | OK |
| 5.10 | `EX` Ctrl-S `10` Ctrl-S, puis ESC pendant l'affichage | Arrêt après au plus seize arrivées sur une ligne : `PRET EN LIGNE 010` ou `020` | OK |
| 5.11 | Pendant la session, taper quelques touches au clavier du IIGS | Rien au terminal ni à l'écran du IIGS. Relever si ces touches apparaissent dans le shell après `AU REVOIR` | OK, mais après AU REVOIR, apparition d'un caractère sablier sur le IIGS. La pression d'une touche affiche à nouveau le prompt. Les caractères tapés sur le IIGS pendant l'exécution n'apparaissent pas. |
| 5.12 | `UT` Ctrl-S (UTILISATION DISQUE) | Le tableau, avec les secteurs libres (appel `Volume` permis) | OK |
| 5.13 | `AU` Ctrl-S | Terminal : `AU REVOIR` et la bannière ; écran du IIGS : retour au shell | OK |
| 5.14 | Tableau de bord : slot 2 sur *Your Card*, puis `LSE -SERIE` | `PORT SERIE INDISPONIBLE`, retour au shell. Remettre ensuite *Modem Port* | OK, seulement après un reboot |
| 5.15 | `LSE -SERIE -MITRA15`, puis `BO` Ctrl-S, `?EQC(200)` RETURN, `AU` Ctrl-S | Pas de question de la date ; bannière MITRA 15 ; `H` (7 bits, IMP-0282) | OK |
| 5.11 bis | Après la correction du 2026-11-19 (`$C010` écrit à la fin d'une session série, option (a)) : refaire 5.11 | Après `AU REVOIR`, le shell redonne son invite directement, sans sablier ; les touches frappées au IIGS pendant la session sont perdues | OK (Apple IIGS, 2026-11-19) : plus de sablier, l'invite du shell revient aussitôt, aucun caractère affiché |

Essai 5.11 (Éric) : après `AU REVOIR`, le shell affiche un sablier et n'affiche son invite qu'après une touche ; les touches frappées au IIGS pendant la session n'apparaissent pas. Lecture retenue : la dernière touche reste dans le registre du clavier, que personne ne lit pendant une session série, et le shell la trouve en attente à son retour. Correction (option (a), choix d'Éric du 2026-11-19) : `console_fermer` écrit dans `$C010` après `CloseGS` (IMP-0283). L'essai 5.11 bis, conforme sur l'Apple IIGS le même jour, confirme cette lecture. Tous les essais de `LSE -SERIE` sont conformes.

Essai 5.14 : le changement de slot n'a été vu qu'après un redémarrage (§0).
