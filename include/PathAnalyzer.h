#ifndef PATHANALYZER_H
#define PATHANALYZER_H

#include "Path.h"
#include "Maze.h"
#include <iostream>


class PathMetrics {
private:
    int totalLength_;
    int numberOfTurns_;
    int directionChanges_;
    double straightness_;
    double avgStepCost_;
    int narrowPassages_;
    
public:
    PathMetrics();
    
    
    void setTotalLength(int length);
    void setNumberOfTurns(int turns);
    void setDirectionChanges(int changes);
    void setStraightness(double straightness);
    void setAvgStepCost(double cost);
    void setNarrowPassages(int passages);
    
    
    int getTotalLength() const;
    int getNumberOfTurns() const;
    int getDirectionChanges() const;
    double getStraightness() const;
    double getAvgStepCost() const;
    int getNarrowPassages() const;
    
    
    void display() const;
    
    
    bool operator>(const PathMetrics& other) const;
    bool operator<(const PathMetrics& other) const;
    friend std::ostream& operator<<(std::ostream& os, const PathMetrics& m);
};


class PathAnalyzer {
private:
    PathMetrics metrics_;
    
    
    int calculateTurns(const Path& path) const;
    int calculateDirectionChanges(const Path& path) const;
    double calculateStraightness(const Path& path) const;
    int countNarrowPassages(const Path& path, const Maze& maze) const;
    int getDirection(const Point& from, const Point& to) const;

public:
    PathAnalyzer();
    
    
    PathMetrics analyze(const Path& path, const Maze& maze);
    
    
    PathMetrics getMetrics() const;
    
    
    void comparePaths(const Path& path1, const Path& path2, const Maze& maze);
};

#endif
