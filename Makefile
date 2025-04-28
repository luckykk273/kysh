CC = gcc
CFLAGS = -Wall -Wextra -Iinc -g
SRC_DIR = src
OBJ_DIR = obj
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BUILD_DIR)/kysh

# Default rule
all: prepare_dirs $(TARGET)

# Create necessary directories once
prepare_dirs:
	@mkdir -p $(OBJ_DIR) $(BUILD_DIR)

# Link object files into final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files and binary
clean:
	rm -rf $(OBJ_DIR) $(BUILD_DIR)

# Run the shell
run: all
	./$(TARGET)

.PHONY: all clean run prepare_dirs
