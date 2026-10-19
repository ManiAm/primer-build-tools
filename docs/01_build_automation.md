# Build Automation

## What Is Build Automation?

Build automation is the process of using tools to compile source code, link libraries, and produce executable programs — without manually typing each command every time.

In a typical C/C++ project, building software involves several steps:

1. **Compiling** — Translating source files (`.c`, `.cpp`) into object files (`.o`).
2. **Linking** — Combining object files and libraries into an executable or shared library.
3. **Cleaning** — Removing intermediate files to start fresh.

Running these commands by hand is tedious and error-prone, especially as projects grow. Build automation tools solve this by letting you define the steps once and repeat them reliably.

## Why Use a Build Tool?

- **Consistency** — The same commands run the same way every time.
- **Speed** — Most tools detect which files have changed and rebuild only what is necessary (incremental builds).
- **Portability** — Many tools work across different operating systems and compilers.
- **Scalability** — As projects grow from a handful of files to thousands, build tools manage complexity that would be unmanageable by hand.

## Categories of Build Tools

Build tools generally fall into two categories:

- **Build systems** — Tools that directly execute build commands (e.g., Make, Ninja, Bazel, SCons, BitBake).
- **Meta-build systems** — Tools that generate configuration files for another build system rather than executing builds themselves (e.g., CMake and Meson both generate Ninja or Make files).

## Comparison of Common Build Tools

The table below summarizes the most widely used build automation tools.

| Feature                   | Make                          | Autotools                    | CMake                          | SCons                      | BitBake                        | Ninja                          | Meson                                | Bazel                                |
|---------------------------|-------------------------------|------------------------------|--------------------------------|----------------------------|--------------------------------|--------------------------------|--------------------------------------|--------------------------------------|
| **Year Created**          | 1976                          | 1991                         | 2000                           | 2001                       | 2004                           | 2012                           | 2013                                 | 2015                                 |
| **Configuration Language**| Makefile (DSL)                | M4 + Shell Scripts           | CMakeLists.txt (DSL)           | Python                     | Python + Shell Scripts         | Build.ninja (low-level syntax) | Python-like DSL                      | Starlark (Python-like)               |
| **Target Languages**      | Any (built-in rules for C, C++, Fortran) | C, C++, Fortran  | C, C++, C#, Fortran, CUDA, others | Any (via Python)       | Any (focus on embedded Linux)  | Any (language-agnostic)        | C, C++, C#, CUDA, Java, Rust, others | C++, Java, Python, Go, others        |
| **Primary Use Case**      | Unix builds, small projects   | Portable Unix builds         | Cross-platform C/C++ builds    | Python and C/C++ builds    | Embedded Linux (Yocto Project) | Fast, low-overhead builds      | Modern C/C++ builds                  | Large-scale, reproducible builds     |
| **Ease of Use**           | Moderate                      | Difficult                    | Moderate                       | Easy                       | Moderate                       | Moderate (not written by hand) | Easy                                 | Moderate                             |
| **Cross-Platform**        | Limited                       | Limited (Unix-like)          | Excellent                      | Excellent                  | Limited (Linux host)           | Excellent                      | Excellent                            | Excellent                            |
| **Build Speed**           | Moderate                      | Moderate                     | Fast (with Ninja backend)      | Moderate                   | Moderate                       | Very fast                      | Very fast (Ninja backend)            | Very fast (incremental + caching)    |
| **Incremental Builds**    | Supported (timestamp-based)   | Supported (via Make)         | Supported                      | Supported                  | Supported                      | Supported                      | Supported                            | Highly optimized                     |
| **Scalability**           | Low                           | Moderate                     | High                           | Moderate                   | High                           | High                           | High                                 | Very high                            |
| **Dependency Management** | Manual                        | Partial (`pkg-config`)       | Partial (`find_package`)       | Automatic                  | Automatic                      | Manual                         | Automatic                            | Automatic                            |
| **IDE Integration**       | Limited                       | Limited                      | Excellent                      | Moderate                   | Limited                        | Limited                        | Moderate                             | Moderate                             |
| **License**               | GPL                           | GPL                          | BSD 3-Clause                   | MIT                        | GPLv2                          | Apache 2.0                     | Apache 2.0                           | Apache 2.0                           |

## Notable Projects Using Each Tool

