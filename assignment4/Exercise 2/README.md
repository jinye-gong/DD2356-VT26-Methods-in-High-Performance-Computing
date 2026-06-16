# Exercise 2 - Parallel Row Sum (MPI Collectives)

**目标**：`MPI_Scatter` 分行、`MPI_Gather` 收集行和、`MPI_Reduce` 求矩阵总和 + 弱扩展性。

## 源文件

- `row_sum_mpi.c` — 课程骨架（待加入 MPI 与计时 TODO）

## 编译

```bash
mpicc -O3 -o row_sum_mpi row_sum_mpi.c
```

## 运行

```bash
mpirun -np 4 ./row_sum_mpi
```

输出：`row_sums_output.txt`（rank 0 写入，供 Python 对比串行结果）。

## 弱扩展实验

进程数增加时，总矩阵规模应随进程数成比例增大（每进程行数大致不变）。

```bash
chmod +x run_scaling_weak.sh
./run_scaling_weak.sh row_sum_mpi scaling_weak_school.csv
```

画图：

```bash
python3 ../plot_scaling_mpi.py --csv scaling_weak_school.csv \
  --mode weak --title "Row Sum Ex2 School Weak" --out weak_rowsum_school.png
```

## 可视化

```bash
python3 plot_row_sums.py
```
