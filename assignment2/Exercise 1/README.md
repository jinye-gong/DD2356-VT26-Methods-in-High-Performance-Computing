# Exercise 1 README

本题目标：运行 MPI Monte Carlo pi 的强扩展与弱扩展测试。

## 文件说明

- `pi_mpi.c`: 主程序
- `Makefile`: 生成 `pi_mpi_strong` 与 `pi_mpi_weak`
- `slurm_strong.sh`, `slurm_weak.sh`: Dardel 批处理脚本
- `plot_efficiency.py`: 计算并绘制强/弱扩展效率

## 编译

### 学校集群 / 本地 (OpenMPI)

```bash
cd "Exercise 1"
make CC=mpicc pi_mpi_strong pi_mpi_weak
```

### Dardel (Cray MPI wrapper)

```bash
cd ~/dd2356_a2/"Exercise 1"
make clean
make CC=cc pi_mpi_strong pi_mpi_weak
```

## 运行

### 学校集群交互式运行

```bash
cd "Exercise 1"
for p in 1 2 4 8 16; do
  mpirun -np $p ./pi_mpi_strong 10000000
done

for p in 1 2 4 8 16; do
  mpirun -np $p ./pi_mpi_weak 0
done
```

说明：弱扩展二进制里每个 rank 固定 `1,000,000` toss，命令行参数在该模式下不使用。

### Dardel 批处理运行

```bash
cd ~/dd2356_a2/"Exercise 1"
chmod +x slurm_strong.sh slurm_weak.sh
sbatch slurm_strong.sh
sbatch slurm_weak.sh
```

查看输出：

```bash
squeue -u $USER
ls -lh dardel_*.out
```

## 绘图（效率曲线）

```bash
cd "Exercise 1"
python3 plot_efficiency.py
```

将生成强扩展与弱扩展效率图（根据脚本默认输出名）。
