CXX = g++
CXXFLAGS = -Wall -lpthread -std=c++11 -Iincludes/ -pthread

INCLUDES:= includes

.PHONY: clean

all: PIController create_task_list

create_task_list: src/create_task_list.cpp sched_attr.o
	$(CXX) $(CXXFLAGS) -o bin/create_task_list src/create_task_list.cpp obj/sched_attr.o

PIController: src/PIController.cc utilities.o
	$(CXX) $(CXXFLAGS) -o bin/PIController src/PIController.cc obj/utilities.o obj/exec_output.o obj/splitString.o obj/sched_attr.o

utilities.o: src/utilities.cc $(INCLUDES)/*.hpp exec_output.o splitString.o sched_attr.o
	$(CXX) $(CXXFLAGS) -o obj/utilities.o -c src/utilities.cc

exec_output.o: src/exec_output.cc $(INCLUDES)/exec_output.hpp
	$(CXX) $(CXXFLAGS) -o obj/exec_output.o -c src/exec_output.cc

splitString.o: src/splitString.cc $(INCLUDES)/splitString.hpp
	$(CXX) $(CXXFLAGS) -o obj/splitString.o -c src/splitString.cc

sched_attr.o: src/sched_attr.cc $(INCLUDES)/sched_attr.hpp
	$(CXX) $(CXXFLAGS) -o obj/sched_attr.o -c src/sched_attr.cc
    
clean:
	rm -f bin/PIController obj/utilities.o obj/exec_output.o obj/splitString.o obj/sched_attr.o
