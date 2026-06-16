#!/bin/bash
#SBATCH -J pr-mpi-strong
#SBATCH -A edu26.dd2356
#SBATCH -p shared
#SBATCH -N 4
#SBATCH --ntasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --hint=nomultithread
#SBATCH -t 00:30:00
#SBATCH -o logs/%x-%j.out
#SBATCH -e logs/%x-%j.err

set -euo pipefail

PROJECT_ROOT=${PROJECT_ROOT:-/cfs/klemming/home/w/weiyil/DD2356/final}
INPUT=${INPUT:-data/soc-LiveJournal1.txt}
RANKS=${RANKS:-"128 256 384 512"}

mkdir -p "$PROJECT_ROOT/logs" "$PROJECT_ROOT/results"
cd "$PROJECT_ROOT"

module load PDC cpe/23.12 cray-mpich

export MPI_LAUNCH=srun
export MPI_NP_FLAG=-n

./scripts/bench_mpi.sh "$INPUT" "$RANKS"
cp results/strong_mpi.csv "results/strong_mpi_dardel_${SLURM_JOB_ID}.csv"
python3 scripts/plot.py results/strong_mpi.csv "results/strong_mpi_dardel_${SLURM_JOB_ID}.png" || true

echo "[done] results/strong_mpi_dardel_${SLURM_JOB_ID}.csv"
