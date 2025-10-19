#ifndef CELL_HPP
#define CELL_HPP

#include <vector>

class Cell {
private:
    int x, y;
    bool visited; // Encapsulated
    bool walls[4]; // 0: North, 1: South, 2: East, 3: West
    std::vector<Cell*> neighbors;

public:
    Cell(int x, int y);
    bool isVisited() const;
    void setVisited(bool v);
    std::vector<Cell*>& getNeighbors();
    void addNeighbor(Cell* neighbor);
    bool hasWall(int direction) const;
    void removeWall(int direction);
    std::pair<int, int> getPosition() const;
};

#endif