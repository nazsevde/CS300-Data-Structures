
// Sevdenaz Yılmaz 3030

#include <iostream>
#include <vector>
#include "theStack.h"
#include "RandGen.h"
#include <fstream>
#include <string>

using namespace std;

struct mazeCell {
    // Stores the cell's location in the matrix (x-coordinate).
    int posX;
    // Stores the cell's location in the matrix (y-coordinate).
    int posY;

    // Initially, all walls are present.
    int wallRight = 1;
    int wallLeft = 1;
    int wallDown = 1;
    int wallUp = 1;

    // Used to keep track of cell visits during maze generation.
    bool marked = false;
    // Used to keep track of cell visits while solving the maze.
    bool marked2 = false;

    // Indicates unsuccessful attempts to move in a direction during maze generation.
    bool attemptRight = false;
    bool attemptLeft = false;
    bool attemptDown = false;
    bool attemptUp = false;

    // Default constructor
    mazeCell() : posX(0), posY(0) {}  // Default values can be used.

    // Constructor definition
    mazeCell(int x, int y) : posX(x), posY(y) {}
};

// Function to break the wall to the right of the current cell
bool breakRightWall(vector<vector<mazeCell>> &maze, mazeCell &currentCell) {
    // Check if there is a cell to the right that is not marked
    bool isRightCellExistsAndNotMarked = currentCell.posY + 1 <= maze[0].size() - 1 && !maze[currentCell.posX][currentCell.posY + 1].marked;
    if (isRightCellExistsAndNotMarked) {
        // Break the wall to the right and update the adjacent cell
        maze[currentCell.posX][currentCell.posY].wallRight = 0; // Breaks the right wall.
        maze[currentCell.posX][currentCell.posY + 1].marked = true; // Marks it as marked.
        maze[currentCell.posX][currentCell.posY + 1].wallLeft = 0;
        return true;
    }
    return false;
}

// Similar functions to break left, down, and up walls
// These functions follow the same logic as breakRightWall, but for different directions.

bool breakLeftWall(vector<vector<mazeCell>> &maze, mazeCell &currentCell) {
    bool isLeftCellExistsAndNotMarked = currentCell.posY - 1 >= 0 && !maze[currentCell.posX][currentCell.posY - 1].marked;
    if (isLeftCellExistsAndNotMarked) {
        maze[currentCell.posX][currentCell.posY].wallLeft = 0;
        maze[currentCell.posX][currentCell.posY - 1].marked = true; // Marks it as marked.
        maze[currentCell.posX][currentCell.posY - 1].wallRight = 0;
        return true;
    }
    return false;
}

bool breakDownWall(vector<vector<mazeCell>> &maze, mazeCell &currentCell) {
    bool isDownCellExistsAndNotMarked = currentCell.posX + 1 <= maze.size() - 1 && !maze[currentCell.posX + 1][currentCell.posY].marked;
    if (isDownCellExistsAndNotMarked) {
        maze[currentCell.posX][currentCell.posY].wallDown = 0;
        maze[currentCell.posX + 1][currentCell.posY].marked = true; // Marks it as marked.
        maze[currentCell.posX + 1][currentCell.posY].wallUp = 0;
        return true;
    }
    return false;
}

bool breakUpWall(vector<vector<mazeCell>> &maze, mazeCell &currentCell) {
    bool isUpCellExistsAndNotMarked = currentCell.posX - 1 >= 0 && !maze[currentCell.posX - 1][currentCell.posY].marked;
    if (isUpCellExistsAndNotMarked) {
        maze[currentCell.posX][currentCell.posY].wallUp = 0;
        maze[currentCell.posX - 1][currentCell.posY].marked = true; // Marks it as marked.
        maze[currentCell.posX - 1][currentCell.posY].wallDown = 0;
        return true;
    }
    return false;
}

// Generate a random direction (1 for down, 2 for left, 3 for up, 4 for right)
int getRandomDirection() {
    RandGen randomGenerator;
    return randomGenerator.RandInt(1, 4);
}

