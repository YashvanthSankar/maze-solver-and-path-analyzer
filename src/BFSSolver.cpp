#include "BFSSolver.h"

PointQueue::PointQueue() : front_(0), rear_(0), capacity_(100), size_(0) {
    data_ = new Point[capacity_];
}

PointQueue::~PointQueue() {
    delete[] data_;
}

void PointQueue::resize() {
    int newCapacity = capacity_ * 2;
    Point* newData = new Point[newCapacity];
    
    int index = 0;
    for (int i = 0; i < size_; i++) {
        newData[index++] = data_[(front_ + i) % capacity_];
    }
    
    delete[] data_;
    data_ = newData;
    front_ = 0;
    rear_ = size_;
    capacity_ = newCapacity;
}

void PointQueue::enqueue(const Point& p) {
    if (size_ >= capacity_) {
        resize();
    }
    data_[rear_] = p;
    rear_ = (rear_ + 1) % capacity_;
    size_++;
}

Point PointQueue::dequeue() {
    if (isEmpty()) {
        return Point(-1, -1);
    }
    Point result = data_[front_];
    front_ = (front_ + 1) % capacity_;
    size_--;
    return result;
}

bool PointQueue::isEmpty() const {
    return size_ == 0;
}

int PointQueue::getSize() const {
    return size_;
}

BFSSolver::BFSSolver() : parent_(nullptr), visited_(nullptr), maxNodes_(0) {}

BFSSolver::~BFSSolver() {
    cleanupSearchState();
}

int BFSSolver::pointToIndex(const Point& p, int width) const {
    return p.getY() * width + p.getX();
}

Point BFSSolver::indexToPoint(int index, int width) const {
    return Point(index % width, index / width);
}

void BFSSolver::initializeSearchState(int width, int height) {
    cleanupSearchState();
    
    maxNodes_ = width * height;
    parent_ = new Point[maxNodes_];
    visited_ = new bool[maxNodes_];
    
    for (int i = 0; i < maxNodes_; i++) {
        parent_[i] = Point(-1, -1);
        visited_[i] = false;
    }
}

void BFSSolver::cleanupSearchState() {
    if (parent_ != nullptr) {
        delete[] parent_;
        parent_ = nullptr;
    }
    if (visited_ != nullptr) {
        delete[] visited_;
        visited_ = nullptr;
    }
    maxNodes_ = 0;
}

Path BFSSolver::reconstructPath(const Point& start, const Point& goal, int width) const {
    Path path;
    Point current = goal;
    
    Point* reversePath = new Point[maxNodes_];
    int pathLength = 0;
    
    while (!(current == start)) {
        reversePath[pathLength++] = current;
        int idx = pointToIndex(current, width);
        current = parent_[idx];
        
        if (current == Point(-1, -1)) {
            delete[] reversePath;
            return path; 
        }
    }
    reversePath[pathLength++] = start;
    
    for (int i = pathLength - 1; i >= 0; i--) {
        path.addPoint(reversePath[i]);
    }
    
    path.setCost(pathLength - 1);
    delete[] reversePath;
    
    return path;
}

Path BFSSolver::solve(const Maze& maze) {
    Point start = maze.getStart();
    Point goal = maze.getGoal();
    int width = maze.getWidth();
    int height = maze.getHeight();
    
    initializeSearchState(width, height);
    
    PointQueue queue;
    queue.enqueue(start);
    visited_[pointToIndex(start, width)] = true;
    
    bool found = false;
    
    while (!queue.isEmpty()) {
        Point current = queue.dequeue();
        
        if (current == goal) {
            found = true;
            break;
        }
        
        Point neighbors[4];
        int neighborCount;
        maze.getNeighbors(current, neighbors, neighborCount);
        
        for (int i = 0; i < neighborCount; i++) {
            Point neighbor = neighbors[i];
            int neighborIdx = pointToIndex(neighbor, width);
            
            if (!visited_[neighborIdx]) {
                visited_[neighborIdx] = true;
                parent_[neighborIdx] = current;
                queue.enqueue(neighbor);
            }
        }
    }
    
    if (found) {
        return reconstructPath(start, goal, width);
    }
    
    return Path();
}

int BFSSolver::getNodesExplored() const {
    int count = 0;
    for (int i = 0; i < maxNodes_; i++) {
        if (visited_[i]) count++;
    }
    return count;
}
