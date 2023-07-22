#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

// 行列をデータ構造として定義しているヘッダファイルです．
//
// Mat型や，read_csv(), save_csv() 関数が実装されています．
// 具体的な使い方はexample.cppを参照してください．
//
// 用途に沿うよう，適当にコピペして改変してもらって結構です．
// このヘッダファイルを全く使わずに自分のやりたいようにやってもらってもいい
// ですが，テストしやすいようにCSVファイルの読み書きの仕様はそろえたいです．
//
// > 本課題は1ファイルで完結するようなアルゴリズムのプログラムを書くことが目的
// > なので，開放的でデバッグしやすいstructを用いました．
// >
// > C++ではclassとstructには機能面でほぼ違いがありません．異なるのは使用感のみ
// > です．structのフィールドやメソッドはデフォルトでpublic宣言されます．ある程
// > 度の規模のソフトウェア開発だったらデフォルトでprivate宣言されるclassを使っ
// > た方がプログラムの依存関係を管理しやすいでしょう．
namespace eigenweg {

// ==========
// = 行列型 =
// ==========

// SeqMatrix: メモリの連続した範囲にデータを保存する ROWS x COLS 行列
//
// 行列の要素が1行目，2行目，3行目... n行目
// と連なって（Sequential）保存されたデータ型である．
// 要素の型は倍精度浮動小数点（double）である．
struct SeqMatrix {
  // 1次元の std::vector で行列を表現する
  // (row, col) 要素は data[row * COLS + col] に対応している．
  // (ただし，0 ≤ row ≤ ROWS - 1, 0 ≤ col ≤ COLS - 1)
  const std::vector<double> m_data;
  const int m_rows; // 行（row）の次数
  const int m_cols; // 列（column）の次数

  SeqMatrix(const int rows, const int cols)
      : m_data(rows * cols), m_rows(rows), m_cols(cols) {}

  template <std::size_t ROWS, std::size_t COLS>
  SeqMatrix(const double (&array)[ROWS][COLS])
      : m_data(ROWS * COLS), m_rows(ROWS), m_cols(COLS) {
    for (int r = 0; r < m_rows; r++) {
      for (int c = 0; c < m_cols; c++) {
        (*this)[r][c] = array[r][c];
      }
    }
  }

  // 添字アクセス  (*this)[ith_row]
  inline double *operator[](const int ith_row) {
    return (double *)m_data.data() + ith_row * m_cols;
  }
  inline const double *operator[](const int ith_row) const {
    return m_data.data() + ith_row * m_cols;
  };

  // 行列どうしの等価性  *this == rhs
  inline bool operator==(const SeqMatrix &rhs) {
    if (this->m_rows != rhs.m_rows || this->m_cols != rhs.m_cols) {
      throw std::string("ERROR: 行列の次数が異なります．");
    }

    for (int r = 0; r < m_rows; r++) {
      for (int c = 0; c < m_cols; c++) {
        if ((*this)[r][c] != rhs[r][c]) {
          return false;
        }
      }
    }
    return true;
  }

  // 行列どうしの非等価性（等価性の否定） *this != rhs
  inline bool operator!=(const SeqMatrix &rhs) { return !(*this == rhs); }

  // 行列加算  *this += rhs
  //
  // なお，自身の行列 *this と他の行列 rhs を足し合わせた res = *this + rhs を
  // 計算する加算メソッドを定義することもできるが，その場合新たにメモリ領域を確
  // 保しなければならなず，計算時間に影響する．必要なとき以外はなるべく上書きす
  // る形で加算を計算した ほうが効率がよい．
  inline SeqMatrix &operator+=(const SeqMatrix &rhs) {
    if (this->m_rows != rhs.m_rows || this->m_cols != rhs.m_cols) {
      // 行列の次数が異なっても加算したい場合はここの処理を変えるとよい．
      // 例えば，被加算行列が3 x 4行列で加算行列が3 x 3行列のとき，
      // 最後の1列を無視して最初の3列だけ加算することが可能である．
      throw std::string("ERROR: 行列の次数が異なります．");
    }

    for (int r = 0; r < m_rows; r++) {
      for (int c = 0; c < m_cols; c++) {
        (*this)[r][c] += rhs[r][c];
      }
    }
    return *this;
  }

