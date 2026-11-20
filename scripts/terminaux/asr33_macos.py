#!/usr/bin/env python3
"""asr33_macos.py — lance asr33emu sous macOS avec la police Teletype33.

  python3 -I scripts/terminaux/asr33_macos.py DOSSIER_ASR33EMU [--manuel] [options d'asr33emu…]

Sous macOS, le frontal Tk d'asr33emu (commit 4428033) n'enregistre pas sa police
(register_font ne connaît que Windows et Linux) et retombe sur Courier ; le
frontal Pygame, qui charge la police lui-même, plante à l'ouverture des panneaux
du ruban (SDL et Tk 9 se disputent l'application Cocoa). Ce lanceur remplace
register_font par un enregistrement auprès de CoreText, limité au processus
(rien n'est installé dans ~/Library/Fonts). Tk sous macOS ne donne aucun
caractère pour RETURN, ESC, delete ni les combinaisons Ctrl : le lanceur les
reconnaît par leur keysym (corriger_clavier). Il exécute ensuite asr33emu.py.
Le clone d'asr33emu n'est pas modifié. Appelé par scripts/terminaux/asr33.sh.
Les touches mortes (^ du clavier français) sont retenues jusqu'à la frappe
suivante, Tk en rendant deux fois le caractère. Les panneaux du ruban, que
Tk 9.1 laisse blancs, sont retaillés d'un point après leur affichage.
Les ordres de la télétype sont réalisés (corriger_commandes) : DC2 reçu met
le perforateur en marche, DC4 l'arrête, X-ON (DC1) met le lecteur en marche,
NAK ($15, EQC(21) d'INRDP75) l'arrête, et le lecteur s'arrête de lui-même
après avoir lu un X-OFF ; ces quatre ordres ne sont pas perforés. Pour
LSE -TTY (IMP-0284). Le bouton Start du lecteur tient lieu du levier START :
X-ON ne relance le lecteur que s'il y est. --manuel supprime ces ordres :
lecteur et perforateur conduits à la main seulement, comme dans asr33emu
(LSE -SERIE). La file de réception est sans borne (corriger_reception) : rien
de ce qui arrive n'est perdu, quelle que soit l'avance de la ligne.
ASR33_TOUCHES=1 : trace de chaque touche (keysym, char, state, time) et des
octets envoyés.
"""

import ctypes
import ctypes.util
import os
import runpy
import sys
import unicodedata

K_CT_FONT_MANAGER_SCOPE_PROCESS = 1


def enregistrer_police(chemin):
    """Enregistre un fichier de police pour ce processus ; vrai si réussi."""
    if sys.platform != "darwin":
        return False
    cf = ctypes.cdll.LoadLibrary(ctypes.util.find_library("CoreFoundation"))
    ct = ctypes.cdll.LoadLibrary(ctypes.util.find_library("CoreText"))
    cf.CFURLCreateFromFileSystemRepresentation.restype = ctypes.c_void_p
    cf.CFURLCreateFromFileSystemRepresentation.argtypes = [
        ctypes.c_void_p, ctypes.c_char_p, ctypes.c_long, ctypes.c_bool]
    cf.CFRelease.argtypes = [ctypes.c_void_p]
    ct.CTFontManagerRegisterFontsForURL.restype = ctypes.c_bool
    ct.CTFontManagerRegisterFontsForURL.argtypes = [
        ctypes.c_void_p, ctypes.c_uint32, ctypes.c_void_p]
    octets = os.path.abspath(chemin).encode()
    url = cf.CFURLCreateFromFileSystemRepresentation(None, octets, len(octets), False)
    if not url:
        return False
    try:
        return bool(ct.CTFontManagerRegisterFontsForURL(
            url, K_CT_FONT_MANAGER_SCOPE_PROCESS, None))
    finally:
        cf.CFRelease(url)


