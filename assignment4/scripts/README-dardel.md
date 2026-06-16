# Dardel 上跑 Assignment 4

工作目录（已创建）：

```text
~/DD2356/assignment4/
├── Exercise 1/   Exercise 2/   Exercise 3/
├── results/ex1/  ex2/  ex3/
├── scripts/
└── logs/         # sbatch 输出（首次提交后生成）
```

## 1. 上传代码（在本地 assignment4 目录执行）

```bash
rsync -avz --progress -e "ssh -o GSSAPIAuthentication=yes" \
  "./" weiyil@dardel.pdc.kth.se:~/DD2356/assignment4/
```

大文件传输建议用 `dardel-ftn01.pdc.kth.se`（课程说明）。

## 2. 登录 Dardel

```bash
ssh -o GSSAPIAuthentication=yes weiyil@dardel.pdc.kth.se
cd ~/DD2356/assignment4
```

## 3. 加载环境

```bash
source scripts/dardel_setup_env.sh
```

## 4. 交互式快速测试（单节点，≤4 进程）

```bash
salloc -A edu26.dd2356 -N 1 -t 00:30:00 --ntasks-per-node=4
cd ~/DD2356/assignment4/Exercise\ 1
source ../scripts/dardel_setup_env.sh
cc -O3 -o wave_mpi wave_mpi.c -lm
srun -n 4 ./wave_mpi
```

## 5. 批处理作业（扩展性 1–16 进程）

项目号：`edu26.dd2356`（已写入 `dardel_run_mpi.slurm`）。

```bash
mkdir -p logs
cd ~/DD2356/assignment4
sbatch scripts/dardel_run_mpi.slurm 1 16   # 练习 1，16 进程
squeue -u $USER
```

## 6. Score-P（最大规模那次）

在实现完成、最大 `np` 跑通后，按课程 Tutorial 对可执行文件做 Score-P 插桩与 `scorep` 分析。

## 注意

- 作业要求：单节点最多 4 进程时，16 进程需跨 4 个节点（见 slurm 里 `-N 4 --ntasks-per-node=4`）。
- 测扩展性前在源码中关闭或减少 I/O。
