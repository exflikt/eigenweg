計測用のフォルダです．
### 準備
JSONを読み込むライブラリ ``jq`` をインストールする必要がある．
```
sudo apt -y install jq
```
### ファイル構成
- ``data_generator.cpp`` : ランダム生成した $N$ 次行列を生成する．乱数はメルセンヌ・ツイスタ．
- ``exact_determinant.py`` : Pythonで行列式を求めるファイル．
- ``measure.sh`` : 計測するシェルスクリプト
- ``settings.json`` : 計測結果や行列を格納するファイルの設定
### 計測方法
1. ``settings.json`` を編集する．
2. ``measure.sh`` で繰り返し数などを編集する．
3. 次のコマンドを実行する．
    ```
    bash measure.sh
    ```