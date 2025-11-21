#include <ncurses.h>
#include "Renderer.h"
#include "PathAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unistd.h>

namespace {
    constexpr int CELL_PRINT_WIDTH = 2;
    const std::string RESET_CODE = "\033[0m";
    const std::string NO_COLOR = "";

    const Renderer::ThemePalette THEME_PRESETS[] = {
        {
            "Neon Matrix",
            "\033[1;38;2;0;255;220m",
            "\033[38;2;120;210;255m",
            "\033[38;2;0;255;200m",
            "\033[1;38;2;140;200;255m",
            "\033[38;2;200;230;255m",
            "\033[38;2;160;210;255m",
            "\033[1;38;2;255;109;255m",
            "\033[48;2;12;16;28m\033[38;2;0;255;220m",
            "\033[48;2;10;20;35m\033[38;2;120;160;200m",
            "\033[48;2;18;32;46m\033[38;2;210;255;245m",
            "\033[48;2;18;32;46m\033[38;2;160;235;255m",
            "\033[48;2;10;30;32m\033[38;2;120;255;160m",
            "\033[48;2;32;10;28m\033[38;2;255;140;140m",
            "\033[48;2;10;30;45m\033[38;2;80;220;255m",
            "\033[48;2;35;25;15m\033[38;2;255;190;110m"
        },
        {
            "Ember Glow",
            "\033[1;38;2;255;180;90m",
            "\033[38;2;255;220;180m",
            "\033[38;2;255;150;70m",
            "\033[1;38;2;255;190;120m",
            "\033[38;2;255;225;200m",
            "\033[38;2;255;210;170m",
            "\033[1;38;2;255;120;90m",
            "\033[48;2;40;16;8m\033[38;2;255;160;80m",
            "\033[48;2;30;18;12m\033[38;2;255;210;160m",
            "\033[48;2;55;22;12m\033[38;2;255;230;200m",
            "\033[48;2;55;22;12m\033[38;2;255;210;170m",
            "\033[48;2;24;32;16m\033[38;2;180;255;160m",
            "\033[48;2;40;16;16m\033[38;2;255;180;150m",
            "\033[48;2;18;30;38m\033[38;2;120;200;255m",
            "\033[48;2;50;30;10m\033[38;2;255;220;140m"
        },
        {
            "Arctic Aurora",
            "\033[1;38;2;180;220;255m",
            "\033[38;2;200;235;255m",
            "\033[38;2;120;190;255m",
            "\033[1;38;2;170;220;255m",
            "\033[38;2;210;240;255m",
            "\033[38;2;180;220;255m",
            "\033[1;38;2;120;215;255m",
            "\033[48;2;16;28;48m\033[38;2;140;200;255m",
            "\033[48;2;12;24;40m\033[38;2;200;230;255m",
            "\033[48;2;20;36;58m\033[38;2;210;255;255m",
            "\033[48;2;20;36;58m\033[38;2;190;245;255m",
            "\033[48;2;18;32;48m\033[38;2;160;255;220m",
            "\033[48;2;32;20;36m\033[38;2;255;170;210m",
            "\033[48;2;12;40;58m\033[38;2;120;210;255m",
            "\033[48;2;40;34;48m\033[38;2;230;220;255m"
        },
        {
            "Monochrome",
            "\033[1;37m",
            "\033[37m",
            "\033[90m",
            "\033[1;37m",
            "\033[37m",
            "\033[37m",
            "\033[1;37m",
            "\033[48;2;30;30;30m\033[37m",
            "\033[48;2;20;20;20m\033[37m",
            "\033[48;2;38;38;38m\033[97m",
            "\033[48;2;38;38;38m\033[37m",
            "\033[48;2;25;25;25m\033[97m",
            "\033[48;2;25;25;25m\033[97m",
            "\033[48;2;22;22;22m\033[97m",
            "\033[48;2;28;28;28m\033[97m"
        }
    };

    constexpr std::size_t THEME_COUNT = sizeof(THEME_PRESETS) / sizeof(THEME_PRESETS[0]);

    std::string joinLabels(const std::vector<std::string>& labels) {
        if (labels.empty()) {
            return "";
        }
        std::ostringstream oss;
        for (std::size_t i = 0; i < labels.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << labels[i];
        }
        return oss.str();
    }
}

Renderer::Renderer()
    : displayGrid_(),
      width_(0),
      height_(0),
      useColors_(true),
      activePalette_(paletteForTheme(0)),
      activeThemeIndex_(0) {}

