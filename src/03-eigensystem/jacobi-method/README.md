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

固有値は降順にソートされています．
固有ベクトルは正規化（ノルムを1に）していて，また符号が逆になっている可能性があるので注意してください．

---

# 理論

実対称行列に対して一連の直交相似変換を行うことによって対角化を行う．各変換はヤコビ回転（Jacobi Rotation）と呼ばれ，平面を回転させることで非対角要素の値を打ち消すことができる．連続して変換を行う過程で以前に零にした要素に非零の値が設定されてしまうことがあるが，繰り返し変換を行うことで非対角要素の値を全体的に小さくすることができる．このようにして，非対角要素をコンピュータで表現できない程小さくすることで，対角行列を近似することが出来る．

ヤコビ法によって得られた対角行列の対角要素が固有値に対応する．また，全ての変換行列の行列積をとることによって固有ベクトルが得られる．

> 次数が10を超えるとヤコビ法はQR方より著しく遅くなっていく．しかし，QR方法に比べてヤコビ方は容易に実装できる．

## ヤコビ回転

p軸とq軸に対する角度 $\phi$ のヤコビ回転行列 $P_{pq}(\phi)$ は以下の形で定義される．

$$
P_{pq}(\phi) = \begin{bmatrix}
1           \\
  & \ddots  \\
  &        & \cos\phi  & \dots  & \sin\phi \\
  &        & \vdots    & E      & \vdots   \\
  &        & -\sin\phi & \cdots & \cos\phi \\
  &        &           &        &          & \ddots \\
  &        &           &        &          &        & 1
\end{bmatrix}
= \begin{bmatrix}
1           \\
  & \ddots  \\
  &        & c      & \dots  & s      \\
  &        & \vdots & E      & \vdots \\
  &        & -s     & \cdots & c      \\
  &        &        &        &        & \ddots \\
  &        &        &        &        &        & 1
\end{bmatrix}
$$

対角成分と平面回転をする4成分以外は全て零である．
転置を取ったものとの積が単位行列となることから，$P_{pq}(\phi)$ は直交行列である．
なお，簡略記法として以降 $c = \cos\phi,\; s = \sin\phi$ を用いる．

