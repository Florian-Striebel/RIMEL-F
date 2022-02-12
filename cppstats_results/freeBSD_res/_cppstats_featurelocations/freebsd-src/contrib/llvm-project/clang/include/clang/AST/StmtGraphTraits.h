












#if !defined(LLVM_CLANG_AST_STMTGRAPHTRAITS_H)
#define LLVM_CLANG_AST_STMTGRAPHTRAITS_H

#include "clang/AST/Stmt.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/GraphTraits.h"

namespace llvm {

template <> struct GraphTraits<clang::Stmt *> {
using NodeRef = clang::Stmt *;
using ChildIteratorType = clang::Stmt::child_iterator;
using nodes_iterator = llvm::df_iterator<clang::Stmt *>;

static NodeRef getEntryNode(clang::Stmt *S) { return S; }

static ChildIteratorType child_begin(NodeRef N) {
if (N) return N->child_begin();
else return ChildIteratorType();
}

static ChildIteratorType child_end(NodeRef N) {
if (N) return N->child_end();
else return ChildIteratorType();
}

static nodes_iterator nodes_begin(clang::Stmt* S) {
return df_begin(S);
}

static nodes_iterator nodes_end(clang::Stmt* S) {
return df_end(S);
}
};

template <> struct GraphTraits<const clang::Stmt *> {
using NodeRef = const clang::Stmt *;
using ChildIteratorType = clang::Stmt::const_child_iterator;
using nodes_iterator = llvm::df_iterator<const clang::Stmt *>;

static NodeRef getEntryNode(const clang::Stmt *S) { return S; }

static ChildIteratorType child_begin(NodeRef N) {
if (N) return N->child_begin();
else return ChildIteratorType();
}

static ChildIteratorType child_end(NodeRef N) {
if (N) return N->child_end();
else return ChildIteratorType();
}

static nodes_iterator nodes_begin(const clang::Stmt* S) {
return df_begin(S);
}

static nodes_iterator nodes_end(const clang::Stmt* S) {
return df_end(S);
}
};

}

#endif
