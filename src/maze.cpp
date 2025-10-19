#include "../include/maze.hpp"
#include <stack>
#include <random>

Maze::Maze(int w, int h) : width(w), height(h) {
    grid.resize(w, std::vector<Cell>(h, Cell(0, 0)));
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            grid[i][j] = Cell(i, j);
        }
    }
    generateMaze();
}

void Maze::generateMaze() {
    std::stack<Cell*> stk;
    std::random_device rd;
    std::mt19937 gen(rd());
    Cell& start = grid[0][0];
    start.setVisited(true);
    stk.push(&start);

    while (!stk.empty()) {
        Cell* current = stk.top();
        auto neighbors = getUnvisitedNeighbors(current);
        if (neighbors.empty()) {
            stk.pop();
        } else {
            std::uniform_int_distribution<> dis(0, neighbors.size() - 1);
            Cell* next = neighbors[dis(gen)];
            removeWallBetween(current, next);
            next->setVisited(true);
            stk.push(next);
        }
    }
}

std::vector<Cell*> Maze::getUnvisitedNeighbors(Cell* cell) {
    std::vector<Cell*> unvisited;
    auto [x, y] = cell->getPosition();
    if (x > 0 && !grid[x-1][y].isVisited()) unvisited.push_back(&grid[x-1][y]);
    if (x < width-1 && !grid[x+1][y].isVisited()) unvisited.push_back(&grid[x+1][y]);
    if (y > 0 && !grid[x][y-1].isVisited()) unvisited.push_back(&grid[x][y-1]);
    if (y < height-1 && !grid[x][y+1].isVisited()) unvisited.push_back(&grid[x][y+1]);
    return unvisited;
}

void Maze::removeWallBetween(Cell* c1, Cell* c2) {
    auto [x1, y1] = c1->getPosition();
    auto [x2, y2] = c2->getPosition();
    if (x1 == x2 && y1 < y2) {
        c1->removeWall(2); c2->removeWall(3);
        c1->addNeighbor(c2); c2->addNeighbor(c1);
    } else if (x1 == x2 && y1 > y2) {
        c1->removeWall(3); c2->removeWall(2);
        c1->addNeighbor(c2); c2->addNeighbor(c1);
    } else if (y1 == y2 && x1 < x2) {
        c1->removeWall(1); c2->removeWall(0);
        c1->addNeighbor(c2); c2->addNeighbor(c1);
    } else if (y1 == y2 && x1 > x2) {
        c1->removeWall(0); c2->removeWall(1);
        c1->addNeighbor(c2); c2->addNeighbor(c1);
    }
}

Cell& Maze::getCell(int x, int y) { return grid[x][y]; }