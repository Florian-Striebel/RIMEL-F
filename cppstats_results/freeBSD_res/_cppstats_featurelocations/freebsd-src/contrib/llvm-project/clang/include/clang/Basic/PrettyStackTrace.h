














#if !defined(LLVM_CLANG_BASIC_PRETTYSTACKTRACE_H)
#define LLVM_CLANG_BASIC_PRETTYSTACKTRACE_H

#include "clang/Basic/SourceLocation.h"
#include "llvm/Support/PrettyStackTrace.h"

namespace clang {



class PrettyStackTraceLoc : public llvm::PrettyStackTraceEntry {
SourceManager &SM;
SourceLocation Loc;
const char *Message;
public:
PrettyStackTraceLoc(SourceManager &sm, SourceLocation L, const char *Msg)
: SM(sm), Loc(L), Message(Msg) {}
void print(raw_ostream &OS) const override;
};
}

#endif
