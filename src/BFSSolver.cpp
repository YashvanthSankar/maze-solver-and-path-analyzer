#include "BFSSolver.h"

#include <vector>

void PointQueue::enqueue(const Point& p) {
    data_.push_back(p);
}

Point PointQueue::dequeue() {
    if (data_.empty()) {
        return Point(-1, -1);
    }
    Point front = data_.front();
    data_.pop_front();
    return front;
}

bool PointQueue::isEmpty() const {
    return data_.empty();
}

int PointQueue::getSize() const {
    return static_cast<int>(data_.size());
}

BFSSolver::BFSSolver() = default;

int BFSSolver::pointToIndex(const Point& p, int width) const {
    return p.getY() * width + p.getX();
}

Path BFSSolver::reconstructPath(const Point& start, const Point& goal, int width) const {
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

    path.setCost(static_cast<double>(reverse.size() - 1));
    return path;
}

Path BFSSolver::solve(const Maze& maze) {
    Point start = maze.getStart();
    Point goal = maze.getGoal();
    int width = maze.getWidth();
    int height = maze.getHeight();

    parent_.assign(static_cast<std::size_t>(width * height), Point(-1, -1));
    visited_.assign(static_cast<std::size_t>(width * height), false);

    PointQueue queue;
    queue.enqueue(start);
    visited_[static_cast<std::size_t>(pointToIndex(start, width))] = true;

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

            if (neighborIdx >= 0 &&
                neighborIdx < static_cast<int>(visited_.size()) &&
                !visited_[static_cast<std::size_t>(neighborIdx)]) {
                visited_[static_cast<std::size_t>(neighborIdx)] = true;
                parent_[static_cast<std::size_t>(neighborIdx)] = current;
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
    for (bool visited : visited_) {
        if (visited) {
            count++;
        }
    }
    return count;
}

std::string BFSSolver::name() const {
    return "Breadth-First Search";
}
