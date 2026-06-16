#!/usr/bin/env python3
import glob

import matplotlib.pyplot as plt
import numpy as np

files = sorted(glob.glob("gol_output_*.txt"))

for file in files:
    data = np.loadtxt(file)
    plt.imshow(data, cmap="binary", origin="lower")
    plt.title(f"Game of Life - {file}")
    plt.pause(0.5)

plt.show()
