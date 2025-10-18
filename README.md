# Maze Solver and Path Analyzer

## Project Description

**MazeMaster** is a C++ application built with the Qt framework, designed to generate, solve, and analyze mazes with a highly interactive graphical user interface (GUI). The system generates random mazes using recursive backtracking, solves them with multiple pathfinding algorithms (Breadth-First Search, Depth-First Search, and A\*), and provides advanced path analysis (e.g., path length, number of turns, dead ends). The Qt-based GUI features dynamic visualization, animating the solving process, and user controls for maze size and algorithm selection. Developed for a college Object-Oriented Programming (OOP) course, this project demonstrates all OOP concepts—encapsulation, inheritance, polymorphism, abstraction, and composition—through a modular, robust design for a team of five, aiming to deliver an engaging and technically impressive solution.

## OOP Concepts Applied

- **Encapsulation**: Private attributes in `Cell` (e.g., `visited`, `walls`) and `Maze` (e.g., `grid`) are protected, with public methods like `getVisited()` and `setWall()` controlling access to ensure data integrity and hide implementation details.
- **Inheritance**: The abstract base class `Solver` defines shared attributes (e.g., `maze` reference) and methods, with subclasses `BFSSolver`, `DFSSolver`, and `AStarSolver` inheriting to implement specific pathfinding logic.
- **Polymorphism**: The virtual `solve()` method in `Solver` is overridden in subclasses (e.g., `BFSSolver` uses a queue, `DFSSolver` uses a stack, `AStarSolver` uses heuristics), enabling dynamic algorithm selection at runtime.
- **Abstraction**: The `Solver` class provides an abstract interface with pure virtual methods (e.g., `solve()`), hiding complex pathfinding logic from the user and ensuring modular design.
- **Composition**: The `Maze` class is composed of `Cell` objects forming a grid; `Analyzer` contains a `Path` (vector of `Cell` pointers) for analysis; `MainWindow` integrates `Maze`, `Solver`, and `Analyzer` for GUI rendering and interaction.

## Team Members

- Yashvanth S (CS24I1029)
- Jeevanandham T (CS24I1034)
- P Y Nithillakrishi (CS24I1039)
- Sri Hari S (CS24B2054)

## Tech Stack

- **Programming Language**: C++ (C++11 or later, leveraging smart pointers and `std::chrono` for timing).
- **GUI Framework**: Qt (version 5 or 6, using Qt Widgets and QPainter for GUI and visualization).
- **Build Tools**: Qt Creator with qmake or CMake for project compilation.
- **Version Control**: Git (via GitHub) for team collaboration.
