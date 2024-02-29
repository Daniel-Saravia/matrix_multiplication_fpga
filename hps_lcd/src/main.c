#include "terasic_os_includes.h"
#include "LCD_Lib.h"
#include "lcd_graphic.h"
#include "font.h"
#include "gameLogic.h"

// Define hardware register constants
#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

// Function prototypes
int openMemoryDevice();
void* mapMemory(int fd);
void initializeLCDCanvas(LCD_CANVAS *canvas, void *virtual_base);
void drawGrid(LCD_CANVAS *canvas);
void printNumbers(LCD_CANVAS *canvas);
void cleanup(void *virtual_base, int fd, LCD_CANVAS *canvas);
void clearNumbers(LCD_CANVAS *canvas);
void displayGridOnLCD(LCD_CANVAS *canvas);
void handleInput();
int checkGameOver();

// Main function - entry point of the program
int main() {
    LCD_CANVAS LcdCanvas;

    // Open memory device for hardware interaction
    int fd = openMemoryDevice();
    if (fd == -1) return 1; // Exit if memory device opening fails

    // Map hardware registers into user space
    void *virtual_base = mapMemory(fd);
    if (virtual_base == MAP_FAILED) return 1; // Exit if mapping fails

    // Initialize the LCD canvas and perform drawing operations
    initializeLCDCanvas(&LcdCanvas, virtual_base);
    drawGrid(&LcdCanvas);

    // Initialize the 2048 game
    initializeGame();

    while(1) {
         // Display the game grid on the LCD
        displayGridOnLCD(&LcdCanvas);

        // Check if the game is over
        if (checkGameOver()) {
            printf("Game Over! Final Score: %d\n", score);
            break;
        }

        // Handle user input
        handleInput();
        clearNumbers(&LcdCanvas);
    }

    // Perform cleanup before exiting the program
    cleanup(virtual_base, fd, &LcdCanvas);
    return 0;
}

// Opens the memory device and returns file descriptor
int openMemoryDevice() {
    int fd = open("/dev/mem", (O_RDWR | O_SYNC));
    if (fd == -1) {
        printf("ERROR: could not open \"/dev/mem\"...\n");
    }
    return fd;
}

// Maps memory for hardware registers and returns the virtual base address
void* mapMemory(int fd) {
    void *virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
    if (virtual_base == MAP_FAILED) {
        printf("ERROR: mmap() failed...\n");
        close(fd);
    }
    return virtual_base;
}

// Initializes the LCD canvas with specified parameters
void initializeLCDCanvas(LCD_CANVAS *canvas, void *virtual_base) {
    printf("Graphic LCD Demo\r\n");
    canvas->Width = LCD_WIDTH;
    canvas->Height = LCD_HEIGHT;
    canvas->BitPerPixel = 1;
    canvas->FrameSize = canvas->Width * canvas->Height / 8;
    canvas->pFrame = (void *)malloc(canvas->FrameSize);
    if (canvas->pFrame == NULL) {
        printf("failed to allocate lcd frame buffer\r\n");
    } else {
        LCDHW_Init(virtual_base);
        LCDHW_BackLight(true);
        LCD_Init();
        DRAW_Clear(canvas, LCD_WHITE);
    }
}

// Draws a 4x4 grid on the LCD screen
void drawGrid(LCD_CANVAS *canvas) {
    int row, col;
    int cellWidth = canvas->Width / 4;
    int cellHeight = canvas->Height / 4;
    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            int x1 = col * cellWidth;
            int y1 = row * cellHeight;
            int x2 = x1 + cellWidth - 1;
            int y2 = y1 + cellHeight - 1;
            DRAW_Rect(canvas, x1, y1, x2, y2, LCD_BLACK);
        }
    }
}

// Prints the number "123" in each cell of the grid
void printNumbers(LCD_CANVAS *canvas) {
    int row, col;
    int cellWidth = canvas->Width / 4;
    int cellHeight = canvas->Height / 4;
    for (row = 0; row < 4; row++) {
        for (col = 0; col < 4; col++) {
            int x = col * cellWidth + (cellWidth - 16) / 2;
            int y = row * cellHeight + (cellHeight - 16) / 2;
            DRAW_PrintString(canvas, x, y, "123", LCD_BLACK, &font_16x16);
        }
    }
    DRAW_Refresh(canvas);
    
}

void clearNumbers(LCD_CANVAS *canvas) {
    // Clear the entire screen
    DRAW_Clear(canvas, LCD_WHITE);

    // Optionally, redraw the grid if needed
    drawGrid(canvas);

    // Refresh the LCD to display the changes
    DRAW_Refresh(canvas);
}


// Cleans up by unmapping memory, closing file descriptor, and freeing canvas frame
void cleanup(void *virtual_base, int fd, LCD_CANVAS *canvas) {
    if (munmap(virtual_base, HW_REGS_SPAN) != 0) {
        printf("ERROR: munmap() failed...\n");
    }
    close(fd);
    
    // Free the canvas frame buffer
    if (canvas->pFrame != NULL) {
        free(canvas->pFrame);
        canvas->pFrame = NULL; // Set to NULL to prevent double free
    }
}
