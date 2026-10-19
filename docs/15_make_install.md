# Make Install

After building a program, you may want to make it available system-wide so users can run it by name from any terminal. The `install` target copies the built binary to a standard system directory.

## Standard Install Paths

The `PREFIX` variable defines the base directory for installation. The conventional default is `/usr/local`:

| Path                | Purpose     |
| ------------------- | ----------- |
| `$(PREFIX)/bin`     | Executables |
| `$(PREFIX)/lib`     | Libraries   |
| `$(PREFIX)/include` | Header files|

Programs installed to `/usr/local/bin` are typically already in the system's `PATH`, so they can be run by name from any directory.

## The `install` Command

The `install` Unix command copies files while setting permissions in one step. It is preferred over `cp` for installation:

- **`install -d <dir>`** — Create a directory (like `mkdir -p`).
- **`install -m 755 <file> <dir>/`** — Copy a file and set its permissions to `755` (owner: read/write/execute, others: read/execute).

## Example

```cpp
// main.cpp
#include <iostream>

int main()
{
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
```

```makefile
CXX      = g++
CXXFLAGS = -Wall
TARGET   = hello
PREFIX   = /usr/local

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

.PHONY: install
install: $(TARGET)
	install -d $(PREFIX)/bin
	install -m 755 $(TARGET) $(PREFIX)/bin/

.PHONY: uninstall
uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)
```

Build, install, and run:

```
$ make
g++ -Wall -o hello main.cpp

$ sudo make install
install -d /usr/local/bin
install -m 755 hello /usr/local/bin/

$ hello
Hello, World!
```

`sudo` is required because `/usr/local/bin` is a system directory. To uninstall:

```
$ sudo make uninstall
rm -f /usr/local/bin/hello
```

To install to a different location, override `PREFIX` on the command line:

```
$ make install PREFIX=$HOME/myapps
```

This installs to `~/myapps/bin/` instead — no `sudo` needed since it is a user-owned directory.

## The Makefile — Adding Install Targets

Continuing from the [ch12 Makefile](12_order_only_prerequisite.md#the-makefile--build-directory), we add `install` and `uninstall` targets:

```makefile
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
```

What changed from ch12:

- **`PREFIX ?= /usr/local`** — default install path, overridable from the command line.
- **`install` target** — depends on `myprog` (builds it first if needed), then copies the binary to `$(PREFIX)/bin/`.
- **`uninstall` target** — removes the installed binary.

---

← Previous: [Package Managers](14_package_managers.md) · Next: [Project Structure](16_project_structure.md) →
