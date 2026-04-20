#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"

gcc -g -O2 matrix_multiply.c -o matrix_multiply.out

echo "=== Naive (i-j-k) ==="
perf stat -e \
cycles,instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses \
./matrix_multiply.out

echo
echo "=== Optimized (i-k-j) ==="
perf stat -e \
cycles,instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses \
./matrix_multiply.out opt

cat <<'EOF'

Use these formulas in the report:
  IPC = instructions / cycles
  L1 miss rate  ~= L1-dcache-load-misses / L1-dcache-loads
  LLC miss rate ~= LLC-load-misses / LLC-loads

EOF

