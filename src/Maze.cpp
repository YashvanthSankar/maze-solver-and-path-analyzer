#include "Maze.h"

#include <algorithm>

Maze::Maze() : grid_(), width_(0), height_(0), start_(0, 0), goal_(0, 0) {}

Maze::Maze(int width, int height)
    : grid_(static_cast<std::size_t>(width * height), '.'),
      width_(width),
      height_(height),
      start_(0, 0),
      goal_(0, 0) {}

int Maze::index(int x, int y) const {
    return y * width_ + x;
}

char Maze::getCellUnchecked(int x, int y) const {
    return grid_[static_cast<std::size_t>(index(x, y))];
}

void Maze::setCellUnchecked(int x, int y, char value) {
    grid_[static_cast<std::size_t>(index(x, y))] = value;
}

int Maze::getWidth() const {
    return width_;
}

int Maze::getHeight() const {
    return height_;
}

Point Maze::getStart() const {
    return start_;
}

Point Maze::getGoal() const {
    return goal_;
}

char Maze::getCellAt(const Point& p) const {
    return getCellAt(p.getX(), p.getY());
}

char Maze::getCellAt(int x, int y) const {
    if (x >= 0 && x < width_ && y >= 0 && y < height_) {
        return getCellUnchecked(x, y);
    }
    return '#';  
}

void Maze::setStart(const Point& p) {
    if (isValid(p)) {
        start_ = p;
    }
}

void Maze::setGoal(const Point& p) {
    if (isValid(p)) {
        goal_ = p;
    }
}

void Maze::setCellAt(const Point& p, char value) {
    setCellAt(p.getX(), p.getY(), value);
}

void Maze::setCellAt(int x, int y, char value) {
    if (x >= 0 && x < width_ && y >= 0 && y < height_) {
        setCellUnchecked(x, y, value);
    }
}

bool Maze::loadFromFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file >> width_ >> height_;

    if (width_ <= 0 || height_ <= 0) {
        return false;
    }

    grid_.assign(static_cast<std::size_t>(width_ * height_), '#');

    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            char c;
            file >> c;
            setCellUnchecked(x, y, c);

            if (c == 'S') {
                start_ = Point(x, y);
            } else if (c == 'G') {
                goal_ = Point(x, y);
            }
        }
    }

    return true;
}

bool Maze::saveToFile(const char* filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << width_ << " " << height_ << "\n";

    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            file << getCellUnchecked(x, y);
            if (x < width_ - 1) file << " ";
        }
        file << "\n";
    }

    return true;
}

bool Maze::isValid(const Point& p) const {
    int x = p.getX();
    int y = p.getY();
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

bool Maze::isWalkable(const Point& p) const {
    if (!isValid(p)) return false;
    char cell = getCellAt(p);
    return cell != '#';
}

bool Maze::isStart(const Point& p) const {
    return p == start_;
}

bool Maze::isGoal(const Point& p) const {
    return p == goal_;
}

void Maze::getNeighbors(const Point& p, Point* neighbors, int& count) const {
    count = 0;

    
    const Point directions[4] = {
        Point(0, -1),   
        Point(1, 0),    
        Point(0, 1),    
        Point(-1, 0)    
    };

    for (const auto& dir : directions) {
        Point neighbor = p + dir;
        if (isWalkable(neighbor)) {
            neighbors[count++] = neighbor;
        }
    }
}

void Maze::display() const {
    std::cout << "Maze (" << width_ << "x" << height_ << "):\n";
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            std::cout << getCellUnchecked(x, y) << " ";
        }
        std::cout << "\n";
    }
}
