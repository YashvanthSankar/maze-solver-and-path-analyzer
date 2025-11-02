#include "DijkstraSolver.h"

// PQNode implementation
PQNode::PQNode() : point_(0, 0), priority_(0.0) {}

PQNode::PQNode(const Point& p, double priority) : point_(p), priority_(priority) {}

Point PQNode::getPoint() const {
    return point_;
}

double PQNode::getPriority() const {
    return priority_;
}

bool PQNode::operator<(const PQNode& other) const {
    return priority_ < other.priority_;
}

bool PQNode::operator>(const PQNode& other) const {
    return priority_ > other.priority_;
}

// PriorityQueue implementation
PriorityQueue::PriorityQueue() : size_(0), capacity_(100) {
    heap_ = new PQNode[capacity_];
}

PriorityQueue::~PriorityQueue() {
    delete[] heap_;
}

void PriorityQueue::resize() {
    capacity_ *= 2;
    PQNode* newHeap = new PQNode[capacity_];
    for (int i = 0; i < size_; i++) {
        newHeap[i] = heap_[i];
    }
    delete[] heap_;
    heap_ = newHeap;
}

int PriorityQueue::parent(int i) const {
    return (i - 1) / 2;
}

int PriorityQueue::leftChild(int i) const {
    return 2 * i + 1;
}

int PriorityQueue::rightChild(int i) const {
    return 2 * i + 2;
}

void PriorityQueue::swap(int i, int j) {
    PQNode temp = heap_[i];
    heap_[i] = heap_[j];
    heap_[j] = temp;
}

void PriorityQueue::heapifyUp(int index) {
    while (index > 0 && heap_[index] < heap_[parent(index)]) {
        swap(index, parent(index));
        index = parent(index);
    }
}

void PriorityQueue::heapifyDown(int index) {
    int minIndex = index;
    int left = leftChild(index);
    int right = rightChild(index);
    
    if (left < size_ && heap_[left] < heap_[minIndex]) {
        minIndex = left;
    }
    if (right < size_ && heap_[right] < heap_[minIndex]) {
        minIndex = right;
    }
    
    if (minIndex != index) {
        swap(index, minIndex);
        heapifyDown(minIndex);
    }
}

void PriorityQueue::push(const PQNode& node) {
    if (size_ >= capacity_) {
        resize();
    }
    heap_[size_] = node;
    heapifyUp(size_);
    size_++;
}

PQNode PriorityQueue::pop() {
    if (isEmpty()) {
        return PQNode();
    }
    
    PQNode result = heap_[0];
    heap_[0] = heap_[size_ - 1];
    size_--;
    heapifyDown(0);
    
    return result;
}

bool PriorityQueue::isEmpty() const {
    return size_ == 0;
}

int PriorityQueue::getSize() const {
    return size_;
}

// DijkstraSolver implementation
DijkstraSolver::DijkstraSolver() : parent_(nullptr), distance_(nullptr), visited_(nullptr), maxNodes_(0) {}

DijkstraSolver::~DijkstraSolver() {
    cleanupSearchState();
}

int DijkstraSolver::pointToIndex(const Point& p, int width) const {
    return p.getY() * width + p.getX();
}

Point DijkstraSolver::indexToPoint(int index, int width) const {
    return Point(index % width, index / width);
}

void DijkstraSolver::initializeSearchState(int width, int height) {
    cleanupSearchState();
    
    maxNodes_ = width * height;
    parent_ = new Point[maxNodes_];
    distance_ = new double[maxNodes_];
    visited_ = new bool[maxNodes_];
    
    for (int i = 0; i < maxNodes_; i++) {
        parent_[i] = Point(-1, -1);
        distance_[i] = 999999.0;  // Large value as infinity
        visited_[i] = false;
    }
}

void DijkstraSolver::cleanupSearchState() {
    if (parent_ != nullptr) {
        delete[] parent_;
        parent_ = nullptr;
    }
    if (distance_ != nullptr) {
        delete[] distance_;
        distance_ = nullptr;
    }
    if (visited_ != nullptr) {
        delete[] visited_;
        visited_ = nullptr;
    }
    maxNodes_ = 0;
}

double DijkstraSolver::getCellCost(const Maze& maze, const Point& p) const {
    char cell = maze.getCellAt(p);
    
    // Different terrain costs
    if (cell == '.' || cell == 'S' || cell == 'G') return 1.0;
    if (cell == '~') return 2.0;  // Water (slower)
    if (cell == '^') return 3.0;  // Mountain (even slower)
    
    return 1.0;
}

Path DijkstraSolver::reconstructPath(const Point& start, const Point& goal, int width) const {
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
    
    // Set actual cost from distance array
    path.setCost(distance_[pointToIndex(goal, width)]);
    delete[] reversePath;
    
    return path;
}

Path DijkstraSolver::solve(const Maze& maze) {
    Point start = maze.getStart();
    Point goal = maze.getGoal();
    int width = maze.getWidth();
    int height = maze.getHeight();
    
    initializeSearchState(width, height);
    
    PriorityQueue pq;
    int startIdx = pointToIndex(start, width);
    distance_[startIdx] = 0.0;
    pq.push(PQNode(start, 0.0));
    
    bool found = false;
    
    while (!pq.isEmpty()) {
        PQNode current = pq.pop();
        Point currentPoint = current.getPoint();
        int currentIdx = pointToIndex(currentPoint, width);
        
        if (visited_[currentIdx]) continue;
        visited_[currentIdx] = true;
        
        if (currentPoint == goal) {
            found = true;
            break;
        }
        
        Point neighbors[4];
        int neighborCount;
        maze.getNeighbors(currentPoint, neighbors, neighborCount);
        
        for (int i = 0; i < neighborCount; i++) {
            Point neighbor = neighbors[i];
            int neighborIdx = pointToIndex(neighbor, width);
            
            if (!visited_[neighborIdx]) {
                double newDist = distance_[currentIdx] + getCellCost(maze, neighbor);
                
                if (newDist < distance_[neighborIdx]) {
                    distance_[neighborIdx] = newDist;
                    parent_[neighborIdx] = currentPoint;
                    pq.push(PQNode(neighbor, newDist));
                }
            }
        }
    }
    
    if (found) {
        return reconstructPath(start, goal, width);
    }
    
    return Path();
}

int DijkstraSolver::getNodesExplored() const {
    int count = 0;
    for (int i = 0; i < maxNodes_; i++) {
        if (visited_[i]) count++;
    }
    return count;
}