// Function to break a random wall in any direction
bool breakRandomWall(vector<vector<mazeCell>> &currMazeCell, mazeCell &block, theStack<mazeCell> &stack, bool &noMoreWalls) {

    // Generate a random direction (1 for down, 2 for left, 3 for up, 4 for right)
    int randomDir = getRandomDirection();
    if (randomDir == 1) // Try popping the bottom wall.
    {
        if (breakDownWall(currMazeCell, block)) {
            block = currMazeCell[block.posX + 1][block.posY]; // Modify the temp to the cell below.
            stack.push(currMazeCell[block.posX][block.posY]); // Push the new cell to the stack.
            return true;
        }
        block.attemptDown = true;
    }
    // Similar logic for other directions
    else if (randomDir == 2) {
        if (breakLeftWall(currMazeCell, block)) {
            block = currMazeCell[block.posX][block.posY - 1];
            stack.push(currMazeCell[block.posX][block.posY]);
            return true;
        }
        block.attemptLeft = true;
    } else if (randomDir == 3) {
        if (breakUpWall(currMazeCell, block)) {
            block = currMazeCell[block.posX - 1][block.posY];
            stack.push(currMazeCell[block.posX][block.posY]);
            return true;
        }
        block.attemptUp = true;
    } else {
        if (breakRightWall(currMazeCell, block)) {
            block = currMazeCell[block.posX][block.posY + 1];
            stack.push(currMazeCell[block.posX][block.posY]);
            return true;
        }
        block.attemptRight = true;
    }
    if (block.attemptRight && block.attemptUp && block.attemptLeft && block.attemptDown) {
        noMoreWalls = true;
        return true;
    }
    return false;
}

// Function to print the stack in reverse order
void reversePrintStack(theStack<mazeCell> &stack, ofstream &output, const int &numRows) {
    if (stack.isEmpty()) {
        return;
    }
    mazeCell temp;
    stack.pop(temp);
    reversePrintStack(stack, output, numRows);
    output << temp.posY << " " << (numRows - temp.posX - 1) << endl;
}

