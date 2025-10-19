#include "../include/astarsolver.hpp"
#include <queue>
#include <climits>
#include <QTimer>

struct Node {
    Cell* cell;
    int gCost; // Cost from start
    int hCost; // Heuristic to goal
    int fCost() const { return gCost + hCost; }
    bool operator<(const Node& other) const { return fCost() > other.fCost(); } // Min-heap
};

AStarSolver::AStarSolver(Maze& m) : Solver(m) {}

int AStarSolver::manhattanDistance(Cell* c1, Cell* c2) {
    auto [x1, y1] = c1->getPosition();
    auto [x2, y2] = c2->getPosition();
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

std::vector<Cell*> AStarSolver::solve(Cell& start, Cell& end, QWidget* widget) {
    std::priority_queue<Node> pq;
    std::vector<Cell*> path;
    std::vector<std::vector<Cell*>> parent(maze.getCell(0, 0).getPosition().first + 1,
                                          std::vector<Cell*>(maze.getCell(0, 0).getPosition().second + 1, nullptr));
    std::vector<std::vector<int>> gCosts(maze.getCell(0, 0).getPosition().first + 1,
                                         std::vector<int>(maze.getCell(0, 0).getPosition().second + 1, INT_MAX));

    pq.push({&start, 0, manhattanDistance(&start, &end)});
    start.setVisited(true);
    gCosts[start.getPosition().first][start.getPosition().second] = 0;

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();
        Cell* cell = current.cell;

        widget->update(); // Trigger GUI repaint
        QTimer::singleShot(50, [=](){}); // Slow animation

        if (cell == &end) {
            Cell* curr = &end;
            while (curr != &start) {
                path.push_back(curr);
                auto pos = curr->getPosition();
                curr = parent[pos.first][pos.second];
            }
            path.push_back(&start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (Cell* neighbor : cell->getNeighbors()) {
            if (!neighbor->isVisited()) {
                int newGCost = gCosts[cell->getPosition().first][cell->getPosition().second] + 1;
                auto pos = neighbor->getPosition();
                if (newGCost < gCosts[pos.first][pos.second]) {
                    gCosts[pos.first][pos.second] = newGCost;
                    pq.push({neighbor, newGCost, manhattanDistance(neighbor, &end)});
                    parent[pos.first][pos.second] = cell;
                    neighbor->setVisited(true);
                }
            }
        }
    }
    return {};
}