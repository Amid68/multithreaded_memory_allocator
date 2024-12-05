###############################################################################
# Makefile for Multithreaded Memory Allocator Project
#
# This Makefile supports building:
# - The main allocator binary
# - Unit tests and running them
# - Benchmarks
# - Automatically generated documentation (if applicable)
#
# Features:
# - Out-of-tree builds (objects placed in build/obj, binaries in build/bin)
# - Configurable compiler flags
# - Separate targets for tests, benchmarks, and main program
# - Automatic dependency generation for source files
#
# Author: Ameed Othman
# Date: 02/12/2024
###############################################################################

# Compiler and Tools
CC          := gcc
CFLAGS      := -Wall -Wextra -Werror -O2 -g -pthread
INCLUDES    := -Iinclude -Itests -Itests/unity
LDFLAGS     :=
LDLIBS      := -pthread

# Paths
SRC_DIR     := src
INC_DIR     := include
TEST_DIR    := tests
BENCH_DIR   := benchmarks
BUILD_DIR   := build
OBJ_DIR     := $(BUILD_DIR)/obj
BIN_DIR     := $(BUILD_DIR)/bin
LIB_DIR     := $(BUILD_DIR)/lib

# Project Targets
MAIN_TARGET := $(BIN_DIR)/allocator_main
TEST_TARGETS := $(BIN_DIR)/test_allocator \
                $(BIN_DIR)/test_multithread \
                $(BIN_DIR)/test_performance \
                $(BIN_DIR)/test_utils
BENCH_TARGETS := $(BIN_DIR)/benchmark_allocator \
                 $(BIN_DIR)/benchmark_standard_malloc

# Source Files
MAIN_SRC      := $(SRC_DIR)/main.c
ALLOCATOR_SRC := $(SRC_DIR)/allocator.c
UTILS_SRC     := $(SRC_DIR)/utils.c

TEST_SRCS     := $(TEST_DIR)/test_allocator.c \
                 $(TEST_DIR)/test_multithread.c \
                 $(TEST_DIR)/test_performance.c \
                 $(TEST_DIR)/test_utils.c \
                 $(TEST_DIR)/unity.c

BENCH_SRCS    := $(BENCH_DIR)/benchmark_allocator.c \
                 $(BENCH_DIR)/benchmark_standard_malloc.c

# Derived lists
MAIN_OBJ      := $(OBJ_DIR)/main.o
ALLOCATOR_OBJ := $(OBJ_DIR)/allocator.o
UTILS_OBJ     := $(OBJ_DIR)/utils.o

TEST_OBJS     := $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.o,$(TEST_SRCS))
BENCH_OBJS    := $(patsubst $(BENCH_DIR)/%.c,$(OBJ_DIR)/%.o,$(BENCH_SRCS))

# Default target
.PHONY: all
all: $(MAIN_TARGET)

###############################################################################
# Rules for building the main program
###############################################################################

$(MAIN_TARGET): $(MAIN_OBJ) $(ALLOCATOR_OBJ) $(UTILS_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

###############################################################################
# Rules for building test executables
###############################################################################

$(BIN_DIR)/test_allocator: $(OBJ_DIR)/test_allocator.o $(OBJ_DIR)/unity.o $(ALLOCATOR_OBJ) $(UTILS_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BIN_DIR)/test_multithread: $(OBJ_DIR)/test_multithread.o $(OBJ_DIR)/unity.o $(ALLOCATOR_OBJ) $(UTILS_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BIN_DIR)/test_performance: $(OBJ_DIR)/test_performance.o $(OBJ_DIR)/unity.o $(ALLOCATOR_OBJ) $(UTILS_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BIN_DIR)/test_utils: $(OBJ_DIR)/test_utils.o $(OBJ_DIR)/unity.o $(ALLOCATOR_OBJ) $(UTILS_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: tests
tests: $(TEST_TARGETS)
	@echo "Running unit tests..."
	@for t in $(TEST_TARGETS); do \
		echo "Running $$t..."; \
		$$t || exit 1; \
	done
	@echo "All tests passed!"

###############################################################################
# Rules for building benchmark executables
###############################################################################

$(BIN_DIR)/benchmark_allocator: $(OBJ_DIR)/benchmark_allocator.o $(ALLOCATOR_OBJ) $(UTILS_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BIN_DIR)/benchmark_standard_malloc: $(OBJ_DIR)/benchmark_standard_malloc.o $(ALLOCATOR_OBJ) $(UTILS_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: benchmarks
benchmarks: $(BENCH_TARGETS)
	@echo "Benchmarks built. Run them manually."

###############################################################################
# Documentation Generation
###############################################################################

.PHONY: docs
docs:
	@scripts/generate_docs.sh
	@echo "Documentation generated in docs_generated/html/"

###############################################################################
# Cleaning up
###############################################################################

.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)

###############################################################################
# Create directories
###############################################################################

$(BIN_DIR) $(OBJ_DIR) $(LIB_DIR):
	@mkdir -p $@

###############################################################################
# Compilation and Dependency Rules
###############################################################################

COMPILE.c = $(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(COMPILE.c) -o $@ $<

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(COMPILE.c) -o $@ $<

$(OBJ_DIR)/%.o: $(BENCH_DIR)/%.c | $(OBJ_DIR)
	$(COMPILE.c) -o $@ $<

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(COMPILE.c) -o $@ $<

###############################################################################
# Include automatically generated dependencies
###############################################################################

-include $(OBJ_DIR)/*.d
