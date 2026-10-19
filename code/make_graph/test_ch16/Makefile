# Ch16: Project Structure
DEBUG  ?= 0
PREFIX ?= /usr/local

SRC_DIR     = src
INCLUDE_DIR = include
BUILD_DIR   = build
BIN_DIR     = bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
DEP_FILES = $(OBJ_FILES:.o=.d)

CXXFLAGS = -Wall -MMD -MP -I$(INCLUDE_DIR)

ifeq ($(DEBUG), 1)
  CXXFLAGS += -g
else
  CXXFLAGS += -O2
endif

TARGET = $(BIN_DIR)/myprog

$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	@echo "[LINK] $@"
	$(CXX) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "[CXX] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

-include $(DEP_FILES)

.PHONY: install
install: $(TARGET)
	install -d $(PREFIX)/bin
	install -m 755 $(TARGET) $(PREFIX)/bin/

.PHONY: uninstall
uninstall:
	rm -f $(PREFIX)/bin/myprog

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: rebuild
rebuild: clean $(TARGET)
