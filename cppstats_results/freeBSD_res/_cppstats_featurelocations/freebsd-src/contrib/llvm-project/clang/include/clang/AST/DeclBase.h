











#if !defined(LLVM_CLANG_AST_DECLBASE_H)
#define LLVM_CLANG_AST_DECLBASE_H

#include "clang/AST/ASTDumperUtils.h"
#include "clang/AST/AttrIterator.h"
#include "clang/AST/DeclarationName.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/VersionTuple.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace clang {

class ASTContext;
class ASTMutationListener;
class Attr;
class BlockDecl;
class DeclContext;
class ExternalSourceSymbolAttr;
class FunctionDecl;
class FunctionType;
class IdentifierInfo;
enum Linkage : unsigned char;
class LinkageSpecDecl;
class Module;
class NamedDecl;
class ObjCCategoryDecl;
class ObjCCategoryImplDecl;
class ObjCContainerDecl;
class ObjCImplDecl;
class ObjCImplementationDecl;
class ObjCInterfaceDecl;
class ObjCMethodDecl;
class ObjCProtocolDecl;
struct PrintingPolicy;
class RecordDecl;
class SourceManager;
class Stmt;
class StoredDeclsMap;
class TemplateDecl;
class TemplateParameterList;
class TranslationUnitDecl;
class UsingDirectiveDecl;



enum AvailabilityResult {
AR_Available = 0,
AR_NotYetIntroduced,
AR_Deprecated,
AR_Unavailable
};








