#!/usr/bin/env python3
# verifier.py - rejoue sous Golden Gate les executions imprimees a Victor Duruy
# et les compare, ligne a ligne, a leur transcription (../executions/).
# Emploi : python3 -I verifier.py TRAVAIL LSE
#   TRAVAIL : repertoire de travail, vide ou absent
#   LSE     : build/LSE
# Chaque programme (../NOM.lse) est tape apres BONJOUR sous LSE F4, puis la
# frappe de l'eleve, notee {...} dans la transcription, est fournie dans l'ordre,
# chaque groupe termine par X-OFF. Sous le repli stdio, LSE ne renvoie pas
# l'echo de la frappe : elle est retiree de la ligne attendue. NUL et BEL sont
# retires de la sortie, comme la teletype ne les imprimait pas. Une ligne ~
# (changement de feuille) admet un nombre quelconque de lignes blanches.
# Bibliotheque propre a l'essai : TRAVAIL/BIB (-BIB), creee par LSE.
# Les ecarts connus sont consignes dans ECARTS ; tout autre ecart fait echouer.
import os, re, shutil, subprocess, sys

ICI = os.path.dirname(os.path.abspath(__file__))
HAUT = os.path.dirname(ICI)
X = b'\x13'

SESSIONS = [            # (programme, execution transcrite, feuille)
    ('NBOR',  'NBOR_0024.txt',  '5EME - NBOR, numerisation0024'),
    ('PUISS', 'PUISS_0006.txt', '5EME - PUISS, numerisation0006'),
    ('MULT',  'MULT_0005.txt',  '5EME - MULT, numerisation0005 (version anterieure a 0015)'),
    ('SPHIN', 'SPHIN_0040.txt', '6EME - SPHIN, numerisation0040'),
    ('ANGLO', 'ANGLO_0039.txt', '5EME - ANGLO, ANGLO_V9_EXEC_numerisation0039'),
    ('CARTO', 'CARTO_0037.txt', '5EME - CARTO, numerisation0037'),
    ('CARTOV1', 'CARTOV1_0009.txt', '5EME - CARTO, CARTO_V1_numerisation0009'),
]

# (execution, ligne imprimee) -> (ligne de LSE-IIGS, raison)
ECARTS = {
    ('NBOR_0024.txt', '3.645752'): ('3.645751', 'RAC du T1600 : un dernier bit de plus (Interpreteur IMP-0280)'),
}

def lancer(rep, entree):
    entree = b'\n' + entree                # LSE F4 demande la date : RETURN (IMP-0279)
    p = subprocess.run(['iix', './LSE', '-BIB', 'BIB'], cwd=rep, input=entree,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=300)
    return bytes(c for c in p.stdout if c not in (0, 7)).decode('latin1')

def lire_execution(nomf):
    attendu, frappe = [], []
    for l in open(os.path.join(HAUT, 'executions', nomf), encoding='utf-8').read().split('\n')[:-1]:
        frappe += re.findall(r'\{([^}]*)\}', l)
        attendu.append(re.sub(r'\{[^}]*\}', '', l).rstrip())
    return attendu, frappe

def aligner(att, obt):
    """Paires (attendu, obtenu) ; ~ absorbe les lignes blanches obtenues."""
    paires, j = [], 0
    for a in att:
        if a == '~':
            while j < len(obt) and obt[j] == '':
                j += 1
            continue
        paires.append((a, obt[j] if j < len(obt) else None))
        j += 1
    return paires, obt[j:]

def session(rep, prog, nomf):
    att, frappe = lire_execution(nomf)
    texte = open(os.path.join(HAUT, prog + '.lse'), 'rb').read().split(b'\n')
    e = b'BO' + X + b''.join(l + X for l in texte if l.strip())
    e += b''.join(f.encode('latin1') + X for f in frappe) + b'AU' + X
    s = [l.rstrip() for l in lancer(rep, e).replace('\r', '').split('\n')]
    if att[0] not in s:
        return ['debut introuvable : ' + att[0]], 0, []
    i = s.index(att[0])
    refus = [l for l in s[:i] if 'ERREUR' in l]
    if refus:
        return ['programme refuse : ' + refus[0]], 0, []
    j = next((k for k in range(i, len(s)) if s[k].startswith('TERMINE')), len(s) - 1)
    paires, reste = aligner(att, s[i:j + 1])
    ecarts, consignes, egales = [], [], 0
    for n, (a, o) in enumerate(paires):
        if a == o:
            egales += 1
        elif (nomf, a) in ECARTS and ECARTS[(nomf, a)][0] == o:
            consignes.append('%r au lieu de %r (%s)' % (o, a, ECARTS[(nomf, a)][1]))
        else:
            ecarts.append('ligne %d : attendu %r, obtenu %r' % (n + 1, a, o))
    if reste:
        ecarts.append('%d ligne(s) en trop : %r' % (len(reste), reste[:3]))
    return ecarts, egales, consignes

def main():
    travail, exe = sys.argv[1:3]
    shutil.rmtree(travail, ignore_errors=True)
    os.makedirs(travail)
    subprocess.check_call(['ditto', exe, os.path.join(travail, 'LSE')])
    echec = 0
    for prog, nomf, feuille in SESSIONS:
        ecarts, egales, consignes = session(travail, prog, nomf)
        etat = 'ok ' if not ecarts else 'ECHEC'
        echec += bool(ecarts)
        print('%s %-15s %3d lignes identiques, %d ecart(s) consigne(s) ; %s'
              % (etat, nomf, egales, len(consignes), feuille))
        for c in consignes:
            print('       consigne : ' + c)
        for c in ecarts[:12]:
            print('       ' + c)
    sys.exit(1 if echec else 0)

if __name__ == '__main__':
    main()
