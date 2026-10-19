# Ch15: Adding Install Targets
DEBUG ?= 0
BUILD_DIR = build
PREFIX ?= /usr/local

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

.PHONY: install
install: myprog
	install -d $(PREFIX)/bin
	install -m 755 myprog $(PREFIX)/bin/

.PHONY: uninstall
uninstall:
	rm -f $(PREFIX)/bin/myprog

.PHONY: clean
clean:
	rm -f myprog
	rm -rf $(BUILD_DIR)

.PHONY: rebuild
rebuild: clean myprog
