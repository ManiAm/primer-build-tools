# Ch10: Adding Debug Support
DEBUG ?= 0

ifeq ($(DEBUG), 1)
  CXXFLAGS = -Wall -g -MMD -MP
else
  CXXFLAGS = -Wall -O2 -MMD -MP
endif

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
