# Exercise 3 README

本题目标：运行 SpMV（penta-diagonal）并记录运行时间与 GFLOP/s。

## 文件说明

- `spmv.c`: SpMV 主程序
- `run_spmv_bench.sh`: 自动跑多个规模并解析输出

## 编译

```bash
cd "Exercise 3"
gcc -O3 -march=native -o spmv spmv.c
```

## 运行（单次手动）

程序参数使用指数形式：`100 -> 10^4`, `1000 -> 10^6`, `10000 -> 10^8`。

```bash
cd "Exercise 3"
taskset -c 0 ./spmv 100
taskset -c 0 ./spmv 1000
taskset -c 0 ./spmv 10000
```

## 运行（脚本批量）

```bash
cd "Exercise 3"
chmod +x run_spmv_bench.sh
./run_spmv_bench.sh
```

脚本会编译并运行三个规模，并输出可用于报告整理的数据（时间与性能）。

## Dardel 示例

```bash
cd ~/dd2356_a2/"Exercise 3"
srun -A edu26.dd2356 -p shared -N1 -n1 -c1 --cpu-bind=cores --mem=20G \
  gcc -O3 -march=native -o spmv spmv.c

srun -A edu26.dd2356 -p shared -N1 -n1 -c1 --cpu-bind=cores --mem=20G \
  taskset -c 0 ./spmv 10000
```
