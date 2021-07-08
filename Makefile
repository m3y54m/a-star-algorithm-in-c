CC = gcc
CFLAGS = -g
SOURCE_DIR = src
BUILD_DIR = build

all: $(BUILD_DIR) main

$(BUILD_DIR):
	mkdir -pv $(BUILD_DIR)

main:
	$(CC) $(CFLAGS) $(SOURCE_DIR)/main.c -o $(BUILD_DIR)/main

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean