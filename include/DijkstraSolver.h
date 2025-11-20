#ifndef DIJKSTRASOLVER_H
#define DIJKSTRASOLVER_H

#include "Maze.h"
#include "MazeSolverStrategy.h"
#include "Path.h"
#include <vector>


class PQNode {
private:
    Point point_;
    double priority_;

public:
    PQNode();
    PQNode(const Point& p, double priority);
    
    Point getPoint() const;
    double getPriority() const;
    
    
    bool operator<(const PQNode& other) const;
    bool operator>(const PQNode& other) const;
};


class PriorityQueue {
private:
    std::vector<PQNode> heap_;

    void heapifyUp(int index);
    void heapifyDown(int index);
    static int parent(int i);
    static int leftChild(int i);
    static int rightChild(int i);

public:
    void push(const PQNode& node);
    PQNode pop();
    bool isEmpty() const;
    int getSize() const;
};


class DijkstraSolver : public MazeSolverStrategy {
private:
    std::vector<Point> parent_;
    std::vector<double> distance_;
    std::vector<bool> visited_;

    int pointToIndex(const Point& p, int width) const;
    Path reconstructPath(const Point& start, const Point& goal, int width) const;
    double getCellCost(const Maze& maze, const Point& p) const;

public:
    DijkstraSolver();

    Path solve(const Maze& maze) override;
    int getNodesExplored() const override;
    std::string name() const override;
};

#endif
