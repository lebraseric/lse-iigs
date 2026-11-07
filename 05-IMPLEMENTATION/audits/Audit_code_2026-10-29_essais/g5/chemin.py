import json, sys
sys.setrecursionlimit(10000)
g = json.load(open('graphe.json'))
FIXE = 5   # JSL (3) + PHD (2)
def frame(f): return g[f]['locals'] + g[f]['params_size'] + FIXE
# Tarjan
idx = {}; low = {}; st = []; on = set(); sccs = []; c = [0]
def tarjan(v):
    idx[v] = low[v] = c[0]; c[0] += 1; st.append(v); on.add(v)
    for w in g[v]['calls']:
        if w not in g: continue
        if w not in idx: tarjan(w); low[v] = min(low[v], low[w])
        elif w in on: low[v] = min(low[v], idx[w])
    if low[v] == idx[v]:
        comp = []
        while True:
            w = st.pop(); on.discard(w); comp.append(w)
            if w == v: break
        sccs.append(comp)
for v in g:
    if v not in idx: tarjan(v)
comp_of = {}
for i, comp in enumerate(sccs):
    for v in comp: comp_of[v] = i
rec = [comp for comp in sccs if len(comp) > 1 or comp[0] in g[comp[0]]['calls']]
print('Composantes récursives :')
for comp in rec:
    print('  ', sorted(comp), 'somme des cadres', sum(frame(f) for f in comp), 'cadre max', max(frame(f) for f in comp))
# plus long chemin, un passage par composante (récursion comptée une fois)
memo = {}
def longest(v, seen):
    if v in memo: return memo[v]
    best = (0, [])
    for w in g[v]['calls']:
        if w not in g or w in seen: continue
        r = longest(w, seen | {w})
        if r[0] > best[0]: best = r
    res = (frame(v) + best[0], [v] + best[1])
    memo[v] = res
    return res
start = sys.argv[1] if len(sys.argv) > 1 else 'main'
tot, path = longest(start, {start})
print('Chemin le plus profond depuis', start, ':', tot, 'octets')
acc = 0
for f in path:
    acc += frame(f)
    print(f'  {frame(f):5d} {acc:5d}  {f}  ({g[f]["file"]}:{g[f]["line"]}, locales {g[f]["locals"]}, param {g[f]["params_size"]})')
