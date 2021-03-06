








#if !defined(LLDB_EXPRESSION_EXPRESSIONTYPESYSTEMHELPER_H)
#define LLDB_EXPRESSION_EXPRESSIONTYPESYSTEMHELPER_H

#include "llvm/Support/Casting.h"

namespace lldb_private {










class ExpressionTypeSystemHelper {
public:
enum LLVMCastKind {
eKindClangHelper,
eKindSwiftHelper,
eKindGoHelper,
kNumKinds
};

LLVMCastKind getKind() const { return m_kind; }

ExpressionTypeSystemHelper(LLVMCastKind kind) : m_kind(kind) {}

~ExpressionTypeSystemHelper() = default;

protected:
LLVMCastKind m_kind;
};

}

#endif
