CC=g++
CCFLAGS=-std=c++11 -Iincludes/ -I/usr/include/boost/ # -fsanitize=address -g

.PHONY: exec

exec: main.o
	$(CC) $(CCFLAGS) obj/*.o -o bin/exec

main.o: src/main.cc \
	utilities.o model_selection.o interval.o \
	includes/types.hpp includes/utilities.hpp includes/model_selection.hpp includes/interval.hpp
	
	$(CC) $(CCFLAGS) -c src/main.cc -o obj/main.o

utilities.o: src/utilities.cc \
	includes/utilities.hpp includes/types.hpp
	$(CC) $(CCFLAGS) -c src/utilities.cc -o obj/utilities.o

model_selection.o: src/model_selection.cc \
	model.o \
	includes/model_selection.hpp includes/types.hpp includes/model.hpp includes/utilities.hpp
	$(CC) $(CCFLAGS) -c src/model_selection.cc -o obj/model_selection.o
	
model.o: src/model.cc \
	matrix.o \
	includes/model.hpp includes/types.hpp includes/matrix.hpp
	$(CC) $(CCFLAGS) -c src/model.cc -o obj/model.o

interval.o: src/interval.cc \
	includes/interval.hpp includes/types.hpp includes/model.hpp
	$(CC) $(CCFLAGS) -c src/interval.cc -o obj/interval.o

matrix.o: src/matrix.cc \
	includes/matrix.hpp includes/types.hpp
	$(CC) $(CCFLAGS) -c src/matrix.cc -o obj/matrix.o

clean:
	rm -f bin/exec obj/*.o
