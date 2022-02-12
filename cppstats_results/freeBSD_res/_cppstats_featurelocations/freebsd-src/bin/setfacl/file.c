

























#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/types.h>
#include <sys/acl.h>

#include <err.h>
#include <stdio.h>
#include <string.h>

#include "setfacl.h"




acl_t
get_acl_from_file(const char *filename)
{
FILE *file;
size_t len;
char buf[BUFSIZ+1];

if (filename == NULL)
err(1, "(null) filename in get_acl_from_file()");

if (strcmp(filename, "-") == 0) {
if (have_stdin)
err(1, "cannot specify more than one stdin");
file = stdin;
have_stdin = true;
} else {
file = fopen(filename, "r");
if (file == NULL)
err(1, "fopen() %s failed", filename);
}

len = fread(buf, (size_t)1, sizeof(buf) - 1, file);
buf[len] = '\0';
if (ferror(file) != 0) {
fclose(file);
err(1, "error reading from %s", filename);
} else if (feof(file) == 0) {
fclose(file);
errx(1, "line too long in %s", filename);
}

fclose(file);

return (acl_from_text(buf));
}
