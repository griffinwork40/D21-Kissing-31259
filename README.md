# Kissing configurations in R^21: 31288 points (latest), 31272, 31259

> **Search ongoing until 2026-10-11 ~22:00 UTC; the number may improve.** This notice will be removed when the run ends.

**Update 2026-10-10 (second): 31288 points.** `data/D21_31288_plain.txt` (and `data/D21_31288_rays.txt`, Li's rays format)
is a kissing configuration in 21 dimensions with **31288 points**: the **full 27720-point Cohn-Li base with zero deletions**
plus 3568 weighted sign-pattern points on 3568 words of D (223 orbits of Li's order-16 sign group). It was obtained from the
31272 configuration by an orbit swap: remove one orbit, add two previously unused orbits, re-solve all weights jointly,
round to exact integers at scale ~1e9. It lies in the full-base family whose ceiling, 31816, is proved in `ceiling/`.

Check it:

```bash
cc -O2 -o checker_c scripts/checker_c.c
./checker_c data/D21_31288_plain.txt          # VALID, Points: 31288
python3 path/to/D19-D21-Kissing/scripts/check_pairs.py data/D21_31288_rays.txt --n 21 --expect-rows 31288   # CHECK PASS
```

Receipts: frozen C checker VALID on Mac arm64 and Linux x86_64; Kevin Li's check_pairs.py CHECK PASS (489,453,828 pairs,
0 violations, smallest exact gap 2.93e-08; log in `data/check_pairs_31288.log`); an independently written exact checker
(128-bit integer arithmetic, all pairs) also returns VALID. The 27720 base points are identical, point for point, to the
base of Kevin Li's own 31272 file. Found 2026-10-10T21:33Z by an agent-afk search lane (orbit-swap job). Details in `PROVENANCE.md`.

**Credit.** This builds entirely on Kevin Li's construction. Kevin Li independently obtained a 31272-point configuration
(github.com/Felpix-Studios/kissing-number), and the d = 21 entry of Henry Cohn's table cites both.

---

## Update 2026-10-10: 31272 points

`data/D21_31272_plain.txt` (and `data/D21_31272_rays.txt`, Li's rays format)
is a kissing configuration in 21 dimensions with **31272 points**. It keeps the **full 27720-point Cohn-Li base with
zero deletions** (the 13 base points Li's 30779 construction removed are restored) and places 3552 weighted
sign-pattern points on the same 3552 words of D as the 31259 release (they form 222 orbits of Li's order-16 sign group).
It lies in the full-base family whose ceiling, 31816, is proved by the certificates in `ceiling/`.

Check it:

```bash
cc -O2 -o checker_c scripts/checker_c.c
./checker_c data/D21_31272_plain.txt          # VALID, Points: 31272
python3 path/to/D19-D21-Kissing/scripts/check_pairs.py data/D21_31272_rays.txt --n 21 --expect-rows 31272   # CHECK PASS
```

Receipts: frozen C checker VALID on Mac arm64 and Linux x86_64; Kevin Li's check_pairs.py CHECK PASS
(488,953,356 pairs, 0 violations, smallest exact gap 9.07e-09, exact rational). Found 2026-10-09T17:03Z by an agent-afk lane
(Claude Opus) that repaired the 31259 weights against the full base with an active-set L-BFGS solve, rounded to exact
integers at scale ~1e9. Positive control: the same solver re-reached a VALID 31259 from perturbed weights (a method
control, not a from-scratch rediscovery). Details in `PROVENANCE.md`. Credit as below: this extends Kevin Li's work.

---

## First release (2026-10-09): 31259 points

This repository provides a kissing configuration in 21 dimensions with **31259 points**,
improving the previous best known lower bound of 30779 due to Kevin Li (2026).

> **Credit first.** The entire base of this construction is the work of Kevin Li
> (github.com/Felpix-Studios/D19-D21-Kissing, paper "Improved kissing numbers in
> nineteen and twenty-one dimensions"). We extend his 30779-point construction by
> adding 480 points via joint weight re-optimization and greedy orbit insertion.
> This keeps all 27707 of Li's base points unchanged, keeps the sign patterns of
> his 3072 added points but replaces every dense direction by a jointly re-weighted version, and adds 480 new
> points (30 orbits) from previously unused words of D.
> We also credit H. Cohn and A. Li (arXiv:2411.04916) for the foundational
> 27720-point base and the theoretical framework.

## Contents

| Path | What |
|---|---|
| `data/D21_31288_plain.txt` | **31288** vectors (latest), one per line, 21 integers |
| `data/D21_31288_rays.txt` | Same, Li's rays format (squared norm then 21 integers) |
| `data/check_pairs_31288.log` | Kevin Li's check_pairs.py run on the 31288 rays file |
| `data/D21_31272_plain.txt` | 31272 vectors, one per line, 21 integers |
| `data/D21_31272_rays.txt` | Same, Li's rays format (squared norm then 21 integers) |
| `constructions/D21-31259.txt` | Plain-text description of the construction |
| `data/D21_31259_plain.txt` | 31259 vectors, one per line, 21 integers |
| `data/D21_31259_rays.txt` | Same, Li's rays format (squared norm then 21 integers) |
| `scripts/check.py` | Dependency-free exact Python checker (stdlib only) |
| `scripts/checker_c.c` | C source of the frozen checker |
| `paper/D21-31259.md` | Short write-up: statement, construction, verification, how it was found |
| `PROVENANCE.md` | Timeline, human inputs, and historical artifact hashes |
| `ceiling/` | Exact full-base ceiling theorem and certificate verifiers |
| `control/` | Sealed blind-control configuration (30536) and session report |
| `SHA256SUMS` | SHA256 hashes |
| `LICENSE` | MIT for code; CC BY 4.0 for data and text |

## Checking the configuration

```bash
# Build and run the frozen C checker (fastest, ~1.3 s)
cc -O2 -o checker_c scripts/checker_c.c
./checker_c data/D21_31259_plain.txt

# Run Li's exact all-pairs Python checker (~30 s, last line: CHECK PASS)
python3 path/to/D19-D21-Kissing/scripts/check_pairs.py \
    data/D21_31259_rays.txt --n 21 --expect-rows 31259

# Run the dependency-free stdlib Python checker (very slow for N=31259: O(N^2) pairs)
# Full verification was completed in about 6.5 minutes.
python3 scripts/check.py data/D21_31259_plain.txt
```

`scripts/check.py` is exact and took 6m33s (about 6.5 minutes) for full verification; `scripts/checker_c.c` checks the file in about a second (build: `cc -O2 -fopenmp checker_c.c -o checker_c`, or without `-fopenmp`).

## Verification receipts

* Frozen C checker: VALID on Mac arm64 and Linux x86_64.
* Li's check_pairs.py: 488,546,911 pairs, 0 violations, CHECK PASS.
* Standard-library scripts/check.py: VALID in about 6.5 minutes.
* Independent freshly written checker (lane V1): VALID in 1.6 seconds.

See `PROVENANCE.md` for receipts. The exact vectors in `data/` are the certificate; the prose describes the search, not a data-free rebuild.

Key numbers from check_pairs.py:
- 488,546,911 pairs checked, 0 violations
- 12,975,933 contacts (cosine exactly 1/2), all between base points
- Smallest gap (1/4 - cos^2): ~2.89e-08 (new-new), ~2.92e-08 (base-new)

## Ceiling

Exact certificates in `ceiling/` prove a ceiling of 31816 for the weighted-sign family with all 27720 base points kept (flat additions at most 2048; coordinate sum > 3.97 forces one point per word of D); this theorem does not cover base deletions, and attainability of 31816 is open.

From `ceiling/`, run `python3 check_flat.py` and `python3 check_q1.py` (default verification; see the optional self-test caveat in `THEOREM.md`).

## How it was found

The search ran under **agent-afk** (github.com/griffinwork40/agent-afk, open source),
model gpt-6.1-sol, on a Mac mini (M4, 10 cores). Griffin Long is the operator.
Campaign started 2026-10-08T23:20Z; 31259 verified on the same day.

Method: screen all 1024 unused words of the punctured Golay code D; jointly re-optimize
orbit weights by SLSQP minimax; greedily add orbits one at a time, rounding to exact
integers at scale ~1e9 after each step, checking with the frozen C checker.
