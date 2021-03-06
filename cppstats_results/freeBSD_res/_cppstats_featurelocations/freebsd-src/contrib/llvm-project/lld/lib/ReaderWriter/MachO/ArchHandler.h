







#if !defined(LLD_READER_WRITER_MACHO_ARCH_HANDLER_H)
#define LLD_READER_WRITER_MACHO_ARCH_HANDLER_H

#include "Atoms.h"
#include "File.h"
#include "MachONormalizedFile.h"
#include "lld/Common/LLVM.h"
#include "lld/Core/Error.h"
#include "lld/Core/Reference.h"
#include "lld/Core/Simple.h"
#include "lld/ReaderWriter/MachOLinkingContext.h"
#include "llvm/ADT/Triple.h"

namespace lld {
namespace mach_o {





class ArchHandler {
public:
virtual ~ArchHandler();



static std::unique_ptr<ArchHandler> create(MachOLinkingContext::Arch arch);


virtual const Registry::KindStrings *kindStrings() = 0;


virtual Reference::KindArch kindArch() = 0;



virtual bool isCallSite(const Reference &) = 0;


virtual bool isGOTAccess(const Reference &, bool &canBypassGOT) {
return false;
}


virtual bool isTLVAccess(const Reference &) const { return false; }


virtual void updateReferenceToTLV(const Reference *) {}


virtual bool isNonCallBranch(const Reference &) = 0;


virtual void updateReferenceToGOT(const Reference *, bool targetIsNowGOT) {}



virtual bool needsCompactUnwind() = 0;



virtual Reference::KindValue imageOffsetKind() = 0;




virtual Reference::KindValue imageOffsetKindIndirect() = 0;



virtual uint32_t dwarfCompactUnwindType() = 0;




virtual Reference::KindValue unwindRefToPersonalityFunctionKind() = 0;


virtual Reference::KindValue unwindRefToCIEKind() = 0;




virtual Reference::KindValue unwindRefToFunctionKind() = 0;





virtual Reference::KindValue unwindRefToEhFrameKind() = 0;



virtual Reference::KindValue pointerKind() = 0;

virtual const Atom *fdeTargetFunction(const DefinedAtom *fde);



virtual bool isPointer(const Reference &) = 0;



virtual bool isLazyPointer(const Reference &);



virtual Reference::KindValue lazyImmediateLocationKind() = 0;


virtual bool isPairedReloc(const normalized::Relocation &) = 0;



typedef std::function<llvm::Error (uint32_t sectionIndex, uint64_t addr,
const lld::Atom **, Reference::Addend *)>
FindAtomBySectionAndAddress;



typedef std::function<llvm::Error (uint32_t symbolIndex,
const lld::Atom **)> FindAtomBySymbolIndex;





virtual llvm::Error
getReferenceInfo(const normalized::Relocation &reloc,
const DefinedAtom *inAtom,
uint32_t offsetInAtom,
uint64_t fixupAddress, bool isBigEndian,
FindAtomBySectionAndAddress atomFromAddress,
FindAtomBySymbolIndex atomFromSymbolIndex,
Reference::KindValue *kind,
const lld::Atom **target,
Reference::Addend *addend) = 0;





virtual llvm::Error
getPairReferenceInfo(const normalized::Relocation &reloc1,
const normalized::Relocation &reloc2,
const DefinedAtom *inAtom,
uint32_t offsetInAtom,
uint64_t fixupAddress, bool isBig, bool scatterable,
FindAtomBySectionAndAddress atomFromAddress,
FindAtomBySymbolIndex atomFromSymbolIndex,
Reference::KindValue *kind,
const lld::Atom **target,
Reference::Addend *addend) = 0;



typedef std::function<uint32_t (const Atom &atom)> FindSymbolIndexForAtom;



typedef std::function<uint32_t (const Atom &atom)> FindSectionIndexForAtom;



typedef std::function<uint64_t (const Atom &atom)> FindAddressForAtom;


virtual bool needsLocalSymbolInRelocatableFile(const DefinedAtom *atom) {
return false;
}


virtual void generateAtomContent(const DefinedAtom &atom, bool relocatable,
FindAddressForAtom findAddress,
FindAddressForAtom findSectionAddress,
uint64_t imageBaseAddress,
llvm::MutableArrayRef<uint8_t> atomContentBuffer) = 0;


virtual void appendSectionRelocations(const DefinedAtom &atom,
uint64_t atomSectionOffset,
const Reference &ref,
FindSymbolIndexForAtom,
FindSectionIndexForAtom,
FindAddressForAtom,
normalized::Relocations&) = 0;


virtual void addAdditionalReferences(MachODefinedAtom &atom) { }


virtual void addDataInCodeReference(MachODefinedAtom &atom, uint32_t atomOff,
uint16_t length, uint16_t kind) { }



virtual bool isDataInCodeTransition(Reference::KindValue refKind) {
return false;
}



virtual Reference::KindValue dataInCodeTransitionStart(
const MachODefinedAtom &atom) {
return 0;
}



virtual Reference::KindValue dataInCodeTransitionEnd(
const MachODefinedAtom &atom) {
return 0;
}


virtual bool isThumbFunction(const DefinedAtom &atom) { return false; }


virtual const DefinedAtom *createShim(MachOFile &file, bool thumbToArm,
const DefinedAtom &) {
llvm_unreachable("shims only support on arm");
}


static bool isDwarfCIE(bool isBig, const DefinedAtom *atom);

struct ReferenceInfo {
Reference::KindArch arch;
uint16_t kind;
uint32_t offset;
int32_t addend;
};

struct OptionalRefInfo {
bool used;
uint16_t kind;
uint32_t offset;
int32_t addend;
};


struct StubInfo {
const char* binderSymbolName;
ReferenceInfo lazyPointerReferenceToHelper;
ReferenceInfo lazyPointerReferenceToFinal;
ReferenceInfo nonLazyPointerReferenceToBinder;
uint8_t codeAlignment;

uint32_t stubSize;
uint8_t stubBytes[16];
ReferenceInfo stubReferenceToLP;
OptionalRefInfo optStubReferenceToLP;

uint32_t stubHelperSize;
uint8_t stubHelperBytes[16];
ReferenceInfo stubHelperReferenceToImm;
ReferenceInfo stubHelperReferenceToHelperCommon;

DefinedAtom::ContentType stubHelperImageCacheContentType;

uint32_t stubHelperCommonSize;
uint8_t stubHelperCommonAlignment;
uint8_t stubHelperCommonBytes[36];
ReferenceInfo stubHelperCommonReferenceToCache;
OptionalRefInfo optStubHelperCommonReferenceToCache;
ReferenceInfo stubHelperCommonReferenceToBinder;
OptionalRefInfo optStubHelperCommonReferenceToBinder;
};

virtual const StubInfo &stubInfo() = 0;

protected:
ArchHandler();

static std::unique_ptr<mach_o::ArchHandler> create_x86_64();
static std::unique_ptr<mach_o::ArchHandler> create_x86();
static std::unique_ptr<mach_o::ArchHandler> create_arm();
static std::unique_ptr<mach_o::ArchHandler> create_arm64();


typedef uint16_t RelocPattern;
enum {
rScattered = 0x8000,
rPcRel = 0x4000,
rExtern = 0x2000,
rLength1 = 0x0000,
rLength2 = 0x0100,
rLength4 = 0x0200,
rLength8 = 0x0300,
rLenArmLo = rLength1,
rLenArmHi = rLength2,
rLenThmbLo = rLength4,
rLenThmbHi = rLength8
};

static RelocPattern relocPattern(const normalized::Relocation &reloc);

static normalized::Relocation relocFromPattern(RelocPattern pattern);

static void appendReloc(normalized::Relocations &relocs, uint32_t offset,
uint32_t symbol, uint32_t value,
RelocPattern pattern);


static int16_t readS16(const uint8_t *addr, bool isBig);
static int32_t readS32(const uint8_t *addr, bool isBig);
static uint32_t readU32(const uint8_t *addr, bool isBig);
static int64_t readS64(const uint8_t *addr, bool isBig);
};

}
}

#endif
