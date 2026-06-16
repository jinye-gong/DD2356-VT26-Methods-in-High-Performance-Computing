#!/bin/bash
#SBATCH -J pr-build
#SBATCH -A edu26.dd2356
#SBATCH -p shared
#SBATCH -N 1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH -t 00:10:00
#SBATCH -o logs/%x-%j.out
#SBATCH -e logs/%x-%j.err

set -euo pipefail

PROJECT_ROOT=${PROJECT_ROOT:-/cfs/klemming/home/w/weiyil/DD2356/final}
mkdir -p "$PROJECT_ROOT/logs" "$PROJECT_ROOT/results"
cd "$PROJECT_ROOT"

module load PDC cpe/23.12 cray-mpich

make clean
make all CC=cc MPICC=cc CFLAGS_EXTRA="-O3 -march=znver2"

echo "[done] binaries in $PROJECT_ROOT/bin"
