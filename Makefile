#Hello guys

CC=g++
LIB_PATH=.
INC_PATH=.
LIBS=
FLAGS=-Wall -std=c++11 -g

all: main

main: main.o state.o
	$(CC) $(FLAGS) -I$(INC_PATH) -L$(LIB_PATH) $(LIBS) -o main main.o state.o bot.h
.cpp.o:
	$(CC) $(FLAGS) -I$(INC_PATH) -c $<

clean:
	rm -rf *.o main
