# Makefile shell
SHELL := /bin/bash

# Compiler
CC = g++

# Compiler Flags
CFLAGS = -I./include -O3 -mavx2 -mfma

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
EVAL = evaluation

# Source Files
SRC_MAIN = $(SRC_DIR)/$(MAIN).cpp
SRC_EVAL = $(SRC_DIR)/$(EVAL).cpp


# Object files (build directory)
OBJ_MAIN = $(BUILD_DIR)/$(MAIN).o
OBJ_EVAL = $(BUILD_DIR)/$(EVAL).o
OBJS_ALL = $(OBJ_MAIN) $(OBJ_EVAL)

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
INCLUDE_EVAL = $(HEADER_INTERFACE)


# Dependencies
DEPENDENCIES_MAIN = $(SRC_MAIN) $(INCLUDE_MAIN)
DEPENDENCIES_EVAL = $(SRC_EVAL) $(INCLUDE_EVAL)


# Executables (bin directory)
EX_MAIN = $(BIN_DIR)/$(MAIN)
EX_EVAL = $(BIN_DIR)/$(EVAL)

# Default target
default: clean dirs $(EX_MAIN) $(EX_EVAL)

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

run_eval:
	make
	$(EX_EVAL)

# Rules to build the executables
$(EX_MAIN): $(OBJ_MAIN)
	@echo "Linking Object Files to Create Executable . . ."
	$(CC) $(CFLAGS) $(OBJ_MAIN) -o $(EX_MAIN) $(LDFLAGS)

$(EX_EVAL): $(OBJ_EVAL)
	@echo "Linking Object Files to Create Executable . . ."
	$(CC) $(CFLAGS) $(OBJ_EVAL) -o $(EX_EVAL) $(LDFLAGS)

# Rule to build main.o
$(OBJ_MAIN): $(DEPENDENCIES_MAIN)
	@echo "Building $(MAIN) . . . "
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(SRC_MAIN) -o $(OBJ_MAIN)
# Rule to build eval.o
$(OBJ_EVAL): $(DEPENDENCIES_EVAL)
	@echo "Building $(EVAL) . . . "
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(SRC_EVAL) -o $(OBJ_EVAL)


clean:
	@echo "Cleaning up $(BUILD_DIR) and $(BIN_DIR) directories . . . "
# Clean build, bin directories
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: clean