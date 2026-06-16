# Exercise 1 - Parallel Matrix-Matrix Multiplication

This folder contains four OpenMP implementations:

- `matmul_ex1_task1.c` - `matmul_omp_parallel()` (`parallel for`)
- `matmul_ex1_task2_simd.c` - `matmul_omp_simd()` (`simd`)
- `matmul_ex1_task3_hybrid.c` - `matmul_omp_hybrid()` (`parallel + simd`)
- `matmul_ex1_task4_gpu.c` - `matmul_omp_gpu()` (`target offload`, fallback supported)

## Build

```bash
gcc -O3 -fopenmp -march=native matmul_ex1_task1.c -o matmul_ex1_task1
gcc -O3 -fopenmp -march=native matmul_ex1_task2_simd.c -o matmul_ex1_task2_simd
gcc -O3 -fopenmp -march=native matmul_ex1_task3_hybrid.c -o matmul_ex1_task3_hybrid
gcc -O3 -fopenmp -march=native matmul_ex1_task4_gpu.c -o matmul_ex1_task4_gpu
```

If Task4 offload toolchain is unavailable:

```bash
gcc -O3 -fopenmp -march=native -DDISABLE_OFFLOAD matmul_ex1_task4_gpu.c -o matmul_ex1_task4_gpu
```

## Run single test

```bash
OMP_NUM_THREADS=64 ./matmul_ex1_task1 1024
OMP_NUM_THREADS=64 ./matmul_ex1_task2_simd 1024
OMP_NUM_THREADS=64 ./matmul_ex1_task3_hybrid 1024
OMP_NUM_THREADS=64 ./matmul_ex1_task4_gpu 1024
```

## Strong scaling

Use provided script:

```bash
chmod +x run_scaling.sh
./run_scaling.sh 1024 scaling_school.csv ./matmul_ex1_task1
./run_scaling.sh 1024 scaling_school_task2_simd.csv ./matmul_ex1_task2_simd
./run_scaling.sh 1024 scaling_school_task3_hybrid.csv ./matmul_ex1_task3_hybrid
./run_scaling.sh 1024 scaling_school_task4_gpu.csv ./matmul_ex1_task4_gpu
```

Plot:

```bash
python3 ../plot_scaling.py --csv scaling_school.csv --title "MatMul Task1 School" --out strong_scaling_school.png
python3 ../plot_scaling.py --csv scaling_school_task2_simd.csv --title "MatMul Task2 SIMD School" --out strong_scaling_school_task2_simd.png
python3 ../plot_scaling.py --csv scaling_school_task3_hybrid.csv --title "MatMul Task3 Hybrid School" --out strong_scaling_school_task3_hybrid.png
python3 ../plot_scaling.py --csv scaling_school_task4_gpu.csv --title "MatMul Task4 School" --out strong_scaling_school_task4_gpu.png
```
