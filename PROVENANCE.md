# Provenance and verification log

## Campaign

| Item | Value |
|---|---|
| Campaign name | kiss21-race |
| Campaign started | 2026-10-08T23:20Z |
| Frozen spec tag | freeze-v1 |
| Frozen spec file | frozen/FROZEN.md (never edited) |
| Goal | kissing configuration in R^21 with N > 30779 |
| Prior record | 30779 (Kevin Li, github.com/Felpix-Studios/D19-D21-Kissing, commit 9bb343f, 2026-10-04) |

## Operator and system

| Item | Value |
|---|---|
| Operator | Griffin Long (griffinwork40@gmail.com) |
| Role | Set the goal, approved compute allocation, defined success criteria |
| Search engine | agent-afk (autonomous agent framework, open source, github.com/griffinwork40/agent-afk) |
| Model used | gpt-6.1-sol |
| Lane | L2_fill |
| Compute | Mac mini (M4, 10 cores, macOS arm64, nice -n 10, nohup) + MacBook laptop |

## Timeline (UTC)

| Time | Event |
|---|---|
| 2026-10-08T23:20Z | Campaign started; frozen spec written and committed |
| 2026-10-08T23:17Z | Positive control: one 16-point orbit perturbed ±0.2%, optimizer recovered VALID 30779. CONTROL_30779.txt passes frozen checker on mini and laptop. (Single-orbit control, not full rediscovery.) |
| 2026-10-08T23:17Z | All 1024 unused words of D screened by SLSQP minimax; best cosine 0.5326 with Li's weights fixed. No single-orbit fixed-baseline insertion found. |
| 2026-10-08T23:19Z | Coupled joint re-optimization of 5 candidate orbits + 192 Li orbits; 5 valid 30795-point candidates found. |
| 2026-10-08T23:21Z | Greedy orbit insertion phase begins; HIT_30811.txt. |
| 2026-10-08T23:21Z | HIT_30827.txt |
| 2026-10-08T23:22Z | HIT_30843 through HIT_30875 |
| 2026-10-08T23:23Z | HIT_30891.txt; independently verified with Li's check_pairs.py |
| 2026-10-08T23:23Z to 23:27Z | HIT_30907 through HIT_31067, greedy adding one orbit per ~30s |
| 2026-10-08T23:27Z | Greedy handed off to Mac mini unattended; HIT_31083 onward |
| 2026-10-08T (after 23:27Z) | HIT_31099 through HIT_31259, greedy adding 30 more orbits |
| 2026-10-08T23:30Z | HIT_30891 verified: frozen checker_c (Mac) VALID, frozen checker_c (Linux desk, gcc) VALID, Li's check_pairs.py CHECK PASS, min gap 2.937e-08. Recorded in results/VERIFIED.md. |
| 2026-10-08T23:40Z | HIT_31035 verified: frozen checker_c VALID (Mac + Linux), check_pairs.py CHECK PASS, min gap 2.916e-08. Recorded in results/VERIFIED.md. |
| 2026-10-08T23:35Z | Novelty recheck: Li repo HEAD = 9bb343f (unchanged); Cohn table d21 = 29768; no arXiv result above 30779; full web sweep max d21 = 30779. |
| 2026-10-08 (same day) | HIT_31259 verified: frozen checker_c VALID (Mac, exit 0, 1.28s); Li's check_pairs.py CHECK PASS (exit 0, 31.4s), violations 0, distinct directions 31259, rank 21. |

## Human interventions (HUMAN_INPUT.md equivalent)

| Time | Person | Action |
|---|---|---|
| 2026-10-08T23:20Z | Griffin Long | Set campaign goal (N > 30779); approved compute on Mac mini and laptop |
| 2026-10-08 | Griffin Long | Approved continued greedy run on Mac mini after initial 30891 verified |

## Historical artifact hashes (SHA256)

The following records are from the initial bundle, before the final text correction pass. The construction and paper text have since changed; use `SHA256SUMS` for current bundle hashes. Data and checker hashes are unchanged.

