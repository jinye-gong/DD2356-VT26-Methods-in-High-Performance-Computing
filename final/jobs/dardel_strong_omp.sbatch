#!/bin/bash
#SBATCH -J pr-omp-strong
#SBATCH -A edu26.dd2356
#SBATCH -p shared
#SBATCH -N 1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=128
#SBATCH --hint=nomultithread
#SBATCH -t 00:20:00
#SBATCH -o logs/%x-%j.out
#SBATCH -e logs/%x-%j.err

set -euo pipefail

PROJECT_ROOT=${PROJECT_ROOT:-/cfs/klemming/home/w/weiyil/DD2356/final}
INPUT=${INPUT:-data/web-Google.txt}
THREADS=${THREADS:-"1 2 4 8 16 32 64 128"}

mkdir -p "$PROJECT_ROOT/logs" "$PROJECT_ROOT/results"
cd "$PROJECT_ROOT"

module load PDC cpe/23.12 cray-mpich

export OMP_PROC_BIND=close
export OMP_PLACES=cores

./scripts/bench_strong.sh "$INPUT" "$THREADS"
cp results/strong_omp.csv "results/strong_omp_dardel_${SLURM_JOB_ID}.csv"

python3 scripts/plot.py results/strong_omp.csv "results/strong_omp_dardel_${SLURM_JOB_ID}.png" || true

echo "[done] results/strong_omp_dardel_${SLURM_JOB_ID}.csv"
