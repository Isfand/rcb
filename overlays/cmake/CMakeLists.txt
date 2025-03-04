cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(rcb VERSION 1.0)

# Force the use of clang++
set(CMAKE_CXX_COMPILER "clang++")

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Clang-specific flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

# Add the include directories
include_directories(upstream)

# Manually find and add source files, explicitly excluding the tests directory
file(GLOB_RECURSE SRCS
    "*.cxx"
)

# Exclude files in upstream/tests
list(REMOVE_ITEM SRCS
    ${CMAKE_SOURCE_DIR}/upstream/tests/tests/tests.cxx
    ${CMAKE_SOURCE_DIR}/upstream/tests/tests/*.cxx
)

# Add the executable target
add_executable(${PROJECT_NAME} ${SRCS})

# Link against SQLite3
target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3)
