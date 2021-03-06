

























#pragma ident "%Z%%M% %I% %E% SMI"

#include <ctf_impl.h>
#include <sys/mman.h>
#include <stdarg.h>

void *
ctf_data_alloc(size_t size)
{
return (mmap(NULL, size, PROT_READ | PROT_WRITE,
MAP_PRIVATE | MAP_ANON, -1, 0));
}

void
ctf_data_free(void *buf, size_t size)
{
(void) munmap(buf, size);
}

void
ctf_data_protect(void *buf, size_t size)
{
(void) mprotect(buf, size, PROT_READ);
}

void *
ctf_alloc(size_t size)
{
return (malloc(size));
}


void
ctf_free(void *buf, __unused size_t size)
{
free(buf);
}

const char *
ctf_strerror(int err)
{
return ((const char *) strerror(err));
}


void
ctf_dprintf(const char *format, ...)
{
if (_libctf_debug) {
va_list alist;

va_start(alist, format);
(void) fputs("libctf DEBUG: ", stderr);
(void) vfprintf(stderr, format, alist);
va_end(alist);
}
}
