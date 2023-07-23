#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <time.h>

bool get_data(std::vector<std::vector<double>>& mat, const std::string& file_name);

// 階乗計算
long long factorial(long long n);

// c++標準の<algorithm>next_permutaitionの機能を拡張し、戻り値に交換回数を返すよう変更している。
// 機能は<algorithm>のドキュメントを参照
int my_next_permutation(int *arr,int len);

int main(int argc, char** argv) {
  const std::string file_name = argv[1];
  const unsigned int significant_figures = 15;
  std::vector<std::vector<double>> mat;
  if (get_data(mat, file_name)) return 1;
  const int N = mat.size();

  clock_t start = clock();
  long long t = factorial(N);// 繰り返し回数
	long long exch = 0;// 交換回数、順列の偶奇を判定するのに利用
	double det = 0;// 行列式の値
	int arr[N];// 順列を格納する配列
	for(int i = 0;i < N;i++){// 順列を格納する配列の初期化
		arr[i] = i;
	}
	for(long long i = 0;i < t;i++){
		/*{
			// テスト出力
			printf("i = %d :arr = {",i);
			for(int j = 0;j < N;j++){
				printf("%d, ",arr[j]);
			}
			printf("} exch = %d\n",exch);
		}*/
		// 行列式を実際に計算する部分
		double pro = 1;// 一行分の積を保持する
		for(int j = 0;j < N;j++){
			pro *= mat[j][arr[j]];
		}
		det += ((exch % 2)?-1:1) * pro;// ここで偶奇を判定
		exch += my_next_permutation(arr,N);// 交換回数を更新
	}

  clock_t end = clock();
  std::cout << std::setprecision(significant_figures) << det << std::endl;
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

long long factorial(long long n){
	if(n == 1)return 1;
	return n * factorial(n-1);
}

// c++標準の<algorithm>next_permutaitionの機能を拡張し、戻り値に交換回数を返すよう変更している。
// 機能は<algorithm>のドキュメントを参照
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