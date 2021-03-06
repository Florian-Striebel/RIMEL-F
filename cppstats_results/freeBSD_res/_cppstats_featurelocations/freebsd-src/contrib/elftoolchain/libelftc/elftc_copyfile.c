

























#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <unistd.h>

#include "libelftc.h"
#include "_libelftc.h"

#if ELFTC_HAVE_MMAP
#include <sys/mman.h>
#endif

ELFTC_VCSID("$Id: elftc_copyfile.c 3297 2016-01-09 15:26:34Z jkoshy $");






int
elftc_copyfile(int ifd, int ofd)
{
size_t file_size, n;
int buf_mmapped;
struct stat sb;
char *b, *buf;
ssize_t nr, nw;


if (fstat(ifd, &sb) < 0)
return (-1);


if (sb.st_size == 0)
return (0);

buf = NULL;
buf_mmapped = 0;
file_size = (size_t) sb.st_size;

#if ELFTC_HAVE_MMAP



buf = mmap(NULL, file_size, PROT_READ, MAP_SHARED, ifd, (off_t) 0);
if (buf != MAP_FAILED)
buf_mmapped = 1;
else
buf = NULL;
#endif





if (buf_mmapped == false) {
if ((buf = malloc(file_size)) == NULL)
return (-1);
b = buf;
for (n = file_size; n > 0; n -= (size_t) nr, b += nr) {
if ((nr = read(ifd, b, n)) < 0) {
free(buf);
return (-1);
}
}
}




for (n = file_size, b = buf; n > 0; n -= (size_t) nw, b += nw)
if ((nw = write(ofd, b, n)) <= 0)
break;


#if ELFTC_HAVE_MMAP
if (buf_mmapped && munmap(buf, file_size) < 0)
return (-1);
#endif

if (!buf_mmapped)
free(buf);

return (n > 0 ? -1 : 0);
}

