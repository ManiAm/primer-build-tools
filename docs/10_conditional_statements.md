# Conditional Statements

Conditional statements let you change how a Makefile behaves based on variable values. This is useful for supporting different build configurations — such as debug vs release mode — or adapting to different platforms.

## Reference

| Conditional    | Description                              | Example |
|----------------|------------------------------------------|---------|
| `ifeq (a, b)`  | True if `a` equals `b`                   | `ifeq ($(DEBUG), 1)` |
| `ifneq (a, b)` | True if `a` does not equal `b`           | `ifneq ($(OS), Linux)` |
| `ifdef VAR`    | True if `VAR` is defined and non-empty   | `ifdef DEBUG` |
| `ifndef VAR`   | True if `VAR` is not defined or is empty | `ifndef RELEASE` |
| `else`         | Alternative branch                       |                  |
| `endif`        | Ends the conditional block               |                  |

## `ifeq` / `ifneq` — Compare Values

`ifeq` checks whether two values are equal. `ifneq` checks whether they are *not* equal:

```makefile
ifeq ($(DEBUG), 1)
  CXXFLAGS += -g
else
  CXXFLAGS += -O2
endif
```

If `DEBUG` is `1`, the debug flag `-g` is added. Otherwise, the optimization flag `-O2` is added.

```makefile
ifneq ($(OS), Linux)
  CFLAGS += -DWINDOWS
endif
```

## `ifdef` / `ifndef` — Check If Defined

`ifdef` checks whether a variable is **defined and non-empty** — it does not check the variable's value:

```makefile
ifdef VERBOSE
  CXXFLAGS += -v
endif
```

`ifndef` is the opposite — true when the variable is not defined or is empty:

```makefile
ifndef CC
  CC = gcc
endif
```

### `ifdef` vs `ifeq` — An Important Distinction

Beginners often confuse `ifdef` with `ifeq`. The difference:

- **`ifdef VAR`** — checks if `VAR` is defined (has any value at all, even `0`).
- **`ifeq ($(VAR), 1)`** — checks if `VAR` has the specific value `1`.

```makefile
DEBUG = 0

ifdef DEBUG
  # This IS true — DEBUG is defined (its value is "0", which is non-empty)
endif

ifeq ($(DEBUG), 1)
  # This is NOT true — DEBUG is "0", not "1"
endif
```

Use `ifeq` when you care about the value. Use `ifdef` when you only care whether the variable exists.

## The Makefile — Adding Debug Support

Building on the [ch09 Makefile](09_header_file_dependencies.md#the-makefile--adding-header-dependencies), we add a `DEBUG` flag that switches between debug and release builds:

```makefile
DEBUG ?= 0

ifeq ($(DEBUG), 1)
  CXXFLAGS = -Wall -g -MMD -MP
else
  CXXFLAGS = -Wall -O2 -MMD -MP
endif

SRC_FILES = main.cpp lib.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)
DEP_FILES = $(OBJ_FILES:.o=.d)

myprog: $(OBJ_FILES)
	g++ $^ -o $@

%.o: %.cpp
	@echo "[CXX] $<"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(DEP_FILES)

.PHONY: clean
clean:
	rm -f myprog *.o *.d

.PHONY: rebuild
rebuild: clean myprog
```

`DEBUG ?= 0` sets the default to release mode (using [`?=`](05_variables.md#4-conditional-variables-) so it can be overridden).

Build in release mode (the default):

    $ make
    [CXX] main.cpp
    g++ -Wall -O2 -MMD -MP   -c main.cpp -o main.o
    [CXX] lib.cpp
    g++ -Wall -O2 -MMD -MP   -c lib.cpp -o lib.o
    g++ main.o lib.o -o myprog

Build in debug mode:

    $ make DEBUG=1
    [CXX] main.cpp
    g++ -Wall -g -MMD -MP   -c main.cpp -o main.o
    [CXX] lib.cpp
    g++ -Wall -g -MMD -MP   -c lib.cpp -o lib.o
    g++ main.o lib.o -o myprog

Notice `-O2` is replaced by `-g` in the debug build.

---

← Previous: [Header File Dependency](09_header_file_dependencies.md) · Next: [Duplicate Targets](11_targets.md) →
