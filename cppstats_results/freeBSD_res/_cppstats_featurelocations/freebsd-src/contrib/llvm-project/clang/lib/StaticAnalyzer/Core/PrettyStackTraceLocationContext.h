







#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CORE_PRETTYSTACKTRACELOCATIONCONTEXT_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CORE_PRETTYSTACKTRACELOCATIONCONTEXT_H

#include "clang/Analysis/AnalysisDeclContext.h"

namespace clang {
namespace ento {












class PrettyStackTraceLocationContext : public llvm::PrettyStackTraceEntry {
const LocationContext *LCtx;
public:
PrettyStackTraceLocationContext(const LocationContext *LC) : LCtx(LC) {
assert(LCtx);
}

void print(raw_ostream &Out) const override {
Out << "While analyzing stack: \n";
LCtx->dumpStack(Out);
}
};

}
}

#endif
