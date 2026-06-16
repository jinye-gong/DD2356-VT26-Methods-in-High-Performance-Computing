# Exercise 1 — 学校集群（`~/dd2356_a4/Exe1`）

## 目录结构

```text
~/dd2356_a4/
├── Exe1/                    ← 在本目录运行
│   ├── wave_mpi.c
│   ├── run_school_ex1.sh
│   ├── run_scaling_strong.sh
│   ├── plot_wave.py
│   └── plot_scaling_mpi.py   ← 建议一并上传（画图用）
└── Artifacts/                ← 可选，没有也能跑
```

## 一键运行

```bash
cd ~/dd2356_a4/Exe1
chmod +x run_school_ex1.sh run_scaling_strong.sh
./run_school_ex1.sh
```

## 分步运行

```bash
cd ~/dd2356_a4/Exe1
mpicc -O3 -o wave_mpi wave_mpi.c -lm
mpirun -np 4 ./wave_mpi 1000 100 1
python3 plot_wave.py

export WAVE_N=200000 WAVE_STEPS=500 WAVE_IO=0
./run_scaling_strong.sh ./wave_mpi scaling_school_ex1.csv
```

## 跑完发回

- `~/dd2356_a4/Exe1/scaling_school_ex1.csv`
