# Compiler
CXX := g++
CXXFLAGS := -std=c++20

# Files
SRC := structures.cpp primitives.cpp main.cpp
HEADERS := primitives.hpp structures.hpp

# Output
OUT := dynacat

# Profiles
CXXFLAGS_DEBUG := -g -O0 -DDEBUG
CXXFLAGS_DEFAULT := -O2
CXXFLAGS_FAST := -O3 -Ofast -funroll-loops -march=native

# Default target
all: default

# Default profile
default: CXXFLAGS = $(CXXFLAGS_DEFAULT)
default: $(OUT)

# Debug profile
debug: CXXFLAGS = $(CXXFLAGS_DEBUG)
debug: $(OUT)

# Lightspeed!!
fast: CXXFLAGS = $(CXXFLAGS_FAST)
fast: $(OUT)

# Build executable
$(OUT): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

# Clean up
clean:
	rm -f $(OUT)

# Phony targets
.PHONY: all default debug clean

