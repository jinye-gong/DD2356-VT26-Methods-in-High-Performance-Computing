# Exercise 2 README

Goal: build and plot roofline models (single, double, integer).

## Files

- `simple_roofline.cpp`: roofline microbenchmark
- `Makefile`: builds `roofline`
- `run_local_roofline.sh`: local run script
- `run_school_cluster_roofline.sh`: school cluster run script
- `slurm_dardel_roofline.sh`: Dardel batch script
- `plot_roofline.py`: converts raw output to roofline plots

## Build

### Local / School cluster

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

## Run

### Local

```bash
cd "Exercise 2"
chmod +x run_local_roofline.sh
./run_local_roofline.sh
```

### School cluster

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

## Plotting

Example (school cluster):

```bash
cd "Exercise 2"
python3 plot_roofline.py \
  --input school_roofline_raw.txt \
  --title "School Cluster Roofline (DD2356 Medium CPU)" \
  --output school_roofline.png
```

Example (Dardel):

```bash
python3 plot_roofline.py \
  --input dardel_roofline_raw.txt \
  --title "Dardel Roofline" \
  --output dardel_roofline.png
```
