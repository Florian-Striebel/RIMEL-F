











#if !defined(LLVM_CLANG_AST_INTERP_POINTER_H)
#define LLVM_CLANG_AST_INTERP_POINTER_H

#include "Descriptor.h"
#include "InterpBlock.h"
#include "clang/AST/ComparisonCategories.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
namespace interp {
class Block;
class DeadBlock;
class Context;
class InterpState;
class Pointer;
class Function;
enum PrimType : unsigned;





class Pointer {
private:
static constexpr unsigned PastEndMark = (unsigned)-1;
static constexpr unsigned RootPtrMark = (unsigned)-1;

public:
Pointer() {}
Pointer(Block *B);
Pointer(const Pointer &P);
Pointer(Pointer &&P);
~Pointer();

void operator=(const Pointer &P);
void operator=(Pointer &&P);


APValue toAPValue() const;


Pointer atIndex(unsigned Idx) const {
if (Base == RootPtrMark)
return Pointer(Pointee, RootPtrMark, getDeclDesc()->getSize());
unsigned Off = Idx * elemSize();
if (getFieldDesc()->ElemDesc)
Off += sizeof(InlineDescriptor);
else
Off += sizeof(InitMap *);
return Pointer(Pointee, Base, Base + Off);
}


Pointer atField(unsigned Off) const {
unsigned Field = Offset + Off;
return Pointer(Pointee, Field, Field);
}


Pointer narrow() const {

if (isZero() || isUnknownSizeArray())
return *this;


if (Base == RootPtrMark)
return Pointer(Pointee, 0, Offset == 0 ? Offset : PastEndMark);


if (isOnePastEnd())
return Pointer(Pointee, Base, PastEndMark);





if (inPrimitiveArray()) {
if (Offset != Base)
return *this;
return Pointer(Pointee, Base, Offset + sizeof(InitMap *));
}


if (Offset != Base)
return Pointer(Pointee, Offset, Offset);


if (!getFieldDesc()->isArray())
return *this;

const unsigned NewBase = Base + sizeof(InlineDescriptor);
return Pointer(Pointee, NewBase, NewBase);
}


Pointer expand() const {
if (isElementPastEnd()) {

unsigned Adjust;
if (inPrimitiveArray())
Adjust = sizeof(InitMap *);
else
Adjust = sizeof(InlineDescriptor);
return Pointer(Pointee, Base, Base + getSize() + Adjust);
}


if (Base != Offset)
return *this;


if (Base == 0)
return Pointer(Pointee, RootPtrMark, 0);


unsigned Next = Base - getInlineDesc()->Offset;
Descriptor *Desc = Next == 0 ? getDeclDesc() : getDescriptor(Next)->Desc;
if (!Desc->IsArray)
return *this;
return Pointer(Pointee, Next, Offset);
}


bool isZero() const { return Pointee == nullptr; }

bool isLive() const { return Pointee && !Pointee->IsDead; }

bool isField() const { return Base != 0 && Base != RootPtrMark; }


Descriptor *getDeclDesc() const { return Pointee->Desc; }
SourceLocation getDeclLoc() const { return getDeclDesc()->getLocation(); }


Pointer getBase() const {
if (Base == RootPtrMark) {
assert(Offset == PastEndMark && "cannot get base of a block");
return Pointer(Pointee, Base, 0);
}
assert(Offset == Base && "not an inner field");
unsigned NewBase = Base - getInlineDesc()->Offset;
return Pointer(Pointee, NewBase, NewBase);
}

Pointer getArray() const {
if (Base == RootPtrMark) {
assert(Offset != 0 && Offset != PastEndMark && "not an array element");
return Pointer(Pointee, Base, 0);
}
assert(Offset != Base && "not an array element");
return Pointer(Pointee, Base, Base);
}


Descriptor *getFieldDesc() const {
if (Base == 0 || Base == RootPtrMark)
return getDeclDesc();
return getInlineDesc()->Desc;
}


QualType getType() const { return getFieldDesc()->getType(); }


size_t elemSize() const {
if (Base == RootPtrMark)
return getDeclDesc()->getSize();
return getFieldDesc()->getElemSize();
}

size_t getSize() const { return getFieldDesc()->getSize(); }


unsigned getOffset() const {
assert(Offset != PastEndMark && "invalid offset");
if (Base == RootPtrMark)
return Offset;

unsigned Adjust = 0;
if (Offset != Base) {
if (getFieldDesc()->ElemDesc)
Adjust = sizeof(InlineDescriptor);
else
Adjust = sizeof(InitMap *);
}
return Offset - Base - Adjust;
}


bool inArray() const { return getFieldDesc()->IsArray; }

bool inPrimitiveArray() const { return getFieldDesc()->isPrimitiveArray(); }

bool isUnknownSizeArray() const {
return getFieldDesc()->isUnknownSizeArray();
}

bool isArrayElement() const { return Base != Offset; }

bool isRoot() const {
return (Base == 0 || Base == RootPtrMark) && Offset == 0;
}


Record *getRecord() const { return getFieldDesc()->ElemRecord; }

const FieldDecl *getField() const { return getFieldDesc()->asFieldDecl(); }


bool isUnion() const;


bool isExtern() const { return Pointee->isExtern(); }

bool isStatic() const { return Pointee->isStatic(); }

bool isTemporary() const { return Pointee->isTemporary(); }

bool isStaticTemporary() const { return isStatic() && isTemporary(); }


bool isMutable() const { return Base != 0 && getInlineDesc()->IsMutable; }

bool isInitialized() const;

bool isActive() const { return Base == 0 || getInlineDesc()->IsActive; }

bool isBaseClass() const { return isField() && getInlineDesc()->IsBase; }


bool isConst() const {
return Base == 0 ? getDeclDesc()->IsConst : getInlineDesc()->IsConst;
}


llvm::Optional<unsigned> getDeclID() const { return Pointee->getDeclID(); }


unsigned getByteOffset() const {
return Offset;
}


unsigned getNumElems() const { return getSize() / elemSize(); }


int64_t getIndex() const {
if (isElementPastEnd())
return 1;
if (auto ElemSize = elemSize())
return getOffset() / ElemSize;
return 0;
}


bool isOnePastEnd() const {
return isElementPastEnd() || getSize() == getOffset();
}


bool isElementPastEnd() const { return Offset == PastEndMark; }


template <typename T> T &deref() const {
assert(isLive() && "Invalid pointer");
return *reinterpret_cast<T *>(Pointee->data() + Offset);
}


template <typename T> T &elem(unsigned I) const {
return reinterpret_cast<T *>(Pointee->data())[I];
}


void initialize() const;

void activate() const;

void deactivate() const;


static bool hasSameBase(const Pointer &A, const Pointer &B);

static bool hasSameArray(const Pointer &A, const Pointer &B);


void print(llvm::raw_ostream &OS) const {
OS << "{" << Base << ", " << Offset << ", ";
if (Pointee)
OS << Pointee->getSize();
else
OS << "nullptr";
OS << "}";
}

private:
friend class Block;
friend class DeadBlock;

Pointer(Block *Pointee, unsigned Base, unsigned Offset);


InlineDescriptor *getInlineDesc() const { return getDescriptor(Base); }


InlineDescriptor *getDescriptor(unsigned Offset) const {
assert(Offset != 0 && "Not a nested pointer");
return reinterpret_cast<InlineDescriptor *>(Pointee->data() + Offset) - 1;
}


InitMap *&getInitMap() const {
return *reinterpret_cast<InitMap **>(Pointee->data() + Base);
}


Block *Pointee = nullptr;

unsigned Base = 0;

unsigned Offset = 0;


Pointer *Prev = nullptr;

Pointer *Next = nullptr;
};

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const Pointer &P) {
P.print(OS);
return OS;
}

}
}

#endif
