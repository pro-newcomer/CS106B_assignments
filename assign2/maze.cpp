/*
 * Student: Yifan Yang
 * Section: Wed 6:45-7:45pm
 *
 * This file implements BFS ways to solve mazes. I've used generics in other
 * programming languages, and I've done research to find this `template` syntax
 * in c++ where I could define a template of the function. The compiler will automatically add
 * the code to the case I used the function.
 */
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;

/* This funtion takes two arguments `maze`, the grid represents the maze; and `cur`,
 * current location and generates a set of locations that is valid to move from current
 * location `cur`.
 */
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;
    // possible directions
    GridLocation N = GridLocation(cur.row - 1, cur.col);
    GridLocation S = GridLocation(cur.row + 1, cur.col);
    GridLocation W = GridLocation(cur.row, cur.col - 1);
    GridLocation E = GridLocation(cur.row, cur.col + 1);
    GridLocation possibleMoves[] = {N, S, W, E};
    for (GridLocation move : possibleMoves) {
        if (maze.inBounds(move) && maze[move]) {
            neighbors.add(move);
        }
    }
    return neighbors;
}

/* This function takes a maze and a path in that maze to validate the path
 * in that maze. Raises errors when it is not valid and returns nothing when
 * the path is valid.
 */
void validatePath(Grid<bool>& maze, Stack<GridLocation> path) {
    GridLocation mazeExit = {maze.numRows() - 1,  maze.numCols() - 1};
    GridLocation mazeStart = {0, 0};

    if (path.peek() != mazeExit) {
        error("Path does not end at maze exit");
    }

    Set<GridLocation> uniqMoves;
    int pathSize = path.size();

    // Validate every move
    GridLocation current = path.pop();
    uniqMoves.add(current);
    GridLocation prev;
    while (!path.isEmpty()) {
        prev = path.pop();
        Set<GridLocation> validMoves = generateValidMoves(maze, prev);
        if (!validMoves.contains(current)) {
            error("Some move is not a valid move of its previous location");
        }
        current = prev;
        uniqMoves.add(current);
    }
    // check path start
    if (prev != mazeStart) {
        error("Path does not start from the (0, 0) position");
    }

    // Check loops
    if (uniqMoves.size() != pathSize) {
        error("Loops in path");
    }
}

/* This function takes the grid of a maze and uses helper methods
 * defined above to find a path for that maze. It visualize the path
 * and the maze while it trying to do BFS to find the final solution.
 */
Stack<GridLocation> solveMaze(Grid<bool>& maze) {
    MazeGraphics::drawGrid(maze);
    Stack<GridLocation> path;

    Queue<Stack<GridLocation>> searchQueue = {{{0,0}}};
    GridLocation mazeExit = {maze.numRows() - 1,  maze.numCols() - 1};

    // track points that is already taken
    Set<GridLocation> uniqMoves;

    while (!searchQueue.isEmpty()) {
        Stack<GridLocation> currentPath = searchQueue.dequeue();
        MazeGraphics::highlightPath(currentPath, "blue");
        if (currentPath.peek() == mazeExit) {
            path = currentPath;
            break;
        } else {
            // iterate possible nextmoves
            for (GridLocation nextMove : generateValidMoves(maze, currentPath.peek())) {
                if (uniqMoves.contains(nextMove)) {
                    // This may form a loop or a longer path
                    continue;
                }
                // add to track
                uniqMoves.add(nextMove);
                // make a copy
                Stack<GridLocation> newPath = currentPath;
                // extend next step
                newPath.push(nextMove);
                // add new path to processing queue
                searchQueue.enqueue(newPath);
            }
        }
    }
    MazeGraphics::highlightPath(path, "green");
    return path;
}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Stack<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0,0}, {0,2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

PROVIDED_TEST("validatePath on correct solution") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for medium maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for large maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/25x33.maze", maze);
    readSolutionFile("res/25x33.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> not_end_at_exit = { {1, 0}, {0, 0} };
    Stack<GridLocation> not_begin_at_entry = { {1, 0}, {1, 1} };
    Stack<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {1, 1} };
    Stack<GridLocation> teleport = { {0 ,0}, {1, 1} };
    Stack<GridLocation> revisit = { {0 ,0}, {1, 0}, {0, 0}, {1, 0}, {1, 1} };

    EXPECT_ERROR(validatePath(maze, not_end_at_exit));
    EXPECT_ERROR(validatePath(maze, not_begin_at_entry));
    EXPECT_ERROR(validatePath(maze, go_through_wall));
    EXPECT_ERROR(validatePath(maze, teleport));
    EXPECT_ERROR(validatePath(maze, revisit));
}

PROVIDED_TEST("solveMaze on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMaze on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

// TODO: add your test cases here

STUDENT_TEST("Test validatePath on margin situation") {
    Grid<bool> maze = {{true}};
    Stack<GridLocation> path = {{0, 0}};
    EXPECT_NO_ERROR(validatePath(maze, path));
}

STUDENT_TEST("Test validatePath") {
    Grid<bool> maze = {{true, false},
                       {false, true}};
    Stack<GridLocation> path = {{0, 0}, {1, 1}};
    EXPECT_ERROR(validatePath(maze, path));
}

STUDENT_TEST("Test validatePath") {
    Grid<bool> maze = {{true, false, true},
                       {false, true, true}};
    Stack<GridLocation> path = {{0, 0}, {1, 1}, {1, 2}};
    EXPECT_ERROR(validatePath(maze, path));
}

STUDENT_TEST("Test solveMaze on file 17x37") {
    Grid<bool> maze;
    readMazeFile("res/17x37.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("Test solveMaze on file 19x35") {
    Grid<bool> maze;
    readMazeFile("res/19x35.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}
