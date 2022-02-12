







#if !defined(LLD_ELF_CALL_GRAPH_SORT_H)
#define LLD_ELF_CALL_GRAPH_SORT_H

#include "llvm/ADT/DenseMap.h"

namespace lld {
namespace elf {
class InputSectionBase;

llvm::DenseMap<const InputSectionBase *, int> computeCallGraphProfileOrder();
}
}

#endif
