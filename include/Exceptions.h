#ifndef MAZE_SOLVER_EXCEPTIONS_H
#define MAZE_SOLVER_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class MazeException : public std::runtime_error {
public:
    explicit MazeException(const std::string& message)
        : std::runtime_error(message) {}
};

class AnalysisException : public std::runtime_error {
public:
    explicit AnalysisException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif
