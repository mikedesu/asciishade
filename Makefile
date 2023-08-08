CC=gcc
FLAGS=-Wall -Werror
LINKS=-lncursesw 
BINARY=asciishade
UNITTEST_READ_ASCII=unittest_read_ascii
UNITTEST_COLORS=unittest_colors
UNITTEST_WIDE_CHARS=unittest_wide_chars
UNITTEST_CHARS=unittest_chars
UNITTEST_TIME=unittest_time
UNITTEST_CANVAS_PIXEL=unittest_canvas_pixel
OBJECTS=tools.o canvas.o colors.o hud.o
JUNK=a.out

all: $(BINARY) $(UNITTEST_READ_ASCII) $(UNITTEST_COLORS) $(UNITTEST_WIDE_CHARS) $(UNITTEST_TIME) $(UNITTEST_CHARS) $(UNITTEST_CANVAS_PIXEL)

# main binary
$(BINARY): main.c $(OBJECTS)
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

# unit tests depending on OBJECTS
unittest_read_ascii: unittest_read_ascii.c $(OBJECTS)
	$(CC) $(FLAGS) $^ $(LINKS) -o $@
unittest_canvas_pixel: unittest_canvas_pixel.c $(OBJECTS)
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

# unit tests not depending on OBJECTS
unittest_colors: unittest_colors.c 
	$(CC) $(FLAGS) $^ $(LINKS) -o $@
unittest_wide_chars: unittest_wide_chars.c
	$(CC) $(FLAGS) $^ $(LINKS) -o $@
unittest_time: unittest_time.c
	$(CC) $(FLAGS) $^ $(LINKS) -o $@
unittest_chars: unittest_chars.c
	$(CC) $(FLAGS) $^ $(LINKS) -o $@

# object files
canvas.o: canvas.c
	$(CC) $(FLAGS) -c $^ -o $@
tools.o: tools.c
	$(CC) $(FLAGS) -c $^ $(LINKS) -o $@
colors.o: colors.c
	$(CC) $(FLAGS) -c $^ $(LINKS) -o $@
hud.o: hud.c
	$(CC) $(FLAGS) -c $^ $(LINKS) -o $@

# clean up
clean:
	rm -rfv $(BINARY) $(OBJECTS) $(UNITTEST_READ_ASCII) $(JUNK) $(UNITTEST_COLORS) *.o *.dSYM $(UNITTEST_WIDE_CHARS) $(UNITTEST_TIME) $(UNITTEST_CHARS) $(UNITTEST_CANVAS_PIXEL)

