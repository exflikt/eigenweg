下準備として，入力ファイルと正答ファイルを生成するときは以下のコマンドを実行してください．
このスクリプトは，n = 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 に対して n x n 行列をそれぞれ9個ずつ作成します．

```
./test-prepare.sh
```

ヤコビ法やQL法の誤差を調べたい場合は以下のコマンドを実行してください．

```
./test-jacobi.sh
./test-ql.sh
```

具体的な固有値や固有ベクトルの中身の出力を抑えたい場合は，以下のように標準エラー出力を消すことができます．

```
./test-jacobi.sh 2>/dev/null
./test-ql.sh 2>/dev/null
```
