



















#if !defined(LLVM_CLANG_TOOLING_SYNTAX_NODES_H)
#define LLVM_CLANG_TOOLING_SYNTAX_NODES_H

#include "clang/Basic/TokenKinds.h"
#include "clang/Lex/Token.h"
#include "clang/Tooling/Syntax/Tokens.h"
#include "clang/Tooling/Syntax/Tree.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
namespace clang {
namespace syntax {




enum class NodeKind : uint16_t {
#define CONCRETE_NODE(Kind, Base) Kind,
#include "clang/Tooling/Syntax/Nodes.inc"
};

raw_ostream &operator<<(raw_ostream &OS, NodeKind K);
















enum class NodeRole : uint8_t {


Detached,

Unknown,

OpenParen,

CloseParen,

IntroducerKeyword,

LiteralToken,

ArrowToken,
ExternKeyword,
TemplateKeyword,



BodyStatement,

ListElement,
ListDelimiter,


OperatorToken,
Operand,
LeftHandSide,
RightHandSide,
ReturnValue,
CaseValue,
ThenStatement,
ElseKeyword,
ElseStatement,
Expression,
Statement,
Condition,
Message,
Declarator,
Declaration,
Size,
Parameters,
TrailingReturn,
UnqualifiedId,
Qualifier,
SubExpression,
Object,
AccessToken,
Member,
Callee,
Arguments,
Declarators
};

raw_ostream &operator<<(raw_ostream &OS, NodeRole R);

#include "clang/Tooling/Syntax/NodeClasses.inc"



class NestedNameSpecifier final : public List {
public:
NestedNameSpecifier() : List(NodeKind::NestedNameSpecifier) {}
static bool classof(const Node *N);
std::vector<NameSpecifier *> getSpecifiers();
std::vector<List::ElementAndDelimiter<syntax::NameSpecifier>>
getSpecifiersAndDoubleColons();
};



class UnqualifiedId final : public Tree {
public:
UnqualifiedId() : Tree(NodeKind::UnqualifiedId) {}
static bool classof(const Node *N);
};



class UnknownExpression final : public Expression {
public:
UnknownExpression() : Expression(NodeKind::UnknownExpression) {}
static bool classof(const Node *N);
};






class CallArguments final : public List {
public:
CallArguments() : List(NodeKind::CallArguments) {}
static bool classof(const Node *N);
std::vector<Expression *> getArguments();
std::vector<List::ElementAndDelimiter<Expression>> getArgumentsAndCommas();
};


class UnaryOperatorExpression : public Expression {
public:
UnaryOperatorExpression(NodeKind K) : Expression(K) {}
static bool classof(const Node *N);
Leaf *getOperatorToken();
Expression *getOperand();
};










class PrefixUnaryOperatorExpression final : public UnaryOperatorExpression {
public:
PrefixUnaryOperatorExpression()
: UnaryOperatorExpression(NodeKind::PrefixUnaryOperatorExpression) {}
static bool classof(const Node *N);
};






class PostfixUnaryOperatorExpression final : public UnaryOperatorExpression {
public:
PostfixUnaryOperatorExpression()
: UnaryOperatorExpression(NodeKind::PostfixUnaryOperatorExpression) {}
static bool classof(const Node *N);
};








class BinaryOperatorExpression final : public Expression {
public:
BinaryOperatorExpression() : Expression(NodeKind::BinaryOperatorExpression) {}
static bool classof(const Node *N);
Expression *getLhs();
Leaf *getOperatorToken();
Expression *getRhs();
};



class Statement : public Tree {
public:
Statement(NodeKind K) : Tree(K) {}
static bool classof(const Node *N);
};



class UnknownStatement final : public Statement {
public:
UnknownStatement() : Statement(NodeKind::UnknownStatement) {}
static bool classof(const Node *N);
};


class DeclarationStatement final : public Statement {
public:
DeclarationStatement() : Statement(NodeKind::DeclarationStatement) {}
static bool classof(const Node *N);
};


class EmptyStatement final : public Statement {
public:
EmptyStatement() : Statement(NodeKind::EmptyStatement) {}
static bool classof(const Node *N);
};


class SwitchStatement final : public Statement {
public:
SwitchStatement() : Statement(NodeKind::SwitchStatement) {}
static bool classof(const Node *N);
Leaf *getSwitchKeyword();
Statement *getBody();
};


class CaseStatement final : public Statement {
public:
CaseStatement() : Statement(NodeKind::CaseStatement) {}
static bool classof(const Node *N);
Leaf *getCaseKeyword();
Expression *getCaseValue();
Statement *getBody();
};


class DefaultStatement final : public Statement {
public:
DefaultStatement() : Statement(NodeKind::DefaultStatement) {}
static bool classof(const Node *N);
Leaf *getDefaultKeyword();
Statement *getBody();
};



class IfStatement final : public Statement {
public:
IfStatement() : Statement(NodeKind::IfStatement) {}
static bool classof(const Node *N);
Leaf *getIfKeyword();
Statement *getThenStatement();
Leaf *getElseKeyword();
Statement *getElseStatement();
};


class ForStatement final : public Statement {
public:
ForStatement() : Statement(NodeKind::ForStatement) {}
static bool classof(const Node *N);
Leaf *getForKeyword();
Statement *getBody();
};


class WhileStatement final : public Statement {
public:
WhileStatement() : Statement(NodeKind::WhileStatement) {}
static bool classof(const Node *N);
Leaf *getWhileKeyword();
Statement *getBody();
};


class ContinueStatement final : public Statement {
public:
ContinueStatement() : Statement(NodeKind::ContinueStatement) {}
static bool classof(const Node *N);
Leaf *getContinueKeyword();
};


class BreakStatement final : public Statement {
public:
BreakStatement() : Statement(NodeKind::BreakStatement) {}
static bool classof(const Node *N);
Leaf *getBreakKeyword();
};



class ReturnStatement final : public Statement {
public:
ReturnStatement() : Statement(NodeKind::ReturnStatement) {}
static bool classof(const Node *N);
Leaf *getReturnKeyword();
Expression *getReturnValue();
};


class RangeBasedForStatement final : public Statement {
public:
RangeBasedForStatement() : Statement(NodeKind::RangeBasedForStatement) {}
static bool classof(const Node *N);
Leaf *getForKeyword();
Statement *getBody();
};



class ExpressionStatement final : public Statement {
public:
ExpressionStatement() : Statement(NodeKind::ExpressionStatement) {}
static bool classof(const Node *N);
Expression *getExpression();
};


class CompoundStatement final : public Statement {
public:
CompoundStatement() : Statement(NodeKind::CompoundStatement) {}
static bool classof(const Node *N);
Leaf *getLbrace();

std::vector<Statement *> getStatements();
Leaf *getRbrace();
};





class Declaration : public Tree {
public:
Declaration(NodeKind K) : Tree(K) {}
static bool classof(const Node *N);
};


class UnknownDeclaration final : public Declaration {
public:
UnknownDeclaration() : Declaration(NodeKind::UnknownDeclaration) {}
static bool classof(const Node *N);
};


class EmptyDeclaration final : public Declaration {
public:
EmptyDeclaration() : Declaration(NodeKind::EmptyDeclaration) {}
static bool classof(const Node *N);
};



class StaticAssertDeclaration final : public Declaration {
public:
StaticAssertDeclaration() : Declaration(NodeKind::StaticAssertDeclaration) {}
static bool classof(const Node *N);
Expression *getCondition();
Expression *getMessage();
};



class LinkageSpecificationDeclaration final : public Declaration {
public:
LinkageSpecificationDeclaration()
: Declaration(NodeKind::LinkageSpecificationDeclaration) {}
static bool classof(const Node *N);
};

class DeclaratorList final : public List {
public:
DeclaratorList() : List(NodeKind::DeclaratorList) {}
static bool classof(const Node *N);
std::vector<SimpleDeclarator *> getDeclarators();
std::vector<List::ElementAndDelimiter<syntax::SimpleDeclarator>>
getDeclaratorsAndCommas();
};




class SimpleDeclaration final : public Declaration {
public:
SimpleDeclaration() : Declaration(NodeKind::SimpleDeclaration) {}
static bool classof(const Node *N);

std::vector<SimpleDeclarator *> getDeclarators();
};


class TemplateDeclaration final : public Declaration {
public:
TemplateDeclaration() : Declaration(NodeKind::TemplateDeclaration) {}
static bool classof(const Node *N);
Leaf *getTemplateKeyword();
Declaration *getDeclaration();
};






class ExplicitTemplateInstantiation final : public Declaration {
public:
ExplicitTemplateInstantiation()
: Declaration(NodeKind::ExplicitTemplateInstantiation) {}
static bool classof(const Node *N);
Leaf *getTemplateKeyword();
Leaf *getExternKeyword();
Declaration *getDeclaration();
};


class NamespaceDefinition final : public Declaration {
public:
NamespaceDefinition() : Declaration(NodeKind::NamespaceDefinition) {}
static bool classof(const Node *N);
};


class NamespaceAliasDefinition final : public Declaration {
public:
NamespaceAliasDefinition()
: Declaration(NodeKind::NamespaceAliasDefinition) {}
static bool classof(const Node *N);
};


class UsingNamespaceDirective final : public Declaration {
public:
UsingNamespaceDirective() : Declaration(NodeKind::UsingNamespaceDirective) {}
static bool classof(const Node *N);
};



class UsingDeclaration final : public Declaration {
public:
UsingDeclaration() : Declaration(NodeKind::UsingDeclaration) {}
static bool classof(const Node *N);
};


class TypeAliasDeclaration final : public Declaration {
public:
TypeAliasDeclaration() : Declaration(NodeKind::TypeAliasDeclaration) {}
static bool classof(const Node *N);
};













class Declarator : public Tree {
public:
Declarator(NodeKind K) : Tree(K) {}
static bool classof(const Node *N);
};



class SimpleDeclarator final : public Declarator {
public:
SimpleDeclarator() : Declarator(NodeKind::SimpleDeclarator) {}
static bool classof(const Node *N);
};




class ParenDeclarator final : public Declarator {
public:
ParenDeclarator() : Declarator(NodeKind::ParenDeclarator) {}
static bool classof(const Node *N);
Leaf *getLparen();
Leaf *getRparen();
};





class ArraySubscript final : public Tree {
public:
ArraySubscript() : Tree(NodeKind::ArraySubscript) {}
static bool classof(const Node *N);

Leaf *getLbracket();
Expression *getSize();
Leaf *getRbracket();
};



class TrailingReturnType final : public Tree {
public:
TrailingReturnType() : Tree(NodeKind::TrailingReturnType) {}
static bool classof(const Node *N);

Leaf *getArrowToken();


SimpleDeclarator *getDeclarator();
};



class ParameterDeclarationList final : public List {
public:
ParameterDeclarationList() : List(NodeKind::ParameterDeclarationList) {}
static bool classof(const Node *N);
std::vector<SimpleDeclaration *> getParameterDeclarations();
std::vector<List::ElementAndDelimiter<syntax::SimpleDeclaration>>
getParametersAndCommas();
};












class ParametersAndQualifiers final : public Tree {
public:
ParametersAndQualifiers() : Tree(NodeKind::ParametersAndQualifiers) {}
static bool classof(const Node *N);
Leaf *getLparen();
ParameterDeclarationList *getParameters();
Leaf *getRparen();
TrailingReturnType *getTrailingReturn();
};



class MemberPointer final : public Tree {
public:
MemberPointer() : Tree(NodeKind::MemberPointer) {}
static bool classof(const Node *N);
};

#define CONCRETE_NODE(Kind, Base) inline bool Kind::classof(const Node *N) { return N->getKind() == NodeKind::Kind; }



#define ABSTRACT_NODE(Kind, Base, First, Last) inline bool Kind::classof(const Node *N) { return N->getKind() >= NodeKind::First && N->getKind() <= NodeKind::Last; }



#include "clang/Tooling/Syntax/Nodes.inc"

}
}
#endif
