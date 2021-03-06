







#if !defined(LLD_MACHO_SYMBOLS_H)
#define LLD_MACHO_SYMBOLS_H

#include "InputFiles.h"
#include "InputSection.h"
#include "Target.h"
#include "lld/Common/ErrorHandler.h"
#include "lld/Common/Strings.h"
#include "llvm/Object/Archive.h"
#include "llvm/Support/MathExtras.h"

namespace lld {
namespace macho {

class InputSection;
class MachHeaderSection;

struct StringRefZ {
StringRefZ(const char *s) : data(s), size(-1) {}
StringRefZ(StringRef s) : data(s.data()), size(s.size()) {}

const char *data;
const uint32_t size;
};

class Symbol {
public:
enum Kind {
DefinedKind,
UndefinedKind,
CommonKind,
DylibKind,
LazyKind,
};

virtual ~Symbol() {}

Kind kind() const { return symbolKind; }

StringRef getName() const {
if (nameSize == (uint32_t)-1)
nameSize = strlen(nameData);
return {nameData, nameSize};
}

bool isLive() const;

virtual uint64_t getVA() const { return 0; }

virtual bool isWeakDef() const { llvm_unreachable("cannot be weak def"); }




virtual bool isWeakRef() const { llvm_unreachable("cannot be a weak ref"); }

virtual bool isTlv() const { llvm_unreachable("cannot be TLV"); }


bool isInGot() const { return gotIndex != UINT32_MAX; }


bool isInStubs() const { return stubsIndex != UINT32_MAX; }

uint64_t getStubVA() const;
uint64_t getGotVA() const;
uint64_t getTlvVA() const;
uint64_t resolveBranchVA() const {
assert(isa<Defined>(this) || isa<DylibSymbol>(this));
return isInStubs() ? getStubVA() : getVA();
}
uint64_t resolveGotVA() const { return isInGot() ? getGotVA() : getVA(); }
uint64_t resolveTlvVA() const { return isInGot() ? getTlvVA() : getVA(); }




uint32_t gotIndex = UINT32_MAX;

uint32_t stubsIndex = UINT32_MAX;

uint32_t symtabIndex = UINT32_MAX;

InputFile *getFile() const { return file; }

protected:
Symbol(Kind k, StringRefZ name, InputFile *file)
: symbolKind(k), nameData(name.data), nameSize(name.size), file(file),
isUsedInRegularObj(!file || isa<ObjFile>(file)),
used(!config->deadStrip) {}

Kind symbolKind;
const char *nameData;
mutable uint32_t nameSize;
InputFile *file;

public:

bool isUsedInRegularObj : 1;


bool used : 1;
};

class Defined : public Symbol {
public:
Defined(StringRefZ name, InputFile *file, InputSection *isec, uint64_t value,
uint64_t size, bool isWeakDef, bool isExternal, bool isPrivateExtern,
bool isThumb, bool isReferencedDynamically, bool noDeadStrip)
: Symbol(DefinedKind, name, file), isec(isec), value(value), size(size),
overridesWeakDef(false), privateExtern(isPrivateExtern),
includeInSymtab(true), thumb(isThumb),
referencedDynamically(isReferencedDynamically),
noDeadStrip(noDeadStrip), weakDef(isWeakDef), external(isExternal) {
if (auto concatIsec = dyn_cast_or_null<ConcatInputSection>(isec))
concatIsec->numRefs++;
}

bool isWeakDef() const override { return weakDef; }
bool isExternalWeakDef() const {
return isWeakDef() && isExternal() && !privateExtern;
}
bool isTlv() const override {
return !isAbsolute() && isThreadLocalVariables(isec->getFlags());
}

bool isExternal() const { return external; }
bool isAbsolute() const { return isec == nullptr; }

uint64_t getVA() const override;

static bool classof(const Symbol *s) { return s->kind() == DefinedKind; }

InputSection *isec;


uint64_t value;

uint64_t size;

bool overridesWeakDef : 1;

bool privateExtern : 1;

bool includeInSymtab : 1;

bool thumb : 1;






bool referencedDynamically : 1;




bool noDeadStrip : 1;

private:
const bool weakDef : 1;
const bool external : 1;
};






enum class RefState : uint8_t { Unreferenced = 0, Weak = 1, Strong = 2 };

class Undefined : public Symbol {
public:
Undefined(StringRefZ name, InputFile *file, RefState refState)
: Symbol(UndefinedKind, name, file), refState(refState) {
assert(refState != RefState::Unreferenced);
}

bool isWeakRef() const override { return refState == RefState::Weak; }

static bool classof(const Symbol *s) { return s->kind() == UndefinedKind; }

RefState refState : 2;
};
















class CommonSymbol : public Symbol {
public:
CommonSymbol(StringRefZ name, InputFile *file, uint64_t size, uint32_t align,
bool isPrivateExtern)
: Symbol(CommonKind, name, file), size(size),
align(align != 1 ? align : llvm::PowerOf2Ceil(size)),
privateExtern(isPrivateExtern) {

}

static bool classof(const Symbol *s) { return s->kind() == CommonKind; }

const uint64_t size;
const uint32_t align;
const bool privateExtern;
};

class DylibSymbol : public Symbol {
public:
DylibSymbol(DylibFile *file, StringRefZ name, bool isWeakDef,
RefState refState, bool isTlv)
: Symbol(DylibKind, name, file), refState(refState), weakDef(isWeakDef),
tlv(isTlv) {
if (file && refState > RefState::Unreferenced)
file->numReferencedSymbols++;
}

uint64_t getVA() const override;
bool isWeakDef() const override { return weakDef; }
bool isWeakRef() const override { return refState == RefState::Weak; }
bool isReferenced() const { return refState != RefState::Unreferenced; }
bool isTlv() const override { return tlv; }
bool isDynamicLookup() const { return file == nullptr; }
bool hasStubsHelper() const { return stubsHelperIndex != UINT32_MAX; }

DylibFile *getFile() const {
assert(!isDynamicLookup());
return cast<DylibFile>(file);
}

static bool classof(const Symbol *s) { return s->kind() == DylibKind; }

uint32_t stubsHelperIndex = UINT32_MAX;
uint32_t lazyBindOffset = UINT32_MAX;

RefState getRefState() const { return refState; }

void reference(RefState newState) {
assert(newState > RefState::Unreferenced);
if (refState == RefState::Unreferenced && file)
getFile()->numReferencedSymbols++;
refState = std::max(refState, newState);
}

void unreference() {

if (refState > RefState::Unreferenced && file) {
assert(getFile()->numReferencedSymbols > 0);
getFile()->numReferencedSymbols--;
}
}

private:
RefState refState : 2;
const bool weakDef : 1;
const bool tlv : 1;
};

class LazySymbol : public Symbol {
public:
LazySymbol(ArchiveFile *file, const llvm::object::Archive::Symbol &sym)
: Symbol(LazyKind, sym.getName(), file), sym(sym) {}

ArchiveFile *getFile() const { return cast<ArchiveFile>(file); }
void fetchArchiveMember();

static bool classof(const Symbol *s) { return s->kind() == LazyKind; }

private:
const llvm::object::Archive::Symbol sym;
};

union SymbolUnion {
alignas(Defined) char a[sizeof(Defined)];
alignas(Undefined) char b[sizeof(Undefined)];
alignas(CommonSymbol) char c[sizeof(CommonSymbol)];
alignas(DylibSymbol) char d[sizeof(DylibSymbol)];
alignas(LazySymbol) char e[sizeof(LazySymbol)];
};

template <typename T, typename... ArgT>
T *replaceSymbol(Symbol *s, ArgT &&...arg) {
static_assert(sizeof(T) <= sizeof(SymbolUnion), "SymbolUnion too small");
static_assert(alignof(T) <= alignof(SymbolUnion),
"SymbolUnion not aligned enough");
assert(static_cast<Symbol *>(static_cast<T *>(nullptr)) == nullptr &&
"Not a Symbol");

bool isUsedInRegularObj = s->isUsedInRegularObj;
bool used = s->used;
T *sym = new (s) T(std::forward<ArgT>(arg)...);
sym->isUsedInRegularObj |= isUsedInRegularObj;
sym->used |= used;
return sym;
}

}

std::string toString(const macho::Symbol &);
std::string toMachOString(const llvm::object::Archive::Symbol &);

}

#endif
