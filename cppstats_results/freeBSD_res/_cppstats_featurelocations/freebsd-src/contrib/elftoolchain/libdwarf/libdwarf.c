

























#include "_libdwarf.h"

ELFTC_VCSID("$Id: libdwarf.c 3161 2015-02-15 21:43:36Z emaste $");

struct _libdwarf_globals _libdwarf = {
.errhand = NULL,
.errarg = NULL,
.applyreloc = 1
};
