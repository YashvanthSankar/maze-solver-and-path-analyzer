#include "GameMode.h"
#include <ncurses.h>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <locale.h>

GameMode::GameMode() 
    : maze_(nullptr), playerPos_(0, 0), goalPos_(0, 0), 
      moves_(0), startTime_(0), gameWon_(false), gameRunning_(false) {
}

GameMode::~GameMode() {
    if (gameRunning_) {
        cleanupNcurses();
    }
}

void GameMode::initNcurses() {
    setlocale(LC_ALL, "");  // Enable UTF-8 support
    initscr();              // Initialize ncurses
    cbreak();               // Disable line buffering
    noecho();               // Don't echo input
    keypad(stdscr, TRUE);   // Enable arrow keys
    curs_set(0);            // Hide cursor
    
    // Enable colors
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);   // Player
        init_pair(2, COLOR_RED, COLOR_BLACK);     // Goal
        init_pair(3, COLOR_WHITE, COLOR_BLACK);   // Walls
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // Path/Empty
        init_pair(5, COLOR_CYAN, COLOR_BLACK);    // Water
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK); // Mountains
        init_pair(7, COLOR_BLACK, COLOR_GREEN);   // Victory
    }
    
    timeout(100); // Non-blocking input with 100ms timeout
}

void GameMode::cleanupNcurses() {
    endwin();
    gameRunning_ = false;
}

void GameMode::startGame(Maze& maze) {
    maze_ = &maze;
    playerPos_ = maze_->getStart();
    goalPos_ = maze_->getGoal();
    moves_ = 0;
    startTime_ = time(nullptr);
    gameWon_ = false;
    gameRunning_ = true;
    
    initNcurses();
    updateDisplay();
    
    // Game loop
    while (gameRunning_) {
        handleInput();
        
        // Check win condition
        if (playerPos_ == goalPos_ && !gameWon_) {
            gameWon_ = true;
            showVictoryScreen();
            break;
        }
        
        updateDisplay();
        napms(50); // Small delay for smooth rendering
    }
    
    cleanupNcurses();
}

void GameMode::handleInput() {
    int ch = getch();
    Point newPos = playerPos_;
    
    switch(ch) {
        case KEY_UP:
        case 'w':
        case 'W':
            newPos = playerPos_ + Point(0, -1);  // Operator overloading
            break;
        case KEY_DOWN:
        case 's':
        case 'S':
            newPos = playerPos_ + Point(0, 1);
            break;
        case KEY_LEFT:
        case 'a':
        case 'A':
            newPos = playerPos_ + Point(-1, 0);
            break;
        case KEY_RIGHT:
        case 'd':
        case 'D':
            newPos = playerPos_ + Point(1, 0);
            break;
        case 'q':
        case 'Q':
        case 27: // ESC
            gameRunning_ = false;
            return;
        default:
            return; // No movement
    }
    
    // Validate and update position
    if (isValidMove(newPos)) {
        playerPos_ = newPos;
        moves_++;
    }
}

bool GameMode::isValidMove(const Point& newPos) const {
    if (!maze_) return false;
    return maze_->isWalkable(newPos);
}

void GameMode::updateDisplay() {
    clear();
    drawMaze();
    drawStatusBar();
    drawInstructions();
    refresh();
}

