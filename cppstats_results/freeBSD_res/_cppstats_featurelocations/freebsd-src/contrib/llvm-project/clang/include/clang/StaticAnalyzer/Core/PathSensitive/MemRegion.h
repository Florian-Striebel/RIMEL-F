













#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_MEMREGION_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_MEMREGION_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/CharUnits.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprObjC.h"
#include "clang/AST/Type.h"
#include "clang/Analysis/AnalysisDeclContext.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SymExpr.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Casting.h"
#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>

namespace clang {

class AnalysisDeclContext;
class CXXRecordDecl;
class Decl;
class LocationContext;
class StackFrameContext;

namespace ento {

class CodeTextRegion;
class MemRegion;
class MemRegionManager;
class MemSpaceRegion;
class SValBuilder;
class SymbolicRegion;
class VarRegion;


class RegionOffset {

const MemRegion *R = nullptr;


int64_t Offset;

public:


static const int64_t Symbolic = std::numeric_limits<int64_t>::max();

RegionOffset() = default;
RegionOffset(const MemRegion *r, int64_t off) : R(r), Offset(off) {}

const MemRegion *getRegion() const { return R; }

bool hasSymbolicOffset() const { return Offset == Symbolic; }

int64_t getOffset() const {
assert(!hasSymbolicOffset());
return Offset;
}

bool isValid() const { return R; }
};






class MemRegion : public llvm::FoldingSetNode {
public:
enum Kind {
#define REGION(Id, Parent) Id ##Kind,
#define REGION_RANGE(Id, First, Last) BEGIN_##Id = First, END_##Id = Last,
#include "clang/StaticAnalyzer/Core/PathSensitive/Regions.def"
};

private:
const Kind kind;
mutable Optional<RegionOffset> cachedOffset;

protected:
MemRegion(Kind k) : kind(k) {}
virtual ~MemRegion();

public:
ASTContext &getContext() const;

virtual void Profile(llvm::FoldingSetNodeID& ID) const = 0;

virtual MemRegionManager &getMemRegionManager() const = 0;

const MemSpaceRegion *getMemorySpace() const;

const MemRegion *getBaseRegion() const;



const MemRegion *getMostDerivedObjectRegion() const;



virtual bool isSubRegionOf(const MemRegion *R) const;

const MemRegion *StripCasts(bool StripBaseAndDerivedCasts = true) const;



const SymbolicRegion *getSymbolicBase() const;

bool hasGlobalsOrParametersStorage() const;

bool hasStackStorage() const;

bool hasStackNonParametersStorage() const;

bool hasStackParametersStorage() const;


RegionOffset getAsOffset() const;


std::string getString() const;

virtual void dumpToStream(raw_ostream &os) const;

void dump() const;


virtual bool canPrintPretty() const;


virtual void printPretty(raw_ostream &os) const;



virtual bool canPrintPrettyAsExpr() const;





virtual void printPrettyAsExpr(raw_ostream &os) const;

Kind getKind() const { return kind; }

template<typename RegionTy> const RegionTy* getAs() const;
template<typename RegionTy> const RegionTy* castAs() const;

virtual bool isBoundable() const { return false; }









std::string getDescriptiveName(bool UseQuotes = true) const;









SourceRange sourceRange() const;
};



class MemSpaceRegion : public MemRegion {
protected:
MemRegionManager &Mgr;

MemSpaceRegion(MemRegionManager &mgr, Kind k) : MemRegion(k), Mgr(mgr) {
assert(classof(this));
}

MemRegionManager &getMemRegionManager() const override { return Mgr; }

public:
bool isBoundable() const override { return false; }

void Profile(llvm::FoldingSetNodeID &ID) const override;

static bool classof(const MemRegion *R) {
Kind k = R->getKind();
return k >= BEGIN_MEMSPACES && k <= END_MEMSPACES;
}
};



class CodeSpaceRegion : public MemSpaceRegion {
friend class MemRegionManager;

CodeSpaceRegion(MemRegionManager &mgr)
: MemSpaceRegion(mgr, CodeSpaceRegionKind) {}

public:
void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion *R) {
return R->getKind() == CodeSpaceRegionKind;
}
};

class GlobalsSpaceRegion : public MemSpaceRegion {
virtual void anchor();

protected:
GlobalsSpaceRegion(MemRegionManager &mgr, Kind k) : MemSpaceRegion(mgr, k) {
assert(classof(this));
}

public:
static bool classof(const MemRegion *R) {
Kind k = R->getKind();
return k >= BEGIN_GLOBAL_MEMSPACES && k <= END_GLOBAL_MEMSPACES;
}
};






class StaticGlobalSpaceRegion : public GlobalsSpaceRegion {
friend class MemRegionManager;

const CodeTextRegion *CR;

StaticGlobalSpaceRegion(MemRegionManager &mgr, const CodeTextRegion *cr)
: GlobalsSpaceRegion(mgr, StaticGlobalSpaceRegionKind), CR(cr) {
assert(cr);
}

public:
void Profile(llvm::FoldingSetNodeID &ID) const override;

void dumpToStream(raw_ostream &os) const override;

const CodeTextRegion *getCodeRegion() const { return CR; }

static bool classof(const MemRegion *R) {
return R->getKind() == StaticGlobalSpaceRegionKind;
}
};







class NonStaticGlobalSpaceRegion : public GlobalsSpaceRegion {
void anchor() override;

protected:
NonStaticGlobalSpaceRegion(MemRegionManager &mgr, Kind k)
: GlobalsSpaceRegion(mgr, k) {
assert(classof(this));
}

public:
static bool classof(const MemRegion *R) {
Kind k = R->getKind();
return k >= BEGIN_NON_STATIC_GLOBAL_MEMSPACES &&
k <= END_NON_STATIC_GLOBAL_MEMSPACES;
}
};



class GlobalSystemSpaceRegion : public NonStaticGlobalSpaceRegion {
friend class MemRegionManager;

GlobalSystemSpaceRegion(MemRegionManager &mgr)
: NonStaticGlobalSpaceRegion(mgr, GlobalSystemSpaceRegionKind) {}

public:
void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion *R) {
return R->getKind() == GlobalSystemSpaceRegionKind;
}
};






