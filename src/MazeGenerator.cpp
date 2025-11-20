#include "MazeGenerator.h"

#include <algorithm>

MazeGenerator::MazeGenerator(int width, int height, int seed)
    : width_(width), height_(height), seed_(seed), visited_(), randomSeed_(seed) {}

void MazeGenerator::initializeVisited(const Maze& maze) {
    visited_.assign(static_cast<std::size_t>(maze.getWidth() * maze.getHeight()), false);
}

void MazeGenerator::resetVisited() {
    std::fill(visited_.begin(), visited_.end(), false);
}

int MazeGenerator::pointToIndex(const Maze& maze, const Point& p) const {
    return p.getY() * maze.getWidth() + p.getX();
}

bool MazeGenerator::isValidForGeneration(const Maze& maze, const Point& p) const {
    return p.getX() > 0 && p.getX() < maze.getWidth() &&
           p.getY() > 0 && p.getY() < maze.getHeight();
}


int MazeGenerator::nextRandom() {
    randomSeed_ = (randomSeed_ * 1103515245 + 12345) & 0x7fffffff;
    return randomSeed_;
}

int MazeGenerator::randomRange(int min, int max) {
    if (min >= max) return min;
    return min + (nextRandom() % (max - min + 1));
}

void MazeGenerator::shuffleDirections(Point* directions, int count) {
    for (int i = count - 1; i > 0; i--) {
        int j = randomRange(0, i);
        std::swap(directions[i], directions[j]);
    }
}

void MazeGenerator::recursiveBacktrack(Maze& maze, const Point& current) {
    int currentIndex = pointToIndex(maze, current);
    if (currentIndex < 0 || currentIndex >= static_cast<int>(visited_.size())) {
        return;
    }

    visited_[static_cast<std::size_t>(currentIndex)] = true;

    Point directions[4] = {
        Point(0, -2), Point(2, 0), Point(0, 2), Point(-2, 0)
    };

    shuffleDirections(directions, 4);

    for (const auto& dir : directions) {
        Point next = current + dir;

        int nextIndex = pointToIndex(maze, next);
        if (isValidForGeneration(maze, next) &&
            nextIndex >= 0 && nextIndex < static_cast<int>(visited_.size()) &&
            !visited_[static_cast<std::size_t>(nextIndex)]) {
            Point wall = current + Point(dir.getX() / 2, dir.getY() / 2);
            maze.setCellAt(wall, '.');
            maze.setCellAt(next, '.');

            recursiveBacktrack(maze, next);
        }
    }
}

Maze MazeGenerator::generatePerfect() {
    int w = (width_ % 2 == 0) ? width_ + 1 : width_;
    int h = (height_ % 2 == 0) ? height_ + 1 : height_;

    Maze maze(w, h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            maze.setCellAt(x, y, '#');
        }
    }

    initializeVisited(maze);
    resetVisited();

    Point start(1, 1);
    maze.setCellAt(start, '.');
    recursiveBacktrack(maze, start);

    maze.setStart(Point(1, 1));
    maze.setCellAt(1, 1, 'S');

    Point goal(w - 2, h - 2);
    maze.setGoal(goal);
    maze.setCellAt(goal, 'G');

    visited_.clear();
    return maze;
}

Maze MazeGenerator::generateWithLoops(int extraPaths) {
    Maze maze = generatePerfect();

    int mazeWidth = maze.getWidth();
    int mazeHeight = maze.getHeight();

    if (mazeWidth <= 2 || mazeHeight <= 2) {
        return maze;
    }

    int added = 0;
    int attempts = 0;
    int maxAttempts = std::max(extraPaths * 10, 1);

    while (added < extraPaths && attempts < maxAttempts) {
        int x = randomRange(1, mazeWidth - 2);
        int y = randomRange(1, mazeHeight - 2);
        Point p(x, y);

        if (maze.getCellAt(p) == '#') {
            int pathCount = 0;
            const Point neighbors[4] = {
                Point(x, y - 1), Point(x + 1, y), Point(x, y + 1), Point(x - 1, y)
            };

            for (const auto& neighbor : neighbors) {
                if (maze.getCellAt(neighbor) != '#') {
                    pathCount++;
                }
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
    int mazeWidth = maze.getWidth();
    int mazeHeight = maze.getHeight();

    if (mazeWidth == 0 || mazeHeight == 0) {
        return;
    }
    int totalCells = mazeWidth * mazeHeight;
    int waterCells = (totalCells * waterPercent) / 100;
    int mountainCells = (totalCells * mountainPercent) / 100;

    int added = 0;
    while (added < waterCells) {
        int x = randomRange(0, mazeWidth - 1);
        int y = randomRange(0, mazeHeight - 1);
        Point p(x, y);

        char cell = maze.getCellAt(p);
        if (cell == '.' ) {
            maze.setCellAt(p, '~');
            added++;
        }
    }

    added = 0;
    while (added < mountainCells) {
        int x = randomRange(0, mazeWidth - 1);
        int y = randomRange(0, mazeHeight - 1);
        Point p(x, y);

        char cell = maze.getCellAt(p);
        if (cell == '.' ) {
            maze.setCellAt(p, '^');
            added++;
        }
    }
}

Maze MazeGenerator::generateWithTerrain(int waterPercent, int mountainPercent) {
    int loops = std::max(1, std::max(width_, height_) / 2);
    Maze maze = generateWithLoops(loops);
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