void GameMode::drawMaze() {
    int height = maze_->getHeight();
    int width = maze_->getWidth();
    
    // Make cells wider for better visibility (each cell is printed cellW characters)
    const int cellW = 3;

    // Center the maze on screen (account for doubled cell width)
    int startY = 3;
    int startX = (COLS - (width * cellW) - 2) / 2;
    
    // Draw top border with beautiful UTF-8 box-drawing
    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(startY, startX, "╔");
    for (int i = 0; i < width; i++) {
        for (int k = 0; k < cellW; k++) addstr("═");
    }
    addstr("╗");
    attroff(COLOR_PAIR(3) | A_BOLD);
    
    // Draw maze content
    for (int y = 0; y < height; y++) {
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(startY + y + 1, startX, "║");
        attroff(COLOR_PAIR(3) | A_BOLD);
        
        for (int x = 0; x < width; x++) {
            Point currentPos(x, y);
            char cell = maze_->getCellAt(currentPos);
            
            // Player position (highest priority)
            if (currentPos == playerPos_) {
                attron(COLOR_PAIR(1) | A_BOLD);
                for (int k = 0; k < cellW; ++k) addstr("●");  // Filled circle for player (double width)
                attroff(COLOR_PAIR(1) | A_BOLD);
            }
            // Goal position
            else if (currentPos == goalPos_) {
                attron(COLOR_PAIR(2) | A_BOLD);
                for (int k = 0; k < cellW; ++k) addstr("★");  // Star for goal
                attroff(COLOR_PAIR(2) | A_BOLD);
            }
            // Terrain
            else {
                switch(cell) {
                    case '#':
                        attron(COLOR_PAIR(3));
                        for (int k = 0; k < cellW; ++k) addstr("█");  // Full block for walls
                        attroff(COLOR_PAIR(3));
                        break;
                    case '~':
                        attron(COLOR_PAIR(5));
                        for (int k = 0; k < cellW; ++k) addstr("≈");  // Water waves
                        attroff(COLOR_PAIR(5));
                        break;
                    case '^':
                        attron(COLOR_PAIR(6));
                        for (int k = 0; k < cellW; ++k) addstr("▲");  // Mountain triangle
                        attroff(COLOR_PAIR(6));
                        break;
                    case '.':
                    case 'S':
                        attron(COLOR_PAIR(4));
                        for (int k = 0; k < cellW; ++k) addstr("·");  // Middle dot for path
                        attroff(COLOR_PAIR(4));
                        break;
                    default:
                        for (int k = 0; k < cellW; ++k) addstr(" ");
                }
            }
        }
        
        attron(COLOR_PAIR(3) | A_BOLD);
        addstr("║");
        attroff(COLOR_PAIR(3) | A_BOLD);
    }
    
    // Draw bottom border (account for cell width)
    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(startY + height + 1, startX, "╚");
    for (int i = 0; i < width; i++) {
        for (int k = 0; k < cellW; k++) addstr("═");
    }
    addstr("╝");
    attroff(COLOR_PAIR(3) | A_BOLD);
}

void GameMode::drawStatusBar() {
    int statusY = 1;
    int elapsed = getElapsedTime();
    
    attron(A_BOLD | COLOR_PAIR(4));
    mvprintw(statusY, 2, "╔════════════════════════════════════════════════════════════════╗");
    mvprintw(statusY + 1, 2, "║");
    attroff(A_BOLD | COLOR_PAIR(4));
    
    // Game title
    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(statusY + 1, 16, "🎮  MAZE RUNNER - INTERACTIVE MODE  🎮");
    attroff(A_BOLD | COLOR_PAIR(2));
    
    attron(A_BOLD | COLOR_PAIR(4));
    mvprintw(statusY + 1, 66, "║");
    mvprintw(statusY + 2, 2, "╚════════════════════════════════════════════════════════════════╝");
    attroff(A_BOLD | COLOR_PAIR(4));
    
    // Stats
    int height = maze_->getHeight();
    int statsY = 3 + height + 3;
    
    attron(COLOR_PAIR(4));
    mvprintw(statsY, 2, "┌─ Statistics ─┐");
    mvprintw(statsY + 1, 2, "│");
    attroff(COLOR_PAIR(4));
    
    attron(A_BOLD | COLOR_PAIR(1));
    addstr(" Moves: ");
    attroff(A_BOLD | COLOR_PAIR(1));
    printw("%d", moves_);
    
    attron(COLOR_PAIR(4));
    addstr("   ");
    attroff(COLOR_PAIR(4));
    
    attron(A_BOLD | COLOR_PAIR(5));
    addstr("Time: ");
    attroff(A_BOLD | COLOR_PAIR(5));
    printw("%02d:%02d", elapsed / 60, elapsed % 60);
    
    attron(COLOR_PAIR(4));
    mvprintw(statsY + 1, 32, "│");
    mvprintw(statsY + 2, 2, "└───────────────┘");
    attroff(COLOR_PAIR(4));
}

