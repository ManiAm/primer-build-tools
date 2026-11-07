# Platform Dependency

Makefiles often rely on tools, commands, and conventions that differ between operating systems. A Makefile written for Linux typically won't work on Windows without modification — and vice versa. This chapter explains what varies and how to handle it.

## What Varies

| Aspect            | Linux / macOS         | Windows (MSVC)           |
| ----------------- | --------------------- | ------------------------ |
| C++ compiler      | `g++` or `clang++`    | `cl`                     |
| Warning flags     | `-Wall`               | `/W4`                    |
| Output flag       | `-o myprog`           | `/Fe:myprog.exe`         |
| Object files      | `.o`                  | `.obj`                   |
| Executable        | `myprog` (no ext.)    | `myprog.exe`             |
| Remove files      | `rm -f`               | `del /Q`                 |
| Path separator    | `/`                   | `\`                      |
| Shell             | `sh` / `bash`         | `cmd.exe`                |

## Example: Linux vs Windows

Consider a simple program:

```cpp
// main.cpp
#include <iostream>

int main()
{
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
```

**Linux Makefile:**

```makefile
CXX      = g++
CXXFLAGS = -Wall -O2
TARGET   = myprog

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGET)
```

```
$ make
g++ -Wall -O2 -o myprog main.cpp
```

**Windows Makefile** (using Microsoft's `cl` compiler):

```makefile
CXX      = cl
CXXFLAGS = /W4 /O2 /EHsc
TARGET   = myprog.exe

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) /Fe:$(TARGET) $<

.PHONY: clean
clean:
	del /Q $(TARGET) *.obj
```

The differences are everywhere: compiler name, flag syntax (`-Wall` vs `/W4`), output flag (`-o` vs `/Fe:`), executable extension, and delete command (`rm -f` vs `del /Q`).

> **Note:** GNU Make is not included with Windows. Common options are MSYS2, [ezwinports](https://community.chocolatey.org/packages/make) (`winget install ezwinports.make`), or WSL. Windows also has its own `nmake`, which uses slightly different Makefile syntax.

## Detecting the OS

Rather than maintaining separate Makefiles per platform, you can detect the OS inside a single Makefile using conditionals ([ch10](10_conditional_statements.md)):

```makefile
ifeq ($(OS), Windows_NT)
    # Windows
    RM     = del /Q
    TARGET = myprog.exe
else
    # Linux / macOS
    RM     = rm -f
    TARGET = myprog

    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Darwin)
        CXX = clang++    # macOS default
    endif
endif
```

`$(OS)` is a Windows environment variable set to `Windows_NT` on all modern versions. On Linux and macOS it's unset, so the `else` branch runs. `$(shell uname -s)` returns `Linux` or `Darwin` (macOS), allowing further platform-specific tuning.

## A Portable Makefile

Combining OS detection with the build rules:

```makefile
ifeq ($(OS), Windows_NT)
    RM       = del /Q
    TARGET   = myprog.exe
    CXX      = cl
    CXXFLAGS = /W4 /O2 /EHsc
    OUTFLAG  = /Fe:
else
    RM       = rm -f
    TARGET   = myprog
    CXX     ?= g++
    CXXFLAGS = -Wall -O2
    OUTFLAG  = -o 
endif

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) $(OUTFLAG)$@ $<

.PHONY: clean
clean:
	$(RM) $(TARGET)
```

The recipe is the same on both platforms — only the variable values change. The `OUTFLAG` variable absorbs the incompatible `-o` vs `/Fe:` syntax.

## Limitations

OS detection in Make works for simple projects, but it has limits:

- **Compiler detection** — `$(OS)` tells you the OS, not which compiler is installed. A Linux machine might have `clang++` but not `g++`.

- **Library paths** — finding libraries and headers across platforms requires checking multiple locations.

- **Feature detection** — some projects need to test whether a library is installed, whether the compiler supports a specific flag, or which version of a tool is available. Make has no built-in way to do this.

These limitations are exactly why tools like **Autotools** ([ch22](22_autotools.md)), **CMake** ([ch23](23_cmake.md)), and **Meson** ([ch27](27_meson.md)) were created. They automate feature detection and generate platform-correct build files for you.

---

← Previous: [Dry Run / Debugging](19_debugging_make.md) · Next: [Build Caching](21_build_caching.md) →
