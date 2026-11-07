# SCons

SCons is a build system written in Python. Unlike CMake, which generates build files for other tools (Make, Ninja), SCons **is** the build tool — it reads a Python-based build script (`SConstruct`) and executes the build directly.

The name comes from its predecessor **Cons** (a Perl-based build tool). SCons was first released in 2001 as a modern, Python-based replacement. Notable projects that use SCons include:

- **Godot Engine** — open-source game engine
- **openpilot** — open-source driver assistance system (comma.ai)
- **gem5** — computer architecture simulator

## How SCons Differs

| Aspect               | Make                  | CMake                     | SCons                    |
| -------------------- | --------------------- | ------------------------- | ------------------------ |
| Type                 | Build tool            | Build system generator    | Build tool               |
| Build script         | `Makefile`            | `CMakeLists.txt`          | `SConstruct` (Python)    |
| Language             | Custom DSL            | Custom DSL                | Python                   |
| Dependency tracking  | Timestamps            | Compiler `.d` files       | MD5 checksums + scanning |
| Parallel builds      | `make -j4`            | `cmake --build . -- -j4` | `scons -j4`             |
| Clean                | `make clean` (manual) | `cmake --build . --target clean` | `scons -c` (built-in) |

Key differences from Make and CMake:

- **Python-based** — build scripts are valid Python, so you can use loops, conditionals, and libraries directly.
- **MD5 checksums** — SCons detects changes by content hash, not just timestamps, reducing unnecessary rebuilds.
- **Built-in dependency scanning** — SCons scans C/C++ files for `#include` directives automatically, without compiler-generated `.d` files.
- **No generated build files** — SCons runs the build directly. This simplifies the workflow but limits IDE integration compared to CMake.

## Basic Example

Install SCons:

```
$ pip install scons
```

Project structure:

```
project/
├── SConstruct
├── main.cpp
├── lib.cpp
└── lib.h
```

```cpp
// lib.h
#ifndef LIB_H
#define LIB_H
void hello_from_lib();
#endif
```

```cpp
// lib.cpp
#include <iostream>
#include "lib.h"

void hello_from_lib()
{
    std::cout << "Hello from lib!" << std::endl;
}
```

```cpp
// main.cpp
#include "lib.h"

int main()
{
    hello_from_lib();
    return 0;
}
```

`SConstruct` — the build script (Python):

```python
# SConstruct
env = Environment()
env.Program(target='myprog', source=['main.cpp', 'lib.cpp'])
```

- **`Environment()`** — creates a build environment with default compiler settings.
- **`env.Program()`** — builds an executable from the listed source files.

Build and run:

```
$ scons
g++ -o lib.o -c lib.cpp
g++ -o main.o -c main.cpp
g++ -o myprog main.o lib.o

$ ./myprog
Hello from lib!
```

Clean:

```
$ scons -c
Removed lib.o
Removed main.o
Removed myprog
```

Unlike Make, `scons -c` is built-in — you don't need to write a `clean` target.

## Header Dependency Tracking

SCons automatically scans source files for `#include` directives and tracks header dependencies. You can view the full dependency tree:

```
$ scons --tree=all
+-.
  +-SConstruct
  +-lib.cpp
  +-lib.h
  +-lib.o
  | +-lib.cpp
  | +-lib.h
  +-main.cpp
  +-main.o
  | +-main.cpp
  | +-lib.h
  +-myprog
    +-main.o
    +-lib.o
```

If you modify `lib.h`, SCons rebuilds both `main.o` and `lib.o` because both depend on it — no `-MMD -MP` flags needed (compare with [ch09](09_header_file_dependencies.md)).

## Out-of-Source Build

To keep build artifacts separate from source files, use `variant_dir` with a `SConscript` file:

**`SConstruct`:**

```python
SConscript('main.scons', variant_dir='build', duplicate=0)
```

**`main.scons`:**

```python
env = Environment()
env.Program(target='myprog', source=['main.cpp', 'lib.cpp'])
```

- **`variant_dir='build'`** — places all build outputs in the `build/` directory.
- **`duplicate=0`** — don't copy source files into the build directory.

## Custom Options

SCons lets you define custom command-line options using `AddOption()`:

```python
AddOption('--enable-debug',
          dest='debug',
          action='store_true',
          default=False,
          help='Enable debug mode')

env = Environment()

if GetOption('debug'):
    env.Append(CCFLAGS=['-g', '-O0'])
else:
    env.Append(CCFLAGS=['-O2'])

env.Program(target='myprog', source=['main.cpp', 'lib.cpp'])
```

```
$ scons --enable-debug    # build with -g -O0
$ scons                   # build with -O2
```

## Aliases

The `Alias` function creates named targets — similar to `.PHONY` targets in Make. This is useful for grouping related build steps:

```python
env = Environment()

prog = env.Program(target='myprog', source=['main.cpp', 'lib.cpp'])
tests = env.Program(target='test_runner', source=['test_main.cpp', 'lib.cpp'])

env.Alias('all', [prog, tests])     # build everything
env.Alias('tests', [tests])         # build only tests
Default(prog)                        # 'scons' with no args builds just myprog
```

```
$ scons              # builds myprog (the default)
$ scons tests        # builds only test_runner
$ scons all          # builds myprog and test_runner
```

## External Libraries

SCons can use `pkg-config` to find system libraries:

```python
env = Environment()
env.ParseConfig('pkg-config --cflags --libs openssl')
env.Program(target='myprog', source='main.cpp')
```

`ParseConfig` runs the shell command and adds the resulting flags (`-I`, `-L`, `-l`) to the build environment — equivalent to `find_package()` in CMake or the `pkg-config` integration we covered in [ch14](14_package_managers.md).

## Limitations

SCons trades some capabilities for simplicity:

- **No IDE project generation** — CMake generates Visual Studio, Xcode, and Eclipse projects. SCons cannot.
- **No built-in testing framework** — CMake ships with CTest. SCons relies on external tools.
- **Smaller community** — fewer tutorials, examples, and online resources compared to CMake.
- **Speed** — for very large projects, SCons' Python-based execution and MD5 scanning can be slower than Make or Ninja.

For most new cross-platform projects, CMake ([ch23](23_cmake.md)) is the more common choice. SCons is well-suited for Python-heavy workflows and projects that benefit from its content-based change detection.

---

← Previous: [CMake](23_cmake.md) · Next: [BitBake](25_bitbake.md) →
