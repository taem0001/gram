# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Iinclude

# Folders
SRC_DIR := src
INC_DIR := include
BIN_DIR := bin

# Output executable
TARGET := $(BIN_DIR)/kilo

# Find all .c files in src/
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Convert each .c into a .o in the SAME folder
OBJS := $(SRCS:.c=.o)

# Default rule
all: $(TARGET)

# Link objects into final binary
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile each .c → .o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/*.h
	$(CC) $(CFLAGS) -c $< -o $@

# Create bin folder if missing
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean build
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)

# Clean everything
fclean: clean
	rm -rf $(BIN_DIR)

# Rebuild everything
re: fclean all

.PHONY: all clean fclean re

