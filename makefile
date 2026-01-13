# =========================
# Optional Windows EXE build
# =========================
EXE ?= 0

ifeq ($(EXE),1)
	CC = x86_64-w64-mingw32-gcc
	EXT = .exe
else
	CC = gcc
	EXT =
endif

# =========================
# Compiler flags
# =========================
CFLAGS = -Wall -Wextra -Iinclude

# =========================
# Directories
# =========================
SRC_DIR = src
BUILD_DIR = build

# =========================
# Sources
# =========================
SRC = $(wildcard $(SRC_DIR)/*.c)

# =========================
# Targets
# =========================
TARGET1 = awale1$(EXT)
TARGET2 = awale2$(EXT)

# =========================
# Object dirs
# =========================
OBJDIR1 = $(BUILD_DIR)/awale1
OBJDIR2 = $(BUILD_DIR)/awale2

# =========================
# Objects
# =========================
OBJ1 = $(patsubst $(SRC_DIR)/%.c,$(OBJDIR1)/%.o,$(SRC))
OBJ2 = $(patsubst $(SRC_DIR)/%.c,$(OBJDIR2)/%.o,$(SRC))

# =========================
# Default target
# =========================
all: $(TARGET1) $(TARGET2)

1: $(TARGET1)
2: $(TARGET2)

# =========================
# Create object directories
# =========================
$(OBJDIR1):
	mkdir -p $(OBJDIR1)

$(OBJDIR2):
	mkdir -p $(OBJDIR2)

# =========================
# Compile objects
# =========================
$(OBJDIR1)/%.o: $(SRC_DIR)/%.c | $(OBJDIR1)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR2)/%.o: $(SRC_DIR)/%.c | $(OBJDIR2)
	$(CC) $(CFLAGS) -c $< -o $@

# =========================
# Link
# =========================
$(TARGET1): $(OBJ1)
	$(CC) $(CFLAGS) $(OBJ1) -o $@

$(TARGET2): $(OBJ2)
	$(CC) $(CFLAGS) $(OBJ2) -o $@

# =========================
# Run (macOS only)
# =========================
run1: $(TARGET1)
	./$(TARGET1)

run2: $(TARGET2)
	./$(TARGET2)

# =========================
# Clean
# =========================
clean:
	rm -rf $(BUILD_DIR) awale1 awale2 awale1.exe awale2.exe

.PHONY: all 1 2 run1 run2 clean