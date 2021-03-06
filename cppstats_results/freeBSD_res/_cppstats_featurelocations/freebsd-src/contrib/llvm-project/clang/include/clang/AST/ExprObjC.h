











#if !defined(LLVM_CLANG_AST_EXPROBJC_H)
#define LLVM_CLANG_AST_EXPROBJC_H

#include "clang/AST/ComputeDependence.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DependenceFlags.h"
#include "clang/AST/Expr.h"
#include "clang/AST/OperationKinds.h"
#include "clang/AST/SelectorLocationsKind.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Type.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/type_traits.h"
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace clang {

class ASTContext;
class CXXBaseSpecifier;



class ObjCStringLiteral : public Expr {
Stmt *String;
SourceLocation AtLoc;

public:
ObjCStringLiteral(StringLiteral *SL, QualType T, SourceLocation L)
: Expr(ObjCStringLiteralClass, T, VK_PRValue, OK_Ordinary), String(SL),
AtLoc(L) {
setDependence(ExprDependence::None);
}
explicit ObjCStringLiteral(EmptyShell Empty)
: Expr(ObjCStringLiteralClass, Empty) {}

StringLiteral *getString() { return cast<StringLiteral>(String); }
const StringLiteral *getString() const { return cast<StringLiteral>(String); }
void setString(StringLiteral *S) { String = S; }

SourceLocation getAtLoc() const { return AtLoc; }
void setAtLoc(SourceLocation L) { AtLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return AtLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return String->getEndLoc(); }


child_range children() { return child_range(&String, &String+1); }

const_child_range children() const {
return const_child_range(&String, &String + 1);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCStringLiteralClass;
}
};


class ObjCBoolLiteralExpr : public Expr {
bool Value;
SourceLocation Loc;

public:
ObjCBoolLiteralExpr(bool val, QualType Ty, SourceLocation l)
: Expr(ObjCBoolLiteralExprClass, Ty, VK_PRValue, OK_Ordinary), Value(val),
Loc(l) {
setDependence(ExprDependence::None);
}
explicit ObjCBoolLiteralExpr(EmptyShell Empty)
: Expr(ObjCBoolLiteralExprClass, Empty) {}

bool getValue() const { return Value; }
void setValue(bool V) { Value = V; }

SourceLocation getBeginLoc() const LLVM_READONLY { return Loc; }
SourceLocation getEndLoc() const LLVM_READONLY { return Loc; }

SourceLocation getLocation() const { return Loc; }
void setLocation(SourceLocation L) { Loc = L; }


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCBoolLiteralExprClass;
}
};





class ObjCBoxedExpr : public Expr {
Stmt *SubExpr;
ObjCMethodDecl *BoxingMethod;
SourceRange Range;

public:
friend class ASTStmtReader;

ObjCBoxedExpr(Expr *E, QualType T, ObjCMethodDecl *method, SourceRange R)
: Expr(ObjCBoxedExprClass, T, VK_PRValue, OK_Ordinary), SubExpr(E),
BoxingMethod(method), Range(R) {
setDependence(computeDependence(this));
}
explicit ObjCBoxedExpr(EmptyShell Empty)
: Expr(ObjCBoxedExprClass, Empty) {}

Expr *getSubExpr() { return cast<Expr>(SubExpr); }
const Expr *getSubExpr() const { return cast<Expr>(SubExpr); }

ObjCMethodDecl *getBoxingMethod() const {
return BoxingMethod;
}



bool isExpressibleAsConstantInitializer() const {
return !BoxingMethod && SubExpr;
}

SourceLocation getAtLoc() const { return Range.getBegin(); }

SourceLocation getBeginLoc() const LLVM_READONLY { return Range.getBegin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Range.getEnd(); }

SourceRange getSourceRange() const LLVM_READONLY {
return Range;
}


child_range children() { return child_range(&SubExpr, &SubExpr+1); }

const_child_range children() const {
return const_child_range(&SubExpr, &SubExpr + 1);
}

using const_arg_iterator = ConstExprIterator;

const_arg_iterator arg_begin() const {
return reinterpret_cast<Stmt const * const*>(&SubExpr);
}

const_arg_iterator arg_end() const {
return reinterpret_cast<Stmt const * const*>(&SubExpr + 1);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCBoxedExprClass;
}
};



class ObjCArrayLiteral final
: public Expr,
private llvm::TrailingObjects<ObjCArrayLiteral, Expr *> {
unsigned NumElements;
SourceRange Range;
ObjCMethodDecl *ArrayWithObjectsMethod;

ObjCArrayLiteral(ArrayRef<Expr *> Elements,
QualType T, ObjCMethodDecl * Method,
SourceRange SR);

explicit ObjCArrayLiteral(EmptyShell Empty, unsigned NumElements)
: Expr(ObjCArrayLiteralClass, Empty), NumElements(NumElements) {}

public:
friend class ASTStmtReader;
friend TrailingObjects;

static ObjCArrayLiteral *Create(const ASTContext &C,
ArrayRef<Expr *> Elements,
QualType T, ObjCMethodDecl * Method,
SourceRange SR);

static ObjCArrayLiteral *CreateEmpty(const ASTContext &C,
unsigned NumElements);

SourceLocation getBeginLoc() const LLVM_READONLY { return Range.getBegin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Range.getEnd(); }
SourceRange getSourceRange() const LLVM_READONLY { return Range; }


Expr **getElements() { return getTrailingObjects<Expr *>(); }


const Expr * const *getElements() const {
return getTrailingObjects<Expr *>();
}


unsigned getNumElements() const { return NumElements; }


Expr *getElement(unsigned Index) {
assert((Index < NumElements) && "Arg access out of range!");
return getElements()[Index];
}
const Expr *getElement(unsigned Index) const {
assert((Index < NumElements) && "Arg access out of range!");
return getElements()[Index];
}

ObjCMethodDecl *getArrayWithObjectsMethod() const {
return ArrayWithObjectsMethod;
}


child_range children() {
return child_range(reinterpret_cast<Stmt **>(getElements()),
reinterpret_cast<Stmt **>(getElements()) + NumElements);
}

const_child_range children() const {
auto Children = const_cast<ObjCArrayLiteral *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCArrayLiteralClass;
}
};



