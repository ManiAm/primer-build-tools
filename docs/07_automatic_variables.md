# Automatic Variables

Here is the Makefile from the [previous chapter](06_substitution.md#the-makefile--adding-variables):

```makefile
CXXFLAGS = -Wall -O2

SRC_FILES = main.cpp lib.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

myprog: $(OBJ_FILES)
	g++ -o myprog $(OBJ_FILES)
```

The link rule still repeats `$(OBJ_FILES)` and the target name `myprog` in the recipe. **Automatic variables** let you eliminate this repetition — they are special variables that `make` sets automatically for each rule, based on its target and prerequisites.

## Reference

| Variable | Expands to                                                       | Example (given `myprog: main.o lib.o`) |
|----------|------------------------------------------------------------------|----------|
| `$@`     | The target name                                                  | `myprog` |
| `$<`     | The first prerequisite                                           | `main.o` |
| `$^`     | All prerequisites (duplicates removed)                           | `main.o lib.o` |
| `$?`     | Prerequisites newer than the target                              | *(only those that changed)* |
| `$*`     | The stem matched by `%` in a [pattern rule](08_pattern_rules.md) | *(covered in the next chapter)* |

> **Important:** Automatic variables are only valid inside **recipes**. They cannot be used in the target or prerequisite lists.

## The Makefile — Automatic Variables

Using `$@` (target) and `$^` (all prerequisites), the link rule becomes:

```makefile
myprog: $(OBJ_FILES)
	g++ $^ -o $@
```

- `$^` expands to all prerequisites: `main.o lib.o`
- `$@` expands to the target: `myprog`

The updated Makefile:

```makefile
CXXFLAGS = -Wall -O2

SRC_FILES = main.cpp lib.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

myprog: $(OBJ_FILES)
	g++ $^ -o $@
```

    $ make
    g++ -Wall -O2   -c -o main.o main.cpp
    g++ -Wall -O2   -c -o lib.o lib.cpp
    g++ main.o lib.o -o myprog

The output is identical — but the recipe no longer hard-codes any file names. If you add a new source file to `SRC_FILES`, the link rule automatically includes it without any changes.

> **Note:** The compilation commands (`g++ -Wall -O2 -c ...`) are handled by [implicit rules](04_implicit_rules.md), which already use `$<` and `$@` internally. In the [next chapter](08_pattern_rules.md), you will write your own pattern rule where `$<` becomes essential.

## `$?` — Newer Prerequisites

`$?` expands to only the prerequisites that are **newer than the target**. For example, if you modify only `lib.cpp` after a successful build:

    $ touch lib.cpp
    $ make
    g++ -Wall -O2   -c -o lib.o lib.cpp
    g++ main.o lib.o -o myprog

When `make` evaluates the `myprog` rule at this point, `$?` would contain only `lib.o` (the one that changed), while `$^` would still contain `main.o lib.o` (all prerequisites).

---

← Previous: [String and Word Manipulation](06_substitution.md) · Next: [Pattern Rules](08_pattern_rules.md) →
