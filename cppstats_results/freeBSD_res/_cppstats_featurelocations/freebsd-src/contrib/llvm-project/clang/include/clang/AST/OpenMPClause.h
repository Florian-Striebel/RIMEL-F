














#if !defined(LLVM_CLANG_AST_OPENMPCLAUSE_H)
#define LLVM_CLANG_AST_OPENMPCLAUSE_H

#include "clang/AST/ASTFwd.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/Expr.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtIterator.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/OpenMPKinds.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Frontend/OpenMP/OMPConstants.h"
#include "llvm/Frontend/OpenMP/OMPContext.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TrailingObjects.h"
#include <cassert>
#include <cstddef>
#include <iterator>
#include <utility>

namespace clang {

class ASTContext;






class OMPClause {

SourceLocation StartLoc;


SourceLocation EndLoc;


OpenMPClauseKind Kind;

protected:
OMPClause(OpenMPClauseKind K, SourceLocation StartLoc, SourceLocation EndLoc)
: StartLoc(StartLoc), EndLoc(EndLoc), Kind(K) {}

public:

SourceLocation getBeginLoc() const { return StartLoc; }


SourceLocation getEndLoc() const { return EndLoc; }


void setLocStart(SourceLocation Loc) { StartLoc = Loc; }


void setLocEnd(SourceLocation Loc) { EndLoc = Loc; }


OpenMPClauseKind getClauseKind() const { return Kind; }

bool isImplicit() const { return StartLoc.isInvalid(); }

using child_iterator = StmtIterator;
using const_child_iterator = ConstStmtIterator;
using child_range = llvm::iterator_range<child_iterator>;
using const_child_range = llvm::iterator_range<const_child_iterator>;

child_range children();
const_child_range children() const {
auto Children = const_cast<OMPClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}




child_range used_children();
const_child_range used_children() const {
auto Children = const_cast<OMPClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *) { return true; }
};



class OMPClauseWithPreInit {
friend class OMPClauseReader;


Stmt *PreInit = nullptr;


OpenMPDirectiveKind CaptureRegion = llvm::omp::OMPD_unknown;

protected:
OMPClauseWithPreInit(const OMPClause *This) {
assert(get(This) && "get is not tuned for pre-init.");
}


void
setPreInitStmt(Stmt *S,
OpenMPDirectiveKind ThisRegion = llvm::omp::OMPD_unknown) {
PreInit = S;
CaptureRegion = ThisRegion;
}

public:

const Stmt *getPreInitStmt() const { return PreInit; }


Stmt *getPreInitStmt() { return PreInit; }


OpenMPDirectiveKind getCaptureRegion() const { return CaptureRegion; }

static OMPClauseWithPreInit *get(OMPClause *C);
static const OMPClauseWithPreInit *get(const OMPClause *C);
};



class OMPClauseWithPostUpdate : public OMPClauseWithPreInit {
friend class OMPClauseReader;


Expr *PostUpdate = nullptr;

protected:
OMPClauseWithPostUpdate(const OMPClause *This) : OMPClauseWithPreInit(This) {
assert(get(This) && "get is not tuned for post-update.");
}


void setPostUpdateExpr(Expr *S) { PostUpdate = S; }

public:

const Expr *getPostUpdateExpr() const { return PostUpdate; }


Expr *getPostUpdateExpr() { return PostUpdate; }

static OMPClauseWithPostUpdate *get(OMPClause *C);
static const OMPClauseWithPostUpdate *get(const OMPClause *C);
};


struct OMPVarListLocTy {

SourceLocation StartLoc;

SourceLocation LParenLoc;

SourceLocation EndLoc;
OMPVarListLocTy() = default;
OMPVarListLocTy(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: StartLoc(StartLoc), LParenLoc(LParenLoc), EndLoc(EndLoc) {}
};




template <class T> class OMPVarListClause : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


unsigned NumVars;

protected:







OMPVarListClause(OpenMPClauseKind K, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc, unsigned N)
: OMPClause(K, StartLoc, EndLoc), LParenLoc(LParenLoc), NumVars(N) {}


MutableArrayRef<Expr *> getVarRefs() {
return MutableArrayRef<Expr *>(
static_cast<T *>(this)->template getTrailingObjects<Expr *>(), NumVars);
}


void setVarRefs(ArrayRef<Expr *> VL) {
assert(VL.size() == NumVars &&
"Number of variables is not the same as the preallocated buffer");
std::copy(VL.begin(), VL.end(),
static_cast<T *>(this)->template getTrailingObjects<Expr *>());
}

public:
using varlist_iterator = MutableArrayRef<Expr *>::iterator;
using varlist_const_iterator = ArrayRef<const Expr *>::iterator;
using varlist_range = llvm::iterator_range<varlist_iterator>;
using varlist_const_range = llvm::iterator_range<varlist_const_iterator>;

unsigned varlist_size() const { return NumVars; }
bool varlist_empty() const { return NumVars == 0; }

varlist_range varlists() {
return varlist_range(varlist_begin(), varlist_end());
}
varlist_const_range varlists() const {
return varlist_const_range(varlist_begin(), varlist_end());
}

varlist_iterator varlist_begin() { return getVarRefs().begin(); }
varlist_iterator varlist_end() { return getVarRefs().end(); }
varlist_const_iterator varlist_begin() const { return getVarRefs().begin(); }
varlist_const_iterator varlist_end() const { return getVarRefs().end(); }


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


ArrayRef<const Expr *> getVarRefs() const {
return llvm::makeArrayRef(
static_cast<const T *>(this)->template getTrailingObjects<Expr *>(),
NumVars);
}
};









class OMPAllocatorClause : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Allocator = nullptr;


void setAllocator(Expr *A) { Allocator = A; }

public:






OMPAllocatorClause(Expr *A, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_allocator, StartLoc, EndLoc),
LParenLoc(LParenLoc), Allocator(A) {}


OMPAllocatorClause()
: OMPClause(llvm::omp::OMPC_allocator, SourceLocation(),
SourceLocation()) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getAllocator() const { return cast_or_null<Expr>(Allocator); }

child_range children() { return child_range(&Allocator, &Allocator + 1); }

const_child_range children() const {
return const_child_range(&Allocator, &Allocator + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_allocator;
}
};








class OMPAllocateClause final
: public OMPVarListClause<OMPAllocateClause>,
private llvm::TrailingObjects<OMPAllocateClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;



Expr *Allocator = nullptr;

SourceLocation ColonLoc;









OMPAllocateClause(SourceLocation StartLoc, SourceLocation LParenLoc,
Expr *Allocator, SourceLocation ColonLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPAllocateClause>(llvm::omp::OMPC_allocate, StartLoc,
LParenLoc, EndLoc, N),
Allocator(Allocator), ColonLoc(ColonLoc) {}




explicit OMPAllocateClause(unsigned N)
: OMPVarListClause<OMPAllocateClause>(llvm::omp::OMPC_allocate,
SourceLocation(), SourceLocation(),
SourceLocation(), N) {}


void setColonLoc(SourceLocation CL) { ColonLoc = CL; }

void setAllocator(Expr *A) { Allocator = A; }

public:









static OMPAllocateClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc, Expr *Allocator,
SourceLocation ColonLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL);


Expr *getAllocator() const { return Allocator; }


SourceLocation getColonLoc() const { return ColonLoc; }





static OMPAllocateClause *CreateEmpty(const ASTContext &C, unsigned N);

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPAllocateClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_allocate;
}
};








class OMPIfClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Condition = nullptr;


SourceLocation ColonLoc;


OpenMPDirectiveKind NameModifier = llvm::omp::OMPD_unknown;


SourceLocation NameModifierLoc;


void setCondition(Expr *Cond) { Condition = Cond; }


void setNameModifier(OpenMPDirectiveKind NM) { NameModifier = NM; }


void setNameModifierLoc(SourceLocation Loc) { NameModifierLoc = Loc; }


void setColonLoc(SourceLocation Loc) { ColonLoc = Loc; }

public:












OMPIfClause(OpenMPDirectiveKind NameModifier, Expr *Cond, Stmt *HelperCond,
OpenMPDirectiveKind CaptureRegion, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation NameModifierLoc,
SourceLocation ColonLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_if, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), Condition(Cond),
ColonLoc(ColonLoc), NameModifier(NameModifier),
NameModifierLoc(NameModifierLoc) {
setPreInitStmt(HelperCond, CaptureRegion);
}


OMPIfClause()
: OMPClause(llvm::omp::OMPC_if, SourceLocation(), SourceLocation()),
OMPClauseWithPreInit(this) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


SourceLocation getColonLoc() const { return ColonLoc; }


Expr *getCondition() const { return cast_or_null<Expr>(Condition); }


OpenMPDirectiveKind getNameModifier() const { return NameModifier; }


SourceLocation getNameModifierLoc() const { return NameModifierLoc; }

child_range children() { return child_range(&Condition, &Condition + 1); }

const_child_range children() const {
return const_child_range(&Condition, &Condition + 1);
}

child_range used_children();
const_child_range used_children() const {
auto Children = const_cast<OMPIfClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_if;
}
};








class OMPFinalClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Condition = nullptr;


void setCondition(Expr *Cond) { Condition = Cond; }

public:









OMPFinalClause(Expr *Cond, Stmt *HelperCond,
OpenMPDirectiveKind CaptureRegion, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_final, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), Condition(Cond) {
setPreInitStmt(HelperCond, CaptureRegion);
}


OMPFinalClause()
: OMPClause(llvm::omp::OMPC_final, SourceLocation(), SourceLocation()),
OMPClauseWithPreInit(this) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getCondition() const { return cast_or_null<Expr>(Condition); }

child_range children() { return child_range(&Condition, &Condition + 1); }

const_child_range children() const {
return const_child_range(&Condition, &Condition + 1);
}

child_range used_children();
const_child_range used_children() const {
auto Children = const_cast<OMPFinalClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_final;
}
};









class OMPNumThreadsClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *NumThreads = nullptr;


void setNumThreads(Expr *NThreads) { NumThreads = NThreads; }

public:









OMPNumThreadsClause(Expr *NumThreads, Stmt *HelperNumThreads,
OpenMPDirectiveKind CaptureRegion,
SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_num_threads, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc),
NumThreads(NumThreads) {
setPreInitStmt(HelperNumThreads, CaptureRegion);
}


OMPNumThreadsClause()
: OMPClause(llvm::omp::OMPC_num_threads, SourceLocation(),
SourceLocation()),
OMPClauseWithPreInit(this) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getNumThreads() const { return cast_or_null<Expr>(NumThreads); }

child_range children() { return child_range(&NumThreads, &NumThreads + 1); }

const_child_range children() const {
return const_child_range(&NumThreads, &NumThreads + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_num_threads;
}
};













class OMPSafelenClause : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Safelen = nullptr;


void setSafelen(Expr *Len) { Safelen = Len; }

public:





OMPSafelenClause(Expr *Len, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_safelen, StartLoc, EndLoc),
LParenLoc(LParenLoc), Safelen(Len) {}


explicit OMPSafelenClause()
: OMPClause(llvm::omp::OMPC_safelen, SourceLocation(), SourceLocation()) {
}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getSafelen() const { return cast_or_null<Expr>(Safelen); }

child_range children() { return child_range(&Safelen, &Safelen + 1); }

const_child_range children() const {
return const_child_range(&Safelen, &Safelen + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_safelen;
}
};












class OMPSimdlenClause : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Simdlen = nullptr;


void setSimdlen(Expr *Len) { Simdlen = Len; }

public:





OMPSimdlenClause(Expr *Len, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_simdlen, StartLoc, EndLoc),
LParenLoc(LParenLoc), Simdlen(Len) {}


explicit OMPSimdlenClause()
: OMPClause(llvm::omp::OMPC_simdlen, SourceLocation(), SourceLocation()) {
}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getSimdlen() const { return cast_or_null<Expr>(Simdlen); }

child_range children() { return child_range(&Simdlen, &Simdlen + 1); }

const_child_range children() const {
return const_child_range(&Simdlen, &Simdlen + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_simdlen;
}
};








class OMPSizesClause final
: public OMPClause,
private llvm::TrailingObjects<OMPSizesClause, Expr *> {
friend class OMPClauseReader;
friend class llvm::TrailingObjects<OMPSizesClause, Expr *>;


SourceLocation LParenLoc;


unsigned NumSizes;


explicit OMPSizesClause(int NumSizes)
: OMPClause(llvm::omp::OMPC_sizes, SourceLocation(), SourceLocation()),
NumSizes(NumSizes) {}

public:







static OMPSizesClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc,
ArrayRef<Expr *> Sizes);





static OMPSizesClause *CreateEmpty(const ASTContext &C, unsigned NumSizes);


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


unsigned getNumSizes() const { return NumSizes; }


MutableArrayRef<Expr *> getSizesRefs() {
return MutableArrayRef<Expr *>(static_cast<OMPSizesClause *>(this)
->template getTrailingObjects<Expr *>(),
NumSizes);
}
ArrayRef<Expr *> getSizesRefs() const {
return ArrayRef<Expr *>(static_cast<const OMPSizesClause *>(this)
->template getTrailingObjects<Expr *>(),
NumSizes);
}


void setSizesRefs(ArrayRef<Expr *> VL) {
assert(VL.size() == NumSizes);
std::copy(VL.begin(), VL.end(),
static_cast<OMPSizesClause *>(this)
->template getTrailingObjects<Expr *>());
}

child_range children() {
MutableArrayRef<Expr *> Sizes = getSizesRefs();
return child_range(reinterpret_cast<Stmt **>(Sizes.begin()),
reinterpret_cast<Stmt **>(Sizes.end()));
}
const_child_range children() const {
ArrayRef<Expr *> Sizes = getSizesRefs();
return const_child_range(reinterpret_cast<Stmt *const *>(Sizes.begin()),
reinterpret_cast<Stmt *const *>(Sizes.end()));
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_sizes;
}
};







class OMPFullClause final : public OMPClause {
friend class OMPClauseReader;


explicit OMPFullClause() : OMPClause(llvm::omp::OMPC_full, {}, {}) {}

public:





static OMPFullClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation EndLoc);




static OMPFullClause *CreateEmpty(const ASTContext &C);

child_range children() { return {child_iterator(), child_iterator()}; }
const_child_range children() const {
return {const_child_iterator(), const_child_iterator()};
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_full;
}
};








class OMPPartialClause final : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Factor;


explicit OMPPartialClause() : OMPClause(llvm::omp::OMPC_partial, {}, {}) {}


void setFactor(Expr *E) { Factor = E; }


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }

public:







static OMPPartialClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation EndLoc, Expr *Factor);




static OMPPartialClause *CreateEmpty(const ASTContext &C);


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getFactor() const { return cast_or_null<Expr>(Factor); }

child_range children() { return child_range(&Factor, &Factor + 1); }
const_child_range children() const {
return const_child_range(&Factor, &Factor + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_partial;
}
};












class OMPCollapseClause : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *NumForLoops = nullptr;


void setNumForLoops(Expr *Num) { NumForLoops = Num; }

public:






OMPCollapseClause(Expr *Num, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_collapse, StartLoc, EndLoc),
LParenLoc(LParenLoc), NumForLoops(Num) {}


explicit OMPCollapseClause()
: OMPClause(llvm::omp::OMPC_collapse, SourceLocation(),
SourceLocation()) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getNumForLoops() const { return cast_or_null<Expr>(NumForLoops); }

child_range children() { return child_range(&NumForLoops, &NumForLoops + 1); }

const_child_range children() const {
return const_child_range(&NumForLoops, &NumForLoops + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_collapse;
}
};








class OMPDefaultClause : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


llvm::omp::DefaultKind Kind = llvm::omp::OMP_DEFAULT_unknown;


SourceLocation KindKwLoc;




void setDefaultKind(llvm::omp::DefaultKind K) { Kind = K; }




void setDefaultKindKwLoc(SourceLocation KLoc) { KindKwLoc = KLoc; }

public:







OMPDefaultClause(llvm::omp::DefaultKind A, SourceLocation ALoc,
SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_default, StartLoc, EndLoc),
LParenLoc(LParenLoc), Kind(A), KindKwLoc(ALoc) {}


OMPDefaultClause()
: OMPClause(llvm::omp::OMPC_default, SourceLocation(), SourceLocation()) {
}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


llvm::omp::DefaultKind getDefaultKind() const { return Kind; }


SourceLocation getDefaultKindKwLoc() const { return KindKwLoc; }

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_default;
}
};









class OMPProcBindClause : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


llvm::omp::ProcBindKind Kind = llvm::omp::OMP_PROC_BIND_unknown;


SourceLocation KindKwLoc;




void setProcBindKind(llvm::omp::ProcBindKind K) { Kind = K; }




void setProcBindKindKwLoc(SourceLocation KLoc) { KindKwLoc = KLoc; }

public:








OMPProcBindClause(llvm::omp::ProcBindKind A, SourceLocation ALoc,
SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_proc_bind, StartLoc, EndLoc),
LParenLoc(LParenLoc), Kind(A), KindKwLoc(ALoc) {}


OMPProcBindClause()
: OMPClause(llvm::omp::OMPC_proc_bind, SourceLocation(),
SourceLocation()) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


llvm::omp::ProcBindKind getProcBindKind() const { return Kind; }


SourceLocation getProcBindKindKwLoc() const { return KindKwLoc; }

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_proc_bind;
}
};









class OMPUnifiedAddressClause final : public OMPClause {
public:
friend class OMPClauseReader;




OMPUnifiedAddressClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_unified_address, StartLoc, EndLoc) {}


OMPUnifiedAddressClause()
: OMPClause(llvm::omp::OMPC_unified_address, SourceLocation(),
SourceLocation()) {}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_unified_address;
}
};









class OMPUnifiedSharedMemoryClause final : public OMPClause {
public:
friend class OMPClauseReader;




OMPUnifiedSharedMemoryClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_unified_shared_memory, StartLoc, EndLoc) {}


OMPUnifiedSharedMemoryClause()
: OMPClause(llvm::omp::OMPC_unified_shared_memory, SourceLocation(),
SourceLocation()) {}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_unified_shared_memory;
}
};









class OMPReverseOffloadClause final : public OMPClause {
public:
friend class OMPClauseReader;




OMPReverseOffloadClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_reverse_offload, StartLoc, EndLoc) {}


OMPReverseOffloadClause()
: OMPClause(llvm::omp::OMPC_reverse_offload, SourceLocation(),
SourceLocation()) {}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_reverse_offload;
}
};









