

















#if !defined(LLVM_CLANG_TOOLING_ASTDIFF_ASTDIFF_H)
#define LLVM_CLANG_TOOLING_ASTDIFF_ASTDIFF_H

#include "clang/Tooling/ASTDiff/ASTDiffInternal.h"

namespace clang {
namespace diff {

enum ChangeKind {
None,
Delete,
Update,
Insert,
Move,
UpdateMove
};


struct Node {
NodeId Parent, LeftMostDescendant, RightMostDescendant;
int Depth, Height, Shift = 0;
DynTypedNode ASTNode;
SmallVector<NodeId, 4> Children;
ChangeKind Change = None;

ASTNodeKind getType() const;
StringRef getTypeLabel() const;
bool isLeaf() const { return Children.empty(); }
llvm::Optional<StringRef> getIdentifier() const;
llvm::Optional<std::string> getQualifiedIdentifier() const;
};

class ASTDiff {
public:
ASTDiff(SyntaxTree &Src, SyntaxTree &Dst, const ComparisonOptions &Options);
~ASTDiff();


NodeId getMapped(const SyntaxTree &SourceTree, NodeId Id) const;

class Impl;

private:
std::unique_ptr<Impl> DiffImpl;
};



class SyntaxTree {
public:

SyntaxTree(ASTContext &AST);

template <class T>
SyntaxTree(T *Node, ASTContext &AST)
: TreeImpl(std::make_unique<Impl>(this, Node, AST)) {}
SyntaxTree(SyntaxTree &&Other) = default;
~SyntaxTree();

const ASTContext &getASTContext() const;
StringRef getFilename() const;

int getSize() const;
NodeId getRootId() const;
using PreorderIterator = NodeId;
PreorderIterator begin() const;
PreorderIterator end() const;

const Node &getNode(NodeId Id) const;
int findPositionInParent(NodeId Id) const;


std::pair<unsigned, unsigned> getSourceRangeOffsets(const Node &N) const;




std::string getNodeValue(NodeId Id) const;
std::string getNodeValue(const Node &Node) const;

class Impl;
std::unique_ptr<Impl> TreeImpl;
};

struct ComparisonOptions {

int MinHeight = 2;



double MinSimilarity = 0.5;



int MaxSize = 100;

bool StopAfterTopDown = false;


bool isMatchingAllowed(const Node &N1, const Node &N2) const {
return N1.getType().isSame(N2.getType());
}
};

}
}

#endif
