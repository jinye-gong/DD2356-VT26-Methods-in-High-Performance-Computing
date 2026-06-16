#!/usr/bin/env bash
set -euo pipefail

# One-click bonus runner:
# - compile serial/task neuron simulation
# - run correctness outputs (neuron_output.txt / neuron_summary.txt)
# - run strong scaling for task version
# - generate scaling plot + activity/histogram plots
# - generate screenshot-style PNGs (code/compile/max-thread)

ROOT="$(pwd)"
SHOT_DIR="$ROOT/Screenshots_bonus"
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
  python3 "$ROOT/render_text_png_bonus.py" --input "$in_txt" --output "$out_png" --title "$title"
}

# 1) code screenshot source
sed -n '1,260p' "$ROOT/neuron_omp_task.c" > "$TXT_DIR/bonus_code.txt"

# 2) compile screenshot
log_cmd "$TXT_DIR/bonus_compile.txt" gcc -O3 -fopenmp -march=native "$ROOT/neuron_omp_task.c" -o "$ROOT/neuron_omp_task"

# 3) correctness run with output files
log_cmd "$TXT_DIR/bonus_task_correctness.txt" "$ROOT/neuron_omp_task" task 1000 500 50 1 42
cp neuron_output.txt "$ROOT/neuron_output_task.txt"
cp neuron_summary.txt "$ROOT/neuron_summary_task.txt"

# 4) max-thread output screenshot
log_cmd "$TXT_DIR/bonus_maxthreads.txt" bash -lc "OMP_NUM_THREADS=64 \"$ROOT/neuron_omp_task\" task 50000 500 50 0 42"

# 5) scaling
chmod +x "$ROOT/run_scaling_neuron.sh"
"$ROOT/run_scaling_neuron.sh" 50000 500 50 "$ROOT/scaling_bonus_neuron_task.csv" "$ROOT/neuron_omp_task"
python3 ../plot_scaling.py --csv "$ROOT/scaling_bonus_neuron_task.csv" --title "Bonus Neuron Task Scaling" --out "$ROOT/strong_scaling_bonus_neuron_task.png"

# 6) activity plots
python3 "$ROOT/plot_neuron_activity.py" \
  --activity "$ROOT/neuron_output_task.txt" \
  --summary "$ROOT/neuron_summary_task.txt" \
  --scatter-out "$ROOT/neuron_activity_scatter.png" \
  --hist-out "$ROOT/neuron_firing_hist.png"

# 7) render screenshot-style PNG files
render "$TXT_DIR/bonus_code.txt" "$SHOT_DIR/bonus_code.png" "Bonus Code"
render "$TXT_DIR/bonus_compile.txt" "$SHOT_DIR/bonus_compile.png" "Bonus Compile"
render "$TXT_DIR/bonus_maxthreads.txt" "$SHOT_DIR/bonus_maxthreads.png" "Bonus Max Threads Output"
render "$TXT_DIR/bonus_task_correctness.txt" "$SHOT_DIR/bonus_correctness_run.png" "Bonus Correctness Run"

echo "Done."
echo "Outputs:"
echo "  $ROOT/scaling_bonus_neuron_task.csv"
echo "  $ROOT/strong_scaling_bonus_neuron_task.png"
echo "  $ROOT/neuron_activity_scatter.png"
echo "  $ROOT/neuron_firing_hist.png"
echo "  $SHOT_DIR"
