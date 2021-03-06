















#if !defined(LLVM_CLANG_ASTMATCHERS_DYNAMIC_VARIANTVALUE_H)
#define LLVM_CLANG_ASTMATCHERS_DYNAMIC_VARIANTVALUE_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/Optional.h"
#include <memory>
#include <vector>

namespace clang {
namespace ast_matchers {
namespace dynamic {




class ArgKind {
public:
enum Kind {
AK_Matcher,
AK_Node,
AK_Boolean,
AK_Double,
AK_Unsigned,
AK_String
};

ArgKind(Kind K) : K(K) { assert(K != AK_Matcher); }


static ArgKind MakeMatcherArg(ASTNodeKind MatcherKind) {
return ArgKind{AK_Matcher, MatcherKind};
}

static ArgKind MakeNodeArg(ASTNodeKind MatcherKind) {
return ArgKind{AK_Node, MatcherKind};
}

Kind getArgKind() const { return K; }
ASTNodeKind getMatcherKind() const {
assert(K == AK_Matcher);
return NodeKind;
}
ASTNodeKind getNodeKind() const {
assert(K == AK_Node);
return NodeKind;
}







bool isConvertibleTo(ArgKind To, unsigned *Specificity) const;

bool operator<(const ArgKind &Other) const {
if ((K == AK_Matcher && Other.K == AK_Matcher) ||
(K == AK_Node && Other.K == AK_Node))
return NodeKind < Other.NodeKind;
return K < Other.K;
}


std::string asString() const;

private:
ArgKind(Kind K, ASTNodeKind NK) : K(K), NodeKind(NK) {}
Kind K;
ASTNodeKind NodeKind;
};

using ast_matchers::internal::DynTypedMatcher;














class VariantMatcher {

class MatcherOps {
public:
MatcherOps(ASTNodeKind NodeKind) : NodeKind(NodeKind) {}

bool canConstructFrom(const DynTypedMatcher &Matcher,
bool &IsExactMatch) const;



DynTypedMatcher convertMatcher(const DynTypedMatcher &Matcher) const;




llvm::Optional<DynTypedMatcher>
constructVariadicOperator(DynTypedMatcher::VariadicOperator Op,
ArrayRef<VariantMatcher> InnerMatchers) const;

private:
ASTNodeKind NodeKind;
};




class Payload {
public:
virtual ~Payload();
virtual llvm::Optional<DynTypedMatcher> getSingleMatcher() const = 0;
virtual std::string getTypeAsString() const = 0;
virtual llvm::Optional<DynTypedMatcher>
getTypedMatcher(const MatcherOps &Ops) const = 0;
virtual bool isConvertibleTo(ASTNodeKind Kind,
unsigned *Specificity) const = 0;
};

public:

VariantMatcher();


static VariantMatcher SingleMatcher(const DynTypedMatcher &Matcher);




static VariantMatcher
PolymorphicMatcher(std::vector<DynTypedMatcher> Matchers);




static VariantMatcher
VariadicOperatorMatcher(DynTypedMatcher::VariadicOperator Op,
std::vector<VariantMatcher> Args);


void reset();


bool isNull() const { return !Value; }






llvm::Optional<DynTypedMatcher> getSingleMatcher() const;









template <class T>
bool hasTypedMatcher() const {
return hasTypedMatcher(ASTNodeKind::getFromNodeKind<T>());
}

bool hasTypedMatcher(ASTNodeKind NK) const {
if (!Value) return false;
return Value->getTypedMatcher(MatcherOps(NK)).hasValue();
}







bool isConvertibleTo(ASTNodeKind Kind, unsigned *Specificity) const {
if (Value)
return Value->isConvertibleTo(Kind, Specificity);
return false;
}





template <class T>
ast_matchers::internal::Matcher<T> getTypedMatcher() const {
assert(hasTypedMatcher<T>() && "hasTypedMatcher<T>() == false");
return Value->getTypedMatcher(MatcherOps(ASTNodeKind::getFromNodeKind<T>()))
->template convertTo<T>();
}

DynTypedMatcher getTypedMatcher(ASTNodeKind NK) const {
assert(hasTypedMatcher(NK) && "hasTypedMatcher(NK) == false");
return *Value->getTypedMatcher(MatcherOps(NK));
}





std::string getTypeAsString() const;

private:
explicit VariantMatcher(std::shared_ptr<Payload> Value)
: Value(std::move(Value)) {}


class SinglePayload;
class PolymorphicPayload;
class VariadicOpPayload;

std::shared_ptr<const Payload> Value;
};















class VariantValue {
public:
VariantValue() : Type(VT_Nothing) {}

VariantValue(const VariantValue &Other);
~VariantValue();
VariantValue &operator=(const VariantValue &Other);


VariantValue(bool Boolean);
VariantValue(double Double);
VariantValue(unsigned Unsigned);
VariantValue(StringRef String);
VariantValue(ASTNodeKind NodeKind);
VariantValue(const VariantMatcher &Matchers);


VariantValue(int Signed) : VariantValue(static_cast<unsigned>(Signed)) {}


explicit operator bool() const { return hasValue(); }
bool hasValue() const { return Type != VT_Nothing; }


bool isBoolean() const;
bool getBoolean() const;
void setBoolean(bool Boolean);


bool isDouble() const;
double getDouble() const;
void setDouble(double Double);


bool isUnsigned() const;
unsigned getUnsigned() const;
void setUnsigned(unsigned Unsigned);


bool isString() const;
const std::string &getString() const;
void setString(StringRef String);

bool isNodeKind() const;
const ASTNodeKind &getNodeKind() const;
void setNodeKind(ASTNodeKind NodeKind);


bool isMatcher() const;
const VariantMatcher &getMatcher() const;
void setMatcher(const VariantMatcher &Matcher);







bool isConvertibleTo(ArgKind Kind, unsigned* Specificity) const;









bool isConvertibleTo(ArrayRef<ArgKind> Kinds, unsigned *Specificity) const;


std::string getTypeAsString() const;

private:
void reset();


enum ValueType {
VT_Nothing,
VT_Boolean,
VT_Double,
VT_Unsigned,
VT_String,
VT_Matcher,
VT_NodeKind
};


union AllValues {
unsigned Unsigned;
double Double;
bool Boolean;
std::string *String;
VariantMatcher *Matcher;
ASTNodeKind *NodeKind;
};

ValueType Type;
AllValues Value;
};

}
}
}

#endif
