#ifndef MAZE_H
#define MAZE_H

#include "Point.h"
#include <iostream>
#include <fstream>

// Maze class demonstrating strong Encapsulation
class Maze {
private:
    char** grid_;        // Encapsulated 2D grid
    int width_;          // Encapsulated dimensions
    int height_;
    Point start_;        // Encapsulated start position
    Point goal_;         // Encapsulated goal position
    
    // Private helper methods (abstraction)
    void allocateGrid();
    void deallocateGrid();
    void copyGrid(char** source);

public:
    // Constructor and Destructor
    Maze();
    Maze(int width, int height);
    Maze(const Maze& other);  // Copy constructor
    ~Maze();
    
    // Assignment operator
    Maze& operator=(const Maze& other);
    
    // Getters (const methods preserve encapsulation)
    int getWidth() const;
    int getHeight() const;
    Point getStart() const;
    Point getGoal() const;
    char getCellAt(const Point& p) const;
    char getCellAt(int x, int y) const;
    
    // Setters (controlled modification)
    void setStart(const Point& p);
    void setGoal(const Point& p);
    void setCellAt(const Point& p, char value);
    void setCellAt(int x, int y, char value);
    
    // File operations
    bool loadFromFile(const char* filename);
    bool saveToFile(const char* filename) const;
    
    // Validation methods
    bool isValid(const Point& p) const;
    bool isWalkable(const Point& p) const;
    bool isStart(const Point& p) const;
    bool isGoal(const Point& p) const;
    
    // Get neighboring cells (abstraction for algorithm independence)
    void getNeighbors(const Point& p, Point* neighbors, int& count) const;
    
    // Display
    void display() const;
};

#endif
