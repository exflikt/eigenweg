計測用のフォルダです．
### 準備
JSONを読み込むライブラリ ``jq`` をインストールする必要がある．
```
sudo apt -y install jq
```
### ファイル構成
- ``data_generator.cpp`` : ランダム生成した $N\times(N+1)$ 次行列を生成する．乱数はメルセンヌ・ツイスタ．
- ``exact_equation.py`` : Pythonで解を求めるファイル．
- ``measure.sh`` : 計測するシェルスクリプト
- ``settings.json`` : 計測結果や行列を格納するファイルの設定
### 計測方法
1. ``settings.json`` を編集する．
2. ``measure.sh`` で繰り返し数，最大の $N$ ( ``MAX`` で定義)などを編集する．
3. 次のコマンドを実行する．
    ```
    bash measure.sh
    ```