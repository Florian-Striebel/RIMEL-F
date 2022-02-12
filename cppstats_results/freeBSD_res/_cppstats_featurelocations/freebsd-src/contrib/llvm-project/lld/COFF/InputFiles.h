







#if !defined(LLD_COFF_INPUT_FILES_H)
#define LLD_COFF_INPUT_FILES_H

#include "Config.h"
#include "lld/Common/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/BinaryFormat/Magic.h"
#include "llvm/Object/Archive.h"
#include "llvm/Object/COFF.h"
#include "llvm/Support/StringSaver.h"
#include <memory>
#include <set>
#include <vector>

namespace llvm {
struct DILineInfo;
namespace pdb {
class DbiModuleDescriptorBuilder;
class NativeSession;
}
namespace lto {
class InputFile;
}
}

namespace lld {
class DWARFCache;

namespace coff {

std::vector<MemoryBufferRef> getArchiveMembers(llvm::object::Archive *file);

using llvm::COFF::IMAGE_FILE_MACHINE_UNKNOWN;
using llvm::COFF::MachineTypes;
using llvm::object::Archive;
using llvm::object::COFFObjectFile;
using llvm::object::COFFSymbolRef;
using llvm::object::coff_import_header;
using llvm::object::coff_section;

class Chunk;
class Defined;
class DefinedImportData;
class DefinedImportThunk;
class DefinedRegular;
class SectionChunk;
class Symbol;
class Undefined;
class TpiSource;


class InputFile {
public:
enum Kind {
ArchiveKind,
ObjectKind,
LazyObjectKind,
PDBKind,
ImportKind,
BitcodeKind,
DLLKind
};
Kind kind() const { return fileKind; }
virtual ~InputFile() {}


StringRef getName() const { return mb.getBufferIdentifier(); }


virtual void parse() = 0;


virtual MachineTypes getMachineType() { return IMAGE_FILE_MACHINE_UNKNOWN; }

MemoryBufferRef mb;


StringRef parentName;


StringRef getDirectives() { return directives; }

protected:
InputFile(Kind k, MemoryBufferRef m) : mb(m), fileKind(k) {}

StringRef directives;

private:
const Kind fileKind;
};


class ArchiveFile : public InputFile {
public:
explicit ArchiveFile(MemoryBufferRef m);
static bool classof(const InputFile *f) { return f->kind() == ArchiveKind; }
void parse() override;




void addMember(const Archive::Symbol &sym);

private:
std::unique_ptr<Archive> file;
llvm::DenseSet<uint64_t> seen;
};


class LazyObjFile : public InputFile {
public:
explicit LazyObjFile(MemoryBufferRef m) : InputFile(LazyObjectKind, m) {}
static bool classof(const InputFile *f) {
return f->kind() == LazyObjectKind;
}

void fetch();

void parse() override;

private:
std::vector<Symbol *> symbols;
};


class ObjFile : public InputFile {
public:
explicit ObjFile(MemoryBufferRef m) : InputFile(ObjectKind, m) {}
explicit ObjFile(MemoryBufferRef m, std::vector<Symbol *> &&symbols)
: InputFile(ObjectKind, m), symbols(std::move(symbols)) {}
static bool classof(const InputFile *f) { return f->kind() == ObjectKind; }
void parse() override;
MachineTypes getMachineType() override;
ArrayRef<Chunk *> getChunks() { return chunks; }
ArrayRef<SectionChunk *> getDebugChunks() { return debugChunks; }
ArrayRef<SectionChunk *> getSXDataChunks() { return sxDataChunks; }
ArrayRef<SectionChunk *> getGuardFidChunks() { return guardFidChunks; }
ArrayRef<SectionChunk *> getGuardIATChunks() { return guardIATChunks; }
ArrayRef<SectionChunk *> getGuardLJmpChunks() { return guardLJmpChunks; }
ArrayRef<SectionChunk *> getGuardEHContChunks() { return guardEHContChunks; }
ArrayRef<Symbol *> getSymbols() { return symbols; }

MutableArrayRef<Symbol *> getMutableSymbols() { return symbols; }

ArrayRef<uint8_t> getDebugSection(StringRef secName);



Symbol *getSymbol(uint32_t symbolIndex) {
return symbols[symbolIndex];
}


COFFObjectFile *getCOFFObj() { return coffObj.get(); }



uint32_t addRangeThunkSymbol(Symbol *thunk) {
symbols.push_back(thunk);
return symbols.size() - 1;
}

void includeResourceChunks();

bool isResourceObjFile() const { return !resourceChunks.empty(); }

static std::vector<ObjFile *> instances;




uint32_t feat00Flags = 0;



bool hasSafeSEH() { return feat00Flags & 0x1; }


bool hasGuardCF() { return feat00Flags & 0x4800; }





llvm::pdb::DbiModuleDescriptorBuilder *moduleDBI = nullptr;

const coff_section *addrsigSec = nullptr;

const coff_section *callgraphSec = nullptr;




llvm::Optional<uint32_t> pchSignature;


bool hotPatchable = false;


bool mergedIntoPDB = false;


TpiSource *debugTypesObj = nullptr;


ArrayRef<uint8_t> debugTypes;

llvm::Optional<std::pair<StringRef, uint32_t>>
getVariableLocation(StringRef var);

llvm::Optional<llvm::DILineInfo> getDILineInfo(uint32_t offset,
uint32_t sectionIndex);

private:
const coff_section* getSection(uint32_t i);
const coff_section *getSection(COFFSymbolRef sym) {
return getSection(sym.getSectionNumber());
}

void initializeChunks();
void initializeSymbols();
void initializeFlags();
void initializeDependencies();

SectionChunk *
readSection(uint32_t sectionNumber,
const llvm::object::coff_aux_section_definition *def,
StringRef leaderName);

void readAssociativeDefinition(
COFFSymbolRef coffSym,
const llvm::object::coff_aux_section_definition *def);

void readAssociativeDefinition(
COFFSymbolRef coffSym,
const llvm::object::coff_aux_section_definition *def,
uint32_t parentSection);

void recordPrevailingSymbolForMingw(
COFFSymbolRef coffSym,
llvm::DenseMap<StringRef, uint32_t> &prevailingSectionMap);

void maybeAssociateSEHForMingw(
COFFSymbolRef sym, const llvm::object::coff_aux_section_definition *def,
const llvm::DenseMap<StringRef, uint32_t> &prevailingSectionMap);







void
handleComdatSelection(COFFSymbolRef sym, llvm::COFF::COMDATType &selection,
bool &prevailing, DefinedRegular *leader,
const llvm::object::coff_aux_section_definition *def);

llvm::Optional<Symbol *>
createDefined(COFFSymbolRef sym,
std::vector<const llvm::object::coff_aux_section_definition *>
&comdatDefs,
bool &prevailingComdat);
Symbol *createRegular(COFFSymbolRef sym);
Symbol *createUndefined(COFFSymbolRef sym);

std::unique_ptr<COFFObjectFile> coffObj;



std::vector<Chunk *> chunks;

std::vector<SectionChunk *> resourceChunks;


std::vector<SectionChunk *> debugChunks;



std::vector<SectionChunk *> sxDataChunks;




std::vector<SectionChunk *> guardFidChunks;
std::vector<SectionChunk *> guardIATChunks;
std::vector<SectionChunk *> guardLJmpChunks;
std::vector<SectionChunk *> guardEHContChunks;





std::vector<Symbol *> symbols;






std::vector<SectionChunk *> sparseChunks;

DWARFCache *dwarf = nullptr;
};




class PDBInputFile : public InputFile {
public:
explicit PDBInputFile(MemoryBufferRef m);
~PDBInputFile();
static bool classof(const InputFile *f) { return f->kind() == PDBKind; }
void parse() override;

static void enqueue(StringRef path, ObjFile *fromFile);

static PDBInputFile *findFromRecordPath(StringRef path, ObjFile *fromFile);

static std::map<std::string, PDBInputFile *> instances;


llvm::Optional<Error> loadErr;


std::unique_ptr<llvm::pdb::NativeSession> session;


TpiSource *debugTypesObj = nullptr;
};




class ImportFile : public InputFile {
public:
explicit ImportFile(MemoryBufferRef m) : InputFile(ImportKind, m) {}

static bool classof(const InputFile *f) { return f->kind() == ImportKind; }

static std::vector<ImportFile *> instances;

Symbol *impSym = nullptr;
Symbol *thunkSym = nullptr;
std::string dllName;

private:
void parse() override;

public:
StringRef externalName;
const coff_import_header *hdr;
Chunk *location = nullptr;









bool live = !config->doGC;
bool thunkLive = !config->doGC;
};


class BitcodeFile : public InputFile {
public:
BitcodeFile(MemoryBufferRef mb, StringRef archiveName,
uint64_t offsetInArchive);
explicit BitcodeFile(MemoryBufferRef m, StringRef archiveName,
uint64_t offsetInArchive,
std::vector<Symbol *> &&symbols);
~BitcodeFile();
static bool classof(const InputFile *f) { return f->kind() == BitcodeKind; }
ArrayRef<Symbol *> getSymbols() { return symbols; }
MachineTypes getMachineType() override;
static std::vector<BitcodeFile *> instances;
std::unique_ptr<llvm::lto::InputFile> obj;

private:
void parse() override;

std::vector<Symbol *> symbols;
};


class DLLFile : public InputFile {
public:
explicit DLLFile(MemoryBufferRef m) : InputFile(DLLKind, m) {}
static bool classof(const InputFile *f) { return f->kind() == DLLKind; }
void parse() override;
MachineTypes getMachineType() override;

struct Symbol {
StringRef dllName;
StringRef symbolName;
llvm::COFF::ImportNameType nameType;
llvm::COFF::ImportType importType;
};

void makeImport(Symbol *s);

private:
std::unique_ptr<COFFObjectFile> coffObj;
llvm::StringSet<> seen;
};

inline bool isBitcode(MemoryBufferRef mb) {
return identify_magic(mb.getBuffer()) == llvm::file_magic::bitcode;
}

std::string replaceThinLTOSuffix(StringRef path);
}

std::string toString(const coff::InputFile *file);
}

#endif
