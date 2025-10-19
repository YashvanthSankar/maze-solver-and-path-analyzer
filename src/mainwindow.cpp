#include "../include/mainwindow.hpp"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>

MainWindow::MainWindow(Maze& m, Solver* s, QWidget* parent) : QMainWindow(parent), maze(m), solver(s), analyzer(nullptr) {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    sizeInput = new QLineEdit("10", this);
    sizeInput->setPlaceholderText("Enter maze size");
    layout->addWidget(sizeInput);

    solveButton = new QPushButton("Solve Maze", this);
    layout->addWidget(solveButton);

    connect(solveButton, &QPushButton::clicked, this, &MainWindow::startSolving);

    setFixedSize(500, 550);
}

void MainWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    float cellSize = 500.0f / maze.getCell(0, 0).getPosition().first;
    for (int i = 0; i < maze.getCell(0, 0).getPosition().first; ++i) {
        for (int j = 0; j < maze.getCell(0, 0).getPosition().second; ++j) {
            Cell& cell = maze.getCell(i, j);
            painter.setPen(Qt::black);
            painter.setBrush(cell.isVisited() ? Qt::green : Qt::white);
            painter.drawRect(i * cellSize, j * cellSize, cellSize, cellSize);
            if (cell.hasWall(0)) painter.drawLine(i * cellSize, j * cellSize, (i+1) * cellSize, j * cellSize);
            if (cell.hasWall(2)) painter.drawLine((i+1) * cellSize, j * cellSize, (i+1) * cellSize, (j+1) * cellSize);
        }
    }
    for (Cell* cell : path) {
        painter.setBrush(Qt::red);
        auto [x, y] = cell->getPosition();
        painter.drawRect(x * cellSize, y * cellSize, cellSize, cellSize);
    }
}

void MainWindow::startSolving() {
    int size = sizeInput->text().toInt();
    if (size >= 2) {
        path = solver->solve(maze.getCell(0, 0), maze.getCell(size-1, size-1), this);
        if (!path.empty()) {
            analyzer = new Analyzer(path);
            analyzer->analyze();
        }
        update();
    }
}