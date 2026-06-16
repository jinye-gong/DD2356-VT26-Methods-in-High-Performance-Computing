#!/bin/bash
#SBATCH -A edu26.dd2356
#SBATCH -p main
#SBATCH -J a4-ex3-gol
#SBATCH -t 00:15:00
#SBATCH -N 1
#SBATCH -n 16
#SBATCH -o logs/a4-ex3-%j.out
#SBATCH -e logs/a4-ex3-%j.err

set -euo pipefail
cd "${SLURM_SUBMIT_DIR:?}"
mkdir -p logs

if [[ -d "Exercise 3" ]]; then
  bash "Exercise 3/run_dardel_ex3.sh"
elif [[ -d Exe3 ]]; then
  bash Exe3/run_dardel_ex3.sh
else
  echo "No Exercise 3/ or Exe3/" >&2
  exit 1
fi
