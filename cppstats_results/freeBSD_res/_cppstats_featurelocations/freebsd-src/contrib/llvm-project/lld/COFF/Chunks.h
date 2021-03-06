







#if !defined(LLD_COFF_CHUNKS_H)
#define LLD_COFF_CHUNKS_H

#include "Config.h"
#include "InputFiles.h"
#include "lld/Common/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/MC/StringTableBuilder.h"
#include "llvm/Object/COFF.h"
#include <utility>
#include <vector>

namespace lld {
namespace coff {

using llvm::COFF::ImportDirectoryTableEntry;
using llvm::object::COFFSymbolRef;
using llvm::object::SectionRef;
using llvm::object::coff_relocation;
using llvm::object::coff_section;

class Baserel;
class Defined;
class DefinedImportData;
class DefinedRegular;
class ObjFile;
class OutputSection;
class RuntimePseudoReloc;
class Symbol;


const uint32_t permMask = 0xFE000000;


const uint32_t typeMask = 0x000000E0;


enum : unsigned { Log2MaxSectionAlignment = 13 };





class Chunk {
public:
enum Kind : uint8_t { SectionKind, OtherKind, ImportThunkKind };
Kind kind() const { return chunkKind; }


size_t getSize() const;



uint32_t getAlignment() const { return 1U << p2Align; }



void setAlignment(uint32_t align) {

align = align ? align : 1;
assert(llvm::isPowerOf2_32(align) && "alignment is not a power of 2");
p2Align = llvm::Log2_32(align);
assert(p2Align <= Log2MaxSectionAlignment &&
"impossible requested alignment");
}





void writeTo(uint8_t *buf) const;




uint32_t getRVA() const { return rva; }
void setRVA(uint64_t v) {

rva = (uint32_t)v;
}


uint32_t getOutputCharacteristics() const;



StringRef getSectionName() const;



void setOutputSectionIdx(uint16_t o) { osidx = o; }
uint16_t getOutputSectionIdx() const { return osidx; }
OutputSection *getOutputSection() const;



void getBaserels(std::vector<Baserel> *res);



StringRef getDebugName() const;




bool isHotPatchable() const;

protected:
Chunk(Kind k = OtherKind) : chunkKind(k), hasData(true), p2Align(0) {}

const Kind chunkKind;

public:




uint8_t hasData : 1;

public:


uint8_t p2Align : 7;



uint16_t osidx = 0;


uint32_t rva = 0;
};

class NonSectionChunk : public Chunk {
public:
virtual ~NonSectionChunk() = default;


virtual size_t getSize() const = 0;

virtual uint32_t getOutputCharacteristics() const { return 0; }





virtual void writeTo(uint8_t *buf) const {}



virtual StringRef getSectionName() const {
llvm_unreachable("unimplemented getSectionName");
}



virtual void getBaserels(std::vector<Baserel> *res) {}



virtual StringRef getDebugName() const { return ""; }

static bool classof(const Chunk *c) { return c->kind() != SectionKind; }

protected:
NonSectionChunk(Kind k = OtherKind) : Chunk(k) {}
};


class SectionChunk final : public Chunk {

friend class ICF;

public:
class symbol_iterator : public llvm::iterator_adaptor_base<
symbol_iterator, const coff_relocation *,
std::random_access_iterator_tag, Symbol *> {
friend SectionChunk;

ObjFile *file;

symbol_iterator(ObjFile *file, const coff_relocation *i)
: symbol_iterator::iterator_adaptor_base(i), file(file) {}

public:
symbol_iterator() = default;

Symbol *operator*() const { return file->getSymbol(I->SymbolTableIndex); }
};

SectionChunk(ObjFile *file, const coff_section *header);
static bool classof(const Chunk *c) { return c->kind() == SectionKind; }
size_t getSize() const { return header->SizeOfRawData; }
ArrayRef<uint8_t> getContents() const;
void writeTo(uint8_t *buf) const;


void sortRelocations();



void writeAndRelocateSubsection(ArrayRef<uint8_t> sec,
ArrayRef<uint8_t> subsec,
uint32_t &nextRelocIndex, uint8_t *buf) const;

uint32_t getOutputCharacteristics() const {
return header->Characteristics & (permMask | typeMask);
}
StringRef getSectionName() const {
return StringRef(sectionNameData, sectionNameSize);
}
void getBaserels(std::vector<Baserel> *res);
bool isCOMDAT() const;
void applyRelocation(uint8_t *off, const coff_relocation &rel) const;
void applyRelX64(uint8_t *off, uint16_t type, OutputSection *os, uint64_t s,
uint64_t p) const;
void applyRelX86(uint8_t *off, uint16_t type, OutputSection *os, uint64_t s,
uint64_t p) const;
void applyRelARM(uint8_t *off, uint16_t type, OutputSection *os, uint64_t s,
uint64_t p) const;
void applyRelARM64(uint8_t *off, uint16_t type, OutputSection *os, uint64_t s,
uint64_t p) const;

void getRuntimePseudoRelocs(std::vector<RuntimePseudoReloc> &res);



void printDiscardedMessage() const;



void addAssociative(SectionChunk *child);

StringRef getDebugName() const;



bool isCodeView() const {
return getSectionName() == ".debug" || getSectionName().startswith(".debug$");
}


bool isDWARF() const {
return getSectionName().startswith(".debug_") || getSectionName() == ".eh_frame";
}


llvm::iterator_range<symbol_iterator> symbols() const {
return llvm::make_range(symbol_iterator(file, relocsData),
symbol_iterator(file, relocsData + relocsSize));
}

ArrayRef<coff_relocation> getRelocs() const {
return llvm::makeArrayRef(relocsData, relocsSize);
}


void setRelocs(ArrayRef<coff_relocation> newRelocs) {
relocsData = newRelocs.data();
relocsSize = newRelocs.size();
assert(relocsSize == newRelocs.size() && "reloc size truncation");
}


class AssociatedIterator
: public llvm::iterator_facade_base<
AssociatedIterator, std::forward_iterator_tag, SectionChunk> {
public:
AssociatedIterator() = default;
AssociatedIterator(SectionChunk *head) : cur(head) {}
bool operator==(const AssociatedIterator &r) const { return cur == r.cur; }

SectionChunk &operator*() const { return *cur; }
SectionChunk &operator*() { return *cur; }
AssociatedIterator &operator++() {
cur = cur->assocChildren;
return *this;
}

private:
SectionChunk *cur = nullptr;
};


llvm::iterator_range<AssociatedIterator> children() const {


bool isAssoc = selection == llvm::COFF::IMAGE_COMDAT_SELECT_ASSOCIATIVE;
return llvm::make_range(
AssociatedIterator(isAssoc ? nullptr : assocChildren),
AssociatedIterator(nullptr));
}


uint32_t getSectionNumber() const;

ArrayRef<uint8_t> consumeDebugMagic();

static ArrayRef<uint8_t> consumeDebugMagic(ArrayRef<uint8_t> data,
StringRef sectionName);

static SectionChunk *findByName(ArrayRef<SectionChunk *> sections,
StringRef name);


ObjFile *file;


const coff_section *header;


DefinedRegular *sym = nullptr;



uint32_t checksum = 0;


bool live;



bool keepUnique = false;


llvm::COFF::COMDATType selection = (llvm::COFF::COMDATType)0;





SectionChunk *repl;

private:
SectionChunk *assocChildren = nullptr;


void replace(SectionChunk *other);
uint32_t eqClass[2] = {0, 0};


const coff_relocation *relocsData;


const char *sectionNameData;

uint32_t relocsSize = 0;
uint32_t sectionNameSize = 0;
};



inline size_t Chunk::getSize() const {
if (isa<SectionChunk>(this))
return static_cast<const SectionChunk *>(this)->getSize();
else
return static_cast<const NonSectionChunk *>(this)->getSize();
}

inline uint32_t Chunk::getOutputCharacteristics() const {
if (isa<SectionChunk>(this))
return static_cast<const SectionChunk *>(this)->getOutputCharacteristics();
else
return static_cast<const NonSectionChunk *>(this)
->getOutputCharacteristics();
}

inline void Chunk::writeTo(uint8_t *buf) const {
if (isa<SectionChunk>(this))
static_cast<const SectionChunk *>(this)->writeTo(buf);
else
static_cast<const NonSectionChunk *>(this)->writeTo(buf);
}

inline StringRef Chunk::getSectionName() const {
if (isa<SectionChunk>(this))
return static_cast<const SectionChunk *>(this)->getSectionName();
else
return static_cast<const NonSectionChunk *>(this)->getSectionName();
}

inline void Chunk::getBaserels(std::vector<Baserel> *res) {
if (isa<SectionChunk>(this))
static_cast<SectionChunk *>(this)->getBaserels(res);
else
static_cast<NonSectionChunk *>(this)->getBaserels(res);
}

inline StringRef Chunk::getDebugName() const {
if (isa<SectionChunk>(this))
return static_cast<const SectionChunk *>(this)->getDebugName();
else
return static_cast<const NonSectionChunk *>(this)->getDebugName();
}










class MergeChunk : public NonSectionChunk {
public:
MergeChunk(uint32_t alignment);
static void addSection(SectionChunk *c);
void finalizeContents();
void assignSubsectionRVAs();

uint32_t getOutputCharacteristics() const override;
StringRef getSectionName() const override { return ".rdata"; }
size_t getSize() const override;
void writeTo(uint8_t *buf) const override;

static MergeChunk *instances[Log2MaxSectionAlignment + 1];
std::vector<SectionChunk *> sections;

private:
llvm::StringTableBuilder builder;
bool finalized = false;
};


class CommonChunk : public NonSectionChunk {
public:
CommonChunk(const COFFSymbolRef sym);
size_t getSize() const override { return sym.getValue(); }
uint32_t getOutputCharacteristics() const override;
StringRef getSectionName() const override { return ".bss"; }

private:
const COFFSymbolRef sym;
};


class StringChunk : public NonSectionChunk {
public:
explicit StringChunk(StringRef s) : str(s) {}
size_t getSize() const override { return str.size() + 1; }
void writeTo(uint8_t *buf) const override;

private:
StringRef str;
};

static const uint8_t importThunkX86[] = {
0xff, 0x25, 0x00, 0x00, 0x00, 0x00,
};

static const uint8_t importThunkARM[] = {
0x40, 0xf2, 0x00, 0x0c,
0xc0, 0xf2, 0x00, 0x0c,
0xdc, 0xf8, 0x00, 0xf0,
};

static const uint8_t importThunkARM64[] = {
0x10, 0x00, 0x00, 0x90,
0x10, 0x02, 0x40, 0xf9,
0x00, 0x02, 0x1f, 0xd6,
};




class ImportThunkChunk : public NonSectionChunk {
public:
ImportThunkChunk(Defined *s)
: NonSectionChunk(ImportThunkKind), impSymbol(s) {}
static bool classof(const Chunk *c) { return c->kind() == ImportThunkKind; }

protected:
Defined *impSymbol;
};

class ImportThunkChunkX64 : public ImportThunkChunk {
public:
explicit ImportThunkChunkX64(Defined *s);
size_t getSize() const override { return sizeof(importThunkX86); }
void writeTo(uint8_t *buf) const override;
};

class ImportThunkChunkX86 : public ImportThunkChunk {
public:
explicit ImportThunkChunkX86(Defined *s) : ImportThunkChunk(s) {}
size_t getSize() const override { return sizeof(importThunkX86); }
void getBaserels(std::vector<Baserel> *res) override;
void writeTo(uint8_t *buf) const override;
};

class ImportThunkChunkARM : public ImportThunkChunk {
public:
explicit ImportThunkChunkARM(Defined *s) : ImportThunkChunk(s) {
setAlignment(2);
}
size_t getSize() const override { return sizeof(importThunkARM); }
void getBaserels(std::vector<Baserel> *res) override;
void writeTo(uint8_t *buf) const override;
};

class ImportThunkChunkARM64 : public ImportThunkChunk {
public:
explicit ImportThunkChunkARM64(Defined *s) : ImportThunkChunk(s) {
setAlignment(4);
}
size_t getSize() const override { return sizeof(importThunkARM64); }
void writeTo(uint8_t *buf) const override;
};

class RangeExtensionThunkARM : public NonSectionChunk {
public:
explicit RangeExtensionThunkARM(Defined *t) : target(t) { setAlignment(2); }
size_t getSize() const override;
void writeTo(uint8_t *buf) const override;

Defined *target;
};

class RangeExtensionThunkARM64 : public NonSectionChunk {
public:
explicit RangeExtensionThunkARM64(Defined *t) : target(t) { setAlignment(4); }
size_t getSize() const override;
void writeTo(uint8_t *buf) const override;

Defined *target;
};



class LocalImportChunk : public NonSectionChunk {
public:
explicit LocalImportChunk(Defined *s) : sym(s) {
setAlignment(config->wordsize);
}
size_t getSize() const override;
void getBaserels(std::vector<Baserel> *res) override;
void writeTo(uint8_t *buf) const override;

private:
Defined *sym;
};




struct ChunkAndOffset {
Chunk *inputChunk;
uint32_t offset;

struct DenseMapInfo {
static ChunkAndOffset getEmptyKey() {
return {llvm::DenseMapInfo<Chunk *>::getEmptyKey(), 0};
}
static ChunkAndOffset getTombstoneKey() {
return {llvm::DenseMapInfo<Chunk *>::getTombstoneKey(), 0};
}
static unsigned getHashValue(const ChunkAndOffset &co) {
return llvm::DenseMapInfo<std::pair<Chunk *, uint32_t>>::getHashValue(
{co.inputChunk, co.offset});
}
static bool isEqual(const ChunkAndOffset &lhs, const ChunkAndOffset &rhs) {
return lhs.inputChunk == rhs.inputChunk && lhs.offset == rhs.offset;
}
};
};

using SymbolRVASet = llvm::DenseSet<ChunkAndOffset>;


class RVATableChunk : public NonSectionChunk {
public:
explicit RVATableChunk(SymbolRVASet s) : syms(std::move(s)) {}
size_t getSize() const override { return syms.size() * 4; }
void writeTo(uint8_t *buf) const override;

private:
SymbolRVASet syms;
};


class RVAFlagTableChunk : public NonSectionChunk {
public:
explicit RVAFlagTableChunk(SymbolRVASet s) : syms(std::move(s)) {}
size_t getSize() const override { return syms.size() * 5; }
void writeTo(uint8_t *buf) const override;

private:
SymbolRVASet syms;
};




class BaserelChunk : public NonSectionChunk {
public:
BaserelChunk(uint32_t page, Baserel *begin, Baserel *end);
size_t getSize() const override { return data.size(); }
void writeTo(uint8_t *buf) const override;

private:
std::vector<uint8_t> data;
};

class Baserel {
public:
Baserel(uint32_t v, uint8_t ty) : rva(v), type(ty) {}
explicit Baserel(uint32_t v) : Baserel(v, getDefaultType()) {}
uint8_t getDefaultType();

uint32_t rva;
uint8_t type;
};





class EmptyChunk : public NonSectionChunk {
public:
EmptyChunk() {}
size_t getSize() const override { return 0; }
void writeTo(uint8_t *buf) const override {}
};







class PseudoRelocTableChunk : public NonSectionChunk {
public:
PseudoRelocTableChunk(std::vector<RuntimePseudoReloc> &relocs)
: relocs(std::move(relocs)) {
setAlignment(4);
}
size_t getSize() const override;
void writeTo(uint8_t *buf) const override;

private:
std::vector<RuntimePseudoReloc> relocs;
};




class RuntimePseudoReloc {
public:
RuntimePseudoReloc(Defined *sym, SectionChunk *target, uint32_t targetOffset,
int flags)
: sym(sym), target(target), targetOffset(targetOffset), flags(flags) {}

Defined *sym;
SectionChunk *target;
uint32_t targetOffset;


int flags;
};


class AbsolutePointerChunk : public NonSectionChunk {
public:
AbsolutePointerChunk(uint64_t value) : value(value) {
setAlignment(getSize());
}
size_t getSize() const override;
void writeTo(uint8_t *buf) const override;

private:
uint64_t value;
};




inline bool Chunk::isHotPatchable() const {
if (auto *sc = dyn_cast<SectionChunk>(this))
return sc->file->hotPatchable;
else if (isa<ImportThunkChunk>(this))
return true;
return false;
}

void applyMOV32T(uint8_t *off, uint32_t v);
void applyBranch24T(uint8_t *off, int32_t v);

void applyArm64Addr(uint8_t *off, uint64_t s, uint64_t p, int shift);
void applyArm64Imm(uint8_t *off, uint64_t imm, uint32_t rangeLimit);
void applyArm64Branch26(uint8_t *off, int64_t v);

}
}

namespace llvm {
template <>
struct DenseMapInfo<lld::coff::ChunkAndOffset>
: lld::coff::ChunkAndOffset::DenseMapInfo {};
}

#endif
