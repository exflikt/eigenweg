Status: 動作テスト中

# 実行方法

このディレクトリで `make` コマンドを実行してコンパイルできます．以下のように mat.csv というCSVファイルを引数としてバイナリを実行すると，固有値が vals.csv に，固有ベクトルが vecs.csv に保存されます．アルゴリズムの性質上，対称行列しか計算できないので，非対称行列のデータを入れるとエラーになります．

```
make
./jacobi mat.csv vals.csv vecs.csv
```

計算結果をコンソールに出力したい場合は，デバッグバイナリをコンパイルして実行できます．

```
make compile-dbg
./jacobi-dbg mat.csv vals.csv vecs.csv
```

# テスト

```
ls test | grep -Ev '^(vals|vecs)' | while read f; do
    echo "\n[test/$f]"
    ./jacobi-dbg "test/$f" "test/vals-$f" "test/vecs-$f"
done &>test.log
```

* [test/example-3.csv](test/example-3.csv): 6.2 例3 (1) 139 p.
* [test/problem-8-1.csv](test/problem-8-1.csv): 6.2 問8 (1) 139 p.
* [test/problem-8-2.csv](test/problem-8-2.csv): 6.2 問8 (2) 139 p.
* [test/problem-8-3.csv](test/problem-8-3.csv): 6.2 問8 (3) 139 p.
* [test/exercise-example-2-1.csv](test/exercise-example-2-1.csv): 6章 例題２ (1) 148 p.
* [test/exercise-example-2-2.csv](test/exercise-example-2-2.csv): 6章 例題２ (2) 148 p.
* [test/exercise-problem-2-1.csv](test/exercise-problem-2-1.csv): 6章 演習２ (1) 149 p.
* [test/exercise-problem-2-2.csv](test/exercise-problem-2-2.csv): 6章 演習２ (2) 149 p.
* [test/exercise-problem-2-3.csv](test/exercise-problem-2-3.csv): 6章 演習２ (3) 149 p.

固有値は降順にソートされている．
固有ベクトルは正規化（ノルムを1に）していて，また符号が逆になっている可能性があるので注意する．

