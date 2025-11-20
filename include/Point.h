#ifndef POINT_H
#define POINT_H

#include <iostream>


class Point {
private:
    int x_;  
    int y_;  

public:
    
    Point(int x = 0, int y = 0);
    
    
    int getX() const;
    int getY() const;
    
    
    Point operator+(const Point& other) const;
    Point operator-(const Point& other) const;
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    bool operator<(const Point& other) const;  
    
    
    friend std::ostream& operator<<(std::ostream& os, const Point& p);
    
    
    int manhattanDistance(const Point& other) const;
};

#endif
