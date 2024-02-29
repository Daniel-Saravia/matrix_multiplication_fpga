#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#define LW_BRIDGE_BASE 0xff200000
#define LW_BRIDGE_SPAN 0x00005000
#define EDGE_CAPTURE_BASE 0xff20005c // Edge capture register address for buttons
#define SLIDE_SWITCH_BASE 0xFF200040 // Slide switch data register address
#define HEX3_HEX0_BASE 0xFF200020 // Base address for HEX3 to HEX0
#define HEX5_HEX4_BASE 0xFF200030 // Base address for HEX5 and HEX4

// Segment patterns for numbers 0-9 on a 7-segment display
const unsigned int segment_patterns[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

int main() {
    int fd;
    void *virtual_base;
    volatile unsigned int *HEX3_HEX0_ptr; // Pointer to HEX3-0 Display Register
    volatile unsigned int *HEX5_HEX4_ptr; // Pointer to HEX5-4 Display Register
    volatile unsigned int *KEY_ptr; // Pointer to the edge capture register for buttons
    volatile unsigned int *SW_ptr;  // Pointer to the slide switch data register
    unsigned int edge_detected;
    unsigned int switch_state, last_switch_state = 0xFFFFFFFF; // Initialize to an unlikely value

    // Open the memory device
    if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
        printf("ERROR: could not open \"/dev/mem\"...\n");
        return 1;
    }

    // Map virtual address space to access the FPGA registers
    virtual_base = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, LW_BRIDGE_BASE);
    if (virtual_base == MAP_FAILED) {
        printf("ERROR: mmap() failed...\n");
        close(fd);
        return 1;
    }

    // Initialize the pointers to the FPGA device registers
    KEY_ptr = (volatile unsigned int *)(virtual_base + (EDGE_CAPTURE_BASE - LW_BRIDGE_BASE));
    SW_ptr = (volatile unsigned int *)(virtual_base + (SLIDE_SWITCH_BASE - LW_BRIDGE_BASE));
    HEX3_HEX0_ptr = (volatile unsigned int *)(virtual_base + (HEX3_HEX0_BASE - LW_BRIDGE_BASE));
    HEX5_HEX4_ptr = (volatile unsigned int *)(virtual_base + (HEX5_HEX4_BASE - LW_BRIDGE_BASE));

    while (1) {
        // Read the edge capture register for button presses
        edge_detected = *KEY_ptr;
        if (edge_detected != 0) {
            // Display the button number (1-4) on HEX3 to HEX0
            unsigned int i;
            for (i = 0; i < 4; i++) {
                if (edge_detected & (1 << i)) {
                    *HEX3_HEX0_ptr = segment_patterns[i + 1]; // Display button number on HEX0
                    break; // Only show the first button pressed
                }
            }
            *KEY_ptr = edge_detected; // Clear the edge capture register
        }

        // Handle slide switch changes
        switch_state = *SW_ptr & 0x0F; // Consider only the lower 4 bits
        if (switch_state != last_switch_state) {
            printf("Slide switch state changed to: %X\n", switch_state);
            // Assuming we want to display the value of the switches as a hex digit on HEX0
            if (switch_state < 10) {
                *HEX3_HEX0_ptr = segment_patterns[switch_state]; // Display switch state on HEX0
            } else {
                // Adjust this part as needed for hexadecimal values A-F
            }
            last_switch_state = switch_state;
        }

        usleep(100000); // Sleep for 100ms to reduce CPU usage
    }

    // Cleanup
    if (munmap(virtual_base, LW_BRIDGE_SPAN) != 0) {
        printf("ERROR: munmap() failed...\n");
    }
    close(fd);
    return 0;
}
