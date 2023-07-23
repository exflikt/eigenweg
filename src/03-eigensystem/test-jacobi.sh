#!/bin/sh

set -e

for n in $(seq 50 50 500) $(seq 600 100 1000); do
	echo "[${n}x${n} matrix]"
	for id in $(seq 9); do
		echo "* $id"
		src=${id}-${n}x${n}.csv
		./jacobi-method/jacobi input/mat$src output/jacobi-vals$src output/jacobi-vecs$src
		python3 compute-error.py jacobi $n $id
	done
	echo
done
