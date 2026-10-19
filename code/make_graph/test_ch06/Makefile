# Ch06: Adding Variables
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