  // 行列減算  *this -= rhs
  inline SeqMatrix &operator-=(const SeqMatrix &rhs) {
    if (this->m_rows != rhs.m_rows || this->m_cols != rhs.m_cols) {
      throw std::string("ERROR: 行列の次数が異なります．");
    }

    for (int r = 0; r < m_rows; r++) {
      for (int c = 0; c < m_cols; c++) {
        (*this)[r][c] -= rhs[r][c];
      }
    }
    return *this;
  }

  // 行列積  res = *this · rhs
  //
  // このメソッドを実行すると，計算結果の行列（res）を保存するためのメモリ領域
  // を確保しなければならない．そのため，頻繁に呼び出さないように注意する必要が
  // ある．
  SeqMatrix dot(const SeqMatrix &rhs) {
    if (this->m_cols != rhs.m_rows) {
      throw std::string(
          "ERROR: 左の列数と右の行数が一致しないため，行列積が存在しません．");
    }

    int res_rows = this->m_rows, vec_len = this->m_cols, res_cols = rhs.m_cols;
    SeqMatrix res = SeqMatrix::zero(res_rows, res_cols);
    for (int r = 0; r < res_rows; r++) {
      for (int i = 0; i < vec_len; i++) {
        for (int c = 0; c < res_cols; c++) {
          res[r][c] += (*this)[r][i] * rhs[i][c];
        }
      }
    }
    return res;
  }

  // n x n 単位行列
  static const SeqMatrix id(int n) {
    SeqMatrix mat(n, n);
    for (int i = 0; i < n; i++) {
      mat[i][i] = 1.0;
    }
    return mat;
  }

  // rows x cols 零行列
  static const SeqMatrix zero(int rows, int cols) {
    SeqMatrix mat(rows, cols);
    for (int r = 0; r < rows; r++) {
      for (int c = 0; c < cols; c++) {
        mat[r][c] = 0.0;
      }
    }
    return mat;
  }

  // デバッグ用に行列全体を出力
  void dbg_print() {
    for (int r = 0; r < m_rows; r++) {
      for (int c = 0; c < m_cols; c++) {
        std::clog << (*this)[r][c] << ' ';
      }
      std::cerr << std::endl;
    }
  }
};

// OooMatrix: 各行ベクトルをヒープの至るところに配置する ROWS x COLS 行列
//
// 各行に対してスマートポインタ（vector<double>）を対応させているので，データの
// 保存場所はバラバラで順番が揃っていない（Out of order）．
// OooMatrixはSeqMatrixと違って，メモリキャッシュ局所性を有効活用できない．
// ただし，行の入れ替えは単にポインタの入れ替えで済むのでO(1)である．
// 要素の型は倍精度浮動小数点（double）である．
struct OooMatrix {
  // std::vector を入れ子にして行列を表現する
  const std::vector<std::vector<double>> m_data;
  const int m_rows; // 行（row）の次数
  const int m_cols; // 列（column）の次数

  OooMatrix(int rows, int cols)
      : m_data(rows, std::vector<double>(cols)), m_rows(rows), m_cols(cols) {}

  template <std::size_t ROWS, std::size_t COLS>
  OooMatrix(const double (&array)[ROWS][COLS])
      : m_data(ROWS * COLS), m_rows(ROWS), m_cols(COLS) {
    for (int r = 0; r < m_rows; r++) {
      for (int c = 0; c < m_cols; c++) {
        (*this)[r][c] = array[r][c];
      }
    }
  }

  // 添字アクセス  (*this)[ith_row]
  std::vector<double> operator[](const int ith_row) { return m_data[ith_row]; }
  const std::vector<double> operator[](const int ith_row) const {
    return m_data[ith_row];
  }

