# MPI Ping-Pong (RTT) Bonus Exercise

This folder contains:

- `ping_pong_mpi.c`: MPI ping-pong benchmark (`bytes,rtt_us` CSV output)
- `Makefile`
- `slurm_dardel_pingpong.sh`: Dardel 2-node run script
- `run_school_pingpong.sh`: school cluster 2-rank run script
- `fit_pingpong_model.py`: linear RTT fit + plot

## 1) Dardel run (two compute nodes)

```bash
cd ~/dd2356_a2/Bonus
sbatch slurm_dardel_pingpong.sh
```

Reasoning for Slurm choices:

- `-N 2 -n 2 --ntasks-per-node=1`: force one rank per node for inter-node RTT
- `-p shared`: course-available partition
- `-A edu26.dd2356`: course allocation

Output files:

- `dardel_pingpong_<jobid>.out` (job log)
- `dardel_pingpong_rtt.csv` (data for fitting)

## 2) School cluster run (two ranks)

```bash
cd ~/dd2356_a2/Bonus
chmod +x run_school_pingpong.sh
./run_school_pingpong.sh
```

Output:

- `school_pingpong_rtt.csv`

## 3) Fit RTT model and plot

Linear model:

`RTT_us = alpha + beta * bytes`

Run:

```bash
python3 fit_pingpong_model.py --input dardel_pingpong_rtt.csv --system "Dardel" --output-prefix dardel
python3 fit_pingpong_model.py --input school_pingpong_rtt.csv --system "School cluster" --output-prefix school
```

Generated files:

- `dardel_model.txt`, `dardel_rtt_fit.png`
- `school_model.txt`, `school_rtt_fit.png`

## 4) If data is noisy / not convergent

Increase data density by:

- larger max size (e.g. `8388608`)
- more repeats (e.g. `5000`)
- more warmup (e.g. `100`)

Example:

```bash
mpirun -np 2 ./ping_pong_mpi 8388608 5000 100 > school_pingpong_rtt.csv
```

