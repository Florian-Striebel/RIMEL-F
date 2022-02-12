

























#include "archive_platform.h"


#if defined(HAVE_PIPE) && defined(HAVE_FCNTL) && (defined(HAVE_FORK) || defined(HAVE_VFORK) || defined(HAVE_POSIX_SPAWNP))


__FBSDID("$FreeBSD: head/lib/libarchive/filter_fork.c 182958 2008-09-12 05:33:00Z kientzle $");

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_POLL) && (defined(HAVE_POLL_H) || defined(HAVE_SYS_POLL_H))
#if defined(HAVE_POLL_H)
#include <poll.h>
#elif defined(HAVE_SYS_POLL_H)
#include <sys/poll.h>
#endif
#elif defined(HAVE_SELECT)
#if defined(HAVE_SYS_SELECT_H)
#include <sys/select.h>
#elif defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#endif
#if defined(HAVE_FCNTL_H)
#include <fcntl.h>
#endif
#if defined(HAVE_SPAWN_H)
#include <spawn.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "archive.h"
#include "archive_cmdline_private.h"

#include "filter_fork.h"

int
__archive_create_child(const char *cmd, int *child_stdin, int *child_stdout,
pid_t *out_child)
{
pid_t child;
int stdin_pipe[2], stdout_pipe[2], tmp;
#if HAVE_POSIX_SPAWNP
posix_spawn_file_actions_t actions;
int r;
#endif
struct archive_cmdline *cmdline;

cmdline = __archive_cmdline_allocate();
if (cmdline == NULL)
goto state_allocated;
if (__archive_cmdline_parse(cmdline, cmd) != ARCHIVE_OK)
goto state_allocated;

if (pipe(stdin_pipe) == -1)
goto state_allocated;
if (stdin_pipe[0] == 1 ) {
if ((tmp = dup(stdin_pipe[0])) == -1)
goto stdin_opened;
close(stdin_pipe[0]);
stdin_pipe[0] = tmp;
}
if (pipe(stdout_pipe) == -1)
goto stdin_opened;
if (stdout_pipe[1] == 0 ) {
if ((tmp = dup(stdout_pipe[1])) == -1)
goto stdout_opened;
close(stdout_pipe[1]);
stdout_pipe[1] = tmp;
}

#if HAVE_POSIX_SPAWNP

r = posix_spawn_file_actions_init(&actions);
if (r != 0) {
errno = r;
goto stdout_opened;
}
r = posix_spawn_file_actions_addclose(&actions, stdin_pipe[1]);
if (r != 0)
goto actions_inited;
r = posix_spawn_file_actions_addclose(&actions, stdout_pipe[0]);
if (r != 0)
goto actions_inited;

r = posix_spawn_file_actions_adddup2(&actions, stdin_pipe[0], 0);
if (r != 0)
goto actions_inited;
if (stdin_pipe[0] != 0 ) {
r = posix_spawn_file_actions_addclose(&actions, stdin_pipe[0]);
if (r != 0)
goto actions_inited;
}

r = posix_spawn_file_actions_adddup2(&actions, stdout_pipe[1], 1);
if (r != 0)
goto actions_inited;
if (stdout_pipe[1] != 1 ) {
r = posix_spawn_file_actions_addclose(&actions, stdout_pipe[1]);
if (r != 0)
goto actions_inited;
}
r = posix_spawnp(&child, cmdline->path, &actions, NULL,
cmdline->argv, NULL);
if (r != 0)
goto actions_inited;
posix_spawn_file_actions_destroy(&actions);

#else

#if HAVE_VFORK
child = vfork();
#else
child = fork();
#endif
if (child == -1)
goto stdout_opened;
if (child == 0) {
close(stdin_pipe[1]);
close(stdout_pipe[0]);
if (dup2(stdin_pipe[0], 0 ) == -1)
_exit(254);
if (stdin_pipe[0] != 0 )
close(stdin_pipe[0]);
if (dup2(stdout_pipe[1], 1 ) == -1)
_exit(254);
if (stdout_pipe[1] != 1 )
close(stdout_pipe[1]);
execvp(cmdline->path, cmdline->argv);
_exit(254);
}
#endif

close(stdin_pipe[0]);
close(stdout_pipe[1]);

*child_stdin = stdin_pipe[1];
fcntl(*child_stdin, F_SETFL, O_NONBLOCK);
*child_stdout = stdout_pipe[0];
fcntl(*child_stdout, F_SETFL, O_NONBLOCK);
__archive_cmdline_free(cmdline);

*out_child = child;
return ARCHIVE_OK;

#if HAVE_POSIX_SPAWNP
actions_inited:
errno = r;
posix_spawn_file_actions_destroy(&actions);
#endif
stdout_opened:
close(stdout_pipe[0]);
close(stdout_pipe[1]);
stdin_opened:
close(stdin_pipe[0]);
close(stdin_pipe[1]);
state_allocated:
__archive_cmdline_free(cmdline);
return ARCHIVE_FAILED;
}

void
__archive_check_child(int in, int out)
{
#if defined(HAVE_POLL) && (defined(HAVE_POLL_H) || defined(HAVE_SYS_POLL_H))
struct pollfd fds[2];
int idx;

idx = 0;
if (in != -1) {
fds[idx].fd = in;
fds[idx].events = POLLOUT;
++idx;
}
if (out != -1) {
fds[idx].fd = out;
fds[idx].events = POLLIN;
++idx;
}

poll(fds, idx, -1);
#elif defined(HAVE_SELECT)
fd_set fds_in, fds_out, fds_error;

FD_ZERO(&fds_in);
FD_ZERO(&fds_out);
FD_ZERO(&fds_error);
if (out != -1) {
FD_SET(out, &fds_in);
FD_SET(out, &fds_error);
}
if (in != -1) {
FD_SET(in, &fds_out);
FD_SET(in, &fds_error);
}
select(in < out ? out + 1 : in + 1, &fds_in, &fds_out, &fds_error, NULL);
#else
sleep(1);
#endif
}

#endif
