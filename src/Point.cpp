#include "Point.h"

Point::Point(int x, int y) : x_(x), y_(y) {}

int Point::getX() const {
    return x_;
}

int Point::getY() const {
    return y_;
}

// Operator+ for adding two points (vector addition)
Point Point::operator+(const Point& other) const {
    return Point(x_ + other.x_, y_ + other.y_);
}

// Operator- for subtracting two points (vector subtraction)
Point Point::operator-(const Point& other) const {
    return Point(x_ - other.x_, y_ - other.y_);
}

// Equality comparison
bool Point::operator==(const Point& other) const {
    return x_ == other.x_ && y_ == other.y_;
}

// Inequality comparison
bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

// Less-than comparison (for ordering in containers)
bool Point::operator<(const Point& other) const {
    if (x_ != other.x_) return x_ < other.x_;
    return y_ < other.y_;
}

// Stream output operator
std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << "(" << p.x_ << "," << p.y_ << ")";
    return os;
}

// Calculate Manhattan distance
int Point::manhattanDistance(const Point& other) const {
    int dx = x_ - other.x_;
    int dy = y_ - other.y_;
    return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
}
