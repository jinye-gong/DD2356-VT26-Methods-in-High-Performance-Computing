# Assignment 4 Screenshot Commands

在对应目录执行，**终端截图需包含：完整命令 + 输出**。  
保存到 `Artifacts/screenshots/school/` 或 `Artifacts/screenshots/dardel/`。

---

## 练习 1（Wave / halo）— 当前必做

### 学校集群 `~/dd2356_a4/Exe1`（Jinye 截）

| 文件名 | 截什么 |
|--------|--------|
| `ex1_code.png` | 核心 MPI 代码（见下方 sed） |
| `ex1_compile.png` | `mpicc -O3 -o wave_mpi wave_mpi.c -lm` 成功 |
| `ex1_run_np16.png` | 最大规模强扩展单次运行或 scaling CSV |
| `ex1_scaling_csv.png` | `cat scaling_school_ex1.csv` 全部 5 行 |

**1) 代码（halo + 计时）**
```bash
cd ~/dd2356_a4/Exe1
sed -n '45,95p' wave_mpi.c
```

**2) 编译**
```bash
cd ~/dd2356_a4/Exe1
mpicc -O3 -o wave_mpi wave_mpi.c -lm
ls -la wave_mpi
```

**3) 最大进程数运行（强扩展设置，无 I/O）**
```bash
cd ~/dd2356_a4/Exe1
export WAVE_N=200000 WAVE_STEPS=500 WAVE_IO=0
mpirun -np 16 ./wave_mpi
```
输出里应有：`WALLTIME ...` 和 `Simulation complete (N=200000, ...)`

**4) 强扩展 CSV（若已跑过 run_school_ex1.sh）**
```bash
cat scaling_school_ex1.csv
```

**5) 可选：正确性**
```bash
mpirun -np 4 ./wave_mpi 1000 100 1
head -5 wave_output_0.txt
```

---

### Dardel `~/DD2356/assignment4/Exercise 1`（Weiyi / agent 截）

| 文件名 | 截什么 |
|--------|--------|
| `ex1_code.png` | 同左 sed（或 agent 生成） |
| `ex1_compile.png` | `cc -O3 ...` + module 已加载 |
| `ex1_run_np16.png` | `srun -n 16` + WALLTIME |
| `ex1_scaling_csv.png` | `cat scaling_dardel_ex1.csv` |
| `ex1_scorep_summary.png` | Score-P 最大规模 profiling 摘要 |

**编译**
```bash
cd ~/DD2356/assignment4
source scripts/dardel_setup_env.sh
cd "Exercise 1"
cc -O3 -o wave_mpi wave_mpi.c -lm
```

**np=16 运行**
```bash
export WAVE_N=200000 WAVE_STEPS=500 WAVE_IO=0
srun -N 4 --ntasks=16 --ntasks-per-node=4 ./wave_mpi
```

**CSV**
```bash
cat scaling_dardel_ex1.csv
```

---

## 练习 2 — 学校集群 `~/dd2356_a4/Exe2`

| 文件 | 截什么 |
|------|--------|
| `ex2_code.png` | `sed -n '95,150p' row_sum_mpi.c`（Scatterv / Gatherv / Reduce + WALLTIME） |
| `ex2_compile.png` | `mpicc -O3 -o row_sum_mpi row_sum_mpi.c` + `ls -la row_sum_mpi` |
| `ex2_run_np16.png` | `export WEAK_N=16000 ROWSUM_IO=0` + `mpirun -np 16 ./row_sum_mpi` |
| `ex2_scaling_csv.png` | `cat scaling_weak_school_ex2.csv` |

**一键跑：** `cd ~/dd2356_a4/Exe2 && ./run_school_ex2.sh`

## 练习 3 — 学校集群 `~/dd2356_a4/Exe3`

| 文件 | 截什么 |
|------|--------|
| `ex3_code.png` | `sed -n '120,200p' gol_mpi.c`（Isend/Irecv + Waitall） |
| `ex3_compile.png` | `mpicc -O3 -o gol_mpi gol_mpi.c` + `ls -la gol_mpi` |
| `ex3_run_np16.png` | `export GOL_N=2000 GOL_STEPS=500 GOL_IO=0` + `mpirun -np 16 ./gol_mpi` |
| `ex3_scaling_csv.png` | `cat efficiency_school_ex3.csv` |

**一键跑：** `cd ~/dd2356_a4/Exe3 && ./run_school_ex3.sh`  

Dardel 练习 2：再加 **`ex2_scorep_summary.png`**（np=16）

---

## 报告里怎么写

在 PDF 中每张图下注明：**平台、编译命令、运行命令、进程数**。
