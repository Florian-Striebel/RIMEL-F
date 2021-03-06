








#if !defined(LLVM_CLANG_TOOLING_SYNTAX_TREE_H)
#define LLVM_CLANG_TOOLING_SYNTAX_TREE_H

#include "clang/AST/Decl.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Tooling/Syntax/Nodes.h"
#include "clang/Tooling/Syntax/Tree.h"

namespace clang {
namespace syntax {




syntax::TranslationUnit *buildSyntaxTree(Arena &A, ASTContext &Context);






syntax::Leaf *createLeaf(syntax::Arena &A, tok::TokenKind K,
StringRef Spelling);



syntax::Leaf *createLeaf(syntax::Arena &A, tok::TokenKind K);




syntax::Tree *
createTree(syntax::Arena &A,
ArrayRef<std::pair<syntax::Node *, syntax::NodeRole>> Children,
syntax::NodeKind K);


syntax::EmptyStatement *createEmptyStatement(syntax::Arena &A);







syntax::Node *deepCopyExpandingMacros(syntax::Arena &A, const syntax::Node *N);
}
}
#endif
