#include "CLIUtils.h"
#include <cstring>
#include <limits>
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include <limits>
#include <termios.h>
#include <sys/ioctl.h>

namespace {
    class RawModeGuard {
    public:
        RawModeGuard() : active_(false) {}

        bool enable() {
            if (tcgetattr(STDIN_FILENO, &oldTerm_) == -1) {
                return false;
            }
            termios raw = oldTerm_;
            raw.c_lflag &= static_cast<unsigned>(~(ICANON | ECHO));
            raw.c_cc[VMIN] = 1;
            raw.c_cc[VTIME] = 0;
            if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
                return false;
            }
            active_ = true;
            return true;
        }

        ~RawModeGuard() {
            if (active_) {
                tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm_);
            }
        }

    private:
        termios oldTerm_{};
        bool active_;
    };
}

const char* CLIUtils::RESET = "\033[0m";
const char* CLIUtils::BOLD = "\033[1m";
const char* CLIUtils::DIM = "\033[2m";

const char* CLIUtils::BLACK = "\033[30m";
const char* CLIUtils::RED = "\033[31m";
const char* CLIUtils::GREEN = "\033[32m";
const char* CLIUtils::YELLOW = "\033[33m";
const char* CLIUtils::BLUE = "\033[34m";
const char* CLIUtils::MAGENTA = "\033[35m";
const char* CLIUtils::CYAN = "\033[36m";
const char* CLIUtils::WHITE = "\033[37m";

const char* CLIUtils::BG_BLACK = "\033[40m";
const char* CLIUtils::BG_RED = "\033[41m";
const char* CLIUtils::BG_GREEN = "\033[42m";
const char* CLIUtils::BG_YELLOW = "\033[43m";
const char* CLIUtils::BG_BLUE = "\033[44m";
const char* CLIUtils::BG_MAGENTA = "\033[45m";
const char* CLIUtils::BG_CYAN = "\033[46m";
const char* CLIUtils::BG_WHITE = "\033[47m";

CLIUtils::CLIUtils() : colorsEnabled_(true), colorScheme_(defaultScheme()) {}

CLIUtils::ColorScheme CLIUtils::defaultScheme() {
    return ColorScheme{
        std::string(CYAN),        
        std::string(BLUE),        
        std::string(YELLOW),      
        std::string(GREEN),       
        std::string(YELLOW),      
        std::string(RED),         
        std::string(CYAN),        
        std::string(DIM),         
        std::string(CYAN),        
        std::string(MAGENTA),     
        std::string(WHITE),       
        std::string(BG_BLACK),    
        std::string(WHITE),       
        std::string(BG_BLUE),     
        std::string(WHITE)        
    };
}

void CLIUtils::getTerminalSize(int& rows, int& cols) const {
    winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0 || ws.ws_row == 0) {
        rows = 40;
        cols = 120;
        return;
    }
    rows = static_cast<int>(ws.ws_row);
    cols = static_cast<int>(ws.ws_col);
}

int CLIUtils::centerPadding(int contentWidth) const {
    int rows = 0;
    int cols = 0;
    getTerminalSize(rows, cols);
    if (contentWidth <= 0 || cols <= contentWidth) {
        return 0;
    }
    return (cols - contentWidth) / 2;
}

int CLIUtils::measureDisplayWidth(const std::string& text) const {
    int width = 0;
    for (std::size_t i = 0; i < text.size(); ++i) {
        unsigned char ch = static_cast<unsigned char>(text[i]);

        if (ch == '\t') {
            width += 4;
            continue;
        }

        if (ch == '\n' || ch == '\r') {
            continue;
        }

        if (ch == '\033') {
            std::size_t j = i + 1;
            if (j < text.size() && text[j] == '[') {
                ++j;
                while (j < text.size() && text[j] != 'm') {
                    ++j;
                }
                if (j < text.size()) {
                    i = j;
                    continue;
                }
            }
        }

        if ((ch & 0x80) == 0) {
            width += 1;
            continue;
        }

        int bytes = 0;
        if ((ch & 0xE0) == 0xC0) {
            bytes = 1;
        } else if ((ch & 0xF0) == 0xE0) {
            bytes = 2;
        } else if ((ch & 0xF8) == 0xF0) {
            bytes = 3;
        }

        std::string utf8;
        utf8 += text[i];
        for (int consumed = 0; consumed < bytes && i + 1 < text.size(); ++consumed) {
            utf8 += text[++i];
        }

        if (utf8 == "█" || utf8 == "≈" || utf8 == "▲" || utf8 == "◆" || utf8 == "◇" || utf8 == "★") {
            width += 2;
        } else {
            width += 1;
        }
    }
    return width;
}

