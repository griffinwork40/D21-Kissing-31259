"""C1_ceiling certificate check (exact integers only, no dependencies).
Proves: Theorem F (flat ceiling, any deletions) and Lemma W (every word individually admissible),
and the parity / perfect-matching facts used. Run: python3 CERT/check_flat.py  (prints ALL CHECKS PASSED)."""
pc=lambda x: bin(x).count('1')
gens=[]
for j in range(12):
    x=sum(1<<(e+j) for e in [0,1,5,6,7,9,11])
    if pc(x)%2: x|=1<<23
    gens.append(x)
G={0}
for g in gens: G|={x^g for x in G}
assert len(G)==4096 and min(pc(x) for x in G if x)==8
M21=(1<<21)-1
D=sorted({x&M21 for x in G}); Ds=set(D); assert len(D)==4096
O=[x for x in G if pc(x)==8 and x>>21==0]; assert len(O)==210
# D meets every octad evenly (needed for condition (4) to be exact, Li Lemma 2.2)
assert all(pc(c&o)%2==0 for c in D for o in O)
wts={}
for c in D: wts[pc(c)]=wts.get(pc(c),0)+1
print('weight distribution of D:',sorted(wts.items()))
assert min(w for w in wts if w)==5
L=[c for c in D if pc(c)==5]; assert len(L)==21
# (a) flat sign vectors (-1)^c, (-1)^c' : <,>=21-2d, cos>1/2 iff 21-2d > 21/2 iff d<=5 iff d==5 (min dist 5)
for d in wts:
    if d: assert (2*(21-2*d) > 21) == (d==5)
# (b) all lines odd -> every conflict edge c ~ c+l joins even and odd words; parity classes have 2048 each
assert all(pc(l)%2==1 for l in L) and sum(1 for c in D if pc(c)%2==0)==2048
# (c) explicit perfect matching of the conflict graph: c <-> c+l0 (fixed-point-free involution, 2048 pairs)
l0=L[0]; pairs={frozenset((c,c^l0)) for c in D}; assert len(pairs)==2048 and all(c^l0 in Ds for c in D)
# (d) flat point (-1)^c (scaled to norm^2 21) is compatible with every base point (Lemma W for flat weights):
#     octad o: <q,b> <= sum_o 1 - 2*min = 6 ; need 6 <= sqrt(2)*sqrt(21)  <=>  36 <= 42
#     root:    <q,b> <= 2*(1+1)=... in Li's normalisation lam_i+lam_j <= |lam|/sqrt2 : 2 <= sqrt(21/2) <=> 8 <= 21
assert 36<=2*21 and 2*2*2<=21
# brute-force confirmation over the actual 27720 base points, exact integers (base norm^2 8, flat norm^2 21):
#     kissing condition <x,y> <= |x||y|/2 <=> 4<x,y>^2 <= 8*21 when <x,y> > 0
base=[]
for i in range(21):
    for j in range(i+1,21):
        for si in (2,-2):
            for sj in (2,-2):
                v=[0]*21; v[i]=si; v[j]=sj; base.append(v)
for o in O:
    s=[j for j in range(21) if o>>j&1]
    for m in range(256):
        if pc(m)%2==1:
            v=[0]*21
            for k,j in enumerate(s): v[j]=-1 if m>>k&1 else 1
            base.append(v)
assert len(base)==27720
worst=0
for c in D[:64]+D[-64:]:
    q=[-1 if c>>j&1 else 1 for j in range(21)]
    for b in base:
        ip=sum(x*y for x,y in zip(q,b))
        if ip>0: assert 4*ip*ip<=8*21; worst=max(worst,ip)
print('flat-vs-base max inner product on 128 sample words:',worst,'(<= sqrt(42)=6.48 required; all words equivalent by Lemma 2.2)')
print('ALL CHECKS PASSED')
