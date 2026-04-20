# Exercise 4 README

Goal: profile matrix multiply and transpose with `perf` and compare optimizations.

## Files

- `matrix_multiply.c`: naive and optimized (loop-reordered) implementations
- `transpose.c`: baseline and blocked transpose implementations
- `run_perf_mm.sh`: batch script for matrix multiply profiling
- `run_perf_transpose.sh`: batch script for transpose profiling

## 1) Matrix Multiply

### Build (assignment baseline command)

```bash
cd "Exercise 4"
gcc -g -O2 matrix_multiply.c -o matrix_multiply.out
```

### Run

```bash
./matrix_multiply.out naive
./matrix_multiply.out opt
```

### perf example

```bash
perf stat -e cycles,instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses \
  ./matrix_multiply.out naive
```

## 2) Transpose

### Build

```bash
cd "Exercise 4"
gcc -O2 -o transpose.out transpose.c
```

### Run examples

```bash
./transpose.out 64
./transpose.out 128
./transpose.out 2048
./transpose.out 2048 blocked 32
```

### perf example

```bash
perf stat -e cycles,instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses \
  ./transpose.out 2048
```

## One-click scripts

```bash
cd "Exercise 4"
chmod +x run_perf_mm.sh run_perf_transpose.sh
./run_perf_mm.sh
./run_perf_transpose.sh
```

If you get `perf` permission errors, temporarily set:

```bash
sudo sysctl -w kernel.perf_event_paranoid=1
```
