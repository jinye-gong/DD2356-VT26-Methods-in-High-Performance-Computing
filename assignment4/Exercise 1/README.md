# Exercise 1 - 1D Wave Equation with Halo Exchange

**目标**：一维波动方程域分解 + 点对点 halo 交换 + `MPI_Wtime` 计时 + 强扩展性。

## 源文件

- `wave_mpi.c` — 课程骨架（待完成 MPI 并行与计时 TODO）

## 编译

```bash
# 本地 / 学校集群
mpicc -O3 -o wave_mpi wave_mpi.c -lm

# Dardel 示例（以集群文档为准）
# module load PDC/24.11
# cc -O3 -o wave_mpi wave_mpi.c -lm
```

## 运行

```bash
# 本地
mpirun -np 4 ./wave_mpi

# 学校集群 / Dardel（示例，按你们分区与账号修改）
# srun -n 4 ./wave_mpi
```

正确性验证时可保留输出；测扩展性时建议在代码中关闭或减少 `write_output`。

## 强扩展实验

进程数：`1, 2, 4, 8, 16`（Dardel 注意单节点最多 4 进程时的放置策略）。

```bash
chmod +x run_scaling_strong.sh
./run_scaling_strong.sh wave_mpi scaling_school.csv
```

画图（结果可拷到 `../../Artifacts/plots/school/`）：

```bash
python3 ../plot_scaling_mpi.py --csv scaling_school.csv \
  --mode strong --title "Wave Ex1 School Strong" --out strong_wave_school.png
```

## 可视化

```bash
python3 plot_wave.py
```

需先运行程序生成 `wave_output_*.txt`。
