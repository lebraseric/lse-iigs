import sys
res=[]
for f in sys.argv[1:]:
    for l in open(f, encoding='latin-1'):
        l=l.rstrip('\n')
        d=0; m=0; inq=False
        for c in l:
            if c=="'": inq = not inq; continue
            if inq: continue
            if c in '([': d+=1; m=max(m,d)
            elif c in ')]': d-=1
        aff = ('AFFICHER' in l) or l.startswith('?')
        res.append((m, aff, f.split('/')[-1], l))
res.sort(key=lambda x:-x[0])
for r in res[:15]: print(r)
import collections
print(collections.Counter(r[0] for r in res))
