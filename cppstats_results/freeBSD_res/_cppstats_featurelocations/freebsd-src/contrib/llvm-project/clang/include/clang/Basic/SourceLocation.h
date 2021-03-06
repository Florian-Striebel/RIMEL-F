












#if !defined(LLVM_CLANG_BASIC_SOURCELOCATION_H)
#define LLVM_CLANG_BASIC_SOURCELOCATION_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include <cassert>
#include <cstdint>
#include <string>
#include <utility>

namespace llvm {

template <typename T> struct DenseMapInfo;

class FoldingSetNodeID;
template <typename T> struct FoldingSetTrait;

}

namespace clang {

class SourceManager;




class FileID {


int ID = 0;

public:
bool isValid() const { return ID != 0; }
bool isInvalid() const { return ID == 0; }

bool operator==(const FileID &RHS) const { return ID == RHS.ID; }
bool operator<(const FileID &RHS) const { return ID < RHS.ID; }
bool operator<=(const FileID &RHS) const { return ID <= RHS.ID; }
bool operator!=(const FileID &RHS) const { return !(*this == RHS); }
bool operator>(const FileID &RHS) const { return RHS < *this; }
bool operator>=(const FileID &RHS) const { return RHS <= *this; }

static FileID getSentinel() { return get(-1); }
unsigned getHashValue() const { return static_cast<unsigned>(ID); }

private:
friend class ASTWriter;
friend class ASTReader;
friend class SourceManager;

static FileID get(int V) {
FileID F;
F.ID = V;
return F;
}

int getOpaqueValue() const { return ID; }
};
















class SourceLocation {
friend class ASTReader;
friend class ASTWriter;
friend class SourceManager;
friend struct llvm::FoldingSetTrait<SourceLocation>;

public:
using UIntTy = uint32_t;
using IntTy = int32_t;

private:
UIntTy ID = 0;

enum : UIntTy { MacroIDBit = 1ULL << (8 * sizeof(UIntTy) - 1) };

public:
bool isFileID() const { return (ID & MacroIDBit) == 0; }
bool isMacroID() const { return (ID & MacroIDBit) != 0; }






bool isValid() const { return ID != 0; }
bool isInvalid() const { return ID == 0; }

private:

UIntTy getOffset() const { return ID & ~MacroIDBit; }

static SourceLocation getFileLoc(UIntTy ID) {
assert((ID & MacroIDBit) == 0 && "Ran out of source locations!");
SourceLocation L;
L.ID = ID;
return L;
}

static SourceLocation getMacroLoc(UIntTy ID) {
assert((ID & MacroIDBit) == 0 && "Ran out of source locations!");
SourceLocation L;
L.ID = MacroIDBit | ID;
return L;
}

public:


SourceLocation getLocWithOffset(IntTy Offset) const {
assert(((getOffset()+Offset) & MacroIDBit) == 0 && "offset overflow");
SourceLocation L;
L.ID = ID+Offset;
return L;
}






UIntTy getRawEncoding() const { return ID; }





static SourceLocation getFromRawEncoding(UIntTy Encoding) {
SourceLocation X;
X.ID = Encoding;
return X;
}






void* getPtrEncoding() const {


return (void*)(uintptr_t)getRawEncoding();
}



static SourceLocation getFromPtrEncoding(const void *Encoding) {
return getFromRawEncoding((SourceLocation::UIntTy)(uintptr_t)Encoding);
}

static bool isPairOfFileLocations(SourceLocation Start, SourceLocation End) {
return Start.isValid() && Start.isFileID() && End.isValid() &&
End.isFileID();
}

unsigned getHashValue() const;
void print(raw_ostream &OS, const SourceManager &SM) const;
std::string printToString(const SourceManager &SM) const;
void dump(const SourceManager &SM) const;
};

inline bool operator==(const SourceLocation &LHS, const SourceLocation &RHS) {
return LHS.getRawEncoding() == RHS.getRawEncoding();
}

inline bool operator!=(const SourceLocation &LHS, const SourceLocation &RHS) {
return !(LHS == RHS);
}



inline bool operator<(const SourceLocation &LHS, const SourceLocation &RHS) {
return LHS.getRawEncoding() < RHS.getRawEncoding();
}
inline bool operator>(const SourceLocation &LHS, const SourceLocation &RHS) {
return LHS.getRawEncoding() > RHS.getRawEncoding();
}
inline bool operator<=(const SourceLocation &LHS, const SourceLocation &RHS) {
return LHS.getRawEncoding() <= RHS.getRawEncoding();
}
inline bool operator>=(const SourceLocation &LHS, const SourceLocation &RHS) {
return LHS.getRawEncoding() >= RHS.getRawEncoding();
}


class SourceRange {
SourceLocation B;
SourceLocation E;

public:
SourceRange() = default;
SourceRange(SourceLocation loc) : B(loc), E(loc) {}
SourceRange(SourceLocation begin, SourceLocation end) : B(begin), E(end) {}

SourceLocation getBegin() const { return B; }
SourceLocation getEnd() const { return E; }

void setBegin(SourceLocation b) { B = b; }
void setEnd(SourceLocation e) { E = e; }

bool isValid() const { return B.isValid() && E.isValid(); }
bool isInvalid() const { return !isValid(); }

bool operator==(const SourceRange &X) const {
return B == X.B && E == X.E;
}

bool operator!=(const SourceRange &X) const {
return B != X.B || E != X.E;
}


bool fullyContains(const SourceRange &other) const {
return B <= other.B && E >= other.E;
}

void print(raw_ostream &OS, const SourceManager &SM) const;
std::string printToString(const SourceManager &SM) const;
void dump(const SourceManager &SM) const;
};








class CharSourceRange {
SourceRange Range;
bool IsTokenRange = false;

public:
CharSourceRange() = default;
CharSourceRange(SourceRange R, bool ITR) : Range(R), IsTokenRange(ITR) {}

static CharSourceRange getTokenRange(SourceRange R) {
return CharSourceRange(R, true);
}

static CharSourceRange getCharRange(SourceRange R) {
return CharSourceRange(R, false);
}

static CharSourceRange getTokenRange(SourceLocation B, SourceLocation E) {
return getTokenRange(SourceRange(B, E));
}

static CharSourceRange getCharRange(SourceLocation B, SourceLocation E) {
return getCharRange(SourceRange(B, E));
}




bool isTokenRange() const { return IsTokenRange; }
bool isCharRange() const { return !IsTokenRange; }

SourceLocation getBegin() const { return Range.getBegin(); }
SourceLocation getEnd() const { return Range.getEnd(); }
SourceRange getAsRange() const { return Range; }

void setBegin(SourceLocation b) { Range.setBegin(b); }
void setEnd(SourceLocation e) { Range.setEnd(e); }
void setTokenRange(bool TR) { IsTokenRange = TR; }

bool isValid() const { return Range.isValid(); }
bool isInvalid() const { return !isValid(); }
};








class PresumedLoc {
const char *Filename = nullptr;
FileID ID;
unsigned Line, Col;
SourceLocation IncludeLoc;

public:
PresumedLoc() = default;
PresumedLoc(const char *FN, FileID FID, unsigned Ln, unsigned Co,
SourceLocation IL)
: Filename(FN), ID(FID), Line(Ln), Col(Co), IncludeLoc(IL) {}





bool isInvalid() const { return Filename == nullptr; }
bool isValid() const { return Filename != nullptr; }




const char *getFilename() const {
assert(isValid());
return Filename;
}

FileID getFileID() const {
assert(isValid());
return ID;
}




unsigned getLine() const {
assert(isValid());
return Line;
}




unsigned getColumn() const {
assert(isValid());
return Col;
}




SourceLocation getIncludeLoc() const {
assert(isValid());
return IncludeLoc;
}
};

class FileEntry;




class FullSourceLoc : public SourceLocation {
const SourceManager *SrcMgr = nullptr;

public:

FullSourceLoc() = default;

explicit FullSourceLoc(SourceLocation Loc, const SourceManager &SM)
: SourceLocation(Loc), SrcMgr(&SM) {}

bool hasManager() const {
bool hasSrcMgr = SrcMgr != nullptr;
assert(hasSrcMgr == isValid() && "FullSourceLoc has location but no manager");
return hasSrcMgr;
}


const SourceManager &getManager() const {
assert(SrcMgr && "SourceManager is NULL.");
return *SrcMgr;
}

FileID getFileID() const;

FullSourceLoc getExpansionLoc() const;
FullSourceLoc getSpellingLoc() const;
FullSourceLoc getFileLoc() const;
PresumedLoc getPresumedLoc(bool UseLineDirectives = true) const;
bool isMacroArgExpansion(FullSourceLoc *StartLoc = nullptr) const;
FullSourceLoc getImmediateMacroCallerLoc() const;
std::pair<FullSourceLoc, StringRef> getModuleImportLoc() const;
unsigned getFileOffset() const;

unsigned getExpansionLineNumber(bool *Invalid = nullptr) const;
unsigned getExpansionColumnNumber(bool *Invalid = nullptr) const;

unsigned getSpellingLineNumber(bool *Invalid = nullptr) const;
unsigned getSpellingColumnNumber(bool *Invalid = nullptr) const;

const char *getCharacterData(bool *Invalid = nullptr) const;

unsigned getLineNumber(bool *Invalid = nullptr) const;
unsigned getColumnNumber(bool *Invalid = nullptr) const;

const FileEntry *getFileEntry() const;



StringRef getBufferData(bool *Invalid = nullptr) const;





std::pair<FileID, unsigned> getDecomposedLoc() const;

bool isInSystemHeader() const;




bool isBeforeInTranslationUnitThan(SourceLocation Loc) const;




bool isBeforeInTranslationUnitThan(FullSourceLoc Loc) const {
assert(Loc.isValid());
assert(SrcMgr == Loc.SrcMgr && "Loc comes from another SourceManager!");
return isBeforeInTranslationUnitThan((SourceLocation)Loc);
}


struct BeforeThanCompare {
bool operator()(const FullSourceLoc& lhs, const FullSourceLoc& rhs) const {
return lhs.isBeforeInTranslationUnitThan(rhs);
}
};




void dump() const;

friend bool
operator==(const FullSourceLoc &LHS, const FullSourceLoc &RHS) {
return LHS.getRawEncoding() == RHS.getRawEncoding() &&
LHS.SrcMgr == RHS.SrcMgr;
}

friend bool
operator!=(const FullSourceLoc &LHS, const FullSourceLoc &RHS) {
return !(LHS == RHS);
}
};

}

namespace llvm {



template <>
struct DenseMapInfo<clang::FileID> {
static clang::FileID getEmptyKey() {
return {};
}

static clang::FileID getTombstoneKey() {
return clang::FileID::getSentinel();
}

static unsigned getHashValue(clang::FileID S) {
return S.getHashValue();
}

static bool isEqual(clang::FileID LHS, clang::FileID RHS) {
return LHS == RHS;
}
};




template <> struct DenseMapInfo<clang::SourceLocation> {
static clang::SourceLocation getEmptyKey() {
constexpr clang::SourceLocation::UIntTy Zero = 0;
return clang::SourceLocation::getFromRawEncoding(~Zero);
}

static clang::SourceLocation getTombstoneKey() {
constexpr clang::SourceLocation::UIntTy Zero = 0;
return clang::SourceLocation::getFromRawEncoding(~Zero - 1);
}

static unsigned getHashValue(clang::SourceLocation Loc) {
return Loc.getHashValue();
}

static bool isEqual(clang::SourceLocation LHS, clang::SourceLocation RHS) {
return LHS == RHS;
}
};


template <> struct FoldingSetTrait<clang::SourceLocation> {
static void Profile(const clang::SourceLocation &X, FoldingSetNodeID &ID);
};

}

#endif
