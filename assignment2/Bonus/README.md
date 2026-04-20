# Bonus README (MPI Ping-Pong)

本题目标：测量 RTT 与消息大小关系，并拟合线性模型。

## 文件说明

- `ping_pong_mpi.c`: ping-pong 基准程序（CSV 输出）
- `Makefile`: 编译程序
- `slurm_dardel_pingpong.sh`: Dardel 两节点作业脚本
- `run_school_pingpong.sh`: 学校集群运行脚本
- `fit_pingpong_model.py`: 拟合 `RTT = alpha + beta * bytes` 并画图

## 编译

### 学校集群 / 本地

```bash
cd Bonus
make clean
make CC=mpicc
```

### Dardel

```bash
cd ~/dd2356_a2/Bonus
make clean
make CC=cc
```

## 运行

### 学校集群（2 进程）

```bash
cd Bonus
chmod +x run_school_pingpong.sh
bash run_school_pingpong.sh
```

输出：`school_pingpong_rtt.csv`

### Dardel（2 节点，1 rank/节点）

```bash
cd ~/dd2356_a2/Bonus
chmod +x slurm_dardel_pingpong.sh
sbatch slurm_dardel_pingpong.sh
```

查看作业：

```bash
squeue -u $USER
ls -lh dardel_pingpong_*.out dardel_pingpong_rtt.csv
```

## 拉取 Dardel 数据到本地

```bash
scp -o GSSAPIAuthentication=yes \
  weiyil@dardel.pdc.kth.se:~/dd2356_a2/Bonus/dardel_pingpong_rtt.csv \
  "Bonus/"
```

## 拟合与绘图

### 学校集群数据

```bash
python3 fit_pingpong_model.py \
  --input school_pingpong_rtt.csv \
  --system "School cluster" \
  --output-prefix school
```

### Dardel 数据

```bash
python3 fit_pingpong_model.py \
  --input dardel_pingpong_rtt.csv \
  --system "Dardel" \
  --output-prefix dardel
```

输出：

- `<prefix>_model.txt`
- `<prefix>_rtt_fit.png`
