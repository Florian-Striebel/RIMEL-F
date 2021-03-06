












#if !defined(LLVM_CLANG_AST_MANGLENUMBERINGCONTEXT_H)
#define LLVM_CLANG_AST_MANGLENUMBERINGCONTEXT_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

namespace clang {

class BlockDecl;
class CXXMethodDecl;
class IdentifierInfo;
class TagDecl;
class Type;
class VarDecl;



class MangleNumberingContext {
public:
virtual ~MangleNumberingContext() {}



virtual unsigned getManglingNumber(const CXXMethodDecl *CallOperator) = 0;



virtual unsigned getManglingNumber(const BlockDecl *BD) = 0;


virtual unsigned getStaticLocalNumber(const VarDecl *VD) = 0;



virtual unsigned getManglingNumber(const VarDecl *VD,
unsigned MSLocalManglingNumber) = 0;



virtual unsigned getManglingNumber(const TagDecl *TD,
unsigned MSLocalManglingNumber) = 0;




virtual unsigned getDeviceManglingNumber(const CXXMethodDecl *) { return 0; }
};

}
#endif