class alignas(8) Decl {
public:

enum Kind {
#define DECL(DERIVED, BASE) DERIVED,
#define ABSTRACT_DECL(DECL)
#define DECL_RANGE(BASE, START, END) first##BASE = START, last##BASE = END,

#define LAST_DECL_RANGE(BASE, START, END) first##BASE = START, last##BASE = END

#include "clang/AST/DeclNodes.inc"
};




struct EmptyShell {};












enum IdentifierNamespace {

IDNS_Label = 0x0001,







IDNS_Tag = 0x0002,




IDNS_Type = 0x0004,





IDNS_Member = 0x0008,



IDNS_Namespace = 0x0010,



IDNS_Ordinary = 0x0020,


IDNS_ObjCProtocol = 0x0040,




IDNS_OrdinaryFriend = 0x0080,




IDNS_TagFriend = 0x0100,





IDNS_Using = 0x0200,




IDNS_NonMemberOperator = 0x0400,






IDNS_LocalExtern = 0x0800,


IDNS_OMPReduction = 0x1000,


IDNS_OMPMapper = 0x2000,
};















enum ObjCDeclQualifier {
OBJC_TQ_None = 0x0,
OBJC_TQ_In = 0x1,
OBJC_TQ_Inout = 0x2,
OBJC_TQ_Out = 0x4,
OBJC_TQ_Bycopy = 0x8,
OBJC_TQ_Byref = 0x10,
OBJC_TQ_Oneway = 0x20,




OBJC_TQ_CSNullability = 0x40
};




enum class ModuleOwnershipKind : unsigned {

Unowned,






Visible,



VisibleWhenImported,



ModulePrivate
};

protected:





llvm::PointerIntPair<Decl *, 2, ModuleOwnershipKind> NextInContextAndBits;

private:
friend class DeclContext;

struct MultipleDC {
DeclContext *SemanticDC;
DeclContext *LexicalDC;
};














llvm::PointerUnion<DeclContext*, MultipleDC*> DeclCtx;

bool isInSemaDC() const { return DeclCtx.is<DeclContext*>(); }
bool isOutOfSemaDC() const { return DeclCtx.is<MultipleDC*>(); }

MultipleDC *getMultipleDC() const {
return DeclCtx.get<MultipleDC*>();
}

DeclContext *getSemanticDC() const {
return DeclCtx.get<DeclContext*>();
}


SourceLocation Loc;


unsigned DeclKind : 7;


unsigned InvalidDecl : 1;


unsigned HasAttrs : 1;



unsigned Implicit : 1;



unsigned Used : 1;





unsigned Referenced : 1;




unsigned TopLevelDeclInObjCContainer : 1;


static bool StatisticsEnabled;

protected:
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend class ASTNodeImporter;
friend class ASTReader;
friend class CXXClassMemberWrapper;
friend class LinkageComputer;
template<typename decl_type> friend class Redeclarable;



unsigned Access : 2;


unsigned FromASTFile : 1;


unsigned IdentifierNamespace : 14;



mutable unsigned CacheValidAndLinkage : 3;










void *operator new(std::size_t Size, const ASTContext &Ctx, unsigned ID,
std::size_t Extra = 0);


void *operator new(std::size_t Size, const ASTContext &Ctx,
DeclContext *Parent, std::size_t Extra = 0);

private:
bool AccessDeclContextSanity() const;



static ModuleOwnershipKind getModuleOwnershipKindForChildOf(DeclContext *DC) {
if (DC) {
auto *D = cast<Decl>(DC);
auto MOK = D->getModuleOwnershipKind();
if (MOK != ModuleOwnershipKind::Unowned &&
(!D->isFromASTFile() || D->hasLocalOwningModuleStorage()))
return MOK;


}
return ModuleOwnershipKind::Unowned;
}

public:
Decl() = delete;
Decl(const Decl&) = delete;
Decl(Decl &&) = delete;
Decl &operator=(const Decl&) = delete;
Decl &operator=(Decl&&) = delete;

protected:
Decl(Kind DK, DeclContext *DC, SourceLocation L)
: NextInContextAndBits(nullptr, getModuleOwnershipKindForChildOf(DC)),
DeclCtx(DC), Loc(L), DeclKind(DK), InvalidDecl(false), HasAttrs(false),
Implicit(false), Used(false), Referenced(false),
TopLevelDeclInObjCContainer(false), Access(AS_none), FromASTFile(0),
IdentifierNamespace(getIdentifierNamespaceForKind(DK)),
CacheValidAndLinkage(0) {
if (StatisticsEnabled) add(DK);
}

Decl(Kind DK, EmptyShell Empty)
: DeclKind(DK), InvalidDecl(false), HasAttrs(false), Implicit(false),
Used(false), Referenced(false), TopLevelDeclInObjCContainer(false),
Access(AS_none), FromASTFile(0),
IdentifierNamespace(getIdentifierNamespaceForKind(DK)),
CacheValidAndLinkage(0) {
if (StatisticsEnabled) add(DK);
}

virtual ~Decl();


void updateOutOfDate(IdentifierInfo &II) const;

Linkage getCachedLinkage() const {
return Linkage(CacheValidAndLinkage - 1);
}

void setCachedLinkage(Linkage L) const {
CacheValidAndLinkage = L + 1;
}

bool hasCachedLinkage() const {
return CacheValidAndLinkage;
}

public:

virtual SourceRange getSourceRange() const LLVM_READONLY {
return SourceRange(getLocation(), getLocation());
}

SourceLocation getBeginLoc() const LLVM_READONLY {
return getSourceRange().getBegin();
}

SourceLocation getEndLoc() const LLVM_READONLY {
return getSourceRange().getEnd();
}

SourceLocation getLocation() const { return Loc; }
void setLocation(SourceLocation L) { Loc = L; }

Kind getKind() const { return static_cast<Kind>(DeclKind); }
const char *getDeclKindName() const;

Decl *getNextDeclInContext() { return NextInContextAndBits.getPointer(); }
const Decl *getNextDeclInContext() const {return NextInContextAndBits.getPointer();}

DeclContext *getDeclContext() {
if (isInSemaDC())
return getSemanticDC();
return getMultipleDC()->SemanticDC;
}
const DeclContext *getDeclContext() const {
return const_cast<Decl*>(this)->getDeclContext();
}






Decl *getNonClosureContext();
const Decl *getNonClosureContext() const {
return const_cast<Decl*>(this)->getNonClosureContext();
}

TranslationUnitDecl *getTranslationUnitDecl();
const TranslationUnitDecl *getTranslationUnitDecl() const {
return const_cast<Decl*>(this)->getTranslationUnitDecl();
}

bool isInAnonymousNamespace() const;

bool isInStdNamespace() const;

ASTContext &getASTContext() const LLVM_READONLY;



const LangOptions &getLangOpts() const LLVM_READONLY;

void setAccess(AccessSpecifier AS) {
Access = AS;
assert(AccessDeclContextSanity());
}

AccessSpecifier getAccess() const {
assert(AccessDeclContextSanity());
return AccessSpecifier(Access);
}



AccessSpecifier getAccessUnsafe() const {
return AccessSpecifier(Access);
}

bool hasAttrs() const { return HasAttrs; }

void setAttrs(const AttrVec& Attrs) {
return setAttrsImpl(Attrs, getASTContext());
}

AttrVec &getAttrs() {
return const_cast<AttrVec&>(const_cast<const Decl*>(this)->getAttrs());
}

const AttrVec &getAttrs() const;
void dropAttrs();
void addAttr(Attr *A);

using attr_iterator = AttrVec::const_iterator;
using attr_range = llvm::iterator_range<attr_iterator>;

attr_range attrs() const {
return attr_range(attr_begin(), attr_end());
}

attr_iterator attr_begin() const {
return hasAttrs() ? getAttrs().begin() : nullptr;
}
attr_iterator attr_end() const {
return hasAttrs() ? getAttrs().end() : nullptr;
}

template <typename T>
void dropAttr() {
if (!HasAttrs) return;

AttrVec &Vec = getAttrs();
llvm::erase_if(Vec, [](Attr *A) { return isa<T>(A); });

if (Vec.empty())
HasAttrs = false;
}

template <typename T>
llvm::iterator_range<specific_attr_iterator<T>> specific_attrs() const {
return llvm::make_range(specific_attr_begin<T>(), specific_attr_end<T>());
}

template <typename T>
specific_attr_iterator<T> specific_attr_begin() const {
return specific_attr_iterator<T>(attr_begin());
}

template <typename T>
specific_attr_iterator<T> specific_attr_end() const {
return specific_attr_iterator<T>(attr_end());
}

template<typename T> T *getAttr() const {
return hasAttrs() ? getSpecificAttr<T>(getAttrs()) : nullptr;
}

template<typename T> bool hasAttr() const {
return hasAttrs() && hasSpecificAttr<T>(getAttrs());
}



unsigned getMaxAlignment() const;



void setInvalidDecl(bool Invalid = true);
bool isInvalidDecl() const { return (bool) InvalidDecl; }




bool isImplicit() const { return Implicit; }
void setImplicit(bool I = true) { Implicit = I; }







bool isUsed(bool CheckUsedAttr = true) const;





void setIsUsed() { getCanonicalDecl()->Used = true; }





void markUsed(ASTContext &C);


bool isReferenced() const;



bool isThisDeclarationReferenced() const { return Referenced; }

void setReferenced(bool R = true) { Referenced = R; }




bool isTopLevelDeclInObjCContainer() const {
return TopLevelDeclInObjCContainer;
}

void setTopLevelDeclInObjCContainer(bool V = true) {
TopLevelDeclInObjCContainer = V;
}



ExternalSourceSymbolAttr *getExternalSourceSymbolAttr() const;



bool isModulePrivate() const {
return getModuleOwnershipKind() == ModuleOwnershipKind::ModulePrivate;
}



bool hasDefiningAttr() const;


const Attr *getDefiningAttr() const;

protected:


void setModulePrivate() {


if (getModuleOwnershipKind() == ModuleOwnershipKind::Unowned)
return;
setModuleOwnershipKind(ModuleOwnershipKind::ModulePrivate);
}

public:



void setFromASTFile() {
FromASTFile = true;
}



void setOwningModuleID(unsigned ID) {
assert(isFromASTFile() && "Only works on a deserialized declaration");
*((unsigned*)this - 2) = ID;
}

public:

















AvailabilityResult
getAvailability(std::string *Message = nullptr,
VersionTuple EnclosingVersion = VersionTuple(),
StringRef *RealizedPlatform = nullptr) const;







VersionTuple getVersionIntroduced() const;






bool isDeprecated(std::string *Message = nullptr) const {
return getAvailability(Message) == AR_Deprecated;
}






bool isUnavailable(std::string *Message = nullptr) const {
return getAvailability(Message) == AR_Unavailable;
}







bool isWeakImported() const;







bool canBeWeakImported(bool &IsDefinition) const;



bool isFromASTFile() const { return FromASTFile; }



unsigned getGlobalID() const {
if (isFromASTFile())
return *((const unsigned*)this - 1);
return 0;
}



unsigned getOwningModuleID() const {
if (isFromASTFile())
return *((const unsigned*)this - 2);
return 0;
}

private:
Module *getOwningModuleSlow() const;

protected:
bool hasLocalOwningModuleStorage() const;

public:


Module *getImportedOwningModule() const {
if (!isFromASTFile() || !hasOwningModule())
return nullptr;

return getOwningModuleSlow();
}



Module *getLocalOwningModule() const {
if (isFromASTFile() || !hasOwningModule())
return nullptr;

assert(hasLocalOwningModuleStorage() &&
"owned local decl but no local module storage");
return reinterpret_cast<Module *const *>(this)[-1];
}
void setLocalOwningModule(Module *M) {
assert(!isFromASTFile() && hasOwningModule() &&
hasLocalOwningModuleStorage() &&
"should not have a cached owning module");
reinterpret_cast<Module **>(this)[-1] = M;
}


bool hasOwningModule() const {
return getModuleOwnershipKind() != ModuleOwnershipKind::Unowned;
}


Module *getOwningModule() const {
return isFromASTFile() ? getImportedOwningModule() : getLocalOwningModule();
}






Module *getOwningModuleForLinkage(bool IgnoreLinkage = false) const;






bool isUnconditionallyVisible() const {
return (int)getModuleOwnershipKind() <= (int)ModuleOwnershipKind::Visible;
}



void setVisibleDespiteOwningModule() {
if (!isUnconditionallyVisible())
setModuleOwnershipKind(ModuleOwnershipKind::Visible);
}


ModuleOwnershipKind getModuleOwnershipKind() const {
return NextInContextAndBits.getInt();
}


void setModuleOwnershipKind(ModuleOwnershipKind MOK) {
assert(!(getModuleOwnershipKind() == ModuleOwnershipKind::Unowned &&
MOK != ModuleOwnershipKind::Unowned && !isFromASTFile() &&
!hasLocalOwningModuleStorage()) &&
"no storage available for owning module for this declaration");
NextInContextAndBits.setInt(MOK);
}

unsigned getIdentifierNamespace() const {
return IdentifierNamespace;
}

bool isInIdentifierNamespace(unsigned NS) const {
return getIdentifierNamespace() & NS;
}

static unsigned getIdentifierNamespaceForKind(Kind DK);

bool hasTagIdentifierNamespace() const {
return isTagIdentifierNamespace(getIdentifierNamespace());
}

static bool isTagIdentifierNamespace(unsigned NS) {

return (NS & ~IDNS_TagFriend) == (IDNS_Tag | IDNS_Type);
}











DeclContext *getLexicalDeclContext() {
if (isInSemaDC())
return getSemanticDC();
return getMultipleDC()->LexicalDC;
}
const DeclContext *getLexicalDeclContext() const {
return const_cast<Decl*>(this)->getLexicalDeclContext();
}



virtual bool isOutOfLine() const;



void setDeclContext(DeclContext *DC);

void setLexicalDeclContext(DeclContext *DC);



bool isTemplated() const;



unsigned getTemplateDepth() const;





bool isDefinedOutsideFunctionOrMethod() const {
return getParentFunctionOrMethod() == nullptr;
}













bool isInLocalScopeForInstantiation() const;



const DeclContext *getParentFunctionOrMethod() const;
DeclContext *getParentFunctionOrMethod() {
return const_cast<DeclContext*>(
const_cast<const Decl*>(this)->getParentFunctionOrMethod());
}


virtual Decl *getCanonicalDecl() { return this; }
const Decl *getCanonicalDecl() const {
return const_cast<Decl*>(this)->getCanonicalDecl();
}


bool isCanonicalDecl() const { return getCanonicalDecl() == this; }

protected:




virtual Decl *getNextRedeclarationImpl() { return this; }



virtual Decl *getPreviousDeclImpl() { return nullptr; }



virtual Decl *getMostRecentDeclImpl() { return this; }

public:

class redecl_iterator {

Decl *Current = nullptr;
Decl *Starter;

public:
using value_type = Decl *;
using reference = const value_type &;
using pointer = const value_type *;
using iterator_category = std::forward_iterator_tag;
using difference_type = std::ptrdiff_t;

redecl_iterator() = default;
explicit redecl_iterator(Decl *C) : Current(C), Starter(C) {}

reference operator*() const { return Current; }
value_type operator->() const { return Current; }

redecl_iterator& operator++() {
assert(Current && "Advancing while iterator has reached end");

Decl *Next = Current->getNextRedeclarationImpl();
assert(Next && "Should return next redeclaration or itself, never null!");
Current = (Next != Starter) ? Next : nullptr;
return *this;
}

redecl_iterator operator++(int) {
redecl_iterator tmp(*this);
++(*this);
return tmp;
}

friend bool operator==(redecl_iterator x, redecl_iterator y) {
return x.Current == y.Current;
}

friend bool operator!=(redecl_iterator x, redecl_iterator y) {
return x.Current != y.Current;
}
};

using redecl_range = llvm::iterator_range<redecl_iterator>;



redecl_range redecls() const {
return redecl_range(redecls_begin(), redecls_end());
}

redecl_iterator redecls_begin() const {
return redecl_iterator(const_cast<Decl *>(this));
}

redecl_iterator redecls_end() const { return redecl_iterator(); }



Decl *getPreviousDecl() { return getPreviousDeclImpl(); }



const Decl *getPreviousDecl() const {
return const_cast<Decl *>(this)->getPreviousDeclImpl();
}


bool isFirstDecl() const {
return getPreviousDecl() == nullptr;
}



Decl *getMostRecentDecl() { return getMostRecentDeclImpl(); }



const Decl *getMostRecentDecl() const {
return const_cast<Decl *>(this)->getMostRecentDeclImpl();
}




virtual Stmt* getBody() const { return nullptr; }





virtual bool hasBody() const { return getBody() != nullptr; }



SourceLocation getBodyRBrace() const;


static void add(Kind k);
static void EnableStatistics();
static void PrintStats();



bool isTemplateParameter() const;



bool isTemplateParameterPack() const;


bool isParameterPack() const;


bool isTemplateDecl() const;


bool isFunctionOrFunctionTemplate() const {
return (DeclKind >= Decl::firstFunction &&
DeclKind <= Decl::lastFunction) ||
DeclKind == FunctionTemplate;
}







TemplateDecl *getDescribedTemplate() const;



const TemplateParameterList *getDescribedTemplateParams() const;



FunctionDecl *getAsFunction() LLVM_READONLY;

const FunctionDecl *getAsFunction() const {
return const_cast<Decl *>(this)->getAsFunction();
}







void setLocalExternDecl() {
Decl *Prev = getPreviousDecl();
IdentifierNamespace &= ~IDNS_Ordinary;




assert((IdentifierNamespace & ~(IDNS_OrdinaryFriend | IDNS_Tag)) == 0 &&
"namespace is not ordinary");

IdentifierNamespace |= IDNS_LocalExtern;
if (Prev && Prev->getIdentifierNamespace() & IDNS_Ordinary)
IdentifierNamespace |= IDNS_Ordinary;
}




bool isLocalExternDecl() {
return IdentifierNamespace & IDNS_LocalExtern;
}








void setObjectOfFriendDecl(bool PerformFriendInjection = false) {
unsigned OldNS = IdentifierNamespace;
assert((OldNS & (IDNS_Tag | IDNS_Ordinary |
IDNS_TagFriend | IDNS_OrdinaryFriend |
IDNS_LocalExtern | IDNS_NonMemberOperator)) &&
"namespace includes neither ordinary nor tag");
assert(!(OldNS & ~(IDNS_Tag | IDNS_Ordinary | IDNS_Type |
IDNS_TagFriend | IDNS_OrdinaryFriend |
IDNS_LocalExtern | IDNS_NonMemberOperator)) &&
"namespace includes other than ordinary or tag");

Decl *Prev = getPreviousDecl();
IdentifierNamespace &= ~(IDNS_Ordinary | IDNS_Tag | IDNS_Type);

if (OldNS & (IDNS_Tag | IDNS_TagFriend)) {
IdentifierNamespace |= IDNS_TagFriend;
if (PerformFriendInjection ||
(Prev && Prev->getIdentifierNamespace() & IDNS_Tag))
IdentifierNamespace |= IDNS_Tag | IDNS_Type;
}

if (OldNS & (IDNS_Ordinary | IDNS_OrdinaryFriend |
IDNS_LocalExtern | IDNS_NonMemberOperator)) {
IdentifierNamespace |= IDNS_OrdinaryFriend;
if (PerformFriendInjection ||
(Prev && Prev->getIdentifierNamespace() & IDNS_Ordinary))
IdentifierNamespace |= IDNS_Ordinary;
}
}

enum FriendObjectKind {
FOK_None,
FOK_Declared,
FOK_Undeclared
};





FriendObjectKind getFriendObjectKind() const {
unsigned mask =
(IdentifierNamespace & (IDNS_TagFriend | IDNS_OrdinaryFriend));
if (!mask) return FOK_None;
return (IdentifierNamespace & (IDNS_Tag | IDNS_Ordinary) ? FOK_Declared
: FOK_Undeclared);
}


void setNonMemberOperator() {
assert(getKind() == Function || getKind() == FunctionTemplate);
assert((IdentifierNamespace & IDNS_Ordinary) &&
"visible non-member operators should be in ordinary namespace");
IdentifierNamespace |= IDNS_NonMemberOperator;
}

static bool classofKind(Kind K) { return true; }
static DeclContext *castToDeclContext(const Decl *);
static Decl *castFromDeclContext(const DeclContext *);

void print(raw_ostream &Out, unsigned Indentation = 0,
bool PrintInstantiation = false) const;
void print(raw_ostream &Out, const PrintingPolicy &Policy,
unsigned Indentation = 0, bool PrintInstantiation = false) const;
static void printGroup(Decl** Begin, unsigned NumDecls,
raw_ostream &Out, const PrintingPolicy &Policy,
unsigned Indentation = 0);


void dump() const;


void dumpColor() const;

void dump(raw_ostream &Out, bool Deserialize = false,
ASTDumpOutputFormat OutputFormat = ADOF_Default) const;


int64_t getID() const;




const FunctionType *getFunctionType(bool BlocksToo = true) const;

private:
void setAttrsImpl(const AttrVec& Attrs, ASTContext &Ctx);
void setDeclContextsImpl(DeclContext *SemaDC, DeclContext *LexicalDC,
ASTContext &Ctx);

protected:
ASTMutationListener *getASTMutationListener() const;
};


