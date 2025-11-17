#include "GameMode.h"
#include <ncurses.h>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <locale.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cwchar>

namespace {
    constexpr short PAIR_PLAYER = 1;
    constexpr short PAIR_GOAL = 2;
    constexpr short PAIR_WALL = 3;
    constexpr short PAIR_PATH = 4;
    constexpr short PAIR_WATER = 5;
    constexpr short PAIR_MOUNTAIN = 6;
    constexpr short PAIR_VICTORY = 7;
    constexpr short PAIR_HEADER_GRADIENT_1 = 8;
    constexpr short PAIR_HEADER_GRADIENT_2 = 9;
    constexpr short PAIR_HEADER_GRADIENT_3 = 10;
    constexpr short PAIR_HEADER_GRADIENT_4 = 11;
    constexpr short PAIR_FOOTER_GRADIENT_1 = 12;
    constexpr short PAIR_FOOTER_GRADIENT_2 = 13;
    constexpr short PAIR_TOAST_BACKGROUND = 14;
    constexpr short PAIR_TOAST_TEXT = 15;
    constexpr short PAIR_ALERT = 16;
    constexpr short PAIR_STATUS_LABEL = 17;
    constexpr short PAIR_STATUS_VALUE = 18;
    constexpr short PAIR_MAZE_GRADIENT_1 = 19;
    constexpr short PAIR_MAZE_GRADIENT_2 = 20;
    constexpr short PAIR_MAZE_GRADIENT_3 = 21;
    constexpr short PAIR_MAZE_GRADIENT_4 = 22;
}

GameMode::GameMode() 
        : maze_(nullptr), playerPos_(0, 0), goalPos_(0, 0),
          moves_(0), startTime_(0), gameWon_(false), gameRunning_(false),
                    theme_(GameTheme::NeonMatrix), headerGradientPairs_{0, 0, 0, 0},
                    mazeGradientPairs_{0, 0, 0, 0}, goalPulsePairs_{0, 0, 0},
                      footerGradientPairs_{0, 0, 0, 0}, toastPrimaryPair_(0), toastAccentPair_(0), toastActiveAccentPair_(0),
          toastDuration_(std::chrono::milliseconds(0)), toastVisible_(false),
          pulseFrame_(0), frameCounter_(0) {
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
    timeout(-1);
    curs_set(1);
    endwin();
}

void GameMode::drawMaze() {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    if (maxY <= 2) return;

    int mazeHeight = maze_->getHeight();
    int mazeWidth = maze_->getWidth();

    int startY = std::max((maxY - mazeHeight) / 2 - 2, 1);
    int startX = std::max((maxX - mazeWidth * 2) / 2, 2);

    for (int y = 0; y < mazeHeight; ++y) {
        for (int x = 0; x < mazeWidth; ++x) {
            Point current(x, y);
            wchar_t glyph = L' ';
            short colorPair = 0;
            char cell = maze_->getCellAt(current);

            if (current == playerPos_) {
                glyph = L'⬤';
                colorPair = PAIR_PLAYER;
            } else if (current == goalPos_ || cell == 'G') {
                glyph = (pulseFrame_ % 6 < 3) ? L'★' : L'✦';
                const int pulseSize = static_cast<int>(goalPulsePairs_.size());
                colorPair = pulseSize > 0 ? goalPulsePairs_[pulseFrame_ % pulseSize] : PAIR_GOAL;
            } else if (cell == '#') {
                bool strongWall = ((x + y + pulseFrame_) % 7) < 3;
                glyph = strongWall ? L'▓' : L'▒';
                const int gradientSize = static_cast<int>(mazeGradientPairs_.size());
                if (gradientSize > 0) {
                    colorPair = mazeGradientPairs_[(x + y + pulseFrame_) % gradientSize];
                } else {
                    colorPair = PAIR_WALL;
                }
            } else if (cell == '~') {
                glyph = L'≈';
                colorPair = PAIR_WATER;
            } else if (cell == '^') {
                glyph = L'▲';
                colorPair = PAIR_MOUNTAIN;
            } else if (cell == '.' || cell == 'S') {
                glyph = (pulseFrame_ % 4 < 2) ? L'·' : L'•';
                colorPair = PAIR_PATH;
            } else if (cell == ' ') {
                glyph = (pulseFrame_ % 8 < 4) ? L'·' : L' ';
                colorPair = PAIR_PATH;
            }

            if (colorPair > 0) attron(COLOR_PAIR(colorPair));
            std::wstring cellStr(2, glyph);
            mvaddwstr(startY + y, startX + x * 2, cellStr.c_str());
            if (colorPair > 0) attroff(COLOR_PAIR(colorPair));
        }
    }

    drawGradientRow(startY - 1, startX, mazeWidth * 2, mazeGradientPairs_, L'▄');
    drawGradientRow(startY + mazeHeight, startX, mazeWidth * 2, mazeGradientPairs_, L'▀');
}

