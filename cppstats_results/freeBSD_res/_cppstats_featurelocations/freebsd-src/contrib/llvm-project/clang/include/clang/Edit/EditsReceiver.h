







#if !defined(LLVM_CLANG_EDIT_EDITSRECEIVER_H)
#define LLVM_CLANG_EDIT_EDITSRECEIVER_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
namespace edit {

class EditsReceiver {
public:
virtual ~EditsReceiver() = default;

virtual void insert(SourceLocation loc, StringRef text) = 0;
virtual void replace(CharSourceRange range, StringRef text) = 0;


virtual void remove(CharSourceRange range);
};

}
}

#endif
