#!/bin/bash
#SBATCH -A edu26.dd2356
#SBATCH -p main
#SBATCH -J a4-ex1-scorep
#SBATCH -t 00:25:00
#SBATCH -N 4
#SBATCH --ntasks-per-node=4
#SBATCH -o logs/a4-ex1-scorep-%j.out
#SBATCH -e logs/a4-ex1-scorep-%j.err

set -euo pipefail
cd "${SLURM_SUBMIT_DIR:?}"
mkdir -p logs "Exercise 1/scorep_run"
cd "Exercise 1/scorep_run"

export WAVE_N=200000
export WAVE_STEPS=100
export WAVE_IO=0
export SCOREP_TOTAL_MEMORY=1200M
export SCOREP_ENABLE_PROFILING=true
export SCOREP_METRIC_EXCLUDE=.*

export SCOREP_METRIC_EXCLUDE=

echo "=== Module load Score-P ==="
module purge --force 2>/dev/null || true
module load PDCOLD/21.11
module load Score-P/7.0-cpeGNU-21.11
module list 2>&1
which scorep
scorep-info config 2>&1 | head -20

echo "=== Instrumented build ==="
scorep --mpp=mpi --thread=none cc -O3 -o wave_mpi_scorep ../wave_mpi.c -lm

echo "=== Run np=16 with Score-P ==="
rm -rf scorep-measurement
export SCOREP_EXPERIMENT_DIRECTORY=scorep-measurement
srun -n 16 ./wave_mpi_scorep

echo "=== Score summary ==="
scorep-score -r scorep-measurement/profile.cubex 2>&1 | tee scorep_summary.txt | head -80
