




























#define KNOWNREF "test_expand.Z.uu"
#define ENVBASE "BSDCAT"
#define PROGRAM "bsdcat"
#define PROGRAM_ALIAS "cat"
#undef LIBRARY
#undef EXTRA_DUMP
#undef EXTRA_ERRNO

#define EXTRA_VERSION (systemf("%s --version", testprog) ? "" : "")

#include "test_common.h"
