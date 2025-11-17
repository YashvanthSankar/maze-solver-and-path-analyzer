#include "DijkstraSolver.h"

#include <algorithm>
#include <limits>
#include <vector>

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

int PriorityQueue::parent(int i) {
    return (i - 1) / 2;
}

int PriorityQueue::leftChild(int i) {
    return 2 * i + 1;
}

int PriorityQueue::rightChild(int i) {
    return 2 * i + 2;
}

void PriorityQueue::heapifyUp(int index) {
    while (index > 0 && heap_[static_cast<std::size_t>(index)] < heap_[static_cast<std::size_t>(parent(index))]) {
        std::swap(heap_[static_cast<std::size_t>(index)], heap_[static_cast<std::size_t>(parent(index))]);
        index = parent(index);
    }
}

void PriorityQueue::heapifyDown(int index) {
    int minIndex = index;
    int left = leftChild(index);
    int right = rightChild(index);

    if (left < static_cast<int>(heap_.size()) && heap_[static_cast<std::size_t>(left)] < heap_[static_cast<std::size_t>(minIndex)]) {
        minIndex = left;
    }
    if (right < static_cast<int>(heap_.size()) && heap_[static_cast<std::size_t>(right)] < heap_[static_cast<std::size_t>(minIndex)]) {
        minIndex = right;
    }

    if (minIndex != index) {
        std::swap(heap_[static_cast<std::size_t>(index)], heap_[static_cast<std::size_t>(minIndex)]);
        heapifyDown(minIndex);
    }
}

void PriorityQueue::push(const PQNode& node) {
    heap_.push_back(node);
    heapifyUp(static_cast<int>(heap_.size()) - 1);
}

PQNode PriorityQueue::pop() {
    if (isEmpty()) {
        return PQNode();
    }

    PQNode result = heap_.front();
    heap_[0] = heap_.back();
    heap_.pop_back();

    if (!heap_.empty()) {
        heapifyDown(0);
    }

    return result;
}

bool PriorityQueue::isEmpty() const {
    return heap_.empty();
}

int PriorityQueue::getSize() const {
    return static_cast<int>(heap_.size());
}

DijkstraSolver::DijkstraSolver() = default;

int DijkstraSolver::pointToIndex(const Point& p, int width) const {
    return p.getY() * width + p.getX();
}

double DijkstraSolver::getCellCost(const Maze& maze, const Point& p) const {
    char cell = maze.getCellAt(p);

    if (cell == '.' || cell == 'S' || cell == 'G') return 1.0;
    if (cell == '~') return 2.0;
    if (cell == '^') return 3.0;

    return 1.0;
}

Path DijkstraSolver::reconstructPath(const Point& start, const Point& goal, int width) const {
    Path path;
    Point current = goal;

    std::vector<Point> reverse;
    reverse.reserve(parent_.size());

    while (!(current == start)) {
        reverse.push_back(current);
        int idx = pointToIndex(current, width);
        if (idx < 0 || idx >= static_cast<int>(parent_.size())) {
            return Path();
        }
        current = parent_[static_cast<std::size_t>(idx)];

        if (current == Point(-1, -1)) {
            return Path();
        }
    }

    reverse.push_back(start);

    for (auto it = reverse.rbegin(); it != reverse.rend(); ++it) {
        path.addPoint(*it);
    }

    int goalIndex = pointToIndex(goal, width);
    if (goalIndex >= 0 && goalIndex < static_cast<int>(distance_.size())) {
        path.setCost(distance_[static_cast<std::size_t>(goalIndex)]);
    }

    return path;
}

Path DijkstraSolver::solve(const Maze& maze) {
    Point start = maze.getStart();
    Point goal = maze.getGoal();
    int width = maze.getWidth();
    int height = maze.getHeight();

    parent_.assign(static_cast<std::size_t>(width * height), Point(-1, -1));
    distance_.assign(static_cast<std::size_t>(width * height), std::numeric_limits<double>::infinity());
    visited_.assign(static_cast<std::size_t>(width * height), false);

    PriorityQueue pq;
    int startIdx = pointToIndex(start, width);
    if (startIdx < 0 || startIdx >= static_cast<int>(distance_.size())) {
        return Path();
    }

    distance_[static_cast<std::size_t>(startIdx)] = 0.0;
    pq.push(PQNode(start, 0.0));

    bool found = false;

    while (!pq.isEmpty()) {
        PQNode current = pq.pop();
        Point currentPoint = current.getPoint();
        int currentIdx = pointToIndex(currentPoint, width);

        if (currentIdx < 0 || currentIdx >= static_cast<int>(visited_.size())) {
            continue;
        }

        if (visited_[static_cast<std::size_t>(currentIdx)]) continue;
        visited_[static_cast<std::size_t>(currentIdx)] = true;

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

            if (neighborIdx < 0 || neighborIdx >= static_cast<int>(visited_.size())) {
                continue;
            }

            if (!visited_[static_cast<std::size_t>(neighborIdx)]) {
                double newDist = distance_[static_cast<std::size_t>(currentIdx)] + getCellCost(maze, neighbor);

                if (newDist < distance_[static_cast<std::size_t>(neighborIdx)]) {
                    distance_[static_cast<std::size_t>(neighborIdx)] = newDist;
                    parent_[static_cast<std::size_t>(neighborIdx)] = currentPoint;
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
    for (bool visited : visited_) {
        if (visited) {
            count++;
        }
    }
    return count;
}
