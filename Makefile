
CC=gcc
FLAGS=-Wall -Werror
LINKS=-lncursesw 
BINARY=asciishade

all: $(BINARY)

asciishade: main.c
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

clean:
	rm -rfv asciishade

