

























#include <sys/param.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "_libpe.h"

ELFTC_VCSID("$Id: libpe_utils.c 3312 2016-01-10 09:23:51Z kaiwang27 $");

off_t
libpe_align(PE *pe, off_t off, size_t align)
{
off_t n;

assert(align > 0 && (align & (align - 1)) == 0);

n = roundup(off, align);
if (n > off) {
if (libpe_pad(pe, n - off) < 0)
return (-1);
}

return (n);
}

int
libpe_pad(PE *pe, size_t pad)
{
char tmp[128];
size_t s;

memset(tmp, 0, sizeof(tmp));
for (; pad > 0; pad -= s) {
s = pad > sizeof(tmp) ? sizeof(tmp) : pad;
if (write(pe->pe_fd, tmp, s) != (ssize_t) s) {
errno = EIO;
return (-1);
}
}

return (0);
}