このヤコビ回転行列を用いて行列Aの相似変換 $A \to {}^t P_{pq}(\phi) A P_{pq}(\phi)\;(= A')$ を行うと以下のようになる．

$$
A' = {}^t P_{pq}(\phi) A P_{pq}(\phi)
$$

$$
= \begin{bmatrix}
1           \\
  & \ddots  \\
  &        & c      & \dots  & -s     \\
  &        & \vdots & E      & \vdots \\
  &        & s      & \cdots & c      \\
  &        &        &        &        & \ddots \\
  &        &        &        &        &        & 1
\end{bmatrix}
\begin{bmatrix}
       &        & a_{1p} &        & a_{1q} &                 \\
       &        & \vdots &        & \vdots &                 \\
a_{p1} & \cdots & a_{pp} & \cdots & a_{pq} & \cdots & a_{pn} \\
       &        & \vdots &        & \vdots &                 \\
a_{q1} & \cdots & a_{qp} & \cdots & a_{qq} & \cdots & a_{qn} \\
       &        & \vdots &        & \vdots &                 \\
       &        & a_{np} &        & a_{nq} &
\end{bmatrix}
\begin{bmatrix}
1           \\
  & \ddots  \\
  &        & c      & \dots  & s      \\
  &        & \vdots & E      & \vdots \\
  &        & -s     & \cdots & c      \\
  &        &        &        &        & \ddots \\
  &        &        &        &        &        & 1
\end{bmatrix}
$$

$$
= \begin{bmatrix}
1           \\
  & \ddots  \\
  &        & c      & \dots  & -s     \\
  &        & \vdots & E      & \vdots \\
  &        & s      & \cdots & c      \\
  &        &        &        &        & \ddots \\
  &        &        &        &        &        & 1
\end{bmatrix}
\begin{bmatrix}
       &        & c a_{1p} - s a_{1q} &        & s a_{1p} + c a_{1q} &                 \\
       &        & \vdots              &        & \vdots              &                 \\
a_{p1} & \cdots & c a_{pp} - s a_{pq} & \cdots & s a_{pp} + c a_{pq} & \cdots & a_{pn} \\
       &        & \vdots              &        & \vdots              &                 \\
a_{q1} & \cdots & c a_{qp} - s a_{qq} & \cdots & s a_{qp} + c a_{qq} & \cdots & a_{qn} \\
       &        & \vdots              &        & \vdots              &                 \\
       &        & c a_{np} - s a_{nq} &        & s a_{np} + c a_{nq} &
\end{bmatrix}
$$

$$
= \begin{bmatrix}
                    &        & c a_{1p} - s a_{1q}                       &        & s a_{1p} + c a_{1q}                       &                              \\
                    &        & \vdots                                    &        & \vdots                                    &                              \\
c a_{p1} - s a_{q1} & \cdots & c^2 a_{pp} + s^2 a_{qq} - 2sc a_{pq}      & \cdots & sc (a_{pp} - a_{qq}) + (c^2 - s^2) a_{pq} & \cdots & c a_{pn} - s a_{qn} \\
                    &        & \vdots                                    &        & \vdots                                    &                              \\
s a_{p1} + c a_{q1} & \cdots & sc (a_{pp} - a_{qq}) + (c^2 - s^2) a_{pq} & \cdots & s^2 a_{pp} + c^2 a_{qq} + 2sc a_{pq}      & \cdots & s a_{qn} + c a_{pn} \\
                    &        & \vdots                                    &        & \vdots                                    &                              \\
                    &        & c a_{np} - s a_{nq}                       &        & s a_{np} + c a_{nq}                       &
\end{bmatrix}
$$

行p，行q，列p，列q のみ更新される．また，(p, p), (p, q), (q, p), (q, q) 要素の値が特殊で，それ以外の場所の計算は似通っている．まとめると以下のようになる．

$$
\begin{cases}
    a'_{pp} = c^2 a_{pp} + s^2 a_{qq} - 2sc a_{pq} \\
    a'_{qq} = s^2 a_{pp} + c^2 a_{qq} + 2sc a_{pq} \\
    a'_{pq} = a'_{qp} = sc (a_{pp} - a_{qq}) + (c^2 - s^2) a_{pq} \\
    \begin{cases}
        a'_{rp} = c a_{rp} - s a_{rq} \\
        a'_{rq} = s a_{rp} + c a_{rq}
    \end{cases} \quad r \ne p,\; r \ne q
\end{cases}
$$

ヤコビ法の目的は非対角要素 $a'_{pq}$ をゼロにすることである．つまり，

$$
sc (a_{pp} - a_{qq}) + (c^2 - s^2) a_{pq} = 0
\quad \therefore \;
\frac{a_{qq} - a_{pp}}{2a_{pq}} = \frac{c^2 - s^2}{2sc}
\left( = \frac{\cos 2\phi}{\sin 2\phi} =  \frac1{\tan 2\phi} = \cot 2\phi \right)
$$

となる．これを用いて，式変形する．

$$
\begin{cases}
    a'_{pp} = a_{pp} + (c^2 - 1) a_{pp} + s^2 a_{qq} - 2sc a_{pq} \\
    a'_{qq} = a_{qq} + s^2 a_{pp} + (c^2 - 1) a_{qq} + 2sc a_{pq} \\
    \begin{cases}
        a'_{rp} = a_{rp} + (c - 1) a_{rp} - s a_{rq} \\
        a'_{rq} = a_{rq} + s a_{rp} + (c - 1) a_{rq}
    \end{cases} \quad r \ne p,\; r \ne q \\
\end{cases}
$$

$$
\begin{cases}
    a'_{pp} = a_{pp} + s^2 (a_{qq} - a_{pp}) - 2sc a_{pq} \\
    a'_{qq} = a_{qq} - s^2 (a_{qq} - a_{pp}) + 2sc a_{pq} \\
    \begin{cases}
        a'_{rp} = a_{rp} + s (\frac{c - 1}s a_{rp} - a_{rq}) \\
        a'_{rq} = a_{rq} + s (a_{rp} + \frac{c - 1}s a_{rq})
    \end{cases} \quad r \ne p,\; r \ne q \\
\end{cases}
$$

$$
\begin{cases}
    a'_{pp} = a_{pp} + \frac sc (c^2 - s^2) a_{pq} - 2sc a_{pq} \\
    a'_{qq} = a_{qq} - \frac sc (c^2 - s^2) a_{pq} + 2sc a_{pq} \\
    \begin{cases}
        a'_{rp} = a_{rp} - s (a_{rq} + \frac{1 - c}s a_{rp}) \\
        a'_{rq} = a_{rq} + s (a_{rp} - \frac{1 - c}s a_{rq})
    \end{cases} \quad r \ne p,\; r \ne q \\
\end{cases}
$$

$$
\begin{cases}
    a'_{pp} = a_{pp} + \frac sc ((c^2 - s^2) - 2c^2) a_{pq} \\
    a'_{qq} = a_{qq} - \frac sc ((c^2 - s^2) - 2c^2) a_{pq} \\
    \begin{cases}
        a'_{rp} = a_{rp} - s (a_{rq} + \frac{1 - c^2}{s(1 + c)} a_{rp}) \\
        a'_{rq} = a_{rq} + s (a_{rp} - \frac{1 - c^2}{s(1 + c)} a_{rq})
    \end{cases} \quad r \ne p,\; r \ne q \\
\end{cases}
$$

$$
\begin{cases}
    a'_{pp} = a_{pp} - \frac sc a_{pq} = a_{pp} - t a_{pq} \\
    a'_{qq} = a_{qq} + \frac sc a_{pq} = a_{qq} + t a_{pq} \\
    \begin{cases}
        a'_{rp} = a_{rp} - s (a_{rq} + \frac{s}{1 + c} a_{rp})
                = a_{rp} - s (a_{rq} + \tau a_{rp}) \\
        a'_{rq} = a_{rq} + s (a_{rp} - \frac{s}{1 + c} a_{rq})
                = a_{rq} + s (a_{rp} - \tau a_{rq})
    \end{cases} \quad r \ne p,\; r \ne q \\
\end{cases}
$$

なお，$t = \tan \phi = \frac{\sin\phi}{\cos\phi},\;\tau = \frac{\sin\phi}{1 + \cos\phi} (= \tan \frac{\phi}2)$ とした．
回転変換の式が導出できたのはよいが，アルゴリズムの実装のために三角関数の値を明示的に表そう．
先程の $a'_{pq} = 0$ から導かれる等式を変形する．

$$
\cot 2\phi
= \frac{c^2 - s^2}{2sc}
= \frac{1 - t^2}{2t}
\quad \therefore \;
t^2 + 2t \cot 2\phi - 1 = 0
\quad \therefore \;
t = - \cot 2\phi \pm \sqrt{\cot^2 2\phi + 1}
$$

絶対値が小さいほうが $\phi$ も小さくなるので，そうなるように二次方程式の解 $t$ を選ぶと，

$$
t = \frac{sgn(\cot 2\phi)}{|\cot 2\phi| + \sqrt{\cot^2 2\phi + 1}}
$$

また，$t = \tan \phi$を用いると，$c = \cos\phi,\;s = \sin\phi$の値は以下で求められる．

$$
c = \frac1{1 + t^2}, \;
s = t \cdot c
$$

$\cot 2\phi$ は 変換前の行列の要素で表される（$\cot 2\phi = \frac{a_{qq} - a_{pp}}{2a_{pq}}$）ので，$tan \phi, cos \phi, sin \phi$ を三角関数を一切用いずに計算できることが示せた．

### 非対角要素の掃き出し

前節では，ヤコビ回転 $P_{pq}(\phi)$ を用いて非対角要素 $a_{pq}$ をゼロにする方法を述べた．
ヤコビ法における対角化の大胆な戦略は，非対角要素をゼロにすることである．そうなるようにヤコビ回転をし続けることで，対角行列が得られるのである．
実対称行列の全ての非対角要素に対してヤコビ回転を施すプロセスは"掃き出し" (Sweep) と呼ばれる．しかし，ヤコビ回転の更新式をみてもわかるように，全ての非対角化要素をゼロにしようとしても，他の要素の値が同時に更新されてしまう．幸い，ヤコビ回転を施す度に非対角要素はゼロに近づいていく．そのため，コンピュータで表せないほどの誤差まで小さくしたところでゼロに設定すれば掃き出しの繰り替えしで対角行列を近似することができる．

ヤコビ法は実対称行列に対してのみ使えるアルゴリズムである．そのため，上三角行列の非対角要素に対して回転を行う．つまり，1度の掃き出しで，$n \cdot (n - 1) / 2$ 個の要素のみ取り扱う．各回転変換にかかる計算量は回転の更新式から $O(n)$ であるので，ヤコビ法での1度の掃き出しにおける計算時間は $O(n^3)$ となる．
