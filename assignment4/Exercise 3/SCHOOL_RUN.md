# Exercise 3 — 学校集群（`~/dd2356_a4/Exe3`）

## 上传文件

```text
gol_mpi.c
run_school_ex3.sh
run_efficiency.sh
plot_gol.py
```

## 一键运行

```bash
cd ~/dd2356_a4/Exe3
chmod +x run_school_ex3.sh run_efficiency.sh
./run_school_ex3.sh
```

## 分步

```bash
mpicc -O3 -o gol_mpi gol_mpi.c
export GOL_N=200 GOL_STEPS=40 GOL_IO=1
mpirun -np 4 ./gol_mpi
python3 plot_gol.py

export GOL_N=2000 GOL_STEPS=500 GOL_IO=0
./run_efficiency.sh ./gol_mpi efficiency_school_ex3.csv
```

## 跑完发回

- `efficiency_school_ex3.csv`
- 可选：`gol_output_0.txt` + `plot_gol.py` 截图

## 截图

见 `../SCREENSHOT_COMMANDS.md`：`ex3_code.png`, `ex3_compile.png`, `ex3_run_np16.png`, `ex3_scaling_csv.png`
