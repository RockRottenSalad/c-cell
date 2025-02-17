
CC := gcc
CFLAGS := -Wall -Wextra -g -std=c23 -DLOGGING
CFLAGS_TEST := $(CFLAGS) -DTEST
LINK_FLAGS := -lm
OBJ := obj
SRC := src
BIN := bin
OUTPUT := program

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

run: all
	./$(BIN)/$(OUTPUT)

test: clean test_build run

test_build:
	$(CC) $(CFLAGS_TEST) -I./$(SRC) -c $(SRC)/main.c -o $(OBJ)/main.o

all: $(OBJECTS)
	$(CC) $(LINK_FLAGS) $^ -o $(BIN)/$(OUTPUT)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -I./$(SRC) -c $< -o $@

clean:
	rm -f $(OBJ)/*.o

