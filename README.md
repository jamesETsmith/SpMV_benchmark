# SpMV_benchmark
A benchmark of sparse matrix dense vector multiplication in C++ using homebuilt and pre-packaged methods.

### Dependencies
- CMake
- Eigen
- Cyclops Tensor Framework

### Building
```bash
mkdir build
cd build
cmake ..
make
```
__Note: The Eigen test may generate a few warnings.__:

### Running the tests
To run all the tests at once you can use the bash script provided in the main repo directory.
```bash
cd /path/to/SpMV_benchmark
./run_all_tests.sh
```

To change the size of the problem (the dimension of the matrix) you can change the `N` variable in `run_all_tests.sh`.

If you want to run the tests individually you can run them from the command line with a single __integer__ argument that specifies the size of the matrix.
Ex.

Input:
```bash
./native 100000000
```
Output:
```
Homemade sparse Matrix Mult. Time 1666 (milliseconds) for matrix of size 100000000 x 100000000
```
