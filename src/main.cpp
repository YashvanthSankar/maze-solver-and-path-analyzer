#include "Maze.h"
#include "Path.h"
#include "Point.h"
#include "BFSSolver.h"
#include "DijkstraSolver.h"
#include "PathAnalyzer.h"
#include "Renderer.h"
#include "MazeGenerator.h"
#include "CLIUtils.h"
#include <iostream>
#include <ctime>
#include <unistd.h>

class MazeSolverApp {
private:
    Maze maze_;
    Path bfsPath_;
    Path dijkstraPath_;
    bool mazeLoaded_;
    bool bfsSolved_;
    bool dijkstraSolved_;
    CLIUtils cli_;
    Renderer renderer_;
    
    void showWelcome();
    void showMainMenu();
    int getMenuChoice(int min, int max);
    
    // Menu handlers
    void handleLoadMaze();
    void handleGenerateMaze();
    void handleSolveBFS();
    void handleSolveDijkstra();
    void handleAnalyzePath();
    void handleComparePaths();
    void handleVisualize();
    void handleQuickSolve();
    void handleSettings();
    
public:
    MazeSolverApp();
    void run();
};

MazeSolverApp::MazeSolverApp() : mazeLoaded_(false), bfsSolved_(false), dijkstraSolved_(false) {}

void MazeSolverApp::showWelcome() {
    cli_.clearScreen();
    
    std::cout << "\n";
    cli_.printColored("  ███╗   ███╗ █████╗ ███████╗███████╗    ███████╗ ██████╗ ██╗    ██╗   ██╗███████╗██████╗ \n", "\033[1;36m");
    cli_.printColored("  ████╗ ████║██╔══██╗╚══███╔╝██╔════╝    ██╔════╝██╔═══██╗██║    ██║   ██║██╔════╝██╔══██╗\n", "\033[1;36m");
    cli_.printColored("  ██╔████╔██║███████║  ███╔╝ █████╗      ███████╗██║   ██║██║    ██║   ██║█████╗  ██████╔╝\n", "\033[1;36m");
    cli_.printColored("  ██║╚██╔╝██║██╔══██║ ███╔╝  ██╔══╝      ╚════██║██║   ██║██║    ╚██╗ ██╔╝██╔══╝  ██╔══██╗\n", "\033[1;36m");
    cli_.printColored("  ██║ ╚═╝ ██║██║  ██║███████╗███████╗    ███████║╚██████╔╝███████╗╚████╔╝ ███████╗██║  ██║\n", "\033[1;36m");
    cli_.printColored("  ╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝╚══════╝    ╚══════╝ ╚═════╝ ╚══════╝ ╚═══╝  ╚══════╝╚═╝  ╚═╝\n", "\033[1;36m");
    
    std::cout << "\n";
    cli_.printCentered("Advanced Pathfinding with OOP Concepts", 90);
    cli_.printCentered("Encapsulation • Abstraction • Operator Overloading", 90);
    std::cout << "\n";
    
    cli_.printSeparator();
    cli_.typewriterEffect("  Initializing maze solver engine...", 20);
    cli_.printSuccess("System ready!");
    
    cli_.waitForEnter();
}

void MazeSolverApp::showMainMenu() {
    cli_.clearScreen();
    cli_.printHeader("MAIN MENU");
    
    std::cout << "\n";
    cli_.printSubHeader("Maze Operations");
    std::cout << "  1. 📁 Load Maze from File\n";
    std::cout << "  2. ✨ Generate New Maze (Instant!)\n";
    std::cout << "  3. 🎯 Quick Solve (Generate + Solve)\n";
    std::cout << "\n";
    
    cli_.printSubHeader("Solving Algorithms");
    std::cout << "  4. 🔍 Solve with BFS\n";
    std::cout << "  5. 🚀 Solve with Dijkstra\n";
    std::cout << "  6. ⚡ Solve with Both\n";
    std::cout << "\n";
    
    cli_.printSubHeader("Analysis & Visualization");
    std::cout << "  7. 📊 Analyze Current Path\n";
    std::cout << "  8. 🔬 Compare Both Solutions\n";
    std::cout << "  9. 🎬 Animated Visualization\n";
    std::cout << " 10. 🖼️  Display Maze\n";
    std::cout << "\n";
    
    cli_.printSubHeader("Other");
    std::cout << " 11. ⚙️  Settings\n";
    std::cout << "  0. 🚪 Exit\n";
    
    cli_.printSeparator();
    
    if (mazeLoaded_) {
        cli_.printInfo("Status: Maze loaded ✓");
    } else {
        cli_.printWarning("Status: No maze loaded");
    }
    
    std::cout << "\n";
}

