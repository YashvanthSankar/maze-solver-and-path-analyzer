#include "GameMode.h"
#include <ncurses.h>
#include <locale.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace {
    
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

GameMode::GameMode(Maze& maze, Renderer& renderer, CLIUtils& cli)
    : renderer_(renderer),
      cli_(cli),
      maze_(&maze),
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
    timeout(60); 

    if (has_colors() && renderer_.isColorModeEnabled()) {
        start_color();
        use_default_colors();

        const auto& p = renderer_.getActivePalette();
        init_pair(PAIR_PLAYER, Renderer::ansiToNcursesColor(p.cellStartColor), -1);
        init_pair(PAIR_GOAL, Renderer::ansiToNcursesColor(p.cellGoalColor), -1);
        init_pair(PAIR_WALL, Renderer::ansiToNcursesColor(p.cellWallColor), -1);
        init_pair(PAIR_PATH, Renderer::ansiToNcursesColor(p.cellFloorColor), -1);
        init_pair(PAIR_WATER, Renderer::ansiToNcursesColor(p.cellWaterColor), -1);
        init_pair(PAIR_MOUNTAIN, Renderer::ansiToNcursesColor(p.cellMountainColor), -1);
        init_pair(PAIR_FRAME, Renderer::ansiToNcursesColor(p.frameColor), -1);
        init_pair(PAIR_HUD_LABEL, Renderer::ansiToNcursesColor(p.statLabelColor), -1);
        init_pair(PAIR_HUD_VALUE, Renderer::ansiToNcursesColor(p.statValueColor), -1);
        init_pair(PAIR_VICTORY_BG, Renderer::ansiToNcursesColor(p.headerPrimary), -1);
        init_pair(PAIR_VICTORY_FG, Renderer::ansiToNcursesColor(p.headerSecondary), -1);
    }
}

void GameMode::cleanupNcurses() {
    endwin();
    gameRunning_ = false;
}

