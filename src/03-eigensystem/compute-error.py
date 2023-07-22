# numpy.linalg.eig 関数 と入力したファイルの誤差を計算するスクリプト
#
# Usage:
#       python compute-error.py matrix.csv
#
# matrix.csv が入力の行列で，vals-matrix.csv に固有値が，vecs-matrix.csv に固有
# ベクトルが保存されていると想定しています．

import os
import sys

import numpy as np

[filepath] = sys.argv[1:]

def eigen_sort(vals, vecs):
    vecs_sorted = vecs[:, vals.argsort()] # sort by the order of eigenvalues
    vals_sorted = np.sort(vals)
    return vals_sorted, vecs_sorted

matrix = np.genfromtxt(filepath, skip_header=1, delimiter=',')
vals_np, vecs_np = np.linalg.eig(matrix)
vals_np, vecs_np = eigen_sort(vals_np, vecs_np)

filedir, filename = os.path.split(filepath)
vals_file = f"{filedir}{'' if filedir == '' else '/'}vals-{filename}"
vecs_file = f"{filedir}{'' if filedir == '' else '/'}vecs-{filename}"
vals = np.genfromtxt(vals_file, skip_header=1, delimiter=',')
vecs = np.genfromtxt(vecs_file, skip_header=1, delimiter=',')
vals, vecs = eigen_sort(vals, vecs)

print('vals_np :')
print(vals_np)
print(f'{vals_file} :')
print(vals)

print('vecs_np :')
print(vecs_np)
print(f'{vecs_file} :')
print(vecs)

n = vals.size
# 二乗平均平方根 (Root Mean Square)
rms = lambda array : np.sqrt(np.sum(array ** 2) / n)

print()
print('--- Result ---')
print('vals_rms = ', rms(vals_np - vals))
vecs_rms_total = 0
for i in range(n):
    # 固有ベクトルは正負が反対になっている可能性があるので
    # 符号を入れ替えて最も小さい値を選ぶ
    a = rms(+ vecs_np[:, i] + vecs[:, i])
    b = rms(- vecs_np[:, i] + vecs[:, i])
    c = rms(+ vecs_np[:, i] - vecs[:, i])
    print(f'rms of vecs[:, {i}] = {min(a, b, c)}')
    vecs_rms_total += min(a, b, c)
print(f'vecs_rms_avg = {vecs_rms_total / n}')
print('--------------')
