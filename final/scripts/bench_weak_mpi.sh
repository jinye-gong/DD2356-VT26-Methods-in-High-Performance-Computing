#!/usr/bin/env bash
# Weak-scaling sweep for MPI PageRank.
#
# Strategy:
#   For each ranks = P, generate an input graph with ~edges_per_rank * P edges
#   (by trimming the original edge list). Then run the MPI solver on that input.
#
# NOTE:
#   This is a pragmatic weak-scaling approximation. If your project insists on
#   strict weak scaling with synthetic R-MAT at constant edges_per_rank, swap
#   `trim_edges.py` with your generator.
#
# Usage:
#   ./bench_weak_mpi.sh data/web-Google.txt "1 2 4 8" 200000
#
# Where:
#   edges_per_rank is the number of edges to keep per MPI rank.
#
set -euo pipefail
cd "$(dirname "$0")/.."

INPUT=${1:-data/sample.edges}
RANKS=${2:-"1 2 4 8"}
EDGES_PER_RANK=${3:-10000}

OUT=results/weak_mpi.csv
mkdir -p results
echo "ranks,edges,seconds,input" > "$OUT"

for p in $RANKS; do
  edges=$((EDGES_PER_RANK * p))
  trimmed="results/weak_input_${p}r_${EDGES_PER_RANK}e.txt"
  if [[ ! -f "$trimmed" ]]; then
    python3 scripts/trim_edges.py "$INPUT" "$trimmed" --edges "$edges"
  fi

  echo "[bench] weak: ranks=$p edges=$edges"

  # MPI launcher is cluster-dependent: on Slurm prefer `MPI_LAUNCH=srun` and `MPI_NP_FLAG=-n`.
  MPI_LAUNCH=${MPI_LAUNCH:-mpirun}
  MPI_NP_FLAG=${MPI_NP_FLAG:--np}

  # pagerank_mpi prints [mpi] time only on rank 0
  MPI_EXTRA_ARGS=${MPI_EXTRA_ARGS:-}
  out=$( ${MPI_LAUNCH} ${MPI_EXTRA_ARGS} ${MPI_NP_FLAG} $p bin/pagerank_mpi "$trimmed" /dev/null 2>&1 )
  sec=$( echo "$out" | grep -oE 'time=[0-9]+\.[0-9]+s' | head -1 | sed 's/time=//;s/s//' )
  if [[ -z "$sec" ]]; then
    echo "[warn] cannot parse runtime for ranks=$p" >&2
    sec="nan"
  fi

  echo "$p,$edges,$sec,$trimmed" >> "$OUT"
done

echo "[bench] wrote $OUT"

