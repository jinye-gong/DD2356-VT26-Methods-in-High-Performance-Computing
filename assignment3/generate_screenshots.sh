#!/usr/bin/env bash
set -euo pipefail

ROOT="$(pwd)"
OUT_DIR="$ROOT/Screenshots"
TXT_DIR="$OUT_DIR/_text"
mkdir -p "$TXT_DIR"

py_render() {
  local in_txt="$1"
  local out_png="$2"
  local title="$3"
  python3 "$ROOT/generate_text_image.py" --input "$in_txt" --output "$out_png" --title "$title"
}

run_capture() {
  local outfile="$1"
  shift
  {
    echo "$ $*"
    "$@"
  } > "$outfile" 2>&1 || true
}

# -------- Exercise 1 code snippets --------
sed -n '25,75p' "$ROOT/Exercise 1/matmul_ex1_task1.c" > "$TXT_DIR/ex1_task1_code.txt"
sed -n '25,75p' "$ROOT/Exercise 1/matmul_ex1_task2_simd.c" > "$TXT_DIR/ex1_task2_code.txt"
sed -n '25,80p' "$ROOT/Exercise 1/matmul_ex1_task3_hybrid.c" > "$TXT_DIR/ex1_task3_code.txt"
sed -n '25,110p' "$ROOT/Exercise 1/matmul_ex1_task4_gpu.c" > "$TXT_DIR/ex1_task4_code.txt"

py_render "$TXT_DIR/ex1_task1_code.txt" "$OUT_DIR/ex1_task1_code.png" "Exercise 1 Task1 Code"
py_render "$TXT_DIR/ex1_task2_code.txt" "$OUT_DIR/ex1_task2_code.png" "Exercise 1 Task2 Code"
py_render "$TXT_DIR/ex1_task3_code.txt" "$OUT_DIR/ex1_task3_code.png" "Exercise 1 Task3 Code"
py_render "$TXT_DIR/ex1_task4_code.txt" "$OUT_DIR/ex1_task4_code.png" "Exercise 1 Task4 Code"

# -------- Exercise 1 compile + max thread outputs (local) --------
run_capture "$TXT_DIR/ex1_task1_compile_local.txt" gcc -O3 -fopenmp -march=native "$ROOT/Exercise 1/matmul_ex1_task1.c" -o "$ROOT/Exercise 1/matmul_ex1_task1"
run_capture "$TXT_DIR/ex1_task2_compile_local.txt" gcc -O3 -fopenmp -march=native "$ROOT/Exercise 1/matmul_ex1_task2_simd.c" -o "$ROOT/Exercise 1/matmul_ex1_task2_simd"
run_capture "$TXT_DIR/ex1_task3_compile_local.txt" gcc -O3 -fopenmp -march=native "$ROOT/Exercise 1/matmul_ex1_task3_hybrid.c" -o "$ROOT/Exercise 1/matmul_ex1_task3_hybrid"
run_capture "$TXT_DIR/ex1_task4_compile_local.txt" gcc -O3 -fopenmp -march=native -DDISABLE_OFFLOAD "$ROOT/Exercise 1/matmul_ex1_task4_gpu.c" -o "$ROOT/Exercise 1/matmul_ex1_task4_gpu"

run_capture "$TXT_DIR/ex1_task1_maxthreads_local.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 1/matmul_ex1_task1\" 1024"
run_capture "$TXT_DIR/ex1_task2_maxthreads_local.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 1/matmul_ex1_task2_simd\" 1024"
run_capture "$TXT_DIR/ex1_task3_maxthreads_local.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 1/matmul_ex1_task3_hybrid\" 1024"
run_capture "$TXT_DIR/ex1_task4_maxthreads_local.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 1/matmul_ex1_task4_gpu\" 1024"

py_render "$TXT_DIR/ex1_task1_compile_local.txt" "$OUT_DIR/ex1_task1_compile_local.png" "Exercise 1 Task1 Compile Local"
py_render "$TXT_DIR/ex1_task2_compile_local.txt" "$OUT_DIR/ex1_task2_compile_local.png" "Exercise 1 Task2 Compile Local"
py_render "$TXT_DIR/ex1_task3_compile_local.txt" "$OUT_DIR/ex1_task3_compile_local.png" "Exercise 1 Task3 Compile Local"
py_render "$TXT_DIR/ex1_task4_compile_local.txt" "$OUT_DIR/ex1_task4_compile_local.png" "Exercise 1 Task4 Compile Local"

