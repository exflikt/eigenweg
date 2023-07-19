#ifndef JACOBI_HPP
#define JACOBI_HPP

#include "eigenweg.hpp"
#include <cmath>
#include <limits>

namespace eigenweg {

inline void throw_when_nonsymmetric(Mat mat);
inline void eigensort(Vec &vals, Mat &vecs);

// ヤコビ法による実対称行列の対角化をする
//
// O(n^3) のアルゴリズム
// * ヤコビ回転の数: 3n^2 ~ 5n^2
// * 各回転に要する浮動小数点計算: 12n（固有ベクトルを求めない場合は8n）
struct Jacobi {
  // 実対称行列の次数 n
  const int m_n;

  // 実対称行列 m_mat[0..n-1][0..n-1]
  //
  // 対称行列なので，相似変換の過程では上三角行列の読み書きしか行わない．
  // ただし，対角要素については m_vals に保存し，m_mat の対角要素は更新しない．
  // これは，対角要素を保存する場所が一箇所のみで事足りるのと，対角化が終了した
  // ときに m_vals が固有値の集合になるからである．わざわざ m_mat の対角要素を
  // 更新して最後に m_vals を更新するよりだったら，最初からメモリが密集していて
  // キャッシュ局所性を酷使できる m_vals を使おうという戦略である．
  //
  // ここまで読んで気付いたかもしれないが，n x n の行列をメンバとして保存してい
  // るにも関わらず，実際に読み書きするのは上三角行列の，しかも非対角要素のみで
  // ある．n^2 個ではなく，(n - 1) · n / 2 個の要素しか必要ないのである．このこ
  // とから，保存領域の効率化ができると考えられる．
  Mat m_mat;

  // 固有値 m_vals[0..n-1]
  //
  // 相似変換の過程ではその時点での対角要素を保存している．つまり，m_vals[p] は
  // (p, p) 要素 を，m_vals[q] は (q, q) 要素を意味する．なお，m_mat の説明で述
  // べたように，対角成分は m_vals に保存することにし，m_mat の対角成分は更新し
  // ない．
  Vec m_vals;

  // n 個の固有ベクトル m_vec[0..n-1][0..n-1]
  //
  // m_vec[i] が i 個目の固有ベクトルに対応している．相似変換の過程では，その時
  // 点までのヤコビ回転行列の総積が保存される．対角化が完了したとき，相似変換行
  // 列の総積はちょうど対角化行列であり，各列ベクトルは固有ベクトルに対応する．
  Mat m_vecs;

  // ヤコビ回転の回数カウンタ
  int m_nrot;

