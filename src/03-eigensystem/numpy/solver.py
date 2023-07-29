#!/usr/bin/env python3
#
# 固有値と固有ベクトルを numpy.linalg.eig を用いて計算する
#
# Usage:
#       ./solver.py input-file output-vals-file output-vecs-file

import sys
import numpy as np

[input_path, vals_path, vecs_path] = sys.argv[1:]

def eigen_sort(vals, vecs):
    vecs_sorted = vecs[:, vals.argsort()[::-1]] # sort by the order of eigenvalues
    vals_sorted = np.sort(vals)[::-1]
    return vals_sorted, vecs_sorted

mat = np.genfromtxt(input_path, skip_header=1, delimiter=',')
np_vals, np_vecs = np.linalg.eig(mat)
np_vals, np_vecs = eigen_sort(np_vals, np_vecs)

print('np_vals :')
print(np_vals)
print(f'np_vecs :')
print(np_vecs)

n = np_vals.size

np.savetxt(vals_path, np_vals.reshape(1, -1), header=f"1,{n}", comments='', delimiter=',')
vecs_p = np.finfo(np_vecs.dtype).precision
np.savetxt(vecs_path, np_vecs, fmt=f'%{vecs_p}e', header=f"{n},{n}", comments='', delimiter=',')
