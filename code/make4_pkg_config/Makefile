CC = g++
CFLAGS = -Wall -std=c++11 $(shell pkg-config --cflags openssl)
LDFLAGS = $(shell pkg-config --libs openssl)

SRC = main.cpp
TARGET = myprog

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)