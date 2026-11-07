# Profileur de trace Golden Gate (--trace-cpu --trace-debug-names).
# Compte les instructions exécutées pendant que la fonction RACINE est
# sur la pile d'appels : exclusif (fonction au sommet) et inclusif.
import sys, re, collections
racine = sys.argv[1] if len(sys.argv) > 1 else 'compilateur_compiler'
pile = []          # libellés
excl = collections.Counter()
incl = collections.Counter()
appels = collections.Counter()
total = 0
attente_nom = False
ligne_re = re.compile(r'^([0-9a-f]{2})/([0-9a-f]{4}) B:')
instr_re = re.compile(r'P: [0-9a-f]{2}\s+[-a-zA-Z]{8}\s+(?:[0-9a-f]{2} ){1,4}\s*([a-z]{3})')
for l in sys.stdin:
    m = ligne_re.match(l)
    if not m:
        nom = l.strip()
        if nom and pile and pile[-1].startswith('lib@'):
            pile[-1] = nom
            if racine in pile:
                appels[nom] += 1
        attente_nom = False
        continue
    attente_nom = False
    # mnémonique : après la colonne des octets
    mm = instr_re.search(l)
    if not mm: continue
    mn = mm.group(1)
    parties = [mn] + l[mm.end():].split()
    actif = racine in pile
    if actif:
        total += 1
        top = pile[-1] if pile else '?'
        excl[top] += 1
        for lab in set(pile):
            incl[lab] += 1
    if mn == 'jsl':
        cible = parties[1]
        if cible.startswith('e1') or cible.startswith('e0'):
            if actif: appels['TOOL@' + cible] += 1
            continue
        pile.append('lib@' + cible)
        attente_nom = True
    elif mn == 'rtl':
        if pile: pile.pop()
print('instructions comptées sous', racine, ':', total)
print('--- exclusif (top 35)')
for k, v in excl.most_common(35):
    print('%-32s %9d %5.1f%%  appels %d' % (k, v, 100.0 * v / max(total, 1), appels.get(k, 0)))
print('--- inclusif (top 45)')
for k, v in incl.most_common(45):
    print('%-32s %9d %5.1f%%  appels %d' % (k, v, 100.0 * v / max(total, 1), appels.get(k, 0)))
print('--- appels outils')
for k, v in appels.items():
    if k.startswith('TOOL'): print(k, v)
