
CC=gcc
FLAGS=-Wall -Werror
LINKS=-lncursesw 
BINARY=asciishade
UNITTEST_BINARY=unittest_driver
OBJECTS=tools.o

all: $(BINARY) $(UNITTEST_BINARY)




asciishade: main.c $(OBJECTS)
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

unittest_driver: unittest_driver.c $(OBJECTS)
	$(CC) $(FLAGS) $^ $(LINKS) -o $@




tools.o: tools.c
	$(CC) $(FLAGS) -c $^ -o $@

clean:
	rm -rfv $(BINARY) $(OBJECTS) $(UNITTEST_BINARY)

