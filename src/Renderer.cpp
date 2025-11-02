#include "Renderer.h"
#include <fstream>
#include <unistd.h>

Renderer::Renderer() : displayGrid_(nullptr), width_(0), height_(0), useColors_(true) {}

Renderer::~Renderer() {
    deallocateDisplayGrid();
}

void Renderer::allocateDisplayGrid() {
    displayGrid_ = new char*[height_];
    for (int i = 0; i < height_; i++) {
        displayGrid_[i] = new char[width_];
    }
}

void Renderer::deallocateDisplayGrid() {
    if (displayGrid_ != nullptr) {
        for (int i = 0; i < height_; i++) {
            delete[] displayGrid_[i];
        }
        delete[] displayGrid_;
        displayGrid_ = nullptr;
    }
}

void Renderer::copyMazeToDisplay(const Maze& maze) {
    deallocateDisplayGrid();
    
    width_ = maze.getWidth();
    height_ = maze.getHeight();
    allocateDisplayGrid();
    
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            displayGrid_[i][j] = maze.getCellAt(j, i);
        }
    }
}

void Renderer::overlayPath(const Path& path) {
    for (int i = 0; i < path.getSize(); i++) {
        Point p = path[i];
        int x = p.getX();
        int y = p.getY();
        
        if (x >= 0 && x < width_ && y >= 0 && y < height_) {
            // Don't overwrite start (S) and goal (G)
            if (displayGrid_[y][x] != 'S' && displayGrid_[y][x] != 'G') {
                displayGrid_[y][x] = '*';
            }
        }
    }
}

void Renderer::overlayMultiplePaths(const Path* paths, int pathCount) {
    // First path uses '*', second uses '+'
    char markers[2] = {'*', '+'};
    
    for (int p = 0; p < pathCount && p < 2; p++) {
        for (int i = 0; i < paths[p].getSize(); i++) {
            Point pt = paths[p][i];
            int x = pt.getX();
            int y = pt.getY();
            
            if (x >= 0 && x < width_ && y >= 0 && y < height_) {
                if (displayGrid_[y][x] != 'S' && displayGrid_[y][x] != 'G') {
                    displayGrid_[y][x] = markers[p];
                }
            }
        }
    }
}

void Renderer::render(const Maze& maze) {
    copyMazeToDisplay(maze);
    
    std::cout << "+";
    for (int i = 0; i < width_; i++) std::cout << "-";
    std::cout << "+\n";
    
    for (int i = 0; i < height_; i++) {
        std::cout << "|";
        for (int j = 0; j < width_; j++) {
            char cell = displayGrid_[i][j];
            if (useColors_) {
                std::cout << getCellColor(cell) << cell << "\033[0m";
            } else {
                std::cout << cell;
            }
        }
        std::cout << "|\n";
    }
    
    std::cout << "+";
    for (int i = 0; i < width_; i++) std::cout << "-";
    std::cout << "+\n";
    
    if (useColors_) {
        std::cout << "\n\033[2mLegend: \033[32mS\033[0m\033[2m=Start, \033[31mG\033[0m\033[2m=Goal, \033[90m#\033[0m\033[2m=Wall, .=Path, \033[36m~\033[0m\033[2m=Water, \033[33m^\033[0m\033[2m=Mountain\033[0m\n";
    } else {
        std::cout << "\nLegend: S=Start, G=Goal, #=Wall, .=Path, ~=Water, ^=Mountain\n";
    }
}

const char* Renderer::getCellColor(char cell) const {
    if (!useColors_) return "";
    
    switch(cell) {
        case 'S': return "\033[1;32m";  // Bold Green
        case 'G': return "\033[1;31m";  // Bold Red
        case '#': return "\033[90m";    // Dark Gray
        case '*': return "\033[1;33m";  // Bold Yellow
        case '+': return "\033[1;35m";  // Bold Magenta
        case '~': return "\033[36m";    // Cyan
        case '^': return "\033[33m";    // Yellow
        case '.': return "\033[2m";     // Dim
        default: return "";
    }
}

