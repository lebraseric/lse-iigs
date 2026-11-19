#!/usr/bin/env python3
"""pont_serie.py — paire de pseudo-terminaux reliés, pour essayer le port série
sous un émulateur (GSSquared) sans câble.

Crée deux pseudo-terminaux, A et B, et recopie octet pour octet tout ce qui
entre par l'un vers l'autre, sans aucune traduction (mode brut : ni écho, ni
CR/LF, ni XON/XOFF ; Ctrl-S et Ctrl-Q passent). Les liens symboliques
DOSSIER/emulateur et DOSSIER/terminal désignent A et B.

  python3 -I scripts/pont_serie.py [-v] [DOSSIER]      (défaut : /tmp/lse-serie)

-v : trace en hexadécimal de chaque passage, avec son sens, et relevé des
drapeaux IXON, IXOFF, IXANY, ICANON, ECHO de chaque côté quand un client les
change : un côté en IXON consomme Ctrl-S et Ctrl-Q avant le client.

Côté émulateur : port modem relié à DOSSIER/emulateur (connexion « serial »).
Côté Mac : screen -fn DOSSIER/terminal 9600,cs8,-ixon,-ixoff
(-fn : sans contrôle de flux de screen, qui garderait Ctrl-S et Ctrl-Q)
Ctrl-C arrête le pont. Essais : 05-IMPLEMENTATION/audits/Essais_IIGS_2026-11-19.md.
"""

import os
import select
import sys
import termios
import tty


def paire():
    maitre, esclave = os.openpty()
    tty.setraw(esclave)
    attr = termios.tcgetattr(esclave)
    attr[0] &= ~(termios.IXON | termios.IXOFF | termios.IXANY)
    termios.tcsetattr(esclave, termios.TCSANOW, attr)
    return maitre, esclave, os.ttyname(esclave)


def lier(lien, cible):
    try:
        os.unlink(lien)
    except FileNotFoundError:
        pass
    os.symlink(cible, lien)


def drapeaux(fd):
    try:
        a = termios.tcgetattr(fd)
    except termios.error:
        return "?"
    noms = []
    for nom, champ, bit in (("IXON", 0, termios.IXON), ("IXOFF", 0, termios.IXOFF),
                            ("IXANY", 0, termios.IXANY), ("ICANON", 3, termios.ICANON),
                            ("ECHO", 3, termios.ECHO)):
        if a[champ] & bit:
            noms.append(nom)
    return " ".join(noms) if noms else "brut"


def main():
    args = sys.argv[1:]
    trace = "-v" in args
    args = [a for a in args if a != "-v"]
    dossier = args[0] if args else "/tmp/lse-serie"
    os.makedirs(dossier, exist_ok=True)
    ma, ea, na = paire()
    mb, eb, nb = paire()
    lier(os.path.join(dossier, "emulateur"), na)
    lier(os.path.join(dossier, "terminal"), nb)
    print(f"emulateur : {os.path.join(dossier, 'emulateur')} -> {na}")
    print(f"terminal  : {os.path.join(dossier, 'terminal')} -> {nb}")
    print("Ctrl-C pour arreter.", flush=True)
    # Les esclaves restent ouverts ici : un client qui ferme puis rouvre son
    # côté ne provoque pas d'erreur EIO sur le maître.
    vers = {ma: mb, mb: ma}
    sens = {ma: "emulateur -> terminal", mb: "terminal -> emulateur"}
    compte = {ma: 0, mb: 0}
    cote = {ea: "emulateur", eb: "terminal"}
    etat = {ea: drapeaux(ea), eb: drapeaux(eb)}
    if trace:
        for fd in (ea, eb):
            print(f"[{cote[fd]}] {etat[fd]}", flush=True)
    try:
        while True:
            prets, _, _ = select.select([ma, mb], [], [], 0.5)
            if trace:
                for fd in (ea, eb):
                    d = drapeaux(fd)
                    if d != etat[fd]:
                        etat[fd] = d
                        print(f"[{cote[fd]}] {d}", flush=True)
            for fd in prets:
                try:
                    donnees = os.read(fd, 4096)
                except OSError:
                    continue
                if donnees:
                    if trace:
                        print(f"{sens[fd]} : {donnees.hex(' ')}", flush=True)
                    os.write(vers[fd], donnees)
                    compte[fd] += len(donnees)
    except KeyboardInterrupt:
        pass
    finally:
        for fd in (ma, mb):
            print(f"{sens[fd]} : {compte[fd]} octets")
        for fd in (ma, mb, ea, eb):
            os.close(fd)


if __name__ == "__main__":
    main()
