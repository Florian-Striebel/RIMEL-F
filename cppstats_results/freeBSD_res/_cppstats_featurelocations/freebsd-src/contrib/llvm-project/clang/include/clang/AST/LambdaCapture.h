












#if !defined(LLVM_CLANG_AST_LAMBDACAPTURE_H)
#define LLVM_CLANG_AST_LAMBDACAPTURE_H

#include "clang/AST/Decl.h"
#include "clang/Basic/Lambda.h"
#include "llvm/ADT/PointerIntPair.h"

namespace clang {



class LambdaCapture {
enum {


Capture_Implicit = 0x01,





Capture_ByCopy = 0x02,



Capture_This = 0x04
};








llvm::PointerIntPair<Decl*, 3> DeclAndBits;

SourceLocation Loc;
SourceLocation EllipsisLoc;

friend class ASTStmtReader;
friend class ASTStmtWriter;

public:















LambdaCapture(SourceLocation Loc, bool Implicit, LambdaCaptureKind Kind,
VarDecl *Var = nullptr,
SourceLocation EllipsisLoc = SourceLocation());


LambdaCaptureKind getCaptureKind() const;



bool capturesThis() const {
return DeclAndBits.getPointer() == nullptr &&
(DeclAndBits.getInt() & Capture_This);
}


bool capturesVariable() const {
return dyn_cast_or_null<VarDecl>(DeclAndBits.getPointer());
}



bool capturesVLAType() const {
return DeclAndBits.getPointer() == nullptr &&
!(DeclAndBits.getInt() & Capture_This);
}






VarDecl *getCapturedVar() const {
assert(capturesVariable() && "No variable available for capture");
return static_cast<VarDecl *>(DeclAndBits.getPointer());
}



bool isImplicit() const {
return DeclAndBits.getInt() & Capture_Implicit;
}



bool isExplicit() const { return !isImplicit(); }







SourceLocation getLocation() const { return Loc; }



bool isPackExpansion() const { return EllipsisLoc.isValid(); }



SourceLocation getEllipsisLoc() const {
assert(isPackExpansion() && "No ellipsis location for a non-expansion");
return EllipsisLoc;
}
};

}

#endif
