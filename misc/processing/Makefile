CC=g++
CCFLAGS=-std=c++11 -Iincludes/

.PHONY: exec

exec: main.o
	$(CC) $(CCFLAGS) obj/*.o -o bin/exec

main.o: src/main.cc utilities.o time.o sample.o includes/*.hpp
	$(CC) $(CCFLAGS) -c src/main.cc -o obj/main.o

utilities.o: src/utilities.cc includes/utilities.hpp includes/sample.hpp
	$(CC) $(CCFLAGS) -c src/utilities.cc -o obj/utilities.o

time.o: src/time.cc includes/time.hpp
	$(CC) $(CCFLAGS) -c src/time.cc -o obj/time.o

sample.o: src/sample.cc includes/sample.hpp
	$(CC) $(CCFLAGS) -c src/sample.cc -o obj/sample.o

clean:
		rm -f bin/exec obj/*.o
