#!/bin/sh

set -e

[ ! -d input ] && mkdir input
[ ! -d answers ] && mkdir answers
[ ! -d output ] && mkdir output

for n in $(seq 50 50 500) $(seq 600 100 1000); do
	for id in $(seq 9); do
		echo "Generating input/mat${id}-${n}x${n}.csv output/{vals,vecs}${id}-${n}x${n}.csv"
		python3 generate-symmat.py $n $id
	done
done

