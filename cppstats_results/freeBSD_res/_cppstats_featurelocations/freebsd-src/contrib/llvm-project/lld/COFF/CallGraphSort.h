







#if !defined(LLD_COFF_CALL_GRAPH_SORT_H)
#define LLD_COFF_CALL_GRAPH_SORT_H

#include "llvm/ADT/DenseMap.h"

namespace lld {
namespace coff {
class SectionChunk;

llvm::DenseMap<const SectionChunk *, int> computeCallGraphProfileOrder();
}
}

#endif
