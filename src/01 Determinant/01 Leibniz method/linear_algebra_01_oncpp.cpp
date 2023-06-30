#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

/*
 * 行列式を計算するプログラム
 * 実行時間（多分）：O(N!)(n >= 5)
 * 訂正↑O(N!*N)が正しい
 * 実装方法：(行列式に関するライプニッツの明示公式というらしい)
 * 長さNの順列を全部列挙して、奇順列、偶順列を判定してから、
 * その順列に対応した符号をつけて一行毎の積を行列式に足し合わせる。
 * 特長：
 * 実装に浮動小数点を用いない方式で計算誤差を抑えることを目指した。
 * 引数：
 * 実行ファイルと同じフォルダ内のスペース区切りcsvファイル名を指定。入力は整数値に限る。
 */

// csvの区切り記号を指定
#define SEPARATE_CHAR (' ')

// 行列の次数 完成版ではファイルから読む
/*
#define N (4)

int sample[N][N] = {
	{2,4,-3,4},
	{-5,2,1,5},
	{-3,4,2,-1},
	{4,6,-7,-2},
};
*/

// string型の文字列を分割する関数　以下のアドレスから https://cvtech.cc/readcsv/
vector<string> split(string& input, char delimiter){
	istringstream stream(input);
	string field;
	vector<string> result;
	while (getline(stream, field, delimiter)) {
		result.push_back(field);
	}
	return result;
}

// 階乗計算
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

int main(int argc,char **argv){
	if(argc < 2){
		cout << "filename is not selected" << endl;
		return 0;
	}
	
	// ファイルから読み込み、ここの処理はcsvの書き方次第で変える
	vector<vector<int>> mtrx;
	int N = -1,l = 0;
	ifstream ifs(argv[1]);
	if(!ifs){
		cout << "cannot find selected file" << endl;
	}
	string line;
	while (getline(ifs,line)){
		vector<string> strvec = split(line,SEPARATE_CHAR);
		if(N == -1){
			N = stoi(strvec.at(0));
			for(int i = 0;i < N;i++){
				mtrx.push_back(vector<int>());
			}
		}else{
			for (int i = 0;i < strvec.size();i++){
				mtrx.at(l).push_back((int)stoi(strvec.at(i)));
			}
			l++;
		}
	}
	
	// 行列式の計算
	long long t = factorial(N);// 繰り返し回数
	long long exch = 0;// 交換回数、順列の偶奇を判定するのに利用
	long long det = 0;// 行列式の値
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
		long long pro = 1;// 一行分の積を保持する
		for(int j = 0;j < N;j++){
			pro *= mtrx[j][arr[j]];
		}
		det += ((exch % 2)?-1:1) * pro;// ここで偶奇を判定
		exch += my_next_permutation(arr,N);// 交換回数を更新
	}
	cout << "det = " << det << endl;
	return 0;
}

