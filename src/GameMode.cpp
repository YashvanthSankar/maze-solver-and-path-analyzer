#include "GameMode.h"
#include <ncurses.h>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <locale.h>
#include <vector>
#include <algorithm>

namespace {
    constexpr short PAIR_PLAYER = 1;
    constexpr short PAIR_GOAL = 2;
    constexpr short PAIR_WALL = 3;
    constexpr short PAIR_PATH = 4;
    constexpr short PAIR_WATER = 5;
    constexpr short PAIR_MOUNTAIN = 6;
    constexpr short PAIR_VICTORY = 7;
}

GameMode::GameMode() 
        : maze_(nullptr), playerPos_(0, 0), goalPos_(0, 0), 
            moves_(0), startTime_(0), gameWon_(false), gameRunning_(false),
            theme_(GameTheme::NeonMatrix) {
}

GameMode::~GameMode() {
    if (gameRunning_) {
        cleanupNcurses();
    }
}

void GameMode::setTheme(GameTheme theme) {
    theme_ = theme;
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
#ifdef NCURSES_VERSION
        use_default_colors();
#endif
        configureColorPairs();
        attrset(COLOR_PAIR(PAIR_PATH));
    }
    
    timeout(100); // Non-blocking input with 100ms timeout
}

void GameMode::cleanupNcurses() {
    endwin();
    gameRunning_ = false;
}

void GameMode::configureColorPairs() {
    if (!has_colors()) {
        return;
    }

    struct PairDef {
        short fg;
        short bg;
    };

    PairDef player {COLOR_CYAN, COLOR_BLACK};
    PairDef goal {COLOR_MAGENTA, COLOR_BLACK};
    PairDef wall {COLOR_GREEN, COLOR_BLACK};
    PairDef path {COLOR_WHITE, COLOR_BLACK};
    PairDef water {COLOR_CYAN, COLOR_BLACK};
    PairDef mountain {COLOR_BLUE, COLOR_BLACK};
    PairDef victory {COLOR_BLACK, COLOR_GREEN};

    switch (theme_) {
        case GameTheme::EmberGlow:
            player = {COLOR_YELLOW, COLOR_BLACK};
            goal = {COLOR_RED, COLOR_BLACK};
            wall = {COLOR_RED, COLOR_BLACK};
            path = {COLOR_YELLOW, COLOR_BLACK};
            water = {COLOR_CYAN, COLOR_BLACK};
            mountain = {COLOR_MAGENTA, COLOR_BLACK};
            victory = {COLOR_BLACK, COLOR_YELLOW};
            break;
        case GameTheme::ArcticAurora:
            player = {COLOR_CYAN, COLOR_BLACK};
            goal = {COLOR_WHITE, COLOR_BLACK};
            wall = {COLOR_BLUE, COLOR_BLACK};
            path = {COLOR_WHITE, COLOR_BLACK};
            water = {COLOR_CYAN, COLOR_BLACK};
            mountain = {COLOR_MAGENTA, COLOR_BLACK};
            victory = {COLOR_BLACK, COLOR_CYAN};
            break;
        case GameTheme::Monochrome:
            player = {COLOR_WHITE, COLOR_BLACK};
            goal = {COLOR_WHITE, COLOR_BLACK};
            wall = {COLOR_WHITE, COLOR_BLACK};
            path = {COLOR_WHITE, COLOR_BLACK};
            water = {COLOR_WHITE, COLOR_BLACK};
            mountain = {COLOR_WHITE, COLOR_BLACK};
            victory = {COLOR_BLACK, COLOR_WHITE};
            break;
        case GameTheme::NeonMatrix:
        default:
            break;
    }

    init_pair(PAIR_PLAYER, player.fg, player.bg);
    init_pair(PAIR_GOAL, goal.fg, goal.bg);
    init_pair(PAIR_WALL, wall.fg, wall.bg);
    init_pair(PAIR_PATH, path.fg, path.bg);
    init_pair(PAIR_WATER, water.fg, water.bg);
    init_pair(PAIR_MOUNTAIN, mountain.fg, mountain.bg);
    init_pair(PAIR_VICTORY, victory.fg, victory.bg);
}