std::string CLIUtils::resolveColor(const std::string& candidate, const char* fallback) const {
    if (!candidate.empty()) {
        return candidate;
    }
    if (fallback) {
        return std::string(fallback);
    }
    return "";
}

void CLIUtils::clearScreen() const {
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

void CLIUtils::moveCursor(int row, int col) const {
    std::cout << "\033[" << row << ";" << col << "H";
}

void CLIUtils::hideCursor() const {
    std::cout << "\033[?25l";
    std::cout.flush();
}

void CLIUtils::showCursor() const {
    std::cout << "\033[?25h";
    std::cout.flush();
}

void CLIUtils::printColored(const char* text, const char* color) const {
    printColored(std::string(text ? text : ""), color ? std::string(color) : std::string());
}

void CLIUtils::printColored(const std::string& text, const std::string& color) const {
    if (colorsEnabled_ && !color.empty()) {
        std::cout << color << text << RESET;
    } else {
        std::cout << text;
    }
}

void CLIUtils::printSuccess(const char* text) const {
    if (colorsEnabled_) {
        const std::string color = resolveColor(colorScheme_.success, GREEN);
        std::cout << color << "✓ " << text << RESET << std::endl;
    } else {
        std::cout << "[OK] " << text << std::endl;
    }
}

void CLIUtils::printError(const char* text) const {
    if (colorsEnabled_) {
        const std::string color = resolveColor(colorScheme_.error, RED);
        std::cout << color << "✗ " << text << RESET << std::endl;
    } else {
        std::cout << "[ERROR] " << text << std::endl;
    }
}

void CLIUtils::printWarning(const char* text) const {
    if (colorsEnabled_) {
        const std::string color = resolveColor(colorScheme_.warning, YELLOW);
        std::cout << color << "⚠ " << text << RESET << std::endl;
    } else {
        std::cout << "[WARNING] " << text << std::endl;
    }
}

void CLIUtils::printInfo(const char* text) const {
    if (colorsEnabled_) {
        const std::string color = resolveColor(colorScheme_.info, CYAN);
        std::cout << color << "ℹ " << text << RESET << std::endl;
    } else {
        std::cout << "[INFO] " << text << std::endl;
    }
}

void CLIUtils::printHighlight(const char* text) const {
    if (colorsEnabled_) {
        const std::string color = resolveColor(colorScheme_.accent, YELLOW);
        std::cout << BOLD << color << text << RESET << std::endl;
    } else {
        std::cout << text << std::endl;
    }
}

void CLIUtils::drawBox(int x, int y, int width, int height, const char* title) const {
    const std::string frameColor = resolveColor(colorScheme_.frame, CYAN);
    const std::string titleColor = resolveColor(colorScheme_.headline, CYAN);

    moveCursor(y, x);
    if (colorsEnabled_) std::cout << frameColor;
    std::cout << "┌";
    for (int i = 0; i < width - 2; i++) std::cout << "─";
    std::cout << "┐";
    if (colorsEnabled_) std::cout << RESET;
    
    if (title != nullptr) {
        int titleLen = std::strlen(title);
        moveCursor(y, x + (width - titleLen) / 2);
        if (colorsEnabled_) {
            std::cout << BOLD << titleColor << title << RESET;
        } else {
            std::cout << title;
        }
    }
    
    for (int i = 1; i < height - 1; i++) {
        moveCursor(y + i, x);
        if (colorsEnabled_) std::cout << frameColor;
        std::cout << "│";
        if (colorsEnabled_) std::cout << RESET;
        moveCursor(y + i, x + width - 1);
        if (colorsEnabled_) std::cout << frameColor;
        std::cout << "│";
        if (colorsEnabled_) std::cout << RESET;
    }
    
    moveCursor(y + height - 1, x);
    if (colorsEnabled_) std::cout << frameColor;
    std::cout << "└";
    for (int i = 0; i < width - 2; i++) std::cout << "─";
    std::cout << "┘";
    if (colorsEnabled_) std::cout << RESET;
}

void CLIUtils::drawHorizontalLine(int length, char c) const {
    for (int i = 0; i < length; i++) {
        std::cout << c;
    }
    std::cout << std::endl;
}

void CLIUtils::drawProgressBar(int current, int total, int width) const {
    float progress = (float)current / total;
    int filled = (int)(progress * width);
    const std::string fillColor = resolveColor(colorScheme_.success, GREEN);
    const std::string remainColor = resolveColor(colorScheme_.muted, DIM);
    const std::string percentColor = resolveColor(colorScheme_.badge, WHITE);
    
    std::cout << "[";
    if (colorsEnabled_) std::cout << fillColor;
    for (int i = 0; i < filled; i++) std::cout << "█";
    if (colorsEnabled_) {
        std::cout << RESET;
        if (filled < width) {
            std::cout << remainColor;
        }
    }
    for (int i = filled; i < width; i++) std::cout << "░";
    if (colorsEnabled_) std::cout << RESET;
    std::cout << "] ";
    if (colorsEnabled_) std::cout << percentColor;
    std::cout << (int)(progress * 100) << "%";
    if (colorsEnabled_) std::cout << RESET;
    std::cout.flush();
}

void CLIUtils::drawSpinner(int frame) const {
    const char* frames[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    if (colorsEnabled_) {
        std::cout << resolveColor(colorScheme_.info, CYAN) << frames[frame % 10] << RESET;
    } else {
        const char* simpleFrames[] = {"|", "/", "-", "\\"};
        std::cout << simpleFrames[frame % 4];
    }
    std::cout.flush();
}

void CLIUtils::waitForEnter() const {
    std::string message = "Press Enter to continue...";
    int pad = centerPadding(measureDisplayWidth(message));
    
    std::cout << "\n" << std::string(pad, ' ');
    if (colorsEnabled_) {
        std::cout << resolveColor(colorScheme_.muted, DIM) << message << RESET;
    } else {
        std::cout << message;
    }
    std::cout.flush();
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int CLIUtils::getNumberInput(const char* prompt, int min, int max) const {
    int value;
    while (true) {
        int promptLen = measureDisplayWidth(std::string(prompt));
        int pad = centerPadding(promptLen + 20); 
        
        std::cout << std::string(pad, ' ');
        if (colorsEnabled_) {
            std::cout << resolveColor(colorScheme_.info, CYAN) << prompt << RESET;
        } else {
            std::cout << prompt;
        }
        std::cout.flush();
        
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(10000, '\n');
            return value;
        }
        
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        printError("Invalid input. Please try again.");
    }
}

void CLIUtils::getStringInput(const char* prompt, char* buffer, int maxLen) const {
    int promptLen = measureDisplayWidth(std::string(prompt));
    int pad = centerPadding(promptLen + 30); 
    
    std::cout << std::string(pad, ' ');
    if (colorsEnabled_) {
        std::cout << resolveColor(colorScheme_.info, CYAN) << prompt << RESET;
    } else {
        std::cout << prompt;
    }
    std::cout.flush();
    std::cin.getline(buffer, maxLen);
}

void CLIUtils::printCentered(const char* text, int width) const {
    int targetWidth = width;
    if (targetWidth <= 0) {
        int rows = 0;
        getTerminalSize(rows, targetWidth);
    }
    int textLen = static_cast<int>(std::strlen(text));
    int padding = centerPadding(std::max(targetWidth, textLen));
    std::cout << std::string(padding, ' ') << text << std::endl;
}

void CLIUtils::printHeader(const char* text, bool leadingBlank) const {
    if (leadingBlank) {
        std::cout << std::endl;
    }
    if (colorsEnabled_) {
        const size_t len = std::strlen(text);
        const std::string frameColor = resolveColor(colorScheme_.frame, CYAN);
        const std::string headlineColor = resolveColor(colorScheme_.headline, WHITE);
        int innerWidth = static_cast<int>(len) + 4;
        int totalWidth = innerWidth + 2;
        int pad = centerPadding(totalWidth);

        std::cout << std::string(pad, ' ');
        std::cout << frameColor << "╔";
        for (int i = 0; i < innerWidth; ++i) std::cout << "═";
        std::cout << "╗" << RESET << std::endl;

        std::cout << std::string(pad, ' ');
        std::cout << frameColor << "║" << RESET << "  "
                  << BOLD << headlineColor << text << RESET
                  << "  " << frameColor << "║" << RESET << std::endl;

        std::cout << std::string(pad, ' ');
        std::cout << frameColor << "╚";
        for (int i = 0; i < innerWidth; ++i) std::cout << "═";
        std::cout << "╝" << RESET << std::endl;
    } else {
        int len = std::strlen(text);
        int innerWidth = len + 4;
        int totalWidth = innerWidth + 2;
        int pad = centerPadding(totalWidth);
        std::cout << std::string(pad, ' ');
        for (int i = 0; i < innerWidth + 2; ++i) std::cout << "=";
        std::cout << std::endl;
        std::cout << std::string(pad, ' ') << "= " << text << " =" << std::endl;
        std::cout << std::string(pad, ' ');
        for (int i = 0; i < innerWidth + 2; ++i) std::cout << "=";
        std::cout << std::endl;
    }
}

void CLIUtils::printSubHeader(const char* text) const {
    std::string content = std::string("▸ ") + text;
    int pad = centerPadding(std::max(3, measureDisplayWidth(content)));
    if (colorsEnabled_) {
        const std::string accentColor = resolveColor(colorScheme_.accent, YELLOW);
        std::cout << std::string(pad, ' ') << BOLD << accentColor << "▸ " << text << RESET << std::endl;
    } else {
        std::cout << std::string(pad, ' ') << ">> " << text << std::endl;
    }
}

void CLIUtils::printSeparator() const {
    int separatorWidth = 60;
    int pad = centerPadding(separatorWidth);
    if (colorsEnabled_) {
        std::cout << resolveColor(colorScheme_.muted, DIM);
    }
    std::cout << std::string(pad, ' ');
    for (int i = 0; i < separatorWidth; ++i) std::cout << "─";
    if (colorsEnabled_) {
        std::cout << RESET;
    }
    std::cout << std::endl;
}

void CLIUtils::typewriterEffect(const char* text, int delayMs) const {
    for (size_t i = 0; i < std::strlen(text); i++) {
        std::cout << text[i];
        std::cout.flush();
        usleep(delayMs * 1000);
    }
    std::cout << std::endl;
}

void CLIUtils::fadeIn(const char* text) const {
    if (colorsEnabled_) {
        std::cout << resolveColor(colorScheme_.muted, DIM) << text << RESET << std::endl;
    } else {
        std::cout << text << std::endl;
    }
}

void CLIUtils::enableColors() {
    colorsEnabled_ = true;
}

void CLIUtils::disableColors() {
    colorsEnabled_ = false;
}

bool CLIUtils::areColorsEnabled() const {
    return colorsEnabled_;
}

void CLIUtils::setColorScheme(const ColorScheme& scheme) {
    colorScheme_ = scheme;
}

const CLIUtils::ColorScheme& CLIUtils::getColorScheme() const {
    return colorScheme_;
}

CLIUtils::InputKey CLIUtils::readMenuKey() const {
    RawModeGuard guard;
    guard.enable();

    unsigned char c = 0;
    if (read(STDIN_FILENO, &c, 1) != 1) {
        return InputKey::Other;
    }

    if (c == '\033') {
        unsigned char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) {
            return InputKey::Escape;
        }
        if (seq[0] == '[') {
            if (read(STDIN_FILENO, &seq[1], 1) != 1) {
                return InputKey::Other;
            }
            switch (seq[1]) {
                case 'A': return InputKey::Up;
                case 'B': return InputKey::Down;
                case 'C': return InputKey::Right;
                case 'D': return InputKey::Left;
                default:  return InputKey::Other;
            }
        }
        return InputKey::Escape;
    }

    switch (c) {
        case '\n':
        case '\r':
            return InputKey::Enter;
        case '\t':
            return InputKey::Right;
        case 'w':
        case 'W':
            return InputKey::Up;
        case 's':
        case 'S':
            return InputKey::Down;
        case 'a':
        case 'A':
            return InputKey::Left;
        case 'd':
        case 'D':
            return InputKey::Right;
        case 'q':
        case 'Q':
            return InputKey::Escape;
        default:
            break;
    }

    return InputKey::Other;
}

int CLIUtils::selectFromList(const std::string& title,
                             const std::vector<std::string>& options,
                             int initialIndex,
                             bool allowEscape,
                             bool wrap) const {
    if (options.empty()) {
        return -1;
    }

    int selected = std::max(0, std::min(static_cast<int>(options.size()) - 1, initialIndex));
    const std::string pointerActive = resolveColor(colorScheme_.accent, YELLOW);
    const std::string pointerIdle = resolveColor(colorScheme_.muted, DIM);
    const std::string textActive = resolveColor(colorScheme_.selectionForeground, WHITE);
    const std::string textIdle = !colorScheme_.panelForeground.empty()
                                     ? colorScheme_.panelForeground
                                     : resolveColor(colorScheme_.primary, WHITE);
    const std::string surfaceBg = colorScheme_.panelBackground;
    const std::string highlightBg = colorScheme_.selectionBackground.empty()
                                        ? std::string(BG_BLUE)
                                        : colorScheme_.selectionBackground;

    int maxLineWidth = 0;
    for (const auto& option : options) {
        std::string sample = std::string(" ➤ ") + option + " ";
        int width = measureDisplayWidth(sample);
        if (width > maxLineWidth) {
            maxLineWidth = width;
        }
    }
    if (maxLineWidth < 20) {
        maxLineWidth = 20;
    }

    while (true) {
        clearScreen();

        int rows = 0;
        int cols = 0;
        getTerminalSize(rows, cols);

        const int headerHeight = title.empty() ? 0 : 3;
        const int headerSpacing = 1; 
        const int optionLines = static_cast<int>(options.size());
        const int spacerAfterOptions = 1;
        const int instructionsLines = 1;
        const int contentHeight = headerHeight + headerSpacing + optionLines + spacerAfterOptions + instructionsLines;
        const int topPadding = std::max(0, (rows - contentHeight) / 2);

        for (int i = 0; i < topPadding; ++i) {
            std::cout << "\n";
        }

        if (!title.empty()) {
            printHeader(title.c_str(), false);
        }

        std::cout << "\n";
        for (std::size_t i = 0; i < options.size(); ++i) {
            bool isSelected = static_cast<int>(i) == selected;
            int leftPad = centerPadding(maxLineWidth);
            std::string pointerSymbol = isSelected ? "➤" : "•";
            std::string lineCore = " " + pointerSymbol + " " + options[i] + " ";
            int lineWidth = measureDisplayWidth(lineCore);
            if (lineWidth > maxLineWidth) {
                lineWidth = maxLineWidth;
            }

            std::cout << std::string(leftPad, ' ');

            if (colorsEnabled_) {
                if (isSelected) {
                    std::cout << highlightBg;
                } else if (!surfaceBg.empty()) {
                    std::cout << surfaceBg;
                }
            }

            std::cout << " ";
            if (colorsEnabled_) {
                std::cout << (isSelected ? pointerActive : pointerIdle);
            }
            std::cout << pointerSymbol;

            if (colorsEnabled_) {
                std::cout << (isSelected ? textActive : textIdle);
            }
            std::cout << " " << options[i] << " ";

            int remaining = maxLineWidth - lineWidth;
            if (remaining > 0) {
                std::cout << std::string(remaining, ' ');
            }

            if (colorsEnabled_) {
                std::cout << RESET;
            }
            std::cout << "\n";
        }

        std::cout << "\n";
        std::string instructions = "Use ↑/↓ to navigate, Enter to select";
        if (allowEscape) {
            instructions += ", Esc to cancel";
        }
        int instructionsWidth = measureDisplayWidth(instructions);
        int infoPad = centerPadding(instructionsWidth);
        std::cout << std::string(infoPad, ' ');
        if (colorsEnabled_) {
            if (!surfaceBg.empty()) {
                std::cout << surfaceBg;
            }
            std::cout << textIdle;
        }
        std::cout << instructions;
        if (colorsEnabled_) {
            std::cout << RESET;
        }
        std::cout << std::flush;

        InputKey key = readMenuKey();
        if (key == InputKey::Enter) {
            return selected;
        } else if (key == InputKey::Escape && allowEscape) {
            return -1;
        } else if (key == InputKey::Up) {
            if (selected > 0) {
                --selected;
            } else if (wrap) {
                selected = static_cast<int>(options.size()) - 1;
            }
        } else if (key == InputKey::Down) {
            if (selected + 1 < static_cast<int>(options.size())) {
                ++selected;
            } else if (wrap) {
                selected = 0;
            }
        }
    }
}
