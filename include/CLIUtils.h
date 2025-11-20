#ifndef CLIUTILS_H
#define CLIUTILS_H

#include <iostream>
#include <string>
#include <vector>


class CLIUtils {
private:
    static const char* RESET;
    static const char* BOLD;
    static const char* DIM;
    
    
    static const char* BLACK;
    static const char* RED;
    static const char* GREEN;
    static const char* YELLOW;
    static const char* BLUE;
    static const char* MAGENTA;
    static const char* CYAN;
    static const char* WHITE;
    
    
    static const char* BG_BLACK;
    static const char* BG_RED;
    static const char* BG_GREEN;
    static const char* BG_YELLOW;
    static const char* BG_BLUE;
    static const char* BG_MAGENTA;
    static const char* BG_CYAN;
    static const char* BG_WHITE;
    
public:
    struct ColorScheme {
        std::string primary;
        std::string secondary;
        std::string accent;
        std::string success;
        std::string warning;
        std::string error;
        std::string info;
        std::string muted;
        std::string frame;
        std::string badge;
        std::string headline;
        std::string panelBackground;
        std::string panelForeground;
        std::string selectionBackground;
        std::string selectionForeground;
    };
    
private:
    
    bool colorsEnabled_;
    ColorScheme colorScheme_;
    static ColorScheme defaultScheme();
    std::string resolveColor(const std::string& candidate, const char* fallback) const;
    
public:
    enum class InputKey {
        Up,
        Down,
        Left,
        Right,
        Enter,
        Escape,
        Other
    };

    CLIUtils();
    
    
    void clearScreen() const;
    void moveCursor(int row, int col) const;
    void hideCursor() const;
    void showCursor() const;
    
    
    void printColored(const char* text, const char* color) const;
    void printColored(const std::string& text, const std::string& color) const;
    void printSuccess(const char* text) const;
    void printError(const char* text) const;
    void printWarning(const char* text) const;
    void printInfo(const char* text) const;
    void printHighlight(const char* text) const;
    
    
    void drawBox(int x, int y, int width, int height, const char* title = nullptr) const;
    void drawHorizontalLine(int length, char c = '=') const;
    
    
    void drawProgressBar(int current, int total, int width = 40) const;
    void drawSpinner(int frame) const;
    
    
    void waitForEnter() const;
    int getNumberInput(const char* prompt, int min, int max) const;
    void getStringInput(const char* prompt, char* buffer, int maxLen) const;
    
    
    void printCentered(const char* text, int width) const;
    void printHeader(const char* text, bool leadingBlank = true) const;
    void printSubHeader(const char* text) const;
    void printSeparator() const;
    
    
    void typewriterEffect(const char* text, int delayMs = 30) const;
    void fadeIn(const char* text) const;
    
    
    void enableColors();
    void disableColors();
    bool areColorsEnabled() const;
    void setColorScheme(const ColorScheme& scheme);
    const ColorScheme& getColorScheme() const;
    void getTerminalSize(int& rows, int& cols) const;
    int measureDisplayWidth(const std::string& text) const;
    int centerPadding(int contentWidth) const;
    InputKey readMenuKey() const;
    int selectFromList(const std::string& title,
                       const std::vector<std::string>& options,
                       int initialIndex = 0,
                       bool allowEscape = false,
                       bool wrap = true) const;
    
    
    void initNcurses();
    void endNcurses();
};

#endif
