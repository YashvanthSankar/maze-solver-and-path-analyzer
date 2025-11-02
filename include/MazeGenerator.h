#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H

#include "Maze.h"
#include "Point.h"

// MazeGenerator class demonstrating Encapsulation and Abstraction
class MazeGenerator {
private:
    int width_;
    int height_;
    int seed_;
    bool* visited_;
    
    // Private helper methods (Abstraction)
    void initializeVisited();
    void cleanupVisited();
    bool isValidForGeneration(const Point& p) const;
    void shuffleDirections(Point* directions, int count);
    void recursiveBacktrack(Maze& maze, const Point& current);
    void addTerrainFeatures(Maze& maze, int waterPercent, int mountainPercent);
    int pointToIndex(const Point& p) const;
    
    // Simple random number generator (encapsulated)
    int randomSeed_;
    int nextRandom();
    int randomRange(int min, int max);

public:
    MazeGenerator(int width, int height, int seed = 12345);
    ~MazeGenerator();
    
    // Generate different maze types
    Maze generatePerfect();  // Single solution, no loops
    Maze generateWithLoops(int extraPaths);  // Multiple solutions
    Maze generateWithTerrain(int waterPercent, int mountainPercent);
    
    // Quick presets
    Maze generateEasy();    // Small, simple
    Maze generateMedium();  // Medium complexity
    Maze generateHard();    // Large, complex
    
    // Setters
    void setSeed(int seed);
    void setDimensions(int width, int height);
};

#endif
