# Build Tools Primer

## About This Primer

### The Build Tools Landscape

Since the 1970s, dozens of build tools have been developed to automate the process of compiling, linking, and packaging software. Many are **language-specific** — Maven and Gradle for Java, Cargo for Rust, Go's built-in `go build`, and npm/webpack for JavaScript — tightly integrated with their language's ecosystem and package registry.

The tools covered in this primer **originated in the C/C++ and Unix ecosystem**, where building involves separate compile, link, and packaging steps. Many have since evolved to support multiple languages, but their core design remains tied to that compiled-language workflow. Each addressed the limitations of its predecessors — Make (1976) replaced manual shell scripts, Autotools (1991) added portability, CMake (2000) added cross-platform generation, and Meson (2013) and Bazel (2015) pushed further on speed and reproducibility. This primer uses C/C++ examples throughout.

### Why Start with Make

The primer dedicates the most coverage to **GNU Make** because its core concepts — targets, prerequisites, dependency graphs, incremental rebuilds, and parallel execution — are the foundation every other tool builds upon. CMake generates Makefiles. Meson and CMake use Ninja, which shares Make's target-and-dependency model. Bazel extends the same ideas with hermeticity and remote caching. Understanding Make deeply means picking up any of these tools is faster.

### How This Primer Is Structured

The Make chapters follow a **progressive approach**: we start with a minimal one-rule Makefile and improve it step by step — separating compilation and linking, adding variables, automating header dependencies, organizing project structure, and enabling parallel builds. Each chapter introduces one concept and immediately applies it, so the Makefile evolves alongside your understanding.

## Introduction

| #  | Topic                                                                     | Description                                           |
| -- | ------------------------------------------------------------------------- | ----------------------------------------------------- |
| 01 | [Build Automation Tools](docs/01_build_automation.md)                     | Overview of build automation and common tools         |

## GNU Make Basics

| #  | Topic                                                                     | Description                                           |
| -- | ------------------------------------------------------------------------- | ----------------------------------------------------- |
| 02 | [GNU `make`](docs/02_gnu_make.md)                                         | Makefile structure, rules, and basic usage            |
| 03 | [Separating Compile and Link](docs/03_separating_compile_link.md)         | Multi-step builds: compiling and linking separately   |
| 04 | [Implicit Rules](docs/04_implicit_rules.md)                               | Built-in pattern rules and default behaviors          |
| 05 | [Variables](docs/05_variables.md)                                         | Defining and using variables in Makefiles             |
| 06 | [String and Word Manipulation](docs/06_substitution.md)                   | Substitution references and text functions            |
| 07 | [Automatic Variables](docs/07_automatic_variables.md)                     | `$@`, `$<`, `$^`, and other automatic variables       |
| 08 | [Pattern Rules](docs/08_pattern_rules.md)                                 | Defining generic rules with `%` patterns              |

## GNU Make Dependencies & Conditionals

| #  | Topic                                                                     | Description                                            |
| -- | ------------------------------------------------------------------------- | ------------------------------------------------------ |
| 09 | [Header File Dependency](docs/09_header_file_dependencies.md)             | Tracking header changes and auto-dependency generation |
| 10 | [Conditional Statements](docs/10_conditional_statements.md)               | `ifeq`, `ifdef`, and conditional logic in Makefiles    |
| 11 | [Duplicate Targets](docs/11_targets.md)                                   | Handling multiple rules for the same target            |
| 12 | [Order-Only Prerequisites](docs/12_order_only_prerequisite.md)            | Prerequisites that don't trigger rebuilds              |
| 13 | [Library Dependency](docs/13_lib_dependency.md)                           | Linking with static and shared libraries               |
| 14 | [Package Managers](docs/14_package_managers.md)                           | Using `pkg-config` and system package managers         |

## GNU Make Advanced Topics

| #  | Topic                                                                     | Description                                           |
| -- | ------------------------------------------------------------------------- | ----------------------------------------------------- |
| 15 | [Make Install](docs/15_make_install.md)                                   | Installing built artifacts with `make install`        |
| 16 | [Project Structure](docs/16_project_structure.md)                         | Organizing source, headers, and build outputs         |
| 17 | [Parallel Make](docs/17_parallel_build.md)                                | Speeding up builds with `make -j`                     |
| 18 | [Recursive Make](docs/18_recursive_make.md)                               | Sub-directory builds with recursive `make` calls      |
| 19 | [Dry Run / Debugging](docs/19_debugging_make.md)                          | `make -n`, `make -d`, and troubleshooting             |
| 20 | [Platform Dependency](docs/20_make_platform_dependency.md)                | Handling OS and compiler differences                  |

## Build Caching

| #  | Topic                                                                     | Description                                           |
| -- | ------------------------------------------------------------------------- | ----------------------------------------------------- |
| 21 | [Build Caching](docs/21_build_caching.md)                                 | Compiler caching with `ccache` and alternatives       |

## Other Build Systems

| #  | Topic                                                                     | Description                                           |
| -- | ------------------------------------------------------------------------- | ----------------------------------------------------- |
| 22 | [Autotools](docs/22_autotools.md)                                         | `autoconf`, `automake`, and the configure workflow    |
| 23 | [CMake](docs/23_cmake.md)                                                 | Cross-platform build system generator                 |
| 24 | [SCons](docs/24_scons.md)                                                 | Python-based build tool with SConstruct files         |
| 25 | [BitBake](docs/25_bitbake.md)                                             | Task executor for Yocto/OpenEmbedded builds           |
| 26 | [Ninja](docs/26_ninja.md)                                                 | Fast, low-level build system                          |
| 27 | [Meson](docs/27_meson.md)                                                 | Modern build system using Ninja as backend            |
| 28 | [Bazel](docs/28_bazel.md)                                                 | Google's scalable, reproducible build system          |
