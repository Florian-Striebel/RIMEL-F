







#if !defined(LLVM_CLANG_DRIVER_OPTIONS_H)
#define LLVM_CLANG_DRIVER_OPTIONS_H

namespace llvm {
namespace opt {
class OptTable;
}
}

namespace clang {
namespace driver {

namespace options {


enum ClangFlags {
NoXarchOption = (1 << 4),
LinkerInput = (1 << 5),
NoArgumentUnused = (1 << 6),
Unsupported = (1 << 7),
CoreOption = (1 << 8),
CLOption = (1 << 9),
CC1Option = (1 << 10),
CC1AsOption = (1 << 11),
NoDriverOption = (1 << 12),
LinkOption = (1 << 13),
FlangOption = (1 << 14),
FC1Option = (1 << 15),
FlangOnlyOption = (1 << 16),
Ignored = (1 << 17),
};

enum ID {
OPT_INVALID = 0,
#define OPTION(PREFIX, NAME, ID, KIND, GROUP, ALIAS, ALIASARGS, FLAGS, PARAM, HELPTEXT, METAVAR, VALUES) OPT_##ID,


#include "clang/Driver/Options.inc"
LastOption
#undef OPTION
};
}

const llvm::opt::OptTable &getDriverOptTable();
}
}

#endif
