# Analyse des réels du T1600 — 2026-11-05

Objet : point 5 de la liste des modifications vers le LSE de Victor Duruy (arithmétique). Avec la réponse 7, `NBOR` imprime `14 ØPERATIØNS` ; LSE-IIGS, qui calculait sur les réels de 24 bits de l'IEEE (IMP-0216), en obtenait 16, et quatre valeurs différaient. Conclusion : les réels du T1600 avaient 23 bits significatifs. Décision : Interpréteur IMP-0280 (choix d'Éric du 2026-11-05).

Essais : `Analyse_reels_T1600_2026-11-05_essais/` (`modele.py`, `temoins.py`, `confronter.py` et sa sortie `confronter.txt`, `sonde23.c`). Commande : `python3 -I confronter.py`, depuis ce répertoire.

## 1. Sources

- **INRDP75**, T1600 version 4, 1974 : 16 résultats imprimés n'employant que `+ − × ÷` et des puissances entières :
  - §3-4, PG16 : `MOYENNE = 8.200001` (82/10) ;
  - PG24 : `10.16666` (122/12) ; PG27 : `8.461538` (110/13) ;
  - PG4 : `TOTO=2.201199E+09 A/B=-2.434332` ;
  - PG7 : `S=993.9886` ; PG19 (Horner) : `7.152624` ; PG58 : `73.10001` ;
  - §4-3 (page 33, session du 30/08/74) : 9 valeurs, de `32.47562` à `-66.28627`.
- **INRDP75**, même session du §4-3 : 9 autres résultats, avec `RAC`, `EXP`, `LGN`, `SIN`, `COS`, `ATG`, `ENT`, `ABS` et une puissance non entière.
- **INRDP75 §3-3**, remarque sur la représentation des nombres : un nombre occupe « 2 mots mémoire (ou 4 octets) [...] sous forme d'une mantisse [...] et d'un exposant (puissance de 10) » ; « Les constructeurs garantissent 6 chiffres significatifs » ; « il ne faudrait pas s'étonner que le nombre 0,4 soit donné par la machine sous la forme 0,3999999 ».
- **INRDP75 §4-3**, note 4 : « Si la puissance est un nombre entier, le calcul est fait par multiplications successives ; sinon il y a passage par le logarithme et l'exponentielle. »
- **NBOR** (5ème, 1979-80, bannière `LSE F4`), `02-HISTORIQUE/Programmes LSE T-1600 Victor Duruy 1978-1983/5EME - NBOR/numerisation0024.pdf`, transcription `02-HISTORIQUE/Transcriptions-T1600-Victor-Duruy/NBOR.lse` : réponse 7, quatorze valeurs de `3.645752` à `2.618034`, puis `1.618034` et `14 ØPERATIØNS`.

## 2. Méthode

`modele.py` calcule en arithmétique exacte (`fractions.Fraction`) une machine dont on fait varier :
- la largeur de la mantisse binaire, de 20 à 26 bits ;
- l'arrondi des opérations et des constantes : au plus près, la demie au pair (RN) ; au plus près, la demie en s'éloignant de zéro (RNA) ; par troncature (RZ) ;
- l'impression de `U`, à 7 chiffres significatifs : arrondie (RN), tronquée (RZ), ou par mise à l'échelle dans les réels de la machine (ECH, ECH10, DIG) ;
- la puissance entière : multiplications de gauche à droite (`inv`), carrés successifs (`carres`), ou un seul arrondi de la valeur exacte (`exacte`, ce que fait LSE-IIGS : `pow` en `double`, puis arrondi).

`temoins.py` rejoue les 16 témoins d'INRDP75 de §1 ; `confronter.py` y ajoute `NBOR` (racine exacte à 120 bits, puis arrondie).

## 3. Résultats

Impression arrondie à 7 chiffres, puissance `exacte` (extrait de `confronter.txt`) :

| Bits | Arrondi | INRDP75 (16) | `NBOR` : opérations | `NBOR` : valeurs (14) |
|---|---|---|---|---|
| 20 | RN / RNA | 3 / 3 | 12 / 13 | 4 / 4 |
| 21 | RN / RNA | 5 / 5 | 13 / 13 | 5 / 6 |
| 22 | RN / RNA | 3 / 3 | 13 / 14 | 5 / 9 |
| **23** | **RN / RNA** | **12 / 12** | **13 / 14** | **8 / 13** |
| 24 | RN / RNA | 4 / 4 | 16 / 15 | 10 / 10 |
| 25 | RN / RNA | 5 / 4 | 16 / 16 | 9 / 10 |
| 26 | RN / RNA | 3 / 3 | 15 / 16 | 9 / 9 |