int GameMode::measureDisplayWidth(const std::string& text) const {
    int width = 0;
    for (std::size_t i = 0; i < text.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);

        if (c == '\033') {
            std::size_t j = i + 1;
            if (j < text.size() && text[j] == '[') {
                ++j;
                while (j < text.size() && text[j] != 'm') {
                    ++j;
                }
                if (j < text.size()) {
                    i = j;
                }
            }
            continue;
        }

        if ((c & 0x80) != 0) {
            std::string utf8Char;
            utf8Char += text[i];
            int continuation = 0;
            if ((c & 0xE0) == 0xC0) continuation = 1;
            else if ((c & 0xF0) == 0xE0) continuation = 2;
            else if ((c & 0xF8) == 0xF0) continuation = 3;

            for (int j = 0; j < continuation && i + 1 < text.size(); ++j) {
                utf8Char += text[++i];
            }

            static const std::vector<std::string> wideChars = {
                "█", "≈", "▲", "●", "★",
                "╔", "╗", "╚", "╝", "╠", "╣", "║", "═"
            };

            if (std::find(wideChars.begin(), wideChars.end(), utf8Char) != wideChars.end()) {
                width += 2;
            } else {
                width += 1;
            }
        } else {
            width += 1;
        }
    }
    return width;
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
    int totalWidth = width * cellW + 2;
    int startX = std::max(0, (COLS - totalWidth) / 2);
    
    // Draw top border with beautiful UTF-8 box-drawing
    attron(COLOR_PAIR(PAIR_WALL) | A_BOLD);
    mvprintw(startY, startX, "╔");
    for (int i = 0; i < width; i++) {
        for (int k = 0; k < cellW; k++) addstr("═");
    }
    addstr("╗");
    attroff(COLOR_PAIR(PAIR_WALL) | A_BOLD);
    
    // Draw maze content
    for (int y = 0; y < height; y++) {
    attron(COLOR_PAIR(PAIR_WALL) | A_BOLD);
        mvprintw(startY + y + 1, startX, "║");
    attroff(COLOR_PAIR(PAIR_WALL) | A_BOLD);
        
        for (int x = 0; x < width; x++) {
            Point currentPos(x, y);
            char cell = maze_->getCellAt(currentPos);
            
            // Player position (highest priority)
            if (currentPos == playerPos_) {
                attron(COLOR_PAIR(PAIR_PLAYER) | A_BOLD);
                for (int k = 0; k < cellW; ++k) addstr("●");  // Filled circle for player (double width)
                attroff(COLOR_PAIR(PAIR_PLAYER) | A_BOLD);
            }
            // Goal position
            else if (currentPos == goalPos_) {
                attron(COLOR_PAIR(PAIR_GOAL) | A_BOLD);
                for (int k = 0; k < cellW; ++k) addstr("★");  // Star for goal
                attroff(COLOR_PAIR(PAIR_GOAL) | A_BOLD);
            }
            // Terrain
            else {
                switch(cell) {
                    case '#':
                        attron(COLOR_PAIR(PAIR_WALL));
                        for (int k = 0; k < cellW; ++k) addstr("█");  // Full block for walls
                        attroff(COLOR_PAIR(PAIR_WALL));
                        break;
                    case '~':
                        attron(COLOR_PAIR(PAIR_WATER));
                        for (int k = 0; k < cellW; ++k) addstr("≈");  // Water waves
                        attroff(COLOR_PAIR(PAIR_WATER));
                        break;
                    case '^':
                        attron(COLOR_PAIR(PAIR_MOUNTAIN));
                        for (int k = 0; k < cellW; ++k) addstr("▲");  // Mountain triangle
                        attroff(COLOR_PAIR(PAIR_MOUNTAIN));
                        break;
                    case '.':
                    case 'S':
                        attron(COLOR_PAIR(PAIR_PATH));
                        for (int k = 0; k < cellW; ++k) addstr("·");  // Middle dot for path
                        attroff(COLOR_PAIR(PAIR_PATH));
                        break;
                    default:
                        for (int k = 0; k < cellW; ++k) addstr(" ");
                }
            }
        }
        
        attron(COLOR_PAIR(PAIR_WALL) | A_BOLD);
        addstr("║");
        attroff(COLOR_PAIR(PAIR_WALL) | A_BOLD);
    }
    
    // Draw bottom border (account for cell width)
    attron(COLOR_PAIR(PAIR_WALL) | A_BOLD);
    mvprintw(startY + height + 1, startX, "╚");
    for (int i = 0; i < width; i++) {
        for (int k = 0; k < cellW; k++) addstr("═");
    }
    addstr("╝");
    attroff(COLOR_PAIR(PAIR_WALL) | A_BOLD);
}

