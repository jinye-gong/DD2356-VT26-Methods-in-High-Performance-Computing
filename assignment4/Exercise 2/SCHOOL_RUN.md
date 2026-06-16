# Exercise 2 — 学校集群（`~/dd2356_a4/Exe2`）

## 上传文件

```text
~/dd2356_a4/Exe2/
├── row_sum_mpi.c
├── run_school_ex2.sh
├── run_scaling_weak.sh
├── plot_row_sums.py
└── plot_scaling_mpi.py
```

## 一键运行

```bash
cd ~/dd2356_a4/Exe2
chmod +x run_school_ex2.sh run_scaling_weak.sh
./run_school_ex2.sh
```

## 分步

```bash
mpicc -O3 -o row_sum_mpi row_sum_mpi.c
export WEAK_N=1000 ROWSUM_IO=1
mpirun -np 4 ./row_sum_mpi
python3 plot_row_sums.py

export WEAK_BASE_N=1000 ROWSUM_IO=0
./run_scaling_weak.sh ./row_sum_mpi scaling_weak_school_ex2.csv
```

## 跑完发回

- `scaling_weak_school_ex2.csv`
- 可选：`row_sums_output.txt`

## 截图（见 `../SCREENSHOT_COMMANDS.md`）

- `ex2_code.png` — Scatter / Gather / Reduce
- `ex2_compile.png` — `mpicc` + `ls -la row_sum_mpi`
- `ex2_run_np16.png` — `mpirun -np 16` + `WALLTIME`
- `ex2_scaling_csv.png` — `cat scaling_weak_school_ex2.csv`
