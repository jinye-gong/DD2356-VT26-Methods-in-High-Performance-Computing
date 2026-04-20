# Exercise 4 README

本题目标：使用 `perf` 分析矩阵乘与转置，并比较优化效果。

## 文件说明

- `matrix_multiply.c`: naive + optimized（循环重排）版本
- `transpose.c`: baseline + blocked transpose
- `run_perf_mm.sh`: 批量跑矩阵乘 perf
- `run_perf_transpose.sh`: 批量跑转置 perf

## 1) Matrix Multiply

### 编译（按作业要求基础命令）

```bash
cd "Exercise 4"
gcc -g -O2 matrix_multiply.c -o matrix_multiply.out
```

### 运行（程序内版本选择）

```bash
./matrix_multiply.out naive
./matrix_multiply.out opt
```

### perf 采样示例

```bash
perf stat -e cycles,instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses \
  ./matrix_multiply.out naive
```

## 2) Transpose

### 编译

```bash
cd "Exercise 4"
gcc -O2 -o transpose.out transpose.c
```

### 运行示例

```bash
./transpose.out 64
./transpose.out 128
./transpose.out 2048
./transpose.out 2048 blocked 32
```

### perf 采样示例

```bash
perf stat -e cycles,instructions,L1-dcache-loads,L1-dcache-load-misses,LLC-loads,LLC-load-misses \
  ./transpose.out 2048
```

## 一键脚本

```bash
cd "Exercise 4"
chmod +x run_perf_mm.sh run_perf_transpose.sh
./run_perf_mm.sh
./run_perf_transpose.sh
```

如果遇到 `perf` 权限限制，请临时设置：

```bash
sudo sysctl -w kernel.perf_event_paranoid=1
```
