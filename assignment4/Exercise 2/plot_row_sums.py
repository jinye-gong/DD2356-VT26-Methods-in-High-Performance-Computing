#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("row_sums_output.txt")
plt.plot(data, marker="o", label="Row Sums")
plt.xlabel("Row Index")
plt.ylabel("Sum Value")
plt.title("Parallel Row Sum Computation with MPI")
plt.legend()
plt.tight_layout()
plt.savefig("row_sums.png", dpi=200)
print("Saved row_sums.png")
plt.show()
