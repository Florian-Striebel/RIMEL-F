







#if !defined(LLD_ELF_INPUT_FILES_H)
#define LLD_ELF_INPUT_FILES_H

#include "Config.h"
#include "lld/Common/ErrorHandler.h"
#include "lld/Common/LLVM.h"
#include "lld/Common/Reproduce.h"
#include "llvm/ADT/CachedHashString.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Comdat.h"
#include "llvm/Object/Archive.h"
#include "llvm/Object/ELF.h"
#include "llvm/Object/IRObjectFile.h"
#include "llvm/Support/Threading.h"
#include <map>

namespace llvm {
struct DILineInfo;
class TarWriter;
namespace lto {
class InputFile;
}
}

namespace lld {
class DWARFCache;


std::string toString(const elf::InputFile *f);

namespace elf {

using llvm::object::Archive;

class Symbol;



extern std::unique_ptr<llvm::TarWriter> tar;


llvm::Optional<MemoryBufferRef> readFile(StringRef path);


void parseFile(InputFile *file);


class InputFile {
public:
enum Kind {
ObjKind,
SharedKind,
LazyObjKind,
ArchiveKind,
BitcodeKind,
BinaryKind,
};

Kind kind() const { return fileKind; }

bool isElf() const {
Kind k = kind();
return k == ObjKind || k == SharedKind;
}

StringRef getName() const { return mb.getBufferIdentifier(); }
MemoryBufferRef mb;



ArrayRef<InputSectionBase *> getSections() const {
assert(fileKind == ObjKind || fileKind == BinaryKind);
return sections;
}



ArrayRef<Symbol *> getSymbols() { return getMutableSymbols(); }

MutableArrayRef<Symbol *> getMutableSymbols() {
assert(fileKind == BinaryKind || fileKind == ObjKind ||
fileKind == BitcodeKind);
return symbols;
}


StringRef getNameForScript() const;



std::string archiveName;



ELFKind ekind = ELFNoneKind;
uint16_t emachine = llvm::ELF::EM_NONE;
uint8_t osabi = 0;
uint8_t abiVersion = 0;


mutable std::string toStringCache;

std::string getSrcMsg(const Symbol &sym, InputSectionBase &sec,
uint64_t offset);


bool justSymbols = false;



uint32_t ppc32Got2OutSecOff = 0;










bool ppc64SmallCodeModelTocRelocs = false;



bool ppc64DisableTLSRelax = false;





uint32_t groupId;
static bool isInGroup;
static uint32_t nextGroupId;


llvm::Optional<size_t> mipsGotIndex;

std::vector<Symbol *> symbols;

protected:
InputFile(Kind k, MemoryBufferRef m);
std::vector<InputSectionBase *> sections;

private:
const Kind fileKind;


mutable std::string nameForScriptCache;
};

class ELFFileBase : public InputFile {
public:
ELFFileBase(Kind k, MemoryBufferRef m);
static bool classof(const InputFile *f) { return f->isElf(); }

template <typename ELFT> llvm::object::ELFFile<ELFT> getObj() const {
return check(llvm::object::ELFFile<ELFT>::create(mb.getBuffer()));
}

StringRef getStringTable() const { return stringTable; }

template <typename ELFT> typename ELFT::SymRange getELFSyms() const {
return typename ELFT::SymRange(
reinterpret_cast<const typename ELFT::Sym *>(elfSyms), numELFSyms);
}
template <typename ELFT> typename ELFT::SymRange getGlobalELFSyms() const {
return getELFSyms<ELFT>().slice(firstGlobal);
}

protected:

template <typename ELFT> void init();

const void *elfSyms = nullptr;
size_t numELFSyms = 0;
uint32_t firstGlobal = 0;
StringRef stringTable;
};


template <class ELFT> class ObjFile : public ELFFileBase {
LLVM_ELF_IMPORT_TYPES_ELFT(ELFT)

public:
static bool classof(const InputFile *f) { return f->kind() == ObjKind; }

llvm::object::ELFFile<ELFT> getObj() const {
return this->ELFFileBase::getObj<ELFT>();
}

ArrayRef<Symbol *> getLocalSymbols();
ArrayRef<Symbol *> getGlobalSymbols();

ObjFile(MemoryBufferRef m, StringRef archiveName) : ELFFileBase(ObjKind, m) {
this->archiveName = std::string(archiveName);
}

void parse(bool ignoreComdats = false);

StringRef getShtGroupSignature(ArrayRef<Elf_Shdr> sections,
const Elf_Shdr &sec);

Symbol &getSymbol(uint32_t symbolIndex) const {
if (symbolIndex >= this->symbols.size())
fatal(toString(this) + ": invalid symbol index");
return *this->symbols[symbolIndex];
}

uint32_t getSectionIndex(const Elf_Sym &sym) const;

template <typename RelT> Symbol &getRelocTargetSym(const RelT &rel) const {
uint32_t symIndex = rel.getSymbol(config->isMips64EL);
return getSymbol(symIndex);
}

llvm::Optional<llvm::DILineInfo> getDILineInfo(InputSectionBase *, uint64_t);
llvm::Optional<std::pair<std::string, unsigned>> getVariableLoc(StringRef name);




uint32_t mipsGp0 = 0;

uint32_t andFeatures = 0;




StringRef sourceFile;



bool splitStack = false;



bool someNoSplitStack = false;


const Elf_Shdr *addrsigSec = nullptr;


uint32_t cgProfileSectionIndex = 0;


DWARFCache *getDwarf();

private:
void initializeSections(bool ignoreComdats);
void initializeSymbols();
void initializeJustSymbols();

InputSectionBase *getRelocTarget(const Elf_Shdr &sec);
InputSectionBase *createInputSection(const Elf_Shdr &sec);
StringRef getSectionName(const Elf_Shdr &sec);

bool shouldMerge(const Elf_Shdr &sec, StringRef name);













ArrayRef<Elf_Word> shndxTable;


StringRef sectionStringTable;





std::unique_ptr<DWARFCache> dwarf;
llvm::once_flag initDwarf;
};








class LazyObjFile : public InputFile {
public:
LazyObjFile(MemoryBufferRef m, StringRef archiveName,
uint64_t offsetInArchive)
: InputFile(LazyObjKind, m), offsetInArchive(offsetInArchive) {
this->archiveName = std::string(archiveName);
}

static bool classof(const InputFile *f) { return f->kind() == LazyObjKind; }

template <class ELFT> void parse();
void fetch();



bool shouldFetchForCommon(const StringRef &name);

bool fetched = false;

private:
uint64_t offsetInArchive;
};


class ArchiveFile : public InputFile {
public:
explicit ArchiveFile(std::unique_ptr<Archive> &&file);
static bool classof(const InputFile *f) { return f->kind() == ArchiveKind; }
void parse();





void fetch(const Archive::Symbol &sym);



bool shouldFetchForCommon(const Archive::Symbol &sym);

size_t getMemberCount() const;
size_t getFetchedMemberCount() const { return seen.size(); }

bool parsed = false;

private:
std::unique_ptr<Archive> file;
llvm::DenseSet<uint64_t> seen;
};

class BitcodeFile : public InputFile {
public:
BitcodeFile(MemoryBufferRef m, StringRef archiveName,
uint64_t offsetInArchive);
static bool classof(const InputFile *f) { return f->kind() == BitcodeKind; }
template <class ELFT> void parse();
std::unique_ptr<llvm::lto::InputFile> obj;
};


class SharedFile : public ELFFileBase {
public:
SharedFile(MemoryBufferRef m, StringRef defaultSoName)
: ELFFileBase(SharedKind, m), soName(std::string(defaultSoName)),
isNeeded(!config->asNeeded) {}


std::vector<const void *> verdefs;




std::vector<unsigned> vernauxs;

static unsigned vernauxNum;

std::vector<StringRef> dtNeeded;
std::string soName;

static bool classof(const InputFile *f) { return f->kind() == SharedKind; }

template <typename ELFT> void parse();


bool isNeeded;



std::vector<Symbol *> requiredSymbols;

private:
template <typename ELFT>
std::vector<uint32_t> parseVerneed(const llvm::object::ELFFile<ELFT> &obj,
const typename ELFT::Shdr *sec);
};

class BinaryFile : public InputFile {
public:
explicit BinaryFile(MemoryBufferRef m) : InputFile(BinaryKind, m) {}
static bool classof(const InputFile *f) { return f->kind() == BinaryKind; }
void parse();
};

InputFile *createObjectFile(MemoryBufferRef mb, StringRef archiveName = "",
uint64_t offsetInArchive = 0);

inline bool isBitcode(MemoryBufferRef mb) {
return identify_magic(mb.getBuffer()) == llvm::file_magic::bitcode;
}

std::string replaceThinLTOSuffix(StringRef path);

extern std::vector<ArchiveFile *> archiveFiles;
extern std::vector<BinaryFile *> binaryFiles;
extern std::vector<BitcodeFile *> bitcodeFiles;
extern std::vector<LazyObjFile *> lazyObjFiles;
extern std::vector<InputFile *> objectFiles;
extern std::vector<SharedFile *> sharedFiles;

}
}

#endif
