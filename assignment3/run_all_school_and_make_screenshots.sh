#!/usr/bin/env bash
set -euo pipefail

# One-click run for school cluster:
# - Compile and run Exercise 1/2/3
# - Generate scaling CSV + plot PNG
# - Capture code / compile / max-thread outputs
# - Render screenshot-style PNGs under Screenshots_school/

ROOT="$(pwd)"
SHOT_DIR="$ROOT/Screenshots_school"
TXT_DIR="$SHOT_DIR/_text"
mkdir -p "$TXT_DIR"

log_cmd() {
  local out="$1"
  shift
  {
    echo "$ $*"
    "$@"
  } >"$out" 2>&1 || true
}

render() {
  local in_txt="$1"
  local out_png="$2"
  local title="$3"
  python3 "$ROOT/render_text_png.py" --input "$in_txt" --output "$out_png" --title "$title"
}

echo "==> Exercise 1"
chmod +x "Exercise 1/run_scaling.sh"

# Task1
sed -n '25,80p' "Exercise 1/matmul_ex1_task1.c" > "$TXT_DIR/ex1_task1_code_school.txt"
log_cmd "$TXT_DIR/ex1_task1_compile_school.txt" gcc -O3 -fopenmp -march=native "Exercise 1/matmul_ex1_task1.c" -o "Exercise 1/matmul_ex1_task1"
log_cmd "$TXT_DIR/ex1_task1_maxthreads_school.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 1/matmul_ex1_task1\" 1024"
./"Exercise 1/run_scaling.sh" 1024 "Exercise 1/scaling_school.csv" "./Exercise 1/matmul_ex1_task1"
python3 plot_scaling.py --csv "Exercise 1/scaling_school.csv" --title "MatMul Task1 School" --out "Exercise 1/strong_scaling_school.png"

# Task2
sed -n '25,80p' "Exercise 1/matmul_ex1_task2_simd.c" > "$TXT_DIR/ex1_task2_code_school.txt"
log_cmd "$TXT_DIR/ex1_task2_compile_school.txt" gcc -O3 -fopenmp -march=native "Exercise 1/matmul_ex1_task2_simd.c" -o "Exercise 1/matmul_ex1_task2_simd"
log_cmd "$TXT_DIR/ex1_task2_maxthreads_school.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 1/matmul_ex1_task2_simd\" 1024"
./"Exercise 1/run_scaling.sh" 1024 "Exercise 1/scaling_school_task2_simd.csv" "./Exercise 1/matmul_ex1_task2_simd"
python3 plot_scaling.py --csv "Exercise 1/scaling_school_task2_simd.csv" --title "MatMul Task2 SIMD School" --out "Exercise 1/strong_scaling_school_task2_simd.png"

# Task3
sed -n '25,90p' "Exercise 1/matmul_ex1_task3_hybrid.c" > "$TXT_DIR/ex1_task3_code_school.txt"
log_cmd "$TXT_DIR/ex1_task3_compile_school.txt" gcc -O3 -fopenmp -march=native "Exercise 1/matmul_ex1_task3_hybrid.c" -o "Exercise 1/matmul_ex1_task3_hybrid"
log_cmd "$TXT_DIR/ex1_task3_maxthreads_school.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 1/matmul_ex1_task3_hybrid\" 1024"
./"Exercise 1/run_scaling.sh" 1024 "Exercise 1/scaling_school_task3_hybrid.csv" "./Exercise 1/matmul_ex1_task3_hybrid"
python3 plot_scaling.py --csv "Exercise 1/scaling_school_task3_hybrid.csv" --title "MatMul Task3 Hybrid School" --out "Exercise 1/strong_scaling_school_task3_hybrid.png"

# Task4 (offload try -> fallback)
sed -n '25,120p' "Exercise 1/matmul_ex1_task4_gpu.c" > "$TXT_DIR/ex1_task4_code_school.txt"
if log_cmd "$TXT_DIR/ex1_task4_compile_school.txt" gcc -O3 -fopenmp -march=native "Exercise 1/matmul_ex1_task4_gpu.c" -o "Exercise 1/matmul_ex1_task4_gpu"; then
  :
else
  log_cmd "$TXT_DIR/ex1_task4_compile_school.txt" gcc -O3 -fopenmp -march=native -DDISABLE_OFFLOAD "Exercise 1/matmul_ex1_task4_gpu.c" -o "Exercise 1/matmul_ex1_task4_gpu"
fi
log_cmd "$TXT_DIR/ex1_task4_maxthreads_school.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 1/matmul_ex1_task4_gpu\" 1024"
./"Exercise 1/run_scaling.sh" 1024 "Exercise 1/scaling_school_task4_gpu.csv" "./Exercise 1/matmul_ex1_task4_gpu"
python3 plot_scaling.py --csv "Exercise 1/scaling_school_task4_gpu.csv" --title "MatMul Task4 School" --out "Exercise 1/strong_scaling_school_task4_gpu.png"

