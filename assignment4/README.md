# DD2356 Assignment 4 - Distributed Memory Programming (MPI)

本目录为 Assignment 4 代码与实验材料（MPI 点对点、集合通信、非阻塞通信）。

## 1. 环境要求

- MPI 实现：Open MPI 或 MPICH（集群上常用 `module load` 后使用 `mpicc` / `mpirun`）
- C 编译器：与 MPI 配套的 `mpicc`
- Python（可视化 / 画图）：`python3`, `numpy`, `matplotlib`（扩展性 CSV 可选 `pandas`）

## 2. 目录结构

```text
assignment4/
├── Exercise 1/          # 一维波动方程 + halo 交换（强扩展）
├── Exercise 2/          # 矩阵行和 + MPI 集合通信（弱扩展）
├── Exercise 3/          # 生命游戏 + 2D 非阻塞 ghost 交换
├── Artifacts/
│   ├── results/{local,school,dardel}/
│   └── plots/{local,school,dardel}/
├── plot_scaling_mpi.py  # 强/弱扩展曲线（根目录）
├── README.md
└── DIRECTORY_MAP.md
```

## 3. 通用编译与运行

```bash
cd "Exercise 1"   # 或 2、3
mpicc -O3 -o app wave_mpi.c -lm    # 练习 1 需要 -lm
mpirun -np 4 ./app
```

学校集群 / Dardel 上请改用作业要求的模块与启动命令（如 `srun`），详见各练习 `README.md`。

## 4. 三题对应关系

| 练习 | 目录 | 源文件 | 扩展性 |
|------|------|--------|--------|
| 1 | `Exercise 1/` | `wave_mpi.c` | 强扩展（固定 N） |
| 2 | `Exercise 2/` | `row_sum_mpi.c` | 弱扩展（随进程数增大问题规模） |
| 3 | `Exercise 3/` | `gol_mpi.c` | 并行效率（学校集群） |

## 5. 本地 vs 集群

- **本地**：开发、查错、小规模正确性验证（`mpirun -np 1..4`）。
- **学校集群 / Dardel**：报告中的正式计时、扩展性图、Dardel Score-P 分析。

测扩展性时建议关闭或降低 I/O 频率，并适当增大 `N` 与步数。

## 6. 提交说明

请提交：

1. `report.pdf`（含策略说明、串行/并行对比图、扩展性分析、组员分工、集群编译运行命令）
2. `assignment4_code.zip`（全部源码 + 每题 `README.md` + 脚本）

结果 CSV 建议放入 `Artifacts/results/<platform>/`。
