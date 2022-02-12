







#if !defined(LLD_COFF_ICF_H)
#define LLD_COFF_ICF_H

#include "Config.h"
#include "lld/Common/LLVM.h"
#include "llvm/ADT/ArrayRef.h"

namespace lld {
namespace coff {

class Chunk;

void doICF(ArrayRef<Chunk *> chunks, ICFLevel);

}
}

#endif
