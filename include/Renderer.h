#ifndef RENDERER_H
#define RENDERER_H

#include "Maze.h"
#include "Path.h"
#include "CLIUtils.h"
#include <iostream>

// Renderer class for visualization (demonstrating Encapsulation)
class Renderer {
private:
    char** displayGrid_;   // Encapsulated display buffer
    int width_;
    int height_;
    CLIUtils cli_;
    bool useColors_;
    
    // Private helper methods (abstraction)
    void allocateDisplayGrid();
    void deallocateDisplayGrid();
    void copyMazeToDisplay(const Maze& maze);
    void overlayPath(const Path& path);
    void overlayMultiplePaths(const Path* paths, int pathCount);
    const char* getCellColor(char cell) const;

public:
    Renderer();
    ~Renderer();
    
    // Render methods
    void render(const Maze& maze);
    void render(const Maze& maze, const Path& path);
    void renderComparison(const Maze& maze, const Path& path1, const Path& path2);
    void renderAnimated(const Maze& maze, const Path& path, int delayMs = 100);
    void setColorMode(bool enabled);
    
    // Save to file
    bool saveToFile(const char* filename) const;
    
    // Clear display
    void clear();
};

#endif
