












#if !defined(LLVM_CLANG_ANALYSIS_DOMAINSPECIFIC_OBJCNORETURN_H)
#define LLVM_CLANG_ANALYSIS_DOMAINSPECIFIC_OBJCNORETURN_H

#include "clang/Basic/IdentifierTable.h"

namespace clang {

class ASTContext;
class ObjCMessageExpr;

class ObjCNoReturn {

Selector RaiseSel;


IdentifierInfo *NSExceptionII;

enum { NUM_RAISE_SELECTORS = 2 };


Selector NSExceptionInstanceRaiseSelectors[NUM_RAISE_SELECTORS];

public:
ObjCNoReturn(ASTContext &C);



bool isImplicitNoReturn(const ObjCMessageExpr *ME);
};
}

#endif