void GameMode::configureColorPairs() {
    struct PairDef {
        short fg;
        short bg;
    };

    PairDef player {COLOR_GREEN, COLOR_BLACK};
    PairDef goal {COLOR_MAGENTA, COLOR_BLACK};
    PairDef wall {COLOR_BLUE, COLOR_BLACK};
    PairDef path {COLOR_WHITE, COLOR_BLACK};
    PairDef water {COLOR_CYAN, COLOR_BLACK};
    PairDef mountain {COLOR_YELLOW, COLOR_BLACK};
    PairDef victory {COLOR_BLACK, COLOR_GREEN};
    std::array<PairDef, 4> headerGradient {
        PairDef{COLOR_BLACK, COLOR_BLUE},
        PairDef{COLOR_BLACK, COLOR_MAGENTA},
        PairDef{COLOR_BLACK, COLOR_CYAN},
        PairDef{COLOR_BLACK, COLOR_BLACK}
    };
    std::array<PairDef, 4> mazeGradient {
        PairDef{COLOR_GREEN, COLOR_BLACK},
        PairDef{COLOR_CYAN, COLOR_BLACK},
        PairDef{COLOR_GREEN, COLOR_BLACK},
        PairDef{COLOR_BLUE, COLOR_BLACK}
    };
    PairDef footerPrimary {COLOR_BLACK, COLOR_BLUE};
    PairDef footerSecondary {COLOR_BLACK, COLOR_GREEN};
    PairDef toastBackground {COLOR_BLACK, goal.fg};
    PairDef toastText {goal.fg, COLOR_BLACK};
    PairDef statusLabel {COLOR_CYAN, COLOR_BLACK};
    PairDef statusValue {COLOR_WHITE, COLOR_BLACK};
    PairDef alert {COLOR_BLACK, COLOR_RED};

    switch (theme_) {
        case GameTheme::EmberGlow:
            player = {COLOR_YELLOW, COLOR_BLACK};
            goal = {COLOR_RED, COLOR_BLACK};
            wall = {COLOR_RED, COLOR_BLACK};
            path = {COLOR_YELLOW, COLOR_BLACK};
            water = {COLOR_CYAN, COLOR_BLACK};
            mountain = {COLOR_MAGENTA, COLOR_BLACK};
            victory = {COLOR_BLACK, COLOR_YELLOW};
            headerGradient = { PairDef{COLOR_BLACK, COLOR_YELLOW},
                               PairDef{COLOR_BLACK, COLOR_RED},
                               PairDef{COLOR_BLACK, COLOR_MAGENTA},
                               PairDef{COLOR_BLACK, COLOR_RED} };
            mazeGradient = { PairDef{COLOR_RED, COLOR_BLACK},
                              PairDef{COLOR_YELLOW, COLOR_BLACK},
                              PairDef{COLOR_RED, COLOR_BLACK},
                              PairDef{COLOR_MAGENTA, COLOR_BLACK} };
            footerPrimary = {COLOR_BLACK, COLOR_RED};
            footerSecondary = {COLOR_BLACK, COLOR_YELLOW};
            toastBackground = {COLOR_BLACK, COLOR_RED};
            toastText = {COLOR_YELLOW, COLOR_BLACK};
            statusLabel = {COLOR_YELLOW, COLOR_BLACK};
            statusValue = {COLOR_RED, COLOR_BLACK};
            break;
        case GameTheme::ArcticAurora:
            player = {COLOR_CYAN, COLOR_BLACK};
            goal = {COLOR_WHITE, COLOR_BLACK};
            wall = {COLOR_BLUE, COLOR_BLACK};
            path = {COLOR_WHITE, COLOR_BLACK};
            water = {COLOR_CYAN, COLOR_BLACK};
            mountain = {COLOR_MAGENTA, COLOR_BLACK};
            victory = {COLOR_BLACK, COLOR_CYAN};
            headerGradient = { PairDef{COLOR_BLACK, COLOR_CYAN},
                               PairDef{COLOR_BLACK, COLOR_BLUE},
                               PairDef{COLOR_BLACK, COLOR_WHITE},
                               PairDef{COLOR_BLACK, COLOR_BLUE} };
            mazeGradient = { PairDef{COLOR_CYAN, COLOR_BLACK},
                              PairDef{COLOR_BLUE, COLOR_BLACK},
                              PairDef{COLOR_WHITE, COLOR_BLACK},
                              PairDef{COLOR_BLUE, COLOR_BLACK} };
            footerPrimary = {COLOR_BLACK, COLOR_BLUE};
            footerSecondary = {COLOR_BLACK, COLOR_CYAN};
            toastBackground = {COLOR_BLACK, COLOR_CYAN};
            toastText = {COLOR_WHITE, COLOR_BLACK};
            statusLabel = {COLOR_CYAN, COLOR_BLACK};
            statusValue = {COLOR_WHITE, COLOR_BLACK};
            break;
        case GameTheme::Monochrome:
            player = {COLOR_WHITE, COLOR_BLACK};
            goal = {COLOR_WHITE, COLOR_BLACK};
            wall = {COLOR_WHITE, COLOR_BLACK};
            path = {COLOR_WHITE, COLOR_BLACK};
            water = {COLOR_WHITE, COLOR_BLACK};
            mountain = {COLOR_WHITE, COLOR_BLACK};
            victory = {COLOR_BLACK, COLOR_WHITE};
            headerGradient = { PairDef{COLOR_WHITE, COLOR_BLACK},
                               PairDef{COLOR_WHITE, COLOR_BLACK},
                               PairDef{COLOR_WHITE, COLOR_BLACK},
                               PairDef{COLOR_WHITE, COLOR_BLACK} };
            mazeGradient = { PairDef{COLOR_WHITE, COLOR_BLACK},
                              PairDef{COLOR_WHITE, COLOR_BLACK},
                              PairDef{COLOR_WHITE, COLOR_BLACK},
                              PairDef{COLOR_WHITE, COLOR_BLACK} };
            footerPrimary = {COLOR_WHITE, COLOR_BLACK};
            footerSecondary = {COLOR_WHITE, COLOR_BLACK};
            toastBackground = {COLOR_BLACK, COLOR_WHITE};
            toastText = {COLOR_WHITE, COLOR_BLACK};
            statusLabel = {COLOR_WHITE, COLOR_BLACK};
            statusValue = {COLOR_WHITE, COLOR_BLACK};
            alert = {COLOR_WHITE, COLOR_BLACK};
            break;
        case GameTheme::NeonMatrix:
        default:
            mazeGradient = { PairDef{COLOR_GREEN, COLOR_BLACK},
                              PairDef{COLOR_CYAN, COLOR_BLACK},
                              PairDef{COLOR_GREEN, COLOR_BLACK},
                              PairDef{COLOR_BLUE, COLOR_BLACK} };
            footerPrimary = {COLOR_BLACK, COLOR_BLUE};
            footerSecondary = {COLOR_BLACK, COLOR_GREEN};
            toastBackground = {COLOR_BLACK, goal.fg};
            toastText = {goal.fg, COLOR_BLACK};
            statusLabel = {COLOR_CYAN, COLOR_BLACK};
            statusValue = {COLOR_MAGENTA, COLOR_BLACK};
            break;
    }

    auto setPair = [](short idx, const PairDef& def) {
        init_pair(idx, def.fg, def.bg);
    };

    init_pair(PAIR_PLAYER, player.fg, player.bg);
    init_pair(PAIR_GOAL, goal.fg, goal.bg);
    init_pair(PAIR_WALL, wall.fg, wall.bg);
    init_pair(PAIR_PATH, path.fg, path.bg);
    init_pair(PAIR_WATER, water.fg, water.bg);
    init_pair(PAIR_MOUNTAIN, mountain.fg, mountain.bg);
    init_pair(PAIR_VICTORY, victory.fg, victory.bg);

    for (std::size_t i = 0; i < headerGradient.size(); ++i) {
        short idx = static_cast<short>(PAIR_HEADER_GRADIENT_1 + i);
        setPair(idx, headerGradient[i]);
        headerGradientPairs_[i] = idx;
    }

    for (std::size_t i = 0; i < mazeGradient.size(); ++i) {
        short idx = static_cast<short>(PAIR_MAZE_GRADIENT_1 + i);
        setPair(idx, mazeGradient[i]);
        mazeGradientPairs_[i] = idx;
    }

    setPair(PAIR_FOOTER_GRADIENT_1, footerPrimary);
    setPair(PAIR_FOOTER_GRADIENT_2, footerSecondary);
    footerGradientPairs_ = {PAIR_FOOTER_GRADIENT_1, PAIR_FOOTER_GRADIENT_2,
                            PAIR_FOOTER_GRADIENT_1, PAIR_FOOTER_GRADIENT_2};

    setPair(PAIR_TOAST_BACKGROUND, toastBackground);
    setPair(PAIR_TOAST_TEXT, toastText);
    setPair(PAIR_ALERT, alert);
    setPair(PAIR_STATUS_LABEL, statusLabel);
    setPair(PAIR_STATUS_VALUE, statusValue);

    toastPrimaryPair_ = PAIR_TOAST_BACKGROUND;
    toastAccentPair_ = PAIR_TOAST_TEXT;
    toastActiveAccentPair_ = toastAccentPair_;
    goalPulsePairs_ = {PAIR_GOAL, PAIR_STATUS_VALUE, PAIR_GOAL};
}

