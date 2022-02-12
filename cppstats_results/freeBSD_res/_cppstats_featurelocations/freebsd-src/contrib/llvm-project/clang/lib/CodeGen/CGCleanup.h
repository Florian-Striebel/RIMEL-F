











#if !defined(LLVM_CLANG_LIB_CODEGEN_CGCLEANUP_H)
#define LLVM_CLANG_LIB_CODEGEN_CGCLEANUP_H

#include "EHScopeStack.h"

#include "Address.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"

namespace llvm {
class BasicBlock;
class Value;
class ConstantInt;
class AllocaInst;
}

namespace clang {
class FunctionDecl;
namespace CodeGen {
class CodeGenModule;
class CodeGenFunction;



struct CatchTypeInfo {
llvm::Constant *RTTI;
unsigned Flags;
};


class EHScope {
llvm::BasicBlock *CachedLandingPad;
llvm::BasicBlock *CachedEHDispatchBlock;

EHScopeStack::stable_iterator EnclosingEHScope;

class CommonBitFields {
friend class EHScope;
unsigned Kind : 3;
};
enum { NumCommonBits = 3 };

protected:
class CatchBitFields {
friend class EHCatchScope;
unsigned : NumCommonBits;

unsigned NumHandlers : 32 - NumCommonBits;
};

class CleanupBitFields {
friend class EHCleanupScope;
unsigned : NumCommonBits;


unsigned IsNormalCleanup : 1;


unsigned IsEHCleanup : 1;


unsigned IsActive : 1;


unsigned IsLifetimeMarker : 1;


unsigned TestFlagInNormalCleanup : 1;


unsigned TestFlagInEHCleanup : 1;



unsigned CleanupSize : 12;
};

class FilterBitFields {
friend class EHFilterScope;
unsigned : NumCommonBits;

unsigned NumFilters : 32 - NumCommonBits;
};

union {
CommonBitFields CommonBits;
CatchBitFields CatchBits;
CleanupBitFields CleanupBits;
FilterBitFields FilterBits;
};

public:
enum Kind { Cleanup, Catch, Terminate, Filter };

EHScope(Kind kind, EHScopeStack::stable_iterator enclosingEHScope)
: CachedLandingPad(nullptr), CachedEHDispatchBlock(nullptr),
EnclosingEHScope(enclosingEHScope) {
CommonBits.Kind = kind;
}

Kind getKind() const { return static_cast<Kind>(CommonBits.Kind); }

llvm::BasicBlock *getCachedLandingPad() const {
return CachedLandingPad;
}

void setCachedLandingPad(llvm::BasicBlock *block) {
CachedLandingPad = block;
}

llvm::BasicBlock *getCachedEHDispatchBlock() const {
return CachedEHDispatchBlock;
}

void setCachedEHDispatchBlock(llvm::BasicBlock *block) {
CachedEHDispatchBlock = block;
}

bool hasEHBranches() const {
if (llvm::BasicBlock *block = getCachedEHDispatchBlock())
return !block->use_empty();
return false;
}

EHScopeStack::stable_iterator getEnclosingEHScope() const {
return EnclosingEHScope;
}
};






class EHCatchScope : public EHScope {





public:
struct Handler {


CatchTypeInfo Type;


llvm::BasicBlock *Block;

bool isCatchAll() const { return Type.RTTI == nullptr; }
};

private:
friend class EHScopeStack;

Handler *getHandlers() {
return reinterpret_cast<Handler*>(this+1);
}

const Handler *getHandlers() const {
return reinterpret_cast<const Handler*>(this+1);
}

public:
static size_t getSizeForNumHandlers(unsigned N) {
return sizeof(EHCatchScope) + N * sizeof(Handler);
}

EHCatchScope(unsigned numHandlers,
EHScopeStack::stable_iterator enclosingEHScope)
: EHScope(Catch, enclosingEHScope) {
CatchBits.NumHandlers = numHandlers;
assert(CatchBits.NumHandlers == numHandlers && "NumHandlers overflow?");
}

unsigned getNumHandlers() const {
return CatchBits.NumHandlers;
}

void setCatchAllHandler(unsigned I, llvm::BasicBlock *Block) {
setHandler(I, CatchTypeInfo{nullptr, 0}, Block);
}

void setHandler(unsigned I, llvm::Constant *Type, llvm::BasicBlock *Block) {
assert(I < getNumHandlers());
getHandlers()[I].Type = CatchTypeInfo{Type, 0};
getHandlers()[I].Block = Block;
}

void setHandler(unsigned I, CatchTypeInfo Type, llvm::BasicBlock *Block) {
assert(I < getNumHandlers());
getHandlers()[I].Type = Type;
getHandlers()[I].Block = Block;
}

const Handler &getHandler(unsigned I) const {
assert(I < getNumHandlers());
return getHandlers()[I];
}





void clearHandlerBlocks() {
for (unsigned I = 0, N = getNumHandlers(); I != N; ++I)
delete getHandler(I).Block;
}

typedef const Handler *iterator;
iterator begin() const { return getHandlers(); }
iterator end() const { return getHandlers() + getNumHandlers(); }

static bool classof(const EHScope *Scope) {
return Scope->getKind() == Catch;
}
};


class alignas(8) EHCleanupScope : public EHScope {

EHScopeStack::stable_iterator EnclosingNormal;


EHScopeStack::stable_iterator EnclosingEH;



llvm::BasicBlock *NormalBlock;



llvm::AllocaInst *ActiveFlag;





struct ExtInfo {

llvm::SmallPtrSet<llvm::BasicBlock*, 4> Branches;


SmallVector<std::pair<llvm::BasicBlock*,llvm::ConstantInt*>, 4>
BranchAfters;
};
mutable struct ExtInfo *ExtInfo;




unsigned FixupDepth;

struct ExtInfo &getExtInfo() {
if (!ExtInfo) ExtInfo = new struct ExtInfo();
return *ExtInfo;
}

const struct ExtInfo &getExtInfo() const {
if (!ExtInfo) ExtInfo = new struct ExtInfo();
return *ExtInfo;
}

public:


static size_t getSizeForCleanupSize(size_t Size) {
return sizeof(EHCleanupScope) + Size;
}

size_t getAllocatedSize() const {
return sizeof(EHCleanupScope) + CleanupBits.CleanupSize;
}

EHCleanupScope(bool isNormal, bool isEH, unsigned cleanupSize,
unsigned fixupDepth,
EHScopeStack::stable_iterator enclosingNormal,
EHScopeStack::stable_iterator enclosingEH)
: EHScope(EHScope::Cleanup, enclosingEH),
EnclosingNormal(enclosingNormal), NormalBlock(nullptr),
ActiveFlag(nullptr), ExtInfo(nullptr), FixupDepth(fixupDepth) {
CleanupBits.IsNormalCleanup = isNormal;
CleanupBits.IsEHCleanup = isEH;
CleanupBits.IsActive = true;
CleanupBits.IsLifetimeMarker = false;
CleanupBits.TestFlagInNormalCleanup = false;
CleanupBits.TestFlagInEHCleanup = false;
CleanupBits.CleanupSize = cleanupSize;

assert(CleanupBits.CleanupSize == cleanupSize && "cleanup size overflow");
}

void Destroy() {
delete ExtInfo;
}

~EHCleanupScope() = delete;

bool isNormalCleanup() const { return CleanupBits.IsNormalCleanup; }
llvm::BasicBlock *getNormalBlock() const { return NormalBlock; }
void setNormalBlock(llvm::BasicBlock *BB) { NormalBlock = BB; }

bool isEHCleanup() const { return CleanupBits.IsEHCleanup; }

bool isActive() const { return CleanupBits.IsActive; }
void setActive(bool A) { CleanupBits.IsActive = A; }

bool isLifetimeMarker() const { return CleanupBits.IsLifetimeMarker; }
void setLifetimeMarker() { CleanupBits.IsLifetimeMarker = true; }

bool hasActiveFlag() const { return ActiveFlag != nullptr; }
Address getActiveFlag() const {
return Address(ActiveFlag, CharUnits::One());
}
void setActiveFlag(Address Var) {
assert(Var.getAlignment().isOne());
ActiveFlag = cast<llvm::AllocaInst>(Var.getPointer());
}

void setTestFlagInNormalCleanup() {
CleanupBits.TestFlagInNormalCleanup = true;
}
bool shouldTestFlagInNormalCleanup() const {
return CleanupBits.TestFlagInNormalCleanup;
}

void setTestFlagInEHCleanup() {
CleanupBits.TestFlagInEHCleanup = true;
}
bool shouldTestFlagInEHCleanup() const {
return CleanupBits.TestFlagInEHCleanup;
}

unsigned getFixupDepth() const { return FixupDepth; }
EHScopeStack::stable_iterator getEnclosingNormalCleanup() const {
return EnclosingNormal;
}

size_t getCleanupSize() const { return CleanupBits.CleanupSize; }
void *getCleanupBuffer() { return this + 1; }

EHScopeStack::Cleanup *getCleanup() {
return reinterpret_cast<EHScopeStack::Cleanup*>(getCleanupBuffer());
}


bool hasBranches() const { return ExtInfo && !ExtInfo->Branches.empty(); }












void addBranchAfter(llvm::ConstantInt *Index,
llvm::BasicBlock *Block) {
struct ExtInfo &ExtInfo = getExtInfo();
if (ExtInfo.Branches.insert(Block).second)
ExtInfo.BranchAfters.push_back(std::make_pair(Block, Index));
}


unsigned getNumBranchAfters() const {
return ExtInfo ? ExtInfo->BranchAfters.size() : 0;
}

llvm::BasicBlock *getBranchAfterBlock(unsigned I) const {
assert(I < getNumBranchAfters());
return ExtInfo->BranchAfters[I].first;
}

llvm::ConstantInt *getBranchAfterIndex(unsigned I) const {
assert(I < getNumBranchAfters());
return ExtInfo->BranchAfters[I].second;
}
















bool addBranchThrough(llvm::BasicBlock *Block) {
return getExtInfo().Branches.insert(Block).second;
}


bool hasBranchThroughs() const {
if (!ExtInfo) return false;
return (ExtInfo->BranchAfters.size() != ExtInfo->Branches.size());
}

static bool classof(const EHScope *Scope) {
return (Scope->getKind() == Cleanup);
}
};






static_assert(alignof(EHCleanupScope) == EHScopeStack::ScopeStackAlignment,
"EHCleanupScope expected alignment");






class EHFilterScope : public EHScope {



llvm::Value **getFilters() {
return reinterpret_cast<llvm::Value**>(this+1);
}

llvm::Value * const *getFilters() const {
return reinterpret_cast<llvm::Value* const *>(this+1);
}

public:
EHFilterScope(unsigned numFilters)
: EHScope(Filter, EHScopeStack::stable_end()) {
FilterBits.NumFilters = numFilters;
assert(FilterBits.NumFilters == numFilters && "NumFilters overflow");
}

static size_t getSizeForNumFilters(unsigned numFilters) {
return sizeof(EHFilterScope) + numFilters * sizeof(llvm::Value*);
}

unsigned getNumFilters() const { return FilterBits.NumFilters; }

void setFilter(unsigned i, llvm::Value *filterValue) {
assert(i < getNumFilters());
getFilters()[i] = filterValue;
}

llvm::Value *getFilter(unsigned i) const {
assert(i < getNumFilters());
return getFilters()[i];
}

static bool classof(const EHScope *scope) {
return scope->getKind() == Filter;
}
};



class EHTerminateScope : public EHScope {
public:
EHTerminateScope(EHScopeStack::stable_iterator enclosingEHScope)
: EHScope(Terminate, enclosingEHScope) {}
static size_t getSize() { return sizeof(EHTerminateScope); }

static bool classof(const EHScope *scope) {
return scope->getKind() == Terminate;
}
};


class EHScopeStack::iterator {
char *Ptr;

friend class EHScopeStack;
explicit iterator(char *Ptr) : Ptr(Ptr) {}

public:
iterator() : Ptr(nullptr) {}

EHScope *get() const {
return reinterpret_cast<EHScope*>(Ptr);
}

EHScope *operator->() const { return get(); }
EHScope &operator*() const { return *get(); }

iterator &operator++() {
size_t Size;
switch (get()->getKind()) {
case EHScope::Catch:
Size = EHCatchScope::getSizeForNumHandlers(
static_cast<const EHCatchScope *>(get())->getNumHandlers());
break;

case EHScope::Filter:
Size = EHFilterScope::getSizeForNumFilters(
static_cast<const EHFilterScope *>(get())->getNumFilters());
break;

case EHScope::Cleanup:
Size = static_cast<const EHCleanupScope *>(get())->getAllocatedSize();
break;

case EHScope::Terminate:
Size = EHTerminateScope::getSize();
break;
}
Ptr += llvm::alignTo(Size, ScopeStackAlignment);
return *this;
}

iterator next() {
iterator copy = *this;
++copy;
return copy;
}

iterator operator++(int) {
iterator copy = *this;
operator++();
return copy;
}

bool encloses(iterator other) const { return Ptr >= other.Ptr; }
bool strictlyEncloses(iterator other) const { return Ptr > other.Ptr; }

bool operator==(iterator other) const { return Ptr == other.Ptr; }
bool operator!=(iterator other) const { return Ptr != other.Ptr; }
};

inline EHScopeStack::iterator EHScopeStack::begin() const {
return iterator(StartOfData);
}

inline EHScopeStack::iterator EHScopeStack::end() const {
return iterator(EndOfBuffer);
}

inline void EHScopeStack::popCatch() {
assert(!empty() && "popping exception stack when not empty");

EHCatchScope &scope = cast<EHCatchScope>(*begin());
InnermostEHScope = scope.getEnclosingEHScope();
deallocate(EHCatchScope::getSizeForNumHandlers(scope.getNumHandlers()));
}

inline void EHScopeStack::popTerminate() {
assert(!empty() && "popping exception stack when not empty");

EHTerminateScope &scope = cast<EHTerminateScope>(*begin());
InnermostEHScope = scope.getEnclosingEHScope();
deallocate(EHTerminateScope::getSize());
}

inline EHScopeStack::iterator EHScopeStack::find(stable_iterator sp) const {
assert(sp.isValid() && "finding invalid savepoint");
assert(sp.Size <= stable_begin().Size && "finding savepoint after pop");
return iterator(EndOfBuffer - sp.Size);
}

inline EHScopeStack::stable_iterator
EHScopeStack::stabilize(iterator ir) const {
assert(StartOfData <= ir.Ptr && ir.Ptr <= EndOfBuffer);
return stable_iterator(EndOfBuffer - ir.Ptr);
}


struct EHPersonality {
const char *PersonalityFn;




const char *CatchallRethrowFn;

static const EHPersonality &get(CodeGenModule &CGM, const FunctionDecl *FD);
static const EHPersonality &get(CodeGenFunction &CGF);

static const EHPersonality GNU_C;
static const EHPersonality GNU_C_SJLJ;
static const EHPersonality GNU_C_SEH;
static const EHPersonality GNU_ObjC;
static const EHPersonality GNU_ObjC_SJLJ;
static const EHPersonality GNU_ObjC_SEH;
static const EHPersonality GNUstep_ObjC;
static const EHPersonality GNU_ObjCXX;
static const EHPersonality NeXT_ObjC;
static const EHPersonality GNU_CPlusPlus;
static const EHPersonality GNU_CPlusPlus_SJLJ;
static const EHPersonality GNU_CPlusPlus_SEH;
static const EHPersonality MSVC_except_handler;
static const EHPersonality MSVC_C_specific_handler;
static const EHPersonality MSVC_CxxFrameHandler3;
static const EHPersonality GNU_Wasm_CPlusPlus;
static const EHPersonality XL_CPlusPlus;



bool usesFuncletPads() const {
return isMSVCPersonality() || isWasmPersonality();
}

bool isMSVCPersonality() const {
return this == &MSVC_except_handler || this == &MSVC_C_specific_handler ||
this == &MSVC_CxxFrameHandler3;
}

bool isWasmPersonality() const { return this == &GNU_Wasm_CPlusPlus; }

bool isMSVCXXPersonality() const { return this == &MSVC_CxxFrameHandler3; }
};
}
}

#endif
