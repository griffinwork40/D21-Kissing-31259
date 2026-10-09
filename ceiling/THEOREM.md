# C1_ceiling: what is proved about the ceiling of family F (Cohn-Li/Golay construction in R^21)
2026-10-09T01:35Z. Notation as in Li (sources/paper.txt sec. 4): B = 27720 base points, D = Golay code punctured to 21
coordinates ([21,12,5], 4096 words, 21 lines = weight-5 words), added points lambda o (-1)^c, c in D, lambda > 0.

## Theorem F (flat ceiling; PROVED, exact certificate CERT/check_flat.py)
If every added point is flat (lambda constant, i.e. a multiple of (-1)^c), then whatever subset of B is kept,
at most 2048 added points can be present, so |config| <= 27720 + 2048 = 29768, and this is attained (Cohn-Li).
Proof. Flat points on the same word coincide in direction (cos 1), so one point per word. Two flat points on words at
distance d have cosine 1 - 2d/21 > 1/2 iff d = 5 (D has minimum distance 5; checked over the weight distribution).
So the used words form an independent set in Cay(D, lines). Every line has odd weight, so every edge joins an even
and an odd word; the map c -> c + l0 (l0 a fixed line) is a perfect matching of the 4096 words into 2048 edges, so an
independent set has at most 2048 words. Even words (2048) are compatible with all of B (condition (4): 6 <= sqrt(42),
2 <= sqrt(21/2); also brute-forced exactly against all 27720 base points on 128 words). Deletions do not help: the
bound never used B. This reproduces Gonzalez (f-keys.com/papers/added-vector-code-odd-sign-construction, Thm 12 /
Cor. 9, "maximal in dimensions 20 and 21") and Li's Lemma 4.1. It is our POSITIVE CONTROL.

## Lemma W (every word admissible; PROVED, same certificate)
With all 27720 base points kept, each of the 4096 words can carry a point (flat weights satisfy condition (4)).
So in sub-question (2) no word is excluded by the base; the only obstructions are between added points.
Li's 13 deletions are therefore NOT forced by the base for some choice of weights; they are forced only by his weights.

## Theorem Q1 (one point per word with FULL BASE; PROVED 2026-10-09T03:10Z)
Every nonnegative unit vector lambda satisfying condition (4) against all 27720 base points has
sum(lambda) > 397/100 = 3.97 > sqrt(63)/2. Consequently any two strictly positive such weight
vectors lambda and mu have cosine > 1/2, so each word of D carries at most one added point.

Exact certificate: CERT/q1_orbit_cert.jsonl. Standard library only verifier: CERT/check_q1.py.
Run with an absolute path, from any working directory:
`python3 /Users/griffinlong/Projects/research/kiss21-race/lanes/C1_ceiling/CERT/check_q1.py`.
Output: Q1 CERTIFICATE VALID, 2097152 subsets, 158 orbit boxes, 45 splits, 200 bound leaves,
3 Farkas leaves and 1694 upper tightenings. Runtime about 0.8 seconds on the laptop.

Proof certificate. Replace sqrt(2) by the rational upper bound 99/70 and 1/sqrt(2) by 99/140,
and add sum(lambda) <= 397/100. Initially 0 <= lambda_i <= 99/140 (pair constraints and
nonnegativity). Partition by H={i: lambda_i >= 21/100}. Two explicit coordinate permutations
preserve all 210 octads. The verifier traverses the subset orbits, independently checking that
158 representatives cover every one of the 2^21 subsets, with no duplicate or missing orbit.
It trusts neither a asserted group order nor a numerical symmetry reduction. For each box,
rational nonnegative LP dual multipliers certify tighter coordinate upper bounds. Binary splits
cover the remaining boxes. Every terminal box has either a strictly negative Farkas bound
(proving emptiness) or the bound sum(lambda_i^2) < 1. The latter uses the exact secant inequality
lambda_i^2 <= (l_i+u_i)lambda_i-l_i*u_i and rational LP duals with residuals repaired using the
box endpoints. Therefore no unit feasible vector can have sum <= 397/100.

The sum bound IS sufficient: let e=(1,...,1)/sqrt(21). For a unit feasible lambda,
lambda.e > (397/100)/sqrt(21) > sqrt(3)/2 because 4*(397/100)^2 > 63. Thus its angle to e
is strictly less than 30 degrees. The spherical triangle inequality gives angle(lambda,mu)
strictly less than 60 degrees, hence lambda.mu > 1/2. This corrects the former wrong caveat.

Sanity control: `python3 -P /Users/griffinlong/Projects/research/kiss21-race/lanes/C1_ceiling/work/sanity_q1.py`
checks exact homogeneous feasibility of weights 633 on a Golay line and 1000 elsewhere. Their
unit-normalized sum is 19165/sqrt(18003445) < 23/5. The same proof replay with target 23/5
rejects a norm leaf, as required. The optional `--self-test` in check_q1.py still contains an
incorrect 2/5 transcription of the paper's sqrt(2/5) line weight: a hook blocked its correction;
use the separate exact sanity control instead. The default certificate verification exits 0.

Scope: FULL BASE kept implies |configuration in F| <= 27720+4096 = 31816. Arbitrary base
deletions do not preserve condition (4), so this theorem does not claim a ceiling for that
larger deleted-base family. No optimizer record control or new kissing configuration is claimed.

## Observation Q3 (no local obstruction to 31816 found; numerical, NOT proved)
A word c together with all 21 neighbours c + l (the full star of distance-5 conflicts) can be realised with the full
base kept and maximal center-to-neighbour cosine 0.4512 < 1/2 (SLSQP, constraint violation 1e-13; work/star2.py).
So the distance-5 conflict, the binding one for flat weights, is not by itself an obstruction to using all words.
Upper bound for family F proved here: none better than 31816-with-Q1; without Q1 we have no finite bound beyond the
general kissing bounds. The true ceiling of F is OPEN; current best in F is 31035 (results/D21_31035_plain.txt: 3328
words used, one point each, 27707 base points).
