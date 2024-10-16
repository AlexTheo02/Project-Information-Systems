# Makefile shell
SHELL := /bin/bash

# Compiler
CC = g++

# Compiler Flags
CFLAGS = -I./include

# Linker Flags
LDFLAGS = -pthread -lm

# Directory Names
SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin
TESTS_DIR = ./tests
INCLUDE_DIR = ./include

# File Names
TYPES = types
UTIL = util
UTIL_T = util_types
GS = greedy_search
RP = robust_prune
VG = vamana_graph

MAIN = main

# Source Files
SRC_UTIL = $(SRC_DIR)/$(UTIL).cpp
SRC_UTIL_T = $(SRC_DIR)/$(UTIL_T).cpp
SRC_GS = $(SRC_DIR)/$(GS).cpp
SRC_RP = $(SRC_DIR)/$(RP).cpp
SRC_VG = $(SRC_DIR)/$(VG).cpp

SRC_MAIN = $(SRC_DIR)/$(MAIN).cpp

# Object files (build directory)
OBJ_UTIL = $(BUILD_DIR)/$(UTIL).o
OBJ_UTIL_T = $(BUILD_DIR)/$(UTIL_T).o
OBJ_GS = $(BUILD_DIR)/$(GS).o
OBJ_RP = $(BUILD_DIR)/$(RP).o
OBJ_VG = $(BUILD_DIR)/$(VG).o

OBJ_MAIN = $(BUILD_DIR)/$(MAIN).o

OBJS_ALL = $(OBJ_MAIN) $(OBJ_UTIL) $(OBJ_UTIL_T) $(OBJ_GS) $(OBJ_RP) $(OBJ_VG)

# Header Files
HEADER_UTIL = $(INCLUDE_DIR)/$(UTIL).hpp
HEADER_UTIL_T = $(INCLUDE_DIR)/$(UTIL_T).hpp
HEADER_TYPES = $(INCLUDE_DIR)/$(TYPES).hpp

# Include
INCLUDE_UTIL = $(HEADER_UTIL)
INCLUDE_UTIL_T = $(HEADER_UTIL_T) $(HEADER_TYPES)
INCLUDE_GS = $(HEADER_UTIL) $(HEADER_UTIL_T)
INCLUDE_RP = $(HEADER_UTIL) $(HEADER_UTIL_T) 
INCLUDE_VG = $(HEADER_UTIL) $(HEADER_UTIL_T)

INCLUDE_MAIN = $(HEADER_UTIL) $(HEADER_UTIL_T) $(HEADER_TYPES)


# Dependencies
DEPENDENCIES_UTIL = $(SRC_UTIL) $(INCLUDE_UTIL)
DEPENDENCIES_UTIL_T = $(SRC_UTIL_T) $(INCLUDE_UTIL_T)
DEPENDENCIES_GS = $(SRC_GS) $(INCLUDE_GS)
DEPENDENCIES_RP = $(SRC_RP) $(INCLUDE_RP)
DEPENDENCIES_VG = $(SRC_VG) $(INCLUDE_VG)

DEPENDENCIES_MAIN = $(SRC_MAIN) $(SRC_GS) $(SRC_RP) $(SRC_VG) $(INCLUDE_MAIN)


# Executables (bin directory)
EX_MAIN = $(BIN_DIR)/$(MAIN)

# Default target
default: dirs $(EX_MAIN)

dirs:
	@mkdir -p bin
	@mkdir -p build

run:
	dirs
	$(EX_MAIN)

all: clean dirs $(EX_MAIN)
	@echo "Executing $(EX_MAIN):"
	$(EX_MAIN)
	


# Rules to build the executables
$(EX_MAIN): $(OBJS_ALL)
	@echo "Linking Object Files to Create Executable . . ."
	$(CC) $(CFLAGS) $(OBJS_ALL) -o $(EX_MAIN) $(LDFLAGS)

# Rule to build util.o
$(OBJ_UTIL): $(DEPENDENCIES_UTIL)
	@echo "Building $(UTIL) . . . "
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(SRC_UTIL) -o $(OBJ_UTIL)

# Rule to build util_types.o
$(OBJ_UTIL_T): $(DEPENDENCIES_UTIL_T)
	@echo "Building $(UTIL_T) . . . "
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(SRC_UTIL_T) -o $(OBJ_UTIL_T)

# Rule to build greedy_search.o
$(OBJ_GS): $(DEPENDENCIES_GS)
	@echo "Building $(GS) . . . "
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(SRC_GS) -o $(OBJ_GS)

# Rule to build robust_prune.o
$(OBJ_RP): $(DEPENDENCIES_RP)
	@echo "Building $(RP) . . . "
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(SRC_RP) -o $(OBJ_RP)

# Rule to build vamana_graph.o
$(OBJ_VG): $(DEPENDENCIES_VG)
	@echo "Building $(VG) . . . "
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(SRC_VG) -o $(OBJ_VG)

# Rule to build main.o
$(OBJ_MAIN): $(DEPENDENCIES_MAIN)
	@echo "Building $(MAIN) . . . "
	$(CC) $(CFLAGS) $(LDFLAGS) -c $(SRC_MAIN) -o $(OBJ_MAIN)

clean:
	@echo "Cleaning up $(BUILD_DIR) and $(BIN_DIR) directories . . . "
# Clean build, bin directories
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: clean