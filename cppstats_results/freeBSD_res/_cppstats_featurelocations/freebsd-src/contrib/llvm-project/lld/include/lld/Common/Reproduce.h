







#if !defined(LLD_COMMON_REPRODUCE_H)
#define LLD_COMMON_REPRODUCE_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"

namespace llvm {
namespace opt { class Arg; }
}

namespace lld {




std::string relativeToRoot(StringRef path);


std::string quote(StringRef s);


std::string toString(const llvm::opt::Arg &arg);
}

#endif
