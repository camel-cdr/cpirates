# Variables

## Task 1
The pirates shall draw a square given an edge length on an empty field.
1. For the first version of the program you can simply specify a desired edge length.
```
>#########
##########
##########
##########
##########
##########
##########
##########
##########
##########
```
2. The second version should count and collect all of treasures in the first line and draw a square with the count as the edge length.
```
>#TT##TTT#
##########
##########
##########
##########
##########
##########
##########
##########
##########
```


## Task 2
Let the pirates compare the number of treasures in the first and second row.
If they have an equal amount of treasure move them to the first column of the first row
otherwise move them to the first column of the second row.

```
>#TT##TT#T##T##T
#TT##TT#T##T##T#
```
```
>#T###T##TTTT##T
##T#####TT##T#T#
```

## Task 3
Somehow the entire map is cluttered with treasure, help the pirates clean it up.
Write a program that collects and counts all of the treasure on the map and
put them down nicely packed from the right to left and bottom to top.


```
>#TT##TTT#
#TT#####T#
##T##T###T
T#T#####T#
T#TTTTTT##
########T#
##TT#T##T#
#TT####T##
####TT####
###T######
```
should become:
```
>#########
##########
##########
##########
##########
##########
########TT
TTTTTTTTTT
TTTTTTTTTT
TTTTTTTTTT
```

## Task 4
The pirates want to find the row with the most treasure, write a program that
counts all of the treasure in each row and moves the pirate ship to the row with the most treasure.

## Task 5
Recreate task 1.1 but draw the square rotated by 45°.

## Task 5
Write a program that reads the first row interpreting every empty field as a '0' and every treasure as a '1'.
Move the ship to the resulting binary number (10110 -> 22 -> move the ship to the 22't field).
