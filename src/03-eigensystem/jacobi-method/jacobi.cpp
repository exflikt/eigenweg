#include "jacobi.hpp"

using namespace std;

using Mat = eigenweg::Mat;
using Jacobi = eigenweg::Jacobi;

int main(const int argc, const char **argv) {
  if (argc != 2) {
    cerr << "ERROR: コマンドの引数でCSVファイルを指定してください．" << endl;
    return 1;
  }
  string filepath(argv[1]);

  try {
    Mat mat = eigenweg::read_csv(filepath);
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

    string filedir, filename;
    size_t last_slash_pos = filepath.find_last_of("/\\");
    if (last_slash_pos != string::npos) {
      filedir = filepath.substr(0, last_slash_pos + 1);
      filename = filepath.substr(last_slash_pos + 1);
    } else {
      filedir = "";
      filename = filepath;
    }

    ofstream vals_ofs(filedir + "vals-" + filename);
    vals_ofs << setprecision(numeric_limits<double>::max_digits10);
    vals_ofs << jac.m_vals.size() << "\n";
    for (int i = 0; i < jac.m_n - 1; i++) {
      vals_ofs << jac.m_vals[i] << ",";
    }
    vals_ofs << jac.m_vals[jac.m_n - 1] << endl;

    eigenweg::save_csv(filedir + "vecs-" + filename, jac.m_vecs);

    cout << "ヤコビ回転の回数: " << jac.m_nrot << "\n";
    cout << "計算時間 [s]: " << elapsed_sec << endl;
  } catch (string err_msg) {
    cerr << err_msg << endl;
    return 1;
  }
}
