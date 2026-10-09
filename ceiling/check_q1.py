"""Exact Q1 certificate verifier, Python standard library only.
This lane verifier is under active development, explicitly requested by the
current task. It is NOT the frozen campaign checker; frozen/ is untouched.
Default: verify q1_orbit_cert.jsonl next to this script.
Each box covers a high-coordinate subset at threshold 21/100. Certified
coordinate upper bounds and binary splits precede secant or Farkas leaves.
A complete orbit traversal of all 2**21 subsets verifies coverage; no group
order or orbit-representative list is trusted. Use --self-test for controls.
"""
import json
import sys
import time
from array import array
from fractions import Fraction as F
from pathlib import Path


def geometry():
    gens=[]
    for j in range(12):
        x=sum(1 << (e+j) for e in [0,1,5,6,7,9,11])
        if x.bit_count()%2: x |= 1 << 23
        gens.append(x)
    code={0}
    for g in gens: code |= {x^g for x in code}
    assert len(code)==4096
    octads=[x for x in code if x.bit_count()==8 and x >> 21 == 0]
    assert len(octads)==210
    lines=[x & ((1 << 21)-1) for x in code if (x & ((1 << 21)-1)).bit_count()==5]
    assert len(set(lines))==21
    return octads, sorted(set(lines))


def coverage(gens, reps, octads):
    n=1 << 21
    assert len(reps)==len(set(reps)) and all(type(r) is int and 0<=r<n for r in reps)
    os=set(octads)
    tables=[]
    for p in gens:
        assert sorted(p)==list(range(21))
        def image(x):
            return sum(1 << p[i] for i in range(21) if x >> i & 1)
        assert {image(o) for o in octads}==os, 'generator does not preserve octads'
        tables.append(([image(x) for x in range(1 << 10)],
                       [image(x << 10) for x in range(1 << 11)]))
    assert tables
    seen=bytearray(n)
    covered=0
    for rep in reps:
        assert not seen[rep], 'duplicate orbit representative'
        queue=array('I',[rep]);seen[rep]=1;head=0
        while head<len(queue):
            x=queue[head];head+=1
            for low,high in tables:
                y=low[x & 1023] | high[x >> 10]
                if not seen[y]:seen[y]=1;queue.append(y)
        covered+=len(queue)
    assert covered==n, 'uncovered high-coordinate subset'
    return covered


def verify(path, target_override=None):
    start=time.monotonic()
    octads,lines=geometry()
    with open(path) as fh:
        d=json.loads(next(fh))
        BO,BR,S,tau=map(F,(d['BO'],d['BR'],d['S'],d['tau']))
        if target_override is not None:S=F(target_override)
        assert BO>0 and BO*BO>=2
        assert BR>0 and 2*BR*BR>=1
        assert S>0 and 4*S*S>63
        assert 0<tau<BR
        covered=coverage(d['gens'],d['reps'],octads)
        rows=[];rhs=[]
        for o in octads:
            support=[j for j in range(21) if o >> j & 1]
            for j in support:
                rows.append(tuple((i,1 if i!=j else -1) for i in support));rhs.append(BO)
        for i in range(21):
            for j in range(i+1,21):rows.append(((i,1),(j,1)));rhs.append(BR)
        rows.append(tuple((i,1) for i in range(21)));rhs.append(S)
        stream=iter(fh);counts={'U':0,'S':0,'B':0,'F':0};worst=F(0)
        def dual(raw,c,l,u):
            lhs=[F(0)]*21;value=F(0)
            assert isinstance(raw,dict)
            for key,v in raw.items():
                k=int(key);y=F(v)
                assert 0<=k<len(rows) and y>=0
                value+=y*rhs[k]
                for i,a in rows[k]:lhs[i]+=a*y
            for i in range(21):
                delta=c[i]-lhs[i]
                value+=delta*(u[i] if delta>=0 else l[i])
            return value
        def box(l,u):
            nonlocal worst
            while True:
                node=json.loads(next(stream));tag=node[0]
                assert tag in counts
                counts[tag]+=1
                if tag=='U':
                    assert len(node)==4
                    i,v,y=node[1],F(node[2]),node[3]
                    assert type(i) is int and 0<=i<21 and l[i]<=v<u[i]
                    c=[F(0)]*21;c[i]=F(1)
                    assert dual(y,c,l,u)<=v, 'invalid coordinate upper bound'
                    u=list(u);u[i]=v
                elif tag=='S':
                    assert len(node)==3
                    i,mid=node[1],F(node[2])
                    assert type(i) is int and 0<=i<21 and l[i]<mid<u[i]
                    u2=list(u);u2[i]=mid;l2=list(l);l2[i]=mid
                    box(l2,u);box(l,u2)
                    return
                elif tag=='F':
                    assert len(node)==2
                    assert dual(node[1],[F(0)]*21,l,u)<0, 'invalid Farkas leaf'
                    return
                else:
                    assert len(node)==2
                    bound=dual(node[1],[l[i]+u[i] for i in range(21)],l,u)-sum(l[i]*u[i] for i in range(21))
                    assert bound<1, 'norm bound does not exclude unit vector'
                    worst=max(worst,bound)
                    return
        for rep in d['reps']:
            l=[tau if rep >> i & 1 else F(0) for i in range(21)]
            u=[BR if rep >> i & 1 else tau for i in range(21)]
            box(l,u)
            assert json.loads(next(stream))==['REP',rep]
        assert json.loads(next(stream))==['END']
        assert next(stream,None) is None, 'trailing certificate data'
    print('Q1 CERTIFICATE VALID: sum >',S,'; subsets',covered,'; orbit boxes',len(d['reps']),
          '; splits',counts['S'],'; bound leaves',counts['B'],'; Farkas leaves',counts['F'],
          '; upper tightenings',counts['U'])
    print('Largest certified squared-norm bound:',worst,'< 1')
    print('Elapsed seconds:',round(time.monotonic()-start,3))
    return octads,lines


def control(path):
    octads,lines=geometry()
    # Li line model, unnormalised integer weights 2 on a line and 5 elsewhere.
    # Norm²=420. Exact homogeneous constraints after normalising by sqrt(420).
    line=lines[0];w=[2 if line >> i & 1 else 5 for i in range(21)]
    norm=sum(x*x for x in w);total=sum(w)
    assert norm==420 and total==90 and min(w)>0
    for o in octads:
        z=[w[j] for j in range(21) if o >> j & 1]
        assert (sum(z)-2*min(z))**2<=2*norm
    assert all(2*(w[i]+w[j])**2<=norm for i in range(21) for j in range(i+1,21))
    assert F(total*total,norm)<F(23,5)**2
    try:verify(path,F(23,5))
    except (AssertionError,StopIteration,ValueError) as e:
        print('FALSE TARGET 23/5 REJECTED:',str(e))
    else:raise AssertionError('false target accepted')
    print('EXACT COUNTEREXAMPLE VALID: line weights 2/5, sum = 90/sqrt(420) < 23/5')


if __name__=='__main__':
    args=[x for x in sys.argv[1:] if x!='--self-test']
    path=Path(args[0]) if args else Path(__file__).with_name('q1_orbit_cert.jsonl')
    try:
        verify(path)
        if '--self-test' in sys.argv:control(path)
    except (AssertionError,StopIteration,ValueError,KeyError,OSError) as e:
        print('Q1 CERTIFICATE INVALID:',str(e),file=sys.stderr)
        sys.exit(1)
