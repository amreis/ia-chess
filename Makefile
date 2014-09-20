#Hello guys

CC=g++
LIB_PATH=.
INC_PATH=.
LIBS=
FLAGS=-Wall -std=c++11 -g

all: main

main: main.o state.o
	$(CC) $(FLAGS) -I$(INC_PATH) -L$(LIB_PATH) $(LIBS) -o main main.o state.o bot.h
main.o: main.cpp
	$(CC) $(FLAGS) -I$(INC_PATH) -c main.cpp
state.o: state.cpp
	$(CC) $(FLAGS) -I$(INC_PATH) -c state.cpp
clean:
	rm -rf *.o main
