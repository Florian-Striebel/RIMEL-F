







#if !defined(LLD_CORE_PASS_H)
#define LLD_CORE_PASS_H

#include "llvm/Support/Error.h"

namespace lld {

class SimpleFile;











class Pass {
public:
virtual ~Pass() = default;


virtual llvm::Error perform(SimpleFile &mergedFile) = 0;

protected:

Pass() = default;
};

}

#endif