void GameMode::drawGradientRow(int y, int startX, int width, const std::array<short, 4>& palette, wchar_t glyph) const {
    if (width <= 0) return;

    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    if (y < 0 || y >= maxY) return;

    int beginX = std::max(0, startX);
    int endX = std::min(maxX, startX + width);
    if (beginX >= endX) return;

    const int span = endX - beginX;
    const int paletteSize = static_cast<int>(palette.size());
    if (paletteSize == 0) return;

    std::wstring glyphStr;
    if (glyph != L' ') {
        glyphStr.assign(1, glyph);
    }

    for (int i = 0; i < span; ++i) {
        int paletteIndex = (paletteSize * i) / std::max(1, span);
        paletteIndex = std::min(paletteSize - 1, paletteIndex);
        short pair = palette[paletteIndex];
        if (pair > 0) attron(COLOR_PAIR(pair));

        if (glyph == L' ') {
            mvaddch(y, beginX + i, ' ');
        } else {
            move(y, beginX + i);
            addwstr(glyphStr.c_str());
        }

        if (pair > 0) attroff(COLOR_PAIR(pair));
    }
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

        if (++frameCounter_ % 6 == 0) {
            pulseFrame_ = (pulseFrame_ + 1) % 12;
        }

        updateToast();

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
        showToast("Step forward", PAIR_STATUS_VALUE, std::chrono::milliseconds(900));
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
    drawFooterBar();
    drawInstructions();
    drawToast();
    refresh();
}


