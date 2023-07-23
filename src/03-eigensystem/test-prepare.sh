#!/bin/sh

set -e

[ ! -d input ] && mkdir input
[ ! -d answers ] && mkdir answers
[ ! -d output ] && mkdir output

for n in 10 20 50 100 200 500 1000; do
	for id in $(seq 9); do
		echo "Generating input/mat${id}-${n}x${n}.csv output/{vals,vecs}${id}-${n}x${n}.csv"
		python3 generate-symmat.py $n $id
	done
done

