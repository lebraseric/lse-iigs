# Analyse approximative de la pile : locales par fonction, graphe d'appel.
import re, sys, os, json
src = sys.argv[1]
files = []
for d in ('noyau', 'plateforme'):
    for f in sorted(os.listdir(os.path.join(src, d))):
        if f.endswith('.c'):
            files.append(os.path.join(src, d, f))

def strip(text):
    # enlève commentaires, chaînes, caractères
    out = []; i = 0; n = len(text)
    while i < n:
        c = text[i]
        if text.startswith('/*', i):
            j = text.find('*/', i + 2); seg = text[i:j+2]
            out.append(''.join('\n' if ch == '\n' else ' ' for ch in seg)); i = j + 2
        elif text.startswith('//', i):
            j = text.find('\n', i); out.append(' ' * (j - i)); i = j
        elif c == '"' or c == "'":
            q = c; j = i + 1
            while text[j] != q:
                if text[j] == '\\': j += 1
                j += 1
            out.append(q + 'x' * 0 + q); out.append(' ' * (j - i - 1)); i = j + 1
        else:
            out.append(c); i += 1
    return ''.join(out)

# tailles de types (ORCA/C)
base = {'char':1,'unsigned char':1,'signed char':1,'int':2,'unsigned int':2,'unsigned':2,'short':2,
        'unsigned short':2,'long':4,'unsigned long':4,'float':4,'double':8,'Word':2,'Long':4,
        'EspaceFichier':1,'ModeFichier':1,'HandleObjet':2,'ProfilImplantation':1,'NatureErreur':2,
        'TagRCIM':1,'TypeStatique':1,'FinExecution':1,'Commande':1,'CategorieEntree':1,
        'ResultatDecompilation':1,'TypeElementPile':1,'EtatIdentificateur':1,'UsageLecture':1,
        'ModeParametre':1,'NatureSymbole':1,'OrigineSymbole':1,'TypeLigneCompilee':1,
        'time_t':4,'FichierPlateforme':4,'HandlePlateforme':4,'Handle':4,'Pointer':4,'FILE':0,
        'InfoExpression':11,'Analyse':4+2+2+4+2+1+4,'TamponRCIM':8,'SpecFormat':11,
        'DiagnosticCompilation':3,'ResultatCompilation':1+4+2,'EntreeObjetCompile':5,
        'EntreeSymbole':10,'EntreeConstante':3,'InfoProcedure':2,'Identification':5,
        'InstantHorloge':7,'Depart':9,'Entree_fichiers':13,'Place':0,'BitsNombre':4,
        'ChaineSurPile':6,'ReferenceResolue':5,'ElementPile':7,'EntreeEnvironnement':5,
        'PileEvaluation':8,'PositionExecution':0,'Cycle':4+4+255+2+1+2+2+2+1+2+4,
        'Decompilation':4+2+128*12+2+1+1,'Entree_decomp':12,'EntreeSlot':4,
        'CurseurSequence':8,'TableParametresFormels':0,'ParametreFormel':3,
        'DIORecGS':22,'DAccessRecGS':18,'DevNumRecGS':2+4+2,
        'OpenRecGS':2+2+4+2+2+2+2+4+2+8+8+4+4+4+4+4,'RefNumRecGS':4,'CreateRecGS':2+4+2+2+4+2+4+4,
        'DirEntryRecGS':2+2+2+2+2+4+2+2+4+4+8+8+2+4+2+4+4+4,'ResultBuf255':2+2+255,
        'ResultBuf32':2+2+32,'GSString255':2+255,'GSString32':2+32,'ExpandPathRecGS':2+4+4+2,
        'DInfoRecGS':2+2+4+2+4+2+2+2+2+2+2+4,'VolumeRecGS':2+4+4+4+4+2+2+2+2,'struct tm':18,
        'SauvegardeLocale':4,'AliasReference':0,'CadreActivation':0,'ContexteBoucle':0,
        'EntreeCommande':0,'EnTeteBlocMaitre':0,'Fonction':0,'EntreeCatalogue':0,'Entree':12,
        }
ptr = 4
funcs = {}
order = []
funcs_list = []
for path in files:
    raw = open(path, encoding='utf-8', errors='replace').read()
    t = strip(raw)
    lines_off = [0]
    for m in re.finditer('\n', t): lines_off.append(m.end())
    def lineno(pos):
        import bisect
        return bisect.bisect_right(lines_off, pos)
    # repérer les définitions de fonctions au niveau 0
    depth = 0; i = 0; n = len(t)
    while i < n:
        c = t[i]
        if c == '{':
            if depth == 0:
                # en-tête : texte depuis la fin du précédent ; ou }
                head_start = max(t.rfind(';', 0, i), t.rfind('}', 0, i)) + 1
                head = t[head_start:i]
                m = re.search(r'([A-Za-z_]\w*)\s*\(([^()]*(\([^()]*\)[^()]*)*)\)\s*$', head, re.S)
                if m and '=' not in head and 'typedef' not in head and 'struct' not in head.split(m.group(1))[0][-10:]:
                    name = m.group(1); params = m.group(2)
                    # trouver fin
                    d = 1; j = i + 1
                    while d:
                        if t[j] == '{': d += 1
                        elif t[j] == '}': d -= 1
                        j += 1
                    body = t[i:j]
                    key = name
                    funcs_list.append((os.path.relpath(path, src), name, head.strip().startswith('static')))
                    funcs[(os.path.relpath(path, src), name)] = dict(file=os.path.relpath(path, src), line=lineno(i), params=params, body=body,
                                       static=head.strip().startswith('static'))
                    order.append((os.path.relpath(path, src), name))
                    i = j; continue
            depth += 1
        elif c == '}': depth -= 1
        i += 1

