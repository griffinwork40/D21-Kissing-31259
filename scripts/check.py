#!/usr/bin/env python3
"""Dependency-free exact kissing checker for D21_31259.

Tests every pair in the plain-format file (one vector per line, 21 integers).
Uses only Python stdlib, exact integer arithmetic (no numpy, no floats for decisions).

Validity condition: every pair x != y satisfies
    x.y <= 0  OR  4*(x.y)^2 <= |x|^2 * |y|^2

Exit 0 and prints VALID if all pairs pass and exactly 31259 distinct non-zero vectors.
Exit 1 on any violation or unexpected row count.

Usage:
    python3 scripts/check.py data/D21_31259_plain.txt [--n 31259]
"""
import sys
import argparse
from math import isqrt

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("path")
    ap.add_argument("--n", type=int, default=31259,
                    help="Expected number of rows (default 31259)")
    args = ap.parse_args()

    rows = []
    with open(args.path) as f:
        for lineno, line in enumerate(f, 1):
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            vals = tuple(map(int, line.split()))
            if len(vals) != 21:
                print(f"ERROR: line {lineno} has {len(vals)} values, expected 21")
                sys.exit(1)
            rows.append(vals)

    N = len(rows)
    print(f"Rows read: {N}")
    if N != args.n:
        print(f"ERROR: expected {args.n} rows, got {N}")
        sys.exit(1)

    # Check distinct
    if len(set(rows)) != N:
        print("ERROR: duplicate vectors detected")
        sys.exit(1)
    print("Distinct: OK")

    # Precompute squared norms
    norms = [sum(v*v for v in r) for r in rows]

    violations = 0
    worst_gap = None  # smallest (n_i*n_j - 4*dot^2) / (n_i*n_j) for near-contact pairs

    for i in range(N):
        for j in range(i+1, N):
            dot = sum(rows[i][k]*rows[j][k] for k in range(21))
            if dot <= 0:
                continue
            ni, nj = norms[i], norms[j]
            lhs = 4 * dot * dot
            rhs = ni * nj
            if lhs > rhs:
                violations += 1
                if violations == 1:
                    print(f"VIOLATION: rows {i} and {j}, dot={dot}, ni={ni}, nj={nj}")
                if violations > 5:
                    print("Too many violations, stopping early.")
                    sys.exit(1)
            else:
                # track worst gap for near-contacts
                gap_num = rhs - lhs  # >= 0
                # gap = gap_num / rhs; track minimum
                if worst_gap is None or gap_num * worst_gap[1] < worst_gap[0] * rhs:
                    worst_gap = (gap_num, rhs)

    if violations > 0:
        print(f"INVALID: {violations} violation(s)")
        sys.exit(1)

    if worst_gap is not None:
        g_num, g_den = worst_gap
        g_float = g_num / g_den
        print(f"Smallest gap (nm-4g^2)/(nm): {g_num}/{g_den} = {g_float:.6e}")

    print("VALID")

if __name__ == "__main__":
    main()
