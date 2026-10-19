# Ch09: Adding Header Dependencies
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
