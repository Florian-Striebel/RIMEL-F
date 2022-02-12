













#if !defined(LLVM_CLANG_LIB_CODEGEN_EHSCOPESTACK_H)
#define LLVM_CLANG_LIB_CODEGEN_EHSCOPESTACK_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

namespace clang {
namespace CodeGen {

class CodeGenFunction;







struct BranchFixup {



llvm::BasicBlock *OptimisticBranchBlock;





llvm::BasicBlock *Destination;


unsigned DestinationIndex;


llvm::BranchInst *InitialBranch;
};

template <class T> struct InvariantValue {
typedef T type;
typedef T saved_type;
static bool needsSaving(type value) { return false; }
static saved_type save(CodeGenFunction &CGF, type value) { return value; }
static type restore(CodeGenFunction &CGF, saved_type value) { return value; }
};



template <class T> struct DominatingValue : InvariantValue<T> {};

template <class T, bool mightBeInstruction =
std::is_base_of<llvm::Value, T>::value &&
!std::is_base_of<llvm::Constant, T>::value &&
!std::is_base_of<llvm::BasicBlock, T>::value>
struct DominatingPointer;
template <class T> struct DominatingPointer<T,false> : InvariantValue<T*> {};


template <class T> struct DominatingValue<T*> : DominatingPointer<T> {};

enum CleanupKind : unsigned {


EHCleanup = 0x1,



NormalCleanup = 0x2,

NormalAndEHCleanup = EHCleanup | NormalCleanup,

LifetimeMarker = 0x8,
NormalEHLifetimeMarker = LifetimeMarker | NormalAndEHCleanup,
};



class EHScopeStack {
public:

enum { ScopeStackAlignment = 8 };



class stable_iterator {
friend class EHScopeStack;


ptrdiff_t Size;

stable_iterator(ptrdiff_t Size) : Size(Size) {}

public:
static stable_iterator invalid() { return stable_iterator(-1); }
stable_iterator() : Size(-1) {}

bool isValid() const { return Size >= 0; }




bool encloses(stable_iterator I) const { return Size <= I.Size; }





bool strictlyEncloses(stable_iterator I) const { return Size < I.Size; }

friend bool operator==(stable_iterator A, stable_iterator B) {
return A.Size == B.Size;
}
friend bool operator!=(stable_iterator A, stable_iterator B) {
return A.Size != B.Size;
}
};








class Cleanup {

virtual void anchor();

protected:
~Cleanup() = default;

public:
Cleanup(const Cleanup &) = default;
Cleanup(Cleanup &&) {}
Cleanup() = default;

virtual bool isRedundantBeforeReturn() { return false; }


class Flags {
enum {
F_IsForEH = 0x1,
F_IsNormalCleanupKind = 0x2,
F_IsEHCleanupKind = 0x4,
F_HasExitSwitch = 0x8,
};
unsigned flags;

public:
Flags() : flags(0) {}


bool isForEHCleanup() const { return flags & F_IsForEH; }
bool isForNormalCleanup() const { return !isForEHCleanup(); }
void setIsForEHCleanup() { flags |= F_IsForEH; }

bool isNormalCleanupKind() const { return flags & F_IsNormalCleanupKind; }
void setIsNormalCleanupKind() { flags |= F_IsNormalCleanupKind; }



bool isEHCleanupKind() const { return flags & F_IsEHCleanupKind; }
void setIsEHCleanupKind() { flags |= F_IsEHCleanupKind; }

bool hasExitSwitch() const { return flags & F_HasExitSwitch; }
void setHasExitSwitch() { flags |= F_HasExitSwitch; }
};







virtual void Emit(CodeGenFunction &CGF, Flags flags) = 0;
};



template <class T, class... As>
class ConditionalCleanup final : public Cleanup {
typedef std::tuple<typename DominatingValue<As>::saved_type...> SavedTuple;
SavedTuple Saved;

template <std::size_t... Is>
T restore(CodeGenFunction &CGF, std::index_sequence<Is...>) {


return T{DominatingValue<As>::restore(CGF, std::get<Is>(Saved))...};
}

void Emit(CodeGenFunction &CGF, Flags flags) override {
restore(CGF, std::index_sequence_for<As...>()).Emit(CGF, flags);
}

public:
ConditionalCleanup(typename DominatingValue<As>::saved_type... A)
: Saved(A...) {}

ConditionalCleanup(SavedTuple Tuple) : Saved(std::move(Tuple)) {}
};

private:







char *StartOfBuffer;


char *EndOfBuffer;


char *StartOfData;


stable_iterator InnermostNormalCleanup;


stable_iterator InnermostEHScope;


CodeGenFunction* CGF;


















SmallVector<BranchFixup, 8> BranchFixups;

char *allocate(size_t Size);
void deallocate(size_t Size);

void *pushCleanup(CleanupKind K, size_t DataSize);

public:
EHScopeStack()
: StartOfBuffer(nullptr), EndOfBuffer(nullptr), StartOfData(nullptr),
InnermostNormalCleanup(stable_end()), InnermostEHScope(stable_end()),
CGF(nullptr) {}
~EHScopeStack() { delete[] StartOfBuffer; }


template <class T, class... As> void pushCleanup(CleanupKind Kind, As... A) {
static_assert(alignof(T) <= ScopeStackAlignment,
"Cleanup's alignment is too large.");
void *Buffer = pushCleanup(Kind, sizeof(T));
Cleanup *Obj = new (Buffer) T(A...);
(void) Obj;
}


template <class T, class... As>
void pushCleanupTuple(CleanupKind Kind, std::tuple<As...> A) {
static_assert(alignof(T) <= ScopeStackAlignment,
"Cleanup's alignment is too large.");
void *Buffer = pushCleanup(Kind, sizeof(T));
Cleanup *Obj = new (Buffer) T(std::move(A));
(void) Obj;
}














template <class T, class... As>
T *pushCleanupWithExtra(CleanupKind Kind, size_t N, As... A) {
static_assert(alignof(T) <= ScopeStackAlignment,
"Cleanup's alignment is too large.");
void *Buffer = pushCleanup(Kind, sizeof(T) + T::getExtraSize(N));
return new (Buffer) T(N, A...);
}

void pushCopyOfCleanup(CleanupKind Kind, const void *Cleanup, size_t Size) {
void *Buffer = pushCleanup(Kind, Size);
std::memcpy(Buffer, Cleanup, Size);
}

void setCGF(CodeGenFunction *inCGF) { CGF = inCGF; }


void popCleanup();




class EHCatchScope *pushCatch(unsigned NumHandlers);


void popCatch();


class EHFilterScope *pushFilter(unsigned NumFilters);


void popFilter();


void pushTerminate();


void popTerminate();



bool containsOnlyLifetimeMarkers(stable_iterator Old) const;


bool empty() const { return StartOfData == EndOfBuffer; }

bool requiresLandingPad() const;


bool hasNormalCleanups() const {
return InnermostNormalCleanup != stable_end();
}



stable_iterator getInnermostNormalCleanup() const {
return InnermostNormalCleanup;
}
stable_iterator getInnermostActiveNormalCleanup() const;

stable_iterator getInnermostEHScope() const {
return InnermostEHScope;
}




class iterator;


iterator begin() const;


iterator end() const;




stable_iterator stable_begin() const {
return stable_iterator(EndOfBuffer - StartOfData);
}


static stable_iterator stable_end() {
return stable_iterator(0);
}


stable_iterator stabilize(iterator it) const;



iterator find(stable_iterator save) const;


BranchFixup &addBranchFixup() {
assert(hasNormalCleanups() && "adding fixup in scope without cleanups");
BranchFixups.push_back(BranchFixup());
return BranchFixups.back();
}

unsigned getNumBranchFixups() const { return BranchFixups.size(); }
BranchFixup &getBranchFixup(unsigned I) {
assert(I < getNumBranchFixups());
return BranchFixups[I];
}




void popNullFixups();



void clearFixups() { BranchFixups.clear(); }
};

}
}

#endif
