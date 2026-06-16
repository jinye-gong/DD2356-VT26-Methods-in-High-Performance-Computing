# DD2356 Assignment 3 - Shared Memory Programming

本目录包含 Assignment 3 的代码与实验说明（OpenMP）。

## 1. 环境要求

- C 编译器：`gcc` (推荐 >= 10) 或 `clang`（支持 OpenMP）
- OpenMP：编译时开启 `-fopenmp`
- Python（用于画图）：`python3`, `numpy`, `matplotlib`

## 2. 目录建议

建议按以下结构组织（可按你们实际情况调整）：

```text
assignment3/
├── ex1_matmul/
│   ├── src/
│   ├── data/
│   ├── scripts/
│   └── README.md
├── ex2_bfs/
│   ├── src/
│   ├── data/
│   ├── scripts/
│   └── README.md
├── ex3_shallow_water/
│   ├── src/
│   ├── scripts/
│   └── README.md
└── README.md
```

## 3. 通用编译与运行方式

如果没有 Makefile，可以用：

```bash
gcc -O3 -fopenmp -march=native -o app main.c
./app
```

如果有 Makefile：

```bash
make
./app
```

线程数控制：

```bash
export OMP_NUM_THREADS=1
./app

export OMP_NUM_THREADS=2
./app
```

建议测试线程数序列：`1, 2, 4, 8, 16, 32, ...`，直到性能不再提升。

## 4. 练习 1：矩阵乘法（MatMul）

需要实现的函数：

- `matmul_omp_parallel()`
- `matmul_omp_simd()`
- `matmul_omp_hybrid()`
- `matmul_omp_gpu()`（仅学校集群）

建议运行方式（示例）：

```bash
./matmul --impl serial --n 2048
./matmul --impl omp_parallel --n 2048
./matmul --impl omp_simd --n 2048
./matmul --impl omp_hybrid --n 2048
./matmul --impl omp_gpu --n 2048
```

需要记录：

- 每个线程数下的运行时间（秒）
- 最多线程时的输出截图
- 强扩展曲线（实测）+ 理想扩展（虚线）
- Dardel / 学校集群 / 本地 结果对比分析

## 5. 练习 2：BFS

需要实现的函数：

- `bfs_omp_parallel()`
- `bfs_omp_task()`

建议运行方式（示例）：

```bash
./bfs --impl serial --graph input.graph --source 0
./bfs --impl omp_parallel --graph input.graph --source 0
./bfs --impl omp_task --graph input.graph --source 0
```

还需要在随机图上重复扩展性实验（Dardel + 学校集群）。

需要记录：

- 图表示方式（CSR / 邻接表等）
- 线程间工作划分策略
- 共享数据结构处理（同步方式）
- 3 种优化尝试（有效或无效都可）
- 强扩展图与结果分析

## 6. 练习 3：浅水模拟

需要完成：

- 使用 `#pragma omp parallel for collapse(2)` 并行化嵌套循环
- 尝试调度策略：`static` / `dynamic` / `guided`
- 用 `omp_get_wtime()` 计时

示例编译与运行：

```bash
gcc -O3 -fopenmp -o shallow shallow_water.c

export OMP_NUM_THREADS=1
./shallow

export OMP_NUM_THREADS=16
./shallow
```

Python 可视化：

```bash
python3 plot_output.py
```

其中 `plot_output.py` 示例：

```python
import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("output.txt")
plt.imshow(data, cmap="viridis", origin="lower")
plt.colorbar(label="Water Height")
plt.title("Shallow Water Simulation Result")
plt.show()
```

## 7. 结果记录模板（建议）

建议每个实验保存为 CSV，例如：

```text
threads,time_sec,speedup,efficiency
1,10.00,1.00,1.00
2,5.40,1.85,0.93
4,2.90,3.45,0.86
...
```

并统一放在：

- `ex1_matmul/results/`
- `ex2_bfs/results/`
- `ex3_shallow_water/results/`

## 8. 提交说明

请提交：

1. `report.pdf`
2. `assignment3_code.zip`

报告建议包含：

- 实现方法与关键代码说明
- 每题实验设置与图表（强扩展+理想扩展）
- 不同平台结果对比分析
- 每位组员贡献分工

代码压缩包建议包含：

- 所有源代码
- 每道题对应 `README.md`（编译/运行方法）
- 实验脚本与绘图脚本
- 结果数据（CSV）与必要截图
