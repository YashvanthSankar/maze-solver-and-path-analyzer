#include "PathAnalyzer.h"
#include "StatsAggregator.h"

#include <algorithm>

PathMetrics::PathMetrics() : totalLength_(0), numberOfTurns_(0), directionChanges_(0),
                             straightness_(0.0), avgStepCost_(0.0), narrowPassages_(0), 
                             totalCostWithPenalty_(0.0) {}

void PathMetrics::setTotalLength(int length) {
    totalLength_ = length;
}

void PathMetrics::setNumberOfTurns(int turns) {
    numberOfTurns_ = turns;
}

void PathMetrics::setDirectionChanges(int changes) {
    directionChanges_ = changes;
}

void PathMetrics::setStraightness(double straightness) {
    straightness_ = straightness;
}

void PathMetrics::setAvgStepCost(double cost) {
    avgStepCost_ = cost;
}

void PathMetrics::setNarrowPassages(int passages) {
    narrowPassages_ = passages;
}

void PathMetrics::setTotalCostWithPenalty(double cost) {
    totalCostWithPenalty_ = cost;
}

int PathMetrics::getTotalLength() const {
    return totalLength_;
}

int PathMetrics::getNumberOfTurns() const {
    return numberOfTurns_;
}

int PathMetrics::getDirectionChanges() const {
    return directionChanges_;
}

double PathMetrics::getStraightness() const {
    return straightness_;
}

double PathMetrics::getAvgStepCost() const {
    return avgStepCost_;
}

int PathMetrics::getNarrowPassages() const {
    return narrowPassages_;
}

double PathMetrics::getTotalCostWithPenalty() const {
    return totalCostWithPenalty_;
}

void PathMetrics::display() const {
    std::cout << "\n=== Path Analysis Results ===\n";
    std::cout << "Total Length: " << totalLength_ << " steps\n";
    std::cout << "Number of Turns: " << numberOfTurns_ << "\n";
    std::cout << "Direction Changes: " << directionChanges_ << "\n";
    std::cout << "Straightness: " << straightness_ << " (0.0-1.0)\n";
    std::cout << "Average Step Cost: " << avgStepCost_ << "\n";
    std::cout << "Total Cost (with turn penalty): " << totalCostWithPenalty_ << "\n";
    std::cout << "Narrow Passages: " << narrowPassages_ << "\n";
    std::cout << "=============================\n";
}

bool PathMetrics::operator>(const PathMetrics& other) const {
    return totalLength_ > other.totalLength_;
}

bool PathMetrics::operator<(const PathMetrics& other) const {
    return totalLength_ < other.totalLength_;
}

std::ostream& operator<<(std::ostream& os, const PathMetrics& m) {
    os << "Length:" << m.totalLength_ 
       << " Turns:" << m.numberOfTurns_ 
       << " Cost:" << m.avgStepCost_;
    return os;
}

PathAnalyzer::PathAnalyzer() {}

int PathAnalyzer::getDirection(const Point& from, const Point& to) const {
    Point diff = to - from;
    int dx = diff.getX();
    int dy = diff.getY();
    
    if (dy < 0) return 0; 
    if (dx > 0) return 1;
    if (dy > 0) return 2; 
    if (dx < 0) return 3; 
    return -1;
}

int PathAnalyzer::calculateTurns(const Path& path) const {
    if (path.getSize() < 3) return 0;
    
    int turns = 0;
    int prevDir = getDirection(path[0], path[1]);
    
    for (int i = 1; i < path.getSize() - 1; i++) {
        int currDir = getDirection(path[i], path[i + 1]);
        
        if (currDir != prevDir && currDir != -1 && prevDir != -1) {
            turns++;
        }
        
        prevDir = currDir;
    }
    
    return turns;
}

int PathAnalyzer::calculateDirectionChanges(const Path& path) const {
    if (path.getSize() < 2) return 0;
    
    int changes = 0;
    int prevDir = getDirection(path[0], path[1]);
    
    for (int i = 1; i < path.getSize() - 1; i++) {
        int currDir = getDirection(path[i], path[i + 1]);
        
        if (currDir != prevDir && currDir != -1 && prevDir != -1) {
            changes++;
        }
        
        prevDir = currDir;
    }
    
    return changes;
}

double PathAnalyzer::calculateStraightness(const Path& path) const {
    if (path.getSize() < 2) return 1.0;
    
    Point start = path[0];
    Point end = path[path.getSize() - 1];
    
    int manhattanDist = start.manhattanDistance(end);
    int actualLength = path.getSize() - 1;
    
    if (actualLength == 0) return 1.0;
    
    return (double)manhattanDist / actualLength;
}

int PathAnalyzer::countNarrowPassages(const Path& path, const Maze& maze) const {
    int narrowCount = 0;
    
    for (int i = 0; i < path.getSize(); i++) {
        Point p = path[i];
        
        Point neighbors[4];
        int neighborCount;
        maze.getNeighbors(p, neighbors, neighborCount);
        
        if (neighborCount <= 2) {
            narrowCount++;
        }
    }
    
    return narrowCount;
}

PathMetrics PathAnalyzer::analyze(const Path& path, const Maze& maze) {
    int effectiveLength = std::max(0, path.getSize() - 1);
    metrics_.setTotalLength(effectiveLength);
    int numberOfTurns = calculateTurns(path);
    metrics_.setNumberOfTurns(numberOfTurns);
    metrics_.setDirectionChanges(calculateDirectionChanges(path));
    metrics_.setStraightness(calculateStraightness(path));
    
    StatsAggregator<double> stepCosts;
    double totalCostWithPenalty = 0.0;
    int prevDir = -1;
    
    for (int i = 1; i < path.getSize(); ++i) {
        const Point& current = path[i];
        char cell = maze.getCellAt(current);
        double cost = 1.0;
        if (cell == '~') {
            cost = 2.0;
        } else if (cell == '^') {
            cost = 3.0;
        }
        
        stepCosts.addSample(cost);
        
        int currDir = getDirection(path[i-1], path[i]);
        if (prevDir != -1 && currDir != -1 && currDir != prevDir) {
            totalCostWithPenalty += 0.5;
        }
        prevDir = currDir;
        
        totalCostWithPenalty += cost;
    }

    double averageCost = 0.0;
    try {
        averageCost = stepCosts.average();
    } catch (const AnalysisException&) {
        averageCost = 0.0;
    }

    metrics_.setAvgStepCost(averageCost);
    metrics_.setTotalCostWithPenalty(totalCostWithPenalty);
    
    metrics_.setNarrowPassages(countNarrowPassages(path, maze));
    
    return metrics_;
}

PathMetrics PathAnalyzer::getMetrics() const {
    return metrics_;
}

void PathAnalyzer::comparePaths(const Path& path1, const Path& path2, const Maze& maze) {
    PathAnalyzer analyzer1, analyzer2;
    PathMetrics m1 = analyzer1.analyze(path1, maze);
    PathMetrics m2 = analyzer2.analyze(path2, maze);
    
    std::cout << "\n=== Path Comparison ===\n";
    std::cout << "Path 1: " << m1 << "\n";
    std::cout << "Path 2: " << m2 << "\n";
    
    if (m1 < m2) {
        std::cout << "Result: Path 1 is shorter\n";
    } else if (m2 < m1) {
        std::cout << "Result: Path 2 is shorter\n";
    } else {
        std::cout << "Result: Both paths have equal length\n";
    }
    std::cout << "=======================\n";
}