class GlobalImmutableSpaceRegion : public NonStaticGlobalSpaceRegion {
friend class MemRegionManager;

GlobalImmutableSpaceRegion(MemRegionManager &mgr)
: NonStaticGlobalSpaceRegion(mgr, GlobalImmutableSpaceRegionKind) {}

public:
void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion *R) {
return R->getKind() == GlobalImmutableSpaceRegionKind;
}
};




class GlobalInternalSpaceRegion : public NonStaticGlobalSpaceRegion {
friend class MemRegionManager;

GlobalInternalSpaceRegion(MemRegionManager &mgr)
: NonStaticGlobalSpaceRegion(mgr, GlobalInternalSpaceRegionKind) {}

public:
void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion *R) {
return R->getKind() == GlobalInternalSpaceRegionKind;
}
};

class HeapSpaceRegion : public MemSpaceRegion {
friend class MemRegionManager;

HeapSpaceRegion(MemRegionManager &mgr)
: MemSpaceRegion(mgr, HeapSpaceRegionKind) {}

public:
void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion *R) {
return R->getKind() == HeapSpaceRegionKind;
}
};

class UnknownSpaceRegion : public MemSpaceRegion {
friend class MemRegionManager;

UnknownSpaceRegion(MemRegionManager &mgr)
: MemSpaceRegion(mgr, UnknownSpaceRegionKind) {}

public:
void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion *R) {
return R->getKind() == UnknownSpaceRegionKind;
}
};

class StackSpaceRegion : public MemSpaceRegion {
virtual void anchor();

const StackFrameContext *SFC;

protected:
StackSpaceRegion(MemRegionManager &mgr, Kind k, const StackFrameContext *sfc)
: MemSpaceRegion(mgr, k), SFC(sfc) {
assert(classof(this));
assert(sfc);
}

public:
const StackFrameContext *getStackFrame() const { return SFC; }

void Profile(llvm::FoldingSetNodeID &ID) const override;

static bool classof(const MemRegion *R) {
Kind k = R->getKind();
return k >= BEGIN_STACK_MEMSPACES && k <= END_STACK_MEMSPACES;
}
};

class StackLocalsSpaceRegion : public StackSpaceRegion {
friend class MemRegionManager;

StackLocalsSpaceRegion(MemRegionManager &mgr, const StackFrameContext *sfc)
: StackSpaceRegion(mgr, StackLocalsSpaceRegionKind, sfc) {}

public:
void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion *R) {
return R->getKind() == StackLocalsSpaceRegionKind;
}
};

class StackArgumentsSpaceRegion : public StackSpaceRegion {
private:
friend class MemRegionManager;

StackArgumentsSpaceRegion(MemRegionManager &mgr, const StackFrameContext *sfc)
: StackSpaceRegion(mgr, StackArgumentsSpaceRegionKind, sfc) {}

public:
void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion *R) {
return R->getKind() == StackArgumentsSpaceRegionKind;
}
};



class SubRegion : public MemRegion {
virtual void anchor();

protected:
const MemRegion* superRegion;

SubRegion(const MemRegion *sReg, Kind k) : MemRegion(k), superRegion(sReg) {
assert(classof(this));
assert(sReg);
}

public:
const MemRegion* getSuperRegion() const {
return superRegion;
}

MemRegionManager &getMemRegionManager() const override;

bool isSubRegionOf(const MemRegion* R) const override;

static bool classof(const MemRegion* R) {
return R->getKind() > END_MEMSPACES;
}
};







