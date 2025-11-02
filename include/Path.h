#ifndef PATH_H
#define PATH_H

#include "Point.h"
#include <iostream>

// Path class demonstrating Encapsulation and Operator Overloading
class Path {
private:
    Point* points_;      // Encapsulated dynamic array of points
    int size_;           // Current number of points
    int capacity_;       // Capacity of array
    double cost_;        // Total path cost (encapsulated)
    
    // Private helper to resize array
    void resize();

public:
    // Constructor and Destructor (RAII)
    Path();
    Path(const Path& other);  // Copy constructor
    ~Path();
    
    // Assignment operator
    Path& operator=(const Path& other);
    
    // Getters
    int getSize() const;
    double getCost() const;
    bool isEmpty() const;
    
    // Setter
    void setCost(double cost);
    
    // Add point to path
    void addPoint(const Point& p);
    
    // Operator Overloading
    Point operator[](int index) const;  // Access by index
    Path operator+(const Path& other) const;  // Concatenate paths
    friend std::ostream& operator<<(std::ostream& os, const Path& path);
    
    // Utility methods
    void clear();
    Point getPointAt(int index) const;
};

#endif