struct ObjCDictionaryElement {

Expr *Key;


Expr *Value;


SourceLocation EllipsisLoc;



Optional<unsigned> NumExpansions;


bool isPackExpansion() const { return EllipsisLoc.isValid(); }
};

}

namespace clang {


struct ObjCDictionaryLiteral_KeyValuePair {
Expr *Key;
Expr *Value;
};




struct ObjCDictionaryLiteral_ExpansionData {


SourceLocation EllipsisLoc;



unsigned NumExpansionsPlusOne;
};



class ObjCDictionaryLiteral final
: public Expr,
private llvm::TrailingObjects<ObjCDictionaryLiteral,
ObjCDictionaryLiteral_KeyValuePair,
ObjCDictionaryLiteral_ExpansionData> {

unsigned NumElements : 31;








unsigned HasPackExpansions : 1;

SourceRange Range;
ObjCMethodDecl *DictWithObjectsMethod;

using KeyValuePair = ObjCDictionaryLiteral_KeyValuePair;
using ExpansionData = ObjCDictionaryLiteral_ExpansionData;

ObjCDictionaryLiteral(ArrayRef<ObjCDictionaryElement> VK,
bool HasPackExpansions,
QualType T, ObjCMethodDecl *method,
SourceRange SR);

explicit ObjCDictionaryLiteral(EmptyShell Empty, unsigned NumElements,
bool HasPackExpansions)
: Expr(ObjCDictionaryLiteralClass, Empty), NumElements(NumElements),
HasPackExpansions(HasPackExpansions) {}

size_t numTrailingObjects(OverloadToken<KeyValuePair>) const {
return NumElements;
}

public:
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;

static ObjCDictionaryLiteral *Create(const ASTContext &C,
ArrayRef<ObjCDictionaryElement> VK,
bool HasPackExpansions,
QualType T, ObjCMethodDecl *method,
SourceRange SR);

static ObjCDictionaryLiteral *CreateEmpty(const ASTContext &C,
unsigned NumElements,
bool HasPackExpansions);



unsigned getNumElements() const { return NumElements; }

ObjCDictionaryElement getKeyValueElement(unsigned Index) const {
assert((Index < NumElements) && "Arg access out of range!");
const KeyValuePair &KV = getTrailingObjects<KeyValuePair>()[Index];
ObjCDictionaryElement Result = { KV.Key, KV.Value, SourceLocation(), None };
if (HasPackExpansions) {
const ExpansionData &Expansion =
getTrailingObjects<ExpansionData>()[Index];
Result.EllipsisLoc = Expansion.EllipsisLoc;
if (Expansion.NumExpansionsPlusOne > 0)
Result.NumExpansions = Expansion.NumExpansionsPlusOne - 1;
}
return Result;
}

ObjCMethodDecl *getDictWithObjectsMethod() const {
return DictWithObjectsMethod;
}

SourceLocation getBeginLoc() const LLVM_READONLY { return Range.getBegin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Range.getEnd(); }
SourceRange getSourceRange() const LLVM_READONLY { return Range; }


child_range children() {


static_assert(sizeof(KeyValuePair) == sizeof(Stmt *) * 2,
"KeyValuePair is expected size");
return child_range(
reinterpret_cast<Stmt **>(getTrailingObjects<KeyValuePair>()),
reinterpret_cast<Stmt **>(getTrailingObjects<KeyValuePair>()) +
NumElements * 2);
}

const_child_range children() const {
auto Children = const_cast<ObjCDictionaryLiteral *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCDictionaryLiteralClass;
}
};




class ObjCEncodeExpr : public Expr {
TypeSourceInfo *EncodedType;
SourceLocation AtLoc, RParenLoc;

public:
ObjCEncodeExpr(QualType T, TypeSourceInfo *EncodedType, SourceLocation at,
SourceLocation rp)
: Expr(ObjCEncodeExprClass, T, VK_LValue, OK_Ordinary),
EncodedType(EncodedType), AtLoc(at), RParenLoc(rp) {
setDependence(computeDependence(this));
}

explicit ObjCEncodeExpr(EmptyShell Empty) : Expr(ObjCEncodeExprClass, Empty){}

SourceLocation getAtLoc() const { return AtLoc; }
void setAtLoc(SourceLocation L) { AtLoc = L; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

QualType getEncodedType() const { return EncodedType->getType(); }

TypeSourceInfo *getEncodedTypeSourceInfo() const { return EncodedType; }

void setEncodedTypeSourceInfo(TypeSourceInfo *EncType) {
EncodedType = EncType;
}

SourceLocation getBeginLoc() const LLVM_READONLY { return AtLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCEncodeExprClass;
}
};


class ObjCSelectorExpr : public Expr {
Selector SelName;
SourceLocation AtLoc, RParenLoc;

public:
ObjCSelectorExpr(QualType T, Selector selInfo, SourceLocation at,
SourceLocation rp)
: Expr(ObjCSelectorExprClass, T, VK_PRValue, OK_Ordinary),
SelName(selInfo), AtLoc(at), RParenLoc(rp) {
setDependence(ExprDependence::None);
}
explicit ObjCSelectorExpr(EmptyShell Empty)
: Expr(ObjCSelectorExprClass, Empty) {}

Selector getSelector() const { return SelName; }
void setSelector(Selector S) { SelName = S; }

SourceLocation getAtLoc() const { return AtLoc; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setAtLoc(SourceLocation L) { AtLoc = L; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return AtLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }


unsigned getNumArgs() const { return SelName.getNumArgs(); }


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCSelectorExprClass;
}
};









class ObjCProtocolExpr : public Expr {
ObjCProtocolDecl *TheProtocol;
SourceLocation AtLoc, ProtoLoc, RParenLoc;

public:
friend class ASTStmtReader;
friend class ASTStmtWriter;

ObjCProtocolExpr(QualType T, ObjCProtocolDecl *protocol, SourceLocation at,
SourceLocation protoLoc, SourceLocation rp)
: Expr(ObjCProtocolExprClass, T, VK_PRValue, OK_Ordinary),
TheProtocol(protocol), AtLoc(at), ProtoLoc(protoLoc), RParenLoc(rp) {
setDependence(ExprDependence::None);
}
explicit ObjCProtocolExpr(EmptyShell Empty)
: Expr(ObjCProtocolExprClass, Empty) {}

ObjCProtocolDecl *getProtocol() const { return TheProtocol; }
void setProtocol(ObjCProtocolDecl *P) { TheProtocol = P; }

SourceLocation getProtocolIdLoc() const { return ProtoLoc; }
SourceLocation getAtLoc() const { return AtLoc; }
SourceLocation getRParenLoc() const { return RParenLoc; }
void setAtLoc(SourceLocation L) { AtLoc = L; }
void setRParenLoc(SourceLocation L) { RParenLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY { return AtLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RParenLoc; }


child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCProtocolExprClass;
}
};


class ObjCIvarRefExpr : public Expr {
ObjCIvarDecl *D;
Stmt *Base;
SourceLocation Loc;


SourceLocation OpLoc;


bool IsArrow : 1;


bool IsFreeIvar : 1;

public:
ObjCIvarRefExpr(ObjCIvarDecl *d, QualType t, SourceLocation l,
SourceLocation oploc, Expr *base, bool arrow = false,
bool freeIvar = false)
: Expr(ObjCIvarRefExprClass, t, VK_LValue,
d->isBitField() ? OK_BitField : OK_Ordinary),
D(d), Base(base), Loc(l), OpLoc(oploc), IsArrow(arrow),
IsFreeIvar(freeIvar) {
setDependence(computeDependence(this));
}

explicit ObjCIvarRefExpr(EmptyShell Empty)
: Expr(ObjCIvarRefExprClass, Empty) {}

ObjCIvarDecl *getDecl() { return D; }
const ObjCIvarDecl *getDecl() const { return D; }
void setDecl(ObjCIvarDecl *d) { D = d; }

const Expr *getBase() const { return cast<Expr>(Base); }
Expr *getBase() { return cast<Expr>(Base); }
void setBase(Expr * base) { Base = base; }

bool isArrow() const { return IsArrow; }
bool isFreeIvar() const { return IsFreeIvar; }
void setIsArrow(bool A) { IsArrow = A; }
void setIsFreeIvar(bool A) { IsFreeIvar = A; }

SourceLocation getLocation() const { return Loc; }
void setLocation(SourceLocation L) { Loc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY {
return isFreeIvar() ? Loc : getBase()->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY { return Loc; }

SourceLocation getOpLoc() const { return OpLoc; }
void setOpLoc(SourceLocation L) { OpLoc = L; }


child_range children() { return child_range(&Base, &Base+1); }

const_child_range children() const {
return const_child_range(&Base, &Base + 1);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCIvarRefExprClass;
}
};



class ObjCPropertyRefExpr : public Expr {
private:




llvm::PointerIntPair<NamedDecl *, 1, bool> PropertyOrGetter;




enum MethodRefFlags {
MethodRef_None = 0,
MethodRef_Getter = 0x1,
MethodRef_Setter = 0x2
};


llvm::PointerIntPair<ObjCMethodDecl *, 2, unsigned> SetterAndMethodRefFlags;






SourceLocation IdLoc;




SourceLocation ReceiverLoc;
llvm::PointerUnion<Stmt *, const Type *, ObjCInterfaceDecl *> Receiver;

public:
ObjCPropertyRefExpr(ObjCPropertyDecl *PD, QualType t, ExprValueKind VK,
ExprObjectKind OK, SourceLocation l, Expr *base)
: Expr(ObjCPropertyRefExprClass, t, VK, OK), PropertyOrGetter(PD, false),
IdLoc(l), Receiver(base) {
assert(t->isSpecificPlaceholderType(BuiltinType::PseudoObject));
setDependence(computeDependence(this));
}

ObjCPropertyRefExpr(ObjCPropertyDecl *PD, QualType t, ExprValueKind VK,
ExprObjectKind OK, SourceLocation l, SourceLocation sl,
QualType st)
: Expr(ObjCPropertyRefExprClass, t, VK, OK), PropertyOrGetter(PD, false),
IdLoc(l), ReceiverLoc(sl), Receiver(st.getTypePtr()) {
assert(t->isSpecificPlaceholderType(BuiltinType::PseudoObject));
setDependence(computeDependence(this));
}

ObjCPropertyRefExpr(ObjCMethodDecl *Getter, ObjCMethodDecl *Setter,
QualType T, ExprValueKind VK, ExprObjectKind OK,
SourceLocation IdLoc, Expr *Base)
: Expr(ObjCPropertyRefExprClass, T, VK, OK),
PropertyOrGetter(Getter, true), SetterAndMethodRefFlags(Setter, 0),
IdLoc(IdLoc), Receiver(Base) {
assert(T->isSpecificPlaceholderType(BuiltinType::PseudoObject));
setDependence(computeDependence(this));
}

ObjCPropertyRefExpr(ObjCMethodDecl *Getter, ObjCMethodDecl *Setter,
QualType T, ExprValueKind VK, ExprObjectKind OK,
SourceLocation IdLoc, SourceLocation SuperLoc,
QualType SuperTy)
: Expr(ObjCPropertyRefExprClass, T, VK, OK),
PropertyOrGetter(Getter, true), SetterAndMethodRefFlags(Setter, 0),
IdLoc(IdLoc), ReceiverLoc(SuperLoc), Receiver(SuperTy.getTypePtr()) {
assert(T->isSpecificPlaceholderType(BuiltinType::PseudoObject));
setDependence(computeDependence(this));
}

ObjCPropertyRefExpr(ObjCMethodDecl *Getter, ObjCMethodDecl *Setter,
QualType T, ExprValueKind VK, ExprObjectKind OK,
SourceLocation IdLoc, SourceLocation ReceiverLoc,
ObjCInterfaceDecl *Receiver)
: Expr(ObjCPropertyRefExprClass, T, VK, OK),
PropertyOrGetter(Getter, true), SetterAndMethodRefFlags(Setter, 0),
IdLoc(IdLoc), ReceiverLoc(ReceiverLoc), Receiver(Receiver) {
assert(T->isSpecificPlaceholderType(BuiltinType::PseudoObject));
setDependence(computeDependence(this));
}

explicit ObjCPropertyRefExpr(EmptyShell Empty)
: Expr(ObjCPropertyRefExprClass, Empty) {}

bool isImplicitProperty() const { return PropertyOrGetter.getInt(); }
bool isExplicitProperty() const { return !PropertyOrGetter.getInt(); }

ObjCPropertyDecl *getExplicitProperty() const {
assert(!isImplicitProperty());
return cast<ObjCPropertyDecl>(PropertyOrGetter.getPointer());
}

ObjCMethodDecl *getImplicitPropertyGetter() const {
assert(isImplicitProperty());
return cast_or_null<ObjCMethodDecl>(PropertyOrGetter.getPointer());
}

ObjCMethodDecl *getImplicitPropertySetter() const {
assert(isImplicitProperty());
return SetterAndMethodRefFlags.getPointer();
}

Selector getGetterSelector() const {
if (isImplicitProperty())
return getImplicitPropertyGetter()->getSelector();
return getExplicitProperty()->getGetterName();
}

Selector getSetterSelector() const {
if (isImplicitProperty())
return getImplicitPropertySetter()->getSelector();
return getExplicitProperty()->getSetterName();
}




bool isMessagingGetter() const {
return SetterAndMethodRefFlags.getInt() & MethodRef_Getter;
}




bool isMessagingSetter() const {
return SetterAndMethodRefFlags.getInt() & MethodRef_Setter;
}

void setIsMessagingGetter(bool val = true) {
setMethodRefFlag(MethodRef_Getter, val);
}

void setIsMessagingSetter(bool val = true) {
setMethodRefFlag(MethodRef_Setter, val);
}

const Expr *getBase() const {
return cast<Expr>(Receiver.get<Stmt*>());
}
Expr *getBase() {
return cast<Expr>(Receiver.get<Stmt*>());
}

SourceLocation getLocation() const { return IdLoc; }

SourceLocation getReceiverLocation() const { return ReceiverLoc; }

QualType getSuperReceiverType() const {
return QualType(Receiver.get<const Type*>(), 0);
}

ObjCInterfaceDecl *getClassReceiver() const {
return Receiver.get<ObjCInterfaceDecl*>();
}

bool isObjectReceiver() const { return Receiver.is<Stmt*>(); }
bool isSuperReceiver() const { return Receiver.is<const Type*>(); }
bool isClassReceiver() const { return Receiver.is<ObjCInterfaceDecl*>(); }


QualType getReceiverType(const ASTContext &ctx) const;

SourceLocation getBeginLoc() const LLVM_READONLY {
return isObjectReceiver() ? getBase()->getBeginLoc()
: getReceiverLocation();
}

SourceLocation getEndLoc() const LLVM_READONLY { return IdLoc; }


child_range children() {
if (Receiver.is<Stmt*>()) {
Stmt **begin = reinterpret_cast<Stmt**>(&Receiver);
return child_range(begin, begin+1);
}
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
auto Children = const_cast<ObjCPropertyRefExpr *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCPropertyRefExprClass;
}

private:
friend class ASTStmtReader;
friend class ASTStmtWriter;

void setExplicitProperty(ObjCPropertyDecl *D, unsigned methRefFlags) {
PropertyOrGetter.setPointer(D);
PropertyOrGetter.setInt(false);
SetterAndMethodRefFlags.setPointer(nullptr);
SetterAndMethodRefFlags.setInt(methRefFlags);
}

void setImplicitProperty(ObjCMethodDecl *Getter, ObjCMethodDecl *Setter,
unsigned methRefFlags) {
PropertyOrGetter.setPointer(Getter);
PropertyOrGetter.setInt(true);
SetterAndMethodRefFlags.setPointer(Setter);
SetterAndMethodRefFlags.setInt(methRefFlags);
}

void setBase(Expr *Base) { Receiver = Base; }
void setSuperReceiver(QualType T) { Receiver = T.getTypePtr(); }
void setClassReceiver(ObjCInterfaceDecl *D) { Receiver = D; }

void setLocation(SourceLocation L) { IdLoc = L; }
void setReceiverLocation(SourceLocation Loc) { ReceiverLoc = Loc; }

void setMethodRefFlag(MethodRefFlags flag, bool val) {
unsigned f = SetterAndMethodRefFlags.getInt();
if (val)
f |= flag;
else
f &= ~flag;
SetterAndMethodRefFlags.setInt(f);
}
};



class ObjCSubscriptRefExpr : public Expr {

SourceLocation RBracket;




enum { BASE, KEY, END_EXPR };
Stmt* SubExprs[END_EXPR];

ObjCMethodDecl *GetAtIndexMethodDecl;



ObjCMethodDecl *SetAtIndexMethodDecl;

public:
ObjCSubscriptRefExpr(Expr *base, Expr *key, QualType T, ExprValueKind VK,
ExprObjectKind OK, ObjCMethodDecl *getMethod,
ObjCMethodDecl *setMethod, SourceLocation RB)
: Expr(ObjCSubscriptRefExprClass, T, VK, OK), RBracket(RB),
GetAtIndexMethodDecl(getMethod), SetAtIndexMethodDecl(setMethod) {
SubExprs[BASE] = base;
SubExprs[KEY] = key;
setDependence(computeDependence(this));
}

explicit ObjCSubscriptRefExpr(EmptyShell Empty)
: Expr(ObjCSubscriptRefExprClass, Empty) {}

SourceLocation getRBracket() const { return RBracket; }
void setRBracket(SourceLocation RB) { RBracket = RB; }

SourceLocation getBeginLoc() const LLVM_READONLY {
return SubExprs[BASE]->getBeginLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY { return RBracket; }

Expr *getBaseExpr() const { return cast<Expr>(SubExprs[BASE]); }
void setBaseExpr(Stmt *S) { SubExprs[BASE] = S; }

Expr *getKeyExpr() const { return cast<Expr>(SubExprs[KEY]); }
void setKeyExpr(Stmt *S) { SubExprs[KEY] = S; }

ObjCMethodDecl *getAtIndexMethodDecl() const {
return GetAtIndexMethodDecl;
}

ObjCMethodDecl *setAtIndexMethodDecl() const {
return SetAtIndexMethodDecl;
}

bool isArraySubscriptRefExpr() const {
return getKeyExpr()->getType()->isIntegralOrEnumerationType();
}

child_range children() {
return child_range(SubExprs, SubExprs+END_EXPR);
}

const_child_range children() const {
return const_child_range(SubExprs, SubExprs + END_EXPR);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCSubscriptRefExprClass;
}

private:
friend class ASTStmtReader;
};



























class ObjCMessageExpr final
: public Expr,
private llvm::TrailingObjects<ObjCMessageExpr, void *, SourceLocation> {



uintptr_t SelectorOrMethod = 0;

enum { NumArgsBitWidth = 16 };



unsigned NumArgs : NumArgsBitWidth;





unsigned Kind : 8;






unsigned HasMethod : 1;



unsigned IsDelegateInitCall : 1;



unsigned IsImplicit : 1;



unsigned SelLocsKind : 2;



SourceLocation SuperLoc;



SourceLocation LBracLoc, RBracLoc;

ObjCMessageExpr(EmptyShell Empty, unsigned NumArgs)
: Expr(ObjCMessageExprClass, Empty), Kind(0), HasMethod(false),
IsDelegateInitCall(false), IsImplicit(false), SelLocsKind(0) {
setNumArgs(NumArgs);
}

ObjCMessageExpr(QualType T, ExprValueKind VK,
SourceLocation LBracLoc,
SourceLocation SuperLoc,
bool IsInstanceSuper,
QualType SuperType,
Selector Sel,
ArrayRef<SourceLocation> SelLocs,
SelectorLocationsKind SelLocsK,
ObjCMethodDecl *Method,
ArrayRef<Expr *> Args,
SourceLocation RBracLoc,
bool isImplicit);
ObjCMessageExpr(QualType T, ExprValueKind VK,
SourceLocation LBracLoc,
TypeSourceInfo *Receiver,
Selector Sel,
ArrayRef<SourceLocation> SelLocs,
SelectorLocationsKind SelLocsK,
ObjCMethodDecl *Method,
ArrayRef<Expr *> Args,
SourceLocation RBracLoc,
bool isImplicit);
ObjCMessageExpr(QualType T, ExprValueKind VK,
SourceLocation LBracLoc,
Expr *Receiver,
Selector Sel,
ArrayRef<SourceLocation> SelLocs,
SelectorLocationsKind SelLocsK,
ObjCMethodDecl *Method,
ArrayRef<Expr *> Args,
SourceLocation RBracLoc,
bool isImplicit);

size_t numTrailingObjects(OverloadToken<void *>) const { return NumArgs + 1; }

void setNumArgs(unsigned Num) {
assert((Num >> NumArgsBitWidth) == 0 && "Num of args is out of range!");
NumArgs = Num;
}

void initArgsAndSelLocs(ArrayRef<Expr *> Args,
ArrayRef<SourceLocation> SelLocs,
SelectorLocationsKind SelLocsK);


void *getReceiverPointer() const { return *getTrailingObjects<void *>(); }


void setReceiverPointer(void *Value) {
*getTrailingObjects<void *>() = Value;
}

SelectorLocationsKind getSelLocsKind() const {
return (SelectorLocationsKind)SelLocsKind;
}

bool hasStandardSelLocs() const {
return getSelLocsKind() != SelLoc_NonStandard;
}



SourceLocation *getStoredSelLocs() {
return getTrailingObjects<SourceLocation>();
}
const SourceLocation *getStoredSelLocs() const {
return getTrailingObjects<SourceLocation>();
}



unsigned getNumStoredSelLocs() const {
if (hasStandardSelLocs())
return 0;
return getNumSelectorLocs();
}

static ObjCMessageExpr *alloc(const ASTContext &C,
ArrayRef<Expr *> Args,
SourceLocation RBraceLoc,
ArrayRef<SourceLocation> SelLocs,
Selector Sel,
SelectorLocationsKind &SelLocsK);
static ObjCMessageExpr *alloc(const ASTContext &C,
unsigned NumArgs,
unsigned NumStoredSelLocs);

public:
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend TrailingObjects;


enum ReceiverKind {

Class = 0,


Instance,


SuperClass,


SuperInstance
};


























static ObjCMessageExpr *Create(const ASTContext &Context, QualType T,
ExprValueKind VK,
SourceLocation LBracLoc,
SourceLocation SuperLoc,
bool IsInstanceSuper,
QualType SuperType,
Selector Sel,
ArrayRef<SourceLocation> SelLocs,
ObjCMethodDecl *Method,
ArrayRef<Expr *> Args,
SourceLocation RBracLoc,
bool isImplicit);
























static ObjCMessageExpr *Create(const ASTContext &Context, QualType T,
ExprValueKind VK,
SourceLocation LBracLoc,
TypeSourceInfo *Receiver,
Selector Sel,
ArrayRef<SourceLocation> SelLocs,
ObjCMethodDecl *Method,
ArrayRef<Expr *> Args,
SourceLocation RBracLoc,
bool isImplicit);
























static ObjCMessageExpr *Create(const ASTContext &Context, QualType T,
ExprValueKind VK,
SourceLocation LBracLoc,
Expr *Receiver,
Selector Sel,
ArrayRef<SourceLocation> SeLocs,
ObjCMethodDecl *Method,
ArrayRef<Expr *> Args,
SourceLocation RBracLoc,
bool isImplicit);








static ObjCMessageExpr *CreateEmpty(const ASTContext &Context,
unsigned NumArgs,
unsigned NumStoredSelLocs);




bool isImplicit() const { return IsImplicit; }



ReceiverKind getReceiverKind() const { return (ReceiverKind)Kind; }






QualType getCallReturnType(ASTContext &Ctx) const;


SourceRange getReceiverRange() const;



bool isInstanceMessage() const {
return getReceiverKind() == Instance || getReceiverKind() == SuperInstance;
}



bool isClassMessage() const {
return getReceiverKind() == Class || getReceiverKind() == SuperClass;
}



Expr *getInstanceReceiver() {
if (getReceiverKind() == Instance)
return static_cast<Expr *>(getReceiverPointer());

return nullptr;
}
const Expr *getInstanceReceiver() const {
return const_cast<ObjCMessageExpr*>(this)->getInstanceReceiver();
}



void setInstanceReceiver(Expr *rec) {
Kind = Instance;
setReceiverPointer(rec);
}



QualType getClassReceiver() const {
if (TypeSourceInfo *TSInfo = getClassReceiverTypeInfo())
return TSInfo->getType();

return {};
}



TypeSourceInfo *getClassReceiverTypeInfo() const {
if (getReceiverKind() == Class)
return reinterpret_cast<TypeSourceInfo *>(getReceiverPointer());
return nullptr;
}

void setClassReceiver(TypeSourceInfo *TSInfo) {
Kind = Class;
setReceiverPointer(TSInfo);
}



SourceLocation getSuperLoc() const {
if (getReceiverKind() == SuperInstance || getReceiverKind() == SuperClass)
return SuperLoc;

return SourceLocation();
}










QualType getReceiverType() const;











ObjCInterfaceDecl *getReceiverInterface() const;






QualType getSuperType() const {
if (getReceiverKind() == SuperInstance || getReceiverKind() == SuperClass)
return QualType::getFromOpaquePtr(getReceiverPointer());

return QualType();
}

void setSuper(SourceLocation Loc, QualType T, bool IsInstanceSuper) {
Kind = IsInstanceSuper? SuperInstance : SuperClass;
SuperLoc = Loc;
setReceiverPointer(T.getAsOpaquePtr());
}

Selector getSelector() const;

void setSelector(Selector S) {
HasMethod = false;
SelectorOrMethod = reinterpret_cast<uintptr_t>(S.getAsOpaquePtr());
}

const ObjCMethodDecl *getMethodDecl() const {
if (HasMethod)
return reinterpret_cast<const ObjCMethodDecl *>(SelectorOrMethod);

return nullptr;
}

ObjCMethodDecl *getMethodDecl() {
if (HasMethod)
return reinterpret_cast<ObjCMethodDecl *>(SelectorOrMethod);

return nullptr;
}

void setMethodDecl(ObjCMethodDecl *MD) {
HasMethod = true;
SelectorOrMethod = reinterpret_cast<uintptr_t>(MD);
}

ObjCMethodFamily getMethodFamily() const {
if (HasMethod) return getMethodDecl()->getMethodFamily();
return getSelector().getMethodFamily();
}



unsigned getNumArgs() const { return NumArgs; }



Expr **getArgs() {
return reinterpret_cast<Expr **>(getTrailingObjects<void *>() + 1);
}
const Expr * const *getArgs() const {
return reinterpret_cast<const Expr *const *>(getTrailingObjects<void *>() +
1);
}


Expr *getArg(unsigned Arg) {
assert(Arg < NumArgs && "Arg access out of range!");
return getArgs()[Arg];
}
const Expr *getArg(unsigned Arg) const {
assert(Arg < NumArgs && "Arg access out of range!");
return getArgs()[Arg];
}


void setArg(unsigned Arg, Expr *ArgExpr) {
assert(Arg < NumArgs && "Arg access out of range!");
getArgs()[Arg] = ArgExpr;
}




bool isDelegateInitCall() const { return IsDelegateInitCall; }
void setDelegateInitCall(bool isDelegate) { IsDelegateInitCall = isDelegate; }

SourceLocation getLeftLoc() const { return LBracLoc; }
SourceLocation getRightLoc() const { return RBracLoc; }

SourceLocation getSelectorStartLoc() const {
if (isImplicit())
return getBeginLoc();
return getSelectorLoc(0);
}

SourceLocation getSelectorLoc(unsigned Index) const {
assert(Index < getNumSelectorLocs() && "Index out of range!");
if (hasStandardSelLocs())
return getStandardSelectorLoc(Index, getSelector(),
getSelLocsKind() == SelLoc_StandardWithSpace,
llvm::makeArrayRef(const_cast<Expr**>(getArgs()),
getNumArgs()),
RBracLoc);
return getStoredSelLocs()[Index];
}

void getSelectorLocs(SmallVectorImpl<SourceLocation> &SelLocs) const;

unsigned getNumSelectorLocs() const {
if (isImplicit())
return 0;
Selector Sel = getSelector();
if (Sel.isUnarySelector())
return 1;
return Sel.getNumArgs();
}

void setSourceRange(SourceRange R) {
LBracLoc = R.getBegin();
RBracLoc = R.getEnd();
}

SourceLocation getBeginLoc() const LLVM_READONLY { return LBracLoc; }
SourceLocation getEndLoc() const LLVM_READONLY { return RBracLoc; }


child_range children();

const_child_range children() const;

using arg_iterator = ExprIterator;
using const_arg_iterator = ConstExprIterator;

llvm::iterator_range<arg_iterator> arguments() {
return llvm::make_range(arg_begin(), arg_end());
}

llvm::iterator_range<const_arg_iterator> arguments() const {
return llvm::make_range(arg_begin(), arg_end());
}

arg_iterator arg_begin() { return reinterpret_cast<Stmt **>(getArgs()); }

arg_iterator arg_end() {
return reinterpret_cast<Stmt **>(getArgs() + NumArgs);
}

const_arg_iterator arg_begin() const {
return reinterpret_cast<Stmt const * const*>(getArgs());
}

const_arg_iterator arg_end() const {
return reinterpret_cast<Stmt const * const*>(getArgs() + NumArgs);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCMessageExprClass;
}
};



class ObjCIsaExpr : public Expr {

Stmt *Base;


SourceLocation IsaMemberLoc;


SourceLocation OpLoc;


bool IsArrow;

public:
ObjCIsaExpr(Expr *base, bool isarrow, SourceLocation l, SourceLocation oploc,
QualType ty)
: Expr(ObjCIsaExprClass, ty, VK_LValue, OK_Ordinary), Base(base),
IsaMemberLoc(l), OpLoc(oploc), IsArrow(isarrow) {
setDependence(computeDependence(this));
}


explicit ObjCIsaExpr(EmptyShell Empty) : Expr(ObjCIsaExprClass, Empty) {}

void setBase(Expr *E) { Base = E; }
Expr *getBase() const { return cast<Expr>(Base); }

bool isArrow() const { return IsArrow; }
void setArrow(bool A) { IsArrow = A; }



SourceLocation getIsaMemberLoc() const { return IsaMemberLoc; }
void setIsaMemberLoc(SourceLocation L) { IsaMemberLoc = L; }

SourceLocation getOpLoc() const { return OpLoc; }
void setOpLoc(SourceLocation L) { OpLoc = L; }

SourceLocation getBeginLoc() const LLVM_READONLY {
return getBase()->getBeginLoc();
}

SourceLocation getBaseLocEnd() const LLVM_READONLY {
return getBase()->getEndLoc();
}

SourceLocation getEndLoc() const LLVM_READONLY { return IsaMemberLoc; }

SourceLocation getExprLoc() const LLVM_READONLY { return IsaMemberLoc; }


child_range children() { return child_range(&Base, &Base+1); }

const_child_range children() const {
return const_child_range(&Base, &Base + 1);
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCIsaExprClass;
}
};























class ObjCIndirectCopyRestoreExpr : public Expr {
friend class ASTReader;
friend class ASTStmtReader;

Stmt *Operand;



explicit ObjCIndirectCopyRestoreExpr(EmptyShell Empty)
: Expr(ObjCIndirectCopyRestoreExprClass, Empty) {}

void setShouldCopy(bool shouldCopy) {
ObjCIndirectCopyRestoreExprBits.ShouldCopy = shouldCopy;
}

public:
ObjCIndirectCopyRestoreExpr(Expr *operand, QualType type, bool shouldCopy)
: Expr(ObjCIndirectCopyRestoreExprClass, type, VK_LValue, OK_Ordinary),
Operand(operand) {
setShouldCopy(shouldCopy);
setDependence(computeDependence(this));
}

Expr *getSubExpr() { return cast<Expr>(Operand); }
const Expr *getSubExpr() const { return cast<Expr>(Operand); }



bool shouldCopy() const { return ObjCIndirectCopyRestoreExprBits.ShouldCopy; }

child_range children() { return child_range(&Operand, &Operand+1); }

const_child_range children() const {
return const_child_range(&Operand, &Operand + 1);
}


SourceLocation getBeginLoc() const LLVM_READONLY {
return Operand->getBeginLoc();
}
SourceLocation getEndLoc() const LLVM_READONLY {
return Operand->getEndLoc();
}

SourceLocation getExprLoc() const LLVM_READONLY {
return getSubExpr()->getExprLoc();
}

static bool classof(const Stmt *s) {
return s->getStmtClass() == ObjCIndirectCopyRestoreExprClass;
}
};







class ObjCBridgedCastExpr final
: public ExplicitCastExpr,
private llvm::TrailingObjects<ObjCBridgedCastExpr, CXXBaseSpecifier *> {
friend class ASTStmtReader;
friend class ASTStmtWriter;
friend class CastExpr;
friend TrailingObjects;

SourceLocation LParenLoc;
SourceLocation BridgeKeywordLoc;
unsigned Kind : 2;

public:
ObjCBridgedCastExpr(SourceLocation LParenLoc, ObjCBridgeCastKind Kind,
CastKind CK, SourceLocation BridgeKeywordLoc,
TypeSourceInfo *TSInfo, Expr *Operand)
: ExplicitCastExpr(ObjCBridgedCastExprClass, TSInfo->getType(),
VK_PRValue, CK, Operand, 0, false, TSInfo),
LParenLoc(LParenLoc), BridgeKeywordLoc(BridgeKeywordLoc), Kind(Kind) {}


explicit ObjCBridgedCastExpr(EmptyShell Shell)
: ExplicitCastExpr(ObjCBridgedCastExprClass, Shell, 0, false) {}

SourceLocation getLParenLoc() const { return LParenLoc; }


ObjCBridgeCastKind getBridgeKind() const {
return static_cast<ObjCBridgeCastKind>(Kind);
}


StringRef getBridgeKindName() const;


SourceLocation getBridgeKeywordLoc() const { return BridgeKeywordLoc; }

SourceLocation getBeginLoc() const LLVM_READONLY { return LParenLoc; }

SourceLocation getEndLoc() const LLVM_READONLY {
return getSubExpr()->getEndLoc();
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCBridgedCastExprClass;
}
};














class ObjCAvailabilityCheckExpr : public Expr {
friend class ASTStmtReader;

VersionTuple VersionToCheck;
SourceLocation AtLoc, RParen;

public:
ObjCAvailabilityCheckExpr(VersionTuple VersionToCheck, SourceLocation AtLoc,
SourceLocation RParen, QualType Ty)
: Expr(ObjCAvailabilityCheckExprClass, Ty, VK_PRValue, OK_Ordinary),
VersionToCheck(VersionToCheck), AtLoc(AtLoc), RParen(RParen) {
setDependence(ExprDependence::None);
}

explicit ObjCAvailabilityCheckExpr(EmptyShell Shell)
: Expr(ObjCAvailabilityCheckExprClass, Shell) {}

SourceLocation getBeginLoc() const { return AtLoc; }
SourceLocation getEndLoc() const { return RParen; }
SourceRange getSourceRange() const { return {AtLoc, RParen}; }


bool hasVersion() const { return !VersionToCheck.empty(); }
VersionTuple getVersion() { return VersionToCheck; }

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const Stmt *T) {
return T->getStmtClass() == ObjCAvailabilityCheckExprClass;
}
};

}

#endif
