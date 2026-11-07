#!/usr/bin/env python3
# verifier.py - rejoue la verification de PERFO.lse et CHARG.lse sous Golden Gate.
# Emploi : python3 -I verifier.py TRAVAIL LSE [RUBANS]
#   TRAVAIL : repertoire de travail, vide ou absent
#   LSE     : build/LSE
#   RUBANS  : repertoire ou ecrire les rubans perfores par PERFO (PRONO, FICH,
#             FICH2), seulement si toute la verification a reussi ; sinon, sortie
#             en erreur et aucun ruban
# Le ruban est le fichier RUBAN de la bibliotheque, que LSE perfore lui-meme
# (IMP-0274). Sous le repli stdio, NUL et BEL arrivent a stdout : ils sont retires
# avant la comparaison, comme la teletype ne les imprimait pas.
import os, shutil, subprocess, sys, difflib

ICI = os.path.dirname(os.path.abspath(__file__))
HAUT = os.path.dirname(ICI)
X = b'\x13'
XON = b'\x11'
F = b'\x13\0\0'
F4 = ['-F4']
MITRA = ['-MITRA15', '-ID', '1']

def lancer(rep, entree, args):
    if '-MITRA15' not in args:
        entree = b'\n' + entree            # LSE F4 demande la date : RETURN (IMP-0279)
    p = subprocess.run(['iix', './LSE'] + args, cwd=rep, input=entree,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=180)
    return p.stdout

def imprime(flux):
    if any(0x11 <= c <= 0x15 for c in flux):
        sys.exit('ordre a la teletype arrive a la console (IMP-0275)')
    return bytes(c for c in flux if c not in (0, 7))

def lire_perfore(rep):
    chemin = os.path.join(rep, 'BIB', 'RUBAN')
    return open(chemin, 'rb').read() if os.path.exists(chemin) else b''

def installer(rep, prof, noms=('CHARG', 'PERFO')):
    bib = os.path.join(rep, 'BIB')
    shutil.rmtree(bib, ignore_errors=True)
    os.mkdir(bib)
    e = b'BO' + X
    for n in noms:
        e += open(os.path.join(HAUT, n + '.lse'), 'rb').read()
        e += b'RA' + X + n.encode() + X + b'EF' + X + b'*' + X
    o = lancer(rep, e + b'AU' + X, prof + ['-BIB', 'BIB'])
    if b'ERREUR' in o: sys.exit('compilation : ' + o.decode('latin1'))

def lire_sortie(nomf):
    L = open(os.path.join(HAUT, 'sorties', nomf)).read().split('\n')
    return L[:-1] if L[-1] == '' else L

def enregistrements(L, nomf):
    """Texte de chaque enregistrement : les lignes imprimees, jointes par l'espace
    que la coupure a laisse en fin de ligne, sauf apres / et ' (coupure sans espace)
    et sauf PRONO 8, dont LGR:73 (numerisation0001) prouve l'absence d'espace."""
    i = [k for k, l in enumerate(L) if l.startswith('ENREGISTREMENT ')]
    fin = [k for k, l in enumerate(L) if l.startswith('& FIN')][0]
    res = []
    for num, (a, b) in enumerate(zip(i, i[1:] + [fin]), 1):
        t = [l for l in L[a + 1:a + 5] if l != '']
        s = ''
        for k, l in enumerate(t):
            s += l
            if k < len(t) - 1 and l[-1] not in "/'" and not (nomf == '0042.txt' and num == 8):
                s += ' '
        res.append(s)
    return res

def ruban_main(nom, Z):
    """Ruban d'entree coupe tous les 72 caracteres, sans la regle de PERFO."""
    r = b'AP' + F + b'CHARG' + F + b'EX' + F + b'1' + F + nom.encode() + F
    for i, z in enumerate(Z, 1):
        r += ('ENREGISTREMENT %d 3 0 ' % i).encode() + F
        for t in [z[0:72], z[72:144], z[144:216], z[216:]]:
            r += t.encode() + F
    return r + b'& FIN DE FICHIER' + F + b'CA' + F + ('%s,%s' % (nom, nom)).encode() + F

def lire_ruban(rep, ruban, prof):
    open(os.path.join(rep, 'BIB', 'RUBAN'), 'wb').write(ruban)
    marche = b'RU' + X if prof is MITRA else XON
    return lancer(rep, b'BO' + X + marche + b'AU' + X, prof + ['-BIB', 'BIB'])

