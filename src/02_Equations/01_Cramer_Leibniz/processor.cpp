#include "eigenweg.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <time.h>
#include <iostream>
#include <string>
#include <cassert>

using Mat = eigenweg::Mat;

// src\01\01による行列式計算の移植
int factorial(int n);
int my_next_permutation(int *arr,int len);
double get_determinant(Mat mat);
// 移植終わり

int main(int argc, char** argv) {
  const std::string file_name = argv[1];
  unsigned int significant_figures = 15;
  Mat mat = eigenweg::read_csv(argv[1]);

  clock_t start = clock();
  double det_A = get_determinant(mat);
  if(det_A == 0){
    std::cout << "cannot determine the answer\n";
    return 0;
  }
  for(int i = 0;i < mat.m_rows;i++){
    Mat swap_mat = mat;
    for(int j = 0;j < mat.m_rows;j++){
      swap_mat[j][i] = mat[j][mat.m_cols-1];
    }
    double swap_det = get_determinant(swap_mat);
    std::cout << std::setprecision(significant_figures) << swap_det/det_A << " ";
  }
  std::cout << std::endl;
  clock_t end = clock();
  /*
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
  */
}

// 自作関数実体
int factorial(int n){
	if(n == 1)return 1;
	return n * factorial(n-1);
}
int my_next_permutation(int *arr,int len){
	int exch = 0;
	int left = len - 2;
	while(left >= 0 && arr[left] >= arr[left+1])left--;
	if(left < 0)return exch;
	int right = len -1;
	while(arr[left] >= arr[right])right--;
	{
		int t = arr[left];
		arr[left] = arr[right];
		arr[right] = t;
		exch++;
	}
	left++;
	right = len - 1;
	while(left < right){
		{
			int t = arr[left];
			arr[left] = arr[right];
			arr[right] = t;
			exch++;
		}
		left++;
		right--;
	}
	return exch;
}
double get_determinant(Mat mat){
	double det;
	int N = mat.m_rows;
	int t = factorial(N);
	int arr[N] = {};
	int exch = 0;
	for(int i = 0;i < sizeof(arr)/sizeof(arr[0]);i++){
		arr[i] = i;
	}
	for(int i = 0;i < t;i++){
		double pro = 1;
		for(int j = 0;j < N;j++){
			pro *= mat[j][arr[j]];
		}
		det += ((exch %2)?-1:1) * pro;
		exch += my_next_permutation(arr,N);
	}
	return det;
}