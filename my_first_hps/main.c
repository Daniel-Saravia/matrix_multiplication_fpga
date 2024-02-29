#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <sys/mman.h>
#include "address_map_arm.h" // Ensure this path is correct for your project's structure

// Define a union for easier GPIO register mapping. This allows for both
// bit-specific and whole-register manipulation.
typedef union {
    unsigned int value; // Allows access to the whole 32-bit register at once
    struct {
        unsigned int gpio0 : 4; // Bits for the first segment/display
        unsigned int gpio1 : 4; // Bits for the second segment/display
        unsigned int gpio2 : 4; // Bits for the third segment/display
        unsigned int gpio3 : 4; // Bits for the fourth segment/display
        unsigned int gpio4 : 4; // Bits for the fifth segment/display
        unsigned int gpio5 : 4; // Bits for the sixth segment/display
        unsigned int gpiou : 11; // Reserved bits (unused in this case)
    } bits;
} GpioRegister;

// Function prototypes for managing physical memory access
int open_physical(int fd);
void close_physical(int fd);
void* map_physical(int fd, unsigned int base, unsigned int span);
int unmap_physical(void* virtual_base, unsigned int span);

// Opens /dev/mem for accessing physical memory, necessary for direct hardware manipulation
int open_physical(int fd) {
    if (fd == -1) {
        fd = open("/dev/mem", (O_RDWR | O_SYNC));
        if (fd == -1) {
            perror("ERROR: could not open \"/dev/mem\"");
            return -1;
        }
    }
    return fd;
}

// Closes the file descriptor associated with /dev/mem
void close_physical(int fd) {
    close(fd);
}

// Maps a physical memory address range into the process's virtual address space
void* map_physical(int fd, unsigned int base, unsigned int span) {
    void *virtual_base = mmap(NULL, span, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, base);
    if (virtual_base == MAP_FAILED) {
        perror("ERROR: mmap() failed");
        close(fd);
        return NULL;
    }
    return virtual_base;
}

// Unmaps a previously mapped virtual address space
int unmap_physical(void* virtual_base, unsigned int span) {
    if (munmap(virtual_base, span) != 0) {
        perror("ERROR: munmap() failed");
        return -1;
    }
    return 0;
}

int main() {
    int fd = -1; // File descriptor for accessing /dev/mem
    void* LW_virtual; // Virtual base address after mapping

    // Attempt to open and map the necessary physical memory
    if ((fd = open_physical(fd)) == -1) {
        return -1;
    }
    if ((LW_virtual = map_physical(fd, LW_BRIDGE_BASE, LW_BRIDGE_SPAN)) == NULL) {
        close_physical(fd);
        return -1;
    }

    // Configure a specific port (JP1) for output
    volatile unsigned int* JP1_ptr = (unsigned int*)(LW_virtual + JP1_BASE);
    *(JP1_ptr + 1) = 0x0000000F; // Example: Setting lower 4 bits for output, adjust as needed

    *JP1_ptr = 4; // Set the output to display the number "4". Adjust this operation as per your hardware's requirement.

    printf("Displaying number 4 on the 7 Segment Decoder circuits\n");

    // Perform cleanup by unmapping memory and closing the file descriptor
    unmap_physical(LW_virtual, LW_BRIDGE_SPAN);
    close_physical(fd);
    return 0;
}