class OMPDynamicAllocatorsClause final : public OMPClause {
public:
friend class OMPClauseReader;




OMPDynamicAllocatorsClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_dynamic_allocators, StartLoc, EndLoc) {}


OMPDynamicAllocatorsClause()
: OMPClause(llvm::omp::OMPC_dynamic_allocators, SourceLocation(),
SourceLocation()) {}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_dynamic_allocators;
}
};









class OMPAtomicDefaultMemOrderClause final : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


OpenMPAtomicDefaultMemOrderClauseKind Kind =
OMPC_ATOMIC_DEFAULT_MEM_ORDER_unknown;


SourceLocation KindKwLoc;




void setAtomicDefaultMemOrderKind(OpenMPAtomicDefaultMemOrderClauseKind K) {
Kind = K;
}




void setAtomicDefaultMemOrderKindKwLoc(SourceLocation KLoc) {
KindKwLoc = KLoc;
}

public:








OMPAtomicDefaultMemOrderClause(OpenMPAtomicDefaultMemOrderClauseKind A,
SourceLocation ALoc, SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_atomic_default_mem_order, StartLoc, EndLoc),
LParenLoc(LParenLoc), Kind(A), KindKwLoc(ALoc) {}


OMPAtomicDefaultMemOrderClause()
: OMPClause(llvm::omp::OMPC_atomic_default_mem_order, SourceLocation(),
SourceLocation()) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


OpenMPAtomicDefaultMemOrderClauseKind getAtomicDefaultMemOrderKind() const {
return Kind;
}


SourceLocation getAtomicDefaultMemOrderKindKwLoc() const { return KindKwLoc; }

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_atomic_default_mem_order;
}
};








class OMPScheduleClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


OpenMPScheduleClauseKind Kind = OMPC_SCHEDULE_unknown;


enum {FIRST, SECOND, NUM_MODIFIERS};
OpenMPScheduleClauseModifier Modifiers[NUM_MODIFIERS];


SourceLocation ModifiersLoc[NUM_MODIFIERS];


SourceLocation KindLoc;


SourceLocation CommaLoc;


Expr *ChunkSize = nullptr;




void setScheduleKind(OpenMPScheduleClauseKind K) { Kind = K; }




void setFirstScheduleModifier(OpenMPScheduleClauseModifier M) {
Modifiers[FIRST] = M;
}




void setSecondScheduleModifier(OpenMPScheduleClauseModifier M) {
Modifiers[SECOND] = M;
}


void setFirstScheduleModifierLoc(SourceLocation Loc) {
ModifiersLoc[FIRST] = Loc;
}


void setSecondScheduleModifierLoc(SourceLocation Loc) {
ModifiersLoc[SECOND] = Loc;
}




void setScheduleModifer(OpenMPScheduleClauseModifier M) {
if (Modifiers[FIRST] == OMPC_SCHEDULE_MODIFIER_unknown)
Modifiers[FIRST] = M;
else {
assert(Modifiers[SECOND] == OMPC_SCHEDULE_MODIFIER_unknown);
Modifiers[SECOND] = M;
}
}




void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }




void setScheduleKindLoc(SourceLocation KLoc) { KindLoc = KLoc; }




void setCommaLoc(SourceLocation Loc) { CommaLoc = Loc; }




void setChunkSize(Expr *E) { ChunkSize = E; }

public:















OMPScheduleClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation KLoc, SourceLocation CommaLoc,
SourceLocation EndLoc, OpenMPScheduleClauseKind Kind,
Expr *ChunkSize, Stmt *HelperChunkSize,
OpenMPScheduleClauseModifier M1, SourceLocation M1Loc,
OpenMPScheduleClauseModifier M2, SourceLocation M2Loc)
: OMPClause(llvm::omp::OMPC_schedule, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), Kind(Kind),
KindLoc(KLoc), CommaLoc(CommaLoc), ChunkSize(ChunkSize) {
setPreInitStmt(HelperChunkSize);
Modifiers[FIRST] = M1;
Modifiers[SECOND] = M2;
ModifiersLoc[FIRST] = M1Loc;
ModifiersLoc[SECOND] = M2Loc;
}


explicit OMPScheduleClause()
: OMPClause(llvm::omp::OMPC_schedule, SourceLocation(), SourceLocation()),
OMPClauseWithPreInit(this) {
Modifiers[FIRST] = OMPC_SCHEDULE_MODIFIER_unknown;
Modifiers[SECOND] = OMPC_SCHEDULE_MODIFIER_unknown;
}


OpenMPScheduleClauseKind getScheduleKind() const { return Kind; }


OpenMPScheduleClauseModifier getFirstScheduleModifier() const {
return Modifiers[FIRST];
}


OpenMPScheduleClauseModifier getSecondScheduleModifier() const {
return Modifiers[SECOND];
}


SourceLocation getLParenLoc() { return LParenLoc; }


SourceLocation getScheduleKindLoc() { return KindLoc; }


SourceLocation getFirstScheduleModifierLoc() const {
return ModifiersLoc[FIRST];
}


SourceLocation getSecondScheduleModifierLoc() const {
return ModifiersLoc[SECOND];
}


SourceLocation getCommaLoc() { return CommaLoc; }


Expr *getChunkSize() { return ChunkSize; }


const Expr *getChunkSize() const { return ChunkSize; }

child_range children() {
return child_range(reinterpret_cast<Stmt **>(&ChunkSize),
reinterpret_cast<Stmt **>(&ChunkSize) + 1);
}

const_child_range children() const {
auto Children = const_cast<OMPScheduleClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_schedule;
}
};








class OMPOrderedClause final
: public OMPClause,
private llvm::TrailingObjects<OMPOrderedClause, Expr *> {
friend class OMPClauseReader;
friend TrailingObjects;


SourceLocation LParenLoc;


Stmt *NumForLoops = nullptr;


unsigned NumberOfLoops = 0;








OMPOrderedClause(Expr *Num, unsigned NumLoops, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_ordered, StartLoc, EndLoc),
LParenLoc(LParenLoc), NumForLoops(Num), NumberOfLoops(NumLoops) {}


explicit OMPOrderedClause(unsigned NumLoops)
: OMPClause(llvm::omp::OMPC_ordered, SourceLocation(), SourceLocation()),
NumberOfLoops(NumLoops) {}


void setNumForLoops(Expr *Num) { NumForLoops = Num; }

public:







static OMPOrderedClause *Create(const ASTContext &C, Expr *Num,
unsigned NumLoops, SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation EndLoc);


static OMPOrderedClause* CreateEmpty(const ASTContext &C, unsigned NumLoops);


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getNumForLoops() const { return cast_or_null<Expr>(NumForLoops); }


void setLoopNumIterations(unsigned NumLoop, Expr *NumIterations);

ArrayRef<Expr *> getLoopNumIterations() const;


void setLoopCounter(unsigned NumLoop, Expr *Counter);

Expr *getLoopCounter(unsigned NumLoop);
const Expr *getLoopCounter(unsigned NumLoop) const;

child_range children() { return child_range(&NumForLoops, &NumForLoops + 1); }

const_child_range children() const {
return const_child_range(&NumForLoops, &NumForLoops + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_ordered;
}
};







class OMPNowaitClause : public OMPClause {
public:




OMPNowaitClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_nowait, StartLoc, EndLoc) {}


OMPNowaitClause()
: OMPClause(llvm::omp::OMPC_nowait, SourceLocation(), SourceLocation()) {}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_nowait;
}
};







class OMPUntiedClause : public OMPClause {
public:




OMPUntiedClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_untied, StartLoc, EndLoc) {}


OMPUntiedClause()
: OMPClause(llvm::omp::OMPC_untied, SourceLocation(), SourceLocation()) {}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_untied;
}
};








class OMPMergeableClause : public OMPClause {
public:




OMPMergeableClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_mergeable, StartLoc, EndLoc) {}


OMPMergeableClause()
: OMPClause(llvm::omp::OMPC_mergeable, SourceLocation(),
SourceLocation()) {}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_mergeable;
}
};







class OMPReadClause : public OMPClause {
public:




OMPReadClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_read, StartLoc, EndLoc) {}


OMPReadClause()
: OMPClause(llvm::omp::OMPC_read, SourceLocation(), SourceLocation()) {}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_read;
}
};







class OMPWriteClause : public OMPClause {
public:




OMPWriteClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_write, StartLoc, EndLoc) {}


OMPWriteClause()
: OMPClause(llvm::omp::OMPC_write, SourceLocation(), SourceLocation()) {}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_write;
}
};
















class OMPUpdateClause final
: public OMPClause,
private llvm::TrailingObjects<OMPUpdateClause, SourceLocation,
OpenMPDependClauseKind> {
friend class OMPClauseReader;
friend TrailingObjects;


bool IsExtended = false;



size_t numTrailingObjects(OverloadToken<SourceLocation>) const {

return IsExtended ? 2 : 0;
}


void setLParenLoc(SourceLocation Loc) {
assert(IsExtended && "Expected extended clause.");
*getTrailingObjects<SourceLocation>() = Loc;
}


void setArgumentLoc(SourceLocation Loc) {
assert(IsExtended && "Expected extended clause.");
*std::next(getTrailingObjects<SourceLocation>(), 1) = Loc;
}


void setDependencyKind(OpenMPDependClauseKind DK) {
assert(IsExtended && "Expected extended clause.");
*getTrailingObjects<OpenMPDependClauseKind>() = DK;
}





OMPUpdateClause(SourceLocation StartLoc, SourceLocation EndLoc,
bool IsExtended)
: OMPClause(llvm::omp::OMPC_update, StartLoc, EndLoc),
IsExtended(IsExtended) {}


OMPUpdateClause(bool IsExtended)
: OMPClause(llvm::omp::OMPC_update, SourceLocation(), SourceLocation()),
IsExtended(IsExtended) {}

public:





static OMPUpdateClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation EndLoc);









static OMPUpdateClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation ArgumentLoc,
OpenMPDependClauseKind DK,
SourceLocation EndLoc);






static OMPUpdateClause *CreateEmpty(const ASTContext &C, bool IsExtended);


bool isExtended() const { return IsExtended; }

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}


SourceLocation getLParenLoc() const {
assert(IsExtended && "Expected extended clause.");
return *getTrailingObjects<SourceLocation>();
}


SourceLocation getArgumentLoc() const {
assert(IsExtended && "Expected extended clause.");
return *std::next(getTrailingObjects<SourceLocation>(), 1);
}


OpenMPDependClauseKind getDependencyKind() const {
assert(IsExtended && "Expected extended clause.");
return *getTrailingObjects<OpenMPDependClauseKind>();
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_update;
}
};








class OMPCaptureClause : public OMPClause {
public:




OMPCaptureClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_capture, StartLoc, EndLoc) {}


OMPCaptureClause()
: OMPClause(llvm::omp::OMPC_capture, SourceLocation(), SourceLocation()) {
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_capture;
}
};








class OMPSeqCstClause : public OMPClause {
public:




OMPSeqCstClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_seq_cst, StartLoc, EndLoc) {}


OMPSeqCstClause()
: OMPClause(llvm::omp::OMPC_seq_cst, SourceLocation(), SourceLocation()) {
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_seq_cst;
}
};








class OMPAcqRelClause final : public OMPClause {
public:




OMPAcqRelClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_acq_rel, StartLoc, EndLoc) {}


OMPAcqRelClause()
: OMPClause(llvm::omp::OMPC_acq_rel, SourceLocation(), SourceLocation()) {
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_acq_rel;
}
};








class OMPAcquireClause final : public OMPClause {
public:




OMPAcquireClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_acquire, StartLoc, EndLoc) {}


OMPAcquireClause()
: OMPClause(llvm::omp::OMPC_acquire, SourceLocation(), SourceLocation()) {
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_acquire;
}
};








class OMPReleaseClause final : public OMPClause {
public:




OMPReleaseClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_release, StartLoc, EndLoc) {}


OMPReleaseClause()
: OMPClause(llvm::omp::OMPC_release, SourceLocation(), SourceLocation()) {
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_release;
}
};








class OMPRelaxedClause final : public OMPClause {
public:




OMPRelaxedClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_relaxed, StartLoc, EndLoc) {}


OMPRelaxedClause()
: OMPClause(llvm::omp::OMPC_relaxed, SourceLocation(), SourceLocation()) {
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_relaxed;
}
};








class OMPPrivateClause final
: public OMPVarListClause<OMPPrivateClause>,
private llvm::TrailingObjects<OMPPrivateClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;







OMPPrivateClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPPrivateClause>(llvm::omp::OMPC_private, StartLoc,
LParenLoc, EndLoc, N) {}




explicit OMPPrivateClause(unsigned N)
: OMPVarListClause<OMPPrivateClause>(llvm::omp::OMPC_private,
SourceLocation(), SourceLocation(),
SourceLocation(), N) {}




void setPrivateCopies(ArrayRef<Expr *> VL);



MutableArrayRef<Expr *> getPrivateCopies() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getPrivateCopies() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}

public:








static OMPPrivateClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL,
ArrayRef<Expr *> PrivateVL);





static OMPPrivateClause *CreateEmpty(const ASTContext &C, unsigned N);

using private_copies_iterator = MutableArrayRef<Expr *>::iterator;
using private_copies_const_iterator = ArrayRef<const Expr *>::iterator;
using private_copies_range = llvm::iterator_range<private_copies_iterator>;
using private_copies_const_range =
llvm::iterator_range<private_copies_const_iterator>;

private_copies_range private_copies() {
return private_copies_range(getPrivateCopies().begin(),
getPrivateCopies().end());
}

private_copies_const_range private_copies() const {
return private_copies_const_range(getPrivateCopies().begin(),
getPrivateCopies().end());
}

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPPrivateClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_private;
}
};









class OMPFirstprivateClause final
: public OMPVarListClause<OMPFirstprivateClause>,
public OMPClauseWithPreInit,
private llvm::TrailingObjects<OMPFirstprivateClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;







OMPFirstprivateClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPFirstprivateClause>(llvm::omp::OMPC_firstprivate,
StartLoc, LParenLoc, EndLoc, N),
OMPClauseWithPreInit(this) {}




explicit OMPFirstprivateClause(unsigned N)
: OMPVarListClause<OMPFirstprivateClause>(
llvm::omp::OMPC_firstprivate, SourceLocation(), SourceLocation(),
SourceLocation(), N),
OMPClauseWithPreInit(this) {}




void setPrivateCopies(ArrayRef<Expr *> VL);



MutableArrayRef<Expr *> getPrivateCopies() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getPrivateCopies() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}




void setInits(ArrayRef<Expr *> VL);



MutableArrayRef<Expr *> getInits() {
return MutableArrayRef<Expr *>(getPrivateCopies().end(), varlist_size());
}
ArrayRef<const Expr *> getInits() const {
return llvm::makeArrayRef(getPrivateCopies().end(), varlist_size());
}

public:













static OMPFirstprivateClause *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL, ArrayRef<Expr *> PrivateVL,
ArrayRef<Expr *> InitVL, Stmt *PreInit);





static OMPFirstprivateClause *CreateEmpty(const ASTContext &C, unsigned N);

using private_copies_iterator = MutableArrayRef<Expr *>::iterator;
using private_copies_const_iterator = ArrayRef<const Expr *>::iterator;
using private_copies_range = llvm::iterator_range<private_copies_iterator>;
using private_copies_const_range =
llvm::iterator_range<private_copies_const_iterator>;

private_copies_range private_copies() {
return private_copies_range(getPrivateCopies().begin(),
getPrivateCopies().end());
}
private_copies_const_range private_copies() const {
return private_copies_const_range(getPrivateCopies().begin(),
getPrivateCopies().end());
}

using inits_iterator = MutableArrayRef<Expr *>::iterator;
using inits_const_iterator = ArrayRef<const Expr *>::iterator;
using inits_range = llvm::iterator_range<inits_iterator>;
using inits_const_range = llvm::iterator_range<inits_const_iterator>;

inits_range inits() {
return inits_range(getInits().begin(), getInits().end());
}
inits_const_range inits() const {
return inits_const_range(getInits().begin(), getInits().end());
}

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPFirstprivateClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}
const_child_range used_children() const {
auto Children = const_cast<OMPFirstprivateClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_firstprivate;
}
};









class OMPLastprivateClause final
: public OMPVarListClause<OMPLastprivateClause>,
public OMPClauseWithPostUpdate,
private llvm::TrailingObjects<OMPLastprivateClause, Expr *> {
















friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;


OpenMPLastprivateModifier LPKind;

SourceLocation LPKindLoc;

SourceLocation ColonLoc;







OMPLastprivateClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, OpenMPLastprivateModifier LPKind,
SourceLocation LPKindLoc, SourceLocation ColonLoc,
unsigned N)
: OMPVarListClause<OMPLastprivateClause>(llvm::omp::OMPC_lastprivate,
StartLoc, LParenLoc, EndLoc, N),
OMPClauseWithPostUpdate(this), LPKind(LPKind), LPKindLoc(LPKindLoc),
ColonLoc(ColonLoc) {}




explicit OMPLastprivateClause(unsigned N)
: OMPVarListClause<OMPLastprivateClause>(
llvm::omp::OMPC_lastprivate, SourceLocation(), SourceLocation(),
SourceLocation(), N),
OMPClauseWithPostUpdate(this) {}



MutableArrayRef<Expr *> getPrivateCopies() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getPrivateCopies() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}





void setSourceExprs(ArrayRef<Expr *> SrcExprs);


MutableArrayRef<Expr *> getSourceExprs() {
return MutableArrayRef<Expr *>(getPrivateCopies().end(), varlist_size());
}
ArrayRef<const Expr *> getSourceExprs() const {
return llvm::makeArrayRef(getPrivateCopies().end(), varlist_size());
}





void setDestinationExprs(ArrayRef<Expr *> DstExprs);


