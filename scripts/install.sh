module load cmake/3.12.3
module load gcc/9.2
module load python/2.7.13

export CC=gcc
export CXX=g++

cd ..
export PROJECT_ROOT=$(pwd)/../..
cmake -S . -Bbuild/ \
  -DCMAKE_INSTALL_PREFIX:PATH=${PREFIX_PATH} \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=17 \
  -DCMAKE_CXX_FLAGS="-Wall -std=c++17 -fmax-errors=4" \
  -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -DNDEBUG -fopenmp -fPIC" && \

echo -e "\033[31m" && date && echo -e "\e[0m" && \
## build cpp_cgns and unit tests ##
cmake --build build/ -j 24
## build only the library, not the unit tests ##
#cmake --build ../build --target cpp_cgns -- -j8
echo -e "\033[31m" && date && echo -e "\e[0m" && \

## Build and install
#cmake --build build/ -j 24 --target install --config Release

## launch unit tests ##
(cd build && \
echo && \
echo "std_e_unit_tests" && \
./external/std_e/std_e_unit_tests && \
echo && \
echo "cpp_cgns_unit_tests" && \
./cpp_cgns_unit_tests)
