CC=g++
CCFLAGS=-std=c++11 -Iincludes/

.PHONY: exec

exec: main.o
	$(CC) $(CCFLAGS) obj/*.o -o bin/exec

main.o: src/main.cc \
	command.o stats.o utilities.o \
	includes/command.hpp includes/stats.hpp includes/utilities.hpp

	$(CC) $(CCFLAGS) -c src/main.cc -o obj/main.o

command.o: src/command.cc \
	includes/command.hpp

	$(CC) $(CCFLAGS) -c src/command.cc -o obj/command.o

stats.o: src/stats.cc \
	command.o \
	includes/stats.hpp includes/command.hpp

	$(CC) $(CCFLAGS) -c src/stats.cc -o obj/stats.o

utilities.o: src/utilities.cc \
	includes/utilities.hpp

	$(CC) $(CCFLAGS) -c src/utilities.cc -o obj/utilities.o

clean:
	rm -f bin/exec obj/*.o
