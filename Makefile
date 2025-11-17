# ==============================================================================
# Maze Solver & Path Analyzer - Professional Makefile
# ==============================================================================
# Demonstrates: Encapsulation, Abstraction, Operator Overloading
# Features: Instant maze generation, BFS & Dijkstra, Animated visualization
# ==============================================================================

# Compiler Configuration
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lncurses
TARGET = maze_solver

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = build
EXAMPLE_DIR = examples

# Source Files (Enhanced Version with Maze Generator and CLI Utils)
CORE_SOURCES = $(SRC_DIR)/Point.cpp \
			   $(SRC_DIR)/Path.cpp \
			   $(SRC_DIR)/Maze.cpp \
			   $(SRC_DIR)/BFSSolver.cpp \
			   $(SRC_DIR)/DijkstraSolver.cpp \
			   $(SRC_DIR)/PathAnalyzer.cpp \
			   $(SRC_DIR)/Renderer.cpp \
			   $(SRC_DIR)/MazeGenerator.cpp \
			   $(SRC_DIR)/CLIUtils.cpp \
			   $(SRC_DIR)/GameMode.cpp

SOURCES = $(CORE_SOURCES) \
		  $(SRC_DIR)/main.cpp

# Object Files (Auto-generated from sources, placed in build directory)
CORE_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CORE_SOURCES))
MAIN_OBJECT = $(OBJ_DIR)/main.o
OBJECTS = $(CORE_OBJECTS) $(MAIN_OBJECT)

# Header Files (For dependency tracking)
HEADERS = $(wildcard $(INC_DIR)/*.h)

# Tests
TEST_DIR = tests
TEST_SOURCES = $(TEST_DIR)/SmokeTest.cpp
TEST_OBJECTS = $(OBJ_DIR)/SmokeTest.o
TEST_BIN_DIR = $(OBJ_DIR)/tests
TEST_TARGET = $(TEST_BIN_DIR)/smoke_tests

# ==============================================================================
# Build Targets
# ==============================================================================

# Default target: Build the complete project
all: $(OBJ_DIR) $(TARGET)
	@echo ""
	@echo "✓ Build complete!"
	@echo "Run with: ./$(TARGET)"
	@echo ""

# Create build directory if it doesn't exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Link all object files to create the executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Compile individual source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test sources
$(OBJ_DIR)/SmokeTest.o: $(TEST_DIR)/SmokeTest.cpp $(HEADERS) | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure test binary directory exists
$(TEST_BIN_DIR): | $(OBJ_DIR)
	@mkdir -p $(TEST_BIN_DIR)

# Link smoke tests
$(TEST_TARGET): $(CORE_OBJECTS) $(TEST_OBJECTS) | $(TEST_BIN_DIR)
	@echo "Linking smoke tests..."
	@$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(CORE_OBJECTS) $(TEST_OBJECTS) $(LDFLAGS)

# ==============================================================================
# Utility Targets
# ==============================================================================

# Clean all build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(OBJ_DIR) $(TARGET)
	@echo "✓ Clean complete!"

# Build and run the program
run: $(TARGET)
	@echo "Running Maze Solver..."
	@echo ""
	@./$(TARGET)

# Build and run smoke tests
test: $(TEST_TARGET)
	@echo "Running smoke tests..."
	@$(TEST_TARGET)

# Clean and rebuild everything
rebuild: clean all

# Install dependencies (if needed)
install-deps:
	@echo "Checking for g++ with C++11 support..."
	@which g++ > /dev/null || (echo "Error: g++ not found. Please install g++." && exit 1)
	@echo "✓ All dependencies satisfied!"

# Check for memory leaks with valgrind (if available)
memcheck: $(TARGET)
	@which valgrind > /dev/null || (echo "Error: valgrind not installed." && exit 1)
	@echo "Running memory leak detection..."
	@valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Show project information
info:
	@echo ""
	@echo "╔════════════════════════════════════════════════╗"
	@echo "║   Maze Solver & Path Analyzer                 ║"
	@echo "╚════════════════════════════════════════════════╝"
	@echo ""
	@echo "Project Structure:"
	@echo "  Source Directory:   $(SRC_DIR)/"
	@echo "  Include Directory:  $(INC_DIR)/"
	@echo "  Build Directory:    $(OBJ_DIR)/"
	@echo "  Examples Directory: $(EXAMPLE_DIR)/"
	@echo ""
	@echo "Build Configuration:"
	@echo "  Compiler:           $(CXX)"
	@echo "  Flags:              $(CXXFLAGS)"
	@echo "  Target:             $(TARGET)"
	@echo ""
	@echo "Source Files:         10"
	@echo "Header Files:         9"
	@echo ""
	@echo "Features:"
	@echo "  • Instant maze generation (Recursive Backtracking)"
	@echo "  • BFS & Dijkstra pathfinding algorithms"
	@echo "  • Animated step-by-step visualization"
	@echo "  • Beautiful CLI with ANSI colors"
	@echo "  • Path analysis and comparison"
	@echo ""
	@echo "OOP Concepts Demonstrated:"
	@echo "  • Encapsulation (private data, public interfaces)"
	@echo "  • Abstraction (complex logic hidden)"
	@echo "  • Operator Overloading (natural syntax)"
	@echo ""

# Display help information
help:
	@echo ""
	@echo "╔════════════════════════════════════════════════╗"
	@echo "║   Maze Solver - Build System Help            ║"
	@echo "╚════════════════════════════════════════════════╝"
	@echo ""
	@echo "Available Commands:"
	@echo "  make              Build the project"
	@echo "  make run          Build and run the program"
	@echo "  make clean        Remove all build artifacts"
	@echo "  make rebuild      Clean and rebuild everything"
	@echo "  make info         Show project information"
	@echo "  make install-deps Check dependencies"
	@echo "  make memcheck     Run memory leak detection"
	@echo "  make help         Show this help message"
	@echo ""
	@echo "Quick Start:"
	@echo "  1. make           # Build the project"
	@echo "  2. ./$(TARGET)    # Run the program"
	@echo "  3. Select option 3 for Quick Solve demo!"
	@echo ""
	@echo "Project Structure:"
	@echo "  src/              C++ source files (.cpp)"
	@echo "  include/          Header files (.h)"
	@echo "  build/            Compiled object files (.o)"
	@echo "  examples/         Sample maze files (.txt)"
	@echo ""

# ==============================================================================
# Phony Targets (Not actual files)
# ==============================================================================

.PHONY: all clean run rebuild install-deps memcheck info help test
