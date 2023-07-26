#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <time.h>

int get_T_data(std::vector<std::vector<double>>& mat, const std::string& file_name);
double get_determinant(std::vector<std::vector<double>>& mat, int N);

int main(int argc, char** argv) {
  const std::string file_name = argv[1];
  unsigned int significant_figures = 15;
  std::vector<std::vector<double>> mat, swap_mat;
  const int N = get_T_data(mat, file_name);

  clock_t start = clock();
  double det_A = get_determinant(mat, N);
  if(det_A == 0){
    std::cout << "cannot determine the answer\n";
    return 0;
  }
  for (int i = 0; i < N; i++) {
    mat[i].swap(mat[N]);
    double swap_det = get_determinant(mat, N);
    mat[i].swap(mat[N]);
    std::cout << std::setprecision(significant_figures) << swap_det/det_A << " ";
  }
  std::cout << std::endl;
  clock_t end = clock();
  std::cout << (double)(end - start)/CLOCKS_PER_SEC << std::endl;

  /*
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
  */
}

int get_T_data(std::vector<std::vector<double>>& mat, const std::string& file_name) {
  std::ifstream data_file (file_name);
  int n;
  data_file >> std::dec >> n;
  mat = std::vector<std::vector<double>>(n+1, std::vector<double>(n, 0));
  for (int j = 0; j < n; j++) {
    for (int i = 0; i < n+1; i++) {
      double x;
      data_file >> std::dec >> x;
      mat[i][j] = x;
    }
  }
  return n;
}

double get_determinant(std::vector<std::vector<double>>& mat, int N) {
  std::vector<std::vector<double>> swap_mat;
  swap_mat = std::vector<std::vector<double>>(N, std::vector<double>(N, 0));
  for (int j = 0; j < N; j++) {
    for (int k = 0; k < N; k++) {
      swap_mat[j][k] = mat[j][k];
    }
  }
  double determinant = 1;
  for (int c = 0; c < N; ++c) {
    double pivot = swap_mat[c][c];
    if (pivot == 0) {
      for (int i = c+1; i < N; ++i) {
        if (swap_mat[i][c] == 0) continue;
        pivot = swap_mat[i][c];
        swap_mat[i].swap(swap_mat[c]);
        determinant *= -1;
        break;
      }
      if (pivot == 0) {
        determinant = 0;
        break;
      }
    }
    determinant *= pivot;
    for (int i = c+1; i < N; ++i) {
      double numerator = swap_mat[i][c] / pivot;
      for (int j = c+1; j < N; ++j) {
        swap_mat[i][j] -= swap_mat[c][j] * numerator;
      }
    }
  }
  return determinant;
}