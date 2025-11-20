#include "Maze.h"
#include "Path.h"
#include "Point.h"
#include "BFSSolver.h"
#include "DijkstraSolver.h"
#include "PathAnalyzer.h"
#include "Renderer.h"
#include "MazeGenerator.h"
#include "CLIUtils.h"
#include "GameMode.h"
#include "MazeSolverStrategy.h"
#include "Exceptions.h"
#include <iostream>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
#include <memory>
#include <unistd.h>

class MazeSolverApp {
private:
    Maze maze_;
    Path bfsPath_;
    Path dijkstraPath_;
    bool mazeLoaded_;
    bool bfsSolved_;
    bool dijkstraSolved_;
    CLIUtils cli_;
    Renderer renderer_;
    
    void showWelcome();
    int showMainMenu();
    CLIUtils::ColorScheme themeToScheme(const Renderer::ThemePalette& palette) const;
    
    
    void handleLoadMaze();
    void handleGenerateMaze();
    void handleSolveBFS();
    void handleSolveDijkstra();
    void handleAnalyzePath();
    void handleComparePaths();
    void handleVisualize();
    void handleQuickSolve();
    void handlePlayGame();
    void handleSettings();
    void applyActiveThemeToCLI();
    
public:
    MazeSolverApp();
    void run();
};

MazeSolverApp::MazeSolverApp() : mazeLoaded_(false), bfsSolved_(false), dijkstraSolved_(false) {
    applyActiveThemeToCLI();
}

CLIUtils::ColorScheme MazeSolverApp::themeToScheme(const Renderer::ThemePalette& palette) const {
    CLIUtils::ColorScheme scheme;
    scheme.primary = palette.headerSecondary;
    scheme.secondary = palette.legendValueColor;
    scheme.accent = palette.legendLabelColor;
    scheme.success = palette.cellStartColor;
    scheme.warning = palette.cellWaterColor;
    scheme.error = palette.cellGoalColor;
    scheme.info = palette.headerSecondary;
    scheme.muted = palette.statLabelColor;
    scheme.frame = palette.frameColor;
    scheme.badge = palette.statValueColor;
    scheme.headline = palette.headerPrimary;
    scheme.panelForeground = palette.legendValueColor;

    switch (renderer_.getTheme()) {
        case Renderer::ThemeId::NeonMatrix:
            scheme.panelBackground = "\033[48;2;12;18;30m";
            scheme.selectionBackground = "\033[48;2;28;52;78m";
            scheme.selectionForeground = "\033[38;2;210;255;245m";
            break;
        case Renderer::ThemeId::EmberGlow:
            scheme.panelBackground = "\033[48;2;36;20;12m";
            scheme.selectionBackground = "\033[48;2;72;32;16m";
            scheme.selectionForeground = "\033[38;2;255;230;190m";
            break;
        case Renderer::ThemeId::ArcticAurora:
            scheme.panelBackground = "\033[48;2;18;30;48m";
            scheme.selectionBackground = "\033[48;2;40;66;96m";
            scheme.selectionForeground = "\033[38;2;215;245;255m";
            break;
        case Renderer::ThemeId::Monochrome:
            scheme.panelBackground = "\033[48;2;26;26;26m";
            scheme.selectionBackground = "\033[48;2;60;60;60m";
            scheme.selectionForeground = "\033[37m";
            break;
        default:
            scheme.panelBackground = "\033[48;2;24;24;24m";
            scheme.selectionBackground = "\033[48;2;60;60;60m";
            scheme.selectionForeground = palette.headerPrimary;
            break;
    }
    return scheme;
}

void MazeSolverApp::applyActiveThemeToCLI() {
    cli_.setColorScheme(themeToScheme(renderer_.getActivePalette()));
}

