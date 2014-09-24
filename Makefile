#Hello guys

CC=g++
LIB_PATH=.
INC_PATH=.
LIBS=
FLAGS=-Wall -std=c++11 -g
OBJECTS=main.o state.o transp_table.o
ADDITIONAL_HEADERS=bot.h

all: main main_32

main: $(OBJECTS)
	$(CC) $(FLAGS) -I$(INC_PATH) -L$(LIB_PATH) $(LIBS) -o main $(OBJECTS) \
	$(ADDITIONAL_HEADERS)
.cpp.o:
	$(CC) $(FLAGS) -I$(INC_PATH) -c $<
main_32:
	g++ -m32 main.cpp transp_table.cpp state.cpp bot.h -o main_32 $(FLAGS)
clean:
	rm -rf *.o main main_32 $(OBJECTS)
