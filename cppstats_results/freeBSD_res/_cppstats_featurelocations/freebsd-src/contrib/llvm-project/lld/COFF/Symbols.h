







#if !defined(LLD_COFF_SYMBOLS_H)
#define LLD_COFF_SYMBOLS_H

#include "Chunks.h"
#include "Config.h"
#include "lld/Common/LLVM.h"
#include "lld/Common/Memory.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Object/Archive.h"
#include "llvm/Object/COFF.h"
#include <atomic>
#include <memory>
#include <vector>

namespace lld {

std::string toString(coff::Symbol &b);




std::string toCOFFString(const coff::Archive::Symbol &b);

namespace coff {

using llvm::object::Archive;
using llvm::object::COFFSymbolRef;
using llvm::object::coff_import_header;
using llvm::object::coff_symbol_generic;

class ArchiveFile;
class InputFile;
class ObjFile;
class SymbolTable;


class Symbol {
public:
enum Kind {





DefinedRegularKind = 0,
DefinedCommonKind,
DefinedLocalImportKind,
DefinedImportThunkKind,
DefinedImportDataKind,
DefinedAbsoluteKind,
DefinedSyntheticKind,

UndefinedKind,
LazyArchiveKind,
LazyObjectKind,
LazyDLLSymbolKind,

LastDefinedCOFFKind = DefinedCommonKind,
LastDefinedKind = DefinedSyntheticKind,
};

Kind kind() const { return static_cast<Kind>(symbolKind); }


StringRef getName() {







if (nameData == nullptr)
computeName();
return StringRef(nameData, nameSize);
}

void replaceKeepingName(Symbol *other, size_t size);


InputFile *getFile();



bool isLive() const;

bool isLazy() const {
return symbolKind == LazyArchiveKind || symbolKind == LazyObjectKind ||
symbolKind == LazyDLLSymbolKind;
}

private:
void computeName();

protected:
friend SymbolTable;
explicit Symbol(Kind k, StringRef n = "")
: symbolKind(k), isExternal(true), isCOMDAT(false),
writtenToSymtab(false), pendingArchiveLoad(false), isGCRoot(false),
isRuntimePseudoReloc(false), deferUndefined(false), canInline(true),
nameSize(n.size()), nameData(n.empty() ? nullptr : n.data()) {}

const unsigned symbolKind : 8;
unsigned isExternal : 1;

public:

unsigned isCOMDAT : 1;



unsigned writtenToSymtab : 1;


unsigned isUsedInRegularObj : 1;




unsigned pendingArchiveLoad : 1;


unsigned isGCRoot : 1;

unsigned isRuntimePseudoReloc : 1;




unsigned deferUndefined : 1;




unsigned canInline : 1;

protected:

uint32_t nameSize;

const char *nameData;
};



class Defined : public Symbol {
public:
Defined(Kind k, StringRef n) : Symbol(k, n) {}

static bool classof(const Symbol *s) { return s->kind() <= LastDefinedKind; }



uint64_t getRVA();



Chunk *getChunk();
};





class DefinedCOFF : public Defined {
friend Symbol;

public:
DefinedCOFF(Kind k, InputFile *f, StringRef n, const coff_symbol_generic *s)
: Defined(k, n), file(f), sym(s) {}

static bool classof(const Symbol *s) {
return s->kind() <= LastDefinedCOFFKind;
}

InputFile *getFile() { return file; }

COFFSymbolRef getCOFFSymbol();

InputFile *file;

protected:
const coff_symbol_generic *sym;
};


class DefinedRegular : public DefinedCOFF {
public:
DefinedRegular(InputFile *f, StringRef n, bool isCOMDAT,
bool isExternal = false,
const coff_symbol_generic *s = nullptr,
SectionChunk *c = nullptr)
: DefinedCOFF(DefinedRegularKind, f, n, s), data(c ? &c->repl : nullptr) {
this->isExternal = isExternal;
this->isCOMDAT = isCOMDAT;
}

static bool classof(const Symbol *s) {
return s->kind() == DefinedRegularKind;
}

uint64_t getRVA() const { return (*data)->getRVA() + sym->Value; }
SectionChunk *getChunk() const { return *data; }
uint32_t getValue() const { return sym->Value; }

SectionChunk **data;
};

class DefinedCommon : public DefinedCOFF {
public:
DefinedCommon(InputFile *f, StringRef n, uint64_t size,
const coff_symbol_generic *s = nullptr,
CommonChunk *c = nullptr)
: DefinedCOFF(DefinedCommonKind, f, n, s), data(c), size(size) {
this->isExternal = true;
}

static bool classof(const Symbol *s) {
return s->kind() == DefinedCommonKind;
}

uint64_t getRVA() { return data->getRVA(); }
CommonChunk *getChunk() { return data; }

private:
friend SymbolTable;
uint64_t getSize() const { return size; }
CommonChunk *data;
uint64_t size;
};


class DefinedAbsolute : public Defined {
public:
DefinedAbsolute(StringRef n, COFFSymbolRef s)
: Defined(DefinedAbsoluteKind, n), va(s.getValue()) {
isExternal = s.isExternal();
}

DefinedAbsolute(StringRef n, uint64_t v)
: Defined(DefinedAbsoluteKind, n), va(v) {}

static bool classof(const Symbol *s) {
return s->kind() == DefinedAbsoluteKind;
}

uint64_t getRVA() { return va - config->imageBase; }
void setVA(uint64_t v) { va = v; }
uint64_t getVA() const { return va; }




static uint16_t numOutputSections;

private:
uint64_t va;
};



class DefinedSynthetic : public Defined {
public:
explicit DefinedSynthetic(StringRef name, Chunk *c)
: Defined(DefinedSyntheticKind, name), c(c) {}

static bool classof(const Symbol *s) {
return s->kind() == DefinedSyntheticKind;
}



uint32_t getRVA() { return c ? c->getRVA() : 0; }
Chunk *getChunk() { return c; }

private:
Chunk *c;
};






class LazyArchive : public Symbol {
public:
LazyArchive(ArchiveFile *f, const Archive::Symbol s)
: Symbol(LazyArchiveKind, s.getName()), file(f), sym(s) {}

static bool classof(const Symbol *s) { return s->kind() == LazyArchiveKind; }

MemoryBufferRef getMemberBuffer();

ArchiveFile *file;
const Archive::Symbol sym;
};

class LazyObject : public Symbol {
public:
LazyObject(LazyObjFile *f, StringRef n)
: Symbol(LazyObjectKind, n), file(f) {}
static bool classof(const Symbol *s) { return s->kind() == LazyObjectKind; }
LazyObjFile *file;
};


class LazyDLLSymbol : public Symbol {
public:
LazyDLLSymbol(DLLFile *f, DLLFile::Symbol *s, StringRef n)
: Symbol(LazyDLLSymbolKind, n), file(f), sym(s) {}
static bool classof(const Symbol *s) {
return s->kind() == LazyDLLSymbolKind;
}

DLLFile *file;
DLLFile::Symbol *sym;
};


class Undefined : public Symbol {
public:
explicit Undefined(StringRef n) : Symbol(UndefinedKind, n) {}

static bool classof(const Symbol *s) { return s->kind() == UndefinedKind; }





Symbol *weakAlias = nullptr;




Defined *getWeakAlias();
};







class DefinedImportData : public Defined {
public:
DefinedImportData(StringRef n, ImportFile *f)
: Defined(DefinedImportDataKind, n), file(f) {
}

static bool classof(const Symbol *s) {
return s->kind() == DefinedImportDataKind;
}

uint64_t getRVA() { return file->location->getRVA(); }
Chunk *getChunk() { return file->location; }
void setLocation(Chunk *addressTable) { file->location = addressTable; }

StringRef getDLLName() { return file->dllName; }
StringRef getExternalName() { return file->externalName; }
uint16_t getOrdinal() { return file->hdr->OrdinalHint; }

ImportFile *file;






DefinedSynthetic *loadThunkSym = nullptr;
};






class DefinedImportThunk : public Defined {
public:
DefinedImportThunk(StringRef name, DefinedImportData *s, uint16_t machine);

static bool classof(const Symbol *s) {
return s->kind() == DefinedImportThunkKind;
}

uint64_t getRVA() { return data->getRVA(); }
Chunk *getChunk() { return data; }

DefinedImportData *wrappedSym;

private:
Chunk *data;
};






class DefinedLocalImport : public Defined {
public:
DefinedLocalImport(StringRef n, Defined *s)
: Defined(DefinedLocalImportKind, n), data(make<LocalImportChunk>(s)) {}

static bool classof(const Symbol *s) {
return s->kind() == DefinedLocalImportKind;
}

uint64_t getRVA() { return data->getRVA(); }
Chunk *getChunk() { return data; }

private:
LocalImportChunk *data;
};

inline uint64_t Defined::getRVA() {
switch (kind()) {
case DefinedAbsoluteKind:
return cast<DefinedAbsolute>(this)->getRVA();
case DefinedSyntheticKind:
return cast<DefinedSynthetic>(this)->getRVA();
case DefinedImportDataKind:
return cast<DefinedImportData>(this)->getRVA();
case DefinedImportThunkKind:
return cast<DefinedImportThunk>(this)->getRVA();
case DefinedLocalImportKind:
return cast<DefinedLocalImport>(this)->getRVA();
case DefinedCommonKind:
return cast<DefinedCommon>(this)->getRVA();
case DefinedRegularKind:
return cast<DefinedRegular>(this)->getRVA();
case LazyArchiveKind:
case LazyObjectKind:
case LazyDLLSymbolKind:
case UndefinedKind:
llvm_unreachable("Cannot get the address for an undefined symbol.");
}
llvm_unreachable("unknown symbol kind");
}

inline Chunk *Defined::getChunk() {
switch (kind()) {
case DefinedRegularKind:
return cast<DefinedRegular>(this)->getChunk();
case DefinedAbsoluteKind:
return nullptr;
case DefinedSyntheticKind:
return cast<DefinedSynthetic>(this)->getChunk();
case DefinedImportDataKind:
return cast<DefinedImportData>(this)->getChunk();
case DefinedImportThunkKind:
return cast<DefinedImportThunk>(this)->getChunk();
case DefinedLocalImportKind:
return cast<DefinedLocalImport>(this)->getChunk();
case DefinedCommonKind:
return cast<DefinedCommon>(this)->getChunk();
case LazyArchiveKind:
case LazyObjectKind:
case LazyDLLSymbolKind:
case UndefinedKind:
llvm_unreachable("Cannot get the chunk of an undefined symbol.");
}
llvm_unreachable("unknown symbol kind");
}




union SymbolUnion {
alignas(DefinedRegular) char a[sizeof(DefinedRegular)];
alignas(DefinedCommon) char b[sizeof(DefinedCommon)];
alignas(DefinedAbsolute) char c[sizeof(DefinedAbsolute)];
alignas(DefinedSynthetic) char d[sizeof(DefinedSynthetic)];
alignas(LazyArchive) char e[sizeof(LazyArchive)];
alignas(Undefined) char f[sizeof(Undefined)];
alignas(DefinedImportData) char g[sizeof(DefinedImportData)];
alignas(DefinedImportThunk) char h[sizeof(DefinedImportThunk)];
alignas(DefinedLocalImport) char i[sizeof(DefinedLocalImport)];
alignas(LazyObject) char j[sizeof(LazyObject)];
alignas(LazyDLLSymbol) char k[sizeof(LazyDLLSymbol)];
};

template <typename T, typename... ArgT>
void replaceSymbol(Symbol *s, ArgT &&... arg) {
static_assert(std::is_trivially_destructible<T>(),
"Symbol types must be trivially destructible");
static_assert(sizeof(T) <= sizeof(SymbolUnion), "Symbol too small");
static_assert(alignof(T) <= alignof(SymbolUnion),
"SymbolUnion not aligned enough");
assert(static_cast<Symbol *>(static_cast<T *>(nullptr)) == nullptr &&
"Not a Symbol");
bool canInline = s->canInline;
new (s) T(std::forward<ArgT>(arg)...);
s->canInline = canInline;
}
}

}

#endif
