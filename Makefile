# Makefile shell
SHELL := /bin/bash

# Compiler
CC = g++

# Compiler Flags
CFLAGS = -I./include -O3

# Linker Flags
LDFLAGS = -pthread -lm -std=c++17

# Directory Names
SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin
TESTS_DIR = ./tests
INCLUDE_DIR = ./include

# File Names
TYPES = types
UTIL = util
CONFIG = config
GRAPH = graph_implementation
FGRAPH = filtered_graph_implementation
NQ = node_and_query
INTERFACE = interface

MAIN = main

# Source Files
SRC_MAIN = $(SRC_DIR)/$(MAIN).cpp

# Object files (build directory)
OBJ_MAIN = $(BUILD_DIR)/$(MAIN).o
OBJS_ALL = $(OBJ_MAIN)

# Header Files
HEADER_UTIL = $(INCLUDE_DIR)/$(UTIL).hpp
HEADER_TYPES = $(INCLUDE_DIR)/$(TYPES).hpp
HEADER_CONFIG = $(INCLUDE_DIR)/$(CONFIG).hpp
HEADER_GRAPH = $(INCLUDE_DIR)/$(GRAPH).hpp
HEADER_FGRAPH = $(INCLUDE_DIR)/$(FGRAPH).hpp
HEADER_NQ = $(INCLUDE_DIR)/$(NQ).hpp
HEADER_INTERFACE = $(INCLUDE_DIR)/$(INTERFACE).hpp

# Include
INCLUDE_MAIN = $(HEADER_INTERFACE)


# Dependencies
DEPENDENCIES_MAIN = $(SRC_MAIN) $(INCLUDE_MAIN)


# Executables (bin directory)
EX_MAIN = $(BIN_DIR)/$(MAIN)

# Default target
default: clean dirs $(EX_MAIN)

dirs:
	@mkdir -p bin
	@mkdir -p build

run_v:
	make
	$(EX_MAIN) --vamana 

run_f:
	make
	$(EX_MAIN) --filtered

run_s:
	make
	$(EX_MAIN) --stitched

all: clean dirs $(EX_MAIN)
	@echo "Executing $(EX_MAIN):"
	$(EX_MAIN)

# Rules to build the executables
$(EX_MAIN): $(OBJS_ALL)
	@echo "Linking Object Files to Create Executable . . ."
	$(CC) $(CFLAGS) $(OBJS_ALL) -o $(EX_MAIN) $(LDFLAGS)

# Rule to build main.o
$(OBJ_MAIN): $(DEPENDENCIES_MAIN)
	@echo "Building $(MAIN) . . . "
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(SRC_MAIN) -o $(OBJ_MAIN)

clean:
	@echo "Cleaning up $(BUILD_DIR) and $(BIN_DIR) directories . . . "
# Clean build, bin directories
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: clean