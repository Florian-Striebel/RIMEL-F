












#if !defined(LLVM_CLANG_SEMA_TYPOCORRECTION_H)
#define LLVM_CLANG_SEMA_TYPOCORRECTION_H

#include "clang/AST/Decl.h"
#include "clang/AST/DeclarationName.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/PartialDiagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Sema/DeclSpec.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Casting.h"
#include <cstddef>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace clang {

class DeclContext;
class IdentifierInfo;
class LangOptions;
class MemberExpr;
class NestedNameSpecifier;
class Sema;


class TypoCorrection {
public:

static const unsigned InvalidDistance = std::numeric_limits<unsigned>::max();



static const unsigned MaximumDistance = 10000U;





static const unsigned CharDistanceWeight = 100U;
static const unsigned QualifierDistanceWeight = 110U;
static const unsigned CallbackDistanceWeight = 150U;

TypoCorrection(const DeclarationName &Name, NamedDecl *NameDecl,
NestedNameSpecifier *NNS = nullptr, unsigned CharDistance = 0,
unsigned QualifierDistance = 0)
: CorrectionName(Name), CorrectionNameSpec(NNS),
CharDistance(CharDistance), QualifierDistance(QualifierDistance) {
if (NameDecl)
CorrectionDecls.push_back(NameDecl);
}

TypoCorrection(NamedDecl *Name, NestedNameSpecifier *NNS = nullptr,
unsigned CharDistance = 0)
: CorrectionName(Name->getDeclName()), CorrectionNameSpec(NNS),
CharDistance(CharDistance) {
if (Name)
CorrectionDecls.push_back(Name);
}

TypoCorrection(DeclarationName Name, NestedNameSpecifier *NNS = nullptr,
unsigned CharDistance = 0)
: CorrectionName(Name), CorrectionNameSpec(NNS),
CharDistance(CharDistance) {}

TypoCorrection() = default;


DeclarationName getCorrection() const { return CorrectionName; }

IdentifierInfo *getCorrectionAsIdentifierInfo() const {
return CorrectionName.getAsIdentifierInfo();
}


NestedNameSpecifier *getCorrectionSpecifier() const {
return CorrectionNameSpec;
}

void setCorrectionSpecifier(NestedNameSpecifier *NNS) {
CorrectionNameSpec = NNS;
ForceSpecifierReplacement = (NNS != nullptr);
}

void WillReplaceSpecifier(bool ForceReplacement) {
ForceSpecifierReplacement = ForceReplacement;
}

bool WillReplaceSpecifier() const {
return ForceSpecifierReplacement;
}

void setQualifierDistance(unsigned ED) {
QualifierDistance = ED;
}

void setCallbackDistance(unsigned ED) {
CallbackDistance = ED;
}




static unsigned NormalizeEditDistance(unsigned ED) {
if (ED > MaximumDistance)
return InvalidDistance;
return (ED + CharDistanceWeight / 2) / CharDistanceWeight;
}




unsigned getEditDistance(bool Normalized = true) const {
if (CharDistance > MaximumDistance || QualifierDistance > MaximumDistance ||
CallbackDistance > MaximumDistance)
return InvalidDistance;
unsigned ED =
CharDistance * CharDistanceWeight +
QualifierDistance * QualifierDistanceWeight +
CallbackDistance * CallbackDistanceWeight;
if (ED > MaximumDistance)
return InvalidDistance;



return Normalized ? NormalizeEditDistance(ED) : ED;
}



NamedDecl *getFoundDecl() const {
return hasCorrectionDecl() ? *(CorrectionDecls.begin()) : nullptr;
}


NamedDecl *getCorrectionDecl() const {
auto *D = getFoundDecl();
return D ? D->getUnderlyingDecl() : nullptr;
}
template <class DeclClass>
DeclClass *getCorrectionDeclAs() const {
return dyn_cast_or_null<DeclClass>(getCorrectionDecl());
}


void ClearCorrectionDecls() {
CorrectionDecls.clear();
}


void setCorrectionDecl(NamedDecl *CDecl) {
CorrectionDecls.clear();
addCorrectionDecl(CDecl);
}


void setCorrectionDecls(ArrayRef<NamedDecl*> Decls) {
CorrectionDecls.clear();
CorrectionDecls.insert(CorrectionDecls.begin(), Decls.begin(), Decls.end());
}



void addCorrectionDecl(NamedDecl *CDecl);

std::string getAsString(const LangOptions &LO) const;

std::string getQuoted(const LangOptions &LO) const {
return "'" + getAsString(LO) + "'";
}


explicit operator bool() const { return bool(CorrectionName); }





void makeKeyword() {
CorrectionDecls.clear();
CorrectionDecls.push_back(nullptr);
ForceSpecifierReplacement = true;
}



bool isKeyword() const {
return !CorrectionDecls.empty() && CorrectionDecls.front() == nullptr;
}


template<std::size_t StrLen>
bool isKeyword(const char (&Str)[StrLen]) const {
return isKeyword() && getCorrectionAsIdentifierInfo()->isStr(Str);
}


bool isResolved() const { return !CorrectionDecls.empty(); }

bool isOverloaded() const {
return CorrectionDecls.size() > 1;
}

void setCorrectionRange(CXXScopeSpec *SS,
const DeclarationNameInfo &TypoName) {
CorrectionRange = TypoName.getSourceRange();
if (ForceSpecifierReplacement && SS && !SS->isEmpty())
CorrectionRange.setBegin(SS->getBeginLoc());
}

SourceRange getCorrectionRange() const {
return CorrectionRange;
}

using decl_iterator = SmallVectorImpl<NamedDecl *>::iterator;

decl_iterator begin() {
return isKeyword() ? CorrectionDecls.end() : CorrectionDecls.begin();
}

decl_iterator end() { return CorrectionDecls.end(); }

using const_decl_iterator = SmallVectorImpl<NamedDecl *>::const_iterator;

const_decl_iterator begin() const {
return isKeyword() ? CorrectionDecls.end() : CorrectionDecls.begin();
}

const_decl_iterator end() const { return CorrectionDecls.end(); }



bool requiresImport() const { return RequiresImport; }
void setRequiresImport(bool Req) { RequiresImport = Req; }



void addExtraDiagnostic(PartialDiagnostic PD) {
ExtraDiagnostics.push_back(std::move(PD));
}
ArrayRef<PartialDiagnostic> getExtraDiagnostics() const {
return ExtraDiagnostics;
}

private:
bool hasCorrectionDecl() const {
return (!isKeyword() && !CorrectionDecls.empty());
}


DeclarationName CorrectionName;
NestedNameSpecifier *CorrectionNameSpec = nullptr;
SmallVector<NamedDecl *, 1> CorrectionDecls;
unsigned CharDistance = 0;
unsigned QualifierDistance = 0;
unsigned CallbackDistance = 0;
SourceRange CorrectionRange;
bool ForceSpecifierReplacement = false;
bool RequiresImport = false;

std::vector<PartialDiagnostic> ExtraDiagnostics;
};



