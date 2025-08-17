CFLAGS := -Wall -Wextra -Wno-unused -O3
MAIN := main_pipe.c

all: cpirates_stdc cpirates_sdl2

cpirates_stdc: cpirates_stdc.c ${MAIN} cpirates.c cpirates.h
	cc cpirates_stdc.c ${MAIN} -o $@ ${CFLAGS}


cpirates_sdl2: cpirates_sdl2.c ${MAIN} cpirates.c cpirates.h
	cc cpirates_sdl2.c ${MAIN} -o $@ $(shell pkg-config sdl2 --cflags --libs) ${CFLAGS}


clean:
	rm -f cpirates_sdl2 cpirates_stdc

.PHONY: clean
