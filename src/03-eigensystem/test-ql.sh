#!/bin/sh

set -e

for n in 10 20 50 100 200 500 1000; do
	echo "[${n}x${n} matrix]"
	for id in $(seq 9); do
		echo "* $id"
		src=${id}-${n}x${n}.csv
		./ql-method/ql input/mat$src output/ql-vals$src output/ql-vecs$src
		python3 compute-error.py ql $n $id
	done
	echo
done
