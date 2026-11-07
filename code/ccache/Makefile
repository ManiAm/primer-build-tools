CXX = ccache g++
CXXFLAGS = -Wall -O2

myprog: main.o lib.o
	$(CXX) main.o lib.o -o myprog

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f myprog *.o
