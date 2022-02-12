







#if !defined(LLD_ELF_TARGET_H)
#define LLD_ELF_TARGET_H

#include "InputSection.h"
#include "lld/Common/ErrorHandler.h"
#include "llvm/Object/ELF.h"
#include "llvm/Support/MathExtras.h"
#include <array>

namespace lld {
std::string toString(elf::RelType type);

namespace elf {
class Defined;
class InputFile;
class Symbol;

class TargetInfo {
public:
virtual uint32_t calcEFlags() const { return 0; }
virtual RelExpr getRelExpr(RelType type, const Symbol &s,
const uint8_t *loc) const = 0;
virtual RelType getDynRel(RelType type) const { return 0; }
virtual void writeGotPltHeader(uint8_t *buf) const {}
virtual void writeGotHeader(uint8_t *buf) const {}
virtual void writeGotPlt(uint8_t *buf, const Symbol &s) const {};
virtual void writeIgotPlt(uint8_t *buf, const Symbol &s) const {}
virtual int64_t getImplicitAddend(const uint8_t *buf, RelType type) const;
virtual int getTlsGdRelaxSkip(RelType type) const { return 1; }




virtual void writePltHeader(uint8_t *buf) const {}

virtual void writePlt(uint8_t *buf, const Symbol &sym,
uint64_t pltEntryAddr) const {}
virtual void writeIplt(uint8_t *buf, const Symbol &sym,
uint64_t pltEntryAddr) const {

writePlt(buf, sym, pltEntryAddr);
}
virtual void writeIBTPlt(uint8_t *buf, size_t numEntries) const {}
virtual void addPltHeaderSymbols(InputSection &isec) const {}
virtual void addPltSymbols(InputSection &isec, uint64_t off) const {}






virtual bool usesOnlyLowPageBits(RelType type) const;



virtual bool needsThunk(RelExpr expr, RelType relocType,
const InputFile *file, uint64_t branchAddr,
const Symbol &s, int64_t a) const;




virtual uint32_t getThunkSectionSpacing() const { return 0; }






virtual bool adjustPrologueForCrossSplitStack(uint8_t *loc, uint8_t *end,
uint8_t stOther) const;


virtual bool inBranchRange(RelType type, uint64_t src,
uint64_t dst) const;

virtual void relocate(uint8_t *loc, const Relocation &rel,
uint64_t val) const = 0;
void relocateNoSym(uint8_t *loc, RelType type, uint64_t val) const {
relocate(loc, Relocation{R_NONE, type, 0, 0, nullptr}, val);
}

virtual void applyJumpInstrMod(uint8_t *loc, JumpModType type,
JumpModType val) const {}

virtual ~TargetInfo();






virtual bool deleteFallThruJmpInsn(InputSection &is, InputFile *file,
InputSection *nextIS) const {
return false;
}

unsigned defaultCommonPageSize = 4096;
unsigned defaultMaxPageSize = 4096;

uint64_t getImageBase() const;


bool gotBaseSymInGotPlt = true;

RelType copyRel;
RelType gotRel;
RelType noneRel;
RelType pltRel;
RelType relativeRel;
RelType iRelativeRel;
RelType symbolicRel;
RelType tlsDescRel;
RelType tlsGotRel;
RelType tlsModuleIndexRel;
RelType tlsOffsetRel;
unsigned gotEntrySize = config->wordsize;
unsigned pltEntrySize;
unsigned pltHeaderSize;
unsigned ipltEntrySize;



unsigned gotPltHeaderEntriesNum = 3;


unsigned gotHeaderEntriesNum = 0;

bool needsThunks = false;



std::array<uint8_t, 4> trapInstr;



llvm::Optional<std::vector<std::vector<uint8_t>>> nopInstrs;




bool needsMoreStackNonSplit = true;

virtual RelExpr adjustTlsExpr(RelType type, RelExpr expr) const;
virtual RelExpr adjustGotPcExpr(RelType type, int64_t addend,
const uint8_t *loc) const;
virtual void relaxGot(uint8_t *loc, const Relocation &rel,
uint64_t val) const;
virtual void relaxTlsGdToIe(uint8_t *loc, const Relocation &rel,
uint64_t val) const;
virtual void relaxTlsGdToLe(uint8_t *loc, const Relocation &rel,
uint64_t val) const;
virtual void relaxTlsIeToLe(uint8_t *loc, const Relocation &rel,
uint64_t val) const;
virtual void relaxTlsLdToLe(uint8_t *loc, const Relocation &rel,
uint64_t val) const;

protected:




uint64_t defaultImageBase = 0x10000;
};

TargetInfo *getAArch64TargetInfo();
TargetInfo *getAMDGPUTargetInfo();
TargetInfo *getARMTargetInfo();
TargetInfo *getAVRTargetInfo();
TargetInfo *getHexagonTargetInfo();
TargetInfo *getMSP430TargetInfo();
TargetInfo *getPPC64TargetInfo();
TargetInfo *getPPCTargetInfo();
TargetInfo *getRISCVTargetInfo();
TargetInfo *getSPARCV9TargetInfo();
TargetInfo *getX86TargetInfo();
TargetInfo *getX86_64TargetInfo();
template <class ELFT> TargetInfo *getMipsTargetInfo();

struct ErrorPlace {
InputSectionBase *isec;
std::string loc;
};


ErrorPlace getErrorPlace(const uint8_t *loc);

static inline std::string getErrorLocation(const uint8_t *loc) {
return getErrorPlace(loc).loc;
}

void writePPC32GlinkSection(uint8_t *buf, size_t numEntries);

bool tryRelaxPPC64TocIndirection(const Relocation &rel, uint8_t *bufLoc);
unsigned getPPCDFormOp(unsigned secondaryOp);








unsigned getPPC64GlobalEntryToLocalEntryOffset(uint8_t stOther);



bool isPPC64SmallCodeModelTocReloc(RelType type);




void writePrefixedInstruction(uint8_t *loc, uint64_t insn);

void addPPC64SaveRestore();
uint64_t getPPC64TocBase();
uint64_t getAArch64Page(uint64_t expr);

extern const TargetInfo *target;
TargetInfo *getTarget();

template <class ELFT> bool isMipsPIC(const Defined *sym);

void reportRangeError(uint8_t *loc, const Relocation &rel, const Twine &v,
int64_t min, uint64_t max);
void reportRangeError(uint8_t *loc, int64_t v, int n, const Symbol &sym,
const Twine &msg);


inline void checkInt(uint8_t *loc, int64_t v, int n, const Relocation &rel) {
if (v != llvm::SignExtend64(v, n))
reportRangeError(loc, rel, Twine(v), llvm::minIntN(n), llvm::maxIntN(n));
}


inline void checkUInt(uint8_t *loc, uint64_t v, int n, const Relocation &rel) {
if ((v >> n) != 0)
reportRangeError(loc, rel, Twine(v), 0, llvm::maxUIntN(n));
}


inline void checkIntUInt(uint8_t *loc, uint64_t v, int n,
const Relocation &rel) {


if (v != (uint64_t)llvm::SignExtend64(v, n) && (v >> n) != 0)
reportRangeError(loc, rel, Twine((int64_t)v), llvm::minIntN(n),
llvm::maxUIntN(n));
}

inline void checkAlignment(uint8_t *loc, uint64_t v, int n,
const Relocation &rel) {
if ((v & (n - 1)) != 0)
error(getErrorLocation(loc) + "improper alignment for relocation " +
lld::toString(rel.type) + ": 0x" + llvm::utohexstr(v) +
" is not aligned to " + Twine(n) + " bytes");
}


inline uint16_t read16(const void *p) {
return llvm::support::endian::read16(p, config->endianness);
}

inline uint32_t read32(const void *p) {
return llvm::support::endian::read32(p, config->endianness);
}

inline uint64_t read64(const void *p) {
return llvm::support::endian::read64(p, config->endianness);
}

inline void write16(void *p, uint16_t v) {
llvm::support::endian::write16(p, v, config->endianness);
}

inline void write32(void *p, uint32_t v) {
llvm::support::endian::write32(p, v, config->endianness);
}

inline void write64(void *p, uint64_t v) {
llvm::support::endian::write64(p, v, config->endianness);
}
}
}

#endif