int Renderer::index(int x, int y) const {
    return y * width_ + x;
}

void Renderer::copyMazeToDisplay(const Maze& maze) {
    width_ = maze.getWidth();
    height_ = maze.getHeight();
    displayGrid_.assign(static_cast<std::size_t>(width_ * height_), ' ');

    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            displayGrid_[static_cast<std::size_t>(index(x, y))] = maze.getCellAt(x, y);
        }
    }
}

void Renderer::overlayPath(const Path& path) {
    for (int i = 0; i < path.getSize(); i++) {
        Point p = path[i];
        int x = p.getX();
        int y = p.getY();

        if (x >= 0 && x < width_ && y >= 0 && y < height_) {
            char& cell = displayGrid_[static_cast<std::size_t>(index(x, y))];
            if (cell != 'S' && cell != 'G') {
                cell = '*';
            }
        }
    }
}

void Renderer::overlayMultiplePaths(const std::vector<const Path*>& paths) {
    static const char markers[] = {'*', '+', 'o'};

    for (std::size_t p = 0; p < paths.size() && p < sizeof(markers); ++p) {
        const Path* path = paths[p];
        if (!path) continue;

        for (int i = 0; i < path->getSize(); i++) {
            Point pt = (*path)[i];
            int x = pt.getX();
            int y = pt.getY();

            if (x >= 0 && x < width_ && y >= 0 && y < height_) {
                char& cell = displayGrid_[static_cast<std::size_t>(index(x, y))];
                if (cell != 'S' && cell != 'G') {
                    cell = markers[p];
                }
            }
        }
    }
}

std::string Renderer::getCellGlyph(char cell) const {
    switch (cell) {
        case '#': return "██";
        case 'S': return "▶ ";
        case 'G': return "★ ";
        case '*': return "◆ ";
        case '+': return "◇ ";
        case '~': return "≈ ";
        case '^': return "▲ ";
        case '.': return "  ";
        default: {
            std::string glyph(1, cell);
            glyph.push_back(' ');
            return glyph;
        }
    }
}

const std::string& Renderer::cellColor(char cell) const {
    if (!useColors_) {
        return NO_COLOR;
    }

    switch (cell) {
        case '#': return activePalette_.cellWallColor;
        case 'S': return activePalette_.cellStartColor;
        case 'G': return activePalette_.cellGoalColor;
        case '*': return activePalette_.cellPathColor;
        case '+': return activePalette_.cellAltPathColor;
        case '~': return activePalette_.cellWaterColor;
        case '^': return activePalette_.cellMountainColor;
        case '.': return activePalette_.cellFloorColor;
        default: return activePalette_.cellFloorColor;
    }
}

void Renderer::printCell(char cell) const {
    const std::string glyph = getCellGlyph(cell);
    if (useColors_) {
        std::cout << cellColor(cell) << glyph << RESET_CODE;
    } else {
        std::cout << glyph;
    }
}

short Renderer::ansiToNcursesColor(const std::string& ansiColor) {
    if (ansiColor.find("31m") != std::string::npos) return COLOR_RED;
    if (ansiColor.find("32m") != std::string::npos) return COLOR_GREEN;
    if (ansiColor.find("33m") != std::string::npos) return COLOR_YELLOW;
    if (ansiColor.find("34m") != std::string::npos) return COLOR_BLUE;
    if (ansiColor.find("35m") != std::string::npos) return COLOR_MAGENTA;
    if (ansiColor.find("36m") != std::string::npos) return COLOR_CYAN;
    if (ansiColor.find("37m") != std::string::npos) return COLOR_WHITE;
    if (ansiColor.find("30m") != std::string::npos) return COLOR_BLACK;
    
    return -1; 
}

void Renderer::drawGrid() const {
    const std::string reset = useColors_ ? RESET_CODE : "";
    const std::string frameColor = useColors_ ? activePalette_.frameColor : "";
    int contentWidth = width_ * CELL_PRINT_WIDTH + 2;
    int pad = cli_.centerPadding(contentWidth);
    std::string margin(static_cast<std::size_t>(pad), ' ');

    std::cout << margin;
    std::cout << frameColor << "╔";
    for (int i = 0; i < width_ * CELL_PRINT_WIDTH; ++i) {
        std::cout << "═";
    }
    std::cout << "╗" << reset << "\n";

    for (int y = 0; y < height_; ++y) {
        std::cout << margin;
        std::cout << frameColor << "║" << reset;
        for (int x = 0; x < width_; ++x) {
            printCell(displayGrid_[static_cast<std::size_t>(index(x, y))]);
        }
        std::cout << frameColor << "║" << reset << "\n";
    }

    std::cout << margin;
    std::cout << frameColor << "╚";
    for (int i = 0; i < width_ * CELL_PRINT_WIDTH; ++i) {
        std::cout << "═";
    }
    std::cout << "╝" << reset << "\n";
}

