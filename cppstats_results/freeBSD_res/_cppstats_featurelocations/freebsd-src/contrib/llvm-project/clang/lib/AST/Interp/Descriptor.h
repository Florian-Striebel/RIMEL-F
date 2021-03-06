











#if !defined(LLVM_CLANG_AST_INTERP_DESCRIPTOR_H)
#define LLVM_CLANG_AST_INTERP_DESCRIPTOR_H

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"

namespace clang {
namespace interp {
class Block;
class Record;
struct Descriptor;
enum PrimType : unsigned;

using DeclTy = llvm::PointerUnion<const Decl *, const Expr *>;




using BlockCtorFn = void (*)(Block *Storage, char *FieldPtr, bool IsConst,
bool IsMutable, bool IsActive,
Descriptor *FieldDesc);



using BlockDtorFn = void (*)(Block *Storage, char *FieldPtr,
Descriptor *FieldDesc);





using BlockMoveFn = void (*)(Block *Storage, char *SrcFieldPtr,
char *DstFieldPtr, Descriptor *FieldDesc);


using InterpSize = unsigned;


struct Descriptor {
private:

const DeclTy Source;

const InterpSize ElemSize;

const InterpSize Size;

const InterpSize AllocSize;


static constexpr unsigned UnknownSizeMark = (unsigned)-1;

public:

struct UnknownSize {};


Record *const ElemRecord = nullptr;

Descriptor *const ElemDesc = nullptr;

const bool IsConst = false;

const bool IsMutable = false;

const bool IsTemporary = false;

const bool IsArray = false;


const BlockCtorFn CtorFn = nullptr;
const BlockDtorFn DtorFn = nullptr;
const BlockMoveFn MoveFn = nullptr;


Descriptor(const DeclTy &D, PrimType Type, bool IsConst, bool IsTemporary,
bool IsMutable);


Descriptor(const DeclTy &D, PrimType Type, size_t NumElems, bool IsConst,
bool IsTemporary, bool IsMutable);


Descriptor(const DeclTy &D, PrimType Type, bool IsTemporary, UnknownSize);


Descriptor(const DeclTy &D, Descriptor *Elem, unsigned NumElems, bool IsConst,
bool IsTemporary, bool IsMutable);


Descriptor(const DeclTy &D, Descriptor *Elem, bool IsTemporary, UnknownSize);


Descriptor(const DeclTy &D, Record *R, bool IsConst, bool IsTemporary,
bool IsMutable);

QualType getType() const;
SourceLocation getLocation() const;

const Decl *asDecl() const { return Source.dyn_cast<const Decl *>(); }
const Expr *asExpr() const { return Source.dyn_cast<const Expr *>(); }

const ValueDecl *asValueDecl() const {
return dyn_cast_or_null<ValueDecl>(asDecl());
}

const FieldDecl *asFieldDecl() const {
return dyn_cast_or_null<FieldDecl>(asDecl());
}

const RecordDecl *asRecordDecl() const {
return dyn_cast_or_null<RecordDecl>(asDecl());
}


unsigned getSize() const {
assert(!isUnknownSizeArray() && "Array of unknown size");
return Size;
}


unsigned getAllocSize() const { return AllocSize; }

unsigned getElemSize() const { return ElemSize; }


unsigned getNumElems() const {
return Size == UnknownSizeMark ? 0 : (getSize() / getElemSize());
}


bool isPrimitiveArray() const { return IsArray && !ElemDesc; }

bool isZeroSizeArray() const { return Size == 0; }

bool isUnknownSizeArray() const { return Size == UnknownSizeMark; }


bool isPrimitive() const { return !IsArray && !ElemRecord; }


bool isArray() const { return IsArray; }
};







struct InlineDescriptor {

unsigned Offset;



unsigned IsConst : 1;




unsigned IsInitialized : 1;

unsigned IsBase : 1;

unsigned IsActive : 1;

unsigned IsMutable : 1;

Descriptor *Desc;
};





struct InitMap {
private:

using T = uint64_t;

static constexpr uint64_t PER_FIELD = sizeof(T) * CHAR_BIT;


InitMap(unsigned N);


T *data();

public:

bool initialize(unsigned I);


bool isInitialized(unsigned I);


static InitMap *allocate(unsigned N);

private:

unsigned UninitFields;
};

}
}

#endif
