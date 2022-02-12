












#if !defined(LLVM_CLANG_AST_PARENTMAPCONTEXT_H)
#define LLVM_CLANG_AST_PARENTMAPCONTEXT_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTTypeTraits.h"

namespace clang {
class DynTypedNodeList;

class ParentMapContext {
public:
ParentMapContext(ASTContext &Ctx);

~ParentMapContext();

























template <typename NodeT> DynTypedNodeList getParents(const NodeT &Node);

DynTypedNodeList getParents(const DynTypedNode &Node);


void clear();

TraversalKind getTraversalKind() const { return Traversal; }
void setTraversalKind(TraversalKind TK) { Traversal = TK; }

const Expr *traverseIgnored(const Expr *E) const;
Expr *traverseIgnored(Expr *E) const;
DynTypedNode traverseIgnored(const DynTypedNode &N) const;

class ParentMap;

private:
ASTContext &ASTCtx;
TraversalKind Traversal = TK_AsIs;
std::unique_ptr<ParentMap> Parents;
};

class TraversalKindScope {
ParentMapContext &Ctx;
TraversalKind TK = TK_AsIs;

public:
TraversalKindScope(ASTContext &ASTCtx, llvm::Optional<TraversalKind> ScopeTK)
: Ctx(ASTCtx.getParentMapContext()) {
TK = Ctx.getTraversalKind();
if (ScopeTK)
Ctx.setTraversalKind(*ScopeTK);
}

~TraversalKindScope() { Ctx.setTraversalKind(TK); }
};



class DynTypedNodeList {
llvm::AlignedCharArrayUnion<DynTypedNode, ArrayRef<DynTypedNode>> Storage;
bool IsSingleNode;

public:
DynTypedNodeList(const DynTypedNode &N) : IsSingleNode(true) {
new (&Storage) DynTypedNode(N);
}

DynTypedNodeList(ArrayRef<DynTypedNode> A) : IsSingleNode(false) {
new (&Storage) ArrayRef<DynTypedNode>(A);
}

const DynTypedNode *begin() const {
if (!IsSingleNode)
return reinterpret_cast<const ArrayRef<DynTypedNode> *>(&Storage)
->begin();
return reinterpret_cast<const DynTypedNode *>(&Storage);
}

const DynTypedNode *end() const {
if (!IsSingleNode)
return reinterpret_cast<const ArrayRef<DynTypedNode> *>(&Storage)->end();
return reinterpret_cast<const DynTypedNode *>(&Storage) + 1;
}

size_t size() const { return end() - begin(); }
bool empty() const { return begin() == end(); }

const DynTypedNode &operator[](size_t N) const {
assert(N < size() && "Out of bounds!");
return *(begin() + N);
}
};

template <typename NodeT>
inline DynTypedNodeList ParentMapContext::getParents(const NodeT &Node) {
return getParents(DynTypedNode::create(Node));
}

template <typename NodeT>
inline DynTypedNodeList ASTContext::getParents(const NodeT &Node) {
return getParentMapContext().getParents(Node);
}

template <>
inline DynTypedNodeList ASTContext::getParents(const DynTypedNode &Node) {
return getParentMapContext().getParents(Node);
}

}

#endif
