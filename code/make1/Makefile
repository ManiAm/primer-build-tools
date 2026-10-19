CXXFLAG = -Wall -O2
SRC_FILES = main.cpp lib.cpp
O_FILES = $(SRC_FILES:%.cpp=%.o)

# Link
myprog: $(O_FILES)
	$(CXX) -o $@ $(O_FILES)

# Compile
%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	-@rm -f myprog
	-@rm -f *.o *.d