inline bool declaresSameEntity(const Decl *D1, const Decl *D2) {
if (!D1 || !D2)
return false;

if (D1 == D2)
return true;

return D1->getCanonicalDecl() == D2->getCanonicalDecl();
}



class PrettyStackTraceDecl : public llvm::PrettyStackTraceEntry {
const Decl *TheDecl;
SourceLocation Loc;
SourceManager &SM;
const char *Message;

public:
PrettyStackTraceDecl(const Decl *theDecl, SourceLocation L,
SourceManager &sm, const char *Msg)
: TheDecl(theDecl), Loc(L), SM(sm), Message(Msg) {}

void print(raw_ostream &OS) const override;
};
}



namespace llvm {
template <> struct PointerLikeTypeTraits<::clang::NamedDecl *> {
static inline void *getAsVoidPointer(::clang::NamedDecl *P) { return P; }
static inline ::clang::NamedDecl *getFromVoidPointer(void *P) {
return static_cast<::clang::NamedDecl *>(P);
}
static constexpr int NumLowBitsAvailable = 3;
};
}

namespace clang {

class DeclListNode {
friend class ASTContext;
friend class StoredDeclsList;
public:
using Decls = llvm::PointerUnion<NamedDecl*, DeclListNode*>;
class iterator {
friend class DeclContextLookupResult;
friend class StoredDeclsList;

Decls Ptr;
iterator(Decls Node) : Ptr(Node) { }
public:
using difference_type = ptrdiff_t;
using value_type = NamedDecl*;
using pointer = void;
using reference = value_type;
using iterator_category = std::forward_iterator_tag;

iterator() = default;

reference operator*() const {
assert(Ptr && "dereferencing end() iterator");
if (DeclListNode *CurNode = Ptr.dyn_cast<DeclListNode*>())
return CurNode->D;
return Ptr.get<NamedDecl*>();
}
void operator->() const { }
bool operator==(const iterator &X) const { return Ptr == X.Ptr; }
bool operator!=(const iterator &X) const { return Ptr != X.Ptr; }
inline iterator &operator++() {
assert(!Ptr.isNull() && "Advancing empty iterator");

if (DeclListNode *CurNode = Ptr.dyn_cast<DeclListNode*>())
Ptr = CurNode->Rest;
else
Ptr = nullptr;
return *this;
}
iterator operator++(int) {
iterator temp = *this;
++(*this);
return temp;
}

iterator end() { return iterator(); }
};
private:
NamedDecl *D = nullptr;
Decls Rest = nullptr;
DeclListNode(NamedDecl *ND) : D(ND) {}
};


class DeclContextLookupResult {
using Decls = DeclListNode::Decls;


Decls Result;

public:
DeclContextLookupResult() = default;
DeclContextLookupResult(Decls Result) : Result(Result) {}

using iterator = DeclListNode::iterator;
using const_iterator = iterator;
using reference = iterator::reference;

iterator begin() { return iterator(Result); }
iterator end() { return iterator(); }
const_iterator begin() const {
return const_cast<DeclContextLookupResult*>(this)->begin();
}
const_iterator end() const { return iterator(); }

bool empty() const { return Result.isNull(); }
bool isSingleResult() const { return Result.dyn_cast<NamedDecl*>(); }
reference front() const { return *begin(); }





template<class T> T *find_first() const {
for (auto *D : *this)
if (T *Decl = dyn_cast<T>(D))
return Decl;

return nullptr;
}
};


















class DeclContext {

friend class ASTDeclReader;


friend class ExternalASTSource;

friend class DependentDiagnostic;


friend class ASTWriter;







class DeclContextBitfields {
friend class DeclContext;

uint64_t DeclKind : 7;




mutable uint64_t ExternalLexicalStorage : 1;




mutable uint64_t ExternalVisibleStorage : 1;





mutable uint64_t NeedToReconcileExternalVisibleStorage : 1;



mutable uint64_t HasLazyLocalLexicalLookups : 1;



mutable uint64_t HasLazyExternalLexicalLookups : 1;




mutable uint64_t UseQualifiedLookup : 1;
};


enum { NumDeclContextBits = 13 };




class TagDeclBitfields {
friend class TagDecl;

uint64_t : NumDeclContextBits;


uint64_t TagDeclKind : 3;




uint64_t IsCompleteDefinition : 1;


uint64_t IsBeingDefined : 1;



uint64_t IsEmbeddedInDeclarator : 1;


uint64_t IsFreeStanding : 1;





uint64_t MayHaveOutOfDateDef : 1;



uint64_t IsCompleteDefinitionRequired : 1;
};


enum { NumTagDeclBits = 9 };




class EnumDeclBitfields {
friend class EnumDecl;

uint64_t : NumDeclContextBits;

uint64_t : NumTagDeclBits;



uint64_t NumPositiveBits : 8;



uint64_t NumNegativeBits : 8;



uint64_t IsScoped : 1;





uint64_t IsScopedUsingClassTag : 1;



uint64_t IsFixed : 1;


uint64_t HasODRHash : 1;
};


enum { NumEnumDeclBits = 20 };




class RecordDeclBitfields {
friend class RecordDecl;

uint64_t : NumDeclContextBits;

uint64_t : NumTagDeclBits;




uint64_t HasFlexibleArrayMember : 1;


uint64_t AnonymousStructOrUnion : 1;



uint64_t HasObjectMember : 1;



uint64_t HasVolatileMember : 1;





mutable uint64_t LoadedFieldsFromExternalStorage : 1;


uint64_t NonTrivialToPrimitiveDefaultInitialize : 1;
uint64_t NonTrivialToPrimitiveCopy : 1;
uint64_t NonTrivialToPrimitiveDestroy : 1;




uint64_t HasNonTrivialToPrimitiveDefaultInitializeCUnion : 1;
uint64_t HasNonTrivialToPrimitiveDestructCUnion : 1;
uint64_t HasNonTrivialToPrimitiveCopyCUnion : 1;


uint64_t ParamDestroyedInCallee : 1;


uint64_t ArgPassingRestrictions : 2;
};


enum { NumRecordDeclBits = 14 };




class OMPDeclareReductionDeclBitfields {
friend class OMPDeclareReductionDecl;

uint64_t : NumDeclContextBits;



uint64_t InitializerKind : 2;
};


enum { NumOMPDeclareReductionDeclBits = 2 };





class FunctionDeclBitfields {
friend class FunctionDecl;

friend class CXXDeductionGuideDecl;

uint64_t : NumDeclContextBits;

uint64_t SClass : 3;
uint64_t IsInline : 1;
uint64_t IsInlineSpecified : 1;

uint64_t IsVirtualAsWritten : 1;
uint64_t IsPure : 1;
uint64_t HasInheritedPrototype : 1;
uint64_t HasWrittenPrototype : 1;
uint64_t IsDeleted : 1;

uint64_t IsTrivial : 1;




uint64_t IsTrivialForCall : 1;

uint64_t IsDefaulted : 1;
uint64_t IsExplicitlyDefaulted : 1;
uint64_t HasDefaultedFunctionInfo : 1;
uint64_t HasImplicitReturnZero : 1;
uint64_t IsLateTemplateParsed : 1;


uint64_t ConstexprKind : 2;
uint64_t InstantiationIsPending : 1;


uint64_t UsesSEHTry : 1;



uint64_t HasSkippedBody : 1;



uint64_t WillHaveBody : 1;



uint64_t IsMultiVersion : 1;




uint64_t IsCopyDeductionCandidate : 1;


uint64_t HasODRHash : 1;


uint64_t UsesFPIntrin : 1;
};


enum { NumFunctionDeclBits = 27 };




class CXXConstructorDeclBitfields {
friend class CXXConstructorDecl;

uint64_t : NumDeclContextBits;

uint64_t : NumFunctionDeclBits;






uint64_t NumCtorInitializers : 21;
uint64_t IsInheritingConstructor : 1;


uint64_t HasTrailingExplicitSpecifier : 1;


uint64_t IsSimpleExplicit : 1;
};


enum {
NumCXXConstructorDeclBits = 64 - NumDeclContextBits - NumFunctionDeclBits
};




class ObjCMethodDeclBitfields {
friend class ObjCMethodDecl;


uint64_t : NumDeclContextBits;




mutable uint64_t Family : ObjCMethodFamilyBitWidth;


uint64_t IsInstance : 1;
uint64_t IsVariadic : 1;


uint64_t IsPropertyAccessor : 1;


uint64_t IsSynthesizedAccessorStub : 1;


uint64_t IsDefined : 1;


uint64_t IsRedeclaration : 1;


mutable uint64_t HasRedeclaration : 1;


uint64_t DeclImplementation : 2;


uint64_t objcDeclQualifier : 7;


uint64_t RelatedResultType : 1;



uint64_t SelLocsKind : 2;








uint64_t IsOverriding : 1;


uint64_t HasSkippedBody : 1;
};


enum { NumObjCMethodDeclBits = 24 };




class ObjCContainerDeclBitfields {
friend class ObjCContainerDecl;

uint32_t : NumDeclContextBits;



SourceLocation AtStart;
};




enum { NumObjCContainerDeclBits = 64 - NumDeclContextBits };




class LinkageSpecDeclBitfields {
friend class LinkageSpecDecl;

uint64_t : NumDeclContextBits;



uint64_t Language : 3;





uint64_t HasBraces : 1;
};


enum { NumLinkageSpecDeclBits = 4 };




class BlockDeclBitfields {
friend class BlockDecl;

uint64_t : NumDeclContextBits;

uint64_t IsVariadic : 1;
uint64_t CapturesCXXThis : 1;
uint64_t BlockMissingReturnType : 1;
uint64_t IsConversionFromLambda : 1;



uint64_t DoesNotEscape : 1;




uint64_t CanAvoidCopyToHeap : 1;
};


enum { NumBlockDeclBits = 5 };







mutable StoredDeclsMap *LookupPtr = nullptr;

protected:








union {
DeclContextBitfields DeclContextBits;
TagDeclBitfields TagDeclBits;
EnumDeclBitfields EnumDeclBits;
RecordDeclBitfields RecordDeclBits;
OMPDeclareReductionDeclBitfields OMPDeclareReductionDeclBits;
FunctionDeclBitfields FunctionDeclBits;
CXXConstructorDeclBitfields CXXConstructorDeclBits;
ObjCMethodDeclBitfields ObjCMethodDeclBits;
ObjCContainerDeclBitfields ObjCContainerDeclBits;
LinkageSpecDeclBitfields LinkageSpecDeclBits;
BlockDeclBitfields BlockDeclBits;

static_assert(sizeof(DeclContextBitfields) <= 8,
"DeclContextBitfields is larger than 8 bytes!");
static_assert(sizeof(TagDeclBitfields) <= 8,
"TagDeclBitfields is larger than 8 bytes!");
static_assert(sizeof(EnumDeclBitfields) <= 8,
"EnumDeclBitfields is larger than 8 bytes!");
static_assert(sizeof(RecordDeclBitfields) <= 8,
"RecordDeclBitfields is larger than 8 bytes!");
static_assert(sizeof(OMPDeclareReductionDeclBitfields) <= 8,
"OMPDeclareReductionDeclBitfields is larger than 8 bytes!");
static_assert(sizeof(FunctionDeclBitfields) <= 8,
"FunctionDeclBitfields is larger than 8 bytes!");
static_assert(sizeof(CXXConstructorDeclBitfields) <= 8,
"CXXConstructorDeclBitfields is larger than 8 bytes!");
static_assert(sizeof(ObjCMethodDeclBitfields) <= 8,
"ObjCMethodDeclBitfields is larger than 8 bytes!");
static_assert(sizeof(ObjCContainerDeclBitfields) <= 8,
"ObjCContainerDeclBitfields is larger than 8 bytes!");
static_assert(sizeof(LinkageSpecDeclBitfields) <= 8,
"LinkageSpecDeclBitfields is larger than 8 bytes!");
static_assert(sizeof(BlockDeclBitfields) <= 8,
"BlockDeclBitfields is larger than 8 bytes!");
};



mutable Decl *FirstDecl = nullptr;





mutable Decl *LastDecl = nullptr;




static std::pair<Decl *, Decl *>
BuildDeclChain(ArrayRef<Decl*> Decls, bool FieldsAlreadyLoaded);

DeclContext(Decl::Kind K);

public:
~DeclContext();

Decl::Kind getDeclKind() const {
return static_cast<Decl::Kind>(DeclContextBits.DeclKind);
}

const char *getDeclKindName() const;


DeclContext *getParent() {
return cast<Decl>(this)->getDeclContext();
}
const DeclContext *getParent() const {
return const_cast<DeclContext*>(this)->getParent();
}










DeclContext *getLexicalParent() {
return cast<Decl>(this)->getLexicalDeclContext();
}
const DeclContext *getLexicalParent() const {
return const_cast<DeclContext*>(this)->getLexicalParent();
}

DeclContext *getLookupParent();

const DeclContext *getLookupParent() const {
return const_cast<DeclContext*>(this)->getLookupParent();
}

ASTContext &getParentASTContext() const {
return cast<Decl>(this)->getASTContext();
}

bool isClosure() const { return getDeclKind() == Decl::Block; }



const BlockDecl *getInnermostBlockDecl() const;

bool isObjCContainer() const {
switch (getDeclKind()) {
case Decl::ObjCCategory:
case Decl::ObjCCategoryImpl:
case Decl::ObjCImplementation:
case Decl::ObjCInterface:
case Decl::ObjCProtocol:
return true;
default:
return false;
}
}

bool isFunctionOrMethod() const {
switch (getDeclKind()) {
case Decl::Block:
case Decl::Captured:
case Decl::ObjCMethod:
return true;
default:
return getDeclKind() >= Decl::firstFunction &&
getDeclKind() <= Decl::lastFunction;
}
}


bool isLookupContext() const {
return !isFunctionOrMethod() && getDeclKind() != Decl::LinkageSpec &&
getDeclKind() != Decl::Export;
}

bool isFileContext() const {
return getDeclKind() == Decl::TranslationUnit ||
getDeclKind() == Decl::Namespace;
}

bool isTranslationUnit() const {
return getDeclKind() == Decl::TranslationUnit;
}

bool isRecord() const {
return getDeclKind() >= Decl::firstRecord &&
getDeclKind() <= Decl::lastRecord;
}

bool isNamespace() const { return getDeclKind() == Decl::Namespace; }

bool isStdNamespace() const;

bool isInlineNamespace() const;



bool isDependentContext() const;
















bool isTransparentContext() const;



bool isExternCContext() const;


const LinkageSpecDecl *getExternCContext() const;



bool isExternCXXContext() const;



bool Equals(const DeclContext *DC) const {
return DC && this->getPrimaryContext() == DC->getPrimaryContext();
}



bool Encloses(const DeclContext *DC) const;




Decl *getNonClosureAncestor();
const Decl *getNonClosureAncestor() const {
return const_cast<DeclContext*>(this)->getNonClosureAncestor();
}







DeclContext *getPrimaryContext();
const DeclContext *getPrimaryContext() const {
return const_cast<DeclContext*>(this)->getPrimaryContext();
}




DeclContext *getRedeclContext();
const DeclContext *getRedeclContext() const {
return const_cast<DeclContext *>(this)->getRedeclContext();
}


DeclContext *getEnclosingNamespaceContext();
const DeclContext *getEnclosingNamespaceContext() const {
return const_cast<DeclContext *>(this)->getEnclosingNamespaceContext();
}


RecordDecl *getOuterLexicalRecordContext();
const RecordDecl *getOuterLexicalRecordContext() const {
return const_cast<DeclContext *>(this)->getOuterLexicalRecordContext();
}







bool InEnclosingNamespaceSetOf(const DeclContext *NS) const;
























void collectAllContexts(SmallVectorImpl<DeclContext *> &Contexts);



class decl_iterator {

Decl *Current = nullptr;

public:
using value_type = Decl *;
using reference = const value_type &;
using pointer = const value_type *;
using iterator_category = std::forward_iterator_tag;
using difference_type = std::ptrdiff_t;

decl_iterator() = default;
explicit decl_iterator(Decl *C) : Current(C) {}

reference operator*() const { return Current; }


value_type operator->() const { return Current; }

decl_iterator& operator++() {
Current = Current->getNextDeclInContext();
return *this;
}

decl_iterator operator++(int) {
decl_iterator tmp(*this);
++(*this);
return tmp;
}

friend bool operator==(decl_iterator x, decl_iterator y) {
return x.Current == y.Current;
}

friend bool operator!=(decl_iterator x, decl_iterator y) {
return x.Current != y.Current;
}
};

using decl_range = llvm::iterator_range<decl_iterator>;



decl_range decls() const { return decl_range(decls_begin(), decls_end()); }
decl_iterator decls_begin() const;
decl_iterator decls_end() const { return decl_iterator(); }
bool decls_empty() const;




decl_range noload_decls() const {
return decl_range(noload_decls_begin(), noload_decls_end());
}
decl_iterator noload_decls_begin() const { return decl_iterator(FirstDecl); }
decl_iterator noload_decls_end() const { return decl_iterator(); }






template<typename SpecificDecl>
class specific_decl_iterator {



DeclContext::decl_iterator Current;




void SkipToNextDecl() {
while (*Current && !isa<SpecificDecl>(*Current))
++Current;
}

public:
using value_type = SpecificDecl *;


using reference = void;
using pointer = void;
using difference_type =
std::iterator_traits<DeclContext::decl_iterator>::difference_type;
using iterator_category = std::forward_iterator_tag;

specific_decl_iterator() = default;









explicit specific_decl_iterator(DeclContext::decl_iterator C) : Current(C) {
SkipToNextDecl();
}

value_type operator*() const { return cast<SpecificDecl>(*Current); }


value_type operator->() const { return **this; }

specific_decl_iterator& operator++() {
++Current;
SkipToNextDecl();
return *this;
}

specific_decl_iterator operator++(int) {
specific_decl_iterator tmp(*this);
++(*this);
return tmp;
}

friend bool operator==(const specific_decl_iterator& x,
const specific_decl_iterator& y) {
return x.Current == y.Current;
}

friend bool operator!=(const specific_decl_iterator& x,
const specific_decl_iterator& y) {
return x.Current != y.Current;
}
};










template<typename SpecificDecl, bool (SpecificDecl::*Acceptable)() const>
class filtered_decl_iterator {



DeclContext::decl_iterator Current;




void SkipToNextDecl() {
while (*Current &&
(!isa<SpecificDecl>(*Current) ||
(Acceptable && !(cast<SpecificDecl>(*Current)->*Acceptable)())))
++Current;
}

public:
using value_type = SpecificDecl *;


using reference = void;
using pointer = void;
using difference_type =
std::iterator_traits<DeclContext::decl_iterator>::difference_type;
using iterator_category = std::forward_iterator_tag;

filtered_decl_iterator() = default;









explicit filtered_decl_iterator(DeclContext::decl_iterator C) : Current(C) {
SkipToNextDecl();
}

value_type operator*() const { return cast<SpecificDecl>(*Current); }
value_type operator->() const { return cast<SpecificDecl>(*Current); }

filtered_decl_iterator& operator++() {
++Current;
SkipToNextDecl();
return *this;
}

filtered_decl_iterator operator++(int) {
filtered_decl_iterator tmp(*this);
++(*this);
return tmp;
}

friend bool operator==(const filtered_decl_iterator& x,
const filtered_decl_iterator& y) {
return x.Current == y.Current;
}

friend bool operator!=(const filtered_decl_iterator& x,
const filtered_decl_iterator& y) {
return x.Current != y.Current;
}
};













void addDecl(Decl *D);









void addDeclInternal(Decl *D);







void addHiddenDecl(Decl *D);


void removeDecl(Decl *D);


bool containsDecl(Decl *D) const;



bool containsDeclAndLoad(Decl *D) const;

using lookup_result = DeclContextLookupResult;
using lookup_iterator = lookup_result::iterator;






lookup_result lookup(DeclarationName Name) const;




lookup_result noload_lookup(DeclarationName Name);










void localUncachedLookup(DeclarationName Name,
SmallVectorImpl<NamedDecl *> &Results);















void makeDeclVisibleInContext(NamedDecl *D);



class all_lookups_iterator;

using lookups_range = llvm::iterator_range<all_lookups_iterator>;

lookups_range lookups() const;


lookups_range noload_lookups(bool PreserveInternalState) const;


all_lookups_iterator lookups_begin() const;
all_lookups_iterator lookups_end() const;




all_lookups_iterator noload_lookups_begin() const;
all_lookups_iterator noload_lookups_end() const;

struct udir_iterator;

using udir_iterator_base =
llvm::iterator_adaptor_base<udir_iterator, lookup_iterator,
typename lookup_iterator::iterator_category,
UsingDirectiveDecl *>;

struct udir_iterator : udir_iterator_base {
udir_iterator(lookup_iterator I) : udir_iterator_base(I) {}

UsingDirectiveDecl *operator*() const;
};

using udir_range = llvm::iterator_range<udir_iterator>;

udir_range using_directives() const;


class ddiag_iterator;

using ddiag_range = llvm::iterator_range<DeclContext::ddiag_iterator>;

inline ddiag_range ddiags() const;









void setMustBuildLookupTable() {
assert(this == getPrimaryContext() &&
"should only be called on primary context");
DeclContextBits.HasLazyExternalLexicalLookups = true;
}



StoredDeclsMap *getLookupPtr() const { return LookupPtr; }


StoredDeclsMap *buildLookup();



bool hasExternalLexicalStorage() const {
return DeclContextBits.ExternalLexicalStorage;
}



void setHasExternalLexicalStorage(bool ES = true) const {
DeclContextBits.ExternalLexicalStorage = ES;
}



bool hasExternalVisibleStorage() const {
return DeclContextBits.ExternalVisibleStorage;
}



void setHasExternalVisibleStorage(bool ES = true) const {
DeclContextBits.ExternalVisibleStorage = ES;
if (ES && LookupPtr)
DeclContextBits.NeedToReconcileExternalVisibleStorage = true;
}



bool isDeclInLexicalTraversal(const Decl *D) const {
return D && (D->NextInContextAndBits.getPointer() || D == FirstDecl ||
D == LastDecl);
}

bool setUseQualifiedLookup(bool use = true) const {
bool old_value = DeclContextBits.UseQualifiedLookup;
DeclContextBits.UseQualifiedLookup = use;
return old_value;
}

bool shouldUseQualifiedLookup() const {
return DeclContextBits.UseQualifiedLookup;
}

static bool classof(const Decl *D);
static bool classof(const DeclContext *D) { return true; }

void dumpDeclContext() const;
void dumpLookups() const;
void dumpLookups(llvm::raw_ostream &OS, bool DumpDecls = false,
bool Deserialize = false) const;

private:




bool hasNeedToReconcileExternalVisibleStorage() const {
return DeclContextBits.NeedToReconcileExternalVisibleStorage;
}





void setNeedToReconcileExternalVisibleStorage(bool Need = true) const {
DeclContextBits.NeedToReconcileExternalVisibleStorage = Need;
}



bool hasLazyLocalLexicalLookups() const {
return DeclContextBits.HasLazyLocalLexicalLookups;
}



void setHasLazyLocalLexicalLookups(bool HasLLLL = true) const {
DeclContextBits.HasLazyLocalLexicalLookups = HasLLLL;
}



bool hasLazyExternalLexicalLookups() const {
return DeclContextBits.HasLazyExternalLexicalLookups;
}



void setHasLazyExternalLexicalLookups(bool HasLELL = true) const {
DeclContextBits.HasLazyExternalLexicalLookups = HasLELL;
}

void reconcileExternalVisibleStorage() const;
bool LoadLexicalDeclsFromExternalStorage() const;







void makeDeclVisibleInContextInternal(NamedDecl *D);

StoredDeclsMap *CreateStoredDeclsMap(ASTContext &C) const;

void loadLazyLocalLexicalLookups();
void buildLookupImpl(DeclContext *DCtx, bool Internal);
void makeDeclVisibleInContextWithFlags(NamedDecl *D, bool Internal,
bool Rediscoverable);
void makeDeclVisibleInContextImpl(NamedDecl *D, bool Internal);
};

inline bool Decl::isTemplateParameter() const {
return getKind() == TemplateTypeParm || getKind() == NonTypeTemplateParm ||
getKind() == TemplateTemplateParm;
}


template <class ToTy,
bool IsKnownSubtype = ::std::is_base_of<DeclContext, ToTy>::value>
struct cast_convert_decl_context {
static const ToTy *doit(const DeclContext *Val) {
return static_cast<const ToTy*>(Decl::castFromDeclContext(Val));
}

static ToTy *doit(DeclContext *Val) {
return static_cast<ToTy*>(Decl::castFromDeclContext(Val));
}
};


template <class ToTy>
struct cast_convert_decl_context<ToTy, true> {
static const ToTy *doit(const DeclContext *Val) {
return static_cast<const ToTy*>(Val);
}

static ToTy *doit(DeclContext *Val) {
return static_cast<ToTy*>(Val);
}
};

}

