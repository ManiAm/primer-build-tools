# Project Structure

In [ch12](12_order_only_prerequisite.md#the-makefile--build-directory), we moved build artifacts into a `build/` directory. But the source files, headers, and binary still live in the project root. As a project grows, this becomes hard to manage. A standard practice is to organize all files into separate directories by purpose.

## Standard Layout

```
project/
├── Makefile
├── include/
│   └── lib.h
├── src/
│   ├── main.cpp
│   └── lib.cpp
├── build/          # Object files (.o) and dependency files (.d)
└── bin/            # Final binary
```

- **`src/`** — Source files (`.cpp`).
- **`include/`** — Header files (`.h`). Keeping headers separate makes it clear which files define the public interface.
- **`build/`** — Intermediate build artifacts (`.o`, `.d`). Created automatically by the Makefile.
- **`bin/`** — The final compiled binary.
- **`Makefile`** — Stays at the project root.

This layout keeps the source tree clean and makes cleanup easy — just remove `build/` and `bin/`.

## Finding Source Files Automatically

With source files in `src/`, we could list them manually:

```makefile
SRC_FILES = src/main.cpp src/lib.cpp
```

But as files are added or removed, this list becomes tedious to maintain. The `$(wildcard ...)` function finds all files matching a pattern:

```makefile
SRC_DIR   = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
```

This expands to all `.cpp` files in `src/` at parse time. If `src/` contains `main.cpp` and `lib.cpp`, the result is `src/main.cpp src/lib.cpp`.

## The Makefile — Project Structure

Continuing from the [ch15 Makefile](15_make_install.md#the-makefile--adding-install-targets), we reorganize the project into directories:

```makefile
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
```

What changed from ch14:

- **`SRC_DIR`, `INCLUDE_DIR`, `BIN_DIR`** — variables for the directory layout.
- **`$(wildcard $(SRC_DIR)/*.cpp)`** — automatically finds all `.cpp` files in `src/` instead of listing them by hand.
- **`$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, ...)`** — maps `src/main.cpp` → `build/main.o` (we covered `patsubst` in [ch06](06_substitution.md#patsubst--pattern-replacement)).
- **`-I$(INCLUDE_DIR)`** — tells the compiler where to find header files.
- **`TARGET = $(BIN_DIR)/myprog`** — the binary now goes into `bin/`.
- **`| $(BIN_DIR)`** — order-only prerequisite for the `bin/` directory, same technique as `build/` from [ch12](12_order_only_prerequisite.md#order-only-prerequisites).
- **`$(SRC_DIR)/%.cpp`** — the pattern rule now matches source files in `src/`.
- **`clean`** removes both `build/` and `bin/`.

Build and verify:

```
$ make
mkdir -p build
mkdir -p bin
[CXX] src/main.cpp
[CXX] src/lib.cpp
[LINK] bin/myprog

$ ./bin/myprog
Hello from lib!
```

## Additional Directories

Larger projects often include more directories:

```
project/
├── tests/       # Unit and integration tests
├── docs/        # Documentation (design specs, API reference)
├── scripts/     # Utility and automation scripts
├── .gitignore   # Excludes build artifacts from version control
└── README.md    # Project overview and setup instructions
```

These are not part of the build process but help keep the project organized and maintainable.

---

← Previous: [Make Install](15_make_install.md) · Next: [Parallel Make](17_parallel_build.md) →
