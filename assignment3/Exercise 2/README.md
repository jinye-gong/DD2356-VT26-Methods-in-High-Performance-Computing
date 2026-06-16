# Exercise 2 - Parallel Breadth-First Search (BFS)

This folder contains:

- `bfs_ex2_task1_parallel.c` - `bfs_omp_parallel()` (fork-join style)
- `bfs_ex2_task2_task.c` - `bfs_omp_task()` (task model)
- `run_scaling_bfs.sh` - scaling script with random graph parameters

## Build

```bash
gcc -O3 -fopenmp -march=native bfs_ex2_task1_parallel.c -o bfs_ex2_task1_parallel
gcc -O3 -fopenmp -march=native bfs_ex2_task2_task.c -o bfs_ex2_task2_task
```

## Run single test

```bash
OMP_NUM_THREADS=64 ./bfs_ex2_task1_parallel 50000 8
OMP_NUM_THREADS=64 ./bfs_ex2_task2_task 50000 8
```

Arguments are:

```text
./bfs_ex2_task*_... [num_vertices] [average_degree]
```

## Strong scaling (Task 1 and Task 2)

```bash
chmod +x run_scaling_bfs.sh
./run_scaling_bfs.sh 50000 8 scaling_school_ex2_task1_bfs.csv ./bfs_ex2_task1_parallel
./run_scaling_bfs.sh 50000 8 scaling_school_ex2_task2_bfs_task.csv ./bfs_ex2_task2_task
```

Plot:

```bash
python3 ../plot_scaling.py --csv scaling_school_ex2_task1_bfs.csv --title "BFS Task1 School" --out strong_scaling_school_ex2_task1_bfs.png
python3 ../plot_scaling.py --csv scaling_school_ex2_task2_bfs_task.csv --title "BFS Task2 Task School" --out strong_scaling_school_ex2_task2_bfs_task.png
```

## Random graph re-run (Task 3)

```bash
./run_scaling_bfs.sh 200000 16 scaling_school_ex2_task3_bfs_parallel.csv ./bfs_ex2_task1_parallel
./run_scaling_bfs.sh 200000 16 scaling_school_ex2_task3_bfs_task.csv ./bfs_ex2_task2_task
```

Plot:

```bash
python3 ../plot_scaling.py --csv scaling_school_ex2_task3_bfs_parallel.csv --title "BFS Task3 Parallel School" --out strong_scaling_school_ex2_task3_bfs_parallel.png
python3 ../plot_scaling.py --csv scaling_school_ex2_task3_bfs_task.csv --title "BFS Task3 Task School" --out strong_scaling_school_ex2_task3_bfs_task.png
```