int MazeSolverApp::getMenuChoice(int min, int max) {
    return cli_.getNumberInput("Enter your choice: ", min, max);
}

void MazeSolverApp::handleLoadMaze() {
    cli_.clearScreen();
    cli_.printHeader("Load Maze from File");
    
    char filename[256];
    cli_.getStringInput("Enter filename: ", filename, 256);
    
    std::cout << "\n";
    cli_.drawSpinner(0);
    std::cout << " Loading maze...\r";
    std::cout.flush();
    
    if (maze_.loadFromFile(filename)) {
        cli_.printSuccess("Maze loaded successfully!");
        std::cout << "\n";
        std::cout << "  Dimensions: " << maze_.getWidth() << "x" << maze_.getHeight() << "\n";
        std::cout << "  Start: " << maze_.getStart() << "\n";
        std::cout << "  Goal: " << maze_.getGoal() << "\n";
        
        mazeLoaded_ = true;
        bfsSolved_ = false;
        dijkstraSolved_ = false;
    } else {
        cli_.printError("Failed to load maze!");
    }
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleGenerateMaze() {
    cli_.clearScreen();
    cli_.printHeader("Generate New Maze");
    
    std::cout << "\n";
    std::cout << "Select maze size:\n";
    std::cout << "  1. Small (15x15) - Easy\n";
    std::cout << "  2. Medium (25x25) - Normal\n";
    std::cout << "  3. Large (35x35) - Hard\n";
    std::cout << "  4. Custom size\n";
    
    int choice = getMenuChoice(1, 4);
    
    int seed = time(nullptr);
    MazeGenerator generator(20, 20, seed);
    
    std::cout << "\n";
    for (int i = 0; i < 10; i++) {
        cli_.drawSpinner(i);
        std::cout << " Generating perfect maze...\r";
        std::cout.flush();
        usleep(50000);
    }
    
    switch(choice) {
        case 1:
            maze_ = generator.generateEasy();
            break;
        case 2:
            maze_ = generator.generateMedium();
            break;
        case 3:
            maze_ = generator.generateHard();
            break;
        case 4: {
            int w = cli_.getNumberInput("Enter width (5-50): ", 5, 50);
            int h = cli_.getNumberInput("Enter height (5-50): ", 5, 50);
            generator.setDimensions(w, h);
            maze_ = generator.generatePerfect();
            break;
        }
    }
    
    std::cout << "\n";
    cli_.printSuccess("Maze generated successfully!");
    std::cout << "\n  Dimensions: " << maze_.getWidth() << "x" << maze_.getHeight() << "\n";
    
    mazeLoaded_ = true;
    bfsSolved_ = false;
    dijkstraSolved_ = false;
    
    // Show preview
    std::cout << "\n";
    cli_.printSubHeader("Preview");
    renderer_.render(maze_);
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleSolveBFS() {
    if (!mazeLoaded_) {
        cli_.printError("Please load or generate a maze first!");
        cli_.waitForEnter();
        return;
    }
    
    cli_.clearScreen();
    cli_.printHeader("BFS Solver");
    
    std::cout << "\n";
    BFSSolver solver;
    
    for (int i = 0; i < 20; i++) {
        cli_.drawSpinner(i);
        std::cout << " Exploring maze with BFS...\r";
        std::cout.flush();
        usleep(30000);
    }
    
    bfsPath_ = solver.solve(maze_);
    
    std::cout << "\n";
    if (bfsPath_.isEmpty()) {
        cli_.printError("No path found!");
    } else {
        cli_.printSuccess("Path found!");
        std::cout << "\n";
        std::cout << "  Path length: " << bfsPath_.getSize() << " steps\n";
        std::cout << "  Nodes explored: " << solver.getNodesExplored() << "\n";
        std::cout << "  Cost: " << bfsPath_.getCost() << "\n";
        bfsSolved_ = true;
        
        std::cout << "\n";
        cli_.printSubHeader("Path Preview");
        renderer_.render(maze_, bfsPath_);
    }
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleSolveDijkstra() {
    if (!mazeLoaded_) {
        cli_.printError("Please load or generate a maze first!");
        cli_.waitForEnter();
        return;
    }
    
    cli_.clearScreen();
    cli_.printHeader("Dijkstra Solver");
    
    std::cout << "\n";
    DijkstraSolver solver;
    
    for (int i = 0; i < 20; i++) {
        cli_.drawSpinner(i);
        std::cout << " Finding optimal path with Dijkstra...\r";
        std::cout.flush();
        usleep(30000);
    }
    
    dijkstraPath_ = solver.solve(maze_);
    
    std::cout << "\n";
    if (dijkstraPath_.isEmpty()) {
        cli_.printError("No path found!");
    } else {
        cli_.printSuccess("Path found!");
        std::cout << "\n";
        std::cout << "  Path length: " << dijkstraPath_.getSize() << " steps\n";
        std::cout << "  Nodes explored: " << solver.getNodesExplored() << "\n";
        std::cout << "  Cost: " << dijkstraPath_.getCost() << "\n";
        dijkstraSolved_ = true;
        
        std::cout << "\n";
        cli_.printSubHeader("Path Preview");
        renderer_.render(maze_, dijkstraPath_);
    }
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleAnalyzePath() {
    if (!bfsSolved_ && !dijkstraSolved_) {
        cli_.printError("Please solve the maze first!");
        cli_.waitForEnter();
        return;
    }
    
    cli_.clearScreen();
    cli_.printHeader("Path Analysis");
    
    Path* pathToAnalyze = bfsSolved_ ? &bfsPath_ : &dijkstraPath_;
    
    PathAnalyzer analyzer;
    PathMetrics metrics = analyzer.analyze(*pathToAnalyze, maze_);
    
    std::cout << "\n";
    cli_.printSubHeader("Detailed Metrics");
    metrics.display();
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleComparePaths() {
    if (!bfsSolved_ || !dijkstraSolved_) {
        cli_.printError("Please solve with both algorithms first!");
        cli_.waitForEnter();
        return;
    }
    
    cli_.clearScreen();
    cli_.printHeader("Path Comparison");
    
    PathAnalyzer analyzer;
    
    std::cout << "\n";
    cli_.printSubHeader("BFS Path Analysis");
    PathMetrics bfsMetrics = analyzer.analyze(bfsPath_, maze_);
    bfsMetrics.display();
    
    std::cout << "\n";
    cli_.printSubHeader("Dijkstra Path Analysis");
    PathMetrics dijkstraMetrics = analyzer.analyze(dijkstraPath_, maze_);
    dijkstraMetrics.display();
    
    std::cout << "\n";
    cli_.printSubHeader("Comparison Results");
    analyzer.comparePaths(bfsPath_, dijkstraPath_, maze_);
    
    std::cout << "\n";
    cli_.printSubHeader("Visual Comparison");
    renderer_.renderComparison(maze_, bfsPath_, dijkstraPath_);
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleVisualize() {
    if (!mazeLoaded_) {
        cli_.printError("Please load or generate a maze first!");
        cli_.waitForEnter();
        return;
    }
    
    cli_.clearScreen();
    cli_.printHeader("Visualization Options");
    
    std::cout << "\n";
    std::cout << "  1. Display maze only\n";
    std::cout << "  2. Display with BFS path\n";
    std::cout << "  3. Display with Dijkstra path\n";
    std::cout << "  4. Animated BFS solving\n";
    std::cout << "  5. Animated Dijkstra solving\n";
    std::cout << "  6. Compare both paths\n";
    
    int choice = getMenuChoice(1, 6);
    
    cli_.clearScreen();
    
    switch(choice) {
        case 1:
            cli_.printHeader("Maze View");
            renderer_.render(maze_);
            break;
        case 2:
            if (bfsSolved_) {
                cli_.printHeader("BFS Path");
                renderer_.render(maze_, bfsPath_);
            } else {
                cli_.printError("BFS not solved yet!");
            }
            break;
        case 3:
            if (dijkstraSolved_) {
                cli_.printHeader("Dijkstra Path");
                renderer_.render(maze_, dijkstraPath_);
            } else {
                cli_.printError("Dijkstra not solved yet!");
            }
            break;
        case 4:
            if (bfsSolved_) {
                renderer_.renderAnimated(maze_, bfsPath_, 80);
            } else {
                cli_.printError("BFS not solved yet!");
            }
            break;
        case 5:
            if (dijkstraSolved_) {
                renderer_.renderAnimated(maze_, dijkstraPath_, 80);
            } else {
                cli_.printError("Dijkstra not solved yet!");
            }
            break;
        case 6:
            if (bfsSolved_ && dijkstraSolved_) {
                cli_.printHeader("Path Comparison");
                renderer_.renderComparison(maze_, bfsPath_, dijkstraPath_);
            } else {
                cli_.printError("Please solve with both algorithms first!");
            }
            break;
    }
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleQuickSolve() {
    cli_.clearScreen();
    cli_.printHeader("Quick Solve");
    
    std::cout << "\n";
    cli_.printInfo("Generating maze...");
    
    int seed = time(nullptr);
    MazeGenerator generator(20, 20, seed);
    maze_ = generator.generateMedium();
    mazeLoaded_ = true;
    
    cli_.printSuccess("Maze generated!");
    
    std::cout << "\n";
    cli_.printInfo("Solving with BFS...");
    BFSSolver bfsSolver;
    bfsPath_ = bfsSolver.solve(maze_);
    bfsSolved_ = true;
    cli_.printSuccess("BFS complete!");
    
    std::cout << "\n";
    cli_.printInfo("Solving with Dijkstra...");
    DijkstraSolver dijkstraSolver;
    dijkstraPath_ = dijkstraSolver.solve(maze_);
    dijkstraSolved_ = true;
    cli_.printSuccess("Dijkstra complete!");
    
    std::cout << "\n";
    cli_.printHeader("Results");
    
    std::cout << "\n  BFS: " << bfsPath_.getSize() << " steps, cost " << bfsPath_.getCost() << "\n";
    std::cout << "  Dijkstra: " << dijkstraPath_.getSize() << " steps, cost " << dijkstraPath_.getCost() << "\n";
    
    std::cout << "\n";
    renderer_.renderComparison(maze_, bfsPath_, dijkstraPath_);
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleSettings() {
    cli_.clearScreen();
    cli_.printHeader("Settings");
    
    std::cout << "\n";
    std::cout << "  1. Toggle colors (currently: " << (cli_.areColorsEnabled() ? "ON" : "OFF") << ")\n";
    std::cout << "  2. Save current maze\n";
    std::cout << "  3. About\n";
    std::cout << "  0. Back\n";
    
    int choice = getMenuChoice(0, 3);
    
    switch(choice) {
        case 1:
            if (cli_.areColorsEnabled()) {
                cli_.disableColors();
                renderer_.setColorMode(false);
                cli_.printInfo("Colors disabled");
            } else {
                cli_.enableColors();
                renderer_.setColorMode(true);
                cli_.printSuccess("Colors enabled");
            }
            cli_.waitForEnter();
            break;
        case 2:
            if (mazeLoaded_) {
                char filename[256];
                cli_.getStringInput("Enter filename to save: ", filename, 256);
                if (maze_.saveToFile(filename)) {
                    cli_.printSuccess("Maze saved successfully!");
                } else {
                    cli_.printError("Failed to save maze!");
                }
            } else {
                cli_.printError("No maze loaded!");
            }
            cli_.waitForEnter();
            break;
        case 3:
            cli_.clearScreen();
            cli_.printHeader("About");
            std::cout << "\n";
            std::cout << "  Maze Solver & Path Analyzer\n";
            std::cout << "  Version 2.0\n";
            std::cout << "\n";
            std::cout << "  Demonstrates OOP Concepts:\n";
            std::cout << "    • Encapsulation\n";
            std::cout << "    • Abstraction\n";
            std::cout << "    • Operator Overloading\n";
            std::cout << "\n";
            std::cout << "  Features:\n";
            std::cout << "    • Instant maze generation\n";
            std::cout << "    • BFS & Dijkstra algorithms\n";
            std::cout << "    • Animated visualization\n";
            std::cout << "    • Path analysis & comparison\n";
            cli_.waitForEnter();
            break;
    }
}

void MazeSolverApp::run() {
    showWelcome();
    
    while (true) {
        showMainMenu();
        int choice = getMenuChoice(0, 11);
        
        switch(choice) {
            case 0:
                cli_.clearScreen();
                cli_.printSuccess("Thank you for using Maze Solver!");
                std::cout << "\n";
                return;
            case 1:
                handleLoadMaze();
                break;
            case 2:
                handleGenerateMaze();
                break;
            case 3:
                handleQuickSolve();
                break;
            case 4:
                handleSolveBFS();
                break;
            case 5:
                handleSolveDijkstra();
                break;
            case 6:
                handleSolveBFS();
                handleSolveDijkstra();
                break;
            case 7:
                handleAnalyzePath();
                break;
            case 8:
                handleComparePaths();
                break;
            case 9:
                handleVisualize();
                break;
            case 10:
                if (mazeLoaded_) {
                    cli_.clearScreen();
                    cli_.printHeader("Maze Display");
                    renderer_.render(maze_);
                    cli_.waitForEnter();
                } else {
                    cli_.printError("No maze loaded!");
                    cli_.waitForEnter();
                }
                break;
            case 11:
                handleSettings();
                break;
        }
    }
}

int main() {
    MazeSolverApp app;
    app.run();
    return 0;
}
