#include "eigenweg.hpp"
#include <cassert>

using namespace std;

using Mat = eigenweg::Mat;
// using Vec = eigenweg::Vec; // vector<double> と同じ

int main() {
  // =====================================
  // = 行列の定義（その１：配列で初期化）=
  // =====================================

  // 3 x 3行列の定義
  Mat mat_A({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}});

  // デバッグ用に行列全体を出力:
  cout << "mat_A =\n";
  mat_A.dbg_print();

  // 添字アクセス:
  assert(mat_A[0][0] == 1.0 && mat_A[2][2] == 9.0);

  // (1, 1) 要素と(3, 3) 要素の入れ替え:
  swap(mat_A[0][0], mat_A[2][2]);
  assert(mat_A[0][0] == 9.0 && mat_A[2][2] == 1.0);

  // =========================================
  // = 行列の定義（その２：行・列の次数指定）=
  // =========================================
  Mat mat_B(3, 3);

  // 2重forループ:
  for (int r = 0; r < mat_A.m_rows; r++) {
    for (int c = 0; c < mat_A.m_cols; c++) {
      mat_B[r][c] = 5.0 - mat_A[r][c];
    }
  }

  // 行列どうしの比較;
  assert(mat_A != mat_B);

  // 単位行列:
  Mat mat_id = Mat::id(3);
  assert(mat_id.m_rows == 3 && mat_id.m_cols == 3);

  // 行列積:
  Mat mat_AB = mat_A.dot(mat_B);
  cout << "mat_AB =\n";
  mat_AB.dbg_print();

  // 単位行列との行列積:
  assert(mat_A.dot(mat_id) == mat_A && mat_id.dot(mat_A) == mat_A);

  // ================================================
  // = 行列の定義（その３：CSVファイルの読込・書込）=
  // ================================================
  // CSVファイルからの読み込み:
  try {
    // コンマ区切りのCSVから行列を読み取る（デフォルト）:
    Mat mat_C = eigenweg::read_csv("example-mat-C.csv");
    // もしくは，
    // Mat mat_C = eigenweg::read_csv("example-mat-C.csv", ',');
    cout << "mat_C =\n";
    mat_C.dbg_print();

    // スペース区切りのCSVから行列を読み取る:
    Mat mat_C_ = eigenweg::read_csv("example-mat-C-space-separated.csv", ' ');
    cout << "mat_C (space separated) =\n";
    mat_C_.dbg_print();
  } catch (string err_msg) {
    // eigenweg.hpp では例外処理として std::string を用いている
    cerr << err_msg << endl;
    exit(1);
  }

  // CSVファイルへの書き込み:
  try {
    // コンマ区切りのフォーマットで保存する（デフォルト）:
    save_csv("example-mat-A.csv", mat_A);
    // もしくは，
    // save_csv("example-mat-A.csv", mat_A, ',');

    // スペース区切りのフォーマットで保存する:
    save_csv("example-mat-A-space-separated.csv", mat_A, ' ');
  } catch (string err_msg) {
    // eigenweg.hpp では例外処理として std::string を用いている
    cerr << err_msg << endl;
    exit(1);
  }
}