void Renderer::setColorMode(bool enabled) {
    useColors_ = enabled;
    cli_.enableColors();
    if (!enabled) cli_.disableColors();
}

void Renderer::renderAnimated(const Maze& maze, const Path& path, int delayMs) {
    cli_.clearScreen();
    cli_.printHeader("Animated Path Discovery");
    
    for (int step = 0; step <= path.getSize(); step++) {
        cli_.moveCursor(3, 1);
        
        copyMazeToDisplay(maze);
        
        // Overlay path up to current step
        for (int i = 0; i < step && i < path.getSize(); i++) {
            Point p = path[i];
            int x = p.getX();
            int y = p.getY();
            if (x >= 0 && x < width_ && y >= 0 && y < height_) {
                if (displayGrid_[y][x] != 'S' && displayGrid_[y][x] != 'G') {
                    displayGrid_[y][x] = '*';
                }
            }
        }
        
        // Display
        std::cout << "+";
        for (int i = 0; i < width_; i++) std::cout << "-";
        std::cout << "+\n";
        
        for (int i = 0; i < height_; i++) {
            std::cout << "|";
            for (int j = 0; j < width_; j++) {
                char cell = displayGrid_[i][j];
                if (useColors_) {
                    std::cout << getCellColor(cell) << cell << "\033[0m";
                } else {
                    std::cout << cell;
                }
            }
            std::cout << "|\n";
        }
        
        std::cout << "+";
        for (int i = 0; i < width_; i++) std::cout << "-";
        std::cout << "+\n";
        
        std::cout << "\nStep: " << step << "/" << path.getSize();
        cli_.drawProgressBar(step, path.getSize(), 40);
        std::cout << "\n";
        
        usleep(delayMs * 1000);
    }
    
    std::cout << "\n";
    cli_.printSuccess("Path discovery complete!");
}

void Renderer::render(const Maze& maze, const Path& path) {
    copyMazeToDisplay(maze);
    overlayPath(path);
    
    std::cout << "\n";
    std::cout << "+";
    for (int i = 0; i < width_; i++) std::cout << "-";
    std::cout << "+\n";
    
    for (int i = 0; i < height_; i++) {
        std::cout << "|";
        for (int j = 0; j < width_; j++) {
            std::cout << displayGrid_[i][j];
        }
        std::cout << "|\n";
    }
    
    std::cout << "+";
    for (int i = 0; i < width_; i++) std::cout << "-";
    std::cout << "+\n";
    
    std::cout << "\nLegend: S=Start, G=Goal, #=Wall, *=Path, ~=Water, ^=Mountain\n";
}

void Renderer::renderComparison(const Maze& maze, const Path& path1, const Path& path2) {
    copyMazeToDisplay(maze);
    
    Path paths[2] = {path1, path2};
    overlayMultiplePaths(paths, 2);
    
    std::cout << "\n";
    std::cout << "+";
    for (int i = 0; i < width_; i++) std::cout << "-";
    std::cout << "+\n";
    
    for (int i = 0; i < height_; i++) {
        std::cout << "|";
        for (int j = 0; j < width_; j++) {
            std::cout << displayGrid_[i][j];
        }
        std::cout << "|\n";
    }
    
    std::cout << "+";
    for (int i = 0; i < width_; i++) std::cout << "-";
    std::cout << "+\n";
    
    std::cout << "\nLegend: S=Start, G=Goal, #=Wall, *=Path1(BFS), +=Path2(Dijkstra)\n";
}

bool Renderer::saveToFile(const char* filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (int i = 0; i < height_; i++) {
        for (int j = 0; j < width_; j++) {
            file << displayGrid_[i][j];
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

void Renderer::clear() {
    deallocateDisplayGrid();
    width_ = 0;
    height_ = 0;
}
