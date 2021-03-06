












#if !defined(LLVM_CLANG_SEMA_LOOKUP_H)
#define LLVM_CLANG_SEMA_LOOKUP_H

#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/Type.h"
#include "clang/AST/UnresolvedSet.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Sema/Sema.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/Casting.h"
#include <cassert>
#include <utility>

namespace clang {

class CXXBasePaths;








class LookupResult {
public:
enum LookupResultKind {

NotFound = 0,




NotFoundInCurrentInstantiation,



Found,



FoundOverloaded,




FoundUnresolvedValue,




Ambiguous
};

enum AmbiguityKind {












AmbiguousBaseSubobjectTypes,













AmbiguousBaseSubobjects,














AmbiguousReference,
















AmbiguousTagHiding
};


enum TemporaryToken {
Temporary
};

using iterator = UnresolvedSetImpl::iterator;

LookupResult(Sema &SemaRef, const DeclarationNameInfo &NameInfo,
Sema::LookupNameKind LookupKind,
Sema::RedeclarationKind Redecl = Sema::NotForRedeclaration)
: SemaPtr(&SemaRef), NameInfo(NameInfo), LookupKind(LookupKind),
Redecl(Redecl != Sema::NotForRedeclaration),
ExternalRedecl(Redecl == Sema::ForExternalRedeclaration),
Diagnose(Redecl == Sema::NotForRedeclaration) {
configure();
}




LookupResult(Sema &SemaRef, DeclarationName Name,
SourceLocation NameLoc, Sema::LookupNameKind LookupKind,
Sema::RedeclarationKind Redecl = Sema::NotForRedeclaration)
: SemaPtr(&SemaRef), NameInfo(Name, NameLoc), LookupKind(LookupKind),
Redecl(Redecl != Sema::NotForRedeclaration),
ExternalRedecl(Redecl == Sema::ForExternalRedeclaration),
Diagnose(Redecl == Sema::NotForRedeclaration) {
configure();
}




LookupResult(TemporaryToken _, const LookupResult &Other)
: SemaPtr(Other.SemaPtr), NameInfo(Other.NameInfo),
LookupKind(Other.LookupKind), IDNS(Other.IDNS), Redecl(Other.Redecl),
ExternalRedecl(Other.ExternalRedecl), HideTags(Other.HideTags),
AllowHidden(Other.AllowHidden),
TemplateNameLookup(Other.TemplateNameLookup) {}



LookupResult(const LookupResult &) = delete;
LookupResult &operator=(const LookupResult &) = delete;

LookupResult(LookupResult &&Other)
: ResultKind(std::move(Other.ResultKind)),
Ambiguity(std::move(Other.Ambiguity)), Decls(std::move(Other.Decls)),
Paths(std::move(Other.Paths)),
NamingClass(std::move(Other.NamingClass)),
BaseObjectType(std::move(Other.BaseObjectType)),
SemaPtr(std::move(Other.SemaPtr)), NameInfo(std::move(Other.NameInfo)),
NameContextRange(std::move(Other.NameContextRange)),
LookupKind(std::move(Other.LookupKind)), IDNS(std::move(Other.IDNS)),
Redecl(std::move(Other.Redecl)),
ExternalRedecl(std::move(Other.ExternalRedecl)),
HideTags(std::move(Other.HideTags)),
Diagnose(std::move(Other.Diagnose)),
AllowHidden(std::move(Other.AllowHidden)),
Shadowed(std::move(Other.Shadowed)),
TemplateNameLookup(std::move(Other.TemplateNameLookup)) {
Other.Paths = nullptr;
Other.Diagnose = false;
}

LookupResult &operator=(LookupResult &&Other) {
ResultKind = std::move(Other.ResultKind);
Ambiguity = std::move(Other.Ambiguity);
Decls = std::move(Other.Decls);
Paths = std::move(Other.Paths);
NamingClass = std::move(Other.NamingClass);
BaseObjectType = std::move(Other.BaseObjectType);
SemaPtr = std::move(Other.SemaPtr);
NameInfo = std::move(Other.NameInfo);
NameContextRange = std::move(Other.NameContextRange);
LookupKind = std::move(Other.LookupKind);
IDNS = std::move(Other.IDNS);
Redecl = std::move(Other.Redecl);
ExternalRedecl = std::move(Other.ExternalRedecl);
HideTags = std::move(Other.HideTags);
Diagnose = std::move(Other.Diagnose);
AllowHidden = std::move(Other.AllowHidden);
Shadowed = std::move(Other.Shadowed);
TemplateNameLookup = std::move(Other.TemplateNameLookup);
Other.Paths = nullptr;
Other.Diagnose = false;
return *this;
}

~LookupResult() {
if (Diagnose) diagnose();
if (Paths) deletePaths(Paths);
}


const DeclarationNameInfo &getLookupNameInfo() const {
return NameInfo;
}


void setLookupNameInfo(const DeclarationNameInfo &NameInfo) {
this->NameInfo = NameInfo;
}


DeclarationName getLookupName() const {
return NameInfo.getName();
}


void setLookupName(DeclarationName Name) {
NameInfo.setName(Name);
}


Sema::LookupNameKind getLookupKind() const {
return LookupKind;
}


bool isForRedeclaration() const {
return Redecl;
}



bool isForExternalRedeclaration() const {
return ExternalRedecl;
}

Sema::RedeclarationKind redeclarationKind() const {
return ExternalRedecl ? Sema::ForExternalRedeclaration :
Redecl ? Sema::ForVisibleRedeclaration : Sema::NotForRedeclaration;
}



void setAllowHidden(bool AH) {
AllowHidden = AH;
}



bool isHiddenDeclarationVisible(NamedDecl *ND) const {
return AllowHidden ||
(isForExternalRedeclaration() && ND->isExternallyDeclarable());
}



void setHideTags(bool Hide) {
HideTags = Hide;
}




void setTemplateNameLookup(bool TemplateName) {
TemplateNameLookup = TemplateName;
}

bool isTemplateNameLookup() const { return TemplateNameLookup; }

bool isAmbiguous() const {
return getResultKind() == Ambiguous;
}




bool isSingleResult() const {
return getResultKind() == Found;
}


bool isOverloadedResult() const {
return getResultKind() == FoundOverloaded;
}

bool isUnresolvableResult() const {
return getResultKind() == FoundUnresolvedValue;
}

LookupResultKind getResultKind() const {
assert(sanity());
return ResultKind;
}

AmbiguityKind getAmbiguityKind() const {
assert(isAmbiguous());
return Ambiguity;
}

const UnresolvedSetImpl &asUnresolvedSet() const {
return Decls;
}

iterator begin() const { return iterator(Decls.begin()); }
iterator end() const { return iterator(Decls.end()); }


bool empty() const { return Decls.empty(); }



CXXBasePaths *getBasePaths() const {
return Paths;
}



static bool isVisible(Sema &SemaRef, NamedDecl *D) {

if (D->isUnconditionallyVisible())
return true;



return isVisibleSlow(SemaRef, D);
}



NamedDecl *getAcceptableDecl(NamedDecl *D) const {
if (!D->isInIdentifierNamespace(IDNS))
return nullptr;

if (isVisible(getSema(), D) || isHiddenDeclarationVisible(D))
return D;

return getAcceptableDeclSlow(D);
}

private:
static bool isVisibleSlow(Sema &SemaRef, NamedDecl *D);
NamedDecl *getAcceptableDeclSlow(NamedDecl *D) const;

public:

unsigned getIdentifierNamespace() const {
return IDNS;
}



bool isClassLookup() const {
return NamingClass != nullptr;
}

















CXXRecordDecl *getNamingClass() const {
return NamingClass;
}


void setNamingClass(CXXRecordDecl *Record) {
NamingClass = Record;
}




QualType getBaseObjectType() const {
return BaseObjectType;
}


void setBaseObjectType(QualType T) {
BaseObjectType = T;
}



void addDecl(NamedDecl *D) {
addDecl(D, D->getAccess());
}



void addDecl(NamedDecl *D, AccessSpecifier AS) {
Decls.addDecl(D, AS);
ResultKind = Found;
}



void addAllDecls(const LookupResult &Other) {
Decls.append(Other.Decls.begin(), Other.Decls.end());
ResultKind = Found;
}



bool wasNotFoundInCurrentInstantiation() const {
return ResultKind == NotFoundInCurrentInstantiation;
}



void setNotFoundInCurrentInstantiation() {
assert(ResultKind == NotFound && Decls.empty());
ResultKind = NotFoundInCurrentInstantiation;
}



bool isShadowed() const { return Shadowed; }



void setShadowed() { Shadowed = true; }






void resolveKind();



void resolveKindAfterFilter() {
if (Decls.empty()) {
if (ResultKind != NotFoundInCurrentInstantiation)
ResultKind = NotFound;

if (Paths) {
deletePaths(Paths);
Paths = nullptr;
}
} else {
llvm::Optional<AmbiguityKind> SavedAK;
bool WasAmbiguous = false;
if (ResultKind == Ambiguous) {
SavedAK = Ambiguity;
WasAmbiguous = true;
}
ResultKind = Found;
resolveKind();



if (ResultKind == Ambiguous) {
(void)WasAmbiguous;
assert(WasAmbiguous);
Ambiguity = SavedAK.getValue();
} else if (Paths) {
deletePaths(Paths);
Paths = nullptr;
}
}
}

template <class DeclClass>
DeclClass *getAsSingle() const {
if (getResultKind() != Found) return nullptr;
return dyn_cast<DeclClass>(getFoundDecl());
}






NamedDecl *getFoundDecl() const {
assert(getResultKind() == Found
&& "getFoundDecl called on non-unique result");
return (*begin())->getUnderlyingDecl();
}


NamedDecl *getRepresentativeDecl() const {
assert(!Decls.empty() && "cannot get representative of empty set");
return *begin();
}


bool isSingleTagDecl() const {
return getResultKind() == Found && isa<TagDecl>(getFoundDecl());
}





void setAmbiguousBaseSubobjectTypes(CXXBasePaths &P);





void setAmbiguousBaseSubobjects(CXXBasePaths &P);




void setAmbiguousQualifiedTagHiding() {
setAmbiguous(AmbiguousTagHiding);
}


LLVM_ATTRIBUTE_REINITIALIZES void clear() {
ResultKind = NotFound;
Decls.clear();
if (Paths) deletePaths(Paths);
Paths = nullptr;
NamingClass = nullptr;
Shadowed = false;
}



void clear(Sema::LookupNameKind Kind) {
clear();
LookupKind = Kind;
configure();
}


void setRedeclarationKind(Sema::RedeclarationKind RK) {
Redecl = (RK != Sema::NotForRedeclaration);
ExternalRedecl = (RK == Sema::ForExternalRedeclaration);
configure();
}

void dump();
void print(raw_ostream &);



void suppressDiagnostics() {
Diagnose = false;
}


bool isSuppressingDiagnostics() const {
return !Diagnose;
}


void setContextRange(SourceRange SR) {
NameContextRange = SR;
}




SourceRange getContextRange() const {
return NameContextRange;
}



SourceLocation getNameLoc() const {
return NameInfo.getLoc();
}



Sema &getSema() const { return *SemaPtr; }




class Filter {
friend class LookupResult;

LookupResult &Results;
LookupResult::iterator I;
bool Changed = false;
bool CalledDone = false;

Filter(LookupResult &Results) : Results(Results), I(Results.begin()) {}

public:
Filter(Filter &&F)
: Results(F.Results), I(F.I), Changed(F.Changed),
CalledDone(F.CalledDone) {
F.CalledDone = true;
}

~Filter() {
assert(CalledDone &&
"LookupResult::Filter destroyed without done() call");
}

bool hasNext() const {
return I != Results.end();
}

NamedDecl *next() {
assert(I != Results.end() && "next() called on empty filter");
return *I++;
}


void restart() {
I = Results.begin();
}


void erase() {
Results.Decls.erase(--I);
Changed = true;
}



void replace(NamedDecl *D) {
Results.Decls.replace(I-1, D);
Changed = true;
}


void replace(NamedDecl *D, AccessSpecifier AS) {
Results.Decls.replace(I-1, D, AS);
Changed = true;
}

void done() {
assert(!CalledDone && "done() called twice");
CalledDone = true;

if (Changed)
Results.resolveKindAfterFilter();
}
};


Filter makeFilter() {
return Filter(*this);
}

void setFindLocalExtern(bool FindLocalExtern) {
if (FindLocalExtern)
IDNS |= Decl::IDNS_LocalExtern;
else
IDNS &= ~Decl::IDNS_LocalExtern;
}

private:
void diagnose() {
if (isAmbiguous())
getSema().DiagnoseAmbiguousLookup(*this);
else if (isClassLookup() && getSema().getLangOpts().AccessControl)
getSema().CheckLookupAccess(*this);
}

void setAmbiguous(AmbiguityKind AK) {
ResultKind = Ambiguous;
Ambiguity = AK;
}

void addDeclsFromBasePaths(const CXXBasePaths &P);
void configure();


bool sanity() const;

bool sanityCheckUnresolved() const {
for (iterator I = begin(), E = end(); I != E; ++I)
if (isa<UnresolvedUsingValueDecl>((*I)->getUnderlyingDecl()))
return true;
return false;
}

static void deletePaths(CXXBasePaths *);


LookupResultKind ResultKind = NotFound;


AmbiguityKind Ambiguity = {};
UnresolvedSet<8> Decls;
CXXBasePaths *Paths = nullptr;
CXXRecordDecl *NamingClass = nullptr;
QualType BaseObjectType;


Sema *SemaPtr;
DeclarationNameInfo NameInfo;
SourceRange NameContextRange;
Sema::LookupNameKind LookupKind;
unsigned IDNS = 0;

bool Redecl;
bool ExternalRedecl;



bool HideTags = true;

bool Diagnose = false;


bool AllowHidden = false;




bool Shadowed = false;


bool TemplateNameLookup = false;
};







class VisibleDeclConsumer {
public:

virtual ~VisibleDeclConsumer();




virtual bool includeHiddenDecls() const;













virtual void FoundDecl(NamedDecl *ND, NamedDecl *Hiding, DeclContext *Ctx,
bool InBaseClass) = 0;





virtual void EnteredContext(DeclContext *Ctx) {}
};


class ADLResult {
private:

llvm::MapVector<NamedDecl*, NamedDecl*> Decls;

struct select_second {
NamedDecl *operator()(std::pair<NamedDecl*, NamedDecl*> P) const {
return P.second;
}
};

public:

void insert(NamedDecl *D);


void erase(NamedDecl *D) {
Decls.erase(cast<NamedDecl>(D->getCanonicalDecl()));
}

using iterator =
llvm::mapped_iterator<decltype(Decls)::iterator, select_second>;

iterator begin() { return iterator(Decls.begin(), select_second()); }
iterator end() { return iterator(Decls.end(), select_second()); }
};

}

#endif