namespace llvm {


template <typename To>
struct isa_impl<To, ::clang::DeclContext> {
static bool doit(const ::clang::DeclContext &Val) {
return To::classofKind(Val.getDeclKind());
}
};


template<class ToTy>
struct cast_convert_val<ToTy,
const ::clang::DeclContext,const ::clang::DeclContext> {
static const ToTy &doit(const ::clang::DeclContext &Val) {
return *::clang::cast_convert_decl_context<ToTy>::doit(&Val);
}
};

template<class ToTy>
struct cast_convert_val<ToTy, ::clang::DeclContext, ::clang::DeclContext> {
static ToTy &doit(::clang::DeclContext &Val) {
return *::clang::cast_convert_decl_context<ToTy>::doit(&Val);
}
};

template<class ToTy>
struct cast_convert_val<ToTy,
const ::clang::DeclContext*, const ::clang::DeclContext*> {
static const ToTy *doit(const ::clang::DeclContext *Val) {
return ::clang::cast_convert_decl_context<ToTy>::doit(Val);
}
};

template<class ToTy>
struct cast_convert_val<ToTy, ::clang::DeclContext*, ::clang::DeclContext*> {
static ToTy *doit(::clang::DeclContext *Val) {
return ::clang::cast_convert_decl_context<ToTy>::doit(Val);
}
};


template<class FromTy>
struct cast_convert_val< ::clang::DeclContext, FromTy, FromTy> {
static ::clang::DeclContext &doit(const FromTy &Val) {
return *FromTy::castToDeclContext(&Val);
}
};

template<class FromTy>
struct cast_convert_val< ::clang::DeclContext, FromTy*, FromTy*> {
static ::clang::DeclContext *doit(const FromTy *Val) {
return FromTy::castToDeclContext(Val);
}
};

template<class FromTy>
struct cast_convert_val< const ::clang::DeclContext, FromTy, FromTy> {
static const ::clang::DeclContext &doit(const FromTy &Val) {
return *FromTy::castToDeclContext(&Val);
}
};

template<class FromTy>
struct cast_convert_val< const ::clang::DeclContext, FromTy*, FromTy*> {
static const ::clang::DeclContext *doit(const FromTy *Val) {
return FromTy::castToDeclContext(Val);
}
};

}

#endif
