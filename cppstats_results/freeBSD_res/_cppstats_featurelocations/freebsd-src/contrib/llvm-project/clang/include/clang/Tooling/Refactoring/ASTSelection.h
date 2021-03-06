







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_AST_SELECTION_H)
#define LLVM_CLANG_TOOLING_REFACTOR_AST_SELECTION_H

#include "clang/AST/ASTTypeTraits.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

namespace clang {

class ASTContext;

namespace tooling {

enum class SourceSelectionKind {

None,



ContainsSelection,


ContainsSelectionStart,


ContainsSelectionEnd,



InsideSelection,
};








struct SelectedASTNode {
DynTypedNode Node;
SourceSelectionKind SelectionKind;
std::vector<SelectedASTNode> Children;

SelectedASTNode(const DynTypedNode &Node, SourceSelectionKind SelectionKind)
: Node(Node), SelectionKind(SelectionKind) {}
SelectedASTNode(SelectedASTNode &&) = default;
SelectedASTNode &operator=(SelectedASTNode &&) = default;

void dump(llvm::raw_ostream &OS = llvm::errs()) const;

using ReferenceType = std::reference_wrapper<const SelectedASTNode>;
};





Optional<SelectedASTNode> findSelectedASTNodes(const ASTContext &Context,
SourceRange SelectionRange);























class CodeRangeASTSelection {
public:
CodeRangeASTSelection(CodeRangeASTSelection &&) = default;
CodeRangeASTSelection &operator=(CodeRangeASTSelection &&) = default;


ArrayRef<SelectedASTNode::ReferenceType> getParents() { return Parents; }


size_t size() const {
if (!AreChildrenSelected)
return 1;
return SelectedNode.get().Children.size();
}

const Stmt *operator[](size_t I) const {
if (!AreChildrenSelected) {
assert(I == 0 && "Invalid index");
return SelectedNode.get().Node.get<Stmt>();
}
return SelectedNode.get().Children[I].Node.get<Stmt>();
}










bool isInFunctionLikeBodyOfCode() const;



const Decl *getFunctionLikeNearestParent() const;

static Optional<CodeRangeASTSelection>
create(SourceRange SelectionRange, const SelectedASTNode &ASTSelection);

private:
CodeRangeASTSelection(SelectedASTNode::ReferenceType SelectedNode,
ArrayRef<SelectedASTNode::ReferenceType> Parents,
bool AreChildrenSelected)
: SelectedNode(SelectedNode), Parents(Parents.begin(), Parents.end()),
AreChildrenSelected(AreChildrenSelected) {}



SelectedASTNode::ReferenceType SelectedNode;

llvm::SmallVector<SelectedASTNode::ReferenceType, 8> Parents;

bool AreChildrenSelected;
};

}
}

#endif
