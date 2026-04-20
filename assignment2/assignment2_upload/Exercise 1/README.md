# Exercise 1 README

Goal: run MPI Monte Carlo pi with strong and weak scaling.

## Files

- `pi_mpi.c`: main MPI source code
- `Makefile`: builds `pi_mpi_strong` and `pi_mpi_weak`
- `slurm_strong.sh`, `slurm_weak.sh`: Dardel batch scripts
- `plot_efficiency.py`: computes and plots scaling efficiency

## Build

### School cluster / Local (OpenMPI)

```bash
cd "Exercise 1"
make CC=mpicc pi_mpi_strong pi_mpi_weak
```

### Dardel (Cray MPI wrapper)

```bash
cd ~/dd2356_a2/"Exercise 1"
make clean
make CC=cc pi_mpi_strong pi_mpi_weak
```

## Run

### School cluster interactive runs

```bash
cd "Exercise 1"
for p in 1 2 4 8 16; do
  mpirun -np $p ./pi_mpi_strong 10000000
done

for p in 1 2 4 8 16; do
  mpirun -np $p ./pi_mpi_weak 0
done
```

Note: in weak scaling mode, each rank uses a fixed `1,000,000` tosses, so the CLI argument is ignored.

### Dardel batch runs

```bash
cd ~/dd2356_a2/"Exercise 1"
chmod +x slurm_strong.sh slurm_weak.sh
sbatch slurm_strong.sh
sbatch slurm_weak.sh
```

Check outputs:

```bash
squeue -u $USER
ls -lh dardel_*.out
```

## Plot efficiency

```bash
cd "Exercise 1"
python3 plot_efficiency.py
```

This generates strong/weak scaling efficiency figures (using script default output names).