| File | SHA256 |
|---|---|
| data/D21_31259_plain.txt | e4f57198b80e600433e70592bf1f55752b72da5595014747f1197b782c317887 |
| data/D21_31259_rays.txt | c33c47573f4d2c5956654df27ea8192e3cda384fb2066fa58fb11ddb95d65e4a |
| constructions/D21-31259.txt | d80f5025d7edf577a349f2a6f04251cedacdf45404ec18cae71f402381af9277 |
| paper/D21-31259.md | 293f3d2fab4a106fe730320235a848771280efe17569a592a48ede73c47efe9e |
| scripts/check.py | d2c0a27eb8788c422930d456d66c9242149dae1977810895371433f805c61c81 |
| scripts/checker_c.c | 8d70de3ebc1ba470dfa0591029ecc3719b7c5d8ad3fb72fd9c85b3955663473d |

## Blind control

A sealed agent session given only Cohn-Li's 29768-point file reached **30536**, rediscovering the weight-push mechanism unprompted. Included files: `control/BEST_30536.txt` and `control/K0c_FINAL.md` (copied from `lanes/K0c_sealed_daemon/`). The session disclosed general background knowledge and awareness of Cohn-Li (2024), but no details of the later mechanism. The coordinator reran its exact checker: VALID. Two earlier controls reaching 30021 and 30551 received a one-line hint from a sibling lane and are reported as method-development controls only. The sealed result is a mechanism rediscovery, not a complete independent reconstruction of Li's 30779 configuration.

## Verification

* Frozen C checker: VALID on Mac arm64 and Linux x86_64.
* Li's check_pairs.py: 488,546,911 pairs, 0 violations, CHECK PASS.
* Standard-library scripts/check.py: VALID in about 6.5 minutes (6m33s).
* Independent freshly written checker (lane V1): VALID in 1.6 seconds.

The independently rebuilt base is 840 roots + 26867 octad vectors (210 × 128 minus 13) = 27707. All base vectors are retained literally; all 3072 Li dense directions are replaced by re-weighted versions with the same sign patterns, plus 480 previously unused words.

## Verification receipts

### data/D21_31259_plain.txt (N = 31259)

**Frozen C checker (Mac, arm64):**
```
Dimension: 21
Vectors read: 31259
VALID
Points: 31259
Time: 1.28 s
Exit code: 0
```

**Kevin Li's check_pairs.py (independent Python integer checker):**
```
rows 31259; every stated norm equals the sum of squares
distinct directions 31259
rank 21: exact determinant -4194304
pairs 488546911, violations 0, contacts 12975933
Python-int recheck of 2020091 pairs: {ok: 2011881, contact: 8210, viol: 0}, 0 mismatches
  base-base pairs 383825071  violations 0  contacts 12975933
  base-new  pairs  98415264  violations 0  contacts 0  within 1e-6 of 1/2: 43680
  new-new   pairs   6306576  violations 0  contacts 0  within 1e-6 of 1/2: 20464
  smallest exact gap base-new:  2.923546e-08 (rows 22761, 28963)
  smallest exact gap new-new:   2.893720e-08 (rows 27779, 29412)
time 31.4 s
CHECK PASS
Exit code: 0
```

**Also verified in results/VERIFIED.md:**
- N = 30891: frozen checker_c VALID (Mac + Linux desk gcc), check_pairs.py CHECK PASS, sha256 3ff0cd3b...7ac30
- N = 31035: frozen checker_c VALID (Mac + Linux), check_pairs.py CHECK PASS, sha256 e6b0ab6f...b0b6f

## Notes

- The frozen checker (scripts/checker_c.c) was written before the search and frozen at tag freeze-v1.
  Its mutation-test controls (mut_dup, mut_close, mut_perturb -> INVALID; ok_drop -> VALID) passed at freeze.
  It was never edited during the campaign.
