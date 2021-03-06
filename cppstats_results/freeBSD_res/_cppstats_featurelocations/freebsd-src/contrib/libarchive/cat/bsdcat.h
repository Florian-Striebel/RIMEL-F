
























#if !defined(BSDCAT_H_INCLUDED)
#define BSDCAT_H_INCLUDED

#if defined(PLATFORM_CONFIG_H)

#include PLATFORM_CONFIG_H
#else

#include "config.h"
#endif

#include <archive.h>
#include <archive_entry.h>

struct bsdcat {

int getopt_state;
char *getopt_word;


int argc;
char **argv;
const char *argument;
};

enum {
OPTION_VERSION
};

int bsdcat_getopt(struct bsdcat *);
void usage(FILE *stream, int eval);
void bsdcat_next(void);
void bsdcat_print_error(void);
void bsdcat_read_to_stdout(const char* filename);

#endif