  Jacobi(Mat &mat)
      : m_n(mat.m_rows), m_mat(mat), m_vals(m_n), m_vecs(Mat::id(m_n)),
        m_nrot(0) {
    throw_when_nonsymmetric(m_mat);

    // 対角要素を保持する変数
    Vec diagonals(m_n);
    // 対角要素を更新するときの差異（-Σ_φ tan φ mat_pq）を保持する変数
    Vec diff_terms(m_n);
    for (int i = 0; i < m_n; i++) {
      // 掃き出しの最中に更新する対角要素
      m_vals[i] = m_mat[i][i];
      // 掃き出しが終わった後に更新するための対角要素
      diagonals[i] = m_mat[i][i];
      diff_terms[i] = 0.0;
    }

    // 最大50回の掃き出しで行列を対角化する
    static const int LIMIT = 50;
    for (int no_iters = 1; no_iters <= LIMIT; no_iters++) {
      // 非対角成分の密度を絶対値の総和で表す
      double total_density = 0.0;
      for (int p = 0; p < m_n - 1; p++) {
        for (int q = p + 1; q < m_n; q++) {
          total_density += std::abs(m_mat[p][q]);
        }
      }
      if (total_density == 0.0) {
        // 非対角要素の密度が完全に 0 であるとき，それは対角行列である．
        eigensort(m_vals, m_vecs);
        return;
      }

      // 最初の三回の掃き出しで P_{pq}(φ) 回転を行うかどうかを決定する閾値
      // Note: 参考にしたプログラムでは以下のように密度の 1/5 に設定
      //       していたが，なぜ 1/5 なのかはよく分からなかった．
      double thresh = no_iters <= 3 ? 0.2 * total_density / (m_n * m_n) : 0.0;

      // 上三角行列の (p, q) 要素が零になるように P_{pq}(φ) 回転を繰り返す
      for (int p = 0; p < m_n - 1; p++) {
        for (int q = p + 1; q < m_n; q++) {
#ifdef DEBUG
          std::cerr << m_nrot << "回転後:\n";
          m_mat.dbg_print();
#endif

          // 引数 x に対して，|mat_pq| << |x| であれば true を返すクロージャ
          // 具体的には，100 · |mat_pq| ≤ ε · x（ε は計算機イプシロン）
          // であれば十分に小さいとみなす．
          auto mat_pq_is_tiny_compared_to =
              [target = 100.0 * std::abs(m_mat[p][q])](double value) -> bool {
            return target <=
                   std::numeric_limits<double>::epsilon() * std::abs(value);
          };

          if (no_iters > 4) {
            // 非対角要素が十分小さいときは 0 に設定して回転をスキップする．
            if (mat_pq_is_tiny_compared_to(m_vals[p]) &&
                mat_pq_is_tiny_compared_to(m_vals[q])) {
              m_mat[p][q] = 0.0;
              continue;
            }
          }

          if (std::abs(m_mat[p][q]) > thresh) {
            // tan φ を計算して各要素の値を更新する
            double tan_phi;
            // P_{pq}(φ) による相似変換後の (p, q) 要素 mat'_pq は，
            // mat'_pq = (cos²φ - sin²φ) mat_pq + sin φ cos φ (mat_pp - mat_qq).
            // であり，mat'_pq = 0 になるように tan φ を定める．
            // mat'_pq = 0
            // <=> (mat_qq - mat_pp) / (2 mat_pq)
            //      = (cos²φ - sin²φ) / (2 sin φ cos φ)
            //      = cot 2φ
            //      = (1 - tan²φ) / (2 tan φ)
            // φ の絶対値が小さくなるように tan φ について解くと，
            // tan φ = |cot 2φ| / (|cot 2φ| + √(cot²2φ + 1))
            double cot_2phi_numerator = m_vals[q] - m_vals[p];
            if (mat_pq_is_tiny_compared_to(cot_2phi_numerator)) {
              // しかし，cot 2φ は mat_pq を含む分子の項が分母の項より十分に
              // 小さい（|mat_pq| << |mat_qq - mat_pp|）ときにオーバーフロー
              // を引き起こす．これを考慮に入れて tan φ ≈ φ と近似すると，
              // cot 2φ = 1 / tan 2φ ≈ 1 / 2φ
              // ∴ φ ≈ 1 / (2 cot 2φ) = mat_pq / (mat_qq - mat_pp)
              tan_phi = (m_mat[p][q]) / cot_2phi_numerator;
            } else {
              double cot_2phi = 0.5 * cot_2phi_numerator / m_mat[p][q];
              tan_phi =
                  1.0 / (std::abs(cot_2phi) + sqrt(1.0 + cot_2phi * cot_2phi));
              if (cot_2phi < 0.0) {
                tan_phi = -tan_phi;
              }
            }

            // (p, p), (q, q) 要素の更新
            double diff_term = tan_phi * m_mat[p][q];
            diff_terms[p] -= diff_term, m_vals[p] -= diff_term;
            diff_terms[q] += diff_term, m_vals[q] += diff_term;

            // (p, q) 要素をゼロに設定する:
            // これがヤコビ回転変換の目的の操作で他は辻褄合わせ
            m_mat[p][q] = 0.0;

            // (p, p), (p, q), (q, q) 以外の要素は，以下の行列の
            // 添字 (a), (b), (c) の組に対して回転操作を行う
            //
            // ```
            //             0,p         0,q
            //              :           :
            //             (a)         (a)
            //              :           :
            // p,0 ······· p,p ··(b)·· p,q ··(c)·· p,n-1
            //              :           :
            //              :          (b)
            //              :           :
            // q,0 ······· q,p ······· q,q ··(c)·· q,n-1
            //              :           :
            //              :           :
            //              :           :
            //            n-1,p       n-1,q
            // ```

            // rotate (a): mat[0..p-1][p] <--> mat[0..p-1][q]
            for (int i = 0; i < p; i++) {
              rot(m_mat, tan_phi, i, p, i, q);
            }
            // rotate (b): mat[p][p+1..q-1] <--> mat[p+1..q-1][q]
            for (int i = p + 1; i < q; i++) {
              rot(m_mat, tan_phi, p, i, i, q);
            }
            // rotate (c): mat[p][q+1..n-1] <--> mat[q][q+1..n-1]
            for (int i = q + 1; i < m_n; i++) {
              rot(m_mat, tan_phi, p, i, q, i);
            }
            // P_{nrot+1} = P_{nrot} · P を求める
            // 右から掛けて列pと列qを更新: vecs[1..n-1][p] <--> vecs[1..n-1][q]
            for (int i = 0; i < m_n; i++) {
              rot(m_vecs, tan_phi, i, p, i, q);
              // mat の場合と異なり，(p, p), (p, q), (q, p), (q, q) 要素も特別
              // 扱いせずに rot 関数を使って回転操作を行っている．vecs に対して
              // は右から回転行列を掛けてるだけで，mat のように左からさらにその
              // 転置行列は掛けてはいない．
            }
            ++m_nrot;
          }
        }
      }
      // 一連の回転操作が終了したら，対角要素を更新する
      // mat_pp := mat_pp - Σ_φ tan φ mat_pq
      for (int i = 0; i < m_n; i++) {
        diagonals[i] += diff_terms[i];
        m_vals[i] = diagonals[i];
        diff_terms[i] = 0.0;
      }
    }

    throw(std::ostringstream() << "ERROR: 反復処理の繰り返し回数が許容限度（"
                               << LIMIT << "回）を超えました．")
        .str();
  }

