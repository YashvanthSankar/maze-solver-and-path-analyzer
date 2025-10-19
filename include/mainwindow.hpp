#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include "maze.hpp"
#include "solver.hpp"
#include "analyzer.hpp"

class QLineEdit;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    Maze& maze; // Composition
    Solver* solver; // Polymorphic reference
    Analyzer* analyzer; // Composition
    std::vector<Cell*> path;
    QLineEdit* sizeInput;
    QPushButton* solveButton;

public:
    MainWindow(Maze& m, Solver* s, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void startSolving();
};

#endif