class AllocaRegion : public SubRegion {
friend class MemRegionManager;



unsigned Cnt;

const Expr *Ex;

AllocaRegion(const Expr *ex, unsigned cnt, const MemSpaceRegion *superRegion)
: SubRegion(superRegion, AllocaRegionKind), Cnt(cnt), Ex(ex) {
assert(Ex);
}

static void ProfileRegion(llvm::FoldingSetNodeID& ID, const Expr *Ex,
unsigned Cnt, const MemRegion *superRegion);

public:
const Expr *getExpr() const { return Ex; }

bool isBoundable() const override { return true; }

void Profile(llvm::FoldingSetNodeID& ID) const override;

void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == AllocaRegionKind;
}
};


class TypedRegion : public SubRegion {
void anchor() override;

protected:
TypedRegion(const MemRegion *sReg, Kind k) : SubRegion(sReg, k) {
assert(classof(this));
}

public:
virtual QualType getLocationType() const = 0;

QualType getDesugaredLocationType(ASTContext &Context) const {
return getLocationType().getDesugaredType(Context);
}

bool isBoundable() const override { return true; }

static bool classof(const MemRegion* R) {
unsigned k = R->getKind();
return k >= BEGIN_TYPED_REGIONS && k <= END_TYPED_REGIONS;
}
};


class TypedValueRegion : public TypedRegion {
void anchor() override;

protected:
TypedValueRegion(const MemRegion* sReg, Kind k) : TypedRegion(sReg, k) {
assert(classof(this));
}

public:
virtual QualType getValueType() const = 0;

QualType getLocationType() const override {

QualType T = getValueType();
ASTContext &ctx = getContext();
if (T->getAs<ObjCObjectType>())
return ctx.getObjCObjectPointerType(T);
return ctx.getPointerType(getValueType());
}

QualType getDesugaredValueType(ASTContext &Context) const {
QualType T = getValueType();
return T.getTypePtrOrNull() ? T.getDesugaredType(Context) : T;
}

static bool classof(const MemRegion* R) {
unsigned k = R->getKind();
return k >= BEGIN_TYPED_VALUE_REGIONS && k <= END_TYPED_VALUE_REGIONS;
}
};

class CodeTextRegion : public TypedRegion {
void anchor() override;

protected:
CodeTextRegion(const MemSpaceRegion *sreg, Kind k) : TypedRegion(sreg, k) {
assert(classof(this));
}

public:
bool isBoundable() const override { return false; }

static bool classof(const MemRegion* R) {
Kind k = R->getKind();
return k >= BEGIN_CODE_TEXT_REGIONS && k <= END_CODE_TEXT_REGIONS;
}
};


class FunctionCodeRegion : public CodeTextRegion {
friend class MemRegionManager;

const NamedDecl *FD;

FunctionCodeRegion(const NamedDecl *fd, const CodeSpaceRegion* sreg)
: CodeTextRegion(sreg, FunctionCodeRegionKind), FD(fd) {
assert(isa<ObjCMethodDecl>(fd) || isa<FunctionDecl>(fd));
}

static void ProfileRegion(llvm::FoldingSetNodeID& ID, const NamedDecl *FD,
const MemRegion*);

public:
QualType getLocationType() const override {
const ASTContext &Ctx = getContext();
if (const auto *D = dyn_cast<FunctionDecl>(FD)) {
return Ctx.getPointerType(D->getType());
}

assert(isa<ObjCMethodDecl>(FD));
assert(false && "Getting the type of ObjCMethod is not supported yet");



return {};
}

const NamedDecl *getDecl() const {
return FD;
}

void dumpToStream(raw_ostream &os) const override;

void Profile(llvm::FoldingSetNodeID& ID) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == FunctionCodeRegionKind;
}
};







class BlockCodeRegion : public CodeTextRegion {
friend class MemRegionManager;

const BlockDecl *BD;
AnalysisDeclContext *AC;
CanQualType locTy;

BlockCodeRegion(const BlockDecl *bd, CanQualType lTy,
AnalysisDeclContext *ac, const CodeSpaceRegion* sreg)
: CodeTextRegion(sreg, BlockCodeRegionKind), BD(bd), AC(ac), locTy(lTy) {
assert(bd);
assert(ac);
assert(lTy->getTypePtr()->isBlockPointerType());
}

static void ProfileRegion(llvm::FoldingSetNodeID& ID, const BlockDecl *BD,
CanQualType, const AnalysisDeclContext*,
const MemRegion*);

public:
QualType getLocationType() const override {
return locTy;
}

const BlockDecl *getDecl() const {
return BD;
}

AnalysisDeclContext *getAnalysisDeclContext() const { return AC; }

void dumpToStream(raw_ostream &os) const override;

void Profile(llvm::FoldingSetNodeID& ID) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == BlockCodeRegionKind;
}
};







