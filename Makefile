

all:
	gcc -Wall main.c -lncursesw -lm -o asciishade

clean:
	rm -rfv asciishade

