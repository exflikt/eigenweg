#!/usr/bin/env python3

import random
import sys
import numpy as np

[n, id] = sys.argv[1:]

n = int(n)
id = int(id)

matrix = [[0 for _ in range(n)] for _ in range(n)]

random.seed()
eig_vals = random.sample(range(-10 * n, 10 * n), n)
vals_mat = np.diag(eig_vals)

# Generate a random orthogonal matrix for eigenvectors
orthogonal_mat, _ = np.linalg.qr(np.random.rand(n, n))

# Form the symmetric matrix A = P * D * P^T, where P is orthogonal and D is diagonal
sym_mat = np.dot(np.dot(orthogonal_mat, vals_mat), orthogonal_mat.T)

sm_p = np.finfo(sym_mat.dtype).precision
np.savetxt(f"input/mat{id}-{n}x{n}.csv", sym_mat, fmt=f'%{sm_p}e', header=f"{n},{n}", comments='', delimiter=',')
np.savetxt(f"answers/vals{id}-{n}x{n}.csv", np.array(eig_vals).reshape(1, -1), header=f"1,{n}", comments='', delimiter=',')
om_p = np.finfo(orthogonal_mat.dtype).precision
np.savetxt(f"answers/vecs{id}-{n}x{n}.csv", orthogonal_mat, fmt=f'%{om_p}e', header=f"{n},{n}", comments='', delimiter=',')
