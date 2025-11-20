#ifndef MAZE_H
#define MAZE_H

#include "Point.h"
#include <iostream>
#include <fstream>
#include <vector>


class Maze {
private:
    std::vector<char> grid_;    
    int width_;                 
    int height_;
    Point start_;               
    Point goal_;                

    int index(int x, int y) const;
    char getCellUnchecked(int x, int y) const;
    void setCellUnchecked(int x, int y, char value);

public:
    
    Maze();
    Maze(int width, int height);

    
    Maze(const Maze&) = default;
    Maze(Maze&&) noexcept = default;
    Maze& operator=(const Maze&) = default;
    Maze& operator=(Maze&&) noexcept = default;
    ~Maze() = default;

    
    int getWidth() const;
    int getHeight() const;
    Point getStart() const;
    Point getGoal() const;
    char getCellAt(const Point& p) const;
    char getCellAt(int x, int y) const;

    
    void setStart(const Point& p);
    void setGoal(const Point& p);
    void setCellAt(const Point& p, char value);
    void setCellAt(int x, int y, char value);

    
    bool loadFromFile(const char* filename);
    bool saveToFile(const char* filename) const;

    
    bool isValid(const Point& p) const;
    bool isWalkable(const Point& p) const;
    bool isStart(const Point& p) const;
    bool isGoal(const Point& p) const;

    
    void getNeighbors(const Point& p, Point* neighbors, int& count) const;

    
    void display() const;
};

#endif
