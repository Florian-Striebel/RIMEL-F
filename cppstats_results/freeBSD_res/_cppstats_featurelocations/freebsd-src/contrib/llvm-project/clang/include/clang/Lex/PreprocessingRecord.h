












#if !defined(LLVM_CLANG_LEX_PREPROCESSINGRECORD_H)
#define LLVM_CLANG_LEX_PREPROCESSINGRECORD_H

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/PPCallbacks.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Compiler.h"
#include <cassert>
#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

namespace clang {

class PreprocessingRecord;

}


void *operator new(size_t bytes, clang::PreprocessingRecord &PR,
unsigned alignment = 8) noexcept;


void operator delete(void *ptr, clang::PreprocessingRecord &PR,
unsigned) noexcept;

namespace clang {

class FileEntry;
class IdentifierInfo;
class MacroInfo;
class SourceManager;
class Token;



class PreprocessedEntity {
public:

enum EntityKind {

InvalidKind,


MacroExpansionKind,





MacroDefinitionKind,



InclusionDirectiveKind,



FirstPreprocessingDirective = MacroDefinitionKind,
LastPreprocessingDirective = InclusionDirectiveKind
};

private:

EntityKind Kind;


SourceRange Range;

protected:
friend class PreprocessingRecord;

PreprocessedEntity(EntityKind Kind, SourceRange Range)
: Kind(Kind), Range(Range) {}

public:

EntityKind getKind() const { return Kind; }



SourceRange getSourceRange() const LLVM_READONLY { return Range; }



bool isInvalid() const { return Kind == InvalidKind; }



void *operator new(size_t bytes, PreprocessingRecord &PR,
unsigned alignment = 8) noexcept {
return ::operator new(bytes, PR, alignment);
}

void *operator new(size_t bytes, void *mem) noexcept { return mem; }

void operator delete(void *ptr, PreprocessingRecord &PR,
unsigned alignment) noexcept {
return ::operator delete(ptr, PR, alignment);
}

void operator delete(void *, std::size_t) noexcept {}
void operator delete(void *, void *) noexcept {}

private:

void *operator new(size_t bytes) noexcept;
void operator delete(void *data) noexcept;
};


class PreprocessingDirective : public PreprocessedEntity {
public:
PreprocessingDirective(EntityKind Kind, SourceRange Range)
: PreprocessedEntity(Kind, Range) {}


static bool classof(const PreprocessedEntity *PD) {
return PD->getKind() >= FirstPreprocessingDirective &&
PD->getKind() <= LastPreprocessingDirective;
}
};


class MacroDefinitionRecord : public PreprocessingDirective {

const IdentifierInfo *Name;

public:
explicit MacroDefinitionRecord(const IdentifierInfo *Name,
SourceRange Range)
: PreprocessingDirective(MacroDefinitionKind, Range), Name(Name) {}


const IdentifierInfo *getName() const { return Name; }


SourceLocation getLocation() const { return getSourceRange().getBegin(); }


static bool classof(const PreprocessedEntity *PE) {
return PE->getKind() == MacroDefinitionKind;
}
};


class MacroExpansion : public PreprocessedEntity {


llvm::PointerUnion<IdentifierInfo *, MacroDefinitionRecord *> NameOrDef;

public:
MacroExpansion(IdentifierInfo *BuiltinName, SourceRange Range)
: PreprocessedEntity(MacroExpansionKind, Range),
NameOrDef(BuiltinName) {}

MacroExpansion(MacroDefinitionRecord *Definition, SourceRange Range)
: PreprocessedEntity(MacroExpansionKind, Range), NameOrDef(Definition) {
}


bool isBuiltinMacro() const { return NameOrDef.is<IdentifierInfo *>(); }


const IdentifierInfo *getName() const {
if (MacroDefinitionRecord *Def = getDefinition())
return Def->getName();
return NameOrDef.get<IdentifierInfo *>();
}



MacroDefinitionRecord *getDefinition() const {
return NameOrDef.dyn_cast<MacroDefinitionRecord *>();
}


static bool classof(const PreprocessedEntity *PE) {
return PE->getKind() == MacroExpansionKind;
}
};



class InclusionDirective : public PreprocessingDirective {
public:


enum InclusionKind {

Include,


Import,


IncludeNext,


IncludeMacros
};

private:


StringRef FileName;



unsigned InQuotes : 1;




unsigned Kind : 2;



unsigned ImportedModule : 1;


const FileEntry *File;

public:
InclusionDirective(PreprocessingRecord &PPRec,
InclusionKind Kind, StringRef FileName,
bool InQuotes, bool ImportedModule,
const FileEntry *File, SourceRange Range);


InclusionKind getKind() const { return static_cast<InclusionKind>(Kind); }


StringRef getFileName() const { return FileName; }



bool wasInQuotes() const { return InQuotes; }



bool importedModule() const { return ImportedModule; }



const FileEntry *getFile() const { return File; }


static bool classof(const PreprocessedEntity *PE) {
return PE->getKind() == InclusionDirectiveKind;
}
};



class ExternalPreprocessingRecordSource {
public:
virtual ~ExternalPreprocessingRecordSource();





virtual PreprocessedEntity *ReadPreprocessedEntity(unsigned Index) = 0;



virtual std::pair<unsigned, unsigned>
findPreprocessedEntitiesInRange(SourceRange Range) = 0;



virtual Optional<bool> isPreprocessedEntityInFileID(unsigned Index,
FileID FID) {
return None;
}


virtual SourceRange ReadSkippedRange(unsigned Index) = 0;
};




class PreprocessingRecord : public PPCallbacks {
SourceManager &SourceMgr;


llvm::BumpPtrAllocator BumpAlloc;



std::vector<PreprocessedEntity *> PreprocessedEntities;






std::vector<PreprocessedEntity *> LoadedPreprocessedEntities;


std::vector<SourceRange> SkippedRanges;

bool SkippedRangesAllLoaded = true;










class PPEntityID {
friend class PreprocessingRecord;

int ID = 0;

explicit PPEntityID(int ID) : ID(ID) {}

public:
PPEntityID() = default;
};

static PPEntityID getPPEntityID(unsigned Index, bool isLoaded) {
return isLoaded ? PPEntityID(-int(Index)-1) : PPEntityID(Index+1);
}


llvm::DenseMap<const MacroInfo *, MacroDefinitionRecord *> MacroDefinitions;


ExternalPreprocessingRecordSource *ExternalSource = nullptr;


PreprocessedEntity *getPreprocessedEntity(PPEntityID PPID);


PreprocessedEntity *getLoadedPreprocessedEntity(unsigned Index);



unsigned getNumLoadedPreprocessedEntities() const {
return LoadedPreprocessedEntities.size();
}



std::pair<unsigned, unsigned>
findLocalPreprocessedEntitiesInRange(SourceRange Range) const;
unsigned findBeginLocalPreprocessedEntity(SourceLocation Loc) const;
unsigned findEndLocalPreprocessedEntity(SourceLocation Loc) const;





unsigned allocateLoadedEntities(unsigned NumEntities);






unsigned allocateSkippedRanges(unsigned NumRanges);


void ensureSkippedRangesLoaded();


void RegisterMacroDefinition(MacroInfo *Macro, MacroDefinitionRecord *Def);

public:

explicit PreprocessingRecord(SourceManager &SM);


void *Allocate(unsigned Size, unsigned Align = 8) {
return BumpAlloc.Allocate(Size, Align);
}


void Deallocate(void *Ptr) {}

size_t getTotalMemory() const;

SourceManager &getSourceManager() const { return SourceMgr; }

















class iterator : public llvm::iterator_adaptor_base<
iterator, int, std::random_access_iterator_tag,
PreprocessedEntity *, int, PreprocessedEntity *,
PreprocessedEntity *> {
friend class PreprocessingRecord;

PreprocessingRecord *Self;

iterator(PreprocessingRecord *Self, int Position)
: iterator::iterator_adaptor_base(Position), Self(Self) {}

public:
iterator() : iterator(nullptr, 0) {}

PreprocessedEntity *operator*() const {
bool isLoaded = this->I < 0;
unsigned Index = isLoaded ?
Self->LoadedPreprocessedEntities.size() + this->I : this->I;
PPEntityID ID = Self->getPPEntityID(Index, isLoaded);
return Self->getPreprocessedEntity(ID);
}
PreprocessedEntity *operator->() const { return **this; }
};


iterator begin() {
return iterator(this, -(int)LoadedPreprocessedEntities.size());
}


iterator end() {
return iterator(this, PreprocessedEntities.size());
}


iterator local_begin() {
return iterator(this, 0);
}


iterator local_end() {
return iterator(this, PreprocessedEntities.size());
}



llvm::iterator_range<iterator> getIteratorsForLoadedRange(unsigned start,
unsigned count) {
unsigned end = start + count;
assert(end <= LoadedPreprocessedEntities.size());
return llvm::make_range(
iterator(this, int(start) - LoadedPreprocessedEntities.size()),
iterator(this, int(end) - LoadedPreprocessedEntities.size()));
}





llvm::iterator_range<iterator>
getPreprocessedEntitiesInRange(SourceRange R);








bool isEntityInFileID(iterator PPEI, FileID FID);


PPEntityID addPreprocessedEntity(PreprocessedEntity *Entity);


void SetExternalSource(ExternalPreprocessingRecordSource &Source);


ExternalPreprocessingRecordSource *getExternalSource() const {
return ExternalSource;
}



MacroDefinitionRecord *findMacroDefinition(const MacroInfo *MI);


const std::vector<SourceRange> &getSkippedRanges() {
ensureSkippedRangesLoaded();
return SkippedRanges;
}

private:
friend class ASTReader;
friend class ASTWriter;

void MacroExpands(const Token &Id, const MacroDefinition &MD,
SourceRange Range, const MacroArgs *Args) override;
void MacroDefined(const Token &Id, const MacroDirective *MD) override;
void MacroUndefined(const Token &Id, const MacroDefinition &MD,
const MacroDirective *Undef) override;
void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok,
StringRef FileName, bool IsAngled,
CharSourceRange FilenameRange,
const FileEntry *File, StringRef SearchPath,
StringRef RelativePath, const Module *Imported,
SrcMgr::CharacteristicKind FileType) override;
void Ifdef(SourceLocation Loc, const Token &MacroNameTok,
const MacroDefinition &MD) override;
void Ifndef(SourceLocation Loc, const Token &MacroNameTok,
const MacroDefinition &MD) override;

using PPCallbacks::Elifdef;
using PPCallbacks::Elifndef;
void Elifdef(SourceLocation Loc, const Token &MacroNameTok,
const MacroDefinition &MD) override;
void Elifndef(SourceLocation Loc, const Token &MacroNameTok,
const MacroDefinition &MD) override;


void Defined(const Token &MacroNameTok, const MacroDefinition &MD,
SourceRange Range) override;

void SourceRangeSkipped(SourceRange Range,
SourceLocation EndifLoc) override;

void addMacroExpansion(const Token &Id, const MacroInfo *MI,
SourceRange Range);



struct {
SourceRange Range;
std::pair<int, int> Result;
} CachedRangeQuery;

std::pair<int, int> getPreprocessedEntitiesInRangeSlow(SourceRange R);
};

}

inline void *operator new(size_t bytes, clang::PreprocessingRecord &PR,
unsigned alignment) noexcept {
return PR.Allocate(bytes, alignment);
}

inline void operator delete(void *ptr, clang::PreprocessingRecord &PR,
unsigned) noexcept {
PR.Deallocate(ptr);
}

#endif
