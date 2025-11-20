#ifndef RENDERER_H
#define RENDERER_H

#include "Maze.h"
#include "Path.h"
#include "CLIUtils.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>


class Renderer {
public:
    struct ThemePalette {
        std::string name;
        std::string headerPrimary;
        std::string headerSecondary;
        std::string frameColor;
        std::string legendLabelColor;
        std::string legendValueColor;
        std::string statLabelColor;
        std::string statValueColor;
        std::string cellWallColor;
        std::string cellFloorColor;
        std::string cellPathColor;
        std::string cellAltPathColor;
        std::string cellStartColor;
        std::string cellGoalColor;
        std::string cellWaterColor;
        std::string cellMountainColor;
    };

private:
    std::vector<char> displayGrid_;   
    int width_;
    int height_;
    CLIUtils cli_;
    bool useColors_;

    ThemePalette activePalette_;
    std::size_t activeThemeIndex_;

    void copyMazeToDisplay(const Maze& maze);
    void overlayPath(const Path& path);
    void overlayMultiplePaths(const std::vector<const Path*>& paths);
    const char* getCellColor(char cell) const;
    int index(int x, int y) const;
    void drawGrid() const;
    void printHeader(const Maze& maze, const std::vector<std::string>& pathLabels = {}) const;
    void printLegend() const;
    void printPathStats(const Maze& maze, const Path& path, const std::string& label) const;
    void printComparisonStats(const Maze& maze, const Path& path1, const std::string& label1, const Path& path2, const std::string& label2) const;
    void drawLegendEntry(const std::string& label, char cell, int pad, int legendWidth) const;
    std::string getCellGlyph(char cell) const;
    const std::string& cellColor(char cell) const;
    void printCell(char cell) const;
    static const ThemePalette& paletteForTheme(std::size_t index);

public:
    enum class ThemeId { NeonMatrix = 0, EmberGlow, ArcticAurora, Monochrome };

    Renderer();

    void render(const Maze& maze);
    void render(const Maze& maze, const Path& path, const std::string& label = "Path");
    void renderComparison(const Maze& maze, const Path& path1, const Path& path2,
                          const std::string& label1 = "Path A", const std::string& label2 = "Path B");
    void renderAnimated(const Maze& maze, const Path& path, int delayMs = 100);
    void setColorMode(bool enabled);
    void setTheme(ThemeId theme);
    void setThemeByIndex(std::size_t index);
    ThemeId getTheme() const;
    std::string getThemeName() const;
    bool isColorModeEnabled() const;
    const ThemePalette& getActivePalette() const;
    static std::vector<std::string> listAvailableThemes();
    static const ThemePalette& getThemePalette(std::size_t index);
    static short ansiToNcursesColor(const std::string& ansiColor);

    bool saveToFile(const char* filename) const;
    void clear();
};

#endif
