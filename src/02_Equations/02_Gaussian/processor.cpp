#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <time.h>

void get_data(std::vector<std::vector<double>>& mat, const std::string& file_name);

int main(int argc, char** argv) {
  const std::string file_name = argv[1];
  unsigned int significant_figures = 15;
  int one_equation;
  try {
    one_equation = std::stoi(argv[2]);
  }
  catch(const std::invalid_argument& e) {
    std::cout << "invalid argument" << std::endl;
    return 1;
  }
  std::vector<std::vector<double>> mat;
  get_data(mat, file_name);
  const int N = mat.size();
  int N_col = N;

  clock_t start = clock();
  // 斉次か非斉次か判定
  // 非斉次の場合: N_col = N + 1 (定数ベクトルを含め，拡大係数行列とする)
  // 斉次の場合: N_col = N (定数ベクトルはゼロであるため，拡大係数行列に含めず無駄を省く)
  for (const std::vector<double>& mat_row : mat) {
    if (mat_row[N] != 0) {
      N_col++;
      break;
    }
  }
  std::vector<int> skip_col; // pivot のない列を列挙するベクタ
  skip_col.reserve(N);
  // i は行，j は列，任意のrankに対応するため iのインクリメントは毎回ではない
  for (int i = 0, j = 0; j < N; ++j) {
    double pivot = 0;
    // pivot が 0 でない行を探し，交換する
    for (int i2 = i; i2 < N; ++i2) {
      if (mat[i2][j] == 0) continue;
      pivot = mat[i2][j];
      mat[i].swap(mat[i2]);
      break;
    }
    // pivot のない列番号を追加しておく
    if (pivot == 0) {
      skip_col.push_back(j);
      continue;
    }
    // pivot のある行について，pivot の次の列から pivot で割る
    for (int j2 = j+1; j2 < N_col; ++j2) {
      mat[i][j2] /= pivot;
    }
    //  pivot のある行を退避させる
    mat[i].swap(mat[0]);
    // pivot のない全ての行に対するfor
    for (int i2 = 1; i2 < N; ++i2) {
      // pivot 列の次の列から最後の列まで全ての列に対するfor
      for (int j2 = j+1; j2 < N_col; ++j2) {
        // 掃出し法基本操作
        mat[i2][j2] -= mat[0][j2] * mat[i2][j];
      }
    }
    // pivot のある行を元の位置に戻す
    mat[i].swap(mat[0]);
    ++i;
  }
  clock_t end = clock();

  int coe_mat_rank = N - skip_col.size();
  if (one_equation) {
    // ランダム行列用 複数の解になることは限りなく少ない
    if (N_col != N && coe_mat_rank == N) {
      for (const std::vector<double> mat_row : mat) {
        std::cout << std::setprecision(significant_figures) << mat_row[N] << " ";
      }
      std::cout << std::endl;
      std::cout << (double)(end - start)/CLOCKS_PER_SEC << std::endl;
    }
    else {
      // 複数の解
      return 1;
    }
  }
  else {
    if (N_col != N) {
      // 非斉次
      if (coe_mat_rank == N) {
        // 1つの解
        std::cout << "非斉次，ただ１つの解を持つ．" << std::endl;
        std::cout << "解:" << std::endl;
        for (const std::vector<double> mat_row : mat) {
          std::cout << std::setprecision(significant_figures) << mat_row[N] << " ";
        }
        std::cout << std::endl;
      }
      else {
        int arg_coe_mat_rank = coe_mat_rank;
        for (int i = N-1; i > coe_mat_rank-1; --i) {
          if (mat[i][N] != 0) {
            arg_coe_mat_rank = i+1;
            break;
          }
        }
        if (arg_coe_mat_rank == coe_mat_rank) {
          // 無数の解
          std::cout << "非斉次，無数の解を持つ．" << std::endl;
          std::cout << "解の自由度: " << skip_col.size() << std::endl;
          std::cout << "未知数:" << std::endl;
          skip_col.push_back(N);
          int skip_pre = -1;
          for (const int& skip_val : skip_col) {
            for (int i = skip_pre+1; i < skip_val; ++i) {
              std::cout << i+1 << " ";
            }
            skip_pre = skip_val;
          }
          skip_col.pop_back();
          std::cout << std::endl << "に対する解は，別の未知数と定数：" << std::endl;
          for (const int& skip_variable : skip_col) {
            std::cout << skip_variable+1 << " ";
          }
          std::cout << "1(const.)" << std::endl;
          std::cout << "と，次の列ベクトルの線形結合で表せる．" << std::endl;
          for (const int& skip_val : skip_col) {
            for (int i = 0; i < coe_mat_rank; ++i) {
              std::cout << std::setprecision(significant_figures) << -mat[i][skip_val] << " ";
            }
            std::cout << std::endl;
          }
          for (int i = 0; i < coe_mat_rank; ++i) {
            std::cout << std::setprecision(significant_figures) << mat[i][N] << " ";
          }
          std::cout << std::endl;
        }
        else {
          // 解なし
          std::cout << "解なし" << std::endl;
          std::cout << "係数行列のrank: " << coe_mat_rank << "，拡大係数行列のrank: " << arg_coe_mat_rank << std::endl;
        }
      }
    }
    else {
      // 斉次
      if (coe_mat_rank == N) {
        // 1つの解(0のみ)
        std::cout << "斉次，自明解のみを持つ．" << std::endl;
      }
      else {
        // 無数の解
        std::cout << "斉次，非自明解を持つ．" << std::endl;
        std::cout << "解の自由度: " << skip_col.size() << std::endl;
        std::cout << "未知数：" << std::endl;
        skip_col.push_back(N);
        int skip_pre = -1;
        for (const int& skip_val : skip_col) {
          for (int i = skip_pre+1; i < skip_val; ++i) {
            std::cout << i+1 << " ";
          }
          skip_pre = skip_val;
        }
        skip_col.pop_back();
        std::cout << std::endl << "に対する解は，別の未知数：" << std::endl;
        for (const int& skip_variable : skip_col) {
          std::cout << skip_variable+1 << " ";
        }
        std::cout << std::endl;
        std::cout << "と，次の列ベクトルの線形結合で表せる．" << std::endl;
        for (const int& skip_val : skip_col) {
          for (int i = 0; i < coe_mat_rank; ++i) {
            std::cout << std::setprecision(significant_figures) << -mat[i][skip_val] << " ";
          }
          std::cout << std::endl;
        }
      }
    }
    std::cout << "処理時間：" << std::endl;
    std::cout << (double)(end - start)/CLOCKS_PER_SEC << std::endl;
  }
  std::cout << (double)(end - start)/CLOCKS_PER_SEC << std::endl;
}


void get_data(std::vector<std::vector<double>>& mat, const std::string& file_name) {
  std::ifstream data_file (file_name);
  int n;
  data_file >> std::dec >> n;
  mat = std::vector<std::vector<double>>(n, std::vector<double>(n+1, 0));
  for (std::vector<double>& mat_row: mat) {
    for (double& x: mat_row) {
      data_file >> std::dec >> x;
    }
  }
}