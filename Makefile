# Compiler and Flags
CC := gcc
CCFLAGS := \
    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wundef -Wpadded \
	-Wdouble-promotion -Wformat=2 -Wno-unused-parameter -Wcast-align=strict \
    -Wpointer-arith -Wlogical-op -Wnull-dereference -Wfloat-equal \
    -fno-common -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
    -O0 -g

RAYLIB_FLAGS := -lraylib
RAYLIB_INC_DIR := raylib/include

# Project Directories
SRC_DIR := .
INC_DIR := inc
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin

# Target Executable Name (with .exe for Windows)
APPNAME := main
ifeq ($(OS),Windows_NT)
    TARGET := $(BIN_DIR)/$(APPNAME).exe
else
    TARGET := $(BIN_DIR)/$(APPNAME)
endif

# Source and Object Files
SRC_FILES := $(SRC_DIR)/main.c $(SRC_DIR)/src/sim_view.c $(SRC_DIR)/src/os_controller.c
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

include config.mk

# Main Build Rule (renamed to avoid circular dependency with 'build' directory)
all: $(TARGET)

# Build Target
$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(CCFLAGS) $(OBJ_FILES) -I$(INC_DIR) -o $@ $(RAYLIB_FLAGS) -L$(RAYLIB_LIB_DIR)

# Compile Source Files to Object Files (Ensure BUILD_DIR exists before compiling)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CCFLAGS) -I$(INC_DIR) -isystem$(RAYLIB_INC_DIR) -c $< -o $@

# Create Build Directories
$(BUILD_DIR) $(BIN_DIR):
	-$(MKDIR) "$@"
	-$(MKDIR) "$@/src"

# Clean Up Build Artifacts
clean:
	-$(RM) $(BUILD_DIR)

# Run the program
run: $(TARGET)
ifeq ($(OS),Windows_NT)
	"$(TARGET)"
else
	./$(TARGET)
endif

# Phony Targets
.PHONY: clean all run
