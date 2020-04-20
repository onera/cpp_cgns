module load cmake/3.12.3
module load gcc/9.2
module load python/2.7.13

export CC=gcc
export CXX=g++

cd ..
export PROJECT_ROOT=$(pwd)/../..
cmake -S . -Bbuild -DCMAKE_INSTALL_PREFIX:PATH=/scratchm/bberthou/dist/test_cpp_cgns

## build cpp_cgns and unit tests ##
cmake --build build/ -j 24 --target install --config Release
## build only the library, not the unit tests ##
#cmake --build ../build --target cpp_cgns -- -j8


## launch unit tests ##
#cd build
#./cpp_cgns_unit_tests
