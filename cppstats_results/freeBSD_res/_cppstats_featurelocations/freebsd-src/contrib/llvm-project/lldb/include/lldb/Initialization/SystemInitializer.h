







#if !defined(LLDB_INITIALIZATION_SYSTEMINITIALIZER_H)
#define LLDB_INITIALIZATION_SYSTEMINITIALIZER_H

#include "llvm/Support/Error.h"

#include <string>

namespace lldb_private {

class SystemInitializer {
public:
SystemInitializer();
virtual ~SystemInitializer();

virtual llvm::Error Initialize() = 0;
virtual void Terminate() = 0;
};
}

#endif
