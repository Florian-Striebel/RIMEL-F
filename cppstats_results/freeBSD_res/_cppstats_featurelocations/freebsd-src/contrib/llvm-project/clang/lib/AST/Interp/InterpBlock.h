











#if !defined(LLVM_CLANG_AST_INTERP_BLOCK_H)
#define LLVM_CLANG_AST_INTERP_BLOCK_H

#include "Descriptor.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ComparisonCategories.h"
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




class Block {
public:

Block(const llvm::Optional<unsigned> &DeclID, Descriptor *Desc,
bool IsStatic = false, bool IsExtern = false)
: DeclID(DeclID), IsStatic(IsStatic), IsExtern(IsExtern), Desc(Desc) {}

Block(Descriptor *Desc, bool IsStatic = false, bool IsExtern = false)
: DeclID((unsigned)-1), IsStatic(IsStatic), IsExtern(IsExtern),
Desc(Desc) {}


Descriptor *getDescriptor() const { return Desc; }

bool hasPointers() const { return Pointers; }

bool isExtern() const { return IsExtern; }

bool isStatic() const { return IsStatic; }

bool isTemporary() const { return Desc->IsTemporary; }

InterpSize getSize() const { return Desc->getAllocSize(); }

llvm::Optional<unsigned> getDeclID() const { return DeclID; }


char *data() { return reinterpret_cast<char *>(this + 1); }


template <typename T>
T &deref() { return *reinterpret_cast<T *>(data()); }


void invokeCtor() {
std::memset(data(), 0, getSize());
if (Desc->CtorFn)
Desc->CtorFn(this, data(), Desc->IsConst, Desc->IsMutable,
true, Desc);
}

protected:
friend class Pointer;
friend class DeadBlock;
friend class InterpState;

Block(Descriptor *Desc, bool IsExtern, bool IsStatic, bool IsDead)
: IsStatic(IsStatic), IsExtern(IsExtern), IsDead(true), Desc(Desc) {}


void cleanup();


void addPointer(Pointer *P);
void removePointer(Pointer *P);
void movePointer(Pointer *From, Pointer *To);


Pointer *Pointers = nullptr;

llvm::Optional<unsigned> DeclID;

bool IsStatic = false;

bool IsExtern = false;

bool IsDead = false;

Descriptor *Desc;
};





class DeadBlock {
public:

DeadBlock(DeadBlock *&Root, Block *Blk);


char *data() { return B.data(); }

private:
friend class Block;
friend class InterpState;

void free();


DeadBlock *&Root;

DeadBlock *Prev;

DeadBlock *Next;


Block B;
};

}
}

#endif