void GameMode::drawStatusBar() {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    if (maxY <= 2) {
        return;
    }

    drawGradientRow(0, 0, maxX, headerGradientPairs_, L' ');
    drawGradientRow(1, 0, maxX, headerGradientPairs_, L' ');
    drawGradientRow(2, 0, maxX, headerGradientPairs_, L' ');

    const std::string title = "Maze Runner · Live Mode";
    const int titleX = std::max(0, (maxX - static_cast<int>(title.size())) / 2);
    move(1, titleX);
    attron(A_BOLD | COLOR_PAIR(headerGradientPairs_[1]));
    addstr(title.c_str());
    attroff(A_BOLD | COLOR_PAIR(headerGradientPairs_[1]));

    const int elapsed = getElapsedTime();
    const int minutes = elapsed / 60;
    const int seconds = elapsed % 60;

    move(2, 2);
    attron(COLOR_PAIR(PAIR_STATUS_LABEL) | A_BOLD);
    addstr("Moves");
    attroff(COLOR_PAIR(PAIR_STATUS_LABEL) | A_BOLD);
    attron(COLOR_PAIR(PAIR_STATUS_VALUE));
    printw(": %d", moves_);
    attroff(COLOR_PAIR(PAIR_STATUS_VALUE));

    move(2, maxX / 3);
    attron(COLOR_PAIR(PAIR_STATUS_LABEL) | A_BOLD);
    addstr("Time");
    attroff(COLOR_PAIR(PAIR_STATUS_LABEL) | A_BOLD);
    attron(COLOR_PAIR(PAIR_STATUS_VALUE));
    printw(": %02d:%02d", minutes, seconds);
    attroff(COLOR_PAIR(PAIR_STATUS_VALUE));

    auto themeToString = [](GameTheme theme) -> const char* {
        switch (theme) {
            case GameTheme::EmberGlow: return "Ember Glow";
            case GameTheme::ArcticAurora: return "Arctic Aurora";
            case GameTheme::Monochrome: return "Monochrome";
            case GameTheme::NeonMatrix:
            default: return "Neon Matrix";
        }
    };

    std::string themeLabel = std::string("Theme: ") + themeToString(theme_);
    const int themeX = std::max(2, maxX - static_cast<int>(themeLabel.size()) - 2);
    move(2, themeX);
    attron(COLOR_PAIR(PAIR_STATUS_LABEL) | A_BOLD);
    addstr("Theme: ");
    attroff(COLOR_PAIR(PAIR_STATUS_LABEL) | A_BOLD);
    attron(COLOR_PAIR(PAIR_STATUS_VALUE));
    addstr(themeToString(theme_));
    attroff(COLOR_PAIR(PAIR_STATUS_VALUE));
}

