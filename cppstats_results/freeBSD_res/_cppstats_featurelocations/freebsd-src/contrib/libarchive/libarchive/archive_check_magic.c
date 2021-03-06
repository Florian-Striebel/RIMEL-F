
























#include "archive_platform.h"
__FBSDID("$FreeBSD$");

#if defined(HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif

#include <stdio.h>
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#include <winbase.h>
#endif

#include "archive_private.h"

static void
errmsg(const char *m)
{
size_t s = strlen(m);
ssize_t written;

while (s > 0) {
written = write(2, m, s);
if (written <= 0)
return;
m += written;
s -= written;
}
}

static __LA_DEAD void
diediedie(void)
{
#if defined(_WIN32) && !defined(__CYGWIN__) && defined(_DEBUG)

DebugBreak();
#endif
abort();
}

static const char *
state_name(unsigned s)
{
switch (s) {
case ARCHIVE_STATE_NEW: return ("new");
case ARCHIVE_STATE_HEADER: return ("header");
case ARCHIVE_STATE_DATA: return ("data");
case ARCHIVE_STATE_EOF: return ("eof");
case ARCHIVE_STATE_CLOSED: return ("closed");
case ARCHIVE_STATE_FATAL: return ("fatal");
default: return ("??");
}
}

static const char *
archive_handle_type_name(unsigned m)
{
switch (m) {
case ARCHIVE_WRITE_MAGIC: return ("archive_write");
case ARCHIVE_READ_MAGIC: return ("archive_read");
case ARCHIVE_WRITE_DISK_MAGIC: return ("archive_write_disk");
case ARCHIVE_READ_DISK_MAGIC: return ("archive_read_disk");
case ARCHIVE_MATCH_MAGIC: return ("archive_match");
default: return NULL;
}
}


static char *
write_all_states(char *buff, unsigned int states)
{
unsigned int lowbit;

buff[0] = '\0';


while ((lowbit = states & (1 + ~states)) != 0) {
states &= ~lowbit;
strcat(buff, state_name(lowbit));
if (states != 0)
strcat(buff, "/");
}
return buff;
}










int
__archive_check_magic(struct archive *a, unsigned int magic,
unsigned int state, const char *function)
{
char states1[64];
char states2[64];
const char *handle_type;






handle_type = archive_handle_type_name(a->magic);

if (!handle_type) {
errmsg("PROGRAMMER ERROR: Function ");
errmsg(function);
errmsg(" invoked with invalid archive handle.\n");
diediedie();
}

if (a->magic != magic) {
archive_set_error(a, -1,
"PROGRAMMER ERROR: Function '%s' invoked"
" on '%s' archive object, which is not supported.",
function,
handle_type);
a->state = ARCHIVE_STATE_FATAL;
return (ARCHIVE_FATAL);
}

if ((a->state & state) == 0) {

if (a->state != ARCHIVE_STATE_FATAL)
archive_set_error(a, -1,
"INTERNAL ERROR: Function '%s' invoked with"
" archive structure in state '%s',"
" should be in state '%s'",
function,
write_all_states(states1, a->state),
write_all_states(states2, state));
a->state = ARCHIVE_STATE_FATAL;
return (ARCHIVE_FATAL);
}
return ARCHIVE_OK;
}
