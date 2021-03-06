







































#if !defined(LLD_READER_WRITER_MACHO_NORMALIZE_FILE_H)
#define LLD_READER_WRITER_MACHO_NORMALIZE_FILE_H

#include "DebugInfo.h"
#include "lld/Common/LLVM.h"
#include "lld/Core/Error.h"
#include "lld/ReaderWriter/MachOLinkingContext.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/BinaryFormat/MachO.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/YAMLTraits.h"

using llvm::BumpPtrAllocator;
using llvm::yaml::Hex64;
using llvm::yaml::Hex32;
using llvm::yaml::Hex16;
using llvm::yaml::Hex8;
using llvm::yaml::SequenceTraits;
using llvm::MachO::HeaderFileType;
using llvm::MachO::BindType;
using llvm::MachO::RebaseType;
using llvm::MachO::NListType;
using llvm::MachO::RelocationInfoType;
using llvm::MachO::SectionType;
using llvm::MachO::LoadCommandType;
using llvm::MachO::ExportSymbolKind;
using llvm::MachO::DataRegionType;

namespace lld {
namespace mach_o {
namespace normalized {





struct Relocation {
Relocation() : offset(0), scattered(false),
type(llvm::MachO::GENERIC_RELOC_VANILLA),
length(0), pcRel(false), isExtern(false), value(0),
symbol(0) { }

Hex32 offset;
bool scattered;
RelocationInfoType type;
uint8_t length;
bool pcRel;
bool isExtern;
Hex32 value;
uint32_t symbol;
};


typedef std::vector<Relocation> Relocations;


typedef std::vector<Hex8> ContentBytes;


typedef std::vector<uint32_t> IndirectSymbols;


LLVM_YAML_STRONG_TYPEDEF(uint32_t, SectionAttr)


LLVM_YAML_STRONG_TYPEDEF(uint16_t, SectionAlignment)



struct Section {
Section() : type(llvm::MachO::S_REGULAR),
attributes(0), alignment(1), address(0) { }

StringRef segmentName;
StringRef sectionName;
SectionType type;
SectionAttr attributes;
SectionAlignment alignment;
Hex64 address;
ArrayRef<uint8_t> content;
Relocations relocations;
IndirectSymbols indirectSymbols;
};



LLVM_YAML_STRONG_TYPEDEF(uint8_t, SymbolScope)


LLVM_YAML_STRONG_TYPEDEF(uint16_t, SymbolDesc)




struct Symbol {
Symbol() : type(llvm::MachO::N_UNDF), scope(0), sect(0), desc(0), value(0) { }

StringRef name;
NListType type;
SymbolScope scope;
uint8_t sect;
SymbolDesc desc;
Hex64 value;
};




inline bool isZeroFillSection(SectionType T) {
return (T == llvm::MachO::S_ZEROFILL ||
T == llvm::MachO::S_THREAD_LOCAL_ZEROFILL);
}


LLVM_YAML_STRONG_TYPEDEF(uint32_t, VMProtect)


LLVM_YAML_STRONG_TYPEDEF(uint32_t, PackedVersion)



struct Segment {
StringRef name;
Hex64 address;
Hex64 size;
VMProtect init_access;
VMProtect max_access;
};



struct DependentDylib {
StringRef path;
LoadCommandType kind;
PackedVersion compatVersion;
PackedVersion currentVersion;
};


struct RebaseLocation {
Hex32 segOffset;
uint8_t segIndex;
RebaseType kind;
};


struct BindLocation {
Hex32 segOffset;
uint8_t segIndex;
BindType kind;
bool canBeNull;
int ordinal;
StringRef symbolName;
Hex64 addend;
};


LLVM_YAML_STRONG_TYPEDEF(uint32_t, ExportFlags)


struct Export {
StringRef name;
Hex64 offset;
ExportSymbolKind kind;
ExportFlags flags;
Hex32 otherOffset;
StringRef otherName;
};


struct DataInCode {
Hex32 offset;
Hex16 length;
DataRegionType kind;
};


LLVM_YAML_STRONG_TYPEDEF(uint32_t, FileFlags)


struct NormalizedFile {
MachOLinkingContext::Arch arch = MachOLinkingContext::arch_unknown;
HeaderFileType fileType = llvm::MachO::MH_OBJECT;
FileFlags flags = 0;
std::vector<Segment> segments;
std::vector<Section> sections;


std::vector<Symbol> localSymbols;
std::vector<Symbol> globalSymbols;
std::vector<Symbol> undefinedSymbols;
std::vector<Symbol> stabsSymbols;


std::vector<DependentDylib> dependentDylibs;
StringRef installName;
PackedVersion compatVersion = 0;
PackedVersion currentVersion = 0;
bool hasUUID = false;
bool hasMinVersionLoadCommand = false;
bool generateDataInCodeLoadCommand = false;
std::vector<StringRef> rpaths;
Hex64 entryAddress = 0;
Hex64 stackSize = 0;
MachOLinkingContext::OS os = MachOLinkingContext::OS::unknown;
Hex64 sourceVersion = 0;
PackedVersion minOSverson = 0;
PackedVersion sdkVersion = 0;
LoadCommandType minOSVersionKind = (LoadCommandType)0;


Hex32 pageSize = 0;
std::vector<RebaseLocation> rebasingInfo;
std::vector<BindLocation> bindingInfo;
std::vector<BindLocation> weakBindingInfo;
std::vector<BindLocation> lazyBindingInfo;
std::vector<Export> exportInfo;
std::vector<uint8_t> functionStarts;
std::vector<DataInCode> dataInCode;







BumpPtrAllocator ownedAllocations;
};


bool isThinObjectFile(StringRef path, MachOLinkingContext::Arch &arch);




bool sliceFromFatFile(MemoryBufferRef mb, MachOLinkingContext::Arch arch,
uint32_t &offset, uint32_t &size);


llvm::Expected<std::unique_ptr<NormalizedFile>>
readBinary(std::unique_ptr<MemoryBuffer> &mb,
const MachOLinkingContext::Arch arch);


llvm::Error writeBinary(const NormalizedFile &file, StringRef path);

size_t headerAndLoadCommandsSize(const NormalizedFile &file,
bool includeFunctionStarts);



llvm::Expected<std::unique_ptr<NormalizedFile>>
readYaml(std::unique_ptr<MemoryBuffer> &mb);


std::error_code writeYaml(const NormalizedFile &file, raw_ostream &out);

llvm::Error
normalizedObjectToAtoms(MachOFile *file,
const NormalizedFile &normalizedFile,
bool copyRefs);

llvm::Error
normalizedDylibToAtoms(MachODylibFile *file,
const NormalizedFile &normalizedFile,
bool copyRefs);


llvm::Expected<std::unique_ptr<lld::File>>
normalizedToAtoms(const NormalizedFile &normalizedFile, StringRef path,
bool copyRefs);


llvm::Expected<std::unique_ptr<NormalizedFile>>
normalizedFromAtoms(const lld::File &atomFile, const MachOLinkingContext &ctxt);


}


class MachOYamlIOTaggedDocumentHandler : public YamlIOTaggedDocumentHandler {
public:
MachOYamlIOTaggedDocumentHandler(MachOLinkingContext::Arch arch)
: _arch(arch) { }
bool handledDocTag(llvm::yaml::IO &io, const lld::File *&file) const override;
private:
const MachOLinkingContext::Arch _arch;
};

}
}

#endif
