import json, sys
g = json.load(open('graphe.json'))
FIXE = 5
def frame(f): return g[f]['locals'] + g[f]['params_size'] + FIXE
target = sys.argv[2]; start = sys.argv[1]
best = {}
import functools
sys.setrecursionlimit(100000)
memo = {}
def longest_to(v, seen):
    # plus long chemin de v vers target sans repasser par seen
    if v == target: return (frame(v), [v])
    key = v
    if key in memo: return memo[key]
    res = None
    for w in g[v]['calls']:
        if w not in g or w in seen: continue
        r = longest_to(w, seen | {w})
        if r and (res is None or r[0] + frame(v) > res[0]): res = (r[0] + frame(v), [v] + r[1])
    memo[key] = res
    return res
r = longest_to(start, {start})
if not r: print('aucun chemin'); sys.exit()
acc = 0
for f in r[1]:
    acc += frame(f); print(f'  {frame(f):5d} {acc:5d}  {f.split("::")[-1]}  ({g[f]["file"]}:{g[f]["line"]})')
