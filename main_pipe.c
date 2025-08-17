#include "cpirates.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int cpdone = 0;

static void child_init(char *const argv[]);
static size_t child_read(void *buf, size_t nbyte);
static size_t child_write(void const *buf, size_t nbyte);

static Dir
read_dir(void)
{
	char ch;
	child_read(&ch, 1);
	switch (ch) {
	case 'h': return HERE;
	case 'f': return FRONT;
	case 'r': return RIGHT;
	case 'b': return BACK;
	case 'l': return LEFT;
	default: assert(0); break;
	}
}

static Obj
read_obj(void)
{
	char ch;
	child_read(&ch, 1);
	switch (ch) {
	case 'e': return EMPTY;
	case 't': return TREASURE;
	case 'b': return BUOY;
	case 'o': return OBSTACLE;
	case 'B': return BORDER;
	default: assert(0); break;
	}
}

static void
write_obj(Obj obj)
{
	char ch[2] = { 'e', '\n' };
	switch (obj) {
	case EMPTY: *ch = 'e'; break;
	case TREASURE: *ch = 't'; break;
	case BUOY: *ch = 'b'; break;
	case OBSTACLE: *ch = 'o'; break;
	case BORDER: *ch = 'B'; break;
	default: assert(0); break;
	}
	child_write(ch, 2);
}


#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>



#define PERR_IF(cond, msg) \
	if (cond) { \
		perror(msg); \
		exit(EXIT_FAILURE); \
	}

static int outfd[2];
static int infd[2];
static pid_t pid = 0;

void
run(char *const argv[])
{
	char ch;
	child_init(argv);

	while (!cpdone && child_read(&ch, 1)) {
		switch (ch) {
		case 'm': move(); break;
		case 't': turn(read_dir()); break;
		case 'l': write_obj(look(read_dir())); break;
		case 'T': write_obj(take()); break;
		case 'p': place(read_obj()); break;
		default:
			break;
		}
	}

	kill(pid, SIGKILL);
}

static void
child_init(char *const argv[])
{
	PERR_IF(pipe(outfd) < 0 || pipe(infd) < 0, "pipe");

	switch ((pid = fork())) {
	case -1:
		perror("fork");
		exit(EXIT_FAILURE);
	case 0:
		/* Child */
		/*
		setbuf(stdout, NULL);
		setbuf(stdin, NULL);
		*/

		PERR_IF(close(STDOUT_FILENO) < 0 || close(STDIN_FILENO) < 0, "close");
		PERR_IF(dup2(infd[1], STDOUT_FILENO) < 0 ||
		        dup2(outfd[0], STDIN_FILENO) < 0, "dup2");
		PERR_IF(close(outfd[0]) < 0 || close(outfd[1]) < 0 ||
		        close(infd[0]) < 0 || close(infd[1]) < 0, "close");

		/* run client */
		PERR_IF(execv(argv[0], argv), "execv");
		/* There was an error if this is executed */
		perror("execv");
		exit(EXIT_FAILURE);
	default: break;
	}

	/* If the child process will exit unexpectedly, */
	/* the parent process will obtain SIGCHLD signal that */
	/* can be handled (e.g. you can respawn the child process). */

	/* close unused pipe ends */
	PERR_IF(close(outfd[0]) < 0 || close(infd[1]) < 0, "close");
}

static size_t
child_read(void *buf, size_t nbyte)
{
	ssize_t ret = read(infd[0], buf, nbyte);
	if (ret < 0) {
		perror("read");
		return 0;
	} else {
		return (size_t)ret;
	}
}

static size_t
child_write(void const *buf, size_t nbyte)
{
	ssize_t ret = write(outfd[1], buf, nbyte);
	if (ret < 0) {
		perror("write");
		return 0;
	} else {
		return (size_t)ret;
	}
}
