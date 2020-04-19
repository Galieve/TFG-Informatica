./configure CXXFLAGS="-mtune=native -fsanitize=undefined"
make
sudo make install
mkdir -p files