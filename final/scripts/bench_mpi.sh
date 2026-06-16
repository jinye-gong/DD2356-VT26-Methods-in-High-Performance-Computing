#!/usr/bin/env bash
# MPI strong-scaling sweep.
# Usage: ./bench_mpi.sh data/web-Google.txt [ranks_list]
set -euo pipefail
cd "$(dirname "$0")/.."

INPUT=${1:-data/sample.edges}
RANKS=${2:-"1 2 4 8"}
OUT=results/strong_mpi.csv

[[ -x bin/pagerank_mpi ]] || { echo "[bench] bin/pagerank_mpi missing - run 'make mpi' first"; exit 1; }

mkdir -p results
echo "ranks,seconds" > "$OUT"
for n in $RANKS; do
  echo "[bench] -np $n"
  # MPI launcher is cluster-dependent: on Slurm prefer `MPI_LAUNCH=srun` and `MPI_NP_FLAG=-n`.
  # On a typical local OpenMPI setup: use defaults (mpirun -np).
  MPI_LAUNCH=${MPI_LAUNCH:-mpirun}
  MPI_NP_FLAG=${MPI_NP_FLAG:--np}
  MPI_EXTRA_ARGS=${MPI_EXTRA_ARGS:-}
  out=$( ${MPI_LAUNCH} ${MPI_EXTRA_ARGS} ${MPI_NP_FLAG} $n bin/pagerank_mpi "$INPUT" /dev/null 2>&1 )
  sec=$( echo "$out" | grep -oE 'time=[0-9]+\.[0-9]+s' | head -1 | sed 's/time=//;s/s//' )
  if [[ -z "$sec" ]]; then
    echo "[warn] cannot parse runtime for ranks=$n" >&2
    sec="nan"
  fi
  echo "$n,$sec" >> "$OUT"
done
echo "[bench] wrote $OUT"
