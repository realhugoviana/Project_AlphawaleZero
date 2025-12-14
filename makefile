# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Iinclude -I/opt/homebrew/opt/libpaho-mqtt/include
LDFLAGS = -L/opt/homebrew/opt/libpaho-mqtt/lib -lpaho-mqtt3cs

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

# Files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
TARGET = awale

# Default rule
all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all run clean