#include "GameMode.h"
#include <ncurses.h>
#include <locale.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace {
    // Define color pairs for ncurses
    constexpr short PAIR_PLAYER = 1;
    constexpr short PAIR_GOAL = 2;
    constexpr short PAIR_WALL = 3;
    constexpr short PAIR_PATH = 4;
    constexpr short PAIR_WATER = 5;
    constexpr short PAIR_MOUNTAIN = 6;
    constexpr short PAIR_FRAME = 7;
    constexpr short PAIR_HUD_LABEL = 8;
    constexpr short PAIR_HUD_VALUE = 9;
    constexpr short PAIR_VICTORY_BG = 10;
    constexpr short PAIR_VICTORY_FG = 11;
}

GameMode::GameMode(Renderer& renderer, CLIUtils& cli)
    : renderer_(renderer),
      cli_(cli),
      maze_(nullptr),
      playerPos_(0, 0),
      goalPos_(0, 0),
      moves_(0),
      startTime_(),
      gameWon_(false),
      gameRunning_(false),
      frameCounter_(0) {}

GameMode::~GameMode() {
    if (gameRunning_) {
        cleanupNcurses();
    }
}

void GameMode::initNcurses() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(60); // ~16 FPS update rate

    if (has_colors() && renderer_.isColorModeEnabled()) {
        start_color();
        use_default_colors();

        // Initialize a conservative set of color pairs for ncurses.
        // Renderer palettes are ANSI/truecolor strings; mapping those precisely
        // to ncurses colors is out of scope here, so use reasonable defaults
        // that work across terminals.
        init_pair(PAIR_PLAYER, COLOR_YELLOW, -1);
        init_pair(PAIR_GOAL, COLOR_MAGENTA, -1);
        init_pair(PAIR_WALL, COLOR_BLUE, -1);
        init_pair(PAIR_PATH, COLOR_WHITE, -1);
        init_pair(PAIR_WATER, COLOR_CYAN, -1);
        init_pair(PAIR_MOUNTAIN, COLOR_RED, -1);
        init_pair(PAIR_FRAME, COLOR_GREEN, -1);
        init_pair(PAIR_HUD_LABEL, COLOR_WHITE, -1);
        init_pair(PAIR_HUD_VALUE, COLOR_CYAN, -1);
        init_pair(PAIR_VICTORY_BG, COLOR_GREEN, -1);
        init_pair(PAIR_VICTORY_FG, COLOR_BLACK, -1);
    }
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
    startTime_ = std::chrono::steady_clock::now();
    gameWon_ = false;
    gameRunning_ = true;
    frameCounter_ = 0;
    statusLog_.clear();

    initNcurses();
    logStatus("Game started. Good luck!");

    while (gameRunning_) {
        handleInput();
        updateDisplay();
        pruneLog();
        frameCounter_++;

        if (playerPos_ == goalPos_ && !gameWon_) {
            gameWon_ = true;
            showVictoryScreen();
            gameRunning_ = false;
        }
    }

    cleanupNcurses();
}

void GameMode::handleInput() {
    int ch = getch();
    Point newPos = playerPos_;

    switch (ch) {
        case KEY_UP: case 'w': case 'W':
            newPos = playerPos_ + Point(0, -1);
            break;
        case KEY_DOWN: case 's': case 'S':
            newPos = playerPos_ + Point(0, 1);
            break;
        case KEY_LEFT: case 'a': case 'A':
            newPos = playerPos_ + Point(-1, 0);
            break;
        case KEY_RIGHT: case 'd': case 'D':
            newPos = playerPos_ + Point(1, 0);
            break;
        case 'q': case 'Q': case 27: // ESC
            gameRunning_ = false;
            logStatus("Exiting game...");
            return;
        case ERR: // No input
            return;
        default:
            logStatus("Unknown key pressed.");
            return;
    }

    if (isValidMove(newPos)) {
        playerPos_ = newPos;
        moves_++;
        logStatus("Moved to (" + std::to_string(newPos.getX()) + ", " + std::to_string(newPos.getY()) + ")");
    } else {
        logStatus("Blocked! Cannot move there.");
    }
}

bool GameMode::isValidMove(const Point& newPos) const {
    return maze_ && maze_->isWalkable(newPos);
}

void GameMode::updateDisplay() {
    clear();
    drawLayout();
    drawMaze();
    drawHud();
    refresh();
}

void GameMode::drawLayout() {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    attron(COLOR_PAIR(PAIR_FRAME));
    box(stdscr, 0, 0);
    mvwhline(stdscr, 2, 1, ACS_HLINE, cols - 2);
    mvwhline(stdscr, rows - 3, 1, ACS_HLINE, cols - 2);
    mvwprintw(stdscr, 1, (cols - 14) / 2, "MAZE ADVENTURE");
    attroff(COLOR_PAIR(PAIR_FRAME));
}

