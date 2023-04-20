all: RUFS.o
	g++ -std=gnu++11 -o RUFS RUFS.o
RUFS.o: RUFS.cpp
	g++ -std=gnu++11 -c RUFS.cpp
clean:
	rm *.o
