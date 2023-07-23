#pragma once

#include "eigenweg.hpp"
#include <cmath>

#ifndef SIGN
// b の符号を a に適用するマクロ
// E.g. SIGN(2, 5) = 2; SIGN(2, -5) = 2; SIGN(-2, 5) = -2; SIGN(-2, -5) = 2
#define SIGN(a, b) ((b) >= 0 ? (a) : -(a))
#endif

namespace eigenweg {

inline void throw_when_nonsymmetric(Mat mat);
inline void tridiagonalize(Mat &sym_mat, Vec &diagonals, Vec &subdiagonals);
inline double pythag(const double a, const double b);
inline void eigensort(Vec &vals, Mat &vecs);

// QL (QR) 法によって固有値・固有ベクトルを求める
//
// 三重対角行列にした後，QL法を適用する．
//
// 三重対角行列にする tridiagonalize メソッドは 4n^3 / 3 = O(n^3) のアルゴリズ
// ムである．一方で，QL法は 6n^3 = O(n^3) のアルゴリズムである．
struct QL {
  // 実対称行列 A の次数 n
  int m_n;

  // 入力: 実対象行列 A m_mat[0..n-1][0..n-1]
  // 出力: 固有ベクトルの集合
  Mat m_mat;

  // 三重対角行列の対角要素 m_diagonals[0..n-1]
  Vec m_diagonals;

  // 三重対角行列の非対角要素 m_sd[0..n-1]
  //
  // 対称行列の場合，右上準対角要素と左下準対角要素が同じになるので，n - 1 個の
  // 要素を保存するだけで十分である．なお，m_subdiagonals[0] = 0 とし，準対角要
  // 素は m_subdiagonals[1..n-1] に保存する．
  Vec m_subdiagonals;

  QL(Mat mat)
      : m_n(mat.m_rows), m_mat(mat), m_diagonals(m_n), m_subdiagonals(m_n) {
    // mat を三重対角化する（関数を実行した後，mat は行列 Q になる）
    tridiagonalize(m_mat, m_diagonals, m_subdiagonals);
    // 三重対角行列 mat を対角化する
    // （対角化した後，mat は固有ベクトルの集合になる）
    diagonalize();
    eigensort(m_diagonals, m_mat);
  }

