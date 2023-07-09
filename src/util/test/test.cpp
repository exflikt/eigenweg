#include "../eigenweg.hpp"
#include <cassert>

using namespace std;
using Mat = eigenweg::Mat;

int main() {
  try {
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

  } catch (std::string msg) {
    std::cerr << msg << std::endl;
    return 1;
  }
}
