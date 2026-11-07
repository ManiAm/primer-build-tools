# Meson

Meson is a modern, open-source build system designed to be fast, user-friendly, and portable across Linux, Windows, and macOS. It uses **Ninja** ([ch26](26_ninja.md)) as its default backend for fast incremental builds. The developer describes *what* to build in a `meson.build` file; Meson generates the low-level build files and Ninja executes them.

Meson was created by Jussi Pakkanen in 2013. The name is inspired by particle physics — mesons are subatomic particles, reflecting the project's goal of being small and fast. Notable projects that use Meson include:

- **systemd** — Linux init system and service manager
- **GNOME** — desktop environment (GTK, GLib)
- **Mesa** — open-source GPU drivers (OpenGL, Vulkan)
- **PipeWire** — multimedia server (audio and video)
- **QEMU** — machine emulator and virtualizer
- **GStreamer** — multimedia framework

## Meson vs CMake

Both Meson and CMake ([ch23](23_cmake.md)) are build system generators that produce Ninja or Make files. The key differences:

| Aspect             | CMake                                   | Meson                                       |
| ------------------ | --------------------------------------- | ------------------------------------------- |
| Input file         | `CMakeLists.txt` (custom DSL)           | `meson.build` (Python-like DSL)             |
| Default backend    | Unix Makefiles (Linux)                  | Ninja                                       |
| Language style     | Command-based (`add_executable(...)`)   | Function-based (`executable(...)`)          |
| Learning curve     | Moderate                                | Low (clean, readable syntax)                |
| Implementation     | C++                                     | Python 3                                    |

Meson requires Python 3 to run. However, the Meson build language is not Python — it has its own domain-specific language (DSL) with Python-like syntax, simplified for build configuration.

## Installation

```
$ pip install meson                 # via Python (any platform)
$ sudo apt install meson            # Debian/Ubuntu
$ sudo dnf install meson            # Fedora
$ brew install meson                # macOS
```

Ninja is also required as the build backend:

```
$ sudo apt install ninja-build      # Debian/Ubuntu
$ sudo dnf install ninja-build      # Fedora
$ brew install ninja                # macOS
```

## Basic Example

A simple project:

```
project/
├── meson.build
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

### The `meson.build` File

```meson
project('example_project', 'cpp', version: '1.0.0')

executable('myprog', ['main.cpp', 'lib.cpp'])
```

Two lines:
- **`project`** — names the project, specifies the language (`'cpp'`), and sets the version.
- **`executable`** — defines a build target and its source files.

### Building

Meson uses an **out-of-source build** — generated files go in a separate directory, keeping the source tree clean:

```
$ meson setup build        # generate build files in build/
$ meson compile -C build   # compile the project
$ ./build/myprog           # run the executable
Hello from lib!
```

- **`meson setup build`** — configures the project and generates `build.ninja` inside the `build/` directory.
- **`meson compile -C build`** — invokes Ninja to compile. You can also run `ninja -C build` directly.

## Header Dependency Tracking

Meson automatically tracks header dependencies — you do not need to list headers in `meson.build`:

```
$ meson compile -C build
[2/2] Linking target myprog

$ touch lib.h                       # modify the header

$ meson compile -C build
[1/2] Compiling C++ object myprog/lib.cpp.o
[2/2] Linking target myprog
```

Only the affected source files are recompiled. Meson handles this internally through Ninja's dependency tracking, similar to the `-MMD -MP` flags we added manually in [ch09](09_header_file_dependencies.md).

## External Libraries (`dependency`)

To use a library installed on the system, Meson provides `dependency()`:

```
project/
├── meson.build
└── main.cpp
```

```cpp
// main.cpp
#include <iostream>
#include <cstring>
#include <openssl/sha.h>

int main()
{
    const char* text = "Hello, World!";
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(reinterpret_cast<const unsigned char*>(text), strlen(text), hash);

    std::cout << "SHA256 Hash: ";
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    std::cout << std::endl;

    return 0;
}
```

```meson
project('openssl_example', 'cpp', version: '1.0.0')

openssl_dep = dependency('openssl', required: true)

executable('myprog', 'main.cpp', dependencies: openssl_dep)
```

- **`dependency('openssl', required: true)`** — searches for OpenSSL using `pkg-config`. If not found, Meson stops with an error.
- **`dependencies: openssl_dep`** — links the dependency to the executable, automatically providing the correct include paths and link flags.

Build:

```
$ meson setup build
$ meson compile -C build
```

Install missing libraries with your system package manager:

```
$ sudo apt install libssl-dev      # Debian/Ubuntu
$ sudo dnf install openssl-devel   # Fedora/RHEL
$ brew install openssl             # macOS
```

---

← Previous: [Ninja](26_ninja.md) · Next: [Bazel](28_bazel.md) →
