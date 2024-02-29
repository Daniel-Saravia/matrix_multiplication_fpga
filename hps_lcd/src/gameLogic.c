#include <stdint.h>
#include "gameLogic.h"
#include <stdlib.h>
#include <stdio.h>
#include "lcd_graphic.h"
#include "LCD_Lib.h"
#include "font.h"
#include "terasic_os_includes.h"

int grid[GRID_SIZE][GRID_SIZE] = {{0}};
int score = 0;

void initializeGame() {
    int i, j; // Declare loop variables outside the loop
    score = 0;
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }
    addRandomTile();
    addRandomTile();
}

void addRandomTile() {
    int emptyCells = 0, randPos, tileValue;
    int i, j; // Declare loop variables outside the loop

    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) {
                emptyCells++;
            }
        }
    }

    if (emptyCells == 0) return;

    randPos = rand() % emptyCells;
    tileValue = (rand() % 2 + 1) * 2; // Randomly 2 or 4

    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) {
                if (randPos == 0) {
                    grid[i][j] = tileValue;
                    return;
                }
                randPos--;
            }
        }
    }
}

void slideAndMergeRow(int *row) {
    int i, j;

    // Slide
    for (i = 0; i < GRID_SIZE - 1; i++) {
        for (j = 0; j < GRID_SIZE - 1; j++) {
            if (row[j] == 0 && row[j + 1] != 0) {
                row[j] = row[j + 1];
                row[j + 1] = 0;
            }
        }
    }

    // Merge
    for (i = 0; i < GRID_SIZE - 1; i++) {
        if (row[i] == row[i + 1]) {
            row[i] *= 2;
            row[i + 1] = 0;
            score += row[i];
        }
    }

    // Slide again after merging
    for (i = 0; i < GRID_SIZE - 1; i++) {
        for (j = 0; j < GRID_SIZE - 1; j++) {
            if (row[j] == 0 && row[j + 1] != 0) {
                row[j] = row[j + 1];
                row[j + 1] = 0;
            }
        }
    }
}

void moveLeft() {
    int i;
    for (i = 0; i < GRID_SIZE; i++) {
        slideAndMergeRow(grid[i]);
    }
    addRandomTile();
}

void moveRight() {
    int i;
    for (i = 0; i < GRID_SIZE; i++) {
        // Reverse each row, apply slideAndMerge, then reverse back
        reverseRow(grid[i]);
        slideAndMergeRow(grid[i]);
        reverseRow(grid[i]);
    }
    addRandomTile();
}

void reverseRow(int *row) {
    int i, temp;
    for (i = 0; i < GRID_SIZE / 2; i++) {
        temp = row[i];
        row[i] = row[GRID_SIZE - i - 1];
        row[GRID_SIZE - i - 1] = temp;
    }
}

void moveUp() {
    transposeGrid();
    moveLeft();
    transposeGrid();
}

void moveDown() {
    transposeGrid();
    moveRight();
    transposeGrid();
}

void transposeGrid() {
    int i, j, temp;
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = i + 1; j < GRID_SIZE; j++) {
            temp = grid[i][j];
            grid[i][j] = grid[j][i];
            grid[j][i] = temp;
        }
    }
}

int checkGameOver() {
    int i, j;
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) {
                return 0; // Game not over, empty cell found
            }
            if (j < GRID_SIZE - 1 && grid[i][j] == grid[i][j + 1]) {
                return 0; // Game not over, mergeable horizontal cells found
            }
            if (i < GRID_SIZE - 1 && grid[i][j] == grid[i + 1][j]) {
                return 0; // Game not over, mergeable vertical cells found
            }
        }
    }
    return 1; // No more moves, game over
}

void handleInput() {
    char input;
    printf("Enter move (W/A/S/D): ");
    scanf(" %c", &input);  // Note the space before %c to skip any whitespace

    switch (input) {
        case 'w':
        case 'W':
            moveUp();
            break;
        case 'a':
        case 'A':
            moveLeft();
            break;
        case 's':
        case 'S':
            moveDown();
            break;
        case 'd':
        case 'D':
            moveRight();
            break;
        default:
            printf("Invalid input! Please use W, A, S, or D.\n");
            return; // Do not add a new tile if input is invalid
    }

    // Add a random tile after a valid move
    addRandomTile();
}

// Prints the game grid numbers on the LCD screen
void displayGridOnLCD(LCD_CANVAS *canvas) {
    int row, col;
    int cellWidth = canvas->Width / GRID_SIZE;  // Assuming GRID_SIZE is 4
    int cellHeight = canvas->Height / GRID_SIZE;
    char buffer[5];  // Buffer to hold number as a string

    for (row = 0; row < GRID_SIZE; row++) {
        for (col = 0; col < GRID_SIZE; col++) {
            int x = col * cellWidth + (cellWidth - 16) / 2;
            int y = row * cellHeight + (cellHeight - 16) / 2;
            
            if (grid[row][col] != 0) {
                snprintf(buffer, sizeof(buffer), "%d", grid[row][col]);
                DRAW_PrintString(canvas, x, y, buffer, LCD_BLACK, &font_16x16);
            } else {
                DRAW_PrintString(canvas, x, y, "    ", LCD_BLACK, &font_16x16);  // Print blank for 0
            }
        }
    }

    DRAW_Refresh(canvas);
}