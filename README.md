# DD2356 VT26 - Methods in High Performance Computing

Coursework repository for **DD2356 (VT26)** at KTH. Contains source code, run scripts, experiment results, figures, and reports for Assignments 2–4.

## Repository layout

| Directory | Topic | Main technologies |
|-----------|-------|-------------------|
| `assignment2/` | Performance measurement & roofline | MPI, `perf`, roofline model |
| `assignment3/` | Shared-memory programming | OpenMP (parallel, SIMD, tasks, GPU) |
| `assignment4/` | Distributed-memory programming | MPI (point-to-point, collectives, non-blocking) |

Each assignment follows a similar structure:

```text
assignmentN/
├── Exercise 1/ … Exercise 3/   # source (.c), scripts, per-exercise README
├── Bonus/                      # optional (assignment2 & 3)
├── Artifacts/
│   ├── results/{local,school,dardel}/   # scaling / timing CSV
│   ├── plots/{local,school,dardel}/     # generated PNG figures
│   └── screenshots/                     # code / compile / run captures
├── AssignmentN_Report.md       # report (markdown)
└── README.md                   # assignment-level instructions
```

See also `assignment3/DIRECTORY_MAP.md` and `assignment4/DIRECTORY_MAP.md` for detailed folder maps.

## Assignment summaries

### Assignment 2 — Measurement & roofline

- **Exercise 1**: MPI π integration, strong/weak scaling
- **Exercise 2**: Roofline model (local, school cluster, Dardel)
- **Exercise 3**: SpMV benchmark
- **Exercise 4**: Matrix multiply & `perf` analysis
- **Bonus**: MPI ping-pong latency / RTT model

Report: `assignment2/Assignment2_Report.md` (PDF included).

### Assignment 3 — OpenMP

- **Exercise 1**: Matrix multiplication (baseline, SIMD, hybrid, GPU)
- **Exercise 2**: BFS (parallel for, tasks)
- **Exercise 3**: Shallow-water simulation (static / guided / dynamic scheduling)
- **Bonus**: Neuron firing model with OpenMP tasks

Report: `assignment3/Assignment3_Report.md`.

Typical build:

```bash
cd "assignment3/Exercise 1"
gcc -O3 -fopenmp -march=native -o app matmul_ex1_task1.c
OMP_NUM_THREADS=8 ./app
```

### Assignment 4 — MPI

- **Exercise 1**: 1D wave equation + halo exchange (strong scaling)
- **Exercise 2**: Matrix row sums + collectives (weak scaling)
- **Exercise 3**: Game of Life + 2D non-blocking ghost exchange (efficiency)

Reports: `assignment4/Assignment4_Report.md`, `assignment4/Assignment4_Report_zh.md`, PDF.

Typical build:

```bash
cd "assignment4/Exercise 1"
mpicc -O3 -o wave wave_mpi.c -lm
mpirun -np 4 ./wave
```

Cluster jobs: see `assignment4/scripts/` (SLURM) and per-exercise `README.md`.

## Environment

| Component | Assignments |
|-----------|-------------|
| `gcc` / `clang` + OpenMP (`-fopenmp`) | 3 |
| `mpicc` / `mpirun` (Open MPI or MPICH) | 2, 4 |
| Python 3 + `numpy`, `matplotlib` | 2–4 (plotting) |
| Dardel / school cluster (`module load`, `srun`) | 2–4 |

## Git & ignored files

The following are **not** tracked (see `.gitignore`):

- Compiled binaries (`*.o`, `*.out`, `a.out`, exercise build targets)
- Upload archives (`assignment2_upload.zip`, `assignment3.zip`, `assignment3/*.tar.gz`)
- Large regenerated outputs (`assignment3/Exercise 3/output_*.txt`, bonus neuron dumps)
- Editor / Python cache (`.vscode/`, `__pycache__/`, …)

Source code, scripts, CSV results, plots, screenshots, and reports are kept under version control.

---

# DD2356 VT26 - 高性能计算方法

