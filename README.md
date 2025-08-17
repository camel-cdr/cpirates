# cpirates


## With C
### Build
in Makefile:
```
MAIN := main.c
```

### Syntax
Edit main.c.
* enum Dir { HERE, FRONT, RIGHT, BACK, LEFT }
* enum Obj { EMPTY, TREASURE, BUOY, OBSTACLE, BORDER }
* move() -> move ship in its currently facing direction
* turn(Dir) -> turn the ship into the specified direction
* Obj look(Dir) -> returns the Object in the specified direction
* Obj take() -> returns the object on the ships position and removes it from the board
* place(Obj) -> place an buoy or treasure on the ships position

### run
Execute cpirats\_\[target\].


## With any other language
### Build
in Makefile:
```
MAIN := main_posix.c
```

### Syntax
Write your programm in any language and read and write to stdio.
Make sure to flush the output.
* "m" -> move ship in its currently facing direction
* "t[frbl]" -> turn the ship into the specified direction
* "l[hfrbl]" -> returns the object in the specified direction; returns: "[etboB]
* "T" -> returns the object on the ships position and removes it from the board; returns: "[tb]"
* "p[tb]" -> place an buoy or treasure on the ships position

### Run
Execute cpirates\_\[target\]. with your program as an argument.
E.g.
```
./cpirates_sdl2 map.txt main.py
```


## Inspired by
https://github.com/jascosius/web-piraten
