




























#define KNOWNREF "test_patterns_2.tar.uu"
#define ENVBASE "BSDTAR"
#define PROGRAM "bsdtar"
#define PROGRAM_ALIAS "tar"
#undef LIBRARY
#undef EXTRA_DUMP
#undef EXTRA_ERRNO

#define EXTRA_VERSION (systemf("%s --version", testprog) ? "" : "")

#include "test_common.h"
