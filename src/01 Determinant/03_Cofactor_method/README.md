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
### 計測方法
##### シェルスクリプトで行う場合
``00_Experiments`` に移動し，``settings.json`` を次のように変更する．
```
{
  "processor": "../02_triangular_method/processor",
  "data_file": "../02_triangular_method/results/data.csv",
  "determinant_record": "../02_triangular_method/results/determinant.csv",
  "clock_record": "../02_triangular_method/results/clock.csv"
}
```
次のコマンドを実行する．(measure.sh 内で，``N`` や ``seed`` を変更できます)
```
bash measure.sh
```
##### 直接行う場合
``02_triangular_method/data.csv`` を変更する．  
``02_triangular_method`` において，次のコマンドを実行する．
```
./processor results/data.csv
```

### 理論
上三角行列を作り，対角線上の要素の総乗を出力する．  
- 行列: $A=\{a_{ij}\}$
- 行列の次数: $N$
- 行列式: $\mathrm{determinant\gets 1}$  
- 対角線上の要素: $\mathrm{pivot}$  

1. $\mathrm{for}\ c\ (1\le c\le N)$ :  
    1. $\mathrm{if}\ a_{cc} = 0$ :  
        1. $a_{ic} \neq 0\ (i \gt c)$ となる $i$ を探し， $A$ の行 $c$ と $i$ を ``swap`` する． $i$ が見つからない場合は行列式は $0$ として終了する．
        2.  $\mathrm{determinant}\gets -\mathrm{determinant}$
    2. $\mathrm{pivot} \gets a_{cc}$
    3. $\mathrm{determinant}\gets \mathrm{determinant} \cdot \mathrm{pivot}$
    4. $\mathrm{for}\ i\ (c\lt i\le N)$ :  
        1. $\mathrm{for}\ j\ (c\lt j\le N)$ :  
            1. $\displaystyle a_{ij}\gets a_{ij}-\frac{a_{cj}\cdot a_{ic}}{\mathrm{pivot}}$  
2. $\mathrm{determinant}$ を行列式として出力する．
