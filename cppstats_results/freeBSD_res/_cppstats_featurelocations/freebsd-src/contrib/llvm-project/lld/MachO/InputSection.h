







#if !defined(LLD_MACHO_INPUT_SECTION_H)
#define LLD_MACHO_INPUT_SECTION_H

#include "Config.h"
#include "Relocations.h"

#include "lld/Common/LLVM.h"
#include "lld/Common/Memory.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/CachedHashString.h"
#include "llvm/BinaryFormat/MachO.h"

namespace lld {
namespace macho {

class InputFile;
class OutputSection;
class Defined;

class InputSection {
public:
enum Kind {
ConcatKind,
CStringLiteralKind,
WordLiteralKind,
};

Kind kind() const { return shared->sectionKind; }
virtual ~InputSection() = default;
virtual uint64_t getSize() const { return data.size(); }
InputFile *getFile() const { return shared->file; }
StringRef getName() const { return shared->name; }
StringRef getSegName() const { return shared->segname; }
uint32_t getFlags() const { return shared->flags; }
uint64_t getFileSize() const;


virtual uint64_t getOffset(uint64_t off) const = 0;

uint64_t getVA(uint64_t off) const;

virtual bool isLive(uint64_t off) const = 0;
virtual void markLive(uint64_t off) = 0;
virtual InputSection *canonical() { return this; }

OutputSection *parent = nullptr;

uint32_t align = 1;
uint32_t callSiteCount : 31;

uint32_t isFinal : 1;

ArrayRef<uint8_t> data;
std::vector<Reloc> relocs;

protected:




struct Shared {
InputFile *file;
StringRef name;
StringRef segname;
uint32_t flags;
Kind sectionKind;
Shared(InputFile *file, StringRef name, StringRef segname, uint32_t flags,
Kind kind)
: file(file), name(name), segname(segname), flags(flags),
sectionKind(kind) {}
};

InputSection(Kind kind, StringRef segname, StringRef name)
: callSiteCount(0), isFinal(false),
shared(make<Shared>(nullptr, name, segname, 0, kind)) {}

InputSection(Kind kind, StringRef segname, StringRef name, InputFile *file,
ArrayRef<uint8_t> data, uint32_t align, uint32_t flags)
: align(align), callSiteCount(0), isFinal(false), data(data),
shared(make<Shared>(file, name, segname, flags, kind)) {}

const Shared *const shared;
};




class ConcatInputSection final : public InputSection {
public:
ConcatInputSection(StringRef segname, StringRef name)
: InputSection(ConcatKind, segname, name) {}

ConcatInputSection(StringRef segname, StringRef name, InputFile *file,
ArrayRef<uint8_t> data, uint32_t align = 1,
uint32_t flags = 0)
: InputSection(ConcatKind, segname, name, file, data, align, flags) {}

uint64_t getOffset(uint64_t off) const override { return outSecOff + off; }
uint64_t getVA() const { return InputSection::getVA(0); }

bool isLive(uint64_t off) const override { return live; }
void markLive(uint64_t off) override { live = true; }
bool isCoalescedWeak() const { return wasCoalesced && numRefs == 0; }
bool shouldOmitFromOutput() const { return !live || isCoalescedWeak(); }
bool isHashableForICF() const;
void hashForICF();
void writeTo(uint8_t *buf);

void foldIdentical(ConcatInputSection *redundant);
InputSection *canonical() override {
return replacement ? replacement : this;
}

static bool classof(const InputSection *isec) {
return isec->kind() == ConcatKind;
}


InputSection *replacement = nullptr;

uint64_t icfEqClass[2] = {0, 0};






bool wasCoalesced = false;
bool live = !config->deadStrip;

uint32_t numRefs = 0;



uint64_t outSecOff = 0;
};


static_assert(sizeof(int) != 8 || sizeof(ConcatInputSection) == 112,
"Try to minimize ConcatInputSection's size, we create many "
"instances of it");



inline bool shouldOmitFromOutput(InputSection *isec) {
return isa<ConcatInputSection>(isec) &&
cast<ConcatInputSection>(isec)->shouldOmitFromOutput();
}

inline bool isCoalescedWeak(InputSection *isec) {
return isa<ConcatInputSection>(isec) &&
cast<ConcatInputSection>(isec)->isCoalescedWeak();
}



struct StringPiece {

uint32_t inSecOff;
uint32_t live : 1;

uint32_t hash : 31;

uint64_t outSecOff = 0;

StringPiece(uint64_t off, uint32_t hash)
: inSecOff(off), live(!config->deadStrip), hash(hash) {}
};

static_assert(sizeof(StringPiece) == 16, "StringPiece is too big!");











class CStringInputSection final : public InputSection {
public:
CStringInputSection(StringRef segname, StringRef name, InputFile *file,
ArrayRef<uint8_t> data, uint32_t align, uint32_t flags)
: InputSection(CStringLiteralKind, segname, name, file, data, align,
flags) {}
uint64_t getOffset(uint64_t off) const override;
bool isLive(uint64_t off) const override { return getStringPiece(off).live; }
void markLive(uint64_t off) override { getStringPiece(off).live = true; }

StringPiece &getStringPiece(uint64_t off);
const StringPiece &getStringPiece(uint64_t off) const;

void splitIntoPieces();

LLVM_ATTRIBUTE_ALWAYS_INLINE
StringRef getStringRef(size_t i) const {
size_t begin = pieces[i].inSecOff;
size_t end =
(pieces.size() - 1 == i) ? data.size() : pieces[i + 1].inSecOff;
return toStringRef(data.slice(begin, end - begin));
}



LLVM_ATTRIBUTE_ALWAYS_INLINE
llvm::CachedHashStringRef getCachedHashStringRef(size_t i) const {
assert(config->dedupLiterals);
return {getStringRef(i), pieces[i].hash};
}

static bool classof(const InputSection *isec) {
return isec->kind() == CStringLiteralKind;
}

std::vector<StringPiece> pieces;
};

class WordLiteralInputSection final : public InputSection {
public:
WordLiteralInputSection(StringRef segname, StringRef name, InputFile *file,
ArrayRef<uint8_t> data, uint32_t align,
uint32_t flags);
uint64_t getOffset(uint64_t off) const override;
bool isLive(uint64_t off) const override {
return live[off >> power2LiteralSize];
}
void markLive(uint64_t off) override { live[off >> power2LiteralSize] = 1; }

static bool classof(const InputSection *isec) {
return isec->kind() == WordLiteralKind;
}

private:
unsigned power2LiteralSize;

llvm::BitVector live;
};

inline uint8_t sectionType(uint32_t flags) {
return flags & llvm::MachO::SECTION_TYPE;
}

inline bool isZeroFill(uint32_t flags) {
return llvm::MachO::isVirtualSection(sectionType(flags));
}

inline bool isThreadLocalVariables(uint32_t flags) {
return sectionType(flags) == llvm::MachO::S_THREAD_LOCAL_VARIABLES;
}


inline bool isThreadLocalData(uint32_t flags) {
return sectionType(flags) == llvm::MachO::S_THREAD_LOCAL_REGULAR ||
sectionType(flags) == llvm::MachO::S_THREAD_LOCAL_ZEROFILL;
}

inline bool isDebugSection(uint32_t flags) {
return (flags & llvm::MachO::SECTION_ATTRIBUTES_USR) ==
llvm::MachO::S_ATTR_DEBUG;
}

inline bool isWordLiteralSection(uint32_t flags) {
return sectionType(flags) == llvm::MachO::S_4BYTE_LITERALS ||
sectionType(flags) == llvm::MachO::S_8BYTE_LITERALS ||
sectionType(flags) == llvm::MachO::S_16BYTE_LITERALS;
}

bool isCodeSection(const InputSection *);

bool isCfStringSection(const InputSection *);

extern std::vector<ConcatInputSection *> inputSections;

namespace section_names {

constexpr const char authGot[] = "__auth_got";
constexpr const char authPtr[] = "__auth_ptr";
constexpr const char binding[] = "__binding";
constexpr const char bitcodeBundle[] = "__bundle";
constexpr const char cString[] = "__cstring";
constexpr const char cfString[] = "__cfstring";
constexpr const char codeSignature[] = "__code_signature";
constexpr const char common[] = "__common";
constexpr const char compactUnwind[] = "__compact_unwind";
constexpr const char data[] = "__data";
constexpr const char debugAbbrev[] = "__debug_abbrev";
constexpr const char debugInfo[] = "__debug_info";
constexpr const char debugStr[] = "__debug_str";
constexpr const char ehFrame[] = "__eh_frame";
constexpr const char export_[] = "__export";
constexpr const char dataInCode[] = "__data_in_code";
constexpr const char functionStarts[] = "__func_starts";
constexpr const char got[] = "__got";
constexpr const char header[] = "__mach_header";
constexpr const char indirectSymbolTable[] = "__ind_sym_tab";
constexpr const char const_[] = "__const";
constexpr const char lazySymbolPtr[] = "__la_symbol_ptr";
constexpr const char lazyBinding[] = "__lazy_binding";
constexpr const char literals[] = "__literals";
constexpr const char moduleInitFunc[] = "__mod_init_func";
constexpr const char moduleTermFunc[] = "__mod_term_func";
constexpr const char nonLazySymbolPtr[] = "__nl_symbol_ptr";
constexpr const char objcCatList[] = "__objc_catlist";
constexpr const char objcClassList[] = "__objc_classlist";
constexpr const char objcConst[] = "__objc_const";
constexpr const char objcImageInfo[] = "__objc_imageinfo";
constexpr const char objcNonLazyCatList[] = "__objc_nlcatlist";
constexpr const char objcNonLazyClassList[] = "__objc_nlclslist";
constexpr const char objcProtoList[] = "__objc_protolist";
constexpr const char pageZero[] = "__pagezero";
constexpr const char pointers[] = "__pointers";
constexpr const char rebase[] = "__rebase";
constexpr const char staticInit[] = "__StaticInit";
constexpr const char stringTable[] = "__string_table";
constexpr const char stubHelper[] = "__stub_helper";
constexpr const char stubs[] = "__stubs";
constexpr const char swift[] = "__swift";
constexpr const char symbolTable[] = "__symbol_table";
constexpr const char textCoalNt[] = "__textcoal_nt";
constexpr const char text[] = "__text";
constexpr const char threadPtrs[] = "__thread_ptrs";
constexpr const char threadVars[] = "__thread_vars";
constexpr const char unwindInfo[] = "__unwind_info";
constexpr const char weakBinding[] = "__weak_binding";
constexpr const char zeroFill[] = "__zerofill";

}

}

std::string toString(const macho::InputSection *);

}

#endif