void MazeSolverApp::showWelcome() {
    cli_.clearScreen();
    cli_.hideCursor();

    int rows = 0;
    int cols = 0;
    cli_.getTerminalSize(rows, cols);

    const bool colorOn = renderer_.isColorModeEnabled();
    const auto& scheme = cli_.getColorScheme();
    const auto& palette = renderer_.getActivePalette();
    const std::string reset = colorOn ? "\033[0m" : "";

    auto pickColor = [&](const std::string& candidate, const std::string& fallback) -> std::string {
        if (!colorOn) {
            return "";
        }
        if (!candidate.empty()) {
            return candidate;
        }
        if (!fallback.empty()) {
            return fallback;
        }
        return "";
    };

    const std::string fallbackHeadline = !scheme.headline.empty() ? scheme.headline : scheme.primary;
    const std::string fallbackAccent = !scheme.accent.empty() ? scheme.accent : scheme.secondary;
    const std::string fallbackInfo = !scheme.info.empty() ? scheme.info : scheme.primary;
    const std::string fallbackMuted = !scheme.muted.empty() ? scheme.muted : scheme.secondary;

    const std::string bannerBase = pickColor(palette.headerPrimary, fallbackHeadline);
    const std::string sweepColor = pickColor(palette.statValueColor, fallbackAccent);
    const std::string captionColor = pickColor(palette.legendLabelColor, fallbackAccent);
    const std::string infoColor = pickColor(palette.headerSecondary, fallbackInfo);
    const std::string shadowColor = pickColor(palette.statLabelColor, fallbackMuted);

    const std::vector<std::string> banner = {
        "███╗   ███╗ █████╗ ███████╗███████╗     ███████╗ ███╗   ██╗ ██████╗ ██╗███╗   ██╗███████╗",
        "████╗ ████║██╔══██╗╚══███╔╝██╔════╝     ██╔════╝ ████╗  ██║██╔════╝ ██║████╗  ██║██╔════╝",
        "██╔████╔██║███████║  ███╔╝ █████╗       █████╗   ██╔██╗ ██║██║  ███╗██║██╔██╗ ██║█████╗  ",
        "██║╚██╔╝██║██╔══██║ ███╔╝  ██╔══╝       ██╔══╝   ██║╚██╗██║██║   ██║██║██║╚██╗██║██╔══╝  ",
        "██║ ╚═╝ ██║██║  ██║███████╗███████╗     ███████╗ ██║ ╚████║╚██████╔╝██║██║ ╚████║███████╗",
        "╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝╚══════╝     ╚══════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚═╝╚═╝  ╚═══╝╚══════╝"
    };

    auto splitGlyphs = [&](const std::string& line) {
        std::vector<std::string> glyphs;
        for (std::size_t i = 0; i < line.size();) {
            unsigned char c = static_cast<unsigned char>(line[i]);
            std::size_t length = 1;
            if ((c & 0x80) != 0) {
                if ((c & 0xE0) == 0xC0) length = 2;
                else if ((c & 0xF0) == 0xE0) length = 3;
                else if ((c & 0xF8) == 0xF0) length = 4;
            }
            glyphs.emplace_back(line.substr(i, length));
            i += length;
        }
        return glyphs;
    };

    std::vector<std::vector<std::string>> glyphLines;
    std::vector<std::vector<int>> glyphWidths;
    std::vector<int> lineWidths;
    glyphLines.reserve(banner.size());
    glyphWidths.reserve(banner.size());
    lineWidths.reserve(banner.size());

    for (const auto& line : banner) {
        auto glyphs = splitGlyphs(line);
        std::vector<int> widths;
        widths.reserve(glyphs.size());
        int lineWidth = 0;
        for (const auto& glyph : glyphs) {
            int w = 1;
            if (!glyph.empty() && glyph[0] == '\t') {
                w = 4;
            }
            widths.push_back(w);
            lineWidth += w;
        }
        glyphLines.push_back(std::move(glyphs));
        glyphWidths.push_back(std::move(widths));
        lineWidths.push_back(lineWidth);
    }

    int bannerWidth = 0;
    for (int width : lineWidths) {
        bannerWidth = std::max(bannerWidth, width);
    }

    auto fallbackRowColor = [&](const std::string& candidate) -> std::string {
        return pickColor(candidate, bannerBase);
    };

    std::vector<std::string> rowColors = {
        fallbackRowColor(palette.headerPrimary),
        fallbackRowColor(palette.headerSecondary),
        fallbackRowColor(palette.legendLabelColor),
        fallbackRowColor(palette.legendValueColor),
        fallbackRowColor(palette.statLabelColor),
        fallbackRowColor(palette.statValueColor)
    };

    if (rowColors.empty()) {
        rowColors.push_back(bannerBase);
    }

    int topPadding = std::max(1, (rows - static_cast<int>(banner.size()) - 12) / 3);
    int leftPadding = std::max(0, cli_.centerPadding(bannerWidth));
    const int sweepWidth = std::max(10, bannerWidth / 12);

    for (int sweep = -sweepWidth; sweep < bannerWidth + sweepWidth; sweep += 2) {
        cli_.clearScreen();
        for (int i = 0; i < topPadding; ++i) {
            std::cout << "\n";
        }

        for (std::size_t row = 0; row < glyphLines.size(); ++row) {
            std::cout << std::string(static_cast<std::size_t>(leftPadding), ' ');
            const auto& glyphs = glyphLines[row];
            const auto& widths = glyphWidths[row];
            int column = 0;
            const std::string& rowColor = rowColors[row % rowColors.size()];
            for (std::size_t g = 0; g < glyphs.size(); ++g) {
                bool highlight = (column + widths[g]) > sweep && column < sweep + sweepWidth;
                const std::string& colorToUse = highlight ? sweepColor : rowColor;
                if (colorOn && !colorToUse.empty()) {
                    std::cout << colorToUse << glyphs[g] << reset;
                } else {
                    std::cout << glyphs[g];
                }
                column += widths[g];
            }
            std::cout << "\n";
        }

        usleep(42000);
    }

    cli_.clearScreen();
    for (int i = 0; i < topPadding; ++i) {
        std::cout << "\n";
    }

    for (std::size_t row = 0; row < glyphLines.size(); ++row) {
        std::cout << std::string(static_cast<std::size_t>(leftPadding), ' ');
        const auto& glyphs = glyphLines[row];
        const std::string& rowColor = rowColors[row % rowColors.size()];
        for (const auto& glyph : glyphs) {
            if (colorOn && !rowColor.empty()) {
                std::cout << rowColor << glyph << reset;
            } else {
                std::cout << glyph;
            }
        }
        std::cout << "\n";
    }

    if (colorOn && !shadowColor.empty()) {
        std::string underline(static_cast<std::size_t>(bannerWidth), '-');
        std::cout << std::string(static_cast<std::size_t>(leftPadding), ' ')
                  << shadowColor << underline << reset << "\n";
    }

    std::cout << "\n";
    auto printCenteredColored = [&](const std::string& text, const std::string& color) {
        int pad = cli_.centerPadding(cli_.measureDisplayWidth(text));
        std::cout << std::string(static_cast<std::size_t>(pad), ' ');
        if (colorOn && !color.empty()) {
            std::cout << color << text << reset << "\n";
        } else {
            std::cout << text << "\n";
        }
    };

    printCenteredColored("MAZE ENGINE v2", captionColor);
    printCenteredColored("Procedural Labyrinths • Intelligent Routing • Immersive Play", infoColor);

    std::cout << "\n";
    cli_.printSeparator();
    std::cout << "\n";

    const std::vector<std::string> bootSteps = {
        "Initializing Maze Engine core...",
        "Awakening solver heuristics...",
        "Optimizing path analytics pipeline...",
        "Linking interactive game systems..."
    };

    for (const auto& step : bootSteps) {
        int stepPad = cli_.centerPadding(cli_.measureDisplayWidth(step));
        std::string line(static_cast<std::size_t>(stepPad), ' ');
        line += step;
        cli_.typewriterEffect(line.c_str(), 14);
        usleep(90000);
    }

    const std::string successLine = "✓ Maze Engine online!";
    int successPad = cli_.centerPadding(cli_.measureDisplayWidth(successLine));
    std::cout << std::string(static_cast<std::size_t>(successPad), ' ');
    cli_.printSuccess("Maze Engine online!");

    std::cout << "\n";
    printCenteredColored("Press Enter to launch the control room...", captionColor);

    cli_.showCursor();
    cli_.waitForEnter();
}

