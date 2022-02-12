







#if !defined(LLD_ELF_INPUT_SECTION_H)
#define LLD_ELF_INPUT_SECTION_H

#include "Config.h"
#include "Relocations.h"
#include "Thunks.h"
#include "lld/Common/LLVM.h"
#include "llvm/ADT/CachedHashString.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/Object/ELF.h"

namespace lld {
namespace elf {

class Symbol;
struct SectionPiece;

class Defined;
struct Partition;
class SyntheticSection;
class MergeSyntheticSection;
template <class ELFT> class ObjFile;
class OutputSection;

extern std::vector<Partition> partitions;





class SectionBase {
public:
enum Kind { Regular, EHFrame, Merge, Synthetic, Output };

Kind kind() const { return (Kind)sectionKind; }

StringRef name;






SectionBase *repl;

uint8_t sectionKind : 3;




uint8_t bss : 1;


uint8_t keepUnique : 1;




uint8_t partition;
elf::Partition &getPartition() const;


uint32_t alignment;
uint64_t flags;
uint64_t entsize;
uint32_t type;
uint32_t link;
uint32_t info;

OutputSection *getOutputSection();
const OutputSection *getOutputSection() const {
return const_cast<SectionBase *>(this)->getOutputSection();
}



uint64_t getOffset(uint64_t offset) const;

uint64_t getVA(uint64_t offset = 0) const;

bool isLive() const { return partition != 0; }
void markLive() { partition = 1; }
void markDead() { partition = 0; }

protected:
SectionBase(Kind sectionKind, StringRef name, uint64_t flags,
uint64_t entsize, uint64_t alignment, uint32_t type,
uint32_t info, uint32_t link)
: name(name), repl(this), sectionKind(sectionKind), bss(false),
keepUnique(false), partition(0), alignment(alignment), flags(flags),
entsize(entsize), type(type), link(link), info(info) {}
};


class InputSectionBase : public SectionBase {
public:
template <class ELFT>
InputSectionBase(ObjFile<ELFT> &file, const typename ELFT::Shdr &header,
StringRef name, Kind sectionKind);

InputSectionBase(InputFile *file, uint64_t flags, uint32_t type,
uint64_t entsize, uint32_t link, uint32_t info,
uint32_t alignment, ArrayRef<uint8_t> data, StringRef name,
Kind sectionKind);

static bool classof(const SectionBase *s) { return s->kind() != Output; }


unsigned numRelocations : 31;
unsigned areRelocsRela : 1;
const void *firstRelocation = nullptr;




InputFile *file;

template <class ELFT> ObjFile<ELFT> *getFile() const {
return cast_or_null<ObjFile<ELFT>>(file);
}





unsigned bytesDropped = 0;



bool nopFiller = false;

void drop_back(uint64_t num) { bytesDropped += num; }

void push_back(uint64_t num) {
assert(bytesDropped >= num);
bytesDropped -= num;
}

void trim() {
if (bytesDropped) {
rawData = rawData.drop_back(bytesDropped);
bytesDropped = 0;
}
}

ArrayRef<uint8_t> data() const {
if (uncompressedSize >= 0)
uncompress();
return rawData;
}

uint64_t getOffsetInFile() const;





SectionBase *parent = nullptr;



InputSectionBase *nextInSectionGroup = nullptr;

template <class ELFT> ArrayRef<typename ELFT::Rel> rels() const {
assert(!areRelocsRela);
return llvm::makeArrayRef(
static_cast<const typename ELFT::Rel *>(firstRelocation),
numRelocations);
}

template <class ELFT> ArrayRef<typename ELFT::Rela> relas() const {
assert(areRelocsRela);
return llvm::makeArrayRef(
static_cast<const typename ELFT::Rela *>(firstRelocation),
numRelocations);
}


llvm::TinyPtrVector<InputSection *> dependentSections;


size_t getSize() const;

InputSection *getLinkOrderDep() const;



template <class ELFT>
Defined *getEnclosingFunction(uint64_t offset);


template <class ELFT> std::string getLocation(uint64_t offset);
std::string getSrcMsg(const Symbol &sym, uint64_t offset);
std::string getObjMsg(uint64_t offset);




template <class ELFT> void relocate(uint8_t *buf, uint8_t *bufEnd);
void relocateAlloc(uint8_t *buf, uint8_t *bufEnd);
static uint64_t getRelocTargetVA(const InputFile *File, RelType Type,
int64_t A, uint64_t P, const Symbol &Sym,
RelExpr Expr);




SmallVector<Relocation, 0> relocations;





SmallVector<JumpInstrMod, 0> jumpInstrMods;






template <typename ELFT>
void adjustSplitStackFunctionPrologues(uint8_t *buf, uint8_t *end);


template <typename T> llvm::ArrayRef<T> getDataAs() const {
size_t s = data().size();
assert(s % sizeof(T) == 0);
return llvm::makeArrayRef<T>((const T *)data().data(), s / sizeof(T));
}

protected:
template <typename ELFT>
void parseCompressedHeader();
void uncompress() const;

mutable ArrayRef<uint8_t> rawData;





mutable int64_t uncompressedSize = -1;
};





struct SectionPiece {
SectionPiece(size_t off, uint32_t hash, bool live)
: inputOff(off), live(live || !config->gcSections), hash(hash >> 1) {}

uint32_t inputOff;
uint32_t live : 1;
uint32_t hash : 31;
uint64_t outputOff = 0;
};

static_assert(sizeof(SectionPiece) == 16, "SectionPiece is too big");


class MergeInputSection : public InputSectionBase {
public:
template <class ELFT>
MergeInputSection(ObjFile<ELFT> &f, const typename ELFT::Shdr &header,
StringRef name);
MergeInputSection(uint64_t flags, uint32_t type, uint64_t entsize,
ArrayRef<uint8_t> data, StringRef name);

static bool classof(const SectionBase *s) { return s->kind() == Merge; }
void splitIntoPieces();



uint64_t getParentOffset(uint64_t offset) const;



std::vector<SectionPiece> pieces;



LLVM_ATTRIBUTE_ALWAYS_INLINE
llvm::CachedHashStringRef getData(size_t i) const {
size_t begin = pieces[i].inputOff;
size_t end =
(pieces.size() - 1 == i) ? data().size() : pieces[i + 1].inputOff;
return {toStringRef(data().slice(begin, end - begin)), pieces[i].hash};
}


SectionPiece *getSectionPiece(uint64_t offset);
const SectionPiece *getSectionPiece(uint64_t offset) const {
return const_cast<MergeInputSection *>(this)->getSectionPiece(offset);
}

SyntheticSection *getParent() const;

private:
void splitStrings(ArrayRef<uint8_t> a, size_t size);
void splitNonStrings(ArrayRef<uint8_t> a, size_t size);
};

struct EhSectionPiece {
EhSectionPiece(size_t off, InputSectionBase *sec, uint32_t size,
unsigned firstRelocation)
: inputOff(off), sec(sec), size(size), firstRelocation(firstRelocation) {}

ArrayRef<uint8_t> data() const {
return {sec->data().data() + this->inputOff, size};
}

size_t inputOff;
ssize_t outputOff = -1;
InputSectionBase *sec;
uint32_t size;
unsigned firstRelocation;
};


class EhInputSection : public InputSectionBase {
public:
template <class ELFT>
EhInputSection(ObjFile<ELFT> &f, const typename ELFT::Shdr &header,
StringRef name);
static bool classof(const SectionBase *s) { return s->kind() == EHFrame; }
template <class ELFT> void split();
template <class ELFT, class RelTy> void split(ArrayRef<RelTy> rels);



std::vector<EhSectionPiece> pieces;

SyntheticSection *getParent() const;
};





class InputSection : public InputSectionBase {
public:
InputSection(InputFile *f, uint64_t flags, uint32_t type, uint32_t alignment,
ArrayRef<uint8_t> data, StringRef name, Kind k = Regular);
template <class ELFT>
InputSection(ObjFile<ELFT> &f, const typename ELFT::Shdr &header,
StringRef name);



template <class ELFT> void writeTo(uint8_t *buf);

uint64_t getOffset(uint64_t offset) const { return outSecOff + offset; }

OutputSection *getParent() const;





uint64_t outSecOff = 0;

static bool classof(const SectionBase *s);

InputSectionBase *getRelocatedSection() const;

template <class ELFT, class RelTy>
void relocateNonAlloc(uint8_t *buf, llvm::ArrayRef<RelTy> rels);


uint32_t eqClass[2] = {0, 0};


void replace(InputSection *other);

static InputSection discarded;

private:
template <class ELFT, class RelTy>
void copyRelocations(uint8_t *buf, llvm::ArrayRef<RelTy> rels);

template <class ELFT> void copyShtGroup(uint8_t *buf);
};

#if defined(_WIN32)
static_assert(sizeof(InputSection) <= 192, "InputSection is too big");
#else
static_assert(sizeof(InputSection) <= 184, "InputSection is too big");
#endif

inline bool isDebugSection(const InputSectionBase &sec) {
return (sec.flags & llvm::ELF::SHF_ALLOC) == 0 &&
(sec.name.startswith(".debug") || sec.name.startswith(".zdebug"));
}


extern std::vector<InputSectionBase *> inputSections;




extern llvm::DenseSet<std::pair<const Symbol *, uint64_t>> ppc64noTocRelax;

}

std::string toString(const elf::InputSectionBase *);
}

#endif