The table below lists well-known, actively maintained projects that use each build tool. This can help illustrate where each tool fits in practice.

| Build Tool    | Notable Projects |
|---------------|------------------|
| **Make**      | Linux kernel (Kbuild), Redis, SONiC, Buildroot, OpenBLAS |
| **Autotools** | GCC, GDB, Binutils, Tesseract OCR, aria2 |
| **CMake**     | LLVM/Clang, PyTorch, OpenCV, Qt 6, KeePassXC |
| **SCons**     | Godot Engine, openpilot (comma.ai), gem5 |
| **BitBake**   | Yocto Project, Automotive Grade Linux, OpenBMC, webOS |
| **Ninja**     | Chromium (via GN), Android (via Soong); also the default backend for CMake and Meson |
| **Meson**     | systemd, GNOME (GTK, GLib), Mesa, PipeWire, QEMU, GStreamer |
| **Bazel**     | TensorFlow, MongoDB, Abseil, Envoy, Angular, Protocol Buffers |

> **Note:** Some projects support multiple build systems. For example, Abseil supports both Bazel and CMake, and Git supports both Autotools and Meson. The table lists the primary build system each project uses today.

## Choosing the Right Tool for Your Project

With so many options, picking the right build tool can be confusing. The guide below describes when each tool is justified for a new project. Tools marked as not recommended still work but should generally be avoided for new work.

- **Make** — Use for small to medium C/C++ projects on Linux/Unix where simplicity matters. It is pre-installed on virtually every Unix system, requires no extra dependencies, and is the easiest way to automate a build with just a few source files. Also essential for understanding how other tools work, since many build systems ultimately generate Makefiles. Not ideal once a project grows beyond a few dozen files or needs cross-platform support.

- **Autotools** — **Not recommended for new projects.** It was the standard for portable Unix builds for decades, and many important projects (GCC, GDB) still depend on it. However, it has a steep learning curve, poor Windows support, and no active modernization effort. If you inherit an existing Autotools project, continue maintaining it; otherwise, choose CMake or Meson instead.

- **CMake** — Use for cross-platform C/C++ projects of any size. It has the largest ecosystem, excellent IDE integration (CLion, Visual Studio, VS Code), and is the de facto standard for open-source C/C++ libraries. Most third-party libraries already provide CMake support via `find_package()`, making dependency integration straightforward. Use with the Ninja generator (`cmake -G Ninja`) for the best build speed.

- **SCons** — Use when your team prefers Python and wants build files to be plain Python scripts. SCons provides automatic dependency scanning and a single language for both build logic and application code. Its main drawback is slower build performance on very large projects due to Python overhead.

- **BitBake** — Use exclusively for building embedded Linux images with the Yocto Project or OpenEmbedded. If you are assembling a complete Linux distribution for an embedded device — including kernel, root filesystem, packages, and BSP layers — BitBake is the purpose-built tool for that workflow. It is not a general-purpose build system.

- **Ninja** — Do not use directly. Ninja is a build executor, not a build system you configure by hand. Instead, use CMake (`cmake -G Ninja`) or Meson (which defaults to Ninja) to generate `build.ninja` files. Ninja then executes builds with minimal overhead, making it the fastest option for the edit-compile-test cycle.

- **Meson** — Use for modern C/C++ projects where ease of use is a priority. Meson has a clean, minimal syntax, automatic dependency management, and defaults to Ninja for fast builds. It is an excellent choice for new Linux-focused projects and is increasingly adopted by major open-source projects (systemd, GNOME, Mesa). Its ecosystem is smaller than CMake's, but growing.

- **Bazel** — Use for large-scale monorepos spanning multiple languages (C++, Java, Python, Go). Bazel provides hermetic builds (fully reproducible), remote caching, remote execution, and fine-grained incremental rebuilds. The trade-off is a steeper learning curve and a more opinionated project structure. Best justified when your codebase has millions of lines and multiple teams contributing.

## Further Reading

- [Simple performance comparison of build tools](https://mesonbuild.com/Simple-comparison.html) — benchmark by the Meson project.
- [List of build automation software](https://en.wikipedia.org/wiki/List_of_build_automation_software) — comprehensive catalog on Wikipedia.

---

Next: [GNU `make`](02_gnu_make.md) →
