# Makefile shell
SHELL := /bin/bash

# Compiler
CC = g++

# Compiler Flags
CFLAGS = -I./include -I../include -mavx2 -mfma

# Linker Flags
LDFLAGS = -pthread -lm -std=c++17

# Directory Names
SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin
INCLUDE_DIR = ./include
PARENT_DIR = ..
PARENT_INCLUDE_DIR = $(PARENT_DIR)/include

# File Names

t_TYPES = test_types
t_UTIL = test_util
t_CONFIG = test_config
t_FILTER = test_filtered
t_INTERFACE	= test_interface

# Source Files
SRC_t_TYPES = $(SRC_DIR)/$(t_TYPES).cpp
SRC_t_UTIL = $(SRC_DIR)/$(t_UTIL).cpp
SRC_t_CONFIG = $(SRC_DIR)/$(t_CONFIG).cpp
SRC_t_FILTER = $(SRC_DIR)/$(t_FILTER).cpp
SRC_t_INTERFACE = $(SRC_DIR)/$(t_INTERFACE).cpp

# Object files (build directory)
# no object files exist in build directory that are needed for this build

# Object files (tests/build directory)
OBJ_t_TYPES = $(BUILD_DIR)/$(t_TYPES).o
OBJ_t_UTIL = $(BUILD_DIR)/$(t_UTIL).o
OBJ_t_CONFIG = $(BUILD_DIR)/$(t_CONFIG).o
OBJ_t_FILTER = $(BUILD_DIR)/$(t_FILTER).o

# Executables (bin directory)
EX_t_TYPES = $(BIN_DIR)/$(t_TYPES)
EX_t_UTIL = $(BIN_DIR)/$(t_UTIL)
EX_t_CONFIG = $(BIN_DIR)/$(t_CONFIG)
EX_t_FILTER = $(BIN_DIR)/$(t_FILTER)
EX_ALL = $(EX_t_TYPES) $(EX_t_UTIL) $(EX_t_CONFIG) $(EX_t_FILTER) $(EX_t_INTERFACE)

DEPENDENCIES = $(INCLUDE_DIR)/acutest.h $(PARENT_INCLUDE_DIR)/interface.hpp

# Default target
default: dirs $(EX_ALL)

dirs:
	@mkdir -p bin
	@mkdir -p build

# Rebuild the main code and the test code
rebuild:
	make -C $(PARENT_DIR) clean
	make -C $(PARENT_DIR)
	make clean
	dirs
	make

u: $(EX_t_UTIL)


run_u: dirs $(EX_t_UTIL)
	$(EX_t_UTIL)

run_t: dirs $(EX_t_TYPES)
	$(EX_t_TYPES)

run_c: dirs $(EX_t_CONFIG)
	$(EX_t_CONFIG)

run_f: dirs $(EX_t_FILTER)
	$(EX_t_FILTER)

# rebuild and run specific test
rebuild_u:
	make rebuild
	make run_u

rebuild_t:
	make rebuild
	make run_t

rebuild_c:
	make rebuild
	make run_c

rebuild_f:
	make rebuild
	make run_f

run:
	make rebuild
	$(EX_t_UTIL)
	$(EX_t_TYPES)
	$(EX_t_CONFIG)
	$(EX_t_FILTER)

val:
	make rebuild
	valgrind $(EX_t_UTIL)
	valgrind $(EX_t_TYPES)
	valgrind $(EX_t_CONFIG)
	valgrind $(EX_t_FILTER)
	

# Build Executable test_Types
$(EX_t_TYPES): $(OBJ_t_TYPES)
	@echo "Linking Object Files to Create Executable $(EX_t_TYPES) . . ."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build Executable test_Util
$(EX_t_UTIL): $(OBJ_t_UTIL)
	@echo "Linking Object Files to Create Executable $(EX_t_UTIL) . . ."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build Executable test_Config
$(EX_t_CONFIG): $(OBJ_t_CONFIG)
	@echo "Linking Object Files to Create Executable $(EX_t_CONFIG) . . ."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build Executable test_filtered
$(EX_t_FILTER): $(OBJ_t_FILTER)
	@echo "Linking Object Files to Create Executable $(EX_t_FILTER) . . ."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build Executable test_interface
$(EX_t_INTERFACE): $(OBJ_t_INTERFACE)
	@echo "Linking Object Files to Create Executable $(EX_t_INTERFACE) . . ."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Rule to build test_types.o
$(OBJ_t_TYPES): $(SRC_t_TYPES) $(DEPENDENCIES)
	@echo "Building $(t_TYPES) . . . "
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

# Rule to build test_util.o
$(OBJ_t_UTIL): $(SRC_t_UTIL) $(DEPENDENCIES)
	@echo "Building $(t_UTIL) . . . "
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

# Rule to build test_config.o
$(OBJ_t_CONFIG): $(SRC_t_CONFIG) $(DEPENDENCIES)
	@echo "Building $(t_CONFIG) . . . "
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

# Rule to build test_filter.o
$(OBJ_t_FILTER): $(SRC_t_FILTER) $(DEPENDENCIES)
	@echo "Building $(t_FILTER) . . . "
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	@echo "Cleaning up $(BUILD_DIR) and $(BIN_DIR) directories . . . "
	# Clean build, bin directories
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: clean
