#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include "cell.hpp"
#include <vector>

class Analyzer {
private:
    std::vector<Cell*> path; // Composition

public:
    Analyzer(const std::vector<Cell*>& p);
    int getPathLength() const;
    int getTurns() const;
    void analyze() const;
};

#endif