MutableArrayRef<Expr *> getDestinationExprs() {
return MutableArrayRef<Expr *>(getSourceExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getDestinationExprs() const {
return llvm::makeArrayRef(getSourceExprs().end(), varlist_size());
}




void setAssignmentOps(ArrayRef<Expr *> AssignmentOps);


MutableArrayRef<Expr *> getAssignmentOps() {
return MutableArrayRef<Expr *>(getDestinationExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getAssignmentOps() const {
return llvm::makeArrayRef(getDestinationExprs().end(), varlist_size());
}


void setKind(OpenMPLastprivateModifier Kind) { LPKind = Kind; }

void setKindLoc(SourceLocation Loc) { LPKindLoc = Loc; }

void setColonLoc(SourceLocation Loc) { ColonLoc = Loc; }

public:



























static OMPLastprivateClause *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL, ArrayRef<Expr *> SrcExprs,
ArrayRef<Expr *> DstExprs, ArrayRef<Expr *> AssignmentOps,
OpenMPLastprivateModifier LPKind, SourceLocation LPKindLoc,
SourceLocation ColonLoc, Stmt *PreInit, Expr *PostUpdate);





static OMPLastprivateClause *CreateEmpty(const ASTContext &C, unsigned N);


OpenMPLastprivateModifier getKind() const { return LPKind; }

SourceLocation getKindLoc() const { return LPKindLoc; }

SourceLocation getColonLoc() const { return ColonLoc; }

using helper_expr_iterator = MutableArrayRef<Expr *>::iterator;
using helper_expr_const_iterator = ArrayRef<const Expr *>::iterator;
using helper_expr_range = llvm::iterator_range<helper_expr_iterator>;
using helper_expr_const_range =
llvm::iterator_range<helper_expr_const_iterator>;



void setPrivateCopies(ArrayRef<Expr *> PrivateCopies);

helper_expr_const_range private_copies() const {
return helper_expr_const_range(getPrivateCopies().begin(),
getPrivateCopies().end());
}

helper_expr_range private_copies() {
return helper_expr_range(getPrivateCopies().begin(),
getPrivateCopies().end());
}

helper_expr_const_range source_exprs() const {
return helper_expr_const_range(getSourceExprs().begin(),
getSourceExprs().end());
}

helper_expr_range source_exprs() {
return helper_expr_range(getSourceExprs().begin(), getSourceExprs().end());
}

helper_expr_const_range destination_exprs() const {
return helper_expr_const_range(getDestinationExprs().begin(),
getDestinationExprs().end());
}

helper_expr_range destination_exprs() {
return helper_expr_range(getDestinationExprs().begin(),
getDestinationExprs().end());
}

helper_expr_const_range assignment_ops() const {
return helper_expr_const_range(getAssignmentOps().begin(),
getAssignmentOps().end());
}

helper_expr_range assignment_ops() {
return helper_expr_range(getAssignmentOps().begin(),
getAssignmentOps().end());
}

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPLastprivateClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_lastprivate;
}
};








class OMPSharedClause final
: public OMPVarListClause<OMPSharedClause>,
private llvm::TrailingObjects<OMPSharedClause, Expr *> {
friend OMPVarListClause;
friend TrailingObjects;







OMPSharedClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPSharedClause>(llvm::omp::OMPC_shared, StartLoc,
LParenLoc, EndLoc, N) {}




explicit OMPSharedClause(unsigned N)
: OMPVarListClause<OMPSharedClause>(llvm::omp::OMPC_shared,
SourceLocation(), SourceLocation(),
SourceLocation(), N) {}

public:







static OMPSharedClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL);





static OMPSharedClause *CreateEmpty(const ASTContext &C, unsigned N);

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPSharedClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_shared;
}
};









class OMPReductionClause final
: public OMPVarListClause<OMPReductionClause>,
public OMPClauseWithPostUpdate,
private llvm::TrailingObjects<OMPReductionClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;


OpenMPReductionClauseModifier Modifier = OMPC_REDUCTION_unknown;


SourceLocation ModifierLoc;


SourceLocation ColonLoc;


NestedNameSpecifierLoc QualifierLoc;


DeclarationNameInfo NameInfo;











OMPReductionClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation ModifierLoc, SourceLocation ColonLoc,
SourceLocation EndLoc,
OpenMPReductionClauseModifier Modifier, unsigned N,
NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo)
: OMPVarListClause<OMPReductionClause>(llvm::omp::OMPC_reduction,
StartLoc, LParenLoc, EndLoc, N),
OMPClauseWithPostUpdate(this), Modifier(Modifier),
ModifierLoc(ModifierLoc), ColonLoc(ColonLoc),
QualifierLoc(QualifierLoc), NameInfo(NameInfo) {}




explicit OMPReductionClause(unsigned N)
: OMPVarListClause<OMPReductionClause>(llvm::omp::OMPC_reduction,
SourceLocation(), SourceLocation(),
SourceLocation(), N),
OMPClauseWithPostUpdate(this) {}


void setModifier(OpenMPReductionClauseModifier M) { Modifier = M; }


void setModifierLoc(SourceLocation Loc) { ModifierLoc = Loc; }


void setColonLoc(SourceLocation CL) { ColonLoc = CL; }


void setNameInfo(DeclarationNameInfo DNI) { NameInfo = DNI; }


void setQualifierLoc(NestedNameSpecifierLoc NSL) { QualifierLoc = NSL; }




void setPrivates(ArrayRef<Expr *> Privates);


MutableArrayRef<Expr *> getPrivates() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getPrivates() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}




void setLHSExprs(ArrayRef<Expr *> LHSExprs);


MutableArrayRef<Expr *> getLHSExprs() {
return MutableArrayRef<Expr *>(getPrivates().end(), varlist_size());
}
ArrayRef<const Expr *> getLHSExprs() const {
return llvm::makeArrayRef(getPrivates().end(), varlist_size());
}






void setRHSExprs(ArrayRef<Expr *> RHSExprs);


MutableArrayRef<Expr *> getRHSExprs() {
return MutableArrayRef<Expr *>(getLHSExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getRHSExprs() const {
return llvm::makeArrayRef(getLHSExprs().end(), varlist_size());
}





void setReductionOps(ArrayRef<Expr *> ReductionOps);


MutableArrayRef<Expr *> getReductionOps() {
return MutableArrayRef<Expr *>(getRHSExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getReductionOps() const {
return llvm::makeArrayRef(getRHSExprs().end(), varlist_size());
}



void setInscanCopyOps(ArrayRef<Expr *> Ops);


MutableArrayRef<Expr *> getInscanCopyOps() {
return MutableArrayRef<Expr *>(getReductionOps().end(), varlist_size());
}
ArrayRef<const Expr *> getInscanCopyOps() const {
return llvm::makeArrayRef(getReductionOps().end(), varlist_size());
}


void setInscanCopyArrayTemps(ArrayRef<Expr *> CopyArrayTemps);


MutableArrayRef<Expr *> getInscanCopyArrayTemps() {
return MutableArrayRef<Expr *>(getInscanCopyOps().end(), varlist_size());
}
ArrayRef<const Expr *> getInscanCopyArrayTemps() const {
return llvm::makeArrayRef(getInscanCopyOps().end(), varlist_size());
}


void setInscanCopyArrayElems(ArrayRef<Expr *> CopyArrayElems);


MutableArrayRef<Expr *> getInscanCopyArrayElems() {
return MutableArrayRef<Expr *>(getInscanCopyArrayTemps().end(),
varlist_size());
}
ArrayRef<const Expr *> getInscanCopyArrayElems() const {
return llvm::makeArrayRef(getInscanCopyArrayTemps().end(), varlist_size());
}

public:







































static OMPReductionClause *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation ModifierLoc, SourceLocation ColonLoc,
SourceLocation EndLoc, OpenMPReductionClauseModifier Modifier,
ArrayRef<Expr *> VL, NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo, ArrayRef<Expr *> Privates,
ArrayRef<Expr *> LHSExprs, ArrayRef<Expr *> RHSExprs,
ArrayRef<Expr *> ReductionOps, ArrayRef<Expr *> CopyOps,
ArrayRef<Expr *> CopyArrayTemps, ArrayRef<Expr *> CopyArrayElems,
Stmt *PreInit, Expr *PostUpdate);






static OMPReductionClause *
CreateEmpty(const ASTContext &C, unsigned N,
OpenMPReductionClauseModifier Modifier);


OpenMPReductionClauseModifier getModifier() const { return Modifier; }


SourceLocation getModifierLoc() const { return ModifierLoc; }


SourceLocation getColonLoc() const { return ColonLoc; }


const DeclarationNameInfo &getNameInfo() const { return NameInfo; }


NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }

using helper_expr_iterator = MutableArrayRef<Expr *>::iterator;
using helper_expr_const_iterator = ArrayRef<const Expr *>::iterator;
using helper_expr_range = llvm::iterator_range<helper_expr_iterator>;
using helper_expr_const_range =
llvm::iterator_range<helper_expr_const_iterator>;

helper_expr_const_range privates() const {
return helper_expr_const_range(getPrivates().begin(), getPrivates().end());
}

helper_expr_range privates() {
return helper_expr_range(getPrivates().begin(), getPrivates().end());
}

helper_expr_const_range lhs_exprs() const {
return helper_expr_const_range(getLHSExprs().begin(), getLHSExprs().end());
}

helper_expr_range lhs_exprs() {
return helper_expr_range(getLHSExprs().begin(), getLHSExprs().end());
}

helper_expr_const_range rhs_exprs() const {
return helper_expr_const_range(getRHSExprs().begin(), getRHSExprs().end());
}

helper_expr_range rhs_exprs() {
return helper_expr_range(getRHSExprs().begin(), getRHSExprs().end());
}

helper_expr_const_range reduction_ops() const {
return helper_expr_const_range(getReductionOps().begin(),
getReductionOps().end());
}

helper_expr_range reduction_ops() {
return helper_expr_range(getReductionOps().begin(),
getReductionOps().end());
}

helper_expr_const_range copy_ops() const {
return helper_expr_const_range(getInscanCopyOps().begin(),
getInscanCopyOps().end());
}

helper_expr_range copy_ops() {
return helper_expr_range(getInscanCopyOps().begin(),
getInscanCopyOps().end());
}

helper_expr_const_range copy_array_temps() const {
return helper_expr_const_range(getInscanCopyArrayTemps().begin(),
getInscanCopyArrayTemps().end());
}

helper_expr_range copy_array_temps() {
return helper_expr_range(getInscanCopyArrayTemps().begin(),
getInscanCopyArrayTemps().end());
}

helper_expr_const_range copy_array_elems() const {
return helper_expr_const_range(getInscanCopyArrayElems().begin(),
getInscanCopyArrayElems().end());
}

helper_expr_range copy_array_elems() {
return helper_expr_range(getInscanCopyArrayElems().begin(),
getInscanCopyArrayElems().end());
}

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPReductionClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}
const_child_range used_children() const {
auto Children = const_cast<OMPReductionClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_reduction;
}
};









class OMPTaskReductionClause final
: public OMPVarListClause<OMPTaskReductionClause>,
public OMPClauseWithPostUpdate,
private llvm::TrailingObjects<OMPTaskReductionClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;


SourceLocation ColonLoc;


NestedNameSpecifierLoc QualifierLoc;


DeclarationNameInfo NameInfo;










OMPTaskReductionClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation ColonLoc, SourceLocation EndLoc,
unsigned N, NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo)
: OMPVarListClause<OMPTaskReductionClause>(
llvm::omp::OMPC_task_reduction, StartLoc, LParenLoc, EndLoc, N),
OMPClauseWithPostUpdate(this), ColonLoc(ColonLoc),
QualifierLoc(QualifierLoc), NameInfo(NameInfo) {}




explicit OMPTaskReductionClause(unsigned N)
: OMPVarListClause<OMPTaskReductionClause>(
llvm::omp::OMPC_task_reduction, SourceLocation(), SourceLocation(),
SourceLocation(), N),
OMPClauseWithPostUpdate(this) {}


void setColonLoc(SourceLocation CL) { ColonLoc = CL; }


void setNameInfo(DeclarationNameInfo DNI) { NameInfo = DNI; }


void setQualifierLoc(NestedNameSpecifierLoc NSL) { QualifierLoc = NSL; }



void setPrivates(ArrayRef<Expr *> Privates);


MutableArrayRef<Expr *> getPrivates() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getPrivates() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}




void setLHSExprs(ArrayRef<Expr *> LHSExprs);


MutableArrayRef<Expr *> getLHSExprs() {
return MutableArrayRef<Expr *>(getPrivates().end(), varlist_size());
}
ArrayRef<const Expr *> getLHSExprs() const {
return llvm::makeArrayRef(getPrivates().end(), varlist_size());
}





void setRHSExprs(ArrayRef<Expr *> RHSExprs);


MutableArrayRef<Expr *> getRHSExprs() {
return MutableArrayRef<Expr *>(getLHSExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getRHSExprs() const {
return llvm::makeArrayRef(getLHSExprs().end(), varlist_size());
}





void setReductionOps(ArrayRef<Expr *> ReductionOps);


MutableArrayRef<Expr *> getReductionOps() {
return MutableArrayRef<Expr *>(getRHSExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getReductionOps() const {
return llvm::makeArrayRef(getRHSExprs().end(), varlist_size());
}

public:
































static OMPTaskReductionClause *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation ColonLoc, SourceLocation EndLoc, ArrayRef<Expr *> VL,
NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo, ArrayRef<Expr *> Privates,
ArrayRef<Expr *> LHSExprs, ArrayRef<Expr *> RHSExprs,
ArrayRef<Expr *> ReductionOps, Stmt *PreInit, Expr *PostUpdate);





static OMPTaskReductionClause *CreateEmpty(const ASTContext &C, unsigned N);


SourceLocation getColonLoc() const { return ColonLoc; }


const DeclarationNameInfo &getNameInfo() const { return NameInfo; }


NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }

using helper_expr_iterator = MutableArrayRef<Expr *>::iterator;
using helper_expr_const_iterator = ArrayRef<const Expr *>::iterator;
using helper_expr_range = llvm::iterator_range<helper_expr_iterator>;
using helper_expr_const_range =
llvm::iterator_range<helper_expr_const_iterator>;

helper_expr_const_range privates() const {
return helper_expr_const_range(getPrivates().begin(), getPrivates().end());
}

helper_expr_range privates() {
return helper_expr_range(getPrivates().begin(), getPrivates().end());
}

helper_expr_const_range lhs_exprs() const {
return helper_expr_const_range(getLHSExprs().begin(), getLHSExprs().end());
}

helper_expr_range lhs_exprs() {
return helper_expr_range(getLHSExprs().begin(), getLHSExprs().end());
}

helper_expr_const_range rhs_exprs() const {
return helper_expr_const_range(getRHSExprs().begin(), getRHSExprs().end());
}

helper_expr_range rhs_exprs() {
return helper_expr_range(getRHSExprs().begin(), getRHSExprs().end());
}

helper_expr_const_range reduction_ops() const {
return helper_expr_const_range(getReductionOps().begin(),
getReductionOps().end());
}

helper_expr_range reduction_ops() {
return helper_expr_range(getReductionOps().begin(),
getReductionOps().end());
}

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPTaskReductionClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_task_reduction;
}
};








class OMPInReductionClause final
: public OMPVarListClause<OMPInReductionClause>,
public OMPClauseWithPostUpdate,
private llvm::TrailingObjects<OMPInReductionClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;


SourceLocation ColonLoc;


NestedNameSpecifierLoc QualifierLoc;


DeclarationNameInfo NameInfo;










OMPInReductionClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation ColonLoc, SourceLocation EndLoc,
unsigned N, NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo)
: OMPVarListClause<OMPInReductionClause>(llvm::omp::OMPC_in_reduction,
StartLoc, LParenLoc, EndLoc, N),
OMPClauseWithPostUpdate(this), ColonLoc(ColonLoc),
QualifierLoc(QualifierLoc), NameInfo(NameInfo) {}




explicit OMPInReductionClause(unsigned N)
: OMPVarListClause<OMPInReductionClause>(
llvm::omp::OMPC_in_reduction, SourceLocation(), SourceLocation(),
SourceLocation(), N),
OMPClauseWithPostUpdate(this) {}


void setColonLoc(SourceLocation CL) { ColonLoc = CL; }


void setNameInfo(DeclarationNameInfo DNI) { NameInfo = DNI; }


void setQualifierLoc(NestedNameSpecifierLoc NSL) { QualifierLoc = NSL; }



void setPrivates(ArrayRef<Expr *> Privates);


MutableArrayRef<Expr *> getPrivates() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getPrivates() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}




void setLHSExprs(ArrayRef<Expr *> LHSExprs);


MutableArrayRef<Expr *> getLHSExprs() {
return MutableArrayRef<Expr *>(getPrivates().end(), varlist_size());
}
ArrayRef<const Expr *> getLHSExprs() const {
return llvm::makeArrayRef(getPrivates().end(), varlist_size());
}





void setRHSExprs(ArrayRef<Expr *> RHSExprs);


MutableArrayRef<Expr *> getRHSExprs() {
return MutableArrayRef<Expr *>(getLHSExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getRHSExprs() const {
return llvm::makeArrayRef(getLHSExprs().end(), varlist_size());
}





void setReductionOps(ArrayRef<Expr *> ReductionOps);


MutableArrayRef<Expr *> getReductionOps() {
return MutableArrayRef<Expr *>(getRHSExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getReductionOps() const {
return llvm::makeArrayRef(getRHSExprs().end(), varlist_size());
}


void setTaskgroupDescriptors(ArrayRef<Expr *> ReductionOps);


MutableArrayRef<Expr *> getTaskgroupDescriptors() {
return MutableArrayRef<Expr *>(getReductionOps().end(), varlist_size());
}
ArrayRef<const Expr *> getTaskgroupDescriptors() const {
return llvm::makeArrayRef(getReductionOps().end(), varlist_size());
}

public:


































static OMPInReductionClause *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation ColonLoc, SourceLocation EndLoc, ArrayRef<Expr *> VL,
NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo, ArrayRef<Expr *> Privates,
ArrayRef<Expr *> LHSExprs, ArrayRef<Expr *> RHSExprs,
ArrayRef<Expr *> ReductionOps, ArrayRef<Expr *> TaskgroupDescriptors,
Stmt *PreInit, Expr *PostUpdate);





static OMPInReductionClause *CreateEmpty(const ASTContext &C, unsigned N);


SourceLocation getColonLoc() const { return ColonLoc; }


const DeclarationNameInfo &getNameInfo() const { return NameInfo; }


NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }

using helper_expr_iterator = MutableArrayRef<Expr *>::iterator;
using helper_expr_const_iterator = ArrayRef<const Expr *>::iterator;
using helper_expr_range = llvm::iterator_range<helper_expr_iterator>;
using helper_expr_const_range =
llvm::iterator_range<helper_expr_const_iterator>;

helper_expr_const_range privates() const {
return helper_expr_const_range(getPrivates().begin(), getPrivates().end());
}

helper_expr_range privates() {
return helper_expr_range(getPrivates().begin(), getPrivates().end());
}

helper_expr_const_range lhs_exprs() const {
return helper_expr_const_range(getLHSExprs().begin(), getLHSExprs().end());
}

helper_expr_range lhs_exprs() {
return helper_expr_range(getLHSExprs().begin(), getLHSExprs().end());
}

helper_expr_const_range rhs_exprs() const {
return helper_expr_const_range(getRHSExprs().begin(), getRHSExprs().end());
}

helper_expr_range rhs_exprs() {
return helper_expr_range(getRHSExprs().begin(), getRHSExprs().end());
}

helper_expr_const_range reduction_ops() const {
return helper_expr_const_range(getReductionOps().begin(),
getReductionOps().end());
}

helper_expr_range reduction_ops() {
return helper_expr_range(getReductionOps().begin(),
getReductionOps().end());
}

helper_expr_const_range taskgroup_descriptors() const {
return helper_expr_const_range(getTaskgroupDescriptors().begin(),
getTaskgroupDescriptors().end());
}

helper_expr_range taskgroup_descriptors() {
return helper_expr_range(getTaskgroupDescriptors().begin(),
getTaskgroupDescriptors().end());
}

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPInReductionClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_in_reduction;
}
};









