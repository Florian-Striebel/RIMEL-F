










































#include "../shell.h"
#include "../mystring.h"
#if defined(SHELL)
#include "../error.h"
#include "../output.h"
#include "builtins.h"
#define FILE struct output
#undef stdout
#define stdout out1
#undef stderr
#define stderr out2
#define printf out1fmt
#undef putc
#define putc(c, file) outc(c, file)
#undef putchar
#define putchar(c) out1c(c)
#define fprintf outfmt
#define fputs outstr
#define fwrite(ptr, size, nmemb, file) outbin(ptr, (size) * (nmemb), file)
#define fflush flushout
#define INITARGS(argv)
#define warnx warning
#define warn(fmt, ...) warning(fmt ": %s", __VA_ARGS__, strerror(errno))
#define errx(exitstatus, ...) error(__VA_ARGS__)

#else
#undef NULL
#include <stdio.h>
#undef main
#define INITARGS(argv) if ((commandname = argv[0]) == NULL) {fputs("Argc is zero\n", stderr); exit(2);} else
#endif

#include <unistd.h>

pointer stalloc(int);
int killjob(const char *, int);

extern char *commandname;