class CorrectionCandidateCallback {
public:
static const unsigned InvalidDistance = TypoCorrection::InvalidDistance;

explicit CorrectionCandidateCallback(IdentifierInfo *Typo = nullptr,
NestedNameSpecifier *TypoNNS = nullptr)
: Typo(Typo), TypoNNS(TypoNNS) {}

virtual ~CorrectionCandidateCallback() = default;











virtual bool ValidateCandidate(const TypoCorrection &candidate);







virtual unsigned RankCandidate(const TypoCorrection &candidate) {
return (!MatchesTypo(candidate) && ValidateCandidate(candidate))
? 0
: InvalidDistance;
}






virtual std::unique_ptr<CorrectionCandidateCallback> clone() = 0;

void setTypoName(IdentifierInfo *II) { Typo = II; }
void setTypoNNS(NestedNameSpecifier *NNS) { TypoNNS = NNS; }




bool WantTypeSpecifiers = true;
bool WantExpressionKeywords = true;
bool WantCXXNamedCasts = true;
bool WantFunctionLikeCasts = true;
bool WantRemainingKeywords = true;
bool WantObjCSuper = false;


bool IsObjCIvarLookup = false;
bool IsAddressOfOperand = false;

protected:
bool MatchesTypo(const TypoCorrection &candidate) {
return Typo && candidate.isResolved() && !candidate.requiresImport() &&
candidate.getCorrectionAsIdentifierInfo() == Typo &&


candidate.getCorrectionSpecifier() == TypoNNS;
}

IdentifierInfo *Typo;
NestedNameSpecifier *TypoNNS;
};

class DefaultFilterCCC final : public CorrectionCandidateCallback {
public:
explicit DefaultFilterCCC(IdentifierInfo *Typo = nullptr,
NestedNameSpecifier *TypoNNS = nullptr)
: CorrectionCandidateCallback(Typo, TypoNNS) {}

std::unique_ptr<CorrectionCandidateCallback> clone() override {
return std::make_unique<DefaultFilterCCC>(*this);
}
};



template <class C>
class DeclFilterCCC final : public CorrectionCandidateCallback {
public:
bool ValidateCandidate(const TypoCorrection &candidate) override {
return candidate.getCorrectionDeclAs<C>();
}
std::unique_ptr<CorrectionCandidateCallback> clone() override {
return std::make_unique<DeclFilterCCC>(*this);
}
};




class FunctionCallFilterCCC : public CorrectionCandidateCallback {
public:
FunctionCallFilterCCC(Sema &SemaRef, unsigned NumArgs,
bool HasExplicitTemplateArgs,
MemberExpr *ME = nullptr);

bool ValidateCandidate(const TypoCorrection &candidate) override;
std::unique_ptr<CorrectionCandidateCallback> clone() override {
return std::make_unique<FunctionCallFilterCCC>(*this);
}

private:
unsigned NumArgs;
bool HasExplicitTemplateArgs;
DeclContext *CurContext;
MemberExpr *MemberFn;
};


class NoTypoCorrectionCCC final : public CorrectionCandidateCallback {
public:
NoTypoCorrectionCCC() {
WantTypeSpecifiers = false;
WantExpressionKeywords = false;
WantCXXNamedCasts = false;
WantFunctionLikeCasts = false;
WantRemainingKeywords = false;
}

bool ValidateCandidate(const TypoCorrection &candidate) override {
return false;
}
std::unique_ptr<CorrectionCandidateCallback> clone() override {
return std::make_unique<NoTypoCorrectionCCC>(*this);
}
};

}

#endif