class OMPLinearClause final
: public OMPVarListClause<OMPLinearClause>,
public OMPClauseWithPostUpdate,
private llvm::TrailingObjects<OMPLinearClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;


OpenMPLinearClauseKind Modifier = OMPC_LINEAR_val;


SourceLocation ModifierLoc;


SourceLocation ColonLoc;


void setStep(Expr *Step) { *(getFinals().end()) = Step; }


void setCalcStep(Expr *CalcStep) { *(getFinals().end() + 1) = CalcStep; }








OMPLinearClause(SourceLocation StartLoc, SourceLocation LParenLoc,
OpenMPLinearClauseKind Modifier, SourceLocation ModifierLoc,
SourceLocation ColonLoc, SourceLocation EndLoc,
unsigned NumVars)
: OMPVarListClause<OMPLinearClause>(llvm::omp::OMPC_linear, StartLoc,
LParenLoc, EndLoc, NumVars),
OMPClauseWithPostUpdate(this), Modifier(Modifier),
ModifierLoc(ModifierLoc), ColonLoc(ColonLoc) {}




explicit OMPLinearClause(unsigned NumVars)
: OMPVarListClause<OMPLinearClause>(llvm::omp::OMPC_linear,
SourceLocation(), SourceLocation(),
SourceLocation(), NumVars),
OMPClauseWithPostUpdate(this) {}













MutableArrayRef<Expr *> getPrivates() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getPrivates() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}

MutableArrayRef<Expr *> getInits() {
return MutableArrayRef<Expr *>(getPrivates().end(), varlist_size());
}
ArrayRef<const Expr *> getInits() const {
return llvm::makeArrayRef(getPrivates().end(), varlist_size());
}


MutableArrayRef<Expr *> getUpdates() {
return MutableArrayRef<Expr *>(getInits().end(), varlist_size());
}
ArrayRef<const Expr *> getUpdates() const {
return llvm::makeArrayRef(getInits().end(), varlist_size());
}


MutableArrayRef<Expr *> getFinals() {
return MutableArrayRef<Expr *>(getUpdates().end(), varlist_size());
}
ArrayRef<const Expr *> getFinals() const {
return llvm::makeArrayRef(getUpdates().end(), varlist_size());
}


MutableArrayRef<Expr *> getUsedExprs() {
return MutableArrayRef<Expr *>(getFinals().end() + 2, varlist_size() + 1);
}
ArrayRef<const Expr *> getUsedExprs() const {
return llvm::makeArrayRef(getFinals().end() + 2, varlist_size() + 1);
}



void setPrivates(ArrayRef<Expr *> PL);



void setInits(ArrayRef<Expr *> IL);

public:



















static OMPLinearClause *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation LParenLoc,
OpenMPLinearClauseKind Modifier, SourceLocation ModifierLoc,
SourceLocation ColonLoc, SourceLocation EndLoc, ArrayRef<Expr *> VL,
ArrayRef<Expr *> PL, ArrayRef<Expr *> IL, Expr *Step, Expr *CalcStep,
Stmt *PreInit, Expr *PostUpdate);





static OMPLinearClause *CreateEmpty(const ASTContext &C, unsigned NumVars);


void setModifier(OpenMPLinearClauseKind Kind) { Modifier = Kind; }


OpenMPLinearClauseKind getModifier() const { return Modifier; }


void setModifierLoc(SourceLocation Loc) { ModifierLoc = Loc; }


SourceLocation getModifierLoc() const { return ModifierLoc; }


void setColonLoc(SourceLocation Loc) { ColonLoc = Loc; }


SourceLocation getColonLoc() const { return ColonLoc; }


Expr *getStep() { return *(getFinals().end()); }


const Expr *getStep() const { return *(getFinals().end()); }


Expr *getCalcStep() { return *(getFinals().end() + 1); }


const Expr *getCalcStep() const { return *(getFinals().end() + 1); }



void setUpdates(ArrayRef<Expr *> UL);



void setFinals(ArrayRef<Expr *> FL);


void setUsedExprs(ArrayRef<Expr *> UE);

using privates_iterator = MutableArrayRef<Expr *>::iterator;
using privates_const_iterator = ArrayRef<const Expr *>::iterator;
using privates_range = llvm::iterator_range<privates_iterator>;
using privates_const_range = llvm::iterator_range<privates_const_iterator>;

privates_range privates() {
return privates_range(getPrivates().begin(), getPrivates().end());
}

privates_const_range privates() const {
return privates_const_range(getPrivates().begin(), getPrivates().end());
}

using inits_iterator = MutableArrayRef<Expr *>::iterator;
using inits_const_iterator = ArrayRef<const Expr *>::iterator;
using inits_range = llvm::iterator_range<inits_iterator>;
using inits_const_range = llvm::iterator_range<inits_const_iterator>;

inits_range inits() {
return inits_range(getInits().begin(), getInits().end());
}

inits_const_range inits() const {
return inits_const_range(getInits().begin(), getInits().end());
}

using updates_iterator = MutableArrayRef<Expr *>::iterator;
using updates_const_iterator = ArrayRef<const Expr *>::iterator;
using updates_range = llvm::iterator_range<updates_iterator>;
using updates_const_range = llvm::iterator_range<updates_const_iterator>;

updates_range updates() {
return updates_range(getUpdates().begin(), getUpdates().end());
}

updates_const_range updates() const {
return updates_const_range(getUpdates().begin(), getUpdates().end());
}

using finals_iterator = MutableArrayRef<Expr *>::iterator;
using finals_const_iterator = ArrayRef<const Expr *>::iterator;
using finals_range = llvm::iterator_range<finals_iterator>;
using finals_const_range = llvm::iterator_range<finals_const_iterator>;

finals_range finals() {
return finals_range(getFinals().begin(), getFinals().end());
}

finals_const_range finals() const {
return finals_const_range(getFinals().begin(), getFinals().end());
}

using used_expressions_iterator = MutableArrayRef<Expr *>::iterator;
using used_expressions_const_iterator = ArrayRef<const Expr *>::iterator;
using used_expressions_range =
llvm::iterator_range<used_expressions_iterator>;
using used_expressions_const_range =
llvm::iterator_range<used_expressions_const_iterator>;

used_expressions_range used_expressions() {
return finals_range(getUsedExprs().begin(), getUsedExprs().end());
}

used_expressions_const_range used_expressions() const {
return finals_const_range(getUsedExprs().begin(), getUsedExprs().end());
}

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPLinearClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children();

const_child_range used_children() const {
auto Children = const_cast<OMPLinearClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_linear;
}
};









class OMPAlignedClause final
: public OMPVarListClause<OMPAlignedClause>,
private llvm::TrailingObjects<OMPAlignedClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;


SourceLocation ColonLoc;


void setAlignment(Expr *A) { *varlist_end() = A; }








OMPAlignedClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation ColonLoc, SourceLocation EndLoc,
unsigned NumVars)
: OMPVarListClause<OMPAlignedClause>(llvm::omp::OMPC_aligned, StartLoc,
LParenLoc, EndLoc, NumVars),
ColonLoc(ColonLoc) {}




explicit OMPAlignedClause(unsigned NumVars)
: OMPVarListClause<OMPAlignedClause>(llvm::omp::OMPC_aligned,
SourceLocation(), SourceLocation(),
SourceLocation(), NumVars) {}

public:









static OMPAlignedClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation ColonLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL,
Expr *A);





static OMPAlignedClause *CreateEmpty(const ASTContext &C, unsigned NumVars);


void setColonLoc(SourceLocation Loc) { ColonLoc = Loc; }


SourceLocation getColonLoc() const { return ColonLoc; }


Expr *getAlignment() { return *varlist_end(); }


const Expr *getAlignment() const { return *varlist_end(); }

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPAlignedClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_aligned;
}
};








class OMPCopyinClause final
: public OMPVarListClause<OMPCopyinClause>,
private llvm::TrailingObjects<OMPCopyinClause, Expr *> {













friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;







OMPCopyinClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPCopyinClause>(llvm::omp::OMPC_copyin, StartLoc,
LParenLoc, EndLoc, N) {}




explicit OMPCopyinClause(unsigned N)
: OMPVarListClause<OMPCopyinClause>(llvm::omp::OMPC_copyin,
SourceLocation(), SourceLocation(),
SourceLocation(), N) {}




void setSourceExprs(ArrayRef<Expr *> SrcExprs);


MutableArrayRef<Expr *> getSourceExprs() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getSourceExprs() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}




void setDestinationExprs(ArrayRef<Expr *> DstExprs);


MutableArrayRef<Expr *> getDestinationExprs() {
return MutableArrayRef<Expr *>(getSourceExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getDestinationExprs() const {
return llvm::makeArrayRef(getSourceExprs().end(), varlist_size());
}





void setAssignmentOps(ArrayRef<Expr *> AssignmentOps);


MutableArrayRef<Expr *> getAssignmentOps() {
return MutableArrayRef<Expr *>(getDestinationExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getAssignmentOps() const {
return llvm::makeArrayRef(getDestinationExprs().end(), varlist_size());
}

public:





















static OMPCopyinClause *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL, ArrayRef<Expr *> SrcExprs,
ArrayRef<Expr *> DstExprs, ArrayRef<Expr *> AssignmentOps);





static OMPCopyinClause *CreateEmpty(const ASTContext &C, unsigned N);

using helper_expr_iterator = MutableArrayRef<Expr *>::iterator;
using helper_expr_const_iterator = ArrayRef<const Expr *>::iterator;
using helper_expr_range = llvm::iterator_range<helper_expr_iterator>;
using helper_expr_const_range =
llvm::iterator_range<helper_expr_const_iterator>;

helper_expr_const_range source_exprs() const {
return helper_expr_const_range(getSourceExprs().begin(),
getSourceExprs().end());
}

helper_expr_range source_exprs() {
return helper_expr_range(getSourceExprs().begin(), getSourceExprs().end());
}

helper_expr_const_range destination_exprs() const {
return helper_expr_const_range(getDestinationExprs().begin(),
getDestinationExprs().end());
}

helper_expr_range destination_exprs() {
return helper_expr_range(getDestinationExprs().begin(),
getDestinationExprs().end());
}

helper_expr_const_range assignment_ops() const {
return helper_expr_const_range(getAssignmentOps().begin(),
getAssignmentOps().end());
}

helper_expr_range assignment_ops() {
return helper_expr_range(getAssignmentOps().begin(),
getAssignmentOps().end());
}

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPCopyinClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_copyin;
}
};









class OMPCopyprivateClause final
: public OMPVarListClause<OMPCopyprivateClause>,
private llvm::TrailingObjects<OMPCopyprivateClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;







OMPCopyprivateClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPCopyprivateClause>(llvm::omp::OMPC_copyprivate,
StartLoc, LParenLoc, EndLoc, N) {
}




explicit OMPCopyprivateClause(unsigned N)
: OMPVarListClause<OMPCopyprivateClause>(
llvm::omp::OMPC_copyprivate, SourceLocation(), SourceLocation(),
SourceLocation(), N) {}




void setSourceExprs(ArrayRef<Expr *> SrcExprs);


MutableArrayRef<Expr *> getSourceExprs() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getSourceExprs() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}




void setDestinationExprs(ArrayRef<Expr *> DstExprs);


MutableArrayRef<Expr *> getDestinationExprs() {
return MutableArrayRef<Expr *>(getSourceExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getDestinationExprs() const {
return llvm::makeArrayRef(getSourceExprs().end(), varlist_size());
}





void setAssignmentOps(ArrayRef<Expr *> AssignmentOps);


MutableArrayRef<Expr *> getAssignmentOps() {
return MutableArrayRef<Expr *>(getDestinationExprs().end(), varlist_size());
}
ArrayRef<const Expr *> getAssignmentOps() const {
return llvm::makeArrayRef(getDestinationExprs().end(), varlist_size());
}

public:




















static OMPCopyprivateClause *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL, ArrayRef<Expr *> SrcExprs,
ArrayRef<Expr *> DstExprs, ArrayRef<Expr *> AssignmentOps);





static OMPCopyprivateClause *CreateEmpty(const ASTContext &C, unsigned N);

using helper_expr_iterator = MutableArrayRef<Expr *>::iterator;
using helper_expr_const_iterator = ArrayRef<const Expr *>::iterator;
using helper_expr_range = llvm::iterator_range<helper_expr_iterator>;
using helper_expr_const_range =
llvm::iterator_range<helper_expr_const_iterator>;

helper_expr_const_range source_exprs() const {
return helper_expr_const_range(getSourceExprs().begin(),
getSourceExprs().end());
}

helper_expr_range source_exprs() {
return helper_expr_range(getSourceExprs().begin(), getSourceExprs().end());
}

helper_expr_const_range destination_exprs() const {
return helper_expr_const_range(getDestinationExprs().begin(),
getDestinationExprs().end());
}

helper_expr_range destination_exprs() {
return helper_expr_range(getDestinationExprs().begin(),
getDestinationExprs().end());
}

helper_expr_const_range assignment_ops() const {
return helper_expr_const_range(getAssignmentOps().begin(),
getAssignmentOps().end());
}

helper_expr_range assignment_ops() {
return helper_expr_range(getAssignmentOps().begin(),
getAssignmentOps().end());
}

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPCopyprivateClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_copyprivate;
}
};













class OMPFlushClause final
: public OMPVarListClause<OMPFlushClause>,
private llvm::TrailingObjects<OMPFlushClause, Expr *> {
friend OMPVarListClause;
friend TrailingObjects;







OMPFlushClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPFlushClause>(llvm::omp::OMPC_flush, StartLoc,
LParenLoc, EndLoc, N) {}




explicit OMPFlushClause(unsigned N)
: OMPVarListClause<OMPFlushClause>(llvm::omp::OMPC_flush,
SourceLocation(), SourceLocation(),
SourceLocation(), N) {}

public:







static OMPFlushClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc,
ArrayRef<Expr *> VL);





static OMPFlushClause *CreateEmpty(const ASTContext &C, unsigned N);

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPFlushClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_flush;
}
};













class OMPDepobjClause final : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Expr *Depobj = nullptr;






OMPDepobjClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_depobj, StartLoc, EndLoc),
LParenLoc(LParenLoc) {}



explicit OMPDepobjClause()
: OMPClause(llvm::omp::OMPC_depobj, SourceLocation(), SourceLocation()) {}

void setDepobj(Expr *E) { Depobj = E; }


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }

public:







static OMPDepobjClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation EndLoc, Expr *Depobj);




static OMPDepobjClause *CreateEmpty(const ASTContext &C);


Expr *getDepobj() { return Depobj; }
const Expr *getDepobj() const { return Depobj; }


SourceLocation getLParenLoc() const { return LParenLoc; }

child_range children() {
return child_range(reinterpret_cast<Stmt **>(&Depobj),
reinterpret_cast<Stmt **>(&Depobj) + 1);
}

const_child_range children() const {
auto Children = const_cast<OMPDepobjClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_depobj;
}
};









class OMPDependClause final
: public OMPVarListClause<OMPDependClause>,
private llvm::TrailingObjects<OMPDependClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;


OpenMPDependClauseKind DepKind = OMPC_DEPEND_unknown;


SourceLocation DepLoc;


SourceLocation ColonLoc;


unsigned NumLoops = 0;









OMPDependClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N, unsigned NumLoops)
: OMPVarListClause<OMPDependClause>(llvm::omp::OMPC_depend, StartLoc,
LParenLoc, EndLoc, N),
NumLoops(NumLoops) {}






explicit OMPDependClause(unsigned N, unsigned NumLoops)
: OMPVarListClause<OMPDependClause>(llvm::omp::OMPC_depend,
SourceLocation(), SourceLocation(),
SourceLocation(), N),
NumLoops(NumLoops) {}


void setDependencyKind(OpenMPDependClauseKind K) { DepKind = K; }


void setDependencyLoc(SourceLocation Loc) { DepLoc = Loc; }


void setColonLoc(SourceLocation Loc) { ColonLoc = Loc; }


void setModifier(Expr *DepModifier);

public:












static OMPDependClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation EndLoc, Expr *DepModifier,
OpenMPDependClauseKind DepKind,
SourceLocation DepLoc, SourceLocation ColonLoc,
ArrayRef<Expr *> VL, unsigned NumLoops);







static OMPDependClause *CreateEmpty(const ASTContext &C, unsigned N,
unsigned NumLoops);


OpenMPDependClauseKind getDependencyKind() const { return DepKind; }


Expr *getModifier();
const Expr *getModifier() const {
return const_cast<OMPDependClause *>(this)->getModifier();
}


SourceLocation getDependencyLoc() const { return DepLoc; }


SourceLocation getColonLoc() const { return ColonLoc; }


unsigned getNumLoops() const { return NumLoops; }



void setLoopData(unsigned NumLoop, Expr *Cnt);


Expr *getLoopData(unsigned NumLoop);
const Expr *getLoopData(unsigned NumLoop) const;

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPDependClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_depend;
}
};









class OMPDeviceClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


OpenMPDeviceClauseModifier Modifier = OMPC_DEVICE_unknown;


SourceLocation ModifierLoc;


Stmt *Device = nullptr;




void setDevice(Expr *E) { Device = E; }


void setModifier(OpenMPDeviceClauseModifier M) { Modifier = M; }


void setModifierLoc(SourceLocation Loc) { ModifierLoc = Loc; }

