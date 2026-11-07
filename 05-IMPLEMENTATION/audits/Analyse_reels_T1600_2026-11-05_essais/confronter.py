"""Confronte des modèles de réels aux témoins d'INRDP75 et à la sortie de NBOR.

Usage : python3 -I confronter.py   (depuis ce répertoire ; aucun argument)
Modèle : modele.py (arithmétique exacte, Fraction) ; témoins : temoins.py.
Chaque modèle fixe la largeur de mantisse p, l'arrondi des opérations et des
constantes, l'impression de U et le calcul des puissances entières."""
import math, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from modele import Machine, Fr
from temoins import temoins

# NBOR (5ème, LSE F4, 1979-80), réponse 7 : valeurs imprimées par la ligne 116
NBOR = ['3.645752', '2.909385', '2.705692', '2.644899', '2.626315', '2.620591', '2.618824',
        '2.618278', '2.61811', '2.618058', '2.618042', '2.618036', '2.618035', '2.618034']

def racine(M, x):                        # racine exacte à 120 bits, puis arrondie
    s = math.isqrt(int(x * Fr(2) ** 240))
    return M.r(Fr(s, 2 ** 120))

def nbor(M, depart='7'):
    x = M.c(depart)
    vals = []
    for _ in range(60):                  # 110 NBRDE_RAC(NBRDE)+1 ; 116 AFFICHER NBRDE
        x = M.add(racine(M, x), Fr(1))
        vals.append(M.u(x))
        if x == M.add(racine(M, x), Fr(1)):   # 120 SI NBRDE#RAC(NBRDE)+1 ...
            break
    return vals

def score(M):
    t = temoins(M)
    ok_i = sum(1 for _, v, a in t if v == a)
    v = nbor(M)
    ok_n = sum(1 for a, b in zip(v, NBOR) if a == b)
    return ok_i, len(t), len(v), ok_n, t, v

if __name__ == '__main__':
    print('p  arrondi impression puissances | INRDP75 | NBOR : operations, valeurs')
    for p in range(20, 27):
        for mode in ['RN', 'RNA', 'RZ']:
            for imp in ['RN', 'RZ', 'ECH', 'ECH10', 'DIG']:
                for pneg in ['inv', 'carres', 'exacte']:
                    M = Machine(p, mode, 'exacte', mode, imp, pneg)
                    oi, ni, nn, on, _, _ = score(M)
                    print('%2d %-4s    %-4s       %-7s | %2d/%2d | %2d, %2d/14' % (p, mode, imp, pneg, oi, ni, nn, on))
    M = Machine(23, 'RNA', 'exacte', 'RNA', 'RN', 'exacte')
    oi, ni, nn, on, t, v = score(M)
    print('\nModele retenu (IMP-0280) : 23 bits, demie en s\'eloignant de zero, U arrondi a 7 chiffres')
    for nom, val, att in t:
        print('  %-24s %-14s %-14s %s' % (nom, val, att, 'ok' if val == att else 'ECART'))
    print('  NBOR :', nn, 'operations')
    for a, b in zip(v, NBOR):
        print('  %-10s %-10s %s' % (a, b, 'ok' if a == b else 'ECART'))
