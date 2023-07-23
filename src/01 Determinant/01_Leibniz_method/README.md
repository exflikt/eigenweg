※"01 Leibniz method" を，実行しやすいように周りの部分だけ改変したものです．  
### ファイル構成
results/data.csv ：解く行列
```
N
a(11) a(12) ... a(1N)
a(21) a(22) ... a(2N)
...   ...   ... ...
a(N1) a(N2) ... a(NN)
```
results/determinant.csv：行列式  
```
N seed det(CPP) det(Python)
```
results/clock.csv：処理時間[s]  
```
N seed process_time[s]
```
### 実行時間
##### シェルスクリプト
``00_Experiments`` に移動し，``settings.json`` を次のように変更する．
```
{
  "processor": "../01_Leibniz_method/processor",
  "data_file": "../01_Leibniz_method/results/data.csv",
  "determinant_record": "../01_Leibniz_method/results/determinant.csv",
  "clock_record": "../01_Leibniz_method/results/clock.csv"
}
```
次のコマンドを実行する．(measure.sh 内で，``N`` や ``seed`` を変更できます)
```
bash measure.sh
```
##### 直接
``01_Leibniz_method/data.csv`` を変更する．  
``01_Leibniz_method`` において，次のコマンドを実行する．
```
./processor results/data.csv
```

### 理論
次のことをやってる
$$\det(A)=\sum_{\tau\in S_n}(\mathrm{sgn}\,\tau)\prod_{i=1}^na_{i,\tau(i)}$$