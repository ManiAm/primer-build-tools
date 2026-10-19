DEBUG    ?= 0   # Default DEBUG flag
VERBOSE  ?= 0   # Set VERBOSE=1 for debugging commands

CXX      := g++
CXXFLAGS := -Wall -MMD -MP  # Enable warnings and dependency generation

ifeq ($(DEBUG), 1)
	CXXFLAGS += -g
else
	CXXFLAGS += -O2
endif

# Verbose output control
ifeq ($(VERBOSE), 1)
  QUIET :=
else
  QUIET := @
endif

SRC_DIR  := src
INCLUDE_DIR := include
BUILD_DIR := build
BIN_DIR  := bin

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
O_FILES   := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
DEP_FILES := $(O_FILES:.o=.d)

CXXFLAGS += -I$(INCLUDE_DIR)

# Binary name
TARGET   := myprog

# Default target
all: $(BIN_DIR)/$(TARGET)

# Linking
$(BIN_DIR)/$(TARGET): $(O_FILES)
	@echo "Linking $@"
	$(QUIET)$(CXX) -o $@ $(O_FILES)

# Compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR) $(BIN_DIR)
	@echo "Compiling $<"
	$(QUIET)$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

-include $(DEP_FILES)

.PHONY: clean
clean:
	@echo "Cleaning up..."
	$(QUIET)rm -rf $(BUILD_DIR) 
	$(QUIET)rm -rf $(BIN_DIR)/$(TARGET)

.PHONY: rebuild
rebuild: clean all

.PHONY: debug
debug:
	@echo "SRC_FILES: $(SRC_FILES)"
	@echo "O_FILES: $(O_FILES)"
	@echo "DEP_FILES: $(DEP_FILES)"
