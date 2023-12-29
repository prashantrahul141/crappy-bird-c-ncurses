CC = gcc
SOURCE_FILES = main.c
OUTPUT_BINARY = main.out
LIBS = ncurses
FLAGS = -Wall -Wextra

all:
	$(CC) $(SOURCE_FILES) -o $(OUTPUT_BINARY) -l$(LIBS) $(FLAGS) -O3

test:
	$(CC) $(SOURCE_FILES) -o $(OUTPUT_BINARY) -l$(LIBS) $(FLAGS) ; ./$(OUTPUT_BINARY)
