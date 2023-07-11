#include "../eigenweg.hpp"
#include <cassert>

using namespace std;
using Mat = eigenweg::Mat;

void general_usage() {
  Mat mat1 = eigenweg::read_csv("test1.csv");
  assert(mat1.m_rows == 3);
  assert(mat1.m_cols == 4);
  assert(mat1.m_data.size() == 3 * 4);

  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      assert(mat1[r][c] == mat1.m_data[r * 4 + c]);
    }
  }

  // 1st row
  assert(mat1[0][0] == 1.1);
  assert(mat1[0][1] == 2.2);
  assert(mat1[0][2] == 3.3);
  assert(mat1[0][3] == 4.4);

  // 2nd row
  assert(mat1[1][0] == 5.5);
  assert(mat1[1][1] == 6.6);
  assert(mat1[1][2] == 7.7);
  assert(mat1[1][3] == 8.8);

  // 3rd row
  assert(mat1[2][0] == 9.9);
  assert(mat1[2][1] == 10.10);
  assert(mat1[2][2] == 11.11);
  assert(mat1[2][3] == 12.12);
}

void file_cannot_be_opened() {
  try {
    eigenweg::read_csv("some-random-nonexistent-file.csv");
  } catch (std::string msg) {
    assert(msg == "ERROR: CSVファイル some-random-nonexistent-file.csv "
                  "が開けませんでした．");
  }
}

void cannot_not_read_onwards_from_3_2_in_csv_test2() {
  try {
    eigenweg::read_csv("test2.csv");
  } catch (std::string msg) {
    std::cerr << msg << std::endl;
    assert(msg == "ERROR: 3行の2列目以降の要素の値が読み取れません．");
  }
}

void cannot_not_read_row_3_in_csv_test3() {
  try {
    eigenweg::read_csv("test3.csv");
  } catch (std::string msg) {
    assert(msg == "ERROR: 3行の1列目以降の要素の値が読み取れません．");
  }
}

void extra_columns_at_row_2_in_csv_test4() {
  try {
    eigenweg::read_csv("test4.csv");
  } catch (std::string msg) {
    assert(msg == "ERROR: 2行目の列の次数 4 以降に余分な値が存在します．");
  }
}

void cannot_not_convert_element_3_2_in_csv_test5() {
  try {
    eigenweg::read_csv("test5.csv");
  } catch (std::string msg) {
    assert(msg ==
           "ERROR: 3行2列の要素の値 \"abcd\" はdouble型に変換できません．");
  }
}

int main() {
  general_usage();
  file_cannot_be_opened();
  cannot_not_read_onwards_from_3_2_in_csv_test2();
  cannot_not_read_row_3_in_csv_test3();
  extra_columns_at_row_2_in_csv_test4();
  cannot_not_convert_element_3_2_in_csv_test5();
}
