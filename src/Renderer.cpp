#include "Renderer.h"
#include <fstream>
#include <unistd.h>

Renderer::Renderer() : displayGrid_(), width_(0), height_(0), useColors_(true) {}

int Renderer::index(int x, int y) const {
    return y * width_ + x;
}

void Renderer::copyMazeToDisplay(const Maze& maze) {
    width_ = maze.getWidth();
    height_ = maze.getHeight();
    displayGrid_.assign(static_cast<std::size_t>(width_ * height_), ' ');

    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            displayGrid_[static_cast<std::size_t>(index(x, y))] = maze.getCellAt(x, y);
        }
    }
}

void Renderer::overlayPath(const Path& path) {
    for (int i = 0; i < path.getSize(); i++) {
        Point p = path[i];
        int x = p.getX();
        int y = p.getY();

        if (x >= 0 && x < width_ && y >= 0 && y < height_) {
            char& cell = displayGrid_[static_cast<std::size_t>(index(x, y))];
            if (cell != 'S' && cell != 'G') {
                cell = '*';
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
                char& cell = displayGrid_[static_cast<std::size_t>(index(x, y))];
                if (cell != 'S' && cell != 'G') {
                    cell = markers[p];
                }
            }
        }
    }
}

void Renderer::render(const Maze& maze) {
    copyMazeToDisplay(maze);
    
    // Make cells wider for better visibility (each cell is printed cellW characters)
    const int cellW = 3;  // Width multiplier for each cell
    
    // Top border with better box-drawing characters
    std::cout << "╔";
    for (int i = 0; i < width_; i++) {
        for (int k = 0; k < cellW; k++) std::cout << "═";
    }
    std::cout << "╗\n";
    
    for (int i = 0; i < height_; i++) {
        std::cout << "║";
        for (int j = 0; j < width_; j++) {
            char cell = displayGrid_[static_cast<std::size_t>(index(j, i))];
            if (useColors_) {
                std::cout << getCellColor(cell);
                // Use better symbols for each cell type
                switch(cell) {
                    case '#': 
                        for (int k = 0; k < cellW; k++) std::cout << "█"; 
                        break;  // Full block for walls
                    case 'S': 
                        for (int k = 0; k < cellW; k++) std::cout << "S"; 
                        break;  // Start
                    case 'G': 
                        for (int k = 0; k < cellW; k++) std::cout << "G"; 
                        break;  // Goal
                    case '.': 
                        for (int k = 0; k < cellW; k++) std::cout << " "; 
                        break;  // Empty space
                    case '*': 
                        for (int k = 0; k < cellW; k++) std::cout << "●"; 
                        break;  // Path dot
                    case '+': 
                        for (int k = 0; k < cellW; k++) std::cout << "○"; 
                        break;  // Alternative path
                    case '~': 
                        for (int k = 0; k < cellW; k++) std::cout << "≈"; 
                        break;  // Water
                    case '^': 
                        for (int k = 0; k < cellW; k++) std::cout << "▲"; 
                        break;  // Mountain
                    default: 
                        for (int k = 0; k < cellW; k++) std::cout << cell;
                }
                std::cout << "\033[0m";
            } else {
                for (int k = 0; k < cellW; k++) std::cout << cell;
            }
        }
        std::cout << "║\n";
    }
    
    // Bottom border
    std::cout << "╚";
    for (int i = 0; i < width_; i++) {
        for (int k = 0; k < cellW; k++) std::cout << "═";
    }
    std::cout << "╝\n";
    
    if (useColors_) {
        std::cout << "\n\033[2mLegend: \033[1;32mS\033[0m\033[2m=Start  \033[1;31mG\033[0m\033[2m=Goal  \033[90m█\033[0m\033[2m=Wall  \033[1;33m●\033[0m\033[2m=Path  \033[36m≈\033[0m\033[2m=Water  \033[33m▲\033[0m\033[2m=Mountain\033[0m\n";
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
    
    // Make cells wider for better visibility (each cell is printed cellW characters)
    const int cellW = 3;  // Width multiplier for each cell
    
    for (int step = 0; step <= path.getSize(); step++) {
        cli_.moveCursor(3, 1);
        
        copyMazeToDisplay(maze);
        
        // Overlay path up to current step
        for (int i = 0; i < step && i < path.getSize(); i++) {
            Point p = path[i];
            int x = p.getX();
            int y = p.getY();
            if (x >= 0 && x < width_ && y >= 0 && y < height_) {
                char& cell = displayGrid_[static_cast<std::size_t>(index(x, y))];
                if (cell != 'S' && cell != 'G') {
                    cell = '*';
                }
            }
        }
        
        // Display with better UTF-8 borders
        std::cout << "╔";
        for (int i = 0; i < width_; i++) {
            for (int k = 0; k < cellW; k++) std::cout << "═";
        }
        std::cout << "╗\n";
        
        for (int i = 0; i < height_; i++) {
            std::cout << "║";
            for (int j = 0; j < width_; j++) {
                char cell = displayGrid_[static_cast<std::size_t>(index(j, i))];
                if (useColors_) {
                    std::cout << getCellColor(cell);
                    // Use better symbols
                    switch(cell) {
                        case '#': 
                            for (int k = 0; k < cellW; k++) std::cout << "█"; 
                            break;
                        case 'S': 
                            for (int k = 0; k < cellW; k++) std::cout << "S"; 
                            break;
                        case 'G': 
                            for (int k = 0; k < cellW; k++) std::cout << "G"; 
                            break;
                        case '.': 
                            for (int k = 0; k < cellW; k++) std::cout << " "; 
                            break;
                        case '*': 
                            for (int k = 0; k < cellW; k++) std::cout << "●"; 
                            break;
                        case '+': 
                            for (int k = 0; k < cellW; k++) std::cout << "○"; 
                            break;
                        case '~': 
                            for (int k = 0; k < cellW; k++) std::cout << "≈"; 
                            break;
                        case '^': 
                            for (int k = 0; k < cellW; k++) std::cout << "▲"; 
                            break;
                        default: 
                            for (int k = 0; k < cellW; k++) std::cout << cell;
                    }
                    std::cout << "\033[0m";
                } else {
                    for (int k = 0; k < cellW; k++) std::cout << cell;
                }
            }
            std::cout << "║\n";
        }
        
        std::cout << "╚";
        for (int i = 0; i < width_; i++) {
            for (int k = 0; k < cellW; k++) std::cout << "═";
        }
        std::cout << "╝\n";
        
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
    
    // Make cells wider for better visibility (each cell is printed cellW characters)
    const int cellW = 3;  // Width multiplier for each cell
    
    // Top border with better box-drawing characters
    std::cout << "╔";
    for (int i = 0; i < width_; i++) {
        for (int k = 0; k < cellW; k++) std::cout << "═";
    }
    std::cout << "╗\n";
    
    for (int i = 0; i < height_; i++) {
        std::cout << "║";
        for (int j = 0; j < width_; j++) {
            char cell = displayGrid_[static_cast<std::size_t>(index(j, i))];
            if (useColors_) {
                std::cout << getCellColor(cell);
                // Use better symbols for each cell type
                switch(cell) {
                    case '#': 
                        for (int k = 0; k < cellW; k++) std::cout << "█"; 
                        break;  // Full block for walls
                    case 'S': 
                        for (int k = 0; k < cellW; k++) std::cout << "S"; 
                        break;  // Start
                    case 'G': 
                        for (int k = 0; k < cellW; k++) std::cout << "G"; 
                        break;  // Goal
                    case '.': 
                        for (int k = 0; k < cellW; k++) std::cout << " "; 
                        break;  // Empty space
                    case '*': 
                        for (int k = 0; k < cellW; k++) std::cout << "●"; 
                        break;  // Path dot
                    case '+': 
                        for (int k = 0; k < cellW; k++) std::cout << "○"; 
                        break;  // Alternative path
                    case '~': 
                        for (int k = 0; k < cellW; k++) std::cout << "≈"; 
                        break;  // Water
                    case '^': 
                        for (int k = 0; k < cellW; k++) std::cout << "▲"; 
                        break;  // Mountain
                    default: 
                        for (int k = 0; k < cellW; k++) std::cout << cell;
                }
                std::cout << "\033[0m";
            } else {
                for (int k = 0; k < cellW; k++) std::cout << cell;
            }
        }
        std::cout << "║\n";
    }
    
    // Bottom border
    std::cout << "╚";
    for (int i = 0; i < width_; i++) {
        for (int k = 0; k < cellW; k++) std::cout << "═";
    }
    std::cout << "╝\n";
    
    if (useColors_) {
        std::cout << "\n\033[2mLegend: \033[1;32mS\033[0m\033[2m=Start  \033[1;31mG\033[0m\033[2m=Goal  \033[90m█\033[0m\033[2m=Wall  \033[1;33m●\033[0m\033[2m=Path  \033[36m≈\033[0m\033[2m=Water  \033[33m▲\033[0m\033[2m=Mountain\033[0m\n";
    } else {
        std::cout << "\nLegend: S=Start, G=Goal, #=Wall, *=Path, ~=Water, ^=Mountain\n";
    }
}

void Renderer::renderComparison(const Maze& maze, const Path& path1, const Path& path2) {
    copyMazeToDisplay(maze);
    
    Path paths[2] = {path1, path2};
    overlayMultiplePaths(paths, 2);
    
    // Make cells wider for better visibility (each cell is printed cellW characters)
    const int cellW = 3;  // Width multiplier for each cell
    
    std::cout << "\n";
    std::cout << "\033[1;35m╔";
    for (int i = 0; i < width_; i++) {
        for (int k = 0; k < cellW; k++) std::cout << "═";
    }
    std::cout << "╗\033[0m\n";
    
    for (int i = 0; i < height_; i++) {
        std::cout << "\033[1;35m║\033[0m";
        for (int j = 0; j < width_; j++) {
            char cell = displayGrid_[static_cast<std::size_t>(index(j, i))];
            if (useColors_) {
                std::cout << getCellColor(cell);
            }
            
            // Use UTF-8 symbols for better visualization
            switch(cell) {
                case '#': 
                    for (int k = 0; k < cellW; k++) std::cout << "█"; 
                    break;
                case 'S': 
                    for (int k = 0; k < cellW; k++) std::cout << "S"; 
                    break;
                case 'G': 
                    for (int k = 0; k < cellW; k++) std::cout << "G"; 
                    break;
                case '.': 
                    for (int k = 0; k < cellW; k++) std::cout << " "; 
                    break;
                case '*': 
                    for (int k = 0; k < cellW; k++) std::cout << "●"; 
                    break;  // BFS path
                case '+': 
                    for (int k = 0; k < cellW; k++) std::cout << "○"; 
                    break;  // Dijkstra path
                case '~': 
                    for (int k = 0; k < cellW; k++) std::cout << "≈"; 
                    break;
                case '^': 
                    for (int k = 0; k < cellW; k++) std::cout << "▲"; 
                    break;
                default: 
                    for (int k = 0; k < cellW; k++) std::cout << cell;
            }
            
            if (useColors_) {
                std::cout << "\033[0m";
            }
        }
        std::cout << "\033[1;35m║\033[0m\n";
    }
    
    std::cout << "\033[1;35m╚";
    for (int i = 0; i < width_; i++) {
        for (int k = 0; k < cellW; k++) std::cout << "═";
    }
    std::cout << "╝\033[0m\n";
    
    if (useColors_) {
        std::cout << "\n\033[2mLegend: \033[1;32mS\033[0m\033[2m=Start  \033[1;31mG\033[0m\033[2m=Goal  \033[90m█\033[0m\033[2m=Wall  \033[1;33m●\033[0m\033[2m=BFS  \033[1;35m○\033[0m\033[2m=Dijkstra  \033[36m≈\033[0m\033[2m=Water  \033[33m▲\033[0m\033[2m=Mountain\033[0m\n";
    } else {
        std::cout << "\nLegend: S=Start, G=Goal, #=Wall, *=BFS, +=Dijkstra, ~=Water, ^=Mountain\n";
    }
}

bool Renderer::saveToFile(const char* filename) const {
    std::ofstream file(filename);
    if (!file.is_open() || displayGrid_.empty() || width_ == 0 || height_ == 0) {
        return false;
    }
    
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            file << displayGrid_[static_cast<std::size_t>(index(x, y))];
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

void Renderer::clear() {
    displayGrid_.clear();
    width_ = 0;
    height_ = 0;
}