void GameMode::drawInstructions() {
    int height = maze_->getHeight();
    int instrY = 3 + height + 6;
    
    attron(COLOR_PAIR(6));
    mvprintw(instrY, 2, "Controls:");
    attroff(COLOR_PAIR(6));
    
    mvprintw(instrY + 1, 4, "↑/W: Move Up    ↓/S: Move Down");
    mvprintw(instrY + 2, 4, "←/A: Move Left  →/D: Move Right");
    mvprintw(instrY + 3, 4, "Q/ESC: Quit Game");
    
    mvprintw(instrY + 5, 2, "Objective: Navigate ");
    attron(COLOR_PAIR(1) | A_BOLD);
    addstr("●");
    attroff(COLOR_PAIR(1) | A_BOLD);
    addstr(" to the goal ");
    attron(COLOR_PAIR(2) | A_BOLD);
    addstr("★");
    attroff(COLOR_PAIR(2) | A_BOLD);
}

void GameMode::showVictoryScreen() {
    clear();
    
    int centerY = LINES / 2 - 5;
    int centerX = COLS / 2;
    
    // Victory banner with beautiful UTF-8
    attron(COLOR_PAIR(7) | A_BOLD | A_BLINK);
    mvprintw(centerY, centerX - 20, "╔══════════════════════════════════════╗");
    mvprintw(centerY + 1, centerX - 20, "║                                      ║");
    mvprintw(centerY + 2, centerX - 20, "║");
    attroff(COLOR_PAIR(7) | A_BOLD | A_BLINK);
    
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(centerY + 2, centerX - 13, "🏆 CONGRATULATIONS! 🏆");
    attroff(COLOR_PAIR(2) | A_BOLD);
    
    attron(COLOR_PAIR(7) | A_BOLD | A_BLINK);
    mvprintw(centerY + 2, centerX + 18, "║");
    mvprintw(centerY + 3, centerX - 20, "║                                      ║");
    mvprintw(centerY + 4, centerX - 20, "╚══════════════════════════════════════╝");
    attroff(COLOR_PAIR(7) | A_BOLD | A_BLINK);
    
    // Stats
    int elapsed = getElapsedTime();
    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(centerY + 6, centerX - 10, "You solved the maze!");
    attroff(COLOR_PAIR(4) | A_BOLD);
    
    attron(COLOR_PAIR(1));
    mvprintw(centerY + 8, centerX - 8, "Total Moves: ");
    attroff(COLOR_PAIR(1));
    attron(A_BOLD);
    printw("%d", moves_);
    attroff(A_BOLD);
    
    attron(COLOR_PAIR(5));
    mvprintw(centerY + 9, centerX - 8, "Time Taken:  ");
    attroff(COLOR_PAIR(5));
    attron(A_BOLD);
    printw("%02d:%02d", elapsed / 60, elapsed % 60);
    attroff(A_BOLD);
    
    // Calculate score (lower is better)
    int score = 10000 - (moves_ * 10) - elapsed;
    if (score < 0) score = 0;
    
    attron(COLOR_PAIR(6));
    mvprintw(centerY + 10, centerX - 8, "Final Score: ");
    attroff(COLOR_PAIR(6));
    attron(A_BOLD);
    printw("%d", score);
    attroff(A_BOLD);
    
    attron(COLOR_PAIR(3));
    mvprintw(centerY + 13, centerX - 15, "Press any key to return to menu...");
    attroff(COLOR_PAIR(3));
    
    refresh();
    timeout(-1); // Blocking input
    getch();
}

int GameMode::getElapsedTime() const {
    return static_cast<int>(time(nullptr) - startTime_);
}

bool GameMode::isGameRunning() const {
    return gameRunning_;
}

int GameMode::getMoves() const {
    return moves_;
}

bool GameMode::hasWon() const {
    return gameWon_;
}
