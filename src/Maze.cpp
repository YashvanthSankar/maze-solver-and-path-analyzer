#include "Maze.h"

Maze::Maze() : grid_(nullptr), width_(0), height_(0), start_(0, 0), goal_(0, 0) {}

Maze::Maze(int width, int height) : width_(width), height_(height), start_(0, 0), goal_(0, 0) {
    allocateGrid();
    // Initialize with empty cells
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            grid_[i][j] = '.';
        }
    }
}

// Copy constructor
Maze::Maze(const Maze& other) : width_(other.width_), height_(other.height_), 
                                 start_(other.start_), goal_(other.goal_) {
    allocateGrid();
    copyGrid(other.grid_);
}

Maze::~Maze() {
    deallocateGrid();
}

// Assignment operator
Maze& Maze::operator=(const Maze& other) {
    if (this != &other) {
        deallocateGrid();
        
        width_ = other.width_;
        height_ = other.height_;
        start_ = other.start_;
        goal_ = other.goal_;
        
        allocateGrid();
        copyGrid(other.grid_);
    }
    return *this;
}

void Maze::allocateGrid() {
    grid_ = new char*[height_];
    for (int i = 0; i < height_; i++) {
        grid_[i] = new char[width_];
    }
}

void Maze::deallocateGrid() {
    if (grid_ != nullptr) {
        for (int i = 0; i < height_; i++) {
            delete[] grid_[i];
        }
        delete[] grid_;
        grid_ = nullptr;
    }
}

void Maze::copyGrid(char** source) {
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            grid_[i][j] = source[i][j];
        }
    }
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
        return grid_[y][x];
    }
    return '#';  // Out of bounds treated as wall
}

void Maze::setStart(const Point& p) {
    start_ = p;
}

void Maze::setGoal(const Point& p) {
    goal_ = p;
}

void Maze::setCellAt(const Point& p, char value) {
    setCellAt(p.getX(), p.getY(), value);
}

void Maze::setCellAt(int x, int y, char value) {
    if (x >= 0 && x < width_ && y >= 0 && y < height_) {
        grid_[y][x] = value;
    }
}

bool Maze::loadFromFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Read dimensions
    file >> width_ >> height_;
    
    if (width_ <= 0 || height_ <= 0) {
        return false;
    }
    
    deallocateGrid();
    allocateGrid();
    
    // Read grid
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            char c;
            file >> c;
            grid_[i][j] = c;
            
            if (c == 'S') {
                start_ = Point(j, i);
            } else if (c == 'G') {
                goal_ = Point(j, i);
            }
        }
    }
    
    file.close();
    return true;
}

bool Maze::saveToFile(const char* filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << width_ << " " << height_ << "\n";
    
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            file << grid_[i][j];
            if (j < width_ - 1) file << " ";
        }
        file << "\n";
    }
    
    file.close();
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
    
    // Four directions: up, right, down, left
    Point directions[4] = {
        Point(0, -1),   // Up
        Point(1, 0),    // Right
        Point(0, 1),    // Down
        Point(-1, 0)    // Left
    };
    
    for (int i = 0; i < 4; i++) {
        Point neighbor = p + directions[i];
        if (isWalkable(neighbor)) {
            neighbors[count++] = neighbor;
        }
    }
}

void Maze::display() const {
    std::cout << "Maze (" << width_ << "x" << height_ << "):\n";
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            std::cout << grid_[i][j] << " ";
        }
        std::cout << "\n";
    }
}
