#ifndef POINT_H
#define POINT_H

#include <iostream>

// Point class demonstrating Encapsulation and Operator Overloading
class Point {
private:
    int x_;  // Encapsulated x coordinate
    int y_;  // Encapsulated y coordinate

public:
    // Constructor
    Point(int x = 0, int y = 0);
    
    // Getters (const methods for encapsulation)
    int getX() const;
    int getY() const;
    
    // Operator Overloading for Point arithmetic and comparison
    Point operator+(const Point& other) const;
    Point operator-(const Point& other) const;
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    bool operator<(const Point& other) const;  // For use in data structures
    
    // Friend function for output operator
    friend std::ostream& operator<<(std::ostream& os, const Point& p);
    
    // Manhattan distance to another point
    int manhattanDistance(const Point& other) const;
};

#endif
