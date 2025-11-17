#ifndef RENDERER_H
#define RENDERER_H

#include "Maze.h"
#include "Path.h"
#include "CLIUtils.h"
#include <iostream>
#include <vector>

// Renderer class for visualization (demonstrating Encapsulation)
class Renderer {
private:
    std::vector<char> displayGrid_;   // Encapsulated display buffer (row-major)
    int width_;
    int height_;
    CLIUtils cli_;
    bool useColors_;

    void copyMazeToDisplay(const Maze& maze);
    void overlayPath(const Path& path);
    void overlayMultiplePaths(const Path* paths, int pathCount);
    const char* getCellColor(char cell) const;
    int index(int x, int y) const;

public:
    Renderer();

    void render(const Maze& maze);
    void render(const Maze& maze, const Path& path);
    void renderComparison(const Maze& maze, const Path& path1, const Path& path2);
    void renderAnimated(const Maze& maze, const Path& path, int delayMs = 100);
    void setColorMode(bool enabled);

    bool saveToFile(const char* filename) const;
    void clear();
};

#endif
