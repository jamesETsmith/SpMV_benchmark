#include <omp.h>
#include <stdio.h>
#include <chrono>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include <ctf.hpp>
using namespace CTF;
typedef std::tuple<int, int, double> TripleType;
typedef std::vector<TripleType> SpMatrix;

double diag = 3.0;
double off_diag = 1.7;
double off_off_diag = 1.5;

void create_matrix(SpMatrix &spm, const int mat_size) {
  /*
  Populate the sparse matrix for multiplication.
  */
  for (int i = 0; i < mat_size; i++) {
    spm.push_back({i, i, diag * (i + 1)});
    if (i + 1 < mat_size) {
      spm.push_back({i, i + 1, off_diag * (i + 1)});
      spm.push_back({i + 1, i, off_diag * (i + 1)});
      if (i + 2 < mat_size) {
        spm.push_back({i, i + 2, off_off_diag / (i + 1)});
        spm.push_back({i + 2, i, off_off_diag / (i + 1)});
      }
    }
  }
}

void print_matrix(SpMatrix &spm, const int mat_size) {
  /*
  Print the sparse matrix
  */
  bool found_val = false;
  for (int i = 0; i < mat_size; i++) {
    for (int j = 0; j < mat_size; j++) {
      found_val = false;
      for (int k = 0; k < spm.size(); k++) {
        if (std::get<0>(spm[k]) == i && std::get<1>(spm[k]) == j) {
          printf("%8.4f\t", std::get<2>(spm[k]));
          found_val = true;
          break;
        }
      }
      if (found_val == false) {
        printf("%8.4f\t", 0.0);
      }
    }
    printf("\n");
  }
}

void sparse_mv_mult(SpMatrix &spm, std::vector<double> &b,
                    std::vector<double> &c, const int mat_size) {
  /*
  Sparse matrix * dense vector mutliplication. With OMP support.
  */

  // TIMING
  auto start = std::chrono::high_resolution_clock::now();
  // TIMING

#pragma omp parallel for
  for (int k = 0; k < spm.size(); k++) {
#pragma omp atomic update
    c[std::get<0>(spm[k])] += std::get<2>(spm[k]) * b[std::get<1>(spm[k])];
  }

  // TIMING
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  printf(
      "Sparse Matrix Mult. Time %llu (microseconds) for matrix of size %d x "
      "%d\n",
      duration.count(), mat_size, mat_size);
  // TIMING
}

int main(int argc, char **argv) {
  // Read command line options
  if (argc < 2) {
    printf(
        "Not enough arguments, use: \n\n./native N \n\nwhere N is the size of "
        "the "
        "matrix dimension\n");
    return -1;
  }
  const int mat_size = std::atoi(argv[1]);

  // Matrix setup and multiplication
  Matrix<> spm(mat_size, mat_size, SP);
  Vector<> b(mat_size);

  // SpMatrix spm;
  // create_matrix(spm, mat_size);
  // std::vector<double> b(mat_size);
  // std::iota(std::begin(b), std::end(b), 0);
  // std::vector<double> c(mat_size);
  // sparse_mv_mult(spm, b, c, mat_size);

  // DEBUGGING
  // print_matrix(spm, mat_size);
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