class BlockDataRegion : public TypedRegion {
friend class MemRegionManager;

const BlockCodeRegion *BC;
const LocationContext *LC;
unsigned BlockCount;
void *ReferencedVars = nullptr;
void *OriginalVars = nullptr;

BlockDataRegion(const BlockCodeRegion *bc, const LocationContext *lc,
unsigned count, const MemSpaceRegion *sreg)
: TypedRegion(sreg, BlockDataRegionKind), BC(bc), LC(lc),
BlockCount(count) {
assert(bc);
assert(lc);
assert(isa<GlobalImmutableSpaceRegion>(sreg) ||
isa<StackLocalsSpaceRegion>(sreg) ||
isa<UnknownSpaceRegion>(sreg));
}

static void ProfileRegion(llvm::FoldingSetNodeID&, const BlockCodeRegion *,
const LocationContext *, unsigned,
const MemRegion *);

public:
const BlockCodeRegion *getCodeRegion() const { return BC; }

const BlockDecl *getDecl() const { return BC->getDecl(); }

QualType getLocationType() const override { return BC->getLocationType(); }

class referenced_vars_iterator {
const MemRegion * const *R;
const MemRegion * const *OriginalR;

public:
explicit referenced_vars_iterator(const MemRegion * const *r,
const MemRegion * const *originalR)
: R(r), OriginalR(originalR) {}

const VarRegion *getCapturedRegion() const {
return cast<VarRegion>(*R);
}

const VarRegion *getOriginalRegion() const {
return cast<VarRegion>(*OriginalR);
}

bool operator==(const referenced_vars_iterator &I) const {
assert((R == nullptr) == (I.R == nullptr));
return I.R == R;
}

bool operator!=(const referenced_vars_iterator &I) const {
assert((R == nullptr) == (I.R == nullptr));
return I.R != R;
}

referenced_vars_iterator &operator++() {
++R;
++OriginalR;
return *this;
}
};



const VarRegion *getOriginalRegion(const VarRegion *VR) const;

referenced_vars_iterator referenced_vars_begin() const;
referenced_vars_iterator referenced_vars_end() const;

void dumpToStream(raw_ostream &os) const override;

void Profile(llvm::FoldingSetNodeID& ID) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == BlockDataRegionKind;
}

private:
void LazyInitializeReferencedVars();
std::pair<const VarRegion *, const VarRegion *>
getCaptureRegions(const VarDecl *VD);
};






class SymbolicRegion : public SubRegion {
friend class MemRegionManager;

const SymbolRef sym;

SymbolicRegion(const SymbolRef s, const MemSpaceRegion *sreg)
: SubRegion(sreg, SymbolicRegionKind), sym(s) {


assert(s && isa<SymbolData>(s));
assert(s->getType()->isAnyPointerType() ||
s->getType()->isReferenceType() ||
s->getType()->isBlockPointerType());
assert(isa<UnknownSpaceRegion>(sreg) || isa<HeapSpaceRegion>(sreg));
}

public:
SymbolRef getSymbol() const { return sym; }

bool isBoundable() const override { return true; }

void Profile(llvm::FoldingSetNodeID& ID) const override;

static void ProfileRegion(llvm::FoldingSetNodeID& ID,
SymbolRef sym,
const MemRegion* superRegion);

void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == SymbolicRegionKind;
}
};


class StringRegion : public TypedValueRegion {
friend class MemRegionManager;

const StringLiteral *Str;

StringRegion(const StringLiteral *str, const GlobalInternalSpaceRegion *sreg)
: TypedValueRegion(sreg, StringRegionKind), Str(str) {
assert(str);
}

static void ProfileRegion(llvm::FoldingSetNodeID &ID,
const StringLiteral *Str,
const MemRegion *superRegion);

public:
const StringLiteral *getStringLiteral() const { return Str; }

QualType getValueType() const override { return Str->getType(); }

bool isBoundable() const override { return false; }

void Profile(llvm::FoldingSetNodeID& ID) const override {
ProfileRegion(ID, Str, superRegion);
}

void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == StringRegionKind;
}
};


class ObjCStringRegion : public TypedValueRegion {
friend class MemRegionManager;

const ObjCStringLiteral *Str;

ObjCStringRegion(const ObjCStringLiteral *str,
const GlobalInternalSpaceRegion *sreg)
: TypedValueRegion(sreg, ObjCStringRegionKind), Str(str) {
assert(str);
}

static void ProfileRegion(llvm::FoldingSetNodeID &ID,
const ObjCStringLiteral *Str,
const MemRegion *superRegion);

public:
const ObjCStringLiteral *getObjCStringLiteral() const { return Str; }

QualType getValueType() const override { return Str->getType(); }

bool isBoundable() const override { return false; }

void Profile(llvm::FoldingSetNodeID& ID) const override {
ProfileRegion(ID, Str, superRegion);
}

void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == ObjCStringRegionKind;
}
};