public:










OMPDeviceClause(OpenMPDeviceClauseModifier Modifier, Expr *E, Stmt *HelperE,
OpenMPDirectiveKind CaptureRegion, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation ModifierLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_device, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), Modifier(Modifier),
ModifierLoc(ModifierLoc), Device(E) {
setPreInitStmt(HelperE, CaptureRegion);
}


OMPDeviceClause()
: OMPClause(llvm::omp::OMPC_device, SourceLocation(), SourceLocation()),
OMPClauseWithPreInit(this) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getDevice() { return cast<Expr>(Device); }


Expr *getDevice() const { return cast<Expr>(Device); }


OpenMPDeviceClauseModifier getModifier() const { return Modifier; }


SourceLocation getModifierLoc() const { return ModifierLoc; }

child_range children() { return child_range(&Device, &Device + 1); }

const_child_range children() const {
return const_child_range(&Device, &Device + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_device;
}
};







class OMPThreadsClause : public OMPClause {
public:




OMPThreadsClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_threads, StartLoc, EndLoc) {}


OMPThreadsClause()
: OMPClause(llvm::omp::OMPC_threads, SourceLocation(), SourceLocation()) {
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_threads;
}
};







class OMPSIMDClause : public OMPClause {
public:




OMPSIMDClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_simd, StartLoc, EndLoc) {}


OMPSIMDClause()
: OMPClause(llvm::omp::OMPC_simd, SourceLocation(), SourceLocation()) {}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_simd;
}
};



class OMPClauseMappableExprCommon {
public:






class MappableComponent {


llvm::PointerIntPair<Expr *, 1, bool> AssociatedExpressionNonContiguousPr;




ValueDecl *AssociatedDeclaration = nullptr;

public:
explicit MappableComponent() = default;
explicit MappableComponent(Expr *AssociatedExpression,
ValueDecl *AssociatedDeclaration,
bool IsNonContiguous)
: AssociatedExpressionNonContiguousPr(AssociatedExpression,
IsNonContiguous),
AssociatedDeclaration(
AssociatedDeclaration
? cast<ValueDecl>(AssociatedDeclaration->getCanonicalDecl())
: nullptr) {}

Expr *getAssociatedExpression() const {
return AssociatedExpressionNonContiguousPr.getPointer();
}

bool isNonContiguous() const {
return AssociatedExpressionNonContiguousPr.getInt();
}

ValueDecl *getAssociatedDeclaration() const {
return AssociatedDeclaration;
}
};



using MappableExprComponentList = SmallVector<MappableComponent, 8>;
using MappableExprComponentListRef = ArrayRef<MappableComponent>;




using MappableExprComponentLists = SmallVector<MappableExprComponentList, 8>;
using MappableExprComponentListsRef = ArrayRef<MappableExprComponentList>;

protected:

static unsigned
getComponentsTotalNumber(MappableExprComponentListsRef ComponentLists);



static unsigned
getUniqueDeclarationsTotalNumber(ArrayRef<const ValueDecl *> Declarations);
};



struct OMPMappableExprListSizeTy {

unsigned NumVars;

unsigned NumUniqueDeclarations;

unsigned NumComponentLists;

unsigned NumComponents;
OMPMappableExprListSizeTy() = default;
OMPMappableExprListSizeTy(unsigned NumVars, unsigned NumUniqueDeclarations,
unsigned NumComponentLists, unsigned NumComponents)
: NumVars(NumVars), NumUniqueDeclarations(NumUniqueDeclarations),
NumComponentLists(NumComponentLists), NumComponents(NumComponents) {}
};





template <class T>
class OMPMappableExprListClause : public OMPVarListClause<T>,
public OMPClauseMappableExprCommon {
friend class OMPClauseReader;


unsigned NumUniqueDeclarations;


unsigned NumComponentLists;


unsigned NumComponents;




const bool SupportsMapper;


NestedNameSpecifierLoc MapperQualifierLoc;


DeclarationNameInfo MapperIdInfo;

protected:


















OMPMappableExprListClause(
OpenMPClauseKind K, const OMPVarListLocTy &Locs,
const OMPMappableExprListSizeTy &Sizes, bool SupportsMapper = false,
NestedNameSpecifierLoc *MapperQualifierLocPtr = nullptr,
DeclarationNameInfo *MapperIdInfoPtr = nullptr)
: OMPVarListClause<T>(K, Locs.StartLoc, Locs.LParenLoc, Locs.EndLoc,
Sizes.NumVars),
NumUniqueDeclarations(Sizes.NumUniqueDeclarations),
NumComponentLists(Sizes.NumComponentLists),
NumComponents(Sizes.NumComponents), SupportsMapper(SupportsMapper) {
if (MapperQualifierLocPtr)
MapperQualifierLoc = *MapperQualifierLocPtr;
if (MapperIdInfoPtr)
MapperIdInfo = *MapperIdInfoPtr;
}



MutableArrayRef<ValueDecl *> getUniqueDeclsRef() {
return MutableArrayRef<ValueDecl *>(
static_cast<T *>(this)->template getTrailingObjects<ValueDecl *>(),
NumUniqueDeclarations);
}



ArrayRef<ValueDecl *> getUniqueDeclsRef() const {
return ArrayRef<ValueDecl *>(
static_cast<const T *>(this)
->template getTrailingObjects<ValueDecl *>(),
NumUniqueDeclarations);
}



void setUniqueDecls(ArrayRef<ValueDecl *> UDs) {
assert(UDs.size() == NumUniqueDeclarations &&
"Unexpected amount of unique declarations.");
std::copy(UDs.begin(), UDs.end(), getUniqueDeclsRef().begin());
}



MutableArrayRef<unsigned> getDeclNumListsRef() {
return MutableArrayRef<unsigned>(
static_cast<T *>(this)->template getTrailingObjects<unsigned>(),
NumUniqueDeclarations);
}



ArrayRef<unsigned> getDeclNumListsRef() const {
return ArrayRef<unsigned>(
static_cast<const T *>(this)->template getTrailingObjects<unsigned>(),
NumUniqueDeclarations);
}



void setDeclNumLists(ArrayRef<unsigned> DNLs) {
assert(DNLs.size() == NumUniqueDeclarations &&
"Unexpected amount of list numbers.");
std::copy(DNLs.begin(), DNLs.end(), getDeclNumListsRef().begin());
}



MutableArrayRef<unsigned> getComponentListSizesRef() {
return MutableArrayRef<unsigned>(
static_cast<T *>(this)->template getTrailingObjects<unsigned>() +
NumUniqueDeclarations,
NumComponentLists);
}



ArrayRef<unsigned> getComponentListSizesRef() const {
return ArrayRef<unsigned>(
static_cast<const T *>(this)->template getTrailingObjects<unsigned>() +
NumUniqueDeclarations,
NumComponentLists);
}



void setComponentListSizes(ArrayRef<unsigned> CLSs) {
assert(CLSs.size() == NumComponentLists &&
"Unexpected amount of component lists.");
std::copy(CLSs.begin(), CLSs.end(), getComponentListSizesRef().begin());
}


MutableArrayRef<MappableComponent> getComponentsRef() {
return MutableArrayRef<MappableComponent>(
static_cast<T *>(this)
->template getTrailingObjects<MappableComponent>(),
NumComponents);
}


ArrayRef<MappableComponent> getComponentsRef() const {
return ArrayRef<MappableComponent>(
static_cast<const T *>(this)
->template getTrailingObjects<MappableComponent>(),
NumComponents);
}




void setComponents(ArrayRef<MappableComponent> Components,
ArrayRef<unsigned> CLSs) {
assert(Components.size() == NumComponents &&
"Unexpected amount of component lists.");
assert(CLSs.size() == NumComponentLists &&
"Unexpected amount of list sizes.");
std::copy(Components.begin(), Components.end(), getComponentsRef().begin());
}



void setClauseInfo(ArrayRef<ValueDecl *> Declarations,
MappableExprComponentListsRef ComponentLists) {


assert(getUniqueDeclarationsTotalNumber(Declarations) ==
NumUniqueDeclarations &&
"Unexpected number of mappable expression info entries!");
assert(getComponentsTotalNumber(ComponentLists) == NumComponents &&
"Unexpected total number of components!");
assert(Declarations.size() == ComponentLists.size() &&
"Declaration and component lists size is not consistent!");
assert(Declarations.size() == NumComponentLists &&
"Unexpected declaration and component lists size!");




llvm::MapVector<ValueDecl *, SmallVector<MappableExprComponentListRef, 8>>
ComponentListMap;
{
auto CI = ComponentLists.begin();
for (auto DI = Declarations.begin(), DE = Declarations.end(); DI != DE;
++DI, ++CI) {
assert(!CI->empty() && "Invalid component list!");
ComponentListMap[*DI].push_back(*CI);
}
}


auto UniqueDeclarations = getUniqueDeclsRef();
auto UDI = UniqueDeclarations.begin();

auto DeclNumLists = getDeclNumListsRef();
auto DNLI = DeclNumLists.begin();

auto ComponentListSizes = getComponentListSizesRef();
auto CLSI = ComponentListSizes.begin();

auto Components = getComponentsRef();
auto CI = Components.begin();


unsigned PrevSize = 0u;


for (auto &M : ComponentListMap) {

auto *D = M.first;

auto CL = M.second;


*UDI = D;
++UDI;

*DNLI = CL.size();
++DNLI;



for (auto C : CL) {

PrevSize += C.size();


*CLSI = PrevSize;
++CLSI;


CI = std::copy(C.begin(), C.end(), CI);
}
}
}


void setMapperQualifierLoc(NestedNameSpecifierLoc NNSL) {
MapperQualifierLoc = NNSL;
}


void setMapperIdInfo(DeclarationNameInfo MapperId) {
MapperIdInfo = MapperId;
}



MutableArrayRef<Expr *> getUDMapperRefs() {
assert(SupportsMapper &&
"Must be a clause that is possible to have user-defined mappers");
return llvm::makeMutableArrayRef<Expr *>(
static_cast<T *>(this)->template getTrailingObjects<Expr *>() +
OMPVarListClause<T>::varlist_size(),
OMPVarListClause<T>::varlist_size());
}



ArrayRef<Expr *> getUDMapperRefs() const {
assert(SupportsMapper &&
"Must be a clause that is possible to have user-defined mappers");
return llvm::makeArrayRef<Expr *>(
static_cast<const T *>(this)->template getTrailingObjects<Expr *>() +
OMPVarListClause<T>::varlist_size(),
OMPVarListClause<T>::varlist_size());
}



void setUDMapperRefs(ArrayRef<Expr *> DMDs) {
assert(DMDs.size() == OMPVarListClause<T>::varlist_size() &&
"Unexpected number of user-defined mappers.");
assert(SupportsMapper &&
"Must be a clause that is possible to have user-defined mappers");
std::copy(DMDs.begin(), DMDs.end(), getUDMapperRefs().begin());
}

public:

unsigned getUniqueDeclarationsNum() const { return NumUniqueDeclarations; }


unsigned getTotalComponentListNum() const { return NumComponentLists; }



unsigned getTotalComponentsNum() const { return NumComponents; }


NestedNameSpecifierLoc getMapperQualifierLoc() const {
return MapperQualifierLoc;
}


const DeclarationNameInfo &getMapperIdInfo() const { return MapperIdInfo; }



class const_component_lists_iterator
: public llvm::iterator_adaptor_base<
const_component_lists_iterator,
MappableExprComponentListRef::const_iterator,
std::forward_iterator_tag, MappableComponent, ptrdiff_t,
MappableComponent, MappableComponent> {

ArrayRef<ValueDecl *>::iterator DeclCur;


ArrayRef<unsigned>::iterator NumListsCur;


const bool SupportsMapper;


ArrayRef<Expr *>::iterator MapperCur;


unsigned RemainingLists = 0;



unsigned PrevListSize = 0;



ArrayRef<unsigned>::const_iterator ListSizeCur;
ArrayRef<unsigned>::const_iterator ListSizeEnd;


MappableExprComponentListRef::const_iterator End;

public:

explicit const_component_lists_iterator(
ArrayRef<ValueDecl *> UniqueDecls, ArrayRef<unsigned> DeclsListNum,
ArrayRef<unsigned> CumulativeListSizes,
MappableExprComponentListRef Components, bool SupportsMapper,
ArrayRef<Expr *> Mappers)
: const_component_lists_iterator::iterator_adaptor_base(
Components.begin()),
DeclCur(UniqueDecls.begin()), NumListsCur(DeclsListNum.begin()),
SupportsMapper(SupportsMapper),
ListSizeCur(CumulativeListSizes.begin()),
ListSizeEnd(CumulativeListSizes.end()), End(Components.end()) {
assert(UniqueDecls.size() == DeclsListNum.size() &&
"Inconsistent number of declarations and list sizes!");
if (!DeclsListNum.empty())
RemainingLists = *NumListsCur;
if (SupportsMapper)
MapperCur = Mappers.begin();
}



explicit const_component_lists_iterator(
const ValueDecl *Declaration, ArrayRef<ValueDecl *> UniqueDecls,
ArrayRef<unsigned> DeclsListNum, ArrayRef<unsigned> CumulativeListSizes,
MappableExprComponentListRef Components, bool SupportsMapper,
ArrayRef<Expr *> Mappers)
: const_component_lists_iterator(UniqueDecls, DeclsListNum,
CumulativeListSizes, Components,
SupportsMapper, Mappers) {



for (; DeclCur != UniqueDecls.end(); ++DeclCur, ++NumListsCur) {
if (*DeclCur == Declaration)
break;

assert(*NumListsCur > 0 && "No lists associated with declaration??");



std::advance(ListSizeCur, *NumListsCur - 1);
PrevListSize = *ListSizeCur;
++ListSizeCur;

if (SupportsMapper)
++MapperCur;
}



if (ListSizeCur == CumulativeListSizes.end()) {
this->I = End;
RemainingLists = 0u;
return;
}



RemainingLists = *NumListsCur;


ListSizeEnd = ListSizeCur;
std::advance(ListSizeEnd, RemainingLists);



std::advance(this->I, PrevListSize);
}



std::tuple<const ValueDecl *, MappableExprComponentListRef,
const ValueDecl *>
operator*() const {
assert(ListSizeCur != ListSizeEnd && "Invalid iterator!");
const ValueDecl *Mapper = nullptr;
if (SupportsMapper && *MapperCur)
Mapper = cast<ValueDecl>(cast<DeclRefExpr>(*MapperCur)->getDecl());
return std::make_tuple(
*DeclCur,
MappableExprComponentListRef(&*this->I, *ListSizeCur - PrevListSize),
Mapper);
}
std::tuple<const ValueDecl *, MappableExprComponentListRef,
const ValueDecl *>
operator->() const {
return **this;
}


const_component_lists_iterator &operator++() {
assert(ListSizeCur != ListSizeEnd && RemainingLists &&
"Invalid iterator!");



if (std::next(ListSizeCur) == ListSizeEnd) {
this->I = End;
RemainingLists = 0;
} else {
std::advance(this->I, *ListSizeCur - PrevListSize);
PrevListSize = *ListSizeCur;


if (!(--RemainingLists)) {
++DeclCur;
++NumListsCur;
RemainingLists = *NumListsCur;
assert(RemainingLists && "No lists in the following declaration??");
}
}

++ListSizeCur;
if (SupportsMapper)
++MapperCur;
return *this;
}
};

using const_component_lists_range =
llvm::iterator_range<const_component_lists_iterator>;


const_component_lists_iterator component_lists_begin() const {
return const_component_lists_iterator(
getUniqueDeclsRef(), getDeclNumListsRef(), getComponentListSizesRef(),
getComponentsRef(), SupportsMapper,
SupportsMapper ? getUDMapperRefs() : llvm::None);
}
const_component_lists_iterator component_lists_end() const {
return const_component_lists_iterator(
ArrayRef<ValueDecl *>(), ArrayRef<unsigned>(), ArrayRef<unsigned>(),
MappableExprComponentListRef(getComponentsRef().end(),
getComponentsRef().end()),
SupportsMapper, llvm::None);
}
const_component_lists_range component_lists() const {
return {component_lists_begin(), component_lists_end()};
}



const_component_lists_iterator
decl_component_lists_begin(const ValueDecl *VD) const {
return const_component_lists_iterator(
VD, getUniqueDeclsRef(), getDeclNumListsRef(),
getComponentListSizesRef(), getComponentsRef(), SupportsMapper,
SupportsMapper ? getUDMapperRefs() : llvm::None);
}
const_component_lists_iterator decl_component_lists_end() const {
return component_lists_end();
}
const_component_lists_range decl_component_lists(const ValueDecl *VD) const {
return {decl_component_lists_begin(VD), decl_component_lists_end()};
}



using const_all_decls_iterator = ArrayRef<ValueDecl *>::iterator;
using const_all_decls_range = llvm::iterator_range<const_all_decls_iterator>;

const_all_decls_range all_decls() const {
auto A = getUniqueDeclsRef();
return const_all_decls_range(A.begin(), A.end());
}

using const_all_num_lists_iterator = ArrayRef<unsigned>::iterator;
using const_all_num_lists_range =
llvm::iterator_range<const_all_num_lists_iterator>;

const_all_num_lists_range all_num_lists() const {
auto A = getDeclNumListsRef();
return const_all_num_lists_range(A.begin(), A.end());
}

using const_all_lists_sizes_iterator = ArrayRef<unsigned>::iterator;
using const_all_lists_sizes_range =
llvm::iterator_range<const_all_lists_sizes_iterator>;

const_all_lists_sizes_range all_lists_sizes() const {
auto A = getComponentListSizesRef();
return const_all_lists_sizes_range(A.begin(), A.end());
}

using const_all_components_iterator = ArrayRef<MappableComponent>::iterator;
using const_all_components_range =
llvm::iterator_range<const_all_components_iterator>;

const_all_components_range all_components() const {
auto A = getComponentsRef();
return const_all_components_range(A.begin(), A.end());
}

using mapperlist_iterator = MutableArrayRef<Expr *>::iterator;
using mapperlist_const_iterator = ArrayRef<const Expr *>::iterator;
using mapperlist_range = llvm::iterator_range<mapperlist_iterator>;
using mapperlist_const_range =
llvm::iterator_range<mapperlist_const_iterator>;

mapperlist_iterator mapperlist_begin() { return getUDMapperRefs().begin(); }
mapperlist_iterator mapperlist_end() { return getUDMapperRefs().end(); }
mapperlist_const_iterator mapperlist_begin() const {
return getUDMapperRefs().begin();
}
mapperlist_const_iterator mapperlist_end() const {
return getUDMapperRefs().end();
}
mapperlist_range mapperlists() {
return mapperlist_range(mapperlist_begin(), mapperlist_end());
}
mapperlist_const_range mapperlists() const {
return mapperlist_const_range(mapperlist_begin(), mapperlist_end());
}
};