KTH 课程 **DD2356 (VT26)** 的作业仓库，包含 Assignment 2–4 的源代码、运行脚本、实验结果、图表与报告。

## 仓库结构

| 目录 | 主题 | 主要技术 |
|------|------|----------|
| `assignment2/` | 性能测量与 Roofline 模型 | MPI、`perf`、Roofline |
| `assignment3/` | 共享内存编程 | OpenMP（并行、SIMD、Task、GPU） |
| `assignment4/` | 分布式内存编程 | MPI（点对点、集合通信、非阻塞） |

各作业目录结构类似：

```text
assignmentN/
├── Exercise 1/ … Exercise 3/   # 源码 (.c)、脚本、各练习 README
├── Bonus/                      # 加分题（assignment2 & 3）
├── Artifacts/
│   ├── results/{local,school,dardel}/   # 扩展性 / 计时 CSV
│   ├── plots/{local,school,dardel}/     # 生成的 PNG 图
│   └── screenshots/                     # 代码 / 编译 / 运行截图
├── AssignmentN_Report.md       # 报告（Markdown）
└── README.md                   # 作业级说明
```

更详细的目录说明见 `assignment3/DIRECTORY_MAP.md` 与 `assignment4/DIRECTORY_MAP.md`。

## 各作业概要

### Assignment 2 — 性能测量与 Roofline

- **练习 1**：MPI 求 π，强/弱扩展
- **练习 2**：Roofline 模型（本地、学校集群、Dardel）
- **练习 3**：SpMV 基准测试
- **练习 4**：矩阵乘法与 `perf` 分析
- **加分题**：MPI Ping-Pong 延迟 / RTT 模型

报告：`assignment2/Assignment2_Report.md`（含 PDF）。

### Assignment 3 — OpenMP

- **练习 1**：矩阵乘法（基线、SIMD、混合、GPU）
- **练习 2**：BFS（`parallel for`、Task）
- **练习 3**：浅水方程模拟（static / guided / dynamic 调度）
- **加分题**：神经元放电模型（OpenMP Task）

报告：`assignment3/Assignment3_Report.md`。

典型编译与运行：

```bash
cd "assignment3/Exercise 1"
gcc -O3 -fopenmp -march=native -o app matmul_ex1_task1.c
OMP_NUM_THREADS=8 ./app
```

### Assignment 4 — MPI

- **练习 1**：一维波动方程 + Halo 交换（强扩展）
- **练习 2**：矩阵行和 + 集合通信（弱扩展）
- **练习 3**：生命游戏 + 二维非阻塞 Ghost 交换（并行效率）

报告：`assignment4/Assignment4_Report.md`、`assignment4/Assignment4_Report_zh.md`、PDF。

典型编译与运行：

```bash
cd "assignment4/Exercise 1"
mpicc -O3 -o wave wave_mpi.c -lm
mpirun -np 4 ./wave
```

集群作业：见 `assignment4/scripts/`（SLURM）及各练习 `README.md`。

## 环境要求

| 组件 | 适用作业 |
|------|----------|
| `gcc` / `clang` + OpenMP（`-fopenmp`） | 3 |
| `mpicc` / `mpirun`（Open MPI 或 MPICH） | 2、4 |
| Python 3 + `numpy`、`matplotlib` | 2–4（画图） |
| Dardel / 学校集群（`module load`、`srun`） | 2–4 |

## Git 与忽略文件

以下内容**不纳入**版本控制（见 `.gitignore`）：

- 编译产物（`*.o`、`*.out`、`a.out`、练习生成的可执行文件）
- 上交用压缩包（`assignment2_upload.zip`、`assignment3.zip`、`assignment3/*.tar.gz`）
- 大体积可重新生成的输出（`assignment3/Exercise 3/output_*.txt`、Bonus 神经元输出等）
- 编辑器 / Python 缓存（`.vscode/`、`__pycache__/` 等）

源代码、脚本、CSV 结果、图表、截图与报告均保留在 Git 中。
