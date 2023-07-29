#!/bin/sh

echo > test.log
ls test | grep -Ev '^(vals|vecs)' | while read f; do
    echo "\n[test/$f]" >> test.log
    ./solver.py "test/$f" "test/vals-$f" "test/vecs-$f" >> test.log
done
