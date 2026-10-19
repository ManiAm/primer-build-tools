# Debugging Make

When a build doesn't behave as expected, Make provides several built-in flags to inspect what it's doing and why.

## The Example Makefile

We use this Makefile throughout the chapter — a simplified version of our running project, kept small so the debug output stays readable:

```makefile
CXX      = g++
CXXFLAGS = -Wall -O2

SRC_FILES = main.cpp lib.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

myprog: $(OBJ_FILES)
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f myprog *.o
```

## Dry Run (`-n`)

The `-n` flag (also `--dry-run` or `--just-print`) prints every command Make *would* execute, without actually running anything:

```
$ make -n
g++ -Wall -O2 -c main.cpp -o main.o
g++ -Wall -O2 -c lib.cpp -o lib.o
g++ main.o lib.o -o myprog
```

This is useful for verifying what Make plans to do — especially after editing a Makefile or when running an unfamiliar build for the first time. If your recipes use the `@` prefix to suppress echoing (like `@echo "[CXX] $<"` in our [ch16 Makefile](16_project_structure.md#the-makefile--project-structure)), `-n` still shows every command.

### Limitation

Because nothing actually executes, `-n` can't detect runtime problems. If one recipe generates a file that a later recipe needs, that file won't exist during the dry run — but Make still prints all the commands as if everything would succeed.

## Trace Mode (`--trace`)

The `--trace` flag runs the build normally but adds a line before each recipe showing *why* that target is being rebuilt:

```
$ make --trace
Makefile:10: update target 'main.o' due to: main.cpp
g++ -Wall -O2 -c main.cpp -o main.o
Makefile:10: update target 'lib.o' due to: lib.cpp
g++ -Wall -O2 -c lib.cpp -o lib.o
Makefile:7: update target 'myprog' due to: main.o lib.o
g++ main.o lib.o -o myprog
```

Each trace line shows:
- **Makefile location** — file and line number of the rule (`Makefile:10`).
- **Target** — which target is being rebuilt (`main.o`).
- **Trigger** — which prerequisite caused the rebuild (`main.cpp`).

Unlike `-n`, the build actually runs — so it catches runtime errors that dry run misses.

## Debug Levels (`--debug`)

For deeper inspection, `--debug` accepts a level:

| Level      | Shows                                                    |
| ---------- | -------------------------------------------------------- |
| `basic`    | Why each target is rebuilt                               |
| `verbose`  | Dependency checking, rule evaluation, variable expansion |
| `implicit` | How Make searches for implicit and pattern rules         |
| `jobs`     | Job scheduling (useful with `-j`)                        |
| `all`      | Everything above combined                                |

The `-d` flag is shorthand for `--debug=all`. Start with `basic` and increase only if you need more detail.

### `--debug=basic`

Shows the rebuild decision for each target — whether a file exists, and whether it needs remaking:

```
$ make --debug=basic

File 'myprog' does not exist.
  File 'main.o' does not exist.
  Must remake target 'main.o'.
  Successfully remade target file 'main.o'.
  File 'lib.o' does not exist.
  Must remake target 'lib.o'.
  Successfully remade target file 'lib.o'.
Must remake target 'myprog'.
Successfully remade target file 'myprog'.
```

### `--debug=verbose`

Adds the full dependency walk — Make shows every target it *considers*, even ones it decides not to rebuild:

```
$ make --debug=verbose

Considering target file 'myprog'.
  File 'myprog' does not exist.
  Considering target file 'main.o'.
    File 'main.o' does not exist.
    Considering target file 'main.cpp'.
    Finished prerequisites of target file 'main.cpp'.
    No need to remake target 'main.cpp'.
  Finished prerequisites of target file 'main.o'.
  Must remake target 'main.o'.
  Successfully remade target file 'main.o'.
  ...
```

This helps when a target is unexpectedly being rebuilt (or not rebuilt) — you can trace exactly which prerequisite Make checked and what it decided.

### `--debug=implicit`

Shows which pattern and built-in rules Make tries when matching a target. Useful when a target isn't matching the rule you expect. The output is very verbose — use it only when debugging rule matching issues.

### `--debug=jobs`

Shows job scheduling — when each recipe starts and finishes. Useful with `-j` to verify targets are running in parallel:

```
$ make -j4 --debug=jobs

Putting child (main.o) PID 12345 on the chain.
Reaping winning child PID 12345
Putting child (lib.o) PID 12346 on the chain.
Reaping winning child PID 12346
Putting child (myprog) PID 12347 on the chain.
Reaping winning child PID 12347
```

## Print Database (`-p`)

The `-p` flag dumps Make's internal database — all variables, rules (both user-defined and built-in), and their current values.

By default, `-p` **also runs the build**. Combine with `-n` to print the database without executing anything:

```
$ make -pn
```

The output is long. Pipe it through `grep` to find what you need:

```
$ make -pn | grep CXXFLAGS
CXXFLAGS = -Wall -O2
```

This is useful for checking how Make expanded a variable, or confirming which implicit rules are loaded.

## Warn Undefined Variables (`--warn-undefined-variables`)

This flag catches typos in variable names. If a variable is referenced but never defined, Make prints a warning. Suppose you mistype `CXXFLAGS` as `CXXFLAG` (missing S):

```makefile
CXXFLAG = -Wall -O2    # typo: should be CXXFLAGS
```

```
$ make --warn-undefined-variables
Makefile:11: warning: undefined variable 'CXXFLAGS'
g++   -c -o main.o main.cpp
```

The compile runs without `-Wall -O2` because `CXXFLAGS` (with S) was never defined. Without this flag, Make silently expands the undefined variable to an empty string — the build appears to succeed but with wrong flags.

## Quick Reference

| Flag                         | What it does                                           |
| ---------------------------- | ------------------------------------------------------ |
| `-n` (`--dry-run`)           | Print commands without executing                       |
| `--trace`                    | Run and show why each target is rebuilt                 |
| `--debug=basic`              | Show rebuild decisions                                 |
| `--debug=verbose`            | Show full dependency walk                              |
| `--debug=implicit`           | Show implicit rule search                              |
| `--debug=jobs`               | Show job scheduling (with `-j`)                        |
| `-d`                         | All debug output (`--debug=all`)                       |
| `-p`                         | Print internal database (variables, rules)             |
| `--warn-undefined-variables` | Warn on undefined variable references                  |
| `-q` (`--question`)          | Exit 0 if up to date, 1 otherwise — no build          |
| `-w`                         | Print directory changes (useful in recursive make)     |
| `-s` (`--silent`)            | Suppress all command echoing                           |

---

← Previous: [Recursive Make](18_recursive_make.md) · Next: [Platform Dependency](20_make_platform_dependency.md) →
