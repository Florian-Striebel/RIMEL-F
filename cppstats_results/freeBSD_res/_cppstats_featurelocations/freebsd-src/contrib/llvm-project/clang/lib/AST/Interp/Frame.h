











#if !defined(LLVM_CLANG_AST_INTERP_FRAME_H)
#define LLVM_CLANG_AST_INTERP_FRAME_H

#include "clang/Basic/SourceLocation.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
class FunctionDecl;

namespace interp {


class Frame {
public:
virtual ~Frame();


virtual void describe(llvm::raw_ostream &OS) = 0;


virtual Frame *getCaller() const = 0;


virtual SourceLocation getCallLocation() const = 0;


virtual const FunctionDecl *getCallee() const = 0;
};

}
}

#endif
