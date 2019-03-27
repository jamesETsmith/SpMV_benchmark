import numpy as np

N = 5
diag = 3
off_diag = 1.7
off_off_diag = 1.5

a = np.zeros((N, N))
for i in range(N):
    a[i, i] = diag * (i + 1)
    if i + 1 < N:
        a[i, i + 1] = off_diag * (i + 1)
        a[i + 1, i] = off_diag * (i + 1)
        if i + 2 < N:
            a[i, i + 2] = off_off_diag / (i + 1)
            a[i + 2, i] = off_off_diag / (i + 1)
print(a)
b = np.arange(N)

print(np.einsum("ij,j", a, b))
