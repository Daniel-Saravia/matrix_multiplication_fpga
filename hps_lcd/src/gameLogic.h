#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#define GRID_SIZE 4

// Global variables
extern int grid[GRID_SIZE][GRID_SIZE];
extern int score;

// Function declarations
void initializeGame();
void addRandomTile();
void slideAndMergeRow(int *row);
void moveLeft();
void moveRight();
void moveUp();
void moveDown();
int checkGameOver();
void reverseRow(int *row);
void transposeGrid();
void handleInput();
#endif // GAMELOGIC_H
