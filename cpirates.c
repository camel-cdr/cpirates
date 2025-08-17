#include "cpirates.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "stretchy-buffer.h"

extern void run(char *const argv[]);

typedef enum {
	NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3, NONE = 4
} CpFacing;


typedef struct {
	Sb(char) objs;
	int width, height;
	int shipx, shipy;
	CpFacing shipFacing;
} CpBoard;

#define CP_MIN(a, b) ((a) < (b) ? (a) : (b))
#define CP_MAX(a, b) ((a) > (b) ? (a) : (b))
#define CP_CLAMP(x,lower,upper) CP_MIN(CP_MAX((x), (lower)), (upper))


static CpBoard cpBoard;

static void cp_update_board(int x, int y);

static void
cp_board_load(char *str)
{
	/* calculate width */
	char *it = str;
	for (cpBoard.width = 0; *it && *it != '\r' && *it != '\n'; ++cpBoard.width, ++it);
	it = str;

	cpBoard.shipFacing = NONE;
	sb_initcap(&cpBoard.objs, cpBoard.width);

	#define SHIP_CASE(c, d) \
		case c: \
			if (cpBoard.shipFacing != NONE) { \
				fprintf(stderr, \
				        "second ships read on %dx%d\n", \
				        x, cpBoard.height); \
				exit(EXIT_FAILURE); \
			} \
			cpBoard.shipFacing = d; \
			cpBoard.shipx = x; \
			cpBoard.shipy = cpBoard.height; \
			sb_push(&cpBoard.objs, EMPTY); \
			break

	/* read the file */
	for (cpBoard.height = 0; *it; ++cpBoard.height) {
		int x;
		for (x = 0; *it && *it != '\n' && *it != '\n'; ++x, ++it) {
			switch (*it) {
			SHIP_CASE('^', NORTH);
			SHIP_CASE('>', EAST);
			SHIP_CASE('V', SOUTH);
			SHIP_CASE('<', WEST);
			case '#': case '@': sb_push(&cpBoard.objs, EMPTY); break;
			case 't': case 'T': sb_push(&cpBoard.objs, TREASURE); break;
			case 'b': case 'B': sb_push(&cpBoard.objs, BUOY); break;
			case 'o' :case 'O': sb_push(&cpBoard.objs, OBSTACLE); break;
			default:
				fprintf(stderr,
				        "unknown obstacle '%c' %dx%d\n",
				        *it, x, cpBoard.height);
				exit(EXIT_FAILURE);
			}
		}
		if (x != cpBoard.width) {
			fprintf(stderr,
				"expected width %d, got %d on line %d\n",
				cpBoard.width - 1, x, cpBoard.height);
			exit(EXIT_FAILURE);
		}
		while (*it && *it == '\n' && *it == '\n') ++it;
	}
	if (cpBoard.shipFacing == NONE) {
		cpBoard.shipFacing = EAST;
		cpBoard.shipx = cpBoard.shipy = 0;
		cpBoard.objs.at[0] = EMPTY;
	}
}

static void
cp_board_load_file(const char *path)
{
	FILE *f = fopen(path, "rb");
	if (!f) {
		fprintf(stderr, "failed to open file '%s'\n", path);
		exit(EXIT_FAILURE);
	}
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *str = malloc(size + 1);
	fread(str, 1, size, f);
	str[size] = 0;
	cp_board_load(str);
	fclose(f);
	free(str);
}


void
move(void)
{
	switch (cpBoard.shipFacing) {
	case NORTH: --cpBoard.shipy; break;
	case EAST: ++cpBoard.shipx; break;
	case SOUTH: ++cpBoard.shipy; break;
	case WEST: --cpBoard.shipx; break;
	default: assert(1); break;
	}
	cpBoard.shipx = CP_CLAMP(cpBoard.shipx, 0, cpBoard.width - 1);
	cpBoard.shipy = CP_CLAMP(cpBoard.shipy, 0, cpBoard.height - 1);

	if (cpBoard.objs.at[cpBoard.shipy*cpBoard.width + cpBoard.shipx] == OBSTACLE) {
		printf("Ship sunk at %dx%d\n", cpBoard.shipx, cpBoard.shipy);
		exit(EXIT_SUCCESS);
	}
	cp_update_board(-1, -1);
}

void
turn(Dir dir)
{
	switch (dir) {
	case RIGHT: cpBoard.shipFacing = (cpBoard.shipFacing + 1) % 4; break;
	case BACK: cpBoard.shipFacing = (cpBoard.shipFacing + 2) % 4; break;
	case LEFT: cpBoard.shipFacing = (cpBoard.shipFacing + 3) % 4; break;
	default: assert(1); break;
	}
	cp_update_board(-1, -1);
}

Obj
look(Dir dir)
{
	int x = cpBoard.shipx, y = cpBoard.shipy;
	CpFacing f = cpBoard.shipFacing;

	switch (dir) {
	case HERE:
		cp_update_board(x, y);
		return cpBoard.objs.at[y * cpBoard.width + x];
	case RIGHT: f = (f + 1) % 4; break;
	case BACK: f = (f + 2) % 4; break;
	case LEFT: f = (f + 3) % 4; break;
	default: assert(1); break;
	}

	switch (f) {
	case NORTH: --y; break;
	case EAST: ++x; break;
	case SOUTH: ++y; break;
	case WEST: --x; break;
	default: assert(1); break;
	}

	cp_update_board(x, y);

	if (x < 0 || x >= cpBoard.width || y < 0 || y >= cpBoard.height) {
		return BORDER;
	} else {
		return cpBoard.objs.at[y * cpBoard.width + x];
	}
}

Obj
take(void)
{
	Obj res = cpBoard.objs.at[cpBoard.shipy * cpBoard.width + cpBoard.shipx];
	cpBoard.objs.at[cpBoard.shipy * cpBoard.width + cpBoard.shipx] = EMPTY;
	cp_update_board(-1, -1);
	return res;
}

void
place(Obj obj)
{
	assert(obj == TREASURE || obj == BUOY);
	cpBoard.objs.at[cpBoard.shipy * cpBoard.width + cpBoard.shipx] = obj;
	cp_update_board(-1, -1);
}

static void
cp_board_print(int hx, int hy)
{
	int x, y;
	for (y = 0; y < cpBoard.height; ++y, putchar('\n'))
	for (x = 0; x < cpBoard.width; ++x) {
		if (x == cpBoard.shipx && y == cpBoard.shipy) {
			switch (cpBoard.shipFacing) {
			case NORTH: putchar('^'); break;
			case EAST: putchar('>'); break;
			case SOUTH: putchar('V'); break;
			case WEST: putchar('<'); break;
			default: assert(1); break;
			}
		} else if (x == hx && y == hy) {
			switch (cpBoard.objs.at[y * cpBoard.width + x]) {
			case EMPTY: putchar('@'); break;
			case TREASURE: putchar('T'); break;
			case BUOY: putchar('B'); break;
			case OBSTACLE: putchar('O'); break;
			default: assert(1); break;
			}
		} else {
			switch (cpBoard.objs.at[y * cpBoard.width + x]) {
			case EMPTY: putchar('#'); break;
			case TREASURE: putchar('t'); break;
			case BUOY: putchar('b'); break;
			case OBSTACLE: putchar('o'); break;
			default: assert(1); break;
			}
		}
	}
	putchar('\n');
}

