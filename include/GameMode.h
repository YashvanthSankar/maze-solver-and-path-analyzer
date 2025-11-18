#ifndef GAMEMODE_H
#define GAMEMODE_H

#include "Maze.h"
#include "Point.h"
#include "Renderer.h"
#include "CLIUtils.h"
#include <chrono>
#include <string>
#include <vector>

class GameMode {
public:
    GameMode(Maze& maze, Renderer& renderer, CLIUtils& cli);
    ~GameMode();

    void startGame();

    bool hasWon() const;
    int getMoves() const;

private:
    // --- Core Components ---
    Renderer& renderer_;
    CLIUtils& cli_;
    Maze* maze_;

    // --- Game State ---
    Point playerPos_;
    Point goalPos_;
    int moves_;
    std::chrono::steady_clock::time_point startTime_;
    bool gameWon_;
    bool gameRunning_;

    // --- Animation & UI State ---
    int frameCounter_;
    std::vector<std::pair<std::string, int>> statusLog_;

    // --- Core Methods ---
    void initNcurses();
    void cleanupNcurses();
    void handleInput();
    void updateDisplay();
    bool isValidMove(const Point& newPos) const;

    // --- Drawing Methods ---
    void drawLayout();
    void drawMaze();
    void drawHud();
    void showVictoryScreen();

    // --- Helpers ---
    void logStatus(const std::string& message, int lifetimeFrames = 120);
    void pruneLog();
    void printStatusLog(int startY, int startX, int width);
    long getElapsedTime() const;
};

#endif // GAMEMODE_H
