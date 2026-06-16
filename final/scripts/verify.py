#!/usr/bin/env python3
"""Compare a `ranks.txt` produced by our C code against NetworkX's PageRank.

Usage:
    python verify.py <edges_file> <ranks_file> [--damping 0.85] [--tol 1e-7]

Exit code 0 if the top-K ordering matches and L1 distance is below threshold,
else 1.  This script is the ground-truth for correctness in the project.
"""

import argparse
import sys
from pathlib import Path


def load_edges(path: Path):
    edges = []
    with path.open() as f:
        for line in f:
            line = line.strip()
            if not line or line[0] in "#%":
                continue
            s, d = line.split()[:2]
            edges.append((int(s), int(d)))
    return edges


def load_ranks(path: Path):
    ranks = {}
    with path.open() as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            v, s = line.split()
            ranks[int(v)] = float(s)
    return ranks


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("edges_file")
    ap.add_argument("ranks_file")
    ap.add_argument("--damping", type=float, default=0.85)
    ap.add_argument("--tol", type=float, default=1e-7)
    ap.add_argument("--topk", type=int, default=10)
    ap.add_argument("--l1-threshold", type=float, default=1e-4)
    args = ap.parse_args()

    try:
        import networkx as nx
    except ImportError:
        print("ERROR: networkx is required (pip install networkx)", file=sys.stderr)
        return 2

    edges = load_edges(Path(args.edges_file))
    g = nx.DiGraph()
    g.add_edges_from(edges)
    ref = nx.pagerank(g, alpha=args.damping, tol=args.tol)

    ours = load_ranks(Path(args.ranks_file))

    common = set(ref.keys()) & set(ours.keys())
    l1 = sum(abs(ref[v] - ours[v]) for v in common)
    print(f"[verify] |ours|={len(ours)}  |ref|={len(ref)}  common={len(common)}")
    print(f"[verify] L1(ref, ours) = {l1:.3e}  (threshold {args.l1_threshold})")

    top_ref = sorted(ref.items(),  key=lambda kv: -kv[1])[: args.topk]
    top_our = sorted(ours.items(), key=lambda kv: -kv[1])[: args.topk]
    print(f"[verify] top-{args.topk} reference: {[v for v, _ in top_ref]}")
    print(f"[verify] top-{args.topk} ours     : {[v for v, _ in top_our]}")

    ok = l1 < args.l1_threshold
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
