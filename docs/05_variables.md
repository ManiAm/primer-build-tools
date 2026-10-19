# Variables

Variables in Makefiles let you store and reuse text values — compiler names, flags, file lists, and more. Instead of repeating the same string in multiple places, you define it once in a variable and reference it wherever needed. This makes Makefiles easier to read, maintain, and modify.

GNU Make supports several types of variables, each with different assignment and evaluation behavior:

| Type               | Operator | Behavior                                  |
|--------------------|----------|-------------------------------------------|
| Simple (Immediate) | `:=`     | Evaluated once, at the time of definition |
| Recursive          | `=`      | Evaluated each time the variable is referenced |
| Append             | `+=`     | Adds to an existing variable's value |
| Conditional        | `?=`     | Assigns only if the variable is not already defined |

> **Note:** Automatic variables (`$@`, `$<`, `$^`, etc.) are a special category covered in the [Automatic Variables](07_automatic_variables.md) chapter.

## Referencing Variables

To use a variable's value, wrap its name in `$(...)` or `${...}`:

```makefile
NAME = John

greet:
	@echo $(NAME)
	@echo ${NAME}
```

Both forms are equivalent. `$(...)` is the more common convention.

### `$(VAR)` vs `$$VAR`

Inside a recipe, there are two ways to reference a variable:

- **`$(VAR)`** — Expanded by `make` before the command is passed to the shell. Uses the value `make` knows about (from the Makefile, environment, or command line).
- **`$$VAR`** — The double `$$` escapes to a single `$`, so the shell sees `$VAR` and expands it at runtime.

```makefile
all:
	@echo "make sees: $(USER)"
	@echo "shell sees: $$USER"
```

In most cases, both produce the same result. The distinction matters when a variable is set or modified inside the recipe's shell commands — `$$VAR` reflects the runtime shell environment, while `$(VAR)` is fixed at parse time.

## Everything Is a String

Unlike C/C++ or Python, **Make has no data types**. Every variable value is a plain text string — there are no integers, booleans, or lists. For example, the following does **not** perform arithmetic:

```makefile
X := 5 + 10

all:
	@echo $(X)
```

    $ make
    5 + 10

The output is the literal string `5 + 10`, not `15`. The characters `5`, `+`, and `10` are just text. Make provides built-in [string manipulation functions](06_substitution.md) (such as `subst` and `patsubst`) to work with these values, but it has no concept of numbers or arithmetic.

## Assignment Operators

### 1. Simple Variables (`:=`)

Simple variables are evaluated **immediately** when defined. Their value is fixed from that point onward — later changes to the variables they reference have no effect.

```makefile
GREETING := hello
MESSAGE := $(GREETING) world
GREETING := goodbye

all:
	@echo $(MESSAGE)
```

    $ make
    hello world

When `make` parses `MESSAGE := $(GREETING) world`, it expands `$(GREETING)` right away (which is `hello` at that point) and stores the result `hello world`. Reassigning `GREETING` to `goodbye` afterward does not change `MESSAGE`.

### 2. Recursive Variables (`=`)

Recursive variables are evaluated **lazily** — their value is determined each time they are referenced, not when they are defined. This allows them to reflect changes in the variables they depend on.

```makefile
GREETING := hello
MESSAGE = $(GREETING) world
GREETING := goodbye

all:
	@echo $(MESSAGE)
```

    $ make
    goodbye world

Here, `MESSAGE` stores the unexpanded expression `$(GREETING) world`. When `make` expands `$(MESSAGE)` during execution, it evaluates `$(GREETING)` at that moment — which is `goodbye`.

Recursive variables are useful when a value needs to depend on variables that may be set later in the Makefile. However, be careful not to create circular references (e.g., `A = $(B)` and `B = $(A)`), as this causes an infinite loop.

### 3. Append Variables (`+=`)

The `+=` operator adds content to an existing variable without overwriting its previous value:

```makefile
CFLAGS := -Wall
CFLAGS += -O2
CFLAGS += -g

all:
	@echo $(CFLAGS)
```

    $ make
    -Wall -O2 -g

`+=` works with both simple and recursive variables. With `:=` variables, the appended value is evaluated immediately. With `=` variables, it is evaluated lazily when referenced.

### 4. Conditional Variables (`?=`)

The `?=` operator assigns a value only if the variable is **not already defined**. This is useful for providing defaults that can be overridden:

```makefile
CC ?= gcc

all:
	@echo $(CC)
```

If `CC` is not set anywhere else:

    $ make
    gcc

If overridden from the command line:

    $ make CC=clang
    clang

## Variable Sources

Variables defined in the Makefile (using `:=`, `=`, `+=`, or `?=`) are only one source. `make` can also receive variables from two other places:

### Environment Variables

`make` automatically inherits environment variables from the shell. You can reference them in a Makefile just like any other variable:

```makefile
print_env:
	@echo "Home directory: $(HOME)"
	@echo "User: $(USER)"
```

### Command-Line Variables

You can pass variables directly when invoking `make`:

    $ make CFLAGS=-O2

This sets `CFLAGS` to `-O2` for that build, without modifying the Makefile.

### Precedence

When the same variable is defined in multiple places, `make` follows a strict priority order (highest to lowest):

| Priority    | Source                     | Example |
|-------------|----------------------------|---|
| 1 (highest) | Command line               | `make CFLAGS=-O2` |
| 2           | Makefile definition        | `CFLAGS = -g -Wall` |
| 3           | Environment variable       | `export CFLAGS=-O2` then `make` |
| 4 (lowest)  | Conditional default (`?=`) | `CFLAGS ?= -g` |

For example, given this Makefile:

```makefile
CFLAGS = -g -Wall

all:
	@echo $(CFLAGS)
```

The Makefile definition wins over an environment variable:

    $ export CFLAGS=-O2
    $ make
    -g -Wall

But a command-line variable wins over the Makefile definition:

    $ make CFLAGS=-O2
    -O2

### The `override` Directive

Command-line variables normally have the highest priority. The `override` directive reverses this — it forces the Makefile's value to take priority:

```makefile
override CFLAGS = -g -Wall

all:
	@echo $(CFLAGS)
```

    $ make CFLAGS=-O2
    -g -Wall

Without `override`, the output would be `-O2`. With it, the Makefile's value wins regardless.

## Variable Evaluation Phases

As described in [How `make` Executes](02_gnu_make.md#two-phases), `make` parses the entire Makefile before executing any recipe. This has a direct consequence for variables — a definition that appears later in the file can affect recipes that appear earlier:

```makefile
MSG := first

all:
	@echo $(MSG)

MSG := second
```

    $ make
    second

During parsing, `make` first sets `MSG` to `first`, then overwrites it with `second`. By the time the `all` recipe executes, `MSG` is already `second`. The position of the `all` rule relative to the second `MSG := second` does not matter — parsing completes before any recipe runs.

---

← Previous: [Implicit Rules](04_implicit_rules.md) · Next: [String and Word Manipulation](06_substitution.md) →
