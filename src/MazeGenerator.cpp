#include "MazeGenerator.h"

MazeGenerator::MazeGenerator(int width, int height, int seed) 
    : width_(width), height_(height), seed_(seed), visited_(nullptr), randomSeed_(seed) {
}

MazeGenerator::~MazeGenerator() {
    cleanupVisited();
}

void MazeGenerator::initializeVisited() {
    cleanupVisited();
    visited_ = new bool[width_ * height_];
    for (int i = 0; i < width_ * height_; i++) {
        visited_[i] = false;
    }
}

void MazeGenerator::cleanupVisited() {
    if (visited_ != nullptr) {
        delete[] visited_;
        visited_ = nullptr;
    }
}

int MazeGenerator::pointToIndex(const Point& p) const {
    return p.getY() * width_ + p.getX();
}

// Linear Congruential Generator for randomness
int MazeGenerator::nextRandom() {
    randomSeed_ = (randomSeed_ * 1103515245 + 12345) & 0x7fffffff;
    return randomSeed_;
}

int MazeGenerator::randomRange(int min, int max) {
    if (min >= max) return min;
    return min + (nextRandom() % (max - min + 1));
}

bool MazeGenerator::isValidForGeneration(const Point& p) const {
    return p.getX() >= 0 && p.getX() < width_ && 
           p.getY() >= 0 && p.getY() < height_;
}

void MazeGenerator::shuffleDirections(Point* directions, int count) {
    for (int i = count - 1; i > 0; i--) {
        int j = randomRange(0, i);
        Point temp = directions[i];
        directions[i] = directions[j];
        directions[j] = temp;
    }
}

void MazeGenerator::recursiveBacktrack(Maze& maze, const Point& current) {
    visited_[pointToIndex(current)] = true;
    
    // Four directions: up, right, down, left
    Point directions[4] = {
        Point(0, -2), Point(2, 0), Point(0, 2), Point(-2, 0)
    };
    
    shuffleDirections(directions, 4);
    
    for (int i = 0; i < 4; i++) {
        Point next = current + directions[i];
        
        if (isValidForGeneration(next) && !visited_[pointToIndex(next)]) {
            // Remove wall between current and next
            Point wall = current + Point(directions[i].getX() / 2, directions[i].getY() / 2);
            maze.setCellAt(wall, '.');
            maze.setCellAt(next, '.');
            
            recursiveBacktrack(maze, next);
        }
    }
}

Maze MazeGenerator::generatePerfect() {
    // Ensure odd dimensions for perfect maze generation
    int w = (width_ % 2 == 0) ? width_ + 1 : width_;
    int h = (height_ % 2 == 0) ? height_ + 1 : height_;
    
    Maze maze(w, h);
    
    // Fill with walls
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            maze.setCellAt(x, y, '#');
        }
    }
    
    initializeVisited();
    
    // Start from (1,1)
    Point start(1, 1);
    maze.setCellAt(start, '.');
    recursiveBacktrack(maze, start);
    
    // Set start and goal
    maze.setStart(Point(1, 1));
    maze.setCellAt(1, 1, 'S');
    
    Point goal(w - 2, h - 2);
    maze.setGoal(goal);
    maze.setCellAt(goal, 'G');
    
    cleanupVisited();
    
    return maze;
}

Maze MazeGenerator::generateWithLoops(int extraPaths) {
    Maze maze = generatePerfect();
    
    // Add random openings to create loops
    int added = 0;
    int attempts = 0;
    int maxAttempts = extraPaths * 10;
    
    while (added < extraPaths && attempts < maxAttempts) {
        int x = randomRange(1, width_ - 2);
        int y = randomRange(1, height_ - 2);
        Point p(x, y);
        
        if (maze.getCellAt(p) == '#') {
            // Check if surrounded by paths
            int pathCount = 0;
            Point neighbors[4] = {
                Point(x, y-1), Point(x+1, y), Point(x, y+1), Point(x-1, y)
            };
            
            for (int i = 0; i < 4; i++) {
                if (maze.getCellAt(neighbors[i]) != '#') pathCount++;
            }
            
            if (pathCount >= 2) {
                maze.setCellAt(p, '.');
                added++;
            }
        }
        attempts++;
    }
    
    return maze;
}

void MazeGenerator::addTerrainFeatures(Maze& maze, int waterPercent, int mountainPercent) {
    int totalCells = width_ * height_;
    int waterCells = (totalCells * waterPercent) / 100;
    int mountainCells = (totalCells * mountainPercent) / 100;
    
    // Add water
    int added = 0;
    while (added < waterCells) {
        int x = randomRange(0, width_ - 1);
        int y = randomRange(0, height_ - 1);
        Point p(x, y);
        
        char cell = maze.getCellAt(p);
        if (cell == '.') {
            maze.setCellAt(p, '~');
            added++;
        }
    }
    
    // Add mountains
    added = 0;
    while (added < mountainCells) {
        int x = randomRange(0, width_ - 1);
        int y = randomRange(0, height_ - 1);
        Point p(x, y);
        
        char cell = maze.getCellAt(p);
        if (cell == '.') {
            maze.setCellAt(p, '^');
            added++;
        }
    }
}

Maze MazeGenerator::generateWithTerrain(int waterPercent, int mountainPercent) {
    Maze maze = generateWithLoops(width_ / 2);
    addTerrainFeatures(maze, waterPercent, mountainPercent);
    return maze;
}

Maze MazeGenerator::generateEasy() {
    int oldW = width_, oldH = height_;
    width_ = 15;
    height_ = 15;
    Maze maze = generatePerfect();
    width_ = oldW;
    height_ = oldH;
    return maze;
}

Maze MazeGenerator::generateMedium() {
    int oldW = width_, oldH = height_;
    width_ = 25;
    height_ = 25;
    Maze maze = generateWithLoops(10);
    addTerrainFeatures(maze, 5, 3);
    width_ = oldW;
    height_ = oldH;
    return maze;
}

Maze MazeGenerator::generateHard() {
    int oldW = width_, oldH = height_;
    width_ = 35;
    height_ = 35;
    Maze maze = generateWithLoops(20);
    addTerrainFeatures(maze, 10, 5);
    width_ = oldW;
    height_ = oldH;
    return maze;
}

void MazeGenerator::setSeed(int seed) {
    seed_ = seed;
    randomSeed_ = seed;
}

void MazeGenerator::setDimensions(int width, int height) {
    width_ = width;
    height_ = height;
}