- The positive control is a single-orbit recovery, not a full randomized rediscovery from scratch.
  This affects confidence in the search method but does not affect the validity of the certificates.
- The greedy orbit insertion is a heuristic; it is not exhaustive. Orbits that were not inserted may
  still be insertable with a different insertion order or larger weight deformations.
- N = 31259 was subsequently rerun with the frozen C checker on Linux x86_64 (desk): VALID.
- Full-base ceiling certificates from lanes/C1_ceiling are bundled unchanged in ceiling/; they do not bound configurations with base deletions. Attainability of 31816 remains open.

## Update 2026-10-10: 31272 (full base, zero deletions)

- 2026-10-09T17:03Z: lane M1b_attain (agent-afk, Claude Opus, laptop CPU) produced 31272 = all 27720 Cohn-Li base points
  (Li's 13 deleted points restored) + 3552 weighted sign points on the same 3552 words of D as the 31259 release.
  Method: starting from the 31259 weights, minimise a squared hinge over the exact full-base condition (all 1890 rows per
  word) plus pairwise cos <= 1/2 - 3e-8 (active set), L-BFGS, then round to integers at scale ~1e9 and exact-check.
  Positive control: from the 31259 weights perturbed log-normally (1e-3, 3e-2) the solver re-reached VALID 31259 files
  (method control, not a from-scratch rediscovery).
- 2026-10-09T17:08Z: frozen checker_c VALID on Mac arm64 (2.95 s) and Linux x86_64 desk (5.35 s); Kevin Li's
  check_pairs.py CHECK PASS, 488,953,356 pairs, 0 violations.
- Held private until Griffin's go (2026-10-10, ~02:00Z). Before release: novelty recheck (Li's repo HEAD unchanged since
  2026-10-05T21:26Z; Cohn table d21 = 29768; newest arXiv kissing papers cover other dimensions), frozen checker rerun on
  the repo copy (VALID, 31272 points), Li's check_pairs.py rerun on data/D21_31272_rays.txt (CHECK PASS, log in data/check_pairs_31272.log).
- data/D21_31272_plain.txt sha256 361758425d2df467e98e5542a850431670048d0b4dd66b7c329f2837ce5d23fb

## Update 2026-10-10 (second): 31288 (full base, zero deletions)

- 2026-10-10T21:33Z: orbit-swap job S2 of lane M1b_attain (agent-afk; laptop CPU), started from the 31272 state (222 orbits of
  Li's order-16 sign group), trial 131: removed 1 orbit, added 2 previously unused orbits, re-solved all orbit weights jointly
  (penalty for cos > 1/2 - 3e-8 plus the exact full-base condition), max cos 0.49999997, rounded at scale ~1e9, frozen
  checker VALID. Result: 27720 base + 3568 dense points on 3568 words of D (223 orbits) = 31288.
- 2026-10-10T22:54Z-23:30Z verification: frozen checker_c VALID on Mac arm64 and Linux x86_64 (same sha256); Kevin Li's
  check_pairs.py CHECK PASS (489,453,828 pairs, 0 violations, smallest exact gap 2.93e-08); an independently written
  exact C checker (__int128, all pairs; mutated copies rejected) VALID; independent rebuild of the Golay octads confirms
  840 roots + 26880 octad sign points + 3568 dense; the 27720 base points equal, point for point, the base of Kevin Li's
  public 31272 file (Felpix-Studios/kissing-number), and Li's 30779 base is this base minus exactly 13 points.
- Novelty recheck before release: Cohn table d21 = 31272; Li's repos unchanged since 2026-10-10T03:35Z (D21 file 31272);
  arXiv:2609.35051 has no d21 result; GitHub search found nothing above 31272.
- Released by Griffin's go ("whatever you think is best proceed", 2026-10-10). The search continues until 2026-10-11T21:55Z.
- data/D21_31288_plain.txt sha256 b51b7b9c376ff226695a8b8c8f880e9202981deb7a6dd8f5f21d7598d9be064b
