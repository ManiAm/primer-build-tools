# Pattern Rules

So far, our Makefile relies on [implicit rules](04_implicit_rules.md) to compile `.cpp` files into `.o` files. This works, but implicit rules are a black box — you can set variables like `CXXFLAGS`, but you cannot change what the recipe actually does. For example, you cannot add a progress message, run an extra command, or apply different logic for different file types.

A **pattern rule** lets you write your own generic rule using the `%` wildcard, giving you full control over the recipe while still applying it to all matching files.

## How Pattern Rules Work

A pattern rule looks like a regular rule, but its target contains a `%` character. The `%` matches any nonempty string, and the same match is substituted into the prerequisites:

```makefile
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
```

When `make` needs to build `main.o`, it matches `%.o` with `%` = `main`, substitutes into `%.cpp` to get `main.cpp`, and runs the recipe. The same rule handles `lib.o`, or any other `.o` target with a corresponding `.cpp` file.

> **Note:** The `%` wildcard is not a regular expression. It simply matches any sequence of characters, similar to how it works in the [`patsubst` function](06_substitution.md#patsubst--pattern-replacement).

## The `$*` Variable (Stem)

The part of the file name matched by `%` is called the **stem**. It is available in the recipe through the automatic variable `$*`:

```makefile
%.o: %.cpp
	@echo "Stem: $*  |  Source: $<  |  Target: $@"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
```

    $ make main.o
    Stem: main  |  Source: main.cpp  |  Target: main.o
    g++   -c main.cpp -o main.o

When the target is `main.o`, the stem `$*` is `main` — the part matched by `%`.

## Why Use a Pattern Rule Over an Implicit Rule?

With an implicit rule, you can only control the build through variables. A pattern rule lets you customize the recipe itself. For example, adding a progress message that shows which file is being compiled:

```makefile
%.o: %.cpp
	@echo "[CXX] $<"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
```

    $ make
    [CXX] main.cpp
    g++ -Wall -O2   -c main.cpp -o main.o
    [CXX] lib.cpp
    g++ -Wall -O2   -c lib.cpp -o lib.o
    g++ main.o lib.o -o myprog

This is not possible with an implicit rule — the built-in recipe is fixed. Pattern rules also let you handle file transformations that have no built-in implicit rule at all (e.g., generating code from `.proto` or `.yaml` files).

## The Makefile — Adding a Pattern Rule

Here is the updated Makefile with a pattern rule replacing the implicit compile rule:

```makefile
CXXFLAGS = -Wall -O2

SRC_FILES = main.cpp lib.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

myprog: $(OBJ_FILES)
	g++ $^ -o $@

%.o: %.cpp
	@echo "[CXX] $<"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f myprog *.o

.PHONY: rebuild
rebuild: clean myprog
```

    $ make
    [CXX] main.cpp
    g++ -Wall -O2   -c main.cpp -o main.o
    [CXX] lib.cpp
    g++ -Wall -O2   -c lib.cpp -o lib.o
    g++ main.o lib.o -o myprog

Compared to the [ch07 Makefile](07_automatic_variables.md#the-makefile--automatic-variables), the only addition is the `%.o: %.cpp` pattern rule with the `[CXX]` progress message. The implicit rule is replaced by an explicit, customizable recipe.

## Static Pattern Rules

A regular pattern rule (like `%.o: %.cpp`) applies globally — it matches **any** `.o` target in the project. A **static pattern rule** restricts the match to a specific list of targets:

    targets: target-pattern: prerequisite-pattern
        recipe

```makefile
OBJS = main.o lib.o

$(OBJS): %.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
```

This rule only applies to `main.o` and `lib.o`. If another `.o` file exists in the project (e.g., from a test or a third-party library), it would **not** be matched by this rule. Static pattern rules are useful in larger projects where different groups of files need different compilation flags or different recipes.

## Suffix Rules (Legacy)

Before pattern rules were introduced, Make used **suffix rules** to define file transformations. A suffix rule uses a pair of file extensions as the target:

```makefile
.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
```

This is equivalent to the pattern rule `%.o: %.cpp`. Suffix rules still work in GNU Make for backward compatibility, but pattern rules are preferred because they are clearer and more flexible.

---

← Previous: [Automatic Variables](07_automatic_variables.md) · Next: [Header File Dependency](09_header_file_dependencies.md) →
