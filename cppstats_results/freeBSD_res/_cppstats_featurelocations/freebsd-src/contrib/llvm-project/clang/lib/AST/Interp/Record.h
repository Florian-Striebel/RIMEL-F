











#if !defined(LLVM_CLANG_AST_INTERP_RECORD_H)
#define LLVM_CLANG_AST_INTERP_RECORD_H

#include "Pointer.h"

namespace clang {
namespace interp {
class Program;


class Record {
public:

struct Field {
const FieldDecl *Decl;
unsigned Offset;
Descriptor *Desc;
};


struct Base {
const RecordDecl *Decl;
unsigned Offset;
Descriptor *Desc;
Record *R;
};


using FieldList = llvm::SmallVector<Field, 8>;

using BaseList = llvm::SmallVector<Base, 8>;

using VirtualBaseList = llvm::SmallVector<Base, 2>;

public:

const RecordDecl *getDecl() const { return Decl; }

bool isUnion() const { return getDecl()->isUnion(); }

unsigned getSize() const { return BaseSize; }

unsigned getFullSize() const { return BaseSize + VirtualSize; }

const Field *getField(const FieldDecl *FD) const;

const Base *getBase(const RecordDecl *FD) const;

const Base *getVirtualBase(const RecordDecl *RD) const;

using const_field_iter = FieldList::const_iterator;
llvm::iterator_range<const_field_iter> fields() const {
return llvm::make_range(Fields.begin(), Fields.end());
}

unsigned getNumFields() { return Fields.size(); }
Field *getField(unsigned I) { return &Fields[I]; }

using const_base_iter = BaseList::const_iterator;
llvm::iterator_range<const_base_iter> bases() const {
return llvm::make_range(Bases.begin(), Bases.end());
}

unsigned getNumBases() { return Bases.size(); }
Base *getBase(unsigned I) { return &Bases[I]; }

using const_virtual_iter = VirtualBaseList::const_iterator;
llvm::iterator_range<const_virtual_iter> virtual_bases() const {
return llvm::make_range(VirtualBases.begin(), VirtualBases.end());
}

unsigned getNumVirtualBases() { return VirtualBases.size(); }
Base *getVirtualBase(unsigned I) { return &VirtualBases[I]; }

private:

Record(const RecordDecl *, BaseList &&Bases, FieldList &&Fields,
VirtualBaseList &&VirtualBases, unsigned VirtualSize,
unsigned BaseSize);

private:
friend class Program;


const RecordDecl *Decl;

BaseList Bases;

FieldList Fields;

VirtualBaseList VirtualBases;


llvm::DenseMap<const RecordDecl *, Base *> BaseMap;

llvm::DenseMap<const FieldDecl *, Field *> FieldMap;

llvm::DenseMap<const RecordDecl *, Base *> VirtualBaseMap;


unsigned BaseSize;

unsigned VirtualSize;
};

}
}

#endif
