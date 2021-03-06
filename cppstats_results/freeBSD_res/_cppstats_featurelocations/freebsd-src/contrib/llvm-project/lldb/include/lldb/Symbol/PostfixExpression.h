












#if !defined(LLDB_SYMBOL_POSTFIXEXPRESSION_H)
#define LLDB_SYMBOL_POSTFIXEXPRESSION_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Casting.h"
#include <vector>

namespace lldb_private {

class Stream;

namespace postfix {


class Node {
public:
enum Kind {
BinaryOp,
InitialValue,
Integer,
Register,
Symbol,
UnaryOp,
};

protected:
Node(Kind kind) : m_kind(kind) {}

public:
Kind GetKind() const { return m_kind; }

private:
Kind m_kind;
};


class BinaryOpNode : public Node {
public:
enum OpType {
Align,
Minus,
Plus,
};

BinaryOpNode(OpType op_type, Node &left, Node &right)
: Node(BinaryOp), m_op_type(op_type), m_left(&left), m_right(&right) {}

OpType GetOpType() const { return m_op_type; }

const Node *Left() const { return m_left; }
Node *&Left() { return m_left; }

const Node *Right() const { return m_right; }
Node *&Right() { return m_right; }

static bool classof(const Node *node) { return node->GetKind() == BinaryOp; }

private:
OpType m_op_type;
Node *m_left;
Node *m_right;
};


class InitialValueNode: public Node {
public:
InitialValueNode() : Node(InitialValue) {}

static bool classof(const Node *node) {
return node->GetKind() == InitialValue;
}
};


class IntegerNode : public Node {
public:
IntegerNode(int64_t value) : Node(Integer), m_value(value) {}

int64_t GetValue() const { return m_value; }

static bool classof(const Node *node) { return node->GetKind() == Integer; }

private:
int64_t m_value;
};





class RegisterNode : public Node {
public:
RegisterNode(uint32_t reg_num) : Node(Register), m_reg_num(reg_num) {}

uint32_t GetRegNum() const { return m_reg_num; }

static bool classof(const Node *node) { return node->GetKind() == Register; }

private:
uint32_t m_reg_num;
};



class SymbolNode : public Node {
public:
SymbolNode(llvm::StringRef name) : Node(Symbol), m_name(name) {}

llvm::StringRef GetName() const { return m_name; }

static bool classof(const Node *node) { return node->GetKind() == Symbol; }

private:
llvm::StringRef m_name;
};


class UnaryOpNode : public Node {
public:
enum OpType {
Deref,
};

UnaryOpNode(OpType op_type, Node &operand)
: Node(UnaryOp), m_op_type(op_type), m_operand(&operand) {}

OpType GetOpType() const { return m_op_type; }

const Node *Operand() const { return m_operand; }
Node *&Operand() { return m_operand; }

static bool classof(const Node *node) { return node->GetKind() == UnaryOp; }

private:
OpType m_op_type;
Node *m_operand;
};











template <typename ResultT = void> class Visitor {
protected:
virtual ~Visitor() = default;

virtual ResultT Visit(BinaryOpNode &binary, Node *&ref) = 0;
virtual ResultT Visit(InitialValueNode &val, Node *&ref) = 0;
virtual ResultT Visit(IntegerNode &integer, Node *&) = 0;
virtual ResultT Visit(RegisterNode &reg, Node *&) = 0;
virtual ResultT Visit(SymbolNode &symbol, Node *&ref) = 0;
virtual ResultT Visit(UnaryOpNode &unary, Node *&ref) = 0;



ResultT Dispatch(Node *&node) {
switch (node->GetKind()) {
case Node::BinaryOp:
return Visit(llvm::cast<BinaryOpNode>(*node), node);
case Node::InitialValue:
return Visit(llvm::cast<InitialValueNode>(*node), node);
case Node::Integer:
return Visit(llvm::cast<IntegerNode>(*node), node);
case Node::Register:
return Visit(llvm::cast<RegisterNode>(*node), node);
case Node::Symbol:
return Visit(llvm::cast<SymbolNode>(*node), node);
case Node::UnaryOp:
return Visit(llvm::cast<UnaryOpNode>(*node), node);
}
llvm_unreachable("Fully covered switch!");
}
};









bool ResolveSymbols(Node *&node,
llvm::function_ref<Node *(SymbolNode &symbol)> replacer);

template <typename T, typename... Args>
inline T *MakeNode(llvm::BumpPtrAllocator &alloc, Args &&... args) {
static_assert(std::is_trivially_destructible<T>::value,
"This object will not be destroyed!");
return new (alloc.Allocate<T>()) T(std::forward<Args>(args)...);
}



Node *ParseOneExpression(llvm::StringRef expr, llvm::BumpPtrAllocator &alloc);

std::vector<std::pair<llvm::StringRef, Node *>>
ParseFPOProgram(llvm::StringRef prog, llvm::BumpPtrAllocator &alloc);






void ToDWARF(Node &node, Stream &stream);

}
}

#endif
