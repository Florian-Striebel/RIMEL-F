












#if !defined(LLVM_CLANG_SEMA_SEMAINTERNAL_H)
#define LLVM_CLANG_SEMA_SEMAINTERNAL_H

#include "clang/AST/ASTContext.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/SemaDiagnostic.h"

namespace clang {

inline PartialDiagnostic Sema::PDiag(unsigned DiagID) {
return PartialDiagnostic(DiagID, Context.getDiagAllocator());
}

inline bool
FTIHasSingleVoidParameter(const DeclaratorChunk::FunctionTypeInfo &FTI) {
return FTI.NumParams == 1 && !FTI.isVariadic &&
FTI.Params[0].Ident == nullptr && FTI.Params[0].Param &&
cast<ParmVarDecl>(FTI.Params[0].Param)->getType()->isVoidType();
}

inline bool
FTIHasNonVoidParameters(const DeclaratorChunk::FunctionTypeInfo &FTI) {

return FTI.NumParams && !FTIHasSingleVoidParameter(FTI);
}




inline bool DeclAttrsMatchCUDAMode(const LangOptions &LangOpts, Decl *D) {
if (!LangOpts.CUDA || !D)
return true;
bool isDeviceSideDecl = D->hasAttr<CUDADeviceAttr>() ||
D->hasAttr<CUDASharedAttr>() ||
D->hasAttr<CUDAGlobalAttr>();
return isDeviceSideDecl == LangOpts.CUDAIsDevice;
}


inline InheritableAttr *getDLLAttr(Decl *D) {
assert(!(D->hasAttr<DLLImportAttr>() && D->hasAttr<DLLExportAttr>()) &&
"A declaration cannot be both dllimport and dllexport.");
if (auto *Import = D->getAttr<DLLImportAttr>())
return Import;
if (auto *Export = D->getAttr<DLLExportAttr>())
return Export;
return nullptr;
}


inline std::pair<unsigned, unsigned> getDepthAndIndex(NamedDecl *ND) {
if (const auto *TTP = dyn_cast<TemplateTypeParmDecl>(ND))
return std::make_pair(TTP->getDepth(), TTP->getIndex());

if (const auto *NTTP = dyn_cast<NonTypeTemplateParmDecl>(ND))
return std::make_pair(NTTP->getDepth(), NTTP->getIndex());

const auto *TTP = cast<TemplateTemplateParmDecl>(ND);
return std::make_pair(TTP->getDepth(), TTP->getIndex());
}


inline std::pair<unsigned, unsigned>
getDepthAndIndex(UnexpandedParameterPack UPP) {
if (const auto *TTP = UPP.first.dyn_cast<const TemplateTypeParmType *>())
return std::make_pair(TTP->getDepth(), TTP->getIndex());

return getDepthAndIndex(UPP.first.get<NamedDecl *>());
}

class TypoCorrectionConsumer : public VisibleDeclConsumer {
typedef SmallVector<TypoCorrection, 1> TypoResultList;
typedef llvm::StringMap<TypoResultList> TypoResultsMap;
typedef std::map<unsigned, TypoResultsMap> TypoEditDistanceMap;

public:
TypoCorrectionConsumer(Sema &SemaRef,
const DeclarationNameInfo &TypoName,
Sema::LookupNameKind LookupKind,
Scope *S, CXXScopeSpec *SS,
std::unique_ptr<CorrectionCandidateCallback> CCC,
DeclContext *MemberContext,
bool EnteringContext)
: Typo(TypoName.getName().getAsIdentifierInfo()), CurrentTCIndex(0),
SavedTCIndex(0), SemaRef(SemaRef), S(S),
SS(SS ? std::make_unique<CXXScopeSpec>(*SS) : nullptr),
CorrectionValidator(std::move(CCC)), MemberContext(MemberContext),
Result(SemaRef, TypoName, LookupKind),
Namespaces(SemaRef.Context, SemaRef.CurContext, SS),
EnteringContext(EnteringContext), SearchNamespaces(false) {
Result.suppressDiagnostics();

ValidatedCorrections.push_back(TypoCorrection());
}

bool includeHiddenDecls() const override { return true; }


void FoundDecl(NamedDecl *ND, NamedDecl *Hiding, DeclContext *Ctx,
bool InBaseClass) override;
void FoundName(StringRef Name);
void addKeywordResult(StringRef Keyword);
void addCorrection(TypoCorrection Correction);

bool empty() const {
return CorrectionResults.empty() && ValidatedCorrections.size() == 1;
}



TypoResultList &operator[](StringRef Name) {
return CorrectionResults.begin()->second[Name];
}



unsigned getBestEditDistance(bool Normalized) {
if (CorrectionResults.empty())
return (std::numeric_limits<unsigned>::max)();

unsigned BestED = CorrectionResults.begin()->first;
return Normalized ? TypoCorrection::NormalizeEditDistance(BestED) : BestED;
}





void
addNamespaces(const llvm::MapVector<NamespaceDecl *, bool> &KnownNamespaces);






const TypoCorrection &getNextCorrection();


const TypoCorrection &getCurrentCorrection() {
return CurrentTCIndex < ValidatedCorrections.size()
? ValidatedCorrections[CurrentTCIndex]
: ValidatedCorrections[0];
}





const TypoCorrection &peekNextCorrection() {
auto Current = CurrentTCIndex;
const TypoCorrection &TC = getNextCorrection();
CurrentTCIndex = Current;
return TC;
}




bool hasMadeAnyCorrectionProgress() const { return CurrentTCIndex != 0; }




void resetCorrectionStream() {
CurrentTCIndex = 0;
}



bool finished() {
return CorrectionResults.empty() &&
CurrentTCIndex >= ValidatedCorrections.size();
}



void saveCurrentPosition() {
SavedTCIndex = CurrentTCIndex;
}


void restoreSavedPosition() {
CurrentTCIndex = SavedTCIndex;
}

ASTContext &getContext() const { return SemaRef.Context; }
const LookupResult &getLookupResult() const { return Result; }

bool isAddressOfOperand() const { return CorrectionValidator->IsAddressOfOperand; }
const CXXScopeSpec *getSS() const { return SS.get(); }
Scope *getScope() const { return S; }
CorrectionCandidateCallback *getCorrectionValidator() const {
return CorrectionValidator.get();
}

private:
class NamespaceSpecifierSet {
struct SpecifierInfo {
DeclContext* DeclCtx;
NestedNameSpecifier* NameSpecifier;
unsigned EditDistance;
};

typedef SmallVector<DeclContext*, 4> DeclContextList;
typedef SmallVector<SpecifierInfo, 16> SpecifierInfoList;

ASTContext &Context;
DeclContextList CurContextChain;
std::string CurNameSpecifier;
SmallVector<const IdentifierInfo*, 4> CurContextIdentifiers;
SmallVector<const IdentifierInfo*, 4> CurNameSpecifierIdentifiers;

std::map<unsigned, SpecifierInfoList> DistanceMap;



static DeclContextList buildContextChain(DeclContext *Start);

unsigned buildNestedNameSpecifier(DeclContextList &DeclChain,
NestedNameSpecifier *&NNS);

public:
NamespaceSpecifierSet(ASTContext &Context, DeclContext *CurContext,
CXXScopeSpec *CurScopeSpec);



void addNameSpecifier(DeclContext *Ctx);


class iterator
: public llvm::iterator_facade_base<iterator, std::forward_iterator_tag,
SpecifierInfo> {

const std::map<unsigned, SpecifierInfoList>::iterator OuterBack;

std::map<unsigned, SpecifierInfoList>::iterator Outer;

SpecifierInfoList::iterator Inner;

public:
iterator(NamespaceSpecifierSet &Set, bool IsAtEnd)
: OuterBack(std::prev(Set.DistanceMap.end())),
Outer(Set.DistanceMap.begin()),
Inner(!IsAtEnd ? Outer->second.begin() : OuterBack->second.end()) {
assert(!Set.DistanceMap.empty());
}

iterator &operator++() {
++Inner;
if (Inner == Outer->second.end() && Outer != OuterBack) {
++Outer;
Inner = Outer->second.begin();
}
return *this;
}

SpecifierInfo &operator*() { return *Inner; }
bool operator==(const iterator &RHS) const { return Inner == RHS.Inner; }
};

iterator begin() { return iterator(*this, false); }
iterator end() { return iterator(*this, true); }
};

void addName(StringRef Name, NamedDecl *ND,
NestedNameSpecifier *NNS = nullptr, bool isKeyword = false);




bool resolveCorrection(TypoCorrection &Candidate);





void performQualifiedLookups();


IdentifierInfo *Typo;






TypoEditDistanceMap CorrectionResults;

SmallVector<TypoCorrection, 4> ValidatedCorrections;
size_t CurrentTCIndex;
size_t SavedTCIndex;

Sema &SemaRef;
Scope *S;
std::unique_ptr<CXXScopeSpec> SS;
std::unique_ptr<CorrectionCandidateCallback> CorrectionValidator;
DeclContext *MemberContext;
LookupResult Result;
NamespaceSpecifierSet Namespaces;
SmallVector<TypoCorrection, 2> QualifiedResults;
bool EnteringContext;
bool SearchNamespaces;
};

inline Sema::TypoExprState::TypoExprState() {}

inline Sema::TypoExprState::TypoExprState(TypoExprState &&other) noexcept {
*this = std::move(other);
}

inline Sema::TypoExprState &Sema::TypoExprState::
operator=(Sema::TypoExprState &&other) noexcept {
Consumer = std::move(other.Consumer);
DiagHandler = std::move(other.DiagHandler);
RecoveryHandler = std::move(other.RecoveryHandler);
return *this;
}

}

#endif
