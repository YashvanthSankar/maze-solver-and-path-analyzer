#include "CLIUtils.h"
#include <cstring>
#include <unistd.h>

// ANSI color codes
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

CLIUtils::CLIUtils() : colorsEnabled_(true) {}

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
    if (colorsEnabled_) {
        std::cout << color << text << RESET;
    } else {
        std::cout << text;
    }
}

void CLIUtils::printSuccess(const char* text) const {
    if (colorsEnabled_) {
        std::cout << GREEN << "✓ " << text << RESET << std::endl;
    } else {
        std::cout << "[OK] " << text << std::endl;
    }
}

void CLIUtils::printError(const char* text) const {
    if (colorsEnabled_) {
        std::cout << RED << "✗ " << text << RESET << std::endl;
    } else {
        std::cout << "[ERROR] " << text << std::endl;
    }
}

void CLIUtils::printWarning(const char* text) const {
    if (colorsEnabled_) {
        std::cout << YELLOW << "⚠ " << text << RESET << std::endl;
    } else {
        std::cout << "[WARNING] " << text << std::endl;
    }
}

void CLIUtils::printInfo(const char* text) const {
    if (colorsEnabled_) {
        std::cout << CYAN << "ℹ " << text << RESET << std::endl;
    } else {
        std::cout << "[INFO] " << text << std::endl;
    }
}

void CLIUtils::printHighlight(const char* text) const {
    if (colorsEnabled_) {
        std::cout << BOLD << YELLOW << text << RESET << std::endl;
    } else {
        std::cout << text << std::endl;
    }
}

void CLIUtils::drawBox(int x, int y, int width, int height, const char* title) const {
    moveCursor(y, x);
    std::cout << "┌";
    for (int i = 0; i < width - 2; i++) std::cout << "─";
    std::cout << "┐";
    
    if (title != nullptr) {
        int titleLen = std::strlen(title);
        moveCursor(y, x + (width - titleLen) / 2);
        if (colorsEnabled_) {
            std::cout << BOLD << CYAN << title << RESET;
        } else {
            std::cout << title;
        }
    }
    
    for (int i = 1; i < height - 1; i++) {
        moveCursor(y + i, x);
        std::cout << "│";
        moveCursor(y + i, x + width - 1);
        std::cout << "│";
    }
    
    moveCursor(y + height - 1, x);
    std::cout << "└";
    for (int i = 0; i < width - 2; i++) std::cout << "─";
    std::cout << "┘";
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
    
    std::cout << "[";
    if (colorsEnabled_) std::cout << GREEN;
    for (int i = 0; i < filled; i++) std::cout << "█";
    if (colorsEnabled_) std::cout << DIM;
    for (int i = filled; i < width; i++) std::cout << "░";
    if (colorsEnabled_) std::cout << RESET;
    std::cout << "] " << (int)(progress * 100) << "%";
    std::cout.flush();
}

void CLIUtils::drawSpinner(int frame) const {
    const char* frames[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    if (colorsEnabled_) {
        std::cout << CYAN << frames[frame % 10] << RESET;
    } else {
        const char* simpleFrames[] = {"|", "/", "-", "\\"};
        std::cout << simpleFrames[frame % 4];
    }
    std::cout.flush();
}

void CLIUtils::waitForEnter() const {
    if (colorsEnabled_) {
        std::cout << DIM << "\nPress Enter to continue..." << RESET;
    } else {
        std::cout << "\nPress Enter to continue...";
    }
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

int CLIUtils::getNumberInput(const char* prompt, int min, int max) const {
    int value;
    while (true) {
        if (colorsEnabled_) {
            std::cout << CYAN << prompt << RESET;
        } else {
            std::cout << prompt;
        }
        
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
    if (colorsEnabled_) {
        std::cout << CYAN << prompt << RESET;
    } else {
        std::cout << prompt;
    }
    std::cin.getline(buffer, maxLen);
}

void CLIUtils::printCentered(const char* text, int width) const {
    int textLen = std::strlen(text);
    int padding = (width - textLen) / 2;
    
    for (int i = 0; i < padding; i++) std::cout << " ";
    std::cout << text << std::endl;
}

void CLIUtils::printHeader(const char* text) const {
    std::cout << std::endl;
    if (colorsEnabled_) {
        std::cout << BOLD << CYAN << "╔";
        for (size_t i = 0; i < std::strlen(text) + 4; i++) std::cout << "═";
        std::cout << "╗" << std::endl;
        std::cout << "║  " << text << "  ║" << std::endl;
        std::cout << "╚";
        for (size_t i = 0; i < std::strlen(text) + 4; i++) std::cout << "═";
        std::cout << "╝" << RESET << std::endl;
    } else {
        int len = std::strlen(text);
        for (int i = 0; i < len + 4; i++) std::cout << "=";
        std::cout << "\n  " << text << "\n";
        for (int i = 0; i < len + 4; i++) std::cout << "=";
        std::cout << std::endl;
    }
}

void CLIUtils::printSubHeader(const char* text) const {
    if (colorsEnabled_) {
        std::cout << BOLD << YELLOW << "▸ " << text << RESET << std::endl;
    } else {
        std::cout << ">> " << text << std::endl;
    }
}

void CLIUtils::printSeparator() const {
    if (colorsEnabled_) {
        std::cout << DIM;
    }
    for (int i = 0; i < 60; i++) std::cout << "─";
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
        std::cout << DIM << text << RESET << std::endl;
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
