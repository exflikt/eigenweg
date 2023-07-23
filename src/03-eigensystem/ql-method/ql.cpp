#include "ql.hpp"

using namespace std;

using Mat = eigenweg::Mat;
using QL = eigenweg::QL;

int main(const int argc, const char **argv) {
  if (argc != 4) {
    cerr << "ERROR: コマンドの引数で入出力のCSVファイルを指定してください．"
         << endl;
    return 1;
  }
  string mat_input_path(argv[1]);
  string vals_output_path(argv[2]);
  string vecs_output_path(argv[3]);

  try {
    Mat mat = eigenweg::read_csv(mat_input_path);
    clock_t time = clock();
    QL ql(mat);
    double elapsed_sec = (double)(clock() - time) / (CLOCKS_PER_SEC);

#ifdef DEBUG
    cerr << "固有値: [";
    for (int i = 0; i < ql.m_n - 1; i++) {
      cerr << ql.m_diagonals[i] << ", ";
    }
    cerr << ql.m_diagonals[ql.m_n - 1] << "]\n";

    cerr << "固有ベクトル:\n";
    ql.m_mat.dbg_print();
#endif

    ofstream vals_ofs(vals_output_path);
    vals_ofs << setprecision(numeric_limits<double>::max_digits10);
    vals_ofs << ql.m_diagonals.size() << "\n";
    for (int i = 0; i < ql.m_n - 1; i++) {
      vals_ofs << ql.m_diagonals[i] << ",";
    }
    vals_ofs << ql.m_diagonals[ql.m_n - 1] << endl;

    eigenweg::save_csv(vecs_output_path, ql.m_mat);

    cout << "計算時間 [s]: " << elapsed_sec << endl;
  } catch (string err_msg) {
    cerr << err_msg << endl;
    return 1;
  }
}