echo "==> Exercise 2"
chmod +x "Exercise 2/run_scaling_bfs.sh"

# Task1
sed -n '70,190p' "Exercise 2/bfs_ex2_task1_parallel.c" > "$TXT_DIR/ex2_task1_code_school.txt"
log_cmd "$TXT_DIR/ex2_task1_compile_school.txt" gcc -O3 -fopenmp -march=native "Exercise 2/bfs_ex2_task1_parallel.c" -o "Exercise 2/bfs_ex2_task1_parallel"
log_cmd "$TXT_DIR/ex2_task1_maxthreads_school.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 2/bfs_ex2_task1_parallel\" 50000 8"
./"Exercise 2/run_scaling_bfs.sh" 50000 8 "Exercise 2/scaling_school_ex2_task1_bfs.csv" "./Exercise 2/bfs_ex2_task1_parallel"
python3 plot_scaling.py --csv "Exercise 2/scaling_school_ex2_task1_bfs.csv" --title "BFS Task1 School" --out "Exercise 2/strong_scaling_school_ex2_task1_bfs.png"

# Task2
sed -n '80,240p' "Exercise 2/bfs_ex2_task2_task.c" > "$TXT_DIR/ex2_task2_code_school.txt"
log_cmd "$TXT_DIR/ex2_task2_compile_school.txt" gcc -O3 -fopenmp -march=native "Exercise 2/bfs_ex2_task2_task.c" -o "Exercise 2/bfs_ex2_task2_task"
log_cmd "$TXT_DIR/ex2_task2_maxthreads_school.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 2/bfs_ex2_task2_task\" 50000 8"
./"Exercise 2/run_scaling_bfs.sh" 50000 8 "Exercise 2/scaling_school_ex2_task2_bfs_task.csv" "./Exercise 2/bfs_ex2_task2_task"
python3 plot_scaling.py --csv "Exercise 2/scaling_school_ex2_task2_bfs_task.csv" --title "BFS Task2 Task School" --out "Exercise 2/strong_scaling_school_ex2_task2_bfs_task.png"

# Task3 random graph rerun
./"Exercise 2/run_scaling_bfs.sh" 200000 16 "Exercise 2/scaling_school_ex2_task3_bfs_parallel.csv" "./Exercise 2/bfs_ex2_task1_parallel"
./"Exercise 2/run_scaling_bfs.sh" 200000 16 "Exercise 2/scaling_school_ex2_task3_bfs_task.csv" "./Exercise 2/bfs_ex2_task2_task"
python3 plot_scaling.py --csv "Exercise 2/scaling_school_ex2_task3_bfs_parallel.csv" --title "BFS Task3 Parallel School" --out "Exercise 2/strong_scaling_school_ex2_task3_bfs_parallel.png"
python3 plot_scaling.py --csv "Exercise 2/scaling_school_ex2_task3_bfs_task.csv" --title "BFS Task3 Task School" --out "Exercise 2/strong_scaling_school_ex2_task3_bfs_task.png"

echo "==> Exercise 3"
chmod +x "Exercise 3/run_scaling_shallow.sh"
sed -n '80,190p' "Exercise 3/shallow_water_ex3.c" > "$TXT_DIR/ex3_code_school.txt"
log_cmd "$TXT_DIR/ex3_compile_school.txt" gcc -O3 -fopenmp -march=native "Exercise 3/shallow_water_ex3.c" -o "Exercise 3/shallow_water_ex3"
log_cmd "$TXT_DIR/ex3_static_maxthreads_school.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 3/shallow_water_ex3\" 500 1000 static \"$ROOT/Exercise 3/output_static_64.txt\""
log_cmd "$TXT_DIR/ex3_dynamic_maxthreads_school.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 3/shallow_water_ex3\" 500 1000 dynamic \"$ROOT/Exercise 3/output_dynamic_64.txt\""
log_cmd "$TXT_DIR/ex3_guided_maxthreads_school.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 3/shallow_water_ex3\" 500 1000 guided \"$ROOT/Exercise 3/output_guided_64.txt\""
./"Exercise 3/run_scaling_shallow.sh" 500 1000 static  "Exercise 3/scaling_school_ex3_static.csv" "./Exercise 3/shallow_water_ex3"
./"Exercise 3/run_scaling_shallow.sh" 500 1000 dynamic "Exercise 3/scaling_school_ex3_dynamic.csv" "./Exercise 3/shallow_water_ex3"
./"Exercise 3/run_scaling_shallow.sh" 500 1000 guided  "Exercise 3/scaling_school_ex3_guided.csv" "./Exercise 3/shallow_water_ex3"
python3 plot_scaling.py --csv "Exercise 3/scaling_school_ex3_static.csv" --title "Shallow Water Static School" --out "Exercise 3/strong_scaling_school_ex3_static.png"
python3 plot_scaling.py --csv "Exercise 3/scaling_school_ex3_dynamic.csv" --title "Shallow Water Dynamic School" --out "Exercise 3/strong_scaling_school_ex3_dynamic.png"
python3 plot_scaling.py --csv "Exercise 3/scaling_school_ex3_guided.csv" --title "Shallow Water Guided School" --out "Exercise 3/strong_scaling_school_ex3_guided.png"

