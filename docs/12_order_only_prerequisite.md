# Order-Only Prerequisites

In all the examples so far, prerequisites trigger a rebuild when they change — if a prerequisite is newer than the target, `make` runs the recipe. But sometimes you need a prerequisite that must **exist** before building the target, without triggering a rebuild when it changes. This is what **order-only prerequisites** are for.

The most common use case is ensuring an output directory exists before writing files into it.

## The Problem

Suppose we want to place our compiled object files in a `build/` directory:

```makefile
build/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

If the `build/` directory does not exist, the recipe fails:

    $ make
    g++: error: build/main.o: No such file or directory

## Approach 1: `mkdir` in the Recipe

One fix is to add `mkdir -p` to the recipe:

```makefile
build/main.o: main.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

This works, but `mkdir -p build` runs on **every** build — even when the directory already exists. It adds unnecessary commands to every recipe.

## Approach 2: Normal Prerequisite

Another option is to make `build` a normal prerequisite:

```makefile
build/main.o: main.cpp build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build
```

The problem here is that `build` is a directory, and its modification time changes whenever a file is added to it. This causes `make` to **rebuild every object file** whenever any file in `build/` changes — even if the source files have not changed.

## Solution: Order-Only Prerequisites

An order-only prerequisite is listed after a `|` (pipe) character in the prerequisite list:

    target: normal-prerequisites | order-only-prerequisites

```makefile
build/main.o: main.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build
```

The `| build` tells `make`:
- **If `build/` does not exist**, create it before running the recipe.
- **If `build/` already exists**, do not check its modification time — changes to the directory do not trigger a rebuild.

This is the correct way to handle output directories in Makefiles.

> **Note:** If the `build/` directory is deleted, `make` will recreate it. But simply *modifying* it (e.g., adding other files to it) will not cause unnecessary recompilation.

## Multiple Order-Only Prerequisites

You can list multiple order-only prerequisites after the `|`:

```makefile
build/main.o: main.cpp | build logs
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build

logs:
	mkdir -p logs
```

Both `build/` and `logs/` are created if they don't exist, without triggering rebuilds.

## Summary

| Prerequisite type | Syntax              | Ensures existence? | Triggers rebuild on change? |
|-------------------|---------------------|--------------------|-----------------------------|
| Normal            | `target: prereq`    | Yes                | Yes                         |
| Order-only        | `target: \| prereq` | Yes                | No                          |

Use order-only prerequisites for output directories and other resources that must exist but should not influence rebuild decisions.

## The Makefile — Build Directory

Until now, our Makefile has placed object files (`.o`) and dependency files (`.d`) alongside the source files. This clutters the source directory. A common practice is to put all build artifacts in a separate `build/` directory, using an order-only prerequisite to ensure it exists.

Building on the [ch10 Makefile](10_conditional_statements.md#the-makefile--adding-debug-support):

```makefile
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
```

What changed from ch10:

- **`BUILD_DIR = build`** — a variable for the output directory.
- **`OBJ_FILES`** now produces `build/main.o` and `build/lib.o` instead of `main.o` and `lib.o`.
- **`$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)`** — the pattern rule outputs into `build/`, with `| $(BUILD_DIR)` as an order-only prerequisite.
- **`$(BUILD_DIR):` rule** — creates the directory if it doesn't exist.
- **`clean`** removes the entire `build/` directory with `rm -rf`.

The source directory stays clean:

    $ make
    mkdir -p build
    [CXX] main.cpp
    g++ -Wall -O2 -MMD -MP   -c main.cpp -o build/main.o
    [CXX] lib.cpp
    g++ -Wall -O2 -MMD -MP   -c lib.cpp -o build/lib.o
    g++ build/main.o build/lib.o -o myprog

On subsequent builds, `mkdir -p build` does **not** run — the order-only prerequisite is already satisfied:

    $ touch main.cpp
    $ make
    [CXX] main.cpp
    g++ -Wall -O2 -MMD -MP   -c main.cpp -o build/main.o
    g++ build/main.o build/lib.o -o myprog

---

← Previous: [Duplicate Targets](11_targets.md) · Next: [Library Dependency](13_lib_dependency.md) →
