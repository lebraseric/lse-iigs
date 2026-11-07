#!/usr/bin/env python3
# encre.py - relit une transcription (NOM.lse) sur la numerisation de son listage,
# colonne par colonne : chaque colonne imprimee porte de l'encre ou non, et la
# transcription y attend un caractere ou une espace. Une espace en trop ou en
# moins decale toute la suite de la ligne et se voit aussitot.
# Emploi : python3 -I encre.py NOM.lse LISTAGE.pdf[:pages] [LISTAGE2.pdf[:pages] ...]
#   pages : 1, 2 ou 1-2 (toutes par defaut).
# Necessite pdftoppm (poppler). Rendu a 200 points par pouce : 20 px par
# colonne et 33,3 px par ligne pour la teletype (10 caracteres et 6 lignes par
# pouce) ; le pas et la marge de chaque ligne imprimee sont ajustes.
# Limites : un signe maigre (. , ' : ;) peut ne pas laisser assez d'encre ; il
# n'est donc jamais compte comme manquant. Les colonnes au-dela de la 71e
# (surimpression) ne sont pas comparees.
import os, subprocess, sys, tempfile

PONCT = set(".,':;-`")

def pages_ppm(pdf, sel, rep):
    args = ['pdftoppm', '-r', '200']
    if sel:
        a, _, b = sel.partition('-')
        args += ['-f', a, '-l', b or a]
    base = os.path.join(rep, 'p%d' % len(os.listdir(rep)))
    subprocess.check_call(args + [pdf, base])
    return sorted(os.path.join(rep, f) for f in os.listdir(rep) if f.startswith(os.path.basename(base)))

def lire(p):
    d = open(p, 'rb').read()
    parts = d.split(maxsplit=4)
    w, h = int(parts[1]), int(parts[2])
    return w, h, parts[4][:w * h * 3]

