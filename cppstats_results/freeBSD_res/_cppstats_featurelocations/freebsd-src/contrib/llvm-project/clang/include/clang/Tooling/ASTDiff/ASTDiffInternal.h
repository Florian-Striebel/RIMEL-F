








#if !defined(LLVM_CLANG_TOOLING_ASTDIFF_ASTDIFFINTERNAL_H)
#define LLVM_CLANG_TOOLING_ASTDIFF_ASTDIFFINTERNAL_H

#include "clang/AST/ASTTypeTraits.h"

namespace clang {
namespace diff {

using DynTypedNode = DynTypedNode;

class SyntaxTree;
class SyntaxTreeImpl;
struct ComparisonOptions;


struct NodeId {
private:
static constexpr int InvalidNodeId = -1;

public:
int Id;

NodeId() : Id(InvalidNodeId) {}
NodeId(int Id) : Id(Id) {}

operator int() const { return Id; }
NodeId &operator++() { return ++Id, *this; }
NodeId &operator--() { return --Id, *this; }

NodeId &operator*() { return *this; }

bool isValid() const { return Id != InvalidNodeId; }
bool isInvalid() const { return Id == InvalidNodeId; }
};

}
}
#endif
