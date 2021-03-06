
































#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

#define NUM_ELEMS(_x) (sizeof(_x) / sizeof((_x)[0]))

#if !defined(VERSION)
#define VERSION "(unknown version)"
#endif


#define SPOIL(_x) volatile long x = (long)(_x); (void)x


typedef void (*stub_t)(void *dest, void *src, size_t n);


struct test
{

const char *name;

stub_t stub;
};


static void empty(volatile char *against)
{



int offset = (1024 - 256)*1024;

for (int i = offset; i < offset + 16*1024*3; i += 64)
{
against[i];
}
}


static void xbounce(void *dest, void *src, size_t n)
{
SPOIL(0);
}


static void xmemcpy(void *dest, void *src, size_t n)
{
SPOIL(memcpy(dest, src, n));
}


static void xmemset(void *dest, void *src, size_t n)
{
SPOIL(memset(dest, 0, n));
}


static void xmemcmp(void *dest, void *src, size_t n)
{
SPOIL(memcmp(dest, src, n));
}


static void xstrcpy(void *dest, void *src, size_t n)
{
SPOIL(strcpy(dest, src));
}


static void xstrlen(void *dest, void *src, size_t n)
{
SPOIL(strlen(dest));
}


static void xstrcmp(void *dest, void *src, size_t n)
{
SPOIL(strcmp(dest, src));
}


static void xstrchr(void *dest, void *src, size_t n)
{

((char *)src)[n-1] = 32;
SPOIL(strchr(src, 32));
}


static void xmemchr(void *dest, void *src, size_t n)
{

((char *)src)[n-1] = 32;
SPOIL(memchr(src, 32, n));
}


static const struct test tests[] =
{
{ "bounce", xbounce },
{ "memchr", xmemchr },
{ "memcpy", xmemcpy },
{ "memset", xmemset },
{ "memcmp", xmemcmp },
{ "strchr", xstrchr },
{ "strcmp", xstrcmp },
{ "strcpy", xstrcpy },
{ "strlen", xstrlen },
{ NULL }
};


static void usage(const char* name)
{
printf("%s %s: run a string related benchmark.\n"
"usage: %s [-c block-size] [-l loop-count] [-a alignment|src_alignment:dst_alignment] [-f] [-t test-name] [-r run-id]\n"
, name, VERSION, name);

printf("Tests:");

for (const struct test *ptest = tests; ptest->name != NULL; ptest++)
{
printf(" %s", ptest->name);
}

printf("\n");

exit(-1);
}


static const struct test *find_test(const char *name)
{
if (name == NULL)
{
return tests + 0;
}
else
{
for (const struct test *p = tests; p->name != NULL; p++)
{
if (strcmp(p->name, name) == 0)
{
return p;
}
}
}

return NULL;
}

#define MIN_BUFFER_SIZE 1024*1024
#define MAX_ALIGNMENT 256


static char *realign(char *p, int alignment)
{
uintptr_t pp = (uintptr_t)p;
pp = (pp + (MAX_ALIGNMENT - 1)) & ~(MAX_ALIGNMENT - 1);
pp += alignment;

return (char *)pp;
}

static int parse_int_arg(const char *arg, const char *exe_name)
{
long int ret;

errno = 0;
ret = strtol(arg, NULL, 0);

if (errno)
{
usage(exe_name);
}

return (int)ret;
}

static void parse_alignment_arg(const char *arg, const char *exe_name,
int *src_alignment, int *dst_alignment)
{
long int ret;
char *endptr;

errno = 0;
ret = strtol(arg, &endptr, 0);

if (errno)
{
usage(exe_name);
}

*src_alignment = (int)ret;

if (ret > 256 || ret < 1)
{
printf("Alignment should be in the range [1, 256].\n");
usage(exe_name);
}

if (ret == 256)
ret = 0;

if (endptr && *endptr == ':')
{
errno = 0;
ret = strtol(endptr + 1, NULL, 0);

if (errno)
{
usage(exe_name);
}

if (ret > 256 || ret < 1)
{
printf("Alignment should be in the range [1, 256].\n");
usage(exe_name);
}

if (ret == 256)
ret = 0;
}

*dst_alignment = (int)ret;
}


int main(int argc, char **argv)
{

size_t buffer_size = MIN_BUFFER_SIZE;


int count = 31;

int loops = 10000000;

int flush = 0;

const char *name = NULL;

int src_alignment = 8;
int dst_alignment = 8;

const char *run_id = "0";

int opt;

while ((opt = getopt(argc, argv, "c:l:ft:r:hva:")) > 0)
{
switch (opt)
{
case 'c':
count = parse_int_arg(optarg, argv[0]);
break;
case 'l':
loops = parse_int_arg(optarg, argv[0]);
break;
case 'a':
parse_alignment_arg(optarg, argv[0], &src_alignment, &dst_alignment);
break;
case 'f':
flush = 1;
break;
case 't':
name = strdup(optarg);
break;
case 'r':
run_id = strdup(optarg);
break;
case 'h':
usage(argv[0]);
break;
default:
usage(argv[0]);
break;
}
}


const struct test *ptest = find_test(name);

if (ptest == NULL)
{
usage(argv[0]);
}

if (count + MAX_ALIGNMENT * 2 > MIN_BUFFER_SIZE)
{
buffer_size = count + MAX_ALIGNMENT * 2;
}


char *src = malloc(buffer_size);
char *dest = malloc(buffer_size);

assert(src != NULL && dest != NULL);

src = realign(src, src_alignment);
dest = realign(dest, dst_alignment);


srandom(1539);

for (int i = 0; i < buffer_size; i++)
{
src[i] = (char)random() | 1;
dest[i] = src[i];
}


src[count] = 0;
dest[count] = 0;

struct timespec start, end;
int err = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
assert(err == 0);


stub_t stub = ptest->stub;


if (flush == 0)
{
for (int i = 0; i < loops; i++)
{
(*stub)(dest, src, count);
}
}
else
{
for (int i = 0; i < loops; i++)
{
(*stub)(dest, src, count);
empty(dest);
}
}

err = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
assert(err == 0);


char *variant = strrchr(argv[0], '/');

if (variant == NULL)
{
variant = argv[0];
}

variant = strstr(variant, "try-");
assert(variant != NULL);

double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

double bounced = 0.448730 * loops / 50000000;


printf("%s:%s:%u:%u:%d:%d:%s:%.6f: took %.6f s for %u calls to %s of %u bytes. ~%.3f MB/s corrected.\n",
variant + 4, ptest->name,
count, loops, src_alignment, dst_alignment, run_id,
elapsed,
elapsed, loops, ptest->name, count,
(double)loops*count/(elapsed - bounced)/(1024*1024));

return 0;
}
