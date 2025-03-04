#CMake overlay for the upstream project. Tests not available.

git clone https://github.com/Isfand/rcb.git upstream
mkdir -p build
cd build
cmake ..
make
