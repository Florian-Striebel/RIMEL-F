








#if !defined(LLVM_CLANG_C_REWRITE_H)
#define LLVM_CLANG_C_REWRITE_H

#include "clang-c/CXString.h"
#include "clang-c/ExternC.h"
#include "clang-c/Index.h"
#include "clang-c/Platform.h"

LLVM_CLANG_C_EXTERN_C_BEGIN

typedef void *CXRewriter;




CINDEX_LINKAGE CXRewriter clang_CXRewriter_create(CXTranslationUnit TU);




CINDEX_LINKAGE void clang_CXRewriter_insertTextBefore(CXRewriter Rew, CXSourceLocation Loc,
const char *Insert);





CINDEX_LINKAGE void clang_CXRewriter_replaceText(CXRewriter Rew, CXSourceRange ToBeReplaced,
const char *Replacement);




CINDEX_LINKAGE void clang_CXRewriter_removeText(CXRewriter Rew, CXSourceRange ToBeRemoved);





CINDEX_LINKAGE int clang_CXRewriter_overwriteChangedFiles(CXRewriter Rew);




CINDEX_LINKAGE void clang_CXRewriter_writeMainFileToStdOut(CXRewriter Rew);




CINDEX_LINKAGE void clang_CXRewriter_dispose(CXRewriter Rew);

LLVM_CLANG_C_EXTERN_C_END

#endif
