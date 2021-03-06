











#if !defined(LLVM_CLANG_AST_INTERP_PROGRAM_H)
#define LLVM_CLANG_AST_INTERP_PROGRAM_H

#include <map>
#include <vector>
#include "Function.h"
#include "Pointer.h"
#include "PrimType.h"
#include "Record.h"
#include "Source.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"

namespace clang {
class RecordDecl;
class Expr;
class FunctionDecl;
class Stmt;
class StringLiteral;
class VarDecl;

namespace interp {
class Context;
class State;
class Record;
class Scope;


class Program {
public:
Program(Context &Ctx) : Ctx(Ctx) {}


unsigned createGlobalString(const StringLiteral *S);


Pointer getPtrGlobal(unsigned Idx);


Block *getGlobal(unsigned Idx) {
assert(Idx < Globals.size());
return Globals[Idx]->block();
}


llvm::Optional<unsigned> getGlobal(const ValueDecl *VD);


llvm::Optional<unsigned> getOrCreateGlobal(const ValueDecl *VD);


llvm::Optional<unsigned> getOrCreateDummy(const ParmVarDecl *PD);


llvm::Optional<unsigned> createGlobal(const ValueDecl *VD);


llvm::Optional<unsigned> createGlobal(const Expr *E);


template <typename... Ts>
Function *createFunction(const FunctionDecl *Def, Ts &&... Args) {
auto *Func = new Function(*this, Def, std::forward<Ts>(Args)...);
Funcs.insert({Def, std::unique_ptr<Function>(Func)});
return Func;
}

template <typename... Ts>
Function *createFunction(Ts &&... Args) {
auto *Func = new Function(*this, std::forward<Ts>(Args)...);
AnonFuncs.emplace_back(Func);
return Func;
}


Function *getFunction(const FunctionDecl *F);




llvm::Expected<Function *> getOrCreateFunction(const FunctionDecl *F);


Record *getOrCreateRecord(const RecordDecl *RD);


Descriptor *createDescriptor(const DeclTy &D, PrimType Type,
bool IsConst = false,
bool IsTemporary = false,
bool IsMutable = false) {
return allocateDescriptor(D, Type, IsConst, IsTemporary, IsMutable);
}


Descriptor *createDescriptor(const DeclTy &D, const Type *Ty,
bool IsConst = false, bool IsTemporary = false,
bool IsMutable = false);


class DeclScope {
public:
DeclScope(Program &P, const VarDecl *VD) : P(P) { P.startDeclaration(VD); }
~DeclScope() { P.endDeclaration(); }

private:
Program &P;
};


llvm::Optional<unsigned> getCurrentDecl() const {
if (CurrentDeclaration == NoDeclaration)
return llvm::Optional<unsigned>{};
return LastDeclaration;
}

private:
friend class DeclScope;

llvm::Optional<unsigned> createGlobal(const DeclTy &D, QualType Ty,
bool IsStatic, bool IsExtern);


Context &Ctx;

llvm::DenseMap<const FunctionDecl *, std::unique_ptr<Function>> Funcs;

std::vector<std::unique_ptr<Function>> AnonFuncs;


llvm::DenseMap<const FunctionDecl *, std::vector<unsigned>> Relocs;


using PoolAllocTy = llvm::BumpPtrAllocatorImpl<llvm::MallocAllocator>;




class Global {
public:

template <typename... Tys>
Global(Tys... Args) : B(std::forward<Tys>(Args)...) {}


void *operator new(size_t Meta, PoolAllocTy &Alloc, size_t Data) {
return Alloc.Allocate(Meta + Data, alignof(void *));
}


char *data() { return B.data(); }

Block *block() { return &B; }

private:

Block B;
};


PoolAllocTy Allocator;


std::vector<Global *> Globals;

llvm::DenseMap<const void *, unsigned> GlobalIndices;


llvm::DenseMap<const RecordDecl *, Record *> Records;


llvm::DenseMap<const ParmVarDecl *, unsigned> DummyParams;


template <typename... Ts>
Descriptor *allocateDescriptor(Ts &&... Args) {
return new (Allocator) Descriptor(std::forward<Ts>(Args)...);
}


static constexpr unsigned NoDeclaration = (unsigned)-1;

unsigned LastDeclaration = 0;

unsigned CurrentDeclaration = NoDeclaration;


void startDeclaration(const VarDecl *Decl) {
LastDeclaration += 1;
CurrentDeclaration = LastDeclaration;
}


void endDeclaration() {
CurrentDeclaration = NoDeclaration;
}

public:

void dump() const;
void dump(llvm::raw_ostream &OS) const;
};

}
}

#endif
