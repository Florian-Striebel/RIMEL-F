







#if !defined(LLD_CORE_PASS_MANAGER_H)
#define LLD_CORE_PASS_MANAGER_H

#include "lld/Common/LLVM.h"
#include "lld/Core/Pass.h"
#include "llvm/Support/Error.h"
#include <memory>
#include <vector>

namespace lld {
class SimpleFile;
class Pass;







class PassManager {
public:
void add(std::unique_ptr<Pass> pass) {
_passes.push_back(std::move(pass));
}

llvm::Error runOnFile(SimpleFile &file) {
for (std::unique_ptr<Pass> &pass : _passes)
if (llvm::Error EC = pass->perform(file))
return EC;
return llvm::Error::success();
}

private:

std::vector<std::unique_ptr<Pass>> _passes;
};
}

#endif
