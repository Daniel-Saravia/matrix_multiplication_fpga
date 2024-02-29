#
TARGET = 2048_LCD_Game
SRC_DIR = src
ALT_DEVICE_FAMILY ?= soc_cv_av
SOCEDS_ROOT ?= $(SOCEDS_DEST_ROOT)
HWLIBS_ROOT = $(SOCEDS_ROOT)/ip/altera/hps/altera_hps/hwlib
CROSS_COMPILE = arm-linux-gnueabihf-
CFLAGS = -static -g -Wall -D$(ALT_DEVICE_FAMILY) -I$(HWLIBS_ROOT)/include/$(ALT_DEVICE_FAMILY) -I$(HWLIBS_ROOT)/include/ -I$(SRC_DIR)
LDFLAGS = -g -Wall
CC = $(CROSS_COMPILE)gcc
ARCH= arm

build: $(TARGET)

$(TARGET): $(SRC_DIR)/main.o $(SRC_DIR)/terasic_lib.o $(SRC_DIR)/LCD_Lib.o $(SRC_DIR)/LCD_Driver.o $(SRC_DIR)/LCD_Hw.o $(SRC_DIR)/lcd_graphic.o $(SRC_DIR)/font.o $(SRC_DIR)/gameLogic.o
	$(CC) $(LDFLAGS) $^ -o $@ -lrt -lm

$(SRC_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(SRC_DIR)/*.o *~

