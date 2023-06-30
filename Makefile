
CC=gcc
FLAGS=-Wall -Werror
LINKS=-lncursesw 
BINARY=asciishade
UNITTEST_BINARY=unittest_driver
UNITTEST_COLORS=unittest_colors
OBJECTS=tools.o canvas.o colors.o
JUNK=a.out

all: $(BINARY) $(UNITTEST_BINARY) $(UNITTEST_COLORS)




asciishade: main.c $(OBJECTS)
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

unittest_driver: unittest_driver.c $(OBJECTS)
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

unittest_colors: unittest_colors.c 
	$(CC) $(FLAGS) $^ $(LINKS) -o $@


canvas.o: canvas.c
	$(CC) $(FLAGS) -c $^ -o $@

tools.o: tools.c
	$(CC) $(FLAGS) -c $^ $(LINKS) -o $@

colors.o: colors.c
	$(CC) $(FLAGS) -c $^ $(LINKS) -o $@

clean:
	rm -rfv $(BINARY) \
		$(OBJECTS) \
		$(UNITTEST_BINARY) \
		$(JUNK) \
		$(UNITTEST_COLORS) \
		*.o \
		*.dSYM

