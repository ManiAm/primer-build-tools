# String and Word Manipulation

As covered in [Everything Is a String](05_variables.md#everything-is-a-string), all values in Make are plain text. GNU Make provides built-in functions to manipulate these strings — replacing substrings, transforming patterns, and filtering word lists. These functions are essential for working with lists of source files and compiler flags.

| Function / Syntax               | Purpose                             | Example                     |
|---------------------------------|-------------------------------------|-----------------------------|
| `$(subst FROM,TO,TEXT)`         | Replace exact substrings            | `$(subst .tmp,.c,$(FILES))` |
| `$(patsubst PATTERN,REPL,TEXT)` | Replace using `%` wildcard patterns | `$(patsubst %.c,%.o,$(FILES))` |
| `$(VAR:SUFFIX=REPL)`            | Shorthand suffix replacement        | `$(SRC:.c=.o)`              |
|                                 |                                     |                             |
| `$(filter PATTERN,TEXT)`        | Keep words matching a pattern       | `$(filter %.c,$(FILES))` |
| `$(filter-out PATTERN,TEXT)`    | Remove words matching a pattern     | `$(filter-out %.c,$(FILES))` |


## Substitution Functions

### `subst` — Literal Replacement

`subst` replaces all occurrences of an exact substring with another string:

    $(subst FROM,TO,TEXT)

```makefile
FILES = main.tmp util.tmp test.tmp
RENAMED = $(subst .tmp,.c,$(FILES))

all:
	@echo $(RENAMED)
```

    $ make
    main.c util.c test.c

The replacement is purely literal — `subst` has no wildcard or pattern matching. Every occurrence of `.tmp` is replaced with `.c`.

### `patsubst` — Pattern Replacement

`patsubst` replaces words that match a pattern containing the `%` wildcard. The `%` matches any sequence of characters:

    $(patsubst PATTERN,REPLACEMENT,TEXT)

```makefile
FILES = main.c util.c test.c
OBJS = $(patsubst %.c,%.o,$(FILES))

all:
	@echo $(OBJS)
```

    $ make
    main.o util.o test.o

For each word in `FILES`, `patsubst` checks if it matches `%.c`. If it does, the part matched by `%` (e.g., `main`) is substituted into `%.o`, producing `main.o`.

> **Note:** The `%` wildcard is not a regular expression. It simply matches any string of characters within a single word.

### Substitution References — Shorthand Syntax

A **substitution reference** provides a shorter way to replace suffixes in a variable's value:

    $(VAR:SUFFIX=REPLACEMENT)

```makefile
SRC = main.c util.c test.c
OBJ = $(SRC:.c=.o)

all:
	@echo $(OBJ)
```

    $ make
    main.o util.o test.o

This is equivalent to `$(patsubst %.c,%.o,$(SRC))` but more concise. It only replaces at the **end of each word** — it cannot replace substrings in the middle.


## Filter Functions

### `filter` — Keep Matching Words

`filter` keeps only the words that match the given pattern and discards the rest:

```makefile
FILES = main.c util.h test.c test.h
SRC = $(filter %.c,$(FILES))

all:
	@echo $(SRC)
```

    $ make
    main.c test.c

### `filter-out` — Remove Matching Words

`filter-out` is the opposite of `filter` — it removes words that match and keeps everything else:

```makefile
FILES = main.c util.h test.c test.h
HEADERS = $(filter-out %.c,$(FILES))

all:
	@echo $(HEADERS)
```

    $ make
    util.h test.h


## The Makefile — Adding Variables

Returning to the running Makefile, we can use a substitution reference to automatically generate the list of object files from source files:

```makefile
CXXFLAGS = -Wall -O2

SRC_FILES = main.cpp lib.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

myprog: $(OBJ_FILES)
	g++ -o myprog $(OBJ_FILES)

.PHONY: clean
clean:
	rm -f myprog *.o

.PHONY: rebuild
rebuild: clean myprog
```

    $ make
    g++ -Wall -O2   -c -o main.o main.cpp
    g++ -Wall -O2   -c -o lib.o lib.cpp
    g++ -o myprog main.o lib.o

The key benefit: adding a new source file to the project requires changing only one line — append its name to `SRC_FILES`. The corresponding object file is generated automatically by the substitution reference, and `make` handles compilation via [implicit rules](04_implicit_rules.md).

---

← Previous: [Variables](05_variables.md) · Next: [Automatic Variables](07_automatic_variables.md) →
