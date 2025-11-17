#ifndef GAMEMODE_H
#define GAMEMODE_H

#include "Maze.h"
#include "Point.h"
#include <ctime>
#include <string>

/**
 * @class GameMode
 * @brief Interactive game mode where player navigates maze using arrow keys
 * 
 * Demonstrates:
 * - Encapsulation: Private game state (position, moves, time)
 * - Abstraction: Simple interface to complex ncurses interaction
 * - Operator Overloading: Uses Point operators for movement
 */
class GameMode {
public:
    enum class GameTheme { NeonMatrix = 0, EmberGlow, ArcticAurora, Monochrome };

private:
    Maze* maze_;
    Point playerPos_;
    Point goalPos_;
    int moves_;
    time_t startTime_;
    bool gameWon_;
    bool gameRunning_;
    GameTheme theme_;
    
    // Encapsulated helper methods
    void initNcurses();
    void cleanupNcurses();
    bool isValidMove(const Point& newPos) const;
    void updateDisplay();
    void drawMaze();
    void drawStatusBar();
    void drawInstructions();
    void showVictoryScreen();
    void configureColorPairs();
    int measureDisplayWidth(const std::string& text) const;
    
public:
    // Constructor and Destructor
    GameMode();
    ~GameMode();
    
    // Abstracted game interface
    void startGame(Maze& maze);
    void handleInput();
    bool isGameRunning() const;
    void setTheme(GameTheme theme);
    
    // Game state accessors
    int getMoves() const;
    int getElapsedTime() const;
    bool hasWon() const;
};

#endif // GAMEMODE_H