  // 三重対角行列のQL法による対角化
  void diagonalize() {
    // Renumber the elements of subdiagonals for convenience
    for (int i = 1; i < m_n; i++) {
      m_subdiagonals[i - 1] = m_subdiagonals[i];
    }
    m_subdiagonals[m_n - 1] = 0.0;

    for (int l = 0; l < m_n; l++) {
      int no_iterations = 0;
      while (true) {
      continue_split_matrix:
        int m = l;
        for (; m < m_n - 1; m++) {
          const double EPS = std::numeric_limits<double>::epsilon();
          // Look for a single small subdiagonal element to split the matrix
          double dd = std::abs(m_diagonals[m]) + std::abs(m_diagonals[m + 1]);
          if (std::abs(m_subdiagonals[m]) <= EPS * dd) {
            break;
          }
        }

        if (m == l) {
          break;
        }

        if (no_iterations++ == 30) {
          throw std::string("Too many iterations in QL method");
        }

        // Form shift
        double g =
            (m_diagonals[l + 1] - m_diagonals[l]) / (2.0 * m_subdiagonals[l]);
        double r = pythag(g, 1.0);
        // This is d_m - k_s
        g = m_diagonals[m] - m_diagonals[l] +
            m_subdiagonals[l] / (g + SIGN(r, g));

        double s = 1.0, c = 1.0;
        double p = 0.0;
        int i = m - 1;
        for (; i >= l; i--) {
          // A plane rotation as in the original QL, followed by Givens
          // rotations to restore tridiagonal form
          double f = s * m_subdiagonals[i];
          double b = c * m_subdiagonals[i];
          r = pythag(f, g);
          m_subdiagonals[i + 1] = r;

          if (r == 0.0) {
            // Recover from underflow
            m_diagonals[i + 1] -= p;
            m_subdiagonals[m] = 0.0;
            goto continue_split_matrix; // 2-level nested `continue`
          }

          s = f / r;
          c = g / r;
          g = m_diagonals[i + 1] - p;
          r = (m_diagonals[i] - g) * s + 2.0 * c * b;
          p = s * r;
          m_diagonals[i + 1] = g + s * r;
          g = c * r - b;

          // 固有ベクトルを形成する
          for (int k = 0; k < m_n; k++) {
            f = m_mat[k][i + 1];
            m_mat[k][i + 1] = s * m_mat[k][i] + c * f;
            m_mat[k][i] = c * m_mat[k][i] - s * f;
          }
        }

        m_diagonals[l] -= p;
        m_subdiagonals[l] = g;
        m_subdiagonals[m] = 0.0;
      }
    }
  }
};

inline void throw_when_nonsymmetric(Mat mat) {
  if (mat.m_rows != mat.m_cols) {
    throw std::string("ERROR: 行と列の次数が一致していません．");
  }
  int n = mat.m_rows;
  for (int r = 0; r < n; r++) {
    for (int c = r + 1; c < n; c++) {
      if (mat[r][c] != mat[c][r]) {
        throw(std::ostringstream()
              << "ERROR: " << r << "行" << c << "列"
              << "と" << c << "行" << r << "列"
              << "の要素が一致していないので対称行列でありません．")
            .str();
      }
    }
  }
}

// 三重対角行列化する関数
inline void tridiagonalize(Mat &mat, Vec &diagonals, Vec &subdiagonals) {
  throw_when_nonsymmetric(mat);

  const int n = mat.m_rows;

  for (int d = n - 1; d > 0; d--) { // d: diagonal index (n - 1 -> 1)
    if (d == 1) {
      subdiagonals[d] = mat[d][d - 1];
      diagonals[d] = 0.0;
      continue;
    }

    double scale = 0.0;
    for (int nd = 0; nd < d; nd++) { // nd: non-diagonal index
      scale += std::abs(mat[d][nd]);
    }

    if (scale == 0.0) {
      subdiagonals[d] = mat[d][d - 1];
      diagonals[d] = 0.0;
      continue;
    }

    double h = 0.0;
    for (int nd = 0; nd < d; nd++) { // nd: non-diagonal index
      mat[d][nd] /= scale;           // Use scaled elements for transformation
      h += mat[d][nd] * mat[d][nd];  // Form σ in h
    }
    double sd_d = mat[d][d - 1];
    double magnitude = sd_d >= 0.0 ? -sqrt(h) : sqrt(h);
    subdiagonals[d] = scale * magnitude;
    h -= sd_d * magnitude;
    // Now h is equation H ≡ 1/2 |u|^2
    mat[d][d - 1] = sd_d - magnitude; // Store u in row d of mat
    double f = 0.0;
    for (int j = 0; j < d; j++) {
      // Store u/H in column d of mat
      mat[j][d] = mat[d][j] / h;

      double g = 0.0;
      // Form an element of A · u in g.
      for (int k = 0; k < j + 1; k++) {
        g += mat[j][k] * mat[d][k];
      }
      for (int k = j + 1; k < d; k++) {
        g += mat[k][j] * mat[d][k];
      }
      subdiagonals[j] = g / h;
      // Form element of p in temporarily unused element of subdiagonals
      f += subdiagonals[j] * mat[d][j];
    }
    double hh = f / (h + h);
    // Form K = {}^t u · p / 2H
    for (int j = 0; j < d; j++) {
      // Form q and store in subdiagonals overwriting p
      double f = mat[d][j];
      double g = subdiagonals[j] - hh * f;
      subdiagonals[j] = subdiagonals[j] - hh * f;
      for (int k = 0; k < j + 1; k++) {
        // Reduce mat, equation A' = A - q · {}^t u - u · {}^t q
        mat[j][k] -= (f * subdiagonals[k] + g * mat[d][k]);
      }
    }
    diagonals[d] = h;
  }

  diagonals[0] = 0.0;
  subdiagonals[0] = 0.0;
  for (int d = 0; d < n; d++) { // d: diagonal index
    // Begin accumulation of transformation matrices
    if (diagonals[d] != 0.0) { // This block is skipped when d = 0
      for (int j = 0; j < d; j++) {
        double g = 0.0;
        for (int k = 0; k < d; k++) {
          // Use u and u/H stored in mat to form P · Q
          g += mat[d][k] * mat[k][j];
        }
        for (int k = 0; k < d; k++) {
          mat[k][j] -= g * mat[k][d];
        }
      }
    }

    diagonals[d] = mat[d][d];
    mat[d][d] = 1.0;
    // Reset row and column of mat to identity matrix for next iteration
    for (int nd = 0; nd < d; nd++) { // nd: non-diagonal index
      mat[nd][d] = mat[d][nd] = 0.0;
    }
  }
}

// √(a^2 + b^2) を計算する
//
// √(a^2 + b^2) = |a| √(1 + (b/a)^2) もしくは
// √(a^2 + b^2) = |b| √((a/b)^2 + 1) で計算する．
inline double pythag(const double a, const double b) {
  // Computes (a^2 + b^2)^{1/2} without destructive underﬂow or overﬂow.
  // sqrt(a^2 + b^2)
  double absa = std::abs(a), absb = std::abs(b);
  if (absa > absb) {
    return absa * sqrt(1.0 + (absb / absa) * (absb / absa));
  } else if (absa < absb) {
    return absb * sqrt((absa / absb) * (absa / absb) + 1.0);
  } else {
    return 0.0;
  }
}

// 固有値 vals[0..n-1] と固有ベクトル vecs[0..n-1]
// を受け取って降順に並び替える
//
// なお，固有値に対応する固有ベクトルは添字が等しいものとする．
// 例えば，2 個目の固有値が 4.5 で，それに対応する固有ベクトルが
// {3.7, -1.2, 8.2} であったとすると，
// vals[1] = 4.5; vecs[1] = {3.7, -1.2, 8.2};
// というように添字がどちらも1で揃っているものとする．
inline void eigensort(Vec &vals, Mat &vecs) {
  int n = vals.size();
  for (int i = 0; i < n - 1; i++) {
    // i 個目以降の固有値で最大値となる場所 max_i を探す
    int max_i = i;
    for (int j = i + 1; j < n; j++) {
      if (vals[j] >= vals[max_i]) {
        max_i = j;
      }
    }

    // i 個目の固有値が最大だったらスキップする
    if (max_i == i) {
      continue;
    }

    // i 番目と max_i 番目の固有値・固有列ベクトルを入れ替える
    std::swap(vals[max_i], vals[i]);
    for (int r = 0; r < n; r++) {
      std::swap(vecs[r][i], vecs[r][max_i]);
    }
  }
}

} // namespace eigenweg