echo "==> Rendering screenshot-style PNGs"
render "$TXT_DIR/ex1_task1_code_school.txt" "$SHOT_DIR/ex1_task1_code_school.png" "Exercise 1 Task1 Code School"
render "$TXT_DIR/ex1_task1_compile_school.txt" "$SHOT_DIR/ex1_task1_compile_school.png" "Exercise 1 Task1 Compile School"
render "$TXT_DIR/ex1_task1_maxthreads_school.txt" "$SHOT_DIR/ex1_task1_maxthreads_school.png" "Exercise 1 Task1 Max Threads School"
render "$TXT_DIR/ex1_task2_code_school.txt" "$SHOT_DIR/ex1_task2_code_school.png" "Exercise 1 Task2 Code School"
render "$TXT_DIR/ex1_task2_compile_school.txt" "$SHOT_DIR/ex1_task2_compile_school.png" "Exercise 1 Task2 Compile School"
render "$TXT_DIR/ex1_task2_maxthreads_school.txt" "$SHOT_DIR/ex1_task2_maxthreads_school.png" "Exercise 1 Task2 Max Threads School"
render "$TXT_DIR/ex1_task3_code_school.txt" "$SHOT_DIR/ex1_task3_code_school.png" "Exercise 1 Task3 Code School"
render "$TXT_DIR/ex1_task3_compile_school.txt" "$SHOT_DIR/ex1_task3_compile_school.png" "Exercise 1 Task3 Compile School"
render "$TXT_DIR/ex1_task3_maxthreads_school.txt" "$SHOT_DIR/ex1_task3_maxthreads_school.png" "Exercise 1 Task3 Max Threads School"
render "$TXT_DIR/ex1_task4_code_school.txt" "$SHOT_DIR/ex1_task4_code_school.png" "Exercise 1 Task4 Code School"
render "$TXT_DIR/ex1_task4_compile_school.txt" "$SHOT_DIR/ex1_task4_compile_school.png" "Exercise 1 Task4 Compile School"
render "$TXT_DIR/ex1_task4_maxthreads_school.txt" "$SHOT_DIR/ex1_task4_maxthreads_school.png" "Exercise 1 Task4 Max Threads School"
render "$TXT_DIR/ex2_task1_code_school.txt" "$SHOT_DIR/ex2_task1_code_school.png" "Exercise 2 Task1 Code School"
render "$TXT_DIR/ex2_task1_compile_school.txt" "$SHOT_DIR/ex2_task1_compile_school.png" "Exercise 2 Task1 Compile School"
render "$TXT_DIR/ex2_task1_maxthreads_school.txt" "$SHOT_DIR/ex2_task1_maxthreads_school.png" "Exercise 2 Task1 Max Threads School"
render "$TXT_DIR/ex2_task2_code_school.txt" "$SHOT_DIR/ex2_task2_code_school.png" "Exercise 2 Task2 Code School"
render "$TXT_DIR/ex2_task2_compile_school.txt" "$SHOT_DIR/ex2_task2_compile_school.png" "Exercise 2 Task2 Compile School"
render "$TXT_DIR/ex2_task2_maxthreads_school.txt" "$SHOT_DIR/ex2_task2_maxthreads_school.png" "Exercise 2 Task2 Max Threads School"
render "$TXT_DIR/ex3_code_school.txt" "$SHOT_DIR/ex3_code_school.png" "Exercise 3 Code School"
render "$TXT_DIR/ex3_compile_school.txt" "$SHOT_DIR/ex3_compile_school.png" "Exercise 3 Compile School"
render "$TXT_DIR/ex3_static_maxthreads_school.txt" "$SHOT_DIR/ex3_static_maxthreads_school.png" "Exercise 3 Static Max Threads School"
render "$TXT_DIR/ex3_dynamic_maxthreads_school.txt" "$SHOT_DIR/ex3_dynamic_maxthreads_school.png" "Exercise 3 Dynamic Max Threads School"
render "$TXT_DIR/ex3_guided_maxthreads_school.txt" "$SHOT_DIR/ex3_guided_maxthreads_school.png" "Exercise 3 Guided Max Threads School"

echo "Done."
echo "School screenshots: $SHOT_DIR"