  // 添字アクセス以外の他のメソッドは SeqMatrix型 と同様に定義できる．
  // ...
};

// デフォルトの行列型 Mat はメモリの一部分に密集して保存する SeqMatrix型
using Mat = SeqMatrix;
// デフォルトのベクトル型 Vec は標準ライブラリの vector<double>
using Vec = std::vector<double>;

// ==============
// = 入出力関数 =
// ==============

static const char NEWLINE_DELIM = '\n';

// CSVファイルを読み込んで行列に変換する関数
//
// 以下のようなCSVファイル名を文字列で受け取って，3 x 4 の行列を読み込む．
//
// ```test/test1.csv
// 3,4
// 1.1,2.2,3.3,4.4
// 5.5,6.6,7.7,8.8
// 9.9,10.1,11.11,12.12
// ```
//
// １行目は行の次数と列の次数を指定する．
// ２〜４行目には行列の要素を浮動小数点で指定する．
//
// ファイルの入力を取り扱うので，随所で例外処理を行っている．文字列をスローする
// ので，catch ブロックは std::string を受け取ることができる．
inline Mat read_csv(std::string filename, const char col_delim = ',') {
  // 入力ファイルストリームの作成
  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    throw std::string("ERROR: CSVファイル " + filename +
                      " が開けませんでした．");
  }

  // CSVの１行目から行の次数 row と列の次数 col を読み取る
  std::string str_buf;
  std::getline(ifs, str_buf, col_delim);
  int row = stoi(str_buf);
  std::getline(ifs, str_buf, NEWLINE_DELIM);
  int col = stoi(str_buf);

  // CSVの２行目以降から行列 mat の要素を読み取る
  Mat mat(row, col);
  std::string col_buf;
  for (int r = 0; r < mat.m_rows; r++) {
    // １行読み取る
    std::getline(ifs, str_buf, NEWLINE_DELIM);
    std::istringstream line_iss(str_buf);
    for (int c = 0; c < mat.m_cols; c++) {
      // １要素読み取る
      std::getline(line_iss, col_buf, col_delim);
      // 例外: r 行目の最後まで読み切ったのに，行数が足りない
      if ((line_iss.rdstate() & std::ios_base::eofbit) && c != mat.m_cols - 1) {
        throw(std::ostringstream() << "ERROR: " << r + 1 << "行の" << c + 1
                                   << "列目以降の要素の値が読み取れません．")
            .str();
      }
      // 例外: cols 行まで読み取ったのにまで後続のフィールドが存在する
      if (!(line_iss.rdstate() & std::ios_base::eofbit) &&
          c == mat.m_cols - 1) {
        throw(std::ostringstream()
              << "ERROR: " << r + 1 << "行目の列の次数 " << mat.m_cols
              << " 以降に余分な値が存在します．")
            .str();
      }
      // 文字列から double 型への変換
      try {
        mat[r][c] = std::stod(col_buf);
      } catch (std::invalid_argument const &e) {
        throw(std::ostringstream()
              << "ERROR: " << r + 1 << "行" << c + 1 << "列の要素の値 \""
              << col_buf << "\" はdouble型に変換できません．")
            .str();
      }
    }
  }
  return mat;
}

// 行列をCSVファイルとして保存する関数
//
// ファイルの出力を取り扱うので，例外処理を行っている．文字列をスローするので，
// catch ブロックは std::string を受け取ることができる．
inline void save_csv(std::string filename, Mat mat,
                     const char col_delim = ',') {
  // 出力ファイルストリームの作成
  std::ofstream ofs(filename);
  if (!ofs.is_open()) {
    throw std::string("ERROR: 書込ファイル " + filename +
                      " が開けませんでした．");
  }

  // 1行目に行の次数と列の次数を書き込む．
  ofs << mat.m_rows << col_delim << mat.m_cols << NEWLINE_DELIM;

  // 2行目以降に行列の要素を書き込む．
  if (/* mat.m_rows > 0 && */ mat.m_cols > 0) {
    ofs << std::setprecision(std::numeric_limits<double>::max_digits10);
    for (int r = 0; r < mat.m_rows; r++) {
      ofs << mat[r][0];
      for (int c = 1; c < mat.m_cols; c++) {
        ofs << col_delim << mat[r][c];
      }
      ofs << NEWLINE_DELIM;
    }
  }

  ofs << std::flush;
}

} // namespace eigenweg
