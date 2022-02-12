












#if !defined(LLVM_CLANG_SEMA_ATTRIBUTELIST_H)
#define LLVM_CLANG_SEMA_ATTRIBUTELIST_H

#include "clang/Basic/AttrSubjectMatchRules.h"
#include "clang/Basic/AttributeCommonInfo.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Sema/Ownership.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Registry.h"
#include "llvm/Support/VersionTuple.h"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <utility>

namespace clang {

class ASTContext;
class Decl;
class Expr;
class IdentifierInfo;
class LangOptions;
class ParsedAttr;
class Sema;
class Stmt;
class TargetInfo;

struct ParsedAttrInfo {

unsigned AttrKind : 16;

unsigned NumArgs : 4;

unsigned OptArgs : 4;

unsigned HasCustomParsing : 1;

unsigned IsTargetSpecific : 1;

unsigned IsType : 1;

unsigned IsStmt : 1;

unsigned IsKnownToGCC : 1;

unsigned IsSupportedByPragmaAttribute : 1;

struct Spelling {
AttributeCommonInfo::Syntax Syntax;
const char *NormalizedFullName;
};
ArrayRef<Spelling> Spellings;

ParsedAttrInfo(AttributeCommonInfo::Kind AttrKind =
AttributeCommonInfo::NoSemaHandlerAttribute)
: AttrKind(AttrKind), NumArgs(0), OptArgs(0), HasCustomParsing(0),
IsTargetSpecific(0), IsType(0), IsStmt(0), IsKnownToGCC(0),
IsSupportedByPragmaAttribute(0) {}

virtual ~ParsedAttrInfo() = default;


virtual bool diagAppertainsToDecl(Sema &S, const ParsedAttr &Attr,
const Decl *D) const {
return true;
}

virtual bool diagAppertainsToStmt(Sema &S, const ParsedAttr &Attr,
const Stmt *St) const {
return true;
}


virtual bool diagMutualExclusion(Sema &S, const ParsedAttr &A,
const Decl *D) const {
return true;
}


virtual bool diagLangOpts(Sema &S, const ParsedAttr &Attr) const {
return true;
}

virtual bool existsInTarget(const TargetInfo &Target) const {
return true;
}

virtual unsigned
spellingIndexToSemanticSpelling(const ParsedAttr &Attr) const {
return UINT_MAX;
}

virtual void getPragmaAttributeMatchRules(
llvm::SmallVectorImpl<std::pair<attr::SubjectMatchRule, bool>> &Rules,
const LangOptions &LangOpts) const {
}
enum AttrHandling {
NotHandled,
AttributeApplied,
AttributeNotApplied
};



virtual AttrHandling handleDeclAttribute(Sema &S, Decl *D,
const ParsedAttr &Attr) const {
return NotHandled;
}

static const ParsedAttrInfo &get(const AttributeCommonInfo &A);
};

typedef llvm::Registry<ParsedAttrInfo> ParsedAttrInfoRegistry;




struct AvailabilityChange {

SourceLocation KeywordLoc;


VersionTuple Version;


SourceRange VersionRange;


bool isValid() const { return !Version.empty(); }
};

namespace detail {
enum AvailabilitySlot {
IntroducedSlot, DeprecatedSlot, ObsoletedSlot, NumAvailabilitySlots
};


struct AvailabilityData {
AvailabilityChange Changes[NumAvailabilitySlots];
SourceLocation StrictLoc;
const Expr *Replacement;

AvailabilityData(const AvailabilityChange &Introduced,
const AvailabilityChange &Deprecated,
const AvailabilityChange &Obsoleted,
SourceLocation Strict, const Expr *ReplaceExpr)
: StrictLoc(Strict), Replacement(ReplaceExpr) {
Changes[IntroducedSlot] = Introduced;
Changes[DeprecatedSlot] = Deprecated;
Changes[ObsoletedSlot] = Obsoleted;
}
};

struct TypeTagForDatatypeData {
ParsedType MatchingCType;
unsigned LayoutCompatible : 1;
unsigned MustBeNull : 1;
};
struct PropertyData {
IdentifierInfo *GetterId, *SetterId;

PropertyData(IdentifierInfo *getterId, IdentifierInfo *setterId)
: GetterId(getterId), SetterId(setterId) {}
};

}


struct IdentifierLoc {
SourceLocation Loc;
IdentifierInfo *Ident;

static IdentifierLoc *create(ASTContext &Ctx, SourceLocation Loc,
IdentifierInfo *Ident);
};



using ArgsUnion = llvm::PointerUnion<Expr *, IdentifierLoc *>;
using ArgsVector = llvm::SmallVector<ArgsUnion, 12U>;










class ParsedAttr final
: public AttributeCommonInfo,
private llvm::TrailingObjects<
ParsedAttr, ArgsUnion, detail::AvailabilityData,
detail::TypeTagForDatatypeData, ParsedType, detail::PropertyData> {
friend TrailingObjects;

size_t numTrailingObjects(OverloadToken<ArgsUnion>) const { return NumArgs; }
size_t numTrailingObjects(OverloadToken<detail::AvailabilityData>) const {
return IsAvailability;
}
size_t
numTrailingObjects(OverloadToken<detail::TypeTagForDatatypeData>) const {
return IsTypeTagForDatatype;
}
size_t numTrailingObjects(OverloadToken<ParsedType>) const {
return HasParsedType;
}
size_t numTrailingObjects(OverloadToken<detail::PropertyData>) const {
return IsProperty;
}

private:
IdentifierInfo *MacroII = nullptr;
SourceLocation MacroExpansionLoc;
SourceLocation EllipsisLoc;



unsigned NumArgs : 16;


mutable unsigned Invalid : 1;


mutable unsigned UsedAsTypeAttr : 1;



unsigned IsAvailability : 1;



unsigned IsTypeTagForDatatype : 1;



unsigned IsProperty : 1;


unsigned HasParsedType : 1;


mutable unsigned HasProcessingCache : 1;


mutable unsigned ProcessingCache : 8;


mutable unsigned IsPragmaClangAttribute : 1;



SourceLocation UnavailableLoc;

const Expr *MessageExpr;

const ParsedAttrInfo &Info;

ArgsUnion *getArgsBuffer() { return getTrailingObjects<ArgsUnion>(); }
ArgsUnion const *getArgsBuffer() const {
return getTrailingObjects<ArgsUnion>();
}

detail::AvailabilityData *getAvailabilityData() {
return getTrailingObjects<detail::AvailabilityData>();
}
const detail::AvailabilityData *getAvailabilityData() const {
return getTrailingObjects<detail::AvailabilityData>();
}

private:
friend class AttributeFactory;
friend class AttributePool;


ParsedAttr(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
ArgsUnion *args, unsigned numArgs, Syntax syntaxUsed,
SourceLocation ellipsisLoc)
: AttributeCommonInfo(attrName, scopeName, attrRange, scopeLoc,
syntaxUsed),
EllipsisLoc(ellipsisLoc), NumArgs(numArgs), Invalid(false),
UsedAsTypeAttr(false), IsAvailability(false),
IsTypeTagForDatatype(false), IsProperty(false), HasParsedType(false),
HasProcessingCache(false), IsPragmaClangAttribute(false),
Info(ParsedAttrInfo::get(*this)) {
if (numArgs)
memcpy(getArgsBuffer(), args, numArgs * sizeof(ArgsUnion));
}


ParsedAttr(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierLoc *Parm, const AvailabilityChange &introduced,
const AvailabilityChange &deprecated,
const AvailabilityChange &obsoleted, SourceLocation unavailable,
const Expr *messageExpr, Syntax syntaxUsed, SourceLocation strict,
const Expr *replacementExpr)
: AttributeCommonInfo(attrName, scopeName, attrRange, scopeLoc,
syntaxUsed),
NumArgs(1), Invalid(false), UsedAsTypeAttr(false), IsAvailability(true),
IsTypeTagForDatatype(false), IsProperty(false), HasParsedType(false),
HasProcessingCache(false), IsPragmaClangAttribute(false),
UnavailableLoc(unavailable), MessageExpr(messageExpr),
Info(ParsedAttrInfo::get(*this)) {
ArgsUnion PVal(Parm);
memcpy(getArgsBuffer(), &PVal, sizeof(ArgsUnion));
new (getAvailabilityData()) detail::AvailabilityData(
introduced, deprecated, obsoleted, strict, replacementExpr);
}


ParsedAttr(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierLoc *Parm1, IdentifierLoc *Parm2, IdentifierLoc *Parm3,
Syntax syntaxUsed)
: AttributeCommonInfo(attrName, scopeName, attrRange, scopeLoc,
syntaxUsed),
NumArgs(3), Invalid(false), UsedAsTypeAttr(false),
IsAvailability(false), IsTypeTagForDatatype(false), IsProperty(false),
HasParsedType(false), HasProcessingCache(false),
IsPragmaClangAttribute(false), Info(ParsedAttrInfo::get(*this)) {
ArgsUnion *Args = getArgsBuffer();
Args[0] = Parm1;
Args[1] = Parm2;
Args[2] = Parm3;
}


ParsedAttr(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierLoc *ArgKind, ParsedType matchingCType,
bool layoutCompatible, bool mustBeNull, Syntax syntaxUsed)
: AttributeCommonInfo(attrName, scopeName, attrRange, scopeLoc,
syntaxUsed),
NumArgs(1), Invalid(false), UsedAsTypeAttr(false),
IsAvailability(false), IsTypeTagForDatatype(true), IsProperty(false),
HasParsedType(false), HasProcessingCache(false),
IsPragmaClangAttribute(false), Info(ParsedAttrInfo::get(*this)) {
ArgsUnion PVal(ArgKind);
memcpy(getArgsBuffer(), &PVal, sizeof(ArgsUnion));
detail::TypeTagForDatatypeData &ExtraData = getTypeTagForDatatypeDataSlot();
new (&ExtraData.MatchingCType) ParsedType(matchingCType);
ExtraData.LayoutCompatible = layoutCompatible;
ExtraData.MustBeNull = mustBeNull;
}


ParsedAttr(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
ParsedType typeArg, Syntax syntaxUsed)
: AttributeCommonInfo(attrName, scopeName, attrRange, scopeLoc,
syntaxUsed),
NumArgs(0), Invalid(false), UsedAsTypeAttr(false),
IsAvailability(false), IsTypeTagForDatatype(false), IsProperty(false),
HasParsedType(true), HasProcessingCache(false),
IsPragmaClangAttribute(false), Info(ParsedAttrInfo::get(*this)) {
new (&getTypeBuffer()) ParsedType(typeArg);
}


ParsedAttr(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierInfo *getterId, IdentifierInfo *setterId,
Syntax syntaxUsed)
: AttributeCommonInfo(attrName, scopeName, attrRange, scopeLoc,
syntaxUsed),
NumArgs(0), Invalid(false), UsedAsTypeAttr(false),
IsAvailability(false), IsTypeTagForDatatype(false), IsProperty(true),
HasParsedType(false), HasProcessingCache(false),
IsPragmaClangAttribute(false), Info(ParsedAttrInfo::get(*this)) {
new (&getPropertyDataBuffer()) detail::PropertyData(getterId, setterId);
}




detail::TypeTagForDatatypeData &getTypeTagForDatatypeDataSlot() {
return *getTrailingObjects<detail::TypeTagForDatatypeData>();
}
const detail::TypeTagForDatatypeData &getTypeTagForDatatypeDataSlot() const {
return *getTrailingObjects<detail::TypeTagForDatatypeData>();
}



ParsedType &getTypeBuffer() { return *getTrailingObjects<ParsedType>(); }
const ParsedType &getTypeBuffer() const {
return *getTrailingObjects<ParsedType>();
}



detail::PropertyData &getPropertyDataBuffer() {
assert(IsProperty);
return *getTrailingObjects<detail::PropertyData>();
}
const detail::PropertyData &getPropertyDataBuffer() const {
assert(IsProperty);
return *getTrailingObjects<detail::PropertyData>();
}

size_t allocated_size() const;

public:
ParsedAttr(const ParsedAttr &) = delete;
ParsedAttr(ParsedAttr &&) = delete;
ParsedAttr &operator=(const ParsedAttr &) = delete;
ParsedAttr &operator=(ParsedAttr &&) = delete;
~ParsedAttr() = delete;

void operator delete(void *) = delete;

bool hasParsedType() const { return HasParsedType; }


bool isDeclspecPropertyAttribute() const {
return IsProperty;
}

bool isInvalid() const { return Invalid; }
void setInvalid(bool b = true) const { Invalid = b; }

bool hasProcessingCache() const { return HasProcessingCache; }

unsigned getProcessingCache() const {
assert(hasProcessingCache());
return ProcessingCache;
}

void setProcessingCache(unsigned value) const {
ProcessingCache = value;
HasProcessingCache = true;
}

bool isUsedAsTypeAttr() const { return UsedAsTypeAttr; }
void setUsedAsTypeAttr(bool Used = true) { UsedAsTypeAttr = Used; }


bool isPragmaClangAttribute() const { return IsPragmaClangAttribute; }

void setIsPragmaClangAttribute() { IsPragmaClangAttribute = true; }

bool isPackExpansion() const { return EllipsisLoc.isValid(); }
SourceLocation getEllipsisLoc() const { return EllipsisLoc; }


unsigned getNumArgs() const { return NumArgs; }


ArgsUnion getArg(unsigned Arg) const {
assert(Arg < NumArgs && "Arg access out of range!");
return getArgsBuffer()[Arg];
}

bool isArgExpr(unsigned Arg) const {
return Arg < NumArgs && getArg(Arg).is<Expr*>();
}

Expr *getArgAsExpr(unsigned Arg) const {
return getArg(Arg).get<Expr*>();
}

bool isArgIdent(unsigned Arg) const {
return Arg < NumArgs && getArg(Arg).is<IdentifierLoc*>();
}

IdentifierLoc *getArgAsIdent(unsigned Arg) const {
return getArg(Arg).get<IdentifierLoc*>();
}

const AvailabilityChange &getAvailabilityIntroduced() const {
assert(getParsedKind() == AT_Availability &&
"Not an availability attribute");
return getAvailabilityData()->Changes[detail::IntroducedSlot];
}

const AvailabilityChange &getAvailabilityDeprecated() const {
assert(getParsedKind() == AT_Availability &&
"Not an availability attribute");
return getAvailabilityData()->Changes[detail::DeprecatedSlot];
}

const AvailabilityChange &getAvailabilityObsoleted() const {
assert(getParsedKind() == AT_Availability &&
"Not an availability attribute");
return getAvailabilityData()->Changes[detail::ObsoletedSlot];
}

SourceLocation getStrictLoc() const {
assert(getParsedKind() == AT_Availability &&
"Not an availability attribute");
return getAvailabilityData()->StrictLoc;
}

SourceLocation getUnavailableLoc() const {
assert(getParsedKind() == AT_Availability &&
"Not an availability attribute");
return UnavailableLoc;
}

const Expr * getMessageExpr() const {
assert(getParsedKind() == AT_Availability &&
"Not an availability attribute");
return MessageExpr;
}

const Expr *getReplacementExpr() const {
assert(getParsedKind() == AT_Availability &&
"Not an availability attribute");
return getAvailabilityData()->Replacement;
}

const ParsedType &getMatchingCType() const {
assert(getParsedKind() == AT_TypeTagForDatatype &&
"Not a type_tag_for_datatype attribute");
return getTypeTagForDatatypeDataSlot().MatchingCType;
}

bool getLayoutCompatible() const {
assert(getParsedKind() == AT_TypeTagForDatatype &&
"Not a type_tag_for_datatype attribute");
return getTypeTagForDatatypeDataSlot().LayoutCompatible;
}

bool getMustBeNull() const {
assert(getParsedKind() == AT_TypeTagForDatatype &&
"Not a type_tag_for_datatype attribute");
return getTypeTagForDatatypeDataSlot().MustBeNull;
}

const ParsedType &getTypeArg() const {
assert(HasParsedType && "Not a type attribute");
return getTypeBuffer();
}

IdentifierInfo *getPropertyDataGetter() const {
assert(isDeclspecPropertyAttribute() &&
"Not a __delcspec(property) attribute");
return getPropertyDataBuffer().GetterId;
}

IdentifierInfo *getPropertyDataSetter() const {
assert(isDeclspecPropertyAttribute() &&
"Not a __delcspec(property) attribute");
return getPropertyDataBuffer().SetterId;
}




void setMacroIdentifier(IdentifierInfo *MacroName, SourceLocation Loc) {
MacroII = MacroName;
MacroExpansionLoc = Loc;
}


bool hasMacroIdentifier() const { return MacroII != nullptr; }



IdentifierInfo *getMacroIdentifier() const { return MacroII; }

SourceLocation getMacroExpansionLoc() const {
assert(hasMacroIdentifier() && "Can only get the macro expansion location "
"if this attribute has a macro identifier.");
return MacroExpansionLoc;
}



bool checkExactlyNumArgs(class Sema &S, unsigned Num) const;


bool checkAtLeastNumArgs(class Sema &S, unsigned Num) const;


bool checkAtMostNumArgs(class Sema &S, unsigned Num) const;

bool isTargetSpecificAttr() const;
bool isTypeAttr() const;
bool isStmtAttr() const;

bool hasCustomParsing() const;
unsigned getMinArgs() const;
unsigned getMaxArgs() const;
bool hasVariadicArg() const;
bool diagnoseAppertainsTo(class Sema &S, const Decl *D) const;
bool diagnoseAppertainsTo(class Sema &S, const Stmt *St) const;
bool diagnoseMutualExclusion(class Sema &S, const Decl *D) const;



bool diagnoseMutualExclusion(class Sema &S, const Stmt *St) const {
return true;
}
bool appliesToDecl(const Decl *D, attr::SubjectMatchRule MatchRule) const;
void getMatchRules(const LangOptions &LangOpts,
SmallVectorImpl<std::pair<attr::SubjectMatchRule, bool>>
&MatchRules) const;
bool diagnoseLangOpts(class Sema &S) const;
bool existsInTarget(const TargetInfo &Target) const;
bool isKnownToGCC() const;
bool isSupportedByPragmaAttribute() const;






unsigned getSemanticSpelling() const;



LangAS asOpenCLLangAS() const {
switch (getParsedKind()) {
case ParsedAttr::AT_OpenCLConstantAddressSpace:
return LangAS::opencl_constant;
case ParsedAttr::AT_OpenCLGlobalAddressSpace:
return LangAS::opencl_global;
case ParsedAttr::AT_OpenCLGlobalDeviceAddressSpace:
return LangAS::opencl_global_device;
case ParsedAttr::AT_OpenCLGlobalHostAddressSpace:
return LangAS::opencl_global_host;
case ParsedAttr::AT_OpenCLLocalAddressSpace:
return LangAS::opencl_local;
case ParsedAttr::AT_OpenCLPrivateAddressSpace:
return LangAS::opencl_private;
case ParsedAttr::AT_OpenCLGenericAddressSpace:
return LangAS::opencl_generic;
default:
return LangAS::Default;
}
}



LangAS asSYCLLangAS() const {
switch (getKind()) {
case ParsedAttr::AT_OpenCLGlobalAddressSpace:
return LangAS::sycl_global;
case ParsedAttr::AT_OpenCLGlobalDeviceAddressSpace:
return LangAS::sycl_global_device;
case ParsedAttr::AT_OpenCLGlobalHostAddressSpace:
return LangAS::sycl_global_host;
case ParsedAttr::AT_OpenCLLocalAddressSpace:
return LangAS::sycl_local;
case ParsedAttr::AT_OpenCLPrivateAddressSpace:
return LangAS::sycl_private;
case ParsedAttr::AT_OpenCLGenericAddressSpace:
default:
return LangAS::Default;
}
}

AttributeCommonInfo::Kind getKind() const {
return AttributeCommonInfo::Kind(Info.AttrKind);
}
const ParsedAttrInfo &getInfo() const { return Info; }
};

class AttributePool;





class AttributeFactory {
public:
enum {
AvailabilityAllocSize =
ParsedAttr::totalSizeToAlloc<ArgsUnion, detail::AvailabilityData,
detail::TypeTagForDatatypeData, ParsedType,
detail::PropertyData>(1, 1, 0, 0, 0),
TypeTagForDatatypeAllocSize =
ParsedAttr::totalSizeToAlloc<ArgsUnion, detail::AvailabilityData,
detail::TypeTagForDatatypeData, ParsedType,
detail::PropertyData>(1, 0, 1, 0, 0),
PropertyAllocSize =
ParsedAttr::totalSizeToAlloc<ArgsUnion, detail::AvailabilityData,
detail::TypeTagForDatatypeData, ParsedType,
detail::PropertyData>(0, 0, 0, 0, 1),
};

private:
enum {





InlineFreeListsCapacity =
1 + (AvailabilityAllocSize - sizeof(ParsedAttr)) / sizeof(void *)
};

llvm::BumpPtrAllocator Alloc;



SmallVector<SmallVector<ParsedAttr *, 8>, InlineFreeListsCapacity> FreeLists;


friend class AttributePool;


void *allocate(size_t size);

void deallocate(ParsedAttr *AL);






void reclaimPool(AttributePool &head);

public:
AttributeFactory();
~AttributeFactory();
};

class AttributePool {
friend class AttributeFactory;
friend class ParsedAttributes;
AttributeFactory &Factory;
llvm::TinyPtrVector<ParsedAttr *> Attrs;

void *allocate(size_t size) {
return Factory.allocate(size);
}

ParsedAttr *add(ParsedAttr *attr) {
Attrs.push_back(attr);
return attr;
}

void remove(ParsedAttr *attr) {
assert(llvm::is_contained(Attrs, attr) &&
"Can't take attribute from a pool that doesn't own it!");
Attrs.erase(llvm::find(Attrs, attr));
}

void takePool(AttributePool &pool);

public:

AttributePool(AttributeFactory &factory) : Factory(factory) {}

AttributePool(const AttributePool &) = delete;

~AttributePool() { Factory.reclaimPool(*this); }


AttributePool(AttributePool &&pool) = default;

AttributeFactory &getFactory() const { return Factory; }

void clear() {
Factory.reclaimPool(*this);
Attrs.clear();
}


void takeAllFrom(AttributePool &pool) {
takePool(pool);
pool.Attrs.clear();
}

ParsedAttr *create(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
ArgsUnion *args, unsigned numArgs,
ParsedAttr::Syntax syntax,
SourceLocation ellipsisLoc = SourceLocation()) {
size_t temp =
ParsedAttr::totalSizeToAlloc<ArgsUnion, detail::AvailabilityData,
detail::TypeTagForDatatypeData, ParsedType,
detail::PropertyData>(numArgs, 0, 0, 0, 0);
(void)temp;
void *memory = allocate(
ParsedAttr::totalSizeToAlloc<ArgsUnion, detail::AvailabilityData,
detail::TypeTagForDatatypeData, ParsedType,
detail::PropertyData>(numArgs, 0, 0, 0,
0));
return add(new (memory) ParsedAttr(attrName, attrRange, scopeName, scopeLoc,
args, numArgs, syntax, ellipsisLoc));
}

ParsedAttr *create(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierLoc *Param, const AvailabilityChange &introduced,
const AvailabilityChange &deprecated,
const AvailabilityChange &obsoleted,
SourceLocation unavailable, const Expr *MessageExpr,
ParsedAttr::Syntax syntax, SourceLocation strict,
const Expr *ReplacementExpr) {
void *memory = allocate(AttributeFactory::AvailabilityAllocSize);
return add(new (memory) ParsedAttr(
attrName, attrRange, scopeName, scopeLoc, Param, introduced, deprecated,
obsoleted, unavailable, MessageExpr, syntax, strict, ReplacementExpr));
}

ParsedAttr *create(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierLoc *Param1, IdentifierLoc *Param2,
IdentifierLoc *Param3, ParsedAttr::Syntax syntax) {
void *memory = allocate(
ParsedAttr::totalSizeToAlloc<ArgsUnion, detail::AvailabilityData,
detail::TypeTagForDatatypeData, ParsedType,
detail::PropertyData>(3, 0, 0, 0, 0));
return add(new (memory) ParsedAttr(attrName, attrRange, scopeName, scopeLoc,
Param1, Param2, Param3, syntax));
}

ParsedAttr *
createTypeTagForDatatype(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierLoc *argumentKind,
ParsedType matchingCType, bool layoutCompatible,
bool mustBeNull, ParsedAttr::Syntax syntax) {
void *memory = allocate(AttributeFactory::TypeTagForDatatypeAllocSize);
return add(new (memory) ParsedAttr(attrName, attrRange, scopeName, scopeLoc,
argumentKind, matchingCType,
layoutCompatible, mustBeNull, syntax));
}

ParsedAttr *createTypeAttribute(IdentifierInfo *attrName,
SourceRange attrRange,
IdentifierInfo *scopeName,
SourceLocation scopeLoc, ParsedType typeArg,
ParsedAttr::Syntax syntaxUsed) {
void *memory = allocate(
ParsedAttr::totalSizeToAlloc<ArgsUnion, detail::AvailabilityData,
detail::TypeTagForDatatypeData, ParsedType,
detail::PropertyData>(0, 0, 0, 1, 0));
return add(new (memory) ParsedAttr(attrName, attrRange, scopeName, scopeLoc,
typeArg, syntaxUsed));
}

ParsedAttr *
createPropertyAttribute(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierInfo *getterId, IdentifierInfo *setterId,
ParsedAttr::Syntax syntaxUsed) {
void *memory = allocate(AttributeFactory::PropertyAllocSize);
return add(new (memory) ParsedAttr(attrName, attrRange, scopeName, scopeLoc,
getterId, setterId, syntaxUsed));
}
};

class ParsedAttributesView {
using VecTy = llvm::TinyPtrVector<ParsedAttr *>;
using SizeType = decltype(std::declval<VecTy>().size());

public:
bool empty() const { return AttrList.empty(); }
SizeType size() const { return AttrList.size(); }
ParsedAttr &operator[](SizeType pos) { return *AttrList[pos]; }
const ParsedAttr &operator[](SizeType pos) const { return *AttrList[pos]; }

void addAtEnd(ParsedAttr *newAttr) {
assert(newAttr);
AttrList.push_back(newAttr);
}

void remove(ParsedAttr *ToBeRemoved) {
assert(is_contained(AttrList, ToBeRemoved) &&
"Cannot remove attribute that isn't in the list");
AttrList.erase(llvm::find(AttrList, ToBeRemoved));
}

void clearListOnly() { AttrList.clear(); }

struct iterator : llvm::iterator_adaptor_base<iterator, VecTy::iterator,
std::random_access_iterator_tag,
ParsedAttr> {
iterator() : iterator_adaptor_base(nullptr) {}
iterator(VecTy::iterator I) : iterator_adaptor_base(I) {}
reference operator*() { return **I; }
friend class ParsedAttributesView;
};
struct const_iterator
: llvm::iterator_adaptor_base<const_iterator, VecTy::const_iterator,
std::random_access_iterator_tag,
ParsedAttr> {
const_iterator() : iterator_adaptor_base(nullptr) {}
const_iterator(VecTy::const_iterator I) : iterator_adaptor_base(I) {}

reference operator*() const { return **I; }
friend class ParsedAttributesView;
};

void addAll(iterator B, iterator E) {
AttrList.insert(AttrList.begin(), B.I, E.I);
}

void addAll(const_iterator B, const_iterator E) {
AttrList.insert(AttrList.begin(), B.I, E.I);
}

void addAllAtEnd(iterator B, iterator E) {
AttrList.insert(AttrList.end(), B.I, E.I);
}

void addAllAtEnd(const_iterator B, const_iterator E) {
AttrList.insert(AttrList.end(), B.I, E.I);
}

iterator begin() { return iterator(AttrList.begin()); }
const_iterator begin() const { return const_iterator(AttrList.begin()); }
iterator end() { return iterator(AttrList.end()); }
const_iterator end() const { return const_iterator(AttrList.end()); }

ParsedAttr &front() {
assert(!empty());
return *AttrList.front();
}
const ParsedAttr &front() const {
assert(!empty());
return *AttrList.front();
}
ParsedAttr &back() {
assert(!empty());
return *AttrList.back();
}
const ParsedAttr &back() const {
assert(!empty());
return *AttrList.back();
}

bool hasAttribute(ParsedAttr::Kind K) const {
return llvm::any_of(AttrList, [K](const ParsedAttr *AL) {
return AL->getParsedKind() == K;
});
}

private:
VecTy AttrList;
};







class ParsedAttributes : public ParsedAttributesView {
public:
ParsedAttributes(AttributeFactory &factory) : pool(factory) {}
ParsedAttributes(const ParsedAttributes &) = delete;

AttributePool &getPool() const { return pool; }

void takeAllFrom(ParsedAttributes &attrs) {
addAll(attrs.begin(), attrs.end());
attrs.clearListOnly();
pool.takeAllFrom(attrs.pool);
}

void takeOneFrom(ParsedAttributes &Attrs, ParsedAttr *PA) {
Attrs.getPool().remove(PA);
Attrs.remove(PA);
getPool().add(PA);
addAtEnd(PA);
}

void clear() {
clearListOnly();
pool.clear();
}


ParsedAttr *addNew(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
ArgsUnion *args, unsigned numArgs,
ParsedAttr::Syntax syntax,
SourceLocation ellipsisLoc = SourceLocation()) {
ParsedAttr *attr = pool.create(attrName, attrRange, scopeName, scopeLoc,
args, numArgs, syntax, ellipsisLoc);
addAtEnd(attr);
return attr;
}


ParsedAttr *addNew(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierLoc *Param, const AvailabilityChange &introduced,
const AvailabilityChange &deprecated,
const AvailabilityChange &obsoleted,
SourceLocation unavailable, const Expr *MessageExpr,
ParsedAttr::Syntax syntax, SourceLocation strict,
const Expr *ReplacementExpr) {
ParsedAttr *attr = pool.create(
attrName, attrRange, scopeName, scopeLoc, Param, introduced, deprecated,
obsoleted, unavailable, MessageExpr, syntax, strict, ReplacementExpr);
addAtEnd(attr);
return attr;
}


ParsedAttr *addNew(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierLoc *Param1, IdentifierLoc *Param2,
IdentifierLoc *Param3, ParsedAttr::Syntax syntax) {
ParsedAttr *attr = pool.create(attrName, attrRange, scopeName, scopeLoc,
Param1, Param2, Param3, syntax);
addAtEnd(attr);
return attr;
}


ParsedAttr *
addNewTypeTagForDatatype(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierLoc *argumentKind,
ParsedType matchingCType, bool layoutCompatible,
bool mustBeNull, ParsedAttr::Syntax syntax) {
ParsedAttr *attr = pool.createTypeTagForDatatype(
attrName, attrRange, scopeName, scopeLoc, argumentKind, matchingCType,
layoutCompatible, mustBeNull, syntax);
addAtEnd(attr);
return attr;
}


ParsedAttr *addNewTypeAttr(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
ParsedType typeArg,
ParsedAttr::Syntax syntaxUsed) {
ParsedAttr *attr = pool.createTypeAttribute(attrName, attrRange, scopeName,
scopeLoc, typeArg, syntaxUsed);
addAtEnd(attr);
return attr;
}


ParsedAttr *
addNewPropertyAttr(IdentifierInfo *attrName, SourceRange attrRange,
IdentifierInfo *scopeName, SourceLocation scopeLoc,
IdentifierInfo *getterId, IdentifierInfo *setterId,
ParsedAttr::Syntax syntaxUsed) {
ParsedAttr *attr =
pool.createPropertyAttribute(attrName, attrRange, scopeName, scopeLoc,
getterId, setterId, syntaxUsed);
addAtEnd(attr);
return attr;
}

private:
mutable AttributePool pool;
};

struct ParsedAttributesWithRange : ParsedAttributes {
ParsedAttributesWithRange(AttributeFactory &factory)
: ParsedAttributes(factory) {}

void clear() {
ParsedAttributes::clear();
Range = SourceRange();
}

SourceRange Range;
};
struct ParsedAttributesViewWithRange : ParsedAttributesView {
ParsedAttributesViewWithRange() : ParsedAttributesView() {}
void clearListOnly() {
ParsedAttributesView::clearListOnly();
Range = SourceRange();
}

SourceRange Range;
};



enum AttributeArgumentNType {
AANT_ArgumentIntOrBool,
AANT_ArgumentIntegerConstant,
AANT_ArgumentString,
AANT_ArgumentIdentifier,
AANT_ArgumentConstantExpr,
};



enum AttributeDeclKind {
ExpectedFunction,
ExpectedUnion,
ExpectedVariableOrFunction,
ExpectedFunctionOrMethod,
ExpectedFunctionMethodOrBlock,
ExpectedFunctionMethodOrParameter,
ExpectedVariable,
ExpectedVariableOrField,
ExpectedVariableFieldOrTag,
ExpectedTypeOrNamespace,
ExpectedFunctionVariableOrClass,
ExpectedKernelFunction,
ExpectedFunctionWithProtoType,
};

inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
const ParsedAttr &At) {
DB.AddTaggedVal(reinterpret_cast<intptr_t>(At.getAttrName()),
DiagnosticsEngine::ak_identifierinfo);
return DB;
}

inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
const ParsedAttr *At) {
DB.AddTaggedVal(reinterpret_cast<intptr_t>(At->getAttrName()),
DiagnosticsEngine::ak_identifierinfo);
return DB;
}






template <typename ACI,
typename std::enable_if_t<
std::is_same<ACI, AttributeCommonInfo>::value, int> = 0>
inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
const ACI &CI) {
DB.AddTaggedVal(reinterpret_cast<intptr_t>(CI.getAttrName()),
DiagnosticsEngine::ak_identifierinfo);
return DB;
}

template <typename ACI,
typename std::enable_if_t<
std::is_same<ACI, AttributeCommonInfo>::value, int> = 0>
inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
const ACI* CI) {
DB.AddTaggedVal(reinterpret_cast<intptr_t>(CI->getAttrName()),
DiagnosticsEngine::ak_identifierinfo);
return DB;
}

}

#endif
