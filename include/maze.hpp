#ifndef MAZE_HPP
#define MAZE_HPP

#include "cell.hpp"
#include <vector>

class Maze {
private:
    int width, height;
    std::vector<std::vector<Cell>> grid; // Encapsulated, composition

public:
    Maze(int w, int h);
    void generateMaze(); // Recursive backtracking
    Cell& getCell(int x, int y);
    std::vector<Cell*> getUnvisitedNeighbors(Cell* cell);
    void removeWallBetween(Cell* c1, Cell* c2);
};

#endif