class CompoundLiteralRegion : public TypedValueRegion {
friend class MemRegionManager;

const CompoundLiteralExpr *CL;

CompoundLiteralRegion(const CompoundLiteralExpr *cl,
const MemSpaceRegion *sReg)
: TypedValueRegion(sReg, CompoundLiteralRegionKind), CL(cl) {
assert(cl);
assert(isa<GlobalInternalSpaceRegion>(sReg) ||
isa<StackLocalsSpaceRegion>(sReg));
}

static void ProfileRegion(llvm::FoldingSetNodeID& ID,
const CompoundLiteralExpr *CL,
const MemRegion* superRegion);

public:
QualType getValueType() const override { return CL->getType(); }

bool isBoundable() const override { return !CL->isFileScope(); }

void Profile(llvm::FoldingSetNodeID& ID) const override;

void dumpToStream(raw_ostream &os) const override;

const CompoundLiteralExpr *getLiteralExpr() const { return CL; }

static bool classof(const MemRegion* R) {
return R->getKind() == CompoundLiteralRegionKind;
}
};

class DeclRegion : public TypedValueRegion {
protected:
DeclRegion(const MemRegion *sReg, Kind k) : TypedValueRegion(sReg, k) {
assert(classof(this));
}

public:
virtual const ValueDecl *getDecl() const = 0;

static bool classof(const MemRegion* R) {
unsigned k = R->getKind();
return k >= BEGIN_DECL_REGIONS && k <= END_DECL_REGIONS;
}
};

class VarRegion : public DeclRegion {
friend class MemRegionManager;

protected:

VarRegion(const MemRegion *sReg, Kind k) : DeclRegion(sReg, k) {




assert(isa<GlobalsSpaceRegion>(sReg) || isa<StackSpaceRegion>(sReg) ||
isa<BlockDataRegion>(sReg) || isa<UnknownSpaceRegion>(sReg));
}

public:
const VarDecl *getDecl() const override = 0;

const StackFrameContext *getStackFrame() const;

QualType getValueType() const override {

return getDecl()->getType();
}

static bool classof(const MemRegion *R) {
unsigned k = R->getKind();
return k >= BEGIN_VAR_REGIONS && k <= END_VAR_REGIONS;
}
};

class NonParamVarRegion : public VarRegion {
friend class MemRegionManager;

const VarDecl *VD;


NonParamVarRegion(const VarDecl *vd, const MemRegion *sReg)
: VarRegion(sReg, NonParamVarRegionKind), VD(vd) {




assert(isa<GlobalsSpaceRegion>(sReg) || isa<StackSpaceRegion>(sReg) ||
isa<BlockDataRegion>(sReg) || isa<UnknownSpaceRegion>(sReg));
}

static void ProfileRegion(llvm::FoldingSetNodeID &ID, const VarDecl *VD,
const MemRegion *superRegion);

public:
void Profile(llvm::FoldingSetNodeID &ID) const override;

const VarDecl *getDecl() const override { return VD; }

QualType getValueType() const override {

return getDecl()->getType();
}

void dumpToStream(raw_ostream &os) const override;

bool canPrintPrettyAsExpr() const override;

void printPrettyAsExpr(raw_ostream &os) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == NonParamVarRegionKind;
}
};











class ParamVarRegion : public VarRegion {
friend class MemRegionManager;

const Expr *OriginExpr;
unsigned Index;

ParamVarRegion(const Expr *OE, unsigned Idx, const MemRegion *SReg)
: VarRegion(SReg, ParamVarRegionKind), OriginExpr(OE), Index(Idx) {
assert(!cast<StackSpaceRegion>(SReg)->getStackFrame()->inTopFrame());
}

static void ProfileRegion(llvm::FoldingSetNodeID &ID, const Expr *OE,
unsigned Idx, const MemRegion *SReg);

public:
const Expr *getOriginExpr() const { return OriginExpr; }
unsigned getIndex() const { return Index; }

void Profile(llvm::FoldingSetNodeID& ID) const override;

void dumpToStream(raw_ostream &os) const override;

QualType getValueType() const override;
const ParmVarDecl *getDecl() const override;

bool canPrintPrettyAsExpr() const override;
void printPrettyAsExpr(raw_ostream &os) const override;

static bool classof(const MemRegion *R) {
return R->getKind() == ParamVarRegionKind;
}
};