typenames = sorted(base.keys(), key=len, reverse=True)
decl_re = re.compile(r'(?:^|[;{}])\s*((?:static\s+|const\s+|volatile\s+|register\s+)*)(' + '|'.join(re.escape(x) for x in typenames) + r')\b([^;(){}]*(?:\([^;{}]*\))?[^;{}]*);', re.M)

def size_of_decl(tname, declarators, fname):
    total = 0; items = []
    # découpe au niveau 0 des virgules
    parts = []; d = 0; cur = ''
    for ch in declarators:
        if ch in '([{': d += 1
        if ch in ')]}': d -= 1
        if ch == ',' and d == 0: parts.append(cur); cur = ''
        else: cur += ch
    parts.append(cur)
    for p in parts:
        p = p.split('=')[0]
        if not p.strip(): continue
        isptr = '*' in p.split('[')[0]
        m = re.match(r'\s*\**\s*([A-Za-z_]\w*)\s*((\[[^\]]*\])*)', p)
        if not m: continue
        var = m.group(1)
        dims = re.findall(r'\[([^\]]*)\]', m.group(2))
        s = ptr if isptr else base.get(tname, 0)
        if tname == 'Entree':
            s = ptr if isptr else (13 if fname.startswith('noyau/fichiers') else 12)
        mult = 1
        for dm in dims:
            mult *= evaldim(dm)
        if isptr and dims: s = ptr
        items.append((var, s * mult)); total += s * mult
    return total, items

consts = {'COMPILATEUR_LONGUEUR_MAX':79,'DECOMPILATEUR_LONGUEUR_MAX':255,'RCIM_LONGUEUR_MAX_SEQUENCE':255,
          'FMT_MAX':40,'DECL_MAX':32,'PILE_MAX':128,'GESTOBJ_NB_SLOTS_MAX_PAR_BLOC':64,'SECTEUR':256,
          'IDENT_TAILLE':6,'CATALOGUE_TAILLE':16,'LONGUEUR_CHEMIN':200,'sizeof saisie':0}
def evaldim(dm):
    e = dm.strip()
    if e == '': return 0
    for k, v in consts.items(): e = re.sub(r'\b' + k + r'\b', str(v), e)
    e = e.replace('u', '')
    try: return int(eval(e))
    except Exception: return -1

names = set(n for (_, n) in funcs)
statics = {}
globs = {}
for (fl, n), v in funcs.items():
    if v['static']: statics[(fl, n)] = v
    else: globs[n] = (fl, n)
def resolve(fl, n):
    if (fl, n) in statics: return fl + '::' + n
    if n in globs: return n
    if (fl, n) in funcs: return fl + '::' + n
    return None
out = {}
for key in order:
    f = funcs[key]
    fl, name = key
    body = f['body']
    loc = 0; items = []
    for m in decl_re.finditer(body):
        quals = m.group(1)
        if 'static' in quals: continue
        tname = m.group(2); decls = m.group(3)
        if re.match(r'\s*\(', decls): continue  # cast
        if tname in ('return',): continue
        s, it = size_of_decl(tname, decls, f['file'])
        loc += s; items += it
    f['locals'] = loc; f['items'] = items
    # paramètres
    ps = 0
    for p in f['params'].split(','):
        p = p.strip()
        if p in ('', 'void'): continue
        if '*' in p or '[' in p: ps += 4; continue
        tn = re.sub(r'\b(const|unsigned|signed|register)\b', lambda m: m.group(0), p).rsplit(None, 1)[0].replace('const ', '').strip()
        sz = base.get(tn, 2)
        if tn in ('float', 'double'): sz = 10
        if sz == 1: sz = 2
        ps += sz
    f['params_size'] = ps
    calls = set()
    for m in re.finditer(r'\b([A-Za-z_]\w*)\s*\(', body):
        c = m.group(1)
        if m.start() == 0: continue
        r = resolve(fl, c)
        if r: calls.add(r)
    f['calls'] = sorted(calls)
    out[resolve(fl, name)] = {kk: vv for kk, vv in f.items() if kk != 'body'}
json.dump(out, open(sys.argv[2], 'w'), indent=1, ensure_ascii=False)
print(len(funcs), 'fonctions')
