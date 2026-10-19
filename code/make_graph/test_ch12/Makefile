# Ch12: Build Directory
DEBUG ?= 0
BUILD_DIR = build

ifeq ($(DEBUG), 1)
  CXXFLAGS = -Wall -g -MMD -MP
else
  CXXFLAGS = -Wall -O2 -MMD -MP
endif

SRC_FILES = main.cpp lib.cpp
OBJ_FILES = $(SRC_FILES:%.cpp=$(BUILD_DIR)/%.o)
DEP_FILES = $(OBJ_FILES:.o=.d)

myprog: $(OBJ_FILES)
	g++ $^ -o $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@echo "[CXX] $<"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

-include $(DEP_FILES)

.PHONY: clean
clean:
	rm -f myprog
	rm -rf $(BUILD_DIR)

.PHONY: rebuild
rebuild: clean myprog
