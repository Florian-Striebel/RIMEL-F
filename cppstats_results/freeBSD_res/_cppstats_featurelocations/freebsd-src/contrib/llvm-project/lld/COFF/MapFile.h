







#if !defined(LLD_COFF_MAPFILE_H)
#define LLD_COFF_MAPFILE_H

#include "llvm/ADT/ArrayRef.h"

namespace lld {
namespace coff {
class OutputSection;
void writeMapFile(llvm::ArrayRef<OutputSection *> outputSections);
}
}

#endif
