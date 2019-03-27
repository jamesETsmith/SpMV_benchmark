#define EIGEN_USE_MKL_ALL
#include <omp.h>
#include <stdio.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <chrono>
// #include <numeric>
// #include <tuple>
#include <iostream>
#include <vector>
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::Triplet<double> T;

double diag = 3.0;
double off_diag = 1.7;
double off_off_diag = 1.5;

void create_matrix(std::vector<T>& coeffs, SpMat& spm, const int mat_size) {
  /*
  Populate the sparse matrix for multiplication.
  */
  for (int i = 0; i < mat_size; i++) {
    coeffs.push_back(T(i, i, diag * (i + 1)));
    if (i + 1 < mat_size) {
      coeffs.push_back(T(i, i + 1, off_diag * (i + 1)));
      coeffs.push_back(T(i + 1, i, off_diag * (i + 1)));
      if (i + 2 < mat_size) {
        coeffs.push_back(T(i, i + 2, off_off_diag / (i + 1)));
        coeffs.push_back(T(i + 2, i, off_off_diag / (i + 1)));
      }
    }
  }
  spm.setFromTriplets(coeffs.begin(), coeffs.end());
}

void sparse_mv_mult(SpMat& spm, Eigen::VectorXd& b, Eigen::VectorXd& c,
                    const int mat_size) {
  /*
  Sparse matrix * dense vector mutliplication. With OMP support.
  */

  // TIMING
  auto start = std::chrono::high_resolution_clock::now();
  // TIMING

  // #pragma omp parallel for
  //   for (int k = 0; k < spm.size(); k++) {
  // #pragma omp atomic update
  //     // c[std::get<0>(spm[k])] += std::get<2>(spm[k]) *
  //     b[std::get<1>(spm[k])];
  //   }

#pragma omp parallel
  c = spm * b;

  // TIMING
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  printf(
      "Eigen sparse Matrix Mult. Time %llu (milliseconds) for matrix of size "
      "%d x "
      "%d\n",
      duration.count(), mat_size, mat_size);
  // TIMING
}

int main(int argc, char** argv) {
  // Read command line options
  if (argc < 2) {
    printf(
        "Not enough arguments, use: \n\n./eigen_test N \n\nwhere N is the size "
        "of "
        "the "
        "matrix dimension\n");
    return -1;
  }
  const int mat_size = std::atoi(argv[1]);

  Eigen::initParallel();
  //
  Eigen::VectorXd b = Eigen::VectorXd::LinSpaced(mat_size, 0, mat_size - 1);
  std::vector<T> coefficients;  // list of non-zeros coefficients
  SpMat spm(mat_size, mat_size);
  create_matrix(coefficients, spm, mat_size);

  Eigen::VectorXd c = Eigen::VectorXd::Zero(mat_size);
  // c.setZero();

  sparse_mv_mult(spm, b, c, mat_size);
  // std::cout << c << "\n";

  // DEBUGGING
  // print_matrix(spm);
  // for (int i = 0; i < mat_size; i++) {
  //   printf("%f\n", b[i]);
  // }
  // printf("\n\n");
  // for (int i = 0; i < mat_size; i++) {
  //   printf("%f\n", c[i]);
  // }
  // DEBUGGING
  return 0;
}
