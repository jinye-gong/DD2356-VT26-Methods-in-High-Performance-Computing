#!/bin/bash
#SBATCH -A edu26.dd2356
#SBATCH -p main
#SBATCH -J a4-ex1-wave
#SBATCH -t 00:20:00
#SBATCH -N 4
#SBATCH --ntasks-per-node=4
#SBATCH -o logs/a4-ex1-%j.out
#SBATCH -e logs/a4-ex1-%j.err

set -euo pipefail
cd "${SLURM_SUBMIT_DIR:?}"
mkdir -p logs
source scripts/dardel_setup_env.sh
if [[ -d Exercise\ 1 ]]; then
  bash Exercise\ 1/run_dardel_ex1.sh
elif [[ -d Exe1 ]]; then
  bash Exe1/run_dardel_ex1.sh
else
  echo "No Exercise 1/ or Exe1/ found" >&2
  exit 1
fi
