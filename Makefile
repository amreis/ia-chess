#Hello guys

CC=g++
LIB_PATH=.
INC_PATH=.
LIBS=
FLAGS=-Wall -std=c++11 -g
OBJECTS=main.o state.o transp_table.o
ADDITIONAL_HEADERS=bot.h

all: main

main: $(OBJECTS)
	$(CC) $(FLAGS) -I$(INC_PATH) -L$(LIB_PATH) $(LIBS) -o main $(OBJECTS) \
	$(ADDITIONAL_HEADERS)
.cpp.o:
	$(CC) $(FLAGS) -I$(INC_PATH) -c $<

clean:
	rm -rf *.o main $(OBJECTS)
