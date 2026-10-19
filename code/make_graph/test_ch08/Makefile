# Ch08: Adding a Pattern Rule
CXXFLAGS = -Wall -O2

SRC_FILES = main.cpp lib.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

myprog: $(OBJ_FILES)
	g++ $^ -o $@

%.o: %.cpp
	@echo "[CXX] $<"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f myprog *.o

.PHONY: rebuild
rebuild: clean myprog
