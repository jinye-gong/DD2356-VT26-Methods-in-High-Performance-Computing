# Bonus README (MPI Ping-Pong)

Goal: measure RTT vs message size and fit a linear model.

## Files

- `ping_pong_mpi.c`: MPI ping-pong benchmark (CSV output)
- `Makefile`: build rules
- `slurm_dardel_pingpong.sh`: Dardel two-node batch script
- `run_school_pingpong.sh`: school cluster run script
- `fit_pingpong_model.py`: fits `RTT = alpha + beta * bytes` and plots

## Build

### School cluster / Local

```bash
cd Bonus
make clean
make CC=mpicc
```

### Dardel

```bash
cd ~/dd2356_a2/Bonus
make clean
make CC=cc
```

## Run

### School cluster (2 MPI ranks)

```bash
cd Bonus
chmod +x run_school_pingpong.sh
bash run_school_pingpong.sh
```

Output: `school_pingpong_rtt.csv`

### Dardel (2 nodes, 1 rank per node)

```bash
cd ~/dd2356_a2/Bonus
chmod +x slurm_dardel_pingpong.sh
sbatch slurm_dardel_pingpong.sh
```

Check job and outputs:

```bash
squeue -u $USER
ls -lh dardel_pingpong_*.out dardel_pingpong_rtt.csv
```

## Pull Dardel data to local machine

```bash
scp -o GSSAPIAuthentication=yes \
  weiyil@dardel.pdc.kth.se:~/dd2356_a2/Bonus/dardel_pingpong_rtt.csv \
  "Bonus/"
```

## Fit model and generate plots

### School cluster data

```bash
python3 fit_pingpong_model.py \
  --input school_pingpong_rtt.csv \
  --system "School cluster" \
  --output-prefix school
```

### Dardel data

```bash
python3 fit_pingpong_model.py \
  --input dardel_pingpong_rtt.csv \
  --system "Dardel" \
  --output-prefix dardel
```

Outputs:

- `<prefix>_model.txt`
- `<prefix>_rtt_fit.png`
