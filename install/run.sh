module load cmake/3.12.3
module load gcc/9.2
module load python/2.7.13

export CC=gcc
export CXX=g++

export LANG=en_US


#rm -rf ../build


cmake -S . -B../build

## build cpp_cgns and unit tests ##
cmake --build ../build -- -j8
## build only the library, not the unit tests ##
#cmake --build ../build --target cpp_cgns -- -j8


## launch unit tests ##
cd ../build
./cpp_cgns_unit_tests