class OMPMapClause final : public OMPMappableExprListClause<OMPMapClause>,
private llvm::TrailingObjects<
OMPMapClause, Expr *, ValueDecl *, unsigned,
OMPClauseMappableExprCommon::MappableComponent> {
friend class OMPClauseReader;
friend OMPMappableExprListClause;
friend OMPVarListClause;
friend TrailingObjects;



size_t numTrailingObjects(OverloadToken<Expr *>) const {


return 2 * varlist_size();
}
size_t numTrailingObjects(OverloadToken<ValueDecl *>) const {
return getUniqueDeclarationsNum();
}
size_t numTrailingObjects(OverloadToken<unsigned>) const {
return getUniqueDeclarationsNum() + getTotalComponentListNum();
}

private:

OpenMPMapModifierKind MapTypeModifiers[NumberOfOMPMapClauseModifiers] = {
OMPC_MAP_MODIFIER_unknown, OMPC_MAP_MODIFIER_unknown,
OMPC_MAP_MODIFIER_unknown, OMPC_MAP_MODIFIER_unknown};


SourceLocation MapTypeModifiersLoc[NumberOfOMPMapClauseModifiers];


OpenMPMapClauseKind MapType = OMPC_MAP_unknown;


bool MapTypeIsImplicit = false;


SourceLocation MapLoc;


SourceLocation ColonLoc;





















explicit OMPMapClause(ArrayRef<OpenMPMapModifierKind> MapModifiers,
ArrayRef<SourceLocation> MapModifiersLoc,
NestedNameSpecifierLoc MapperQualifierLoc,
DeclarationNameInfo MapperIdInfo,
OpenMPMapClauseKind MapType, bool MapTypeIsImplicit,
SourceLocation MapLoc, const OMPVarListLocTy &Locs,
const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_map, Locs, Sizes,
true, &MapperQualifierLoc,
&MapperIdInfo),
MapType(MapType), MapTypeIsImplicit(MapTypeIsImplicit), MapLoc(MapLoc) {
assert(llvm::array_lengthof(MapTypeModifiers) == MapModifiers.size() &&
"Unexpected number of map type modifiers.");
llvm::copy(MapModifiers, std::begin(MapTypeModifiers));

assert(llvm::array_lengthof(MapTypeModifiersLoc) ==
MapModifiersLoc.size() &&
"Unexpected number of map type modifier locations.");
llvm::copy(MapModifiersLoc, std::begin(MapTypeModifiersLoc));
}








explicit OMPMapClause(const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_map, OMPVarListLocTy(), Sizes,
true) {}





void setMapTypeModifier(unsigned I, OpenMPMapModifierKind T) {
assert(I < NumberOfOMPMapClauseModifiers &&
"Unexpected index to store map type modifier, exceeds array size.");
MapTypeModifiers[I] = T;
}





void setMapTypeModifierLoc(unsigned I, SourceLocation TLoc) {
assert(I < NumberOfOMPMapClauseModifiers &&
"Index to store map type modifier location exceeds array size.");
MapTypeModifiersLoc[I] = TLoc;
}




void setMapType(OpenMPMapClauseKind T) { MapType = T; }




void setMapLoc(SourceLocation TLoc) { MapLoc = TLoc; }


void setColonLoc(SourceLocation Loc) { ColonLoc = Loc; }

public:



















static OMPMapClause *
Create(const ASTContext &C, const OMPVarListLocTy &Locs,
ArrayRef<Expr *> Vars, ArrayRef<ValueDecl *> Declarations,
MappableExprComponentListsRef ComponentLists,
ArrayRef<Expr *> UDMapperRefs,
ArrayRef<OpenMPMapModifierKind> MapModifiers,
ArrayRef<SourceLocation> MapModifiersLoc,
NestedNameSpecifierLoc UDMQualifierLoc, DeclarationNameInfo MapperId,
OpenMPMapClauseKind Type, bool TypeIsImplicit, SourceLocation TypeLoc);











static OMPMapClause *CreateEmpty(const ASTContext &C,
const OMPMappableExprListSizeTy &Sizes);


OpenMPMapClauseKind getMapType() const LLVM_READONLY { return MapType; }






bool isImplicitMapType() const LLVM_READONLY { return MapTypeIsImplicit; }




OpenMPMapModifierKind getMapTypeModifier(unsigned Cnt) const LLVM_READONLY {
assert(Cnt < NumberOfOMPMapClauseModifiers &&
"Requested modifier exceeds the total number of modifiers.");
return MapTypeModifiers[Cnt];
}





SourceLocation getMapTypeModifierLoc(unsigned Cnt) const LLVM_READONLY {
assert(Cnt < NumberOfOMPMapClauseModifiers &&
"Requested modifier location exceeds total number of modifiers.");
return MapTypeModifiersLoc[Cnt];
}


ArrayRef<OpenMPMapModifierKind> getMapTypeModifiers() const LLVM_READONLY {
return llvm::makeArrayRef(MapTypeModifiers);
}


ArrayRef<SourceLocation> getMapTypeModifiersLoc() const LLVM_READONLY {
return llvm::makeArrayRef(MapTypeModifiersLoc);
}


SourceLocation getMapLoc() const LLVM_READONLY { return MapLoc; }


SourceLocation getColonLoc() const { return ColonLoc; }

child_range children() {
return child_range(
reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPMapClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
if (MapType == OMPC_MAP_to || MapType == OMPC_MAP_tofrom)
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
auto Children = const_cast<OMPMapClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}


static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_map;
}
};









class OMPNumTeamsClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *NumTeams = nullptr;




void setNumTeams(Expr *E) { NumTeams = E; }

public:









OMPNumTeamsClause(Expr *E, Stmt *HelperE, OpenMPDirectiveKind CaptureRegion,
SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_num_teams, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), NumTeams(E) {
setPreInitStmt(HelperE, CaptureRegion);
}


OMPNumTeamsClause()
: OMPClause(llvm::omp::OMPC_num_teams, SourceLocation(),
SourceLocation()),
OMPClauseWithPreInit(this) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getNumTeams() { return cast<Expr>(NumTeams); }


Expr *getNumTeams() const { return cast<Expr>(NumTeams); }

child_range children() { return child_range(&NumTeams, &NumTeams + 1); }

const_child_range children() const {
return const_child_range(&NumTeams, &NumTeams + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_num_teams;
}
};









class OMPThreadLimitClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *ThreadLimit = nullptr;




void setThreadLimit(Expr *E) { ThreadLimit = E; }

public:









OMPThreadLimitClause(Expr *E, Stmt *HelperE,
OpenMPDirectiveKind CaptureRegion,
SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_thread_limit, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), ThreadLimit(E) {
setPreInitStmt(HelperE, CaptureRegion);
}


OMPThreadLimitClause()
: OMPClause(llvm::omp::OMPC_thread_limit, SourceLocation(),
SourceLocation()),
OMPClauseWithPreInit(this) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getThreadLimit() { return cast<Expr>(ThreadLimit); }


Expr *getThreadLimit() const { return cast<Expr>(ThreadLimit); }

child_range children() { return child_range(&ThreadLimit, &ThreadLimit + 1); }

const_child_range children() const {
return const_child_range(&ThreadLimit, &ThreadLimit + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_thread_limit;
}
};









class OMPPriorityClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Priority = nullptr;




void setPriority(Expr *E) { Priority = E; }

public:









OMPPriorityClause(Expr *Priority, Stmt *HelperPriority,
OpenMPDirectiveKind CaptureRegion, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_priority, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), Priority(Priority) {
setPreInitStmt(HelperPriority, CaptureRegion);
}


OMPPriorityClause()
: OMPClause(llvm::omp::OMPC_priority, SourceLocation(), SourceLocation()),
OMPClauseWithPreInit(this) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getPriority() { return cast<Expr>(Priority); }


Expr *getPriority() const { return cast<Expr>(Priority); }

child_range children() { return child_range(&Priority, &Priority + 1); }

const_child_range children() const {
return const_child_range(&Priority, &Priority + 1);
}

child_range used_children();
const_child_range used_children() const {
auto Children = const_cast<OMPPriorityClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_priority;
}
};









class OMPGrainsizeClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Grainsize = nullptr;


void setGrainsize(Expr *Size) { Grainsize = Size; }

public:








OMPGrainsizeClause(Expr *Size, Stmt *HelperSize,
OpenMPDirectiveKind CaptureRegion, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_grainsize, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), Grainsize(Size) {
setPreInitStmt(HelperSize, CaptureRegion);
}


explicit OMPGrainsizeClause()
: OMPClause(llvm::omp::OMPC_grainsize, SourceLocation(),
SourceLocation()),
OMPClauseWithPreInit(this) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getGrainsize() const { return cast_or_null<Expr>(Grainsize); }

child_range children() { return child_range(&Grainsize, &Grainsize + 1); }

const_child_range children() const {
return const_child_range(&Grainsize, &Grainsize + 1);
}

child_range used_children();
const_child_range used_children() const {
auto Children = const_cast<OMPGrainsizeClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_grainsize;
}
};







class OMPNogroupClause : public OMPClause {
public:




OMPNogroupClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_nogroup, StartLoc, EndLoc) {}


OMPNogroupClause()
: OMPClause(llvm::omp::OMPC_nogroup, SourceLocation(), SourceLocation()) {
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_nogroup;
}
};









class OMPNumTasksClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *NumTasks = nullptr;


void setNumTasks(Expr *Size) { NumTasks = Size; }

public:








OMPNumTasksClause(Expr *Size, Stmt *HelperSize,
OpenMPDirectiveKind CaptureRegion, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_num_tasks, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), NumTasks(Size) {
setPreInitStmt(HelperSize, CaptureRegion);
}


explicit OMPNumTasksClause()
: OMPClause(llvm::omp::OMPC_num_tasks, SourceLocation(),
SourceLocation()),
OMPClauseWithPreInit(this) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getNumTasks() const { return cast_or_null<Expr>(NumTasks); }

child_range children() { return child_range(&NumTasks, &NumTasks + 1); }

const_child_range children() const {
return const_child_range(&NumTasks, &NumTasks + 1);
}

child_range used_children();
const_child_range used_children() const {
auto Children = const_cast<OMPNumTasksClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_num_tasks;
}
};








class OMPHintClause : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Hint = nullptr;


void setHint(Expr *H) { Hint = H; }

public:






OMPHintClause(Expr *Hint, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_hint, StartLoc, EndLoc), LParenLoc(LParenLoc),
Hint(Hint) {}


OMPHintClause()
: OMPClause(llvm::omp::OMPC_hint, SourceLocation(), SourceLocation()) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getHint() const { return cast_or_null<Expr>(Hint); }

child_range children() { return child_range(&Hint, &Hint + 1); }

const_child_range children() const {
return const_child_range(&Hint, &Hint + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_hint;
}
};









class OMPDistScheduleClause : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


OpenMPDistScheduleClauseKind Kind = OMPC_DIST_SCHEDULE_unknown;


SourceLocation KindLoc;


SourceLocation CommaLoc;


Expr *ChunkSize = nullptr;




void setDistScheduleKind(OpenMPDistScheduleClauseKind K) { Kind = K; }




void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }




void setDistScheduleKindLoc(SourceLocation KLoc) { KindLoc = KLoc; }




void setCommaLoc(SourceLocation Loc) { CommaLoc = Loc; }




void setChunkSize(Expr *E) { ChunkSize = E; }

public:











OMPDistScheduleClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation KLoc, SourceLocation CommaLoc,
SourceLocation EndLoc,
OpenMPDistScheduleClauseKind Kind, Expr *ChunkSize,
Stmt *HelperChunkSize)
: OMPClause(llvm::omp::OMPC_dist_schedule, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), Kind(Kind),
KindLoc(KLoc), CommaLoc(CommaLoc), ChunkSize(ChunkSize) {
setPreInitStmt(HelperChunkSize);
}


explicit OMPDistScheduleClause()
: OMPClause(llvm::omp::OMPC_dist_schedule, SourceLocation(),
SourceLocation()),
OMPClauseWithPreInit(this) {}


OpenMPDistScheduleClauseKind getDistScheduleKind() const { return Kind; }


SourceLocation getLParenLoc() { return LParenLoc; }


SourceLocation getDistScheduleKindLoc() { return KindLoc; }


SourceLocation getCommaLoc() { return CommaLoc; }


Expr *getChunkSize() { return ChunkSize; }


const Expr *getChunkSize() const { return ChunkSize; }

child_range children() {
return child_range(reinterpret_cast<Stmt **>(&ChunkSize),
reinterpret_cast<Stmt **>(&ChunkSize) + 1);
}

const_child_range children() const {
auto Children = const_cast<OMPDistScheduleClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_dist_schedule;
}
};








class OMPDefaultmapClause : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


OpenMPDefaultmapClauseModifier Modifier = OMPC_DEFAULTMAP_MODIFIER_unknown;


SourceLocation ModifierLoc;


OpenMPDefaultmapClauseKind Kind = OMPC_DEFAULTMAP_unknown;


SourceLocation KindLoc;




void setDefaultmapKind(OpenMPDefaultmapClauseKind K) { Kind = K; }




void setDefaultmapModifier(OpenMPDefaultmapClauseModifier M) {
Modifier = M;
}


void setDefaultmapModifierLoc(SourceLocation Loc) {
ModifierLoc = Loc;
}




void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }




void setDefaultmapKindLoc(SourceLocation KLoc) { KindLoc = KLoc; }

public:









OMPDefaultmapClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation MLoc, SourceLocation KLoc,
SourceLocation EndLoc, OpenMPDefaultmapClauseKind Kind,
OpenMPDefaultmapClauseModifier M)
: OMPClause(llvm::omp::OMPC_defaultmap, StartLoc, EndLoc),
LParenLoc(LParenLoc), Modifier(M), ModifierLoc(MLoc), Kind(Kind),
KindLoc(KLoc) {}


explicit OMPDefaultmapClause()
: OMPClause(llvm::omp::OMPC_defaultmap, SourceLocation(),
SourceLocation()) {}


OpenMPDefaultmapClauseKind getDefaultmapKind() const { return Kind; }


OpenMPDefaultmapClauseModifier getDefaultmapModifier() const {
return Modifier;
}


SourceLocation getLParenLoc() { return LParenLoc; }


SourceLocation getDefaultmapKindLoc() { return KindLoc; }


SourceLocation getDefaultmapModifierLoc() const {
return ModifierLoc;
}

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_defaultmap;
}
};









class OMPToClause final : public OMPMappableExprListClause<OMPToClause>,
private llvm::TrailingObjects<
OMPToClause, Expr *, ValueDecl *, unsigned,
OMPClauseMappableExprCommon::MappableComponent> {
friend class OMPClauseReader;
friend OMPMappableExprListClause;
friend OMPVarListClause;
friend TrailingObjects;


OpenMPMotionModifierKind MotionModifiers[NumberOfOMPMotionModifiers] = {
OMPC_MOTION_MODIFIER_unknown, OMPC_MOTION_MODIFIER_unknown};


SourceLocation MotionModifiersLoc[NumberOfOMPMotionModifiers];


SourceLocation ColonLoc;
















explicit OMPToClause(ArrayRef<OpenMPMotionModifierKind> TheMotionModifiers,
ArrayRef<SourceLocation> TheMotionModifiersLoc,
NestedNameSpecifierLoc MapperQualifierLoc,
DeclarationNameInfo MapperIdInfo,
const OMPVarListLocTy &Locs,
const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_to, Locs, Sizes,
true, &MapperQualifierLoc,
&MapperIdInfo) {
assert(llvm::array_lengthof(MotionModifiers) == TheMotionModifiers.size() &&
"Unexpected number of motion modifiers.");
llvm::copy(TheMotionModifiers, std::begin(MotionModifiers));

assert(llvm::array_lengthof(MotionModifiersLoc) ==
TheMotionModifiersLoc.size() &&
"Unexpected number of motion modifier locations.");
llvm::copy(TheMotionModifiersLoc, std::begin(MotionModifiersLoc));
}








explicit OMPToClause(const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_to, OMPVarListLocTy(), Sizes,
true) {}





void setMotionModifier(unsigned I, OpenMPMotionModifierKind T) {
assert(I < NumberOfOMPMotionModifiers &&
"Unexpected index to store motion modifier, exceeds array size.");
MotionModifiers[I] = T;
}





void setMotionModifierLoc(unsigned I, SourceLocation TLoc) {
assert(I < NumberOfOMPMotionModifiers &&
"Index to store motion modifier location exceeds array size.");
MotionModifiersLoc[I] = TLoc;
}


void setColonLoc(SourceLocation Loc) { ColonLoc = Loc; }



size_t numTrailingObjects(OverloadToken<Expr *>) const {


return 2 * varlist_size();
}
size_t numTrailingObjects(OverloadToken<ValueDecl *>) const {
return getUniqueDeclarationsNum();
}
size_t numTrailingObjects(OverloadToken<unsigned>) const {
return getUniqueDeclarationsNum() + getTotalComponentListNum();
}

public:
















static OMPToClause *Create(const ASTContext &C, const OMPVarListLocTy &Locs,
ArrayRef<Expr *> Vars,
ArrayRef<ValueDecl *> Declarations,
MappableExprComponentListsRef ComponentLists,
ArrayRef<Expr *> UDMapperRefs,
ArrayRef<OpenMPMotionModifierKind> MotionModifiers,
ArrayRef<SourceLocation> MotionModifiersLoc,
NestedNameSpecifierLoc UDMQualifierLoc,
DeclarationNameInfo MapperId);









static OMPToClause *CreateEmpty(const ASTContext &C,
const OMPMappableExprListSizeTy &Sizes);




OpenMPMotionModifierKind getMotionModifier(unsigned Cnt) const LLVM_READONLY {
assert(Cnt < NumberOfOMPMotionModifiers &&
"Requested modifier exceeds the total number of modifiers.");
return MotionModifiers[Cnt];
}





SourceLocation getMotionModifierLoc(unsigned Cnt) const LLVM_READONLY {
assert(Cnt < NumberOfOMPMotionModifiers &&
"Requested modifier location exceeds total number of modifiers.");
return MotionModifiersLoc[Cnt];
}


ArrayRef<OpenMPMotionModifierKind> getMotionModifiers() const LLVM_READONLY {
return llvm::makeArrayRef(MotionModifiers);
}


ArrayRef<SourceLocation> getMotionModifiersLoc() const LLVM_READONLY {
return llvm::makeArrayRef(MotionModifiersLoc);
}


SourceLocation getColonLoc() const { return ColonLoc; }

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPToClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_to;
}
};