def perfo(rep, nom, reponse, bornes, prof, suite=b''):
    """Lance PERFO sur un ruban vierge ; rend (imprime, ruban)."""
    chemin = os.path.join(rep, 'BIB', 'RUBAN')
    if os.path.exists(chemin): os.remove(chemin)
    e = b'BO' + X + b'AP' + X + b'PERFO' + X + b'EX' + X + b'1' + X
    e += nom.encode() + X + reponse + X + bornes.encode() + X + suite + b'AU' + X
    return imprime(lancer(rep, e, prof + ['-BIB', 'BIB'])), lire_perfore(rep)

def attendu(L, nom):
    a = []
    for l in L:
        if l == 'EXECUTER A PARTIR DE 1': l = 'ECUTER A PARTIR DE '
        elif l == 'NOM : ' + nom: l = 'NOM : '
        elif l.startswith('PERFORATION ?'): l = 'PERFORATION ? (OUI/NON) '
        elif l.startswith('PREMIER ET DERNIER'): l = 'PREMIER ET DERNIER ENREGISTREMENT : '
        a.append(l.rstrip())
    return a

def sortie(rep, nomf, nom, bornes, prof=F4):
    L = lire_sortie(nomf)
    Z = enregistrements(L, nomf)
    installer(rep, prof)
    o = lire_ruban(rep, ruban_main(nom, Z), prof)
    if b'ERREUR' in o:
        return 'CHARG : ' + [l for l in o.decode('latin1').split('\n') if 'ERREUR' in l][0], None
    chemin = os.path.join(rep, 'BIB', 'DONNEES', nom)
    avant = open(chemin, 'rb').read()
    imp, ruban = perfo(rep, nom, b'OUI', bornes, prof)
    s = imp.decode('latin1').replace('\r', '').split('\n')
    i = [k for k, l in enumerate(s) if l.startswith('ECUTER')][0]
    j = [k for k, l in enumerate(s) if l.startswith('TERMINE')][0]
    obt = [l.rstrip() for l in s[i:j + 1]]
    att = attendu(L, nom) if prof is F4 else attendu(L, nom)[:-1] + ['TERMINE']
    if obt != att:
        return 'sortie differente : ' + ' | '.join(list(difflib.unified_diff(att, obt, lineterm='', n=0))[:6]), ruban
    installer(rep, prof, ('CHARG',))
    o = lire_ruban(rep, ruban, prof)
    if b'ERREUR' in o or open(chemin, 'rb').read() != avant:
        return 'aller-retour different', ruban
    return ('%d lignes identiques, %d enregistrements, ruban de %d octets (%d CR/LF), fichier relu '
            'identique a l\'octet' % (len(obt), len(Z), len(ruban), ruban.count(b'\r') + ruban.count(b'\n'))), ruban