void GameMode::drawStatusBar() {
    const int statusY = 1;
    const std::string title = "MAZE RUNNER - INTERACTIVE MODE";
    const int titleWidth = measureDisplayWidth(title);
    const int innerWidth = std::max(titleWidth + 8, 60);
    const int startX = std::max(0, (COLS - (innerWidth + 2)) / 2);

    attron(A_BOLD | COLOR_PAIR(PAIR_WALL));
    move(statusY, startX);
    addstr("╔");
    for (int i = 0; i < innerWidth; ++i) addstr("═");
    addstr("╗");
    attroff(A_BOLD | COLOR_PAIR(PAIR_WALL));

    move(statusY + 1, startX);
    attron(A_BOLD | COLOR_PAIR(PAIR_WALL));
    addstr("║");
    attroff(A_BOLD | COLOR_PAIR(PAIR_WALL));

    const int leftPad = (innerWidth - titleWidth) / 2;
    const int rightPad = innerWidth - leftPad - titleWidth;

    attron(COLOR_PAIR(PAIR_PATH));
    for (int i = 0; i < leftPad; ++i) addch(' ');
    attroff(COLOR_PAIR(PAIR_PATH));

    attron(A_BOLD | COLOR_PAIR(PAIR_GOAL));
    addstr(title.c_str());
    attroff(A_BOLD | COLOR_PAIR(PAIR_GOAL));

    attron(COLOR_PAIR(PAIR_PATH));
    for (int i = 0; i < rightPad; ++i) addch(' ');
    attroff(COLOR_PAIR(PAIR_PATH));

    attron(A_BOLD | COLOR_PAIR(PAIR_WALL));
    addstr("║");
    attroff(A_BOLD | COLOR_PAIR(PAIR_WALL));

    move(statusY + 2, startX);
    attron(A_BOLD | COLOR_PAIR(PAIR_WALL));
    addstr("╚");
    for (int i = 0; i < innerWidth; ++i) addstr("═");
    addstr("╝");
    attroff(A_BOLD | COLOR_PAIR(PAIR_WALL));

    const int elapsed = getElapsedTime();
    std::ostringstream statsBuilder;
    statsBuilder << "Moves: " << moves_ << "   Time: "
                 << std::setfill('0') << std::setw(2) << (elapsed / 60)
                 << ":" << std::setw(2) << (elapsed % 60);
    const std::string statsPreview = statsBuilder.str();
    const int statsWidth = measureDisplayWidth(statsPreview);
    const int statsY = 3 + maze_->getHeight() + 3;
    const int statsX = std::max(0, (COLS - statsWidth) / 2);

    move(statsY, statsX);
    attron(COLOR_PAIR(PAIR_PATH));
    addstr("Moves: ");
    attroff(COLOR_PAIR(PAIR_PATH));

    attron(A_BOLD | COLOR_PAIR(PAIR_PLAYER));
    printw("%d", moves_);
    attroff(A_BOLD | COLOR_PAIR(PAIR_PLAYER));

    attron(COLOR_PAIR(PAIR_PATH));
    addstr("   Time: ");
    attroff(COLOR_PAIR(PAIR_PATH));

    attron(A_BOLD | COLOR_PAIR(PAIR_WATER));
    printw("%02d:%02d", elapsed / 60, elapsed % 60);
    attroff(A_BOLD | COLOR_PAIR(PAIR_WATER));
}

