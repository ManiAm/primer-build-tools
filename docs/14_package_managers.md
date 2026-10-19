# Package Managers

In languages like Python (`pip`), Rust (`cargo`), or JavaScript (`npm`), installing a library is a single command. C and C++ have no built-in package manager. Instead, libraries are typically installed through **system package managers**, their compiler and linker flags are discovered with **`pkg-config`**, and larger projects use **C/C++ package managers** like Conan or vcpkg.

## System Package Managers

The most common way to install C/C++ libraries is through your operating system's package manager:

| OS            | Command                            | Example                          |
| ------------- | ---------------------------------- | -------------------------------- |
| Debian/Ubuntu | `sudo apt install <package>-dev`   | `sudo apt install libssl-dev`    |
| Fedora/RHEL   | `sudo dnf install <package>-devel` | `sudo dnf install openssl-devel` |
| macOS         | `brew install <package>`           | `brew install openssl`           |

The `-dev` (or `-devel`) suffix includes header files and `pkg-config` metadata needed for compilation. The base package without the suffix only contains the shared library for running programs.

## `pkg-config` in Detail

[Chapter 13](13_lib_dependency.md#using-pkg-config) introduced `pkg-config` and using it in a Makefile. Here we look at how it works under the hood.

### `.pc` Files

When a library is installed (e.g., `libssl-dev`), it places a `.pc` file in a standard location like `/usr/lib/pkgconfig/`:

```
# /usr/lib/pkgconfig/openssl.pc
prefix=/usr
libdir=${prefix}/lib
includedir=${prefix}/include

Name: OpenSSL
Description: Secure Sockets Layer and cryptography libraries
Version: 3.0.2
Libs: -L${libdir} -lssl -lcrypto
Cflags: -I${includedir}
```

When you run `pkg-config --cflags openssl`, it reads this file and outputs `-I/usr/include`. When you run `pkg-config --libs openssl`, it outputs `-L/usr/lib -lssl -lcrypto`. This is how your Makefile gets the correct flags without hardcoding paths.

### Useful Commands

```
$ pkg-config --list-all               # list all available packages
$ pkg-config --modversion openssl     # show version of a package
$ pkg-config --exists openssl && echo "found"   # check if a package exists
```

## C/C++ Package Managers

System package managers work well for common libraries, but they have limitations:

- Only one version of a library can be installed system-wide at a time.
- Available versions depend on your OS release.
- Cross-platform builds require different commands per OS.

C/C++ package managers solve these problems by managing library versions, build configurations, and dependencies independently of the OS:

| Tool          | Year | OS Support                   | Description                                                        |
| ------------- | ---- | ---------------------------- | ------------------------------------------------------------------ |
| **Spack**     | 2013 | Linux, macOS, HPC systems    | Focused on scientific computing and HPC environments.              |
| **Hunter**    | 2013 | Windows, Linux, macOS        | CMake-based dependency management. Downloads and builds at configure time. |
| **Conan**     | 2015 | Windows, Linux, macOS        | General-purpose, cross-platform. Supports Make, CMake, Meson, etc. |
| **vcpkg**     | 2016 | Windows, Linux, macOS        | Microsoft-backed, easy setup. Best with CMake and Visual Studio.   |
| **CPM.cmake** | 2019 | Windows, Linux, macOS        | Lightweight, single-file CMake module. No external tools needed.   |

**Conan** is the most widely used for cross-platform C++ projects and integrates well with Makefiles, so the examples below use Conan.

## Using Conan with Make

### Prerequisites

Conan does not install compilers or build tools — those must already be present on your system (e.g., `g++`, `make`). Install Conan with `pip`:

```
$ pip install conan
$ conan --version
$ conan profile detect
```

`conan profile detect` creates a default profile based on your compiler and OS. You also need CMake installed (`sudo apt install cmake`), as Conan uses it to build some dependencies from source.

### Project Files

```
project/
├── main.cpp
├── conanfile.txt
└── Makefile
```

The following program sends an HTTP request using the [Poco](https://pocoproject.org/) C++ library:

```cpp
// main.cpp
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
#include <iostream>
#include <string>

int main()
{
    try {
        Poco::URI uri("http://www.google.com");
        std::string path = uri.getPathAndQuery();
        if (path.empty()) path = "/";

        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(
            Poco::Net::HTTPRequest::HTTP_GET, path,
            Poco::Net::HTTPMessage::HTTP_1_1);
        session.sendRequest(request);

        Poco::Net::HTTPResponse response;
        std::istream& body = session.receiveResponse(response);

        std::cout << "Status: " << response.getStatus()
                  << " " << response.getReason() << std::endl;
        Poco::StreamCopier::copyStream(body, std::cout);
    } catch (Poco::Exception& ex) {
        std::cerr << "Error: " << ex.displayText() << std::endl;
        return 1;
    }
    return 0;
}
```

#### Conan Recipe

Create `conanfile.txt` to declare the dependency and choose generators:

```
[requires]
poco/1.12.4

[generators]
AutotoolsToolchain
AutotoolsDeps
```

The **generators** tell Conan how to expose library flags to your build system. `AutotoolsToolchain` and `AutotoolsDeps` generate a shell script (`conanbuild.sh`) that sets environment variables (`CXXFLAGS`, `CPPFLAGS`, `LDFLAGS`, `LIBS`). Make picks up these variables automatically from the environment.

> Conan provides generators for many build systems (CMake, Meson, Bazel, etc.). For Make-based projects, the Autotools generators are the simplest approach because they use standard environment variables.

#### Makefile

```makefile
SRC    = main.cpp
OBJ    = $(SRC:.cpp=.o)
TARGET = myprog

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)
```

The Makefile does not hardcode any Poco-specific flags. It relies on standard variables (`CXXFLAGS`, `CPPFLAGS`, `LDFLAGS`, `LIBS`) that will be set by `conanbuild.sh` before building.

### Install Dependencies and Build

With all three files in place, install the dependencies:

```
$ conan install . --build=missing -s compiler.cppstd=17
```

This downloads Poco and its transitive dependencies, builds any that lack prebuilt binaries, and generates `conanbuild.sh`. Source the generated environment:

```
$ source conanbuild.sh
```

You can verify the flags are set:

```
$ echo $CXXFLAGS
$ echo $LDFLAGS
```

Build and run:

```
$ make
$ ./myprog
Status: 200 OK
...
```

## Conan Remotes

Conan packages are hosted on **remotes** — servers that store and distribute packages:

```
$ conan remote list
conancenter: https://center2.conan.io [Verify SSL: True, Enabled: True]
```

- **ConanCenter** — the default public remote, maintained by JFrog. Browse available packages at [conan.io/center](https://conan.io/center).
- **Private remotes** — self-hosted servers (e.g., Artifactory) for internal packages.

To add a private remote:

```
$ conan remote add myremote https://my-server.com/conan
```

## Conan Local Cache

Conan stores downloaded packages in a local cache (`~/.conan2/` on Linux/macOS). Each project resolves dependencies from this cache, so different projects can use different library versions without conflict.

```
$ conan list                            # list cached packages
$ export CONAN_HOME=/custom/path        # override cache location
```

## Exporting and Uploading Packages

Once you create your own Conan package, you can share it:

```
$ conan export . myuser/channel                              # register recipe in local cache
$ conan upload hello/1.0@myuser/channel -r=myremote --all    # upload to a remote
```

Exporting adds the recipe to your local cache. Uploading publishes it to a remote so others can use it.

---

← Previous: [Library Dependency](13_lib_dependency.md) · Next: [Make Install](15_make_install.md) →
