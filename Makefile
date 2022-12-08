INPUT = main.cpp

CC = g++

COMPILER_FLAGS = -w

INCLUDE = -I/usr/include/SDL2
LIB = -L/usr/lib

LINKER_FLAGS = -lSDL2

OUTPUT = sdl2_faraound

all : $(INPUT)
	$(CC) $(INPUT) $(INCLUDE) $(LIB) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OUTPUT)
clean:
	rm $(OUTPUT)
