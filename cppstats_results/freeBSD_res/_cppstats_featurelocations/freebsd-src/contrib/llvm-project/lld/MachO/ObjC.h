







#if !defined(LLD_MACHO_OBJC_H)
#define LLD_MACHO_OBJC_H

#include "llvm/Support/MemoryBuffer.h"

namespace lld {
namespace macho {

namespace objc {

constexpr const char klass[] = "_OBJC_CLASS_$_";
constexpr const char metaclass[] = "_OBJC_METACLASS_$_";
constexpr const char ehtype[] = "_OBJC_EHTYPE_$_";
constexpr const char ivar[] = "_OBJC_IVAR_$_";

}

bool hasObjCSection(llvm::MemoryBufferRef);

}
}

#endif
