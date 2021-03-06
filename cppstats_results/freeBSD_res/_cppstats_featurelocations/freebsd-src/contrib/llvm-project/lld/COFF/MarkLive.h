







#if !defined(LLD_COFF_MARKLIVE_H)
#define LLD_COFF_MARKLIVE_H

#include "lld/Common/LLVM.h"
#include "llvm/ADT/ArrayRef.h"

namespace lld {
namespace coff {

class Chunk;

void markLive(ArrayRef<Chunk *> chunks);

}
}

#endif
