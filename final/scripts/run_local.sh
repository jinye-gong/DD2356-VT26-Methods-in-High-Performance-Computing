#!/usr/bin/env bash
# Smoke test all available binaries on data/sample.edges and verify correctness.
set -euo pipefail
cd "$(dirname "$0")/.."

mkdir -p results
INPUT=${INPUT:-data/sample.edges}

run() {
  local name="$1"; shift
  local bin="$1"; shift
  if [[ ! -x "$bin" ]]; then
    echo "[skip] $name: $bin not built"; return
  fi
  echo "---- $name ----"
  "$@"
}

run serial bin/pagerank_serial \
    bin/pagerank_serial "$INPUT" results/ranks_serial.txt

run omp bin/pagerank_omp \
    bin/pagerank_omp "$INPUT" results/ranks_omp.txt

if [[ -x bin/pagerank_mpi ]]; then
  echo "---- mpi (2 ranks) ----"
  mpirun -np 2 bin/pagerank_mpi "$INPUT" results/ranks_mpi.txt
fi

if [[ -x bin/pagerank_hybrid ]]; then
  echo "---- hybrid (2 ranks x 2 threads) ----"
  OMP_NUM_THREADS=2 mpirun -np 2 bin/pagerank_hybrid "$INPUT" results/ranks_hybrid.txt
fi

if command -v python3 >/dev/null && python3 -c 'import networkx' 2>/dev/null; then
  echo "---- verify against networkx ----"
  for f in results/ranks_*.txt; do
    [[ -f "$f" ]] || continue
    echo ">> $f"
    python3 scripts/verify.py "$INPUT" "$f" || true
  done
else
  echo "[info] install networkx (pip install networkx) to run automatic verification."
fi
