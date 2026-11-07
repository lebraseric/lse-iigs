# Attribue les appels d'outils (lignes « Nom(...) » de --trace-tools) à la
# fonction nommée la plus interne et à la chaîne d'appel, sous RACINE.
import sys, re, collections
racine = sys.argv[1]
ligne_re = re.compile(r'^([0-9a-f]{2})/([0-9a-f]{4}) B:')
instr_re = re.compile(r'P: [0-9a-f]{2}\s+[-a-zA-Z]{8}\s+(?:[0-9a-f]{2} ){1,4}\s*([a-z]{3})')
pile = []
par_fn = collections.Counter()
nb_racine = 0
for l in sys.stdin:
    if not ligne_re.match(l):
        t = l.strip()
        if not t: continue
        if '(' in t:
            if racine in pile:
                outil = t.split('(')[0]
                nommes = [x for x in pile if not x.startswith('lib@')]
                par_fn[(nommes[-1] if nommes else '?', outil)] += 1
        elif pile and pile[-1].startswith('lib@'):
            pile[-1] = t
            if t == racine: nb_racine += 1
        continue
    mm = instr_re.search(l)
    if not mm: continue
    mn = mm.group(1)
    if mn == 'jsl':
        cible = l[mm.end():].split()[0]
        if cible.startswith('e1') or cible.startswith('e0'): continue
        pile.append('lib@' + cible)
    elif mn == 'rtl':
        if pile: pile.pop()
print('appels de', racine, ':', nb_racine)
for (f, o), v in sorted(par_fn.items(), key=lambda x: -x[1]):
    print('%-28s %-14s %6d  (%.2f par appel)' % (f, o, v, v / max(nb_racine, 1)))
