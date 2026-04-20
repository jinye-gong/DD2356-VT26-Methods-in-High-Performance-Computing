#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
gcc -O3 -march=native -o spmv spmv.c

python3 - <<'PY'
import re
import subprocess

print("n,nrows,nnz,time_s,gflops")
for n in (100, 1000, 10000):
    out = subprocess.check_output(["./spmv", str(n)], text=True).strip()
    m = re.search(r"nrows=(\d+), nnz=(\d+), T = ([0-9.]+)", out)
    if not m:
        raise RuntimeError(f"Cannot parse output: {out}")
    nrows = int(m.group(1))
    nnz = int(m.group(2))
    t = float(m.group(3))
    gflops = (2.0 * nnz) / t / 1e9
    print(f"{n},{nrows},{nnz},{t:.6f},{gflops:.6f}")
PY

