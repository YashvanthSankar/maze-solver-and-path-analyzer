#ifndef BFSSOLVER_H
#define BFSSOLVER_H

#include "Maze.h"
#include "MazeSolverStrategy.h"
#include "Path.h"
#include <deque>
#include <vector>


class PointQueue {
private:
    std::deque<Point> data_;

public:
    void enqueue(const Point& p);
    Point dequeue();
    bool isEmpty() const;
    int getSize() const;
};


class BFSSolver : public MazeSolverStrategy {
private:
    std::vector<Point> parent_;   
    std::vector<bool> visited_;   

    int pointToIndex(const Point& p, int width) const;
    Path reconstructPath(const Point& start, const Point& goal, int width) const;

public:
    BFSSolver();

    Path solve(const Maze& maze) override;
    int getNodesExplored() const override;
    std::string name() const override;
};

#endif