void Renderer::drawLegendEntry(const std::string& label, char cell, int pad, int legendWidth) const {
    const std::string reset = useColors_ ? RESET_CODE : "";
    std::string glyph = getCellGlyph(cell);
    std::string plainLine = "  " + glyph + "  " + label;
    int lineWidth = cli_.measureDisplayWidth(plainLine);

    std::cout << std::string(static_cast<std::size_t>(pad), ' ') << "  ";
    if (useColors_) {
        std::cout << cellColor(cell) << glyph << RESET_CODE;
    } else {
        std::cout << glyph;
    }
    std::cout << "  ";
    if (useColors_) {
        std::cout << activePalette_.legendValueColor;
    }
    std::cout << label;
    if (useColors_) {
        std::cout << reset;
    }

    int spaces = legendWidth - lineWidth;
    if (spaces > 0) {
        std::cout << std::string(static_cast<std::size_t>(spaces), ' ');
    }
    std::cout << "\n";
}

void Renderer::printLegend() const {
    std::vector<std::pair<std::string, char>> entries = {
        {"Start", 'S'},
        {"Goal", 'G'},
        {"Wall", '#'},
        {"Open floor", '.'},
        {"Primary path", '*'},
        {"Secondary path", '+'},
        {"Water", '~'},
        {"Mountain", '^'}
    };

    int legendWidth = cli_.measureDisplayWidth("Legend");
    for (const auto& entry : entries) {
        std::string plainLine = "  " + getCellGlyph(entry.second) + "  " + entry.first;
        legendWidth = std::max(legendWidth, cli_.measureDisplayWidth(plainLine));
    }

    int pad = cli_.centerPadding(legendWidth);

    std::cout << "\n";
    if (useColors_) {
        std::cout << activePalette_.legendLabelColor;
    }
    std::cout << std::string(static_cast<std::size_t>(pad), ' ') << "Legend";
    if (useColors_) {
        std::cout << RESET_CODE;
    }
    std::cout << "\n";

    for (const auto& entry : entries) {
        drawLegendEntry(entry.first, entry.second, pad, legendWidth);
    }
}

void Renderer::printHeader(const Maze& maze, const std::vector<std::string>& pathLabels) const {
    const Point start = maze.getStart();
    const Point goal = maze.getGoal();
    const std::string labelString = joinLabels(pathLabels);
    std::string themeLine = "» " + activePalette_.name + " Theme";
    std::ostringstream info;
    info << "  Maze " << width_ << "×" << height_
         << "   Start(" << start.getX() << "," << start.getY() << ")"
         << "   Goal(" << goal.getX() << "," << goal.getY() << ")";
    if (!labelString.empty()) {
        info << "   Highlighted: " << labelString;
    }
    std::string infoLine = info.str();

    int width = std::max(cli_.measureDisplayWidth(themeLine), cli_.measureDisplayWidth(infoLine));
    int pad = cli_.centerPadding(width);

    std::cout << "\n";
    std::cout << std::string(static_cast<std::size_t>(pad), ' ');
    if (useColors_) {
        std::cout << activePalette_.headerPrimary;
    }
    std::cout << themeLine;
    if (useColors_) {
        std::cout << RESET_CODE;
    }
    std::cout << "\n";

    std::cout << std::string(static_cast<std::size_t>(pad), ' ');
    if (useColors_) {
        std::cout << activePalette_.headerSecondary;
    }
    std::cout << infoLine;
    if (useColors_) {
        std::cout << RESET_CODE;
    }
    std::cout << "\n\n";
}

