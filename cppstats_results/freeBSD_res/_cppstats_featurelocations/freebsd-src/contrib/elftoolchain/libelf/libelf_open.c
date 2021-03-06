

























#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>
#include <errno.h>
#include <libelf.h>
#include <stdlib.h>
#include <unistd.h>

#include "_libelf.h"

#if ELFTC_HAVE_MMAP
#include <sys/mman.h>
#endif

ELFTC_VCSID("$Id: libelf_open.c 3007 2014-03-22 08:10:14Z jkoshy $");

#define _LIBELF_INITSIZE (64*1024)




static void *
_libelf_read_special_file(int fd, size_t *fsz)
{
ssize_t readsz;
size_t bufsz, datasz;
unsigned char *buf, *t;

datasz = 0;
readsz = 0;
bufsz = _LIBELF_INITSIZE;
if ((buf = malloc(bufsz)) == NULL)
goto resourceerror;





do {

if (datasz == bufsz) {
bufsz *= 2;
if ((t = realloc(buf, bufsz)) == NULL)
goto resourceerror;
buf = t;
}

do {
assert(bufsz - datasz > 0);
t = buf + datasz;
if ((readsz = read(fd, t, bufsz - datasz)) <= 0)
break;
datasz += (size_t) readsz;
} while (datasz < bufsz);

} while (readsz > 0);

if (readsz < 0) {
LIBELF_SET_ERROR(IO, errno);
goto error;
}

assert(readsz == 0);




if (bufsz > datasz) {
if (datasz > 0) {
if ((t = realloc(buf, datasz)) == NULL)
goto resourceerror;
buf = t;
} else {
LIBELF_SET_ERROR(ARGUMENT, 0);
free(buf);
buf = NULL;
}
}

*fsz = datasz;
return (buf);

resourceerror:
LIBELF_SET_ERROR(RESOURCE, 0);
error:
if (buf != NULL)
free(buf);
return (NULL);
}






Elf *
_libelf_open_object(int fd, Elf_Cmd c, int reporterror)
{
Elf *e;
void *m;
mode_t mode;
size_t fsize;
struct stat sb;
unsigned int flags;

assert(c == ELF_C_READ || c == ELF_C_RDWR || c == ELF_C_WRITE);

if (fstat(fd, &sb) < 0) {
LIBELF_SET_ERROR(IO, errno);
return (NULL);
}

mode = sb.st_mode;
fsize = (size_t) sb.st_size;




if (!S_ISREG(mode) && !S_ISCHR(mode) && !S_ISFIFO(mode) &&
!S_ISSOCK(mode)) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}




if (c == ELF_C_WRITE) {
if ((e = _libelf_allocate_elf()) != NULL) {
_libelf_init_elf(e, ELF_K_ELF);
e->e_byteorder = LIBELF_PRIVATE(byteorder);
e->e_fd = fd;
e->e_cmd = c;
if (!S_ISREG(mode))
e->e_flags |= LIBELF_F_SPECIAL_FILE;
}

return (e);
}





m = NULL;
flags = 0;
if (S_ISREG(mode)) {




if (fsize == 0) {
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

#if ELFTC_HAVE_MMAP









m = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd, (off_t) 0);

if (m == MAP_FAILED)
m = NULL;
else
flags = LIBELF_F_RAWFILE_MMAP;
#endif





if (m == NULL) {
if ((m = malloc(fsize)) == NULL) {
LIBELF_SET_ERROR(RESOURCE, 0);
return (NULL);
}

if (read(fd, m, fsize) != (ssize_t) fsize) {
LIBELF_SET_ERROR(IO, errno);
free(m);
return (NULL);
}

flags = LIBELF_F_RAWFILE_MALLOC;
}
} else if ((m = _libelf_read_special_file(fd, &fsize)) != NULL)
flags = LIBELF_F_RAWFILE_MALLOC | LIBELF_F_SPECIAL_FILE;
else
return (NULL);

if ((e = _libelf_memory(m, fsize, reporterror)) == NULL) {
assert((flags & LIBELF_F_RAWFILE_MALLOC) ||
(flags & LIBELF_F_RAWFILE_MMAP));
if (flags & LIBELF_F_RAWFILE_MALLOC)
free(m);
#if ELFTC_HAVE_MMAP
else
(void) munmap(m, fsize);
#endif
return (NULL);
}


if (c == ELF_C_RDWR && e->e_kind == ELF_K_AR) {
(void) elf_end(e);
LIBELF_SET_ERROR(ARGUMENT, 0);
return (NULL);
}

e->e_flags |= flags;
e->e_fd = fd;
e->e_cmd = c;

return (e);
}
