













#if !defined(LLVM_CLANG_BASIC_IDENTIFIERTABLE_H)
#define LLVM_CLANG_BASIC_IDENTIFIERTABLE_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/TokenKinds.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/type_traits.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>

namespace clang {

class DeclarationName;
class DeclarationNameTable;
class IdentifierInfo;
class LangOptions;
class MultiKeywordSelector;
class SourceLocation;

enum class ReservedIdentifierStatus {
NotReserved = 0,
StartsWithUnderscoreAtGlobalScope,
StartsWithDoubleUnderscore,
StartsWithUnderscoreFollowedByCapitalLetter,
ContainsDoubleUnderscore,
};


using IdentifierLocPair = std::pair<IdentifierInfo *, SourceLocation>;




enum { IdentifierInfoAlignment = 8 };

static constexpr int ObjCOrBuiltinIDBits = 16;







class alignas(IdentifierInfoAlignment) IdentifierInfo {
friend class IdentifierTable;


unsigned TokenID : 9;




unsigned ObjCOrBuiltinID : ObjCOrBuiltinIDBits;


unsigned HasMacro : 1;


unsigned HadMacro : 1;


unsigned IsExtension : 1;


unsigned IsFutureCompatKeyword : 1;


unsigned IsPoisoned : 1;


unsigned IsCPPOperatorKeyword : 1;



unsigned NeedsHandleIdentifier : 1;


unsigned IsFromAST : 1;



unsigned ChangedAfterLoad : 1;



unsigned FEChangedAfterLoad : 1;


unsigned RevertedTokenID : 1;



unsigned OutOfDate : 1;


unsigned IsModulesImport : 1;


unsigned IsMangledOpenMPVariantName : 1;




void *FETokenInfo = nullptr;

llvm::StringMapEntry<IdentifierInfo *> *Entry = nullptr;

IdentifierInfo()
: TokenID(tok::identifier), ObjCOrBuiltinID(0), HasMacro(false),
HadMacro(false), IsExtension(false), IsFutureCompatKeyword(false),
IsPoisoned(false), IsCPPOperatorKeyword(false),
NeedsHandleIdentifier(false), IsFromAST(false), ChangedAfterLoad(false),
FEChangedAfterLoad(false), RevertedTokenID(false), OutOfDate(false),
IsModulesImport(false), IsMangledOpenMPVariantName(false) {}

public:
IdentifierInfo(const IdentifierInfo &) = delete;
IdentifierInfo &operator=(const IdentifierInfo &) = delete;
IdentifierInfo(IdentifierInfo &&) = delete;
IdentifierInfo &operator=(IdentifierInfo &&) = delete;




template <std::size_t StrLen>
bool isStr(const char (&Str)[StrLen]) const {
return getLength() == StrLen-1 &&
memcmp(getNameStart(), Str, StrLen-1) == 0;
}


bool isStr(llvm::StringRef Str) const {
llvm::StringRef ThisStr(getNameStart(), getLength());
return ThisStr == Str;
}



const char *getNameStart() const { return Entry->getKeyData(); }


unsigned getLength() const { return Entry->getKeyLength(); }


StringRef getName() const {
return StringRef(getNameStart(), getLength());
}



bool hasMacroDefinition() const {
return HasMacro;
}
void setHasMacroDefinition(bool Val) {
if (HasMacro == Val) return;

HasMacro = Val;
if (Val) {
NeedsHandleIdentifier = true;
HadMacro = true;
} else {
RecomputeNeedsHandleIdentifier();
}
}



bool hadMacroDefinition() const {
return HadMacro;
}




tok::TokenKind getTokenID() const { return (tok::TokenKind)TokenID; }


bool hasRevertedTokenIDToIdentifier() const { return RevertedTokenID; }







void revertTokenIDToIdentifier() {
assert(TokenID != tok::identifier && "Already at tok::identifier");
TokenID = tok::identifier;
RevertedTokenID = true;
}
void revertIdentifierToTokenID(tok::TokenKind TK) {
assert(TokenID == tok::identifier && "Should be at tok::identifier");
TokenID = TK;
RevertedTokenID = false;
}




tok::PPKeywordKind getPPKeywordID() const;




tok::ObjCKeywordKind getObjCKeywordID() const {
if (ObjCOrBuiltinID < tok::NUM_OBJC_KEYWORDS)
return tok::ObjCKeywordKind(ObjCOrBuiltinID);
else
return tok::objc_not_keyword;
}
void setObjCKeywordID(tok::ObjCKeywordKind ID) { ObjCOrBuiltinID = ID; }




unsigned getBuiltinID() const {
if (ObjCOrBuiltinID >= tok::NUM_OBJC_KEYWORDS)
return ObjCOrBuiltinID - tok::NUM_OBJC_KEYWORDS;
else
return 0;
}
void setBuiltinID(unsigned ID) {
ObjCOrBuiltinID = ID + tok::NUM_OBJC_KEYWORDS;
assert(ObjCOrBuiltinID - unsigned(tok::NUM_OBJC_KEYWORDS) == ID
&& "ID too large for field!");
}

unsigned getObjCOrBuiltinID() const { return ObjCOrBuiltinID; }
void setObjCOrBuiltinID(unsigned ID) { ObjCOrBuiltinID = ID; }




bool isExtensionToken() const { return IsExtension; }
void setIsExtensionToken(bool Val) {
IsExtension = Val;
if (Val)
NeedsHandleIdentifier = true;
else
RecomputeNeedsHandleIdentifier();
}






bool isFutureCompatKeyword() const { return IsFutureCompatKeyword; }
void setIsFutureCompatKeyword(bool Val) {
IsFutureCompatKeyword = Val;
if (Val)
NeedsHandleIdentifier = true;
else
RecomputeNeedsHandleIdentifier();
}



void setIsPoisoned(bool Value = true) {
IsPoisoned = Value;
if (Value)
NeedsHandleIdentifier = true;
else
RecomputeNeedsHandleIdentifier();
}


bool isPoisoned() const { return IsPoisoned; }



void setIsCPlusPlusOperatorKeyword(bool Val = true) {
IsCPPOperatorKeyword = Val;
}
bool isCPlusPlusOperatorKeyword() const { return IsCPPOperatorKeyword; }


bool isKeyword(const LangOptions &LangOpts) const;



bool isCPlusPlusKeyword(const LangOptions &LangOpts) const;



void *getFETokenInfo() const { return FETokenInfo; }
void setFETokenInfo(void *T) { FETokenInfo = T; }






bool isHandleIdentifierCase() const { return NeedsHandleIdentifier; }



bool isFromAST() const { return IsFromAST; }

void setIsFromAST() { IsFromAST = true; }



bool hasChangedSinceDeserialization() const {
return ChangedAfterLoad;
}



void setChangedSinceDeserialization() {
ChangedAfterLoad = true;
}



bool hasFETokenInfoChangedSinceDeserialization() const {
return FEChangedAfterLoad;
}



void setFETokenInfoChangedSinceDeserialization() {
FEChangedAfterLoad = true;
}



bool isOutOfDate() const { return OutOfDate; }



void setOutOfDate(bool OOD) {
OutOfDate = OOD;
if (OOD)
NeedsHandleIdentifier = true;
else
RecomputeNeedsHandleIdentifier();
}


bool isModulesImport() const { return IsModulesImport; }


void setModulesImport(bool I) {
IsModulesImport = I;
if (I)
NeedsHandleIdentifier = true;
else
RecomputeNeedsHandleIdentifier();
}


bool isMangledOpenMPVariantName() const { return IsMangledOpenMPVariantName; }


void setMangledOpenMPVariantName(bool I) { IsMangledOpenMPVariantName = I; }










bool isEditorPlaceholder() const {
return getName().startswith("<#") && getName().endswith("#>");
}



ReservedIdentifierStatus isReserved(const LangOptions &LangOpts) const;


bool operator<(const IdentifierInfo &RHS) const {
return getName() < RHS.getName();
}

private:






void RecomputeNeedsHandleIdentifier() {
NeedsHandleIdentifier = isPoisoned() || hasMacroDefinition() ||
isExtensionToken() || isFutureCompatKeyword() ||
isOutOfDate() || isModulesImport();
}
};





class PoisonIdentifierRAIIObject {
IdentifierInfo *const II;
const bool OldValue;

public:
PoisonIdentifierRAIIObject(IdentifierInfo *II, bool NewValue)
: II(II), OldValue(II ? II->isPoisoned() : false) {
if(II)
II->setIsPoisoned(NewValue);
}

~PoisonIdentifierRAIIObject() {
if(II)
II->setIsPoisoned(OldValue);
}
};











class IdentifierIterator {
protected:
IdentifierIterator() = default;

public:
IdentifierIterator(const IdentifierIterator &) = delete;
IdentifierIterator &operator=(const IdentifierIterator &) = delete;

virtual ~IdentifierIterator();






virtual StringRef Next() = 0;
};


class IdentifierInfoLookup {
public:
virtual ~IdentifierInfoLookup();






virtual IdentifierInfo* get(StringRef Name) = 0;











virtual IdentifierIterator *getIdentifiers();
};






class IdentifierTable {


using HashTableTy = llvm::StringMap<IdentifierInfo *, llvm::BumpPtrAllocator>;
HashTableTy HashTable;

IdentifierInfoLookup* ExternalLookup;

public:

explicit IdentifierTable(IdentifierInfoLookup *ExternalLookup = nullptr);



explicit IdentifierTable(const LangOptions &LangOpts,
IdentifierInfoLookup *ExternalLookup = nullptr);


void setExternalIdentifierLookup(IdentifierInfoLookup *IILookup) {
ExternalLookup = IILookup;
}


IdentifierInfoLookup *getExternalIdentifierLookup() const {
return ExternalLookup;
}

llvm::BumpPtrAllocator& getAllocator() {
return HashTable.getAllocator();
}



IdentifierInfo &get(StringRef Name) {
auto &Entry = *HashTable.insert(std::make_pair(Name, nullptr)).first;

IdentifierInfo *&II = Entry.second;
if (II) return *II;


if (ExternalLookup) {
II = ExternalLookup->get(Name);
if (II)
return *II;
}


void *Mem = getAllocator().Allocate<IdentifierInfo>();
II = new (Mem) IdentifierInfo();



II->Entry = &Entry;

return *II;
}

IdentifierInfo &get(StringRef Name, tok::TokenKind TokenCode) {
IdentifierInfo &II = get(Name);
II.TokenID = TokenCode;
assert(II.TokenID == (unsigned) TokenCode && "TokenCode too large");
return II;
}







IdentifierInfo &getOwn(StringRef Name) {
auto &Entry = *HashTable.insert(std::make_pair(Name, nullptr)).first;

IdentifierInfo *&II = Entry.second;
if (II)
return *II;


void *Mem = getAllocator().Allocate<IdentifierInfo>();
II = new (Mem) IdentifierInfo();



II->Entry = &Entry;


if (Name.equals("import"))
II->setModulesImport(true);

return *II;
}

using iterator = HashTableTy::const_iterator;
using const_iterator = HashTableTy::const_iterator;

iterator begin() const { return HashTable.begin(); }
iterator end() const { return HashTable.end(); }
unsigned size() const { return HashTable.size(); }

iterator find(StringRef Name) const { return HashTable.find(Name); }



void PrintStats() const;



void AddKeywords(const LangOptions &LangOpts);
};


















enum ObjCMethodFamily {

OMF_None,





OMF_alloc,
OMF_copy,
OMF_init,
OMF_mutableCopy,
OMF_new,



OMF_autorelease,
OMF_dealloc,
OMF_finalize,
OMF_release,
OMF_retain,
OMF_retainCount,
OMF_self,
OMF_initialize,


OMF_performSelector
};



enum { ObjCMethodFamilyBitWidth = 4 };


enum { InvalidObjCMethodFamily = (1 << ObjCMethodFamilyBitWidth) - 1 };





enum ObjCInstanceTypeFamily {
OIT_None,
OIT_Array,
OIT_Dictionary,
OIT_Singleton,
OIT_Init,
OIT_ReturnsSelf
};

enum ObjCStringFormatFamily {
SFF_None,
SFF_NSString,
SFF_CFString
};








class Selector {
friend class Diagnostic;
friend class SelectorTable;
friend class DeclarationName;

enum IdentifierInfoFlag {


ZeroArg = 0x01,
OneArg = 0x02,
MultiArg = 0x07,
ArgFlags = 0x07
};





uintptr_t InfoPtr = 0;

Selector(IdentifierInfo *II, unsigned nArgs) {
InfoPtr = reinterpret_cast<uintptr_t>(II);
assert((InfoPtr & ArgFlags) == 0 &&"Insufficiently aligned IdentifierInfo");
assert(nArgs < 2 && "nArgs not equal to 0/1");
InfoPtr |= nArgs+1;
}

Selector(MultiKeywordSelector *SI) {
InfoPtr = reinterpret_cast<uintptr_t>(SI);
assert((InfoPtr & ArgFlags) == 0 &&"Insufficiently aligned IdentifierInfo");
InfoPtr |= MultiArg;
}

IdentifierInfo *getAsIdentifierInfo() const {
if (getIdentifierInfoFlag() < MultiArg)
return reinterpret_cast<IdentifierInfo *>(InfoPtr & ~ArgFlags);
return nullptr;
}

MultiKeywordSelector *getMultiKeywordSelector() const {
return reinterpret_cast<MultiKeywordSelector *>(InfoPtr & ~ArgFlags);
}

unsigned getIdentifierInfoFlag() const {
return InfoPtr & ArgFlags;
}

static ObjCMethodFamily getMethodFamilyImpl(Selector sel);

static ObjCStringFormatFamily getStringFormatFamilyImpl(Selector sel);

public:


Selector() = default;
explicit Selector(uintptr_t V) : InfoPtr(V) {}


bool operator==(Selector RHS) const {
return InfoPtr == RHS.InfoPtr;
}
bool operator!=(Selector RHS) const {
return InfoPtr != RHS.InfoPtr;
}

void *getAsOpaquePtr() const {
return reinterpret_cast<void*>(InfoPtr);
}


bool isNull() const { return InfoPtr == 0; }


bool isKeywordSelector() const {
return getIdentifierInfoFlag() != ZeroArg;
}

bool isUnarySelector() const {
return getIdentifierInfoFlag() == ZeroArg;
}


bool isKeywordSelector(ArrayRef<StringRef> Names) const;


bool isUnarySelector(StringRef Name) const;

unsigned getNumArgs() const;














IdentifierInfo *getIdentifierInfoForSlot(unsigned argIndex) const;









StringRef getNameForSlot(unsigned argIndex) const;



std::string getAsString() const;


void print(llvm::raw_ostream &OS) const;

void dump() const;


ObjCMethodFamily getMethodFamily() const {
return getMethodFamilyImpl(*this);
}

ObjCStringFormatFamily getStringFormatFamily() const {
return getStringFormatFamilyImpl(*this);
}

static Selector getEmptyMarker() {
return Selector(uintptr_t(-1));
}

static Selector getTombstoneMarker() {
return Selector(uintptr_t(-2));
}

static ObjCInstanceTypeFamily getInstTypeMethodFamily(Selector sel);
};



class SelectorTable {

void *Impl;

public:
SelectorTable();
SelectorTable(const SelectorTable &) = delete;
SelectorTable &operator=(const SelectorTable &) = delete;
~SelectorTable();





Selector getSelector(unsigned NumArgs, IdentifierInfo **IIV);

Selector getUnarySelector(IdentifierInfo *ID) {
return Selector(ID, 1);
}

Selector getNullarySelector(IdentifierInfo *ID) {
return Selector(ID, 0);
}


size_t getTotalMemory() const;





static SmallString<64> constructSetterName(StringRef Name);





static Selector constructSetterSelector(IdentifierTable &Idents,
SelectorTable &SelTable,
const IdentifierInfo *Name);


static std::string getPropertyNameFromSetterSelector(Selector Sel);
};

namespace detail {









class alignas(IdentifierInfoAlignment) DeclarationNameExtra {
friend class clang::DeclarationName;
friend class clang::DeclarationNameTable;

protected:





enum ExtraKind {
CXXDeductionGuideName,
CXXLiteralOperatorName,
CXXUsingDirective,
ObjCMultiArgSelector
};











unsigned ExtraKindOrNumArgs;

DeclarationNameExtra(ExtraKind Kind) : ExtraKindOrNumArgs(Kind) {}
DeclarationNameExtra(unsigned NumArgs)
: ExtraKindOrNumArgs(ObjCMultiArgSelector + NumArgs) {}


ExtraKind getKind() const {
return static_cast<ExtraKind>(ExtraKindOrNumArgs >
(unsigned)ObjCMultiArgSelector
? (unsigned)ObjCMultiArgSelector
: ExtraKindOrNumArgs);
}



unsigned getNumArgs() const {
assert(ExtraKindOrNumArgs >= (unsigned)ObjCMultiArgSelector &&
"getNumArgs called but this is not an ObjC selector!");
return ExtraKindOrNumArgs - (unsigned)ObjCMultiArgSelector;
}
};

}

}

