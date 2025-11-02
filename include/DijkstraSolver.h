#ifndef DIJKSTRASOLVER_H
#define DIJKSTRASOLVER_H

#include "Maze.h"
#include "Path.h"

// Node for priority queue (encapsulation)
class PQNode {
private:
    Point point_;
    double priority_;

public:
    PQNode();
    PQNode(const Point& p, double priority);
    
    Point getPoint() const;
    double getPriority() const;
    
    // Operator overloading for comparison
    bool operator<(const PQNode& other) const;
    bool operator>(const PQNode& other) const;
};

// Simple Priority Queue (min-heap, no templates)
class PriorityQueue {
private:
    PQNode* heap_;
    int size_;
    int capacity_;
    
    void resize();
    void heapifyUp(int index);
    void heapifyDown(int index);
    void swap(int i, int j);
    int parent(int i) const;
    int leftChild(int i) const;
    int rightChild(int i) const;

public:
    PriorityQueue();
    ~PriorityQueue();
    
    void push(const PQNode& node);
    PQNode pop();
    bool isEmpty() const;
    int getSize() const;
};

// DijkstraSolver class with weighted pathfinding
class DijkstraSolver {
private:
    Point* parent_;
    double* distance_;
    bool* visited_;
    int maxNodes_;
    
    int pointToIndex(const Point& p, int width) const;
    Point indexToPoint(int index, int width) const;
    void initializeSearchState(int width, int height);
    void cleanupSearchState();
    Path reconstructPath(const Point& start, const Point& goal, int width) const;
    double getCellCost(const Maze& maze, const Point& p) const;

public:
    DijkstraSolver();
    ~DijkstraSolver();
    
    Path solve(const Maze& maze);
    int getNodesExplored() const;
};

#endif
