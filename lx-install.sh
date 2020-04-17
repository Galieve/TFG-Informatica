./configure CXXFLAGS="-mtune=native -std=c++17 -pthread -fsanitize=undefined"
make
sudo make install
mkdir files