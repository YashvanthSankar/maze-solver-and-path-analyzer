#ifndef MAZE_SOLVER_STRATEGY_H
#define MAZE_SOLVER_STRATEGY_H

#include "Maze.h"
#include "Path.h"
#include <string>


class MazeSolverStrategy {
public:
    virtual ~MazeSolverStrategy() = default;

    virtual Path solve(const Maze& maze) = 0;
    virtual int getNodesExplored() const = 0;
    virtual std::string name() const = 0;
};

#endif
