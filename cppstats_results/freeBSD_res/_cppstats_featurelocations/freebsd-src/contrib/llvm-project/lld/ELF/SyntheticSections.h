


















#if !defined(LLD_ELF_SYNTHETIC_SECTIONS_H)
#define LLD_ELF_SYNTHETIC_SECTIONS_H

#include "DWARF.h"
#include "EhFrame.h"
#include "InputSection.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/MC/StringTableBuilder.h"
#include "llvm/Support/Endian.h"
#include <functional>

namespace lld {
namespace elf {
class Defined;
struct PhdrEntry;
class SymbolTableBaseSection;
class VersionNeedBaseSection;

class SyntheticSection : public InputSection {
public:
SyntheticSection(uint64_t flags, uint32_t type, uint32_t alignment,
StringRef name)
: InputSection(nullptr, flags, type, alignment, {}, name,
InputSectionBase::Synthetic) {
markLive();
}

virtual ~SyntheticSection() = default;
virtual void writeTo(uint8_t *buf) = 0;
virtual size_t getSize() const = 0;
virtual void finalizeContents() {}


virtual bool updateAllocSize() { return false; }
virtual bool isNeeded() const { return true; }

static bool classof(const SectionBase *d) {
return d->kind() == InputSectionBase::Synthetic;
}
};

struct CieRecord {
EhSectionPiece *cie = nullptr;
std::vector<EhSectionPiece *> fdes;
};


class EhFrameSection final : public SyntheticSection {
public:
EhFrameSection();
void writeTo(uint8_t *buf) override;
void finalizeContents() override;
bool isNeeded() const override { return !sections.empty(); }
size_t getSize() const override { return size; }

static bool classof(const SectionBase *d) {
return SyntheticSection::classof(d) && d->name == ".eh_frame";
}

void addSection(EhInputSection *sec);

std::vector<EhInputSection *> sections;
size_t numFdes = 0;

struct FdeData {
uint32_t pcRel;
uint32_t fdeVARel;
};

std::vector<FdeData> getFdeData() const;
ArrayRef<CieRecord *> getCieRecords() const { return cieRecords; }
template <class ELFT>
void iterateFDEWithLSDA(llvm::function_ref<void(InputSection &)> fn);

private:


llvm::DenseMap<size_t, CieRecord *> offsetToCie;

uint64_t size = 0;

template <class ELFT, class RelTy>
void addRecords(EhInputSection *s, llvm::ArrayRef<RelTy> rels);
template <class ELFT> void addSectionAux(EhInputSection *s);
template <class ELFT, class RelTy>
void iterateFDEWithLSDAAux(EhInputSection &sec, ArrayRef<RelTy> rels,
llvm::DenseSet<size_t> &ciesWithLSDA,
llvm::function_ref<void(InputSection &)> fn);

template <class ELFT, class RelTy>
CieRecord *addCie(EhSectionPiece &piece, ArrayRef<RelTy> rels);

template <class ELFT, class RelTy>
Defined *isFdeLive(EhSectionPiece &piece, ArrayRef<RelTy> rels);

uint64_t getFdePc(uint8_t *buf, size_t off, uint8_t enc) const;

std::vector<CieRecord *> cieRecords;


llvm::DenseMap<std::pair<ArrayRef<uint8_t>, Symbol *>, CieRecord *> cieMap;
};

class GotSection : public SyntheticSection {
public:
GotSection();
size_t getSize() const override { return size; }
void finalizeContents() override;
bool isNeeded() const override;
void writeTo(uint8_t *buf) override;

void addEntry(Symbol &sym);
bool addDynTlsEntry(Symbol &sym);
bool addTlsIndex();
uint64_t getGlobalDynAddr(const Symbol &b) const;
uint64_t getGlobalDynOffset(const Symbol &b) const;

uint64_t getTlsIndexVA() { return this->getVA() + tlsIndexOff; }
uint32_t getTlsIndexOff() const { return tlsIndexOff; }



bool hasGotOffRel = false;

protected:
size_t numEntries = 0;
uint32_t tlsIndexOff = -1;
uint64_t size = 0;
};


class GnuStackSection : public SyntheticSection {
public:
GnuStackSection()
: SyntheticSection(0, llvm::ELF::SHT_PROGBITS, 1, ".note.GNU-stack") {}
void writeTo(uint8_t *buf) override {}
size_t getSize() const override { return 0; }
};

class GnuPropertySection : public SyntheticSection {
public:
GnuPropertySection();
void writeTo(uint8_t *buf) override;
size_t getSize() const override;
};


class BuildIdSection : public SyntheticSection {

static const unsigned headerSize = 16;

public:
const size_t hashSize;
BuildIdSection();
void writeTo(uint8_t *buf) override;
size_t getSize() const override { return headerSize + hashSize; }
void writeBuildId(llvm::ArrayRef<uint8_t> buf);

private:
uint8_t *hashBuf;
};





class BssSection final : public SyntheticSection {
public:
BssSection(StringRef name, uint64_t size, uint32_t alignment);
void writeTo(uint8_t *) override {
llvm_unreachable("unexpected writeTo() call for SHT_NOBITS section");
}
bool isNeeded() const override { return size != 0; }
size_t getSize() const override { return size; }

static bool classof(const SectionBase *s) { return s->bss; }
uint64_t size;
};

class MipsGotSection final : public SyntheticSection {
public:
MipsGotSection();
void writeTo(uint8_t *buf) override;
size_t getSize() const override { return size; }
bool updateAllocSize() override;
void finalizeContents() override;
bool isNeeded() const override;



void build();

void addEntry(InputFile &file, Symbol &sym, int64_t addend, RelExpr expr);
void addDynTlsEntry(InputFile &file, Symbol &sym);
void addTlsIndex(InputFile &file);

uint64_t getPageEntryOffset(const InputFile *f, const Symbol &s,
int64_t addend) const;
uint64_t getSymEntryOffset(const InputFile *f, const Symbol &s,
int64_t addend) const;
uint64_t getGlobalDynOffset(const InputFile *f, const Symbol &s) const;
uint64_t getTlsIndexOffset(const InputFile *f) const;





const Symbol *getFirstGlobalEntry() const;



unsigned getLocalEntriesNum() const;


uint64_t getGp(const InputFile *f = nullptr) const;

private:























































































static const unsigned headerEntriesNum = 2;

uint64_t size = 0;


using GotEntry = std::pair<Symbol *, int64_t>;

struct FileGot {
InputFile *file = nullptr;
size_t startIndex = 0;

struct PageBlock {
size_t firstIndex;
size_t count;
PageBlock() : firstIndex(0), count(0) {}
};




llvm::SmallMapVector<const OutputSection *, PageBlock, 16> pagesMap;

llvm::MapVector<GotEntry, size_t> local16;
llvm::MapVector<GotEntry, size_t> local32;
llvm::MapVector<Symbol *, size_t> global;
llvm::MapVector<Symbol *, size_t> relocs;
llvm::MapVector<Symbol *, size_t> tls;

llvm::MapVector<Symbol *, size_t> dynTlsSymbols;


size_t getEntriesNum() const;

size_t getPageEntriesNum() const;

size_t getIndexedEntriesNum() const;
};




std::vector<FileGot> gots;


FileGot &getGot(InputFile &f);




bool tryMergeGots(FileGot & dst, FileGot & src, bool isPrimary);
};

class GotPltSection final : public SyntheticSection {
public:
GotPltSection();
void addEntry(Symbol &sym);
size_t getSize() const override;
void writeTo(uint8_t *buf) override;
bool isNeeded() const override;



bool hasGotPltOffRel = false;

private:
std::vector<const Symbol *> entries;
};





class IgotPltSection final : public SyntheticSection {
public:
IgotPltSection();
void addEntry(Symbol &sym);
size_t getSize() const override;
void writeTo(uint8_t *buf) override;
bool isNeeded() const override { return !entries.empty(); }

private:
std::vector<const Symbol *> entries;
};

class StringTableSection final : public SyntheticSection {
public:
StringTableSection(StringRef name, bool dynamic);
unsigned addString(StringRef s, bool hashIt = true);
void writeTo(uint8_t *buf) override;
size_t getSize() const override { return size; }
bool isDynamic() const { return dynamic; }

private:
const bool dynamic;

uint64_t size = 0;

llvm::DenseMap<StringRef, unsigned> stringMap;
std::vector<StringRef> strings;
};

class DynamicReloc {
public:
enum Kind {


AddendOnly,



AddendOnlyWithTargetVA,


AgainstSymbol,




AgainstSymbolWithTargetVA,


MipsMultiGotPage,
};

DynamicReloc(RelType type, const InputSectionBase *inputSec,
uint64_t offsetInSec, Kind kind, Symbol &sym, int64_t addend,
RelExpr expr)
: type(type), sym(&sym), inputSec(inputSec), offsetInSec(offsetInSec),
kind(kind), expr(expr), addend(addend) {}

DynamicReloc(RelType type, const InputSectionBase *inputSec,
uint64_t offsetInSec, int64_t addend = 0)
: type(type), sym(nullptr), inputSec(inputSec), offsetInSec(offsetInSec),
kind(AddendOnly), expr(R_ADDEND), addend(addend) {}


DynamicReloc(RelType type, const InputSectionBase *inputSec,
uint64_t offsetInSec, const OutputSection *outputSec,
int64_t addend)
: type(type), sym(nullptr), inputSec(inputSec), offsetInSec(offsetInSec),
kind(MipsMultiGotPage), expr(R_ADDEND), addend(addend),
outputSec(outputSec) {}

uint64_t getOffset() const;
uint32_t getSymIndex(SymbolTableBaseSection *symTab) const;
bool needsDynSymIndex() const {
return kind == AgainstSymbol || kind == AgainstSymbolWithTargetVA;
}




int64_t computeAddend() const;

RelType type;
Symbol *sym;
const InputSectionBase *inputSec;
uint64_t offsetInSec;

private:
Kind kind;


RelExpr expr;
int64_t addend;
const OutputSection *outputSec = nullptr;
};

template <class ELFT> class DynamicSection final : public SyntheticSection {
LLVM_ELF_IMPORT_TYPES_ELFT(ELFT)


std::vector<std::pair<int32_t, std::function<uint64_t()>>> entries;

public:
DynamicSection();
void finalizeContents() override;
void writeTo(uint8_t *buf) override;
size_t getSize() const override { return size; }

private:
void add(int32_t tag, std::function<uint64_t()> fn);
void addInt(int32_t tag, uint64_t val);
void addInSec(int32_t tag, InputSection *sec);
void addInSecRelative(int32_t tag, InputSection *sec);
void addOutSec(int32_t tag, OutputSection *sec);
void addSize(int32_t tag, OutputSection *sec);
void addSym(int32_t tag, Symbol *sym);

uint64_t size = 0;
};

class RelocationBaseSection : public SyntheticSection {
public:
RelocationBaseSection(StringRef name, uint32_t type, int32_t dynamicTag,
int32_t sizeDynamicTag);



void addReloc(const DynamicReloc &reloc);

void addSymbolReloc(RelType dynType, InputSectionBase *isec,
uint64_t offsetInSec, Symbol &sym, int64_t addend = 0,
llvm::Optional<RelType> addendRelType = llvm::None);


void addRelativeReloc(RelType dynType, InputSectionBase *isec,
uint64_t offsetInSec, Symbol &sym, int64_t addend,
RelType addendRelType, RelExpr expr);


void addAddendOnlyRelocIfNonPreemptible(RelType dynType,
InputSectionBase *isec,
uint64_t offsetInSec, Symbol &sym,
RelType addendRelType);
void addReloc(DynamicReloc::Kind kind, RelType dynType,
InputSectionBase *inputSec, uint64_t offsetInSec, Symbol &sym,
int64_t addend, RelExpr expr, RelType addendRelType);
bool isNeeded() const override { return !relocs.empty(); }
size_t getSize() const override { return relocs.size() * this->entsize; }
size_t getRelativeRelocCount() const { return numRelativeRelocs; }
void finalizeContents() override;
static bool classof(const SectionBase *d) {
return SyntheticSection::classof(d) &&
(d->type == llvm::ELF::SHT_RELA || d->type == llvm::ELF::SHT_REL ||
d->type == llvm::ELF::SHT_RELR);
}
int32_t dynamicTag, sizeDynamicTag;
std::vector<DynamicReloc> relocs;

protected:
size_t numRelativeRelocs = 0;
};

template <class ELFT>
class RelocationSection final : public RelocationBaseSection {
using Elf_Rel = typename ELFT::Rel;
using Elf_Rela = typename ELFT::Rela;

public:
RelocationSection(StringRef name, bool sort);
void writeTo(uint8_t *buf) override;

private:
bool sort;
};

template <class ELFT>
class AndroidPackedRelocationSection final : public RelocationBaseSection {
using Elf_Rel = typename ELFT::Rel;
using Elf_Rela = typename ELFT::Rela;

public:
AndroidPackedRelocationSection(StringRef name);

bool updateAllocSize() override;
size_t getSize() const override { return relocData.size(); }
void writeTo(uint8_t *buf) override {
memcpy(buf, relocData.data(), relocData.size());
}

private:
SmallVector<char, 0> relocData;
};

struct RelativeReloc {
uint64_t getOffset() const { return inputSec->getVA(offsetInSec); }

const InputSectionBase *inputSec;
uint64_t offsetInSec;
};

class RelrBaseSection : public SyntheticSection {
public:
RelrBaseSection();
bool isNeeded() const override { return !relocs.empty(); }
std::vector<RelativeReloc> relocs;
};





template <class ELFT> class RelrSection final : public RelrBaseSection {
using Elf_Relr = typename ELFT::Relr;

public:
RelrSection();

bool updateAllocSize() override;
size_t getSize() const override { return relrRelocs.size() * this->entsize; }
void writeTo(uint8_t *buf) override {
memcpy(buf, relrRelocs.data(), getSize());
}

private:
std::vector<Elf_Relr> relrRelocs;
};

struct SymbolTableEntry {
Symbol *sym;
size_t strTabOffset;
};

class SymbolTableBaseSection : public SyntheticSection {
public:
SymbolTableBaseSection(StringTableSection &strTabSec);
void finalizeContents() override;
size_t getSize() const override { return getNumSymbols() * entsize; }
void addSymbol(Symbol *sym);
unsigned getNumSymbols() const { return symbols.size() + 1; }
size_t getSymbolIndex(Symbol *sym);
ArrayRef<SymbolTableEntry> getSymbols() const { return symbols; }

protected:
void sortSymTabSymbols();


std::vector<SymbolTableEntry> symbols;

StringTableSection &strTabSec;

llvm::once_flag onceFlag;
llvm::DenseMap<Symbol *, size_t> symbolIndexMap;
llvm::DenseMap<OutputSection *, size_t> sectionIndexMap;
};

template <class ELFT>
class SymbolTableSection final : public SymbolTableBaseSection {
using Elf_Sym = typename ELFT::Sym;

public:
SymbolTableSection(StringTableSection &strTabSec);
void writeTo(uint8_t *buf) override;
};

class SymtabShndxSection final : public SyntheticSection {
public:
SymtabShndxSection();

void writeTo(uint8_t *buf) override;
size_t getSize() const override;
bool isNeeded() const override;
void finalizeContents() override;
};



class GnuHashTableSection final : public SyntheticSection {
public:
GnuHashTableSection();
void finalizeContents() override;
void writeTo(uint8_t *buf) override;
size_t getSize() const override { return size; }



void addSymbols(std::vector<SymbolTableEntry> &symbols);

private:

enum { Shift2 = 26 };

void writeBloomFilter(uint8_t *buf);
void writeHashTable(uint8_t *buf);

struct Entry {
Symbol *sym;
size_t strTabOffset;
uint32_t hash;
uint32_t bucketIdx;
};

std::vector<Entry> symbols;
size_t maskWords;
size_t nBuckets = 0;
size_t size = 0;
};

class HashTableSection final : public SyntheticSection {
public:
HashTableSection();
void finalizeContents() override;
void writeTo(uint8_t *buf) override;
size_t getSize() const override { return size; }

private:
size_t size = 0;
};












class PltSection : public SyntheticSection {
public:
PltSection();
void writeTo(uint8_t *buf) override;
size_t getSize() const override;
bool isNeeded() const override;
void addSymbols();
void addEntry(Symbol &sym);
size_t getNumEntries() const { return entries.size(); }

size_t headerSize;

std::vector<const Symbol *> entries;
};





class IpltSection final : public SyntheticSection {
std::vector<const Symbol *> entries;

public:
IpltSection();
void writeTo(uint8_t *buf) override;
size_t getSize() const override;
bool isNeeded() const override { return !entries.empty(); }
void addSymbols();
void addEntry(Symbol &sym);
};

class PPC32GlinkSection : public PltSection {
public:
PPC32GlinkSection();
void writeTo(uint8_t *buf) override;
size_t getSize() const override;

std::vector<const Symbol *> canonical_plts;
static constexpr size_t footerSize = 64;
};


class IBTPltSection : public SyntheticSection {
public:
IBTPltSection();
void writeTo(uint8_t *Buf) override;
size_t getSize() const override;
};

class GdbIndexSection final : public SyntheticSection {
public:
struct AddressEntry {
InputSection *section;
uint64_t lowAddress;
uint64_t highAddress;
uint32_t cuIndex;
};

struct CuEntry {
uint64_t cuOffset;
uint64_t cuLength;
};

struct NameAttrEntry {
llvm::CachedHashStringRef name;
uint32_t cuIndexAndAttrs;
};

struct GdbChunk {
InputSection *sec;
std::vector<AddressEntry> addressAreas;
std::vector<CuEntry> compilationUnits;
};

struct GdbSymbol {
llvm::CachedHashStringRef name;
std::vector<uint32_t> cuVector;
uint32_t nameOff;
uint32_t cuVectorOff;
};

GdbIndexSection();
template <typename ELFT> static GdbIndexSection *create();
void writeTo(uint8_t *buf) override;
size_t getSize() const override { return size; }
bool isNeeded() const override;

private:
struct GdbIndexHeader {
llvm::support::ulittle32_t version;
llvm::support::ulittle32_t cuListOff;
llvm::support::ulittle32_t cuTypesOff;
llvm::support::ulittle32_t addressAreaOff;
llvm::support::ulittle32_t symtabOff;
llvm::support::ulittle32_t constantPoolOff;
};

void initOutputSize();
size_t computeSymtabSize() const;



std::vector<GdbChunk> chunks;


std::vector<GdbSymbol> symbols;

size_t size;
};










class EhFrameHeader final : public SyntheticSection {
public:
EhFrameHeader();
void write();
void writeTo(uint8_t *buf) override;
size_t getSize() const override;
bool isNeeded() const override;
};









class VersionDefinitionSection final : public SyntheticSection {
public:
VersionDefinitionSection();
void finalizeContents() override;
size_t getSize() const override;
void writeTo(uint8_t *buf) override;

private:
enum { EntrySize = 28 };
void writeOne(uint8_t *buf, uint32_t index, StringRef name, size_t nameOff);
StringRef getFileDefName();

unsigned fileDefNameOff;
std::vector<unsigned> verDefNameOffs;
};







class VersionTableSection final : public SyntheticSection {
public:
VersionTableSection();
void finalizeContents() override;
size_t getSize() const override;
void writeTo(uint8_t *buf) override;
bool isNeeded() const override;
};






template <class ELFT>
class VersionNeedSection final : public SyntheticSection {
using Elf_Verneed = typename ELFT::Verneed;
using Elf_Vernaux = typename ELFT::Vernaux;

struct Vernaux {
uint64_t hash;
uint32_t verneedIndex;
uint64_t nameStrTab;
};

struct Verneed {
uint64_t nameStrTab;
std::vector<Vernaux> vernauxs;
};

std::vector<Verneed> verneeds;

public:
VersionNeedSection();
void finalizeContents() override;
void writeTo(uint8_t *buf) override;
size_t getSize() const override;
bool isNeeded() const override;
};





class MergeSyntheticSection : public SyntheticSection {
public:
void addSection(MergeInputSection *ms);
std::vector<MergeInputSection *> sections;

protected:
MergeSyntheticSection(StringRef name, uint32_t type, uint64_t flags,
uint32_t alignment)
: SyntheticSection(flags, type, alignment, name) {}
};

class MergeTailSection final : public MergeSyntheticSection {
public:
MergeTailSection(StringRef name, uint32_t type, uint64_t flags,
uint32_t alignment);

size_t getSize() const override;
void writeTo(uint8_t *buf) override;
void finalizeContents() override;

private:
llvm::StringTableBuilder builder;
};

class MergeNoTailSection final : public MergeSyntheticSection {
public:
MergeNoTailSection(StringRef name, uint32_t type, uint64_t flags,
uint32_t alignment)
: MergeSyntheticSection(name, type, flags, alignment) {}

size_t getSize() const override { return size; }
void writeTo(uint8_t *buf) override;
void finalizeContents() override;

private:





size_t getShardId(uint32_t hash) {
assert((hash >> 31) == 0);
return hash >> (31 - llvm::countTrailingZeros(numShards));
}


size_t size;


constexpr static size_t numShards = 32;
std::vector<llvm::StringTableBuilder> shards;
size_t shardOffsets[numShards];
};


template <class ELFT>
class MipsAbiFlagsSection final : public SyntheticSection {
using Elf_Mips_ABIFlags = llvm::object::Elf_Mips_ABIFlags<ELFT>;

public:
static MipsAbiFlagsSection *create();

MipsAbiFlagsSection(Elf_Mips_ABIFlags flags);
size_t getSize() const override { return sizeof(Elf_Mips_ABIFlags); }
void writeTo(uint8_t *buf) override;

private:
Elf_Mips_ABIFlags flags;
};


template <class ELFT> class MipsOptionsSection final : public SyntheticSection {
using Elf_Mips_Options = llvm::object::Elf_Mips_Options<ELFT>;
using Elf_Mips_RegInfo = llvm::object::Elf_Mips_RegInfo<ELFT>;

public:
static MipsOptionsSection *create();

MipsOptionsSection(Elf_Mips_RegInfo reginfo);
void writeTo(uint8_t *buf) override;

size_t getSize() const override {
return sizeof(Elf_Mips_Options) + sizeof(Elf_Mips_RegInfo);
}

private:
Elf_Mips_RegInfo reginfo;
};


template <class ELFT> class MipsReginfoSection final : public SyntheticSection {
using Elf_Mips_RegInfo = llvm::object::Elf_Mips_RegInfo<ELFT>;

public:
static MipsReginfoSection *create();

MipsReginfoSection(Elf_Mips_RegInfo reginfo);
size_t getSize() const override { return sizeof(Elf_Mips_RegInfo); }
void writeTo(uint8_t *buf) override;

private:
Elf_Mips_RegInfo reginfo;
};





class MipsRldMapSection : public SyntheticSection {
public:
MipsRldMapSection();
size_t getSize() const override { return config->wordsize; }
void writeTo(uint8_t *buf) override {}
};



































class ARMExidxSyntheticSection : public SyntheticSection {
public:
ARMExidxSyntheticSection();



bool addSection(InputSection *isec);

size_t getSize() const override { return size; }
void writeTo(uint8_t *buf) override;
bool isNeeded() const override;

void finalizeContents() override;
InputSection *getLinkOrderDep() const;

static bool classof(const SectionBase *d);



std::vector<InputSection *> exidxSections;

private:
size_t size = 0;





std::vector<InputSection *> executableSections;





InputSection *sentinel = nullptr;
};



class ThunkSection : public SyntheticSection {
public:

ThunkSection(OutputSection *os, uint64_t off);





void addThunk(Thunk *t);
size_t getSize() const override;
void writeTo(uint8_t *buf) override;
InputSection *getTargetInputSection() const;
bool assignOffsets();



bool roundUpSizeForErrata = false;

private:
std::vector<Thunk *> thunks;
size_t size = 0;
};



class PPC32Got2Section final : public SyntheticSection {
public:
PPC32Got2Section();
size_t getSize() const override { return 0; }
bool isNeeded() const override;
void finalizeContents() override;
void writeTo(uint8_t *buf) override {}
};






class PPC64LongBranchTargetSection final : public SyntheticSection {
public:
PPC64LongBranchTargetSection();
uint64_t getEntryVA(const Symbol *sym, int64_t addend);
llvm::Optional<uint32_t> addEntry(const Symbol *sym, int64_t addend);
size_t getSize() const override;
void writeTo(uint8_t *buf) override;
bool isNeeded() const override;
void finalizeContents() override { finalized = true; }

private:
std::vector<std::pair<const Symbol *, int64_t>> entries;
llvm::DenseMap<std::pair<const Symbol *, int64_t>, uint32_t> entry_index;
bool finalized = false;
};

template <typename ELFT>
class PartitionElfHeaderSection : public SyntheticSection {
public:
PartitionElfHeaderSection();
size_t getSize() const override;
void writeTo(uint8_t *buf) override;
};

template <typename ELFT>
class PartitionProgramHeadersSection : public SyntheticSection {
public:
PartitionProgramHeadersSection();
size_t getSize() const override;
void writeTo(uint8_t *buf) override;
};

class PartitionIndexSection : public SyntheticSection {
public:
PartitionIndexSection();
size_t getSize() const override;
void finalizeContents() override;
void writeTo(uint8_t *buf) override;
};

InputSection *createInterpSection();
MergeInputSection *createCommentSection();
MergeSyntheticSection *createMergeSynthetic(StringRef name, uint32_t type,
uint64_t flags, uint32_t alignment);
template <class ELFT> void splitSections();

template <typename ELFT> void writeEhdr(uint8_t *buf, Partition &part);
template <typename ELFT> void writePhdrs(uint8_t *buf, Partition &part);

Defined *addSyntheticLocal(StringRef name, uint8_t type, uint64_t value,
uint64_t size, InputSectionBase &section);

void addVerneed(Symbol *ss);


struct Partition {
StringRef name;
uint64_t nameStrTab;

SyntheticSection *elfHeader;
SyntheticSection *programHeaders;
std::vector<PhdrEntry *> phdrs;

ARMExidxSyntheticSection *armExidx;
BuildIdSection *buildId;
SyntheticSection *dynamic;
StringTableSection *dynStrTab;
SymbolTableBaseSection *dynSymTab;
EhFrameHeader *ehFrameHdr;
EhFrameSection *ehFrame;
GnuHashTableSection *gnuHashTab;
HashTableSection *hashTab;
RelocationBaseSection *relaDyn;
RelrBaseSection *relrDyn;
VersionDefinitionSection *verDef;
SyntheticSection *verNeed;
VersionTableSection *verSym;

unsigned getNumber() const { return this - &partitions[0] + 1; }
};

extern Partition *mainPart;

inline Partition &SectionBase::getPartition() const {
assert(isLive());
return partitions[partition - 1];
}



struct InStruct {
InputSection *attributes;
BssSection *bss;
BssSection *bssRelRo;
GotSection *got;
GotPltSection *gotPlt;
IgotPltSection *igotPlt;
PPC64LongBranchTargetSection *ppc64LongBranchTarget;
MipsGotSection *mipsGot;
MipsRldMapSection *mipsRldMap;
SyntheticSection *partEnd;
SyntheticSection *partIndex;
PltSection *plt;
IpltSection *iplt;
PPC32Got2Section *ppc32Got2;
IBTPltSection *ibtPlt;
RelocationBaseSection *relaPlt;
RelocationBaseSection *relaIplt;
StringTableSection *shStrTab;
StringTableSection *strTab;
SymbolTableBaseSection *symTab;
SymtabShndxSection *symTabShndx;
};

extern InStruct in;

}
}

#endif