def cas_non_attestes(rep):
    r = []
    Z = enregistrements(lire_sortie('0042.txt'), '0042.txt')
    installer(rep, F4)
    lire_ruban(rep, ruban_main('PRONO', Z), F4)
    oui, _ = perfo(rep, 'PRONO', b'OUI', '1 9', F4)
    imp, rub = perfo(rep, 'PRONO', b'NON', '1 9', F4)
    corps = lambda b: [l.rstrip() for l in b.split(b'\n')[2:-2]]
    r.append(('NON : meme imprime, rien de perfore', corps(oui) == corps(imp) and rub == b''))
    imp, rub = perfo(rep, 'NEANT', b'OUI', '1 3', F4)
    r.append(('fichier absent : FICHIER INEXISTANT, rien de perfore', b'FICHIER INEXISTANT' in imp and rub == b''))
    imp, rub = perfo(rep, 'PRONO', b'OUI', '8 11', F4)
    r.append(('bornes 8 11 : enregistrements 10 et 11 sautes', imp.count(b'ENREGISTREMENT ') == 3 and b'TERMINE EN LIGNE 222' in imp))
    P = (b"1 V_-2.5\n2 TABLEAU T[3];T[1]_1.5;T[2]_-7;T[3]_1E+10\n"
         b"3 TABLEAU M[2,3];M[1,1]_1;M[1,2]_2;M[1,3]_3;M[2,1]_4;M[2,2]_5;M[2,3]_6\n"
         b"4 CHAINE C;C_'ABC DEF'\n5 GARER V,1,'MIX';GARER T,2,'MIX';GARER M,3,'MIX';GARER C,4,'MIX'\n6 TERMINER\n")
    lancer(rep, b'BO' + X + P + b'EX' + X + b'1' + X + b'CA' + X + b'MIX,MIX' + X + b'EF' + X + b'*' + X + b'AU' + X, F4 + ['-BIB', 'BIB'])
    chemin = os.path.join(rep, 'BIB', 'DONNEES', 'MIX')
    avant = open(chemin, 'rb').read()
    imp, rub = perfo(rep, 'MIX', b'OUI', '1 4', F4, b'3' + X + b'2 3' + X)
    installer(rep, F4, ('CHARG',))
    o = lire_ruban(rep, rub, F4)
    r.append(('variable, vecteur, matrice, chaine : fichier relu identique', b'ERREUR' not in o and open(chemin, 'rb').read() == avant))
    longs = ['A' * 100, 'A' * 72, 'A' * 73, '', 'X' * 71 + ' ' + 'Y' * 10, ('B' * 40 + ' ') * 6]
    installer(rep, F4)
    P = (b"1 CHAINE C,L;I_1\n2 LIRE N\n3 FAIRE 7 POUR I_1 JUSQUA N\n4 C_''\n5 LIRE L\n"
         b"6 SI L#'*' ALORS DEBUT C_C!L;ALLER EN 5 FIN\n7 GARER C,I,'LIM'\n8 TERMINER\n")
    e = b'BO' + X + P + b'EX' + X + b'1' + X + str(len(longs)).encode() + X
    for z in longs:
        for k in range(0, len(z), 100): e += z[k:k + 100].encode() + X
        e += b'*' + X
    lancer(rep, e + b'CA' + X + b'LIM,LIM' + X + b'EF' + X + b'*' + X + b'AU' + X, F4 + ['-BIB', 'BIB'])
    chemin = os.path.join(rep, 'BIB', 'DONNEES', 'LIM')
    avant = open(chemin, 'rb').read()
    imp, rub = perfo(rep, 'LIM', b'OUI', '1 6', F4)
    installer(rep, F4, ('CHARG',))
    o = lire_ruban(rep, rub, F4)
    r.append(('mots de plus de 72 lettres, chaine vide, 4e ligne trop longue : relu identique', b'ERREUR' not in o and open(chemin, 'rb').read() == avant))
    return r

def main():
    travail, exe = sys.argv[1:3]
    rep = os.path.join(travail, 'essai')
    shutil.rmtree(rep, ignore_errors=True)
    os.makedirs(rep)
    subprocess.check_call(['ditto', exe, os.path.join(rep, 'LSE')])
    print('F4 :')
    rubans = {}
    bon = True
    for nomf, nom, bornes in (('0042.txt', 'PRONO', '1 9'), ('0012.txt', 'FICH', '1 8'), ('0029.txt', 'FICH', '1 4'), ('0011.txt', 'FICH2', '1 10')):
        res, rub = sortie(rep, nomf, nom, bornes)
        rubans[nomf] = rub
        bon = bon and res[0].isdigit()
        print('  %s %-6s %s' % (nomf, nom, res))
    print('MITRA 15 :')
    res, rub = sortie(rep, '0042.txt', 'PRONO', '1 9', MITRA)
    meme = rub == rubans['0042.txt']
    bon = bon and res[0].isdigit() and meme
    print('  0042.txt PRONO  %s ; meme ruban que sous F4 : %s' % (res, 'oui' if meme else 'NON'))
    installer(rep, MITRA, ('CHARG',))
    o = lire_ruban(rep, rubans['0042.txt'], MITRA)
    lu = b'ERREUR' not in o and os.path.exists(os.path.join(rep, 'BIB', 'DONNEES', 'PRONO'))
    bon = bon and lu
    print('  ruban perfore sous F4, lu sous MITRA 15 :', 'ok' if lu else 'ECHEC')
    print('Cas non attestes :')
    for t, ok in cas_non_attestes(rep):
        bon = bon and ok
        print('  %-3s %s' % ('ok' if ok else 'NON', t))
    if not bon:
        sys.exit('verification en echec : aucun ruban ecrit')
    if len(sys.argv) > 3:
        os.makedirs(sys.argv[3], exist_ok=True)
        for nomf, nom in (('0042.txt', 'PRONO'), ('0012.txt', 'FICH'), ('0011.txt', 'FICH2')):
            open(os.path.join(sys.argv[3], nom), 'wb').write(rubans[nomf])

main()
