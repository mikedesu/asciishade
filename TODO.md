# TODO

**noted on 7/21/23**

- documentation
- 'smart' paintbucket that can detect blocks and compensate
- draw rect
- fill rect
- draw circle
- fill circle

**noted on 7/15/23**

- realized we need both an "invert colors" tool, as well as a "vertical flip" and "horizontal flip" tool

**noted on 7/10/23**

- while implementing 99 colors, realized that ncurses can only have 256 active color pairs at any given moment (verified by enumerating the color pairs and watching the palette reset after 0xff)
    - one possible solution is to section-off every 256 color pairs or something in order to manage having all 99x99 color pairs in palette but this will likely cause the rendered screen colors to shift when rotating into a new section
    - another realistic solution is to allow the user to define their own color palette via a config file
- resizing of canvas still needed
- navigating outside of terminal bounds for larger canvases still needed
- we need a tool that can calculate how many different color pairs are present in a given ascii
- another idea is that, when loading asciis from file, we can calculate what colors are used and generate a set of color pairs based off of that input
- in other words: our palette might be limited, even though we can still support 99x99 colors on output
- we could have a flag that sets how many colors we are supporting in this run and have it be dynamic like that

**noted on 7/07/23**

- want to track how long individual actions/commands take to execute
- want to display how long each action takes on the HUD
- want to start logging each run in verbose mode or debug mode

**noted on 7/02/23**

- need clear canvas ('c' key perhaps?)
- need paintbucket fill-tool
- need other block support (half, quarter)
- need support for other unicode characters besides the block
- another program that is simply an ascii viewer and not an editor
- updates to the HUD
- layers
- frames (inspired by aseprite, for possible animated gif and/or spritesheet export?)
- export to different file formats
- file name on untitled save
- scripting
- it would be interesting to read from stdin

**noted on 6/28/23**

- confirm on quit (to prevent accidental quits!)

**noted on 6/25/23**

- resize canvas 
- navigate a larger canvas than fits inside the console
- be able to cat out asciis onto the commandline and render properly
    - terminal color escape sequences are hard

**noted on 6/18/23**

- undo / move history

-----

**completed on 6/28/23**

- load from file
    - have to count rows and columns in order to generate canvas

**completed on 6/25/23**

- block delete

