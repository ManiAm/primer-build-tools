# Ch03: Separating Compile and Link
myprog: main.o lib.o
	g++ main.o lib.o -o myprog

main.o: main.cpp
	g++ -c main.cpp -o main.o

lib.o: lib.cpp
	g++ -c lib.cpp -o lib.o
