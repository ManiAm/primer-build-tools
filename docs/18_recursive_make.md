# Recursive Make

When a project grows to have multiple independent components — libraries, tools, or modules — managing everything in a single Makefile becomes unwieldy. **Recursive make** splits the build across multiple Makefiles, one per directory, with a top-level Makefile that coordinates them.

## How It Works

The top-level Makefile invokes `make` in each subdirectory using `$(MAKE) -C`:

```makefile
.PHONY: all
all:
	$(MAKE) -C src/module1
	$(MAKE) -C src/module2
```

`-C src/module1` changes into the `src/module1` directory and runs that directory's own Makefile. Always use `$(MAKE)` instead of `make` so that flags like `-j` (parallel builds) and `-n` (dry run) are passed down to sub-builds.

This was briefly introduced in [ch11](11_targets.md#multiple-targets-in-a-rule). Here we build a full example.

## Example

Consider a project with two modules and a main program:

```
project/
├── Makefile
├── common.mk
├── include/
│   ├── module1/
│   │   └── foo.h
│   └── module2/
│       └── bar.h
├── src/
│   ├── main.cpp
│   ├── module1/
│   │   ├── Makefile
│   │   └── foo.cpp
│   └── module2/
│       ├── Makefile
│       └── bar.cpp
├── build/
└── bin/
```

### Source Files

```cpp
// include/module1/foo.h
#ifndef MODULE1_FOO_H
#define MODULE1_FOO_H
void foo_function();
#endif
```

```cpp
// include/module2/bar.h
#ifndef MODULE2_BAR_H
#define MODULE2_BAR_H
void bar_function();
#endif
```

```cpp
// src/module1/foo.cpp
#include <iostream>
#include "module1/foo.h"

void foo_function()
{
    std::cout << "foo_function called" << std::endl;
}
```

```cpp
// src/module2/bar.cpp
#include <iostream>
#include "module2/bar.h"

void bar_function()
{
    std::cout << "bar_function called" << std::endl;
}
```

```cpp
// src/main.cpp
#include "module1/foo.h"
#include "module2/bar.h"

int main()
{
    foo_function();
    bar_function();
    return 0;
}
```

### Shared Settings — `common.mk`

Without a shared file, every sub-Makefile would repeat the same compiler, flags, and include paths. Instead, put shared settings in a file that every Makefile can `include`.

There is no strict standard for naming this file. Common conventions are:

- **`common.mk`** — most widely used; `.mk` is the recognized extension for included Makefiles (used by BSD, GNU examples, and most editors for syntax highlighting).
- **`Makefile.common`** — also common; readable, but some editors may not highlight it as a Makefile.

We use `common.mk`:

```makefile
# common.mk
CXX      = g++
CXXFLAGS = -Wall -O2 -MMD -MP -I$(PROJECT_ROOT)/include
```

Each Makefile sets `PROJECT_ROOT` relative to its own location, then includes this file.

### Makefiles

**Top-level** — orchestrates the build:

```makefile
# project/Makefile
PROJECT_ROOT = .
BUILD_DIR    = build
BIN_DIR      = bin

include common.mk

OBJ_FILES = $(BUILD_DIR)/main.o \
            $(BUILD_DIR)/module1/foo.o \
            $(BUILD_DIR)/module2/bar.o

TARGET = $(BIN_DIR)/myprog

.PHONY: all
all: modules $(TARGET)

.PHONY: modules
modules:
	$(MAKE) -C src/module1
	$(MAKE) -C src/module2

$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	@echo "[LINK] $@"
	$(CXX) $^ -o $@

$(BUILD_DIR)/main.o: src/main.cpp | $(BUILD_DIR)
	@echo "[CXX] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

-include $(BUILD_DIR)/main.d

.PHONY: clean
clean:
	$(MAKE) -C src/module1 clean
	$(MAKE) -C src/module2 clean
	rm -f $(BUILD_DIR)/main.o $(BUILD_DIR)/main.d
	rm -f $(TARGET)
```

The `modules` target runs before linking. Each sub-make builds its own objects into `build/<module>/`.

**Module 1:**

```makefile
# src/module1/Makefile
PROJECT_ROOT = ../..
BUILD_DIR    = $(PROJECT_ROOT)/build/module1

include $(PROJECT_ROOT)/common.mk

.PHONY: all
all: $(BUILD_DIR)/foo.o

$(BUILD_DIR)/foo.o: foo.cpp | $(BUILD_DIR)
	@echo "[CXX] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

-include $(BUILD_DIR)/*.d

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
```

**Module 2** follows the same pattern — only the file names differ:

```makefile
# src/module2/Makefile
PROJECT_ROOT = ../..
BUILD_DIR    = $(PROJECT_ROOT)/build/module2

include $(PROJECT_ROOT)/common.mk

.PHONY: all
all: $(BUILD_DIR)/bar.o

$(BUILD_DIR)/bar.o: bar.cpp | $(BUILD_DIR)
	@echo "[CXX] $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

-include $(BUILD_DIR)/*.d

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
```

### Build Output

```
$ make
[CXX] foo.cpp
[CXX] bar.cpp
[CXX] src/main.cpp
[LINK] bin/myprog

$ ./bin/myprog
foo_function called
bar_function called
```

Notice that `build/` mirrors the source layout (`build/module1/`, `build/module2/`), avoiding file name clashes when different modules have files with the same name.

## Passing Variables to Sub-Builds

Variables set on the command line propagate to sub-makes automatically:

```
$ make CXXFLAGS="-Wall -g"
```

Every `$(MAKE) -C` call inherits the overridden `CXXFLAGS`.

To export a variable defined inside a Makefile so that sub-makes see it, use `export`:

```makefile
export DEBUG = 1
```

Without `export`, variables defined in a parent Makefile are **not** visible to sub-makes.

## Limitations

The paper [*Recursive Make Considered Harmful*](https://aegis.sourceforge.net/auug97.pdf) (Peter Miller, 1998) highlights a fundamental issue: when the build is split across multiple Makefiles, no single invocation of `make` sees the full dependency graph. Here are the three main problems, illustrated with our project:

### 1. Reduced Parallelism

In GNU Make, `-j` parallelizes independent **targets**, not lines within a single recipe. The two `$(MAKE) -C` calls in our `modules` target are lines in one recipe, so they always run sequentially — even with `make -j4`:

```
make -j4 with a single Makefile:

    foo.o  ─┐
    bar.o  ─┼── link ── myprog      (all 3 compile in parallel)
    main.o ─┘

make -j4 with recursive make:

    [module1: foo.o] ─ then ─ [module2: bar.o] ─┐
    [main.o] ───────────────────────────────────┼── link ── myprog
                                                │
                    (modules are sequential; main.o runs in parallel)
```

Each sub-make can parallelize its own files internally, but the sequential top-level calls limit overall throughput.

### 2. Missed Dependencies

Suppose module1 generates a config header during its build, and module2 includes it:

```
module1 build  →  generates config.h
module2 build  →  bar.cpp includes config.h
```

With a **single Makefile**, you would write `bar.o: config.h` and Make handles the ordering. With **recursive make**, module2's Makefile has no way to express this dependency. If config.h is stale or missing, the build silently uses the wrong version.

Because developers cannot trust incremental builds in this situation, they often resort to `make clean && make` — rebuilding everything from scratch.

### 3. Over-Building

Because `modules` is `.PHONY`, Make **always** enters every sub-directory and runs its sub-make — even when nothing has changed. Each sub-make must scan all files and check timestamps before deciding there is nothing to do. On large projects with many modules, this overhead adds up significantly.

### Summary

| Problem               | Single Makefile            | Recursive Make                          |
| --------------------- | -------------------------- | --------------------------------------- |
| Parallel builds       | Full parallelism with `-j` | Limited by sequential sub-make calls    |
| Dependency tracking   | Complete graph — correct   | Fragmented — can miss cross-module deps |
| Incremental builds    | Only rebuilds what changed | Sub-makes always entered; may over-build |

For large projects, build systems like **CMake**, **Meson**, or **Bazel** avoid these issues by generating a single dependency graph across all modules. Despite its limitations, recursive make works well for projects with clearly independent modules that rarely depend on each other.

---

← Previous: [Parallel Make](17_parallel_build.md) · Next: [Dry Run / Debugging](19_debugging_make.md) →
