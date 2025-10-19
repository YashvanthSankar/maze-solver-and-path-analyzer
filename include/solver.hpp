#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "maze.hpp"
#include <QWidget>
#include <vector>

class Solver {
protected:
    Maze& maze;

public:
    Solver(Maze& m);
    virtual std::vector<Cell*> solve(Cell& start, Cell& end, QWidget* widget) = 0; // Abstraction, polymorphism
    virtual ~Solver() {}
};

#endif