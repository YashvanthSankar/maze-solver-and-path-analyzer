#ifndef PATH_H
#define PATH_H

#include "Point.h"
#include <iostream>
#include <vector>

// Path class demonstrating Encapsulation and Operator Overloading
class Path {
private:
    std::vector<Point> points_;      // Encapsulated dynamic list of points
    double cost_;                    // Total path cost (encapsulated)

public:
    // Constructors / Rule of Zero
    Path();
    Path(const Path&) = default;
    Path(Path&&) noexcept = default;
    Path& operator=(const Path&) = default;
    Path& operator=(Path&&) noexcept = default;
    ~Path() = default;

    // Getters
    int getSize() const;
    double getCost() const;
    bool isEmpty() const;

    // Setter
    void setCost(double cost);

    // Add point to path
    void addPoint(const Point& p);

    // Operator Overloading
    Point operator[](int index) const;                  // Access by index (checked)
    Path operator+(const Path& other) const;            // Concatenate paths
    friend std::ostream& operator<<(std::ostream& os, const Path& path);

    // Utility methods
    void clear();
    Point getPointAt(int index) const;
    bool tryGetPoint(int index, Point& outPoint) const; // Safe access without assertions
};

#endif
