# Exercise 2 README

本题目标：构建并绘制 roofline（single/double/integer）。

## 文件说明

- `simple_roofline.cpp`: roofline 基准程序
- `Makefile`: 编译 `roofline`
- `run_local_roofline.sh`: 本地运行
- `run_school_cluster_roofline.sh`: 学校集群运行
- `slurm_dardel_roofline.sh`: Dardel 批处理脚本
- `plot_roofline.py`: 将原始输出转换为图

## 编译

### 本地 / 学校集群

```bash
cd "Exercise 2"
make clean
make
```

### Dardel

```bash
cd ~/dd2356_a2/"Exercise 2"
make clean
make CXX=CC
```

## 运行

### 本地

```bash
cd "Exercise 2"
chmod +x run_local_roofline.sh
./run_local_roofline.sh
```

### 学校集群

```bash
cd "Exercise 2"
chmod +x run_school_cluster_roofline.sh
./run_school_cluster_roofline.sh
```

### Dardel

```bash
cd ~/dd2356_a2/"Exercise 2"
chmod +x slurm_dardel_roofline.sh
sbatch slurm_dardel_roofline.sh
```

## 绘图

示例（学校集群）：

```bash
cd "Exercise 2"
python3 plot_roofline.py \
  --input school_roofline_raw.txt \
  --title "School Cluster Roofline (DD2356 Medium CPU)" \
  --output school_roofline.png
```

示例（Dardel）：

```bash
python3 plot_roofline.py \
  --input dardel_roofline_raw.txt \
  --title "Dardel Roofline" \
  --output dardel_roofline.png
```
