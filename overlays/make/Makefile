# Compiler
override CXX = clang++

# Compiler flags
override CXXFLAGS += -std=c++23 -stdlib=libc++ -Iupstream

# Linker flags (always link against SQLite3)
override LDFLAGS += -lsqlite3

# Find all C++ source files with .cxx extension recursively. Excluding tests/
SRCS := $(shell find . -type f -name "*.cxx" ! -path "./upstream/tests/*")

# Generate object files for each source file
OBJS := $(SRCS:.cxx=.o)

# Target executable
TARGET = rcb

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS)

# Compilation rule
%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