int MazeSolverApp::showMainMenu() {
    struct MenuItem {
        int id;
        std::string label;
        std::string badge;
    };

    struct MenuSection {
        std::string title;
        std::vector<MenuItem> items;
    };

    const std::vector<MenuSection> sections = {
        {">> Maze Operations",
         {{1, "Load Maze from File", ""},
          {2, "Generate New Maze", "(Instant!)"},
          {3, "Quick Solve", "(Generate + Solve)"}}},
        {">> Solving Algorithms",
         {{4, "Solve with BFS", ""},
          {5, "Solve with Dijkstra", ""},
          {6, "Solve with Both Algorithms", ""}}},
        {">> Analysis & Visualization",
         {{7, "Analyze Current Path", ""},
          {8, "Compare Both Solutions", ""},
          {9, "Animated Visualization", ""},
          {10, "Display Maze", ""}}},
        {">> Interactive Mode",
         {{11, "Play Maze Game", "(Arrow Keys!)"}}},
        {">> Other Options",
         {{12, "Settings", ""},
          {0, "Exit", ""}}}
    };

    struct DisplayItem {
        const MenuItem* ref;
        std::string number;
        std::string text;
        std::string badge;
        int textWidth;
    };

    std::vector<DisplayItem> displayItems;
    displayItems.reserve(13);

    const int pointerArea = 3;
    int baseWidth = pointerArea + cli_.measureDisplayWidth("MAIN MENU");

    auto updateWidth = [&](int width) {
        if (width > baseWidth) {
            baseWidth = width;
        }
    };

    for (const auto& section : sections) {
        updateWidth(pointerArea + cli_.measureDisplayWidth(section.title));
        for (const auto& entry : section.items) {
            std::string number = (entry.id < 10 ? std::string(" ") : std::string()) + std::to_string(entry.id) + ".";
            std::string combined = number + " " + entry.label;
            if (!entry.badge.empty()) {
                combined += " " + entry.badge;
            }
            int textWidth = cli_.measureDisplayWidth(combined);
            updateWidth(pointerArea + textWidth);
            displayItems.push_back(DisplayItem{&entry, number, entry.label, entry.badge, textWidth});
        }
        updateWidth(pointerArea); 
    }

    updateWidth(pointerArea + cli_.measureDisplayWidth("Session Status"));

    int selected = 0;

    while (true) {
        cli_.clearScreen();

        const bool colorOn = renderer_.isColorModeEnabled();
        const auto& scheme = cli_.getColorScheme();
        const std::string reset = colorOn ? "\033[0m" : "";

        struct StatusEntry {
            std::string text;
            std::string color;
        };

        std::vector<StatusEntry> statusLines;
        if (mazeLoaded_) {
            statusLines.push_back({"✓ Maze Loaded", scheme.success});
            if (bfsSolved_) {
                statusLines.push_back({"✓ BFS Solved", scheme.success.empty() ? scheme.primary : scheme.success});
            }
            if (dijkstraSolved_) {
                statusLines.push_back({"✓ Dijkstra Solved", scheme.success.empty() ? scheme.primary : scheme.success});
            }
        } else {
            statusLines.push_back({"⚠ No Maze Loaded", scheme.warning});
        }

        int boxWidth = baseWidth;
        for (const auto& status : statusLines) {
            int width = pointerArea + cli_.measureDisplayWidth(status.text);
            if (width > boxWidth) {
                boxWidth = width;
            }
        }
        boxWidth = std::max(boxWidth, 64);

        const int borderWidth = boxWidth + 2;
        const int leftPad = cli_.centerPadding(borderWidth);

        int rows = 0;
        int cols = 0;
        cli_.getTerminalSize(rows, cols);

        const int sectionCount = static_cast<int>(sections.size());
        const int totalItems = static_cast<int>(displayItems.size());
        const int statusCount = static_cast<int>(statusLines.size());
        const int boxLines = 5 + sectionCount * 2 + totalItems + statusCount;
        const int contentHeight = boxLines + 1 + 1; 
        const int topPadding = std::max(0, (rows - contentHeight) / 2);

        for (int i = 0; i < topPadding; ++i) {
            std::cout << "\n";
        }

        auto printHorizontal = [&](const std::string& left, const std::string& fill, const std::string& right) {
            std::cout << std::string(leftPad, ' ');
            if (colorOn && !scheme.frame.empty()) std::cout << scheme.frame;
            std::cout << left;
            for (int i = 0; i < boxWidth; ++i) std::cout << fill;
            std::cout << right;
            if (colorOn && !scheme.frame.empty()) std::cout << reset;
            std::cout << "\n";
        };

        auto beginLine = [&]() {
            std::cout << std::string(leftPad, ' ');
            if (colorOn && !scheme.frame.empty()) std::cout << scheme.frame;
            std::cout << "║";
            if (colorOn && !scheme.frame.empty()) std::cout << reset;

            if (colorOn && !scheme.panelBackground.empty()) {
                std::cout << scheme.panelBackground;
                if (!scheme.panelForeground.empty()) std::cout << scheme.panelForeground;
            }
        };

        auto endLine = [&]() {
            if (colorOn) std::cout << reset;
            if (colorOn && !scheme.frame.empty()) std::cout << scheme.frame;
            std::cout << "║";
            if (colorOn && !scheme.frame.empty()) std::cout << reset;
            std::cout << "\n";
        };

        auto printStaticLine = [&](const std::string& text, const std::string& color, bool center = false) {
            int textWidth = cli_.measureDisplayWidth(text);
            int available = boxWidth - pointerArea;
            int leftSpaces = pointerArea;
            if (center && available > textWidth) {
                leftSpaces += (available - textWidth) / 2;
            }
            int rightSpaces = boxWidth - leftSpaces - textWidth;
            if (rightSpaces < 0) {
                rightSpaces = 0;
            }

            beginLine();
            std::cout << std::string(leftSpaces, ' ');
            if (colorOn && !color.empty()) {
                std::cout << color;
            }
            std::cout << text;
            if (colorOn && !scheme.panelForeground.empty()) {
                std::cout << scheme.panelForeground;
            }
            std::cout << std::string(rightSpaces, ' ');
            endLine();
        };

        auto printMenuItemLine = [&](const DisplayItem& display, bool isSelected) {
            beginLine();

            if (colorOn) {
                const std::string& bg = isSelected ? scheme.selectionBackground : scheme.panelBackground;
                if (!bg.empty()) std::cout << bg;
                const std::string& fg = isSelected ? scheme.selectionForeground : scheme.panelForeground;
                if (!fg.empty()) std::cout << fg;
            }

            if (colorOn && !scheme.accent.empty()) {
                std::cout << (isSelected ? scheme.accent : scheme.muted);
            }
            std::cout << " " << (isSelected ? "➤" : "•") << " ";
            if (colorOn && !scheme.panelForeground.empty()) {
                std::cout << (isSelected ? scheme.selectionForeground : scheme.panelForeground);
            }

            if (colorOn && !scheme.accent.empty()) {
                std::cout << scheme.accent;
            }
            std::cout << display.number;
            if (colorOn && !scheme.panelForeground.empty()) {
                std::cout << scheme.panelForeground;
            }
            std::cout << " " << display.text;
            if (!display.badge.empty()) {
                std::cout << " ";
                if (colorOn && !scheme.badge.empty()) {
                    std::cout << scheme.badge;
                }
                std::cout << display.badge;
                if (colorOn && !scheme.panelForeground.empty()) {
                    std::cout << scheme.panelForeground;
                }
            }

            int contentWidth = pointerArea + display.textWidth;
            int remaining = boxWidth - contentWidth;
            if (remaining > 0) {
                std::cout << std::string(remaining, ' ');
            }

            endLine();
        };

        printHorizontal("╔", "═", "╗");
        printStaticLine("MAIN MENU", scheme.headline.empty() ? scheme.primary : scheme.headline, true);
        printHorizontal("╠", "═", "╣");

        std::size_t displayIndex = 0;
        for (const auto& section : sections) {
            printStaticLine(section.title, scheme.accent, false);
            for (std::size_t i = 0; i < section.items.size(); ++i) {
                bool isSelected = static_cast<int>(displayIndex) == selected;
                printMenuItemLine(displayItems[displayIndex], isSelected);
                ++displayIndex;
            }
            printStaticLine("", scheme.panelForeground, false);
        }

        printStaticLine("Session Status", scheme.accent, false);
        for (const auto& status : statusLines) {
            printStaticLine(status.text, status.color, false);
        }

        printHorizontal("╚", "═", "╝");

        std::string instructions = "Use ↑/↓ to navigate, Enter to select, Esc to exit";
        int instructionsWidth = cli_.measureDisplayWidth(instructions);
        int instructionPad = cli_.centerPadding(instructionsWidth);
        std::cout << "\n" << std::string(instructionPad, ' ');
        if (colorOn && !scheme.panelForeground.empty()) {
            std::cout << scheme.panelForeground;
        }
        std::cout << instructions;
        if (colorOn) {
            std::cout << reset;
        }
        std::cout << std::flush;

        CLIUtils::InputKey key = cli_.readMenuKey();
        if (key == CLIUtils::InputKey::Enter) {
            return displayItems[static_cast<std::size_t>(selected)].ref->id;
        } else if (key == CLIUtils::InputKey::Escape) {
            return 0;
        } else if (key == CLIUtils::InputKey::Up) {
            if (selected > 0) {
                --selected;
            } else {
                selected = static_cast<int>(displayItems.size()) - 1;
            }
        } else if (key == CLIUtils::InputKey::Down) {
            selected = (selected + 1) % static_cast<int>(displayItems.size());
        }
    }
}

