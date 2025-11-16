#include "Path.h"

Path::Path() : points_(nullptr), size_(0), capacity_(0), cost_(0.0) {
    capacity_ = 10;
    points_ = new Point[capacity_];
}

Path::Path(const Path& other) : size_(other.size_), capacity_(other.capacity_), cost_(other.cost_) {
    points_ = new Point[capacity_];
    for (int i = 0; i < size_; i++) {
        points_[i] = other.points_[i];
    }
}

Path::~Path() {
    delete[] points_;
}

Path& Path::operator=(const Path& other) {
    if (this != &other) {
        delete[] points_;
        
        size_ = other.size_;
        capacity_ = other.capacity_;
        cost_ = other.cost_;
        points_ = new Point[capacity_];
        
        for (int i = 0; i < size_; i++) {
            points_[i] = other.points_[i];
        }
    }
    return *this;
}

int Path::getSize() const {
    return size_;
}

double Path::getCost() const {
    return cost_;
}

bool Path::isEmpty() const {
    return size_ == 0;
}

void Path::setCost(double cost) {
    cost_ = cost;
}

void Path::resize() {
    capacity_ *= 2;
    Point* newPoints = new Point[capacity_];
    for (int i = 0; i < size_; i++) {
        newPoints[i] = points_[i];
    }
    delete[] points_;
    points_ = newPoints;
}

void Path::addPoint(const Point& p) {
    if (size_ >= capacity_) {
        resize();
    }
    points_[size_++] = p;
}

Point Path::operator[](int index) const {
    if (index >= 0 && index < size_) {
        return points_[index];
    }
    return Point(-1, -1);
}

Path Path::operator+(const Path& other) const {
    Path result;
    result.cost_ = cost_ + other.cost_;
    
    for (int i = 0; i < size_; i++) {
        result.addPoint(points_[i]);
    }
    for (int i = 0; i < other.size_; i++) {
        result.addPoint(other.points_[i]);
    }
    
    return result;
}

std::ostream& operator<<(std::ostream& os, const Path& path) {
    os << "Path[" << path.size_ << " points, cost=" << path.cost_ << "]: ";
    for (int i = 0; i < path.size_; i++) {
        os << path.points_[i];
        if (i < path.size_ - 1) os << " -> ";
    }
    return os;
}

void Path::clear() {
    size_ = 0;
    cost_ = 0.0;
}

Point Path::getPointAt(int index) const {
    return (*this)[index];
}