# Touches que Tk sous macOS rend sans caractère (event.char vide) : asr33emu
# ne les transmettait pas (constat d'Éric du 2026-11-19 sur le pont, -v).
# La touche « delete » du Mac (BackSpace pour Tk) et fn-delete (Delete) donnent
# RUB-OUT ($7F), comme l'application Terminal (essai 5.6) et la touche RUB OUT
# de l'ASR-33.
TOUCHES = {
    "Return": 0x0D,
    "KP_Enter": 0x0D,
    "Escape": 0x1B,
    "BackSpace": 0x7F,
    "Delete": 0x7F,
    "Tab": 0x09,
    "Linefeed": 0x0A,
}
MASQUE_CONTROLE = 0x0004      # ControlMask de Tk, sur toutes les plates-formes


def octet_touche(event):
    """Octet d'une touche de commande ou d'une combinaison Ctrl, sinon None."""
    keysym = event.keysym or ""
    if keysym in TOUCHES:
        return TOUCHES[keysym]
    if event.state & MASQUE_CONTROLE and len(keysym) == 1:
        code = ord(keysym.upper())
        if 0x40 <= code <= 0x5F:          # Ctrl-@ à Ctrl-_ : $00 à $1F
            return code - 0x40
    return None


# Touches mortes du clavier du Mac (clavier français : ^ ; ` et ~ selon la
# disposition). Tk rend le caractère de la touche morte une première fois à sa
# frappe, puis de nouveau, en tête du résultat de la composition, à la frappe
# suivante : `^` puis `1` donne trois événements `^`, `^`, `1` (constat d'Éric
# du 2026-11-19, ASR33_TOUCHES=1). Rien ne distingue le premier : il est
# retenu, et n'est envoyé qu'avec la frappe suivante.
MORTES = "^`~"
DIACRITIQUES = {"\u0302": "^", "\u0300": "`", "\u0303": "~"}


def decomposer(caractere):
    """Lettre ASCII et signe d'un caractère composé (ê → 'e', '^'), ou None."""
    forme = unicodedata.normalize("NFD", caractere)
    if len(forme) == 2 and forme[0].isascii() and forme[1] in DIACRITIQUES:
        return forme[0], DIACRITIQUES[forme[1]]
    return None


def corriger_clavier(classe):
    """Remplace _keypress du frontal Tk : commandes et Ctrl par le keysym,
    touches mortes retenues jusqu'à la frappe suivante."""
    trace = bool(os.environ.get("ASR33_TOUCHES"))

    def envoyer(self, octet):
        if self.keyboard_parity_mode == "even":
            donnee = self._term.encode_even_parity(bytes([octet]))
        elif self.keyboard_parity_mode == "mark":
            donnee = bytes([octet | 0x80])
        else:
            donnee = bytes([octet & 0x7F])
        self._backend.send_data(donnee)
        if trace:
            print(f"  envoyé ${donnee[0]:02X}", file=sys.stderr, flush=True)

    def envoyer_texte(self, texte):
        for caractere in texte:
            if self.keyboard_uppercase_only:
                caractere = caractere.upper()
            envoyer(self, ord(caractere))

    def _keypress(self, event):
        octet = octet_touche(event)
        texte = event.char or ""
        temps = getattr(event, "time", None)
        morte = getattr(self, "_lse_morte", None)
        if trace:
            print(f"touche keysym={event.keysym!r} char={texte!r} "
                  f"state={event.state:#x} time={temps}",
                  file=sys.stderr, flush=True)
        if octet is None and not texte:
            return None                   # touche de modification seule
        if octet is None and len(texte) == 1 and texte in MORTES:
            if morte is None and temps != getattr(self, "_lse_temps", None):
                self._lse_morte = texte   # frappe de la touche morte : retenue
                return "break"
            # Résultat d'une composition (première frappe rendue, ou suite
            # du même résultat, de même instant) : envoyé.
            self._lse_morte = None
            self._lse_temps = temps
            envoyer_texte(self, texte)
        else:
            if morte is not None:
                self._lse_morte = None
                if octet is None and len(texte) == 1 and not texte.isascii():
                    paire = decomposer(texte)
                    if paire is not None:  # ê : ^ puis E
                        envoyer_texte(self, paire[1] + paire[0])
                        texte = ""
                    else:
                        envoyer_texte(self, morte)
                else:
                    envoyer_texte(self, morte)  # la touche n'était pas morte
            if octet is not None:
                envoyer(self, octet)
            elif texte.isascii():
                envoyer_texte(self, texte)
            # autre caractère non ASCII (touche accentuée) : rien n'est envoyé
        self.screen_top_lln = None
        if self._sounds is not None and hasattr(self._sounds, "keypress"):
            self._sounds.keypress()
        return "break"

    classe._keypress = _keypress          # pylint: disable=protected-access


