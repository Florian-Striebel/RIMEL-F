












#if !defined(LLVM_CLANG_FRONTENDTOOL_UTILS_H)
#define LLVM_CLANG_FRONTENDTOOL_UTILS_H

#include <memory>

namespace clang {

class CompilerInstance;
class FrontendAction;





std::unique_ptr<FrontendAction> CreateFrontendAction(CompilerInstance &CI);





bool ExecuteCompilerInvocation(CompilerInstance *Clang);

}

#endif