class CXXThisRegion : public TypedValueRegion {
friend class MemRegionManager;

CXXThisRegion(const PointerType *thisPointerTy,
const StackArgumentsSpaceRegion *sReg)
: TypedValueRegion(sReg, CXXThisRegionKind),
ThisPointerTy(thisPointerTy) {
assert(ThisPointerTy->getPointeeType()->getAsCXXRecordDecl() &&
"Invalid region type!");
}

static void ProfileRegion(llvm::FoldingSetNodeID &ID,
const PointerType *PT,
const MemRegion *sReg);

public:
void Profile(llvm::FoldingSetNodeID &ID) const override;

QualType getValueType() const override {
return QualType(ThisPointerTy, 0);
}

void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == CXXThisRegionKind;
}

private:
const PointerType *ThisPointerTy;
};

class FieldRegion : public DeclRegion {
friend class MemRegionManager;

const FieldDecl *FD;

FieldRegion(const FieldDecl *fd, const SubRegion *sReg)
: DeclRegion(sReg, FieldRegionKind), FD(fd) {}

static void ProfileRegion(llvm::FoldingSetNodeID &ID, const FieldDecl *FD,
const MemRegion* superRegion) {
ID.AddInteger(static_cast<unsigned>(FieldRegionKind));
ID.AddPointer(FD);
ID.AddPointer(superRegion);
}

public:
const FieldDecl *getDecl() const override { return FD; }

void Profile(llvm::FoldingSetNodeID &ID) const override;

QualType getValueType() const override {

return getDecl()->getType();
}

void dumpToStream(raw_ostream &os) const override;

bool canPrintPretty() const override;
void printPretty(raw_ostream &os) const override;
bool canPrintPrettyAsExpr() const override;
void printPrettyAsExpr(raw_ostream &os) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == FieldRegionKind;
}
};

class ObjCIvarRegion : public DeclRegion {
friend class MemRegionManager;

const ObjCIvarDecl *IVD;

ObjCIvarRegion(const ObjCIvarDecl *ivd, const SubRegion *sReg);

static void ProfileRegion(llvm::FoldingSetNodeID& ID, const ObjCIvarDecl *ivd,
const MemRegion* superRegion);

public:
const ObjCIvarDecl *getDecl() const override;

void Profile(llvm::FoldingSetNodeID& ID) const override;

QualType getValueType() const override;

bool canPrintPrettyAsExpr() const override;
void printPrettyAsExpr(raw_ostream &os) const override;

void dumpToStream(raw_ostream &os) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == ObjCIvarRegionKind;
}
};





class RegionRawOffset {
friend class ElementRegion;

const MemRegion *Region;
CharUnits Offset;

RegionRawOffset(const MemRegion* reg, CharUnits offset = CharUnits::Zero())
: Region(reg), Offset(offset) {}

public:

CharUnits getOffset() const { return Offset; }
const MemRegion *getRegion() const { return Region; }

void dumpToStream(raw_ostream &os) const;
void dump() const;
};


class ElementRegion : public TypedValueRegion {
friend class MemRegionManager;

QualType ElementType;
NonLoc Index;

ElementRegion(QualType elementType, NonLoc Idx, const SubRegion *sReg)
: TypedValueRegion(sReg, ElementRegionKind), ElementType(elementType),
Index(Idx) {
assert((!Idx.getAs<nonloc::ConcreteInt>() ||
Idx.castAs<nonloc::ConcreteInt>().getValue().isSigned()) &&
"The index must be signed");
assert(!elementType.isNull() && !elementType->isVoidType() &&
"Invalid region type!");
}

static void ProfileRegion(llvm::FoldingSetNodeID& ID, QualType elementType,
SVal Idx, const MemRegion* superRegion);

public:
NonLoc getIndex() const { return Index; }

QualType getValueType() const override { return ElementType; }

QualType getElementType() const { return ElementType; }


RegionRawOffset getAsArrayOffset() const;

void dumpToStream(raw_ostream &os) const override;

void Profile(llvm::FoldingSetNodeID& ID) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == ElementRegionKind;
}
};


class CXXTempObjectRegion : public TypedValueRegion {
friend class MemRegionManager;

Expr const *Ex;

CXXTempObjectRegion(Expr const *E, MemSpaceRegion const *sReg)
: TypedValueRegion(sReg, CXXTempObjectRegionKind), Ex(E) {
assert(E);
assert(isa<StackLocalsSpaceRegion>(sReg) ||
isa<GlobalInternalSpaceRegion>(sReg));
}

static void ProfileRegion(llvm::FoldingSetNodeID &ID,
Expr const *E, const MemRegion *sReg);

public:
const Expr *getExpr() const { return Ex; }

QualType getValueType() const override { return Ex->getType(); }

void dumpToStream(raw_ostream &os) const override;

void Profile(llvm::FoldingSetNodeID &ID) const override;

static bool classof(const MemRegion* R) {
return R->getKind() == CXXTempObjectRegionKind;
}
};



