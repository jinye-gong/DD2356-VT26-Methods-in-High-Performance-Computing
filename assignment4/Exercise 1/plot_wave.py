#!/usr/bin/env python3
import glob

import matplotlib.pyplot as plt
import numpy as np

files = sorted(glob.glob("wave_output_*.txt"))

for file in files:
    data = np.loadtxt(file)
    step = file.split("_")[-1].split(".")[0]
    plt.plot(data, label=f"Time step {step}")

plt.xlabel("Grid Point")
plt.ylabel("Wave Amplitude")
plt.title("Wave Equation Evolution")
plt.legend()
plt.tight_layout()
plt.savefig("wave_evolution.png", dpi=200)
print("Saved wave_evolution.png")
plt.show()