void MazeSolverApp::handleLoadMaze() {
    cli_.clearScreen();
    cli_.printHeader("Load Maze from File");
    
    char filename[256];
    cli_.getStringInput("Enter filename: ", filename, 256);
    
    std::cout << "\n";
    cli_.drawSpinner(0);
    std::cout << " Loading maze...\r";
    std::cout.flush();
    
    try {
        if (!maze_.loadFromFile(filename)) {
            throw MazeException(std::string("Failed to load maze from file: ") + filename);
        }

        cli_.printSuccess("Maze loaded successfully!");
        std::cout << "\n";
        std::cout << "  Dimensions: " << maze_.getWidth() << "x" << maze_.getHeight() << "\n";
        std::cout << "  Start: " << maze_.getStart() << "\n";
        std::cout << "  Goal: " << maze_.getGoal() << "\n";

        mazeLoaded_ = true;
        bfsSolved_ = false;
        dijkstraSolved_ = false;
    } catch (const MazeException& ex) {
        cli_.printError(ex.what());
    }
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleGenerateMaze() {
    std::vector<std::string> options = {
        "Small (15x15) · Easy",
        "Medium (25x25) · Normal",
        "Large (35x35) · Hard",
        "Custom size"
    };

    int selection = cli_.selectFromList("Select Maze Size", options, 1, true, false);
    if (selection == -1) {
        cli_.printInfo("Maze generation canceled.");
        cli_.waitForEnter();
        return;
    }

    cli_.clearScreen();
    cli_.printHeader("Generate New Maze");

    int seed = static_cast<int>(time(nullptr));
    MazeGenerator generator(20, 20, seed);

    std::cout << "\n";
    for (int i = 0; i < 10; i++) {
        cli_.drawSpinner(i);
        std::cout << " Generating perfect maze...\r";
        std::cout.flush();
        usleep(50000);
    }

    switch (selection) {
        case 0:
            maze_ = generator.generateEasy();
            break;
        case 1:
            maze_ = generator.generateMedium();
            break;
        case 2:
            maze_ = generator.generateHard();
            break;
        case 3: {
            int w = cli_.getNumberInput("Enter width (5-50): ", 5, 50);
            int h = cli_.getNumberInput("Enter height (5-50): ", 5, 50);
            generator.setDimensions(w, h);
            maze_ = generator.generatePerfect();
            break;
        }
    }

    std::cout << "\n";
    cli_.printSuccess("Maze generated successfully!");
    std::cout << "\n  Dimensions: " << maze_.getWidth() << "x" << maze_.getHeight() << "\n";

    mazeLoaded_ = true;
    bfsSolved_ = false;
    dijkstraSolved_ = false;

    std::cout << "\n";
    cli_.printSubHeader("Preview");
    renderer_.render(maze_);

    cli_.waitForEnter();
}

void MazeSolverApp::handleSolveBFS() {
    if (!mazeLoaded_) {
        cli_.printError("Please load or generate a maze first!");
        cli_.waitForEnter();
        return;
    }
    
    cli_.clearScreen();
    cli_.printHeader("BFS Solver");
    
    std::cout << "\n";
    BFSSolver solver;
    
    for (int i = 0; i < 20; i++) {
        cli_.drawSpinner(i);
        std::cout << " Exploring maze with BFS...\r";
        std::cout.flush();
        usleep(30000);
    }
    
    bfsPath_ = solver.solve(maze_);
    
    std::cout << "\n";
    if (bfsPath_.isEmpty()) {
        cli_.printError("No path found!");
    } else {
        cli_.printSuccess("Path found!");
        std::cout << "\n";
        int bfsSteps = std::max(0, bfsPath_.getSize() - 1);
        std::cout << "  Path length: " << bfsSteps << " steps\n";
        std::cout << "  Nodes explored: " << solver.getNodesExplored() << "\n";
        std::cout << "  Cost: " << bfsPath_.getCost() << "\n";
        bfsSolved_ = true;
        
        std::cout << "\n";
        cli_.printSubHeader("Path Preview");
        renderer_.render(maze_, bfsPath_, "BFS");
    }
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleSolveDijkstra() {
    if (!mazeLoaded_) {
        cli_.printError("Please load or generate a maze first!");
        cli_.waitForEnter();
        return;
    }
    
    cli_.clearScreen();
    cli_.printHeader("Dijkstra Solver");
    
    std::cout << "\n";
    DijkstraSolver solver;
    
    for (int i = 0; i < 20; i++) {
        cli_.drawSpinner(i);
        std::cout << " Finding optimal path with Dijkstra...\r";
        std::cout.flush();
        usleep(30000);
    }
    
    dijkstraPath_ = solver.solve(maze_);
    
    std::cout << "\n";
    if (dijkstraPath_.isEmpty()) {
        cli_.printError("No path found!");
    } else {
        cli_.printSuccess("Path found!");
        std::cout << "\n";
        int dijkstraSteps = std::max(0, dijkstraPath_.getSize() - 1);
        std::cout << "  Path length: " << dijkstraSteps << " steps\n";
        std::cout << "  Nodes explored: " << solver.getNodesExplored() << "\n";
        std::cout << "  Cost: " << dijkstraPath_.getCost() << "\n";
        dijkstraSolved_ = true;
        
        std::cout << "\n";
        cli_.printSubHeader("Path Preview");
        renderer_.render(maze_, dijkstraPath_, "Dijkstra");
    }
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleAnalyzePath() {
    if (!bfsSolved_ && !dijkstraSolved_) {
        cli_.printError("Please solve the maze first!");
        cli_.waitForEnter();
        return;
    }
    
    cli_.clearScreen();
    cli_.printHeader("Path Analysis");
    
    Path* pathToAnalyze = bfsSolved_ ? &bfsPath_ : &dijkstraPath_;
    
    PathAnalyzer analyzer;
    PathMetrics metrics = analyzer.analyze(*pathToAnalyze, maze_);
    
    std::cout << "\n";
    cli_.printSubHeader("Detailed Metrics");
    metrics.display();
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleComparePaths() {
    if (!bfsSolved_ || !dijkstraSolved_) {
        cli_.printError("Please solve with both algorithms first!");
        cli_.waitForEnter();
        return;
    }
    
    cli_.clearScreen();
    cli_.printHeader("Path Comparison");
    
    PathAnalyzer analyzer;
    
    std::cout << "\n";
    cli_.printSubHeader("BFS Path Analysis");
    PathMetrics bfsMetrics = analyzer.analyze(bfsPath_, maze_);
    bfsMetrics.display();
    
    std::cout << "\n";
    cli_.printSubHeader("Dijkstra Path Analysis");
    PathMetrics dijkstraMetrics = analyzer.analyze(dijkstraPath_, maze_);
    dijkstraMetrics.display();
    
    std::cout << "\n";
    cli_.printSubHeader("Comparison Results");
    analyzer.comparePaths(bfsPath_, dijkstraPath_, maze_);
    
    std::cout << "\n";
    cli_.printSubHeader("Visual Comparison");
    renderer_.renderComparison(maze_, bfsPath_, dijkstraPath_, "BFS", "Dijkstra");
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleVisualize() {
    if (!mazeLoaded_) {
        cli_.printError("Please load or generate a maze first!");
        cli_.waitForEnter();
        return;
    }

    std::vector<std::string> options = {
        "Display maze only",
        "Display with BFS path",
        "Display with Dijkstra path",
        "Animated BFS solving",
        "Animated Dijkstra solving",
        "Compare both paths"
    };

    int selection = cli_.selectFromList("Visualization Options", options, 0, true, false);
    if (selection == -1) {
        return;
    }

    cli_.clearScreen();

    switch (selection) {
        case 0:
            cli_.printHeader("Maze View");
            renderer_.render(maze_);
            break;
        case 1:
            if (bfsSolved_) {
                cli_.printHeader("BFS Path");
                renderer_.render(maze_, bfsPath_, "BFS");
            } else {
                cli_.printError("BFS not solved yet!");
            }
            break;
        case 2:
            if (dijkstraSolved_) {
                cli_.printHeader("Dijkstra Path");
                renderer_.render(maze_, dijkstraPath_, "Dijkstra");
            } else {
                cli_.printError("Dijkstra not solved yet!");
            }
            break;
        case 3:
            if (bfsSolved_) {
                renderer_.renderAnimated(maze_, bfsPath_, 80);
            } else {
                cli_.printError("BFS not solved yet!");
            }
            break;
        case 4:
            if (dijkstraSolved_) {
                renderer_.renderAnimated(maze_, dijkstraPath_, 80);
            } else {
                cli_.printError("Dijkstra not solved yet!");
            }
            break;
        case 5:
            if (bfsSolved_ && dijkstraSolved_) {
                cli_.printHeader("Path Comparison");
                renderer_.renderComparison(maze_, bfsPath_, dijkstraPath_, "BFS", "Dijkstra");
            } else {
                cli_.printError("Please solve with both algorithms first!");
            }
            break;
    }

    cli_.waitForEnter();
}

void MazeSolverApp::handleQuickSolve() {
    cli_.clearScreen();
    cli_.printHeader("Quick Solve");
    
    std::cout << "\n";
    cli_.printInfo("Generating maze...");
    
    int seed = time(nullptr);
    MazeGenerator generator(20, 20, seed);
    maze_ = generator.generateMedium();
    mazeLoaded_ = true;
    
    cli_.printSuccess("Maze generated!");
    
    std::cout << "\n";

    std::vector<std::unique_ptr<MazeSolverStrategy>> strategies;
    strategies.emplace_back(std::unique_ptr<MazeSolverStrategy>(new BFSSolver()));
    strategies.emplace_back(std::unique_ptr<MazeSolverStrategy>(new DijkstraSolver()));

    for (auto& strategy : strategies) {
        const std::string solverName = strategy->name();
        const std::string infoMessage = "Solving with " + solverName + "...";
        cli_.printInfo(infoMessage.c_str());

        Path solvedPath = strategy->solve(maze_);

        if (solvedPath.isEmpty()) {
            const std::string warnMessage = solverName + " could not find a path.";
            cli_.printWarning(warnMessage.c_str());
            continue;
        }

        const std::string successMessage = solverName + " complete!";
        cli_.printSuccess(successMessage.c_str());
        std::cout << "  Path length: " << std::max(0, solvedPath.getSize() - 1) << " steps\n";
        std::cout << "  Nodes explored: " << strategy->getNodesExplored() << "\n";
        std::cout << "  Cost: " << solvedPath.getCost() << "\n\n";

        if (solverName.find("Breadth") != std::string::npos) {
            bfsPath_ = solvedPath;
            bfsSolved_ = true;
        } else {
            dijkstraPath_ = solvedPath;
            dijkstraSolved_ = true;
        }
    }
    
    std::cout << "\n";
    cli_.printHeader("Results");
    
    int bfsSteps = std::max(0, bfsPath_.getSize() - 1);
    int dijkstraSteps = std::max(0, dijkstraPath_.getSize() - 1);

    std::cout << "\n  BFS: " << bfsSteps << " steps, cost " << bfsPath_.getCost() << "\n";
    std::cout << "  Dijkstra: " << dijkstraSteps << " steps, cost " << dijkstraPath_.getCost() << "\n";
    
    std::cout << "\n";
    renderer_.renderComparison(maze_, bfsPath_, dijkstraPath_, "BFS", "Dijkstra");
    
    cli_.waitForEnter();
}

void MazeSolverApp::handlePlayGame() {
    cli_.clearScreen();
    cli_.printHeader(">> Interactive Game Mode");
    
    std::cout << "\n";
    
    
    if (!mazeLoaded_) {
        cli_.printInfo("No maze loaded. Let's generate one for you!");

        std::vector<std::string> difficultyOptions = {
            "Easy (15x15)",
            "Medium (25x25)",
            "Hard (35x35)",
            "Cancel"
        };

        int difficulty = cli_.selectFromList("Choose difficulty", difficultyOptions, 1, true, false);
        if (difficulty == -1 || difficulty == 3) {
            cli_.printInfo("Game cancelled.");
            cli_.waitForEnter();
            return;
        }

        std::cout << "\n";
        cli_.drawSpinner(0);
        std::cout << " Generating game maze...\r";
        std::cout.flush();

        MazeGenerator generator(1,1); 
        switch (difficulty) {
            case 0: 
                generator.setDimensions(15, 15);
                maze_ = generator.generateEasy();
                break;
            case 1: 
                generator.setDimensions(25, 25);
                maze_ = generator.generateMedium();
                break;
            case 2: 
                generator.setDimensions(35, 35);
                maze_ = generator.generateHard();
                break;
        }

        mazeLoaded_ = true;
        cli_.printSuccess("Game maze generated!");
        usleep(1500000); 
    }
    
    
    cli_.clearScreen();
    cli_.printHeader("Entering Game Mode");
    cli_.printInfo("The screen will now switch to the interactive game.");
    cli_.printInfo("Use arrow keys or WASD to move. Press 'Q' to quit.");
    cli_.printSuccess("Good luck, have fun!");
    usleep(4000000); 

    
    GameMode game(maze_, renderer_, cli_);
    game.startGame();
    
    
    cli_.clearScreen();
    cli_.showCursor(); 
    cli_.printHeader("Game Session Ended");

    if (game.hasWon()) {
        cli_.printSuccess("You have returned to the main menu.");
    } else {
        cli_.printInfo("Game exited. You have returned to the main menu.");
    }
    
    cli_.waitForEnter();
}

void MazeSolverApp::handleSettings() {
    int initialIndex = 0;

    while (true) {
        std::vector<std::string> options;
        options.push_back(std::string(cli_.areColorsEnabled() ? "Toggle colors · currently: ON"
                                                              : "Toggle colors · currently: OFF"));

        std::string saveLabel = "Save current maze";
        if (mazeLoaded_) {
            saveLabel += " · " + std::to_string(maze_.getWidth()) + "x" + std::to_string(maze_.getHeight());
        } else {
            saveLabel += " · requires maze";
        }
        options.push_back(saveLabel);

        options.emplace_back("About this project");

        options.push_back("Change theme · current: " + renderer_.getThemeName());
        options.emplace_back("Back to main menu");

        int selection = cli_.selectFromList("Settings", options, initialIndex, true, false);
        if (selection == -1 || selection == static_cast<int>(options.size()) - 1) {
            return;
        }

        initialIndex = selection;

        switch (selection) {
            case 0: {
                bool wasEnabled = cli_.areColorsEnabled();
                cli_.clearScreen();
                cli_.printHeader("Settings · Colors");

                if (wasEnabled) {
                    cli_.disableColors();
                    renderer_.setColorMode(false);
                    applyActiveThemeToCLI();
                    cli_.printInfo("Colors disabled");
                } else {
                    cli_.enableColors();
                    renderer_.setColorMode(true);
                    applyActiveThemeToCLI();
                    cli_.printSuccess("Colors enabled");
                }

                cli_.waitForEnter();
                break;
            }
            case 1: {
                cli_.clearScreen();
                cli_.printHeader("Save Maze");

                if (mazeLoaded_) {
                    char filename[256];
                    cli_.getStringInput("Enter filename to save: ", filename, 256);
                    if (maze_.saveToFile(filename)) {
                        cli_.printSuccess("Maze saved successfully!");
                    } else {
                        cli_.printError("Failed to save maze!");
                    }
                } else {
                    cli_.printError("No maze loaded!");
                }

                cli_.waitForEnter();
                break;
            }
            case 2: {
                cli_.clearScreen();
                cli_.printHeader("About");
                std::cout << "\n";
                std::cout << "  Maze Solver & Path Analyzer\n";
                std::cout << "  Version 2.0\n";
                std::cout << "\n";
                std::cout << "  Demonstrates OOP Concepts:\n";
                std::cout << "    • Encapsulation\n";
                std::cout << "    • Abstraction\n";
                std::cout << "    • Operator Overloading\n";
                std::cout << "\n";
                std::cout << "  Features:\n";
                std::cout << "    • Instant maze generation\n";
                std::cout << "    • BFS & Dijkstra algorithms\n";
                std::cout << "    • Animated visualization\n";
                std::cout << "    • Path analysis & comparison\n";
                cli_.waitForEnter();
                break;
            }
            case 3: {
                const auto themes = Renderer::listAvailableThemes();
                if (themes.empty()) {
                    cli_.clearScreen();
                    cli_.printHeader("Themes");
                    cli_.printError("No themes available!");
                    cli_.waitForEnter();
                    break;
                }

                std::vector<std::string> themeOptions;
                themeOptions.reserve(themes.size());

                const bool showColors = renderer_.isColorModeEnabled() && cli_.areColorsEnabled();
                const std::string reset = "\033[0m";
                for (std::size_t i = 0; i < themes.size(); ++i) {
                    std::string line = themes[i];
                    if (static_cast<std::size_t>(renderer_.getTheme()) == i) {
                        line += " · current";
                    }

                    if (showColors) {
                        const Renderer::ThemePalette& preview = Renderer::getThemePalette(i);
                        line += "   " + preview.cellWallColor + "██" + reset;
                        line += " " + preview.cellPathColor + "◆◆" + reset;
                        line += " " + preview.cellStartColor + "▶ " + reset;
                        line += " " + preview.cellGoalColor + "★ " + reset;
                    }

                    themeOptions.push_back(line);
                }

                int currentTheme = static_cast<int>(renderer_.getTheme());
                int picked = cli_.selectFromList("Select Theme", themeOptions,
                                                 std::max(0, currentTheme), true, true);
                if (picked >= 0 && picked < static_cast<int>(themes.size())) {
                    renderer_.setThemeByIndex(static_cast<std::size_t>(picked));
                    applyActiveThemeToCLI();

                    cli_.clearScreen();
                    cli_.printHeader("Theme Updated");
                    std::string message = "Theme switched to " + renderer_.getThemeName();
                    cli_.printSuccess(message.c_str());

                    if (renderer_.isColorModeEnabled() && cli_.areColorsEnabled()) {
                        const Renderer::ThemePalette& activePreview = renderer_.getActivePalette();
                        std::cout << "\n  Sample preview:\n";
                        std::cout << "    "
                                  << activePreview.cellWallColor << "██" << reset << " Walls   "
                                  << activePreview.cellPathColor << "◆◆" << reset << " Path   "
                                  << activePreview.cellStartColor << "▶ " << reset << " Start   "
                                  << activePreview.cellGoalColor << "★ " << reset << " Goal\n";
                        std::cout << "    " << activePreview.headerPrimary << "Accent text" << reset
                                  << "  " << activePreview.legendLabelColor << "Legend" << reset
                                  << "  " << activePreview.statLabelColor << "Stats" << reset << "\n";
                    }

                    cli_.waitForEnter();
                }
                break;
            }
            default:
                return;
        }
    }
}

void MazeSolverApp::run() {
    showWelcome();
    
    while (true) {
        int choice = showMainMenu();
        
        switch(choice) {
            case 0:
                cli_.clearScreen();
                cli_.printSuccess("Thank you for using Maze Solver!");
                std::cout << "\n";
                return;
            case 1:
                handleLoadMaze();
                break;
            case 2:
                handleGenerateMaze();
                break;
            case 3:
                handleQuickSolve();
                break;
            case 4:
                handleSolveBFS();
                break;
            case 5:
                handleSolveDijkstra();
                break;
            case 6:
                handleSolveBFS();
                handleSolveDijkstra();
                break;
            case 7:
                handleAnalyzePath();
                break;
            case 8:
                handleComparePaths();
                break;
            case 9:
                handleVisualize();
                break;
            case 10:
                if (mazeLoaded_) {
                    cli_.clearScreen();
                    cli_.printHeader("Maze Display");
                    renderer_.render(maze_);
                    cli_.waitForEnter();
                } else {
                    cli_.printError("No maze loaded!");
                    cli_.waitForEnter();
                }
                break;
            case 11:
                handlePlayGame();
                break;
            case 12:
                handleSettings();
                break;
        }
    }
}

int main() {
    MazeSolverApp app;
    app.run();
    return 0;
}