class OMPFromClause final
: public OMPMappableExprListClause<OMPFromClause>,
private llvm::TrailingObjects<
OMPFromClause, Expr *, ValueDecl *, unsigned,
OMPClauseMappableExprCommon::MappableComponent> {
friend class OMPClauseReader;
friend OMPMappableExprListClause;
friend OMPVarListClause;
friend TrailingObjects;


OpenMPMotionModifierKind MotionModifiers[NumberOfOMPMotionModifiers] = {
OMPC_MOTION_MODIFIER_unknown, OMPC_MOTION_MODIFIER_unknown};


SourceLocation MotionModifiersLoc[NumberOfOMPMotionModifiers];


SourceLocation ColonLoc;
















explicit OMPFromClause(ArrayRef<OpenMPMotionModifierKind> TheMotionModifiers,
ArrayRef<SourceLocation> TheMotionModifiersLoc,
NestedNameSpecifierLoc MapperQualifierLoc,
DeclarationNameInfo MapperIdInfo,
const OMPVarListLocTy &Locs,
const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_from, Locs, Sizes,
true, &MapperQualifierLoc,
&MapperIdInfo) {
assert(llvm::array_lengthof(MotionModifiers) == TheMotionModifiers.size() &&
"Unexpected number of motion modifiers.");
llvm::copy(TheMotionModifiers, std::begin(MotionModifiers));

assert(llvm::array_lengthof(MotionModifiersLoc) ==
TheMotionModifiersLoc.size() &&
"Unexpected number of motion modifier locations.");
llvm::copy(TheMotionModifiersLoc, std::begin(MotionModifiersLoc));
}








explicit OMPFromClause(const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_from, OMPVarListLocTy(),
Sizes, true) {}





void setMotionModifier(unsigned I, OpenMPMotionModifierKind T) {
assert(I < NumberOfOMPMotionModifiers &&
"Unexpected index to store motion modifier, exceeds array size.");
MotionModifiers[I] = T;
}





void setMotionModifierLoc(unsigned I, SourceLocation TLoc) {
assert(I < NumberOfOMPMotionModifiers &&
"Index to store motion modifier location exceeds array size.");
MotionModifiersLoc[I] = TLoc;
}


void setColonLoc(SourceLocation Loc) { ColonLoc = Loc; }



size_t numTrailingObjects(OverloadToken<Expr *>) const {


return 2 * varlist_size();
}
size_t numTrailingObjects(OverloadToken<ValueDecl *>) const {
return getUniqueDeclarationsNum();
}
size_t numTrailingObjects(OverloadToken<unsigned>) const {
return getUniqueDeclarationsNum() + getTotalComponentListNum();
}

public:
















static OMPFromClause *
Create(const ASTContext &C, const OMPVarListLocTy &Locs,
ArrayRef<Expr *> Vars, ArrayRef<ValueDecl *> Declarations,
MappableExprComponentListsRef ComponentLists,
ArrayRef<Expr *> UDMapperRefs,
ArrayRef<OpenMPMotionModifierKind> MotionModifiers,
ArrayRef<SourceLocation> MotionModifiersLoc,
NestedNameSpecifierLoc UDMQualifierLoc, DeclarationNameInfo MapperId);









static OMPFromClause *CreateEmpty(const ASTContext &C,
const OMPMappableExprListSizeTy &Sizes);




OpenMPMotionModifierKind getMotionModifier(unsigned Cnt) const LLVM_READONLY {
assert(Cnt < NumberOfOMPMotionModifiers &&
"Requested modifier exceeds the total number of modifiers.");
return MotionModifiers[Cnt];
}





SourceLocation getMotionModifierLoc(unsigned Cnt) const LLVM_READONLY {
assert(Cnt < NumberOfOMPMotionModifiers &&
"Requested modifier location exceeds total number of modifiers.");
return MotionModifiersLoc[Cnt];
}


ArrayRef<OpenMPMotionModifierKind> getMotionModifiers() const LLVM_READONLY {
return llvm::makeArrayRef(MotionModifiers);
}


ArrayRef<SourceLocation> getMotionModifiersLoc() const LLVM_READONLY {
return llvm::makeArrayRef(MotionModifiersLoc);
}


SourceLocation getColonLoc() const { return ColonLoc; }

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPFromClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_from;
}
};









class OMPUseDevicePtrClause final
: public OMPMappableExprListClause<OMPUseDevicePtrClause>,
private llvm::TrailingObjects<
OMPUseDevicePtrClause, Expr *, ValueDecl *, unsigned,
OMPClauseMappableExprCommon::MappableComponent> {
friend class OMPClauseReader;
friend OMPMappableExprListClause;
friend OMPVarListClause;
friend TrailingObjects;











explicit OMPUseDevicePtrClause(const OMPVarListLocTy &Locs,
const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_use_device_ptr, Locs, Sizes) {
}








explicit OMPUseDevicePtrClause(const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_use_device_ptr,
OMPVarListLocTy(), Sizes) {}



size_t numTrailingObjects(OverloadToken<Expr *>) const {
return 3 * varlist_size();
}
size_t numTrailingObjects(OverloadToken<ValueDecl *>) const {
return getUniqueDeclarationsNum();
}
size_t numTrailingObjects(OverloadToken<unsigned>) const {
return getUniqueDeclarationsNum() + getTotalComponentListNum();
}




void setPrivateCopies(ArrayRef<Expr *> VL);



MutableArrayRef<Expr *> getPrivateCopies() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getPrivateCopies() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}




void setInits(ArrayRef<Expr *> VL);



MutableArrayRef<Expr *> getInits() {
return MutableArrayRef<Expr *>(getPrivateCopies().end(), varlist_size());
}
ArrayRef<const Expr *> getInits() const {
return llvm::makeArrayRef(getPrivateCopies().end(), varlist_size());
}

public:











static OMPUseDevicePtrClause *
Create(const ASTContext &C, const OMPVarListLocTy &Locs,
ArrayRef<Expr *> Vars, ArrayRef<Expr *> PrivateVars,
ArrayRef<Expr *> Inits, ArrayRef<ValueDecl *> Declarations,
MappableExprComponentListsRef ComponentLists);









static OMPUseDevicePtrClause *
CreateEmpty(const ASTContext &C, const OMPMappableExprListSizeTy &Sizes);

using private_copies_iterator = MutableArrayRef<Expr *>::iterator;
using private_copies_const_iterator = ArrayRef<const Expr *>::iterator;
using private_copies_range = llvm::iterator_range<private_copies_iterator>;
using private_copies_const_range =
llvm::iterator_range<private_copies_const_iterator>;

private_copies_range private_copies() {
return private_copies_range(getPrivateCopies().begin(),
getPrivateCopies().end());
}

private_copies_const_range private_copies() const {
return private_copies_const_range(getPrivateCopies().begin(),
getPrivateCopies().end());
}

using inits_iterator = MutableArrayRef<Expr *>::iterator;
using inits_const_iterator = ArrayRef<const Expr *>::iterator;
using inits_range = llvm::iterator_range<inits_iterator>;
using inits_const_range = llvm::iterator_range<inits_const_iterator>;

inits_range inits() {
return inits_range(getInits().begin(), getInits().end());
}

inits_const_range inits() const {
return inits_const_range(getInits().begin(), getInits().end());
}

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPUseDevicePtrClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_use_device_ptr;
}
};









class OMPUseDeviceAddrClause final
: public OMPMappableExprListClause<OMPUseDeviceAddrClause>,
private llvm::TrailingObjects<
OMPUseDeviceAddrClause, Expr *, ValueDecl *, unsigned,
OMPClauseMappableExprCommon::MappableComponent> {
friend class OMPClauseReader;
friend OMPMappableExprListClause;
friend OMPVarListClause;
friend TrailingObjects;











explicit OMPUseDeviceAddrClause(const OMPVarListLocTy &Locs,
const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_use_device_addr, Locs,
Sizes) {}








explicit OMPUseDeviceAddrClause(const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_use_device_addr,
OMPVarListLocTy(), Sizes) {}



size_t numTrailingObjects(OverloadToken<Expr *>) const {
return varlist_size();
}
size_t numTrailingObjects(OverloadToken<ValueDecl *>) const {
return getUniqueDeclarationsNum();
}
size_t numTrailingObjects(OverloadToken<unsigned>) const {
return getUniqueDeclarationsNum() + getTotalComponentListNum();
}

public:









static OMPUseDeviceAddrClause *
Create(const ASTContext &C, const OMPVarListLocTy &Locs,
ArrayRef<Expr *> Vars, ArrayRef<ValueDecl *> Declarations,
MappableExprComponentListsRef ComponentLists);









static OMPUseDeviceAddrClause *
CreateEmpty(const ASTContext &C, const OMPMappableExprListSizeTy &Sizes);

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPUseDeviceAddrClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_use_device_addr;
}
};









class OMPIsDevicePtrClause final
: public OMPMappableExprListClause<OMPIsDevicePtrClause>,
private llvm::TrailingObjects<
OMPIsDevicePtrClause, Expr *, ValueDecl *, unsigned,
OMPClauseMappableExprCommon::MappableComponent> {
friend class OMPClauseReader;
friend OMPMappableExprListClause;
friend OMPVarListClause;
friend TrailingObjects;











explicit OMPIsDevicePtrClause(const OMPVarListLocTy &Locs,
const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_is_device_ptr, Locs, Sizes) {}








explicit OMPIsDevicePtrClause(const OMPMappableExprListSizeTy &Sizes)
: OMPMappableExprListClause(llvm::omp::OMPC_is_device_ptr,
OMPVarListLocTy(), Sizes) {}



size_t numTrailingObjects(OverloadToken<Expr *>) const {
return varlist_size();
}
size_t numTrailingObjects(OverloadToken<ValueDecl *>) const {
return getUniqueDeclarationsNum();
}
size_t numTrailingObjects(OverloadToken<unsigned>) const {
return getUniqueDeclarationsNum() + getTotalComponentListNum();
}

public:









static OMPIsDevicePtrClause *
Create(const ASTContext &C, const OMPVarListLocTy &Locs,
ArrayRef<Expr *> Vars, ArrayRef<ValueDecl *> Declarations,
MappableExprComponentListsRef ComponentLists);









static OMPIsDevicePtrClause *
CreateEmpty(const ASTContext &C, const OMPMappableExprListSizeTy &Sizes);

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPIsDevicePtrClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_is_device_ptr;
}
};








class OMPNontemporalClause final
: public OMPVarListClause<OMPNontemporalClause>,
private llvm::TrailingObjects<OMPNontemporalClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;







OMPNontemporalClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPNontemporalClause>(llvm::omp::OMPC_nontemporal,
StartLoc, LParenLoc, EndLoc, N) {
}




explicit OMPNontemporalClause(unsigned N)
: OMPVarListClause<OMPNontemporalClause>(
llvm::omp::OMPC_nontemporal, SourceLocation(), SourceLocation(),
SourceLocation(), N) {}



MutableArrayRef<Expr *> getPrivateRefs() {
return MutableArrayRef<Expr *>(varlist_end(), varlist_size());
}
ArrayRef<const Expr *> getPrivateRefs() const {
return llvm::makeArrayRef(varlist_end(), varlist_size());
}

public:







static OMPNontemporalClause *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL);





static OMPNontemporalClause *CreateEmpty(const ASTContext &C, unsigned N);



void setPrivateRefs(ArrayRef<Expr *> VL);

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPNontemporalClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range private_refs() {
return child_range(reinterpret_cast<Stmt **>(getPrivateRefs().begin()),
reinterpret_cast<Stmt **>(getPrivateRefs().end()));
}

const_child_range private_refs() const {
auto Children = const_cast<OMPNontemporalClause *>(this)->private_refs();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_nontemporal;
}
};








class OMPOrderClause final : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


OpenMPOrderClauseKind Kind = OMPC_ORDER_unknown;


SourceLocation KindKwLoc;




void setKind(OpenMPOrderClauseKind K) { Kind = K; }




void setKindKwLoc(SourceLocation KLoc) { KindKwLoc = KLoc; }

public:







OMPOrderClause(OpenMPOrderClauseKind A, SourceLocation ALoc,
SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_order, StartLoc, EndLoc),
LParenLoc(LParenLoc), Kind(A), KindKwLoc(ALoc) {}


OMPOrderClause()
: OMPClause(llvm::omp::OMPC_order, SourceLocation(), SourceLocation()) {}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


SourceLocation getLParenLoc() const { return LParenLoc; }


OpenMPOrderClauseKind getKind() const { return Kind; }


SourceLocation getKindKwLoc() const { return KindKwLoc; }

child_range children() {
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_order;
}
};






class OMPInitClause final
: public OMPVarListClause<OMPInitClause>,
private llvm::TrailingObjects<OMPInitClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;


SourceLocation VarLoc;

bool IsTarget = false;
bool IsTargetSync = false;

void setInteropVar(Expr *E) { varlist_begin()[0] = E; }

void setIsTarget(bool V) { IsTarget = V; }

void setIsTargetSync(bool V) { IsTargetSync = V; }


void setVarLoc(SourceLocation Loc) { VarLoc = Loc; }










OMPInitClause(bool IsTarget, bool IsTargetSync, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation VarLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPInitClause>(llvm::omp::OMPC_init, StartLoc,
LParenLoc, EndLoc, N),
VarLoc(VarLoc), IsTarget(IsTarget), IsTargetSync(IsTargetSync) {}


OMPInitClause(unsigned N)
: OMPVarListClause<OMPInitClause>(llvm::omp::OMPC_init, SourceLocation(),
SourceLocation(), SourceLocation(), N) {
}

public:











static OMPInitClause *Create(const ASTContext &C, Expr *InteropVar,
ArrayRef<Expr *> PrefExprs, bool IsTarget,
bool IsTargetSync, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation VarLoc,
SourceLocation EndLoc);





static OMPInitClause *CreateEmpty(const ASTContext &C, unsigned N);


SourceLocation getVarLoc() const { return VarLoc; }


Expr *getInteropVar() { return varlist_begin()[0]; }
const Expr *getInteropVar() const { return varlist_begin()[0]; }


bool getIsTarget() const { return IsTarget; }


bool getIsTargetSync() const { return IsTargetSync; }

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPInitClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

using prefs_iterator = MutableArrayRef<Expr *>::iterator;
using const_prefs_iterator = ArrayRef<const Expr *>::iterator;
using prefs_range = llvm::iterator_range<prefs_iterator>;
using const_prefs_range = llvm::iterator_range<const_prefs_iterator>;

prefs_range prefs() {
return prefs_range(reinterpret_cast<Expr **>(std::next(varlist_begin())),
reinterpret_cast<Expr **>(varlist_end()));
}

const_prefs_range prefs() const {
auto Prefs = const_cast<OMPInitClause *>(this)->prefs();
return const_prefs_range(Prefs.begin(), Prefs.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_init;
}
};






class OMPUseClause final : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


SourceLocation VarLoc;


Stmt *InteropVar = nullptr;


void setInteropVar(Expr *E) { InteropVar = E; }


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


void setVarLoc(SourceLocation Loc) { VarLoc = Loc; }

public:







OMPUseClause(Expr *InteropVar, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation VarLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_use, StartLoc, EndLoc), LParenLoc(LParenLoc),
VarLoc(VarLoc), InteropVar(InteropVar) {}


OMPUseClause()
: OMPClause(llvm::omp::OMPC_use, SourceLocation(), SourceLocation()) {}


SourceLocation getLParenLoc() const { return LParenLoc; }


SourceLocation getVarLoc() const { return VarLoc; }


Expr *getInteropVar() const { return cast<Expr>(InteropVar); }

child_range children() { return child_range(&InteropVar, &InteropVar + 1); }

const_child_range children() const {
return const_child_range(&InteropVar, &InteropVar + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_use;
}
};










class OMPDestroyClause final : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


SourceLocation VarLoc;


Stmt *InteropVar = nullptr;


void setInteropVar(Expr *E) { InteropVar = E; }


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


void setVarLoc(SourceLocation Loc) { VarLoc = Loc; }

public:







OMPDestroyClause(Expr *InteropVar, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation VarLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_destroy, StartLoc, EndLoc),
LParenLoc(LParenLoc), VarLoc(VarLoc), InteropVar(InteropVar) {}





OMPDestroyClause(SourceLocation StartLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_destroy, StartLoc, EndLoc) {}


OMPDestroyClause()
: OMPClause(llvm::omp::OMPC_destroy, SourceLocation(), SourceLocation()) {
}


SourceLocation getLParenLoc() const { return LParenLoc; }


SourceLocation getVarLoc() const { return VarLoc; }


Expr *getInteropVar() const { return cast_or_null<Expr>(InteropVar); }

child_range children() {
if (InteropVar)
return child_range(&InteropVar, &InteropVar + 1);
return child_range(child_iterator(), child_iterator());
}

const_child_range children() const {
if (InteropVar)
return const_child_range(&InteropVar, &InteropVar + 1);
return const_child_range(const_child_iterator(), const_child_iterator());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_destroy;
}
};








class OMPNovariantsClause final : public OMPClause,
public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Condition = nullptr;


void setCondition(Expr *Cond) { Condition = Cond; }


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }

public:









OMPNovariantsClause(Expr *Cond, Stmt *HelperCond,
OpenMPDirectiveKind CaptureRegion,
SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_novariants, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), Condition(Cond) {
setPreInitStmt(HelperCond, CaptureRegion);
}


OMPNovariantsClause()
: OMPClause(llvm::omp::OMPC_novariants, SourceLocation(),
SourceLocation()),
OMPClauseWithPreInit(this) {}


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getCondition() const { return cast_or_null<Expr>(Condition); }

child_range children() { return child_range(&Condition, &Condition + 1); }

const_child_range children() const {
return const_child_range(&Condition, &Condition + 1);
}

child_range used_children();
const_child_range used_children() const {
auto Children = const_cast<OMPNovariantsClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_novariants;
}
};








class OMPNocontextClause final : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Condition = nullptr;


void setCondition(Expr *Cond) { Condition = Cond; }


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }

public:









OMPNocontextClause(Expr *Cond, Stmt *HelperCond,
OpenMPDirectiveKind CaptureRegion, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_nocontext, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), Condition(Cond) {
setPreInitStmt(HelperCond, CaptureRegion);
}


OMPNocontextClause()
: OMPClause(llvm::omp::OMPC_nocontext, SourceLocation(),
SourceLocation()),
OMPClauseWithPreInit(this) {}


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getCondition() const { return cast_or_null<Expr>(Condition); }

child_range children() { return child_range(&Condition, &Condition + 1); }

const_child_range children() const {
return const_child_range(&Condition, &Condition + 1);
}

child_range used_children();
const_child_range used_children() const {
auto Children = const_cast<OMPNocontextClause *>(this)->used_children();
return const_child_range(Children.begin(), Children.end());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_nocontext;
}
};








class OMPDetachClause final : public OMPClause {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *Evt = nullptr;


void setEventHandler(Expr *E) { Evt = E; }


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }

public:






OMPDetachClause(Expr *Evt, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_detach, StartLoc, EndLoc),
LParenLoc(LParenLoc), Evt(Evt) {}


OMPDetachClause()
: OMPClause(llvm::omp::OMPC_detach, SourceLocation(), SourceLocation()) {}


SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getEventHandler() const { return cast_or_null<Expr>(Evt); }

child_range children() { return child_range(&Evt, &Evt + 1); }

const_child_range children() const {
return const_child_range(&Evt, &Evt + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_detach;
}
};








class OMPInclusiveClause final
: public OMPVarListClause<OMPInclusiveClause>,
private llvm::TrailingObjects<OMPInclusiveClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;







OMPInclusiveClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPInclusiveClause>(llvm::omp::OMPC_inclusive,
StartLoc, LParenLoc, EndLoc, N) {}




explicit OMPInclusiveClause(unsigned N)
: OMPVarListClause<OMPInclusiveClause>(llvm::omp::OMPC_inclusive,
SourceLocation(), SourceLocation(),
SourceLocation(), N) {}

public:







static OMPInclusiveClause *Create(const ASTContext &C,
SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL);





static OMPInclusiveClause *CreateEmpty(const ASTContext &C, unsigned N);

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPInclusiveClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_inclusive;
}
};








class OMPExclusiveClause final
: public OMPVarListClause<OMPExclusiveClause>,
private llvm::TrailingObjects<OMPExclusiveClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;







OMPExclusiveClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPExclusiveClause>(llvm::omp::OMPC_exclusive,
StartLoc, LParenLoc, EndLoc, N) {}




explicit OMPExclusiveClause(unsigned N)
: OMPVarListClause<OMPExclusiveClause>(llvm::omp::OMPC_exclusive,
SourceLocation(), SourceLocation(),
SourceLocation(), N) {}

public:







static OMPExclusiveClause *Create(const ASTContext &C,
SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation EndLoc, ArrayRef<Expr *> VL);





static OMPExclusiveClause *CreateEmpty(const ASTContext &C, unsigned N);

child_range children() {
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end()));
}

const_child_range children() const {
auto Children = const_cast<OMPExclusiveClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_exclusive;
}
};









class OMPUsesAllocatorsClause final
: public OMPClause,
private llvm::TrailingObjects<OMPUsesAllocatorsClause, Expr *,
SourceLocation> {
public:

struct Data {

Expr *Allocator = nullptr;

Expr *AllocatorTraits = nullptr;

SourceLocation LParenLoc, RParenLoc;
};

private:
friend class OMPClauseReader;
friend TrailingObjects;

enum class ExprOffsets {
Allocator,
AllocatorTraits,
Total,
};

enum class ParenLocsOffsets {
LParen,
RParen,
Total,
};


SourceLocation LParenLoc;

unsigned NumOfAllocators = 0;







OMPUsesAllocatorsClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, unsigned N)
: OMPClause(llvm::omp::OMPC_uses_allocators, StartLoc, EndLoc),
LParenLoc(LParenLoc), NumOfAllocators(N) {}




explicit OMPUsesAllocatorsClause(unsigned N)
: OMPClause(llvm::omp::OMPC_uses_allocators, SourceLocation(),
SourceLocation()),
NumOfAllocators(N) {}

unsigned numTrailingObjects(OverloadToken<Expr *>) const {
return NumOfAllocators * static_cast<int>(ExprOffsets::Total);
}


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }


void setAllocatorsData(ArrayRef<OMPUsesAllocatorsClause::Data> Data);

public:







static OMPUsesAllocatorsClause *
Create(const ASTContext &C, SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation EndLoc, ArrayRef<OMPUsesAllocatorsClause::Data> Data);





static OMPUsesAllocatorsClause *CreateEmpty(const ASTContext &C, unsigned N);


SourceLocation getLParenLoc() const { return LParenLoc; }


unsigned getNumberOfAllocators() const { return NumOfAllocators; }


OMPUsesAllocatorsClause::Data getAllocatorData(unsigned I) const;


child_range children() {
Stmt **Begin = reinterpret_cast<Stmt **>(getTrailingObjects<Expr *>());
return child_range(Begin, Begin + NumOfAllocators *
static_cast<int>(ExprOffsets::Total));
}
const_child_range children() const {
Stmt *const *Begin =
reinterpret_cast<Stmt *const *>(getTrailingObjects<Expr *>());
return const_child_range(
Begin, Begin + NumOfAllocators * static_cast<int>(ExprOffsets::Total));
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_uses_allocators;
}
};










class OMPAffinityClause final
: public OMPVarListClause<OMPAffinityClause>,
private llvm::TrailingObjects<OMPAffinityClause, Expr *> {
friend class OMPClauseReader;
friend OMPVarListClause;
friend TrailingObjects;


SourceLocation ColonLoc;








OMPAffinityClause(SourceLocation StartLoc, SourceLocation LParenLoc,
SourceLocation ColonLoc, SourceLocation EndLoc, unsigned N)
: OMPVarListClause<OMPAffinityClause>(llvm::omp::OMPC_affinity, StartLoc,
LParenLoc, EndLoc, N) {}




explicit OMPAffinityClause(unsigned N)
: OMPVarListClause<OMPAffinityClause>(llvm::omp::OMPC_affinity,
SourceLocation(), SourceLocation(),
SourceLocation(), N) {}


void setModifier(Expr *E) {
getTrailingObjects<Expr *>()[varlist_size()] = E;
}


void setColonLoc(SourceLocation Loc) { ColonLoc = Loc; }

public:








static OMPAffinityClause *Create(const ASTContext &C, SourceLocation StartLoc,
SourceLocation LParenLoc,
SourceLocation ColonLoc,
SourceLocation EndLoc, Expr *Modifier,
ArrayRef<Expr *> Locators);





static OMPAffinityClause *CreateEmpty(const ASTContext &C, unsigned N);


Expr *getModifier() { return getTrailingObjects<Expr *>()[varlist_size()]; }
Expr *getModifier() const {
return getTrailingObjects<Expr *>()[varlist_size()];
}


SourceLocation getColonLoc() const { return ColonLoc; }


child_range children() {
int Offset = getModifier() ? 1 : 0;
return child_range(reinterpret_cast<Stmt **>(varlist_begin()),
reinterpret_cast<Stmt **>(varlist_end() + Offset));
}

const_child_range children() const {
auto Children = const_cast<OMPAffinityClause *>(this)->children();
return const_child_range(Children.begin(), Children.end());
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_affinity;
}
};








class OMPFilterClause final : public OMPClause, public OMPClauseWithPreInit {
friend class OMPClauseReader;


SourceLocation LParenLoc;


Stmt *ThreadID = nullptr;


void setThreadID(Expr *TID) { ThreadID = TID; }


void setLParenLoc(SourceLocation Loc) { LParenLoc = Loc; }

public:









OMPFilterClause(Expr *ThreadID, Stmt *HelperE,
OpenMPDirectiveKind CaptureRegion, SourceLocation StartLoc,
SourceLocation LParenLoc, SourceLocation EndLoc)
: OMPClause(llvm::omp::OMPC_filter, StartLoc, EndLoc),
OMPClauseWithPreInit(this), LParenLoc(LParenLoc), ThreadID(ThreadID) {
setPreInitStmt(HelperE, CaptureRegion);
}


OMPFilterClause()
: OMPClause(llvm::omp::OMPC_filter, SourceLocation(), SourceLocation()),
OMPClauseWithPreInit(this) {}

SourceLocation getLParenLoc() const { return LParenLoc; }


Expr *getThreadID() { return cast<Expr>(ThreadID); }


Expr *getThreadID() const { return cast<Expr>(ThreadID); }

child_range children() { return child_range(&ThreadID, &ThreadID + 1); }

const_child_range children() const {
return const_child_range(&ThreadID, &ThreadID + 1);
}

child_range used_children() {
return child_range(child_iterator(), child_iterator());
}
const_child_range used_children() const {
return const_child_range(const_child_iterator(), const_child_iterator());
}

static bool classof(const OMPClause *T) {
return T->getClauseKind() == llvm::omp::OMPC_filter;
}
};



template<class ImplClass, template <typename> class Ptr, typename RetTy>
class OMPClauseVisitorBase {
public:
#define PTR(CLASS) Ptr<CLASS>
#define DISPATCH(CLASS) return static_cast<ImplClass*>(this)->Visit##CLASS(static_cast<PTR(CLASS)>(S))


#define GEN_CLANG_CLAUSE_CLASS
#define CLAUSE_CLASS(Enum, Str, Class) RetTy Visit##Class(PTR(Class) S) { DISPATCH(Class); }

#include "llvm/Frontend/OpenMP/OMP.inc"

RetTy Visit(PTR(OMPClause) S) {

switch (S->getClauseKind()) {
#define GEN_CLANG_CLAUSE_CLASS
#define CLAUSE_CLASS(Enum, Str, Class) case llvm::omp::Clause::Enum: return Visit##Class(static_cast<PTR(Class)>(S));


#define CLAUSE_NO_CLASS(Enum, Str) case llvm::omp::Clause::Enum: break;


#include "llvm/Frontend/OpenMP/OMP.inc"
}
}

RetTy VisitOMPClause(PTR(OMPClause) Node) { return RetTy(); }
#undef PTR
#undef DISPATCH
};

template <typename T> using const_ptr = std::add_pointer_t<std::add_const_t<T>>;

template <class ImplClass, typename RetTy = void>
class OMPClauseVisitor
: public OMPClauseVisitorBase<ImplClass, std::add_pointer_t, RetTy> {};
template<class ImplClass, typename RetTy = void>
class ConstOMPClauseVisitor :
public OMPClauseVisitorBase <ImplClass, const_ptr, RetTy> {};

class OMPClausePrinter final : public OMPClauseVisitor<OMPClausePrinter> {
raw_ostream &OS;
const PrintingPolicy &Policy;


template <typename T> void VisitOMPClauseList(T *Node, char StartSym);

template <typename T> void VisitOMPMotionClause(T *Node);

public:
OMPClausePrinter(raw_ostream &OS, const PrintingPolicy &Policy)
: OS(OS), Policy(Policy) {}

#define GEN_CLANG_CLAUSE_CLASS
#define CLAUSE_CLASS(Enum, Str, Class) void Visit##Class(Class *S);
#include "llvm/Frontend/OpenMP/OMP.inc"
};

struct OMPTraitProperty {
llvm::omp::TraitProperty Kind = llvm::omp::TraitProperty::invalid;



StringRef RawString;
};
struct OMPTraitSelector {
Expr *ScoreOrCondition = nullptr;
llvm::omp::TraitSelector Kind = llvm::omp::TraitSelector::invalid;
llvm::SmallVector<OMPTraitProperty, 1> Properties;
};
struct OMPTraitSet {
llvm::omp::TraitSet Kind = llvm::omp::TraitSet::invalid;
llvm::SmallVector<OMPTraitSelector, 2> Selectors;
};






class OMPTraitInfo {

OMPTraitInfo() {}
friend class ASTContext;

public:

OMPTraitInfo(StringRef MangledName);


llvm::SmallVector<OMPTraitSet, 2> Sets;

bool anyScoreOrCondition(
llvm::function_ref<bool(Expr *&, bool )> Cond) {
return llvm::any_of(Sets, [&](OMPTraitSet &Set) {
return llvm::any_of(
Set.Selectors, [&](OMPTraitSelector &Selector) {
return Cond(Selector.ScoreOrCondition,
Selector.Kind !=
llvm::omp::TraitSelector::user_condition);
});
});
}






void getAsVariantMatchInfo(ASTContext &ASTCtx,
llvm::omp::VariantMatchInfo &VMI) const;


std::string getMangledName() const;


bool isExtensionActive(llvm::omp::TraitProperty TP) {
for (const OMPTraitSet &Set : Sets) {
if (Set.Kind != llvm::omp::TraitSet::implementation)
continue;
for (const OMPTraitSelector &Selector : Set.Selectors) {
if (Selector.Kind != llvm::omp::TraitSelector::implementation_extension)
continue;
for (const OMPTraitProperty &Property : Selector.Properties) {
if (Property.Kind == TP)
return true;
}
}
}
return false;
}


void print(llvm::raw_ostream &OS, const PrintingPolicy &Policy) const;
};
llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const OMPTraitInfo &TI);
llvm::raw_ostream &operator<<(llvm::raw_ostream &OS, const OMPTraitInfo *TI);


struct TargetOMPContext final : public llvm::omp::OMPContext {

TargetOMPContext(ASTContext &ASTCtx,
std::function<void(StringRef)> &&DiagUnknownTrait,
const FunctionDecl *CurrentFunctionDecl);
virtual ~TargetOMPContext() = default;


bool matchesISATrait(StringRef RawString) const override;

private:
std::function<bool(StringRef)> FeatureValidityCheck;
std::function<void(StringRef)> DiagUnknownTrait;
llvm::StringMap<bool> FeatureMap;
};




class OMPChildren final
: private llvm::TrailingObjects<OMPChildren, OMPClause *, Stmt *> {
friend TrailingObjects;
friend class OMPClauseReader;
friend class OMPExecutableDirective;
template <typename T> friend class OMPDeclarativeDirective;


unsigned NumClauses = 0;

unsigned NumChildren = 0;

bool HasAssociatedStmt = false;



size_t numTrailingObjects(OverloadToken<OMPClause *>) const {
return NumClauses;
}

OMPChildren() = delete;

OMPChildren(unsigned NumClauses, unsigned NumChildren, bool HasAssociatedStmt)
: NumClauses(NumClauses), NumChildren(NumChildren),
HasAssociatedStmt(HasAssociatedStmt) {}

static size_t size(unsigned NumClauses, bool HasAssociatedStmt,
unsigned NumChildren);

static OMPChildren *Create(void *Mem, ArrayRef<OMPClause *> Clauses);
static OMPChildren *Create(void *Mem, ArrayRef<OMPClause *> Clauses, Stmt *S,
unsigned NumChildren = 0);
static OMPChildren *CreateEmpty(void *Mem, unsigned NumClauses,
bool HasAssociatedStmt = false,
unsigned NumChildren = 0);

public:
unsigned getNumClauses() const { return NumClauses; }
unsigned getNumChildren() const { return NumChildren; }
bool hasAssociatedStmt() const { return HasAssociatedStmt; }


void setAssociatedStmt(Stmt *S) {
getTrailingObjects<Stmt *>()[NumChildren] = S;
}

void setChildren(ArrayRef<Stmt *> Children);





void setClauses(ArrayRef<OMPClause *> Clauses);


const Stmt *getAssociatedStmt() const {
return const_cast<OMPChildren *>(this)->getAssociatedStmt();
}
Stmt *getAssociatedStmt() {
assert(HasAssociatedStmt &&
"Expected directive with the associated statement.");
return getTrailingObjects<Stmt *>()[NumChildren];
}


MutableArrayRef<OMPClause *> getClauses() {
return llvm::makeMutableArrayRef(getTrailingObjects<OMPClause *>(),
NumClauses);
}
ArrayRef<OMPClause *> getClauses() const {
return const_cast<OMPChildren *>(this)->getClauses();
}





const CapturedStmt *
getCapturedStmt(OpenMPDirectiveKind RegionKind,
ArrayRef<OpenMPDirectiveKind> CaptureRegions) const {
assert(llvm::any_of(
CaptureRegions,
[=](const OpenMPDirectiveKind K) { return K == RegionKind; }) &&
"RegionKind not found in OpenMP CaptureRegions.");
auto *CS = cast<CapturedStmt>(getAssociatedStmt());
for (auto ThisCaptureRegion : CaptureRegions) {
if (ThisCaptureRegion == RegionKind)
return CS;
CS = cast<CapturedStmt>(CS->getCapturedStmt());
}
llvm_unreachable("Incorrect RegionKind specified for directive.");
}


CapturedStmt *
getInnermostCapturedStmt(ArrayRef<OpenMPDirectiveKind> CaptureRegions) {
assert(hasAssociatedStmt() && "Must have associated captured statement.");
assert(!CaptureRegions.empty() &&
"At least one captured statement must be provided.");
auto *CS = cast<CapturedStmt>(getAssociatedStmt());
for (unsigned Level = CaptureRegions.size(); Level > 1; --Level)
CS = cast<CapturedStmt>(CS->getCapturedStmt());
return CS;
}

const CapturedStmt *
getInnermostCapturedStmt(ArrayRef<OpenMPDirectiveKind> CaptureRegions) const {
return const_cast<OMPChildren *>(this)->getInnermostCapturedStmt(
CaptureRegions);
}

MutableArrayRef<Stmt *> getChildren();
ArrayRef<Stmt *> getChildren() const {
return const_cast<OMPChildren *>(this)->getChildren();
}

Stmt *getRawStmt() {
assert(HasAssociatedStmt &&
"Expected directive with the associated statement.");
if (auto *CS = dyn_cast<CapturedStmt>(getAssociatedStmt())) {
Stmt *S = nullptr;
do {
S = CS->getCapturedStmt();
CS = dyn_cast<CapturedStmt>(S);
} while (CS);
return S;
}
return getAssociatedStmt();
}
const Stmt *getRawStmt() const {
return const_cast<OMPChildren *>(this)->getRawStmt();
}

Stmt::child_range getAssociatedStmtAsRange() {
if (!HasAssociatedStmt)
return Stmt::child_range(Stmt::child_iterator(), Stmt::child_iterator());
return Stmt::child_range(&getTrailingObjects<Stmt *>()[NumChildren],
&getTrailingObjects<Stmt *>()[NumChildren + 1]);
}
};

}

#endif
