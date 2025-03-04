#CMake overlay for the upstream project. Tests not available.
#Dependencies: sqlite3(libsqlite3.so)

git clone https://github.com/Isfand/rcb.git upstream
mkdir -p build
cd build
cmake ..
make