void GameMode::drawMaze() {
    const int mazeHeight = maze_->getHeight();
    const int mazeWidth = maze_->getWidth();
    const int cellDisplayWidth = 2;

    const int availableHeight = LINES - 6;
    const int availableWidth = COLS - 4;

    const int mazeRenderHeight = mazeHeight;
    const int mazeRenderWidth = mazeWidth * cellDisplayWidth;

    const int startY = 3 + (availableHeight - mazeRenderHeight) / 2;
    const int startX = 2 + (availableWidth - mazeRenderWidth) / 2;

    for (int y = 0; y < mazeHeight; ++y) {
        for (int x = 0; x < mazeWidth; ++x) {
            Point currentPos(x, y);
            char cell = maze_->getCellAt(currentPos);
            int pair = PAIR_PATH;
            std::string glyph = "  ";

            if (currentPos == playerPos_) {
                pair = PAIR_PLAYER;
                glyph = (frameCounter_ % 20 < 10) ? "▶ " : "▷ ";
            } else if (currentPos == goalPos_) {
                pair = PAIR_GOAL;
                glyph = (frameCounter_ % 30 < 15) ? "★ " : "☆ ";
            } else {
                switch (cell) {
                    case '#': pair = PAIR_WALL; glyph = "██"; break;
                    case '~': pair = PAIR_WATER; glyph = "≈≈"; break;
                    case '^': pair = PAIR_MOUNTAIN; glyph = "▲▲"; break;
                    case '.':
                    case 'S':
                    case 'G':
                        pair = PAIR_PATH; glyph = "  "; break;
                    default:
                        glyph = "??"; break;
                }
            }
            attron(COLOR_PAIR(pair));
            mvwprintw(stdscr, startY + y, startX + x * cellDisplayWidth, "%s", glyph.c_str());
            attroff(COLOR_PAIR(pair));
        }
    }
}

void GameMode::drawHud() {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    long elapsed = getElapsedTime();
    std::ostringstream timeStream;
    timeStream << std::setfill('0') << std::setw(2) << (elapsed / 60) << ":" << std::setw(2) << (elapsed % 60);

    std::string movesText = "Moves: " + std::to_string(moves_);
    std::string timeText = "Time: " + timeStream.str();
    std::string themeText = "Theme: " + renderer_.getThemeName();

    attron(COLOR_PAIR(PAIR_HUD_LABEL));
    mvwprintw(stdscr, rows - 2, 3, "%s", movesText.c_str());
    mvwprintw(stdscr, rows - 2, (cols - timeText.length()) / 2, "%s", timeText.c_str());
    mvwprintw(stdscr, rows - 2, cols - 3 - themeText.length(), "%s", themeText.c_str());
    attroff(COLOR_PAIR(PAIR_HUD_LABEL));

    printStatusLog(3, 3, cols - 6);
}

void GameMode::showVictoryScreen() {
    clear();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    const std::vector<std::string> art = {
        " __   __           __        __            _ ",
        " \\ \\ / /__  _   _  \\ \\      / /__  _ __ __| |",
        "  \\ V / _ \\| | | |  \\ \\ /\\ / / _ \\| '__/ _` |",
        "   | | (_) | |_| |   \\ V  V / (_) | | | (_| |",
        "   |_|\\___/ \\__,_|    \\_/\\_/ \\___/|_|  \\__,_|"
    };

    attron(COLOR_PAIR(PAIR_VICTORY_BG));
    for (int r = 0; r < rows; ++r) {
        mvwhline(stdscr, r, 0, ' ', cols);
    }
    attroff(COLOR_PAIR(PAIR_VICTORY_BG));

    int artY = (rows - art.size() - 5) / 2;
    attron(COLOR_PAIR(PAIR_VICTORY_FG) | A_BOLD);
    for (size_t i = 0; i < art.size(); ++i) {
        mvwprintw(stdscr, artY + i, (cols - art[i].length()) / 2, "%s", art[i].c_str());
    }
    attroff(COLOR_PAIR(PAIR_VICTORY_FG) | A_BOLD);

    long elapsed = getElapsedTime();
    std::string movesStr = "Moves: " + std::to_string(moves_);
    std::ostringstream timeStream;
    timeStream << "Time: " << std::setfill('0') << std::setw(2) << (elapsed / 60) << ":" << std::setw(2) << (elapsed % 60);
    std::string timeStr = timeStream.str();

    mvwprintw(stdscr, artY + art.size() + 2, (cols - movesStr.length()) / 2, "%s", movesStr.c_str());
    mvwprintw(stdscr, artY + art.size() + 3, (cols - timeStr.length()) / 2, "%s", timeStr.c_str());
    mvwprintw(stdscr, rows - 3, (cols - 25) / 2, "Press any key to continue");

    refresh();
    timeout(-1); // Wait for key press
    getch();
}

void GameMode::logStatus(const std::string& message, int lifetimeFrames) {
    if (statusLog_.size() > 5) {
        statusLog_.erase(statusLog_.begin());
    }
    statusLog_.emplace_back(message, lifetimeFrames);
}

void GameMode::pruneLog() {
    statusLog_.erase(std::remove_if(statusLog_.begin(), statusLog_.end(),
        [](std::pair<std::string, int>& entry) {
            return --entry.second <= 0;
        }), statusLog_.end());
}

void GameMode::printStatusLog(int startY, int startX, int width) {
    int y = startY;
    for (const auto& entry : statusLog_) {
        mvwprintw(stdscr, y++, startX, "%.*s", width, entry.first.c_str());
    }
}

long GameMode::getElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - startTime_).count();
}

bool GameMode::hasWon() const {
    return gameWon_;
}

int GameMode::getMoves() const {
    return moves_;
}
