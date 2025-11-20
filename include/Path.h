#ifndef PATH_H
#define PATH_H

#include "Point.h"
#include <iostream>
#include <vector>


class Path {
private:
    std::vector<Point> points_;      
    double cost_;                    

public:
    
    Path();
    Path(const Path&) = default;
    Path(Path&&) noexcept = default;
    Path& operator=(const Path&) = default;
    Path& operator=(Path&&) noexcept = default;
    ~Path() = default;

    
    int getSize() const;
    double getCost() const;
    bool isEmpty() const;

    
    void setCost(double cost);

    
    void addPoint(const Point& p);

    
    Point operator[](int index) const;                  
    Path operator+(const Path& other) const;            
    friend std::ostream& operator<<(std::ostream& os, const Path& path);

    
    void clear();
    Point getPointAt(int index) const;
    bool tryGetPoint(int index, Point& outPoint) const; 
};

#endif
