
























#include <sys/types.h>

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static
int
h_echo(const char *msg)
{
printf("%s\n", msg);
return EXIT_SUCCESS;
}

static
int
h_exit_failure(void)
{
return EXIT_FAILURE;
}

static
int
h_exit_signal(void)
{
kill(getpid(), SIGKILL);
assert(0);
return EXIT_FAILURE;
}

static
int
h_exit_success(void)
{
return EXIT_SUCCESS;
}

static
int
h_stdout_stderr(const char *id)
{
fprintf(stdout, "Line 1 to stdout for %s\n", id);
fprintf(stdout, "Line 2 to stdout for %s\n", id);
fprintf(stderr, "Line 1 to stderr for %s\n", id);
fprintf(stderr, "Line 2 to stderr for %s\n", id);

return EXIT_SUCCESS;
}

static
void
check_args(const int argc, const char *const argv[], const int required)
{
if (argc < required) {
fprintf(stderr, "Usage: %s helper-name [args]\n", argv[0]);
exit(EXIT_FAILURE);
}
}

int
main(int argc, const char *const argv[])
{
int exitcode;

check_args(argc, argv, 2);

if (strcmp(argv[1], "echo") == 0) {
check_args(argc, argv, 3);
exitcode = h_echo(argv[2]);
} else if (strcmp(argv[1], "exit-failure") == 0)
exitcode = h_exit_failure();
else if (strcmp(argv[1], "exit-signal") == 0)
exitcode = h_exit_signal();
else if (strcmp(argv[1], "exit-success") == 0)
exitcode = h_exit_success();
else if (strcmp(argv[1], "stdout-stderr") == 0) {
check_args(argc, argv, 3);
exitcode = h_stdout_stderr(argv[2]);
} else {
fprintf(stderr, "%s: Unknown helper %s\n", argv[0], argv[1]);
exitcode = EXIT_FAILURE;
}

return exitcode;
}
