







#if !defined(LLD_COMMON_DRIVER_H)
#define LLD_COMMON_DRIVER_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/raw_ostream.h"

namespace lld {
struct SafeReturn {
int ret;
bool canRunAgain;
};







SafeReturn safeLldMain(int argc, const char **argv, llvm::raw_ostream &stdoutOS,
llvm::raw_ostream &stderrOS);

namespace coff {
bool link(llvm::ArrayRef<const char *> args, bool canExitEarly,
llvm::raw_ostream &stdoutOS, llvm::raw_ostream &stderrOS);
}

namespace mingw {
bool link(llvm::ArrayRef<const char *> args, bool canExitEarly,
llvm::raw_ostream &stdoutOS, llvm::raw_ostream &stderrOS);
}

namespace elf {
bool link(llvm::ArrayRef<const char *> args, bool canExitEarly,
llvm::raw_ostream &stdoutOS, llvm::raw_ostream &stderrOS);
}

namespace mach_o {
bool link(llvm::ArrayRef<const char *> args, bool canExitEarly,
llvm::raw_ostream &stdoutOS, llvm::raw_ostream &stderrOS);
}

namespace macho {
bool link(llvm::ArrayRef<const char *> args, bool canExitEarly,
llvm::raw_ostream &stdoutOS, llvm::raw_ostream &stderrOS);
}

namespace wasm {
bool link(llvm::ArrayRef<const char *> args, bool canExitEarly,
llvm::raw_ostream &stdoutOS, llvm::raw_ostream &stderrOS);
}
}

#endif
