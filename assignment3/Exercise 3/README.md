# Exercise 3 - Parallel Shallow Water Simulation

This folder contains:

- `shallow_water_ex3.c` - OpenMP-parallel 2D solver (`collapse(2)`, runtime schedule)
- `run_scaling_shallow.sh` - scaling script for static/dynamic/guided

## Build

```bash
gcc -O3 -fopenmp -march=native shallow_water_ex3.c -o shallow_water_ex3
```

## Run single test

```bash
OMP_NUM_THREADS=64 ./shallow_water_ex3 500 1000 static output_static_64.txt
OMP_NUM_THREADS=64 ./shallow_water_ex3 500 1000 dynamic output_dynamic_64.txt
OMP_NUM_THREADS=64 ./shallow_water_ex3 500 1000 guided output_guided_64.txt
```

Arguments:

```text
./shallow_water_ex3 [N] [ITER] [static|dynamic|guided] [output.txt]
```

## Strong scaling

```bash
chmod +x run_scaling_shallow.sh
./run_scaling_shallow.sh 500 1000 static scaling_school_ex3_static.csv ./shallow_water_ex3
./run_scaling_shallow.sh 500 1000 dynamic scaling_school_ex3_dynamic.csv ./shallow_water_ex3
./run_scaling_shallow.sh 500 1000 guided scaling_school_ex3_guided.csv ./shallow_water_ex3
```

Plot:

```bash
python3 ../plot_scaling.py --csv scaling_school_ex3_static.csv --title "Shallow Water Static School" --out strong_scaling_school_ex3_static.png
python3 ../plot_scaling.py --csv scaling_school_ex3_dynamic.csv --title "Shallow Water Dynamic School" --out strong_scaling_school_ex3_dynamic.png
python3 ../plot_scaling.py --csv scaling_school_ex3_guided.csv --title "Shallow Water Guided School" --out strong_scaling_school_ex3_guided.png
```

## Output visualization

```python
import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("output_static_64.txt")
plt.imshow(data, cmap="viridis", origin="lower")
plt.colorbar(label="Water Height")
plt.title("Shallow Water Simulation Result")
plt.show()
```
