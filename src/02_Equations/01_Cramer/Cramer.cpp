#include "eigenweg.hpp"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;

using Mat = eigenweg::Mat;

// src\01\01による行列式計算の移植
int factorial(int n);
int my_next_permutation(int *arr,int len);
double get_determinant(Mat mat);
// 移植終わり

// 分割用の関数


// main
int main(int argc,char **argv){
	if(argc < 1){
		cout << "Missing argument. FileName is expected." << endl;
		return -1;
	}
	try{
		Mat mat = eigenweg::read_csv(argv[1]);
		cout << "mat = \n";
		mat.dbg_print();
		if(mat.m_cols - mat.m_rows > 1){
			cout << "matrix shape doesn't fit.\ncannot determine the answer.\n";
			return 0;
		}
		double det_A = get_determinant(mat);
		cout << "det(mat) = " << det_A << endl;
		if(det_A == 0){
			cout << "cannot determine the answer\n";
			return 0;
		}
		for(int i = 0;i < mat.m_rows;i++){
			cout << "--------------------\n";
			Mat swap_mat = mat;
			for(int j = 0;j < mat.m_rows;j++){
				swap_mat[j][i] = mat[j][mat.m_cols-1];
			}
			cout << "swap_mat = \n";
			swap_mat.dbg_print();
			cout << "mat = \n";
			mat.dbg_print();
			double swap_det = get_determinant(swap_mat);
			cout << "x" << i+1 << " = " << swap_det/det_A << endl;
		}
	}catch(string err_msg){
		cerr << err_msg << endl;
		exit(1);
	}
	return 0;
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