#Makefile overlay for the upstream project. Tests not available.
#Dependencies: sqlite3(libsqlite3.so)

#Clone the upstream
git clone https://github.com/Isfand/rcb.git upstream

#Choose between release or debug options

#Release build
make CXXFLAGS="-O2 -DNDEBUG" LDFLAGS="-s"

#Debug build
make CXXFLAGS="-O0 -g -Wall -Wextra -Wshadow -fno-inline -fno-omit-frame-pointer -fno-unroll-loops -fno-vectorize"

