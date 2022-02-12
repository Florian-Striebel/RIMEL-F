











#if !defined(LLD_ELF_SYMBOLS_H)
#define LLD_ELF_SYMBOLS_H

#include "InputFiles.h"
#include "InputSection.h"
#include "lld/Common/LLVM.h"
#include "lld/Common/Strings.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Object/Archive.h"
#include "llvm/Object/ELF.h"

namespace lld {

std::string toString(const elf::Symbol &);




std::string toELFString(const llvm::object::Archive::Symbol &);

namespace elf {
class CommonSymbol;
class Defined;
class InputFile;
class LazyArchive;
class LazyObject;
class SharedSymbol;
class Symbol;
class Undefined;







struct StringRefZ {
StringRefZ(const char *s) : data(s), size(-1) {}
StringRefZ(StringRef s) : data(s.data()), size(s.size()) {}

const char *data;
const uint32_t size;
};


class Symbol {
public:
enum Kind {
PlaceholderKind,
DefinedKind,
CommonKind,
SharedKind,
UndefinedKind,
LazyArchiveKind,
LazyObjectKind,
};

Kind kind() const { return static_cast<Kind>(symbolKind); }


InputFile *file;

protected:
const char *nameData;
mutable uint32_t nameSize;

public:
uint32_t dynsymIndex = 0;
uint32_t gotIndex = -1;
uint32_t pltIndex = -1;

uint32_t globalDynIndex = -1;


uint32_t verdefIndex = -1;


uint16_t versionId;






uint8_t binding;


uint8_t type;
uint8_t stOther;

uint8_t symbolKind;



uint8_t visibility : 2;





uint8_t isUsedInRegularObj : 1;









uint8_t exportDynamic : 1;




uint8_t inDynamicList : 1;




uint8_t canInline : 1;






uint8_t referenced : 1;


uint8_t traced : 1;

inline void replace(const Symbol &newSym);

bool includeInDynsym() const;
uint8_t computeBinding() const;
bool isWeak() const { return binding == llvm::ELF::STB_WEAK; }

bool isUndefined() const { return symbolKind == UndefinedKind; }
bool isCommon() const { return symbolKind == CommonKind; }
bool isDefined() const { return symbolKind == DefinedKind; }
bool isShared() const { return symbolKind == SharedKind; }
bool isPlaceholder() const { return symbolKind == PlaceholderKind; }

bool isLocal() const { return binding == llvm::ELF::STB_LOCAL; }

bool isLazy() const {
return symbolKind == LazyArchiveKind || symbolKind == LazyObjectKind;
}



bool isUndefWeak() const {

return isWeak() && (isUndefined() || isLazy());
}

StringRef getName() const {
if (nameSize == (uint32_t)-1)
nameSize = strlen(nameData);
return {nameData, nameSize};
}

void setName(StringRef s) {
nameData = s.data();
nameSize = s.size();
}

void parseSymbolVersion();





const char *getVersionSuffix() const {
(void)getName();
return nameData + nameSize;
}

bool isInGot() const { return gotIndex != -1U; }
bool isInPlt() const { return pltIndex != -1U; }

uint64_t getVA(int64_t addend = 0) const;

uint64_t getGotOffset() const;
uint64_t getGotVA() const;
uint64_t getGotPltOffset() const;
uint64_t getGotPltVA() const;
uint64_t getPltVA() const;
uint64_t getSize() const;
OutputSection *getOutputSection() const;














void mergeProperties(const Symbol &other);
void resolve(const Symbol &other);




void fetch() const;

static bool isExportDynamic(Kind k, uint8_t visibility) {
if (k == SharedKind)
return visibility == llvm::ELF::STV_DEFAULT;
return config->shared || config->exportDynamic;
}

private:
void resolveUndefined(const Undefined &other);
void resolveCommon(const CommonSymbol &other);
void resolveDefined(const Defined &other);
template <class LazyT> void resolveLazy(const LazyT &other);
void resolveShared(const SharedSymbol &other);

int compare(const Symbol *other) const;

inline size_t getSymbolSize() const;

protected:
Symbol(Kind k, InputFile *file, StringRefZ name, uint8_t binding,
uint8_t stOther, uint8_t type)
: file(file), nameData(name.data), nameSize(name.size), binding(binding),
type(type), stOther(stOther), symbolKind(k), visibility(stOther & 3),
isUsedInRegularObj(!file || file->kind() == InputFile::ObjKind),
exportDynamic(isExportDynamic(k, visibility)), inDynamicList(false),
canInline(false), referenced(false), traced(false), needsPltAddr(false),
isInIplt(false), gotInIgot(false), isPreemptible(false),
used(!config->gcSections), needsTocRestore(false),
scriptDefined(false) {}

public:


uint8_t needsPltAddr : 1;



uint8_t isInIplt : 1;



uint8_t gotInIgot : 1;


uint8_t isPreemptible : 1;





uint8_t used : 1;



uint8_t needsTocRestore : 1;


uint8_t scriptDefined : 1;


uint8_t partition = 1;

bool isSection() const { return type == llvm::ELF::STT_SECTION; }
bool isTls() const { return type == llvm::ELF::STT_TLS; }
bool isFunc() const { return type == llvm::ELF::STT_FUNC; }
bool isGnuIFunc() const { return type == llvm::ELF::STT_GNU_IFUNC; }
bool isObject() const { return type == llvm::ELF::STT_OBJECT; }
bool isFile() const { return type == llvm::ELF::STT_FILE; }
};


class Defined : public Symbol {
public:
Defined(InputFile *file, StringRefZ name, uint8_t binding, uint8_t stOther,
uint8_t type, uint64_t value, uint64_t size, SectionBase *section)
: Symbol(DefinedKind, file, name, binding, stOther, type), value(value),
size(size), section(section) {}

static bool classof(const Symbol *s) { return s->isDefined(); }

uint64_t value;
uint64_t size;
SectionBase *section;
};






















class CommonSymbol : public Symbol {
public:
CommonSymbol(InputFile *file, StringRefZ name, uint8_t binding,
uint8_t stOther, uint8_t type, uint64_t alignment, uint64_t size)
: Symbol(CommonKind, file, name, binding, stOther, type),
alignment(alignment), size(size) {}

static bool classof(const Symbol *s) { return s->isCommon(); }

uint32_t alignment;
uint64_t size;
};

class Undefined : public Symbol {
public:
Undefined(InputFile *file, StringRefZ name, uint8_t binding, uint8_t stOther,
uint8_t type, uint32_t discardedSecIdx = 0)
: Symbol(UndefinedKind, file, name, binding, stOther, type),
discardedSecIdx(discardedSecIdx) {}

static bool classof(const Symbol *s) { return s->kind() == UndefinedKind; }


uint32_t discardedSecIdx;
};

class SharedSymbol : public Symbol {
public:
static bool classof(const Symbol *s) { return s->kind() == SharedKind; }

SharedSymbol(InputFile &file, StringRef name, uint8_t binding,
uint8_t stOther, uint8_t type, uint64_t value, uint64_t size,
uint32_t alignment, uint32_t verdefIndex)
: Symbol(SharedKind, &file, name, binding, stOther, type), value(value),
size(size), alignment(alignment) {
this->verdefIndex = verdefIndex;
















if (this->type == llvm::ELF::STT_GNU_IFUNC)
this->type = llvm::ELF::STT_FUNC;
}

SharedFile &getFile() const { return *cast<SharedFile>(file); }

uint64_t value;
uint64_t size;
uint32_t alignment;
};















class LazyArchive : public Symbol {
public:
LazyArchive(InputFile &file, const llvm::object::Archive::Symbol s)
: Symbol(LazyArchiveKind, &file, s.getName(), llvm::ELF::STB_GLOBAL,
llvm::ELF::STV_DEFAULT, llvm::ELF::STT_NOTYPE),
sym(s) {}

static bool classof(const Symbol *s) { return s->kind() == LazyArchiveKind; }

MemoryBufferRef getMemberBuffer();

const llvm::object::Archive::Symbol sym;
};



class LazyObject : public Symbol {
public:
LazyObject(InputFile &file, StringRef name)
: Symbol(LazyObjectKind, &file, name, llvm::ELF::STB_GLOBAL,
llvm::ELF::STV_DEFAULT, llvm::ELF::STT_NOTYPE) {}

static bool classof(const Symbol *s) { return s->kind() == LazyObjectKind; }
};



struct ElfSym {

static Defined *bss;


static Defined *etext1;
static Defined *etext2;


static Defined *edata1;
static Defined *edata2;


static Defined *end1;
static Defined *end2;




static Defined *globalOffsetTable;


static Defined *mipsGp;
static Defined *mipsGpDisp;
static Defined *mipsLocalGp;


static Defined *relaIpltStart;
static Defined *relaIpltEnd;


static Defined *riscvGlobalPointer;


static Defined *tlsModuleBase;
};




union SymbolUnion {
alignas(Defined) char a[sizeof(Defined)];
alignas(CommonSymbol) char b[sizeof(CommonSymbol)];
alignas(Undefined) char c[sizeof(Undefined)];
alignas(SharedSymbol) char d[sizeof(SharedSymbol)];
alignas(LazyArchive) char e[sizeof(LazyArchive)];
alignas(LazyObject) char f[sizeof(LazyObject)];
};




static_assert(sizeof(SymbolUnion) <= 80, "SymbolUnion too large");

template <typename T> struct AssertSymbol {
static_assert(std::is_trivially_destructible<T>(),
"Symbol types must be trivially destructible");
static_assert(sizeof(T) <= sizeof(SymbolUnion), "SymbolUnion too small");
static_assert(alignof(T) <= alignof(SymbolUnion),
"SymbolUnion not aligned enough");
};

static inline void assertSymbols() {
AssertSymbol<Defined>();
AssertSymbol<CommonSymbol>();
AssertSymbol<Undefined>();
AssertSymbol<SharedSymbol>();
AssertSymbol<LazyArchive>();
AssertSymbol<LazyObject>();
}

void printTraceSymbol(const Symbol *sym);

size_t Symbol::getSymbolSize() const {
switch (kind()) {
case CommonKind:
return sizeof(CommonSymbol);
case DefinedKind:
return sizeof(Defined);
case LazyArchiveKind:
return sizeof(LazyArchive);
case LazyObjectKind:
return sizeof(LazyObject);
case SharedKind:
return sizeof(SharedSymbol);
case UndefinedKind:
return sizeof(Undefined);
case PlaceholderKind:
return sizeof(Symbol);
}
llvm_unreachable("unknown symbol kind");
}




void Symbol::replace(const Symbol &newSym) {
using llvm::ELF::STT_TLS;








if (symbolKind != PlaceholderKind && !newSym.isLazy() &&
(type == STT_TLS) != (newSym.type == STT_TLS) &&
type != llvm::ELF::STT_NOTYPE)
error("TLS attribute mismatch: " + toString(*this) + "\n>>> defined in " +
toString(newSym.file) + "\n>>> defined in " + toString(file));

Symbol old = *this;
memcpy(this, &newSym, newSym.getSymbolSize());



versionId = old.versionId;
visibility = old.visibility;
isUsedInRegularObj = old.isUsedInRegularObj;
exportDynamic = old.exportDynamic;
inDynamicList = old.inDynamicList;
canInline = old.canInline;
referenced = old.referenced;
traced = old.traced;
isPreemptible = old.isPreemptible;
scriptDefined = old.scriptDefined;
partition = old.partition;



if (nameData == old.nameData && nameSize == 0 && old.nameSize != 0)
nameSize = old.nameSize;



if (traced)
printTraceSymbol(this);
}

void maybeWarnUnorderableSymbol(const Symbol *sym);
bool computeIsPreemptible(const Symbol &sym);
void reportBackrefs();



extern llvm::DenseMap<const Symbol *,
std::pair<const InputFile *, const InputFile *>>
backwardReferences;

}
}

#endif
