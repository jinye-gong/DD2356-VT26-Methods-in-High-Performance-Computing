#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ./run_scaling.sh [matrix_size] [output_csv] [binary]
# Example:
#   ./run_scaling.sh 1024 scaling_local.csv ./matmul_ex1_task1

N="${1:-1024}"
OUT_CSV="${2:-scaling_local.csv}"
BIN="${3:-./matmul_ex1_task1}"

THREADS=(1 2 4 8 16 32 64)

if [[ ! -x "$BIN" ]]; then
  echo "Error: binary not found or not executable: $BIN"
  echo "Compile first, for example:"
  echo "  gcc -O3 -march=native -fopenmp matmul_ex1_task1.c -o matmul_ex1_task1"
  exit 1
fi

echo "threads,time_sec,speedup,efficiency" > "$OUT_CSV"

T1=""
for t in "${THREADS[@]}"; do
  export OMP_NUM_THREADS="$t"

  # Run once and parse "OMP time" from program output.
  output="$("$BIN" "$N")"
  time_sec="$(printf "%s\n" "$output" | awk '/OMP time:/ {print $3}')"

  # Skip thread count if runtime could not be parsed (e.g., unsupported high thread count).
  if [[ -z "$time_sec" ]]; then
    echo "Warning: failed to parse runtime for threads=$t, skipping"
    continue
  fi

  if [[ -z "$T1" ]]; then
    T1="$time_sec"
  fi

  speedup="$(awk -v t1="$T1" -v tp="$time_sec" 'BEGIN { printf "%.6f", t1/tp }')"
  efficiency="$(awk -v s="$speedup" -v p="$t" 'BEGIN { printf "%.6f", s/p }')"

  echo "$t,$time_sec,$speedup,$efficiency" >> "$OUT_CSV"
  echo "threads=$t time=${time_sec}s speedup=${speedup}x efficiency=${efficiency}"
done

echo "Saved: $OUT_CSV"
