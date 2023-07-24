### ファイル構成
results/data.csv : 拡大係数行列
```
N
a(11) a(12) ... a(1 N+1)
a(21) a(22) ... a(2 N+1)
...   ...   ... ...
a(N1) a(N2) ... a(N N+1)
```
results/equation.csv：解  
measure.sh で実行した場合，解ベクトルの要素数を統一するため， $N$ の最大数 ``MAX`` を導入している． $x$ ， `x` はcppで求めた解， $x'$ ， `x'` はPythonで求めた解である． $x_a\ (a\gt N)$ や $x'_a\ (a\gt N)$ は `0` として出力する．
```
N seed x1 x2 ... xMAX x'1 x'2 ... x'MAX
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
  "processor": "../02_Gaussian/processor",
  "data_file": "../02_Gaussian/results/data.csv",
  "equation_record": "../02_Gaussian/results/equation.csv",
  "clock_record": "../02_Gaussian/results/clock.csv"
}
```
次のコマンドを実行する．(measure.sh 内で，``N`` や ``seed`` ， `MAX` を変更できます)
```
bash measure.sh
```
##### 直接行う場合
``02_triangular_method/data.csv`` を変更する．  
``02_triangular_method`` において，次のコマンドを実行する．拡大係数行列がランダムに生成され，解が複数ある可能性が限りなく小さい場合，解のみを出力する次のコマンドを実行する．
```
./processor results/data.csv 1
```
解が複数存在する可能性がある場合や無限に存在する場合，存在しない場合を含むような拡大係数行列を渡す場合，解の詳細を出力する次のコマンドを実行する．
```
./processor results/data.csv 0
```
出力は５つに分かれる．  

- 非斉次，ただ一つの解
```math
A_+=\begin{pmatrix}
1 & 1 & 1 & 10 \\
1 & 4 & 2 & 17 \\
2 & 3 & 1 & 17 
\end{pmatrix}
```
```math
\mathbf{x}=\begin{pmatrix}
5 \\ 1 \\ 4
\end{pmatrix}
```
  ```
  非斉次，ただ１つの解を持つ．
  解:
  5 1 4 
  処理時間：
  6e-06
  ```
- 非斉次，無数の解
```math
A_+=\begin{pmatrix}
1 & 2 & 3 & 4 \\
2 & 3 & 4 & 5 \\
3 & 4 & 5 & 6 
\end{pmatrix}$$
```
```math
\begin{pmatrix}
x_1 \\ x_2
\end{pmatrix}=\begin{pmatrix}
1 \\ -2
\end{pmatrix}x_3+\begin{pmatrix}
-2 \\ 3
\end{pmatrix}
```
  ```
  非斉次，無数の解を持つ．
  解の自由度: 1
  未知数:
  1 2 
  に対する解は，別の未知数と定数：
  3 1(const.)
  と，次の列ベクトルの線形結合で表せる．
  1 -2 
  -2 3 
  処理時間：
  5e-06
  ```
- 解なし
```math
A_+=\begin{pmatrix}
1 & 2 & 3 & 4 \\
2 & 4 & 6 & 6 \\
2 & 6 & 9 & 9 
\end{pmatrix}$$
```
  ```
  解なし
  係数行列のrank: 2，拡大係数行列のrank: 3
  処理時間：
  1.1e-05
  ```
- 斉次，非自明解をもつ
```math
A_+=\begin{pmatrix}
1 & 2 & 3 & 0 \\
2 & 3 & 4 & 0 \\
3 & 4 & 5 & 0 
\end{pmatrix}
```
```math
\begin{pmatrix}
x_1 \\ x_2
\end{pmatrix}=\begin{pmatrix}
1 \\ -2
\end{pmatrix}x_3
```
  ```
  斉次，非自明解を持つ．
  解の自由度: 1
  未知数：
  1 2 
  に対する解は，別の未知数：
  3 
  と，次の列ベクトルの線形結合で表せる．
  1 -2 
  処理時間：
  4e-06
  ```
- 斉次，自明解のみをもつ
```math
A_+=\begin{pmatrix}
1 & 1 & 1 & 0 \\
1 & 2 & 1 & 0 \\
2 & 1 & 0 & 0 
\end{pmatrix}
```
$$\mathbf{x}=\mathbf{0}$$
  ```
  斉次，自明解のみを持つ．
  処理時間：
  5e-06
  ```
### 理論
拡大係数行列を行に関する基本変形を行うことによって，行階段形を作る．係数行列が単位行列に変換できれば拡大係数行列の一番右側の列が解となる．単位行列にならない場合は拡大係数行列と係数行列のランクを比較し，解を判定する.
- 拡大係数行列: $A_+=\{a_{ij}\}$
- 係数行列の次数: $N$ 
- 対角線上の要素: $\mathrm{pivot}$  
- 拡大係数行列の列方向の次数: $N_{col}$

1. 非斉次である場合 $N_{col}=N+1$ ，斉次である場合 $N_{col}=N$
2. $i\gets 0$
3. $\mathrm{for}\ j\ (1\le j\le N)$ :
    1. $i\le i_2\le N$ に対し， $a_{i_2j}\neq0$ である $i_2$ を探し， $i$ 行と $i_2$ 行を `swap` する．そのような $i_2$ がない場合，`continue` する．
    2. $\mathrm{pivot}\gets a_{i_2j}$
    3. $\mathrm{for}\ j_2\ (j+1\le j_2\le N_{col})$ :
        1. $\displaystyle a_{ij_2}\gets \frac{a_{ij_2}}{\mathrm{pivot}}$
    4. $\mathrm{for}\ i_2\ (1\le i_2\le N, i_2\neq i)$ :
        1. $\mathrm{for}\ j_2\ (j+1\le j_2\le N_{col})$ :
            1. $a_{i_2j_2}\gets a_{i_2j_2} - a_{ij_2}\cdot a_{i_2j}$
    5. $i\gets i+1$
4. $A_+\sim(E\mid\mathbf{b})$ となれば，解は $\mathbf{b}$ である．
