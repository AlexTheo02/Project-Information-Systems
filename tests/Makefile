# Makefile shell
SHELL := /bin/bash

# Compiler
CC = g++

# Compiler Flags
CFLAGS = -I./include -I../include

# Linker Flags
LDFLAGS = -pthread -lm

# Directory Names
SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin
INCLUDE_DIR = ./include

# File Names
TYPES = types
UTIL = util
UTIL_T = util_types
GS = greedy_search
RP = robust_prune
VG = vamana_graph

t_TYPES = test_Types
t_UTIL = test_Util
t_UT = test_UtilTypes

# Source Files
SRC_t_TYPES = $(SRC_DIR)/$(t_TYPES).cpp
SRC_t_UTIL = $(SRC_DIR)/$(t_UTIL).cpp
SRC_t_UT = $(SRC_DIR)/$(t_UT).cpp

# Object files (build directory)
OBJ_UTIL = .$(BUILD_DIR)/$(UTIL).o
OBJ_UTIL_T = .$(BUILD_DIR)/$(UTIL_T).o
OBJ_GS = .$(BUILD_DIR)/$(GS).o
OBJ_RP = .$(BUILD_DIR)/$(RP).o
OBJ_VG = .$(BUILD_DIR)/$(VG).o

OBJ_t_TYPES = $(BUILD_DIR)/$(t_TYPES).o
OBJ_t_UTIL = $(BUILD_DIR)/$(t_UTIL).o
OBJ_t_UT = $(BUILD_DIR)/$(t_UT).o

# Executables (bin directory)
EX_t_TYPES = $(BIN_DIR)/$(t_TYPES)
EX_t_UTIL = $(BIN_DIR)/$(t_UTIL)
EX_t_UT = $(BIN_DIR)/$(t_UT)

# Default target
default: $(EX_t_UTIL) $(EX_t_TYPES) $(EX_t_UT)

run_u:
	$(EX_t_UTIL)

run_t:
	$(EX_t_TYPES)

run_ut:
	$(EX_t_UT)

# Build Executable test_Types
$(EX_t_TYPES): $(OBJ_t_TYPES)
	@echo "Linking Object Files to Create Executable $(EX_t_TYPES) . . ."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build Executable test_Util
$(EX_t_UTIL): $(OBJ_t_UTIL) $(OBJ_UTIL)
	@echo "Linking Object Files to Create Executable $(EX_t_UTIL) . . ."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build Executable test_UtilTypes
$(EX_t_UT): $(OBJ_t_UT) $(OBJ_UTIL_T)
	@echo "Linking Object Files to Create Executable $(EX_t_UT) . . ."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


# Rules to build test_Types.o
$(OBJ_t_TYPES): $(SRC_t_TYPES) $(INCLUDE_DIR)/acutest.h
	@echo "Building $(t_TYPES) . . . "
	$(CC) $(CFLAGS) -c $< -o $@

# Rules to build test_Util.o
$(OBJ_t_UTIL): $(SRC_t_UTIL) $(INCLUDE_DIR)/acutest.h
	@echo "Building $(t_UTIL) . . . "
	$(CC) $(CFLAGS) -c $< -o $@

# Rules to build test_UtilTypes.o
$(OBJ_t_UT): $(SRC_t_UT) $(INCLUDE_DIR)/acutest.h
	@echo "Building $(t_UT) . . . "
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up $(BUILD_DIR) and $(BIN_DIR) directories . . . "
	# Clean build, bin directories
	rm -f $(BUILD_DIR)/* $(BIN_DIR)/*
	touch ./bin/dummy.txt
	touch ./build/dummy.txt

.PHONY: clean
