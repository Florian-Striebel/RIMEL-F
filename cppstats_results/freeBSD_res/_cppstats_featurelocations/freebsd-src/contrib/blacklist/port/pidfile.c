





























#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: pidfile.c,v 1.2 2016/04/05 12:28:57 christos Exp $");
#endif

#include <sys/param.h>

#include <paths.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#if defined(HAVE_LIBUTIL_H)
#include <libutil.h>
#endif
#if defined(HAVE_UTIL_H)
#include <util.h>
#endif

static pid_t pidfile_pid;
static char *pidfile_path;


static void
pidfile_cleanup(void)
{

if ((pidfile_path != NULL) && (pidfile_pid == getpid()))
(void) unlink(pidfile_path);
}






static int
register_atexit_handler(void)
{
static bool done = false;

if (!done) {
if (atexit(pidfile_cleanup) < 0)
return -1;
done = true;
}

return 0;
}







static int
cleanup_old_pidfile(const char* path)
{
if (pidfile_path != NULL) {
if (strcmp(pidfile_path, path) != 0) {
pidfile_cleanup();

free(pidfile_path);
pidfile_path = NULL;

return 1;
} else
return 0;
} else
return 1;
}







static char *
generate_varrun_path(const char *bname)
{
char *path;

if (bname == NULL)
bname = getprogname();


if (asprintf(&path, "%s%s.pid", _PATH_VARRUN, bname) == -1)
return NULL;
return path;
}







static int
create_pidfile(const char* path)
{
FILE *f;

if (register_atexit_handler() == -1)
return -1;

if (cleanup_old_pidfile(path) == 0)
return 0;

pidfile_path = strdup(path);
if (pidfile_path == NULL)
return -1;

if ((f = fopen(path, "w")) == NULL) {
free(pidfile_path);
pidfile_path = NULL;
return -1;
}

pidfile_pid = getpid();

(void) fprintf(f, "%d\n", pidfile_pid);
(void) fclose(f);

return 0;
}

int
pidfile(const char *path)
{

if (path == NULL || strchr(path, '/') == NULL) {
char *default_path;

if ((default_path = generate_varrun_path(path)) == NULL)
return -1;

if (create_pidfile(default_path) == -1) {
free(default_path);
return -1;
}

free(default_path);
return 0;
} else
return create_pidfile(path);
}
