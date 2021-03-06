







#if !defined(LLD_COFF_LLDMAPFILE_H)
#define LLD_COFF_LLDMAPFILE_H

#include "llvm/ADT/ArrayRef.h"

namespace lld {
namespace coff {
class OutputSection;
void writeLLDMapFile(llvm::ArrayRef<OutputSection *> outputSections);
}
}

#endif
