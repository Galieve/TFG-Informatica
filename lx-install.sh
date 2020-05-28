./configure CXXFLAGS="" --prefix=$PWD
make
make install
mkdir -p files
cd files
mkdir -p log circuit production production_flush