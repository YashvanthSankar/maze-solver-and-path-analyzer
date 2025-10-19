#include "../include/analyzer.hpp"
#include <iostream>

Analyzer::Analyzer(const std::vector<Cell*>& p) : path(p) {}

int Analyzer::getPathLength() const { return path.size(); }

int Analyzer::getTurns() const {
    int turns = 0;
    for (size_t i = 1; i < path.size() - 1; ++i) {
        auto [x1, y1] = path[i-1]->getPosition();
        auto [x2, y2] = path[i]->getPosition();
        auto [x3, y3] = path[i+1]->getPosition();
        if ((x2 - x1) != (x3 - x2) || (y2 - y1) != (y3 - y2)) turns++;
    }
    return turns;
}

void Analyzer::analyze() const {
    std::cout << "Path Length: " << getPathLength() << "\nTurns: " << getTurns() << std::endl;
}