void GameMode::drawFooterBar() {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    const int baseY = std::max(0, maxY - 3);
    drawGradientRow(baseY, 0, maxX, footerGradientPairs_, L' ');
    drawGradientRow(baseY + 1, 0, maxX, footerGradientPairs_, L' ');

    move(baseY, 2);
    attron(COLOR_PAIR(PAIR_STATUS_LABEL) | A_BOLD);
    addstr("Legend");
    attroff(COLOR_PAIR(PAIR_STATUS_LABEL) | A_BOLD);

    attron(COLOR_PAIR(PAIR_STATUS_VALUE));
    addstr(": Player ");
    attron(COLOR_PAIR(PAIR_PLAYER) | A_BOLD);
    addstr("●");
    attroff(COLOR_PAIR(PAIR_PLAYER) | A_BOLD);
    attron(COLOR_PAIR(PAIR_STATUS_VALUE));
    addstr("  Goal ");
    attron(COLOR_PAIR(PAIR_GOAL) | A_BOLD);
    addstr("★");
    attroff(COLOR_PAIR(PAIR_GOAL) | A_BOLD);
    attron(COLOR_PAIR(PAIR_STATUS_VALUE));
    addstr("  Path ");
    attron(COLOR_PAIR(PAIR_PATH) | A_BOLD);
    addstr("·");
    attroff(COLOR_PAIR(PAIR_PATH) | A_BOLD);
    attron(COLOR_PAIR(PAIR_STATUS_VALUE));
    addstr("  Wall ");
    short wallPair = mazeGradientPairs_[0] ? mazeGradientPairs_[0] : PAIR_WALL;
    attron(COLOR_PAIR(wallPair) | A_BOLD);
    addstr("▒");
    attroff(COLOR_PAIR(wallPair) | A_BOLD);
    attron(COLOR_PAIR(PAIR_STATUS_VALUE));
    addstr("  ");
    attroff(COLOR_PAIR(PAIR_STATUS_VALUE));

    move(baseY + 1, 2);
    attron(COLOR_PAIR(PAIR_STATUS_LABEL) | A_BOLD);
    addstr("Hint");
    attroff(COLOR_PAIR(PAIR_STATUS_LABEL) | A_BOLD);
    attron(COLOR_PAIR(PAIR_STATUS_VALUE));
    addstr(": Reach the star swiftly. Press Q to return to the hub.");
    attroff(COLOR_PAIR(PAIR_STATUS_VALUE));
}

void GameMode::drawInstructions() {
    if (!maze_) return;

    const int mazeHeight = maze_->getHeight();
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    int baseY = std::min(maxY - 8, std::max(4, (maxY - mazeHeight) / 2 + mazeHeight + 2));

    struct Segment {
        std::string text;
        short pair;
        bool bold;
    };

    const std::vector<std::vector<Segment>> lines = {
        {{"Controls", PAIR_STATUS_LABEL, true}},
        {{"▲/W", PAIR_STATUS_VALUE, true}, {" move up", PAIR_STATUS_VALUE, false}},
        {{"▼/S", PAIR_STATUS_VALUE, true}, {" move down", PAIR_STATUS_VALUE, false}},
        {{"◄/A", PAIR_STATUS_VALUE, true}, {" move left", PAIR_STATUS_VALUE, false}},
        {{"►/D", PAIR_STATUS_VALUE, true}, {" move right", PAIR_STATUS_VALUE, false}},
        {{"Press Q or ESC to exit", PAIR_STATUS_VALUE, false}}
    };

    int currentY = baseY;
    for (const auto& line : lines) {
        std::string plain;
        for (const auto& segment : line) {
            plain += segment.text;
            plain += ' ';
        }

        const int width = measureDisplayWidth(plain);
        const int startX = std::max(0, (maxX - width) / 2);
        move(currentY, startX);

        for (const auto& segment : line) {
            if (segment.pair > 0) attron(COLOR_PAIR(segment.pair));
            if (segment.bold) attron(A_BOLD);
            addstr(segment.text.c_str());
            if (segment.bold) attroff(A_BOLD);
            if (segment.pair > 0) attroff(COLOR_PAIR(segment.pair));
            addch(' ');
        }

        ++currentY;
    }
}

