#ifndef CPIRATES_H_INCLUDED

typedef enum { HERE, FRONT, RIGHT, BACK, LEFT } Dir;
typedef enum { EMPTY, TREASURE, BUOY, OBSTACLE, BORDER } Obj;

void move(void);
void turn(Dir dir);
Obj look(Dir dir);
Obj take(void);
void place(Obj obj);

#undef CPIRATES_H_INCLUDED
#endif
