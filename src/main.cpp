#include <QApplication>
#include "../include/mainwindow.hpp"
#include "../include/astarsolver.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Maze maze(10, 10);
    AStarSolver solver(maze);
    MainWindow window(maze, &solver);
    window.show();
    return app.exec();
}