# Panneaux du lecteur (F1) et du perforateur (F3) : sous macOS, Tk 9.1 les
# affiche blancs, bien que tous leurs éléments soient en place. Seul un
# changement de taille après l'affichage les fait dessiner (essai d'Éric du
# 2026-11-19, quatre variantes : rattachement « transient » ôté, fenêtre cachée
# puis remontrée, sans effet ; élargie d'un point puis remise à sa taille,
# contenu affiché).
DELAI_RETAILLE_MS = 300


def corriger_panneaux(classe):
    """Après chaque deiconify d'un PapertapeViewer, retaille la fenêtre."""
    origine = classe.deiconify

    def retailler(fenetre):
        largeur, hauteur = fenetre.winfo_width(), fenetre.winfo_height()
        fenetre.resizable(True, True)
        fenetre.geometry(f"{largeur + 1}x{hauteur}")
        fenetre.update_idletasks()
        fenetre.geometry(f"{largeur}x{hauteur}")

    def deiconify(self):
        resultat = origine(self)
        self.after(DELAI_RETAILLE_MS, lambda: retailler(self))
        return resultat

    classe.deiconify = deiconify


# Ordres de la télétype ASR-33 (INRDP75, annexe V) : EQC(17) met le lecteur en
# route, EQC(18) le perforateur, EQC(20) arrête le perforateur, EQC(21) le
# lecteur ; le lecteur s'arrête aussi de lui-même sur le X-OFF qu'il lit
# (INRDP75 p. 193 : il « peut ne pas s'arrêter immédiatement dès le ⓧ »).
# asr33emu (commit 4428033) n'obéit à aucun : son lecteur et son perforateur ne
# se commandent qu'à la main, et le perforateur enregistre tout ce qui arrive,
# ordres et écho de la commande PERFORER compris (essai 7.5 du 2026-11-20).
# Choix de l'émulation, non attesté : les quatre ordres ne sont pas perforés
# (le dessin du ruban d'INRDP75 p. 188 ne montre que des nuls avant CR LF).
DC1, DC2, DC3, DC4, NAK = 0x11, 0x12, 0x13, 0x14, 0x15


def marche(organe):
    """Met en marche le perforateur, s'il a un ruban, ou le lecteur, si son
    levier est sur START."""
    vue = organe.papertape_viewer
    if vue is None or (vue.mode == "reader" and not getattr(organe, "_lse_arme", False)):
        return
    if organe.on():
        vue._lse_demande = True                         # boutons mis à jour par Tk


def arret(organe):
    """Arrête le lecteur ou le perforateur."""
    if organe.papertape_viewer is not None and organe.off():
        organe.papertape_viewer._lse_demande = True


