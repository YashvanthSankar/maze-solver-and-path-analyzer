#ifndef BFSSOLVER_H
#define BFSSOLVER_H

#include "Maze.h"
#include "Path.h"

// Simple Queue implementation (no templates, demonstrating encapsulation)
class PointQueue {
private:
    Point* data_;
    int front_;
    int rear_;
    int capacity_;
    int size_;
    
    void resize();

public:
    PointQueue();
    ~PointQueue();
    
    void enqueue(const Point& p);
    Point dequeue();
    bool isEmpty() const;
    int getSize() const;
};

// BFSSolver class demonstrating Encapsulation and Abstraction
class BFSSolver {
private:
    // Encapsulated internal state
    Point* parent_;     // Parent map for path reconstruction
    bool* visited_;     // Visited flags
    int maxNodes_;      // Maximum nodes (width * height)
    
    // Private helper methods (abstraction)
    int pointToIndex(const Point& p, int width) const;
    Point indexToPoint(int index, int width) const;
    void initializeSearchState(int width, int height);
    void cleanupSearchState();
    Path reconstructPath(const Point& start, const Point& goal, int width) const;

public:
    BFSSolver();
    ~BFSSolver();
    
    // Main solving method
    Path solve(const Maze& maze);
    
    // Get statistics
    int getNodesExplored() const;
};

#endif
