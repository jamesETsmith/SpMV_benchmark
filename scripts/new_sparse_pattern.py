#!/usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

# Make patterned, but "nonstandard" sparse matrix
# Watch out for collision (you want to make sure we don't set the same value
# twice). This isn't a problem now, but it will be in C++.
N = 400
reg_size = int(N / 3)

spm = np.zeros((N, N))
for i in range(1, reg_size):
    for j in range(1, 5):
        # Region 1
        # print(i, i * j)
        if i * j < reg_size:
            spm[i, i * j] = np.log(1. + i + j)
        # Region 2
        if (i + reg_size + j) % 2 == 0 and i + j < reg_size:
            spm[i + reg_size, i + reg_size + j] = np.log(i + reg_size + j + 1)
            spm[i + reg_size + j, i + reg_size] = np.log(i + reg_size + j + 1)
        # Region 3
        if i + 2 * j < reg_size:
            spm[i + reg_size, i + 2 * reg_size +
                2 * j] = np.log(i + 2 * reg_size + j + 1)
            spm[i + 2 * j + reg_size, i +
                2 * reg_size] = np.log(i + 2 * reg_size + j + 1)
            spm[i + 2 * reg_size, i + 2 * j +
                reg_size] = np.log(i + 2 * reg_size + j + 1)
            spm[i + 2 * reg_size + 2 * j, i +
                reg_size] = np.log(i + 2 * reg_size + j + 1)

b = np.arange(N)
c = np.einsum("ij,j", spm, b)
np.set_printoptions(precision=4)
print(spm)
print(b)
print(c)

plt.figure()
sns.heatmap(spm)
plt.axis('off')
# plt.show()
plt.savefig("Sparse_pattern.png")
