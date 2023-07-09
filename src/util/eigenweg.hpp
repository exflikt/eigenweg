#include <fstream>
#include <iostream>
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

  // 添字アクセス
  inline double *operator[](const int ith_row) {
    return (double *)m_data.data() + ith_row * m_cols;
  }
  inline const double *operator[](const int ith_row) const {
    return m_data.data() + ith_row * m_cols;
  };

  // 行列どうしの等価性
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

  inline bool operator==(SeqMatrix &rhs) {
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

  // 行列どうしの非等価性（等価性の否定）
  inline bool operator!=(const SeqMatrix &rhs) { return !(*this == rhs); }

  // 行列加算（A += B）
  //
  // なお，自身の行列Aと他の行列Bを足し合わせた A + B を計算する加算メソッドを
  // 定義することもできるが，その場合新たにメモリ領域を確保しなければならないの
  // で計算時間に影響する．必要なとき以外はなるべく上書きする形で加算を計算した
  // ほうが効率がよい．
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

  // 行列減算（A -= B）
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

  // 行列積（C = A · B）
  //
  // このメソッドを実行すると，計算結果の行列を保存するためのメモリ領域を確保し
  // なければならない．そのため，頻繁に呼び出さないように注意する必要がある．
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

  // 単位行列
  static const SeqMatrix id(int n) {
    SeqMatrix mat(n, n);
    for (int i = 0; i < n; i++) {
      mat[i][i] = 1.0;
    }
    return mat;
  }

  // 零行列
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
// 各行に対してスマートポインタ（vector<double>）を対応させているので，
// データの保存場所はバラバラであり，順番が揃っていない（Out of order）．
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

  // 添字アクセス
  std::vector<double> operator[](const int ith_row) { return m_data[ith_row]; }
  const std::vector<double> operator[](const int ith_row) const {
    return m_data[ith_row];
  }

  // 添字アクセス以外の他のメソッドは SeqMatrix型 と同様に定義できる．
  // ...
};

// デフォルトの行列型 Mat はメモリの一部分に密集して保存する SeqMatrix型
typedef SeqMatrix Mat;
// デフォルトのベクトル型 Vec は標準ライブラリの vector<double>
typedef std::vector<double> Vec;

// ==============
// = 入出力関数 =
// ==============

static const char NEWLINE_DELIM = '\n';

// CSVファイルを読み込んで行列に変換する関数
//
// ファイル名を文字列で受け取って，以下のようなCSVファイルを読み込む．
//
// ```example.csv
// 3,4
// 1.1,2.2,3.3,4.4
// 5.5,6.6,7.7,8.8
// 9.9,10.1,11.11,12.12
// ```
//
// １行目は行の次数と列の次数を指定する．
// ２〜４行目には行列の要素を浮動小数点で指定する．
inline Mat read_csv(std::string filename, const char col_delim = ',') {
  std::ifstream ifs(filename);

  if (!ifs.is_open()) {
    throw std::string("ERROR: CSVファイル " + filename +
                      " が開けませんでした．");
  }

  std::string str_buf;
  std::getline(ifs, str_buf, col_delim);
  int row = stoi(str_buf);
  std::getline(ifs, str_buf, NEWLINE_DELIM);
  int col = stoi(str_buf);

  Mat mat(row, col);
  std::string col_buf;
  for (int r = 0; r < mat.m_rows; r++) {
    std::getline(ifs, str_buf, NEWLINE_DELIM);
    std::istringstream line_iss(str_buf);
    for (int c = 0; c < mat.m_cols; c++) {
      std::getline(line_iss, col_buf, col_delim);

      std::string err_msg =
          std::to_string(r + 1) + "行の" + std::to_string(c + 1) + "列目";
      // c行目の要素の値が読み取れない
      if (line_iss.rdstate() & std::ios_base::failbit) {
        throw std::string("ERROR: " + err_msg + "の要素の値が読み取れません．");
      }
      // r行目の最後まで読み切ったのに，行数が足りないとき
      if ((line_iss.rdstate() & std::ios_base::eofbit) && c != mat.m_cols - 1) {
        throw std::string("ERROR: " + err_msg +
                          "以降の要素の値が読み取れません．");
      }

      // 文字列からdouble型への変換
      try {
        mat[r][c] = std::stod(col_buf);
      } catch (std::invalid_argument const &e) {
        throw std::string("ERROR: ") + e.what() + err_msg + "の要素の値 \"" +
            col_buf + "\" はdouble型に変換できません．";
      }
    }
  }
  return mat;
}

// 行列をCSVファイルとして保存する関数
inline void save_csv(std::string filename, Mat mat,
                     const char col_delim = ',') {
  std::ofstream ofs(filename);

  if (!ofs.is_open()) {
    throw std::string("ERROR: 書込ファイル " + filename +
                      " が開けませんでした．");
  }

  // 1行目に行の次数と列の次数を書き込む．
  ofs << mat.m_rows << col_delim << mat.m_cols << '\n';

  // 2行目以降に行列の要素を書き込む．
  if (/* mat.m_rows > 0 && */ mat.m_cols > 0) {
    for (int r = 0; r < mat.m_rows; r++) {
      ofs << mat[r][0];
      for (int c = 1; c < mat.m_cols; c++) {
        ofs << col_delim << mat[r][c];
      }
      ofs << '\n';
    }
  }

  ofs << std::flush;
}

} // namespace eigenweg