namespace llvm {



template <>
struct DenseMapInfo<clang::Selector> {
static clang::Selector getEmptyKey() {
return clang::Selector::getEmptyMarker();
}

static clang::Selector getTombstoneKey() {
return clang::Selector::getTombstoneMarker();
}

static unsigned getHashValue(clang::Selector S);

static bool isEqual(clang::Selector LHS, clang::Selector RHS) {
return LHS == RHS;
}
};

template<>
struct PointerLikeTypeTraits<clang::Selector> {
static const void *getAsVoidPointer(clang::Selector P) {
return P.getAsOpaquePtr();
}

static clang::Selector getFromVoidPointer(const void *P) {
return clang::Selector(reinterpret_cast<uintptr_t>(P));
}

static constexpr int NumLowBitsAvailable = 0;
};



template<>
struct PointerLikeTypeTraits<clang::IdentifierInfo*> {
static void *getAsVoidPointer(clang::IdentifierInfo* P) {
return P;
}

static clang::IdentifierInfo *getFromVoidPointer(void *P) {
return static_cast<clang::IdentifierInfo*>(P);
}

static constexpr int NumLowBitsAvailable = 1;
};

template<>
struct PointerLikeTypeTraits<const clang::IdentifierInfo*> {
static const void *getAsVoidPointer(const clang::IdentifierInfo* P) {
return P;
}

static const clang::IdentifierInfo *getFromVoidPointer(const void *P) {
return static_cast<const clang::IdentifierInfo*>(P);
}

static constexpr int NumLowBitsAvailable = 1;
};

}

#endif
