




























#define KNOWNREF "test_option_f.cpio.uu"
#define ENVBASE "BSDCPIO"
#define PROGRAM "bsdcpio"
#define PROGRAM_ALIAS "cpio"
#undef LIBRARY
#undef EXTRA_DUMP
#undef EXTRA_ERRNO

#define EXTRA_VERSION (systemf("%s --version", testprog) ? "" : "")

#include "test_common.h"
