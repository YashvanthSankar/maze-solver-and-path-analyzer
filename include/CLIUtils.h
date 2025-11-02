#ifndef CLIUTILS_H
#define CLIUTILS_H

#include <iostream>

// CLI Utilities class for enhanced user experience (Encapsulation)
class CLIUtils {
private:
    static const char* RESET;
    static const char* BOLD;
    static const char* DIM;
    
    // Colors
    static const char* BLACK;
    static const char* RED;
    static const char* GREEN;
    static const char* YELLOW;
    static const char* BLUE;
    static const char* MAGENTA;
    static const char* CYAN;
    static const char* WHITE;
    
    // Background colors
    static const char* BG_BLACK;
    static const char* BG_RED;
    static const char* BG_GREEN;
    static const char* BG_YELLOW;
    static const char* BG_BLUE;
    static const char* BG_MAGENTA;
    static const char* BG_CYAN;
    static const char* BG_WHITE;
    
    // Encapsulated state
    bool colorsEnabled_;
    
public:
    CLIUtils();
    
    // Screen control
    void clearScreen() const;
    void moveCursor(int row, int col) const;
    void hideCursor() const;
    void showCursor() const;
    
    // Color output
    void printColored(const char* text, const char* color) const;
    void printSuccess(const char* text) const;
    void printError(const char* text) const;
    void printWarning(const char* text) const;
    void printInfo(const char* text) const;
    void printHighlight(const char* text) const;
    
    // Box drawing
    void drawBox(int x, int y, int width, int height, const char* title = nullptr) const;
    void drawHorizontalLine(int length, char c = '=') const;
    
    // Progress indicators
    void drawProgressBar(int current, int total, int width = 40) const;
    void drawSpinner(int frame) const;
    
    // Input helpers
    void waitForEnter() const;
    int getNumberInput(const char* prompt, int min, int max) const;
    void getStringInput(const char* prompt, char* buffer, int maxLen) const;
    
    // Formatting
    void printCentered(const char* text, int width) const;
    void printHeader(const char* text) const;
    void printSubHeader(const char* text) const;
    void printSeparator() const;
    
    // Animations
    void typewriterEffect(const char* text, int delayMs = 30) const;
    void fadeIn(const char* text) const;
    
    // Color control
    void enableColors();
    void disableColors();
    bool areColorsEnabled() const;
};

#endif