void Renderer::printPathStats(const Maze& maze, const Path& path, const std::string& label) const {
    if (path.isEmpty()) {
        return;
    }

    PathAnalyzer analyzer;
    PathMetrics metrics = analyzer.analyze(path, maze);

    int steps = std::max(0, path.getSize() - 1);
    int turns = metrics.getNumberOfTurns();
    int narrow = metrics.getNarrowPassages();
    double straightness = metrics.getStraightness() * 100.0;

    std::ostringstream costStream;
    costStream << std::fixed << std::setprecision(2) << metrics.getTotalCostWithPenalty();

    std::ostringstream avgStream;
    if (steps > 0) {
        avgStream << std::fixed << std::setprecision(2) << metrics.getAvgStepCost();
    } else {
        avgStream << "0.00";
    }

    std::ostringstream straightStream;
    straightStream << std::fixed << std::setprecision(1) << straightness;

    std::vector<std::pair<std::string, std::string>> stats = {
        {"Steps", std::to_string(steps)},
        {"Cost", costStream.str()},
        {"Turns", std::to_string(turns)},
        {"Narrow passages", std::to_string(narrow)},
        {"Straightness", straightStream.str() + "%"},
        {"Avg step cost", avgStream.str()}
    };

    int width = cli_.measureDisplayWidth(label + " Path Metrics");
    for (const auto& stat : stats) {
        std::string plain = "  • " + stat.first + ": " + stat.second;
        width = std::max(width, cli_.measureDisplayWidth(plain));
    }
    int pad = cli_.centerPadding(width);

    std::cout << "\n";
    std::cout << std::string(static_cast<std::size_t>(pad), ' ');
    if (useColors_) std::cout << activePalette_.legendLabelColor;
    std::cout << label << " Path Metrics";
    if (useColors_) std::cout << RESET_CODE;
    std::cout << "\n";

    for (const auto& stat : stats) {
        std::cout << std::string(static_cast<std::size_t>(pad), ' ');
        if (useColors_) std::cout << activePalette_.statLabelColor;
        std::cout << "  • " << stat.first << ": ";
        if (useColors_) std::cout << activePalette_.statValueColor;
        std::cout << stat.second;
        if (useColors_) std::cout << RESET_CODE;
        std::cout << "\n";
    }
}

void Renderer::printComparisonStats(const Maze& maze,
                                    const Path& path1,
                                    const std::string& label1,
                                    const Path& path2,
                                    const std::string& label2) const {
    printPathStats(maze, path1, label1);
    printPathStats(maze, path2, label2);

    PathAnalyzer analyzer1, analyzer2;
    PathMetrics metrics1 = analyzer1.analyze(path1, maze);
    PathMetrics metrics2 = analyzer2.analyze(path2, maze);

    int steps1 = std::max(0, path1.getSize() - 1);
    int steps2 = std::max(0, path2.getSize() - 1);
    double cost1 = metrics1.getTotalCostWithPenalty();
    double cost2 = metrics2.getTotalCostWithPenalty();

    std::string advantagePrefix = "  Advantage: ";
    std::string advantageValue;
    if (steps1 == steps2) {
        advantageValue = "Equal path length";
    } else {
        const std::string& winner = (steps1 < steps2) ? label1 : label2;
        std::ostringstream advantageStream;
        advantageStream << winner << " shorter by " << std::abs(steps1 - steps2) << " step(s)";
        advantageValue = advantageStream.str();
    }

    std::ostringstream costDiff;
    costDiff << std::fixed << std::setprecision(2) << std::abs(cost1 - cost2);
    std::string costPrefix = "  Cost delta: ";
    std::string costValue;
    if (std::abs(cost1 - cost2) < 1e-6) {
        costValue = "Identical";
    } else {
        const std::string& cheaper = (cost1 < cost2) ? label1 : label2;
        std::ostringstream costStream;
        costStream << costDiff.str() << " (" << cheaper << " cheaper)";
        costValue = costStream.str();
    }

    int width = std::max(cli_.measureDisplayWidth(advantagePrefix + advantageValue),
                         cli_.measureDisplayWidth(costPrefix + costValue));
    int pad = cli_.centerPadding(width);

    std::cout << "\n";
    std::cout << std::string(static_cast<std::size_t>(pad), ' ');
    if (useColors_) std::cout << activePalette_.statLabelColor;
    std::cout << advantagePrefix;
    if (useColors_) std::cout << activePalette_.statValueColor;
    std::cout << advantageValue;
    if (useColors_) std::cout << RESET_CODE;
    std::cout << "\n";

    std::cout << std::string(static_cast<std::size_t>(pad), ' ');
    if (useColors_) std::cout << activePalette_.statLabelColor;
    std::cout << costPrefix;
    if (useColors_) std::cout << activePalette_.statValueColor;
    std::cout << costValue;
    if (useColors_) std::cout << RESET_CODE;
    std::cout << "\n";
}

void Renderer::render(const Maze& maze) {
    copyMazeToDisplay(maze);
    printHeader(maze, {});
    drawGrid();
    printLegend();
}

