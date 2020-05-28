mkdir -p main_directory
./configure CXXFLAGS="" --prefix=$PWD
make
sudo make install
mkdir -p files
cd files
mkdir -p log circuit production production_flush