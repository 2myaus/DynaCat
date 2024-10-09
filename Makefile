# Compiler
CXX := g++
CXXFLAGS := -std=c++17

# Files
SRC := structures.cpp primitives.cpp
HEADERS := primitives.hpp structures.hpp

# Output
OUT := main

# Profiles
CXXFLAGS_DEBUG := -g -O0 -DDEBUG
CXXFLAGS_DEFAULT := -O2

# Default target
all: default

# Default profile
default: CXXFLAGS += $(CXXFLAGS_DEFAULT)
default: $(OUT)

# Debug profile
debug: CXXFLAGS += $(CXXFLAGS_DEBUG)
debug: $(OUT)

# Build executable
$(OUT): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

# Clean up
clean:
	rm -f $(OUT)

# Phony targets
.PHONY: all default debug clean

