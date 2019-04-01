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
  // TIMING
  auto start = std::chrono::high_resolution_clock::now();
  // TIMING

  // Popluating
  int reg_size = (int)mat_size / 3.0;

#pragma omp declare reduction( \
    merge:std::vector<T>             \
    : omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))
#pragma omp parallel for reduction(merge : coeffs)
  for (int i = 1; i < reg_size; i++) {
    for (int j = 1; j < 5; j++) {
      // Region 1
      if (i * j < reg_size) {
        coeffs.push_back({i, i * j, log(i + j + 1.0)});
      }
      // Region 2
      if ((i + reg_size + j) % 2 == 0 && i + j < reg_size) {
        coeffs.push_back(
            {i + reg_size, i + reg_size + j, log(i + reg_size + j + 1)});
        coeffs.push_back(
            {i + reg_size + j, i + reg_size, log(i + reg_size + j + 1)});
      }
      // Region 3
      if (i + 2 * j < reg_size) {
        coeffs.push_back({i + reg_size, i + 2 * reg_size + 2 * j,
                          log(i + 2 * reg_size + j + 1)});
        coeffs.push_back({i + 2 * j + reg_size, i + 2 * reg_size,
                          log(i + 2 * reg_size + j + 1)});
        coeffs.push_back({i + 2 * reg_size, i + 2 * j + reg_size,
                          log(i + 2 * reg_size + j + 1)});
        coeffs.push_back({i + 2 * reg_size + 2 * j, i + reg_size,
                          log(i + 2 * reg_size + j + 1)});
      }
    }
  }

  spm.setFromTriplets(coeffs.begin(), coeffs.end());

  // TIMING
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  printf(
      "Eigen matrix population time %llu (milliseconds) for matrix of "
      "size %d x %d\n",
      duration.count(), mat_size, mat_size);
  // TIMING
}

void sparse_mv_mult(SpMat& spm, Eigen::VectorXd& b, Eigen::VectorXd& c,
                    const int mat_size) {
  /*
  Sparse matrix * dense vector mutliplication.
  */

  // TIMING
  auto start = std::chrono::high_resolution_clock::now();
  // TIMING

  c = spm * b;

  // TIMING
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  printf(
      "Eigen sparse matrix mult. time %llu (milliseconds) for matrix of size "
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
