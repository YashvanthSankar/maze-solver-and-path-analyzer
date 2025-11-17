#ifndef MAZE_H
#define MAZE_H

#include "Point.h"
#include <iostream>
#include <fstream>
#include <vector>

// Maze class demonstrating strong Encapsulation
class Maze {
private:
    std::vector<char> grid_;    // Encapsulated 2D grid stored row-major
    int width_;                 // Encapsulated dimensions
    int height_;
    Point start_;               // Encapsulated start position
    Point goal_;                // Encapsulated goal position

    int index(int x, int y) const;
    char getCellUnchecked(int x, int y) const;
    void setCellUnchecked(int x, int y, char value);

public:
    // Constructor and Destructor
    Maze();
    Maze(int width, int height);

    // Rule-of-zero semantics
    Maze(const Maze&) = default;
    Maze(Maze&&) noexcept = default;
    Maze& operator=(const Maze&) = default;
    Maze& operator=(Maze&&) noexcept = default;
    ~Maze() = default;

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
