#include "cpirates.c"
#include "arg.h"

#include <threads.h>

/* config */
static float sleepfor = 0.5;

static void
cp_update_board(int x, int y)
{
	struct timespec duration = { 0 };
	int secs = sleepfor;
	duration.tv_sec = secs;
	duration.tv_nsec = 1000000000*(sleepfor-secs);
	thrd_sleep(&duration, (void*)0);
	cp_board_print(x, y);
}

int
main(int argc, char **argv)
{
	char *argv0 = argv[0];
	ARG_BEGIN {
		if (ARG_LONG("delay")) case 'd': {
			sscanf(ARG_VAL(), "%f", &sleepfor);
		} else if (ARG_LONG("help")) case 'h': case '?': {
			usage:
			printf("Usage: %s [OPTIONS...] MAP_TXT EXECUTABLE [args...]\n", argv0);
			printf("Example usage of arg.h\n\n");
			printf("Options:\n");
			printf("  -d, --delay=SEC   set update delay (current: %f sec)\n", sleepfor);
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
	cp_board_print(-1, -1);
	run(&argv[1]);
	return EXIT_SUCCESS;
}