- La largeur ne fait pas de doute : 23 bits l'emportent sur toutes les autres, pour les deux corpus.
- L'arrondi de la demie départage `NBOR` : en s'éloignant de zéro, 14 opérations, comme la feuille ; au pair, 13.
- La mantisse décimale que suggère INRDP75 §3-3 (« exposant (puissance de 10) ») est exclue : 82/10 y serait exact, et la machine imprime `8.200001`.

Variantes d'impression, sur 23 bits RNA, puissance `exacte` :

| Impression | INRDP75 | `NBOR` : valeurs |
|---|---|---|
| **arrondie (RN)** | **12** | **13** |
| tronquée (RZ) | 12 | 7 |
| mise à l'échelle (ECH) | 14 | 9 |
| par dix successifs (ECH10) | 10 | 10 |
| chiffre par chiffre (DIG) | 11 | 7 |

La mise à l'échelle explique deux témoins de plus d'INRDP75, mais fait tomber `NBOR` à 9 : elle n'est pas retenue, faute de pouvoir l'établir pour F4.

Puissances entières, sur 23 bits RNA : `carres` et `exacte` donnent 12 sur 16, `inv` 11 : `PSI*R1*(XA+2)/(A+B)↑(2*K)` y devient `-9.701531E-05` au lieu de `-9.701532E-05`. Les « multiplications successives » de la note 4 ne sont donc pas faites une à une de gauche à droite, ou pas arrondies à chaque pas ; les témoins ne départagent pas les carrés successifs et l'arrondi unique.

## 4. Modèle retenu et écarts

23 bits significatifs, la demie en s'éloignant de zéro, impression arrondie à 7 chiffres, puissance entière arrondie une fois (IMP-0280). Écarts restants :
- INRDP75 : `10.16666` (`10.16667`), `2.201199E+09` (`2.2012E+09`), `-2.434332` (`-2.434333`), `30.14374` (`30.14375`) ;
- `NBOR` : la première valeur, `3.645752` (`3.645751`) : 1 + √7 vaut 3,6457513 ; la racine du T1600 rendait vraisemblablement un dernier bit de plus.

## 5. Vérification sur `LSE`

Après réalisation (2026-11-05, Golden Gate, `build/LSE` du profil `cible`) :
- `NBOR`, programme transcrit, réponse 7 : 14 opérations sous `LSE F4` et `-T1600`, 13 valeurs sur 14 ; 16 opérations sous `-MITRA15`, inchangé ;
- session du §4-3 d'INRDP75 (18 valeurs, fonctions comprises) : 14 reproduites sous `LSE F4` et `-T1600`, 9 avec les réels de 24 bits. Restent `30.14374`, `4.6089` (`LGN(ZP↑0.25)` ; le passage par le logarithme et l'exponentielle arrondis à 23 bits donne aussi `4.608899`), `-1.416871` (`-1.416872`) et `-0.9561998` (`-0.9562017`) ;
- suites `representation_rcim` (routine d'arrondi) et `composants_entrees_sorties` (témoins, `NBOR`, constantes, `LIRE`, entiers autour de 2²³, `ALE`) ;
- témoin versé après la décision : le dossier dactylographié de PRO et PRON2 (`4EME - PRO/Exécution PRO-PRON2.pdf`) imprime la note `9.565216 /20`, soit `11/(23/20)` ; LSE-IIGS rend `9.565216` sous `LSE F4` et `-T1600`, `9.565218` sous `-MITRA15` ; le calcul exact donne `9.565217`.

## 6. Coût

`sonde23.c`, mesuré par `iix --cycles` en profil `cible` : 277 cycles par arrondi, soit 0,024 ms avec l'étalonnage de l'audit du code. Cette mesure ne compte pas les appels à SANE. Mesuré sur l'Apple IIGS le 2026-11-05 (`PERF`, essai 3.6 de `Essais_IIGS_2026-11-05.md`) : +0,433 ms par opération et +5,8 % sur la boucle (e), la première réalisation comparant le résultat à `FLT_MAX` par deux comparaisons en format étendu ; remplacées le même jour par un test sur les bits du `float` ; remesuré : `1.5/3` +0,033 ms, `0.1+0.2` +0,067 ms, boucle (e) +1,1 % (résolution de la sonde : 0,033 ms). Mémoire de données : aucune ; un réel reste un `float` de 4 octets, dont le dernier bit de mantisse est nul.