class CXXBaseObjectRegion : public TypedValueRegion {
friend class MemRegionManager;

llvm::PointerIntPair<const CXXRecordDecl *, 1, bool> Data;

CXXBaseObjectRegion(const CXXRecordDecl *RD, bool IsVirtual,
const SubRegion *SReg)
: TypedValueRegion(SReg, CXXBaseObjectRegionKind), Data(RD, IsVirtual) {
assert(RD);
}

static void ProfileRegion(llvm::FoldingSetNodeID &ID, const CXXRecordDecl *RD,
bool IsVirtual, const MemRegion *SReg);

public:
const CXXRecordDecl *getDecl() const { return Data.getPointer(); }
bool isVirtual() const { return Data.getInt(); }

QualType getValueType() const override;

void dumpToStream(raw_ostream &os) const override;

void Profile(llvm::FoldingSetNodeID &ID) const override;

bool canPrintPrettyAsExpr() const override;

void printPrettyAsExpr(raw_ostream &os) const override;

static bool classof(const MemRegion *region) {
return region->getKind() == CXXBaseObjectRegionKind;
}
};






class CXXDerivedObjectRegion : public TypedValueRegion {
friend class MemRegionManager;

const CXXRecordDecl *DerivedD;

CXXDerivedObjectRegion(const CXXRecordDecl *DerivedD, const SubRegion *SReg)
: TypedValueRegion(SReg, CXXDerivedObjectRegionKind), DerivedD(DerivedD) {
assert(DerivedD);



assert(SReg->getSymbolicBase() &&
"Should have unwrapped a base region instead!");
}

static void ProfileRegion(llvm::FoldingSetNodeID &ID, const CXXRecordDecl *RD,
const MemRegion *SReg);

public:
const CXXRecordDecl *getDecl() const { return DerivedD; }

QualType getValueType() const override;

void dumpToStream(raw_ostream &os) const override;

void Profile(llvm::FoldingSetNodeID &ID) const override;

bool canPrintPrettyAsExpr() const override;

void printPrettyAsExpr(raw_ostream &os) const override;

static bool classof(const MemRegion *region) {
return region->getKind() == CXXDerivedObjectRegionKind;
}
};

template<typename RegionTy>
const RegionTy* MemRegion::getAs() const {
if (const auto *RT = dyn_cast<RegionTy>(this))
return RT;

return nullptr;
}

template<typename RegionTy>
const RegionTy* MemRegion::castAs() const {
return cast<RegionTy>(this);
}





