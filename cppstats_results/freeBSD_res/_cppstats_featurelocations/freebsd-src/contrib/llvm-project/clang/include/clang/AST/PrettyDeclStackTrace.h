













#if !defined(LLVM_CLANG_AST_PRETTYDECLSTACKTRACE_H)
#define LLVM_CLANG_AST_PRETTYDECLSTACKTRACE_H

#include "clang/Basic/SourceLocation.h"
#include "llvm/Support/PrettyStackTrace.h"

namespace clang {

class ASTContext;
class Decl;
class SourceManager;




class PrettyDeclStackTraceEntry : public llvm::PrettyStackTraceEntry {
ASTContext &Context;
Decl *TheDecl;
SourceLocation Loc;
const char *Message;

public:
PrettyDeclStackTraceEntry(ASTContext &Ctx, Decl *D, SourceLocation Loc,
const char *Msg)
: Context(Ctx), TheDecl(D), Loc(Loc), Message(Msg) {}

void print(raw_ostream &OS) const override;
};

}

#endif