py_render "$TXT_DIR/ex1_task1_maxthreads_local.txt" "$OUT_DIR/ex1_task1_maxthreads_local.png" "Exercise 1 Task1 Max Threads Local"
py_render "$TXT_DIR/ex1_task2_maxthreads_local.txt" "$OUT_DIR/ex1_task2_maxthreads_local.png" "Exercise 1 Task2 Max Threads Local"
py_render "$TXT_DIR/ex1_task3_maxthreads_local.txt" "$OUT_DIR/ex1_task3_maxthreads_local.png" "Exercise 1 Task3 Max Threads Local"
py_render "$TXT_DIR/ex1_task4_maxthreads_local.txt" "$OUT_DIR/ex1_task4_maxthreads_local.png" "Exercise 1 Task4 Max Threads Local"

# -------- Exercise 2 code + output (local) --------
sed -n '70,180p' "$ROOT/Exercise 2/bfs_ex2_task1_parallel.c" > "$TXT_DIR/ex2_task1_code.txt"
sed -n '80,220p' "$ROOT/Exercise 2/bfs_ex2_task2_task.c" > "$TXT_DIR/ex2_task2_code.txt"
py_render "$TXT_DIR/ex2_task1_code.txt" "$OUT_DIR/ex2_task1_code.png" "Exercise 2 Task1 Code"
py_render "$TXT_DIR/ex2_task2_code.txt" "$OUT_DIR/ex2_task2_code.png" "Exercise 2 Task2 Code"

run_capture "$TXT_DIR/ex2_task1_compile_local.txt" gcc -O3 -fopenmp -march=native "$ROOT/Exercise 2/bfs_ex2_task1_parallel.c" -o "$ROOT/Exercise 2/bfs_ex2_task1_parallel"
run_capture "$TXT_DIR/ex2_task2_compile_local.txt" gcc -O3 -fopenmp -march=native "$ROOT/Exercise 2/bfs_ex2_task2_task.c" -o "$ROOT/Exercise 2/bfs_ex2_task2_task"
run_capture "$TXT_DIR/ex2_task1_maxthreads_local.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 2/bfs_ex2_task1_parallel\" 50000 8"
run_capture "$TXT_DIR/ex2_task2_maxthreads_local.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 2/bfs_ex2_task2_task\" 50000 8"

py_render "$TXT_DIR/ex2_task1_compile_local.txt" "$OUT_DIR/ex2_task1_compile_local.png" "Exercise 2 Task1 Compile Local"
py_render "$TXT_DIR/ex2_task2_compile_local.txt" "$OUT_DIR/ex2_task2_compile_local.png" "Exercise 2 Task2 Compile Local"
py_render "$TXT_DIR/ex2_task1_maxthreads_local.txt" "$OUT_DIR/ex2_task1_maxthreads_local.png" "Exercise 2 Task1 Max Threads Local"
py_render "$TXT_DIR/ex2_task2_maxthreads_local.txt" "$OUT_DIR/ex2_task2_maxthreads_local.png" "Exercise 2 Task2 Max Threads Local"

# -------- Exercise 3 code + output (local) --------
sed -n '80,170p' "$ROOT/Exercise 3/shallow_water_ex3.c" > "$TXT_DIR/ex3_code.txt"
py_render "$TXT_DIR/ex3_code.txt" "$OUT_DIR/ex3_code.png" "Exercise 3 Code"

run_capture "$TXT_DIR/ex3_compile_local.txt" gcc -O3 -fopenmp -march=native "$ROOT/Exercise 3/shallow_water_ex3.c" -o "$ROOT/Exercise 3/shallow_water_ex3"
run_capture "$TXT_DIR/ex3_static_maxthreads_local.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 3/shallow_water_ex3\" 500 1000 static \"$ROOT/Exercise 3/output_static_64.txt\""
run_capture "$TXT_DIR/ex3_dynamic_maxthreads_local.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 3/shallow_water_ex3\" 500 1000 dynamic \"$ROOT/Exercise 3/output_dynamic_64.txt\""
run_capture "$TXT_DIR/ex3_guided_maxthreads_local.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/Exercise 3/shallow_water_ex3\" 500 1000 guided \"$ROOT/Exercise 3/output_guided_64.txt\""

py_render "$TXT_DIR/ex3_compile_local.txt" "$OUT_DIR/ex3_compile_local.png" "Exercise 3 Compile Local"
py_render "$TXT_DIR/ex3_static_maxthreads_local.txt" "$OUT_DIR/ex3_static_maxthreads_local.png" "Exercise 3 Static Max Threads Local"
py_render "$TXT_DIR/ex3_dynamic_maxthreads_local.txt" "$OUT_DIR/ex3_dynamic_maxthreads_local.png" "Exercise 3 Dynamic Max Threads Local"
py_render "$TXT_DIR/ex3_guided_maxthreads_local.txt" "$OUT_DIR/ex3_guided_maxthreads_local.png" "Exercise 3 Guided Max Threads Local"

echo "All generated under: $OUT_DIR"
