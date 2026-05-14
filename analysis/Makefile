# Compiler and flags
CC = g++
CCFLAGS = -std=c++11 -I/usr/include/boost

# Definitions
S = shared
SRC_S = $(S)/src
INCL_S = $(S)/includes
OBJ_S = $(S)/obj

R = ranking
SRC_R = $(R)/src
INCL_R = $(R)/includes
OBJ_R = $(R)/obj
BIN_R = $(R)/bin

T = trigger
SRC_T = $(T)/src
INCL_T = $(T)/includes
OBJ_T = $(T)/obj
BIN_T = $(T)/bin

V = validation
SRC_V = $(V)/src
INCL_V = $(V)/includes
OBJ_V = $(V)/obj
BIN_V = $(V)/bin


# Default target
all: $(R) $(T) $(V)


# Link object files

$(R): $(R).o
	$(CC) $(CCFLAGS) $(OBJ_R)/*.o $(OBJ_S)/*.o -o $(BIN_R)/exec

$(T): $(T).o
	$(CC) $(CCFLAGS) $(OBJ_T)/*.o $(OBJ_S)/*.o -o $(BIN_T)/exec

$(V): $(V).o
	$(CC) $(CCFLAGS) $(OBJ_V)/*.o $(OBJ_S)/*.o -o $(BIN_V)/exec


# Rules to compile shared .cpp files

$(S)_utilities.o: $(SRC_S)/utilities.cc $(INCL_S)/utilities.hpp \
	$(INCL_S)/types.hpp

	$(CC) $(CCFLAGS) -c $(SRC_S)/utilities.cc -o $(OBJ_S)/utilities.o

$(S)_weights.o: $(SRC_S)/weights.cc $(INCL_S)/weights.hpp

	$(CC) $(CCFLAGS) -c $(SRC_S)/weights.cc -o $(OBJ_S)/weights.o

$(S)_node.o: $(SRC_S)/node.cc $(INCL_S)/node.hpp \
	$(INCL_S)/datapoint.hpp $(INCL_S)/types.hpp $(INCL_S)/utilities.hpp $(INCL_S)/weights.hpp \
	$(S)_utilities.o $(S)_weights.o

	$(CC) $(CCFLAGS) -c $(SRC_S)/node.cc -o $(OBJ_S)/node.o

$(S)_container.o: $(SRC_S)/container.cc $(INCL_S)/container.hpp \
	$(INCL_S)/types.hpp $(INCL_S)/utilities.hpp \
	$(S)_utilities.o

	$(CC) $(CCFLAGS) -c $(SRC_S)/container.cc -o $(OBJ_S)/container.o

$(S)_candidate.o: $(SRC_S)/candidate.cc $(INCL_S)/candidate.hpp \
	$(INCL_S)/node.hpp $(INCL_S)/container.hpp \
	$(S)_node.o $(S)_container.o

	$(CC) $(CCFLAGS) -c $(SRC_S)/candidate.cc -o $(OBJ_S)/candidate.o


# Rules to compile ranking .cpp files

$(R).o: $(SRC_R)/main.cc \
	$(INCL_S)/utilities.hpp $(INCL_S)/node.hpp $(INCL_S)/container.hpp $(INCL_S)/candidate.hpp \
	$(INCL_R)/checks.hpp \
	$(S)_utilities.o $(S)_node.o $(S)_container.o $(S)_candidate.o \
	$(R)_checks.o

	$(CC) $(CCFLAGS) -c $(SRC_R)/main.cc -o $(OBJ_R)/main.o
	
$(R)_checks.o: $(SRC_R)/checks.cc $(INCL_R)/checks.hpp \
	$(INCL_S)/datapoint.hpp $(INCL_S)/types.hpp $(INCL_S)/node.hpp $(INCL_S)/container.hpp \
	$(S)_node.o $(S)_container.o

	$(CC) $(CCFLAGS) -c $(SRC_R)/checks.cc -o $(OBJ_R)/checks.o


# Rules to compile trigger .cpp files

$(T).o: $(SRC_T)/main.cc \
	$(INCL_S)/utilities.hpp $(INCL_S)/node.hpp $(INCL_S)/container.hpp \
	$(INCL_T)/checks.hpp $(INCL_T)/selection.hpp \
	$(S)_utilities.o $(S)_node.o $(S)_container.o \
	$(T)_checks.o $(T)_selection.o

	$(CC) $(CCFLAGS) -c $(SRC_T)/main.cc -o $(OBJ_T)/main.o

$(T)_checks.o: $(SRC_T)/checks.cc $(INCL_T)/checks.hpp \
	$(INCL_S)/datapoint.hpp $(INCL_S)/types.hpp $(INCL_S)/node.hpp $(INCL_S)/container.hpp \
	$(S)_node.o $(S)_container.o

	$(CC) $(CCFLAGS) -c $(SRC_T)/checks.cc -o $(OBJ_T)/checks.o

$(T)_selection.o: $(SRC_T)/selection.cc $(INCL_T)/selection.hpp \
	$(INCL_S)/datapoint.hpp $(INCL_S)/container.hpp \
	$(S)_container.o

	$(CC) $(CCFLAGS) -c $(SRC_T)/selection.cc -o $(OBJ_T)/selection.o


# Rules to compile validation .cpp files

$(V).o: $(SRC_V)/main.cc \
	$(INCL_S)/utilities.hpp $(INCL_S)/node.hpp $(INCL_S)/container.hpp $(INCL_S)/candidate.hpp \
	$(INCL_V)/checks.hpp \
	$(S)_utilities.o $(S)_node.o $(S)_container.o $(S)_candidate.o \
	$(V)_checks.o

	$(CC) $(CCFLAGS) -c $(SRC_V)/main.cc -o $(OBJ_V)/main.o
	
$(V)_checks.o: $(SRC_V)/checks.cc $(INCL_V)/checks.hpp \
	$(INCL_S)/datapoint.hpp $(INCL_S)/types.hpp $(INCL_S)/node.hpp $(INCL_S)/container.hpp \
	$(S)_node.o $(S)_container.o

	$(CC) $(CCFLAGS) -c $(SRC_V)/checks.cc -o $(OBJ_V)/checks.o


clean:
	$(foreach dir, $(BIN_R) $(BIN_T) $(BIN_V), rm -f $(dir)/exec)
	$(foreach dir, $(OBJ_S) $(OBJ_R) $(OBJ_T) $(OBJ_V), rm -f $(dir)/*.o)
