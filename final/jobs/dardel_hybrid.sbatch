#!/bin/bash
#SBATCH -J pr-hybrid
#SBATCH -A edu26.dd2356
#SBATCH -p shared
#SBATCH -N 2
#SBATCH --ntasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --hint=nomultithread
#SBATCH -t 00:35:00
#SBATCH -o logs/%x-%j.out
#SBATCH -e logs/%x-%j.err

set -euo pipefail

PROJECT_ROOT=${PROJECT_ROOT:-/cfs/klemming/home/w/weiyil/DD2356/final}
INPUT=${INPUT:-data/web-Google.txt}
TOTAL_CORES=${TOTAL_CORES:-256}
RANKS_LIST=${RANKS_LIST:-"1 2 4 8 16 32 64 128 256"}

mkdir -p "$PROJECT_ROOT/logs" "$PROJECT_ROOT/results"
cd "$PROJECT_ROOT"

module load PDC cpe/23.12 cray-mpich

export MPI_LAUNCH=srun
export MPI_NP_FLAG=-n
export OMP_PROC_BIND=close
export OMP_PLACES=cores

./scripts/bench_hybrid.sh "$INPUT" "$TOTAL_CORES" "$RANKS_LIST"
cp results/hybrid_sweep.csv "results/hybrid_dardel_${SLURM_JOB_ID}.csv"

echo "[done] results/hybrid_dardel_${SLURM_JOB_ID}.csv"
