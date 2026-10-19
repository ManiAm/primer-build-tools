# Header File Dependencies

Our Makefile from the [previous chapter](08_pattern_rules.md#the-makefile--adding-a-pattern-rule) tracks dependencies between `.cpp` and `.o` files — if a source file changes, its object file is recompiled. But there is a gap: **header files are not tracked**. If you modify `lib.h`, `make` does not know that `main.cpp` and `lib.cpp` include it, so it skips recompilation. This can lead to linker errors or subtle runtime bugs from outdated object files.

This chapter covers how to make `make` aware of header file dependencies.

## The Problem

Consider our running example where both `main.cpp` and `lib.cpp` include `lib.h`. After a successful build, modifying `lib.h` and running `make` produces:

    $ make
    make: 'myprog' is up to date.

`make` sees that `main.cpp` and `lib.cpp` have not changed, so it does nothing. The change to `lib.h` is invisible because `lib.h` is not listed as a prerequisite in any rule.

## Manual Dependencies

The simplest fix is to add the header as a prerequisite to the pattern rule:

```makefile
%.o: %.cpp lib.h
	@echo "[CXX] $<"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
```

Now, any change to `lib.h` triggers recompilation of **all** `.cpp` files. This works for small projects, but it has two drawbacks:

- You must manually list every header — easy to forget one.
- Every `.o` file depends on every listed header, even if it does not include them. This causes unnecessary recompilation.

## Automatic Dependency Generation

The compiler already knows which headers each source file includes. GCC and Clang can output this information as a **dependency file** (`.d`) during compilation.

### Generating `.d` Files

The `-MMD` flag tells the compiler to produce a `.d` file alongside the object file:

    $ g++ -c -MMD main.cpp -o main.o
    $ g++ -c -MMD lib.cpp -o lib.o

This creates `main.d` and `lib.d`:

```makefile
# main.d
main.o: main.cpp lib.h
```

```makefile
# lib.d
lib.o: lib.cpp lib.h
```

Each `.d` file is a Makefile fragment — it contains a rule listing the exact headers that source file depends on. By including these files in the Makefile, `make` learns the complete dependency graph.

### The `-MP` Flag

It is good practice to also pass `-MP`, which adds an empty rule for each header:

```makefile
# lib.d (with -MP)
lib.o: lib.cpp lib.h

lib.h:
```

The empty `lib.h:` rule prevents `make` from failing if the header is later deleted or renamed. Without it, `make` would report an error because it cannot find a rule to build the missing file.

### Including `.d` Files

The `-include` directive loads the generated `.d` files into the Makefile:

```makefile
DEP_FILES = $(OBJ_FILES:.o=.d)
-include $(DEP_FILES)
```

The leading `-` tells `make` to silently continue if the `.d` files do not exist yet (which is the case on the first build — the `.d` files are created during compilation).

> **Note:** Without the `-`, `make` would fail on a clean build because it cannot find the `.d` files. The `-include` directive avoids this.

## The Makefile — Adding Header Dependencies

Here is the updated Makefile with automatic header dependency tracking:

```makefile
CXXFLAGS = -Wall -O2 -MMD -MP

SRC_FILES = main.cpp lib.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)
DEP_FILES = $(OBJ_FILES:.o=.d)

myprog: $(OBJ_FILES)
	g++ $^ -o $@

%.o: %.cpp
	@echo "[CXX] $<"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(DEP_FILES)

.PHONY: clean
clean:
	rm -f myprog *.o *.d

.PHONY: rebuild
rebuild: clean myprog
```

Compared to the [ch08 Makefile](08_pattern_rules.md#the-makefile--adding-a-pattern-rule), the changes are:

- `-MMD -MP` added to `CXXFLAGS` — the compiler now generates `.d` files during compilation.
- `DEP_FILES` variable computes the list of `.d` files from `OBJ_FILES`.
- `-include $(DEP_FILES)` loads the dependency information.
- The `clean` target now also removes `*.d` files.

On the first build:

    $ make
    [CXX] main.cpp
    g++ -Wall -O2 -MMD -MP   -c main.cpp -o main.o
    [CXX] lib.cpp
    g++ -Wall -O2 -MMD -MP   -c lib.cpp -o lib.o
    g++ main.o lib.o -o myprog

Now, modifying only `lib.h` triggers the correct recompilation:

    $ touch lib.h
    $ make
    [CXX] main.cpp
    g++ -Wall -O2 -MMD -MP   -c main.cpp -o main.o
    [CXX] lib.cpp
    g++ -Wall -O2 -MMD -MP   -c lib.cpp -o lib.o
    g++ main.o lib.o -o myprog

Both files are recompiled because both include `lib.h`, as recorded in their `.d` files.

---

← Previous: [Pattern Rules](08_pattern_rules.md) · Next: [Conditional Statements](10_conditional_statements.md) →