void GameMode::drawInstructions() {
    const int baseY = 3 + maze_->getHeight() + 6;

    struct Segment {
        std::string text;
        short pair;
        bool bold;
    };

    const std::vector<std::vector<Segment>> lines = {
        {{"Controls:", PAIR_MOUNTAIN, true}},
        {{"↑/W", PAIR_PATH, true}, {": Move Up    ", PAIR_PATH, false}, {"↓/S", PAIR_PATH, true}, {": Move Down", PAIR_PATH, false}},
        {{"←/A", PAIR_PATH, true}, {": Move Left  ", PAIR_PATH, false}, {"→/D", PAIR_PATH, true}, {": Move Right", PAIR_PATH, false}},
        {{"Q/ESC", PAIR_PATH, true}, {": Quit Game", PAIR_PATH, false}},
        {},
        {{"Objective: Navigate ", PAIR_PATH, false}, {"●", PAIR_PLAYER, true}, {" to the goal ", PAIR_PATH, false}, {"★", PAIR_GOAL, true}},
        {{"Stay swift: fewer moves and faster time boost your score!", PAIR_PATH, false}}
    };

    int currentY = baseY;
    for (const auto& line : lines) {
        std::string plain;
        for (const auto& segment : line) {
            plain += segment.text;
        }

        if (plain.empty()) {
            ++currentY;
            continue;
        }

        const int width = measureDisplayWidth(plain);
        const int startX = std::max(0, (COLS - width) / 2);
        move(currentY, startX);

        for (const auto& segment : line) {
            if (segment.pair > 0) {
                attron(COLOR_PAIR(segment.pair));
            }
            if (segment.bold) {
                attron(A_BOLD);
            }
            addstr(segment.text.c_str());
            if (segment.bold) {
                attroff(A_BOLD);
            }
            if (segment.pair > 0) {
                attroff(COLOR_PAIR(segment.pair));
            }
        }

        ++currentY;
    }
}

void GameMode::showVictoryScreen() {
    clear();
    
    int centerY = LINES / 2 - 5;
    int centerX = COLS / 2;
    
    // Victory banner with beautiful UTF-8 - "CONGRATULATIONS!" is 16 chars, box is 38 wide
    // (38-16)/2 = 11 left + 11 right
    attron(COLOR_PAIR(PAIR_VICTORY) | A_BOLD | A_BLINK);
    mvprintw(centerY, centerX - 19, "╔══════════════════════════════════════╗");
    mvprintw(centerY + 1, centerX - 19, "║                                      ║");
    mvprintw(centerY + 2, centerX - 19, "║");
    attroff(COLOR_PAIR(PAIR_VICTORY) | A_BOLD | A_BLINK);
    
    attron(COLOR_PAIR(PAIR_GOAL) | A_BOLD);
    mvprintw(centerY + 2, centerX - 8, "CONGRATULATIONS!");
    attroff(COLOR_PAIR(PAIR_GOAL) | A_BOLD);
    
    attron(COLOR_PAIR(PAIR_VICTORY) | A_BOLD | A_BLINK);
    mvprintw(centerY + 2, centerX + 19, "║");
    mvprintw(centerY + 3, centerX - 19, "║                                      ║");
    mvprintw(centerY + 4, centerX - 19, "╚══════════════════════════════════════╝");
    attroff(COLOR_PAIR(PAIR_VICTORY) | A_BOLD | A_BLINK);
    
    // Stats
    int elapsed = getElapsedTime();
    attron(COLOR_PAIR(PAIR_PATH) | A_BOLD);
    mvprintw(centerY + 6, centerX - 10, "You solved the maze!");
    attroff(COLOR_PAIR(PAIR_PATH) | A_BOLD);
    
    attron(COLOR_PAIR(PAIR_PLAYER));
    mvprintw(centerY + 8, centerX - 8, "Total Moves: ");
    attroff(COLOR_PAIR(PAIR_PLAYER));
    attron(A_BOLD);
    printw("%d", moves_);
    attroff(A_BOLD);
    
    attron(COLOR_PAIR(PAIR_WATER));
    mvprintw(centerY + 9, centerX - 8, "Time Taken:  ");
    attroff(COLOR_PAIR(PAIR_WATER));
    attron(A_BOLD);
    printw("%02d:%02d", elapsed / 60, elapsed % 60);
    attroff(A_BOLD);
    
    // Calculate score (lower is better)
    int score = 10000 - (moves_ * 10) - elapsed;
    if (score < 0) score = 0;
    
    attron(COLOR_PAIR(PAIR_MOUNTAIN));
    mvprintw(centerY + 10, centerX - 8, "Final Score: ");
    attroff(COLOR_PAIR(PAIR_MOUNTAIN));
    attron(A_BOLD);
    printw("%d", score);
    attroff(A_BOLD);
    
    attron(COLOR_PAIR(PAIR_WALL));
    mvprintw(centerY + 13, centerX - 15, "Press any key to return to menu...");
    attroff(COLOR_PAIR(PAIR_WALL));
    
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
