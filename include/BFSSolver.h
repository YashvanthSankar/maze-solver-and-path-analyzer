#ifndef BFSSOLVER_H
#define BFSSOLVER_H

#include "Maze.h"
#include "Path.h"
#include <deque>
#include <vector>

// Simple Queue implementation (no templates, demonstrating encapsulation)
class PointQueue {
private:
    std::deque<Point> data_;

public:
    void enqueue(const Point& p);
    Point dequeue();
    bool isEmpty() const;
    int getSize() const;
};

// BFSSolver class demonstrating Encapsulation and Abstraction
class BFSSolver {
private:
    std::vector<Point> parent_;   // Parent map for path reconstruction
    std::vector<bool> visited_;   // Visited flags

    int pointToIndex(const Point& p, int width) const;
    Path reconstructPath(const Point& start, const Point& goal, int width) const;

public:
    BFSSolver();

    Path solve(const Maze& maze);
    int getNodesExplored() const;
};

#endif