int main() {
    int totalMazes, numRows, numCols;

    // Input the total number of mazes
    cout << "Enter the total number of mazes: ";
    cin >> totalMazes;

    // Input the number of rows and columns
    cout << "Enter the number of rows and columns (M and N): ";
    cin >> numRows >> numCols;

    vector<vector<vector<mazeCell>>> allMazes;

    for (int i = 1; i <= totalMazes; i++) {
        vector<vector<mazeCell>> currMazeCell(numRows, vector<mazeCell>(numCols));
        for (int x = 0; x < currMazeCell.size(); x++) {
            for (int y = 0; y < currMazeCell[0].size(); y++) {
                currMazeCell[x][y].posX = x;
                currMazeCell[x][y].posY = y;
            }
        }
        theStack<mazeCell> mazeBuilder;
        mazeBuilder.push(currMazeCell[0][0]);
        currMazeCell[0][0].marked = true;
        mazeCell currentBlock = currMazeCell[0][0];

        // Build the maze by breaking walls randomly
        while (!mazeBuilder.isEmpty()) {
            bool done = false;
            bool noMoreWalls = false;
            while (!done) {
                done = breakRandomWall(currMazeCell, currentBlock, mazeBuilder, noMoreWalls);
            }
            if (noMoreWalls == true) {
                mazeBuilder.pop(currentBlock);
            }
        }
        allMazes.push_back(currMazeCell);

        // Output the generated maze to a file
        ofstream outputFile;
        string filename = "maze_" + to_string(i) + ".txt";
        outputFile.open(filename.c_str());
        outputFile << currMazeCell.size() << " " << currMazeCell[0].size() << endl;
        int z = numRows - 1;
        for (int x = 0; x < currMazeCell.size(); x++) {
            for (int y = 0; y < currMazeCell[0].size(); y++) {
                outputFile << "x=" << y << " y=" << x << " l=" << currMazeCell[z][y].wallLeft << " r=" << currMazeCell[z][y].wallRight << " u=" << currMazeCell[z][y].wallUp << " d=" << currMazeCell[z][y].wallDown << endl;
            }
            z--;
        }
        outputFile.close();
    }
    cout << "All mazes are generated." << endl;

    int mazeID, startPosX, startPosY, endPosX, endPosY;

    // Input the maze ID and start/end positions
    cout << "Enter a maze ID between 1 and " << totalMazes << " inclusive to find a path: ";
    cin >> mazeID;
    cout << endl;

    cout << "Enter x and y coordinates of the entry points (x,y) or (column,row): ";
    cin >> startPosX >> startPosY;
    cout << endl;

    cout << "Enter x and y coordinates of the exit points (x,y) or (column,row): ";
    cin >> endPosX >> endPosY;
    cout << endl;

    theStack<mazeCell> solveStack;
    vector<vector<mazeCell>> mazeToSolve;

    mazeCell currentBlock;
    mazeCell previousBlock;

    int cellPosX, cellPosY;
    cellPosX = numRows - 1 - startPosY;
    cellPosY = startPosX;

    mazeToSolve = allMazes[mazeID - 1];
    currentBlock = mazeToSolve[cellPosX][cellPosY];

    solveStack.push(currentBlock);
    cellPosX = numRows - 1 - endPosY;
    cellPosY = endPosX;

    // Find a path through the maze
    while (!(currentBlock.posX == cellPosX && currentBlock.posY == cellPosY)) {
        bool noMoreWalls = false;

        // Check if we can move up and conditions for that move.
        bool canMoveUp = (mazeToSolve[currentBlock.posX][currentBlock.posY].wallUp == 0) && !(previousBlock.posX == currentBlock.posX - 1 && previousBlock.posY == currentBlock.posY) && !mazeToSolve[currentBlock.posX - 1][currentBlock.posY].marked2;

        // Check if we can move left and conditions for that move.
        bool canMoveLeft = (mazeToSolve[currentBlock.posX][currentBlock.posY].wallLeft == 0) && !(previousBlock.posX == currentBlock.posX && previousBlock.posY == currentBlock.posY - 1) && !mazeToSolve[currentBlock.posX][currentBlock.posY - 1].marked2;

        // Check if we can move right and conditions for that move.
        bool canMoveRight = (mazeToSolve[currentBlock.posX][currentBlock.posY].wallRight == 0) && (currentBlock.posY + 1 <= numCols) && !(previousBlock.posX == currentBlock.posX && previousBlock.posY == currentBlock.posY + 1) && !mazeToSolve[currentBlock.posX][currentBlock.posY + 1].marked2;

        // Check if we can move down and conditions for that move.
        bool canMoveDown = (mazeToSolve[currentBlock.posX][currentBlock.posY].wallDown == 0) && !(previousBlock.posX == currentBlock.posX + 1 && previousBlock.posY == currentBlock.posY) && !mazeToSolve[currentBlock.posX + 1][currentBlock.posY].marked2;

        if (canMoveUp) {
            mazeToSolve[currentBlock.posX - 1][currentBlock.posY].marked2 = true;
            previousBlock = currentBlock;
            currentBlock = mazeToSolve[currentBlock.posX - 1][currentBlock.posY];
            solveStack.push(mazeToSolve[currentBlock.posX][currentBlock.posY]);
        }
        else if (canMoveLeft) {
            mazeToSolve[currentBlock.posX][currentBlock.posY - 1].marked2 = true;
            previousBlock = currentBlock;
            currentBlock = mazeToSolve[currentBlock.posX][currentBlock.posY - 1];
            solveStack.push(mazeToSolve[currentBlock.posX][currentBlock.posY]);
        }
        else if (canMoveRight) {
            mazeToSolve[currentBlock.posX][currentBlock.posY + 1].marked2 = true;
            previousBlock = currentBlock;
            currentBlock = mazeToSolve[currentBlock.posX][currentBlock.posY + 1];
            solveStack.push(mazeToSolve[currentBlock.posX][currentBlock.posY]);
        }
        else if (canMoveDown) {
            mazeToSolve[currentBlock.posX + 1][currentBlock.posY].marked2 = true;
            previousBlock = currentBlock;
            currentBlock = mazeToSolve[currentBlock.posX + 1][currentBlock.posY];
            solveStack.push(mazeToSolve[currentBlock.posX][currentBlock.posY]);
        }
        else {
            noMoreWalls = true;
        }
        if (noMoreWalls) {
            solveStack.pop(currentBlock);
            solveStack.pop(currentBlock);
            solveStack.push(currentBlock);
        }
    }
    // Output the path to a file
    ofstream pathOutFile;
    string pathFilename = "maze_" + to_string(mazeID) + "_path_" + to_string(startPosX) + "_" + to_string(startPosY) + "_" + to_string(endPosX) + "_" + to_string(endPosY) + ".txt";
    pathOutFile.open(pathFilename.c_str());
    reversePrintStack(solveStack, pathOutFile, numRows);
    pathOutFile.close();
    return 0;
}


