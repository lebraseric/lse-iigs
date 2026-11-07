"""Simulateur de réels binaires à p bits, arithmétique exacte (Fraction)."""
from fractions import Fraction as Fr
import itertools, math

def arrondir(x, p, mode):
    if x == 0: return Fr(0)
    s = -1 if x < 0 else 1
    a = abs(x)
    e = math.floor(math.log2(a.numerator) - math.log2(a.denominator))
    # corrige e : 2^e <= a < 2^(e+1)
    while Fr(2)**e > a: e -= 1
    while Fr(2)**(e+1) <= a: e += 1
    u = Fr(2)**(e - (p - 1))          # ulp
    q = a / u
    fl = q.numerator // q.denominator
    r = q - fl
    if mode == 'RZ': n = fl
    elif mode == 'RN':
        n = fl + (1 if (r > Fr(1,2) or (r == Fr(1,2) and fl % 2 == 1)) else 0)
    elif mode == 'RNA': n = fl + (1 if r >= Fr(1,2) else 0)
    elif mode == 'RU': n = fl + (1 if (r > 0 and s > 0) else 0)
    elif mode == 'RD': n = fl + (1 if (r > 0 and s < 0) else 0)
    else: raise ValueError(mode)
    return s * n * u

class Machine:
    def __init__(s, p, op, conv, convmode, imp, pneg='inv'):
        s.p, s.op, s.conv, s.convmode, s.imp, s.pneg = p, op, conv, convmode, imp, pneg
    def r(s, x): return arrondir(x, s.p, s.op)
    def add(s, a, b): return s.r(a + b)
    def sub(s, a, b): return s.r(a - b)
    def mul(s, a, b): return s.r(a * b)
    def div(s, a, b): return s.r(a / b)
    def puiss(s, x, n):                 # entier : multiplications successives, carrés, ou un seul arrondi
        if n == 0: return Fr(1)
        if s.pneg == 'exacte': return s.r(x ** n)   # pow() en double, arrondi une fois (LSE-IIGS)
        if s.pneg == 'carres':
            y, b, m = Fr(1), x, abs(n)
            prem = True
            while m:
                if m & 1: y = b if prem else s.mul(y, b); prem = False
                m >>= 1
                if m: b = s.mul(b, b)
        else:
            y = x
            for _ in range(abs(n) - 1): y = s.mul(y, x)
        return y if n > 0 else s.div(Fr(1), y)
    def c(s, texte):                     # constante décimale
        t = texte.upper()
        neg = t.startswith('-')
        if neg: t = t[1:]
        mant, _, ex = t.partition('E')
        ex = int(ex) if ex else 0
        ent, _, dec = mant.partition('.')
        if s.conv == 'exacte':
            v = arrondir(Fr(ent + dec) * Fr(10) ** (ex - len(dec)), s.p, s.convmode)
        else:                            # chiffres accumulés, puis division par 10^k
            m = arrondir(Fr(int(ent + dec)), s.p, s.convmode)
            k = len(dec) - ex
            if k > 0: v = arrondir(m / arrondir(Fr(10) ** k, s.p, s.convmode), s.p, s.convmode)
            elif k < 0: v = arrondir(m * Fr(10) ** (-k), s.p, s.convmode)
            else: v = m
        return -v if neg else v
    def u(s, x):                         # spécification U : 7 chiffres significatifs
        if x == 0: return '0'
        signe = '-' if x < 0 else ''
        a = abs(x)
        e = math.floor(math.log10(a.numerator) - math.log10(a.denominator))
        while Fr(10)**e > a: e -= 1
        while Fr(10)**(e+1) <= a: e += 1
        k = 6 - e
        if s.imp == 'ECH':               # mise à l'échelle dans les réels de la machine, puis troncature
            q = s.r(a * Fr(10)**k) if k >= 0 else s.r(a / Fr(10)**(-k))
        elif s.imp == 'ECH10':           # multiplications ou divisions successives par 10
            q = a
            for _ in range(abs(k)): q = s.r(q * 10) if k > 0 else s.r(q / 10)
        elif s.imp == 'DIG':             # ramené dans [1,10[, puis chiffre par chiffre
            y = a
            for _ in range(abs(e)): y = s.r(y / 10) if e > 0 else s.r(y * 10)
            n = 0
            for _ in range(7):
                d = y.numerator // y.denominator
                if d > 9: d = 9
                n = n * 10 + d
                y = s.r((y - d) * 10)
            q = Fr(n)
        else:
            q = a / Fr(10)**(e - 6)
        fl = q.numerator // q.denominator
        r = q - fl
        n = fl + (1 if (s.imp == 'RN' and r >= Fr(1,2)) else 0)
        if n >= 10**7: n //= 10; e += 1
        ch = str(n)
        if Fr(1,1000) <= a < 10**6:
            if e >= 6: txt = ch + '0' * (e - 6)
            elif e >= 0: txt = ch[:e+1] + '.' + ch[e+1:]
            else: txt = '0.' + '0' * (-e - 1) + ch
            if '.' in txt: txt = txt.rstrip('0').rstrip('.')
            return signe + txt
        m = ch[0] + '.' + ch[1:]
        m = m.rstrip('0').rstrip('.')
        return signe + m + 'E' + ('+' if e >= 0 else '-') + '%02d' % abs(e)
