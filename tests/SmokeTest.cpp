#include "Maze.h"
#include "Path.h"
#include "BFSSolver.h"
#include "DijkstraSolver.h"
#include "PathAnalyzer.h"
#include <cstddef>
#include <iostream>

namespace {

bool testPathAnalyzerSingleNode(std::ostream& err) {
    Maze maze(3, 3);
    Point pivot(1, 1);
    maze.setStart(pivot);
    maze.setGoal(pivot);
    maze.setCellAt(pivot, 'S');

    Path trivial;
    trivial.addPoint(pivot);
    trivial.setCost(0.0);

    PathAnalyzer analyzer;
    PathMetrics metrics = analyzer.analyze(trivial, maze);

    bool ok = true;
    if (metrics.getTotalLength() != 0) {
        err << "Expected zero length for single-node path\n";
        ok = false;
    }
    if (metrics.getAvgStepCost() != 0.0) {
        err << "Expected zero cost for single-node path\n";
        ok = false;
    }
    return ok;
}

bool testBfsShortestPath(std::ostream& err) {
    Maze maze(3, 3);
    maze.setStart(Point(0, 0));
    maze.setCellAt(0, 0, 'S');
    maze.setGoal(Point(2, 0));
    maze.setCellAt(2, 0, 'G');

    BFSSolver solver;
    Path path = solver.solve(maze);

    bool ok = true;
    if (path.getSize() != 3) {
        err << "BFS path expected 3 nodes, got " << path.getSize() << "\n";
        ok = false;
    }
    if (path.getCost() != 2.0) {
        err << "BFS cost expected 2, got " << path.getCost() << "\n";
        ok = false;
    }
    Point mid;
    if (!path.tryGetPoint(1, mid)) {
        err << "Failed to access BFS intermediate node\n";
        ok = false;
    } else {
        if (mid != Point(1, 0)) {
            err << "BFS mid point expected (1,0), got " << mid << "\n";
            ok = false;
        }
    }
    return ok;
}

bool testDijkstraWeightedCost(std::ostream& err) {
    Maze maze(3, 3);
    maze.setStart(Point(0, 0));
    maze.setCellAt(0, 0, 'S');
    maze.setGoal(Point(2, 0));
    maze.setCellAt(2, 0, 'G');
    maze.setCellAt(1, 0, '~'); // Weighted cell (cost 2)

    DijkstraSolver solver;
    Path path = solver.solve(maze);

    bool ok = true;
    if (path.getSize() != 3) {
        err << "Dijkstra path expected 3 nodes, got " << path.getSize() << "\n";
        ok = false;
    }
    if (path.getCost() != 3.0) {
        err << "Dijkstra cost expected 3, got " << path.getCost() << "\n";
        ok = false;
    }
    return ok;
}

} // namespace

int main() {
    int failures = 0;

    struct TestCase {
        const char* name;
        bool (*fn)(std::ostream&);
    };

    const TestCase cases[] = {
        {"PathAnalyzer single-node", &testPathAnalyzerSingleNode},
        {"BFS shortest path", &testBfsShortestPath},
        {"Dijkstra weighted cost", &testDijkstraWeightedCost}
    };

    for (std::size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        if (!cases[i].fn(std::cerr)) {
            std::cerr << "[FAIL] " << cases[i].name << "\n";
            ++failures;
        } else {
            std::cout << "[PASS] " << cases[i].name << "\n";
        }
    }

    if (failures == 0) {
        std::cout << "All smoke tests passed." << std::endl;
    }

    return failures;
}