class Page:
    def __init__(self, chemin):
        self.w, self.h, px = lire(chemin)
        n = self.w * self.h
        # teinte du papier : mediane de la luminance et de l'ecart rouge-vert
        lum = sorted(px[3 * i] + px[3 * i + 1] + px[3 * i + 2] for i in range(0, n, 97))
        rg = sorted(px[3 * i] - px[3 * i + 1] for i in range(0, n, 97))
        fond, fond_rg = lum[len(lum) // 2], rg[len(rg) // 2]
        # encre : nettement plus sombre que le papier, ou nettement plus rouge
        self.m = bytearray(n)
        for i in range(n):
            r, g, b = px[3 * i], px[3 * i + 1], px[3 * i + 2]
            if r + g + b < fond - 150 or (r - g > fond_rg + 70 and r + g + b < fond - 60):
                self.m[i] = 1
        self.lignes = self.rangees()
        self.profils = {l: self.colonnes(*l) for l in self.lignes}
        self.grille()

    def grille(self):
        """Pas et phase de la grille de la page : les blancs entre caracteres
        tombent sur les bords des cellules."""
        meilleur = None
        for pas100 in range(1940, 2081, 5):
            pas = pas100 / 100.0
            for phase in range(0, 21):
                bord = 0
                for cols in self.profils.values():
                    k = 0
                    while True:
                        x = int(phase + k * pas)
                        if x >= len(cols) - 60:
                            break
                        bord += sum(cols[max(0, x - 1):x + 2])
                        k += 1
                if meilleur is None or bord < meilleur[0]:
                    meilleur = (bord, pas, phase)
        self.pas, self.phase = meilleur[1], meilleur[2]

    def cellules(self, ligne, n):
        """Encre des n cellules d'une ligne, depuis celle de son premier caractere."""
        cols = self.profils[ligne]
        # debut et fin du texte : encre soutenue sur 8 pixels (une poussiere
        # isolee ne compte pas)
        xs = [x for x in range(len(cols) - 8) if cols[x] > 2 and sum(cols[x:x + 8]) > 30]
        if not xs:
            return []
        pas = self.pas
        # phase propre a la ligne : bords de cellules sur les blancs, a partir
        # d'un bord place juste avant le premier caractere
        meilleur = None
        for d10 in range(0, int(pas * 10), 5):
            x0 = xs[0] - pas + d10 / 10.0
            bord = 0
            k = 0
            while x0 + k * pas < xs[-1]:
                x = int(x0 + k * pas)
                bord += sum(cols[max(0, x - 1):x + 2])
                k += 1
            if meilleur is None or bord < meilleur[0]:
                meilleur = (bord, x0)
        x0 = meilleur[1]
        while x0 + pas <= xs[0]:
            x0 += pas
        res = []
        for k in range(n):
            xa, xb = int(x0 + k * pas + 2), int(x0 + (k + 1) * pas - 2)
            if xb >= len(cols) - 60:
                break
            res.append(sum(cols[xa:xb]))
        return res

    def rangees(self):
        w = self.w
        prof = [sum(self.m[y * w + 40:y * w + w - 60]) for y in range(self.h)]
        res, y = [], 0
        while y < self.h:
            if prof[y] > 6:
                y0 = y
                while y < self.h and prof[y] > 6:
                    y += 1
                if 12 < y - y0 < 45:
                    res.append((y0, y))
                elif y - y0 >= 45:
                    # bande trop haute (pli, rangees jointives) : coupee aux
                    # minimums du profil, environ toutes les 33 lignes de pixels
                    a = y0
                    while y - a >= 45:
                        b = min(range(a + 22, min(a + 42, y)), key=lambda v: prof[v])
                        res.append((a, b))
                        a = b + 1
                    if y - a > 12:
                        res.append((a, y))
            y += 1
        return res

    def colonnes(self, y0, y1):
        w = self.w
        cols = [0] * w
        for y in range(max(0, y0 - 2), min(self.h, y1 + 2)):
            ligne = self.m[y * w:(y + 1) * w]
            for x in range(40, w - 60):
                if ligne[x]:
                    cols[x] += 1
        return cols

def carte(cols, x0, pas, n):
    res = []
    for c in range(n):
        xa, xb = int(x0 + c * pas + 3), int(x0 + c * pas + pas - 3)
        if xb >= len(cols) - 60:
            break
        res.append(sum(cols[xa:xb]))
    return res

def comparer(texte, encre):
    """(accords, ecarts) ; un signe maigre absent n'est pas un ecart."""
    ok, ecarts = 0, []
    for c, (ch, e) in enumerate(zip(texte, encre)):
        a_encre = e > 12
        attendu = ch != ' '
        if a_encre == attendu or (attendu and not a_encre and ch in PONCT):
            ok += 1
        else:
            ecarts.append(c)
    return ok, ecarts

def ajuster(cols, texte):
    xs = [x for x in range(len(cols)) if cols[x] > 2]
    if not xs:
        return None
    meilleur = None
    for dx in range(-6, 7, 2):
        for pas10 in range(190, 214, 2):
            pas = pas10 / 10.0
            enc = carte(cols, xs[0] + dx, pas, len(texte))
            ok, ec = comparer(texte, enc)
            score = ok - 0.001 * abs(pas - 20)
            if meilleur is None or score > meilleur[0]:
                meilleur = (score, ec, enc, xs[0] + dx, pas)
    return meilleur

def main():
    lse = sys.argv[1]
    textes = [l.rstrip('\n') for l in open(lse, encoding='utf-8') if l.strip()]
    with tempfile.TemporaryDirectory() as rep:
        pages = []
        for a in sys.argv[2:]:
            pdf, _, sel = a.rpartition(':') if not a.endswith('.pdf') else (a, '', '')
            for p in pages_ppm(pdf, sel, rep):
                pages.append(Page(p))
        rangs = [(pg, y0, y1) for pg in pages for (y0, y1) in pg.lignes]
        encres = [pg.cellules((y0, y1), 71) for pg, y0, y1 in rangs]
        vis = [t[:71] for t in textes]
        n, m = len(textes), len(rangs)
        def note(a, b):
            if len(encres[b]) < len(vis[a]):
                return -len(vis[a]), None
            ok, ec = comparer(vis[a], encres[b])
            return ok - len(vis[a]), ec
        # alignement global, dans l'ordre : D[a][b] = meilleur total pour les a
        # premieres lignes et les b premieres rangees
        NEG = -10 ** 9
        D = [[NEG] * (m + 1) for _ in range(n + 1)]
        choix = [[None] * (m + 1) for _ in range(n + 1)]
        for b in range(m + 1):
            D[0][b] = 0
        notes = {}
        for a in range(1, n + 1):
            D[a][0] = D[a - 1][0] - len(vis[a - 1])
            choix[a][0] = 'ligne'
            for b in range(1, m + 1):
                if (a - 1, b - 1) not in notes:
                    notes[(a - 1, b - 1)] = note(a - 1, b - 1)
                cands = [(D[a][b - 1], 'rangee'),
                         (D[a - 1][b - 1] + notes[(a - 1, b - 1)][0], 'paire'),
                         (D[a - 1][b] - len(vis[a - 1]), 'ligne')]
                D[a][b], choix[a][b] = max(cands, key=lambda c: c[0])
        paires = {}
        a, b = n, m
        while a > 0:
            c = choix[a][b]
            if c == 'paire':
                paires[a - 1] = b - 1
                a, b = a - 1, b - 1
            elif c == 'rangee':
                b -= 1
            else:
                a -= 1
        sans, avec, absentes = 0, [], []
        for a, t in enumerate(textes):
            b = paires.get(a)
            sc, ec = note(a, b) if b is not None else (-len(vis[a]), None)
            if b is None or sc < -0.2 * len(vis[a]):
                best = None
                if b is not None:
                    best = (len(vis[a]) + sc, ec, encres[b][:len(vis[a])])
                absentes.append((t, best))
            elif ec:
                avec.append((t, ec, ''.join('#' if e > 12 else '.' for e in encres[b][:len(vis[a])]), rangs[b][0].pas, pages.index(rangs[b][0]) + 1, rangs[b][1]))
            else:
                sans += 1
        print('%s : %d lignes concordantes, %d avec ecart, %d non retrouvees'
              % (os.path.basename(lse), sans, len(avec), len(absentes)))
        for t, ec, enc, pas, npage, y in avec:
            print('  ecart colonne(s) %s (page %d, y %d px a 200 ppp, pas %.1f)' % (','.join(str(c + 1) for c in ec), npage, y, pas))
            print('    texte  ' + ''.join('.' if ch == ' ' else '#' for ch in t[:len(enc)]) + '   ' + t[:71])
            print('    encre  ' + enc)
        for t, b in absentes:
            print('  non retrouvee : ' + t[:71])
            if b:
                print('    texte  ' + ''.join('.' if ch == ' ' else '#' for ch in t[:len(b[2])]))
                print('    encre  ' + ''.join('#' if e > 12 else '.' for e in b[2]) + '   (meilleure rangee, %d/%d)' % (b[0], len(b[2])))

if __name__ == '__main__':
    main()
