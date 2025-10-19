#include "../include/cell.hpp"

Cell::Cell(int x, int y) : x(x), y(y), visited(false) {
    for (int i = 0; i < 4; ++i) walls[i] = true;
}

bool Cell::isVisited() const { return visited; }
void Cell::setVisited(bool v) { visited = v; }
std::vector<Cell*>& Cell::getNeighbors() { return neighbors; }
void Cell::addNeighbor(Cell* neighbor) { neighbors.push_back(neighbor); }
bool Cell::hasWall(int direction) const { return walls[direction]; }
void Cell::removeWall(int direction) { walls[direction] = false; }
std::pair<int, int> Cell::getPosition() const { return {x, y}; }