void GameMode::showToast(const std::string& message, short colorPair, std::chrono::milliseconds duration) {
    if (message.empty()) return;

    toastMessage_ = message;
    toastStart_ = std::chrono::steady_clock::now();
    toastDuration_ = duration;
    toastVisible_ = true;
    toastActiveAccentPair_ = colorPair > 0 ? colorPair : toastAccentPair_;
}

void GameMode::updateToast() {
    if (!toastVisible_) return;

    auto now = std::chrono::steady_clock::now();
    if (now - toastStart_ >= toastDuration_) {
        toastVisible_ = false;
        toastMessage_.clear();
        toastActiveAccentPair_ = toastAccentPair_;
    }
}

void GameMode::drawToast() {
    if (!toastVisible_ || toastMessage_.empty()) return;

    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    const int padding = 6;
    const int maxBoxWidth = std::max(24, maxX - 10);
    int messageWidth = measureDisplayWidth(toastMessage_);
    std::string displayMessage = toastMessage_;

    if (messageWidth > maxBoxWidth - padding) {
        displayMessage = toastMessage_.substr(0, std::max(0, static_cast<int>(toastMessage_.size()) - 3)) + "...";
        messageWidth = measureDisplayWidth(displayMessage);
    }

    int boxWidth = std::min(maxBoxWidth, messageWidth + padding);
    if (boxWidth < 10) boxWidth = 10;

    int startX = std::max(0, (maxX - boxWidth) / 2);
    int toastY = std::max(3, maxY - 12);

    auto now = std::chrono::steady_clock::now();
    float progress = std::min(1.0f, static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - toastStart_).count()) /
                                   std::max(1.0f, static_cast<float>(toastDuration_.count())));
    bool pulseBright = (pulseFrame_ % 6) < 3;

    attron(COLOR_PAIR(toastActiveAccentPair_) | A_BOLD);
    move(toastY, startX);
    addstr("╭");
    for (int i = 0; i < boxWidth - 2; ++i) addstr("─");
    addstr("╮");
    attroff(COLOR_PAIR(toastActiveAccentPair_) | A_BOLD);

    attron(COLOR_PAIR(toastPrimaryPair_));
    move(toastY + 1, startX);
    addstr("│");
    for (int i = 0; i < boxWidth - 2; ++i) addch(' ');
    addstr("│");
    move(toastY + 2, startX);
    addstr("│");
    for (int i = 0; i < boxWidth - 2; ++i) addch(' ');
    addstr("│");
    attroff(COLOR_PAIR(toastPrimaryPair_));

    attron(COLOR_PAIR(toastActiveAccentPair_) | A_BOLD);
    move(toastY + 3, startX);
    addstr("╰");
    for (int i = 0; i < boxWidth - 2; ++i) addstr("─");
    addstr("╯");
    attroff(COLOR_PAIR(toastActiveAccentPair_) | A_BOLD);

    int textX = startX + std::max(1, (boxWidth - messageWidth) / 2);
    move(toastY + 1, textX);
    attron(COLOR_PAIR(toastActiveAccentPair_) | (pulseBright ? A_BOLD : A_DIM));
    addstr(displayMessage.c_str());
    attroff(COLOR_PAIR(toastActiveAccentPair_) | (pulseBright ? A_BOLD : A_DIM));

    int barWidth = boxWidth - 4;
    int barStartX = startX + 2;
    move(toastY + 2, barStartX);
    attron(COLOR_PAIR(toastActiveAccentPair_) | A_DIM);
    for (int i = 0; i < barWidth; ++i) addstr("─");
    attroff(COLOR_PAIR(toastActiveAccentPair_) | A_DIM);

    int remaining = static_cast<int>((1.0f - progress) * barWidth);
    move(toastY + 2, barStartX);
    attron(COLOR_PAIR(toastActiveAccentPair_) | A_BOLD);
    for (int i = 0; i < remaining; ++i) addstr("━");
    attroff(COLOR_PAIR(toastActiveAccentPair_) | A_BOLD);
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