def corriger_commandes(frontal, panneau, lecteur):
    """Réalise les ordres de la télétype (fil de réception et fil du lecteur ;
    les boutons des panneaux sont mis à jour par la boucle de Tk)."""

    def receive_data(self, data):
        debut = 0
        for i, octet in enumerate(data):
            c = octet & 0x7F
            if c not in (DC1, DC2, DC4, NAK):
                continue
            if i > debut:
                self.papertape_punch.punch_bytes(data[debut:i])
            debut = i + 1
            if c == DC2:
                marche(self.papertape_punch)
            elif c == DC4:
                arret(self.papertape_punch)
            elif c == DC1:
                marche(self.papertape_reader)
            else:
                arret(self.papertape_reader)
        if len(data) > debut:
            self.papertape_punch.punch_bytes(data[debut:])
        self.display_update_needed = True

    frontal.receive_data = receive_data

    # Levier du lecteur de l'ASR-33 (START, STOP, FREE) : sur START, le lecteur
    # obéit à X-ON et s'arrête sur X-OFF ; sur STOP, X-ON est sans effet. Le
    # bouton Start du panneau met le levier sur START, Stop sur STOP ; la fin
    # du ruban le remet sur STOP. Le panneau montre le levier, non l'arrêt
    # momentané sur un X-OFF.
    origine_start = panneau.on_button_click
    origine_stop = panneau.off_button_click

    def on_button_click(self):
        self.outer._lse_arme = True
        return origine_start(self)

    def off_button_click(self):
        self.outer._lse_arme = False
        return origine_stop(self)

    panneau.on_button_click = on_button_click
    panneau.off_button_click = off_button_click

    origine_traiter = panneau.process_viewer

    def process_viewer(self, tape_loaded):
        # Une demande en attente (ordre reçu, ou arrêt du lecteur par son fil)
        # : les boutons suivent l'état réel, qui a pu changer plusieurs fois
        # depuis le dernier passage.
        if getattr(self, "_lse_demande", False) or self._safe_off_request:
            self._lse_demande = False
            self._safe_off_request = False
            marche_ = self.outer.active or getattr(self.outer, "_lse_arme", False)
            self.set_button_state("on" if marche_ else "off")
        return origine_traiter(self, tape_loaded)

    panneau.process_viewer = process_viewer

    origine_fin = lecteur._end_check

    def _end_check(self, position):
        if origine_fin(self, position):
            self._lse_arme = False                 # fin du ruban : levier sur STOP
            return True
        if (position > 0 and position != getattr(self, "_lse_arret", None)
                and self.tape_data[position - 1] & 0x7F == DC3):
            self._lse_arret = position             # une fois par X-OFF lu
            self.stop_cause = "x_off"
            return True
        return False

    lecteur._end_check = _end_check


def corriger_reception(limiteur):
    """File de réception du limiteur de débit sans borne.

    asr33emu la borne à 8 blocs, pour ne pas laisser la ligne devancer
    l'impression : pleine, elle bloque la lecture du port, et ce qui arrive
    ensuite est perdu, faute de contrôle de flux. Essai 7.7 du 2026-11-20 : PERFORER
    de PRO sous GSSquared, environ 2 400 octets perdus au milieu de la ligne 102
    (le pseudo-terminal du pont refuse l'écriture au-delà de 1 022 octets en
    attente). Une ASR-33 reliée à 110 bauds n'a pas ce problème, la ligne
    allant au pas de l'impression ; à 9 600 bauds, l'émulateur doit tout garder."""
    origine = limiteur.__init__

    def __init__(self, *args, **kwargs):
        kwargs["receive_queue_size"] = 0          # sans borne
        origine(self, *args, **kwargs)

    limiteur.__init__ = __init__


def main():
    if len(sys.argv) < 2:
        print(__doc__.strip().splitlines()[2].strip(), file=sys.stderr)
        sys.exit(2)
    emu = os.path.abspath(sys.argv[1])
    manuel = "--manuel" in sys.argv[2:]
    options = [a for a in sys.argv[2:] if a != "--manuel"]
    os.chdir(emu)                 # police et sons cherchés depuis ce dossier
    sys.path.insert(0, emu)
    import asr33_frontend_tk      # pylint: disable=import-outside-toplevel
    asr33_frontend_tk.register_font = enregistrer_police
    corriger_clavier(asr33_frontend_tk.ASR33TkFrontend)
    import asr33_pt_animate_tk    # pylint: disable=import-outside-toplevel
    corriger_panneaux(asr33_pt_animate_tk.PapertapeViewer)
    import asr33_shim_throttle        # pylint: disable=import-outside-toplevel
    corriger_reception(asr33_shim_throttle.DataThrottle)
    if not manuel:
        import asr33_papertape        # pylint: disable=import-outside-toplevel
        corriger_commandes(asr33_frontend_tk.ASR33TkFrontend,
                           asr33_pt_animate_tk.PapertapeViewer,
                           asr33_papertape.PapertapeReader)
    sys.argv = [os.path.join(emu, "asr33emu.py")] + options
    runpy.run_path(sys.argv[0], run_name="__main__")


if __name__ == "__main__":
    main()
