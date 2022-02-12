







#if !defined(LLD_ELF_LINKER_SCRIPT_H)
#define LLD_ELF_LINKER_SCRIPT_H

#include "Config.h"
#include "Writer.h"
#include "lld/Common/LLVM.h"
#include "lld/Common/Strings.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace lld {
namespace elf {

class Defined;
class InputFile;
class InputSection;
class InputSectionBase;
class OutputSection;
class SectionBase;
class Symbol;
class ThunkSection;


struct ExprValue {
ExprValue(SectionBase *sec, bool forceAbsolute, uint64_t val,
const Twine &loc)
: sec(sec), forceAbsolute(forceAbsolute), val(val), loc(loc.str()) {}

ExprValue(uint64_t val) : ExprValue(nullptr, false, val, "") {}

bool isAbsolute() const { return forceAbsolute || sec == nullptr; }
uint64_t getValue() const;
uint64_t getSecAddr() const;
uint64_t getSectionOffset() const;


SectionBase *sec;



bool forceAbsolute;

uint64_t val;
uint64_t alignment = 1;



uint8_t type = llvm::ELF::STT_NOTYPE;


std::string loc;
};




using Expr = std::function<ExprValue()>;



enum SectionsCommandKind {
AssignmentKind,
OutputSectionKind,
InputSectionKind,
ByteKind
};

struct BaseCommand {
BaseCommand(int k) : kind(k) {}
int kind;
};


struct SymbolAssignment : BaseCommand {
SymbolAssignment(StringRef name, Expr e, std::string loc)
: BaseCommand(AssignmentKind), name(name), expression(e), location(loc) {}

static bool classof(const BaseCommand *c) {
return c->kind == AssignmentKind;
}


StringRef name;
Defined *sym = nullptr;


Expr expression;


bool provide = false;
bool hidden = false;


std::string location;


std::string commandString;


uint64_t addr;



uint64_t size;
};





enum class ConstraintKind { NoConstraint, ReadOnly, ReadWrite };




struct MemoryRegion {
MemoryRegion(StringRef name, Expr origin, Expr length, uint32_t flags,
uint32_t negFlags)
: name(std::string(name)), origin(origin), length(length), flags(flags),
negFlags(negFlags) {}

std::string name;
Expr origin;
Expr length;
uint32_t flags;
uint32_t negFlags;
uint64_t curPos = 0;
};




class SectionPattern {
StringMatcher excludedFilePat;


mutable llvm::Optional<std::pair<const InputFile *, bool>> excludesFileCache;

public:
SectionPattern(StringMatcher &&pat1, StringMatcher &&pat2)
: excludedFilePat(pat1), sectionPat(pat2),
sortOuter(SortSectionPolicy::Default),
sortInner(SortSectionPolicy::Default) {}

bool excludesFile(const InputFile *file) const;

StringMatcher sectionPat;
SortSectionPolicy sortOuter;
SortSectionPolicy sortInner;
};

class InputSectionDescription : public BaseCommand {
SingleStringMatcher filePat;


mutable llvm::Optional<std::pair<const InputFile *, bool>> matchesFileCache;

public:
InputSectionDescription(StringRef filePattern, uint64_t withFlags = 0,
uint64_t withoutFlags = 0)
: BaseCommand(InputSectionKind), filePat(filePattern),
withFlags(withFlags), withoutFlags(withoutFlags) {}

static bool classof(const BaseCommand *c) {
return c->kind == InputSectionKind;
}

bool matchesFile(const InputFile *file) const;



std::vector<SectionPattern> sectionPatterns;



std::vector<InputSectionBase *> sectionBases;



std::vector<InputSection *> sections;




std::vector<std::pair<ThunkSection *, uint32_t>> thunkSections;


uint64_t withFlags;
uint64_t withoutFlags;
};


struct ByteCommand : BaseCommand {
ByteCommand(Expr e, unsigned size, std::string commandString)
: BaseCommand(ByteKind), commandString(commandString), expression(e),
size(size) {}

static bool classof(const BaseCommand *c) { return c->kind == ByteKind; }


std::string commandString;

Expr expression;


unsigned offset;


unsigned size;
};

struct InsertCommand {
std::vector<StringRef> names;
bool isAfter;
StringRef where;
};

struct PhdrsCommand {
StringRef name;
unsigned type = llvm::ELF::PT_NULL;
bool hasFilehdr = false;
bool hasPhdrs = false;
llvm::Optional<unsigned> flags;
Expr lmaExpr = nullptr;
};

class LinkerScript final {



struct AddressState {
AddressState();
OutputSection *outSec = nullptr;
MemoryRegion *memRegion = nullptr;
MemoryRegion *lmaRegion = nullptr;
uint64_t lmaOffset = 0;
uint64_t tbssAddr = 0;
};

llvm::DenseMap<StringRef, OutputSection *> nameToOutputSection;

void addSymbol(SymbolAssignment *cmd);
void assignSymbol(SymbolAssignment *cmd, bool inSec);
void setDot(Expr e, const Twine &loc, bool inSec);
void expandOutputSection(uint64_t size);
void expandMemoryRegions(uint64_t size);

std::vector<InputSectionBase *>
computeInputSections(const InputSectionDescription *,
ArrayRef<InputSectionBase *>);

std::vector<InputSectionBase *> createInputSectionList(OutputSection &cmd);

void discardSynthetic(OutputSection &);

std::vector<size_t> getPhdrIndices(OutputSection *sec);

MemoryRegion *findMemoryRegion(OutputSection *sec);

void switchTo(OutputSection *sec);
uint64_t advance(uint64_t size, unsigned align);
void output(InputSection *sec);

void assignOffsets(OutputSection *sec);







AddressState *ctx = nullptr;

OutputSection *aether;

uint64_t dot;

public:
OutputSection *createOutputSection(StringRef name, StringRef location);
OutputSection *getOrCreateOutputSection(StringRef name);

bool hasPhdrsCommands() { return !phdrsCommands.empty(); }
uint64_t getDot() { return dot; }
void discard(InputSectionBase *s);

ExprValue getSymbolValue(StringRef name, const Twine &loc);

void addOrphanSections();
void diagnoseOrphanHandling() const;
void adjustSectionsBeforeSorting();
void adjustSectionsAfterSorting();

std::vector<PhdrEntry *> createPhdrs();
bool needsInterpSection();

bool shouldKeep(InputSectionBase *s);
const Defined *assignAddresses();
void allocateHeaders(std::vector<PhdrEntry *> &phdrs);
void processSectionCommands();
void processSymbolAssignments();
void declareSymbols();


void processInsertCommands();


std::vector<BaseCommand *> sectionCommands;


std::vector<PhdrsCommand> phdrsCommands;

bool hasSectionsCommand = false;
bool errorOnMissingSection = false;



std::vector<InputSectionDescription *> keptSections;


llvm::MapVector<llvm::StringRef, MemoryRegion *> memoryRegions;


std::vector<llvm::StringRef> referencedSymbols;



std::vector<InsertCommand> insertCommands;


std::vector<OutputSection *> overwriteSections;


std::vector<const InputSectionBase *> orphanSections;
};

extern LinkerScript *script;

}
}

#endif