class MemRegionManager {
ASTContext &Ctx;
llvm::BumpPtrAllocator& A;

llvm::FoldingSet<MemRegion> Regions;

GlobalInternalSpaceRegion *InternalGlobals = nullptr;
GlobalSystemSpaceRegion *SystemGlobals = nullptr;
GlobalImmutableSpaceRegion *ImmutableGlobals = nullptr;

llvm::DenseMap<const StackFrameContext *, StackLocalsSpaceRegion *>
StackLocalsSpaceRegions;
llvm::DenseMap<const StackFrameContext *, StackArgumentsSpaceRegion *>
StackArgumentsSpaceRegions;
llvm::DenseMap<const CodeTextRegion *, StaticGlobalSpaceRegion *>
StaticsGlobalSpaceRegions;

HeapSpaceRegion *heap = nullptr;
UnknownSpaceRegion *unknown = nullptr;
CodeSpaceRegion *code = nullptr;

public:
MemRegionManager(ASTContext &c, llvm::BumpPtrAllocator &a) : Ctx(c), A(a) {}
~MemRegionManager();

ASTContext &getContext() { return Ctx; }

llvm::BumpPtrAllocator &getAllocator() { return A; }



DefinedOrUnknownSVal getStaticSize(const MemRegion *MR,
SValBuilder &SVB) const;



const StackLocalsSpaceRegion *
getStackLocalsRegion(const StackFrameContext *STC);



const StackArgumentsSpaceRegion *
getStackArgumentsRegion(const StackFrameContext *STC);



const GlobalsSpaceRegion *getGlobalsRegion(
MemRegion::Kind K = MemRegion::GlobalInternalSpaceRegionKind,
const CodeTextRegion *R = nullptr);



const HeapSpaceRegion *getHeapRegion();



const UnknownSpaceRegion *getUnknownRegion();

const CodeSpaceRegion *getCodeRegion();


const AllocaRegion *getAllocaRegion(const Expr *Ex, unsigned Cnt,
const LocationContext *LC);



const CompoundLiteralRegion*
getCompoundLiteralRegion(const CompoundLiteralExpr *CL,
const LocationContext *LC);



const CXXThisRegion *getCXXThisRegion(QualType thisPointerTy,
const LocationContext *LC);


const SymbolicRegion* getSymbolicRegion(SymbolRef Sym);


const SymbolicRegion *getSymbolicHeapRegion(SymbolRef sym);

const StringRegion *getStringRegion(const StringLiteral *Str);

const ObjCStringRegion *getObjCStringRegion(const ObjCStringLiteral *Str);



const VarRegion *getVarRegion(const VarDecl *VD, const LocationContext *LC);



const NonParamVarRegion *getNonParamVarRegion(const VarDecl *VD,
const MemRegion *superR);



const ParamVarRegion *getParamVarRegion(const Expr *OriginExpr,
unsigned Index,
const LocationContext *LC);



const ElementRegion *getElementRegion(QualType elementType, NonLoc Idx,
const SubRegion *superRegion,
ASTContext &Ctx);

const ElementRegion *getElementRegionWithSuper(const ElementRegion *ER,
const SubRegion *superRegion) {
return getElementRegion(ER->getElementType(), ER->getIndex(),
superRegion, ER->getContext());
}





const FieldRegion *getFieldRegion(const FieldDecl *fd,
const SubRegion* superRegion);

const FieldRegion *getFieldRegionWithSuper(const FieldRegion *FR,
const SubRegion *superRegion) {
return getFieldRegion(FR->getDecl(), superRegion);
}





const ObjCIvarRegion *getObjCIvarRegion(const ObjCIvarDecl *ivd,
const SubRegion* superRegion);

const CXXTempObjectRegion *getCXXTempObjectRegion(Expr const *Ex,
LocationContext const *LC);





const CXXBaseObjectRegion *
getCXXBaseObjectRegion(const CXXRecordDecl *BaseClass, const SubRegion *Super,
bool IsVirtual);



const CXXBaseObjectRegion *
getCXXBaseObjectRegionWithSuper(const CXXBaseObjectRegion *baseReg,
const SubRegion *superRegion) {
return getCXXBaseObjectRegion(baseReg->getDecl(), superRegion,
baseReg->isVirtual());
}





const CXXDerivedObjectRegion *
getCXXDerivedObjectRegion(const CXXRecordDecl *BaseClass,
const SubRegion *Super);

const FunctionCodeRegion *getFunctionCodeRegion(const NamedDecl *FD);
const BlockCodeRegion *getBlockCodeRegion(const BlockDecl *BD,
CanQualType locTy,
AnalysisDeclContext *AC);





const BlockDataRegion *getBlockDataRegion(const BlockCodeRegion *bc,
const LocationContext *lc,
unsigned blockCount);




const CXXTempObjectRegion *getCXXStaticTempObjectRegion(const Expr *Ex);

private:
template <typename RegionTy, typename SuperTy,
typename Arg1Ty>
RegionTy* getSubRegion(const Arg1Ty arg1,
const SuperTy* superRegion);

template <typename RegionTy, typename SuperTy,
typename Arg1Ty, typename Arg2Ty>
RegionTy* getSubRegion(const Arg1Ty arg1, const Arg2Ty arg2,
const SuperTy* superRegion);

template <typename RegionTy, typename SuperTy,
typename Arg1Ty, typename Arg2Ty, typename Arg3Ty>
RegionTy* getSubRegion(const Arg1Ty arg1, const Arg2Ty arg2,
const Arg3Ty arg3,
const SuperTy* superRegion);

template <typename REG>
const REG* LazyAllocate(REG*& region);

template <typename REG, typename ARG>
const REG* LazyAllocate(REG*& region, ARG a);
};





inline ASTContext &MemRegion::getContext() const {
return getMemRegionManager().getContext();
}






class RegionAndSymbolInvalidationTraits {
using StorageTypeForKinds = unsigned char;

llvm::DenseMap<const MemRegion *, StorageTypeForKinds> MRTraitsMap;
llvm::DenseMap<SymbolRef, StorageTypeForKinds> SymTraitsMap;

using const_region_iterator =
llvm::DenseMap<const MemRegion *, StorageTypeForKinds>::const_iterator;
using const_symbol_iterator =
llvm::DenseMap<SymbolRef, StorageTypeForKinds>::const_iterator;

public:

enum InvalidationKinds {

TK_PreserveContents = 0x1,


TK_SuppressEscape = 0x2,


TK_DoNotInvalidateSuperRegion = 0x4,



TK_EntireMemSpace = 0x8



};

void setTrait(SymbolRef Sym, InvalidationKinds IK);
void setTrait(const MemRegion *MR, InvalidationKinds IK);
bool hasTrait(SymbolRef Sym, InvalidationKinds IK) const;
bool hasTrait(const MemRegion *MR, InvalidationKinds IK) const;
};




inline raw_ostream &operator<<(raw_ostream &os, const MemRegion *R) {
R->dumpToStream(os);
return os;
}

}

}

#endif
