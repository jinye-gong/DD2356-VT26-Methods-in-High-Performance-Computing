#!/usr/bin/env bash
set -euo pipefail

# Usage:
# ./run_scaling_shallow.sh [N] [ITER] [schedule] [output_csv] [binary]

N="${1:-500}"
ITER="${2:-1000}"
SCHED="${3:-static}"
OUT_CSV="${4:-scaling_shallow.csv}"
BIN="${5:-./shallow_water_ex3}"

THREADS=(1 2 4 8 16 32 64)

if [[ ! -x "$BIN" ]]; then
  echo "Error: binary not found or not executable: $BIN"
  exit 1
fi

echo "threads,time_sec,speedup,efficiency" > "$OUT_CSV"

T1=""
for t in "${THREADS[@]}"; do
  export OMP_NUM_THREADS="$t"
  output="$("$BIN" "$N" "$ITER" "$SCHED" "output_${SCHED}_${t}.txt")"
  time_sec="$(printf "%s\n" "$output" | awk '/OMP time:/ {print $3}')"

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
