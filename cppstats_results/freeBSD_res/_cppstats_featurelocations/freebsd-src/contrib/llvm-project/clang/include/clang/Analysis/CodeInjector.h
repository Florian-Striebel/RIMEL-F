














#if !defined(LLVM_CLANG_ANALYSIS_CODEINJECTOR_H)
#define LLVM_CLANG_ANALYSIS_CODEINJECTOR_H

namespace clang {

class Stmt;
class FunctionDecl;
class ObjCMethodDecl;











class CodeInjector {
public:
CodeInjector();
virtual ~CodeInjector();

virtual Stmt *getBody(const FunctionDecl *D) = 0;
virtual Stmt *getBody(const ObjCMethodDecl *D) = 0;
};
}

#endif
