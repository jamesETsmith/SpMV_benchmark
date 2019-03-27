cd build
N=100000000

echo "OMP_NUM_THREADS="$OMP_NUM_THREADS
./native $N
./eigen_test $N
