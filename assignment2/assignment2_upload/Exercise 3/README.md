# Exercise 3 README

Goal: run SpMV (penta-diagonal matrix) and collect time and GFLOP/s.

## Files

- `spmv.c`: main SpMV program
- `run_spmv_bench.sh`: batch runner for multiple problem sizes

## Build

```bash
cd "Exercise 3"
gcc -O3 -march=native -o spmv spmv.c
```

## Run (manual)

The program uses exponent-style arguments: `100 -> 10^4`, `1000 -> 10^6`, `10000 -> 10^8`.

```bash
cd "Exercise 3"
taskset -c 0 ./spmv 100
taskset -c 0 ./spmv 1000
taskset -c 0 ./spmv 10000
```

## Run (script)

```bash
cd "Exercise 3"
chmod +x run_spmv_bench.sh
./run_spmv_bench.sh
```

The script compiles and runs all three sizes and prints data for report tables.

## Dardel example

```bash
cd ~/dd2356_a2/"Exercise 3"
srun -A edu26.dd2356 -p shared -N1 -n1 -c1 --cpu-bind=cores --mem=20G \
  gcc -O3 -march=native -o spmv spmv.c

srun -A edu26.dd2356 -p shared -N1 -n1 -c1 --cpu-bind=cores --mem=20G \
  taskset -c 0 ./spmv 10000
```
