#include "Point.h"

Point::Point(int x, int y) : x_(x), y_(y) {}

int Point::getX() const {
    return x_;
}

int Point::getY() const {
    return y_;
}


Point Point::operator+(const Point& other) const {
    return Point(x_ + other.x_, y_ + other.y_);
}


Point Point::operator-(const Point& other) const {
    return Point(x_ - other.x_, y_ - other.y_);
}


bool Point::operator==(const Point& other) const {
    return x_ == other.x_ && y_ == other.y_;
}


bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}


bool Point::operator<(const Point& other) const {
    if (x_ != other.x_) return x_ < other.x_;
    return y_ < other.y_;
}


std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << "(" << p.x_ << "," << p.y_ << ")";
    return os;
}


int Point::manhattanDistance(const Point& other) const {
    int dx = x_ - other.x_;
    int dy = y_ - other.y_;
    return (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
}
