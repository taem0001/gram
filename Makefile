# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -Iinclude

# Directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := build
BIN_DIR := bin

# Output executable
TARGET := $(BIN_DIR)/kilo

# All source files
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Turn src/file.c → build/file.o
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Default rule
all: $(TARGET)

# Link all objects
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile .c → .o inside build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean object files
clean:
	rm -rf $(OBJ_DIR)

# Clean everything including executable
fclean: clean
	rm -rf $(BIN_DIR)

# Rebuild from scratch
re: fclean all

.PHONY: all clean fclean re

