












#if !defined(LLVM_CLANG_AST_SELECTORLOCATIONSKIND_H)
#define LLVM_CLANG_AST_SELECTORLOCATIONSKIND_H

#include "clang/Basic/LLVM.h"

namespace clang {
class Selector;
class SourceLocation;
class Expr;
class ParmVarDecl;



enum SelectorLocationsKind {

SelLoc_NonStandard = 0,





SelLoc_StandardNoSpace = 1,





SelLoc_StandardWithSpace = 2
};


SelectorLocationsKind hasStandardSelectorLocs(Selector Sel,
ArrayRef<SourceLocation> SelLocs,
ArrayRef<Expr *> Args,
SourceLocation EndLoc);







SourceLocation getStandardSelectorLoc(unsigned Index,
Selector Sel,
bool WithArgSpace,
ArrayRef<Expr *> Args,
SourceLocation EndLoc);


SelectorLocationsKind hasStandardSelectorLocs(Selector Sel,
ArrayRef<SourceLocation> SelLocs,
ArrayRef<ParmVarDecl *> Args,
SourceLocation EndLoc);







SourceLocation getStandardSelectorLoc(unsigned Index,
Selector Sel,
bool WithArgSpace,
ArrayRef<ParmVarDecl *> Args,
SourceLocation EndLoc);

}

#endif
