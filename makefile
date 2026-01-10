# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Iinclude

# Directories
SRC_DIR = src
BUILD_DIR = build

# Sources
SRC = $(wildcard $(SRC_DIR)/*.c)

# Targets
TARGET1 = awale1
TARGET2 = awale2

# Object dirs
OBJDIR1 = $(BUILD_DIR)/awale1
OBJDIR2 = $(BUILD_DIR)/awale2

# Objects
OBJ1 = $(patsubst $(SRC_DIR)/%.c,$(OBJDIR1)/%.o,$(SRC))
OBJ2 = $(patsubst $(SRC_DIR)/%.c,$(OBJDIR2)/%.o,$(SRC))

# Default: build both
all: $(TARGET1) $(TARGET2)

# Build only target 1 / 2
1: $(TARGET1)
2: $(TARGET2)

# Create object directories
$(OBJDIR1):
	mkdir -p $(OBJDIR1)

$(OBJDIR2):
	mkdir -p $(OBJDIR2)

# Compile objects (target 1)
$(OBJDIR1)/%.o: $(SRC_DIR)/%.c | $(OBJDIR1)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile objects (target 2)
$(OBJDIR2)/%.o: $(SRC_DIR)/%.c | $(OBJDIR2)
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(TARGET1): $(OBJ1)
	$(CC) $(CFLAGS) $(OBJ1) -o $@ -lm

$(TARGET2): $(OBJ2)
	$(CC) $(CFLAGS) $(OBJ2) -o $@ -lm

run1: $(TARGET1)
	./$(TARGET1)

run2: $(TARGET2)
	./$(TARGET2)

clean:
	rm -rf $(BUILD_DIR) $(TARGET1) $(TARGET2)

.PHONY: all 1 2 run1 run2 clean
