# Exercise 3 - Game of Life (2D, Non-blocking Ghost Exchange)

**目标**：2D 域分解 + `MPI_Isend`/`MPI_Irecv` 幽灵格交换 + Conway 规则 + 效率分析。

## 源文件

- `gol_mpi.c` — 课程骨架（待完成 2D 分解与非阻塞通信）

## 编译

```bash
mpicc -O3 -o gol_mpi gol_mpi.c
```

## 运行

```bash
mpirun -np 4 ./gol_mpi
```

输出：`gol_output_<step>.txt`（每 10 步，可在测性能时关闭）。

## 效率 / 扩展（学校集群）

增大 `N` 与 `STEPS`，关闭 I/O 后测试不同进程数：

```bash
chmod +x run_efficiency.sh
./run_efficiency.sh gol_mpi efficiency_school.csv
```

## 可视化

```bash
python3 plot_gol.py
```
