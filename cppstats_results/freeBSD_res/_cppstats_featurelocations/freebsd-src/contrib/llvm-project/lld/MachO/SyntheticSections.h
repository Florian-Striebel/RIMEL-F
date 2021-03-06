







#if !defined(LLD_MACHO_SYNTHETIC_SECTIONS_H)
#define LLD_MACHO_SYNTHETIC_SECTIONS_H

#include "Config.h"
#include "ExportTrie.h"
#include "InputSection.h"
#include "OutputSection.h"
#include "OutputSegment.h"
#include "Target.h"
#include "Writer.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/MC/StringTableBuilder.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"

#include <unordered_map>

namespace llvm {
class DWARFUnit;
}

namespace lld {
namespace macho {

class Defined;
class DylibSymbol;
class LoadCommand;
class ObjFile;
class UnwindInfoSection;

class SyntheticSection : public OutputSection {
public:
SyntheticSection(const char *segname, const char *name);
virtual ~SyntheticSection() = default;

static bool classof(const OutputSection *sec) {
return sec->kind() == SyntheticKind;
}

StringRef segname;


InputSection *isec;
};


class LinkEditSection : public SyntheticSection {
public:
LinkEditSection(const char *segname, const char *name)
: SyntheticSection(segname, name) {
align = target->wordSize;
}

virtual void finalizeContents() {}




bool isHidden() const override final { return true; }

virtual uint64_t getRawSize() const = 0;







uint64_t getSize() const override final {
return llvm::alignTo(getRawSize(), align);
}
};


class MachHeaderSection final : public SyntheticSection {
public:
MachHeaderSection();
bool isHidden() const override { return true; }
uint64_t getSize() const override;
void writeTo(uint8_t *buf) const override;

void addLoadCommand(LoadCommand *);

protected:
std::vector<LoadCommand *> loadCommands;
uint32_t sizeOfCmds = 0;
};



class PageZeroSection final : public SyntheticSection {
public:
PageZeroSection();
bool isHidden() const override { return true; }
uint64_t getSize() const override { return target->pageZeroSize; }
uint64_t getFileSize() const override { return 0; }
void writeTo(uint8_t *buf) const override {}
};





class NonLazyPointerSectionBase : public SyntheticSection {
public:
NonLazyPointerSectionBase(const char *segname, const char *name);
const llvm::SetVector<const Symbol *> &getEntries() const { return entries; }
bool isNeeded() const override { return !entries.empty(); }
uint64_t getSize() const override {
return entries.size() * target->wordSize;
}
void writeTo(uint8_t *buf) const override;
void addEntry(Symbol *sym);
uint64_t getVA(uint32_t gotIndex) const {
return addr + gotIndex * target->wordSize;
}

private:
llvm::SetVector<const Symbol *> entries;
};

class GotSection final : public NonLazyPointerSectionBase {
public:
GotSection();
};

class TlvPointerSection final : public NonLazyPointerSectionBase {
public:
TlvPointerSection();
};

struct Location {
const InputSection *isec;
uint64_t offset;

Location(const InputSection *isec, uint64_t offset)
: isec(isec), offset(offset) {}
uint64_t getVA() const { return isec->getVA(offset); }
};




class RebaseSection final : public LinkEditSection {
public:
RebaseSection();
void finalizeContents() override;
uint64_t getRawSize() const override { return contents.size(); }
bool isNeeded() const override { return !locations.empty(); }
void writeTo(uint8_t *buf) const override;

void addEntry(const InputSection *isec, uint64_t offset) {
if (config->isPic)
locations.push_back({isec, offset});
}

private:
std::vector<Location> locations;
SmallVector<char, 128> contents;
};

struct BindingEntry {
int64_t addend;
Location target;
BindingEntry(int64_t addend, Location target)
: addend(addend), target(std::move(target)) {}
};

template <class Sym>
using BindingsMap = llvm::DenseMap<Sym, std::vector<BindingEntry>>;


class BindingSection final : public LinkEditSection {
public:
BindingSection();
void finalizeContents() override;
uint64_t getRawSize() const override { return contents.size(); }
bool isNeeded() const override { return !bindingsMap.empty(); }
void writeTo(uint8_t *buf) const override;

void addEntry(const DylibSymbol *dysym, const InputSection *isec,
uint64_t offset, int64_t addend = 0) {
bindingsMap[dysym].emplace_back(addend, Location(isec, offset));
}

private:
BindingsMap<const DylibSymbol *> bindingsMap;
SmallVector<char, 128> contents;
};











class WeakBindingSection final : public LinkEditSection {
public:
WeakBindingSection();
void finalizeContents() override;
uint64_t getRawSize() const override { return contents.size(); }
bool isNeeded() const override {
return !bindingsMap.empty() || !definitions.empty();
}

void writeTo(uint8_t *buf) const override;

void addEntry(const Symbol *symbol, const InputSection *isec, uint64_t offset,
int64_t addend = 0) {
bindingsMap[symbol].emplace_back(addend, Location(isec, offset));
}

bool hasEntry() const { return !bindingsMap.empty(); }

void addNonWeakDefinition(const Defined *defined) {
definitions.emplace_back(defined);
}

bool hasNonWeakDefinition() const { return !definitions.empty(); }

private:
BindingsMap<const Symbol *> bindingsMap;
std::vector<const Defined *> definitions;
SmallVector<char, 128> contents;
};
































class StubsSection final : public SyntheticSection {
public:
StubsSection();
uint64_t getSize() const override;
bool isNeeded() const override { return !entries.empty(); }
void finalize() override;
void writeTo(uint8_t *buf) const override;
const llvm::SetVector<Symbol *> &getEntries() const { return entries; }


bool addEntry(Symbol *);
uint64_t getVA(uint32_t stubsIndex) const {
assert(isFinal || target->usesThunks());



return isFinal ? addr + stubsIndex * target->stubSize
: TargetInfo::outOfRangeVA;
}

bool isFinal = false;

private:
llvm::SetVector<Symbol *> entries;
};

class StubHelperSection final : public SyntheticSection {
public:
StubHelperSection();
uint64_t getSize() const override;
bool isNeeded() const override;
void writeTo(uint8_t *buf) const override;

void setup();

DylibSymbol *stubBinder = nullptr;
Defined *dyldPrivate = nullptr;
};



class LazyPointerSection final : public SyntheticSection {
public:
LazyPointerSection();
uint64_t getSize() const override;
bool isNeeded() const override;
void writeTo(uint8_t *buf) const override;
};

class LazyBindingSection final : public LinkEditSection {
public:
LazyBindingSection();
void finalizeContents() override;
uint64_t getRawSize() const override { return contents.size(); }
bool isNeeded() const override { return !entries.empty(); }
void writeTo(uint8_t *buf) const override;


void addEntry(DylibSymbol *dysym);
const llvm::SetVector<DylibSymbol *> &getEntries() const { return entries; }

private:
uint32_t encode(const DylibSymbol &);

llvm::SetVector<DylibSymbol *> entries;
SmallVector<char, 128> contents;
llvm::raw_svector_ostream os{contents};
};


class ExportSection final : public LinkEditSection {
public:
ExportSection();
void finalizeContents() override;
uint64_t getRawSize() const override { return size; }
void writeTo(uint8_t *buf) const override;

bool hasWeakSymbol = false;

private:
TrieBuilder trieBuilder;
size_t size = 0;
};



class DataInCodeSection final : public LinkEditSection {
public:
DataInCodeSection();
void finalizeContents() override;
uint64_t getRawSize() const override {
return sizeof(llvm::MachO::data_in_code_entry) * entries.size();
}
void writeTo(uint8_t *buf) const override;

private:
std::vector<llvm::MachO::data_in_code_entry> entries;
};


class FunctionStartsSection final : public LinkEditSection {
public:
FunctionStartsSection();
void finalizeContents() override;
uint64_t getRawSize() const override { return contents.size(); }
void writeTo(uint8_t *buf) const override;

private:
SmallVector<char, 128> contents;
};


class StringTableSection final : public LinkEditSection {
public:
StringTableSection();

uint32_t addString(StringRef);
uint64_t getRawSize() const override { return size; }
void writeTo(uint8_t *buf) const override;

static constexpr size_t emptyStringIndex = 1;

private:



std::vector<StringRef> strings{" "};
size_t size = 2;
};

struct SymtabEntry {
Symbol *sym;
size_t strx;
};

struct StabsEntry {
uint8_t type = 0;
uint32_t strx = StringTableSection::emptyStringIndex;
uint8_t sect = 0;
uint16_t desc = 0;
uint64_t value = 0;

StabsEntry() = default;
explicit StabsEntry(uint8_t type) : type(type) {}
};





class SymtabSection : public LinkEditSection {
public:
void finalizeContents() override;
uint32_t getNumSymbols() const;
uint32_t getNumLocalSymbols() const {
return stabs.size() + localSymbols.size();
}
uint32_t getNumExternalSymbols() const { return externalSymbols.size(); }
uint32_t getNumUndefinedSymbols() const { return undefinedSymbols.size(); }

private:
void emitBeginSourceStab(llvm::DWARFUnit *compileUnit);
void emitEndSourceStab();
void emitObjectFileStab(ObjFile *);
void emitEndFunStab(Defined *);
void emitStabs();

protected:
SymtabSection(StringTableSection &);

StringTableSection &stringTableSection;


std::vector<StabsEntry> stabs;
std::vector<SymtabEntry> localSymbols;
std::vector<SymtabEntry> externalSymbols;
std::vector<SymtabEntry> undefinedSymbols;
};

template <class LP> SymtabSection *makeSymtabSection(StringTableSection &);











class IndirectSymtabSection final : public LinkEditSection {
public:
IndirectSymtabSection();
void finalizeContents() override;
uint32_t getNumSymbols() const;
uint64_t getRawSize() const override {
return getNumSymbols() * sizeof(uint32_t);
}
bool isNeeded() const override;
void writeTo(uint8_t *buf) const override;
};


class CodeSignatureSection final : public LinkEditSection {
public:
static constexpr uint8_t blockSizeShift = 12;
static constexpr size_t blockSize = (1 << blockSizeShift);
static constexpr size_t hashSize = 256 / 8;
static constexpr size_t blobHeadersSize = llvm::alignTo<8>(
sizeof(llvm::MachO::CS_SuperBlob) + sizeof(llvm::MachO::CS_BlobIndex));
static constexpr uint32_t fixedHeadersSize =
blobHeadersSize + sizeof(llvm::MachO::CS_CodeDirectory);

uint32_t fileNamePad = 0;
uint32_t allHeadersSize = 0;
StringRef fileName;

CodeSignatureSection();
uint64_t getRawSize() const override;
bool isNeeded() const override { return true; }
void writeTo(uint8_t *buf) const override;
uint32_t getBlockCount() const;
void writeHashes(uint8_t *buf) const;
};

class BitcodeBundleSection final : public SyntheticSection {
public:
BitcodeBundleSection();
uint64_t getSize() const override { return xarSize; }
void finalize() override;
void writeTo(uint8_t *buf) const override;

private:
llvm::SmallString<261> xarPath;
uint64_t xarSize;
};

class CStringSection : public SyntheticSection {
public:
CStringSection();
void addInput(CStringInputSection *);
uint64_t getSize() const override { return size; }
virtual void finalizeContents();
bool isNeeded() const override { return !inputs.empty(); }
void writeTo(uint8_t *buf) const override;

std::vector<CStringInputSection *> inputs;

private:
uint64_t size;
};

class DeduplicatedCStringSection final : public CStringSection {
public:
DeduplicatedCStringSection();
uint64_t getSize() const override { return builder.getSize(); }
void finalizeContents() override;
void writeTo(uint8_t *buf) const override { builder.write(buf); }

private:
llvm::StringTableBuilder builder;
};





class WordLiteralSection final : public SyntheticSection {
public:
using UInt128 = std::pair<uint64_t, uint64_t>;


static_assert(sizeof(UInt128) == 16, "");

WordLiteralSection();
void addInput(WordLiteralInputSection *);
void finalizeContents();
void writeTo(uint8_t *buf) const override;

uint64_t getSize() const override {
return literal16Map.size() * 16 + literal8Map.size() * 8 +
literal4Map.size() * 4;
}

bool isNeeded() const override {
return !literal16Map.empty() || !literal4Map.empty() ||
!literal8Map.empty();
}

uint64_t getLiteral16Offset(const uint8_t *buf) const {
return literal16Map.at(*reinterpret_cast<const UInt128 *>(buf)) * 16;
}

uint64_t getLiteral8Offset(const uint8_t *buf) const {
return literal16Map.size() * 16 +
literal8Map.at(*reinterpret_cast<const uint64_t *>(buf)) * 8;
}

uint64_t getLiteral4Offset(const uint8_t *buf) const {
return literal16Map.size() * 16 + literal8Map.size() * 8 +
literal4Map.at(*reinterpret_cast<const uint32_t *>(buf)) * 4;
}

private:
std::vector<WordLiteralInputSection *> inputs;

template <class T> struct Hasher {
llvm::hash_code operator()(T v) const { return llvm::hash_value(v); }
};



std::unordered_map<UInt128, uint64_t, Hasher<UInt128>> literal16Map;
std::unordered_map<uint64_t, uint64_t> literal8Map;
std::unordered_map<uint32_t, uint64_t> literal4Map;
};

struct InStruct {
MachHeaderSection *header = nullptr;
CStringSection *cStringSection = nullptr;
WordLiteralSection *wordLiteralSection = nullptr;
RebaseSection *rebase = nullptr;
BindingSection *binding = nullptr;
WeakBindingSection *weakBinding = nullptr;
LazyBindingSection *lazyBinding = nullptr;
ExportSection *exports = nullptr;
GotSection *got = nullptr;
TlvPointerSection *tlvPointers = nullptr;
LazyPointerSection *lazyPointers = nullptr;
StubsSection *stubs = nullptr;
StubHelperSection *stubHelper = nullptr;
UnwindInfoSection *unwindInfo = nullptr;
ConcatInputSection *imageLoaderCache = nullptr;
};

extern InStruct in;
extern std::vector<SyntheticSection *> syntheticSections;

void createSyntheticSymbols();

}
}

#endif
