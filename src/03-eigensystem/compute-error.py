#!/usr/bin/env python3
#
# 入力したファイルと正当の誤差を計算するスクリプト
#
# Usage:
#       ./compute-error.py method n
#
# * method: jacobi or ql
# * n: 10, 20, 50, 100, 200, 500, 1000

import sys

import numpy as np

[method, n, id] = sys.argv[1:]

n = int(n)
id = int(id)

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

def eigen_sort(vals, vecs):
    vecs_sorted = vecs[:, vals.argsort()] # sort by the order of eigenvalues
    vals_sorted = np.sort(vals)
    return vals_sorted, vecs_sorted

# mat = np.genfromtxt(f'input/mat-{n}x{n}.csv', skip_header=1, delimiter=',')
# np_vals, np_vecs = np.linalg.eig(mat)
# np_vals, np_vecs = eigen_sort(np_vals, np_vecs)

ans_vals = np.genfromtxt(f'answers/vals{id}-{n}x{n}.csv', skip_header=1, delimiter=',')
ans_vecs = np.genfromtxt(f'answers/vecs{id}-{n}x{n}.csv', skip_header=1, delimiter=',')
ans_vals, ans_vecs = eigen_sort(ans_vals, ans_vecs)

vals_file = f"output/{method}-vals{id}-{n}x{n}.csv"
vecs_file = f"output/{method}-vecs{id}-{n}x{n}.csv"
vals = np.genfromtxt(vals_file, skip_header=1, delimiter=',')
vecs = np.genfromtxt(vecs_file, skip_header=1, delimiter=',')
vals, vecs = eigen_sort(vals, vecs)

eprint('ans_vals :')
eprint(ans_vals)
eprint(f'{vals_file} :')
eprint(vals)

eprint('ans_vecs :')
eprint(ans_vecs)
eprint(f'{vecs_file} :')
eprint(vecs)

n = vals.size
# 二乗平均平方根 (Root Mean Square)
rms = lambda array : np.sqrt(np.sum(array ** 2) / n)

eprint()
eprint('--- Result ---')
print(f'vals_rms: {rms(ans_vals - vals)}')
vecs_rms_total = 0
for i in range(n):
    # 固有ベクトルは正負が反対になっている可能性があるので
    # 符号を入れ替えて最も小さい値を選ぶ
    a = rms(+ ans_vecs[:, i] + vecs[:, i])
    b = rms(- ans_vecs[:, i] + vecs[:, i])
    c = rms(+ ans_vecs[:, i] - vecs[:, i])
    eprint(f'rms of vecs[:, {i}] = {min(a, b, c)}')
    vecs_rms_total += min(a, b, c)
print(f'vecs_rms_avg: {vecs_rms_total / n}')
eprint('--------------')
