# Assignment3 Directory Map

This project is now organized in two layers:

## 1) Source/Execution folders

- `Exercise 1/` - Matrix multiplication code and run scripts
- `Exercise 2/` - BFS code and run scripts
- `Exercise 3/` - Shallow water code and run scripts
- `Bonus/` - Bonus neuron-task implementation and scripts

These folders keep source code and runnable binaries/scripts.

## 2) Unified artifacts folder

- `Artifacts/results/`
  - `local/` - local CSV timing/scaling results
  - `school/` - school-cluster CSV timing/scaling results
  - `dardel/` - Dardel CSV timing/scaling results
  - `bonus/` - bonus CSV results
- `Artifacts/plots/`
  - `local/` - local scaling PNGs
  - `school/` - school scaling PNGs
  - `dardel/` - Dardel scaling PNGs
  - `bonus/` - bonus plots (scaling/activity/histogram)
- `Artifacts/screenshots/`
  - `local/` - local screenshot-style PNGs
  - `dardel/` - Dardel screenshot-style PNGs
  - `school/` - school screenshot-style PNGs (copied from `Bonus/school_results`)
  - `bonus/` - reserved for bonus-only screenshots
  - `raw_text/` - intermediate text captures used to render screenshot PNGs

## Notes

- Existing scripts and report remain usable.
- If you package for submission, prefer this order:
  1. `Exercise 1/`, `Exercise 2/`, `Exercise 3/`, `Bonus/`
  2. `Artifacts/` (optional but recommended for traceability)
  3. `Assignment3_Report.md` / PDF
