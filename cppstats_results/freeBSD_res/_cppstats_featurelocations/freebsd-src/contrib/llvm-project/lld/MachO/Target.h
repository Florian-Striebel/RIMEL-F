







#if !defined(LLD_MACHO_TARGET_H)
#define LLD_MACHO_TARGET_H

#include "MachOStructs.h"
#include "Relocations.h"

#include "llvm/ADT/BitmaskEnum.h"
#include "llvm/BinaryFormat/MachO.h"
#include "llvm/Support/MemoryBuffer.h"

#include <cstddef>
#include <cstdint>

namespace lld {
namespace macho {
LLVM_ENABLE_BITMASK_ENUMS_IN_NAMESPACE();

class Symbol;
class Defined;
class DylibSymbol;
class InputSection;

class TargetInfo {
public:
template <class LP> TargetInfo(LP) {


magic = LP::magic;
pageZeroSize = LP::pageZeroSize;
headerSize = sizeof(typename LP::mach_header);
wordSize = LP::wordSize;
}

virtual ~TargetInfo() = default;


virtual int64_t
getEmbeddedAddend(llvm::MemoryBufferRef, uint64_t offset,
const llvm::MachO::relocation_info) const = 0;
virtual void relocateOne(uint8_t *loc, const Reloc &, uint64_t va,
uint64_t relocVA) const = 0;



virtual void writeStub(uint8_t *buf, const Symbol &) const = 0;
virtual void writeStubHelperHeader(uint8_t *buf) const = 0;
virtual void writeStubHelperEntry(uint8_t *buf, const DylibSymbol &,
uint64_t entryAddr) const = 0;






virtual void relaxGotLoad(uint8_t *loc, uint8_t type) const = 0;

virtual const RelocAttrs &getRelocAttrs(uint8_t type) const = 0;

virtual uint64_t getPageSize() const = 0;

virtual void populateThunk(InputSection *thunk, Symbol *funcSym) {
llvm_unreachable("target does not use thunks");
}

bool hasAttr(uint8_t type, RelocAttrBits bit) const {
return getRelocAttrs(type).hasAttr(bit);
}

bool usesThunks() const { return thunkSize > 0; }

uint32_t magic;
llvm::MachO::CPUType cpuType;
uint32_t cpuSubtype;

uint64_t pageZeroSize;
size_t headerSize;
size_t stubSize;
size_t stubHelperHeaderSize;
size_t stubHelperEntrySize;
size_t wordSize;

size_t thunkSize = 0;
uint64_t branchRange = 0;





static constexpr uint64_t outOfRangeVA = 0xfull << 60;
};

TargetInfo *createX86_64TargetInfo();
TargetInfo *createARM64TargetInfo();
TargetInfo *createARM64_32TargetInfo();
TargetInfo *createARMTargetInfo(uint32_t cpuSubtype);

struct LP64 {
using mach_header = llvm::MachO::mach_header_64;
using nlist = structs::nlist_64;
using segment_command = llvm::MachO::segment_command_64;
using section = llvm::MachO::section_64;
using encryption_info_command = llvm::MachO::encryption_info_command_64;

static constexpr uint32_t magic = llvm::MachO::MH_MAGIC_64;
static constexpr uint32_t segmentLCType = llvm::MachO::LC_SEGMENT_64;
static constexpr uint32_t encryptionInfoLCType =
llvm::MachO::LC_ENCRYPTION_INFO_64;

static constexpr uint64_t pageZeroSize = 1ull << 32;
static constexpr size_t wordSize = 8;
};

struct ILP32 {
using mach_header = llvm::MachO::mach_header;
using nlist = structs::nlist;
using segment_command = llvm::MachO::segment_command;
using section = llvm::MachO::section;
using encryption_info_command = llvm::MachO::encryption_info_command;

static constexpr uint32_t magic = llvm::MachO::MH_MAGIC;
static constexpr uint32_t segmentLCType = llvm::MachO::LC_SEGMENT;
static constexpr uint32_t encryptionInfoLCType =
llvm::MachO::LC_ENCRYPTION_INFO;

static constexpr uint64_t pageZeroSize = 1ull << 12;
static constexpr size_t wordSize = 4;
};

extern TargetInfo *target;

}
}

#endif