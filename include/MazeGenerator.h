#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H

#include "Maze.h"
#include "Point.h"
#include <vector>


class MazeGenerator {
private:
    int width_;
    int height_;
    int seed_;
    std::vector<bool> visited_;

    
    void initializeVisited(const Maze& maze);
    void resetVisited();
    bool isValidForGeneration(const Maze& maze, const Point& p) const;
    void shuffleDirections(Point* directions, int count);
    void recursiveBacktrack(Maze& maze, const Point& current);
    void addTerrainFeatures(Maze& maze, int waterPercent, int mountainPercent);
    int pointToIndex(const Maze& maze, const Point& p) const;

    
    int randomSeed_;
    int nextRandom();
    int randomRange(int min, int max);

public:
    MazeGenerator(int width, int height, int seed = 12345);

    
    Maze generatePerfect();  
    Maze generateWithLoops(int extraPaths);  
    Maze generateWithTerrain(int waterPercent, int mountainPercent);

    
    Maze generateEasy();    
    Maze generateMedium();  
    Maze generateHard();    

    
    void setSeed(int seed);
    void setDimensions(int width, int height);
};

#endif
