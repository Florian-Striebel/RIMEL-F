












#if !defined(LLVM_CLANG_AST_ASTCONTEXTALLOCATE_H)
#define LLVM_CLANG_AST_ASTCONTEXTALLOCATE_H

#include <cstddef>

namespace clang {

class ASTContext;

}


void *operator new(size_t Bytes, const clang::ASTContext &C,
size_t Alignment = 8);
void *operator new[](size_t Bytes, const clang::ASTContext &C,
size_t Alignment = 8);




void operator delete(void *Ptr, const clang::ASTContext &C, size_t);
void operator delete[](void *Ptr, const clang::ASTContext &C, size_t);

#endif
