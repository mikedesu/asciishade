# NOTES

As I work on this project I want to write down some notes about things I discovered during development:

- ChatGPT kept recommending to me the use of a `cchar_t` datatype as well as wide-character ncurses functions, but gcc was acting like they did not exist...
    - Digging into `ncurses.h`, you can plainly see the datatype declared and defined, but it is not clear how you link it to your project.
    - In this link below, it is explained to add a define macro at the top of your source file before including ncurses.h:

```
#define _XOPEN_SOURCE_EXTENDED 1
```

    - [https://lists.gnu.org/archive/html/bug-ncurses/2011-07/msg00006.html](https://lists.gnu.org/archive/html/bug-ncurses/2011-07/msg00006.html)