const char* Renderer::getCellColor(char cell) const {
    if (!useColors_) {
        return "";
    }
    return cellColor(cell).c_str();
}

void Renderer::setColorMode(bool enabled) {
    useColors_ = enabled;
    if (enabled) {
        cli_.enableColors();
    } else {
        cli_.disableColors();
    }
}

void Renderer::renderAnimated(const Maze& maze, const Path& path, int delayMs) {
    cli_.clearScreen();
    cli_.printHeader("Animated Path Discovery");

    for (int step = 0; step <= path.getSize(); ++step) {
        cli_.moveCursor(6, 1);

        copyMazeToDisplay(maze);

        for (int i = 0; i < step && i < path.getSize(); ++i) {
            Point p = path[i];
            int x = p.getX();
            int y = p.getY();
            if (x >= 0 && x < width_ && y >= 0 && y < height_) {
                char& cell = displayGrid_[static_cast<std::size_t>(index(x, y))];
                if (cell != 'S' && cell != 'G') {
                    cell = '*';
                }
            }
        }

        drawGrid();

        std::cout << "\n";
        if (useColors_) std::cout << activePalette_.statLabelColor;
        std::cout << "  Step " << step << " / " << path.getSize();
        if (useColors_) std::cout << RESET_CODE;
        std::cout << "\n";
        cli_.drawProgressBar(step, path.getSize(), 50);
        std::cout << "\n";

        usleep(delayMs * 1000);
    }

    std::cout << "\n";
    cli_.printSuccess("Path discovery complete!");
    printLegend();
}

void Renderer::render(const Maze& maze, const Path& path, const std::string& label) {
    copyMazeToDisplay(maze);
    if (!path.isEmpty()) {
        overlayPath(path);
    }

    std::vector<std::string> labels;
    if (!path.isEmpty()) {
        labels.push_back(label);
    }

    printHeader(maze, labels);
    drawGrid();
    printLegend();

    if (!path.isEmpty()) {
        printPathStats(maze, path, label);
    } else {
        if (useColors_) std::cout << activePalette_.statLabelColor;
        std::cout << "\n  No path data available.";
        if (useColors_) std::cout << RESET_CODE;
        std::cout << "\n";
    }
}

void Renderer::renderComparison(const Maze& maze,
                                const Path& path1,
                                const Path& path2,
                                const std::string& label1,
                                const std::string& label2) {
    copyMazeToDisplay(maze);
    std::vector<const Path*> overlays = {&path1, &path2};
    overlayMultiplePaths(overlays);

    printHeader(maze, {label1, label2});
    drawGrid();
    printLegend();
    printComparisonStats(maze, path1, label1, path2, label2);
}

const Renderer::ThemePalette& Renderer::paletteForTheme(std::size_t index) {
    if (index >= THEME_COUNT) {
        index = 0;
    }
    return THEME_PRESETS[index];
}

void Renderer::setTheme(ThemeId theme) {
    setThemeByIndex(static_cast<std::size_t>(theme));
}

void Renderer::setThemeByIndex(std::size_t index) {
    if (index >= THEME_COUNT) {
        index = 0;
    }
    activeThemeIndex_ = index;
    activePalette_ = paletteForTheme(activeThemeIndex_);
}

Renderer::ThemeId Renderer::getTheme() const {
    return static_cast<ThemeId>(activeThemeIndex_ % THEME_COUNT);
}

std::string Renderer::getThemeName() const {
    return activePalette_.name;
}

bool Renderer::isColorModeEnabled() const {
    return useColors_;
}

const Renderer::ThemePalette& Renderer::getActivePalette() const {
    return activePalette_;
}

std::vector<std::string> Renderer::listAvailableThemes() {
    std::vector<std::string> names;
    names.reserve(THEME_COUNT);
    for (std::size_t i = 0; i < THEME_COUNT; ++i) {
        names.push_back(THEME_PRESETS[i].name);
    }
    return names;
}

const Renderer::ThemePalette& Renderer::getThemePalette(std::size_t index) {
    return paletteForTheme(index);
}

bool Renderer::saveToFile(const char* filename) const {
    std::ofstream file(filename);
    if (!file.is_open() || displayGrid_.empty() || width_ == 0 || height_ == 0) {
        return false;
    }
    
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            file << displayGrid_[static_cast<std::size_t>(index(x, y))];
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

void Renderer::clear() {
    displayGrid_.clear();
    width_ = 0;
    height_ = 0;
}
