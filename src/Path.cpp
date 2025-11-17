#include "Path.h"

#include <cassert>

Path::Path() : points_(), cost_(0.0) {}

int Path::getSize() const {
    return static_cast<int>(points_.size());
}

double Path::getCost() const {
    return cost_;
}

bool Path::isEmpty() const {
    return points_.empty();
}

void Path::setCost(double cost) {
    cost_ = cost;
}

void Path::addPoint(const Point& p) {
    points_.push_back(p);
}

Point Path::operator[](int index) const {
    assert(index >= 0 && index < getSize());
    return points_[static_cast<std::size_t>(index)];
}

Path Path::operator+(const Path& other) const {
    Path result;
    result.cost_ = cost_ + other.cost_;
    result.points_.reserve(points_.size() + other.points_.size());

    for (const auto& point : points_) {
        result.points_.push_back(point);
    }
    for (const auto& point : other.points_) {
        result.points_.push_back(point);
    }

    return result;
}

std::ostream& operator<<(std::ostream& os, const Path& path) {
    os << "Path[" << path.getSize() << " points, cost=" << path.cost_ << "]: ";
    for (int i = 0; i < path.getSize(); i++) {
        os << path.points_[static_cast<std::size_t>(i)];
        if (i < path.getSize() - 1) os << " -> ";
    }
    return os;
}

void Path::clear() {
    points_.clear();
    cost_ = 0.0;
}

Point Path::getPointAt(int index) const {
    return (*this)[index];
}

bool Path::tryGetPoint(int index, Point& outPoint) const {
    if (index < 0 || index >= getSize()) {
        return false;
    }
    outPoint = points_[static_cast<std::size_t>(index)];
    return true;
}
