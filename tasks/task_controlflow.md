# Controlflow

## Task 1
Write two programs that replaces every treasure on any given line with a buoy.
One program using recursion and another one using loops.
The lines are expected to only consist of treasures or empty fields.

Example maps:
```
>##T#TT##T
```
```
>TT###T#######TT#
```


## Task 2
Write a program using recursion that collects a trail of treasures.
Every treasure shall have at most two neighboring treasures.
Watch out for the obstacle and don't sink the ship.

Example map:
```
>TTTTo#TTT
###oTo#T#T
#TTTTooToT
oT####oToT
oTTTTTTT#T
oo##ooo##T
#TTTTTo#oT
#ToooT##oT
oT#ToTTTTT
oTTT#o####
```


## Task 3
Expand task 2's program so that after collecting all the treasure the
pirates backtrack and place buoys on every place they've visited.


## Task 4
Let the pirates not only follow one trail but allow them to follow unexplored
branches on there way back.

Example map:
```
>TTTT####o
ToooToo#oo
TTTTToo###
T#ooTTTTTT
To##T##T#o
TTTTTo#T##
TTTTo#T#oo
To####Too#
To##ooT###
TTTTTTT###
```

