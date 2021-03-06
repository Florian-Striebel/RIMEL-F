












#if !defined(LLVM_CLANG_BASIC_SOURCEMANAGERINTERNALS_H)
#define LLVM_CLANG_BASIC_SOURCEMANAGERINTERNALS_H

#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include <cassert>
#include <map>
#include <vector>

namespace clang {





struct LineEntry {

unsigned FileOffset;


unsigned LineNo;



int FilenameID;


SrcMgr::CharacteristicKind FileKind;





unsigned IncludeOffset;

static LineEntry get(unsigned Offs, unsigned Line, int Filename,
SrcMgr::CharacteristicKind FileKind,
unsigned IncludeOffset) {
LineEntry E;
E.FileOffset = Offs;
E.LineNo = Line;
E.FilenameID = Filename;
E.FileKind = FileKind;
E.IncludeOffset = IncludeOffset;
return E;
}
};


inline bool operator<(const LineEntry &lhs, const LineEntry &rhs) {

return lhs.FileOffset < rhs.FileOffset;
}

inline bool operator<(const LineEntry &E, unsigned Offset) {
return E.FileOffset < Offset;
}

inline bool operator<(unsigned Offset, const LineEntry &E) {
return Offset < E.FileOffset;
}


class LineTableInfo {






llvm::StringMap<unsigned, llvm::BumpPtrAllocator> FilenameIDs;
std::vector<llvm::StringMapEntry<unsigned>*> FilenamesByID;



std::map<FileID, std::vector<LineEntry>> LineEntries;

public:
void clear() {
FilenameIDs.clear();
FilenamesByID.clear();
LineEntries.clear();
}

unsigned getLineTableFilenameID(StringRef Str);

StringRef getFilename(unsigned ID) const {
assert(ID < FilenamesByID.size() && "Invalid FilenameID");
return FilenamesByID[ID]->getKey();
}

unsigned getNumFilenames() const { return FilenamesByID.size(); }

void AddLineNote(FileID FID, unsigned Offset,
unsigned LineNo, int FilenameID,
unsigned EntryExit, SrcMgr::CharacteristicKind FileKind);





const LineEntry *FindNearestLineEntry(FileID FID, unsigned Offset);


using iterator = std::map<FileID, std::vector<LineEntry>>::iterator;

iterator begin() { return LineEntries.begin(); }
iterator end() { return LineEntries.end(); }



void AddEntry(FileID FID, const std::vector<LineEntry> &Entries);
};

}

#endif