  // (i, j) 要素と (k, l) 要素の φ 回転
  //
  // { mat'_ij = cos φ mat_ij - sin φ mat_kl
  // { mat'_kl = sin φ mat_ij + cos φ mat_kl
  // <=> { mat'_ij = mat_ij + (cos φ - 1) mat_ij - sin φ mat_kl
  //     { mat'_kl = mat_kl + sin φ mat_ij + (cos φ - 1) mat_kl
  // <=> { mat'_ij = mat_ij - sin φ (sin φ / (cos φ + 1) mat_ij + mat_kl)
  //     { mat'_kl = mat_kl + sin φ (mat_ij - sin φ / (cos φ + 1) mat_kl)
  // <=> { mat'_ij = mat_ij - sin φ (mat_kl + τ mat_ij)
  //     { mat'_kl = mat_kl + sin φ (mat_ij - τ mat_kl)
  //     where τ = sin φ / (1 + cos φ)
  static inline void rot(Mat &mat, const double tan_phi, const int i,
                         const int j, const int k, const int l) {
    double cos_phi = 1.0 / sqrt(1 + tan_phi * tan_phi);
    double sin_phi = tan_phi * cos_phi;
    double tau = sin_phi / (1.0 + cos_phi);

    double prev_mat_ij = mat[i][j];
    double prev_mat_kl = mat[k][l];
    mat[i][j] = prev_mat_ij - sin_phi * (prev_mat_kl + prev_mat_ij * tau);
    mat[k][l] = prev_mat_kl + sin_phi * (prev_mat_ij - prev_mat_kl * tau);
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

#endif
