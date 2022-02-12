



















#if !defined(LLVM_CLANG_TOOLING_SYNTAX_TREE_CASCADE_H)
#define LLVM_CLANG_TOOLING_SYNTAX_TREE_CASCADE_H

#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Tooling/Syntax/Tokens.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/iterator.h"
#include "llvm/Support/Allocator.h"
#include <cstdint>
#include <iterator>

namespace clang {
namespace syntax {



class Arena {
public:
Arena(SourceManager &SourceMgr, const LangOptions &LangOpts,
const TokenBuffer &Tokens);

const SourceManager &getSourceManager() const { return SourceMgr; }
const LangOptions &getLangOptions() const { return LangOpts; }

const TokenBuffer &getTokenBuffer() const;
llvm::BumpPtrAllocator &getAllocator() { return Allocator; }

private:



std::pair<FileID, ArrayRef<Token>>
lexBuffer(std::unique_ptr<llvm::MemoryBuffer> Buffer);
friend class FactoryImpl;

private:
SourceManager &SourceMgr;
const LangOptions &LangOpts;
const TokenBuffer &Tokens;

llvm::DenseMap<FileID, std::vector<Token>> ExtraTokens;

llvm::BumpPtrAllocator Allocator;
};

class Tree;
class TreeBuilder;
class FactoryImpl;
class MutationsImpl;

enum class NodeKind : uint16_t;
enum class NodeRole : uint8_t;



class Node {
protected:


Node(NodeKind Kind);

~Node() = default;

public:

Node(const Node &) = delete;
Node &operator=(const Node &) = delete;

Node(Node &&) = delete;
Node &operator=(Node &&) = delete;

NodeKind getKind() const { return static_cast<NodeKind>(Kind); }
NodeRole getRole() const { return static_cast<NodeRole>(Role); }


bool isDetached() const;






bool isOriginal() const { return Original; }






bool canModify() const { return CanModify; }

const Tree *getParent() const { return Parent; }
Tree *getParent() { return Parent; }

const Node *getNextSibling() const { return NextSibling; }
Node *getNextSibling() { return NextSibling; }
const Node *getPreviousSibling() const { return PreviousSibling; }
Node *getPreviousSibling() { return PreviousSibling; }


std::string dump(const SourceManager &SM) const;

std::string dumpTokens(const SourceManager &SM) const;



void assertInvariants() const;

void assertInvariantsRecursive() const;

private:

friend class Tree;

friend class TreeBuilder;

friend class MutationsImpl;

friend class FactoryImpl;

void setRole(NodeRole NR);

Tree *Parent;
Node *NextSibling;
Node *PreviousSibling;
unsigned Kind : 16;
unsigned Role : 8;
unsigned Original : 1;
unsigned CanModify : 1;
};


class Leaf final : public Node {
public:
Leaf(const Token *T);
static bool classof(const Node *N);

const Token *getToken() const { return Tok; }

private:
const Token *Tok;
};


class Tree : public Node {


template <typename DerivedT, typename NodeT>
class ChildIteratorBase
: public llvm::iterator_facade_base<DerivedT, std::forward_iterator_tag,
NodeT> {
protected:
NodeT *N = nullptr;
using Base = ChildIteratorBase;

public:
ChildIteratorBase() = default;
explicit ChildIteratorBase(NodeT *N) : N(N) {}

bool operator==(const DerivedT &O) const { return O.N == N; }
NodeT &operator*() const { return *N; }
DerivedT &operator++() {
N = N->getNextSibling();
return *static_cast<DerivedT *>(this);
}



explicit operator bool() const { return N != nullptr; }

NodeT *asPointer() const { return N; }
};

public:
static bool classof(const Node *N);

Node *getFirstChild() { return FirstChild; }
const Node *getFirstChild() const { return FirstChild; }
Node *getLastChild() { return LastChild; }
const Node *getLastChild() const { return LastChild; }

const Leaf *findFirstLeaf() const;
Leaf *findFirstLeaf() {
return const_cast<Leaf *>(const_cast<const Tree *>(this)->findFirstLeaf());
}

const Leaf *findLastLeaf() const;
Leaf *findLastLeaf() {
return const_cast<Leaf *>(const_cast<const Tree *>(this)->findLastLeaf());
}


struct ChildIterator : ChildIteratorBase<ChildIterator, Node> {
using Base::ChildIteratorBase;
};
struct ConstChildIterator
: ChildIteratorBase<ConstChildIterator, const Node> {
using Base::ChildIteratorBase;
ConstChildIterator() = default;
ConstChildIterator(const ChildIterator &I) : Base(I.asPointer()) {}
};

llvm::iterator_range<ChildIterator> getChildren() {
return {ChildIterator(getFirstChild()), ChildIterator()};
}
llvm::iterator_range<ConstChildIterator> getChildren() const {
return {ConstChildIterator(getFirstChild()), ConstChildIterator()};
}


const Node *findChild(NodeRole R) const;
Node *findChild(NodeRole R) {
return const_cast<Node *>(const_cast<const Tree *>(this)->findChild(R));
}

protected:
using Node::Node;

private:




void appendChildLowLevel(Node *Child, NodeRole Role);

void prependChildLowLevel(Node *Child, NodeRole Role);



void appendChildLowLevel(Node *Child);
void prependChildLowLevel(Node *Child);
friend class TreeBuilder;
friend class FactoryImpl;







void replaceChildRangeLowLevel(Node *Begin, Node *End, Node *New);
friend class MutationsImpl;

Node *FirstChild = nullptr;
Node *LastChild = nullptr;
};




inline bool operator==(const Tree::ConstChildIterator &A,
const Tree::ConstChildIterator &B) {
return A.operator==(B);
}





class List : public Tree {
public:
template <typename Element> struct ElementAndDelimiter {
Element *element;
Leaf *delimiter;
};

enum class TerminationKind {
Terminated,
MaybeTerminated,
Separated,
};

using Tree::Tree;
static bool classof(const Node *N);














std::vector<ElementAndDelimiter<Node>> getElementsAsNodesAndDelimiters();




std::vector<Node *> getElementsAsNodes();







clang::tok::TokenKind getDelimiterTokenKind() const;

TerminationKind getTerminationKind() const;






bool canBeEmpty() const;
};

}
}

#endif
