






















#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#if defined(__APPLE__)
#if defined(HAVE_MACH_MACH_TIME_H)
#include <mach/mach_time.h>
#endif
#endif

#include "ucl.h"

static double
get_ticks (void)
{
double res;

#if defined(__APPLE__)
res = mach_absolute_time () / 1000000000.;
#else
struct timespec ts;
clock_gettime (CLOCK_MONOTONIC, &ts);

res = (double)ts.tv_sec + ts.tv_nsec / 1000000000.;
#endif

return res;
}

int
main (int argc, char **argv)
{
void *map;
struct ucl_parser *parser;
ucl_object_t *obj;
int fin;
unsigned char *emitted;
struct stat st;
const char *fname_in = NULL;
int ret = 0;
double start, end, seconds;

switch (argc) {
case 2:
fname_in = argv[1];
break;
}

fin = open (fname_in, O_RDONLY);
if (fin == -1) {
perror ("open failed");
exit (EXIT_FAILURE);
}
parser = ucl_parser_new (UCL_PARSER_ZEROCOPY);

(void)fstat (fin, &st);
map = mmap (NULL, st.st_size, PROT_READ, MAP_SHARED, fin, 0);
if (map == MAP_FAILED) {
perror ("mmap failed");
exit (EXIT_FAILURE);
}

close (fin);

start = get_ticks ();
ucl_parser_add_chunk (parser, map, st.st_size);

obj = ucl_parser_get_object (parser);
end = get_ticks ();

seconds = end - start;
printf ("ucl: parsed input in %.4f seconds\n", seconds);
if (ucl_parser_get_error(parser)) {
printf ("Error occurred: %s\n", ucl_parser_get_error(parser));
ret = 1;
goto err;
}

start = get_ticks ();
emitted = ucl_object_emit (obj, UCL_EMIT_CONFIG);
end = get_ticks ();

seconds = end - start;
printf ("ucl: emitted config in %.4f seconds\n", seconds);

free (emitted);

start = get_ticks ();
emitted = ucl_object_emit (obj, UCL_EMIT_JSON);
end = get_ticks ();

seconds = end - start;
printf ("ucl: emitted json in %.4f seconds\n", seconds);

free (emitted);

start = get_ticks ();
emitted = ucl_object_emit (obj, UCL_EMIT_JSON_COMPACT);
end = get_ticks ();

seconds = end - start;
printf ("ucl: emitted compact json in %.4f seconds\n", seconds);

free (emitted);

start = get_ticks ();
emitted = ucl_object_emit (obj, UCL_EMIT_YAML);
end = get_ticks ();

seconds = end - start;
printf ("ucl: emitted yaml in %.4f seconds\n", seconds);

free (emitted);

ucl_parser_free (parser);
ucl_object_unref (obj);

err:
munmap (map, st.st_size);

return ret;
}
