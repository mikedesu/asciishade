CC=gcc
FLAGS=-Wall -Werror
LINKS=-lncursesw 
BINARY=asciishade
UNITTEST_BINARY=unittest_driver
UNITTEST_COLORS=unittest_colors
UNITTEST_WIDE_CHARS=unittest_wide_chars
UNITTEST_CHARS=unittest_chars
UNITTEST_TIME=unittest_time
OBJECTS=tools.o canvas.o colors.o hud.o
JUNK=a.out

all: $(BINARY) $(UNITTEST_BINARY) $(UNITTEST_COLORS) $(UNITTEST_WIDE_CHARS) $(UNITTEST_TIME) $(UNITTEST_CHARS)

asciishade: main.c $(OBJECTS)
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

unittest_driver: unittest_driver.c $(OBJECTS)
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

unittest_colors: unittest_colors.c 
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

unittest_wide_chars: unittest_wide_chars.c
	$(CC) $(FLAGS) $^ $(LINKS) -o $@
unittest_time: unittest_time.c
	$(CC) $(FLAGS) $^ $(LINKS) -o $@
unittest_chars: unittest_chars.c
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

canvas.o: canvas.c
	$(CC) $(FLAGS) -c $^ -o $@

tools.o: tools.c
	$(CC) $(FLAGS) -c $^ $(LINKS) -o $@

colors.o: colors.c
	$(CC) $(FLAGS) -c $^ $(LINKS) -o $@

hud.o: hud.c
	$(CC) $(FLAGS) -c $^ $(LINKS) -o $@

clean:
	rm -rfv $(BINARY) $(OBJECTS) $(UNITTEST_BINARY) $(JUNK) $(UNITTEST_COLORS) *.o *.dSYM $(UNITTEST_WIDE_CHARS) $(UNITTEST_TIME) $(UNITTEST_CHARS)

