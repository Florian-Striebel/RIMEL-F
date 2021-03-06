







#if !defined(LLD_MACHO_INPUT_FILES_H)
#define LLD_MACHO_INPUT_FILES_H

#include "MachOStructs.h"
#include "Target.h"

#include "lld/Common/LLVM.h"
#include "lld/Common/Memory.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/BinaryFormat/MachO.h"
#include "llvm/DebugInfo/DWARF/DWARFUnit.h"
#include "llvm/Object/Archive.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/TextAPI/TextAPIReader.h"

#include <vector>

namespace llvm {
namespace lto {
class InputFile;
}
namespace MachO {
class InterfaceFile;
}
class TarWriter;
}

namespace lld {
namespace macho {

struct PlatformInfo;
class ConcatInputSection;
class Symbol;
struct Reloc;
enum class RefState : uint8_t;



extern std::unique_ptr<llvm::TarWriter> tar;




struct SubsectionEntry {
uint64_t offset;
InputSection *isec;
};
using SubsectionMap = std::vector<SubsectionEntry>;

class InputFile {
public:
enum Kind {
ObjKind,
OpaqueKind,
DylibKind,
ArchiveKind,
BitcodeKind,
};

virtual ~InputFile() = default;
Kind kind() const { return fileKind; }
StringRef getName() const { return name; }

MemoryBufferRef mb;

std::vector<Symbol *> symbols;
std::vector<SubsectionMap> subsections;

const int id;



std::string archiveName;

protected:
InputFile(Kind kind, MemoryBufferRef mb)
: mb(mb), id(idCount++), fileKind(kind), name(mb.getBufferIdentifier()) {}

InputFile(Kind, const llvm::MachO::InterfaceFile &);

private:
const Kind fileKind;
const StringRef name;

static int idCount;
};


class ObjFile final : public InputFile {
public:
ObjFile(MemoryBufferRef mb, uint32_t modTime, StringRef archiveName);
static bool classof(const InputFile *f) { return f->kind() == ObjKind; }

llvm::DWARFUnit *compileUnit = nullptr;
const uint32_t modTime;
std::vector<ConcatInputSection *> debugSections;
ArrayRef<llvm::MachO::data_in_code_entry> dataInCodeEntries;

private:
template <class LP> void parse();
template <class Section> void parseSections(ArrayRef<Section>);
template <class LP>
void parseSymbols(ArrayRef<typename LP::section> sectionHeaders,
ArrayRef<typename LP::nlist> nList, const char *strtab,
bool subsectionsViaSymbols);
template <class NList>
Symbol *parseNonSectionSymbol(const NList &sym, StringRef name);
template <class Section>
void parseRelocations(ArrayRef<Section> sectionHeaders, const Section &,
SubsectionMap &);
void parseDebugInfo();
void parseDataInCode();
};


class OpaqueFile final : public InputFile {
public:
OpaqueFile(MemoryBufferRef mb, StringRef segName, StringRef sectName);
static bool classof(const InputFile *f) { return f->kind() == OpaqueKind; }
};


class DylibFile final : public InputFile {
public:







explicit DylibFile(MemoryBufferRef mb, DylibFile *umbrella,
bool isBundleLoader = false);
explicit DylibFile(const llvm::MachO::InterfaceFile &interface,
DylibFile *umbrella = nullptr,
bool isBundleLoader = false);

void parseLoadCommands(MemoryBufferRef mb);
void parseReexports(const llvm::MachO::InterfaceFile &interface);

static bool classof(const InputFile *f) { return f->kind() == DylibKind; }

StringRef installName;
DylibFile *exportingFile = nullptr;
DylibFile *umbrella;
SmallVector<StringRef, 2> rpaths;
uint32_t compatibilityVersion = 0;
uint32_t currentVersion = 0;
int64_t ordinal = 0;
RefState refState;
bool reexport = false;
bool forceNeeded = false;
bool forceWeakImport = false;
bool deadStrippable = false;
bool explicitlyLinked = false;

unsigned numReferencedSymbols = 0;

bool isReferenced() const { return numReferencedSymbols > 0; }





bool isBundleLoader;

private:
bool handleLDSymbol(StringRef originalName);
void handleLDPreviousSymbol(StringRef name, StringRef originalName);
void handleLDInstallNameSymbol(StringRef name, StringRef originalName);
void checkAppExtensionSafety(bool dylibIsAppExtensionSafe) const;
};


class ArchiveFile final : public InputFile {
public:
explicit ArchiveFile(std::unique_ptr<llvm::object::Archive> &&file);
static bool classof(const InputFile *f) { return f->kind() == ArchiveKind; }
void fetch(const llvm::object::Archive::Symbol &sym);

private:
std::unique_ptr<llvm::object::Archive> file;


llvm::DenseSet<uint64_t> seen;
};

class BitcodeFile final : public InputFile {
public:
explicit BitcodeFile(MemoryBufferRef mb, StringRef archiveName,
uint64_t offsetInArchive);
static bool classof(const InputFile *f) { return f->kind() == BitcodeKind; }

std::unique_ptr<llvm::lto::InputFile> obj;
};

extern llvm::SetVector<InputFile *> inputFiles;

llvm::Optional<MemoryBufferRef> readFile(StringRef path);

namespace detail {

template <class CommandType, class... Types>
std::vector<const CommandType *>
findCommands(const void *anyHdr, size_t maxCommands, Types... types) {
std::vector<const CommandType *> cmds;
std::initializer_list<uint32_t> typesList{types...};
const auto *hdr = reinterpret_cast<const llvm::MachO::mach_header *>(anyHdr);
const uint8_t *p =
reinterpret_cast<const uint8_t *>(hdr) + target->headerSize;
for (uint32_t i = 0, n = hdr->ncmds; i < n; ++i) {
auto *cmd = reinterpret_cast<const CommandType *>(p);
if (llvm::is_contained(typesList, cmd->cmd)) {
cmds.push_back(cmd);
if (cmds.size() == maxCommands)
return cmds;
}
p += cmd->cmdsize;
}
return cmds;
}

}


template <class CommandType = llvm::MachO::load_command, class... Types>
const CommandType *findCommand(const void *anyHdr, Types... types) {
std::vector<const CommandType *> cmds =
detail::findCommands<CommandType>(anyHdr, 1, types...);
return cmds.size() ? cmds[0] : nullptr;
}

template <class CommandType = llvm::MachO::load_command, class... Types>
std::vector<const CommandType *> findCommands(const void *anyHdr,
Types... types) {
return detail::findCommands<CommandType>(anyHdr, 0, types...);
}

}

std::string toString(const macho::InputFile *file);
}

#endif
