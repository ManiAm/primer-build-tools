# Library Dependencies

When your program uses functionality from an external library (e.g., OpenSSL, zlib, or ncurses), you must tell the compiler where to find the library's headers and tell the linker which library to link against. This chapter covers how to handle library dependencies in a Makefile.

## Static vs Shared Libraries

Libraries come in two forms:

- **Static library** (`.a`) — For example, `libfoo.a`. The library code is copied into your executable at link time. The resulting binary is self-contained but larger.

- **Shared library** (`.so`, `.dylib`) — For example, `libfoo.so`. The library is loaded at runtime. The binary is smaller, but the library must be present on the system when the program runs.

## Linker Flags

Two flags control library linking:

- **`-l<name>`** — Link against a library. For example, `-lssl` links against `libssl.so` or `libssl.a`.
- **`-L<path>`** — Add a directory to the library search path. For example, `-L/opt/lib` tells the linker to also look in `/opt/lib`.

These flags are typically stored in a variable called `LDFLAGS` or `LDLIBS`:

```makefile
LDLIBS = -lssl -lcrypto
```

## Static vs Shared: How the Linker Chooses

When you pass `-lfoo`, the linker searches its library paths and **prefers the shared library** by default:

1. Look for `libfoo.so` (shared) — if found, use it.
2. If no `.so` exists, fall back to `libfoo.a` (static).

To override this default:

- **`-static`** — Force all libraries to link statically: `g++ -static -o myprog main.o -lfoo`
- **`-Wl,-Bstatic` / `-Wl,-Bdynamic`** — Control per library. Everything between `-Bstatic` and `-Bdynamic` is linked statically:

      g++ -o myprog main.o -Wl,-Bstatic -lfoo -Wl,-Bdynamic -lbar

  Here `foo` is linked statically and `bar` is linked shared. The `-Wl,` prefix passes the flag through the compiler driver to the linker.

- **Full path** — Bypass `-l` entirely by specifying the archive directly: `g++ -o myprog main.o /usr/lib/libfoo.a`

Shared linking is the default because it produces smaller binaries and allows multiple programs to share one copy of the library in memory. Static linking is used when you need a self-contained binary that does not depend on libraries being installed on the target system.

## Using `pkg-config`

Manually finding the right `-I`, `-l`, and `-L` flags for a library can be tedious. `pkg-config` is a standard Unix tool that provides these flags automatically:

    $ pkg-config --cflags openssl    # compiler flags (e.g., -I/usr/include/openssl)
    $ pkg-config --libs openssl      # linker flags (e.g., -lssl -lcrypto)

In a Makefile, use the `$(shell ...)` function to call `pkg-config` and capture its output:

```makefile
CXXFLAGS += $(shell pkg-config --cflags openssl)
LDLIBS   = $(shell pkg-config --libs openssl)
```

`$(shell ...)` executes a shell command during Makefile parsing and substitutes the output. `pkg-config` is covered in more detail in the [Package Managers](14_package_managers.md) chapter.

## Example: Linking Against OpenSSL

Here is a standalone example that compiles a program using the OpenSSL library:

    project/
    ├── Makefile
    └── main.cpp

```cpp
// main.cpp

#include <iostream>
#include <cstring>
#include <openssl/sha.h>

int main()
{
    const char* text = "Hello, World!";
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(reinterpret_cast<const unsigned char*>(text), strlen(text), hash);

    std::cout << "SHA256 Hash: ";
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    std::cout << std::endl;

    return 0;
}
```

```makefile
CXX      = g++
CXXFLAGS = -Wall -std=c++11 $(shell pkg-config --cflags openssl)
LDLIBS   = $(shell pkg-config --libs openssl)

SRC    = main.cpp
TARGET = myprog

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)
```

    $ make
    g++ -Wall -std=c++11 -o myprog main.cpp -lssl -lcrypto

    $ ./myprog
    SHA256 Hash: dffd6021bb2bd5b0af676290809ec3a53191dd81c7f70a4b28688a362182986f

The key points:
- **`CXXFLAGS`** includes `$(shell pkg-config --cflags openssl)` for compiler flags (header paths).
- **`LDLIBS`** includes `$(shell pkg-config --libs openssl)` for linker flags (`-lssl -lcrypto`).
- **`$(LDLIBS)` appears at the end** of the link command — linker flags must come after the source/object files.

---

← Previous: [Order-Only Prerequisites](12_order_only_prerequisite.md) · Next: [Package Managers](14_package_managers.md) →
