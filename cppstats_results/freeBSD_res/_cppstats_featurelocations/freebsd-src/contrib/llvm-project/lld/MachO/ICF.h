







#if !defined(LLD_MACHO_ICF_H)
#define LLD_MACHO_ICF_H

#include "lld/Common/LLVM.h"
#include <vector>

namespace lld {
namespace macho {

void foldIdenticalSections();

}
}

#endif