void GameMode::startGame() {
    playerPos_ = maze_->getStart();
    goalPos_ = maze_->getGoal();
    moves_ = 0;
    startTime_ = std::chrono::steady_clock::now();
    gameWon_ = false;
    gameRunning_ = true;
    frameCounter_ = 0;
    statusLog_.clear();

    initNcurses();
    
    
    clear();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    
    std::string startMsg = "GAME STARTED";
    std::string goodLuck = "Good luck!";
    std::string controls = "Use Arrow Keys or WASD to move | Press Q to quit";
    
    attron(COLOR_PAIR(PAIR_HUD_VALUE) | A_BOLD);
    mvwprintw(stdscr, rows / 2 - 2, (cols - startMsg.length()) / 2, "%s", startMsg.c_str());
    attroff(COLOR_PAIR(PAIR_HUD_VALUE) | A_BOLD);
    
    attron(COLOR_PAIR(PAIR_GOAL));
    mvwprintw(stdscr, rows / 2, (cols - goodLuck.length()) / 2, "%s", goodLuck.c_str());
    attroff(COLOR_PAIR(PAIR_GOAL));
    
    attron(COLOR_PAIR(PAIR_HUD_LABEL));
    mvwprintw(stdscr, rows / 2 + 2, (cols - controls.length()) / 2, "%s", controls.c_str());
    attroff(COLOR_PAIR(PAIR_HUD_LABEL));
    
    refresh();
    napms(2000); 

    while (gameRunning_) {
        handleInput();
        updateDisplay();
        pruneLog();
        frameCounter_++;

        if (playerPos_ == goalPos_ && !gameWon_) {
            gameWon_ = true;
            updateDisplay(); 
            showVictoryScreen();
            gameRunning_ = false;
            break; 
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
        case 'q': case 'Q': case 27: 
            gameRunning_ = false;
            logStatus("Exiting game...");
            return;
        case ERR: 
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
        "##    ##  #######  ##     ##    ##      ##  #######  ##    ## ",
        " ##  ##  ##     ## ##     ##    ##  ##  ## ##     ## ###   ## ",
        "  ####   ##     ## ##     ##    ##  ##  ## ##     ## ####  ## ",
        "   ##    ##     ## ##     ##    ##  ##  ## ##     ## ## ## ## ",
        "   ##     #######   #######      ###  ###   #######  ##  #### "
    };

    const std::vector<std::string> stars = {
        "          * * * * * * * * *          ",
        "          Congratulations!           ",
        "          * * * * * * * * *          "
    };

    const short SWEEP_COLOR_PAIR = 12;
    const short STAR_COLOR_PAIR = 13;
    if (has_colors()) {
        init_pair(SWEEP_COLOR_PAIR, COLOR_YELLOW, -1);
        init_pair(STAR_COLOR_PAIR, COLOR_MAGENTA, -1);
    }

    int artHeight = art.size();

    int artY = (rows - artHeight - 12) / 2;

    
    for (int phase = 0; phase <= 30; ++phase) {
        attron(COLOR_PAIR(PAIR_VICTORY_BG));
        for (int r = 0; r < rows; ++r) {
            mvwhline(stdscr, r, 0, ' ', cols);
        }
        attroff(COLOR_PAIR(PAIR_VICTORY_BG));

        
        for (size_t i = 0; i < art.size(); ++i) {
            int wave_offset = (int)(1.5 * sin((phase * 0.2) + (i * 0.5)));
            int xPos = (cols - art[i].length()) / 2 + wave_offset;
            
            if (phase > (int)(i * 3)) {
                attron(COLOR_PAIR(PAIR_VICTORY_FG) | A_BOLD);
                mvwprintw(stdscr, artY + i, xPos, "%s", art[i].c_str());
                attroff(COLOR_PAIR(PAIR_VICTORY_FG) | A_BOLD);
            }
        }

        
        if (phase > 15) {
            for (size_t i = 0; i < stars.size(); ++i) {
                bool twinkle = ((phase + i) % 3 < 2);
                int centerX = (cols - stars[i].length()) / 2;
                
                if (i == 1) {
                    
                    attron(COLOR_PAIR(SWEEP_COLOR_PAIR) | A_BOLD);
                } else if (twinkle) {
                    attron(COLOR_PAIR(STAR_COLOR_PAIR) | A_BOLD);
                } else {
                    attron(COLOR_PAIR(SWEEP_COLOR_PAIR));
                }
                mvwprintw(stdscr, artY + art.size() + 2 + i, centerX, "%s", stars[i].c_str());
                attroff(COLOR_PAIR(STAR_COLOR_PAIR) | A_BOLD);
                attroff(COLOR_PAIR(SWEEP_COLOR_PAIR) | A_BOLD);
                attroff(COLOR_PAIR(SWEEP_COLOR_PAIR));
            }
        }

        refresh();
        napms(60);
    }

    
    attron(COLOR_PAIR(PAIR_VICTORY_BG));
    for (int r = 0; r < rows; ++r) mvwhline(stdscr, r, 0, ' ', cols);
    attroff(COLOR_PAIR(PAIR_VICTORY_BG));

    attron(COLOR_PAIR(PAIR_VICTORY_FG) | A_BOLD);
    for (size_t i = 0; i < art.size(); ++i) {
        int centerX = (cols - art[i].length()) / 2;
        mvwprintw(stdscr, artY + i, centerX, "%s", art[i].c_str());
    }
    attroff(COLOR_PAIR(PAIR_VICTORY_FG) | A_BOLD);

    
    for (size_t i = 0; i < stars.size(); ++i) {
        int centerX = (cols - stars[i].length()) / 2;
        if (i == 1) {
            attron(COLOR_PAIR(SWEEP_COLOR_PAIR) | A_BOLD);
        } else {
            attron(COLOR_PAIR(STAR_COLOR_PAIR) | A_BOLD);
        }
        mvwprintw(stdscr, artY + art.size() + 2 + i, centerX, "%s", stars[i].c_str());
        if (i == 1) {
            attroff(COLOR_PAIR(SWEEP_COLOR_PAIR) | A_BOLD);
        } else {
            attroff(COLOR_PAIR(STAR_COLOR_PAIR) | A_BOLD);
        }
    }

    long elapsed = getElapsedTime();
    std::string movesStr = "Moves: " + std::to_string(moves_);
    std::ostringstream timeStream;
    timeStream << "Time: " << std::setfill('0') << std::setw(2) << (elapsed / 60) << ":" << std::setw(2) << (elapsed % 60);
    std::string timeStr = timeStream.str();

    attron(COLOR_PAIR(SWEEP_COLOR_PAIR) | A_BOLD);
    mvwprintw(stdscr, artY + art.size() + stars.size() + 4, (cols - movesStr.length()) / 2, "%s", movesStr.c_str());
    mvwprintw(stdscr, artY + art.size() + stars.size() + 5, (cols - timeStr.length()) / 2, "%s", timeStr.c_str());
    attroff(COLOR_PAIR(SWEEP_COLOR_PAIR) | A_BOLD);

    std::string pressKey = "[ Press any key to continue ]";
    attron(COLOR_PAIR(PAIR_VICTORY_FG));
    mvwprintw(stdscr, rows - 3, (cols - pressKey.length()) / 2, "%s", pressKey.c_str());
    attroff(COLOR_PAIR(PAIR_VICTORY_FG));

    refresh();
    timeout(-1);
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
        
        int msgLen = entry.first.length();
        int centerX = startX + (width - msgLen) / 2;
        if (centerX < startX) centerX = startX;
        
        attron(COLOR_PAIR(PAIR_HUD_VALUE));
        mvwprintw(stdscr, y++, centerX, "%.*s", width, entry.first.c_str());
        attroff(COLOR_PAIR(PAIR_HUD_VALUE));
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
