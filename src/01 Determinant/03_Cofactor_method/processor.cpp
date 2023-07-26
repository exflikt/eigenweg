#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <time.h>

bool get_data(std::vector<std::vector<double>>& mat, const std::string& file_name);
double get_cofactor(std::vector<std::vector<double>>& mat, int n);

int main(int argc, char** argv) {
  const std::string file_name = argv[1];
  const unsigned int significant_figures = 15;
  std::vector<std::vector<double>> mat;
  if (get_data(mat, file_name)) return 1;
  const int N = mat.size();

  clock_t start = clock();
  double determinant = get_cofactor(mat, N);
  // double determinant = 1;
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

double get_cofactor(std::vector<std::vector<double>>& mat, int n) {
  if (n == 1) {
    return mat[0][0];
  }
  double cofactor = 0;
  int sgn = 1;
  cofactor += sgn * mat[n-1][n-1] * get_cofactor(mat, n-1);
  for (int i = 0; i < n-1; i++) {
    mat[n-2-i].swap(mat[n-1]);
    sgn *= -1;
    cofactor += sgn * mat[n-1][n-1] * get_cofactor(mat, n-1);
  }
  for (int i = 0; i < n-1; i++) {
    mat[i].swap(mat[n-1]);
  }
  return cofactor;
}