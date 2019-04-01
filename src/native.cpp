#include <math.h> /* log */
#include <omp.h>
#include <stdio.h>
#include <chrono>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

typedef std::tuple<int, int, double> TripleType;
typedef std::vector<TripleType> SpMatrix;

void create_matrix(SpMatrix &spm, const int mat_size) {
  /*
  Populate the sparse matrix for multiplication.
  */
  // TIMING
  auto start = std::chrono::high_resolution_clock::now();
  // TIMING

  // Popluating
  int reg_size = (int)mat_size / 3.0;

#pragma omp declare reduction( \
    merge:SpMatrix             \
    : omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))
#pragma omp parallel for reduction(merge : spm)
  for (int i = 1; i < reg_size; i++) {
    for (int j = 1; j < 5; j++) {
      // Region 1
      if (i * j < reg_size) {
        spm.push_back({i, i * j, log(i + j + 1.0)});
      }
      // Region 2
      if ((i + reg_size + j) % 2 == 0 && i + j < reg_size) {
        spm.push_back(
            {i + reg_size, i + reg_size + j, log(i + reg_size + j + 1)});
        spm.push_back(
            {i + reg_size + j, i + reg_size, log(i + reg_size + j + 1)});
      }
      // Region 3
      if (i + 2 * j < reg_size) {
        spm.push_back({i + reg_size, i + 2 * reg_size + 2 * j,
                       log(i + 2 * reg_size + j + 1)});
        spm.push_back({i + 2 * j + reg_size, i + 2 * reg_size,
                       log(i + 2 * reg_size + j + 1)});
        spm.push_back({i + 2 * reg_size, i + 2 * j + reg_size,
                       log(i + 2 * reg_size + j + 1)});
        spm.push_back({i + 2 * reg_size + 2 * j, i + reg_size,
                       log(i + 2 * reg_size + j + 1)});
      }
    }
  }

  // TIMING
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  printf(
      "Homemade matrix population time %llu (milliseconds) for matrix of "
      "size %d x %d\n",
      duration.count(), mat_size, mat_size);
  // TIMING
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

int sparse_mv_mult(SpMatrix &spm, std::vector<double> &b,
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
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  printf(
      "Homemade sparse matrix mult. time %llu (milliseconds) for matrix of "
      "size %d x "
      "%d\n",
      duration.count(), mat_size, mat_size);
  // TIMING
  return duration.count();
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
  SpMatrix spm;
  create_matrix(spm, mat_size);
  std::vector<double> b(mat_size);
  std::iota(std::begin(b), std::end(b), 0);
  std::vector<double> c(mat_size);

  sparse_mv_mult(spm, b, c, mat_size);
  // int repeats = 10;
  // double counts = 0;
  // for (int i = 0; i < repeats; i++) {
  //   counts += sparse_mv_mult(spm, b, c, mat_size);
  // }
  // counts /= repeats;
  // printf("Avg. time for matrix multiplication: %f\n", counts);

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
