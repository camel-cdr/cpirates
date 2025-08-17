#include "cpirates.c"
#include "arg.h"

#include <SDL2/SDL.h>

/* config */
static float sleepfor = 1.0;

static int cpHighliteX = -1, cpHighliteY = -1;
static int cpinit = 0;
static int cpverbose = 0;
extern int cpdone;

static int cp_main(void *args);

static void
cp_update_board(int x, int y)
{
	if (cpverbose) cp_board_print(x, y);
	cpHighliteX = x;
	cpHighliteY = y;
	do SDL_Delay(1000 * sleepfor);
	while (!cpinit);
}

int
main(int argc, char **argv)
{
	char *argv0 = argv[0];
	ARG_BEGIN {
		if (ARG_LONG("verbose")) case 'v': {
			cpverbose = 1;
		} else if (ARG_LONG("delay")) case 'd': {
			sscanf(ARG_VAL(), "%f", &sleepfor);
		} else if (ARG_LONG("help")) case 'h': case '?': {
			usage:
			printf("Usage: %s [OPTIONS...] MAP_TXT EXECUTABLE [args...]\n", argv0);
			printf("Example usage of arg.h\n\n");
			printf("Options:\n");
			printf("  -d, --delay=SEC   set update delay (current: %f sec)\n", sleepfor);
			printf("  -v, --verbose     log board states to stdout\n");
			printf("  -h, --help        display this help and exit\n");
			return EXIT_SUCCESS;
		} else { default:
			fprintf(stderr,
			        "%s: invalid option '%s'\n"
			        "Try '%s --help' for more information.\n",
			        argv0, *argv, argv0);
			return EXIT_FAILURE;
		}
	} ARG_END;
	if (argc < 2) goto usage;

	cp_board_load_file(argv[0]);
	SDL_Thread *thread;

	thread = SDL_CreateThread(cp_main, "Sdl2 Thread", (void*)NULL);
	if (!thread) {
		fprintf(stderr, "SDL_CreateThread failed: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	cp_update_board(-1, -1);
	run(&argv[1]);
	SDL_WaitThread(thread, NULL);

	return EXIT_SUCCESS;
}


#define SDL_ASSERT(x) \
	if (!(x)) { \
		fprintf(stderr, "%s failed: %s", #x, SDL_GetError()); \
		exit(EXIT_FAILURE); \
	}


static struct {
	SDL_Texture *north, *east, *south, *west;
	SDL_Texture *empty, *obstacle, *treasure, *buoy;
	SDL_Texture *highlite;
	SDL_Window *wnd;
	SDL_Renderer *ren;
} cp = { 0 };

static SDL_Texture *
cp_load_texture(char *path)
{
	SDL_Texture *tex;
	SDL_Surface *surf;
	SDL_ASSERT(surf = SDL_LoadBMP(path));
	SDL_ASSERT(tex = SDL_CreateTextureFromSurface(cp.ren, surf));
	SDL_FreeSurface(surf);
	return tex;
}

static void
cp_render(void)
{
	int x, y;
	SDL_Rect rect;

	SDL_GetWindowSize(cp.wnd, &rect.w, &rect.h);
	rect.w /= cpBoard.width;
	rect.h /= cpBoard.height;
	rect.w = rect.h = CP_MIN(rect.w, rect.h);

	for (x = 0; x < cpBoard.width; ++x)
	for (y = 0; y < cpBoard.height; ++y) {
		SDL_Texture *t;
		rect.x = x * rect.w;
		rect.y = y * rect.h;

		switch (cpBoard.objs.at[y * cpBoard.width + x]) {
		case TREASURE: t = cp.treasure; break;
		case BUOY:     t = cp.buoy; break;
		case OBSTACLE: t = cp.obstacle; break;
		default:
		case EMPTY:    t = cp.empty; break;
		}
		SDL_RenderCopy(cp.ren, t, NULL, &rect);

		if (x == cpBoard.shipx && y == cpBoard.shipy) {
			switch (cpBoard.shipFacing) {
			case NORTH: t = cp.north; break;
			case EAST: t = cp.east; break;
			case SOUTH: t = cp.south; break;
			case WEST: t = cp.west; break;
			default: assert(1); break;
			}
			SDL_RenderCopy(cp.ren, t, NULL, &rect);
		}
		if (x == cpHighliteX && cpHighliteY == y) {
			SDL_RenderCopy(cp.ren, cp.highlite, NULL, &rect);
		}
	}
}

static int
cp_main(void *args)
{
	(void)args;

	SDL_ASSERT(SDL_Init(SDL_INIT_VIDEO) >= 0);
	SDL_ASSERT(cp.wnd = SDL_CreateWindow(
				"cpirates",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				cpBoard.width * 32, cpBoard.height * 32,
				SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
	SDL_ASSERT(cp.ren = SDL_CreateRenderer(cp.wnd, -1, SDL_RENDERER_ACCELERATED));

	cp.north = cp_load_texture("res/north.bmp");
	cp.east = cp_load_texture("res/east.bmp");
	cp.south = cp_load_texture("res/south.bmp");
	cp.west = cp_load_texture("res/west.bmp");

	cp.empty = cp_load_texture("res/empty.bmp");
	cp.obstacle = cp_load_texture("res/obstacle.bmp");
	cp.treasure = cp_load_texture("res/treasure.bmp");
	cp.buoy = cp_load_texture("res/buoy.bmp");

	cp.highlite = cp_load_texture("res/highlite.bmp");
	cpinit = 1;

	while (!cpdone) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				cpdone = 1;
				break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE: case SDLK_q:
					cpdone = 1;
					break;
				case SDLK_LEFT:  sleepfor *= 1.5; break;
				case SDLK_RIGHT: sleepfor /= 1.5; break;
				}
				break;
			}
		}
		SDL_RenderClear(cp.ren);
		cp_render();
		SDL_RenderPresent(cp.ren);
	}

	SDL_DestroyTexture(cp.highlite);

	SDL_DestroyTexture(cp.buoy);
	SDL_DestroyTexture(cp.treasure);
	SDL_DestroyTexture(cp.obstacle);
	SDL_DestroyTexture(cp.empty);

	SDL_DestroyTexture(cp.west);
	SDL_DestroyTexture(cp.south);
	SDL_DestroyTexture(cp.east);
	SDL_DestroyTexture(cp.north);

	SDL_DestroyRenderer(cp.ren);
	SDL_DestroyWindow(cp.wnd);

	return EXIT_SUCCESS;
}

