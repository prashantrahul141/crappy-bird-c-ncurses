CC = gcc
SOURCE_FILES = main.c
OUTPUT_BINARY = main.out
LIBS = ncurses

all:
	$(CC) $(SOURCE_FILES) -o $(OUTPUT_BINARY) -l$(LIBS)

test:
	$(CC) $(SOURCE_FILES) -o $(OUTPUT_BINARY) -l$(LIBS) ; ./$(OUTPUT_BINARY)
