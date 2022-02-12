#pragma ident "%Z%%M% %I% %E% SMI"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>
#include <assert.h>
#include "ctftools.h"
#include "memory.h"
const char *progname;
int debug_level = DEBUG_LEVEL;
static char *infile = NULL;
static const char *outfile = NULL;
static int dynsym;
static void
usage(void)
{
(void) fprintf(stderr,
"Usage: %s [-gis] -l label | -L labelenv [-o outfile] object_file\n"
"\n"
" Note: if -L labelenv is specified and labelenv is not set in\n"
" the environment, a default value is used.\n",
progname);
}
static void
terminate_cleanup(void)
{
#if !defined(__FreeBSD__)
if (!outfile) {
fprintf(stderr, "Removing %s\n", infile);
unlink(infile);
}
#endif
}
static void
handle_sig(int sig)
{
terminate("Caught signal %d - exiting\n", sig);
}
static int
file_read(tdata_t *td, char *filename, int ignore_non_c)
{
typedef int (*reader_f)(tdata_t *, Elf *, char *);
static reader_f readers[] = {
stabs_read,
dw_read,
NULL
};
source_types_t source_types;
Elf *elf;
int i, rc, fd;
if ((fd = open(filename, O_RDONLY)) < 0)
terminate("failed to open %s", filename);
(void) elf_version(EV_CURRENT);
if ((elf = elf_begin(fd, ELF_C_READ, NULL)) == NULL) {
close(fd);
terminate("failed to read %s: %s\n", filename,
elf_errmsg(-1));
}
source_types = built_source_types(elf, filename);
if ((source_types == SOURCE_NONE || (source_types & SOURCE_UNKNOWN)) &&
ignore_non_c) {
debug(1, "Ignoring file %s from unknown sources\n", filename);
exit(0);
}
for (i = 0; readers[i] != NULL; i++) {
if ((rc = readers[i](td, elf, filename)) == 0)
break;
assert(rc < 0 && errno == ENOENT);
}
if (readers[i] == NULL) {
if (findelfsecidx(elf, filename, ".debug") >= 0) {
terminate("%s: DWARF version 1 is not supported\n",
filename);
}
if (!(source_types & SOURCE_C) && ignore_non_c) {
debug(1, "Ignoring file %s not built from C sources\n",
filename);
exit(0);
}
rc = 0;
} else {
rc = 1;
}
(void) elf_end(elf);
(void) close(fd);
return (rc);
}
int
main(int argc, char **argv)
{
tdata_t *filetd, *mstrtd;
const char *label = NULL;
int verbose = 0;
int ignore_non_c = 0;
int keep_stabs = 0;
int c;
#if defined(illumos)
sighold(SIGINT);
sighold(SIGQUIT);
sighold(SIGTERM);
#endif
progname = basename(argv[0]);
if (getenv("CTFCONVERT_DEBUG_LEVEL"))
debug_level = atoi(getenv("CTFCONVERT_DEBUG_LEVEL"));
if (getenv("CTFCONVERT_DEBUG_PARSE"))
debug_parse = atoi(getenv("CTFCONVERT_DEBUG_PARSE"));
while ((c = getopt(argc, argv, ":l:L:o:givs")) != EOF) {
switch (c) {
case 'l':
label = optarg;
break;
case 'L':
if ((label = getenv(optarg)) == NULL)
label = CTF_DEFAULT_LABEL;
break;
case 'o':
outfile = optarg;
break;
case 's':
dynsym = CTF_USE_DYNSYM;
break;
case 'i':
ignore_non_c = 1;
break;
case 'g':
keep_stabs = CTF_KEEP_STABS;
break;
case 'v':
verbose = 1;
break;
default:
usage();
exit(2);
}
}
if (getenv("STRIPSTABS_KEEP_STABS") != NULL)
keep_stabs = CTF_KEEP_STABS;
if (argc - optind != 1 || label == NULL) {
usage();
exit(2);
}
infile = argv[optind];
if (access(infile, R_OK) != 0)
terminate("Can't access %s", infile);
set_terminate_cleanup(terminate_cleanup);
#if defined(illumos)
sigset(SIGINT, handle_sig);
sigset(SIGQUIT, handle_sig);
sigset(SIGTERM, handle_sig);
#else
signal(SIGINT, handle_sig);
signal(SIGQUIT, handle_sig);
signal(SIGTERM, handle_sig);
#endif
filetd = tdata_new();
if (!file_read(filetd, infile, ignore_non_c))
terminate("%s doesn't have type data to convert\n", infile);
if (verbose)
iidesc_stats(filetd->td_iihash);
mstrtd = tdata_new();
merge_into_master(filetd, mstrtd, NULL, 1);
tdata_label_add(mstrtd, label, CTF_LABEL_LASTIDX);
if (outfile && strcmp(infile, outfile) != 0) {
write_ctf(mstrtd, infile, outfile, dynsym | keep_stabs);
} else {
char *tmpname = mktmpname(infile, ".ctf");
write_ctf(mstrtd, infile, tmpname, dynsym | keep_stabs);
if (rename(tmpname, infile) != 0)
terminate("Couldn't rename temp file %s", tmpname);
free(tmpname);
}
return (0);
}
