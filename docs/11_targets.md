# Target Topics

This chapter covers three target-related topics: what happens when the same target appears more than once, how to set variables for individual targets, and how to write one rule that applies to multiple targets.

## Duplicate Targets

If two rules define the same target, `make` combines their prerequisites but **the second recipe overrides the first**:

```makefile
all: app1
	@echo "first recipe"

all: app2
	@echo "second recipe"

app1:
	@echo "Building app1"

app2:
	@echo "Building app2"
```

    $ make
    Makefile:4: warning: overriding recipe for target 'all'
    Makefile:1: warning: ignoring old recipe for target 'all'
    Building app1
    Building app2
    second recipe

Notice that `make` warns about the override. The prerequisites from both rules are merged (`app1` and `app2` are both built), but only the second recipe runs.

Defining the same target twice is usually a mistake. If you need a target with multiple prerequisites, list them in a single rule:

```makefile
all: app1 app2

app1:
	@echo "Building app1"

app2:
	@echo "Building app2"
```

## Target-Specific Variables

A target-specific variable applies only to a single target and its prerequisites, without affecting the rest of the Makefile:

    target: variable = value

This is useful when different targets need different compiler flags. For example, building a debug version and a release version of the same program:

```makefile
CXXFLAGS = -Wall

debug: CXXFLAGS += -g
release: CXXFLAGS += -O2

debug: myprog
release: myprog

myprog: main.o lib.o
	g++ $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

    $ make debug    # compiles with -Wall -g
    $ make release  # compiles with -Wall -O2

Each target gets its own value of `CXXFLAGS`. The variable assignment does not affect other targets.

## Multiple Targets in a Rule

When several targets share the same recipe, you can list them together instead of writing separate rules for each:

```makefile
app1 app2 app3:
	@echo "Building $@"
```

This is equivalent to writing three separate rules with the same recipe. The automatic variable `$@` expands to whichever target is being built:

    $ make app1
    Building app1

    $ make app2
    Building app2

This pattern is especially useful for phony targets that delegate to the same command:

```makefile
.PHONY: all clean
all clean:
	$(MAKE) -C subdir $@
```

Here, both `make all` and `make clean` run the same recipe, with `$@` passing the target name to the sub-make. Recursive make with `$(MAKE)` and `-C` is covered in detail in a [later chapter](18_recursive_make.md).

---

← Previous: [Conditional Statements](10_conditional_statements.md) · Next: [Order-Only Prerequisites](12_order_only_prerequisite.md) →
