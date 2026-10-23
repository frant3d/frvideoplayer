# Compiler settings
CC       := gcc
CFLAGS   := -Wall -Wextra -O2 -Isrc -Iinclude
LDFLAGS  := -lavformat -lavcodec -lavutil -lswscale -lswresample -lSDL2

# Directory definitions
SRC_DIR  := src
BUILD_DIR:= build
TARGET   := bin/frvideoplayer

# Dynamically discover all .c files anywhere under the src/ directory tree
SRCS     := $(shell find $(SRC_DIR) -name "*.c")

# Map source files to corresponding object files in the build directory
OBJS     := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default rule
all: $(TARGET)

# Rule to link the final executable
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful."

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to completely wipe build outputs and executable binaries
clean:
	rm -rf $(BUILD_DIR) bin/
	@echo "Cleaned."

.PHONY: all clean
