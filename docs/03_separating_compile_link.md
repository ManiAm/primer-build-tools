# Separating Compile and Link

In the [previous chapter](02_gnu_make.md#the-makefile), our Makefile compiled and linked the entire program in a single command. If any source file changed, everything was recompiled — even files that had not been modified.

This chapter introduces **separate compilation**: compiling each source file into an object file independently, then linking the object files into the final executable. This approach lets `make` rebuild only the files that actually changed, which significantly reduces build time on larger projects.

## Compilation vs Linking

Building a C/C++ program involves two distinct steps:

1. **Compiling** — The compiler translates each `.cpp` source file into a `.o` **object file** containing machine code. The `-c` flag tells `g++` to compile without linking.

       g++ -c main.cpp -o main.o
       g++ -c lib.cpp  -o lib.o

2. **Linking** — The linker combines all `.o` files into the final executable.

       g++ main.o lib.o -o myprog

By splitting these steps, `make` can skip recompilation for any source file that has not changed.

## The Makefile — Separating Compile and Link

Here is a Makefile that separates compilation and linking. It uses the same [example project](02_gnu_make.md#example-c-program) from the previous chapter:

```makefile
# Makefile

myprog: main.o lib.o
	g++ main.o lib.o -o myprog

main.o: main.cpp
	g++ -c main.cpp -o main.o

lib.o: lib.cpp
	g++ -c lib.cpp -o lib.o
```

The Makefile now contains three rules:

- **`myprog`** depends on `main.o` and `lib.o`. The recipe links them into the executable.
- **`main.o`** depends on `main.cpp`. The recipe compiles it into an object file.
- **`lib.o`** depends on `lib.cpp`. The recipe compiles it into an object file.

### How `make` Processes These Rules

When you run `make` for the first time:

    $ make
    g++ -c main.cpp -o main.o
    g++ -c lib.cpp -o lib.o
    g++ main.o lib.o -o myprog

`make` starts with the first target, `myprog`, and checks its prerequisites (`main.o` and `lib.o`). Since neither object file exists yet, `make` evaluates their rules first — compiling each source file. Once both object files are ready, `make` returns to the `myprog` rule and links them into the executable.

### Incremental Rebuilds

If you modify only `lib.cpp` and run `make` again:

    $ make
    g++ -c lib.cpp -o lib.o
    g++ main.o lib.o -o myprog

`make` detects that `lib.cpp` is newer than `lib.o`, so it recompiles only `lib.cpp`. It then sees that `lib.o` is now newer than `myprog`, so it re-links. Crucially, `main.o` is not recompiled because `main.cpp` has not changed.

## Suppressing Command Echoing (`@`)

By default, `make` prints (echoes) each command before executing it. This is useful for tracking the build, but it can produce cluttered output.

To suppress echoing for a specific command, prepend the `@` character:

```makefile
main.o: main.cpp
	@g++ -c main.cpp -o main.o
```

With `@` on the compile commands, a full build now prints only the link step:

    $ make
    g++ main.o lib.o -o myprog

To suppress all command echoing without modifying the Makefile, run `make` with the `-s` (silent) flag:

    $ make -s

## Handling Errors

### Default Behavior

`make` stops immediately when a command fails (exits with a non-zero status). This prevents further commands from running when a prerequisite cannot be built.

For example, if `main.cpp` has a syntax error:

    $ make
    g++ -c main.cpp -o main.o
    main.cpp: In function 'int main()':
    main.cpp:6:11: error: expected ';' before '}' token
    make: *** [Makefile:7: main.o] Error 1

`make` aborts without attempting to compile `lib.cpp` or link `myprog`.

### The `-k` (Keep Going) Flag

To continue building independent targets after an error, use the `-k` flag:

    $ make -k
    g++ -c main.cpp -o main.o
    main.cpp: In function 'int main()':
    main.cpp:6:11: error: expected ';' before '}' token
    make: *** [Makefile:7: main.o] Error 1
    g++ -c lib.cpp -o lib.o
    make: Target 'myprog' not remade because of errors.

`make` still compiles `lib.cpp` because it does not depend on `main.o`. However, it skips linking `myprog` because the required `main.o` was not built. This flag is useful on large projects to discover all compilation errors at once.

### The `-` Prefix (Ignore Errors)

Prefixing a recipe line with `-` tells `make` to continue even if that command fails:

```makefile
clean:
	-rm myprog
	-rm *.o
```

If `myprog` does not exist, the first `rm` fails — but `make` continues to the next line and removes the `.o` files. Without the `-` prefix, `make` would stop at the first failure.

> **Note:** Using `rm -f` (which silently ignores missing files) is generally preferred over the `-` prefix for cleanup recipes.

## The `clean` Target

A `clean` target removes generated files (object files, executables, and other build artifacts) so that subsequent builds start from a clean slate. Add the following to the Makefile:

```makefile
clean:
	rm -f myprog
	rm -f *.o
```

Invoke it with:

    $ make clean
    rm -f myprog
    rm -f *.o

The `-f` flag prevents `rm` from returning an error if a file does not exist, making `make clean` safe to run at any time.

### Phony Targets

The `clean` target does not produce a file — it is a label for an action. If a file named `clean` happens to exist in the project directory, `make` would treat the target as up-to-date and skip it.

To prevent this, declare `clean` as a **phony target**:

```makefile
.PHONY: clean
clean:
	rm -f myprog
	rm -f *.o
```

`.PHONY` tells `make` that `clean` is not a real file, so its recipe always runs when invoked. Any target that represents an action rather than a file should be declared phony.

### The `rebuild` Target

A common pattern is a `rebuild` target that cleans and then rebuilds from scratch:

```makefile
.PHONY: rebuild
rebuild: clean myprog
```

Because `rebuild` lists `clean` and `myprog` as prerequisites, `make` runs the `clean` recipe first, then rebuilds `myprog`.

## Printing Messages with `echo`

The `echo` command prints messages to the terminal during a build. Use `@` before `echo` to display only the message, not the command itself:

```makefile
build:
	@echo "Starting the build process..."
	g++ -o myprog main.cpp lib.cpp
	@echo "Build complete!"
```

You can also print environment variables and shell command output:

```makefile
print_info:
	@echo "Home directory: $(HOME)"
	@echo "User: $(USER)"
	@echo "Current user: $(shell whoami)"
	@echo "Date: $(shell date)"
```

`$(HOME)` and `$(USER)` are variables that `make` inherits from the environment. The `$(shell ...)` function executes a shell command during Makefile parsing and substitutes its output. Variables are covered in detail in the [Variables](05_variables.md#environment-variables) chapter.

---

← Previous: [GNU `make`](02_gnu_make.md) · Next: [Implicit Rules](04_implicit_rules.md) →
