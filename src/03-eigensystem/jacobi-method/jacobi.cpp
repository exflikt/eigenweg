#include "jacobi.hpp"

using namespace std;

using Mat = eigenweg::Mat;
using Jacobi = eigenweg::Jacobi;

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
    Jacobi jac(mat);
    double elapsed_sec = (double)(clock() - time) / (CLOCKS_PER_SEC);

#ifdef DEBUG
    cerr << "固有値: [";
    for (int i = 0; i < jac.m_n - 1; i++) {
      cerr << jac.m_vals[i] << ", ";
    }
    cerr << jac.m_vals[jac.m_n - 1] << "]\n";

    cerr << "固有ベクトル:\n";
    jac.m_vecs.dbg_print();
#endif

    ofstream vals_ofs(vals_output_path);
    vals_ofs << setprecision(numeric_limits<double>::max_digits10);
    vals_ofs << jac.m_vals.size() << "\n";
    for (int i = 0; i < jac.m_n - 1; i++) {
      vals_ofs << jac.m_vals[i] << ",";
    }
    vals_ofs << jac.m_vals[jac.m_n - 1] << endl;

    eigenweg::save_csv(vecs_output_path, jac.m_vecs);

    cout << "ヤコビ回転の回数: " << jac.m_nrot << "\n";
    cout << "計算時間 [s]: " << elapsed_sec << endl;
  } catch (string err_msg) {
    cerr << err_msg << endl;
    return 1;
  }
}
