# Implicit Rules

In the [previous chapter](03_separating_compile_link.md#the-makefile--separating-compile-and-link), we wrote explicit rules to compile each `.cpp` file into a `.o` object file. For common tasks like this, GNU Make provides **implicit rules** — built-in rules that `make` applies automatically when no explicit recipe is given. This means you can omit the compilation rules entirely and let `make` figure out how to build object files on its own.

## How Implicit Rules Work

When `make` needs to build a target and the Makefile does not provide a recipe for it, `make` searches its built-in implicit rules for a match. For object files, the logic is:

1. `make` sees that a target like `main.o` has no recipe.
2. It looks for a source file with a matching base name — in this case `main.cpp` (or `main.c`).
3. If a matching source file exists, `make` applies the corresponding built-in compilation rule.

## Built-in Compilation Rules

GNU Make has built-in rules for the most common compilation tasks. The two used most often are:

**C source files** — A `.c` file is compiled into a `.o` file using:

    $(CC) $(CPPFLAGS) $(CFLAGS) -c <source>.c -o <source>.o

**C++ source files** — A `.cpp` file is compiled into a `.o` file using:

    $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c <source>.cpp -o <source>.o

These rules use a set of predefined variables that you can customize:

| Variable   | Purpose | Default Value |
|------------|---|---|
| `CC`       | The C compiler | `cc` (typically `gcc`) |
| `CXX`      | The C++ compiler | `g++` |
| `CFLAGS`   | Flags passed to the C compiler (e.g., `-Wall`, `-O2`) | *(empty)* |
| `CXXFLAGS` | Flags passed to the C++ compiler (e.g., `-Wall`, `-O2`) | *(empty)* |
| `CPPFLAGS` | Preprocessor flags shared by both C and C++ (e.g., `-I` include paths, `-D` macro definitions) | *(empty)* |

> **Note:** `CPPFLAGS` stands for "C PreProcessor Flags", not "C++ Flags." It is common to both C and C++ compilation because both languages use the same preprocessor.

For the full list of variables used by implicit rules, see the [GNU Make manual — Implicit Variables](https://www.gnu.org/software/make/manual/make.html#Implicit-Variables).

## The Makefile — Using Implicit Rules

Here is the [Makefile from the previous chapter](03_separating_compile_link.md#the-makefile--separating-compile-and-link) with explicit compilation rules:

```makefile
myprog: main.o lib.o
	g++ main.o lib.o -o myprog

main.o: main.cpp
	g++ -c main.cpp -o main.o

lib.o: lib.cpp
	g++ -c lib.cpp -o lib.o
```

Because `make` already knows how to compile `.cpp` files into `.o` files, we can remove the two compilation rules entirely:

```makefile
myprog: main.o lib.o
	g++ main.o lib.o -o myprog
```

That is the entire Makefile. When you run `make`:

    $ make
    g++    -c -o main.o main.cpp
    g++    -c -o lib.o lib.cpp
    g++ main.o lib.o -o myprog

`make` sees that `myprog` depends on `main.o` and `lib.o`. Neither has an explicit rule, so `make` finds `main.cpp` and `lib.cpp` in the directory and applies the built-in C++ compilation rule to produce the object files. Once they are ready, it runs the explicit link recipe.

## Customizing with Variables

You can control the implicit rules by setting the predefined variables. For example, adding `CXXFLAGS` enables compiler warnings and optimizations:

```makefile
CXXFLAGS = -Wall -O2

myprog: main.o lib.o
	g++ main.o lib.o -o myprog
```

Now the implicit compilation commands include those flags:

    $ make
    g++ -Wall -O2   -c -o main.o main.cpp
    g++ -Wall -O2   -c -o lib.o lib.cpp
    g++ main.o lib.o -o myprog

You do not need to modify any recipes — `make` automatically incorporates `CXXFLAGS` into the built-in C++ compilation rule.

---

← Previous: [Separating Compile and Link](03_separating_compile_link.md) · Next: [Variables](05_variables.md) →
