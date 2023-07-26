#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <time.h>

bool get_data(std::vector<std::vector<double>>& mat, const std::string& file_name);

double cofactor(std::vector<std::vector<double>>& mat, int n);

int main(int argc, char** argv) {
  const std::string file_name = argv[1];
  const unsigned int significant_figures = 15;
  std::vector<std::vector<double>> mat;
  if (get_data(mat, file_name)) return 1;
  const int N = mat.size();

  clock_t start = clock();
  double determinant = 1;
  for (int c = 0; c < N; ++c) {
    double pivot = mat[c][c];
    if (pivot == 0) {
      for (int i = c+1; i < N; ++i) {
        if (mat[i][c] == 0) continue;
        pivot = mat[i][c];
        mat[i].swap(mat[c]);
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
      double numerator = mat[i][c] / pivot;
      for (int j = c+1; j < N; ++j) {
        mat[i][j] -= mat[c][j] * numerator;
      }
    }
  }
  clock_t end = clock();
  std::cout << std::setprecision(significant_figures) << determinant << std::endl;
  std::cout << (double)(end - start)/CLOCKS_PER_SEC << std::endl;
  return 0;
}

bool get_data(std::vector<std::vector<double>>& mat, const std::string& file_name) {
  std::ifstream data_file (file_name);
  if (!data_file.is_open()) return true;
  int n;
  data_file >> std::dec >> n;
  mat = std::vector<std::vector<double>>(n, std::vector<double>(n, 0));
  for (std::vector<double>& mat_row: mat) {
    for (double& x: mat_row) {
      data_file >> std::dec >> x;
    }
  }
  return false;
}

double cofactor(std::vector<std::vector<double>>& mat, int n) {
  
}