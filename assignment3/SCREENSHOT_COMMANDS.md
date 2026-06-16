# Assignment3 Screenshot Commands

在 `assignment3` 根目录执行。  
每条命令运行后，拍终端窗口（包含命令 + 输出）。

## Exercise 1

### Task1 code
```bash
sed -n '25,60p' "Exercise 1/matmul_ex1_task1.c"
```

### Task1 compile + max threads
```bash
gcc -O3 -fopenmp -march=native "Exercise 1/matmul_ex1_task1.c" -o "Exercise 1/matmul_ex1_task1"
OMP_NUM_THREADS=64 "./Exercise 1/matmul_ex1_task1" 1024
```

### Task2 code
```bash
sed -n '28,62p' "Exercise 1/matmul_ex1_task2_simd.c"
```

### Task2 compile + max threads
```bash
gcc -O3 -fopenmp -march=native "Exercise 1/matmul_ex1_task2_simd.c" -o "Exercise 1/matmul_ex1_task2_simd"
OMP_NUM_THREADS=64 "./Exercise 1/matmul_ex1_task2_simd" 1024
```

### Task3 code
```bash
sed -n '28,66p' "Exercise 1/matmul_ex1_task3_hybrid.c"
```

### Task3 compile + max threads
```bash
gcc -O3 -fopenmp -march=native "Exercise 1/matmul_ex1_task3_hybrid.c" -o "Exercise 1/matmul_ex1_task3_hybrid"
OMP_NUM_THREADS=64 "./Exercise 1/matmul_ex1_task3_hybrid" 1024
```

### Task4 code
```bash
sed -n '30,90p' "Exercise 1/matmul_ex1_task4_gpu.c"
```

### Task4 compile (school cluster offload try)
```bash
gcc -O3 -fopenmp -march=native "Exercise 1/matmul_ex1_task4_gpu.c" -o "Exercise 1/matmul_ex1_task4_gpu"
```

### Task4 fallback compile + max threads (if offload toolchain fails)
```bash
gcc -O3 -fopenmp -march=native -DDISABLE_OFFLOAD "Exercise 1/matmul_ex1_task4_gpu.c" -o "Exercise 1/matmul_ex1_task4_gpu"
OMP_NUM_THREADS=64 "./Exercise 1/matmul_ex1_task4_gpu" 1024
```

## Exercise 2

### Task1 code
```bash
sed -n '75,155p' "Exercise 2/bfs_ex2_task1_parallel.c"
```

### Task1 compile + max threads
```bash
gcc -O3 -fopenmp -march=native "Exercise 2/bfs_ex2_task1_parallel.c" -o "Exercise 2/bfs_ex2_task1_parallel"
OMP_NUM_THREADS=64 "./Exercise 2/bfs_ex2_task1_parallel" 50000 8
```

### Task2 code
```bash
sed -n '80,180p' "Exercise 2/bfs_ex2_task2_task.c"
```

### Task2 compile + max threads
```bash
gcc -O3 -fopenmp -march=native "Exercise 2/bfs_ex2_task2_task.c" -o "Exercise 2/bfs_ex2_task2_task"
OMP_NUM_THREADS=64 "./Exercise 2/bfs_ex2_task2_task" 50000 8
```

### Task3 random graph max threads
```bash
OMP_NUM_THREADS=64 "./Exercise 2/bfs_ex2_task1_parallel" 200000 16
OMP_NUM_THREADS=64 "./Exercise 2/bfs_ex2_task2_task" 200000 16
```

## Exercise 3

### Code
```bash
sed -n '85,150p' "Exercise 3/shallow_water_ex3.c"
```

### Compile
```bash
gcc -O3 -fopenmp -march=native "Exercise 3/shallow_water_ex3.c" -o "Exercise 3/shallow_water_ex3"
```

### Max threads outputs (static/dynamic/guided)
```bash
OMP_NUM_THREADS=64 "./Exercise 3/shallow_water_ex3" 500 1000 static output_static_64.txt
OMP_NUM_THREADS=64 "./Exercise 3/shallow_water_ex3" 500 1000 dynamic output_dynamic_64.txt
OMP_NUM_THREADS=64 "./Exercise 3/shallow_water_ex3" 500 1000 guided output_guided_64.txt
```

### Plot output visualization
```bash
python3 - <<'PY'
import numpy as np
import matplotlib.pyplot as plt
data = np.loadtxt("Exercise 3/output_static_64.txt")
plt.imshow(data, cmap='viridis', origin='lower')
plt.colorbar(label='Water Height')
plt.title('Shallow Water Simulation Result')
plt.show()
PY
```

