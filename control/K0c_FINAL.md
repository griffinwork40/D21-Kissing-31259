# FINAL (K0c sealed run)

## Best valid N: **30536** (+768 over the 29768 start)
File: `BEST_30536.txt`. `./checker` prints VALID and exits 0. Intermediate valid files: `BEST_29769.txt`, `BEST_30280.txt`.

## How it works
1. **What the start file contains** (found by analysing it):
   - 840 vectors (±2,±2,0^19).
   - 26880 = 210 × 128 vectors ±1^8. The 210 supports are the symmetric differences of two lines of a projective plane of order 4 on the 21 coordinates. Each support carries every sign pattern with an odd number of minus signs.
   - 2048 vectors ±1^21. Their minus-sign sets form a linear code C of dimension 11 and minimum distance 6.
   - The octad constraints allow exactly 4096 sign words for the ±1^21 vectors: C plus its complement coset (the "odd" words). An odd word and an even word conflict exactly when they differ on one line (5 coordinates). That conflict graph is 21-regular and bipartite, so 2048 is the most ±1^21 points you can have if all shapes stay fixed.
2. **The slack:** the ±1^21 points are only tight against each other. Against the norm-8 core their largest cosine is 6/√168 ≈ 0.463, which is below 1/2.
3. **The mechanism:**
   - Add some odd words. These are the antipodes of existing ±1^21 points.
   - For each even word that conflicts with an added odd word, make its entries larger on the coordinates of the conflicting line. This "pushes" it to more than 60° from the new point, and it stays at 60° or more from the core.
   - On its own, a single odd word works with entries 8 → 9 (N = 29769). Random greedy additions at that one scale reached 29908.
4. **Making it scale:**
   - Choose the added set S = 1 + f⁻¹(T), where f: C → F₂^m is linear and T ⊂ F₂^m. Each even word then sees a line set that depends only on its class f(c), so there are only 2^m weight-shaping problems to solve.
   - Each problem has 1890 core constraints plus the pushed lines. SLSQP solved them for optimal real weights. I scaled those by 1000 and rounded to integers.
   - m = 2 gives +512 (30280). m = 4 with |T| = 6 gives **+768 (30536)**: line classes (0^6, 2^9, 3^1), at most 9 pushed lines per even word, smallest margin 0.00059 before rounding.

## What failed
- **Hard ceiling for this mechanism:** with the core fixed, every sampled even word with 10 or 11 pushed lines was infeasible. That caps any S at 9·2048/21 ≈ 877 (+864 for linear m = 6, +832 for m = 5).
- **m = 5 searches** (random G with greedy T, then DFS with feasibility pruning, about 40 CPU-min) found nothing better than +640.
- **Simulated annealing** over arbitrary S reached only +400 from scratch and never improved on the 768 solution.
- **Extending 768 to 832** by adding one more odd class to the m = 5 lift failed in two ways:
  - Every candidate exceeds 9 lines per even word.
  - A joint optimisation that also lets the odd points and all class shapes move (SLSQP and a soft-min gradient method) improved the 768 base margin to 0.0028. But every 23-class set stayed negative (best −0.00025).
- **Swaps** (drop r classes, add r+1) all break the 9-line bound.
- **Random 5-dimensional labellings of the whole 4096-word space** never produced more than 22 classes that pass the line-count cap.
- **Not tried:** moving or deleting core points, other norm shells, or non-±1 directions beyond reweighting.

## Own analysis vs prior knowledge
- **My own work, from analysing the file and running experiments here:**
  - every structural fact above: the plane-of-order-4 line structure, odd versus even words, the bipartite conflict graph, the slack against the core;
  - the antipode/push mechanism, the linear class construction, the SLSQP weight shaping, and all the bounds and searches.
- **What I brought in:**
  - The task names Cohn & Li (2024) as the source of 29768. I believe their paper is about kissing configurations in dimensions 17–21 obtained by sign changes to Leech-lattice-type constructions. I do not remember its details or any specific later improvement for dimension 21, and I did not use any.
  - General background: binary codes, projective planes, and the fact that kissing-number records are often built from code-based ±1 vectors.
- A hash-keyed feasibility cache sped up the search. Its key is not a complete invariant, so it is only a heuristic. Every number reported above was confirmed with `./checker`.
- Wall-clock used: about 1h35m.
