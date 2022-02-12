























#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "brssl.h"
#include "bearssl.h"

static void
print_blob(const char *name, const unsigned char *buf, size_t len)
{
size_t u;

printf("\nstatic const unsigned char %s[] = {", name);
for (u = 0; u < len; u ++) {
if (u != 0) {
printf(",");
}
if (u % 12 == 0) {
printf("\n\t");
} else {
printf(" ");
}
printf("0x%02X", buf[u]);
}
printf("\n};\n");
}

static void
usage_chain(void)
{
fprintf(stderr,
"usage: brssl chain [ options ] file...\n");
fprintf(stderr,
"options:\n");
fprintf(stderr,
" -q suppress verbose messages\n");
}


int
do_chain(int argc, char *argv[])
{
int retcode;
int verbose;
int i, num_files;
long k, ctr;

retcode = 0;
verbose = 1;
num_files = 0;
for (i = 0; i < argc; i ++) {
const char *arg;

arg = argv[i];
if (arg[0] != '-') {
num_files ++;
continue;
}
argv[i] = NULL;
if (eqstr(arg, "-v") || eqstr(arg, "-verbose")) {
verbose = 1;
} else if (eqstr(arg, "-q") || eqstr(arg, "-quiet")) {
verbose = 0;
} else {
fprintf(stderr, "ERROR: unknown option: '%s'\n", arg);
usage_chain();
goto chain_exit_error;
}
}
if (num_files == 0) {
fprintf(stderr, "ERROR: no certificate file provided\n");
usage_chain();
goto chain_exit_error;
}

ctr = 0;
for (i = 0; i < argc; i ++) {
const char *fname;
br_x509_certificate *xcs;
size_t u, num;

fname = argv[i];
if (fname == NULL) {
continue;
}
if (verbose) {
fprintf(stderr, "Reading file '%s': ", fname);
fflush(stderr);
}
xcs = read_certificates(fname, &num);
if (xcs == NULL) {
goto chain_exit_error;
}
if (verbose) {
fprintf(stderr, "%lu certificate%s\n",
(unsigned long)num, num > 1 ? "s" : "");
}
for (u = 0; u < num; u ++) {
char tmp[50];

sprintf(tmp, "CERT%ld", ctr ++);
print_blob(tmp, xcs[u].data, xcs[u].data_len);
xfree(xcs[u].data);
}
xfree(xcs);
}

printf("\nstatic const br_x509_certificate CHAIN[] = {");
for (k = 0; k < ctr; k ++) {
if (k != 0) {
printf(",");
}
printf("\n\t{ (unsigned char *)CERT%ld, sizeof CERT%ld }",
k, k);
}
printf("\n};\n");
printf("\n#define CHAIN_LEN %ld\n", ctr);




chain_exit:
return retcode;

chain_exit_error:
retcode = -1;
goto chain_exit;
}
