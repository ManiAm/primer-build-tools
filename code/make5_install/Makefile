CC = gcc                 # Compiler
CFLAGS = -Wall -Werror   # Compiler flags
TARGET = hello           # Target executable name
PREFIX = /usr/local

all: $(TARGET)

$(TARGET): hello.c
	$(CC) $(CFLAGS) -o $(TARGET) hello.c

install: $(TARGET)
	@install -d $(PREFIX)/bin                # Create target directory
	@install -m 755 $(TARGET) $(PREFIX)/bin/ # Copy binary with executable permissions

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)           # Remove the binary

clean:
	rm -f $(TARGET)