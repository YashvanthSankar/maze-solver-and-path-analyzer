#ifndef ASTAR_SOLVER_HPP
#define ASTAR_SOLVER_HPP

#include "solver.hpp"

class AStarSolver : public Solver {
public:
    AStarSolver(Maze& m);
    std::vector<Cell*> solve(Cell& start, Cell& end, QWidget* widget) override;
private:
    int manhattanDistance(Cell* c1, Cell* c2);
};

#endif